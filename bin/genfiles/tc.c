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
typedef void*Cyc_parg_t;struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 160 "core.h"
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
# 61
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 123
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 305
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 377
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
# 50
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 747 "absyn.h"
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 904
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 912
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 914
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 918
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 932
void*Cyc_Absyn_compress_kb(void*);
# 937
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 961
void*Cyc_Absyn_string_typ(void*rgn);
# 962
void*Cyc_Absyn_const_string_typ(void*rgn);
# 983
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 994
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1065
extern struct _tuple0*Cyc_Absyn_uniquergn_qvar;
# 1151
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 56 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 58
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 64
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 66
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 69
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 77
int Cyc_Dict_is_empty(struct Cyc_Dict_Dict d);
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
# 189
struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d);struct _tuple10{void*f1;void*f2;};
# 232 "dict.h"
struct _tuple10*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
# 255
struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(struct _RegionHandle*,int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 288
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;
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
# 115 "tcenv.h"
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*);
# 116
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,unsigned int,struct Cyc_Absyn_Fndecl*);
# 121
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,unsigned int,struct _dyneither_ptr*,struct Cyc_List_List*);
# 126
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 182
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);
# 210
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
# 211
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 65
void*Cyc_Tcutil_compress(void*t);
# 68
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 91
int Cyc_Tcutil_is_function_type(void*t);
# 99
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 100
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
# 101
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 103
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
# 106
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
# 107
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
# 108
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
# 109
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 134
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 148
void Cyc_Tcutil_explain_failure();
# 150
int Cyc_Tcutil_unify(void*,void*);
# 164
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 174
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 204 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 206
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 214
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 218
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 270
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 295
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 306
int Cyc_Tcutil_bits_only(void*t);
# 309
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 316
int Cyc_Tcutil_supports_default(void*);
# 327
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 331
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 338
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 33
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple10*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
# 40
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 27 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 34 "tc.h"
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 38
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
# 41
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);
# 42
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
# 43
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Enumdecl*);extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 41 "tcdecl.h"
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_xdatatypefielddecl_t;
# 66 "tcdecl.h"
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 69
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 71
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 79
void*Cyc_Tcdecl_merge_binding(void*d0,void*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 88
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg);static char _tmp0[1]="";
# 46 "tc.cyc"
static struct _dyneither_ptr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1};
# 47
static struct _dyneither_ptr*Cyc_Tc_tc_msg=(struct _dyneither_ptr*)& Cyc_Tc_tc_msg_c;struct _tuple12{unsigned int f1;struct _tuple0*f2;int f3;};
# 49
static int Cyc_Tc_export_member(struct _tuple0*x,struct Cyc_List_List*exports){
# 50
for(0;exports != 0;exports=exports->tl){
# 51
struct _tuple12*_tmp1=(struct _tuple12*)exports->hd;
# 52
if(Cyc_Absyn_qvar_cmp(x,(*_tmp1).f2)== 0){
# 55
(*_tmp1).f3=1;
# 56
return 1;}}
# 59
return 0;}struct _tuple13{void*f1;int f2;};
# 62
static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){
# 66
struct Cyc_Absyn_Vardecl _tmp3;enum Cyc_Absyn_Scope _tmp4;struct _tuple0*_tmp5;struct _tuple0 _tmp6;union Cyc_Absyn_Nmspace _tmp7;struct _dyneither_ptr*_tmp8;void*_tmp9;struct Cyc_Absyn_Exp*_tmpA;struct Cyc_List_List*_tmpB;struct Cyc_Absyn_Vardecl*_tmp2=vd;_tmp3=*_tmp2;_tmp4=_tmp3.sc;_tmp5=_tmp3.name;_tmp6=*_tmp5;_tmp7=_tmp6.f1;_tmp8=_tmp6.f2;_tmp9=_tmp3.type;_tmpA=_tmp3.initializer;_tmpB=_tmp3.attributes;
# 72
{union Cyc_Absyn_Nmspace _tmpC=_tmp7;struct Cyc_List_List*_tmpD;_LL1: if((_tmpC.Rel_n).tag != 1)goto _LL3;_tmpD=(struct Cyc_List_List*)(_tmpC.Rel_n).val;_LL2:
# 75
(*vd->name).f1=Cyc_Absyn_Abs_n(te->ns,vd->sc == Cyc_Absyn_ExternC  || in_cinclude);
# 76
goto _LL0;_LL3:;_LL4:
# 77
 goto _LL0;_LL0:;}
# 81
{void*_tmpE=Cyc_Tcutil_compress(_tmp9);struct Cyc_Absyn_ArrayInfo _tmp10;void*_tmp11;struct Cyc_Absyn_Tqual _tmp12;struct Cyc_Absyn_Exp*_tmp13;union Cyc_Absyn_Constraint*_tmp14;unsigned int _tmp15;_LL6:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpF=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE;if(_tmpF->tag != 8)goto _LL8;else{_tmp10=_tmpF->f1;_tmp11=_tmp10.elt_type;_tmp12=_tmp10.tq;_tmp13=_tmp10.num_elts;if(_tmp13 != 0)goto _LL8;_tmp14=_tmp10.zero_term;_tmp15=_tmp10.zt_loc;}}if(!(_tmpA != 0))goto _LL8;_LL7:
# 83
{void*_tmp16=_tmpA->r;union Cyc_Absyn_Cnst _tmp18;struct _dyneither_ptr _tmp19;union Cyc_Absyn_Cnst _tmp1B;struct _dyneither_ptr _tmp1C;struct Cyc_Absyn_Exp*_tmp1E;struct Cyc_List_List*_tmp20;struct Cyc_List_List*_tmp22;_LLB: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp17=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16;if(_tmp17->tag != 0)goto _LLD;else{_tmp18=_tmp17->f1;if((_tmp18.String_c).tag != 8)goto _LLD;_tmp19=(struct _dyneither_ptr)(_tmp18.String_c).val;}}_LLC:
# 85
 _tmp9=(vd->type=Cyc_Absyn_array_typ(_tmp11,_tmp12,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(_get_dyneither_size(_tmp19,sizeof(char)),0),_tmp14,_tmp15));
# 86
goto _LLA;_LLD: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16;if(_tmp1A->tag != 0)goto _LLF;else{_tmp1B=_tmp1A->f1;if((_tmp1B.Wstring_c).tag != 9)goto _LLF;_tmp1C=(struct _dyneither_ptr)(_tmp1B.Wstring_c).val;}}_LLE:
# 88
 _tmp9=(vd->type=Cyc_Absyn_array_typ(_tmp11,_tmp12,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0),_tmp14,_tmp15));
# 89
goto _LLA;_LLF: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp1D=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp16;if(_tmp1D->tag != 26)goto _LL11;else{_tmp1E=_tmp1D->f2;}}_LL10:
# 92
 _tmp9=(vd->type=Cyc_Absyn_array_typ(_tmp11,_tmp12,(struct Cyc_Absyn_Exp*)_tmp1E,_tmp14,_tmp15));
# 93
goto _LLA;_LL11: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp1F=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp16;if(_tmp1F->tag != 34)goto _LL13;else{_tmp20=_tmp1F->f2;}}_LL12:
# 94
 _tmp22=_tmp20;goto _LL14;_LL13: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp21=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp16;if(_tmp21->tag != 25)goto _LL15;else{_tmp22=_tmp21->f1;}}_LL14:
# 96
 _tmp9=(vd->type=Cyc_Absyn_array_typ(_tmp11,_tmp12,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp22),0),_tmp14,_tmp15));
# 97
goto _LLA;_LL15:;_LL16:
# 98
 goto _LLA;_LLA:;}
# 100
goto _LL5;_LL8:;_LL9:
# 101
 goto _LL5;_LL5:;}
# 104
Cyc_Tcutil_check_valid_toplevel_type(loc,te,_tmp9);
# 106
(vd->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,_tmp9);
# 109
{void*_tmp23=Cyc_Tcutil_compress(_tmp9);_LL18: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp24=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp23;if(_tmp24->tag != 8)goto _LL1A;}_LL19:
# 110
 vd->escapes=0;goto _LL17;_LL1A:;_LL1B:
# 111
 vd->escapes=1;goto _LL17;_LL17:;}
# 114
if(Cyc_Tcutil_is_function_type(_tmp9))
# 115
_tmpB=Cyc_Tcutil_transfer_fn_type_atts(_tmp9,_tmpB);
# 117
if(_tmp4 == Cyc_Absyn_Extern  || _tmp4 == Cyc_Absyn_ExternC){
# 118
if(_tmpA != 0){
# 119
const char*_tmp28E;void*_tmp28D;(_tmp28D=0,Cyc_Tcutil_terr(loc,((_tmp28E="extern declaration should not have initializer",_tag_dyneither(_tmp28E,sizeof(char),47))),_tag_dyneither(_tmp28D,sizeof(void*),0)));}}else{
# 120
if(!Cyc_Tcutil_is_function_type(_tmp9)){
# 124
for(0;_tmpB != 0;_tmpB=_tmpB->tl){
# 125
void*_tmp27=(void*)_tmpB->hd;_LL1D: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp28=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp27;if(_tmp28->tag != 6)goto _LL1F;}_LL1E:
# 126
 goto _LL20;_LL1F: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp29=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp27;if(_tmp29->tag != 8)goto _LL21;}_LL20:
# 129
 goto _LL22;_LL21: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp2A=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp27;if(_tmp2A->tag != 9)goto _LL23;}_LL22:
# 130
 goto _LL24;_LL23: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp2B=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp27;if(_tmp2B->tag != 10)goto _LL25;}_LL24:
# 131
 goto _LL26;_LL25: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp2C=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp27;if(_tmp2C->tag != 11)goto _LL27;}_LL26:
# 132
 goto _LL28;_LL27: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp2D=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp27;if(_tmp2D->tag != 12)goto _LL29;}_LL28:
# 133
 goto _LL2A;_LL29: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp2E=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp27;if(_tmp2E->tag != 13)goto _LL2B;}_LL2A:
# 134
 goto _LL2C;_LL2B: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp2F=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp27;if(_tmp2F->tag != 14)goto _LL2D;}_LL2C:
# 135
 continue;_LL2D:;_LL2E:
# 137
{const char*_tmp293;void*_tmp292[2];struct Cyc_String_pa_PrintArg_struct _tmp291;struct Cyc_String_pa_PrintArg_struct _tmp290;(_tmp290.tag=0,((_tmp290.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 138
Cyc_Absynpp_qvar2string(vd->name)),((_tmp291.tag=0,((_tmp291.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmpB->hd)),((_tmp292[0]=& _tmp291,((_tmp292[1]=& _tmp290,Cyc_Tcutil_terr(loc,((_tmp293="bad attribute %s for variable %s",_tag_dyneither(_tmp293,sizeof(char),33))),_tag_dyneither(_tmp292,sizeof(void*),2)))))))))))));}
# 139
goto _LL1C;_LL1C:;}
# 142
if(_tmpA == 0  || in_cinclude){
# 143
if(check_var_init  && !Cyc_Tcutil_supports_default(_tmp9)){
# 144
const char*_tmp298;void*_tmp297[2];struct Cyc_String_pa_PrintArg_struct _tmp296;struct Cyc_String_pa_PrintArg_struct _tmp295;(_tmp295.tag=0,((_tmp295.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 145
Cyc_Absynpp_typ2string(_tmp9)),((_tmp296.tag=0,((_tmp296.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp297[0]=& _tmp296,((_tmp297[1]=& _tmp295,Cyc_Tcutil_terr(loc,((_tmp298="initializer required for variable %s of type %s",_tag_dyneither(_tmp298,sizeof(char),48))),_tag_dyneither(_tmp297,sizeof(void*),2)))))))))))));}}else{
# 147
struct Cyc_Absyn_Exp*_tmp38=(struct Cyc_Absyn_Exp*)_tmpA;
# 149
void*_tmp39=Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp9,_tmp38);
# 150
if(!Cyc_Tcutil_coerce_assign(te,_tmp38,_tmp9)){
# 151
{const char*_tmp29E;void*_tmp29D[3];struct Cyc_String_pa_PrintArg_struct _tmp29C;struct Cyc_String_pa_PrintArg_struct _tmp29B;struct Cyc_String_pa_PrintArg_struct _tmp29A;(_tmp29A.tag=0,((_tmp29A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 152
Cyc_Absynpp_typ2string(_tmp39)),((_tmp29B.tag=0,((_tmp29B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp9)),((_tmp29C.tag=0,((_tmp29C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp29D[0]=& _tmp29C,((_tmp29D[1]=& _tmp29B,((_tmp29D[2]=& _tmp29A,Cyc_Tcutil_terr(loc,((_tmp29E="%s declared with type \n%s\n but initialized with type \n%s",_tag_dyneither(_tmp29E,sizeof(char),57))),_tag_dyneither(_tmp29D,sizeof(void*),3)))))))))))))))))));}
# 153
Cyc_Tcutil_explain_failure();}
# 156
if(!Cyc_Tcutil_is_const_exp(te,_tmp38)){
# 157
const char*_tmp2A1;void*_tmp2A0;(_tmp2A0=0,Cyc_Tcutil_terr(loc,((_tmp2A1="initializer is not a constant expression",_tag_dyneither(_tmp2A1,sizeof(char),41))),_tag_dyneither(_tmp2A0,sizeof(void*),0)));}}}else{
# 161
for(0;_tmpB != 0;_tmpB=_tmpB->tl){
# 162
void*_tmp41=(void*)_tmpB->hd;_LL30: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp42=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp41;if(_tmp42->tag != 0)goto _LL32;}_LL31:
# 164
 goto _LL33;_LL32: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp43=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp41;if(_tmp43->tag != 1)goto _LL34;}_LL33:
# 165
 goto _LL35;_LL34: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp44=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp41;if(_tmp44->tag != 2)goto _LL36;}_LL35:
# 166
 goto _LL37;_LL36: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp45=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp41;if(_tmp45->tag != 3)goto _LL38;}_LL37:
# 167
 goto _LL39;_LL38: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp46=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp41;if(_tmp46->tag != 4)goto _LL3A;}_LL39:
# 168
 goto _LL3B;_LL3A: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp47=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp41;if(_tmp47->tag != 19)goto _LL3C;}_LL3B:
# 169
 goto _LL3D;_LL3C: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp48=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp41;if(_tmp48->tag != 20)goto _LL3E;}_LL3D:
# 170
 goto _LL3F;_LL3E: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp49=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp41;if(_tmp49->tag != 22)goto _LL40;}_LL3F:
# 171
 goto _LL41;_LL40: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp4A=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp41;if(_tmp4A->tag != 5)goto _LL42;}_LL41: {
# 172
const char*_tmp2A4;void*_tmp2A3;(_tmp2A3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp2A4="tcVardecl: fn type atts in function var decl",_tag_dyneither(_tmp2A4,sizeof(char),45))),_tag_dyneither(_tmp2A3,sizeof(void*),0)));}_LL42: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp4B=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp41;if(_tmp4B->tag != 6)goto _LL44;}_LL43:
# 174
 goto _LL45;_LL44: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp4C=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp41;if(_tmp4C->tag != 7)goto _LL46;}_LL45:
# 176
{const char*_tmp2A8;void*_tmp2A7[1];struct Cyc_String_pa_PrintArg_struct _tmp2A6;(_tmp2A6.tag=0,((_tmp2A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 177
Cyc_Absyn_attribute2string((void*)_tmpB->hd)),((_tmp2A7[0]=& _tmp2A6,Cyc_Tcutil_terr(loc,((_tmp2A8="bad attribute %s in function declaration",_tag_dyneither(_tmp2A8,sizeof(char),41))),_tag_dyneither(_tmp2A7,sizeof(void*),1)))))));}
# 178
goto _LL2F;_LL46:;_LL47:
# 179
 continue;_LL2F:;}}}
# 184
{struct _handler_cons _tmp52;_push_handler(& _tmp52);{int _tmp54=0;if(setjmp(_tmp52.handler))_tmp54=1;if(!_tmp54){
# 185
{struct _tuple13*_tmp55=((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,_tmp8);
# 186
void*_tmp56=(*_tmp55).f1;void*_tmp58;_LL49: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp57=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp56;if(_tmp57->tag != 0)goto _LL4B;else{_tmp58=(void*)_tmp57->f1;}}_LL4A: {
# 188
struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp2AB;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2AA;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp5D=(_tmp2AA=_cycalloc(sizeof(*_tmp2AA)),((_tmp2AA[0]=((_tmp2AB.tag=1,((_tmp2AB.f1=vd,_tmp2AB)))),_tmp2AA)));
# 189
void*_tmp5E=Cyc_Tcdecl_merge_binding(_tmp58,(void*)_tmp5D,loc,Cyc_Tc_tc_msg);
# 190
if(_tmp5E == (void*)& Cyc_Absyn_Unresolved_b_val){_npop_handler(0);return;}
# 192
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
# 193
if(_tmp5E != _tmp58  || (*_tmp55).f2){
# 195
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp2B1;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp2B0;struct _tuple13*_tmp2AF;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,_tmp8,(struct _tuple13*)(
# 196
(_tmp2AF=_cycalloc(sizeof(*_tmp2AF)),((_tmp2AF->f1=(void*)((_tmp2B1=_cycalloc(sizeof(*_tmp2B1)),((_tmp2B1[0]=((_tmp2B0.tag=0,((_tmp2B0.f1=(void*)_tmp5E,_tmp2B0)))),_tmp2B1)))),((_tmp2AF->f2=1,_tmp2AF)))))));}}
# 197
_npop_handler(0);return;}_LL4B: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp59=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp56;if(_tmp59->tag != 1)goto _LL4D;}_LL4C:
# 199
{const char*_tmp2B4;void*_tmp2B3;(_tmp2B3=0,Cyc_Tcutil_warn(loc,((_tmp2B4="variable declaration shadows previous struct declaration",_tag_dyneither(_tmp2B4,sizeof(char),57))),_tag_dyneither(_tmp2B3,sizeof(void*),0)));}
# 200
goto _LL48;_LL4D: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp5A=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp56;if(_tmp5A->tag != 2)goto _LL4F;}_LL4E:
# 202
{const char*_tmp2B7;void*_tmp2B6;(_tmp2B6=0,Cyc_Tcutil_warn(loc,((_tmp2B7="variable declaration shadows previous datatype constructor",_tag_dyneither(_tmp2B7,sizeof(char),59))),_tag_dyneither(_tmp2B6,sizeof(void*),0)));}
# 203
goto _LL48;_LL4F: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp5B=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp56;if(_tmp5B->tag != 4)goto _LL51;}_LL50:
# 204
 goto _LL52;_LL51: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp5C=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp56;if(_tmp5C->tag != 3)goto _LL48;}_LL52:
# 206
{const char*_tmp2BA;void*_tmp2B9;(_tmp2B9=0,Cyc_Tcutil_warn(loc,((_tmp2BA="variable declaration shadows previous enum tag",_tag_dyneither(_tmp2BA,sizeof(char),47))),_tag_dyneither(_tmp2B9,sizeof(void*),0)));}
# 207
goto _LL48;_LL48:;}
# 185
;_pop_handler();}else{void*_tmp53=(void*)_exn_thrown;void*_tmp6B=_tmp53;_LL54: {struct Cyc_Dict_Absent_exn_struct*_tmp6C=(struct Cyc_Dict_Absent_exn_struct*)_tmp6B;if(_tmp6C->tag != Cyc_Dict_Absent)goto _LL56;}_LL55:
# 209
 goto _LL53;_LL56:;_LL57:(void)_throw(_tmp6B);_LL53:;}};}
# 210
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
# 211
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp2C9;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2C8;struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp2C7;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp2C6;struct _tuple13*_tmp2C5;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,_tmp8,(struct _tuple13*)(
# 212
(_tmp2C5=_cycalloc(sizeof(*_tmp2C5)),((_tmp2C5->f1=(void*)((_tmp2C9=_cycalloc(sizeof(*_tmp2C9)),((_tmp2C9[0]=((_tmp2C6.tag=0,((_tmp2C6.f1=(void*)((void*)((_tmp2C8=_cycalloc(sizeof(*_tmp2C8)),((_tmp2C8[0]=((_tmp2C7.tag=1,((_tmp2C7.f1=vd,_tmp2C7)))),_tmp2C8))))),_tmp2C6)))),_tmp2C9)))),((_tmp2C5->f2=0,_tmp2C5)))))));}}
# 215
static int Cyc_Tc_is_main(struct _tuple0*n){
# 216
struct _tuple0 _tmp73;union Cyc_Absyn_Nmspace _tmp74;struct _dyneither_ptr*_tmp75;struct _tuple0*_tmp72=n;_tmp73=*_tmp72;_tmp74=_tmp73.f1;_tmp75=_tmp73.f2;{
# 217
union Cyc_Absyn_Nmspace _tmp76=_tmp74;struct Cyc_List_List*_tmp77;_LL59: if((_tmp76.Abs_n).tag != 2)goto _LL5B;_tmp77=(struct Cyc_List_List*)(_tmp76.Abs_n).val;if(_tmp77 != 0)goto _LL5B;_LL5A: {
# 219
const char*_tmp2CA;return Cyc_strcmp((struct _dyneither_ptr)*_tmp75,((_tmp2CA="main",_tag_dyneither(_tmp2CA,sizeof(char),5))))== 0;}_LL5B:;_LL5C:
# 220
 return 0;_LL58:;};}
# 224
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Fndecl*fd,int in_cinclude,struct Cyc_List_List**exports){
# 227
struct _dyneither_ptr*v=(*fd->name).f2;
# 231
if(fd->sc == Cyc_Absyn_ExternC  && !in_cinclude){
# 232
const char*_tmp2CD;void*_tmp2CC;(_tmp2CC=0,Cyc_Tcutil_terr(loc,((_tmp2CD="extern \"C\" functions cannot be implemented in Cyclone",_tag_dyneither(_tmp2CD,sizeof(char),54))),_tag_dyneither(_tmp2CC,sizeof(void*),0)));}
# 234
{union Cyc_Absyn_Nmspace _tmp7B=(*fd->name).f1;struct Cyc_List_List*_tmp7C;_LL5E: if((_tmp7B.Rel_n).tag != 1)goto _LL60;_tmp7C=(struct Cyc_List_List*)(_tmp7B.Rel_n).val;if(_tmp7C != 0)goto _LL60;_LL5F:
# 237
(*fd->name).f1=Cyc_Absyn_Abs_n(te->ns,fd->sc == Cyc_Absyn_ExternC  || in_cinclude);
# 238
goto _LL5D;_LL60:;_LL61:
# 240
 goto _LL5D;_LL5D:;}
# 243
Cyc_Tcutil_check_fndecl_valid_type(loc,te,fd);{
# 245
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 247
fd->attributes=Cyc_Tcutil_transfer_fn_type_atts(t,fd->attributes);
# 249
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
# 250
void*_tmp7D=(void*)atts->hd;_LL63: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp7E=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp7D;if(_tmp7E->tag != 7)goto _LL65;}_LL64:
# 251
 goto _LL66;_LL65: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp7F=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp7D;if(_tmp7F->tag != 6)goto _LL67;}_LL66:
# 253
{const char*_tmp2D1;void*_tmp2D0[1];struct Cyc_String_pa_PrintArg_struct _tmp2CF;(_tmp2CF.tag=0,((_tmp2CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp2D0[0]=& _tmp2CF,Cyc_Tcutil_terr(loc,((_tmp2D1="bad attribute %s for function",_tag_dyneither(_tmp2D1,sizeof(char),30))),_tag_dyneither(_tmp2D0,sizeof(void*),1)))))));}
# 254
goto _LL62;_LL67:;_LL68:
# 255
 goto _LL62;_LL62:;}}
# 259
{struct _handler_cons _tmp83;_push_handler(& _tmp83);{int _tmp85=0;if(setjmp(_tmp83.handler))_tmp85=1;if(!_tmp85){
# 260
{struct _tuple13*_tmp86=((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,v);
# 261
void*_tmp87=(*_tmp86).f1;void*_tmp89;_LL6A: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp88=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp87;if(_tmp88->tag != 0)goto _LL6C;else{_tmp89=(void*)_tmp88->f1;}}_LL6B: {
# 263
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp2D4;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp2D3;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp8E=(_tmp2D3=_cycalloc(sizeof(*_tmp2D3)),((_tmp2D3[0]=((_tmp2D4.tag=2,((_tmp2D4.f1=fd,_tmp2D4)))),_tmp2D3)));
# 264
void*_tmp8F=Cyc_Tcdecl_merge_binding(_tmp89,(void*)_tmp8E,loc,Cyc_Tc_tc_msg);
# 265
if(_tmp8F == (void*)& Cyc_Absyn_Unresolved_b_val)goto _LL69;
# 267
if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
# 268
if(!(_tmp8F == _tmp89  && (*_tmp86).f2)){
# 269
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp2DA;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp2D9;struct _tuple13*_tmp2D8;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,v,(struct _tuple13*)(
# 270
(_tmp2D8=_cycalloc(sizeof(*_tmp2D8)),((_tmp2D8->f1=(void*)((_tmp2DA=_cycalloc(sizeof(*_tmp2DA)),((_tmp2DA[0]=((_tmp2D9.tag=0,((_tmp2D9.f1=(void*)_tmp8F,_tmp2D9)))),_tmp2DA)))),((_tmp2D8->f2=1,_tmp2D8)))))));}}
# 271
goto _LL69;}_LL6C: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp8A=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp87;if(_tmp8A->tag != 1)goto _LL6E;}_LL6D:
# 273
{const char*_tmp2DD;void*_tmp2DC;(_tmp2DC=0,Cyc_Tcutil_warn(loc,((_tmp2DD="function declaration shadows previous type declaration",_tag_dyneither(_tmp2DD,sizeof(char),55))),_tag_dyneither(_tmp2DC,sizeof(void*),0)));}
# 274
goto _LL69;_LL6E: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp8B=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp87;if(_tmp8B->tag != 2)goto _LL70;}_LL6F:
# 276
{const char*_tmp2E0;void*_tmp2DF;(_tmp2DF=0,Cyc_Tcutil_warn(loc,((_tmp2E0="function declaration shadows previous datatype constructor",_tag_dyneither(_tmp2E0,sizeof(char),59))),_tag_dyneither(_tmp2DF,sizeof(void*),0)));}
# 277
goto _LL69;_LL70: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp8C=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp87;if(_tmp8C->tag != 4)goto _LL72;}_LL71:
# 278
 goto _LL73;_LL72: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp8D=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp87;if(_tmp8D->tag != 3)goto _LL69;}_LL73:
# 280
{const char*_tmp2E3;void*_tmp2E2;(_tmp2E2=0,Cyc_Tcutil_warn(loc,((_tmp2E3="function declaration shadows previous enum tag",_tag_dyneither(_tmp2E3,sizeof(char),47))),_tag_dyneither(_tmp2E2,sizeof(void*),0)));}
# 281
goto _LL69;_LL69:;}
# 260
;_pop_handler();}else{void*_tmp84=(void*)_exn_thrown;void*_tmp9C=_tmp84;_LL75: {struct Cyc_Dict_Absent_exn_struct*_tmp9D=(struct Cyc_Dict_Absent_exn_struct*)_tmp9C;if(_tmp9D->tag != Cyc_Dict_Absent)goto _LL77;}_LL76:
# 284
 if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
# 285
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp2E6;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp2E5;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp9E=(_tmp2E5=_cycalloc(sizeof(*_tmp2E5)),((_tmp2E5[0]=((_tmp2E6.tag=2,((_tmp2E6.f1=fd,_tmp2E6)))),_tmp2E5)));
# 286
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp2F5;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp2F4;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp2F3;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp2F2;struct _tuple13*_tmp2F1;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,v,(struct _tuple13*)(
# 287
(_tmp2F1=_cycalloc(sizeof(*_tmp2F1)),((_tmp2F1->f1=(void*)((_tmp2F5=_cycalloc(sizeof(*_tmp2F5)),((_tmp2F5[0]=((_tmp2F2.tag=0,((_tmp2F2.f1=(void*)((void*)((_tmp2F4=_cycalloc(sizeof(*_tmp2F4)),((_tmp2F4[0]=((_tmp2F3.tag=2,((_tmp2F3.f1=fd,_tmp2F3)))),_tmp2F4))))),_tmp2F2)))),_tmp2F5)))),((_tmp2F1->f2=0,_tmp2F1)))))));}
# 289
goto _LL74;_LL77:;_LL78:(void)_throw(_tmp9C);_LL74:;}};}
# 293
if(in_cinclude)return;{
# 298
struct _RegionHandle _tmpA6=_new_region("fnrgn");struct _RegionHandle*fnrgn=& _tmpA6;_push_region(fnrgn);{
# 299
struct Cyc_Tcenv_Fenv*_tmpA7=Cyc_Tcenv_new_fenv(fnrgn,loc,fd);
# 300
struct Cyc_Tcenv_Tenv*_tmp2F6;struct Cyc_Tcenv_Tenv*_tmpA8=(_tmp2F6=_region_malloc(fnrgn,sizeof(*_tmp2F6)),((_tmp2F6->ns=te->ns,((_tmp2F6->ae=te->ae,((_tmp2F6->le=(struct Cyc_Tcenv_Fenv*)_tmpA7,((_tmp2F6->allow_valueof=0,_tmp2F6)))))))));
# 302
Cyc_Tcstmt_tcStmt(_tmpA8,fd->body,0);
# 305
Cyc_Tcenv_check_delayed_effects(_tmpA8);
# 306
Cyc_Tcenv_check_delayed_constraints(_tmpA8);
# 309
if(!Cyc_Tcenv_all_labels_resolved(_tmpA8)){
# 310
const char*_tmp2F9;void*_tmp2F8;(_tmp2F8=0,Cyc_Tcutil_terr(loc,((_tmp2F9="function has goto statements to undefined labels",_tag_dyneither(_tmp2F9,sizeof(char),49))),_tag_dyneither(_tmp2F8,sizeof(void*),0)));}}
# 313
if(Cyc_Tc_is_main(fd->name)){
# 315
{void*_tmpAC=Cyc_Tcutil_compress(fd->ret_type);enum Cyc_Absyn_Size_of _tmpAF;_LL7A: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpAD=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpAC;if(_tmpAD->tag != 0)goto _LL7C;}_LL7B:
# 316
{const char*_tmp2FC;void*_tmp2FB;(_tmp2FB=0,Cyc_Tcutil_warn(loc,((_tmp2FC="main declared with return type void",_tag_dyneither(_tmp2FC,sizeof(char),36))),_tag_dyneither(_tmp2FB,sizeof(void*),0)));}goto _LL79;_LL7C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpAE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpAC;if(_tmpAE->tag != 6)goto _LL7E;else{_tmpAF=_tmpAE->f2;}}_LL7D:
# 317
 goto _LL79;_LL7E:;_LL7F:
# 319
{const char*_tmp300;void*_tmp2FF[1];struct Cyc_String_pa_PrintArg_struct _tmp2FE;(_tmp2FE.tag=0,((_tmp2FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 320
Cyc_Absynpp_typ2string(fd->ret_type)),((_tmp2FF[0]=& _tmp2FE,Cyc_Tcutil_terr(loc,((_tmp300="main declared with return type %s instead of int or void",_tag_dyneither(_tmp300,sizeof(char),57))),_tag_dyneither(_tmp2FF,sizeof(void*),1)))))));}
# 321
goto _LL79;_LL79:;}
# 323
if(fd->c_varargs  || fd->cyc_varargs != 0){
# 324
const char*_tmp303;void*_tmp302;(_tmp302=0,Cyc_Tcutil_terr(loc,((_tmp303="main declared with varargs",_tag_dyneither(_tmp303,sizeof(char),27))),_tag_dyneither(_tmp302,sizeof(void*),0)));}{
# 325
struct Cyc_List_List*_tmpB7=fd->args;
# 326
if(_tmpB7 != 0){
# 327
struct _tuple8 _tmpB9;void*_tmpBA;struct _tuple8*_tmpB8=(struct _tuple8*)_tmpB7->hd;_tmpB9=*_tmpB8;_tmpBA=_tmpB9.f3;
# 328
{void*_tmpBB=Cyc_Tcutil_compress(_tmpBA);enum Cyc_Absyn_Size_of _tmpBD;_LL81: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpBC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpBB;if(_tmpBC->tag != 6)goto _LL83;else{_tmpBD=_tmpBC->f2;}}_LL82:
# 329
 goto _LL80;_LL83:;_LL84:
# 331
{const char*_tmp307;void*_tmp306[1];struct Cyc_String_pa_PrintArg_struct _tmp305;(_tmp305.tag=0,((_tmp305.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 332
Cyc_Absynpp_typ2string(_tmpBA)),((_tmp306[0]=& _tmp305,Cyc_Tcutil_terr(loc,((_tmp307="main declared with first argument of type %s instead of int",_tag_dyneither(_tmp307,sizeof(char),60))),_tag_dyneither(_tmp306,sizeof(void*),1)))))));}
# 333
goto _LL80;_LL80:;}
# 335
_tmpB7=_tmpB7->tl;
# 336
if(_tmpB7 != 0){
# 337
struct _tuple8 _tmpC2;void*_tmpC3;struct _tuple8*_tmpC1=(struct _tuple8*)_tmpB7->hd;_tmpC2=*_tmpC1;_tmpC3=_tmpC2.f3;
# 338
_tmpB7=_tmpB7->tl;
# 339
if(_tmpB7 != 0){
# 340
const char*_tmp30A;void*_tmp309;(_tmp309=0,Cyc_Tcutil_terr(loc,((_tmp30A="main declared with too many arguments",_tag_dyneither(_tmp30A,sizeof(char),38))),_tag_dyneither(_tmp309,sizeof(void*),0)));}{
# 341
struct Cyc_Core_Opt*_tmp30B;struct Cyc_Core_Opt*tvs=(_tmp30B=_cycalloc(sizeof(*_tmp30B)),((_tmp30B->v=fd->tvs,_tmp30B)));
# 342
struct Cyc_Core_Opt*_tmp313;struct Cyc_Core_Opt*_tmp312;struct Cyc_Core_Opt*_tmp311;struct Cyc_Core_Opt*_tmp310;struct Cyc_Core_Opt*_tmp30F;struct Cyc_Core_Opt*_tmp30E;struct Cyc_Core_Opt*_tmp30D;struct Cyc_Core_Opt*_tmp30C;if(((!Cyc_Tcutil_unify(_tmpC3,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(((_tmp312=_cycalloc(sizeof(*_tmp312)),((_tmp312->v=& Cyc_Tcutil_rk,_tmp312)))),tvs)),
# 343
Cyc_Absyn_new_evar(((_tmp313=_cycalloc(sizeof(*_tmp313)),((_tmp313->v=& Cyc_Tcutil_rk,_tmp313)))),tvs),
# 344
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())) && !
# 345
Cyc_Tcutil_unify(_tmpC3,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(((_tmp310=_cycalloc(sizeof(*_tmp310)),((_tmp310->v=& Cyc_Tcutil_rk,_tmp310)))),tvs)),
# 347
Cyc_Absyn_new_evar(((_tmp311=_cycalloc(sizeof(*_tmp311)),((_tmp311->v=& Cyc_Tcutil_rk,_tmp311)))),tvs),
# 348
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
# 349
Cyc_Tcutil_unify(_tmpC3,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(((_tmp30E=_cycalloc(sizeof(*_tmp30E)),((_tmp30E->v=& Cyc_Tcutil_rk,_tmp30E)))),tvs)),
# 350
Cyc_Absyn_new_evar(((_tmp30F=_cycalloc(sizeof(*_tmp30F)),((_tmp30F->v=& Cyc_Tcutil_rk,_tmp30F)))),tvs),
# 351
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
# 352
Cyc_Tcutil_unify(_tmpC3,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(((_tmp30C=_cycalloc(sizeof(*_tmp30C)),((_tmp30C->v=& Cyc_Tcutil_rk,_tmp30C)))),tvs)),
# 354
Cyc_Absyn_new_evar(((_tmp30D=_cycalloc(sizeof(*_tmp30D)),((_tmp30D->v=& Cyc_Tcutil_rk,_tmp30D)))),tvs),
# 355
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))){
# 356
const char*_tmp317;void*_tmp316[1];struct Cyc_String_pa_PrintArg_struct _tmp315;(_tmp315.tag=0,((_tmp315.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 357
Cyc_Absynpp_typ2string(_tmpC3)),((_tmp316[0]=& _tmp315,Cyc_Tcutil_terr(loc,((_tmp317="second argument of main has type %s instead of char??",_tag_dyneither(_tmp317,sizeof(char),54))),_tag_dyneither(_tmp316,sizeof(void*),1)))))));}};}}};}
# 298
;_pop_region(fnrgn);};};}
# 364
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Typedefdecl*td){
# 365
struct _dyneither_ptr*v=(*td->name).f2;
# 370
{union Cyc_Absyn_Nmspace _tmpD2=(*td->name).f1;struct Cyc_List_List*_tmpD3;_LL86: if((_tmpD2.Rel_n).tag != 1)goto _LL88;_tmpD3=(struct Cyc_List_List*)(_tmpD2.Rel_n).val;_LL87:
# 373
(*td->name).f1=Cyc_Absyn_Abs_n(te->ns,0);
# 374
goto _LL85;_LL88:;_LL89:
# 375
 goto _LL85;_LL85:;}
# 378
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(ge->typedefs,v)){
# 379
{const char*_tmp31B;void*_tmp31A[1];struct Cyc_String_pa_PrintArg_struct _tmp319;(_tmp319.tag=0,((_tmp319.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp31A[0]=& _tmp319,Cyc_Tcutil_terr(loc,((_tmp31B="redeclaration of typedef %s",_tag_dyneither(_tmp31B,sizeof(char),28))),_tag_dyneither(_tmp31A,sizeof(void*),1)))))));}
# 380
return;}
# 383
Cyc_Tcutil_check_unique_tvars(loc,td->tvs);
# 384
Cyc_Tcutil_add_tvar_identities(td->tvs);
# 385
if(td->defn != 0){
# 386
Cyc_Tcutil_check_type(loc,te,td->tvs,& Cyc_Tcutil_tak,0,(void*)_check_null(td->defn));
# 387
(td->tq).real_const=
# 388
Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(void*)_check_null(td->defn));}
# 392
{struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){
# 393
void*_tmpD7=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);struct Cyc_Core_Opt*_tmpD9;struct Cyc_Core_Opt**_tmpDA;struct Cyc_Core_Opt*_tmpDC;struct Cyc_Core_Opt**_tmpDD;struct Cyc_Absyn_Kind*_tmpDE;_LL8B: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpD8=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpD7;if(_tmpD8->tag != 1)goto _LL8D;else{_tmpD9=_tmpD8->f1;_tmpDA=(struct Cyc_Core_Opt**)& _tmpD8->f1;}}_LL8C:
# 395
 if(td->defn != 0){
# 396
const char*_tmp31F;void*_tmp31E[1];struct Cyc_String_pa_PrintArg_struct _tmp31D;(_tmp31D.tag=0,((_tmp31D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs->hd)->name),((_tmp31E[0]=& _tmp31D,Cyc_Tcutil_warn(loc,((_tmp31F="type variable %s is not used in typedef definition",_tag_dyneither(_tmp31F,sizeof(char),51))),_tag_dyneither(_tmp31E,sizeof(void*),1)))))));}
# 398
{struct Cyc_Core_Opt*_tmp320;*_tmpDA=((_tmp320=_cycalloc(sizeof(*_tmp320)),((_tmp320->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk),_tmp320))));}goto _LL8A;_LL8D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDB=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpD7;if(_tmpDB->tag != 2)goto _LL8F;else{_tmpDC=_tmpDB->f1;_tmpDD=(struct Cyc_Core_Opt**)& _tmpDB->f1;_tmpDE=_tmpDB->f2;}}_LL8E:
# 405
{struct Cyc_Core_Opt*_tmp321;*_tmpDD=((_tmp321=_cycalloc(sizeof(*_tmp321)),((_tmp321->v=Cyc_Tcutil_kind_to_bound(_tmpDE),_tmp321))));}
# 406
goto _LL8A;_LL8F:;_LL90:
# 407
 continue;_LL8A:;}}
# 411
ge->typedefs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(ge->typedefs,v,td);}
# 414
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){
# 421
struct _RegionHandle _tmpE4=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpE4;_push_region(uprev_rgn);
# 423
for(0;rpo != 0;rpo=rpo->tl){
# 424
struct _tuple10 _tmpE6;void*_tmpE7;void*_tmpE8;struct _tuple10*_tmpE5=(struct _tuple10*)rpo->hd;_tmpE6=*_tmpE5;_tmpE7=_tmpE6.f1;_tmpE8=_tmpE6.f2;
# 425
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_ek,0,_tmpE7);
# 426
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_trk,0,_tmpE8);}{
# 429
struct Cyc_List_List*prev_fields=0;
# 431
struct Cyc_List_List*_tmpE9=fields;for(0;_tmpE9 != 0;_tmpE9=_tmpE9->tl){
# 432
struct Cyc_Absyn_Aggrfield _tmpEB;struct _dyneither_ptr*_tmpEC;struct Cyc_Absyn_Tqual _tmpED;void*_tmpEE;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_List_List*_tmpF0;struct Cyc_Absyn_Aggrfield*_tmpEA=(struct Cyc_Absyn_Aggrfield*)_tmpE9->hd;_tmpEB=*_tmpEA;_tmpEC=_tmpEB.name;_tmpED=_tmpEB.tq;_tmpEE=_tmpEB.type;_tmpEF=_tmpEB.width;_tmpF0=_tmpEB.attributes;
# 434
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpEC)){
# 435
const char*_tmp325;void*_tmp324[1];struct Cyc_String_pa_PrintArg_struct _tmp323;(_tmp323.tag=0,((_tmp323.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpEC),((_tmp324[0]=& _tmp323,Cyc_Tcutil_terr(loc,((_tmp325="duplicate member %s",_tag_dyneither(_tmp325,sizeof(char),20))),_tag_dyneither(_tmp324,sizeof(void*),1)))))));}
# 438
{const char*_tmp326;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpEC,((_tmp326="",_tag_dyneither(_tmp326,sizeof(char),1))))!= 0){
# 439
struct Cyc_List_List*_tmp327;prev_fields=((_tmp327=_region_malloc(uprev_rgn,sizeof(*_tmp327)),((_tmp327->hd=_tmpEC,((_tmp327->tl=prev_fields,_tmp327))))));}}{
# 441
struct Cyc_Absyn_Kind*field_kind=& Cyc_Tcutil_tmk;
# 445
if(_tmpE9->tl == 0  && str_or_union == Cyc_Absyn_StructA)
# 446
field_kind=& Cyc_Tcutil_tak;
# 447
Cyc_Tcutil_check_type(loc,te,tvs,field_kind,0,_tmpEE);
# 449
(((struct Cyc_Absyn_Aggrfield*)_tmpE9->hd)->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)_tmpE9->hd)->tq).print_const,_tmpEE);
# 452
Cyc_Tcutil_check_bitfield(loc,te,_tmpEE,_tmpEF,_tmpEC);};}};
# 423
;_pop_region(uprev_rgn);}
# 458
static void Cyc_Tc_rule_out_memkind(unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*tvs,int constrain_top_kind){
# 460
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 461
void*_tmpF6=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);struct Cyc_Core_Opt*_tmpF8;struct Cyc_Core_Opt**_tmpF9;struct Cyc_Core_Opt*_tmpFB;struct Cyc_Core_Opt**_tmpFC;struct Cyc_Absyn_Kind*_tmpFD;struct Cyc_Absyn_Kind _tmpFE;enum Cyc_Absyn_KindQual _tmpFF;enum Cyc_Absyn_AliasQual _tmp100;struct Cyc_Core_Opt*_tmp102;struct Cyc_Core_Opt**_tmp103;struct Cyc_Absyn_Kind*_tmp104;struct Cyc_Absyn_Kind _tmp105;enum Cyc_Absyn_KindQual _tmp106;enum Cyc_Absyn_AliasQual _tmp107;struct Cyc_Core_Opt*_tmp109;struct Cyc_Core_Opt**_tmp10A;struct Cyc_Absyn_Kind*_tmp10B;struct Cyc_Absyn_Kind*_tmp10D;struct Cyc_Absyn_Kind _tmp10E;enum Cyc_Absyn_KindQual _tmp10F;enum Cyc_Absyn_AliasQual _tmp110;_LL92: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpF7=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpF6;if(_tmpF7->tag != 1)goto _LL94;else{_tmpF8=_tmpF7->f1;_tmpF9=(struct Cyc_Core_Opt**)& _tmpF7->f1;}}_LL93:
# 463
{struct Cyc_Core_Opt*_tmp328;*_tmpF9=((_tmp328=_cycalloc(sizeof(*_tmp328)),((_tmp328->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp328))));}continue;_LL94: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpFA=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpF6;if(_tmpFA->tag != 2)goto _LL96;else{_tmpFB=_tmpFA->f1;_tmpFC=(struct Cyc_Core_Opt**)& _tmpFA->f1;_tmpFD=_tmpFA->f2;_tmpFE=*_tmpFD;_tmpFF=_tmpFE.kind;if(_tmpFF != Cyc_Absyn_MemKind)goto _LL96;_tmp100=_tmpFE.aliasqual;}}_LL95:
# 465
 if(constrain_top_kind  && _tmp100 == Cyc_Absyn_Top){
# 466
struct Cyc_Absyn_Kind*_tmp32B;struct Cyc_Core_Opt*_tmp32A;*_tmpFC=((_tmp32A=_cycalloc(sizeof(*_tmp32A)),((_tmp32A->v=Cyc_Tcutil_kind_to_bound(((_tmp32B=_cycalloc_atomic(sizeof(*_tmp32B)),((_tmp32B->kind=Cyc_Absyn_BoxKind,((_tmp32B->aliasqual=Cyc_Absyn_Aliasable,_tmp32B))))))),_tmp32A))));}else{
# 468
struct Cyc_Absyn_Kind*_tmp32E;struct Cyc_Core_Opt*_tmp32D;*_tmpFC=((_tmp32D=_cycalloc(sizeof(*_tmp32D)),((_tmp32D->v=Cyc_Tcutil_kind_to_bound(((_tmp32E=_cycalloc_atomic(sizeof(*_tmp32E)),((_tmp32E->kind=Cyc_Absyn_BoxKind,((_tmp32E->aliasqual=_tmp100,_tmp32E))))))),_tmp32D))));}
# 469
continue;_LL96:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp101=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpF6;if(_tmp101->tag != 2)goto _LL98;else{_tmp102=_tmp101->f1;_tmp103=(struct Cyc_Core_Opt**)& _tmp101->f1;_tmp104=_tmp101->f2;_tmp105=*_tmp104;_tmp106=_tmp105.kind;if(_tmp106 != Cyc_Absyn_BoxKind)goto _LL98;_tmp107=_tmp105.aliasqual;if(_tmp107 != Cyc_Absyn_Top)goto _LL98;}}if(!constrain_top_kind)goto _LL98;_LL97:
# 471
{struct Cyc_Absyn_Kind*_tmp331;struct Cyc_Core_Opt*_tmp330;*_tmp103=((_tmp330=_cycalloc(sizeof(*_tmp330)),((_tmp330->v=Cyc_Tcutil_kind_to_bound(((_tmp331=_cycalloc_atomic(sizeof(*_tmp331)),((_tmp331->kind=Cyc_Absyn_BoxKind,((_tmp331->aliasqual=Cyc_Absyn_Aliasable,_tmp331))))))),_tmp330))));}
# 472
continue;_LL98: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp108=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpF6;if(_tmp108->tag != 2)goto _LL9A;else{_tmp109=_tmp108->f1;_tmp10A=(struct Cyc_Core_Opt**)& _tmp108->f1;_tmp10B=_tmp108->f2;}}_LL99:
# 474
{struct Cyc_Core_Opt*_tmp332;*_tmp10A=((_tmp332=_cycalloc(sizeof(*_tmp332)),((_tmp332->v=Cyc_Tcutil_kind_to_bound(_tmp10B),_tmp332))));}continue;_LL9A: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp10C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpF6;if(_tmp10C->tag != 0)goto _LL9C;else{_tmp10D=_tmp10C->f1;_tmp10E=*_tmp10D;_tmp10F=_tmp10E.kind;if(_tmp10F != Cyc_Absyn_MemKind)goto _LL9C;_tmp110=_tmp10E.aliasqual;}}_LL9B:
# 476
{const char*_tmp33A;void*_tmp339[3];struct Cyc_String_pa_PrintArg_struct _tmp338;struct Cyc_String_pa_PrintArg_struct _tmp337;struct Cyc_Absyn_Kind*_tmp336;struct Cyc_String_pa_PrintArg_struct _tmp335;(_tmp335.tag=0,((_tmp335.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 477
Cyc_Absynpp_kind2string(((_tmp336=_cycalloc_atomic(sizeof(*_tmp336)),((_tmp336->kind=Cyc_Absyn_MemKind,((_tmp336->aliasqual=_tmp110,_tmp336)))))))),((_tmp337.tag=0,((_tmp337.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp338.tag=0,((_tmp338.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n),((_tmp339[0]=& _tmp338,((_tmp339[1]=& _tmp337,((_tmp339[2]=& _tmp335,Cyc_Tcutil_terr(loc,((_tmp33A="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp33A,sizeof(char),57))),_tag_dyneither(_tmp339,sizeof(void*),3)))))))))))))))))));}
# 478
continue;_LL9C:;_LL9D:
# 479
 continue;_LL91:;}}struct _tuple14{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_Aggrdecl***f2;};
# 484
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Aggrdecl*ad){
# 485
struct _dyneither_ptr*_tmp11F=(*ad->name).f2;
# 491
{struct Cyc_List_List*atts=ad->attributes;for(0;atts != 0;atts=atts->tl){
# 492
void*_tmp120=(void*)atts->hd;_LL9F: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp121=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp120;if(_tmp121->tag != 7)goto _LLA1;}_LLA0:
# 493
 goto _LLA2;_LLA1: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp122=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp120;if(_tmp122->tag != 6)goto _LLA3;}_LLA2:
# 494
 continue;_LLA3:;_LLA4:
# 496
{const char*_tmp33F;void*_tmp33E[2];struct Cyc_String_pa_PrintArg_struct _tmp33D;struct Cyc_String_pa_PrintArg_struct _tmp33C;(_tmp33C.tag=0,((_tmp33C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp11F),((_tmp33D.tag=0,((_tmp33D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 497
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp33E[0]=& _tmp33D,((_tmp33E[1]=& _tmp33C,Cyc_Tcutil_terr(loc,((_tmp33F="bad attribute %s in  %s definition",_tag_dyneither(_tmp33F,sizeof(char),35))),_tag_dyneither(_tmp33E,sizeof(void*),2)))))))))))));}
# 498
goto _LL9E;_LL9E:;}}{
# 501
struct Cyc_List_List*_tmp127=ad->tvs;
# 503
{union Cyc_Absyn_Nmspace _tmp128=(*ad->name).f1;struct Cyc_List_List*_tmp129;_LLA6: if((_tmp128.Rel_n).tag != 1)goto _LLA8;_tmp129=(struct Cyc_List_List*)(_tmp128.Rel_n).val;if(_tmp129 != 0)goto _LLA8;_LLA7:
# 506
(*ad->name).f1=Cyc_Absyn_Abs_n(te->ns,ad->sc == Cyc_Absyn_ExternC);
# 507
goto _LLA5;_LLA8:;_LLA9:
# 509
 goto _LLA5;_LLA5:;}
# 513
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);
# 515
Cyc_Tcutil_add_tvar_identities(ad->tvs);
# 519
{struct _tuple14 _tmp340;struct _tuple14 _tmp12B=(_tmp340.f1=ad->impl,((_tmp340.f2=((struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->aggrdecls,_tmp11F),_tmp340)));struct Cyc_Absyn_AggrdeclImpl*_tmp12C;struct Cyc_Absyn_Aggrdecl***_tmp12D;struct Cyc_Absyn_AggrdeclImpl*_tmp12E;struct Cyc_Absyn_AggrdeclImpl _tmp12F;struct Cyc_List_List*_tmp130;struct Cyc_List_List*_tmp131;struct Cyc_List_List*_tmp132;int _tmp133;struct Cyc_Absyn_Aggrdecl***_tmp134;struct Cyc_Absyn_AggrdeclImpl*_tmp135;struct Cyc_Absyn_AggrdeclImpl _tmp136;struct Cyc_List_List*_tmp137;struct Cyc_List_List*_tmp138;struct Cyc_List_List*_tmp139;int _tmp13A;struct Cyc_Absyn_Aggrdecl***_tmp13B;struct Cyc_Absyn_Aggrdecl**_tmp13C;struct Cyc_Absyn_AggrdeclImpl*_tmp13D;struct Cyc_Absyn_Aggrdecl***_tmp13E;struct Cyc_Absyn_Aggrdecl**_tmp13F;_LLAB: _tmp12C=_tmp12B.f1;if(_tmp12C != 0)goto _LLAD;_tmp12D=_tmp12B.f2;if(_tmp12D != 0)goto _LLAD;_LLAC:
# 522
 Cyc_Tc_rule_out_memkind(loc,_tmp11F,_tmp127,0);
# 523
{struct Cyc_Absyn_Aggrdecl**_tmp341;ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,_tmp11F,((_tmp341=_cycalloc(sizeof(*_tmp341)),((_tmp341[0]=ad,_tmp341)))));}
# 524
goto _LLAA;_LLAD: _tmp12E=_tmp12B.f1;if(_tmp12E == 0)goto _LLAF;_tmp12F=*_tmp12E;_tmp130=_tmp12F.exist_vars;_tmp131=_tmp12F.rgn_po;_tmp132=_tmp12F.fields;_tmp133=_tmp12F.tagged;_tmp134=_tmp12B.f2;if(_tmp134 != 0)goto _LLAF;_LLAE: {
# 528
struct Cyc_Absyn_Aggrdecl*_tmp344;struct Cyc_Absyn_Aggrdecl**_tmp343;struct Cyc_Absyn_Aggrdecl**_tmp141=(_tmp343=_cycalloc(sizeof(*_tmp343)),((_tmp343[0]=((_tmp344=_cycalloc(sizeof(*_tmp344)),((_tmp344->kind=ad->kind,((_tmp344->sc=Cyc_Absyn_Extern,((_tmp344->name=ad->name,((_tmp344->tvs=_tmp127,((_tmp344->impl=0,((_tmp344->attributes=ad->attributes,_tmp344)))))))))))))),_tmp343)));
# 530
ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,_tmp11F,_tmp141);
# 533
Cyc_Tcutil_check_unique_tvars(loc,_tmp130);
# 535
Cyc_Tcutil_add_tvar_identities(_tmp130);
# 538
if(_tmp133  && ad->kind == Cyc_Absyn_StructA){
# 539
const char*_tmp347;void*_tmp346;(_tmp346=0,Cyc_Tcutil_terr(loc,((_tmp347="@tagged qualifier is only allowed on union declarations",_tag_dyneither(_tmp347,sizeof(char),56))),_tag_dyneither(_tmp346,sizeof(void*),0)));}
# 540
Cyc_Tc_tcAggrImpl(te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp127,_tmp130),_tmp131,_tmp132);
# 542
Cyc_Tc_rule_out_memkind(loc,_tmp11F,_tmp127,0);
# 545
Cyc_Tc_rule_out_memkind(loc,_tmp11F,_tmp130,1);
# 547
if(ad->kind == Cyc_Absyn_UnionA  && !_tmp133){
# 550
struct Cyc_List_List*f=_tmp132;for(0;f != 0;f=f->tl){
# 551
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)){
# 552
const char*_tmp34D;void*_tmp34C[3];struct Cyc_String_pa_PrintArg_struct _tmp34B;struct Cyc_String_pa_PrintArg_struct _tmp34A;struct Cyc_String_pa_PrintArg_struct _tmp349;(_tmp349.tag=0,((_tmp349.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 553
Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)),((_tmp34A.tag=0,((_tmp34A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp11F),((_tmp34B.tag=0,((_tmp34B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name),((_tmp34C[0]=& _tmp34B,((_tmp34C[1]=& _tmp34A,((_tmp34C[2]=& _tmp349,Cyc_Tcutil_warn(loc,((_tmp34D="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read",_tag_dyneither(_tmp34D,sizeof(char),98))),_tag_dyneither(_tmp34C,sizeof(void*),3)))))))))))))))))));}}}
# 555
*_tmp141=ad;
# 556
goto _LLAA;}_LLAF: _tmp135=_tmp12B.f1;if(_tmp135 == 0)goto _LLB1;_tmp136=*_tmp135;_tmp137=_tmp136.exist_vars;_tmp138=_tmp136.rgn_po;_tmp139=_tmp136.fields;_tmp13A=_tmp136.tagged;_tmp13B=_tmp12B.f2;if(_tmp13B == 0)goto _LLB1;_tmp13C=*_tmp13B;_LLB0:
# 559
 if(ad->kind != (*_tmp13C)->kind){
# 560
const char*_tmp350;void*_tmp34F;(_tmp34F=0,Cyc_Tcutil_terr(loc,((_tmp350="cannot reuse struct names for unions and vice-versa",_tag_dyneither(_tmp350,sizeof(char),52))),_tag_dyneither(_tmp34F,sizeof(void*),0)));}{
# 562
struct Cyc_Absyn_Aggrdecl*_tmp14D=*_tmp13C;
# 564
{struct Cyc_Absyn_Aggrdecl*_tmp351;*_tmp13C=((_tmp351=_cycalloc(sizeof(*_tmp351)),((_tmp351->kind=ad->kind,((_tmp351->sc=Cyc_Absyn_Extern,((_tmp351->name=ad->name,((_tmp351->tvs=_tmp127,((_tmp351->impl=0,((_tmp351->attributes=ad->attributes,_tmp351))))))))))))));}
# 568
Cyc_Tcutil_check_unique_tvars(loc,_tmp137);
# 570
Cyc_Tcutil_add_tvar_identities(_tmp137);
# 572
if(_tmp13A  && ad->kind == Cyc_Absyn_StructA){
# 573
const char*_tmp354;void*_tmp353;(_tmp353=0,Cyc_Tcutil_terr(loc,((_tmp354="@tagged qualifier is only allowed on union declarations",_tag_dyneither(_tmp354,sizeof(char),56))),_tag_dyneither(_tmp353,sizeof(void*),0)));}
# 574
Cyc_Tc_tcAggrImpl(te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp127,_tmp137),_tmp138,_tmp139);
# 576
*_tmp13C=_tmp14D;
# 577
_tmp13F=_tmp13C;goto _LLB2;};_LLB1: _tmp13D=_tmp12B.f1;if(_tmp13D != 0)goto _LLAA;_tmp13E=_tmp12B.f2;if(_tmp13E == 0)goto _LLAA;_tmp13F=*_tmp13E;_LLB2: {
# 580
struct Cyc_Absyn_Aggrdecl*_tmp151=Cyc_Tcdecl_merge_aggrdecl(*_tmp13F,ad,loc,Cyc_Tc_tc_msg);
# 581
if(_tmp151 == 0)
# 582
return;else{
# 584
Cyc_Tc_rule_out_memkind(loc,_tmp11F,_tmp127,0);
# 587
if(ad->impl != 0)
# 588
Cyc_Tc_rule_out_memkind(loc,_tmp11F,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);
# 590
*_tmp13F=(struct Cyc_Absyn_Aggrdecl*)_tmp151;
# 591
ad=(struct Cyc_Absyn_Aggrdecl*)_tmp151;
# 592
goto _LLAA;}}_LLAA:;}{
# 596
struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp35A;struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct _tmp359;struct _tuple13*_tmp358;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,_tmp11F,(struct _tuple13*)(
# 597
(_tmp358=_cycalloc(sizeof(*_tmp358)),((_tmp358->f1=(void*)((_tmp35A=_cycalloc(sizeof(*_tmp35A)),((_tmp35A[0]=((_tmp359.tag=1,((_tmp359.f1=ad,_tmp359)))),_tmp35A)))),((_tmp358->f2=1,_tmp358)))))));};};}
# 600
static void Cyc_Tc_rule_out_mem_and_unique(unsigned int loc,struct _dyneither_ptr*v,struct Cyc_List_List*tvs){
# 601
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 602
void*_tmp155=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);struct Cyc_Core_Opt*_tmp157;struct Cyc_Core_Opt**_tmp158;struct Cyc_Core_Opt*_tmp15A;struct Cyc_Core_Opt**_tmp15B;struct Cyc_Absyn_Kind*_tmp15C;struct Cyc_Absyn_Kind _tmp15D;enum Cyc_Absyn_KindQual _tmp15E;enum Cyc_Absyn_AliasQual _tmp15F;struct Cyc_Core_Opt*_tmp161;struct Cyc_Core_Opt**_tmp162;struct Cyc_Absyn_Kind*_tmp163;struct Cyc_Absyn_Kind _tmp164;enum Cyc_Absyn_KindQual _tmp165;enum Cyc_Absyn_AliasQual _tmp166;struct Cyc_Core_Opt*_tmp168;struct Cyc_Core_Opt**_tmp169;struct Cyc_Absyn_Kind*_tmp16A;struct Cyc_Absyn_Kind _tmp16B;enum Cyc_Absyn_KindQual _tmp16C;enum Cyc_Absyn_AliasQual _tmp16D;struct Cyc_Core_Opt*_tmp16F;struct Cyc_Core_Opt**_tmp170;struct Cyc_Absyn_Kind*_tmp171;struct Cyc_Absyn_Kind _tmp172;enum Cyc_Absyn_KindQual _tmp173;enum Cyc_Absyn_AliasQual _tmp174;struct Cyc_Core_Opt*_tmp176;struct Cyc_Core_Opt**_tmp177;struct Cyc_Absyn_Kind*_tmp178;struct Cyc_Absyn_Kind _tmp179;enum Cyc_Absyn_KindQual _tmp17A;enum Cyc_Absyn_AliasQual _tmp17B;struct Cyc_Absyn_Kind*_tmp17D;struct Cyc_Absyn_Kind _tmp17E;enum Cyc_Absyn_KindQual _tmp17F;enum Cyc_Absyn_AliasQual _tmp180;struct Cyc_Core_Opt*_tmp182;struct Cyc_Core_Opt**_tmp183;struct Cyc_Absyn_Kind*_tmp184;struct Cyc_Absyn_Kind _tmp185;enum Cyc_Absyn_KindQual _tmp186;enum Cyc_Absyn_AliasQual _tmp187;struct Cyc_Absyn_Kind*_tmp189;struct Cyc_Absyn_Kind _tmp18A;enum Cyc_Absyn_KindQual _tmp18B;enum Cyc_Absyn_AliasQual _tmp18C;struct Cyc_Absyn_Kind*_tmp18E;struct Cyc_Absyn_Kind _tmp18F;enum Cyc_Absyn_KindQual _tmp190;enum Cyc_Absyn_AliasQual _tmp191;_LLB4: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp156=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp155;if(_tmp156->tag != 1)goto _LLB6;else{_tmp157=_tmp156->f1;_tmp158=(struct Cyc_Core_Opt**)& _tmp156->f1;}}_LLB5:
# 603
 _tmp15B=_tmp158;goto _LLB7;_LLB6: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp159=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp155;if(_tmp159->tag != 2)goto _LLB8;else{_tmp15A=_tmp159->f1;_tmp15B=(struct Cyc_Core_Opt**)& _tmp159->f1;_tmp15C=_tmp159->f2;_tmp15D=*_tmp15C;_tmp15E=_tmp15D.kind;if(_tmp15E != Cyc_Absyn_MemKind)goto _LLB8;_tmp15F=_tmp15D.aliasqual;if(_tmp15F != Cyc_Absyn_Top)goto _LLB8;}}_LLB7:
# 605
 _tmp162=_tmp15B;goto _LLB9;_LLB8: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp160=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp155;if(_tmp160->tag != 2)goto _LLBA;else{_tmp161=_tmp160->f1;_tmp162=(struct Cyc_Core_Opt**)& _tmp160->f1;_tmp163=_tmp160->f2;_tmp164=*_tmp163;_tmp165=_tmp164.kind;if(_tmp165 != Cyc_Absyn_MemKind)goto _LLBA;_tmp166=_tmp164.aliasqual;if(_tmp166 != Cyc_Absyn_Aliasable)goto _LLBA;}}_LLB9:
# 607
{struct Cyc_Core_Opt*_tmp35B;*_tmp162=((_tmp35B=_cycalloc(sizeof(*_tmp35B)),((_tmp35B->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp35B))));}goto _LLB3;_LLBA: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp167=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp155;if(_tmp167->tag != 2)goto _LLBC;else{_tmp168=_tmp167->f1;_tmp169=(struct Cyc_Core_Opt**)& _tmp167->f1;_tmp16A=_tmp167->f2;_tmp16B=*_tmp16A;_tmp16C=_tmp16B.kind;if(_tmp16C != Cyc_Absyn_AnyKind)goto _LLBC;_tmp16D=_tmp16B.aliasqual;if(_tmp16D != Cyc_Absyn_Top)goto _LLBC;}}_LLBB:
# 608
 _tmp170=_tmp169;goto _LLBD;_LLBC: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp16E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp155;if(_tmp16E->tag != 2)goto _LLBE;else{_tmp16F=_tmp16E->f1;_tmp170=(struct Cyc_Core_Opt**)& _tmp16E->f1;_tmp171=_tmp16E->f2;_tmp172=*_tmp171;_tmp173=_tmp172.kind;if(_tmp173 != Cyc_Absyn_AnyKind)goto _LLBE;_tmp174=_tmp172.aliasqual;if(_tmp174 != Cyc_Absyn_Aliasable)goto _LLBE;}}_LLBD:
# 610
{struct Cyc_Core_Opt*_tmp35C;*_tmp170=((_tmp35C=_cycalloc(sizeof(*_tmp35C)),((_tmp35C->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak),_tmp35C))));}goto _LLB3;_LLBE: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp175=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp155;if(_tmp175->tag != 2)goto _LLC0;else{_tmp176=_tmp175->f1;_tmp177=(struct Cyc_Core_Opt**)& _tmp175->f1;_tmp178=_tmp175->f2;_tmp179=*_tmp178;_tmp17A=_tmp179.kind;if(_tmp17A != Cyc_Absyn_RgnKind)goto _LLC0;_tmp17B=_tmp179.aliasqual;if(_tmp17B != Cyc_Absyn_Top)goto _LLC0;}}_LLBF:
# 612
{struct Cyc_Core_Opt*_tmp35D;*_tmp177=((_tmp35D=_cycalloc(sizeof(*_tmp35D)),((_tmp35D->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmp35D))));}goto _LLB3;_LLC0: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp17C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp155;if(_tmp17C->tag != 0)goto _LLC2;else{_tmp17D=_tmp17C->f1;_tmp17E=*_tmp17D;_tmp17F=_tmp17E.kind;if(_tmp17F != Cyc_Absyn_RgnKind)goto _LLC2;_tmp180=_tmp17E.aliasqual;if(_tmp180 != Cyc_Absyn_Top)goto _LLC2;}}_LLC1:
# 614
{const char*_tmp362;void*_tmp361[2];struct Cyc_String_pa_PrintArg_struct _tmp360;struct Cyc_String_pa_PrintArg_struct _tmp35F;(_tmp35F.tag=0,((_tmp35F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp360.tag=0,((_tmp360.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp361[0]=& _tmp360,((_tmp361[1]=& _tmp35F,Cyc_Tcutil_terr(loc,((_tmp362="type %s attempts to abstract type variable %s of kind TR",_tag_dyneither(_tmp362,sizeof(char),57))),_tag_dyneither(_tmp361,sizeof(void*),2)))))))))))));}
# 615
goto _LLB3;_LLC2: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp181=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp155;if(_tmp181->tag != 2)goto _LLC4;else{_tmp182=_tmp181->f1;_tmp183=(struct Cyc_Core_Opt**)& _tmp181->f1;_tmp184=_tmp181->f2;_tmp185=*_tmp184;_tmp186=_tmp185.kind;_tmp187=_tmp185.aliasqual;if(_tmp187 != Cyc_Absyn_Unique)goto _LLC4;}}_LLC3:
# 616
 _tmp18B=_tmp186;goto _LLC5;_LLC4: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp188=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp155;if(_tmp188->tag != 0)goto _LLC6;else{_tmp189=_tmp188->f1;_tmp18A=*_tmp189;_tmp18B=_tmp18A.kind;_tmp18C=_tmp18A.aliasqual;if(_tmp18C != Cyc_Absyn_Unique)goto _LLC6;}}_LLC5:
# 618
{const char*_tmp36A;void*_tmp369[3];struct Cyc_String_pa_PrintArg_struct _tmp368;struct Cyc_String_pa_PrintArg_struct _tmp367;struct Cyc_Absyn_Kind*_tmp366;struct Cyc_String_pa_PrintArg_struct _tmp365;(_tmp365.tag=0,((_tmp365.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 620
Cyc_Absynpp_kind2string(((_tmp366=_cycalloc_atomic(sizeof(*_tmp366)),((_tmp366->kind=_tmp18B,((_tmp366->aliasqual=Cyc_Absyn_Unique,_tmp366)))))))),((_tmp367.tag=0,((_tmp367.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp368.tag=0,((_tmp368.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp369[0]=& _tmp368,((_tmp369[1]=& _tmp367,((_tmp369[2]=& _tmp365,Cyc_Tcutil_terr(loc,((_tmp36A="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp36A,sizeof(char),57))),_tag_dyneither(_tmp369,sizeof(void*),3)))))))))))))))))));}goto _LLB3;_LLC6: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp18D=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp155;if(_tmp18D->tag != 0)goto _LLC8;else{_tmp18E=_tmp18D->f1;_tmp18F=*_tmp18E;_tmp190=_tmp18F.kind;if(_tmp190 != Cyc_Absyn_MemKind)goto _LLC8;_tmp191=_tmp18F.aliasqual;}}_LLC7:
# 622
{const char*_tmp372;void*_tmp371[3];struct Cyc_String_pa_PrintArg_struct _tmp370;struct Cyc_String_pa_PrintArg_struct _tmp36F;struct Cyc_Absyn_Kind*_tmp36E;struct Cyc_String_pa_PrintArg_struct _tmp36D;(_tmp36D.tag=0,((_tmp36D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 624
Cyc_Absynpp_kind2string(((_tmp36E=_cycalloc_atomic(sizeof(*_tmp36E)),((_tmp36E->kind=Cyc_Absyn_MemKind,((_tmp36E->aliasqual=_tmp191,_tmp36E)))))))),((_tmp36F.tag=0,((_tmp36F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp370.tag=0,((_tmp370.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp371[0]=& _tmp370,((_tmp371[1]=& _tmp36F,((_tmp371[2]=& _tmp36D,Cyc_Tcutil_terr(loc,((_tmp372="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp372,sizeof(char),57))),_tag_dyneither(_tmp371,sizeof(void*),3)))))))))))))))))));}goto _LLB3;_LLC8:;_LLC9:
# 625
 goto _LLB3;_LLB3:;}}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 631
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct _dyneither_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){
# 640
{struct Cyc_List_List*_tmp1A5=fields;for(0;_tmp1A5 != 0;_tmp1A5=_tmp1A5->tl){
# 641
struct Cyc_Absyn_Datatypefield*_tmp1A6=(struct Cyc_Absyn_Datatypefield*)_tmp1A5->hd;
# 643
{struct Cyc_List_List*typs=_tmp1A6->typs;for(0;typs != 0;typs=typs->tl){
# 644
Cyc_Tcutil_check_type(_tmp1A6->loc,te,tvs,& Cyc_Tcutil_tmk,0,(*((struct _tuple15*)typs->hd)).f2);{
# 646
struct _RegionHandle _tmp1A7=_new_region("temp");struct _RegionHandle*temp=& _tmp1A7;_push_region(temp);
# 647
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(*((struct _tuple15*)typs->hd)).f2)){
# 648
const char*_tmp376;void*_tmp375[1];struct Cyc_String_pa_PrintArg_struct _tmp374;(_tmp374.tag=0,((_tmp374.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 650
Cyc_Absynpp_qvar2string(_tmp1A6->name)),((_tmp375[0]=& _tmp374,Cyc_Tcutil_terr(_tmp1A6->loc,((_tmp376="noalias pointers in datatypes are not allowed (%s)",_tag_dyneither(_tmp376,sizeof(char),51))),_tag_dyneither(_tmp375,sizeof(void*),1)))))));}
# 652
((*((struct _tuple15*)typs->hd)).f1).real_const=
# 653
Cyc_Tcutil_extract_const_from_typedef(_tmp1A6->loc,((*((struct _tuple15*)typs->hd)).f1).print_const,(*((struct _tuple15*)typs->hd)).f2);
# 646
;_pop_region(temp);};}}{
# 657
union Cyc_Absyn_Nmspace _tmp1AB=(*_tmp1A6->name).f1;struct Cyc_List_List*_tmp1AC;struct Cyc_List_List*_tmp1AD;struct Cyc_List_List*_tmp1AE;struct Cyc_List_List*_tmp1AF;int _tmp1B0;_LLCB: if((_tmp1AB.Rel_n).tag != 1)goto _LLCD;_tmp1AC=(struct Cyc_List_List*)(_tmp1AB.Rel_n).val;if(_tmp1AC != 0)goto _LLCD;_LLCC:
# 659
 if(is_extensible)
# 660
(*_tmp1A6->name).f1=Cyc_Absyn_Abs_n(te->ns,0);else{
# 662
(*_tmp1A6->name).f1=(*name).f1;}
# 663
goto _LLCA;_LLCD: if((_tmp1AB.Rel_n).tag != 1)goto _LLCF;_tmp1AD=(struct Cyc_List_List*)(_tmp1AB.Rel_n).val;_LLCE:
# 665
{const char*_tmp37A;void*_tmp379[1];struct Cyc_String_pa_PrintArg_struct _tmp378;(_tmp378.tag=0,((_tmp378.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 666
Cyc_Absynpp_qvar2string(_tmp1A6->name)),((_tmp379[0]=& _tmp378,Cyc_Tcutil_terr(_tmp1A6->loc,((_tmp37A="qualified datatypefield declarations are not allowed (%s)",_tag_dyneither(_tmp37A,sizeof(char),58))),_tag_dyneither(_tmp379,sizeof(void*),1)))))));}
# 667
goto _LLCA;_LLCF: if((_tmp1AB.C_n).tag != 3)goto _LLD1;_tmp1AE=(struct Cyc_List_List*)(_tmp1AB.C_n).val;_LLD0:
# 668
 goto _LLCA;_LLD1: if((_tmp1AB.Abs_n).tag != 2)goto _LLD3;_tmp1AF=(struct Cyc_List_List*)(_tmp1AB.Abs_n).val;_LLD2:
# 669
 goto _LLCA;_LLD3: if((_tmp1AB.Loc_n).tag != 4)goto _LLCA;_tmp1B0=(int)(_tmp1AB.Loc_n).val;_LLD4: {
# 670
const char*_tmp37D;void*_tmp37C;(_tmp37C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp37D="tcDatatypeFields: Loc_n",_tag_dyneither(_tmp37D,sizeof(char),24))),_tag_dyneither(_tmp37C,sizeof(void*),0)));}_LLCA:;};}}{
# 674
struct Cyc_List_List*fields2;
# 675
if(is_extensible){
# 677
int _tmp1B6=1;
# 678
struct Cyc_List_List*_tmp1B7=Cyc_Tcdecl_sort_xdatatype_fields(fields,& _tmp1B6,(*name).f2,loc,Cyc_Tc_tc_msg);
# 679
if(_tmp1B6)
# 680
fields2=_tmp1B7;else{
# 682
fields2=0;}}else{
# 684
struct _RegionHandle _tmp1B8=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp1B8;_push_region(uprev_rgn);
# 686
{struct Cyc_List_List*prev_fields=0;
# 687
{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){
# 688
struct Cyc_Absyn_Datatypefield*_tmp1B9=(struct Cyc_Absyn_Datatypefield*)fs->hd;
# 689
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp1B9->name).f2)){
# 690
const char*_tmp382;void*_tmp381[2];struct Cyc_String_pa_PrintArg_struct _tmp380;struct Cyc_String_pa_PrintArg_struct _tmp37F;(_tmp37F.tag=0,((_tmp37F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj),((_tmp380.tag=0,((_tmp380.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1B9->name).f2),((_tmp381[0]=& _tmp380,((_tmp381[1]=& _tmp37F,Cyc_Tcutil_terr(_tmp1B9->loc,((_tmp382="duplicate field name %s in %s",_tag_dyneither(_tmp382,sizeof(char),30))),_tag_dyneither(_tmp381,sizeof(void*),2)))))))))))));}else{
# 692
struct Cyc_List_List*_tmp383;prev_fields=((_tmp383=_region_malloc(uprev_rgn,sizeof(*_tmp383)),((_tmp383->hd=(*_tmp1B9->name).f2,((_tmp383->tl=prev_fields,_tmp383))))));}
# 694
if(_tmp1B9->sc != Cyc_Absyn_Public){
# 695
{const char*_tmp387;void*_tmp386[1];struct Cyc_String_pa_PrintArg_struct _tmp385;(_tmp385.tag=0,((_tmp385.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1B9->name).f2),((_tmp386[0]=& _tmp385,Cyc_Tcutil_warn(loc,((_tmp387="ignoring scope of field %s",_tag_dyneither(_tmp387,sizeof(char),27))),_tag_dyneither(_tmp386,sizeof(void*),1)))))));}
# 696
_tmp1B9->sc=Cyc_Absyn_Public;}}}
# 699
fields2=fields;}
# 686
;_pop_region(uprev_rgn);}
# 705
{struct Cyc_List_List*_tmp1C2=fields;for(0;_tmp1C2 != 0;_tmp1C2=_tmp1C2->tl){
# 706
struct Cyc_Absyn_Datatypefield*_tmp1C3=(struct Cyc_Absyn_Datatypefield*)_tmp1C2->hd;
# 707
struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp38D;struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp38C;struct _tuple13*_tmp38B;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp1C3->name).f2,(struct _tuple13*)(
# 708
(_tmp38B=_cycalloc(sizeof(*_tmp38B)),((_tmp38B->f1=(void*)((_tmp38D=_cycalloc(sizeof(*_tmp38D)),((_tmp38D[0]=((_tmp38C.tag=2,((_tmp38C.f1=tudres,((_tmp38C.f2=_tmp1C3,_tmp38C)))))),_tmp38D)))),((_tmp38B->f2=1,_tmp38B)))))));}}
# 711
return fields2;};}struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};
# 714
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Datatypedecl*tud){
# 715
struct _dyneither_ptr*v=(*tud->name).f2;
# 716
const char*_tmp38F;const char*_tmp38E;struct _dyneither_ptr obj=tud->is_extensible?(_tmp38F="@extensible datatype",_tag_dyneither(_tmp38F,sizeof(char),21)):((_tmp38E="datatype",_tag_dyneither(_tmp38E,sizeof(char),9)));
# 721
struct Cyc_List_List*tvs=tud->tvs;
# 724
Cyc_Tcutil_check_unique_tvars(loc,tvs);
# 726
Cyc_Tcutil_add_tvar_identities(tvs);{
# 731
struct _RegionHandle _tmp1C7=_new_region("temp");struct _RegionHandle*temp=& _tmp1C7;_push_region(temp);
# 732
{struct Cyc_Absyn_Datatypedecl***tud_opt;
# 741 "tc.cyc"
{struct _handler_cons _tmp1C8;_push_handler(& _tmp1C8);{int _tmp1CA=0;if(setjmp(_tmp1C8.handler))_tmp1CA=1;if(!_tmp1CA){
# 742
tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(temp,te,loc,tud->name);
# 743
if(tud_opt == 0  && !tud->is_extensible){struct Cyc_Dict_Absent_exn_struct _tmp392;struct Cyc_Dict_Absent_exn_struct*_tmp391;(int)_throw((void*)((_tmp391=_cycalloc_atomic(sizeof(*_tmp391)),((_tmp391[0]=((_tmp392.tag=Cyc_Dict_Absent,_tmp392)),_tmp391)))));}
# 744
if(tud_opt != 0)
# 745
tud->name=(*(*tud_opt))->name;else{
# 747
(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns,0);}
# 742
;_pop_handler();}else{void*_tmp1C9=(void*)_exn_thrown;void*_tmp1CE=_tmp1C9;_LLD6: {struct Cyc_Dict_Absent_exn_struct*_tmp1CF=(struct Cyc_Dict_Absent_exn_struct*)_tmp1CE;if(_tmp1CF->tag != Cyc_Dict_Absent)goto _LLD8;}_LLD7:
# 751
{union Cyc_Absyn_Nmspace _tmp1D0=(*tud->name).f1;struct Cyc_List_List*_tmp1D1;_LLDB: if((_tmp1D0.Rel_n).tag != 1)goto _LLDD;_tmp1D1=(struct Cyc_List_List*)(_tmp1D0.Rel_n).val;if(_tmp1D1 != 0)goto _LLDD;_LLDC:
# 752
(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LLDA;_LLDD:;_LLDE:
# 753
 goto _LLDA;_LLDA:;}{
# 755
struct Cyc_Absyn_Datatypedecl***_tmp1D2=((struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->datatypedecls,v);
# 756
{struct Cyc_Absyn_Datatypedecl***_tmp393;tud_opt=(unsigned int)_tmp1D2?(_tmp393=_region_malloc(temp,sizeof(*_tmp393)),((_tmp393[0]=*_tmp1D2,_tmp393))): 0;}
# 757
goto _LLD5;};_LLD8:;_LLD9:(void)_throw(_tmp1CE);_LLD5:;}};}{
# 762
struct _tuple16 _tmp394;struct _tuple16 _tmp1D5=(_tmp394.f1=tud->fields,((_tmp394.f2=tud_opt,_tmp394)));struct Cyc_Core_Opt*_tmp1D6;struct Cyc_Absyn_Datatypedecl***_tmp1D7;struct Cyc_Core_Opt*_tmp1D8;struct Cyc_Core_Opt _tmp1D9;struct Cyc_List_List*_tmp1DA;struct Cyc_List_List**_tmp1DB;struct Cyc_Absyn_Datatypedecl***_tmp1DC;struct Cyc_Core_Opt*_tmp1DD;struct Cyc_Core_Opt _tmp1DE;struct Cyc_List_List*_tmp1DF;struct Cyc_List_List**_tmp1E0;struct Cyc_Absyn_Datatypedecl***_tmp1E1;struct Cyc_Absyn_Datatypedecl**_tmp1E2;struct Cyc_Core_Opt*_tmp1E3;struct Cyc_Absyn_Datatypedecl***_tmp1E4;struct Cyc_Absyn_Datatypedecl**_tmp1E5;_LLE0: _tmp1D6=_tmp1D5.f1;if(_tmp1D6 != 0)goto _LLE2;_tmp1D7=_tmp1D5.f2;if(_tmp1D7 != 0)goto _LLE2;_LLE1:
# 765
 Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
# 766
{struct Cyc_Absyn_Datatypedecl**_tmp395;ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(ge->datatypedecls,v,((_tmp395=_cycalloc(sizeof(*_tmp395)),((_tmp395[0]=tud,_tmp395)))));}
# 767
goto _LLDF;_LLE2: _tmp1D8=_tmp1D5.f1;if(_tmp1D8 == 0)goto _LLE4;_tmp1D9=*_tmp1D8;_tmp1DA=(struct Cyc_List_List*)_tmp1D9.v;_tmp1DB=(struct Cyc_List_List**)&(*_tmp1D5.f1).v;_tmp1DC=_tmp1D5.f2;if(_tmp1DC != 0)goto _LLE4;_LLE3: {
# 770
struct Cyc_Absyn_Datatypedecl*_tmp398;struct Cyc_Absyn_Datatypedecl**_tmp397;struct Cyc_Absyn_Datatypedecl**_tmp1E7=(_tmp397=_cycalloc(sizeof(*_tmp397)),((_tmp397[0]=((_tmp398=_cycalloc(sizeof(*_tmp398)),((_tmp398->sc=Cyc_Absyn_Extern,((_tmp398->name=tud->name,((_tmp398->tvs=tvs,((_tmp398->fields=0,((_tmp398->is_extensible=tud->is_extensible,_tmp398)))))))))))),_tmp397)));
# 772
ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(ge->datatypedecls,v,_tmp1E7);
# 775
*_tmp1DB=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,tud->name,*_tmp1DB,tvs,tud);
# 776
Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
# 777
*_tmp1E7=tud;
# 778
goto _LLDF;}_LLE4: _tmp1DD=_tmp1D5.f1;if(_tmp1DD == 0)goto _LLE6;_tmp1DE=*_tmp1DD;_tmp1DF=(struct Cyc_List_List*)_tmp1DE.v;_tmp1E0=(struct Cyc_List_List**)&(*_tmp1D5.f1).v;_tmp1E1=_tmp1D5.f2;if(_tmp1E1 == 0)goto _LLE6;_tmp1E2=*_tmp1E1;_LLE5: {
# 780
struct Cyc_Absyn_Datatypedecl*_tmp1EA=*_tmp1E2;
# 783
if((!tud->is_extensible  && (unsigned int)_tmp1EA) && _tmp1EA->is_extensible)
# 784
tud->is_extensible=1;
# 786
{struct Cyc_Absyn_Datatypedecl*_tmp399;*_tmp1E2=((_tmp399=_cycalloc(sizeof(*_tmp399)),((_tmp399->sc=Cyc_Absyn_Extern,((_tmp399->name=tud->name,((_tmp399->tvs=tvs,((_tmp399->fields=0,((_tmp399->is_extensible=tud->is_extensible,_tmp399))))))))))));}
# 790
*_tmp1E0=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,tud->name,*_tmp1E0,tvs,tud);
# 794
*_tmp1E2=_tmp1EA;
# 795
_tmp1E5=_tmp1E2;goto _LLE7;}_LLE6: _tmp1E3=_tmp1D5.f1;if(_tmp1E3 != 0)goto _LLDF;_tmp1E4=_tmp1D5.f2;if(_tmp1E4 == 0)goto _LLDF;_tmp1E5=*_tmp1E4;_LLE7: {
# 798
struct Cyc_Absyn_Datatypedecl*_tmp1EC=Cyc_Tcdecl_merge_datatypedecl(*_tmp1E5,tud,loc,Cyc_Tc_tc_msg);
# 799
Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
# 800
if(_tmp1EC == 0){
# 801
_npop_handler(0);return;}else{
# 803
*_tmp1E5=(struct Cyc_Absyn_Datatypedecl*)_tmp1EC;
# 804
goto _LLDF;}}_LLDF:;};}
# 732 "tc.cyc"
;_pop_region(temp);};}
# 810 "tc.cyc"
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Enumdecl*ed){
# 811
struct _dyneither_ptr*v=(*ed->name).f2;
# 817
{union Cyc_Absyn_Nmspace _tmp1EF=(*ed->name).f1;struct Cyc_List_List*_tmp1F0;_LLE9: if((_tmp1EF.Rel_n).tag != 1)goto _LLEB;_tmp1F0=(struct Cyc_List_List*)(_tmp1EF.Rel_n).val;if(_tmp1F0 != 0)goto _LLEB;_LLEA:
# 820
(*ed->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);
# 821
goto _LLE8;_LLEB:;_LLEC:
# 822
 goto _LLE8;_LLE8:;}
# 826
if(ed->fields != 0){struct _RegionHandle _tmp1F1=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp1F1;_push_region(uprev_rgn);
# 827
{struct Cyc_List_List*prev_fields=0;
# 828
unsigned int tag_count=0;
# 829
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
# 830
struct Cyc_Absyn_Enumfield*_tmp1F2=(struct Cyc_Absyn_Enumfield*)fs->hd;
# 832
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp1F2->name).f2)){
# 833
const char*_tmp39D;void*_tmp39C[1];struct Cyc_String_pa_PrintArg_struct _tmp39B;(_tmp39B.tag=0,((_tmp39B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1F2->name).f2),((_tmp39C[0]=& _tmp39B,Cyc_Tcutil_terr(_tmp1F2->loc,((_tmp39D="duplicate field name %s",_tag_dyneither(_tmp39D,sizeof(char),24))),_tag_dyneither(_tmp39C,sizeof(void*),1)))))));}else{
# 835
struct Cyc_List_List*_tmp39E;prev_fields=((_tmp39E=_region_malloc(uprev_rgn,sizeof(*_tmp39E)),((_tmp39E->hd=(*_tmp1F2->name).f2,((_tmp39E->tl=prev_fields,_tmp39E))))));}
# 837
if(((struct _tuple13**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->ordinaries,(*_tmp1F2->name).f2)!= 0){
# 838
const char*_tmp3A2;void*_tmp3A1[1];struct Cyc_String_pa_PrintArg_struct _tmp3A0;(_tmp3A0.tag=0,((_tmp3A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1F2->name).f2),((_tmp3A1[0]=& _tmp3A0,Cyc_Tcutil_terr(_tmp1F2->loc,((_tmp3A2="enum field name %s shadows global name",_tag_dyneither(_tmp3A2,sizeof(char),39))),_tag_dyneither(_tmp3A1,sizeof(void*),1)))))));}
# 840
if(_tmp1F2->tag == 0){
# 841
_tmp1F2->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmp1F2->loc);
# 842
++ tag_count;}else{
# 845
if(Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_check_null(_tmp1F2->tag))){
# 846
unsigned int _tmp1FB;int _tmp1FC;struct _tuple11 _tmp1FA=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1F2->tag));_tmp1FB=_tmp1FA.f1;_tmp1FC=_tmp1FA.f2;
# 847
if(_tmp1FC)tag_count=_tmp1FB + 1;}}
# 849
(*_tmp1F2->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);}}
# 827
;_pop_region(uprev_rgn);}
# 854
{struct _handler_cons _tmp1FD;_push_handler(& _tmp1FD);{int _tmp1FF=0;if(setjmp(_tmp1FD.handler))_tmp1FF=1;if(!_tmp1FF){
# 855
{struct Cyc_Absyn_Enumdecl**_tmp200=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->enumdecls,v);
# 856
struct Cyc_Absyn_Enumdecl*_tmp201=Cyc_Tcdecl_merge_enumdecl(*_tmp200,ed,loc,Cyc_Tc_tc_msg);
# 857
if(_tmp201 == 0){_npop_handler(0);return;}
# 858
*_tmp200=(struct Cyc_Absyn_Enumdecl*)_tmp201;}
# 855
;_pop_handler();}else{void*_tmp1FE=(void*)_exn_thrown;void*_tmp203=_tmp1FE;_LLEE: {struct Cyc_Dict_Absent_exn_struct*_tmp204=(struct Cyc_Dict_Absent_exn_struct*)_tmp203;if(_tmp204->tag != Cyc_Dict_Absent)goto _LLF0;}_LLEF: {
# 860
struct Cyc_Absyn_Enumdecl**_tmp3A3;struct Cyc_Absyn_Enumdecl**_tmp205=(_tmp3A3=_cycalloc(sizeof(*_tmp3A3)),((_tmp3A3[0]=ed,_tmp3A3)));
# 861
ge->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)(ge->enumdecls,v,_tmp205);
# 862
goto _LLED;}_LLF0:;_LLF1:(void)_throw(_tmp203);_LLED:;}};}
# 864
if(ed->fields != 0){
# 865
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
# 866
struct Cyc_Absyn_Enumfield*_tmp207=(struct Cyc_Absyn_Enumfield*)fs->hd;
# 867
struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp3A9;struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct _tmp3A8;struct _tuple13*_tmp3A7;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp207->name).f2,(struct _tuple13*)(
# 868
(_tmp3A7=_cycalloc(sizeof(*_tmp3A7)),((_tmp3A7->f1=(void*)((_tmp3A9=_cycalloc(sizeof(*_tmp3A9)),((_tmp3A9[0]=((_tmp3A8.tag=3,((_tmp3A8.f1=ed,((_tmp3A8.f2=_tmp207,_tmp3A8)))))),_tmp3A9)))),((_tmp3A7->f2=1,_tmp3A7)))))));}}
# 873
if(ed->fields != 0){
# 874
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
# 875
struct Cyc_Absyn_Enumfield*_tmp20B=(struct Cyc_Absyn_Enumfield*)fs->hd;
# 876
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmp20B->tag));
# 877
if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_check_null(_tmp20B->tag))){
# 878
const char*_tmp3AE;void*_tmp3AD[2];struct Cyc_String_pa_PrintArg_struct _tmp3AC;struct Cyc_String_pa_PrintArg_struct _tmp3AB;(_tmp3AB.tag=0,((_tmp3AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp20B->name).f2),((_tmp3AC.tag=0,((_tmp3AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3AD[0]=& _tmp3AC,((_tmp3AD[1]=& _tmp3AB,Cyc_Tcutil_terr(loc,((_tmp3AE="enum %s, field %s: expression is not constant",_tag_dyneither(_tmp3AE,sizeof(char),46))),_tag_dyneither(_tmp3AD,sizeof(void*),2)))))))))))));}}}}
# 883
static int Cyc_Tc_okay_externC(unsigned int loc,enum Cyc_Absyn_Scope sc,int in_include){
# 884
switch(sc){case Cyc_Absyn_Static: _LLF2:
# 886
 if(!in_include){
# 887
const char*_tmp3B1;void*_tmp3B0;(_tmp3B0=0,Cyc_Tcutil_warn(loc,((_tmp3B1="static declaration nested within extern \"C\"",_tag_dyneither(_tmp3B1,sizeof(char),44))),_tag_dyneither(_tmp3B0,sizeof(void*),0)));}
# 888
return 0;case Cyc_Absyn_Abstract: _LLF3:
# 890
{const char*_tmp3B4;void*_tmp3B3;(_tmp3B3=0,Cyc_Tcutil_warn(loc,((_tmp3B4="abstract declaration nested within extern \"C\"",_tag_dyneither(_tmp3B4,sizeof(char),46))),_tag_dyneither(_tmp3B3,sizeof(void*),0)));}
# 891
return 0;case Cyc_Absyn_Public: _LLF4:
# 892
 goto _LLF5;case Cyc_Absyn_Register: _LLF5:
# 893
 goto _LLF6;case Cyc_Absyn_Extern: _LLF6:
# 894
 return 1;case Cyc_Absyn_ExternC: _LLF7:
# 896
{const char*_tmp3B7;void*_tmp3B6;(_tmp3B6=0,Cyc_Tcutil_warn(loc,((_tmp3B7="nested extern \"C\" declaration",_tag_dyneither(_tmp3B7,sizeof(char),30))),_tag_dyneither(_tmp3B6,sizeof(void*),0)));}
# 897
return 1;}}
# 901
static void Cyc_Tc_resolve_export_namespace(struct Cyc_Tcenv_Tenv*te,struct _tuple12*exp){
# 902
unsigned int _tmp217;struct _tuple0*_tmp218;struct _tuple12 _tmp216=*exp;_tmp217=_tmp216.f1;_tmp218=_tmp216.f2;{
# 903
struct _tuple0 _tmp21A;union Cyc_Absyn_Nmspace _tmp21B;struct _dyneither_ptr*_tmp21C;struct _tuple0*_tmp219=_tmp218;_tmp21A=*_tmp219;_tmp21B=_tmp21A.f1;_tmp21C=_tmp21A.f2;{
# 904
union Cyc_Absyn_Nmspace _tmp21D=_tmp21B;struct Cyc_List_List*_tmp21E;_LLFA: if((_tmp21D.Rel_n).tag != 1)goto _LLFC;_tmp21E=(struct Cyc_List_List*)(_tmp21D.Rel_n).val;if(_tmp21E != 0)goto _LLFC;_LLFB:
# 907
(*_tmp218).f1=Cyc_Absyn_Abs_n(te->ns,1);
# 908
goto _LLF9;_LLFC:;_LLFD:
# 909
 goto _LLF9;_LLF9:;};};}
# 918
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int in_externCinclude,int check_var_init,struct _RegionHandle*grgn,struct Cyc_List_List**exports){
# 925
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
# 926
struct Cyc_List_List*last=0;
# 928
struct Cyc_List_List*_tmp21F=ds0;for(0;_tmp21F != 0;(last=_tmp21F,_tmp21F=_tmp21F->tl)){
# 929
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp21F->hd;
# 930
unsigned int loc=d->loc;
# 932
void*_tmp220=d->r;struct Cyc_Absyn_Vardecl*_tmp226;struct Cyc_Absyn_Fndecl*_tmp228;struct Cyc_Absyn_Typedefdecl*_tmp22A;struct Cyc_Absyn_Aggrdecl*_tmp22C;struct Cyc_Absyn_Datatypedecl*_tmp22E;struct Cyc_Absyn_Enumdecl*_tmp230;struct _dyneither_ptr*_tmp234;struct Cyc_List_List*_tmp235;struct _tuple0*_tmp237;struct _tuple0 _tmp238;union Cyc_Absyn_Nmspace _tmp239;struct _dyneither_ptr*_tmp23A;struct Cyc_List_List*_tmp23B;struct Cyc_List_List*_tmp23D;struct Cyc_List_List*_tmp23F;struct Cyc_List_List*_tmp240;_LLFF: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp221=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp220;if(_tmp221->tag != 2)goto _LL101;}_LL100:
# 933
 goto _LL102;_LL101: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp222=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp220;if(_tmp222->tag != 3)goto _LL103;}_LL102:
# 935
{const char*_tmp3BA;void*_tmp3B9;(_tmp3B9=0,Cyc_Tcutil_terr(loc,((_tmp3BA="top level let-declarations are not implemented",_tag_dyneither(_tmp3BA,sizeof(char),47))),_tag_dyneither(_tmp3B9,sizeof(void*),0)));}
# 936
goto _LLFE;_LL103: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp223=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp220;if(_tmp223->tag != 4)goto _LL105;}_LL104:
# 938
{const char*_tmp3BD;void*_tmp3BC;(_tmp3BC=0,Cyc_Tcutil_terr(loc,((_tmp3BD="top level region declarations are not implemented",_tag_dyneither(_tmp3BD,sizeof(char),50))),_tag_dyneither(_tmp3BC,sizeof(void*),0)));}
# 939
goto _LLFE;_LL105: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp224=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp220;if(_tmp224->tag != 5)goto _LL107;}_LL106:
# 941
{const char*_tmp3C0;void*_tmp3BF;(_tmp3BF=0,Cyc_Tcutil_terr(loc,((_tmp3C0="top level alias declarations are not implemented",_tag_dyneither(_tmp3C0,sizeof(char),49))),_tag_dyneither(_tmp3BF,sizeof(void*),0)));}
# 942
goto _LLFE;_LL107: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp225=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp220;if(_tmp225->tag != 0)goto _LL109;else{_tmp226=_tmp225->f1;}}_LL108:
# 944
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp226->sc,in_externCinclude))
# 945
_tmp226->sc=Cyc_Absyn_ExternC;
# 946
Cyc_Tc_tcVardecl(te,ge,loc,_tmp226,check_var_init,in_externCinclude,exports);
# 947
goto _LLFE;_LL109: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp227=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp220;if(_tmp227->tag != 1)goto _LL10B;else{_tmp228=_tmp227->f1;}}_LL10A:
# 949
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp228->sc,in_externCinclude))
# 950
_tmp228->sc=Cyc_Absyn_ExternC;
# 951
Cyc_Tc_tcFndecl(te,ge,loc,_tmp228,in_externCinclude,exports);
# 952
goto _LLFE;_LL10B: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp229=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp220;if(_tmp229->tag != 9)goto _LL10D;else{_tmp22A=_tmp229->f1;}}_LL10C:
# 954
 Cyc_Tc_tcTypedefdecl(te,ge,loc,_tmp22A);
# 955
goto _LLFE;_LL10D: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp22B=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp220;if(_tmp22B->tag != 6)goto _LL10F;else{_tmp22C=_tmp22B->f1;}}_LL10E:
# 957
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp22C->sc,in_externCinclude))
# 958
_tmp22C->sc=Cyc_Absyn_ExternC;
# 959
Cyc_Tc_tcAggrdecl(te,ge,loc,_tmp22C);
# 960
goto _LLFE;_LL10F: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp22D=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp220;if(_tmp22D->tag != 7)goto _LL111;else{_tmp22E=_tmp22D->f1;}}_LL110:
# 962
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp22E->sc,in_externCinclude))
# 963
_tmp22E->sc=Cyc_Absyn_ExternC;
# 964
Cyc_Tc_tcDatatypedecl(te,ge,loc,_tmp22E);
# 965
goto _LLFE;_LL111: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp22F=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp220;if(_tmp22F->tag != 8)goto _LL113;else{_tmp230=_tmp22F->f1;}}_LL112:
# 967
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp230->sc,in_externCinclude))
# 968
_tmp230->sc=Cyc_Absyn_ExternC;
# 969
Cyc_Tc_tcEnumdecl(te,ge,loc,_tmp230);
# 970
goto _LLFE;_LL113: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp231=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp220;if(_tmp231->tag != 14)goto _LL115;}_LL114:
# 972
{const char*_tmp3C3;void*_tmp3C2;(_tmp3C2=0,Cyc_Tcutil_warn(d->loc,((_tmp3C3="spurious __cyclone_port_on__",_tag_dyneither(_tmp3C3,sizeof(char),29))),_tag_dyneither(_tmp3C2,sizeof(void*),0)));}
# 973
goto _LLFE;_LL115: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp232=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp220;if(_tmp232->tag != 15)goto _LL117;}_LL116:
# 975
 goto _LLFE;_LL117: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp233=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp220;if(_tmp233->tag != 10)goto _LL119;else{_tmp234=_tmp233->f1;_tmp235=_tmp233->f2;}}_LL118: {
# 978
struct Cyc_List_List*_tmp249=te->ns;
# 979
struct Cyc_List_List*_tmp3C4;struct Cyc_List_List*_tmp24A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp249,((_tmp3C4=_cycalloc(sizeof(*_tmp3C4)),((_tmp3C4->hd=_tmp234,((_tmp3C4->tl=0,_tmp3C4)))))));
# 982
if(!((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ge->namespaces,_tmp234)){
# 983
ge->namespaces=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(grgn,ge->namespaces,_tmp234);
# 984
te->ae=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(te->ae,_tmp24A,Cyc_Tcenv_empty_genv(grgn));}
# 987
te->ns=_tmp24A;
# 988
Cyc_Tc_tc_decls(te,_tmp235,in_externC,in_externCinclude,check_var_init,grgn,exports);
# 989
te->ns=_tmp249;
# 990
goto _LLFE;}_LL119: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp236=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp220;if(_tmp236->tag != 11)goto _LL11B;else{_tmp237=_tmp236->f1;_tmp238=*_tmp237;_tmp239=_tmp238.f1;_tmp23A=_tmp238.f2;_tmp23B=_tmp236->f2;}}_LL11A: {
# 994
struct _dyneither_ptr*first;
# 995
struct Cyc_List_List*rest;
# 996
{union Cyc_Absyn_Nmspace _tmp24C=_tmp239;int _tmp24D;struct Cyc_List_List*_tmp24E;struct Cyc_List_List*_tmp24F;struct Cyc_List_List*_tmp250;struct Cyc_List_List*_tmp251;struct Cyc_List_List _tmp252;struct _dyneither_ptr*_tmp253;struct Cyc_List_List*_tmp254;struct Cyc_List_List*_tmp255;struct Cyc_List_List _tmp256;struct _dyneither_ptr*_tmp257;struct Cyc_List_List*_tmp258;struct Cyc_List_List*_tmp259;struct Cyc_List_List _tmp25A;struct _dyneither_ptr*_tmp25B;struct Cyc_List_List*_tmp25C;_LL120: if((_tmp24C.Loc_n).tag != 4)goto _LL122;_tmp24D=(int)(_tmp24C.Loc_n).val;_LL121:
# 997
 goto _LL123;_LL122: if((_tmp24C.Rel_n).tag != 1)goto _LL124;_tmp24E=(struct Cyc_List_List*)(_tmp24C.Rel_n).val;if(_tmp24E != 0)goto _LL124;_LL123:
# 998
 goto _LL125;_LL124: if((_tmp24C.C_n).tag != 3)goto _LL126;_tmp24F=(struct Cyc_List_List*)(_tmp24C.C_n).val;if(_tmp24F != 0)goto _LL126;_LL125:
# 999
 goto _LL127;_LL126: if((_tmp24C.Abs_n).tag != 2)goto _LL128;_tmp250=(struct Cyc_List_List*)(_tmp24C.Abs_n).val;if(_tmp250 != 0)goto _LL128;_LL127:
# 1001
 first=_tmp23A;rest=0;goto _LL11F;_LL128: if((_tmp24C.Rel_n).tag != 1)goto _LL12A;_tmp251=(struct Cyc_List_List*)(_tmp24C.Rel_n).val;if(_tmp251 == 0)goto _LL12A;_tmp252=*_tmp251;_tmp253=(struct _dyneither_ptr*)_tmp252.hd;_tmp254=_tmp252.tl;_LL129:
# 1002
 _tmp257=_tmp253;_tmp258=_tmp254;goto _LL12B;_LL12A: if((_tmp24C.C_n).tag != 3)goto _LL12C;_tmp255=(struct Cyc_List_List*)(_tmp24C.C_n).val;if(_tmp255 == 0)goto _LL12C;_tmp256=*_tmp255;_tmp257=(struct _dyneither_ptr*)_tmp256.hd;_tmp258=_tmp256.tl;_LL12B:
# 1003
 _tmp25B=_tmp257;_tmp25C=_tmp258;goto _LL12D;_LL12C: if((_tmp24C.Abs_n).tag != 2)goto _LL11F;_tmp259=(struct Cyc_List_List*)(_tmp24C.Abs_n).val;if(_tmp259 == 0)goto _LL11F;_tmp25A=*_tmp259;_tmp25B=(struct _dyneither_ptr*)_tmp25A.hd;_tmp25C=_tmp25A.tl;_LL12D:
# 1005
 first=_tmp25B;{struct Cyc_List_List*_tmp3C5;rest=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp25C,((_tmp3C5=_cycalloc(sizeof(*_tmp3C5)),((_tmp3C5->hd=_tmp23A,((_tmp3C5->tl=0,_tmp3C5)))))));}goto _LL11F;_LL11F:;}{
# 1007
struct Cyc_List_List*_tmp25E=Cyc_Tcenv_resolve_namespace(te,loc,first,rest);
# 1008
{struct Cyc_List_List*_tmp3C6;ge->availables=(struct Cyc_List_List*)((_tmp3C6=_cycalloc(sizeof(*_tmp3C6)),((_tmp3C6->hd=_tmp25E,((_tmp3C6->tl=ge->availables,_tmp3C6))))));}
# 1009
Cyc_Tc_tc_decls(te,_tmp23B,in_externC,in_externCinclude,check_var_init,grgn,exports);
# 1010
ge->availables=((struct Cyc_List_List*)_check_null(ge->availables))->tl;
# 1011
goto _LLFE;};}_LL11B: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp23C=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp220;if(_tmp23C->tag != 12)goto _LL11D;else{_tmp23D=_tmp23C->f1;}}_LL11C:
# 1014
 Cyc_Tc_tc_decls(te,_tmp23D,1,in_externCinclude,check_var_init,grgn,exports);
# 1015
goto _LLFE;_LL11D: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp23E=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp220;if(_tmp23E->tag != 13)goto _LLFE;else{_tmp23F=_tmp23E->f1;_tmp240=_tmp23E->f2;}}_LL11E:
# 1018
((void(*)(void(*f)(struct Cyc_Tcenv_Tenv*,struct _tuple12*),struct Cyc_Tcenv_Tenv*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tc_resolve_export_namespace,te,_tmp240);{
# 1019
struct Cyc_List_List*newexs=
# 1020
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp240,(unsigned int)exports?*exports: 0);
# 1021
Cyc_Tc_tc_decls(te,_tmp23F,1,1,check_var_init,grgn,(struct Cyc_List_List**)& newexs);
# 1023
for(0;_tmp240 != 0;_tmp240=_tmp240->tl){
# 1024
struct _tuple12*_tmp260=(struct _tuple12*)_tmp240->hd;
# 1025
if(!(*_tmp260).f3){
# 1026
const char*_tmp3CA;void*_tmp3C9[1];struct Cyc_String_pa_PrintArg_struct _tmp3C8;(_tmp3C8.tag=0,((_tmp3C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1027
Cyc_Absynpp_qvar2string((*_tmp260).f2)),((_tmp3C9[0]=& _tmp3C8,Cyc_Tcutil_warn((*_tmp260).f1,((_tmp3CA="%s is exported but not defined",_tag_dyneither(_tmp3CA,sizeof(char),31))),_tag_dyneither(_tmp3C9,sizeof(void*),1)))))));}}
# 1029
goto _LLFE;};_LLFE:;}}
# 1034
void Cyc_Tc_tc(struct _RegionHandle*g,struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){
# 1036
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
# 1037
Cyc_Tc_tc_decls(te,ds,0,0,check_var_init,g,0);}struct Cyc_Tc_TreeshakeEnv{struct _RegionHandle*rgn;int in_cinclude;struct Cyc_Dict_Dict nsdict;};
# 1053 "tc.cyc"
typedef struct Cyc_Tc_TreeshakeEnv*Cyc_Tc_treeshake_env_t;
# 1055
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);
# 1057
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){
# 1058
return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}struct _tuple17{struct Cyc_Tcenv_Genv*f1;struct Cyc_Set_Set*f2;};
# 1061
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){
# 1062
void*_tmp264=d->r;struct Cyc_Absyn_Vardecl*_tmp266;struct Cyc_List_List*_tmp268;struct Cyc_List_List**_tmp269;struct Cyc_List_List*_tmp26B;struct Cyc_List_List**_tmp26C;struct Cyc_List_List*_tmp26E;struct Cyc_List_List**_tmp26F;struct Cyc_List_List*_tmp271;struct Cyc_List_List**_tmp272;_LL12F: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp265=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp264;if(_tmp265->tag != 0)goto _LL131;else{_tmp266=_tmp265->f1;}}_LL130:
# 1066
 if((env->in_cinclude  || _tmp266->sc != Cyc_Absyn_Extern  && _tmp266->sc != Cyc_Absyn_ExternC) || !
# 1067
Cyc_Absyn_qvar_cmp(_tmp266->name,Cyc_Absyn_uniquergn_qvar))
# 1069
return 1;{
# 1071
struct _tuple0 _tmp274;union Cyc_Absyn_Nmspace _tmp275;struct _dyneither_ptr*_tmp276;struct _tuple0*_tmp273=_tmp266->name;_tmp274=*_tmp273;_tmp275=_tmp274.f1;_tmp276=_tmp274.f2;{
# 1072
struct Cyc_List_List*ns;
# 1073
{union Cyc_Absyn_Nmspace _tmp277=_tmp275;int _tmp278;struct Cyc_List_List*_tmp279;struct Cyc_List_List*_tmp27A;struct Cyc_List_List*_tmp27B;_LL13C: if((_tmp277.Loc_n).tag != 4)goto _LL13E;_tmp278=(int)(_tmp277.Loc_n).val;_LL13D:
# 1074
 ns=0;goto _LL13B;_LL13E: if((_tmp277.Rel_n).tag != 1)goto _LL140;_tmp279=(struct Cyc_List_List*)(_tmp277.Rel_n).val;_LL13F:
# 1075
 ns=_tmp279;goto _LL13B;_LL140: if((_tmp277.C_n).tag != 3)goto _LL142;_tmp27A=(struct Cyc_List_List*)(_tmp277.C_n).val;_LL141:
# 1076
 ns=_tmp27A;goto _LL13B;_LL142: if((_tmp277.Abs_n).tag != 2)goto _LL13B;_tmp27B=(struct Cyc_List_List*)(_tmp277.Abs_n).val;_LL143:
# 1077
 ns=_tmp27B;goto _LL13B;_LL13B:;}{
# 1079
struct _tuple17*_tmp27C=((struct _tuple17*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(env->nsdict,ns);
# 1080
struct Cyc_Tcenv_Genv*_tmp27D=(*_tmp27C).f1;
# 1081
int _tmp27E=(*((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp27D->ordinaries,_tmp276)).f2;
# 1082
if(!_tmp27E)
# 1083
(*_tmp27C).f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(env->rgn,(*_tmp27C).f2,_tmp276);
# 1087
return _tmp27E;};};};_LL131: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp267=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp264;if(_tmp267->tag != 12)goto _LL133;else{_tmp268=_tmp267->f1;_tmp269=(struct Cyc_List_List**)& _tmp267->f1;}}_LL132:
# 1088
 _tmp26C=_tmp269;goto _LL134;_LL133: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp26A=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp264;if(_tmp26A->tag != 11)goto _LL135;else{_tmp26B=_tmp26A->f2;_tmp26C=(struct Cyc_List_List**)& _tmp26A->f2;}}_LL134:
# 1089
 _tmp26F=_tmp26C;goto _LL136;_LL135: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp26D=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp264;if(_tmp26D->tag != 10)goto _LL137;else{_tmp26E=_tmp26D->f2;_tmp26F=(struct Cyc_List_List**)& _tmp26D->f2;}}_LL136:
# 1091
*_tmp26F=Cyc_Tc_treeshake_f(env,*_tmp26F);
# 1092
return 1;_LL137: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp270=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp264;if(_tmp270->tag != 13)goto _LL139;else{_tmp271=_tmp270->f1;_tmp272=(struct Cyc_List_List**)& _tmp270->f1;}}_LL138: {
# 1094
int in_cinclude=env->in_cinclude;
# 1095
env->in_cinclude=1;
# 1096
*_tmp272=Cyc_Tc_treeshake_f(env,*_tmp272);
# 1097
env->in_cinclude=in_cinclude;
# 1098
return 1;}_LL139:;_LL13A:
# 1099
 return 1;_LL12E:;}
# 1103
static int Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,void*y){
# 1104
return !((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(set,x);}
# 1107
static struct _tuple17*Cyc_Tc_treeshake_make_env_f(struct _RegionHandle*rgn,struct Cyc_Tcenv_Genv*ge){
# 1109
struct _tuple17*_tmp3CB;return(_tmp3CB=_region_malloc(rgn,sizeof(*_tmp3CB)),((_tmp3CB->f1=ge,((_tmp3CB->f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(rgn,Cyc_strptrcmp),_tmp3CB)))));}struct _tuple18{struct Cyc_List_List*f1;struct _tuple17*f2;};
# 1112
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){
# 1113
struct _RegionHandle _tmp280=_new_region("rgn");struct _RegionHandle*rgn=& _tmp280;_push_region(rgn);
# 1114
{struct Cyc_Tc_TreeshakeEnv _tmp3CC;struct Cyc_Tc_TreeshakeEnv _tmp281=(_tmp3CC.rgn=rgn,((_tmp3CC.in_cinclude=0,((_tmp3CC.nsdict=
# 1115
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct _tuple17*(*f)(struct _RegionHandle*,struct Cyc_Tcenv_Genv*),struct _RegionHandle*env,struct Cyc_Dict_Dict d))Cyc_Dict_rmap_c)(rgn,Cyc_Tc_treeshake_make_env_f,rgn,te->ae),_tmp3CC)))));
# 1116
struct Cyc_List_List*_tmp282=Cyc_Tc_treeshake_f(& _tmp281,ds);
# 1117
if(((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp281.nsdict)){
# 1118
struct Cyc_List_List*_tmp283=_tmp282;_npop_handler(0);return _tmp283;}{
# 1120
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(Cyc_Core_heap_region,_tmp281.nsdict);
# 1121
struct _tuple18 _tmp284=*((struct _tuple18*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(rgn,_tmp281.nsdict);
# 1122
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(iter,& _tmp284)){
# 1123
struct _tuple17*_tmp286;struct _tuple17 _tmp287;struct Cyc_Tcenv_Genv*_tmp288;struct Cyc_Set_Set*_tmp289;struct _tuple18 _tmp285=_tmp284;_tmp286=_tmp285.f2;_tmp287=*_tmp286;_tmp288=_tmp287.f1;_tmp289=_tmp287.f2;
# 1124
_tmp288->ordinaries=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*f)(struct Cyc_Set_Set*,struct _dyneither_ptr*,struct _tuple13*),struct Cyc_Set_Set*env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(_tmp288->grgn,(int(*)(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,struct _tuple13*y))Cyc_Tc_treeshake_remove_f,_tmp289,_tmp288->ordinaries);}{
# 1126
struct Cyc_List_List*_tmp28A=_tmp282;_npop_handler(0);return _tmp28A;};};}
# 1114
;_pop_region(rgn);}
