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
typedef int Cyc_bool;
# 69 "core.h"
void exit(int);struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};
# 26 "cycboot.h"
typedef unsigned long Cyc_size_t;
# 33
typedef unsigned short Cyc_mode_t;struct Cyc___cycFILE;
# 49
typedef struct Cyc___cycFILE Cyc_FILE;
# 51
extern struct Cyc___cycFILE*Cyc_stdout;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 79
int Cyc_fclose(struct Cyc___cycFILE*);
# 84
int Cyc_feof(struct Cyc___cycFILE*);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 90
int Cyc_fgetc(struct Cyc___cycFILE*);
# 98
struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
# 108
unsigned long Cyc_fread(struct _dyneither_ptr,unsigned long,unsigned long,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 140 "cycboot.h"
unsigned long Cyc_fwrite(struct _dyneither_ptr,unsigned long,unsigned long,struct Cyc___cycFILE*);
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);
# 167
int remove(const char*);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 271 "cycboot.h"
struct Cyc___cycFILE*Cyc_file_open(struct _dyneither_ptr,struct _dyneither_ptr);
# 272
void Cyc_file_close(struct Cyc___cycFILE*);
# 300
int isspace(int);
# 313
char*getenv(const char*);
# 318
int system(const char*);
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 162 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};
# 234 "core.h"
struct _dyneither_ptr Cstring_to_string(char*);
# 244
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
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 160
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 319
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 347
struct _dyneither_ptr Cyc_List_to_array(struct Cyc_List_List*x);
# 374
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);extern char Cyc_Arg_Bad[4];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6];struct Cyc_Arg_Error_exn_struct{char*tag;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
# 55 "arg.h"
typedef void*Cyc_Arg_spec_t;
# 57
typedef struct Cyc_List_List*Cyc_Arg_speclist_t;
# 69
extern int Cyc_Arg_current;
# 71
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr args);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
# 50
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 51
int Cyc_strncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 64
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);
# 102 "string.h"
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);
# 107
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);
# 118
struct _dyneither_ptr Cyc_strchr(struct _dyneither_ptr s,char c);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 30 "filename.h"
struct _dyneither_ptr Cyc_Filename_concat(struct _dyneither_ptr,struct _dyneither_ptr);
# 34
struct _dyneither_ptr Cyc_Filename_chop_extension(struct _dyneither_ptr);
# 40
struct _dyneither_ptr Cyc_Filename_dirname(struct _dyneither_ptr);
# 43
struct _dyneither_ptr Cyc_Filename_basename(struct _dyneither_ptr);
# 46
int Cyc_Filename_check_suffix(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 32 "position.h"
void Cyc_Position_reset_position(struct _dyneither_ptr);
# 37
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 61
extern int Cyc_Position_use_gcc_style_location;
# 63
extern int Cyc_Position_max_errors;
# 65
int Cyc_Position_error_p();
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1171 "absyn.h"
extern int Cyc_Absyn_porting_c_code;
# 1173
extern int Cyc_Absyn_no_regions;extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 57 "lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
# 32 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
# 33
extern int Cyc_Parse_no_register;struct _tuple10{struct _tuple10*tl;};struct Cyc_FlatList{struct _tuple10*tl;};
# 36
typedef struct _tuple10*Cyc_flat_list_t;struct Cyc_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 47
typedef struct Cyc_Type_specifier Cyc_type_specifier_t;struct Cyc_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};
# 52
typedef struct Cyc_Declarator Cyc_declarator_t;struct _tuple11{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple12{struct _tuple12*tl;struct _tuple11 hd;};
# 53
typedef struct _tuple12*Cyc_declarator_list_t;
# 55
enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc  = 1,Cyc_ExternC_sc  = 2,Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,Cyc_Abstract_sc  = 6};
# 59
typedef enum Cyc_Storage_class Cyc_storage_class_t;struct Cyc_Declaration_spec{enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};
# 67
typedef struct Cyc_Declaration_spec Cyc_decl_spec_t;struct Cyc_Abstractdeclarator{struct Cyc_List_List*tms;};
# 71
typedef struct Cyc_Abstractdeclarator Cyc_abstractdeclarator_t;struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple13{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple13 val;};struct _tuple14{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple15{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY16{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY20{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY21{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY24{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_Declarator val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple17{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple17 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple18*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 75 "parse_tab.h"
typedef struct Cyc_Yyltype Cyc_yyltype;struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 56 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 58
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;
# 60
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 31 "absyndump.h"
void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs);
# 32
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 114 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
# 44 "tcutil.h"
void Cyc_Tcutil_flush_warnings();
# 83
extern int Cyc_Tcutil_warn_alias_coerce;
# 86
extern int Cyc_Tcutil_warn_region_coerce;
# 338 "tcutil.h"
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 34 "tc.h"
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 38
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
# 32 "toc.h"
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);
# 27 "tovc.h"
extern int Cyc_Tovc_elim_array_initializers;
# 28
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*decls);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 49 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 60
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 62
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 67
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple19{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple19 val;};struct _union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct _union_RelnOp_LessEqConst LessEqConst;};
# 79
typedef union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_reln_op_t;struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp rop;};
# 94
typedef struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_reln_t;
# 95
typedef struct Cyc_List_List*Cyc_CfFlowInfo_relns_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 108
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
# 113
typedef void*Cyc_CfFlowInfo_absRval_t;
# 114
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
# 115
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 120
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};
# 135
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 142
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple20{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple20 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 158 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 177
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 256 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 35 "new_control_flow.h"
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);
# 39
extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_Interface_I;
# 37 "interface.h"
typedef struct Cyc_Interface_I*Cyc_Interface_i_t;
# 40
struct Cyc_Interface_I*Cyc_Interface_empty();
# 49 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_final();
# 53
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict ae);struct _tuple21{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
# 61
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple21*info);
# 76 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo);
# 82
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);
# 85
void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
# 88
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);
# 91
typedef struct Cyc_Interface_I*Cyc_interface_t;
# 25 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 27
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};
# 33
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;
# 42 "cyclone.cyc"
void Cyc_Port_port(struct Cyc_List_List*);
# 45
extern int Cyc_Lex_compile_for_boot_r;
# 46
void Cyc_Lex_pos_init();
# 47
void Cyc_Lex_lex_init(int use_cyclone_keywords);
# 50
int Cyc_Toc_warn_bounds_checks;
# 51
int Cyc_Toc_warn_all_null_deref;
# 60
static int Cyc_pp_r=0;
# 61
int Cyc_noexpand_r=0;
# 62
static int Cyc_noshake_r=0;
# 63
static int Cyc_stop_after_cpp_r=0;
# 64
static int Cyc_parseonly_r=0;
# 65
static int Cyc_tc_r=0;
# 66
static int Cyc_cf_r=0;
# 67
static int Cyc_noprint_r=0;
# 68
static int Cyc_ic_r=0;
# 69
static int Cyc_toc_r=0;
# 70
static int Cyc_stop_after_objectfile_r=0;
# 71
static int Cyc_stop_after_asmfile_r=0;
# 72
static int Cyc_inline_functions_r=0;
# 73
static int Cyc_elim_se_r=0;
# 74
static int Cyc_v_r=0;
# 75
static int Cyc_save_temps_r=0;
# 76
static int Cyc_save_c_r=0;
# 77
static int Cyc_nogc_r=0;
# 78
static int Cyc_pa_r=0;
# 79
static int Cyc_pg_r=0;
# 80
static int Cyc_nocheck_r=0;
# 81
static int Cyc_add_cyc_namespace_r=1;
# 82
static int Cyc_generate_line_directives_r=1;
# 83
static int Cyc_print_full_evars_r=0;
# 84
static int Cyc_print_all_tvars_r=0;
# 85
static int Cyc_print_all_kinds_r=0;
# 86
static int Cyc_print_all_effects_r=0;
# 87
static int Cyc_nocyc_setjmp_r=0;
# 90
static struct Cyc_List_List*Cyc_ccargs=0;
# 91
static void Cyc_add_ccarg(struct _dyneither_ptr s){
# 92
struct _dyneither_ptr*_tmp3BD;struct Cyc_List_List*_tmp3BC;Cyc_ccargs=((_tmp3BC=_cycalloc(sizeof(*_tmp3BC)),((_tmp3BC->hd=((_tmp3BD=_cycalloc(sizeof(*_tmp3BD)),((_tmp3BD[0]=s,_tmp3BD)))),((_tmp3BC->tl=Cyc_ccargs,_tmp3BC))))));}
# 98
void Cyc_set_c_compiler(struct _dyneither_ptr s){
# 99
const char*_tmp3BE;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3BE="vc",_tag_dyneither(_tmp3BE,sizeof(char),3))))== 0){
# 100
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Vc_c;{
# 101
const char*_tmp3BF;Cyc_add_ccarg(((_tmp3BF="-DVC_C",_tag_dyneither(_tmp3BF,sizeof(char),7))));};}else{
# 103
const char*_tmp3C0;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3C0="gcc",_tag_dyneither(_tmp3C0,sizeof(char),4))))== 0){
# 104
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;{
# 105
const char*_tmp3C1;Cyc_add_ccarg(((_tmp3C1="-DGCC_C",_tag_dyneither(_tmp3C1,sizeof(char),8))));};}}}
# 111
void Cyc_set_port_c_code(){
# 112
Cyc_Absyn_porting_c_code=1;
# 113
Cyc_Position_max_errors=65535;
# 114
Cyc_save_c_r=1;
# 115
Cyc_parseonly_r=1;}
# 119
static struct _dyneither_ptr*Cyc_output_file=0;
# 120
static void Cyc_set_output_file(struct _dyneither_ptr s){
# 121
struct _dyneither_ptr*_tmp3C2;Cyc_output_file=((_tmp3C2=_cycalloc(sizeof(*_tmp3C2)),((_tmp3C2[0]=s,_tmp3C2))));}
# 128
extern char*Cdef_inc_path;
# 129
extern char*Cdef_lib_path;
# 130
extern char*Carch;
# 131
extern char*Ccomp;
# 132
extern char*Cversion;static char _tmp7[1]="";
# 135
static struct _dyneither_ptr Cyc_cpp={_tmp7,_tmp7,_tmp7 + 1};
# 136
static void Cyc_set_cpp(struct _dyneither_ptr s){
# 137
Cyc_cpp=s;}
# 140
static struct Cyc_List_List*Cyc_cppargs=0;
# 141
static void Cyc_add_cpparg(struct _dyneither_ptr s){
# 142
struct _dyneither_ptr*_tmp3C5;struct Cyc_List_List*_tmp3C4;Cyc_cppargs=((_tmp3C4=_cycalloc(sizeof(*_tmp3C4)),((_tmp3C4->hd=((_tmp3C5=_cycalloc(sizeof(*_tmp3C5)),((_tmp3C5[0]=s,_tmp3C5)))),((_tmp3C4->tl=Cyc_cppargs,_tmp3C4))))));}
# 145
static void Cyc_add_iprefix(struct _dyneither_ptr s){
# 146
const char*_tmp3C9;void*_tmp3C8[1];struct Cyc_String_pa_PrintArg_struct _tmp3C7;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3C7.tag=0,((_tmp3C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3C8[0]=& _tmp3C7,Cyc_aprintf(((_tmp3C9="-iprefix %s",_tag_dyneither(_tmp3C9,sizeof(char),12))),_tag_dyneither(_tmp3C8,sizeof(void*),1)))))))));}
# 148
static void Cyc_add_iwithprefix(struct _dyneither_ptr s){
# 149
const char*_tmp3CD;void*_tmp3CC[1];struct Cyc_String_pa_PrintArg_struct _tmp3CB;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3CB.tag=0,((_tmp3CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3CC[0]=& _tmp3CB,Cyc_aprintf(((_tmp3CD="-iwithprefix %s",_tag_dyneither(_tmp3CD,sizeof(char),16))),_tag_dyneither(_tmp3CC,sizeof(void*),1)))))))));}
# 151
static void Cyc_add_iwithprefixbefore(struct _dyneither_ptr s){
# 152
const char*_tmp3D1;void*_tmp3D0[1];struct Cyc_String_pa_PrintArg_struct _tmp3CF;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3CF.tag=0,((_tmp3CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3D0[0]=& _tmp3CF,Cyc_aprintf(((_tmp3D1="-iwithprefixbefore %s",_tag_dyneither(_tmp3D1,sizeof(char),22))),_tag_dyneither(_tmp3D0,sizeof(void*),1)))))))));}
# 154
static void Cyc_add_isystem(struct _dyneither_ptr s){
# 155
const char*_tmp3D5;void*_tmp3D4[1];struct Cyc_String_pa_PrintArg_struct _tmp3D3;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3D3.tag=0,((_tmp3D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3D4[0]=& _tmp3D3,Cyc_aprintf(((_tmp3D5="-isystem %s",_tag_dyneither(_tmp3D5,sizeof(char),12))),_tag_dyneither(_tmp3D4,sizeof(void*),1)))))))));}
# 157
static void Cyc_add_idirafter(struct _dyneither_ptr s){
# 158
const char*_tmp3D9;void*_tmp3D8[1];struct Cyc_String_pa_PrintArg_struct _tmp3D7;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3D7.tag=0,((_tmp3D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3D8[0]=& _tmp3D7,Cyc_aprintf(((_tmp3D9="-idirafter %s",_tag_dyneither(_tmp3D9,sizeof(char),14))),_tag_dyneither(_tmp3D8,sizeof(void*),1)))))))));}
# 161
static struct _dyneither_ptr Cyc_target_arch={(void*)0,(void*)0,(void*)(0 + 0)};
# 162
static void Cyc_set_target_arch(struct _dyneither_ptr s){
# 163
Cyc_toc_r=1;
# 164
Cyc_target_arch=s;}
# 168
static void Cyc_print_version(){
# 169
{const char*_tmp3DD;void*_tmp3DC[1];struct Cyc_String_pa_PrintArg_struct _tmp3DB;(_tmp3DB.tag=0,((_tmp3DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cversion)),((_tmp3DC[0]=& _tmp3DB,Cyc_printf(((_tmp3DD="The Cyclone compiler, version %s\n",_tag_dyneither(_tmp3DD,sizeof(char),34))),_tag_dyneither(_tmp3DC,sizeof(void*),1)))))));}
# 170
{const char*_tmp3E2;void*_tmp3E1[2];struct Cyc_String_pa_PrintArg_struct _tmp3E0;struct Cyc_String_pa_PrintArg_struct _tmp3DF;(_tmp3DF.tag=0,((_tmp3DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 171
Cstring_to_string(Ccomp)),((_tmp3E0.tag=0,((_tmp3E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Carch)),((_tmp3E1[0]=& _tmp3E0,((_tmp3E1[1]=& _tmp3DF,Cyc_printf(((_tmp3E2="Compiled for architecture: %s CC=\"%s\"\n",_tag_dyneither(_tmp3E2,sizeof(char),39))),_tag_dyneither(_tmp3E1,sizeof(void*),2)))))))))))));}
# 172
{const char*_tmp3E6;void*_tmp3E5[1];struct Cyc_String_pa_PrintArg_struct _tmp3E4;(_tmp3E4.tag=0,((_tmp3E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cdef_lib_path)),((_tmp3E5[0]=& _tmp3E4,Cyc_printf(((_tmp3E6="Standard library directory: %s\n",_tag_dyneither(_tmp3E6,sizeof(char),32))),_tag_dyneither(_tmp3E5,sizeof(void*),1)))))));}
# 173
{const char*_tmp3EA;void*_tmp3E9[1];struct Cyc_String_pa_PrintArg_struct _tmp3E8;(_tmp3E8.tag=0,((_tmp3E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path)),((_tmp3E9[0]=& _tmp3E8,Cyc_printf(((_tmp3EA="Standard include directory: %s\n",_tag_dyneither(_tmp3EA,sizeof(char),32))),_tag_dyneither(_tmp3E9,sizeof(void*),1)))))));}
# 174
exit(0);}
# 177
static int Cyc_is_cyclone_sourcefile(struct _dyneither_ptr s){
# 178
unsigned long _tmp26=Cyc_strlen((struct _dyneither_ptr)s);
# 179
if(_tmp26 <= 4)return 0;else{
# 180
const char*_tmp3EB;return Cyc_strcmp((struct _dyneither_ptr)_dyneither_ptr_plus(s,sizeof(char),(int)(_tmp26 - 4)),((_tmp3EB=".cyc",_tag_dyneither(_tmp3EB,sizeof(char),5))))== 0;}}
# 183
static struct Cyc_List_List*Cyc_cyclone_exec_path=0;
# 184
static void Cyc_add_cyclone_exec_path(struct _dyneither_ptr s){
# 185
unsigned long _tmp28=Cyc_strlen((struct _dyneither_ptr)s);
# 186
if(_tmp28 <= 2)return;{
# 187
struct _dyneither_ptr _tmp29=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,2,_tmp28 - 2);
# 188
struct _dyneither_ptr*_tmp3EE;struct Cyc_List_List*_tmp3ED;Cyc_cyclone_exec_path=((_tmp3ED=_cycalloc(sizeof(*_tmp3ED)),((_tmp3ED->hd=((_tmp3EE=_cycalloc(sizeof(*_tmp3EE)),((_tmp3EE[0]=_tmp29,_tmp3EE)))),((_tmp3ED->tl=Cyc_cyclone_exec_path,_tmp3ED))))));};}
# 196
static struct Cyc_List_List*Cyc_libargs=0;
# 197
static void Cyc_add_libarg(struct _dyneither_ptr s){
# 198
const char*_tmp3EF;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3EF="-lxml",_tag_dyneither(_tmp3EF,sizeof(char),6))))== 0){
# 199
if(!Cyc_pa_r)
# 200
Cyc_add_ccarg(s);else{
# 202
const char*_tmp3F0;Cyc_add_ccarg(((_tmp3F0="-lxml_a",_tag_dyneither(_tmp3F0,sizeof(char),8))));}}else{
# 204
struct _dyneither_ptr*_tmp3F3;struct Cyc_List_List*_tmp3F2;Cyc_libargs=((_tmp3F2=_cycalloc(sizeof(*_tmp3F2)),((_tmp3F2->hd=((_tmp3F3=_cycalloc(sizeof(*_tmp3F3)),((_tmp3F3[0]=s,_tmp3F3)))),((_tmp3F2->tl=Cyc_libargs,_tmp3F2))))));}}
# 207
static void Cyc_add_marg(struct _dyneither_ptr s){
# 208
Cyc_add_ccarg(s);}
# 211
static void Cyc_set_save_temps(){
# 212
Cyc_save_temps_r=1;{
# 213
const char*_tmp3F4;Cyc_add_ccarg(((_tmp3F4="-save-temps",_tag_dyneither(_tmp3F4,sizeof(char),12))));};}
# 216
static int Cyc_produce_dependencies=0;
# 217
static void Cyc_set_produce_dependencies(){
# 218
Cyc_stop_after_cpp_r=1;
# 219
Cyc_produce_dependencies=1;{
# 220
const char*_tmp3F5;Cyc_add_cpparg(((_tmp3F5="-M",_tag_dyneither(_tmp3F5,sizeof(char),3))));};}
# 223
static struct _dyneither_ptr*Cyc_dependencies_target=0;
# 224
static void Cyc_set_dependencies_target(struct _dyneither_ptr s){
# 225
struct _dyneither_ptr*_tmp3F6;Cyc_dependencies_target=((_tmp3F6=_cycalloc(sizeof(*_tmp3F6)),((_tmp3F6[0]=s,_tmp3F6))));}
# 228
static void Cyc_set_stop_after_objectfile(){
# 229
Cyc_stop_after_objectfile_r=1;{
# 230
const char*_tmp3F7;Cyc_add_ccarg(((_tmp3F7="-c",_tag_dyneither(_tmp3F7,sizeof(char),3))));};}
# 233
static void Cyc_set_nocppprecomp(){
# 234
{const char*_tmp3F8;Cyc_add_cpparg(((_tmp3F8="-no-cpp-precomp",_tag_dyneither(_tmp3F8,sizeof(char),16))));}{
# 235
const char*_tmp3F9;Cyc_add_ccarg(((_tmp3F9="-no-cpp-precomp",_tag_dyneither(_tmp3F9,sizeof(char),16))));};}
# 238
static void Cyc_clear_lineno(){
# 239
Cyc_generate_line_directives_r=0;
# 240
Cyc_set_save_temps();}
# 242
static void Cyc_set_inline_functions(){
# 243
Cyc_inline_functions_r=1;}
# 245
static void Cyc_set_elim_se(){
# 246
Cyc_elim_se_r=1;
# 247
Cyc_set_inline_functions();}
# 249
static void Cyc_set_tovc(){
# 250
Cyc_Cyclone_tovc_r=1;
# 251
{const char*_tmp3FA;Cyc_add_ccarg(((_tmp3FA="-DCYC_ANSI_OUTPUT",_tag_dyneither(_tmp3FA,sizeof(char),18))));}
# 252
Cyc_set_elim_se();}
# 254
static void Cyc_set_noboundschecks(){
# 255
const char*_tmp3FB;Cyc_add_ccarg(((_tmp3FB="-DNO_CYC_BOUNDS_CHECKS",_tag_dyneither(_tmp3FB,sizeof(char),23))));}
# 257
static void Cyc_set_nonullchecks(){
# 258
const char*_tmp3FC;Cyc_add_ccarg(((_tmp3FC="-DNO_CYC_NULL_CHECKS",_tag_dyneither(_tmp3FC,sizeof(char),21))));}
# 260
static void Cyc_set_nochecks(){
# 261
Cyc_set_noboundschecks();
# 262
Cyc_set_nonullchecks();
# 263
Cyc_nocheck_r=1;}
# 266
static void Cyc_set_nocyc(){
# 267
Cyc_add_cyc_namespace_r=0;{
# 268
const char*_tmp3FD;Cyc_add_ccarg(((_tmp3FD="-DNO_CYC_PREFIX",_tag_dyneither(_tmp3FD,sizeof(char),16))));};}
# 271
static void Cyc_set_pa(){
# 272
Cyc_pa_r=1;
# 273
{const char*_tmp3FE;Cyc_add_ccarg(((_tmp3FE="-DCYC_REGION_PROFILE",_tag_dyneither(_tmp3FE,sizeof(char),21))));}{
# 274
const char*_tmp3FF;Cyc_add_cpparg(((_tmp3FF="-DCYC_REGION_PROFILE",_tag_dyneither(_tmp3FF,sizeof(char),21))));};}
# 277
static void Cyc_set_pg(){
# 278
Cyc_pg_r=1;{
# 279
const char*_tmp400;Cyc_add_ccarg(((_tmp400="-pg",_tag_dyneither(_tmp400,sizeof(char),4))));};}
# 282
static void Cyc_set_stop_after_asmfile(){
# 283
Cyc_stop_after_asmfile_r=1;{
# 284
const char*_tmp401;Cyc_add_ccarg(((_tmp401="-S",_tag_dyneither(_tmp401,sizeof(char),3))));};}
# 287
static void Cyc_set_all_warnings(){
# 288
Cyc_Toc_warn_bounds_checks=1;
# 289
Cyc_Toc_warn_all_null_deref=1;
# 290
Cyc_NewControlFlow_warn_lose_unique=1;
# 291
Cyc_Tcutil_warn_alias_coerce=1;
# 292
Cyc_Tcutil_warn_region_coerce=1;}
# 296
enum Cyc_inputtype{Cyc_DEFAULTINPUT  = 0,Cyc_CYCLONEFILE  = 1};
# 300
typedef enum Cyc_inputtype Cyc_inputtype_t;
# 301
static enum Cyc_inputtype Cyc_intype=Cyc_DEFAULTINPUT;
# 302
static void Cyc_set_inputtype(struct _dyneither_ptr s){
# 303
const char*_tmp402;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp402="cyc",_tag_dyneither(_tmp402,sizeof(char),4))))== 0)
# 304
Cyc_intype=Cyc_CYCLONEFILE;else{
# 305
const char*_tmp403;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp403="none",_tag_dyneither(_tmp403,sizeof(char),5))))== 0)
# 306
Cyc_intype=Cyc_DEFAULTINPUT;else{
# 308
const char*_tmp407;void*_tmp406[1];struct Cyc_String_pa_PrintArg_struct _tmp405;(_tmp405.tag=0,((_tmp405.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp406[0]=& _tmp405,Cyc_fprintf(Cyc_stderr,((_tmp407="Input type '%s' not supported\n",_tag_dyneither(_tmp407,sizeof(char),31))),_tag_dyneither(_tmp406,sizeof(void*),1)))))));}}}
# 312
struct _dyneither_ptr Cyc_make_base_filename(struct _dyneither_ptr s,struct _dyneither_ptr*output_file){
# 313
struct _dyneither_ptr _tmp43=Cyc_Filename_dirname(output_file == 0?s:*output_file);
# 314
struct _dyneither_ptr _tmp44=Cyc_Filename_chop_extension((struct _dyneither_ptr)Cyc_Filename_basename(s));
# 318
struct _dyneither_ptr _tmp45=Cyc_strlen((struct _dyneither_ptr)_tmp43)> 0?Cyc_Filename_concat((struct _dyneither_ptr)_tmp43,(struct _dyneither_ptr)_tmp44): _tmp44;
# 319
return(struct _dyneither_ptr)_tmp45;}
# 324
static struct Cyc_List_List*Cyc_cyclone_files=0;
# 326
static void Cyc_add_other(struct _dyneither_ptr s){
# 327
if(Cyc_intype == Cyc_CYCLONEFILE  || Cyc_is_cyclone_sourcefile(s)){
# 329
{struct _dyneither_ptr*_tmp40A;struct Cyc_List_List*_tmp409;Cyc_cyclone_files=((_tmp409=_cycalloc(sizeof(*_tmp409)),((_tmp409->hd=((_tmp40A=_cycalloc(sizeof(*_tmp40A)),((_tmp40A[0]=s,_tmp40A)))),((_tmp409->tl=Cyc_cyclone_files,_tmp409))))));}{
# 332
struct _dyneither_ptr _tmp48=Cyc_make_base_filename(s,Cyc_output_file);
# 333
const char*_tmp40B;struct _dyneither_ptr _tmp49=Cyc_strconcat((struct _dyneither_ptr)_tmp48,((_tmp40B=".c",_tag_dyneither(_tmp40B,sizeof(char),3))));
# 334
Cyc_add_ccarg((struct _dyneither_ptr)_tmp49);};}else{
# 336
Cyc_add_ccarg(s);}}
# 339
static void Cyc_remove_file(struct _dyneither_ptr s){
# 340
if(Cyc_save_temps_r)return;else{
# 341
remove((const char*)_untag_dyneither_ptr(s,sizeof(char),1));}}struct _tuple22{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;};
# 350
struct Cyc_List_List*Cyc_read_specs(struct _dyneither_ptr file){
# 351
struct Cyc_List_List*_tmp4B=0;
# 352
int c;
# 353
int i;
# 354
char strname[256];
# 355
char strvalue[4096];
# 356
struct Cyc___cycFILE*spec_file=Cyc_fopen((const char*)_untag_dyneither_ptr(file,sizeof(char),1),(const char*)"r");
# 358
if(spec_file == 0){
# 359
{const char*_tmp40F;void*_tmp40E[1];struct Cyc_String_pa_PrintArg_struct _tmp40D;(_tmp40D.tag=0,((_tmp40D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp40E[0]=& _tmp40D,Cyc_fprintf(Cyc_stderr,((_tmp40F="Error opening spec file %s\n",_tag_dyneither(_tmp40F,sizeof(char),28))),_tag_dyneither(_tmp40E,sizeof(void*),1)))))));}
# 360
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
# 361
return 0;}
# 364
while(1){
# 365
while(1){
# 366
c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);
# 367
if(isspace(c))continue;
# 368
if(c == '*')break;
# 369
if(c != - 1){
# 370
{const char*_tmp414;void*_tmp413[2];struct Cyc_String_pa_PrintArg_struct _tmp412;struct Cyc_Int_pa_PrintArg_struct _tmp411;(_tmp411.tag=1,((_tmp411.f1=(unsigned long)c,((_tmp412.tag=0,((_tmp412.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp413[0]=& _tmp412,((_tmp413[1]=& _tmp411,Cyc_fprintf(Cyc_stderr,((_tmp414="Error reading spec file %s: unexpected character '%c'\n",_tag_dyneither(_tmp414,sizeof(char),55))),_tag_dyneither(_tmp413,sizeof(void*),2)))))))))))));}
# 373
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}
# 375
goto CLEANUP_AND_RETURN;}
# 377
JUST_AFTER_STAR:
# 378
 i=0;
# 379
while(1){
# 380
c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);
# 381
if(c == - 1){
# 382
{const char*_tmp418;void*_tmp417[1];struct Cyc_String_pa_PrintArg_struct _tmp416;(_tmp416.tag=0,((_tmp416.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp417[0]=& _tmp416,Cyc_fprintf(Cyc_stderr,((_tmp418="Error reading spec file %s: unexpected EOF\n",_tag_dyneither(_tmp418,sizeof(char),44))),_tag_dyneither(_tmp417,sizeof(void*),1)))))));}
# 385
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
# 386
goto CLEANUP_AND_RETURN;}
# 388
if(c == ':'){
# 389
strname[_check_known_subscript_notnull(256,i)]='\000';
# 390
break;}
# 392
strname[_check_known_subscript_notnull(256,i)]=(char)c;
# 393
++ i;
# 394
if(i >= 256){
# 395
{const char*_tmp41C;void*_tmp41B[1];struct Cyc_String_pa_PrintArg_struct _tmp41A;(_tmp41A.tag=0,((_tmp41A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp41B[0]=& _tmp41A,Cyc_fprintf(Cyc_stderr,((_tmp41C="Error reading spec file %s: string name too long\n",_tag_dyneither(_tmp41C,sizeof(char),50))),_tag_dyneither(_tmp41B,sizeof(void*),1)))))));}
# 398
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
# 399
goto CLEANUP_AND_RETURN;}}
# 402
while(1){
# 403
c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);
# 404
if(isspace(c))continue;
# 405
break;}
# 407
if(c == '*'){
# 409
{struct _tuple22*_tmp429;struct _dyneither_ptr*_tmp428;const char*_tmp427;struct _dyneither_ptr*_tmp426;struct Cyc_List_List*_tmp425;_tmp4B=((_tmp425=_cycalloc(sizeof(*_tmp425)),((_tmp425->hd=((_tmp429=_cycalloc(sizeof(*_tmp429)),((_tmp429->f1=((_tmp428=_cycalloc(sizeof(*_tmp428)),((_tmp428[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256)),_tmp428)))),((_tmp429->f2=(
# 410
(_tmp426=_cycalloc(sizeof(*_tmp426)),((_tmp426[0]=(struct _dyneither_ptr)Cyc_strdup(((_tmp427="",_tag_dyneither(_tmp427,sizeof(char),1)))),_tmp426)))),_tmp429)))))),((_tmp425->tl=_tmp4B,_tmp425))))));}
# 411
goto JUST_AFTER_STAR;}
# 414
strvalue[0]=(char)c;
# 415
i=1;
# 416
while(1){
# 417
c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);
# 418
if((c == - 1  || c == '\n') || c == '\r'){strvalue[_check_known_subscript_notnull(4096,i)]='\000';break;}
# 419
strvalue[_check_known_subscript_notnull(4096,i)]=(char)c;
# 420
++ i;
# 421
if(i >= 4096){
# 422
{const char*_tmp42E;void*_tmp42D[2];struct Cyc_String_pa_PrintArg_struct _tmp42C;struct Cyc_String_pa_PrintArg_struct _tmp42B;(_tmp42B.tag=0,((_tmp42B.f1=(struct _dyneither_ptr)
# 424
_tag_dyneither(strname,sizeof(char),256),((_tmp42C.tag=0,((_tmp42C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp42D[0]=& _tmp42C,((_tmp42D[1]=& _tmp42B,Cyc_fprintf(Cyc_stderr,((_tmp42E="Error reading spec file %s: value of %s too long\n",_tag_dyneither(_tmp42E,sizeof(char),50))),_tag_dyneither(_tmp42D,sizeof(void*),2)))))))))))));}
# 425
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
# 426
goto CLEANUP_AND_RETURN;}}
# 430
{struct _tuple22*_tmp437;struct _dyneither_ptr*_tmp436;struct _dyneither_ptr*_tmp435;struct Cyc_List_List*_tmp434;_tmp4B=((_tmp434=_cycalloc(sizeof(*_tmp434)),((_tmp434->hd=((_tmp437=_cycalloc(sizeof(*_tmp437)),((_tmp437->f1=((_tmp436=_cycalloc(sizeof(*_tmp436)),((_tmp436[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256)),_tmp436)))),((_tmp437->f2=(
# 431
(_tmp435=_cycalloc(sizeof(*_tmp435)),((_tmp435[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strvalue,sizeof(char),4096)),_tmp435)))),_tmp437)))))),((_tmp434->tl=_tmp4B,_tmp434))))));}
# 432
if(c == - 1)goto CLEANUP_AND_RETURN;}
# 435
CLEANUP_AND_RETURN:
# 436
 Cyc_fclose((struct Cyc___cycFILE*)spec_file);
# 437
return _tmp4B;}struct _dyneither_ptr Cyc_split_specs(struct _dyneither_ptr cmdline);static void _tmp441(struct _dyneither_ptr*_tmp6C,unsigned int*_tmp440,unsigned int*_tmp43F,struct _dyneither_ptr**_tmp43D){for(*_tmp440=0;*_tmp440 < *_tmp43F;(*_tmp440)++){(*_tmp43D)[*_tmp440]=*((struct _dyneither_ptr**)(*_tmp6C).curr)[(int)*_tmp440];}}
# 441
struct _dyneither_ptr Cyc_split_specs(struct _dyneither_ptr cmdline){
# 442
if((char*)cmdline.curr == (char*)(_tag_dyneither(0,0,0)).curr)return _tag_dyneither(0,0,0);{
# 443
unsigned long _tmp66=Cyc_strlen((struct _dyneither_ptr)cmdline);
# 444
struct Cyc_List_List*_tmp67=0;
# 445
char buf[4096];
# 446
int i=0;
# 447
int j=0;
# 448
if(_tmp66 > 4096)goto DONE;
# 449
while(1){
# 451
while(1){
# 452
if(i >= _tmp66)goto DONE;
# 453
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)goto DONE;
# 454
if(!isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;
# 455
++ i;}
# 457
j=0;
# 462
while(1){
# 463
if(i >= _tmp66)break;
# 464
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;
# 465
if(isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;
# 466
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== '\\'){
# 468
++ i;
# 469
if(i >= _tmp66)break;
# 470
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;
# 471
buf[_check_known_subscript_notnull(4096,j)]=*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i));
# 472
++ j;}else{
# 475
buf[_check_known_subscript_notnull(4096,j)]=*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i));
# 476
++ j;}
# 478
++ i;}
# 480
if(j < 4096)
# 481
buf[_check_known_subscript_notnull(4096,j)]='\000';{
# 483
struct _dyneither_ptr*_tmp43A;struct Cyc_List_List*_tmp439;_tmp67=((_tmp439=_cycalloc(sizeof(*_tmp439)),((_tmp439->hd=((_tmp43A=_cycalloc(sizeof(*_tmp43A)),((_tmp43A[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(buf,sizeof(char),4096)),_tmp43A)))),((_tmp439->tl=_tmp67,_tmp439))))));};}
# 485
DONE:
# 486
 _tmp67=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp67);
# 487
{struct Cyc_List_List*_tmp43B;_tmp67=((_tmp43B=_cycalloc(sizeof(*_tmp43B)),((_tmp43B->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((_tmp43B->tl=_tmp67,_tmp43B))))));}{
# 488
struct _dyneither_ptr _tmp6C=((struct _dyneither_ptr(*)(struct Cyc_List_List*x))Cyc_List_to_array)(_tmp67);
# 489
unsigned int _tmp440;unsigned int _tmp43F;struct _dyneither_ptr _tmp43E;struct _dyneither_ptr*_tmp43D;unsigned int _tmp43C;struct _dyneither_ptr _tmp6D=(_tmp43C=_get_dyneither_size(_tmp6C,sizeof(struct _dyneither_ptr*)),((_tmp43D=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp43C)),((_tmp43E=_tag_dyneither(_tmp43D,sizeof(struct _dyneither_ptr),_tmp43C),((((_tmp43F=_tmp43C,_tmp441(& _tmp6C,& _tmp440,& _tmp43F,& _tmp43D))),_tmp43E)))))));
# 490
return _tmp6D;};};}
# 495
int Cyc_compile_failure=0;
# 497
struct Cyc___cycFILE*Cyc_try_file_open(struct _dyneither_ptr filename,struct _dyneither_ptr mode,struct _dyneither_ptr msg_part){
# 498
struct _handler_cons _tmp72;_push_handler(& _tmp72);{int _tmp74=0;if(setjmp(_tmp72.handler))_tmp74=1;if(!_tmp74){{struct Cyc___cycFILE*_tmp75=(struct Cyc___cycFILE*)Cyc_file_open(filename,mode);_npop_handler(0);return _tmp75;};_pop_handler();}else{void*_tmp73=(void*)_exn_thrown;void*_tmp77=_tmp73;_LL1:;_LL2:
# 501
 Cyc_compile_failure=1;
# 502
{const char*_tmp446;void*_tmp445[2];struct Cyc_String_pa_PrintArg_struct _tmp444;struct Cyc_String_pa_PrintArg_struct _tmp443;(_tmp443.tag=0,((_tmp443.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp444.tag=0,((_tmp444.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp445[0]=& _tmp444,((_tmp445[1]=& _tmp443,Cyc_fprintf(Cyc_stderr,((_tmp446="\nError: couldn't open %s %s\n",_tag_dyneither(_tmp446,sizeof(char),29))),_tag_dyneither(_tmp445,sizeof(void*),2)))))))))))));}
# 503
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
# 504
return 0;_LL3:;_LL4:(void)_throw(_tmp77);_LL0:;}};}
# 508
void CYCALLOCPROFILE_mark(const char*s);
# 510
struct Cyc_List_List*Cyc_do_stage(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(void*,struct Cyc_List_List*),void*env,struct Cyc_List_List*tds){
# 514
const char*_tmp447;struct _dyneither_ptr exn_string=(_tmp447="",_tag_dyneither(_tmp447,sizeof(char),1));
# 515
const char*_tmp448;struct _dyneither_ptr explain_string=(_tmp448="",_tag_dyneither(_tmp448,sizeof(char),1));
# 516
int other_exn=0;
# 517
struct Cyc_Core_Impossible_exn_struct _tmp44E;const char*_tmp44D;struct Cyc_Core_Impossible_exn_struct*_tmp44C;void*ex=(void*)((_tmp44C=_cycalloc(sizeof(*_tmp44C)),((_tmp44C[0]=((_tmp44E.tag=Cyc_Core_Impossible,((_tmp44E.f1=((_tmp44D="",_tag_dyneither(_tmp44D,sizeof(char),1))),_tmp44E)))),_tmp44C))));
# 518
struct Cyc_List_List*_tmp7C=0;
# 520
CYCALLOCPROFILE_mark((const char*)_check_null(_untag_dyneither_ptr(stage_name,sizeof(char),1)));
# 522
{struct _handler_cons _tmp7D;_push_handler(& _tmp7D);{int _tmp7F=0;if(setjmp(_tmp7D.handler))_tmp7F=1;if(!_tmp7F){_tmp7C=f(env,tds);;_pop_handler();}else{void*_tmp7E=(void*)_exn_thrown;void*_tmp81=_tmp7E;struct _dyneither_ptr _tmp83;struct _dyneither_ptr _tmp86;struct _dyneither_ptr _tmp88;_LL6: {struct Cyc_Core_Impossible_exn_struct*_tmp82=(struct Cyc_Core_Impossible_exn_struct*)_tmp81;if(_tmp82->tag != Cyc_Core_Impossible)goto _LL8;else{_tmp83=_tmp82->f1;}}_LL7:
# 525
{const char*_tmp44F;exn_string=((_tmp44F="Exception Core::Impossible",_tag_dyneither(_tmp44F,sizeof(char),27)));}
# 526
explain_string=_tmp83;
# 527
goto _LL5;_LL8: {struct Cyc_Dict_Absent_exn_struct*_tmp84=(struct Cyc_Dict_Absent_exn_struct*)_tmp81;if(_tmp84->tag != Cyc_Dict_Absent)goto _LLA;}_LL9:
# 529
{const char*_tmp450;exn_string=((_tmp450="Exception Dict::Absent",_tag_dyneither(_tmp450,sizeof(char),23)));}
# 530
goto _LL5;_LLA: {struct Cyc_Core_Invalid_argument_exn_struct*_tmp85=(struct Cyc_Core_Invalid_argument_exn_struct*)_tmp81;if(_tmp85->tag != Cyc_Core_Invalid_argument)goto _LLC;else{_tmp86=_tmp85->f1;}}_LLB:
# 532
{const char*_tmp451;exn_string=((_tmp451="Exception Core::Invalid_argument",_tag_dyneither(_tmp451,sizeof(char),33)));}
# 533
explain_string=_tmp86;
# 534
goto _LL5;_LLC: {struct Cyc_Core_Failure_exn_struct*_tmp87=(struct Cyc_Core_Failure_exn_struct*)_tmp81;if(_tmp87->tag != Cyc_Core_Failure)goto _LLE;else{_tmp88=_tmp87->f1;}}_LLD:
# 536
{const char*_tmp452;exn_string=((_tmp452="Exception Core::Failure",_tag_dyneither(_tmp452,sizeof(char),24)));}
# 537
explain_string=_tmp88;
# 538
goto _LL5;_LLE:;_LLF:
# 540
 ex=_tmp81;
# 541
other_exn=1;
# 542
{const char*_tmp453;exn_string=((_tmp453="Uncaught exception",_tag_dyneither(_tmp453,sizeof(char),19)));}
# 543
goto _LL5;_LL10:;_LL11:(void)_throw(_tmp81);_LL5:;}};}
# 545
if(Cyc_Position_error_p())
# 546
Cyc_compile_failure=1;
# 547
{const char*_tmp454;if(Cyc_strcmp((struct _dyneither_ptr)exn_string,((_tmp454="",_tag_dyneither(_tmp454,sizeof(char),1))))!= 0){
# 548
Cyc_compile_failure=1;{
# 549
const char*_tmp45A;void*_tmp459[3];struct Cyc_String_pa_PrintArg_struct _tmp458;struct Cyc_String_pa_PrintArg_struct _tmp457;struct Cyc_String_pa_PrintArg_struct _tmp456;(_tmp456.tag=0,((_tmp456.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)explain_string),((_tmp457.tag=0,((_tmp457.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp458.tag=0,((_tmp458.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)exn_string),((_tmp459[0]=& _tmp458,((_tmp459[1]=& _tmp457,((_tmp459[2]=& _tmp456,Cyc_fprintf(Cyc_stderr,((_tmp45A="\n%s thrown during %s: %s",_tag_dyneither(_tmp45A,sizeof(char),25))),_tag_dyneither(_tmp459,sizeof(void*),3)))))))))))))))))));};}}
# 552
if(Cyc_compile_failure){
# 553
{const char*_tmp45D;void*_tmp45C;(_tmp45C=0,Cyc_fprintf(Cyc_stderr,((_tmp45D="\nCOMPILATION FAILED!\n",_tag_dyneither(_tmp45D,sizeof(char),22))),_tag_dyneither(_tmp45C,sizeof(void*),0)));}
# 554
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
# 555
if(other_exn)
# 556
(int)_throw(ex);
# 557
return _tmp7C;}else{
# 559
if(Cyc_v_r){
# 560
{const char*_tmp461;void*_tmp460[1];struct Cyc_String_pa_PrintArg_struct _tmp45F;(_tmp45F.tag=0,((_tmp45F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp460[0]=& _tmp45F,Cyc_fprintf(Cyc_stderr,((_tmp461="%s completed.\n",_tag_dyneither(_tmp461,sizeof(char),15))),_tag_dyneither(_tmp460,sizeof(void*),1)))))));}
# 561
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
# 562
return _tmp7C;}}
# 564
return _tmp7C;}
# 567
struct Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore){
# 568
Cyc_Lex_lex_init(1);
# 569
Cyc_Lex_pos_init();{
# 570
struct Cyc_List_List*_tmp9E=Cyc_Parse_parse_file(f);
# 571
Cyc_Lex_lex_init(1);
# 572
return _tmp9E;};}struct _tuple23{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;};
# 575
struct Cyc_List_List*Cyc_do_typecheck(struct _tuple23*tcenv,struct Cyc_List_List*tds){
# 577
struct _RegionHandle*_tmpA0;struct Cyc_Tcenv_Tenv*_tmpA1;struct _tuple23 _tmp9F=*tcenv;_tmpA0=_tmp9F.f1;_tmpA1=_tmp9F.f2;
# 578
Cyc_Tc_tc(_tmpA0,_tmpA1,1,tds);
# 579
if(!Cyc_noshake_r)
# 580
tds=Cyc_Tc_treeshake(_tmpA1,tds);
# 581
return tds;}
# 584
struct Cyc_List_List*Cyc_do_cfcheck(int ignore,struct Cyc_List_List*tds){
# 585
Cyc_NewControlFlow_cf_check(tds);
# 586
return tds;}struct _tuple24{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*f3;};
# 591
struct Cyc_List_List*Cyc_do_interface(struct _tuple24*params,struct Cyc_List_List*tds){
# 592
struct _tuple24 _tmpA3;struct Cyc_Tcenv_Tenv*_tmpA4;struct Cyc___cycFILE*_tmpA5;struct Cyc___cycFILE*_tmpA6;struct _tuple24*_tmpA2=params;_tmpA3=*_tmpA2;_tmpA4=_tmpA3.f1;_tmpA5=_tmpA3.f2;_tmpA6=_tmpA3.f3;{
# 593
struct Cyc_Interface_I*_tmpA7=Cyc_Interface_extract(_tmpA4->ae);
# 594
if(_tmpA5 == 0)
# 595
Cyc_Interface_save(_tmpA7,_tmpA6);else{
# 597
struct Cyc_Interface_I*_tmpA8=Cyc_Interface_parse((struct Cyc___cycFILE*)_tmpA5);
# 598
const char*_tmp466;const char*_tmp465;struct _tuple21*_tmp464;if(!Cyc_Interface_is_subinterface(_tmpA8,_tmpA7,((_tmp464=_cycalloc(sizeof(*_tmp464)),((_tmp464->f1=((_tmp465="written interface",_tag_dyneither(_tmp465,sizeof(char),18))),((_tmp464->f2=((_tmp466="maximal interface",_tag_dyneither(_tmp466,sizeof(char),18))),_tmp464))))))))
# 599
Cyc_compile_failure=1;else{
# 601
Cyc_Interface_save(_tmpA8,_tmpA6);}}
# 603
return tds;};}
# 606
struct Cyc_List_List*Cyc_do_translate(int ignore,struct Cyc_List_List*tds){
# 607
return Cyc_Toc_toc(tds);}
# 609
struct Cyc_List_List*Cyc_do_tovc(int ignore,struct Cyc_List_List*tds){
# 610
Cyc_Tovc_elim_array_initializers=Cyc_Cyclone_tovc_r;
# 611
return Cyc_Tovc_tovc(tds);}
# 614
static struct _dyneither_ptr Cyc_cyc_setjmp={(void*)0,(void*)0,(void*)(0 + 0)};
# 616
static struct _dyneither_ptr Cyc_cyc_include={(void*)0,(void*)0,(void*)(0 + 0)};
# 618
static void Cyc_set_cyc_include(struct _dyneither_ptr f){
# 619
Cyc_cyc_include=f;}
# 623
int Cyc_copy_internal_file(struct _dyneither_ptr file,struct Cyc___cycFILE*out_file){
# 626
if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr){
# 627
{const char*_tmp469;void*_tmp468;(_tmp468=0,Cyc_fprintf(Cyc_stderr,((_tmp469="Internal error: copy_internal_file called with NULL\n",_tag_dyneither(_tmp469,sizeof(char),53))),_tag_dyneither(_tmp468,sizeof(void*),0)));}
# 628
return 1;}{
# 630
const char*_tmp46B;const char*_tmp46A;struct Cyc___cycFILE*_tmpAE=Cyc_try_file_open(file,((_tmp46A="r",_tag_dyneither(_tmp46A,sizeof(char),2))),((_tmp46B="internal compiler file",_tag_dyneither(_tmp46B,sizeof(char),23))));
# 631
if(_tmpAE == 0)return 1;{
# 632
unsigned long n_read=1024;
# 633
unsigned long n_written;
# 634
char buf[1024];
# 635
{unsigned int _tmpB5=1024;unsigned int i;for(i=0;i < _tmpB5;i ++){buf[i]='\000';}}while(n_read == 1024){
# 636
n_read=Cyc_fread(_tag_dyneither(buf,sizeof(char),1024),1,1024,(struct Cyc___cycFILE*)_tmpAE);
# 637
if(n_read != 1024  && !Cyc_feof((struct Cyc___cycFILE*)_tmpAE)){
# 638
Cyc_compile_failure=1;
# 639
{const char*_tmp46F;void*_tmp46E[1];struct Cyc_String_pa_PrintArg_struct _tmp46D;(_tmp46D.tag=0,((_tmp46D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp46E[0]=& _tmp46D,Cyc_fprintf(Cyc_stderr,((_tmp46F="\nError: problem copying %s\n",_tag_dyneither(_tmp46F,sizeof(char),28))),_tag_dyneither(_tmp46E,sizeof(void*),1)))))));}
# 640
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
# 641
return 1;}
# 643
n_written=Cyc_fwrite(_tag_dyneither(buf,sizeof(char),1024),1,n_read,out_file);
# 644
if(n_read != n_written){
# 645
Cyc_compile_failure=1;
# 646
{const char*_tmp473;void*_tmp472[1];struct Cyc_String_pa_PrintArg_struct _tmp471;(_tmp471.tag=0,((_tmp471.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp472[0]=& _tmp471,Cyc_fprintf(Cyc_stderr,((_tmp473="\nError: problem copying %s\n",_tag_dyneither(_tmp473,sizeof(char),28))),_tag_dyneither(_tmp472,sizeof(void*),1)))))));}
# 647
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
# 648
return 1;}}
# 651
Cyc_fclose((struct Cyc___cycFILE*)_tmpAE);
# 652
return 0;};};}
# 656
struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*out_file,struct Cyc_List_List*tds){
# 657
struct Cyc_Absynpp_Params _tmpB8=Cyc_tc_r?Cyc_Absynpp_cyc_params_r: Cyc_Absynpp_c_params_r;
# 658
_tmpB8.expand_typedefs=!Cyc_noexpand_r;
# 659
_tmpB8.to_VC=Cyc_Cyclone_tovc_r;
# 660
_tmpB8.add_cyc_prefix=Cyc_add_cyc_namespace_r;
# 661
_tmpB8.generate_line_directives=Cyc_generate_line_directives_r;
# 662
_tmpB8.print_full_evars=Cyc_print_full_evars_r;
# 663
_tmpB8.print_all_tvars=Cyc_print_all_tvars_r;
# 664
_tmpB8.print_all_kinds=Cyc_print_all_kinds_r;
# 665
_tmpB8.print_all_effects=Cyc_print_all_effects_r;
# 667
if(Cyc_inline_functions_r){
# 668
const char*_tmp476;void*_tmp475;(_tmp475=0,Cyc_fprintf(out_file,((_tmp476="#define _INLINE_FUNCTIONS\n",_tag_dyneither(_tmp476,sizeof(char),27))),_tag_dyneither(_tmp475,sizeof(void*),0)));}
# 671
if((!Cyc_parseonly_r  && !Cyc_tc_r) && !Cyc_cf_r){
# 672
if(!Cyc_nocyc_setjmp_r){
# 673
if(Cyc_Lex_compile_for_boot_r){
# 674
const char*_tmp479;void*_tmp478;(_tmp478=0,Cyc_fprintf(out_file,((_tmp479="#include <setjmp.h>\n",_tag_dyneither(_tmp479,sizeof(char),21))),_tag_dyneither(_tmp478,sizeof(void*),0)));}else{
# 675
if(Cyc_copy_internal_file(Cyc_cyc_setjmp,out_file))return tds;}}
# 677
if(Cyc_copy_internal_file(Cyc_cyc_include,out_file))return tds;}
# 679
if(Cyc_pp_r){
# 680
Cyc_Absynpp_set_params(& _tmpB8);
# 681
Cyc_Absynpp_decllist2file(tds,out_file);}else{
# 683
Cyc_Absyndump_set_params(& _tmpB8);
# 684
Cyc_Absyndump_dumpdecllist2file(tds,out_file);}
# 686
Cyc_fflush((struct Cyc___cycFILE*)out_file);
# 687
return tds;}
# 692
static struct Cyc_List_List*Cyc_cfiles=0;
# 693
static void Cyc_remove_cfiles(){
# 694
if(!Cyc_save_c_r)
# 695
for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){
# 696
Cyc_remove_file(*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}
# 700
static struct Cyc_List_List*Cyc_split_by_char(struct _dyneither_ptr s,char c){
# 701
if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr)return 0;{
# 702
struct Cyc_List_List*_tmpBD=0;
# 703
unsigned long _tmpBE=Cyc_strlen((struct _dyneither_ptr)s);
# 705
while(_tmpBE > 0){
# 706
struct _dyneither_ptr _tmpBF=Cyc_strchr(s,c);
# 707
if((char*)_tmpBF.curr == (char*)(_tag_dyneither(0,0,0)).curr){
# 708
{struct _dyneither_ptr*_tmp47C;struct Cyc_List_List*_tmp47B;_tmpBD=((_tmp47B=_cycalloc(sizeof(*_tmp47B)),((_tmp47B->hd=((_tmp47C=_cycalloc(sizeof(*_tmp47C)),((_tmp47C[0]=s,_tmp47C)))),((_tmp47B->tl=_tmpBD,_tmp47B))))));}
# 709
break;}else{
# 712
{struct _dyneither_ptr*_tmp47F;struct Cyc_List_List*_tmp47E;_tmpBD=((_tmp47E=_cycalloc(sizeof(*_tmp47E)),((_tmp47E->hd=((_tmp47F=_cycalloc(sizeof(*_tmp47F)),((_tmp47F[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,0,(unsigned long)((((struct _dyneither_ptr)_tmpBF).curr - s.curr)/ sizeof(char))),_tmp47F)))),((_tmp47E->tl=_tmpBD,_tmp47E))))));}
# 713
_tmpBE -=(((struct _dyneither_ptr)_tmpBF).curr - s.curr)/ sizeof(char);
# 714
s=_dyneither_ptr_plus(_tmpBF,sizeof(char),1);}}
# 717
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpBD);};}
# 721
static int Cyc_file_exists(struct _dyneither_ptr file){
# 722
struct Cyc___cycFILE*f=0;
# 723
{struct _handler_cons _tmpC4;_push_handler(& _tmpC4);{int _tmpC6=0;if(setjmp(_tmpC4.handler))_tmpC6=1;if(!_tmpC6){{const char*_tmp480;f=(struct Cyc___cycFILE*)Cyc_file_open(file,((_tmp480="r",_tag_dyneither(_tmp480,sizeof(char),2))));};_pop_handler();}else{void*_tmpC5=(void*)_exn_thrown;void*_tmpC9=_tmpC5;_LL13:;_LL14: goto _LL12;_LL15:;_LL16:(void)_throw(_tmpC9);_LL12:;}};}
# 724
if(f == 0)return 0;else{
# 725
Cyc_fclose((struct Cyc___cycFILE*)f);return 1;}}
# 729
static struct _dyneither_ptr Cyc_sprint_list(struct Cyc_List_List*dirs){
# 730
const char*_tmp481;struct _dyneither_ptr tmp=(_tmp481="",_tag_dyneither(_tmp481,sizeof(char),1));
# 731
for(0;dirs != 0;dirs=dirs->tl){
# 732
struct _dyneither_ptr _tmpCA=*((struct _dyneither_ptr*)dirs->hd);
# 733
if((char*)_tmpCA.curr == (char*)(_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpCA)== 0)continue;
# 734
{const char*_tmp482;_tmpCA=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpCA,((_tmp482=":",_tag_dyneither(_tmp482,sizeof(char),2))));}
# 735
tmp=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpCA,(struct _dyneither_ptr)tmp);}
# 737
return tmp;}
# 742
static struct _dyneither_ptr*Cyc_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
# 743
if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr)return 0;
# 744
for(0;dirs != 0;dirs=dirs->tl){
# 745
struct _dyneither_ptr _tmpCD=*((struct _dyneither_ptr*)dirs->hd);
# 746
if((char*)_tmpCD.curr == (char*)(_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpCD)== 0)continue;{
# 747
struct _dyneither_ptr s=(struct _dyneither_ptr)Cyc_Filename_concat(_tmpCD,file);
# 748
if(Cyc_file_exists(s)){struct _dyneither_ptr*_tmp483;return(_tmp483=_cycalloc(sizeof(*_tmp483)),((_tmp483[0]=s,_tmp483)));}};}
# 750
return 0;}
# 753
static struct _dyneither_ptr Cyc_do_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
# 754
struct _dyneither_ptr*_tmpCF=Cyc_find(dirs,file);
# 755
if(_tmpCF == 0){
# 756
Cyc_compile_failure=1;
# 757
Cyc_remove_cfiles();
# 758
{const char*_tmp488;void*_tmp487[2];struct Cyc_String_pa_PrintArg_struct _tmp486;struct Cyc_String_pa_PrintArg_struct _tmp485;(_tmp485.tag=0,((_tmp485.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 759
Cyc_sprint_list(dirs)),((_tmp486.tag=0,((_tmp486.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp487[0]=& _tmp486,((_tmp487[1]=& _tmp485,Cyc_fprintf(Cyc_stderr,((_tmp488="Error: can't find internal compiler file %s in path %s\n",_tag_dyneither(_tmp488,sizeof(char),56))),_tag_dyneither(_tmp487,sizeof(void*),2)))))))))))));}
# 760
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
# 761
struct Cyc_Core_Failure_exn_struct _tmp498;const char*_tmp497;void*_tmp496[2];struct Cyc_String_pa_PrintArg_struct _tmp495;struct Cyc_String_pa_PrintArg_struct _tmp494;struct Cyc_Core_Failure_exn_struct*_tmp493;(int)_throw((void*)((_tmp493=_cycalloc(sizeof(*_tmp493)),((_tmp493[0]=((_tmp498.tag=Cyc_Core_Failure,((_tmp498.f1=(struct _dyneither_ptr)((_tmp494.tag=0,((_tmp494.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sprint_list(dirs)),((_tmp495.tag=0,((_tmp495.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp496[0]=& _tmp495,((_tmp496[1]=& _tmp494,Cyc_aprintf(((_tmp497="Error: can't find internal compiler file %s in path %s\n",_tag_dyneither(_tmp497,sizeof(char),56))),_tag_dyneither(_tmp496,sizeof(void*),2)))))))))))))),_tmp498)))),_tmp493)))));};}
# 765
return*_tmpCF;}
# 770
static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 772
struct Cyc_List_List*_tmpDA=0;
# 773
for(0;dirs != 0;dirs=dirs->tl){
# 774
struct _dyneither_ptr*_tmp49B;struct Cyc_List_List*_tmp49A;_tmpDA=((_tmp49A=_cycalloc(sizeof(*_tmp49A)),((_tmp49A->hd=((_tmp49B=_cycalloc(sizeof(*_tmp49B)),((_tmp49B[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir),_tmp49B)))),((_tmp49A->tl=_tmpDA,_tmp49A))))));}
# 776
_tmpDA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpDA);
# 777
return _tmpDA;}
# 782
static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 784
struct Cyc_List_List*_tmpDD=0;
# 785
for(0;dirs != 0;dirs=dirs->tl){
# 786
{struct Cyc_List_List*_tmp49C;_tmpDD=((_tmp49C=_cycalloc(sizeof(*_tmp49C)),((_tmp49C->hd=(struct _dyneither_ptr*)dirs->hd,((_tmp49C->tl=_tmpDD,_tmp49C))))));}{
# 787
struct _dyneither_ptr*_tmp49F;struct Cyc_List_List*_tmp49E;_tmpDD=((_tmp49E=_cycalloc(sizeof(*_tmp49E)),((_tmp49E->hd=((_tmp49F=_cycalloc(sizeof(*_tmp49F)),((_tmp49F[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir),_tmp49F)))),((_tmp49E->tl=_tmpDD,_tmp49E))))));};}
# 789
_tmpDD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpDD);
# 790
return _tmpDD;}
# 794
static int Cyc_is_other_special(char c){
# 795
switch(c){case '\\': _LL17:
# 796
 goto _LL18;case '"': _LL18:
# 797
 goto _LL19;case ';': _LL19:
# 798
 goto _LL1A;case '&': _LL1A:
# 799
 goto _LL1B;case '(': _LL1B:
# 800
 goto _LL1C;case ')': _LL1C:
# 801
 goto _LL1D;case '|': _LL1D:
# 802
 goto _LL1E;case '^': _LL1E:
# 803
 goto _LL1F;case '<': _LL1F:
# 804
 goto _LL20;case '>': _LL20:
# 805
 goto _LL21;case '\n': _LL21:
# 809
 goto _LL22;case '\t': _LL22:
# 810
 return 1;default: _LL23:
# 811
 return 0;}}static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s);static void _tmp4A8(unsigned int*_tmp4A7,unsigned int*_tmp4A6,char**_tmp4A4){for(*_tmp4A7=0;*_tmp4A7 < *_tmp4A6;(*_tmp4A7)++){(*_tmp4A4)[*_tmp4A7]='\000';}}
# 816
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
# 817
unsigned long _tmpE1=Cyc_strlen((struct _dyneither_ptr)s);
# 820
int _tmpE2=0;
# 821
int _tmpE3=0;
# 822
{int i=0;for(0;i < _tmpE1;++ i){
# 823
char _tmpE4=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
# 824
if(_tmpE4 == '\'')++ _tmpE2;else{
# 825
if(Cyc_is_other_special(_tmpE4))++ _tmpE3;}}}
# 829
if(_tmpE2 == 0  && _tmpE3 == 0)
# 830
return s;
# 833
if(_tmpE2 == 0){
# 834
struct _dyneither_ptr*_tmp4A2;struct _dyneither_ptr*_tmp4A1[3];return(struct _dyneither_ptr)Cyc_strconcat_l(((_tmp4A1[2]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((_tmp4A1[1]=((_tmp4A2=_cycalloc(sizeof(*_tmp4A2)),((_tmp4A2[0]=(struct _dyneither_ptr)s,_tmp4A2)))),((_tmp4A1[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A1,sizeof(struct _dyneither_ptr*),3)))))))));}{
# 837
unsigned long _tmpE9=(_tmpE1 + _tmpE2)+ _tmpE3;
# 838
unsigned int _tmp4A7;unsigned int _tmp4A6;struct _dyneither_ptr _tmp4A5;char*_tmp4A4;unsigned int _tmp4A3;struct _dyneither_ptr s2=(_tmp4A3=_tmpE9 + 1,((_tmp4A4=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp4A3 + 1)),((_tmp4A5=_tag_dyneither(_tmp4A4,sizeof(char),_tmp4A3 + 1),((((_tmp4A6=_tmp4A3,((_tmp4A8(& _tmp4A7,& _tmp4A6,& _tmp4A4),_tmp4A4[_tmp4A6]=(char)0)))),_tmp4A5)))))));
# 839
int _tmpEA=0;
# 840
int _tmpEB=0;
# 841
for(0;_tmpEA < _tmpE1;++ _tmpEA){
# 842
char _tmpEC=*((const char*)_check_dyneither_subscript(s,sizeof(char),_tmpEA));
# 843
if(_tmpEC == '\''  || Cyc_is_other_special(_tmpEC)){
# 844
char _tmp4AB;char _tmp4AA;struct _dyneither_ptr _tmp4A9;(_tmp4A9=_dyneither_ptr_plus(s2,sizeof(char),_tmpEB ++),((_tmp4AA=*((char*)_check_dyneither_subscript(_tmp4A9,sizeof(char),0)),((_tmp4AB='\\',((_get_dyneither_size(_tmp4A9,sizeof(char))== 1  && (_tmp4AA == '\000'  && _tmp4AB != '\000')?_throw_arraybounds(): 1,*((char*)_tmp4A9.curr)=_tmp4AB)))))));}{
# 845
char _tmp4AE;char _tmp4AD;struct _dyneither_ptr _tmp4AC;(_tmp4AC=_dyneither_ptr_plus(s2,sizeof(char),_tmpEB ++),((_tmp4AD=*((char*)_check_dyneither_subscript(_tmp4AC,sizeof(char),0)),((_tmp4AE=_tmpEC,((_get_dyneither_size(_tmp4AC,sizeof(char))== 1  && (_tmp4AD == '\000'  && _tmp4AE != '\000')?_throw_arraybounds(): 1,*((char*)_tmp4AC.curr)=_tmp4AE)))))));};}
# 847
return(struct _dyneither_ptr)s2;};}
# 849
static struct _dyneither_ptr*Cyc_sh_escape_stringptr(struct _dyneither_ptr*sp){
# 850
struct _dyneither_ptr*_tmp4AF;return(_tmp4AF=_cycalloc(sizeof(*_tmp4AF)),((_tmp4AF[0]=Cyc_sh_escape_string(*sp),_tmp4AF)));}
# 853
static void Cyc_process_file(struct _dyneither_ptr filename){
# 855
struct _dyneither_ptr _tmpF8=Cyc_make_base_filename(filename,Cyc_output_file);
# 856
const char*_tmp4B0;struct _dyneither_ptr _tmpF9=Cyc_strconcat((struct _dyneither_ptr)_tmpF8,((_tmp4B0=".cyp",_tag_dyneither(_tmp4B0,sizeof(char),5))));
# 857
const char*_tmp4B1;struct _dyneither_ptr _tmpFA=Cyc_strconcat((struct _dyneither_ptr)_tmpF8,((_tmp4B1=".cyci",_tag_dyneither(_tmp4B1,sizeof(char),6))));
# 858
const char*_tmp4B2;struct _dyneither_ptr _tmpFB=Cyc_strconcat((struct _dyneither_ptr)_tmpF8,((_tmp4B2=".cycio",_tag_dyneither(_tmp4B2,sizeof(char),7))));
# 859
const char*_tmp4B3;struct _dyneither_ptr _tmpFC=Cyc_strconcat((struct _dyneither_ptr)_tmpF8,((_tmp4B3=".c",_tag_dyneither(_tmp4B3,sizeof(char),3))));
# 861
if(Cyc_v_r){const char*_tmp4B7;void*_tmp4B6[1];struct Cyc_String_pa_PrintArg_struct _tmp4B5;(_tmp4B5.tag=0,((_tmp4B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp4B6[0]=& _tmp4B5,Cyc_fprintf(Cyc_stderr,((_tmp4B7="Compiling %s\n",_tag_dyneither(_tmp4B7,sizeof(char),14))),_tag_dyneither(_tmp4B6,sizeof(void*),1)))))));}{
# 864
const char*_tmp4B9;const char*_tmp4B8;struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,((_tmp4B8="r",_tag_dyneither(_tmp4B8,sizeof(char),2))),((_tmp4B9="input file",_tag_dyneither(_tmp4B9,sizeof(char),11))));
# 865
if(Cyc_compile_failure  || !((unsigned int)f0))
# 866
return;
# 867
Cyc_fclose((struct Cyc___cycFILE*)f0);{
# 871
const char*_tmp4C0;struct _dyneither_ptr*_tmp4BF;const char*_tmp4BE;struct Cyc_List_List*_tmp4BD;struct _dyneither_ptr _tmp100=
# 872
Cyc_str_sepstr(((_tmp4BD=_cycalloc(sizeof(*_tmp4BD)),((_tmp4BD->hd=((_tmp4BF=_cycalloc(sizeof(*_tmp4BF)),((_tmp4BF[0]=(struct _dyneither_ptr)((_tmp4BE="",_tag_dyneither(_tmp4BE,sizeof(char),1))),_tmp4BF)))),((_tmp4BD->tl=
# 873
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs)),_tmp4BD)))))),(
# 874
(_tmp4C0=" ",_tag_dyneither(_tmp4C0,sizeof(char),2))));
# 881
struct _dyneither_ptr def_inc_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path);
# 882
struct Cyc_List_List*_tmp101=Cyc_add_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);
# 883
{const char*_tmp4C1;_tmp101=Cyc_add_subdir(_tmp101,((_tmp4C1="include",_tag_dyneither(_tmp4C1,sizeof(char),8))));}
# 884
if(Cyc_strlen((struct _dyneither_ptr)def_inc_path)> 0){
# 885
struct _dyneither_ptr*_tmp4C4;struct Cyc_List_List*_tmp4C3;_tmp101=((_tmp4C3=_cycalloc(sizeof(*_tmp4C3)),((_tmp4C3->hd=((_tmp4C4=_cycalloc(sizeof(*_tmp4C4)),((_tmp4C4[0]=def_inc_path,_tmp4C4)))),((_tmp4C3->tl=_tmp101,_tmp4C3))))));}{
# 887
char*_tmp105=getenv((const char*)"CYCLONE_INCLUDE_PATH");
# 888
if((char*)_tmp105 != 0){
# 889
char*_tmp4C5;_tmp101=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(((_tmp4C5=_tmp105,_tag_dyneither(_tmp4C5,sizeof(char),_get_zero_arr_size_char((void*)_tmp4C5,1)))),':'),_tmp101);}{
# 892
const char*_tmp4CC;struct _dyneither_ptr*_tmp4CB;const char*_tmp4CA;struct Cyc_List_List*_tmp4C9;struct _dyneither_ptr stdinc_string=(struct _dyneither_ptr)
# 893
Cyc_str_sepstr(((_tmp4C9=_cycalloc(sizeof(*_tmp4C9)),((_tmp4C9->hd=((_tmp4CB=_cycalloc(sizeof(*_tmp4CB)),((_tmp4CB[0]=(struct _dyneither_ptr)((_tmp4CA="",_tag_dyneither(_tmp4CA,sizeof(char),1))),_tmp4CB)))),((_tmp4C9->tl=
# 894
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,_tmp101),_tmp4C9)))))),(
# 895
(_tmp4CC=" -I",_tag_dyneither(_tmp4CC,sizeof(char),4))));
# 896
struct _dyneither_ptr ofile_string;
# 897
if(Cyc_stop_after_cpp_r){
# 898
if(Cyc_output_file != 0){
# 899
const char*_tmp4D0;void*_tmp4CF[1];struct Cyc_String_pa_PrintArg_struct _tmp4CE;ofile_string=(struct _dyneither_ptr)((_tmp4CE.tag=0,((_tmp4CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_output_file))),((_tmp4CF[0]=& _tmp4CE,Cyc_aprintf(((_tmp4D0=" > %s",_tag_dyneither(_tmp4D0,sizeof(char),6))),_tag_dyneither(_tmp4CF,sizeof(void*),1))))))));}else{
# 901
const char*_tmp4D1;ofile_string=((_tmp4D1="",_tag_dyneither(_tmp4D1,sizeof(char),1)));}}else{
# 903
const char*_tmp4D5;void*_tmp4D4[1];struct Cyc_String_pa_PrintArg_struct _tmp4D3;ofile_string=(struct _dyneither_ptr)((_tmp4D3.tag=0,((_tmp4D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string((struct _dyneither_ptr)_tmpF9)),((_tmp4D4[0]=& _tmp4D3,Cyc_aprintf(((_tmp4D5=" > %s",_tag_dyneither(_tmp4D5,sizeof(char),6))),_tag_dyneither(_tmp4D4,sizeof(void*),1))))))));}{
# 905
struct _dyneither_ptr fixup_string;
# 906
if(Cyc_produce_dependencies){
# 909
if(Cyc_dependencies_target == 0){
# 913
const char*_tmp4D6;fixup_string=((_tmp4D6=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",_tag_dyneither(_tmp4D6,sizeof(char),35)));}else{
# 918
const char*_tmp4DA;void*_tmp4D9[1];struct Cyc_String_pa_PrintArg_struct _tmp4D8;fixup_string=(struct _dyneither_ptr)(
# 919
(_tmp4D8.tag=0,((_tmp4D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_dependencies_target))),((_tmp4D9[0]=& _tmp4D8,Cyc_aprintf(((_tmp4DA=" | sed 's/^.*\\.cyc\\.o:/%s:/'",_tag_dyneither(_tmp4DA,sizeof(char),29))),_tag_dyneither(_tmp4D9,sizeof(void*),1))))))));}}else{
# 922
const char*_tmp4DB;fixup_string=((_tmp4DB="",_tag_dyneither(_tmp4DB,sizeof(char),1)));}{
# 924
const char*_tmp4E4;void*_tmp4E3[6];struct Cyc_String_pa_PrintArg_struct _tmp4E2;struct Cyc_String_pa_PrintArg_struct _tmp4E1;struct Cyc_String_pa_PrintArg_struct _tmp4E0;struct Cyc_String_pa_PrintArg_struct _tmp4DF;struct Cyc_String_pa_PrintArg_struct _tmp4DE;struct Cyc_String_pa_PrintArg_struct _tmp4DD;struct _dyneither_ptr _tmp113=(_tmp4DD.tag=0,((_tmp4DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ofile_string),((_tmp4DE.tag=0,((_tmp4DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)fixup_string),((_tmp4DF.tag=0,((_tmp4DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 927
Cyc_sh_escape_string(filename)),((_tmp4E0.tag=0,((_tmp4E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdinc_string),((_tmp4E1.tag=0,((_tmp4E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp100),((_tmp4E2.tag=0,((_tmp4E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cpp),((_tmp4E3[0]=& _tmp4E2,((_tmp4E3[1]=& _tmp4E1,((_tmp4E3[2]=& _tmp4E0,((_tmp4E3[3]=& _tmp4DF,((_tmp4E3[4]=& _tmp4DE,((_tmp4E3[5]=& _tmp4DD,Cyc_aprintf(((_tmp4E4="%s %s%s %s%s%s",_tag_dyneither(_tmp4E4,sizeof(char),15))),_tag_dyneither(_tmp4E3,sizeof(void*),6)))))))))))))))))))))))))))))))))))));
# 930
if(Cyc_v_r){const char*_tmp4E8;void*_tmp4E7[1];struct Cyc_String_pa_PrintArg_struct _tmp4E6;(_tmp4E6.tag=0,((_tmp4E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp113),((_tmp4E7[0]=& _tmp4E6,Cyc_fprintf(Cyc_stderr,((_tmp4E8="%s\n",_tag_dyneither(_tmp4E8,sizeof(char),4))),_tag_dyneither(_tmp4E7,sizeof(void*),1)))))));}
# 931
if(system((const char*)_untag_dyneither_ptr(_tmp113,sizeof(char),1))!= 0){
# 932
Cyc_compile_failure=1;
# 933
{const char*_tmp4EB;void*_tmp4EA;(_tmp4EA=0,Cyc_fprintf(Cyc_stderr,((_tmp4EB="\nError: preprocessing\n",_tag_dyneither(_tmp4EB,sizeof(char),23))),_tag_dyneither(_tmp4EA,sizeof(void*),0)));}
# 934
return;}
# 936
if(Cyc_stop_after_cpp_r)return;
# 939
Cyc_Position_reset_position((struct _dyneither_ptr)_tmpF9);{
# 940
const char*_tmp4ED;const char*_tmp4EC;struct Cyc___cycFILE*in_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpF9,((_tmp4EC="r",_tag_dyneither(_tmp4EC,sizeof(char),2))),((_tmp4ED="file",_tag_dyneither(_tmp4ED,sizeof(char),5))));
# 941
if(Cyc_compile_failure)return;{
# 943
struct Cyc_List_List*tds=0;
# 946
{struct _handler_cons _tmp119;_push_handler(& _tmp119);{int _tmp11B=0;if(setjmp(_tmp119.handler))_tmp11B=1;if(!_tmp11B){
# 947
{const char*_tmp4EE;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4EE="parsing",_tag_dyneither(_tmp4EE,sizeof(char),8))),Cyc_do_parse,(struct Cyc___cycFILE*)_check_null(in_file),tds);};_pop_handler();}else{void*_tmp11A=(void*)_exn_thrown;void*_tmp11E=_tmp11A;_LL26:;_LL27:
# 950
 Cyc_file_close((struct Cyc___cycFILE*)_check_null(in_file));
# 951
Cyc_remove_file((struct _dyneither_ptr)_tmpF9);
# 952
(int)_throw(_tmp11E);_LL28:;_LL29:(void)_throw(_tmp11E);_LL25:;}};}
# 954
Cyc_file_close((struct Cyc___cycFILE*)in_file);
# 955
if(Cyc_compile_failure){
# 956
Cyc_remove_file((struct _dyneither_ptr)_tmpF9);
# 957
return;}
# 960
if(Cyc_Absyn_porting_c_code){
# 961
Cyc_Port_port(tds);
# 962
return;}{
# 965
struct _RegionHandle _tmp11F=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp11F;_push_region(tc_rgn);{
# 966
struct Cyc_Tcenv_Tenv*_tmp120=Cyc_Tcenv_tc_init(tc_rgn);
# 967
if(Cyc_parseonly_r)goto PRINTC;{
# 970
struct _tuple23 _tmp4EF;struct _tuple23 _tmp121=(_tmp4EF.f1=tc_rgn,((_tmp4EF.f2=_tmp120,_tmp4EF)));
# 971
{const char*_tmp4F0;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_List_List*),struct _tuple23*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4F0="type checking",_tag_dyneither(_tmp4F0,sizeof(char),14))),Cyc_do_typecheck,& _tmp121,tds);}
# 973
if(Cyc_compile_failure){
# 974
Cyc_remove_file((struct _dyneither_ptr)_tmpF9);
# 975
_npop_handler(0);return;}
# 977
if(Cyc_tc_r)goto PRINTC;
# 978
{const char*_tmp4F1;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4F1="control-flow checking",_tag_dyneither(_tmp4F1,sizeof(char),22))),Cyc_do_cfcheck,1,tds);}
# 979
if(Cyc_compile_failure){
# 980
Cyc_remove_file((struct _dyneither_ptr)_tmpF9);
# 981
_npop_handler(0);return;}
# 985
if(Cyc_ic_r){
# 986
struct Cyc___cycFILE*inter_file=Cyc_fopen((const char*)((char*)_untag_dyneither_ptr(_tmpFA,sizeof(char),1)),(const char*)"r");
# 987
const char*_tmp4F3;const char*_tmp4F2;struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open((struct _dyneither_ptr)_tmpFB,((_tmp4F2="w",_tag_dyneither(_tmp4F2,sizeof(char),2))),((_tmp4F3="interface object file",_tag_dyneither(_tmp4F3,sizeof(char),22))));
# 988
if(inter_objfile == 0){
# 989
Cyc_compile_failure=1;
# 990
_npop_handler(0);return;}
# 992
Cyc_Position_reset_position((struct _dyneither_ptr)_tmpFA);{
# 993
struct _tuple24 _tmp4F4;struct _tuple24 _tmp124=(_tmp4F4.f1=_tmp120,((_tmp4F4.f2=inter_file,((_tmp4F4.f3=(struct Cyc___cycFILE*)inter_objfile,_tmp4F4)))));
# 994
{const char*_tmp4F5;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_List_List*),struct _tuple24*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4F5="interface checking",_tag_dyneither(_tmp4F5,sizeof(char),19))),Cyc_do_interface,& _tmp124,tds);}
# 995
if(inter_file != 0)
# 996
Cyc_file_close((struct Cyc___cycFILE*)inter_file);
# 997
Cyc_file_close((struct Cyc___cycFILE*)inter_objfile);};}};}
# 1000
if(Cyc_cf_r)goto PRINTC;
# 1003
{const char*_tmp4F6;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4F6="translation to C",_tag_dyneither(_tmp4F6,sizeof(char),17))),Cyc_do_translate,1,tds);}
# 1004
if(Cyc_compile_failure){
# 1005
Cyc_remove_file((struct _dyneither_ptr)_tmpF9);
# 1006
_npop_handler(0);return;}
# 1009
if(!Cyc_compile_failure)
# 1010
Cyc_Tcutil_flush_warnings();
# 1011
Cyc_remove_file((struct _dyneither_ptr)_tmpF9);
# 1012
if(Cyc_compile_failure){_npop_handler(0);return;}
# 1013
if(Cyc_Cyclone_tovc_r  || Cyc_elim_se_r){
# 1017
const char*_tmp4F7;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4F7="post-pass to VC",_tag_dyneither(_tmp4F7,sizeof(char),16))),Cyc_do_tovc,1,tds);}
# 1018
if(Cyc_compile_failure){_npop_handler(0);return;}
# 1024
PRINTC: {
# 1025
struct Cyc___cycFILE*out_file;
# 1026
if((Cyc_parseonly_r  || Cyc_tc_r) || Cyc_cf_r){
# 1027
if(Cyc_output_file != 0){
# 1028
const char*_tmp4F9;const char*_tmp4F8;out_file=Cyc_try_file_open(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp4F8="w",_tag_dyneither(_tmp4F8,sizeof(char),2))),((_tmp4F9="output file",_tag_dyneither(_tmp4F9,sizeof(char),12))));}else{
# 1030
out_file=(struct Cyc___cycFILE*)Cyc_stdout;}}else{
# 1031
if(Cyc_toc_r  && Cyc_output_file != 0){
# 1032
const char*_tmp4FB;const char*_tmp4FA;out_file=Cyc_try_file_open(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp4FA="w",_tag_dyneither(_tmp4FA,sizeof(char),2))),((_tmp4FB="output file",_tag_dyneither(_tmp4FB,sizeof(char),12))));}else{
# 1034
const char*_tmp4FD;const char*_tmp4FC;out_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpFC,((_tmp4FC="w",_tag_dyneither(_tmp4FC,sizeof(char),2))),((_tmp4FD="output file",_tag_dyneither(_tmp4FD,sizeof(char),12))));}}
# 1036
if(Cyc_compile_failure  || !((unsigned int)out_file)){_npop_handler(0);return;}
# 1038
{struct _handler_cons _tmp132;_push_handler(& _tmp132);{int _tmp134=0;if(setjmp(_tmp132.handler))_tmp134=1;if(!_tmp134){
# 1039
if(!Cyc_noprint_r){
# 1040
const char*_tmp4FE;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4FE="printing",_tag_dyneither(_tmp4FE,sizeof(char),9))),Cyc_do_print,(struct Cyc___cycFILE*)out_file,tds);}
# 1039
;_pop_handler();}else{void*_tmp133=(void*)_exn_thrown;void*_tmp137=_tmp133;_LL2B:;_LL2C:
# 1043
 Cyc_compile_failure=1;
# 1044
Cyc_file_close((struct Cyc___cycFILE*)out_file);
# 1045
{struct _dyneither_ptr*_tmp501;struct Cyc_List_List*_tmp500;Cyc_cfiles=((_tmp500=_cycalloc(sizeof(*_tmp500)),((_tmp500->hd=((_tmp501=_cycalloc(sizeof(*_tmp501)),((_tmp501[0]=(struct _dyneither_ptr)_tmpFC,_tmp501)))),((_tmp500->tl=Cyc_cfiles,_tmp500))))));}
# 1046
(int)_throw(_tmp137);_LL2D:;_LL2E:(void)_throw(_tmp137);_LL2A:;}};}
# 1048
Cyc_file_close((struct Cyc___cycFILE*)out_file);{
# 1049
struct _dyneither_ptr*_tmp504;struct Cyc_List_List*_tmp503;Cyc_cfiles=((_tmp503=_cycalloc(sizeof(*_tmp503)),((_tmp503->hd=((_tmp504=_cycalloc(sizeof(*_tmp504)),((_tmp504[0]=(struct _dyneither_ptr)_tmpFC,_tmp504)))),((_tmp503->tl=Cyc_cfiles,_tmp503))))));};}
# 965
;_pop_region(tc_rgn);};};};};};};};};};}static char _tmp154[8]="<final>";
# 1053
static struct _dyneither_ptr Cyc_final_str={_tmp154,_tmp154,_tmp154 + 8};
# 1054
static struct _dyneither_ptr*Cyc_final_strptr=& Cyc_final_str;
# 1056
static struct Cyc_Interface_I*Cyc_read_cycio(struct _dyneither_ptr*n){
# 1057
if(n == (struct _dyneither_ptr*)Cyc_final_strptr)
# 1058
return Cyc_Interface_final();{
# 1060
struct _dyneither_ptr basename;
# 1061
{struct _handler_cons _tmp155;_push_handler(& _tmp155);{int _tmp157=0;if(setjmp(_tmp155.handler))_tmp157=1;if(!_tmp157){
# 1062
basename=(struct _dyneither_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*_tmp156=(void*)_exn_thrown;void*_tmp159=_tmp156;_LL30: {struct Cyc_Core_Invalid_argument_exn_struct*_tmp15A=(struct Cyc_Core_Invalid_argument_exn_struct*)_tmp159;if(_tmp15A->tag != Cyc_Core_Invalid_argument)goto _LL32;}_LL31:
# 1064
 basename=*n;goto _LL2F;_LL32:;_LL33:(void)_throw(_tmp159);_LL2F:;}};}{
# 1066
const char*_tmp505;struct _dyneither_ptr _tmp15B=Cyc_strconcat((struct _dyneither_ptr)basename,((_tmp505=".cycio",_tag_dyneither(_tmp505,sizeof(char),7))));
# 1067
const char*_tmp507;const char*_tmp506;struct Cyc___cycFILE*_tmp15C=Cyc_try_file_open((struct _dyneither_ptr)_tmp15B,((_tmp506="rb",_tag_dyneither(_tmp506,sizeof(char),3))),((_tmp507="interface object file",_tag_dyneither(_tmp507,sizeof(char),22))));
# 1068
if(_tmp15C == 0){
# 1069
Cyc_compile_failure=1;
# 1070
Cyc_remove_cfiles();
# 1071
exit(1);}
# 1073
Cyc_Position_reset_position((struct _dyneither_ptr)_tmp15B);{
# 1075
struct Cyc_Interface_I*_tmp15D=Cyc_Interface_load((struct Cyc___cycFILE*)_tmp15C);
# 1076
Cyc_file_close((struct Cyc___cycFILE*)_tmp15C);
# 1077
return _tmp15D;};};};}
# 1080
static int Cyc_is_cfile(struct _dyneither_ptr*n){
# 1081
return*((const char*)_check_dyneither_subscript(*n,sizeof(char),0))!= '-';}
# 1084
void GC_blacklist_warn_clear();struct _tuple25{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};int Cyc_main(int argc,struct _dyneither_ptr argv);static void _tmp50D(unsigned int*_tmp50C,unsigned int*_tmp50B,int**_tmp509){for(*_tmp50C=0;*_tmp50C < *_tmp50B;(*_tmp50C)++){(*_tmp509)[*_tmp50C]=0;}}static void _tmp515(unsigned int*_tmp514,unsigned int*_tmp513,struct _dyneither_ptr**_tmp511){for(*_tmp514=0;*_tmp514 < *_tmp513;(*_tmp514)++){(*_tmp511)[*_tmp514]=(struct _dyneither_ptr)
# 1121
_tag_dyneither(0,0,0);}}static void _tmp51B(unsigned int*_tmp51A,unsigned int*_tmp519,struct _dyneither_ptr**_tmp517){for(*_tmp51A=0;*_tmp51A < *_tmp519;(*_tmp51A)++){(*_tmp517)[*_tmp51A]=(struct _dyneither_ptr)
# 1122
_tag_dyneither(0,0,0);}}
# 1087
int Cyc_main(int argc,struct _dyneither_ptr argv){
# 1092
GC_blacklist_warn_clear();{
# 1094
struct Cyc_List_List*cyclone_arch_path;
# 1095
struct _dyneither_ptr def_lib_path;
# 1096
struct _dyneither_ptr comp=(struct _dyneither_ptr)Cstring_to_string(Ccomp);
# 1099
struct _RegionHandle _tmp161=_new_region("r");struct _RegionHandle*r=& _tmp161;_push_region(r);{
# 1101
unsigned int _tmp50C;unsigned int _tmp50B;struct _dyneither_ptr _tmp50A;int*_tmp509;unsigned int _tmp508;struct _dyneither_ptr _tmp162=(_tmp508=(unsigned int)argc,((_tmp509=(int*)_cycalloc_atomic(_check_times(sizeof(int),_tmp508)),((_tmp50A=_tag_dyneither(_tmp509,sizeof(int),_tmp508),((((_tmp50B=_tmp508,_tmp50D(& _tmp50C,& _tmp50B,& _tmp509))),_tmp50A)))))));
# 1102
int _tmp163=0;
# 1103
int i;int j;int k;
# 1104
for(i=1;i < argc;++ i){
# 1105
const char*_tmp50E;if(Cyc_strncmp(((_tmp50E="-B",_tag_dyneither(_tmp50E,sizeof(char),3))),(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)),2)== 0){
# 1106
*((int*)_check_dyneither_subscript(_tmp162,sizeof(int),i))=1;
# 1107
++ _tmp163;}else{
# 1109
const char*_tmp50F;if(Cyc_strcmp(((_tmp50F="-b",_tag_dyneither(_tmp50F,sizeof(char),3))),(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)))== 0){
# 1110
*((int*)_check_dyneither_subscript(_tmp162,sizeof(int),i))=1;
# 1111
++ _tmp163;
# 1112
++ i;if(i >= argc)break;
# 1113
*((int*)_check_dyneither_subscript(_tmp162,sizeof(int),i))=1;
# 1114
++ _tmp163;}}}{
# 1121
unsigned int _tmp514;unsigned int _tmp513;struct _dyneither_ptr _tmp512;struct _dyneither_ptr*_tmp511;unsigned int _tmp510;struct _dyneither_ptr _tmp166=(_tmp510=(unsigned int)(_tmp163 + 1),((_tmp511=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp510)),((_tmp512=_tag_dyneither(_tmp511,sizeof(struct _dyneither_ptr),_tmp510),((((_tmp513=_tmp510,_tmp515(& _tmp514,& _tmp513,& _tmp511))),_tmp512)))))));
# 1122
unsigned int _tmp51A;unsigned int _tmp519;struct _dyneither_ptr _tmp518;struct _dyneither_ptr*_tmp517;unsigned int _tmp516;struct _dyneither_ptr _tmp167=(_tmp516=(unsigned int)(argc - _tmp163),((_tmp517=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp516)),((_tmp518=_tag_dyneither(_tmp517,sizeof(struct _dyneither_ptr),_tmp516),((((_tmp519=_tmp516,_tmp51B(& _tmp51A,& _tmp519,& _tmp517))),_tmp518)))))));
# 1123
*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp166,sizeof(struct _dyneither_ptr),0))=(*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp167,sizeof(struct _dyneither_ptr),0))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),0)));
# 1124
for(i=(j=(k=1));i < argc;++ i){
# 1125
if(*((int*)_check_dyneither_subscript(_tmp162,sizeof(int),i)))*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp166,sizeof(struct _dyneither_ptr),j ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));else{
# 1126
*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp167,sizeof(struct _dyneither_ptr),k ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));}}{
# 1130
struct _tuple25*_tmpA50;const char*_tmpA4F;const char*_tmpA4E;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmpA4D;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmpA4C;const char*_tmpA4B;struct _tuple25*_tmpA4A;const char*_tmpA49;const char*_tmpA48;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA47;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA46;const char*_tmpA45;struct _tuple25*_tmpA44;const char*_tmpA43;const char*_tmpA42;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmpA41;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmpA40;const char*_tmpA3F;struct _tuple25*_tmpA3E;const char*_tmpA3D;const char*_tmpA3C;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA3B;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA3A;const char*_tmpA39;struct _tuple25*_tmpA38;const char*_tmpA37;const char*_tmpA36;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA35;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA34;const char*_tmpA33;struct _tuple25*_tmpA32;const char*_tmpA31;const char*_tmpA30;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA2F;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA2E;const char*_tmpA2D;struct _tuple25*_tmpA2C;const char*_tmpA2B;const char*_tmpA2A;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA29;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA28;const char*_tmpA27;struct _tuple25*_tmpA26;const char*_tmpA25;const char*_tmpA24;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA23;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA22;const char*_tmpA21;struct _tuple25*_tmpA20;const char*_tmpA1F;const char*_tmpA1E;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA1D;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA1C;const char*_tmpA1B;struct _tuple25*_tmpA1A;const char*_tmpA19;const char*_tmpA18;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA17;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA16;const char*_tmpA15;struct _tuple25*_tmpA14;const char*_tmpA13;const char*_tmpA12;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmpA11;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmpA10;const char*_tmpA0F;struct _tuple25*_tmpA0E;const char*_tmpA0D;const char*_tmpA0C;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA0B;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA0A;const char*_tmpA09;struct _tuple25*_tmpA08;const char*_tmpA07;const char*_tmpA06;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA05;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA04;const char*_tmpA03;struct _tuple25*_tmpA02;const char*_tmpA01;const char*_tmpA00;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp9FF;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp9FE;const char*_tmp9FD;struct _tuple25*_tmp9FC;const char*_tmp9FB;const char*_tmp9FA;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp9F9;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp9F8;const char*_tmp9F7;struct _tuple25*_tmp9F6;const char*_tmp9F5;const char*_tmp9F4;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp9F3;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp9F2;const char*_tmp9F1;struct _tuple25*_tmp9F0;const char*_tmp9EF;const char*_tmp9EE;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp9ED;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp9EC;const char*_tmp9EB;struct _tuple25*_tmp9EA;const char*_tmp9E9;const char*_tmp9E8;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp9E7;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp9E6;const char*_tmp9E5;struct _tuple25*_tmp9E4;const char*_tmp9E3;const char*_tmp9E2;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp9E1;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp9E0;const char*_tmp9DF;struct _tuple25*_tmp9DE;const char*_tmp9DD;const char*_tmp9DC;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9DB;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9DA;const char*_tmp9D9;struct _tuple25*_tmp9D8;const char*_tmp9D7;const char*_tmp9D6;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9D5;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9D4;const char*_tmp9D3;struct _tuple25*_tmp9D2;const char*_tmp9D1;const char*_tmp9D0;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9CF;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9CE;const char*_tmp9CD;struct _tuple25*_tmp9CC;const char*_tmp9CB;const char*_tmp9CA;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9C9;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9C8;const char*_tmp9C7;struct _tuple25*_tmp9C6;const char*_tmp9C5;const char*_tmp9C4;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp9C3;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp9C2;const char*_tmp9C1;struct _tuple25*_tmp9C0;const char*_tmp9BF;const char*_tmp9BE;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp9BD;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp9BC;const char*_tmp9BB;struct _tuple25*_tmp9BA;const char*_tmp9B9;const char*_tmp9B8;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp9B7;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp9B6;const char*_tmp9B5;struct _tuple25*_tmp9B4;const char*_tmp9B3;const char*_tmp9B2;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9B1;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9B0;const char*_tmp9AF;struct _tuple25*_tmp9AE;const char*_tmp9AD;const char*_tmp9AC;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9AB;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9AA;const char*_tmp9A9;struct _tuple25*_tmp9A8;const char*_tmp9A7;const char*_tmp9A6;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9A5;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9A4;const char*_tmp9A3;struct _tuple25*_tmp9A2;const char*_tmp9A1;const char*_tmp9A0;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp99F;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp99E;const char*_tmp99D;struct _tuple25*_tmp99C;const char*_tmp99B;const char*_tmp99A;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp999;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp998;const char*_tmp997;struct _tuple25*_tmp996;const char*_tmp995;const char*_tmp994;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp993;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp992;const char*_tmp991;struct _tuple25*_tmp990;const char*_tmp98F;const char*_tmp98E;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp98D;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp98C;const char*_tmp98B;struct _tuple25*_tmp98A;const char*_tmp989;const char*_tmp988;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp987;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp986;const char*_tmp985;struct _tuple25*_tmp984;const char*_tmp983;const char*_tmp982;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp981;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp980;const char*_tmp97F;struct _tuple25*_tmp97E;const char*_tmp97D;const char*_tmp97C;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp97B;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp97A;const char*_tmp979;struct _tuple25*_tmp978;const char*_tmp977;const char*_tmp976;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp975;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp974;const char*_tmp973;struct _tuple25*_tmp972;const char*_tmp971;const char*_tmp970;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp96F;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp96E;const char*_tmp96D;struct _tuple25*_tmp96C;const char*_tmp96B;const char*_tmp96A;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp969;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp968;const char*_tmp967;struct _tuple25*_tmp966;const char*_tmp965;const char*_tmp964;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp963;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp962;const char*_tmp961;struct _tuple25*_tmp960;const char*_tmp95F;const char*_tmp95E;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp95D;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp95C;const char*_tmp95B;struct _tuple25*_tmp95A;const char*_tmp959;const char*_tmp958;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp957;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp956;const char*_tmp955;struct _tuple25*_tmp954;const char*_tmp953;const char*_tmp952;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp951;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp950;const char*_tmp94F;struct _tuple25*_tmp94E;const char*_tmp94D;const char*_tmp94C;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp94B;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp94A;const char*_tmp949;struct _tuple25*_tmp948;const char*_tmp947;const char*_tmp946;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp945;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp944;const char*_tmp943;struct _tuple25*_tmp942;const char*_tmp941;const char*_tmp940;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp93F;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp93E;const char*_tmp93D;struct _tuple25*_tmp93C;const char*_tmp93B;const char*_tmp93A;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp939;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp938;const char*_tmp937;struct _tuple25*_tmp936;const char*_tmp935;const char*_tmp934;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp933;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp932;const char*_tmp931;struct _tuple25*_tmp930;const char*_tmp92F;const char*_tmp92E;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp92D;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp92C;const char*_tmp92B;struct _tuple25*_tmp92A;const char*_tmp929;const char*_tmp928;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp927;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp926;const char*_tmp925;struct _tuple25*_tmp924;const char*_tmp923;const char*_tmp922;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp921;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp920;const char*_tmp91F;struct _tuple25*_tmp91E;const char*_tmp91D;const char*_tmp91C;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp91B;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp91A;const char*_tmp919;struct _tuple25*_tmp918;const char*_tmp917;const char*_tmp916;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp915;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp914;const char*_tmp913;struct _tuple25*_tmp912;const char*_tmp911;const char*_tmp910;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp90F;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp90E;const char*_tmp90D;struct _tuple25*_tmp90C;const char*_tmp90B;const char*_tmp90A;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp909;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp908;const char*_tmp907;struct _tuple25*_tmp906;const char*_tmp905;const char*_tmp904;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp903;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp902;const char*_tmp901;struct _tuple25*_tmp900;const char*_tmp8FF;const char*_tmp8FE;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8FD;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8FC;const char*_tmp8FB;struct _tuple25*_tmp8FA;const char*_tmp8F9;const char*_tmp8F8;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8F7;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8F6;const char*_tmp8F5;struct _tuple25*_tmp8F4;const char*_tmp8F3;const char*_tmp8F2;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8F1;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8F0;const char*_tmp8EF;struct _tuple25*_tmp8EE;const char*_tmp8ED;const char*_tmp8EC;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8EB;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8EA;const char*_tmp8E9;struct _tuple25*_tmp8E8;const char*_tmp8E7;const char*_tmp8E6;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8E5;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8E4;const char*_tmp8E3;struct _tuple25*_tmp8E2;const char*_tmp8E1;const char*_tmp8E0;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8DF;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8DE;const char*_tmp8DD;struct _tuple25*_tmp8DC;const char*_tmp8DB;const char*_tmp8DA;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8D9;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8D8;const char*_tmp8D7;struct _tuple25*_tmp8D6;const char*_tmp8D5;const char*_tmp8D4;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8D3;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8D2;const char*_tmp8D1;struct _tuple25*_tmp8D0;const char*_tmp8CF;const char*_tmp8CE;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8CD;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8CC;const char*_tmp8CB;struct _tuple25*_tmp8CA;const char*_tmp8C9;const char*_tmp8C8;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8C7;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8C6;const char*_tmp8C5;struct _tuple25*_tmp8C4;const char*_tmp8C3;const char*_tmp8C2;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp8C1;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp8C0;const char*_tmp8BF;struct _tuple25*_tmp8BE;const char*_tmp8BD;const char*_tmp8BC;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp8BB;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp8BA;const char*_tmp8B9;struct _tuple25*_tmp8B8;const char*_tmp8B7;const char*_tmp8B6;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8B5;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8B4;const char*_tmp8B3;struct _tuple25*_tmp8B2;const char*_tmp8B1;const char*_tmp8B0;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8AF;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8AE;const char*_tmp8AD;struct _tuple25*_tmp8AC;const char*_tmp8AB;const char*_tmp8AA;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8A9;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8A8;const char*_tmp8A7;struct _tuple25*_tmp8A6;const char*_tmp8A5;const char*_tmp8A4;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8A3;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8A2;const char*_tmp8A1;struct _tuple25*_tmp8A0;const char*_tmp89F;const char*_tmp89E;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp89D;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp89C;const char*_tmp89B;struct _tuple25*_tmp89A;const char*_tmp899;const char*_tmp898;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp897;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp896;const char*_tmp895;struct _tuple25*_tmp894[74];struct Cyc_List_List*options=
# 1131
(_tmp894[73]=(
# 1352
(_tmp89A=_region_malloc(r,sizeof(*_tmp89A)),((_tmp89A->f1=((_tmp899="-idirafter",_tag_dyneither(_tmp899,sizeof(char),11))),((_tmp89A->f2=1,((_tmp89A->f3=((_tmp898="<dir>",_tag_dyneither(_tmp898,sizeof(char),6))),((_tmp89A->f4=(void*)(
# 1353
(_tmp896=_region_malloc(r,sizeof(*_tmp896)),((_tmp896[0]=((_tmp897.tag=5,((_tmp897.f1=Cyc_add_idirafter,_tmp897)))),_tmp896)))),((_tmp89A->f5=(
# 1354
(_tmp895="Add the directory to the second include path.",_tag_dyneither(_tmp895,sizeof(char),46))),_tmp89A)))))))))))),((_tmp894[72]=(
# 1349
(_tmp8A0=_region_malloc(r,sizeof(*_tmp8A0)),((_tmp8A0->f1=((_tmp89F="-isystem",_tag_dyneither(_tmp89F,sizeof(char),9))),((_tmp8A0->f2=1,((_tmp8A0->f3=((_tmp89E="<dir>",_tag_dyneither(_tmp89E,sizeof(char),6))),((_tmp8A0->f4=(void*)(
# 1350
(_tmp89C=_region_malloc(r,sizeof(*_tmp89C)),((_tmp89C[0]=((_tmp89D.tag=5,((_tmp89D.f1=Cyc_add_isystem,_tmp89D)))),_tmp89C)))),((_tmp8A0->f5=(
# 1351
(_tmp89B="Add <dir> to the beginning of the second include path and treat it as a\nsystem directory.",_tag_dyneither(_tmp89B,sizeof(char),90))),_tmp8A0)))))))))))),((_tmp894[71]=(
# 1346
(_tmp8A6=_region_malloc(r,sizeof(*_tmp8A6)),((_tmp8A6->f1=((_tmp8A5="-iwithprefixbefore",_tag_dyneither(_tmp8A5,sizeof(char),19))),((_tmp8A6->f2=1,((_tmp8A6->f3=((_tmp8A4="<dir>",_tag_dyneither(_tmp8A4,sizeof(char),6))),((_tmp8A6->f4=(void*)(
# 1347
(_tmp8A2=_region_malloc(r,sizeof(*_tmp8A2)),((_tmp8A2[0]=((_tmp8A3.tag=5,((_tmp8A3.f1=Cyc_add_iwithprefixbefore,_tmp8A3)))),_tmp8A2)))),((_tmp8A6->f5=(
# 1348
(_tmp8A1="Add <prefix>/<dir> to the main include path.",_tag_dyneither(_tmp8A1,sizeof(char),45))),_tmp8A6)))))))))))),((_tmp894[70]=(
# 1343
(_tmp8AC=_region_malloc(r,sizeof(*_tmp8AC)),((_tmp8AC->f1=((_tmp8AB="-iwithprefix",_tag_dyneither(_tmp8AB,sizeof(char),13))),((_tmp8AC->f2=1,((_tmp8AC->f3=((_tmp8AA="<dir>",_tag_dyneither(_tmp8AA,sizeof(char),6))),((_tmp8AC->f4=(void*)(
# 1344
(_tmp8A8=_region_malloc(r,sizeof(*_tmp8A8)),((_tmp8A8[0]=((_tmp8A9.tag=5,((_tmp8A9.f1=Cyc_add_iwithprefix,_tmp8A9)))),_tmp8A8)))),((_tmp8AC->f5=(
# 1345
(_tmp8A7="Add <prefix>/<dir> to the second include path.",_tag_dyneither(_tmp8A7,sizeof(char),47))),_tmp8AC)))))))))))),((_tmp894[69]=(
# 1340
(_tmp8B2=_region_malloc(r,sizeof(*_tmp8B2)),((_tmp8B2->f1=((_tmp8B1="-iprefix",_tag_dyneither(_tmp8B1,sizeof(char),9))),((_tmp8B2->f2=1,((_tmp8B2->f3=((_tmp8B0="<prefix>",_tag_dyneither(_tmp8B0,sizeof(char),9))),((_tmp8B2->f4=(void*)(
# 1341
(_tmp8AE=_region_malloc(r,sizeof(*_tmp8AE)),((_tmp8AE[0]=((_tmp8AF.tag=5,((_tmp8AF.f1=Cyc_add_iprefix,_tmp8AF)))),_tmp8AE)))),((_tmp8B2->f5=(
# 1342
(_tmp8AD="Specify <prefix> as the prefix for subsequent -iwithprefix options",_tag_dyneither(_tmp8AD,sizeof(char),67))),_tmp8B2)))))))))))),((_tmp894[68]=(
# 1337
(_tmp8B8=_region_malloc(r,sizeof(*_tmp8B8)),((_tmp8B8->f1=((_tmp8B7="-noregions",_tag_dyneither(_tmp8B7,sizeof(char),11))),((_tmp8B8->f2=0,((_tmp8B8->f3=((_tmp8B6="",_tag_dyneither(_tmp8B6,sizeof(char),1))),((_tmp8B8->f4=(void*)(
# 1338
(_tmp8B4=_region_malloc(r,sizeof(*_tmp8B4)),((_tmp8B4[0]=((_tmp8B5.tag=3,((_tmp8B5.f1=& Cyc_Absyn_no_regions,_tmp8B5)))),_tmp8B4)))),((_tmp8B8->f5=(
# 1339
(_tmp8B3="Generate code that doesn't use regions",_tag_dyneither(_tmp8B3,sizeof(char),39))),_tmp8B8)))))))))))),((_tmp894[67]=(
# 1334
(_tmp8BE=_region_malloc(r,sizeof(*_tmp8BE)),((_tmp8BE->f1=((_tmp8BD="-port",_tag_dyneither(_tmp8BD,sizeof(char),6))),((_tmp8BE->f2=0,((_tmp8BE->f3=((_tmp8BC="",_tag_dyneither(_tmp8BC,sizeof(char),1))),((_tmp8BE->f4=(void*)(
# 1335
(_tmp8BA=_region_malloc(r,sizeof(*_tmp8BA)),((_tmp8BA[0]=((_tmp8BB.tag=0,((_tmp8BB.f1=Cyc_set_port_c_code,_tmp8BB)))),_tmp8BA)))),((_tmp8BE->f5=(
# 1336
(_tmp8B9="Suggest how to port C code to Cyclone",_tag_dyneither(_tmp8B9,sizeof(char),38))),_tmp8BE)))))))))))),((_tmp894[66]=(
# 1331
(_tmp8C4=_region_malloc(r,sizeof(*_tmp8C4)),((_tmp8C4->f1=((_tmp8C3="-detailedlocation",_tag_dyneither(_tmp8C3,sizeof(char),18))),((_tmp8C4->f2=0,((_tmp8C4->f3=((_tmp8C2="",_tag_dyneither(_tmp8C2,sizeof(char),1))),((_tmp8C4->f4=(void*)(
# 1332
(_tmp8C0=_region_malloc(r,sizeof(*_tmp8C0)),((_tmp8C0[0]=((_tmp8C1.tag=4,((_tmp8C1.f1=& Cyc_Position_use_gcc_style_location,_tmp8C1)))),_tmp8C0)))),((_tmp8C4->f5=(
# 1333
(_tmp8BF="Try to give more detailed location information for errors",_tag_dyneither(_tmp8BF,sizeof(char),58))),_tmp8C4)))))))))))),((_tmp894[65]=(
# 1328
(_tmp8CA=_region_malloc(r,sizeof(*_tmp8CA)),((_tmp8CA->f1=((_tmp8C9="-noregister",_tag_dyneither(_tmp8C9,sizeof(char),12))),((_tmp8CA->f2=0,((_tmp8CA->f3=((_tmp8C8="",_tag_dyneither(_tmp8C8,sizeof(char),1))),((_tmp8CA->f4=(void*)(
# 1329
(_tmp8C6=_region_malloc(r,sizeof(*_tmp8C6)),((_tmp8C6[0]=((_tmp8C7.tag=3,((_tmp8C7.f1=& Cyc_Parse_no_register,_tmp8C7)))),_tmp8C6)))),((_tmp8CA->f5=(
# 1330
(_tmp8C5="Treat register storage class as public",_tag_dyneither(_tmp8C5,sizeof(char),39))),_tmp8CA)))))))))))),((_tmp894[64]=(
# 1325
(_tmp8D0=_region_malloc(r,sizeof(*_tmp8D0)),((_tmp8D0->f1=((_tmp8CF="-warnregioncoerce",_tag_dyneither(_tmp8CF,sizeof(char),18))),((_tmp8D0->f2=0,((_tmp8D0->f3=((_tmp8CE="",_tag_dyneither(_tmp8CE,sizeof(char),1))),((_tmp8D0->f4=(void*)(
# 1326
(_tmp8CC=_region_malloc(r,sizeof(*_tmp8CC)),((_tmp8CC[0]=((_tmp8CD.tag=3,((_tmp8CD.f1=& Cyc_Tcutil_warn_region_coerce,_tmp8CD)))),_tmp8CC)))),((_tmp8D0->f5=(
# 1327
(_tmp8CB="Warn when any region coercion is inserted",_tag_dyneither(_tmp8CB,sizeof(char),42))),_tmp8D0)))))))))))),((_tmp894[63]=(
# 1322
(_tmp8D6=_region_malloc(r,sizeof(*_tmp8D6)),((_tmp8D6->f1=((_tmp8D5="-warnaliascoercion",_tag_dyneither(_tmp8D5,sizeof(char),19))),((_tmp8D6->f2=0,((_tmp8D6->f3=((_tmp8D4="",_tag_dyneither(_tmp8D4,sizeof(char),1))),((_tmp8D6->f4=(void*)(
# 1323
(_tmp8D2=_region_malloc(r,sizeof(*_tmp8D2)),((_tmp8D2[0]=((_tmp8D3.tag=3,((_tmp8D3.f1=& Cyc_Tcutil_warn_alias_coerce,_tmp8D3)))),_tmp8D2)))),((_tmp8D6->f5=(
# 1324
(_tmp8D1="Warn when any alias coercion is inserted",_tag_dyneither(_tmp8D1,sizeof(char),41))),_tmp8D6)))))))))))),((_tmp894[62]=(
# 1319
(_tmp8DC=_region_malloc(r,sizeof(*_tmp8DC)),((_tmp8DC->f1=((_tmp8DB="-warnnullchecks",_tag_dyneither(_tmp8DB,sizeof(char),16))),((_tmp8DC->f2=0,((_tmp8DC->f3=((_tmp8DA="",_tag_dyneither(_tmp8DA,sizeof(char),1))),((_tmp8DC->f4=(void*)(
# 1320
(_tmp8D8=_region_malloc(r,sizeof(*_tmp8D8)),((_tmp8D8[0]=((_tmp8D9.tag=3,((_tmp8D9.f1=& Cyc_Toc_warn_all_null_deref,_tmp8D9)))),_tmp8D8)))),((_tmp8DC->f5=(
# 1321
(_tmp8D7="Warn when any null check can't be eliminated",_tag_dyneither(_tmp8D7,sizeof(char),45))),_tmp8DC)))))))))))),((_tmp894[61]=(
# 1316
(_tmp8E2=_region_malloc(r,sizeof(*_tmp8E2)),((_tmp8E2->f1=((_tmp8E1="-warnboundschecks",_tag_dyneither(_tmp8E1,sizeof(char),18))),((_tmp8E2->f2=0,((_tmp8E2->f3=((_tmp8E0="",_tag_dyneither(_tmp8E0,sizeof(char),1))),((_tmp8E2->f4=(void*)(
# 1317
(_tmp8DE=_region_malloc(r,sizeof(*_tmp8DE)),((_tmp8DE[0]=((_tmp8DF.tag=3,((_tmp8DF.f1=& Cyc_Toc_warn_bounds_checks,_tmp8DF)))),_tmp8DE)))),((_tmp8E2->f5=(
# 1318
(_tmp8DD="Warn when bounds checks can't be eliminated",_tag_dyneither(_tmp8DD,sizeof(char),44))),_tmp8E2)))))))))))),((_tmp894[60]=(
# 1313
(_tmp8E8=_region_malloc(r,sizeof(*_tmp8E8)),((_tmp8E8->f1=((_tmp8E7="-CI",_tag_dyneither(_tmp8E7,sizeof(char),4))),((_tmp8E8->f2=0,((_tmp8E8->f3=((_tmp8E6=" <file>",_tag_dyneither(_tmp8E6,sizeof(char),8))),((_tmp8E8->f4=(void*)(
# 1314
(_tmp8E4=_region_malloc(r,sizeof(*_tmp8E4)),((_tmp8E4[0]=((_tmp8E5.tag=5,((_tmp8E5.f1=Cyc_set_cyc_include,_tmp8E5)))),_tmp8E4)))),((_tmp8E8->f5=(
# 1315
(_tmp8E3="Set cyc_include.h to be <file>",_tag_dyneither(_tmp8E3,sizeof(char),31))),_tmp8E8)))))))))))),((_tmp894[59]=(
# 1310
(_tmp8EE=_region_malloc(r,sizeof(*_tmp8EE)),((_tmp8EE->f1=((_tmp8ED="-compile-for-boot",_tag_dyneither(_tmp8ED,sizeof(char),18))),((_tmp8EE->f2=0,((_tmp8EE->f3=((_tmp8EC="",_tag_dyneither(_tmp8EC,sizeof(char),1))),((_tmp8EE->f4=(void*)(
# 1311
(_tmp8EA=_region_malloc(r,sizeof(*_tmp8EA)),((_tmp8EA[0]=((_tmp8EB.tag=3,((_tmp8EB.f1=& Cyc_Lex_compile_for_boot_r,_tmp8EB)))),_tmp8EA)))),((_tmp8EE->f5=(
# 1312
(_tmp8E9="Compile using the special boot library instead of the standard library",_tag_dyneither(_tmp8E9,sizeof(char),71))),_tmp8EE)))))))))))),((_tmp894[58]=(
# 1307
(_tmp8F4=_region_malloc(r,sizeof(*_tmp8F4)),((_tmp8F4->f1=((_tmp8F3="-nocyc_setjmp",_tag_dyneither(_tmp8F3,sizeof(char),14))),((_tmp8F4->f2=0,((_tmp8F4->f3=((_tmp8F2="",_tag_dyneither(_tmp8F2,sizeof(char),1))),((_tmp8F4->f4=(void*)(
# 1308
(_tmp8F0=_region_malloc(r,sizeof(*_tmp8F0)),((_tmp8F0[0]=((_tmp8F1.tag=3,((_tmp8F1.f1=& Cyc_nocyc_setjmp_r,_tmp8F1)))),_tmp8F0)))),((_tmp8F4->f5=(
# 1309
(_tmp8EF="Do not use compiler special file cyc_setjmp.h",_tag_dyneither(_tmp8EF,sizeof(char),46))),_tmp8F4)))))))))))),((_tmp894[57]=(
# 1304
(_tmp8FA=_region_malloc(r,sizeof(*_tmp8FA)),((_tmp8FA->f1=((_tmp8F9="-printalleffects",_tag_dyneither(_tmp8F9,sizeof(char),17))),((_tmp8FA->f2=0,((_tmp8FA->f3=((_tmp8F8="",_tag_dyneither(_tmp8F8,sizeof(char),1))),((_tmp8FA->f4=(void*)(
# 1305
(_tmp8F6=_region_malloc(r,sizeof(*_tmp8F6)),((_tmp8F6[0]=((_tmp8F7.tag=3,((_tmp8F7.f1=& Cyc_print_all_effects_r,_tmp8F7)))),_tmp8F6)))),((_tmp8FA->f5=(
# 1306
(_tmp8F5="Print effects for functions (type debugging)",_tag_dyneither(_tmp8F5,sizeof(char),45))),_tmp8FA)))))))))))),((_tmp894[56]=(
# 1301
(_tmp900=_region_malloc(r,sizeof(*_tmp900)),((_tmp900->f1=((_tmp8FF="-printfullevars",_tag_dyneither(_tmp8FF,sizeof(char),16))),((_tmp900->f2=0,((_tmp900->f3=((_tmp8FE="",_tag_dyneither(_tmp8FE,sizeof(char),1))),((_tmp900->f4=(void*)(
# 1302
(_tmp8FC=_region_malloc(r,sizeof(*_tmp8FC)),((_tmp8FC[0]=((_tmp8FD.tag=3,((_tmp8FD.f1=& Cyc_print_full_evars_r,_tmp8FD)))),_tmp8FC)))),((_tmp900->f5=(
# 1303
(_tmp8FB="Print full information for evars (type debugging)",_tag_dyneither(_tmp8FB,sizeof(char),50))),_tmp900)))))))))))),((_tmp894[55]=(
# 1298
(_tmp906=_region_malloc(r,sizeof(*_tmp906)),((_tmp906->f1=((_tmp905="-printallkinds",_tag_dyneither(_tmp905,sizeof(char),15))),((_tmp906->f2=0,((_tmp906->f3=((_tmp904="",_tag_dyneither(_tmp904,sizeof(char),1))),((_tmp906->f4=(void*)(
# 1299
(_tmp902=_region_malloc(r,sizeof(*_tmp902)),((_tmp902[0]=((_tmp903.tag=3,((_tmp903.f1=& Cyc_print_all_kinds_r,_tmp903)))),_tmp902)))),((_tmp906->f5=(
# 1300
(_tmp901="Always print kinds of type variables",_tag_dyneither(_tmp901,sizeof(char),37))),_tmp906)))))))))))),((_tmp894[54]=(
# 1295
(_tmp90C=_region_malloc(r,sizeof(*_tmp90C)),((_tmp90C->f1=((_tmp90B="-printalltvars",_tag_dyneither(_tmp90B,sizeof(char),15))),((_tmp90C->f2=0,((_tmp90C->f3=((_tmp90A="",_tag_dyneither(_tmp90A,sizeof(char),1))),((_tmp90C->f4=(void*)(
# 1296
(_tmp908=_region_malloc(r,sizeof(*_tmp908)),((_tmp908[0]=((_tmp909.tag=3,((_tmp909.f1=& Cyc_print_all_tvars_r,_tmp909)))),_tmp908)))),((_tmp90C->f5=(
# 1297
(_tmp907="Print all type variables (even implicit default effects)",_tag_dyneither(_tmp907,sizeof(char),57))),_tmp90C)))))))))))),((_tmp894[53]=(
# 1292
(_tmp912=_region_malloc(r,sizeof(*_tmp912)),((_tmp912->f1=((_tmp911="-noexpandtypedefs",_tag_dyneither(_tmp911,sizeof(char),18))),((_tmp912->f2=0,((_tmp912->f3=((_tmp910="",_tag_dyneither(_tmp910,sizeof(char),1))),((_tmp912->f4=(void*)(
# 1293
(_tmp90E=_region_malloc(r,sizeof(*_tmp90E)),((_tmp90E[0]=((_tmp90F.tag=3,((_tmp90F.f1=& Cyc_noexpand_r,_tmp90F)))),_tmp90E)))),((_tmp912->f5=(
# 1294
(_tmp90D="Don't expand typedefs in pretty printing",_tag_dyneither(_tmp90D,sizeof(char),41))),_tmp912)))))))))))),((_tmp894[52]=(
# 1289
(_tmp918=_region_malloc(r,sizeof(*_tmp918)),((_tmp918->f1=((_tmp917="-noremoveunused",_tag_dyneither(_tmp917,sizeof(char),16))),((_tmp918->f2=0,((_tmp918->f3=((_tmp916="",_tag_dyneither(_tmp916,sizeof(char),1))),((_tmp918->f4=(void*)(
# 1290
(_tmp914=_region_malloc(r,sizeof(*_tmp914)),((_tmp914[0]=((_tmp915.tag=3,((_tmp915.f1=& Cyc_noshake_r,_tmp915)))),_tmp914)))),((_tmp918->f5=(
# 1291
(_tmp913="Don't remove externed variables that aren't used",_tag_dyneither(_tmp913,sizeof(char),49))),_tmp918)))))))))))),((_tmp894[51]=(
# 1286
(_tmp91E=_region_malloc(r,sizeof(*_tmp91E)),((_tmp91E->f1=((_tmp91D="-nogc",_tag_dyneither(_tmp91D,sizeof(char),6))),((_tmp91E->f2=0,((_tmp91E->f3=((_tmp91C="",_tag_dyneither(_tmp91C,sizeof(char),1))),((_tmp91E->f4=(void*)(
# 1287
(_tmp91A=_region_malloc(r,sizeof(*_tmp91A)),((_tmp91A[0]=((_tmp91B.tag=3,((_tmp91B.f1=& Cyc_nogc_r,_tmp91B)))),_tmp91A)))),((_tmp91E->f5=(
# 1288
(_tmp919="Don't link in the garbage collector",_tag_dyneither(_tmp919,sizeof(char),36))),_tmp91E)))))))))))),((_tmp894[50]=(
# 1283
(_tmp924=_region_malloc(r,sizeof(*_tmp924)),((_tmp924->f1=((_tmp923="-nocyc",_tag_dyneither(_tmp923,sizeof(char),7))),((_tmp924->f2=0,((_tmp924->f3=((_tmp922="",_tag_dyneither(_tmp922,sizeof(char),1))),((_tmp924->f4=(void*)(
# 1284
(_tmp920=_region_malloc(r,sizeof(*_tmp920)),((_tmp920[0]=((_tmp921.tag=0,((_tmp921.f1=Cyc_set_nocyc,_tmp921)))),_tmp920)))),((_tmp924->f5=(
# 1285
(_tmp91F="Don't add implicit namespace Cyc",_tag_dyneither(_tmp91F,sizeof(char),33))),_tmp924)))))))))))),((_tmp894[49]=(
# 1280
(_tmp92A=_region_malloc(r,sizeof(*_tmp92A)),((_tmp92A->f1=((_tmp929="-no-cpp-precomp",_tag_dyneither(_tmp929,sizeof(char),16))),((_tmp92A->f2=0,((_tmp92A->f3=((_tmp928="",_tag_dyneither(_tmp928,sizeof(char),1))),((_tmp92A->f4=(void*)(
# 1281
(_tmp926=_region_malloc(r,sizeof(*_tmp926)),((_tmp926[0]=((_tmp927.tag=0,((_tmp927.f1=Cyc_set_nocppprecomp,_tmp927)))),_tmp926)))),((_tmp92A->f5=(
# 1282
(_tmp925="Don't do smart preprocessing (mac only)",_tag_dyneither(_tmp925,sizeof(char),40))),_tmp92A)))))))))))),((_tmp894[48]=(
# 1277
(_tmp930=_region_malloc(r,sizeof(*_tmp930)),((_tmp930->f1=((_tmp92F="-use-cpp",_tag_dyneither(_tmp92F,sizeof(char),9))),((_tmp930->f2=0,((_tmp930->f3=((_tmp92E="<path>",_tag_dyneither(_tmp92E,sizeof(char),7))),((_tmp930->f4=(void*)(
# 1278
(_tmp92C=_region_malloc(r,sizeof(*_tmp92C)),((_tmp92C[0]=((_tmp92D.tag=5,((_tmp92D.f1=Cyc_set_cpp,_tmp92D)))),_tmp92C)))),((_tmp930->f5=(
# 1279
(_tmp92B="Indicate which preprocessor to use",_tag_dyneither(_tmp92B,sizeof(char),35))),_tmp930)))))))))))),((_tmp894[47]=(
# 1274
(_tmp936=_region_malloc(r,sizeof(*_tmp936)),((_tmp936->f1=((_tmp935="--inline-checks",_tag_dyneither(_tmp935,sizeof(char),16))),((_tmp936->f2=0,((_tmp936->f3=((_tmp934="",_tag_dyneither(_tmp934,sizeof(char),1))),((_tmp936->f4=(void*)(
# 1275
(_tmp932=_region_malloc(r,sizeof(*_tmp932)),((_tmp932[0]=((_tmp933.tag=0,((_tmp933.f1=Cyc_set_inline_functions,_tmp933)))),_tmp932)))),((_tmp936->f5=(
# 1276
(_tmp931="Inline bounds checks instead of #define",_tag_dyneither(_tmp931,sizeof(char),40))),_tmp936)))))))))))),((_tmp894[46]=(
# 1271
(_tmp93C=_region_malloc(r,sizeof(*_tmp93C)),((_tmp93C->f1=((_tmp93B="--noboundschecks",_tag_dyneither(_tmp93B,sizeof(char),17))),((_tmp93C->f2=0,((_tmp93C->f3=((_tmp93A="",_tag_dyneither(_tmp93A,sizeof(char),1))),((_tmp93C->f4=(void*)(
# 1272
(_tmp938=_region_malloc(r,sizeof(*_tmp938)),((_tmp938[0]=((_tmp939.tag=0,((_tmp939.f1=Cyc_set_noboundschecks,_tmp939)))),_tmp938)))),((_tmp93C->f5=(
# 1273
(_tmp937="Disable bounds checks",_tag_dyneither(_tmp937,sizeof(char),22))),_tmp93C)))))))))))),((_tmp894[45]=(
# 1268
(_tmp942=_region_malloc(r,sizeof(*_tmp942)),((_tmp942->f1=((_tmp941="--nonullchecks",_tag_dyneither(_tmp941,sizeof(char),15))),((_tmp942->f2=0,((_tmp942->f3=((_tmp940="",_tag_dyneither(_tmp940,sizeof(char),1))),((_tmp942->f4=(void*)(
# 1269
(_tmp93E=_region_malloc(r,sizeof(*_tmp93E)),((_tmp93E[0]=((_tmp93F.tag=0,((_tmp93F.f1=Cyc_set_nonullchecks,_tmp93F)))),_tmp93E)))),((_tmp942->f5=(
# 1270
(_tmp93D="Disable null checks",_tag_dyneither(_tmp93D,sizeof(char),20))),_tmp942)))))))))))),((_tmp894[44]=(
# 1265
(_tmp948=_region_malloc(r,sizeof(*_tmp948)),((_tmp948->f1=((_tmp947="--nochecks",_tag_dyneither(_tmp947,sizeof(char),11))),((_tmp948->f2=0,((_tmp948->f3=((_tmp946="",_tag_dyneither(_tmp946,sizeof(char),1))),((_tmp948->f4=(void*)(
# 1266
(_tmp944=_region_malloc(r,sizeof(*_tmp944)),((_tmp944[0]=((_tmp945.tag=0,((_tmp945.f1=Cyc_set_nochecks,_tmp945)))),_tmp944)))),((_tmp948->f5=(
# 1267
(_tmp943="Disable bounds/null checks",_tag_dyneither(_tmp943,sizeof(char),27))),_tmp948)))))))))))),((_tmp894[43]=(
# 1262
(_tmp94E=_region_malloc(r,sizeof(*_tmp94E)),((_tmp94E->f1=((_tmp94D="--nolineno",_tag_dyneither(_tmp94D,sizeof(char),11))),((_tmp94E->f2=0,((_tmp94E->f3=((_tmp94C="",_tag_dyneither(_tmp94C,sizeof(char),1))),((_tmp94E->f4=(void*)(
# 1263
(_tmp94A=_region_malloc(r,sizeof(*_tmp94A)),((_tmp94A[0]=((_tmp94B.tag=0,((_tmp94B.f1=Cyc_clear_lineno,_tmp94B)))),_tmp94A)))),((_tmp94E->f5=(
# 1264
(_tmp949="Don't generate line numbers for debugging (automatic with -pp)",_tag_dyneither(_tmp949,sizeof(char),63))),_tmp94E)))))))))))),((_tmp894[42]=(
# 1259
(_tmp954=_region_malloc(r,sizeof(*_tmp954)),((_tmp954->f1=((_tmp953="-save-c",_tag_dyneither(_tmp953,sizeof(char),8))),((_tmp954->f2=0,((_tmp954->f3=((_tmp952="",_tag_dyneither(_tmp952,sizeof(char),1))),((_tmp954->f4=(void*)(
# 1260
(_tmp950=_region_malloc(r,sizeof(*_tmp950)),((_tmp950[0]=((_tmp951.tag=3,((_tmp951.f1=& Cyc_save_c_r,_tmp951)))),_tmp950)))),((_tmp954->f5=(
# 1261
(_tmp94F="Don't delete temporary C files",_tag_dyneither(_tmp94F,sizeof(char),31))),_tmp954)))))))))))),((_tmp894[41]=(
# 1256
(_tmp95A=_region_malloc(r,sizeof(*_tmp95A)),((_tmp95A->f1=((_tmp959="-save-temps",_tag_dyneither(_tmp959,sizeof(char),12))),((_tmp95A->f2=0,((_tmp95A->f3=((_tmp958="",_tag_dyneither(_tmp958,sizeof(char),1))),((_tmp95A->f4=(void*)(
# 1257
(_tmp956=_region_malloc(r,sizeof(*_tmp956)),((_tmp956[0]=((_tmp957.tag=0,((_tmp957.f1=Cyc_set_save_temps,_tmp957)))),_tmp956)))),((_tmp95A->f5=(
# 1258
(_tmp955="Don't delete temporary files",_tag_dyneither(_tmp955,sizeof(char),29))),_tmp95A)))))))))))),((_tmp894[40]=(
# 1253
(_tmp960=_region_malloc(r,sizeof(*_tmp960)),((_tmp960->f1=((_tmp95F="-c-comp",_tag_dyneither(_tmp95F,sizeof(char),8))),((_tmp960->f2=0,((_tmp960->f3=((_tmp95E=" <compiler>",_tag_dyneither(_tmp95E,sizeof(char),12))),((_tmp960->f4=(void*)(
# 1254
(_tmp95C=_region_malloc(r,sizeof(*_tmp95C)),((_tmp95C[0]=((_tmp95D.tag=5,((_tmp95D.f1=Cyc_set_c_compiler,_tmp95D)))),_tmp95C)))),((_tmp960->f5=(
# 1255
(_tmp95B="Produce C output for the given compiler",_tag_dyneither(_tmp95B,sizeof(char),40))),_tmp960)))))))))))),((_tmp894[39]=(
# 1250
(_tmp966=_region_malloc(r,sizeof(*_tmp966)),((_tmp966->f1=((_tmp965="-un-gcc",_tag_dyneither(_tmp965,sizeof(char),8))),((_tmp966->f2=0,((_tmp966->f3=((_tmp964="",_tag_dyneither(_tmp964,sizeof(char),1))),((_tmp966->f4=(void*)(
# 1251
(_tmp962=_region_malloc(r,sizeof(*_tmp962)),((_tmp962[0]=((_tmp963.tag=0,((_tmp963.f1=Cyc_set_tovc,_tmp963)))),_tmp962)))),((_tmp966->f5=(
# 1252
(_tmp961="Avoid gcc extensions in C output (except for attributes)",_tag_dyneither(_tmp961,sizeof(char),57))),_tmp966)))))))))))),((_tmp894[38]=(
# 1247
(_tmp96C=_region_malloc(r,sizeof(*_tmp96C)),((_tmp96C->f1=((_tmp96B="-no-elim-statement-expressions",_tag_dyneither(_tmp96B,sizeof(char),31))),((_tmp96C->f2=0,((_tmp96C->f3=((_tmp96A="",_tag_dyneither(_tmp96A,sizeof(char),1))),((_tmp96C->f4=(void*)(
# 1248
(_tmp968=_region_malloc(r,sizeof(*_tmp968)),((_tmp968[0]=((_tmp969.tag=4,((_tmp969.f1=& Cyc_elim_se_r,_tmp969)))),_tmp968)))),((_tmp96C->f5=(
# 1249
(_tmp967="Do not avoid statement expressions in C output",_tag_dyneither(_tmp967,sizeof(char),47))),_tmp96C)))))))))))),((_tmp894[37]=(
# 1243
(_tmp972=_region_malloc(r,sizeof(*_tmp972)),((_tmp972->f1=((_tmp971="-elim-statement-expressions",_tag_dyneither(_tmp971,sizeof(char),28))),((_tmp972->f2=0,((_tmp972->f3=((_tmp970="",_tag_dyneither(_tmp970,sizeof(char),1))),((_tmp972->f4=(void*)(
# 1244
(_tmp96E=_region_malloc(r,sizeof(*_tmp96E)),((_tmp96E[0]=((_tmp96F.tag=3,((_tmp96F.f1=& Cyc_elim_se_r,_tmp96F)))),_tmp96E)))),((_tmp972->f5=(
# 1246
(_tmp96D="Avoid statement expressions in C output (implies --inline-checks)",_tag_dyneither(_tmp96D,sizeof(char),66))),_tmp972)))))))))))),((_tmp894[36]=(
# 1240
(_tmp978=_region_malloc(r,sizeof(*_tmp978)),((_tmp978->f1=((_tmp977="-up",_tag_dyneither(_tmp977,sizeof(char),4))),((_tmp978->f2=0,((_tmp978->f3=((_tmp976="",_tag_dyneither(_tmp976,sizeof(char),1))),((_tmp978->f4=(void*)(
# 1241
(_tmp974=_region_malloc(r,sizeof(*_tmp974)),((_tmp974[0]=((_tmp975.tag=4,((_tmp975.f1=& Cyc_pp_r,_tmp975)))),_tmp974)))),((_tmp978->f5=(
# 1242
(_tmp973="Ugly print the C code that Cyclone generates (default)",_tag_dyneither(_tmp973,sizeof(char),55))),_tmp978)))))))))))),((_tmp894[35]=(
# 1237
(_tmp97E=_region_malloc(r,sizeof(*_tmp97E)),((_tmp97E->f1=((_tmp97D="-pp",_tag_dyneither(_tmp97D,sizeof(char),4))),((_tmp97E->f2=0,((_tmp97E->f3=((_tmp97C="",_tag_dyneither(_tmp97C,sizeof(char),1))),((_tmp97E->f4=(void*)(
# 1238
(_tmp97A=_region_malloc(r,sizeof(*_tmp97A)),((_tmp97A[0]=((_tmp97B.tag=3,((_tmp97B.f1=& Cyc_pp_r,_tmp97B)))),_tmp97A)))),((_tmp97E->f5=(
# 1239
(_tmp979="Pretty print the C code that Cyclone generates",_tag_dyneither(_tmp979,sizeof(char),47))),_tmp97E)))))))))))),((_tmp894[34]=(
# 1234
(_tmp984=_region_malloc(r,sizeof(*_tmp984)),((_tmp984->f1=((_tmp983="-ic",_tag_dyneither(_tmp983,sizeof(char),4))),((_tmp984->f2=0,((_tmp984->f3=((_tmp982="",_tag_dyneither(_tmp982,sizeof(char),1))),((_tmp984->f4=(void*)(
# 1235
(_tmp980=_region_malloc(r,sizeof(*_tmp980)),((_tmp980[0]=((_tmp981.tag=3,((_tmp981.f1=& Cyc_ic_r,_tmp981)))),_tmp980)))),((_tmp984->f5=(
# 1236
(_tmp97F="Activate the link-checker",_tag_dyneither(_tmp97F,sizeof(char),26))),_tmp984)))))))))))),((_tmp894[33]=(
# 1231
(_tmp98A=_region_malloc(r,sizeof(*_tmp98A)),((_tmp98A->f1=((_tmp989="-stopafter-toc",_tag_dyneither(_tmp989,sizeof(char),15))),((_tmp98A->f2=0,((_tmp98A->f3=((_tmp988="",_tag_dyneither(_tmp988,sizeof(char),1))),((_tmp98A->f4=(void*)(
# 1232
(_tmp986=_region_malloc(r,sizeof(*_tmp986)),((_tmp986[0]=((_tmp987.tag=3,((_tmp987.f1=& Cyc_toc_r,_tmp987)))),_tmp986)))),((_tmp98A->f5=(
# 1233
(_tmp985="Stop after translation to C",_tag_dyneither(_tmp985,sizeof(char),28))),_tmp98A)))))))))))),((_tmp894[32]=(
# 1228
(_tmp990=_region_malloc(r,sizeof(*_tmp990)),((_tmp990->f1=((_tmp98F="-stopafter-cf",_tag_dyneither(_tmp98F,sizeof(char),14))),((_tmp990->f2=0,((_tmp990->f3=((_tmp98E="",_tag_dyneither(_tmp98E,sizeof(char),1))),((_tmp990->f4=(void*)(
# 1229
(_tmp98C=_region_malloc(r,sizeof(*_tmp98C)),((_tmp98C[0]=((_tmp98D.tag=3,((_tmp98D.f1=& Cyc_cf_r,_tmp98D)))),_tmp98C)))),((_tmp990->f5=(
# 1230
(_tmp98B="Stop after control-flow checking",_tag_dyneither(_tmp98B,sizeof(char),33))),_tmp990)))))))))))),((_tmp894[31]=(
# 1225
(_tmp996=_region_malloc(r,sizeof(*_tmp996)),((_tmp996->f1=((_tmp995="-noprint",_tag_dyneither(_tmp995,sizeof(char),9))),((_tmp996->f2=0,((_tmp996->f3=((_tmp994="",_tag_dyneither(_tmp994,sizeof(char),1))),((_tmp996->f4=(void*)(
# 1226
(_tmp992=_region_malloc(r,sizeof(*_tmp992)),((_tmp992[0]=((_tmp993.tag=3,((_tmp993.f1=& Cyc_noprint_r,_tmp993)))),_tmp992)))),((_tmp996->f5=(
# 1227
(_tmp991="Do not print output (when stopping early)",_tag_dyneither(_tmp991,sizeof(char),42))),_tmp996)))))))))))),((_tmp894[30]=(
# 1222
(_tmp99C=_region_malloc(r,sizeof(*_tmp99C)),((_tmp99C->f1=((_tmp99B="-stopafter-tc",_tag_dyneither(_tmp99B,sizeof(char),14))),((_tmp99C->f2=0,((_tmp99C->f3=((_tmp99A="",_tag_dyneither(_tmp99A,sizeof(char),1))),((_tmp99C->f4=(void*)(
# 1223
(_tmp998=_region_malloc(r,sizeof(*_tmp998)),((_tmp998[0]=((_tmp999.tag=3,((_tmp999.f1=& Cyc_tc_r,_tmp999)))),_tmp998)))),((_tmp99C->f5=(
# 1224
(_tmp997="Stop after type checking",_tag_dyneither(_tmp997,sizeof(char),25))),_tmp99C)))))))))))),((_tmp894[29]=(
# 1219
(_tmp9A2=_region_malloc(r,sizeof(*_tmp9A2)),((_tmp9A2->f1=((_tmp9A1="-stopafter-parse",_tag_dyneither(_tmp9A1,sizeof(char),17))),((_tmp9A2->f2=0,((_tmp9A2->f3=((_tmp9A0="",_tag_dyneither(_tmp9A0,sizeof(char),1))),((_tmp9A2->f4=(void*)(
# 1220
(_tmp99E=_region_malloc(r,sizeof(*_tmp99E)),((_tmp99E[0]=((_tmp99F.tag=3,((_tmp99F.f1=& Cyc_parseonly_r,_tmp99F)))),_tmp99E)))),((_tmp9A2->f5=(
# 1221
(_tmp99D="Stop after parsing",_tag_dyneither(_tmp99D,sizeof(char),19))),_tmp9A2)))))))))))),((_tmp894[28]=(
# 1216
(_tmp9A8=_region_malloc(r,sizeof(*_tmp9A8)),((_tmp9A8->f1=((_tmp9A7="-E",_tag_dyneither(_tmp9A7,sizeof(char),3))),((_tmp9A8->f2=0,((_tmp9A8->f3=((_tmp9A6="",_tag_dyneither(_tmp9A6,sizeof(char),1))),((_tmp9A8->f4=(void*)(
# 1217
(_tmp9A4=_region_malloc(r,sizeof(*_tmp9A4)),((_tmp9A4[0]=((_tmp9A5.tag=3,((_tmp9A5.f1=& Cyc_stop_after_cpp_r,_tmp9A5)))),_tmp9A4)))),((_tmp9A8->f5=(
# 1218
(_tmp9A3="Stop after preprocessing",_tag_dyneither(_tmp9A3,sizeof(char),25))),_tmp9A8)))))))))))),((_tmp894[27]=(
# 1213
(_tmp9AE=_region_malloc(r,sizeof(*_tmp9AE)),((_tmp9AE->f1=((_tmp9AD="-Wall",_tag_dyneither(_tmp9AD,sizeof(char),6))),((_tmp9AE->f2=0,((_tmp9AE->f3=((_tmp9AC="",_tag_dyneither(_tmp9AC,sizeof(char),1))),((_tmp9AE->f4=(void*)(
# 1214
(_tmp9AA=_region_malloc(r,sizeof(*_tmp9AA)),((_tmp9AA[0]=((_tmp9AB.tag=0,((_tmp9AB.f1=Cyc_set_all_warnings,_tmp9AB)))),_tmp9AA)))),((_tmp9AE->f5=(
# 1215
(_tmp9A9="Turn on all warnings",_tag_dyneither(_tmp9A9,sizeof(char),21))),_tmp9AE)))))))))))),((_tmp894[26]=(
# 1210
(_tmp9B4=_region_malloc(r,sizeof(*_tmp9B4)),((_tmp9B4->f1=((_tmp9B3="-Wlose-unique",_tag_dyneither(_tmp9B3,sizeof(char),14))),((_tmp9B4->f2=0,((_tmp9B4->f3=((_tmp9B2="",_tag_dyneither(_tmp9B2,sizeof(char),1))),((_tmp9B4->f4=(void*)(
# 1211
(_tmp9B0=_region_malloc(r,sizeof(*_tmp9B0)),((_tmp9B0[0]=((_tmp9B1.tag=3,((_tmp9B1.f1=& Cyc_NewControlFlow_warn_lose_unique,_tmp9B1)))),_tmp9B0)))),((_tmp9B4->f5=(
# 1212
(_tmp9AF="Try to warn when a unique pointer might get lost",_tag_dyneither(_tmp9AF,sizeof(char),49))),_tmp9B4)))))))))))),((_tmp894[25]=(
# 1207
(_tmp9BA=_region_malloc(r,sizeof(*_tmp9BA)),((_tmp9BA->f1=((_tmp9B9="-b",_tag_dyneither(_tmp9B9,sizeof(char),3))),((_tmp9BA->f2=0,((_tmp9BA->f3=((_tmp9B8="<arch>",_tag_dyneither(_tmp9B8,sizeof(char),7))),((_tmp9BA->f4=(void*)(
# 1208
(_tmp9B6=_region_malloc(r,sizeof(*_tmp9B6)),((_tmp9B6[0]=((_tmp9B7.tag=5,((_tmp9B7.f1=Cyc_set_target_arch,_tmp9B7)))),_tmp9B6)))),((_tmp9BA->f5=(
# 1209
(_tmp9B5="Set target architecture; implies -stopafter-toc",_tag_dyneither(_tmp9B5,sizeof(char),48))),_tmp9BA)))))))))))),((_tmp894[24]=(
# 1204
(_tmp9C0=_region_malloc(r,sizeof(*_tmp9C0)),((_tmp9C0->f1=((_tmp9BF="-MT",_tag_dyneither(_tmp9BF,sizeof(char),4))),((_tmp9C0->f2=0,((_tmp9C0->f3=((_tmp9BE=" <target>",_tag_dyneither(_tmp9BE,sizeof(char),10))),((_tmp9C0->f4=(void*)(
# 1205
(_tmp9BC=_region_malloc(r,sizeof(*_tmp9BC)),((_tmp9BC[0]=((_tmp9BD.tag=5,((_tmp9BD.f1=Cyc_set_dependencies_target,_tmp9BD)))),_tmp9BC)))),((_tmp9C0->f5=(
# 1206
(_tmp9BB="Give target for dependencies",_tag_dyneither(_tmp9BB,sizeof(char),29))),_tmp9C0)))))))))))),((_tmp894[23]=(
# 1200
(_tmp9C6=_region_malloc(r,sizeof(*_tmp9C6)),((_tmp9C6->f1=((_tmp9C5="-MG",_tag_dyneither(_tmp9C5,sizeof(char),4))),((_tmp9C6->f2=0,((_tmp9C6->f3=((_tmp9C4="",_tag_dyneither(_tmp9C4,sizeof(char),1))),((_tmp9C6->f4=(void*)(
# 1201
(_tmp9C2=_region_malloc(r,sizeof(*_tmp9C2)),((_tmp9C2[0]=((_tmp9C3.tag=1,((_tmp9C3.f1=Cyc_add_cpparg,_tmp9C3)))),_tmp9C2)))),((_tmp9C6->f5=(
# 1203
(_tmp9C1="When producing dependencies assume that missing files are generated",_tag_dyneither(_tmp9C1,sizeof(char),68))),_tmp9C6)))))))))))),((_tmp894[22]=(
# 1197
(_tmp9CC=_region_malloc(r,sizeof(*_tmp9CC)),((_tmp9CC->f1=((_tmp9CB="-M",_tag_dyneither(_tmp9CB,sizeof(char),3))),((_tmp9CC->f2=0,((_tmp9CC->f3=((_tmp9CA="",_tag_dyneither(_tmp9CA,sizeof(char),1))),((_tmp9CC->f4=(void*)(
# 1198
(_tmp9C8=_region_malloc(r,sizeof(*_tmp9C8)),((_tmp9C8[0]=((_tmp9C9.tag=0,((_tmp9C9.f1=Cyc_set_produce_dependencies,_tmp9C9)))),_tmp9C8)))),((_tmp9CC->f5=(
# 1199
(_tmp9C7="Produce dependencies",_tag_dyneither(_tmp9C7,sizeof(char),21))),_tmp9CC)))))))))))),((_tmp894[21]=(
# 1194
(_tmp9D2=_region_malloc(r,sizeof(*_tmp9D2)),((_tmp9D2->f1=((_tmp9D1="-S",_tag_dyneither(_tmp9D1,sizeof(char),3))),((_tmp9D2->f2=0,((_tmp9D2->f3=((_tmp9D0="",_tag_dyneither(_tmp9D0,sizeof(char),1))),((_tmp9D2->f4=(void*)(
# 1195
(_tmp9CE=_region_malloc(r,sizeof(*_tmp9CE)),((_tmp9CE[0]=((_tmp9CF.tag=0,((_tmp9CF.f1=Cyc_set_stop_after_asmfile,_tmp9CF)))),_tmp9CE)))),((_tmp9D2->f5=(
# 1196
(_tmp9CD="Stop after producing assembly code",_tag_dyneither(_tmp9CD,sizeof(char),35))),_tmp9D2)))))))))))),((_tmp894[20]=(
# 1191
(_tmp9D8=_region_malloc(r,sizeof(*_tmp9D8)),((_tmp9D8->f1=((_tmp9D7="-pa",_tag_dyneither(_tmp9D7,sizeof(char),4))),((_tmp9D8->f2=0,((_tmp9D8->f3=((_tmp9D6="",_tag_dyneither(_tmp9D6,sizeof(char),1))),((_tmp9D8->f4=(void*)(
# 1192
(_tmp9D4=_region_malloc(r,sizeof(*_tmp9D4)),((_tmp9D4[0]=((_tmp9D5.tag=0,((_tmp9D5.f1=Cyc_set_pa,_tmp9D5)))),_tmp9D4)))),((_tmp9D8->f5=(
# 1193
(_tmp9D3="Compile for profiling with aprof",_tag_dyneither(_tmp9D3,sizeof(char),33))),_tmp9D8)))))))))))),((_tmp894[19]=(
# 1188
(_tmp9DE=_region_malloc(r,sizeof(*_tmp9DE)),((_tmp9DE->f1=((_tmp9DD="-pg",_tag_dyneither(_tmp9DD,sizeof(char),4))),((_tmp9DE->f2=0,((_tmp9DE->f3=((_tmp9DC="",_tag_dyneither(_tmp9DC,sizeof(char),1))),((_tmp9DE->f4=(void*)(
# 1189
(_tmp9DA=_region_malloc(r,sizeof(*_tmp9DA)),((_tmp9DA[0]=((_tmp9DB.tag=0,((_tmp9DB.f1=Cyc_set_pg,_tmp9DB)))),_tmp9DA)))),((_tmp9DE->f5=(
# 1190
(_tmp9D9="Compile for profiling with gprof",_tag_dyneither(_tmp9D9,sizeof(char),33))),_tmp9DE)))))))))))),((_tmp894[18]=(
# 1185
(_tmp9E4=_region_malloc(r,sizeof(*_tmp9E4)),((_tmp9E4->f1=((_tmp9E3="-p",_tag_dyneither(_tmp9E3,sizeof(char),3))),((_tmp9E4->f2=0,((_tmp9E4->f3=((_tmp9E2="",_tag_dyneither(_tmp9E2,sizeof(char),1))),((_tmp9E4->f4=(void*)(
# 1186
(_tmp9E0=_region_malloc(r,sizeof(*_tmp9E0)),((_tmp9E0[0]=((_tmp9E1.tag=1,((_tmp9E1.f1=Cyc_add_ccarg,_tmp9E1)))),_tmp9E0)))),((_tmp9E4->f5=(
# 1187
(_tmp9DF="Compile for profiling with prof",_tag_dyneither(_tmp9DF,sizeof(char),32))),_tmp9E4)))))))))))),((_tmp894[17]=(
# 1182
(_tmp9EA=_region_malloc(r,sizeof(*_tmp9EA)),((_tmp9EA->f1=((_tmp9E9="-g",_tag_dyneither(_tmp9E9,sizeof(char),3))),((_tmp9EA->f2=0,((_tmp9EA->f3=((_tmp9E8="",_tag_dyneither(_tmp9E8,sizeof(char),1))),((_tmp9EA->f4=(void*)(
# 1183
(_tmp9E6=_region_malloc(r,sizeof(*_tmp9E6)),((_tmp9E6[0]=((_tmp9E7.tag=1,((_tmp9E7.f1=Cyc_add_ccarg,_tmp9E7)))),_tmp9E6)))),((_tmp9EA->f5=(
# 1184
(_tmp9E5="Compile for debugging",_tag_dyneither(_tmp9E5,sizeof(char),22))),_tmp9EA)))))))))))),((_tmp894[16]=(
# 1179
(_tmp9F0=_region_malloc(r,sizeof(*_tmp9F0)),((_tmp9F0->f1=((_tmp9EF="-fomit-frame-pointer",_tag_dyneither(_tmp9EF,sizeof(char),21))),((_tmp9F0->f2=0,((_tmp9F0->f3=((_tmp9EE="",_tag_dyneither(_tmp9EE,sizeof(char),1))),((_tmp9F0->f4=(void*)(
# 1180
(_tmp9EC=_region_malloc(r,sizeof(*_tmp9EC)),((_tmp9EC[0]=((_tmp9ED.tag=1,((_tmp9ED.f1=Cyc_add_ccarg,_tmp9ED)))),_tmp9EC)))),((_tmp9F0->f5=(
# 1181
(_tmp9EB="Omit frame pointer",_tag_dyneither(_tmp9EB,sizeof(char),19))),_tmp9F0)))))))))))),((_tmp894[15]=(
# 1176
(_tmp9F6=_region_malloc(r,sizeof(*_tmp9F6)),((_tmp9F6->f1=((_tmp9F5="-O3",_tag_dyneither(_tmp9F5,sizeof(char),4))),((_tmp9F6->f2=0,((_tmp9F6->f3=((_tmp9F4="",_tag_dyneither(_tmp9F4,sizeof(char),1))),((_tmp9F6->f4=(void*)(
# 1177
(_tmp9F2=_region_malloc(r,sizeof(*_tmp9F2)),((_tmp9F2[0]=((_tmp9F3.tag=1,((_tmp9F3.f1=Cyc_add_ccarg,_tmp9F3)))),_tmp9F2)))),((_tmp9F6->f5=(
# 1178
(_tmp9F1="Even more optimization",_tag_dyneither(_tmp9F1,sizeof(char),23))),_tmp9F6)))))))))))),((_tmp894[14]=(
# 1173
(_tmp9FC=_region_malloc(r,sizeof(*_tmp9FC)),((_tmp9FC->f1=((_tmp9FB="-O2",_tag_dyneither(_tmp9FB,sizeof(char),4))),((_tmp9FC->f2=0,((_tmp9FC->f3=((_tmp9FA="",_tag_dyneither(_tmp9FA,sizeof(char),1))),((_tmp9FC->f4=(void*)(
# 1174
(_tmp9F8=_region_malloc(r,sizeof(*_tmp9F8)),((_tmp9F8[0]=((_tmp9F9.tag=1,((_tmp9F9.f1=Cyc_add_ccarg,_tmp9F9)))),_tmp9F8)))),((_tmp9FC->f5=(
# 1175
(_tmp9F7="A higher level of optimization",_tag_dyneither(_tmp9F7,sizeof(char),31))),_tmp9FC)))))))))))),((_tmp894[13]=(
# 1170
(_tmpA02=_region_malloc(r,sizeof(*_tmpA02)),((_tmpA02->f1=((_tmpA01="-O",_tag_dyneither(_tmpA01,sizeof(char),3))),((_tmpA02->f2=0,((_tmpA02->f3=((_tmpA00="",_tag_dyneither(_tmpA00,sizeof(char),1))),((_tmpA02->f4=(void*)(
# 1171
(_tmp9FE=_region_malloc(r,sizeof(*_tmp9FE)),((_tmp9FE[0]=((_tmp9FF.tag=1,((_tmp9FF.f1=Cyc_add_ccarg,_tmp9FF)))),_tmp9FE)))),((_tmpA02->f5=(
# 1172
(_tmp9FD="Optimize",_tag_dyneither(_tmp9FD,sizeof(char),9))),_tmpA02)))))))))))),((_tmp894[12]=(
# 1167
(_tmpA08=_region_malloc(r,sizeof(*_tmpA08)),((_tmpA08->f1=((_tmpA07="-O0",_tag_dyneither(_tmpA07,sizeof(char),4))),((_tmpA08->f2=0,((_tmpA08->f3=((_tmpA06="",_tag_dyneither(_tmpA06,sizeof(char),1))),((_tmpA08->f4=(void*)(
# 1168
(_tmpA04=_region_malloc(r,sizeof(*_tmpA04)),((_tmpA04[0]=((_tmpA05.tag=1,((_tmpA05.f1=Cyc_add_ccarg,_tmpA05)))),_tmpA04)))),((_tmpA08->f5=(
# 1169
(_tmpA03="Don't optimize",_tag_dyneither(_tmpA03,sizeof(char),15))),_tmpA08)))))))))))),((_tmp894[11]=(
# 1164
(_tmpA0E=_region_malloc(r,sizeof(*_tmpA0E)),((_tmpA0E->f1=((_tmpA0D="-s",_tag_dyneither(_tmpA0D,sizeof(char),3))),((_tmpA0E->f2=0,((_tmpA0E->f3=((_tmpA0C="",_tag_dyneither(_tmpA0C,sizeof(char),1))),((_tmpA0E->f4=(void*)(
# 1165
(_tmpA0A=_region_malloc(r,sizeof(*_tmpA0A)),((_tmpA0A[0]=((_tmpA0B.tag=1,((_tmpA0B.f1=Cyc_add_ccarg,_tmpA0B)))),_tmpA0A)))),((_tmpA0E->f5=(
# 1166
(_tmpA09="Remove all symbol table and relocation info from executable",_tag_dyneither(_tmpA09,sizeof(char),60))),_tmpA0E)))))))))))),((_tmp894[10]=(
# 1161
(_tmpA14=_region_malloc(r,sizeof(*_tmpA14)),((_tmpA14->f1=((_tmpA13="-x",_tag_dyneither(_tmpA13,sizeof(char),3))),((_tmpA14->f2=0,((_tmpA14->f3=((_tmpA12=" <language>",_tag_dyneither(_tmpA12,sizeof(char),12))),((_tmpA14->f4=(void*)(
# 1162
(_tmpA10=_region_malloc(r,sizeof(*_tmpA10)),((_tmpA10[0]=((_tmpA11.tag=5,((_tmpA11.f1=Cyc_set_inputtype,_tmpA11)))),_tmpA10)))),((_tmpA14->f5=(
# 1163
(_tmpA0F="Specify <language> for the following input files",_tag_dyneither(_tmpA0F,sizeof(char),49))),_tmpA14)))))))))))),((_tmp894[9]=(
# 1158
(_tmpA1A=_region_malloc(r,sizeof(*_tmpA1A)),((_tmpA1A->f1=((_tmpA19="-c",_tag_dyneither(_tmpA19,sizeof(char),3))),((_tmpA1A->f2=0,((_tmpA1A->f3=((_tmpA18="",_tag_dyneither(_tmpA18,sizeof(char),1))),((_tmpA1A->f4=(void*)(
# 1159
(_tmpA16=_region_malloc(r,sizeof(*_tmpA16)),((_tmpA16[0]=((_tmpA17.tag=0,((_tmpA17.f1=Cyc_set_stop_after_objectfile,_tmpA17)))),_tmpA16)))),((_tmpA1A->f5=(
# 1160
(_tmpA15="Produce an object file instead of an executable; do not link",_tag_dyneither(_tmpA15,sizeof(char),61))),_tmpA1A)))))))))))),((_tmp894[8]=(
# 1155
(_tmpA20=_region_malloc(r,sizeof(*_tmpA20)),((_tmpA20->f1=((_tmpA1F="-m",_tag_dyneither(_tmpA1F,sizeof(char),3))),((_tmpA20->f2=1,((_tmpA20->f3=((_tmpA1E="<option>",_tag_dyneither(_tmpA1E,sizeof(char),9))),((_tmpA20->f4=(void*)(
# 1156
(_tmpA1C=_region_malloc(r,sizeof(*_tmpA1C)),((_tmpA1C[0]=((_tmpA1D.tag=1,((_tmpA1D.f1=Cyc_add_marg,_tmpA1D)))),_tmpA1C)))),((_tmpA20->f5=(
# 1157
(_tmpA1B="GCC specific: pass machine-dependent flag on to gcc",_tag_dyneither(_tmpA1B,sizeof(char),52))),_tmpA20)))))))))))),((_tmp894[7]=(
# 1152
(_tmpA26=_region_malloc(r,sizeof(*_tmpA26)),((_tmpA26->f1=((_tmpA25="-l",_tag_dyneither(_tmpA25,sizeof(char),3))),((_tmpA26->f2=1,((_tmpA26->f3=((_tmpA24="<libname>",_tag_dyneither(_tmpA24,sizeof(char),10))),((_tmpA26->f4=(void*)(
# 1153
(_tmpA22=_region_malloc(r,sizeof(*_tmpA22)),((_tmpA22[0]=((_tmpA23.tag=1,((_tmpA23.f1=Cyc_add_libarg,_tmpA23)))),_tmpA22)))),((_tmpA26->f5=(
# 1154
(_tmpA21="Library file",_tag_dyneither(_tmpA21,sizeof(char),13))),_tmpA26)))))))))))),((_tmp894[6]=(
# 1149
(_tmpA2C=_region_malloc(r,sizeof(*_tmpA2C)),((_tmpA2C->f1=((_tmpA2B="-L",_tag_dyneither(_tmpA2B,sizeof(char),3))),((_tmpA2C->f2=1,((_tmpA2C->f3=((_tmpA2A="<dir>",_tag_dyneither(_tmpA2A,sizeof(char),6))),((_tmpA2C->f4=(void*)(
# 1150
(_tmpA28=_region_malloc(r,sizeof(*_tmpA28)),((_tmpA28[0]=((_tmpA29.tag=1,((_tmpA29.f1=Cyc_add_ccarg,_tmpA29)))),_tmpA28)))),((_tmpA2C->f5=(
# 1151
(_tmpA27="Add to the list of directories for -l",_tag_dyneither(_tmpA27,sizeof(char),38))),_tmpA2C)))))))))))),((_tmp894[5]=(
# 1146
(_tmpA32=_region_malloc(r,sizeof(*_tmpA32)),((_tmpA32->f1=((_tmpA31="-I",_tag_dyneither(_tmpA31,sizeof(char),3))),((_tmpA32->f2=1,((_tmpA32->f3=((_tmpA30="<dir>",_tag_dyneither(_tmpA30,sizeof(char),6))),((_tmpA32->f4=(void*)(
# 1147
(_tmpA2E=_region_malloc(r,sizeof(*_tmpA2E)),((_tmpA2E[0]=((_tmpA2F.tag=1,((_tmpA2F.f1=Cyc_add_cpparg,_tmpA2F)))),_tmpA2E)))),((_tmpA32->f5=(
# 1148
(_tmpA2D="Add to the list of directories to search for include files",_tag_dyneither(_tmpA2D,sizeof(char),59))),_tmpA32)))))))))))),((_tmp894[4]=(
# 1143
(_tmpA38=_region_malloc(r,sizeof(*_tmpA38)),((_tmpA38->f1=((_tmpA37="-B",_tag_dyneither(_tmpA37,sizeof(char),3))),((_tmpA38->f2=1,((_tmpA38->f3=((_tmpA36="<file>",_tag_dyneither(_tmpA36,sizeof(char),7))),((_tmpA38->f4=(void*)(
# 1144
(_tmpA34=_region_malloc(r,sizeof(*_tmpA34)),((_tmpA34[0]=((_tmpA35.tag=1,((_tmpA35.f1=Cyc_add_cyclone_exec_path,_tmpA35)))),_tmpA34)))),((_tmpA38->f5=(
# 1145
(_tmpA33="Add to the list of directories to search for compiler files",_tag_dyneither(_tmpA33,sizeof(char),60))),_tmpA38)))))))))))),((_tmp894[3]=(
# 1140
(_tmpA3E=_region_malloc(r,sizeof(*_tmpA3E)),((_tmpA3E->f1=((_tmpA3D="-D",_tag_dyneither(_tmpA3D,sizeof(char),3))),((_tmpA3E->f2=1,((_tmpA3E->f3=((_tmpA3C="<name>[=<value>]",_tag_dyneither(_tmpA3C,sizeof(char),17))),((_tmpA3E->f4=(void*)(
# 1141
(_tmpA3A=_region_malloc(r,sizeof(*_tmpA3A)),((_tmpA3A[0]=((_tmpA3B.tag=1,((_tmpA3B.f1=Cyc_add_cpparg,_tmpA3B)))),_tmpA3A)))),((_tmpA3E->f5=(
# 1142
(_tmpA39="Pass definition to preprocessor",_tag_dyneither(_tmpA39,sizeof(char),32))),_tmpA3E)))))))))))),((_tmp894[2]=(
# 1137
(_tmpA44=_region_malloc(r,sizeof(*_tmpA44)),((_tmpA44->f1=((_tmpA43="-o",_tag_dyneither(_tmpA43,sizeof(char),3))),((_tmpA44->f2=0,((_tmpA44->f3=((_tmpA42=" <file>",_tag_dyneither(_tmpA42,sizeof(char),8))),((_tmpA44->f4=(void*)(
# 1138
(_tmpA40=_region_malloc(r,sizeof(*_tmpA40)),((_tmpA40[0]=((_tmpA41.tag=5,((_tmpA41.f1=Cyc_set_output_file,_tmpA41)))),_tmpA40)))),((_tmpA44->f5=(
# 1139
(_tmpA3F="Set the output file name to <file>",_tag_dyneither(_tmpA3F,sizeof(char),35))),_tmpA44)))))))))))),((_tmp894[1]=(
# 1134
(_tmpA4A=_region_malloc(r,sizeof(*_tmpA4A)),((_tmpA4A->f1=((_tmpA49="--version",_tag_dyneither(_tmpA49,sizeof(char),10))),((_tmpA4A->f2=0,((_tmpA4A->f3=((_tmpA48="",_tag_dyneither(_tmpA48,sizeof(char),1))),((_tmpA4A->f4=(void*)(
# 1135
(_tmpA46=_region_malloc(r,sizeof(*_tmpA46)),((_tmpA46[0]=((_tmpA47.tag=0,((_tmpA47.f1=Cyc_print_version,_tmpA47)))),_tmpA46)))),((_tmpA4A->f5=(
# 1136
(_tmpA45="Print version information and exit",_tag_dyneither(_tmpA45,sizeof(char),35))),_tmpA4A)))))))))))),((_tmp894[0]=(
# 1131
(_tmpA50=_region_malloc(r,sizeof(*_tmpA50)),((_tmpA50->f1=((_tmpA4F="-v",_tag_dyneither(_tmpA4F,sizeof(char),3))),((_tmpA50->f2=0,((_tmpA50->f3=((_tmpA4E="",_tag_dyneither(_tmpA4E,sizeof(char),1))),((_tmpA50->f4=(void*)(
# 1132
(_tmpA4C=_region_malloc(r,sizeof(*_tmpA4C)),((_tmpA4C[0]=((_tmpA4D.tag=3,((_tmpA4D.f1=& Cyc_v_r,_tmpA4D)))),_tmpA4C)))),((_tmpA50->f5=(
# 1133
(_tmpA4B="Print compilation stages verbosely",_tag_dyneither(_tmpA4B,sizeof(char),35))),_tmpA50)))))))))))),((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(_tmp894,sizeof(struct _tuple25*),74)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
# 1357
const char*_tmpA51;struct _dyneither_ptr optstring=(_tmpA51="Options:",_tag_dyneither(_tmpA51,sizeof(char),9));
# 1358
Cyc_Arg_current=0;
# 1359
Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp166);
# 1362
if((char*)Cyc_target_arch.curr == (char*)(_tag_dyneither(0,0,0)).curr)Cyc_target_arch=(struct _dyneither_ptr)Cstring_to_string(Carch);{
# 1370
char*_tmpA52;struct _dyneither_ptr cyclone_exec_prefix=(_tmpA52=getenv((const char*)"CYCLONE_EXEC_PREFIX"),_tag_dyneither(_tmpA52,sizeof(char),_get_zero_arr_size_char((void*)_tmpA52,1)));
# 1371
if((char*)cyclone_exec_prefix.curr != (char*)(_tag_dyneither(0,0,0)).curr){
# 1372
struct _dyneither_ptr*_tmpA55;struct Cyc_List_List*_tmpA54;Cyc_cyclone_exec_path=((_tmpA54=_cycalloc(sizeof(*_tmpA54)),((_tmpA54->hd=((_tmpA55=_cycalloc(sizeof(*_tmpA55)),((_tmpA55[0]=cyclone_exec_prefix,_tmpA55)))),((_tmpA54->tl=Cyc_cyclone_exec_path,_tmpA54))))));}
# 1373
def_lib_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_lib_path);
# 1374
if(Cyc_strlen((struct _dyneither_ptr)def_lib_path)> 0){
# 1375
struct _dyneither_ptr*_tmpA5B;const char*_tmpA5A;struct Cyc_List_List*_tmpA59;Cyc_cyclone_exec_path=(
# 1376
(_tmpA59=_cycalloc(sizeof(*_tmpA59)),((_tmpA59->hd=((_tmpA5B=_cycalloc(sizeof(*_tmpA5B)),((_tmpA5B[0]=(struct _dyneither_ptr)Cyc_Filename_concat(def_lib_path,((_tmpA5A="cyc-lib",_tag_dyneither(_tmpA5A,sizeof(char),8)))),_tmpA5B)))),((_tmpA59->tl=Cyc_cyclone_exec_path,_tmpA59))))));}
# 1378
Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_cyclone_exec_path);
# 1379
cyclone_arch_path=Cyc_also_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);{
# 1386
const char*_tmpA5C;struct _dyneither_ptr _tmp16D=Cyc_do_find(cyclone_arch_path,((_tmpA5C="cycspecs",_tag_dyneither(_tmpA5C,sizeof(char),9))));
# 1387
if(Cyc_v_r){const char*_tmpA60;void*_tmpA5F[1];struct Cyc_String_pa_PrintArg_struct _tmpA5E;(_tmpA5E.tag=0,((_tmpA5E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp16D),((_tmpA5F[0]=& _tmpA5E,Cyc_fprintf(Cyc_stderr,((_tmpA60="Reading from specs file %s\n",_tag_dyneither(_tmpA60,sizeof(char),28))),_tag_dyneither(_tmpA5F,sizeof(void*),1)))))));}{
# 1388
struct Cyc_List_List*_tmp171=Cyc_read_specs(_tmp16D);
# 1389
struct _dyneither_ptr _tmp172=_tag_dyneither(0,0,0);
# 1390
{struct _handler_cons _tmp173;_push_handler(& _tmp173);{int _tmp175=0;if(setjmp(_tmp173.handler))_tmp175=1;if(!_tmp175){
# 1391
{struct _dyneither_ptr _tmp176=*((struct _dyneither_ptr*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_assoc_cmp)(Cyc_strptrcmp,_tmp171,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"cyclone",sizeof(char),8));
# 1392
_tmp172=Cyc_split_specs(_tmp176);}
# 1391
;_pop_handler();}else{void*_tmp174=(void*)_exn_thrown;void*_tmp179=_tmp174;_LL35: {struct Cyc_Core_Not_found_exn_struct*_tmp17A=(struct Cyc_Core_Not_found_exn_struct*)_tmp179;if(_tmp17A->tag != Cyc_Core_Not_found)goto _LL37;}_LL36:
# 1394
 goto _LL34;_LL37:;_LL38:(void)_throw(_tmp179);_LL34:;}};}
# 1396
if((struct _dyneither_ptr*)_tmp172.curr != (struct _dyneither_ptr*)(_tag_dyneither(0,0,0)).curr){
# 1397
Cyc_Arg_current=0;
# 1398
Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp172);}
# 1401
Cyc_Arg_current=0;
# 1402
Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp167);{
# 1403
const char*_tmpA61;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_cpp,((_tmpA61="",_tag_dyneither(_tmpA61,sizeof(char),1))))== 0){
# 1419 "cyclone.cyc"
const char*_tmp17C=Cyc_produce_dependencies?"":(const char*)" -E";
# 1420
const char*_tmpA69;void*_tmpA68[3];struct Cyc_String_pa_PrintArg_struct _tmpA67;const char*_tmpA66;struct Cyc_String_pa_PrintArg_struct _tmpA65;struct Cyc_String_pa_PrintArg_struct _tmpA64;Cyc_set_cpp((struct _dyneither_ptr)((_tmpA64.tag=0,((_tmpA64.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp16D),((_tmpA65.tag=0,((_tmpA65.f1=(struct _dyneither_ptr)((_tmpA66=_tmp17C,_tag_dyneither(_tmpA66,sizeof(char),_get_zero_arr_size_char((void*)_tmpA66,1)))),((_tmpA67.tag=0,((_tmpA67.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmpA68[0]=& _tmpA67,((_tmpA68[1]=& _tmpA65,((_tmpA68[2]=& _tmpA64,Cyc_aprintf(((_tmpA69="%s -w -x c%s -specs %s",_tag_dyneither(_tmpA69,sizeof(char),23))),_tag_dyneither(_tmpA68,sizeof(void*),3)))))))))))))))))))));}};};};};};};}
# 1423
if(Cyc_cyclone_files == 0  && Cyc_ccargs == 0){
# 1424
{const char*_tmpA6C;void*_tmpA6B;(_tmpA6B=0,Cyc_fprintf(Cyc_stderr,((_tmpA6C="missing file\n",_tag_dyneither(_tmpA6C,sizeof(char),14))),_tag_dyneither(_tmpA6B,sizeof(void*),0)));}
# 1425
exit(1);}
# 1432
if(!Cyc_stop_after_cpp_r  && !Cyc_nocyc_setjmp_r){
# 1433
const char*_tmpA6D;Cyc_cyc_setjmp=Cyc_do_find(cyclone_arch_path,((_tmpA6D="cyc_setjmp.h",_tag_dyneither(_tmpA6D,sizeof(char),13))));}
# 1434
if(!Cyc_stop_after_cpp_r  && Cyc_strlen((struct _dyneither_ptr)Cyc_cyc_include)== 0){
# 1435
const char*_tmpA6E;Cyc_cyc_include=Cyc_do_find(Cyc_cyclone_exec_path,((_tmpA6E="cyc_include.h",_tag_dyneither(_tmpA6E,sizeof(char),14))));}
# 1438
{struct Cyc_List_List*_tmp353=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmp353 != 0;_tmp353=_tmp353->tl){
# 1439
Cyc_process_file(*((struct _dyneither_ptr*)_tmp353->hd));
# 1440
if(Cyc_compile_failure){int _tmp354=1;_npop_handler(0);return _tmp354;}}}
# 1443
if(((Cyc_stop_after_cpp_r  || Cyc_parseonly_r) || Cyc_tc_r) || Cyc_toc_r){int _tmp355=0;_npop_handler(0);return _tmp355;}
# 1449
if(Cyc_ccargs == 0){int _tmp356=0;_npop_handler(0);return _tmp356;}
# 1452
{const char*_tmpA6F;Cyc_add_ccarg((struct _dyneither_ptr)Cyc_strconcat(((_tmpA6F="-L",_tag_dyneither(_tmpA6F,sizeof(char),3))),(struct _dyneither_ptr)def_lib_path));}
# 1454
Cyc_ccargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{
# 1455
const char*_tmpA70;struct _dyneither_ptr _tmp358=Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_ccargs),((_tmpA70=" ",_tag_dyneither(_tmpA70,sizeof(char),2))));
# 1456
Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_libargs);{
# 1457
const char*_tmpA72;struct Cyc_List_List*_tmpA71;struct _dyneither_ptr _tmp359=Cyc_str_sepstr(((_tmpA71=_cycalloc(sizeof(*_tmpA71)),((_tmpA71->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((_tmpA71->tl=
# 1458
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_libargs),_tmpA71)))))),((_tmpA72=" ",_tag_dyneither(_tmpA72,sizeof(char),2))));
# 1460
struct Cyc_List_List*stdlib;
# 1461
struct _dyneither_ptr stdlib_string;
# 1462
const char*_tmpA74;const char*_tmpA73;int _tmp35A=
# 1463
((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r) || 
# 1465
Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmpA74=".a",_tag_dyneither(_tmpA74,sizeof(char),3))))) || 
# 1466
Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmpA73=".lib",_tag_dyneither(_tmpA73,sizeof(char),5))));
# 1467
if(_tmp35A){
# 1468
stdlib=0;{
# 1469
const char*_tmpA75;stdlib_string=(struct _dyneither_ptr)((_tmpA75="",_tag_dyneither(_tmpA75,sizeof(char),1)));};}else{
# 1473
struct _dyneither_ptr libcyc_flag;
# 1474
struct _dyneither_ptr nogc_filename;
# 1475
struct _dyneither_ptr runtime_filename;
# 1476
struct _dyneither_ptr gc_filename;
# 1477
if(Cyc_pa_r){
# 1478
{const char*_tmpA76;libcyc_flag=((_tmpA76="-lcyc_a",_tag_dyneither(_tmpA76,sizeof(char),8)));}
# 1479
{const char*_tmpA77;nogc_filename=((_tmpA77="nogc_a.a",_tag_dyneither(_tmpA77,sizeof(char),9)));}{
# 1480
const char*_tmpA78;runtime_filename=((_tmpA78="runtime_cyc_a.o",_tag_dyneither(_tmpA78,sizeof(char),16)));};}else{
# 1481
if(Cyc_nocheck_r){
# 1482
{const char*_tmpA79;libcyc_flag=((_tmpA79="-lcyc_nocheck",_tag_dyneither(_tmpA79,sizeof(char),14)));}
# 1483
{const char*_tmpA7A;nogc_filename=((_tmpA7A="nogc.a",_tag_dyneither(_tmpA7A,sizeof(char),7)));}{
# 1484
const char*_tmpA7B;runtime_filename=((_tmpA7B="runtime_cyc.o",_tag_dyneither(_tmpA7B,sizeof(char),14)));};}else{
# 1485
if(Cyc_pg_r){
# 1486
{const char*_tmpA7C;libcyc_flag=((_tmpA7C="-lcyc_pg",_tag_dyneither(_tmpA7C,sizeof(char),9)));}
# 1487
{const char*_tmpA7D;runtime_filename=((_tmpA7D="runtime_cyc_pg.o",_tag_dyneither(_tmpA7D,sizeof(char),17)));}{
# 1488
const char*_tmpA7E;nogc_filename=((_tmpA7E="nogc_pg.a",_tag_dyneither(_tmpA7E,sizeof(char),10)));};}else{
# 1489
if(Cyc_Lex_compile_for_boot_r){
# 1491
{const char*_tmpA7F;libcyc_flag=((_tmpA7F="-lcycboot",_tag_dyneither(_tmpA7F,sizeof(char),10)));}
# 1492
{const char*_tmpA80;nogc_filename=((_tmpA80="nogc.a",_tag_dyneither(_tmpA80,sizeof(char),7)));}{
# 1493
const char*_tmpA81;runtime_filename=((_tmpA81="runtime_cyc.o",_tag_dyneither(_tmpA81,sizeof(char),14)));};}else{
# 1495
{const char*_tmpA82;libcyc_flag=((_tmpA82="-lcyc",_tag_dyneither(_tmpA82,sizeof(char),6)));}
# 1496
{const char*_tmpA83;nogc_filename=((_tmpA83="nogc.a",_tag_dyneither(_tmpA83,sizeof(char),7)));}{
# 1497
const char*_tmpA84;runtime_filename=((_tmpA84="runtime_cyc.o",_tag_dyneither(_tmpA84,sizeof(char),14)));};}}}}
# 1499
{const char*_tmpA85;gc_filename=((_tmpA85="gc.a",_tag_dyneither(_tmpA85,sizeof(char),5)));}{
# 1501
struct _dyneither_ptr _tmp36C=Cyc_nogc_r?
# 1502
Cyc_do_find(cyclone_arch_path,nogc_filename):
# 1503
 Cyc_do_find(cyclone_arch_path,gc_filename);
# 1504
struct _dyneither_ptr _tmp36D=Cyc_do_find(cyclone_arch_path,runtime_filename);
# 1508
stdlib=0;{
# 1509
const char*_tmpA8B;void*_tmpA8A[3];struct Cyc_String_pa_PrintArg_struct _tmpA89;struct Cyc_String_pa_PrintArg_struct _tmpA88;struct Cyc_String_pa_PrintArg_struct _tmpA87;stdlib_string=(struct _dyneither_ptr)((_tmpA87.tag=0,((_tmpA87.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp36C),((_tmpA88.tag=0,((_tmpA88.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)libcyc_flag),((_tmpA89.tag=0,((_tmpA89.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp36D),((_tmpA8A[0]=& _tmpA89,((_tmpA8A[1]=& _tmpA88,((_tmpA8A[2]=& _tmpA87,Cyc_aprintf(((_tmpA8B=" %s %s %s",_tag_dyneither(_tmpA8B,sizeof(char),10))),_tag_dyneither(_tmpA8A,sizeof(void*),3))))))))))))))))))));};};}
# 1512
if(Cyc_ic_r){struct _handler_cons _tmp373;_push_handler(& _tmp373);{int _tmp375=0;if(setjmp(_tmp373.handler))_tmp375=1;if(!_tmp375){
# 1513
Cyc_ccargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_ccargs);
# 1514
Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{
# 1515
struct Cyc_List_List*_tmp376=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));
# 1516
if(!_tmp35A){
# 1517
struct Cyc_List_List*_tmpA8C;_tmp376=((_tmpA8C=_cycalloc(sizeof(*_tmpA8C)),((_tmpA8C->hd=Cyc_final_strptr,((_tmpA8C->tl=_tmp376,_tmpA8C))))));}{
# 1520
struct Cyc_Interface_I*_tmp378=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct _dyneither_ptr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(Cyc_read_cycio,_tmp376,_tmp376);
# 1521
if(_tmp378 == 0){
# 1522
{const char*_tmpA8F;void*_tmpA8E;(_tmpA8E=0,Cyc_fprintf(Cyc_stderr,((_tmpA8F="Error: interfaces incompatible\n",_tag_dyneither(_tmpA8F,sizeof(char),32))),_tag_dyneither(_tmpA8E,sizeof(void*),0)));}
# 1523
Cyc_compile_failure=1;
# 1524
Cyc_remove_cfiles();{
# 1525
int _tmp37B=1;_npop_handler(1);return _tmp37B;};}
# 1527
if(_tmp35A){
# 1528
if(Cyc_output_file != 0){
# 1529
const char*_tmpA93;void*_tmpA92[1];struct Cyc_String_pa_PrintArg_struct _tmpA91;struct _dyneither_ptr _tmp37C=(_tmpA91.tag=0,((_tmpA91.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Filename_chop_extension(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpA92[0]=& _tmpA91,Cyc_aprintf(((_tmpA93="%s.cycio",_tag_dyneither(_tmpA93,sizeof(char),9))),_tag_dyneither(_tmpA92,sizeof(void*),1)))))));
# 1530
const char*_tmpA95;const char*_tmpA94;struct Cyc___cycFILE*_tmp37D=Cyc_try_file_open((struct _dyneither_ptr)_tmp37C,((_tmpA94="wb",_tag_dyneither(_tmpA94,sizeof(char),3))),((_tmpA95="interface object file",_tag_dyneither(_tmpA95,sizeof(char),22))));
# 1531
if(_tmp37D == 0){
# 1532
Cyc_compile_failure=1;
# 1533
Cyc_remove_cfiles();{
# 1534
int _tmp37E=1;_npop_handler(1);return _tmp37E;};}
# 1536
Cyc_Interface_save((struct Cyc_Interface_I*)_tmp378,(struct Cyc___cycFILE*)_tmp37D);
# 1537
Cyc_file_close((struct Cyc___cycFILE*)_tmp37D);}}else{
# 1540
const char*_tmpA9A;const char*_tmpA99;struct _tuple21*_tmpA98;if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),(struct Cyc_Interface_I*)_tmp378,(
# 1541
(_tmpA98=_cycalloc(sizeof(*_tmpA98)),((_tmpA98->f1=((_tmpA99="empty interface",_tag_dyneither(_tmpA99,sizeof(char),16))),((_tmpA98->f2=((_tmpA9A="global interface",_tag_dyneither(_tmpA9A,sizeof(char),17))),_tmpA98)))))))){
# 1542
{const char*_tmpA9D;void*_tmpA9C;(_tmpA9C=0,Cyc_fprintf(Cyc_stderr,((_tmpA9D="Error: some objects are still undefined\n",_tag_dyneither(_tmpA9D,sizeof(char),41))),_tag_dyneither(_tmpA9C,sizeof(void*),0)));}
# 1543
Cyc_compile_failure=1;
# 1544
Cyc_remove_cfiles();{
# 1545
int _tmp389=1;_npop_handler(1);return _tmp389;};}}};};
# 1513
;_pop_handler();}else{void*_tmp374=(void*)_exn_thrown;void*_tmp38B=_tmp374;_LL3A:;_LL3B:
# 1549
{void*_tmp38C=_tmp38B;struct _dyneither_ptr _tmp38E;struct _dyneither_ptr _tmp390;struct _dyneither_ptr _tmp393;_LL3F: {struct Cyc_Core_Failure_exn_struct*_tmp38D=(struct Cyc_Core_Failure_exn_struct*)_tmp38C;if(_tmp38D->tag != Cyc_Core_Failure)goto _LL41;else{_tmp38E=_tmp38D->f1;}}_LL40:
# 1551
{const char*_tmpAA1;void*_tmpAA0[1];struct Cyc_String_pa_PrintArg_struct _tmpA9F;(_tmpA9F.tag=0,((_tmpA9F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp38E),((_tmpAA0[0]=& _tmpA9F,Cyc_fprintf(Cyc_stderr,((_tmpAA1="Exception Core::Failure %s\n",_tag_dyneither(_tmpAA1,sizeof(char),28))),_tag_dyneither(_tmpAA0,sizeof(void*),1)))))));}goto _LL3E;_LL41: {struct Cyc_Core_Impossible_exn_struct*_tmp38F=(struct Cyc_Core_Impossible_exn_struct*)_tmp38C;if(_tmp38F->tag != Cyc_Core_Impossible)goto _LL43;else{_tmp390=_tmp38F->f1;}}_LL42:
# 1553
{const char*_tmpAA5;void*_tmpAA4[1];struct Cyc_String_pa_PrintArg_struct _tmpAA3;(_tmpAA3.tag=0,((_tmpAA3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp390),((_tmpAA4[0]=& _tmpAA3,Cyc_fprintf(Cyc_stderr,((_tmpAA5="Exception Core::Impossible %s\n",_tag_dyneither(_tmpAA5,sizeof(char),31))),_tag_dyneither(_tmpAA4,sizeof(void*),1)))))));}goto _LL3E;_LL43: {struct Cyc_Dict_Absent_exn_struct*_tmp391=(struct Cyc_Dict_Absent_exn_struct*)_tmp38C;if(_tmp391->tag != Cyc_Dict_Absent)goto _LL45;}_LL44:
# 1555
{const char*_tmpAA8;void*_tmpAA7;(_tmpAA7=0,Cyc_fprintf(Cyc_stderr,((_tmpAA8="Exception Dict::Absent\n",_tag_dyneither(_tmpAA8,sizeof(char),24))),_tag_dyneither(_tmpAA7,sizeof(void*),0)));}goto _LL3E;_LL45: {struct Cyc_Core_Invalid_argument_exn_struct*_tmp392=(struct Cyc_Core_Invalid_argument_exn_struct*)_tmp38C;if(_tmp392->tag != Cyc_Core_Invalid_argument)goto _LL47;else{_tmp393=_tmp392->f1;}}_LL46:
# 1557
{const char*_tmpAAC;void*_tmpAAB[1];struct Cyc_String_pa_PrintArg_struct _tmpAAA;(_tmpAAA.tag=0,((_tmpAAA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp393),((_tmpAAB[0]=& _tmpAAA,Cyc_fprintf(Cyc_stderr,((_tmpAAC="Exception Core::Invalid_argument %s\n",_tag_dyneither(_tmpAAC,sizeof(char),37))),_tag_dyneither(_tmpAAB,sizeof(void*),1)))))));}goto _LL3E;_LL47:;_LL48:
# 1559
{const char*_tmpAAF;void*_tmpAAE;(_tmpAAE=0,Cyc_fprintf(Cyc_stderr,((_tmpAAF="Uncaught exception\n",_tag_dyneither(_tmpAAF,sizeof(char),20))),_tag_dyneither(_tmpAAE,sizeof(void*),0)));}goto _LL3E;_LL3E:;}
# 1561
Cyc_compile_failure=1;
# 1562
Cyc_remove_cfiles();{
# 1563
int _tmp3A1=1;_npop_handler(0);return _tmp3A1;};_LL3C:;_LL3D:(void)_throw(_tmp38B);_LL39:;}};}{
# 1566
const char*_tmpAB0;struct _dyneither_ptr outfile_str=(_tmpAB0="",_tag_dyneither(_tmpAB0,sizeof(char),1));
# 1567
if(Cyc_output_file != 0){
# 1568
const char*_tmpAB4;void*_tmpAB3[1];struct Cyc_String_pa_PrintArg_struct _tmpAB2;outfile_str=(struct _dyneither_ptr)((_tmpAB2.tag=0,((_tmpAB2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpAB3[0]=& _tmpAB2,Cyc_aprintf(((_tmpAB4=" -o %s",_tag_dyneither(_tmpAB4,sizeof(char),7))),_tag_dyneither(_tmpAB3,sizeof(void*),1))))))));}{
# 1570
const char*_tmpABC;void*_tmpABB[5];struct Cyc_String_pa_PrintArg_struct _tmpABA;struct Cyc_String_pa_PrintArg_struct _tmpAB9;struct Cyc_String_pa_PrintArg_struct _tmpAB8;struct Cyc_String_pa_PrintArg_struct _tmpAB7;struct Cyc_String_pa_PrintArg_struct _tmpAB6;struct _dyneither_ptr _tmp3A5=
# 1571
(_tmpAB6.tag=0,((_tmpAB6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp359),((_tmpAB7.tag=0,((_tmpAB7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdlib_string),((_tmpAB8.tag=0,((_tmpAB8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp358),((_tmpAB9.tag=0,((_tmpAB9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)outfile_str),((_tmpABA.tag=0,((_tmpABA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmpABB[0]=& _tmpABA,((_tmpABB[1]=& _tmpAB9,((_tmpABB[2]=& _tmpAB8,((_tmpABB[3]=& _tmpAB7,((_tmpABB[4]=& _tmpAB6,Cyc_aprintf(((_tmpABC="%s %s %s%s%s",_tag_dyneither(_tmpABC,sizeof(char),13))),_tag_dyneither(_tmpABB,sizeof(void*),5)))))))))))))))))))))))))))))));
# 1573
if(Cyc_v_r){{const char*_tmpAC0;void*_tmpABF[1];struct Cyc_String_pa_PrintArg_struct _tmpABE;(_tmpABE.tag=0,((_tmpABE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3A5),((_tmpABF[0]=& _tmpABE,Cyc_fprintf(Cyc_stderr,((_tmpAC0="%s\n",_tag_dyneither(_tmpAC0,sizeof(char),4))),_tag_dyneither(_tmpABF,sizeof(void*),1)))))));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}
# 1574
if(system((const char*)_untag_dyneither_ptr(_tmp3A5,sizeof(char),1))!= 0){
# 1575
{const char*_tmpAC3;void*_tmpAC2;(_tmpAC2=0,Cyc_fprintf(Cyc_stderr,((_tmpAC3="Error: C compiler failed\n",_tag_dyneither(_tmpAC3,sizeof(char),26))),_tag_dyneither(_tmpAC2,sizeof(void*),0)));}
# 1576
Cyc_compile_failure=1;
# 1577
Cyc_remove_cfiles();{
# 1578
int _tmp3AB=1;_npop_handler(0);return _tmp3AB;};}
# 1580
Cyc_remove_cfiles();{
# 1582
int _tmp3AC=Cyc_compile_failure?1: 0;_npop_handler(0);return _tmp3AC;};};};};};
# 1099 "cyclone.cyc"
;_pop_region(r);};}
