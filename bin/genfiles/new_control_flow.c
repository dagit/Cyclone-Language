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
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 123 "core.h"
void*Cyc_Core_snd(struct _tuple0*);
# 138
int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 72
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 123
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 230
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 259
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 302
int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 113
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
# 126 "dict.h"
int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**ans);
# 149
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);struct Cyc_Hashtable_Table;
# 35 "hashtable.h"
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
# 47
struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 56
void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
# 100
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
# 101
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 922 "absyn.h"
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 925
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
# 1160
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1164
int Cyc_Absyn_is_nontagged_union_type(void*);struct Cyc_RgnOrder_RgnPO;
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
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 65
void*Cyc_Tcutil_compress(void*t);
# 95
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 155
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 164
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 228 "tcutil.h"
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 260
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 270
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 323
int Cyc_Tcutil_is_noreturn(void*);
# 338
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 49 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 60
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 62
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 67
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple12{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple12 val;};struct _union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct _union_RelnOp_LessEqConst LessEqConst;};
# 79
typedef union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_reln_op_t;
# 82
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*,void*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(unsigned int);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqConst(unsigned int);struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp rop;};
# 94
typedef struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_reln_t;
# 95
typedef struct Cyc_List_List*Cyc_CfFlowInfo_relns_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 102
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 108
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
# 109
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
# 110
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();
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
# 137
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,unsigned int loc);
# 142
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;
# 143
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple13{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple13 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 158 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
# 159
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
# 160
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 177
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
# 178
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);
# 180
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
# 181
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f);
# 183
int Cyc_CfFlowInfo_root_cmp(void*,void*);
# 186
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
# 187
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
# 188
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
# 189
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
# 190
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);
# 192
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
# 193
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
# 195
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 217 "cf_flowinfo.h"
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
# 218
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 220
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 221
struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
# 222
int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);
# 227
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);
# 231
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);
# 236
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple14{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
# 237
struct _tuple14 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple14 pr1,struct _tuple14 pr2);
# 242
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 247
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 256
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);
# 35
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);
# 39
extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 68 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
# 61 "new_control_flow.cyc"
typedef struct Cyc_Dict_Dict Cyc_NewControlFlow_dict_t;
# 62
typedef unsigned int Cyc_NewControlFlow_seg_t;
# 65
int Cyc_NewControlFlow_warn_lose_unique=0;struct Cyc_NewControlFlow_CFStmtAnnot{struct Cyc_Absyn_Stmt*encloser;int visited;};
# 73
typedef struct Cyc_NewControlFlow_CFStmtAnnot Cyc_NewControlFlow_cf_stmt_annot_t;static char Cyc_NewControlFlow_CFAnnot[8]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};
# 78
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){
# 79
struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp7C2;struct Cyc_NewControlFlow_CFStmtAnnot _tmp7C1;struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp7C0;enclosee->annot=(void*)((_tmp7C0=_cycalloc(sizeof(*_tmp7C0)),((_tmp7C0[0]=((_tmp7C2.tag=Cyc_NewControlFlow_CFAnnot,((_tmp7C2.f1=((_tmp7C1.encloser=encloser,((_tmp7C1.visited=0,_tmp7C1)))),_tmp7C2)))),_tmp7C0))));}struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;struct Cyc_List_List*param_roots;struct Cyc_Hashtable_Table*flow_table;};
# 104 "new_control_flow.cyc"
typedef struct Cyc_NewControlFlow_AnalEnv*Cyc_NewControlFlow_analenv_t;
# 112
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*);
# 113
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple15{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal f2;};
# 114
static struct _tuple15 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int expand_unique,struct Cyc_Absyn_Exp*);
# 115
static struct _tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple16{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
# 116
static struct _tuple16 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
# 117
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t);
# 121
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){
# 122
void*_tmp3=s->annot;struct Cyc_NewControlFlow_CFStmtAnnot _tmp5;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp6;_LL1: {struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp4=(struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp3;if(_tmp4->tag != Cyc_NewControlFlow_CFAnnot)goto _LL3;else{_tmp5=_tmp4->f1;_tmp6=(struct Cyc_NewControlFlow_CFStmtAnnot*)& _tmp4->f1;}}_LL2:
# 123
 return _tmp6;_LL3:;_LL4: {
# 124
struct Cyc_Core_Impossible_exn_struct _tmp7C8;const char*_tmp7C7;struct Cyc_Core_Impossible_exn_struct*_tmp7C6;(int)_throw((void*)((_tmp7C6=_cycalloc(sizeof(*_tmp7C6)),((_tmp7C6[0]=((_tmp7C8.tag=Cyc_Core_Impossible,((_tmp7C8.f1=((_tmp7C7="ControlFlow -- wrong stmt annotation",_tag_dyneither(_tmp7C7,sizeof(char),37))),_tmp7C8)))),_tmp7C6)))));}_LL0:;}
# 128
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
# 129
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
# 130
if(sflow == 0){
# 131
union Cyc_CfFlowInfo_FlowInfo*_tmp7C9;union Cyc_CfFlowInfo_FlowInfo*res=(_tmp7C9=_region_malloc(env->r,sizeof(*_tmp7C9)),((_tmp7C9[0]=Cyc_CfFlowInfo_BottomFL(),_tmp7C9)));
# 132
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);
# 133
return res;}
# 135
return*sflow;}struct _tuple17{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 138
static struct _tuple17 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
# 139
struct Cyc_NewControlFlow_CFStmtAnnot*_tmpB=Cyc_NewControlFlow_get_stmt_annot(s);
# 140
union Cyc_CfFlowInfo_FlowInfo*_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 142
*_tmpC=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpC);
# 145
_tmpB->visited=env->iteration_num;{
# 146
struct _tuple17 _tmp7CA;return(_tmp7CA.f1=_tmpB,((_tmp7CA.f2=_tmpC,_tmp7CA)));};}
# 157 "new_control_flow.cyc"
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
# 158
if(env->in_try)
# 165
env->tryflow=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,new_flow,env->tryflow);}struct _tuple18{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned int f2;struct Cyc_Dict_Dict f3;};
# 172
static void Cyc_NewControlFlow_check_unique_root(struct _tuple18*ckenv,void*root,void*rval){
# 174
struct _tuple18 _tmpF;struct Cyc_NewControlFlow_AnalEnv*_tmp10;unsigned int _tmp11;struct Cyc_Dict_Dict _tmp12;struct _tuple18*_tmpE=ckenv;_tmpF=*_tmpE;_tmp10=_tmpF.f1;_tmp11=_tmpF.f2;_tmp12=_tmpF.f3;{
# 175
void*_tmp13=root;struct Cyc_Absyn_Vardecl*_tmp15;_LL6: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp14=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp13;if(_tmp14->tag != 0)goto _LL8;else{_tmp15=_tmp14->f1;}}_LL7:
# 177
 if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(_tmp12,root,& rval) && 
# 178
Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp10->fenv)->r,_tmp15->type)){
# 179
{void*_tmp16=rval;enum Cyc_CfFlowInfo_InitLevel _tmp19;_LLB: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp17=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp16;if(_tmp17->tag != 7)goto _LLD;}_LLC:
# 180
 goto _LLE;_LLD: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp18=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp16;if(_tmp18->tag != 3)goto _LLF;else{_tmp19=_tmp18->f1;if(_tmp19 != Cyc_CfFlowInfo_NoneIL)goto _LLF;}}_LLE:
# 181
 goto _LL10;_LLF: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp1A=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp16;if(_tmp1A->tag != 0)goto _LL11;}_LL10:
# 182
 goto _LLA;_LL11:;_LL12:
# 184
{const char*_tmp7CE;void*_tmp7CD[1];struct Cyc_String_pa_PrintArg_struct _tmp7CC;(_tmp7CC.tag=0,((_tmp7CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp15->name)),((_tmp7CD[0]=& _tmp7CC,Cyc_Tcutil_warn(_tmp11,((_tmp7CE="unique pointer(s) reachable from %s may become unreachable.",_tag_dyneither(_tmp7CE,sizeof(char),60))),_tag_dyneither(_tmp7CD,sizeof(void*),1)))))));}
# 185
goto _LLA;_LLA:;}
# 187
goto _LL5;}
# 189
goto _LL5;_LL8:;_LL9:
# 190
 goto _LL5;_LL5:;};}
# 197
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
# 198
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp1E=Cyc_NewControlFlow_get_stmt_annot(s);
# 199
union Cyc_CfFlowInfo_FlowInfo*_tmp1F=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 200
union Cyc_CfFlowInfo_FlowInfo _tmp20=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,flow,*_tmp1F);
# 201
if(Cyc_NewControlFlow_warn_lose_unique){
# 202
struct _tuple16 _tmp7CF;struct _tuple16 _tmp22=(_tmp7CF.f1=flow,((_tmp7CF.f2=_tmp20,_tmp7CF)));union Cyc_CfFlowInfo_FlowInfo _tmp23;struct _tuple13 _tmp24;struct Cyc_Dict_Dict _tmp25;union Cyc_CfFlowInfo_FlowInfo _tmp26;struct _tuple13 _tmp27;struct Cyc_Dict_Dict _tmp28;_LL14: _tmp23=_tmp22.f1;if((_tmp23.ReachableFL).tag != 2)goto _LL16;_tmp24=(struct _tuple13)(_tmp23.ReachableFL).val;_tmp25=_tmp24.f1;_tmp26=_tmp22.f2;if((_tmp26.ReachableFL).tag != 2)goto _LL16;_tmp27=(struct _tuple13)(_tmp26.ReachableFL).val;_tmp28=_tmp27.f1;_LL15: {
# 204
struct _tuple18 _tmp7D0;struct _tuple18 _tmp29=(_tmp7D0.f1=env,((_tmp7D0.f2=s->loc,((_tmp7D0.f3=_tmp28,_tmp7D0)))));
# 205
((void(*)(void(*f)(struct _tuple18*,void*,void*),struct _tuple18*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& _tmp29,_tmp25);
# 206
goto _LL13;}_LL16:;_LL17:
# 207
 goto _LL13;_LL13:;}
# 212
if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp20,*_tmp1F)){
# 219
*_tmp1F=_tmp20;
# 223
if(_tmp1E->visited == env->iteration_num)
# 225
env->iterate_again=1;}}
# 230
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned int loc){
# 233
union Cyc_CfFlowInfo_FlowInfo _tmp2B=inflow;int _tmp2C;struct _tuple13 _tmp2D;struct Cyc_Dict_Dict _tmp2E;struct Cyc_List_List*_tmp2F;_LL19: if((_tmp2B.BottomFL).tag != 1)goto _LL1B;_tmp2C=(int)(_tmp2B.BottomFL).val;_LL1A:
# 234
 return Cyc_CfFlowInfo_BottomFL();_LL1B: if((_tmp2B.ReachableFL).tag != 2)goto _LL18;_tmp2D=(struct _tuple13)(_tmp2B.ReachableFL).val;_tmp2E=_tmp2D.f1;_tmp2F=_tmp2D.f2;_LL1C:
# 236
 for(0;vds != 0;vds=vds->tl){
# 237
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp7D3;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp7D2;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp30=(_tmp7D2=_region_malloc(fenv->r,sizeof(*_tmp7D2)),((_tmp7D2[0]=((_tmp7D3.tag=0,((_tmp7D3.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp7D3)))),_tmp7D2)));
# 239
_tmp2E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2E,(void*)_tmp30,Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval));}
# 241
return Cyc_CfFlowInfo_ReachableFL(_tmp2E,_tmp2F);_LL18:;}
# 245
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
# 246
union Cyc_CfFlowInfo_FlowInfo _tmp33=inflow;int _tmp34;struct _tuple13 _tmp35;struct Cyc_Dict_Dict _tmp36;struct Cyc_List_List*_tmp37;_LL1E: if((_tmp33.BottomFL).tag != 1)goto _LL20;_tmp34=(int)(_tmp33.BottomFL).val;_LL1F:
# 247
 return Cyc_CfFlowInfo_BottomFL();_LL20: if((_tmp33.ReachableFL).tag != 2)goto _LL1D;_tmp35=(struct _tuple13)(_tmp33.ReachableFL).val;_tmp36=_tmp35.f1;_tmp37=_tmp35.f2;_LL21:
# 249
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp36,r)!= Cyc_CfFlowInfo_AllIL){
# 250
const char*_tmp7D6;void*_tmp7D5;(_tmp7D5=0,Cyc_Tcutil_terr(loc,((_tmp7D6="expression may not be fully initialized",_tag_dyneither(_tmp7D6,sizeof(char),40))),_tag_dyneither(_tmp7D5,sizeof(void*),0)));}{
# 251
struct Cyc_Dict_Dict _tmp3A=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp36,env->all_changed,r);
# 252
if(_tmp36.t == _tmp3A.t)return inflow;{
# 253
union Cyc_CfFlowInfo_FlowInfo _tmp3B=Cyc_CfFlowInfo_ReachableFL(_tmp3A,_tmp37);
# 254
Cyc_NewControlFlow_update_tryflow(env,_tmp3B);
# 255
return _tmp3B;};};_LL1D:;}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 259
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
# 260
struct _tuple0 _tmp7D7;struct _tuple0 _tmp3D=(_tmp7D7.f1=Cyc_Tcutil_compress(t),((_tmp7D7.f2=r,_tmp7D7)));void*_tmp3E;enum Cyc_CfFlowInfo_InitLevel _tmp40;void*_tmp41;void*_tmp43;void*_tmp45;struct Cyc_Absyn_DatatypeFieldInfo _tmp47;union Cyc_Absyn_DatatypeFieldInfoU _tmp48;struct _tuple2 _tmp49;struct Cyc_Absyn_Datatypefield*_tmp4A;void*_tmp4B;struct _dyneither_ptr _tmp4D;void*_tmp4E;struct Cyc_List_List*_tmp50;void*_tmp51;struct _dyneither_ptr _tmp53;void*_tmp54;struct Cyc_Absyn_AggrInfo _tmp56;union Cyc_Absyn_AggrInfoU _tmp57;struct Cyc_List_List*_tmp58;void*_tmp59;struct _dyneither_ptr _tmp5B;void*_tmp5C;enum Cyc_Absyn_AggrKind _tmp5E;struct Cyc_List_List*_tmp5F;void*_tmp60;struct _dyneither_ptr _tmp62;void*_tmp63;_LL23: _tmp3E=_tmp3D.f2;{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp3F=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp3E;if(_tmp3F->tag != 3)goto _LL25;else{_tmp40=_tmp3F->f1;if(_tmp40 != Cyc_CfFlowInfo_NoneIL)goto _LL25;}};_LL24:
# 261
 return;_LL25: _tmp41=_tmp3D.f2;{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp42=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp41;if(_tmp42->tag != 0)goto _LL27;};_LL26:
# 262
 return;_LL27: _tmp43=_tmp3D.f2;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp44=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp43;if(_tmp44->tag != 7)goto _LL29;};_LL28:
# 263
 return;_LL29: _tmp45=_tmp3D.f1;{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp46=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp45;if(_tmp46->tag != 4)goto _LL2B;else{_tmp47=_tmp46->f1;_tmp48=_tmp47.field_info;if((_tmp48.KnownDatatypefield).tag != 2)goto _LL2B;_tmp49=(struct _tuple2)(_tmp48.KnownDatatypefield).val;_tmp4A=_tmp49.f2;}};_tmp4B=_tmp3D.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4C=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4B;if(_tmp4C->tag != 6)goto _LL2B;else{_tmp4D=_tmp4C->f2;}};_LL2A:
# 265
 if(_tmp4A->typs == 0)
# 266
return;
# 267
_tmp50=_tmp4A->typs;_tmp53=_tmp4D;goto _LL2C;_LL2B: _tmp4E=_tmp3D.f1;{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4F=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4E;if(_tmp4F->tag != 10)goto _LL2D;else{_tmp50=_tmp4F->f1;}};_tmp51=_tmp3D.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp52=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp51;if(_tmp52->tag != 6)goto _LL2D;else{_tmp53=_tmp52->f2;}};_LL2C: {
# 269
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp50);
# 270
{int i=0;for(0;i < sz;++ i){
# 271
Cyc_NewControlFlow_check_nounique(env,loc,(*((struct _tuple19*)((struct Cyc_List_List*)_check_null(_tmp50))->hd)).f2,*((void**)_check_dyneither_subscript(_tmp53,sizeof(void*),i)));}}
# 273
return;}_LL2D: _tmp54=_tmp3D.f1;{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp55=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp54;if(_tmp55->tag != 11)goto _LL2F;else{_tmp56=_tmp55->f1;_tmp57=_tmp56.aggr_info;_tmp58=_tmp56.targs;}};_tmp59=_tmp3D.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp5A=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp59;if(_tmp5A->tag != 6)goto _LL2F;else{_tmp5B=_tmp5A->f2;}};_LL2E: {
# 276
struct Cyc_Absyn_Aggrdecl*_tmp65=Cyc_Absyn_get_known_aggrdecl(_tmp57);
# 277
if(_tmp65->impl == 0)return;{
# 278
struct Cyc_List_List*_tmp66=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp65->impl))->fields;
# 279
struct _RegionHandle _tmp67=_new_region("temp");struct _RegionHandle*temp=& _tmp67;_push_region(temp);
# 280
{struct Cyc_List_List*_tmp68=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp65->tvs,_tmp58);
# 281
{int i=0;for(0;i < _get_dyneither_size(_tmp5B,sizeof(void*));(i ++,_tmp66=_tmp66->tl)){
# 282
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp66))->hd)->type;
# 283
if(_tmp68 != 0)t=Cyc_Tcutil_rsubstitute(temp,_tmp68,t);
# 284
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)_tmp5B.curr)[i]);}}
# 286
_npop_handler(0);return;}
# 280
;_pop_region(temp);};}_LL2F: _tmp5C=_tmp3D.f1;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp5D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5C;if(_tmp5D->tag != 12)goto _LL31;else{_tmp5E=_tmp5D->f1;_tmp5F=_tmp5D->f2;}};_tmp60=_tmp3D.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp61=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp60;if(_tmp61->tag != 6)goto _LL31;else{_tmp62=_tmp61->f2;}};_LL30:
# 289
{int i=0;for(0;i < _get_dyneither_size(_tmp62,sizeof(void*));(i ++,_tmp5F=_tmp5F->tl)){
# 290
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp5F))->hd)->type,((void**)_tmp62.curr)[i]);}}
# 291
return;_LL31: _tmp63=_tmp3D.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp64=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp63;if(_tmp64->tag != 5)goto _LL33;};if(!
# 292
Cyc_Tcutil_is_noalias_pointer(t))goto _LL33;_LL32:
# 293
{const char*_tmp7DA;void*_tmp7D9;(_tmp7D9=0,Cyc_Tcutil_warn(loc,((_tmp7DA="argument may still contain unique pointers",_tag_dyneither(_tmp7DA,sizeof(char),43))),_tag_dyneither(_tmp7D9,sizeof(void*),0)));}
# 294
return;_LL33:;_LL34:
# 295
 return;_LL22:;}
# 299
static void Cyc_NewControlFlow_check_unique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
# 300
struct _tuple0 _tmp7DB;struct _tuple0 _tmp6C=(_tmp7DB.f1=Cyc_Tcutil_compress(t),((_tmp7DB.f2=r,_tmp7DB)));void*_tmp6D;enum Cyc_CfFlowInfo_InitLevel _tmp6F;void*_tmp70;void*_tmp72;void*_tmp74;struct Cyc_Absyn_DatatypeFieldInfo _tmp76;union Cyc_Absyn_DatatypeFieldInfoU _tmp77;struct _tuple2 _tmp78;struct Cyc_Absyn_Datatypefield*_tmp79;void*_tmp7A;struct _dyneither_ptr _tmp7C;void*_tmp7D;struct Cyc_List_List*_tmp7F;void*_tmp80;struct _dyneither_ptr _tmp82;void*_tmp83;struct Cyc_Absyn_AggrInfo _tmp85;union Cyc_Absyn_AggrInfoU _tmp86;struct Cyc_List_List*_tmp87;void*_tmp88;struct _dyneither_ptr _tmp8A;void*_tmp8B;enum Cyc_Absyn_AggrKind _tmp8D;struct Cyc_List_List*_tmp8E;void*_tmp8F;struct _dyneither_ptr _tmp91;void*_tmp92;_LL36: _tmp6D=_tmp6C.f2;{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp6E=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp6D;if(_tmp6E->tag != 3)goto _LL38;else{_tmp6F=_tmp6E->f1;if(_tmp6F != Cyc_CfFlowInfo_NoneIL)goto _LL38;}};_LL37:
# 301
 return;_LL38: _tmp70=_tmp6C.f2;{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp71=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp70;if(_tmp71->tag != 0)goto _LL3A;};_LL39:
# 302
 return;_LL3A: _tmp72=_tmp6C.f2;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp73=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp72;if(_tmp73->tag != 7)goto _LL3C;};_LL3B:
# 303
 return;_LL3C: _tmp74=_tmp6C.f1;{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp75=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp74;if(_tmp75->tag != 4)goto _LL3E;else{_tmp76=_tmp75->f1;_tmp77=_tmp76.field_info;if((_tmp77.KnownDatatypefield).tag != 2)goto _LL3E;_tmp78=(struct _tuple2)(_tmp77.KnownDatatypefield).val;_tmp79=_tmp78.f2;}};_tmp7A=_tmp6C.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp7B=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp7A;if(_tmp7B->tag != 6)goto _LL3E;else{_tmp7C=_tmp7B->f2;}};_LL3D:
# 305
 if(_tmp79->typs == 0)
# 306
return;
# 307
_tmp7F=_tmp79->typs;_tmp82=_tmp7C;goto _LL3F;_LL3E: _tmp7D=_tmp6C.f1;{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp7E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp7D;if(_tmp7E->tag != 10)goto _LL40;else{_tmp7F=_tmp7E->f1;}};_tmp80=_tmp6C.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp81=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp80;if(_tmp81->tag != 6)goto _LL40;else{_tmp82=_tmp81->f2;}};_LL3F: {
# 309
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7F);
# 310
{int i=0;for(0;i < sz;++ i){
# 311
Cyc_NewControlFlow_check_nounique(env,loc,(*((struct _tuple19*)((struct Cyc_List_List*)_check_null(_tmp7F))->hd)).f2,*((void**)_check_dyneither_subscript(_tmp82,sizeof(void*),i)));}}
# 313
return;}_LL40: _tmp83=_tmp6C.f1;{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp84=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp83;if(_tmp84->tag != 11)goto _LL42;else{_tmp85=_tmp84->f1;_tmp86=_tmp85.aggr_info;_tmp87=_tmp85.targs;}};_tmp88=_tmp6C.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp89=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp88;if(_tmp89->tag != 6)goto _LL42;else{_tmp8A=_tmp89->f2;}};_LL41: {
# 316
struct Cyc_Absyn_Aggrdecl*_tmp94=Cyc_Absyn_get_known_aggrdecl(_tmp86);
# 317
if(_tmp94->impl == 0)return;{
# 318
struct Cyc_List_List*_tmp95=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp94->impl))->fields;
# 319
struct _RegionHandle _tmp96=_new_region("temp");struct _RegionHandle*temp=& _tmp96;_push_region(temp);
# 320
{struct Cyc_List_List*_tmp97=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp94->tvs,_tmp87);
# 321
{int i=0;for(0;i < _get_dyneither_size(_tmp8A,sizeof(void*));(i ++,_tmp95=_tmp95->tl)){
# 322
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp95))->hd)->type;
# 323
if(_tmp97 != 0)t=Cyc_Tcutil_rsubstitute(temp,_tmp97,t);
# 324
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)_tmp8A.curr)[i]);}}
# 326
_npop_handler(0);return;}
# 320
;_pop_region(temp);};}_LL42: _tmp8B=_tmp6C.f1;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp8C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp8B;if(_tmp8C->tag != 12)goto _LL44;else{_tmp8D=_tmp8C->f1;_tmp8E=_tmp8C->f2;}};_tmp8F=_tmp6C.f2;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp90=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp8F;if(_tmp90->tag != 6)goto _LL44;else{_tmp91=_tmp90->f2;}};_LL43:
# 329
{int i=0;for(0;i < _get_dyneither_size(_tmp91,sizeof(void*));(i ++,_tmp8E=_tmp8E->tl)){
# 330
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp8E))->hd)->type,((void**)_tmp91.curr)[i]);}}
# 331
return;_LL44: _tmp92=_tmp6C.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp93=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp92;if(_tmp93->tag != 5)goto _LL46;};if(!
# 332
Cyc_Tcutil_is_noalias_pointer(t))goto _LL46;_LL45:
# 333
{const char*_tmp7DE;void*_tmp7DD;(_tmp7DD=0,Cyc_Tcutil_warn(loc,((_tmp7DE="argument may still contain unique pointers",_tag_dyneither(_tmp7DE,sizeof(char),43))),_tag_dyneither(_tmp7DD,sizeof(void*),0)));}
# 334
return;_LL46:;_LL47:
# 335
 return;_LL35:;}
# 339
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
# 340
union Cyc_CfFlowInfo_FlowInfo _tmp9A=inflow;int _tmp9B;struct _tuple13 _tmp9C;struct Cyc_Dict_Dict _tmp9D;struct Cyc_List_List*_tmp9E;_LL49: if((_tmp9A.BottomFL).tag != 1)goto _LL4B;_tmp9B=(int)(_tmp9A.BottomFL).val;_LL4A:
# 341
 return Cyc_CfFlowInfo_BottomFL();_LL4B: if((_tmp9A.ReachableFL).tag != 2)goto _LL48;_tmp9C=(struct _tuple13)(_tmp9A.ReachableFL).val;_tmp9D=_tmp9C.f1;_tmp9E=_tmp9C.f2;_LL4C:
# 343
 if(!Cyc_Tcutil_is_noalias_pointer(t)){
# 344
{const char*_tmp7E1;void*_tmp7E0;(_tmp7E0=0,Cyc_Tcutil_terr(loc,((_tmp7E1="noliveunique attribute requires unique pointer",_tag_dyneither(_tmp7E1,sizeof(char),47))),_tag_dyneither(_tmp7E0,sizeof(void*),0)));}
# 345
return Cyc_CfFlowInfo_BottomFL();}{
# 347
void*_tmpA1=Cyc_Tcutil_pointer_elt_type(t);
# 348
{void*_tmpA2=r;struct Cyc_CfFlowInfo_Place*_tmpA4;_LL4E: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmpA3=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpA2;if(_tmpA3->tag != 5)goto _LL50;else{_tmpA4=_tmpA3->f1;}}_LL4F:
# 350
 Cyc_NewControlFlow_check_nounique(env,loc,_tmpA1,Cyc_CfFlowInfo_lookup_place(_tmp9D,_tmpA4));
# 351
goto _LL4D;_LL50:;_LL51:
# 353
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,_tmpA1)){
# 354
const char*_tmp7E4;void*_tmp7E3;(_tmp7E3=0,Cyc_Tcutil_warn(loc,((_tmp7E4="argument may contain live unique pointers",_tag_dyneither(_tmp7E4,sizeof(char),42))),_tag_dyneither(_tmp7E3,sizeof(void*),0)));}
# 355
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);_LL4D:;}{
# 357
struct Cyc_Dict_Dict _tmpA7=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp9D,env->all_changed,r);
# 358
if(_tmp9D.t == _tmpA7.t)return inflow;{
# 359
union Cyc_CfFlowInfo_FlowInfo _tmpA8=Cyc_CfFlowInfo_ReachableFL(_tmpA7,_tmp9E);
# 360
Cyc_NewControlFlow_update_tryflow(env,_tmpA8);
# 361
return _tmpA8;};};};_LL48:;}struct _tuple20{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 366
static struct _tuple20 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 373
if(es == 0){
# 374
struct _tuple20 _tmp7E5;return(_tmp7E5.f1=inflow,((_tmp7E5.f2=0,_tmp7E5)));}
# 375
if(es->tl == 0){
# 376
union Cyc_CfFlowInfo_FlowInfo _tmpAB;void*_tmpAC;struct _tuple14 _tmpAA=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmpAB=_tmpAA.f1;_tmpAC=_tmpAA.f2;{
# 377
struct Cyc_List_List*_tmp7E8;struct _tuple20 _tmp7E7;return(_tmp7E7.f1=_tmpAB,((_tmp7E7.f2=((_tmp7E8=_region_malloc(rgn,sizeof(*_tmp7E8)),((_tmp7E8->hd=_tmpAC,((_tmp7E8->tl=0,_tmp7E8)))))),_tmp7E7)));};}{
# 379
struct Cyc_Dict_Dict*outer_all_changed=env->all_changed;
# 380
struct Cyc_Dict_Dict this_all_changed;
# 381
union Cyc_CfFlowInfo_FlowInfo old_inflow;
# 382
union Cyc_CfFlowInfo_FlowInfo outflow;
# 383
struct Cyc_List_List*rvals;
# 384
do{
# 385
this_all_changed=(env->fenv)->mt_place_set;
# 387
{struct Cyc_Dict_Dict*_tmp7E9;env->all_changed=((_tmp7E9=_region_malloc(env->r,sizeof(*_tmp7E9)),((_tmp7E9[0]=(env->fenv)->mt_place_set,_tmp7E9))));}{
# 388
union Cyc_CfFlowInfo_FlowInfo _tmpB1;void*_tmpB2;struct _tuple14 _tmpB0=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmpB1=_tmpB0.f1;_tmpB2=_tmpB0.f2;
# 389
outflow=_tmpB1;
# 390
{struct Cyc_List_List*_tmp7EA;rvals=((_tmp7EA=_region_malloc(rgn,sizeof(*_tmp7EA)),((_tmp7EA->hd=_tmpB2,((_tmp7EA->tl=0,_tmp7EA))))));}
# 391
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);
# 393
{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){
# 394
{struct Cyc_Dict_Dict*_tmp7EB;env->all_changed=((_tmp7EB=_region_malloc(env->r,sizeof(*_tmp7EB)),((_tmp7EB[0]=(env->fenv)->mt_place_set,_tmp7EB))));}{
# 395
union Cyc_CfFlowInfo_FlowInfo _tmpB6;void*_tmpB7;struct _tuple14 _tmpB5=Cyc_NewControlFlow_anal_Rexp(env,others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);_tmpB6=_tmpB5.f1;_tmpB7=_tmpB5.f2;
# 396
{struct Cyc_List_List*_tmp7EC;rvals=((_tmp7EC=_region_malloc(rgn,sizeof(*_tmp7EC)),((_tmp7EC->hd=_tmpB7,((_tmp7EC->tl=rvals,_tmp7EC))))));}
# 397
outflow=Cyc_CfFlowInfo_after_flow(env->fenv,(struct Cyc_Dict_Dict*)& this_all_changed,outflow,_tmpB6,this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)));
# 399
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);};}}
# 402
old_inflow=inflow;
# 405
inflow=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,inflow,outflow);};}while(!
# 410
Cyc_CfFlowInfo_flow_lessthan_approx(inflow,old_inflow));
# 411
if(outer_all_changed == 0)
# 412
env->all_changed=0;else{
# 414
struct Cyc_Dict_Dict*_tmp7ED;env->all_changed=((_tmp7ED=_region_malloc(env->r,sizeof(*_tmp7ED)),((_tmp7ED[0]=Cyc_CfFlowInfo_union_place_set(*outer_all_changed,this_all_changed,0),_tmp7ED))));}
# 416
Cyc_NewControlFlow_update_tryflow(env,outflow);{
# 417
struct _tuple20 _tmp7EE;return(_tmp7EE.f1=outflow,((_tmp7EE.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp7EE)));};};}
# 422
static struct _tuple14 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 425
struct _RegionHandle _tmpBB=_new_region("rgn");struct _RegionHandle*rgn=& _tmpBB;_push_region(rgn);
# 426
{union Cyc_CfFlowInfo_FlowInfo _tmpBD;struct Cyc_List_List*_tmpBE;struct _tuple20 _tmpBC=
# 427
Cyc_NewControlFlow_anal_unordered_Rexps(rgn,env,inflow,es,0,0);_tmpBD=_tmpBC.f1;_tmpBE=_tmpBC.f2;
# 428
{union Cyc_CfFlowInfo_FlowInfo _tmpBF=_tmpBD;struct _tuple13 _tmpC0;struct Cyc_Dict_Dict _tmpC1;int _tmpC2;_LL53: if((_tmpBF.ReachableFL).tag != 2)goto _LL55;_tmpC0=(struct _tuple13)(_tmpBF.ReachableFL).val;_tmpC1=_tmpC0.f1;_LL54:
# 430
 for(0;_tmpBE != 0;(_tmpBE=_tmpBE->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
# 431
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmpC1,(void*)_tmpBE->hd)== Cyc_CfFlowInfo_NoneIL){
# 432
const char*_tmp7F1;void*_tmp7F0;(_tmp7F0=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp7F1="expression may not be initialized",_tag_dyneither(_tmp7F1,sizeof(char),34))),_tag_dyneither(_tmp7F0,sizeof(void*),0)));}}
# 433
goto _LL52;_LL55: if((_tmpBF.BottomFL).tag != 1)goto _LL52;_tmpC2=(int)(_tmpBF.BottomFL).val;_LL56:
# 434
 goto _LL52;_LL52:;}{
# 436
struct _tuple14 _tmp7F2;struct _tuple14 _tmpC6=(_tmp7F2.f1=_tmpBD,((_tmp7F2.f2=(env->fenv)->unknown_all,_tmp7F2)));_npop_handler(0);return _tmpC6;};}
# 426
;_pop_region(rgn);}
# 450 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il){
# 456
union Cyc_CfFlowInfo_FlowInfo _tmpC7=outflow;int _tmpC8;struct _tuple13 _tmpC9;struct Cyc_Dict_Dict _tmpCA;struct Cyc_List_List*_tmpCB;_LL58: if((_tmpC7.BottomFL).tag != 1)goto _LL5A;_tmpC8=(int)(_tmpC7.BottomFL).val;_LL59:
# 457
 return outflow;_LL5A: if((_tmpC7.ReachableFL).tag != 2)goto _LL57;_tmpC9=(struct _tuple13)(_tmpC7.ReachableFL).val;_tmpCA=_tmpC9.f1;_tmpCB=_tmpC9.f2;_LL5B: {
# 459
union Cyc_CfFlowInfo_AbsLVal _tmpCC=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,e)).f2;int _tmpCD;struct Cyc_CfFlowInfo_Place*_tmpCE;_LL5D: if((_tmpCC.UnknownL).tag != 2)goto _LL5F;_tmpCD=(int)(_tmpCC.UnknownL).val;_LL5E:
# 463
 return outflow;_LL5F: if((_tmpCC.PlaceL).tag != 1)goto _LL5C;_tmpCE=(struct Cyc_CfFlowInfo_Place*)(_tmpCC.PlaceL).val;_LL60: {
# 467
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
# 468
union Cyc_CfFlowInfo_FlowInfo _tmpCF=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpCA,env->all_changed,_tmpCE,nzval),_tmpCB);
# 472
return _tmpCF;}_LL5C:;}_LL57:;}
# 481
static struct _tuple16 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il){
# 484
union Cyc_CfFlowInfo_FlowInfo _tmpD0=outflow;int _tmpD1;struct _tuple13 _tmpD2;struct Cyc_Dict_Dict _tmpD3;struct Cyc_List_List*_tmpD4;_LL62: if((_tmpD0.BottomFL).tag != 1)goto _LL64;_tmpD1=(int)(_tmpD0.BottomFL).val;_LL63: {
# 485
struct _tuple16 _tmp7F3;return(_tmp7F3.f1=outflow,((_tmp7F3.f2=outflow,_tmp7F3)));}_LL64: if((_tmpD0.ReachableFL).tag != 2)goto _LL61;_tmpD2=(struct _tuple13)(_tmpD0.ReachableFL).val;_tmpD3=_tmpD2.f1;_tmpD4=_tmpD2.f2;_LL65: {
# 487
union Cyc_CfFlowInfo_AbsLVal _tmpD6=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,e)).f2;int _tmpD7;struct Cyc_CfFlowInfo_Place*_tmpD8;_LL67: if((_tmpD6.UnknownL).tag != 2)goto _LL69;_tmpD7=(int)(_tmpD6.UnknownL).val;_LL68: {
# 488
struct _tuple16 _tmp7F4;return(_tmp7F4.f1=outflow,((_tmp7F4.f2=outflow,_tmp7F4)));}_LL69: if((_tmpD6.PlaceL).tag != 1)goto _LL66;_tmpD8=(struct Cyc_CfFlowInfo_Place*)(_tmpD6.PlaceL).val;_LL6A: {
# 490
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
# 491
struct _tuple16 _tmp7F5;return
# 492
(_tmp7F5.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpD3,env->all_changed,_tmpD8,nzval),_tmpD4),((_tmp7F5.f2=
# 494
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpD3,env->all_changed,_tmpD8,(env->fenv)->zero),_tmpD4),_tmp7F5)));}_LL66:;}_LL61:;}
# 500
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
# 501
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 506
static struct _tuple14 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r){
# 510
void*_tmpDD=Cyc_Tcutil_compress((void*)_check_null(e->topt));struct Cyc_Absyn_PtrInfo _tmpDF;void*_tmpE0;struct Cyc_Absyn_PtrAtts _tmpE1;union Cyc_Absyn_Constraint*_tmpE2;union Cyc_Absyn_Constraint*_tmpE3;_LL6C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDD;if(_tmpDE->tag != 5)goto _LL6E;else{_tmpDF=_tmpDE->f1;_tmpE0=_tmpDF.elt_typ;_tmpE1=_tmpDF.ptr_atts;_tmpE2=_tmpE1.bounds;_tmpE3=_tmpE1.zero_term;}}_LL6D: {
# 512
union Cyc_CfFlowInfo_FlowInfo _tmpE4=f;int _tmpE5;struct _tuple13 _tmpE6;struct Cyc_Dict_Dict _tmpE7;struct Cyc_List_List*_tmpE8;_LL71: if((_tmpE4.BottomFL).tag != 1)goto _LL73;_tmpE5=(int)(_tmpE4.BottomFL).val;_LL72: {
# 514
struct _tuple14 _tmp7F6;return(_tmp7F6.f1=f,((_tmp7F6.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpE0,0,(env->fenv)->unknown_all),_tmp7F6)));}_LL73: if((_tmpE4.ReachableFL).tag != 2)goto _LL70;_tmpE6=(struct _tuple13)(_tmpE4.ReachableFL).val;_tmpE7=_tmpE6.f1;_tmpE8=_tmpE6.f2;_LL74:
# 517
 if(Cyc_Tcutil_is_bound_one(_tmpE2)){
# 518
void*_tmpEA=r;struct Cyc_CfFlowInfo_Place*_tmpEE;enum Cyc_CfFlowInfo_InitLevel _tmpF1;_LL76: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmpEB=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmpEA;if(_tmpEB->tag != 1)goto _LL78;}_LL77:
# 519
 goto _LL79;_LL78: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmpEC=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmpEA;if(_tmpEC->tag != 2)goto _LL7A;}_LL79:
# 521
{void*_tmpF2=e->annot;struct Cyc_List_List*_tmpF4;_LL83: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpF3=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpF2;if(_tmpF3->tag != Cyc_CfFlowInfo_NotZero)goto _LL85;else{_tmpF4=_tmpF3->f1;}}_LL84:
# 523
 if(!Cyc_CfFlowInfo_same_relns(_tmpE8,_tmpF4))goto _LL86;
# 524
goto _LL82;_LL85:;_LL86:
# 527
{void*_tmpF5=e->r;_LL88: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpF6=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF5;if(_tmpF6->tag != 22)goto _LL8A;}_LL89:
# 529
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp7F9;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp7F8;e->annot=(void*)((_tmp7F8=_cycalloc(sizeof(*_tmp7F8)),((_tmp7F8[0]=((_tmp7F9.tag=Cyc_CfFlowInfo_NotZero,((_tmp7F9.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmpE8),_tmp7F9)))),_tmp7F8))));}
# 530
goto _LL87;_LL8A:;_LL8B:
# 532
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
# 533
goto _LL87;_LL87:;}
# 535
goto _LL82;_LL82:;}
# 537
goto _LL75;_LL7A: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmpED=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpEA;if(_tmpED->tag != 5)goto _LL7C;else{_tmpEE=_tmpED->f1;}}_LL7B:
# 539
{void*_tmpF9=e->annot;struct Cyc_List_List*_tmpFB;_LL8D: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpFA=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpF9;if(_tmpFA->tag != Cyc_CfFlowInfo_NotZero)goto _LL8F;else{_tmpFB=_tmpFA->f1;}}_LL8E:
# 541
 if(!Cyc_CfFlowInfo_same_relns(_tmpE8,_tmpFB))goto _LL90;
# 542
goto _LL8C;_LL8F:;_LL90:
# 545
{void*_tmpFC=e->r;_LL92: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpFD=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpFC;if(_tmpFD->tag != 22)goto _LL94;}_LL93:
# 547
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp7FC;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp7FB;e->annot=(void*)((_tmp7FB=_cycalloc(sizeof(*_tmp7FB)),((_tmp7FB[0]=((_tmp7FC.tag=Cyc_CfFlowInfo_NotZero,((_tmp7FC.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmpE8),_tmp7FC)))),_tmp7FB))));}
# 548
goto _LL91;_LL94:;_LL95:
# 550
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
# 551
goto _LL91;_LL91:;}
# 553
goto _LL8C;_LL8C:;}{
# 555
struct _tuple14 _tmp7FD;return(_tmp7FD.f1=f,((_tmp7FD.f2=Cyc_CfFlowInfo_lookup_place(_tmpE7,_tmpEE),_tmp7FD)));};_LL7C: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmpEF=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmpEA;if(_tmpEF->tag != 0)goto _LL7E;}_LL7D:
# 557
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
# 558
struct _tuple14 _tmp7FE;return(_tmp7FE.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7FE.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpE0,0,(env->fenv)->unknown_all),_tmp7FE)));};_LL7E: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpF0=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpEA;if(_tmpF0->tag != 3)goto _LL80;else{_tmpF1=_tmpF0->f1;}}_LL7F:
# 560
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpF1);goto _LL81;_LL80:;_LL81:
# 562
{void*_tmp102=e->r;_LL97: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp103=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp102;if(_tmp103->tag != 22)goto _LL99;}_LL98:
# 564
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp801;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp800;e->annot=(void*)((_tmp800=_cycalloc(sizeof(*_tmp800)),((_tmp800[0]=((_tmp801.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp801.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmpE8),_tmp801)))),_tmp800))));}
# 565
goto _LL96;_LL99:;_LL9A:
# 567
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
# 568
goto _LL96;_LL96:;}
# 570
goto _LL75;_LL75:;}else{
# 573
void*_tmp106=e->annot;struct Cyc_List_List*_tmp108;_LL9C: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp107=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp106;if(_tmp107->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL9E;else{_tmp108=_tmp107->f1;}}_LL9D:
# 575
 if(!Cyc_CfFlowInfo_same_relns(_tmpE8,_tmp108))goto _LL9F;
# 576
goto _LL9B;_LL9E:;_LL9F:
# 578
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp804;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp803;e->annot=(void*)((_tmp803=_cycalloc(sizeof(*_tmp803)),((_tmp803[0]=((_tmp804.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp804.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmpE8),_tmp804)))),_tmp803))));}
# 579
goto _LL9B;_LL9B:;}
# 582
switch(Cyc_CfFlowInfo_initlevel(env->fenv,_tmpE7,r)){case Cyc_CfFlowInfo_NoneIL: _LLA0:
# 584
{void*_tmp10B=r;_LLA3: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp10C=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp10B;if(_tmp10C->tag != 7)goto _LLA5;}_LLA4:
# 586
{const char*_tmp807;void*_tmp806;(_tmp806=0,Cyc_Tcutil_terr(e->loc,((_tmp807="attempt to use a unique pointer that has already been copied",_tag_dyneither(_tmp807,sizeof(char),61))),_tag_dyneither(_tmp806,sizeof(void*),0)));}
# 587
goto _LLA2;_LLA5:;_LLA6: {
# 589
const char*_tmp80A;void*_tmp809;(_tmp809=0,Cyc_Tcutil_terr(e->loc,((_tmp80A="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp80A,sizeof(char),46))),_tag_dyneither(_tmp809,sizeof(void*),0)));}_LLA2:;}
# 591
goto _LLA1;case Cyc_CfFlowInfo_AllIL: _LLA1: {
# 593
struct _tuple14 _tmp80B;return(_tmp80B.f1=f,((_tmp80B.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpE0,0,(env->fenv)->unknown_all),_tmp80B)));}case Cyc_CfFlowInfo_ThisIL: _LLA7: {
# 595
struct _tuple14 _tmp80C;return(_tmp80C.f1=f,((_tmp80C.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpE0,0,(env->fenv)->unknown_none),_tmp80C)));}}_LL70:;}_LL6E:;_LL6F: {
# 598
struct Cyc_Core_Impossible_exn_struct _tmp812;const char*_tmp811;struct Cyc_Core_Impossible_exn_struct*_tmp810;(int)_throw((void*)((_tmp810=_cycalloc(sizeof(*_tmp810)),((_tmp810[0]=((_tmp812.tag=Cyc_Core_Impossible,((_tmp812.f1=((_tmp811="right deref of non-pointer-type",_tag_dyneither(_tmp811,sizeof(char),32))),_tmp812)))),_tmp810)))));}_LL6B:;}
# 604
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 606
void*_tmp116=e1->r;void*_tmp118;struct Cyc_Absyn_Vardecl*_tmp11A;void*_tmp11C;struct Cyc_Absyn_Vardecl*_tmp11E;void*_tmp120;struct Cyc_Absyn_Vardecl*_tmp122;void*_tmp124;struct Cyc_Absyn_Vardecl*_tmp126;_LLAA: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp117=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp117->tag != 1)goto _LLAC;else{_tmp118=(void*)_tmp117->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp119=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp118;if(_tmp119->tag != 5)goto _LLAC;else{_tmp11A=_tmp119->f1;}};}}_LLAB:
# 607
 _tmp11E=_tmp11A;goto _LLAD;_LLAC: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp11B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp11B->tag != 1)goto _LLAE;else{_tmp11C=(void*)_tmp11B->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp11D=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp11C;if(_tmp11D->tag != 4)goto _LLAE;else{_tmp11E=_tmp11D->f1;}};}}_LLAD:
# 608
 _tmp122=_tmp11E;goto _LLAF;_LLAE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp11F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp11F->tag != 1)goto _LLB0;else{_tmp120=(void*)_tmp11F->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp121=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp120;if(_tmp121->tag != 3)goto _LLB0;else{_tmp122=_tmp121->f1;}};}}_LLAF:
# 609
 _tmp126=_tmp122;goto _LLB1;_LLB0: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp123=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp123->tag != 1)goto _LLB2;else{_tmp124=(void*)_tmp123->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp125=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp124;if(_tmp125->tag != 1)goto _LLB2;else{_tmp126=_tmp125->f1;}};}}_LLB1:
# 611
 if(!_tmp126->escapes){
# 612
void*_tmp127=e2->r;void*_tmp129;struct Cyc_Absyn_Vardecl*_tmp12B;void*_tmp12D;struct Cyc_Absyn_Vardecl*_tmp12F;void*_tmp131;struct Cyc_Absyn_Vardecl*_tmp133;void*_tmp135;struct Cyc_Absyn_Vardecl*_tmp137;_LLB5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp128=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp127;if(_tmp128->tag != 1)goto _LLB7;else{_tmp129=(void*)_tmp128->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp12A=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp129;if(_tmp12A->tag != 5)goto _LLB7;else{_tmp12B=_tmp12A->f1;}};}}_LLB6:
# 613
 _tmp12F=_tmp12B;goto _LLB8;_LLB7: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp12C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp127;if(_tmp12C->tag != 1)goto _LLB9;else{_tmp12D=(void*)_tmp12C->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp12E=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp12D;if(_tmp12E->tag != 4)goto _LLB9;else{_tmp12F=_tmp12E->f1;}};}}_LLB8:
# 614
 _tmp133=_tmp12F;goto _LLBA;_LLB9: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp130=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp127;if(_tmp130->tag != 1)goto _LLBB;else{_tmp131=(void*)_tmp130->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp132=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp131;if(_tmp132->tag != 3)goto _LLBB;else{_tmp133=_tmp132->f1;}};}}_LLBA:
# 615
 _tmp137=_tmp133;goto _LLBC;_LLBB: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp134=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp127;if(_tmp134->tag != 1)goto _LLBD;else{_tmp135=(void*)_tmp134->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp136=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp135;if(_tmp136->tag != 1)goto _LLBD;else{_tmp137=_tmp136->f1;}};}}_LLBC:
# 617
 if(!_tmp137->escapes){
# 619
int found=0;
# 620
{struct Cyc_List_List*_tmp138=relns;for(0;_tmp138 != 0;_tmp138=_tmp138->tl){
# 621
struct Cyc_CfFlowInfo_Reln*_tmp139=(struct Cyc_CfFlowInfo_Reln*)_tmp138->hd;
# 622
if(_tmp139->vd == _tmp137){
# 623
union Cyc_CfFlowInfo_RelnOp _tmp13A=_tmp139->rop;struct Cyc_Absyn_Vardecl*_tmp13B;_LLC0: if((_tmp13A.LessNumelts).tag != 3)goto _LLC2;_tmp13B=(struct Cyc_Absyn_Vardecl*)(_tmp13A.LessNumelts).val;if(!(_tmp13B == _tmp126))goto _LLC2;_LLC1:
# 624
 return relns;_LLC2:;_LLC3:
# 625
 continue;_LLBF:;}}}
# 628
if(!found){
# 629
struct Cyc_CfFlowInfo_Reln*_tmp815;struct Cyc_List_List*_tmp814;return(_tmp814=_region_malloc(rgn,sizeof(*_tmp814)),((_tmp814->hd=((_tmp815=_region_malloc(rgn,sizeof(*_tmp815)),((_tmp815->vd=_tmp137,((_tmp815->rop=Cyc_CfFlowInfo_LessNumelts(_tmp126),_tmp815)))))),((_tmp814->tl=relns,_tmp814)))));}}
# 631
return relns;_LLBD:;_LLBE:
# 632
 return relns;_LLB4:;}else{
# 634
return relns;}_LLB2:;_LLB3:
# 635
 return relns;_LLA9:;}
# 641
static struct _tuple14 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 649
union Cyc_CfFlowInfo_FlowInfo _tmp13E=outflow;int _tmp13F;struct _tuple13 _tmp140;struct Cyc_Dict_Dict _tmp141;struct Cyc_List_List*_tmp142;_LLC5: if((_tmp13E.BottomFL).tag != 1)goto _LLC7;_tmp13F=(int)(_tmp13E.BottomFL).val;_LLC6: {
# 652
struct _tuple14 _tmp816;return(_tmp816.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp816.f2=rval,_tmp816)));}_LLC7: if((_tmp13E.ReachableFL).tag != 2)goto _LLC4;_tmp140=(struct _tuple13)(_tmp13E.ReachableFL).val;_tmp141=_tmp140.f1;_tmp142=_tmp140.f2;_LLC8: {
# 654
union Cyc_CfFlowInfo_AbsLVal _tmp144=lval;struct Cyc_CfFlowInfo_Place*_tmp145;int _tmp146;_LLCA: if((_tmp144.PlaceL).tag != 1)goto _LLCC;_tmp145=(struct Cyc_CfFlowInfo_Place*)(_tmp144.PlaceL).val;_LLCB: {
# 656
struct Cyc_Dict_Dict _tmp147=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp141,env->all_changed,_tmp145,rval);
# 657
_tmp142=Cyc_CfFlowInfo_reln_assign_exp(fenv->r,_tmp142,lexp,rexp);
# 658
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp147,_tmp142);
# 659
if(Cyc_NewControlFlow_warn_lose_unique  && 
# 660
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(lexp->topt))){
# 662
void*_tmp148=Cyc_CfFlowInfo_lookup_place(_tmp141,_tmp145);enum Cyc_CfFlowInfo_InitLevel _tmp14A;_LLCF: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp149=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp148;if(_tmp149->tag != 3)goto _LLD1;else{_tmp14A=_tmp149->f1;if(_tmp14A != Cyc_CfFlowInfo_NoneIL)goto _LLD1;}}_LLD0:
# 663
 goto _LLD2;_LLD1: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp14B=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp148;if(_tmp14B->tag != 7)goto _LLD3;}_LLD2:
# 664
 goto _LLD4;_LLD3: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp14C=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp148;if(_tmp14C->tag != 0)goto _LLD5;}_LLD4:
# 665
 goto _LLCE;_LLD5:;_LLD6:
# 667
{const char*_tmp819;void*_tmp818;(_tmp818=0,Cyc_Tcutil_warn(lexp->loc,((_tmp819="assignment may overwrite unique pointer(s)",_tag_dyneither(_tmp819,sizeof(char),43))),_tag_dyneither(_tmp818,sizeof(void*),0)));}
# 668
goto _LLCE;_LLCE:;}
# 672
Cyc_NewControlFlow_update_tryflow(env,outflow);{
# 674
struct _tuple14 _tmp81A;return(_tmp81A.f1=outflow,((_tmp81A.f2=rval,_tmp81A)));};}_LLCC: if((_tmp144.UnknownL).tag != 2)goto _LLC9;_tmp146=(int)(_tmp144.UnknownL).val;_LLCD: {
# 677
struct _tuple14 _tmp81B;return(_tmp81B.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval),((_tmp81B.f2=rval,_tmp81B)));}_LLC9:;}_LLC4:;}
# 682
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e){
# 683
void*_tmp151=e->r;void*_tmp153;void*_tmp156;void*_tmp159;struct Cyc_Absyn_Exp*_tmp15C;struct Cyc_Absyn_Exp*_tmp15E;struct Cyc_Absyn_Exp*_tmp160;struct Cyc_Absyn_Exp*_tmp162;struct Cyc_Absyn_Exp*_tmp164;_LLD8: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp152=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp151;if(_tmp152->tag != 1)goto _LLDA;else{_tmp153=(void*)_tmp152->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp154=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp153;if(_tmp154->tag != 4)goto _LLDA;};}}_LLD9:
# 684
 goto _LLDB;_LLDA: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp155=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp151;if(_tmp155->tag != 1)goto _LLDC;else{_tmp156=(void*)_tmp155->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp157=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp156;if(_tmp157->tag != 3)goto _LLDC;};}}_LLDB:
# 685
 goto _LLDD;_LLDC: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp158=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp151;if(_tmp158->tag != 1)goto _LLDE;else{_tmp159=(void*)_tmp158->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp15A=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp159;if(_tmp15A->tag != 5)goto _LLDE;};}}_LLDD:
# 686
 return 1;_LLDE: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp15B=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp151;if(_tmp15B->tag != 19)goto _LLE0;else{_tmp15C=_tmp15B->f1;}}_LLDF:
# 687
 _tmp15E=_tmp15C;goto _LLE1;_LLE0: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp15D=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp151;if(_tmp15D->tag != 20)goto _LLE2;else{_tmp15E=_tmp15D->f1;}}_LLE1:
# 688
 _tmp160=_tmp15E;goto _LLE3;_LLE2: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp15F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp151;if(_tmp15F->tag != 21)goto _LLE4;else{_tmp160=_tmp15F->f1;}}_LLE3:
# 690
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp160);_LLE4: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp161=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp151;if(_tmp161->tag != 22)goto _LLE6;else{_tmp162=_tmp161->f1;}}_LLE5: {
# 692
void*_tmp165=Cyc_Tcutil_compress((void*)_check_null(_tmp162->topt));_LLEB: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp166=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp165;if(_tmp166->tag != 10)goto _LLED;}_LLEC:
# 693
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp162);_LLED:;_LLEE:
# 694
 return 0;_LLEA:;}_LLE6: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp163=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp151;if(_tmp163->tag != 13)goto _LLE8;else{_tmp164=_tmp163->f2;}}_LLE7:
# 696
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp164);_LLE8:;_LLE9:
# 697
 return 0;_LLD7:;}
# 701
static struct _tuple14 Cyc_NewControlFlow_anal_stmt_exp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
# 703
struct Cyc_CfFlowInfo_FlowEnv*_tmp167=env->fenv;
# 707
while(1){
# 708
union Cyc_CfFlowInfo_FlowInfo*_tmp169;struct _tuple17 _tmp168=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);_tmp169=_tmp168.f2;
# 709
inflow=*_tmp169;{
# 710
void*_tmp16A=s->r;struct Cyc_Absyn_Stmt*_tmp16C;struct Cyc_Absyn_Stmt*_tmp16D;struct Cyc_Absyn_Decl*_tmp16F;struct Cyc_Absyn_Decl _tmp170;void*_tmp171;struct Cyc_Absyn_Exp*_tmp173;struct Cyc_Absyn_Tvar*_tmp174;struct Cyc_Absyn_Vardecl*_tmp175;struct Cyc_Absyn_Stmt*_tmp176;struct Cyc_Absyn_Decl*_tmp178;struct Cyc_Absyn_Stmt*_tmp179;struct Cyc_Absyn_Exp*_tmp17B;_LLF0: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp16B=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp16A;if(_tmp16B->tag != 2)goto _LLF2;else{_tmp16C=_tmp16B->f1;_tmp16D=_tmp16B->f2;}}_LLF1:
# 712
 inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp16C);
# 713
s=_tmp16D;
# 714
goto _LLEF;_LLF2: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp16E=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp16A;if(_tmp16E->tag != 12)goto _LLF4;else{_tmp16F=_tmp16E->f1;_tmp170=*_tmp16F;_tmp171=_tmp170.r;{struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp172=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp171;if(_tmp172->tag != 5)goto _LLF4;else{_tmp173=_tmp172->f1;_tmp174=_tmp172->f2;_tmp175=_tmp172->f3;}};_tmp176=_tmp16E->f2;}}_LLF3: {
# 718
union Cyc_CfFlowInfo_FlowInfo _tmp17D;union Cyc_CfFlowInfo_AbsLVal _tmp17E;struct _tuple15 _tmp17C=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp173);_tmp17D=_tmp17C.f1;_tmp17E=_tmp17C.f2;{
# 719
struct _tuple15 _tmp81C;struct _tuple15 _tmp180=(_tmp81C.f1=_tmp17D,((_tmp81C.f2=_tmp17E,_tmp81C)));union Cyc_CfFlowInfo_FlowInfo _tmp181;struct _tuple13 _tmp182;struct Cyc_Dict_Dict _tmp183;struct Cyc_List_List*_tmp184;union Cyc_CfFlowInfo_AbsLVal _tmp185;struct Cyc_CfFlowInfo_Place*_tmp186;union Cyc_CfFlowInfo_FlowInfo _tmp187;int _tmp188;_LLFB: _tmp181=_tmp180.f1;if((_tmp181.ReachableFL).tag != 2)goto _LLFD;_tmp182=(struct _tuple13)(_tmp181.ReachableFL).val;_tmp183=_tmp182.f1;_tmp184=_tmp182.f2;_tmp185=_tmp180.f2;if((_tmp185.PlaceL).tag != 1)goto _LLFD;_tmp186=(struct Cyc_CfFlowInfo_Place*)(_tmp185.PlaceL).val;_LLFC: {
# 721
void*_tmp189=Cyc_CfFlowInfo_lookup_place(_tmp183,_tmp186);
# 723
void*t=(void*)_check_null(_tmp173->topt);
# 724
{void*_tmp18A=_tmp173->r;struct Cyc_Absyn_Exp*_tmp18C;_LL102: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp18B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp18A;if(_tmp18B->tag != 13)goto _LL104;else{_tmp18C=_tmp18B->f2;}}_LL103:
# 725
 t=(void*)_check_null(_tmp18C->topt);goto _LL101;_LL104:;_LL105:
# 726
 goto _LL101;_LL101:;}{
# 728
void*_tmp18D=Cyc_CfFlowInfo_make_unique_consumed(_tmp167,t,_tmp173,env->iteration_num,_tmp189);
# 729
_tmp183=Cyc_CfFlowInfo_assign_place(_tmp167,_tmp173->loc,_tmp183,env->all_changed,_tmp186,_tmp18D);
# 730
_tmp17D=Cyc_CfFlowInfo_ReachableFL(_tmp183,_tmp184);{
# 731
void*leaf;
# 732
switch(Cyc_CfFlowInfo_initlevel(_tmp167,_tmp183,_tmp189)){case Cyc_CfFlowInfo_AllIL: _LL106:
# 733
 leaf=_tmp167->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LL107:
# 734
 leaf=_tmp167->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LL108:
# 735
 leaf=_tmp167->unknown_this;break;}{
# 737
void*_tmp18E=Cyc_CfFlowInfo_typ_to_absrval(_tmp167,t,0,leaf);
# 738
_tmp17D=Cyc_NewControlFlow_use_Rval(env,_tmp173->loc,_tmp17D,_tmp189);{
# 739
struct Cyc_List_List _tmp81D;struct Cyc_List_List _tmp18F=(_tmp81D.hd=_tmp175,((_tmp81D.tl=0,_tmp81D)));
# 740
_tmp17D=Cyc_NewControlFlow_add_vars(_tmp167,_tmp17D,(struct Cyc_List_List*)& _tmp18F,_tmp167->unknown_all,s->loc);{
# 741
union Cyc_CfFlowInfo_FlowInfo _tmp191;void*_tmp192;struct _tuple14 _tmp190=Cyc_NewControlFlow_anal_stmt_exp(env,copy_ctxt,_tmp17D,_tmp176);_tmp191=_tmp190.f1;_tmp192=_tmp190.f2;{
# 742
union Cyc_CfFlowInfo_FlowInfo _tmp193=_tmp191;struct _tuple13 _tmp194;struct Cyc_Dict_Dict _tmp195;struct Cyc_List_List*_tmp196;_LL10B: if((_tmp193.ReachableFL).tag != 2)goto _LL10D;_tmp194=(struct _tuple13)(_tmp193.ReachableFL).val;_tmp195=_tmp194.f1;_tmp196=_tmp194.f2;_LL10C:
# 744
 _tmp195=Cyc_CfFlowInfo_assign_place(_tmp167,s->loc,_tmp195,env->all_changed,_tmp186,_tmp18E);{
# 745
union Cyc_CfFlowInfo_FlowInfo _tmp197=Cyc_CfFlowInfo_ReachableFL(_tmp195,_tmp196);
# 746
struct _tuple14 _tmp81E;return(_tmp81E.f1=_tmp197,((_tmp81E.f2=_tmp192,_tmp81E)));};_LL10D:;_LL10E: {
# 747
struct _tuple14 _tmp81F;return(_tmp81F.f1=_tmp191,((_tmp81F.f2=_tmp192,_tmp81F)));}_LL10A:;};};};};};};}_LLFD: _tmp187=_tmp180.f1;if((_tmp187.BottomFL).tag != 1)goto _LLFF;_tmp188=(int)(_tmp187.BottomFL).val;_LLFE: {
# 749
struct _tuple14 _tmp820;return(_tmp820.f1=_tmp17D,((_tmp820.f2=_tmp167->unknown_all,_tmp820)));}_LLFF:;_LL100:
# 750
{const char*_tmp823;void*_tmp822;(_tmp822=0,Cyc_Tcutil_terr(_tmp173->loc,((_tmp823="bad alias expression--no unique path found",_tag_dyneither(_tmp823,sizeof(char),43))),_tag_dyneither(_tmp822,sizeof(void*),0)));}{
# 751
struct _tuple14 _tmp824;return(_tmp824.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp824.f2=_tmp167->unknown_all,_tmp824)));};_LLFA:;};}_LLF4: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp177=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp16A;if(_tmp177->tag != 12)goto _LLF6;else{_tmp178=_tmp177->f1;_tmp179=_tmp177->f2;}}_LLF5:
# 754
 inflow=Cyc_NewControlFlow_anal_decl(env,inflow,_tmp178);
# 755
s=_tmp179;
# 756
goto _LLEF;_LLF6: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp17A=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp16A;if(_tmp17A->tag != 1)goto _LLF8;else{_tmp17B=_tmp17A->f1;}}_LLF7:
# 758
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp17B);_LLF8:;_LLF9: {
# 759
struct Cyc_Core_Impossible_exn_struct _tmp82A;const char*_tmp829;struct Cyc_Core_Impossible_exn_struct*_tmp828;(int)_throw((void*)((_tmp828=_cycalloc(sizeof(*_tmp828)),((_tmp828[0]=((_tmp82A.tag=Cyc_Core_Impossible,((_tmp82A.f1=((_tmp829="analyze_Rexp: ill-formed StmtExp",_tag_dyneither(_tmp829,sizeof(char),33))),_tmp82A)))),_tmp828)))));}_LLEF:;};}}struct _tuple21{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple22{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);static void _tmp8CC(unsigned int*_tmp8CB,unsigned int*_tmp8CA,void***_tmp8C8,struct Cyc_List_List**_tmp3A1){for(*_tmp8CB=0;*_tmp8CB < *_tmp8CA;(*_tmp8CB)++){
# 1307 "new_control_flow.cyc"
void*_tmp8C6;(*_tmp8C8)[*_tmp8CB]=((_tmp8C6=(void*)((struct Cyc_List_List*)_check_null(*_tmp3A1))->hd,((*_tmp3A1=(*_tmp3A1)->tl,_tmp8C6))));}}
# 766 "new_control_flow.cyc"
static struct _tuple14 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 770
struct Cyc_CfFlowInfo_FlowEnv*_tmp1A2=env->fenv;
# 771
struct Cyc_Dict_Dict d;
# 772
struct Cyc_List_List*relns;
# 776
{union Cyc_CfFlowInfo_FlowInfo _tmp1A3=inflow;int _tmp1A4;struct _tuple13 _tmp1A5;struct Cyc_Dict_Dict _tmp1A6;struct Cyc_List_List*_tmp1A7;_LL110: if((_tmp1A3.BottomFL).tag != 1)goto _LL112;_tmp1A4=(int)(_tmp1A3.BottomFL).val;_LL111: {
# 777
struct _tuple14 _tmp82B;return(_tmp82B.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp82B.f2=_tmp1A2->unknown_all,_tmp82B)));}_LL112: if((_tmp1A3.ReachableFL).tag != 2)goto _LL10F;_tmp1A5=(struct _tuple13)(_tmp1A3.ReachableFL).val;_tmp1A6=_tmp1A5.f1;_tmp1A7=_tmp1A5.f2;_LL113:
# 778
 d=_tmp1A6;relns=_tmp1A7;_LL10F:;}
# 785
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e)) && 
# 786
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
# 806 "new_control_flow.cyc"
union Cyc_CfFlowInfo_FlowInfo _tmp1AA;union Cyc_CfFlowInfo_AbsLVal _tmp1AB;struct _tuple15 _tmp1A9=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,e);_tmp1AA=_tmp1A9.f1;_tmp1AB=_tmp1A9.f2;{
# 807
struct _tuple15 _tmp82C;struct _tuple15 _tmp1AD=(_tmp82C.f1=_tmp1AA,((_tmp82C.f2=_tmp1AB,_tmp82C)));union Cyc_CfFlowInfo_FlowInfo _tmp1AE;struct _tuple13 _tmp1AF;struct Cyc_Dict_Dict _tmp1B0;struct Cyc_List_List*_tmp1B1;union Cyc_CfFlowInfo_AbsLVal _tmp1B2;struct Cyc_CfFlowInfo_Place*_tmp1B3;_LL115: _tmp1AE=_tmp1AD.f1;if((_tmp1AE.ReachableFL).tag != 2)goto _LL117;_tmp1AF=(struct _tuple13)(_tmp1AE.ReachableFL).val;_tmp1B0=_tmp1AF.f1;_tmp1B1=_tmp1AF.f2;_tmp1B2=_tmp1AD.f2;if((_tmp1B2.PlaceL).tag != 1)goto _LL117;_tmp1B3=(struct Cyc_CfFlowInfo_Place*)(_tmp1B2.PlaceL).val;_LL116: {
# 809
void*_tmp1B4=Cyc_CfFlowInfo_lookup_place(_tmp1B0,_tmp1B3);
# 810
int needs_unconsume=0;
# 811
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp1B4,& needs_unconsume)){
# 812
{const char*_tmp82F;void*_tmp82E;(_tmp82E=0,Cyc_Tcutil_terr(e->loc,((_tmp82F="expression attempts to copy unique value that may be copied elsewhere.",_tag_dyneither(_tmp82F,sizeof(char),71))),_tag_dyneither(_tmp82E,sizeof(void*),0)));}{
# 813
struct _tuple14 _tmp830;return(_tmp830.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp830.f2=_tmp1A2->unknown_all,_tmp830)));};}else{
# 815
if(needs_unconsume){
# 817
struct _tuple14 _tmp831;return(_tmp831.f1=_tmp1AA,((_tmp831.f2=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp1A2,_tmp1B4),_tmp831)));}else{
# 820
void*_tmp1B9=Cyc_CfFlowInfo_make_unique_consumed(_tmp1A2,(void*)_check_null(e->topt),e,env->iteration_num,_tmp1B4);
# 821
struct Cyc_Dict_Dict _tmp1BA=Cyc_CfFlowInfo_assign_place(_tmp1A2,e->loc,_tmp1B0,env->all_changed,_tmp1B3,_tmp1B9);
# 831
struct _tuple14 _tmp832;return(_tmp832.f1=Cyc_CfFlowInfo_ReachableFL(_tmp1BA,_tmp1B1),((_tmp832.f2=_tmp1B4,_tmp832)));}}}_LL117:;_LL118:
# 833
 goto _LL114;_LL114:;};}{
# 836
void*_tmp1BC=e->r;struct Cyc_Absyn_Exp*_tmp1BE;enum Cyc_Absyn_Coercion _tmp1BF;struct Cyc_Absyn_Exp*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C3;struct Cyc_Absyn_Exp*_tmp1C5;union Cyc_Absyn_Cnst _tmp1C7;int _tmp1C8;union Cyc_Absyn_Cnst _tmp1CA;struct _tuple6 _tmp1CB;int _tmp1CC;union Cyc_Absyn_Cnst _tmp1CE;struct _tuple6 _tmp1CF;void*_tmp1D1;struct Cyc_List_List*_tmp1D4;void*_tmp1DC;void*_tmp1DF;struct Cyc_Absyn_Vardecl*_tmp1E1;void*_tmp1E3;struct Cyc_Absyn_Vardecl*_tmp1E5;void*_tmp1E7;struct Cyc_Absyn_Vardecl*_tmp1E9;enum Cyc_Absyn_Primop _tmp1EB;struct Cyc_List_List*_tmp1EC;struct Cyc_Absyn_Exp*_tmp1EE;struct Cyc_Absyn_Exp*_tmp1F0;struct Cyc_Core_Opt*_tmp1F1;struct Cyc_Core_Opt _tmp1F2;struct Cyc_Absyn_Exp*_tmp1F3;struct Cyc_Absyn_Exp*_tmp1F5;struct Cyc_Core_Opt*_tmp1F6;struct Cyc_Absyn_Exp*_tmp1F7;struct Cyc_Absyn_Exp*_tmp1F9;struct Cyc_Absyn_Exp*_tmp1FA;struct Cyc_Absyn_Exp*_tmp1FC;struct Cyc_Absyn_Exp*_tmp1FE;struct Cyc_List_List*_tmp1FF;struct Cyc_Absyn_MallocInfo _tmp201;int _tmp202;struct Cyc_Absyn_Exp*_tmp203;void**_tmp204;struct Cyc_Absyn_Exp*_tmp205;int _tmp206;struct Cyc_Absyn_Exp*_tmp208;struct Cyc_Absyn_Exp*_tmp209;struct Cyc_Absyn_Exp*_tmp20B;struct Cyc_Absyn_Exp*_tmp20C;struct Cyc_Absyn_Exp*_tmp20E;struct Cyc_Absyn_Exp*_tmp210;struct Cyc_Absyn_Exp*_tmp212;struct _dyneither_ptr*_tmp213;struct Cyc_Absyn_Exp*_tmp215;struct _dyneither_ptr*_tmp216;struct Cyc_Absyn_Exp*_tmp218;struct _dyneither_ptr*_tmp219;struct Cyc_Absyn_Exp*_tmp21B;struct Cyc_Absyn_Exp*_tmp21C;struct Cyc_Absyn_Exp*_tmp21D;struct Cyc_Absyn_Exp*_tmp21F;struct Cyc_Absyn_Exp*_tmp220;struct Cyc_Absyn_Exp*_tmp222;struct Cyc_Absyn_Exp*_tmp223;struct Cyc_Absyn_Exp*_tmp225;struct Cyc_Absyn_Exp*_tmp226;struct Cyc_List_List*_tmp228;struct Cyc_Absyn_Datatypedecl*_tmp229;struct Cyc_List_List*_tmp22B;struct Cyc_List_List*_tmp22D;struct Cyc_List_List*_tmp22F;struct Cyc_Absyn_Aggrdecl*_tmp230;struct Cyc_Absyn_Aggrdecl _tmp231;enum Cyc_Absyn_AggrKind _tmp232;struct Cyc_Absyn_AggrdeclImpl*_tmp233;struct Cyc_Absyn_AggrdeclImpl _tmp234;struct Cyc_List_List*_tmp235;struct Cyc_List_List*_tmp238;struct Cyc_Absyn_Vardecl*_tmp23A;struct Cyc_Absyn_Exp*_tmp23B;struct Cyc_Absyn_Exp*_tmp23C;int _tmp23D;struct Cyc_Absyn_Stmt*_tmp23F;void*_tmp241;_LL11A: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1BD=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1BD->tag != 13)goto _LL11C;else{_tmp1BE=_tmp1BD->f2;_tmp1BF=_tmp1BD->f4;if(_tmp1BF != Cyc_Absyn_NonNull_to_Null)goto _LL11C;}}_LL11B: {
# 840
union Cyc_CfFlowInfo_FlowInfo _tmp248;void*_tmp249;struct _tuple14 _tmp247=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp1BE);_tmp248=_tmp247.f1;_tmp249=_tmp247.f2;{
# 841
union Cyc_CfFlowInfo_FlowInfo _tmp24B;void*_tmp24C;struct _tuple14 _tmp24A=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp248,_tmp1BE,_tmp249);_tmp24B=_tmp24A.f1;_tmp24C=_tmp24A.f2;{
# 842
struct _tuple14 _tmp833;return(_tmp833.f1=_tmp24B,((_tmp833.f2=_tmp249,_tmp833)));};};}_LL11C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1C0=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1C0->tag != 13)goto _LL11E;else{_tmp1C1=_tmp1C0->f2;}}_LL11D:
# 846
 _tmp1C3=_tmp1C1;goto _LL11F;_LL11E: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp1C2=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1C2->tag != 11)goto _LL120;else{_tmp1C3=_tmp1C2->f1;}}_LL11F:
# 847
 _tmp1C5=_tmp1C3;goto _LL121;_LL120: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp1C4=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1C4->tag != 12)goto _LL122;else{_tmp1C5=_tmp1C4->f1;}}_LL121:
# 848
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp1C5);_LL122: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1C6=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1C6->tag != 0)goto _LL124;else{_tmp1C7=_tmp1C6->f1;if((_tmp1C7.Null_c).tag != 1)goto _LL124;_tmp1C8=(int)(_tmp1C7.Null_c).val;}}_LL123:
# 850
 goto _LL125;_LL124: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1C9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1C9->tag != 0)goto _LL126;else{_tmp1CA=_tmp1C9->f1;if((_tmp1CA.Int_c).tag != 5)goto _LL126;_tmp1CB=(struct _tuple6)(_tmp1CA.Int_c).val;_tmp1CC=_tmp1CB.f2;if(_tmp1CC != 0)goto _LL126;}}_LL125: {
# 851
struct _tuple14 _tmp834;return(_tmp834.f1=inflow,((_tmp834.f2=_tmp1A2->zero,_tmp834)));}_LL126: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1CD=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1CD->tag != 0)goto _LL128;else{_tmp1CE=_tmp1CD->f1;if((_tmp1CE.Int_c).tag != 5)goto _LL128;_tmp1CF=(struct _tuple6)(_tmp1CE.Int_c).val;}}_LL127:
# 852
 goto _LL129;_LL128: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1D0=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1D0->tag != 1)goto _LL12A;else{_tmp1D1=(void*)_tmp1D0->f2;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp1D2=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp1D1;if(_tmp1D2->tag != 2)goto _LL12A;};}}_LL129: {
# 853
struct _tuple14 _tmp835;return(_tmp835.f1=inflow,((_tmp835.f2=_tmp1A2->notzeroall,_tmp835)));}_LL12A: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1D3=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1D3->tag != 29)goto _LL12C;else{_tmp1D4=_tmp1D3->f1;if(_tmp1D4 != 0)goto _LL12C;}}_LL12B:
# 855
 goto _LL12D;_LL12C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1D5=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1D5->tag != 0)goto _LL12E;}_LL12D:
# 856
 goto _LL12F;_LL12E: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp1D6=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1D6->tag != 17)goto _LL130;}_LL12F:
# 857
 goto _LL131;_LL130: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp1D7=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1D7->tag != 16)goto _LL132;}_LL131:
# 858
 goto _LL133;_LL132: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp1D8=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1D8->tag != 18)goto _LL134;}_LL133:
# 859
 goto _LL135;_LL134: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp1D9=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1D9->tag != 31)goto _LL136;}_LL135:
# 860
 goto _LL137;_LL136: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp1DA=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1DA->tag != 30)goto _LL138;}_LL137: {
# 861
struct _tuple14 _tmp836;return(_tmp836.f1=inflow,((_tmp836.f2=_tmp1A2->unknown_all,_tmp836)));}_LL138: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1DB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1DB->tag != 1)goto _LL13A;else{_tmp1DC=(void*)_tmp1DB->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp1DD=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp1DC;if(_tmp1DD->tag != 1)goto _LL13A;};}}_LL139: {
# 864
struct _tuple14 _tmp837;return(_tmp837.f1=inflow,((_tmp837.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp1A2,(void*)_check_null(e->topt),0,_tmp1A2->unknown_all),_tmp837)));}_LL13A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1DE=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1DE->tag != 1)goto _LL13C;else{_tmp1DF=(void*)_tmp1DE->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp1E0=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp1DF;if(_tmp1E0->tag != 3)goto _LL13C;else{_tmp1E1=_tmp1E0->f1;}};}}_LL13B:
# 867
 _tmp1E5=_tmp1E1;goto _LL13D;_LL13C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1E2=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1E2->tag != 1)goto _LL13E;else{_tmp1E3=(void*)_tmp1E2->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp1E4=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp1E3;if(_tmp1E4->tag != 4)goto _LL13E;else{_tmp1E5=_tmp1E4->f1;}};}}_LL13D:
# 868
 _tmp1E9=_tmp1E5;goto _LL13F;_LL13E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1E6=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1E6->tag != 1)goto _LL140;else{_tmp1E7=(void*)_tmp1E6->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp1E8=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp1E7;if(_tmp1E8->tag != 5)goto _LL140;else{_tmp1E9=_tmp1E8->f1;}};}}_LL13F: {
# 871
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp838;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp838.tag=0,((_tmp838.f1=_tmp1E9,_tmp838)));
# 872
struct _tuple14 _tmp839;return(_tmp839.f1=inflow,((_tmp839.f2=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot),_tmp839)));}_LL140: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1EA=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1EA->tag != 2)goto _LL142;else{_tmp1EB=_tmp1EA->f1;_tmp1EC=_tmp1EA->f2;}}_LL141: {
# 878
union Cyc_CfFlowInfo_FlowInfo _tmp255;void*_tmp256;struct _tuple14 _tmp254=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp1EC);_tmp255=_tmp254.f1;_tmp256=_tmp254.f2;{
# 879
struct _tuple14 _tmp83A;return(_tmp83A.f1=_tmp255,((_tmp83A.f2=_tmp256,_tmp83A)));};}_LL142: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp1ED=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1ED->tag != 4)goto _LL144;else{_tmp1EE=_tmp1ED->f1;}}_LL143: {
# 882
struct Cyc_List_List _tmp83B;struct Cyc_List_List _tmp258=(_tmp83B.hd=_tmp1EE,((_tmp83B.tl=0,_tmp83B)));
# 883
union Cyc_CfFlowInfo_FlowInfo _tmp25A;struct _tuple14 _tmp259=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct Cyc_List_List*)& _tmp258);_tmp25A=_tmp259.f1;{
# 884
union Cyc_CfFlowInfo_AbsLVal _tmp25C;struct _tuple15 _tmp25B=Cyc_NewControlFlow_anal_Lexp(env,_tmp25A,0,_tmp1EE);_tmp25C=_tmp25B.f2;
# 885
{struct _tuple21 _tmp83C;struct _tuple21 _tmp25E=(_tmp83C.f1=_tmp25C,((_tmp83C.f2=_tmp25A,_tmp83C)));union Cyc_CfFlowInfo_AbsLVal _tmp25F;struct Cyc_CfFlowInfo_Place*_tmp260;union Cyc_CfFlowInfo_FlowInfo _tmp261;struct _tuple13 _tmp262;struct Cyc_Dict_Dict _tmp263;struct Cyc_List_List*_tmp264;_LL181: _tmp25F=_tmp25E.f1;if((_tmp25F.PlaceL).tag != 1)goto _LL183;_tmp260=(struct Cyc_CfFlowInfo_Place*)(_tmp25F.PlaceL).val;_tmp261=_tmp25E.f2;if((_tmp261.ReachableFL).tag != 2)goto _LL183;_tmp262=(struct _tuple13)(_tmp261.ReachableFL).val;_tmp263=_tmp262.f1;_tmp264=_tmp262.f2;_LL182:
# 887
 _tmp264=Cyc_CfFlowInfo_reln_kill_exp(_tmp1A2->r,_tmp264,_tmp1EE);
# 888
_tmp25A=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp1A2,_tmp1EE->loc,_tmp263,env->all_changed,_tmp260,_tmp1A2->unknown_all),_tmp264);
# 891
goto _LL180;_LL183:;_LL184:
# 892
 goto _LL180;_LL180:;}{
# 894
struct _tuple14 _tmp83D;return(_tmp83D.f1=_tmp25A,((_tmp83D.f2=_tmp1A2->unknown_all,_tmp83D)));};};}_LL144: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp1EF=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1EF->tag != 3)goto _LL146;else{_tmp1F0=_tmp1EF->f1;_tmp1F1=_tmp1EF->f2;if(_tmp1F1 == 0)goto _LL146;_tmp1F2=*_tmp1F1;_tmp1F3=_tmp1EF->f3;}}_LL145:
# 897
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
# 898
{const char*_tmp840;void*_tmp83F;(_tmp83F=0,Cyc_Tcutil_terr(e->loc,((_tmp840="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp840,sizeof(char),58))),_tag_dyneither(_tmp83F,sizeof(void*),0)));}{
# 899
struct _tuple14 _tmp841;return(_tmp841.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp841.f2=_tmp1A2->unknown_all,_tmp841)));};}{
# 901
struct Cyc_List_List _tmp842;struct Cyc_List_List _tmp26A=(_tmp842.hd=_tmp1F3,((_tmp842.tl=0,_tmp842)));
# 902
struct Cyc_List_List _tmp843;struct Cyc_List_List _tmp26B=(_tmp843.hd=_tmp1F0,((_tmp843.tl=(struct Cyc_List_List*)& _tmp26A,_tmp843)));
# 903
union Cyc_CfFlowInfo_FlowInfo _tmp26D;struct _tuple14 _tmp26C=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct Cyc_List_List*)& _tmp26B);_tmp26D=_tmp26C.f1;{
# 904
union Cyc_CfFlowInfo_AbsLVal _tmp26F;struct _tuple15 _tmp26E=Cyc_NewControlFlow_anal_Lexp(env,_tmp26D,0,_tmp1F0);_tmp26F=_tmp26E.f2;
# 905
{union Cyc_CfFlowInfo_FlowInfo _tmp270=_tmp26D;struct _tuple13 _tmp271;struct Cyc_Dict_Dict _tmp272;struct Cyc_List_List*_tmp273;_LL186: if((_tmp270.ReachableFL).tag != 2)goto _LL188;_tmp271=(struct _tuple13)(_tmp270.ReachableFL).val;_tmp272=_tmp271.f1;_tmp273=_tmp271.f2;_LL187:
# 907
{union Cyc_CfFlowInfo_AbsLVal _tmp274=_tmp26F;struct Cyc_CfFlowInfo_Place*_tmp275;int _tmp276;_LL18B: if((_tmp274.PlaceL).tag != 1)goto _LL18D;_tmp275=(struct Cyc_CfFlowInfo_Place*)(_tmp274.PlaceL).val;_LL18C:
# 911
 _tmp273=Cyc_CfFlowInfo_reln_kill_exp(_tmp1A2->r,_tmp273,_tmp1F0);
# 912
_tmp272=Cyc_CfFlowInfo_assign_place(_tmp1A2,_tmp1F0->loc,_tmp272,env->all_changed,_tmp275,_tmp1A2->unknown_all);
# 914
_tmp26D=Cyc_CfFlowInfo_ReachableFL(_tmp272,_tmp273);
# 917
goto _LL18A;_LL18D: if((_tmp274.UnknownL).tag != 2)goto _LL18A;_tmp276=(int)(_tmp274.UnknownL).val;_LL18E:
# 920
 goto _LL18A;_LL18A:;}
# 922
goto _LL185;_LL188:;_LL189:
# 923
 goto _LL185;_LL185:;}{
# 925
struct _tuple14 _tmp844;return(_tmp844.f1=_tmp26D,((_tmp844.f2=_tmp1A2->unknown_all,_tmp844)));};};};_LL146: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp1F4=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1F4->tag != 3)goto _LL148;else{_tmp1F5=_tmp1F4->f1;_tmp1F6=_tmp1F4->f2;if(_tmp1F6 != 0)goto _LL148;_tmp1F7=_tmp1F4->f3;}}_LL147:
# 929
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
# 930
{const char*_tmp847;void*_tmp846;(_tmp846=0,Cyc_Tcutil_terr(e->loc,((_tmp847="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp847,sizeof(char),58))),_tag_dyneither(_tmp846,sizeof(void*),0)));}{
# 931
struct _tuple14 _tmp848;return(_tmp848.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp848.f2=_tmp1A2->unknown_all,_tmp848)));};}{
# 933
struct Cyc_Dict_Dict*_tmp27D=env->all_changed;
# 934
while(1){
# 935
{struct Cyc_Dict_Dict*_tmp849;env->all_changed=((_tmp849=_region_malloc(env->r,sizeof(*_tmp849)),((_tmp849[0]=_tmp1A2->mt_place_set,_tmp849))));}{
# 936
union Cyc_CfFlowInfo_FlowInfo _tmp280;union Cyc_CfFlowInfo_AbsLVal _tmp281;struct _tuple15 _tmp27F=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp1F5);_tmp280=_tmp27F.f1;_tmp281=_tmp27F.f2;{
# 937
struct Cyc_Dict_Dict _tmp282=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
# 938
{struct Cyc_Dict_Dict*_tmp84A;env->all_changed=((_tmp84A=_region_malloc(env->r,sizeof(*_tmp84A)),((_tmp84A[0]=_tmp1A2->mt_place_set,_tmp84A))));}{
# 939
union Cyc_CfFlowInfo_FlowInfo _tmp285;void*_tmp286;struct _tuple14 _tmp284=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1F7);_tmp285=_tmp284.f1;_tmp286=_tmp284.f2;{
# 940
struct Cyc_Dict_Dict _tmp287=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
# 941
union Cyc_CfFlowInfo_FlowInfo _tmp288=Cyc_CfFlowInfo_after_flow(_tmp1A2,(struct Cyc_Dict_Dict*)& _tmp282,_tmp280,_tmp285,_tmp282,_tmp287);
# 944
union Cyc_CfFlowInfo_FlowInfo _tmp289=Cyc_CfFlowInfo_join_flow(_tmp1A2,_tmp27D,inflow,_tmp288);
# 947
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp289,inflow)){
# 948
if(_tmp27D == 0)
# 949
env->all_changed=0;else{
# 951
*((struct Cyc_Dict_Dict*)_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp27D,
# 952
Cyc_CfFlowInfo_union_place_set(_tmp282,_tmp287,0),0);}
# 954
return Cyc_NewControlFlow_do_assign(_tmp1A2,env,_tmp288,_tmp1F5,_tmp281,_tmp1F7,_tmp286,e->loc);}
# 957
inflow=_tmp289;};};};};}};_LL148: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp1F8=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1F8->tag != 8)goto _LL14A;else{_tmp1F9=_tmp1F8->f1;_tmp1FA=_tmp1F8->f2;}}_LL149: {
# 962
union Cyc_CfFlowInfo_FlowInfo _tmp28B;void*_tmp28C;struct _tuple14 _tmp28A=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1F9);_tmp28B=_tmp28A.f1;_tmp28C=_tmp28A.f2;
# 963
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp28B,_tmp1FA);}_LL14A: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp1FB=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1FB->tag != 10)goto _LL14C;else{_tmp1FC=_tmp1FB->f1;}}_LL14B: {
# 966
union Cyc_CfFlowInfo_FlowInfo _tmp28E;void*_tmp28F;struct _tuple14 _tmp28D=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1FC);_tmp28E=_tmp28D.f1;_tmp28F=_tmp28D.f2;
# 967
Cyc_NewControlFlow_use_Rval(env,_tmp1FC->loc,_tmp28E,_tmp28F);{
# 968
struct _tuple14 _tmp84B;return(_tmp84B.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp84B.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp1A2,(void*)_check_null(e->topt),0,_tmp1A2->unknown_all),_tmp84B)));};}_LL14C: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp1FD=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp1FD->tag != 9)goto _LL14E;else{_tmp1FE=_tmp1FD->f1;_tmp1FF=_tmp1FD->f2;}}_LL14D: {
# 971
struct _RegionHandle _tmp291=_new_region("temp");struct _RegionHandle*temp=& _tmp291;_push_region(temp);
# 972
{union Cyc_CfFlowInfo_FlowInfo _tmp294;struct Cyc_List_List*_tmp295;struct Cyc_List_List*_tmp84C;struct _tuple20 _tmp293=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((_tmp84C=_region_malloc(temp,sizeof(*_tmp84C)),((_tmp84C->hd=_tmp1FE,((_tmp84C->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp1FF),_tmp84C)))))),0,1);_tmp294=_tmp293.f1;_tmp295=_tmp293.f2;{
# 974
union Cyc_CfFlowInfo_FlowInfo _tmp296=Cyc_NewControlFlow_use_Rval(env,_tmp1FE->loc,_tmp294,(void*)((struct Cyc_List_List*)_check_null(_tmp295))->hd);
# 975
_tmp295=_tmp295->tl;{
# 977
struct Cyc_List_List*init_params=0;
# 978
struct Cyc_List_List*nolive_unique_params=0;
# 979
struct Cyc_List_List*noconsume_params=0;
# 980
{void*_tmp297=Cyc_Tcutil_compress((void*)_check_null(_tmp1FE->topt));struct Cyc_Absyn_PtrInfo _tmp299;void*_tmp29A;_LL190: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp298=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp297;if(_tmp298->tag != 5)goto _LL192;else{_tmp299=_tmp298->f1;_tmp29A=_tmp299.elt_typ;}}_LL191:
# 982
{void*_tmp29B=Cyc_Tcutil_compress(_tmp29A);struct Cyc_Absyn_FnInfo _tmp29D;struct Cyc_List_List*_tmp29E;_LL195: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp29C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp29B;if(_tmp29C->tag != 9)goto _LL197;else{_tmp29D=_tmp29C->f1;_tmp29E=_tmp29D.attributes;}}_LL196:
# 984
 for(0;_tmp29E != 0;_tmp29E=_tmp29E->tl){
# 986
void*_tmp29F=(void*)_tmp29E->hd;int _tmp2A1;int _tmp2A3;int _tmp2A5;_LL19A: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2A0=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp29F;if(_tmp2A0->tag != 20)goto _LL19C;else{_tmp2A1=_tmp2A0->f1;}}_LL19B:
# 988
{struct Cyc_List_List*_tmp84D;init_params=((_tmp84D=_region_malloc(temp,sizeof(*_tmp84D)),((_tmp84D->hd=(void*)_tmp2A1,((_tmp84D->tl=init_params,_tmp84D))))));}goto _LL199;_LL19C: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp2A2=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp29F;if(_tmp2A2->tag != 21)goto _LL19E;else{_tmp2A3=_tmp2A2->f1;}}_LL19D:
# 990
{struct Cyc_List_List*_tmp84E;nolive_unique_params=((_tmp84E=_region_malloc(temp,sizeof(*_tmp84E)),((_tmp84E->hd=(void*)_tmp2A3,((_tmp84E->tl=nolive_unique_params,_tmp84E))))));}
# 991
goto _LL199;_LL19E: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp2A4=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp29F;if(_tmp2A4->tag != 22)goto _LL1A0;else{_tmp2A5=_tmp2A4->f1;}}_LL19F:
# 994
{struct Cyc_List_List*_tmp84F;noconsume_params=((_tmp84F=_region_malloc(temp,sizeof(*_tmp84F)),((_tmp84F->hd=(void*)_tmp2A5,((_tmp84F->tl=noconsume_params,_tmp84F))))));}
# 995
goto _LL199;_LL1A0:;_LL1A1:
# 996
 goto _LL199;_LL199:;}
# 998
goto _LL194;_LL197:;_LL198: {
# 999
const char*_tmp852;void*_tmp851;(_tmp851=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp852="anal_Rexp: bad function type",_tag_dyneither(_tmp852,sizeof(char),29))),_tag_dyneither(_tmp851,sizeof(void*),0)));}_LL194:;}
# 1001
goto _LL18F;_LL192:;_LL193: {
# 1002
const char*_tmp855;void*_tmp854;(_tmp854=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp855="anal_Rexp: bad function type",_tag_dyneither(_tmp855,sizeof(char),29))),_tag_dyneither(_tmp854,sizeof(void*),0)));}_LL18F:;}
# 1004
{int i=1;for(0;_tmp295 != 0;(((_tmp295=_tmp295->tl,_tmp1FF=((struct Cyc_List_List*)_check_null(_tmp1FF))->tl)),++ i)){
# 1005
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
# 1006
union Cyc_CfFlowInfo_FlowInfo _tmp2AD=_tmp294;int _tmp2AE;struct _tuple13 _tmp2AF;struct Cyc_Dict_Dict _tmp2B0;_LL1A3: if((_tmp2AD.BottomFL).tag != 1)goto _LL1A5;_tmp2AE=(int)(_tmp2AD.BottomFL).val;_LL1A4:
# 1007
 goto _LL1A2;_LL1A5: if((_tmp2AD.ReachableFL).tag != 2)goto _LL1A2;_tmp2AF=(struct _tuple13)(_tmp2AD.ReachableFL).val;_tmp2B0=_tmp2AF.f1;_LL1A6:
# 1009
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2B0,(void*)_tmp295->hd)== Cyc_CfFlowInfo_NoneIL){
# 1010
const char*_tmp858;void*_tmp857;(_tmp857=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1FF))->hd)->loc,((_tmp858="expression may not be initialized",_tag_dyneither(_tmp858,sizeof(char),34))),_tag_dyneither(_tmp857,sizeof(void*),0)));}
# 1011
{union Cyc_CfFlowInfo_FlowInfo _tmp2B3=_tmp296;int _tmp2B4;struct _tuple13 _tmp2B5;struct Cyc_Dict_Dict _tmp2B6;struct Cyc_List_List*_tmp2B7;_LL1A8: if((_tmp2B3.BottomFL).tag != 1)goto _LL1AA;_tmp2B4=(int)(_tmp2B3.BottomFL).val;_LL1A9:
# 1012
 goto _LL1A7;_LL1AA: if((_tmp2B3.ReachableFL).tag != 2)goto _LL1A7;_tmp2B5=(struct _tuple13)(_tmp2B3.ReachableFL).val;_tmp2B6=_tmp2B5.f1;_tmp2B7=_tmp2B5.f2;_LL1AB: {
# 1016
struct Cyc_Dict_Dict _tmp2B8=Cyc_CfFlowInfo_escape_deref(_tmp1A2,_tmp2B6,env->all_changed,(void*)_tmp295->hd);
# 1017
{void*_tmp2B9=(void*)_tmp295->hd;struct Cyc_CfFlowInfo_Place*_tmp2BB;_LL1AD: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2BA=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2B9;if(_tmp2BA->tag != 5)goto _LL1AF;else{_tmp2BB=_tmp2BA->f1;}}_LL1AE:
# 1019
{void*_tmp2BC=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1FF))->hd)->topt));struct Cyc_Absyn_PtrInfo _tmp2BE;void*_tmp2BF;_LL1B2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2BD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BC;if(_tmp2BD->tag != 5)goto _LL1B4;else{_tmp2BE=_tmp2BD->f1;_tmp2BF=_tmp2BE.elt_typ;}}_LL1B3:
# 1021
 _tmp2B8=Cyc_CfFlowInfo_assign_place(_tmp1A2,((struct Cyc_Absyn_Exp*)_tmp1FF->hd)->loc,_tmp2B8,env->all_changed,_tmp2BB,
# 1023
Cyc_CfFlowInfo_typ_to_absrval(_tmp1A2,_tmp2BF,0,_tmp1A2->esc_all));
# 1025
goto _LL1B1;_LL1B4:;_LL1B5: {
# 1026
const char*_tmp85B;void*_tmp85A;(_tmp85A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp85B="anal_Rexp:bad type for initialized arg",_tag_dyneither(_tmp85B,sizeof(char),39))),_tag_dyneither(_tmp85A,sizeof(void*),0)));}_LL1B1:;}
# 1028
goto _LL1AC;_LL1AF:;_LL1B0:
# 1029
 goto _LL1AC;_LL1AC:;}
# 1031
_tmp296=Cyc_CfFlowInfo_ReachableFL(_tmp2B8,_tmp2B7);
# 1032
goto _LL1A7;}_LL1A7:;}
# 1034
goto _LL1A2;_LL1A2:;}else{
# 1037
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1040
_tmp296=Cyc_NewControlFlow_use_nounique_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1FF))->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1FF))->hd)->topt),_tmp296,(void*)_tmp295->hd);else{
# 1047
_tmp296=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1FF))->hd)->loc,_tmp296,(void*)_tmp295->hd);
# 1048
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i)){
# 1050
union Cyc_CfFlowInfo_AbsLVal _tmp2C3;struct _tuple15 _tmp2C2=Cyc_NewControlFlow_anal_Lexp(env,_tmp296,1,(struct Cyc_Absyn_Exp*)_tmp1FF->hd);_tmp2C3=_tmp2C2.f2;{
# 1051
struct _tuple15 _tmp85C;struct _tuple15 _tmp2C5=(_tmp85C.f1=_tmp296,((_tmp85C.f2=_tmp2C3,_tmp85C)));union Cyc_CfFlowInfo_FlowInfo _tmp2C6;struct _tuple13 _tmp2C7;struct Cyc_Dict_Dict _tmp2C8;struct Cyc_List_List*_tmp2C9;union Cyc_CfFlowInfo_AbsLVal _tmp2CA;struct Cyc_CfFlowInfo_Place*_tmp2CB;union Cyc_CfFlowInfo_FlowInfo _tmp2CC;int _tmp2CD;_LL1B7: _tmp2C6=_tmp2C5.f1;if((_tmp2C6.ReachableFL).tag != 2)goto _LL1B9;_tmp2C7=(struct _tuple13)(_tmp2C6.ReachableFL).val;_tmp2C8=_tmp2C7.f1;_tmp2C9=_tmp2C7.f2;_tmp2CA=_tmp2C5.f2;if((_tmp2CA.PlaceL).tag != 1)goto _LL1B9;_tmp2CB=(struct Cyc_CfFlowInfo_Place*)(_tmp2CA.PlaceL).val;_LL1B8: {
# 1053
void*_tmp2CE=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp1A2,(void*)_tmp295->hd);
# 1056
_tmp2C8=Cyc_CfFlowInfo_assign_place(_tmp1A2,((struct Cyc_Absyn_Exp*)_tmp1FF->hd)->loc,_tmp2C8,env->all_changed,_tmp2CB,_tmp2CE);
# 1057
_tmp296=Cyc_CfFlowInfo_ReachableFL(_tmp2C8,_tmp2C9);
# 1061
goto _LL1B6;}_LL1B9: _tmp2CC=_tmp2C5.f1;if((_tmp2CC.BottomFL).tag != 1)goto _LL1BB;_tmp2CD=(int)(_tmp2CC.BottomFL).val;_LL1BA:
# 1062
 continue;_LL1BB:;_LL1BC: {
# 1064
const char*_tmp85F;void*_tmp85E;(_tmp85E=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp1FF->hd)->loc,((_tmp85F="noconsume() parameters must be l-values",_tag_dyneither(_tmp85F,sizeof(char),40))),_tag_dyneither(_tmp85E,sizeof(void*),0)));}_LL1B6:;};}}}}}
# 1071
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp1FE->topt))){
# 1072
struct _tuple14 _tmp860;struct _tuple14 _tmp2D2=(_tmp860.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp860.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp1A2,(void*)_check_null(e->topt),0,_tmp1A2->unknown_all),_tmp860)));_npop_handler(0);return _tmp2D2;}else{
# 1074
struct _tuple14 _tmp861;struct _tuple14 _tmp2D4=(_tmp861.f1=_tmp296,((_tmp861.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp1A2,(void*)_check_null(e->topt),0,_tmp1A2->unknown_all),_tmp861)));_npop_handler(0);return _tmp2D4;}};};}
# 972
;_pop_region(temp);}_LL14E: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp200=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp200->tag != 32)goto _LL150;else{_tmp201=_tmp200->f1;_tmp202=_tmp201.is_calloc;_tmp203=_tmp201.rgn;_tmp204=_tmp201.elt_type;_tmp205=_tmp201.num_elts;_tmp206=_tmp201.fat_result;}}_LL14F: {
# 1077
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp864;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp863;void*root=(void*)((_tmp863=_region_malloc(_tmp1A2->r,sizeof(*_tmp863)),((_tmp863[0]=((_tmp864.tag=1,((_tmp864.f1=_tmp205,((_tmp864.f2=(void*)((void*)_check_null(e->topt)),_tmp864)))))),_tmp863))));
# 1078
struct Cyc_CfFlowInfo_Place*_tmp865;struct Cyc_CfFlowInfo_Place*place=(_tmp865=_region_malloc(_tmp1A2->r,sizeof(*_tmp865)),((_tmp865->root=root,((_tmp865->fields=0,_tmp865)))));
# 1079
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp868;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp867;void*rval=(void*)((_tmp867=_region_malloc(_tmp1A2->r,sizeof(*_tmp867)),((_tmp867[0]=((_tmp868.tag=5,((_tmp868.f1=place,_tmp868)))),_tmp867))));
# 1080
void*place_val;
# 1081
if(_tmp206)place_val=_tmp1A2->notzeroall;else{
# 1082
if(_tmp202)place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp1A2,*((void**)_check_null(_tmp204)),0,_tmp1A2->zero);else{
# 1083
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp1A2,*((void**)_check_null(_tmp204)),0,_tmp1A2->unknown_none);}}{
# 1084
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1085
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
# 1086
if(_tmp203 != 0){
# 1087
struct _RegionHandle _tmp2D5=_new_region("temp");struct _RegionHandle*temp=& _tmp2D5;_push_region(temp);
# 1088
{union Cyc_CfFlowInfo_FlowInfo _tmp2D8;struct Cyc_List_List*_tmp2D9;struct Cyc_Absyn_Exp*_tmp869[2];struct _tuple20 _tmp2D7=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
# 1089
(_tmp869[1]=_tmp205,((_tmp869[0]=(struct Cyc_Absyn_Exp*)_tmp203,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp869,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp2D8=_tmp2D7.f1;_tmp2D9=_tmp2D7.f2;
# 1090
outflow=_tmp2D8;}
# 1088
;_pop_region(temp);}else{
# 1093
union Cyc_CfFlowInfo_FlowInfo _tmp2DB;struct _tuple14 _tmp2DA=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp205);_tmp2DB=_tmp2DA.f1;
# 1094
outflow=_tmp2DB;}{
# 1096
union Cyc_CfFlowInfo_FlowInfo _tmp2DC=outflow;int _tmp2DD;struct _tuple13 _tmp2DE;struct Cyc_Dict_Dict _tmp2DF;struct Cyc_List_List*_tmp2E0;_LL1BE: if((_tmp2DC.BottomFL).tag != 1)goto _LL1C0;_tmp2DD=(int)(_tmp2DC.BottomFL).val;_LL1BF: {
# 1097
struct _tuple14 _tmp86A;return(_tmp86A.f1=outflow,((_tmp86A.f2=rval,_tmp86A)));}_LL1C0: if((_tmp2DC.ReachableFL).tag != 2)goto _LL1BD;_tmp2DE=(struct _tuple13)(_tmp2DC.ReachableFL).val;_tmp2DF=_tmp2DE.f1;_tmp2E0=_tmp2DE.f2;_LL1C1: {
# 1099
struct _tuple14 _tmp86B;return(_tmp86B.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2DF,root,place_val),_tmp2E0),((_tmp86B.f2=rval,_tmp86B)));}_LL1BD:;};};}_LL150: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp207=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp207->tag != 33)goto _LL152;else{_tmp208=_tmp207->f1;_tmp209=_tmp207->f2;}}_LL151: {
# 1103
void*left_rval;
# 1104
void*right_rval;
# 1105
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1109
struct _RegionHandle _tmp2E8=_new_region("temp");struct _RegionHandle*temp=& _tmp2E8;_push_region(temp);{
# 1110
union Cyc_CfFlowInfo_FlowInfo _tmp2EB;struct Cyc_List_List*_tmp2EC;struct Cyc_Absyn_Exp*_tmp86C[2];struct _tuple20 _tmp2EA=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
# 1111
(_tmp86C[1]=_tmp209,((_tmp86C[0]=_tmp208,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp86C,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);_tmp2EB=_tmp2EA.f1;_tmp2EC=_tmp2EA.f2;
# 1112
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2EC))->hd;
# 1113
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2EC->tl))->hd;
# 1114
outflow=_tmp2EB;}{
# 1117
union Cyc_CfFlowInfo_AbsLVal _tmp2EE;struct _tuple15 _tmp2ED=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp208);_tmp2EE=_tmp2ED.f2;{
# 1118
union Cyc_CfFlowInfo_AbsLVal _tmp2F0;struct _tuple15 _tmp2EF=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,_tmp209);_tmp2F0=_tmp2EF.f2;
# 1119
{union Cyc_CfFlowInfo_FlowInfo _tmp2F1=outflow;struct _tuple13 _tmp2F2;struct Cyc_Dict_Dict _tmp2F3;struct Cyc_List_List*_tmp2F4;_LL1C3: if((_tmp2F1.ReachableFL).tag != 2)goto _LL1C5;_tmp2F2=(struct _tuple13)(_tmp2F1.ReachableFL).val;_tmp2F3=_tmp2F2.f1;_tmp2F4=_tmp2F2.f2;_LL1C4:
# 1121
{union Cyc_CfFlowInfo_AbsLVal _tmp2F5=_tmp2EE;struct Cyc_CfFlowInfo_Place*_tmp2F6;int _tmp2F7;_LL1C8: if((_tmp2F5.PlaceL).tag != 1)goto _LL1CA;_tmp2F6=(struct Cyc_CfFlowInfo_Place*)(_tmp2F5.PlaceL).val;_LL1C9:
# 1123
 _tmp2F3=Cyc_CfFlowInfo_assign_place(_tmp1A2,_tmp208->loc,_tmp2F3,env->all_changed,_tmp2F6,right_rval);
# 1125
goto _LL1C7;_LL1CA: if((_tmp2F5.UnknownL).tag != 2)goto _LL1C7;_tmp2F7=(int)(_tmp2F5.UnknownL).val;_LL1CB:
# 1126
 goto _LL1C7;_LL1C7:;}
# 1128
{union Cyc_CfFlowInfo_AbsLVal _tmp2F8=_tmp2F0;struct Cyc_CfFlowInfo_Place*_tmp2F9;int _tmp2FA;_LL1CD: if((_tmp2F8.PlaceL).tag != 1)goto _LL1CF;_tmp2F9=(struct Cyc_CfFlowInfo_Place*)(_tmp2F8.PlaceL).val;_LL1CE:
# 1130
 _tmp2F3=Cyc_CfFlowInfo_assign_place(_tmp1A2,_tmp209->loc,_tmp2F3,env->all_changed,_tmp2F9,left_rval);
# 1132
goto _LL1CC;_LL1CF: if((_tmp2F8.UnknownL).tag != 2)goto _LL1CC;_tmp2FA=(int)(_tmp2F8.UnknownL).val;_LL1D0:
# 1133
 goto _LL1CC;_LL1CC:;}
# 1136
_tmp2F4=Cyc_CfFlowInfo_reln_kill_exp(_tmp1A2->r,_tmp2F4,_tmp208);
# 1137
_tmp2F4=Cyc_CfFlowInfo_reln_kill_exp(_tmp1A2->r,_tmp2F4,_tmp209);
# 1139
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp2F3,_tmp2F4);
# 1140
goto _LL1C2;_LL1C5:;_LL1C6:
# 1141
 goto _LL1C2;_LL1C2:;}{
# 1145
struct _tuple14 _tmp86D;struct _tuple14 _tmp2FC=(_tmp86D.f1=outflow,((_tmp86D.f2=_tmp1A2->unknown_all,_tmp86D)));_npop_handler(0);return _tmp2FC;};};};
# 1109
;_pop_region(temp);}_LL152: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp20A=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp20A->tag != 15)goto _LL154;else{_tmp20B=_tmp20A->f1;_tmp20C=_tmp20A->f2;}}_LL153: {
# 1148
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp870;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp86F;void*root=(void*)((_tmp86F=_region_malloc(_tmp1A2->r,sizeof(*_tmp86F)),((_tmp86F[0]=((_tmp870.tag=1,((_tmp870.f1=_tmp20C,((_tmp870.f2=(void*)((void*)_check_null(e->topt)),_tmp870)))))),_tmp86F))));
# 1149
struct Cyc_CfFlowInfo_Place*_tmp871;struct Cyc_CfFlowInfo_Place*place=(_tmp871=_region_malloc(_tmp1A2->r,sizeof(*_tmp871)),((_tmp871->root=root,((_tmp871->fields=0,_tmp871)))));
# 1150
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp874;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp873;void*rval=(void*)((_tmp873=_region_malloc(_tmp1A2->r,sizeof(*_tmp873)),((_tmp873[0]=((_tmp874.tag=5,((_tmp874.f1=place,_tmp874)))),_tmp873))));
# 1151
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
# 1152
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1153
void*place_val;
# 1154
if(_tmp20B != 0){
# 1155
struct _RegionHandle _tmp2FD=_new_region("temp");struct _RegionHandle*temp=& _tmp2FD;_push_region(temp);
# 1156
{union Cyc_CfFlowInfo_FlowInfo _tmp300;struct Cyc_List_List*_tmp301;struct Cyc_Absyn_Exp*_tmp875[2];struct _tuple20 _tmp2FF=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
# 1157
(_tmp875[1]=_tmp20C,((_tmp875[0]=(struct Cyc_Absyn_Exp*)_tmp20B,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp875,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);_tmp300=_tmp2FF.f1;_tmp301=_tmp2FF.f2;
# 1158
outflow=_tmp300;
# 1159
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp301))->tl))->hd;}
# 1156
;_pop_region(temp);}else{
# 1162
union Cyc_CfFlowInfo_FlowInfo _tmp303;void*_tmp304;struct _tuple14 _tmp302=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp20C);_tmp303=_tmp302.f1;_tmp304=_tmp302.f2;
# 1163
outflow=_tmp303;
# 1164
place_val=_tmp304;}{
# 1166
union Cyc_CfFlowInfo_FlowInfo _tmp305=outflow;int _tmp306;struct _tuple13 _tmp307;struct Cyc_Dict_Dict _tmp308;struct Cyc_List_List*_tmp309;_LL1D2: if((_tmp305.BottomFL).tag != 1)goto _LL1D4;_tmp306=(int)(_tmp305.BottomFL).val;_LL1D3: {
# 1167
struct _tuple14 _tmp876;return(_tmp876.f1=outflow,((_tmp876.f2=rval,_tmp876)));}_LL1D4: if((_tmp305.ReachableFL).tag != 2)goto _LL1D1;_tmp307=(struct _tuple13)(_tmp305.ReachableFL).val;_tmp308=_tmp307.f1;_tmp309=_tmp307.f2;_LL1D5: {
# 1169
struct _tuple14 _tmp877;return(_tmp877.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp308,root,place_val),_tmp309),((_tmp877.f2=rval,_tmp877)));}_LL1D1:;};};}_LL154: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp20D=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp20D->tag != 14)goto _LL156;else{_tmp20E=_tmp20D->f1;}}_LL155: {
# 1173
union Cyc_CfFlowInfo_FlowInfo _tmp312;union Cyc_CfFlowInfo_AbsLVal _tmp313;struct _tuple15 _tmp311=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,_tmp20E);_tmp312=_tmp311.f1;_tmp313=_tmp311.f2;{
# 1174
union Cyc_CfFlowInfo_AbsLVal _tmp314=_tmp313;int _tmp315;struct Cyc_CfFlowInfo_Place*_tmp316;_LL1D7: if((_tmp314.UnknownL).tag != 2)goto _LL1D9;_tmp315=(int)(_tmp314.UnknownL).val;_LL1D8: {
# 1175
struct _tuple14 _tmp878;return(_tmp878.f1=_tmp312,((_tmp878.f2=_tmp1A2->notzeroall,_tmp878)));}_LL1D9: if((_tmp314.PlaceL).tag != 1)goto _LL1D6;_tmp316=(struct Cyc_CfFlowInfo_Place*)(_tmp314.PlaceL).val;_LL1DA: {
# 1176
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp87E;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp87D;struct _tuple14 _tmp87C;return(_tmp87C.f1=_tmp312,((_tmp87C.f2=(void*)((_tmp87E=_region_malloc(env->r,sizeof(*_tmp87E)),((_tmp87E[0]=((_tmp87D.tag=5,((_tmp87D.f1=_tmp316,_tmp87D)))),_tmp87E)))),_tmp87C)));}_LL1D6:;};}_LL156: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp20F=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp20F->tag != 19)goto _LL158;else{_tmp210=_tmp20F->f1;}}_LL157: {
# 1180
union Cyc_CfFlowInfo_FlowInfo _tmp31C;void*_tmp31D;struct _tuple14 _tmp31B=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp210);_tmp31C=_tmp31B.f1;_tmp31D=_tmp31B.f2;
# 1181
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp31C,_tmp210,_tmp31D);}_LL158: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp211=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp211->tag != 20)goto _LL15A;else{_tmp212=_tmp211->f1;_tmp213=_tmp211->f2;}}_LL159: {
# 1185
union Cyc_CfFlowInfo_FlowInfo _tmp31F;void*_tmp320;struct _tuple14 _tmp31E=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp212);_tmp31F=_tmp31E.f1;_tmp320=_tmp31E.f2;
# 1186
if(Cyc_Absyn_is_nontagged_union_type((void*)_check_null(_tmp212->topt))){
# 1187
struct _tuple14 _tmp87F;return(_tmp87F.f1=_tmp31F,((_tmp87F.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp1A2,(void*)_check_null(e->topt),0,_tmp1A2->unknown_all),_tmp87F)));}{
# 1188
void*_tmp322=_tmp320;struct Cyc_CfFlowInfo_UnionRInfo _tmp324;int _tmp325;int _tmp326;struct _dyneither_ptr _tmp327;_LL1DC: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp323=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp322;if(_tmp323->tag != 6)goto _LL1DE;else{_tmp324=_tmp323->f1;_tmp325=_tmp324.is_union;_tmp326=_tmp324.fieldnum;_tmp327=_tmp323->f2;}}_LL1DD: {
# 1190
int _tmp328=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp212->topt),_tmp213);
# 1191
if((_tmp325  && _tmp326 != - 1) && _tmp326 != _tmp328){
# 1192
struct _tuple14 _tmp880;return(_tmp880.f1=_tmp31F,((_tmp880.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp1A2,(void*)_check_null(e->topt),1,_tmp1A2->unknown_none),_tmp880)));}{
# 1193
struct _tuple14 _tmp881;return(_tmp881.f1=_tmp31F,((_tmp881.f2=*((void**)_check_dyneither_subscript(_tmp327,sizeof(void*),_tmp328)),_tmp881)));};}_LL1DE:;_LL1DF: {
# 1195
struct Cyc_Core_Impossible_exn_struct _tmp88E;const char*_tmp88D;void*_tmp88C[1];struct Cyc_String_pa_PrintArg_struct _tmp88B;struct Cyc_Core_Impossible_exn_struct*_tmp88A;(int)_throw((void*)((_tmp88A=_cycalloc(sizeof(*_tmp88A)),((_tmp88A[0]=((_tmp88E.tag=Cyc_Core_Impossible,((_tmp88E.f1=(struct _dyneither_ptr)((_tmp88B.tag=0,((_tmp88B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp88C[0]=& _tmp88B,Cyc_aprintf(((_tmp88D="anal_Rexp: AggrMember: %s",_tag_dyneither(_tmp88D,sizeof(char),26))),_tag_dyneither(_tmp88C,sizeof(void*),1)))))))),_tmp88E)))),_tmp88A)))));}_LL1DB:;};}_LL15A: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp214=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp214->tag != 36)goto _LL15C;else{_tmp215=_tmp214->f1;_tmp216=_tmp214->f2;}}_LL15B: {
# 1202
union Cyc_CfFlowInfo_FlowInfo _tmp331;void*_tmp332;struct _tuple14 _tmp330=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp215);_tmp331=_tmp330.f1;_tmp332=_tmp330.f2;
# 1203
if(Cyc_Absyn_is_nontagged_union_type((void*)_check_null(_tmp215->topt))){
# 1204
struct _tuple14 _tmp88F;return(_tmp88F.f1=_tmp331,((_tmp88F.f2=_tmp1A2->unknown_all,_tmp88F)));}{
# 1205
void*_tmp334=_tmp332;struct Cyc_CfFlowInfo_UnionRInfo _tmp336;int _tmp337;int _tmp338;struct _dyneither_ptr _tmp339;_LL1E1: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp335=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp334;if(_tmp335->tag != 6)goto _LL1E3;else{_tmp336=_tmp335->f1;_tmp337=_tmp336.is_union;_tmp338=_tmp336.fieldnum;_tmp339=_tmp335->f2;}}_LL1E2: {
# 1207
int _tmp33A=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp215->topt),_tmp216);
# 1208
if(_tmp337  && _tmp338 != - 1){
# 1209
if(_tmp338 != _tmp33A){
# 1210
struct _tuple14 _tmp890;return(_tmp890.f1=_tmp331,((_tmp890.f2=_tmp1A2->zero,_tmp890)));}else{
# 1212
struct _tuple14 _tmp891;return(_tmp891.f1=_tmp331,((_tmp891.f2=_tmp1A2->notzeroall,_tmp891)));}}else{
# 1214
struct _tuple14 _tmp892;return(_tmp892.f1=_tmp331,((_tmp892.f2=_tmp1A2->unknown_all,_tmp892)));}}_LL1E3:;_LL1E4: {
# 1216
struct Cyc_Core_Impossible_exn_struct _tmp89F;const char*_tmp89E;void*_tmp89D[1];struct Cyc_String_pa_PrintArg_struct _tmp89C;struct Cyc_Core_Impossible_exn_struct*_tmp89B;(int)_throw((void*)((_tmp89B=_cycalloc(sizeof(*_tmp89B)),((_tmp89B[0]=((_tmp89F.tag=Cyc_Core_Impossible,((_tmp89F.f1=(struct _dyneither_ptr)((_tmp89C.tag=0,((_tmp89C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp89D[0]=& _tmp89C,Cyc_aprintf(((_tmp89E="anal_Rexp: TagCheck_e: %s",_tag_dyneither(_tmp89E,sizeof(char),26))),_tag_dyneither(_tmp89D,sizeof(void*),1)))))))),_tmp89F)))),_tmp89B)))));}_LL1E0:;};}_LL15C: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp217=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp217->tag != 21)goto _LL15E;else{_tmp218=_tmp217->f1;_tmp219=_tmp217->f2;}}_LL15D: {
# 1221
union Cyc_CfFlowInfo_FlowInfo _tmp344;void*_tmp345;struct _tuple14 _tmp343=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp218);_tmp344=_tmp343.f1;_tmp345=_tmp343.f2;{
# 1224
union Cyc_CfFlowInfo_FlowInfo _tmp347;void*_tmp348;struct _tuple14 _tmp346=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp344,_tmp218,_tmp345);_tmp347=_tmp346.f1;_tmp348=_tmp346.f2;{
# 1227
void*_tmp349=Cyc_Tcutil_compress((void*)_check_null(_tmp218->topt));struct Cyc_Absyn_PtrInfo _tmp34B;void*_tmp34C;_LL1E6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp34A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp349;if(_tmp34A->tag != 5)goto _LL1E8;else{_tmp34B=_tmp34A->f1;_tmp34C=_tmp34B.elt_typ;}}_LL1E7:
# 1229
 if(Cyc_Absyn_is_nontagged_union_type(_tmp34C)){
# 1230
struct _tuple14 _tmp8A0;return(_tmp8A0.f1=_tmp347,((_tmp8A0.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp1A2,(void*)_check_null(e->topt),0,_tmp1A2->unknown_all),_tmp8A0)));}{
# 1231
void*_tmp34E=_tmp348;struct Cyc_CfFlowInfo_UnionRInfo _tmp350;int _tmp351;int _tmp352;struct _dyneither_ptr _tmp353;_LL1EB: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp34F=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp34E;if(_tmp34F->tag != 6)goto _LL1ED;else{_tmp350=_tmp34F->f1;_tmp351=_tmp350.is_union;_tmp352=_tmp350.fieldnum;_tmp353=_tmp34F->f2;}}_LL1EC: {
# 1233
int _tmp354=Cyc_CfFlowInfo_get_field_index(_tmp34C,_tmp219);
# 1234
if((_tmp351  && _tmp352 != - 1) && _tmp352 != _tmp354){
# 1235
struct _tuple14 _tmp8A1;return(_tmp8A1.f1=_tmp347,((_tmp8A1.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp1A2,(void*)_check_null(e->topt),1,_tmp1A2->unknown_none),_tmp8A1)));}{
# 1236
struct _tuple14 _tmp8A2;return(_tmp8A2.f1=_tmp347,((_tmp8A2.f2=*((void**)_check_dyneither_subscript(_tmp353,sizeof(void*),_tmp354)),_tmp8A2)));};}_LL1ED:;_LL1EE: {
# 1237
struct Cyc_Core_Impossible_exn_struct _tmp8A8;const char*_tmp8A7;struct Cyc_Core_Impossible_exn_struct*_tmp8A6;(int)_throw((void*)((_tmp8A6=_cycalloc(sizeof(*_tmp8A6)),((_tmp8A6[0]=((_tmp8A8.tag=Cyc_Core_Impossible,((_tmp8A8.f1=((_tmp8A7="anal_Rexp: AggrArrow",_tag_dyneither(_tmp8A7,sizeof(char),21))),_tmp8A8)))),_tmp8A6)))));}_LL1EA:;};_LL1E8:;_LL1E9: {
# 1239
struct Cyc_Core_Impossible_exn_struct _tmp8AE;const char*_tmp8AD;struct Cyc_Core_Impossible_exn_struct*_tmp8AC;(int)_throw((void*)((_tmp8AC=_cycalloc(sizeof(*_tmp8AC)),((_tmp8AC[0]=((_tmp8AE.tag=Cyc_Core_Impossible,((_tmp8AE.f1=((_tmp8AD="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp8AD,sizeof(char),25))),_tmp8AE)))),_tmp8AC)))));}_LL1E5:;};};}_LL15E: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp21A=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp21A->tag != 5)goto _LL160;else{_tmp21B=_tmp21A->f1;_tmp21C=_tmp21A->f2;_tmp21D=_tmp21A->f3;}}_LL15F: {
# 1243
union Cyc_CfFlowInfo_FlowInfo _tmp35E;union Cyc_CfFlowInfo_FlowInfo _tmp35F;struct _tuple16 _tmp35D=Cyc_NewControlFlow_anal_test(env,inflow,_tmp21B);_tmp35E=_tmp35D.f1;_tmp35F=_tmp35D.f2;{
# 1244
struct _tuple14 _tmp360=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp35E,_tmp21C);
# 1245
struct _tuple14 _tmp361=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp35F,_tmp21D);
# 1247
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp1A2,env->all_changed,_tmp360,_tmp361);};}_LL160: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp21E=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp21E->tag != 6)goto _LL162;else{_tmp21F=_tmp21E->f1;_tmp220=_tmp21E->f2;}}_LL161: {
# 1250
union Cyc_CfFlowInfo_FlowInfo _tmp363;union Cyc_CfFlowInfo_FlowInfo _tmp364;struct _tuple16 _tmp362=Cyc_NewControlFlow_anal_test(env,inflow,_tmp21F);_tmp363=_tmp362.f1;_tmp364=_tmp362.f2;{
# 1251
union Cyc_CfFlowInfo_FlowInfo _tmp366;void*_tmp367;struct _tuple14 _tmp365=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp363,_tmp220);_tmp366=_tmp365.f1;_tmp367=_tmp365.f2;{
# 1252
struct _tuple14 _tmp8AF;struct _tuple14 _tmp368=(_tmp8AF.f1=_tmp366,((_tmp8AF.f2=_tmp367,_tmp8AF)));
# 1253
struct _tuple14 _tmp8B0;struct _tuple14 _tmp369=(_tmp8B0.f1=_tmp364,((_tmp8B0.f2=_tmp1A2->zero,_tmp8B0)));
# 1254
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp1A2,env->all_changed,_tmp368,_tmp369);};};}_LL162: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp221=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp221->tag != 7)goto _LL164;else{_tmp222=_tmp221->f1;_tmp223=_tmp221->f2;}}_LL163: {
# 1257
union Cyc_CfFlowInfo_FlowInfo _tmp36D;union Cyc_CfFlowInfo_FlowInfo _tmp36E;struct _tuple16 _tmp36C=Cyc_NewControlFlow_anal_test(env,inflow,_tmp222);_tmp36D=_tmp36C.f1;_tmp36E=_tmp36C.f2;{
# 1258
union Cyc_CfFlowInfo_FlowInfo _tmp370;void*_tmp371;struct _tuple14 _tmp36F=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp36E,_tmp223);_tmp370=_tmp36F.f1;_tmp371=_tmp36F.f2;{
# 1259
struct _tuple14 _tmp8B1;struct _tuple14 _tmp372=(_tmp8B1.f1=_tmp370,((_tmp8B1.f2=_tmp371,_tmp8B1)));
# 1260
struct _tuple14 _tmp8B2;struct _tuple14 _tmp373=(_tmp8B2.f1=_tmp36D,((_tmp8B2.f2=_tmp1A2->notzeroall,_tmp8B2)));
# 1261
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp1A2,env->all_changed,_tmp372,_tmp373);};};}_LL164: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp224=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp224->tag != 22)goto _LL166;else{_tmp225=_tmp224->f1;_tmp226=_tmp224->f2;}}_LL165: {
# 1264
struct _RegionHandle _tmp376=_new_region("temp");struct _RegionHandle*temp=& _tmp376;_push_region(temp);
# 1265
{union Cyc_CfFlowInfo_FlowInfo _tmp379;struct Cyc_List_List*_tmp37A;struct Cyc_Absyn_Exp*_tmp8B3[2];struct _tuple20 _tmp378=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
# 1266
(_tmp8B3[1]=_tmp226,((_tmp8B3[0]=_tmp225,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp8B3,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp379=_tmp378.f1;_tmp37A=_tmp378.f2;{
# 1269
union Cyc_CfFlowInfo_FlowInfo _tmp37B=_tmp379;
# 1270
{union Cyc_CfFlowInfo_FlowInfo _tmp37C=_tmp379;struct _tuple13 _tmp37D;struct Cyc_Dict_Dict _tmp37E;struct Cyc_List_List*_tmp37F;_LL1F0: if((_tmp37C.ReachableFL).tag != 2)goto _LL1F2;_tmp37D=(struct _tuple13)(_tmp37C.ReachableFL).val;_tmp37E=_tmp37D.f1;_tmp37F=_tmp37D.f2;_LL1F1:
# 1274
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp37E,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp37A))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
# 1275
const char*_tmp8B6;void*_tmp8B5;(_tmp8B5=0,Cyc_Tcutil_terr(_tmp226->loc,((_tmp8B6="expression may not be initialized",_tag_dyneither(_tmp8B6,sizeof(char),34))),_tag_dyneither(_tmp8B5,sizeof(void*),0)));}{
# 1276
struct Cyc_List_List*_tmp382=Cyc_NewControlFlow_add_subscript_reln(_tmp1A2->r,_tmp37F,_tmp225,_tmp226);
# 1277
if(_tmp37F != _tmp382)
# 1278
_tmp37B=Cyc_CfFlowInfo_ReachableFL(_tmp37E,_tmp382);
# 1279
goto _LL1EF;};_LL1F2:;_LL1F3:
# 1280
 goto _LL1EF;_LL1EF:;}{
# 1282
void*_tmp383=Cyc_Tcutil_compress((void*)_check_null(_tmp225->topt));struct Cyc_List_List*_tmp385;struct Cyc_Absyn_PtrInfo _tmp387;struct Cyc_Absyn_PtrAtts _tmp388;union Cyc_Absyn_Constraint*_tmp389;_LL1F5: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp384=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp383;if(_tmp384->tag != 10)goto _LL1F7;else{_tmp385=_tmp384->f1;}}_LL1F6: {
# 1284
void*_tmp38A=(void*)((struct Cyc_List_List*)_check_null(_tmp37A))->hd;struct _dyneither_ptr _tmp38C;_LL1FC: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp38B=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp38A;if(_tmp38B->tag != 6)goto _LL1FE;else{_tmp38C=_tmp38B->f2;}}_LL1FD: {
# 1286
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp226)).f1;
# 1287
struct _tuple14 _tmp8B7;struct _tuple14 _tmp38E=(_tmp8B7.f1=_tmp37B,((_tmp8B7.f2=*((void**)_check_dyneither_subscript(_tmp38C,sizeof(void*),(int)i)),_tmp8B7)));_npop_handler(0);return _tmp38E;}_LL1FE:;_LL1FF: {
# 1288
struct Cyc_Core_Impossible_exn_struct _tmp8BD;const char*_tmp8BC;struct Cyc_Core_Impossible_exn_struct*_tmp8BB;(int)_throw((void*)((_tmp8BB=_cycalloc(sizeof(*_tmp8BB)),((_tmp8BB[0]=((_tmp8BD.tag=Cyc_Core_Impossible,((_tmp8BD.f1=((_tmp8BC="anal_Rexp: Subscript",_tag_dyneither(_tmp8BC,sizeof(char),21))),_tmp8BD)))),_tmp8BB)))));}_LL1FB:;}_LL1F7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp386=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp383;if(_tmp386->tag != 5)goto _LL1F9;else{_tmp387=_tmp386->f1;_tmp388=_tmp387.ptr_atts;_tmp389=_tmp388.bounds;}}_LL1F8: {
# 1291
union Cyc_CfFlowInfo_FlowInfo _tmp393;void*_tmp394;struct _tuple14 _tmp392=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp379,_tmp225,(void*)((struct Cyc_List_List*)_check_null(_tmp37A))->hd);_tmp393=_tmp392.f1;_tmp394=_tmp392.f2;{
# 1292
union Cyc_CfFlowInfo_FlowInfo _tmp395=_tmp393;int _tmp396;_LL201: if((_tmp395.BottomFL).tag != 1)goto _LL203;_tmp396=(int)(_tmp395.BottomFL).val;_LL202: {
# 1293
struct _tuple14 _tmp8BE;struct _tuple14 _tmp398=(_tmp8BE.f1=_tmp393,((_tmp8BE.f2=_tmp394,_tmp8BE)));_npop_handler(0);return _tmp398;}_LL203:;_LL204: {
# 1294
struct _tuple14 _tmp8BF;struct _tuple14 _tmp39A=(_tmp8BF.f1=_tmp37B,((_tmp8BF.f2=_tmp394,_tmp8BF)));_npop_handler(0);return _tmp39A;}_LL200:;};}_LL1F9:;_LL1FA: {
# 1296
struct Cyc_Core_Impossible_exn_struct _tmp8C5;const char*_tmp8C4;struct Cyc_Core_Impossible_exn_struct*_tmp8C3;(int)_throw((void*)((_tmp8C3=_cycalloc(sizeof(*_tmp8C3)),((_tmp8C3[0]=((_tmp8C5.tag=Cyc_Core_Impossible,((_tmp8C5.f1=((_tmp8C4="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp8C4,sizeof(char),33))),_tmp8C5)))),_tmp8C3)))));}_LL1F4:;};};}
# 1265
;_pop_region(temp);}_LL166: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp227=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp227->tag != 29)goto _LL168;else{_tmp228=_tmp227->f1;_tmp229=_tmp227->f2;}}_LL167:
# 1301
 _tmp22B=_tmp228;goto _LL169;_LL168: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp22A=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp22A->tag != 23)goto _LL16A;else{_tmp22B=_tmp22A->f1;}}_LL169: {
# 1303
struct _RegionHandle _tmp39E=_new_region("temp");struct _RegionHandle*temp=& _tmp39E;_push_region(temp);
# 1304
{union Cyc_CfFlowInfo_FlowInfo _tmp3A0;struct Cyc_List_List*_tmp3A1;struct _tuple20 _tmp39F=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp22B,1,1);_tmp3A0=_tmp39F.f1;_tmp3A1=_tmp39F.f2;{
# 1305
unsigned int _tmp8CB;unsigned int _tmp8CA;struct _dyneither_ptr _tmp8C9;void**_tmp8C8;unsigned int _tmp8C7;struct _dyneither_ptr aggrdict=
# 1306
(_tmp8C7=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp22B),((_tmp8C8=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp8C7)),((_tmp8C9=_tag_dyneither(_tmp8C8,sizeof(void*),_tmp8C7),((((_tmp8CA=_tmp8C7,_tmp8CC(& _tmp8CB,& _tmp8CA,& _tmp8C8,& _tmp3A1))),_tmp8C9)))))));
# 1311
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp8D6;struct Cyc_CfFlowInfo_UnionRInfo _tmp8D5;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp8D4;struct _tuple14 _tmp8D3;struct _tuple14 _tmp3A6=(_tmp8D3.f1=_tmp3A0,((_tmp8D3.f2=(void*)((_tmp8D6=_region_malloc(env->r,sizeof(*_tmp8D6)),((_tmp8D6[0]=((_tmp8D4.tag=6,((_tmp8D4.f1=((_tmp8D5.is_union=0,((_tmp8D5.fieldnum=- 1,_tmp8D5)))),((_tmp8D4.f2=aggrdict,_tmp8D4)))))),_tmp8D6)))),_tmp8D3)));_npop_handler(0);return _tmp3A6;};}
# 1304
;_pop_region(temp);}_LL16A: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp22C=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp22C->tag != 28)goto _LL16C;else{_tmp22D=_tmp22C->f2;}}_LL16B: {
# 1314
struct Cyc_List_List*fs;
# 1315
{void*_tmp3AC=Cyc_Tcutil_compress((void*)_check_null(e->topt));struct Cyc_List_List*_tmp3AE;_LL206: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp3AD=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3AC;if(_tmp3AD->tag != 12)goto _LL208;else{_tmp3AE=_tmp3AD->f2;}}_LL207:
# 1316
 fs=_tmp3AE;goto _LL205;_LL208:;_LL209: {
# 1317
struct Cyc_Core_Impossible_exn_struct _tmp8DC;const char*_tmp8DB;struct Cyc_Core_Impossible_exn_struct*_tmp8DA;(int)_throw((void*)((_tmp8DA=_cycalloc(sizeof(*_tmp8DA)),((_tmp8DA[0]=((_tmp8DC.tag=Cyc_Core_Impossible,((_tmp8DC.f1=((_tmp8DB="anal_Rexp:anon struct has bad type",_tag_dyneither(_tmp8DB,sizeof(char),35))),_tmp8DC)))),_tmp8DA)))));}_LL205:;}
# 1319
_tmp22F=_tmp22D;_tmp232=Cyc_Absyn_StructA;_tmp235=fs;goto _LL16D;}_LL16C: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp22E=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp22E->tag != 27)goto _LL16E;else{_tmp22F=_tmp22E->f3;_tmp230=_tmp22E->f4;if(_tmp230 == 0)goto _LL16E;_tmp231=*_tmp230;_tmp232=_tmp231.kind;_tmp233=_tmp231.impl;if(_tmp233 == 0)goto _LL16E;_tmp234=*_tmp233;_tmp235=_tmp234.fields;}}_LL16D: {
# 1321
struct _RegionHandle _tmp3B2=_new_region("temp");struct _RegionHandle*temp=& _tmp3B2;_push_region(temp);
# 1322
{union Cyc_CfFlowInfo_FlowInfo _tmp3B4;struct Cyc_List_List*_tmp3B5;struct _tuple20 _tmp3B3=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,
# 1323
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple22*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple22*))Cyc_Core_snd,_tmp22F),1,1);_tmp3B4=_tmp3B3.f1;_tmp3B5=_tmp3B3.f2;{
# 1324
struct _dyneither_ptr aggrdict=
# 1325
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp1A2,_tmp235,_tmp232 == Cyc_Absyn_UnionA,_tmp232 == Cyc_Absyn_UnionA?_tmp1A2->unknown_none: _tmp1A2->unknown_all);
# 1326
int field_no=- 1;
# 1327
{int i=0;for(0;_tmp3B5 != 0;(((_tmp3B5=_tmp3B5->tl,_tmp22F=_tmp22F->tl)),++ i)){
# 1328
struct Cyc_List_List*ds=(*((struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp22F))->hd)).f1;for(0;ds != 0;ds=ds->tl){
# 1329
void*_tmp3B6=(void*)ds->hd;struct _dyneither_ptr*_tmp3B9;_LL20B: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp3B7=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp3B6;if(_tmp3B7->tag != 0)goto _LL20D;}_LL20C: {
# 1330
struct Cyc_Core_Impossible_exn_struct _tmp8E2;const char*_tmp8E1;struct Cyc_Core_Impossible_exn_struct*_tmp8E0;(int)_throw((void*)((_tmp8E0=_cycalloc(sizeof(*_tmp8E0)),((_tmp8E0[0]=((_tmp8E2.tag=Cyc_Core_Impossible,((_tmp8E2.f1=((_tmp8E1="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp8E1,sizeof(char),22))),_tmp8E2)))),_tmp8E0)))));}_LL20D: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3B8=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3B6;if(_tmp3B8->tag != 1)goto _LL20A;else{_tmp3B9=_tmp3B8->f1;}}_LL20E:
# 1333
 field_no=Cyc_CfFlowInfo_get_field_index_fs(_tmp235,_tmp3B9);
# 1334
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp3B5->hd;
# 1336
if(_tmp232 == Cyc_Absyn_UnionA){
# 1337
struct Cyc_Absyn_Exp*_tmp3BD=(*((struct _tuple22*)_tmp22F->hd)).f2;
# 1338
_tmp3B4=Cyc_NewControlFlow_use_Rval(env,_tmp3BD->loc,_tmp3B4,(void*)_tmp3B5->hd);}_LL20A:;}}}{
# 1341
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp8EC;struct Cyc_CfFlowInfo_UnionRInfo _tmp8EB;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp8EA;struct _tuple14 _tmp8E9;struct _tuple14 _tmp3C2=(_tmp8E9.f1=_tmp3B4,((_tmp8E9.f2=(void*)((_tmp8EC=_region_malloc(env->r,sizeof(*_tmp8EC)),((_tmp8EC[0]=((_tmp8EA.tag=6,((_tmp8EA.f1=((_tmp8EB.is_union=_tmp232 == Cyc_Absyn_UnionA,((_tmp8EB.fieldnum=field_no,_tmp8EB)))),((_tmp8EA.f2=aggrdict,_tmp8EA)))))),_tmp8EC)))),_tmp8E9)));_npop_handler(0);return _tmp3C2;};};}
# 1322
;_pop_region(temp);}_LL16E: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp236=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp236->tag != 27)goto _LL170;}_LL16F: {
# 1344
struct Cyc_Core_Impossible_exn_struct _tmp8F2;const char*_tmp8F1;struct Cyc_Core_Impossible_exn_struct*_tmp8F0;(int)_throw((void*)((_tmp8F0=_cycalloc(sizeof(*_tmp8F0)),((_tmp8F0[0]=((_tmp8F2.tag=Cyc_Core_Impossible,((_tmp8F2.f1=((_tmp8F1="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp8F1,sizeof(char),31))),_tmp8F2)))),_tmp8F0)))));}_LL170: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp237=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp237->tag != 25)goto _LL172;else{_tmp238=_tmp237->f1;}}_LL171: {
# 1346
struct _RegionHandle _tmp3C6=_new_region("temp");struct _RegionHandle*temp=& _tmp3C6;_push_region(temp);
# 1347
{struct Cyc_List_List*_tmp3C7=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple22*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple22*))Cyc_Core_snd,_tmp238);
# 1348
union Cyc_CfFlowInfo_FlowInfo _tmp3C9;struct Cyc_List_List*_tmp3CA;struct _tuple20 _tmp3C8=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp3C7,1,1);_tmp3C9=_tmp3C8.f1;_tmp3CA=_tmp3C8.f2;
# 1349
for(0;_tmp3CA != 0;(_tmp3CA=_tmp3CA->tl,_tmp3C7=_tmp3C7->tl)){
# 1350
_tmp3C9=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3C7))->hd)->loc,_tmp3C9,(void*)_tmp3CA->hd);}{
# 1351
struct _tuple14 _tmp8F3;struct _tuple14 _tmp3CC=(_tmp8F3.f1=_tmp3C9,((_tmp8F3.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp1A2,(void*)_check_null(e->topt),0,_tmp1A2->unknown_all),_tmp8F3)));_npop_handler(0);return _tmp3CC;};}
# 1347
;_pop_region(temp);}_LL172: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp239=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp239->tag != 26)goto _LL174;else{_tmp23A=_tmp239->f1;_tmp23B=_tmp239->f2;_tmp23C=_tmp239->f3;_tmp23D=_tmp239->f4;}}_LL173: {
# 1355
union Cyc_CfFlowInfo_FlowInfo _tmp3CE;void*_tmp3CF;struct _tuple14 _tmp3CD=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp23B);_tmp3CE=_tmp3CD.f1;_tmp3CF=_tmp3CD.f2;{
# 1356
union Cyc_CfFlowInfo_FlowInfo _tmp3D0=_tmp3CE;int _tmp3D1;struct _tuple13 _tmp3D2;struct Cyc_Dict_Dict _tmp3D3;struct Cyc_List_List*_tmp3D4;_LL210: if((_tmp3D0.BottomFL).tag != 1)goto _LL212;_tmp3D1=(int)(_tmp3D0.BottomFL).val;_LL211: {
# 1357
struct _tuple14 _tmp8F4;return(_tmp8F4.f1=_tmp3CE,((_tmp8F4.f2=_tmp1A2->unknown_all,_tmp8F4)));}_LL212: if((_tmp3D0.ReachableFL).tag != 2)goto _LL20F;_tmp3D2=(struct _tuple13)(_tmp3D0.ReachableFL).val;_tmp3D3=_tmp3D2.f1;_tmp3D4=_tmp3D2.f2;_LL213:
# 1359
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp3D3,_tmp3CF)== Cyc_CfFlowInfo_NoneIL){
# 1360
const char*_tmp8F7;void*_tmp8F6;(_tmp8F6=0,Cyc_Tcutil_terr(_tmp23B->loc,((_tmp8F7="expression may not be initialized",_tag_dyneither(_tmp8F7,sizeof(char),34))),_tag_dyneither(_tmp8F6,sizeof(void*),0)));}{
# 1363
struct Cyc_List_List*new_relns=_tmp3D4;
# 1364
comp_loop: {
# 1365
void*_tmp3D8=_tmp23B->r;struct Cyc_Absyn_Exp*_tmp3DA;void*_tmp3DC;struct Cyc_Absyn_Vardecl*_tmp3DE;void*_tmp3E0;struct Cyc_Absyn_Vardecl*_tmp3E2;void*_tmp3E4;struct Cyc_Absyn_Vardecl*_tmp3E6;void*_tmp3E8;struct Cyc_Absyn_Vardecl*_tmp3EA;union Cyc_Absyn_Cnst _tmp3EC;struct _tuple6 _tmp3ED;int _tmp3EE;enum Cyc_Absyn_Primop _tmp3F0;struct Cyc_List_List*_tmp3F1;struct Cyc_List_List _tmp3F2;struct Cyc_Absyn_Exp*_tmp3F3;_LL215: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3D9=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3D8;if(_tmp3D9->tag != 13)goto _LL217;else{_tmp3DA=_tmp3D9->f2;}}_LL216:
# 1366
 _tmp23B=_tmp3DA;goto comp_loop;_LL217:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3DB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3D8;if(_tmp3DB->tag != 1)goto _LL219;else{_tmp3DC=(void*)_tmp3DB->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp3DD=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp3DC;if(_tmp3DD->tag != 1)goto _LL219;else{_tmp3DE=_tmp3DD->f1;}};}}if(!(!_tmp3DE->escapes))goto _LL219;_LL218:
# 1367
 _tmp3E2=_tmp3DE;goto _LL21A;_LL219:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3DF=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3D8;if(_tmp3DF->tag != 1)goto _LL21B;else{_tmp3E0=(void*)_tmp3DF->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3E1=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp3E0;if(_tmp3E1->tag != 4)goto _LL21B;else{_tmp3E2=_tmp3E1->f1;}};}}if(!(!_tmp3E2->escapes))goto _LL21B;_LL21A:
# 1368
 _tmp3E6=_tmp3E2;goto _LL21C;_LL21B:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3E3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3D8;if(_tmp3E3->tag != 1)goto _LL21D;else{_tmp3E4=(void*)_tmp3E3->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp3E5=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp3E4;if(_tmp3E5->tag != 5)goto _LL21D;else{_tmp3E6=_tmp3E5->f1;}};}}if(!(!_tmp3E6->escapes))goto _LL21D;_LL21C:
# 1369
 _tmp3EA=_tmp3E6;goto _LL21E;_LL21D:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3E7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3D8;if(_tmp3E7->tag != 1)goto _LL21F;else{_tmp3E8=(void*)_tmp3E7->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp3E9=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp3E8;if(_tmp3E9->tag != 3)goto _LL21F;else{_tmp3EA=_tmp3E9->f1;}};}}if(!(!_tmp3EA->escapes))goto _LL21F;_LL21E:
# 1371
{struct Cyc_CfFlowInfo_Reln*_tmp8FA;struct Cyc_List_List*_tmp8F9;new_relns=((_tmp8F9=_region_malloc(env->r,sizeof(*_tmp8F9)),((_tmp8F9->hd=((_tmp8FA=_region_malloc(env->r,sizeof(*_tmp8FA)),((_tmp8FA->vd=_tmp23A,((_tmp8FA->rop=Cyc_CfFlowInfo_LessVar(_tmp3EA,_tmp3EA->type),_tmp8FA)))))),((_tmp8F9->tl=_tmp3D4,_tmp8F9))))));}
# 1372
goto _LL214;_LL21F: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp3EB=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3D8;if(_tmp3EB->tag != 0)goto _LL221;else{_tmp3EC=_tmp3EB->f1;if((_tmp3EC.Int_c).tag != 5)goto _LL221;_tmp3ED=(struct _tuple6)(_tmp3EC.Int_c).val;_tmp3EE=_tmp3ED.f2;}}_LL220:
# 1374
{struct Cyc_CfFlowInfo_Reln*_tmp8FD;struct Cyc_List_List*_tmp8FC;new_relns=((_tmp8FC=_region_malloc(env->r,sizeof(*_tmp8FC)),((_tmp8FC->hd=((_tmp8FD=_region_malloc(env->r,sizeof(*_tmp8FD)),((_tmp8FD->vd=_tmp23A,((_tmp8FD->rop=Cyc_CfFlowInfo_LessConst((unsigned int)_tmp3EE),_tmp8FD)))))),((_tmp8FC->tl=_tmp3D4,_tmp8FC))))));}
# 1375
goto _LL214;_LL221: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp3EF=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3D8;if(_tmp3EF->tag != 2)goto _LL223;else{_tmp3F0=_tmp3EF->f1;_tmp3F1=_tmp3EF->f2;if(_tmp3F1 == 0)goto _LL223;_tmp3F2=*_tmp3F1;_tmp3F3=(struct Cyc_Absyn_Exp*)_tmp3F2.hd;}}_LL222:
# 1377
{void*_tmp3F8=_tmp3F3->r;void*_tmp3FA;struct Cyc_Absyn_Vardecl*_tmp3FC;void*_tmp3FE;struct Cyc_Absyn_Vardecl*_tmp400;void*_tmp402;struct Cyc_Absyn_Vardecl*_tmp404;void*_tmp406;struct Cyc_Absyn_Vardecl*_tmp408;_LL226:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3F9=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3F8;if(_tmp3F9->tag != 1)goto _LL228;else{_tmp3FA=(void*)_tmp3F9->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp3FB=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp3FA;if(_tmp3FB->tag != 1)goto _LL228;else{_tmp3FC=_tmp3FB->f1;}};}}if(!(!_tmp3FC->escapes))goto _LL228;_LL227:
# 1378
 _tmp400=_tmp3FC;goto _LL229;_LL228:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3FD=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3F8;if(_tmp3FD->tag != 1)goto _LL22A;else{_tmp3FE=(void*)_tmp3FD->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3FF=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp3FE;if(_tmp3FF->tag != 4)goto _LL22A;else{_tmp400=_tmp3FF->f1;}};}}if(!(!_tmp400->escapes))goto _LL22A;_LL229:
# 1379
 _tmp404=_tmp400;goto _LL22B;_LL22A:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp401=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3F8;if(_tmp401->tag != 1)goto _LL22C;else{_tmp402=(void*)_tmp401->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp403=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp402;if(_tmp403->tag != 5)goto _LL22C;else{_tmp404=_tmp403->f1;}};}}if(!(!_tmp404->escapes))goto _LL22C;_LL22B:
# 1380
 _tmp408=_tmp404;goto _LL22D;_LL22C:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp405=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3F8;if(_tmp405->tag != 1)goto _LL22E;else{_tmp406=(void*)_tmp405->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp407=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp406;if(_tmp407->tag != 3)goto _LL22E;else{_tmp408=_tmp407->f1;}};}}if(!(!_tmp408->escapes))goto _LL22E;_LL22D:
# 1383
{struct Cyc_CfFlowInfo_Reln*_tmp900;struct Cyc_List_List*_tmp8FF;new_relns=((_tmp8FF=_region_malloc(env->r,sizeof(*_tmp8FF)),((_tmp8FF->hd=((_tmp900=_region_malloc(env->r,sizeof(*_tmp900)),((_tmp900->vd=_tmp23A,((_tmp900->rop=Cyc_CfFlowInfo_LessNumelts(_tmp408),_tmp900)))))),((_tmp8FF->tl=_tmp3D4,_tmp8FF))))));}
# 1384
goto _LL225;_LL22E:;_LL22F:
# 1385
 goto _LL225;_LL225:;}
# 1387
goto _LL214;_LL223:;_LL224:
# 1388
 goto _LL214;_LL214:;}
# 1390
if(_tmp3D4 != new_relns)
# 1391
_tmp3CE=Cyc_CfFlowInfo_ReachableFL(_tmp3D3,new_relns);{
# 1394
void*_tmp40B=_tmp3CF;_LL231: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp40C=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp40B;if(_tmp40C->tag != 0)goto _LL233;}_LL232: {
# 1395
struct _tuple14 _tmp901;return(_tmp901.f1=_tmp3CE,((_tmp901.f2=_tmp1A2->unknown_all,_tmp901)));}_LL233: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp40D=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp40B;if(_tmp40D->tag != 2)goto _LL235;}_LL234:
# 1396
 goto _LL236;_LL235: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp40E=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp40B;if(_tmp40E->tag != 1)goto _LL237;}_LL236:
# 1397
 goto _LL238;_LL237: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp40F=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp40B;if(_tmp40F->tag != 5)goto _LL239;}_LL238: {
# 1399
struct Cyc_List_List _tmp902;struct Cyc_List_List _tmp411=(_tmp902.hd=_tmp23A,((_tmp902.tl=0,_tmp902)));
# 1400
_tmp3CE=Cyc_NewControlFlow_add_vars(_tmp1A2,_tmp3CE,(struct Cyc_List_List*)& _tmp411,_tmp1A2->unknown_all,e->loc);{
# 1402
union Cyc_CfFlowInfo_FlowInfo _tmp413;void*_tmp414;struct _tuple14 _tmp412=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp3CE,_tmp23C);_tmp413=_tmp412.f1;_tmp414=_tmp412.f2;
# 1403
{union Cyc_CfFlowInfo_FlowInfo _tmp415=_tmp413;int _tmp416;struct _tuple13 _tmp417;struct Cyc_Dict_Dict _tmp418;_LL23C: if((_tmp415.BottomFL).tag != 1)goto _LL23E;_tmp416=(int)(_tmp415.BottomFL).val;_LL23D: {
# 1404
struct _tuple14 _tmp903;return(_tmp903.f1=_tmp413,((_tmp903.f2=_tmp1A2->unknown_all,_tmp903)));}_LL23E: if((_tmp415.ReachableFL).tag != 2)goto _LL23B;_tmp417=(struct _tuple13)(_tmp415.ReachableFL).val;_tmp418=_tmp417.f1;_LL23F:
# 1406
 if(Cyc_CfFlowInfo_initlevel(_tmp1A2,_tmp418,_tmp414)!= Cyc_CfFlowInfo_AllIL){
# 1407
{const char*_tmp906;void*_tmp905;(_tmp905=0,Cyc_Tcutil_terr(_tmp23B->loc,((_tmp906="expression may not be initialized",_tag_dyneither(_tmp906,sizeof(char),34))),_tag_dyneither(_tmp905,sizeof(void*),0)));}{
# 1408
struct _tuple14 _tmp907;return(_tmp907.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp907.f2=_tmp1A2->unknown_all,_tmp907)));};}_LL23B:;}
# 1411
_tmp3CE=_tmp413;
# 1412
goto _LL23A;};}_LL239:;_LL23A:
# 1414
 while(1){
# 1415
struct Cyc_List_List _tmp908;struct Cyc_List_List _tmp41E=(_tmp908.hd=_tmp23A,((_tmp908.tl=0,_tmp908)));
# 1416
_tmp3CE=Cyc_NewControlFlow_add_vars(_tmp1A2,_tmp3CE,(struct Cyc_List_List*)& _tmp41E,_tmp1A2->unknown_all,e->loc);{
# 1417
union Cyc_CfFlowInfo_FlowInfo _tmp420;void*_tmp421;struct _tuple14 _tmp41F=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp3CE,_tmp23C);_tmp420=_tmp41F.f1;_tmp421=_tmp41F.f2;
# 1418
{union Cyc_CfFlowInfo_FlowInfo _tmp422=_tmp420;int _tmp423;struct _tuple13 _tmp424;struct Cyc_Dict_Dict _tmp425;_LL241: if((_tmp422.BottomFL).tag != 1)goto _LL243;_tmp423=(int)(_tmp422.BottomFL).val;_LL242:
# 1419
 goto _LL240;_LL243: if((_tmp422.ReachableFL).tag != 2)goto _LL240;_tmp424=(struct _tuple13)(_tmp422.ReachableFL).val;_tmp425=_tmp424.f1;_LL244:
# 1421
 if(Cyc_CfFlowInfo_initlevel(_tmp1A2,_tmp425,_tmp421)!= Cyc_CfFlowInfo_AllIL){
# 1422
{const char*_tmp90B;void*_tmp90A;(_tmp90A=0,Cyc_Tcutil_terr(_tmp23B->loc,((_tmp90B="expression may not be initialized",_tag_dyneither(_tmp90B,sizeof(char),34))),_tag_dyneither(_tmp90A,sizeof(void*),0)));}{
# 1423
struct _tuple14 _tmp90C;return(_tmp90C.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp90C.f2=_tmp1A2->unknown_all,_tmp90C)));};}_LL240:;}{
# 1426
union Cyc_CfFlowInfo_FlowInfo _tmp429=Cyc_CfFlowInfo_join_flow(_tmp1A2,env->all_changed,_tmp3CE,_tmp420);
# 1427
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp429,_tmp3CE))
# 1428
break;
# 1429
_tmp3CE=_tmp429;};};}{
# 1431
struct _tuple14 _tmp90D;return(_tmp90D.f1=_tmp3CE,((_tmp90D.f2=_tmp1A2->unknown_all,_tmp90D)));};_LL230:;};};_LL20F:;};}_LL174: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp23E=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp23E->tag != 35)goto _LL176;else{_tmp23F=_tmp23E->f1;}}_LL175:
# 1434
 return Cyc_NewControlFlow_anal_stmt_exp(env,copy_ctxt,inflow,_tmp23F);_LL176: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp240=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp240->tag != 1)goto _LL178;else{_tmp241=(void*)_tmp240->f2;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp242=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp241;if(_tmp242->tag != 0)goto _LL178;};}}_LL177:
# 1435
 goto _LL179;_LL178: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp243=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp243->tag != 34)goto _LL17A;}_LL179:
# 1436
 goto _LL17B;_LL17A: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp244=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp244->tag != 24)goto _LL17C;}_LL17B:
# 1437
 goto _LL17D;_LL17C: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp245=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp245->tag != 37)goto _LL17E;}_LL17D:
# 1438
 goto _LL17F;_LL17E: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp246=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp1BC;if(_tmp246->tag != 38)goto _LL119;}_LL17F: {
# 1440
struct Cyc_Core_Impossible_exn_struct _tmp913;const char*_tmp912;struct Cyc_Core_Impossible_exn_struct*_tmp911;(int)_throw((void*)((_tmp911=_cycalloc(sizeof(*_tmp911)),((_tmp911[0]=((_tmp913.tag=Cyc_Core_Impossible,((_tmp913.f1=((_tmp912="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp912,sizeof(char),31))),_tmp913)))),_tmp911)))));}_LL119:;};}
# 1449
static struct _tuple15 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,struct Cyc_List_List*flds){
# 1456
struct Cyc_CfFlowInfo_FlowEnv*_tmp42F=env->fenv;
# 1457
void*_tmp430=Cyc_Tcutil_compress((void*)_check_null(e->topt));struct Cyc_Absyn_PtrInfo _tmp432;void*_tmp433;struct Cyc_Absyn_PtrAtts _tmp434;union Cyc_Absyn_Constraint*_tmp435;union Cyc_Absyn_Constraint*_tmp436;_LL246: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp431=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp430;if(_tmp431->tag != 5)goto _LL248;else{_tmp432=_tmp431->f1;_tmp433=_tmp432.elt_typ;_tmp434=_tmp432.ptr_atts;_tmp435=_tmp434.bounds;_tmp436=_tmp434.zero_term;}}_LL247: {
# 1459
union Cyc_CfFlowInfo_FlowInfo _tmp437=f;int _tmp438;struct _tuple13 _tmp439;struct Cyc_Dict_Dict _tmp43A;struct Cyc_List_List*_tmp43B;_LL24B: if((_tmp437.BottomFL).tag != 1)goto _LL24D;_tmp438=(int)(_tmp437.BottomFL).val;_LL24C: {
# 1460
struct _tuple15 _tmp914;return(_tmp914.f1=f,((_tmp914.f2=Cyc_CfFlowInfo_UnknownL(),_tmp914)));}_LL24D: if((_tmp437.ReachableFL).tag != 2)goto _LL24A;_tmp439=(struct _tuple13)(_tmp437.ReachableFL).val;_tmp43A=_tmp439.f1;_tmp43B=_tmp439.f2;_LL24E:
# 1463
 if(Cyc_Tcutil_is_bound_one(_tmp435)){
# 1464
void*_tmp43D=r;struct Cyc_CfFlowInfo_Place*_tmp441;struct Cyc_CfFlowInfo_Place _tmp442;void*_tmp443;struct Cyc_List_List*_tmp444;enum Cyc_CfFlowInfo_InitLevel _tmp447;_LL250: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp43E=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp43D;if(_tmp43E->tag != 1)goto _LL252;}_LL251:
# 1465
 goto _LL253;_LL252: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp43F=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp43D;if(_tmp43F->tag != 2)goto _LL254;}_LL253:
# 1467
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp917;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp916;e->annot=(void*)((_tmp916=_cycalloc(sizeof(*_tmp916)),((_tmp916[0]=((_tmp917.tag=Cyc_CfFlowInfo_NotZero,((_tmp917.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp43B),_tmp917)))),_tmp916))));}goto _LL24F;_LL254: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp440=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp43D;if(_tmp440->tag != 5)goto _LL256;else{_tmp441=_tmp440->f1;_tmp442=*_tmp441;_tmp443=_tmp442.root;_tmp444=_tmp442.fields;}}_LL255:
# 1469
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp91A;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp919;e->annot=(void*)((_tmp919=_cycalloc(sizeof(*_tmp919)),((_tmp919[0]=((_tmp91A.tag=Cyc_CfFlowInfo_NotZero,((_tmp91A.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp43B),_tmp91A)))),_tmp919))));}{
# 1470
struct Cyc_CfFlowInfo_Place*_tmp91D;struct _tuple15 _tmp91C;return(_tmp91C.f1=f,((_tmp91C.f2=Cyc_CfFlowInfo_PlaceL(((_tmp91D=_region_malloc(_tmp42F->r,sizeof(*_tmp91D)),((_tmp91D->root=_tmp443,((_tmp91D->fields=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp42F->r,_tmp444,flds),_tmp91D))))))),_tmp91C)));};_LL256: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp445=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp43D;if(_tmp445->tag != 0)goto _LL258;}_LL257:
# 1472
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
# 1473
struct _tuple15 _tmp91E;return(_tmp91E.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp91E.f2=Cyc_CfFlowInfo_UnknownL(),_tmp91E)));};_LL258: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp446=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp43D;if(_tmp446->tag != 3)goto _LL25A;else{_tmp447=_tmp446->f1;}}_LL259:
# 1474
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp447);goto _LL25B;_LL25A:;_LL25B: {
# 1476
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp921;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp920;e->annot=(void*)((_tmp920=_cycalloc(sizeof(*_tmp920)),((_tmp920[0]=((_tmp921.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp921.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp43B),_tmp921)))),_tmp920))));}_LL24F:;}else{
# 1479
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp924;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp923;e->annot=(void*)((_tmp923=_cycalloc(sizeof(*_tmp923)),((_tmp923[0]=((_tmp924.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp924.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp43B),_tmp924)))),_tmp923))));}
# 1480
if(Cyc_CfFlowInfo_initlevel(_tmp42F,_tmp43A,r)== Cyc_CfFlowInfo_NoneIL){
# 1481
const char*_tmp927;void*_tmp926;(_tmp926=0,Cyc_Tcutil_terr(e->loc,((_tmp927="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp927,sizeof(char),46))),_tag_dyneither(_tmp926,sizeof(void*),0)));}{
# 1482
struct _tuple15 _tmp928;return(_tmp928.f1=f,((_tmp928.f2=Cyc_CfFlowInfo_UnknownL(),_tmp928)));};_LL24A:;}_LL248:;_LL249: {
# 1484
struct Cyc_Core_Impossible_exn_struct _tmp92E;const char*_tmp92D;struct Cyc_Core_Impossible_exn_struct*_tmp92C;(int)_throw((void*)((_tmp92C=_cycalloc(sizeof(*_tmp92C)),((_tmp92C[0]=((_tmp92E.tag=Cyc_Core_Impossible,((_tmp92E.f1=((_tmp92D="left deref of non-pointer-type",_tag_dyneither(_tmp92D,sizeof(char),31))),_tmp92E)))),_tmp92C)))));}_LL245:;}
# 1492
static struct _tuple15 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){
# 1495
struct Cyc_Dict_Dict d;
# 1496
struct Cyc_CfFlowInfo_FlowEnv*_tmp459=env->fenv;
# 1497
{union Cyc_CfFlowInfo_FlowInfo _tmp45A=inflow;int _tmp45B;struct _tuple13 _tmp45C;struct Cyc_Dict_Dict _tmp45D;struct Cyc_List_List*_tmp45E;_LL25D: if((_tmp45A.BottomFL).tag != 1)goto _LL25F;_tmp45B=(int)(_tmp45A.BottomFL).val;_LL25E: {
# 1498
struct _tuple15 _tmp92F;return(_tmp92F.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp92F.f2=Cyc_CfFlowInfo_UnknownL(),_tmp92F)));}_LL25F: if((_tmp45A.ReachableFL).tag != 2)goto _LL25C;_tmp45C=(struct _tuple13)(_tmp45A.ReachableFL).val;_tmp45D=_tmp45C.f1;_tmp45E=_tmp45C.f2;_LL260:
# 1500
 d=_tmp45D;_LL25C:;}{
# 1504
void*_tmp460=e->r;struct Cyc_Absyn_Exp*_tmp462;struct Cyc_Absyn_Exp*_tmp464;struct Cyc_Absyn_Exp*_tmp466;void*_tmp468;struct Cyc_Absyn_Vardecl*_tmp46A;void*_tmp46C;struct Cyc_Absyn_Vardecl*_tmp46E;void*_tmp470;struct Cyc_Absyn_Vardecl*_tmp472;void*_tmp474;struct Cyc_Absyn_Vardecl*_tmp476;struct Cyc_Absyn_Exp*_tmp478;struct _dyneither_ptr*_tmp479;struct Cyc_Absyn_Exp*_tmp47B;struct Cyc_Absyn_Exp*_tmp47D;struct Cyc_Absyn_Exp*_tmp47E;struct Cyc_Absyn_Exp*_tmp480;struct _dyneither_ptr*_tmp481;_LL262: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp461=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp460;if(_tmp461->tag != 13)goto _LL264;else{_tmp462=_tmp461->f2;}}_LL263:
# 1505
 _tmp464=_tmp462;goto _LL265;_LL264: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp463=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp460;if(_tmp463->tag != 11)goto _LL266;else{_tmp464=_tmp463->f1;}}_LL265:
# 1506
 _tmp466=_tmp464;goto _LL267;_LL266: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp465=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp460;if(_tmp465->tag != 12)goto _LL268;else{_tmp466=_tmp465->f1;}}_LL267:
# 1507
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp466,flds);_LL268: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp467=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp460;if(_tmp467->tag != 1)goto _LL26A;else{_tmp468=(void*)_tmp467->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp469=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp468;if(_tmp469->tag != 3)goto _LL26A;else{_tmp46A=_tmp469->f1;}};}}_LL269:
# 1509
 _tmp46E=_tmp46A;goto _LL26B;_LL26A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp46B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp460;if(_tmp46B->tag != 1)goto _LL26C;else{_tmp46C=(void*)_tmp46B->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp46D=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp46C;if(_tmp46D->tag != 4)goto _LL26C;else{_tmp46E=_tmp46D->f1;}};}}_LL26B:
# 1510
 _tmp472=_tmp46E;goto _LL26D;_LL26C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp46F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp460;if(_tmp46F->tag != 1)goto _LL26E;else{_tmp470=(void*)_tmp46F->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp471=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp470;if(_tmp471->tag != 5)goto _LL26E;else{_tmp472=_tmp471->f1;}};}}_LL26D: {
# 1512
struct Cyc_CfFlowInfo_Place*_tmp939;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp938;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp937;struct _tuple15 _tmp936;return(_tmp936.f1=inflow,((_tmp936.f2=Cyc_CfFlowInfo_PlaceL(((_tmp939=_region_malloc(env->r,sizeof(*_tmp939)),((_tmp939->root=(void*)((_tmp937=_region_malloc(env->r,sizeof(*_tmp937)),((_tmp937[0]=((_tmp938.tag=0,((_tmp938.f1=_tmp472,_tmp938)))),_tmp937)))),((_tmp939->fields=flds,_tmp939))))))),_tmp936)));}_LL26E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp473=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp460;if(_tmp473->tag != 1)goto _LL270;else{_tmp474=(void*)_tmp473->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp475=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp474;if(_tmp475->tag != 1)goto _LL270;else{_tmp476=_tmp475->f1;}};}}_LL26F: {
# 1514
struct _tuple15 _tmp93A;return(_tmp93A.f1=inflow,((_tmp93A.f2=Cyc_CfFlowInfo_UnknownL(),_tmp93A)));}_LL270: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp477=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp460;if(_tmp477->tag != 21)goto _LL272;else{_tmp478=_tmp477->f1;_tmp479=_tmp477->f2;}}_LL271:
# 1517
{void*_tmp487=Cyc_Tcutil_compress((void*)_check_null(_tmp478->topt));struct Cyc_Absyn_PtrInfo _tmp489;void*_tmp48A;_LL27B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp488=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp487;if(_tmp488->tag != 5)goto _LL27D;else{_tmp489=_tmp488->f1;_tmp48A=_tmp489.elt_typ;}}_LL27C:
# 1519
 if(!Cyc_Absyn_is_nontagged_union_type(_tmp48A)){
# 1520
struct Cyc_List_List*_tmp93B;flds=((_tmp93B=_region_malloc(env->r,sizeof(*_tmp93B)),((_tmp93B->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp48A,_tmp479),((_tmp93B->tl=flds,_tmp93B))))));}
# 1521
goto _LL27A;_LL27D:;_LL27E: {
# 1522
struct Cyc_Core_Impossible_exn_struct _tmp941;const char*_tmp940;struct Cyc_Core_Impossible_exn_struct*_tmp93F;(int)_throw((void*)((_tmp93F=_cycalloc(sizeof(*_tmp93F)),((_tmp93F[0]=((_tmp941.tag=Cyc_Core_Impossible,((_tmp941.f1=((_tmp940="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp940,sizeof(char),25))),_tmp941)))),_tmp93F)))));}_LL27A:;}
# 1524
_tmp47B=_tmp478;goto _LL273;_LL272: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp47A=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp460;if(_tmp47A->tag != 19)goto _LL274;else{_tmp47B=_tmp47A->f1;}}_LL273:
# 1526
 if(expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp47B->topt))){
# 1527
union Cyc_CfFlowInfo_FlowInfo _tmp490;union Cyc_CfFlowInfo_AbsLVal _tmp491;struct _tuple15 _tmp48F=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp47B);_tmp490=_tmp48F.f1;_tmp491=_tmp48F.f2;{
# 1528
struct _tuple15 _tmp942;struct _tuple15 _tmp493=(_tmp942.f1=_tmp490,((_tmp942.f2=_tmp491,_tmp942)));union Cyc_CfFlowInfo_FlowInfo _tmp494;struct _tuple13 _tmp495;struct Cyc_Dict_Dict _tmp496;struct Cyc_List_List*_tmp497;union Cyc_CfFlowInfo_AbsLVal _tmp498;struct Cyc_CfFlowInfo_Place*_tmp499;_LL280: _tmp494=_tmp493.f1;if((_tmp494.ReachableFL).tag != 2)goto _LL282;_tmp495=(struct _tuple13)(_tmp494.ReachableFL).val;_tmp496=_tmp495.f1;_tmp497=_tmp495.f2;_tmp498=_tmp493.f2;if((_tmp498.PlaceL).tag != 1)goto _LL282;_tmp499=(struct Cyc_CfFlowInfo_Place*)(_tmp498.PlaceL).val;_LL281: {
# 1530
void*_tmp49A=Cyc_CfFlowInfo_lookup_place(_tmp496,_tmp499);
# 1531
void*_tmp49B=_tmp49A;_LL285: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp49C=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp49B;if(_tmp49C->tag != 5)goto _LL287;}_LL286:
# 1532
 return Cyc_NewControlFlow_anal_derefL(env,_tmp490,_tmp47B,_tmp490,_tmp49A,flds);_LL287:;_LL288: {
# 1539
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp459,_tmp496,_tmp49A);
# 1540
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp459->unknown_all: _tmp459->unknown_none;
# 1541
void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp459,Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp47B->topt)),0,leaf);
# 1542
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp945;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp944;void*new_root=(void*)((_tmp944=_region_malloc(_tmp459->r,sizeof(*_tmp944)),((_tmp944[0]=((_tmp945.tag=1,((_tmp945.f1=e,((_tmp945.f2=(void*)((void*)_check_null(e->topt)),_tmp945)))))),_tmp944))));
# 1543
struct Cyc_CfFlowInfo_Place*_tmp946;struct Cyc_CfFlowInfo_Place*place=(_tmp946=_region_malloc(_tmp459->r,sizeof(*_tmp946)),((_tmp946->root=new_root,((_tmp946->fields=0,_tmp946)))));
# 1544
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp949;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp948;void*res=(void*)((_tmp948=_region_malloc(_tmp459->r,sizeof(*_tmp948)),((_tmp948[0]=((_tmp949.tag=5,((_tmp949.f1=place,_tmp949)))),_tmp948))));
# 1545
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
# 1546
_tmp496=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp496,new_root,new_rval);
# 1547
_tmp496=Cyc_CfFlowInfo_assign_place(_tmp459,e->loc,_tmp496,env->all_changed,_tmp499,res);{
# 1548
union Cyc_CfFlowInfo_FlowInfo _tmp49D=Cyc_CfFlowInfo_ReachableFL(_tmp496,_tmp497);
# 1552
return Cyc_NewControlFlow_anal_derefL(env,_tmp49D,_tmp47B,_tmp49D,res,flds);};}_LL284:;}_LL282:;_LL283:
# 1554
 goto _LL27F;_LL27F:;};}{
# 1557
union Cyc_CfFlowInfo_FlowInfo _tmp4A4;void*_tmp4A5;struct _tuple14 _tmp4A3=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp47B);_tmp4A4=_tmp4A3.f1;_tmp4A5=_tmp4A3.f2;
# 1558
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp47B,_tmp4A4,_tmp4A5,flds);};_LL274: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp47C=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp460;if(_tmp47C->tag != 22)goto _LL276;else{_tmp47D=_tmp47C->f1;_tmp47E=_tmp47C->f2;}}_LL275: {
# 1561
void*_tmp4A6=Cyc_Tcutil_compress((void*)_check_null(_tmp47D->topt));struct Cyc_Absyn_PtrInfo _tmp4A9;struct Cyc_Absyn_PtrAtts _tmp4AA;union Cyc_Absyn_Constraint*_tmp4AB;_LL28A: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4A7=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4A6;if(_tmp4A7->tag != 10)goto _LL28C;}_LL28B: {
# 1563
unsigned int _tmp4AC=(Cyc_Evexp_eval_const_uint_exp(_tmp47E)).f1;
# 1564
struct Cyc_List_List*_tmp94A;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp47D,((_tmp94A=_region_malloc(env->r,sizeof(*_tmp94A)),((_tmp94A->hd=(void*)((int)_tmp4AC),((_tmp94A->tl=flds,_tmp94A)))))));}_LL28C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4A8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A6;if(_tmp4A8->tag != 5)goto _LL28E;else{_tmp4A9=_tmp4A8->f1;_tmp4AA=_tmp4A9.ptr_atts;_tmp4AB=_tmp4AA.bounds;}}_LL28D: {
# 1566
struct _RegionHandle _tmp4AE=_new_region("temp");struct _RegionHandle*temp=& _tmp4AE;_push_region(temp);
# 1567
{union Cyc_CfFlowInfo_FlowInfo _tmp4B1;struct Cyc_List_List*_tmp4B2;struct Cyc_Absyn_Exp*_tmp94B[2];struct _tuple20 _tmp4B0=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
# 1568
(_tmp94B[1]=_tmp47E,((_tmp94B[0]=_tmp47D,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp94B,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);_tmp4B1=_tmp4B0.f1;_tmp4B2=_tmp4B0.f2;{
# 1569
union Cyc_CfFlowInfo_FlowInfo _tmp4B3=_tmp4B1;
# 1570
{union Cyc_CfFlowInfo_FlowInfo _tmp4B4=_tmp4B1;struct _tuple13 _tmp4B5;struct Cyc_Dict_Dict _tmp4B6;struct Cyc_List_List*_tmp4B7;_LL291: if((_tmp4B4.ReachableFL).tag != 2)goto _LL293;_tmp4B5=(struct _tuple13)(_tmp4B4.ReachableFL).val;_tmp4B6=_tmp4B5.f1;_tmp4B7=_tmp4B5.f2;_LL292:
# 1572
 if(Cyc_CfFlowInfo_initlevel(_tmp459,_tmp4B6,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp4B2))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
# 1573
const char*_tmp94E;void*_tmp94D;(_tmp94D=0,Cyc_Tcutil_terr(_tmp47E->loc,((_tmp94E="expression may not be initialized",_tag_dyneither(_tmp94E,sizeof(char),34))),_tag_dyneither(_tmp94D,sizeof(void*),0)));}{
# 1574
struct Cyc_List_List*_tmp4BA=Cyc_NewControlFlow_add_subscript_reln(_tmp459->r,_tmp4B7,_tmp47D,_tmp47E);
# 1575
if(_tmp4B7 != _tmp4BA)
# 1576
_tmp4B3=Cyc_CfFlowInfo_ReachableFL(_tmp4B6,_tmp4BA);
# 1577
goto _LL290;};_LL293:;_LL294:
# 1578
 goto _LL290;_LL290:;}{
# 1580
union Cyc_CfFlowInfo_FlowInfo _tmp4BC;union Cyc_CfFlowInfo_AbsLVal _tmp4BD;struct _tuple15 _tmp4BB=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp47D,_tmp4B1,(void*)((struct Cyc_List_List*)_check_null(_tmp4B2))->hd,flds);_tmp4BC=_tmp4BB.f1;_tmp4BD=_tmp4BB.f2;{
# 1581
union Cyc_CfFlowInfo_FlowInfo _tmp4BE=_tmp4BC;int _tmp4BF;_LL296: if((_tmp4BE.BottomFL).tag != 1)goto _LL298;_tmp4BF=(int)(_tmp4BE.BottomFL).val;_LL297: {
# 1582
struct _tuple15 _tmp94F;struct _tuple15 _tmp4C1=(_tmp94F.f1=_tmp4BC,((_tmp94F.f2=_tmp4BD,_tmp94F)));_npop_handler(0);return _tmp4C1;}_LL298:;_LL299: {
# 1583
struct _tuple15 _tmp950;struct _tuple15 _tmp4C3=(_tmp950.f1=_tmp4B3,((_tmp950.f2=_tmp4BD,_tmp950)));_npop_handler(0);return _tmp4C3;}_LL295:;};};};}
# 1567
;_pop_region(temp);}_LL28E:;_LL28F: {
# 1586
struct Cyc_Core_Impossible_exn_struct _tmp956;const char*_tmp955;struct Cyc_Core_Impossible_exn_struct*_tmp954;(int)_throw((void*)((_tmp954=_cycalloc(sizeof(*_tmp954)),((_tmp954[0]=((_tmp956.tag=Cyc_Core_Impossible,((_tmp956.f1=((_tmp955="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp955,sizeof(char),33))),_tmp956)))),_tmp954)))));}_LL289:;}_LL276: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp47F=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp460;if(_tmp47F->tag != 20)goto _LL278;else{_tmp480=_tmp47F->f1;_tmp481=_tmp47F->f2;}}_LL277:
# 1590
 if(Cyc_Absyn_is_nontagged_union_type((void*)_check_null(_tmp480->topt))){
# 1591
struct _tuple15 _tmp957;return(_tmp957.f1=inflow,((_tmp957.f2=Cyc_CfFlowInfo_UnknownL(),_tmp957)));}{
# 1592
struct Cyc_List_List*_tmp958;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,_tmp480,(
# 1593
(_tmp958=_region_malloc(env->r,sizeof(*_tmp958)),((_tmp958->hd=(void*)Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp480->topt),_tmp481),((_tmp958->tl=flds,_tmp958)))))));};_LL278:;_LL279: {
# 1595
struct _tuple15 _tmp959;return(_tmp959.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp959.f2=Cyc_CfFlowInfo_UnknownL(),_tmp959)));}_LL261:;};}
# 1599
static struct _tuple15 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,struct Cyc_Absyn_Exp*e){
# 1601
union Cyc_CfFlowInfo_FlowInfo _tmp4CB;union Cyc_CfFlowInfo_AbsLVal _tmp4CC;struct _tuple15 _tmp4CA=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,e,0);_tmp4CB=_tmp4CA.f1;_tmp4CC=_tmp4CA.f2;{
# 1602
struct _tuple15 _tmp95A;return(_tmp95A.f1=_tmp4CB,((_tmp95A.f2=_tmp4CC,_tmp95A)));};}
# 1607
static struct _tuple16 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 1609
struct Cyc_CfFlowInfo_FlowEnv*_tmp4CE=env->fenv;
# 1610
{void*_tmp4CF=e->r;struct Cyc_Absyn_Exp*_tmp4D1;struct Cyc_Absyn_Exp*_tmp4D2;struct Cyc_Absyn_Exp*_tmp4D3;struct Cyc_Absyn_Exp*_tmp4D5;struct Cyc_Absyn_Exp*_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D8;struct Cyc_Absyn_Exp*_tmp4D9;struct Cyc_Absyn_Exp*_tmp4DB;struct Cyc_Absyn_Exp*_tmp4DC;enum Cyc_Absyn_Primop _tmp4DE;struct Cyc_List_List*_tmp4DF;struct Cyc_List_List _tmp4E0;struct Cyc_Absyn_Exp*_tmp4E1;struct Cyc_List_List*_tmp4E2;enum Cyc_Absyn_Primop _tmp4E4;struct Cyc_List_List*_tmp4E5;_LL29B: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp4D0=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4CF;if(_tmp4D0->tag != 5)goto _LL29D;else{_tmp4D1=_tmp4D0->f1;_tmp4D2=_tmp4D0->f2;_tmp4D3=_tmp4D0->f3;}}_LL29C: {
# 1612
union Cyc_CfFlowInfo_FlowInfo _tmp4E7;union Cyc_CfFlowInfo_FlowInfo _tmp4E8;struct _tuple16 _tmp4E6=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4D1);_tmp4E7=_tmp4E6.f1;_tmp4E8=_tmp4E6.f2;{
# 1613
union Cyc_CfFlowInfo_FlowInfo _tmp4EA;union Cyc_CfFlowInfo_FlowInfo _tmp4EB;struct _tuple16 _tmp4E9=Cyc_NewControlFlow_anal_test(env,_tmp4E7,_tmp4D2);_tmp4EA=_tmp4E9.f1;_tmp4EB=_tmp4E9.f2;{
# 1614
union Cyc_CfFlowInfo_FlowInfo _tmp4ED;union Cyc_CfFlowInfo_FlowInfo _tmp4EE;struct _tuple16 _tmp4EC=Cyc_NewControlFlow_anal_test(env,_tmp4E8,_tmp4D3);_tmp4ED=_tmp4EC.f1;_tmp4EE=_tmp4EC.f2;{
# 1615
struct _tuple16 _tmp95B;return(_tmp95B.f1=Cyc_CfFlowInfo_join_flow(_tmp4CE,env->all_changed,_tmp4EA,_tmp4ED),((_tmp95B.f2=
# 1616
Cyc_CfFlowInfo_join_flow(_tmp4CE,env->all_changed,_tmp4EB,_tmp4EE),_tmp95B)));};};};}_LL29D: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp4D4=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4CF;if(_tmp4D4->tag != 6)goto _LL29F;else{_tmp4D5=_tmp4D4->f1;_tmp4D6=_tmp4D4->f2;}}_LL29E: {
# 1618
union Cyc_CfFlowInfo_FlowInfo _tmp4F1;union Cyc_CfFlowInfo_FlowInfo _tmp4F2;struct _tuple16 _tmp4F0=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4D5);_tmp4F1=_tmp4F0.f1;_tmp4F2=_tmp4F0.f2;{
# 1619
union Cyc_CfFlowInfo_FlowInfo _tmp4F4;union Cyc_CfFlowInfo_FlowInfo _tmp4F5;struct _tuple16 _tmp4F3=Cyc_NewControlFlow_anal_test(env,_tmp4F1,_tmp4D6);_tmp4F4=_tmp4F3.f1;_tmp4F5=_tmp4F3.f2;{
# 1621
struct _tuple16 _tmp95C;return(_tmp95C.f1=_tmp4F4,((_tmp95C.f2=Cyc_CfFlowInfo_join_flow(_tmp4CE,env->all_changed,_tmp4F2,_tmp4F5),_tmp95C)));};};}_LL29F: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp4D7=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4CF;if(_tmp4D7->tag != 7)goto _LL2A1;else{_tmp4D8=_tmp4D7->f1;_tmp4D9=_tmp4D7->f2;}}_LL2A0: {
# 1623
union Cyc_CfFlowInfo_FlowInfo _tmp4F8;union Cyc_CfFlowInfo_FlowInfo _tmp4F9;struct _tuple16 _tmp4F7=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4D8);_tmp4F8=_tmp4F7.f1;_tmp4F9=_tmp4F7.f2;{
# 1624
union Cyc_CfFlowInfo_FlowInfo _tmp4FB;union Cyc_CfFlowInfo_FlowInfo _tmp4FC;struct _tuple16 _tmp4FA=Cyc_NewControlFlow_anal_test(env,_tmp4F9,_tmp4D9);_tmp4FB=_tmp4FA.f1;_tmp4FC=_tmp4FA.f2;{
# 1626
struct _tuple16 _tmp95D;return(_tmp95D.f1=Cyc_CfFlowInfo_join_flow(_tmp4CE,env->all_changed,_tmp4F8,_tmp4FB),((_tmp95D.f2=_tmp4FC,_tmp95D)));};};}_LL2A1: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp4DA=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4CF;if(_tmp4DA->tag != 8)goto _LL2A3;else{_tmp4DB=_tmp4DA->f1;_tmp4DC=_tmp4DA->f2;}}_LL2A2: {
# 1628
union Cyc_CfFlowInfo_FlowInfo _tmp4FF;void*_tmp500;struct _tuple14 _tmp4FE=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp4DB);_tmp4FF=_tmp4FE.f1;_tmp500=_tmp4FE.f2;
# 1629
return Cyc_NewControlFlow_anal_test(env,_tmp4FF,_tmp4DC);}_LL2A3: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4DD=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4CF;if(_tmp4DD->tag != 2)goto _LL2A5;else{_tmp4DE=_tmp4DD->f1;if(_tmp4DE != Cyc_Absyn_Not)goto _LL2A5;_tmp4DF=_tmp4DD->f2;if(_tmp4DF == 0)goto _LL2A5;_tmp4E0=*_tmp4DF;_tmp4E1=(struct Cyc_Absyn_Exp*)_tmp4E0.hd;_tmp4E2=_tmp4E0.tl;if(_tmp4E2 != 0)goto _LL2A5;}}_LL2A4: {
# 1631
union Cyc_CfFlowInfo_FlowInfo _tmp502;union Cyc_CfFlowInfo_FlowInfo _tmp503;struct _tuple16 _tmp501=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4E1);_tmp502=_tmp501.f1;_tmp503=_tmp501.f2;{
# 1632
struct _tuple16 _tmp95E;return(_tmp95E.f1=_tmp503,((_tmp95E.f2=_tmp502,_tmp95E)));};}_LL2A5: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4E3=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4CF;if(_tmp4E3->tag != 2)goto _LL2A7;else{_tmp4E4=_tmp4E3->f1;_tmp4E5=_tmp4E3->f2;}}_LL2A6: {
# 1636
void*r1;void*r2;
# 1637
union Cyc_CfFlowInfo_FlowInfo f;
# 1638
struct _RegionHandle _tmp505=_new_region("temp");struct _RegionHandle*temp=& _tmp505;_push_region(temp);{
# 1639
union Cyc_CfFlowInfo_FlowInfo _tmp507;struct Cyc_List_List*_tmp508;struct _tuple20 _tmp506=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp4E5,0,0);_tmp507=_tmp506.f1;_tmp508=_tmp506.f2;
# 1640
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp508))->hd;
# 1641
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp508->tl))->hd;
# 1642
f=_tmp507;}{
# 1646
union Cyc_CfFlowInfo_FlowInfo _tmp509=f;int _tmp50A;struct _tuple13 _tmp50B;struct Cyc_Dict_Dict _tmp50C;struct Cyc_List_List*_tmp50D;_LL2AA: if((_tmp509.BottomFL).tag != 1)goto _LL2AC;_tmp50A=(int)(_tmp509.BottomFL).val;_LL2AB: {
# 1647
struct _tuple16 _tmp95F;struct _tuple16 _tmp50F=(_tmp95F.f1=f,((_tmp95F.f2=f,_tmp95F)));_npop_handler(0);return _tmp50F;}_LL2AC: if((_tmp509.ReachableFL).tag != 2)goto _LL2A9;_tmp50B=(struct _tuple13)(_tmp509.ReachableFL).val;_tmp50C=_tmp50B.f1;_tmp50D=_tmp50B.f2;_LL2AD: {
# 1649
struct Cyc_Absyn_Exp*_tmp510=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E5))->hd;
# 1650
struct Cyc_Absyn_Exp*_tmp511=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E5->tl))->hd;
# 1651
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp50C,r1)== Cyc_CfFlowInfo_NoneIL){
# 1652
const char*_tmp962;void*_tmp961;(_tmp961=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp4E5->hd)->loc,((_tmp962="expression may not be initialized",_tag_dyneither(_tmp962,sizeof(char),34))),_tag_dyneither(_tmp961,sizeof(void*),0)));}
# 1653
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp50C,r2)== Cyc_CfFlowInfo_NoneIL){
# 1654
const char*_tmp965;void*_tmp964;(_tmp964=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E5->tl))->hd)->loc,((_tmp965="expression may not be initialized",_tag_dyneither(_tmp965,sizeof(char),34))),_tag_dyneither(_tmp964,sizeof(void*),0)));}
# 1658
if(_tmp4E4 == Cyc_Absyn_Eq  || _tmp4E4 == Cyc_Absyn_Neq){
# 1659
struct _tuple0 _tmp966;struct _tuple0 _tmp517=(_tmp966.f1=r1,((_tmp966.f2=r2,_tmp966)));void*_tmp518;enum Cyc_CfFlowInfo_InitLevel _tmp51A;void*_tmp51B;void*_tmp51D;void*_tmp51F;enum Cyc_CfFlowInfo_InitLevel _tmp521;void*_tmp522;void*_tmp524;void*_tmp526;void*_tmp528;void*_tmp52A;void*_tmp52C;void*_tmp52E;void*_tmp530;void*_tmp532;void*_tmp534;void*_tmp536;void*_tmp538;void*_tmp53A;void*_tmp53C;_LL2AF: _tmp518=_tmp517.f1;{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp519=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp518;if(_tmp519->tag != 3)goto _LL2B1;else{_tmp51A=_tmp519->f1;}};_tmp51B=_tmp517.f2;{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp51C=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp51B;if(_tmp51C->tag != 0)goto _LL2B1;};_LL2B0: {
# 1663
union Cyc_CfFlowInfo_FlowInfo _tmp53F;union Cyc_CfFlowInfo_FlowInfo _tmp540;struct _tuple16 _tmp53E=Cyc_NewControlFlow_splitzero(env,f,f,_tmp510,_tmp51A);_tmp53F=_tmp53E.f1;_tmp540=_tmp53E.f2;
# 1664
switch(_tmp4E4){case Cyc_Absyn_Eq: _LL2C3: {
# 1665
struct _tuple16 _tmp967;struct _tuple16 _tmp542=(_tmp967.f1=_tmp540,((_tmp967.f2=_tmp53F,_tmp967)));_npop_handler(0);return _tmp542;}case Cyc_Absyn_Neq: _LL2C4: {
# 1666
struct _tuple16 _tmp968;struct _tuple16 _tmp544=(_tmp968.f1=_tmp53F,((_tmp968.f2=_tmp540,_tmp968)));_npop_handler(0);return _tmp544;}default: _LL2C5: {
# 1667
struct Cyc_Core_Impossible_exn_struct _tmp96E;const char*_tmp96D;struct Cyc_Core_Impossible_exn_struct*_tmp96C;(int)_throw((void*)((_tmp96C=_cycalloc(sizeof(*_tmp96C)),((_tmp96C[0]=((_tmp96E.tag=Cyc_Core_Impossible,((_tmp96E.f1=((_tmp96D="anal_test, zero-split",_tag_dyneither(_tmp96D,sizeof(char),22))),_tmp96E)))),_tmp96C)))));}}}_LL2B1: _tmp51D=_tmp517.f1;{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp51E=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp51D;if(_tmp51E->tag != 0)goto _LL2B3;};_tmp51F=_tmp517.f2;{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp520=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp51F;if(_tmp520->tag != 3)goto _LL2B3;else{_tmp521=_tmp520->f1;}};_LL2B2: {
# 1670
union Cyc_CfFlowInfo_FlowInfo _tmp549;union Cyc_CfFlowInfo_FlowInfo _tmp54A;struct _tuple16 _tmp548=Cyc_NewControlFlow_splitzero(env,f,f,_tmp511,_tmp521);_tmp549=_tmp548.f1;_tmp54A=_tmp548.f2;
# 1671
switch(_tmp4E4){case Cyc_Absyn_Eq: _LL2C7: {
# 1672
struct _tuple16 _tmp96F;struct _tuple16 _tmp54C=(_tmp96F.f1=_tmp54A,((_tmp96F.f2=_tmp549,_tmp96F)));_npop_handler(0);return _tmp54C;}case Cyc_Absyn_Neq: _LL2C8: {
# 1673
struct _tuple16 _tmp970;struct _tuple16 _tmp54E=(_tmp970.f1=_tmp549,((_tmp970.f2=_tmp54A,_tmp970)));_npop_handler(0);return _tmp54E;}default: _LL2C9: {
# 1674
struct Cyc_Core_Impossible_exn_struct _tmp976;const char*_tmp975;struct Cyc_Core_Impossible_exn_struct*_tmp974;(int)_throw((void*)((_tmp974=_cycalloc(sizeof(*_tmp974)),((_tmp974[0]=((_tmp976.tag=Cyc_Core_Impossible,((_tmp976.f1=((_tmp975="anal_test, zero-split",_tag_dyneither(_tmp975,sizeof(char),22))),_tmp976)))),_tmp974)))));}}}_LL2B3: _tmp522=_tmp517.f1;{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp523=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp522;if(_tmp523->tag != 0)goto _LL2B5;};_tmp524=_tmp517.f2;{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp525=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp524;if(_tmp525->tag != 0)goto _LL2B5;};_LL2B4:
# 1677
 if(_tmp4E4 == Cyc_Absyn_Eq){struct _tuple16 _tmp977;struct _tuple16 _tmp553=(_tmp977.f1=f,((_tmp977.f2=Cyc_CfFlowInfo_BottomFL(),_tmp977)));_npop_handler(0);return _tmp553;}else{
# 1678
struct _tuple16 _tmp978;struct _tuple16 _tmp555=(_tmp978.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp978.f2=f,_tmp978)));_npop_handler(0);return _tmp555;}_LL2B5: _tmp526=_tmp517.f1;{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp527=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp526;if(_tmp527->tag != 0)goto _LL2B7;};_tmp528=_tmp517.f2;{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp529=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp528;if(_tmp529->tag != 1)goto _LL2B7;};_LL2B6:
# 1679
 goto _LL2B8;_LL2B7: _tmp52A=_tmp517.f1;{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp52B=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp52A;if(_tmp52B->tag != 0)goto _LL2B9;};_tmp52C=_tmp517.f2;{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp52D=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp52C;if(_tmp52D->tag != 2)goto _LL2B9;};_LL2B8:
# 1680
 goto _LL2BA;_LL2B9: _tmp52E=_tmp517.f1;{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp52F=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp52E;if(_tmp52F->tag != 0)goto _LL2BB;};_tmp530=_tmp517.f2;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp531=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp530;if(_tmp531->tag != 5)goto _LL2BB;};_LL2BA:
# 1681
 goto _LL2BC;_LL2BB: _tmp532=_tmp517.f1;{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp533=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp532;if(_tmp533->tag != 1)goto _LL2BD;};_tmp534=_tmp517.f2;{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp535=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp534;if(_tmp535->tag != 0)goto _LL2BD;};_LL2BC:
# 1682
 goto _LL2BE;_LL2BD: _tmp536=_tmp517.f1;{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp537=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp536;if(_tmp537->tag != 2)goto _LL2BF;};_tmp538=_tmp517.f2;{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp539=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp538;if(_tmp539->tag != 0)goto _LL2BF;};_LL2BE:
# 1683
 goto _LL2C0;_LL2BF: _tmp53A=_tmp517.f1;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp53B=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp53A;if(_tmp53B->tag != 5)goto _LL2C1;};_tmp53C=_tmp517.f2;{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp53D=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp53C;if(_tmp53D->tag != 0)goto _LL2C1;};_LL2C0:
# 1685
 if(_tmp4E4 == Cyc_Absyn_Neq){struct _tuple16 _tmp979;struct _tuple16 _tmp557=(_tmp979.f1=f,((_tmp979.f2=Cyc_CfFlowInfo_BottomFL(),_tmp979)));_npop_handler(0);return _tmp557;}else{
# 1686
struct _tuple16 _tmp97A;struct _tuple16 _tmp559=(_tmp97A.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp97A.f2=f,_tmp97A)));_npop_handler(0);return _tmp559;}_LL2C1:;_LL2C2:
# 1687
 goto _LL2AE;_LL2AE:;}
# 1694
{struct _tuple0 _tmp97B;struct _tuple0 _tmp55B=(_tmp97B.f1=Cyc_Tcutil_compress((void*)_check_null(_tmp510->topt)),((_tmp97B.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp511->topt)),_tmp97B)));void*_tmp55C;enum Cyc_Absyn_Sign _tmp55E;void*_tmp55F;enum Cyc_Absyn_Sign _tmp561;void*_tmp562;void*_tmp564;_LL2CC: _tmp55C=_tmp55B.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp55D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp55C;if(_tmp55D->tag != 6)goto _LL2CE;else{_tmp55E=_tmp55D->f1;if(_tmp55E != Cyc_Absyn_Unsigned)goto _LL2CE;}};_LL2CD:
# 1695
 goto _LL2CF;_LL2CE: _tmp55F=_tmp55B.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp560=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp55F;if(_tmp560->tag != 6)goto _LL2D0;else{_tmp561=_tmp560->f1;if(_tmp561 != Cyc_Absyn_Unsigned)goto _LL2D0;}};_LL2CF:
# 1696
 goto _LL2D1;_LL2D0: _tmp562=_tmp55B.f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp563=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp562;if(_tmp563->tag != 19)goto _LL2D2;};_LL2D1:
# 1697
 goto _LL2D3;_LL2D2: _tmp564=_tmp55B.f2;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp565=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp564;if(_tmp565->tag != 19)goto _LL2D4;};_LL2D3:
# 1698
 goto _LL2CB;_LL2D4:;_LL2D5: {
# 1699
struct _tuple16 _tmp97C;struct _tuple16 _tmp567=(_tmp97C.f1=f,((_tmp97C.f2=f,_tmp97C)));_npop_handler(0);return _tmp567;}_LL2CB:;}
# 1702
switch(_tmp4E4){case Cyc_Absyn_Eq: _LL2D6:
# 1703
 goto _LL2D7;case Cyc_Absyn_Neq: _LL2D7:
# 1704
 goto _LL2D8;case Cyc_Absyn_Gt: _LL2D8:
# 1705
 goto _LL2D9;case Cyc_Absyn_Gte: _LL2D9: {
# 1706
struct _tuple16 _tmp97D;struct _tuple16 _tmp569=(_tmp97D.f1=f,((_tmp97D.f2=f,_tmp97D)));_npop_handler(0);return _tmp569;}case Cyc_Absyn_Lt: _LL2DA: {
# 1708
union Cyc_CfFlowInfo_FlowInfo _tmp56A=f;
# 1709
union Cyc_CfFlowInfo_FlowInfo _tmp56B=f;
# 1711
{union Cyc_CfFlowInfo_FlowInfo _tmp56C=_tmp56A;int _tmp56D;struct _tuple13 _tmp56E;struct Cyc_Dict_Dict _tmp56F;_LL2DD: if((_tmp56C.BottomFL).tag != 1)goto _LL2DF;_tmp56D=(int)(_tmp56C.BottomFL).val;_LL2DE: {
# 1712
struct Cyc_Core_Impossible_exn_struct _tmp983;const char*_tmp982;struct Cyc_Core_Impossible_exn_struct*_tmp981;(int)_throw((void*)((_tmp981=_cycalloc(sizeof(*_tmp981)),((_tmp981[0]=((_tmp983.tag=Cyc_Core_Impossible,((_tmp983.f1=((_tmp982="anal_test, Lt",_tag_dyneither(_tmp982,sizeof(char),14))),_tmp983)))),_tmp981)))));}_LL2DF: if((_tmp56C.ReachableFL).tag != 2)goto _LL2DC;_tmp56E=(struct _tuple13)(_tmp56C.ReachableFL).val;_tmp56F=_tmp56E.f1;_LL2E0:
# 1713
 _tmp50C=_tmp56F;_LL2DC:;}{
# 1717
void*_tmp573=_tmp510->r;void*_tmp575;struct Cyc_Absyn_Vardecl*_tmp577;void*_tmp579;struct Cyc_Absyn_Vardecl*_tmp57B;void*_tmp57D;struct Cyc_Absyn_Vardecl*_tmp57F;void*_tmp581;struct Cyc_Absyn_Vardecl*_tmp583;_LL2E2:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp574=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp573;if(_tmp574->tag != 1)goto _LL2E4;else{_tmp575=(void*)_tmp574->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp576=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp575;if(_tmp576->tag != 1)goto _LL2E4;else{_tmp577=_tmp576->f1;}};}}if(!(!_tmp577->escapes))goto _LL2E4;_LL2E3:
# 1719
 _tmp57B=_tmp577;goto _LL2E5;_LL2E4:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp578=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp573;if(_tmp578->tag != 1)goto _LL2E6;else{_tmp579=(void*)_tmp578->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp57A=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp579;if(_tmp57A->tag != 4)goto _LL2E6;else{_tmp57B=_tmp57A->f1;}};}}if(!(!_tmp57B->escapes))goto _LL2E6;_LL2E5:
# 1720
 _tmp57F=_tmp57B;goto _LL2E7;_LL2E6:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp57C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp573;if(_tmp57C->tag != 1)goto _LL2E8;else{_tmp57D=(void*)_tmp57C->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp57E=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp57D;if(_tmp57E->tag != 5)goto _LL2E8;else{_tmp57F=_tmp57E->f1;}};}}if(!(!_tmp57F->escapes))goto _LL2E8;_LL2E7:
# 1721
 _tmp583=_tmp57F;goto _LL2E9;_LL2E8:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp580=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp573;if(_tmp580->tag != 1)goto _LL2EA;else{_tmp581=(void*)_tmp580->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp582=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp581;if(_tmp582->tag != 3)goto _LL2EA;else{_tmp583=_tmp582->f1;}};}}if(!(!_tmp583->escapes))goto _LL2EA;_LL2E9: {
# 1723
void*_tmp584=_tmp511->r;void*_tmp586;struct Cyc_Absyn_Vardecl*_tmp588;void*_tmp58A;struct Cyc_Absyn_Vardecl*_tmp58C;void*_tmp58E;struct Cyc_Absyn_Vardecl*_tmp590;void*_tmp592;struct Cyc_Absyn_Vardecl*_tmp594;union Cyc_Absyn_Cnst _tmp596;struct _tuple6 _tmp597;int _tmp598;struct Cyc_Absyn_Exp*_tmp59A;struct Cyc_Absyn_Exp _tmp59B;void*_tmp59C;union Cyc_Absyn_Cnst _tmp59E;struct _tuple6 _tmp59F;int _tmp5A0;enum Cyc_Absyn_Primop _tmp5A2;struct Cyc_List_List*_tmp5A3;struct Cyc_List_List _tmp5A4;struct Cyc_Absyn_Exp*_tmp5A5;_LL2ED:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp585=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp584;if(_tmp585->tag != 1)goto _LL2EF;else{_tmp586=(void*)_tmp585->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp587=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp586;if(_tmp587->tag != 1)goto _LL2EF;else{_tmp588=_tmp587->f1;}};}}if(!(!_tmp588->escapes))goto _LL2EF;_LL2EE:
# 1724
 _tmp58C=_tmp588;goto _LL2F0;_LL2EF:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp589=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp584;if(_tmp589->tag != 1)goto _LL2F1;else{_tmp58A=(void*)_tmp589->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp58B=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp58A;if(_tmp58B->tag != 4)goto _LL2F1;else{_tmp58C=_tmp58B->f1;}};}}if(!(!_tmp58C->escapes))goto _LL2F1;_LL2F0:
# 1725
 _tmp590=_tmp58C;goto _LL2F2;_LL2F1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp58D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp584;if(_tmp58D->tag != 1)goto _LL2F3;else{_tmp58E=(void*)_tmp58D->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp58F=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp58E;if(_tmp58F->tag != 5)goto _LL2F3;else{_tmp590=_tmp58F->f1;}};}}if(!(!_tmp590->escapes))goto _LL2F3;_LL2F2:
# 1726
 _tmp594=_tmp590;goto _LL2F4;_LL2F3:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp591=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp584;if(_tmp591->tag != 1)goto _LL2F5;else{_tmp592=(void*)_tmp591->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp593=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp592;if(_tmp593->tag != 3)goto _LL2F5;else{_tmp594=_tmp593->f1;}};}}if(!(!_tmp594->escapes))goto _LL2F5;_LL2F4: {
# 1730
struct _RegionHandle*_tmp5A6=(env->fenv)->r;
# 1731
struct Cyc_CfFlowInfo_Reln*_tmp986;struct Cyc_List_List*_tmp985;struct Cyc_List_List*_tmp5A7=(_tmp985=_region_malloc(_tmp5A6,sizeof(*_tmp985)),((_tmp985->hd=((_tmp986=_region_malloc(_tmp5A6,sizeof(*_tmp986)),((_tmp986->vd=_tmp583,((_tmp986->rop=Cyc_CfFlowInfo_LessVar(_tmp594,_tmp594->type),_tmp986)))))),((_tmp985->tl=_tmp50D,_tmp985)))));
# 1732
struct _tuple16 _tmp987;struct _tuple16 _tmp5A9=(_tmp987.f1=Cyc_CfFlowInfo_ReachableFL(_tmp50C,_tmp5A7),((_tmp987.f2=_tmp56B,_tmp987)));_npop_handler(0);return _tmp5A9;}_LL2F5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp595=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp584;if(_tmp595->tag != 0)goto _LL2F7;else{_tmp596=_tmp595->f1;if((_tmp596.Int_c).tag != 5)goto _LL2F7;_tmp597=(struct _tuple6)(_tmp596.Int_c).val;_tmp598=_tmp597.f2;}}_LL2F6:
# 1733
 _tmp5A0=_tmp598;goto _LL2F8;_LL2F7: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp599=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp584;if(_tmp599->tag != 13)goto _LL2F9;else{_tmp59A=_tmp599->f2;_tmp59B=*_tmp59A;_tmp59C=_tmp59B.r;{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp59D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp59C;if(_tmp59D->tag != 0)goto _LL2F9;else{_tmp59E=_tmp59D->f1;if((_tmp59E.Int_c).tag != 5)goto _LL2F9;_tmp59F=(struct _tuple6)(_tmp59E.Int_c).val;_tmp5A0=_tmp59F.f2;}};}}_LL2F8: {
# 1735
struct _RegionHandle*_tmp5AC=(env->fenv)->r;
# 1736
struct Cyc_CfFlowInfo_Reln*_tmp98A;struct Cyc_List_List*_tmp989;struct Cyc_List_List*_tmp5AD=(_tmp989=_region_malloc(_tmp5AC,sizeof(*_tmp989)),((_tmp989->hd=((_tmp98A=_region_malloc(_tmp5AC,sizeof(*_tmp98A)),((_tmp98A->vd=_tmp583,((_tmp98A->rop=Cyc_CfFlowInfo_LessConst((unsigned int)_tmp5A0),_tmp98A)))))),((_tmp989->tl=_tmp50D,_tmp989)))));
# 1737
struct _tuple16 _tmp98B;struct _tuple16 _tmp5AF=(_tmp98B.f1=Cyc_CfFlowInfo_ReachableFL(_tmp50C,_tmp5AD),((_tmp98B.f2=_tmp56B,_tmp98B)));_npop_handler(0);return _tmp5AF;}_LL2F9: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp5A1=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp584;if(_tmp5A1->tag != 2)goto _LL2FB;else{_tmp5A2=_tmp5A1->f1;_tmp5A3=_tmp5A1->f2;if(_tmp5A3 == 0)goto _LL2FB;_tmp5A4=*_tmp5A3;_tmp5A5=(struct Cyc_Absyn_Exp*)_tmp5A4.hd;}}_LL2FA: {
# 1739
void*_tmp5B2=_tmp5A5->r;void*_tmp5B4;struct Cyc_Absyn_Vardecl*_tmp5B6;void*_tmp5B8;struct Cyc_Absyn_Vardecl*_tmp5BA;void*_tmp5BC;struct Cyc_Absyn_Vardecl*_tmp5BE;void*_tmp5C0;struct Cyc_Absyn_Vardecl*_tmp5C2;_LL2FE:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp5B3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5B2;if(_tmp5B3->tag != 1)goto _LL300;else{_tmp5B4=(void*)_tmp5B3->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp5B5=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5B4;if(_tmp5B5->tag != 1)goto _LL300;else{_tmp5B6=_tmp5B5->f1;}};}}if(!(!_tmp5B6->escapes))goto _LL300;_LL2FF:
# 1740
 _tmp5BA=_tmp5B6;goto _LL301;_LL300:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp5B7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5B2;if(_tmp5B7->tag != 1)goto _LL302;else{_tmp5B8=(void*)_tmp5B7->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp5B9=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5B8;if(_tmp5B9->tag != 4)goto _LL302;else{_tmp5BA=_tmp5B9->f1;}};}}if(!(!_tmp5BA->escapes))goto _LL302;_LL301:
# 1741
 _tmp5BE=_tmp5BA;goto _LL303;_LL302:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp5BB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5B2;if(_tmp5BB->tag != 1)goto _LL304;else{_tmp5BC=(void*)_tmp5BB->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp5BD=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp5BC;if(_tmp5BD->tag != 5)goto _LL304;else{_tmp5BE=_tmp5BD->f1;}};}}if(!(!_tmp5BE->escapes))goto _LL304;_LL303:
# 1742
 _tmp5C2=_tmp5BE;goto _LL305;_LL304:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp5BF=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5B2;if(_tmp5BF->tag != 1)goto _LL306;else{_tmp5C0=(void*)_tmp5BF->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp5C1=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp5C0;if(_tmp5C1->tag != 3)goto _LL306;else{_tmp5C2=_tmp5C1->f1;}};}}if(!(!_tmp5C2->escapes))goto _LL306;_LL305: {
# 1745
struct _RegionHandle*_tmp5C3=(env->fenv)->r;
# 1746
struct Cyc_CfFlowInfo_Reln*_tmp98E;struct Cyc_List_List*_tmp98D;struct Cyc_List_List*_tmp5C4=(_tmp98D=_region_malloc(_tmp5C3,sizeof(*_tmp98D)),((_tmp98D->hd=((_tmp98E=_region_malloc(_tmp5C3,sizeof(*_tmp98E)),((_tmp98E->vd=_tmp583,((_tmp98E->rop=Cyc_CfFlowInfo_LessNumelts(_tmp5C2),_tmp98E)))))),((_tmp98D->tl=_tmp50D,_tmp98D)))));
# 1747
struct _tuple16 _tmp98F;struct _tuple16 _tmp5C6=(_tmp98F.f1=Cyc_CfFlowInfo_ReachableFL(_tmp50C,_tmp5C4),((_tmp98F.f2=_tmp56B,_tmp98F)));_npop_handler(0);return _tmp5C6;}_LL306:;_LL307: {
# 1748
struct _tuple16 _tmp990;struct _tuple16 _tmp5CA=(_tmp990.f1=_tmp56A,((_tmp990.f2=_tmp56B,_tmp990)));_npop_handler(0);return _tmp5CA;}_LL2FD:;}_LL2FB:;_LL2FC: {
# 1750
struct _tuple16 _tmp991;struct _tuple16 _tmp5CC=(_tmp991.f1=_tmp56A,((_tmp991.f2=_tmp56B,_tmp991)));_npop_handler(0);return _tmp5CC;}_LL2EC:;}_LL2EA:;_LL2EB: {
# 1752
struct _tuple16 _tmp992;struct _tuple16 _tmp5CE=(_tmp992.f1=_tmp56A,((_tmp992.f2=_tmp56B,_tmp992)));_npop_handler(0);return _tmp5CE;}_LL2E1:;};}case Cyc_Absyn_Lte: _LL2DB: {
# 1756
union Cyc_CfFlowInfo_FlowInfo _tmp5CF=f;
# 1757
union Cyc_CfFlowInfo_FlowInfo _tmp5D0=f;
# 1759
{union Cyc_CfFlowInfo_FlowInfo _tmp5D1=_tmp5CF;int _tmp5D2;struct _tuple13 _tmp5D3;struct Cyc_Dict_Dict _tmp5D4;_LL30A: if((_tmp5D1.BottomFL).tag != 1)goto _LL30C;_tmp5D2=(int)(_tmp5D1.BottomFL).val;_LL30B: {
# 1760
struct Cyc_Core_Impossible_exn_struct _tmp998;const char*_tmp997;struct Cyc_Core_Impossible_exn_struct*_tmp996;(int)_throw((void*)((_tmp996=_cycalloc(sizeof(*_tmp996)),((_tmp996[0]=((_tmp998.tag=Cyc_Core_Impossible,((_tmp998.f1=((_tmp997="anal_test, Lte",_tag_dyneither(_tmp997,sizeof(char),15))),_tmp998)))),_tmp996)))));}_LL30C: if((_tmp5D1.ReachableFL).tag != 2)goto _LL309;_tmp5D3=(struct _tuple13)(_tmp5D1.ReachableFL).val;_tmp5D4=_tmp5D3.f1;_LL30D:
# 1761
 _tmp50C=_tmp5D4;_LL309:;}{
# 1765
void*_tmp5D8=_tmp510->r;void*_tmp5DA;struct Cyc_Absyn_Vardecl*_tmp5DC;void*_tmp5DE;struct Cyc_Absyn_Vardecl*_tmp5E0;void*_tmp5E2;struct Cyc_Absyn_Vardecl*_tmp5E4;void*_tmp5E6;struct Cyc_Absyn_Vardecl*_tmp5E8;_LL30F:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp5D9=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5D8;if(_tmp5D9->tag != 1)goto _LL311;else{_tmp5DA=(void*)_tmp5D9->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp5DB=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5DA;if(_tmp5DB->tag != 1)goto _LL311;else{_tmp5DC=_tmp5DB->f1;}};}}if(!(!_tmp5DC->escapes))goto _LL311;_LL310:
# 1767
 _tmp5E0=_tmp5DC;goto _LL312;_LL311:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp5DD=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5D8;if(_tmp5DD->tag != 1)goto _LL313;else{_tmp5DE=(void*)_tmp5DD->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp5DF=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5DE;if(_tmp5DF->tag != 4)goto _LL313;else{_tmp5E0=_tmp5DF->f1;}};}}if(!(!_tmp5E0->escapes))goto _LL313;_LL312:
# 1768
 _tmp5E4=_tmp5E0;goto _LL314;_LL313:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp5E1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5D8;if(_tmp5E1->tag != 1)goto _LL315;else{_tmp5E2=(void*)_tmp5E1->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp5E3=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp5E2;if(_tmp5E3->tag != 5)goto _LL315;else{_tmp5E4=_tmp5E3->f1;}};}}if(!(!_tmp5E4->escapes))goto _LL315;_LL314:
# 1769
 _tmp5E8=_tmp5E4;goto _LL316;_LL315:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp5E5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5D8;if(_tmp5E5->tag != 1)goto _LL317;else{_tmp5E6=(void*)_tmp5E5->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp5E7=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp5E6;if(_tmp5E7->tag != 3)goto _LL317;else{_tmp5E8=_tmp5E7->f1;}};}}if(!(!_tmp5E8->escapes))goto _LL317;_LL316: {
# 1771
void*_tmp5E9=_tmp511->r;union Cyc_Absyn_Cnst _tmp5EB;struct _tuple6 _tmp5EC;int _tmp5ED;struct Cyc_Absyn_Exp*_tmp5EF;struct Cyc_Absyn_Exp _tmp5F0;void*_tmp5F1;union Cyc_Absyn_Cnst _tmp5F3;struct _tuple6 _tmp5F4;int _tmp5F5;enum Cyc_Absyn_Primop _tmp5F7;struct Cyc_List_List*_tmp5F8;struct Cyc_List_List _tmp5F9;struct Cyc_Absyn_Exp*_tmp5FA;_LL31A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5EA=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E9;if(_tmp5EA->tag != 0)goto _LL31C;else{_tmp5EB=_tmp5EA->f1;if((_tmp5EB.Int_c).tag != 5)goto _LL31C;_tmp5EC=(struct _tuple6)(_tmp5EB.Int_c).val;_tmp5ED=_tmp5EC.f2;}}_LL31B:
# 1772
 _tmp5F5=_tmp5ED;goto _LL31D;_LL31C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp5EE=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5E9;if(_tmp5EE->tag != 13)goto _LL31E;else{_tmp5EF=_tmp5EE->f2;_tmp5F0=*_tmp5EF;_tmp5F1=_tmp5F0.r;{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5F2=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5F1;if(_tmp5F2->tag != 0)goto _LL31E;else{_tmp5F3=_tmp5F2->f1;if((_tmp5F3.Int_c).tag != 5)goto _LL31E;_tmp5F4=(struct _tuple6)(_tmp5F3.Int_c).val;_tmp5F5=_tmp5F4.f2;}};}}_LL31D: {
# 1774
struct _RegionHandle*_tmp5FB=(env->fenv)->r;
# 1775
struct Cyc_CfFlowInfo_Reln*_tmp99B;struct Cyc_List_List*_tmp99A;struct Cyc_List_List*_tmp5FC=(_tmp99A=_region_malloc(_tmp5FB,sizeof(*_tmp99A)),((_tmp99A->hd=(
# 1776
(_tmp99B=_region_malloc(_tmp5FB,sizeof(*_tmp99B)),((_tmp99B->vd=_tmp5E8,((_tmp99B->rop=Cyc_CfFlowInfo_LessEqConst((unsigned int)_tmp5F5),_tmp99B)))))),((_tmp99A->tl=_tmp50D,_tmp99A)))));
# 1777
struct _tuple16 _tmp99C;struct _tuple16 _tmp5FE=(_tmp99C.f1=Cyc_CfFlowInfo_ReachableFL(_tmp50C,_tmp5FC),((_tmp99C.f2=_tmp5D0,_tmp99C)));_npop_handler(0);return _tmp5FE;}_LL31E: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp5F6=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5E9;if(_tmp5F6->tag != 2)goto _LL320;else{_tmp5F7=_tmp5F6->f1;_tmp5F8=_tmp5F6->f2;if(_tmp5F8 == 0)goto _LL320;_tmp5F9=*_tmp5F8;_tmp5FA=(struct Cyc_Absyn_Exp*)_tmp5F9.hd;}}_LL31F: {
# 1779
void*_tmp601=_tmp5FA->r;void*_tmp603;struct Cyc_Absyn_Vardecl*_tmp605;void*_tmp607;struct Cyc_Absyn_Vardecl*_tmp609;void*_tmp60B;struct Cyc_Absyn_Vardecl*_tmp60D;void*_tmp60F;struct Cyc_Absyn_Vardecl*_tmp611;_LL323:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp602=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp601;if(_tmp602->tag != 1)goto _LL325;else{_tmp603=(void*)_tmp602->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp604=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp603;if(_tmp604->tag != 1)goto _LL325;else{_tmp605=_tmp604->f1;}};}}if(!(!_tmp605->escapes))goto _LL325;_LL324:
# 1780
 _tmp609=_tmp605;goto _LL326;_LL325:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp606=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp601;if(_tmp606->tag != 1)goto _LL327;else{_tmp607=(void*)_tmp606->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp608=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp607;if(_tmp608->tag != 4)goto _LL327;else{_tmp609=_tmp608->f1;}};}}if(!(!_tmp609->escapes))goto _LL327;_LL326:
# 1781
 _tmp60D=_tmp609;goto _LL328;_LL327:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp60A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp601;if(_tmp60A->tag != 1)goto _LL329;else{_tmp60B=(void*)_tmp60A->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp60C=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp60B;if(_tmp60C->tag != 5)goto _LL329;else{_tmp60D=_tmp60C->f1;}};}}if(!(!_tmp60D->escapes))goto _LL329;_LL328:
# 1782
 _tmp611=_tmp60D;goto _LL32A;_LL329:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp60E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp601;if(_tmp60E->tag != 1)goto _LL32B;else{_tmp60F=(void*)_tmp60E->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp610=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp60F;if(_tmp610->tag != 3)goto _LL32B;else{_tmp611=_tmp610->f1;}};}}if(!(!_tmp611->escapes))goto _LL32B;_LL32A: {
# 1785
struct Cyc_CfFlowInfo_FlowEnv*_tmp612=env->fenv;
# 1786
struct Cyc_CfFlowInfo_Reln*_tmp99F;struct Cyc_List_List*_tmp99E;struct Cyc_List_List*_tmp613=(_tmp99E=_region_malloc(_tmp612->r,sizeof(*_tmp99E)),((_tmp99E->hd=((_tmp99F=_region_malloc(_tmp612->r,sizeof(*_tmp99F)),((_tmp99F->vd=_tmp5E8,((_tmp99F->rop=Cyc_CfFlowInfo_LessEqNumelts(_tmp611),_tmp99F)))))),((_tmp99E->tl=_tmp50D,_tmp99E)))));
# 1787
struct _tuple16 _tmp9A0;struct _tuple16 _tmp615=(_tmp9A0.f1=Cyc_CfFlowInfo_ReachableFL(_tmp50C,_tmp613),((_tmp9A0.f2=_tmp5D0,_tmp9A0)));_npop_handler(0);return _tmp615;}_LL32B:;_LL32C: {
# 1788
struct _tuple16 _tmp9A1;struct _tuple16 _tmp619=(_tmp9A1.f1=_tmp5CF,((_tmp9A1.f2=_tmp5D0,_tmp9A1)));_npop_handler(0);return _tmp619;}_LL322:;}_LL320:;_LL321: {
# 1790
struct _tuple16 _tmp9A2;struct _tuple16 _tmp61B=(_tmp9A2.f1=_tmp5CF,((_tmp9A2.f2=_tmp5D0,_tmp9A2)));_npop_handler(0);return _tmp61B;}_LL319:;}_LL317:;_LL318: {
# 1792
struct _tuple16 _tmp9A3;struct _tuple16 _tmp61D=(_tmp9A3.f1=_tmp5CF,((_tmp9A3.f2=_tmp5D0,_tmp9A3)));_npop_handler(0);return _tmp61D;}_LL30E:;};}default: _LL308: {
# 1794
struct _tuple16 _tmp9A4;struct _tuple16 _tmp61F=(_tmp9A4.f1=f,((_tmp9A4.f2=f,_tmp9A4)));_npop_handler(0);return _tmp61F;}}}_LL2A9:;};
# 1638
;_pop_region(temp);}_LL2A7:;_LL2A8:
# 1797
 goto _LL29A;_LL29A:;}{
# 1801
union Cyc_CfFlowInfo_FlowInfo _tmp621;void*_tmp622;struct _tuple14 _tmp620=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);_tmp621=_tmp620.f1;_tmp622=_tmp620.f2;{
# 1802
union Cyc_CfFlowInfo_FlowInfo _tmp623=_tmp621;int _tmp624;struct _tuple13 _tmp625;struct Cyc_Dict_Dict _tmp626;_LL32F: if((_tmp623.BottomFL).tag != 1)goto _LL331;_tmp624=(int)(_tmp623.BottomFL).val;_LL330: {
# 1803
struct _tuple16 _tmp9A5;return(_tmp9A5.f1=_tmp621,((_tmp9A5.f2=_tmp621,_tmp9A5)));}_LL331: if((_tmp623.ReachableFL).tag != 2)goto _LL32E;_tmp625=(struct _tuple13)(_tmp623.ReachableFL).val;_tmp626=_tmp625.f1;_LL332: {
# 1805
void*_tmp628=_tmp622;enum Cyc_CfFlowInfo_InitLevel _tmp62E;enum Cyc_CfFlowInfo_InitLevel _tmp630;enum Cyc_CfFlowInfo_InitLevel _tmp633;_LL334: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp629=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp628;if(_tmp629->tag != 0)goto _LL336;}_LL335: {
# 1806
struct _tuple16 _tmp9A6;return(_tmp9A6.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp9A6.f2=_tmp621,_tmp9A6)));}_LL336: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp62A=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp628;if(_tmp62A->tag != 2)goto _LL338;}_LL337:
# 1807
 goto _LL339;_LL338: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp62B=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp628;if(_tmp62B->tag != 1)goto _LL33A;}_LL339:
# 1808
 goto _LL33B;_LL33A: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp62C=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp628;if(_tmp62C->tag != 5)goto _LL33C;}_LL33B: {
# 1809
struct _tuple16 _tmp9A7;return(_tmp9A7.f1=_tmp621,((_tmp9A7.f2=Cyc_CfFlowInfo_BottomFL(),_tmp9A7)));}_LL33C: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp62D=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp628;if(_tmp62D->tag != 3)goto _LL33E;else{_tmp62E=_tmp62D->f1;if(_tmp62E != Cyc_CfFlowInfo_NoneIL)goto _LL33E;}}_LL33D:
# 1810
 goto _LL33F;_LL33E: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp62F=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp628;if(_tmp62F->tag != 4)goto _LL340;else{_tmp630=_tmp62F->f1;if(_tmp630 != Cyc_CfFlowInfo_NoneIL)goto _LL340;}}_LL33F:
# 1811
 goto _LL341;_LL340: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp631=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp628;if(_tmp631->tag != 7)goto _LL342;}_LL341:
# 1813
{const char*_tmp9AA;void*_tmp9A9;(_tmp9A9=0,Cyc_Tcutil_terr(e->loc,((_tmp9AA="expression may not be initialized",_tag_dyneither(_tmp9AA,sizeof(char),34))),_tag_dyneither(_tmp9A9,sizeof(void*),0)));}{
# 1814
struct _tuple16 _tmp9AB;return(_tmp9AB.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp9AB.f2=Cyc_CfFlowInfo_BottomFL(),_tmp9AB)));};_LL342: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp632=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp628;if(_tmp632->tag != 3)goto _LL344;else{_tmp633=_tmp632->f1;}}_LL343:
# 1816
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp621,e,_tmp633);_LL344: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp634=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp628;if(_tmp634->tag != 4)goto _LL346;}_LL345: {
# 1817
struct _tuple16 _tmp9AC;return(_tmp9AC.f1=_tmp621,((_tmp9AC.f2=_tmp621,_tmp9AC)));}_LL346: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp635=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp628;if(_tmp635->tag != 6)goto _LL333;}_LL347: {
# 1818
struct Cyc_Core_Impossible_exn_struct _tmp9B2;const char*_tmp9B1;struct Cyc_Core_Impossible_exn_struct*_tmp9B0;(int)_throw((void*)((_tmp9B0=_cycalloc(sizeof(*_tmp9B0)),((_tmp9B0[0]=((_tmp9B2.tag=Cyc_Core_Impossible,((_tmp9B2.f1=((_tmp9B1="anal_test",_tag_dyneither(_tmp9B1,sizeof(char),10))),_tmp9B2)))),_tmp9B0)))));}_LL333:;}_LL32E:;};};}struct _tuple23{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 1823
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple23*ckenv,void*root,void*rval){
# 1825
struct _tuple23 _tmp640;unsigned int _tmp641;struct Cyc_NewControlFlow_AnalEnv*_tmp642;struct Cyc_Dict_Dict _tmp643;struct _tuple23*_tmp63F=ckenv;_tmp640=*_tmp63F;_tmp641=_tmp640.f1;_tmp642=_tmp640.f2;_tmp643=_tmp640.f3;{
# 1826
void*_tmp644=root;struct Cyc_Absyn_Vardecl*_tmp646;_LL349: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp645=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp644;if(_tmp645->tag != 0)goto _LL34B;else{_tmp646=_tmp645->f1;}}_LL34A:
# 1828
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp642->fenv)->r,_tmp646->type)){
# 1829
void*_tmp647=rval;enum Cyc_CfFlowInfo_InitLevel _tmp64B;_LL34E: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp648=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp647;if(_tmp648->tag != 7)goto _LL350;}_LL34F:
# 1830
 goto _LL351;_LL350: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp649=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp647;if(_tmp649->tag != 0)goto _LL352;}_LL351:
# 1831
 goto _LL353;_LL352: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp64A=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp647;if(_tmp64A->tag != 3)goto _LL354;else{_tmp64B=_tmp64A->f1;if(_tmp64B != Cyc_CfFlowInfo_NoneIL)goto _LL354;}}_LL353:
# 1832
 goto _LL34D;_LL354:;_LL355:
# 1834
{const char*_tmp9B6;void*_tmp9B5[1];struct Cyc_String_pa_PrintArg_struct _tmp9B4;(_tmp9B4.tag=0,((_tmp9B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp646->name)),((_tmp9B5[0]=& _tmp9B4,Cyc_Tcutil_warn(_tmp641,((_tmp9B6="unique pointers reachable from %s may become unreachable",_tag_dyneither(_tmp9B6,sizeof(char),57))),_tag_dyneither(_tmp9B5,sizeof(void*),1)))))));}
# 1835
goto _LL34D;_LL34D:;}
# 1837
goto _LL348;_LL34B:;_LL34C:
# 1838
 goto _LL348;_LL348:;};}
# 1842
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
# 1843
union Cyc_CfFlowInfo_FlowInfo _tmp64F=flow;int _tmp650;struct _tuple13 _tmp651;struct Cyc_Dict_Dict _tmp652;_LL357: if((_tmp64F.BottomFL).tag != 1)goto _LL359;_tmp650=(int)(_tmp64F.BottomFL).val;_LL358:
# 1844
 return;_LL359: if((_tmp64F.ReachableFL).tag != 2)goto _LL356;_tmp651=(struct _tuple13)(_tmp64F.ReachableFL).val;_tmp652=_tmp651.f1;_LL35A:
# 1846
{struct Cyc_List_List*_tmp653=env->param_roots;for(0;_tmp653 != 0;_tmp653=_tmp653->tl){
# 1847
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp652,Cyc_CfFlowInfo_lookup_place(_tmp652,(struct Cyc_CfFlowInfo_Place*)_tmp653->hd))!= Cyc_CfFlowInfo_AllIL){
# 1848
const char*_tmp9B9;void*_tmp9B8;(_tmp9B8=0,Cyc_Tcutil_terr(loc,((_tmp9B9="function may not initialize all the parameters with attribute 'initializes'",_tag_dyneither(_tmp9B9,sizeof(char),76))),_tag_dyneither(_tmp9B8,sizeof(void*),0)));}}}
# 1850
if(Cyc_NewControlFlow_warn_lose_unique){
# 1851
struct _tuple23 _tmp9BA;struct _tuple23 _tmp656=(_tmp9BA.f1=loc,((_tmp9BA.f2=env,((_tmp9BA.f3=_tmp652,_tmp9BA)))));
# 1852
((void(*)(void(*f)(struct _tuple23*,void*,void*),struct _tuple23*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp656,_tmp652);}
# 1854
return;_LL356:;}
# 1861
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs){
# 1863
struct Cyc_CfFlowInfo_FlowEnv*_tmp658=env->fenv;
# 1864
for(0;scs != 0;scs=scs->tl){
# 1865
struct Cyc_Absyn_Switch_clause _tmp65A;struct Cyc_Core_Opt*_tmp65B;struct Cyc_Absyn_Exp*_tmp65C;struct Cyc_Absyn_Stmt*_tmp65D;unsigned int _tmp65E;struct Cyc_Absyn_Switch_clause*_tmp659=(struct Cyc_Absyn_Switch_clause*)scs->hd;_tmp65A=*_tmp659;_tmp65B=_tmp65A.pat_vars;_tmp65C=_tmp65A.where_clause;_tmp65D=_tmp65A.body;_tmp65E=_tmp65A.loc;{
# 1866
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_add_vars(_tmp658,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp65B))->v,_tmp658->unknown_all,_tmp65E);
# 1867
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
# 1868
if(_tmp65C != 0){
# 1869
struct Cyc_Absyn_Exp*wexp=(struct Cyc_Absyn_Exp*)_tmp65C;
# 1870
union Cyc_CfFlowInfo_FlowInfo _tmp660;union Cyc_CfFlowInfo_FlowInfo _tmp661;struct _tuple16 _tmp65F=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);_tmp660=_tmp65F.f1;_tmp661=_tmp65F.f2;
# 1871
inflow=_tmp661;
# 1872
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp660,_tmp65D);}else{
# 1874
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp65D);}{
# 1876
union Cyc_CfFlowInfo_FlowInfo _tmp662=clause_outflow;int _tmp663;_LL35C: if((_tmp662.BottomFL).tag != 1)goto _LL35E;_tmp663=(int)(_tmp662.BottomFL).val;_LL35D:
# 1877
 goto _LL35B;_LL35E:;_LL35F:
# 1880
 if(scs->tl == 0)
# 1881
return clause_outflow;else{
# 1885
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){
# 1886
const char*_tmp9BD;void*_tmp9BC;(_tmp9BC=0,Cyc_Tcutil_terr(_tmp65D->loc,((_tmp9BD="switch clause may implicitly fallthru",_tag_dyneither(_tmp9BD,sizeof(char),38))),_tag_dyneither(_tmp9BC,sizeof(void*),0)));}else{
# 1888
const char*_tmp9C0;void*_tmp9BF;(_tmp9BF=0,Cyc_Tcutil_warn(_tmp65D->loc,((_tmp9C0="switch clause may implicitly fallthru",_tag_dyneither(_tmp9C0,sizeof(char),38))),_tag_dyneither(_tmp9BF,sizeof(void*),0)));}
# 1890
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 1892
goto _LL35B;_LL35B:;};};}
# 1895
return Cyc_CfFlowInfo_BottomFL();}struct _tuple24{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 1900
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple24*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 1902
struct _tuple24 _tmp669;struct Cyc_NewControlFlow_AnalEnv*_tmp66A;struct Cyc_Dict_Dict _tmp66B;unsigned int _tmp66C;struct _tuple24*_tmp668=vdenv;_tmp669=*_tmp668;_tmp66A=_tmp669.f1;_tmp66B=_tmp669.f2;_tmp66C=_tmp669.f3;
# 1903
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp66A->fenv)->r,vd->type)){
# 1905
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp9C1;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp9C1.tag=0,((_tmp9C1.f1=vd,_tmp9C1)));
# 1907
void*_tmp66D=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp66B,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot);
# 1908
void*_tmp66E=_tmp66D;enum Cyc_CfFlowInfo_InitLevel _tmp672;_LL361: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp66F=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp66E;if(_tmp66F->tag != 7)goto _LL363;}_LL362:
# 1909
 goto _LL364;_LL363: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp670=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp66E;if(_tmp670->tag != 0)goto _LL365;}_LL364:
# 1910
 goto _LL366;_LL365: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp671=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp66E;if(_tmp671->tag != 3)goto _LL367;else{_tmp672=_tmp671->f1;if(_tmp672 != Cyc_CfFlowInfo_NoneIL)goto _LL367;}}_LL366:
# 1911
 goto _LL360;_LL367:;_LL368:
# 1913
{const char*_tmp9C5;void*_tmp9C4[1];struct Cyc_String_pa_PrintArg_struct _tmp9C3;(_tmp9C3.tag=0,((_tmp9C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp9C4[0]=& _tmp9C3,Cyc_Tcutil_warn(_tmp66C,((_tmp9C5="unique pointers may still exist after variable %s goes out of scope",_tag_dyneither(_tmp9C5,sizeof(char),68))),_tag_dyneither(_tmp9C4,sizeof(void*),1)))))));}
# 1915
goto _LL360;_LL360:;}}
# 1920
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 1922
{union Cyc_CfFlowInfo_FlowInfo _tmp677=inflow;struct _tuple13 _tmp678;struct Cyc_Dict_Dict _tmp679;_LL36A: if((_tmp677.ReachableFL).tag != 2)goto _LL36C;_tmp678=(struct _tuple13)(_tmp677.ReachableFL).val;_tmp679=_tmp678.f1;_LL36B: {
# 1924
struct _tuple24 _tmp9C6;struct _tuple24 _tmp67A=(_tmp9C6.f1=env,((_tmp9C6.f2=_tmp679,((_tmp9C6.f3=decl->loc,_tmp9C6)))));
# 1925
struct Cyc_CfFlowInfo_FlowEnv*_tmp67B=env->fenv;
# 1926
{void*_tmp67C=decl->r;struct Cyc_Absyn_Vardecl*_tmp67E;struct Cyc_Core_Opt*_tmp680;struct Cyc_Core_Opt _tmp681;struct Cyc_List_List*_tmp682;struct Cyc_List_List*_tmp684;_LL36F: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp67D=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp67C;if(_tmp67D->tag != 0)goto _LL371;else{_tmp67E=_tmp67D->f1;}}_LL370:
# 1928
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp67A,_tmp67E);
# 1929
goto _LL36E;_LL371: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp67F=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp67C;if(_tmp67F->tag != 2)goto _LL373;else{_tmp680=_tmp67F->f2;if(_tmp680 == 0)goto _LL373;_tmp681=*_tmp680;_tmp682=(struct Cyc_List_List*)_tmp681.v;}}_LL372:
# 1930
 _tmp684=_tmp682;goto _LL374;_LL373: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp683=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp67C;if(_tmp683->tag != 3)goto _LL375;else{_tmp684=_tmp683->f1;}}_LL374:
# 1932
((void(*)(void(*f)(struct _tuple24*,struct Cyc_Absyn_Vardecl*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp67A,_tmp684);
# 1933
goto _LL36E;_LL375:;_LL376:
# 1934
 goto _LL36E;_LL36E:;}
# 1936
goto _LL369;}_LL36C:;_LL36D:
# 1937
 goto _LL369;_LL369:;}
# 1939
return;}
# 1942
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
# 1943
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1947
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp687;union Cyc_CfFlowInfo_FlowInfo*_tmp688;struct _tuple17 _tmp686=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);_tmp687=_tmp686.f1;_tmp688=_tmp686.f2;
# 1948
inflow=*_tmp688;{
# 1949
struct Cyc_CfFlowInfo_FlowEnv*_tmp689=env->fenv;
# 1950
void*_tmp68A=s->r;struct Cyc_Absyn_Exp*_tmp68D;struct Cyc_Absyn_Exp*_tmp68F;struct Cyc_Absyn_Exp*_tmp691;struct Cyc_Absyn_Stmt*_tmp693;struct Cyc_Absyn_Stmt*_tmp694;struct Cyc_Absyn_Exp*_tmp696;struct Cyc_Absyn_Stmt*_tmp697;struct Cyc_Absyn_Stmt*_tmp698;struct _tuple10 _tmp69A;struct Cyc_Absyn_Exp*_tmp69B;struct Cyc_Absyn_Stmt*_tmp69C;struct Cyc_Absyn_Stmt*_tmp69D;struct Cyc_Absyn_Stmt*_tmp69F;struct _tuple10 _tmp6A0;struct Cyc_Absyn_Exp*_tmp6A1;struct Cyc_Absyn_Stmt*_tmp6A2;struct Cyc_Absyn_Exp*_tmp6A4;struct _tuple10 _tmp6A5;struct Cyc_Absyn_Exp*_tmp6A6;struct Cyc_Absyn_Stmt*_tmp6A7;struct _tuple10 _tmp6A8;struct Cyc_Absyn_Exp*_tmp6A9;struct Cyc_Absyn_Stmt*_tmp6AA;struct Cyc_Absyn_Stmt*_tmp6AB;struct Cyc_Absyn_Stmt*_tmp6AD;struct Cyc_List_List*_tmp6AF;struct Cyc_Absyn_Switch_clause**_tmp6B0;struct Cyc_Absyn_Switch_clause*_tmp6B1;struct Cyc_Absyn_Stmt*_tmp6B3;struct Cyc_Absyn_Stmt*_tmp6B5;struct Cyc_Absyn_Stmt*_tmp6B7;struct Cyc_Absyn_Exp*_tmp6B9;struct Cyc_List_List*_tmp6BA;struct Cyc_Absyn_Stmt*_tmp6BC;struct Cyc_List_List*_tmp6BD;struct Cyc_Absyn_Decl*_tmp6BF;struct Cyc_Absyn_Decl _tmp6C0;void*_tmp6C1;struct Cyc_Absyn_Exp*_tmp6C3;struct Cyc_Absyn_Tvar*_tmp6C4;struct Cyc_Absyn_Vardecl*_tmp6C5;struct Cyc_Absyn_Stmt*_tmp6C6;struct Cyc_Absyn_Decl*_tmp6C8;struct Cyc_Absyn_Stmt*_tmp6C9;struct Cyc_Absyn_Stmt*_tmp6CB;struct Cyc_Absyn_Exp*_tmp6CD;_LL378: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp68B=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp68A;if(_tmp68B->tag != 0)goto _LL37A;}_LL379:
# 1951
 return inflow;_LL37A: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp68C=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp68A;if(_tmp68C->tag != 3)goto _LL37C;else{_tmp68D=_tmp68C->f1;if(_tmp68D != 0)goto _LL37C;}}_LL37B:
# 1953
 if(env->noreturn){
# 1954
const char*_tmp9C9;void*_tmp9C8;(_tmp9C8=0,Cyc_Tcutil_terr(s->loc,((_tmp9C9="`noreturn' function might return",_tag_dyneither(_tmp9C9,sizeof(char),33))),_tag_dyneither(_tmp9C8,sizeof(void*),0)));}
# 1955
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
# 1957
return Cyc_CfFlowInfo_BottomFL();_LL37C: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp68E=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp68A;if(_tmp68E->tag != 3)goto _LL37E;else{_tmp68F=_tmp68E->f1;}}_LL37D:
# 1959
 if(env->noreturn){
# 1960
const char*_tmp9CC;void*_tmp9CB;(_tmp9CB=0,Cyc_Tcutil_terr(s->loc,((_tmp9CC="`noreturn' function might return",_tag_dyneither(_tmp9CC,sizeof(char),33))),_tag_dyneither(_tmp9CB,sizeof(void*),0)));}{
# 1961
union Cyc_CfFlowInfo_FlowInfo _tmp6D3;void*_tmp6D4;struct _tuple14 _tmp6D2=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp68F));_tmp6D3=_tmp6D2.f1;_tmp6D4=_tmp6D2.f2;
# 1962
_tmp6D3=Cyc_NewControlFlow_use_Rval(env,_tmp68F->loc,_tmp6D3,_tmp6D4);
# 1963
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp6D3);
# 1964
return Cyc_CfFlowInfo_BottomFL();};_LL37E: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp690=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp68A;if(_tmp690->tag != 1)goto _LL380;else{_tmp691=_tmp690->f1;}}_LL37F:
# 1966
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp691)).f1;_LL380: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp692=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp68A;if(_tmp692->tag != 2)goto _LL382;else{_tmp693=_tmp692->f1;_tmp694=_tmp692->f2;}}_LL381:
# 1969
 return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp693),_tmp694);_LL382: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp695=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp68A;if(_tmp695->tag != 4)goto _LL384;else{_tmp696=_tmp695->f1;_tmp697=_tmp695->f2;_tmp698=_tmp695->f3;}}_LL383: {
# 1972
union Cyc_CfFlowInfo_FlowInfo _tmp6D6;union Cyc_CfFlowInfo_FlowInfo _tmp6D7;struct _tuple16 _tmp6D5=Cyc_NewControlFlow_anal_test(env,inflow,_tmp696);_tmp6D6=_tmp6D5.f1;_tmp6D7=_tmp6D5.f2;{
# 1979
union Cyc_CfFlowInfo_FlowInfo _tmp6D8=Cyc_NewControlFlow_anal_stmt(env,_tmp6D7,_tmp698);
# 1980
union Cyc_CfFlowInfo_FlowInfo _tmp6D9=Cyc_NewControlFlow_anal_stmt(env,_tmp6D6,_tmp697);
# 1981
return Cyc_CfFlowInfo_join_flow(_tmp689,env->all_changed,_tmp6D9,_tmp6D8);};}_LL384: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp699=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp68A;if(_tmp699->tag != 5)goto _LL386;else{_tmp69A=_tmp699->f1;_tmp69B=_tmp69A.f1;_tmp69C=_tmp69A.f2;_tmp69D=_tmp699->f2;}}_LL385: {
# 1987
union Cyc_CfFlowInfo_FlowInfo*_tmp6DB;struct _tuple17 _tmp6DA=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp69C);_tmp6DB=_tmp6DA.f2;{
# 1988
union Cyc_CfFlowInfo_FlowInfo _tmp6DC=*_tmp6DB;
# 1989
union Cyc_CfFlowInfo_FlowInfo _tmp6DE;union Cyc_CfFlowInfo_FlowInfo _tmp6DF;struct _tuple16 _tmp6DD=Cyc_NewControlFlow_anal_test(env,_tmp6DC,_tmp69B);_tmp6DE=_tmp6DD.f1;_tmp6DF=_tmp6DD.f2;{
# 1990
union Cyc_CfFlowInfo_FlowInfo _tmp6E0=Cyc_NewControlFlow_anal_stmt(env,_tmp6DE,_tmp69D);
# 1991
Cyc_NewControlFlow_update_flow(env,_tmp69C,_tmp6E0);
# 1992
return _tmp6DF;};};}_LL386: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp69E=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp68A;if(_tmp69E->tag != 14)goto _LL388;else{_tmp69F=_tmp69E->f1;_tmp6A0=_tmp69E->f2;_tmp6A1=_tmp6A0.f1;_tmp6A2=_tmp6A0.f2;}}_LL387: {
# 1997
union Cyc_CfFlowInfo_FlowInfo _tmp6E1=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp69F);
# 1998
union Cyc_CfFlowInfo_FlowInfo*_tmp6E3;struct _tuple17 _tmp6E2=Cyc_NewControlFlow_pre_stmt_check(env,_tmp6E1,_tmp6A2);_tmp6E3=_tmp6E2.f2;{
# 1999
union Cyc_CfFlowInfo_FlowInfo _tmp6E4=*_tmp6E3;
# 2000
union Cyc_CfFlowInfo_FlowInfo _tmp6E6;union Cyc_CfFlowInfo_FlowInfo _tmp6E7;struct _tuple16 _tmp6E5=Cyc_NewControlFlow_anal_test(env,_tmp6E4,_tmp6A1);_tmp6E6=_tmp6E5.f1;_tmp6E7=_tmp6E5.f2;
# 2001
Cyc_NewControlFlow_update_flow(env,_tmp69F,_tmp6E6);
# 2002
return _tmp6E7;};}_LL388: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp6A3=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp68A;if(_tmp6A3->tag != 9)goto _LL38A;else{_tmp6A4=_tmp6A3->f1;_tmp6A5=_tmp6A3->f2;_tmp6A6=_tmp6A5.f1;_tmp6A7=_tmp6A5.f2;_tmp6A8=_tmp6A3->f3;_tmp6A9=_tmp6A8.f1;_tmp6AA=_tmp6A8.f2;_tmp6AB=_tmp6A3->f4;}}_LL389: {
# 2006
union Cyc_CfFlowInfo_FlowInfo _tmp6E8=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp6A4)).f1;
# 2007
union Cyc_CfFlowInfo_FlowInfo*_tmp6EA;struct _tuple17 _tmp6E9=Cyc_NewControlFlow_pre_stmt_check(env,_tmp6E8,_tmp6A7);_tmp6EA=_tmp6E9.f2;{
# 2008
union Cyc_CfFlowInfo_FlowInfo _tmp6EB=*_tmp6EA;
# 2009
union Cyc_CfFlowInfo_FlowInfo _tmp6ED;union Cyc_CfFlowInfo_FlowInfo _tmp6EE;struct _tuple16 _tmp6EC=Cyc_NewControlFlow_anal_test(env,_tmp6EB,_tmp6A6);_tmp6ED=_tmp6EC.f1;_tmp6EE=_tmp6EC.f2;{
# 2010
union Cyc_CfFlowInfo_FlowInfo _tmp6EF=Cyc_NewControlFlow_anal_stmt(env,_tmp6ED,_tmp6AB);
# 2011
union Cyc_CfFlowInfo_FlowInfo*_tmp6F1;struct _tuple17 _tmp6F0=Cyc_NewControlFlow_pre_stmt_check(env,_tmp6EF,_tmp6AA);_tmp6F1=_tmp6F0.f2;{
# 2012
union Cyc_CfFlowInfo_FlowInfo _tmp6F2=*_tmp6F1;
# 2013
union Cyc_CfFlowInfo_FlowInfo _tmp6F3=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp6F2,_tmp6A9)).f1;
# 2014
Cyc_NewControlFlow_update_flow(env,_tmp6A7,_tmp6F3);
# 2015
return _tmp6EE;};};};}_LL38A: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp6AC=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp68A;if(_tmp6AC->tag != 6)goto _LL38C;else{_tmp6AD=_tmp6AC->f1;if(_tmp6AD != 0)goto _LL38C;}}_LL38B:
# 2018
 return Cyc_CfFlowInfo_BottomFL();_LL38C: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp6AE=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp68A;if(_tmp6AE->tag != 11)goto _LL38E;else{_tmp6AF=_tmp6AE->f1;_tmp6B0=_tmp6AE->f2;if(_tmp6B0 == 0)goto _LL38E;_tmp6B1=*_tmp6B0;}}_LL38D: {
# 2020
struct _RegionHandle _tmp6F4=_new_region("temp");struct _RegionHandle*temp=& _tmp6F4;_push_region(temp);
# 2021
{union Cyc_CfFlowInfo_FlowInfo _tmp6F6;struct Cyc_List_List*_tmp6F7;struct _tuple20 _tmp6F5=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp6AF,1,1);_tmp6F6=_tmp6F5.f1;_tmp6F7=_tmp6F5.f2;
# 2023
for(0;_tmp6F7 != 0;(_tmp6F7=_tmp6F7->tl,_tmp6AF=_tmp6AF->tl)){
# 2024
_tmp6F6=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp6AF))->hd)->loc,_tmp6F6,(void*)_tmp6F7->hd);}
# 2026
_tmp6F6=Cyc_NewControlFlow_add_vars(_tmp689,_tmp6F6,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp6B1->pat_vars))->v,_tmp689->unknown_all,s->loc);
# 2027
Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_tmp6B1->body,_tmp6F6);{
# 2028
union Cyc_CfFlowInfo_FlowInfo _tmp6F8=Cyc_CfFlowInfo_BottomFL();_npop_handler(0);return _tmp6F8;};}
# 2021
;_pop_region(temp);}_LL38E: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp6B2=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp68A;if(_tmp6B2->tag != 6)goto _LL390;else{_tmp6B3=_tmp6B2->f1;}}_LL38F:
# 2030
 _tmp6B5=_tmp6B3;goto _LL391;_LL390: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp6B4=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp68A;if(_tmp6B4->tag != 7)goto _LL392;else{_tmp6B5=_tmp6B4->f1;}}_LL391:
# 2031
 _tmp6B7=_tmp6B5;goto _LL393;_LL392: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp6B6=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp68A;if(_tmp6B6->tag != 8)goto _LL394;else{_tmp6B7=_tmp6B6->f2;}}_LL393:
# 2035
 if(env->iteration_num == 1){
# 2036
struct Cyc_Absyn_Stmt*_tmp6F9=_tmp687->encloser;
# 2037
struct Cyc_Absyn_Stmt*_tmp6FA=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(_tmp6B7)))->encloser;
# 2038
while(_tmp6FA != _tmp6F9){
# 2039
struct Cyc_Absyn_Stmt*_tmp6FB=(Cyc_NewControlFlow_get_stmt_annot(_tmp6F9))->encloser;
# 2040
if(_tmp6FB == _tmp6F9){
# 2041
{const char*_tmp9CF;void*_tmp9CE;(_tmp9CE=0,Cyc_Tcutil_terr(s->loc,((_tmp9CF="goto enters local scope or exception handler",_tag_dyneither(_tmp9CF,sizeof(char),45))),_tag_dyneither(_tmp9CE,sizeof(void*),0)));}
# 2042
break;}
# 2044
_tmp6F9=_tmp6FB;}}
# 2049
Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_check_null(_tmp6B7),inflow);
# 2050
return Cyc_CfFlowInfo_BottomFL();_LL394: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp6B8=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp68A;if(_tmp6B8->tag != 10)goto _LL396;else{_tmp6B9=_tmp6B8->f1;_tmp6BA=_tmp6B8->f2;}}_LL395: {
# 2053
union Cyc_CfFlowInfo_FlowInfo _tmp6FF;void*_tmp700;struct _tuple14 _tmp6FE=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp6B9);_tmp6FF=_tmp6FE.f1;_tmp700=_tmp6FE.f2;
# 2054
_tmp6FF=Cyc_NewControlFlow_use_Rval(env,_tmp6B9->loc,_tmp6FF,_tmp700);
# 2055
return Cyc_NewControlFlow_anal_scs(env,_tmp6FF,_tmp6BA);}_LL396: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp6BB=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp68A;if(_tmp6BB->tag != 15)goto _LL398;else{_tmp6BC=_tmp6BB->f1;_tmp6BD=_tmp6BB->f2;}}_LL397: {
# 2060
int old_in_try=env->in_try;
# 2061
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
# 2062
env->in_try=1;
# 2063
env->tryflow=inflow;{
# 2064
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp6BC);
# 2065
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2068
env->in_try=old_in_try;
# 2069
env->tryflow=old_tryflow;
# 2071
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
# 2072
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp6BD);
# 2073
{union Cyc_CfFlowInfo_FlowInfo _tmp701=scs_outflow;int _tmp702;_LL3A3: if((_tmp701.BottomFL).tag != 1)goto _LL3A5;_tmp702=(int)(_tmp701.BottomFL).val;_LL3A4:
# 2074
 goto _LL3A2;_LL3A5:;_LL3A6: {
# 2075
const char*_tmp9D2;void*_tmp9D1;(_tmp9D1=0,Cyc_Tcutil_terr(s->loc,((_tmp9D2="last catch clause may implicitly fallthru",_tag_dyneither(_tmp9D2,sizeof(char),42))),_tag_dyneither(_tmp9D1,sizeof(void*),0)));}_LL3A2:;}
# 2077
outflow=s1_outflow;
# 2079
return outflow;};};}_LL398: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp6BE=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp68A;if(_tmp6BE->tag != 12)goto _LL39A;else{_tmp6BF=_tmp6BE->f1;_tmp6C0=*_tmp6BF;_tmp6C1=_tmp6C0.r;{struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp6C2=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp6C1;if(_tmp6C2->tag != 5)goto _LL39A;else{_tmp6C3=_tmp6C2->f1;_tmp6C4=_tmp6C2->f2;_tmp6C5=_tmp6C2->f3;}};_tmp6C6=_tmp6BE->f2;}}_LL399: {
# 2085
union Cyc_CfFlowInfo_FlowInfo _tmp706;union Cyc_CfFlowInfo_AbsLVal _tmp707;struct _tuple15 _tmp705=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,_tmp6C3);_tmp706=_tmp705.f1;_tmp707=_tmp705.f2;{
# 2089
struct _tuple15 _tmp9D3;struct _tuple15 _tmp709=(_tmp9D3.f1=_tmp706,((_tmp9D3.f2=_tmp707,_tmp9D3)));union Cyc_CfFlowInfo_FlowInfo _tmp70A;struct _tuple13 _tmp70B;struct Cyc_Dict_Dict _tmp70C;struct Cyc_List_List*_tmp70D;union Cyc_CfFlowInfo_AbsLVal _tmp70E;struct Cyc_CfFlowInfo_Place*_tmp70F;union Cyc_CfFlowInfo_FlowInfo _tmp710;int _tmp711;_LL3A8: _tmp70A=_tmp709.f1;if((_tmp70A.ReachableFL).tag != 2)goto _LL3AA;_tmp70B=(struct _tuple13)(_tmp70A.ReachableFL).val;_tmp70C=_tmp70B.f1;_tmp70D=_tmp70B.f2;_tmp70E=_tmp709.f2;if((_tmp70E.PlaceL).tag != 1)goto _LL3AA;_tmp70F=(struct Cyc_CfFlowInfo_Place*)(_tmp70E.PlaceL).val;_LL3A9: {
# 2091
void*_tmp712=Cyc_CfFlowInfo_lookup_place(_tmp70C,_tmp70F);
# 2093
void*t=(void*)_check_null(_tmp6C3->topt);
# 2094
{void*_tmp713=_tmp6C3->r;struct Cyc_Absyn_Exp*_tmp715;_LL3AF: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp714=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp713;if(_tmp714->tag != 13)goto _LL3B1;else{_tmp715=_tmp714->f2;}}_LL3B0:
# 2095
 t=(void*)_check_null(_tmp715->topt);goto _LL3AE;_LL3B1:;_LL3B2:
# 2096
 goto _LL3AE;_LL3AE:;}{
# 2098
void*_tmp716=Cyc_CfFlowInfo_make_unique_consumed(_tmp689,t,_tmp6C3,env->iteration_num,_tmp712);
# 2099
_tmp70C=Cyc_CfFlowInfo_assign_place(_tmp689,_tmp6C3->loc,_tmp70C,env->all_changed,_tmp70F,_tmp716);
# 2100
_tmp706=Cyc_CfFlowInfo_ReachableFL(_tmp70C,_tmp70D);{
# 2104
void*leaf;
# 2105
switch(Cyc_CfFlowInfo_initlevel(_tmp689,_tmp70C,_tmp712)){case Cyc_CfFlowInfo_AllIL: _LL3B3:
# 2106
 leaf=_tmp689->unknown_all;break;case Cyc_CfFlowInfo_NoneIL: _LL3B4:
# 2107
 leaf=_tmp689->unknown_none;break;case Cyc_CfFlowInfo_ThisIL: _LL3B5:
# 2108
 leaf=_tmp689->unknown_this;break;}{
# 2110
void*_tmp717=Cyc_CfFlowInfo_typ_to_absrval(_tmp689,t,0,leaf);
# 2111
_tmp706=Cyc_NewControlFlow_use_Rval(env,_tmp6C3->loc,_tmp706,_tmp712);{
# 2112
struct Cyc_List_List _tmp9D4;struct Cyc_List_List _tmp718=(_tmp9D4.hd=_tmp6C5,((_tmp9D4.tl=0,_tmp9D4)));
# 2113
_tmp706=Cyc_NewControlFlow_add_vars(_tmp689,_tmp706,(struct Cyc_List_List*)& _tmp718,_tmp689->unknown_all,s->loc);
# 2117
_tmp706=Cyc_NewControlFlow_anal_stmt(env,_tmp706,_tmp6C6);{
# 2122
union Cyc_CfFlowInfo_FlowInfo _tmp719=_tmp706;struct _tuple13 _tmp71A;struct Cyc_Dict_Dict _tmp71B;struct Cyc_List_List*_tmp71C;_LL3B8: if((_tmp719.ReachableFL).tag != 2)goto _LL3BA;_tmp71A=(struct _tuple13)(_tmp719.ReachableFL).val;_tmp71B=_tmp71A.f1;_tmp71C=_tmp71A.f2;_LL3B9:
# 2124
 _tmp71B=Cyc_CfFlowInfo_assign_place(_tmp689,s->loc,_tmp71B,env->all_changed,_tmp70F,_tmp717);
# 2125
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp71B,_tmp71C);
# 2129
return outflow;_LL3BA:;_LL3BB:
# 2130
 return _tmp706;_LL3B7:;};};};};};}_LL3AA: _tmp710=_tmp709.f1;if((_tmp710.BottomFL).tag != 1)goto _LL3AC;_tmp711=(int)(_tmp710.BottomFL).val;_LL3AB:
# 2132
 return _tmp706;_LL3AC:;_LL3AD:
# 2133
{const char*_tmp9D7;void*_tmp9D6;(_tmp9D6=0,Cyc_Tcutil_terr(_tmp6C3->loc,((_tmp9D7="bad alias expression--no unique path found",_tag_dyneither(_tmp9D7,sizeof(char),43))),_tag_dyneither(_tmp9D6,sizeof(void*),0)));}
# 2134
return Cyc_CfFlowInfo_BottomFL();_LL3A7:;};}_LL39A: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp6C7=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp68A;if(_tmp6C7->tag != 12)goto _LL39C;else{_tmp6C8=_tmp6C7->f1;_tmp6C9=_tmp6C7->f2;}}_LL39B:
# 2138
 outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp6C8),_tmp6C9);
# 2139
if(Cyc_NewControlFlow_warn_lose_unique)
# 2140
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp6C8);
# 2141
return outflow;_LL39C: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp6CA=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp68A;if(_tmp6CA->tag != 13)goto _LL39E;else{_tmp6CB=_tmp6CA->f2;}}_LL39D:
# 2143
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp6CB);_LL39E: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp6CC=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp68A;if(_tmp6CC->tag != 16)goto _LL3A0;else{_tmp6CD=_tmp6CC->f1;}}_LL39F: {
# 2146
union Cyc_CfFlowInfo_FlowInfo _tmp721;void*_tmp722;struct _tuple14 _tmp720=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp6CD);_tmp721=_tmp720.f1;_tmp722=_tmp720.f2;{
# 2147
union Cyc_CfFlowInfo_FlowInfo _tmp723=Cyc_NewControlFlow_use_Rval(env,_tmp6CD->loc,_tmp721,_tmp722);
# 2148
void*_tmp724=Cyc_Tcutil_compress((void*)_check_null(_tmp6CD->topt));void*_tmp726;_LL3BD: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp725=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp724;if(_tmp725->tag != 15)goto _LL3BF;else{_tmp726=(void*)_tmp725->f1;}}_LL3BE:
# 2150
 return Cyc_CfFlowInfo_kill_flow_region(_tmp689,_tmp721,_tmp726);_LL3BF:;_LL3C0: {
# 2151
struct Cyc_Core_Impossible_exn_struct _tmp9DD;const char*_tmp9DC;struct Cyc_Core_Impossible_exn_struct*_tmp9DB;(int)_throw((void*)((_tmp9DB=_cycalloc(sizeof(*_tmp9DB)),((_tmp9DB[0]=((_tmp9DD.tag=Cyc_Core_Impossible,((_tmp9DD.f1=((_tmp9DC="anal_stmt -- reset_region",_tag_dyneither(_tmp9DC,sizeof(char),26))),_tmp9DD)))),_tmp9DB)))));}_LL3BC:;};}_LL3A0:;_LL3A1: {
# 2154
struct Cyc_Core_Impossible_exn_struct _tmp9E3;const char*_tmp9E2;struct Cyc_Core_Impossible_exn_struct*_tmp9E1;(int)_throw((void*)((_tmp9E1=_cycalloc(sizeof(*_tmp9E1)),((_tmp9E1[0]=((_tmp9E3.tag=Cyc_Core_Impossible,((_tmp9E3.f1=((_tmp9E2="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(_tmp9E2,sizeof(char),56))),_tmp9E3)))),_tmp9E1)))));}_LL377:;};}
# 2159
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2161
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 2162
struct Cyc_CfFlowInfo_FlowEnv*_tmp72D=env->fenv;
# 2163
void*_tmp72E=decl->r;struct Cyc_Absyn_Vardecl*_tmp730;struct Cyc_Core_Opt*_tmp732;struct Cyc_Core_Opt _tmp733;struct Cyc_List_List*_tmp734;struct Cyc_Absyn_Exp*_tmp735;struct Cyc_List_List*_tmp737;struct Cyc_Absyn_Fndecl*_tmp739;struct Cyc_Absyn_Tvar*_tmp73B;struct Cyc_Absyn_Vardecl*_tmp73C;int _tmp73D;struct Cyc_Absyn_Exp*_tmp73E;_LL3C2: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp72F=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp72E;if(_tmp72F->tag != 0)goto _LL3C4;else{_tmp730=_tmp72F->f1;}}_LL3C3: {
# 2171
struct Cyc_List_List _tmp9E4;struct Cyc_List_List _tmp73F=(_tmp9E4.hd=_tmp730,((_tmp9E4.tl=0,_tmp9E4)));
# 2172
inflow=Cyc_NewControlFlow_add_vars(_tmp72D,inflow,(struct Cyc_List_List*)& _tmp73F,_tmp72D->unknown_none,decl->loc);{
# 2173
struct Cyc_Absyn_Exp*_tmp740=_tmp730->initializer;
# 2174
if(_tmp740 == 0)
# 2175
return inflow;{
# 2176
union Cyc_CfFlowInfo_FlowInfo _tmp742;void*_tmp743;struct _tuple14 _tmp741=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_tmp740);_tmp742=_tmp741.f1;_tmp743=_tmp741.f2;{
# 2177
union Cyc_CfFlowInfo_FlowInfo _tmp744=_tmp742;int _tmp745;struct _tuple13 _tmp746;struct Cyc_Dict_Dict _tmp747;struct Cyc_List_List*_tmp748;_LL3CF: if((_tmp744.BottomFL).tag != 1)goto _LL3D1;_tmp745=(int)(_tmp744.BottomFL).val;_LL3D0:
# 2178
 return Cyc_CfFlowInfo_BottomFL();_LL3D1: if((_tmp744.ReachableFL).tag != 2)goto _LL3CE;_tmp746=(struct _tuple13)(_tmp744.ReachableFL).val;_tmp747=_tmp746.f1;_tmp748=_tmp746.f2;_LL3D2:
# 2180
{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp9EA;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp9E9;struct Cyc_CfFlowInfo_Place*_tmp9E8;_tmp747=Cyc_CfFlowInfo_assign_place(_tmp72D,decl->loc,_tmp747,env->all_changed,(
# 2181
(_tmp9E8=_region_malloc(env->r,sizeof(*_tmp9E8)),((_tmp9E8->root=(void*)((_tmp9EA=_region_malloc(env->r,sizeof(*_tmp9EA)),((_tmp9EA[0]=((_tmp9E9.tag=0,((_tmp9E9.f1=_tmp730,_tmp9E9)))),_tmp9EA)))),((_tmp9E8->fields=0,_tmp9E8)))))),_tmp743);}
# 2183
_tmp748=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp748,_tmp730,(struct Cyc_Absyn_Exp*)_check_null(_tmp730->initializer));{
# 2184
union Cyc_CfFlowInfo_FlowInfo _tmp74C=Cyc_CfFlowInfo_ReachableFL(_tmp747,_tmp748);
# 2185
Cyc_NewControlFlow_update_tryflow(env,_tmp74C);
# 2188
return _tmp74C;};_LL3CE:;};};};}_LL3C4: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp731=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp72E;if(_tmp731->tag != 2)goto _LL3C6;else{_tmp732=_tmp731->f2;if(_tmp732 == 0)goto _LL3C6;_tmp733=*_tmp732;_tmp734=(struct Cyc_List_List*)_tmp733.v;_tmp735=_tmp731->f3;}}_LL3C5: {
# 2193
union Cyc_CfFlowInfo_FlowInfo _tmp74F;void*_tmp750;struct _tuple14 _tmp74E=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp735);_tmp74F=_tmp74E.f1;_tmp750=_tmp74E.f2;
# 2194
_tmp74F=Cyc_NewControlFlow_use_Rval(env,_tmp735->loc,_tmp74F,_tmp750);
# 2195
return Cyc_NewControlFlow_add_vars(_tmp72D,_tmp74F,_tmp734,_tmp72D->unknown_all,decl->loc);}_LL3C6: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp736=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp72E;if(_tmp736->tag != 3)goto _LL3C8;else{_tmp737=_tmp736->f1;}}_LL3C7:
# 2199
 return Cyc_NewControlFlow_add_vars(_tmp72D,inflow,_tmp737,_tmp72D->unknown_none,decl->loc);_LL3C8: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp738=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp72E;if(_tmp738->tag != 1)goto _LL3CA;else{_tmp739=_tmp738->f1;}}_LL3C9:
# 2202
 Cyc_NewControlFlow_check_nested_fun(_tmp72D,inflow,_tmp739);{
# 2203
void*t=(void*)_check_null(_tmp739->cached_typ);
# 2204
struct Cyc_Absyn_Vardecl*_tmp751=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp739->fn_vardecl);
# 2208
struct Cyc_List_List*_tmp9EB;return Cyc_NewControlFlow_add_vars(_tmp72D,inflow,((_tmp9EB=_region_malloc(env->r,sizeof(*_tmp9EB)),((_tmp9EB->hd=_tmp751,((_tmp9EB->tl=0,_tmp9EB)))))),_tmp72D->unknown_all,decl->loc);};_LL3CA: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp73A=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp72E;if(_tmp73A->tag != 4)goto _LL3CC;else{_tmp73B=_tmp73A->f1;_tmp73C=_tmp73A->f2;_tmp73D=_tmp73A->f3;_tmp73E=_tmp73A->f4;}}_LL3CB:
# 2211
 if(_tmp73E != 0){
# 2212
struct Cyc_Absyn_Exp*_tmp753=(struct Cyc_Absyn_Exp*)_tmp73E;
# 2213
union Cyc_CfFlowInfo_FlowInfo _tmp755;void*_tmp756;struct _tuple14 _tmp754=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp753);_tmp755=_tmp754.f1;_tmp756=_tmp754.f2;
# 2214
inflow=Cyc_NewControlFlow_use_Rval(env,_tmp753->loc,_tmp755,_tmp756);}{
# 2216
struct Cyc_List_List _tmp9EC;struct Cyc_List_List _tmp757=(_tmp9EC.hd=_tmp73C,((_tmp9EC.tl=0,_tmp9EC)));
# 2217
return Cyc_NewControlFlow_add_vars(_tmp72D,inflow,(struct Cyc_List_List*)& _tmp757,_tmp72D->unknown_all,decl->loc);};_LL3CC:;_LL3CD: {
# 2220
struct Cyc_Core_Impossible_exn_struct _tmp9F2;const char*_tmp9F1;struct Cyc_Core_Impossible_exn_struct*_tmp9F0;(int)_throw((void*)((_tmp9F0=_cycalloc(sizeof(*_tmp9F0)),((_tmp9F0[0]=((_tmp9F2.tag=Cyc_Core_Impossible,((_tmp9F2.f1=((_tmp9F1="anal_decl: unexpected decl variant",_tag_dyneither(_tmp9F1,sizeof(char),35))),_tmp9F2)))),_tmp9F0)))));}_LL3C1:;}
# 2260 "new_control_flow.cyc"
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){
# 2261
struct _RegionHandle _tmp75C=_new_region("frgn");struct _RegionHandle*frgn=& _tmp75C;_push_region(frgn);
# 2262
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(frgn);
# 2263
Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);}
# 2262
;_pop_region(frgn);}
# 2267
static int Cyc_NewControlFlow_hash_ptr(void*s){
# 2268
return(int)s;}
# 2271
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2275
struct _RegionHandle*_tmp75D=fenv->r;
# 2276
unsigned int _tmp75E=(fd->body)->loc;
# 2277
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp75E);{
# 2281
struct Cyc_List_List*param_roots=0;
# 2282
{union Cyc_CfFlowInfo_FlowInfo _tmp75F=inflow;int _tmp760;struct _tuple13 _tmp761;struct Cyc_Dict_Dict _tmp762;struct Cyc_List_List*_tmp763;_LL3D4: if((_tmp75F.BottomFL).tag != 1)goto _LL3D6;_tmp760=(int)(_tmp75F.BottomFL).val;_LL3D5: {
# 2283
const char*_tmp9F5;void*_tmp9F4;(_tmp9F4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9F5="check_fun",_tag_dyneither(_tmp9F5,sizeof(char),10))),_tag_dyneither(_tmp9F4,sizeof(void*),0)));}_LL3D6: if((_tmp75F.ReachableFL).tag != 2)goto _LL3D3;_tmp761=(struct _tuple13)(_tmp75F.ReachableFL).val;_tmp762=_tmp761.f1;_tmp763=_tmp761.f2;_LL3D7: {
# 2287
struct Cyc_List_List*atts;
# 2288
{void*_tmp766=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));struct Cyc_Absyn_FnInfo _tmp768;struct Cyc_List_List*_tmp769;_LL3D9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp767=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp766;if(_tmp767->tag != 9)goto _LL3DB;else{_tmp768=_tmp767->f1;_tmp769=_tmp768.attributes;}}_LL3DA:
# 2289
 atts=_tmp769;goto _LL3D8;_LL3DB:;_LL3DC: {
# 2290
const char*_tmp9F8;void*_tmp9F7;(_tmp9F7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9F8="check_fun: non-function type cached with fndecl_t",_tag_dyneither(_tmp9F8,sizeof(char),50))),_tag_dyneither(_tmp9F7,sizeof(void*),0)));}_LL3D8:;}
# 2292
for(0;atts != 0;atts=atts->tl){
# 2293
void*_tmp76C=(void*)atts->hd;int _tmp76E;int _tmp770;int _tmp772;_LL3DE: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp76D=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp76C;if(_tmp76D->tag != 21)goto _LL3E0;else{_tmp76E=_tmp76D->f1;}}_LL3DF: {
# 2295
unsigned int j=(unsigned int)_tmp76E;
# 2296
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
# 2297
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
# 2298
{const char*_tmp9FB;void*_tmp9FA;(_tmp9FA=0,Cyc_Tcutil_terr(_tmp75E,((_tmp9FB="noliveunique attribute exceeds number of parameters",_tag_dyneither(_tmp9FB,sizeof(char),52))),_tag_dyneither(_tmp9FA,sizeof(void*),0)));}
# 2299
continue;}{
# 2301
struct Cyc_Absyn_Vardecl*_tmp775=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
# 2302
void*t=Cyc_Tcutil_compress(_tmp775->type);
# 2303
if(!Cyc_Tcutil_is_noalias_pointer(t)){
# 2304
{const char*_tmp9FE;void*_tmp9FD;(_tmp9FD=0,Cyc_Tcutil_terr(_tmp75E,((_tmp9FE="noliveunique attribute requires unique pointer type",_tag_dyneither(_tmp9FE,sizeof(char),52))),_tag_dyneither(_tmp9FD,sizeof(void*),0)));}
# 2305
continue;}{
# 2307
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
# 2308
void*_tmp778=
# 2309
Cyc_CfFlowInfo_make_unique_consumed(fenv,elttype,bogus_exp,- 1,
# 2310
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
# 2311
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmpA01;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmpA00;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp779=(_tmpA00=_region_malloc(_tmp75D,sizeof(*_tmpA00)),((_tmpA00[0]=((_tmpA01.tag=2,((_tmpA01.f1=(int)j,((_tmpA01.f2=(void*)t,_tmpA01)))))),_tmpA00)));
# 2312
struct Cyc_CfFlowInfo_Place*_tmpA02;struct Cyc_CfFlowInfo_Place*_tmp77A=(_tmpA02=_region_malloc(_tmp75D,sizeof(*_tmpA02)),((_tmpA02->root=(void*)_tmp779,((_tmpA02->fields=0,_tmpA02)))));
# 2313
_tmp762=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp762,(void*)_tmp779,_tmp778);
# 2314
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmpA08;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmpA07;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmpA05;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmpA04;_tmp762=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp762,(void*)((_tmpA04=_region_malloc(_tmp75D,sizeof(*_tmpA04)),((_tmpA04[0]=((_tmpA05.tag=0,((_tmpA05.f1=_tmp775,_tmpA05)))),_tmpA04)))),(void*)((_tmpA07=_region_malloc(_tmp75D,sizeof(*_tmpA07)),((_tmpA07[0]=((_tmpA08.tag=5,((_tmpA08.f1=_tmp77A,_tmpA08)))),_tmpA07)))));}
# 2315
goto _LL3DD;};};}_LL3E0: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp76F=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp76C;if(_tmp76F->tag != 20)goto _LL3E2;else{_tmp770=_tmp76F->f1;}}_LL3E1: {
# 2317
unsigned int j=(unsigned int)_tmp770;
# 2318
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
# 2319
{const char*_tmpA0B;void*_tmpA0A;(_tmpA0A=0,Cyc_Tcutil_terr(_tmp75E,((_tmpA0B="initializes attribute exceeds number of parameters",_tag_dyneither(_tmpA0B,sizeof(char),51))),_tag_dyneither(_tmpA0A,sizeof(void*),0)));}
# 2321
continue;}{
# 2323
struct Cyc_Absyn_Vardecl*_tmp784=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
# 2324
{void*_tmp785=Cyc_Tcutil_compress(_tmp784->type);struct Cyc_Absyn_PtrInfo _tmp787;void*_tmp788;struct Cyc_Absyn_PtrAtts _tmp789;union Cyc_Absyn_Constraint*_tmp78A;union Cyc_Absyn_Constraint*_tmp78B;union Cyc_Absyn_Constraint*_tmp78C;_LL3E7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp786=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp785;if(_tmp786->tag != 5)goto _LL3E9;else{_tmp787=_tmp786->f1;_tmp788=_tmp787.elt_typ;_tmp789=_tmp787.ptr_atts;_tmp78A=_tmp789.nullable;_tmp78B=_tmp789.bounds;_tmp78C=_tmp789.zero_term;}}_LL3E8:
# 2326
 if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp78A)){
# 2327
const char*_tmpA0E;void*_tmpA0D;(_tmpA0D=0,Cyc_Tcutil_terr(_tmp75E,((_tmpA0E="initializes attribute not allowed on nullable pointers",_tag_dyneither(_tmpA0E,sizeof(char),55))),_tag_dyneither(_tmpA0D,sizeof(void*),0)));}
# 2329
if(!Cyc_Tcutil_is_bound_one(_tmp78B)){
# 2330
const char*_tmpA11;void*_tmpA10;(_tmpA10=0,Cyc_Tcutil_terr(_tmp75E,((_tmpA11="initializes attribute allowed only on pointers of size 1",_tag_dyneither(_tmpA11,sizeof(char),57))),_tag_dyneither(_tmpA10,sizeof(void*),0)));}
# 2332
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp78C)){
# 2333
const char*_tmpA14;void*_tmpA13;(_tmpA13=0,Cyc_Tcutil_terr(_tmp75E,((_tmpA14="initializes attribute allowed only on pointers to non-zero-terminated arrays",_tag_dyneither(_tmpA14,sizeof(char),77))),_tag_dyneither(_tmpA13,sizeof(void*),0)));}{
# 2335
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmpA17;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmpA16;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp793=(_tmpA16=_region_malloc(_tmp75D,sizeof(*_tmpA16)),((_tmpA16[0]=((_tmpA17.tag=2,((_tmpA17.f1=(int)j,((_tmpA17.f2=(void*)_tmp788,_tmpA17)))))),_tmpA16)));
# 2336
struct Cyc_CfFlowInfo_Place*_tmpA18;struct Cyc_CfFlowInfo_Place*_tmp794=(_tmpA18=_region_malloc(_tmp75D,sizeof(*_tmpA18)),((_tmpA18->root=(void*)_tmp793,((_tmpA18->fields=0,_tmpA18)))));
# 2337
_tmp762=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp762,(void*)_tmp793,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp788,0,fenv->esc_none));
# 2338
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmpA1E;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmpA1D;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmpA1B;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmpA1A;_tmp762=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp762,(void*)((_tmpA1A=_region_malloc(_tmp75D,sizeof(*_tmpA1A)),((_tmpA1A[0]=((_tmpA1B.tag=0,((_tmpA1B.f1=_tmp784,_tmpA1B)))),_tmpA1A)))),(void*)((_tmpA1D=_region_malloc(_tmp75D,sizeof(*_tmpA1D)),((_tmpA1D[0]=((_tmpA1E.tag=5,((_tmpA1E.f1=_tmp794,_tmpA1E)))),_tmpA1D)))));}
# 2339
{struct Cyc_List_List*_tmpA1F;param_roots=((_tmpA1F=_region_malloc(_tmp75D,sizeof(*_tmpA1F)),((_tmpA1F->hd=_tmp794,((_tmpA1F->tl=param_roots,_tmpA1F))))));}
# 2340
goto _LL3E6;};_LL3E9:;_LL3EA: {
# 2341
const char*_tmpA22;void*_tmpA21;(_tmpA21=0,Cyc_Tcutil_terr(_tmp75E,((_tmpA22="initializes attribute on non-pointer",_tag_dyneither(_tmpA22,sizeof(char),37))),_tag_dyneither(_tmpA21,sizeof(void*),0)));}_LL3E6:;}
# 2343
goto _LL3DD;};}_LL3E2: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp771=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp76C;if(_tmp771->tag != 22)goto _LL3E4;else{_tmp772=_tmp771->f1;}}_LL3E3: {
# 2345
unsigned int j=(unsigned int)_tmp772;
# 2346
if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){
# 2347
{const char*_tmpA25;void*_tmpA24;(_tmpA24=0,Cyc_Tcutil_terr(_tmp75E,((_tmpA25="noconsume attribute exceeds number of parameters",_tag_dyneither(_tmpA25,sizeof(char),49))),_tag_dyneither(_tmpA24,sizeof(void*),0)));}
# 2348
continue;}{
# 2350
struct Cyc_Absyn_Vardecl*_tmp7A1=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(int)(j - 1));
# 2351
void*t=Cyc_Tcutil_compress(_tmp7A1->type);
# 2352
if(!Cyc_Tcutil_is_noalias_pointer(t)){
# 2353
{const char*_tmpA28;void*_tmpA27;(_tmpA27=0,Cyc_Tcutil_terr(_tmp75E,((_tmpA28="noconsume attribute requires unique pointer type",_tag_dyneither(_tmpA28,sizeof(char),49))),_tag_dyneither(_tmpA27,sizeof(void*),0)));}
# 2354
continue;}
# 2357
{const char*_tmpA2B;void*_tmpA2A;(_tmpA2A=0,Cyc_Tcutil_terr(_tmp75E,((_tmpA2B="noconsume attribute not yet supported on fn defs",_tag_dyneither(_tmpA2B,sizeof(char),49))),_tag_dyneither(_tmpA2A,sizeof(void*),0)));}
# 2358
continue;};}_LL3E4:;_LL3E5:
# 2359
 goto _LL3DD;_LL3DD:;}
# 2361
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp762,_tmp763);}_LL3D3:;}{
# 2364
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
# 2365
struct Cyc_Hashtable_Table*flow_table=
# 2366
((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp75D,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
# 2367
struct Cyc_NewControlFlow_AnalEnv*_tmpA2C;struct Cyc_NewControlFlow_AnalEnv*env=(_tmpA2C=_region_malloc(_tmp75D,sizeof(*_tmpA2C)),((_tmpA2C->r=_tmp75D,((_tmpA2C->fenv=fenv,((_tmpA2C->iterate_again=1,((_tmpA2C->iteration_num=0,((_tmpA2C->in_try=0,((_tmpA2C->tryflow=inflow,((_tmpA2C->all_changed=0,((_tmpA2C->noreturn=noreturn,((_tmpA2C->param_roots=param_roots,((_tmpA2C->flow_table=flow_table,_tmpA2C)))))))))))))))))))));
# 2368
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
# 2369
while(env->iterate_again  && !Cyc_Position_error_p()){
# 2370
++ env->iteration_num;
# 2374
env->iterate_again=0;
# 2375
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);}{
# 2377
union Cyc_CfFlowInfo_FlowInfo _tmp7A6=outflow;int _tmp7A7;_LL3EC: if((_tmp7A6.BottomFL).tag != 1)goto _LL3EE;_tmp7A7=(int)(_tmp7A6.BottomFL).val;_LL3ED:
# 2378
 goto _LL3EB;_LL3EE:;_LL3EF:
# 2380
 Cyc_NewControlFlow_check_init_params(_tmp75E,env,outflow);
# 2383
if(noreturn){
# 2384
const char*_tmpA2F;void*_tmpA2E;(_tmpA2E=0,Cyc_Tcutil_terr(_tmp75E,((_tmpA2F="`noreturn' function might (implicitly) return",_tag_dyneither(_tmpA2F,sizeof(char),46))),_tag_dyneither(_tmpA2E,sizeof(void*),0)));}else{
# 2386
void*_tmp7AA=Cyc_Tcutil_compress(fd->ret_type);_LL3F1: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp7AB=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp7AA;if(_tmp7AB->tag != 0)goto _LL3F3;}_LL3F2:
# 2387
 goto _LL3F0;_LL3F3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7AC=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7AA;if(_tmp7AC->tag != 7)goto _LL3F5;}_LL3F4:
# 2388
 goto _LL3F6;_LL3F5: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7AD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7AA;if(_tmp7AD->tag != 6)goto _LL3F7;}_LL3F6:
# 2390
{const char*_tmpA32;void*_tmpA31;(_tmpA31=0,Cyc_Tcutil_warn(_tmp75E,((_tmpA32="function may not return a value",_tag_dyneither(_tmpA32,sizeof(char),32))),_tag_dyneither(_tmpA31,sizeof(void*),0)));}goto _LL3F0;_LL3F7:;_LL3F8:
# 2392
{const char*_tmpA35;void*_tmpA34;(_tmpA34=0,Cyc_Tcutil_terr(_tmp75E,((_tmpA35="function may not return a value",_tag_dyneither(_tmpA35,sizeof(char),32))),_tag_dyneither(_tmpA34,sizeof(void*),0)));}goto _LL3F0;_LL3F0:;}
# 2394
goto _LL3EB;_LL3EB:;};};};}
# 2399
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){
# 2400
for(0;ds != 0;ds=ds->tl){
# 2401
void*_tmp7B3=((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp7B5;struct Cyc_List_List*_tmp7B7;struct Cyc_List_List*_tmp7B9;struct Cyc_List_List*_tmp7BB;_LL3FA: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp7B4=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp7B3;if(_tmp7B4->tag != 1)goto _LL3FC;else{_tmp7B5=_tmp7B4->f1;}}_LL3FB:
# 2402
 Cyc_NewControlFlow_check_fun(_tmp7B5);goto _LL3F9;_LL3FC: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp7B6=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp7B3;if(_tmp7B6->tag != 12)goto _LL3FE;else{_tmp7B7=_tmp7B6->f1;}}_LL3FD:
# 2403
 _tmp7B9=_tmp7B7;goto _LL3FF;_LL3FE: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp7B8=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp7B3;if(_tmp7B8->tag != 11)goto _LL400;else{_tmp7B9=_tmp7B8->f2;}}_LL3FF:
# 2404
 _tmp7BB=_tmp7B9;goto _LL401;_LL400: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp7BA=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp7B3;if(_tmp7BA->tag != 10)goto _LL402;else{_tmp7BB=_tmp7BA->f2;}}_LL401:
# 2405
 Cyc_NewControlFlow_cf_check(_tmp7BB);goto _LL3F9;_LL402: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp7BC=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp7B3;if(_tmp7BC->tag != 13)goto _LL404;}_LL403:
# 2406
 goto _LL3F9;_LL404:;_LL405:
# 2407
 goto _LL3F9;_LL3F9:;}}
