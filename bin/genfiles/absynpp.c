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
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
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
struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,1,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 206
struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,0,1,0,0,0,0,0,0,0,0,1,0,0};
# 226
struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0};
# 247
static void Cyc_Absynpp_curr_namespace_add(struct _dyneither_ptr*v){
# 248
struct Cyc_List_List*_tmp60F;Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,((_tmp60F=_cycalloc(sizeof(*_tmp60F)),((_tmp60F->hd=v,((_tmp60F->tl=0,_tmp60F)))))));}
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
const char*_tmp610;return(_tmp610="\\a",_tag_dyneither(_tmp610,sizeof(char),3));}case '\b': _LL1: {
# 266
const char*_tmp611;return(_tmp611="\\b",_tag_dyneither(_tmp611,sizeof(char),3));}case '\f': _LL2: {
# 267
const char*_tmp612;return(_tmp612="\\f",_tag_dyneither(_tmp612,sizeof(char),3));}case '\n': _LL3: {
# 268
const char*_tmp613;return(_tmp613="\\n",_tag_dyneither(_tmp613,sizeof(char),3));}case '\r': _LL4: {
# 269
const char*_tmp614;return(_tmp614="\\r",_tag_dyneither(_tmp614,sizeof(char),3));}case '\t': _LL5: {
# 270
const char*_tmp615;return(_tmp615="\\t",_tag_dyneither(_tmp615,sizeof(char),3));}case '\v': _LL6: {
# 271
const char*_tmp616;return(_tmp616="\\v",_tag_dyneither(_tmp616,sizeof(char),3));}case '\\': _LL7: {
# 272
const char*_tmp617;return(_tmp617="\\\\",_tag_dyneither(_tmp617,sizeof(char),3));}case '"': _LL8: {
# 273
const char*_tmp618;return(_tmp618="\"",_tag_dyneither(_tmp618,sizeof(char),2));}case '\'': _LL9: {
# 274
const char*_tmp619;return(_tmp619="\\'",_tag_dyneither(_tmp619,sizeof(char),3));}default: _LLA:
# 276
 if(c >= ' '  && c <= '~'){
# 277
struct _dyneither_ptr _tmpC=Cyc_Core_new_string(2);
# 278
{char _tmp61C;char _tmp61B;struct _dyneither_ptr _tmp61A;(_tmp61A=_dyneither_ptr_plus(_tmpC,sizeof(char),0),((_tmp61B=*((char*)_check_dyneither_subscript(_tmp61A,sizeof(char),0)),((_tmp61C=c,((_get_dyneither_size(_tmp61A,sizeof(char))== 1  && (_tmp61B == '\000'  && _tmp61C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61A.curr)=_tmp61C)))))));}
# 279
return(struct _dyneither_ptr)_tmpC;}else{
# 281
struct _dyneither_ptr _tmp10=Cyc_Core_new_string(5);
# 282
int j=0;
# 283
{char _tmp61F;char _tmp61E;struct _dyneither_ptr _tmp61D;(_tmp61D=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp61E=*((char*)_check_dyneither_subscript(_tmp61D,sizeof(char),0)),((_tmp61F='\\',((_get_dyneither_size(_tmp61D,sizeof(char))== 1  && (_tmp61E == '\000'  && _tmp61F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61D.curr)=_tmp61F)))))));}
# 284
{char _tmp622;char _tmp621;struct _dyneither_ptr _tmp620;(_tmp620=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp621=*((char*)_check_dyneither_subscript(_tmp620,sizeof(char),0)),((_tmp622=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp620,sizeof(char))== 1  && (_tmp621 == '\000'  && _tmp622 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp620.curr)=_tmp622)))))));}
# 285
{char _tmp625;char _tmp624;struct _dyneither_ptr _tmp623;(_tmp623=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp624=*((char*)_check_dyneither_subscript(_tmp623,sizeof(char),0)),((_tmp625=(char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp623,sizeof(char))== 1  && (_tmp624 == '\000'  && _tmp625 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp623.curr)=_tmp625)))))));}
# 286
{char _tmp628;char _tmp627;struct _dyneither_ptr _tmp626;(_tmp626=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp627=*((char*)_check_dyneither_subscript(_tmp626,sizeof(char),0)),((_tmp628=(char)('0' + (c & 7)),((_get_dyneither_size(_tmp626,sizeof(char))== 1  && (_tmp627 == '\000'  && _tmp628 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp626.curr)=_tmp628)))))));}
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
{char _tmp62B;char _tmp62A;struct _dyneither_ptr _tmp629;(_tmp629=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp62A=*((char*)_check_dyneither_subscript(_tmp629,sizeof(char),0)),((_tmp62B='\\',((_get_dyneither_size(_tmp629,sizeof(char))== 1  && (_tmp62A == '\000'  && _tmp62B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp629.curr)=_tmp62B)))))));}{char _tmp62E;char _tmp62D;struct _dyneither_ptr _tmp62C;(_tmp62C=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp62D=*((char*)_check_dyneither_subscript(_tmp62C,sizeof(char),0)),((_tmp62E='a',((_get_dyneither_size(_tmp62C,sizeof(char))== 1  && (_tmp62D == '\000'  && _tmp62E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62C.curr)=_tmp62E)))))));}goto _LL21;_LL24: if(_tmp1E != '\b')goto _LL26;_LL25:
# 332
{char _tmp631;char _tmp630;struct _dyneither_ptr _tmp62F;(_tmp62F=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp630=*((char*)_check_dyneither_subscript(_tmp62F,sizeof(char),0)),((_tmp631='\\',((_get_dyneither_size(_tmp62F,sizeof(char))== 1  && (_tmp630 == '\000'  && _tmp631 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62F.curr)=_tmp631)))))));}{char _tmp634;char _tmp633;struct _dyneither_ptr _tmp632;(_tmp632=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp633=*((char*)_check_dyneither_subscript(_tmp632,sizeof(char),0)),((_tmp634='b',((_get_dyneither_size(_tmp632,sizeof(char))== 1  && (_tmp633 == '\000'  && _tmp634 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp632.curr)=_tmp634)))))));}goto _LL21;_LL26: if(_tmp1E != '\f')goto _LL28;_LL27:
# 333
{char _tmp637;char _tmp636;struct _dyneither_ptr _tmp635;(_tmp635=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp636=*((char*)_check_dyneither_subscript(_tmp635,sizeof(char),0)),((_tmp637='\\',((_get_dyneither_size(_tmp635,sizeof(char))== 1  && (_tmp636 == '\000'  && _tmp637 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp635.curr)=_tmp637)))))));}{char _tmp63A;char _tmp639;struct _dyneither_ptr _tmp638;(_tmp638=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp639=*((char*)_check_dyneither_subscript(_tmp638,sizeof(char),0)),((_tmp63A='f',((_get_dyneither_size(_tmp638,sizeof(char))== 1  && (_tmp639 == '\000'  && _tmp63A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp638.curr)=_tmp63A)))))));}goto _LL21;_LL28: if(_tmp1E != '\n')goto _LL2A;_LL29:
# 334
{char _tmp63D;char _tmp63C;struct _dyneither_ptr _tmp63B;(_tmp63B=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp63C=*((char*)_check_dyneither_subscript(_tmp63B,sizeof(char),0)),((_tmp63D='\\',((_get_dyneither_size(_tmp63B,sizeof(char))== 1  && (_tmp63C == '\000'  && _tmp63D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp63B.curr)=_tmp63D)))))));}{char _tmp640;char _tmp63F;struct _dyneither_ptr _tmp63E;(_tmp63E=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp63F=*((char*)_check_dyneither_subscript(_tmp63E,sizeof(char),0)),((_tmp640='n',((_get_dyneither_size(_tmp63E,sizeof(char))== 1  && (_tmp63F == '\000'  && _tmp640 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp63E.curr)=_tmp640)))))));}goto _LL21;_LL2A: if(_tmp1E != '\r')goto _LL2C;_LL2B:
# 335
{char _tmp643;char _tmp642;struct _dyneither_ptr _tmp641;(_tmp641=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp642=*((char*)_check_dyneither_subscript(_tmp641,sizeof(char),0)),((_tmp643='\\',((_get_dyneither_size(_tmp641,sizeof(char))== 1  && (_tmp642 == '\000'  && _tmp643 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp641.curr)=_tmp643)))))));}{char _tmp646;char _tmp645;struct _dyneither_ptr _tmp644;(_tmp644=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp645=*((char*)_check_dyneither_subscript(_tmp644,sizeof(char),0)),((_tmp646='r',((_get_dyneither_size(_tmp644,sizeof(char))== 1  && (_tmp645 == '\000'  && _tmp646 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp644.curr)=_tmp646)))))));}goto _LL21;_LL2C: if(_tmp1E != '\t')goto _LL2E;_LL2D:
# 336
{char _tmp649;char _tmp648;struct _dyneither_ptr _tmp647;(_tmp647=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp648=*((char*)_check_dyneither_subscript(_tmp647,sizeof(char),0)),((_tmp649='\\',((_get_dyneither_size(_tmp647,sizeof(char))== 1  && (_tmp648 == '\000'  && _tmp649 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp647.curr)=_tmp649)))))));}{char _tmp64C;char _tmp64B;struct _dyneither_ptr _tmp64A;(_tmp64A=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp64B=*((char*)_check_dyneither_subscript(_tmp64A,sizeof(char),0)),((_tmp64C='t',((_get_dyneither_size(_tmp64A,sizeof(char))== 1  && (_tmp64B == '\000'  && _tmp64C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp64A.curr)=_tmp64C)))))));}goto _LL21;_LL2E: if(_tmp1E != '\v')goto _LL30;_LL2F:
# 337
{char _tmp64F;char _tmp64E;struct _dyneither_ptr _tmp64D;(_tmp64D=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp64E=*((char*)_check_dyneither_subscript(_tmp64D,sizeof(char),0)),((_tmp64F='\\',((_get_dyneither_size(_tmp64D,sizeof(char))== 1  && (_tmp64E == '\000'  && _tmp64F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp64D.curr)=_tmp64F)))))));}{char _tmp652;char _tmp651;struct _dyneither_ptr _tmp650;(_tmp650=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp651=*((char*)_check_dyneither_subscript(_tmp650,sizeof(char),0)),((_tmp652='v',((_get_dyneither_size(_tmp650,sizeof(char))== 1  && (_tmp651 == '\000'  && _tmp652 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp650.curr)=_tmp652)))))));}goto _LL21;_LL30: if(_tmp1E != '\\')goto _LL32;_LL31:
# 338
{char _tmp655;char _tmp654;struct _dyneither_ptr _tmp653;(_tmp653=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp654=*((char*)_check_dyneither_subscript(_tmp653,sizeof(char),0)),((_tmp655='\\',((_get_dyneither_size(_tmp653,sizeof(char))== 1  && (_tmp654 == '\000'  && _tmp655 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp653.curr)=_tmp655)))))));}{char _tmp658;char _tmp657;struct _dyneither_ptr _tmp656;(_tmp656=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp657=*((char*)_check_dyneither_subscript(_tmp656,sizeof(char),0)),((_tmp658='\\',((_get_dyneither_size(_tmp656,sizeof(char))== 1  && (_tmp657 == '\000'  && _tmp658 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp656.curr)=_tmp658)))))));}goto _LL21;_LL32: if(_tmp1E != '"')goto _LL34;_LL33:
# 339
{char _tmp65B;char _tmp65A;struct _dyneither_ptr _tmp659;(_tmp659=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp65A=*((char*)_check_dyneither_subscript(_tmp659,sizeof(char),0)),((_tmp65B='\\',((_get_dyneither_size(_tmp659,sizeof(char))== 1  && (_tmp65A == '\000'  && _tmp65B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp659.curr)=_tmp65B)))))));}{char _tmp65E;char _tmp65D;struct _dyneither_ptr _tmp65C;(_tmp65C=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp65D=*((char*)_check_dyneither_subscript(_tmp65C,sizeof(char),0)),((_tmp65E='"',((_get_dyneither_size(_tmp65C,sizeof(char))== 1  && (_tmp65D == '\000'  && _tmp65E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp65C.curr)=_tmp65E)))))));}goto _LL21;_LL34:;_LL35:
# 341
 if(_tmp1E >= ' '  && _tmp1E <= '~'){char _tmp661;char _tmp660;struct _dyneither_ptr _tmp65F;(_tmp65F=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp660=*((char*)_check_dyneither_subscript(_tmp65F,sizeof(char),0)),((_tmp661=_tmp1E,((_get_dyneither_size(_tmp65F,sizeof(char))== 1  && (_tmp660 == '\000'  && _tmp661 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp65F.curr)=_tmp661)))))));}else{
# 343
{char _tmp664;char _tmp663;struct _dyneither_ptr _tmp662;(_tmp662=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp663=*((char*)_check_dyneither_subscript(_tmp662,sizeof(char),0)),((_tmp664='\\',((_get_dyneither_size(_tmp662,sizeof(char))== 1  && (_tmp663 == '\000'  && _tmp664 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp662.curr)=_tmp664)))))));}
# 344
{char _tmp667;char _tmp666;struct _dyneither_ptr _tmp665;(_tmp665=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp666=*((char*)_check_dyneither_subscript(_tmp665,sizeof(char),0)),((_tmp667=(char)('0' + (_tmp1E >> 6 & 7)),((_get_dyneither_size(_tmp665,sizeof(char))== 1  && (_tmp666 == '\000'  && _tmp667 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp665.curr)=_tmp667)))))));}
# 345
{char _tmp66A;char _tmp669;struct _dyneither_ptr _tmp668;(_tmp668=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp669=*((char*)_check_dyneither_subscript(_tmp668,sizeof(char),0)),((_tmp66A=(char)('0' + (_tmp1E >> 3 & 7)),((_get_dyneither_size(_tmp668,sizeof(char))== 1  && (_tmp669 == '\000'  && _tmp66A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp668.curr)=_tmp66A)))))));}{
# 346
char _tmp66D;char _tmp66C;struct _dyneither_ptr _tmp66B;(_tmp66B=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp66C=*((char*)_check_dyneither_subscript(_tmp66B,sizeof(char),0)),((_tmp66D=(char)('0' + (_tmp1E & 7)),((_get_dyneither_size(_tmp66B,sizeof(char))== 1  && (_tmp66C == '\000'  && _tmp66D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp66B.curr)=_tmp66D)))))));};}
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
if(tq.q_restrict){struct Cyc_List_List*_tmp66E;l=((_tmp66E=_cycalloc(sizeof(*_tmp66E)),((_tmp66E->hd=Cyc_Absynpp_restrict_sp,((_tmp66E->tl=l,_tmp66E))))));}
# 364
if(tq.q_volatile){struct Cyc_List_List*_tmp66F;l=((_tmp66F=_cycalloc(sizeof(*_tmp66F)),((_tmp66F->hd=Cyc_Absynpp_volatile_sp,((_tmp66F->tl=l,_tmp66F))))));}
# 365
if(tq.print_const){struct Cyc_List_List*_tmp670;l=((_tmp670=_cycalloc(sizeof(*_tmp670)),((_tmp670->hd=Cyc_Absynpp_const_sp,((_tmp670->tl=l,_tmp670))))));}{
# 366
const char*_tmp673;const char*_tmp672;const char*_tmp671;return Cyc_PP_egroup(((_tmp671="",_tag_dyneither(_tmp671,sizeof(char),1))),((_tmp672=" ",_tag_dyneither(_tmp672,sizeof(char),2))),((_tmp673=" ",_tag_dyneither(_tmp673,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));};}
# 369
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){
# 370
struct Cyc_Absyn_Kind _tmp6E;enum Cyc_Absyn_KindQual _tmp6F;enum Cyc_Absyn_AliasQual _tmp70;struct Cyc_Absyn_Kind*_tmp6D=ka;_tmp6E=*_tmp6D;_tmp6F=_tmp6E.kind;_tmp70=_tmp6E.aliasqual;
# 371
switch(_tmp6F){case Cyc_Absyn_AnyKind: _LL36:
# 373
 switch(_tmp70){case Cyc_Absyn_Aliasable: _LL38: {
# 374
const char*_tmp674;return(_tmp674="A",_tag_dyneither(_tmp674,sizeof(char),2));}case Cyc_Absyn_Unique: _LL39: {
# 375
const char*_tmp675;return(_tmp675="UA",_tag_dyneither(_tmp675,sizeof(char),3));}case Cyc_Absyn_Top: _LL3A: {
# 376
const char*_tmp676;return(_tmp676="TA",_tag_dyneither(_tmp676,sizeof(char),3));}}case Cyc_Absyn_MemKind: _LL37:
# 379
 switch(_tmp70){case Cyc_Absyn_Aliasable: _LL3D: {
# 380
const char*_tmp677;return(_tmp677="M",_tag_dyneither(_tmp677,sizeof(char),2));}case Cyc_Absyn_Unique: _LL3E: {
# 381
const char*_tmp678;return(_tmp678="UM",_tag_dyneither(_tmp678,sizeof(char),3));}case Cyc_Absyn_Top: _LL3F: {
# 382
const char*_tmp679;return(_tmp679="TM",_tag_dyneither(_tmp679,sizeof(char),3));}}case Cyc_Absyn_BoxKind: _LL3C:
# 385
 switch(_tmp70){case Cyc_Absyn_Aliasable: _LL42: {
# 386
const char*_tmp67A;return(_tmp67A="B",_tag_dyneither(_tmp67A,sizeof(char),2));}case Cyc_Absyn_Unique: _LL43: {
# 387
const char*_tmp67B;return(_tmp67B="UB",_tag_dyneither(_tmp67B,sizeof(char),3));}case Cyc_Absyn_Top: _LL44: {
# 388
const char*_tmp67C;return(_tmp67C="TB",_tag_dyneither(_tmp67C,sizeof(char),3));}}case Cyc_Absyn_RgnKind: _LL41:
# 391
 switch(_tmp70){case Cyc_Absyn_Aliasable: _LL47: {
# 392
const char*_tmp67D;return(_tmp67D="R",_tag_dyneither(_tmp67D,sizeof(char),2));}case Cyc_Absyn_Unique: _LL48: {
# 393
const char*_tmp67E;return(_tmp67E="UR",_tag_dyneither(_tmp67E,sizeof(char),3));}case Cyc_Absyn_Top: _LL49: {
# 394
const char*_tmp67F;return(_tmp67F="TR",_tag_dyneither(_tmp67F,sizeof(char),3));}}case Cyc_Absyn_EffKind: _LL46: {
# 396
const char*_tmp680;return(_tmp680="E",_tag_dyneither(_tmp680,sizeof(char),2));}case Cyc_Absyn_IntKind: _LL4B: {
# 397
const char*_tmp681;return(_tmp681="I",_tag_dyneither(_tmp681,sizeof(char),2));}}}
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
const char*_tmp682;return(_tmp682="{?}",_tag_dyneither(_tmp682,sizeof(char),4));}else{
# 408
const char*_tmp683;return(_tmp683="?",_tag_dyneither(_tmp683,sizeof(char),2));}_LL52: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp83=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7F;if(_tmp83->tag != 2)goto _LL4D;else{_tmp84=_tmp83->f2;}}_LL53: {
# 409
const char*_tmp687;void*_tmp686[1];struct Cyc_String_pa_PrintArg_struct _tmp685;return(struct _dyneither_ptr)((_tmp685.tag=0,((_tmp685.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp84)),((_tmp686[0]=& _tmp685,Cyc_aprintf(((_tmp687="<=%s",_tag_dyneither(_tmp687,sizeof(char),5))),_tag_dyneither(_tmp686,sizeof(void*),1))))))));}_LL4D:;}
# 413
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
# 414
void*_tmp8A=Cyc_Absyn_compress_kb(c);struct Cyc_Absyn_Kind*_tmp8C;struct Cyc_Absyn_Kind*_tmp8F;_LL55: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp8B=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8A;if(_tmp8B->tag != 0)goto _LL57;else{_tmp8C=_tmp8B->f1;}}_LL56:
# 415
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp8C));_LL57: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp8D=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8A;if(_tmp8D->tag != 1)goto _LL59;}_LL58:
# 417
 if(Cyc_PP_tex_output){
# 418
const char*_tmp688;return Cyc_PP_text_width(((_tmp688="{?}",_tag_dyneither(_tmp688,sizeof(char),4))),1);}else{
# 419
const char*_tmp689;return Cyc_PP_text(((_tmp689="?",_tag_dyneither(_tmp689,sizeof(char),2))));}_LL59: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8A;if(_tmp8E->tag != 2)goto _LL54;else{_tmp8F=_tmp8E->f2;}}_LL5A:
# 420
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp8F));_LL54:;}
# 424
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
# 425
const char*_tmp68C;const char*_tmp68B;const char*_tmp68A;return Cyc_PP_egroup(((_tmp68A="<",_tag_dyneither(_tmp68A,sizeof(char),2))),((_tmp68B=">",_tag_dyneither(_tmp68B,sizeof(char),2))),((_tmp68C=",",_tag_dyneither(_tmp68C,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));}
# 428
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
# 429
struct _dyneither_ptr _tmp95=*tv->name;
# 430
return Cyc_PP_text(_tmp95);}
# 433
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
# 434
void*_tmp96=Cyc_Absyn_compress_kb(tv->kind);struct Cyc_Absyn_Kind*_tmp99;struct Cyc_Absyn_Kind _tmp9A;enum Cyc_Absyn_KindQual _tmp9B;enum Cyc_Absyn_AliasQual _tmp9C;struct Cyc_Absyn_Kind*_tmp9E;struct Cyc_Absyn_Kind*_tmpA0;_LL5C: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp97=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp96;if(_tmp97->tag != 1)goto _LL5E;}_LL5D:
# 435
 goto _LL5F;_LL5E: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp98=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp96;if(_tmp98->tag != 0)goto _LL60;else{_tmp99=_tmp98->f1;_tmp9A=*_tmp99;_tmp9B=_tmp9A.kind;if(_tmp9B != Cyc_Absyn_BoxKind)goto _LL60;_tmp9C=_tmp9A.aliasqual;if(_tmp9C != Cyc_Absyn_Aliasable)goto _LL60;}}_LL5F:
# 436
 return Cyc_Absynpp_tvar2doc(tv);_LL60: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp96;if(_tmp9D->tag != 2)goto _LL62;else{_tmp9E=_tmp9D->f2;}}_LL61:
# 437
 _tmpA0=_tmp9E;goto _LL63;_LL62: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp9F=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp96;if(_tmp9F->tag != 0)goto _LL5B;else{_tmpA0=_tmp9F->f1;}}_LL63: {
# 438
const char*_tmp68F;struct Cyc_PP_Doc*_tmp68E[3];return(_tmp68E[2]=Cyc_Absynpp_kind2doc(_tmpA0),((_tmp68E[1]=Cyc_PP_text(((_tmp68F="::",_tag_dyneither(_tmp68F,sizeof(char),3)))),((_tmp68E[0]=Cyc_Absynpp_tvar2doc(tv),Cyc_PP_cat(_tag_dyneither(_tmp68E,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL5B:;}
# 442
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
# 443
const char*_tmp692;const char*_tmp691;const char*_tmp690;return Cyc_PP_egroup(((_tmp690="<",_tag_dyneither(_tmp690,sizeof(char),2))),((_tmp691=">",_tag_dyneither(_tmp691,sizeof(char),2))),((_tmp692=",",_tag_dyneither(_tmp692,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}
# 446
static struct _dyneither_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){
# 447
return tv->name;}
# 450
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
# 451
if(Cyc_Absynpp_print_all_kinds)
# 452
return Cyc_Absynpp_ktvars2doc(tvs);{
# 453
const char*_tmp695;const char*_tmp694;const char*_tmp693;return Cyc_PP_egroup(((_tmp693="<",_tag_dyneither(_tmp693,sizeof(char),2))),((_tmp694=">",_tag_dyneither(_tmp694,sizeof(char),2))),((_tmp695=",",_tag_dyneither(_tmp695,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,tvs)));};}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 456
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){
# 457
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 460
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
# 461
const char*_tmp698;const char*_tmp697;const char*_tmp696;return Cyc_PP_group(((_tmp696="(",_tag_dyneither(_tmp696,sizeof(char),2))),((_tmp697=")",_tag_dyneither(_tmp697,sizeof(char),2))),((_tmp698=",",_tag_dyneither(_tmp698,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}
# 464
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){
# 465
void*_tmpAC=att;_LL65: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpAD=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpAC;if(_tmpAD->tag != 1)goto _LL67;}_LL66:
# 466
 goto _LL68;_LL67: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpAE=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpAC;if(_tmpAE->tag != 2)goto _LL69;}_LL68:
# 467
 goto _LL6A;_LL69: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpAF=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpAC;if(_tmpAF->tag != 3)goto _LL6B;}_LL6A:
# 468
 return Cyc_PP_nil_doc();_LL6B:;_LL6C:
# 469
 return Cyc_PP_text(Cyc_Absyn_attribute2string(att));_LL64:;}
# 473
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
# 474
for(0;atts != 0;atts=atts->tl){
# 475
void*_tmpB0=(void*)atts->hd;_LL6E: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpB1=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpB0;if(_tmpB1->tag != 1)goto _LL70;}_LL6F: {
# 476
const char*_tmp699;return Cyc_PP_text(((_tmp699=" _stdcall ",_tag_dyneither(_tmp699,sizeof(char),11))));}_LL70: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpB2=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpB0;if(_tmpB2->tag != 2)goto _LL72;}_LL71: {
# 477
const char*_tmp69A;return Cyc_PP_text(((_tmp69A=" _cdecl ",_tag_dyneither(_tmp69A,sizeof(char),9))));}_LL72: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpB3=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpB0;if(_tmpB3->tag != 3)goto _LL74;}_LL73: {
# 478
const char*_tmp69B;return Cyc_PP_text(((_tmp69B=" _fastcall ",_tag_dyneither(_tmp69B,sizeof(char),12))));}_LL74:;_LL75:
# 479
 goto _LL6D;_LL6D:;}
# 481
return Cyc_PP_nil_doc();}
# 484
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){
# 485
int hasatt=0;
# 486
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
# 487
void*_tmpB7=(void*)atts2->hd;_LL77: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpB8=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpB7;if(_tmpB8->tag != 1)goto _LL79;}_LL78:
# 488
 goto _LL7A;_LL79: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpB9=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpB7;if(_tmpB9->tag != 2)goto _LL7B;}_LL7A:
# 489
 goto _LL7C;_LL7B: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpBA=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpB7;if(_tmpBA->tag != 3)goto _LL7D;}_LL7C:
# 490
 goto _LL76;_LL7D:;_LL7E:
# 491
 hasatt=1;goto _LL76;_LL76:;}}
# 493
if(!hasatt)
# 494
return Cyc_PP_nil_doc();{
# 495
const char*_tmp6A6;const char*_tmp6A5;const char*_tmp6A4;const char*_tmp6A3;const char*_tmp6A2;struct Cyc_PP_Doc*_tmp6A1[3];return(_tmp6A1[2]=
# 497
Cyc_PP_text(((_tmp6A2=")",_tag_dyneither(_tmp6A2,sizeof(char),2)))),((_tmp6A1[1]=
# 496
Cyc_PP_group(((_tmp6A5="",_tag_dyneither(_tmp6A5,sizeof(char),1))),((_tmp6A4="",_tag_dyneither(_tmp6A4,sizeof(char),1))),((_tmp6A3=" ",_tag_dyneither(_tmp6A3,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts)),((_tmp6A1[0]=
# 495
Cyc_PP_text(((_tmp6A6=" __declspec(",_tag_dyneither(_tmp6A6,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp6A1,sizeof(struct Cyc_PP_Doc*),3)))))));};}
# 500
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
# 501
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
# 504
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
# 505
if(atts == 0)return Cyc_PP_nil_doc();
# 506
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL7F:
# 507
 return Cyc_Absynpp_noncallconv2doc(atts);case Cyc_Cyclone_Gcc_c: _LL80: {
# 508
const char*_tmp6AF;const char*_tmp6AE;const char*_tmp6AD;const char*_tmp6AC;struct Cyc_PP_Doc*_tmp6AB[2];return(_tmp6AB[1]=
# 509
Cyc_PP_group(((_tmp6AE="((",_tag_dyneither(_tmp6AE,sizeof(char),3))),((_tmp6AD="))",_tag_dyneither(_tmp6AD,sizeof(char),3))),((_tmp6AC=",",_tag_dyneither(_tmp6AC,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts)),((_tmp6AB[0]=
# 508
Cyc_PP_text(((_tmp6AF=" __attribute__",_tag_dyneither(_tmp6AF,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmp6AB,sizeof(struct Cyc_PP_Doc*),2)))));}}}
# 513
int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
# 514
if(tms == 0)return 0;{
# 515
void*_tmpC6=(void*)tms->hd;_LL83: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpC7=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpC6;if(_tmpC7->tag != 2)goto _LL85;}_LL84:
# 516
 return 1;_LL85: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC8=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpC6;if(_tmpC8->tag != 5)goto _LL87;}_LL86:
# 518
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL89:
# 519
 return 0;default: _LL8A:
# 520
 return Cyc_Absynpp_next_is_pointer(tms->tl);}_LL87:;_LL88:
# 522
 return 0;_LL82:;};}
# 526
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
# 529
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;
# 530
static struct Cyc_PP_Doc*Cyc_Absynpp_question(){
# 531
if(!((unsigned int)Cyc_Absynpp_cache_question)){
# 532
if(Cyc_PP_tex_output){
# 533
const char*_tmp6B0;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp6B0="{?}",_tag_dyneither(_tmp6B0,sizeof(char),4))),1);}else{
# 534
const char*_tmp6B1;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp6B1="?",_tag_dyneither(_tmp6B1,sizeof(char),2))));}}
# 536
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
# 538
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
# 539
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
# 540
if(!((unsigned int)Cyc_Absynpp_cache_lb)){
# 541
if(Cyc_PP_tex_output){
# 542
const char*_tmp6B2;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp6B2="{\\lb}",_tag_dyneither(_tmp6B2,sizeof(char),6))),1);}else{
# 543
const char*_tmp6B3;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp6B3="{",_tag_dyneither(_tmp6B3,sizeof(char),2))));}}
# 545
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
# 547
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
# 548
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
# 549
if(!((unsigned int)Cyc_Absynpp_cache_rb)){
# 550
if(Cyc_PP_tex_output){
# 551
const char*_tmp6B4;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp6B4="{\\rb}",_tag_dyneither(_tmp6B4,sizeof(char),6))),1);}else{
# 552
const char*_tmp6B5;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp6B5="}",_tag_dyneither(_tmp6B5,sizeof(char),2))));}}
# 554
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
# 556
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
# 557
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
# 558
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
# 559
if(Cyc_PP_tex_output){
# 560
const char*_tmp6B6;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp6B6="\\$",_tag_dyneither(_tmp6B6,sizeof(char),3))),1);}else{
# 561
const char*_tmp6B7;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp6B7="$",_tag_dyneither(_tmp6B7,sizeof(char),2))));}}
# 563
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
# 565
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*ss){
# 566
struct Cyc_PP_Doc*_tmp6B8[3];return(_tmp6B8[2]=Cyc_Absynpp_rb(),((_tmp6B8[1]=Cyc_PP_seq(sep,ss),((_tmp6B8[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp6B8,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 570
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
# 571
for(0;tms != 0;tms=tms->tl){
# 572
void*_tmpD2=(void*)tms->hd;void*_tmpD6;struct Cyc_List_List*_tmpD8;_LL8D: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD3=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpD2;if(_tmpD3->tag != 0)goto _LL8F;}_LL8E:
# 573
{const char*_tmp6BB;void*_tmp6BA;(_tmp6BA=0,Cyc_fprintf(Cyc_stderr,((_tmp6BB="Carray_mod ",_tag_dyneither(_tmp6BB,sizeof(char),12))),_tag_dyneither(_tmp6BA,sizeof(void*),0)));}goto _LL8C;_LL8F: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD4=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpD2;if(_tmpD4->tag != 1)goto _LL91;}_LL90:
# 574
{const char*_tmp6BE;void*_tmp6BD;(_tmp6BD=0,Cyc_fprintf(Cyc_stderr,((_tmp6BE="ConstArray_mod ",_tag_dyneither(_tmp6BE,sizeof(char),16))),_tag_dyneither(_tmp6BD,sizeof(void*),0)));}goto _LL8C;_LL91: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD5=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD2;if(_tmpD5->tag != 3)goto _LL93;else{_tmpD6=(void*)_tmpD5->f1;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD7=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpD6;if(_tmpD7->tag != 1)goto _LL93;else{_tmpD8=_tmpD7->f1;}};}}_LL92:
# 576
{const char*_tmp6C1;void*_tmp6C0;(_tmp6C0=0,Cyc_fprintf(Cyc_stderr,((_tmp6C1="Function_mod(",_tag_dyneither(_tmp6C1,sizeof(char),14))),_tag_dyneither(_tmp6C0,sizeof(void*),0)));}
# 577
for(0;_tmpD8 != 0;_tmpD8=_tmpD8->tl){
# 578
struct _dyneither_ptr*_tmpE3=(*((struct _tuple8*)_tmpD8->hd)).f1;
# 579
if(_tmpE3 == 0){const char*_tmp6C4;void*_tmp6C3;(_tmp6C3=0,Cyc_fprintf(Cyc_stderr,((_tmp6C4="?",_tag_dyneither(_tmp6C4,sizeof(char),2))),_tag_dyneither(_tmp6C3,sizeof(void*),0)));}else{
# 580
void*_tmp6C5;(_tmp6C5=0,Cyc_fprintf(Cyc_stderr,*_tmpE3,_tag_dyneither(_tmp6C5,sizeof(void*),0)));}
# 581
if(_tmpD8->tl != 0){const char*_tmp6C8;void*_tmp6C7;(_tmp6C7=0,Cyc_fprintf(Cyc_stderr,((_tmp6C8=",",_tag_dyneither(_tmp6C8,sizeof(char),2))),_tag_dyneither(_tmp6C7,sizeof(void*),0)));}}
# 583
{const char*_tmp6CB;void*_tmp6CA;(_tmp6CA=0,Cyc_fprintf(Cyc_stderr,((_tmp6CB=") ",_tag_dyneither(_tmp6CB,sizeof(char),3))),_tag_dyneither(_tmp6CA,sizeof(void*),0)));}
# 584
goto _LL8C;_LL93: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD9=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD2;if(_tmpD9->tag != 3)goto _LL95;}_LL94:
# 586
{const char*_tmp6CE;void*_tmp6CD;(_tmp6CD=0,Cyc_fprintf(Cyc_stderr,((_tmp6CE="Function_mod()",_tag_dyneither(_tmp6CE,sizeof(char),15))),_tag_dyneither(_tmp6CD,sizeof(void*),0)));}goto _LL8C;_LL95: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpDA=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpD2;if(_tmpDA->tag != 5)goto _LL97;}_LL96:
# 587
{const char*_tmp6D1;void*_tmp6D0;(_tmp6D0=0,Cyc_fprintf(Cyc_stderr,((_tmp6D1="Attributes_mod ",_tag_dyneither(_tmp6D1,sizeof(char),16))),_tag_dyneither(_tmp6D0,sizeof(void*),0)));}goto _LL8C;_LL97: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpDB=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpD2;if(_tmpDB->tag != 4)goto _LL99;}_LL98:
# 588
{const char*_tmp6D4;void*_tmp6D3;(_tmp6D3=0,Cyc_fprintf(Cyc_stderr,((_tmp6D4="TypeParams_mod ",_tag_dyneither(_tmp6D4,sizeof(char),16))),_tag_dyneither(_tmp6D3,sizeof(void*),0)));}goto _LL8C;_LL99: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpDC=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpD2;if(_tmpDC->tag != 2)goto _LL8C;}_LL9A:
# 589
{const char*_tmp6D7;void*_tmp6D6;(_tmp6D6=0,Cyc_fprintf(Cyc_stderr,((_tmp6D7="Pointer_mod ",_tag_dyneither(_tmp6D7,sizeof(char),13))),_tag_dyneither(_tmp6D6,sizeof(void*),0)));}goto _LL8C;_LL8C:;}{
# 591
const char*_tmp6DA;void*_tmp6D9;(_tmp6D9=0,Cyc_fprintf(Cyc_stderr,((_tmp6DA="\n",_tag_dyneither(_tmp6DA,sizeof(char),2))),_tag_dyneither(_tmp6D9,sizeof(void*),0)));};}
# 594
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 596
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
# 597
if(tms == 0)return d;{
# 598
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
# 599
const char*_tmp6DF;const char*_tmp6DE;struct Cyc_PP_Doc*_tmp6DD[3];struct Cyc_PP_Doc*p_rest=(_tmp6DD[2]=Cyc_PP_text(((_tmp6DE=")",_tag_dyneither(_tmp6DE,sizeof(char),2)))),((_tmp6DD[1]=rest,((_tmp6DD[0]=Cyc_PP_text(((_tmp6DF="(",_tag_dyneither(_tmp6DF,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6DD,sizeof(struct Cyc_PP_Doc*),3)))))));
# 600
void*_tmpF5=(void*)tms->hd;union Cyc_Absyn_Constraint*_tmpF7;struct Cyc_Absyn_Exp*_tmpF9;union Cyc_Absyn_Constraint*_tmpFA;void*_tmpFC;struct Cyc_List_List*_tmpFE;struct Cyc_List_List*_tmp100;unsigned int _tmp101;int _tmp102;struct Cyc_Absyn_PtrAtts _tmp104;void*_tmp105;union Cyc_Absyn_Constraint*_tmp106;union Cyc_Absyn_Constraint*_tmp107;union Cyc_Absyn_Constraint*_tmp108;struct Cyc_Absyn_Tqual _tmp109;_LL9C: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpF6=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpF5;if(_tmpF6->tag != 0)goto _LL9E;else{_tmpF7=_tmpF6->f1;}}_LL9D:
# 602
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
# 603
const char*_tmp6E4;const char*_tmp6E3;struct Cyc_PP_Doc*_tmp6E2[2];return(_tmp6E2[1]=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF7)?
# 604
Cyc_PP_text(((_tmp6E3="[]@zeroterm",_tag_dyneither(_tmp6E3,sizeof(char),12)))): Cyc_PP_text(((_tmp6E4="[]",_tag_dyneither(_tmp6E4,sizeof(char),3)))),((_tmp6E2[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6E2,sizeof(struct Cyc_PP_Doc*),2)))));};_LL9E: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpF8=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpF5;if(_tmpF8->tag != 1)goto _LLA0;else{_tmpF9=_tmpF8->f1;_tmpFA=_tmpF8->f2;}}_LL9F:
# 606
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
# 607
const char*_tmp6EB;const char*_tmp6EA;const char*_tmp6E9;struct Cyc_PP_Doc*_tmp6E8[4];return(_tmp6E8[3]=
# 608
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpFA)?Cyc_PP_text(((_tmp6E9="]@zeroterm",_tag_dyneither(_tmp6E9,sizeof(char),11)))): Cyc_PP_text(((_tmp6EA="]",_tag_dyneither(_tmp6EA,sizeof(char),2)))),((_tmp6E8[2]=
# 607
Cyc_Absynpp_exp2doc(_tmpF9),((_tmp6E8[1]=Cyc_PP_text(((_tmp6EB="[",_tag_dyneither(_tmp6EB,sizeof(char),2)))),((_tmp6E8[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6E8,sizeof(struct Cyc_PP_Doc*),4)))))))));};_LLA0: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpFB=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpF5;if(_tmpFB->tag != 3)goto _LLA2;else{_tmpFC=(void*)_tmpFB->f1;}}_LLA1:
# 610
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
# 611
void*_tmp111=_tmpFC;struct Cyc_List_List*_tmp113;int _tmp114;struct Cyc_Absyn_VarargInfo*_tmp115;void*_tmp116;struct Cyc_List_List*_tmp117;struct Cyc_List_List*_tmp119;unsigned int _tmp11A;_LLA9: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp112=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp111;if(_tmp112->tag != 1)goto _LLAB;else{_tmp113=_tmp112->f1;_tmp114=_tmp112->f2;_tmp115=_tmp112->f3;_tmp116=(void*)_tmp112->f4;_tmp117=_tmp112->f5;}}_LLAA: {
# 613
struct Cyc_PP_Doc*_tmp6EC[2];return(_tmp6EC[1]=Cyc_Absynpp_funargs2doc(_tmp113,_tmp114,_tmp115,_tmp116,_tmp117),((_tmp6EC[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6EC,sizeof(struct Cyc_PP_Doc*),2)))));}_LLAB: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp118=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp111;if(_tmp118->tag != 0)goto _LLA8;else{_tmp119=_tmp118->f1;_tmp11A=_tmp118->f2;}}_LLAC: {
# 615
const char*_tmp6F3;const char*_tmp6F2;const char*_tmp6F1;struct Cyc_PP_Doc*_tmp6F0[2];return(_tmp6F0[1]=Cyc_PP_group(((_tmp6F3="(",_tag_dyneither(_tmp6F3,sizeof(char),2))),((_tmp6F2=")",_tag_dyneither(_tmp6F2,sizeof(char),2))),((_tmp6F1=",",_tag_dyneither(_tmp6F1,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp119)),((_tmp6F0[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6F0,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA8:;};_LLA2: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpFD=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpF5;if(_tmpFD->tag != 5)goto _LLA4;else{_tmpFE=_tmpFD->f2;}}_LLA3:
# 618
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LLAD:
# 620
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
# 621
struct Cyc_PP_Doc*_tmp6F4[2];return(_tmp6F4[1]=Cyc_Absynpp_atts2doc(_tmpFE),((_tmp6F4[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6F4,sizeof(struct Cyc_PP_Doc*),2)))));};case Cyc_Cyclone_Vc_c: _LLAE:
# 623
 if(Cyc_Absynpp_next_is_pointer(tms->tl)){
# 624
struct Cyc_PP_Doc*_tmp6F5[2];return(_tmp6F5[1]=rest,((_tmp6F5[0]=Cyc_Absynpp_callconv2doc(_tmpFE),Cyc_PP_cat(_tag_dyneither(_tmp6F5,sizeof(struct Cyc_PP_Doc*),2)))));}
# 625
return rest;}_LLA4: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpFF=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF5;if(_tmpFF->tag != 4)goto _LLA6;else{_tmp100=_tmpFF->f1;_tmp101=_tmpFF->f2;_tmp102=_tmpFF->f3;}}_LLA5:
# 628
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
# 629
if(_tmp102){
# 630
struct Cyc_PP_Doc*_tmp6F6[2];return(_tmp6F6[1]=Cyc_Absynpp_ktvars2doc(_tmp100),((_tmp6F6[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6F6,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 632
struct Cyc_PP_Doc*_tmp6F7[2];return(_tmp6F7[1]=Cyc_Absynpp_tvars2doc(_tmp100),((_tmp6F7[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6F7,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA6: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp103=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF5;if(_tmp103->tag != 2)goto _LL9B;else{_tmp104=_tmp103->f1;_tmp105=_tmp104.rgn;_tmp106=_tmp104.nullable;_tmp107=_tmp104.bounds;_tmp108=_tmp104.zero_term;_tmp109=_tmp103->f2;}}_LLA7: {
# 636
struct Cyc_PP_Doc*ptr;
# 637
{void*_tmp124=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp107);struct Cyc_Absyn_Exp*_tmp127;_LLB1: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp125=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp124;if(_tmp125->tag != 0)goto _LLB3;}_LLB2:
# 638
 ptr=Cyc_Absynpp_question();goto _LLB0;_LLB3: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp126=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp124;if(_tmp126->tag != 1)goto _LLB0;else{_tmp127=_tmp126->f1;}}_LLB4:
# 640
{const char*_tmp6F9;const char*_tmp6F8;ptr=Cyc_PP_text(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp106)?(_tmp6F9="*",_tag_dyneither(_tmp6F9,sizeof(char),2)):((_tmp6F8="@",_tag_dyneither(_tmp6F8,sizeof(char),2))));}{
# 641
unsigned int _tmp12B;int _tmp12C;struct _tuple11 _tmp12A=Cyc_Evexp_eval_const_uint_exp(_tmp127);_tmp12B=_tmp12A.f1;_tmp12C=_tmp12A.f2;
# 642
if(!_tmp12C  || _tmp12B != 1){
# 643
struct Cyc_PP_Doc*_tmp6FA[4];ptr=((_tmp6FA[3]=Cyc_Absynpp_rb(),((_tmp6FA[2]=Cyc_Absynpp_exp2doc(_tmp127),((_tmp6FA[1]=Cyc_Absynpp_lb(),((_tmp6FA[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6FA,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 644
goto _LLB0;};_LLB0:;}
# 646
if(Cyc_Absynpp_print_zeroterm){
# 647
if(!is_char_ptr  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp108)){
# 648
const char*_tmp6FD;struct Cyc_PP_Doc*_tmp6FC[2];ptr=((_tmp6FC[1]=Cyc_PP_text(((_tmp6FD="@zeroterm ",_tag_dyneither(_tmp6FD,sizeof(char),11)))),((_tmp6FC[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6FC,sizeof(struct Cyc_PP_Doc*),2))))));}else{
# 649
if(is_char_ptr  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp108)){
# 650
const char*_tmp700;struct Cyc_PP_Doc*_tmp6FF[2];ptr=((_tmp6FF[1]=Cyc_PP_text(((_tmp700="@nozeroterm ",_tag_dyneither(_tmp700,sizeof(char),13)))),((_tmp6FF[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6FF,sizeof(struct Cyc_PP_Doc*),2))))));}}}
# 652
{void*_tmp132=Cyc_Tcutil_compress(_tmp105);_LLB6: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp133=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp132;if(_tmp133->tag != 20)goto _LLB8;}_LLB7:
# 653
 goto _LLB5;_LLB8:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp134=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp132;if(_tmp134->tag != 1)goto _LLBA;}if(!Cyc_Absynpp_print_for_cycdoc)goto _LLBA;_LLB9:
# 654
 goto _LLB5;_LLBA:;_LLBB: {
# 655
const char*_tmp703;struct Cyc_PP_Doc*_tmp702[3];ptr=((_tmp702[2]=Cyc_PP_text(((_tmp703=" ",_tag_dyneither(_tmp703,sizeof(char),2)))),((_tmp702[1]=Cyc_Absynpp_rgn2doc(_tmp105),((_tmp702[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp702,sizeof(struct Cyc_PP_Doc*),3))))))));}_LLB5:;}
# 657
{struct Cyc_PP_Doc*_tmp704[2];ptr=((_tmp704[1]=Cyc_Absynpp_tqual2doc(_tmp109),((_tmp704[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp704,sizeof(struct Cyc_PP_Doc*),2))))));}{
# 658
struct Cyc_PP_Doc*_tmp705[2];return(_tmp705[1]=rest,((_tmp705[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp705,sizeof(struct Cyc_PP_Doc*),2)))));};}_LL9B:;};}
# 662
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
# 663
void*_tmp13C=Cyc_Tcutil_compress(t);_LLBD: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp13D=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp13C;if(_tmp13D->tag != 20)goto _LLBF;}_LLBE: {
# 664
const char*_tmp706;return Cyc_PP_text(((_tmp706="`H",_tag_dyneither(_tmp706,sizeof(char),3))));}_LLBF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp13E=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp13C;if(_tmp13E->tag != 21)goto _LLC1;}_LLC0: {
# 665
const char*_tmp707;return Cyc_PP_text(((_tmp707="`U",_tag_dyneither(_tmp707,sizeof(char),3))));}_LLC1: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp13F=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp13C;if(_tmp13F->tag != 22)goto _LLC3;}_LLC2: {
# 666
const char*_tmp708;return Cyc_PP_text(((_tmp708="`RC",_tag_dyneither(_tmp708,sizeof(char),4))));}_LLC3: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp140=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp13C;if(_tmp140->tag != 2)goto _LLC5;}_LLC4:
# 667
 return Cyc_Absynpp_ntyp2doc(t);_LLC5:;_LLC6: {
# 668
const char*_tmp70D;const char*_tmp70C;struct Cyc_PP_Doc*_tmp70B[3];return(_tmp70B[2]=Cyc_PP_text(((_tmp70C=")",_tag_dyneither(_tmp70C,sizeof(char),2)))),((_tmp70B[1]=Cyc_Absynpp_ntyp2doc(t),((_tmp70B[0]=Cyc_PP_text(((_tmp70D="@region(",_tag_dyneither(_tmp70D,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp70B,sizeof(struct Cyc_PP_Doc*),3)))))));}_LLBC:;}
# 672
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 676
void*_tmp147=Cyc_Tcutil_compress(t);void*_tmp149;struct Cyc_List_List*_tmp14B;_LLC8: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp148=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp147;if(_tmp148->tag != 23)goto _LLCA;else{_tmp149=(void*)_tmp148->f1;}}_LLC9:
# 677
{struct Cyc_List_List*_tmp70E;*rgions=((_tmp70E=_cycalloc(sizeof(*_tmp70E)),((_tmp70E->hd=Cyc_Absynpp_rgn2doc(_tmp149),((_tmp70E->tl=*rgions,_tmp70E))))));}goto _LLC7;_LLCA: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp14A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp147;if(_tmp14A->tag != 24)goto _LLCC;else{_tmp14B=_tmp14A->f1;}}_LLCB:
# 679
 for(0;_tmp14B != 0;_tmp14B=_tmp14B->tl){
# 680
Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp14B->hd);}
# 682
goto _LLC7;_LLCC:;_LLCD:
# 683
{struct Cyc_List_List*_tmp70F;*effects=((_tmp70F=_cycalloc(sizeof(*_tmp70F)),((_tmp70F->hd=Cyc_Absynpp_typ2doc(t),((_tmp70F->tl=*effects,_tmp70F))))));}goto _LLC7;_LLC7:;}
# 687
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
# 688
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
# 689
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
# 690
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
# 691
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
# 692
if(rgions == 0  && effects != 0){
# 693
const char*_tmp712;const char*_tmp711;const char*_tmp710;return Cyc_PP_group(((_tmp710="",_tag_dyneither(_tmp710,sizeof(char),1))),((_tmp711="",_tag_dyneither(_tmp711,sizeof(char),1))),((_tmp712="+",_tag_dyneither(_tmp712,sizeof(char),2))),effects);}else{
# 695
const char*_tmp713;struct Cyc_PP_Doc*_tmp151=Cyc_Absynpp_group_braces(((_tmp713=",",_tag_dyneither(_tmp713,sizeof(char),2))),rgions);
# 696
struct Cyc_List_List*_tmp717;const char*_tmp716;const char*_tmp715;const char*_tmp714;return Cyc_PP_group(((_tmp714="",_tag_dyneither(_tmp714,sizeof(char),1))),((_tmp715="",_tag_dyneither(_tmp715,sizeof(char),1))),((_tmp716="+",_tag_dyneither(_tmp716,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((_tmp717=_cycalloc(sizeof(*_tmp717)),((_tmp717->hd=_tmp151,((_tmp717->tl=0,_tmp717))))))));}}
# 700
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
# 701
switch(k){case Cyc_Absyn_StructA: _LLCE: {
# 702
const char*_tmp718;return Cyc_PP_text(((_tmp718="struct ",_tag_dyneither(_tmp718,sizeof(char),8))));}case Cyc_Absyn_UnionA: _LLCF: {
# 703
const char*_tmp719;return Cyc_PP_text(((_tmp719="union ",_tag_dyneither(_tmp719,sizeof(char),7))));}}}
# 708
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
# 709
struct Cyc_PP_Doc*s;
# 710
{void*_tmp159=t;struct Cyc_Core_Opt*_tmp15F;void*_tmp160;int _tmp161;struct Cyc_Core_Opt*_tmp162;struct Cyc_Absyn_Tvar*_tmp164;struct Cyc_Absyn_DatatypeInfo _tmp166;union Cyc_Absyn_DatatypeInfoU _tmp167;struct Cyc_List_List*_tmp168;struct Cyc_Absyn_DatatypeFieldInfo _tmp16A;union Cyc_Absyn_DatatypeFieldInfoU _tmp16B;struct Cyc_List_List*_tmp16C;enum Cyc_Absyn_Sign _tmp16E;enum Cyc_Absyn_Size_of _tmp16F;int _tmp171;struct Cyc_List_List*_tmp173;struct Cyc_Absyn_AggrInfo _tmp175;union Cyc_Absyn_AggrInfoU _tmp176;struct Cyc_List_List*_tmp177;enum Cyc_Absyn_AggrKind _tmp179;struct Cyc_List_List*_tmp17A;struct Cyc_List_List*_tmp17C;struct _tuple0*_tmp17E;struct Cyc_Absyn_Exp*_tmp180;struct _tuple0*_tmp182;struct Cyc_List_List*_tmp183;struct Cyc_Absyn_Typedefdecl*_tmp184;void*_tmp186;void*_tmp188;void*_tmp189;void*_tmp18B;void*_tmp190;struct Cyc_Absyn_TypeDecl*_tmp194;struct Cyc_Absyn_TypeDecl _tmp195;void*_tmp196;struct Cyc_Absyn_Aggrdecl*_tmp198;struct Cyc_Absyn_TypeDecl*_tmp19A;struct Cyc_Absyn_TypeDecl _tmp19B;void*_tmp19C;struct Cyc_Absyn_Enumdecl*_tmp19E;struct Cyc_Absyn_TypeDecl*_tmp1A0;struct Cyc_Absyn_TypeDecl _tmp1A1;void*_tmp1A2;struct Cyc_Absyn_Datatypedecl*_tmp1A4;_LLD2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp15A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp159;if(_tmp15A->tag != 8)goto _LLD4;}_LLD3: {
# 712
const char*_tmp71A;return Cyc_PP_text(((_tmp71A="[[[array]]]",_tag_dyneither(_tmp71A,sizeof(char),12))));}_LLD4: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp15B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp159;if(_tmp15B->tag != 9)goto _LLD6;}_LLD5:
# 713
 return Cyc_PP_nil_doc();_LLD6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp15C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp159;if(_tmp15C->tag != 5)goto _LLD8;}_LLD7:
# 714
 return Cyc_PP_nil_doc();_LLD8: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp15D=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp159;if(_tmp15D->tag != 0)goto _LLDA;}_LLD9:
# 716
{const char*_tmp71B;s=Cyc_PP_text(((_tmp71B="void",_tag_dyneither(_tmp71B,sizeof(char),5))));}goto _LLD1;_LLDA: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp15E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp159;if(_tmp15E->tag != 1)goto _LLDC;else{_tmp15F=_tmp15E->f1;_tmp160=(void*)_tmp15E->f2;_tmp161=_tmp15E->f3;_tmp162=_tmp15E->f4;}}_LLDB:
# 718
 if(_tmp160 != 0)
# 720
return Cyc_Absynpp_ntyp2doc((void*)_tmp160);else{
# 728
const char*_tmp72B;const char*_tmp72A;const char*_tmp729;struct Cyc_Int_pa_PrintArg_struct _tmp728;void*_tmp727[1];const char*_tmp726;const char*_tmp725;struct Cyc_PP_Doc*_tmp724[6];s=((_tmp724[5]=
# 733
_tmp15F == 0?Cyc_Absynpp_question(): Cyc_Absynpp_kind2doc((struct Cyc_Absyn_Kind*)_tmp15F->v),((_tmp724[4]=Cyc_PP_text(((_tmp725=")::",_tag_dyneither(_tmp725,sizeof(char),4)))),((_tmp724[3]=
# 731
(!Cyc_Absynpp_print_full_evars  || _tmp162 == 0)?
# 732
Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)_tmp162->v),((_tmp724[2]=
# 730
Cyc_PP_text((struct _dyneither_ptr)((_tmp728.tag=1,((_tmp728.f1=(unsigned long)_tmp161,((_tmp727[0]=& _tmp728,Cyc_aprintf(((_tmp726="%d",_tag_dyneither(_tmp726,sizeof(char),3))),_tag_dyneither(_tmp727,sizeof(void*),1))))))))),((_tmp724[1]=
# 729
Cyc_PP_text(((_tmp729="(",_tag_dyneither(_tmp729,sizeof(char),2)))),((_tmp724[0]=
# 728
Cyc_Absynpp_print_for_cycdoc?Cyc_PP_text(((_tmp72A="\\%",_tag_dyneither(_tmp72A,sizeof(char),3)))): Cyc_PP_text(((_tmp72B="%",_tag_dyneither(_tmp72B,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp724,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
# 736
goto _LLD1;_LLDC: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp163=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp159;if(_tmp163->tag != 2)goto _LLDE;else{_tmp164=_tmp163->f1;}}_LLDD:
# 738
 s=Cyc_PP_textptr(_tmp164->name);
# 739
if(Cyc_Absynpp_print_all_kinds){
# 740
const char*_tmp72E;struct Cyc_PP_Doc*_tmp72D[3];s=((_tmp72D[2]=Cyc_Absynpp_kindbound2doc(_tmp164->kind),((_tmp72D[1]=Cyc_PP_text(((_tmp72E="::",_tag_dyneither(_tmp72E,sizeof(char),3)))),((_tmp72D[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp72D,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 744
if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(_tmp164)){
# 747
const char*_tmp733;const char*_tmp732;struct Cyc_PP_Doc*_tmp731[3];s=((_tmp731[2]=Cyc_PP_text(((_tmp732=" */",_tag_dyneither(_tmp732,sizeof(char),4)))),((_tmp731[1]=s,((_tmp731[0]=Cyc_PP_text(((_tmp733="_ /* ",_tag_dyneither(_tmp733,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp731,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 750
goto _LLD1;_LLDE: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp165=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp159;if(_tmp165->tag != 3)goto _LLE0;else{_tmp166=_tmp165->f1;_tmp167=_tmp166.datatype_info;_tmp168=_tmp166.targs;}}_LLDF:
# 752
{union Cyc_Absyn_DatatypeInfoU _tmp1B4=_tmp167;struct Cyc_Absyn_UnknownDatatypeInfo _tmp1B5;struct _tuple0*_tmp1B6;int _tmp1B7;struct Cyc_Absyn_Datatypedecl**_tmp1B8;struct Cyc_Absyn_Datatypedecl*_tmp1B9;struct Cyc_Absyn_Datatypedecl _tmp1BA;struct _tuple0*_tmp1BB;int _tmp1BC;_LL10D: if((_tmp1B4.UnknownDatatype).tag != 1)goto _LL10F;_tmp1B5=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1B4.UnknownDatatype).val;_tmp1B6=_tmp1B5.name;_tmp1B7=_tmp1B5.is_extensible;_LL10E:
# 754
 _tmp1BB=_tmp1B6;_tmp1BC=_tmp1B7;goto _LL110;_LL10F: if((_tmp1B4.KnownDatatype).tag != 2)goto _LL10C;_tmp1B8=(struct Cyc_Absyn_Datatypedecl**)(_tmp1B4.KnownDatatype).val;_tmp1B9=*_tmp1B8;_tmp1BA=*_tmp1B9;_tmp1BB=_tmp1BA.name;_tmp1BC=_tmp1BA.is_extensible;_LL110: {
# 756
const char*_tmp734;struct Cyc_PP_Doc*_tmp1BD=Cyc_PP_text(((_tmp734="datatype ",_tag_dyneither(_tmp734,sizeof(char),10))));
# 757
const char*_tmp735;struct Cyc_PP_Doc*_tmp1BE=_tmp1BC?Cyc_PP_text(((_tmp735="@extensible ",_tag_dyneither(_tmp735,sizeof(char),13)))): Cyc_PP_nil_doc();
# 758
{struct Cyc_PP_Doc*_tmp736[4];s=((_tmp736[3]=Cyc_Absynpp_tps2doc(_tmp168),((_tmp736[2]=Cyc_Absynpp_qvar2doc(_tmp1BB),((_tmp736[1]=_tmp1BD,((_tmp736[0]=_tmp1BE,Cyc_PP_cat(_tag_dyneither(_tmp736,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 759
goto _LL10C;}_LL10C:;}
# 761
goto _LLD1;_LLE0: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp169=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp159;if(_tmp169->tag != 4)goto _LLE2;else{_tmp16A=_tmp169->f1;_tmp16B=_tmp16A.field_info;_tmp16C=_tmp16A.targs;}}_LLE1:
# 763
{union Cyc_Absyn_DatatypeFieldInfoU _tmp1C2=_tmp16B;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp1C3;struct _tuple0*_tmp1C4;struct _tuple0*_tmp1C5;int _tmp1C6;struct _tuple1 _tmp1C7;struct Cyc_Absyn_Datatypedecl*_tmp1C8;struct Cyc_Absyn_Datatypedecl _tmp1C9;struct _tuple0*_tmp1CA;int _tmp1CB;struct Cyc_Absyn_Datatypefield*_tmp1CC;struct Cyc_Absyn_Datatypefield _tmp1CD;struct _tuple0*_tmp1CE;_LL112: if((_tmp1C2.UnknownDatatypefield).tag != 1)goto _LL114;_tmp1C3=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1C2.UnknownDatatypefield).val;_tmp1C4=_tmp1C3.datatype_name;_tmp1C5=_tmp1C3.field_name;_tmp1C6=_tmp1C3.is_extensible;_LL113:
# 765
 _tmp1CA=_tmp1C4;_tmp1CB=_tmp1C6;_tmp1CE=_tmp1C5;goto _LL115;_LL114: if((_tmp1C2.KnownDatatypefield).tag != 2)goto _LL111;_tmp1C7=(struct _tuple1)(_tmp1C2.KnownDatatypefield).val;_tmp1C8=_tmp1C7.f1;_tmp1C9=*_tmp1C8;_tmp1CA=_tmp1C9.name;_tmp1CB=_tmp1C9.is_extensible;_tmp1CC=_tmp1C7.f2;_tmp1CD=*_tmp1CC;_tmp1CE=_tmp1CD.name;_LL115: {
# 768
const char*_tmp738;const char*_tmp737;struct Cyc_PP_Doc*_tmp1CF=Cyc_PP_text(_tmp1CB?(_tmp738="@extensible datatype ",_tag_dyneither(_tmp738,sizeof(char),22)):((_tmp737="datatype ",_tag_dyneither(_tmp737,sizeof(char),10))));
# 769
{const char*_tmp73B;struct Cyc_PP_Doc*_tmp73A[4];s=((_tmp73A[3]=Cyc_Absynpp_qvar2doc(_tmp1CE),((_tmp73A[2]=Cyc_PP_text(((_tmp73B=".",_tag_dyneither(_tmp73B,sizeof(char),2)))),((_tmp73A[1]=Cyc_Absynpp_qvar2doc(_tmp1CA),((_tmp73A[0]=_tmp1CF,Cyc_PP_cat(_tag_dyneither(_tmp73A,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 770
goto _LL111;}_LL111:;}
# 772
goto _LLD1;_LLE2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp16D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp159;if(_tmp16D->tag != 6)goto _LLE4;else{_tmp16E=_tmp16D->f1;_tmp16F=_tmp16D->f2;}}_LLE3: {
# 774
struct _dyneither_ptr sns;
# 775
struct _dyneither_ptr ts;
# 776
switch(_tmp16E){case Cyc_Absyn_None: _LL116:
# 777
 goto _LL117;case Cyc_Absyn_Signed: _LL117:
# 778
{const char*_tmp73C;sns=((_tmp73C="",_tag_dyneither(_tmp73C,sizeof(char),1)));}break;case Cyc_Absyn_Unsigned: _LL118:
# 779
{const char*_tmp73D;sns=((_tmp73D="unsigned ",_tag_dyneither(_tmp73D,sizeof(char),10)));}break;}
# 781
switch(_tmp16F){case Cyc_Absyn_Char_sz: _LL11A:
# 783
 switch(_tmp16E){case Cyc_Absyn_None: _LL11C:
# 784
{const char*_tmp73E;sns=((_tmp73E="",_tag_dyneither(_tmp73E,sizeof(char),1)));}break;case Cyc_Absyn_Signed: _LL11D:
# 785
{const char*_tmp73F;sns=((_tmp73F="signed ",_tag_dyneither(_tmp73F,sizeof(char),8)));}break;case Cyc_Absyn_Unsigned: _LL11E:
# 786
{const char*_tmp740;sns=((_tmp740="unsigned ",_tag_dyneither(_tmp740,sizeof(char),10)));}break;}
# 788
{const char*_tmp741;ts=((_tmp741="char",_tag_dyneither(_tmp741,sizeof(char),5)));}
# 789
break;case Cyc_Absyn_Short_sz: _LL11B:
# 790
{const char*_tmp742;ts=((_tmp742="short",_tag_dyneither(_tmp742,sizeof(char),6)));}break;case Cyc_Absyn_Int_sz: _LL120:
# 791
{const char*_tmp743;ts=((_tmp743="int",_tag_dyneither(_tmp743,sizeof(char),4)));}break;case Cyc_Absyn_Long_sz: _LL121:
# 792
{const char*_tmp744;ts=((_tmp744="long",_tag_dyneither(_tmp744,sizeof(char),5)));}break;case Cyc_Absyn_LongLong_sz: _LL122:
# 794
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL124:
# 795
{const char*_tmp745;ts=((_tmp745="long long",_tag_dyneither(_tmp745,sizeof(char),10)));}break;case Cyc_Cyclone_Vc_c: _LL125:
# 796
{const char*_tmp746;ts=((_tmp746="__int64",_tag_dyneither(_tmp746,sizeof(char),8)));}break;}
# 798
break;}
# 800
{const char*_tmp74B;void*_tmp74A[2];struct Cyc_String_pa_PrintArg_struct _tmp749;struct Cyc_String_pa_PrintArg_struct _tmp748;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp748.tag=0,((_tmp748.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ts),((_tmp749.tag=0,((_tmp749.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sns),((_tmp74A[0]=& _tmp749,((_tmp74A[1]=& _tmp748,Cyc_aprintf(((_tmp74B="%s%s",_tag_dyneither(_tmp74B,sizeof(char),5))),_tag_dyneither(_tmp74A,sizeof(void*),2)))))))))))))));}
# 801
goto _LLD1;}_LLE4: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp170=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp159;if(_tmp170->tag != 7)goto _LLE6;else{_tmp171=_tmp170->f1;}}_LLE5:
# 803
 if(_tmp171 == 0){
# 804
const char*_tmp74C;s=Cyc_PP_text(((_tmp74C="float",_tag_dyneither(_tmp74C,sizeof(char),6))));}else{
# 805
if(_tmp171 == 1){
# 806
const char*_tmp74D;s=Cyc_PP_text(((_tmp74D="double",_tag_dyneither(_tmp74D,sizeof(char),7))));}else{
# 808
const char*_tmp74E;s=Cyc_PP_text(((_tmp74E="long double",_tag_dyneither(_tmp74E,sizeof(char),12))));}}
# 809
goto _LLD1;_LLE6: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp172=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp159;if(_tmp172->tag != 10)goto _LLE8;else{_tmp173=_tmp172->f1;}}_LLE7:
# 811
{struct Cyc_PP_Doc*_tmp74F[2];s=((_tmp74F[1]=Cyc_Absynpp_args2doc(_tmp173),((_tmp74F[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp74F,sizeof(struct Cyc_PP_Doc*),2))))));}
# 812
goto _LLD1;_LLE8: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp174=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp159;if(_tmp174->tag != 11)goto _LLEA;else{_tmp175=_tmp174->f1;_tmp176=_tmp175.aggr_info;_tmp177=_tmp175.targs;}}_LLE9: {
# 814
enum Cyc_Absyn_AggrKind _tmp1E8;struct _tuple0*_tmp1E9;struct _tuple10 _tmp1E7=Cyc_Absyn_aggr_kinded_name(_tmp176);_tmp1E8=_tmp1E7.f1;_tmp1E9=_tmp1E7.f2;
# 815
{struct Cyc_PP_Doc*_tmp750[3];s=((_tmp750[2]=Cyc_Absynpp_tps2doc(_tmp177),((_tmp750[1]=Cyc_Absynpp_qvar2doc(_tmp1E9),((_tmp750[0]=Cyc_Absynpp_aggr_kind2doc(_tmp1E8),Cyc_PP_cat(_tag_dyneither(_tmp750,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 816
goto _LLD1;}_LLEA: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp178=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp159;if(_tmp178->tag != 12)goto _LLEC;else{_tmp179=_tmp178->f1;_tmp17A=_tmp178->f2;}}_LLEB:
# 818
{struct Cyc_PP_Doc*_tmp751[4];s=((_tmp751[3]=
# 820
Cyc_Absynpp_rb(),((_tmp751[2]=
# 819
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp17A)),((_tmp751[1]=
# 818
Cyc_Absynpp_lb(),((_tmp751[0]=Cyc_Absynpp_aggr_kind2doc(_tmp179),Cyc_PP_cat(_tag_dyneither(_tmp751,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 821
goto _LLD1;_LLEC: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp17B=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp159;if(_tmp17B->tag != 14)goto _LLEE;else{_tmp17C=_tmp17B->f1;}}_LLED:
# 823
{const char*_tmp754;struct Cyc_PP_Doc*_tmp753[4];s=((_tmp753[3]=Cyc_Absynpp_rb(),((_tmp753[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp17C)),((_tmp753[1]=Cyc_Absynpp_lb(),((_tmp753[0]=Cyc_PP_text(((_tmp754="enum ",_tag_dyneither(_tmp754,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp753,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLD1;_LLEE: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp17D=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp159;if(_tmp17D->tag != 13)goto _LLF0;else{_tmp17E=_tmp17D->f1;}}_LLEF:
# 825
{const char*_tmp757;struct Cyc_PP_Doc*_tmp756[2];s=((_tmp756[1]=Cyc_Absynpp_qvar2doc(_tmp17E),((_tmp756[0]=Cyc_PP_text(((_tmp757="enum ",_tag_dyneither(_tmp757,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp756,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLD1;_LLF0: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp17F=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp159;if(_tmp17F->tag != 18)goto _LLF2;else{_tmp180=_tmp17F->f1;}}_LLF1:
# 827
{const char*_tmp75C;const char*_tmp75B;struct Cyc_PP_Doc*_tmp75A[3];s=((_tmp75A[2]=Cyc_PP_text(((_tmp75B=")",_tag_dyneither(_tmp75B,sizeof(char),2)))),((_tmp75A[1]=Cyc_Absynpp_exp2doc(_tmp180),((_tmp75A[0]=Cyc_PP_text(((_tmp75C="valueof_t(",_tag_dyneither(_tmp75C,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp75A,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLF2: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp181=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp159;if(_tmp181->tag != 17)goto _LLF4;else{_tmp182=_tmp181->f1;_tmp183=_tmp181->f2;_tmp184=_tmp181->f3;}}_LLF3:
# 834
{struct Cyc_PP_Doc*_tmp75D[2];s=((_tmp75D[1]=Cyc_Absynpp_tps2doc(_tmp183),((_tmp75D[0]=Cyc_Absynpp_qvar2doc(_tmp182),Cyc_PP_cat(_tag_dyneither(_tmp75D,sizeof(struct Cyc_PP_Doc*),2))))));}
# 835
goto _LLD1;_LLF4: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp185=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp159;if(_tmp185->tag != 15)goto _LLF6;else{_tmp186=(void*)_tmp185->f1;}}_LLF5:
# 837
{const char*_tmp762;const char*_tmp761;struct Cyc_PP_Doc*_tmp760[3];s=((_tmp760[2]=Cyc_PP_text(((_tmp761=">",_tag_dyneither(_tmp761,sizeof(char),2)))),((_tmp760[1]=Cyc_Absynpp_rgn2doc(_tmp186),((_tmp760[0]=Cyc_PP_text(((_tmp762="region_t<",_tag_dyneither(_tmp762,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp760,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLF6: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp187=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp159;if(_tmp187->tag != 16)goto _LLF8;else{_tmp188=(void*)_tmp187->f1;_tmp189=(void*)_tmp187->f2;}}_LLF7:
# 839
{const char*_tmp769;const char*_tmp768;const char*_tmp767;struct Cyc_PP_Doc*_tmp766[5];s=((_tmp766[4]=
# 840
Cyc_PP_text(((_tmp767=">",_tag_dyneither(_tmp767,sizeof(char),2)))),((_tmp766[3]=
# 839
Cyc_Absynpp_rgn2doc(_tmp189),((_tmp766[2]=Cyc_PP_text(((_tmp768=",",_tag_dyneither(_tmp768,sizeof(char),2)))),((_tmp766[1]=Cyc_Absynpp_rgn2doc(_tmp188),((_tmp766[0]=Cyc_PP_text(((_tmp769="dynregion_t<",_tag_dyneither(_tmp769,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp766,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 840
goto _LLD1;_LLF8: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp18A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp159;if(_tmp18A->tag != 19)goto _LLFA;else{_tmp18B=(void*)_tmp18A->f1;}}_LLF9:
# 842
{const char*_tmp76E;const char*_tmp76D;struct Cyc_PP_Doc*_tmp76C[3];s=((_tmp76C[2]=Cyc_PP_text(((_tmp76D=">",_tag_dyneither(_tmp76D,sizeof(char),2)))),((_tmp76C[1]=Cyc_Absynpp_typ2doc(_tmp18B),((_tmp76C[0]=Cyc_PP_text(((_tmp76E="tag_t<",_tag_dyneither(_tmp76E,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp76C,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLFA: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp18C=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp159;if(_tmp18C->tag != 21)goto _LLFC;}_LLFB:
# 843
 goto _LLFD;_LLFC: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp18D=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp159;if(_tmp18D->tag != 20)goto _LLFE;}_LLFD:
# 844
 goto _LLFF;_LLFE: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp18E=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp159;if(_tmp18E->tag != 22)goto _LL100;}_LLFF:
# 846
 s=Cyc_Absynpp_rgn2doc(t);goto _LLD1;_LL100: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp18F=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp159;if(_tmp18F->tag != 25)goto _LL102;else{_tmp190=(void*)_tmp18F->f1;}}_LL101:
# 848
{const char*_tmp773;const char*_tmp772;struct Cyc_PP_Doc*_tmp771[3];s=((_tmp771[2]=Cyc_PP_text(((_tmp772=")",_tag_dyneither(_tmp772,sizeof(char),2)))),((_tmp771[1]=Cyc_Absynpp_typ2doc(_tmp190),((_tmp771[0]=Cyc_PP_text(((_tmp773="regions(",_tag_dyneither(_tmp773,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp771,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LL102: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp191=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp159;if(_tmp191->tag != 23)goto _LL104;}_LL103:
# 849
 goto _LL105;_LL104: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp192=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp159;if(_tmp192->tag != 24)goto _LL106;}_LL105:
# 851
 s=Cyc_Absynpp_eff2doc(t);goto _LLD1;_LL106: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp193=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp159;if(_tmp193->tag != 26)goto _LL108;else{_tmp194=_tmp193->f1;_tmp195=*_tmp194;_tmp196=_tmp195.r;{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp197=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp196;if(_tmp197->tag != 0)goto _LL108;else{_tmp198=_tmp197->f1;}};}}_LL107:
# 853
 s=Cyc_Absynpp_aggrdecl2doc(_tmp198);goto _LLD1;_LL108: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp199=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp159;if(_tmp199->tag != 26)goto _LL10A;else{_tmp19A=_tmp199->f1;_tmp19B=*_tmp19A;_tmp19C=_tmp19B.r;{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp19D=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp19C;if(_tmp19D->tag != 1)goto _LL10A;else{_tmp19E=_tmp19D->f1;}};}}_LL109:
# 855
 s=Cyc_Absynpp_enumdecl2doc(_tmp19E);goto _LLD1;_LL10A: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp19F=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp159;if(_tmp19F->tag != 26)goto _LLD1;else{_tmp1A0=_tmp19F->f1;_tmp1A1=*_tmp1A0;_tmp1A2=_tmp1A1.r;{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp1A3=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp1A2;if(_tmp1A3->tag != 2)goto _LLD1;else{_tmp1A4=_tmp1A3->f1;}};}}_LL10B:
# 857
 s=Cyc_Absynpp_datatypedecl2doc(_tmp1A4);goto _LLD1;_LLD1:;}
# 859
return s;}
# 862
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _dyneither_ptr*vo){
# 863
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((struct _dyneither_ptr*)vo));}struct _tuple16{void*f1;void*f2;};
# 866
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple16*cmp){
# 867
struct _tuple16 _tmp202;void*_tmp203;void*_tmp204;struct _tuple16*_tmp201=cmp;_tmp202=*_tmp201;_tmp203=_tmp202.f1;_tmp204=_tmp202.f2;{
# 868
const char*_tmp776;struct Cyc_PP_Doc*_tmp775[3];return(_tmp775[2]=Cyc_Absynpp_rgn2doc(_tmp204),((_tmp775[1]=Cyc_PP_text(((_tmp776=" > ",_tag_dyneither(_tmp776,sizeof(char),4)))),((_tmp775[0]=Cyc_Absynpp_rgn2doc(_tmp203),Cyc_PP_cat(_tag_dyneither(_tmp775,sizeof(struct Cyc_PP_Doc*),3)))))));};}
# 871
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
# 872
const char*_tmp779;const char*_tmp778;const char*_tmp777;return Cyc_PP_group(((_tmp777="",_tag_dyneither(_tmp777,sizeof(char),1))),((_tmp778="",_tag_dyneither(_tmp778,sizeof(char),1))),((_tmp779=",",_tag_dyneither(_tmp779,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}
# 875
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
# 876
struct _dyneither_ptr*_tmp20A=(*t).f1;
# 877
struct Cyc_Core_Opt*_tmp77A;struct Cyc_Core_Opt*dopt=_tmp20A == 0?0:((_tmp77A=_cycalloc(sizeof(*_tmp77A)),((_tmp77A->v=Cyc_PP_text(*_tmp20A),_tmp77A))));
# 878
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 881
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po){
# 884
struct Cyc_List_List*_tmp20C=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);
# 885
struct Cyc_PP_Doc*eff_doc;
# 886
if(c_varargs){
# 887
const char*_tmp77D;struct Cyc_List_List*_tmp77C;_tmp20C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp20C,((_tmp77C=_cycalloc(sizeof(*_tmp77C)),((_tmp77C->hd=Cyc_PP_text(((_tmp77D="...",_tag_dyneither(_tmp77D,sizeof(char),4)))),((_tmp77C->tl=0,_tmp77C)))))));}else{
# 888
if(cyc_varargs != 0){
# 889
const char*_tmp786;const char*_tmp785;const char*_tmp784;struct _tuple8*_tmp783;struct Cyc_PP_Doc*_tmp782[3];struct Cyc_PP_Doc*_tmp20F=(_tmp782[2]=
# 891
Cyc_Absynpp_funarg2doc(((_tmp783=_cycalloc(sizeof(*_tmp783)),((_tmp783->f1=cyc_varargs->name,((_tmp783->f2=cyc_varargs->tq,((_tmp783->f3=cyc_varargs->type,_tmp783))))))))),((_tmp782[1]=
# 890
cyc_varargs->inject?Cyc_PP_text(((_tmp784=" inject ",_tag_dyneither(_tmp784,sizeof(char),9)))): Cyc_PP_text(((_tmp785=" ",_tag_dyneither(_tmp785,sizeof(char),2)))),((_tmp782[0]=
# 889
Cyc_PP_text(((_tmp786="...",_tag_dyneither(_tmp786,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp782,sizeof(struct Cyc_PP_Doc*),3)))))));
# 893
struct Cyc_List_List*_tmp787;_tmp20C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp20C,((_tmp787=_cycalloc(sizeof(*_tmp787)),((_tmp787->hd=_tmp20F,((_tmp787->tl=0,_tmp787)))))));}}{
# 895
const char*_tmp78A;const char*_tmp789;const char*_tmp788;struct Cyc_PP_Doc*_tmp216=Cyc_PP_group(((_tmp788="",_tag_dyneither(_tmp788,sizeof(char),1))),((_tmp789="",_tag_dyneither(_tmp789,sizeof(char),1))),((_tmp78A=",",_tag_dyneither(_tmp78A,sizeof(char),2))),_tmp20C);
# 896
if(effopt != 0  && Cyc_Absynpp_print_all_effects){
# 897
const char*_tmp78D;struct Cyc_PP_Doc*_tmp78C[3];_tmp216=((_tmp78C[2]=Cyc_Absynpp_eff2doc((void*)effopt),((_tmp78C[1]=Cyc_PP_text(((_tmp78D=";",_tag_dyneither(_tmp78D,sizeof(char),2)))),((_tmp78C[0]=_tmp216,Cyc_PP_cat(_tag_dyneither(_tmp78C,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 898
if(rgn_po != 0){
# 899
const char*_tmp790;struct Cyc_PP_Doc*_tmp78F[3];_tmp216=((_tmp78F[2]=Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp78F[1]=Cyc_PP_text(((_tmp790=":",_tag_dyneither(_tmp790,sizeof(char),2)))),((_tmp78F[0]=_tmp216,Cyc_PP_cat(_tag_dyneither(_tmp78F,sizeof(struct Cyc_PP_Doc*),3))))))));}{
# 900
const char*_tmp795;const char*_tmp794;struct Cyc_PP_Doc*_tmp793[3];return(_tmp793[2]=Cyc_PP_text(((_tmp794=")",_tag_dyneither(_tmp794,sizeof(char),2)))),((_tmp793[1]=_tmp216,((_tmp793[0]=Cyc_PP_text(((_tmp795="(",_tag_dyneither(_tmp795,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp793,sizeof(struct Cyc_PP_Doc*),3)))))));};};}
# 903
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg){
# 904
struct _tuple8 _tmp222;struct _dyneither_ptr*_tmp223;struct Cyc_Absyn_Tqual _tmp224;void*_tmp225;struct _tuple8*_tmp221=arg;_tmp222=*_tmp221;_tmp223=_tmp222.f1;_tmp224=_tmp222.f2;_tmp225=_tmp222.f3;{
# 905
struct _tuple8*_tmp796;return(_tmp796=_cycalloc(sizeof(*_tmp796)),((_tmp796->f1=(struct _dyneither_ptr*)_tmp223,((_tmp796->f2=_tmp224,((_tmp796->f3=_tmp225,_tmp796)))))));};}
# 908
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){return Cyc_PP_text(*v);}
# 910
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
# 911
struct Cyc_List_List*_tmp227=0;
# 912
int match;
# 913
{union Cyc_Absyn_Nmspace _tmp228=(*q).f1;int _tmp229;struct Cyc_List_List*_tmp22A;struct Cyc_List_List*_tmp22B;struct Cyc_List_List*_tmp22C;_LL128: if((_tmp228.Loc_n).tag != 4)goto _LL12A;_tmp229=(int)(_tmp228.Loc_n).val;_LL129:
# 915
 _tmp22A=0;goto _LL12B;_LL12A: if((_tmp228.Rel_n).tag != 1)goto _LL12C;_tmp22A=(struct Cyc_List_List*)(_tmp228.Rel_n).val;_LL12B:
# 917
 match=0;
# 918
_tmp227=_tmp22A;
# 919
goto _LL127;_LL12C: if((_tmp228.C_n).tag != 3)goto _LL12E;_tmp22B=(struct Cyc_List_List*)(_tmp228.C_n).val;_LL12D:
# 921
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp22B,Cyc_Absynpp_curr_namespace);
# 923
goto _LL127;_LL12E: if((_tmp228.Abs_n).tag != 2)goto _LL127;_tmp22C=(struct Cyc_List_List*)(_tmp228.Abs_n).val;_LL12F:
# 925
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp22C,Cyc_Absynpp_curr_namespace);
# 926
{struct Cyc_List_List*_tmp797;_tmp227=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(_tmp797=_cycalloc(sizeof(*_tmp797)),((_tmp797->hd=Cyc_Absynpp_cyc_stringptr,((_tmp797->tl=_tmp22C,_tmp797))))): _tmp22C;}
# 927
goto _LL127;_LL127:;}
# 929
if(Cyc_Absynpp_qvar_to_Cids){
# 930
const char*_tmp799;struct Cyc_List_List*_tmp798;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp227,(
# 931
(_tmp798=_cycalloc(sizeof(*_tmp798)),((_tmp798->hd=(*q).f2,((_tmp798->tl=0,_tmp798))))))),((_tmp799="_",_tag_dyneither(_tmp799,sizeof(char),2))));}else{
# 934
if(match)
# 935
return*(*q).f2;else{
# 937
const char*_tmp79B;struct Cyc_List_List*_tmp79A;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp227,(
# 938
(_tmp79A=_cycalloc(sizeof(*_tmp79A)),((_tmp79A->hd=(*q).f2,((_tmp79A->tl=0,_tmp79A))))))),((_tmp79B="::",_tag_dyneither(_tmp79B,sizeof(char),3))));}}}
# 942
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
# 943
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 946
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
# 947
struct _dyneither_ptr _tmp232=Cyc_Absynpp_qvar2string(q);
# 948
if(Cyc_PP_tex_output){
# 950
const char*_tmp79D;const char*_tmp79C;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp79C="\\textbf{",_tag_dyneither(_tmp79C,sizeof(char),9))),(struct _dyneither_ptr)_tmp232),((_tmp79D="}",_tag_dyneither(_tmp79D,sizeof(char),2)))),(int)
# 951
Cyc_strlen((struct _dyneither_ptr)_tmp232));}else{
# 952
return Cyc_PP_text(_tmp232);}}
# 955
struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 957
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 960
if(Cyc_Absynpp_use_curr_namespace){
# 961
union Cyc_Absyn_Nmspace _tmp235=(*v).f1;int _tmp236;struct Cyc_List_List*_tmp237;struct Cyc_List_List*_tmp238;struct Cyc_List_List*_tmp239;_LL131: if((_tmp235.Loc_n).tag != 4)goto _LL133;_tmp236=(int)(_tmp235.Loc_n).val;_LL132:
# 962
 goto _LL134;_LL133: if((_tmp235.Rel_n).tag != 1)goto _LL135;_tmp237=(struct Cyc_List_List*)(_tmp235.Rel_n).val;if(_tmp237 != 0)goto _LL135;_LL134:
# 963
 return*(*v).f2;_LL135: if((_tmp235.C_n).tag != 3)goto _LL137;_tmp238=(struct Cyc_List_List*)(_tmp235.C_n).val;_LL136:
# 964
 _tmp239=_tmp238;goto _LL138;_LL137: if((_tmp235.Abs_n).tag != 2)goto _LL139;_tmp239=(struct Cyc_List_List*)(_tmp235.Abs_n).val;_LL138:
# 966
 if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp239,Cyc_Absynpp_curr_namespace)== 0)
# 967
return*(*v).f2;else{
# 969
goto _LL13A;}_LL139:;_LL13A: {
# 971
const char*_tmp79E;return(struct _dyneither_ptr)Cyc_strconcat(((_tmp79E="/* bad namespace : */ ",_tag_dyneither(_tmp79E,sizeof(char),23))),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));}_LL130:;}else{
# 974
return*(*v).f2;}}
# 977
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
# 978
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 980
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){
# 981
struct _dyneither_ptr _tmp23B=Cyc_Absynpp_typedef_name2string(v);
# 982
if(Cyc_PP_tex_output){
# 984
const char*_tmp7A0;const char*_tmp79F;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp79F="\\textbf{",_tag_dyneither(_tmp79F,sizeof(char),9))),(struct _dyneither_ptr)_tmp23B),((_tmp7A0="}",_tag_dyneither(_tmp7A0,sizeof(char),2)))),(int)
# 985
Cyc_strlen((struct _dyneither_ptr)_tmp23B));}else{
# 986
return Cyc_PP_text(_tmp23B);}}
# 989
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
# 990
return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,0);}
# 1000 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
# 1001
void*_tmp23E=e->r;enum Cyc_Absyn_Primop _tmp242;struct Cyc_Absyn_Exp*_tmp24C;struct Cyc_Absyn_Exp*_tmp24E;_LL13C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp23F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp23F->tag != 0)goto _LL13E;}_LL13D:
# 1002
 goto _LL13F;_LL13E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp240=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp240->tag != 1)goto _LL140;}_LL13F:
# 1003
 return 10000;_LL140: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp241=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp241->tag != 2)goto _LL142;else{_tmp242=_tmp241->f1;}}_LL141:
# 1005
 switch(_tmp242){case Cyc_Absyn_Plus: _LL18A:
# 1006
 return 100;case Cyc_Absyn_Times: _LL18B:
# 1007
 return 110;case Cyc_Absyn_Minus: _LL18C:
# 1008
 return 100;case Cyc_Absyn_Div: _LL18D:
# 1009
 goto _LL18E;case Cyc_Absyn_Mod: _LL18E:
# 1010
 return 110;case Cyc_Absyn_Eq: _LL18F:
# 1011
 goto _LL190;case Cyc_Absyn_Neq: _LL190:
# 1012
 return 70;case Cyc_Absyn_Gt: _LL191:
# 1013
 goto _LL192;case Cyc_Absyn_Lt: _LL192:
# 1014
 goto _LL193;case Cyc_Absyn_Gte: _LL193:
# 1015
 goto _LL194;case Cyc_Absyn_Lte: _LL194:
# 1016
 return 80;case Cyc_Absyn_Not: _LL195:
# 1017
 goto _LL196;case Cyc_Absyn_Bitnot: _LL196:
# 1018
 return 130;case Cyc_Absyn_Bitand: _LL197:
# 1019
 return 60;case Cyc_Absyn_Bitor: _LL198:
# 1020
 return 40;case Cyc_Absyn_Bitxor: _LL199:
# 1021
 return 50;case Cyc_Absyn_Bitlshift: _LL19A:
# 1022
 return 90;case Cyc_Absyn_Bitlrshift: _LL19B:
# 1023
 return 80;case Cyc_Absyn_Bitarshift: _LL19C:
# 1024
 return 80;case Cyc_Absyn_Numelts: _LL19D:
# 1025
 return 140;}_LL142: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp243=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp243->tag != 3)goto _LL144;}_LL143:
# 1027
 return 20;_LL144: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp244=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp244->tag != 4)goto _LL146;}_LL145:
# 1028
 return 130;_LL146: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp245=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp245->tag != 5)goto _LL148;}_LL147:
# 1029
 return 30;_LL148: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp246=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp246->tag != 6)goto _LL14A;}_LL149:
# 1030
 return 35;_LL14A: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp247=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp247->tag != 7)goto _LL14C;}_LL14B:
# 1031
 return 30;_LL14C: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp248=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp248->tag != 8)goto _LL14E;}_LL14D:
# 1032
 return 10;_LL14E: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp249=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp249->tag != 9)goto _LL150;}_LL14F:
# 1033
 return 140;_LL150: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp24A=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp24A->tag != 10)goto _LL152;}_LL151:
# 1034
 return 130;_LL152: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp24B=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp24B->tag != 11)goto _LL154;else{_tmp24C=_tmp24B->f1;}}_LL153:
# 1035
 return Cyc_Absynpp_exp_prec(_tmp24C);_LL154: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp24D=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp24D->tag != 12)goto _LL156;else{_tmp24E=_tmp24D->f1;}}_LL155:
# 1036
 return Cyc_Absynpp_exp_prec(_tmp24E);_LL156: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp24F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp24F->tag != 13)goto _LL158;}_LL157:
# 1037
 return 120;_LL158: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp250=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp250->tag != 15)goto _LL15A;}_LL159:
# 1038
 return 15;_LL15A: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp251=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp251->tag != 14)goto _LL15C;}_LL15B:
# 1039
 goto _LL15D;_LL15C: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp252=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp252->tag != 16)goto _LL15E;}_LL15D:
# 1040
 goto _LL15F;_LL15E: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp253=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp253->tag != 17)goto _LL160;}_LL15F:
# 1041
 goto _LL161;_LL160: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp254=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp254->tag != 37)goto _LL162;}_LL161:
# 1042
 goto _LL163;_LL162: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp255=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp255->tag != 38)goto _LL164;}_LL163:
# 1043
 goto _LL165;_LL164: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp256=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp256->tag != 36)goto _LL166;}_LL165:
# 1044
 goto _LL167;_LL166: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp257=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp257->tag != 18)goto _LL168;}_LL167:
# 1045
 goto _LL169;_LL168: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp258=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp258->tag != 19)goto _LL16A;}_LL169:
# 1046
 return 130;_LL16A: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp259=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp259->tag != 20)goto _LL16C;}_LL16B:
# 1047
 goto _LL16D;_LL16C: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp25A=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp25A->tag != 21)goto _LL16E;}_LL16D:
# 1048
 goto _LL16F;_LL16E: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp25B=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp25B->tag != 22)goto _LL170;}_LL16F:
# 1049
 return 140;_LL170: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp25C=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp25C->tag != 23)goto _LL172;}_LL171:
# 1050
 return 150;_LL172: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp25D=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp25D->tag != 24)goto _LL174;}_LL173:
# 1051
 goto _LL175;_LL174: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp25E=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp25E->tag != 25)goto _LL176;}_LL175:
# 1052
 goto _LL177;_LL176: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp25F=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp25F->tag != 26)goto _LL178;}_LL177:
# 1053
 goto _LL179;_LL178: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp260=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp260->tag != 27)goto _LL17A;}_LL179:
# 1054
 goto _LL17B;_LL17A: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp261=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp261->tag != 28)goto _LL17C;}_LL17B:
# 1055
 goto _LL17D;_LL17C: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp262=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp262->tag != 29)goto _LL17E;}_LL17D:
# 1056
 goto _LL17F;_LL17E: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp263=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp263->tag != 30)goto _LL180;}_LL17F:
# 1057
 goto _LL181;_LL180: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp264=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp264->tag != 31)goto _LL182;}_LL181:
# 1058
 goto _LL183;_LL182: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp265=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp265->tag != 32)goto _LL184;}_LL183:
# 1059
 goto _LL185;_LL184: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp266=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp266->tag != 33)goto _LL186;}_LL185:
# 1060
 goto _LL187;_LL186: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp267=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp267->tag != 34)goto _LL188;}_LL187:
# 1061
 return 140;_LL188: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp268=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp23E;if(_tmp268->tag != 35)goto _LL13B;}_LL189:
# 1062
 return 10000;_LL13B:;}
# 1066
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
# 1067
return Cyc_Absynpp_exp2doc_prec(0,e);}
# 1070
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
# 1071
int myprec=Cyc_Absynpp_exp_prec(e);
# 1072
struct Cyc_PP_Doc*s;
# 1073
{void*_tmp269=e->r;union Cyc_Absyn_Cnst _tmp26B;struct _tuple0*_tmp26D;enum Cyc_Absyn_Primop _tmp26F;struct Cyc_List_List*_tmp270;struct Cyc_Absyn_Exp*_tmp272;struct Cyc_Core_Opt*_tmp273;struct Cyc_Absyn_Exp*_tmp274;struct Cyc_Absyn_Exp*_tmp276;enum Cyc_Absyn_Incrementor _tmp277;struct Cyc_Absyn_Exp*_tmp279;struct Cyc_Absyn_Exp*_tmp27A;struct Cyc_Absyn_Exp*_tmp27B;struct Cyc_Absyn_Exp*_tmp27D;struct Cyc_Absyn_Exp*_tmp27E;struct Cyc_Absyn_Exp*_tmp280;struct Cyc_Absyn_Exp*_tmp281;struct Cyc_Absyn_Exp*_tmp283;struct Cyc_Absyn_Exp*_tmp284;struct Cyc_Absyn_Exp*_tmp286;struct Cyc_List_List*_tmp287;struct Cyc_Absyn_Exp*_tmp289;struct Cyc_Absyn_Exp*_tmp28B;struct Cyc_Absyn_Exp*_tmp28D;void*_tmp28F;struct Cyc_Absyn_Exp*_tmp290;struct Cyc_Absyn_Exp*_tmp292;struct Cyc_Absyn_Exp*_tmp294;struct Cyc_Absyn_Exp*_tmp295;void*_tmp297;struct Cyc_Absyn_Exp*_tmp299;void*_tmp29B;int _tmp29D;struct _dyneither_ptr _tmp29E;struct Cyc_Absyn_Exp*_tmp2A0;struct _dyneither_ptr*_tmp2A1;void*_tmp2A3;void*_tmp2A4;struct _dyneither_ptr*_tmp2A6;void*_tmp2A8;void*_tmp2A9;unsigned int _tmp2AB;struct Cyc_Absyn_Exp*_tmp2AD;struct Cyc_Absyn_Exp*_tmp2AF;struct _dyneither_ptr*_tmp2B0;struct Cyc_Absyn_Exp*_tmp2B2;struct _dyneither_ptr*_tmp2B3;struct Cyc_Absyn_Exp*_tmp2B5;struct Cyc_Absyn_Exp*_tmp2B6;struct Cyc_List_List*_tmp2B8;struct _tuple8*_tmp2BA;struct Cyc_List_List*_tmp2BB;struct Cyc_List_List*_tmp2BD;struct Cyc_Absyn_Vardecl*_tmp2BF;struct Cyc_Absyn_Exp*_tmp2C0;struct Cyc_Absyn_Exp*_tmp2C1;struct _tuple0*_tmp2C3;struct Cyc_List_List*_tmp2C4;struct Cyc_List_List*_tmp2C5;struct Cyc_List_List*_tmp2C7;struct Cyc_List_List*_tmp2C9;struct Cyc_Absyn_Datatypefield*_tmp2CA;struct _tuple0*_tmp2CC;struct _tuple0*_tmp2CE;struct Cyc_Absyn_MallocInfo _tmp2D0;int _tmp2D1;struct Cyc_Absyn_Exp*_tmp2D2;void**_tmp2D3;struct Cyc_Absyn_Exp*_tmp2D4;struct Cyc_Absyn_Exp*_tmp2D6;struct Cyc_Absyn_Exp*_tmp2D7;struct Cyc_Core_Opt*_tmp2D9;struct Cyc_List_List*_tmp2DA;struct Cyc_Absyn_Stmt*_tmp2DC;_LL1A0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp26A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp26A->tag != 0)goto _LL1A2;else{_tmp26B=_tmp26A->f1;}}_LL1A1:
# 1074
 s=Cyc_Absynpp_cnst2doc(_tmp26B);goto _LL19F;_LL1A2: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp26C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp26C->tag != 1)goto _LL1A4;else{_tmp26D=_tmp26C->f1;}}_LL1A3:
# 1075
 s=Cyc_Absynpp_qvar2doc(_tmp26D);goto _LL19F;_LL1A4: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp26E=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp26E->tag != 2)goto _LL1A6;else{_tmp26F=_tmp26E->f1;_tmp270=_tmp26E->f2;}}_LL1A5:
# 1076
 s=Cyc_Absynpp_primapp2doc(myprec,_tmp26F,_tmp270);goto _LL19F;_LL1A6: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp271=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp271->tag != 3)goto _LL1A8;else{_tmp272=_tmp271->f1;_tmp273=_tmp271->f2;_tmp274=_tmp271->f3;}}_LL1A7:
# 1078
{const char*_tmp7A5;const char*_tmp7A4;struct Cyc_PP_Doc*_tmp7A3[5];s=((_tmp7A3[4]=
# 1082
Cyc_Absynpp_exp2doc_prec(myprec,_tmp274),((_tmp7A3[3]=
# 1081
Cyc_PP_text(((_tmp7A4="= ",_tag_dyneither(_tmp7A4,sizeof(char),3)))),((_tmp7A3[2]=
# 1080
_tmp273 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp273->v),((_tmp7A3[1]=
# 1079
Cyc_PP_text(((_tmp7A5=" ",_tag_dyneither(_tmp7A5,sizeof(char),2)))),((_tmp7A3[0]=
# 1078
Cyc_Absynpp_exp2doc_prec(myprec,_tmp272),Cyc_PP_cat(_tag_dyneither(_tmp7A3,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1083
goto _LL19F;_LL1A8: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp275=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp275->tag != 4)goto _LL1AA;else{_tmp276=_tmp275->f1;_tmp277=_tmp275->f2;}}_LL1A9: {
# 1085
struct Cyc_PP_Doc*_tmp2E0=Cyc_Absynpp_exp2doc_prec(myprec,_tmp276);
# 1086
switch(_tmp277){case Cyc_Absyn_PreInc: _LL1F0:
# 1087
{const char*_tmp7A8;struct Cyc_PP_Doc*_tmp7A7[2];s=((_tmp7A7[1]=_tmp2E0,((_tmp7A7[0]=Cyc_PP_text(((_tmp7A8="++",_tag_dyneither(_tmp7A8,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp7A7,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PreDec: _LL1F1:
# 1088
{const char*_tmp7AB;struct Cyc_PP_Doc*_tmp7AA[2];s=((_tmp7AA[1]=_tmp2E0,((_tmp7AA[0]=Cyc_PP_text(((_tmp7AB="--",_tag_dyneither(_tmp7AB,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp7AA,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostInc: _LL1F2:
# 1089
{const char*_tmp7AE;struct Cyc_PP_Doc*_tmp7AD[2];s=((_tmp7AD[1]=Cyc_PP_text(((_tmp7AE="++",_tag_dyneither(_tmp7AE,sizeof(char),3)))),((_tmp7AD[0]=_tmp2E0,Cyc_PP_cat(_tag_dyneither(_tmp7AD,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostDec: _LL1F3:
# 1090
{const char*_tmp7B1;struct Cyc_PP_Doc*_tmp7B0[2];s=((_tmp7B0[1]=Cyc_PP_text(((_tmp7B1="--",_tag_dyneither(_tmp7B1,sizeof(char),3)))),((_tmp7B0[0]=_tmp2E0,Cyc_PP_cat(_tag_dyneither(_tmp7B0,sizeof(struct Cyc_PP_Doc*),2))))));}break;}
# 1092
goto _LL19F;}_LL1AA: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp278=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp278->tag != 5)goto _LL1AC;else{_tmp279=_tmp278->f1;_tmp27A=_tmp278->f2;_tmp27B=_tmp278->f3;}}_LL1AB:
# 1094
{const char*_tmp7B6;const char*_tmp7B5;struct Cyc_PP_Doc*_tmp7B4[5];s=((_tmp7B4[4]=
# 1095
Cyc_Absynpp_exp2doc_prec(myprec,_tmp27B),((_tmp7B4[3]=Cyc_PP_text(((_tmp7B5=" : ",_tag_dyneither(_tmp7B5,sizeof(char),4)))),((_tmp7B4[2]=
# 1094
Cyc_Absynpp_exp2doc_prec(0,_tmp27A),((_tmp7B4[1]=Cyc_PP_text(((_tmp7B6=" ? ",_tag_dyneither(_tmp7B6,sizeof(char),4)))),((_tmp7B4[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp279),Cyc_PP_cat(_tag_dyneither(_tmp7B4,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1096
goto _LL19F;_LL1AC: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp27C=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp27C->tag != 6)goto _LL1AE;else{_tmp27D=_tmp27C->f1;_tmp27E=_tmp27C->f2;}}_LL1AD:
# 1098
{const char*_tmp7B9;struct Cyc_PP_Doc*_tmp7B8[3];s=((_tmp7B8[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp27E),((_tmp7B8[1]=Cyc_PP_text(((_tmp7B9=" && ",_tag_dyneither(_tmp7B9,sizeof(char),5)))),((_tmp7B8[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp27D),Cyc_PP_cat(_tag_dyneither(_tmp7B8,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1099
goto _LL19F;_LL1AE: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp27F=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp27F->tag != 7)goto _LL1B0;else{_tmp280=_tmp27F->f1;_tmp281=_tmp27F->f2;}}_LL1AF:
# 1101
{const char*_tmp7BC;struct Cyc_PP_Doc*_tmp7BB[3];s=((_tmp7BB[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp281),((_tmp7BB[1]=Cyc_PP_text(((_tmp7BC=" || ",_tag_dyneither(_tmp7BC,sizeof(char),5)))),((_tmp7BB[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp280),Cyc_PP_cat(_tag_dyneither(_tmp7BB,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1102
goto _LL19F;_LL1B0: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp282=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp282->tag != 8)goto _LL1B2;else{_tmp283=_tmp282->f1;_tmp284=_tmp282->f2;}}_LL1B1:
# 1106
{const char*_tmp7C3;const char*_tmp7C2;const char*_tmp7C1;struct Cyc_PP_Doc*_tmp7C0[5];s=((_tmp7C0[4]=Cyc_PP_text(((_tmp7C1=")",_tag_dyneither(_tmp7C1,sizeof(char),2)))),((_tmp7C0[3]=Cyc_Absynpp_exp2doc(_tmp284),((_tmp7C0[2]=Cyc_PP_text(((_tmp7C2=", ",_tag_dyneither(_tmp7C2,sizeof(char),3)))),((_tmp7C0[1]=Cyc_Absynpp_exp2doc(_tmp283),((_tmp7C0[0]=Cyc_PP_text(((_tmp7C3="(",_tag_dyneither(_tmp7C3,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7C0,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1107
goto _LL19F;_LL1B2: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp285=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp285->tag != 9)goto _LL1B4;else{_tmp286=_tmp285->f1;_tmp287=_tmp285->f2;}}_LL1B3:
# 1109
{const char*_tmp7C8;const char*_tmp7C7;struct Cyc_PP_Doc*_tmp7C6[4];s=((_tmp7C6[3]=
# 1112
Cyc_PP_text(((_tmp7C7=")",_tag_dyneither(_tmp7C7,sizeof(char),2)))),((_tmp7C6[2]=
# 1111
Cyc_Absynpp_exps2doc_prec(20,_tmp287),((_tmp7C6[1]=
# 1110
Cyc_PP_text(((_tmp7C8="(",_tag_dyneither(_tmp7C8,sizeof(char),2)))),((_tmp7C6[0]=
# 1109
Cyc_Absynpp_exp2doc_prec(myprec,_tmp286),Cyc_PP_cat(_tag_dyneither(_tmp7C6,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1113
goto _LL19F;_LL1B4: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp288=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp288->tag != 10)goto _LL1B6;else{_tmp289=_tmp288->f1;}}_LL1B5:
# 1115
{const char*_tmp7CB;struct Cyc_PP_Doc*_tmp7CA[2];s=((_tmp7CA[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp289),((_tmp7CA[0]=Cyc_PP_text(((_tmp7CB="throw ",_tag_dyneither(_tmp7CB,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp7CA,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1116
goto _LL19F;_LL1B6: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp28A=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp28A->tag != 11)goto _LL1B8;else{_tmp28B=_tmp28A->f1;}}_LL1B7:
# 1118
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp28B);
# 1119
goto _LL19F;_LL1B8: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp28C=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp28C->tag != 12)goto _LL1BA;else{_tmp28D=_tmp28C->f1;}}_LL1B9:
# 1121
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp28D);
# 1122
goto _LL19F;_LL1BA: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp28E=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp28E->tag != 13)goto _LL1BC;else{_tmp28F=(void*)_tmp28E->f1;_tmp290=_tmp28E->f2;}}_LL1BB:
# 1124
{const char*_tmp7D0;const char*_tmp7CF;struct Cyc_PP_Doc*_tmp7CE[4];s=((_tmp7CE[3]=
# 1127
Cyc_Absynpp_exp2doc_prec(myprec,_tmp290),((_tmp7CE[2]=
# 1126
Cyc_PP_text(((_tmp7CF=")",_tag_dyneither(_tmp7CF,sizeof(char),2)))),((_tmp7CE[1]=
# 1125
Cyc_Absynpp_typ2doc(_tmp28F),((_tmp7CE[0]=
# 1124
Cyc_PP_text(((_tmp7D0="(",_tag_dyneither(_tmp7D0,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7CE,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1128
goto _LL19F;_LL1BC: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp291=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp291->tag != 14)goto _LL1BE;else{_tmp292=_tmp291->f1;}}_LL1BD:
# 1130
{const char*_tmp7D3;struct Cyc_PP_Doc*_tmp7D2[2];s=((_tmp7D2[1]=
# 1131
Cyc_Absynpp_exp2doc_prec(myprec,_tmp292),((_tmp7D2[0]=
# 1130
Cyc_PP_text(((_tmp7D3="&",_tag_dyneither(_tmp7D3,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7D2,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1132
goto _LL19F;_LL1BE: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp293=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp293->tag != 15)goto _LL1C0;else{_tmp294=_tmp293->f1;_tmp295=_tmp293->f2;}}_LL1BF:
# 1134
 if(_tmp294 == 0){
# 1135
const char*_tmp7D6;struct Cyc_PP_Doc*_tmp7D5[2];s=((_tmp7D5[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp295),((_tmp7D5[0]=Cyc_PP_text(((_tmp7D6="new ",_tag_dyneither(_tmp7D6,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp7D5,sizeof(struct Cyc_PP_Doc*),2))))));}else{
# 1137
const char*_tmp7DB;const char*_tmp7DA;struct Cyc_PP_Doc*_tmp7D9[4];s=((_tmp7D9[3]=
# 1138
Cyc_Absynpp_exp2doc_prec(myprec,_tmp295),((_tmp7D9[2]=
# 1137
Cyc_PP_text(((_tmp7DA=") ",_tag_dyneither(_tmp7DA,sizeof(char),3)))),((_tmp7D9[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp294),((_tmp7D9[0]=Cyc_PP_text(((_tmp7DB="rnew(",_tag_dyneither(_tmp7DB,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp7D9,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1139
goto _LL19F;_LL1C0: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp296=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp296->tag != 16)goto _LL1C2;else{_tmp297=(void*)_tmp296->f1;}}_LL1C1:
# 1140
{const char*_tmp7E0;const char*_tmp7DF;struct Cyc_PP_Doc*_tmp7DE[3];s=((_tmp7DE[2]=Cyc_PP_text(((_tmp7DF=")",_tag_dyneither(_tmp7DF,sizeof(char),2)))),((_tmp7DE[1]=Cyc_Absynpp_typ2doc(_tmp297),((_tmp7DE[0]=Cyc_PP_text(((_tmp7E0="sizeof(",_tag_dyneither(_tmp7E0,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7DE,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL19F;_LL1C2: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp298=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp298->tag != 17)goto _LL1C4;else{_tmp299=_tmp298->f1;}}_LL1C3:
# 1141
{const char*_tmp7E5;const char*_tmp7E4;struct Cyc_PP_Doc*_tmp7E3[3];s=((_tmp7E3[2]=Cyc_PP_text(((_tmp7E4=")",_tag_dyneither(_tmp7E4,sizeof(char),2)))),((_tmp7E3[1]=Cyc_Absynpp_exp2doc(_tmp299),((_tmp7E3[0]=Cyc_PP_text(((_tmp7E5="sizeof(",_tag_dyneither(_tmp7E5,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7E3,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL19F;_LL1C4: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp29A=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp29A->tag != 37)goto _LL1C6;else{_tmp29B=(void*)_tmp29A->f1;}}_LL1C5:
# 1142
{const char*_tmp7EA;const char*_tmp7E9;struct Cyc_PP_Doc*_tmp7E8[3];s=((_tmp7E8[2]=Cyc_PP_text(((_tmp7E9=")",_tag_dyneither(_tmp7E9,sizeof(char),2)))),((_tmp7E8[1]=Cyc_Absynpp_typ2doc(_tmp29B),((_tmp7E8[0]=Cyc_PP_text(((_tmp7EA="valueof(",_tag_dyneither(_tmp7EA,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7E8,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL19F;_LL1C6: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp29C=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp29C->tag != 38)goto _LL1C8;else{_tmp29D=_tmp29C->f1;_tmp29E=_tmp29C->f2;}}_LL1C7:
# 1144
 if(_tmp29D){
# 1145
const char*_tmp7F1;const char*_tmp7F0;const char*_tmp7EF;struct Cyc_PP_Doc*_tmp7EE[4];s=((_tmp7EE[3]=Cyc_PP_text(((_tmp7EF=")",_tag_dyneither(_tmp7EF,sizeof(char),2)))),((_tmp7EE[2]=Cyc_PP_text(_tmp29E),((_tmp7EE[1]=Cyc_PP_text(((_tmp7F0=" volatile (",_tag_dyneither(_tmp7F0,sizeof(char),12)))),((_tmp7EE[0]=Cyc_PP_text(((_tmp7F1="__asm__",_tag_dyneither(_tmp7F1,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7EE,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1147
const char*_tmp7F6;const char*_tmp7F5;struct Cyc_PP_Doc*_tmp7F4[3];s=((_tmp7F4[2]=Cyc_PP_text(((_tmp7F5=")",_tag_dyneither(_tmp7F5,sizeof(char),2)))),((_tmp7F4[1]=Cyc_PP_text(_tmp29E),((_tmp7F4[0]=Cyc_PP_text(((_tmp7F6="__asm__(",_tag_dyneither(_tmp7F6,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7F4,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1148
goto _LL19F;_LL1C8: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp29F=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp29F->tag != 36)goto _LL1CA;else{_tmp2A0=_tmp29F->f1;_tmp2A1=_tmp29F->f2;}}_LL1C9:
# 1150
{const char*_tmp7FD;const char*_tmp7FC;const char*_tmp7FB;struct Cyc_PP_Doc*_tmp7FA[5];s=((_tmp7FA[4]=Cyc_PP_text(((_tmp7FB=")",_tag_dyneither(_tmp7FB,sizeof(char),2)))),((_tmp7FA[3]=Cyc_PP_textptr(_tmp2A1),((_tmp7FA[2]=Cyc_PP_text(((_tmp7FC=".",_tag_dyneither(_tmp7FC,sizeof(char),2)))),((_tmp7FA[1]=Cyc_Absynpp_exp2doc(_tmp2A0),((_tmp7FA[0]=Cyc_PP_text(((_tmp7FD="tagcheck(",_tag_dyneither(_tmp7FD,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7FA,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1151
goto _LL19F;_LL1CA: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp2A2=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp2A2->tag != 18)goto _LL1CC;else{_tmp2A3=(void*)_tmp2A2->f1;_tmp2A4=(void*)_tmp2A2->f2;{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp2A5=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp2A4;if(_tmp2A5->tag != 0)goto _LL1CC;else{_tmp2A6=_tmp2A5->f1;}};}}_LL1CB:
# 1153
{const char*_tmp804;const char*_tmp803;const char*_tmp802;struct Cyc_PP_Doc*_tmp801[5];s=((_tmp801[4]=Cyc_PP_text(((_tmp802=")",_tag_dyneither(_tmp802,sizeof(char),2)))),((_tmp801[3]=Cyc_PP_textptr(_tmp2A6),((_tmp801[2]=Cyc_PP_text(((_tmp803=",",_tag_dyneither(_tmp803,sizeof(char),2)))),((_tmp801[1]=Cyc_Absynpp_typ2doc(_tmp2A3),((_tmp801[0]=Cyc_PP_text(((_tmp804="offsetof(",_tag_dyneither(_tmp804,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp801,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1154
goto _LL19F;_LL1CC: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp2A7=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp2A7->tag != 18)goto _LL1CE;else{_tmp2A8=(void*)_tmp2A7->f1;_tmp2A9=(void*)_tmp2A7->f2;{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp2AA=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp2A9;if(_tmp2AA->tag != 1)goto _LL1CE;else{_tmp2AB=_tmp2AA->f1;}};}}_LL1CD:
# 1156
{const char*_tmp812;const char*_tmp811;struct Cyc_Int_pa_PrintArg_struct _tmp810;void*_tmp80F[1];const char*_tmp80E;const char*_tmp80D;struct Cyc_PP_Doc*_tmp80C[5];s=((_tmp80C[4]=Cyc_PP_text(((_tmp80D=")",_tag_dyneither(_tmp80D,sizeof(char),2)))),((_tmp80C[3]=Cyc_PP_text((struct _dyneither_ptr)((_tmp810.tag=1,((_tmp810.f1=(unsigned long)((int)_tmp2AB),((_tmp80F[0]=& _tmp810,Cyc_aprintf(((_tmp80E="%d",_tag_dyneither(_tmp80E,sizeof(char),3))),_tag_dyneither(_tmp80F,sizeof(void*),1))))))))),((_tmp80C[2]=Cyc_PP_text(((_tmp811=",",_tag_dyneither(_tmp811,sizeof(char),2)))),((_tmp80C[1]=Cyc_Absynpp_typ2doc(_tmp2A8),((_tmp80C[0]=Cyc_PP_text(((_tmp812="offsetof(",_tag_dyneither(_tmp812,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp80C,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1157
goto _LL19F;_LL1CE: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp2AC=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp2AC->tag != 19)goto _LL1D0;else{_tmp2AD=_tmp2AC->f1;}}_LL1CF:
# 1159
{const char*_tmp815;struct Cyc_PP_Doc*_tmp814[2];s=((_tmp814[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AD),((_tmp814[0]=Cyc_PP_text(((_tmp815="*",_tag_dyneither(_tmp815,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp814,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1160
goto _LL19F;_LL1D0: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp2AE=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp2AE->tag != 20)goto _LL1D2;else{_tmp2AF=_tmp2AE->f1;_tmp2B0=_tmp2AE->f2;}}_LL1D1:
# 1162
{const char*_tmp818;struct Cyc_PP_Doc*_tmp817[3];s=((_tmp817[2]=Cyc_PP_textptr(_tmp2B0),((_tmp817[1]=Cyc_PP_text(((_tmp818=".",_tag_dyneither(_tmp818,sizeof(char),2)))),((_tmp817[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AF),Cyc_PP_cat(_tag_dyneither(_tmp817,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1163
goto _LL19F;_LL1D2: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp2B1=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp2B1->tag != 21)goto _LL1D4;else{_tmp2B2=_tmp2B1->f1;_tmp2B3=_tmp2B1->f2;}}_LL1D3:
# 1165
{const char*_tmp81B;struct Cyc_PP_Doc*_tmp81A[3];s=((_tmp81A[2]=Cyc_PP_textptr(_tmp2B3),((_tmp81A[1]=Cyc_PP_text(((_tmp81B="->",_tag_dyneither(_tmp81B,sizeof(char),3)))),((_tmp81A[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B2),Cyc_PP_cat(_tag_dyneither(_tmp81A,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1166
goto _LL19F;_LL1D4: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp2B4=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp2B4->tag != 22)goto _LL1D6;else{_tmp2B5=_tmp2B4->f1;_tmp2B6=_tmp2B4->f2;}}_LL1D5:
# 1168
{const char*_tmp820;const char*_tmp81F;struct Cyc_PP_Doc*_tmp81E[4];s=((_tmp81E[3]=
# 1171
Cyc_PP_text(((_tmp81F="]",_tag_dyneither(_tmp81F,sizeof(char),2)))),((_tmp81E[2]=
# 1170
Cyc_Absynpp_exp2doc(_tmp2B6),((_tmp81E[1]=
# 1169
Cyc_PP_text(((_tmp820="[",_tag_dyneither(_tmp820,sizeof(char),2)))),((_tmp81E[0]=
# 1168
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B5),Cyc_PP_cat(_tag_dyneither(_tmp81E,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1172
goto _LL19F;_LL1D6: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp2B7=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp2B7->tag != 23)goto _LL1D8;else{_tmp2B8=_tmp2B7->f1;}}_LL1D7:
# 1174
{const char*_tmp825;const char*_tmp824;struct Cyc_PP_Doc*_tmp823[4];s=((_tmp823[3]=
# 1177
Cyc_PP_text(((_tmp824=")",_tag_dyneither(_tmp824,sizeof(char),2)))),((_tmp823[2]=
# 1176
Cyc_Absynpp_exps2doc_prec(20,_tmp2B8),((_tmp823[1]=
# 1175
Cyc_PP_text(((_tmp825="(",_tag_dyneither(_tmp825,sizeof(char),2)))),((_tmp823[0]=
# 1174
Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp823,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1178
goto _LL19F;_LL1D8: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp2B9=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp2B9->tag != 24)goto _LL1DA;else{_tmp2BA=_tmp2B9->f1;_tmp2BB=_tmp2B9->f2;}}_LL1D9:
# 1180
{const char*_tmp82C;const char*_tmp82B;const char*_tmp82A;struct Cyc_PP_Doc*_tmp829[4];s=((_tmp829[3]=
# 1183
Cyc_Absynpp_group_braces(((_tmp82A=",",_tag_dyneither(_tmp82A,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2BB)),((_tmp829[2]=
# 1182
Cyc_PP_text(((_tmp82B=")",_tag_dyneither(_tmp82B,sizeof(char),2)))),((_tmp829[1]=
# 1181
Cyc_Absynpp_typ2doc((*_tmp2BA).f3),((_tmp829[0]=
# 1180
Cyc_PP_text(((_tmp82C="(",_tag_dyneither(_tmp82C,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp829,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1184
goto _LL19F;_LL1DA: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp2BC=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp2BC->tag != 25)goto _LL1DC;else{_tmp2BD=_tmp2BC->f1;}}_LL1DB:
# 1186
{const char*_tmp82D;s=Cyc_Absynpp_group_braces(((_tmp82D=",",_tag_dyneither(_tmp82D,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2BD));}
# 1187
goto _LL19F;_LL1DC: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp2BE=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp2BE->tag != 26)goto _LL1DE;else{_tmp2BF=_tmp2BE->f1;_tmp2C0=_tmp2BE->f2;_tmp2C1=_tmp2BE->f3;}}_LL1DD:
# 1189
{const char*_tmp834;const char*_tmp833;const char*_tmp832;struct Cyc_PP_Doc*_tmp831[8];s=((_tmp831[7]=
# 1195
Cyc_Absynpp_rb(),((_tmp831[6]=
# 1194
Cyc_Absynpp_exp2doc(_tmp2C1),((_tmp831[5]=
# 1193
Cyc_PP_text(((_tmp832=" : ",_tag_dyneither(_tmp832,sizeof(char),4)))),((_tmp831[4]=
# 1192
Cyc_Absynpp_exp2doc(_tmp2C0),((_tmp831[3]=
# 1191
Cyc_PP_text(((_tmp833=" < ",_tag_dyneither(_tmp833,sizeof(char),4)))),((_tmp831[2]=
# 1190
Cyc_PP_text(*(*_tmp2BF->name).f2),((_tmp831[1]=
# 1189
Cyc_PP_text(((_tmp834="for ",_tag_dyneither(_tmp834,sizeof(char),5)))),((_tmp831[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp831,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1196
goto _LL19F;_LL1DE: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp2C2=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp2C2->tag != 27)goto _LL1E0;else{_tmp2C3=_tmp2C2->f1;_tmp2C4=_tmp2C2->f2;_tmp2C5=_tmp2C2->f3;}}_LL1DF: {
# 1198
struct Cyc_List_List*_tmp337=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2C5);
# 1199
{const char*_tmp839;struct Cyc_List_List*_tmp838;struct Cyc_PP_Doc*_tmp837[2];s=((_tmp837[1]=
# 1200
Cyc_Absynpp_group_braces(((_tmp839=",",_tag_dyneither(_tmp839,sizeof(char),2))),
# 1201
_tmp2C4 != 0?(_tmp838=_cycalloc(sizeof(*_tmp838)),((_tmp838->hd=Cyc_Absynpp_tps2doc(_tmp2C4),((_tmp838->tl=_tmp337,_tmp838))))): _tmp337),((_tmp837[0]=
# 1199
Cyc_Absynpp_qvar2doc(_tmp2C3),Cyc_PP_cat(_tag_dyneither(_tmp837,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1202
goto _LL19F;}_LL1E0: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp2C6=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp2C6->tag != 28)goto _LL1E2;else{_tmp2C7=_tmp2C6->f2;}}_LL1E1:
# 1204
{const char*_tmp83A;s=Cyc_Absynpp_group_braces(((_tmp83A=",",_tag_dyneither(_tmp83A,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2C7));}
# 1205
goto _LL19F;_LL1E2: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp2C8=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp2C8->tag != 29)goto _LL1E4;else{_tmp2C9=_tmp2C8->f1;_tmp2CA=_tmp2C8->f3;}}_LL1E3:
# 1207
 if(_tmp2C9 == 0)
# 1209
s=Cyc_Absynpp_qvar2doc(_tmp2CA->name);else{
# 1211
const char*_tmp841;const char*_tmp840;const char*_tmp83F;struct Cyc_PP_Doc*_tmp83E[2];s=((_tmp83E[1]=
# 1212
Cyc_PP_egroup(((_tmp841="(",_tag_dyneither(_tmp841,sizeof(char),2))),((_tmp840=")",_tag_dyneither(_tmp840,sizeof(char),2))),((_tmp83F=",",_tag_dyneither(_tmp83F,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp2C9)),((_tmp83E[0]=
# 1211
Cyc_Absynpp_qvar2doc(_tmp2CA->name),Cyc_PP_cat(_tag_dyneither(_tmp83E,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1213
goto _LL19F;_LL1E4: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp2CB=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp2CB->tag != 30)goto _LL1E6;else{_tmp2CC=_tmp2CB->f1;}}_LL1E5:
# 1215
 s=Cyc_Absynpp_qvar2doc(_tmp2CC);
# 1216
goto _LL19F;_LL1E6: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp2CD=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp2CD->tag != 31)goto _LL1E8;else{_tmp2CE=_tmp2CD->f1;}}_LL1E7:
# 1218
 s=Cyc_Absynpp_qvar2doc(_tmp2CE);
# 1219
goto _LL19F;_LL1E8: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp2CF=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp2CF->tag != 32)goto _LL1EA;else{_tmp2D0=_tmp2CF->f1;_tmp2D1=_tmp2D0.is_calloc;_tmp2D2=_tmp2D0.rgn;_tmp2D3=_tmp2D0.elt_type;_tmp2D4=_tmp2D0.num_elts;}}_LL1E9:
# 1223
 if(_tmp2D1){
# 1225
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp2D3)),0);
# 1226
if(_tmp2D2 == 0){
# 1227
const char*_tmp848;const char*_tmp847;const char*_tmp846;struct Cyc_PP_Doc*_tmp845[5];s=((_tmp845[4]=Cyc_PP_text(((_tmp846=")",_tag_dyneither(_tmp846,sizeof(char),2)))),((_tmp845[3]=Cyc_Absynpp_exp2doc(st),((_tmp845[2]=Cyc_PP_text(((_tmp847=",",_tag_dyneither(_tmp847,sizeof(char),2)))),((_tmp845[1]=Cyc_Absynpp_exp2doc(_tmp2D4),((_tmp845[0]=Cyc_PP_text(((_tmp848="calloc(",_tag_dyneither(_tmp848,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp845,sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{
# 1229
const char*_tmp851;const char*_tmp850;const char*_tmp84F;const char*_tmp84E;struct Cyc_PP_Doc*_tmp84D[7];s=((_tmp84D[6]=
# 1230
Cyc_PP_text(((_tmp84E=")",_tag_dyneither(_tmp84E,sizeof(char),2)))),((_tmp84D[5]=Cyc_Absynpp_exp2doc(st),((_tmp84D[4]=Cyc_PP_text(((_tmp84F=",",_tag_dyneither(_tmp84F,sizeof(char),2)))),((_tmp84D[3]=Cyc_Absynpp_exp2doc(_tmp2D4),((_tmp84D[2]=
# 1229
Cyc_PP_text(((_tmp850=",",_tag_dyneither(_tmp850,sizeof(char),2)))),((_tmp84D[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2D2),((_tmp84D[0]=Cyc_PP_text(((_tmp851="rcalloc(",_tag_dyneither(_tmp851,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp84D,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1232
struct Cyc_Absyn_Exp*new_e;
# 1234
if(_tmp2D3 == 0)
# 1235
new_e=_tmp2D4;else{
# 1237
new_e=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp2D3,0),_tmp2D4,0);}
# 1239
if(_tmp2D2 == 0){
# 1240
const char*_tmp856;const char*_tmp855;struct Cyc_PP_Doc*_tmp854[3];s=((_tmp854[2]=Cyc_PP_text(((_tmp855=")",_tag_dyneither(_tmp855,sizeof(char),2)))),((_tmp854[1]=Cyc_Absynpp_exp2doc(new_e),((_tmp854[0]=Cyc_PP_text(((_tmp856="malloc(",_tag_dyneither(_tmp856,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp854,sizeof(struct Cyc_PP_Doc*),3))))))));}else{
# 1242
const char*_tmp85D;const char*_tmp85C;const char*_tmp85B;struct Cyc_PP_Doc*_tmp85A[5];s=((_tmp85A[4]=
# 1243
Cyc_PP_text(((_tmp85B=")",_tag_dyneither(_tmp85B,sizeof(char),2)))),((_tmp85A[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp85A[2]=
# 1242
Cyc_PP_text(((_tmp85C=",",_tag_dyneither(_tmp85C,sizeof(char),2)))),((_tmp85A[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2D2),((_tmp85A[0]=Cyc_PP_text(((_tmp85D="rmalloc(",_tag_dyneither(_tmp85D,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp85A,sizeof(struct Cyc_PP_Doc*),5))))))))))));}}
# 1245
goto _LL19F;_LL1EA: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp2D5=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp2D5->tag != 33)goto _LL1EC;else{_tmp2D6=_tmp2D5->f1;_tmp2D7=_tmp2D5->f2;}}_LL1EB:
# 1247
{const char*_tmp860;struct Cyc_PP_Doc*_tmp85F[3];s=((_tmp85F[2]=
# 1249
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D7),((_tmp85F[1]=
# 1248
Cyc_PP_text(((_tmp860=" :=: ",_tag_dyneither(_tmp860,sizeof(char),6)))),((_tmp85F[0]=
# 1247
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D6),Cyc_PP_cat(_tag_dyneither(_tmp85F,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1250
goto _LL19F;_LL1EC: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp2D8=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp2D8->tag != 34)goto _LL1EE;else{_tmp2D9=_tmp2D8->f1;_tmp2DA=_tmp2D8->f2;}}_LL1ED:
# 1253
{const char*_tmp861;s=Cyc_Absynpp_group_braces(((_tmp861=",",_tag_dyneither(_tmp861,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2DA));}
# 1254
goto _LL19F;_LL1EE: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp2DB=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp2DB->tag != 35)goto _LL19F;else{_tmp2DC=_tmp2DB->f1;}}_LL1EF:
# 1256
{const char*_tmp866;const char*_tmp865;struct Cyc_PP_Doc*_tmp864[7];s=((_tmp864[6]=
# 1258
Cyc_PP_text(((_tmp865=")",_tag_dyneither(_tmp865,sizeof(char),2)))),((_tmp864[5]=Cyc_Absynpp_rb(),((_tmp864[4]=Cyc_PP_blank_doc(),((_tmp864[3]=
# 1257
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2DC,1)),((_tmp864[2]=
# 1256
Cyc_PP_blank_doc(),((_tmp864[1]=Cyc_Absynpp_lb(),((_tmp864[0]=Cyc_PP_text(((_tmp866="(",_tag_dyneither(_tmp866,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp864,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1259
goto _LL19F;_LL19F:;}
# 1261
if(inprec >= myprec){
# 1262
const char*_tmp86B;const char*_tmp86A;struct Cyc_PP_Doc*_tmp869[3];s=((_tmp869[2]=Cyc_PP_text(((_tmp86A=")",_tag_dyneither(_tmp86A,sizeof(char),2)))),((_tmp869[1]=s,((_tmp869[0]=Cyc_PP_text(((_tmp86B="(",_tag_dyneither(_tmp86B,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp869,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1263
return s;}
# 1266
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
# 1267
void*_tmp359=d;struct Cyc_Absyn_Exp*_tmp35B;struct _dyneither_ptr*_tmp35D;_LL1F6: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp35A=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp359;if(_tmp35A->tag != 0)goto _LL1F8;else{_tmp35B=_tmp35A->f1;}}_LL1F7: {
# 1268
const char*_tmp870;const char*_tmp86F;struct Cyc_PP_Doc*_tmp86E[3];return(_tmp86E[2]=Cyc_PP_text(((_tmp86F="]",_tag_dyneither(_tmp86F,sizeof(char),2)))),((_tmp86E[1]=Cyc_Absynpp_exp2doc(_tmp35B),((_tmp86E[0]=Cyc_PP_text(((_tmp870=".[",_tag_dyneither(_tmp870,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp86E,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL1F8: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp35C=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp359;if(_tmp35C->tag != 1)goto _LL1F5;else{_tmp35D=_tmp35C->f1;}}_LL1F9: {
# 1269
const char*_tmp873;struct Cyc_PP_Doc*_tmp872[2];return(_tmp872[1]=Cyc_PP_textptr(_tmp35D),((_tmp872[0]=Cyc_PP_text(((_tmp873=".",_tag_dyneither(_tmp873,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp872,sizeof(struct Cyc_PP_Doc*),2)))));}_LL1F5:;}
# 1273
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de){
# 1274
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
# 1275
const char*_tmp87A;const char*_tmp879;const char*_tmp878;struct Cyc_PP_Doc*_tmp877[2];return(_tmp877[1]=
# 1276
Cyc_Absynpp_exp2doc((*de).f2),((_tmp877[0]=
# 1275
Cyc_PP_egroup(((_tmp87A="",_tag_dyneither(_tmp87A,sizeof(char),1))),((_tmp879="=",_tag_dyneither(_tmp879,sizeof(char),2))),((_tmp878="=",_tag_dyneither(_tmp878,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1)),Cyc_PP_cat(_tag_dyneither(_tmp877,sizeof(struct Cyc_PP_Doc*),2)))));}}
# 1279
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
# 1280
const char*_tmp87D;const char*_tmp87C;const char*_tmp87B;return Cyc_PP_group(((_tmp87B="",_tag_dyneither(_tmp87B,sizeof(char),1))),((_tmp87C="",_tag_dyneither(_tmp87C,sizeof(char),1))),((_tmp87D=",",_tag_dyneither(_tmp87D,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}
# 1284
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
# 1285
union Cyc_Absyn_Cnst _tmp36A=c;struct _tuple3 _tmp36B;enum Cyc_Absyn_Sign _tmp36C;char _tmp36D;struct _dyneither_ptr _tmp36E;struct _tuple4 _tmp36F;enum Cyc_Absyn_Sign _tmp370;short _tmp371;struct _tuple5 _tmp372;enum Cyc_Absyn_Sign _tmp373;int _tmp374;struct _tuple5 _tmp375;enum Cyc_Absyn_Sign _tmp376;int _tmp377;struct _tuple5 _tmp378;enum Cyc_Absyn_Sign _tmp379;int _tmp37A;struct _tuple6 _tmp37B;enum Cyc_Absyn_Sign _tmp37C;long long _tmp37D;struct _tuple7 _tmp37E;struct _dyneither_ptr _tmp37F;int _tmp380;struct _dyneither_ptr _tmp381;struct _dyneither_ptr _tmp382;_LL1FB: if((_tmp36A.Char_c).tag != 2)goto _LL1FD;_tmp36B=(struct _tuple3)(_tmp36A.Char_c).val;_tmp36C=_tmp36B.f1;_tmp36D=_tmp36B.f2;_LL1FC: {
# 1286
const char*_tmp881;void*_tmp880[1];struct Cyc_String_pa_PrintArg_struct _tmp87F;return Cyc_PP_text((struct _dyneither_ptr)((_tmp87F.tag=0,((_tmp87F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp36D)),((_tmp880[0]=& _tmp87F,Cyc_aprintf(((_tmp881="'%s'",_tag_dyneither(_tmp881,sizeof(char),5))),_tag_dyneither(_tmp880,sizeof(void*),1)))))))));}_LL1FD: if((_tmp36A.Wchar_c).tag != 3)goto _LL1FF;_tmp36E=(struct _dyneither_ptr)(_tmp36A.Wchar_c).val;_LL1FE: {
# 1287
const char*_tmp885;void*_tmp884[1];struct Cyc_String_pa_PrintArg_struct _tmp883;return Cyc_PP_text((struct _dyneither_ptr)((_tmp883.tag=0,((_tmp883.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp36E),((_tmp884[0]=& _tmp883,Cyc_aprintf(((_tmp885="L'%s'",_tag_dyneither(_tmp885,sizeof(char),6))),_tag_dyneither(_tmp884,sizeof(void*),1)))))))));}_LL1FF: if((_tmp36A.Short_c).tag != 4)goto _LL201;_tmp36F=(struct _tuple4)(_tmp36A.Short_c).val;_tmp370=_tmp36F.f1;_tmp371=_tmp36F.f2;_LL200: {
# 1288
const char*_tmp889;void*_tmp888[1];struct Cyc_Int_pa_PrintArg_struct _tmp887;return Cyc_PP_text((struct _dyneither_ptr)((_tmp887.tag=1,((_tmp887.f1=(unsigned long)((int)_tmp371),((_tmp888[0]=& _tmp887,Cyc_aprintf(((_tmp889="%d",_tag_dyneither(_tmp889,sizeof(char),3))),_tag_dyneither(_tmp888,sizeof(void*),1)))))))));}_LL201: if((_tmp36A.Int_c).tag != 5)goto _LL203;_tmp372=(struct _tuple5)(_tmp36A.Int_c).val;_tmp373=_tmp372.f1;if(_tmp373 != Cyc_Absyn_None)goto _LL203;_tmp374=_tmp372.f2;_LL202:
# 1289
 _tmp377=_tmp374;goto _LL204;_LL203: if((_tmp36A.Int_c).tag != 5)goto _LL205;_tmp375=(struct _tuple5)(_tmp36A.Int_c).val;_tmp376=_tmp375.f1;if(_tmp376 != Cyc_Absyn_Signed)goto _LL205;_tmp377=_tmp375.f2;_LL204: {
# 1290
const char*_tmp88D;void*_tmp88C[1];struct Cyc_Int_pa_PrintArg_struct _tmp88B;return Cyc_PP_text((struct _dyneither_ptr)((_tmp88B.tag=1,((_tmp88B.f1=(unsigned long)_tmp377,((_tmp88C[0]=& _tmp88B,Cyc_aprintf(((_tmp88D="%d",_tag_dyneither(_tmp88D,sizeof(char),3))),_tag_dyneither(_tmp88C,sizeof(void*),1)))))))));}_LL205: if((_tmp36A.Int_c).tag != 5)goto _LL207;_tmp378=(struct _tuple5)(_tmp36A.Int_c).val;_tmp379=_tmp378.f1;if(_tmp379 != Cyc_Absyn_Unsigned)goto _LL207;_tmp37A=_tmp378.f2;_LL206: {
# 1291
const char*_tmp891;void*_tmp890[1];struct Cyc_Int_pa_PrintArg_struct _tmp88F;return Cyc_PP_text((struct _dyneither_ptr)((_tmp88F.tag=1,((_tmp88F.f1=(unsigned int)_tmp37A,((_tmp890[0]=& _tmp88F,Cyc_aprintf(((_tmp891="%u",_tag_dyneither(_tmp891,sizeof(char),3))),_tag_dyneither(_tmp890,sizeof(void*),1)))))))));}_LL207: if((_tmp36A.LongLong_c).tag != 6)goto _LL209;_tmp37B=(struct _tuple6)(_tmp36A.LongLong_c).val;_tmp37C=_tmp37B.f1;_tmp37D=_tmp37B.f2;_LL208: {
# 1293
const char*_tmp892;return Cyc_PP_text(((_tmp892="<<FIX LONG LONG CONSTANT>>",_tag_dyneither(_tmp892,sizeof(char),27))));}_LL209: if((_tmp36A.Float_c).tag != 7)goto _LL20B;_tmp37E=(struct _tuple7)(_tmp36A.Float_c).val;_tmp37F=_tmp37E.f1;_LL20A:
# 1294
 return Cyc_PP_text(_tmp37F);_LL20B: if((_tmp36A.Null_c).tag != 1)goto _LL20D;_tmp380=(int)(_tmp36A.Null_c).val;_LL20C: {
# 1295
const char*_tmp893;return Cyc_PP_text(((_tmp893="NULL",_tag_dyneither(_tmp893,sizeof(char),5))));}_LL20D: if((_tmp36A.String_c).tag != 8)goto _LL20F;_tmp381=(struct _dyneither_ptr)(_tmp36A.String_c).val;_LL20E: {
# 1297
const char*_tmp898;const char*_tmp897;struct Cyc_PP_Doc*_tmp896[3];return(_tmp896[2]=Cyc_PP_text(((_tmp897="\"",_tag_dyneither(_tmp897,sizeof(char),2)))),((_tmp896[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp381)),((_tmp896[0]=Cyc_PP_text(((_tmp898="\"",_tag_dyneither(_tmp898,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp896,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL20F: if((_tmp36A.Wstring_c).tag != 9)goto _LL1FA;_tmp382=(struct _dyneither_ptr)(_tmp36A.Wstring_c).val;_LL210: {
# 1299
const char*_tmp89D;const char*_tmp89C;struct Cyc_PP_Doc*_tmp89B[3];return(_tmp89B[2]=Cyc_PP_text(((_tmp89C="\"",_tag_dyneither(_tmp89C,sizeof(char),2)))),((_tmp89B[1]=Cyc_PP_text(_tmp382),((_tmp89B[0]=Cyc_PP_text(((_tmp89D="L\"",_tag_dyneither(_tmp89D,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp89B,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL1FA:;}
# 1303
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1304
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
# 1305
if(p == (enum Cyc_Absyn_Primop)Cyc_Absyn_Numelts){
# 1306
if(es == 0  || es->tl != 0){
# 1307
struct Cyc_Core_Failure_exn_struct _tmp8AA;const char*_tmp8A9;void*_tmp8A8[1];struct Cyc_String_pa_PrintArg_struct _tmp8A7;struct Cyc_Core_Failure_exn_struct*_tmp8A6;(int)_throw((void*)((_tmp8A6=_cycalloc(sizeof(*_tmp8A6)),((_tmp8A6[0]=((_tmp8AA.tag=Cyc_Core_Failure,((_tmp8AA.f1=(struct _dyneither_ptr)((_tmp8A7.tag=0,((_tmp8A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp8A8[0]=& _tmp8A7,Cyc_aprintf(((_tmp8A9="Absynpp::primapp2doc Numelts: %s with bad args",_tag_dyneither(_tmp8A9,sizeof(char),47))),_tag_dyneither(_tmp8A8,sizeof(void*),1)))))))),_tmp8AA)))),_tmp8A6)))));}{
# 1309
const char*_tmp8AF;const char*_tmp8AE;struct Cyc_PP_Doc*_tmp8AD[3];return(_tmp8AD[2]=Cyc_PP_text(((_tmp8AE=")",_tag_dyneither(_tmp8AE,sizeof(char),2)))),((_tmp8AD[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((_tmp8AD[0]=Cyc_PP_text(((_tmp8AF="numelts(",_tag_dyneither(_tmp8AF,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp8AD,sizeof(struct Cyc_PP_Doc*),3)))))));};}else{
# 1311
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
# 1312
if(ds == 0){
# 1313
struct Cyc_Core_Failure_exn_struct _tmp8BC;const char*_tmp8BB;void*_tmp8BA[1];struct Cyc_String_pa_PrintArg_struct _tmp8B9;struct Cyc_Core_Failure_exn_struct*_tmp8B8;(int)_throw((void*)((_tmp8B8=_cycalloc(sizeof(*_tmp8B8)),((_tmp8B8[0]=((_tmp8BC.tag=Cyc_Core_Failure,((_tmp8BC.f1=(struct _dyneither_ptr)((_tmp8B9.tag=0,((_tmp8B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp8BA[0]=& _tmp8B9,Cyc_aprintf(((_tmp8BB="Absynpp::primapp2doc: %s with no args",_tag_dyneither(_tmp8BB,sizeof(char),38))),_tag_dyneither(_tmp8BA,sizeof(void*),1)))))))),_tmp8BC)))),_tmp8B8)))));}else{
# 1315
if(ds->tl == 0){
# 1316
const char*_tmp8BF;struct Cyc_PP_Doc*_tmp8BE[3];return(_tmp8BE[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp8BE[1]=Cyc_PP_text(((_tmp8BF=" ",_tag_dyneither(_tmp8BF,sizeof(char),2)))),((_tmp8BE[0]=ps,Cyc_PP_cat(_tag_dyneither(_tmp8BE,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
# 1317
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0){
# 1318
struct Cyc_Core_Failure_exn_struct _tmp8CC;const char*_tmp8CB;void*_tmp8CA[1];struct Cyc_String_pa_PrintArg_struct _tmp8C9;struct Cyc_Core_Failure_exn_struct*_tmp8C8;(int)_throw((void*)((_tmp8C8=_cycalloc(sizeof(*_tmp8C8)),((_tmp8C8[0]=((_tmp8CC.tag=Cyc_Core_Failure,((_tmp8CC.f1=(struct _dyneither_ptr)((_tmp8C9.tag=0,((_tmp8C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp8CA[0]=& _tmp8C9,Cyc_aprintf(((_tmp8CB="Absynpp::primapp2doc: %s with more than 2 args",_tag_dyneither(_tmp8CB,sizeof(char),47))),_tag_dyneither(_tmp8CA,sizeof(void*),1)))))))),_tmp8CC)))),_tmp8C8)))));}else{
# 1321
const char*_tmp8D1;const char*_tmp8D0;struct Cyc_PP_Doc*_tmp8CF[5];return(_tmp8CF[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd,((_tmp8CF[3]=Cyc_PP_text(((_tmp8D0=" ",_tag_dyneither(_tmp8D0,sizeof(char),2)))),((_tmp8CF[2]=ps,((_tmp8CF[1]=Cyc_PP_text(((_tmp8D1=" ",_tag_dyneither(_tmp8D1,sizeof(char),2)))),((_tmp8CF[0]=(struct Cyc_PP_Doc*)ds->hd,Cyc_PP_cat(_tag_dyneither(_tmp8CF,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
# 1325
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
# 1326
switch(p){case Cyc_Absyn_Plus: _LL211: {
# 1327
const char*_tmp8D2;return(_tmp8D2="+",_tag_dyneither(_tmp8D2,sizeof(char),2));}case Cyc_Absyn_Times: _LL212: {
# 1328
const char*_tmp8D3;return(_tmp8D3="*",_tag_dyneither(_tmp8D3,sizeof(char),2));}case Cyc_Absyn_Minus: _LL213: {
# 1329
const char*_tmp8D4;return(_tmp8D4="-",_tag_dyneither(_tmp8D4,sizeof(char),2));}case Cyc_Absyn_Div: _LL214: {
# 1330
const char*_tmp8D5;return(_tmp8D5="/",_tag_dyneither(_tmp8D5,sizeof(char),2));}case Cyc_Absyn_Mod: _LL215: {
# 1331
const char*_tmp8D7;const char*_tmp8D6;return Cyc_Absynpp_print_for_cycdoc?(_tmp8D7="\\%",_tag_dyneither(_tmp8D7,sizeof(char),3)):((_tmp8D6="%",_tag_dyneither(_tmp8D6,sizeof(char),2)));}case Cyc_Absyn_Eq: _LL216: {
# 1332
const char*_tmp8D8;return(_tmp8D8="==",_tag_dyneither(_tmp8D8,sizeof(char),3));}case Cyc_Absyn_Neq: _LL217: {
# 1333
const char*_tmp8D9;return(_tmp8D9="!=",_tag_dyneither(_tmp8D9,sizeof(char),3));}case Cyc_Absyn_Gt: _LL218: {
# 1334
const char*_tmp8DA;return(_tmp8DA=">",_tag_dyneither(_tmp8DA,sizeof(char),2));}case Cyc_Absyn_Lt: _LL219: {
# 1335
const char*_tmp8DB;return(_tmp8DB="<",_tag_dyneither(_tmp8DB,sizeof(char),2));}case Cyc_Absyn_Gte: _LL21A: {
# 1336
const char*_tmp8DC;return(_tmp8DC=">=",_tag_dyneither(_tmp8DC,sizeof(char),3));}case Cyc_Absyn_Lte: _LL21B: {
# 1337
const char*_tmp8DD;return(_tmp8DD="<=",_tag_dyneither(_tmp8DD,sizeof(char),3));}case Cyc_Absyn_Not: _LL21C: {
# 1338
const char*_tmp8DE;return(_tmp8DE="!",_tag_dyneither(_tmp8DE,sizeof(char),2));}case Cyc_Absyn_Bitnot: _LL21D: {
# 1339
const char*_tmp8DF;return(_tmp8DF="~",_tag_dyneither(_tmp8DF,sizeof(char),2));}case Cyc_Absyn_Bitand: _LL21E: {
# 1340
const char*_tmp8E0;return(_tmp8E0="&",_tag_dyneither(_tmp8E0,sizeof(char),2));}case Cyc_Absyn_Bitor: _LL21F: {
# 1341
const char*_tmp8E1;return(_tmp8E1="|",_tag_dyneither(_tmp8E1,sizeof(char),2));}case Cyc_Absyn_Bitxor: _LL220: {
# 1342
const char*_tmp8E2;return(_tmp8E2="^",_tag_dyneither(_tmp8E2,sizeof(char),2));}case Cyc_Absyn_Bitlshift: _LL221: {
# 1343
const char*_tmp8E3;return(_tmp8E3="<<",_tag_dyneither(_tmp8E3,sizeof(char),3));}case Cyc_Absyn_Bitlrshift: _LL222: {
# 1344
const char*_tmp8E4;return(_tmp8E4=">>",_tag_dyneither(_tmp8E4,sizeof(char),3));}case Cyc_Absyn_Bitarshift: _LL223: {
# 1345
const char*_tmp8E5;return(_tmp8E5=">>>",_tag_dyneither(_tmp8E5,sizeof(char),4));}case Cyc_Absyn_Numelts: _LL224: {
# 1346
const char*_tmp8E6;return(_tmp8E6="numelts",_tag_dyneither(_tmp8E6,sizeof(char),8));}}}
# 1350
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
# 1351
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1354
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
# 1355
void*_tmp3C6=s->r;_LL227: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3C7=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3C6;if(_tmp3C7->tag != 12)goto _LL229;}_LL228:
# 1356
 return 1;_LL229:;_LL22A:
# 1357
 return 0;_LL226:;}
# 1361
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp){
# 1362
struct Cyc_PP_Doc*s;
# 1363
{void*_tmp3C8=st->r;struct Cyc_Absyn_Exp*_tmp3CB;struct Cyc_Absyn_Stmt*_tmp3CD;struct Cyc_Absyn_Stmt*_tmp3CE;struct Cyc_Absyn_Exp*_tmp3D0;struct Cyc_Absyn_Exp*_tmp3D2;struct Cyc_Absyn_Stmt*_tmp3D3;struct Cyc_Absyn_Stmt*_tmp3D4;struct _tuple9 _tmp3D6;struct Cyc_Absyn_Exp*_tmp3D7;struct Cyc_Absyn_Stmt*_tmp3D8;struct _dyneither_ptr*_tmp3DC;struct Cyc_Absyn_Exp*_tmp3DE;struct _tuple9 _tmp3DF;struct Cyc_Absyn_Exp*_tmp3E0;struct _tuple9 _tmp3E1;struct Cyc_Absyn_Exp*_tmp3E2;struct Cyc_Absyn_Stmt*_tmp3E3;struct Cyc_Absyn_Exp*_tmp3E5;struct Cyc_List_List*_tmp3E6;struct Cyc_List_List*_tmp3E8;struct Cyc_List_List*_tmp3EA;struct Cyc_Absyn_Decl*_tmp3EC;struct Cyc_Absyn_Stmt*_tmp3ED;struct _dyneither_ptr*_tmp3EF;struct Cyc_Absyn_Stmt*_tmp3F0;struct Cyc_Absyn_Stmt*_tmp3F2;struct _tuple9 _tmp3F3;struct Cyc_Absyn_Exp*_tmp3F4;struct Cyc_Absyn_Stmt*_tmp3F6;struct Cyc_List_List*_tmp3F7;struct Cyc_Absyn_Exp*_tmp3F9;_LL22C: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp3C9=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp3C8;if(_tmp3C9->tag != 0)goto _LL22E;}_LL22D:
# 1364
{const char*_tmp8E7;s=Cyc_PP_text(((_tmp8E7=";",_tag_dyneither(_tmp8E7,sizeof(char),2))));}goto _LL22B;_LL22E: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3CA=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3C8;if(_tmp3CA->tag != 1)goto _LL230;else{_tmp3CB=_tmp3CA->f1;}}_LL22F:
# 1365
{const char*_tmp8EA;struct Cyc_PP_Doc*_tmp8E9[2];s=((_tmp8E9[1]=Cyc_PP_text(((_tmp8EA=";",_tag_dyneither(_tmp8EA,sizeof(char),2)))),((_tmp8E9[0]=Cyc_Absynpp_exp2doc(_tmp3CB),Cyc_PP_cat(_tag_dyneither(_tmp8E9,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL22B;_LL230: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp3CC=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3C8;if(_tmp3CC->tag != 2)goto _LL232;else{_tmp3CD=_tmp3CC->f1;_tmp3CE=_tmp3CC->f2;}}_LL231:
# 1367
 if(Cyc_Absynpp_decls_first){
# 1368
if(Cyc_Absynpp_is_declaration(_tmp3CD)){
# 1369
struct Cyc_PP_Doc*_tmp8F5[5];struct Cyc_PP_Doc*_tmp8F4[7];const char*_tmp8F3;const char*_tmp8F2;struct Cyc_PP_Doc*_tmp8F1[7];s=((_tmp8F1[6]=
# 1375
Cyc_Absynpp_is_declaration(_tmp3CE)?
# 1376
stmtexp?
# 1377
(_tmp8F4[6]=
# 1380
Cyc_PP_line_doc(),((_tmp8F4[5]=
# 1379
Cyc_PP_text(((_tmp8F3=");",_tag_dyneither(_tmp8F3,sizeof(char),3)))),((_tmp8F4[4]=Cyc_Absynpp_rb(),((_tmp8F4[3]=
# 1378
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3CE,stmtexp)),((_tmp8F4[2]=
# 1377
Cyc_PP_blank_doc(),((_tmp8F4[1]=Cyc_Absynpp_lb(),((_tmp8F4[0]=Cyc_PP_text(((_tmp8F2="(",_tag_dyneither(_tmp8F2,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8F4,sizeof(struct Cyc_PP_Doc*),7))))))))))))))):(
# 1381
(_tmp8F5[4]=
# 1384
Cyc_PP_line_doc(),((_tmp8F5[3]=
# 1383
Cyc_Absynpp_rb(),((_tmp8F5[2]=
# 1382
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3CE,stmtexp)),((_tmp8F5[1]=
# 1381
Cyc_PP_blank_doc(),((_tmp8F5[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8F5,sizeof(struct Cyc_PP_Doc*),5)))))))))))):
# 1385
 Cyc_Absynpp_stmt2doc(_tmp3CE,stmtexp),((_tmp8F1[5]=
# 1374
Cyc_PP_line_doc(),((_tmp8F1[4]=
# 1373
Cyc_Absynpp_rb(),((_tmp8F1[3]=
# 1372
Cyc_PP_line_doc(),((_tmp8F1[2]=
# 1371
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3CD,0)),((_tmp8F1[1]=
# 1370
Cyc_PP_blank_doc(),((_tmp8F1[0]=
# 1369
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8F1,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1386
if(Cyc_Absynpp_is_declaration(_tmp3CE)){
# 1387
struct Cyc_PP_Doc*_tmp900[4];struct Cyc_PP_Doc*_tmp8FF[6];const char*_tmp8FE;const char*_tmp8FD;struct Cyc_PP_Doc*_tmp8FC[4];s=((_tmp8FC[3]=
# 1397
Cyc_PP_line_doc(),((_tmp8FC[2]=
# 1389
stmtexp?
# 1390
(_tmp8FF[5]=
# 1392
Cyc_PP_text(((_tmp8FE=");",_tag_dyneither(_tmp8FE,sizeof(char),3)))),((_tmp8FF[4]=Cyc_Absynpp_rb(),((_tmp8FF[3]=
# 1391
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3CE,stmtexp)),((_tmp8FF[2]=
# 1390
Cyc_PP_blank_doc(),((_tmp8FF[1]=Cyc_Absynpp_lb(),((_tmp8FF[0]=Cyc_PP_text(((_tmp8FD="(",_tag_dyneither(_tmp8FD,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8FF,sizeof(struct Cyc_PP_Doc*),6))))))))))))):(
# 1394
(_tmp900[3]=
# 1396
Cyc_Absynpp_rb(),((_tmp900[2]=
# 1395
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3CE,stmtexp)),((_tmp900[1]=
# 1394
Cyc_PP_blank_doc(),((_tmp900[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp900,sizeof(struct Cyc_PP_Doc*),4)))))))))),((_tmp8FC[1]=
# 1388
Cyc_PP_line_doc(),((_tmp8FC[0]=
# 1387
Cyc_Absynpp_stmt2doc(_tmp3CD,0),Cyc_PP_cat(_tag_dyneither(_tmp8FC,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1399
struct Cyc_PP_Doc*_tmp901[3];s=((_tmp901[2]=Cyc_Absynpp_stmt2doc(_tmp3CE,stmtexp),((_tmp901[1]=Cyc_PP_line_doc(),((_tmp901[0]=Cyc_Absynpp_stmt2doc(_tmp3CD,0),Cyc_PP_cat(_tag_dyneither(_tmp901,sizeof(struct Cyc_PP_Doc*),3))))))));}}}else{
# 1402
struct Cyc_PP_Doc*_tmp902[3];s=((_tmp902[2]=Cyc_Absynpp_stmt2doc(_tmp3CE,stmtexp),((_tmp902[1]=Cyc_PP_line_doc(),((_tmp902[0]=Cyc_Absynpp_stmt2doc(_tmp3CD,0),Cyc_PP_cat(_tag_dyneither(_tmp902,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1403
goto _LL22B;_LL232: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp3CF=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp3C8;if(_tmp3CF->tag != 3)goto _LL234;else{_tmp3D0=_tmp3CF->f1;}}_LL233:
# 1405
 if(_tmp3D0 == 0){
# 1406
const char*_tmp903;s=Cyc_PP_text(((_tmp903="return;",_tag_dyneither(_tmp903,sizeof(char),8))));}else{
# 1408
const char*_tmp908;const char*_tmp907;struct Cyc_PP_Doc*_tmp906[3];s=((_tmp906[2]=
# 1410
Cyc_PP_text(((_tmp907=";",_tag_dyneither(_tmp907,sizeof(char),2)))),((_tmp906[1]=
# 1409
_tmp3D0 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp3D0),((_tmp906[0]=
# 1408
Cyc_PP_text(((_tmp908="return ",_tag_dyneither(_tmp908,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp906,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1411
goto _LL22B;_LL234: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp3D1=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3C8;if(_tmp3D1->tag != 4)goto _LL236;else{_tmp3D2=_tmp3D1->f1;_tmp3D3=_tmp3D1->f2;_tmp3D4=_tmp3D1->f3;}}_LL235: {
# 1413
int print_else;
# 1414
{void*_tmp40D=_tmp3D4->r;_LL251: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp40E=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp40D;if(_tmp40E->tag != 0)goto _LL253;}_LL252:
# 1415
 print_else=0;goto _LL250;_LL253:;_LL254:
# 1416
 print_else=1;goto _LL250;_LL250:;}
# 1418
{const char*_tmp917;const char*_tmp916;struct Cyc_PP_Doc*_tmp915[2];struct Cyc_PP_Doc*_tmp914[6];struct Cyc_PP_Doc*_tmp913[2];const char*_tmp912;struct Cyc_PP_Doc*_tmp911[8];s=((_tmp911[7]=
# 1425
print_else?
# 1426
(_tmp914[5]=
# 1431
Cyc_Absynpp_rb(),((_tmp914[4]=
# 1430
Cyc_PP_line_doc(),((_tmp914[3]=
# 1429
Cyc_PP_nest(2,((_tmp913[1]=Cyc_Absynpp_stmt2doc(_tmp3D4,0),((_tmp913[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp913,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp914[2]=
# 1428
Cyc_Absynpp_lb(),((_tmp914[1]=
# 1427
Cyc_PP_text(((_tmp912="else ",_tag_dyneither(_tmp912,sizeof(char),6)))),((_tmp914[0]=
# 1426
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp914,sizeof(struct Cyc_PP_Doc*),6))))))))))))):
# 1432
 Cyc_PP_nil_doc(),((_tmp911[6]=
# 1424
Cyc_Absynpp_rb(),((_tmp911[5]=
# 1423
Cyc_PP_line_doc(),((_tmp911[4]=
# 1422
Cyc_PP_nest(2,((_tmp915[1]=Cyc_Absynpp_stmt2doc(_tmp3D3,0),((_tmp915[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp915,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp911[3]=
# 1421
Cyc_Absynpp_lb(),((_tmp911[2]=
# 1420
Cyc_PP_text(((_tmp916=") ",_tag_dyneither(_tmp916,sizeof(char),3)))),((_tmp911[1]=
# 1419
Cyc_Absynpp_exp2doc(_tmp3D2),((_tmp911[0]=
# 1418
Cyc_PP_text(((_tmp917="if (",_tag_dyneither(_tmp917,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp911,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1433
goto _LL22B;}_LL236: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp3D5=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3C8;if(_tmp3D5->tag != 5)goto _LL238;else{_tmp3D6=_tmp3D5->f1;_tmp3D7=_tmp3D6.f1;_tmp3D8=_tmp3D5->f2;}}_LL237:
# 1435
{const char*_tmp91E;const char*_tmp91D;struct Cyc_PP_Doc*_tmp91C[2];struct Cyc_PP_Doc*_tmp91B[7];s=((_tmp91B[6]=
# 1441
Cyc_Absynpp_rb(),((_tmp91B[5]=
# 1440
Cyc_PP_line_doc(),((_tmp91B[4]=
# 1439
Cyc_PP_nest(2,((_tmp91C[1]=Cyc_Absynpp_stmt2doc(_tmp3D8,0),((_tmp91C[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp91C,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp91B[3]=
# 1438
Cyc_Absynpp_lb(),((_tmp91B[2]=
# 1437
Cyc_PP_text(((_tmp91D=") ",_tag_dyneither(_tmp91D,sizeof(char),3)))),((_tmp91B[1]=
# 1436
Cyc_Absynpp_exp2doc(_tmp3D7),((_tmp91B[0]=
# 1435
Cyc_PP_text(((_tmp91E="while (",_tag_dyneither(_tmp91E,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp91B,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1442
goto _LL22B;_LL238: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp3D9=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp3C8;if(_tmp3D9->tag != 6)goto _LL23A;}_LL239:
# 1443
{const char*_tmp91F;s=Cyc_PP_text(((_tmp91F="break;",_tag_dyneither(_tmp91F,sizeof(char),7))));}goto _LL22B;_LL23A: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp3DA=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp3C8;if(_tmp3DA->tag != 7)goto _LL23C;}_LL23B:
# 1444
{const char*_tmp920;s=Cyc_PP_text(((_tmp920="continue;",_tag_dyneither(_tmp920,sizeof(char),10))));}goto _LL22B;_LL23C: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp3DB=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp3C8;if(_tmp3DB->tag != 8)goto _LL23E;else{_tmp3DC=_tmp3DB->f1;}}_LL23D:
# 1445
{const char*_tmp924;void*_tmp923[1];struct Cyc_String_pa_PrintArg_struct _tmp922;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp922.tag=0,((_tmp922.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3DC),((_tmp923[0]=& _tmp922,Cyc_aprintf(((_tmp924="goto %s;",_tag_dyneither(_tmp924,sizeof(char),9))),_tag_dyneither(_tmp923,sizeof(void*),1)))))))));}goto _LL22B;_LL23E: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp3DD=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3C8;if(_tmp3DD->tag != 9)goto _LL240;else{_tmp3DE=_tmp3DD->f1;_tmp3DF=_tmp3DD->f2;_tmp3E0=_tmp3DF.f1;_tmp3E1=_tmp3DD->f3;_tmp3E2=_tmp3E1.f1;_tmp3E3=_tmp3DD->f4;}}_LL23F:
# 1447
{const char*_tmp92F;const char*_tmp92E;const char*_tmp92D;const char*_tmp92C;struct Cyc_PP_Doc*_tmp92B[2];struct Cyc_PP_Doc*_tmp92A[11];s=((_tmp92A[10]=
# 1457
Cyc_Absynpp_rb(),((_tmp92A[9]=
# 1456
Cyc_PP_line_doc(),((_tmp92A[8]=
# 1455
Cyc_PP_nest(2,((_tmp92B[1]=Cyc_Absynpp_stmt2doc(_tmp3E3,0),((_tmp92B[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp92B,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp92A[7]=
# 1454
Cyc_Absynpp_lb(),((_tmp92A[6]=
# 1453
Cyc_PP_text(((_tmp92C=") ",_tag_dyneither(_tmp92C,sizeof(char),3)))),((_tmp92A[5]=
# 1452
Cyc_Absynpp_exp2doc(_tmp3E2),((_tmp92A[4]=
# 1451
Cyc_PP_text(((_tmp92D="; ",_tag_dyneither(_tmp92D,sizeof(char),3)))),((_tmp92A[3]=
# 1450
Cyc_Absynpp_exp2doc(_tmp3E0),((_tmp92A[2]=
# 1449
Cyc_PP_text(((_tmp92E="; ",_tag_dyneither(_tmp92E,sizeof(char),3)))),((_tmp92A[1]=
# 1448
Cyc_Absynpp_exp2doc(_tmp3DE),((_tmp92A[0]=
# 1447
Cyc_PP_text(((_tmp92F="for(",_tag_dyneither(_tmp92F,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp92A,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1458
goto _LL22B;_LL240: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp3E4=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp3C8;if(_tmp3E4->tag != 10)goto _LL242;else{_tmp3E5=_tmp3E4->f1;_tmp3E6=_tmp3E4->f2;}}_LL241:
# 1460
{const char*_tmp934;const char*_tmp933;struct Cyc_PP_Doc*_tmp932[8];s=((_tmp932[7]=
# 1467
Cyc_Absynpp_rb(),((_tmp932[6]=
# 1466
Cyc_PP_line_doc(),((_tmp932[5]=
# 1465
Cyc_Absynpp_switchclauses2doc(_tmp3E6),((_tmp932[4]=
# 1464
Cyc_PP_line_doc(),((_tmp932[3]=
# 1463
Cyc_Absynpp_lb(),((_tmp932[2]=
# 1462
Cyc_PP_text(((_tmp933=") ",_tag_dyneither(_tmp933,sizeof(char),3)))),((_tmp932[1]=
# 1461
Cyc_Absynpp_exp2doc(_tmp3E5),((_tmp932[0]=
# 1460
Cyc_PP_text(((_tmp934="switch (",_tag_dyneither(_tmp934,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp932,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1468
goto _LL22B;_LL242: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp3E7=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3C8;if(_tmp3E7->tag != 11)goto _LL244;else{_tmp3E8=_tmp3E7->f1;if(_tmp3E8 != 0)goto _LL244;}}_LL243:
# 1469
{const char*_tmp935;s=Cyc_PP_text(((_tmp935="fallthru;",_tag_dyneither(_tmp935,sizeof(char),10))));}goto _LL22B;_LL244: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp3E9=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3C8;if(_tmp3E9->tag != 11)goto _LL246;else{_tmp3EA=_tmp3E9->f1;}}_LL245:
# 1471
{const char*_tmp93A;const char*_tmp939;struct Cyc_PP_Doc*_tmp938[3];s=((_tmp938[2]=Cyc_PP_text(((_tmp939=");",_tag_dyneither(_tmp939,sizeof(char),3)))),((_tmp938[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp3EA),((_tmp938[0]=Cyc_PP_text(((_tmp93A="fallthru(",_tag_dyneither(_tmp93A,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp938,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL22B;_LL246: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3EB=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3C8;if(_tmp3EB->tag != 12)goto _LL248;else{_tmp3EC=_tmp3EB->f1;_tmp3ED=_tmp3EB->f2;}}_LL247:
# 1473
{struct Cyc_PP_Doc*_tmp93B[3];s=((_tmp93B[2]=
# 1475
Cyc_Absynpp_stmt2doc(_tmp3ED,stmtexp),((_tmp93B[1]=
# 1474
Cyc_PP_line_doc(),((_tmp93B[0]=
# 1473
Cyc_Absynpp_decl2doc(_tmp3EC),Cyc_PP_cat(_tag_dyneither(_tmp93B,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1476
goto _LL22B;_LL248: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp3EE=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp3C8;if(_tmp3EE->tag != 13)goto _LL24A;else{_tmp3EF=_tmp3EE->f1;_tmp3F0=_tmp3EE->f2;}}_LL249:
# 1478
 if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3F0)){
# 1479
if(stmtexp){
# 1480
const char*_tmp940;const char*_tmp93F;struct Cyc_PP_Doc*_tmp93E[8];s=((_tmp93E[7]=
# 1486
Cyc_PP_text(((_tmp93F=");",_tag_dyneither(_tmp93F,sizeof(char),3)))),((_tmp93E[6]=Cyc_Absynpp_rb(),((_tmp93E[5]=
# 1485
Cyc_PP_line_doc(),((_tmp93E[4]=
# 1484
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3F0,1)),((_tmp93E[3]=
# 1483
Cyc_PP_line_doc(),((_tmp93E[2]=
# 1482
Cyc_Absynpp_lb(),((_tmp93E[1]=
# 1481
Cyc_PP_text(((_tmp940=": (",_tag_dyneither(_tmp940,sizeof(char),4)))),((_tmp93E[0]=
# 1480
Cyc_PP_textptr(_tmp3EF),Cyc_PP_cat(_tag_dyneither(_tmp93E,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1488
const char*_tmp943;struct Cyc_PP_Doc*_tmp942[7];s=((_tmp942[6]=
# 1494
Cyc_Absynpp_rb(),((_tmp942[5]=
# 1493
Cyc_PP_line_doc(),((_tmp942[4]=
# 1492
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3F0,0)),((_tmp942[3]=
# 1491
Cyc_PP_line_doc(),((_tmp942[2]=
# 1490
Cyc_Absynpp_lb(),((_tmp942[1]=
# 1489
Cyc_PP_text(((_tmp943=": ",_tag_dyneither(_tmp943,sizeof(char),3)))),((_tmp942[0]=
# 1488
Cyc_PP_textptr(_tmp3EF),Cyc_PP_cat(_tag_dyneither(_tmp942,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1496
const char*_tmp946;struct Cyc_PP_Doc*_tmp945[3];s=((_tmp945[2]=Cyc_Absynpp_stmt2doc(_tmp3F0,stmtexp),((_tmp945[1]=Cyc_PP_text(((_tmp946=": ",_tag_dyneither(_tmp946,sizeof(char),3)))),((_tmp945[0]=Cyc_PP_textptr(_tmp3EF),Cyc_PP_cat(_tag_dyneither(_tmp945,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1497
goto _LL22B;_LL24A: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp3F1=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3C8;if(_tmp3F1->tag != 14)goto _LL24C;else{_tmp3F2=_tmp3F1->f1;_tmp3F3=_tmp3F1->f2;_tmp3F4=_tmp3F3.f1;}}_LL24B:
# 1499
{const char*_tmp94D;const char*_tmp94C;const char*_tmp94B;struct Cyc_PP_Doc*_tmp94A[9];s=((_tmp94A[8]=
# 1507
Cyc_PP_text(((_tmp94B=");",_tag_dyneither(_tmp94B,sizeof(char),3)))),((_tmp94A[7]=
# 1506
Cyc_Absynpp_exp2doc(_tmp3F4),((_tmp94A[6]=
# 1505
Cyc_PP_text(((_tmp94C=" while (",_tag_dyneither(_tmp94C,sizeof(char),9)))),((_tmp94A[5]=
# 1504
Cyc_Absynpp_rb(),((_tmp94A[4]=
# 1503
Cyc_PP_line_doc(),((_tmp94A[3]=
# 1502
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3F2,0)),((_tmp94A[2]=
# 1501
Cyc_PP_line_doc(),((_tmp94A[1]=
# 1500
Cyc_Absynpp_lb(),((_tmp94A[0]=
# 1499
Cyc_PP_text(((_tmp94D="do ",_tag_dyneither(_tmp94D,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp94A,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1508
goto _LL22B;_LL24C: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp3F5=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp3C8;if(_tmp3F5->tag != 15)goto _LL24E;else{_tmp3F6=_tmp3F5->f1;_tmp3F7=_tmp3F5->f2;}}_LL24D:
# 1510
{const char*_tmp952;const char*_tmp951;struct Cyc_PP_Doc*_tmp950[12];s=((_tmp950[11]=
# 1521
Cyc_Absynpp_rb(),((_tmp950[10]=
# 1520
Cyc_PP_line_doc(),((_tmp950[9]=
# 1519
Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp3F7)),((_tmp950[8]=
# 1518
Cyc_PP_line_doc(),((_tmp950[7]=
# 1517
Cyc_Absynpp_lb(),((_tmp950[6]=
# 1516
Cyc_PP_text(((_tmp951=" catch ",_tag_dyneither(_tmp951,sizeof(char),8)))),((_tmp950[5]=
# 1515
Cyc_Absynpp_rb(),((_tmp950[4]=
# 1514
Cyc_PP_line_doc(),((_tmp950[3]=
# 1513
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3F6,0)),((_tmp950[2]=
# 1512
Cyc_PP_line_doc(),((_tmp950[1]=
# 1511
Cyc_Absynpp_lb(),((_tmp950[0]=
# 1510
Cyc_PP_text(((_tmp952="try ",_tag_dyneither(_tmp952,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp950,sizeof(struct Cyc_PP_Doc*),12))))))))))))))))))))))))));}
# 1522
goto _LL22B;_LL24E: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp3F8=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp3C8;if(_tmp3F8->tag != 16)goto _LL22B;else{_tmp3F9=_tmp3F8->f1;}}_LL24F:
# 1524
{const char*_tmp957;const char*_tmp956;struct Cyc_PP_Doc*_tmp955[3];s=((_tmp955[2]=Cyc_PP_text(((_tmp956=");",_tag_dyneither(_tmp956,sizeof(char),3)))),((_tmp955[1]=Cyc_Absynpp_exp2doc(_tmp3F9),((_tmp955[0]=Cyc_PP_text(((_tmp957="reset_region(",_tag_dyneither(_tmp957,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp955,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1525
goto _LL22B;_LL22B:;}
# 1527
return s;}
# 1530
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
# 1531
struct Cyc_PP_Doc*s;
# 1532
{void*_tmp43E=p->r;enum Cyc_Absyn_Sign _tmp442;int _tmp443;char _tmp445;struct _dyneither_ptr _tmp447;struct Cyc_Absyn_Vardecl*_tmp449;struct Cyc_Absyn_Pat*_tmp44A;struct Cyc_Absyn_Pat _tmp44B;void*_tmp44C;struct Cyc_Absyn_Vardecl*_tmp44F;struct Cyc_Absyn_Pat*_tmp450;struct Cyc_Absyn_Tvar*_tmp452;struct Cyc_Absyn_Vardecl*_tmp453;struct Cyc_List_List*_tmp455;int _tmp456;struct Cyc_Absyn_Pat*_tmp458;struct Cyc_Absyn_Vardecl*_tmp45A;struct Cyc_Absyn_Pat*_tmp45B;struct Cyc_Absyn_Pat _tmp45C;void*_tmp45D;struct Cyc_Absyn_Vardecl*_tmp460;struct Cyc_Absyn_Pat*_tmp461;struct _tuple0*_tmp463;struct _tuple0*_tmp465;struct Cyc_List_List*_tmp466;int _tmp467;struct Cyc_Absyn_AggrInfo*_tmp469;struct Cyc_Absyn_AggrInfo _tmp46A;union Cyc_Absyn_AggrInfoU _tmp46B;struct Cyc_List_List*_tmp46C;struct Cyc_List_List*_tmp46D;int _tmp46E;struct Cyc_Absyn_AggrInfo*_tmp470;struct Cyc_List_List*_tmp471;struct Cyc_List_List*_tmp472;int _tmp473;struct Cyc_Absyn_Enumfield*_tmp475;struct Cyc_Absyn_Enumfield*_tmp477;struct Cyc_Absyn_Datatypefield*_tmp479;struct Cyc_List_List*_tmp47A;struct Cyc_Absyn_Datatypefield*_tmp47C;struct Cyc_List_List*_tmp47D;int _tmp47E;struct Cyc_Absyn_Exp*_tmp480;_LL256: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp43F=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp43E;if(_tmp43F->tag != 0)goto _LL258;}_LL257:
# 1533
{const char*_tmp958;s=Cyc_PP_text(((_tmp958="_",_tag_dyneither(_tmp958,sizeof(char),2))));}goto _LL255;_LL258: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp440=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp43E;if(_tmp440->tag != 8)goto _LL25A;}_LL259:
# 1534
{const char*_tmp959;s=Cyc_PP_text(((_tmp959="NULL",_tag_dyneither(_tmp959,sizeof(char),5))));}goto _LL255;_LL25A: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp441=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp43E;if(_tmp441->tag != 9)goto _LL25C;else{_tmp442=_tmp441->f1;_tmp443=_tmp441->f2;}}_LL25B:
# 1536
 if(_tmp442 != (enum Cyc_Absyn_Sign)Cyc_Absyn_Unsigned){
# 1537
const char*_tmp95D;void*_tmp95C[1];struct Cyc_Int_pa_PrintArg_struct _tmp95B;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp95B.tag=1,((_tmp95B.f1=(unsigned long)_tmp443,((_tmp95C[0]=& _tmp95B,Cyc_aprintf(((_tmp95D="%d",_tag_dyneither(_tmp95D,sizeof(char),3))),_tag_dyneither(_tmp95C,sizeof(void*),1)))))))));}else{
# 1538
const char*_tmp961;void*_tmp960[1];struct Cyc_Int_pa_PrintArg_struct _tmp95F;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp95F.tag=1,((_tmp95F.f1=(unsigned int)_tmp443,((_tmp960[0]=& _tmp95F,Cyc_aprintf(((_tmp961="%u",_tag_dyneither(_tmp961,sizeof(char),3))),_tag_dyneither(_tmp960,sizeof(void*),1)))))))));}
# 1539
goto _LL255;_LL25C: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp444=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp43E;if(_tmp444->tag != 10)goto _LL25E;else{_tmp445=_tmp444->f1;}}_LL25D:
# 1540
{const char*_tmp965;void*_tmp964[1];struct Cyc_String_pa_PrintArg_struct _tmp963;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp963.tag=0,((_tmp963.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp445)),((_tmp964[0]=& _tmp963,Cyc_aprintf(((_tmp965="'%s'",_tag_dyneither(_tmp965,sizeof(char),5))),_tag_dyneither(_tmp964,sizeof(void*),1)))))))));}goto _LL255;_LL25E: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp446=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp43E;if(_tmp446->tag != 11)goto _LL260;else{_tmp447=_tmp446->f1;}}_LL25F:
# 1541
 s=Cyc_PP_text(_tmp447);goto _LL255;_LL260: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp448=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp43E;if(_tmp448->tag != 1)goto _LL262;else{_tmp449=_tmp448->f1;_tmp44A=_tmp448->f2;_tmp44B=*_tmp44A;_tmp44C=_tmp44B.r;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp44D=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp44C;if(_tmp44D->tag != 0)goto _LL262;};}}_LL261:
# 1543
 s=Cyc_Absynpp_qvar2doc(_tmp449->name);goto _LL255;_LL262: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp44E=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp43E;if(_tmp44E->tag != 1)goto _LL264;else{_tmp44F=_tmp44E->f1;_tmp450=_tmp44E->f2;}}_LL263:
# 1546
{const char*_tmp968;struct Cyc_PP_Doc*_tmp967[3];s=((_tmp967[2]=Cyc_Absynpp_pat2doc(_tmp450),((_tmp967[1]=Cyc_PP_text(((_tmp968=" as ",_tag_dyneither(_tmp968,sizeof(char),5)))),((_tmp967[0]=Cyc_Absynpp_qvar2doc(_tmp44F->name),Cyc_PP_cat(_tag_dyneither(_tmp967,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL255;_LL264: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp451=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp43E;if(_tmp451->tag != 3)goto _LL266;else{_tmp452=_tmp451->f1;_tmp453=_tmp451->f2;}}_LL265:
# 1548
{const char*_tmp96D;const char*_tmp96C;struct Cyc_PP_Doc*_tmp96B[4];s=((_tmp96B[3]=Cyc_PP_text(((_tmp96C=">",_tag_dyneither(_tmp96C,sizeof(char),2)))),((_tmp96B[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp452)),((_tmp96B[1]=Cyc_PP_text(((_tmp96D="<",_tag_dyneither(_tmp96D,sizeof(char),2)))),((_tmp96B[0]=Cyc_Absynpp_qvar2doc(_tmp453->name),Cyc_PP_cat(_tag_dyneither(_tmp96B,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1549
goto _LL255;_LL266: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp454=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp43E;if(_tmp454->tag != 4)goto _LL268;else{_tmp455=_tmp454->f1;_tmp456=_tmp454->f2;}}_LL267:
# 1551
{const char*_tmp976;const char*_tmp975;const char*_tmp974;const char*_tmp973;struct Cyc_PP_Doc*_tmp972[4];s=((_tmp972[3]=
# 1552
_tmp456?Cyc_PP_text(((_tmp973=", ...)",_tag_dyneither(_tmp973,sizeof(char),7)))): Cyc_PP_text(((_tmp974=")",_tag_dyneither(_tmp974,sizeof(char),2)))),((_tmp972[2]=
# 1551
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp975=",",_tag_dyneither(_tmp975,sizeof(char),2))),_tmp455),((_tmp972[1]=Cyc_PP_text(((_tmp976="(",_tag_dyneither(_tmp976,sizeof(char),2)))),((_tmp972[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp972,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1553
goto _LL255;_LL268: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp457=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp43E;if(_tmp457->tag != 5)goto _LL26A;else{_tmp458=_tmp457->f1;}}_LL269:
# 1555
{const char*_tmp979;struct Cyc_PP_Doc*_tmp978[2];s=((_tmp978[1]=Cyc_Absynpp_pat2doc(_tmp458),((_tmp978[0]=Cyc_PP_text(((_tmp979="&",_tag_dyneither(_tmp979,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp978,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1556
goto _LL255;_LL26A: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp459=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp43E;if(_tmp459->tag != 2)goto _LL26C;else{_tmp45A=_tmp459->f1;_tmp45B=_tmp459->f2;_tmp45C=*_tmp45B;_tmp45D=_tmp45C.r;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp45E=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp45D;if(_tmp45E->tag != 0)goto _LL26C;};}}_LL26B:
# 1558
{const char*_tmp97C;struct Cyc_PP_Doc*_tmp97B[2];s=((_tmp97B[1]=Cyc_Absynpp_qvar2doc(_tmp45A->name),((_tmp97B[0]=Cyc_PP_text(((_tmp97C="*",_tag_dyneither(_tmp97C,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp97B,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1559
goto _LL255;_LL26C: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp45F=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp43E;if(_tmp45F->tag != 2)goto _LL26E;else{_tmp460=_tmp45F->f1;_tmp461=_tmp45F->f2;}}_LL26D:
# 1561
{const char*_tmp981;const char*_tmp980;struct Cyc_PP_Doc*_tmp97F[4];s=((_tmp97F[3]=Cyc_Absynpp_pat2doc(_tmp461),((_tmp97F[2]=Cyc_PP_text(((_tmp980=" as ",_tag_dyneither(_tmp980,sizeof(char),5)))),((_tmp97F[1]=Cyc_Absynpp_qvar2doc(_tmp460->name),((_tmp97F[0]=Cyc_PP_text(((_tmp981="*",_tag_dyneither(_tmp981,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp97F,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1562
goto _LL255;_LL26E: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp462=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp43E;if(_tmp462->tag != 14)goto _LL270;else{_tmp463=_tmp462->f1;}}_LL26F:
# 1564
 s=Cyc_Absynpp_qvar2doc(_tmp463);
# 1565
goto _LL255;_LL270: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp464=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp43E;if(_tmp464->tag != 15)goto _LL272;else{_tmp465=_tmp464->f1;_tmp466=_tmp464->f2;_tmp467=_tmp464->f3;}}_LL271: {
# 1567
const char*_tmp983;const char*_tmp982;struct _dyneither_ptr term=_tmp467?(_tmp983=", ...)",_tag_dyneither(_tmp983,sizeof(char),7)):((_tmp982=")",_tag_dyneither(_tmp982,sizeof(char),2)));
# 1568
{const char*_tmp988;const char*_tmp987;struct Cyc_PP_Doc*_tmp986[2];s=((_tmp986[1]=Cyc_PP_group(((_tmp988="(",_tag_dyneither(_tmp988,sizeof(char),2))),term,((_tmp987=",",_tag_dyneither(_tmp987,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp466)),((_tmp986[0]=Cyc_Absynpp_qvar2doc(_tmp465),Cyc_PP_cat(_tag_dyneither(_tmp986,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1569
goto _LL255;}_LL272: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp468=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp43E;if(_tmp468->tag != 6)goto _LL274;else{_tmp469=_tmp468->f1;if(_tmp469 == 0)goto _LL274;_tmp46A=*_tmp469;_tmp46B=_tmp46A.aggr_info;_tmp46C=_tmp468->f2;_tmp46D=_tmp468->f3;_tmp46E=_tmp468->f4;}}_LL273: {
# 1571
const char*_tmp98A;const char*_tmp989;struct _dyneither_ptr term=_tmp46E?(_tmp98A=", ...}",_tag_dyneither(_tmp98A,sizeof(char),7)):((_tmp989="}",_tag_dyneither(_tmp989,sizeof(char),2)));
# 1572
struct _tuple0*_tmp4A3;struct _tuple10 _tmp4A2=Cyc_Absyn_aggr_kinded_name(_tmp46B);_tmp4A3=_tmp4A2.f2;
# 1573
{const char*_tmp995;const char*_tmp994;const char*_tmp993;const char*_tmp992;const char*_tmp991;struct Cyc_PP_Doc*_tmp990[4];s=((_tmp990[3]=
# 1575
Cyc_PP_group(((_tmp992="",_tag_dyneither(_tmp992,sizeof(char),1))),term,((_tmp991=",",_tag_dyneither(_tmp991,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp46D)),((_tmp990[2]=
# 1574
Cyc_PP_egroup(((_tmp995="[",_tag_dyneither(_tmp995,sizeof(char),2))),((_tmp994="]",_tag_dyneither(_tmp994,sizeof(char),2))),((_tmp993=",",_tag_dyneither(_tmp993,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp46C))),((_tmp990[1]=
# 1573
Cyc_Absynpp_lb(),((_tmp990[0]=Cyc_Absynpp_qvar2doc(_tmp4A3),Cyc_PP_cat(_tag_dyneither(_tmp990,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1576
goto _LL255;}_LL274: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp46F=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp43E;if(_tmp46F->tag != 6)goto _LL276;else{_tmp470=_tmp46F->f1;if(_tmp470 != 0)goto _LL276;_tmp471=_tmp46F->f2;_tmp472=_tmp46F->f3;_tmp473=_tmp46F->f4;}}_LL275: {
# 1578
const char*_tmp997;const char*_tmp996;struct _dyneither_ptr term=_tmp473?(_tmp997=", ...}",_tag_dyneither(_tmp997,sizeof(char),7)):((_tmp996="}",_tag_dyneither(_tmp996,sizeof(char),2)));
# 1579
{const char*_tmp9A2;const char*_tmp9A1;const char*_tmp9A0;const char*_tmp99F;const char*_tmp99E;struct Cyc_PP_Doc*_tmp99D[3];s=((_tmp99D[2]=
# 1581
Cyc_PP_group(((_tmp99F="",_tag_dyneither(_tmp99F,sizeof(char),1))),term,((_tmp99E=",",_tag_dyneither(_tmp99E,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp472)),((_tmp99D[1]=
# 1580
Cyc_PP_egroup(((_tmp9A2="[",_tag_dyneither(_tmp9A2,sizeof(char),2))),((_tmp9A1="]",_tag_dyneither(_tmp9A1,sizeof(char),2))),((_tmp9A0=",",_tag_dyneither(_tmp9A0,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp471))),((_tmp99D[0]=
# 1579
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp99D,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1582
goto _LL255;}_LL276: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp474=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp43E;if(_tmp474->tag != 12)goto _LL278;else{_tmp475=_tmp474->f2;}}_LL277:
# 1583
 _tmp477=_tmp475;goto _LL279;_LL278: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp476=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp43E;if(_tmp476->tag != 13)goto _LL27A;else{_tmp477=_tmp476->f2;}}_LL279:
# 1584
 s=Cyc_Absynpp_qvar2doc(_tmp477->name);goto _LL255;_LL27A: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp478=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp43E;if(_tmp478->tag != 7)goto _LL27C;else{_tmp479=_tmp478->f2;_tmp47A=_tmp478->f3;if(_tmp47A != 0)goto _LL27C;}}_LL27B:
# 1585
 s=Cyc_Absynpp_qvar2doc(_tmp479->name);goto _LL255;_LL27C: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp47B=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp43E;if(_tmp47B->tag != 7)goto _LL27E;else{_tmp47C=_tmp47B->f2;_tmp47D=_tmp47B->f3;_tmp47E=_tmp47B->f4;}}_LL27D: {
# 1587
const char*_tmp9A4;const char*_tmp9A3;struct _dyneither_ptr term=_tmp47E?(_tmp9A4=", ...)",_tag_dyneither(_tmp9A4,sizeof(char),7)):((_tmp9A3=")",_tag_dyneither(_tmp9A3,sizeof(char),2)));
# 1588
{const char*_tmp9A9;const char*_tmp9A8;struct Cyc_PP_Doc*_tmp9A7[2];s=((_tmp9A7[1]=Cyc_PP_egroup(((_tmp9A9="(",_tag_dyneither(_tmp9A9,sizeof(char),2))),term,((_tmp9A8=",",_tag_dyneither(_tmp9A8,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp47D)),((_tmp9A7[0]=Cyc_Absynpp_qvar2doc(_tmp47C->name),Cyc_PP_cat(_tag_dyneither(_tmp9A7,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1589
goto _LL255;}_LL27E: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp47F=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp43E;if(_tmp47F->tag != 16)goto _LL255;else{_tmp480=_tmp47F->f1;}}_LL27F:
# 1591
 s=Cyc_Absynpp_exp2doc(_tmp480);goto _LL255;_LL255:;}
# 1593
return s;}
# 1596
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){
# 1597
const char*_tmp9B0;const char*_tmp9AF;const char*_tmp9AE;struct Cyc_PP_Doc*_tmp9AD[2];return(_tmp9AD[1]=
# 1598
Cyc_Absynpp_pat2doc((*dp).f2),((_tmp9AD[0]=
# 1597
Cyc_PP_egroup(((_tmp9B0="",_tag_dyneither(_tmp9B0,sizeof(char),1))),((_tmp9AF="=",_tag_dyneither(_tmp9AF,sizeof(char),2))),((_tmp9AE="=",_tag_dyneither(_tmp9AE,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp9AD,sizeof(struct Cyc_PP_Doc*),2)))));}
# 1601
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
# 1602
if(c->where_clause == 0  && (c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val){
# 1603
const char*_tmp9B5;struct Cyc_PP_Doc*_tmp9B4[2];struct Cyc_PP_Doc*_tmp9B3[2];return(_tmp9B3[1]=
# 1604
Cyc_PP_nest(2,((_tmp9B4[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp9B4[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9B4,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9B3[0]=
# 1603
Cyc_PP_text(((_tmp9B5="default: ",_tag_dyneither(_tmp9B5,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp9B3,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 1605
if(c->where_clause == 0){
# 1606
const char*_tmp9BC;const char*_tmp9BB;struct Cyc_PP_Doc*_tmp9BA[2];struct Cyc_PP_Doc*_tmp9B9[4];return(_tmp9B9[3]=
# 1609
Cyc_PP_nest(2,((_tmp9BA[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp9BA[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9BA,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9B9[2]=
# 1608
Cyc_PP_text(((_tmp9BB=": ",_tag_dyneither(_tmp9BB,sizeof(char),3)))),((_tmp9B9[1]=
# 1607
Cyc_Absynpp_pat2doc(c->pattern),((_tmp9B9[0]=
# 1606
Cyc_PP_text(((_tmp9BC="case ",_tag_dyneither(_tmp9BC,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp9B9,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1611
const char*_tmp9C5;const char*_tmp9C4;const char*_tmp9C3;struct Cyc_PP_Doc*_tmp9C2[2];struct Cyc_PP_Doc*_tmp9C1[6];return(_tmp9C1[5]=
# 1616
Cyc_PP_nest(2,((_tmp9C2[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp9C2[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9C2,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9C1[4]=
# 1615
Cyc_PP_text(((_tmp9C3=": ",_tag_dyneither(_tmp9C3,sizeof(char),3)))),((_tmp9C1[3]=
# 1614
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp9C1[2]=
# 1613
Cyc_PP_text(((_tmp9C4=" && ",_tag_dyneither(_tmp9C4,sizeof(char),5)))),((_tmp9C1[1]=
# 1612
Cyc_Absynpp_pat2doc(c->pattern),((_tmp9C1[0]=
# 1611
Cyc_PP_text(((_tmp9C5="case ",_tag_dyneither(_tmp9C5,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp9C1,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
# 1619
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
# 1620
const char*_tmp9C6;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((_tmp9C6="",_tag_dyneither(_tmp9C6,sizeof(char),1))),cs);}
# 1623
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
# 1624
if(f->tag == 0)
# 1625
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1627
const char*_tmp9C9;struct Cyc_PP_Doc*_tmp9C8[3];return(_tmp9C8[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp9C8[1]=Cyc_PP_text(((_tmp9C9=" = ",_tag_dyneither(_tmp9C9,sizeof(char),4)))),((_tmp9C8[0]=Cyc_Absynpp_qvar2doc(f->name),Cyc_PP_cat(_tag_dyneither(_tmp9C8,sizeof(struct Cyc_PP_Doc*),3)))))));}}
# 1630
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
# 1631
const char*_tmp9CA;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((_tmp9CA=",",_tag_dyneither(_tmp9CA,sizeof(char),2))),fs);}
# 1634
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
# 1635
return Cyc_Absynpp_qvar2doc(v->name);}
# 1638
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
# 1639
const char*_tmp9CB;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,((_tmp9CB=",",_tag_dyneither(_tmp9CB,sizeof(char),2))),vds);}
# 1642
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
# 1643
struct Cyc_Absyn_Vardecl _tmp4CF;enum Cyc_Absyn_Scope _tmp4D0;struct _tuple0*_tmp4D1;struct Cyc_Absyn_Tqual _tmp4D2;void*_tmp4D3;struct Cyc_Absyn_Exp*_tmp4D4;struct Cyc_List_List*_tmp4D5;struct Cyc_Absyn_Vardecl*_tmp4CE=vd;_tmp4CF=*_tmp4CE;_tmp4D0=_tmp4CF.sc;_tmp4D1=_tmp4CF.name;_tmp4D2=_tmp4CF.tq;_tmp4D3=_tmp4CF.type;_tmp4D4=_tmp4CF.initializer;_tmp4D5=_tmp4CF.attributes;{
# 1644
struct Cyc_PP_Doc*s;
# 1645
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp4D1);
# 1646
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4D5);
# 1647
struct Cyc_PP_Doc*beforenamedoc;
# 1648
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL280:
# 1649
 beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL281:
# 1651
{void*_tmp4D6=Cyc_Tcutil_compress(_tmp4D3);struct Cyc_Absyn_FnInfo _tmp4D8;struct Cyc_List_List*_tmp4D9;_LL284: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4D7=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D6;if(_tmp4D7->tag != 9)goto _LL286;else{_tmp4D8=_tmp4D7->f1;_tmp4D9=_tmp4D8.attributes;}}_LL285:
# 1653
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4D9);
# 1654
goto _LL283;_LL286:;_LL287:
# 1655
 beforenamedoc=Cyc_PP_nil_doc();goto _LL283;_LL283:;}
# 1657
break;}{
# 1660
struct Cyc_PP_Doc*tmp_doc;
# 1661
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL288:
# 1662
 tmp_doc=Cyc_PP_nil_doc();break;case Cyc_Cyclone_Vc_c: _LL289:
# 1663
 tmp_doc=attsdoc;break;}
# 1665
{struct Cyc_Core_Opt*_tmp9D8;struct Cyc_PP_Doc*_tmp9D7[2];struct Cyc_PP_Doc*_tmp9D6[2];const char*_tmp9D5;const char*_tmp9D4;struct Cyc_PP_Doc*_tmp9D3[5];s=((_tmp9D3[4]=
# 1672
Cyc_PP_text(((_tmp9D4=";",_tag_dyneither(_tmp9D4,sizeof(char),2)))),((_tmp9D3[3]=
# 1669
_tmp4D4 == 0?
# 1670
Cyc_PP_nil_doc():(
# 1671
(_tmp9D6[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp4D4),((_tmp9D6[0]=Cyc_PP_text(((_tmp9D5=" = ",_tag_dyneither(_tmp9D5,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp9D6,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9D3[2]=
# 1668
Cyc_Absynpp_tqtd2doc(_tmp4D2,_tmp4D3,((_tmp9D8=_cycalloc(sizeof(*_tmp9D8)),((_tmp9D8->v=((_tmp9D7[1]=sn,((_tmp9D7[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmp9D7,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9D8))))),((_tmp9D3[1]=
# 1667
Cyc_Absynpp_scope2doc(_tmp4D0),((_tmp9D3[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(_tmp9D3,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1673
return s;};};}struct _tuple17{unsigned int f1;struct _tuple0*f2;int f3;};
# 1676
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){
# 1677
struct _tuple0*_tmp4E1;struct _tuple17 _tmp4E0=*x;_tmp4E1=_tmp4E0.f2;
# 1678
return Cyc_Absynpp_qvar2doc(_tmp4E1);}
# 1681
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
# 1682
if(ad->impl == 0){
# 1683
struct Cyc_PP_Doc*_tmp9D9[4];return(_tmp9D9[3]=
# 1686
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmp9D9[2]=
# 1685
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp9D9[1]=
# 1684
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmp9D9[0]=
# 1683
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmp9D9,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1688
struct Cyc_PP_Doc*_tmp9E1[2];const char*_tmp9E0;struct Cyc_PP_Doc*_tmp9DF[2];struct Cyc_PP_Doc*_tmp9DE[12];return(_tmp9DE[11]=
# 1699
Cyc_Absynpp_atts2doc(ad->attributes),((_tmp9DE[10]=
# 1698
Cyc_Absynpp_rb(),((_tmp9DE[9]=
# 1697
Cyc_PP_line_doc(),((_tmp9DE[8]=
# 1696
Cyc_PP_nest(2,((_tmp9DF[1]=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields),((_tmp9DF[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9DF,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9DE[7]=
# 1694
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?Cyc_PP_nil_doc():(
# 1695
(_tmp9E1[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po),((_tmp9E1[0]=Cyc_PP_text(((_tmp9E0=":",_tag_dyneither(_tmp9E0,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp9E1,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9DE[6]=
# 1693
Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars),((_tmp9DE[5]=
# 1692
Cyc_Absynpp_lb(),((_tmp9DE[4]=Cyc_PP_blank_doc(),((_tmp9DE[3]=
# 1691
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmp9DE[2]=
# 1690
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp9DE[1]=
# 1689
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmp9DE[0]=
# 1688
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmp9DE,sizeof(struct Cyc_PP_Doc*),12)))))))))))))))))))))))));}}
# 1702
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
# 1703
struct Cyc_Absyn_Datatypedecl _tmp4E8;enum Cyc_Absyn_Scope _tmp4E9;struct _tuple0*_tmp4EA;struct Cyc_List_List*_tmp4EB;struct Cyc_Core_Opt*_tmp4EC;int _tmp4ED;struct Cyc_Absyn_Datatypedecl*_tmp4E7=dd;_tmp4E8=*_tmp4E7;_tmp4E9=_tmp4E8.sc;_tmp4EA=_tmp4E8.name;_tmp4EB=_tmp4E8.tvs;_tmp4EC=_tmp4E8.fields;_tmp4ED=_tmp4E8.is_extensible;
# 1704
if(_tmp4EC == 0){
# 1705
const char*_tmp9E6;const char*_tmp9E5;struct Cyc_PP_Doc*_tmp9E4[5];return(_tmp9E4[4]=
# 1709
Cyc_Absynpp_ktvars2doc(_tmp4EB),((_tmp9E4[3]=
# 1708
_tmp4ED?Cyc_Absynpp_qvar2bolddoc(_tmp4EA): Cyc_Absynpp_typedef_name2bolddoc(_tmp4EA),((_tmp9E4[2]=
# 1707
Cyc_PP_text(((_tmp9E5="datatype ",_tag_dyneither(_tmp9E5,sizeof(char),10)))),((_tmp9E4[1]=
# 1706
_tmp4ED?Cyc_PP_text(((_tmp9E6="@extensible ",_tag_dyneither(_tmp9E6,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp9E4[0]=
# 1705
Cyc_Absynpp_scope2doc(_tmp4E9),Cyc_PP_cat(_tag_dyneither(_tmp9E4,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 1711
const char*_tmp9ED;const char*_tmp9EC;struct Cyc_PP_Doc*_tmp9EB[2];struct Cyc_PP_Doc*_tmp9EA[10];return(_tmp9EA[9]=
# 1719
Cyc_Absynpp_rb(),((_tmp9EA[8]=
# 1718
Cyc_PP_line_doc(),((_tmp9EA[7]=
# 1717
Cyc_PP_nest(2,((_tmp9EB[1]=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)_tmp4EC->v),((_tmp9EB[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9EB,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9EA[6]=
# 1716
Cyc_Absynpp_lb(),((_tmp9EA[5]=Cyc_PP_blank_doc(),((_tmp9EA[4]=
# 1715
Cyc_Absynpp_ktvars2doc(_tmp4EB),((_tmp9EA[3]=
# 1714
_tmp4ED?Cyc_Absynpp_qvar2bolddoc(_tmp4EA): Cyc_Absynpp_typedef_name2bolddoc(_tmp4EA),((_tmp9EA[2]=
# 1713
Cyc_PP_text(((_tmp9EC="datatype ",_tag_dyneither(_tmp9EC,sizeof(char),10)))),((_tmp9EA[1]=
# 1712
_tmp4ED?Cyc_PP_text(((_tmp9ED="@extensible ",_tag_dyneither(_tmp9ED,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp9EA[0]=
# 1711
Cyc_Absynpp_scope2doc(_tmp4E9),Cyc_PP_cat(_tag_dyneither(_tmp9EA,sizeof(struct Cyc_PP_Doc*),10)))))))))))))))))))));}}
# 1722
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
# 1723
struct Cyc_Absyn_Enumdecl _tmp4F6;enum Cyc_Absyn_Scope _tmp4F7;struct _tuple0*_tmp4F8;struct Cyc_Core_Opt*_tmp4F9;struct Cyc_Absyn_Enumdecl*_tmp4F5=ed;_tmp4F6=*_tmp4F5;_tmp4F7=_tmp4F6.sc;_tmp4F8=_tmp4F6.name;_tmp4F9=_tmp4F6.fields;
# 1724
if(_tmp4F9 == 0){
# 1725
const char*_tmp9F0;struct Cyc_PP_Doc*_tmp9EF[3];return(_tmp9EF[2]=
# 1727
Cyc_Absynpp_typedef_name2bolddoc(_tmp4F8),((_tmp9EF[1]=
# 1726
Cyc_PP_text(((_tmp9F0="enum ",_tag_dyneither(_tmp9F0,sizeof(char),6)))),((_tmp9EF[0]=
# 1725
Cyc_Absynpp_scope2doc(_tmp4F7),Cyc_PP_cat(_tag_dyneither(_tmp9EF,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
# 1730
const char*_tmp9F5;struct Cyc_PP_Doc*_tmp9F4[2];struct Cyc_PP_Doc*_tmp9F3[8];return(_tmp9F3[7]=
# 1736
Cyc_Absynpp_rb(),((_tmp9F3[6]=
# 1735
Cyc_PP_line_doc(),((_tmp9F3[5]=
# 1734
Cyc_PP_nest(2,((_tmp9F4[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp4F9->v),((_tmp9F4[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9F4,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9F3[4]=
# 1733
Cyc_Absynpp_lb(),((_tmp9F3[3]=Cyc_PP_blank_doc(),((_tmp9F3[2]=
# 1732
Cyc_Absynpp_qvar2bolddoc(_tmp4F8),((_tmp9F3[1]=
# 1731
Cyc_PP_text(((_tmp9F5="enum ",_tag_dyneither(_tmp9F5,sizeof(char),6)))),((_tmp9F3[0]=
# 1730
Cyc_Absynpp_scope2doc(_tmp4F7),Cyc_PP_cat(_tag_dyneither(_tmp9F3,sizeof(struct Cyc_PP_Doc*),8)))))))))))))))));}}
# 1739
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
# 1740
struct Cyc_PP_Doc*s;
# 1741
{void*_tmp4FF=d->r;struct Cyc_Absyn_Fndecl*_tmp501;struct Cyc_Absyn_Aggrdecl*_tmp503;struct Cyc_Absyn_Vardecl*_tmp505;struct Cyc_Absyn_Tvar*_tmp507;struct Cyc_Absyn_Vardecl*_tmp508;int _tmp509;struct Cyc_Absyn_Exp*_tmp50A;struct Cyc_Absyn_Exp*_tmp50C;struct Cyc_Absyn_Tvar*_tmp50D;struct Cyc_Absyn_Vardecl*_tmp50E;struct Cyc_Absyn_Datatypedecl*_tmp510;struct Cyc_Absyn_Pat*_tmp512;struct Cyc_Absyn_Exp*_tmp513;struct Cyc_List_List*_tmp515;struct Cyc_Absyn_Enumdecl*_tmp517;struct Cyc_Absyn_Typedefdecl*_tmp519;struct _dyneither_ptr*_tmp51B;struct Cyc_List_List*_tmp51C;struct _tuple0*_tmp51E;struct Cyc_List_List*_tmp51F;struct Cyc_List_List*_tmp521;struct Cyc_List_List*_tmp523;struct Cyc_List_List*_tmp524;_LL28C: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp500=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp4FF;if(_tmp500->tag != 1)goto _LL28E;else{_tmp501=_tmp500->f1;}}_LL28D: {
# 1743
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp9FB;struct Cyc_Absyn_FnInfo _tmp9FA;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9F9;void*t=(void*)((_tmp9F9=_cycalloc(sizeof(*_tmp9F9)),((_tmp9F9[0]=((_tmp9FB.tag=9,((_tmp9FB.f1=((_tmp9FA.tvars=_tmp501->tvs,((_tmp9FA.effect=_tmp501->effect,((_tmp9FA.ret_tqual=_tmp501->ret_tqual,((_tmp9FA.ret_typ=_tmp501->ret_type,((_tmp9FA.args=
# 1747
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp501->args),((_tmp9FA.c_varargs=_tmp501->c_varargs,((_tmp9FA.cyc_varargs=_tmp501->cyc_varargs,((_tmp9FA.rgn_po=_tmp501->rgn_po,((_tmp9FA.attributes=0,_tmp9FA)))))))))))))))))),_tmp9FB)))),_tmp9F9))));
# 1751
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp501->attributes);
# 1752
struct Cyc_PP_Doc*inlinedoc;
# 1753
if(_tmp501->is_inline)
# 1754
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2AC:
# 1755
{const char*_tmp9FC;inlinedoc=Cyc_PP_text(((_tmp9FC="inline ",_tag_dyneither(_tmp9FC,sizeof(char),8))));}break;case Cyc_Cyclone_Vc_c: _LL2AD:
# 1756
{const char*_tmp9FD;inlinedoc=Cyc_PP_text(((_tmp9FD="__inline ",_tag_dyneither(_tmp9FD,sizeof(char),10))));}break;}else{
# 1759
inlinedoc=Cyc_PP_nil_doc();}{
# 1760
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(_tmp501->sc);
# 1761
struct Cyc_PP_Doc*beforenamedoc;
# 1762
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2AF:
# 1763
 beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL2B0:
# 1764
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp501->attributes);break;}{
# 1766
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp501->name);
# 1767
struct Cyc_PP_Doc*_tmpA00[2];struct Cyc_Core_Opt*_tmp9FF;struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,(
# 1768
(_tmp9FF=_cycalloc(sizeof(*_tmp9FF)),((_tmp9FF->v=((_tmpA00[1]=namedoc,((_tmpA00[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmpA00,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9FF)))));
# 1769
struct Cyc_PP_Doc*_tmpA03[2];struct Cyc_PP_Doc*_tmpA02[5];struct Cyc_PP_Doc*bodydoc=(_tmpA02[4]=
# 1772
Cyc_Absynpp_rb(),((_tmpA02[3]=
# 1771
Cyc_PP_line_doc(),((_tmpA02[2]=
# 1770
Cyc_PP_nest(2,((_tmpA03[1]=Cyc_Absynpp_stmt2doc(_tmp501->body,0),((_tmpA03[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmpA03,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmpA02[1]=
# 1769
Cyc_Absynpp_lb(),((_tmpA02[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(_tmpA02,sizeof(struct Cyc_PP_Doc*),5)))))))))));
# 1773
{struct Cyc_PP_Doc*_tmpA04[4];s=((_tmpA04[3]=bodydoc,((_tmpA04[2]=tqtddoc,((_tmpA04[1]=scopedoc,((_tmpA04[0]=inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmpA04,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1775
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL2B2:
# 1776
{struct Cyc_PP_Doc*_tmpA05[2];s=((_tmpA05[1]=s,((_tmpA05[0]=attsdoc,Cyc_PP_cat(_tag_dyneither(_tmpA05,sizeof(struct Cyc_PP_Doc*),2))))));}break;default: _LL2B3:
# 1777
 break;}
# 1780
goto _LL28B;};};}_LL28E: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp502=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp4FF;if(_tmp502->tag != 6)goto _LL290;else{_tmp503=_tmp502->f1;}}_LL28F:
# 1783
{const char*_tmpA08;struct Cyc_PP_Doc*_tmpA07[2];s=((_tmpA07[1]=Cyc_PP_text(((_tmpA08=";",_tag_dyneither(_tmpA08,sizeof(char),2)))),((_tmpA07[0]=Cyc_Absynpp_aggrdecl2doc(_tmp503),Cyc_PP_cat(_tag_dyneither(_tmpA07,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1784
goto _LL28B;_LL290: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp504=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp4FF;if(_tmp504->tag != 0)goto _LL292;else{_tmp505=_tmp504->f1;}}_LL291:
# 1786
 s=Cyc_Absynpp_vardecl2doc(_tmp505);
# 1787
goto _LL28B;_LL292: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp506=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp4FF;if(_tmp506->tag != 4)goto _LL294;else{_tmp507=_tmp506->f1;_tmp508=_tmp506->f2;_tmp509=_tmp506->f3;_tmp50A=_tmp506->f4;}}_LL293:
# 1789
{const char*_tmpA1B;const char*_tmpA1A;const char*_tmpA19;const char*_tmpA18;struct Cyc_PP_Doc*_tmpA17[3];const char*_tmpA16;const char*_tmpA15;const char*_tmpA14;struct Cyc_PP_Doc*_tmpA13[8];s=((_tmpA13[7]=
# 1797
Cyc_PP_text(((_tmpA14=";",_tag_dyneither(_tmpA14,sizeof(char),2)))),((_tmpA13[6]=
# 1795
_tmp50A != 0?
# 1796
(_tmpA17[2]=Cyc_PP_text(((_tmpA16=")",_tag_dyneither(_tmpA16,sizeof(char),2)))),((_tmpA17[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp50A),((_tmpA17[0]=Cyc_PP_text(((_tmpA15=" = open(",_tag_dyneither(_tmpA15,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmpA17,sizeof(struct Cyc_PP_Doc*),3))))))):
# 1797
 Cyc_PP_nil_doc(),((_tmpA13[5]=
# 1794
_tmp509?Cyc_PP_text(((_tmpA18=" @resetable",_tag_dyneither(_tmpA18,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmpA13[4]=
# 1793
Cyc_Absynpp_qvar2doc(_tmp508->name),((_tmpA13[3]=
# 1792
Cyc_PP_text(((_tmpA19=">",_tag_dyneither(_tmpA19,sizeof(char),2)))),((_tmpA13[2]=
# 1791
Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp507)),((_tmpA13[1]=
# 1790
Cyc_PP_text(((_tmpA1A="<",_tag_dyneither(_tmpA1A,sizeof(char),2)))),((_tmpA13[0]=
# 1789
Cyc_PP_text(((_tmpA1B="region",_tag_dyneither(_tmpA1B,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA13,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1798
goto _LL28B;_LL294: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp50B=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp4FF;if(_tmp50B->tag != 5)goto _LL296;else{_tmp50C=_tmp50B->f1;_tmp50D=_tmp50B->f2;_tmp50E=_tmp50B->f3;}}_LL295:
# 1800
{const char*_tmpA26;const char*_tmpA25;const char*_tmpA24;const char*_tmpA23;const char*_tmpA22;struct Cyc_PP_Doc*_tmpA21[8];s=((_tmpA21[7]=
# 1801
Cyc_PP_text(((_tmpA22=";",_tag_dyneither(_tmpA22,sizeof(char),2)))),((_tmpA21[6]=Cyc_Absynpp_exp2doc(_tmp50C),((_tmpA21[5]=Cyc_PP_text(((_tmpA23=" = ",_tag_dyneither(_tmpA23,sizeof(char),4)))),((_tmpA21[4]=Cyc_Absynpp_qvar2doc(_tmp50E->name),((_tmpA21[3]=
# 1800
Cyc_PP_text(((_tmpA24="> ",_tag_dyneither(_tmpA24,sizeof(char),3)))),((_tmpA21[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp50D)),((_tmpA21[1]=Cyc_PP_text(((_tmpA25=" <",_tag_dyneither(_tmpA25,sizeof(char),3)))),((_tmpA21[0]=Cyc_PP_text(((_tmpA26="alias",_tag_dyneither(_tmpA26,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmpA21,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1802
goto _LL28B;_LL296: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp50F=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp4FF;if(_tmp50F->tag != 7)goto _LL298;else{_tmp510=_tmp50F->f1;}}_LL297:
# 1804
{const char*_tmpA29;struct Cyc_PP_Doc*_tmpA28[2];s=((_tmpA28[1]=Cyc_PP_text(((_tmpA29=";",_tag_dyneither(_tmpA29,sizeof(char),2)))),((_tmpA28[0]=Cyc_Absynpp_datatypedecl2doc(_tmp510),Cyc_PP_cat(_tag_dyneither(_tmpA28,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1805
goto _LL28B;_LL298: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp511=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp4FF;if(_tmp511->tag != 2)goto _LL29A;else{_tmp512=_tmp511->f1;_tmp513=_tmp511->f3;}}_LL299:
# 1807
{const char*_tmpA30;const char*_tmpA2F;const char*_tmpA2E;struct Cyc_PP_Doc*_tmpA2D[5];s=((_tmpA2D[4]=
# 1811
Cyc_PP_text(((_tmpA2E=";",_tag_dyneither(_tmpA2E,sizeof(char),2)))),((_tmpA2D[3]=
# 1810
Cyc_Absynpp_exp2doc(_tmp513),((_tmpA2D[2]=
# 1809
Cyc_PP_text(((_tmpA2F=" = ",_tag_dyneither(_tmpA2F,sizeof(char),4)))),((_tmpA2D[1]=
# 1808
Cyc_Absynpp_pat2doc(_tmp512),((_tmpA2D[0]=
# 1807
Cyc_PP_text(((_tmpA30="let ",_tag_dyneither(_tmpA30,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmpA2D,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1812
goto _LL28B;_LL29A: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp514=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp4FF;if(_tmp514->tag != 3)goto _LL29C;else{_tmp515=_tmp514->f1;}}_LL29B:
# 1814
{const char*_tmpA35;const char*_tmpA34;struct Cyc_PP_Doc*_tmpA33[3];s=((_tmpA33[2]=Cyc_PP_text(((_tmpA34=";",_tag_dyneither(_tmpA34,sizeof(char),2)))),((_tmpA33[1]=Cyc_Absynpp_ids2doc(_tmp515),((_tmpA33[0]=Cyc_PP_text(((_tmpA35="let ",_tag_dyneither(_tmpA35,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmpA33,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1815
goto _LL28B;_LL29C: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp516=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp4FF;if(_tmp516->tag != 8)goto _LL29E;else{_tmp517=_tmp516->f1;}}_LL29D:
# 1817
{const char*_tmpA38;struct Cyc_PP_Doc*_tmpA37[2];s=((_tmpA37[1]=Cyc_PP_text(((_tmpA38=";",_tag_dyneither(_tmpA38,sizeof(char),2)))),((_tmpA37[0]=Cyc_Absynpp_enumdecl2doc(_tmp517),Cyc_PP_cat(_tag_dyneither(_tmpA37,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1818
goto _LL28B;_LL29E: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp518=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp4FF;if(_tmp518->tag != 9)goto _LL2A0;else{_tmp519=_tmp518->f1;}}_LL29F: {
# 1820
void*t;
# 1821
if(_tmp519->defn != 0)
# 1822
t=(void*)_check_null(_tmp519->defn);else{
# 1824
t=Cyc_Absyn_new_evar(_tmp519->kind,0);}
# 1825
{const char*_tmpA42;struct Cyc_Core_Opt*_tmpA41;struct Cyc_PP_Doc*_tmpA40[2];const char*_tmpA3F;struct Cyc_PP_Doc*_tmpA3E[4];s=((_tmpA3E[3]=
# 1832
Cyc_PP_text(((_tmpA3F=";",_tag_dyneither(_tmpA3F,sizeof(char),2)))),((_tmpA3E[2]=
# 1831
Cyc_Absynpp_atts2doc(_tmp519->atts),((_tmpA3E[1]=
# 1826
Cyc_Absynpp_tqtd2doc(_tmp519->tq,t,(
# 1828
(_tmpA41=_cycalloc(sizeof(*_tmpA41)),((_tmpA41->v=((_tmpA40[1]=
# 1829
Cyc_Absynpp_tvars2doc(_tmp519->tvs),((_tmpA40[0]=
# 1828
Cyc_Absynpp_typedef_name2bolddoc(_tmp519->name),Cyc_PP_cat(_tag_dyneither(_tmpA40,sizeof(struct Cyc_PP_Doc*),2)))))),_tmpA41))))),((_tmpA3E[0]=
# 1825
Cyc_PP_text(((_tmpA42="typedef ",_tag_dyneither(_tmpA42,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmpA3E,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1834
goto _LL28B;}_LL2A0: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp51A=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp4FF;if(_tmp51A->tag != 10)goto _LL2A2;else{_tmp51B=_tmp51A->f1;_tmp51C=_tmp51A->f2;}}_LL2A1:
# 1836
 if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp51B);
# 1837
{const char*_tmpA47;const char*_tmpA46;struct Cyc_PP_Doc*_tmpA45[8];s=((_tmpA45[7]=
# 1843
Cyc_Absynpp_rb(),((_tmpA45[6]=
# 1842
Cyc_PP_line_doc(),((_tmpA45[5]=
# 1841
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA46="",_tag_dyneither(_tmpA46,sizeof(char),1))),_tmp51C),((_tmpA45[4]=
# 1840
Cyc_PP_line_doc(),((_tmpA45[3]=
# 1839
Cyc_Absynpp_lb(),((_tmpA45[2]=Cyc_PP_blank_doc(),((_tmpA45[1]=
# 1838
Cyc_PP_textptr(_tmp51B),((_tmpA45[0]=
# 1837
Cyc_PP_text(((_tmpA47="namespace ",_tag_dyneither(_tmpA47,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmpA45,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1844
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
# 1845
goto _LL28B;_LL2A2: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp51D=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp4FF;if(_tmp51D->tag != 11)goto _LL2A4;else{_tmp51E=_tmp51D->f1;_tmp51F=_tmp51D->f2;}}_LL2A3:
# 1847
 if(Cyc_Absynpp_print_using_stmts){
# 1848
const char*_tmpA4C;const char*_tmpA4B;struct Cyc_PP_Doc*_tmpA4A[8];s=((_tmpA4A[7]=
# 1854
Cyc_Absynpp_rb(),((_tmpA4A[6]=
# 1853
Cyc_PP_line_doc(),((_tmpA4A[5]=
# 1852
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA4B="",_tag_dyneither(_tmpA4B,sizeof(char),1))),_tmp51F),((_tmpA4A[4]=
# 1851
Cyc_PP_line_doc(),((_tmpA4A[3]=
# 1850
Cyc_Absynpp_lb(),((_tmpA4A[2]=Cyc_PP_blank_doc(),((_tmpA4A[1]=
# 1849
Cyc_Absynpp_qvar2doc(_tmp51E),((_tmpA4A[0]=
# 1848
Cyc_PP_text(((_tmpA4C="using ",_tag_dyneither(_tmpA4C,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA4A,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1856
const char*_tmpA57;const char*_tmpA56;const char*_tmpA55;const char*_tmpA54;const char*_tmpA53;struct Cyc_PP_Doc*_tmpA52[11];s=((_tmpA52[10]=
# 1866
Cyc_PP_text(((_tmpA53=" */",_tag_dyneither(_tmpA53,sizeof(char),4)))),((_tmpA52[9]=
# 1865
Cyc_Absynpp_rb(),((_tmpA52[8]=
# 1864
Cyc_PP_text(((_tmpA54="/* ",_tag_dyneither(_tmpA54,sizeof(char),4)))),((_tmpA52[7]=
# 1863
Cyc_PP_line_doc(),((_tmpA52[6]=
# 1862
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA55="",_tag_dyneither(_tmpA55,sizeof(char),1))),_tmp51F),((_tmpA52[5]=
# 1861
Cyc_PP_line_doc(),((_tmpA52[4]=
# 1860
Cyc_PP_text(((_tmpA56=" */",_tag_dyneither(_tmpA56,sizeof(char),4)))),((_tmpA52[3]=
# 1859
Cyc_Absynpp_lb(),((_tmpA52[2]=
# 1858
Cyc_PP_blank_doc(),((_tmpA52[1]=
# 1857
Cyc_Absynpp_qvar2doc(_tmp51E),((_tmpA52[0]=
# 1856
Cyc_PP_text(((_tmpA57="/* using ",_tag_dyneither(_tmpA57,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmpA52,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1867
goto _LL28B;_LL2A4: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp520=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp4FF;if(_tmp520->tag != 12)goto _LL2A6;else{_tmp521=_tmp520->f1;}}_LL2A5:
# 1869
 if(Cyc_Absynpp_print_externC_stmts){
# 1870
const char*_tmpA5C;const char*_tmpA5B;struct Cyc_PP_Doc*_tmpA5A[6];s=((_tmpA5A[5]=
# 1875
Cyc_Absynpp_rb(),((_tmpA5A[4]=
# 1874
Cyc_PP_line_doc(),((_tmpA5A[3]=
# 1873
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA5B="",_tag_dyneither(_tmpA5B,sizeof(char),1))),_tmp521),((_tmpA5A[2]=
# 1872
Cyc_PP_line_doc(),((_tmpA5A[1]=
# 1871
Cyc_Absynpp_lb(),((_tmpA5A[0]=
# 1870
Cyc_PP_text(((_tmpA5C="extern \"C\" ",_tag_dyneither(_tmpA5C,sizeof(char),12)))),Cyc_PP_cat(_tag_dyneither(_tmpA5A,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}else{
# 1877
const char*_tmpA67;const char*_tmpA66;const char*_tmpA65;const char*_tmpA64;const char*_tmpA63;struct Cyc_PP_Doc*_tmpA62[9];s=((_tmpA62[8]=
# 1885
Cyc_PP_text(((_tmpA63=" */",_tag_dyneither(_tmpA63,sizeof(char),4)))),((_tmpA62[7]=
# 1884
Cyc_Absynpp_rb(),((_tmpA62[6]=
# 1883
Cyc_PP_text(((_tmpA64="/* ",_tag_dyneither(_tmpA64,sizeof(char),4)))),((_tmpA62[5]=
# 1882
Cyc_PP_line_doc(),((_tmpA62[4]=
# 1881
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA65="",_tag_dyneither(_tmpA65,sizeof(char),1))),_tmp521),((_tmpA62[3]=
# 1880
Cyc_PP_line_doc(),((_tmpA62[2]=
# 1879
Cyc_PP_text(((_tmpA66=" */",_tag_dyneither(_tmpA66,sizeof(char),4)))),((_tmpA62[1]=
# 1878
Cyc_Absynpp_lb(),((_tmpA62[0]=
# 1877
Cyc_PP_text(((_tmpA67="/* extern \"C\" ",_tag_dyneither(_tmpA67,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmpA62,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1886
goto _LL28B;_LL2A6: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp522=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp4FF;if(_tmp522->tag != 13)goto _LL2A8;else{_tmp523=_tmp522->f1;_tmp524=_tmp522->f2;}}_LL2A7:
# 1888
 if(Cyc_Absynpp_print_externC_stmts){
# 1889
struct Cyc_PP_Doc*exs_doc;
# 1890
if(_tmp524 != 0){
# 1891
const char*_tmpA6C;const char*_tmpA6B;struct Cyc_PP_Doc*_tmpA6A[7];exs_doc=((_tmpA6A[6]=
# 1893
Cyc_Absynpp_rb(),((_tmpA6A[5]=Cyc_PP_line_doc(),((_tmpA6A[4]=
# 1892
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,((_tmpA6B=",",_tag_dyneither(_tmpA6B,sizeof(char),2))),_tmp524),((_tmpA6A[3]=Cyc_PP_line_doc(),((_tmpA6A[2]=
# 1891
Cyc_Absynpp_lb(),((_tmpA6A[1]=Cyc_PP_text(((_tmpA6C=" export ",_tag_dyneither(_tmpA6C,sizeof(char),9)))),((_tmpA6A[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmpA6A,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1895
exs_doc=Cyc_Absynpp_rb();}{
# 1896
const char*_tmpA71;const char*_tmpA70;struct Cyc_PP_Doc*_tmpA6F[6];s=((_tmpA6F[5]=exs_doc,((_tmpA6F[4]=
# 1900
Cyc_PP_line_doc(),((_tmpA6F[3]=
# 1899
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA70="",_tag_dyneither(_tmpA70,sizeof(char),1))),_tmp523),((_tmpA6F[2]=
# 1898
Cyc_PP_line_doc(),((_tmpA6F[1]=
# 1897
Cyc_Absynpp_lb(),((_tmpA6F[0]=
# 1896
Cyc_PP_text(((_tmpA71="extern \"C include\" ",_tag_dyneither(_tmpA71,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmpA6F,sizeof(struct Cyc_PP_Doc*),6))))))))))))));};}else{
# 1903
const char*_tmpA7C;const char*_tmpA7B;const char*_tmpA7A;const char*_tmpA79;const char*_tmpA78;struct Cyc_PP_Doc*_tmpA77[9];s=((_tmpA77[8]=
# 1911
Cyc_PP_text(((_tmpA78=" */",_tag_dyneither(_tmpA78,sizeof(char),4)))),((_tmpA77[7]=
# 1910
Cyc_Absynpp_rb(),((_tmpA77[6]=
# 1909
Cyc_PP_text(((_tmpA79="/* ",_tag_dyneither(_tmpA79,sizeof(char),4)))),((_tmpA77[5]=
# 1908
Cyc_PP_line_doc(),((_tmpA77[4]=
# 1907
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA7A="",_tag_dyneither(_tmpA7A,sizeof(char),1))),_tmp523),((_tmpA77[3]=
# 1906
Cyc_PP_line_doc(),((_tmpA77[2]=
# 1905
Cyc_PP_text(((_tmpA7B=" */",_tag_dyneither(_tmpA7B,sizeof(char),4)))),((_tmpA77[1]=
# 1904
Cyc_Absynpp_lb(),((_tmpA77[0]=
# 1903
Cyc_PP_text(((_tmpA7C="/* extern \"C include\" ",_tag_dyneither(_tmpA7C,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmpA77,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1912
goto _LL28B;_LL2A8: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp525=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp4FF;if(_tmp525->tag != 14)goto _LL2AA;}_LL2A9:
# 1914
{const char*_tmpA7F;struct Cyc_PP_Doc*_tmpA7E[2];s=((_tmpA7E[1]=Cyc_Absynpp_lb(),((_tmpA7E[0]=Cyc_PP_text(((_tmpA7F="__cyclone_port_on__;",_tag_dyneither(_tmpA7F,sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmpA7E,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1915
goto _LL28B;_LL2AA: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp526=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp4FF;if(_tmp526->tag != 15)goto _LL28B;}_LL2AB:
# 1917
{const char*_tmpA82;struct Cyc_PP_Doc*_tmpA81[2];s=((_tmpA81[1]=Cyc_Absynpp_lb(),((_tmpA81[0]=Cyc_PP_text(((_tmpA82="__cyclone_port_off__;",_tag_dyneither(_tmpA82,sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmpA81,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1918
goto _LL28B;_LL28B:;}
# 1921
return s;}
# 1924
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
# 1925
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();
# 1926
switch(sc){case Cyc_Absyn_Static: _LL2B5: {
# 1927
const char*_tmpA83;return Cyc_PP_text(((_tmpA83="static ",_tag_dyneither(_tmpA83,sizeof(char),8))));}case Cyc_Absyn_Public: _LL2B6:
# 1928
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL2B7: {
# 1929
const char*_tmpA84;return Cyc_PP_text(((_tmpA84="extern ",_tag_dyneither(_tmpA84,sizeof(char),8))));}case Cyc_Absyn_ExternC: _LL2B8: {
# 1930
const char*_tmpA85;return Cyc_PP_text(((_tmpA85="extern \"C\" ",_tag_dyneither(_tmpA85,sizeof(char),12))));}case Cyc_Absyn_Abstract: _LL2B9: {
# 1931
const char*_tmpA86;return Cyc_PP_text(((_tmpA86="abstract ",_tag_dyneither(_tmpA86,sizeof(char),10))));}case Cyc_Absyn_Register: _LL2BA: {
# 1932
const char*_tmpA87;return Cyc_PP_text(((_tmpA87="register ",_tag_dyneither(_tmpA87,sizeof(char),10))));}}}
# 1937
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
# 1938
void*_tmp57D=t;struct Cyc_Absyn_Tvar*_tmp57F;struct Cyc_List_List*_tmp581;_LL2BD: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp57E=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp57D;if(_tmp57E->tag != 2)goto _LL2BF;else{_tmp57F=_tmp57E->f1;}}_LL2BE:
# 1939
 return Cyc_Tcutil_is_temp_tvar(_tmp57F);_LL2BF: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp580=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp57D;if(_tmp580->tag != 24)goto _LL2C1;else{_tmp581=_tmp580->f1;}}_LL2C0:
# 1940
 return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp581);_LL2C1:;_LL2C2:
# 1941
 return 0;_LL2BC:;}
# 1949
int Cyc_Absynpp_is_anon_aggrtype(void*t){
# 1950
void*_tmp582=t;struct Cyc_Absyn_Typedefdecl*_tmp586;void*_tmp587;_LL2C4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp583=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp582;if(_tmp583->tag != 12)goto _LL2C6;}_LL2C5:
# 1951
 return 1;_LL2C6: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp584=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp582;if(_tmp584->tag != 14)goto _LL2C8;}_LL2C7:
# 1952
 return 1;_LL2C8:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp585=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp582;if(_tmp585->tag != 17)goto _LL2CA;else{_tmp586=_tmp585->f3;_tmp587=(void*)_tmp585->f4;}}if(!(_tmp587 != 0))goto _LL2CA;_LL2C9:
# 1956
 return Cyc_Absynpp_is_anon_aggrtype((void*)_tmp587);_LL2CA:;_LL2CB:
# 1957
 return 0;_LL2C3:;}
# 1963
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 1966
if(tms != 0  && tms->tl != 0){
# 1967
struct _tuple16 _tmpA88;struct _tuple16 _tmp589=(_tmpA88.f1=(void*)tms->hd,((_tmpA88.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,_tmpA88)));void*_tmp58A;void*_tmp58C;_LL2CD: _tmp58A=_tmp589.f1;{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp58B=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp58A;if(_tmp58B->tag != 2)goto _LL2CF;};_tmp58C=_tmp589.f2;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp58D=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp58C;if(_tmp58D->tag != 3)goto _LL2CF;};_LL2CE: {
# 1969
struct Cyc_List_List*_tmpA8B;struct Cyc_List_List*_tmpA8A;return(_tmpA8A=_region_malloc(r,sizeof(*_tmpA8A)),((_tmpA8A->hd=(void*)tms->hd,((_tmpA8A->tl=((_tmpA8B=_region_malloc(r,sizeof(*_tmpA8B)),((_tmpA8B->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,((_tmpA8B->tl=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmpA8B)))))),_tmpA8A)))));}_LL2CF:;_LL2D0: {
# 1970
struct Cyc_List_List*_tmpA8C;return(_tmpA8C=_region_malloc(r,sizeof(*_tmpA8C)),((_tmpA8C->hd=atts,((_tmpA8C->tl=tms,_tmpA8C)))));}_LL2CC:;}else{
# 1972
struct Cyc_List_List*_tmpA8D;return(_tmpA8D=_region_malloc(r,sizeof(*_tmpA8D)),((_tmpA8D->hd=atts,((_tmpA8D->tl=tms,_tmpA8D)))));}}
# 1977
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 1979
void*_tmp592=t;struct Cyc_Absyn_ArrayInfo _tmp594;void*_tmp595;struct Cyc_Absyn_Tqual _tmp596;struct Cyc_Absyn_Exp*_tmp597;union Cyc_Absyn_Constraint*_tmp598;unsigned int _tmp599;struct Cyc_Absyn_PtrInfo _tmp59B;void*_tmp59C;struct Cyc_Absyn_Tqual _tmp59D;struct Cyc_Absyn_PtrAtts _tmp59E;struct Cyc_Absyn_FnInfo _tmp5A0;struct Cyc_List_List*_tmp5A1;void*_tmp5A2;struct Cyc_Absyn_Tqual _tmp5A3;void*_tmp5A4;struct Cyc_List_List*_tmp5A5;int _tmp5A6;struct Cyc_Absyn_VarargInfo*_tmp5A7;struct Cyc_List_List*_tmp5A8;struct Cyc_List_List*_tmp5A9;struct Cyc_Core_Opt*_tmp5AB;void*_tmp5AC;int _tmp5AD;struct _tuple0*_tmp5AF;struct Cyc_List_List*_tmp5B0;struct Cyc_Absyn_Typedefdecl*_tmp5B1;void*_tmp5B2;_LL2D2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp593=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp592;if(_tmp593->tag != 8)goto _LL2D4;else{_tmp594=_tmp593->f1;_tmp595=_tmp594.elt_type;_tmp596=_tmp594.tq;_tmp597=_tmp594.num_elts;_tmp598=_tmp594.zero_term;_tmp599=_tmp594.zt_loc;}}_LL2D3: {
# 1982
struct Cyc_Absyn_Tqual _tmp5B4;void*_tmp5B5;struct Cyc_List_List*_tmp5B6;struct _tuple12 _tmp5B3=Cyc_Absynpp_to_tms(r,_tmp596,_tmp595);_tmp5B4=_tmp5B3.f1;_tmp5B5=_tmp5B3.f2;_tmp5B6=_tmp5B3.f3;{
# 1983
void*tm;
# 1984
if(_tmp597 == 0){
# 1985
struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpA90;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpA8F;tm=(void*)((_tmpA8F=_region_malloc(r,sizeof(*_tmpA8F)),((_tmpA8F[0]=((_tmpA90.tag=0,((_tmpA90.f1=_tmp598,((_tmpA90.f2=_tmp599,_tmpA90)))))),_tmpA8F))));}else{
# 1987
struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpA93;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpA92;tm=(void*)((_tmpA92=_region_malloc(r,sizeof(*_tmpA92)),((_tmpA92[0]=((_tmpA93.tag=1,((_tmpA93.f1=(struct Cyc_Absyn_Exp*)_tmp597,((_tmpA93.f2=_tmp598,((_tmpA93.f3=_tmp599,_tmpA93)))))))),_tmpA92))));}{
# 1988
struct Cyc_List_List*_tmpA96;struct _tuple12 _tmpA95;return(_tmpA95.f1=_tmp5B4,((_tmpA95.f2=_tmp5B5,((_tmpA95.f3=((_tmpA96=_region_malloc(r,sizeof(*_tmpA96)),((_tmpA96->hd=tm,((_tmpA96->tl=_tmp5B6,_tmpA96)))))),_tmpA95)))));};};}_LL2D4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp59A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp592;if(_tmp59A->tag != 5)goto _LL2D6;else{_tmp59B=_tmp59A->f1;_tmp59C=_tmp59B.elt_typ;_tmp59D=_tmp59B.elt_tq;_tmp59E=_tmp59B.ptr_atts;}}_LL2D5: {
# 1991
struct Cyc_Absyn_Tqual _tmp5BE;void*_tmp5BF;struct Cyc_List_List*_tmp5C0;struct _tuple12 _tmp5BD=Cyc_Absynpp_to_tms(r,_tmp59D,_tmp59C);_tmp5BE=_tmp5BD.f1;_tmp5BF=_tmp5BD.f2;_tmp5C0=_tmp5BD.f3;
# 1992
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpA9C;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpA9B;struct Cyc_List_List*_tmpA9A;_tmp5C0=((_tmpA9A=_region_malloc(r,sizeof(*_tmpA9A)),((_tmpA9A->hd=(void*)((_tmpA9C=_region_malloc(r,sizeof(*_tmpA9C)),((_tmpA9C[0]=((_tmpA9B.tag=2,((_tmpA9B.f1=_tmp59E,((_tmpA9B.f2=tq,_tmpA9B)))))),_tmpA9C)))),((_tmpA9A->tl=_tmp5C0,_tmpA9A))))));}{
# 1993
struct _tuple12 _tmpA9D;return(_tmpA9D.f1=_tmp5BE,((_tmpA9D.f2=_tmp5BF,((_tmpA9D.f3=_tmp5C0,_tmpA9D)))));};}_LL2D6: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp59F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp592;if(_tmp59F->tag != 9)goto _LL2D8;else{_tmp5A0=_tmp59F->f1;_tmp5A1=_tmp5A0.tvars;_tmp5A2=_tmp5A0.effect;_tmp5A3=_tmp5A0.ret_tqual;_tmp5A4=_tmp5A0.ret_typ;_tmp5A5=_tmp5A0.args;_tmp5A6=_tmp5A0.c_varargs;_tmp5A7=_tmp5A0.cyc_varargs;_tmp5A8=_tmp5A0.rgn_po;_tmp5A9=_tmp5A0.attributes;}}_LL2D7:
# 1997
 if(!Cyc_Absynpp_print_all_tvars){
# 2001
if(_tmp5A2 == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp5A2)){
# 2002
_tmp5A2=0;
# 2003
_tmp5A1=0;}}else{
# 2006
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2009
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,_tmp5A1);}{
# 2014
struct Cyc_Absyn_Tqual _tmp5C6;void*_tmp5C7;struct Cyc_List_List*_tmp5C8;struct _tuple12 _tmp5C5=Cyc_Absynpp_to_tms(r,_tmp5A3,_tmp5A4);_tmp5C6=_tmp5C5.f1;_tmp5C7=_tmp5C5.f2;_tmp5C8=_tmp5C5.f3;{
# 2015
struct Cyc_List_List*tms=_tmp5C8;
# 2025 "absynpp.cyc"
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2DE:
# 2027
 if(_tmp5A9 != 0){
# 2030
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpAA0;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpA9F;tms=Cyc_Absynpp_bubble_attributes(r,(void*)((_tmpA9F=_region_malloc(r,sizeof(*_tmpA9F)),((_tmpA9F[0]=((_tmpAA0.tag=5,((_tmpAA0.f1=0,((_tmpAA0.f2=_tmp5A9,_tmpAA0)))))),_tmpA9F)))),tms);}
# 2032
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpAAF;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpAAE;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpAAD;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpAAC;struct Cyc_List_List*_tmpAAB;tms=((_tmpAAB=_region_malloc(r,sizeof(*_tmpAAB)),((_tmpAAB->hd=(void*)((_tmpAAF=_region_malloc(r,sizeof(*_tmpAAF)),((_tmpAAF[0]=(
# 2033
(_tmpAAC.tag=3,((_tmpAAC.f1=(void*)((void*)((_tmpAAE=_region_malloc(r,sizeof(*_tmpAAE)),((_tmpAAE[0]=((_tmpAAD.tag=1,((_tmpAAD.f1=_tmp5A5,((_tmpAAD.f2=_tmp5A6,((_tmpAAD.f3=_tmp5A7,((_tmpAAD.f4=(void*)_tmp5A2,((_tmpAAD.f5=_tmp5A8,_tmpAAD)))))))))))),_tmpAAE))))),_tmpAAC)))),_tmpAAF)))),((_tmpAAB->tl=tms,_tmpAAB))))));}
# 2036
break;case Cyc_Cyclone_Vc_c: _LL2DF:
# 2038
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpABE;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpABD;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpABC;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpABB;struct Cyc_List_List*_tmpABA;tms=((_tmpABA=_region_malloc(r,sizeof(*_tmpABA)),((_tmpABA->hd=(void*)((_tmpABE=_region_malloc(r,sizeof(*_tmpABE)),((_tmpABE[0]=(
# 2039
(_tmpABB.tag=3,((_tmpABB.f1=(void*)((void*)((_tmpABD=_region_malloc(r,sizeof(*_tmpABD)),((_tmpABD[0]=((_tmpABC.tag=1,((_tmpABC.f1=_tmp5A5,((_tmpABC.f2=_tmp5A6,((_tmpABC.f3=_tmp5A7,((_tmpABC.f4=(void*)_tmp5A2,((_tmpABC.f5=_tmp5A8,_tmpABC)))))))))))),_tmpABD))))),_tmpABB)))),_tmpABE)))),((_tmpABA->tl=tms,_tmpABA))))));}
# 2042
for(0;_tmp5A9 != 0;_tmp5A9=_tmp5A9->tl){
# 2043
void*_tmp5D5=(void*)_tmp5A9->hd;_LL2E2: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp5D6=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp5D5;if(_tmp5D6->tag != 1)goto _LL2E4;}_LL2E3:
# 2044
 goto _LL2E5;_LL2E4: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp5D7=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp5D5;if(_tmp5D7->tag != 2)goto _LL2E6;}_LL2E5:
# 2045
 goto _LL2E7;_LL2E6: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp5D8=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp5D5;if(_tmp5D8->tag != 3)goto _LL2E8;}_LL2E7:
# 2047
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpAC8;struct Cyc_List_List*_tmpAC7;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpAC6;struct Cyc_List_List*_tmpAC5;tms=((_tmpAC5=_region_malloc(r,sizeof(*_tmpAC5)),((_tmpAC5->hd=(void*)((_tmpAC8=_region_malloc(r,sizeof(*_tmpAC8)),((_tmpAC8[0]=((_tmpAC6.tag=5,((_tmpAC6.f1=0,((_tmpAC6.f2=((_tmpAC7=_cycalloc(sizeof(*_tmpAC7)),((_tmpAC7->hd=(void*)_tmp5A9->hd,((_tmpAC7->tl=0,_tmpAC7)))))),_tmpAC6)))))),_tmpAC8)))),((_tmpAC5->tl=tms,_tmpAC5))))));}
# 2048
goto AfterAtts;_LL2E8:;_LL2E9:
# 2049
 goto _LL2E1;_LL2E1:;}
# 2051
break;}
# 2054
AfterAtts:
# 2055
 if(_tmp5A1 != 0){
# 2056
struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpACE;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpACD;struct Cyc_List_List*_tmpACC;tms=((_tmpACC=_region_malloc(r,sizeof(*_tmpACC)),((_tmpACC->hd=(void*)((_tmpACE=_region_malloc(r,sizeof(*_tmpACE)),((_tmpACE[0]=((_tmpACD.tag=4,((_tmpACD.f1=_tmp5A1,((_tmpACD.f2=0,((_tmpACD.f3=1,_tmpACD)))))))),_tmpACE)))),((_tmpACC->tl=tms,_tmpACC))))));}{
# 2057
struct _tuple12 _tmpACF;return(_tmpACF.f1=_tmp5C6,((_tmpACF.f2=_tmp5C7,((_tmpACF.f3=tms,_tmpACF)))));};};};_LL2D8: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5AA=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp592;if(_tmp5AA->tag != 1)goto _LL2DA;else{_tmp5AB=_tmp5AA->f1;_tmp5AC=(void*)_tmp5AA->f2;_tmp5AD=_tmp5AA->f3;}}_LL2D9:
# 2060
 if(_tmp5AC == 0){
# 2061
struct _tuple12 _tmpAD0;return(_tmpAD0.f1=tq,((_tmpAD0.f2=t,((_tmpAD0.f3=0,_tmpAD0)))));}else{
# 2063
return Cyc_Absynpp_to_tms(r,tq,(void*)_tmp5AC);}_LL2DA: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5AE=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp592;if(_tmp5AE->tag != 17)goto _LL2DC;else{_tmp5AF=_tmp5AE->f1;_tmp5B0=_tmp5AE->f2;_tmp5B1=_tmp5AE->f3;_tmp5B2=(void*)_tmp5AE->f4;}}_LL2DB:
# 2068
 if((_tmp5B2 == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype((void*)_tmp5B2)){
# 2069
struct _tuple12 _tmpAD1;return(_tmpAD1.f1=tq,((_tmpAD1.f2=t,((_tmpAD1.f3=0,_tmpAD1)))));}else{
# 2071
if(tq.real_const)
# 2072
tq.print_const=tq.real_const;
# 2073
return Cyc_Absynpp_to_tms(r,tq,(void*)_tmp5B2);}_LL2DC:;_LL2DD: {
# 2076
struct _tuple12 _tmpAD2;return(_tmpAD2.f1=tq,((_tmpAD2.f2=t,((_tmpAD2.f3=0,_tmpAD2)))));}_LL2D1:;}
# 2080
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2082
void*_tmp5E4=t;void*_tmp5E6;struct Cyc_Absyn_PtrInfo _tmp5E8;void*_tmp5E9;_LL2EB:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5E5=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5E4;if(_tmp5E5->tag != 1)goto _LL2ED;else{_tmp5E6=(void*)_tmp5E5->f2;}}if(!(_tmp5E6 != 0))goto _LL2ED;_LL2EC:
# 2083
 return Cyc_Absynpp_is_char_ptr((void*)_tmp5E6);_LL2ED: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5E7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E4;if(_tmp5E7->tag != 5)goto _LL2EF;else{_tmp5E8=_tmp5E7->f1;_tmp5E9=_tmp5E8.elt_typ;}}_LL2EE:
# 2085
 L: {
# 2086
void*_tmp5EA=_tmp5E9;void*_tmp5EC;void*_tmp5EE;enum Cyc_Absyn_Size_of _tmp5F0;_LL2F2:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5EB=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5EA;if(_tmp5EB->tag != 1)goto _LL2F4;else{_tmp5EC=(void*)_tmp5EB->f2;}}if(!(_tmp5EC != 0))goto _LL2F4;_LL2F3:
# 2087
 _tmp5E9=(void*)_tmp5EC;goto L;_LL2F4:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5ED=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5EA;if(_tmp5ED->tag != 17)goto _LL2F6;else{_tmp5EE=(void*)_tmp5ED->f4;}}if(!(_tmp5EE != 0))goto _LL2F6;_LL2F5:
# 2088
 _tmp5E9=(void*)_tmp5EE;goto L;_LL2F6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5EF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5EA;if(_tmp5EF->tag != 6)goto _LL2F8;else{_tmp5F0=_tmp5EF->f2;if(_tmp5F0 != Cyc_Absyn_Char_sz)goto _LL2F8;}}_LL2F7:
# 2089
 return 1;_LL2F8:;_LL2F9:
# 2090
 return 0;_LL2F1:;}_LL2EF:;_LL2F0:
# 2092
 return 0;_LL2EA:;}
# 2096
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
# 2097
struct _RegionHandle _tmp5F1=_new_region("temp");struct _RegionHandle*temp=& _tmp5F1;_push_region(temp);
# 2098
{struct Cyc_Absyn_Tqual _tmp5F3;void*_tmp5F4;struct Cyc_List_List*_tmp5F5;struct _tuple12 _tmp5F2=Cyc_Absynpp_to_tms(temp,tq,typ);_tmp5F3=_tmp5F2.f1;_tmp5F4=_tmp5F2.f2;_tmp5F5=_tmp5F2.f3;
# 2099
_tmp5F5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5F5);
# 2100
if(_tmp5F5 == 0  && dopt == 0){
# 2101
struct Cyc_PP_Doc*_tmpAD3[2];struct Cyc_PP_Doc*_tmp5F7=(_tmpAD3[1]=Cyc_Absynpp_ntyp2doc(_tmp5F4),((_tmpAD3[0]=Cyc_Absynpp_tqual2doc(_tmp5F3),Cyc_PP_cat(_tag_dyneither(_tmpAD3,sizeof(struct Cyc_PP_Doc*),2)))));_npop_handler(0);return _tmp5F7;}else{
# 2103
const char*_tmpAD6;struct Cyc_PP_Doc*_tmpAD5[4];struct Cyc_PP_Doc*_tmp5FA=
# 2104
(_tmpAD5[3]=
# 2107
Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp5F5),((_tmpAD5[2]=
# 2106
Cyc_PP_text(((_tmpAD6=" ",_tag_dyneither(_tmpAD6,sizeof(char),2)))),((_tmpAD5[1]=
# 2105
Cyc_Absynpp_ntyp2doc(_tmp5F4),((_tmpAD5[0]=
# 2104
Cyc_Absynpp_tqual2doc(_tmp5F3),Cyc_PP_cat(_tag_dyneither(_tmpAD5,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);return _tmp5FA;}}
# 2098
;_pop_region(temp);}
# 2111
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
# 2112
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2FA:
# 2114
 if(f->width != 0){
# 2115
struct Cyc_Core_Opt*_tmpADD;const char*_tmpADC;const char*_tmpADB;struct Cyc_PP_Doc*_tmpADA[5];return(_tmpADA[4]=
# 2117
Cyc_PP_text(((_tmpADB=";",_tag_dyneither(_tmpADB,sizeof(char),2)))),((_tmpADA[3]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpADA[2]=
# 2116
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpADA[1]=Cyc_PP_text(((_tmpADC=":",_tag_dyneither(_tmpADC,sizeof(char),2)))),((_tmpADA[0]=
# 2115
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpADD=_cycalloc(sizeof(*_tmpADD)),((_tmpADD->v=Cyc_PP_textptr(f->name),_tmpADD))))),Cyc_PP_cat(_tag_dyneither(_tmpADA,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2119
struct Cyc_Core_Opt*_tmpAE2;const char*_tmpAE1;struct Cyc_PP_Doc*_tmpAE0[3];return(_tmpAE0[2]=
# 2120
Cyc_PP_text(((_tmpAE1=";",_tag_dyneither(_tmpAE1,sizeof(char),2)))),((_tmpAE0[1]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpAE0[0]=
# 2119
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAE2=_cycalloc(sizeof(*_tmpAE2)),((_tmpAE2->v=Cyc_PP_textptr(f->name),_tmpAE2))))),Cyc_PP_cat(_tag_dyneither(_tmpAE0,sizeof(struct Cyc_PP_Doc*),3)))))));}case Cyc_Cyclone_Vc_c: _LL2FB:
# 2122
 if(f->width != 0){
# 2123
struct Cyc_Core_Opt*_tmpAE9;const char*_tmpAE8;const char*_tmpAE7;struct Cyc_PP_Doc*_tmpAE6[5];return(_tmpAE6[4]=
# 2125
Cyc_PP_text(((_tmpAE7=";",_tag_dyneither(_tmpAE7,sizeof(char),2)))),((_tmpAE6[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpAE6[2]=Cyc_PP_text(((_tmpAE8=":",_tag_dyneither(_tmpAE8,sizeof(char),2)))),((_tmpAE6[1]=
# 2124
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAE9=_cycalloc(sizeof(*_tmpAE9)),((_tmpAE9->v=Cyc_PP_textptr(f->name),_tmpAE9))))),((_tmpAE6[0]=
# 2123
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpAE6,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2127
struct Cyc_Core_Opt*_tmpAEE;const char*_tmpAED;struct Cyc_PP_Doc*_tmpAEC[3];return(_tmpAEC[2]=
# 2128
Cyc_PP_text(((_tmpAED=";",_tag_dyneither(_tmpAED,sizeof(char),2)))),((_tmpAEC[1]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAEE=_cycalloc(sizeof(*_tmpAEE)),((_tmpAEE->v=Cyc_PP_textptr(f->name),_tmpAEE))))),((_tmpAEC[0]=
# 2127
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpAEC,sizeof(struct Cyc_PP_Doc*),3)))))));}}}
# 2133
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
# 2134
const char*_tmpAEF;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((_tmpAEF="",_tag_dyneither(_tmpAEF,sizeof(char),1))),fields);}
# 2137
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
# 2138
struct Cyc_PP_Doc*_tmpAF0[3];return(_tmpAF0[2]=
# 2139
f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((_tmpAF0[1]=
# 2138
Cyc_Absynpp_typedef_name2doc(f->name),((_tmpAF0[0]=Cyc_Absynpp_scope2doc(f->sc),Cyc_PP_cat(_tag_dyneither(_tmpAF0,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 2142
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
# 2143
const char*_tmpAF1;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((_tmpAF1=",",_tag_dyneither(_tmpAF1,sizeof(char),2))),fields);}
# 2151
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
# 2152
for(0;tdl != 0;tdl=tdl->tl){
# 2153
Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{
# 2154
const char*_tmpAF4;void*_tmpAF3;(_tmpAF3=0,Cyc_fprintf(f,((_tmpAF4="\n",_tag_dyneither(_tmpAF4,sizeof(char),2))),_tag_dyneither(_tmpAF3,sizeof(void*),0)));};}}
# 2158
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
# 2159
const char*_tmpAF5;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmpAF5="",_tag_dyneither(_tmpAF5,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),72);}
# 2161
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
# 2162
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0),72);}
# 2163
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}
# 2165
struct _dyneither_ptr Cyc_Absynpp_typ2cstring(void*t){
# 2166
int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;
# 2167
int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
# 2168
Cyc_Absynpp_qvar_to_Cids=1;
# 2169
Cyc_Absynpp_add_cyc_prefix=0;{
# 2170
struct _dyneither_ptr s=Cyc_Absynpp_typ2string(t);
# 2171
Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;
# 2172
Cyc_Absynpp_add_cyc_prefix=old_add_cyc_prefix;
# 2173
return s;};}
# 2175
struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
# 2176
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
# 2177
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
