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
# 157 "/home/jgm/cyclone/include/cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[
15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[
14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
typedef struct Cyc_List_List*Cyc_List_List_t;
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{
char*tag;};
# 123
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 198
struct Cyc_List_List*Cyc_List_merge_sort(int(*cmp)(void*,void*),struct Cyc_List_List*
x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 49 "/home/jgm/cyclone/include/string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
# 50
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 64
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "/home/jgm/cyclone/include/lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];
struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "/home/jgm/cyclone/include/position.h"
typedef unsigned int Cyc_Position_seg_t;
# 42
struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);
struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{
int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct
_dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[
10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 61
extern int Cyc_Position_use_gcc_style_location;
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
f1;};
# 245
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;
struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int
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
# 904 "/home/jgm/cyclone/src/absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 914
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 918
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 921
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 939
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 944
extern void*Cyc_Absyn_sint_typ;
# 961
void*Cyc_Absyn_string_typ(void*rgn);
# 983
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zero_term);struct Cyc_PP_Ppstate;
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
# 68 "/home/jgm/cyclone/src/absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};
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
char*tag;};
# 62
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 122 "/home/jgm/cyclone/include/dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
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
# 38 "/home/jgm/cyclone/src/tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 338 "/home/jgm/cyclone/src/tcutil.h"
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);struct _tuple10{void*f1;
void*f2;};struct Cyc_Tcexp_TestEnv{struct _tuple10*eq;int isTrue;};
# 39 "/home/jgm/cyclone/src/tcexp.h"
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
# 40
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr msg_part);extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_exn_struct{
char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;
struct Cyc_Absyn_Datatypefield*field;};
# 41 "/home/jgm/cyclone/src/tcdecl.h"
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_xdatatypefielddecl_t;
struct Cyc_Position_Segment{int start;int end;};struct Cyc_Port_Edit{unsigned int loc;
struct _dyneither_ptr old_string;struct _dyneither_ptr new_string;};
# 92 "/home/jgm/cyclone/src/port.cyc"
typedef struct Cyc_Port_Edit*Cyc_Port_edit_t;
# 97
typedef struct Cyc_List_List*Cyc_Port_edits_t;struct Cyc_Port_Edit;struct Cyc_Port_Edit;
struct Cyc_Port_Edit;struct Cyc_Port_Edit;
# 99
int Cyc_Port_cmp_edit(struct Cyc_Port_Edit*e1,struct Cyc_Port_Edit*e2){
# 100
return(int)e1 - (int)e2;}
# 107
typedef void*Cyc_Port_ctype_t;struct Cyc_Port_Cvar{int id;void**eq;struct Cyc_List_List*
uppers;struct Cyc_List_List*lowers;};
# 109
typedef struct Cyc_Port_Cvar*Cyc_Port_cvar_t;struct Cyc_Port_Cfield{void*qual;
struct _dyneither_ptr*f;void*type;};
# 116
typedef struct Cyc_Port_Cfield*Cyc_Port_cfield_t;
# 121
typedef struct Cyc_List_List*Cyc_Port_cfields_t;struct Cyc_Port_Const_ct_Port_Ctype_struct{
int tag;};struct Cyc_Port_Notconst_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Thin_ct_Port_Ctype_struct{
int tag;};struct Cyc_Port_Fat_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Void_ct_Port_Ctype_struct{
int tag;};struct Cyc_Port_Zero_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Arith_ct_Port_Ctype_struct{
int tag;};struct Cyc_Port_Heap_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Zterm_ct_Port_Ctype_struct{
int tag;};struct Cyc_Port_Nozterm_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_RgnVar_ct_Port_Ctype_struct{
int tag;struct _dyneither_ptr*f1;};struct Cyc_Port_Ptr_ct_Port_Ctype_struct{int tag;
void*f1;void*f2;void*f3;void*f4;void*f5;};struct Cyc_Port_Array_ct_Port_Ctype_struct{
int tag;void*f1;void*f2;void*f3;};struct _tuple11{struct Cyc_Absyn_Aggrdecl*f1;
struct Cyc_List_List*f2;};struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct{int tag;
struct _tuple11*f1;};struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct{int tag;
struct Cyc_List_List*f1;void**f2;};struct Cyc_Port_Fn_ct_Port_Ctype_struct{int tag;
void*f1;struct Cyc_List_List*f2;};struct Cyc_Port_Cvar;struct Cyc_Port_Var_ct_Port_Ctype_struct{
int tag;struct Cyc_Port_Cvar*f1;};
# 151
struct Cyc_Port_Const_ct_Port_Ctype_struct Cyc_Port_Const_ct_val={0};
# 152
struct Cyc_Port_Notconst_ct_Port_Ctype_struct Cyc_Port_Notconst_ct_val={1};
# 153
struct Cyc_Port_Thin_ct_Port_Ctype_struct Cyc_Port_Thin_ct_val={2};
# 154
struct Cyc_Port_Fat_ct_Port_Ctype_struct Cyc_Port_Fat_ct_val={3};
# 155
struct Cyc_Port_Void_ct_Port_Ctype_struct Cyc_Port_Void_ct_val={4};
# 156
struct Cyc_Port_Zero_ct_Port_Ctype_struct Cyc_Port_Zero_ct_val={5};
# 157
struct Cyc_Port_Arith_ct_Port_Ctype_struct Cyc_Port_Arith_ct_val={6};
# 158
struct Cyc_Port_Heap_ct_Port_Ctype_struct Cyc_Port_Heap_ct_val={7};
# 159
struct Cyc_Port_Zterm_ct_Port_Ctype_struct Cyc_Port_Zterm_ct_val={8};
# 160
struct Cyc_Port_Nozterm_ct_Port_Ctype_struct Cyc_Port_Nozterm_ct_val={9};
# 164
static struct _dyneither_ptr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*ts);
# 165
static struct _dyneither_ptr Cyc_Port_cfields2string(int deep,struct Cyc_List_List*
ts);struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
# 166
static struct _dyneither_ptr Cyc_Port_ctype2string(int deep,void*t){
# 167
void*_tmpA=t;struct _dyneither_ptr*_tmp16;void*_tmp18;void*_tmp19;void*_tmp1A;
void*_tmp1B;void*_tmp1C;void*_tmp1E;void*_tmp1F;void*_tmp20;struct _tuple11*
_tmp22;struct _tuple11 _tmp23;struct Cyc_Absyn_Aggrdecl*_tmp24;struct Cyc_List_List*
_tmp25;struct Cyc_List_List*_tmp27;void**_tmp28;void*_tmp29;struct Cyc_List_List*
_tmp2B;void*_tmp2D;struct Cyc_List_List*_tmp2E;struct Cyc_Port_Cvar*_tmp30;_LL1: {
struct Cyc_Port_Const_ct_Port_Ctype_struct*_tmpB=(struct Cyc_Port_Const_ct_Port_Ctype_struct*)
_tmpA;if(_tmpB->tag != 0)goto _LL3;}_LL2: {
# 168
const char*_tmp535;return(_tmp535="const",_tag_dyneither(_tmp535,sizeof(char),6));}
_LL3: {struct Cyc_Port_Notconst_ct_Port_Ctype_struct*_tmpC=(struct Cyc_Port_Notconst_ct_Port_Ctype_struct*)
_tmpA;if(_tmpC->tag != 1)goto _LL5;}_LL4: {
# 169
const char*_tmp536;return(_tmp536="notconst",_tag_dyneither(_tmp536,sizeof(char),
9));}_LL5: {struct Cyc_Port_Thin_ct_Port_Ctype_struct*_tmpD=(struct Cyc_Port_Thin_ct_Port_Ctype_struct*)
_tmpA;if(_tmpD->tag != 2)goto _LL7;}_LL6: {
# 170
const char*_tmp537;return(_tmp537="thin",_tag_dyneither(_tmp537,sizeof(char),5));}
_LL7: {struct Cyc_Port_Fat_ct_Port_Ctype_struct*_tmpE=(struct Cyc_Port_Fat_ct_Port_Ctype_struct*)
_tmpA;if(_tmpE->tag != 3)goto _LL9;}_LL8: {
# 171
const char*_tmp538;return(_tmp538="fat",_tag_dyneither(_tmp538,sizeof(char),4));}
_LL9: {struct Cyc_Port_Void_ct_Port_Ctype_struct*_tmpF=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)
_tmpA;if(_tmpF->tag != 4)goto _LLB;}_LLA: {
# 172
const char*_tmp539;return(_tmp539="void",_tag_dyneither(_tmp539,sizeof(char),5));}
_LLB: {struct Cyc_Port_Zero_ct_Port_Ctype_struct*_tmp10=(struct Cyc_Port_Zero_ct_Port_Ctype_struct*)
_tmpA;if(_tmp10->tag != 5)goto _LLD;}_LLC: {
# 173
const char*_tmp53A;return(_tmp53A="zero",_tag_dyneither(_tmp53A,sizeof(char),5));}
_LLD: {struct Cyc_Port_Arith_ct_Port_Ctype_struct*_tmp11=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)
_tmpA;if(_tmp11->tag != 6)goto _LLF;}_LLE: {
# 174
const char*_tmp53B;return(_tmp53B="arith",_tag_dyneither(_tmp53B,sizeof(char),6));}
_LLF: {struct Cyc_Port_Heap_ct_Port_Ctype_struct*_tmp12=(struct Cyc_Port_Heap_ct_Port_Ctype_struct*)
_tmpA;if(_tmp12->tag != 7)goto _LL11;}_LL10: {
# 175
const char*_tmp53C;return(_tmp53C="heap",_tag_dyneither(_tmp53C,sizeof(char),5));}
_LL11: {struct Cyc_Port_Zterm_ct_Port_Ctype_struct*_tmp13=(struct Cyc_Port_Zterm_ct_Port_Ctype_struct*)
_tmpA;if(_tmp13->tag != 8)goto _LL13;}_LL12: {
# 176
const char*_tmp53D;return(_tmp53D="ZT",_tag_dyneither(_tmp53D,sizeof(char),3));}
_LL13: {struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*_tmp14=(struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*)
_tmpA;if(_tmp14->tag != 9)goto _LL15;}_LL14: {
# 177
const char*_tmp53E;return(_tmp53E="NZT",_tag_dyneither(_tmp53E,sizeof(char),4));}
_LL15: {struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmp15=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)
_tmpA;if(_tmp15->tag != 10)goto _LL17;else{_tmp16=_tmp15->f1;}}_LL16: {
# 178
const char*_tmp542;void*_tmp541[1];struct Cyc_String_pa_PrintArg_struct _tmp540;
return(struct _dyneither_ptr)((_tmp540.tag=0,((_tmp540.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp16),((_tmp541[0]=& _tmp540,Cyc_aprintf(((_tmp542="%s",
_tag_dyneither(_tmp542,sizeof(char),3))),_tag_dyneither(_tmp541,sizeof(void*),1))))))));}
_LL17: {struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp17=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)
_tmpA;if(_tmp17->tag != 11)goto _LL19;else{_tmp18=(void*)_tmp17->f1;_tmp19=(void*)
_tmp17->f2;_tmp1A=(void*)_tmp17->f3;_tmp1B=(void*)_tmp17->f4;_tmp1C=(void*)
_tmp17->f5;}}_LL18: {
# 180
const char*_tmp54A;void*_tmp549[5];struct Cyc_String_pa_PrintArg_struct _tmp548;
struct Cyc_String_pa_PrintArg_struct _tmp547;struct Cyc_String_pa_PrintArg_struct
_tmp546;struct Cyc_String_pa_PrintArg_struct _tmp545;struct Cyc_String_pa_PrintArg_struct
_tmp544;return(struct _dyneither_ptr)((_tmp544.tag=0,((_tmp544.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)
# 182
Cyc_Port_ctype2string(deep,_tmp1C)),((_tmp545.tag=0,((_tmp545.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp1B)),((
_tmp546.tag=0,((_tmp546.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 181
Cyc_Port_ctype2string(deep,_tmp1A)),((_tmp547.tag=0,((_tmp547.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp19)),((
_tmp548.tag=0,((_tmp548.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 180
Cyc_Port_ctype2string(deep,_tmp18)),((_tmp549[0]=& _tmp548,((_tmp549[1]=& _tmp547,((
_tmp549[2]=& _tmp546,((_tmp549[3]=& _tmp545,((_tmp549[4]=& _tmp544,Cyc_aprintf(((
_tmp54A="ptr(%s,%s,%s,%s,%s)",_tag_dyneither(_tmp54A,sizeof(char),20))),
_tag_dyneither(_tmp549,sizeof(void*),5))))))))))))))))))))))))))))))));}_LL19: {
struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp1D=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)
_tmpA;if(_tmp1D->tag != 12)goto _LL1B;else{_tmp1E=(void*)_tmp1D->f1;_tmp1F=(void*)
_tmp1D->f2;_tmp20=(void*)_tmp1D->f3;}}_LL1A: {
# 184
const char*_tmp550;void*_tmp54F[3];struct Cyc_String_pa_PrintArg_struct _tmp54E;
struct Cyc_String_pa_PrintArg_struct _tmp54D;struct Cyc_String_pa_PrintArg_struct
_tmp54C;return(struct _dyneither_ptr)((_tmp54C.tag=0,((_tmp54C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)
# 185
Cyc_Port_ctype2string(deep,_tmp20)),((_tmp54D.tag=0,((_tmp54D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp1F)),((
_tmp54E.tag=0,((_tmp54E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 184
Cyc_Port_ctype2string(deep,_tmp1E)),((_tmp54F[0]=& _tmp54E,((_tmp54F[1]=& _tmp54D,((
_tmp54F[2]=& _tmp54C,Cyc_aprintf(((_tmp550="array(%s,%s,%s)",_tag_dyneither(
_tmp550,sizeof(char),16))),_tag_dyneither(_tmp54F,sizeof(void*),3))))))))))))))))))));}
_LL1B: {struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmp21=(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)
_tmpA;if(_tmp21->tag != 13)goto _LL1D;else{_tmp22=_tmp21->f1;_tmp23=*_tmp22;_tmp24=
_tmp23.f1;_tmp25=_tmp23.f2;}}_LL1C: {
# 187
const char*_tmp552;const char*_tmp551;struct _dyneither_ptr s=_tmp24->kind == Cyc_Absyn_StructA?(
_tmp552="struct",_tag_dyneither(_tmp552,sizeof(char),7)):((_tmp551="union",
_tag_dyneither(_tmp551,sizeof(char),6)));
# 188
if(!deep){
# 189
const char*_tmp557;void*_tmp556[2];struct Cyc_String_pa_PrintArg_struct _tmp555;
struct Cyc_String_pa_PrintArg_struct _tmp554;return(struct _dyneither_ptr)((_tmp554.tag=
0,((_tmp554.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp24->name)),((_tmp555.tag=0,((_tmp555.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmp556[0]=& _tmp555,((_tmp556[1]=& _tmp554,Cyc_aprintf(((
_tmp557="%s %s",_tag_dyneither(_tmp557,sizeof(char),6))),_tag_dyneither(_tmp556,
sizeof(void*),2))))))))))))));}else{
# 191
const char*_tmp55D;void*_tmp55C[3];struct Cyc_String_pa_PrintArg_struct _tmp55B;
struct Cyc_String_pa_PrintArg_struct _tmp55A;struct Cyc_String_pa_PrintArg_struct
_tmp559;return(struct _dyneither_ptr)((_tmp559.tag=0,((_tmp559.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)
# 192
Cyc_Port_cfields2string(0,_tmp25)),((_tmp55A.tag=0,((_tmp55A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)
# 191
Cyc_Absynpp_qvar2string(_tmp24->name)),((_tmp55B.tag=0,((_tmp55B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)s),((_tmp55C[0]=& _tmp55B,((_tmp55C[1]=&
_tmp55A,((_tmp55C[2]=& _tmp559,Cyc_aprintf(((_tmp55D="%s %s {%s}",_tag_dyneither(
_tmp55D,sizeof(char),11))),_tag_dyneither(_tmp55C,sizeof(void*),3))))))))))))))))))));}}
_LL1D: {struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmp26=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)
_tmpA;if(_tmp26->tag != 14)goto _LL1F;else{_tmp27=_tmp26->f1;_tmp28=_tmp26->f2;if(
_tmp28 == 0)goto _LL1F;_tmp29=*_tmp28;}}_LL1E:
# 193
 return Cyc_Port_ctype2string(deep,_tmp29);_LL1F: {struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*
_tmp2A=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpA;if(_tmp2A->tag != 
14)goto _LL21;else{_tmp2B=_tmp2A->f1;}}_LL20: {
# 195
const char*_tmp561;void*_tmp560[1];struct Cyc_String_pa_PrintArg_struct _tmp55F;
return(struct _dyneither_ptr)((_tmp55F.tag=0,((_tmp55F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Port_cfields2string(deep,_tmp2B)),((_tmp560[0]=& _tmp55F,
Cyc_aprintf(((_tmp561="aggr {%s}",_tag_dyneither(_tmp561,sizeof(char),10))),
_tag_dyneither(_tmp560,sizeof(void*),1))))))));}_LL21: {struct Cyc_Port_Fn_ct_Port_Ctype_struct*
_tmp2C=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpA;if(_tmp2C->tag != 15)goto
_LL23;else{_tmp2D=(void*)_tmp2C->f1;_tmp2E=_tmp2C->f2;}}_LL22: {
# 197
const char*_tmp566;void*_tmp565[2];struct Cyc_String_pa_PrintArg_struct _tmp564;
struct Cyc_String_pa_PrintArg_struct _tmp563;return(struct _dyneither_ptr)((_tmp563.tag=
0,((_tmp563.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(
deep,_tmp2D)),((_tmp564.tag=0,((_tmp564.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Port_ctypes2string(deep,_tmp2E)),((_tmp565[0]=& _tmp564,((
_tmp565[1]=& _tmp563,Cyc_aprintf(((_tmp566="fn(%s)->%s",_tag_dyneither(_tmp566,
sizeof(char),11))),_tag_dyneither(_tmp565,sizeof(void*),2))))))))))))));}_LL23: {
struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmp2F=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)
_tmpA;if(_tmp2F->tag != 16)goto _LL0;else{_tmp30=_tmp2F->f1;}}_LL24:
# 199
 if((unsigned int)_tmp30->eq)
# 200
return Cyc_Port_ctype2string(deep,*((void**)_check_null(_tmp30->eq)));else{
# 201
if(!deep  || _tmp30->uppers == 0  && _tmp30->lowers == 0){
# 202
const char*_tmp56A;void*_tmp569[1];struct Cyc_Int_pa_PrintArg_struct _tmp568;
return(struct _dyneither_ptr)((_tmp568.tag=1,((_tmp568.f1=(unsigned long)_tmp30->id,((
_tmp569[0]=& _tmp568,Cyc_aprintf(((_tmp56A="var(%d)",_tag_dyneither(_tmp56A,
sizeof(char),8))),_tag_dyneither(_tmp569,sizeof(void*),1))))))));}else{
# 203
const char*_tmp570;void*_tmp56F[3];struct Cyc_String_pa_PrintArg_struct _tmp56E;
struct Cyc_Int_pa_PrintArg_struct _tmp56D;struct Cyc_String_pa_PrintArg_struct
_tmp56C;return(struct _dyneither_ptr)((_tmp56C.tag=0,((_tmp56C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)
# 206
Cyc_Port_ctypes2string(0,_tmp30->uppers)),((_tmp56D.tag=1,((_tmp56D.f1=(
unsigned long)_tmp30->id,((_tmp56E.tag=0,((_tmp56E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)
# 204
Cyc_Port_ctypes2string(0,_tmp30->lowers)),((_tmp56F[0]=& _tmp56E,((_tmp56F[1]=&
_tmp56D,((_tmp56F[2]=& _tmp56C,Cyc_aprintf(((_tmp570="var([%s]<=%d<=[%s])",
_tag_dyneither(_tmp570,sizeof(char),20))),_tag_dyneither(_tmp56F,sizeof(void*),3))))))))))))))))))));}}
_LL0:;}
# 209
static struct _dyneither_ptr*Cyc_Port_ctype2stringptr(int deep,void*t){struct
_dyneither_ptr*_tmp571;return(_tmp571=_cycalloc(sizeof(*_tmp571)),((_tmp571[0]=
Cyc_Port_ctype2string(deep,t),_tmp571)));}
# 210
struct Cyc_List_List*Cyc_Port_sep(struct _dyneither_ptr s,struct Cyc_List_List*xs){
# 211
struct _dyneither_ptr*_tmp572;struct _dyneither_ptr*_tmp65=(_tmp572=_cycalloc(
sizeof(*_tmp572)),((_tmp572[0]=s,_tmp572)));
# 212
if(xs == 0)return xs;{
# 213
struct Cyc_List_List*_tmp66=xs;
# 214
struct Cyc_List_List*_tmp67=xs->tl;
# 215
for(0;_tmp67 != 0;(_tmp66=_tmp67,_tmp67=_tmp67->tl)){
# 216
struct Cyc_List_List*_tmp573;((struct Cyc_List_List*)_check_null(_tmp66))->tl=((
_tmp573=_cycalloc(sizeof(*_tmp573)),((_tmp573->hd=_tmp65,((_tmp573->tl=_tmp67,
_tmp573))))));}
# 218
return xs;};}struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
# 220
static struct _dyneither_ptr*Cyc_Port_cfield2stringptr(int deep,struct Cyc_Port_Cfield*
f){
# 221
const char*_tmp579;void*_tmp578[3];struct Cyc_String_pa_PrintArg_struct _tmp577;
struct Cyc_String_pa_PrintArg_struct _tmp576;struct Cyc_String_pa_PrintArg_struct
_tmp575;struct _dyneither_ptr s=(struct _dyneither_ptr)(
# 222
(_tmp575.tag=0,((_tmp575.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(
deep,f->type)),((_tmp576.tag=0,((_tmp576.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f->f),((_tmp577.tag=0,((_tmp577.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Port_ctype2string(deep,f->qual)),((_tmp578[0]=& _tmp577,((
_tmp578[1]=& _tmp576,((_tmp578[2]=& _tmp575,Cyc_aprintf(((_tmp579="%s %s: %s",
_tag_dyneither(_tmp579,sizeof(char),10))),_tag_dyneither(_tmp578,sizeof(void*),3))))))))))))))))))));
# 223
struct _dyneither_ptr*_tmp57A;return(_tmp57A=_cycalloc(sizeof(*_tmp57A)),((
_tmp57A[0]=s,_tmp57A)));}
# 225
static struct _dyneither_ptr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*ts){
# 226
const char*_tmp57B;return(struct _dyneither_ptr)Cyc_strconcat_l(Cyc_Port_sep(((
_tmp57B=",",_tag_dyneither(_tmp57B,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Port_ctype2stringptr,deep,ts)));}struct Cyc_Port_Cfield;
# 228
static struct _dyneither_ptr Cyc_Port_cfields2string(int deep,struct Cyc_List_List*
fs){
# 229
const char*_tmp57C;return(struct _dyneither_ptr)Cyc_strconcat_l(Cyc_Port_sep(((
_tmp57C=";",_tag_dyneither(_tmp57C,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr*(*f)(int,struct Cyc_Port_Cfield*),int env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Port_cfield2stringptr,deep,fs)));}
# 234
static void*Cyc_Port_notconst_ct(){return(void*)& Cyc_Port_Notconst_ct_val;}
# 235
static void*Cyc_Port_const_ct(){return(void*)& Cyc_Port_Const_ct_val;}
# 236
static void*Cyc_Port_thin_ct(){return(void*)& Cyc_Port_Thin_ct_val;}
# 237
static void*Cyc_Port_fat_ct(){return(void*)& Cyc_Port_Fat_ct_val;}
# 238
static void*Cyc_Port_void_ct(){return(void*)& Cyc_Port_Void_ct_val;}
# 239
static void*Cyc_Port_zero_ct(){return(void*)& Cyc_Port_Zero_ct_val;}
# 240
static void*Cyc_Port_arith_ct(){return(void*)& Cyc_Port_Arith_ct_val;}
# 241
static void*Cyc_Port_heap_ct(){return(void*)& Cyc_Port_Heap_ct_val;}
# 242
static void*Cyc_Port_zterm_ct(){return(void*)& Cyc_Port_Zterm_ct_val;}
# 243
static void*Cyc_Port_nozterm_ct(){return(void*)& Cyc_Port_Nozterm_ct_val;}
# 244
static void*Cyc_Port_rgnvar_ct(struct _dyneither_ptr*n){struct Cyc_Port_RgnVar_ct_Port_Ctype_struct
_tmp57F;struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmp57E;return(void*)((
_tmp57E=_cycalloc(sizeof(*_tmp57E)),((_tmp57E[0]=((_tmp57F.tag=10,((_tmp57F.f1=n,
_tmp57F)))),_tmp57E))));}
# 245
static void*Cyc_Port_unknown_aggr_ct(struct Cyc_List_List*fs){
# 246
struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct _tmp582;struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*
_tmp581;return(void*)((_tmp581=_cycalloc(sizeof(*_tmp581)),((_tmp581[0]=((
_tmp582.tag=14,((_tmp582.f1=fs,((_tmp582.f2=0,_tmp582)))))),_tmp581))));}
# 248
static void*Cyc_Port_known_aggr_ct(struct _tuple11*p){
# 249
struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct _tmp585;struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*
_tmp584;return(void*)((_tmp584=_cycalloc(sizeof(*_tmp584)),((_tmp584[0]=((
_tmp585.tag=13,((_tmp585.f1=p,_tmp585)))),_tmp584))));}
# 251
static void*Cyc_Port_ptr_ct(void*elt,void*qual,void*ptr_kind,void*r,void*zt){
# 253
struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmp588;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*
_tmp587;return(void*)((_tmp587=_cycalloc(sizeof(*_tmp587)),((_tmp587[0]=((
_tmp588.tag=11,((_tmp588.f1=(void*)elt,((_tmp588.f2=(void*)qual,((_tmp588.f3=(
void*)ptr_kind,((_tmp588.f4=(void*)r,((_tmp588.f5=(void*)zt,_tmp588)))))))))))),
_tmp587))));}
# 255
static void*Cyc_Port_array_ct(void*elt,void*qual,void*zt){
# 256
struct Cyc_Port_Array_ct_Port_Ctype_struct _tmp58B;struct Cyc_Port_Array_ct_Port_Ctype_struct*
_tmp58A;return(void*)((_tmp58A=_cycalloc(sizeof(*_tmp58A)),((_tmp58A[0]=((
_tmp58B.tag=12,((_tmp58B.f1=(void*)elt,((_tmp58B.f2=(void*)qual,((_tmp58B.f3=(
void*)zt,_tmp58B)))))))),_tmp58A))));}
# 258
static void*Cyc_Port_fn_ct(void*return_type,struct Cyc_List_List*args){
# 259
struct Cyc_Port_Fn_ct_Port_Ctype_struct _tmp58E;struct Cyc_Port_Fn_ct_Port_Ctype_struct*
_tmp58D;return(void*)((_tmp58D=_cycalloc(sizeof(*_tmp58D)),((_tmp58D[0]=((
_tmp58E.tag=15,((_tmp58E.f1=(void*)return_type,((_tmp58E.f2=args,_tmp58E)))))),
_tmp58D))));}struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
# 261
static void*Cyc_Port_var(){
# 262
static int counter=0;
# 263
struct Cyc_Port_Var_ct_Port_Ctype_struct _tmp594;struct Cyc_Port_Cvar*_tmp593;
struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmp592;return(void*)((_tmp592=_cycalloc(
sizeof(*_tmp592)),((_tmp592[0]=((_tmp594.tag=16,((_tmp594.f1=((_tmp593=_cycalloc(
sizeof(*_tmp593)),((_tmp593->id=counter ++,((_tmp593->eq=0,((_tmp593->uppers=0,((
_tmp593->lowers=0,_tmp593)))))))))),_tmp594)))),_tmp592))));}
# 265
static void*Cyc_Port_new_var(void*x){
# 266
return Cyc_Port_var();}
# 268
static struct _dyneither_ptr*Cyc_Port_new_region_var(){
# 269
static int counter=0;
# 270
const char*_tmp598;void*_tmp597[1];struct Cyc_Int_pa_PrintArg_struct _tmp596;
struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp596.tag=1,((_tmp596.f1=(
unsigned long)counter ++,((_tmp597[0]=& _tmp596,Cyc_aprintf(((_tmp598="`R%d",
_tag_dyneither(_tmp598,sizeof(char),5))),_tag_dyneither(_tmp597,sizeof(void*),1))))))));
# 271
struct _dyneither_ptr*_tmp599;return(_tmp599=_cycalloc(sizeof(*_tmp599)),((
_tmp599[0]=s,_tmp599)));}
# 276
static int Cyc_Port_unifies(void*t1,void*t2);struct Cyc_Port_Cvar;
# 278
static void*Cyc_Port_compress_ct(void*t){
# 279
void*_tmp85=t;struct Cyc_Port_Cvar*_tmp87;struct Cyc_Port_Cvar _tmp88;void**_tmp89;
void***_tmp8A;struct Cyc_List_List*_tmp8B;struct Cyc_List_List*_tmp8C;void**_tmp8E;
_LL26: {struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmp86=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)
_tmp85;if(_tmp86->tag != 16)goto _LL28;else{_tmp87=_tmp86->f1;_tmp88=*_tmp87;
_tmp89=_tmp88.eq;_tmp8A=(void***)&(*_tmp86->f1).eq;_tmp8B=_tmp88.uppers;_tmp8C=
_tmp88.lowers;}}_LL27: {
# 281
void**_tmp8F=*_tmp8A;
# 282
if((unsigned int)_tmp8F){
# 285
void*r=Cyc_Port_compress_ct(*_tmp8F);
# 286
if(*_tmp8F != r){void**_tmp59A;*_tmp8A=((_tmp59A=_cycalloc(sizeof(*_tmp59A)),((
_tmp59A[0]=r,_tmp59A))));}
# 287
return r;}
# 292
for(0;_tmp8C != 0;_tmp8C=_tmp8C->tl){
# 293
void*_tmp91=(void*)_tmp8C->hd;_LL2D: {struct Cyc_Port_Const_ct_Port_Ctype_struct*
_tmp92=(struct Cyc_Port_Const_ct_Port_Ctype_struct*)_tmp91;if(_tmp92->tag != 0)
goto _LL2F;}_LL2E:
# 294
 goto _LL30;_LL2F: {struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*_tmp93=(struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*)
_tmp91;if(_tmp93->tag != 9)goto _LL31;}_LL30:
# 295
 goto _LL32;_LL31: {struct Cyc_Port_Void_ct_Port_Ctype_struct*_tmp94=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)
_tmp91;if(_tmp94->tag != 4)goto _LL33;}_LL32:
# 296
 goto _LL34;_LL33: {struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmp95=(struct
Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp91;if(_tmp95->tag != 13)goto _LL35;}
_LL34:
# 297
 goto _LL36;_LL35: {struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp96=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)
_tmp91;if(_tmp96->tag != 15)goto _LL37;}_LL36:
# 299
{void**_tmp59B;*_tmp8A=((_tmp59B=_cycalloc(sizeof(*_tmp59B)),((_tmp59B[0]=(void*)
_tmp8C->hd,_tmp59B))));}
# 300
return(void*)_tmp8C->hd;_LL37:;_LL38:
# 302
 goto _LL2C;_LL2C:;}
# 305
for(0;_tmp8B != 0;_tmp8B=_tmp8B->tl){
# 306
void*_tmp98=(void*)_tmp8B->hd;_LL3A: {struct Cyc_Port_Notconst_ct_Port_Ctype_struct*
_tmp99=(struct Cyc_Port_Notconst_ct_Port_Ctype_struct*)_tmp98;if(_tmp99->tag != 1)
goto _LL3C;}_LL3B:
# 307
 goto _LL3D;_LL3C: {struct Cyc_Port_Zterm_ct_Port_Ctype_struct*_tmp9A=(struct Cyc_Port_Zterm_ct_Port_Ctype_struct*)
_tmp98;if(_tmp9A->tag != 8)goto _LL3E;}_LL3D:
# 308
 goto _LL3F;_LL3E: {struct Cyc_Port_Zero_ct_Port_Ctype_struct*_tmp9B=(struct Cyc_Port_Zero_ct_Port_Ctype_struct*)
_tmp98;if(_tmp9B->tag != 5)goto _LL40;}_LL3F:
# 309
 goto _LL41;_LL40: {struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmp9C=(struct
Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp98;if(_tmp9C->tag != 13)goto _LL42;}
_LL41:
# 310
 goto _LL43;_LL42: {struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp9D=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)
_tmp98;if(_tmp9D->tag != 15)goto _LL44;}_LL43:
# 312
{void**_tmp59C;*_tmp8A=((_tmp59C=_cycalloc(sizeof(*_tmp59C)),((_tmp59C[0]=(void*)
_tmp8B->hd,_tmp59C))));}
# 313
return(void*)_tmp8B->hd;_LL44:;_LL45:
# 315
 goto _LL39;_LL39:;}
# 318
return t;}_LL28: {struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmp8D=(struct
Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp85;if(_tmp8D->tag != 14)goto _LL2A;
else{_tmp8E=_tmp8D->f2;}}_LL29:
# 321
 if((unsigned int)_tmp8E)return Cyc_Port_compress_ct(*_tmp8E);else{
# 322
return t;}_LL2A:;_LL2B:
# 325
 return t;_LL25:;}
# 331
static void*Cyc_Port_inst(struct Cyc_Dict_Dict*instenv,void*t){
# 332
void*_tmp9F=Cyc_Port_compress_ct(t);struct _dyneither_ptr*_tmpAE;void*_tmpB0;
void*_tmpB1;void*_tmpB2;void*_tmpB3;void*_tmpB4;void*_tmpB6;void*_tmpB7;void*
_tmpB8;void*_tmpBA;struct Cyc_List_List*_tmpBB;_LL47: {struct Cyc_Port_Const_ct_Port_Ctype_struct*
_tmpA0=(struct Cyc_Port_Const_ct_Port_Ctype_struct*)_tmp9F;if(_tmpA0->tag != 0)
goto _LL49;}_LL48:
# 333
 goto _LL4A;_LL49: {struct Cyc_Port_Notconst_ct_Port_Ctype_struct*_tmpA1=(struct
Cyc_Port_Notconst_ct_Port_Ctype_struct*)_tmp9F;if(_tmpA1->tag != 1)goto _LL4B;}
_LL4A:
# 334
 goto _LL4C;_LL4B: {struct Cyc_Port_Thin_ct_Port_Ctype_struct*_tmpA2=(struct Cyc_Port_Thin_ct_Port_Ctype_struct*)
_tmp9F;if(_tmpA2->tag != 2)goto _LL4D;}_LL4C:
# 335
 goto _LL4E;_LL4D: {struct Cyc_Port_Fat_ct_Port_Ctype_struct*_tmpA3=(struct Cyc_Port_Fat_ct_Port_Ctype_struct*)
_tmp9F;if(_tmpA3->tag != 3)goto _LL4F;}_LL4E:
# 336
 goto _LL50;_LL4F: {struct Cyc_Port_Void_ct_Port_Ctype_struct*_tmpA4=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)
_tmp9F;if(_tmpA4->tag != 4)goto _LL51;}_LL50:
# 337
 goto _LL52;_LL51: {struct Cyc_Port_Zero_ct_Port_Ctype_struct*_tmpA5=(struct Cyc_Port_Zero_ct_Port_Ctype_struct*)
_tmp9F;if(_tmpA5->tag != 5)goto _LL53;}_LL52:
# 338
 goto _LL54;_LL53: {struct Cyc_Port_Arith_ct_Port_Ctype_struct*_tmpA6=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)
_tmp9F;if(_tmpA6->tag != 6)goto _LL55;}_LL54:
# 339
 goto _LL56;_LL55: {struct Cyc_Port_Zterm_ct_Port_Ctype_struct*_tmpA7=(struct Cyc_Port_Zterm_ct_Port_Ctype_struct*)
_tmp9F;if(_tmpA7->tag != 8)goto _LL57;}_LL56:
# 340
 goto _LL58;_LL57: {struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*_tmpA8=(struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*)
_tmp9F;if(_tmpA8->tag != 9)goto _LL59;}_LL58:
# 341
 goto _LL5A;_LL59: {struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmpA9=(
struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9F;if(_tmpA9->tag != 14)goto
_LL5B;}_LL5A:
# 342
 goto _LL5C;_LL5B: {struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmpAA=(struct
Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp9F;if(_tmpAA->tag != 13)goto _LL5D;}
_LL5C:
# 343
 goto _LL5E;_LL5D: {struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmpAB=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)
_tmp9F;if(_tmpAB->tag != 16)goto _LL5F;}_LL5E:
# 344
 goto _LL60;_LL5F: {struct Cyc_Port_Heap_ct_Port_Ctype_struct*_tmpAC=(struct Cyc_Port_Heap_ct_Port_Ctype_struct*)
_tmp9F;if(_tmpAC->tag != 7)goto _LL61;}_LL60:
# 345
 return t;_LL61: {struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmpAD=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)
_tmp9F;if(_tmpAD->tag != 10)goto _LL63;else{_tmpAE=_tmpAD->f1;}}_LL62:
# 347
 if(!((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*
instenv,_tmpAE))
# 348
*instenv=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,
void*v))Cyc_Dict_insert)(*instenv,_tmpAE,Cyc_Port_var());
# 349
return((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*
instenv,_tmpAE);_LL63: {struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpAF=(struct
Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F;if(_tmpAF->tag != 11)goto _LL65;else{
_tmpB0=(void*)_tmpAF->f1;_tmpB1=(void*)_tmpAF->f2;_tmpB2=(void*)_tmpAF->f3;
_tmpB3=(void*)_tmpAF->f4;_tmpB4=(void*)_tmpAF->f5;}}_LL64: {
# 351
void*_tmpBE;void*_tmpBF;struct _tuple10 _tmp59D;struct _tuple10 _tmpBD=(_tmp59D.f1=
Cyc_Port_inst(instenv,_tmpB0),((_tmp59D.f2=Cyc_Port_inst(instenv,_tmpB3),_tmp59D)));
_tmpBE=_tmpBD.f1;_tmpBF=_tmpBD.f2;
# 352
if(_tmpBE == _tmpB0  && _tmpBF == _tmpB3)return t;{
# 353
struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmp5A0;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*
_tmp59F;return(void*)((_tmp59F=_cycalloc(sizeof(*_tmp59F)),((_tmp59F[0]=((
_tmp5A0.tag=11,((_tmp5A0.f1=(void*)_tmpBE,((_tmp5A0.f2=(void*)_tmpB1,((_tmp5A0.f3=(
void*)_tmpB2,((_tmp5A0.f4=(void*)_tmpBF,((_tmp5A0.f5=(void*)_tmpB4,_tmp5A0)))))))))))),
_tmp59F))));};}_LL65: {struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmpB5=(struct
Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9F;if(_tmpB5->tag != 12)goto _LL67;else{
_tmpB6=(void*)_tmpB5->f1;_tmpB7=(void*)_tmpB5->f2;_tmpB8=(void*)_tmpB5->f3;}}
_LL66: {
# 355
void*_tmpC2=Cyc_Port_inst(instenv,_tmpB6);
# 356
if(_tmpC2 == _tmpB6)return t;{
# 357
struct Cyc_Port_Array_ct_Port_Ctype_struct _tmp5A3;struct Cyc_Port_Array_ct_Port_Ctype_struct*
_tmp5A2;return(void*)((_tmp5A2=_cycalloc(sizeof(*_tmp5A2)),((_tmp5A2[0]=((
_tmp5A3.tag=12,((_tmp5A3.f1=(void*)_tmpC2,((_tmp5A3.f2=(void*)_tmpB7,((_tmp5A3.f3=(
void*)_tmpB8,_tmp5A3)))))))),_tmp5A2))));};}_LL67: {struct Cyc_Port_Fn_ct_Port_Ctype_struct*
_tmpB9=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp9F;if(_tmpB9->tag != 15)goto
_LL46;else{_tmpBA=(void*)_tmpB9->f1;_tmpBB=_tmpB9->f2;}}_LL68: {
# 359
struct Cyc_Port_Fn_ct_Port_Ctype_struct _tmp5A6;struct Cyc_Port_Fn_ct_Port_Ctype_struct*
_tmp5A5;return(void*)((_tmp5A5=_cycalloc(sizeof(*_tmp5A5)),((_tmp5A5[0]=((
_tmp5A6.tag=15,((_tmp5A6.f1=(void*)Cyc_Port_inst(instenv,_tmpBA),((_tmp5A6.f2=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Dict_Dict*,void*),struct Cyc_Dict_Dict*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_inst,instenv,_tmpBB),_tmp5A6)))))),
_tmp5A5))));}_LL46:;}
# 363
void*Cyc_Port_instantiate(void*t){
# 364
struct Cyc_Dict_Dict*_tmp5A7;return Cyc_Port_inst(((_tmp5A7=_cycalloc(sizeof(*
_tmp5A7)),((_tmp5A7[0]=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),_tmp5A7)))),t);}
# 370
struct Cyc_List_List*Cyc_Port_filter_self(void*t,struct Cyc_List_List*ts){
# 371
int found=0;
# 372
{struct Cyc_List_List*_tmpC8=ts;for(0;(unsigned int)_tmpC8;_tmpC8=_tmpC8->tl){
# 373
void*_tmpC9=Cyc_Port_compress_ct((void*)_tmpC8->hd);
# 374
if(t == _tmpC9)found=1;}}
# 376
if(!found)return ts;{
# 377
struct Cyc_List_List*res=0;
# 378
for(0;ts != 0;ts=ts->tl){
# 379
if(t == Cyc_Port_compress_ct((void*)ts->hd))continue;{
# 380
struct Cyc_List_List*_tmp5A8;res=((_tmp5A8=_cycalloc(sizeof(*_tmp5A8)),((_tmp5A8->hd=(
void*)ts->hd,((_tmp5A8->tl=res,_tmp5A8))))));};}
# 382
return res;};}struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
# 387
void Cyc_Port_generalize(int is_rgn,void*t){
# 388
t=Cyc_Port_compress_ct(t);{
# 389
void*_tmpCB=t;struct Cyc_Port_Cvar*_tmpCD;void*_tmpDC;void*_tmpDD;void*_tmpDE;
void*_tmpDF;void*_tmpE0;void*_tmpE2;void*_tmpE3;void*_tmpE4;void*_tmpE6;struct
Cyc_List_List*_tmpE7;_LL6A: {struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmpCC=(
struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmpCB;if(_tmpCC->tag != 16)goto _LL6C;
else{_tmpCD=_tmpCC->f1;}}_LL6B:
# 392
 _tmpCD->uppers=Cyc_Port_filter_self(t,_tmpCD->uppers);
# 393
_tmpCD->lowers=Cyc_Port_filter_self(t,_tmpCD->lowers);
# 394
if(is_rgn){
# 397
if(_tmpCD->uppers == 0  && _tmpCD->lowers == 0){
# 398
Cyc_Port_unifies(t,Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var()));
# 399
return;}
# 402
if((unsigned int)_tmpCD->uppers){
# 403
Cyc_Port_unifies(t,(void*)((struct Cyc_List_List*)_check_null(_tmpCD->uppers))->hd);
# 404
Cyc_Port_generalize(1,t);}else{
# 406
Cyc_Port_unifies(t,(void*)((struct Cyc_List_List*)_check_null(_tmpCD->lowers))->hd);
# 407
Cyc_Port_generalize(1,t);}}
# 410
return;_LL6C: {struct Cyc_Port_Const_ct_Port_Ctype_struct*_tmpCE=(struct Cyc_Port_Const_ct_Port_Ctype_struct*)
_tmpCB;if(_tmpCE->tag != 0)goto _LL6E;}_LL6D:
# 411
 goto _LL6F;_LL6E: {struct Cyc_Port_Notconst_ct_Port_Ctype_struct*_tmpCF=(struct
Cyc_Port_Notconst_ct_Port_Ctype_struct*)_tmpCB;if(_tmpCF->tag != 1)goto _LL70;}
_LL6F:
# 412
 goto _LL71;_LL70: {struct Cyc_Port_Thin_ct_Port_Ctype_struct*_tmpD0=(struct Cyc_Port_Thin_ct_Port_Ctype_struct*)
_tmpCB;if(_tmpD0->tag != 2)goto _LL72;}_LL71:
# 413
 goto _LL73;_LL72: {struct Cyc_Port_Fat_ct_Port_Ctype_struct*_tmpD1=(struct Cyc_Port_Fat_ct_Port_Ctype_struct*)
_tmpCB;if(_tmpD1->tag != 3)goto _LL74;}_LL73:
# 414
 goto _LL75;_LL74: {struct Cyc_Port_Void_ct_Port_Ctype_struct*_tmpD2=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)
_tmpCB;if(_tmpD2->tag != 4)goto _LL76;}_LL75:
# 415
 goto _LL77;_LL76: {struct Cyc_Port_Zero_ct_Port_Ctype_struct*_tmpD3=(struct Cyc_Port_Zero_ct_Port_Ctype_struct*)
_tmpCB;if(_tmpD3->tag != 5)goto _LL78;}_LL77:
# 416
 goto _LL79;_LL78: {struct Cyc_Port_Arith_ct_Port_Ctype_struct*_tmpD4=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)
_tmpCB;if(_tmpD4->tag != 6)goto _LL7A;}_LL79:
# 417
 goto _LL7B;_LL7A: {struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmpD5=(
struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpCB;if(_tmpD5->tag != 14)goto
_LL7C;}_LL7B:
# 418
 goto _LL7D;_LL7C: {struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmpD6=(struct
Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpCB;if(_tmpD6->tag != 13)goto _LL7E;}
_LL7D:
# 419
 goto _LL7F;_LL7E: {struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmpD7=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)
_tmpCB;if(_tmpD7->tag != 10)goto _LL80;}_LL7F:
# 420
 goto _LL81;_LL80: {struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*_tmpD8=(struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*)
_tmpCB;if(_tmpD8->tag != 9)goto _LL82;}_LL81:
# 421
 goto _LL83;_LL82: {struct Cyc_Port_Zterm_ct_Port_Ctype_struct*_tmpD9=(struct Cyc_Port_Zterm_ct_Port_Ctype_struct*)
_tmpCB;if(_tmpD9->tag != 8)goto _LL84;}_LL83:
# 422
 goto _LL85;_LL84: {struct Cyc_Port_Heap_ct_Port_Ctype_struct*_tmpDA=(struct Cyc_Port_Heap_ct_Port_Ctype_struct*)
_tmpCB;if(_tmpDA->tag != 7)goto _LL86;}_LL85:
# 423
 return;_LL86: {struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpDB=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)
_tmpCB;if(_tmpDB->tag != 11)goto _LL88;else{_tmpDC=(void*)_tmpDB->f1;_tmpDD=(void*)
_tmpDB->f2;_tmpDE=(void*)_tmpDB->f3;_tmpDF=(void*)_tmpDB->f4;_tmpE0=(void*)
_tmpDB->f5;}}_LL87:
# 427
 Cyc_Port_generalize(0,_tmpDC);Cyc_Port_generalize(1,_tmpDF);goto _LL69;_LL88: {
struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmpE1=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)
_tmpCB;if(_tmpE1->tag != 12)goto _LL8A;else{_tmpE2=(void*)_tmpE1->f1;_tmpE3=(void*)
_tmpE1->f2;_tmpE4=(void*)_tmpE1->f3;}}_LL89:
# 429
 Cyc_Port_generalize(0,_tmpE2);Cyc_Port_generalize(0,_tmpE3);goto _LL69;_LL8A: {
struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmpE5=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)
_tmpCB;if(_tmpE5->tag != 15)goto _LL69;else{_tmpE6=(void*)_tmpE5->f1;_tmpE7=_tmpE5->f2;}}
_LL8B:
# 431
 Cyc_Port_generalize(0,_tmpE6);((void(*)(void(*f)(int,void*),int env,struct Cyc_List_List*
x))Cyc_List_iter_c)(Cyc_Port_generalize,0,_tmpE7);goto _LL69;_LL69:;};}struct Cyc_Port_Cfield;
struct Cyc_Port_Cfield;
# 437
static int Cyc_Port_occurs(void*v,void*t){
# 438
t=Cyc_Port_compress_ct(t);
# 439
if(t == v)return 1;{
# 440
void*_tmpE8=t;void*_tmpEA;void*_tmpEB;void*_tmpEC;void*_tmpED;void*_tmpEE;void*
_tmpF0;void*_tmpF1;void*_tmpF2;void*_tmpF4;struct Cyc_List_List*_tmpF5;struct
_tuple11*_tmpF7;struct _tuple11 _tmpF8;struct Cyc_List_List*_tmpF9;struct Cyc_List_List*
_tmpFB;_LL8D: {struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpE9=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)
_tmpE8;if(_tmpE9->tag != 11)goto _LL8F;else{_tmpEA=(void*)_tmpE9->f1;_tmpEB=(void*)
_tmpE9->f2;_tmpEC=(void*)_tmpE9->f3;_tmpED=(void*)_tmpE9->f4;_tmpEE=(void*)
_tmpE9->f5;}}_LL8E:
# 442
 return(((Cyc_Port_occurs(v,_tmpEA) || Cyc_Port_occurs(v,_tmpEB)) || Cyc_Port_occurs(
v,_tmpEC)) || Cyc_Port_occurs(v,_tmpED)) || 
# 443
Cyc_Port_occurs(v,_tmpEE);_LL8F: {struct Cyc_Port_Array_ct_Port_Ctype_struct*
_tmpEF=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpE8;if(_tmpEF->tag != 12)
goto _LL91;else{_tmpF0=(void*)_tmpEF->f1;_tmpF1=(void*)_tmpEF->f2;_tmpF2=(void*)
_tmpEF->f3;}}_LL90:
# 445
 return(Cyc_Port_occurs(v,_tmpF0) || Cyc_Port_occurs(v,_tmpF1)) || Cyc_Port_occurs(
v,_tmpF2);_LL91: {struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmpF3=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)
_tmpE8;if(_tmpF3->tag != 15)goto _LL93;else{_tmpF4=(void*)_tmpF3->f1;_tmpF5=_tmpF3->f2;}}
_LL92:
# 447
 if(Cyc_Port_occurs(v,_tmpF4))return 1;
# 448
for(0;(unsigned int)_tmpF5;_tmpF5=_tmpF5->tl){
# 449
if(Cyc_Port_occurs(v,(void*)_tmpF5->hd))return 1;}
# 450
return 0;_LL93: {struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmpF6=(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)
_tmpE8;if(_tmpF6->tag != 13)goto _LL95;else{_tmpF7=_tmpF6->f1;_tmpF8=*_tmpF7;
_tmpF9=_tmpF8.f2;}}_LL94:
# 451
 return 0;_LL95: {struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmpFA=(struct
Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpE8;if(_tmpFA->tag != 14)goto _LL97;
else{_tmpFB=_tmpFA->f1;}}_LL96:
# 453
 for(0;(unsigned int)_tmpFB;_tmpFB=_tmpFB->tl){
# 454
if(Cyc_Port_occurs(v,((struct Cyc_Port_Cfield*)_tmpFB->hd)->qual) || Cyc_Port_occurs(
v,((struct Cyc_Port_Cfield*)_tmpFB->hd)->type))return 1;}
# 455
return 0;_LL97:;_LL98:
# 456
 return 0;_LL8C:;};}char Cyc_Port_Unify_ct[9]="Unify_ct";struct Cyc_Port_Unify_ct_exn_struct{
char*tag;};
# 465
struct Cyc_Port_Unify_ct_exn_struct Cyc_Port_Unify_ct_val={Cyc_Port_Unify_ct};
# 467
static int Cyc_Port_leq(void*t1,void*t2);
# 468
static void Cyc_Port_unify_cts(struct Cyc_List_List*t1,struct Cyc_List_List*t2);
# 469
static struct Cyc_List_List*Cyc_Port_merge_fields(struct Cyc_List_List*fs1,struct
Cyc_List_List*fs2,int allow_subset);struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
# 471
static void Cyc_Port_unify_ct(void*t1,void*t2){
# 472
t1=Cyc_Port_compress_ct(t1);
# 473
t2=Cyc_Port_compress_ct(t2);
# 474
if(t1 == t2)return;{
# 475
struct _tuple10 _tmp5A9;struct _tuple10 _tmpFE=(_tmp5A9.f1=t1,((_tmp5A9.f2=t2,
_tmp5A9)));void*_tmpFF;struct Cyc_Port_Cvar*_tmp101;void*_tmp102;struct Cyc_Port_Cvar*
_tmp104;void*_tmp105;void*_tmp107;void*_tmp108;void*_tmp109;void*_tmp10A;void*
_tmp10B;void*_tmp10C;void*_tmp10E;void*_tmp10F;void*_tmp110;void*_tmp111;void*
_tmp112;void*_tmp113;struct _dyneither_ptr*_tmp115;struct _dyneither_ptr _tmp116;
void*_tmp117;struct _dyneither_ptr*_tmp119;struct _dyneither_ptr _tmp11A;void*
_tmp11B;void*_tmp11D;void*_tmp11E;void*_tmp11F;void*_tmp120;void*_tmp122;void*
_tmp123;void*_tmp124;void*_tmp125;void*_tmp127;struct Cyc_List_List*_tmp128;void*
_tmp129;void*_tmp12B;struct Cyc_List_List*_tmp12C;void*_tmp12D;struct _tuple11*
_tmp12F;void*_tmp130;struct _tuple11*_tmp132;void*_tmp133;struct Cyc_List_List*
_tmp135;void**_tmp136;void***_tmp137;void*_tmp138;struct Cyc_List_List*_tmp13A;
void**_tmp13B;void***_tmp13C;void*_tmp13D;struct Cyc_List_List*_tmp13F;void**
_tmp140;void***_tmp141;void*_tmp142;struct _tuple11*_tmp144;struct _tuple11 _tmp145;
struct Cyc_Absyn_Aggrdecl*_tmp146;struct Cyc_List_List*_tmp147;void*_tmp148;struct
_tuple11*_tmp14A;struct _tuple11 _tmp14B;struct Cyc_Absyn_Aggrdecl*_tmp14C;struct
Cyc_List_List*_tmp14D;void*_tmp14E;struct Cyc_List_List*_tmp150;void**_tmp151;
void***_tmp152;_LL9A: _tmpFF=_tmpFE.f1;{struct Cyc_Port_Var_ct_Port_Ctype_struct*
_tmp100=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmpFF;if(_tmp100->tag != 16)
goto _LL9C;else{_tmp101=_tmp100->f1;}};_LL9B:
# 477
 if(!Cyc_Port_occurs(t1,t2)){
# 480
{struct Cyc_List_List*_tmp153=Cyc_Port_filter_self(t1,_tmp101->uppers);for(0;(
unsigned int)_tmp153;_tmp153=_tmp153->tl){
# 481
if(!Cyc_Port_leq(t2,(void*)_tmp153->hd))(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}
# 482
{struct Cyc_List_List*_tmp154=Cyc_Port_filter_self(t1,_tmp101->lowers);for(0;(
unsigned int)_tmp154;_tmp154=_tmp154->tl){
# 483
if(!Cyc_Port_leq((void*)_tmp154->hd,t2))(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}
# 484
{void**_tmp5AA;_tmp101->eq=((_tmp5AA=_cycalloc(sizeof(*_tmp5AA)),((_tmp5AA[0]=
t2,_tmp5AA))));}
# 485
return;}else{
# 486
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}_LL9C: _tmp102=_tmpFE.f2;{struct Cyc_Port_Var_ct_Port_Ctype_struct*
_tmp103=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp102;if(_tmp103->tag != 16)
goto _LL9E;else{_tmp104=_tmp103->f1;}};_LL9D:
# 487
 Cyc_Port_unify_ct(t2,t1);return;_LL9E: _tmp105=_tmpFE.f1;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*
_tmp106=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp105;if(_tmp106->tag != 11)
goto _LLA0;else{_tmp107=(void*)_tmp106->f1;_tmp108=(void*)_tmp106->f2;_tmp109=(
void*)_tmp106->f3;_tmp10A=(void*)_tmp106->f4;_tmp10B=(void*)_tmp106->f5;}};
_tmp10C=_tmpFE.f2;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp10D=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)
_tmp10C;if(_tmp10D->tag != 11)goto _LLA0;else{_tmp10E=(void*)_tmp10D->f1;_tmp10F=(
void*)_tmp10D->f2;_tmp110=(void*)_tmp10D->f3;_tmp111=(void*)_tmp10D->f4;_tmp112=(
void*)_tmp10D->f5;}};_LL9F:
# 489
 Cyc_Port_unify_ct(_tmp107,_tmp10E);Cyc_Port_unify_ct(_tmp108,_tmp10F);Cyc_Port_unify_ct(
_tmp109,_tmp110);Cyc_Port_unify_ct(_tmp10A,_tmp111);
# 490
Cyc_Port_unify_ct(_tmp10B,_tmp112);
# 491
return;_LLA0: _tmp113=_tmpFE.f1;{struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*
_tmp114=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp113;if(_tmp114->tag != 
10)goto _LLA2;else{_tmp115=_tmp114->f1;_tmp116=*_tmp115;}};_tmp117=_tmpFE.f2;{
struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmp118=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)
_tmp117;if(_tmp118->tag != 10)goto _LLA2;else{_tmp119=_tmp118->f1;_tmp11A=*_tmp119;}};
_LLA1:
# 493
 if(Cyc_strcmp((struct _dyneither_ptr)_tmp116,(struct _dyneither_ptr)_tmp11A)!= 0)(
int)_throw((void*)& Cyc_Port_Unify_ct_val);
# 494
return;_LLA2: _tmp11B=_tmpFE.f1;{struct Cyc_Port_Array_ct_Port_Ctype_struct*
_tmp11C=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp11B;if(_tmp11C->tag != 12)
goto _LLA4;else{_tmp11D=(void*)_tmp11C->f1;_tmp11E=(void*)_tmp11C->f2;_tmp11F=(
void*)_tmp11C->f3;}};_tmp120=_tmpFE.f2;{struct Cyc_Port_Array_ct_Port_Ctype_struct*
_tmp121=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp120;if(_tmp121->tag != 12)
goto _LLA4;else{_tmp122=(void*)_tmp121->f1;_tmp123=(void*)_tmp121->f2;_tmp124=(
void*)_tmp121->f3;}};_LLA3:
# 496
 Cyc_Port_unify_ct(_tmp11D,_tmp122);Cyc_Port_unify_ct(_tmp11E,_tmp123);Cyc_Port_unify_ct(
_tmp11F,_tmp124);return;_LLA4: _tmp125=_tmpFE.f1;{struct Cyc_Port_Fn_ct_Port_Ctype_struct*
_tmp126=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp125;if(_tmp126->tag != 15)
goto _LLA6;else{_tmp127=(void*)_tmp126->f1;_tmp128=_tmp126->f2;}};_tmp129=_tmpFE.f2;{
struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp12A=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)
_tmp129;if(_tmp12A->tag != 15)goto _LLA6;else{_tmp12B=(void*)_tmp12A->f1;_tmp12C=
_tmp12A->f2;}};_LLA5:
# 498
 Cyc_Port_unify_ct(_tmp127,_tmp12B);Cyc_Port_unify_cts(_tmp128,_tmp12C);return;
_LLA6: _tmp12D=_tmpFE.f1;{struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmp12E=(
struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp12D;if(_tmp12E->tag != 13)goto
_LLA8;else{_tmp12F=_tmp12E->f1;}};_tmp130=_tmpFE.f2;{struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*
_tmp131=(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp130;if(_tmp131->tag
!= 13)goto _LLA8;else{_tmp132=_tmp131->f1;}};_LLA7:
# 500
 if(_tmp12F == _tmp132)return;else{(int)_throw((void*)& Cyc_Port_Unify_ct_val);}
_LLA8: _tmp133=_tmpFE.f1;{struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmp134=(
struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp133;if(_tmp134->tag != 14)
goto _LLAA;else{_tmp135=_tmp134->f1;_tmp136=_tmp134->f2;_tmp137=(void***)& _tmp134->f2;}};
_tmp138=_tmpFE.f2;{struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmp139=(
struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp138;if(_tmp139->tag != 14)
goto _LLAA;else{_tmp13A=_tmp139->f1;_tmp13B=_tmp139->f2;_tmp13C=(void***)& _tmp139->f2;}};
_LLA9: {
# 502
void*_tmp156=Cyc_Port_unknown_aggr_ct(Cyc_Port_merge_fields(_tmp135,_tmp13A,1));
# 503
{void**_tmp5AB;*_tmp137=(*_tmp13C=((_tmp5AB=_cycalloc(sizeof(*_tmp5AB)),((
_tmp5AB[0]=_tmp156,_tmp5AB)))));}
# 504
return;}_LLAA: _tmp13D=_tmpFE.f1;{struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*
_tmp13E=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp13D;if(_tmp13E->tag
!= 14)goto _LLAC;else{_tmp13F=_tmp13E->f1;_tmp140=_tmp13E->f2;_tmp141=(void***)&
_tmp13E->f2;}};_tmp142=_tmpFE.f2;{struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*
_tmp143=(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp142;if(_tmp143->tag
!= 13)goto _LLAC;else{_tmp144=_tmp143->f1;_tmp145=*_tmp144;_tmp146=_tmp145.f1;
_tmp147=_tmp145.f2;}};_LLAB:
# 506
 Cyc_Port_merge_fields(_tmp147,_tmp13F,0);
# 507
{void**_tmp5AC;*_tmp141=((_tmp5AC=_cycalloc(sizeof(*_tmp5AC)),((_tmp5AC[0]=t2,
_tmp5AC))));}
# 508
return;_LLAC: _tmp148=_tmpFE.f1;{struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*
_tmp149=(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp148;if(_tmp149->tag
!= 13)goto _LLAE;else{_tmp14A=_tmp149->f1;_tmp14B=*_tmp14A;_tmp14C=_tmp14B.f1;
_tmp14D=_tmp14B.f2;}};_tmp14E=_tmpFE.f2;{struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*
_tmp14F=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp14E;if(_tmp14F->tag
!= 14)goto _LLAE;else{_tmp150=_tmp14F->f1;_tmp151=_tmp14F->f2;_tmp152=(void***)&
_tmp14F->f2;}};_LLAD:
# 510
 Cyc_Port_merge_fields(_tmp14D,_tmp150,0);
# 511
{void**_tmp5AD;*_tmp152=((_tmp5AD=_cycalloc(sizeof(*_tmp5AD)),((_tmp5AD[0]=t1,
_tmp5AD))));}
# 512
return;_LLAE:;_LLAF:
# 513
(int)_throw((void*)& Cyc_Port_Unify_ct_val);_LL99:;};}
# 517
static void Cyc_Port_unify_cts(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
# 518
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
# 519
Cyc_Port_unify_ct((void*)t1->hd,(void*)t2->hd);}
# 521
if(t1 != 0  || t2 != 0)
# 522
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
# 527
static struct Cyc_List_List*Cyc_Port_merge_fields(struct Cyc_List_List*fs1,struct
Cyc_List_List*fs2,int allow_f1_subset_f2){
# 529
struct Cyc_List_List*common=0;
# 530
{struct Cyc_List_List*_tmp15A=fs2;for(0;(unsigned int)_tmp15A;_tmp15A=_tmp15A->tl){
# 531
struct Cyc_Port_Cfield*_tmp15B=(struct Cyc_Port_Cfield*)_tmp15A->hd;
# 532
int found=0;
# 533
{struct Cyc_List_List*_tmp15C=fs1;for(0;(unsigned int)_tmp15C;_tmp15C=_tmp15C->tl){
# 534
struct Cyc_Port_Cfield*_tmp15D=(struct Cyc_Port_Cfield*)_tmp15C->hd;
# 535
if(Cyc_strptrcmp(_tmp15D->f,_tmp15B->f)== 0){
# 536
{struct Cyc_List_List*_tmp5AE;common=((_tmp5AE=_cycalloc(sizeof(*_tmp5AE)),((
_tmp5AE->hd=_tmp15D,((_tmp5AE->tl=common,_tmp5AE))))));}
# 537
Cyc_Port_unify_ct(_tmp15D->qual,_tmp15B->qual);
# 538
Cyc_Port_unify_ct(_tmp15D->type,_tmp15B->type);
# 539
found=1;
# 540
break;}}}
# 543
if(!found){
# 544
if(allow_f1_subset_f2){
# 545
struct Cyc_List_List*_tmp5AF;common=((_tmp5AF=_cycalloc(sizeof(*_tmp5AF)),((
_tmp5AF->hd=_tmp15B,((_tmp5AF->tl=common,_tmp5AF))))));}else{
# 546
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}}}
# 549
{struct Cyc_List_List*_tmp160=fs1;for(0;(unsigned int)_tmp160;_tmp160=_tmp160->tl){
# 550
struct Cyc_Port_Cfield*_tmp161=(struct Cyc_Port_Cfield*)_tmp160->hd;
# 551
int found=0;
# 552
{struct Cyc_List_List*_tmp162=fs2;for(0;(unsigned int)_tmp162;_tmp162=_tmp162->tl){
# 553
struct Cyc_Port_Cfield*_tmp163=(struct Cyc_Port_Cfield*)_tmp162->hd;
# 554
if(Cyc_strptrcmp(_tmp161->f,_tmp163->f))found=1;}}
# 556
if(!found){
# 557
struct Cyc_List_List*_tmp5B0;common=((_tmp5B0=_cycalloc(sizeof(*_tmp5B0)),((
_tmp5B0->hd=_tmp161,((_tmp5B0->tl=common,_tmp5B0))))));}}}
# 559
return common;}
# 562
static int Cyc_Port_unifies(void*t1,void*t2){
# 563
{struct _handler_cons _tmp165;_push_handler(& _tmp165);{int _tmp167=0;if(setjmp(
_tmp165.handler))_tmp167=1;if(!_tmp167){
# 569
Cyc_Port_unify_ct(t1,t2);;_pop_handler();}else{void*_tmp166=(void*)_exn_thrown;
void*_tmp169=_tmp166;_LLB1: {struct Cyc_Port_Unify_ct_exn_struct*_tmp16A=(struct
Cyc_Port_Unify_ct_exn_struct*)_tmp169;if(_tmp16A->tag != Cyc_Port_Unify_ct)goto
_LLB3;}_LLB2:
# 576
 return 0;_LLB3:;_LLB4:(void)_throw(_tmp169);_LLB0:;}};}
# 578
return 1;}struct _tuple12{void*f1;void*f2;void*f3;void*f4;void*f5;};
# 584
static struct Cyc_List_List*Cyc_Port_insert_upper(void*v,void*t,struct Cyc_List_List**
uppers){
# 586
t=Cyc_Port_compress_ct(t);
# 587
{void*_tmp16B=t;_LLB6: {struct Cyc_Port_Notconst_ct_Port_Ctype_struct*_tmp16C=(
struct Cyc_Port_Notconst_ct_Port_Ctype_struct*)_tmp16B;if(_tmp16C->tag != 1)goto
_LLB8;}_LLB7:
# 590
 goto _LLB9;_LLB8: {struct Cyc_Port_Zterm_ct_Port_Ctype_struct*_tmp16D=(struct Cyc_Port_Zterm_ct_Port_Ctype_struct*)
_tmp16B;if(_tmp16D->tag != 8)goto _LLBA;}_LLB9:
# 591
 goto _LLBB;_LLBA: {struct Cyc_Port_Zero_ct_Port_Ctype_struct*_tmp16E=(struct Cyc_Port_Zero_ct_Port_Ctype_struct*)
_tmp16B;if(_tmp16E->tag != 5)goto _LLBC;}_LLBB:
# 592
 goto _LLBD;_LLBC: {struct Cyc_Port_Thin_ct_Port_Ctype_struct*_tmp16F=(struct Cyc_Port_Thin_ct_Port_Ctype_struct*)
_tmp16B;if(_tmp16F->tag != 2)goto _LLBE;}_LLBD:
# 593
 goto _LLBF;_LLBE: {struct Cyc_Port_Fat_ct_Port_Ctype_struct*_tmp170=(struct Cyc_Port_Fat_ct_Port_Ctype_struct*)
_tmp16B;if(_tmp170->tag != 3)goto _LLC0;}_LLBF:
# 594
 goto _LLC1;_LLC0: {struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp171=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)
_tmp16B;if(_tmp171->tag != 12)goto _LLC2;}_LLC1:
# 595
 goto _LLC3;_LLC2: {struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmp172=(struct
Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp16B;if(_tmp172->tag != 13)goto _LLC4;}
_LLC3:
# 596
 goto _LLC5;_LLC4: {struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp173=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)
_tmp16B;if(_tmp173->tag != 15)goto _LLC6;}_LLC5:
# 597
 goto _LLC7;_LLC6: {struct Cyc_Port_Heap_ct_Port_Ctype_struct*_tmp174=(struct Cyc_Port_Heap_ct_Port_Ctype_struct*)
_tmp16B;if(_tmp174->tag != 7)goto _LLC8;}_LLC7:
# 601
*uppers=0;
# 602
Cyc_Port_unifies(v,t);
# 603
return*uppers;_LLC8: {struct Cyc_Port_Void_ct_Port_Ctype_struct*_tmp175=(struct
Cyc_Port_Void_ct_Port_Ctype_struct*)_tmp16B;if(_tmp175->tag != 4)goto _LLCA;}_LLC9:
# 606
 goto _LLCB;_LLCA: {struct Cyc_Port_Const_ct_Port_Ctype_struct*_tmp176=(struct Cyc_Port_Const_ct_Port_Ctype_struct*)
_tmp16B;if(_tmp176->tag != 0)goto _LLCC;}_LLCB:
# 607
 goto _LLCD;_LLCC: {struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*_tmp177=(struct
Cyc_Port_Nozterm_ct_Port_Ctype_struct*)_tmp16B;if(_tmp177->tag != 9)goto _LLCE;}
_LLCD:
# 609
 return*uppers;_LLCE:;_LLCF:
# 610
 goto _LLB5;_LLB5:;}
# 613
{struct Cyc_List_List*_tmp178=*uppers;for(0;(unsigned int)_tmp178;_tmp178=
_tmp178->tl){
# 614
void*_tmp179=Cyc_Port_compress_ct((void*)_tmp178->hd);
# 616
if(t == _tmp179)return*uppers;{
# 617
struct _tuple10 _tmp5B1;struct _tuple10 _tmp17B=(_tmp5B1.f1=t,((_tmp5B1.f2=_tmp179,
_tmp5B1)));void*_tmp17C;void*_tmp17E;void*_tmp180;void*_tmp182;void*_tmp184;void*
_tmp186;void*_tmp188;void*_tmp18A;void*_tmp18B;void*_tmp18C;void*_tmp18D;void*
_tmp18E;void*_tmp18F;void*_tmp191;void*_tmp192;void*_tmp193;void*_tmp194;void*
_tmp195;_LLD1: _tmp17C=_tmp17B.f1;{struct Cyc_Port_Arith_ct_Port_Ctype_struct*
_tmp17D=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmp17C;if(_tmp17D->tag != 6)
goto _LLD3;};_tmp17E=_tmp17B.f2;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp17F=(
struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp17E;if(_tmp17F->tag != 11)goto _LLD3;};
_LLD2:
# 621
 goto _LLD4;_LLD3: _tmp180=_tmp17B.f1;{struct Cyc_Port_Arith_ct_Port_Ctype_struct*
_tmp181=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmp180;if(_tmp181->tag != 6)
goto _LLD5;};_tmp182=_tmp17B.f2;{struct Cyc_Port_Zero_ct_Port_Ctype_struct*_tmp183=(
struct Cyc_Port_Zero_ct_Port_Ctype_struct*)_tmp182;if(_tmp183->tag != 5)goto _LLD5;};
_LLD4:
# 622
 goto _LLD6;_LLD5: _tmp184=_tmp17B.f1;{struct Cyc_Port_Arith_ct_Port_Ctype_struct*
_tmp185=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmp184;if(_tmp185->tag != 6)
goto _LLD7;};_tmp186=_tmp17B.f2;{struct Cyc_Port_Array_ct_Port_Ctype_struct*
_tmp187=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp186;if(_tmp187->tag != 12)
goto _LLD7;};_LLD6:
# 624
 return*uppers;_LLD7: _tmp188=_tmp17B.f1;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*
_tmp189=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp188;if(_tmp189->tag != 11)
goto _LLD9;else{_tmp18A=(void*)_tmp189->f1;_tmp18B=(void*)_tmp189->f2;_tmp18C=(
void*)_tmp189->f3;_tmp18D=(void*)_tmp189->f4;_tmp18E=(void*)_tmp189->f5;}};
_tmp18F=_tmp17B.f2;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp190=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)
_tmp18F;if(_tmp190->tag != 11)goto _LLD9;else{_tmp191=(void*)_tmp190->f1;_tmp192=(
void*)_tmp190->f2;_tmp193=(void*)_tmp190->f3;_tmp194=(void*)_tmp190->f4;_tmp195=(
void*)_tmp190->f5;}};_LLD8: {
# 629
void*_tmp198;void*_tmp199;void*_tmp19A;void*_tmp19B;void*_tmp19C;struct _tuple12
_tmp5B2;struct _tuple12 _tmp197=(_tmp5B2.f1=Cyc_Port_var(),((_tmp5B2.f2=Cyc_Port_var(),((
_tmp5B2.f3=Cyc_Port_var(),((_tmp5B2.f4=Cyc_Port_var(),((_tmp5B2.f5=Cyc_Port_var(),
_tmp5B2)))))))));_tmp198=_tmp197.f1;_tmp199=_tmp197.f2;_tmp19A=_tmp197.f3;
_tmp19B=_tmp197.f4;_tmp19C=_tmp197.f5;{
# 630
struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmp5B5;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*
_tmp5B4;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp19D=(_tmp5B4=_cycalloc(
sizeof(*_tmp5B4)),((_tmp5B4[0]=((_tmp5B5.tag=11,((_tmp5B5.f1=(void*)_tmp198,((
_tmp5B5.f2=(void*)_tmp199,((_tmp5B5.f3=(void*)_tmp19A,((_tmp5B5.f4=(void*)
_tmp19B,((_tmp5B5.f5=(void*)_tmp19C,_tmp5B5)))))))))))),_tmp5B4)));
# 631
Cyc_Port_leq(_tmp198,_tmp18A);Cyc_Port_leq(_tmp198,_tmp191);
# 632
Cyc_Port_leq(_tmp199,_tmp18B);Cyc_Port_leq(_tmp199,_tmp192);
# 633
Cyc_Port_leq(_tmp19A,_tmp18C);Cyc_Port_leq(_tmp19A,_tmp192);
# 634
Cyc_Port_leq(_tmp19B,_tmp18D);Cyc_Port_leq(_tmp19B,_tmp194);
# 635
Cyc_Port_leq(_tmp19C,_tmp18E);Cyc_Port_leq(_tmp19C,_tmp195);
# 636
_tmp178->hd=(void*)((void*)_tmp19D);
# 637
return*uppers;};}_LLD9:;_LLDA:
# 638
 goto _LLD0;_LLD0:;};}}{
# 641
struct Cyc_List_List*_tmp5B6;return(_tmp5B6=_cycalloc(sizeof(*_tmp5B6)),((
_tmp5B6->hd=t,((_tmp5B6->tl=*uppers,_tmp5B6)))));};}
# 646
static struct Cyc_List_List*Cyc_Port_insert_lower(void*v,void*t,struct Cyc_List_List**
lowers){
# 648
t=Cyc_Port_compress_ct(t);
# 649
{void*_tmp1A1=t;_LLDC: {struct Cyc_Port_Const_ct_Port_Ctype_struct*_tmp1A2=(
struct Cyc_Port_Const_ct_Port_Ctype_struct*)_tmp1A1;if(_tmp1A2->tag != 0)goto _LLDE;}
_LLDD:
# 650
 goto _LLDF;_LLDE: {struct Cyc_Port_Zterm_ct_Port_Ctype_struct*_tmp1A3=(struct Cyc_Port_Zterm_ct_Port_Ctype_struct*)
_tmp1A1;if(_tmp1A3->tag != 8)goto _LLE0;}_LLDF:
# 651
 goto _LLE1;_LLE0: {struct Cyc_Port_Thin_ct_Port_Ctype_struct*_tmp1A4=(struct Cyc_Port_Thin_ct_Port_Ctype_struct*)
_tmp1A1;if(_tmp1A4->tag != 2)goto _LLE2;}_LLE1:
# 652
 goto _LLE3;_LLE2: {struct Cyc_Port_Fat_ct_Port_Ctype_struct*_tmp1A5=(struct Cyc_Port_Fat_ct_Port_Ctype_struct*)
_tmp1A1;if(_tmp1A5->tag != 3)goto _LLE4;}_LLE3:
# 653
 goto _LLE5;_LLE4: {struct Cyc_Port_Void_ct_Port_Ctype_struct*_tmp1A6=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)
_tmp1A1;if(_tmp1A6->tag != 4)goto _LLE6;}_LLE5:
# 654
 goto _LLE7;_LLE6: {struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmp1A7=(struct
Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp1A1;if(_tmp1A7->tag != 13)goto _LLE8;}
_LLE7:
# 655
 goto _LLE9;_LLE8: {struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp1A8=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)
_tmp1A1;if(_tmp1A8->tag != 15)goto _LLEA;}_LLE9:
# 656
 goto _LLEB;_LLEA: {struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmp1A9=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)
_tmp1A1;if(_tmp1A9->tag != 10)goto _LLEC;}_LLEB:
# 658
*lowers=0;
# 659
Cyc_Port_unifies(v,t);
# 660
return*lowers;_LLEC: {struct Cyc_Port_Heap_ct_Port_Ctype_struct*_tmp1AA=(struct
Cyc_Port_Heap_ct_Port_Ctype_struct*)_tmp1A1;if(_tmp1AA->tag != 7)goto _LLEE;}_LLED:
# 661
 goto _LLEF;_LLEE: {struct Cyc_Port_Notconst_ct_Port_Ctype_struct*_tmp1AB=(struct
Cyc_Port_Notconst_ct_Port_Ctype_struct*)_tmp1A1;if(_tmp1AB->tag != 1)goto _LLF0;}
_LLEF:
# 662
 goto _LLF1;_LLF0: {struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*_tmp1AC=(struct
Cyc_Port_Nozterm_ct_Port_Ctype_struct*)_tmp1A1;if(_tmp1AC->tag != 9)goto _LLF2;}
_LLF1:
# 664
 return*lowers;_LLF2:;_LLF3:
# 666
 goto _LLDB;_LLDB:;}
# 668
{struct Cyc_List_List*_tmp1AD=*lowers;for(0;(unsigned int)_tmp1AD;_tmp1AD=
_tmp1AD->tl){
# 669
void*_tmp1AE=Cyc_Port_compress_ct((void*)_tmp1AD->hd);
# 670
if(t == _tmp1AE)return*lowers;{
# 671
struct _tuple10 _tmp5B7;struct _tuple10 _tmp1B0=(_tmp5B7.f1=t,((_tmp5B7.f2=_tmp1AE,
_tmp5B7)));void*_tmp1B1;void*_tmp1B3;void*_tmp1B5;void*_tmp1B7;void*_tmp1B9;void*
_tmp1BB;void*_tmp1BD;void*_tmp1BF;void*_tmp1C1;void*_tmp1C3;void*_tmp1C5;void*
_tmp1C6;void*_tmp1C7;void*_tmp1C8;void*_tmp1C9;void*_tmp1CA;void*_tmp1CC;void*
_tmp1CD;void*_tmp1CE;void*_tmp1CF;void*_tmp1D0;_LLF5: _tmp1B1=_tmp1B0.f2;{struct
Cyc_Port_Void_ct_Port_Ctype_struct*_tmp1B2=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)
_tmp1B1;if(_tmp1B2->tag != 4)goto _LLF7;};_LLF6:
# 672
 goto _LLF8;_LLF7: _tmp1B3=_tmp1B0.f1;{struct Cyc_Port_Zero_ct_Port_Ctype_struct*
_tmp1B4=(struct Cyc_Port_Zero_ct_Port_Ctype_struct*)_tmp1B3;if(_tmp1B4->tag != 5)
goto _LLF9;};_tmp1B5=_tmp1B0.f2;{struct Cyc_Port_Arith_ct_Port_Ctype_struct*
_tmp1B6=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmp1B5;if(_tmp1B6->tag != 6)
goto _LLF9;};_LLF8:
# 673
 goto _LLFA;_LLF9: _tmp1B7=_tmp1B0.f1;{struct Cyc_Port_Zero_ct_Port_Ctype_struct*
_tmp1B8=(struct Cyc_Port_Zero_ct_Port_Ctype_struct*)_tmp1B7;if(_tmp1B8->tag != 5)
goto _LLFB;};_tmp1B9=_tmp1B0.f2;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp1BA=(
struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1B9;if(_tmp1BA->tag != 11)goto _LLFB;};
_LLFA:
# 674
 goto _LLFC;_LLFB: _tmp1BB=_tmp1B0.f1;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*
_tmp1BC=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1BB;if(_tmp1BC->tag != 11)
goto _LLFD;};_tmp1BD=_tmp1B0.f2;{struct Cyc_Port_Arith_ct_Port_Ctype_struct*
_tmp1BE=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmp1BD;if(_tmp1BE->tag != 6)
goto _LLFD;};_LLFC:
# 675
 goto _LLFE;_LLFD: _tmp1BF=_tmp1B0.f1;{struct Cyc_Port_Array_ct_Port_Ctype_struct*
_tmp1C0=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp1BF;if(_tmp1C0->tag != 12)
goto _LLFF;};_tmp1C1=_tmp1B0.f2;{struct Cyc_Port_Arith_ct_Port_Ctype_struct*
_tmp1C2=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmp1C1;if(_tmp1C2->tag != 6)
goto _LLFF;};_LLFE:
# 677
 return*lowers;_LLFF: _tmp1C3=_tmp1B0.f1;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*
_tmp1C4=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1C3;if(_tmp1C4->tag != 11)
goto _LL101;else{_tmp1C5=(void*)_tmp1C4->f1;_tmp1C6=(void*)_tmp1C4->f2;_tmp1C7=(
void*)_tmp1C4->f3;_tmp1C8=(void*)_tmp1C4->f4;_tmp1C9=(void*)_tmp1C4->f5;}};
_tmp1CA=_tmp1B0.f2;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp1CB=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)
_tmp1CA;if(_tmp1CB->tag != 11)goto _LL101;else{_tmp1CC=(void*)_tmp1CB->f1;_tmp1CD=(
void*)_tmp1CB->f2;_tmp1CE=(void*)_tmp1CB->f3;_tmp1CF=(void*)_tmp1CB->f4;_tmp1D0=(
void*)_tmp1CB->f5;}};_LL100: {
# 682
void*_tmp1D3;void*_tmp1D4;void*_tmp1D5;void*_tmp1D6;void*_tmp1D7;struct _tuple12
_tmp5B8;struct _tuple12 _tmp1D2=(_tmp5B8.f1=Cyc_Port_var(),((_tmp5B8.f2=Cyc_Port_var(),((
_tmp5B8.f3=Cyc_Port_var(),((_tmp5B8.f4=Cyc_Port_var(),((_tmp5B8.f5=Cyc_Port_var(),
_tmp5B8)))))))));_tmp1D3=_tmp1D2.f1;_tmp1D4=_tmp1D2.f2;_tmp1D5=_tmp1D2.f3;
_tmp1D6=_tmp1D2.f4;_tmp1D7=_tmp1D2.f5;{
# 683
struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmp5BB;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*
_tmp5BA;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp1D8=(_tmp5BA=_cycalloc(
sizeof(*_tmp5BA)),((_tmp5BA[0]=((_tmp5BB.tag=11,((_tmp5BB.f1=(void*)_tmp1D3,((
_tmp5BB.f2=(void*)_tmp1D4,((_tmp5BB.f3=(void*)_tmp1D5,((_tmp5BB.f4=(void*)
_tmp1D6,((_tmp5BB.f5=(void*)_tmp1D7,_tmp5BB)))))))))))),_tmp5BA)));
# 684
Cyc_Port_leq(_tmp1C5,_tmp1D3);Cyc_Port_leq(_tmp1CC,_tmp1D3);
# 685
Cyc_Port_leq(_tmp1C6,_tmp1D4);Cyc_Port_leq(_tmp1CD,_tmp1D4);
# 686
Cyc_Port_leq(_tmp1C7,_tmp1D5);Cyc_Port_leq(_tmp1CD,_tmp1D5);
# 687
Cyc_Port_leq(_tmp1C8,_tmp1D6);Cyc_Port_leq(_tmp1CF,_tmp1D6);
# 688
Cyc_Port_leq(_tmp1C9,_tmp1D7);Cyc_Port_leq(_tmp1D0,_tmp1D7);
# 689
_tmp1AD->hd=(void*)((void*)_tmp1D8);
# 690
return*lowers;};}_LL101:;_LL102:
# 691
 goto _LLF4;_LLF4:;};}}{
# 694
struct Cyc_List_List*_tmp5BC;return(_tmp5BC=_cycalloc(sizeof(*_tmp5BC)),((
_tmp5BC->hd=t,((_tmp5BC->tl=*lowers,_tmp5BC)))));};}struct Cyc_Port_Cvar;struct
Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct
Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
# 701
static int Cyc_Port_leq(void*t1,void*t2){
# 707
if(t1 == t2)return 1;
# 708
t1=Cyc_Port_compress_ct(t1);
# 709
t2=Cyc_Port_compress_ct(t2);{
# 710
struct _tuple10 _tmp5BD;struct _tuple10 _tmp1DD=(_tmp5BD.f1=t1,((_tmp5BD.f2=t2,
_tmp5BD)));void*_tmp1DE;void*_tmp1E0;struct _dyneither_ptr*_tmp1E2;struct
_dyneither_ptr _tmp1E3;void*_tmp1E4;struct _dyneither_ptr*_tmp1E6;struct
_dyneither_ptr _tmp1E7;void*_tmp1E8;struct _dyneither_ptr*_tmp1EA;struct
_dyneither_ptr _tmp1EB;void*_tmp1EC;void*_tmp1EE;void*_tmp1F0;void*_tmp1F2;void*
_tmp1F4;void*_tmp1F6;void*_tmp1F8;void*_tmp1FA;void*_tmp1FC;void*_tmp1FE;void*
_tmp200;void*_tmp202;void*_tmp204;void*_tmp206;void*_tmp208;void*_tmp20A;void*
_tmp20C;void*_tmp20E;void*_tmp210;void*_tmp212;void*_tmp214;void*_tmp216;void*
_tmp218;void*_tmp21A;void*_tmp21C;void*_tmp21E;void*_tmp220;void*_tmp222;void*
_tmp224;void*_tmp226;void*_tmp227;void*_tmp228;void*_tmp229;void*_tmp22A;void*
_tmp22B;void*_tmp22D;void*_tmp22E;void*_tmp22F;void*_tmp230;void*_tmp231;void*
_tmp232;void*_tmp234;void*_tmp235;void*_tmp236;void*_tmp237;void*_tmp239;void*
_tmp23A;void*_tmp23B;void*_tmp23C;void*_tmp23E;void*_tmp23F;void*_tmp240;void*
_tmp241;void*_tmp243;void*_tmp244;void*_tmp245;void*_tmp246;void*_tmp247;struct
Cyc_Port_Cvar*_tmp249;void*_tmp24A;struct Cyc_Port_Cvar*_tmp24C;void*_tmp24D;
struct Cyc_Port_Cvar*_tmp24F;void*_tmp250;struct Cyc_Port_Cvar*_tmp252;_LL104:
_tmp1DE=_tmp1DD.f1;{struct Cyc_Port_Heap_ct_Port_Ctype_struct*_tmp1DF=(struct Cyc_Port_Heap_ct_Port_Ctype_struct*)
_tmp1DE;if(_tmp1DF->tag != 7)goto _LL106;};_LL105:
# 711
 return 1;_LL106: _tmp1E0=_tmp1DD.f1;{struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*
_tmp1E1=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp1E0;if(_tmp1E1->tag != 
10)goto _LL108;else{_tmp1E2=_tmp1E1->f1;_tmp1E3=*_tmp1E2;}};_tmp1E4=_tmp1DD.f2;{
struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmp1E5=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)
_tmp1E4;if(_tmp1E5->tag != 10)goto _LL108;else{_tmp1E6=_tmp1E5->f1;_tmp1E7=*
_tmp1E6;}};_LL107:
# 712
 return Cyc_strcmp((struct _dyneither_ptr)_tmp1E3,(struct _dyneither_ptr)_tmp1E7)== 
0;_LL108: _tmp1E8=_tmp1DD.f1;{struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmp1E9=(
struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp1E8;if(_tmp1E9->tag != 10)goto
_LL10A;else{_tmp1EA=_tmp1E9->f1;_tmp1EB=*_tmp1EA;}};_tmp1EC=_tmp1DD.f2;{struct
Cyc_Port_Heap_ct_Port_Ctype_struct*_tmp1ED=(struct Cyc_Port_Heap_ct_Port_Ctype_struct*)
_tmp1EC;if(_tmp1ED->tag != 7)goto _LL10A;};_LL109:
# 713
 return 0;_LL10A: _tmp1EE=_tmp1DD.f1;{struct Cyc_Port_Notconst_ct_Port_Ctype_struct*
_tmp1EF=(struct Cyc_Port_Notconst_ct_Port_Ctype_struct*)_tmp1EE;if(_tmp1EF->tag != 
1)goto _LL10C;};_tmp1F0=_tmp1DD.f2;{struct Cyc_Port_Const_ct_Port_Ctype_struct*
_tmp1F1=(struct Cyc_Port_Const_ct_Port_Ctype_struct*)_tmp1F0;if(_tmp1F1->tag != 0)
goto _LL10C;};_LL10B:
# 714
 return 1;_LL10C: _tmp1F2=_tmp1DD.f1;{struct Cyc_Port_Const_ct_Port_Ctype_struct*
_tmp1F3=(struct Cyc_Port_Const_ct_Port_Ctype_struct*)_tmp1F2;if(_tmp1F3->tag != 0)
goto _LL10E;};_tmp1F4=_tmp1DD.f2;{struct Cyc_Port_Notconst_ct_Port_Ctype_struct*
_tmp1F5=(struct Cyc_Port_Notconst_ct_Port_Ctype_struct*)_tmp1F4;if(_tmp1F5->tag != 
1)goto _LL10E;};_LL10D:
# 715
 return 0;_LL10E: _tmp1F6=_tmp1DD.f1;{struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*
_tmp1F7=(struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*)_tmp1F6;if(_tmp1F7->tag != 
9)goto _LL110;};_tmp1F8=_tmp1DD.f2;{struct Cyc_Port_Zterm_ct_Port_Ctype_struct*
_tmp1F9=(struct Cyc_Port_Zterm_ct_Port_Ctype_struct*)_tmp1F8;if(_tmp1F9->tag != 8)
goto _LL110;};_LL10F:
# 716
 return 0;_LL110: _tmp1FA=_tmp1DD.f1;{struct Cyc_Port_Zterm_ct_Port_Ctype_struct*
_tmp1FB=(struct Cyc_Port_Zterm_ct_Port_Ctype_struct*)_tmp1FA;if(_tmp1FB->tag != 8)
goto _LL112;};_tmp1FC=_tmp1DD.f2;{struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*
_tmp1FD=(struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*)_tmp1FC;if(_tmp1FD->tag != 
9)goto _LL112;};_LL111:
# 717
 return 1;_LL112: _tmp1FE=_tmp1DD.f1;{struct Cyc_Port_Var_ct_Port_Ctype_struct*
_tmp1FF=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp1FE;if(_tmp1FF->tag != 16)
goto _LL114;};_tmp200=_tmp1DD.f2;{struct Cyc_Port_Const_ct_Port_Ctype_struct*
_tmp201=(struct Cyc_Port_Const_ct_Port_Ctype_struct*)_tmp200;if(_tmp201->tag != 0)
goto _LL114;};_LL113:
# 718
 return 1;_LL114: _tmp202=_tmp1DD.f1;{struct Cyc_Port_Var_ct_Port_Ctype_struct*
_tmp203=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp202;if(_tmp203->tag != 16)
goto _LL116;};_tmp204=_tmp1DD.f2;{struct Cyc_Port_Void_ct_Port_Ctype_struct*
_tmp205=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)_tmp204;if(_tmp205->tag != 4)
goto _LL116;};_LL115:
# 719
 return 1;_LL116: _tmp206=_tmp1DD.f1;{struct Cyc_Port_Void_ct_Port_Ctype_struct*
_tmp207=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)_tmp206;if(_tmp207->tag != 4)
goto _LL118;};_LL117:
# 720
 return 0;_LL118: _tmp208=_tmp1DD.f1;{struct Cyc_Port_Zero_ct_Port_Ctype_struct*
_tmp209=(struct Cyc_Port_Zero_ct_Port_Ctype_struct*)_tmp208;if(_tmp209->tag != 5)
goto _LL11A;};_tmp20A=_tmp1DD.f2;{struct Cyc_Port_Arith_ct_Port_Ctype_struct*
_tmp20B=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmp20A;if(_tmp20B->tag != 6)
goto _LL11A;};_LL119:
# 721
 return 1;_LL11A: _tmp20C=_tmp1DD.f1;{struct Cyc_Port_Zero_ct_Port_Ctype_struct*
_tmp20D=(struct Cyc_Port_Zero_ct_Port_Ctype_struct*)_tmp20C;if(_tmp20D->tag != 5)
goto _LL11C;};_tmp20E=_tmp1DD.f2;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp20F=(
struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp20E;if(_tmp20F->tag != 11)goto _LL11C;};
_LL11B:
# 722
 return 1;_LL11C: _tmp210=_tmp1DD.f1;{struct Cyc_Port_Zero_ct_Port_Ctype_struct*
_tmp211=(struct Cyc_Port_Zero_ct_Port_Ctype_struct*)_tmp210;if(_tmp211->tag != 5)
goto _LL11E;};_tmp212=_tmp1DD.f2;{struct Cyc_Port_Void_ct_Port_Ctype_struct*
_tmp213=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)_tmp212;if(_tmp213->tag != 4)
goto _LL11E;};_LL11D:
# 723
 return 1;_LL11E: _tmp214=_tmp1DD.f1;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*
_tmp215=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp214;if(_tmp215->tag != 11)
goto _LL120;};_tmp216=_tmp1DD.f2;{struct Cyc_Port_Arith_ct_Port_Ctype_struct*
_tmp217=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmp216;if(_tmp217->tag != 6)
goto _LL120;};_LL11F:
# 724
 return 1;_LL120: _tmp218=_tmp1DD.f1;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*
_tmp219=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp218;if(_tmp219->tag != 11)
goto _LL122;};_tmp21A=_tmp1DD.f2;{struct Cyc_Port_Void_ct_Port_Ctype_struct*
_tmp21B=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)_tmp21A;if(_tmp21B->tag != 4)
goto _LL122;};_LL121:
# 725
 return 1;_LL122: _tmp21C=_tmp1DD.f1;{struct Cyc_Port_Array_ct_Port_Ctype_struct*
_tmp21D=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp21C;if(_tmp21D->tag != 12)
goto _LL124;};_tmp21E=_tmp1DD.f2;{struct Cyc_Port_Arith_ct_Port_Ctype_struct*
_tmp21F=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmp21E;if(_tmp21F->tag != 6)
goto _LL124;};_LL123:
# 726
 return 1;_LL124: _tmp220=_tmp1DD.f1;{struct Cyc_Port_Array_ct_Port_Ctype_struct*
_tmp221=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp220;if(_tmp221->tag != 12)
goto _LL126;};_tmp222=_tmp1DD.f2;{struct Cyc_Port_Void_ct_Port_Ctype_struct*
_tmp223=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)_tmp222;if(_tmp223->tag != 4)
goto _LL126;};_LL125:
# 727
 return 1;_LL126: _tmp224=_tmp1DD.f1;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*
_tmp225=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp224;if(_tmp225->tag != 11)
goto _LL128;else{_tmp226=(void*)_tmp225->f1;_tmp227=(void*)_tmp225->f2;_tmp228=(
void*)_tmp225->f3;_tmp229=(void*)_tmp225->f4;_tmp22A=(void*)_tmp225->f5;}};
_tmp22B=_tmp1DD.f2;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp22C=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)
_tmp22B;if(_tmp22C->tag != 11)goto _LL128;else{_tmp22D=(void*)_tmp22C->f1;_tmp22E=(
void*)_tmp22C->f2;_tmp22F=(void*)_tmp22C->f3;_tmp230=(void*)_tmp22C->f4;_tmp231=(
void*)_tmp22C->f5;}};_LL127:
# 729
 return(((Cyc_Port_leq(_tmp226,_tmp22D) && Cyc_Port_leq(_tmp227,_tmp22E)) && Cyc_Port_unifies(
_tmp228,_tmp22F)) && Cyc_Port_leq(_tmp229,_tmp230)) && 
# 730
Cyc_Port_leq(_tmp22A,_tmp231);_LL128: _tmp232=_tmp1DD.f1;{struct Cyc_Port_Array_ct_Port_Ctype_struct*
_tmp233=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp232;if(_tmp233->tag != 12)
goto _LL12A;else{_tmp234=(void*)_tmp233->f1;_tmp235=(void*)_tmp233->f2;_tmp236=(
void*)_tmp233->f3;}};_tmp237=_tmp1DD.f2;{struct Cyc_Port_Array_ct_Port_Ctype_struct*
_tmp238=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp237;if(_tmp238->tag != 12)
goto _LL12A;else{_tmp239=(void*)_tmp238->f1;_tmp23A=(void*)_tmp238->f2;_tmp23B=(
void*)_tmp238->f3;}};_LL129:
# 732
 return(Cyc_Port_leq(_tmp234,_tmp239) && Cyc_Port_leq(_tmp235,_tmp23A)) && Cyc_Port_leq(
_tmp236,_tmp23B);_LL12A: _tmp23C=_tmp1DD.f1;{struct Cyc_Port_Array_ct_Port_Ctype_struct*
_tmp23D=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp23C;if(_tmp23D->tag != 12)
goto _LL12C;else{_tmp23E=(void*)_tmp23D->f1;_tmp23F=(void*)_tmp23D->f2;_tmp240=(
void*)_tmp23D->f3;}};_tmp241=_tmp1DD.f2;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*
_tmp242=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp241;if(_tmp242->tag != 11)
goto _LL12C;else{_tmp243=(void*)_tmp242->f1;_tmp244=(void*)_tmp242->f2;_tmp245=(
void*)_tmp242->f3;_tmp246=(void*)_tmp242->f5;}};_LL12B:
# 734
 return((Cyc_Port_leq(_tmp23E,_tmp243) && Cyc_Port_leq(_tmp23F,_tmp244)) && Cyc_Port_unifies((
void*)& Cyc_Port_Fat_ct_val,_tmp245)) && 
# 735
Cyc_Port_leq(_tmp240,_tmp246);_LL12C: _tmp247=_tmp1DD.f1;{struct Cyc_Port_Var_ct_Port_Ctype_struct*
_tmp248=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp247;if(_tmp248->tag != 16)
goto _LL12E;else{_tmp249=_tmp248->f1;}};_tmp24A=_tmp1DD.f2;{struct Cyc_Port_Var_ct_Port_Ctype_struct*
_tmp24B=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp24A;if(_tmp24B->tag != 16)
goto _LL12E;else{_tmp24C=_tmp24B->f1;}};_LL12D:
# 737
 _tmp249->uppers=Cyc_Port_filter_self(t1,_tmp249->uppers);
# 738
_tmp24C->lowers=Cyc_Port_filter_self(t2,_tmp24C->lowers);
# 739
_tmp249->uppers=Cyc_Port_insert_upper(t1,t2,& _tmp249->uppers);
# 740
_tmp24C->lowers=Cyc_Port_insert_lower(t2,t1,& _tmp24C->lowers);
# 741
return 1;_LL12E: _tmp24D=_tmp1DD.f1;{struct Cyc_Port_Var_ct_Port_Ctype_struct*
_tmp24E=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp24D;if(_tmp24E->tag != 16)
goto _LL130;else{_tmp24F=_tmp24E->f1;}};_LL12F:
# 743
 _tmp24F->uppers=Cyc_Port_filter_self(t1,_tmp24F->uppers);
# 744
_tmp24F->uppers=Cyc_Port_insert_upper(t1,t2,& _tmp24F->uppers);
# 745
return 1;_LL130: _tmp250=_tmp1DD.f2;{struct Cyc_Port_Var_ct_Port_Ctype_struct*
_tmp251=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp250;if(_tmp251->tag != 16)
goto _LL132;else{_tmp252=_tmp251->f1;}};_LL131:
# 747
 _tmp252->lowers=Cyc_Port_filter_self(t2,_tmp252->lowers);
# 748
_tmp252->lowers=Cyc_Port_insert_lower(t2,t1,& _tmp252->lowers);
# 749
return 1;_LL132:;_LL133:
# 750
 return Cyc_Port_unifies(t1,t2);_LL103:;};}struct Cyc_Port_GlobalCenv{struct Cyc_Dict_Dict
typedef_dict;struct Cyc_Dict_Dict struct_dict;struct Cyc_Dict_Dict union_dict;void*
return_type;struct Cyc_List_List*qualifier_edits;struct Cyc_List_List*
pointer_edits;struct Cyc_List_List*zeroterm_edits;struct Cyc_List_List*edits;int
porting;};
# 756
typedef struct Cyc_Port_GlobalCenv*Cyc_Port_gcenv_t;
# 779
enum Cyc_Port_CPos{Cyc_Port_FnRes_pos  = 0,Cyc_Port_FnArg_pos  = 1,Cyc_Port_FnBody_pos
 = 2,Cyc_Port_Toplevel_pos  = 3};
# 780
typedef enum Cyc_Port_CPos Cyc_Port_cpos_t;struct Cyc_Port_GlobalCenv;struct Cyc_Port_Cenv{
struct Cyc_Port_GlobalCenv*gcenv;struct Cyc_Dict_Dict var_dict;enum Cyc_Port_CPos
cpos;};
# 782
typedef struct Cyc_Port_Cenv*Cyc_Port_cenv_t;struct Cyc_Port_Cenv;struct Cyc_Port_GlobalCenv;
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_GlobalCenv;struct Cyc_Port_GlobalCenv;
# 792
static struct Cyc_Port_Cenv*Cyc_Port_empty_cenv(){
# 793
struct Cyc_Port_GlobalCenv*_tmp5BE;struct Cyc_Port_GlobalCenv*g=(_tmp5BE=
_cycalloc(sizeof(*_tmp5BE)),((_tmp5BE->typedef_dict=((struct Cyc_Dict_Dict(*)(int(*
cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),((
_tmp5BE->struct_dict=
# 794
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp),((_tmp5BE->union_dict=
# 795
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp),((_tmp5BE->qualifier_edits=0,((_tmp5BE->pointer_edits=0,((
_tmp5BE->zeroterm_edits=0,((_tmp5BE->edits=0,((_tmp5BE->porting=0,((_tmp5BE->return_type=
# 801
Cyc_Port_void_ct(),_tmp5BE)))))))))))))))))));
# 802
struct Cyc_Port_Cenv*_tmp5BF;return(_tmp5BF=_cycalloc(sizeof(*_tmp5BF)),((
_tmp5BF->gcenv=g,((_tmp5BF->cpos=Cyc_Port_Toplevel_pos,((_tmp5BF->var_dict=
# 804
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp),_tmp5BF)))))));}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 810
static int Cyc_Port_in_fn_arg(struct Cyc_Port_Cenv*env){
# 811
return env->cpos == Cyc_Port_FnArg_pos;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 813
static int Cyc_Port_in_fn_res(struct Cyc_Port_Cenv*env){
# 814
return env->cpos == Cyc_Port_FnRes_pos;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 816
static int Cyc_Port_in_toplevel(struct Cyc_Port_Cenv*env){
# 817
return env->cpos == Cyc_Port_Toplevel_pos;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 819
static void*Cyc_Port_lookup_return_type(struct Cyc_Port_Cenv*env){
# 820
return(env->gcenv)->return_type;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 822
static void*Cyc_Port_lookup_typedef(struct Cyc_Port_Cenv*env,struct _tuple0*n){
# 823
struct _handler_cons _tmp255;_push_handler(& _tmp255);{int _tmp257=0;if(setjmp(
_tmp255.handler))_tmp257=1;if(!_tmp257){
# 824
{void*_tmp259;struct _tuple10 _tmp258=*((struct _tuple10*(*)(struct Cyc_Dict_Dict d,
struct _tuple0*k))Cyc_Dict_lookup)((env->gcenv)->typedef_dict,n);_tmp259=_tmp258.f1;{
# 825
void*_tmp25A=_tmp259;_npop_handler(0);return _tmp25A;};}
# 824
;_pop_handler();}else{void*_tmp256=(void*)_exn_thrown;void*_tmp25C=_tmp256;
_LL135: {struct Cyc_Dict_Absent_exn_struct*_tmp25D=(struct Cyc_Dict_Absent_exn_struct*)
_tmp25C;if(_tmp25D->tag != Cyc_Dict_Absent)goto _LL137;}_LL136:
# 831
 return Cyc_Absyn_sint_typ;_LL137:;_LL138:(void)_throw(_tmp25C);_LL134:;}};}
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 836
static void*Cyc_Port_lookup_typedef_ctype(struct Cyc_Port_Cenv*env,struct _tuple0*
n){
# 837
struct _handler_cons _tmp25E;_push_handler(& _tmp25E);{int _tmp260=0;if(setjmp(
_tmp25E.handler))_tmp260=1;if(!_tmp260){
# 838
{void*_tmp262;struct _tuple10 _tmp261=*((struct _tuple10*(*)(struct Cyc_Dict_Dict d,
struct _tuple0*k))Cyc_Dict_lookup)((env->gcenv)->typedef_dict,n);_tmp262=_tmp261.f2;{
# 839
void*_tmp263=_tmp262;_npop_handler(0);return _tmp263;};}
# 838
;_pop_handler();}else{void*_tmp25F=(void*)_exn_thrown;void*_tmp265=_tmp25F;
_LL13A: {struct Cyc_Dict_Absent_exn_struct*_tmp266=(struct Cyc_Dict_Absent_exn_struct*)
_tmp265;if(_tmp266->tag != Cyc_Dict_Absent)goto _LL13C;}_LL13B:
# 845
 return Cyc_Port_var();_LL13C:;_LL13D:(void)_throw(_tmp265);_LL139:;}};}struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;
# 851
static struct _tuple11*Cyc_Port_lookup_struct_decl(struct Cyc_Port_Cenv*env,struct
_tuple0*n){
# 853
struct _tuple11**_tmp267=((struct _tuple11**(*)(struct Cyc_Dict_Dict d,struct
_tuple0*k))Cyc_Dict_lookup_opt)((env->gcenv)->struct_dict,n);
# 854
if((unsigned int)_tmp267)
# 855
return*_tmp267;else{
# 857
struct Cyc_Absyn_Aggrdecl*_tmp5C0;struct Cyc_Absyn_Aggrdecl*_tmp268=(_tmp5C0=
_cycalloc(sizeof(*_tmp5C0)),((_tmp5C0->kind=Cyc_Absyn_StructA,((_tmp5C0->sc=Cyc_Absyn_Public,((
_tmp5C0->name=n,((_tmp5C0->tvs=0,((_tmp5C0->impl=0,((_tmp5C0->attributes=0,
_tmp5C0)))))))))))));
# 859
struct _tuple11*_tmp5C1;struct _tuple11*p=(_tmp5C1=_cycalloc(sizeof(*_tmp5C1)),((
_tmp5C1->f1=_tmp268,((_tmp5C1->f2=0,_tmp5C1)))));
# 860
(env->gcenv)->struct_dict=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_tuple0*k,struct _tuple11*v))Cyc_Dict_insert)((env->gcenv)->struct_dict,n,p);
# 861
return p;}}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 867
static struct _tuple11*Cyc_Port_lookup_union_decl(struct Cyc_Port_Cenv*env,struct
_tuple0*n){
# 869
struct _tuple11**_tmp26B=((struct _tuple11**(*)(struct Cyc_Dict_Dict d,struct
_tuple0*k))Cyc_Dict_lookup_opt)((env->gcenv)->union_dict,n);
# 870
if((unsigned int)_tmp26B)
# 871
return*_tmp26B;else{
# 873
struct Cyc_Absyn_Aggrdecl*_tmp5C2;struct Cyc_Absyn_Aggrdecl*_tmp26C=(_tmp5C2=
_cycalloc(sizeof(*_tmp5C2)),((_tmp5C2->kind=Cyc_Absyn_UnionA,((_tmp5C2->sc=Cyc_Absyn_Public,((
_tmp5C2->name=n,((_tmp5C2->tvs=0,((_tmp5C2->impl=0,((_tmp5C2->attributes=0,
_tmp5C2)))))))))))));
# 875
struct _tuple11*_tmp5C3;struct _tuple11*p=(_tmp5C3=_cycalloc(sizeof(*_tmp5C3)),((
_tmp5C3->f1=_tmp26C,((_tmp5C3->f2=0,_tmp5C3)))));
# 876
(env->gcenv)->union_dict=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_tuple0*k,struct _tuple11*v))Cyc_Dict_insert)((env->gcenv)->union_dict,n,p);
# 877
return p;}}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct _tuple13{void*f1;struct
_tuple10*f2;};
# 882
static struct _tuple10 Cyc_Port_lookup_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
# 883
struct _handler_cons _tmp26F;_push_handler(& _tmp26F);{int _tmp271=0;if(setjmp(
_tmp26F.handler))_tmp271=1;if(!_tmp271){
# 884
{struct _tuple10*_tmp273;struct _tuple13 _tmp272=*((struct _tuple13*(*)(struct Cyc_Dict_Dict
d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);_tmp273=_tmp272.f2;{
# 885
struct _tuple10 _tmp274=*_tmp273;_npop_handler(0);return _tmp274;};}
# 884
;_pop_handler();}else{void*_tmp270=(void*)_exn_thrown;void*_tmp276=_tmp270;
_LL13F: {struct Cyc_Dict_Absent_exn_struct*_tmp277=(struct Cyc_Dict_Absent_exn_struct*)
_tmp276;if(_tmp277->tag != Cyc_Dict_Absent)goto _LL141;}_LL140: {
# 891
struct _tuple10 _tmp5C4;return(_tmp5C4.f1=Cyc_Port_var(),((_tmp5C4.f2=Cyc_Port_var(),
_tmp5C4)));}_LL141:;_LL142:(void)_throw(_tmp276);_LL13E:;}};}struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;
# 894
static struct _tuple13*Cyc_Port_lookup_full_var(struct Cyc_Port_Cenv*env,struct
_tuple0*x){
# 895
return((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(
env->var_dict,x);}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 898
static int Cyc_Port_declared_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
# 899
return((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_member)(env->var_dict,
x);}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 902
static int Cyc_Port_isfn(struct Cyc_Port_Cenv*env,struct _tuple0*x){
# 903
struct _handler_cons _tmp279;_push_handler(& _tmp279);{int _tmp27B=0;if(setjmp(
_tmp279.handler))_tmp27B=1;if(!_tmp27B){
# 904
{void*_tmp27D;struct _tuple13 _tmp27C=*((struct _tuple13*(*)(struct Cyc_Dict_Dict d,
struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);_tmp27D=_tmp27C.f1;
# 905
LOOP: {void*_tmp27E=_tmp27D;struct _tuple0*_tmp280;_LL144: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*
_tmp27F=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp27E;if(_tmp27F->tag
!= 17)goto _LL146;else{_tmp280=_tmp27F->f1;}}_LL145:
# 906
 _tmp27D=Cyc_Port_lookup_typedef(env,_tmp280);goto LOOP;_LL146: {struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp281=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp27E;if(_tmp281->tag != 9)
goto _LL148;}_LL147: {
# 907
int _tmp282=1;_npop_handler(0);return _tmp282;}_LL148:;_LL149: {
# 908
int _tmp283=0;_npop_handler(0);return _tmp283;}_LL143:;}}
# 904
;_pop_handler();}else{void*_tmp27A=(void*)_exn_thrown;void*_tmp285=_tmp27A;
_LL14B: {struct Cyc_Dict_Absent_exn_struct*_tmp286=(struct Cyc_Dict_Absent_exn_struct*)
_tmp285;if(_tmp286->tag != Cyc_Dict_Absent)goto _LL14D;}_LL14C:
# 915
 return 0;_LL14D:;_LL14E:(void)_throw(_tmp285);_LL14A:;}};}struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;
# 919
static int Cyc_Port_isarray(struct Cyc_Port_Cenv*env,struct _tuple0*x){
# 920
struct _handler_cons _tmp287;_push_handler(& _tmp287);{int _tmp289=0;if(setjmp(
_tmp287.handler))_tmp289=1;if(!_tmp289){
# 921
{void*_tmp28B;struct _tuple13 _tmp28A=*((struct _tuple13*(*)(struct Cyc_Dict_Dict d,
struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);_tmp28B=_tmp28A.f1;
# 922
LOOP: {void*_tmp28C=_tmp28B;struct _tuple0*_tmp28E;_LL150: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*
_tmp28D=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp28C;if(_tmp28D->tag
!= 17)goto _LL152;else{_tmp28E=_tmp28D->f1;}}_LL151:
# 923
 _tmp28B=Cyc_Port_lookup_typedef(env,_tmp28E);goto LOOP;_LL152: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp28F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp28C;if(_tmp28F->tag != 
8)goto _LL154;}_LL153: {
# 924
int _tmp290=1;_npop_handler(0);return _tmp290;}_LL154:;_LL155: {
# 925
int _tmp291=0;_npop_handler(0);return _tmp291;}_LL14F:;}}
# 921
;_pop_handler();}else{void*_tmp288=(void*)_exn_thrown;void*_tmp293=_tmp288;
_LL157: {struct Cyc_Dict_Absent_exn_struct*_tmp294=(struct Cyc_Dict_Absent_exn_struct*)
_tmp293;if(_tmp294->tag != Cyc_Dict_Absent)goto _LL159;}_LL158:
# 932
 return 0;_LL159:;_LL15A:(void)_throw(_tmp293);_LL156:;}};}struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 938
static struct Cyc_Port_Cenv*Cyc_Port_set_cpos(struct Cyc_Port_Cenv*env,enum Cyc_Port_CPos
cpos){
# 939
struct Cyc_Port_Cenv*_tmp5C5;return(_tmp5C5=_cycalloc(sizeof(*_tmp5C5)),((
_tmp5C5->gcenv=env->gcenv,((_tmp5C5->var_dict=env->var_dict,((_tmp5C5->cpos=cpos,
_tmp5C5)))))));}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 943
static void Cyc_Port_add_return_type(struct Cyc_Port_Cenv*env,void*t){
# 944
(env->gcenv)->return_type=t;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 948
static struct Cyc_Port_Cenv*Cyc_Port_add_var(struct Cyc_Port_Cenv*env,struct
_tuple0*x,void*t,void*qual,void*ctype){
# 950
struct _tuple13*_tmp5CB;struct _tuple10*_tmp5CA;struct Cyc_Port_Cenv*_tmp5C9;
return(_tmp5C9=_cycalloc(sizeof(*_tmp5C9)),((_tmp5C9->gcenv=env->gcenv,((_tmp5C9->var_dict=
# 951
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple13*v))
Cyc_Dict_insert)(env->var_dict,x,((_tmp5CB=_cycalloc(sizeof(*_tmp5CB)),((_tmp5CB->f1=
t,((_tmp5CB->f2=((_tmp5CA=_cycalloc(sizeof(*_tmp5CA)),((_tmp5CA->f1=qual,((
_tmp5CA->f2=ctype,_tmp5CA)))))),_tmp5CB))))))),((_tmp5C9->cpos=env->cpos,_tmp5C9)))))));}
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 956
static void Cyc_Port_add_typedef(struct Cyc_Port_Cenv*env,struct _tuple0*n,void*t,
void*ct){
# 957
struct _tuple10*_tmp5CC;(env->gcenv)->typedef_dict=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple10*v))Cyc_Dict_insert)((env->gcenv)->typedef_dict,
n,(
# 958
(_tmp5CC=_cycalloc(sizeof(*_tmp5CC)),((_tmp5CC->f1=t,((_tmp5CC->f2=ct,_tmp5CC)))))));}
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct _tuple14{void*f1;void*f2;
unsigned int f3;};
# 963
static void Cyc_Port_register_const_cvar(struct Cyc_Port_Cenv*env,void*new_qual,
void*orig_qual,unsigned int loc){
# 965
struct _tuple14*_tmp5CF;struct Cyc_List_List*_tmp5CE;(env->gcenv)->qualifier_edits=((
_tmp5CE=_cycalloc(sizeof(*_tmp5CE)),((_tmp5CE->hd=((_tmp5CF=_cycalloc(sizeof(*
_tmp5CF)),((_tmp5CF->f1=new_qual,((_tmp5CF->f2=orig_qual,((_tmp5CF->f3=loc,
_tmp5CF)))))))),((_tmp5CE->tl=(env->gcenv)->qualifier_edits,_tmp5CE))))));}
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 971
static void Cyc_Port_register_ptr_cvars(struct Cyc_Port_Cenv*env,void*new_ptr,void*
orig_ptr,unsigned int loc){
# 973
struct _tuple14*_tmp5D2;struct Cyc_List_List*_tmp5D1;(env->gcenv)->pointer_edits=((
_tmp5D1=_cycalloc(sizeof(*_tmp5D1)),((_tmp5D1->hd=((_tmp5D2=_cycalloc(sizeof(*
_tmp5D2)),((_tmp5D2->f1=new_ptr,((_tmp5D2->f2=orig_ptr,((_tmp5D2->f3=loc,_tmp5D2)))))))),((
_tmp5D1->tl=(env->gcenv)->pointer_edits,_tmp5D1))))));}struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;
# 977
static void Cyc_Port_register_zeroterm_cvars(struct Cyc_Port_Cenv*env,void*new_zt,
void*orig_zt,unsigned int loc){
# 979
struct _tuple14*_tmp5D5;struct Cyc_List_List*_tmp5D4;(env->gcenv)->zeroterm_edits=((
_tmp5D4=_cycalloc(sizeof(*_tmp5D4)),((_tmp5D4->hd=((_tmp5D5=_cycalloc(sizeof(*
_tmp5D5)),((_tmp5D5->f1=new_zt,((_tmp5D5->f2=orig_zt,((_tmp5D5->f3=loc,_tmp5D5)))))))),((
_tmp5D4->tl=(env->gcenv)->zeroterm_edits,_tmp5D4))))));}
# 985
static void*Cyc_Port_main_type(){
# 986
struct _tuple8*_tmp5D6;struct _tuple8*arg1=
# 987
(_tmp5D6=_cycalloc(sizeof(*_tmp5D6)),((_tmp5D6->f1=0,((_tmp5D6->f2=Cyc_Absyn_empty_tqual(
0),((_tmp5D6->f3=Cyc_Absyn_sint_typ,_tmp5D6)))))));
# 988
struct _tuple8*_tmp5D7;struct _tuple8*arg2=
# 989
(_tmp5D7=_cycalloc(sizeof(*_tmp5D7)),((_tmp5D7->f1=0,((_tmp5D7->f2=Cyc_Absyn_empty_tqual(
0),((_tmp5D7->f3=
# 990
Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_wildtyp(0)),Cyc_Absyn_wildtyp(
0),
# 991
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()),
_tmp5D7)))))));
# 992
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp5E1;struct _tuple8*_tmp5E0[2];struct
Cyc_Absyn_FnInfo _tmp5DF;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5DE;return(
void*)((_tmp5DE=_cycalloc(sizeof(*_tmp5DE)),((_tmp5DE[0]=((_tmp5E1.tag=9,((
_tmp5E1.f1=((_tmp5DF.tvars=0,((_tmp5DF.effect=0,((_tmp5DF.ret_tqual=
# 993
Cyc_Absyn_empty_tqual(0),((_tmp5DF.ret_typ=Cyc_Absyn_sint_typ,((_tmp5DF.args=(
# 995
(_tmp5E0[1]=arg2,((_tmp5E0[0]=arg1,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5E0,sizeof(struct _tuple8*),2)))))),((
_tmp5DF.c_varargs=0,((_tmp5DF.cyc_varargs=0,((_tmp5DF.rgn_po=0,((_tmp5DF.attributes=
0,_tmp5DF)))))))))))))))))),_tmp5E1)))),_tmp5DE))));}struct Cyc_Port_Cenv;
# 1001
static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t);struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1004
static struct Cyc_Port_Cenv*Cyc_Port_initial_cenv(){
# 1005
struct Cyc_Port_Cenv*_tmp2A6=Cyc_Port_empty_cenv();
# 1007
{struct _tuple0*_tmp5E2;_tmp2A6=Cyc_Port_add_var(_tmp2A6,((_tmp5E2=_cycalloc(
sizeof(*_tmp5E2)),((_tmp5E2->f1=Cyc_Absyn_Loc_n,((_tmp5E2->f2=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"main",sizeof(char),
5),_tmp5E2)))))),Cyc_Port_main_type(),Cyc_Port_const_ct(),
# 1008
Cyc_Port_type_to_ctype(_tmp2A6,Cyc_Port_main_type()));}
# 1009
return _tmp2A6;}
# 1015
static void Cyc_Port_region_counts(struct Cyc_Dict_Dict*cts,void*t){
# 1016
void*_tmp2A9=Cyc_Port_compress_ct(t);struct _dyneither_ptr*_tmp2B8;void*_tmp2BA;
void*_tmp2BB;void*_tmp2BC;void*_tmp2BD;void*_tmp2BE;void*_tmp2C0;void*_tmp2C1;
void*_tmp2C2;void*_tmp2C4;struct Cyc_List_List*_tmp2C5;_LL15C: {struct Cyc_Port_Const_ct_Port_Ctype_struct*
_tmp2AA=(struct Cyc_Port_Const_ct_Port_Ctype_struct*)_tmp2A9;if(_tmp2AA->tag != 0)
goto _LL15E;}_LL15D:
# 1017
 goto _LL15F;_LL15E: {struct Cyc_Port_Notconst_ct_Port_Ctype_struct*_tmp2AB=(
struct Cyc_Port_Notconst_ct_Port_Ctype_struct*)_tmp2A9;if(_tmp2AB->tag != 1)goto
_LL160;}_LL15F:
# 1018
 goto _LL161;_LL160: {struct Cyc_Port_Thin_ct_Port_Ctype_struct*_tmp2AC=(struct Cyc_Port_Thin_ct_Port_Ctype_struct*)
_tmp2A9;if(_tmp2AC->tag != 2)goto _LL162;}_LL161:
# 1019
 goto _LL163;_LL162: {struct Cyc_Port_Fat_ct_Port_Ctype_struct*_tmp2AD=(struct Cyc_Port_Fat_ct_Port_Ctype_struct*)
_tmp2A9;if(_tmp2AD->tag != 3)goto _LL164;}_LL163:
# 1020
 goto _LL165;_LL164: {struct Cyc_Port_Void_ct_Port_Ctype_struct*_tmp2AE=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)
_tmp2A9;if(_tmp2AE->tag != 4)goto _LL166;}_LL165:
# 1021
 goto _LL167;_LL166: {struct Cyc_Port_Zero_ct_Port_Ctype_struct*_tmp2AF=(struct Cyc_Port_Zero_ct_Port_Ctype_struct*)
_tmp2A9;if(_tmp2AF->tag != 5)goto _LL168;}_LL167:
# 1022
 goto _LL169;_LL168: {struct Cyc_Port_Arith_ct_Port_Ctype_struct*_tmp2B0=(struct
Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmp2A9;if(_tmp2B0->tag != 6)goto _LL16A;}
_LL169:
# 1023
 goto _LL16B;_LL16A: {struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmp2B1=(
struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp2A9;if(_tmp2B1->tag != 14)
goto _LL16C;}_LL16B:
# 1024
 goto _LL16D;_LL16C: {struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmp2B2=(
struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp2A9;if(_tmp2B2->tag != 13)goto
_LL16E;}_LL16D:
# 1025
 goto _LL16F;_LL16E: {struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmp2B3=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)
_tmp2A9;if(_tmp2B3->tag != 16)goto _LL170;}_LL16F:
# 1026
 goto _LL171;_LL170: {struct Cyc_Port_Zterm_ct_Port_Ctype_struct*_tmp2B4=(struct
Cyc_Port_Zterm_ct_Port_Ctype_struct*)_tmp2A9;if(_tmp2B4->tag != 8)goto _LL172;}
_LL171:
# 1027
 goto _LL173;_LL172: {struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*_tmp2B5=(struct
Cyc_Port_Nozterm_ct_Port_Ctype_struct*)_tmp2A9;if(_tmp2B5->tag != 9)goto _LL174;}
_LL173:
# 1028
 goto _LL175;_LL174: {struct Cyc_Port_Heap_ct_Port_Ctype_struct*_tmp2B6=(struct Cyc_Port_Heap_ct_Port_Ctype_struct*)
_tmp2A9;if(_tmp2B6->tag != 7)goto _LL176;}_LL175:
# 1029
 return;_LL176: {struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmp2B7=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)
_tmp2A9;if(_tmp2B7->tag != 10)goto _LL178;else{_tmp2B8=_tmp2B7->f1;}}_LL177:
# 1031
 if(!((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*cts,
_tmp2B8)){
# 1032
int*_tmp5E3;*cts=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,int*v))Cyc_Dict_insert)(*cts,_tmp2B8,(int*)((_tmp5E3=
_cycalloc_atomic(sizeof(*_tmp5E3)),((_tmp5E3[0]=0,_tmp5E3)))));}{
# 1033
int*_tmp2C7=((int*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*
cts,_tmp2B8);
# 1034
*_tmp2C7=*_tmp2C7 + 1;
# 1035
return;};_LL178: {struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp2B9=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)
_tmp2A9;if(_tmp2B9->tag != 11)goto _LL17A;else{_tmp2BA=(void*)_tmp2B9->f1;_tmp2BB=(
void*)_tmp2B9->f2;_tmp2BC=(void*)_tmp2B9->f3;_tmp2BD=(void*)_tmp2B9->f4;_tmp2BE=(
void*)_tmp2B9->f5;}}_LL179:
# 1037
 Cyc_Port_region_counts(cts,_tmp2BA);
# 1038
Cyc_Port_region_counts(cts,_tmp2BD);
# 1039
return;_LL17A: {struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp2BF=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)
_tmp2A9;if(_tmp2BF->tag != 12)goto _LL17C;else{_tmp2C0=(void*)_tmp2BF->f1;_tmp2C1=(
void*)_tmp2BF->f2;_tmp2C2=(void*)_tmp2BF->f3;}}_LL17B:
# 1041
 Cyc_Port_region_counts(cts,_tmp2C0);
# 1042
return;_LL17C: {struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp2C3=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)
_tmp2A9;if(_tmp2C3->tag != 15)goto _LL15B;else{_tmp2C4=(void*)_tmp2C3->f1;_tmp2C5=
_tmp2C3->f2;}}_LL17D:
# 1044
 Cyc_Port_region_counts(cts,_tmp2C4);
# 1045
for(0;(unsigned int)_tmp2C5;_tmp2C5=_tmp2C5->tl){Cyc_Port_region_counts(cts,(
void*)_tmp2C5->hd);}
# 1046
return;_LL15B:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Edit;
struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;
# 1055
static void Cyc_Port_register_rgns(struct Cyc_Port_Cenv*env,struct Cyc_Dict_Dict
counts,int fn_res,void*t,void*c){
# 1057
c=Cyc_Port_compress_ct(c);{
# 1058
struct _tuple10 _tmp5E4;struct _tuple10 _tmp2C9=(_tmp5E4.f1=t,((_tmp5E4.f2=c,
_tmp5E4)));void*_tmp2CA;struct Cyc_Absyn_PtrInfo _tmp2CC;void*_tmp2CD;struct Cyc_Absyn_PtrAtts
_tmp2CE;void*_tmp2CF;struct Cyc_Absyn_PtrLoc*_tmp2D0;void*_tmp2D1;void*_tmp2D3;
void*_tmp2D4;void*_tmp2D5;struct Cyc_Absyn_ArrayInfo _tmp2D7;void*_tmp2D8;void*
_tmp2D9;void*_tmp2DB;void*_tmp2DC;struct Cyc_Absyn_FnInfo _tmp2DE;void*_tmp2DF;
struct Cyc_List_List*_tmp2E0;void*_tmp2E1;void*_tmp2E3;struct Cyc_List_List*
_tmp2E4;_LL17F: _tmp2CA=_tmp2C9.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp2CB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2CA;if(_tmp2CB->tag
!= 5)goto _LL181;else{_tmp2CC=_tmp2CB->f1;_tmp2CD=_tmp2CC.elt_typ;_tmp2CE=_tmp2CC.ptr_atts;
_tmp2CF=_tmp2CE.rgn;_tmp2D0=_tmp2CE.ptrloc;}};_tmp2D1=_tmp2C9.f2;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*
_tmp2D2=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp2D1;if(_tmp2D2->tag != 11)
goto _LL181;else{_tmp2D3=(void*)_tmp2D2->f1;_tmp2D4=(void*)_tmp2D2->f4;}};_LL180:
# 1060
 Cyc_Port_register_rgns(env,counts,fn_res,_tmp2CD,_tmp2D3);{
# 1061
unsigned int _tmp2E5=(unsigned int)_tmp2D0?_tmp2D0->rgn_loc:(unsigned int)0;
# 1062
_tmp2D4=Cyc_Port_compress_ct(_tmp2D4);
# 1063
{struct _tuple10 _tmp5E5;struct _tuple10 _tmp2E7=(_tmp5E5.f1=_tmp2CF,((_tmp5E5.f2=
_tmp2D4,_tmp5E5)));void*_tmp2E8;void*_tmp2EA;void*_tmp2EC;void*_tmp2EE;struct
_dyneither_ptr*_tmp2F0;_LL188: _tmp2E8=_tmp2E7.f1;{struct Cyc_Absyn_Evar_Absyn_Type_struct*
_tmp2E9=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2E8;if(_tmp2E9->tag != 1)
goto _LL18A;};_tmp2EA=_tmp2E7.f2;{struct Cyc_Port_Heap_ct_Port_Ctype_struct*
_tmp2EB=(struct Cyc_Port_Heap_ct_Port_Ctype_struct*)_tmp2EA;if(_tmp2EB->tag != 7)
goto _LL18A;};if(!(!fn_res))goto _LL18A;_LL189:
# 1065
{struct Cyc_Port_Edit*_tmp5EE;const char*_tmp5ED;const char*_tmp5EC;struct Cyc_List_List*
_tmp5EB;(env->gcenv)->edits=(
# 1066
(_tmp5EB=_cycalloc(sizeof(*_tmp5EB)),((_tmp5EB->hd=((_tmp5EE=_cycalloc(sizeof(*
_tmp5EE)),((_tmp5EE->loc=_tmp2E5,((_tmp5EE->old_string=((_tmp5ED="`H ",
_tag_dyneither(_tmp5ED,sizeof(char),4))),((_tmp5EE->new_string=((_tmp5EC="",
_tag_dyneither(_tmp5EC,sizeof(char),1))),_tmp5EE)))))))),((_tmp5EB->tl=(env->gcenv)->edits,
_tmp5EB))))));}
# 1067
goto _LL187;_LL18A: _tmp2EC=_tmp2E7.f1;{struct Cyc_Absyn_Evar_Absyn_Type_struct*
_tmp2ED=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2EC;if(_tmp2ED->tag != 1)
goto _LL18C;};_tmp2EE=_tmp2E7.f2;{struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*
_tmp2EF=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp2EE;if(_tmp2EF->tag != 
10)goto _LL18C;else{_tmp2F0=_tmp2EF->f1;}};_LL18B: {
# 1069
int _tmp2F5=*((int*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
counts,_tmp2F0);
# 1070
if(_tmp2F5 > 1){
# 1071
struct Cyc_Port_Edit*_tmp5FE;const char*_tmp5FD;void*_tmp5FC[1];struct Cyc_String_pa_PrintArg_struct
_tmp5FB;const char*_tmp5FA;struct Cyc_List_List*_tmp5F9;(env->gcenv)->edits=(
# 1072
(_tmp5F9=_cycalloc(sizeof(*_tmp5F9)),((_tmp5F9->hd=((_tmp5FE=_cycalloc(sizeof(*
_tmp5FE)),((_tmp5FE->loc=_tmp2E5,((_tmp5FE->old_string=(struct _dyneither_ptr)((
_tmp5FB.tag=0,((_tmp5FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2F0),((
_tmp5FC[0]=& _tmp5FB,Cyc_aprintf(((_tmp5FD="%s ",_tag_dyneither(_tmp5FD,sizeof(
char),4))),_tag_dyneither(_tmp5FC,sizeof(void*),1)))))))),((_tmp5FE->new_string=((
_tmp5FA="",_tag_dyneither(_tmp5FA,sizeof(char),1))),_tmp5FE)))))))),((_tmp5F9->tl=(
env->gcenv)->edits,_tmp5F9))))));}
# 1073
goto _LL187;}_LL18C:;_LL18D:
# 1074
 goto _LL187;_LL187:;}
# 1076
goto _LL17E;};_LL181: _tmp2D5=_tmp2C9.f1;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp2D6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D5;if(_tmp2D6->tag != 
8)goto _LL183;else{_tmp2D7=_tmp2D6->f1;_tmp2D8=_tmp2D7.elt_type;}};_tmp2D9=
_tmp2C9.f2;{struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp2DA=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)
_tmp2D9;if(_tmp2DA->tag != 12)goto _LL183;else{_tmp2DB=(void*)_tmp2DA->f1;}};
_LL182:
# 1078
 Cyc_Port_register_rgns(env,counts,fn_res,_tmp2D8,_tmp2DB);
# 1079
goto _LL17E;_LL183: _tmp2DC=_tmp2C9.f1;{struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp2DD=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2DC;if(_tmp2DD->tag != 9)
goto _LL185;else{_tmp2DE=_tmp2DD->f1;_tmp2DF=_tmp2DE.ret_typ;_tmp2E0=_tmp2DE.args;}};
_tmp2E1=_tmp2C9.f2;{struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp2E2=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)
_tmp2E1;if(_tmp2E2->tag != 15)goto _LL185;else{_tmp2E3=(void*)_tmp2E2->f1;_tmp2E4=
_tmp2E2->f2;}};_LL184:
# 1081
 Cyc_Port_register_rgns(env,counts,1,_tmp2DF,_tmp2E3);
# 1082
for(0;_tmp2E0 != 0  && _tmp2E4 != 0;(_tmp2E0=_tmp2E0->tl,_tmp2E4=_tmp2E4->tl)){
# 1083
void*_tmp2FD;struct _tuple8 _tmp2FC=*((struct _tuple8*)_tmp2E0->hd);_tmp2FD=
_tmp2FC.f3;{
# 1084
void*_tmp2FE=(void*)_tmp2E4->hd;
# 1085
Cyc_Port_register_rgns(env,counts,0,_tmp2FD,_tmp2FE);};}
# 1087
goto _LL17E;_LL185:;_LL186:
# 1088
 goto _LL17E;_LL17E:;};}struct Cyc_Port_Cenv;
# 1094
static void*Cyc_Port_gen_exp(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e);
struct Cyc_Port_Cenv;
# 1095
static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Stmt*s);
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1096
static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*env,struct
Cyc_Absyn_Decl*d);struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1099
static int Cyc_Port_is_char(struct Cyc_Port_Cenv*env,void*t){
# 1100
void*_tmp2FF=t;struct _tuple0*_tmp301;enum Cyc_Absyn_Size_of _tmp303;_LL18F: {
struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp300=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)
_tmp2FF;if(_tmp300->tag != 17)goto _LL191;else{_tmp301=_tmp300->f1;}}_LL190:
# 1102
(*_tmp301).f1=Cyc_Absyn_Loc_n;
# 1103
return Cyc_Port_is_char(env,Cyc_Port_lookup_typedef(env,_tmp301));_LL191: {
struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp302=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp2FF;if(_tmp302->tag != 6)goto _LL193;else{_tmp303=_tmp302->f2;}}_LL192:
# 1104
 return 1;_LL193:;_LL194:
# 1105
 return 0;_LL18E:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1110
static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t){
# 1111
void*_tmp304=t;struct _tuple0*_tmp306;struct Cyc_Absyn_PtrInfo _tmp309;void*
_tmp30A;struct Cyc_Absyn_Tqual _tmp30B;struct Cyc_Absyn_PtrAtts _tmp30C;void*_tmp30D;
union Cyc_Absyn_Constraint*_tmp30E;union Cyc_Absyn_Constraint*_tmp30F;union Cyc_Absyn_Constraint*
_tmp310;struct Cyc_Absyn_PtrLoc*_tmp311;struct Cyc_Absyn_ArrayInfo _tmp315;void*
_tmp316;struct Cyc_Absyn_Tqual _tmp317;union Cyc_Absyn_Constraint*_tmp318;
unsigned int _tmp319;struct Cyc_Absyn_FnInfo _tmp31B;void*_tmp31C;struct Cyc_List_List*
_tmp31D;struct Cyc_Absyn_AggrInfo _tmp31F;union Cyc_Absyn_AggrInfoU _tmp320;struct
Cyc_List_List*_tmp323;_LL196: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*
_tmp305=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp304;if(_tmp305->tag
!= 17)goto _LL198;else{_tmp306=_tmp305->f1;}}_LL197:
# 1113
(*_tmp306).f1=Cyc_Absyn_Loc_n;
# 1114
return Cyc_Port_lookup_typedef_ctype(env,_tmp306);_LL198: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*
_tmp307=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp304;if(_tmp307->tag != 0)
goto _LL19A;}_LL199:
# 1115
 return Cyc_Port_void_ct();_LL19A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp308=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp304;if(_tmp308->tag
!= 5)goto _LL19C;else{_tmp309=_tmp308->f1;_tmp30A=_tmp309.elt_typ;_tmp30B=_tmp309.elt_tq;
_tmp30C=_tmp309.ptr_atts;_tmp30D=_tmp30C.rgn;_tmp30E=_tmp30C.nullable;_tmp30F=
_tmp30C.bounds;_tmp310=_tmp30C.zero_term;_tmp311=_tmp30C.ptrloc;}}_LL19B: {
# 1117
unsigned int _tmp324=(unsigned int)_tmp311?_tmp311->ptr_loc:(unsigned int)0;
# 1118
unsigned int _tmp325=(unsigned int)_tmp311?_tmp311->rgn_loc:(unsigned int)0;
# 1119
unsigned int _tmp326=(unsigned int)_tmp311?_tmp311->zt_loc:(unsigned int)0;
# 1123
void*_tmp327=Cyc_Port_type_to_ctype(env,_tmp30A);
# 1124
void*new_rgn;
# 1126
{void*_tmp328=_tmp30D;struct Cyc_Absyn_Tvar*_tmp32B;struct Cyc_Absyn_Tvar _tmp32C;
struct _dyneither_ptr*_tmp32D;_LL1AD: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*
_tmp329=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp328;if(_tmp329->tag != 20)
goto _LL1AF;}_LL1AE:
# 1128
 new_rgn=Cyc_Port_heap_ct();goto _LL1AC;_LL1AF: {struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp32A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp328;if(_tmp32A->tag != 2)
goto _LL1B1;else{_tmp32B=_tmp32A->f1;_tmp32C=*_tmp32B;_tmp32D=_tmp32C.name;}}
_LL1B0:
# 1130
 new_rgn=Cyc_Port_rgnvar_ct(_tmp32D);goto _LL1AC;_LL1B1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*
_tmp32E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp328;if(_tmp32E->tag != 1)
goto _LL1B3;}_LL1B2:
# 1134
 if(Cyc_Port_in_fn_arg(env))
# 1135
new_rgn=Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var());else{
# 1137
if(Cyc_Port_in_fn_res(env) || Cyc_Port_in_toplevel(env))
# 1138
new_rgn=Cyc_Port_heap_ct();else{
# 1139
new_rgn=Cyc_Port_var();}}
# 1140
goto _LL1AC;_LL1B3:;_LL1B4:
# 1142
 new_rgn=Cyc_Port_heap_ct();goto _LL1AC;_LL1AC:;}{
# 1144
int _tmp32F=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp30F)== (void*)& Cyc_Absyn_DynEither_b_val;
# 1145
int orig_zt;
# 1146
{union Cyc_Absyn_Constraint _tmp330=*_tmp310;int _tmp331;_LL1B6: if((_tmp330.No_constr).tag
!= 3)goto _LL1B8;_tmp331=(int)(_tmp330.No_constr).val;_LL1B7:
# 1147
 orig_zt=Cyc_Port_is_char(env,_tmp30A);goto _LL1B5;_LL1B8:;_LL1B9:
# 1148
 orig_zt=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp310);
goto _LL1B5;_LL1B5:;}
# 1150
if((env->gcenv)->porting){
# 1151
void*_tmp332=Cyc_Port_var();
# 1155
Cyc_Port_register_const_cvar(env,_tmp332,_tmp30B.print_const?Cyc_Port_const_ct():
Cyc_Port_notconst_ct(),_tmp30B.loc);
# 1158
if(_tmp30B.print_const)Cyc_Port_unify_ct(_tmp332,Cyc_Port_const_ct());{
# 1164
void*_tmp333=Cyc_Port_var();
# 1167
Cyc_Port_register_ptr_cvars(env,_tmp333,_tmp32F?Cyc_Port_fat_ct(): Cyc_Port_thin_ct(),
_tmp324);
# 1169
if(_tmp32F)Cyc_Port_unify_ct(_tmp333,Cyc_Port_fat_ct());{
# 1170
void*_tmp334=Cyc_Port_var();
# 1173
Cyc_Port_register_zeroterm_cvars(env,_tmp334,orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct(),
_tmp326);
# 1175
{union Cyc_Absyn_Constraint _tmp335=*_tmp310;int _tmp336;_LL1BB: if((_tmp335.No_constr).tag
!= 3)goto _LL1BD;_tmp336=(int)(_tmp335.No_constr).val;_LL1BC:
# 1177
 goto _LL1BA;_LL1BD:;_LL1BE:
# 1179
 Cyc_Port_unify_ct(_tmp334,((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp310)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());
# 1180
goto _LL1BA;_LL1BA:;}
# 1185
return Cyc_Port_ptr_ct(_tmp327,_tmp332,_tmp333,new_rgn,_tmp334);};};}else{
# 1189
return Cyc_Port_ptr_ct(_tmp327,_tmp30B.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),
# 1190
_tmp32F?Cyc_Port_fat_ct(): Cyc_Port_thin_ct(),new_rgn,
# 1191
orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());}};}_LL19C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp312=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp304;if(_tmp312->tag != 6)
goto _LL19E;}_LL19D:
# 1193
 goto _LL19F;_LL19E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp313=(struct
Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp304;if(_tmp313->tag != 7)goto _LL1A0;}
_LL19F:
# 1194
 return Cyc_Port_arith_ct();_LL1A0: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp314=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp304;if(_tmp314->tag != 
8)goto _LL1A2;else{_tmp315=_tmp314->f1;_tmp316=_tmp315.elt_type;_tmp317=_tmp315.tq;
_tmp318=_tmp315.zero_term;_tmp319=_tmp315.zt_loc;}}_LL1A1: {
# 1197
void*_tmp337=Cyc_Port_type_to_ctype(env,_tmp316);
# 1198
int orig_zt;
# 1199
{union Cyc_Absyn_Constraint _tmp338=*_tmp318;int _tmp339;_LL1C0: if((_tmp338.No_constr).tag
!= 3)goto _LL1C2;_tmp339=(int)(_tmp338.No_constr).val;_LL1C1:
# 1200
 orig_zt=0;goto _LL1BF;_LL1C2:;_LL1C3:
# 1201
 orig_zt=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp318);
goto _LL1BF;_LL1BF:;}
# 1203
if((env->gcenv)->porting){
# 1204
void*_tmp33A=Cyc_Port_var();
# 1205
Cyc_Port_register_const_cvar(env,_tmp33A,_tmp317.print_const?Cyc_Port_const_ct():
Cyc_Port_notconst_ct(),_tmp317.loc);{
# 1212
void*_tmp33B=Cyc_Port_var();
# 1213
Cyc_Port_register_zeroterm_cvars(env,_tmp33B,orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct(),
_tmp319);
# 1215
{union Cyc_Absyn_Constraint _tmp33C=*_tmp318;int _tmp33D;_LL1C5: if((_tmp33C.No_constr).tag
!= 3)goto _LL1C7;_tmp33D=(int)(_tmp33C.No_constr).val;_LL1C6:
# 1217
 goto _LL1C4;_LL1C7:;_LL1C8:
# 1219
 Cyc_Port_unify_ct(_tmp33B,((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp318)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());
# 1220
goto _LL1C4;_LL1C4:;}
# 1222
return Cyc_Port_array_ct(_tmp337,_tmp33A,_tmp33B);};}else{
# 1224
return Cyc_Port_array_ct(_tmp337,_tmp317.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),
# 1225
orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());}}_LL1A2: {struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp31A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp304;if(_tmp31A->tag != 9)
goto _LL1A4;else{_tmp31B=_tmp31A->f1;_tmp31C=_tmp31B.ret_typ;_tmp31D=_tmp31B.args;}}
_LL1A3: {
# 1228
void*_tmp33E=Cyc_Port_type_to_ctype(Cyc_Port_set_cpos(env,Cyc_Port_FnRes_pos),
_tmp31C);
# 1229
struct Cyc_Port_Cenv*_tmp33F=Cyc_Port_set_cpos(env,Cyc_Port_FnArg_pos);
# 1230
struct Cyc_List_List*cargs=0;
# 1231
for(0;_tmp31D != 0;_tmp31D=_tmp31D->tl){
# 1232
struct Cyc_Absyn_Tqual _tmp341;void*_tmp342;struct _tuple8 _tmp340=*((struct _tuple8*)
_tmp31D->hd);_tmp341=_tmp340.f2;_tmp342=_tmp340.f3;{
# 1233
struct Cyc_List_List*_tmp5FF;cargs=((_tmp5FF=_cycalloc(sizeof(*_tmp5FF)),((
_tmp5FF->hd=Cyc_Port_type_to_ctype(_tmp33F,_tmp342),((_tmp5FF->tl=cargs,_tmp5FF))))));};}
# 1235
return Cyc_Port_fn_ct(_tmp33E,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(cargs));}_LL1A4: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp31E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp304;if(_tmp31E->tag != 
11)goto _LL1A6;else{_tmp31F=_tmp31E->f1;_tmp320=_tmp31F.aggr_info;}}_LL1A5: {
# 1237
union Cyc_Absyn_AggrInfoU _tmp344=_tmp320;struct _tuple2 _tmp345;enum Cyc_Absyn_AggrKind
_tmp346;struct _tuple0*_tmp347;struct _tuple2 _tmp348;enum Cyc_Absyn_AggrKind
_tmp349;struct _tuple0*_tmp34A;struct Cyc_Absyn_Aggrdecl**_tmp34B;_LL1CA: if((
_tmp344.UnknownAggr).tag != 1)goto _LL1CC;_tmp345=(struct _tuple2)(_tmp344.UnknownAggr).val;
_tmp346=_tmp345.f1;if(_tmp346 != Cyc_Absyn_StructA)goto _LL1CC;_tmp347=_tmp345.f2;
_LL1CB:
# 1239
(*_tmp347).f1=Cyc_Absyn_Loc_n;{
# 1240
struct _tuple11*_tmp34C=Cyc_Port_lookup_struct_decl(env,_tmp347);
# 1241
return Cyc_Port_known_aggr_ct(_tmp34C);};_LL1CC: if((_tmp344.UnknownAggr).tag != 1)
goto _LL1CE;_tmp348=(struct _tuple2)(_tmp344.UnknownAggr).val;_tmp349=_tmp348.f1;
if(_tmp349 != Cyc_Absyn_UnionA)goto _LL1CE;_tmp34A=_tmp348.f2;_LL1CD:
# 1243
(*_tmp34A).f1=Cyc_Absyn_Loc_n;{
# 1244
struct _tuple11*_tmp34D=Cyc_Port_lookup_union_decl(env,_tmp34A);
# 1245
return Cyc_Port_known_aggr_ct(_tmp34D);};_LL1CE: if((_tmp344.KnownAggr).tag != 2)
goto _LL1C9;_tmp34B=(struct Cyc_Absyn_Aggrdecl**)(_tmp344.KnownAggr).val;_LL1CF: {
# 1247
struct Cyc_Absyn_Aggrdecl*_tmp34E=*_tmp34B;
# 1248
switch(_tmp34E->kind){case Cyc_Absyn_StructA: _LL1D0: {
# 1250
struct _tuple11*_tmp34F=Cyc_Port_lookup_struct_decl(env,_tmp34E->name);
# 1251
return Cyc_Port_known_aggr_ct(_tmp34F);}case Cyc_Absyn_UnionA: _LL1D1: {
# 1253
struct _tuple11*_tmp350=Cyc_Port_lookup_union_decl(env,_tmp34E->name);
# 1254
return Cyc_Port_known_aggr_ct(_tmp350);}}}_LL1C9:;}_LL1A6: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmp321=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp304;if(_tmp321->tag != 
13)goto _LL1A8;}_LL1A7:
# 1257
 return Cyc_Port_arith_ct();_LL1A8: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*
_tmp322=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp304;if(_tmp322->tag
!= 14)goto _LL1AA;else{_tmp323=_tmp322->f1;}}_LL1A9:
# 1260
 for(0;(unsigned int)_tmp323;_tmp323=_tmp323->tl){
# 1261
(*((struct Cyc_Absyn_Enumfield*)_tmp323->hd)->name).f1=Cyc_Absyn_Loc_n;
# 1262
env=Cyc_Port_add_var(env,((struct Cyc_Absyn_Enumfield*)_tmp323->hd)->name,Cyc_Absyn_sint_typ,
Cyc_Port_const_ct(),Cyc_Port_arith_ct());}
# 1264
return Cyc_Port_arith_ct();_LL1AA:;_LL1AB:
# 1265
 return Cyc_Port_arith_ct();_LL195:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1270
static void*Cyc_Port_gen_primop(struct Cyc_Port_Cenv*env,enum Cyc_Absyn_Primop p,
struct Cyc_List_List*args){
# 1271
void*_tmp351=Cyc_Port_compress_ct((void*)((struct Cyc_List_List*)_check_null(
args))->hd);
# 1272
struct Cyc_List_List*_tmp352=args->tl;
# 1273
enum Cyc_Absyn_Primop _tmp353=p;_LL1D4: if(_tmp353 != Cyc_Absyn_Plus)goto _LL1D6;
_LL1D5: {
# 1278
void*_tmp354=Cyc_Port_compress_ct((void*)((struct Cyc_List_List*)_check_null(
_tmp352))->hd);
# 1279
if(Cyc_Port_leq(_tmp351,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),
Cyc_Port_var(),Cyc_Port_var()))){
# 1280
Cyc_Port_leq(_tmp354,Cyc_Port_arith_ct());
# 1281
return _tmp351;}else{
# 1282
if(Cyc_Port_leq(_tmp354,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),
Cyc_Port_var(),Cyc_Port_var()))){
# 1283
Cyc_Port_leq(_tmp351,Cyc_Port_arith_ct());
# 1284
return _tmp354;}else{
# 1286
Cyc_Port_leq(_tmp351,Cyc_Port_arith_ct());
# 1287
Cyc_Port_leq(_tmp354,Cyc_Port_arith_ct());
# 1288
return Cyc_Port_arith_ct();}}}_LL1D6: if(_tmp353 != Cyc_Absyn_Minus)goto _LL1D8;
_LL1D7:
# 1295
 if(_tmp352 == 0){
# 1297
Cyc_Port_leq(_tmp351,Cyc_Port_arith_ct());
# 1298
return Cyc_Port_arith_ct();}else{
# 1300
void*_tmp355=Cyc_Port_compress_ct((void*)_tmp352->hd);
# 1301
if(Cyc_Port_leq(_tmp351,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),
Cyc_Port_var(),Cyc_Port_var()))){
# 1302
if(Cyc_Port_leq(_tmp355,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),
Cyc_Port_var(),Cyc_Port_var())))
# 1303
return Cyc_Port_arith_ct();else{
# 1305
Cyc_Port_leq(_tmp355,Cyc_Port_arith_ct());
# 1306
return _tmp351;}}else{
# 1309
Cyc_Port_leq(_tmp351,Cyc_Port_arith_ct());
# 1310
Cyc_Port_leq(_tmp351,Cyc_Port_arith_ct());
# 1311
return Cyc_Port_arith_ct();}}_LL1D8: if(_tmp353 != Cyc_Absyn_Times)goto _LL1DA;
_LL1D9:
# 1314
 goto _LL1DB;_LL1DA: if(_tmp353 != Cyc_Absyn_Div)goto _LL1DC;_LL1DB: goto _LL1DD;
_LL1DC: if(_tmp353 != Cyc_Absyn_Mod)goto _LL1DE;_LL1DD: goto _LL1DF;_LL1DE: if(_tmp353
!= Cyc_Absyn_Eq)goto _LL1E0;_LL1DF: goto _LL1E1;_LL1E0: if(_tmp353 != Cyc_Absyn_Neq)
goto _LL1E2;_LL1E1: goto _LL1E3;_LL1E2: if(_tmp353 != Cyc_Absyn_Lt)goto _LL1E4;_LL1E3:
goto _LL1E5;_LL1E4: if(_tmp353 != Cyc_Absyn_Gt)goto _LL1E6;_LL1E5: goto _LL1E7;_LL1E6:
if(_tmp353 != Cyc_Absyn_Lte)goto _LL1E8;_LL1E7:
# 1315
 goto _LL1E9;_LL1E8: if(_tmp353 != Cyc_Absyn_Gte)goto _LL1EA;_LL1E9: goto _LL1EB;
_LL1EA: if(_tmp353 != Cyc_Absyn_Bitand)goto _LL1EC;_LL1EB: goto _LL1ED;_LL1EC: if(
_tmp353 != Cyc_Absyn_Bitor)goto _LL1EE;_LL1ED: goto _LL1EF;_LL1EE: if(_tmp353 != Cyc_Absyn_Bitxor)
goto _LL1F0;_LL1EF: goto _LL1F1;_LL1F0: if(_tmp353 != Cyc_Absyn_Bitlshift)goto _LL1F2;
_LL1F1: goto _LL1F3;_LL1F2: if(_tmp353 != Cyc_Absyn_Bitlrshift)goto _LL1F4;_LL1F3:
# 1316
 goto _LL1F5;_LL1F4: if(_tmp353 != Cyc_Absyn_Bitarshift)goto _LL1F6;_LL1F5:
# 1317
 Cyc_Port_leq(_tmp351,Cyc_Port_arith_ct());
# 1318
Cyc_Port_leq((void*)((struct Cyc_List_List*)_check_null(_tmp352))->hd,Cyc_Port_arith_ct());
# 1319
return Cyc_Port_arith_ct();_LL1F6: if(_tmp353 != Cyc_Absyn_Not)goto _LL1F8;_LL1F7:
# 1320
 goto _LL1F9;_LL1F8: if(_tmp353 != Cyc_Absyn_Bitnot)goto _LL1FA;_LL1F9:
# 1322
 Cyc_Port_leq(_tmp351,Cyc_Port_arith_ct());
# 1323
return Cyc_Port_arith_ct();_LL1FA:;_LL1FB: {
# 1324
const char*_tmp602;void*_tmp601;(_tmp601=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp602=".size primop",
_tag_dyneither(_tmp602,sizeof(char),13))),_tag_dyneither(_tmp601,sizeof(void*),0)));}
_LL1D3:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
# 1329
static struct _tuple10 Cyc_Port_gen_lhs(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*
e){
# 1330
void*_tmp358=e->r;struct _tuple0*_tmp35A;struct Cyc_Absyn_Exp*_tmp35C;struct Cyc_Absyn_Exp*
_tmp35D;struct Cyc_Absyn_Exp*_tmp35F;struct Cyc_Absyn_Exp*_tmp361;struct
_dyneither_ptr*_tmp362;struct Cyc_Absyn_Exp*_tmp364;struct _dyneither_ptr*_tmp365;
_LL1FD: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp359=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmp358;if(_tmp359->tag != 1)goto _LL1FF;else{_tmp35A=_tmp359->f1;}}_LL1FE:
# 1332
(*_tmp35A).f1=Cyc_Absyn_Loc_n;
# 1333
return Cyc_Port_lookup_var(env,_tmp35A);_LL1FF: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*
_tmp35B=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp358;if(_tmp35B->tag
!= 22)goto _LL201;else{_tmp35C=_tmp35B->f1;_tmp35D=_tmp35B->f2;}}_LL200: {
# 1335
void*_tmp366=Cyc_Port_var();
# 1336
void*_tmp367=Cyc_Port_var();
# 1337
void*_tmp368=Cyc_Port_gen_exp(env,_tmp35C);
# 1338
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp35D),Cyc_Port_arith_ct());{
# 1339
void*_tmp369=Cyc_Port_ptr_ct(_tmp366,_tmp367,Cyc_Port_fat_ct(),Cyc_Port_var(),
Cyc_Port_var());
# 1340
Cyc_Port_leq(_tmp368,_tmp369);{
# 1341
struct _tuple10 _tmp603;return(_tmp603.f1=_tmp367,((_tmp603.f2=_tmp366,_tmp603)));};};}
_LL201: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp35E=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)
_tmp358;if(_tmp35E->tag != 19)goto _LL203;else{_tmp35F=_tmp35E->f1;}}_LL202: {
# 1343
void*_tmp36B=Cyc_Port_var();
# 1344
void*_tmp36C=Cyc_Port_var();
# 1345
void*_tmp36D=Cyc_Port_ptr_ct(_tmp36B,_tmp36C,Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var());
# 1346
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp35F),_tmp36D);{
# 1347
struct _tuple10 _tmp604;return(_tmp604.f1=_tmp36C,((_tmp604.f2=_tmp36B,_tmp604)));};}
_LL203: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp360=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)
_tmp358;if(_tmp360->tag != 20)goto _LL205;else{_tmp361=_tmp360->f1;_tmp362=_tmp360->f2;}}
_LL204: {
# 1349
void*_tmp36F=Cyc_Port_var();
# 1350
void*_tmp370=Cyc_Port_var();
# 1351
void*_tmp371=Cyc_Port_gen_exp(env,_tmp361);
# 1352
{struct Cyc_Port_Cfield*_tmp607;struct Cyc_Port_Cfield*_tmp606[1];Cyc_Port_leq(
Cyc_Port_gen_exp(env,_tmp361),Cyc_Port_unknown_aggr_ct(((_tmp606[0]=((_tmp607=
_cycalloc(sizeof(*_tmp607)),((_tmp607->qual=_tmp370,((_tmp607->f=_tmp362,((
_tmp607->type=_tmp36F,_tmp607)))))))),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp606,sizeof(struct Cyc_Port_Cfield*),
1))))));}{
# 1353
struct _tuple10 _tmp608;return(_tmp608.f1=_tmp370,((_tmp608.f2=_tmp36F,_tmp608)));};}
_LL205: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp363=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)
_tmp358;if(_tmp363->tag != 21)goto _LL207;else{_tmp364=_tmp363->f1;_tmp365=_tmp363->f2;}}
_LL206: {
# 1355
void*_tmp375=Cyc_Port_var();
# 1356
void*_tmp376=Cyc_Port_var();
# 1357
void*_tmp377=Cyc_Port_gen_exp(env,_tmp364);
# 1358
{struct Cyc_Port_Cfield*_tmp60B;struct Cyc_Port_Cfield*_tmp60A[1];Cyc_Port_leq(
Cyc_Port_gen_exp(env,_tmp364),Cyc_Port_ptr_ct(Cyc_Port_unknown_aggr_ct(((_tmp60A[
0]=((_tmp60B=_cycalloc(sizeof(*_tmp60B)),((_tmp60B->qual=_tmp376,((_tmp60B->f=
_tmp365,((_tmp60B->type=_tmp375,_tmp60B)))))))),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp60A,sizeof(struct Cyc_Port_Cfield*),
1))))),
# 1359
Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var()));}{
# 1360
struct _tuple10 _tmp60C;return(_tmp60C.f1=_tmp376,((_tmp60C.f2=_tmp375,_tmp60C)));};}
_LL207:;_LL208: {
# 1361
struct Cyc_String_pa_PrintArg_struct _tmp614;void*_tmp613[1];const char*_tmp612;
void*_tmp611;(_tmp611=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmp614.tag=0,((_tmp614.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp613[0]=&
_tmp614,Cyc_aprintf(((_tmp612="gen_lhs: %s",_tag_dyneither(_tmp612,sizeof(char),
12))),_tag_dyneither(_tmp613,sizeof(void*),1)))))))),_tag_dyneither(_tmp611,
sizeof(void*),0)));}_LL1FC:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cfield;
struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
# 1366
static void*Cyc_Port_gen_exp(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){
# 1367
void*_tmp37F=e->r;union Cyc_Absyn_Cnst _tmp381;struct _tuple3 _tmp382;union Cyc_Absyn_Cnst
_tmp384;struct _dyneither_ptr _tmp385;union Cyc_Absyn_Cnst _tmp387;struct _tuple4
_tmp388;union Cyc_Absyn_Cnst _tmp38A;struct _tuple6 _tmp38B;union Cyc_Absyn_Cnst
_tmp392;struct _tuple7 _tmp393;union Cyc_Absyn_Cnst _tmp395;struct _tuple5 _tmp396;int
_tmp397;union Cyc_Absyn_Cnst _tmp399;struct _tuple5 _tmp39A;union Cyc_Absyn_Cnst
_tmp39C;struct _dyneither_ptr _tmp39D;union Cyc_Absyn_Cnst _tmp39F;struct
_dyneither_ptr _tmp3A0;union Cyc_Absyn_Cnst _tmp3A2;int _tmp3A3;struct _tuple0*
_tmp3A5;enum Cyc_Absyn_Primop _tmp3A7;struct Cyc_List_List*_tmp3A8;struct Cyc_Absyn_Exp*
_tmp3AA;struct Cyc_Core_Opt*_tmp3AB;struct Cyc_Absyn_Exp*_tmp3AC;struct Cyc_Absyn_Exp*
_tmp3AE;enum Cyc_Absyn_Incrementor _tmp3AF;struct Cyc_Absyn_Exp*_tmp3B1;struct Cyc_Absyn_Exp*
_tmp3B2;struct Cyc_Absyn_Exp*_tmp3B3;struct Cyc_Absyn_Exp*_tmp3B5;struct Cyc_Absyn_Exp*
_tmp3B6;struct Cyc_Absyn_Exp*_tmp3B8;struct Cyc_Absyn_Exp*_tmp3B9;struct Cyc_Absyn_Exp*
_tmp3BB;struct Cyc_Absyn_Exp*_tmp3BC;struct Cyc_Absyn_Exp*_tmp3BE;struct Cyc_List_List*
_tmp3BF;struct Cyc_Absyn_Exp*_tmp3C2;void*_tmp3C5;struct Cyc_Absyn_Exp*_tmp3C6;
struct Cyc_Absyn_Exp*_tmp3C8;struct Cyc_Absyn_Exp*_tmp3CA;struct Cyc_Absyn_Exp*
_tmp3CB;struct Cyc_Absyn_Exp*_tmp3CD;struct Cyc_Absyn_Exp*_tmp3CF;struct
_dyneither_ptr*_tmp3D0;struct Cyc_Absyn_Exp*_tmp3D2;struct _dyneither_ptr*_tmp3D3;
struct Cyc_Absyn_MallocInfo _tmp3D5;void**_tmp3D6;struct Cyc_Absyn_Exp*_tmp3D7;
struct Cyc_Absyn_Exp*_tmp3D9;struct Cyc_Absyn_Exp*_tmp3DA;struct Cyc_Absyn_Stmt*
_tmp3E5;_LL20A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp380=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmp37F;if(_tmp380->tag != 0)goto _LL20C;else{_tmp381=_tmp380->f1;if((_tmp381.Char_c).tag
!= 2)goto _LL20C;_tmp382=(struct _tuple3)(_tmp381.Char_c).val;}}_LL20B:
# 1368
 goto _LL20D;_LL20C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp383=(
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp383->tag != 0)goto
_LL20E;else{_tmp384=_tmp383->f1;if((_tmp384.Wchar_c).tag != 3)goto _LL20E;_tmp385=(
struct _dyneither_ptr)(_tmp384.Wchar_c).val;}}_LL20D:
# 1369
 goto _LL20F;_LL20E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp386=(
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp386->tag != 0)goto
_LL210;else{_tmp387=_tmp386->f1;if((_tmp387.Short_c).tag != 4)goto _LL210;_tmp388=(
struct _tuple4)(_tmp387.Short_c).val;}}_LL20F:
# 1370
 goto _LL211;_LL210: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp389=(
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp389->tag != 0)goto
_LL212;else{_tmp38A=_tmp389->f1;if((_tmp38A.LongLong_c).tag != 6)goto _LL212;
_tmp38B=(struct _tuple6)(_tmp38A.LongLong_c).val;}}_LL211:
# 1371
 goto _LL213;_LL212: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp38C=(
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp38C->tag != 16)
goto _LL214;}_LL213:
# 1372
 goto _LL215;_LL214: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp38D=(
struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp38D->tag != 17)
goto _LL216;}_LL215:
# 1373
 goto _LL217;_LL216: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp38E=(
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp38E->tag != 18)
goto _LL218;}_LL217:
# 1374
 goto _LL219;_LL218: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp38F=(struct
Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp38F->tag != 30)goto _LL21A;}
_LL219:
# 1375
 goto _LL21B;_LL21A: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp390=(
struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp390->tag != 31)
goto _LL21C;}_LL21B:
# 1376
 goto _LL21D;_LL21C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp391=(
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp391->tag != 0)goto
_LL21E;else{_tmp392=_tmp391->f1;if((_tmp392.Float_c).tag != 7)goto _LL21E;_tmp393=(
struct _tuple7)(_tmp392.Float_c).val;}}_LL21D:
# 1377
 return Cyc_Port_arith_ct();_LL21E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp394=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp394->tag != 
0)goto _LL220;else{_tmp395=_tmp394->f1;if((_tmp395.Int_c).tag != 5)goto _LL220;
_tmp396=(struct _tuple5)(_tmp395.Int_c).val;_tmp397=_tmp396.f2;if(_tmp397 != 0)
goto _LL220;}}_LL21F:
# 1378
 return Cyc_Port_zero_ct();_LL220: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp398=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp398->tag != 
0)goto _LL222;else{_tmp399=_tmp398->f1;if((_tmp399.Int_c).tag != 5)goto _LL222;
_tmp39A=(struct _tuple5)(_tmp399.Int_c).val;}}_LL221:
# 1379
 return Cyc_Port_arith_ct();_LL222: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp39B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp39B->tag != 
0)goto _LL224;else{_tmp39C=_tmp39B->f1;if((_tmp39C.String_c).tag != 8)goto _LL224;
_tmp39D=(struct _dyneither_ptr)(_tmp39C.String_c).val;}}_LL223:
# 1381
 return Cyc_Port_ptr_ct(Cyc_Port_arith_ct(),Cyc_Port_const_ct(),Cyc_Port_fat_ct(),
Cyc_Port_heap_ct(),Cyc_Port_zterm_ct());_LL224: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp39E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp39E->tag != 
0)goto _LL226;else{_tmp39F=_tmp39E->f1;if((_tmp39F.Wstring_c).tag != 9)goto _LL226;
_tmp3A0=(struct _dyneither_ptr)(_tmp39F.Wstring_c).val;}}_LL225:
# 1383
 return Cyc_Port_ptr_ct(Cyc_Port_arith_ct(),Cyc_Port_const_ct(),Cyc_Port_fat_ct(),
Cyc_Port_heap_ct(),Cyc_Port_zterm_ct());_LL226: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*
_tmp3A1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp3A1->tag != 
0)goto _LL228;else{_tmp3A2=_tmp3A1->f1;if((_tmp3A2.Null_c).tag != 1)goto _LL228;
_tmp3A3=(int)(_tmp3A2.Null_c).val;}}_LL227:
# 1384
 return Cyc_Port_zero_ct();_LL228: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*
_tmp3A4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp3A4->tag != 1)
goto _LL22A;else{_tmp3A5=_tmp3A4->f1;}}_LL229:
# 1386
(*_tmp3A5).f1=Cyc_Absyn_Loc_n;{
# 1387
void*_tmp3EA;struct _tuple10 _tmp3E9=Cyc_Port_lookup_var(env,_tmp3A5);_tmp3EA=
_tmp3E9.f2;
# 1388
if(Cyc_Port_isfn(env,_tmp3A5)){
# 1389
_tmp3EA=Cyc_Port_instantiate(_tmp3EA);
# 1390
return Cyc_Port_ptr_ct(_tmp3EA,Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_heap_ct(),
Cyc_Port_nozterm_ct());}else{
# 1392
if(Cyc_Port_isarray(env,_tmp3A5)){
# 1393
void*_tmp3EB=Cyc_Port_var();
# 1394
void*_tmp3EC=Cyc_Port_var();
# 1395
void*_tmp3ED=Cyc_Port_var();
# 1396
void*_tmp3EE=Cyc_Port_array_ct(_tmp3EB,_tmp3EC,_tmp3ED);
# 1397
Cyc_Port_unifies(_tmp3EA,_tmp3EE);{
# 1398
void*_tmp3EF=Cyc_Port_ptr_ct(_tmp3EB,_tmp3EC,Cyc_Port_fat_ct(),Cyc_Port_var(),
_tmp3ED);
# 1399
return _tmp3EF;};}else{
# 1400
return _tmp3EA;}}};_LL22A: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*
_tmp3A6=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp3A6->tag
!= 2)goto _LL22C;else{_tmp3A7=_tmp3A6->f1;_tmp3A8=_tmp3A6->f2;}}_LL22B:
# 1401
 return Cyc_Port_gen_primop(env,_tmp3A7,((struct Cyc_List_List*(*)(void*(*f)(
struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Port_gen_exp,env,_tmp3A8));_LL22C: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*
_tmp3A9=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp3A9->tag
!= 3)goto _LL22E;else{_tmp3AA=_tmp3A9->f1;_tmp3AB=_tmp3A9->f2;_tmp3AC=_tmp3A9->f3;}}
_LL22D: {
# 1403
void*_tmp3F1;void*_tmp3F2;struct _tuple10 _tmp3F0=Cyc_Port_gen_lhs(env,_tmp3AA);
_tmp3F1=_tmp3F0.f1;_tmp3F2=_tmp3F0.f2;
# 1404
Cyc_Port_unifies(_tmp3F1,Cyc_Port_notconst_ct());{
# 1405
void*_tmp3F3=Cyc_Port_gen_exp(env,_tmp3AC);
# 1406
if((unsigned int)_tmp3AB){
# 1407
struct Cyc_List_List _tmp615;struct Cyc_List_List x2=(_tmp615.hd=_tmp3F3,((_tmp615.tl=
0,_tmp615)));
# 1408
struct Cyc_List_List _tmp616;struct Cyc_List_List x1=(_tmp616.hd=_tmp3F2,((_tmp616.tl=(
struct Cyc_List_List*)& x2,_tmp616)));
# 1409
void*_tmp3F4=Cyc_Port_gen_primop(env,(enum Cyc_Absyn_Primop)_tmp3AB->v,(struct
Cyc_List_List*)& x1);
# 1410
Cyc_Port_leq(_tmp3F4,_tmp3F2);
# 1411
return _tmp3F2;}else{
# 1413
Cyc_Port_leq(_tmp3F3,_tmp3F2);
# 1414
return _tmp3F2;}};}_LL22E: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*
_tmp3AD=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp3AD->tag
!= 4)goto _LL230;else{_tmp3AE=_tmp3AD->f1;_tmp3AF=_tmp3AD->f2;}}_LL22F: {
# 1417
void*_tmp3F8;void*_tmp3F9;struct _tuple10 _tmp3F7=Cyc_Port_gen_lhs(env,_tmp3AE);
_tmp3F8=_tmp3F7.f1;_tmp3F9=_tmp3F7.f2;
# 1418
Cyc_Port_unifies(_tmp3F8,Cyc_Port_notconst_ct());
# 1420
Cyc_Port_leq(_tmp3F9,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),
Cyc_Port_var(),Cyc_Port_var()));
# 1421
Cyc_Port_leq(_tmp3F9,Cyc_Port_arith_ct());
# 1422
return _tmp3F9;}_LL230: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*
_tmp3B0=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp3B0->tag
!= 5)goto _LL232;else{_tmp3B1=_tmp3B0->f1;_tmp3B2=_tmp3B0->f2;_tmp3B3=_tmp3B0->f3;}}
_LL231: {
# 1424
void*_tmp3FA=Cyc_Port_var();
# 1425
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp3B1),Cyc_Port_arith_ct());
# 1426
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp3B2),_tmp3FA);
# 1427
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp3B3),_tmp3FA);
# 1428
return _tmp3FA;}_LL232: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp3B4=(
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp3B4->tag != 6)goto
_LL234;else{_tmp3B5=_tmp3B4->f1;_tmp3B6=_tmp3B4->f2;}}_LL233:
# 1429
 _tmp3B8=_tmp3B5;_tmp3B9=_tmp3B6;goto _LL235;_LL234: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*
_tmp3B7=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp3B7->tag != 7)
goto _LL236;else{_tmp3B8=_tmp3B7->f1;_tmp3B9=_tmp3B7->f2;}}_LL235:
# 1431
 Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp3B8),Cyc_Port_arith_ct());
# 1432
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp3B9),Cyc_Port_arith_ct());
# 1433
return Cyc_Port_arith_ct();_LL236: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*
_tmp3BA=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp3BA->tag
!= 8)goto _LL238;else{_tmp3BB=_tmp3BA->f1;_tmp3BC=_tmp3BA->f2;}}_LL237:
# 1435
 Cyc_Port_gen_exp(env,_tmp3BB);
# 1436
return Cyc_Port_gen_exp(env,_tmp3BC);_LL238: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*
_tmp3BD=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp3BD->tag
!= 9)goto _LL23A;else{_tmp3BE=_tmp3BD->f1;_tmp3BF=_tmp3BD->f2;}}_LL239: {
# 1438
void*_tmp3FB=Cyc_Port_var();
# 1439
void*_tmp3FC=Cyc_Port_gen_exp(env,_tmp3BE);
# 1440
struct Cyc_List_List*_tmp3FD=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Port_Cenv*,
struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Port_gen_exp,env,_tmp3BF);
# 1441
struct Cyc_List_List*_tmp3FE=((struct Cyc_List_List*(*)(void*(*f)(void*),struct
Cyc_List_List*x))Cyc_List_map)((void*(*)(void*x))Cyc_Port_new_var,_tmp3FD);
# 1442
Cyc_Port_unifies(_tmp3FC,Cyc_Port_ptr_ct(Cyc_Port_fn_ct(_tmp3FB,_tmp3FE),Cyc_Port_var(),
Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_nozterm_ct()));
# 1443
for(0;_tmp3FD != 0;(_tmp3FD=_tmp3FD->tl,_tmp3FE=_tmp3FE->tl)){
# 1444
Cyc_Port_leq((void*)_tmp3FD->hd,(void*)((struct Cyc_List_List*)_check_null(
_tmp3FE))->hd);}
# 1446
return _tmp3FB;}_LL23A: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp3C0=(
struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp3C0->tag != 10)goto
_LL23C;}_LL23B: {
# 1447
const char*_tmp619;void*_tmp618;(_tmp618=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp619="throw",_tag_dyneither(
_tmp619,sizeof(char),6))),_tag_dyneither(_tmp618,sizeof(void*),0)));}_LL23C: {
struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp3C1=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)
_tmp37F;if(_tmp3C1->tag != 11)goto _LL23E;else{_tmp3C2=_tmp3C1->f1;}}_LL23D:
# 1448
 return Cyc_Port_gen_exp(env,_tmp3C2);_LL23E: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*
_tmp3C3=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp3C3->tag
!= 12)goto _LL240;}_LL23F: {
# 1449
const char*_tmp61C;void*_tmp61B;(_tmp61B=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp61C="instantiate",_tag_dyneither(
_tmp61C,sizeof(char),12))),_tag_dyneither(_tmp61B,sizeof(void*),0)));}_LL240: {
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3C4=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)
_tmp37F;if(_tmp3C4->tag != 13)goto _LL242;else{_tmp3C5=(void*)_tmp3C4->f1;_tmp3C6=
_tmp3C4->f2;}}_LL241: {
# 1451
void*_tmp403=Cyc_Port_type_to_ctype(env,_tmp3C5);
# 1452
void*_tmp404=Cyc_Port_gen_exp(env,_tmp3C6);
# 1453
Cyc_Port_leq(_tmp403,_tmp404);
# 1454
return _tmp404;}_LL242: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp3C7=(
struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp3C7->tag != 14)
goto _LL244;else{_tmp3C8=_tmp3C7->f1;}}_LL243: {
# 1456
void*_tmp406;void*_tmp407;struct _tuple10 _tmp405=Cyc_Port_gen_lhs(env,_tmp3C8);
_tmp406=_tmp405.f1;_tmp407=_tmp405.f2;
# 1457
return Cyc_Port_ptr_ct(_tmp407,_tmp406,Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var());}
_LL244: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp3C9=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)
_tmp37F;if(_tmp3C9->tag != 22)goto _LL246;else{_tmp3CA=_tmp3C9->f1;_tmp3CB=_tmp3C9->f2;}}
_LL245: {
# 1459
void*_tmp408=Cyc_Port_var();
# 1460
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp3CB),Cyc_Port_arith_ct());{
# 1461
void*_tmp409=Cyc_Port_gen_exp(env,_tmp3CA);
# 1462
Cyc_Port_leq(_tmp409,Cyc_Port_ptr_ct(_tmp408,Cyc_Port_var(),Cyc_Port_fat_ct(),
Cyc_Port_var(),Cyc_Port_var()));
# 1463
return _tmp408;};}_LL246: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp3CC=(
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp3CC->tag != 19)goto
_LL248;else{_tmp3CD=_tmp3CC->f1;}}_LL247: {
# 1465
void*_tmp40A=Cyc_Port_var();
# 1466
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp3CD),Cyc_Port_ptr_ct(_tmp40A,Cyc_Port_var(),
Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var()));
# 1467
return _tmp40A;}_LL248: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*
_tmp3CE=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp3CE->tag
!= 20)goto _LL24A;else{_tmp3CF=_tmp3CE->f1;_tmp3D0=_tmp3CE->f2;}}_LL249: {
# 1469
void*_tmp40B=Cyc_Port_var();
# 1470
void*_tmp40C=Cyc_Port_gen_exp(env,_tmp3CF);
# 1471
{struct Cyc_Port_Cfield*_tmp61F;struct Cyc_Port_Cfield*_tmp61E[1];Cyc_Port_leq(
Cyc_Port_gen_exp(env,_tmp3CF),Cyc_Port_unknown_aggr_ct(((_tmp61E[0]=((_tmp61F=
_cycalloc(sizeof(*_tmp61F)),((_tmp61F->qual=Cyc_Port_var(),((_tmp61F->f=_tmp3D0,((
_tmp61F->type=_tmp40B,_tmp61F)))))))),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp61E,sizeof(struct Cyc_Port_Cfield*),
1))))));}
# 1472
return _tmp40B;}_LL24A: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*
_tmp3D1=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp3D1->tag
!= 21)goto _LL24C;else{_tmp3D2=_tmp3D1->f1;_tmp3D3=_tmp3D1->f2;}}_LL24B: {
# 1474
void*_tmp40F=Cyc_Port_var();
# 1475
void*_tmp410=Cyc_Port_gen_exp(env,_tmp3D2);
# 1476
{struct Cyc_Port_Cfield*_tmp622;struct Cyc_Port_Cfield*_tmp621[1];Cyc_Port_leq(
Cyc_Port_gen_exp(env,_tmp3D2),Cyc_Port_ptr_ct(Cyc_Port_unknown_aggr_ct(((_tmp621[
0]=((_tmp622=_cycalloc(sizeof(*_tmp622)),((_tmp622->qual=Cyc_Port_var(),((
_tmp622->f=_tmp3D3,((_tmp622->type=_tmp40F,_tmp622)))))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp621,sizeof(struct Cyc_Port_Cfield*),
1))))),
# 1477
Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var()));}
# 1478
return _tmp40F;}_LL24C: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp3D4=(
struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp3D4->tag != 32)goto
_LL24E;else{_tmp3D5=_tmp3D4->f1;_tmp3D6=_tmp3D5.elt_type;_tmp3D7=_tmp3D5.num_elts;}}
_LL24D:
# 1480
 Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp3D7),Cyc_Port_arith_ct());{
# 1481
void*_tmp413=(unsigned int)_tmp3D6?Cyc_Port_type_to_ctype(env,*_tmp3D6): Cyc_Port_var();
# 1482
return Cyc_Port_ptr_ct(_tmp413,Cyc_Port_var(),Cyc_Port_fat_ct(),Cyc_Port_heap_ct(),
Cyc_Port_var());};_LL24E: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp3D8=(
struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp37F;if(_tmp3D8->tag != 33)goto
_LL250;else{_tmp3D9=_tmp3D8->f1;_tmp3DA=_tmp3D8->f2;}}_LL24F: {
# 1483
const char*_tmp625;void*_tmp624;(_tmp624=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp625="Swap_e",_tag_dyneither(
_tmp625,sizeof(char),7))),_tag_dyneither(_tmp624,sizeof(void*),0)));}_LL250: {
struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp3DB=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)
_tmp37F;if(_tmp3DB->tag != 15)goto _LL252;}_LL251: {
# 1484
const char*_tmp628;void*_tmp627;(_tmp627=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp628="New_e",_tag_dyneither(
_tmp628,sizeof(char),6))),_tag_dyneither(_tmp627,sizeof(void*),0)));}_LL252: {
struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp3DC=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)
_tmp37F;if(_tmp3DC->tag != 23)goto _LL254;}_LL253: {
# 1485
const char*_tmp62B;void*_tmp62A;(_tmp62A=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp62B="Tuple_e",_tag_dyneither(
_tmp62B,sizeof(char),8))),_tag_dyneither(_tmp62A,sizeof(void*),0)));}_LL254: {
struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp3DD=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)
_tmp37F;if(_tmp3DD->tag != 24)goto _LL256;}_LL255: {
# 1486
const char*_tmp62E;void*_tmp62D;(_tmp62D=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp62E="CompoundLit_e",
_tag_dyneither(_tmp62E,sizeof(char),14))),_tag_dyneither(_tmp62D,sizeof(void*),0)));}
_LL256: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3DE=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)
_tmp37F;if(_tmp3DE->tag != 25)goto _LL258;}_LL257: {
# 1487
const char*_tmp631;void*_tmp630;(_tmp630=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp631="Array_e",_tag_dyneither(
_tmp631,sizeof(char),8))),_tag_dyneither(_tmp630,sizeof(void*),0)));}_LL258: {
struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3DF=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)
_tmp37F;if(_tmp3DF->tag != 26)goto _LL25A;}_LL259: {
# 1488
const char*_tmp634;void*_tmp633;(_tmp633=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp634="Comprehension_e",
_tag_dyneither(_tmp634,sizeof(char),16))),_tag_dyneither(_tmp633,sizeof(void*),0)));}
_LL25A: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3E0=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)
_tmp37F;if(_tmp3E0->tag != 27)goto _LL25C;}_LL25B: {
# 1489
const char*_tmp637;void*_tmp636;(_tmp636=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp637="Aggregate_e",_tag_dyneither(
_tmp637,sizeof(char),12))),_tag_dyneither(_tmp636,sizeof(void*),0)));}_LL25C: {
struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp3E1=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)
_tmp37F;if(_tmp3E1->tag != 28)goto _LL25E;}_LL25D: {
# 1490
const char*_tmp63A;void*_tmp639;(_tmp639=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp63A="AnonStruct_e",
_tag_dyneither(_tmp63A,sizeof(char),13))),_tag_dyneither(_tmp639,sizeof(void*),0)));}
_LL25E: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp3E2=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)
_tmp37F;if(_tmp3E2->tag != 29)goto _LL260;}_LL25F: {
# 1491
const char*_tmp63D;void*_tmp63C;(_tmp63C=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp63D="Datatype_e",_tag_dyneither(
_tmp63D,sizeof(char),11))),_tag_dyneither(_tmp63C,sizeof(void*),0)));}_LL260: {
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp3E3=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)
_tmp37F;if(_tmp3E3->tag != 34)goto _LL262;}_LL261: {
# 1492
const char*_tmp640;void*_tmp63F;(_tmp63F=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp640="UnresolvedMem_e",
_tag_dyneither(_tmp640,sizeof(char),16))),_tag_dyneither(_tmp63F,sizeof(void*),0)));}
_LL262: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp3E4=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)
_tmp37F;if(_tmp3E4->tag != 35)goto _LL264;else{_tmp3E5=_tmp3E4->f1;}}_LL263: {
# 1493
const char*_tmp643;void*_tmp642;(_tmp642=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp643="StmtExp_e",_tag_dyneither(
_tmp643,sizeof(char),10))),_tag_dyneither(_tmp642,sizeof(void*),0)));}_LL264: {
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp3E6=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)
_tmp37F;if(_tmp3E6->tag != 37)goto _LL266;}_LL265: {
# 1494
const char*_tmp646;void*_tmp645;(_tmp645=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp646="Valueof_e",_tag_dyneither(
_tmp646,sizeof(char),10))),_tag_dyneither(_tmp645,sizeof(void*),0)));}_LL266: {
struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp3E7=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)
_tmp37F;if(_tmp3E7->tag != 38)goto _LL268;}_LL267: {
# 1495
const char*_tmp649;void*_tmp648;(_tmp648=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp649="Asm_e",_tag_dyneither(
_tmp649,sizeof(char),6))),_tag_dyneither(_tmp648,sizeof(void*),0)));}_LL268: {
struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp3E8=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)
_tmp37F;if(_tmp3E8->tag != 36)goto _LL209;}_LL269: {
# 1496
const char*_tmp64C;void*_tmp64B;(_tmp64B=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp64C="Tagcheck_e",_tag_dyneither(
_tmp64C,sizeof(char),11))),_tag_dyneither(_tmp64B,sizeof(void*),0)));}_LL209:;}
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1501
static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Stmt*s){
# 1502
void*_tmp430=s->r;struct Cyc_Absyn_Exp*_tmp433;struct Cyc_Absyn_Stmt*_tmp435;
struct Cyc_Absyn_Stmt*_tmp436;struct Cyc_Absyn_Exp*_tmp438;struct Cyc_Absyn_Exp*
_tmp43A;struct Cyc_Absyn_Stmt*_tmp43B;struct Cyc_Absyn_Stmt*_tmp43C;struct _tuple9
_tmp43E;struct Cyc_Absyn_Exp*_tmp43F;struct Cyc_Absyn_Stmt*_tmp440;struct Cyc_Absyn_Exp*
_tmp445;struct _tuple9 _tmp446;struct Cyc_Absyn_Exp*_tmp447;struct _tuple9 _tmp448;
struct Cyc_Absyn_Exp*_tmp449;struct Cyc_Absyn_Stmt*_tmp44A;struct Cyc_Absyn_Exp*
_tmp44C;struct Cyc_List_List*_tmp44D;struct Cyc_Absyn_Decl*_tmp450;struct Cyc_Absyn_Stmt*
_tmp451;struct Cyc_Absyn_Stmt*_tmp453;struct Cyc_Absyn_Stmt*_tmp455;struct _tuple9
_tmp456;struct Cyc_Absyn_Exp*_tmp457;_LL26B: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*
_tmp431=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp430;if(_tmp431->tag != 
0)goto _LL26D;}_LL26C:
# 1503
 return;_LL26D: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp432=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)
_tmp430;if(_tmp432->tag != 1)goto _LL26F;else{_tmp433=_tmp432->f1;}}_LL26E:
# 1504
 Cyc_Port_gen_exp(env,_tmp433);return;_LL26F: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*
_tmp434=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp430;if(_tmp434->tag != 
2)goto _LL271;else{_tmp435=_tmp434->f1;_tmp436=_tmp434->f2;}}_LL270:
# 1505
 Cyc_Port_gen_stmt(env,_tmp435);Cyc_Port_gen_stmt(env,_tmp436);return;_LL271: {
struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp437=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)
_tmp430;if(_tmp437->tag != 3)goto _LL273;else{_tmp438=_tmp437->f1;}}_LL272: {
# 1507
void*_tmp45A=Cyc_Port_lookup_return_type(env);
# 1508
void*_tmp45B=(unsigned int)_tmp438?Cyc_Port_gen_exp(env,(struct Cyc_Absyn_Exp*)
_tmp438): Cyc_Port_void_ct();
# 1509
Cyc_Port_leq(_tmp45B,_tmp45A);
# 1510
return;}_LL273: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp439=(
struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp430;if(_tmp439->tag != 4)
goto _LL275;else{_tmp43A=_tmp439->f1;_tmp43B=_tmp439->f2;_tmp43C=_tmp439->f3;}}
_LL274:
# 1512
 Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp43A),Cyc_Port_arith_ct());
# 1513
Cyc_Port_gen_stmt(env,_tmp43B);
# 1514
Cyc_Port_gen_stmt(env,_tmp43C);
# 1515
return;_LL275: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp43D=(struct
Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp430;if(_tmp43D->tag != 5)goto _LL277;
else{_tmp43E=_tmp43D->f1;_tmp43F=_tmp43E.f1;_tmp440=_tmp43D->f2;}}_LL276:
# 1517
 Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp43F),Cyc_Port_arith_ct());
# 1518
Cyc_Port_gen_stmt(env,_tmp440);
# 1519
return;_LL277: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp441=(struct
Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp430;if(_tmp441->tag != 6)goto _LL279;}
_LL278:
# 1520
 goto _LL27A;_LL279: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp442=(
struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp430;if(_tmp442->tag != 7)
goto _LL27B;}_LL27A:
# 1521
 goto _LL27C;_LL27B: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp443=(
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp430;if(_tmp443->tag != 8)goto
_LL27D;}_LL27C:
# 1522
 return;_LL27D: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp444=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)
_tmp430;if(_tmp444->tag != 9)goto _LL27F;else{_tmp445=_tmp444->f1;_tmp446=_tmp444->f2;
_tmp447=_tmp446.f1;_tmp448=_tmp444->f3;_tmp449=_tmp448.f1;_tmp44A=_tmp444->f4;}}
_LL27E:
# 1524
 Cyc_Port_gen_exp(env,_tmp445);
# 1525
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp447),Cyc_Port_arith_ct());
# 1526
Cyc_Port_gen_exp(env,_tmp449);
# 1527
Cyc_Port_gen_stmt(env,_tmp44A);
# 1528
return;_LL27F: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp44B=(struct
Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp430;if(_tmp44B->tag != 10)goto
_LL281;else{_tmp44C=_tmp44B->f1;_tmp44D=_tmp44B->f2;}}_LL280:
# 1530
 Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp44C),Cyc_Port_arith_ct());
# 1531
for(0;(unsigned int)_tmp44D;_tmp44D=_tmp44D->tl){
# 1533
Cyc_Port_gen_stmt(env,((struct Cyc_Absyn_Switch_clause*)_tmp44D->hd)->body);}
# 1535
return;_LL281: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp44E=(
struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp430;if(_tmp44E->tag != 11)
goto _LL283;}_LL282: {
# 1536
const char*_tmp64F;void*_tmp64E;(_tmp64E=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp64F="fallthru",_tag_dyneither(
_tmp64F,sizeof(char),9))),_tag_dyneither(_tmp64E,sizeof(void*),0)));}_LL283: {
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp44F=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)
_tmp430;if(_tmp44F->tag != 12)goto _LL285;else{_tmp450=_tmp44F->f1;_tmp451=_tmp44F->f2;}}
_LL284:
# 1537
 env=Cyc_Port_gen_localdecl(env,_tmp450);Cyc_Port_gen_stmt(env,_tmp451);return;
_LL285: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp452=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)
_tmp430;if(_tmp452->tag != 13)goto _LL287;else{_tmp453=_tmp452->f2;}}_LL286:
# 1538
 Cyc_Port_gen_stmt(env,_tmp453);return;_LL287: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*
_tmp454=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp430;if(_tmp454->tag != 
14)goto _LL289;else{_tmp455=_tmp454->f1;_tmp456=_tmp454->f2;_tmp457=_tmp456.f1;}}
_LL288:
# 1540
 Cyc_Port_gen_stmt(env,_tmp455);
# 1541
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp457),Cyc_Port_arith_ct());
# 1542
return;_LL289: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp458=(
struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp430;if(_tmp458->tag != 15)
goto _LL28B;}_LL28A: {
# 1543
const char*_tmp652;void*_tmp651;(_tmp651=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp652="try/catch",_tag_dyneither(
_tmp652,sizeof(char),10))),_tag_dyneither(_tmp651,sizeof(void*),0)));}_LL28B: {
struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp459=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)
_tmp430;if(_tmp459->tag != 16)goto _LL26A;}_LL28C: {
# 1544
const char*_tmp655;void*_tmp654;(_tmp654=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp655="reset region",
_tag_dyneither(_tmp655,sizeof(char),13))),_tag_dyneither(_tmp654,sizeof(void*),0)));}
_LL26A:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct _tuple15{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};
# 1549
static void*Cyc_Port_gen_initializer(struct Cyc_Port_Cenv*env,void*t,struct Cyc_Absyn_Exp*
e){
# 1550
void*_tmp462=e->r;struct Cyc_List_List*_tmp464;struct Cyc_List_List*_tmp466;
struct Cyc_List_List*_tmp468;union Cyc_Absyn_Cnst _tmp46A;struct _dyneither_ptr
_tmp46B;union Cyc_Absyn_Cnst _tmp46D;struct _dyneither_ptr _tmp46E;_LL28E: {struct
Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp463=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)
_tmp462;if(_tmp463->tag != 34)goto _LL290;else{_tmp464=_tmp463->f2;}}_LL28F:
# 1551
 _tmp466=_tmp464;goto _LL291;_LL290: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*
_tmp465=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp462;if(_tmp465->tag != 
25)goto _LL292;else{_tmp466=_tmp465->f1;}}_LL291:
# 1552
 _tmp468=_tmp466;goto _LL293;_LL292: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*
_tmp467=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp462;if(_tmp467->tag
!= 24)goto _LL294;else{_tmp468=_tmp467->f2;}}_LL293:
# 1554
 LOOP: {
# 1555
void*_tmp46F=t;struct _tuple0*_tmp471;struct Cyc_Absyn_ArrayInfo _tmp473;void*
_tmp474;union Cyc_Absyn_Constraint*_tmp475;unsigned int _tmp476;struct Cyc_Absyn_AggrInfo
_tmp478;union Cyc_Absyn_AggrInfoU _tmp479;struct _tuple2 _tmp47A;enum Cyc_Absyn_AggrKind
_tmp47B;struct _tuple0*_tmp47C;struct Cyc_Absyn_AggrInfo _tmp47E;union Cyc_Absyn_AggrInfoU
_tmp47F;struct Cyc_Absyn_Aggrdecl**_tmp480;struct Cyc_Absyn_Aggrdecl*_tmp481;
_LL29B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp470=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)
_tmp46F;if(_tmp470->tag != 17)goto _LL29D;else{_tmp471=_tmp470->f1;}}_LL29C:
# 1557
(*_tmp471).f1=Cyc_Absyn_Loc_n;
# 1558
t=Cyc_Port_lookup_typedef(env,_tmp471);goto LOOP;_LL29D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp472=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46F;if(_tmp472->tag != 
8)goto _LL29F;else{_tmp473=_tmp472->f1;_tmp474=_tmp473.elt_type;_tmp475=_tmp473.zero_term;
_tmp476=_tmp473.zt_loc;}}_LL29E: {
# 1560
void*_tmp482=Cyc_Port_var();
# 1561
void*_tmp483=Cyc_Port_var();
# 1562
void*_tmp484=Cyc_Port_var();
# 1563
void*_tmp485=Cyc_Port_type_to_ctype(env,_tmp474);
# 1564
for(0;_tmp468 != 0;_tmp468=_tmp468->tl){
# 1565
struct Cyc_List_List*_tmp487;struct Cyc_Absyn_Exp*_tmp488;struct _tuple15 _tmp486=*((
struct _tuple15*)_tmp468->hd);_tmp487=_tmp486.f1;_tmp488=_tmp486.f2;
# 1566
if((unsigned int)_tmp487){const char*_tmp658;void*_tmp657;(_tmp657=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp658="designators in initializers",
_tag_dyneither(_tmp658,sizeof(char),28))),_tag_dyneither(_tmp657,sizeof(void*),0)));}{
# 1567
void*_tmp48B=Cyc_Port_gen_initializer(env,_tmp474,_tmp488);
# 1568
Cyc_Port_leq(_tmp48B,_tmp482);};}
# 1570
return Cyc_Port_array_ct(_tmp482,_tmp483,_tmp484);}_LL29F: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp477=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp46F;if(_tmp477->tag != 
11)goto _LL2A1;else{_tmp478=_tmp477->f1;_tmp479=_tmp478.aggr_info;if((_tmp479.UnknownAggr).tag
!= 1)goto _LL2A1;_tmp47A=(struct _tuple2)(_tmp479.UnknownAggr).val;_tmp47B=_tmp47A.f1;
if(_tmp47B != Cyc_Absyn_StructA)goto _LL2A1;_tmp47C=_tmp47A.f2;}}_LL2A0:
# 1572
(*_tmp47C).f1=Cyc_Absyn_Loc_n;{
# 1573
struct Cyc_Absyn_Aggrdecl*_tmp48D;struct _tuple11 _tmp48C=*Cyc_Port_lookup_struct_decl(
env,_tmp47C);_tmp48D=_tmp48C.f1;
# 1574
if((struct Cyc_Absyn_Aggrdecl*)_tmp48D == 0){const char*_tmp65B;void*_tmp65A;(
_tmp65A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp65B="struct is not yet defined",_tag_dyneither(_tmp65B,sizeof(char),26))),
_tag_dyneither(_tmp65A,sizeof(void*),0)));}
# 1575
_tmp481=_tmp48D;goto _LL2A2;};_LL2A1: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp47D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp46F;if(_tmp47D->tag != 
11)goto _LL2A3;else{_tmp47E=_tmp47D->f1;_tmp47F=_tmp47E.aggr_info;if((_tmp47F.KnownAggr).tag
!= 2)goto _LL2A3;_tmp480=(struct Cyc_Absyn_Aggrdecl**)(_tmp47F.KnownAggr).val;
_tmp481=*_tmp480;}}_LL2A2: {
# 1577
struct Cyc_List_List*_tmp490=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp481->impl))->fields;
# 1578
for(0;_tmp468 != 0;_tmp468=_tmp468->tl){
# 1579
struct Cyc_List_List*_tmp492;struct Cyc_Absyn_Exp*_tmp493;struct _tuple15 _tmp491=*((
struct _tuple15*)_tmp468->hd);_tmp492=_tmp491.f1;_tmp493=_tmp491.f2;
# 1580
if((unsigned int)_tmp492){const char*_tmp65E;void*_tmp65D;(_tmp65D=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp65E="designators in initializers",
_tag_dyneither(_tmp65E,sizeof(char),28))),_tag_dyneither(_tmp65D,sizeof(void*),0)));}{
# 1581
struct Cyc_Absyn_Aggrfield*_tmp496=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)
_check_null(_tmp490))->hd;
# 1582
_tmp490=_tmp490->tl;{
# 1583
void*_tmp497=Cyc_Port_gen_initializer(env,_tmp496->type,_tmp493);;};};}
# 1585
return Cyc_Port_type_to_ctype(env,t);}_LL2A3:;_LL2A4: {
# 1586
const char*_tmp661;void*_tmp660;(_tmp660=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp661="bad type for aggregate initializer",
_tag_dyneither(_tmp661,sizeof(char),35))),_tag_dyneither(_tmp660,sizeof(void*),0)));}
_LL29A:;}_LL294: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp469=(struct
Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp462;if(_tmp469->tag != 0)goto _LL296;
else{_tmp46A=_tmp469->f1;if((_tmp46A.String_c).tag != 8)goto _LL296;_tmp46B=(
struct _dyneither_ptr)(_tmp46A.String_c).val;}}_LL295:
# 1588
 goto _LL297;_LL296: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp46C=(
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp462;if(_tmp46C->tag != 0)goto
_LL298;else{_tmp46D=_tmp46C->f1;if((_tmp46D.Wstring_c).tag != 9)goto _LL298;
_tmp46E=(struct _dyneither_ptr)(_tmp46D.Wstring_c).val;}}_LL297:
# 1590
 LOOP2: {
# 1591
void*_tmp49A=t;struct _tuple0*_tmp49C;_LL2A6: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*
_tmp49B=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp49A;if(_tmp49B->tag
!= 17)goto _LL2A8;else{_tmp49C=_tmp49B->f1;}}_LL2A7:
# 1593
(*_tmp49C).f1=Cyc_Absyn_Loc_n;
# 1594
t=Cyc_Port_lookup_typedef(env,_tmp49C);goto LOOP2;_LL2A8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*
_tmp49D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp49A;if(_tmp49D->tag != 
8)goto _LL2AA;}_LL2A9:
# 1595
 return Cyc_Port_array_ct(Cyc_Port_arith_ct(),Cyc_Port_const_ct(),Cyc_Port_zterm_ct());
_LL2AA:;_LL2AB:
# 1596
 return Cyc_Port_gen_exp(env,e);_LL2A5:;}_LL298:;_LL299:
# 1598
 return Cyc_Port_gen_exp(env,e);_LL28D:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1603
static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*env,struct
Cyc_Absyn_Decl*d){
# 1604
void*_tmp49E=d->r;struct Cyc_Absyn_Vardecl*_tmp4A0;_LL2AD: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*
_tmp49F=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp49E;if(_tmp49F->tag != 
0)goto _LL2AF;else{_tmp4A0=_tmp49F->f1;}}_LL2AE: {
# 1606
void*_tmp4A1=Cyc_Port_var();
# 1607
void*q;
# 1608
if((env->gcenv)->porting){
# 1609
q=Cyc_Port_var();
# 1610
Cyc_Port_register_const_cvar(env,q,
# 1611
(_tmp4A0->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),(_tmp4A0->tq).loc);}
else{
# 1619
q=(_tmp4A0->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
# 1621
(*_tmp4A0->name).f1=Cyc_Absyn_Loc_n;
# 1622
env=Cyc_Port_add_var(env,_tmp4A0->name,_tmp4A0->type,q,_tmp4A1);
# 1623
Cyc_Port_unifies(_tmp4A1,Cyc_Port_type_to_ctype(env,_tmp4A0->type));
# 1624
if((unsigned int)_tmp4A0->initializer){
# 1625
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp4A0->initializer);
# 1626
void*t2=Cyc_Port_gen_initializer(env,_tmp4A0->type,e);
# 1627
Cyc_Port_leq(t2,_tmp4A1);}
# 1629
return env;}_LL2AF:;_LL2B0: {
# 1630
const char*_tmp664;void*_tmp663;(_tmp663=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp664="non-local decl that isn't a variable",
_tag_dyneither(_tmp664,sizeof(char),37))),_tag_dyneither(_tmp663,sizeof(void*),0)));}
_LL2AC:;}
# 1634
static struct _tuple8*Cyc_Port_make_targ(struct _tuple8*arg){
# 1635
struct _dyneither_ptr*_tmp4A5;struct Cyc_Absyn_Tqual _tmp4A6;void*_tmp4A7;struct
_tuple8 _tmp4A4=*arg;_tmp4A5=_tmp4A4.f1;_tmp4A6=_tmp4A4.f2;_tmp4A7=_tmp4A4.f3;{
# 1636
struct _tuple8*_tmp665;return(_tmp665=_cycalloc(sizeof(*_tmp665)),((_tmp665->f1=
0,((_tmp665->f2=_tmp4A6,((_tmp665->f3=_tmp4A7,_tmp665)))))));};}struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;
# 1639
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*env,struct
Cyc_List_List*ds);struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;struct _tuple16{struct _dyneither_ptr*f1;void*f2;void*f3;void*
f4;};struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;
# 1641
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecl(struct Cyc_Port_Cenv*env,struct
Cyc_Absyn_Decl*d){
# 1642
void*_tmp4A9=d->r;struct Cyc_Absyn_Vardecl*_tmp4AD;struct Cyc_Absyn_Fndecl*
_tmp4AF;struct Cyc_Absyn_Typedefdecl*_tmp4B1;struct Cyc_Absyn_Aggrdecl*_tmp4B3;
struct Cyc_Absyn_Enumdecl*_tmp4B5;_LL2B2: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*
_tmp4AA=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp4A9;if(_tmp4AA->tag
!= 14)goto _LL2B4;}_LL2B3:
# 1644
(env->gcenv)->porting=1;
# 1645
return env;_LL2B4: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp4AB=(
struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp4A9;if(_tmp4AB->tag != 15)
goto _LL2B6;}_LL2B5:
# 1647
(env->gcenv)->porting=0;
# 1648
return env;_LL2B6: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp4AC=(struct
Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp4A9;if(_tmp4AC->tag != 0)goto _LL2B8;
else{_tmp4AD=_tmp4AC->f1;}}_LL2B7:
# 1650
(*_tmp4AD->name).f1=Cyc_Absyn_Loc_n;
# 1654
if(Cyc_Port_declared_var(env,_tmp4AD->name)){
# 1655
void*_tmp4B7;void*_tmp4B8;struct _tuple10 _tmp4B6=Cyc_Port_lookup_var(env,_tmp4AD->name);
_tmp4B7=_tmp4B6.f1;_tmp4B8=_tmp4B6.f2;{
# 1656
void*q2;
# 1657
if((env->gcenv)->porting  && !Cyc_Port_isfn(env,_tmp4AD->name)){
# 1658
q2=Cyc_Port_var();
# 1659
Cyc_Port_register_const_cvar(env,q2,
# 1660
(_tmp4AD->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),(_tmp4AD->tq).loc);}
else{
# 1668
q2=(_tmp4AD->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
# 1670
Cyc_Port_unifies(_tmp4B7,q2);
# 1671
Cyc_Port_unifies(_tmp4B8,Cyc_Port_type_to_ctype(env,_tmp4AD->type));
# 1672
if((unsigned int)_tmp4AD->initializer){
# 1673
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp4AD->initializer);
# 1674
Cyc_Port_leq(Cyc_Port_gen_initializer(env,_tmp4AD->type,e),_tmp4B8);}
# 1676
return env;};}else{
# 1678
return Cyc_Port_gen_localdecl(env,d);}_LL2B8: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*
_tmp4AE=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp4A9;if(_tmp4AE->tag != 1)
goto _LL2BA;else{_tmp4AF=_tmp4AE->f1;}}_LL2B9:
# 1684
(*_tmp4AF->name).f1=Cyc_Absyn_Loc_n;{
# 1685
struct _tuple13*predeclared=0;
# 1687
if(Cyc_Port_declared_var(env,_tmp4AF->name))
# 1688
predeclared=(struct _tuple13*)Cyc_Port_lookup_full_var(env,_tmp4AF->name);{
# 1690
void*_tmp4B9=_tmp4AF->ret_type;
# 1691
struct Cyc_List_List*_tmp4BA=_tmp4AF->args;
# 1692
struct Cyc_List_List*_tmp4BB=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct
_tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Port_make_targ,_tmp4BA);
# 1693
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp66B;struct Cyc_Absyn_FnInfo _tmp66A;
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp669;struct Cyc_Absyn_FnType_Absyn_Type_struct*
_tmp4BC=
# 1694
(_tmp669=_cycalloc(sizeof(*_tmp669)),((_tmp669[0]=((_tmp66B.tag=9,((_tmp66B.f1=((
_tmp66A.tvars=0,((_tmp66A.effect=0,((_tmp66A.ret_tqual=Cyc_Absyn_empty_tqual(0),((
_tmp66A.ret_typ=_tmp4B9,((_tmp66A.args=_tmp4BB,((_tmp66A.c_varargs=0,((_tmp66A.cyc_varargs=
0,((_tmp66A.rgn_po=0,((_tmp66A.attributes=0,_tmp66A)))))))))))))))))),_tmp66B)))),
_tmp669)));
# 1697
struct Cyc_Port_Cenv*_tmp4BD=Cyc_Port_set_cpos(env,Cyc_Port_FnBody_pos);
# 1698
void*_tmp4BE=Cyc_Port_type_to_ctype(_tmp4BD,_tmp4B9);
# 1699
struct Cyc_List_List*c_args=0;
# 1700
struct Cyc_List_List*c_arg_types=0;
# 1701
{struct Cyc_List_List*_tmp4BF=_tmp4BA;for(0;(unsigned int)_tmp4BF;_tmp4BF=
_tmp4BF->tl){
# 1702
struct _dyneither_ptr*_tmp4C1;struct Cyc_Absyn_Tqual _tmp4C2;void*_tmp4C3;struct
_tuple8 _tmp4C0=*((struct _tuple8*)_tmp4BF->hd);_tmp4C1=_tmp4C0.f1;_tmp4C2=_tmp4C0.f2;
_tmp4C3=_tmp4C0.f3;{
# 1705
void*_tmp4C4=Cyc_Port_type_to_ctype(_tmp4BD,_tmp4C3);
# 1706
void*tqv;
# 1707
if((env->gcenv)->porting){
# 1708
tqv=Cyc_Port_var();
# 1709
Cyc_Port_register_const_cvar(env,tqv,_tmp4C2.print_const?Cyc_Port_const_ct():
Cyc_Port_notconst_ct(),_tmp4C2.loc);}else{
# 1717
tqv=_tmp4C2.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
# 1719
{struct _tuple16*_tmp66E;struct Cyc_List_List*_tmp66D;c_args=((_tmp66D=_cycalloc(
sizeof(*_tmp66D)),((_tmp66D->hd=((_tmp66E=_cycalloc(sizeof(*_tmp66E)),((_tmp66E->f1=
_tmp4C1,((_tmp66E->f2=_tmp4C3,((_tmp66E->f3=tqv,((_tmp66E->f4=_tmp4C4,_tmp66E)))))))))),((
_tmp66D->tl=c_args,_tmp66D))))));}{
# 1720
struct Cyc_List_List*_tmp66F;c_arg_types=((_tmp66F=_cycalloc(sizeof(*_tmp66F)),((
_tmp66F->hd=_tmp4C4,((_tmp66F->tl=c_arg_types,_tmp66F))))));};};}}
# 1722
c_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
c_args);
# 1723
c_arg_types=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
c_arg_types);{
# 1724
void*_tmp4C8=Cyc_Port_fn_ct(_tmp4BE,c_arg_types);
# 1728
(*_tmp4AF->name).f1=Cyc_Absyn_Loc_n;
# 1729
_tmp4BD=Cyc_Port_add_var(_tmp4BD,_tmp4AF->name,(void*)_tmp4BC,Cyc_Port_const_ct(),
_tmp4C8);
# 1730
Cyc_Port_add_return_type(_tmp4BD,_tmp4BE);
# 1731
{struct Cyc_List_List*_tmp4C9=c_args;for(0;(unsigned int)_tmp4C9;_tmp4C9=_tmp4C9->tl){
# 1732
struct _dyneither_ptr*_tmp4CB;void*_tmp4CC;void*_tmp4CD;void*_tmp4CE;struct
_tuple16 _tmp4CA=*((struct _tuple16*)_tmp4C9->hd);_tmp4CB=_tmp4CA.f1;_tmp4CC=
_tmp4CA.f2;_tmp4CD=_tmp4CA.f3;_tmp4CE=_tmp4CA.f4;{
# 1733
struct _tuple0*_tmp670;_tmp4BD=Cyc_Port_add_var(_tmp4BD,((_tmp670=_cycalloc(
sizeof(*_tmp670)),((_tmp670->f1=Cyc_Absyn_Loc_n,((_tmp670->f2=_tmp4CB,_tmp670)))))),
_tmp4CC,_tmp4CD,_tmp4CE);};}}
# 1735
Cyc_Port_gen_stmt(_tmp4BD,_tmp4AF->body);
# 1740
Cyc_Port_generalize(0,_tmp4C8);{
# 1747
struct Cyc_Dict_Dict counts=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct
_dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);
# 1748
Cyc_Port_region_counts(& counts,_tmp4C8);
# 1751
Cyc_Port_register_rgns(env,counts,1,(void*)_tmp4BC,_tmp4C8);
# 1752
env=Cyc_Port_add_var(_tmp4BD,_tmp4AF->name,(void*)_tmp4BC,Cyc_Port_const_ct(),
_tmp4C8);
# 1753
if((unsigned int)predeclared){
# 1760
void*_tmp4D1;struct _tuple10*_tmp4D2;struct _tuple10 _tmp4D3;void*_tmp4D4;void*
_tmp4D5;struct _tuple13 _tmp4D0=*predeclared;_tmp4D1=_tmp4D0.f1;_tmp4D2=_tmp4D0.f2;
_tmp4D3=*_tmp4D2;_tmp4D4=_tmp4D3.f1;_tmp4D5=_tmp4D3.f2;
# 1761
Cyc_Port_unifies(_tmp4D4,Cyc_Port_const_ct());
# 1762
Cyc_Port_unifies(_tmp4D5,Cyc_Port_instantiate(_tmp4C8));
# 1764
Cyc_Port_register_rgns(env,counts,1,_tmp4D1,_tmp4C8);}
# 1766
return env;};};};};_LL2BA: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*
_tmp4B0=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp4A9;if(_tmp4B0->tag
!= 9)goto _LL2BC;else{_tmp4B1=_tmp4B0->f1;}}_LL2BB: {
# 1772
void*_tmp4D9=(void*)_check_null(_tmp4B1->defn);
# 1773
void*_tmp4DA=Cyc_Port_type_to_ctype(env,_tmp4D9);
# 1774
(*_tmp4B1->name).f1=Cyc_Absyn_Loc_n;
# 1775
Cyc_Port_add_typedef(env,_tmp4B1->name,_tmp4D9,_tmp4DA);
# 1776
return env;}_LL2BC: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp4B2=(
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp4A9;if(_tmp4B2->tag != 6)goto
_LL2BE;else{_tmp4B3=_tmp4B2->f1;}}_LL2BD: {
# 1782
struct _tuple0*_tmp4DB=_tmp4B3->name;
# 1783
(*_tmp4B3->name).f1=Cyc_Absyn_Loc_n;{
# 1784
struct _tuple11*previous;
# 1785
struct Cyc_List_List*curr=0;
# 1786
switch(_tmp4B3->kind){case Cyc_Absyn_StructA: _LL2C2:
# 1788
 previous=Cyc_Port_lookup_struct_decl(env,_tmp4DB);
# 1789
break;case Cyc_Absyn_UnionA: _LL2C3:
# 1791
 previous=Cyc_Port_lookup_union_decl(env,_tmp4DB);
# 1792
break;}
# 1794
if((unsigned int)_tmp4B3->impl){
# 1795
struct Cyc_List_List*cf=(*previous).f2;
# 1796
{struct Cyc_List_List*_tmp4DC=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp4B3->impl))->fields;for(0;(unsigned int)_tmp4DC;_tmp4DC=_tmp4DC->tl){
# 1797
struct Cyc_Absyn_Aggrfield*_tmp4DD=(struct Cyc_Absyn_Aggrfield*)_tmp4DC->hd;
# 1798
void*qv;
# 1799
if((env->gcenv)->porting){
# 1800
qv=Cyc_Port_var();
# 1801
Cyc_Port_register_const_cvar(env,qv,
# 1802
(_tmp4DD->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),(_tmp4DD->tq).loc);}
else{
# 1810
qv=(_tmp4DD->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}{
# 1812
void*_tmp4DE=Cyc_Port_type_to_ctype(env,_tmp4DD->type);
# 1813
if(cf != 0){
# 1814
void*_tmp4E0;void*_tmp4E1;struct Cyc_Port_Cfield _tmp4DF=*((struct Cyc_Port_Cfield*)
cf->hd);_tmp4E0=_tmp4DF.qual;_tmp4E1=_tmp4DF.type;
# 1815
cf=cf->tl;
# 1816
Cyc_Port_unifies(qv,_tmp4E0);
# 1817
Cyc_Port_unifies(_tmp4DE,_tmp4E1);}{
# 1819
struct Cyc_Port_Cfield*_tmp673;struct Cyc_List_List*_tmp672;curr=((_tmp672=
_cycalloc(sizeof(*_tmp672)),((_tmp672->hd=((_tmp673=_cycalloc(sizeof(*_tmp673)),((
_tmp673->qual=qv,((_tmp673->f=_tmp4DD->name,((_tmp673->type=_tmp4DE,_tmp673)))))))),((
_tmp672->tl=curr,_tmp672))))));};};}}
# 1821
curr=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(curr);
# 1822
if((*previous).f2 == 0)
# 1823
(*previous).f2=curr;}
# 1826
return env;};}_LL2BE: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp4B4=(
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp4A9;if(_tmp4B4->tag != 8)goto
_LL2C0;else{_tmp4B5=_tmp4B4->f1;}}_LL2BF:
# 1831
(*_tmp4B5->name).f1=Cyc_Absyn_Loc_n;
# 1833
if((unsigned int)_tmp4B5->fields){
# 1834
struct Cyc_List_List*_tmp4E4=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp4B5->fields))->v;for(0;(unsigned int)_tmp4E4;_tmp4E4=_tmp4E4->tl){
# 1835
(*((struct Cyc_Absyn_Enumfield*)_tmp4E4->hd)->name).f1=Cyc_Absyn_Loc_n;{
# 1836
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp676;struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmp675;env=Cyc_Port_add_var(env,((struct Cyc_Absyn_Enumfield*)_tmp4E4->hd)->name,(
void*)((_tmp675=_cycalloc(sizeof(*_tmp675)),((_tmp675[0]=((_tmp676.tag=13,((
_tmp676.f1=_tmp4B5->name,((_tmp676.f2=(struct Cyc_Absyn_Enumdecl*)_tmp4B5,_tmp676)))))),
_tmp675)))),
# 1837
Cyc_Port_const_ct(),Cyc_Port_arith_ct());};}}
# 1839
return env;_LL2C0:;_LL2C1:
# 1841
 if((env->gcenv)->porting){
# 1842
const char*_tmp679;void*_tmp678;(_tmp678=0,Cyc_fprintf(Cyc_stderr,((_tmp679="Warning: Cyclone declaration found in code to be ported -- skipping.",
_tag_dyneither(_tmp679,sizeof(char),69))),_tag_dyneither(_tmp678,sizeof(void*),0)));}
# 1843
return env;_LL2B1:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
struct Cyc_Port_Cenv;
# 1848
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*env,struct
Cyc_List_List*ds){
# 1849
for(0;(unsigned int)ds;ds=ds->tl){
# 1850
env=Cyc_Port_gen_topdecl(env,(struct Cyc_Absyn_Decl*)ds->hd);}
# 1851
return env;}struct Cyc_Port_Cenv;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct
Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct
Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct
Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct
Cyc_Port_Edit;
# 1855
static struct Cyc_List_List*Cyc_Port_gen_edits(struct Cyc_List_List*ds){
# 1857
struct Cyc_Port_Cenv*env=Cyc_Port_gen_topdecls(Cyc_Port_initial_cenv(),ds);
# 1859
struct Cyc_List_List*_tmp4E9=(env->gcenv)->pointer_edits;
# 1860
struct Cyc_List_List*_tmp4EA=(env->gcenv)->qualifier_edits;
# 1861
struct Cyc_List_List*_tmp4EB=(env->gcenv)->zeroterm_edits;
# 1862
struct Cyc_List_List*_tmp4EC=(env->gcenv)->edits;
# 1864
{struct Cyc_List_List*_tmp4ED=_tmp4E9;for(0;(unsigned int)_tmp4ED;_tmp4ED=
_tmp4ED->tl){
# 1865
void*_tmp4EF;void*_tmp4F0;unsigned int _tmp4F1;struct _tuple14 _tmp4EE=*((struct
_tuple14*)_tmp4ED->hd);_tmp4EF=_tmp4EE.f1;_tmp4F0=_tmp4EE.f2;_tmp4F1=_tmp4EE.f3;
# 1866
Cyc_Port_unifies(_tmp4EF,_tmp4F0);}}
# 1868
{struct Cyc_List_List*_tmp4F2=_tmp4EA;for(0;(unsigned int)_tmp4F2;_tmp4F2=
_tmp4F2->tl){
# 1869
void*_tmp4F4;void*_tmp4F5;unsigned int _tmp4F6;struct _tuple14 _tmp4F3=*((struct
_tuple14*)_tmp4F2->hd);_tmp4F4=_tmp4F3.f1;_tmp4F5=_tmp4F3.f2;_tmp4F6=_tmp4F3.f3;
# 1870
Cyc_Port_unifies(_tmp4F4,_tmp4F5);}}
# 1872
{struct Cyc_List_List*_tmp4F7=_tmp4EB;for(0;(unsigned int)_tmp4F7;_tmp4F7=
_tmp4F7->tl){
# 1873
void*_tmp4F9;void*_tmp4FA;unsigned int _tmp4FB;struct _tuple14 _tmp4F8=*((struct
_tuple14*)_tmp4F7->hd);_tmp4F9=_tmp4F8.f1;_tmp4FA=_tmp4F8.f2;_tmp4FB=_tmp4F8.f3;
# 1874
Cyc_Port_unifies(_tmp4F9,_tmp4FA);}}
# 1878
for(0;(unsigned int)_tmp4E9;_tmp4E9=_tmp4E9->tl){
# 1879
void*_tmp4FD;void*_tmp4FE;unsigned int _tmp4FF;struct _tuple14 _tmp4FC=*((struct
_tuple14*)_tmp4E9->hd);_tmp4FD=_tmp4FC.f1;_tmp4FE=_tmp4FC.f2;_tmp4FF=_tmp4FC.f3;
# 1880
if(!Cyc_Port_unifies(_tmp4FD,_tmp4FE) && (int)_tmp4FF){
# 1881
void*_tmp500=Cyc_Port_compress_ct(_tmp4FE);_LL2C6: {struct Cyc_Port_Thin_ct_Port_Ctype_struct*
_tmp501=(struct Cyc_Port_Thin_ct_Port_Ctype_struct*)_tmp500;if(_tmp501->tag != 2)
goto _LL2C8;}_LL2C7:
# 1883
{struct Cyc_Port_Edit*_tmp682;const char*_tmp681;const char*_tmp680;struct Cyc_List_List*
_tmp67F;_tmp4EC=((_tmp67F=_cycalloc(sizeof(*_tmp67F)),((_tmp67F->hd=((_tmp682=
_cycalloc(sizeof(*_tmp682)),((_tmp682->loc=_tmp4FF,((_tmp682->old_string=((
_tmp681="?",_tag_dyneither(_tmp681,sizeof(char),2))),((_tmp682->new_string=((
_tmp680="*",_tag_dyneither(_tmp680,sizeof(char),2))),_tmp682)))))))),((_tmp67F->tl=
_tmp4EC,_tmp67F))))));}
# 1884
goto _LL2C5;_LL2C8: {struct Cyc_Port_Fat_ct_Port_Ctype_struct*_tmp502=(struct Cyc_Port_Fat_ct_Port_Ctype_struct*)
_tmp500;if(_tmp502->tag != 3)goto _LL2CA;}_LL2C9:
# 1886
{struct Cyc_Port_Edit*_tmp68B;const char*_tmp68A;const char*_tmp689;struct Cyc_List_List*
_tmp688;_tmp4EC=((_tmp688=_cycalloc(sizeof(*_tmp688)),((_tmp688->hd=((_tmp68B=
_cycalloc(sizeof(*_tmp68B)),((_tmp68B->loc=_tmp4FF,((_tmp68B->old_string=((
_tmp68A="*",_tag_dyneither(_tmp68A,sizeof(char),2))),((_tmp68B->new_string=((
_tmp689="?",_tag_dyneither(_tmp689,sizeof(char),2))),_tmp68B)))))))),((_tmp688->tl=
_tmp4EC,_tmp688))))));}
# 1887
goto _LL2C5;_LL2CA:;_LL2CB:
# 1888
 goto _LL2C5;_LL2C5:;}}
# 1893
for(0;(unsigned int)_tmp4EA;_tmp4EA=_tmp4EA->tl){
# 1894
void*_tmp50C;void*_tmp50D;unsigned int _tmp50E;struct _tuple14 _tmp50B=*((struct
_tuple14*)_tmp4EA->hd);_tmp50C=_tmp50B.f1;_tmp50D=_tmp50B.f2;_tmp50E=_tmp50B.f3;
# 1895
if(!Cyc_Port_unifies(_tmp50C,_tmp50D) && (int)_tmp50E){
# 1896
void*_tmp50F=Cyc_Port_compress_ct(_tmp50D);_LL2CD: {struct Cyc_Port_Notconst_ct_Port_Ctype_struct*
_tmp510=(struct Cyc_Port_Notconst_ct_Port_Ctype_struct*)_tmp50F;if(_tmp510->tag != 
1)goto _LL2CF;}_LL2CE:
# 1898
{struct Cyc_Port_Edit*_tmp694;const char*_tmp693;const char*_tmp692;struct Cyc_List_List*
_tmp691;_tmp4EC=((_tmp691=_cycalloc(sizeof(*_tmp691)),((_tmp691->hd=((_tmp694=
_cycalloc(sizeof(*_tmp694)),((_tmp694->loc=_tmp50E,((_tmp694->old_string=((
_tmp693="const ",_tag_dyneither(_tmp693,sizeof(char),7))),((_tmp694->new_string=((
_tmp692="",_tag_dyneither(_tmp692,sizeof(char),1))),_tmp694)))))))),((_tmp691->tl=
_tmp4EC,_tmp691))))));}goto _LL2CC;_LL2CF: {struct Cyc_Port_Const_ct_Port_Ctype_struct*
_tmp511=(struct Cyc_Port_Const_ct_Port_Ctype_struct*)_tmp50F;if(_tmp511->tag != 0)
goto _LL2D1;}_LL2D0:
# 1900
{struct Cyc_Port_Edit*_tmp69D;const char*_tmp69C;const char*_tmp69B;struct Cyc_List_List*
_tmp69A;_tmp4EC=((_tmp69A=_cycalloc(sizeof(*_tmp69A)),((_tmp69A->hd=((_tmp69D=
_cycalloc(sizeof(*_tmp69D)),((_tmp69D->loc=_tmp50E,((_tmp69D->old_string=((
_tmp69C="",_tag_dyneither(_tmp69C,sizeof(char),1))),((_tmp69D->new_string=((
_tmp69B="const ",_tag_dyneither(_tmp69B,sizeof(char),7))),_tmp69D)))))))),((
_tmp69A->tl=_tmp4EC,_tmp69A))))));}goto _LL2CC;_LL2D1:;_LL2D2:
# 1901
 goto _LL2CC;_LL2CC:;}}
# 1906
for(0;(unsigned int)_tmp4EB;_tmp4EB=_tmp4EB->tl){
# 1907
void*_tmp51B;void*_tmp51C;unsigned int _tmp51D;struct _tuple14 _tmp51A=*((struct
_tuple14*)_tmp4EB->hd);_tmp51B=_tmp51A.f1;_tmp51C=_tmp51A.f2;_tmp51D=_tmp51A.f3;
# 1908
if(!Cyc_Port_unifies(_tmp51B,_tmp51C) && (int)_tmp51D){
# 1909
void*_tmp51E=Cyc_Port_compress_ct(_tmp51C);_LL2D4: {struct Cyc_Port_Zterm_ct_Port_Ctype_struct*
_tmp51F=(struct Cyc_Port_Zterm_ct_Port_Ctype_struct*)_tmp51E;if(_tmp51F->tag != 8)
goto _LL2D6;}_LL2D5:
# 1911
{struct Cyc_Port_Edit*_tmp6A6;const char*_tmp6A5;const char*_tmp6A4;struct Cyc_List_List*
_tmp6A3;_tmp4EC=((_tmp6A3=_cycalloc(sizeof(*_tmp6A3)),((_tmp6A3->hd=((_tmp6A6=
_cycalloc(sizeof(*_tmp6A6)),((_tmp6A6->loc=_tmp51D,((_tmp6A6->old_string=((
_tmp6A5="NOZEROTERM ",_tag_dyneither(_tmp6A5,sizeof(char),12))),((_tmp6A6->new_string=((
_tmp6A4="",_tag_dyneither(_tmp6A4,sizeof(char),1))),_tmp6A6)))))))),((_tmp6A3->tl=
_tmp4EC,_tmp6A3))))));}goto _LL2D3;_LL2D6: {struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*
_tmp520=(struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*)_tmp51E;if(_tmp520->tag != 
9)goto _LL2D8;}_LL2D7:
# 1913
{struct Cyc_Port_Edit*_tmp6AF;const char*_tmp6AE;const char*_tmp6AD;struct Cyc_List_List*
_tmp6AC;_tmp4EC=((_tmp6AC=_cycalloc(sizeof(*_tmp6AC)),((_tmp6AC->hd=((_tmp6AF=
_cycalloc(sizeof(*_tmp6AF)),((_tmp6AF->loc=_tmp51D,((_tmp6AF->old_string=((
_tmp6AE="ZEROTERM ",_tag_dyneither(_tmp6AE,sizeof(char),10))),((_tmp6AF->new_string=((
_tmp6AD="",_tag_dyneither(_tmp6AD,sizeof(char),1))),_tmp6AF)))))))),((_tmp6AC->tl=
_tmp4EC,_tmp6AC))))));}goto _LL2D3;_LL2D8:;_LL2D9:
# 1914
 goto _LL2D3;_LL2D3:;}}
# 1920
_tmp4EC=((struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_Port_Edit*,struct Cyc_Port_Edit*),
struct Cyc_List_List*x))Cyc_List_merge_sort)(Cyc_Port_cmp_edit,_tmp4EC);
# 1922
while((unsigned int)_tmp4EC  && ((struct Cyc_Port_Edit*)_tmp4EC->hd)->loc == 0){
# 1926
_tmp4EC=_tmp4EC->tl;}
# 1928
return _tmp4EC;}struct Cyc_Port_Edit;struct Cyc_Port_Edit;
# 1931
static unsigned int Cyc_Port_get_seg(struct Cyc_Port_Edit*e){
# 1932
return e->loc;}struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;
# 1937
void Cyc_Port_port(struct Cyc_List_List*ds){
# 1938
struct Cyc_List_List*_tmp529=Cyc_Port_gen_edits(ds);
# 1939
struct Cyc_List_List*_tmp52A=((struct Cyc_List_List*(*)(unsigned int(*f)(struct
Cyc_Port_Edit*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Port_get_seg,_tmp529);
# 1940
Cyc_Position_use_gcc_style_location=0;{
# 1941
struct Cyc_List_List*_tmp52B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_Position_strings_of_segments(_tmp52A));
# 1942
for(0;(unsigned int)_tmp529;(_tmp529=_tmp529->tl,_tmp52B=_tmp52B->tl)){
# 1943
unsigned int _tmp52D;struct _dyneither_ptr _tmp52E;struct _dyneither_ptr _tmp52F;
struct Cyc_Port_Edit _tmp52C=*((struct Cyc_Port_Edit*)_tmp529->hd);_tmp52D=_tmp52C.loc;
_tmp52E=_tmp52C.old_string;_tmp52F=_tmp52C.new_string;{
# 1944
struct _dyneither_ptr sloc=(struct _dyneither_ptr)*((struct _dyneither_ptr*)((
struct Cyc_List_List*)_check_null(_tmp52B))->hd);
# 1945
const char*_tmp6B5;void*_tmp6B4[3];struct Cyc_String_pa_PrintArg_struct _tmp6B3;
struct Cyc_String_pa_PrintArg_struct _tmp6B2;struct Cyc_String_pa_PrintArg_struct
_tmp6B1;(_tmp6B1.tag=0,((_tmp6B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp52F),((_tmp6B2.tag=0,((_tmp6B2.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp52E),((_tmp6B3.tag=0,((_tmp6B3.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)sloc),((_tmp6B4[0]=& _tmp6B3,((_tmp6B4[1]=& _tmp6B2,((_tmp6B4[
2]=& _tmp6B1,Cyc_printf(((_tmp6B5="%s: insert `%s' for `%s'\n",_tag_dyneither(
_tmp6B5,sizeof(char),26))),_tag_dyneither(_tmp6B4,sizeof(void*),3)))))))))))))))))));};}};}
