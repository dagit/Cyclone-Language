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
# 26 "/home/jgm/cyclone/include/cycboot.h"
typedef unsigned long Cyc_size_t;
# 33
typedef unsigned short Cyc_mode_t;struct Cyc___cycFILE;
# 49
typedef struct Cyc___cycFILE Cyc_FILE;struct Cyc_String_pa_PrintArg_struct{int tag;
struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};
struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};
struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};
struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{
int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};
struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 252 "/home/jgm/cyclone/include/cycboot.h"
int Cyc_getw(struct Cyc___cycFILE*);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{
char*tag;struct _dyneither_ptr f1;};
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
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];
struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4];
struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(
void*env,void*dest);};
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
# 62
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 68
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 72
struct Cyc_Dict_Dict Cyc_Dict_rshare(struct _RegionHandle*,struct Cyc_Dict_Dict);
# 77
int Cyc_Dict_is_empty(struct Cyc_Dict_Dict d);
# 80
int Cyc_Dict_cardinality(struct Cyc_Dict_Dict d);
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 92
struct Cyc_Dict_Dict Cyc_Dict_insert_new(struct Cyc_Dict_Dict d,void*k,void*v);
# 96
struct Cyc_Dict_Dict Cyc_Dict_inserts(struct Cyc_Dict_Dict d,struct Cyc_List_List*l);
# 101
struct Cyc_Dict_Dict Cyc_Dict_singleton(int(*cmp)(void*,void*),void*k,void*v);
# 104
struct Cyc_Dict_Dict Cyc_Dict_rsingleton(struct _RegionHandle*,int(*cmp)(void*,
void*),void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 113
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
# 122 "/home/jgm/cyclone/include/dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
# 126
int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**ans);
# 131
void*Cyc_Dict_fold(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d,void*accum);
# 135
void*Cyc_Dict_fold_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict
d,void*accum);
# 139
void Cyc_Dict_app(void*(*f)(void*,void*),struct Cyc_Dict_Dict d);
# 143
void Cyc_Dict_app_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 147
void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict d);
# 149
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 153
void Cyc_Dict_iter2(void(*f)(void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2);
# 158
void Cyc_Dict_iter2_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,
struct Cyc_Dict_Dict d2);
# 162
void*Cyc_Dict_fold2_c(void*(*f)(void*,void*,void*,void*,void*),void*env,struct
Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2,void*accum);
# 170
struct Cyc_Dict_Dict Cyc_Dict_rcopy(struct _RegionHandle*,struct Cyc_Dict_Dict);
# 173
struct Cyc_Dict_Dict Cyc_Dict_copy(struct Cyc_Dict_Dict);
# 176
struct Cyc_Dict_Dict Cyc_Dict_map(void*(*f)(void*),struct Cyc_Dict_Dict d);
# 181
struct Cyc_Dict_Dict Cyc_Dict_rmap(struct _RegionHandle*,void*(*f)(void*),struct
Cyc_Dict_Dict d);
# 186
struct Cyc_Dict_Dict Cyc_Dict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict
d);
# 189
struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_Dict_Dict d);
# 195
struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 206 "/home/jgm/cyclone/include/dict.h"
struct Cyc_Dict_Dict Cyc_Dict_intersect(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict
d1,struct Cyc_Dict_Dict d2);
# 215
struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),void*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 221
int Cyc_Dict_forall_c(int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 225
int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,
struct Cyc_Dict_Dict d2);struct _tuple0{void*f1;void*f2;};
# 232
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
# 236
struct Cyc_List_List*Cyc_Dict_to_list(struct Cyc_Dict_Dict d);
# 239
struct Cyc_List_List*Cyc_Dict_rto_list(struct _RegionHandle*,struct Cyc_Dict_Dict d);
# 243
struct Cyc_Dict_Dict Cyc_Dict_filter(int(*f)(void*,void*),struct Cyc_Dict_Dict d);
# 247
struct Cyc_Dict_Dict Cyc_Dict_rfilter(struct _RegionHandle*,int(*f)(void*,void*),
struct Cyc_Dict_Dict d);
# 251
struct Cyc_Dict_Dict Cyc_Dict_filter_c(int(*f)(void*,void*,void*),void*env,struct
Cyc_Dict_Dict d);
# 255
struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(struct _RegionHandle*,int(*f)(void*,void*,
void*),void*env,struct Cyc_Dict_Dict d);
# 262
struct Cyc_Dict_Dict Cyc_Dict_difference(struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2);
# 268
struct Cyc_Dict_Dict Cyc_Dict_rdifference(struct _RegionHandle*,struct Cyc_Dict_Dict
d1,struct Cyc_Dict_Dict d2);
# 275
struct Cyc_Dict_Dict Cyc_Dict_delete(struct Cyc_Dict_Dict,void*);
# 279
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict,
void*);
# 282
struct Cyc_Dict_Dict Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict,void*);
# 288
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict
d);
# 295
void*Cyc_Dict_marshal(struct _RegionHandle*rgn,void*env,void*(*write_key)(void*,
struct Cyc___cycFILE*,void*),void*(*write_val)(void*,struct Cyc___cycFILE*,void*),
struct Cyc___cycFILE*fp,struct Cyc_Dict_Dict dict);
# 302
struct Cyc_Dict_Dict Cyc_Dict_unmarshal(struct _RegionHandle*rgn,void*env,int(*cmp)(
void*,void*),void*(*read_key)(void*,struct Cyc___cycFILE*),void*(*read_val)(void*,
struct Cyc___cycFILE*),struct Cyc___cycFILE*fp);char Cyc_Dict_Absent[7]="Absent";
char Cyc_Dict_Present[8]="Present";
# 27 "/home/jgm/cyclone/lib/dict.cyc"
struct Cyc_Dict_Absent_exn_struct Cyc_Dict_Absent_val={Cyc_Dict_Absent};
# 28
struct Cyc_Dict_Present_exn_struct Cyc_Dict_Present_val={Cyc_Dict_Present};
# 30
enum Cyc_Dict_Color{Cyc_Dict_R  = 0,Cyc_Dict_B  = 1};struct Cyc_Dict_T{enum Cyc_Dict_Color
color;const struct Cyc_Dict_T*left;const struct Cyc_Dict_T*right;struct _tuple0
key_val;};
# 39
typedef const struct Cyc_Dict_T*Cyc_Dict_Tree_t;
# 41
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*r,int(*comp)(void*,void*)){
# 42
struct Cyc_Dict_Dict _tmp12F;return(_tmp12F.rel=comp,((_tmp12F.r=r,((_tmp12F.t=0,
_tmp12F)))));}
# 44
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*comp)(void*,void*)){
# 45
return Cyc_Dict_rempty(Cyc_Core_heap_region,comp);}
# 47
struct Cyc_Dict_Dict Cyc_Dict_rsingleton(struct _RegionHandle*r,int(*comp)(void*,
void*),void*key,void*data){
# 49
struct Cyc_Dict_T*_tmp135;struct _tuple0 _tmp134;struct Cyc_Dict_Dict _tmp133;return(
_tmp133.rel=comp,((_tmp133.r=r,((_tmp133.t=(const struct Cyc_Dict_T*)((_tmp135=
_region_malloc(r,sizeof(*_tmp135)),((_tmp135->color=Cyc_Dict_B,((_tmp135->left=0,((
_tmp135->right=0,((_tmp135->key_val=((_tmp134.f1=key,((_tmp134.f2=data,_tmp134)))),
_tmp135)))))))))),_tmp133)))));}
# 51
struct Cyc_Dict_Dict Cyc_Dict_singleton(int(*comp)(void*,void*),void*key,void*
data){
# 52
return Cyc_Dict_rsingleton(Cyc_Core_heap_region,comp,key,data);}
# 55
struct Cyc_Dict_Dict Cyc_Dict_rshare(struct _RegionHandle*r,struct Cyc_Dict_Dict d){
# 56
const struct Cyc_Dict_T*t=d.t;
# 57
const struct Cyc_Dict_T*t2=(const struct Cyc_Dict_T*)t;
# 58
struct Cyc_Dict_Dict _tmp136;return(_tmp136.rel=d.rel,((_tmp136.r=r,((_tmp136.t=(
const struct Cyc_Dict_T*)t2,_tmp136)))));}
# 61
int Cyc_Dict_is_empty(struct Cyc_Dict_Dict d){
# 62
return d.t == (const struct Cyc_Dict_T*)0;}
# 65
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*key){
# 66
int(*_tmp7)(void*,void*)=d.rel;
# 67
const struct Cyc_Dict_T*_tmp8=d.t;
# 68
while(_tmp8 != (const struct Cyc_Dict_T*)0){
# 69
int _tmp9=_tmp7(key,(_tmp8->key_val).f1);
# 70
if(_tmp9 < 0)_tmp8=_tmp8->left;else{
# 71
if(_tmp9 > 0)_tmp8=_tmp8->right;else{
# 72
return 1;}}}
# 74
return 0;}
# 78
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*key){
# 79
int(*_tmpA)(void*,void*)=d.rel;
# 80
const struct Cyc_Dict_T*_tmpB=d.t;
# 81
while(_tmpB != (const struct Cyc_Dict_T*)0){
# 82
int _tmpC=_tmpA(key,(_tmpB->key_val).f1);
# 83
if(_tmpC < 0)_tmpB=_tmpB->left;else{
# 84
if(_tmpC > 0)_tmpB=_tmpB->right;else{
# 85
return(_tmpB->key_val).f2;}}}
# 87
(int)_throw((void*)& Cyc_Dict_Absent_val);}
# 90
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*key){
# 91
const struct Cyc_Dict_T*_tmpD=d.t;
# 92
while(_tmpD != (const struct Cyc_Dict_T*)0){
# 93
int _tmpE=cmp(key,(_tmpD->key_val).f1);
# 94
if(_tmpE < 0)_tmpD=_tmpD->left;else{
# 95
if(_tmpE > 0)_tmpD=_tmpD->right;else{
# 96
return(_tmpD->key_val).f2;}}}
# 98
(int)_throw((void*)& Cyc_Dict_Absent_val);}
# 101
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*key){
# 102
int(*_tmpF)(void*,void*)=d.rel;
# 103
const struct Cyc_Dict_T*_tmp10=d.t;
# 104
while(_tmp10 != (const struct Cyc_Dict_T*)0){
# 105
int _tmp11=_tmpF(key,(_tmp10->key_val).f1);
# 106
if(_tmp11 < 0)_tmp10=_tmp10->left;else{
# 107
if(_tmp11 > 0)_tmp10=_tmp10->right;else{
# 108
return(void**)&(_tmp10->key_val).f2;}}}
# 110
return 0;}
# 113
int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*key,void**ans_place){
# 114
int(*_tmp12)(void*,void*)=d.rel;
# 115
const struct Cyc_Dict_T*_tmp13=d.t;
# 116
while(_tmp13 != (const struct Cyc_Dict_T*)0){
# 117
int _tmp14=_tmp12(key,(_tmp13->key_val).f1);
# 118
if(_tmp14 < 0)_tmp13=_tmp13->left;else{
# 119
if(_tmp14 > 0)_tmp13=_tmp13->right;else{
# 121
*ans_place=(_tmp13->key_val).f2;
# 122
return 1;}}}
# 125
return 0;}struct _tuple1{enum Cyc_Dict_Color f1;const struct Cyc_Dict_T*f2;const
struct Cyc_Dict_T*f3;struct _tuple0 f4;};
# 128
static const struct Cyc_Dict_T*Cyc_Dict_balance(struct _RegionHandle*r,struct
_tuple1 quad){
# 131
struct _tuple1 _tmp15=quad;enum Cyc_Dict_Color _tmp16;const struct Cyc_Dict_T*_tmp17;
struct Cyc_Dict_T _tmp18;enum Cyc_Dict_Color _tmp19;const struct Cyc_Dict_T*_tmp1A;
struct Cyc_Dict_T _tmp1B;enum Cyc_Dict_Color _tmp1C;const struct Cyc_Dict_T*_tmp1D;
const struct Cyc_Dict_T*_tmp1E;struct _tuple0 _tmp1F;const struct Cyc_Dict_T*_tmp20;
struct _tuple0 _tmp21;const struct Cyc_Dict_T*_tmp22;struct _tuple0 _tmp23;enum Cyc_Dict_Color
_tmp24;const struct Cyc_Dict_T*_tmp25;struct Cyc_Dict_T _tmp26;enum Cyc_Dict_Color
_tmp27;const struct Cyc_Dict_T*_tmp28;const struct Cyc_Dict_T*_tmp29;struct Cyc_Dict_T
_tmp2A;enum Cyc_Dict_Color _tmp2B;const struct Cyc_Dict_T*_tmp2C;const struct Cyc_Dict_T*
_tmp2D;struct _tuple0 _tmp2E;struct _tuple0 _tmp2F;const struct Cyc_Dict_T*_tmp30;
struct _tuple0 _tmp31;enum Cyc_Dict_Color _tmp32;const struct Cyc_Dict_T*_tmp33;const
struct Cyc_Dict_T*_tmp34;struct Cyc_Dict_T _tmp35;enum Cyc_Dict_Color _tmp36;const
struct Cyc_Dict_T*_tmp37;struct Cyc_Dict_T _tmp38;enum Cyc_Dict_Color _tmp39;const
struct Cyc_Dict_T*_tmp3A;const struct Cyc_Dict_T*_tmp3B;struct _tuple0 _tmp3C;const
struct Cyc_Dict_T*_tmp3D;struct _tuple0 _tmp3E;struct _tuple0 _tmp3F;enum Cyc_Dict_Color
_tmp40;const struct Cyc_Dict_T*_tmp41;const struct Cyc_Dict_T*_tmp42;struct Cyc_Dict_T
_tmp43;enum Cyc_Dict_Color _tmp44;const struct Cyc_Dict_T*_tmp45;const struct Cyc_Dict_T*
_tmp46;struct Cyc_Dict_T _tmp47;enum Cyc_Dict_Color _tmp48;const struct Cyc_Dict_T*
_tmp49;const struct Cyc_Dict_T*_tmp4A;struct _tuple0 _tmp4B;struct _tuple0 _tmp4C;
struct _tuple0 _tmp4D;enum Cyc_Dict_Color _tmp4E;const struct Cyc_Dict_T*_tmp4F;const
struct Cyc_Dict_T*_tmp50;struct _tuple0 _tmp51;_LL1: _tmp16=_tmp15.f1;if(_tmp16 != 
Cyc_Dict_B)goto _LL3;_tmp17=_tmp15.f2;if(_tmp17 == 0)goto _LL3;_tmp18=*_tmp17;
_tmp19=_tmp18.color;if(_tmp19 != Cyc_Dict_R)goto _LL3;_tmp1A=_tmp18.left;if(_tmp1A
== 0)goto _LL3;_tmp1B=*_tmp1A;_tmp1C=_tmp1B.color;if(_tmp1C != Cyc_Dict_R)goto _LL3;
_tmp1D=_tmp1B.left;_tmp1E=_tmp1B.right;_tmp1F=_tmp1B.key_val;_tmp20=_tmp18.right;
_tmp21=_tmp18.key_val;_tmp22=_tmp15.f3;_tmp23=_tmp15.f4;_LL2: {
# 133
struct Cyc_Dict_T*_tmp13B;struct Cyc_Dict_T*_tmp13A;struct Cyc_Dict_T*_tmp139;
return(const struct Cyc_Dict_T*)((_tmp139=_region_malloc(r,sizeof(*_tmp139)),((
_tmp139->color=Cyc_Dict_R,((_tmp139->left=(const struct Cyc_Dict_T*)((_tmp13A=
_region_malloc(r,sizeof(*_tmp13A)),((_tmp13A->color=Cyc_Dict_B,((_tmp13A->left=
_tmp1D,((_tmp13A->right=_tmp1E,((_tmp13A->key_val=_tmp1F,_tmp13A)))))))))),((
_tmp139->right=(const struct Cyc_Dict_T*)((_tmp13B=_region_malloc(r,sizeof(*
_tmp13B)),((_tmp13B->color=Cyc_Dict_B,((_tmp13B->left=_tmp20,((_tmp13B->right=
_tmp22,((_tmp13B->key_val=_tmp23,_tmp13B)))))))))),((_tmp139->key_val=_tmp21,
_tmp139))))))))));}_LL3: _tmp24=_tmp15.f1;if(_tmp24 != Cyc_Dict_B)goto _LL5;_tmp25=
_tmp15.f2;if(_tmp25 == 0)goto _LL5;_tmp26=*_tmp25;_tmp27=_tmp26.color;if(_tmp27 != 
Cyc_Dict_R)goto _LL5;_tmp28=_tmp26.left;_tmp29=_tmp26.right;if(_tmp29 == 0)goto
_LL5;_tmp2A=*_tmp29;_tmp2B=_tmp2A.color;if(_tmp2B != Cyc_Dict_R)goto _LL5;_tmp2C=
_tmp2A.left;_tmp2D=_tmp2A.right;_tmp2E=_tmp2A.key_val;_tmp2F=_tmp26.key_val;
_tmp30=_tmp15.f3;_tmp31=_tmp15.f4;_LL4: {
# 135
struct Cyc_Dict_T*_tmp140;struct Cyc_Dict_T*_tmp13F;struct Cyc_Dict_T*_tmp13E;
return(const struct Cyc_Dict_T*)((_tmp13E=_region_malloc(r,sizeof(*_tmp13E)),((
_tmp13E->color=Cyc_Dict_R,((_tmp13E->left=(const struct Cyc_Dict_T*)((_tmp13F=
_region_malloc(r,sizeof(*_tmp13F)),((_tmp13F->color=Cyc_Dict_B,((_tmp13F->left=
_tmp28,((_tmp13F->right=_tmp2C,((_tmp13F->key_val=_tmp2F,_tmp13F)))))))))),((
_tmp13E->right=(const struct Cyc_Dict_T*)((_tmp140=_region_malloc(r,sizeof(*
_tmp140)),((_tmp140->color=Cyc_Dict_B,((_tmp140->left=_tmp2D,((_tmp140->right=
_tmp30,((_tmp140->key_val=_tmp31,_tmp140)))))))))),((_tmp13E->key_val=_tmp2E,
_tmp13E))))))))));}_LL5: _tmp32=_tmp15.f1;if(_tmp32 != Cyc_Dict_B)goto _LL7;_tmp33=
_tmp15.f2;_tmp34=_tmp15.f3;if(_tmp34 == 0)goto _LL7;_tmp35=*_tmp34;_tmp36=_tmp35.color;
if(_tmp36 != Cyc_Dict_R)goto _LL7;_tmp37=_tmp35.left;if(_tmp37 == 0)goto _LL7;_tmp38=*
_tmp37;_tmp39=_tmp38.color;if(_tmp39 != Cyc_Dict_R)goto _LL7;_tmp3A=_tmp38.left;
_tmp3B=_tmp38.right;_tmp3C=_tmp38.key_val;_tmp3D=_tmp35.right;_tmp3E=_tmp35.key_val;
_tmp3F=_tmp15.f4;_LL6: {
# 137
struct Cyc_Dict_T*_tmp145;struct Cyc_Dict_T*_tmp144;struct Cyc_Dict_T*_tmp143;
return(const struct Cyc_Dict_T*)((_tmp143=_region_malloc(r,sizeof(*_tmp143)),((
_tmp143->color=Cyc_Dict_R,((_tmp143->left=(const struct Cyc_Dict_T*)((_tmp144=
_region_malloc(r,sizeof(*_tmp144)),((_tmp144->color=Cyc_Dict_B,((_tmp144->left=
_tmp33,((_tmp144->right=_tmp3A,((_tmp144->key_val=_tmp3F,_tmp144)))))))))),((
_tmp143->right=(const struct Cyc_Dict_T*)((_tmp145=_region_malloc(r,sizeof(*
_tmp145)),((_tmp145->color=Cyc_Dict_B,((_tmp145->left=_tmp3B,((_tmp145->right=
_tmp3D,((_tmp145->key_val=_tmp3E,_tmp145)))))))))),((_tmp143->key_val=_tmp3C,
_tmp143))))))))));}_LL7: _tmp40=_tmp15.f1;if(_tmp40 != Cyc_Dict_B)goto _LL9;_tmp41=
_tmp15.f2;_tmp42=_tmp15.f3;if(_tmp42 == 0)goto _LL9;_tmp43=*_tmp42;_tmp44=_tmp43.color;
if(_tmp44 != Cyc_Dict_R)goto _LL9;_tmp45=_tmp43.left;_tmp46=_tmp43.right;if(_tmp46
== 0)goto _LL9;_tmp47=*_tmp46;_tmp48=_tmp47.color;if(_tmp48 != Cyc_Dict_R)goto _LL9;
_tmp49=_tmp47.left;_tmp4A=_tmp47.right;_tmp4B=_tmp47.key_val;_tmp4C=_tmp43.key_val;
_tmp4D=_tmp15.f4;_LL8: {
# 139
struct Cyc_Dict_T*_tmp14A;struct Cyc_Dict_T*_tmp149;struct Cyc_Dict_T*_tmp148;
return(const struct Cyc_Dict_T*)((_tmp148=_region_malloc(r,sizeof(*_tmp148)),((
_tmp148->color=Cyc_Dict_R,((_tmp148->left=(const struct Cyc_Dict_T*)((_tmp149=
_region_malloc(r,sizeof(*_tmp149)),((_tmp149->color=Cyc_Dict_B,((_tmp149->left=
_tmp41,((_tmp149->right=_tmp45,((_tmp149->key_val=_tmp4D,_tmp149)))))))))),((
_tmp148->right=(const struct Cyc_Dict_T*)((_tmp14A=_region_malloc(r,sizeof(*
_tmp14A)),((_tmp14A->color=Cyc_Dict_B,((_tmp14A->left=_tmp49,((_tmp14A->right=
_tmp4A,((_tmp14A->key_val=_tmp4B,_tmp14A)))))))))),((_tmp148->key_val=_tmp4C,
_tmp148))))))))));}_LL9: _tmp4E=_tmp15.f1;_tmp4F=_tmp15.f2;_tmp50=_tmp15.f3;
_tmp51=_tmp15.f4;_LLA: {
# 141
struct Cyc_Dict_T*_tmp14B;return(const struct Cyc_Dict_T*)((_tmp14B=_region_malloc(
r,sizeof(*_tmp14B)),((_tmp14B->color=_tmp4E,((_tmp14B->left=_tmp4F,((_tmp14B->right=
_tmp50,((_tmp14B->key_val=_tmp51,_tmp14B))))))))));}_LL0:;}
# 145
static const struct Cyc_Dict_T*Cyc_Dict_ins(struct _RegionHandle*r,int(*rel)(void*,
void*),struct _tuple0 key_val,const struct Cyc_Dict_T*t){
# 147
const struct Cyc_Dict_T*_tmp5F=t;struct Cyc_Dict_T _tmp60;enum Cyc_Dict_Color _tmp61;
const struct Cyc_Dict_T*_tmp62;const struct Cyc_Dict_T*_tmp63;struct _tuple0 _tmp64;
_LLC: if(_tmp5F != 0)goto _LLE;_LLD: {
# 148
struct Cyc_Dict_T*_tmp14C;return(const struct Cyc_Dict_T*)((_tmp14C=_region_malloc(
r,sizeof(*_tmp14C)),((_tmp14C->color=Cyc_Dict_R,((_tmp14C->left=0,((_tmp14C->right=
0,((_tmp14C->key_val=key_val,_tmp14C))))))))));}_LLE: if(_tmp5F == 0)goto _LLB;
_tmp60=*_tmp5F;_tmp61=_tmp60.color;_tmp62=_tmp60.left;_tmp63=_tmp60.right;_tmp64=
_tmp60.key_val;_LLF: {
# 150
int _tmp66=rel(key_val.f1,_tmp64.f1);
# 151
if(_tmp66 < 0){struct _tuple1 _tmp14D;return Cyc_Dict_balance(r,((_tmp14D.f1=_tmp61,((
_tmp14D.f2=Cyc_Dict_ins(r,rel,key_val,_tmp62),((_tmp14D.f3=_tmp63,((_tmp14D.f4=
_tmp64,_tmp14D)))))))));}else{
# 152
if(_tmp66 > 0){struct _tuple1 _tmp14E;return Cyc_Dict_balance(r,((_tmp14E.f1=_tmp61,((
_tmp14E.f2=_tmp62,((_tmp14E.f3=Cyc_Dict_ins(r,rel,key_val,_tmp63),((_tmp14E.f4=
_tmp64,_tmp14E)))))))));}else{
# 153
struct Cyc_Dict_T*_tmp14F;return(const struct Cyc_Dict_T*)((_tmp14F=_region_malloc(
r,sizeof(*_tmp14F)),((_tmp14F->color=_tmp61,((_tmp14F->left=_tmp62,((_tmp14F->right=
_tmp63,((_tmp14F->key_val=key_val,_tmp14F))))))))));}}}_LLB:;}
# 157
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*key,void*data){
# 158
struct _tuple0 _tmp150;const struct Cyc_Dict_T*_tmp6A=Cyc_Dict_ins(d.r,d.rel,((
_tmp150.f1=key,((_tmp150.f2=data,_tmp150)))),d.t);
# 159
((const struct Cyc_Dict_T*)_check_null(_tmp6A))->color=Cyc_Dict_B;{
# 160
struct Cyc_Dict_Dict _tmp151;struct Cyc_Dict_Dict _tmp6B=(_tmp151.rel=d.rel,((
_tmp151.r=d.r,((_tmp151.t=_tmp6A,_tmp151)))));
# 161
return _tmp6B;};}
# 165
struct Cyc_Dict_Dict Cyc_Dict_insert_new(struct Cyc_Dict_Dict d,void*key,void*data){
# 166
if(Cyc_Dict_member(d,key))
# 167
(int)_throw((void*)& Cyc_Dict_Absent_val);
# 168
return Cyc_Dict_insert(d,key,data);}
# 171
struct Cyc_Dict_Dict Cyc_Dict_inserts(struct Cyc_Dict_Dict d,struct Cyc_List_List*
kds){
# 173
for(0;kds != 0;kds=kds->tl){
# 174
d=Cyc_Dict_insert(d,(((struct _tuple0*)kds->hd)[0]).f1,(((struct _tuple0*)kds->hd)[
0]).f2);}
# 175
return d;}
# 178
static void*Cyc_Dict_fold_tree(void*(*f)(void*,void*,void*),const struct Cyc_Dict_T*
t,void*accum){
# 179
struct Cyc_Dict_T _tmp6F;const struct Cyc_Dict_T*_tmp70;const struct Cyc_Dict_T*
_tmp71;struct _tuple0 _tmp72;void*_tmp73;void*_tmp74;const struct Cyc_Dict_T*_tmp6E=
t;_tmp6F=*_tmp6E;_tmp70=_tmp6F.left;_tmp71=_tmp6F.right;_tmp72=_tmp6F.key_val;
_tmp73=_tmp72.f1;_tmp74=_tmp72.f2;
# 180
if(_tmp70 != (const struct Cyc_Dict_T*)0)accum=Cyc_Dict_fold_tree(f,(const struct
Cyc_Dict_T*)_tmp70,accum);
# 181
accum=f(_tmp73,_tmp74,accum);
# 182
if(_tmp71 != (const struct Cyc_Dict_T*)0)accum=Cyc_Dict_fold_tree(f,(const struct
Cyc_Dict_T*)_tmp71,accum);
# 183
return accum;}
# 186
void*Cyc_Dict_fold(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d,void*accum){
# 187
const struct Cyc_Dict_T*_tmp75=d.t;
# 188
if(_tmp75 == (const struct Cyc_Dict_T*)0)
# 189
return accum;
# 190
return Cyc_Dict_fold_tree(f,(const struct Cyc_Dict_T*)_tmp75,accum);}
# 193
static void*Cyc_Dict_fold_tree_c(void*(*f)(void*,void*,void*,void*),void*env,
const struct Cyc_Dict_T*t,void*accum){
# 195
struct Cyc_Dict_T _tmp77;const struct Cyc_Dict_T*_tmp78;const struct Cyc_Dict_T*
_tmp79;struct _tuple0 _tmp7A;void*_tmp7B;void*_tmp7C;const struct Cyc_Dict_T*_tmp76=
t;_tmp77=*_tmp76;_tmp78=_tmp77.left;_tmp79=_tmp77.right;_tmp7A=_tmp77.key_val;
_tmp7B=_tmp7A.f1;_tmp7C=_tmp7A.f2;
# 196
if(_tmp78 != (const struct Cyc_Dict_T*)0)
# 197
accum=Cyc_Dict_fold_tree_c(f,env,(const struct Cyc_Dict_T*)_tmp78,accum);
# 198
accum=f(env,_tmp7B,_tmp7C,accum);
# 199
if(_tmp79 != (const struct Cyc_Dict_T*)0)
# 200
accum=Cyc_Dict_fold_tree_c(f,env,(const struct Cyc_Dict_T*)_tmp79,accum);
# 201
return accum;}
# 204
void*Cyc_Dict_fold_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict
d,void*accum){
# 205
const struct Cyc_Dict_T*_tmp7D=d.t;
# 206
if(_tmp7D == (const struct Cyc_Dict_T*)0)
# 207
return accum;
# 208
return Cyc_Dict_fold_tree_c(f,env,(const struct Cyc_Dict_T*)_tmp7D,accum);}
# 211
static void Cyc_Dict_app_tree(void*(*f)(void*,void*),const struct Cyc_Dict_T*t){
# 212
struct Cyc_Dict_T _tmp7F;const struct Cyc_Dict_T*_tmp80;const struct Cyc_Dict_T*
_tmp81;struct _tuple0 _tmp82;void*_tmp83;void*_tmp84;const struct Cyc_Dict_T*_tmp7E=
t;_tmp7F=*_tmp7E;_tmp80=_tmp7F.left;_tmp81=_tmp7F.right;_tmp82=_tmp7F.key_val;
_tmp83=_tmp82.f1;_tmp84=_tmp82.f2;
# 213
if(_tmp80 != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree(f,(const struct Cyc_Dict_T*)
_tmp80);
# 214
f(_tmp83,_tmp84);
# 215
if(_tmp81 != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree(f,(const struct Cyc_Dict_T*)
_tmp81);}
# 218
void Cyc_Dict_app(void*(*f)(void*,void*),struct Cyc_Dict_Dict d){
# 219
const struct Cyc_Dict_T*_tmp85=d.t;
# 220
if(_tmp85 != (const struct Cyc_Dict_T*)0)
# 221
Cyc_Dict_app_tree(f,(const struct Cyc_Dict_T*)_tmp85);}
# 224
static void Cyc_Dict_app_tree_c(void*(*f)(void*,void*,void*),void*env,const struct
Cyc_Dict_T*t){
# 225
struct Cyc_Dict_T _tmp87;const struct Cyc_Dict_T*_tmp88;const struct Cyc_Dict_T*
_tmp89;struct _tuple0 _tmp8A;void*_tmp8B;void*_tmp8C;const struct Cyc_Dict_T*_tmp86=
t;_tmp87=*_tmp86;_tmp88=_tmp87.left;_tmp89=_tmp87.right;_tmp8A=_tmp87.key_val;
_tmp8B=_tmp8A.f1;_tmp8C=_tmp8A.f2;
# 226
if(_tmp88 != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree_c(f,env,(const struct Cyc_Dict_T*)
_tmp88);
# 227
f(env,_tmp8B,_tmp8C);
# 228
if(_tmp89 != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree_c(f,env,(const struct Cyc_Dict_T*)
_tmp89);}
# 231
void Cyc_Dict_app_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d){
# 232
const struct Cyc_Dict_T*_tmp8D=d.t;
# 233
if(_tmp8D != (const struct Cyc_Dict_T*)0)
# 234
Cyc_Dict_app_tree_c(f,env,(const struct Cyc_Dict_T*)_tmp8D);}
# 237
static void Cyc_Dict_iter_tree(void(*f)(void*,void*),const struct Cyc_Dict_T*t){
# 238
struct Cyc_Dict_T _tmp8F;const struct Cyc_Dict_T*_tmp90;const struct Cyc_Dict_T*
_tmp91;struct _tuple0 _tmp92;void*_tmp93;void*_tmp94;const struct Cyc_Dict_T*_tmp8E=
t;_tmp8F=*_tmp8E;_tmp90=_tmp8F.left;_tmp91=_tmp8F.right;_tmp92=_tmp8F.key_val;
_tmp93=_tmp92.f1;_tmp94=_tmp92.f2;
# 239
if(_tmp90 != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree(f,(const struct Cyc_Dict_T*)
_tmp90);
# 240
f(_tmp93,_tmp94);
# 241
if(_tmp91 != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree(f,(const struct Cyc_Dict_T*)
_tmp91);}
# 244
void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict d){
# 245
const struct Cyc_Dict_T*_tmp95=d.t;
# 246
if(_tmp95 != (const struct Cyc_Dict_T*)0)
# 247
Cyc_Dict_iter_tree(f,(const struct Cyc_Dict_T*)_tmp95);}
# 250
static void Cyc_Dict_iter_tree_c(void(*f)(void*,void*,void*),void*env,const struct
Cyc_Dict_T*t){
# 251
struct Cyc_Dict_T _tmp97;const struct Cyc_Dict_T*_tmp98;const struct Cyc_Dict_T*
_tmp99;struct _tuple0 _tmp9A;void*_tmp9B;void*_tmp9C;const struct Cyc_Dict_T*_tmp96=
t;_tmp97=*_tmp96;_tmp98=_tmp97.left;_tmp99=_tmp97.right;_tmp9A=_tmp97.key_val;
_tmp9B=_tmp9A.f1;_tmp9C=_tmp9A.f2;
# 252
if(_tmp98 != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree_c(f,env,(const struct
Cyc_Dict_T*)_tmp98);
# 253
f(env,_tmp9B,_tmp9C);
# 254
if(_tmp99 != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree_c(f,env,(const struct
Cyc_Dict_T*)_tmp99);}
# 257
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d){
# 258
const struct Cyc_Dict_T*_tmp9D=d.t;
# 259
if(_tmp9D != (const struct Cyc_Dict_T*)0)
# 260
Cyc_Dict_iter_tree_c(f,env,(const struct Cyc_Dict_T*)_tmp9D);}
# 263
static void Cyc_Dict_count_elem(int*cnt,void*a,void*b){
# 264
*cnt=*cnt + 1;}
# 266
int Cyc_Dict_cardinality(struct Cyc_Dict_Dict d){
# 267
int num=0;
# 268
((void(*)(void(*f)(int*,void*,void*),int*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_Dict_count_elem,& num,d);
# 269
return num;}struct _tuple2{void(*f1)(void*,void*);struct Cyc_Dict_Dict f2;};
# 272
static void Cyc_Dict_iter2_f(struct _tuple2*env,void*a,void*b1){
# 274
struct _tuple2 _tmp9F;void(*_tmpA0)(void*,void*);struct Cyc_Dict_Dict _tmpA1;struct
_tuple2*_tmp9E=env;_tmp9F=*_tmp9E;_tmpA0=_tmp9F.f1;_tmpA1=_tmp9F.f2;
# 275
_tmpA0(b1,Cyc_Dict_lookup(_tmpA1,a));}
# 278
void Cyc_Dict_iter2(void(*f)(void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2){
# 281
struct _tuple2 _tmp152;struct _tuple2 _tmpA2=(_tmp152.f1=f,((_tmp152.f2=d2,_tmp152)));
# 282
((void(*)(void(*f)(struct _tuple2*,void*,void*),struct _tuple2*env,struct Cyc_Dict_Dict
d))Cyc_Dict_iter_c)(Cyc_Dict_iter2_f,& _tmpA2,d1);}struct _tuple3{void(*f1)(void*,
void*,void*);struct Cyc_Dict_Dict f2;void*f3;};
# 285
static void Cyc_Dict_iter2_c_f(struct _tuple3*env,void*a,void*b1){
# 287
struct _tuple3 _tmpA5;void(*_tmpA6)(void*,void*,void*);struct Cyc_Dict_Dict _tmpA7;
void*_tmpA8;struct _tuple3*_tmpA4=env;_tmpA5=*_tmpA4;_tmpA6=_tmpA5.f1;_tmpA7=
_tmpA5.f2;_tmpA8=_tmpA5.f3;
# 288
_tmpA6(_tmpA8,b1,Cyc_Dict_lookup(_tmpA7,a));}
# 291
void Cyc_Dict_iter2_c(void(*f)(void*,void*,void*),void*inner_env,struct Cyc_Dict_Dict
d1,struct Cyc_Dict_Dict d2){
# 294
struct _tuple3 _tmp153;struct _tuple3 _tmpA9=(_tmp153.f1=f,((_tmp153.f2=d2,((
_tmp153.f3=inner_env,_tmp153)))));
# 295
((void(*)(void(*f)(struct _tuple3*,void*,void*),struct _tuple3*env,struct Cyc_Dict_Dict
d))Cyc_Dict_iter_c)(Cyc_Dict_iter2_c_f,& _tmpA9,d1);}struct _tuple4{void*(*f1)(
void*,void*,void*,void*,void*);struct Cyc_Dict_Dict f2;void*f3;};
# 298
static void*Cyc_Dict_fold2_c_f(struct _tuple4*env,void*a,void*b1,void*accum){
# 302
struct _tuple4 _tmpAC;void*(*_tmpAD)(void*,void*,void*,void*,void*);struct Cyc_Dict_Dict
_tmpAE;void*_tmpAF;struct _tuple4*_tmpAB=env;_tmpAC=*_tmpAB;_tmpAD=_tmpAC.f1;
_tmpAE=_tmpAC.f2;_tmpAF=_tmpAC.f3;
# 303
return _tmpAD(_tmpAF,a,b1,Cyc_Dict_lookup(_tmpAE,a),accum);}
# 306
void*Cyc_Dict_fold2_c(void*(*f)(void*,void*,void*,void*,void*),void*inner_env,
struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2,void*accum){
# 310
struct _tuple4 _tmp154;struct _tuple4 _tmpB0=(_tmp154.f1=f,((_tmp154.f2=d2,((
_tmp154.f3=inner_env,_tmp154)))));
# 311
return((void*(*)(void*(*f)(struct _tuple4*,void*,void*,void*),struct _tuple4*env,
struct Cyc_Dict_Dict d,void*accum))Cyc_Dict_fold_c)(Cyc_Dict_fold2_c_f,& _tmpB0,d1,
accum);}
# 314
static const struct Cyc_Dict_T*Cyc_Dict_copy_tree(struct _RegionHandle*r2,const
struct Cyc_Dict_T*t){
# 315
if(t == (const struct Cyc_Dict_T*)0)return 0;else{
# 317
enum Cyc_Dict_Color _tmpB3;const struct Cyc_Dict_T*_tmpB4;const struct Cyc_Dict_T*
_tmpB5;struct _tuple0 _tmpB6;struct Cyc_Dict_T _tmpB2=*t;_tmpB3=_tmpB2.color;_tmpB4=
_tmpB2.left;_tmpB5=_tmpB2.right;_tmpB6=_tmpB2.key_val;{
# 318
const struct Cyc_Dict_T*_tmpB7=Cyc_Dict_copy_tree(r2,_tmpB4);
# 319
const struct Cyc_Dict_T*_tmpB8=Cyc_Dict_copy_tree(r2,_tmpB5);
# 320
struct Cyc_Dict_T*_tmp155;return(const struct Cyc_Dict_T*)((_tmp155=_region_malloc(
r2,sizeof(*_tmp155)),((_tmp155->color=_tmpB3,((_tmp155->left=_tmpB7,((_tmp155->right=
_tmpB8,((_tmp155->key_val=_tmpB6,_tmp155))))))))));};}}
# 324
struct Cyc_Dict_Dict Cyc_Dict_rcopy(struct _RegionHandle*r2,struct Cyc_Dict_Dict d){
# 325
struct Cyc_Dict_Dict _tmp156;return(_tmp156.rel=d.rel,((_tmp156.r=r2,((_tmp156.t=
Cyc_Dict_copy_tree(r2,d.t),_tmp156)))));}
# 328
struct Cyc_Dict_Dict Cyc_Dict_copy(struct Cyc_Dict_Dict d){
# 329
return Cyc_Dict_rcopy(Cyc_Core_heap_region,d);}
# 332
static const struct Cyc_Dict_T*Cyc_Dict_map_tree(struct _RegionHandle*r,void*(*f)(
void*),const struct Cyc_Dict_T*t){
# 334
struct Cyc_Dict_T _tmpBC;enum Cyc_Dict_Color _tmpBD;const struct Cyc_Dict_T*_tmpBE;
const struct Cyc_Dict_T*_tmpBF;struct _tuple0 _tmpC0;void*_tmpC1;void*_tmpC2;const
struct Cyc_Dict_T*_tmpBB=t;_tmpBC=*_tmpBB;_tmpBD=_tmpBC.color;_tmpBE=_tmpBC.left;
_tmpBF=_tmpBC.right;_tmpC0=_tmpBC.key_val;_tmpC1=_tmpC0.f1;_tmpC2=_tmpC0.f2;{
# 335
const struct Cyc_Dict_T*_tmpC3=_tmpBE == (const struct Cyc_Dict_T*)0?(const struct Cyc_Dict_T*)
0: Cyc_Dict_map_tree(r,f,(const struct Cyc_Dict_T*)_tmpBE);
# 336
void*_tmpC4=f(_tmpC2);
# 337
const struct Cyc_Dict_T*_tmpC5=_tmpBF == (const struct Cyc_Dict_T*)0?(const struct Cyc_Dict_T*)
0: Cyc_Dict_map_tree(r,f,(const struct Cyc_Dict_T*)_tmpBF);
# 338
struct _tuple0 _tmp159;struct Cyc_Dict_T*_tmp158;return(const struct Cyc_Dict_T*)((
_tmp158=_region_malloc(r,sizeof(*_tmp158)),((_tmp158->color=_tmpBD,((_tmp158->left=
_tmpC3,((_tmp158->right=_tmpC5,((_tmp158->key_val=((_tmp159.f1=_tmpC1,((_tmp159.f2=
_tmpC4,_tmp159)))),_tmp158))))))))));};}
# 341
struct Cyc_Dict_Dict Cyc_Dict_rmap(struct _RegionHandle*r,void*(*f)(void*),struct
Cyc_Dict_Dict d){
# 342
const struct Cyc_Dict_T*_tmpC8=d.t;
# 343
if(_tmpC8 == (const struct Cyc_Dict_T*)0){
# 344
struct Cyc_Dict_Dict _tmp15A;return(_tmp15A.rel=d.rel,((_tmp15A.r=r,((_tmp15A.t=0,
_tmp15A)))));}{
# 345
struct Cyc_Dict_Dict _tmp15B;return(_tmp15B.rel=d.rel,((_tmp15B.r=r,((_tmp15B.t=
Cyc_Dict_map_tree(r,f,(const struct Cyc_Dict_T*)_tmpC8),_tmp15B)))));};}
# 348
struct Cyc_Dict_Dict Cyc_Dict_map(void*(*f)(void*),struct Cyc_Dict_Dict d){
# 349
return Cyc_Dict_rmap(Cyc_Core_heap_region,f,d);}
# 352
static const struct Cyc_Dict_T*Cyc_Dict_map_tree_c(struct _RegionHandle*r,void*(*f)(
void*,void*),void*env,const struct Cyc_Dict_T*t){
# 354
struct Cyc_Dict_T _tmpCC;enum Cyc_Dict_Color _tmpCD;const struct Cyc_Dict_T*_tmpCE;
const struct Cyc_Dict_T*_tmpCF;struct _tuple0 _tmpD0;void*_tmpD1;void*_tmpD2;const
struct Cyc_Dict_T*_tmpCB=t;_tmpCC=*_tmpCB;_tmpCD=_tmpCC.color;_tmpCE=_tmpCC.left;
_tmpCF=_tmpCC.right;_tmpD0=_tmpCC.key_val;_tmpD1=_tmpD0.f1;_tmpD2=_tmpD0.f2;{
# 355
const struct Cyc_Dict_T*_tmpD3=
# 356
_tmpCE == (const struct Cyc_Dict_T*)0?(const struct Cyc_Dict_T*)0: Cyc_Dict_map_tree_c(
r,f,env,(const struct Cyc_Dict_T*)_tmpCE);
# 357
void*_tmpD4=f(env,_tmpD2);
# 358
const struct Cyc_Dict_T*_tmpD5=
# 359
_tmpCF == (const struct Cyc_Dict_T*)0?(const struct Cyc_Dict_T*)0: Cyc_Dict_map_tree_c(
r,f,env,(const struct Cyc_Dict_T*)_tmpCF);
# 360
struct _tuple0 _tmp15E;struct Cyc_Dict_T*_tmp15D;return(const struct Cyc_Dict_T*)((
_tmp15D=_region_malloc(r,sizeof(*_tmp15D)),((_tmp15D->color=_tmpCD,((_tmp15D->left=
_tmpD3,((_tmp15D->right=_tmpD5,((_tmp15D->key_val=((_tmp15E.f1=_tmpD1,((_tmp15E.f2=
_tmpD4,_tmp15E)))),_tmp15D))))))))));};}
# 363
struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),
void*env,struct Cyc_Dict_Dict d){
# 365
const struct Cyc_Dict_T*_tmpD8=d.t;
# 366
if(_tmpD8 == (const struct Cyc_Dict_T*)0){
# 367
struct Cyc_Dict_Dict _tmp15F;return(_tmp15F.rel=d.rel,((_tmp15F.r=r,((_tmp15F.t=0,
_tmp15F)))));}{
# 368
struct Cyc_Dict_Dict _tmp160;return(_tmp160.rel=d.rel,((_tmp160.r=r,((_tmp160.t=
Cyc_Dict_map_tree_c(r,f,env,(const struct Cyc_Dict_T*)_tmpD8),_tmp160)))));};}
# 371
struct Cyc_Dict_Dict Cyc_Dict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict
d){
# 372
return Cyc_Dict_rmap_c(Cyc_Core_heap_region,f,env,d);}
# 375
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d){
# 376
if(d.t == (const struct Cyc_Dict_T*)0)
# 377
(int)_throw((void*)& Cyc_Dict_Absent_val);{
# 378
struct _tuple0*_tmp161;return(_tmp161=_region_malloc(r,sizeof(*_tmp161)),((
_tmp161->f1=((d.t)->key_val).f1,((_tmp161->f2=((d.t)->key_val).f2,_tmp161)))));};}
# 381
static int Cyc_Dict_forall_tree_c(int(*f)(void*,void*,void*),void*env,const struct
Cyc_Dict_T*t){
# 382
struct Cyc_Dict_T _tmpDD;const struct Cyc_Dict_T*_tmpDE;const struct Cyc_Dict_T*
_tmpDF;struct _tuple0 _tmpE0;void*_tmpE1;void*_tmpE2;const struct Cyc_Dict_T*_tmpDC=
t;_tmpDD=*_tmpDC;_tmpDE=_tmpDD.left;_tmpDF=_tmpDD.right;_tmpE0=_tmpDD.key_val;
_tmpE1=_tmpE0.f1;_tmpE2=_tmpE0.f2;
# 383
return
# 384
((_tmpDE == (const struct Cyc_Dict_T*)0  || Cyc_Dict_forall_tree_c(f,env,(const
struct Cyc_Dict_T*)_tmpDE)) && 
# 385
f(env,_tmpE1,_tmpE2)) && (
# 386
_tmpDF == (const struct Cyc_Dict_T*)0  || Cyc_Dict_forall_tree_c(f,env,(const struct
Cyc_Dict_T*)_tmpDF));}
# 389
int Cyc_Dict_forall_c(int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d){
# 390
const struct Cyc_Dict_T*_tmpE3=d.t;
# 391
if(_tmpE3 == (const struct Cyc_Dict_T*)0)
# 392
return 1;
# 393
return Cyc_Dict_forall_tree_c(f,env,(const struct Cyc_Dict_T*)_tmpE3);}struct
_tuple5{int(*f1)(void*,void*,void*);struct Cyc_Dict_Dict f2;};
# 396
static int Cyc_Dict_forall_intersect_f(struct _tuple5*env,void*a,void*b){
# 398
struct _tuple5 _tmpE5;int(*_tmpE6)(void*,void*,void*);struct Cyc_Dict_Dict _tmpE7;
struct _tuple5*_tmpE4=env;_tmpE5=*_tmpE4;_tmpE6=_tmpE5.f1;_tmpE7=_tmpE5.f2;
# 399
if(Cyc_Dict_member(_tmpE7,a))
# 400
return _tmpE6(a,b,Cyc_Dict_lookup(_tmpE7,a));
# 401
return 1;}
# 405
int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,
struct Cyc_Dict_Dict d2){
# 406
struct _tuple5 _tmp162;struct _tuple5 _tmpE8=(_tmp162.f1=f,((_tmp162.f2=d2,_tmp162)));
# 407
return((int(*)(int(*f)(struct _tuple5*,void*,void*),struct _tuple5*env,struct Cyc_Dict_Dict
d))Cyc_Dict_forall_c)(Cyc_Dict_forall_intersect_f,& _tmpE8,d1);}struct _tuple6{
void*(*f1)(void*,void*,void*,void*);void*f2;};
# 410
static struct Cyc_Dict_Dict*Cyc_Dict_union_f(struct _tuple6*env,void*a,void*b,
struct Cyc_Dict_Dict*d1){
# 414
if(Cyc_Dict_member(*d1,a)){
# 416
void*_tmpEA=Cyc_Dict_lookup(*d1,a);
# 417
void*_tmpEB=((*env).f1)((*env).f2,a,_tmpEA,b);
# 418
if(_tmpEB != _tmpEA)
# 419
*d1=Cyc_Dict_insert(*d1,a,_tmpEB);
# 420
return d1;}
# 422
*d1=Cyc_Dict_insert(*d1,a,b);
# 423
return d1;}
# 426
struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 429
if((int)d1.t == (int)d2.t)return d1;
# 430
if(d1.t == (const struct Cyc_Dict_T*)0)return d2;
# 431
if(d2.t == (const struct Cyc_Dict_T*)0)return d1;{
# 432
struct _tuple6 _tmp163;struct _tuple6 _tmpEC=(_tmp163.f1=f,((_tmp163.f2=env,_tmp163)));
# 433
((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*f)(struct _tuple6*,void*,void*,
struct Cyc_Dict_Dict*),struct _tuple6*env,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
accum))Cyc_Dict_fold_c)(Cyc_Dict_union_f,& _tmpEC,d2,& d1);
# 434
return d1;};}struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,
void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);static void _tmp168(
const struct Cyc_Dict_T**_tmpEE,unsigned int*_tmp167,unsigned int*_tmp166,const
struct Cyc_Dict_T***_tmp165){for(*_tmp167=0;*_tmp167 < *_tmp166;(*_tmp167)++){(*
_tmp165)[*_tmp167]=(const struct Cyc_Dict_T*)*_tmpEE;}}static void _tmp16E(struct
_dyneither_ptr*queue,const struct Cyc_Dict_T**_tmpEE,unsigned int*_tmp16D,
unsigned int*_tmp16C,const struct Cyc_Dict_T***_tmp16A){for(*_tmp16D=0;*_tmp16D < *
_tmp16C;(*_tmp16D)++){(*_tmp16A)[*_tmp16D]=
# 452
*_tmp16D < _get_dyneither_size(*queue,sizeof(const struct Cyc_Dict_T*))?((const
struct Cyc_Dict_T**)(*queue).curr)[(int)*_tmp16D]:(const struct Cyc_Dict_T*)*_tmpEE;}}
# 437
struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),void*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 440
const struct Cyc_Dict_T*_tmpEE=d2.t;
# 441
if(_tmpEE == (const struct Cyc_Dict_T*)0)return d2;
# 442
if((int)d1.t == (int)_tmpEE)return d2;{
# 443
const struct Cyc_Dict_T*ans_tree=0;
# 444
struct _RegionHandle _tmpEF=_new_region("temp");struct _RegionHandle*temp=& _tmpEF;
_push_region(temp);{
# 446
unsigned int _tmp167;unsigned int _tmp166;const struct Cyc_Dict_T**_tmp165;
unsigned int _tmp164;struct _dyneither_ptr queue=_tag_dyneither(((_tmp164=(
unsigned int)16,((_tmp165=(const struct Cyc_Dict_T**)_region_malloc(temp,
_check_times(sizeof(const struct Cyc_Dict_T*),_tmp164)),((((_tmp166=_tmp164,
_tmp168(& _tmpEE,& _tmp167,& _tmp166,& _tmp165))),_tmp165)))))),sizeof(const struct
Cyc_Dict_T*),(unsigned int)16);
# 447
int ind=0;
# 448
while(ind != - 1){
# 449
struct Cyc_Dict_T _tmpF1;const struct Cyc_Dict_T*_tmpF2;const struct Cyc_Dict_T*
_tmpF3;struct _tuple0 _tmpF4;void*_tmpF5;void*_tmpF6;const struct Cyc_Dict_T*_tmpF0=*((
const struct Cyc_Dict_T**)_check_dyneither_subscript(queue,sizeof(const struct Cyc_Dict_T*),
ind --));_tmpF1=*_tmpF0;_tmpF2=_tmpF1.left;_tmpF3=_tmpF1.right;_tmpF4=_tmpF1.key_val;
_tmpF5=_tmpF4.f1;_tmpF6=_tmpF4.f2;
# 450
if(ind + 2 >= _get_dyneither_size(queue,sizeof(const struct Cyc_Dict_T*))){
# 451
unsigned int _tmp16D;unsigned int _tmp16C;struct _dyneither_ptr _tmp16B;const struct
Cyc_Dict_T**_tmp16A;unsigned int _tmp169;queue=((_tmp169=_get_dyneither_size(
queue,sizeof(const struct Cyc_Dict_T*))* 2,((_tmp16A=(const struct Cyc_Dict_T**)
_region_malloc(temp,_check_times(sizeof(const struct Cyc_Dict_T*),_tmp169)),((
_tmp16B=_tag_dyneither(_tmp16A,sizeof(const struct Cyc_Dict_T*),_tmp169),((((
_tmp16C=_tmp169,_tmp16E(& queue,& _tmpEE,& _tmp16D,& _tmp16C,& _tmp16A))),_tmp16B))))))));}
# 453
if(_tmpF2 != (const struct Cyc_Dict_T*)0)*((const struct Cyc_Dict_T**)
_check_dyneither_subscript(queue,sizeof(const struct Cyc_Dict_T*),++ ind))=(const
struct Cyc_Dict_T*)_tmpF2;
# 454
if(_tmpF3 != (const struct Cyc_Dict_T*)0)*((const struct Cyc_Dict_T**)
_check_dyneither_subscript(queue,sizeof(const struct Cyc_Dict_T*),++ ind))=(const
struct Cyc_Dict_T*)_tmpF3;
# 455
if(Cyc_Dict_member(d1,_tmpF5)){
# 456
struct _tuple0 _tmp16F;ans_tree=Cyc_Dict_ins(d2.r,d2.rel,((_tmp16F.f1=_tmpF5,((
_tmp16F.f2=f(env,_tmpF5,Cyc_Dict_lookup(d1,_tmpF5),_tmpF6),_tmp16F)))),ans_tree);}}}{
# 459
struct Cyc_Dict_Dict _tmp170;struct Cyc_Dict_Dict _tmp100=(_tmp170.rel=d2.rel,((
_tmp170.r=d2.r,((_tmp170.t=ans_tree,_tmp170)))));_npop_handler(0);return _tmp100;};
# 444
;_pop_region(temp);};}
# 462
static void*Cyc_Dict_intersect_f(void*(*f)(void*,void*,void*),void*a,void*b1,
void*b2){
# 463
return f(a,b1,b2);}
# 466
struct Cyc_Dict_Dict Cyc_Dict_intersect(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict
d1,struct Cyc_Dict_Dict d2){
# 469
return((struct Cyc_Dict_Dict(*)(void*(*f)(void*(*)(void*,void*,void*),void*,void*,
void*),void*(*env)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_intersect_c)(Cyc_Dict_intersect_f,f,d1,d2);}
# 472
static struct Cyc_List_List*Cyc_Dict_to_list_f(struct _RegionHandle*r,void*k,void*
v,struct Cyc_List_List*accum){
# 474
struct _tuple0*_tmp173;struct Cyc_List_List*_tmp172;return(_tmp172=_region_malloc(
r,sizeof(*_tmp172)),((_tmp172->hd=((_tmp173=_region_malloc(r,sizeof(*_tmp173)),((
_tmp173->f1=k,((_tmp173->f2=v,_tmp173)))))),((_tmp172->tl=accum,_tmp172)))));}
# 477
struct Cyc_List_List*Cyc_Dict_rto_list(struct _RegionHandle*r,struct Cyc_Dict_Dict
d){
# 478
return((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _RegionHandle*,
void*,void*,struct Cyc_List_List*),struct _RegionHandle*env,struct Cyc_Dict_Dict d,
struct Cyc_List_List*accum))Cyc_Dict_fold_c)(Cyc_Dict_to_list_f,r,d,0);}
# 480
struct Cyc_List_List*Cyc_Dict_to_list(struct Cyc_Dict_Dict d){
# 481
return Cyc_Dict_rto_list(Cyc_Core_heap_region,d);}struct _tuple7{int(*f1)(void*,
void*);struct _RegionHandle*f2;};
# 484
static struct Cyc_Dict_Dict*Cyc_Dict_filter_f(struct _tuple7*env,void*x,void*y,
struct Cyc_Dict_Dict*acc){
# 487
struct _tuple7 _tmp104;int(*_tmp105)(void*,void*);struct _RegionHandle*_tmp106;
struct _tuple7*_tmp103=env;_tmp104=*_tmp103;_tmp105=_tmp104.f1;_tmp106=_tmp104.f2;
# 488
if(_tmp105(x,y))
# 489
*acc=Cyc_Dict_insert(*acc,x,y);
# 491
return acc;}
# 494
struct Cyc_Dict_Dict Cyc_Dict_rfilter(struct _RegionHandle*r2,int(*f)(void*,void*),
struct Cyc_Dict_Dict d){
# 496
struct _tuple7 _tmp174;struct _tuple7 _tmp107=(_tmp174.f1=f,((_tmp174.f2=r2,_tmp174)));
# 497
struct Cyc_Dict_Dict _tmp108=Cyc_Dict_rempty(r2,d.rel);
# 498
return*((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*f)(struct _tuple7*,void*,
void*,struct Cyc_Dict_Dict*),struct _tuple7*env,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
accum))Cyc_Dict_fold_c)(Cyc_Dict_filter_f,& _tmp107,d,& _tmp108);}
# 506
struct Cyc_Dict_Dict Cyc_Dict_filter(int(*f)(void*,void*),struct Cyc_Dict_Dict d){
# 507
return Cyc_Dict_rfilter(Cyc_Core_heap_region,f,d);}struct _tuple8{int(*f1)(void*,
void*,void*);void*f2;struct _RegionHandle*f3;};
# 511
static struct Cyc_Dict_Dict*Cyc_Dict_filter_c_f(struct _tuple8*env,void*x,void*y,
struct Cyc_Dict_Dict*acc){
# 515
struct _tuple8 _tmp10B;int(*_tmp10C)(void*,void*,void*);void*_tmp10D;struct
_RegionHandle*_tmp10E;struct _tuple8*_tmp10A=env;_tmp10B=*_tmp10A;_tmp10C=_tmp10B.f1;
_tmp10D=_tmp10B.f2;_tmp10E=_tmp10B.f3;
# 516
if(_tmp10C(_tmp10D,x,y))
# 517
*acc=Cyc_Dict_insert(*acc,x,y);
# 518
return acc;}
# 521
struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(struct _RegionHandle*r2,int(*f)(void*,void*,
void*),void*f_env,struct Cyc_Dict_Dict d){
# 523
struct _tuple8 _tmp175;struct _tuple8 _tmp10F=(_tmp175.f1=f,((_tmp175.f2=f_env,((
_tmp175.f3=r2,_tmp175)))));
# 524
struct Cyc_Dict_Dict _tmp110=Cyc_Dict_rempty(r2,d.rel);
# 525
return*((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*f)(struct _tuple8*,void*,
void*,struct Cyc_Dict_Dict*),struct _tuple8*env,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
accum))Cyc_Dict_fold_c)(Cyc_Dict_filter_c_f,& _tmp10F,d,& _tmp110);}
# 527
struct Cyc_Dict_Dict Cyc_Dict_filter_c(int(*f)(void*,void*,void*),void*f_env,
struct Cyc_Dict_Dict d){
# 529
return Cyc_Dict_rfilter_c(Cyc_Core_heap_region,f,f_env,d);}
# 533
static int Cyc_Dict_difference_f(struct Cyc_Dict_Dict*d,void*x,void*y){
# 534
return !Cyc_Dict_member(*d,x);}
# 537
struct Cyc_Dict_Dict Cyc_Dict_rdifference(struct _RegionHandle*r2,struct Cyc_Dict_Dict
d1,struct Cyc_Dict_Dict d2){
# 539
return((struct Cyc_Dict_Dict(*)(struct _RegionHandle*r2,int(*f)(struct Cyc_Dict_Dict*,
void*,void*),struct Cyc_Dict_Dict*f_env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(
r2,Cyc_Dict_difference_f,& d2,d1);}
# 541
struct Cyc_Dict_Dict Cyc_Dict_difference(struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2){
# 543
return Cyc_Dict_rdifference(Cyc_Core_heap_region,d1,d2);}struct _tuple9{int(*f1)(
void*,void*);void*f2;};
# 546
static int Cyc_Dict_delete_f(struct _tuple9*env,void*x,void*y){
# 547
struct _tuple9 _tmp113;int(*_tmp114)(void*,void*);void*_tmp115;struct _tuple9*
_tmp112=env;_tmp113=*_tmp112;_tmp114=_tmp113.f1;_tmp115=_tmp113.f2;
# 548
return _tmp114(_tmp115,x)!= 0;}
# 551
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*r2,struct Cyc_Dict_Dict d,
void*x){
# 552
if(!Cyc_Dict_member(d,x))return Cyc_Dict_rcopy(r2,d);{
# 553
struct _tuple9 _tmp176;struct _tuple9 _tmp116=(_tmp176.f1=d.rel,((_tmp176.f2=x,
_tmp176)));
# 554
return((struct Cyc_Dict_Dict(*)(struct _RegionHandle*r2,int(*f)(struct _tuple9*,
void*,void*),struct _tuple9*f_env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(r2,
Cyc_Dict_delete_f,& _tmp116,d);};}
# 557
struct Cyc_Dict_Dict Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict d,void*x){
# 558
if(!Cyc_Dict_member(d,x))return d;{
# 559
struct _tuple9 _tmp177;struct _tuple9 _tmp118=(_tmp177.f1=d.rel,((_tmp177.f2=x,
_tmp177)));
# 560
return((struct Cyc_Dict_Dict(*)(struct _RegionHandle*r2,int(*f)(struct _tuple9*,
void*,void*),struct _tuple9*f_env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(d.r,
Cyc_Dict_delete_f,& _tmp118,d);};}
# 563
struct Cyc_Dict_Dict Cyc_Dict_delete(struct Cyc_Dict_Dict d,void*x){
# 564
return Cyc_Dict_rdelete(Cyc_Core_heap_region,d,x);}struct _tuple10{struct
_dyneither_ptr f1;int f2;};
# 567
int Cyc_Dict_iter_f(struct _tuple10*stk,struct _tuple0*dest){
# 570
struct _tuple10 _tmp11B;struct _dyneither_ptr _tmp11C;int _tmp11D;int*_tmp11E;struct
_tuple10*_tmp11A=stk;_tmp11B=*_tmp11A;_tmp11C=_tmp11B.f1;_tmp11D=_tmp11B.f2;
_tmp11E=(int*)&(*_tmp11A).f2;{
# 571
int _tmp11F=*_tmp11E;
# 572
if(_tmp11F == - 1)
# 573
return 0;{
# 574
const struct Cyc_Dict_T*_tmp120=*((const struct Cyc_Dict_T**)
_check_dyneither_subscript(_tmp11C,sizeof(const struct Cyc_Dict_T*),_tmp11F));
# 575
*dest=((const struct Cyc_Dict_T*)_check_null(_tmp120))->key_val;
# 576
-- _tmp11F;
# 577
if((unsigned int)_tmp120->left)
# 578
*((const struct Cyc_Dict_T**)_check_dyneither_subscript(_tmp11C,sizeof(const
struct Cyc_Dict_T*),++ _tmp11F))=_tmp120->left;
# 579
if((unsigned int)_tmp120->right)
# 580
*((const struct Cyc_Dict_T**)_check_dyneither_subscript(_tmp11C,sizeof(const
struct Cyc_Dict_T*),++ _tmp11F))=_tmp120->right;
# 581
*_tmp11E=_tmp11F;
# 582
return 1;};};}struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,
struct Cyc_Dict_Dict d);static void _tmp17D(unsigned int*_tmp17C,unsigned int*
_tmp17B,const struct Cyc_Dict_T***_tmp179,const struct Cyc_Dict_T**_tmp121){for(*
_tmp17C=0;*_tmp17C < *_tmp17B;(*_tmp17C)++){(*_tmp179)[*_tmp17C]=*_tmp121;}}
# 585
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict
d){
# 590
int half_max_size=1;
# 591
const struct Cyc_Dict_T*_tmp121=d.t;
# 592
while(_tmp121 != (const struct Cyc_Dict_T*)0){
# 593
_tmp121=_tmp121->left;
# 594
++ half_max_size;}
# 596
_tmp121=d.t;{
# 597
unsigned int _tmp17C;unsigned int _tmp17B;struct _dyneither_ptr _tmp17A;const struct
Cyc_Dict_T**_tmp179;unsigned int _tmp178;struct _dyneither_ptr _tmp122=(_tmp178=(
unsigned int)(2 * half_max_size),((_tmp179=(const struct Cyc_Dict_T**)
_region_malloc(rgn,_check_times(sizeof(const struct Cyc_Dict_T*),_tmp178)),((
_tmp17A=_tag_dyneither(_tmp179,sizeof(const struct Cyc_Dict_T*),_tmp178),((((
_tmp17B=_tmp178,_tmp17D(& _tmp17C,& _tmp17B,& _tmp179,& _tmp121))),_tmp17A)))))));
# 598
struct _tuple10*_tmp180;struct Cyc_Iter_Iter _tmp17F;return(_tmp17F.env=(void*)((
_tmp180=_region_malloc(rgn,sizeof(*_tmp180)),((_tmp180->f1=_tmp122,((_tmp180->f2=(
unsigned int)_tmp121?0: - 1,_tmp180)))))),((_tmp17F.next=(int(*)(void*env,void*
dest))Cyc_Dict_iter_f,(struct Cyc_Iter_Iter)_tmp17F)));};}
# 606
void*Cyc_Dict_marshal(struct _RegionHandle*rgn,void*env,void*(*write_key)(void*,
struct Cyc___cycFILE*,void*),void*(*write_val)(void*,struct Cyc___cycFILE*,void*),
struct Cyc___cycFILE*fp,struct Cyc_Dict_Dict dict){
# 613
struct Cyc_List_List*dict_list=Cyc_Dict_rto_list(rgn,dict);
# 614
int len=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dict_list);
# 617
{struct Cyc_Core_Failure_exn_struct _tmp186;const char*_tmp185;struct Cyc_Core_Failure_exn_struct*
_tmp184;(int)_throw((void*)((_tmp184=_cycalloc(sizeof(*_tmp184)),((_tmp184[0]=((
_tmp186.tag=Cyc_Core_Failure,((_tmp186.f1=((_tmp185="Dict::marshal: Write failure",
_tag_dyneither(_tmp185,sizeof(char),29))),_tmp186)))),_tmp184)))));}
# 619
while(dict_list != 0){
# 620
env=((void*(*)(void*,struct Cyc___cycFILE*,struct _tuple0*))write_key)(env,fp,(
struct _tuple0*)dict_list->hd);
# 621
env=((void*(*)(void*,struct Cyc___cycFILE*,struct _tuple0*))write_val)(env,fp,(
struct _tuple0*)dict_list->hd);
# 622
dict_list=dict_list->tl;}
# 624
return env;}
# 627
struct Cyc_Dict_Dict Cyc_Dict_unmarshal(struct _RegionHandle*rgn,void*env,int(*cmp)(
void*,void*),void*(*read_key)(void*,struct Cyc___cycFILE*),void*(*read_val)(void*,
struct Cyc___cycFILE*),struct Cyc___cycFILE*fp){
# 634
struct Cyc_Dict_Dict dict=Cyc_Dict_empty(cmp);
# 635
int len=Cyc_getw(fp);
# 636
if(len == - 1){
# 637
struct Cyc_Core_Failure_exn_struct _tmp18C;const char*_tmp18B;struct Cyc_Core_Failure_exn_struct*
_tmp18A;(int)_throw((void*)((_tmp18A=_cycalloc(sizeof(*_tmp18A)),((_tmp18A[0]=((
_tmp18C.tag=Cyc_Core_Failure,((_tmp18C.f1=((_tmp18B="Dict::unmarshal: list length is -1",
_tag_dyneither(_tmp18B,sizeof(char),35))),_tmp18C)))),_tmp18A)))));}
# 639
{int i=0;for(0;i < len;++ i){
# 640
void*key=read_key(env,fp);
# 641
void*val=read_val(env,fp);
# 642
dict=Cyc_Dict_insert(dict,key,val);}}
# 644
return dict;}
