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
typedef struct Cyc___cycFILE Cyc_FILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 162 "core.h"
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
typedef struct Cyc_List_List*Cyc_List_List_t;extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 259
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
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
# 526
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 533
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 540
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 558
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1001 "absyn.h"
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 64 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 68
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 62
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
# 65
void*Cyc_Tcutil_compress(void*t);
# 101
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 150
int Cyc_Tcutil_unify(void*,void*);
# 152
int Cyc_Tcutil_typecmp(void*,void*);
# 155
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 338 "tcutil.h"
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);struct _tuple10{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 42
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 45
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 48
int Cyc_Evexp_okay_szofarg(void*t);struct _tuple11{union Cyc_Absyn_Cnst f1;int f2;};
# 41 "evexp.cyc"
static struct _tuple11 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*);
# 43
static union Cyc_Absyn_Cnst Cyc_Evexp_promote_const(union Cyc_Absyn_Cnst cn){
# 44
union Cyc_Absyn_Cnst _tmp0=cn;struct _tuple3 _tmp1;enum Cyc_Absyn_Sign _tmp2;char _tmp3;struct _tuple4 _tmp4;enum Cyc_Absyn_Sign _tmp5;short _tmp6;_LL1: if((_tmp0.Char_c).tag != 2)goto _LL3;_tmp1=(struct _tuple3)(_tmp0.Char_c).val;_tmp2=_tmp1.f1;_tmp3=_tmp1.f2;_LL2:
# 45
 return Cyc_Absyn_Int_c(_tmp2,(int)_tmp3);_LL3: if((_tmp0.Short_c).tag != 4)goto _LL5;_tmp4=(struct _tuple4)(_tmp0.Short_c).val;_tmp5=_tmp4.f1;_tmp6=_tmp4.f2;_LL4:
# 47
 return Cyc_Absyn_Int_c(_tmp5,(int)_tmp6);_LL5:;_LL6:
# 48
 return cn;_LL0:;}
# 52
struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e){
# 53
union Cyc_Absyn_Cnst _tmp8;int _tmp9;struct _tuple11 _tmp7=Cyc_Evexp_eval_const_exp(e);_tmp8=_tmp7.f1;_tmp9=_tmp7.f2;
# 54
if(!_tmp9){
# 55
struct _tuple10 _tmp1FF;return(_tmp1FF.f1=0,((_tmp1FF.f2=0,_tmp1FF)));}{
# 56
union Cyc_Absyn_Cnst _tmpB=Cyc_Evexp_promote_const(_tmp8);struct _tuple5 _tmpC;int _tmpD;_LL8: if((_tmpB.Int_c).tag != 5)goto _LLA;_tmpC=(struct _tuple5)(_tmpB.Int_c).val;_tmpD=_tmpC.f2;_LL9: {
# 57
struct _tuple10 _tmp200;return(_tmp200.f1=(unsigned int)_tmpD,((_tmp200.f2=1,_tmp200)));}_LLA:;_LLB:
# 58
{const char*_tmp203;void*_tmp202;(_tmp202=0,Cyc_Tcutil_terr(e->loc,((_tmp203="expecting unsigned int",_tag_dyneither(_tmp203,sizeof(char),23))),_tag_dyneither(_tmp202,sizeof(void*),0)));}{struct _tuple10 _tmp204;return(_tmp204.f1=0,((_tmp204.f2=1,_tmp204)));};_LL7:;};}struct _tuple12{int f1;int f2;};
# 62
static struct _tuple12 Cyc_Evexp_eval_const_bool_exp(struct Cyc_Absyn_Exp*e){
# 63
union Cyc_Absyn_Cnst _tmp13;int _tmp14;struct _tuple11 _tmp12=Cyc_Evexp_eval_const_exp(e);_tmp13=_tmp12.f1;_tmp14=_tmp12.f2;
# 64
if(!_tmp14){
# 65
struct _tuple12 _tmp205;return(_tmp205.f1=0,((_tmp205.f2=0,_tmp205)));}{
# 66
union Cyc_Absyn_Cnst _tmp16=Cyc_Evexp_promote_const(_tmp13);struct _tuple5 _tmp17;int _tmp18;struct _tuple6 _tmp19;long long _tmp1A;int _tmp1B;_LLD: if((_tmp16.Int_c).tag != 5)goto _LLF;_tmp17=(struct _tuple5)(_tmp16.Int_c).val;_tmp18=_tmp17.f2;_LLE: {
# 67
struct _tuple12 _tmp206;return(_tmp206.f1=_tmp18 != 0,((_tmp206.f2=1,_tmp206)));}_LLF: if((_tmp16.LongLong_c).tag != 6)goto _LL11;_tmp19=(struct _tuple6)(_tmp16.LongLong_c).val;_tmp1A=_tmp19.f2;_LL10: {
# 68
struct _tuple12 _tmp207;return(_tmp207.f1=_tmp1A != 0,((_tmp207.f2=1,_tmp207)));}_LL11: if((_tmp16.Null_c).tag != 1)goto _LL13;_tmp1B=(int)(_tmp16.Null_c).val;_LL12: {
# 69
struct _tuple12 _tmp208;return(_tmp208.f1=0,((_tmp208.f2=0,_tmp208)));}_LL13:;_LL14:
# 70
{const char*_tmp20B;void*_tmp20A;(_tmp20A=0,Cyc_Tcutil_terr(e->loc,((_tmp20B="expecting bool",_tag_dyneither(_tmp20B,sizeof(char),15))),_tag_dyneither(_tmp20A,sizeof(void*),0)));}{struct _tuple12 _tmp20C;return(_tmp20C.f1=0,((_tmp20C.f2=0,_tmp20C)));};_LLC:;};}struct _tuple13{enum Cyc_Absyn_Primop f1;union Cyc_Absyn_Cnst f2;};
# 74
static struct _tuple11 Cyc_Evexp_eval_const_unprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 75
union Cyc_Absyn_Cnst _tmp23;int _tmp24;struct _tuple11 _tmp22=Cyc_Evexp_eval_const_exp(e);_tmp23=_tmp22.f1;_tmp24=_tmp22.f2;
# 76
if(!_tmp24){
# 77
struct _tuple11 _tmp20D;return(_tmp20D.f1=_tmp23,((_tmp20D.f2=0,_tmp20D)));}
# 78
{struct _tuple13 _tmp20E;struct _tuple13 _tmp27=(_tmp20E.f1=p,((_tmp20E.f2=_tmp23,_tmp20E)));enum Cyc_Absyn_Primop _tmp28;enum Cyc_Absyn_Primop _tmp29;union Cyc_Absyn_Cnst _tmp2A;struct _tuple5 _tmp2B;enum Cyc_Absyn_Sign _tmp2C;int _tmp2D;enum Cyc_Absyn_Primop _tmp2E;union Cyc_Absyn_Cnst _tmp2F;struct _tuple5 _tmp30;enum Cyc_Absyn_Sign _tmp31;int _tmp32;enum Cyc_Absyn_Primop _tmp33;union Cyc_Absyn_Cnst _tmp34;struct _tuple5 _tmp35;int _tmp36;enum Cyc_Absyn_Primop _tmp37;union Cyc_Absyn_Cnst _tmp38;int _tmp39;_LL16: _tmp28=_tmp27.f1;if(_tmp28 != Cyc_Absyn_Plus)goto _LL18;_LL17:
# 79
 goto _LL15;_LL18: _tmp29=_tmp27.f1;if(_tmp29 != Cyc_Absyn_Minus)goto _LL1A;_tmp2A=_tmp27.f2;if((_tmp2A.Int_c).tag != 5)goto _LL1A;_tmp2B=(struct _tuple5)(_tmp2A.Int_c).val;_tmp2C=_tmp2B.f1;_tmp2D=_tmp2B.f2;_LL19:
# 80
 _tmp23=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,- _tmp2D);goto _LL15;_LL1A: _tmp2E=_tmp27.f1;if(_tmp2E != Cyc_Absyn_Bitnot)goto _LL1C;_tmp2F=_tmp27.f2;if((_tmp2F.Int_c).tag != 5)goto _LL1C;_tmp30=(struct _tuple5)(_tmp2F.Int_c).val;_tmp31=_tmp30.f1;_tmp32=_tmp30.f2;_LL1B:
# 81
 _tmp23=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,~ _tmp32);goto _LL15;_LL1C: _tmp33=_tmp27.f1;if(_tmp33 != Cyc_Absyn_Not)goto _LL1E;_tmp34=_tmp27.f2;if((_tmp34.Int_c).tag != 5)goto _LL1E;_tmp35=(struct _tuple5)(_tmp34.Int_c).val;_tmp36=_tmp35.f2;_LL1D:
# 82
 _tmp23=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,_tmp36 == 0?1: 0);goto _LL15;_LL1E: _tmp37=_tmp27.f1;if(_tmp37 != Cyc_Absyn_Not)goto _LL20;_tmp38=_tmp27.f2;if((_tmp38.Null_c).tag != 1)goto _LL20;_tmp39=(int)(_tmp38.Null_c).val;_LL1F:
# 83
 _tmp23=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,1);goto _LL15;_LL20:;_LL21:
# 84
{const char*_tmp211;void*_tmp210;(_tmp210=0,Cyc_Tcutil_terr(e->loc,((_tmp211="bad constant expression",_tag_dyneither(_tmp211,sizeof(char),24))),_tag_dyneither(_tmp210,sizeof(void*),0)));}goto _LL15;_LL15:;}{
# 86
struct _tuple11 _tmp212;return(_tmp212.f1=_tmp23,((_tmp212.f2=1,_tmp212)));};}struct _tuple14{enum Cyc_Absyn_Primop f1;int f2;};
# 90
static struct _tuple11 Cyc_Evexp_eval_const_binprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 91
union Cyc_Absyn_Cnst _tmp3E;int _tmp3F;struct _tuple11 _tmp3D=Cyc_Evexp_eval_const_exp(e1);_tmp3E=_tmp3D.f1;_tmp3F=_tmp3D.f2;{
# 92
union Cyc_Absyn_Cnst _tmp41;int _tmp42;struct _tuple11 _tmp40=Cyc_Evexp_eval_const_exp(e2);_tmp41=_tmp40.f1;_tmp42=_tmp40.f2;
# 93
if(!_tmp3F  || !_tmp42){
# 94
struct _tuple11 _tmp213;return(_tmp213.f1=_tmp3E,((_tmp213.f2=0,_tmp213)));}
# 95
_tmp3E=Cyc_Evexp_promote_const(_tmp3E);
# 96
_tmp41=Cyc_Evexp_promote_const(_tmp41);{
# 97
enum Cyc_Absyn_Sign s1;enum Cyc_Absyn_Sign s2;
# 98
int i1;int i2;
# 99
{union Cyc_Absyn_Cnst _tmp44=_tmp3E;struct _tuple5 _tmp45;enum Cyc_Absyn_Sign _tmp46;int _tmp47;_LL23: if((_tmp44.Int_c).tag != 5)goto _LL25;_tmp45=(struct _tuple5)(_tmp44.Int_c).val;_tmp46=_tmp45.f1;_tmp47=_tmp45.f2;_LL24:
# 100
 s1=_tmp46;i1=_tmp47;goto _LL22;_LL25:;_LL26:
# 101
{const char*_tmp216;void*_tmp215;(_tmp215=0,Cyc_Tcutil_terr(e1->loc,((_tmp216="bad constant expression",_tag_dyneither(_tmp216,sizeof(char),24))),_tag_dyneither(_tmp215,sizeof(void*),0)));}{struct _tuple11 _tmp217;return(_tmp217.f1=_tmp3E,((_tmp217.f2=1,_tmp217)));};_LL22:;}
# 103
{union Cyc_Absyn_Cnst _tmp4B=_tmp41;struct _tuple5 _tmp4C;enum Cyc_Absyn_Sign _tmp4D;int _tmp4E;_LL28: if((_tmp4B.Int_c).tag != 5)goto _LL2A;_tmp4C=(struct _tuple5)(_tmp4B.Int_c).val;_tmp4D=_tmp4C.f1;_tmp4E=_tmp4C.f2;_LL29:
# 104
 s2=_tmp4D;i2=_tmp4E;goto _LL27;_LL2A:;_LL2B:
# 105
{const char*_tmp21A;void*_tmp219;(_tmp219=0,Cyc_Tcutil_terr(e2->loc,((_tmp21A="bad constant expression",_tag_dyneither(_tmp21A,sizeof(char),24))),_tag_dyneither(_tmp219,sizeof(void*),0)));}{struct _tuple11 _tmp21B;return(_tmp21B.f1=_tmp3E,((_tmp21B.f2=1,_tmp21B)));};_LL27:;}
# 107
switch(p){case Cyc_Absyn_Div: _LL2C:
# 108
 goto _LL2D;case Cyc_Absyn_Mod: _LL2D:
# 110
 if(i2 == 0){
# 111
{const char*_tmp21E;void*_tmp21D;(_tmp21D=0,Cyc_Tcutil_terr(e2->loc,((_tmp21E="division by zero in constant expression",_tag_dyneither(_tmp21E,sizeof(char),40))),_tag_dyneither(_tmp21D,sizeof(void*),0)));}{
# 112
struct _tuple11 _tmp21F;return(_tmp21F.f1=_tmp3E,((_tmp21F.f2=1,_tmp21F)));};}
# 114
break;default: _LL2E:
# 115
 break;}{
# 117
int has_u_arg=s1 == Cyc_Absyn_Unsigned  || s2 == Cyc_Absyn_Unsigned;
# 118
unsigned int u1=(unsigned int)i1;
# 119
unsigned int u2=(unsigned int)i2;
# 120
int i3=0;
# 121
unsigned int u3=0;
# 122
int b3=1;
# 123
int use_i3=0;
# 124
int use_u3=0;
# 125
int use_b3=0;
# 126
{struct _tuple14 _tmp220;struct _tuple14 _tmp56=(_tmp220.f1=p,((_tmp220.f2=has_u_arg,_tmp220)));enum Cyc_Absyn_Primop _tmp57;int _tmp58;enum Cyc_Absyn_Primop _tmp59;int _tmp5A;enum Cyc_Absyn_Primop _tmp5B;int _tmp5C;enum Cyc_Absyn_Primop _tmp5D;int _tmp5E;enum Cyc_Absyn_Primop _tmp5F;int _tmp60;enum Cyc_Absyn_Primop _tmp61;int _tmp62;enum Cyc_Absyn_Primop _tmp63;int _tmp64;enum Cyc_Absyn_Primop _tmp65;int _tmp66;enum Cyc_Absyn_Primop _tmp67;int _tmp68;enum Cyc_Absyn_Primop _tmp69;int _tmp6A;enum Cyc_Absyn_Primop _tmp6B;enum Cyc_Absyn_Primop _tmp6C;enum Cyc_Absyn_Primop _tmp6D;int _tmp6E;enum Cyc_Absyn_Primop _tmp6F;int _tmp70;enum Cyc_Absyn_Primop _tmp71;int _tmp72;enum Cyc_Absyn_Primop _tmp73;int _tmp74;enum Cyc_Absyn_Primop _tmp75;int _tmp76;enum Cyc_Absyn_Primop _tmp77;int _tmp78;enum Cyc_Absyn_Primop _tmp79;int _tmp7A;enum Cyc_Absyn_Primop _tmp7B;int _tmp7C;enum Cyc_Absyn_Primop _tmp7D;enum Cyc_Absyn_Primop _tmp7E;enum Cyc_Absyn_Primop _tmp7F;enum Cyc_Absyn_Primop _tmp80;enum Cyc_Absyn_Primop _tmp81;enum Cyc_Absyn_Primop _tmp82;_LL31: _tmp57=_tmp56.f1;if(_tmp57 != Cyc_Absyn_Plus)goto _LL33;_tmp58=_tmp56.f2;if(_tmp58 != 0)goto _LL33;_LL32:
# 127
 i3=i1 + i2;use_i3=1;goto _LL30;_LL33: _tmp59=_tmp56.f1;if(_tmp59 != Cyc_Absyn_Times)goto _LL35;_tmp5A=_tmp56.f2;if(_tmp5A != 0)goto _LL35;_LL34:
# 128
 i3=i1 * i2;use_i3=1;goto _LL30;_LL35: _tmp5B=_tmp56.f1;if(_tmp5B != Cyc_Absyn_Minus)goto _LL37;_tmp5C=_tmp56.f2;if(_tmp5C != 0)goto _LL37;_LL36:
# 129
 i3=i1 - i2;use_i3=1;goto _LL30;_LL37: _tmp5D=_tmp56.f1;if(_tmp5D != Cyc_Absyn_Div)goto _LL39;_tmp5E=_tmp56.f2;if(_tmp5E != 0)goto _LL39;_LL38:
# 130
 i3=i1 / i2;use_i3=1;goto _LL30;_LL39: _tmp5F=_tmp56.f1;if(_tmp5F != Cyc_Absyn_Mod)goto _LL3B;_tmp60=_tmp56.f2;if(_tmp60 != 0)goto _LL3B;_LL3A:
# 131
 i3=i1 % i2;use_i3=1;goto _LL30;_LL3B: _tmp61=_tmp56.f1;if(_tmp61 != Cyc_Absyn_Plus)goto _LL3D;_tmp62=_tmp56.f2;if(_tmp62 != 1)goto _LL3D;_LL3C:
# 132
 u3=u1 + u2;use_u3=1;goto _LL30;_LL3D: _tmp63=_tmp56.f1;if(_tmp63 != Cyc_Absyn_Times)goto _LL3F;_tmp64=_tmp56.f2;if(_tmp64 != 1)goto _LL3F;_LL3E:
# 133
 u3=u1 * u2;use_u3=1;goto _LL30;_LL3F: _tmp65=_tmp56.f1;if(_tmp65 != Cyc_Absyn_Minus)goto _LL41;_tmp66=_tmp56.f2;if(_tmp66 != 1)goto _LL41;_LL40:
# 134
 u3=u1 - u2;use_u3=1;goto _LL30;_LL41: _tmp67=_tmp56.f1;if(_tmp67 != Cyc_Absyn_Div)goto _LL43;_tmp68=_tmp56.f2;if(_tmp68 != 1)goto _LL43;_LL42:
# 135
 u3=u1 / u2;use_u3=1;goto _LL30;_LL43: _tmp69=_tmp56.f1;if(_tmp69 != Cyc_Absyn_Mod)goto _LL45;_tmp6A=_tmp56.f2;if(_tmp6A != 1)goto _LL45;_LL44:
# 136
 u3=u1 % u2;use_u3=1;goto _LL30;_LL45: _tmp6B=_tmp56.f1;if(_tmp6B != Cyc_Absyn_Eq)goto _LL47;_LL46:
# 137
 b3=i1 == i2;use_b3=1;goto _LL30;_LL47: _tmp6C=_tmp56.f1;if(_tmp6C != Cyc_Absyn_Neq)goto _LL49;_LL48:
# 138
 b3=i1 != i2;use_b3=1;goto _LL30;_LL49: _tmp6D=_tmp56.f1;if(_tmp6D != Cyc_Absyn_Gt)goto _LL4B;_tmp6E=_tmp56.f2;if(_tmp6E != 0)goto _LL4B;_LL4A:
# 139
 b3=i1 > i2;use_b3=1;goto _LL30;_LL4B: _tmp6F=_tmp56.f1;if(_tmp6F != Cyc_Absyn_Lt)goto _LL4D;_tmp70=_tmp56.f2;if(_tmp70 != 0)goto _LL4D;_LL4C:
# 140
 b3=i1 < i2;use_b3=1;goto _LL30;_LL4D: _tmp71=_tmp56.f1;if(_tmp71 != Cyc_Absyn_Gte)goto _LL4F;_tmp72=_tmp56.f2;if(_tmp72 != 0)goto _LL4F;_LL4E:
# 141
 b3=i1 >= i2;use_b3=1;goto _LL30;_LL4F: _tmp73=_tmp56.f1;if(_tmp73 != Cyc_Absyn_Lte)goto _LL51;_tmp74=_tmp56.f2;if(_tmp74 != 0)goto _LL51;_LL50:
# 142
 b3=i1 <= i2;use_b3=1;goto _LL30;_LL51: _tmp75=_tmp56.f1;if(_tmp75 != Cyc_Absyn_Gt)goto _LL53;_tmp76=_tmp56.f2;if(_tmp76 != 1)goto _LL53;_LL52:
# 143
 b3=u1 > u2;use_b3=1;goto _LL30;_LL53: _tmp77=_tmp56.f1;if(_tmp77 != Cyc_Absyn_Lt)goto _LL55;_tmp78=_tmp56.f2;if(_tmp78 != 1)goto _LL55;_LL54:
# 144
 b3=u1 < u2;use_b3=1;goto _LL30;_LL55: _tmp79=_tmp56.f1;if(_tmp79 != Cyc_Absyn_Gte)goto _LL57;_tmp7A=_tmp56.f2;if(_tmp7A != 1)goto _LL57;_LL56:
# 145
 b3=u1 >= u2;use_b3=1;goto _LL30;_LL57: _tmp7B=_tmp56.f1;if(_tmp7B != Cyc_Absyn_Lte)goto _LL59;_tmp7C=_tmp56.f2;if(_tmp7C != 1)goto _LL59;_LL58:
# 146
 b3=u1 <= u2;use_b3=1;goto _LL30;_LL59: _tmp7D=_tmp56.f1;if(_tmp7D != Cyc_Absyn_Bitand)goto _LL5B;_LL5A:
# 147
 u3=u1 & u2;use_u3=1;goto _LL30;_LL5B: _tmp7E=_tmp56.f1;if(_tmp7E != Cyc_Absyn_Bitor)goto _LL5D;_LL5C:
# 148
 u3=u1 | u2;use_u3=1;goto _LL30;_LL5D: _tmp7F=_tmp56.f1;if(_tmp7F != Cyc_Absyn_Bitxor)goto _LL5F;_LL5E:
# 149
 u3=u1 ^ u2;use_u3=1;goto _LL30;_LL5F: _tmp80=_tmp56.f1;if(_tmp80 != Cyc_Absyn_Bitlshift)goto _LL61;_LL60:
# 150
 u3=u1 << u2;use_u3=1;goto _LL30;_LL61: _tmp81=_tmp56.f1;if(_tmp81 != Cyc_Absyn_Bitlrshift)goto _LL63;_LL62:
# 151
 u3=u1 >> u2;use_u3=1;goto _LL30;_LL63: _tmp82=_tmp56.f1;if(_tmp82 != Cyc_Absyn_Bitarshift)goto _LL65;_LL64:
# 152
{const char*_tmp223;void*_tmp222;(_tmp222=0,Cyc_Tcutil_terr(e1->loc,((_tmp223=">>> NOT IMPLEMENTED",_tag_dyneither(_tmp223,sizeof(char),20))),_tag_dyneither(_tmp222,sizeof(void*),0)));}goto _LL66;_LL65:;_LL66:
# 153
{const char*_tmp226;void*_tmp225;(_tmp225=0,Cyc_Tcutil_terr(e1->loc,((_tmp226="bad constant expression",_tag_dyneither(_tmp226,sizeof(char),24))),_tag_dyneither(_tmp225,sizeof(void*),0)));}{struct _tuple11 _tmp227;return(_tmp227.f1=_tmp3E,((_tmp227.f2=1,_tmp227)));};_LL30:;}
# 155
if(use_i3){struct _tuple11 _tmp228;return(_tmp228.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,i3),((_tmp228.f2=1,_tmp228)));}
# 156
if(use_u3){struct _tuple11 _tmp229;return(_tmp229.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,(int)u3),((_tmp229.f2=1,_tmp229)));}
# 157
if(use_b3){struct _tuple11 _tmp22A;return(_tmp22A.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,b3?1: 0),((_tmp22A.f2=1,_tmp22A)));}{
# 158
const char*_tmp22D;void*_tmp22C;(_tmp22C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp22D="Evexp::eval_const_binop",_tag_dyneither(_tmp22D,sizeof(char),24))),_tag_dyneither(_tmp22C,sizeof(void*),0)));};};};};}struct _tuple15{void*f1;union Cyc_Absyn_Cnst f2;};
# 165
static struct _tuple11 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*e){
# 166
struct _tuple11 ans;
# 167
{void*_tmp8D=e->r;union Cyc_Absyn_Cnst _tmp8F;struct Cyc_Absyn_Exp*_tmp91;struct Cyc_Absyn_Exp*_tmp92;struct Cyc_Absyn_Exp*_tmp93;struct Cyc_Absyn_Exp*_tmp95;struct Cyc_Absyn_Exp*_tmp96;struct Cyc_Absyn_Exp*_tmp98;struct Cyc_Absyn_Exp*_tmp99;enum Cyc_Absyn_Primop _tmp9B;struct Cyc_List_List*_tmp9C;void*_tmp9E;void*_tmpA3;struct Cyc_Absyn_Exp*_tmpA4;struct Cyc_Absyn_Enumfield*_tmpA6;struct Cyc_Absyn_Enumfield*_tmpA8;_LL68: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp8E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8D;if(_tmp8E->tag != 0)goto _LL6A;else{_tmp8F=_tmp8E->f1;}}_LL69: {
# 168
struct _tuple11 _tmp22E;return(_tmp22E.f1=_tmp8F,((_tmp22E.f2=1,_tmp22E)));}_LL6A: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp90=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8D;if(_tmp90->tag != 5)goto _LL6C;else{_tmp91=_tmp90->f1;_tmp92=_tmp90->f2;_tmp93=_tmp90->f3;}}_LL6B: {
# 170
int _tmpAB;int _tmpAC;struct _tuple12 _tmpAA=Cyc_Evexp_eval_const_bool_exp(_tmp91);_tmpAB=_tmpAA.f1;_tmpAC=_tmpAA.f2;
# 171
if(!_tmpAC){
# 172
Cyc_Evexp_eval_const_exp(_tmp92);
# 173
Cyc_Evexp_eval_const_exp(_tmp93);{
# 174
struct _tuple11 _tmp22F;return(_tmp22F.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp22F.f2=0,_tmp22F)));};}
# 176
ans=_tmpAB?Cyc_Evexp_eval_const_exp(_tmp92): Cyc_Evexp_eval_const_exp(_tmp93);
# 177
goto _LL67;}_LL6C: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp94=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8D;if(_tmp94->tag != 6)goto _LL6E;else{_tmp95=_tmp94->f1;_tmp96=_tmp94->f2;}}_LL6D: {
# 179
int _tmpAF;int _tmpB0;struct _tuple12 _tmpAE=Cyc_Evexp_eval_const_bool_exp(_tmp95);_tmpAF=_tmpAE.f1;_tmpB0=_tmpAE.f2;
# 180
if(!_tmpB0){
# 181
Cyc_Evexp_eval_const_exp(_tmp96);{
# 182
struct _tuple11 _tmp230;return(_tmp230.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp230.f2=0,_tmp230)));};}
# 184
if(_tmpAF)ans=Cyc_Evexp_eval_const_exp(_tmp96);else{
# 185
struct _tuple11 _tmp231;ans=((_tmp231.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp231.f2=1,_tmp231))));}
# 186
goto _LL67;}_LL6E: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp97=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8D;if(_tmp97->tag != 7)goto _LL70;else{_tmp98=_tmp97->f1;_tmp99=_tmp97->f2;}}_LL6F: {
# 188
int _tmpB4;int _tmpB5;struct _tuple12 _tmpB3=Cyc_Evexp_eval_const_bool_exp(_tmp98);_tmpB4=_tmpB3.f1;_tmpB5=_tmpB3.f2;
# 189
if(!_tmpB5){
# 190
Cyc_Evexp_eval_const_exp(_tmp99);{
# 191
struct _tuple11 _tmp232;return(_tmp232.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp232.f2=0,_tmp232)));};}
# 193
if(!_tmpB4)ans=Cyc_Evexp_eval_const_exp(_tmp99);else{
# 194
struct _tuple11 _tmp233;ans=((_tmp233.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,1),((_tmp233.f2=1,_tmp233))));}
# 195
goto _LL67;}_LL70: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp9A=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp8D;if(_tmp9A->tag != 2)goto _LL72;else{_tmp9B=_tmp9A->f1;_tmp9C=_tmp9A->f2;}}_LL71:
# 197
 if(_tmp9C == 0){
# 198
{const char*_tmp236;void*_tmp235;(_tmp235=0,Cyc_Tcutil_terr(e->loc,((_tmp236="bad static expression (no args to primop)",_tag_dyneither(_tmp236,sizeof(char),42))),_tag_dyneither(_tmp235,sizeof(void*),0)));}{
# 199
struct _tuple11 _tmp237;return(_tmp237.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp237.f2=1,_tmp237)));};}
# 201
if(_tmp9C->tl == 0){
# 202
ans=Cyc_Evexp_eval_const_unprimop(_tmp9B,(struct Cyc_Absyn_Exp*)_tmp9C->hd);
# 203
goto _LL67;}
# 205
if(((struct Cyc_List_List*)_check_null(_tmp9C->tl))->tl == 0){
# 206
ans=Cyc_Evexp_eval_const_binprimop(_tmp9B,(struct Cyc_Absyn_Exp*)_tmp9C->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp9C->tl))->hd);
# 207
goto _LL67;}
# 209
{const char*_tmp23A;void*_tmp239;(_tmp239=0,Cyc_Tcutil_terr(e->loc,((_tmp23A="bad static expression (too many args to primop)",_tag_dyneither(_tmp23A,sizeof(char),48))),_tag_dyneither(_tmp239,sizeof(void*),0)));}{
# 210
struct _tuple11 _tmp23B;return(_tmp23B.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp23B.f2=1,_tmp23B)));};_LL72: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp9D=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp8D;if(_tmp9D->tag != 37)goto _LL74;else{_tmp9E=(void*)_tmp9D->f1;}}_LL73:
# 213
{void*_tmpBE=Cyc_Tcutil_compress(_tmp9E);struct Cyc_Absyn_Exp*_tmpC0;_LL83: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpBF=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmpBE;if(_tmpBF->tag != 18)goto _LL85;else{_tmpC0=_tmpBF->f1;}}_LL84:
# 214
 return Cyc_Evexp_eval_const_exp(_tmpC0);_LL85:;_LL86:
# 215
 goto _LL82;_LL82:;}
# 217
goto _LL75;_LL74: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp9F=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp8D;if(_tmp9F->tag != 16)goto _LL76;}_LL75:
# 218
 goto _LL77;_LL76: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpA0=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp8D;if(_tmpA0->tag != 17)goto _LL78;}_LL77:
# 219
 goto _LL79;_LL78: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpA1=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp8D;if(_tmpA1->tag != 18)goto _LL7A;}_LL79:
# 220
{struct _tuple11 _tmp23C;ans=((_tmp23C.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp23C.f2=0,_tmp23C))));}goto _LL67;_LL7A: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpA2=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8D;if(_tmpA2->tag != 13)goto _LL7C;else{_tmpA3=(void*)_tmpA2->f1;_tmpA4=_tmpA2->f2;}}_LL7B:
# 223
 ans=Cyc_Evexp_eval_const_exp(_tmpA4);
# 224
if(ans.f2){
# 225
struct _tuple15 _tmp23D;struct _tuple15 _tmpC3=(_tmp23D.f1=Cyc_Tcutil_compress(_tmpA3),((_tmp23D.f2=ans.f1,_tmp23D)));void*_tmpC4;enum Cyc_Absyn_Sign _tmpC6;enum Cyc_Absyn_Size_of _tmpC7;union Cyc_Absyn_Cnst _tmpC8;struct _tuple3 _tmpC9;enum Cyc_Absyn_Sign _tmpCA;char _tmpCB;void*_tmpCC;enum Cyc_Absyn_Sign _tmpCE;enum Cyc_Absyn_Size_of _tmpCF;union Cyc_Absyn_Cnst _tmpD0;struct _tuple4 _tmpD1;enum Cyc_Absyn_Sign _tmpD2;short _tmpD3;void*_tmpD4;enum Cyc_Absyn_Sign _tmpD6;enum Cyc_Absyn_Size_of _tmpD7;union Cyc_Absyn_Cnst _tmpD8;struct _tuple5 _tmpD9;enum Cyc_Absyn_Sign _tmpDA;int _tmpDB;void*_tmpDC;void*_tmpDE;union Cyc_Absyn_Cnst _tmpDF;struct _tuple3 _tmpE0;enum Cyc_Absyn_Sign _tmpE1;char _tmpE2;void*_tmpE3;void*_tmpE5;union Cyc_Absyn_Cnst _tmpE6;struct _tuple4 _tmpE7;enum Cyc_Absyn_Sign _tmpE8;short _tmpE9;void*_tmpEA;void*_tmpEC;union Cyc_Absyn_Cnst _tmpED;struct _tuple5 _tmpEE;enum Cyc_Absyn_Sign _tmpEF;int _tmpF0;_LL88: _tmpC4=_tmpC3.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpC5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpC4;if(_tmpC5->tag != 6)goto _LL8A;else{_tmpC6=_tmpC5->f1;_tmpC7=_tmpC5->f2;}};_tmpC8=_tmpC3.f2;if((_tmpC8.Char_c).tag != 2)goto _LL8A;_tmpC9=(struct _tuple3)(_tmpC8.Char_c).val;_tmpCA=_tmpC9.f1;_tmpCB=_tmpC9.f2;_LL89:
# 227
 _tmpCE=_tmpC6;_tmpCF=_tmpC7;_tmpD2=_tmpCA;_tmpD3=(short)_tmpCB;goto _LL8B;_LL8A: _tmpCC=_tmpC3.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpCD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpCC;if(_tmpCD->tag != 6)goto _LL8C;else{_tmpCE=_tmpCD->f1;_tmpCF=_tmpCD->f2;}};_tmpD0=_tmpC3.f2;if((_tmpD0.Short_c).tag != 4)goto _LL8C;_tmpD1=(struct _tuple4)(_tmpD0.Short_c).val;_tmpD2=_tmpD1.f1;_tmpD3=_tmpD1.f2;_LL8B:
# 228
 _tmpD6=_tmpCE;_tmpD7=_tmpCF;_tmpDA=_tmpD2;_tmpDB=(int)_tmpD3;goto _LL8D;_LL8C: _tmpD4=_tmpC3.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpD5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD4;if(_tmpD5->tag != 6)goto _LL8E;else{_tmpD6=_tmpD5->f1;_tmpD7=_tmpD5->f2;}};_tmpD8=_tmpC3.f2;if((_tmpD8.Int_c).tag != 5)goto _LL8E;_tmpD9=(struct _tuple5)(_tmpD8.Int_c).val;_tmpDA=_tmpD9.f1;_tmpDB=_tmpD9.f2;_LL8D:
# 230
 if(_tmpD6 != _tmpDA)
# 231
ans.f1=Cyc_Absyn_Int_c(_tmpD6,_tmpDB);
# 232
goto _LL87;_LL8E: _tmpDC=_tmpC3.f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpDD=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpDC;if(_tmpDD->tag != 19)goto _LL90;else{_tmpDE=(void*)_tmpDD->f1;}};_tmpDF=_tmpC3.f2;if((_tmpDF.Char_c).tag != 2)goto _LL90;_tmpE0=(struct _tuple3)(_tmpDF.Char_c).val;_tmpE1=_tmpE0.f1;_tmpE2=_tmpE0.f2;_LL8F:
# 234
 _tmpE5=_tmpDE;_tmpE8=_tmpE1;_tmpE9=(short)_tmpE2;goto _LL91;_LL90: _tmpE3=_tmpC3.f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE4=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpE3;if(_tmpE4->tag != 19)goto _LL92;else{_tmpE5=(void*)_tmpE4->f1;}};_tmpE6=_tmpC3.f2;if((_tmpE6.Short_c).tag != 4)goto _LL92;_tmpE7=(struct _tuple4)(_tmpE6.Short_c).val;_tmpE8=_tmpE7.f1;_tmpE9=_tmpE7.f2;_LL91:
# 235
 _tmpEC=_tmpE5;_tmpEF=_tmpE8;_tmpF0=(int)_tmpE9;goto _LL93;_LL92: _tmpEA=_tmpC3.f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpEB=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpEA;if(_tmpEB->tag != 19)goto _LL94;else{_tmpEC=(void*)_tmpEB->f1;}};_tmpED=_tmpC3.f2;if((_tmpED.Int_c).tag != 5)goto _LL94;_tmpEE=(struct _tuple5)(_tmpED.Int_c).val;_tmpEF=_tmpEE.f1;_tmpF0=_tmpEE.f2;_LL93:
# 237
 if(_tmpF0 < 0){
# 238
const char*_tmp240;void*_tmp23F;(_tmp23F=0,Cyc_Tcutil_terr(e->loc,((_tmp240="cannot cast negative number to a tag type",_tag_dyneither(_tmp240,sizeof(char),42))),_tag_dyneither(_tmp23F,sizeof(void*),0)));}
# 239
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp243;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp242;Cyc_Tcutil_unify(_tmpEC,(void*)((_tmp242=_cycalloc(sizeof(*_tmp242)),((_tmp242[0]=((_tmp243.tag=18,((_tmp243.f1=Cyc_Absyn_const_exp(ans.f1,0),_tmp243)))),_tmp242)))));}
# 240
ans.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmpF0);
# 241
goto _LL87;_LL94:;_LL95:
# 242
{const char*_tmp247;void*_tmp246[1];struct Cyc_String_pa_PrintArg_struct _tmp245;(_tmp245.tag=0,((_tmp245.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpA3)),((_tmp246[0]=& _tmp245,Cyc_Tcutil_terr(e->loc,((_tmp247="cannot cast to %s",_tag_dyneither(_tmp247,sizeof(char),18))),_tag_dyneither(_tmp246,sizeof(void*),1)))))));}goto _LL87;_LL87:;}
# 244
goto _LL67;_LL7C: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpA5=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp8D;if(_tmpA5->tag != 31)goto _LL7E;else{_tmpA6=_tmpA5->f3;}}_LL7D:
# 245
 _tmpA8=_tmpA6;goto _LL7F;_LL7E: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpA7=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp8D;if(_tmpA7->tag != 30)goto _LL80;else{_tmpA8=_tmpA7->f3;}}_LL7F:
# 246
 ans=Cyc_Evexp_eval_const_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)_check_null(_tmpA8))->tag));goto _LL67;_LL80:;_LL81:
# 248
{const char*_tmp24B;void*_tmp24A[1];struct Cyc_String_pa_PrintArg_struct _tmp249;(_tmp249.tag=0,((_tmp249.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp24A[0]=& _tmp249,Cyc_Tcutil_terr(e->loc,((_tmp24B="bad static expression %s",_tag_dyneither(_tmp24B,sizeof(char),25))),_tag_dyneither(_tmp24A,sizeof(void*),1)))))));}{
# 249
struct _tuple11 _tmp24C;return(_tmp24C.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp24C.f2=1,_tmp24C)));};_LL67:;}
# 251
if(ans.f2){
# 252
void*c;
# 253
union Cyc_Absyn_Cnst _tmpFC=ans.f1;
# 254
{union Cyc_Absyn_Cnst _tmpFD=_tmpFC;struct _tuple5 _tmpFE;enum Cyc_Absyn_Sign _tmpFF;int _tmp100;struct _tuple5 _tmp101;enum Cyc_Absyn_Sign _tmp102;int _tmp103;struct _tuple5 _tmp104;enum Cyc_Absyn_Sign _tmp105;int _tmp106;struct _tuple5 _tmp107;enum Cyc_Absyn_Sign _tmp108;int _tmp109;_LL97: if((_tmpFD.Int_c).tag != 5)goto _LL99;_tmpFE=(struct _tuple5)(_tmpFD.Int_c).val;_tmpFF=_tmpFE.f1;if(_tmpFF != Cyc_Absyn_Unsigned)goto _LL99;_tmp100=_tmpFE.f2;if(_tmp100 != 0)goto _LL99;_LL98: {
# 256
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uzero={0,{.Int_c={5,{Cyc_Absyn_Unsigned,0}}}};
# 258
c=(void*)& uzero;
# 259
goto _LL96;}_LL99: if((_tmpFD.Int_c).tag != 5)goto _LL9B;_tmp101=(struct _tuple5)(_tmpFD.Int_c).val;_tmp102=_tmp101.f1;if(_tmp102 != Cyc_Absyn_Unsigned)goto _LL9B;_tmp103=_tmp101.f2;if(_tmp103 != 1)goto _LL9B;_LL9A: {
# 261
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uone={0,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};
# 263
c=(void*)& uone;
# 264
goto _LL96;}_LL9B: if((_tmpFD.Int_c).tag != 5)goto _LL9D;_tmp104=(struct _tuple5)(_tmpFD.Int_c).val;_tmp105=_tmp104.f1;if(_tmp105 != Cyc_Absyn_Signed)goto _LL9D;_tmp106=_tmp104.f2;if(_tmp106 != 0)goto _LL9D;_LL9C: {
# 266
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct szero={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 268
c=(void*)& szero;
# 269
goto _LL96;}_LL9D: if((_tmpFD.Int_c).tag != 5)goto _LL9F;_tmp107=(struct _tuple5)(_tmpFD.Int_c).val;_tmp108=_tmp107.f1;if(_tmp108 != Cyc_Absyn_Signed)goto _LL9F;_tmp109=_tmp107.f2;if(_tmp109 != 1)goto _LL9F;_LL9E: {
# 271
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct sone={0,{.Int_c={5,{Cyc_Absyn_Signed,1}}}};
# 273
c=(void*)& sone;
# 274
goto _LL96;}_LL9F:;_LLA0:
# 275
{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp24F;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp24E;c=(void*)((_tmp24E=_cycalloc(sizeof(*_tmp24E)),((_tmp24E[0]=((_tmp24F.tag=0,((_tmp24F.f1=_tmpFC,_tmp24F)))),_tmp24E))));}goto _LL96;_LL96:;}
# 277
e->r=c;}
# 279
return ans;}
# 282
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e){
# 283
void*_tmp110=e->r;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp120;enum Cyc_Absyn_Primop _tmp122;struct Cyc_List_List*_tmp123;struct Cyc_Absyn_Exp*_tmp126;_LLA2: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp111=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp111->tag != 31)goto _LLA4;}_LLA3:
# 284
 goto _LLA5;_LLA4: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp112=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp112->tag != 30)goto _LLA6;}_LLA5:
# 285
 goto _LLA7;_LLA6: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp113=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp113->tag != 16)goto _LLA8;}_LLA7:
# 286
 goto _LLA9;_LLA8: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp114=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp114->tag != 17)goto _LLAA;}_LLA9:
# 287
 goto _LLAB;_LLAA: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp115=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp115->tag != 18)goto _LLAC;}_LLAB:
# 288
 goto _LLAD;_LLAC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp116=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp116->tag != 0)goto _LLAE;}_LLAD:
# 289
 return 1;_LLAE: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp117=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp117->tag != 5)goto _LLB0;else{_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;_tmp11A=_tmp117->f3;}}_LLAF:
# 291
 return(Cyc_Evexp_c_can_eval(_tmp118) && Cyc_Evexp_c_can_eval(_tmp119)) && Cyc_Evexp_c_can_eval(_tmp11A);_LLB0: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp11B=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp11B->tag != 6)goto _LLB2;else{_tmp11C=_tmp11B->f1;_tmp11D=_tmp11B->f2;}}_LLB1:
# 292
 _tmp11F=_tmp11C;_tmp120=_tmp11D;goto _LLB3;_LLB2: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp11E=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp11E->tag != 7)goto _LLB4;else{_tmp11F=_tmp11E->f1;_tmp120=_tmp11E->f2;}}_LLB3:
# 294
 return Cyc_Evexp_c_can_eval(_tmp11F) && Cyc_Evexp_c_can_eval(_tmp120);_LLB4: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp121=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp121->tag != 2)goto _LLB6;else{_tmp122=_tmp121->f1;_tmp123=_tmp121->f2;}}_LLB5:
# 296
 for(0;_tmp123 != 0;_tmp123=_tmp123->tl){
# 297
if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_tmp123->hd))return 0;}
# 298
return 1;_LLB6: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp124=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp124->tag != 37)goto _LLB8;}_LLB7:
# 299
 return 0;_LLB8: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp125=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp125->tag != 13)goto _LLBA;else{_tmp126=_tmp125->f2;}}_LLB9:
# 300
 return Cyc_Evexp_c_can_eval(_tmp126);_LLBA:;_LLBB:
# 301
 return 0;_LLA1:;}
# 305
static int Cyc_Evexp_const_exp_case_number(struct Cyc_Absyn_Exp*e){
# 306
void*_tmp127=e->r;_LLBD: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp128=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp127;if(_tmp128->tag != 0)goto _LLBF;}_LLBE:
# 307
 return 1;_LLBF: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp129=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp127;if(_tmp129->tag != 5)goto _LLC1;}_LLC0:
# 308
 return 2;_LLC1: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp12A=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp127;if(_tmp12A->tag != 2)goto _LLC3;}_LLC2:
# 309
 return 3;_LLC3: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp12B=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp127;if(_tmp12B->tag != 16)goto _LLC5;}_LLC4:
# 310
 goto _LLC6;_LLC5: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp12C=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp127;if(_tmp12C->tag != 17)goto _LLC7;}_LLC6:
# 311
 return 4;_LLC7: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp12D=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp127;if(_tmp12D->tag != 18)goto _LLC9;}_LLC8:
# 312
 return 5;_LLC9: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp12E=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp127;if(_tmp12E->tag != 13)goto _LLCB;}_LLCA:
# 313
 return 6;_LLCB: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp12F=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp127;if(_tmp12F->tag != 6)goto _LLCD;}_LLCC:
# 314
 return 7;_LLCD: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp130=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp127;if(_tmp130->tag != 7)goto _LLCF;}_LLCE:
# 315
 return 8;_LLCF: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp131=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp127;if(_tmp131->tag != 37)goto _LLD1;}_LLD0:
# 316
 return 9;_LLD1:;_LLD2:
# 318
{const char*_tmp253;void*_tmp252[1];struct Cyc_String_pa_PrintArg_struct _tmp251;(_tmp251.tag=0,((_tmp251.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp252[0]=& _tmp251,Cyc_Tcutil_terr(e->loc,((_tmp253="bad static expression %s",_tag_dyneither(_tmp253,sizeof(char),25))),_tag_dyneither(_tmp252,sizeof(void*),1)))))));}return 0;_LLBC:;}struct _tuple16{void*f1;void*f2;};
# 322
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 323
unsigned int _tmp136;int _tmp137;struct _tuple10 _tmp135=Cyc_Evexp_eval_const_uint_exp(e1);_tmp136=_tmp135.f1;_tmp137=_tmp135.f2;{
# 324
unsigned int _tmp139;int _tmp13A;struct _tuple10 _tmp138=Cyc_Evexp_eval_const_uint_exp(e2);_tmp139=_tmp138.f1;_tmp13A=_tmp138.f2;
# 325
if(_tmp137  && _tmp13A)
# 326
return(int)(_tmp136 - _tmp139);{
# 327
int _tmp13B=Cyc_Evexp_const_exp_case_number(e1);
# 328
int _tmp13C=Cyc_Evexp_const_exp_case_number(e2);
# 329
if(_tmp13B != _tmp13C)
# 330
return _tmp13B - _tmp13C;{
# 331
struct _tuple16 _tmp254;struct _tuple16 _tmp13E=(_tmp254.f1=e1->r,((_tmp254.f2=e2->r,_tmp254)));void*_tmp13F;struct Cyc_Absyn_Exp*_tmp141;struct Cyc_Absyn_Exp*_tmp142;struct Cyc_Absyn_Exp*_tmp143;void*_tmp144;struct Cyc_Absyn_Exp*_tmp146;struct Cyc_Absyn_Exp*_tmp147;struct Cyc_Absyn_Exp*_tmp148;void*_tmp149;struct Cyc_Absyn_Exp*_tmp14B;struct Cyc_Absyn_Exp*_tmp14C;void*_tmp14D;struct Cyc_Absyn_Exp*_tmp14F;struct Cyc_Absyn_Exp*_tmp150;void*_tmp151;struct Cyc_Absyn_Exp*_tmp153;struct Cyc_Absyn_Exp*_tmp154;void*_tmp155;struct Cyc_Absyn_Exp*_tmp157;struct Cyc_Absyn_Exp*_tmp158;void*_tmp159;enum Cyc_Absyn_Primop _tmp15B;struct Cyc_List_List*_tmp15C;void*_tmp15D;enum Cyc_Absyn_Primop _tmp15F;struct Cyc_List_List*_tmp160;void*_tmp161;void*_tmp163;void*_tmp164;void*_tmp166;void*_tmp167;void*_tmp169;void*_tmp16A;struct Cyc_Absyn_Exp*_tmp16C;void*_tmp16D;struct Cyc_Absyn_Exp*_tmp16F;void*_tmp170;void*_tmp172;void*_tmp173;struct Cyc_Absyn_Exp*_tmp175;void*_tmp176;struct Cyc_Absyn_Exp*_tmp178;void*_tmp179;void*_tmp17B;void*_tmp17C;void*_tmp17D;void*_tmp17F;void*_tmp180;void*_tmp181;void*_tmp183;struct Cyc_Absyn_Exp*_tmp184;void*_tmp185;void*_tmp187;struct Cyc_Absyn_Exp*_tmp188;void*_tmp189;void*_tmp18B;void*_tmp18C;void*_tmp18E;_LLD4: _tmp13F=_tmp13E.f1;{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp140=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp13F;if(_tmp140->tag != 5)goto _LLD6;else{_tmp141=_tmp140->f1;_tmp142=_tmp140->f2;_tmp143=_tmp140->f3;}};_tmp144=_tmp13E.f2;{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp145=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp144;if(_tmp145->tag != 5)goto _LLD6;else{_tmp146=_tmp145->f1;_tmp147=_tmp145->f2;_tmp148=_tmp145->f3;}};_LLD5: {
# 336
int _tmp18F=Cyc_Evexp_const_exp_cmp(_tmp143,_tmp148);
# 337
if(_tmp18F != 0)return _tmp18F;
# 338
_tmp14B=_tmp141;_tmp14C=_tmp142;_tmp14F=_tmp146;_tmp150=_tmp147;goto _LLD7;}_LLD6: _tmp149=_tmp13E.f1;{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp14A=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp149;if(_tmp14A->tag != 6)goto _LLD8;else{_tmp14B=_tmp14A->f1;_tmp14C=_tmp14A->f2;}};_tmp14D=_tmp13E.f2;{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp14E=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp14D;if(_tmp14E->tag != 6)goto _LLD8;else{_tmp14F=_tmp14E->f1;_tmp150=_tmp14E->f2;}};_LLD7:
# 339
 _tmp153=_tmp14B;_tmp154=_tmp14C;_tmp157=_tmp14F;_tmp158=_tmp150;goto _LLD9;_LLD8: _tmp151=_tmp13E.f1;{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp152=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp151;if(_tmp152->tag != 7)goto _LLDA;else{_tmp153=_tmp152->f1;_tmp154=_tmp152->f2;}};_tmp155=_tmp13E.f2;{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp156=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp155;if(_tmp156->tag != 7)goto _LLDA;else{_tmp157=_tmp156->f1;_tmp158=_tmp156->f2;}};_LLD9: {
# 341
int _tmp190=Cyc_Evexp_const_exp_cmp(_tmp153,_tmp157);
# 342
if(_tmp190 != 0)return _tmp190;
# 343
return Cyc_Evexp_const_exp_cmp(_tmp154,_tmp158);}_LLDA: _tmp159=_tmp13E.f1;{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp15A=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp159;if(_tmp15A->tag != 2)goto _LLDC;else{_tmp15B=_tmp15A->f1;_tmp15C=_tmp15A->f2;}};_tmp15D=_tmp13E.f2;{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp15E=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp15D;if(_tmp15E->tag != 2)goto _LLDC;else{_tmp15F=_tmp15E->f1;_tmp160=_tmp15E->f2;}};_LLDB: {
# 345
int _tmp191=(int)_tmp15B - (int)_tmp15F;
# 346
if(_tmp191 != 0)return _tmp191;
# 347
for(0;_tmp15C != 0  && _tmp160 != 0;(_tmp15C=_tmp15C->tl,_tmp160=_tmp160->tl)){
# 348
int _tmp192=Cyc_Evexp_const_exp_cmp((struct Cyc_Absyn_Exp*)_tmp15C->hd,(struct Cyc_Absyn_Exp*)_tmp160->hd);
# 349
if(_tmp192 != 0)
# 350
return _tmp192;}
# 352
return 0;}_LLDC: _tmp161=_tmp13E.f1;{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp162=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp161;if(_tmp162->tag != 16)goto _LLDE;else{_tmp163=(void*)_tmp162->f1;}};_tmp164=_tmp13E.f2;{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp165=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp164;if(_tmp165->tag != 16)goto _LLDE;else{_tmp166=(void*)_tmp165->f1;}};_LLDD:
# 353
 return Cyc_Tcutil_typecmp(_tmp163,_tmp166);_LLDE: _tmp167=_tmp13E.f1;{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp168=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp167;if(_tmp168->tag != 16)goto _LLE0;else{_tmp169=(void*)_tmp168->f1;}};_tmp16A=_tmp13E.f2;{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp16B=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp16A;if(_tmp16B->tag != 17)goto _LLE0;else{_tmp16C=_tmp16B->f1;}};_LLDF: {
# 355
void*_tmp193=_tmp16C->topt;
# 356
if(_tmp193 == 0){
# 357
const char*_tmp257;void*_tmp256;(_tmp256=0,Cyc_Tcutil_terr(e2->loc,((_tmp257="cannot handle sizeof(exp) here -- use sizeof(type)",_tag_dyneither(_tmp257,sizeof(char),51))),_tag_dyneither(_tmp256,sizeof(void*),0)));}
# 358
return Cyc_Tcutil_typecmp(_tmp169,(void*)_check_null(_tmp193));}_LLE0: _tmp16D=_tmp13E.f1;{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp16E=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp16D;if(_tmp16E->tag != 17)goto _LLE2;else{_tmp16F=_tmp16E->f1;}};_tmp170=_tmp13E.f2;{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp171=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp170;if(_tmp171->tag != 16)goto _LLE2;else{_tmp172=(void*)_tmp171->f1;}};_LLE1: {
# 360
void*_tmp196=_tmp16F->topt;
# 361
if(_tmp196 == 0){
# 362
const char*_tmp25A;void*_tmp259;(_tmp259=0,Cyc_Tcutil_terr(e1->loc,((_tmp25A="cannot handle sizeof(exp) here -- use sizeof(type)",_tag_dyneither(_tmp25A,sizeof(char),51))),_tag_dyneither(_tmp259,sizeof(void*),0)));}
# 363
return Cyc_Tcutil_typecmp((void*)_check_null(_tmp196),_tmp172);}_LLE2: _tmp173=_tmp13E.f1;{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp174=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp173;if(_tmp174->tag != 17)goto _LLE4;else{_tmp175=_tmp174->f1;}};_tmp176=_tmp13E.f2;{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp177=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp176;if(_tmp177->tag != 17)goto _LLE4;else{_tmp178=_tmp177->f1;}};_LLE3: {
# 365
void*_tmp199=_tmp175->topt;
# 366
void*_tmp19A=_tmp178->topt;
# 367
if(_tmp199 == 0){
# 368
const char*_tmp25D;void*_tmp25C;(_tmp25C=0,Cyc_Tcutil_terr(e1->loc,((_tmp25D="cannot handle sizeof(exp) here -- use sizeof(type)",_tag_dyneither(_tmp25D,sizeof(char),51))),_tag_dyneither(_tmp25C,sizeof(void*),0)));}
# 369
if(_tmp19A == 0){
# 370
const char*_tmp260;void*_tmp25F;(_tmp25F=0,Cyc_Tcutil_terr(e2->loc,((_tmp260="cannot handle sizeof(exp) here -- use sizeof(type)",_tag_dyneither(_tmp260,sizeof(char),51))),_tag_dyneither(_tmp25F,sizeof(void*),0)));}
# 371
return Cyc_Tcutil_typecmp((void*)_check_null(_tmp199),(void*)_check_null(_tmp19A));}_LLE4: _tmp179=_tmp13E.f1;{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp17A=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp179;if(_tmp17A->tag != 18)goto _LLE6;else{_tmp17B=(void*)_tmp17A->f1;_tmp17C=(void*)_tmp17A->f2;}};_tmp17D=_tmp13E.f2;{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp17E=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp17D;if(_tmp17E->tag != 18)goto _LLE6;else{_tmp17F=(void*)_tmp17E->f1;_tmp180=(void*)_tmp17E->f2;}};_LLE5: {
# 373
int _tmp19F=Cyc_Tcutil_typecmp(_tmp17B,_tmp17F);
# 374
if(_tmp19F != 0)return _tmp19F;{
# 375
struct _tuple16 _tmp261;struct _tuple16 _tmp1A1=(_tmp261.f1=_tmp17C,((_tmp261.f2=_tmp180,_tmp261)));void*_tmp1A2;void*_tmp1A4;void*_tmp1A6;void*_tmp1A8;void*_tmp1AA;struct _dyneither_ptr*_tmp1AC;void*_tmp1AD;struct _dyneither_ptr*_tmp1AF;void*_tmp1B0;unsigned int _tmp1B2;void*_tmp1B3;unsigned int _tmp1B5;_LLED: _tmp1A2=_tmp1A1.f1;{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp1A3=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp1A2;if(_tmp1A3->tag != 0)goto _LLEF;};_tmp1A4=_tmp1A1.f2;{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp1A5=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp1A4;if(_tmp1A5->tag != 1)goto _LLEF;};_LLEE:
# 376
 return - 1;_LLEF: _tmp1A6=_tmp1A1.f1;{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp1A7=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp1A6;if(_tmp1A7->tag != 1)goto _LLF1;};_tmp1A8=_tmp1A1.f2;{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp1A9=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp1A8;if(_tmp1A9->tag != 0)goto _LLF1;};_LLF0:
# 377
 return 1;_LLF1: _tmp1AA=_tmp1A1.f1;{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp1AB=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp1AA;if(_tmp1AB->tag != 0)goto _LLF3;else{_tmp1AC=_tmp1AB->f1;}};_tmp1AD=_tmp1A1.f2;{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp1AE=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp1AD;if(_tmp1AE->tag != 0)goto _LLF3;else{_tmp1AF=_tmp1AE->f1;}};_LLF2:
# 378
 return Cyc_strptrcmp(_tmp1AC,_tmp1AF);_LLF3: _tmp1B0=_tmp1A1.f1;{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp1B1=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp1B0;if(_tmp1B1->tag != 1)goto _LLEC;else{_tmp1B2=_tmp1B1->f1;}};_tmp1B3=_tmp1A1.f2;{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp1B4=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp1B3;if(_tmp1B4->tag != 1)goto _LLEC;else{_tmp1B5=_tmp1B4->f1;}};_LLF4:
# 379
 return(int)(_tmp1B2 - _tmp1B5);_LLEC:;};}_LLE6: _tmp181=_tmp13E.f1;{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp182=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp181;if(_tmp182->tag != 13)goto _LLE8;else{_tmp183=(void*)_tmp182->f1;_tmp184=_tmp182->f2;}};_tmp185=_tmp13E.f2;{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp186=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp185;if(_tmp186->tag != 13)goto _LLE8;else{_tmp187=(void*)_tmp186->f1;_tmp188=_tmp186->f2;}};_LLE7: {
# 382
int _tmp1B6=Cyc_Tcutil_typecmp(_tmp183,_tmp187);
# 383
if(_tmp1B6 != 0)return _tmp1B6;
# 384
return Cyc_Evexp_const_exp_cmp(_tmp184,_tmp188);}_LLE8: _tmp189=_tmp13E.f1;{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp18A=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp189;if(_tmp18A->tag != 37)goto _LLEA;else{_tmp18B=(void*)_tmp18A->f1;}};_tmp18C=_tmp13E.f2;{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp18D=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp18C;if(_tmp18D->tag != 37)goto _LLEA;else{_tmp18E=(void*)_tmp18D->f1;}};_LLE9:
# 386
 if(Cyc_Tcutil_unify(_tmp18B,_tmp18E))return 0;
# 387
return Cyc_Tcutil_typecmp(_tmp18B,_tmp18E);_LLEA:;_LLEB: {
# 388
const char*_tmp264;void*_tmp263;(_tmp263=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp264="Evexp::const_exp_cmp, unexpected case",_tag_dyneither(_tmp264,sizeof(char),38))),_tag_dyneither(_tmp263,sizeof(void*),0)));}_LLD3:;};};};}
# 391
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 392
return Cyc_Evexp_const_exp_cmp(e1,e2)== 0;}
# 394
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 395
unsigned int _tmp1BA;int _tmp1BB;struct _tuple10 _tmp1B9=Cyc_Evexp_eval_const_uint_exp(e1);_tmp1BA=_tmp1B9.f1;_tmp1BB=_tmp1B9.f2;{
# 396
unsigned int _tmp1BD;int _tmp1BE;struct _tuple10 _tmp1BC=Cyc_Evexp_eval_const_uint_exp(e2);_tmp1BD=_tmp1BC.f1;_tmp1BE=_tmp1BC.f2;
# 397
if(_tmp1BB  && _tmp1BE)
# 398
return _tmp1BA <= _tmp1BD;
# 399
return Cyc_Evexp_same_const_exp(e1,e2);};}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 404
int Cyc_Evexp_okay_szofarg(void*t){
# 405
void*_tmp1BF=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp1C2;struct Cyc_Core_Opt*_tmp1C4;struct Cyc_Core_Opt _tmp1C5;struct Cyc_Absyn_Kind*_tmp1C6;struct Cyc_Absyn_Kind _tmp1C7;enum Cyc_Absyn_KindQual _tmp1C8;struct Cyc_Absyn_DatatypeFieldInfo _tmp1CC;union Cyc_Absyn_DatatypeFieldInfoU _tmp1CD;struct _tuple1 _tmp1CE;struct Cyc_Absyn_Datatypefield*_tmp1CF;struct Cyc_List_List*_tmp1D1;struct Cyc_Absyn_ArrayInfo _tmp1D8;void*_tmp1D9;struct Cyc_Absyn_Exp*_tmp1DA;struct Cyc_Absyn_AggrInfo _tmp1DF;union Cyc_Absyn_AggrInfoU _tmp1E0;struct Cyc_List_List*_tmp1E1;struct Cyc_List_List*_tmp1E3;struct Cyc_Absyn_Typedefdecl*_tmp1EF;_LLF6: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp1C0=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1BF;if(_tmp1C0->tag != 0)goto _LLF8;}_LLF7:
# 406
 return 0;_LLF8: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1C1=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1BF;if(_tmp1C1->tag != 2)goto _LLFA;else{_tmp1C2=_tmp1C1->f1;}}_LLF9:
# 408
 switch((Cyc_Tcutil_tvar_kind(_tmp1C2,& Cyc_Tcutil_bk))->kind){case Cyc_Absyn_BoxKind: _LL130:
# 409
 return 1;default: _LL131:
# 410
 return 0;}_LLFA: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp1C3=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BF;if(_tmp1C3->tag != 1)goto _LLFC;else{_tmp1C4=_tmp1C3->f1;if(_tmp1C4 == 0)goto _LLFC;_tmp1C5=*_tmp1C4;_tmp1C6=(struct Cyc_Absyn_Kind*)_tmp1C5.v;_tmp1C7=*_tmp1C6;_tmp1C8=_tmp1C7.kind;if(_tmp1C8 != Cyc_Absyn_BoxKind)goto _LLFC;}}_LLFB:
# 412
 return 1;_LLFC: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp1C9=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BF;if(_tmp1C9->tag != 1)goto _LLFE;}_LLFD:
# 413
 return 0;_LLFE: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1CA=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1BF;if(_tmp1CA->tag != 3)goto _LL100;}_LLFF:
# 414
 return 1;_LL100: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1CB=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1BF;if(_tmp1CB->tag != 4)goto _LL102;else{_tmp1CC=_tmp1CB->f1;_tmp1CD=_tmp1CC.field_info;if((_tmp1CD.KnownDatatypefield).tag != 2)goto _LL102;_tmp1CE=(struct _tuple1)(_tmp1CD.KnownDatatypefield).val;_tmp1CF=_tmp1CE.f2;}}_LL101:
# 416
 _tmp1D1=_tmp1CF->typs;goto _LL103;_LL102: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1D0=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1BF;if(_tmp1D0->tag != 10)goto _LL104;else{_tmp1D1=_tmp1D0->f1;}}_LL103:
# 418
 for(0;_tmp1D1 != 0;_tmp1D1=_tmp1D1->tl){
# 419
if(!Cyc_Evexp_okay_szofarg((*((struct _tuple17*)_tmp1D1->hd)).f2))
# 420
return 0;}
# 421
return 1;_LL104: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1D2=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1BF;if(_tmp1D2->tag != 4)goto _LL106;}_LL105:
# 423
 return 0;_LL106: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1D3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BF;if(_tmp1D3->tag != 5)goto _LL108;}_LL107:
# 426
 return 1;_LL108: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1D4=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1BF;if(_tmp1D4->tag != 19)goto _LL10A;}_LL109:
# 428
 goto _LL10B;_LL10A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1D5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1BF;if(_tmp1D5->tag != 6)goto _LL10C;}_LL10B:
# 429
 goto _LL10D;_LL10C: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp1D6=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1BF;if(_tmp1D6->tag != 7)goto _LL10E;}_LL10D:
# 430
 return 1;_LL10E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1D7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1BF;if(_tmp1D7->tag != 8)goto _LL110;else{_tmp1D8=_tmp1D7->f1;_tmp1D9=_tmp1D8.elt_type;_tmp1DA=_tmp1D8.num_elts;}}_LL10F:
# 432
 return _tmp1DA != 0;_LL110: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1DB=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BF;if(_tmp1DB->tag != 9)goto _LL112;}_LL111:
# 433
 return 0;_LL112: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp1DC=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1BF;if(_tmp1DC->tag != 15)goto _LL114;}_LL113:
# 434
 return 1;_LL114: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp1DD=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp1BF;if(_tmp1DD->tag != 16)goto _LL116;}_LL115:
# 435
 return 1;_LL116: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1DE=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1BF;if(_tmp1DE->tag != 11)goto _LL118;else{_tmp1DF=_tmp1DE->f1;_tmp1E0=_tmp1DF.aggr_info;_tmp1E1=_tmp1DF.targs;}}_LL117: {
# 437
struct Cyc_Absyn_Aggrdecl*ad;
# 438
{union Cyc_Absyn_AggrInfoU _tmp1F0=_tmp1E0;struct _tuple2 _tmp1F1;struct Cyc_Absyn_Aggrdecl**_tmp1F2;struct Cyc_Absyn_Aggrdecl*_tmp1F3;_LL134: if((_tmp1F0.UnknownAggr).tag != 1)goto _LL136;_tmp1F1=(struct _tuple2)(_tmp1F0.UnknownAggr).val;_LL135: {
# 439
const char*_tmp267;void*_tmp266;(_tmp266=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp267="szof on unchecked StructType",_tag_dyneither(_tmp267,sizeof(char),29))),_tag_dyneither(_tmp266,sizeof(void*),0)));}_LL136: if((_tmp1F0.KnownAggr).tag != 2)goto _LL133;_tmp1F2=(struct Cyc_Absyn_Aggrdecl**)(_tmp1F0.KnownAggr).val;_tmp1F3=*_tmp1F2;_LL137:
# 440
 ad=_tmp1F3;_LL133:;}
# 442
if(ad->impl == 0)
# 443
return 0;
# 444
{
# 445
struct _RegionHandle _tmp1F6=_new_region("temp");struct _RegionHandle*temp=& _tmp1F6;_push_region(temp);
# 446
{struct Cyc_List_List*_tmp1F7=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,_tmp1E1);
# 447
{struct Cyc_List_List*_tmp1F8=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp1F8 != 0;_tmp1F8=_tmp1F8->tl){
# 448
if(!Cyc_Evexp_okay_szofarg(Cyc_Tcutil_rsubstitute(temp,_tmp1F7,((struct Cyc_Absyn_Aggrfield*)_tmp1F8->hd)->type))){int _tmp1F9=0;_npop_handler(0);return _tmp1F9;}}}{
# 449
int _tmp1FA=1;_npop_handler(0);return _tmp1FA;};}
# 446
;_pop_region(temp);}
# 451
_tmp1E3=(ad->impl)->fields;goto _LL119;}_LL118: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1E2=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1BF;if(_tmp1E2->tag != 12)goto _LL11A;else{_tmp1E3=_tmp1E2->f2;}}_LL119:
# 453
 for(0;_tmp1E3 != 0;_tmp1E3=_tmp1E3->tl){
# 454
if(!Cyc_Evexp_okay_szofarg(((struct Cyc_Absyn_Aggrfield*)_tmp1E3->hd)->type))
# 455
return 0;}
# 456
return 1;_LL11A: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp1E4=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1BF;if(_tmp1E4->tag != 13)goto _LL11C;}_LL11B:
# 457
 goto _LL11D;_LL11C: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp1E5=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1BF;if(_tmp1E5->tag != 14)goto _LL11E;}_LL11D:
# 458
 return 1;_LL11E: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1E6=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1BF;if(_tmp1E6->tag != 26)goto _LL120;}_LL11F:
# 459
 goto _LL121;_LL120: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp1E7=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp1BF;if(_tmp1E7->tag != 20)goto _LL122;}_LL121:
# 460
 goto _LL123;_LL122: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp1E8=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp1BF;if(_tmp1E8->tag != 22)goto _LL124;}_LL123:
# 461
 goto _LL125;_LL124: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp1E9=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp1BF;if(_tmp1E9->tag != 21)goto _LL126;}_LL125:
# 462
 goto _LL127;_LL126: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1EA=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp1BF;if(_tmp1EA->tag != 24)goto _LL128;}_LL127:
# 463
 goto _LL129;_LL128: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1EB=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp1BF;if(_tmp1EB->tag != 23)goto _LL12A;}_LL129:
# 464
 goto _LL12B;_LL12A: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1EC=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1BF;if(_tmp1EC->tag != 18)goto _LL12C;}_LL12B:
# 465
 goto _LL12D;_LL12C: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1ED=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1BF;if(_tmp1ED->tag != 25)goto _LL12E;}_LL12D:
# 466
 return 0;_LL12E: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1EE=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1BF;if(_tmp1EE->tag != 17)goto _LLF5;else{_tmp1EF=_tmp1EE->f3;}}_LL12F:
# 469
 if(_tmp1EF == 0  || _tmp1EF->kind == 0){
# 470
struct Cyc_String_pa_PrintArg_struct _tmp26F;void*_tmp26E[1];const char*_tmp26D;void*_tmp26C;(_tmp26C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmp26F.tag=0,((_tmp26F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp26E[0]=& _tmp26F,Cyc_aprintf(((_tmp26D="szof typedeftype %s",_tag_dyneither(_tmp26D,sizeof(char),20))),_tag_dyneither(_tmp26E,sizeof(void*),1)))))))),_tag_dyneither(_tmp26C,sizeof(void*),0)));}
# 471
return((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1EF->kind))->v)->kind == Cyc_Absyn_BoxKind;_LLF5:;}
