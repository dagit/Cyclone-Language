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
typedef void*Cyc_parg_t;struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};
# 158 "core.h"
extern struct Cyc_Core_Not_found_exn_struct Cyc_Core_Not_found_val;extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 160
extern struct _RegionHandle*Cyc_Core_heap_region;
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
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 64
void*Cyc_List_hd(struct Cyc_List_List*x);
# 67
struct Cyc_List_List*Cyc_List_tl(struct Cyc_List_List*x);
# 70
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
# 72
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 94
struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 100
struct Cyc_List_List*Cyc_List_rmap2(struct _RegionHandle*,void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 105
void Cyc_List_app(void*(*f)(void*),struct Cyc_List_List*x);
# 108
void Cyc_List_app_c(void*(*f)(void*,void*),void*,struct Cyc_List_List*x);
# 112
void Cyc_List_app2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 117
void Cyc_List_app2_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 121
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 123
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 127
void Cyc_List_iter2(void(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 129
void Cyc_List_iter2_c(void(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 133
void*Cyc_List_fold_left(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x);
# 137
void*Cyc_List_fold_left_c(void*(*f)(void*,void*,void*),void*,void*accum,struct Cyc_List_List*x);
# 141
void*Cyc_List_fold_right(void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum);
# 145
void*Cyc_List_fold_right_c(void*(*f)(void*,void*,void*),void*,struct Cyc_List_List*x,void*accum);
# 149
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 155
struct Cyc_List_List*Cyc_List_rrevappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 160
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 163
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*x);
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 178
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 183
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 189
struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*x);
# 193
struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*,struct Cyc_List_List*x);
# 198
struct Cyc_List_List*Cyc_List_merge_sort(int(*cmp)(void*,void*),struct Cyc_List_List*x);
# 202
struct Cyc_List_List*Cyc_List_rmerge_sort(struct _RegionHandle*,int(*cmp)(void*,void*),struct Cyc_List_List*x);
# 208
struct Cyc_List_List*Cyc_List_rimp_merge_sort(int(*cmp)(void*,void*),struct Cyc_List_List*x);
# 215
struct Cyc_List_List*Cyc_List_merge(int(*cmp)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 218
struct Cyc_List_List*Cyc_List_rmerge(struct _RegionHandle*,int(*cmp)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b);
# 222
struct Cyc_List_List*Cyc_List_imp_merge(int(*cmp)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 230
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 234
struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i);
# 239
int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);
# 242
int Cyc_List_forall_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 246
int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);
# 249
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 253
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 259
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 264
struct Cyc_List_List*Cyc_List_zip3(struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z);
# 271
struct Cyc_List_List*Cyc_List_rzip3(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 277
struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 281
struct _tuple1 Cyc_List_split3(struct Cyc_List_List*x);
# 286
struct _tuple0 Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);
# 293
struct _tuple1 Cyc_List_rsplit3(struct _RegionHandle*r3,struct _RegionHandle*r4,struct _RegionHandle*r5,struct Cyc_List_List*x);
# 302
int Cyc_List_memq(struct Cyc_List_List*l,void*x);
# 305
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 311
void*Cyc_List_assoc(struct Cyc_List_List*l,void*k);
# 319
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 325
int Cyc_List_mem_assoc(struct Cyc_List_List*l,void*x);
# 328
int Cyc_List_mem_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 332
struct Cyc_List_List*Cyc_List_delete(struct Cyc_List_List*l,void*x);
# 337
struct Cyc_List_List*Cyc_List_delete_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 342
struct Cyc_Core_Opt*Cyc_List_check_unique(int(*cmp)(void*,void*),struct Cyc_List_List*x);
# 347
struct _dyneither_ptr Cyc_List_to_array(struct Cyc_List_List*x);
# 350
struct _dyneither_ptr Cyc_List_rto_array(struct _RegionHandle*r,struct Cyc_List_List*x);
# 354
struct Cyc_List_List*Cyc_List_from_array(struct _dyneither_ptr arr);
# 357
struct Cyc_List_List*Cyc_List_rfrom_array(struct _RegionHandle*r2,struct _dyneither_ptr arr);
# 361
struct Cyc_List_List*Cyc_List_tabulate(int n,void*(*f)(int));
# 362
struct Cyc_List_List*Cyc_List_tabulate_c(int n,void*(*f)(void*,int),void*env);
# 363
struct Cyc_List_List*Cyc_List_rtabulate(struct _RegionHandle*r,int n,void*(*f)(int));
# 364
struct Cyc_List_List*Cyc_List_rtabulate_c(struct _RegionHandle*r,int n,void*(*f)(void*,int),void*env);
# 366
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 370
int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 374
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);
# 377
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 380
struct Cyc_List_List*Cyc_List_rfilter(struct _RegionHandle*r,int(*f)(void*),struct Cyc_List_List*x);
# 383
struct Cyc_List_List*Cyc_List_rfilter_c(struct _RegionHandle*r,int(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 32 "list.cyc"
int Cyc_List_length(struct Cyc_List_List*x){
# 33
int i=0;
# 35
while(x != 0){
# 36
++ i;
# 37
x=x->tl;}
# 39
return i;}
# 43
void*Cyc_List_hd(struct Cyc_List_List*x){
# 44
if((struct Cyc_List_List*)x == 0){struct Cyc_Core_Failure_exn_struct _tmp45;const char*_tmp44;struct Cyc_Core_Failure_exn_struct*_tmp43;(int)_throw((void*)((_tmp43=_cycalloc(sizeof(*_tmp43)),((_tmp43[0]=((_tmp45.tag=Cyc_Core_Failure,((_tmp45.f1=((_tmp44="hd",_tag_dyneither(_tmp44,sizeof(char),3))),_tmp45)))),_tmp43)))));}
# 45
return(void*)x->hd;}
# 49
struct Cyc_List_List*Cyc_List_tl(struct Cyc_List_List*x){
# 50
if((struct Cyc_List_List*)x == 0){struct Cyc_Core_Failure_exn_struct _tmp4B;const char*_tmp4A;struct Cyc_Core_Failure_exn_struct*_tmp49;(int)_throw((void*)((_tmp49=_cycalloc(sizeof(*_tmp49)),((_tmp49[0]=((_tmp4B.tag=Cyc_Core_Failure,((_tmp4B.f1=((_tmp4A="tl",_tag_dyneither(_tmp4A,sizeof(char),3))),_tmp4B)))),_tmp49)))));}
# 51
return x->tl;}
# 56
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*r,struct _dyneither_ptr argv){
# 57
struct Cyc_List_List*result=0;
# 58
{int i=(int)(_get_dyneither_size(argv,sizeof(void*))- 1);for(0;i >= 0;-- i){
# 59
struct Cyc_List_List*_tmp4C;result=((_tmp4C=_region_malloc(r,sizeof(*_tmp4C)),((_tmp4C->hd=(void*)*((void**)_check_dyneither_subscript(argv,sizeof(void*),i)),((_tmp4C->tl=result,_tmp4C))))));}}
# 60
return result;}
# 65
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr argv){
# 66
struct Cyc_List_List*result=0;
# 67
{int i=(int)(_get_dyneither_size(argv,sizeof(void*))- 1);for(0;i >= 0;-- i){
# 68
struct Cyc_List_List*_tmp4D;result=((_tmp4D=_cycalloc(sizeof(*_tmp4D)),((_tmp4D->hd=(void*)*((void**)_check_dyneither_subscript(argv,sizeof(void*),i)),((_tmp4D->tl=result,_tmp4D))))));}}
# 69
return result;}
# 73
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*r2,struct Cyc_List_List*x){
# 74
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 76
if(x == 0)return 0;
# 77
{struct Cyc_List_List*_tmp4E;result=((_tmp4E=_region_malloc(r2,sizeof(*_tmp4E)),((_tmp4E->hd=(void*)((void*)x->hd),((_tmp4E->tl=0,_tmp4E))))));}
# 78
prev=result;
# 79
for(x=x->tl;x != 0;x=x->tl){
# 80
struct Cyc_List_List*_tmp4F;struct Cyc_List_List*temp=(_tmp4F=_region_malloc(r2,sizeof(*_tmp4F)),((_tmp4F->hd=(void*)((void*)x->hd),((_tmp4F->tl=0,_tmp4F)))));
# 81
prev->tl=(struct Cyc_List_List*)temp;
# 82
prev=temp;}
# 84
return(struct Cyc_List_List*)result;}
# 87
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x){
# 88
return Cyc_List_rcopy(Cyc_Core_heap_region,x);}
# 92
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*r2,void*(*f)(void*),struct Cyc_List_List*x){
# 93
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 95
if(x == 0)return 0;
# 96
{struct Cyc_List_List*_tmp50;result=((_tmp50=_region_malloc(r2,sizeof(*_tmp50)),((_tmp50->hd=(void*)f((void*)x->hd),((_tmp50->tl=0,_tmp50))))));}
# 97
prev=result;
# 98
for(x=x->tl;x != 0;x=x->tl){
# 99
struct Cyc_List_List*_tmp51;struct Cyc_List_List*temp=(_tmp51=_region_malloc(r2,sizeof(*_tmp51)),((_tmp51->hd=(void*)f((void*)x->hd),((_tmp51->tl=0,_tmp51)))));
# 100
prev->tl=(struct Cyc_List_List*)temp;
# 101
prev=temp;}
# 103
return(struct Cyc_List_List*)result;}
# 106
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x){
# 107
return Cyc_List_rmap(Cyc_Core_heap_region,f,x);}
# 111
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 112
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 114
if(x == 0)return 0;
# 115
{struct Cyc_List_List*_tmp52;result=((_tmp52=_region_malloc(r2,sizeof(*_tmp52)),((_tmp52->hd=(void*)f(env,(void*)x->hd),((_tmp52->tl=0,_tmp52))))));}
# 116
prev=result;
# 117
for(x=x->tl;x != 0;x=x->tl){
# 118
struct Cyc_List_List*_tmp53;struct Cyc_List_List*e=(_tmp53=_region_malloc(r2,sizeof(*_tmp53)),((_tmp53->hd=(void*)f(env,(void*)x->hd),((_tmp53->tl=0,_tmp53)))));
# 119
prev->tl=(struct Cyc_List_List*)e;
# 120
prev=e;}
# 122
return(struct Cyc_List_List*)result;}
# 125
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 126
return Cyc_List_rmap_c(Cyc_Core_heap_region,f,env,x);}char Cyc_List_List_mismatch[14]="List_mismatch";
# 131
struct Cyc_List_List_mismatch_exn_struct Cyc_List_List_mismatch_val={Cyc_List_List_mismatch};
# 136
struct Cyc_List_List*Cyc_List_rmap2(struct _RegionHandle*r3,void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){
# 138
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 140
if(x == 0  && y == 0)return 0;
# 141
if(x == 0  || y == 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
# 143
{struct Cyc_List_List*_tmp54;result=((_tmp54=_region_malloc(r3,sizeof(*_tmp54)),((_tmp54->hd=(void*)f((void*)x->hd,(void*)y->hd),((_tmp54->tl=0,_tmp54))))));}
# 144
prev=result;
# 146
x=x->tl;
# 147
y=y->tl;
# 149
while(x != 0  && y != 0){
# 150
struct Cyc_List_List*_tmp55;struct Cyc_List_List*temp=(_tmp55=_region_malloc(r3,sizeof(*_tmp55)),((_tmp55->hd=(void*)f((void*)x->hd,(void*)y->hd),((_tmp55->tl=0,_tmp55)))));
# 151
prev->tl=(struct Cyc_List_List*)temp;
# 152
prev=temp;
# 153
x=x->tl;
# 154
y=y->tl;}
# 156
if(x != 0  || y != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
# 157
return(struct Cyc_List_List*)result;}
# 160
struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){
# 161
return Cyc_List_rmap2(Cyc_Core_heap_region,f,x,y);}
# 167
void Cyc_List_app(void*(*f)(void*),struct Cyc_List_List*x){
# 168
while(x != 0){
# 169
f((void*)x->hd);
# 170
x=x->tl;}}
# 174
void Cyc_List_app_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 175
while(x != 0){
# 176
f(env,(void*)x->hd);
# 177
x=x->tl;}}
# 184
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x){
# 185
while(x != 0){
# 186
f((void*)x->hd);
# 187
x=x->tl;}}
# 191
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 192
while(x != 0){
# 193
f(env,(void*)x->hd);
# 194
x=x->tl;}}
# 200
void Cyc_List_app2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){
# 201
while(x != 0  && y != 0){
# 202
f((void*)x->hd,(void*)y->hd);
# 203
x=x->tl;
# 204
y=y->tl;}
# 206
if(x != 0  || y != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);}
# 209
void Cyc_List_app2_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,struct Cyc_List_List*y){
# 210
while(x != 0  && y != 0){
# 211
f(env,(void*)x->hd,(void*)y->hd);
# 212
x=x->tl;
# 213
y=y->tl;}
# 215
if(x != 0  || y != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);}
# 220
void Cyc_List_iter2(void(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){
# 221
while(x != 0  && y != 0){
# 222
f((void*)x->hd,(void*)y->hd);
# 223
x=x->tl;
# 224
y=y->tl;}
# 226
if(x != 0  || y != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);}
# 228
void Cyc_List_iter2_c(void(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,struct Cyc_List_List*y){
# 229
while(x != 0  && y != 0){
# 230
f(env,(void*)x->hd,(void*)y->hd);
# 231
x=x->tl;
# 232
y=y->tl;}
# 234
if(x != 0  || y != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);}
# 240
void*Cyc_List_fold_left(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x){
# 241
while(x != 0){
# 242
accum=f(accum,(void*)x->hd);
# 243
x=x->tl;}
# 245
return accum;}
# 248
void*Cyc_List_fold_left_c(void*(*f)(void*,void*,void*),void*env,void*accum,struct Cyc_List_List*x){
# 249
while(x != 0){
# 250
accum=f(env,accum,(void*)x->hd);
# 251
x=x->tl;}
# 253
return accum;}
# 259
void*Cyc_List_fold_right(void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum){
# 260
if(x == 0)return accum;else{
# 261
return f((void*)x->hd,Cyc_List_fold_right(f,x->tl,accum));}}
# 263
void*Cyc_List_fold_right_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,void*accum){
# 264
if(x == 0)return accum;else{
# 265
return f(env,(void*)x->hd,Cyc_List_fold_right_c(f,env,x->tl,accum));}}
# 270
struct Cyc_List_List*Cyc_List_rrevappend(struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y){
# 272
while(x != 0){
# 273
{struct Cyc_List_List*_tmp56;y=((_tmp56=_region_malloc(r2,sizeof(*_tmp56)),((_tmp56->hd=(void*)((void*)x->hd),((_tmp56->tl=y,_tmp56))))));}
# 274
x=x->tl;}
# 276
return y;}
# 279
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y){
# 280
return Cyc_List_rrevappend(Cyc_Core_heap_region,x,y);}
# 284
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*r2,struct Cyc_List_List*x){
# 286
if(x == 0)
# 287
return 0;
# 288
return Cyc_List_rrevappend(r2,x,0);}
# 291
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x){
# 292
return Cyc_List_rrev(Cyc_Core_heap_region,x);}
# 296
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x){
# 297
if(x == 0)return x;else{
# 299
struct Cyc_List_List*first=x;
# 300
struct Cyc_List_List*second=x->tl;
# 301
x->tl=0;
# 302
while(second != 0){
# 303
struct Cyc_List_List*temp=second->tl;
# 304
second->tl=first;
# 305
first=second;
# 306
second=temp;}
# 308
return first;}}
# 313
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y){
# 314
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 316
if(x == 0)return y;
# 317
if(y == 0)return Cyc_List_rcopy(r2,x);
# 318
{struct Cyc_List_List*_tmp57;result=((_tmp57=_region_malloc(r2,sizeof(*_tmp57)),((_tmp57->hd=(void*)((void*)x->hd),((_tmp57->tl=0,_tmp57))))));}
# 319
prev=result;
# 320
for(x=x->tl;x != 0;x=x->tl){
# 321
struct Cyc_List_List*_tmp58;struct Cyc_List_List*temp=(_tmp58=_region_malloc(r2,sizeof(*_tmp58)),((_tmp58->hd=(void*)((void*)x->hd),((_tmp58->tl=0,_tmp58)))));
# 322
prev->tl=(struct Cyc_List_List*)temp;
# 323
prev=temp;}
# 325
prev->tl=y;
# 326
return(struct Cyc_List_List*)result;}
# 329
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y){
# 330
return Cyc_List_rappend(Cyc_Core_heap_region,x,y);}
# 336
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y){
# 337
struct Cyc_List_List*z;
# 339
if(x == 0)return y;
# 340
if(y == 0)return x;
# 341
for(z=x;z->tl != 0;z=z->tl){
# 343
;}
# 344
z->tl=y;
# 345
return x;}
# 349
struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*r3,struct Cyc_List_List*x){
# 350
return((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*),struct _RegionHandle*env,struct Cyc_List_List*x,struct Cyc_List_List*accum))Cyc_List_fold_right_c)(Cyc_List_rappend,r3,x,0);}
# 353
struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*x){
# 354
return Cyc_List_rflatten(Cyc_Core_heap_region,x);}
# 358
struct Cyc_List_List*Cyc_List_imp_merge(int(*less_eq)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b){
# 359
struct Cyc_List_List*c;struct Cyc_List_List*d;
# 361
if(a == 0)return b;
# 362
if(b == 0)return a;
# 368
if(less_eq((void*)a->hd,(void*)b->hd)<= 0){
# 369
c=a;
# 370
a=a->tl;}else{
# 372
c=b;
# 373
b=b->tl;}
# 375
d=c;
# 377
while(a != 0  && b != 0){
# 379
if(less_eq((void*)a->hd,(void*)b->hd)<= 0){
# 380
c->tl=a;
# 381
c=a;
# 382
a=a->tl;}else{
# 384
c->tl=b;
# 385
c=b;
# 386
b=b->tl;}}
# 390
if(a == 0)
# 391
c->tl=b;else{
# 393
c->tl=a;}
# 394
return d;}
# 398
struct Cyc_List_List*Cyc_List_rimp_merge_sort(int(*less_eq)(void*,void*),struct Cyc_List_List*x){
# 399
if(x == 0)return x;{
# 400
struct Cyc_List_List*temp=x->tl;
# 401
if(temp == 0)return x;{
# 404
struct Cyc_List_List*a=(struct Cyc_List_List*)x;struct Cyc_List_List*aptr=a;
# 405
struct Cyc_List_List*b=(struct Cyc_List_List*)temp;struct Cyc_List_List*bptr=b;
# 406
x=b->tl;
# 407
while(x != 0){
# 408
aptr->tl=x;
# 409
aptr=(struct Cyc_List_List*)x;
# 410
x=x->tl;
# 411
if(x != 0){
# 412
bptr->tl=x;
# 413
bptr=(struct Cyc_List_List*)x;
# 414
x=x->tl;}}
# 417
aptr->tl=0;
# 418
bptr->tl=0;
# 419
return Cyc_List_imp_merge(less_eq,
# 420
Cyc_List_rimp_merge_sort(less_eq,(struct Cyc_List_List*)a),Cyc_List_rimp_merge_sort(less_eq,(struct Cyc_List_List*)b));};};}
# 427
struct Cyc_List_List*Cyc_List_rmerge_sort(struct _RegionHandle*r2,int(*less_eq)(void*,void*),struct Cyc_List_List*x){
# 428
return Cyc_List_rimp_merge_sort(less_eq,Cyc_List_rcopy(r2,x));}
# 432
struct Cyc_List_List*Cyc_List_rmerge(struct _RegionHandle*r3,int(*less_eq)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b){
# 436
struct Cyc_List_List*c;struct Cyc_List_List*d;
# 439
if(a == 0)return Cyc_List_rcopy(r3,b);
# 440
if(b == 0)return Cyc_List_rcopy(r3,a);
# 446
if(less_eq((void*)a->hd,(void*)b->hd)<= 0){
# 447
{struct Cyc_List_List*_tmp59;c=((_tmp59=_region_malloc(r3,sizeof(*_tmp59)),((_tmp59->hd=(void*)((void*)a->hd),((_tmp59->tl=0,_tmp59))))));}
# 448
a=a->tl;}else{
# 450
{struct Cyc_List_List*_tmp5A;c=((_tmp5A=_region_malloc(r3,sizeof(*_tmp5A)),((_tmp5A->hd=(void*)((void*)b->hd),((_tmp5A->tl=0,_tmp5A))))));}
# 451
b=b->tl;}
# 453
d=c;
# 455
while(a != 0  && b != 0){
# 457
struct Cyc_List_List*temp;
# 458
if(less_eq((void*)a->hd,(void*)b->hd)<= 0){
# 459
{struct Cyc_List_List*_tmp5B;temp=((_tmp5B=_region_malloc(r3,sizeof(*_tmp5B)),((_tmp5B->hd=(void*)((void*)a->hd),((_tmp5B->tl=0,_tmp5B))))));}
# 460
c->tl=(struct Cyc_List_List*)temp;
# 461
c=temp;
# 462
a=a->tl;}else{
# 464
{struct Cyc_List_List*_tmp5C;temp=((_tmp5C=_region_malloc(r3,sizeof(*_tmp5C)),((_tmp5C->hd=(void*)((void*)b->hd),((_tmp5C->tl=0,_tmp5C))))));}
# 465
c->tl=(struct Cyc_List_List*)temp;
# 466
c=temp;
# 467
b=b->tl;}}
# 471
if(a == 0)
# 472
c->tl=Cyc_List_rcopy(r3,b);else{
# 474
c->tl=Cyc_List_rcopy(r3,a);}
# 475
return(struct Cyc_List_List*)d;}
# 478
struct Cyc_List_List*Cyc_List_merge_sort(int(*less_eq)(void*,void*),struct Cyc_List_List*x){
# 479
return Cyc_List_rmerge_sort(Cyc_Core_heap_region,less_eq,x);}
# 482
struct Cyc_List_List*Cyc_List_merge(int(*less_eq)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b){
# 483
return Cyc_List_rmerge(Cyc_Core_heap_region,less_eq,a,b);}char Cyc_List_Nth[4]="Nth";
# 488
struct Cyc_List_Nth_exn_struct Cyc_List_Nth_val={Cyc_List_Nth};
# 493
void*Cyc_List_nth(struct Cyc_List_List*x,int i){
# 495
while(i > 0  && x != 0){
# 496
-- i;
# 497
x=x->tl;}
# 499
if(i < 0  || x == 0)(int)_throw((void*)& Cyc_List_Nth_val);
# 500
return(void*)x->hd;}
# 505
struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i){
# 506
if(i < 0)(int)_throw((void*)& Cyc_List_Nth_val);
# 507
while(i != 0){
# 508
if(x == 0)(int)_throw((void*)& Cyc_List_Nth_val);
# 509
x=x->tl;
# 510
-- i;}
# 512
return x;}
# 517
int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x){
# 518
while(x != 0  && pred((void*)x->hd)){x=x->tl;}
# 519
return x == 0;}
# 521
int Cyc_List_forall_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x){
# 522
while(x != 0  && pred(env,(void*)x->hd)){x=x->tl;}
# 523
return x == 0;}
# 528
int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x){
# 529
while(x != 0  && !pred((void*)x->hd)){x=x->tl;}
# 530
return x != 0;}
# 532
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x){
# 533
while(x != 0  && !pred(env,(void*)x->hd)){x=x->tl;}
# 534
return x != 0;}struct _tuple2{void*f1;void*f2;};
# 539
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r3,struct _RegionHandle*r4,struct Cyc_List_List*x,struct Cyc_List_List*y){
# 541
struct Cyc_List_List*result;struct Cyc_List_List*prev;struct Cyc_List_List*temp;
# 543
if(x == 0  && y == 0)return 0;
# 544
if(x == 0  || y == 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
# 546
{struct _tuple2*_tmp5F;struct Cyc_List_List*_tmp5E;result=((_tmp5E=_region_malloc(r3,sizeof(*_tmp5E)),((_tmp5E->hd=((_tmp5F=_region_malloc(r4,sizeof(*_tmp5F)),((_tmp5F->f1=(void*)x->hd,((_tmp5F->f2=(void*)y->hd,_tmp5F)))))),((_tmp5E->tl=0,_tmp5E))))));}
# 547
prev=result;
# 549
x=x->tl;
# 550
y=y->tl;
# 552
while(x != 0  && y != 0){
# 553
{struct _tuple2*_tmp62;struct Cyc_List_List*_tmp61;temp=((_tmp61=_region_malloc(r3,sizeof(*_tmp61)),((_tmp61->hd=((_tmp62=_region_malloc(r4,sizeof(*_tmp62)),((_tmp62->f1=(void*)x->hd,((_tmp62->f2=(void*)y->hd,_tmp62)))))),((_tmp61->tl=0,_tmp61))))));}
# 554
prev->tl=(struct Cyc_List_List*)temp;
# 555
prev=temp;
# 556
x=x->tl;
# 557
y=y->tl;}
# 559
if(x != 0  || y != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
# 560
return(struct Cyc_List_List*)result;}
# 563
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y){
# 564
return Cyc_List_rzip(Cyc_Core_heap_region,Cyc_Core_heap_region,x,y);}struct _tuple3{void*f1;void*f2;void*f3;};
# 567
struct Cyc_List_List*Cyc_List_rzip3(struct _RegionHandle*r3,struct _RegionHandle*r4,struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z){
# 569
struct Cyc_List_List*result;struct Cyc_List_List*prev;struct Cyc_List_List*temp;
# 571
if((x == 0  && y == 0) && z == 0)return 0;
# 572
if((x == 0  || y == 0) || z == 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
# 574
{struct _tuple3*_tmp65;struct Cyc_List_List*_tmp64;result=((_tmp64=_region_malloc(r3,sizeof(*_tmp64)),((_tmp64->hd=((_tmp65=_region_malloc(r4,sizeof(*_tmp65)),((_tmp65->f1=(void*)x->hd,((_tmp65->f2=(void*)y->hd,((_tmp65->f3=(void*)z->hd,_tmp65)))))))),((_tmp64->tl=0,_tmp64))))));}
# 575
prev=result;
# 577
x=x->tl;
# 578
y=y->tl;
# 579
z=z->tl;
# 581
while((x != 0  && y != 0) && z != 0){
# 582
{struct _tuple3*_tmp68;struct Cyc_List_List*_tmp67;temp=((_tmp67=_region_malloc(r3,sizeof(*_tmp67)),((_tmp67->hd=((_tmp68=_region_malloc(r4,sizeof(*_tmp68)),((_tmp68->f1=(void*)x->hd,((_tmp68->f2=(void*)y->hd,((_tmp68->f3=(void*)z->hd,_tmp68)))))))),((_tmp67->tl=0,_tmp67))))));}
# 583
prev->tl=(struct Cyc_List_List*)temp;
# 584
prev=temp;
# 585
x=x->tl;
# 586
y=y->tl;
# 587
z=z->tl;}
# 589
if((x != 0  || y != 0) || z != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
# 590
return(struct Cyc_List_List*)result;}
# 593
struct Cyc_List_List*Cyc_List_zip3(struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z){
# 594
return Cyc_List_rzip3(Cyc_Core_heap_region,Cyc_Core_heap_region,x,y,z);}
# 599
struct _tuple0 Cyc_List_rsplit(struct _RegionHandle*r3,struct _RegionHandle*r4,struct Cyc_List_List*x){
# 601
struct Cyc_List_List*result1;struct Cyc_List_List*prev1;struct Cyc_List_List*temp1;
# 602
struct Cyc_List_List*result2;struct Cyc_List_List*prev2;struct Cyc_List_List*temp2;
# 604
if(x == 0){struct _tuple0 _tmp69;return(_tmp69.f1=0,((_tmp69.f2=0,_tmp69)));}
# 606
{struct Cyc_List_List*_tmp6A;prev1=(result1=((_tmp6A=_region_malloc(r3,sizeof(*_tmp6A)),((_tmp6A->hd=(void*)(((struct _tuple2*)x->hd)[0]).f1,((_tmp6A->tl=0,_tmp6A)))))));}
# 607
{struct Cyc_List_List*_tmp6B;prev2=(result2=((_tmp6B=_region_malloc(r4,sizeof(*_tmp6B)),((_tmp6B->hd=(void*)(((struct _tuple2*)x->hd)[0]).f2,((_tmp6B->tl=0,_tmp6B)))))));}
# 609
for(x=x->tl;x != 0;x=x->tl){
# 610
{struct Cyc_List_List*_tmp6C;temp1=((_tmp6C=_region_malloc(r3,sizeof(*_tmp6C)),((_tmp6C->hd=(void*)(((struct _tuple2*)x->hd)[0]).f1,((_tmp6C->tl=0,_tmp6C))))));}
# 611
{struct Cyc_List_List*_tmp6D;temp2=((_tmp6D=_region_malloc(r4,sizeof(*_tmp6D)),((_tmp6D->hd=(void*)(((struct _tuple2*)x->hd)[0]).f2,((_tmp6D->tl=0,_tmp6D))))));}
# 612
prev1->tl=(struct Cyc_List_List*)temp1;
# 613
prev2->tl=(struct Cyc_List_List*)temp2;
# 614
prev1=temp1;
# 615
prev2=temp2;}{
# 617
struct _tuple0 _tmp6E;return(_tmp6E.f1=(struct Cyc_List_List*)result1,((_tmp6E.f2=(struct Cyc_List_List*)result2,_tmp6E)));};}
# 620
struct _tuple0 Cyc_List_split(struct Cyc_List_List*x){
# 621
return Cyc_List_rsplit(Cyc_Core_heap_region,Cyc_Core_heap_region,x);}
# 626
struct _tuple1 Cyc_List_rsplit3(struct _RegionHandle*r3,struct _RegionHandle*r4,struct _RegionHandle*r5,struct Cyc_List_List*x){
# 629
struct Cyc_List_List*result1;struct Cyc_List_List*prev1;struct Cyc_List_List*temp1;
# 630
struct Cyc_List_List*result2;struct Cyc_List_List*prev2;struct Cyc_List_List*temp2;
# 631
struct Cyc_List_List*result3;struct Cyc_List_List*prev3;struct Cyc_List_List*temp3;
# 633
if(x == 0){struct _tuple1 _tmp6F;return(_tmp6F.f1=0,((_tmp6F.f2=0,((_tmp6F.f3=0,_tmp6F)))));}
# 635
{struct Cyc_List_List*_tmp70;prev1=(result1=((_tmp70=_region_malloc(r3,sizeof(*_tmp70)),((_tmp70->hd=(void*)(((struct _tuple3*)x->hd)[0]).f1,((_tmp70->tl=0,_tmp70)))))));}
# 636
{struct Cyc_List_List*_tmp71;prev2=(result2=((_tmp71=_region_malloc(r4,sizeof(*_tmp71)),((_tmp71->hd=(void*)(((struct _tuple3*)x->hd)[0]).f2,((_tmp71->tl=0,_tmp71)))))));}
# 637
{struct Cyc_List_List*_tmp72;prev3=(result3=((_tmp72=_region_malloc(r5,sizeof(*_tmp72)),((_tmp72->hd=(void*)(((struct _tuple3*)x->hd)[0]).f3,((_tmp72->tl=0,_tmp72)))))));}
# 639
for(x=x->tl;x != 0;x=x->tl){
# 640
{struct Cyc_List_List*_tmp73;temp1=((_tmp73=_region_malloc(r3,sizeof(*_tmp73)),((_tmp73->hd=(void*)(((struct _tuple3*)x->hd)[0]).f1,((_tmp73->tl=0,_tmp73))))));}
# 641
{struct Cyc_List_List*_tmp74;temp2=((_tmp74=_region_malloc(r4,sizeof(*_tmp74)),((_tmp74->hd=(void*)(((struct _tuple3*)x->hd)[0]).f2,((_tmp74->tl=0,_tmp74))))));}
# 642
{struct Cyc_List_List*_tmp75;temp3=((_tmp75=_region_malloc(r5,sizeof(*_tmp75)),((_tmp75->hd=(void*)(((struct _tuple3*)x->hd)[0]).f3,((_tmp75->tl=0,_tmp75))))));}
# 643
prev1->tl=(struct Cyc_List_List*)temp1;
# 644
prev2->tl=(struct Cyc_List_List*)temp2;
# 645
prev3->tl=(struct Cyc_List_List*)temp3;
# 646
prev1=temp1;
# 647
prev2=temp2;
# 648
prev3=temp3;}{
# 650
struct _tuple1 _tmp76;return(_tmp76.f1=(struct Cyc_List_List*)result1,((_tmp76.f2=(struct Cyc_List_List*)result2,((_tmp76.f3=(struct Cyc_List_List*)result3,_tmp76)))));};}
# 653
struct _tuple1 Cyc_List_split3(struct Cyc_List_List*x){
# 654
return Cyc_List_rsplit3(Cyc_Core_heap_region,Cyc_Core_heap_region,Cyc_Core_heap_region,x);}
# 660
int Cyc_List_memq(struct Cyc_List_List*l,void*x){
# 661
while(l != 0){
# 662
if((void*)l->hd == x)return 1;
# 663
l=l->tl;}
# 665
return 0;}
# 668
int Cyc_List_mem(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){
# 669
while(l != 0){
# 670
if(cmp((void*)l->hd,x)== 0)return 1;
# 671
l=l->tl;}
# 673
return 0;}
# 682
void*Cyc_List_assoc(struct Cyc_List_List*l,void*x){
# 683
while(l != 0){
# 684
if((((struct _tuple2*)l->hd)[0]).f1 == x)return(((struct _tuple2*)l->hd)[0]).f2;
# 685
l=l->tl;}
# 687
(int)_throw((void*)& Cyc_Core_Not_found_val);}
# 690
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){
# 691
while(l != 0){
# 692
if(cmp((*((struct _tuple2*)l->hd)).f1,x)== 0)return(*((struct _tuple2*)l->hd)).f2;
# 693
l=l->tl;}
# 695
(int)_throw((void*)& Cyc_Core_Not_found_val);}
# 702
struct Cyc_List_List*Cyc_List_delete_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){
# 703
struct Cyc_List_List*prev=0;
# 704
struct Cyc_List_List*_tmp2F=l;
# 705
while(_tmp2F != 0){
# 706
if(cmp((void*)_tmp2F->hd,x)== 0){
# 707
if(prev == 0)
# 708
return((struct Cyc_List_List*)_check_null(l))->tl;else{
# 710
prev->tl=_tmp2F->tl;
# 711
return l;}}
# 714
prev=_tmp2F;
# 715
_tmp2F=_tmp2F->tl;}
# 717
(int)_throw((void*)& Cyc_Core_Not_found_val);}
# 720
static int Cyc_List_ptrequal(void*x,void*y){
# 721
return x == y;}
# 727
struct Cyc_List_List*Cyc_List_delete(struct Cyc_List_List*l,void*x){
# 728
return Cyc_List_delete_cmp(Cyc_List_ptrequal,l,x);}
# 733
int Cyc_List_mem_assoc(struct Cyc_List_List*l,void*x){
# 734
while(l != 0){
# 735
if((((struct _tuple2*)l->hd)[0]).f1 == x)return 1;
# 736
l=l->tl;}
# 738
return 0;}
# 743
int Cyc_List_mem_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){
# 744
while(l != 0){
# 745
if(cmp((((struct _tuple2*)l->hd)[0]).f1,x)== 0)return 1;
# 746
l=l->tl;}
# 748
return 0;}
# 754
struct Cyc_Core_Opt*Cyc_List_check_unique(int(*cmp)(void*,void*),struct Cyc_List_List*x){
# 755
while(x != 0){
# 756
struct Cyc_List_List*_tmp30=x->tl;
# 757
if(_tmp30 != 0){
# 758
if(cmp((void*)x->hd,(void*)_tmp30->hd)== 0){struct Cyc_Core_Opt*_tmp77;return(_tmp77=_cycalloc(sizeof(*_tmp77)),((_tmp77->v=(void*)((void*)x->hd),_tmp77)));}}
# 759
x=_tmp30;}
# 761
return 0;}struct _dyneither_ptr Cyc_List_rto_array(struct _RegionHandle*r2,struct Cyc_List_List*x);static void _tmp7E(struct Cyc_List_List**x,unsigned int*_tmp7D,unsigned int*_tmp7C,void***_tmp7A){for(*_tmp7D=0;*_tmp7D < *_tmp7C;(*_tmp7D)++){
# 770
void*_tmp78;(*_tmp7A)[*_tmp7D]=((_tmp78=(void*)((struct Cyc_List_List*)_check_null(*x))->hd,((*x=(*x)->tl,_tmp78))));}}
# 765
struct _dyneither_ptr Cyc_List_rto_array(struct _RegionHandle*r2,struct Cyc_List_List*x){
# 766
int s;
# 767
struct _dyneither_ptr arr;
# 769
s=Cyc_List_length(x);
# 770
{unsigned int _tmp7D;unsigned int _tmp7C;struct _dyneither_ptr _tmp7B;void**_tmp7A;unsigned int _tmp79;arr=((_tmp79=(unsigned int)s,((_tmp7A=(void**)_region_malloc(r2,_check_times(sizeof(void*),_tmp79)),((_tmp7B=_tag_dyneither(_tmp7A,sizeof(void*),_tmp79),((((_tmp7C=_tmp79,_tmp7E(& x,& _tmp7D,& _tmp7C,& _tmp7A))),_tmp7B))))))));}
# 773
return arr;}
# 776
struct _dyneither_ptr Cyc_List_to_array(struct Cyc_List_List*x){
# 777
return Cyc_List_rto_array(Cyc_Core_heap_region,x);}
# 781
struct Cyc_List_List*Cyc_List_rfrom_array(struct _RegionHandle*r2,struct _dyneither_ptr arr){
# 782
struct Cyc_List_List*ans=0;
# 783
{int i=(int)(_get_dyneither_size(arr,sizeof(void*))- 1);for(0;i >= 0;-- i){
# 784
struct Cyc_List_List*_tmp7F;ans=((_tmp7F=_region_malloc(r2,sizeof(*_tmp7F)),((_tmp7F->hd=(void*)*((void**)_check_dyneither_subscript(arr,sizeof(void*),i)),((_tmp7F->tl=ans,_tmp7F))))));}}
# 785
return ans;}
# 788
struct Cyc_List_List*Cyc_List_from_array(struct _dyneither_ptr arr){
# 789
return Cyc_List_rfrom_array(Cyc_Core_heap_region,arr);}
# 792
struct Cyc_List_List*Cyc_List_rtabulate(struct _RegionHandle*r,int n,void*(*f)(int)){
# 793
struct Cyc_List_List*res=0;
# 794
{int i=0;for(0;i < n;++ i){
# 795
struct Cyc_List_List*_tmp80;res=((_tmp80=_region_malloc(r,sizeof(*_tmp80)),((_tmp80->hd=(void*)f(i),((_tmp80->tl=res,_tmp80))))));}}
# 797
return Cyc_List_imp_rev(res);}
# 800
struct Cyc_List_List*Cyc_List_tabulate(int n,void*(*f)(int)){
# 801
return Cyc_List_rtabulate(Cyc_Core_heap_region,n,f);}
# 804
struct Cyc_List_List*Cyc_List_rtabulate_c(struct _RegionHandle*r,int n,void*(*f)(void*,int),void*env){
# 805
struct Cyc_List_List*res=0;
# 806
{int i=0;for(0;i < n;++ i){
# 807
struct Cyc_List_List*_tmp81;res=((_tmp81=_region_malloc(r,sizeof(*_tmp81)),((_tmp81->hd=(void*)f(env,i),((_tmp81->tl=res,_tmp81))))));}}
# 809
return Cyc_List_imp_rev(res);}
# 812
struct Cyc_List_List*Cyc_List_tabulate_c(int n,void*(*f)(void*,int),void*env){
# 813
return Cyc_List_rtabulate_c(Cyc_Core_heap_region,n,f,env);}
# 816
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2){
# 817
for(0;l1 != 0  && l2 != 0;(l1=l1->tl,l2=l2->tl)){
# 819
if((unsigned int)l1 == (unsigned int)l2)
# 820
return 0;{
# 821
int _tmp3A=cmp((void*)l1->hd,(void*)l2->hd);
# 822
if(_tmp3A != 0)
# 823
return _tmp3A;};}
# 825
if(l1 != 0)
# 826
return 1;
# 827
if(l2 != 0)
# 828
return - 1;
# 829
return 0;}
# 833
int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2){
# 834
for(0;l1 != 0  && l2 != 0;(l1=l1->tl,l2=l2->tl)){
# 837
if((unsigned int)l1 == (unsigned int)l2)
# 838
return 1;{
# 839
int _tmp3B=cmp((void*)l1->hd,(void*)l2->hd);
# 840
if(_tmp3B != 0)
# 841
return 0;};}
# 843
return l1 == 0;}
# 846
struct Cyc_List_List*Cyc_List_rfilter_c(struct _RegionHandle*r,int(*f)(void*,void*),void*env,struct Cyc_List_List*l){
# 847
if(l == 0)
# 848
return 0;{
# 850
struct Cyc_List_List*_tmp82;struct Cyc_List_List*result=(_tmp82=_region_malloc(r,sizeof(*_tmp82)),((_tmp82->hd=(void*)((void*)l->hd),((_tmp82->tl=0,_tmp82)))));
# 851
struct Cyc_List_List*end=result;
# 852
struct Cyc_List_List*temp;
# 853
for(0;l != 0;l=l->tl){
# 854
if(f(env,(void*)l->hd)){
# 855
{struct Cyc_List_List*_tmp83;temp=((_tmp83=_region_malloc(r,sizeof(*_tmp83)),((_tmp83->hd=(void*)((void*)l->hd),((_tmp83->tl=0,_tmp83))))));}
# 856
end->tl=(struct Cyc_List_List*)temp;
# 857
end=temp;}}
# 860
return result->tl;};}
# 863
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*l){
# 864
return Cyc_List_rfilter_c(Cyc_Core_heap_region,f,env,l);}
# 867
struct Cyc_List_List*Cyc_List_rfilter(struct _RegionHandle*r,int(*f)(void*),struct Cyc_List_List*l){
# 868
if(l == 0)
# 869
return 0;{
# 871
struct Cyc_List_List*_tmp84;struct Cyc_List_List*result=(_tmp84=_region_malloc(r,sizeof(*_tmp84)),((_tmp84->hd=(void*)((void*)l->hd),((_tmp84->tl=0,_tmp84)))));
# 872
struct Cyc_List_List*end=result;
# 873
struct Cyc_List_List*temp;
# 874
for(0;l != 0;l=l->tl){
# 875
if(f((void*)l->hd)){
# 876
{struct Cyc_List_List*_tmp85;temp=((_tmp85=_region_malloc(r,sizeof(*_tmp85)),((_tmp85->hd=(void*)((void*)l->hd),((_tmp85->tl=0,_tmp85))))));}
# 877
end->tl=(struct Cyc_List_List*)temp;
# 878
end=temp;}}
# 881
return result->tl;};}
# 884
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*l){
# 885
return Cyc_List_rfilter(Cyc_Core_heap_region,f,l);}
