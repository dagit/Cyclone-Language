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
typedef void*Cyc_sarg_t;
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 123 "core.h"
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 178
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 230
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 234
struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i);
# 249
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 245
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 280
typedef void*Cyc_Absyn_pointer_qual_t;
# 281
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 340
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 353
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 431
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
# 433
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 507
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 523
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 525
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
# 526
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 533
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 540
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 558
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 912 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 914
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 917
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
# 918
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 921
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 924
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 926
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
# 927
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
# 928
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 929
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 930
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 937
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 939
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 942
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 944
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
# 946
void*Cyc_Absyn_float_typ(int);void*Cyc_Absyn_wchar_typ();
# 956
void*Cyc_Absyn_exn_typ();
# 967
extern void*Cyc_Absyn_bounds_one;
# 972
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 975
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 978
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 983
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 994
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 998
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1000
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1046
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1060
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
# 1066
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;
# 1070
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1081
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1140
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1142
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1145
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1147
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
# 1159
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 64 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 68
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 100
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
# 122 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
# 123
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
# 124
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
# 132
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 133
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 136
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 137
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 158
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
# 159
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
# 160
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);
# 164
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
# 165
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
# 177
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
# 178
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
# 183
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
# 191
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 195
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 199
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 206
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 53
void*Cyc_Tcutil_copy_type(void*t);
# 55
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*);
# 58
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 62
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
# 63
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 65
void*Cyc_Tcutil_compress(void*t);
# 66
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
# 67
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
# 68
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 69
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 70
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);
# 71
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 72
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 74
int Cyc_Tcutil_coerceable(void*);
# 76
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 78
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);struct _tuple11{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 81
struct _tuple11 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 89
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 90
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
# 92
int Cyc_Tcutil_is_pointer_type(void*t);
# 93
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
# 94
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);
# 95
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 96
void*Cyc_Tcutil_pointer_region(void*t);
# 100
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
# 101
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 109
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 116
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 118
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 119
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
# 123
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 124
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
# 126
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 142
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 144
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 148
void Cyc_Tcutil_explain_failure();
# 150
int Cyc_Tcutil_unify(void*,void*);
# 155
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 164
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 169
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 178
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
# 214 "tcutil.h"
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 224
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 226
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 228
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 232
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 238
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 240
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 242
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 252
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 257
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 260
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 265
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 270
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple14{int f1;void*f2;};
# 279
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 306
int Cyc_Tcutil_bits_only(void*t);
# 309
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 335
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 338
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);struct _tuple15{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple15 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 48
int Cyc_Evexp_okay_szofarg(void*t);
# 27 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);
# 34 "formatstr.h"
struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
# 37
struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 33
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
# 40
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 42
extern int Cyc_Tcexp_in_stmt_exp;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 49 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 60
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 62
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 67
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple16{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple16 val;};struct _union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct _union_RelnOp_LessEqConst LessEqConst;};
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
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple17{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple17 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 158 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 177
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 256 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);
# 52 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){
# 56
{void*_tmp6D4;(_tmp6D4=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_dyneither(_tmp6D4,sizeof(void*),0)));}
# 57
if(topt == 0)
# 58
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 60
return*topt;}}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 71 "tcexp.cyc"
static struct _tuple18*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){
# 72
struct _tuple18*_tmp6D5;return(_tmp6D5=_cycalloc(sizeof(*_tmp6D5)),((_tmp6D5->f1=0,((_tmp6D5->f2=e,_tmp6D5)))));}
# 77
static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){
# 78
void*_tmp2=e1->r;struct _tuple1*_tmp4;void*_tmp5;void**_tmp6;_LL1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2;if(_tmp3->tag != 1)goto _LL3;else{_tmp4=_tmp3->f1;_tmp5=(void**)& _tmp3->f2;_tmp6=(void**)((void**)& _tmp3->f2);}}_LL2: {
# 80
struct _handler_cons _tmp7;_push_handler(& _tmp7);{int _tmp9=0;if(setjmp(_tmp7.handler))_tmp9=1;if(!_tmp9){
# 81
{struct _RegionHandle _tmpA=_new_region("r");struct _RegionHandle*r=& _tmpA;_push_region(r);
# 82
{void*_tmpB=Cyc_Tcenv_lookup_ordinary(r,te,e1->loc,_tmp4);void*_tmpD;void*_tmp10;struct Cyc_Absyn_Datatypedecl*_tmp12;struct Cyc_Absyn_Datatypefield*_tmp13;struct Cyc_Absyn_Aggrdecl*_tmp15;_LL6: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpC=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpB;if(_tmpC->tag != 0)goto _LL8;else{_tmpD=(void*)_tmpC->f1;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpE=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpD;if(_tmpE->tag != 0)goto _LL8;};}}_LL7:
# 85
 _npop_handler(1);return;_LL8: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpF=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpB;if(_tmpF->tag != 0)goto _LLA;else{_tmp10=(void*)_tmpF->f1;}}_LL9:
# 87
*_tmp6=_tmp10;
# 88
_npop_handler(1);return;_LLA: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp11=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpB;if(_tmp11->tag != 2)goto _LLC;else{_tmp12=_tmp11->f1;_tmp13=_tmp11->f2;}}_LLB:
# 90
 if(_tmp13->typs == 0){
# 92
const char*_tmp6D9;void*_tmp6D8[1];struct Cyc_String_pa_PrintArg_struct _tmp6D7;(_tmp6D7.tag=0,((_tmp6D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 93
Cyc_Absynpp_qvar2string(_tmp13->name)),((_tmp6D8[0]=& _tmp6D7,Cyc_Tcutil_terr(e->loc,((_tmp6D9="%s is a constant, not a function",_tag_dyneither(_tmp6D9,sizeof(char),33))),_tag_dyneither(_tmp6D8,sizeof(void*),1)))))));}
# 94
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp6DC;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp6DB;e->r=(void*)((_tmp6DB=_cycalloc(sizeof(*_tmp6DB)),((_tmp6DB[0]=((_tmp6DC.tag=29,((_tmp6DC.f1=es,((_tmp6DC.f2=_tmp12,((_tmp6DC.f3=_tmp13,_tmp6DC)))))))),_tmp6DB))));}
# 95
_npop_handler(1);return;_LLC: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp14=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpB;if(_tmp14->tag != 1)goto _LLE;else{_tmp15=_tmp14->f1;}}_LLD: {
# 97
struct Cyc_List_List*_tmp1D=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);
# 98
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp6DF;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp6DE;e->r=(void*)((_tmp6DE=_cycalloc(sizeof(*_tmp6DE)),((_tmp6DE[0]=((_tmp6DF.tag=27,((_tmp6DF.f1=_tmp15->name,((_tmp6DF.f2=0,((_tmp6DF.f3=_tmp1D,((_tmp6DF.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp15,_tmp6DF)))))))))),_tmp6DE))));}
# 99
_npop_handler(1);return;}_LLE: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp16=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpB;if(_tmp16->tag != 4)goto _LL10;}_LLF:
# 100
 goto _LL11;_LL10: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp17=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpB;if(_tmp17->tag != 3)goto _LL5;}_LL11:
# 102
{const char*_tmp6E3;void*_tmp6E2[1];struct Cyc_String_pa_PrintArg_struct _tmp6E1;(_tmp6E1.tag=0,((_tmp6E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp6E2[0]=& _tmp6E1,Cyc_Tcutil_terr(e->loc,((_tmp6E3="%s is an enum constructor, not a function",_tag_dyneither(_tmp6E3,sizeof(char),42))),_tag_dyneither(_tmp6E2,sizeof(void*),1)))))));}
# 103
_npop_handler(1);return;_LL5:;}
# 82
;_pop_region(r);}
# 81
;_pop_handler();}else{void*_tmp8=(void*)_exn_thrown;void*_tmp24=_tmp8;_LL13: {struct Cyc_Dict_Absent_exn_struct*_tmp25=(struct Cyc_Dict_Absent_exn_struct*)_tmp24;if(_tmp25->tag != Cyc_Dict_Absent)goto _LL15;}_LL14:
# 109
 return;_LL15:;_LL16:(void)_throw(_tmp24);_LL12:;}};}_LL3:;_LL4:
# 112
 return;_LL0:;}
# 116
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 120
if(topt == 0){
# 122
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6E6;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6E5;e->r=(void*)((_tmp6E5=_cycalloc(sizeof(*_tmp6E5)),((_tmp6E5[0]=((_tmp6E6.tag=25,((_tmp6E6.f1=des,_tmp6E6)))),_tmp6E5))));}
# 123
return;}{
# 125
void*t=*topt;
# 126
void*_tmp28=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp2A;union Cyc_Absyn_AggrInfoU _tmp2B;struct Cyc_Absyn_ArrayInfo _tmp2D;void*_tmp2E;struct Cyc_Absyn_Tqual _tmp2F;_LL18: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp29=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp28;if(_tmp29->tag != 11)goto _LL1A;else{_tmp2A=_tmp29->f1;_tmp2B=_tmp2A.aggr_info;}}_LL19:
# 128
{union Cyc_Absyn_AggrInfoU _tmp31=_tmp2B;struct _tuple3 _tmp32;struct Cyc_Absyn_Aggrdecl**_tmp33;struct Cyc_Absyn_Aggrdecl*_tmp34;_LL21: if((_tmp31.UnknownAggr).tag != 1)goto _LL23;_tmp32=(struct _tuple3)(_tmp31.UnknownAggr).val;_LL22: {
# 129
const char*_tmp6E9;void*_tmp6E8;(_tmp6E8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp6E9="struct type not properly set",_tag_dyneither(_tmp6E9,sizeof(char),29))),_tag_dyneither(_tmp6E8,sizeof(void*),0)));}_LL23: if((_tmp31.KnownAggr).tag != 2)goto _LL20;_tmp33=(struct Cyc_Absyn_Aggrdecl**)(_tmp31.KnownAggr).val;_tmp34=*_tmp33;_LL24: {
# 130
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp6EC;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp6EB;e->r=(void*)((_tmp6EB=_cycalloc(sizeof(*_tmp6EB)),((_tmp6EB[0]=((_tmp6EC.tag=27,((_tmp6EC.f1=_tmp34->name,((_tmp6EC.f2=0,((_tmp6EC.f3=des,((_tmp6EC.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp34,_tmp6EC)))))))))),_tmp6EB))));}_LL20:;}
# 132
goto _LL17;_LL1A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp28;if(_tmp2C->tag != 8)goto _LL1C;else{_tmp2D=_tmp2C->f1;_tmp2E=_tmp2D.elt_type;_tmp2F=_tmp2D.tq;}}_LL1B:
# 133
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6EF;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6EE;e->r=(void*)((_tmp6EE=_cycalloc(sizeof(*_tmp6EE)),((_tmp6EE[0]=((_tmp6EF.tag=25,((_tmp6EF.f1=des,_tmp6EF)))),_tmp6EE))));}goto _LL17;_LL1C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp30=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp28;if(_tmp30->tag != 12)goto _LL1E;}_LL1D:
# 134
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp6F2;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp6F1;e->r=(void*)((_tmp6F1=_cycalloc(sizeof(*_tmp6F1)),((_tmp6F1[0]=((_tmp6F2.tag=28,((_tmp6F2.f1=(void*)t,((_tmp6F2.f2=des,_tmp6F2)))))),_tmp6F1))));}goto _LL17;_LL1E:;_LL1F:
# 135
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6F5;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6F4;e->r=(void*)((_tmp6F4=_cycalloc(sizeof(*_tmp6F4)),((_tmp6F4[0]=((_tmp6F5.tag=25,((_tmp6F5.f1=des,_tmp6F5)))),_tmp6F4))));}goto _LL17;_LL17:;};}
# 142
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
# 143
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
# 146
static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){
# 147
for(0;es != 0;es=es->tl){
# 148
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
# 152
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){
# 153
Cyc_Tcutil_check_contains_assign(e);
# 154
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_sint_typ,e);
# 155
if(!Cyc_Tcutil_coerce_to_bool(te,e)){
# 156
const char*_tmp6FA;void*_tmp6F9[2];struct Cyc_String_pa_PrintArg_struct _tmp6F8;struct Cyc_String_pa_PrintArg_struct _tmp6F7;(_tmp6F7.tag=0,((_tmp6F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 157
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp6F8.tag=0,((_tmp6F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp6F9[0]=& _tmp6F8,((_tmp6F9[1]=& _tmp6F7,Cyc_Tcutil_terr(e->loc,((_tmp6FA="test of %s has type %s instead of integral or * type",_tag_dyneither(_tmp6FA,sizeof(char),53))),_tag_dyneither(_tmp6F9,sizeof(void*),2)))))))))))));}
# 161
{void*_tmp43=e->r;enum Cyc_Absyn_Primop _tmp45;struct Cyc_List_List*_tmp46;_LL26: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp44=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp43;if(_tmp44->tag != 2)goto _LL28;else{_tmp45=_tmp44->f1;_tmp46=_tmp44->f2;}}_LL27:
# 163
 if(_tmp45 == Cyc_Absyn_Eq  || _tmp45 == Cyc_Absyn_Neq){
# 164
struct _tuple0 _tmp6FB;struct _tuple0 _tmp48=(_tmp6FB.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp46))->hd)->topt),((_tmp6FB.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp46))->tl))->hd)->topt),_tmp6FB)));void*_tmp49;void*_tmp4B;void*_tmp4C;void*_tmp4E;_LL2B: _tmp49=_tmp48.f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4A=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp49;if(_tmp4A->tag != 15)goto _LL2D;else{_tmp4B=(void*)_tmp4A->f1;}};_tmp4C=_tmp48.f2;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4C;if(_tmp4D->tag != 15)goto _LL2D;else{_tmp4E=(void*)_tmp4D->f1;}};_LL2C: {
# 166
struct _tuple0*_tmp6FE;struct Cyc_Tcexp_TestEnv _tmp6FD;return(_tmp6FD.eq=((_tmp6FE=_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp6FE)),((_tmp6FE->f1=_tmp4B,((_tmp6FE->f2=_tmp4E,_tmp6FE)))))),((_tmp6FD.isTrue=_tmp45 == Cyc_Absyn_Eq,_tmp6FD)));}_LL2D:;_LL2E:
# 168
 goto _LL2A;_LL2A:;}
# 171
goto _LL25;_LL28:;_LL29:
# 172
 goto _LL25;_LL25:;}{
# 174
struct Cyc_Tcexp_TestEnv _tmp6FF;return(_tmp6FF.eq=0,((_tmp6FF.isTrue=0,_tmp6FF)));};}
# 195 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _dyneither_ptr s){
# 196
return 1;}
# 200
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){
# 201
void*t;
# 202
void*string_elt_typ=Cyc_Absyn_char_typ;
# 203
int string_numelts=0;
# 204
{union Cyc_Absyn_Cnst _tmp52=*((union Cyc_Absyn_Cnst*)_check_null(c));struct _tuple4 _tmp53;enum Cyc_Absyn_Sign _tmp54;struct _tuple4 _tmp55;enum Cyc_Absyn_Sign _tmp56;struct _tuple4 _tmp57;enum Cyc_Absyn_Sign _tmp58;struct _dyneither_ptr _tmp59;struct _tuple5 _tmp5A;enum Cyc_Absyn_Sign _tmp5B;struct _tuple7 _tmp5C;enum Cyc_Absyn_Sign _tmp5D;struct _tuple8 _tmp5E;int _tmp5F;struct _tuple6 _tmp60;enum Cyc_Absyn_Sign _tmp61;int _tmp62;struct _dyneither_ptr _tmp63;struct _dyneither_ptr _tmp64;int _tmp65;_LL30: if((_tmp52.Char_c).tag != 2)goto _LL32;_tmp53=(struct _tuple4)(_tmp52.Char_c).val;_tmp54=_tmp53.f1;if(_tmp54 != Cyc_Absyn_Signed)goto _LL32;_LL31:
# 205
 t=Cyc_Absyn_schar_typ;goto _LL2F;_LL32: if((_tmp52.Char_c).tag != 2)goto _LL34;_tmp55=(struct _tuple4)(_tmp52.Char_c).val;_tmp56=_tmp55.f1;if(_tmp56 != Cyc_Absyn_Unsigned)goto _LL34;_LL33:
# 206
 t=Cyc_Absyn_uchar_typ;goto _LL2F;_LL34: if((_tmp52.Char_c).tag != 2)goto _LL36;_tmp57=(struct _tuple4)(_tmp52.Char_c).val;_tmp58=_tmp57.f1;if(_tmp58 != Cyc_Absyn_None)goto _LL36;_LL35:
# 207
 t=Cyc_Absyn_char_typ;goto _LL2F;_LL36: if((_tmp52.Wchar_c).tag != 3)goto _LL38;_tmp59=(struct _dyneither_ptr)(_tmp52.Wchar_c).val;_LL37:
# 208
 t=Cyc_Absyn_wchar_typ();goto _LL2F;_LL38: if((_tmp52.Short_c).tag != 4)goto _LL3A;_tmp5A=(struct _tuple5)(_tmp52.Short_c).val;_tmp5B=_tmp5A.f1;_LL39:
# 210
 t=_tmp5B == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL2F;_LL3A: if((_tmp52.LongLong_c).tag != 6)goto _LL3C;_tmp5C=(struct _tuple7)(_tmp52.LongLong_c).val;_tmp5D=_tmp5C.f1;_LL3B:
# 212
 t=_tmp5D == Cyc_Absyn_Unsigned?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;goto _LL2F;_LL3C: if((_tmp52.Float_c).tag != 7)goto _LL3E;_tmp5E=(struct _tuple8)(_tmp52.Float_c).val;_tmp5F=_tmp5E.f2;_LL3D:
# 213
 t=Cyc_Absyn_float_typ(_tmp5F);goto _LL2F;_LL3E: if((_tmp52.Int_c).tag != 5)goto _LL40;_tmp60=(struct _tuple6)(_tmp52.Int_c).val;_tmp61=_tmp60.f1;_tmp62=_tmp60.f2;_LL3F:
# 215
 if(topt == 0)
# 216
t=_tmp61 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{
# 222
void*_tmp66=Cyc_Tcutil_compress(*topt);enum Cyc_Absyn_Sign _tmp68;enum Cyc_Absyn_Size_of _tmp69;enum Cyc_Absyn_Sign _tmp6B;enum Cyc_Absyn_Size_of _tmp6C;enum Cyc_Absyn_Sign _tmp6E;enum Cyc_Absyn_Size_of _tmp6F;enum Cyc_Absyn_Sign _tmp71;enum Cyc_Absyn_Size_of _tmp72;struct Cyc_Absyn_PtrInfo _tmp74;void*_tmp75;struct Cyc_Absyn_Tqual _tmp76;struct Cyc_Absyn_PtrAtts _tmp77;void*_tmp78;union Cyc_Absyn_Constraint*_tmp79;union Cyc_Absyn_Constraint*_tmp7A;union Cyc_Absyn_Constraint*_tmp7B;void*_tmp7D;_LL47: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp67=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp66;if(_tmp67->tag != 6)goto _LL49;else{_tmp68=_tmp67->f1;_tmp69=_tmp67->f2;if(_tmp69 != Cyc_Absyn_Char_sz)goto _LL49;}}_LL48:
# 224
 switch(_tmp68){case Cyc_Absyn_Unsigned: _LL55:
# 225
 t=Cyc_Absyn_uchar_typ;break;case Cyc_Absyn_Signed: _LL56:
# 226
 t=Cyc_Absyn_schar_typ;break;case Cyc_Absyn_None: _LL57:
# 227
 t=Cyc_Absyn_char_typ;break;}
# 229
*c=Cyc_Absyn_Char_c(_tmp68,(char)_tmp62);
# 230
goto _LL46;_LL49: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp66;if(_tmp6A->tag != 6)goto _LL4B;else{_tmp6B=_tmp6A->f1;_tmp6C=_tmp6A->f2;if(_tmp6C != Cyc_Absyn_Short_sz)goto _LL4B;}}_LL4A:
# 232
 t=_tmp6B == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;
# 233
*c=Cyc_Absyn_Short_c(_tmp6B,(short)_tmp62);
# 234
goto _LL46;_LL4B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp66;if(_tmp6D->tag != 6)goto _LL4D;else{_tmp6E=_tmp6D->f1;_tmp6F=_tmp6D->f2;if(_tmp6F != Cyc_Absyn_Int_sz)goto _LL4D;}}_LL4C:
# 236
 t=_tmp6E == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
# 237
goto _LL46;_LL4D: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp70=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp66;if(_tmp70->tag != 6)goto _LL4F;else{_tmp71=_tmp70->f1;_tmp72=_tmp70->f2;if(_tmp72 != Cyc_Absyn_Long_sz)goto _LL4F;}}_LL4E:
# 239
 t=_tmp71 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
# 240
goto _LL46;_LL4F:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp73=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp66;if(_tmp73->tag != 5)goto _LL51;else{_tmp74=_tmp73->f1;_tmp75=_tmp74.elt_typ;_tmp76=_tmp74.elt_tq;_tmp77=_tmp74.ptr_atts;_tmp78=_tmp77.rgn;_tmp79=_tmp77.nullable;_tmp7A=_tmp77.bounds;_tmp7B=_tmp77.zero_term;}}if(!(_tmp62 == 0))goto _LL51;_LL50: {
# 243
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0,{.Null_c={1,0}}};
# 244
e->r=(void*)& nullc;
# 245
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp79))return*topt;{
# 246
struct Cyc_List_List*_tmp7E=Cyc_Tcenv_lookup_type_vars(te);
# 247
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp709;struct Cyc_Absyn_PtrAtts _tmp708;struct Cyc_Absyn_PtrInfo _tmp707;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp706;t=(void*)((_tmp706=_cycalloc(sizeof(*_tmp706)),((_tmp706[0]=((_tmp709.tag=5,((_tmp709.f1=((_tmp707.elt_typ=_tmp75,((_tmp707.elt_tq=_tmp76,((_tmp707.ptr_atts=(
# 248
(_tmp708.rgn=_tmp78,((_tmp708.nullable=Cyc_Absyn_true_conref,((_tmp708.bounds=_tmp7A,((_tmp708.zero_term=_tmp7B,((_tmp708.ptrloc=0,_tmp708)))))))))),_tmp707)))))),_tmp709)))),_tmp706))));}
# 249
goto _LL46;};}_LL51: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp7C=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp66;if(_tmp7C->tag != 19)goto _LL53;else{_tmp7D=(void*)_tmp7C->f1;}}_LL52: {
# 251
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp70C;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp70B;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp84=(_tmp70B=_cycalloc(sizeof(*_tmp70B)),((_tmp70B[0]=((_tmp70C.tag=18,((_tmp70C.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp62,0),_tmp70C)))),_tmp70B)));
# 252
if(!Cyc_Tcutil_unify(_tmp7D,(void*)_tmp84)){
# 253
{const char*_tmp711;void*_tmp710[2];struct Cyc_String_pa_PrintArg_struct _tmp70F;struct Cyc_String_pa_PrintArg_struct _tmp70E;(_tmp70E.tag=0,((_tmp70E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_tmp84)),((_tmp70F.tag=0,((_tmp70F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp7D)),((_tmp710[0]=& _tmp70F,((_tmp710[1]=& _tmp70E,Cyc_Tcutil_terr(loc,((_tmp711="expecting %s but found %s",_tag_dyneither(_tmp711,sizeof(char),26))),_tag_dyneither(_tmp710,sizeof(void*),2)))))))))))));}
# 254
Cyc_Tcutil_explain_failure();}
# 256
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp714;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp713;t=(void*)((_tmp713=_cycalloc(sizeof(*_tmp713)),((_tmp713[0]=((_tmp714.tag=19,((_tmp714.f1=(void*)((void*)_tmp84),_tmp714)))),_tmp713))));}
# 257
goto _LL46;}_LL53:;_LL54:
# 259
 t=_tmp61 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
# 260
goto _LL46;_LL46:;}
# 262
goto _LL2F;_LL40: if((_tmp52.String_c).tag != 8)goto _LL42;_tmp63=(struct _dyneither_ptr)(_tmp52.String_c).val;_LL41:
# 264
 string_numelts=(int)_get_dyneither_size(_tmp63,sizeof(char));
# 265
_tmp64=_tmp63;goto _LL43;_LL42: if((_tmp52.Wstring_c).tag != 9)goto _LL44;_tmp64=(struct _dyneither_ptr)(_tmp52.Wstring_c).val;_LL43:
# 267
 if(string_numelts == 0){
# 268
string_numelts=Cyc_Tcexp_wchar_numelts(_tmp64);
# 269
string_elt_typ=Cyc_Absyn_wchar_typ();}{
# 271
struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts),loc);
# 272
elen->topt=(void*)Cyc_Absyn_uint_typ;{
# 276
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp717;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp716;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp8D=(_tmp716=_cycalloc(sizeof(*_tmp716)),((_tmp716[0]=((_tmp717.tag=1,((_tmp717.f1=elen,_tmp717)))),_tmp716)));
# 277
t=Cyc_Absyn_atb_typ(string_elt_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(0),(void*)_tmp8D,Cyc_Absyn_true_conref);
# 279
if(topt != 0){
# 280
void*_tmp8E=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp90;struct Cyc_Absyn_Tqual _tmp91;_LL5A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8E;if(_tmp8F->tag != 8)goto _LL5C;else{_tmp90=_tmp8F->f1;_tmp91=_tmp90.tq;}}_LL5B:
# 284
 return Cyc_Absyn_array_typ(string_elt_typ,_tmp91,(struct Cyc_Absyn_Exp*)elen,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0);_LL5C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp92=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8E;if(_tmp92->tag != 5)goto _LL5E;}_LL5D:
# 286
 if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
# 287
e->topt=(void*)t;
# 288
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
# 289
t=*topt;}else{
# 292
t=Cyc_Absyn_atb_typ(string_elt_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te)),
# 293
Cyc_Absyn_const_tqual(0),(void*)_tmp8D,Cyc_Absyn_true_conref);
# 294
if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
# 295
e->topt=(void*)t;
# 296
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
# 297
t=*topt;}}
# 300
goto _LL59;_LL5E:;_LL5F:
# 301
 goto _LL59;_LL59:;}
# 304
return t;};};_LL44: if((_tmp52.Null_c).tag != 1)goto _LL2F;_tmp65=(int)(_tmp52.Null_c).val;_LL45:
# 306
 if(topt != 0){
# 307
void*_tmp95=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp97;void*_tmp98;struct Cyc_Absyn_Tqual _tmp99;struct Cyc_Absyn_PtrAtts _tmp9A;void*_tmp9B;union Cyc_Absyn_Constraint*_tmp9C;union Cyc_Absyn_Constraint*_tmp9D;union Cyc_Absyn_Constraint*_tmp9E;_LL61: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp96=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp95;if(_tmp96->tag != 5)goto _LL63;else{_tmp97=_tmp96->f1;_tmp98=_tmp97.elt_typ;_tmp99=_tmp97.elt_tq;_tmp9A=_tmp97.ptr_atts;_tmp9B=_tmp9A.rgn;_tmp9C=_tmp9A.nullable;_tmp9D=_tmp9A.bounds;_tmp9E=_tmp9A.zero_term;}}_LL62:
# 310
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp9C))return*topt;{
# 311
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp721;struct Cyc_Absyn_PtrAtts _tmp720;struct Cyc_Absyn_PtrInfo _tmp71F;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp71E;return(void*)((_tmp71E=_cycalloc(sizeof(*_tmp71E)),((_tmp71E[0]=((_tmp721.tag=5,((_tmp721.f1=((_tmp71F.elt_typ=_tmp98,((_tmp71F.elt_tq=_tmp99,((_tmp71F.ptr_atts=((_tmp720.rgn=_tmp9B,((_tmp720.nullable=Cyc_Absyn_true_conref,((_tmp720.bounds=_tmp9D,((_tmp720.zero_term=_tmp9E,((_tmp720.ptrloc=0,_tmp720)))))))))),_tmp71F)))))),_tmp721)))),_tmp71E))));};_LL63:;_LL64:
# 313
 goto _LL60;_LL60:;}{
# 316
struct Cyc_List_List*_tmpA3=Cyc_Tcenv_lookup_type_vars(te);
# 317
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp734;struct Cyc_Absyn_PtrAtts _tmp733;struct Cyc_Core_Opt*_tmp732;struct Cyc_Core_Opt*_tmp731;struct Cyc_Absyn_PtrInfo _tmp730;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp72F;t=(void*)((_tmp72F=_cycalloc(sizeof(*_tmp72F)),((_tmp72F[0]=((_tmp734.tag=5,((_tmp734.f1=((_tmp730.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tako,((_tmp731=_cycalloc(sizeof(*_tmp731)),((_tmp731->v=_tmpA3,_tmp731))))),((_tmp730.elt_tq=
# 318
Cyc_Absyn_empty_tqual(0),((_tmp730.ptr_atts=(
# 319
(_tmp733.rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((_tmp732=_cycalloc(sizeof(*_tmp732)),((_tmp732->v=_tmpA3,_tmp732))))),((_tmp733.nullable=Cyc_Absyn_true_conref,((_tmp733.bounds=
# 321
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp733.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp733.ptrloc=0,_tmp733)))))))))),_tmp730)))))),_tmp734)))),_tmp72F))));}
# 323
goto _LL2F;};_LL2F:;}
# 325
return t;}
# 329
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf);
# 334
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _tuple1*q,void**b){
# 338
void*_tmpAA=*((void**)_check_null(b));struct Cyc_Absyn_Vardecl*_tmpAD;struct Cyc_Absyn_Fndecl*_tmpAF;struct Cyc_Absyn_Vardecl*_tmpB1;struct Cyc_Absyn_Vardecl*_tmpB3;struct Cyc_Absyn_Vardecl*_tmpB5;_LL66: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpAB=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpAA;if(_tmpAB->tag != 0)goto _LL68;}_LL67: {
# 340
struct _handler_cons _tmpB6;_push_handler(& _tmpB6);{int _tmpB8=0;if(setjmp(_tmpB6.handler))_tmpB8=1;if(!_tmpB8){
# 341
{struct _RegionHandle _tmpB9=_new_region("r");struct _RegionHandle*r=& _tmpB9;_push_region(r);
# 342
{void*_tmpBA=Cyc_Tcenv_lookup_ordinary(r,te,e->loc,q);void*_tmpBC;void*_tmpBF;struct Cyc_Absyn_Enumdecl*_tmpC1;struct Cyc_Absyn_Enumfield*_tmpC2;void*_tmpC4;struct Cyc_Absyn_Enumfield*_tmpC5;struct Cyc_Absyn_Datatypedecl*_tmpC7;struct Cyc_Absyn_Datatypefield*_tmpC8;_LL73: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpBB=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpBA;if(_tmpBB->tag != 0)goto _LL75;else{_tmpBC=(void*)_tmpBB->f1;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpBD=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpBC;if(_tmpBD->tag != 0)goto _LL75;};}}_LL74: {
# 344
const char*_tmp738;void*_tmp737[1];struct Cyc_String_pa_PrintArg_struct _tmp736;void*_tmpCD=(_tmp736.tag=0,((_tmp736.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp737[0]=& _tmp736,Cyc_Tcexp_expr_err(te,loc,0,((_tmp738="undeclared identifier: %s",_tag_dyneither(_tmp738,sizeof(char),26))),_tag_dyneither(_tmp737,sizeof(void*),1)))))));_npop_handler(1);return _tmpCD;}_LL75: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpBE=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpBA;if(_tmpBE->tag != 0)goto _LL77;else{_tmpBF=(void*)_tmpBE->f1;}}_LL76:
# 346
*b=_tmpBF;{
# 347
void*_tmpCE=Cyc_Tcexp_tcVar(te,loc,topt,e,q,b);_npop_handler(1);return _tmpCE;};_LL77: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpC0=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpBA;if(_tmpC0->tag != 3)goto _LL79;else{_tmpC1=_tmpC0->f1;_tmpC2=_tmpC0->f2;}}_LL78:
# 349
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp73B;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp73A;e->r=(void*)((_tmp73A=_cycalloc(sizeof(*_tmp73A)),((_tmp73A[0]=((_tmp73B.tag=30,((_tmp73B.f1=_tmpC2->name,((_tmp73B.f2=(struct Cyc_Absyn_Enumdecl*)_tmpC1,((_tmp73B.f3=(struct Cyc_Absyn_Enumfield*)_tmpC2,_tmp73B)))))))),_tmp73A))));}{
# 350
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp73E;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp73D;void*_tmpD3=(void*)((_tmp73D=_cycalloc(sizeof(*_tmp73D)),((_tmp73D[0]=((_tmp73E.tag=13,((_tmp73E.f1=_tmpC1->name,((_tmp73E.f2=(struct Cyc_Absyn_Enumdecl*)_tmpC1,_tmp73E)))))),_tmp73D))));_npop_handler(1);return _tmpD3;};_LL79: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpC3=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpBA;if(_tmpC3->tag != 4)goto _LL7B;else{_tmpC4=(void*)_tmpC3->f1;_tmpC5=_tmpC3->f2;}}_LL7A:
# 352
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp741;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp740;e->r=(void*)((_tmp740=_cycalloc(sizeof(*_tmp740)),((_tmp740[0]=((_tmp741.tag=31,((_tmp741.f1=_tmpC5->name,((_tmp741.f2=(void*)_tmpC4,((_tmp741.f3=(struct Cyc_Absyn_Enumfield*)_tmpC5,_tmp741)))))))),_tmp740))));}{
# 353
void*_tmpD6=_tmpC4;_npop_handler(1);return _tmpD6;};_LL7B: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpC6=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpBA;if(_tmpC6->tag != 2)goto _LL7D;else{_tmpC7=_tmpC6->f1;_tmpC8=_tmpC6->f2;}}_LL7C:
# 355
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp744;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp743;e->r=(void*)((_tmp743=_cycalloc(sizeof(*_tmp743)),((_tmp743[0]=((_tmp744.tag=29,((_tmp744.f1=0,((_tmp744.f2=_tmpC7,((_tmp744.f3=_tmpC8,_tmp744)))))))),_tmp743))));}{
# 356
void*_tmpD9=Cyc_Tcexp_tcDatatype(te,loc,topt,e,0,_tmpC7,_tmpC8);_npop_handler(1);return _tmpD9;};_LL7D: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpC9=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpBA;if(_tmpC9->tag != 1)goto _LL72;}_LL7E: {
# 358
const char*_tmp748;void*_tmp747[1];struct Cyc_String_pa_PrintArg_struct _tmp746;void*_tmpDD=(_tmp746.tag=0,((_tmp746.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp747[0]=& _tmp746,Cyc_Tcexp_expr_err(te,loc,0,((_tmp748="bad occurrence of type name %s",_tag_dyneither(_tmp748,sizeof(char),31))),_tag_dyneither(_tmp747,sizeof(void*),1)))))));_npop_handler(1);return _tmpDD;}_LL72:;}
# 342
;_pop_region(r);}
# 341
;_pop_handler();}else{void*_tmpB7=(void*)_exn_thrown;void*_tmpDF=_tmpB7;_LL80: {struct Cyc_Dict_Absent_exn_struct*_tmpE0=(struct Cyc_Dict_Absent_exn_struct*)_tmpDF;if(_tmpE0->tag != Cyc_Dict_Absent)goto _LL82;}_LL81: {
# 363
const char*_tmp74C;void*_tmp74B[1];struct Cyc_String_pa_PrintArg_struct _tmp74A;return(_tmp74A.tag=0,((_tmp74A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp74B[0]=& _tmp74A,Cyc_Tcexp_expr_err(te,loc,0,((_tmp74C="undeclared identifier: %s",_tag_dyneither(_tmp74C,sizeof(char),26))),_tag_dyneither(_tmp74B,sizeof(void*),1)))))));}_LL82:;_LL83:(void)_throw(_tmpDF);_LL7F:;}};}_LL68: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpAC=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpAA;if(_tmpAC->tag != 1)goto _LL6A;else{_tmpAD=_tmpAC->f1;}}_LL69:
# 366
*q=*_tmpAD->name;
# 367
return _tmpAD->type;_LL6A: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpAE=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpAA;if(_tmpAE->tag != 2)goto _LL6C;else{_tmpAF=_tmpAE->f1;}}_LL6B:
# 368
*q=*_tmpAF->name;return Cyc_Tcutil_fndecl2typ(_tmpAF);_LL6C: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpB0=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpAA;if(_tmpB0->tag != 5)goto _LL6E;else{_tmpB1=_tmpB0->f1;}}_LL6D:
# 369
 _tmpB3=_tmpB1;goto _LL6F;_LL6E: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB2=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpAA;if(_tmpB2->tag != 4)goto _LL70;else{_tmpB3=_tmpB2->f1;}}_LL6F:
# 370
 _tmpB5=_tmpB3;goto _LL71;_LL70: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpB4=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpAA;if(_tmpB4->tag != 3)goto _LL65;else{_tmpB5=_tmpB4->f1;}}_LL71:
# 372
{union Cyc_Absyn_Nmspace _tmp74D;(*q).f1=(((_tmp74D.Loc_n).val=0,(((_tmp74D.Loc_n).tag=4,_tmp74D))));}
# 373
if(te->allow_valueof){
# 374
void*_tmpE5=Cyc_Tcutil_compress(_tmpB5->type);void*_tmpE7;_LL85: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE6=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpE5;if(_tmpE6->tag != 19)goto _LL87;else{_tmpE7=(void*)_tmpE6->f1;}}_LL86:
# 376
{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp750;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp74F;e->r=(void*)((_tmp74F=_cycalloc(sizeof(*_tmp74F)),((_tmp74F[0]=((_tmp750.tag=37,((_tmp750.f1=(void*)_tmpE7,_tmp750)))),_tmp74F))));}
# 377
goto _LL84;_LL87:;_LL88:
# 378
 goto _LL84;_LL84:;}
# 381
return _tmpB5->type;_LL65:;}
# 385
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,struct _RegionHandle*temp,struct Cyc_List_List*(*type_getter)(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int)){
# 394
struct Cyc_List_List*desc_types;
# 395
{void*_tmpEA=fmt->r;union Cyc_Absyn_Cnst _tmpEC;struct _dyneither_ptr _tmpED;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_Absyn_Exp _tmpF0;void*_tmpF1;union Cyc_Absyn_Cnst _tmpF3;struct _dyneither_ptr _tmpF4;_LL8A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpEB=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEA;if(_tmpEB->tag != 0)goto _LL8C;else{_tmpEC=_tmpEB->f1;if((_tmpEC.String_c).tag != 8)goto _LL8C;_tmpED=(struct _dyneither_ptr)(_tmpEC.String_c).val;}}_LL8B:
# 396
 _tmpF4=_tmpED;goto _LL8D;_LL8C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpEE=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpEA;if(_tmpEE->tag != 13)goto _LL8E;else{_tmpEF=_tmpEE->f2;_tmpF0=*_tmpEF;_tmpF1=_tmpF0.r;{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpF2=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF1;if(_tmpF2->tag != 0)goto _LL8E;else{_tmpF3=_tmpF2->f1;if((_tmpF3.String_c).tag != 8)goto _LL8E;_tmpF4=(struct _dyneither_ptr)(_tmpF3.String_c).val;}};}}_LL8D:
# 398
 desc_types=type_getter(temp,te,(struct _dyneither_ptr)_tmpF4,fmt->loc);goto _LL89;_LL8E:;_LL8F:
# 402
 if(opt_args != 0){
# 403
struct Cyc_List_List*_tmpF5=(struct Cyc_List_List*)opt_args->v;
# 404
for(0;_tmpF5 != 0;_tmpF5=_tmpF5->tl){
# 405
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmpF5->hd);{
# 406
struct _RegionHandle _tmpF6=_new_region("temp");struct _RegionHandle*temp=& _tmpF6;_push_region(temp);
# 407
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_check_null(((struct Cyc_Absyn_Exp*)_tmpF5->hd)->topt)) && !
# 408
Cyc_Tcutil_is_noalias_path(temp,(struct Cyc_Absyn_Exp*)_tmpF5->hd)){
# 409
const char*_tmp753;void*_tmp752;(_tmp752=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpF5->hd)->loc,((_tmp753="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp753,sizeof(char),49))),_tag_dyneither(_tmp752,sizeof(void*),0)));}
# 407
;_pop_region(temp);};}}
# 413
return;_LL89:;}
# 415
if(opt_args != 0){
# 416
struct Cyc_List_List*_tmpF9=(struct Cyc_List_List*)opt_args->v;
# 418
for(0;desc_types != 0  && _tmpF9 != 0;
# 419
(((desc_types=desc_types->tl,_tmpF9=_tmpF9->tl)),arg_cnt ++)){
# 420
int alias_coercion=0;
# 421
void*t=(void*)desc_types->hd;
# 422
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpF9->hd;
# 423
Cyc_Tcexp_tcExp(te,(void**)& t,e);
# 424
if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){
# 425
{const char*_tmp758;void*_tmp757[2];struct Cyc_String_pa_PrintArg_struct _tmp756;struct Cyc_String_pa_PrintArg_struct _tmp755;(_tmp755.tag=0,((_tmp755.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 426
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp756.tag=0,((_tmp756.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp757[0]=& _tmp756,((_tmp757[1]=& _tmp755,Cyc_Tcutil_terr(e->loc,((_tmp758="descriptor has type \n%s\n but argument has type \n%s",_tag_dyneither(_tmp758,sizeof(char),51))),_tag_dyneither(_tmp757,sizeof(void*),2)))))))))))));}
# 427
Cyc_Tcutil_explain_failure();}
# 429
if(alias_coercion){
# 430
struct Cyc_List_List*_tmp759;*alias_arg_exps=(struct Cyc_List_List*)((_tmp759=_cycalloc(sizeof(*_tmp759)),((_tmp759->hd=(void*)arg_cnt,((_tmp759->tl=*alias_arg_exps,_tmp759))))));}{
# 431
struct _RegionHandle _tmpFF=_new_region("temp");struct _RegionHandle*temp=& _tmpFF;_push_region(temp);
# 432
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
# 433
const char*_tmp75C;void*_tmp75B;(_tmp75B=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpF9->hd)->loc,((_tmp75C="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp75C,sizeof(char),49))),_tag_dyneither(_tmp75B,sizeof(void*),0)));}
# 432
;_pop_region(temp);};}
# 437
if(desc_types != 0){
# 438
const char*_tmp75F;void*_tmp75E;(_tmp75E=0,Cyc_Tcutil_terr(fmt->loc,((_tmp75F="too few arguments",_tag_dyneither(_tmp75F,sizeof(char),18))),_tag_dyneither(_tmp75E,sizeof(void*),0)));}
# 439
if(_tmpF9 != 0){
# 440
const char*_tmp762;void*_tmp761;(_tmp761=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpF9->hd)->loc,((_tmp762="too many arguments",_tag_dyneither(_tmp762,sizeof(char),19))),_tag_dyneither(_tmp761,sizeof(void*),0)));}}}
# 443
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 445
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 446
switch(p){case Cyc_Absyn_Plus: _LL90:
# 447
 goto _LL91;case Cyc_Absyn_Minus: _LL91:
# 449
 if(!Cyc_Tcutil_is_numeric(e)){
# 450
const char*_tmp766;void*_tmp765[1];struct Cyc_String_pa_PrintArg_struct _tmp764;(_tmp764.tag=0,((_tmp764.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp765[0]=& _tmp764,Cyc_Tcutil_terr(loc,((_tmp766="expecting arithmetic type but found %s",_tag_dyneither(_tmp766,sizeof(char),39))),_tag_dyneither(_tmp765,sizeof(void*),1)))))));}
# 451
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL92:
# 453
 Cyc_Tcutil_check_contains_assign(e);
# 454
if(!Cyc_Tcutil_coerce_to_bool(te,e)){
# 455
const char*_tmp76A;void*_tmp769[1];struct Cyc_String_pa_PrintArg_struct _tmp768;(_tmp768.tag=0,((_tmp768.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp769[0]=& _tmp768,Cyc_Tcutil_terr(loc,((_tmp76A="expecting integral or * type but found %s",_tag_dyneither(_tmp76A,sizeof(char),42))),_tag_dyneither(_tmp769,sizeof(void*),1)))))));}
# 456
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL93:
# 458
 if(!Cyc_Tcutil_is_integral(e)){
# 459
const char*_tmp76E;void*_tmp76D[1];struct Cyc_String_pa_PrintArg_struct _tmp76C;(_tmp76C.tag=0,((_tmp76C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp76D[0]=& _tmp76C,Cyc_Tcutil_terr(loc,((_tmp76E="expecting integral type but found %s",_tag_dyneither(_tmp76E,sizeof(char),37))),_tag_dyneither(_tmp76D,sizeof(void*),1)))))));}
# 460
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL94:
# 462
{void*_tmp10F=t;struct Cyc_Absyn_PtrInfo _tmp112;struct Cyc_Absyn_PtrAtts _tmp113;union Cyc_Absyn_Constraint*_tmp114;_LL97: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp110=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp10F;if(_tmp110->tag != 8)goto _LL99;}_LL98:
# 463
 goto _LL96;_LL99: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp111=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10F;if(_tmp111->tag != 5)goto _LL9B;else{_tmp112=_tmp111->f1;_tmp113=_tmp112.ptr_atts;_tmp114=_tmp113.bounds;}}_LL9A:
# 465
{void*_tmp115=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp114);struct Cyc_Absyn_Exp*_tmp118;_LL9E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp116=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp115;if(_tmp116->tag != 0)goto _LLA0;}_LL9F:
# 466
 goto _LL9D;_LLA0: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp117=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp115;if(_tmp117->tag != 1)goto _LL9D;else{_tmp118=_tmp117->f1;}}_LLA1:
# 468
 if(!Cyc_Evexp_c_can_eval(_tmp118)){
# 469
const char*_tmp771;void*_tmp770;(_tmp770=0,Cyc_Tcutil_terr(loc,((_tmp771="cannot apply numelts to a pointer with abstract bounds",_tag_dyneither(_tmp771,sizeof(char),55))),_tag_dyneither(_tmp770,sizeof(void*),0)));}
# 470
goto _LL9D;_LL9D:;}
# 472
goto _LL96;_LL9B:;_LL9C: {
# 474
const char*_tmp775;void*_tmp774[1];struct Cyc_String_pa_PrintArg_struct _tmp773;(_tmp773.tag=0,((_tmp773.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp774[0]=& _tmp773,Cyc_Tcutil_terr(loc,((_tmp775="numelts requires pointer or array type, not %s",_tag_dyneither(_tmp775,sizeof(char),47))),_tag_dyneither(_tmp774,sizeof(void*),1)))))));}_LL96:;}
# 476
return Cyc_Absyn_uint_typ;default: _LL95: {
# 477
const char*_tmp778;void*_tmp777;(_tmp777=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp778="Non-unary primop",_tag_dyneither(_tmp778,sizeof(char),17))),_tag_dyneither(_tmp777,sizeof(void*),0)));}}}
# 482
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 485
if(!checker(e1)){
# 486
{const char*_tmp77C;void*_tmp77B[1];struct Cyc_String_pa_PrintArg_struct _tmp77A;(_tmp77A.tag=0,((_tmp77A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp77B[0]=& _tmp77A,Cyc_Tcutil_terr(e1->loc,((_tmp77C="type %s cannot be used here",_tag_dyneither(_tmp77C,sizeof(char),28))),_tag_dyneither(_tmp77B,sizeof(void*),1)))))));}
# 487
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 489
if(!checker(e2)){
# 490
{const char*_tmp780;void*_tmp77F[1];struct Cyc_String_pa_PrintArg_struct _tmp77E;(_tmp77E.tag=0,((_tmp77E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp77F[0]=& _tmp77E,Cyc_Tcutil_terr(e2->loc,((_tmp780="type %s cannot be used here",_tag_dyneither(_tmp780,sizeof(char),28))),_tag_dyneither(_tmp77F,sizeof(void*),1)))))));}
# 491
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 493
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
# 494
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
# 495
return Cyc_Tcutil_max_arithmetic_type(t1,t2);};}
# 498
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 499
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
# 500
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
# 501
void*_tmp126=t1;struct Cyc_Absyn_PtrInfo _tmp128;void*_tmp129;struct Cyc_Absyn_Tqual _tmp12A;struct Cyc_Absyn_PtrAtts _tmp12B;void*_tmp12C;union Cyc_Absyn_Constraint*_tmp12D;union Cyc_Absyn_Constraint*_tmp12E;union Cyc_Absyn_Constraint*_tmp12F;_LLA4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp127=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp126;if(_tmp127->tag != 5)goto _LLA6;else{_tmp128=_tmp127->f1;_tmp129=_tmp128.elt_typ;_tmp12A=_tmp128.elt_tq;_tmp12B=_tmp128.ptr_atts;_tmp12C=_tmp12B.rgn;_tmp12D=_tmp12B.nullable;_tmp12E=_tmp12B.bounds;_tmp12F=_tmp12B.zero_term;}}_LLA5:
# 503
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp129),& Cyc_Tcutil_tmk)){
# 504
const char*_tmp783;void*_tmp782;(_tmp782=0,Cyc_Tcutil_terr(e1->loc,((_tmp783="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp783,sizeof(char),50))),_tag_dyneither(_tmp782,sizeof(void*),0)));}
# 505
if(Cyc_Tcutil_is_noalias_pointer(t1)){
# 506
const char*_tmp786;void*_tmp785;(_tmp785=0,Cyc_Tcutil_terr(e1->loc,((_tmp786="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp786,sizeof(char),54))),_tag_dyneither(_tmp785,sizeof(void*),0)));}
# 507
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
# 508
const char*_tmp78A;void*_tmp789[1];struct Cyc_String_pa_PrintArg_struct _tmp788;(_tmp788.tag=0,((_tmp788.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp789[0]=& _tmp788,Cyc_Tcutil_terr(e2->loc,((_tmp78A="expecting int but found %s",_tag_dyneither(_tmp78A,sizeof(char),27))),_tag_dyneither(_tmp789,sizeof(void*),1)))))));}{
# 509
void*_tmp137=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp12E);struct Cyc_Absyn_Exp*_tmp13A;_LLA9: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp138=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp137;if(_tmp138->tag != 0)goto _LLAB;}_LLAA:
# 510
 return t1;_LLAB: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp139=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp137;if(_tmp139->tag != 1)goto _LLA8;else{_tmp13A=_tmp139->f1;}}_LLAC:
# 514
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp12F)){
# 515
const char*_tmp78D;void*_tmp78C;(_tmp78C=0,Cyc_Tcutil_warn(e1->loc,((_tmp78D="pointer arithmetic on thin, zero-terminated pointer may be expensive.",_tag_dyneither(_tmp78D,sizeof(char),70))),_tag_dyneither(_tmp78C,sizeof(void*),0)));}{
# 522
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp797;struct Cyc_Absyn_PtrAtts _tmp796;struct Cyc_Absyn_PtrInfo _tmp795;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp794;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp13D=(_tmp794=_cycalloc(sizeof(*_tmp794)),((_tmp794[0]=((_tmp797.tag=5,((_tmp797.f1=((_tmp795.elt_typ=_tmp129,((_tmp795.elt_tq=_tmp12A,((_tmp795.ptr_atts=(
# 523
(_tmp796.rgn=_tmp12C,((_tmp796.nullable=Cyc_Absyn_true_conref,((_tmp796.bounds=Cyc_Absyn_bounds_dyneither_conref,((_tmp796.zero_term=_tmp12F,((_tmp796.ptrloc=0,_tmp796)))))))))),_tmp795)))))),_tmp797)))),_tmp794)));
# 526
Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmp13D,Cyc_Absyn_Other_coercion);
# 527
return(void*)_tmp13D;};_LLA8:;};_LLA6:;_LLA7:
# 529
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LLA3:;}
# 534
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 535
void*t1=(void*)_check_null(e1->topt);
# 536
void*t2=(void*)_check_null(e2->topt);
# 537
void*t1_elt=(void*)& Cyc_Absyn_VoidType_val;
# 538
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){
# 539
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
# 540
if(!Cyc_Tcutil_unify(t1,t2)){
# 541
{const char*_tmp79C;void*_tmp79B[2];struct Cyc_String_pa_PrintArg_struct _tmp79A;struct Cyc_String_pa_PrintArg_struct _tmp799;(_tmp799.tag=0,((_tmp799.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 543
Cyc_Absynpp_typ2string(t2)),((_tmp79A.tag=0,((_tmp79A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp79B[0]=& _tmp79A,((_tmp79B[1]=& _tmp799,Cyc_Tcutil_terr(e1->loc,((_tmp79C="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp79C,sizeof(char),59))),_tag_dyneither(_tmp79B,sizeof(void*),2)))))))))))));}
# 544
Cyc_Tcutil_explain_failure();}
# 546
return Cyc_Absyn_sint_typ;}else{
# 548
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){
# 549
const char*_tmp79F;void*_tmp79E;(_tmp79E=0,Cyc_Tcutil_terr(e1->loc,((_tmp79F="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp79F,sizeof(char),50))),_tag_dyneither(_tmp79E,sizeof(void*),0)));}
# 550
if(Cyc_Tcutil_is_noalias_pointer(t1)){
# 551
const char*_tmp7A2;void*_tmp7A1;(_tmp7A1=0,Cyc_Tcutil_terr(e1->loc,((_tmp7A2="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp7A2,sizeof(char),54))),_tag_dyneither(_tmp7A1,sizeof(void*),0)));}
# 552
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
# 553
{const char*_tmp7A7;void*_tmp7A6[2];struct Cyc_String_pa_PrintArg_struct _tmp7A5;struct Cyc_String_pa_PrintArg_struct _tmp7A4;(_tmp7A4.tag=0,((_tmp7A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 554
Cyc_Absynpp_typ2string(t2)),((_tmp7A5.tag=0,((_tmp7A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7A6[0]=& _tmp7A5,((_tmp7A6[1]=& _tmp7A4,Cyc_Tcutil_terr(e2->loc,((_tmp7A7="expecting either %s or int but found %s",_tag_dyneither(_tmp7A7,sizeof(char),40))),_tag_dyneither(_tmp7A6,sizeof(void*),2)))))))))))));}
# 555
Cyc_Tcutil_explain_failure();}
# 557
return t1;}}
# 561
if(Cyc_Tcutil_is_pointer_type(t1))
# 562
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);
# 563
if(Cyc_Tcutil_is_pointer_type(t2))
# 564
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);
# 565
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 568
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 569
int e1_is_num=Cyc_Tcutil_is_numeric(e1);
# 570
int e2_is_num=Cyc_Tcutil_is_numeric(e2);
# 571
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
# 572
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
# 573
if(e1_is_num  && e2_is_num)
# 574
return Cyc_Absyn_sint_typ;else{
# 576
if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind  || 
# 577
Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)))){
# 578
if(Cyc_Tcutil_unify(t1,t2))
# 579
return Cyc_Absyn_sint_typ;else{
# 581
if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){
# 582
Cyc_Tcutil_unchecked_cast(te,e2,t1,Cyc_Absyn_Other_coercion);
# 583
return Cyc_Absyn_sint_typ;}else{
# 584
if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){
# 585
Cyc_Tcutil_unchecked_cast(te,e1,t2,Cyc_Absyn_Other_coercion);
# 586
return Cyc_Absyn_sint_typ;}else{
# 587
if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
# 588
return Cyc_Absyn_sint_typ;else{
# 589
goto pointer_cmp;}}}}}else{
# 594
pointer_cmp: {
# 595
struct _tuple0 _tmp7A8;struct _tuple0 _tmp14F=(_tmp7A8.f1=Cyc_Tcutil_compress(t1),((_tmp7A8.f2=Cyc_Tcutil_compress(t2),_tmp7A8)));void*_tmp150;struct Cyc_Absyn_PtrInfo _tmp152;void*_tmp153;void*_tmp154;struct Cyc_Absyn_PtrInfo _tmp156;void*_tmp157;void*_tmp158;void*_tmp15A;_LLAE: _tmp150=_tmp14F.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp151=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp150;if(_tmp151->tag != 5)goto _LLB0;else{_tmp152=_tmp151->f1;_tmp153=_tmp152.elt_typ;}};_tmp154=_tmp14F.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp155=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp154;if(_tmp155->tag != 5)goto _LLB0;else{_tmp156=_tmp155->f1;_tmp157=_tmp156.elt_typ;}};_LLAF:
# 598
 if(Cyc_Tcutil_unify(_tmp153,_tmp157))return Cyc_Absyn_sint_typ;goto _LLAD;_LLB0: _tmp158=_tmp14F.f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp159=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp158;if(_tmp159->tag != 15)goto _LLB2;};_tmp15A=_tmp14F.f2;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp15B=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp15A;if(_tmp15B->tag != 15)goto _LLB2;};_LLB1:
# 600
 return Cyc_Absyn_sint_typ;_LLB2:;_LLB3:
# 601
 goto _LLAD;_LLAD:;}
# 603
{const char*_tmp7AD;void*_tmp7AC[2];struct Cyc_String_pa_PrintArg_struct _tmp7AB;struct Cyc_String_pa_PrintArg_struct _tmp7AA;(_tmp7AA.tag=0,((_tmp7AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 604
Cyc_Absynpp_typ2string(t2)),((_tmp7AB.tag=0,((_tmp7AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7AC[0]=& _tmp7AB,((_tmp7AC[1]=& _tmp7AA,Cyc_Tcutil_terr(loc,((_tmp7AD="comparison not allowed between %s and %s",_tag_dyneither(_tmp7AD,sizeof(char),41))),_tag_dyneither(_tmp7AC,sizeof(void*),2)))))))))))));}
# 605
Cyc_Tcutil_explain_failure();
# 606
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}}
# 610
static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 612
if(Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2)== Cyc_Absyn_sint_typ)
# 613
return Cyc_Absyn_sint_typ;{
# 618
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
# 619
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
# 620
{struct _tuple0 _tmp7AE;struct _tuple0 _tmp161=(_tmp7AE.f1=t1,((_tmp7AE.f2=t2,_tmp7AE)));void*_tmp162;void*_tmp164;void*_tmp165;void*_tmp167;_LLB5: _tmp162=_tmp161.f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp163=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp162;if(_tmp163->tag != 15)goto _LLB7;else{_tmp164=(void*)_tmp163->f1;}};_tmp165=_tmp161.f2;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp166=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp165;if(_tmp166->tag != 15)goto _LLB7;else{_tmp167=(void*)_tmp166->f1;}};_LLB6:
# 623
 return Cyc_Absyn_sint_typ;_LLB7:;_LLB8:
# 624
 goto _LLB4;_LLB4:;}
# 626
{const char*_tmp7B3;void*_tmp7B2[2];struct Cyc_String_pa_PrintArg_struct _tmp7B1;struct Cyc_String_pa_PrintArg_struct _tmp7B0;(_tmp7B0.tag=0,((_tmp7B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 627
Cyc_Absynpp_typ2string(t2)),((_tmp7B1.tag=0,((_tmp7B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7B2[0]=& _tmp7B1,((_tmp7B2[1]=& _tmp7B0,Cyc_Tcutil_terr(loc,((_tmp7B3="comparison not allowed between %s and %s",_tag_dyneither(_tmp7B3,sizeof(char),41))),_tag_dyneither(_tmp7B2,sizeof(void*),2)))))))))))));}
# 628
Cyc_Tcutil_explain_failure();
# 629
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));};}
# 634
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 636
switch(p){case Cyc_Absyn_Plus: _LLB9:
# 637
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus: _LLBA:
# 638
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LLBB:
# 640
 goto _LLBC;case Cyc_Absyn_Div: _LLBC:
# 641
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod: _LLBD:
# 643
 goto _LLBE;case Cyc_Absyn_Bitand: _LLBE:
# 644
 goto _LLBF;case Cyc_Absyn_Bitor: _LLBF:
# 645
 goto _LLC0;case Cyc_Absyn_Bitxor: _LLC0:
# 646
 goto _LLC1;case Cyc_Absyn_Bitlshift: _LLC1:
# 647
 goto _LLC2;case Cyc_Absyn_Bitlrshift: _LLC2:
# 648
 goto _LLC3;case Cyc_Absyn_Bitarshift: _LLC3:
# 649
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq: _LLC4:
# 653
 goto _LLC5;case Cyc_Absyn_Neq: _LLC5:
# 654
 return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);case Cyc_Absyn_Gt: _LLC6:
# 656
 goto _LLC7;case Cyc_Absyn_Lt: _LLC7:
# 657
 goto _LLC8;case Cyc_Absyn_Gte: _LLC8:
# 658
 goto _LLC9;case Cyc_Absyn_Lte: _LLC9:
# 659
 return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);default: _LLCA: {
# 661
const char*_tmp7B6;void*_tmp7B5;(_tmp7B5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7B6="bad binary primop",_tag_dyneither(_tmp7B6,sizeof(char),18))),_tag_dyneither(_tmp7B5,sizeof(void*),0)));}}}
# 665
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 673
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1)
# 674
return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
# 675
Cyc_Tcexp_tcExpList(te,es);{
# 676
void*t;
# 677
switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case 0: _LLCC: {
# 678
const char*_tmp7B9;void*_tmp7B8;return(_tmp7B8=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7B9="primitive operator has 0 arguments",_tag_dyneither(_tmp7B9,sizeof(char),35))),_tag_dyneither(_tmp7B8,sizeof(void*),0)));}case 1: _LLCD:
# 679
 t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);break;case 2: _LLCE:
# 680
 t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default: _LLCF: {
# 681
const char*_tmp7BC;void*_tmp7BB;return(_tmp7BB=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7BC="primitive operator has > 2 arguments",_tag_dyneither(_tmp7BC,sizeof(char),37))),_tag_dyneither(_tmp7BB,sizeof(void*),0)));}}
# 683
return t;};}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 686
static int Cyc_Tcexp_check_writable_aggr(unsigned int loc,void*t){
# 687
void*_tmp172=Cyc_Tcutil_compress(t);
# 688
void*_tmp173=_tmp172;struct Cyc_Absyn_AggrInfo _tmp175;union Cyc_Absyn_AggrInfoU _tmp176;struct Cyc_Absyn_Aggrdecl**_tmp177;struct Cyc_Absyn_Aggrdecl*_tmp178;struct Cyc_List_List*_tmp17A;struct Cyc_Absyn_DatatypeFieldInfo _tmp17C;union Cyc_Absyn_DatatypeFieldInfoU _tmp17D;struct _tuple2 _tmp17E;struct Cyc_Absyn_Datatypefield*_tmp17F;struct Cyc_Absyn_ArrayInfo _tmp181;void*_tmp182;struct Cyc_Absyn_Tqual _tmp183;struct Cyc_List_List*_tmp185;_LLD2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp174=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp173;if(_tmp174->tag != 11)goto _LLD4;else{_tmp175=_tmp174->f1;_tmp176=_tmp175.aggr_info;if((_tmp176.KnownAggr).tag != 2)goto _LLD4;_tmp177=(struct Cyc_Absyn_Aggrdecl**)(_tmp176.KnownAggr).val;_tmp178=*_tmp177;}}_LLD3:
# 690
 _tmp17A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp178->impl))->fields;goto _LLD5;_LLD4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp179=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp173;if(_tmp179->tag != 12)goto _LLD6;else{_tmp17A=_tmp179->f2;}}_LLD5:
# 692
 for(0;_tmp17A != 0;_tmp17A=_tmp17A->tl){
# 693
struct Cyc_Absyn_Aggrfield*_tmp186=(struct Cyc_Absyn_Aggrfield*)_tmp17A->hd;
# 694
if((_tmp186->tq).real_const){
# 695
{const char*_tmp7C0;void*_tmp7BF[1];struct Cyc_String_pa_PrintArg_struct _tmp7BE;(_tmp7BE.tag=0,((_tmp7BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp186->name),((_tmp7BF[0]=& _tmp7BE,Cyc_Tcutil_terr(loc,((_tmp7C0="attempt to over-write an aggregate with const member %s",_tag_dyneither(_tmp7C0,sizeof(char),56))),_tag_dyneither(_tmp7BF,sizeof(void*),1)))))));}
# 696
return 0;}
# 698
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp186->type))return 0;}
# 700
return 1;_LLD6: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp17B=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp173;if(_tmp17B->tag != 4)goto _LLD8;else{_tmp17C=_tmp17B->f1;_tmp17D=_tmp17C.field_info;if((_tmp17D.KnownDatatypefield).tag != 2)goto _LLD8;_tmp17E=(struct _tuple2)(_tmp17D.KnownDatatypefield).val;_tmp17F=_tmp17E.f2;}}_LLD7:
# 702
{struct Cyc_List_List*_tmp18A=_tmp17F->typs;for(0;_tmp18A != 0;_tmp18A=_tmp18A->tl){
# 703
struct _tuple19 _tmp18C;struct Cyc_Absyn_Tqual _tmp18D;void*_tmp18E;struct _tuple19*_tmp18B=(struct _tuple19*)_tmp18A->hd;_tmp18C=*_tmp18B;_tmp18D=_tmp18C.f1;_tmp18E=_tmp18C.f2;
# 704
if(_tmp18D.real_const){
# 705
{const char*_tmp7C4;void*_tmp7C3[1];struct Cyc_String_pa_PrintArg_struct _tmp7C2;(_tmp7C2.tag=0,((_tmp7C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp17F->name)),((_tmp7C3[0]=& _tmp7C2,Cyc_Tcutil_terr(loc,((_tmp7C4="attempt to over-write a datatype field (%s) with a const member",_tag_dyneither(_tmp7C4,sizeof(char),64))),_tag_dyneither(_tmp7C3,sizeof(void*),1)))))));}
# 706
return 0;}
# 708
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp18E))return 0;}}
# 710
return 1;_LLD8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp180=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp173;if(_tmp180->tag != 8)goto _LLDA;else{_tmp181=_tmp180->f1;_tmp182=_tmp181.elt_type;_tmp183=_tmp181.tq;}}_LLD9:
# 712
 if(_tmp183.real_const){
# 713
{const char*_tmp7C7;void*_tmp7C6;(_tmp7C6=0,Cyc_Tcutil_terr(loc,((_tmp7C7="attempt to over-write a const array",_tag_dyneither(_tmp7C7,sizeof(char),36))),_tag_dyneither(_tmp7C6,sizeof(void*),0)));}
# 714
return 0;}
# 716
return Cyc_Tcexp_check_writable_aggr(loc,_tmp182);_LLDA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp184=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp173;if(_tmp184->tag != 10)goto _LLDC;else{_tmp185=_tmp184->f1;}}_LLDB:
# 718
 for(0;_tmp185 != 0;_tmp185=_tmp185->tl){
# 719
struct _tuple19 _tmp195;struct Cyc_Absyn_Tqual _tmp196;void*_tmp197;struct _tuple19*_tmp194=(struct _tuple19*)_tmp185->hd;_tmp195=*_tmp194;_tmp196=_tmp195.f1;_tmp197=_tmp195.f2;
# 720
if(_tmp196.real_const){
# 721
{const char*_tmp7CA;void*_tmp7C9;(_tmp7C9=0,Cyc_Tcutil_terr(loc,((_tmp7CA="attempt to over-write a tuple field with a const member",_tag_dyneither(_tmp7CA,sizeof(char),56))),_tag_dyneither(_tmp7C9,sizeof(void*),0)));}
# 722
return 0;}
# 724
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp197))return 0;}
# 726
return 1;_LLDC:;_LLDD:
# 727
 return 1;_LLD1:;}
# 734
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 737
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
# 738
{void*_tmp19A=e->r;void*_tmp19C;struct Cyc_Absyn_Vardecl*_tmp19E;void*_tmp1A0;struct Cyc_Absyn_Vardecl*_tmp1A2;void*_tmp1A4;struct Cyc_Absyn_Vardecl*_tmp1A6;void*_tmp1A8;struct Cyc_Absyn_Vardecl*_tmp1AA;struct Cyc_Absyn_Exp*_tmp1AC;struct Cyc_Absyn_Exp*_tmp1AD;struct Cyc_Absyn_Exp*_tmp1AF;struct _dyneither_ptr*_tmp1B0;struct Cyc_Absyn_Exp*_tmp1B2;struct _dyneither_ptr*_tmp1B3;struct Cyc_Absyn_Exp*_tmp1B5;struct Cyc_Absyn_Exp*_tmp1B7;struct Cyc_Absyn_Exp*_tmp1B9;_LLDF: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp19B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp19B->tag != 1)goto _LLE1;else{_tmp19C=(void*)_tmp19B->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp19D=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp19C;if(_tmp19D->tag != 3)goto _LLE1;else{_tmp19E=_tmp19D->f1;}};}}_LLE0:
# 739
 _tmp1A2=_tmp19E;goto _LLE2;_LLE1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp19F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp19F->tag != 1)goto _LLE3;else{_tmp1A0=(void*)_tmp19F->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp1A1=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp1A0;if(_tmp1A1->tag != 4)goto _LLE3;else{_tmp1A2=_tmp1A1->f1;}};}}_LLE2:
# 740
 _tmp1A6=_tmp1A2;goto _LLE4;_LLE3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1A3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1A3->tag != 1)goto _LLE5;else{_tmp1A4=(void*)_tmp1A3->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp1A5=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp1A4;if(_tmp1A5->tag != 5)goto _LLE5;else{_tmp1A6=_tmp1A5->f1;}};}}_LLE4:
# 741
 _tmp1AA=_tmp1A6;goto _LLE6;_LLE5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1A7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1A7->tag != 1)goto _LLE7;else{_tmp1A8=(void*)_tmp1A7->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp1A9=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp1A8;if(_tmp1A9->tag != 1)goto _LLE7;else{_tmp1AA=_tmp1A9->f1;}};}}_LLE6:
# 742
 if(!(_tmp1AA->tq).real_const)return;goto _LLDE;_LLE7: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp1AB=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1AB->tag != 22)goto _LLE9;else{_tmp1AC=_tmp1AB->f1;_tmp1AD=_tmp1AB->f2;}}_LLE8:
# 744
{void*_tmp1BA=Cyc_Tcutil_compress((void*)_check_null(_tmp1AC->topt));struct Cyc_Absyn_PtrInfo _tmp1BC;struct Cyc_Absyn_Tqual _tmp1BD;struct Cyc_Absyn_ArrayInfo _tmp1BF;struct Cyc_Absyn_Tqual _tmp1C0;struct Cyc_List_List*_tmp1C2;_LLF6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1BB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BA;if(_tmp1BB->tag != 5)goto _LLF8;else{_tmp1BC=_tmp1BB->f1;_tmp1BD=_tmp1BC.elt_tq;}}_LLF7:
# 745
 _tmp1C0=_tmp1BD;goto _LLF9;_LLF8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1BE=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1BA;if(_tmp1BE->tag != 8)goto _LLFA;else{_tmp1BF=_tmp1BE->f1;_tmp1C0=_tmp1BF.tq;}}_LLF9:
# 746
 if(!_tmp1C0.real_const)return;goto _LLF5;_LLFA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1C1=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1BA;if(_tmp1C1->tag != 10)goto _LLFC;else{_tmp1C2=_tmp1C1->f1;}}_LLFB: {
# 748
unsigned int _tmp1C4;int _tmp1C5;struct _tuple15 _tmp1C3=Cyc_Evexp_eval_const_uint_exp(_tmp1AD);_tmp1C4=_tmp1C3.f1;_tmp1C5=_tmp1C3.f2;
# 749
if(!_tmp1C5){
# 750
{const char*_tmp7CD;void*_tmp7CC;(_tmp7CC=0,Cyc_Tcutil_terr(e->loc,((_tmp7CD="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp7CD,sizeof(char),47))),_tag_dyneither(_tmp7CC,sizeof(void*),0)));}
# 751
return;}
# 753
{struct _handler_cons _tmp1C8;_push_handler(& _tmp1C8);{int _tmp1CA=0;if(setjmp(_tmp1C8.handler))_tmp1CA=1;if(!_tmp1CA){
# 754
{struct _tuple19 _tmp1CC;struct Cyc_Absyn_Tqual _tmp1CD;struct _tuple19*_tmp1CB=((struct _tuple19*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp1C2,(int)_tmp1C4);_tmp1CC=*_tmp1CB;_tmp1CD=_tmp1CC.f1;
# 755
if(!_tmp1CD.real_const){_npop_handler(0);return;}}
# 754
;_pop_handler();}else{void*_tmp1C9=(void*)_exn_thrown;void*_tmp1CF=_tmp1C9;_LLFF: {struct Cyc_List_Nth_exn_struct*_tmp1D0=(struct Cyc_List_Nth_exn_struct*)_tmp1CF;if(_tmp1D0->tag != Cyc_List_Nth)goto _LL101;}_LL100:
# 756
 return;_LL101:;_LL102:(void)_throw(_tmp1CF);_LLFE:;}};}
# 757
goto _LLF5;}_LLFC:;_LLFD:
# 758
 goto _LLF5;_LLF5:;}
# 760
goto _LLDE;_LLE9: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp1AE=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1AE->tag != 20)goto _LLEB;else{_tmp1AF=_tmp1AE->f1;_tmp1B0=_tmp1AE->f2;}}_LLEA:
# 762
{void*_tmp1D1=Cyc_Tcutil_compress((void*)_check_null(_tmp1AF->topt));struct Cyc_Absyn_AggrInfo _tmp1D3;union Cyc_Absyn_AggrInfoU _tmp1D4;struct Cyc_Absyn_Aggrdecl**_tmp1D5;struct Cyc_List_List*_tmp1D7;_LL104: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1D2=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1D1;if(_tmp1D2->tag != 11)goto _LL106;else{_tmp1D3=_tmp1D2->f1;_tmp1D4=_tmp1D3.aggr_info;if((_tmp1D4.KnownAggr).tag != 2)goto _LL106;_tmp1D5=(struct Cyc_Absyn_Aggrdecl**)(_tmp1D4.KnownAggr).val;}}_LL105: {
# 764
struct Cyc_Absyn_Aggrfield*sf=
# 765
(struct Cyc_Absyn_Aggrdecl**)_tmp1D5 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp1D5,_tmp1B0);
# 766
if(sf == 0  || !(sf->tq).real_const)return;
# 767
goto _LL103;}_LL106: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1D6=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1D1;if(_tmp1D6->tag != 12)goto _LL108;else{_tmp1D7=_tmp1D6->f2;}}_LL107: {
# 769
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp1D7,_tmp1B0);
# 770
if(sf == 0  || !(sf->tq).real_const)return;
# 771
goto _LL103;}_LL108:;_LL109:
# 772
 goto _LL103;_LL103:;}
# 774
goto _LLDE;_LLEB: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp1B1=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1B1->tag != 21)goto _LLED;else{_tmp1B2=_tmp1B1->f1;_tmp1B3=_tmp1B1->f2;}}_LLEC:
# 776
{void*_tmp1D8=Cyc_Tcutil_compress((void*)_check_null(_tmp1B2->topt));struct Cyc_Absyn_PtrInfo _tmp1DA;void*_tmp1DB;_LL10B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1D9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D8;if(_tmp1D9->tag != 5)goto _LL10D;else{_tmp1DA=_tmp1D9->f1;_tmp1DB=_tmp1DA.elt_typ;}}_LL10C:
# 778
{void*_tmp1DC=Cyc_Tcutil_compress(_tmp1DB);struct Cyc_Absyn_AggrInfo _tmp1DE;union Cyc_Absyn_AggrInfoU _tmp1DF;struct Cyc_Absyn_Aggrdecl**_tmp1E0;struct Cyc_List_List*_tmp1E2;_LL110: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1DD=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1DC;if(_tmp1DD->tag != 11)goto _LL112;else{_tmp1DE=_tmp1DD->f1;_tmp1DF=_tmp1DE.aggr_info;if((_tmp1DF.KnownAggr).tag != 2)goto _LL112;_tmp1E0=(struct Cyc_Absyn_Aggrdecl**)(_tmp1DF.KnownAggr).val;}}_LL111: {
# 780
struct Cyc_Absyn_Aggrfield*sf=
# 781
(struct Cyc_Absyn_Aggrdecl**)_tmp1E0 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp1E0,_tmp1B3);
# 782
if(sf == 0  || !(sf->tq).real_const)return;
# 783
goto _LL10F;}_LL112: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1E1=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1DC;if(_tmp1E1->tag != 12)goto _LL114;else{_tmp1E2=_tmp1E1->f2;}}_LL113: {
# 785
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp1E2,_tmp1B3);
# 786
if(sf == 0  || !(sf->tq).real_const)return;
# 787
goto _LL10F;}_LL114:;_LL115:
# 788
 goto _LL10F;_LL10F:;}
# 790
goto _LL10A;_LL10D:;_LL10E:
# 791
 goto _LL10A;_LL10A:;}
# 793
goto _LLDE;_LLED: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp1B4=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1B4->tag != 19)goto _LLEF;else{_tmp1B5=_tmp1B4->f1;}}_LLEE:
# 795
{void*_tmp1E3=Cyc_Tcutil_compress((void*)_check_null(_tmp1B5->topt));struct Cyc_Absyn_PtrInfo _tmp1E5;struct Cyc_Absyn_Tqual _tmp1E6;struct Cyc_Absyn_ArrayInfo _tmp1E8;struct Cyc_Absyn_Tqual _tmp1E9;_LL117: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1E4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E3;if(_tmp1E4->tag != 5)goto _LL119;else{_tmp1E5=_tmp1E4->f1;_tmp1E6=_tmp1E5.elt_tq;}}_LL118:
# 796
 _tmp1E9=_tmp1E6;goto _LL11A;_LL119: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1E7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1E3;if(_tmp1E7->tag != 8)goto _LL11B;else{_tmp1E8=_tmp1E7->f1;_tmp1E9=_tmp1E8.tq;}}_LL11A:
# 797
 if(!_tmp1E9.real_const)return;goto _LL116;_LL11B:;_LL11C:
# 798
 goto _LL116;_LL116:;}
# 800
goto _LLDE;_LLEF: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp1B6=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1B6->tag != 11)goto _LLF1;else{_tmp1B7=_tmp1B6->f1;}}_LLF0:
# 801
 _tmp1B9=_tmp1B7;goto _LLF2;_LLF1: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp1B8=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1B8->tag != 12)goto _LLF3;else{_tmp1B9=_tmp1B8->f1;}}_LLF2:
# 802
 Cyc_Tcexp_check_writable(te,_tmp1B9);return;_LLF3:;_LLF4:
# 803
 goto _LLDE;_LLDE:;}{
# 805
const char*_tmp7D1;void*_tmp7D0[1];struct Cyc_String_pa_PrintArg_struct _tmp7CF;(_tmp7CF.tag=0,((_tmp7CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp7D0[0]=& _tmp7CF,Cyc_Tcutil_terr(e->loc,((_tmp7D1="attempt to write a const location: %s",_tag_dyneither(_tmp7D1,sizeof(char),38))),_tag_dyneither(_tmp7D0,sizeof(void*),1)))))));};}
# 808
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 811
struct _RegionHandle _tmp1ED=_new_region("temp");struct _RegionHandle*temp=& _tmp1ED;_push_region(temp);
# 812
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(temp,te),0,e);
# 814
if(!Cyc_Absyn_is_lvalue(e)){
# 815
const char*_tmp7D4;void*_tmp7D3;void*_tmp1F0=(_tmp7D3=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7D4="increment/decrement of non-lvalue",_tag_dyneither(_tmp7D4,sizeof(char),34))),_tag_dyneither(_tmp7D3,sizeof(void*),0)));_npop_handler(0);return _tmp1F0;}
# 816
Cyc_Tcexp_check_writable(te,e);{
# 817
void*t=(void*)_check_null(e->topt);
# 819
if(!Cyc_Tcutil_is_numeric(e)){
# 820
void*telt=(void*)& Cyc_Absyn_VoidType_val;
# 821
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
# 822
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
# 823
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk)){
# 824
const char*_tmp7D7;void*_tmp7D6;(_tmp7D6=0,Cyc_Tcutil_terr(e->loc,((_tmp7D7="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp7D7,sizeof(char),50))),_tag_dyneither(_tmp7D6,sizeof(void*),0)));}
# 825
if(Cyc_Tcutil_is_noalias_pointer(t)){
# 826
const char*_tmp7DA;void*_tmp7D9;(_tmp7D9=0,Cyc_Tcutil_terr(e->loc,((_tmp7DA="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp7DA,sizeof(char),54))),_tag_dyneither(_tmp7D9,sizeof(void*),0)));}}else{
# 828
const char*_tmp7DE;void*_tmp7DD[1];struct Cyc_String_pa_PrintArg_struct _tmp7DC;(_tmp7DC.tag=0,((_tmp7DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp7DD[0]=& _tmp7DC,Cyc_Tcutil_terr(e->loc,((_tmp7DE="expecting arithmetic or ? type but found %s",_tag_dyneither(_tmp7DE,sizeof(char),44))),_tag_dyneither(_tmp7DD,sizeof(void*),1)))))));}}{
# 830
void*_tmp1F8=t;_npop_handler(0);return _tmp1F8;};};
# 811
;_pop_region(temp);}
# 834
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 836
struct _tuple0*_tmp1FB;int _tmp1FC;const char*_tmp7DF;struct Cyc_Tcexp_TestEnv _tmp1FA=Cyc_Tcexp_tcTest(te,e1,((_tmp7DF="conditional expression",_tag_dyneither(_tmp7DF,sizeof(char),23))));_tmp1FB=_tmp1FA.eq;_tmp1FC=_tmp1FA.isTrue;
# 837
Cyc_Tcexp_tcExp(te,topt,e2);
# 838
Cyc_Tcexp_tcExp(te,topt,e3);{
# 839
void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
# 840
struct Cyc_List_List _tmp7E0;struct Cyc_List_List _tmp1FD=(_tmp7E0.hd=e3,((_tmp7E0.tl=0,_tmp7E0)));
# 841
struct Cyc_List_List _tmp7E1;struct Cyc_List_List _tmp1FE=(_tmp7E1.hd=e2,((_tmp7E1.tl=(struct Cyc_List_List*)& _tmp1FD,_tmp7E1)));
# 842
if(!Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)& _tmp1FE)){
# 843
{const char*_tmp7E6;void*_tmp7E5[2];struct Cyc_String_pa_PrintArg_struct _tmp7E4;struct Cyc_String_pa_PrintArg_struct _tmp7E3;(_tmp7E3.tag=0,((_tmp7E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 844
Cyc_Absynpp_typ2string((void*)_check_null(e3->topt))),((_tmp7E4.tag=0,((_tmp7E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp7E5[0]=& _tmp7E4,((_tmp7E5[1]=& _tmp7E3,Cyc_Tcutil_terr(loc,((_tmp7E6="conditional clause types do not match: %s != %s",_tag_dyneither(_tmp7E6,sizeof(char),48))),_tag_dyneither(_tmp7E5,sizeof(void*),2)))))))))))));}
# 845
Cyc_Tcutil_explain_failure();}
# 847
return t;};}
# 851
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 853
{const char*_tmp7E7;Cyc_Tcexp_tcTest(te,e1,((_tmp7E7="logical-and expression",_tag_dyneither(_tmp7E7,sizeof(char),23))));}
# 854
{const char*_tmp7E8;Cyc_Tcexp_tcTest(te,e2,((_tmp7E8="logical-and expression",_tag_dyneither(_tmp7E8,sizeof(char),23))));}
# 855
return Cyc_Absyn_sint_typ;}
# 859
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 861
{const char*_tmp7E9;Cyc_Tcexp_tcTest(te,e1,((_tmp7E9="logical-or expression",_tag_dyneither(_tmp7E9,sizeof(char),22))));}
# 862
{const char*_tmp7EA;Cyc_Tcexp_tcTest(te,e2,((_tmp7EA="logical-or expression",_tag_dyneither(_tmp7EA,sizeof(char),22))));}
# 863
return Cyc_Absyn_sint_typ;}
# 867
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 874
struct _RegionHandle _tmp209=_new_region("r");struct _RegionHandle*r=& _tmp209;_push_region(r);
# 875
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te)),0,e1);{
# 877
void*t1=(void*)_check_null(e1->topt);
# 878
Cyc_Tcexp_tcExp(te,(void**)& t1,e2);{
# 879
void*t2=(void*)_check_null(e2->topt);
# 881
{void*_tmp20A=Cyc_Tcutil_compress(t1);_LL11E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp20B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp20A;if(_tmp20B->tag != 8)goto _LL120;}_LL11F:
# 882
{const char*_tmp7ED;void*_tmp7EC;(_tmp7EC=0,Cyc_Tcutil_terr(loc,((_tmp7ED="cannot assign to an array",_tag_dyneither(_tmp7ED,sizeof(char),26))),_tag_dyneither(_tmp7EC,sizeof(void*),0)));}goto _LL11D;_LL120:;_LL121:
# 883
 goto _LL11D;_LL11D:;}
# 886
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk)){
# 887
const char*_tmp7F0;void*_tmp7EF;(_tmp7EF=0,Cyc_Tcutil_terr(loc,((_tmp7F0="type is abstract (can't determine size).",_tag_dyneither(_tmp7F0,sizeof(char),41))),_tag_dyneither(_tmp7EF,sizeof(void*),0)));}
# 890
if(!Cyc_Absyn_is_lvalue(e1)){
# 891
const char*_tmp7F3;void*_tmp7F2;void*_tmp212=(_tmp7F2=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7F3="assignment to non-lvalue",_tag_dyneither(_tmp7F3,sizeof(char),25))),_tag_dyneither(_tmp7F2,sizeof(void*),0)));_npop_handler(0);return _tmp212;}
# 892
Cyc_Tcexp_check_writable(te,e1);
# 893
if(po == 0){
# 894
struct _RegionHandle _tmp213=_new_region("temp");struct _RegionHandle*temp=& _tmp213;_push_region(temp);
# 895
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t2) && !Cyc_Tcutil_is_noalias_path(temp,e2)){
# 896
const char*_tmp7F6;void*_tmp7F5;(_tmp7F5=0,Cyc_Tcutil_terr(e2->loc,((_tmp7F6="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp7F6,sizeof(char),49))),_tag_dyneither(_tmp7F5,sizeof(void*),0)));}
# 898
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
# 899
const char*_tmp7FB;void*_tmp7FA[2];struct Cyc_String_pa_PrintArg_struct _tmp7F9;struct Cyc_String_pa_PrintArg_struct _tmp7F8;void*_tmp216=(_tmp7F8.tag=0,((_tmp7F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 900
Cyc_Absynpp_typ2string(t2)),((_tmp7F9.tag=0,((_tmp7F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7FA[0]=& _tmp7F9,((_tmp7FA[1]=& _tmp7F8,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7FB="type mismatch: %s != %s",_tag_dyneither(_tmp7FB,sizeof(char),24))),_tag_dyneither(_tmp7FA,sizeof(void*),2)))))))))))));
# 901
Cyc_Tcutil_unify(t1,t2);
# 902
Cyc_Tcutil_explain_failure();{
# 903
void*_tmp217=_tmp216;_npop_handler(1);return _tmp217;};}
# 894
;_pop_region(temp);}else{
# 906
enum Cyc_Absyn_Primop _tmp21C=(enum Cyc_Absyn_Primop)po->v;
# 907
void*_tmp21D=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp21C,e1,e2);
# 908
if(!(Cyc_Tcutil_unify(_tmp21D,t1) || Cyc_Tcutil_coerceable(_tmp21D) && Cyc_Tcutil_coerceable(t1))){
# 909
const char*_tmp800;void*_tmp7FF[2];struct Cyc_String_pa_PrintArg_struct _tmp7FE;struct Cyc_String_pa_PrintArg_struct _tmp7FD;void*_tmp21E=
# 910
(_tmp7FD.tag=0,((_tmp7FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 912
Cyc_Absynpp_typ2string(t2)),((_tmp7FE.tag=0,((_tmp7FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 911
Cyc_Absynpp_typ2string(t1)),((_tmp7FF[0]=& _tmp7FE,((_tmp7FF[1]=& _tmp7FD,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp800="Cannot use this operator in an assignment when the arguments have types %s and %s",_tag_dyneither(_tmp800,sizeof(char),82))),_tag_dyneither(_tmp7FF,sizeof(void*),2)))))))))))));
# 913
Cyc_Tcutil_unify(_tmp21D,t1);
# 914
Cyc_Tcutil_explain_failure();{
# 915
void*_tmp21F=_tmp21E;_npop_handler(0);return _tmp21F;};}{
# 917
void*_tmp224=_tmp21D;_npop_handler(0);return _tmp224;};}{
# 919
void*_tmp225=t1;_npop_handler(0);return _tmp225;};};};
# 874
;_pop_region(r);}
# 923
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 924
Cyc_Tcexp_tcExp(te,0,e1);
# 925
Cyc_Tcexp_tcExp(te,topt,e2);
# 926
return(void*)_check_null(e2->topt);}
# 930
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 934
struct Cyc_List_List*fields;
# 935
void*t1=(void*)_check_null(e->topt);
# 937
{void*_tmp226=Cyc_Tcutil_compress(t1);int _tmp228;enum Cyc_Absyn_Size_of _tmp22A;enum Cyc_Absyn_Size_of _tmp22C;_LL123: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp227=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp226;if(_tmp227->tag != 7)goto _LL125;else{_tmp228=_tmp227->f1;if(_tmp228 != 0)goto _LL125;}}_LL124:
# 938
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_float_typ(1),Cyc_Absyn_No_coercion);t1=Cyc_Absyn_float_typ(1);goto _LL122;_LL125: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp229=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp226;if(_tmp229->tag != 6)goto _LL127;else{_tmp22A=_tmp229->f2;if(_tmp22A != Cyc_Absyn_Char_sz)goto _LL127;}}_LL126:
# 939
 goto _LL128;_LL127: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp22B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp226;if(_tmp22B->tag != 6)goto _LL129;else{_tmp22C=_tmp22B->f2;if(_tmp22C != Cyc_Absyn_Short_sz)goto _LL129;}}_LL128:
# 940
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_typ;goto _LL122;_LL129:;_LL12A:
# 941
 goto _LL122;_LL122:;}
# 944
for(fields=fs;fields != 0;fields=fields->tl){
# 945
struct _tuple1*_tmp22E;struct Cyc_List_List*_tmp22F;unsigned int _tmp230;enum Cyc_Absyn_Scope _tmp231;struct Cyc_Absyn_Datatypefield _tmp22D=*((struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp22E=_tmp22D.name;_tmp22F=_tmp22D.typs;_tmp230=_tmp22D.loc;_tmp231=_tmp22D.sc;
# 947
if(_tmp22F == 0  || _tmp22F->tl != 0)continue;{
# 948
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple19*)_tmp22F->hd)).f2);
# 950
if(Cyc_Tcutil_unify(t1,t2))
# 951
return(struct Cyc_Absyn_Datatypefield*)((struct Cyc_Absyn_Datatypefield*)fields->hd);};}
# 955
for(fields=fs;fields != 0;fields=fields->tl){
# 956
struct _tuple1*_tmp233;struct Cyc_List_List*_tmp234;unsigned int _tmp235;enum Cyc_Absyn_Scope _tmp236;struct Cyc_Absyn_Datatypefield _tmp232=*((struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp233=_tmp232.name;_tmp234=_tmp232.typs;_tmp235=_tmp232.loc;_tmp236=_tmp232.sc;
# 958
if(_tmp234 == 0  || _tmp234->tl != 0)continue;{
# 959
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple19*)_tmp234->hd)).f2);
# 962
int bogus=0;
# 963
if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))
# 964
return(struct Cyc_Absyn_Datatypefield*)((struct Cyc_Absyn_Datatypefield*)fields->hd);};}
# 967
{const char*_tmp805;void*_tmp804[2];struct Cyc_String_pa_PrintArg_struct _tmp803;struct Cyc_String_pa_PrintArg_struct _tmp802;(_tmp802.tag=0,((_tmp802.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 968
Cyc_Absynpp_typ2string(t1)),((_tmp803.tag=0,((_tmp803.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu)),((_tmp804[0]=& _tmp803,((_tmp804[1]=& _tmp802,Cyc_Tcutil_terr(e->loc,((_tmp805="can't find a field in %s to inject a value of type %s",_tag_dyneither(_tmp805,sizeof(char),54))),_tag_dyneither(_tmp804,sizeof(void*),2)))))))))))));}
# 969
return 0;}
# 973
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 979
struct Cyc_List_List*_tmp23B=args;
# 980
int _tmp23C=0;
# 981
struct _RegionHandle _tmp23D=_new_region("ter");struct _RegionHandle*ter=& _tmp23D;_push_region(ter);
# 982
{struct Cyc_Tcenv_Tenv*_tmp23E=Cyc_Tcenv_new_block(ter,loc,te_orig);
# 983
Cyc_Tcexp_tcExp(_tmp23E,0,e);{
# 984
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 988
void*_tmp23F=t;struct Cyc_Absyn_PtrInfo _tmp241;void*_tmp242;struct Cyc_Absyn_Tqual _tmp243;struct Cyc_Absyn_PtrAtts _tmp244;void*_tmp245;union Cyc_Absyn_Constraint*_tmp246;union Cyc_Absyn_Constraint*_tmp247;union Cyc_Absyn_Constraint*_tmp248;_LL12C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp240=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23F;if(_tmp240->tag != 5)goto _LL12E;else{_tmp241=_tmp240->f1;_tmp242=_tmp241.elt_typ;_tmp243=_tmp241.elt_tq;_tmp244=_tmp241.ptr_atts;_tmp245=_tmp244.rgn;_tmp246=_tmp244.nullable;_tmp247=_tmp244.bounds;_tmp248=_tmp244.zero_term;}}_LL12D:
# 993
 Cyc_Tcenv_check_rgn_accessible(_tmp23E,loc,_tmp245);
# 995
Cyc_Tcutil_check_nonzero_bound(loc,_tmp247);{
# 996
void*_tmp249=Cyc_Tcutil_compress(_tmp242);struct Cyc_Absyn_FnInfo _tmp24B;struct Cyc_List_List*_tmp24C;void*_tmp24D;struct Cyc_Absyn_Tqual _tmp24E;void*_tmp24F;struct Cyc_List_List*_tmp250;int _tmp251;struct Cyc_Absyn_VarargInfo*_tmp252;struct Cyc_List_List*_tmp253;struct Cyc_List_List*_tmp254;_LL131: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp24A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp249;if(_tmp24A->tag != 9)goto _LL133;else{_tmp24B=_tmp24A->f1;_tmp24C=_tmp24B.tvars;_tmp24D=_tmp24B.effect;_tmp24E=_tmp24B.ret_tqual;_tmp24F=_tmp24B.ret_typ;_tmp250=_tmp24B.args;_tmp251=_tmp24B.c_varargs;_tmp252=_tmp24B.cyc_varargs;_tmp253=_tmp24B.rgn_po;_tmp254=_tmp24B.attributes;}}_LL132:
# 1002
 if(topt != 0)Cyc_Tcutil_unify(_tmp24F,*topt);
# 1004
while(_tmp23B != 0  && _tmp250 != 0){
# 1006
int alias_coercion=0;
# 1007
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp23B->hd;
# 1008
void*t2=(*((struct _tuple9*)_tmp250->hd)).f3;
# 1009
Cyc_Tcexp_tcExp(_tmp23E,(void**)& t2,e1);
# 1010
if(!Cyc_Tcutil_coerce_arg(_tmp23E,e1,t2,& alias_coercion)){
# 1011
{const char*_tmp80A;void*_tmp809[2];struct Cyc_String_pa_PrintArg_struct _tmp808;struct Cyc_String_pa_PrintArg_struct _tmp807;(_tmp807.tag=0,((_tmp807.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1014
Cyc_Absynpp_typ2string(t2)),((_tmp808.tag=0,((_tmp808.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp809[0]=& _tmp808,((_tmp809[1]=& _tmp807,Cyc_Tcutil_terr(e1->loc,((_tmp80A="actual argument has type \n\t%s\n but formal has type \n\t%s.",_tag_dyneither(_tmp80A,sizeof(char),57))),_tag_dyneither(_tmp809,sizeof(void*),2)))))))))))));}
# 1015
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
# 1016
Cyc_Tcutil_explain_failure();}
# 1019
if(alias_coercion){
# 1020
struct Cyc_List_List*_tmp80B;*alias_arg_exps=(struct Cyc_List_List*)((_tmp80B=_cycalloc(sizeof(*_tmp80B)),((_tmp80B->hd=(void*)_tmp23C,((_tmp80B->tl=*alias_arg_exps,_tmp80B))))));}
# 1021
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,t2) && !Cyc_Tcutil_is_noalias_path(ter,e1)){
# 1022
const char*_tmp80E;void*_tmp80D;(_tmp80D=0,Cyc_Tcutil_terr(e1->loc,((_tmp80E="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp80E,sizeof(char),49))),_tag_dyneither(_tmp80D,sizeof(void*),0)));}
# 1023
_tmp23B=_tmp23B->tl;
# 1024
_tmp250=_tmp250->tl;
# 1025
++ _tmp23C;}{
# 1030
int args_already_checked=0;
# 1031
{struct Cyc_List_List*a=_tmp254;for(0;a != 0;a=a->tl){
# 1032
void*_tmp25C=(void*)a->hd;enum Cyc_Absyn_Format_Type _tmp25E;int _tmp25F;int _tmp260;_LL136: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp25D=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp25C;if(_tmp25D->tag != 19)goto _LL138;else{_tmp25E=_tmp25D->f1;_tmp25F=_tmp25D->f2;_tmp260=_tmp25D->f3;}}_LL137:
# 1034
{struct _handler_cons _tmp261;_push_handler(& _tmp261);{int _tmp263=0;if(setjmp(_tmp261.handler))_tmp263=1;if(!_tmp263){
# 1036
{struct Cyc_Absyn_Exp*_tmp264=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp25F - 1);
# 1038
struct Cyc_Core_Opt*fmt_args;
# 1039
if(_tmp260 == 0)
# 1040
fmt_args=0;else{
# 1042
struct Cyc_Core_Opt*_tmp80F;fmt_args=((_tmp80F=_cycalloc(sizeof(*_tmp80F)),((_tmp80F->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp260 - 1),_tmp80F))));}
# 1043
args_already_checked=1;{
# 1044
struct _RegionHandle _tmp266=_new_region("temp");struct _RegionHandle*temp=& _tmp266;_push_region(temp);
# 1045
switch(_tmp25E){case Cyc_Absyn_Printf_ft: _LL13A:
# 1047
 Cyc_Tcexp_check_format_args(_tmp23E,_tmp264,fmt_args,_tmp260 - 1,alias_arg_exps,temp,Cyc_Formatstr_get_format_typs);
# 1050
break;case Cyc_Absyn_Scanf_ft: _LL13B:
# 1052
 Cyc_Tcexp_check_format_args(_tmp23E,_tmp264,fmt_args,_tmp260 - 1,alias_arg_exps,temp,Cyc_Formatstr_get_scanf_typs);
# 1055
break;}
# 1045
;_pop_region(temp);};}
# 1036
;_pop_handler();}else{void*_tmp262=(void*)_exn_thrown;void*_tmp268=_tmp262;_LL13E: {struct Cyc_List_Nth_exn_struct*_tmp269=(struct Cyc_List_Nth_exn_struct*)_tmp268;if(_tmp269->tag != Cyc_List_Nth)goto _LL140;}_LL13F:
# 1059
{const char*_tmp812;void*_tmp811;(_tmp811=0,Cyc_Tcutil_terr(loc,((_tmp812="bad format arguments",_tag_dyneither(_tmp812,sizeof(char),21))),_tag_dyneither(_tmp811,sizeof(void*),0)));}goto _LL13D;_LL140:;_LL141:(void)_throw(_tmp268);_LL13D:;}};}
# 1061
goto _LL135;_LL138:;_LL139:
# 1062
 goto _LL135;_LL135:;}}
# 1065
if(_tmp250 != 0){const char*_tmp815;void*_tmp814;(_tmp814=0,Cyc_Tcutil_terr(loc,((_tmp815="too few arguments for function",_tag_dyneither(_tmp815,sizeof(char),31))),_tag_dyneither(_tmp814,sizeof(void*),0)));}else{
# 1067
if((_tmp23B != 0  || _tmp251) || _tmp252 != 0){
# 1068
if(_tmp251)
# 1069
for(0;_tmp23B != 0;_tmp23B=_tmp23B->tl){
# 1070
Cyc_Tcexp_tcExp(_tmp23E,0,(struct Cyc_Absyn_Exp*)_tmp23B->hd);}else{
# 1071
if(_tmp252 == 0){
# 1072
const char*_tmp818;void*_tmp817;(_tmp817=0,Cyc_Tcutil_terr(loc,((_tmp818="too many arguments for function",_tag_dyneither(_tmp818,sizeof(char),32))),_tag_dyneither(_tmp817,sizeof(void*),0)));}else{
# 1074
void*_tmp271;int _tmp272;struct Cyc_Absyn_VarargInfo _tmp270=*_tmp252;_tmp271=_tmp270.type;_tmp272=_tmp270.inject;{
# 1075
struct Cyc_Absyn_VarargCallInfo*_tmp819;struct Cyc_Absyn_VarargCallInfo*_tmp273=(_tmp819=_cycalloc(sizeof(*_tmp819)),((_tmp819->num_varargs=0,((_tmp819->injectors=0,((_tmp819->vai=(struct Cyc_Absyn_VarargInfo*)_tmp252,_tmp819)))))));
# 1078
*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)_tmp273;
# 1080
if(!_tmp272)
# 1082
for(0;_tmp23B != 0;(_tmp23B=_tmp23B->tl,_tmp23C ++)){
# 1083
int alias_coercion=0;
# 1084
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp23B->hd;
# 1085
++ _tmp273->num_varargs;
# 1086
Cyc_Tcexp_tcExp(_tmp23E,(void**)& _tmp271,e1);
# 1087
if(!Cyc_Tcutil_coerce_arg(_tmp23E,e1,_tmp271,& alias_coercion)){
# 1088
{const char*_tmp81E;void*_tmp81D[2];struct Cyc_String_pa_PrintArg_struct _tmp81C;struct Cyc_String_pa_PrintArg_struct _tmp81B;(_tmp81B.tag=0,((_tmp81B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1089
Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp81C.tag=0,((_tmp81C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp271)),((_tmp81D[0]=& _tmp81C,((_tmp81D[1]=& _tmp81B,Cyc_Tcutil_terr(loc,((_tmp81E="vararg requires type %s but argument has type %s",_tag_dyneither(_tmp81E,sizeof(char),49))),_tag_dyneither(_tmp81D,sizeof(void*),2)))))))))))));}
# 1090
Cyc_Tcutil_explain_failure();}
# 1092
if(alias_coercion){
# 1093
struct Cyc_List_List*_tmp81F;*alias_arg_exps=(struct Cyc_List_List*)((_tmp81F=_cycalloc(sizeof(*_tmp81F)),((_tmp81F->hd=(void*)_tmp23C,((_tmp81F->tl=*alias_arg_exps,_tmp81F))))));}
# 1094
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,_tmp271) && !
# 1095
Cyc_Tcutil_is_noalias_path(ter,e1)){
# 1096
const char*_tmp822;void*_tmp821;(_tmp821=0,Cyc_Tcutil_terr(e1->loc,((_tmp822="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp822,sizeof(char),49))),_tag_dyneither(_tmp821,sizeof(void*),0)));}}else{
# 1101
void*_tmp27B=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp271));struct Cyc_Absyn_DatatypeInfo _tmp27D;union Cyc_Absyn_DatatypeInfoU _tmp27E;struct Cyc_Absyn_Datatypedecl**_tmp27F;struct Cyc_Absyn_Datatypedecl*_tmp280;struct Cyc_List_List*_tmp281;_LL143: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp27C=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp27B;if(_tmp27C->tag != 3)goto _LL145;else{_tmp27D=_tmp27C->f1;_tmp27E=_tmp27D.datatype_info;if((_tmp27E.KnownDatatype).tag != 2)goto _LL145;_tmp27F=(struct Cyc_Absyn_Datatypedecl**)(_tmp27E.KnownDatatype).val;_tmp280=*_tmp27F;_tmp281=_tmp27D.targs;}}_LL144: {
# 1105
struct Cyc_Absyn_Datatypedecl*_tmp282=*Cyc_Tcenv_lookup_datatypedecl(_tmp23E,loc,_tmp280->name);
# 1106
struct Cyc_List_List*fields=0;
# 1107
if(_tmp282->fields == 0){
# 1108
const char*_tmp826;void*_tmp825[1];struct Cyc_String_pa_PrintArg_struct _tmp824;(_tmp824.tag=0,((_tmp824.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1109
Cyc_Absynpp_typ2string(_tmp271)),((_tmp825[0]=& _tmp824,Cyc_Tcutil_terr(loc,((_tmp826="can't inject into abstract datatype %s",_tag_dyneither(_tmp826,sizeof(char),39))),_tag_dyneither(_tmp825,sizeof(void*),1)))))));}else{
# 1110
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp282->fields))->v;}
# 1117
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp271),Cyc_Tcenv_curr_rgn(_tmp23E))){
# 1118
const char*_tmp82B;void*_tmp82A[2];struct Cyc_String_pa_PrintArg_struct _tmp829;struct Cyc_String_pa_PrintArg_struct _tmp828;(_tmp828.tag=0,((_tmp828.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1119
Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp23E))),((_tmp829.tag=0,((_tmp829.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp271)),((_tmp82A[0]=& _tmp829,((_tmp82A[1]=& _tmp828,Cyc_Tcutil_terr(loc,((_tmp82B="can't unify pointer region for %s to curr_rgn %s",_tag_dyneither(_tmp82B,sizeof(char),49))),_tag_dyneither(_tmp82A,sizeof(void*),2)))))))))))));}{
# 1121
struct _RegionHandle _tmp28A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp28A;_push_region(rgn);{
# 1122
struct Cyc_List_List*_tmp28B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp282->tvs,_tmp281);
# 1123
for(0;_tmp23B != 0;_tmp23B=_tmp23B->tl){
# 1124
++ _tmp273->num_varargs;{
# 1125
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp23B->hd;
# 1127
if(!args_already_checked){
# 1128
Cyc_Tcexp_tcExp(_tmp23E,0,e1);
# 1129
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(void*)_check_null(e1->topt)) && !
# 1130
Cyc_Tcutil_is_noalias_path(rgn,e1)){
# 1131
const char*_tmp82E;void*_tmp82D;(_tmp82D=0,Cyc_Tcutil_terr(e1->loc,((_tmp82E="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp82E,sizeof(char),49))),_tag_dyneither(_tmp82D,sizeof(void*),0)));}}{
# 1133
struct Cyc_Absyn_Datatypefield*_tmp28E=Cyc_Tcexp_tcInjection(_tmp23E,e1,Cyc_Tcutil_pointer_elt_type(_tmp271),rgn,_tmp28B,fields);
# 1135
if(_tmp28E != 0){
# 1136
struct Cyc_List_List*_tmp82F;_tmp273->injectors=
# 1137
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp273->injectors,(
# 1138
(_tmp82F=_cycalloc(sizeof(*_tmp82F)),((_tmp82F->hd=(struct Cyc_Absyn_Datatypefield*)_tmp28E,((_tmp82F->tl=0,_tmp82F)))))));}};};}}
# 1141
_npop_handler(0);goto _LL142;
# 1121
;_pop_region(rgn);};}_LL145:;_LL146:
# 1142
{const char*_tmp832;void*_tmp831;(_tmp831=0,Cyc_Tcutil_terr(loc,((_tmp832="bad inject vararg type",_tag_dyneither(_tmp832,sizeof(char),23))),_tag_dyneither(_tmp831,sizeof(void*),0)));}goto _LL142;_LL142:;}};}}}}
# 1149
Cyc_Tcenv_check_effect_accessible(_tmp23E,loc,(void*)_check_null(_tmp24D));
# 1152
Cyc_Tcenv_check_rgn_partial_order(_tmp23E,loc,_tmp253);{
# 1153
void*_tmp293=_tmp24F;_npop_handler(0);return _tmp293;};};_LL133:;_LL134: {
# 1154
const char*_tmp835;void*_tmp834;void*_tmp296=(_tmp834=0,Cyc_Tcexp_expr_err(_tmp23E,loc,topt,((_tmp835="expected pointer to function",_tag_dyneither(_tmp835,sizeof(char),29))),_tag_dyneither(_tmp834,sizeof(void*),0)));_npop_handler(0);return _tmp296;}_LL130:;};_LL12E:;_LL12F: {
# 1156
const char*_tmp838;void*_tmp837;void*_tmp299=(_tmp837=0,Cyc_Tcexp_expr_err(_tmp23E,loc,topt,((_tmp838="expected pointer to function",_tag_dyneither(_tmp838,sizeof(char),29))),_tag_dyneither(_tmp837,sizeof(void*),0)));_npop_handler(0);return _tmp299;}_LL12B:;};}
# 982
;_pop_region(ter);}
# 1162
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
# 1163
int bogus=0;
# 1164
void*_tmp29A=Cyc_Absyn_exn_typ();
# 1165
Cyc_Tcexp_tcExp(te,(void**)& _tmp29A,e);
# 1166
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp29A,& bogus)){
# 1167
const char*_tmp83D;void*_tmp83C[2];struct Cyc_String_pa_PrintArg_struct _tmp83B;struct Cyc_String_pa_PrintArg_struct _tmp83A;(_tmp83A.tag=0,((_tmp83A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1168
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp83B.tag=0,((_tmp83B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1167
Cyc_Absynpp_typ2string(_tmp29A)),((_tmp83C[0]=& _tmp83B,((_tmp83C[1]=& _tmp83A,Cyc_Tcutil_terr(loc,((_tmp83D="expected %s but found %s",_tag_dyneither(_tmp83D,sizeof(char),25))),_tag_dyneither(_tmp83C,sizeof(void*),2)))))))))))));}
# 1169
if(topt != 0)return*topt;
# 1170
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1174
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1176
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1178
e->topt=(void*)Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);{
# 1179
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 1180
{void*_tmp29F=t1;struct Cyc_Absyn_PtrInfo _tmp2A1;void*_tmp2A2;struct Cyc_Absyn_Tqual _tmp2A3;struct Cyc_Absyn_PtrAtts _tmp2A4;void*_tmp2A5;union Cyc_Absyn_Constraint*_tmp2A6;union Cyc_Absyn_Constraint*_tmp2A7;union Cyc_Absyn_Constraint*_tmp2A8;_LL148: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2A0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29F;if(_tmp2A0->tag != 5)goto _LL14A;else{_tmp2A1=_tmp2A0->f1;_tmp2A2=_tmp2A1.elt_typ;_tmp2A3=_tmp2A1.elt_tq;_tmp2A4=_tmp2A1.ptr_atts;_tmp2A5=_tmp2A4.rgn;_tmp2A6=_tmp2A4.nullable;_tmp2A7=_tmp2A4.bounds;_tmp2A8=_tmp2A4.zero_term;}}_LL149:
# 1182
{void*_tmp2A9=Cyc_Tcutil_compress(_tmp2A2);struct Cyc_Absyn_FnInfo _tmp2AB;struct Cyc_List_List*_tmp2AC;void*_tmp2AD;struct Cyc_Absyn_Tqual _tmp2AE;void*_tmp2AF;struct Cyc_List_List*_tmp2B0;int _tmp2B1;struct Cyc_Absyn_VarargInfo*_tmp2B2;struct Cyc_List_List*_tmp2B3;struct Cyc_List_List*_tmp2B4;_LL14D: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2AA=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2A9;if(_tmp2AA->tag != 9)goto _LL14F;else{_tmp2AB=_tmp2AA->f1;_tmp2AC=_tmp2AB.tvars;_tmp2AD=_tmp2AB.effect;_tmp2AE=_tmp2AB.ret_tqual;_tmp2AF=_tmp2AB.ret_typ;_tmp2B0=_tmp2AB.args;_tmp2B1=_tmp2AB.c_varargs;_tmp2B2=_tmp2AB.cyc_varargs;_tmp2B3=_tmp2AB.rgn_po;_tmp2B4=_tmp2AB.attributes;}}_LL14E: {
# 1184
struct _RegionHandle _tmp2B5=_new_region("temp");struct _RegionHandle*temp=& _tmp2B5;_push_region(temp);
# 1185
{struct Cyc_List_List*instantiation=0;
# 1187
for(0;ts != 0  && _tmp2AC != 0;(ts=ts->tl,_tmp2AC=_tmp2AC->tl)){
# 1188
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2AC->hd,& Cyc_Tcutil_bk);
# 1189
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(void*)ts->hd);
# 1190
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
# 1191
struct _tuple13*_tmp840;struct Cyc_List_List*_tmp83F;instantiation=(
# 1192
(_tmp83F=_region_malloc(temp,sizeof(*_tmp83F)),((_tmp83F->hd=((_tmp840=_region_malloc(temp,sizeof(*_tmp840)),((_tmp840->f1=(struct Cyc_Absyn_Tvar*)_tmp2AC->hd,((_tmp840->f2=(void*)ts->hd,_tmp840)))))),((_tmp83F->tl=instantiation,_tmp83F))))));};}
# 1194
if(ts != 0){
# 1195
const char*_tmp843;void*_tmp842;void*_tmp2BA=
# 1196
(_tmp842=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp843="too many type variables in instantiation",_tag_dyneither(_tmp843,sizeof(char),41))),_tag_dyneither(_tmp842,sizeof(void*),0)));_npop_handler(0);return _tmp2BA;}{
# 1197
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp849;struct Cyc_Absyn_FnInfo _tmp848;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp847;void*new_fn_typ=
# 1198
Cyc_Tcutil_rsubstitute(temp,instantiation,(void*)(
# 1199
(_tmp847=_cycalloc(sizeof(*_tmp847)),((_tmp847[0]=((_tmp849.tag=9,((_tmp849.f1=((_tmp848.tvars=_tmp2AC,((_tmp848.effect=_tmp2AD,((_tmp848.ret_tqual=_tmp2AE,((_tmp848.ret_typ=_tmp2AF,((_tmp848.args=_tmp2B0,((_tmp848.c_varargs=_tmp2B1,((_tmp848.cyc_varargs=_tmp2B2,((_tmp848.rgn_po=_tmp2B3,((_tmp848.attributes=_tmp2B4,_tmp848)))))))))))))))))),_tmp849)))),_tmp847)))));
# 1201
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp853;struct Cyc_Absyn_PtrAtts _tmp852;struct Cyc_Absyn_PtrInfo _tmp851;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp850;void*_tmp2BF=(void*)((_tmp850=_cycalloc(sizeof(*_tmp850)),((_tmp850[0]=((_tmp853.tag=5,((_tmp853.f1=((_tmp851.elt_typ=new_fn_typ,((_tmp851.elt_tq=_tmp2A3,((_tmp851.ptr_atts=((_tmp852.rgn=_tmp2A5,((_tmp852.nullable=_tmp2A6,((_tmp852.bounds=_tmp2A7,((_tmp852.zero_term=_tmp2A8,((_tmp852.ptrloc=0,_tmp852)))))))))),_tmp851)))))),_tmp853)))),_tmp850))));_npop_handler(0);return _tmp2BF;};}
# 1185
;_pop_region(temp);}_LL14F:;_LL150:
# 1203
 goto _LL14C;_LL14C:;}
# 1205
goto _LL147;_LL14A:;_LL14B:
# 1206
 goto _LL147;_LL147:;}{
# 1208
const char*_tmp857;void*_tmp856[1];struct Cyc_String_pa_PrintArg_struct _tmp855;return(_tmp855.tag=0,((_tmp855.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1209
Cyc_Absynpp_typ2string(t1)),((_tmp856[0]=& _tmp855,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp857="expecting polymorphic type but found %s",_tag_dyneither(_tmp857,sizeof(char),40))),_tag_dyneither(_tmp856,sizeof(void*),1)))))));};};}
# 1213
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1215
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
# 1216
Cyc_Tcutil_check_no_qual(loc,t);
# 1218
Cyc_Tcexp_tcExp(te,(void**)& t,e);{
# 1219
void*t2=(void*)_check_null(e->topt);
# 1220
if(Cyc_Tcutil_silent_castable(te,loc,t2,t))
# 1221
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1223
enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,loc,t2,t);
# 1224
if(crc != Cyc_Absyn_Unknown_coercion)
# 1225
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
# 1226
if(Cyc_Tcutil_zero_to_null(te,t,e))
# 1227
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1230
Cyc_Tcutil_unify(t2,t);{
# 1231
const char*_tmp85C;void*_tmp85B[2];struct Cyc_String_pa_PrintArg_struct _tmp85A;struct Cyc_String_pa_PrintArg_struct _tmp859;void*_tmp2C6=(_tmp859.tag=0,((_tmp859.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1232
Cyc_Absynpp_typ2string(t)),((_tmp85A.tag=0,((_tmp85A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp85B[0]=& _tmp85A,((_tmp85B[1]=& _tmp859,Cyc_Tcexp_expr_err(te,loc,(void**)& t,((_tmp85C="cannot cast %s to %s",_tag_dyneither(_tmp85C,sizeof(char),21))),_tag_dyneither(_tmp85B,sizeof(void*),2)))))))))))));
# 1233
Cyc_Tcutil_explain_failure();
# 1234
return _tmp2C6;};}}}
# 1240
{struct _tuple0 _tmp85D;struct _tuple0 _tmp2CC=(_tmp85D.f1=e->r,((_tmp85D.f2=Cyc_Tcutil_compress(t),_tmp85D)));void*_tmp2CD;struct Cyc_Absyn_MallocInfo _tmp2CF;int _tmp2D0;void*_tmp2D1;struct Cyc_Absyn_PtrInfo _tmp2D3;struct Cyc_Absyn_PtrAtts _tmp2D4;union Cyc_Absyn_Constraint*_tmp2D5;union Cyc_Absyn_Constraint*_tmp2D6;union Cyc_Absyn_Constraint*_tmp2D7;_LL152: _tmp2CD=_tmp2CC.f1;{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp2CE=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2CD;if(_tmp2CE->tag != 32)goto _LL154;else{_tmp2CF=_tmp2CE->f1;_tmp2D0=_tmp2CF.fat_result;}};_tmp2D1=_tmp2CC.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2D2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D1;if(_tmp2D2->tag != 5)goto _LL154;else{_tmp2D3=_tmp2D2->f1;_tmp2D4=_tmp2D3.ptr_atts;_tmp2D5=_tmp2D4.nullable;_tmp2D6=_tmp2D4.bounds;_tmp2D7=_tmp2D4.zero_term;}};_LL153:
# 1244
 if((_tmp2D0  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2D7)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2D5)){
# 1245
void*_tmp2D8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp2D6);struct Cyc_Absyn_Exp*_tmp2DA;_LL157: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2D9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2D8;if(_tmp2D9->tag != 1)goto _LL159;else{_tmp2DA=_tmp2D9->f1;}}_LL158:
# 1247
 if((Cyc_Evexp_eval_const_uint_exp(_tmp2DA)).f1 == 1){
# 1248
const char*_tmp860;void*_tmp85F;(_tmp85F=0,Cyc_Tcutil_warn(loc,((_tmp860="cast from ? pointer to * pointer will lose size information",_tag_dyneither(_tmp860,sizeof(char),60))),_tag_dyneither(_tmp85F,sizeof(void*),0)));}
# 1249
goto _LL156;_LL159:;_LL15A:
# 1250
 goto _LL156;_LL156:;}
# 1253
goto _LL151;_LL154:;_LL155:
# 1254
 goto _LL151;_LL151:;}
# 1256
return t;};}
# 1260
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
# 1262
void**_tmp2DD=0;
# 1263
struct Cyc_Absyn_Tqual _tmp2DE=Cyc_Absyn_empty_tqual(0);
# 1264
if(topt != 0){
# 1265
void*_tmp2DF=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp2E1;void*_tmp2E2;void**_tmp2E3;struct Cyc_Absyn_Tqual _tmp2E4;struct Cyc_Absyn_PtrAtts _tmp2E5;union Cyc_Absyn_Constraint*_tmp2E6;_LL15C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2E0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DF;if(_tmp2E0->tag != 5)goto _LL15E;else{_tmp2E1=_tmp2E0->f1;_tmp2E2=_tmp2E1.elt_typ;_tmp2E3=(void**)&(_tmp2E0->f1).elt_typ;_tmp2E4=_tmp2E1.elt_tq;_tmp2E5=_tmp2E1.ptr_atts;_tmp2E6=_tmp2E5.zero_term;}}_LL15D:
# 1267
 _tmp2DD=(void**)_tmp2E3;
# 1268
_tmp2DE=_tmp2E4;
# 1269
goto _LL15B;_LL15E:;_LL15F:
# 1270
 goto _LL15B;_LL15B:;}{
# 1280
struct _RegionHandle _tmp2E7=_new_region("r");struct _RegionHandle*r=& _tmp2E7;_push_region(r);
# 1281
Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te)),_tmp2DD,e);
# 1283
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(e->topt))){
# 1284
const char*_tmp863;void*_tmp862;(_tmp862=0,Cyc_Tcutil_terr(e->loc,((_tmp863="Cannot take the address of an alias-free path",_tag_dyneither(_tmp863,sizeof(char),46))),_tag_dyneither(_tmp862,sizeof(void*),0)));}
# 1290
{void*_tmp2EA=e->r;struct Cyc_Absyn_Exp*_tmp2EC;struct Cyc_Absyn_Exp*_tmp2ED;_LL161: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp2EB=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2EA;if(_tmp2EB->tag != 22)goto _LL163;else{_tmp2EC=_tmp2EB->f1;_tmp2ED=_tmp2EB->f2;}}_LL162:
# 1292
{void*_tmp2EE=Cyc_Tcutil_compress((void*)_check_null(_tmp2EC->topt));_LL166: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2EF=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2EE;if(_tmp2EF->tag != 10)goto _LL168;}_LL167:
# 1293
 goto _LL165;_LL168:;_LL169:
# 1300
 e0->r=(Cyc_Absyn_add_exp(_tmp2EC,_tmp2ED,0))->r;{
# 1301
void*_tmp2F0=Cyc_Tcexp_tcPlus(te,_tmp2EC,_tmp2ED);_npop_handler(0);return _tmp2F0;};_LL165:;}
# 1303
goto _LL160;_LL163:;_LL164:
# 1304
 goto _LL160;_LL160:;}
# 1308
{void*_tmp2F1=e->r;int _tmp2F3;int _tmp2F5;_LL16B: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp2F2=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F1;if(_tmp2F2->tag != 20)goto _LL16D;else{_tmp2F3=_tmp2F2->f3;if(_tmp2F3 != 1)goto _LL16D;}}_LL16C:
# 1309
 goto _LL16E;_LL16D: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp2F4=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F1;if(_tmp2F4->tag != 21)goto _LL16F;else{_tmp2F5=_tmp2F4->f3;if(_tmp2F5 != 1)goto _LL16F;}}_LL16E:
# 1311
{const char*_tmp866;void*_tmp865;(_tmp865=0,Cyc_Tcutil_terr(e->loc,((_tmp866="cannot take the address of a @tagged union member",_tag_dyneither(_tmp866,sizeof(char),50))),_tag_dyneither(_tmp865,sizeof(void*),0)));}
# 1312
goto _LL16A;_LL16F:;_LL170:
# 1313
 goto _LL16A;_LL16A:;}{
# 1317
int _tmp2F9;void*_tmp2FA;struct _tuple14 _tmp2F8=Cyc_Tcutil_addressof_props(te,e);_tmp2F9=_tmp2F8.f1;_tmp2FA=_tmp2F8.f2;
# 1318
if(Cyc_Tcutil_is_noalias_region(_tmp2FA,0)){
# 1319
const char*_tmp869;void*_tmp868;(_tmp868=0,Cyc_Tcutil_terr(e->loc,((_tmp869="using & would manufacture an alias to an alias-free pointer",_tag_dyneither(_tmp869,sizeof(char),60))),_tag_dyneither(_tmp868,sizeof(void*),0)));}{
# 1321
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
# 1322
if(_tmp2F9){
# 1323
tq.print_const=1;
# 1324
tq.real_const=1;}{
# 1327
void*t=Cyc_Absyn_at_typ((void*)_check_null(e->topt),_tmp2FA,tq,Cyc_Absyn_false_conref);
# 1328
void*_tmp2FD=t;_npop_handler(0);return _tmp2FD;};};};
# 1280
;_pop_region(r);};}
# 1332
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t){
# 1333
if(te->allow_valueof)
# 1336
return Cyc_Absyn_uint_typ;
# 1338
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
# 1339
Cyc_Tcutil_check_no_qual(loc,t);
# 1340
if(!Cyc_Evexp_okay_szofarg(t)){
# 1341
const char*_tmp86D;void*_tmp86C[1];struct Cyc_String_pa_PrintArg_struct _tmp86B;(_tmp86B.tag=0,((_tmp86B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1342
Cyc_Absynpp_typ2string(t)),((_tmp86C[0]=& _tmp86B,Cyc_Tcutil_terr(loc,((_tmp86D="sizeof applied to type %s, which has unknown size here",_tag_dyneither(_tmp86D,sizeof(char),55))),_tag_dyneither(_tmp86C,sizeof(void*),1)))))));}
# 1343
if(topt != 0){
# 1344
void*_tmp301=Cyc_Tcutil_compress(*topt);void*_tmp303;_LL172: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp302=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp301;if(_tmp302->tag != 19)goto _LL174;else{_tmp303=(void*)_tmp302->f1;}}_LL173: {
# 1347
struct Cyc_Absyn_Exp*_tmp304=Cyc_Absyn_sizeoftyp_exp(t,0);
# 1348
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp870;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp86F;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp305=(_tmp86F=_cycalloc(sizeof(*_tmp86F)),((_tmp86F[0]=((_tmp870.tag=18,((_tmp870.f1=_tmp304,_tmp870)))),_tmp86F)));
# 1349
if(Cyc_Tcutil_unify(_tmp303,(void*)_tmp305))return _tmp301;
# 1350
goto _LL171;}_LL174:;_LL175:
# 1351
 goto _LL171;_LL171:;}
# 1353
return Cyc_Absyn_uint_typ;}
# 1356
int Cyc_Tcexp_structfield_has_name(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
# 1357
return Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0;}
# 1363
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,void*n){
# 1365
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
# 1366
Cyc_Tcutil_check_no_qual(loc,t);
# 1367
{void*_tmp308=n;struct _dyneither_ptr*_tmp30A;unsigned int _tmp30C;_LL177: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp309=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp308;if(_tmp309->tag != 0)goto _LL179;else{_tmp30A=_tmp309->f1;}}_LL178: {
# 1369
int bad_type=1;
# 1370
{void*_tmp30D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp30F;union Cyc_Absyn_AggrInfoU _tmp310;struct Cyc_Absyn_Aggrdecl**_tmp311;struct Cyc_List_List*_tmp313;_LL17C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp30E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp30D;if(_tmp30E->tag != 11)goto _LL17E;else{_tmp30F=_tmp30E->f1;_tmp310=_tmp30F.aggr_info;if((_tmp310.KnownAggr).tag != 2)goto _LL17E;_tmp311=(struct Cyc_Absyn_Aggrdecl**)(_tmp310.KnownAggr).val;}}_LL17D:
# 1372
 if((*_tmp311)->impl == 0)
# 1373
goto _LL17B;
# 1374
if(!((int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp30A,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp311)->impl))->fields)){
# 1375
const char*_tmp874;void*_tmp873[1];struct Cyc_String_pa_PrintArg_struct _tmp872;(_tmp872.tag=0,((_tmp872.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp30A),((_tmp873[0]=& _tmp872,Cyc_Tcutil_terr(loc,((_tmp874="no field of struct/union has name %s",_tag_dyneither(_tmp874,sizeof(char),37))),_tag_dyneither(_tmp873,sizeof(void*),1)))))));}
# 1376
bad_type=0;
# 1377
goto _LL17B;_LL17E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp312=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp30D;if(_tmp312->tag != 12)goto _LL180;else{_tmp313=_tmp312->f2;}}_LL17F:
# 1379
 if(!((int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp30A,_tmp313)){
# 1380
const char*_tmp878;void*_tmp877[1];struct Cyc_String_pa_PrintArg_struct _tmp876;(_tmp876.tag=0,((_tmp876.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp30A),((_tmp877[0]=& _tmp876,Cyc_Tcutil_terr(loc,((_tmp878="no field of struct/union has name %s",_tag_dyneither(_tmp878,sizeof(char),37))),_tag_dyneither(_tmp877,sizeof(void*),1)))))));}
# 1381
bad_type=0;
# 1382
goto _LL17B;_LL180:;_LL181:
# 1383
 goto _LL17B;_LL17B:;}
# 1385
if(bad_type){
# 1386
const char*_tmp87C;void*_tmp87B[1];struct Cyc_String_pa_PrintArg_struct _tmp87A;(_tmp87A.tag=0,((_tmp87A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp87B[0]=& _tmp87A,Cyc_Tcutil_terr(loc,((_tmp87C="%s is not a known struct/union type",_tag_dyneither(_tmp87C,sizeof(char),36))),_tag_dyneither(_tmp87B,sizeof(void*),1)))))));}
# 1387
goto _LL176;}_LL179: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp30B=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp308;if(_tmp30B->tag != 1)goto _LL176;else{_tmp30C=_tmp30B->f1;}}_LL17A: {
# 1389
int bad_type=1;
# 1390
{void*_tmp31D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp31F;union Cyc_Absyn_AggrInfoU _tmp320;struct Cyc_Absyn_Aggrdecl**_tmp321;struct Cyc_List_List*_tmp323;struct Cyc_List_List*_tmp325;struct Cyc_Absyn_DatatypeFieldInfo _tmp327;union Cyc_Absyn_DatatypeFieldInfoU _tmp328;struct _tuple2 _tmp329;struct Cyc_Absyn_Datatypefield*_tmp32A;_LL183: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp31E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp31D;if(_tmp31E->tag != 11)goto _LL185;else{_tmp31F=_tmp31E->f1;_tmp320=_tmp31F.aggr_info;if((_tmp320.KnownAggr).tag != 2)goto _LL185;_tmp321=(struct Cyc_Absyn_Aggrdecl**)(_tmp320.KnownAggr).val;}}_LL184:
# 1392
 if((*_tmp321)->impl == 0)
# 1393
goto _LL182;
# 1394
_tmp323=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp321)->impl))->fields;goto _LL186;_LL185: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp322=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp31D;if(_tmp322->tag != 12)goto _LL187;else{_tmp323=_tmp322->f2;}}_LL186:
# 1396
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp323)<= _tmp30C){
# 1397
const char*_tmp881;void*_tmp880[2];struct Cyc_Int_pa_PrintArg_struct _tmp87F;struct Cyc_Int_pa_PrintArg_struct _tmp87E;(_tmp87E.tag=1,((_tmp87E.f1=(unsigned long)((int)_tmp30C),((_tmp87F.tag=1,((_tmp87F.f1=(unsigned long)
# 1398
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp323),((_tmp880[0]=& _tmp87F,((_tmp880[1]=& _tmp87E,Cyc_Tcutil_terr(loc,((_tmp881="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp881,sizeof(char),46))),_tag_dyneither(_tmp880,sizeof(void*),2)))))))))))));}
# 1399
bad_type=0;
# 1400
goto _LL182;_LL187: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp324=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp31D;if(_tmp324->tag != 10)goto _LL189;else{_tmp325=_tmp324->f1;}}_LL188:
# 1402
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp325)<= _tmp30C){
# 1403
const char*_tmp886;void*_tmp885[2];struct Cyc_Int_pa_PrintArg_struct _tmp884;struct Cyc_Int_pa_PrintArg_struct _tmp883;(_tmp883.tag=1,((_tmp883.f1=(unsigned long)((int)_tmp30C),((_tmp884.tag=1,((_tmp884.f1=(unsigned long)
# 1404
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp325),((_tmp885[0]=& _tmp884,((_tmp885[1]=& _tmp883,Cyc_Tcutil_terr(loc,((_tmp886="tuple has too few components: %d <= %d",_tag_dyneither(_tmp886,sizeof(char),39))),_tag_dyneither(_tmp885,sizeof(void*),2)))))))))))));}
# 1405
bad_type=0;
# 1406
goto _LL182;_LL189: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp326=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp31D;if(_tmp326->tag != 4)goto _LL18B;else{_tmp327=_tmp326->f1;_tmp328=_tmp327.field_info;if((_tmp328.KnownDatatypefield).tag != 2)goto _LL18B;_tmp329=(struct _tuple2)(_tmp328.KnownDatatypefield).val;_tmp32A=_tmp329.f2;}}_LL18A:
# 1408
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp32A->typs)< _tmp30C){
# 1409
const char*_tmp88B;void*_tmp88A[2];struct Cyc_Int_pa_PrintArg_struct _tmp889;struct Cyc_Int_pa_PrintArg_struct _tmp888;(_tmp888.tag=1,((_tmp888.f1=(unsigned long)((int)_tmp30C),((_tmp889.tag=1,((_tmp889.f1=(unsigned long)
# 1410
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp32A->typs),((_tmp88A[0]=& _tmp889,((_tmp88A[1]=& _tmp888,Cyc_Tcutil_terr(loc,((_tmp88B="datatype field has too few components: %d < %d",_tag_dyneither(_tmp88B,sizeof(char),47))),_tag_dyneither(_tmp88A,sizeof(void*),2)))))))))))));}
# 1411
bad_type=0;
# 1412
goto _LL182;_LL18B:;_LL18C:
# 1413
 goto _LL182;_LL182:;}
# 1415
if(bad_type){
# 1416
const char*_tmp88F;void*_tmp88E[1];struct Cyc_String_pa_PrintArg_struct _tmp88D;(_tmp88D.tag=0,((_tmp88D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp88E[0]=& _tmp88D,Cyc_Tcutil_terr(loc,((_tmp88F="%s is not a known type",_tag_dyneither(_tmp88F,sizeof(char),23))),_tag_dyneither(_tmp88E,sizeof(void*),1)))))));}
# 1417
goto _LL176;}_LL176:;}
# 1419
return Cyc_Absyn_uint_typ;}
# 1423
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
# 1424
struct _RegionHandle _tmp33A=_new_region("r");struct _RegionHandle*r=& _tmp33A;_push_region(r);
# 1425
{struct Cyc_Tcenv_Tenv*_tmp33B=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
# 1426
Cyc_Tcexp_tcExp(_tmp33B,0,e);{
# 1427
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 1428
void*_tmp33C=t;struct Cyc_Absyn_PtrInfo _tmp33F;void*_tmp340;struct Cyc_Absyn_PtrAtts _tmp341;void*_tmp342;union Cyc_Absyn_Constraint*_tmp343;union Cyc_Absyn_Constraint*_tmp344;_LL18E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp33D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp33C;if(_tmp33D->tag != 1)goto _LL190;}_LL18F: {
# 1430
struct Cyc_List_List*_tmp345=Cyc_Tcenv_lookup_type_vars(_tmp33B);
# 1431
struct Cyc_Core_Opt*_tmp890;void*_tmp346=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mko,((_tmp890=_cycalloc(sizeof(*_tmp890)),((_tmp890->v=_tmp345,_tmp890)))));
# 1432
struct Cyc_Core_Opt*_tmp891;void*_tmp347=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((_tmp891=_cycalloc(sizeof(*_tmp891)),((_tmp891->v=_tmp345,_tmp891)))));
# 1433
union Cyc_Absyn_Constraint*_tmp348=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
# 1434
union Cyc_Absyn_Constraint*_tmp349=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
# 1435
struct Cyc_Absyn_PtrAtts _tmp892;struct Cyc_Absyn_PtrAtts _tmp34A=(_tmp892.rgn=_tmp347,((_tmp892.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp892.bounds=_tmp348,((_tmp892.zero_term=_tmp349,((_tmp892.ptrloc=0,_tmp892)))))))));
# 1436
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp898;struct Cyc_Absyn_PtrInfo _tmp897;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp896;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp34B=(_tmp896=_cycalloc(sizeof(*_tmp896)),((_tmp896[0]=((_tmp898.tag=5,((_tmp898.f1=((_tmp897.elt_typ=_tmp346,((_tmp897.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp897.ptr_atts=_tmp34A,_tmp897)))))),_tmp898)))),_tmp896)));
# 1437
Cyc_Tcutil_unify(t,(void*)_tmp34B);
# 1438
_tmp340=_tmp346;_tmp342=_tmp347;_tmp343=_tmp348;_tmp344=_tmp349;goto _LL191;}_LL190: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp33E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp33C;if(_tmp33E->tag != 5)goto _LL192;else{_tmp33F=_tmp33E->f1;_tmp340=_tmp33F.elt_typ;_tmp341=_tmp33F.ptr_atts;_tmp342=_tmp341.rgn;_tmp343=_tmp341.bounds;_tmp344=_tmp341.zero_term;}}_LL191:
# 1440
 Cyc_Tcenv_check_rgn_accessible(_tmp33B,loc,_tmp342);
# 1441
Cyc_Tcutil_check_nonzero_bound(loc,_tmp343);
# 1442
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp340),& Cyc_Tcutil_tmk)){
# 1443
const char*_tmp89B;void*_tmp89A;(_tmp89A=0,Cyc_Tcutil_terr(loc,((_tmp89B="can't dereference abstract pointer type",_tag_dyneither(_tmp89B,sizeof(char),40))),_tag_dyneither(_tmp89A,sizeof(void*),0)));}{
# 1444
void*_tmp354=_tmp340;_npop_handler(0);return _tmp354;};_LL192:;_LL193: {
# 1446
const char*_tmp89F;void*_tmp89E[1];struct Cyc_String_pa_PrintArg_struct _tmp89D;void*_tmp358=(_tmp89D.tag=0,((_tmp89D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp89E[0]=& _tmp89D,Cyc_Tcexp_expr_err(_tmp33B,loc,topt,((_tmp89F="expecting *, @, or ? type but found %s",_tag_dyneither(_tmp89F,sizeof(char),39))),_tag_dyneither(_tmp89E,sizeof(void*),1)))))));_npop_handler(0);return _tmp358;}_LL18D:;};}
# 1425
;_pop_region(r);}
# 1452
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1456
struct _RegionHandle _tmp359=_new_region("r");struct _RegionHandle*r=& _tmp359;_push_region(r);
# 1457
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_clear_notreadctxt(r,te),0,e);
# 1460
*is_read=!Cyc_Tcenv_in_notreadctxt(te);{
# 1461
void*_tmp35A=Cyc_Tcutil_compress((void*)_check_null(e->topt));struct Cyc_Absyn_AggrInfo _tmp35C;union Cyc_Absyn_AggrInfoU _tmp35D;struct Cyc_Absyn_Aggrdecl**_tmp35E;struct Cyc_Absyn_Aggrdecl*_tmp35F;struct Cyc_List_List*_tmp360;enum Cyc_Absyn_AggrKind _tmp362;struct Cyc_List_List*_tmp363;_LL195: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp35B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp35A;if(_tmp35B->tag != 11)goto _LL197;else{_tmp35C=_tmp35B->f1;_tmp35D=_tmp35C.aggr_info;if((_tmp35D.KnownAggr).tag != 2)goto _LL197;_tmp35E=(struct Cyc_Absyn_Aggrdecl**)(_tmp35D.KnownAggr).val;_tmp35F=*_tmp35E;_tmp360=_tmp35C.targs;}}_LL196: {
# 1463
struct Cyc_Absyn_Aggrfield*_tmp368=Cyc_Absyn_lookup_decl_field(_tmp35F,f);
# 1464
if(_tmp368 == 0){
# 1465
const char*_tmp8A4;void*_tmp8A3[2];struct Cyc_String_pa_PrintArg_struct _tmp8A2;struct Cyc_String_pa_PrintArg_struct _tmp8A1;void*_tmp36D=(_tmp8A1.tag=0,((_tmp8A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8A2.tag=0,((_tmp8A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1466
Cyc_Absynpp_qvar2string(_tmp35F->name)),((_tmp8A3[0]=& _tmp8A2,((_tmp8A3[1]=& _tmp8A1,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8A4="type %s has no %s field",_tag_dyneither(_tmp8A4,sizeof(char),24))),_tag_dyneither(_tmp8A3,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp36D;}
# 1468
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp35F->impl))->tagged)*is_tagged=1;{
# 1469
void*t2=_tmp368->type;
# 1470
if(_tmp360 != 0){
# 1471
struct _RegionHandle _tmp36E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp36E;_push_region(rgn);
# 1472
{struct Cyc_List_List*_tmp36F=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp35F->tvs,_tmp360);
# 1473
t2=Cyc_Tcutil_rsubstitute(rgn,_tmp36F,_tmp368->type);}
# 1472
;_pop_region(rgn);}
# 1477
if(((_tmp35F->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp35F->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(t2)){
# 1478
const char*_tmp8A8;void*_tmp8A7[1];struct Cyc_String_pa_PrintArg_struct _tmp8A6;void*_tmp373=(_tmp8A6.tag=0,((_tmp8A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8A7[0]=& _tmp8A6,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8A8="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8A8,sizeof(char),56))),_tag_dyneither(_tmp8A7,sizeof(void*),1)))))));_npop_handler(0);return _tmp373;}
# 1479
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp35F->impl))->exist_vars != 0){
# 1482
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
# 1483
const char*_tmp8AC;void*_tmp8AB[1];struct Cyc_String_pa_PrintArg_struct _tmp8AA;void*_tmp377=(_tmp8AA.tag=0,((_tmp8AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8AB[0]=& _tmp8AA,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8AC="must use pattern-matching to access field %s\n\tdue to existential type variables.",_tag_dyneither(_tmp8AC,sizeof(char),81))),_tag_dyneither(_tmp8AB,sizeof(void*),1)))))));_npop_handler(0);return _tmp377;}}{
# 1485
void*_tmp378=t2;_npop_handler(0);return _tmp378;};};}_LL197: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp361=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp35A;if(_tmp361->tag != 12)goto _LL199;else{_tmp362=_tmp361->f1;_tmp363=_tmp361->f2;}}_LL198: {
# 1487
struct Cyc_Absyn_Aggrfield*_tmp379=Cyc_Absyn_lookup_field(_tmp363,f);
# 1488
if(_tmp379 == 0){
# 1489
const char*_tmp8B0;void*_tmp8AF[1];struct Cyc_String_pa_PrintArg_struct _tmp8AE;void*_tmp37D=(_tmp8AE.tag=0,((_tmp8AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8AF[0]=& _tmp8AE,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8B0="type has no %s field",_tag_dyneither(_tmp8B0,sizeof(char),21))),_tag_dyneither(_tmp8AF,sizeof(void*),1)))))));_npop_handler(0);return _tmp37D;}
# 1490
if((_tmp362 == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(_tmp379->type)){
# 1491
const char*_tmp8B4;void*_tmp8B3[1];struct Cyc_String_pa_PrintArg_struct _tmp8B2;void*_tmp381=(_tmp8B2.tag=0,((_tmp8B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8B3[0]=& _tmp8B2,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8B4="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8B4,sizeof(char),56))),_tag_dyneither(_tmp8B3,sizeof(void*),1)))))));_npop_handler(0);return _tmp381;}{
# 1492
void*_tmp382=_tmp379->type;_npop_handler(0);return _tmp382;};}_LL199:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp364=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp35A;if(_tmp364->tag != 8)goto _LL19B;}{const char*_tmp8B5;if(!(
# 1493
Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp8B5="size",_tag_dyneither(_tmp8B5,sizeof(char),5))))== 0))goto _LL19B;};_LL19A: goto _LL19C;_LL19B:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp366=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35A;if(_tmp366->tag != 5)goto _LL19D;}{const char*_tmp8B6;if(!(
# 1494
Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp8B6="size",_tag_dyneither(_tmp8B6,sizeof(char),5))))== 0))goto _LL19D;};_LL19C:
# 1495
{const char*_tmp8BA;void*_tmp8B9[1];struct Cyc_String_pa_PrintArg_struct _tmp8B8;(_tmp8B8.tag=0,((_tmp8B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp8B9[0]=& _tmp8B8,Cyc_Tcutil_warn(e->loc,((_tmp8BA="deprecated `.size' used here -- change to numelts(%s)",_tag_dyneither(_tmp8BA,sizeof(char),54))),_tag_dyneither(_tmp8B9,sizeof(void*),1)))))));}
# 1496
{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp8C0;struct Cyc_List_List*_tmp8BF;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp8BE;outer_e->r=(void*)((_tmp8BE=_cycalloc(sizeof(*_tmp8BE)),((_tmp8BE[0]=((_tmp8C0.tag=2,((_tmp8C0.f1=Cyc_Absyn_Numelts,((_tmp8C0.f2=((_tmp8BF=_cycalloc(sizeof(*_tmp8BF)),((_tmp8BF->hd=e,((_tmp8BF->tl=0,_tmp8BF)))))),_tmp8C0)))))),_tmp8BE))));}{
# 1497
void*_tmp389=Cyc_Absyn_uint_typ;_npop_handler(0);return _tmp389;};_LL19D:;_LL19E: {
# 1499
const char*_tmp8C1;if(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp8C1="size",_tag_dyneither(_tmp8C1,sizeof(char),5))))== 0){
# 1500
const char*_tmp8C5;void*_tmp8C4[1];struct Cyc_String_pa_PrintArg_struct _tmp8C3;void*_tmp38E=(_tmp8C3.tag=0,((_tmp8C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1502
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp8C4[0]=& _tmp8C3,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8C5="expecting struct, union, or array, found %s",_tag_dyneither(_tmp8C5,sizeof(char),44))),_tag_dyneither(_tmp8C4,sizeof(void*),1)))))));_npop_handler(0);return _tmp38E;}else{
# 1504
const char*_tmp8C9;void*_tmp8C8[1];struct Cyc_String_pa_PrintArg_struct _tmp8C7;void*_tmp392=(_tmp8C7.tag=0,((_tmp8C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1505
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp8C8[0]=& _tmp8C7,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8C9="expecting struct or union, found %s",_tag_dyneither(_tmp8C9,sizeof(char),36))),_tag_dyneither(_tmp8C8,sizeof(void*),1)))))));_npop_handler(0);return _tmp392;}}_LL194:;};
# 1456
;_pop_region(r);}
# 1510
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1513
struct _RegionHandle _tmp393=_new_region("r");struct _RegionHandle*r=& _tmp393;_push_region(r);
# 1514
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te)),0,e);
# 1517
*is_read=!Cyc_Tcenv_in_notreadctxt(te);
# 1518
{void*_tmp394=Cyc_Tcutil_compress((void*)_check_null(e->topt));struct Cyc_Absyn_PtrInfo _tmp396;void*_tmp397;struct Cyc_Absyn_PtrAtts _tmp398;void*_tmp399;union Cyc_Absyn_Constraint*_tmp39A;union Cyc_Absyn_Constraint*_tmp39B;_LL1A0: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp395=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp394;if(_tmp395->tag != 5)goto _LL1A2;else{_tmp396=_tmp395->f1;_tmp397=_tmp396.elt_typ;_tmp398=_tmp396.ptr_atts;_tmp399=_tmp398.rgn;_tmp39A=_tmp398.bounds;_tmp39B=_tmp398.zero_term;}}_LL1A1:
# 1520
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp39A);
# 1521
{void*_tmp39C=Cyc_Tcutil_compress(_tmp397);struct Cyc_Absyn_AggrInfo _tmp39E;union Cyc_Absyn_AggrInfoU _tmp39F;struct Cyc_Absyn_Aggrdecl**_tmp3A0;struct Cyc_Absyn_Aggrdecl*_tmp3A1;struct Cyc_List_List*_tmp3A2;enum Cyc_Absyn_AggrKind _tmp3A4;struct Cyc_List_List*_tmp3A5;_LL1A5: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp39D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp39C;if(_tmp39D->tag != 11)goto _LL1A7;else{_tmp39E=_tmp39D->f1;_tmp39F=_tmp39E.aggr_info;if((_tmp39F.KnownAggr).tag != 2)goto _LL1A7;_tmp3A0=(struct Cyc_Absyn_Aggrdecl**)(_tmp39F.KnownAggr).val;_tmp3A1=*_tmp3A0;_tmp3A2=_tmp39E.targs;}}_LL1A6: {
# 1523
struct Cyc_Absyn_Aggrfield*_tmp3A6=Cyc_Absyn_lookup_decl_field(_tmp3A1,f);
# 1524
if(_tmp3A6 == 0){
# 1525
const char*_tmp8CE;void*_tmp8CD[2];struct Cyc_String_pa_PrintArg_struct _tmp8CC;struct Cyc_String_pa_PrintArg_struct _tmp8CB;void*_tmp3AB=(_tmp8CB.tag=0,((_tmp8CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8CC.tag=0,((_tmp8CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1526
Cyc_Absynpp_qvar2string(_tmp3A1->name)),((_tmp8CD[0]=& _tmp8CC,((_tmp8CD[1]=& _tmp8CB,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8CE="type %s has no %s field",_tag_dyneither(_tmp8CE,sizeof(char),24))),_tag_dyneither(_tmp8CD,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp3AB;}
# 1528
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3A1->impl))->tagged)*is_tagged=1;{
# 1529
void*t3=_tmp3A6->type;
# 1530
if(_tmp3A2 != 0){
# 1531
struct _RegionHandle _tmp3AC=_new_region("rgn");struct _RegionHandle*rgn=& _tmp3AC;_push_region(rgn);
# 1532
{struct Cyc_List_List*_tmp3AD=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp3A1->tvs,_tmp3A2);
# 1533
t3=Cyc_Tcutil_rsubstitute(rgn,_tmp3AD,_tmp3A6->type);}
# 1532
;_pop_region(rgn);}{
# 1536
struct Cyc_Absyn_Kind*_tmp3AE=Cyc_Tcutil_typ_kind(t3);
# 1537
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp3AE)){
# 1538
const char*_tmp8D2;void*_tmp8D1[1];struct Cyc_String_pa_PrintArg_struct _tmp8D0;void*_tmp3B2=(_tmp8D0.tag=0,((_tmp8D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8D1[0]=& _tmp8D0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8D2="cannot get member %s since its type is abstract",_tag_dyneither(_tmp8D2,sizeof(char),48))),_tag_dyneither(_tmp8D1,sizeof(void*),1)))))));_npop_handler(0);return _tmp3B2;}
# 1540
if(((_tmp3A1->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3A1->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(t3)){
# 1541
const char*_tmp8D6;void*_tmp8D5[1];struct Cyc_String_pa_PrintArg_struct _tmp8D4;void*_tmp3B6=(_tmp8D4.tag=0,((_tmp8D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8D5[0]=& _tmp8D4,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8D6="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8D6,sizeof(char),56))),_tag_dyneither(_tmp8D5,sizeof(void*),1)))))));_npop_handler(0);return _tmp3B6;}
# 1542
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3A1->impl))->exist_vars != 0){
# 1543
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
# 1544
const char*_tmp8DA;void*_tmp8D9[1];struct Cyc_String_pa_PrintArg_struct _tmp8D8;void*_tmp3BA=(_tmp8D8.tag=0,((_tmp8D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8D9[0]=& _tmp8D8,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8DA="must use pattern-matching to access field %s\n\tdue to extistential types",_tag_dyneither(_tmp8DA,sizeof(char),72))),_tag_dyneither(_tmp8D9,sizeof(void*),1)))))));_npop_handler(0);return _tmp3BA;}}{
# 1548
void*_tmp3BB=t3;_npop_handler(0);return _tmp3BB;};};};}_LL1A7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp3A3=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp39C;if(_tmp3A3->tag != 12)goto _LL1A9;else{_tmp3A4=_tmp3A3->f1;_tmp3A5=_tmp3A3->f2;}}_LL1A8: {
# 1550
struct Cyc_Absyn_Aggrfield*_tmp3BC=Cyc_Absyn_lookup_field(_tmp3A5,f);
# 1551
if(_tmp3BC == 0){
# 1552
const char*_tmp8DE;void*_tmp8DD[1];struct Cyc_String_pa_PrintArg_struct _tmp8DC;void*_tmp3C0=(_tmp8DC.tag=0,((_tmp8DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8DD[0]=& _tmp8DC,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8DE="type has no %s field",_tag_dyneither(_tmp8DE,sizeof(char),21))),_tag_dyneither(_tmp8DD,sizeof(void*),1)))))));_npop_handler(0);return _tmp3C0;}
# 1553
if((_tmp3A4 == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(_tmp3BC->type)){
# 1554
const char*_tmp8E2;void*_tmp8E1[1];struct Cyc_String_pa_PrintArg_struct _tmp8E0;void*_tmp3C4=(_tmp8E0.tag=0,((_tmp8E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8E1[0]=& _tmp8E0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8E2="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8E2,sizeof(char),56))),_tag_dyneither(_tmp8E1,sizeof(void*),1)))))));_npop_handler(0);return _tmp3C4;}{
# 1555
void*_tmp3C5=_tmp3BC->type;_npop_handler(0);return _tmp3C5;};}_LL1A9:;_LL1AA:
# 1556
 goto _LL1A4;_LL1A4:;}
# 1558
goto _LL19F;_LL1A2:;_LL1A3:
# 1559
 goto _LL19F;_LL19F:;}{
# 1561
const char*_tmp8E6;void*_tmp8E5[1];struct Cyc_String_pa_PrintArg_struct _tmp8E4;void*_tmp3C9=(_tmp8E4.tag=0,((_tmp8E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1562
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp8E5[0]=& _tmp8E4,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8E6="expecting struct or union pointer, found %s",_tag_dyneither(_tmp8E6,sizeof(char),44))),_tag_dyneither(_tmp8E5,sizeof(void*),1)))))));_npop_handler(0);return _tmp3C9;};
# 1513
;_pop_region(r);}
# 1566
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1568
unsigned int _tmp3CB;int _tmp3CC;struct _tuple15 _tmp3CA=Cyc_Evexp_eval_const_uint_exp(index);_tmp3CB=_tmp3CA.f1;_tmp3CC=_tmp3CA.f2;
# 1569
if(!_tmp3CC){
# 1570
const char*_tmp8E9;void*_tmp8E8;return(_tmp8E8=0,Cyc_Tcexp_expr_err(te,loc,0,((_tmp8E9="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp8E9,sizeof(char),47))),_tag_dyneither(_tmp8E8,sizeof(void*),0)));}{
# 1572
struct _handler_cons _tmp3CF;_push_handler(& _tmp3CF);{int _tmp3D1=0;if(setjmp(_tmp3CF.handler))_tmp3D1=1;if(!_tmp3D1){
# 1573
{void*_tmp3D2=(*((struct _tuple19*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp3CB)).f2;_npop_handler(0);return _tmp3D2;};_pop_handler();}else{void*_tmp3D0=(void*)_exn_thrown;void*_tmp3D4=_tmp3D0;_LL1AC: {struct Cyc_List_Nth_exn_struct*_tmp3D5=(struct Cyc_List_Nth_exn_struct*)_tmp3D4;if(_tmp3D5->tag != Cyc_List_Nth)goto _LL1AE;}_LL1AD: {
# 1575
const char*_tmp8EE;void*_tmp8ED[2];struct Cyc_Int_pa_PrintArg_struct _tmp8EC;struct Cyc_Int_pa_PrintArg_struct _tmp8EB;return(_tmp8EB.tag=1,((_tmp8EB.f1=(unsigned long)
# 1576
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts),((_tmp8EC.tag=1,((_tmp8EC.f1=(unsigned long)((int)_tmp3CB),((_tmp8ED[0]=& _tmp8EC,((_tmp8ED[1]=& _tmp8EB,Cyc_Tcexp_expr_err(te,loc,0,((_tmp8EE="index is %d but tuple has only %d fields",_tag_dyneither(_tmp8EE,sizeof(char),41))),_tag_dyneither(_tmp8ED,sizeof(void*),2)))))))))))));}_LL1AE:;_LL1AF:(void)_throw(_tmp3D4);_LL1AB:;}};};}
# 1580
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1582
struct _RegionHandle _tmp3DA=_new_region("r");struct _RegionHandle*r=& _tmp3DA;_push_region(r);
# 1583
{struct Cyc_Tcenv_Tenv*_tmp3DB=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
# 1584
Cyc_Tcexp_tcExp(_tmp3DB,0,e1);
# 1585
Cyc_Tcexp_tcExp(_tmp3DB,0,e2);{
# 1586
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
# 1587
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
# 1588
if(!Cyc_Tcutil_coerce_sint_typ(_tmp3DB,e2)){
# 1589
const char*_tmp8F2;void*_tmp8F1[1];struct Cyc_String_pa_PrintArg_struct _tmp8F0;void*_tmp3DF=(_tmp8F0.tag=0,((_tmp8F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1590
Cyc_Absynpp_typ2string(t2)),((_tmp8F1[0]=& _tmp8F0,Cyc_Tcexp_expr_err(_tmp3DB,e2->loc,topt,((_tmp8F2="expecting int subscript, found %s",_tag_dyneither(_tmp8F2,sizeof(char),34))),_tag_dyneither(_tmp8F1,sizeof(void*),1)))))));_npop_handler(0);return _tmp3DF;}{
# 1593
void*_tmp3E0=t1;struct Cyc_Absyn_PtrInfo _tmp3E2;void*_tmp3E3;struct Cyc_Absyn_Tqual _tmp3E4;struct Cyc_Absyn_PtrAtts _tmp3E5;void*_tmp3E6;union Cyc_Absyn_Constraint*_tmp3E7;union Cyc_Absyn_Constraint*_tmp3E8;struct Cyc_List_List*_tmp3EA;_LL1B1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3E1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E0;if(_tmp3E1->tag != 5)goto _LL1B3;else{_tmp3E2=_tmp3E1->f1;_tmp3E3=_tmp3E2.elt_typ;_tmp3E4=_tmp3E2.elt_tq;_tmp3E5=_tmp3E2.ptr_atts;_tmp3E6=_tmp3E5.rgn;_tmp3E7=_tmp3E5.bounds;_tmp3E8=_tmp3E5.zero_term;}}_LL1B2:
# 1597
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3E8)){
# 1598
int emit_warning=1;
# 1599
{void*_tmp3EB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp3E7);struct Cyc_Absyn_Exp*_tmp3ED;_LL1B8: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp3EC=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3EB;if(_tmp3EC->tag != 1)goto _LL1BA;else{_tmp3ED=_tmp3EC->f1;}}_LL1B9:
# 1601
 if(Cyc_Tcutil_is_const_exp(_tmp3DB,e2)){
# 1602
unsigned int _tmp3F0;int _tmp3F1;struct _tuple15 _tmp3EF=Cyc_Evexp_eval_const_uint_exp(e2);_tmp3F0=_tmp3EF.f1;_tmp3F1=_tmp3EF.f2;
# 1603
if(_tmp3F1){
# 1604
unsigned int _tmp3F3;int _tmp3F4;struct _tuple15 _tmp3F2=Cyc_Evexp_eval_const_uint_exp(_tmp3ED);_tmp3F3=_tmp3F2.f1;_tmp3F4=_tmp3F2.f2;
# 1605
if(_tmp3F4  && _tmp3F3 > _tmp3F0)emit_warning=0;}}
# 1608
goto _LL1B7;_LL1BA: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp3EE=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp3EB;if(_tmp3EE->tag != 0)goto _LL1B7;}_LL1BB:
# 1609
 emit_warning=0;goto _LL1B7;_LL1B7:;}
# 1611
if(emit_warning){
# 1612
const char*_tmp8F5;void*_tmp8F4;(_tmp8F4=0,Cyc_Tcutil_warn(e2->loc,((_tmp8F5="subscript on thin, zero-terminated pointer could be expensive.",_tag_dyneither(_tmp8F5,sizeof(char),63))),_tag_dyneither(_tmp8F4,sizeof(void*),0)));}}else{
# 1615
if(Cyc_Tcutil_is_const_exp(_tmp3DB,e2)){
# 1616
unsigned int _tmp3F8;int _tmp3F9;struct _tuple15 _tmp3F7=Cyc_Evexp_eval_const_uint_exp(e2);_tmp3F8=_tmp3F7.f1;_tmp3F9=_tmp3F7.f2;
# 1617
if(_tmp3F9)
# 1618
Cyc_Tcutil_check_bound(loc,_tmp3F8,_tmp3E7);}else{
# 1621
if(Cyc_Tcutil_is_bound_one(_tmp3E7) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3E8)){
# 1622
const char*_tmp8F8;void*_tmp8F7;(_tmp8F7=0,Cyc_Tcutil_warn(e1->loc,((_tmp8F8="subscript applied to pointer to one object",_tag_dyneither(_tmp8F8,sizeof(char),43))),_tag_dyneither(_tmp8F7,sizeof(void*),0)));}
# 1623
Cyc_Tcutil_check_nonzero_bound(loc,_tmp3E7);}}
# 1626
Cyc_Tcenv_check_rgn_accessible(_tmp3DB,loc,_tmp3E6);
# 1627
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp3E3),& Cyc_Tcutil_tmk)){
# 1628
const char*_tmp8FB;void*_tmp8FA;(_tmp8FA=0,Cyc_Tcutil_terr(e1->loc,((_tmp8FB="can't subscript an abstract pointer",_tag_dyneither(_tmp8FB,sizeof(char),36))),_tag_dyneither(_tmp8FA,sizeof(void*),0)));}{
# 1629
void*_tmp3FE=_tmp3E3;_npop_handler(0);return _tmp3FE;};_LL1B3: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3E9=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3E0;if(_tmp3E9->tag != 10)goto _LL1B5;else{_tmp3EA=_tmp3E9->f1;}}_LL1B4: {
# 1630
void*_tmp3FF=Cyc_Tcexp_ithTupleType(_tmp3DB,loc,_tmp3EA,e2);_npop_handler(0);return _tmp3FF;}_LL1B5:;_LL1B6: {
# 1631
const char*_tmp8FF;void*_tmp8FE[1];struct Cyc_String_pa_PrintArg_struct _tmp8FD;void*_tmp403=(_tmp8FD.tag=0,((_tmp8FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp8FE[0]=& _tmp8FD,Cyc_Tcexp_expr_err(_tmp3DB,loc,topt,((_tmp8FF="subscript applied to %s",_tag_dyneither(_tmp8FF,sizeof(char),24))),_tag_dyneither(_tmp8FE,sizeof(void*),1)))))));_npop_handler(0);return _tmp403;}_LL1B0:;};};}
# 1583
;_pop_region(r);}
# 1637
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){
# 1638
int done=0;
# 1639
struct Cyc_List_List*fields=0;
# 1640
if(topt != 0){
# 1641
void*_tmp404=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp406;_LL1BD: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp405=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp404;if(_tmp405->tag != 10)goto _LL1BF;else{_tmp406=_tmp405->f1;}}_LL1BE:
# 1643
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp406)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es))
# 1646
goto _LL1BC;
# 1648
for(0;es != 0;(es=es->tl,_tmp406=_tmp406->tl)){
# 1649
int bogus=0;
# 1650
void*_tmp407=(*((struct _tuple19*)((struct Cyc_List_List*)_check_null(_tmp406))->hd)).f2;
# 1651
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp407,(struct Cyc_Absyn_Exp*)es->hd);
# 1653
Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple19*)_tmp406->hd)).f2,& bogus);{
# 1654
struct _tuple19*_tmp902;struct Cyc_List_List*_tmp901;fields=((_tmp901=_cycalloc(sizeof(*_tmp901)),((_tmp901->hd=((_tmp902=_cycalloc(sizeof(*_tmp902)),((_tmp902->f1=(*((struct _tuple19*)_tmp406->hd)).f1,((_tmp902->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp902)))))),((_tmp901->tl=fields,_tmp901))))));};}
# 1656
done=1;
# 1657
goto _LL1BC;_LL1BF:;_LL1C0:
# 1658
 goto _LL1BC;_LL1BC:;}
# 1660
if(!done)
# 1661
for(0;es != 0;es=es->tl){
# 1662
Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)es->hd);{
# 1663
struct _tuple19*_tmp905;struct Cyc_List_List*_tmp904;fields=((_tmp904=_cycalloc(sizeof(*_tmp904)),((_tmp904->hd=((_tmp905=_cycalloc(sizeof(*_tmp905)),((_tmp905->f1=Cyc_Absyn_empty_tqual(0),((_tmp905->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp905)))))),((_tmp904->tl=fields,_tmp904))))));};}{
# 1665
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp908;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp907;return(void*)((_tmp907=_cycalloc(sizeof(*_tmp907)),((_tmp907[0]=((_tmp908.tag=10,((_tmp908.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmp908)))),_tmp907))));};}
# 1669
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1672
const char*_tmp90B;void*_tmp90A;return(_tmp90A=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp90B="tcCompoundLit",_tag_dyneither(_tmp90B,sizeof(char),14))),_tag_dyneither(_tmp90A,sizeof(void*),0)));}
# 1682 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1685
void*res_t2;
# 1686
struct _RegionHandle _tmp410=_new_region("r");struct _RegionHandle*r=& _tmp410;_push_region(r);{
# 1687
int _tmp411=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
# 1688
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
# 1689
void*res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
# 1690
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp90E;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp90D;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp412=(_tmp90D=_cycalloc(sizeof(*_tmp90D)),((_tmp90D[0]=((_tmp90E.tag=0,((_tmp90E.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp411),_tmp90E)))),_tmp90D)));
# 1691
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp412,loc);
# 1694
if(zero_term){
# 1695
struct Cyc_Absyn_Exp*_tmp413=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp411 - 1);
# 1696
if(!Cyc_Tcutil_is_zero(_tmp413)){
# 1697
const char*_tmp911;void*_tmp910;(_tmp910=0,Cyc_Tcutil_terr(_tmp413->loc,((_tmp911="zero-terminated array doesn't end with zero.",_tag_dyneither(_tmp911,sizeof(char),45))),_tag_dyneither(_tmp910,sizeof(void*),0)));}}
# 1699
sz_exp->topt=(void*)Cyc_Absyn_uint_typ;
# 1700
res_t2=Cyc_Absyn_array_typ(res,
# 1701
(unsigned int)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0),(struct Cyc_Absyn_Exp*)sz_exp,
# 1702
zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0);
# 1704
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
# 1705
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1707
if(!Cyc_Tcutil_coerce_list(te,res,es)){
# 1709
const char*_tmp915;void*_tmp914[1];struct Cyc_String_pa_PrintArg_struct _tmp913;(_tmp913.tag=0,((_tmp913.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1710
Cyc_Absynpp_typ2string(res)),((_tmp914[0]=& _tmp913,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp915="elements of array do not all have the same type (%s)",_tag_dyneither(_tmp915,sizeof(char),53))),_tag_dyneither(_tmp914,sizeof(void*),1)))))));}}
# 1713
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
# 1714
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
# 1715
if(ds != 0){
# 1718
void*_tmp41B=(void*)ds->hd;struct Cyc_Absyn_Exp*_tmp41E;_LL1C2: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp41C=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp41B;if(_tmp41C->tag != 1)goto _LL1C4;}_LL1C3:
# 1720
{const char*_tmp918;void*_tmp917;(_tmp917=0,Cyc_Tcutil_terr(loc,((_tmp918="only array index designators are supported",_tag_dyneither(_tmp918,sizeof(char),43))),_tag_dyneither(_tmp917,sizeof(void*),0)));}
# 1721
goto _LL1C1;_LL1C4: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp41D=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp41B;if(_tmp41D->tag != 0)goto _LL1C1;else{_tmp41E=_tmp41D->f1;}}_LL1C5:
# 1723
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp41E);{
# 1724
unsigned int _tmp422;int _tmp423;struct _tuple15 _tmp421=Cyc_Evexp_eval_const_uint_exp(_tmp41E);_tmp422=_tmp421.f1;_tmp423=_tmp421.f2;
# 1725
if(!_tmp423){
# 1726
const char*_tmp91B;void*_tmp91A;(_tmp91A=0,Cyc_Tcutil_terr(_tmp41E->loc,((_tmp91B="index designator cannot use sizeof or offsetof",_tag_dyneither(_tmp91B,sizeof(char),47))),_tag_dyneither(_tmp91A,sizeof(void*),0)));}else{
# 1727
if(_tmp422 != offset){
# 1728
const char*_tmp920;void*_tmp91F[2];struct Cyc_Int_pa_PrintArg_struct _tmp91E;struct Cyc_Int_pa_PrintArg_struct _tmp91D;(_tmp91D.tag=1,((_tmp91D.f1=(unsigned long)((int)_tmp422),((_tmp91E.tag=1,((_tmp91E.f1=(unsigned long)offset,((_tmp91F[0]=& _tmp91E,((_tmp91F[1]=& _tmp91D,Cyc_Tcutil_terr(_tmp41E->loc,((_tmp920="expecting index designator %d but found %d",_tag_dyneither(_tmp920,sizeof(char),43))),_tag_dyneither(_tmp91F,sizeof(void*),2)))))))))))));}}
# 1730
goto _LL1C1;};_LL1C1:;}}}{
# 1734
void*_tmp42A=res_t2;_npop_handler(0);return _tmp42A;};
# 1686
;_pop_region(r);}
# 1738
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1741
Cyc_Tcexp_tcExp(te,0,bound);
# 1742
{void*_tmp42B=Cyc_Tcutil_compress((void*)_check_null(bound->topt));_LL1C7: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp42C=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp42B;if(_tmp42C->tag != 19)goto _LL1C9;}_LL1C8:
# 1743
 goto _LL1C6;_LL1C9:;_LL1CA:
# 1745
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){
# 1746
const char*_tmp924;void*_tmp923[1];struct Cyc_String_pa_PrintArg_struct _tmp922;(_tmp922.tag=0,((_tmp922.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1747
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp923[0]=& _tmp922,Cyc_Tcutil_terr(bound->loc,((_tmp924="expecting unsigned int, found %s",_tag_dyneither(_tmp924,sizeof(char),33))),_tag_dyneither(_tmp923,sizeof(void*),1)))))));}_LL1C6:;}
# 1750
if(!(vd->tq).real_const){
# 1751
const char*_tmp927;void*_tmp926;(_tmp926=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp927="comprehension index variable is not declared const!",_tag_dyneither(_tmp927,sizeof(char),52))),_tag_dyneither(_tmp926,sizeof(void*),0)));}{
# 1753
struct _RegionHandle _tmp432=_new_region("r");struct _RegionHandle*r=& _tmp432;_push_region(r);
# 1754
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(r,te);
# 1755
if(te2->le != 0){
# 1756
te2=Cyc_Tcenv_new_block(r,loc,te2);
# 1757
te2=Cyc_Tcenv_add_local_var(r,loc,te2,vd);}{
# 1759
struct Cyc_Tcenv_Tenv*_tmp433=te2;
# 1760
void**_tmp434=0;
# 1761
struct Cyc_Absyn_Tqual*_tmp435=0;
# 1762
union Cyc_Absyn_Constraint**_tmp436=0;
# 1764
if(topt != 0){
# 1765
void*_tmp437=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp439;struct Cyc_Absyn_ArrayInfo _tmp43B;void*_tmp43C;struct Cyc_Absyn_Tqual _tmp43D;struct Cyc_Absyn_Exp*_tmp43E;union Cyc_Absyn_Constraint*_tmp43F;_LL1CC: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp438=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp437;if(_tmp438->tag != 5)goto _LL1CE;else{_tmp439=_tmp438->f1;}}_LL1CD:
# 1767
{void**_tmp928;_tmp434=((_tmp928=_region_malloc(r,sizeof(*_tmp928)),((_tmp928[0]=_tmp439.elt_typ,_tmp928))));}
# 1768
{struct Cyc_Absyn_Tqual*_tmp929;_tmp435=((_tmp929=_region_malloc(r,sizeof(*_tmp929)),((_tmp929[0]=_tmp439.elt_tq,_tmp929))));}
# 1769
{union Cyc_Absyn_Constraint**_tmp92A;_tmp436=((_tmp92A=_region_malloc(r,sizeof(*_tmp92A)),((_tmp92A[0]=(_tmp439.ptr_atts).zero_term,_tmp92A))));}
# 1770
goto _LL1CB;_LL1CE: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp43A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp437;if(_tmp43A->tag != 8)goto _LL1D0;else{_tmp43B=_tmp43A->f1;_tmp43C=_tmp43B.elt_type;_tmp43D=_tmp43B.tq;_tmp43E=_tmp43B.num_elts;_tmp43F=_tmp43B.zero_term;}}_LL1CF:
# 1772
{void**_tmp92B;_tmp434=((_tmp92B=_region_malloc(r,sizeof(*_tmp92B)),((_tmp92B[0]=_tmp43C,_tmp92B))));}
# 1773
{struct Cyc_Absyn_Tqual*_tmp92C;_tmp435=((_tmp92C=_region_malloc(r,sizeof(*_tmp92C)),((_tmp92C[0]=_tmp43D,_tmp92C))));}
# 1774
{union Cyc_Absyn_Constraint**_tmp92D;_tmp436=((_tmp92D=_region_malloc(r,sizeof(*_tmp92D)),((_tmp92D[0]=_tmp43F,_tmp92D))));}
# 1775
goto _LL1CB;_LL1D0:;_LL1D1:
# 1777
 goto _LL1CB;_LL1CB:;}{
# 1780
void*t=Cyc_Tcexp_tcExp(_tmp433,_tmp434,body);
# 1781
if(_tmp433->le == 0){
# 1783
if(!Cyc_Tcutil_is_const_exp(_tmp433,bound)){
# 1784
const char*_tmp930;void*_tmp92F;(_tmp92F=0,Cyc_Tcutil_terr(bound->loc,((_tmp930="bound is not constant",_tag_dyneither(_tmp930,sizeof(char),22))),_tag_dyneither(_tmp92F,sizeof(void*),0)));}
# 1785
if(!Cyc_Tcutil_is_const_exp(_tmp433,body)){
# 1786
const char*_tmp933;void*_tmp932;(_tmp932=0,Cyc_Tcutil_terr(bound->loc,((_tmp933="body is not constant",_tag_dyneither(_tmp933,sizeof(char),21))),_tag_dyneither(_tmp932,sizeof(void*),0)));}}
# 1788
if(_tmp436 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp436)){
# 1791
struct Cyc_Absyn_Exp*_tmp44A=Cyc_Absyn_uint_exp(1,0);_tmp44A->topt=(void*)Cyc_Absyn_uint_typ;
# 1792
bound=Cyc_Absyn_add_exp(bound,_tmp44A,0);bound->topt=(void*)Cyc_Absyn_uint_typ;
# 1793
*is_zero_term=1;}{
# 1796
void*_tmp44B=Cyc_Absyn_array_typ(t,_tmp435 == 0?Cyc_Absyn_empty_tqual(0):*_tmp435,(struct Cyc_Absyn_Exp*)bound,
# 1797
_tmp436 == 0?Cyc_Absyn_false_conref:*_tmp436,0);
# 1798
void*_tmp44C=_tmp44B;_npop_handler(0);return _tmp44C;};};};}
# 1754
;_pop_region(r);};}struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1812 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1816
struct Cyc_Absyn_Aggrdecl**adptr;
# 1817
struct Cyc_Absyn_Aggrdecl*ad;
# 1818
if(*ad_opt != 0){
# 1819
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);{
# 1820
struct Cyc_Absyn_Aggrdecl**_tmp934;adptr=((_tmp934=_cycalloc(sizeof(*_tmp934)),((_tmp934[0]=ad,_tmp934))));};}else{
# 1822
{struct _handler_cons _tmp44E;_push_handler(& _tmp44E);{int _tmp450=0;if(setjmp(_tmp44E.handler))_tmp450=1;if(!_tmp450){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
# 1823
ad=*adptr;
# 1822
;_pop_handler();}else{void*_tmp44F=(void*)_exn_thrown;void*_tmp452=_tmp44F;_LL1D3: {struct Cyc_Dict_Absent_exn_struct*_tmp453=(struct Cyc_Dict_Absent_exn_struct*)_tmp452;if(_tmp453->tag != Cyc_Dict_Absent)goto _LL1D5;}_LL1D4:
# 1825
{const char*_tmp938;void*_tmp937[1];struct Cyc_String_pa_PrintArg_struct _tmp936;(_tmp936.tag=0,((_tmp936.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp937[0]=& _tmp936,Cyc_Tcutil_terr(loc,((_tmp938="unbound struct/union name %s",_tag_dyneither(_tmp938,sizeof(char),29))),_tag_dyneither(_tmp937,sizeof(void*),1)))))));}
# 1826
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;_LL1D5:;_LL1D6:(void)_throw(_tmp452);_LL1D2:;}};}
# 1828
*ad_opt=(struct Cyc_Absyn_Aggrdecl*)ad;
# 1829
*tn=ad->name;}
# 1831
if(ad->impl == 0){
# 1832
{const char*_tmp940;void*_tmp93F[1];const char*_tmp93E;const char*_tmp93D;struct Cyc_String_pa_PrintArg_struct _tmp93C;(_tmp93C.tag=0,((_tmp93C.f1=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp93D="struct",_tag_dyneither(_tmp93D,sizeof(char),7)):((_tmp93E="union",_tag_dyneither(_tmp93E,sizeof(char),6)))),((_tmp93F[0]=& _tmp93C,Cyc_Tcutil_terr(loc,((_tmp940="can't construct abstract %s",_tag_dyneither(_tmp940,sizeof(char),28))),_tag_dyneither(_tmp93F,sizeof(void*),1)))))));}
# 1833
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 1835
struct _RegionHandle _tmp45C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp45C;_push_region(rgn);
# 1837
{struct _tuple12 _tmp941;struct _tuple12 _tmp45D=(_tmp941.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp941.f2=rgn,_tmp941)));
# 1838
struct Cyc_List_List*_tmp45E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp45D,ad->tvs);
# 1839
struct Cyc_List_List*_tmp45F=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp45D,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
# 1840
struct Cyc_List_List*_tmp460=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp45E);
# 1841
struct Cyc_List_List*_tmp461=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp45F);
# 1842
struct Cyc_List_List*_tmp462=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp45E,_tmp45F);
# 1843
void*res_typ;
# 1848
if(topt != 0){
# 1849
void*_tmp463=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_AggrInfo _tmp465;union Cyc_Absyn_AggrInfoU _tmp466;struct Cyc_Absyn_Aggrdecl**_tmp467;struct Cyc_List_List*_tmp468;_LL1D8: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp464=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp463;if(_tmp464->tag != 11)goto _LL1DA;else{_tmp465=_tmp464->f1;_tmp466=_tmp465.aggr_info;if((_tmp466.KnownAggr).tag != 2)goto _LL1DA;_tmp467=(struct Cyc_Absyn_Aggrdecl**)(_tmp466.KnownAggr).val;_tmp468=_tmp465.targs;}}_LL1D9:
# 1851
 if(*_tmp467 == *adptr){
# 1852
{struct Cyc_List_List*_tmp469=_tmp460;for(0;_tmp469 != 0  && _tmp468 != 0;
# 1853
(_tmp469=_tmp469->tl,_tmp468=_tmp468->tl)){
# 1854
Cyc_Tcutil_unify((void*)_tmp469->hd,(void*)_tmp468->hd);}}
# 1856
res_typ=*topt;
# 1857
goto _LL1D7;}
# 1859
goto _LL1DB;_LL1DA:;_LL1DB: {
# 1861
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp947;struct Cyc_Absyn_AggrInfo _tmp946;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp945;res_typ=(void*)((_tmp945=_cycalloc(sizeof(*_tmp945)),((_tmp945[0]=((_tmp947.tag=11,((_tmp947.f1=((_tmp946.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp946.targs=_tmp460,_tmp946)))),_tmp947)))),_tmp945))));}_LL1D7:;}else{
# 1864
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp94D;struct Cyc_Absyn_AggrInfo _tmp94C;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp94B;res_typ=(void*)((_tmp94B=_cycalloc(sizeof(*_tmp94B)),((_tmp94B[0]=((_tmp94D.tag=11,((_tmp94D.f1=((_tmp94C.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp94C.targs=_tmp460,_tmp94C)))),_tmp94D)))),_tmp94B))));}{
# 1867
struct Cyc_List_List*_tmp470=*ts;
# 1868
struct Cyc_List_List*_tmp471=_tmp461;
# 1869
while(_tmp470 != 0  && _tmp471 != 0){
# 1871
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_ak,1,(void*)_tmp470->hd);
# 1872
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp470->hd);
# 1873
Cyc_Tcutil_unify((void*)_tmp470->hd,(void*)_tmp471->hd);
# 1874
_tmp470=_tmp470->tl;
# 1875
_tmp471=_tmp471->tl;}
# 1877
if(_tmp470 != 0){
# 1878
const char*_tmp950;void*_tmp94F;(_tmp94F=0,Cyc_Tcutil_terr(loc,((_tmp950="too many explicit witness types",_tag_dyneither(_tmp950,sizeof(char),32))),_tag_dyneither(_tmp94F,sizeof(void*),0)));}
# 1880
*ts=_tmp461;{
# 1883
struct Cyc_List_List*fields=
# 1884
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
# 1885
for(0;fields != 0;fields=fields->tl){
# 1886
int bogus=0;
# 1887
struct _tuple20 _tmp475;struct Cyc_Absyn_Aggrfield*_tmp476;struct Cyc_Absyn_Exp*_tmp477;struct _tuple20*_tmp474=(struct _tuple20*)fields->hd;_tmp475=*_tmp474;_tmp476=_tmp475.f1;_tmp477=_tmp475.f2;{
# 1888
void*_tmp478=Cyc_Tcutil_rsubstitute(rgn,_tmp462,_tmp476->type);
# 1889
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp478,_tmp477);
# 1890
if(!Cyc_Tcutil_coerce_arg(te,_tmp477,_tmp478,& bogus)){
# 1891
{const char*_tmp95C;void*_tmp95B[5];struct Cyc_String_pa_PrintArg_struct _tmp95A;const char*_tmp959;const char*_tmp958;struct Cyc_String_pa_PrintArg_struct _tmp957;struct Cyc_String_pa_PrintArg_struct _tmp956;struct Cyc_String_pa_PrintArg_struct _tmp955;struct Cyc_String_pa_PrintArg_struct _tmp954;(_tmp954.tag=0,((_tmp954.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1894
Cyc_Absynpp_typ2string((void*)_check_null(_tmp477->topt))),((_tmp955.tag=0,((_tmp955.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1893
Cyc_Absynpp_typ2string(_tmp478)),((_tmp956.tag=0,((_tmp956.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp957.tag=0,((_tmp957.f1=(struct _dyneither_ptr)(
# 1892
ad->kind == Cyc_Absyn_StructA?(_tmp958="struct",_tag_dyneither(_tmp958,sizeof(char),7)):((_tmp959="union",_tag_dyneither(_tmp959,sizeof(char),6)))),((_tmp95A.tag=0,((_tmp95A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp476->name),((_tmp95B[0]=& _tmp95A,((_tmp95B[1]=& _tmp957,((_tmp95B[2]=& _tmp956,((_tmp95B[3]=& _tmp955,((_tmp95B[4]=& _tmp954,Cyc_Tcutil_terr(_tmp477->loc,((_tmp95C="field %s of %s %s expects type %s != %s",_tag_dyneither(_tmp95C,sizeof(char),40))),_tag_dyneither(_tmp95B,sizeof(void*),5)))))))))))))))))))))))))))))));}
# 1895
Cyc_Tcutil_explain_failure();}};}{
# 1898
struct Cyc_List_List*_tmp482=0;
# 1899
{struct Cyc_List_List*_tmp483=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp483 != 0;_tmp483=_tmp483->tl){
# 1900
struct _tuple0*_tmp95F;struct Cyc_List_List*_tmp95E;_tmp482=((_tmp95E=_cycalloc(sizeof(*_tmp95E)),((_tmp95E->hd=((_tmp95F=_cycalloc(sizeof(*_tmp95F)),((_tmp95F->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp462,(*((struct _tuple0*)_tmp483->hd)).f1),((_tmp95F->f2=
# 1901
Cyc_Tcutil_rsubstitute(rgn,_tmp462,(*((struct _tuple0*)_tmp483->hd)).f2),_tmp95F)))))),((_tmp95E->tl=_tmp482,_tmp95E))))));}}
# 1903
_tmp482=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp482);
# 1904
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp482);{
# 1905
void*_tmp486=res_typ;_npop_handler(0);return _tmp486;};};};};}
# 1837
;_pop_region(rgn);};}
# 1909
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 1911
struct _RegionHandle _tmp488=_new_region("rgn");struct _RegionHandle*rgn=& _tmp488;_push_region(rgn);{
# 1912
void*_tmp489=Cyc_Tcutil_compress(ts);enum Cyc_Absyn_AggrKind _tmp48B;struct Cyc_List_List*_tmp48C;_LL1DD: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp48A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp489;if(_tmp48A->tag != 12)goto _LL1DF;else{_tmp48B=_tmp48A->f1;_tmp48C=_tmp48A->f2;}}_LL1DE:
# 1914
 if(_tmp48B == Cyc_Absyn_UnionA){
# 1915
const char*_tmp962;void*_tmp961;(_tmp961=0,Cyc_Tcutil_terr(loc,((_tmp962="expecting struct but found union",_tag_dyneither(_tmp962,sizeof(char),33))),_tag_dyneither(_tmp961,sizeof(void*),0)));}{
# 1916
struct Cyc_List_List*fields=
# 1917
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,Cyc_Absyn_StructA,_tmp48C);
# 1918
for(0;fields != 0;fields=fields->tl){
# 1919
int bogus=0;
# 1920
struct _tuple20 _tmp490;struct Cyc_Absyn_Aggrfield*_tmp491;struct Cyc_Absyn_Exp*_tmp492;struct _tuple20*_tmp48F=(struct _tuple20*)fields->hd;_tmp490=*_tmp48F;_tmp491=_tmp490.f1;_tmp492=_tmp490.f2;
# 1921
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp491->type,_tmp492);
# 1922
if(!Cyc_Tcutil_coerce_arg(te,_tmp492,_tmp491->type,& bogus)){
# 1923
{const char*_tmp968;void*_tmp967[3];struct Cyc_String_pa_PrintArg_struct _tmp966;struct Cyc_String_pa_PrintArg_struct _tmp965;struct Cyc_String_pa_PrintArg_struct _tmp964;(_tmp964.tag=0,((_tmp964.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1925
Cyc_Absynpp_typ2string((void*)_check_null(_tmp492->topt))),((_tmp965.tag=0,((_tmp965.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1924
Cyc_Absynpp_typ2string(_tmp491->type)),((_tmp966.tag=0,((_tmp966.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp491->name),((_tmp967[0]=& _tmp966,((_tmp967[1]=& _tmp965,((_tmp967[2]=& _tmp964,Cyc_Tcutil_terr(_tmp492->loc,((_tmp968="field %s of struct expects type %s != %s",_tag_dyneither(_tmp968,sizeof(char),41))),_tag_dyneither(_tmp967,sizeof(void*),3)))))))))))))))))));}
# 1926
Cyc_Tcutil_explain_failure();}}
# 1929
goto _LL1DC;};_LL1DF:;_LL1E0: {
# 1930
const char*_tmp96B;void*_tmp96A;(_tmp96A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp96B="tcAnonStruct: wrong type",_tag_dyneither(_tmp96B,sizeof(char),25))),_tag_dyneither(_tmp96A,sizeof(void*),0)));}_LL1DC:;}{
# 1933
void*_tmp49A=ts;_npop_handler(0);return _tmp49A;};
# 1911
;_pop_region(rgn);}
# 1937
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 1940
struct _RegionHandle _tmp49B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp49B;_push_region(rgn);
# 1941
{struct _tuple12 _tmp96C;struct _tuple12 _tmp49C=(_tmp96C.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp96C.f2=rgn,_tmp96C)));
# 1942
struct Cyc_List_List*_tmp49D=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp49C,tud->tvs);
# 1943
struct Cyc_List_List*_tmp49E=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp49D);
# 1944
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp972;struct Cyc_Absyn_DatatypeFieldInfo _tmp971;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp970;void*res=(void*)(
# 1945
(_tmp970=_cycalloc(sizeof(*_tmp970)),((_tmp970[0]=((_tmp972.tag=4,((_tmp972.f1=((_tmp971.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf),((_tmp971.targs=_tmp49E,_tmp971)))),_tmp972)))),_tmp970))));
# 1948
if(topt != 0){
# 1949
void*_tmp49F=Cyc_Tcutil_compress(*topt);_LL1E2: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4A0=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp49F;if(_tmp4A0->tag != 4)goto _LL1E4;}_LL1E3:
# 1950
 Cyc_Tcutil_unify(*topt,res);goto _LL1E1;_LL1E4:;_LL1E5:
# 1951
 goto _LL1E1;_LL1E1:;}{
# 1954
struct Cyc_List_List*ts=tuf->typs;
# 1955
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
# 1956
int bogus=0;
# 1957
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
# 1958
void*t=(*((struct _tuple19*)ts->hd)).f2;
# 1959
if(_tmp49D != 0)t=Cyc_Tcutil_rsubstitute(rgn,_tmp49D,t);
# 1960
Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);
# 1961
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
# 1962
{const char*_tmp97A;void*_tmp979[3];struct Cyc_String_pa_PrintArg_struct _tmp978;struct Cyc_String_pa_PrintArg_struct _tmp977;const char*_tmp976;struct Cyc_String_pa_PrintArg_struct _tmp975;(_tmp975.tag=0,((_tmp975.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
# 1965
e->topt == 0?(struct _dyneither_ptr)((_tmp976="?",_tag_dyneither(_tmp976,sizeof(char),2))): Cyc_Absynpp_typ2string((void*)_check_null(e->topt)))),((_tmp977.tag=0,((_tmp977.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1964
Cyc_Absynpp_typ2string(t)),((_tmp978.tag=0,((_tmp978.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((_tmp979[0]=& _tmp978,((_tmp979[1]=& _tmp977,((_tmp979[2]=& _tmp975,Cyc_Tcutil_terr(e->loc,((_tmp97A="datatype constructor %s expects argument of type %s but this argument has type %s",_tag_dyneither(_tmp97A,sizeof(char),82))),_tag_dyneither(_tmp979,sizeof(void*),3)))))))))))))))))));}
# 1966
Cyc_Tcutil_explain_failure();}}
# 1969
if(es != 0){
# 1970
const char*_tmp97E;void*_tmp97D[1];struct Cyc_String_pa_PrintArg_struct _tmp97C;void*_tmp4AA=(_tmp97C.tag=0,((_tmp97C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1972
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp97D[0]=& _tmp97C,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,topt,((_tmp97E="too many arguments for datatype constructor %s",_tag_dyneither(_tmp97E,sizeof(char),47))),_tag_dyneither(_tmp97D,sizeof(void*),1)))))));_npop_handler(0);return _tmp4AA;}
# 1973
if(ts != 0){
# 1974
const char*_tmp982;void*_tmp981[1];struct Cyc_String_pa_PrintArg_struct _tmp980;void*_tmp4AE=(_tmp980.tag=0,((_tmp980.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1975
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp981[0]=& _tmp980,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp982="too few arguments for datatype constructor %s",_tag_dyneither(_tmp982,sizeof(char),46))),_tag_dyneither(_tmp981,sizeof(void*),1)))))));_npop_handler(0);return _tmp4AE;}{
# 1976
void*_tmp4AF=res;_npop_handler(0);return _tmp4AF;};};}
# 1941
;_pop_region(rgn);}
# 1980
static int Cyc_Tcexp_zeroable_type(void*t){
# 1981
void*_tmp4B4=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp4BB;struct Cyc_Absyn_PtrAtts _tmp4BC;union Cyc_Absyn_Constraint*_tmp4BD;struct Cyc_Absyn_ArrayInfo _tmp4C1;void*_tmp4C2;struct Cyc_List_List*_tmp4C5;struct Cyc_Absyn_AggrInfo _tmp4C7;union Cyc_Absyn_AggrInfoU _tmp4C8;struct Cyc_List_List*_tmp4C9;struct Cyc_List_List*_tmp4CD;_LL1E7: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp4B5=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp4B4;if(_tmp4B5->tag != 0)goto _LL1E9;}_LL1E8:
# 1982
 return 1;_LL1E9: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4B6=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4B4;if(_tmp4B6->tag != 1)goto _LL1EB;}_LL1EA:
# 1983
 goto _LL1EC;_LL1EB: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4B7=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4B4;if(_tmp4B7->tag != 2)goto _LL1ED;}_LL1EC:
# 1984
 goto _LL1EE;_LL1ED: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4B8=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4B4;if(_tmp4B8->tag != 3)goto _LL1EF;}_LL1EE:
# 1985
 goto _LL1F0;_LL1EF: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4B9=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4B4;if(_tmp4B9->tag != 4)goto _LL1F1;}_LL1F0:
# 1986
 return 0;_LL1F1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4BA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4B4;if(_tmp4BA->tag != 5)goto _LL1F3;else{_tmp4BB=_tmp4BA->f1;_tmp4BC=_tmp4BB.ptr_atts;_tmp4BD=_tmp4BC.nullable;}}_LL1F2:
# 1988
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp4BD);_LL1F3: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4BE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4B4;if(_tmp4BE->tag != 6)goto _LL1F5;}_LL1F4:
# 1989
 goto _LL1F6;_LL1F5: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4BF=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4B4;if(_tmp4BF->tag != 7)goto _LL1F7;}_LL1F6:
# 1990
 return 1;_LL1F7: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4C0=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4B4;if(_tmp4C0->tag != 8)goto _LL1F9;else{_tmp4C1=_tmp4C0->f1;_tmp4C2=_tmp4C1.elt_type;}}_LL1F8:
# 1991
 return Cyc_Tcexp_zeroable_type(_tmp4C2);_LL1F9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4C3=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4B4;if(_tmp4C3->tag != 9)goto _LL1FB;}_LL1FA:
# 1992
 return 0;_LL1FB: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4C4=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4B4;if(_tmp4C4->tag != 10)goto _LL1FD;else{_tmp4C5=_tmp4C4->f1;}}_LL1FC:
# 1994
 for(0;(unsigned int)_tmp4C5;_tmp4C5=_tmp4C5->tl){
# 1995
if(!Cyc_Tcexp_zeroable_type((*((struct _tuple19*)_tmp4C5->hd)).f2))return 0;}
# 1996
return 1;_LL1FD: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4C6=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4B4;if(_tmp4C6->tag != 11)goto _LL1FF;else{_tmp4C7=_tmp4C6->f1;_tmp4C8=_tmp4C7.aggr_info;_tmp4C9=_tmp4C7.targs;}}_LL1FE: {
# 1998
struct Cyc_Absyn_Aggrdecl*_tmp4DA=Cyc_Absyn_get_known_aggrdecl(_tmp4C8);
# 1999
if(_tmp4DA->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4DA->impl))->exist_vars != 0)
# 2000
return 0;{
# 2001
struct _RegionHandle _tmp4DB=_new_region("r");struct _RegionHandle*r=& _tmp4DB;_push_region(r);
# 2002
{struct Cyc_List_List*_tmp4DC=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp4DA->tvs,_tmp4C9);
# 2003
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4DA->impl))->fields;for(0;fs != 0;fs=fs->tl){
# 2004
if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp4DC,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp4DD=0;_npop_handler(0);return _tmp4DD;}}}{
# 2005
int _tmp4DE=1;_npop_handler(0);return _tmp4DE;};}
# 2002
;_pop_region(r);};}_LL1FF: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp4CA=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4B4;if(_tmp4CA->tag != 13)goto _LL201;}_LL200:
# 2007
 return 1;_LL201: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4CB=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4B4;if(_tmp4CB->tag != 19)goto _LL203;}_LL202:
# 2008
 return 1;_LL203: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4CC=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4B4;if(_tmp4CC->tag != 12)goto _LL205;else{_tmp4CD=_tmp4CC->f2;}}_LL204:
# 2010
 for(0;_tmp4CD != 0;_tmp4CD=_tmp4CD->tl){
# 2011
if(!Cyc_Tcexp_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmp4CD->hd)->type))return 0;}
# 2012
return 1;_LL205: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp4CE=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4B4;if(_tmp4CE->tag != 14)goto _LL207;}_LL206:
# 2013
 return 1;_LL207: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp4CF=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4B4;if(_tmp4CF->tag != 17)goto _LL209;}_LL208:
# 2014
 return 0;_LL209: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp4D0=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4B4;if(_tmp4D0->tag != 16)goto _LL20B;}_LL20A:
# 2015
 return 0;_LL20B: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4D1=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4B4;if(_tmp4D1->tag != 15)goto _LL20D;}_LL20C:
# 2016
 return 0;_LL20D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp4D2=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4B4;if(_tmp4D2->tag != 26)goto _LL20F;}_LL20E:
# 2018
 goto _LL210;_LL20F: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4D3=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4B4;if(_tmp4D3->tag != 18)goto _LL211;}_LL210:
# 2019
 goto _LL212;_LL211: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp4D4=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp4B4;if(_tmp4D4->tag != 20)goto _LL213;}_LL212:
# 2020
 goto _LL214;_LL213: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp4D5=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp4B4;if(_tmp4D5->tag != 21)goto _LL215;}_LL214:
# 2021
 goto _LL216;_LL215: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp4D6=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp4B4;if(_tmp4D6->tag != 22)goto _LL217;}_LL216:
# 2022
 goto _LL218;_LL217: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4D7=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4B4;if(_tmp4D7->tag != 23)goto _LL219;}_LL218:
# 2023
 goto _LL21A;_LL219: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4D8=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4B4;if(_tmp4D8->tag != 24)goto _LL21B;}_LL21A:
# 2024
 goto _LL21C;_LL21B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp4D9=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4B4;if(_tmp4D9->tag != 25)goto _LL1E6;}_LL21C: {
# 2025
const char*_tmp986;void*_tmp985[1];struct Cyc_String_pa_PrintArg_struct _tmp984;(_tmp984.tag=0,((_tmp984.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp985[0]=& _tmp984,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp986="bad type `%s' in zeroable type",_tag_dyneither(_tmp986,sizeof(char),31))),_tag_dyneither(_tmp985,sizeof(void*),1)))))));}_LL1E6:;}
# 2029
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2031
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
# 2033
if(topt != 0){
# 2034
void*_tmp4E2=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp4E4;void*_tmp4E5;_LL21E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4E3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E2;if(_tmp4E3->tag != 5)goto _LL220;else{_tmp4E4=_tmp4E3->f1;_tmp4E5=_tmp4E4.elt_typ;}}_LL21F:
# 2036
 Cyc_Tcutil_unify(_tmp4E5,t);
# 2037
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
# 2038
goto _LL21D;_LL220:;_LL221:
# 2039
 goto _LL21D;_LL21D:;}{
# 2042
const char*_tmp98F;void*_tmp98E[2];const char*_tmp98D;const char*_tmp98C;struct Cyc_String_pa_PrintArg_struct _tmp98B;struct Cyc_String_pa_PrintArg_struct _tmp98A;(_tmp98A.tag=0,((_tmp98A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2043
Cyc_Absynpp_typ2string(t)),((_tmp98B.tag=0,((_tmp98B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)((_tmp98C="calloc",_tag_dyneither(_tmp98C,sizeof(char),7))):(struct _dyneither_ptr)((_tmp98D="malloc",_tag_dyneither(_tmp98D,sizeof(char),7))))),((_tmp98E[0]=& _tmp98B,((_tmp98E[1]=& _tmp98A,Cyc_Tcutil_terr(loc,((_tmp98F="%s cannot be used with type %s\n\t(type needs initialization)",_tag_dyneither(_tmp98F,sizeof(char),60))),_tag_dyneither(_tmp98E,sizeof(void*),2)))))))))))));};}
# 2046
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2048
switch((Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual){case Cyc_Absyn_Unique: _LL222:
# 2049
 return(void*)& Cyc_Absyn_UniqueRgn_val;default: _LL223:
# 2050
 return(void*)& Cyc_Absyn_HeapRgn_val;}}
# 2054
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2059
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
# 2060
if(*ropt != 0){
# 2062
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp992;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp991;void*expected_type=(void*)(
# 2063
(_tmp991=_cycalloc(sizeof(*_tmp991)),((_tmp991[0]=((_tmp992.tag=15,((_tmp992.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp992)))),_tmp991))));
# 2064
void*handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
# 2065
void*_tmp4EC=Cyc_Tcutil_compress(handle_type);void*_tmp4EE;_LL226: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4ED=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4EC;if(_tmp4ED->tag != 15)goto _LL228;else{_tmp4EE=(void*)_tmp4ED->f1;}}_LL227:
# 2067
 rgn=_tmp4EE;
# 2068
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
# 2069
goto _LL225;_LL228:;_LL229:
# 2071
{const char*_tmp996;void*_tmp995[1];struct Cyc_String_pa_PrintArg_struct _tmp994;(_tmp994.tag=0,((_tmp994.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2072
Cyc_Absynpp_typ2string(handle_type)),((_tmp995[0]=& _tmp994,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc,((_tmp996="expecting region_t type but found %s",_tag_dyneither(_tmp996,sizeof(char),37))),_tag_dyneither(_tmp995,sizeof(void*),1)))))));}
# 2073
goto _LL225;_LL225:;}else{
# 2078
if(topt != 0){
# 2079
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
# 2080
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
# 2081
rgn=Cyc_Tcexp_mallocRgn(optrgn);
# 2082
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uniquergn_exp;}}}
# 2086
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*e);{
# 2095 "tcexp.cyc"
void*elt_type;
# 2096
struct Cyc_Absyn_Exp*num_elts;
# 2097
int one_elt;
# 2098
if(*is_calloc){
# 2099
if(*t == 0){const char*_tmp999;void*_tmp998;(_tmp998=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp999="calloc with empty type",_tag_dyneither(_tmp999,sizeof(char),23))),_tag_dyneither(_tmp998,sizeof(void*),0)));}
# 2100
elt_type=*((void**)_check_null(*t));
# 2101
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,elt_type);
# 2102
Cyc_Tcutil_check_no_qual(loc,elt_type);
# 2103
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
# 2104
num_elts=*e;
# 2105
one_elt=0;}else{
# 2107
void*er=(*e)->r;
# 2108
retry_sizeof: {
# 2109
void*_tmp4F6=er;void*_tmp4F8;enum Cyc_Absyn_Primop _tmp4FA;struct Cyc_List_List*_tmp4FB;struct Cyc_List_List _tmp4FC;struct Cyc_Absyn_Exp*_tmp4FD;struct Cyc_List_List*_tmp4FE;struct Cyc_List_List _tmp4FF;struct Cyc_Absyn_Exp*_tmp500;struct Cyc_List_List*_tmp501;_LL22B: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4F7=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4F6;if(_tmp4F7->tag != 16)goto _LL22D;else{_tmp4F8=(void*)_tmp4F7->f1;}}_LL22C:
# 2111
 elt_type=_tmp4F8;
# 2112
{void**_tmp99A;*t=(void**)((_tmp99A=_cycalloc(sizeof(*_tmp99A)),((_tmp99A[0]=elt_type,_tmp99A))));}
# 2113
num_elts=Cyc_Absyn_uint_exp(1,0);
# 2114
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,num_elts);
# 2115
one_elt=1;
# 2116
goto _LL22A;_LL22D: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4F9=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4F6;if(_tmp4F9->tag != 2)goto _LL22F;else{_tmp4FA=_tmp4F9->f1;if(_tmp4FA != Cyc_Absyn_Times)goto _LL22F;_tmp4FB=_tmp4F9->f2;if(_tmp4FB == 0)goto _LL22F;_tmp4FC=*_tmp4FB;_tmp4FD=(struct Cyc_Absyn_Exp*)_tmp4FC.hd;_tmp4FE=_tmp4FC.tl;if(_tmp4FE == 0)goto _LL22F;_tmp4FF=*_tmp4FE;_tmp500=(struct Cyc_Absyn_Exp*)_tmp4FF.hd;_tmp501=_tmp4FF.tl;if(_tmp501 != 0)goto _LL22F;}}_LL22E:
# 2118
{struct _tuple0 _tmp99B;struct _tuple0 _tmp504=(_tmp99B.f1=_tmp4FD->r,((_tmp99B.f2=_tmp500->r,_tmp99B)));void*_tmp505;void*_tmp507;void*_tmp508;void*_tmp50A;_LL232: _tmp505=_tmp504.f1;{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp506=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp505;if(_tmp506->tag != 16)goto _LL234;else{_tmp507=(void*)_tmp506->f1;}};_LL233:
# 2120
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp507);
# 2121
elt_type=_tmp507;
# 2122
{void**_tmp99C;*t=(void**)((_tmp99C=_cycalloc(sizeof(*_tmp99C)),((_tmp99C[0]=elt_type,_tmp99C))));}
# 2123
num_elts=_tmp500;
# 2124
one_elt=0;
# 2125
goto _LL231;_LL234: _tmp508=_tmp504.f2;{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp509=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp508;if(_tmp509->tag != 16)goto _LL236;else{_tmp50A=(void*)_tmp509->f1;}};_LL235:
# 2127
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp50A);
# 2128
elt_type=_tmp50A;
# 2129
{void**_tmp99D;*t=(void**)((_tmp99D=_cycalloc(sizeof(*_tmp99D)),((_tmp99D[0]=elt_type,_tmp99D))));}
# 2130
num_elts=_tmp4FD;
# 2131
one_elt=0;
# 2132
goto _LL231;_LL236:;_LL237:
# 2133
 goto No_sizeof;_LL231:;}
# 2135
goto _LL22A;_LL22F:;_LL230:
# 2137
 No_sizeof: {
# 2140
struct Cyc_Absyn_Exp*_tmp50D=*e;
# 2141
{void*_tmp50E=_tmp50D->r;struct Cyc_Absyn_Exp*_tmp510;_LL239: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp50F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp50E;if(_tmp50F->tag != 13)goto _LL23B;else{_tmp510=_tmp50F->f2;}}_LL23A:
# 2142
 _tmp50D=_tmp510;goto _LL238;_LL23B:;_LL23C:
# 2143
 goto _LL238;_LL238:;}
# 2145
{void*_tmp511=Cyc_Tcutil_compress((void*)_check_null(_tmp50D->topt));void*_tmp513;_LL23E: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp512=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp511;if(_tmp512->tag != 19)goto _LL240;else{_tmp513=(void*)_tmp512->f1;}}_LL23F:
# 2147
{void*_tmp514=Cyc_Tcutil_compress(_tmp513);struct Cyc_Absyn_Exp*_tmp516;_LL243: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp515=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp514;if(_tmp515->tag != 18)goto _LL245;else{_tmp516=_tmp515->f1;}}_LL244:
# 2149
 er=_tmp516->r;goto retry_sizeof;_LL245:;_LL246:
# 2150
 goto _LL242;_LL242:;}
# 2152
goto _LL23D;_LL240:;_LL241:
# 2153
 goto _LL23D;_LL23D:;}
# 2155
elt_type=Cyc_Absyn_char_typ;
# 2156
{void**_tmp99E;*t=(void**)((_tmp99E=_cycalloc(sizeof(*_tmp99E)),((_tmp99E[0]=elt_type,_tmp99E))));}
# 2157
num_elts=*e;
# 2158
one_elt=0;}
# 2160
goto _LL22A;_LL22A:;}}
# 2164
*is_fat=!one_elt;
# 2167
{void*_tmp518=elt_type;struct Cyc_Absyn_AggrInfo _tmp51A;union Cyc_Absyn_AggrInfoU _tmp51B;struct Cyc_Absyn_Aggrdecl**_tmp51C;struct Cyc_Absyn_Aggrdecl*_tmp51D;_LL248: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp519=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp518;if(_tmp519->tag != 11)goto _LL24A;else{_tmp51A=_tmp519->f1;_tmp51B=_tmp51A.aggr_info;if((_tmp51B.KnownAggr).tag != 2)goto _LL24A;_tmp51C=(struct Cyc_Absyn_Aggrdecl**)(_tmp51B.KnownAggr).val;_tmp51D=*_tmp51C;}}_LL249:
# 2169
 if(_tmp51D->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp51D->impl))->exist_vars != 0){
# 2170
const char*_tmp9A1;void*_tmp9A0;(_tmp9A0=0,Cyc_Tcutil_terr(loc,((_tmp9A1="malloc with existential types not yet implemented",_tag_dyneither(_tmp9A1,sizeof(char),50))),_tag_dyneither(_tmp9A0,sizeof(void*),0)));}
# 2171
goto _LL247;_LL24A:;_LL24B:
# 2172
 goto _LL247;_LL247:;}{
# 2176
void*(*_tmp520)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
# 2177
union Cyc_Absyn_Constraint*_tmp521=Cyc_Absyn_false_conref;
# 2178
if(topt != 0){
# 2179
void*_tmp522=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp524;struct Cyc_Absyn_PtrAtts _tmp525;union Cyc_Absyn_Constraint*_tmp526;union Cyc_Absyn_Constraint*_tmp527;union Cyc_Absyn_Constraint*_tmp528;_LL24D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp523=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp522;if(_tmp523->tag != 5)goto _LL24F;else{_tmp524=_tmp523->f1;_tmp525=_tmp524.ptr_atts;_tmp526=_tmp525.nullable;_tmp527=_tmp525.bounds;_tmp528=_tmp525.zero_term;}}_LL24E:
# 2181
 _tmp521=_tmp528;
# 2182
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp526))
# 2183
_tmp520=Cyc_Absyn_star_typ;
# 2186
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp528) && !(*is_calloc)){
# 2187
{const char*_tmp9A4;void*_tmp9A3;(_tmp9A3=0,Cyc_Tcutil_warn(loc,((_tmp9A4="converting malloc to calloc to ensure zero-termination",_tag_dyneither(_tmp9A4,sizeof(char),55))),_tag_dyneither(_tmp9A3,sizeof(void*),0)));}
# 2188
*is_calloc=1;}
# 2192
{void*_tmp52B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp527);struct Cyc_Absyn_Exp*_tmp52E;_LL252: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp52C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp52B;if(_tmp52C->tag != 0)goto _LL254;}_LL253:
# 2193
 goto _LL251;_LL254:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp52D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp52B;if(_tmp52D->tag != 1)goto _LL256;else{_tmp52E=_tmp52D->f1;}}if(!(!one_elt))goto _LL256;_LL255: {
# 2195
int _tmp52F=Cyc_Evexp_c_can_eval(num_elts);
# 2196
if(_tmp52F  && Cyc_Evexp_same_const_exp(_tmp52E,num_elts)){
# 2197
*is_fat=0;
# 2198
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp527),_tmp521);}
# 2201
{void*_tmp530=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp532;_LL259: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp531=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp530;if(_tmp531->tag != 19)goto _LL25B;else{_tmp532=(void*)_tmp531->f1;}}_LL25A: {
# 2203
struct Cyc_Absyn_Exp*_tmp533=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp532,0),0,Cyc_Absyn_No_coercion,0);
# 2205
if(Cyc_Evexp_same_const_exp(_tmp533,_tmp52E)){
# 2206
*is_fat=0;
# 2207
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp527),_tmp521);}
# 2210
goto _LL258;}_LL25B:;_LL25C:
# 2211
 goto _LL258;_LL258:;}
# 2213
goto _LL251;}_LL256:;_LL257:
# 2214
 goto _LL251;_LL251:;}
# 2216
goto _LL24C;_LL24F:;_LL250:
# 2217
 goto _LL24C;_LL24C:;}
# 2219
if(!one_elt)_tmp520=Cyc_Absyn_dyneither_typ;
# 2220
return _tmp520(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp521);};};}
# 2224
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2230
struct _RegionHandle _tmp534=_new_region("r");struct _RegionHandle*r=& _tmp534;_push_region(r);{
# 2231
struct Cyc_Tcenv_Tenv*_tmp535=Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te));
# 2232
Cyc_Tcexp_tcExp(_tmp535,0,e1);{
# 2233
void*_tmp536=(void*)_check_null(e1->topt);
# 2234
Cyc_Tcexp_tcExp(_tmp535,(void**)& _tmp536,e2);};}{
# 2236
void*t1=(void*)_check_null(e1->topt);
# 2237
void*t2=(void*)_check_null(e2->topt);
# 2239
{void*_tmp537=Cyc_Tcutil_compress(t1);_LL25E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp538=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp537;if(_tmp538->tag != 8)goto _LL260;}_LL25F:
# 2240
{const char*_tmp9A7;void*_tmp9A6;(_tmp9A6=0,Cyc_Tcutil_terr(loc,((_tmp9A7="cannot assign to an array",_tag_dyneither(_tmp9A7,sizeof(char),26))),_tag_dyneither(_tmp9A6,sizeof(void*),0)));}goto _LL25D;_LL260:;_LL261:
# 2241
 goto _LL25D;_LL25D:;}{
# 2244
int ign_1=0;
# 2245
if(!Cyc_Tcutil_is_pointer_or_boxed(t1,& ign_1)){
# 2246
const char*_tmp9AA;void*_tmp9A9;(_tmp9A9=0,Cyc_Tcutil_terr(loc,((_tmp9AA="Swap not allowed for non-pointer or non-word-sized types.",_tag_dyneither(_tmp9AA,sizeof(char),58))),_tag_dyneither(_tmp9A9,sizeof(void*),0)));}
# 2249
if(!Cyc_Absyn_is_lvalue(e1)){
# 2250
const char*_tmp9AD;void*_tmp9AC;void*_tmp53F=(_tmp9AC=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp9AD="swap non-lvalue",_tag_dyneither(_tmp9AD,sizeof(char),16))),_tag_dyneither(_tmp9AC,sizeof(void*),0)));_npop_handler(0);return _tmp53F;}
# 2251
if(!Cyc_Absyn_is_lvalue(e2)){
# 2252
const char*_tmp9B0;void*_tmp9AF;void*_tmp542=(_tmp9AF=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp9B0="swap non-lvalue",_tag_dyneither(_tmp9B0,sizeof(char),16))),_tag_dyneither(_tmp9AF,sizeof(void*),0)));_npop_handler(0);return _tmp542;}{
# 2254
void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
# 2255
int b_ign1=0;
# 2256
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
# 2257
const char*_tmp9B3;void*_tmp9B2;void*_tmp545=(_tmp9B2=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp9B3="swap value in zeroterm array",_tag_dyneither(_tmp9B3,sizeof(char),29))),_tag_dyneither(_tmp9B2,sizeof(void*),0)));_npop_handler(0);return _tmp545;}
# 2258
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2)){
# 2259
const char*_tmp9B6;void*_tmp9B5;void*_tmp548=(_tmp9B5=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp9B6="swap value in zeroterm array",_tag_dyneither(_tmp9B6,sizeof(char),29))),_tag_dyneither(_tmp9B5,sizeof(void*),0)));_npop_handler(0);return _tmp548;}
# 2261
Cyc_Tcexp_check_writable(te,e1);
# 2262
Cyc_Tcexp_check_writable(te,e2);
# 2263
if(!Cyc_Tcutil_unify(t1,t2)){
# 2264
const char*_tmp9BB;void*_tmp9BA[2];struct Cyc_String_pa_PrintArg_struct _tmp9B9;struct Cyc_String_pa_PrintArg_struct _tmp9B8;void*_tmp549=(_tmp9B8.tag=0,((_tmp9B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2265
Cyc_Absynpp_typ2string(t2)),((_tmp9B9.tag=0,((_tmp9B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp9BA[0]=& _tmp9B9,((_tmp9BA[1]=& _tmp9B8,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp9BB="type mismatch: %s != %s",_tag_dyneither(_tmp9BB,sizeof(char),24))),_tag_dyneither(_tmp9BA,sizeof(void*),2)))))))))))));
# 2266
void*_tmp54A=_tmp549;_npop_handler(0);return _tmp54A;}{
# 2268
void*_tmp54F=(void*)& Cyc_Absyn_VoidType_val;_npop_handler(0);return _tmp54F;};};};};
# 2230
;_pop_region(r);}
# 2272
int Cyc_Tcexp_in_stmt_exp=0;
# 2274
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
# 2275
struct _RegionHandle _tmp550=_new_region("r");struct _RegionHandle*r=& _tmp550;_push_region(r);{
# 2276
int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;
# 2277
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(r,te,s),s,1);
# 2278
Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}
# 2281
Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
# 2283
while(1){
# 2284
void*_tmp551=s->r;struct Cyc_Absyn_Exp*_tmp553;struct Cyc_Absyn_Stmt*_tmp555;struct Cyc_Absyn_Stmt*_tmp556;struct Cyc_Absyn_Decl*_tmp558;struct Cyc_Absyn_Stmt*_tmp559;_LL263: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp552=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp551;if(_tmp552->tag != 1)goto _LL265;else{_tmp553=_tmp552->f1;}}_LL264: {
# 2287
void*_tmp55A=(void*)_check_null(_tmp553->topt);
# 2288
if(!Cyc_Tcutil_unify(_tmp55A,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
# 2289
{const char*_tmp9BF;void*_tmp9BE[1];struct Cyc_String_pa_PrintArg_struct _tmp9BD;(_tmp9BD.tag=0,((_tmp9BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2290
Cyc_Absynpp_typ2string(_tmp55A)),((_tmp9BE[0]=& _tmp9BD,Cyc_Tcutil_terr(loc,((_tmp9BF="statement expression returns type %s",_tag_dyneither(_tmp9BF,sizeof(char),37))),_tag_dyneither(_tmp9BE,sizeof(void*),1)))))));}
# 2291
Cyc_Tcutil_explain_failure();}{
# 2293
void*_tmp55E=_tmp55A;_npop_handler(0);return _tmp55E;};}_LL265: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp554=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp551;if(_tmp554->tag != 2)goto _LL267;else{_tmp555=_tmp554->f1;_tmp556=_tmp554->f2;}}_LL266:
# 2294
 s=_tmp556;continue;_LL267: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp557=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp551;if(_tmp557->tag != 12)goto _LL269;else{_tmp558=_tmp557->f1;_tmp559=_tmp557->f2;}}_LL268:
# 2295
 s=_tmp559;continue;_LL269:;_LL26A: {
# 2297
const char*_tmp9C2;void*_tmp9C1;void*_tmp561=(_tmp9C1=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp9C2="statement expression must end with expression",_tag_dyneither(_tmp9C2,sizeof(char),46))),_tag_dyneither(_tmp9C1,sizeof(void*),0)));_npop_handler(0);return _tmp561;}_LL262:;}
# 2275
;_pop_region(r);}
# 2302
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
# 2303
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExp(te,0,e));
# 2304
{void*_tmp562=t;struct Cyc_Absyn_AggrInfo _tmp564;union Cyc_Absyn_AggrInfoU _tmp565;struct Cyc_Absyn_Aggrdecl**_tmp566;struct Cyc_Absyn_Aggrdecl*_tmp567;_LL26C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp563=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp562;if(_tmp563->tag != 11)goto _LL26E;else{_tmp564=_tmp563->f1;_tmp565=_tmp564.aggr_info;if((_tmp565.KnownAggr).tag != 2)goto _LL26E;_tmp566=(struct Cyc_Absyn_Aggrdecl**)(_tmp565.KnownAggr).val;_tmp567=*_tmp566;}}_LL26D:
# 2306
 if((_tmp567->kind == Cyc_Absyn_UnionA  && _tmp567->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp567->impl))->tagged)goto _LL26B;
# 2307
goto _LL26F;_LL26E:;_LL26F:
# 2309
{const char*_tmp9C6;void*_tmp9C5[1];struct Cyc_String_pa_PrintArg_struct _tmp9C4;(_tmp9C4.tag=0,((_tmp9C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp9C5[0]=& _tmp9C4,Cyc_Tcutil_terr(loc,((_tmp9C6="expecting @tagged union but found %s",_tag_dyneither(_tmp9C6,sizeof(char),37))),_tag_dyneither(_tmp9C5,sizeof(void*),1)))))));}
# 2310
goto _LL26B;_LL26B:;}
# 2312
return Cyc_Absyn_uint_typ;}
# 2316
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2320
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
# 2322
if(*rgn_handle != 0){
# 2324
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp9C9;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp9C8;void*expected_type=(void*)(
# 2325
(_tmp9C8=_cycalloc(sizeof(*_tmp9C8)),((_tmp9C8[0]=((_tmp9C9.tag=15,((_tmp9C9.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp9C9)))),_tmp9C8))));
# 2326
void*handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
# 2327
void*_tmp56B=Cyc_Tcutil_compress(handle_type);void*_tmp56D;_LL271: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp56C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp56B;if(_tmp56C->tag != 15)goto _LL273;else{_tmp56D=(void*)_tmp56C->f1;}}_LL272:
# 2329
 rgn=_tmp56D;
# 2330
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
# 2331
goto _LL270;_LL273:;_LL274:
# 2333
{const char*_tmp9CD;void*_tmp9CC[1];struct Cyc_String_pa_PrintArg_struct _tmp9CB;(_tmp9CB.tag=0,((_tmp9CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2334
Cyc_Absynpp_typ2string(handle_type)),((_tmp9CC[0]=& _tmp9CB,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc,((_tmp9CD="expecting region_t type but found %s",_tag_dyneither(_tmp9CD,sizeof(char),37))),_tag_dyneither(_tmp9CC,sizeof(void*),1)))))));}
# 2335
goto _LL270;_LL270:;}else{
# 2340
if(topt != 0){
# 2341
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
# 2342
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
# 2343
rgn=Cyc_Tcexp_mallocRgn(optrgn);
# 2344
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uniquergn_exp;}}}{
# 2348
void*_tmp573=e1->r;struct Cyc_Core_Opt*_tmp576;struct Cyc_List_List*_tmp577;struct Cyc_List_List*_tmp579;union Cyc_Absyn_Cnst _tmp57B;struct _dyneither_ptr _tmp57C;union Cyc_Absyn_Cnst _tmp57E;struct _dyneither_ptr _tmp57F;_LL276: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp574=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp573;if(_tmp574->tag != 26)goto _LL278;}_LL277: {
# 2351
void*_tmp580=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);
# 2352
void*_tmp581=Cyc_Tcutil_compress(_tmp580);struct Cyc_Absyn_ArrayInfo _tmp583;void*_tmp584;struct Cyc_Absyn_Tqual _tmp585;struct Cyc_Absyn_Exp*_tmp586;union Cyc_Absyn_Constraint*_tmp587;_LL283: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp582=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp581;if(_tmp582->tag != 8)goto _LL285;else{_tmp583=_tmp582->f1;_tmp584=_tmp583.elt_type;_tmp585=_tmp583.tq;_tmp586=_tmp583.num_elts;_tmp587=_tmp583.zero_term;}}_LL284: {
# 2354
struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp586);
# 2355
void*b;
# 2356
{void*_tmp588=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmp58A;_LL288: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp589=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp588;if(_tmp589->tag != 19)goto _LL28A;else{_tmp58A=(void*)_tmp589->f1;}}_LL289:
# 2358
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9D0;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9CF;b=(void*)((_tmp9CF=_cycalloc(sizeof(*_tmp9CF)),((_tmp9CF[0]=((_tmp9D0.tag=1,((_tmp9D0.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp58A,0),0,Cyc_Absyn_No_coercion,0),_tmp9D0)))),_tmp9CF))));}
# 2359
goto _LL287;_LL28A:;_LL28B:
# 2361
 if(Cyc_Tcutil_is_const_exp(te,bnd)){
# 2362
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9D3;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9D2;b=(void*)((_tmp9D2=_cycalloc(sizeof(*_tmp9D2)),((_tmp9D2[0]=((_tmp9D3.tag=1,((_tmp9D3.f1=bnd,_tmp9D3)))),_tmp9D2))));}else{
# 2364
b=(void*)& Cyc_Absyn_DynEither_b_val;}_LL287:;}{
# 2366
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp9DD;struct Cyc_Absyn_PtrAtts _tmp9DC;struct Cyc_Absyn_PtrInfo _tmp9DB;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9DA;void*res_typ=(void*)(
# 2367
(_tmp9DA=_cycalloc(sizeof(*_tmp9DA)),((_tmp9DA[0]=((_tmp9DD.tag=5,((_tmp9DD.f1=((_tmp9DB.elt_typ=_tmp584,((_tmp9DB.elt_tq=_tmp585,((_tmp9DB.ptr_atts=(
# 2368
(_tmp9DC.rgn=rgn,((_tmp9DC.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp9DC.bounds=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b),((_tmp9DC.zero_term=_tmp587,((_tmp9DC.ptrloc=0,_tmp9DC)))))))))),_tmp9DB)))))),_tmp9DD)))),_tmp9DA))));
# 2370
if(topt != 0){
# 2371
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){
# 2372
e->topt=(void*)res_typ;
# 2373
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
# 2374
res_typ=*topt;}}
# 2377
return res_typ;};}_LL285:;_LL286: {
# 2378
const char*_tmp9E0;void*_tmp9DF;(_tmp9DF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9E0="tcNew: comprehension returned non-array type",_tag_dyneither(_tmp9E0,sizeof(char),45))),_tag_dyneither(_tmp9DF,sizeof(void*),0)));}_LL282:;}_LL278: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp575=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp573;if(_tmp575->tag != 34)goto _LL27A;else{_tmp576=_tmp575->f1;_tmp577=_tmp575->f2;}}_LL279:
# 2381
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp9E3;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp9E2;e1->r=(void*)((_tmp9E2=_cycalloc(sizeof(*_tmp9E2)),((_tmp9E2[0]=((_tmp9E3.tag=25,((_tmp9E3.f1=_tmp577,_tmp9E3)))),_tmp9E2))));}
# 2382
_tmp579=_tmp577;goto _LL27B;_LL27A: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp578=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp573;if(_tmp578->tag != 25)goto _LL27C;else{_tmp579=_tmp578->f1;}}_LL27B: {
# 2384
void**elt_typ_opt=0;
# 2385
int zero_term=0;
# 2386
if(topt != 0){
# 2387
void*_tmp597=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp599;void*_tmp59A;void**_tmp59B;struct Cyc_Absyn_Tqual _tmp59C;struct Cyc_Absyn_PtrAtts _tmp59D;union Cyc_Absyn_Constraint*_tmp59E;_LL28D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp598=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp597;if(_tmp598->tag != 5)goto _LL28F;else{_tmp599=_tmp598->f1;_tmp59A=_tmp599.elt_typ;_tmp59B=(void**)&(_tmp598->f1).elt_typ;_tmp59C=_tmp599.elt_tq;_tmp59D=_tmp599.ptr_atts;_tmp59E=_tmp59D.zero_term;}}_LL28E:
# 2390
 elt_typ_opt=(void**)_tmp59B;
# 2391
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp59E);
# 2392
goto _LL28C;_LL28F:;_LL290:
# 2393
 goto _LL28C;_LL28C:;}{
# 2396
void*_tmp59F=Cyc_Tcexp_tcArray(te,e1->loc,elt_typ_opt,0,zero_term,_tmp579);
# 2397
e1->topt=(void*)_tmp59F;{
# 2398
void*res_typ;
# 2399
{void*_tmp5A0=Cyc_Tcutil_compress(_tmp59F);struct Cyc_Absyn_ArrayInfo _tmp5A2;void*_tmp5A3;struct Cyc_Absyn_Tqual _tmp5A4;struct Cyc_Absyn_Exp*_tmp5A5;union Cyc_Absyn_Constraint*_tmp5A6;_LL292: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5A1=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A0;if(_tmp5A1->tag != 8)goto _LL294;else{_tmp5A2=_tmp5A1->f1;_tmp5A3=_tmp5A2.elt_type;_tmp5A4=_tmp5A2.tq;_tmp5A5=_tmp5A2.num_elts;_tmp5A6=_tmp5A2.zero_term;}}_LL293:
# 2401
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp9F8;struct Cyc_Absyn_PtrAtts _tmp9F7;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9F6;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9F5;struct Cyc_Absyn_PtrInfo _tmp9F4;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9F3;res_typ=(void*)(
# 2402
(_tmp9F3=_cycalloc(sizeof(*_tmp9F3)),((_tmp9F3[0]=((_tmp9F8.tag=5,((_tmp9F8.f1=((_tmp9F4.elt_typ=_tmp5A3,((_tmp9F4.elt_tq=_tmp5A4,((_tmp9F4.ptr_atts=(
# 2403
(_tmp9F7.rgn=rgn,((_tmp9F7.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp9F7.bounds=
# 2404
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp9F5=_cycalloc(sizeof(*_tmp9F5)),((_tmp9F5[0]=((_tmp9F6.tag=1,((_tmp9F6.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp5A5),_tmp9F6)))),_tmp9F5))))),((_tmp9F7.zero_term=_tmp5A6,((_tmp9F7.ptrloc=0,_tmp9F7)))))))))),_tmp9F4)))))),_tmp9F8)))),_tmp9F3))));}
# 2406
if(topt != 0){
# 2410
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){
# 2411
e->topt=(void*)res_typ;
# 2412
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
# 2413
res_typ=*topt;}}
# 2416
goto _LL291;_LL294:;_LL295: {
# 2417
const char*_tmp9FB;void*_tmp9FA;(_tmp9FA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9FB="tcExpNoPromote on Array_e returned non-array type",_tag_dyneither(_tmp9FB,sizeof(char),50))),_tag_dyneither(_tmp9FA,sizeof(void*),0)));}_LL291:;}
# 2419
return res_typ;};};}_LL27C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp57A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp573;if(_tmp57A->tag != 0)goto _LL27E;else{_tmp57B=_tmp57A->f1;if((_tmp57B.String_c).tag != 8)goto _LL27E;_tmp57C=(struct _dyneither_ptr)(_tmp57B.String_c).val;}}_LL27D: {
# 2424
void*_tmp5AF=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2426
void*_tmp5B0=Cyc_Tcexp_tcExp(te,(void**)& _tmp5AF,e1);
# 2427
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9FE;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9FD;return Cyc_Absyn_atb_typ(_tmp5B0,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp9FD=_cycalloc(sizeof(*_tmp9FD)),((_tmp9FD[0]=((_tmp9FE.tag=1,((_tmp9FE.f1=Cyc_Absyn_uint_exp(1,0),_tmp9FE)))),_tmp9FD)))),Cyc_Absyn_false_conref);}_LL27E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp57D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp573;if(_tmp57D->tag != 0)goto _LL280;else{_tmp57E=_tmp57D->f1;if((_tmp57E.Wstring_c).tag != 9)goto _LL280;_tmp57F=(struct _dyneither_ptr)(_tmp57E.Wstring_c).val;}}_LL27F: {
# 2431
void*_tmp5B3=Cyc_Absyn_atb_typ(Cyc_Absyn_wchar_typ(),rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2433
void*_tmp5B4=Cyc_Tcexp_tcExp(te,(void**)& _tmp5B3,e1);
# 2434
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpA01;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA00;return Cyc_Absyn_atb_typ(_tmp5B4,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmpA00=_cycalloc(sizeof(*_tmpA00)),((_tmpA00[0]=((_tmpA01.tag=1,((_tmpA01.f1=Cyc_Absyn_uint_exp(1,0),_tmpA01)))),_tmpA00)))),Cyc_Absyn_false_conref);}_LL280:;_LL281:
# 2440
 RG: {
# 2441
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
# 2442
void**topt2=0;
# 2443
if(topt != 0){
# 2444
void*_tmp5B7=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp5B9;void*_tmp5BA;void**_tmp5BB;struct Cyc_Absyn_Tqual _tmp5BC;_LL297: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5B8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B7;if(_tmp5B8->tag != 5)goto _LL299;else{_tmp5B9=_tmp5B8->f1;_tmp5BA=_tmp5B9.elt_typ;_tmp5BB=(void**)&(_tmp5B8->f1).elt_typ;_tmp5BC=_tmp5B9.elt_tq;}}_LL298:
# 2446
 topt2=(void**)_tmp5BB;goto _LL296;_LL299: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5BD=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5B7;if(_tmp5BD->tag != 3)goto _LL29B;}_LL29A:
# 2450
 bogus=*topt;
# 2451
topt2=(void**)& bogus;
# 2452
goto _LL296;_LL29B:;_LL29C:
# 2453
 goto _LL296;_LL296:;}{
# 2456
void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);
# 2457
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpA0B;struct Cyc_Absyn_PtrAtts _tmpA0A;struct Cyc_Absyn_PtrInfo _tmpA09;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA08;void*res_typ=(void*)(
# 2458
(_tmpA08=_cycalloc(sizeof(*_tmpA08)),((_tmpA08[0]=((_tmpA0B.tag=5,((_tmpA0B.f1=((_tmpA09.elt_typ=telt,((_tmpA09.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmpA09.ptr_atts=(
# 2459
(_tmpA0A.rgn=rgn,((_tmpA0A.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmpA0A.bounds=Cyc_Absyn_bounds_one_conref,((_tmpA0A.zero_term=Cyc_Absyn_false_conref,((_tmpA0A.ptrloc=0,_tmpA0A)))))))))),_tmpA09)))))),_tmpA0B)))),_tmpA08))));
# 2462
if(topt != 0){
# 2463
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){
# 2464
e->topt=(void*)res_typ;
# 2465
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
# 2466
res_typ=*topt;}}
# 2469
return res_typ;};}_LL275:;};}
# 2475
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
# 2476
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
# 2477
void*_tmp5C2=t;struct Cyc_Absyn_ArrayInfo _tmp5C4;void*_tmp5C5;struct Cyc_Absyn_Tqual _tmp5C6;struct Cyc_Absyn_Exp*_tmp5C7;union Cyc_Absyn_Constraint*_tmp5C8;_LL29E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5C3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5C2;if(_tmp5C3->tag != 8)goto _LL2A0;else{_tmp5C4=_tmp5C3->f1;_tmp5C5=_tmp5C4.elt_type;_tmp5C6=_tmp5C4.tq;_tmp5C7=_tmp5C4.num_elts;_tmp5C8=_tmp5C4.zero_term;}}_LL29F: {
# 2479
void*_tmp5CA;struct _tuple14 _tmp5C9=Cyc_Tcutil_addressof_props(te,e);_tmp5CA=_tmp5C9.f2;{
# 2480
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpA0E;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA0D;void*_tmp5CB=_tmp5C7 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmpA0D=_cycalloc(sizeof(*_tmpA0D)),((_tmpA0D[0]=((_tmpA0E.tag=1,((_tmpA0E.f1=(struct Cyc_Absyn_Exp*)_tmp5C7,_tmpA0E)))),_tmpA0D))));
# 2481
t=Cyc_Absyn_atb_typ(_tmp5C5,_tmp5CA,_tmp5C6,_tmp5CB,_tmp5C8);
# 2482
e->topt=(void*)t;
# 2483
return t;};}_LL2A0:;_LL2A1:
# 2485
 return t;_LL29D:;}
# 2492
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
# 2493
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2496
struct _RegionHandle _tmp5CE=_new_region("temp");struct _RegionHandle*temp=& _tmp5CE;_push_region(temp);
# 2497
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
# 2502
const char*_tmpA11;void*_tmpA10;(_tmpA10=0,Cyc_Tcutil_terr(e->loc,((_tmpA11="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmpA11,sizeof(char),49))),_tag_dyneither(_tmpA10,sizeof(void*),0)));}{
# 2505
void*_tmp5D1=e->r;union Cyc_Absyn_Cnst _tmp5D5;struct _dyneither_ptr _tmp5D6;union Cyc_Absyn_Cnst _tmp5D8;struct _dyneither_ptr _tmp5D9;_LL2A3: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5D2=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5D1;if(_tmp5D2->tag != 25)goto _LL2A5;}_LL2A4:
# 2506
 goto _LL2A6;_LL2A5: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp5D3=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5D1;if(_tmp5D3->tag != 26)goto _LL2A7;}_LL2A6:
# 2507
 goto _LL2A8;_LL2A7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5D4=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D1;if(_tmp5D4->tag != 0)goto _LL2A9;else{_tmp5D5=_tmp5D4->f1;if((_tmp5D5.Wstring_c).tag != 9)goto _LL2A9;_tmp5D6=(struct _dyneither_ptr)(_tmp5D5.Wstring_c).val;}}_LL2A8:
# 2508
 goto _LL2AA;_LL2A9: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5D7=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D1;if(_tmp5D7->tag != 0)goto _LL2AB;else{_tmp5D8=_tmp5D7->f1;if((_tmp5D8.String_c).tag != 8)goto _LL2AB;_tmp5D9=(struct _dyneither_ptr)(_tmp5D8.String_c).val;}}_LL2AA: {
# 2509
void*_tmp5DA=t;_npop_handler(0);return _tmp5DA;}_LL2AB:;_LL2AC:
# 2511
 t=Cyc_Tcutil_compress(t);{
# 2512
void*_tmp5DB=t;struct Cyc_Absyn_ArrayInfo _tmp5DD;void*_tmp5DE;struct Cyc_Absyn_Tqual _tmp5DF;struct Cyc_Absyn_Exp*_tmp5E0;union Cyc_Absyn_Constraint*_tmp5E1;_LL2AE: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5DC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5DB;if(_tmp5DC->tag != 8)goto _LL2B0;else{_tmp5DD=_tmp5DC->f1;_tmp5DE=_tmp5DD.elt_type;_tmp5DF=_tmp5DD.tq;_tmp5E0=_tmp5DD.num_elts;_tmp5E1=_tmp5DD.zero_term;}}_LL2AF: {
# 2514
void*_tmp5E3;struct _tuple14 _tmp5E2=Cyc_Tcutil_addressof_props(te,e);_tmp5E3=_tmp5E2.f2;{
# 2515
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpA14;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA13;void*b=_tmp5E0 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)(
# 2516
(_tmpA13=_cycalloc(sizeof(*_tmpA13)),((_tmpA13[0]=((_tmpA14.tag=1,((_tmpA14.f1=(struct Cyc_Absyn_Exp*)_tmp5E0,_tmpA14)))),_tmpA13))));
# 2517
t=Cyc_Absyn_atb_typ(_tmp5DE,_tmp5E3,_tmp5DF,b,_tmp5E1);
# 2518
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);{
# 2519
void*_tmp5E4=t;_npop_handler(0);return _tmp5E4;};};}_LL2B0:;_LL2B1: {
# 2521
void*_tmp5E7=t;_npop_handler(0);return _tmp5E7;}_LL2AD:;};_LL2A2:;};
# 2496
;_pop_region(temp);}
# 2533 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
# 2534
{void*_tmp5E8=e->r;struct Cyc_Absyn_Exp*_tmp5EA;_LL2B3: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp5E9=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5E8;if(_tmp5E9->tag != 11)goto _LL2B5;else{_tmp5EA=_tmp5E9->f1;}}_LL2B4:
# 2537
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp5EA);
# 2538
_tmp5EA->topt=(void*)Cyc_Absyn_pointer_expand((void*)_check_null(_tmp5EA->topt),0);
# 2539
e->topt=_tmp5EA->topt;
# 2540
goto _LL2B2;_LL2B5:;_LL2B6:
# 2543
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
# 2544
e->topt=(void*)Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);
# 2546
{void*_tmp5EB=Cyc_Tcutil_compress((void*)_check_null(e->topt));struct Cyc_Absyn_PtrInfo _tmp5ED;void*_tmp5EE;struct Cyc_Absyn_Tqual _tmp5EF;struct Cyc_Absyn_PtrAtts _tmp5F0;void*_tmp5F1;union Cyc_Absyn_Constraint*_tmp5F2;union Cyc_Absyn_Constraint*_tmp5F3;union Cyc_Absyn_Constraint*_tmp5F4;_LL2B8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5EC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5EB;if(_tmp5EC->tag != 5)goto _LL2BA;else{_tmp5ED=_tmp5EC->f1;_tmp5EE=_tmp5ED.elt_typ;_tmp5EF=_tmp5ED.elt_tq;_tmp5F0=_tmp5ED.ptr_atts;_tmp5F1=_tmp5F0.rgn;_tmp5F2=_tmp5F0.nullable;_tmp5F3=_tmp5F0.bounds;_tmp5F4=_tmp5F0.zero_term;}}_LL2B9:
# 2548
{void*_tmp5F5=Cyc_Tcutil_compress(_tmp5EE);struct Cyc_Absyn_FnInfo _tmp5F7;struct Cyc_List_List*_tmp5F8;void*_tmp5F9;struct Cyc_Absyn_Tqual _tmp5FA;void*_tmp5FB;struct Cyc_List_List*_tmp5FC;int _tmp5FD;struct Cyc_Absyn_VarargInfo*_tmp5FE;struct Cyc_List_List*_tmp5FF;struct Cyc_List_List*_tmp600;_LL2BD: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5F6=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5F5;if(_tmp5F6->tag != 9)goto _LL2BF;else{_tmp5F7=_tmp5F6->f1;_tmp5F8=_tmp5F7.tvars;_tmp5F9=_tmp5F7.effect;_tmp5FA=_tmp5F7.ret_tqual;_tmp5FB=_tmp5F7.ret_typ;_tmp5FC=_tmp5F7.args;_tmp5FD=_tmp5F7.c_varargs;_tmp5FE=_tmp5F7.cyc_varargs;_tmp5FF=_tmp5F7.rgn_po;_tmp600=_tmp5F7.attributes;}}_LL2BE:
# 2550
 if(_tmp5F8 != 0){
# 2551
struct _RegionHandle _tmp601=_new_region("rgn");struct _RegionHandle*rgn=& _tmp601;_push_region(rgn);
# 2552
{struct _tuple12 _tmpA15;struct _tuple12 _tmp602=(_tmpA15.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmpA15.f2=rgn,_tmpA15)));
# 2553
struct Cyc_List_List*inst=
# 2554
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp602,_tmp5F8);
# 2555
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2556
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpA1B;struct Cyc_Absyn_FnInfo _tmpA1A;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA19;void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
# 2557
(_tmpA19=_cycalloc(sizeof(*_tmpA19)),((_tmpA19[0]=((_tmpA1B.tag=9,((_tmpA1B.f1=((_tmpA1A.tvars=0,((_tmpA1A.effect=_tmp5F9,((_tmpA1A.ret_tqual=_tmp5FA,((_tmpA1A.ret_typ=_tmp5FB,((_tmpA1A.args=_tmp5FC,((_tmpA1A.c_varargs=_tmp5FD,((_tmpA1A.cyc_varargs=_tmp5FE,((_tmpA1A.rgn_po=_tmp5FF,((_tmpA1A.attributes=_tmp600,_tmpA1A)))))))))))))))))),_tmpA1B)))),_tmpA19)))));
# 2560
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpA25;struct Cyc_Absyn_PtrAtts _tmpA24;struct Cyc_Absyn_PtrInfo _tmpA23;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA22;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp603=(_tmpA22=_cycalloc(sizeof(*_tmpA22)),((_tmpA22[0]=((_tmpA25.tag=5,((_tmpA25.f1=((_tmpA23.elt_typ=ftyp,((_tmpA23.elt_tq=_tmp5EF,((_tmpA23.ptr_atts=((_tmpA24.rgn=_tmp5F1,((_tmpA24.nullable=_tmp5F2,((_tmpA24.bounds=_tmp5F3,((_tmpA24.zero_term=_tmp5F4,((_tmpA24.ptrloc=0,_tmpA24)))))))))),_tmpA23)))))),_tmpA25)))),_tmpA22)));
# 2562
struct Cyc_Absyn_Exp*_tmp604=Cyc_Absyn_copy_exp(e);
# 2563
{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmpA28;struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpA27;e->r=(void*)((_tmpA27=_cycalloc(sizeof(*_tmpA27)),((_tmpA27[0]=((_tmpA28.tag=12,((_tmpA28.f1=_tmp604,((_tmpA28.f2=ts,_tmpA28)))))),_tmpA27))));}
# 2564
e->topt=(void*)_tmp603;}
# 2552
;_pop_region(rgn);}
# 2567
goto _LL2BC;_LL2BF:;_LL2C0:
# 2568
 goto _LL2BC;_LL2BC:;}
# 2570
goto _LL2B7;_LL2BA:;_LL2BB:
# 2571
 goto _LL2B7;_LL2B7:;}
# 2573
goto _LL2B2;_LL2B2:;}
# 2575
return(void*)_check_null(e->topt);}
# 2583
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2585
struct _RegionHandle _tmp60F=_new_region("r");struct _RegionHandle*r=& _tmp60F;_push_region(r);
# 2586
{struct Cyc_List_List*_tmp610=0;
# 2588
{void*_tmp611=fn_exp->r;struct Cyc_List_List*_tmp613;_LL2C2: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp612=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp611;if(_tmp612->tag != 9)goto _LL2C4;else{_tmp613=_tmp612->f2;}}_LL2C3:
# 2590
{void*_tmp614=e->r;struct Cyc_List_List*_tmp616;_LL2C7: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp615=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp614;if(_tmp615->tag != 9)goto _LL2C9;else{_tmp616=_tmp615->f2;}}_LL2C8: {
# 2592
struct Cyc_List_List*_tmp617=alias_arg_exps;
# 2593
int _tmp618=0;
# 2594
while(_tmp617 != 0){
# 2595
while(_tmp618 != (int)_tmp617->hd){
# 2596
if(_tmp613 == 0){
# 2597
struct Cyc_Int_pa_PrintArg_struct _tmpA32;struct Cyc_Int_pa_PrintArg_struct _tmpA31;void*_tmpA30[2];const char*_tmpA2F;void*_tmpA2E;(_tmpA2E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpA32.tag=1,((_tmpA32.f1=(unsigned long)((int)_tmp617->hd),((_tmpA31.tag=1,((_tmpA31.f1=(unsigned long)_tmp618,((_tmpA30[0]=& _tmpA31,((_tmpA30[1]=& _tmpA32,Cyc_aprintf(((_tmpA2F="bad count %d/%d for alias coercion!",_tag_dyneither(_tmpA2F,sizeof(char),36))),_tag_dyneither(_tmpA30,sizeof(void*),2)))))))))))))),_tag_dyneither(_tmpA2E,sizeof(void*),0)));}
# 2599
++ _tmp618;
# 2600
_tmp613=_tmp613->tl;
# 2601
_tmp616=((struct Cyc_List_List*)_check_null(_tmp616))->tl;}{
# 2604
struct Cyc_Absyn_Decl*_tmp61F;struct Cyc_Absyn_Exp*_tmp620;struct _tuple11 _tmp61E=Cyc_Tcutil_insert_alias((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp613))->hd,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp616))->hd)->topt)));_tmp61F=_tmp61E.f1;_tmp620=_tmp61E.f2;
# 2605
_tmp613->hd=(void*)_tmp620;
# 2606
{struct Cyc_List_List*_tmpA33;_tmp610=((_tmpA33=_region_malloc(r,sizeof(*_tmpA33)),((_tmpA33->hd=_tmp61F,((_tmpA33->tl=_tmp610,_tmpA33))))));}
# 2607
_tmp617=_tmp617->tl;};}
# 2609
goto _LL2C6;}_LL2C9:;_LL2CA: {
# 2610
const char*_tmpA36;void*_tmpA35;(_tmpA35=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA36="not a function call!",_tag_dyneither(_tmpA36,sizeof(char),21))),_tag_dyneither(_tmpA35,sizeof(void*),0)));}_LL2C6:;}
# 2612
goto _LL2C1;_LL2C4:;_LL2C5: {
# 2613
const char*_tmpA39;void*_tmpA38;(_tmpA38=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA39="not a function call!",_tag_dyneither(_tmpA39,sizeof(char),21))),_tag_dyneither(_tmpA38,sizeof(void*),0)));}_LL2C1:;}
# 2617
while(_tmp610 != 0){
# 2618
struct Cyc_Absyn_Decl*_tmp626=(struct Cyc_Absyn_Decl*)_tmp610->hd;
# 2619
fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(_tmp626,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);
# 2620
_tmp610=_tmp610->tl;}
# 2624
e->topt=0;
# 2625
e->r=fn_exp->r;}
# 2586
;_pop_region(r);}
# 2630
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
# 2631
unsigned int loc=e->loc;
# 2632
void*t;
# 2634
{void*_tmp627=e->r;struct Cyc_Absyn_Exp*_tmp629;struct Cyc_Absyn_Exp*_tmp62B;struct Cyc_List_List*_tmp62C;struct Cyc_Absyn_VarargCallInfo*_tmp62D;struct Cyc_Absyn_VarargCallInfo**_tmp62E;int _tmp62F;int*_tmp630;struct Cyc_Core_Opt*_tmp632;struct Cyc_List_List*_tmp633;union Cyc_Absyn_Cnst _tmp635;union Cyc_Absyn_Cnst*_tmp636;struct _tuple1*_tmp638;void*_tmp639;void**_tmp63A;enum Cyc_Absyn_Primop _tmp63C;struct Cyc_List_List*_tmp63D;struct Cyc_Absyn_Exp*_tmp63F;enum Cyc_Absyn_Incrementor _tmp640;struct Cyc_Absyn_Exp*_tmp642;struct Cyc_Core_Opt*_tmp643;struct Cyc_Absyn_Exp*_tmp644;struct Cyc_Absyn_Exp*_tmp646;struct Cyc_Absyn_Exp*_tmp647;struct Cyc_Absyn_Exp*_tmp648;struct Cyc_Absyn_Exp*_tmp64A;struct Cyc_Absyn_Exp*_tmp64B;struct Cyc_Absyn_Exp*_tmp64D;struct Cyc_Absyn_Exp*_tmp64E;struct Cyc_Absyn_Exp*_tmp650;struct Cyc_Absyn_Exp*_tmp651;struct Cyc_Absyn_Exp*_tmp653;struct Cyc_List_List*_tmp654;struct Cyc_Absyn_VarargCallInfo*_tmp655;struct Cyc_Absyn_VarargCallInfo**_tmp656;struct Cyc_Absyn_Exp*_tmp658;struct Cyc_Absyn_Exp*_tmp65A;struct Cyc_List_List*_tmp65B;void*_tmp65D;struct Cyc_Absyn_Exp*_tmp65E;enum Cyc_Absyn_Coercion _tmp65F;enum Cyc_Absyn_Coercion*_tmp660;struct Cyc_Absyn_Exp*_tmp662;struct Cyc_Absyn_Exp*_tmp664;struct Cyc_Absyn_Exp**_tmp665;struct Cyc_Absyn_Exp*_tmp666;struct Cyc_Absyn_Exp*_tmp668;void*_tmp66A;void*_tmp66C;void*_tmp66D;struct Cyc_Absyn_Exp*_tmp66F;struct Cyc_Absyn_Exp*_tmp671;struct _dyneither_ptr*_tmp672;int _tmp673;int*_tmp674;int _tmp675;int*_tmp676;struct Cyc_Absyn_Exp*_tmp678;struct _dyneither_ptr*_tmp679;int _tmp67A;int*_tmp67B;int _tmp67C;int*_tmp67D;struct Cyc_Absyn_Exp*_tmp67F;struct Cyc_Absyn_Exp*_tmp680;struct Cyc_List_List*_tmp682;struct _tuple9*_tmp684;struct Cyc_List_List*_tmp685;struct Cyc_List_List*_tmp687;struct Cyc_Absyn_Stmt*_tmp689;struct Cyc_Absyn_Vardecl*_tmp68B;struct Cyc_Absyn_Exp*_tmp68C;struct Cyc_Absyn_Exp*_tmp68D;int _tmp68E;int*_tmp68F;struct _tuple1*_tmp691;struct _tuple1**_tmp692;struct Cyc_List_List*_tmp693;struct Cyc_List_List**_tmp694;struct Cyc_List_List*_tmp695;struct Cyc_Absyn_Aggrdecl*_tmp696;struct Cyc_Absyn_Aggrdecl**_tmp697;void*_tmp699;struct Cyc_List_List*_tmp69A;struct Cyc_List_List*_tmp69C;struct Cyc_Absyn_Datatypedecl*_tmp69D;struct Cyc_Absyn_Datatypefield*_tmp69E;struct _tuple1*_tmp6A0;struct _tuple1**_tmp6A1;struct Cyc_Absyn_Enumdecl*_tmp6A2;struct Cyc_Absyn_Enumfield*_tmp6A3;struct _tuple1*_tmp6A5;struct _tuple1**_tmp6A6;void*_tmp6A7;struct Cyc_Absyn_Enumfield*_tmp6A8;struct Cyc_Absyn_MallocInfo _tmp6AA;int _tmp6AB;int*_tmp6AC;struct Cyc_Absyn_Exp*_tmp6AD;struct Cyc_Absyn_Exp**_tmp6AE;void**_tmp6AF;void***_tmp6B0;struct Cyc_Absyn_Exp*_tmp6B1;struct Cyc_Absyn_Exp**_tmp6B2;int _tmp6B3;int*_tmp6B4;struct Cyc_Absyn_Exp*_tmp6B6;struct Cyc_Absyn_Exp*_tmp6B7;struct Cyc_Absyn_Exp*_tmp6B9;struct _dyneither_ptr*_tmp6BA;void*_tmp6BC;_LL2CC: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp628=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp628->tag != 11)goto _LL2CE;else{_tmp629=_tmp628->f1;}}_LL2CD:
# 2639
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp629);
# 2640
return;_LL2CE:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp62A=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp62A->tag != 9)goto _LL2D0;else{_tmp62B=_tmp62A->f1;_tmp62C=_tmp62A->f2;_tmp62D=_tmp62A->f3;_tmp62E=(struct Cyc_Absyn_VarargCallInfo**)& _tmp62A->f3;_tmp62F=_tmp62A->f4;_tmp630=(int*)& _tmp62A->f4;}}if(!(!(*_tmp630)))goto _LL2D0;_LL2CF:
# 2645
 Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp62B,_tmp62C);
# 2646
*_tmp630=1;
# 2647
Cyc_Tcexp_tcExpNoInst(te,topt,e);
# 2648
return;_LL2D0: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp631=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp631->tag != 34)goto _LL2D2;else{_tmp632=_tmp631->f1;_tmp633=_tmp631->f2;}}_LL2D1:
# 2652
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp633);
# 2653
Cyc_Tcexp_tcExpNoInst(te,topt,e);
# 2654
return;_LL2D2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp634=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp634->tag != 0)goto _LL2D4;else{_tmp635=_tmp634->f1;_tmp636=(union Cyc_Absyn_Cnst*)& _tmp634->f1;}}_LL2D3:
# 2657
 t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst*)_tmp636,e);goto _LL2CB;_LL2D4: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp637=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp637->tag != 1)goto _LL2D6;else{_tmp638=_tmp637->f1;_tmp639=(void**)& _tmp637->f2;_tmp63A=(void**)((void**)& _tmp637->f2);}}_LL2D5:
# 2659
 t=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp638,(void**)_tmp63A);goto _LL2CB;_LL2D6: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp63B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp63B->tag != 2)goto _LL2D8;else{_tmp63C=_tmp63B->f1;_tmp63D=_tmp63B->f2;}}_LL2D7:
# 2661
 t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp63C,_tmp63D);goto _LL2CB;_LL2D8: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp63E=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp63E->tag != 4)goto _LL2DA;else{_tmp63F=_tmp63E->f1;_tmp640=_tmp63E->f2;}}_LL2D9:
# 2663
 t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp63F,_tmp640);goto _LL2CB;_LL2DA: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp641=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp641->tag != 3)goto _LL2DC;else{_tmp642=_tmp641->f1;_tmp643=_tmp641->f2;_tmp644=_tmp641->f3;}}_LL2DB:
# 2665
 t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp642,_tmp643,_tmp644);goto _LL2CB;_LL2DC: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp645=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp645->tag != 5)goto _LL2DE;else{_tmp646=_tmp645->f1;_tmp647=_tmp645->f2;_tmp648=_tmp645->f3;}}_LL2DD:
# 2667
 t=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp646,_tmp647,_tmp648);goto _LL2CB;_LL2DE: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp649=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp649->tag != 6)goto _LL2E0;else{_tmp64A=_tmp649->f1;_tmp64B=_tmp649->f2;}}_LL2DF:
# 2669
 t=Cyc_Tcexp_tcAnd(te,loc,_tmp64A,_tmp64B);goto _LL2CB;_LL2E0: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp64C=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp64C->tag != 7)goto _LL2E2;else{_tmp64D=_tmp64C->f1;_tmp64E=_tmp64C->f2;}}_LL2E1:
# 2671
 t=Cyc_Tcexp_tcOr(te,loc,_tmp64D,_tmp64E);goto _LL2CB;_LL2E2: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp64F=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp64F->tag != 8)goto _LL2E4;else{_tmp650=_tmp64F->f1;_tmp651=_tmp64F->f2;}}_LL2E3:
# 2673
 t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp650,_tmp651);goto _LL2CB;_LL2E4: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp652=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp652->tag != 9)goto _LL2E6;else{_tmp653=_tmp652->f1;_tmp654=_tmp652->f2;_tmp655=_tmp652->f3;_tmp656=(struct Cyc_Absyn_VarargCallInfo**)& _tmp652->f3;}}_LL2E5: {
# 2679
struct Cyc_List_List*alias_arg_exps=0;
# 2680
int ok=1;
# 2681
struct Cyc_Absyn_Exp*fn_exp;
# 2682
{struct _handler_cons _tmp6BE;_push_handler(& _tmp6BE);{int _tmp6C0=0;if(setjmp(_tmp6BE.handler))_tmp6C0=1;if(!_tmp6C0){
# 2683
fn_exp=Cyc_Tcutil_deep_copy_exp(e);;_pop_handler();}else{void*_tmp6BF=(void*)_exn_thrown;void*_tmp6C2=_tmp6BF;struct _dyneither_ptr _tmp6C4;_LL31D: {struct Cyc_Core_Failure_exn_struct*_tmp6C3=(struct Cyc_Core_Failure_exn_struct*)_tmp6C2;if(_tmp6C3->tag != Cyc_Core_Failure)goto _LL31F;else{_tmp6C4=_tmp6C3->f1;}}_LL31E:
# 2686
 ok=0;
# 2687
fn_exp=e;
# 2688
goto _LL31C;_LL31F:;_LL320:(void)_throw(_tmp6C2);_LL31C:;}};}
# 2690
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp653,_tmp654,_tmp656,& alias_arg_exps);
# 2691
if(alias_arg_exps != 0  && ok){
# 2692
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
# 2693
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
# 2694
Cyc_Tcexp_tcExpNoInst(te,topt,e);
# 2695
return;}
# 2697
goto _LL2CB;}_LL2E6: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp657=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp657->tag != 10)goto _LL2E8;else{_tmp658=_tmp657->f1;}}_LL2E7:
# 2699
 t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp658);goto _LL2CB;_LL2E8: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp659=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp659->tag != 12)goto _LL2EA;else{_tmp65A=_tmp659->f1;_tmp65B=_tmp659->f2;}}_LL2E9:
# 2701
 t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp65A,_tmp65B);goto _LL2CB;_LL2EA: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp65C=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp65C->tag != 13)goto _LL2EC;else{_tmp65D=(void*)_tmp65C->f1;_tmp65E=_tmp65C->f2;_tmp65F=_tmp65C->f4;_tmp660=(enum Cyc_Absyn_Coercion*)& _tmp65C->f4;}}_LL2EB:
# 2703
 t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp65D,_tmp65E,(enum Cyc_Absyn_Coercion*)_tmp660);goto _LL2CB;_LL2EC: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp661=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp661->tag != 14)goto _LL2EE;else{_tmp662=_tmp661->f1;}}_LL2ED:
# 2705
 t=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp662);goto _LL2CB;_LL2EE: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp663=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp663->tag != 15)goto _LL2F0;else{_tmp664=_tmp663->f1;_tmp665=(struct Cyc_Absyn_Exp**)& _tmp663->f1;_tmp666=_tmp663->f2;}}_LL2EF:
# 2707
 t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp665,e,_tmp666);goto _LL2CB;_LL2F0: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp667=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp667->tag != 17)goto _LL2F2;else{_tmp668=_tmp667->f1;}}_LL2F1: {
# 2709
void*_tmp6C5=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp668);
# 2710
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp6C5);goto _LL2CB;}_LL2F2: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp669=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp669->tag != 16)goto _LL2F4;else{_tmp66A=(void*)_tmp669->f1;}}_LL2F3:
# 2712
 t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp66A);goto _LL2CB;_LL2F4: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp66B=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp66B->tag != 18)goto _LL2F6;else{_tmp66C=(void*)_tmp66B->f1;_tmp66D=(void*)_tmp66B->f2;}}_LL2F5:
# 2714
 t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp66C,_tmp66D);goto _LL2CB;_LL2F6: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp66E=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp66E->tag != 19)goto _LL2F8;else{_tmp66F=_tmp66E->f1;}}_LL2F7:
# 2716
 t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp66F);goto _LL2CB;_LL2F8: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp670=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp670->tag != 20)goto _LL2FA;else{_tmp671=_tmp670->f1;_tmp672=_tmp670->f2;_tmp673=_tmp670->f3;_tmp674=(int*)& _tmp670->f3;_tmp675=_tmp670->f4;_tmp676=(int*)& _tmp670->f4;}}_LL2F9:
# 2718
 t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp671,_tmp672,_tmp674,_tmp676);goto _LL2CB;_LL2FA: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp677=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp677->tag != 21)goto _LL2FC;else{_tmp678=_tmp677->f1;_tmp679=_tmp677->f2;_tmp67A=_tmp677->f3;_tmp67B=(int*)& _tmp677->f3;_tmp67C=_tmp677->f4;_tmp67D=(int*)& _tmp677->f4;}}_LL2FB:
# 2720
 t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp678,_tmp679,_tmp67B,_tmp67D);goto _LL2CB;_LL2FC: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp67E=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp67E->tag != 22)goto _LL2FE;else{_tmp67F=_tmp67E->f1;_tmp680=_tmp67E->f2;}}_LL2FD:
# 2722
 t=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp67F,_tmp680);goto _LL2CB;_LL2FE: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp681=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp681->tag != 23)goto _LL300;else{_tmp682=_tmp681->f1;}}_LL2FF:
# 2724
 t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp682);goto _LL2CB;_LL300: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp683=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp683->tag != 24)goto _LL302;else{_tmp684=_tmp683->f1;_tmp685=_tmp683->f2;}}_LL301:
# 2726
 t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp684,_tmp685);goto _LL2CB;_LL302: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp686=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp686->tag != 25)goto _LL304;else{_tmp687=_tmp686->f1;}}_LL303: {
# 2730
void**elt_topt=0;
# 2731
struct Cyc_Absyn_Tqual*elt_tqopt=0;
# 2732
int zero_term=0;
# 2733
if(topt != 0){
# 2734
void*_tmp6C6=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp6C8;void*_tmp6C9;void**_tmp6CA;struct Cyc_Absyn_Tqual _tmp6CB;struct Cyc_Absyn_Tqual*_tmp6CC;union Cyc_Absyn_Constraint*_tmp6CD;_LL322: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp6C7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6C6;if(_tmp6C7->tag != 8)goto _LL324;else{_tmp6C8=_tmp6C7->f1;_tmp6C9=_tmp6C8.elt_type;_tmp6CA=(void**)&(_tmp6C7->f1).elt_type;_tmp6CB=_tmp6C8.tq;_tmp6CC=(struct Cyc_Absyn_Tqual*)&(_tmp6C7->f1).tq;_tmp6CD=_tmp6C8.zero_term;}}_LL323:
# 2736
 elt_topt=(void**)_tmp6CA;
# 2737
elt_tqopt=(struct Cyc_Absyn_Tqual*)_tmp6CC;
# 2738
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6CD);
# 2739
goto _LL321;_LL324:;_LL325:
# 2740
 goto _LL321;_LL321:;}
# 2743
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp687);goto _LL2CB;}_LL304: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp688=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp688->tag != 35)goto _LL306;else{_tmp689=_tmp688->f1;}}_LL305:
# 2745
 t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp689);goto _LL2CB;_LL306: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp68A=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp68A->tag != 26)goto _LL308;else{_tmp68B=_tmp68A->f1;_tmp68C=_tmp68A->f2;_tmp68D=_tmp68A->f3;_tmp68E=_tmp68A->f4;_tmp68F=(int*)& _tmp68A->f4;}}_LL307:
# 2747
 t=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp68B,_tmp68C,_tmp68D,_tmp68F);goto _LL2CB;_LL308: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp690=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp690->tag != 27)goto _LL30A;else{_tmp691=_tmp690->f1;_tmp692=(struct _tuple1**)& _tmp690->f1;_tmp693=_tmp690->f2;_tmp694=(struct Cyc_List_List**)& _tmp690->f2;_tmp695=_tmp690->f3;_tmp696=_tmp690->f4;_tmp697=(struct Cyc_Absyn_Aggrdecl**)& _tmp690->f4;}}_LL309:
# 2749
 t=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp692,_tmp694,_tmp695,_tmp697);goto _LL2CB;_LL30A: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp698=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp698->tag != 28)goto _LL30C;else{_tmp699=(void*)_tmp698->f1;_tmp69A=_tmp698->f2;}}_LL30B:
# 2751
 t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp699,_tmp69A);goto _LL2CB;_LL30C: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp69B=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp69B->tag != 29)goto _LL30E;else{_tmp69C=_tmp69B->f1;_tmp69D=_tmp69B->f2;_tmp69E=_tmp69B->f3;}}_LL30D:
# 2753
 t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp69C,_tmp69D,_tmp69E);goto _LL2CB;_LL30E: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp69F=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp69F->tag != 30)goto _LL310;else{_tmp6A0=_tmp69F->f1;_tmp6A1=(struct _tuple1**)& _tmp69F->f1;_tmp6A2=_tmp69F->f2;_tmp6A3=_tmp69F->f3;}}_LL30F:
# 2755
*_tmp6A1=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp6A3))->name;
# 2756
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpA3C;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpA3B;t=(void*)((_tmpA3B=_cycalloc(sizeof(*_tmpA3B)),((_tmpA3B[0]=((_tmpA3C.tag=13,((_tmpA3C.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp6A2))->name,((_tmpA3C.f2=_tmp6A2,_tmpA3C)))))),_tmpA3B))));}goto _LL2CB;_LL310: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6A4=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp6A4->tag != 31)goto _LL312;else{_tmp6A5=_tmp6A4->f1;_tmp6A6=(struct _tuple1**)& _tmp6A4->f1;_tmp6A7=(void*)_tmp6A4->f2;_tmp6A8=_tmp6A4->f3;}}_LL311:
# 2758
*_tmp6A6=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp6A8))->name;
# 2759
t=_tmp6A7;goto _LL2CB;_LL312: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp6A9=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp6A9->tag != 32)goto _LL314;else{_tmp6AA=_tmp6A9->f1;_tmp6AB=_tmp6AA.is_calloc;_tmp6AC=(int*)&(_tmp6A9->f1).is_calloc;_tmp6AD=_tmp6AA.rgn;_tmp6AE=(struct Cyc_Absyn_Exp**)&(_tmp6A9->f1).rgn;_tmp6AF=_tmp6AA.elt_type;_tmp6B0=(void***)&(_tmp6A9->f1).elt_type;_tmp6B1=_tmp6AA.num_elts;_tmp6B2=(struct Cyc_Absyn_Exp**)&(_tmp6A9->f1).num_elts;_tmp6B3=_tmp6AA.fat_result;_tmp6B4=(int*)&(_tmp6A9->f1).fat_result;}}_LL313:
# 2761
 t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp6AE,_tmp6B0,_tmp6B2,_tmp6AC,_tmp6B4);goto _LL2CB;_LL314: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp6B5=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp6B5->tag != 33)goto _LL316;else{_tmp6B6=_tmp6B5->f1;_tmp6B7=_tmp6B5->f2;}}_LL315:
# 2763
 t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp6B6,_tmp6B7);goto _LL2CB;_LL316: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp6B8=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp6B8->tag != 36)goto _LL318;else{_tmp6B9=_tmp6B8->f1;_tmp6BA=_tmp6B8->f2;}}_LL317:
# 2765
 t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp6B9,_tmp6BA);goto _LL2CB;_LL318: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp6BB=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp6BB->tag != 37)goto _LL31A;else{_tmp6BC=(void*)_tmp6BB->f1;}}_LL319:
# 2767
 if(!te->allow_valueof){
# 2768
const char*_tmpA3F;void*_tmpA3E;(_tmpA3E=0,Cyc_Tcutil_terr(e->loc,((_tmpA3F="valueof(-) can only occur within types",_tag_dyneither(_tmpA3F,sizeof(char),39))),_tag_dyneither(_tmpA3E,sizeof(void*),0)));}
# 2775
t=Cyc_Absyn_sint_typ;
# 2776
goto _LL2CB;_LL31A: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp6BD=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp6BD->tag != 38)goto _LL2CB;}_LL31B:
# 2778
{const char*_tmpA42;void*_tmpA41;(_tmpA41=0,Cyc_Tcutil_terr(e->loc,((_tmpA42="asm expressions cannot occur within Cyclone code.",_tag_dyneither(_tmpA42,sizeof(char),50))),_tag_dyneither(_tmpA41,sizeof(void*),0)));}
# 2779
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));_LL2CB:;}
# 2781
e->topt=(void*)t;}
