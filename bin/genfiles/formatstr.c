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
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{
char*tag;struct _dyneither_ptr f1;};
# 290 "/home/jgm/cyclone/include/cycboot.h"
int isdigit(int);
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
return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "/home/jgm/cyclone/include/list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "/home/jgm/cyclone/include/list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;extern char Cyc_List_List_mismatch[14];
struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[
4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 38 "/home/jgm/cyclone/include/string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 134 "/home/jgm/cyclone/include/string.h"
struct _dyneither_ptr Cyc_implode(struct Cyc_List_List*c);struct Cyc_Lineno_Pos{
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
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[
10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
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
f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual
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
# 912 "/home/jgm/cyclone/src/absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 914
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 928
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 937
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 942
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;
# 944
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_slong_typ;
# 946
void*Cyc_Absyn_float_typ(int);
# 978
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zero_term);
# 983
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zero_term);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 29 "/home/jgm/cyclone/include/iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 30
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "/home/jgm/cyclone/include/set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{
char*tag;};struct Cyc_Dict_T;
# 46 "/home/jgm/cyclone/include/dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct
Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{
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
# 136 "/home/jgm/cyclone/src/tcenv.h"
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 40 "/home/jgm/cyclone/src/tcutil.h"
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 116
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 338 "/home/jgm/cyclone/src/tcutil.h"
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 34 "/home/jgm/cyclone/src/formatstr.h"
struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*,struct
Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
# 37
struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct _dyneither_ptr,unsigned int);struct _tuple10{struct Cyc_List_List*f1;struct
Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;char f5;int f6;};
# 44 "/home/jgm/cyclone/src/formatstr.cyc"
struct Cyc_Core_Opt*Cyc_Formatstr_parse_conversionspecification(struct
_RegionHandle*r,struct _dyneither_ptr s,int i){
# 48
unsigned long _tmp0=Cyc_strlen((struct _dyneither_ptr)s);
# 49
if(i < 0  || i >= _tmp0)return 0;{
# 52
struct Cyc_List_List*_tmp1=0;
# 53
char c=' ';
# 54
for(0;i < _tmp0;++ i){
# 55
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
# 56
switch(c){case '+': _LL0:
# 57
 goto _LL1;case '-': _LL1:
# 58
 goto _LL2;case ' ': _LL2:
# 59
 goto _LL3;case '#': _LL3:
# 60
 goto _LL4;case '0': _LL4:
# 61
{struct Cyc_List_List*_tmp138;_tmp1=((_tmp138=_region_malloc(r,sizeof(*_tmp138)),((
_tmp138->hd=(void*)((int)c),((_tmp138->tl=_tmp1,_tmp138))))));}continue;default:
_LL5:
# 62
 break;}
# 64
break;}
# 66
if(i >= _tmp0)return 0;
# 67
_tmp1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1);{
# 70
struct Cyc_List_List*_tmp3=0;
# 71
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
# 72
if(c == '*'){
# 73
{struct Cyc_List_List*_tmp139;_tmp3=((_tmp139=_region_malloc(r,sizeof(*_tmp139)),((
_tmp139->hd=(void*)((int)c),((_tmp139->tl=_tmp3,_tmp139))))));}
# 74
++ i;}else{
# 76
for(0;i < _tmp0;++ i){
# 77
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
# 78
if(isdigit((int)c)){struct Cyc_List_List*_tmp13A;_tmp3=((_tmp13A=_region_malloc(
r,sizeof(*_tmp13A)),((_tmp13A->hd=(void*)((int)c),((_tmp13A->tl=_tmp3,_tmp13A))))));}
else{
# 79
break;}}}
# 82
if(i >= _tmp0)return 0;
# 83
_tmp3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3);{
# 86
struct Cyc_List_List*_tmp6=0;
# 87
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
# 88
if(c == '.'){
# 89
{struct Cyc_List_List*_tmp13B;_tmp6=((_tmp13B=_region_malloc(r,sizeof(*_tmp13B)),((
_tmp13B->hd=(void*)((int)c),((_tmp13B->tl=_tmp6,_tmp13B))))));}
# 90
++ i;
# 91
if(i >= _tmp0)return 0;
# 92
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
# 93
if(c == '*'){
# 94
{struct Cyc_List_List*_tmp13C;_tmp6=((_tmp13C=_region_malloc(r,sizeof(*_tmp13C)),((
_tmp13C->hd=(void*)((int)c),((_tmp13C->tl=_tmp6,_tmp13C))))));}
# 95
++ i;}else{
# 97
for(0;i < _tmp0;++ i){
# 98
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
# 99
if(isdigit((int)c)){struct Cyc_List_List*_tmp13D;_tmp6=((_tmp13D=_region_malloc(
r,sizeof(*_tmp13D)),((_tmp13D->hd=(void*)((int)c),((_tmp13D->tl=_tmp6,_tmp13D))))));}
else{
# 100
break;}}}}
# 103
if(i >= _tmp0)return 0;
# 104
_tmp6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6);{
# 108
struct Cyc_List_List*_tmpA=0;
# 109
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
# 110
switch(c){case 'h': _LL7:
# 112
{struct Cyc_List_List*_tmp13E;_tmpA=((_tmp13E=_region_malloc(r,sizeof(*_tmp13E)),((
_tmp13E->hd=(void*)((int)c),((_tmp13E->tl=_tmpA,_tmp13E))))));}
# 113
++ i;
# 114
if(i >= _tmp0)return 0;
# 115
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
# 116
if(c == 'h'){{struct Cyc_List_List*_tmp13F;_tmpA=((_tmp13F=_region_malloc(r,
sizeof(*_tmp13F)),((_tmp13F->hd=(void*)((int)c),((_tmp13F->tl=_tmpA,_tmp13F))))));}
++ i;}
# 117
break;case 'l': _LL8:
# 119
{struct Cyc_List_List*_tmp140;_tmpA=((_tmp140=_region_malloc(r,sizeof(*_tmp140)),((
_tmp140->hd=(void*)((int)c),((_tmp140->tl=_tmpA,_tmp140))))));}
# 120
++ i;
# 121
if(i >= _tmp0)return 0;
# 122
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
# 123
if(c == 'l'){{struct Cyc_List_List*_tmp141;_tmpA=((_tmp141=_region_malloc(r,
sizeof(*_tmp141)),((_tmp141->hd=(void*)((int)c),((_tmp141->tl=_tmpA,_tmp141))))));}
++ i;}
# 124
break;case 'j': _LL9:
# 125
 goto _LLA;case 'z': _LLA:
# 126
 goto _LLB;case 't': _LLB:
# 127
 goto _LLC;case 'L': _LLC:
# 129
{struct Cyc_List_List*_tmp142;_tmpA=((_tmp142=_region_malloc(r,sizeof(*_tmp142)),((
_tmp142->hd=(void*)((int)c),((_tmp142->tl=_tmpA,_tmp142))))));}
# 130
++ i;
# 131
break;default: _LLD:
# 132
 break;}
# 134
if(i >= _tmp0)return 0;
# 135
_tmpA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpA);
# 138
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
# 139
switch(c){case 'd': _LLF:
# 140
 goto _LL10;case 'i': _LL10:
# 141
 goto _LL11;case 'o': _LL11:
# 142
 goto _LL12;case 'u': _LL12:
# 143
 goto _LL13;case 'x': _LL13:
# 144
 goto _LL14;case 'X': _LL14:
# 145
 goto _LL15;case 'f': _LL15:
# 146
 goto _LL16;case 'F': _LL16:
# 147
 goto _LL17;case 'e': _LL17:
# 148
 goto _LL18;case 'E': _LL18:
# 149
 goto _LL19;case 'g': _LL19:
# 150
 goto _LL1A;case 'G': _LL1A:
# 151
 goto _LL1B;case 'a': _LL1B:
# 152
 goto _LL1C;case 'A': _LL1C:
# 153
 goto _LL1D;case 'c': _LL1D:
# 154
 goto _LL1E;case 's': _LL1E:
# 155
 goto _LL1F;case 'p': _LL1F:
# 156
 goto _LL20;case 'n': _LL20:
# 157
 goto _LL21;case '%': _LL21:
# 158
 break;default: _LL22:
# 159
 return 0;}{
# 170 "/home/jgm/cyclone/src/formatstr.cyc"
struct _tuple10*_tmp145;struct Cyc_Core_Opt*_tmp144;return(_tmp144=_region_malloc(
r,sizeof(*_tmp144)),((_tmp144->v=((_tmp145=_region_malloc(r,sizeof(*_tmp145)),((
_tmp145->f1=_tmp1,((_tmp145->f2=_tmp3,((_tmp145->f3=_tmp6,((_tmp145->f4=_tmpA,((
_tmp145->f5=c,((_tmp145->f6=i + 1,_tmp145)))))))))))))),_tmp144)));};};};};};}
# 173
struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*r,struct
Cyc_Tcenv_Tenv*te,struct _dyneither_ptr s,unsigned int loc){
# 176
unsigned long _tmp12=Cyc_strlen((struct _dyneither_ptr)s);
# 177
struct Cyc_List_List*_tmp13=0;
# 178
int i;
# 179
struct _RegionHandle _tmp14=_new_region("temp");struct _RegionHandle*temp=& _tmp14;
_push_region(temp);
# 180
for(i=0;i < _tmp12;++ i){
# 181
if(*((const char*)_check_dyneither_subscript(s,sizeof(char),i))!= '%')continue;{
# 182
struct Cyc_Core_Opt*_tmp15=Cyc_Formatstr_parse_conversionspecification(temp,s,i + 
1);
# 183
if(_tmp15 == 0){
# 184
{const char*_tmp148;void*_tmp147;(_tmp147=0,Cyc_Tcutil_terr(loc,((_tmp148="bad format string",
_tag_dyneither(_tmp148,sizeof(char),18))),_tag_dyneither(_tmp147,sizeof(void*),0)));}{
# 185
struct Cyc_List_List*_tmp18=0;_npop_handler(0);return _tmp18;};}{
# 187
struct _tuple10 _tmp1A;struct Cyc_List_List*_tmp1B;struct Cyc_List_List*_tmp1C;
struct Cyc_List_List*_tmp1D;struct Cyc_List_List*_tmp1E;char _tmp1F;int _tmp20;
struct _tuple10*_tmp19=(struct _tuple10*)_tmp15->v;_tmp1A=*_tmp19;_tmp1B=_tmp1A.f1;
_tmp1C=_tmp1A.f2;_tmp1D=_tmp1A.f3;_tmp1E=_tmp1A.f4;_tmp1F=_tmp1A.f5;_tmp20=
_tmp1A.f6;
# 188
i=_tmp20 - 1;
# 189
{struct Cyc_List_List*_tmp21=_tmp1E;struct Cyc_List_List _tmp22;int _tmp23;struct
Cyc_List_List*_tmp24;_LL25: if(_tmp21 == 0)goto _LL27;_tmp22=*_tmp21;_tmp23=(int)
_tmp22.hd;_tmp24=_tmp22.tl;if(_tmp24 != 0)goto _LL27;if(!(
# 190
(_tmp23 == 'j'  || _tmp23 == 'z') || _tmp23 == 't'))goto _LL27;_LL26:
# 193
{const char*_tmp14C;void*_tmp14B[1];struct Cyc_Int_pa_PrintArg_struct _tmp14A;(
_tmp14A.tag=1,((_tmp14A.f1=(unsigned long)_tmp23,((_tmp14B[0]=& _tmp14A,Cyc_Tcutil_terr(
loc,((_tmp14C="length modifier '%c' is not supported",_tag_dyneither(_tmp14C,
sizeof(char),38))),_tag_dyneither(_tmp14B,sizeof(void*),1)))))));}{
# 194
struct Cyc_List_List*_tmp28=0;_npop_handler(0);return _tmp28;};_LL27:;_LL28:
# 195
 goto _LL24;_LL24:;}
# 197
{struct Cyc_List_List*_tmp29=_tmp1C;struct Cyc_List_List _tmp2A;int _tmp2B;struct
Cyc_List_List*_tmp2C;_LL2A: if(_tmp29 == 0)goto _LL2C;_tmp2A=*_tmp29;_tmp2B=(int)
_tmp2A.hd;_tmp2C=_tmp2A.tl;if(_tmp2C != 0)goto _LL2C;if(!(_tmp2B == '*'))goto _LL2C;
_LL2B:
# 198
{struct Cyc_List_List*_tmp14D;_tmp13=((_tmp14D=_region_malloc(r,sizeof(*_tmp14D)),((
_tmp14D->hd=Cyc_Absyn_sint_typ,((_tmp14D->tl=_tmp13,_tmp14D))))));}goto _LL29;
_LL2C:;_LL2D:
# 199
 goto _LL29;_LL29:;}
# 201
{struct Cyc_List_List*_tmp2E=_tmp1D;struct Cyc_List_List _tmp2F;int _tmp30;struct
Cyc_List_List*_tmp31;struct Cyc_List_List _tmp32;int _tmp33;struct Cyc_List_List*
_tmp34;_LL2F: if(_tmp2E == 0)goto _LL31;_tmp2F=*_tmp2E;_tmp30=(int)_tmp2F.hd;_tmp31=
_tmp2F.tl;if(_tmp31 == 0)goto _LL31;_tmp32=*_tmp31;_tmp33=(int)_tmp32.hd;_tmp34=
_tmp32.tl;if(_tmp34 != 0)goto _LL31;if(!(
# 202
_tmp30 == '.'  && _tmp33 == '*'))goto _LL31;_LL30:
# 203
{struct Cyc_List_List*_tmp14E;_tmp13=((_tmp14E=_region_malloc(r,sizeof(*_tmp14E)),((
_tmp14E->hd=Cyc_Absyn_sint_typ,((_tmp14E->tl=_tmp13,_tmp14E))))));}goto _LL2E;
_LL31:;_LL32:
# 204
 goto _LL2E;_LL2E:;}{
# 206
void*t;
# 207
switch(_tmp1F){case 'd': _LL33:
# 208
 goto _LL34;case 'i': _LL34:
# 210
{struct Cyc_List_List*f=_tmp1B;for(0;f != 0;f=f->tl){
# 211
if((int)f->hd == '#'){
# 212
{const char*_tmp152;void*_tmp151[1];struct Cyc_Int_pa_PrintArg_struct _tmp150;(
_tmp150.tag=1,((_tmp150.f1=(unsigned long)((int)_tmp1F),((_tmp151[0]=& _tmp150,
Cyc_Tcutil_terr(loc,((_tmp152="flag '#' is not valid with %%%c",_tag_dyneither(
_tmp152,sizeof(char),32))),_tag_dyneither(_tmp151,sizeof(void*),1)))))));}{
# 213
struct Cyc_List_List*_tmp39=0;_npop_handler(0);return _tmp39;};}}}
# 215
{struct Cyc_List_List*_tmp3A=_tmp1E;struct Cyc_List_List _tmp3B;int _tmp3C;struct
Cyc_List_List*_tmp3D;struct Cyc_List_List _tmp3E;int _tmp3F;struct Cyc_List_List*
_tmp40;struct Cyc_List_List _tmp41;int _tmp42;struct Cyc_List_List*_tmp43;struct Cyc_List_List
_tmp44;int _tmp45;struct Cyc_List_List*_tmp46;_LL37: if(_tmp3A != 0)goto _LL39;_LL38:
# 216
 t=Cyc_Absyn_sint_typ;goto _LL36;_LL39: if(_tmp3A == 0)goto _LL3B;_tmp3B=*_tmp3A;
_tmp3C=(int)_tmp3B.hd;_tmp3D=_tmp3B.tl;if(_tmp3D != 0)goto _LL3B;if(!(_tmp3C == 'l'))
goto _LL3B;_LL3A:
# 217
 t=Cyc_Absyn_slong_typ;goto _LL36;_LL3B: if(_tmp3A == 0)goto _LL3D;_tmp3E=*_tmp3A;
_tmp3F=(int)_tmp3E.hd;_tmp40=_tmp3E.tl;if(_tmp40 != 0)goto _LL3D;if(!(_tmp3F == 'h'))
goto _LL3D;_LL3C:
# 218
 t=Cyc_Absyn_sshort_typ;goto _LL36;_LL3D: if(_tmp3A == 0)goto _LL3F;_tmp41=*_tmp3A;
_tmp42=(int)_tmp41.hd;_tmp43=_tmp41.tl;if(_tmp43 == 0)goto _LL3F;_tmp44=*_tmp43;
_tmp45=(int)_tmp44.hd;_tmp46=_tmp44.tl;if(_tmp46 != 0)goto _LL3F;if(!(
# 219
_tmp42 == 'h'  && _tmp45 == 'h'))goto _LL3F;_LL3E:
# 220
 t=Cyc_Absyn_schar_typ;goto _LL36;_LL3F:;_LL40:
# 222
{const char*_tmp157;void*_tmp156[2];struct Cyc_String_pa_PrintArg_struct _tmp155;
struct Cyc_Int_pa_PrintArg_struct _tmp154;(_tmp154.tag=1,((_tmp154.f1=(
unsigned long)((int)_tmp1F),((_tmp155.tag=0,((_tmp155.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)
# 223
Cyc_implode(_tmp1E)),((_tmp156[0]=& _tmp155,((_tmp156[1]=& _tmp154,Cyc_Tcutil_terr(
loc,((_tmp157="length modifier '%s' is not allowed with %%%c",_tag_dyneither(
_tmp157,sizeof(char),46))),_tag_dyneither(_tmp156,sizeof(void*),2)))))))))))));}{
# 224
struct Cyc_List_List*_tmp4B=0;_npop_handler(0);return _tmp4B;};_LL36:;}
# 226
{struct Cyc_List_List*_tmp158;_tmp13=((_tmp158=_region_malloc(r,sizeof(*_tmp158)),((
_tmp158->hd=t,((_tmp158->tl=_tmp13,_tmp158))))));}
# 227
break;case 'u': _LL35:
# 229
{struct Cyc_List_List*f=_tmp1B;for(0;f != 0;f=f->tl){
# 230
if((int)f->hd == '#'){
# 231
{const char*_tmp15B;void*_tmp15A;(_tmp15A=0,Cyc_Tcutil_terr(loc,((_tmp15B="Flag '#' not valid with %%u",
_tag_dyneither(_tmp15B,sizeof(char),28))),_tag_dyneither(_tmp15A,sizeof(void*),0)));}{
# 232
struct Cyc_List_List*_tmp4F=0;_npop_handler(0);return _tmp4F;};}}}
# 234
goto _LL41;case 'o': _LL41:
# 235
 goto _LL42;case 'x': _LL42:
# 236
 goto _LL43;case 'X': _LL43:
# 238
{struct Cyc_List_List*_tmp50=_tmp1E;struct Cyc_List_List _tmp51;int _tmp52;struct
Cyc_List_List*_tmp53;struct Cyc_List_List _tmp54;int _tmp55;struct Cyc_List_List*
_tmp56;struct Cyc_List_List _tmp57;int _tmp58;struct Cyc_List_List*_tmp59;struct Cyc_List_List
_tmp5A;int _tmp5B;struct Cyc_List_List*_tmp5C;_LL46: if(_tmp50 != 0)goto _LL48;_LL47:
# 239
 t=Cyc_Absyn_uint_typ;goto _LL45;_LL48: if(_tmp50 == 0)goto _LL4A;_tmp51=*_tmp50;
_tmp52=(int)_tmp51.hd;_tmp53=_tmp51.tl;if(_tmp53 != 0)goto _LL4A;if(!(_tmp52 == 'l'))
goto _LL4A;_LL49:
# 240
 t=Cyc_Absyn_ulong_typ;goto _LL45;_LL4A: if(_tmp50 == 0)goto _LL4C;_tmp54=*_tmp50;
_tmp55=(int)_tmp54.hd;_tmp56=_tmp54.tl;if(_tmp56 != 0)goto _LL4C;if(!(_tmp55 == 'h'))
goto _LL4C;_LL4B:
# 241
 t=Cyc_Absyn_ushort_typ;goto _LL45;_LL4C: if(_tmp50 == 0)goto _LL4E;_tmp57=*_tmp50;
_tmp58=(int)_tmp57.hd;_tmp59=_tmp57.tl;if(_tmp59 == 0)goto _LL4E;_tmp5A=*_tmp59;
_tmp5B=(int)_tmp5A.hd;_tmp5C=_tmp5A.tl;if(_tmp5C != 0)goto _LL4E;if(!(
# 242
_tmp58 == 'h'  && _tmp5B == 'h'))goto _LL4E;_LL4D:
# 243
 t=Cyc_Absyn_uchar_typ;goto _LL45;_LL4E:;_LL4F:
# 246
{const char*_tmp160;void*_tmp15F[2];struct Cyc_String_pa_PrintArg_struct _tmp15E;
struct Cyc_Int_pa_PrintArg_struct _tmp15D;(_tmp15D.tag=1,((_tmp15D.f1=(
unsigned long)((int)_tmp1F),((_tmp15E.tag=0,((_tmp15E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)
# 247
Cyc_implode(_tmp1E)),((_tmp15F[0]=& _tmp15E,((_tmp15F[1]=& _tmp15D,Cyc_Tcutil_terr(
loc,((_tmp160="length modifier '%s' is not allowed with %%%c",_tag_dyneither(
_tmp160,sizeof(char),46))),_tag_dyneither(_tmp15F,sizeof(void*),2)))))))))))));}{
# 248
struct Cyc_List_List*_tmp61=0;_npop_handler(0);return _tmp61;};_LL45:;}
# 250
{struct Cyc_List_List*_tmp161;_tmp13=((_tmp161=_region_malloc(r,sizeof(*_tmp161)),((
_tmp161->hd=t,((_tmp161->tl=_tmp13,_tmp161))))));}
# 251
break;case 'f': _LL44:
# 252
 goto _LL50;case 'F': _LL50:
# 253
 goto _LL51;case 'e': _LL51:
# 254
 goto _LL52;case 'E': _LL52:
# 255
 goto _LL53;case 'g': _LL53:
# 256
 goto _LL54;case 'G': _LL54:
# 257
 goto _LL55;case 'a': _LL55:
# 258
 goto _LL56;case 'A': _LL56:
# 266
{struct Cyc_List_List*_tmp63=_tmp1E;struct Cyc_List_List _tmp64;int _tmp65;struct
Cyc_List_List*_tmp66;_LL59: if(_tmp63 != 0)goto _LL5B;_LL5A:
# 267
{struct Cyc_List_List*_tmp162;_tmp13=((_tmp162=_region_malloc(r,sizeof(*_tmp162)),((
_tmp162->hd=Cyc_Absyn_float_typ(1),((_tmp162->tl=_tmp13,_tmp162))))));}goto _LL58;
_LL5B: if(_tmp63 == 0)goto _LL5D;_tmp64=*_tmp63;_tmp65=(int)_tmp64.hd;_tmp66=_tmp64.tl;
if(_tmp66 != 0)goto _LL5D;if(!(_tmp65 == 'l'))goto _LL5D;_LL5C:
# 269
{struct Cyc_List_List*_tmp163;_tmp13=((_tmp163=_region_malloc(r,sizeof(*_tmp163)),((
_tmp163->hd=Cyc_Absyn_float_typ(2),((_tmp163->tl=_tmp13,_tmp163))))));}goto _LL58;
_LL5D:;_LL5E:
# 271
{const char*_tmp168;void*_tmp167[2];struct Cyc_String_pa_PrintArg_struct _tmp166;
struct Cyc_Int_pa_PrintArg_struct _tmp165;(_tmp165.tag=1,((_tmp165.f1=(
unsigned long)((int)_tmp1F),((_tmp166.tag=0,((_tmp166.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)
# 272
Cyc_implode(_tmp1E)),((_tmp167[0]=& _tmp166,((_tmp167[1]=& _tmp165,Cyc_Tcutil_terr(
loc,((_tmp168="length modifier '%s' is not allowed with %%%c",_tag_dyneither(
_tmp168,sizeof(char),46))),_tag_dyneither(_tmp167,sizeof(void*),2)))))))))))));}{
# 273
struct Cyc_List_List*_tmp6D=0;_npop_handler(0);return _tmp6D;};_LL58:;}
# 275
break;case 'c': _LL57:
# 277
{struct Cyc_List_List*f=_tmp1B;for(0;f != 0;f=f->tl){
# 278
if((int)f->hd == '#'  || (int)f->hd == '0'){
# 279
{const char*_tmp16C;void*_tmp16B[1];struct Cyc_Int_pa_PrintArg_struct _tmp16A;(
_tmp16A.tag=1,((_tmp16A.f1=(unsigned long)((int)f->hd),((_tmp16B[0]=& _tmp16A,Cyc_Tcutil_terr(
loc,((_tmp16C="flag '%c' not allowed with %%c",_tag_dyneither(_tmp16C,sizeof(
char),31))),_tag_dyneither(_tmp16B,sizeof(void*),1)))))));}{
# 280
struct Cyc_List_List*_tmp71=0;_npop_handler(0);return _tmp71;};}}}
# 284
if(_tmp1E != 0){
# 285
{const char*_tmp170;void*_tmp16F[1];struct Cyc_String_pa_PrintArg_struct _tmp16E;(
_tmp16E.tag=0,((_tmp16E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 286
Cyc_implode(_tmp1E)),((_tmp16F[0]=& _tmp16E,Cyc_Tcutil_terr(loc,((_tmp170="length modifier '%s' not allowed with %%c",
_tag_dyneither(_tmp170,sizeof(char),42))),_tag_dyneither(_tmp16F,sizeof(void*),1)))))));}{
# 287
struct Cyc_List_List*_tmp75=0;_npop_handler(0);return _tmp75;};}
# 289
if(_tmp1D != 0){
# 290
{const char*_tmp174;void*_tmp173[1];struct Cyc_String_pa_PrintArg_struct _tmp172;(
_tmp172.tag=0,((_tmp172.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 291
Cyc_implode(_tmp1D)),((_tmp173[0]=& _tmp172,Cyc_Tcutil_terr(loc,((_tmp174="precision '%s' not allowed with %%c",
_tag_dyneither(_tmp174,sizeof(char),36))),_tag_dyneither(_tmp173,sizeof(void*),1)))))));}{
# 292
struct Cyc_List_List*_tmp79=0;_npop_handler(0);return _tmp79;};}
# 294
{struct Cyc_List_List*_tmp175;_tmp13=((_tmp175=_region_malloc(r,sizeof(*_tmp175)),((
_tmp175->hd=Cyc_Absyn_sint_typ,((_tmp175->tl=_tmp13,_tmp175))))));}
# 295
break;case 's': _LL5F:
# 298
{struct Cyc_List_List*f=_tmp1B;for(0;f != 0;f=f->tl){
# 299
if((int)f->hd != '-'){
# 300
{const char*_tmp178;void*_tmp177;(_tmp177=0,Cyc_Tcutil_terr(loc,((_tmp178="a flag not allowed with %%s",
_tag_dyneither(_tmp178,sizeof(char),28))),_tag_dyneither(_tmp177,sizeof(void*),0)));}{
# 301
struct Cyc_List_List*_tmp7D=0;_npop_handler(0);return _tmp7D;};}}}
# 305
if(_tmp1E != 0){
# 306
{const char*_tmp17B;void*_tmp17A;(_tmp17A=0,Cyc_Tcutil_terr(loc,((_tmp17B="length modifiers not allowed with %%s",
_tag_dyneither(_tmp17B,sizeof(char),38))),_tag_dyneither(_tmp17A,sizeof(void*),0)));}{
# 307
struct Cyc_List_List*_tmp80=0;_npop_handler(0);return _tmp80;};}{
# 311
struct Cyc_Core_Opt*_tmp17C;void*ptr=Cyc_Absyn_dyneither_typ(Cyc_Absyn_char_typ,
# 312
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,(
# 313
(_tmp17C=_cycalloc(sizeof(*_tmp17C)),((_tmp17C->v=Cyc_Tcenv_lookup_type_vars(te),
_tmp17C))))),
# 314
Cyc_Absyn_const_tqual(0),Cyc_Absyn_false_conref);
# 315
{struct Cyc_List_List*_tmp17D;_tmp13=((_tmp17D=_region_malloc(r,sizeof(*_tmp17D)),((
_tmp17D->hd=ptr,((_tmp17D->tl=_tmp13,_tmp17D))))));}
# 316
break;};case 'p': _LL60:
# 319
{struct Cyc_List_List*_tmp17E;_tmp13=((_tmp17E=_region_malloc(r,sizeof(*_tmp17E)),((
_tmp17E->hd=Cyc_Absyn_uint_typ,((_tmp17E->tl=_tmp13,_tmp17E))))));}
# 320
break;case 'n': _LL61:
# 322
{struct Cyc_List_List*f=_tmp1B;for(0;f != 0;f=f->tl){
# 323
if((int)f->hd == '#'  || (int)f->hd == '0'){
# 324
{const char*_tmp182;void*_tmp181[1];struct Cyc_Int_pa_PrintArg_struct _tmp180;(
_tmp180.tag=1,((_tmp180.f1=(unsigned long)((int)f->hd),((_tmp181[0]=& _tmp180,Cyc_Tcutil_terr(
loc,((_tmp182="flag '%c' not allowed with %%n",_tag_dyneither(_tmp182,sizeof(
char),31))),_tag_dyneither(_tmp181,sizeof(void*),1)))))));}{
# 325
struct Cyc_List_List*_tmp87=0;_npop_handler(0);return _tmp87;};}}}
# 327
if(_tmp1D != 0){
# 328
{const char*_tmp186;void*_tmp185[1];struct Cyc_String_pa_PrintArg_struct _tmp184;(
_tmp184.tag=0,((_tmp184.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 329
Cyc_implode(_tmp1D)),((_tmp185[0]=& _tmp184,Cyc_Tcutil_terr(loc,((_tmp186="precision '%s' not allowed with %%n",
_tag_dyneither(_tmp186,sizeof(char),36))),_tag_dyneither(_tmp185,sizeof(void*),1)))))));}{
# 330
struct Cyc_List_List*_tmp8B=0;_npop_handler(0);return _tmp8B;};}{
# 332
struct Cyc_Core_Opt*_tmp187;struct Cyc_Core_Opt*_tmp8C=(_tmp187=_cycalloc(sizeof(*
_tmp187)),((_tmp187->v=Cyc_Tcenv_lookup_type_vars(te),_tmp187)));
# 333
{struct Cyc_List_List*_tmp8D=_tmp1E;struct Cyc_List_List _tmp8E;int _tmp8F;struct
Cyc_List_List*_tmp90;struct Cyc_List_List _tmp91;int _tmp92;struct Cyc_List_List*
_tmp93;struct Cyc_List_List _tmp94;int _tmp95;struct Cyc_List_List*_tmp96;struct Cyc_List_List
_tmp97;int _tmp98;struct Cyc_List_List*_tmp99;_LL64: if(_tmp8D != 0)goto _LL66;_LL65:
# 334
 t=Cyc_Absyn_sint_typ;goto _LL63;_LL66: if(_tmp8D == 0)goto _LL68;_tmp8E=*_tmp8D;
_tmp8F=(int)_tmp8E.hd;_tmp90=_tmp8E.tl;if(_tmp90 != 0)goto _LL68;if(!(_tmp8F == 'l'))
goto _LL68;_LL67:
# 336
 t=Cyc_Absyn_ulong_typ;goto _LL63;_LL68: if(_tmp8D == 0)goto _LL6A;_tmp91=*_tmp8D;
_tmp92=(int)_tmp91.hd;_tmp93=_tmp91.tl;if(_tmp93 != 0)goto _LL6A;if(!(_tmp92 == 'h'))
goto _LL6A;_LL69:
# 337
 t=Cyc_Absyn_sshort_typ;goto _LL63;_LL6A: if(_tmp8D == 0)goto _LL6C;_tmp94=*_tmp8D;
_tmp95=(int)_tmp94.hd;_tmp96=_tmp94.tl;if(_tmp96 == 0)goto _LL6C;_tmp97=*_tmp96;
_tmp98=(int)_tmp97.hd;_tmp99=_tmp97.tl;if(_tmp99 != 0)goto _LL6C;if(!(
# 338
_tmp95 == 'h'  && _tmp98 == 'h'))goto _LL6C;_LL6B:
# 339
 t=Cyc_Absyn_schar_typ;goto _LL63;_LL6C:;_LL6D:
# 341
{const char*_tmp18C;void*_tmp18B[2];struct Cyc_String_pa_PrintArg_struct _tmp18A;
struct Cyc_Int_pa_PrintArg_struct _tmp189;(_tmp189.tag=1,((_tmp189.f1=(
unsigned long)((int)_tmp1F),((_tmp18A.tag=0,((_tmp18A.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)
# 342
Cyc_implode(_tmp1E)),((_tmp18B[0]=& _tmp18A,((_tmp18B[1]=& _tmp189,Cyc_Tcutil_terr(
loc,((_tmp18C="length modifier '%s' is not allowed with %%%c",_tag_dyneither(
_tmp18C,sizeof(char),46))),_tag_dyneither(_tmp18B,sizeof(void*),2)))))))))))));}{
# 343
struct Cyc_List_List*_tmp9E=0;_npop_handler(0);return _tmp9E;};_LL63:;}
# 345
t=Cyc_Absyn_at_typ(t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,
_tmp8C),Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);
# 346
{struct Cyc_List_List*_tmp18D;_tmp13=((_tmp18D=_region_malloc(r,sizeof(*_tmp18D)),((
_tmp18D->hd=t,((_tmp18D->tl=_tmp13,_tmp18D))))));}
# 347
break;};case '%': _LL62:
# 349
 if(_tmp1B != 0){
# 350
{const char*_tmp191;void*_tmp190[1];struct Cyc_String_pa_PrintArg_struct _tmp18F;(
_tmp18F.tag=0,((_tmp18F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(
_tmp1B)),((_tmp190[0]=& _tmp18F,Cyc_Tcutil_terr(loc,((_tmp191="flags '%s' not allowed with %%%%",
_tag_dyneither(_tmp191,sizeof(char),33))),_tag_dyneither(_tmp190,sizeof(void*),1)))))));}{
# 351
struct Cyc_List_List*_tmpA4=0;_npop_handler(0);return _tmpA4;};}
# 353
if(_tmp1C != 0){
# 354
{const char*_tmp195;void*_tmp194[1];struct Cyc_String_pa_PrintArg_struct _tmp193;(
_tmp193.tag=0,((_tmp193.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(
_tmp1C)),((_tmp194[0]=& _tmp193,Cyc_Tcutil_terr(loc,((_tmp195="width '%s' not allowed with %%%%",
_tag_dyneither(_tmp195,sizeof(char),33))),_tag_dyneither(_tmp194,sizeof(void*),1)))))));}{
# 355
struct Cyc_List_List*_tmpA8=0;_npop_handler(0);return _tmpA8;};}
# 357
if(_tmp1D != 0){
# 358
{const char*_tmp199;void*_tmp198[1];struct Cyc_String_pa_PrintArg_struct _tmp197;(
_tmp197.tag=0,((_tmp197.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(
_tmp1D)),((_tmp198[0]=& _tmp197,Cyc_Tcutil_terr(loc,((_tmp199="precision '%s' not allowed with %%%%",
_tag_dyneither(_tmp199,sizeof(char),37))),_tag_dyneither(_tmp198,sizeof(void*),1)))))));}{
# 359
struct Cyc_List_List*_tmpAC=0;_npop_handler(0);return _tmpAC;};}
# 361
if(_tmp1E != 0){
# 362
{const char*_tmp19D;void*_tmp19C[1];struct Cyc_String_pa_PrintArg_struct _tmp19B;(
_tmp19B.tag=0,((_tmp19B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(
_tmp1E)),((_tmp19C[0]=& _tmp19B,Cyc_Tcutil_terr(loc,((_tmp19D="length modifier '%s' not allowed with %%%%",
_tag_dyneither(_tmp19D,sizeof(char),43))),_tag_dyneither(_tmp19C,sizeof(void*),1)))))));}{
# 363
struct Cyc_List_List*_tmpB0=0;_npop_handler(0);return _tmpB0;};}
# 365
break;default: _LL6E: {
# 367
struct Cyc_List_List*_tmpB1=0;_npop_handler(0);return _tmpB1;}}};};};}{
# 370
struct Cyc_List_List*_tmpB2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp13);_npop_handler(0);return _tmpB2;};
# 180
;_pop_region(temp);}struct _tuple11{int f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;char f4;int f5;};
# 381 "/home/jgm/cyclone/src/formatstr.cyc"
struct Cyc_Core_Opt*Cyc_Formatstr_parse_inputformat(struct _RegionHandle*r,struct
_dyneither_ptr s,int i){
# 383
unsigned long _tmpB3=Cyc_strlen((struct _dyneither_ptr)s);
# 384
if(i < 0  || i >= _tmpB3)return 0;{
# 386
int _tmpB4=0;
# 387
char _tmpB5=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
# 388
if(_tmpB5 == '*'){
# 389
_tmpB4=1;
# 390
++ i;
# 391
if(i >= _tmpB3)return 0;}{
# 394
struct Cyc_List_List*_tmpB6=0;
# 395
for(0;i < _tmpB3;++ i){
# 396
_tmpB5=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
# 397
if(isdigit((int)_tmpB5)){struct Cyc_List_List*_tmp19E;_tmpB6=((_tmp19E=
_region_malloc(r,sizeof(*_tmp19E)),((_tmp19E->hd=(void*)((int)_tmpB5),((_tmp19E->tl=
_tmpB6,_tmp19E))))));}else{
# 398
break;}}
# 400
if(i >= _tmpB3)return 0;
# 401
_tmpB6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpB6);{
# 405
struct Cyc_List_List*_tmpB8=0;
# 406
_tmpB5=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
# 407
switch(_tmpB5){case 'h': _LL70:
# 409
{struct Cyc_List_List*_tmp19F;_tmpB8=((_tmp19F=_region_malloc(r,sizeof(*_tmp19F)),((
_tmp19F->hd=(void*)((int)_tmpB5),((_tmp19F->tl=_tmpB8,_tmp19F))))));}
# 410
++ i;
# 411
if(i >= _tmpB3)return 0;
# 412
_tmpB5=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
# 413
if(_tmpB5 == 'h'){{struct Cyc_List_List*_tmp1A0;_tmpB8=((_tmp1A0=_region_malloc(r,
sizeof(*_tmp1A0)),((_tmp1A0->hd=(void*)((int)_tmpB5),((_tmp1A0->tl=_tmpB8,
_tmp1A0))))));}++ i;}
# 414
break;case 'l': _LL71:
# 416
{struct Cyc_List_List*_tmp1A1;_tmpB8=((_tmp1A1=_region_malloc(r,sizeof(*_tmp1A1)),((
_tmp1A1->hd=(void*)((int)_tmpB5),((_tmp1A1->tl=_tmpB8,_tmp1A1))))));}
# 417
++ i;
# 418
if(i >= _tmpB3)return 0;
# 419
_tmpB5=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
# 420
if(_tmpB5 == 'l'){{struct Cyc_List_List*_tmp1A2;_tmpB8=((_tmp1A2=_region_malloc(r,
sizeof(*_tmp1A2)),((_tmp1A2->hd=(void*)((int)_tmpB5),((_tmp1A2->tl=_tmpB8,
_tmp1A2))))));}++ i;}
# 421
break;case 'j': _LL72:
# 422
 goto _LL73;case 'z': _LL73:
# 423
 goto _LL74;case 't': _LL74:
# 424
 goto _LL75;case 'L': _LL75:
# 426
{struct Cyc_List_List*_tmp1A3;_tmpB8=((_tmp1A3=_region_malloc(r,sizeof(*_tmp1A3)),((
_tmp1A3->hd=(void*)((int)_tmpB5),((_tmp1A3->tl=_tmpB8,_tmp1A3))))));}
# 427
++ i;
# 428
break;default: _LL76:
# 429
 break;}
# 431
if(i >= _tmpB3)return 0;
# 432
_tmpB8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpB8);
# 435
_tmpB5=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
# 436
switch(_tmpB5){case 'd': _LL78:
# 437
 goto _LL79;case 'i': _LL79:
# 438
 goto _LL7A;case 'o': _LL7A:
# 439
 goto _LL7B;case 'u': _LL7B:
# 440
 goto _LL7C;case 'x': _LL7C:
# 441
 goto _LL7D;case 'X': _LL7D:
# 442
 goto _LL7E;case 'f': _LL7E:
# 443
 goto _LL7F;case 'F': _LL7F:
# 444
 goto _LL80;case 'e': _LL80:
# 445
 goto _LL81;case 'E': _LL81:
# 446
 goto _LL82;case 'g': _LL82:
# 447
 goto _LL83;case 'G': _LL83:
# 448
 goto _LL84;case 'a': _LL84:
# 449
 goto _LL85;case 'A': _LL85:
# 450
 goto _LL86;case 'c': _LL86:
# 451
 goto _LL87;case 's': _LL87:
# 452
 goto _LL88;case 'p': _LL88:
# 453
 goto _LL89;case 'n': _LL89:
# 454
 goto _LL8A;case '%': _LL8A:
# 455
 break;default: _LL8B:
# 456
 return 0;}{
# 458
struct _tuple11*_tmp1A6;struct Cyc_Core_Opt*_tmp1A5;return(_tmp1A5=_region_malloc(
r,sizeof(*_tmp1A5)),((_tmp1A5->v=((_tmp1A6=_region_malloc(r,sizeof(*_tmp1A6)),((
_tmp1A6->f1=_tmpB4,((_tmp1A6->f2=_tmpB6,((_tmp1A6->f3=_tmpB8,((_tmp1A6->f4=
_tmpB5,((_tmp1A6->f5=i + 1,_tmp1A6)))))))))))),_tmp1A5)));};};};};}
# 460
struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*r,struct
Cyc_Tcenv_Tenv*te,struct _dyneither_ptr s,unsigned int loc){
# 463
unsigned long _tmpC0=Cyc_strlen((struct _dyneither_ptr)s);
# 464
struct Cyc_List_List*_tmpC1=0;
# 465
int i;
# 466
struct _RegionHandle _tmpC2=_new_region("temp");struct _RegionHandle*temp=& _tmpC2;
_push_region(temp);
# 467
for(i=0;i < _tmpC0;++ i){
# 468
if(*((const char*)_check_dyneither_subscript(s,sizeof(char),i))!= '%')continue;{
# 469
struct Cyc_Core_Opt*_tmpC3=Cyc_Formatstr_parse_inputformat(temp,s,i + 1);
# 470
if(_tmpC3 == 0){
# 471
{const char*_tmp1A9;void*_tmp1A8;(_tmp1A8=0,Cyc_Tcutil_terr(loc,((_tmp1A9="bad format string",
_tag_dyneither(_tmp1A9,sizeof(char),18))),_tag_dyneither(_tmp1A8,sizeof(void*),0)));}{
# 472
struct Cyc_List_List*_tmpC6=0;_npop_handler(0);return _tmpC6;};}{
# 474
struct _tuple11 _tmpC8;int _tmpC9;struct Cyc_List_List*_tmpCA;struct Cyc_List_List*
_tmpCB;char _tmpCC;int _tmpCD;struct _tuple11*_tmpC7=(struct _tuple11*)_tmpC3->v;
_tmpC8=*_tmpC7;_tmpC9=_tmpC8.f1;_tmpCA=_tmpC8.f2;_tmpCB=_tmpC8.f3;_tmpCC=_tmpC8.f4;
_tmpCD=_tmpC8.f5;
# 475
i=_tmpCD - 1;
# 476
{struct Cyc_List_List*_tmpCE=_tmpCB;struct Cyc_List_List _tmpCF;int _tmpD0;struct
Cyc_List_List*_tmpD1;_LL8E: if(_tmpCE == 0)goto _LL90;_tmpCF=*_tmpCE;_tmpD0=(int)
_tmpCF.hd;_tmpD1=_tmpCF.tl;if(_tmpD1 != 0)goto _LL90;if(!(
# 477
(_tmpD0 == 'j'  || _tmpD0 == 'z') || _tmpD0 == 't'))goto _LL90;_LL8F:
# 479
{const char*_tmp1AD;void*_tmp1AC[1];struct Cyc_Int_pa_PrintArg_struct _tmp1AB;(
_tmp1AB.tag=1,((_tmp1AB.f1=(unsigned long)_tmpD0,((_tmp1AC[0]=& _tmp1AB,Cyc_Tcutil_terr(
loc,((_tmp1AD="length modifier '%c' is not supported",_tag_dyneither(_tmp1AD,
sizeof(char),38))),_tag_dyneither(_tmp1AC,sizeof(void*),1)))))));}{
# 480
struct Cyc_List_List*_tmpD5=0;_npop_handler(0);return _tmpD5;};_LL90:;_LL91:
# 481
 goto _LL8D;_LL8D:;}
# 483
if(_tmpC9)continue;{
# 484
void*t;
# 485
switch(_tmpCC){case 'd': _LL92:
# 486
 goto _LL93;case 'i': _LL93: {
# 488
struct Cyc_Core_Opt*_tmp1AE;struct Cyc_Core_Opt*_tmpD6=(_tmp1AE=_cycalloc(sizeof(*
_tmp1AE)),((_tmp1AE->v=Cyc_Tcenv_lookup_type_vars(te),_tmp1AE)));
# 489
{struct Cyc_List_List*_tmpD7=_tmpCB;struct Cyc_List_List _tmpD8;int _tmpD9;struct
Cyc_List_List*_tmpDA;struct Cyc_List_List _tmpDB;int _tmpDC;struct Cyc_List_List*
_tmpDD;struct Cyc_List_List _tmpDE;int _tmpDF;struct Cyc_List_List*_tmpE0;struct Cyc_List_List
_tmpE1;int _tmpE2;struct Cyc_List_List*_tmpE3;_LL96: if(_tmpD7 != 0)goto _LL98;_LL97:
# 490
 t=Cyc_Absyn_sint_typ;goto _LL95;_LL98: if(_tmpD7 == 0)goto _LL9A;_tmpD8=*_tmpD7;
_tmpD9=(int)_tmpD8.hd;_tmpDA=_tmpD8.tl;if(_tmpDA != 0)goto _LL9A;if(!(_tmpD9 == 'l'))
goto _LL9A;_LL99:
# 491
 t=Cyc_Absyn_slong_typ;goto _LL95;_LL9A: if(_tmpD7 == 0)goto _LL9C;_tmpDB=*_tmpD7;
_tmpDC=(int)_tmpDB.hd;_tmpDD=_tmpDB.tl;if(_tmpDD != 0)goto _LL9C;if(!(_tmpDC == 'h'))
goto _LL9C;_LL9B:
# 492
 t=Cyc_Absyn_sshort_typ;goto _LL95;_LL9C: if(_tmpD7 == 0)goto _LL9E;_tmpDE=*_tmpD7;
_tmpDF=(int)_tmpDE.hd;_tmpE0=_tmpDE.tl;if(_tmpE0 == 0)goto _LL9E;_tmpE1=*_tmpE0;
_tmpE2=(int)_tmpE1.hd;_tmpE3=_tmpE1.tl;if(_tmpE3 != 0)goto _LL9E;if(!(
# 493
_tmpDF == 'h'  && _tmpE2 == 'h'))goto _LL9E;_LL9D: t=Cyc_Absyn_schar_typ;goto _LL95;
_LL9E:;_LL9F:
# 495
{const char*_tmp1B3;void*_tmp1B2[2];struct Cyc_String_pa_PrintArg_struct _tmp1B1;
struct Cyc_Int_pa_PrintArg_struct _tmp1B0;(_tmp1B0.tag=1,((_tmp1B0.f1=(
unsigned long)((int)_tmpCC),((_tmp1B1.tag=0,((_tmp1B1.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)
# 496
Cyc_implode(_tmpCB)),((_tmp1B2[0]=& _tmp1B1,((_tmp1B2[1]=& _tmp1B0,Cyc_Tcutil_terr(
loc,((_tmp1B3="length modifier '%s' is not allowed with %%%c",_tag_dyneither(
_tmp1B3,sizeof(char),46))),_tag_dyneither(_tmp1B2,sizeof(void*),2)))))))))))));}{
# 497
struct Cyc_List_List*_tmpE8=0;_npop_handler(0);return _tmpE8;};_LL95:;}
# 499
t=Cyc_Absyn_at_typ(t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,
_tmpD6),Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);
# 500
{struct Cyc_List_List*_tmp1B4;_tmpC1=((_tmp1B4=_region_malloc(r,sizeof(*_tmp1B4)),((
_tmp1B4->hd=t,((_tmp1B4->tl=_tmpC1,_tmp1B4))))));}
# 501
break;}case 'u': _LL94:
# 502
 goto _LLA0;case 'o': _LLA0:
# 503
 goto _LLA1;case 'x': _LLA1:
# 504
 goto _LLA2;case 'X': _LLA2: {
# 506
struct Cyc_Core_Opt*_tmp1B5;struct Cyc_Core_Opt*_tmpEB=(_tmp1B5=_cycalloc(sizeof(*
_tmp1B5)),((_tmp1B5->v=Cyc_Tcenv_lookup_type_vars(te),_tmp1B5)));
# 507
{struct Cyc_List_List*_tmpEC=_tmpCB;struct Cyc_List_List _tmpED;int _tmpEE;struct
Cyc_List_List*_tmpEF;struct Cyc_List_List _tmpF0;int _tmpF1;struct Cyc_List_List*
_tmpF2;struct Cyc_List_List _tmpF3;int _tmpF4;struct Cyc_List_List*_tmpF5;struct Cyc_List_List
_tmpF6;int _tmpF7;struct Cyc_List_List*_tmpF8;_LLA5: if(_tmpEC != 0)goto _LLA7;_LLA6:
# 508
 t=Cyc_Absyn_uint_typ;goto _LLA4;_LLA7: if(_tmpEC == 0)goto _LLA9;_tmpED=*_tmpEC;
_tmpEE=(int)_tmpED.hd;_tmpEF=_tmpED.tl;if(_tmpEF != 0)goto _LLA9;if(!(_tmpEE == 'l'))
goto _LLA9;_LLA8:
# 509
 t=Cyc_Absyn_ulong_typ;goto _LLA4;_LLA9: if(_tmpEC == 0)goto _LLAB;_tmpF0=*_tmpEC;
_tmpF1=(int)_tmpF0.hd;_tmpF2=_tmpF0.tl;if(_tmpF2 != 0)goto _LLAB;if(!(_tmpF1 == 'h'))
goto _LLAB;_LLAA:
# 510
 t=Cyc_Absyn_ushort_typ;goto _LLA4;_LLAB: if(_tmpEC == 0)goto _LLAD;_tmpF3=*_tmpEC;
_tmpF4=(int)_tmpF3.hd;_tmpF5=_tmpF3.tl;if(_tmpF5 == 0)goto _LLAD;_tmpF6=*_tmpF5;
_tmpF7=(int)_tmpF6.hd;_tmpF8=_tmpF6.tl;if(_tmpF8 != 0)goto _LLAD;if(!(
# 511
_tmpF4 == 'h'  && _tmpF7 == 'h'))goto _LLAD;_LLAC: t=Cyc_Absyn_uchar_typ;goto _LLA4;
_LLAD:;_LLAE:
# 513
{const char*_tmp1BA;void*_tmp1B9[2];struct Cyc_String_pa_PrintArg_struct _tmp1B8;
struct Cyc_Int_pa_PrintArg_struct _tmp1B7;(_tmp1B7.tag=1,((_tmp1B7.f1=(
unsigned long)((int)_tmpCC),((_tmp1B8.tag=0,((_tmp1B8.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)
# 514
Cyc_implode(_tmpCB)),((_tmp1B9[0]=& _tmp1B8,((_tmp1B9[1]=& _tmp1B7,Cyc_Tcutil_terr(
loc,((_tmp1BA="length modifier '%s' is not allowed with %%%c",_tag_dyneither(
_tmp1BA,sizeof(char),46))),_tag_dyneither(_tmp1B9,sizeof(void*),2)))))))))))));}{
# 515
struct Cyc_List_List*_tmpFD=0;_npop_handler(0);return _tmpFD;};_LLA4:;}
# 517
t=Cyc_Absyn_at_typ(t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,
_tmpEB),Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);
# 518
{struct Cyc_List_List*_tmp1BB;_tmpC1=((_tmp1BB=_region_malloc(r,sizeof(*_tmp1BB)),((
_tmp1BB->hd=t,((_tmp1BB->tl=_tmpC1,_tmp1BB))))));}
# 519
break;}case 'f': _LLA3:
# 520
 goto _LLAF;case 'F': _LLAF:
# 521
 goto _LLB0;case 'e': _LLB0:
# 522
 goto _LLB1;case 'E': _LLB1:
# 523
 goto _LLB2;case 'g': _LLB2:
# 524
 goto _LLB3;case 'G': _LLB3:
# 525
 goto _LLB4;case 'a': _LLB4:
# 526
 goto _LLB5;case 'A': _LLB5: {
# 528
struct Cyc_Core_Opt*_tmp1BC;struct Cyc_Core_Opt*_tmp100=(_tmp1BC=_cycalloc(
sizeof(*_tmp1BC)),((_tmp1BC->v=Cyc_Tcenv_lookup_type_vars(te),_tmp1BC)));
# 529
{struct Cyc_List_List*_tmp101=_tmpCB;struct Cyc_List_List _tmp102;int _tmp103;
struct Cyc_List_List*_tmp104;_LLB8: if(_tmp101 != 0)goto _LLBA;_LLB9:
# 530
 t=Cyc_Absyn_float_typ(0);goto _LLB7;_LLBA: if(_tmp101 == 0)goto _LLBC;_tmp102=*
_tmp101;_tmp103=(int)_tmp102.hd;_tmp104=_tmp102.tl;if(_tmp104 != 0)goto _LLBC;if(!(
_tmp103 == 'l'))goto _LLBC;_LLBB:
# 532
 t=Cyc_Absyn_float_typ(1);goto _LLB7;_LLBC:;_LLBD:
# 534
{const char*_tmp1C1;void*_tmp1C0[2];struct Cyc_String_pa_PrintArg_struct _tmp1BF;
struct Cyc_Int_pa_PrintArg_struct _tmp1BE;(_tmp1BE.tag=1,((_tmp1BE.f1=(
unsigned long)((int)_tmpCC),((_tmp1BF.tag=0,((_tmp1BF.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)
# 535
Cyc_implode(_tmpCB)),((_tmp1C0[0]=& _tmp1BF,((_tmp1C0[1]=& _tmp1BE,Cyc_Tcutil_terr(
loc,((_tmp1C1="length modifier '%s' is not allowed with %%%c",_tag_dyneither(
_tmp1C1,sizeof(char),46))),_tag_dyneither(_tmp1C0,sizeof(void*),2)))))))))))));}{
# 536
struct Cyc_List_List*_tmp109=0;_npop_handler(0);return _tmp109;};_LLB7:;}
# 538
t=Cyc_Absyn_at_typ(t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,
_tmp100),Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);
# 539
{struct Cyc_List_List*_tmp1C2;_tmpC1=((_tmp1C2=_region_malloc(r,sizeof(*_tmp1C2)),((
_tmp1C2->hd=t,((_tmp1C2->tl=_tmpC1,_tmp1C2))))));}
# 540
break;}case 'c': _LLB6: {
# 543
struct Cyc_Core_Opt*_tmp1C3;struct Cyc_Core_Opt*_tmp10C=(_tmp1C3=_cycalloc(
sizeof(*_tmp1C3)),((_tmp1C3->v=Cyc_Tcenv_lookup_type_vars(te),_tmp1C3)));
# 544
{struct Cyc_List_List*_tmp1C4;_tmpC1=((_tmp1C4=_region_malloc(r,sizeof(*_tmp1C4)),((
_tmp1C4->hd=Cyc_Absyn_dyneither_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,_tmp10C),
# 545
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),((_tmp1C4->tl=_tmpC1,_tmp1C4))))));}
# 547
break;}case 's': _LLBE: {
# 549
struct Cyc_Core_Opt*_tmp1C5;struct Cyc_Core_Opt*_tmp10F=(_tmp1C5=_cycalloc(
sizeof(*_tmp1C5)),((_tmp1C5->v=Cyc_Tcenv_lookup_type_vars(te),_tmp1C5)));
# 551
void*ptr=Cyc_Absyn_dyneither_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_rko,_tmp10F),
# 552
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);
# 553
{struct Cyc_List_List*_tmp1C6;_tmpC1=((_tmp1C6=_region_malloc(r,sizeof(*_tmp1C6)),((
_tmp1C6->hd=ptr,((_tmp1C6->tl=_tmpC1,_tmp1C6))))));}
# 554
break;}case '[': _LLBF:
# 555
 goto _LLC0;case 'p': _LLC0:
# 557
{const char*_tmp1CA;void*_tmp1C9[1];struct Cyc_Int_pa_PrintArg_struct _tmp1C8;(
_tmp1C8.tag=1,((_tmp1C8.f1=(unsigned long)((int)_tmpCC),((_tmp1C9[0]=& _tmp1C8,
Cyc_Tcutil_terr(loc,((_tmp1CA="%%%c is not supported",_tag_dyneither(_tmp1CA,
sizeof(char),22))),_tag_dyneither(_tmp1C9,sizeof(void*),1)))))));}{
# 558
struct Cyc_List_List*_tmp115=0;_npop_handler(0);return _tmp115;};case 'n': _LLC1: {
# 560
struct Cyc_Core_Opt*_tmp1CB;struct Cyc_Core_Opt*_tmp116=(_tmp1CB=_cycalloc(
sizeof(*_tmp1CB)),((_tmp1CB->v=Cyc_Tcenv_lookup_type_vars(te),_tmp1CB)));
# 561
{struct Cyc_List_List*_tmp117=_tmpCB;struct Cyc_List_List _tmp118;int _tmp119;
struct Cyc_List_List*_tmp11A;struct Cyc_List_List _tmp11B;int _tmp11C;struct Cyc_List_List*
_tmp11D;struct Cyc_List_List _tmp11E;int _tmp11F;struct Cyc_List_List*_tmp120;struct
Cyc_List_List _tmp121;int _tmp122;struct Cyc_List_List*_tmp123;_LLC4: if(_tmp117 != 0)
goto _LLC6;_LLC5:
# 562
 t=Cyc_Absyn_sint_typ;goto _LLC3;_LLC6: if(_tmp117 == 0)goto _LLC8;_tmp118=*_tmp117;
_tmp119=(int)_tmp118.hd;_tmp11A=_tmp118.tl;if(_tmp11A != 0)goto _LLC8;if(!(_tmp119
== 'l'))goto _LLC8;_LLC7:
# 563
 t=Cyc_Absyn_ulong_typ;goto _LLC3;_LLC8: if(_tmp117 == 0)goto _LLCA;_tmp11B=*_tmp117;
_tmp11C=(int)_tmp11B.hd;_tmp11D=_tmp11B.tl;if(_tmp11D != 0)goto _LLCA;if(!(_tmp11C
== 'h'))goto _LLCA;_LLC9:
# 564
 t=Cyc_Absyn_sshort_typ;goto _LLC3;_LLCA: if(_tmp117 == 0)goto _LLCC;_tmp11E=*
_tmp117;_tmp11F=(int)_tmp11E.hd;_tmp120=_tmp11E.tl;if(_tmp120 == 0)goto _LLCC;
_tmp121=*_tmp120;_tmp122=(int)_tmp121.hd;_tmp123=_tmp121.tl;if(_tmp123 != 0)goto
_LLCC;if(!(
# 565
_tmp11F == 'h'  && _tmp122 == 'h'))goto _LLCC;_LLCB: t=Cyc_Absyn_schar_typ;goto _LLC3;
_LLCC:;_LLCD:
# 567
{const char*_tmp1D0;void*_tmp1CF[2];struct Cyc_String_pa_PrintArg_struct _tmp1CE;
struct Cyc_Int_pa_PrintArg_struct _tmp1CD;(_tmp1CD.tag=1,((_tmp1CD.f1=(
unsigned long)((int)_tmpCC),((_tmp1CE.tag=0,((_tmp1CE.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)
# 568
Cyc_implode(_tmpCB)),((_tmp1CF[0]=& _tmp1CE,((_tmp1CF[1]=& _tmp1CD,Cyc_Tcutil_terr(
loc,((_tmp1D0="length modifier '%s' is not allowed with %%%c",_tag_dyneither(
_tmp1D0,sizeof(char),46))),_tag_dyneither(_tmp1CF,sizeof(void*),2)))))))))))));}{
# 569
struct Cyc_List_List*_tmp128=0;_npop_handler(0);return _tmp128;};_LLC3:;}
# 571
t=Cyc_Absyn_at_typ(t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,
_tmp116),Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);
# 572
{struct Cyc_List_List*_tmp1D1;_tmpC1=((_tmp1D1=_region_malloc(r,sizeof(*_tmp1D1)),((
_tmp1D1->hd=t,((_tmp1D1->tl=_tmpC1,_tmp1D1))))));}
# 573
break;}case '%': _LLC2:
# 575
 if(_tmpC9){
# 576
{const char*_tmp1D4;void*_tmp1D3;(_tmp1D3=0,Cyc_Tcutil_terr(loc,((_tmp1D4="Assignment suppression (*) is not allowed with %%%%",
_tag_dyneither(_tmp1D4,sizeof(char),52))),_tag_dyneither(_tmp1D3,sizeof(void*),0)));}{
# 577
struct Cyc_List_List*_tmp12D=0;_npop_handler(0);return _tmp12D;};}
# 579
if(_tmpCA != 0){
# 580
{const char*_tmp1D8;void*_tmp1D7[1];struct Cyc_String_pa_PrintArg_struct _tmp1D6;(
_tmp1D6.tag=0,((_tmp1D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(
_tmpCA)),((_tmp1D7[0]=& _tmp1D6,Cyc_Tcutil_terr(loc,((_tmp1D8="width '%s' not allowed with %%%%",
_tag_dyneither(_tmp1D8,sizeof(char),33))),_tag_dyneither(_tmp1D7,sizeof(void*),1)))))));}{
# 581
struct Cyc_List_List*_tmp131=0;_npop_handler(0);return _tmp131;};}
# 583
if(_tmpCB != 0){
# 584
{const char*_tmp1DC;void*_tmp1DB[1];struct Cyc_String_pa_PrintArg_struct _tmp1DA;(
_tmp1DA.tag=0,((_tmp1DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(
_tmpCB)),((_tmp1DB[0]=& _tmp1DA,Cyc_Tcutil_terr(loc,((_tmp1DC="length modifier '%s' not allowed with %%%%",
_tag_dyneither(_tmp1DC,sizeof(char),43))),_tag_dyneither(_tmp1DB,sizeof(void*),1)))))));}{
# 585
struct Cyc_List_List*_tmp135=0;_npop_handler(0);return _tmp135;};}
# 587
break;default: _LLCE: {
# 589
struct Cyc_List_List*_tmp136=0;_npop_handler(0);return _tmp136;}}};};};}{
# 593
struct Cyc_List_List*_tmp137=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmpC1);_npop_handler(0);return _tmp137;};
# 466
;_pop_region(temp);}
