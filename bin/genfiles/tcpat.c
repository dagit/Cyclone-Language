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
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{
char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 123 "core.h"
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17];
struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{
char*tag;struct _dyneither_ptr f1;};
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
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct
Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{
char*tag;};
# 163
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*x);
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 178
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,
struct Cyc_List_List*y);
# 183
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 259
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y);
# 364
struct Cyc_List_List*Cyc_List_rtabulate_c(struct _RegionHandle*r,int n,void*(*f)(
void*,int),void*env);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);
};
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
char*tag;};struct Cyc_Set_Set;
# 40 "/home/jgm/cyclone/include/set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 54 "/home/jgm/cyclone/include/set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 69
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,
void*elt);
# 94
int Cyc_Set_cardinality(struct Cyc_Set_Set*s);
# 97
int Cyc_Set_is_empty(struct Cyc_Set_Set*s);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];
struct Cyc_Set_Absent_exn_struct{char*tag;};
# 137
void*Cyc_Set_choose(struct Cyc_Set_Set*s);
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
_tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 322
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);
struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;
struct _tuple1*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 340
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,
struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU
field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;
struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int
tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**
val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;
struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 353
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct
Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};
struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*
num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;};
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
struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{
int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;
void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{
int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 433 "/home/jgm/cyclone/src/absyn.h"
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct
Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;
unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct
Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*
f5;};
# 444
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
struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;
struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};
struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;
struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct
_union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;
long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct
_tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct
_tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct
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
f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;
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
int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct
_tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;
int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};
struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{
int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;
};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*
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
struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{
int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*
f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int
tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{
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
int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int
tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{
int tag;struct Cyc_Absyn_Exp*f1;};
# 719 "/home/jgm/cyclone/src/absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{
void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;
};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual
tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual
ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;
struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual
tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;
struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind
kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct
_tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};
struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{
struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{
enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
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
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{
int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{
void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{
int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{
char*tag;};
# 912
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 914
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 918
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 924
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 927
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
# 928
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 929
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 932
void*Cyc_Absyn_compress_kb(void*);
# 937
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 939
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 942
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 944
extern void*Cyc_Absyn_sint_typ;
# 946
void*Cyc_Absyn_float_typ(int);
# 948
extern void*Cyc_Absyn_empty_effect;
# 1092
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1101
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*
init);struct Cyc_PP_Ppstate;
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
# 64 "/home/jgm/cyclone/src/absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 66
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 67
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);
# 68
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_RgnOrder_RgnPO;
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
# 122 "/home/jgm/cyclone/src/tcenv.h"
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
unsigned int,struct _tuple1*);
# 123
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,
unsigned int,struct _tuple1*);
# 136
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 137
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 138
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 195
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 38 "/home/jgm/cyclone/src/tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 58
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 63
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 65
void*Cyc_Tcutil_compress(void*t);
# 74
int Cyc_Tcutil_coerceable(void*);
# 109
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 116
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 117
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
# 119
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
# 123
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 150
int Cyc_Tcutil_unify(void*,void*);
# 155
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct _tuple11{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple12{
struct Cyc_Absyn_Tvar*f1;void*f2;};
# 169
struct _tuple12*Cyc_Tcutil_r_make_inst_var(struct _tuple11*,struct Cyc_Absyn_Tvar*);
# 217 "/home/jgm/cyclone/src/tcutil.h"
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct
_dyneither_ptr err_msg);
# 232
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct
_RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,
struct Cyc_List_List*fields);
# 260
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 270
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 290
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 309
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 338
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 32 "/home/jgm/cyclone/src/tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct
Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
# 40
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr msg_part);struct _tuple13{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple13*tvars_and_bounds_opt;struct Cyc_List_List*
patvars;};
# 38 "/home/jgm/cyclone/src/tcpat.h"
typedef struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcpat_result_t;
# 42
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*
p,void**topt);
# 43
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p);
# 44
void Cyc_Tcpat_check_switch_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct
Cyc_List_List*);
# 45
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Pat*p);
# 46
void Cyc_Tcpat_check_catch_overlap(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
struct _tuple14{unsigned int f1;int f2;};
# 28 "/home/jgm/cyclone/src/evexp.h"
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct
_tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 52 "/home/jgm/cyclone/src/tcpat.cyc"
static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Pat*
p){
# 53
void*_tmp0=p->r;struct _tuple1*_tmp2;struct _tuple1*_tmp4;struct Cyc_List_List*
_tmp5;int _tmp6;struct Cyc_Absyn_AggrInfo*_tmp8;struct Cyc_List_List*_tmp9;struct
Cyc_List_List*_tmpA;int _tmpB;struct Cyc_Absyn_AggrInfo*_tmpD;struct Cyc_Absyn_AggrInfo
_tmpE;union Cyc_Absyn_AggrInfoU _tmpF;struct _tuple3 _tmp10;enum Cyc_Absyn_AggrKind
_tmp11;struct _tuple1*_tmp12;struct Cyc_List_List*_tmp13;struct Cyc_List_List*
_tmp14;struct Cyc_List_List**_tmp15;struct Cyc_List_List*_tmp16;int _tmp17;struct
Cyc_Absyn_Exp*_tmp19;_LL1: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*
_tmp1=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp1->tag != 
14)goto _LL3;else{_tmp2=_tmp1->f1;}}_LL2:
# 55
{struct _handler_cons _tmp1A;_push_handler(& _tmp1A);{int _tmp1C=0;if(setjmp(_tmp1A.handler))
_tmp1C=1;if(!_tmp1C){
# 56
{struct _RegionHandle _tmp1D=_new_region("r");struct _RegionHandle*r=& _tmp1D;
_push_region(r);
# 57
{void*_tmp1E=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp2);struct Cyc_Absyn_Datatypedecl*
_tmp21;struct Cyc_Absyn_Datatypefield*_tmp22;struct Cyc_Absyn_Enumdecl*_tmp24;
struct Cyc_Absyn_Enumfield*_tmp25;void*_tmp27;struct Cyc_Absyn_Enumfield*_tmp28;
_LLE: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp1F=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)
_tmp1E;if(_tmp1F->tag != 1)goto _LL10;}_LLF:
# 59
 p->r=(void*)& Cyc_Absyn_Wild_p_val;
# 60
{const char*_tmp3BB;void*_tmp3BA;(_tmp3BA=0,Cyc_Tcutil_terr(p->loc,((_tmp3BB="struct tag used without arguments in pattern",
_tag_dyneither(_tmp3BB,sizeof(char),45))),_tag_dyneither(_tmp3BA,sizeof(void*),0)));}
# 61
_npop_handler(1);return;_LL10: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*
_tmp20=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp1E;if(_tmp20->tag
!= 2)goto _LL12;else{_tmp21=_tmp20->f1;_tmp22=_tmp20->f2;}}_LL11:
# 63
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp3BE;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*
_tmp3BD;p->r=(void*)((_tmp3BD=_cycalloc(sizeof(*_tmp3BD)),((_tmp3BD[0]=((_tmp3BE.tag=
7,((_tmp3BE.f1=_tmp21,((_tmp3BE.f2=_tmp22,((_tmp3BE.f3=0,((_tmp3BE.f4=0,_tmp3BE)))))))))),
_tmp3BD))));}_npop_handler(1);return;_LL12: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*
_tmp23=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp1E;if(_tmp23->tag != 3)
goto _LL14;else{_tmp24=_tmp23->f1;_tmp25=_tmp23->f2;}}_LL13:
# 64
{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct _tmp3C1;struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*
_tmp3C0;p->r=(void*)((_tmp3C0=_cycalloc(sizeof(*_tmp3C0)),((_tmp3C0[0]=((_tmp3C1.tag=
12,((_tmp3C1.f1=_tmp24,((_tmp3C1.f2=_tmp25,_tmp3C1)))))),_tmp3C0))));}
_npop_handler(1);return;_LL14: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*
_tmp26=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp1E;if(_tmp26->tag
!= 4)goto _LL16;else{_tmp27=(void*)_tmp26->f1;_tmp28=_tmp26->f2;}}_LL15:
# 65
{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct _tmp3C4;struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*
_tmp3C3;p->r=(void*)((_tmp3C3=_cycalloc(sizeof(*_tmp3C3)),((_tmp3C3[0]=((_tmp3C4.tag=
13,((_tmp3C4.f1=(void*)_tmp27,((_tmp3C4.f2=_tmp28,_tmp3C4)))))),_tmp3C3))));}
_npop_handler(1);return;_LL16: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*
_tmp29=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp1E;if(_tmp29->tag != 0)
goto _LLD;}_LL17:
# 66
 goto _LLD;_LLD:;}
# 57
;_pop_region(r);}
# 56
;_pop_handler();}else{void*_tmp1B=(void*)_exn_thrown;void*_tmp33=_tmp1B;_LL19: {
struct Cyc_Dict_Absent_exn_struct*_tmp34=(struct Cyc_Dict_Absent_exn_struct*)
_tmp33;if(_tmp34->tag != Cyc_Dict_Absent)goto _LL1B;}_LL1A:
# 69
 goto _LL18;_LL1B:;_LL1C:(void)_throw(_tmp33);_LL18:;}};}{
# 71
union Cyc_Absyn_Nmspace _tmp35=(*_tmp2).f1;int _tmp36;struct Cyc_List_List*_tmp37;
_LL1E: if((_tmp35.Loc_n).tag != 4)goto _LL20;_tmp36=(int)(_tmp35.Loc_n).val;_LL1F:
# 72
 goto _LL21;_LL20: if((_tmp35.Rel_n).tag != 1)goto _LL22;_tmp37=(struct Cyc_List_List*)(
_tmp35.Rel_n).val;if(_tmp37 != 0)goto _LL22;_LL21:
# 74
(*_tmp2).f1=Cyc_Absyn_Loc_n;
# 75
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp3C7;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*
_tmp3C6;p->r=(void*)((_tmp3C6=_cycalloc(sizeof(*_tmp3C6)),((_tmp3C6[0]=((_tmp3C7.tag=
1,((_tmp3C7.f1=Cyc_Absyn_new_vardecl(_tmp2,(void*)& Cyc_Absyn_VoidType_val,0),((
_tmp3C7.f2=
# 76
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0),_tmp3C7)))))),_tmp3C6))));}
# 77
return;_LL22:;_LL23:
# 78
{const char*_tmp3CA;void*_tmp3C9;(_tmp3C9=0,Cyc_Tcutil_terr(p->loc,((_tmp3CA="qualified variable in pattern",
_tag_dyneither(_tmp3CA,sizeof(char),30))),_tag_dyneither(_tmp3C9,sizeof(void*),0)));}
return;_LL1D:;};_LL3: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp3=(
struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp3->tag != 15)
goto _LL5;else{_tmp4=_tmp3->f1;_tmp5=_tmp3->f2;_tmp6=_tmp3->f3;}}_LL4:
# 81
{struct _handler_cons _tmp3C;_push_handler(& _tmp3C);{int _tmp3E=0;if(setjmp(_tmp3C.handler))
_tmp3E=1;if(!_tmp3E){
# 82
{struct _RegionHandle _tmp3F=_new_region("r");struct _RegionHandle*r=& _tmp3F;
_push_region(r);
# 83
{void*_tmp40=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp4);struct Cyc_Absyn_Aggrdecl*
_tmp42;struct Cyc_Absyn_Datatypedecl*_tmp44;struct Cyc_Absyn_Datatypefield*_tmp45;
_LL25: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp41=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)
_tmp40;if(_tmp41->tag != 1)goto _LL27;else{_tmp42=_tmp41->f1;}}_LL26: {
# 85
struct Cyc_List_List*_tmp49=0;
# 86
for(0;_tmp5 != 0;_tmp5=_tmp5->tl){
# 87
struct _tuple15*_tmp3CD;struct Cyc_List_List*_tmp3CC;_tmp49=((_tmp3CC=_cycalloc(
sizeof(*_tmp3CC)),((_tmp3CC->hd=((_tmp3CD=_cycalloc(sizeof(*_tmp3CD)),((_tmp3CD->f1=
0,((_tmp3CD->f2=(struct Cyc_Absyn_Pat*)_tmp5->hd,_tmp3CD)))))),((_tmp3CC->tl=
_tmp49,_tmp3CC))))));}
# 88
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp3D7;struct Cyc_Absyn_Aggrdecl**
_tmp3D6;struct Cyc_Absyn_AggrInfo*_tmp3D5;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*
_tmp3D4;p->r=(void*)((_tmp3D4=_cycalloc(sizeof(*_tmp3D4)),((_tmp3D4[0]=((_tmp3D7.tag=
6,((_tmp3D7.f1=((_tmp3D5=_cycalloc(sizeof(*_tmp3D5)),((_tmp3D5->aggr_info=Cyc_Absyn_KnownAggr(((
_tmp3D6=_cycalloc(sizeof(*_tmp3D6)),((_tmp3D6[0]=_tmp42,_tmp3D6))))),((_tmp3D5->targs=
0,_tmp3D5)))))),((_tmp3D7.f2=0,((_tmp3D7.f3=
# 89
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp49),((
_tmp3D7.f4=_tmp6,_tmp3D7)))))))))),_tmp3D4))));}
# 90
_npop_handler(1);return;}_LL27: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*
_tmp43=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp40;if(_tmp43->tag
!= 2)goto _LL29;else{_tmp44=_tmp43->f1;_tmp45=_tmp43->f2;}}_LL28:
# 91
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp3DA;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*
_tmp3D9;p->r=(void*)((_tmp3D9=_cycalloc(sizeof(*_tmp3D9)),((_tmp3D9[0]=((_tmp3DA.tag=
7,((_tmp3DA.f1=_tmp44,((_tmp3DA.f2=_tmp45,((_tmp3DA.f3=_tmp5,((_tmp3DA.f4=_tmp6,
_tmp3DA)))))))))),_tmp3D9))));}_npop_handler(1);return;_LL29: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*
_tmp46=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp40;if(_tmp46->tag != 3)
goto _LL2B;}_LL2A:
# 92
 goto _LL2C;_LL2B: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp47=(
struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp40;if(_tmp47->tag != 4)
goto _LL2D;}_LL2C:
# 94
{const char*_tmp3DD;void*_tmp3DC;(_tmp3DC=0,Cyc_Tcutil_terr(p->loc,((_tmp3DD="enum tag used with arguments in pattern",
_tag_dyneither(_tmp3DD,sizeof(char),40))),_tag_dyneither(_tmp3DC,sizeof(void*),0)));}
# 95
p->r=(void*)& Cyc_Absyn_Wild_p_val;
# 96
_npop_handler(1);return;_LL2D: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*
_tmp48=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp40;if(_tmp48->tag != 0)
goto _LL24;}_LL2E:
# 97
 goto _LL24;_LL24:;}
# 83
;_pop_region(r);}
# 82
;_pop_handler();}else{void*_tmp3D=(void*)_exn_thrown;void*_tmp55=_tmp3D;_LL30: {
struct Cyc_Dict_Absent_exn_struct*_tmp56=(struct Cyc_Dict_Absent_exn_struct*)
_tmp55;if(_tmp56->tag != Cyc_Dict_Absent)goto _LL32;}_LL31:
# 99
 goto _LL2F;_LL32:;_LL33:(void)_throw(_tmp55);_LL2F:;}};}
# 100
{const char*_tmp3E1;void*_tmp3E0[1];struct Cyc_String_pa_PrintArg_struct _tmp3DF;(
_tmp3DF.tag=0,((_tmp3DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp4)),((_tmp3E0[0]=& _tmp3DF,Cyc_Tcutil_terr(p->loc,((_tmp3E1="%s is not a constructor in pattern",
_tag_dyneither(_tmp3E1,sizeof(char),35))),_tag_dyneither(_tmp3E0,sizeof(void*),1)))))));}
# 101
p->r=(void*)& Cyc_Absyn_Wild_p_val;
# 102
return;_LL5: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)
_tmp0;if(_tmp7->tag != 6)goto _LL7;else{_tmp8=_tmp7->f1;if(_tmp8 != 0)goto _LL7;
_tmp9=_tmp7->f2;_tmpA=_tmp7->f3;_tmpB=_tmp7->f4;}}_LL6:
# 105
 if(topt == 0){
# 106
const char*_tmp3E4;void*_tmp3E3;(_tmp3E3=0,Cyc_Tcutil_terr(p->loc,((_tmp3E4="cannot determine pattern type",
_tag_dyneither(_tmp3E4,sizeof(char),30))),_tag_dyneither(_tmp3E3,sizeof(void*),0)));}{
# 107
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));
# 108
{void*_tmp5C=t;struct Cyc_Absyn_AggrInfo _tmp5E;_LL35: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp5D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5C;if(_tmp5D->tag != 11)
goto _LL37;else{_tmp5E=_tmp5D->f1;}}_LL36:
# 110
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp3EA;struct Cyc_Absyn_AggrInfo*
_tmp3E9;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp3E8;p->r=(void*)((
_tmp3E8=_cycalloc(sizeof(*_tmp3E8)),((_tmp3E8[0]=((_tmp3EA.tag=6,((_tmp3EA.f1=((
_tmp3E9=_cycalloc(sizeof(*_tmp3E9)),((_tmp3E9[0]=_tmp5E,_tmp3E9)))),((_tmp3EA.f2=
_tmp9,((_tmp3EA.f3=_tmpA,((_tmp3EA.f4=_tmpB,_tmp3EA)))))))))),_tmp3E8))));}
# 111
Cyc_Tcpat_resolve_pat(te,topt,p);
# 112
goto _LL34;_LL37:;_LL38:
# 114
{const char*_tmp3EE;void*_tmp3ED[1];struct Cyc_String_pa_PrintArg_struct _tmp3EC;(
_tmp3EC.tag=0,((_tmp3EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 115
Cyc_Absynpp_typ2string(t)),((_tmp3ED[0]=& _tmp3EC,Cyc_Tcutil_terr(p->loc,((
_tmp3EE="pattern expects aggregate type instead of %s",_tag_dyneither(_tmp3EE,
sizeof(char),45))),_tag_dyneither(_tmp3ED,sizeof(void*),1)))))));}
# 117
goto _LL34;_LL34:;}
# 119
return;};_LL7: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpC=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)
_tmp0;if(_tmpC->tag != 6)goto _LL9;else{_tmpD=_tmpC->f1;if(_tmpD == 0)goto _LL9;
_tmpE=*_tmpD;_tmpF=_tmpE.aggr_info;if((_tmpF.UnknownAggr).tag != 1)goto _LL9;
_tmp10=(struct _tuple3)(_tmpF.UnknownAggr).val;_tmp11=_tmp10.f1;_tmp12=_tmp10.f2;
_tmp13=_tmpE.targs;_tmp14=_tmpC->f2;_tmp15=(struct Cyc_List_List**)& _tmpC->f2;
_tmp16=_tmpC->f3;_tmp17=_tmpC->f4;}}_LL8:
# 122
{struct _handler_cons _tmp65;_push_handler(& _tmp65);{int _tmp67=0;if(setjmp(_tmp65.handler))
_tmp67=1;if(!_tmp67){
# 123
{struct Cyc_Absyn_Aggrdecl**_tmp68=Cyc_Tcenv_lookup_aggrdecl(te,p->loc,_tmp12);
# 124
struct Cyc_Absyn_Aggrdecl*_tmp69=*_tmp68;
# 125
if(_tmp69->impl == 0){
# 126
{const char*_tmp3F6;void*_tmp3F5[1];const char*_tmp3F4;const char*_tmp3F3;struct
Cyc_String_pa_PrintArg_struct _tmp3F2;(_tmp3F2.tag=0,((_tmp3F2.f1=(struct
_dyneither_ptr)(
# 127
_tmp69->kind == Cyc_Absyn_StructA?(_tmp3F3="struct",_tag_dyneither(_tmp3F3,
sizeof(char),7)):((_tmp3F4="union",_tag_dyneither(_tmp3F4,sizeof(char),6)))),((
_tmp3F5[0]=& _tmp3F2,Cyc_Tcutil_terr(p->loc,((_tmp3F6="can't destructure an abstract %s",
_tag_dyneither(_tmp3F6,sizeof(char),33))),_tag_dyneither(_tmp3F5,sizeof(void*),1)))))));}
# 128
p->r=(void*)& Cyc_Absyn_Wild_p_val;
# 129
_npop_handler(0);return;}{
# 131
int more_exists=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp69->impl))->exist_vars)- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*
_tmp15);
# 132
if(more_exists < 0){
# 133
{const char*_tmp3F9;void*_tmp3F8;(_tmp3F8=0,Cyc_Tcutil_terr(p->loc,((_tmp3F9="too many existentially bound type variables in pattern",
_tag_dyneither(_tmp3F9,sizeof(char),55))),_tag_dyneither(_tmp3F8,sizeof(void*),0)));}{
# 134
struct Cyc_List_List**_tmp71=_tmp15;
# 135
{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp69->impl))->exist_vars);for(0;n != 0;-- n){
# 136
_tmp71=&((struct Cyc_List_List*)_check_null(*_tmp71))->tl;}}
# 137
*_tmp71=0;};}else{
# 138
if(more_exists > 0){
# 140
struct Cyc_List_List*_tmp72=0;
# 141
for(0;more_exists != 0;-- more_exists){
# 142
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp3FF;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct
_tmp3FE;struct Cyc_List_List*_tmp3FD;_tmp72=((_tmp3FD=_cycalloc(sizeof(*_tmp3FD)),((
_tmp3FD->hd=Cyc_Tcutil_new_tvar((void*)((_tmp3FF=_cycalloc(sizeof(*_tmp3FF)),((
_tmp3FF[0]=((_tmp3FE.tag=1,((_tmp3FE.f1=0,_tmp3FE)))),_tmp3FF))))),((_tmp3FD->tl=
_tmp72,_tmp3FD))))));}
# 143
*_tmp15=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(*_tmp15,_tmp72);}}{
# 145
struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp405;struct Cyc_Absyn_AggrInfo*
_tmp404;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp403;p->r=(void*)((
_tmp403=_cycalloc(sizeof(*_tmp403)),((_tmp403[0]=((_tmp405.tag=6,((_tmp405.f1=((
_tmp404=_cycalloc(sizeof(*_tmp404)),((_tmp404->aggr_info=Cyc_Absyn_KnownAggr(
_tmp68),((_tmp404->targs=_tmp13,_tmp404)))))),((_tmp405.f2=*_tmp15,((_tmp405.f3=
_tmp16,((_tmp405.f4=_tmp17,_tmp405)))))))))),_tmp403))));};};}
# 123
;_pop_handler();}else{void*_tmp66=(void*)_exn_thrown;void*_tmp7A=_tmp66;_LL3A: {
struct Cyc_Dict_Absent_exn_struct*_tmp7B=(struct Cyc_Dict_Absent_exn_struct*)
_tmp7A;if(_tmp7B->tag != Cyc_Dict_Absent)goto _LL3C;}_LL3B:
# 148
{const char*_tmp408;void*_tmp407;(_tmp407=0,Cyc_Tcutil_terr(p->loc,((_tmp408="Non-aggregate name has designator patterns",
_tag_dyneither(_tmp408,sizeof(char),43))),_tag_dyneither(_tmp407,sizeof(void*),0)));}
# 149
p->r=(void*)& Cyc_Absyn_Wild_p_val;
# 150
goto _LL39;_LL3C:;_LL3D:(void)_throw(_tmp7A);_LL39:;}};}
# 152
return;_LL9: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp18=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)
_tmp0;if(_tmp18->tag != 16)goto _LLB;else{_tmp19=_tmp18->f1;}}_LLA:
# 154
 Cyc_Tcexp_tcExp(te,0,_tmp19);
# 155
if(!Cyc_Tcutil_is_const_exp(te,_tmp19)){
# 156
{const char*_tmp40B;void*_tmp40A;(_tmp40A=0,Cyc_Tcutil_terr(p->loc,((_tmp40B="non-constant expression in case pattern",
_tag_dyneither(_tmp40B,sizeof(char),40))),_tag_dyneither(_tmp40A,sizeof(void*),0)));}
# 157
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
# 159
unsigned int _tmp81;int _tmp82;struct _tuple14 _tmp80=Cyc_Evexp_eval_const_uint_exp(
_tmp19);_tmp81=_tmp80.f1;_tmp82=_tmp80.f2;
# 160
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp40E;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*
_tmp40D;p->r=(void*)((_tmp40D=_cycalloc_atomic(sizeof(*_tmp40D)),((_tmp40D[0]=((
_tmp40E.tag=9,((_tmp40E.f1=Cyc_Absyn_None,((_tmp40E.f2=(int)_tmp81,_tmp40E)))))),
_tmp40D))));}
# 161
return;};_LLB:;_LLC:
# 162
 return;_LL0:;}
# 166
static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
# 167
return(*vd->name).f2;}
# 169
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
# 170
if(topt != 0)
# 171
return*topt;{
# 172
struct Cyc_Core_Opt*_tmp40F;return Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mko,((
_tmp40F=_cycalloc(sizeof(*_tmp40F)),((_tmp40F->v=s,_tmp40F)))));};}
# 174
static void*Cyc_Tcpat_num_type(void**topt,void*numt){
# 178
if(topt != 0  && Cyc_Tcutil_coerceable(*topt))
# 179
return*topt;
# 181
{void*_tmp86=Cyc_Tcutil_compress(numt);_LL3F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmp87=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp86;if(_tmp87->tag != 13)
goto _LL41;}_LL40:
# 182
 if(topt != 0)return*topt;goto _LL3E;_LL41:;_LL42:
# 183
 goto _LL3E;_LL3E:;}
# 185
return numt;}
# 187
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*vd,struct Cyc_List_List**
v_result_ptr,void*t){
# 188
vd->type=t;
# 189
vd->tq=Cyc_Absyn_empty_tqual(0);{
# 190
struct Cyc_List_List*_tmp410;*v_result_ptr=(struct Cyc_List_List*)((_tmp410=
_cycalloc(sizeof(*_tmp410)),((_tmp410->hd=vd,((_tmp410->tl=*v_result_ptr,_tmp410))))));};}
# 192
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult
res1,struct Cyc_Tcpat_TcPatResult res2){
# 194
struct _tuple13*_tmp8A;struct Cyc_List_List*_tmp8B;struct Cyc_Tcpat_TcPatResult
_tmp89=res1;_tmp8A=_tmp89.tvars_and_bounds_opt;_tmp8B=_tmp89.patvars;{
# 195
struct _tuple13*_tmp8D;struct Cyc_List_List*_tmp8E;struct Cyc_Tcpat_TcPatResult
_tmp8C=res2;_tmp8D=_tmp8C.tvars_and_bounds_opt;_tmp8E=_tmp8C.patvars;
# 196
if(_tmp8A != 0  || _tmp8D != 0){
# 197
if(_tmp8A == 0){
# 198
struct _tuple13*_tmp411;_tmp8A=((_tmp411=_cycalloc(sizeof(*_tmp411)),((_tmp411->f1=
0,((_tmp411->f2=0,_tmp411))))));}
# 199
if(_tmp8D == 0){
# 200
struct _tuple13*_tmp412;_tmp8D=((_tmp412=_cycalloc(sizeof(*_tmp412)),((_tmp412->f1=
0,((_tmp412->f2=0,_tmp412))))));}{
# 201
struct _tuple13*_tmp415;struct Cyc_Tcpat_TcPatResult _tmp414;return(_tmp414.tvars_and_bounds_opt=((
_tmp415=_cycalloc(sizeof(*_tmp415)),((_tmp415->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp8A).f1,(*
_tmp8D).f1),((_tmp415->f2=
# 202
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*
_tmp8A).f2,(*_tmp8D).f2),_tmp415)))))),((_tmp414.patvars=
# 203
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp8B,_tmp8E),_tmp414)));};}{
# 205
struct Cyc_Tcpat_TcPatResult _tmp416;return(_tmp416.tvars_and_bounds_opt=0,((
_tmp416.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp8B,_tmp8E),_tmp416)));};};}
# 208
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned int loc){
# 209
struct Cyc_Absyn_Pat*_tmp417;return(_tmp417=_cycalloc(sizeof(*_tmp417)),((
_tmp417->loc=loc,((_tmp417->topt=0,((_tmp417->r=(void*)& Cyc_Absyn_Wild_p_val,
_tmp417)))))));}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple17{
struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 212
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Pat*p,void**topt,void**rgn_opt){
# 213
Cyc_Tcpat_resolve_pat(te,topt,p);{
# 214
void*t;
# 215
struct Cyc_Tcpat_TcPatResult _tmp418;struct Cyc_Tcpat_TcPatResult res=(_tmp418.tvars_and_bounds_opt=
0,((_tmp418.patvars=0,_tmp418)));
# 218
{void*_tmp95=p->r;struct Cyc_Absyn_Vardecl*_tmp98;struct Cyc_Absyn_Pat*_tmp99;
struct Cyc_Absyn_Vardecl*_tmp9B;struct Cyc_Absyn_Pat*_tmp9C;struct Cyc_Absyn_Tvar*
_tmp9E;struct Cyc_Absyn_Vardecl*_tmp9F;enum Cyc_Absyn_Sign _tmpA1;enum Cyc_Absyn_Sign
_tmpA3;enum Cyc_Absyn_Sign _tmpA5;int _tmpA8;struct Cyc_Absyn_Enumdecl*_tmpAA;void*
_tmpAC;struct Cyc_Absyn_Pat*_tmpAF;struct Cyc_List_List*_tmpB1;struct Cyc_List_List**
_tmpB2;int _tmpB3;struct Cyc_Absyn_AggrInfo*_tmpB5;struct Cyc_Absyn_AggrInfo _tmpB6;
union Cyc_Absyn_AggrInfoU _tmpB7;struct Cyc_Absyn_Aggrdecl**_tmpB8;struct Cyc_Absyn_Aggrdecl*
_tmpB9;struct Cyc_List_List*_tmpBA;struct Cyc_List_List**_tmpBB;struct Cyc_List_List*
_tmpBC;struct Cyc_List_List*_tmpBD;struct Cyc_List_List**_tmpBE;int _tmpBF;struct
Cyc_Absyn_Datatypedecl*_tmpC1;struct Cyc_Absyn_Datatypefield*_tmpC2;struct Cyc_List_List*
_tmpC3;struct Cyc_List_List**_tmpC4;int _tmpC5;struct Cyc_Absyn_AggrInfo*_tmpC7;
struct Cyc_Absyn_AggrInfo*_tmpC9;struct Cyc_Absyn_AggrInfo _tmpCA;union Cyc_Absyn_AggrInfoU
_tmpCB;struct _tuple3 _tmpCC;_LL44: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*
_tmp96=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp95;if(_tmp96->tag != 0)
goto _LL46;}_LL45:
# 221
 t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);
# 222
goto _LL43;_LL46: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp97=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)
_tmp95;if(_tmp97->tag != 1)goto _LL48;else{_tmp98=_tmp97->f1;_tmp99=_tmp97->f2;}}
_LL47:
# 224
 res=Cyc_Tcpat_tcPatRec(te,_tmp99,topt,rgn_opt);
# 225
t=(void*)_check_null(_tmp99->topt);
# 226
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk)){
# 227
const char*_tmp41B;void*_tmp41A;(_tmp41A=0,Cyc_Tcutil_terr(p->loc,((_tmp41B="pattern would point to an abstract member",
_tag_dyneither(_tmp41B,sizeof(char),42))),_tag_dyneither(_tmp41A,sizeof(void*),0)));}
# 228
Cyc_Tcpat_set_vd(_tmp98,& res.patvars,t);
# 229
goto _LL43;_LL48: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp9A=(
struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp95;if(_tmp9A->tag != 2)goto
_LL4A;else{_tmp9B=_tmp9A->f1;_tmp9C=_tmp9A->f2;}}_LL49:
# 231
 res=Cyc_Tcpat_tcPatRec(te,_tmp9C,topt,rgn_opt);
# 232
t=(void*)_check_null(_tmp9C->topt);
# 233
if(rgn_opt == 0){
# 234
{const char*_tmp41E;void*_tmp41D;(_tmp41D=0,Cyc_Tcutil_terr(p->loc,((_tmp41E="* pattern would point into an unknown/unallowed region",
_tag_dyneither(_tmp41E,sizeof(char),55))),_tag_dyneither(_tmp41D,sizeof(void*),0)));}
# 235
goto _LL43;}else{
# 237
if(Cyc_Tcutil_is_noalias_pointer(t)){
# 238
const char*_tmp421;void*_tmp420;(_tmp420=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp421="* pattern cannot take the address of an alias-free path",
_tag_dyneither(_tmp421,sizeof(char),56))),_tag_dyneither(_tmp420,sizeof(void*),0)));}}
# 239
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp42B;struct Cyc_Absyn_PtrAtts
_tmp42A;struct Cyc_Absyn_PtrInfo _tmp429;struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp428;Cyc_Tcpat_set_vd(_tmp9B,& res.patvars,(void*)(
# 240
(_tmp428=_cycalloc(sizeof(*_tmp428)),((_tmp428[0]=((_tmp42B.tag=5,((_tmp42B.f1=((
_tmp429.elt_typ=t,((_tmp429.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp429.ptr_atts=(
# 241
(_tmp42A.rgn=*rgn_opt,((_tmp42A.nullable=Cyc_Absyn_false_conref,((_tmp42A.bounds=
# 242
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp42A.zero_term=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp42A.ptrloc=0,
_tmp42A)))))))))),_tmp429)))))),_tmp42B)))),_tmp428)))));}
# 244
goto _LL43;_LL4A: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp9D=(struct
Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp95;if(_tmp9D->tag != 3)goto _LL4C;
else{_tmp9E=_tmp9D->f1;_tmp9F=_tmp9D->f2;}}_LL4B:
# 249
 Cyc_Tcpat_set_vd(_tmp9F,& res.patvars,_tmp9F->type);{
# 253
struct _RegionHandle _tmpDA=_new_region("r2");struct _RegionHandle*r2=& _tmpDA;
_push_region(r2);{
# 254
struct Cyc_Absyn_Tvar*_tmp42C[1];Cyc_Tcenv_add_type_vars(r2,p->loc,te,((_tmp42C[
0]=_tmp9E,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp42C,sizeof(struct Cyc_Absyn_Tvar*),1)))));}
# 256
if(res.tvars_and_bounds_opt == 0){
# 257
struct _tuple13*_tmp42D;res.tvars_and_bounds_opt=((_tmp42D=_cycalloc(sizeof(*
_tmp42D)),((_tmp42D->f1=0,((_tmp42D->f2=0,_tmp42D))))));}
# 258
{struct Cyc_List_List*_tmp42E;(*res.tvars_and_bounds_opt).f1=(
# 259
(_tmp42E=_cycalloc(sizeof(*_tmp42E)),((_tmp42E->hd=_tmp9E,((_tmp42E->tl=(*res.tvars_and_bounds_opt).f1,
_tmp42E))))));}
# 260
t=Cyc_Absyn_uint_typ;
# 261
_npop_handler(0);goto _LL43;
# 253
;_pop_region(r2);};_LL4C: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpA0=(
struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp95;if(_tmpA0->tag != 9)goto _LL4E;
else{_tmpA1=_tmpA0->f1;if(_tmpA1 != Cyc_Absyn_Unsigned)goto _LL4E;}}_LL4D:
# 263
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);goto _LL43;_LL4E: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*
_tmpA2=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp95;if(_tmpA2->tag != 9)
goto _LL50;else{_tmpA3=_tmpA2->f1;if(_tmpA3 != Cyc_Absyn_None)goto _LL50;}}_LL4F:
# 264
 goto _LL51;_LL50: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpA4=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)
_tmp95;if(_tmpA4->tag != 9)goto _LL52;else{_tmpA5=_tmpA4->f1;if(_tmpA5 != Cyc_Absyn_Signed)
goto _LL52;}}_LL51:
# 265
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL43;_LL52: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*
_tmpA6=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp95;if(_tmpA6->tag != 10)
goto _LL54;}_LL53:
# 266
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);goto _LL43;_LL54: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*
_tmpA7=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp95;if(_tmpA7->tag != 11)
goto _LL56;else{_tmpA8=_tmpA7->f2;}}_LL55:
# 267
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_float_typ(_tmpA8));goto _LL43;_LL56: {struct
Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmpA9=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)
_tmp95;if(_tmpA9->tag != 12)goto _LL58;else{_tmpAA=_tmpA9->f1;}}_LL57:
# 268
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp431;struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmp430;t=Cyc_Tcpat_num_type(topt,(void*)((_tmp430=_cycalloc(sizeof(*_tmp430)),((
_tmp430[0]=((_tmp431.tag=13,((_tmp431.f1=_tmpAA->name,((_tmp431.f2=(struct Cyc_Absyn_Enumdecl*)
_tmpAA,_tmp431)))))),_tmp430)))));}goto _LL43;_LL58: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*
_tmpAB=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp95;if(_tmpAB->tag != 
13)goto _LL5A;else{_tmpAC=(void*)_tmpAB->f1;}}_LL59:
# 269
 t=Cyc_Tcpat_num_type(topt,_tmpAC);goto _LL43;_LL5A: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*
_tmpAD=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp95;if(_tmpAD->tag != 8)
goto _LL5C;}_LL5B:
# 271
 if(topt != 0){
# 272
void*_tmpE0=Cyc_Tcutil_compress(*topt);_LL6F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmpE1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE0;if(_tmpE1->tag != 5)
goto _LL71;}_LL70:
# 273
 t=*topt;goto tcpat_end;_LL71:;_LL72:
# 274
 goto _LL6E;_LL6E:;}{
# 276
struct Cyc_Core_Opt*_tmpE2=Cyc_Tcenv_lookup_opt_type_vars(te);
# 277
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp43B;struct Cyc_Absyn_PtrAtts
_tmp43A;struct Cyc_Absyn_PtrInfo _tmp439;struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp438;t=(void*)((_tmp438=_cycalloc(sizeof(*_tmp438)),((_tmp438[0]=((_tmp43B.tag=
5,((_tmp43B.f1=((_tmp439.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ako,
_tmpE2),((_tmp439.elt_tq=
# 278
Cyc_Absyn_empty_tqual(0),((_tmp439.ptr_atts=(
# 281
(_tmp43A.rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,_tmpE2),((
_tmp43A.nullable=Cyc_Absyn_true_conref,((_tmp43A.bounds=
# 283
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp43A.zero_term=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp43A.ptrloc=0,
_tmp43A)))))))))),_tmp439)))))),_tmp43B)))),_tmp438))));}
# 286
goto _LL43;};_LL5C: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpAE=(
struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp95;if(_tmpAE->tag != 5)goto
_LL5E;else{_tmpAF=_tmpAE->f1;}}_LL5D: {
# 292
void*inner_typ=(void*)& Cyc_Absyn_VoidType_val;
# 293
void**_tmpE7=0;
# 294
int elt_const=0;
# 295
if(topt != 0){
# 296
void*_tmpE8=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmpEA;void*
_tmpEB;struct Cyc_Absyn_Tqual _tmpEC;_LL74: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmpE9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE8;if(_tmpE9->tag != 5)
goto _LL76;else{_tmpEA=_tmpE9->f1;_tmpEB=_tmpEA.elt_typ;_tmpEC=_tmpEA.elt_tq;}}
_LL75:
# 298
 inner_typ=_tmpEB;
# 299
_tmpE7=(void**)& inner_typ;
# 300
elt_const=_tmpEC.real_const;
# 301
goto _LL73;_LL76:;_LL77:
# 302
 goto _LL73;_LL73:;}{
# 307
void*ptr_rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(
te));
# 308
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmpAF,_tmpE7,(void**)&
ptr_rgn));
# 313
{void*_tmpED=Cyc_Tcutil_compress((void*)_check_null(_tmpAF->topt));struct Cyc_Absyn_DatatypeFieldInfo
_tmpEF;union Cyc_Absyn_DatatypeFieldInfoU _tmpF0;struct _tuple2 _tmpF1;struct Cyc_Absyn_Datatypedecl*
_tmpF2;struct Cyc_Absyn_Datatypefield*_tmpF3;struct Cyc_List_List*_tmpF4;_LL79: {
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpEE=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)
_tmpED;if(_tmpEE->tag != 4)goto _LL7B;else{_tmpEF=_tmpEE->f1;_tmpF0=_tmpEF.field_info;
if((_tmpF0.KnownDatatypefield).tag != 2)goto _LL7B;_tmpF1=(struct _tuple2)(_tmpF0.KnownDatatypefield).val;
_tmpF2=_tmpF1.f1;_tmpF3=_tmpF1.f2;_tmpF4=_tmpEF.targs;}}_LL7A:
# 317
{void*_tmpF5=Cyc_Tcutil_compress(inner_typ);_LL7E: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*
_tmpF6=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpF5;if(_tmpF6->tag
!= 4)goto _LL80;}_LL7F:
# 319
 goto DONT_PROMOTE;_LL80:;_LL81:
# 320
 goto _LL7D;_LL7D:;}{
# 323
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp445;struct Cyc_Absyn_Datatypedecl**
_tmp444;struct Cyc_Absyn_DatatypeInfo _tmp443;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*
_tmp442;void*new_type=(void*)((_tmp442=_cycalloc(sizeof(*_tmp442)),((_tmp442[0]=((
_tmp445.tag=3,((_tmp445.f1=((_tmp443.datatype_info=Cyc_Absyn_KnownDatatype(((
_tmp444=_cycalloc(sizeof(*_tmp444)),((_tmp444[0]=_tmpF2,_tmp444))))),((_tmp443.targs=
_tmpF4,_tmp443)))),_tmp445)))),_tmp442))));
# 326
_tmpAF->topt=(void*)new_type;
# 327
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp44F;struct Cyc_Absyn_PtrAtts
_tmp44E;struct Cyc_Absyn_PtrInfo _tmp44D;struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp44C;t=(void*)((_tmp44C=_cycalloc(sizeof(*_tmp44C)),((_tmp44C[0]=((_tmp44F.tag=
5,((_tmp44F.f1=((_tmp44D.elt_typ=new_type,((_tmp44D.elt_tq=
# 328
elt_const?Cyc_Absyn_const_tqual(0):
# 329
 Cyc_Absyn_empty_tqual(0),((_tmp44D.ptr_atts=(
# 330
(_tmp44E.rgn=ptr_rgn,((_tmp44E.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp44E.bounds=Cyc_Absyn_bounds_one_conref,((_tmp44E.zero_term=Cyc_Absyn_false_conref,((
_tmp44E.ptrloc=0,_tmp44E)))))))))),_tmp44D)))))),_tmp44F)))),_tmp44C))));}
# 333
goto _LL78;};_LL7B:;_LL7C:
# 335
 DONT_PROMOTE: {
# 336
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp459;struct Cyc_Absyn_PtrAtts
_tmp458;struct Cyc_Absyn_PtrInfo _tmp457;struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp456;t=(void*)((_tmp456=_cycalloc(sizeof(*_tmp456)),((_tmp456[0]=((_tmp459.tag=
5,((_tmp459.f1=((_tmp457.elt_typ=(void*)_check_null(_tmpAF->topt),((_tmp457.elt_tq=
# 337
elt_const?Cyc_Absyn_const_tqual(0):
# 338
 Cyc_Absyn_empty_tqual(0),((_tmp457.ptr_atts=(
# 339
(_tmp458.rgn=ptr_rgn,((_tmp458.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp458.bounds=
# 340
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp458.zero_term=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp458.ptrloc=0,
_tmp458)))))))))),_tmp457)))))),_tmp459)))),_tmp456))));}_LL78:;}
# 343
goto _LL43;};}_LL5E: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpB0=(
struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp95;if(_tmpB0->tag != 4)goto
_LL60;else{_tmpB1=_tmpB0->f1;_tmpB2=(struct Cyc_List_List**)& _tmpB0->f1;_tmpB3=
_tmpB0->f2;}}_LL5F: {
# 346
struct Cyc_List_List*_tmp103=*_tmpB2;
# 347
struct Cyc_List_List*pat_ts=0;
# 348
struct Cyc_List_List*topt_ts=0;
# 349
if(topt != 0){
# 350
void*_tmp104=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp106;_LL83: {
struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp105=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)
_tmp104;if(_tmp105->tag != 10)goto _LL85;else{_tmp106=_tmp105->f1;}}_LL84:
# 352
 topt_ts=_tmp106;
# 353
if(_tmpB3){
# 355
int _tmp107=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp103);
# 356
int _tmp108=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp106);
# 357
if(_tmp107 < _tmp108){
# 358
struct Cyc_List_List*wild_ps=0;
# 359
{int i=0;for(0;i < _tmp108 - _tmp107;++ i){
# 360
struct Cyc_List_List*_tmp45A;wild_ps=((_tmp45A=_cycalloc(sizeof(*_tmp45A)),((
_tmp45A->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp45A->tl=wild_ps,_tmp45A))))));}}
# 361
*_tmpB2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp103,wild_ps);
# 362
_tmp103=*_tmpB2;}else{
# 363
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp103)== ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp106)){
# 364
const char*_tmp45D;void*_tmp45C;(_tmp45C=0,Cyc_Tcutil_warn(p->loc,((_tmp45D="unnecessary ... in tuple pattern",
_tag_dyneither(_tmp45D,sizeof(char),33))),_tag_dyneither(_tmp45C,sizeof(void*),0)));}}}
# 367
goto _LL82;_LL85:;_LL86:
# 369
 goto _LL82;_LL82:;}else{
# 371
if(_tmpB3){
# 372
const char*_tmp460;void*_tmp45F;(_tmp45F=0,Cyc_Tcutil_terr(p->loc,((_tmp460="cannot determine missing fields for ... in tuple pattern",
_tag_dyneither(_tmp460,sizeof(char),57))),_tag_dyneither(_tmp45F,sizeof(void*),0)));}}
# 373
for(0;_tmp103 != 0;_tmp103=_tmp103->tl){
# 374
void**_tmp10E=0;
# 375
if(topt_ts != 0){
# 376
_tmp10E=(void**)&(*((struct _tuple16*)topt_ts->hd)).f2;
# 377
topt_ts=topt_ts->tl;}
# 379
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)
_tmp103->hd,_tmp10E,rgn_opt));{
# 380
struct _tuple16*_tmp463;struct Cyc_List_List*_tmp462;pat_ts=((_tmp462=_cycalloc(
sizeof(*_tmp462)),((_tmp462->hd=((_tmp463=_cycalloc(sizeof(*_tmp463)),((_tmp463->f1=
Cyc_Absyn_empty_tqual(0),((_tmp463->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)
_tmp103->hd)->topt),_tmp463)))))),((_tmp462->tl=pat_ts,_tmp462))))));};}
# 382
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp466;struct Cyc_Absyn_TupleType_Absyn_Type_struct*
_tmp465;t=(void*)((_tmp465=_cycalloc(sizeof(*_tmp465)),((_tmp465[0]=((_tmp466.tag=
10,((_tmp466.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
pat_ts),_tmp466)))),_tmp465))));}
# 383
goto _LL43;}_LL60: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpB4=(struct
Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp95;if(_tmpB4->tag != 6)goto _LL62;else{
_tmpB5=_tmpB4->f1;if(_tmpB5 == 0)goto _LL62;_tmpB6=*_tmpB5;_tmpB7=_tmpB6.aggr_info;
if((_tmpB7.KnownAggr).tag != 2)goto _LL62;_tmpB8=(struct Cyc_Absyn_Aggrdecl**)(
_tmpB7.KnownAggr).val;_tmpB9=*_tmpB8;_tmpBA=_tmpB6.targs;_tmpBB=(struct Cyc_List_List**)&(*
_tmpB4->f1).targs;_tmpBC=_tmpB4->f2;_tmpBD=_tmpB4->f3;_tmpBE=(struct Cyc_List_List**)&
_tmpB4->f3;_tmpBF=_tmpB4->f4;}}_LL61: {
# 386
struct Cyc_List_List*_tmp113=*_tmpBE;
# 387
const char*_tmp468;const char*_tmp467;struct _dyneither_ptr aggr_str=_tmpB9->kind == 
Cyc_Absyn_StructA?(_tmp468="struct",_tag_dyneither(_tmp468,sizeof(char),7)):((
_tmp467="union",_tag_dyneither(_tmp467,sizeof(char),6)));
# 388
if(_tmpB9->impl == 0){
# 389
{const char*_tmp46C;void*_tmp46B[1];struct Cyc_String_pa_PrintArg_struct _tmp46A;(
_tmp46A.tag=0,((_tmp46A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((
_tmp46B[0]=& _tmp46A,Cyc_Tcutil_terr(p->loc,((_tmp46C="can't destructure an abstract %s",
_tag_dyneither(_tmp46C,sizeof(char),33))),_tag_dyneither(_tmp46B,sizeof(void*),1)))))));}
# 390
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
# 391
goto _LL43;}
# 394
if(_tmpBC != 0  || _tmpB9->kind == Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB9->impl))->tagged)
# 395
rgn_opt=0;{
# 396
struct _RegionHandle _tmp117=_new_region("rgn");struct _RegionHandle*rgn=& _tmp117;
_push_region(rgn);{
# 398
struct Cyc_List_List*_tmp118=0;
# 399
struct Cyc_List_List*outlives_constraints=0;
# 400
struct Cyc_List_List*_tmp119=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB9->impl))->exist_vars;
# 401
{struct Cyc_List_List*t=_tmpBC;for(0;t != 0;t=t->tl){
# 402
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
# 403
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)
_check_null(_tmp119))->hd;
# 404
_tmp119=_tmp119->tl;{
# 405
void*_tmp11A=Cyc_Absyn_compress_kb(tv->kind);
# 406
void*_tmp11B=Cyc_Absyn_compress_kb(uv->kind);
# 407
int error=0;
# 408
struct Cyc_Absyn_Kind*k2;
# 409
{void*_tmp11C=_tmp11B;struct Cyc_Absyn_Kind*_tmp11E;struct Cyc_Absyn_Kind*_tmp120;
_LL88: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp11D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)
_tmp11C;if(_tmp11D->tag != 2)goto _LL8A;else{_tmp11E=_tmp11D->f2;}}_LL89:
# 410
 _tmp120=_tmp11E;goto _LL8B;_LL8A: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*
_tmp11F=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp11C;if(_tmp11F->tag != 
0)goto _LL8C;else{_tmp120=_tmp11F->f1;}}_LL8B:
# 411
 k2=_tmp120;goto _LL87;_LL8C:;_LL8D: {
# 412
const char*_tmp46F;void*_tmp46E;(_tmp46E=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp46F="unconstrained existential type variable in aggregate",
_tag_dyneither(_tmp46F,sizeof(char),53))),_tag_dyneither(_tmp46E,sizeof(void*),0)));}
_LL87:;}
# 414
{void*_tmp123=_tmp11A;struct Cyc_Absyn_Kind*_tmp125;struct Cyc_Core_Opt*_tmp127;
struct Cyc_Core_Opt**_tmp128;struct Cyc_Absyn_Kind*_tmp129;struct Cyc_Core_Opt*
_tmp12B;struct Cyc_Core_Opt**_tmp12C;_LL8F: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*
_tmp124=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp123;if(_tmp124->tag != 
0)goto _LL91;else{_tmp125=_tmp124->f1;}}_LL90:
# 417
 if(!Cyc_Tcutil_kind_leq(k2,_tmp125))
# 418
error=1;
# 419
goto _LL8E;_LL91: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp126=(
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp123;if(_tmp126->tag != 2)goto
_LL93;else{_tmp127=_tmp126->f1;_tmp128=(struct Cyc_Core_Opt**)& _tmp126->f1;
_tmp129=_tmp126->f2;}}_LL92:
# 420
 _tmp12C=_tmp128;goto _LL94;_LL93: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*
_tmp12A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp123;if(_tmp12A->tag
!= 1)goto _LL8E;else{_tmp12B=_tmp12A->f1;_tmp12C=(struct Cyc_Core_Opt**)& _tmp12A->f1;}}
_LL94:
# 421
{struct Cyc_Core_Opt*_tmp470;*_tmp12C=((_tmp470=_cycalloc(sizeof(*_tmp470)),((
_tmp470->v=_tmp11B,_tmp470))));}goto _LL8E;_LL8E:;}
# 423
if(error){
# 424
const char*_tmp476;void*_tmp475[3];struct Cyc_String_pa_PrintArg_struct _tmp474;
struct Cyc_String_pa_PrintArg_struct _tmp473;struct Cyc_String_pa_PrintArg_struct
_tmp472;(_tmp472.tag=0,((_tmp472.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 427
Cyc_Absynpp_kind2string(k2)),((_tmp473.tag=0,((_tmp473.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(_tmp11A)),((_tmp474.tag=0,((
_tmp474.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp475[0]=&
_tmp474,((_tmp475[1]=& _tmp473,((_tmp475[2]=& _tmp472,Cyc_Tcutil_terr(p->loc,((
_tmp476="type variable %s has kind %s but must have at least kind %s",
_tag_dyneither(_tmp476,sizeof(char),60))),_tag_dyneither(_tmp475,sizeof(void*),3)))))))))))))))))));}{
# 429
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp479;struct Cyc_Absyn_VarType_Absyn_Type_struct*
_tmp478;void*vartype=(void*)((_tmp478=_cycalloc(sizeof(*_tmp478)),((_tmp478[0]=((
_tmp479.tag=2,((_tmp479.f1=tv,_tmp479)))),_tmp478))));
# 430
{struct Cyc_List_List*_tmp47A;_tmp118=((_tmp47A=_region_malloc(rgn,sizeof(*
_tmp47A)),((_tmp47A->hd=(void*)vartype,((_tmp47A->tl=_tmp118,_tmp47A))))));}
# 433
if(k2->kind == Cyc_Absyn_RgnKind){
# 434
if(k2->aliasqual == Cyc_Absyn_Aliasable){
# 435
struct _tuple0*_tmp47D;struct Cyc_List_List*_tmp47C;outlives_constraints=(
# 436
(_tmp47C=_cycalloc(sizeof(*_tmp47C)),((_tmp47C->hd=((_tmp47D=_cycalloc(sizeof(*
_tmp47D)),((_tmp47D->f1=Cyc_Absyn_empty_effect,((_tmp47D->f2=vartype,_tmp47D)))))),((
_tmp47C->tl=outlives_constraints,_tmp47C))))));}else{
# 438
const char*_tmp480;void*_tmp47F;(_tmp47F=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp480="opened existential had unique or top region kind",
_tag_dyneither(_tmp480,sizeof(char),49))),_tag_dyneither(_tmp47F,sizeof(void*),0)));}}};};}}
# 442
_tmp118=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp118);{
# 444
struct _RegionHandle _tmp13A=_new_region("r2");struct _RegionHandle*r2=& _tmp13A;
_push_region(r2);
# 445
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,_tmpBC);
# 447
struct Cyc_List_List*_tmp13B=Cyc_Tcenv_lookup_type_vars(te2);
# 448
struct _tuple11 _tmp481;struct _tuple11 _tmp13C=(_tmp481.f1=_tmp13B,((_tmp481.f2=
rgn,_tmp481)));
# 449
struct Cyc_List_List*_tmp13D=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),struct _tuple11*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp13C,
_tmpB9->tvs);
# 450
struct Cyc_List_List*_tmp13E=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB9->impl))->exist_vars,
_tmp118);
# 451
struct Cyc_List_List*_tmp13F=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple12*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,
_tmp13D);
# 452
struct Cyc_List_List*_tmp140=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple12*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,
_tmp13E);
# 453
struct Cyc_List_List*_tmp141=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp13D,
_tmp13E);
# 455
if(_tmpBC != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB9->impl))->rgn_po
!= 0){
# 456
if(res.tvars_and_bounds_opt == 0){
# 457
struct _tuple13*_tmp482;res.tvars_and_bounds_opt=((_tmp482=_cycalloc(sizeof(*
_tmp482)),((_tmp482->f1=0,((_tmp482->f2=0,_tmp482))))));}
# 458
(*res.tvars_and_bounds_opt).f1=
# 459
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*
res.tvars_and_bounds_opt).f1,_tmpBC);
# 460
(*res.tvars_and_bounds_opt).f2=
# 461
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*
res.tvars_and_bounds_opt).f2,outlives_constraints);{
# 462
struct Cyc_List_List*_tmp143=0;
# 463
{struct Cyc_List_List*_tmp144=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB9->impl))->rgn_po;
for(0;_tmp144 != 0;_tmp144=_tmp144->tl){
# 464
struct _tuple0*_tmp485;struct Cyc_List_List*_tmp484;_tmp143=((_tmp484=_cycalloc(
sizeof(*_tmp484)),((_tmp484->hd=((_tmp485=_cycalloc(sizeof(*_tmp485)),((_tmp485->f1=
Cyc_Tcutil_rsubstitute(rgn,_tmp141,(*((struct _tuple0*)_tmp144->hd)).f1),((
_tmp485->f2=
# 465
Cyc_Tcutil_rsubstitute(rgn,_tmp141,(*((struct _tuple0*)_tmp144->hd)).f2),_tmp485)))))),((
_tmp484->tl=_tmp143,_tmp484))))));}}
# 467
_tmp143=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp143);
# 468
(*res.tvars_and_bounds_opt).f2=
# 469
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*
res.tvars_and_bounds_opt).f2,_tmp143);};}
# 472
*_tmpBB=_tmp13F;
# 473
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp48F;struct Cyc_Absyn_Aggrdecl**
_tmp48E;struct Cyc_Absyn_AggrInfo _tmp48D;struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp48C;t=(void*)((_tmp48C=_cycalloc(sizeof(*_tmp48C)),((_tmp48C[0]=((_tmp48F.tag=
11,((_tmp48F.f1=((_tmp48D.aggr_info=Cyc_Absyn_KnownAggr(((_tmp48E=_cycalloc(
sizeof(*_tmp48E)),((_tmp48E[0]=_tmpB9,_tmp48E))))),((_tmp48D.targs=*_tmpBB,
_tmp48D)))),_tmp48F)))),_tmp48C))));}
# 474
if(_tmpBF  && _tmpB9->kind == Cyc_Absyn_UnionA){
# 475
const char*_tmp492;void*_tmp491;(_tmp491=0,Cyc_Tcutil_warn(p->loc,((_tmp492="`...' pattern not allowed in union pattern",
_tag_dyneither(_tmp492,sizeof(char),43))),_tag_dyneither(_tmp491,sizeof(void*),0)));}
else{
# 476
if(_tmpBF){
# 478
int _tmp14D=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp113);
# 479
int _tmp14E=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB9->impl))->fields);
# 480
if(_tmp14D < _tmp14E){
# 481
struct Cyc_List_List*wild_dps=0;
# 482
{int i=0;for(0;i < _tmp14E - _tmp14D;++ i){
# 483
struct _tuple15*_tmp495;struct Cyc_List_List*_tmp494;wild_dps=((_tmp494=_cycalloc(
sizeof(*_tmp494)),((_tmp494->hd=((_tmp495=_cycalloc(sizeof(*_tmp495)),((_tmp495->f1=
0,((_tmp495->f2=Cyc_Tcpat_wild_pat(p->loc),_tmp495)))))),((_tmp494->tl=wild_dps,
_tmp494))))));}}
# 484
*_tmpBE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp113,wild_dps);
# 485
_tmp113=*_tmpBE;}else{
# 486
if(_tmp14D == _tmp14E){
# 487
const char*_tmp498;void*_tmp497;(_tmp497=0,Cyc_Tcutil_warn(p->loc,((_tmp498="unnecessary ... in struct pattern",
_tag_dyneither(_tmp498,sizeof(char),34))),_tag_dyneither(_tmp497,sizeof(void*),0)));}}}}{
# 489
struct Cyc_List_List*fields=
# 490
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*
des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
rgn,p->loc,_tmp113,_tmpB9->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmpB9->impl))->fields);
# 491
for(0;fields != 0;fields=fields->tl){
# 492
struct _tuple17 _tmp154;struct Cyc_Absyn_Aggrfield*_tmp155;struct Cyc_Absyn_Pat*
_tmp156;struct _tuple17*_tmp153=(struct _tuple17*)fields->hd;_tmp154=*_tmp153;
_tmp155=_tmp154.f1;_tmp156=_tmp154.f2;{
# 493
void*_tmp157=Cyc_Tcutil_rsubstitute(rgn,_tmp141,_tmp155->type);
# 494
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te2,_tmp156,(void**)&
_tmp157,rgn_opt));
# 498
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp156->topt),_tmp157)){
# 499
const char*_tmp49F;void*_tmp49E[4];struct Cyc_String_pa_PrintArg_struct _tmp49D;
struct Cyc_String_pa_PrintArg_struct _tmp49C;struct Cyc_String_pa_PrintArg_struct
_tmp49B;struct Cyc_String_pa_PrintArg_struct _tmp49A;(_tmp49A.tag=0,((_tmp49A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)
# 501
Cyc_Absynpp_typ2string((void*)_check_null(_tmp156->topt))),((_tmp49B.tag=0,((
_tmp49B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 500
Cyc_Absynpp_typ2string(_tmp157)),((_tmp49C.tag=0,((_tmp49C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp49D.tag=0,((_tmp49D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp155->name),((_tmp49E[0]=&
_tmp49D,((_tmp49E[1]=& _tmp49C,((_tmp49E[2]=& _tmp49B,((_tmp49E[3]=& _tmp49A,Cyc_Tcutil_terr(
p->loc,((_tmp49F="field %s of %s pattern expects type %s != %s",_tag_dyneither(
_tmp49F,sizeof(char),45))),_tag_dyneither(_tmp49E,sizeof(void*),4)))))))))))))))))))))))));}};}};}
# 445
;_pop_region(r2);};}
# 505
_npop_handler(0);goto _LL43;
# 396
;_pop_region(rgn);};}_LL62: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*
_tmpC0=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp95;if(_tmpC0->tag != 
7)goto _LL64;else{_tmpC1=_tmpC0->f1;_tmpC2=_tmpC0->f2;_tmpC3=_tmpC0->f3;_tmpC4=(
struct Cyc_List_List**)& _tmpC0->f3;_tmpC5=_tmpC0->f4;}}_LL63: {
# 508
struct Cyc_List_List*_tmp161=*_tmpC4;
# 509
struct _RegionHandle _tmp162=_new_region("rgn");struct _RegionHandle*rgn=& _tmp162;
_push_region(rgn);{
# 510
struct Cyc_List_List*tqts=_tmpC2->typs;
# 512
struct Cyc_List_List*_tmp163=Cyc_Tcenv_lookup_type_vars(te);
# 513
struct _tuple11 _tmp4A0;struct _tuple11 _tmp164=(_tmp4A0.f1=_tmp163,((_tmp4A0.f2=
rgn,_tmp4A0)));
# 514
struct Cyc_List_List*_tmp165=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),struct _tuple11*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp164,
_tmpC1->tvs);
# 515
struct Cyc_List_List*_tmp166=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple12*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,
_tmp165);
# 516
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp4A6;struct Cyc_Absyn_DatatypeFieldInfo
_tmp4A5;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4A4;t=(void*)((
_tmp4A4=_cycalloc(sizeof(*_tmp4A4)),((_tmp4A4[0]=((_tmp4A6.tag=4,((_tmp4A6.f1=((
_tmp4A5.field_info=Cyc_Absyn_KnownDatatypefield(_tmpC1,_tmpC2),((_tmp4A5.targs=
_tmp166,_tmp4A5)))),_tmp4A6)))),_tmp4A4))));}
# 518
if(_tmpC5){
# 520
int _tmp16A=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp161);
# 521
int _tmp16B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);
# 522
if(_tmp16A < _tmp16B){
# 523
struct Cyc_List_List*wild_ps=0;
# 524
{int i=0;for(0;i < _tmp16B - _tmp16A;++ i){
# 525
struct Cyc_List_List*_tmp4A7;wild_ps=((_tmp4A7=_cycalloc(sizeof(*_tmp4A7)),((
_tmp4A7->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp4A7->tl=wild_ps,_tmp4A7))))));}}
# 526
*_tmpC4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp161,wild_ps);
# 527
_tmp161=*_tmpC4;}else{
# 528
if(_tmp16A == _tmp16B){
# 529
const char*_tmp4AB;void*_tmp4AA[1];struct Cyc_String_pa_PrintArg_struct _tmp4A9;(
_tmp4A9.tag=0,((_tmp4A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 530
Cyc_Absynpp_qvar2string(_tmpC1->name)),((_tmp4AA[0]=& _tmp4A9,Cyc_Tcutil_warn(p->loc,((
_tmp4AB="unnecessary ... in datatype field %s",_tag_dyneither(_tmp4AB,sizeof(
char),37))),_tag_dyneither(_tmp4AA,sizeof(void*),1)))))));}}}
# 532
for(0;_tmp161 != 0  && tqts != 0;(_tmp161=_tmp161->tl,tqts=tqts->tl)){
# 533
struct Cyc_Absyn_Pat*_tmp170=(struct Cyc_Absyn_Pat*)_tmp161->hd;
# 536
void*_tmp171=Cyc_Tcutil_rsubstitute(rgn,_tmp165,(*((struct _tuple16*)tqts->hd)).f2);
# 537
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmp170,(void**)&
_tmp171,rgn_opt));
# 541
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp170->topt),_tmp171)){
# 542
const char*_tmp4B1;void*_tmp4B0[3];struct Cyc_String_pa_PrintArg_struct _tmp4AF;
struct Cyc_String_pa_PrintArg_struct _tmp4AE;struct Cyc_String_pa_PrintArg_struct
_tmp4AD;(_tmp4AD.tag=0,((_tmp4AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 544
Cyc_Absynpp_typ2string((void*)_check_null(_tmp170->topt))),((_tmp4AE.tag=0,((
_tmp4AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 543
Cyc_Absynpp_typ2string(_tmp171)),((_tmp4AF.tag=0,((_tmp4AF.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpC2->name)),((
_tmp4B0[0]=& _tmp4AF,((_tmp4B0[1]=& _tmp4AE,((_tmp4B0[2]=& _tmp4AD,Cyc_Tcutil_terr(
_tmp170->loc,((_tmp4B1="%s expects argument type %s, not %s",_tag_dyneither(
_tmp4B1,sizeof(char),36))),_tag_dyneither(_tmp4B0,sizeof(void*),3)))))))))))))))))));}}
# 546
if(_tmp161 != 0){
# 547
const char*_tmp4B5;void*_tmp4B4[1];struct Cyc_String_pa_PrintArg_struct _tmp4B3;(
_tmp4B3.tag=0,((_tmp4B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 548
Cyc_Absynpp_qvar2string(_tmpC2->name)),((_tmp4B4[0]=& _tmp4B3,Cyc_Tcutil_terr(p->loc,((
_tmp4B5="too many arguments for datatype constructor %s",_tag_dyneither(_tmp4B5,
sizeof(char),47))),_tag_dyneither(_tmp4B4,sizeof(void*),1)))))));}
# 549
if(tqts != 0){
# 550
const char*_tmp4B9;void*_tmp4B8[1];struct Cyc_String_pa_PrintArg_struct _tmp4B7;(
_tmp4B7.tag=0,((_tmp4B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 551
Cyc_Absynpp_qvar2string(_tmpC2->name)),((_tmp4B8[0]=& _tmp4B7,Cyc_Tcutil_terr(p->loc,((
_tmp4B9="too few arguments for datatype constructor %s",_tag_dyneither(_tmp4B9,
sizeof(char),46))),_tag_dyneither(_tmp4B8,sizeof(void*),1)))))));}}
# 553
_npop_handler(0);goto _LL43;
# 509
;_pop_region(rgn);}_LL64: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpC6=(
struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp95;if(_tmpC6->tag != 6)goto _LL66;
else{_tmpC7=_tmpC6->f1;if(_tmpC7 != 0)goto _LL66;}}_LL65:
# 555
 goto _LL67;_LL66: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpC8=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)
_tmp95;if(_tmpC8->tag != 6)goto _LL68;else{_tmpC9=_tmpC8->f1;if(_tmpC9 == 0)goto
_LL68;_tmpCA=*_tmpC9;_tmpCB=_tmpCA.aggr_info;if((_tmpCB.UnknownAggr).tag != 1)
goto _LL68;_tmpCC=(struct _tuple3)(_tmpCB.UnknownAggr).val;}}_LL67:
# 556
 goto _LL69;_LL68: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpCD=(
struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp95;if(_tmpCD->tag != 14)
goto _LL6A;}_LL69:
# 557
 goto _LL6B;_LL6A: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmpCE=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)
_tmp95;if(_tmpCE->tag != 16)goto _LL6C;}_LL6B:
# 558
 goto _LL6D;_LL6C: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpCF=(
struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp95;if(_tmpCF->tag != 15)
goto _LL43;}_LL6D:
# 560
 t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL43;_LL43:;}
# 562
tcpat_end:
# 563
 p->topt=(void*)t;
# 564
return res;};}
# 567
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*
p,void**topt){
# 568
struct Cyc_Tcpat_TcPatResult _tmp17F=Cyc_Tcpat_tcPatRec(te,p,(void**)topt,0);
# 570
struct _RegionHandle _tmp180=_new_region("r");struct _RegionHandle*r=& _tmp180;
_push_region(r);{
# 571
const char*_tmp4BA;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*
x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp17F.patvars),p->loc,(
# 572
(_tmp4BA="pattern contains a repeated variable",_tag_dyneither(_tmp4BA,sizeof(
char),37))));}{
# 574
struct Cyc_Tcpat_TcPatResult _tmp182=_tmp17F;_npop_handler(0);return _tmp182;};
# 570
;_pop_region(r);}
# 577
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
# 578
void*_tmp183=p->r;struct Cyc_Absyn_Pat*_tmp185;struct Cyc_List_List*_tmp187;
struct Cyc_List_List*_tmp189;struct Cyc_List_List*_tmp18B;_LL96: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*
_tmp184=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp183;if(_tmp184->tag
!= 5)goto _LL98;else{_tmp185=_tmp184->f1;}}_LL97:
# 580
 Cyc_Tcpat_check_pat_regions(te,_tmp185);{
# 582
struct _RegionHandle _tmp18C=_new_region("temp");struct _RegionHandle*temp=&
_tmp18C;_push_region(temp);
# 583
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_check_null(_tmp185->topt))){
# 584
{const char*_tmp4BD;void*_tmp4BC;(_tmp4BC=0,Cyc_Tcutil_terr(p->loc,((_tmp4BD="Pattern dereferences to a non-aliased pointer; use swap",
_tag_dyneither(_tmp4BD,sizeof(char),56))),_tag_dyneither(_tmp4BC,sizeof(void*),0)));}
# 585
_npop_handler(0);return;}
# 589
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_check_null(p->topt))){
# 590
{const char*_tmp4C0;void*_tmp4BF;(_tmp4BF=0,Cyc_Tcutil_terr(p->loc,((_tmp4C0="Pattern dereferences a non-aliased pointer; use swap",
_tag_dyneither(_tmp4C0,sizeof(char),53))),_tag_dyneither(_tmp4BF,sizeof(void*),0)));}
# 591
_npop_handler(0);return;}{
# 594
void*_tmp191=(void*)_check_null(p->topt);struct Cyc_Absyn_PtrInfo _tmp193;struct
Cyc_Absyn_PtrAtts _tmp194;void*_tmp195;_LLA1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp192=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp191;if(_tmp192->tag
!= 5)goto _LLA3;else{_tmp193=_tmp192->f1;_tmp194=_tmp193.ptr_atts;_tmp195=_tmp194.rgn;}}
_LLA2:
# 596
 Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp195);
# 597
_npop_handler(0);return;_LLA3:;_LLA4: {
# 598
const char*_tmp4C3;void*_tmp4C2;(_tmp4C2=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4C3="check_pat_regions: bad pointer type",
_tag_dyneither(_tmp4C3,sizeof(char),36))),_tag_dyneither(_tmp4C2,sizeof(void*),0)));}
_LLA0:;};
# 582
;_pop_region(temp);};_LL98: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*
_tmp186=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp183;if(_tmp186->tag != 
6)goto _LL9A;else{_tmp187=_tmp186->f3;}}_LL99:
# 601
 for(0;_tmp187 != 0;_tmp187=_tmp187->tl){Cyc_Tcpat_check_pat_regions(te,(*((
struct _tuple15*)_tmp187->hd)).f2);}
# 602
return;_LL9A: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp188=(struct
Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp183;if(_tmp188->tag != 7)goto _LL9C;
else{_tmp189=_tmp188->f3;}}_LL9B:
# 604
 _tmp18B=_tmp189;goto _LL9D;_LL9C: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*
_tmp18A=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp183;if(_tmp18A->tag != 
4)goto _LL9E;else{_tmp18B=_tmp18A->f1;}}_LL9D:
# 606
 for(0;_tmp18B != 0;_tmp18B=_tmp18B->tl){Cyc_Tcpat_check_pat_regions(te,(struct
Cyc_Absyn_Pat*)_tmp18B->hd);}
# 607
return;_LL9E:;_LL9F:
# 608
 return;_LL95:;}struct _union_Name_value_Name_v{int tag;struct _dyneither_ptr val;};
struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct
_union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 649 "/home/jgm/cyclone/src/tcpat.cyc"
typedef union Cyc_Tcpat_Name_value Cyc_Tcpat_name_value_t;
# 650
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _dyneither_ptr s){union Cyc_Tcpat_Name_value
_tmp4C4;return((_tmp4C4.Name_v).val=s,(((_tmp4C4.Name_v).tag=1,_tmp4C4)));}
# 651
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){union Cyc_Tcpat_Name_value _tmp4C5;
return((_tmp4C5.Int_v).val=i,(((_tmp4C5.Int_v).tag=2,_tmp4C5)));}struct Cyc_Tcpat_Con_s{
union Cyc_Tcpat_Name_value name;int arity;int*span;struct Cyc_Absyn_Pat*orig_pat;};
# 659
typedef struct Cyc_Tcpat_Con_s*Cyc_Tcpat_con_t;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{
int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*
f1;struct Cyc_List_List*f2;};
# 664
typedef void*Cyc_Tcpat_simple_pat_t;
# 667
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*
c2){
# 668
union Cyc_Tcpat_Name_value _tmp19A=c1->name;struct _dyneither_ptr _tmp19B;int
_tmp19C;_LLA6: if((_tmp19A.Name_v).tag != 1)goto _LLA8;_tmp19B=(struct
_dyneither_ptr)(_tmp19A.Name_v).val;_LLA7: {
# 670
union Cyc_Tcpat_Name_value _tmp19D=c2->name;struct _dyneither_ptr _tmp19E;int
_tmp19F;_LLAB: if((_tmp19D.Name_v).tag != 1)goto _LLAD;_tmp19E=(struct
_dyneither_ptr)(_tmp19D.Name_v).val;_LLAC:
# 671
 return Cyc_strcmp((struct _dyneither_ptr)_tmp19B,(struct _dyneither_ptr)_tmp19E);
_LLAD: if((_tmp19D.Int_v).tag != 2)goto _LLAA;_tmp19F=(int)(_tmp19D.Int_v).val;
_LLAE:
# 672
 return - 1;_LLAA:;}_LLA8: if((_tmp19A.Int_v).tag != 2)goto _LLA5;_tmp19C=(int)(
_tmp19A.Int_v).val;_LLA9: {
# 675
union Cyc_Tcpat_Name_value _tmp1A0=c2->name;struct _dyneither_ptr _tmp1A1;int
_tmp1A2;_LLB0: if((_tmp1A0.Name_v).tag != 1)goto _LLB2;_tmp1A1=(struct
_dyneither_ptr)(_tmp1A0.Name_v).val;_LLB1:
# 676
 return 1;_LLB2: if((_tmp1A0.Int_v).tag != 2)goto _LLAF;_tmp1A2=(int)(_tmp1A0.Int_v).val;
_LLB3:
# 677
 return _tmp19C - _tmp1A2;_LLAF:;}_LLA5:;}
# 683
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct _RegionHandle*r){
# 684
return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,
struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}
# 687
static int Cyc_Tcpat_one_opt=1;
# 688
static int Cyc_Tcpat_two_opt=2;
# 689
static int Cyc_Tcpat_twofiftysix_opt=256;
# 691
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*
p){
# 692
const char*_tmp4C8;struct Cyc_Tcpat_Con_s*_tmp4C7;return(_tmp4C7=_region_malloc(r,
sizeof(*_tmp4C7)),((_tmp4C7->name=Cyc_Tcpat_Name_v(((_tmp4C8="NULL",
_tag_dyneither(_tmp4C8,sizeof(char),5)))),((_tmp4C7->arity=0,((_tmp4C7->span=(
int*)& Cyc_Tcpat_two_opt,((_tmp4C7->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4C7)))))))));}
# 694
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct _RegionHandle*r,struct
Cyc_Absyn_Pat*p){
# 695
const char*_tmp4CB;struct Cyc_Tcpat_Con_s*_tmp4CA;return(_tmp4CA=_region_malloc(r,
sizeof(*_tmp4CA)),((_tmp4CA->name=Cyc_Tcpat_Name_v(((_tmp4CB="&",_tag_dyneither(
_tmp4CB,sizeof(char),2)))),((_tmp4CA->arity=1,((_tmp4CA->span=(int*)& Cyc_Tcpat_two_opt,((
_tmp4CA->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4CA)))))))));}
# 697
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*
p){
# 698
const char*_tmp4CE;struct Cyc_Tcpat_Con_s*_tmp4CD;return(_tmp4CD=_region_malloc(r,
sizeof(*_tmp4CD)),((_tmp4CD->name=Cyc_Tcpat_Name_v(((_tmp4CE="&",_tag_dyneither(
_tmp4CE,sizeof(char),2)))),((_tmp4CD->arity=1,((_tmp4CD->span=(int*)& Cyc_Tcpat_one_opt,((
_tmp4CD->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4CD)))))))));}
# 700
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(struct _RegionHandle*r,int i,struct
Cyc_Absyn_Pat*p){
# 701
struct Cyc_Tcpat_Con_s*_tmp4CF;return(_tmp4CF=_region_malloc(r,sizeof(*_tmp4CF)),((
_tmp4CF->name=Cyc_Tcpat_Int_v(i),((_tmp4CF->arity=0,((_tmp4CF->span=0,((_tmp4CF->orig_pat=
p,_tmp4CF)))))))));}
# 703
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _RegionHandle*r,struct
_dyneither_ptr f,struct Cyc_Absyn_Pat*p){
# 704
struct Cyc_Tcpat_Con_s*_tmp4D0;return(_tmp4D0=_region_malloc(r,sizeof(*_tmp4D0)),((
_tmp4D0->name=Cyc_Tcpat_Name_v(f),((_tmp4D0->arity=0,((_tmp4D0->span=0,((_tmp4D0->orig_pat=(
struct Cyc_Absyn_Pat*)p,_tmp4D0)))))))));}
# 706
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(struct _RegionHandle*r,char c,
struct Cyc_Absyn_Pat*p){
# 707
struct Cyc_Tcpat_Con_s*_tmp4D1;return(_tmp4D1=_region_malloc(r,sizeof(*_tmp4D1)),((
_tmp4D1->name=Cyc_Tcpat_Int_v((int)c),((_tmp4D1->arity=0,((_tmp4D1->span=(int*)&
Cyc_Tcpat_twofiftysix_opt,((_tmp4D1->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4D1)))))))));}
# 709
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(struct _RegionHandle*r,int i,
struct Cyc_Absyn_Pat*p){
# 710
const char*_tmp4D4;struct Cyc_Tcpat_Con_s*_tmp4D3;return(_tmp4D3=_region_malloc(r,
sizeof(*_tmp4D3)),((_tmp4D3->name=Cyc_Tcpat_Name_v(((_tmp4D4="$",_tag_dyneither(
_tmp4D4,sizeof(char),2)))),((_tmp4D3->arity=i,((_tmp4D3->span=(int*)& Cyc_Tcpat_one_opt,((
_tmp4D3->orig_pat=p,_tmp4D3)))))))));}
# 714
static void*Cyc_Tcpat_null_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
# 715
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4D7;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*
_tmp4D6;return(void*)((_tmp4D6=_region_malloc(r,sizeof(*_tmp4D6)),((_tmp4D6[0]=((
_tmp4D7.tag=1,((_tmp4D7.f1=Cyc_Tcpat_null_con(r,p),((_tmp4D7.f2=0,_tmp4D7)))))),
_tmp4D6))));}
# 717
static void*Cyc_Tcpat_int_pat(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
# 718
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4DA;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*
_tmp4D9;return(void*)((_tmp4D9=_region_malloc(r,sizeof(*_tmp4D9)),((_tmp4D9[0]=((
_tmp4DA.tag=1,((_tmp4DA.f1=Cyc_Tcpat_int_con(r,i,p),((_tmp4DA.f2=0,_tmp4DA)))))),
_tmp4D9))));}
# 720
static void*Cyc_Tcpat_char_pat(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){
# 721
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4DD;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*
_tmp4DC;return(void*)((_tmp4DC=_region_malloc(r,sizeof(*_tmp4DC)),((_tmp4DC[0]=((
_tmp4DD.tag=1,((_tmp4DD.f1=Cyc_Tcpat_char_con(r,c,p),((_tmp4DD.f2=0,_tmp4DD)))))),
_tmp4DC))));}
# 723
static void*Cyc_Tcpat_float_pat(struct _RegionHandle*r,struct _dyneither_ptr f,
struct Cyc_Absyn_Pat*p){
# 724
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4E0;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*
_tmp4DF;return(void*)((_tmp4DF=_region_malloc(r,sizeof(*_tmp4DF)),((_tmp4DF[0]=((
_tmp4E0.tag=1,((_tmp4E0.f1=Cyc_Tcpat_float_con(r,f,p),((_tmp4E0.f2=0,_tmp4E0)))))),
_tmp4DF))));}
# 726
static void*Cyc_Tcpat_null_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*
p0){
# 727
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4E6;struct Cyc_List_List*_tmp4E5;
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4E4;return(void*)((_tmp4E4=
_region_malloc(r,sizeof(*_tmp4E4)),((_tmp4E4[0]=((_tmp4E6.tag=1,((_tmp4E6.f1=Cyc_Tcpat_null_ptr_con(
r,p0),((_tmp4E6.f2=((_tmp4E5=_region_malloc(r,sizeof(*_tmp4E5)),((_tmp4E5->hd=p,((
_tmp4E5->tl=0,_tmp4E5)))))),_tmp4E6)))))),_tmp4E4))));}
# 729
static void*Cyc_Tcpat_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*
p0){
# 730
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4EC;struct Cyc_List_List*_tmp4EB;
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4EA;return(void*)((_tmp4EA=
_region_malloc(r,sizeof(*_tmp4EA)),((_tmp4EA[0]=((_tmp4EC.tag=1,((_tmp4EC.f1=Cyc_Tcpat_ptr_con(
r,p0),((_tmp4EC.f2=((_tmp4EB=_region_malloc(r,sizeof(*_tmp4EB)),((_tmp4EB->hd=p,((
_tmp4EB->tl=0,_tmp4EB)))))),_tmp4EC)))))),_tmp4EA))));}
# 732
static void*Cyc_Tcpat_tuple_pat(struct _RegionHandle*r,struct Cyc_List_List*ss,
struct Cyc_Absyn_Pat*p){
# 733
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4EF;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*
_tmp4EE;return(void*)((_tmp4EE=_region_malloc(r,sizeof(*_tmp4EE)),((_tmp4EE[0]=((
_tmp4EF.tag=1,((_tmp4EF.f1=Cyc_Tcpat_tuple_con(r,((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(ss),p),((_tmp4EF.f2=ss,_tmp4EF)))))),_tmp4EE))));}
# 735
static void*Cyc_Tcpat_con_pat(struct _RegionHandle*r,struct _dyneither_ptr con_name,
int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 737
struct Cyc_Tcpat_Con_s*_tmp4F0;struct Cyc_Tcpat_Con_s*c=(_tmp4F0=_region_malloc(r,
sizeof(*_tmp4F0)),((_tmp4F0->name=Cyc_Tcpat_Name_v(con_name),((_tmp4F0->arity=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps),((_tmp4F0->span=span,((
_tmp4F0->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4F0)))))))));
# 738
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4F3;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*
_tmp4F2;return(void*)((_tmp4F2=_region_malloc(r,sizeof(*_tmp4F2)),((_tmp4F2[0]=((
_tmp4F3.tag=1,((_tmp4F3.f1=c,((_tmp4F3.f2=ps,_tmp4F3)))))),_tmp4F2))));}
# 742
static void*Cyc_Tcpat_compile_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
# 743
void*s;
# 744
{void*_tmp1C1=p->r;enum Cyc_Absyn_Sign _tmp1C6;int _tmp1C7;char _tmp1C9;struct
_dyneither_ptr _tmp1CB;struct Cyc_Absyn_Pat*_tmp1CD;struct Cyc_Absyn_Pat*_tmp1CF;
struct Cyc_Absyn_Pat*_tmp1D1;struct Cyc_Absyn_Datatypedecl*_tmp1D3;struct Cyc_Absyn_Datatypefield*
_tmp1D4;struct Cyc_List_List*_tmp1D5;struct Cyc_List_List*_tmp1D7;struct Cyc_Absyn_AggrInfo*
_tmp1D9;struct Cyc_Absyn_AggrInfo _tmp1DA;union Cyc_Absyn_AggrInfoU _tmp1DB;struct
Cyc_Absyn_Aggrdecl**_tmp1DC;struct Cyc_Absyn_Aggrdecl*_tmp1DD;struct Cyc_List_List*
_tmp1DE;struct Cyc_Absyn_Enumdecl*_tmp1E0;struct Cyc_Absyn_Enumfield*_tmp1E1;void*
_tmp1E3;struct Cyc_Absyn_Enumfield*_tmp1E4;_LLB5: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*
_tmp1C2=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp1C1;if(_tmp1C2->tag != 
0)goto _LLB7;}_LLB6:
# 745
 goto _LLB8;_LLB7: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp1C3=(struct
Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1C1;if(_tmp1C3->tag != 3)goto _LLB9;}
_LLB8:
# 746
{struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp4F6;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*
_tmp4F5;s=(void*)((_tmp4F5=_region_malloc(r,sizeof(*_tmp4F5)),((_tmp4F5[0]=((
_tmp4F6.tag=0,_tmp4F6)),_tmp4F5))));}goto _LLB4;_LLB9: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*
_tmp1C4=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp1C1;if(_tmp1C4->tag != 
8)goto _LLBB;}_LLBA:
# 747
 s=Cyc_Tcpat_null_pat(r,p);goto _LLB4;_LLBB: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*
_tmp1C5=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1C1;if(_tmp1C5->tag != 9)
goto _LLBD;else{_tmp1C6=_tmp1C5->f1;_tmp1C7=_tmp1C5->f2;}}_LLBC:
# 748
 s=Cyc_Tcpat_int_pat(r,_tmp1C7,(struct Cyc_Absyn_Pat*)p);goto _LLB4;_LLBD: {struct
Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp1C8=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)
_tmp1C1;if(_tmp1C8->tag != 10)goto _LLBF;else{_tmp1C9=_tmp1C8->f1;}}_LLBE:
# 749
 s=Cyc_Tcpat_char_pat(r,_tmp1C9,p);goto _LLB4;_LLBF: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*
_tmp1CA=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1C1;if(_tmp1CA->tag != 
11)goto _LLC1;else{_tmp1CB=_tmp1CA->f1;}}_LLC0:
# 750
 s=Cyc_Tcpat_float_pat(r,_tmp1CB,p);goto _LLB4;_LLC1: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*
_tmp1CC=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1C1;if(_tmp1CC->tag != 1)
goto _LLC3;else{_tmp1CD=_tmp1CC->f2;}}_LLC2:
# 751
 s=Cyc_Tcpat_compile_pat(r,_tmp1CD);goto _LLB4;_LLC3: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*
_tmp1CE=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1C1;if(_tmp1CE->tag
!= 2)goto _LLC5;else{_tmp1CF=_tmp1CE->f2;}}_LLC4:
# 752
 s=Cyc_Tcpat_compile_pat(r,_tmp1CF);goto _LLB4;_LLC5: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*
_tmp1D0=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1C1;if(_tmp1D0->tag
!= 5)goto _LLC7;else{_tmp1D1=_tmp1D0->f1;}}_LLC6:
# 754
{void*_tmp1EB=Cyc_Tcutil_compress((void*)_check_null(p->topt));struct Cyc_Absyn_PtrInfo
_tmp1ED;struct Cyc_Absyn_PtrAtts _tmp1EE;union Cyc_Absyn_Constraint*_tmp1EF;_LLDA: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1EC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)
_tmp1EB;if(_tmp1EC->tag != 5)goto _LLDC;else{_tmp1ED=_tmp1EC->f1;_tmp1EE=_tmp1ED.ptr_atts;
_tmp1EF=_tmp1EE.nullable;}}_LLDB: {
# 756
int is_nullable=0;
# 757
int still_working=1;
# 758
while(still_working){
# 759
union Cyc_Absyn_Constraint*_tmp1F0=_tmp1EF;union Cyc_Absyn_Constraint _tmp1F1;
union Cyc_Absyn_Constraint*_tmp1F2;union Cyc_Absyn_Constraint _tmp1F3;int _tmp1F4;
union Cyc_Absyn_Constraint _tmp1F5;int _tmp1F6;_LLDF: _tmp1F1=*_tmp1F0;if((_tmp1F1.Forward_constr).tag
!= 2)goto _LLE1;_tmp1F2=(union Cyc_Absyn_Constraint*)(_tmp1F1.Forward_constr).val;
_LLE0:
# 761
*_tmp1EF=*_tmp1F2;
# 762
continue;_LLE1: _tmp1F3=*_tmp1F0;if((_tmp1F3.No_constr).tag != 3)goto _LLE3;
_tmp1F4=(int)(_tmp1F3.No_constr).val;_LLE2:
# 764
{struct _union_Constraint_Eq_constr*_tmp4F7;(_tmp4F7=& _tmp1EF->Eq_constr,((
_tmp4F7->tag=1,_tmp4F7->val=0)));}
# 765
is_nullable=0;
# 766
still_working=0;
# 767
goto _LLDE;_LLE3: _tmp1F5=*_tmp1F0;if((_tmp1F5.Eq_constr).tag != 1)goto _LLDE;
_tmp1F6=(int)(_tmp1F5.Eq_constr).val;_LLE4:
# 769
 is_nullable=(int)_tmp1F6;
# 770
still_working=0;
# 771
goto _LLDE;_LLDE:;}{
# 774
void*ss=Cyc_Tcpat_compile_pat(r,_tmp1D1);
# 775
if(is_nullable)
# 776
s=Cyc_Tcpat_null_ptr_pat(r,ss,p);else{
# 778
s=Cyc_Tcpat_ptr_pat(r,ss,p);}
# 779
goto _LLD9;};}_LLDC:;_LLDD: {
# 780
const char*_tmp4FA;void*_tmp4F9;(_tmp4F9=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4FA="expecting pointertype for pattern!",
_tag_dyneither(_tmp4FA,sizeof(char),35))),_tag_dyneither(_tmp4F9,sizeof(void*),0)));}
_LLD9:;}
# 782
goto _LLB4;_LLC7: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1D2=(
struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1C1;if(_tmp1D2->tag != 7)
goto _LLC9;else{_tmp1D3=_tmp1D2->f1;_tmp1D4=_tmp1D2->f2;_tmp1D5=_tmp1D2->f3;}}
_LLC8: {
# 784
int*span;
# 785
{void*_tmp1FA=Cyc_Tcutil_compress((void*)_check_null(p->topt));_LLE6: {struct
Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1FB=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)
_tmp1FA;if(_tmp1FB->tag != 3)goto _LLE8;}_LLE7:
# 787
 if(_tmp1D3->is_extensible)
# 788
span=0;else{
# 790
int*_tmp4FB;span=((_tmp4FB=_region_malloc(r,sizeof(*_tmp4FB)),((_tmp4FB[0]=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp1D3->fields))->v),_tmp4FB))));}
# 791
goto _LLE5;_LLE8: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1FC=(
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1FA;if(_tmp1FC->tag != 4)
goto _LLEA;}_LLE9:
# 792
 span=(int*)& Cyc_Tcpat_one_opt;goto _LLE5;_LLEA:;_LLEB:
# 793
{const char*_tmp4FE;void*_tmp4FD;span=((_tmp4FD=0,((int*(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4FE="void datatype pattern has bad type",
_tag_dyneither(_tmp4FE,sizeof(char),35))),_tag_dyneither(_tmp4FD,sizeof(void*),0))));}
goto _LLE5;_LLE5:;}
# 795
s=Cyc_Tcpat_con_pat(r,*(*_tmp1D4->name).f2,span,((struct Cyc_List_List*(*)(
struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,
r,_tmp1D5),p);
# 796
goto _LLB4;}_LLC9: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp1D6=(struct
Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1C1;if(_tmp1D6->tag != 4)goto _LLCB;
else{_tmp1D7=_tmp1D6->f1;}}_LLCA:
# 799
 s=Cyc_Tcpat_tuple_pat(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(
struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1D7),(struct Cyc_Absyn_Pat*)p);
goto _LLB4;_LLCB: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1D8=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)
_tmp1C1;if(_tmp1D8->tag != 6)goto _LLCD;else{_tmp1D9=_tmp1D8->f1;if(_tmp1D9 == 0)
goto _LLCD;_tmp1DA=*_tmp1D9;_tmp1DB=_tmp1DA.aggr_info;if((_tmp1DB.KnownAggr).tag
!= 2)goto _LLCD;_tmp1DC=(struct Cyc_Absyn_Aggrdecl**)(_tmp1DB.KnownAggr).val;
_tmp1DD=*_tmp1DC;_tmp1DE=_tmp1D8->f3;}}_LLCC:
# 804
 if(_tmp1DD->kind == Cyc_Absyn_StructA){
# 805
struct Cyc_List_List*ps=0;
# 806
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1DD->impl))->fields;
for(0;fields != 0;fields=fields->tl){
# 808
const char*_tmp4FF;int found=Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)
fields->hd)->name,((_tmp4FF="",_tag_dyneither(_tmp4FF,sizeof(char),1))))== 0;
# 809
{struct Cyc_List_List*dlps0=_tmp1DE;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
# 810
struct _tuple15 _tmp201;struct Cyc_List_List*_tmp202;struct Cyc_Absyn_Pat*_tmp203;
struct _tuple15*_tmp200=(struct _tuple15*)dlps0->hd;_tmp201=*_tmp200;_tmp202=
_tmp201.f1;_tmp203=_tmp201.f2;{
# 811
struct Cyc_List_List*_tmp204=_tmp202;struct Cyc_List_List _tmp205;void*_tmp206;
struct _dyneither_ptr*_tmp208;struct Cyc_List_List*_tmp209;_LLED: if(_tmp204 == 0)
goto _LLEF;_tmp205=*_tmp204;_tmp206=(void*)_tmp205.hd;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*
_tmp207=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp206;if(_tmp207->tag
!= 1)goto _LLEF;else{_tmp208=_tmp207->f1;}};_tmp209=_tmp205.tl;if(_tmp209 != 0)
goto _LLEF;_LLEE:
# 813
 if(Cyc_strptrcmp(_tmp208,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
# 814
{struct Cyc_List_List*_tmp500;ps=((_tmp500=_region_malloc(r,sizeof(*_tmp500)),((
_tmp500->hd=Cyc_Tcpat_compile_pat(r,_tmp203),((_tmp500->tl=ps,_tmp500))))));}
# 815
found=1;}
# 817
goto _LLEC;_LLEF:;_LLF0: {
# 818
const char*_tmp503;void*_tmp502;(_tmp502=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp503="bad designator(s)",
_tag_dyneither(_tmp503,sizeof(char),18))),_tag_dyneither(_tmp502,sizeof(void*),0)));}
_LLEC:;};}}
# 821
if(!found){
# 822
const char*_tmp506;void*_tmp505;(_tmp505=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp506="bad designator",
_tag_dyneither(_tmp506,sizeof(char),15))),_tag_dyneither(_tmp505,sizeof(void*),0)));}}}
# 824
s=Cyc_Tcpat_tuple_pat(r,ps,(struct Cyc_Absyn_Pat*)p);}else{
# 827
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1DD->impl))->tagged){
# 828
const char*_tmp509;void*_tmp508;(_tmp508=0,Cyc_Tcutil_terr(p->loc,((_tmp509="patterns on untagged unions not yet supported.",
_tag_dyneither(_tmp509,sizeof(char),47))),_tag_dyneither(_tmp508,sizeof(void*),0)));}{
# 829
int*_tmp50A;int*span=(_tmp50A=_region_malloc(r,sizeof(*_tmp50A)),((_tmp50A[0]=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1DD->impl))->fields),_tmp50A)));
# 830
struct Cyc_List_List*_tmp212=_tmp1DE;struct Cyc_List_List _tmp213;struct _tuple15*
_tmp214;struct _tuple15 _tmp215;struct Cyc_List_List*_tmp216;struct Cyc_List_List
_tmp217;void*_tmp218;struct _dyneither_ptr*_tmp21A;struct Cyc_List_List*_tmp21B;
struct Cyc_Absyn_Pat*_tmp21C;struct Cyc_List_List*_tmp21D;_LLF2: if(_tmp212 == 0)
goto _LLF4;_tmp213=*_tmp212;_tmp214=(struct _tuple15*)_tmp213.hd;_tmp215=*_tmp214;
_tmp216=_tmp215.f1;if(_tmp216 == 0)goto _LLF4;_tmp217=*_tmp216;_tmp218=(void*)
_tmp217.hd;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp219=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)
_tmp218;if(_tmp219->tag != 1)goto _LLF4;else{_tmp21A=_tmp219->f1;}};_tmp21B=
_tmp217.tl;if(_tmp21B != 0)goto _LLF4;_tmp21C=_tmp215.f2;_tmp21D=_tmp213.tl;if(
_tmp21D != 0)goto _LLF4;_LLF3:
# 832
{struct Cyc_List_List*_tmp50B;s=Cyc_Tcpat_con_pat(r,*_tmp21A,span,((_tmp50B=
_region_malloc(r,sizeof(*_tmp50B)),((_tmp50B->hd=Cyc_Tcpat_compile_pat(r,_tmp21C),((
_tmp50B->tl=0,_tmp50B)))))),p);}
# 833
goto _LLF1;_LLF4:;_LLF5: {
# 834
const char*_tmp50E;void*_tmp50D;(_tmp50D=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp50E="bad union pattern",
_tag_dyneither(_tmp50E,sizeof(char),18))),_tag_dyneither(_tmp50D,sizeof(void*),0)));}
_LLF1:;};}
# 837
goto _LLB4;_LLCD: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp1DF=(struct
Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1C1;if(_tmp1DF->tag != 12)goto _LLCF;
else{_tmp1E0=_tmp1DF->f1;_tmp1E1=_tmp1DF->f2;}}_LLCE: {
# 842
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1E0->fields))->v);
# 843
{int*_tmp50F;s=Cyc_Tcpat_con_pat(r,*(*_tmp1E1->name).f2,((_tmp50F=
_region_malloc(r,sizeof(*_tmp50F)),((_tmp50F[0]=span,_tmp50F)))),0,p);}
# 844
goto _LLB4;}_LLCF: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp1E2=(
struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1C1;if(_tmp1E2->tag != 13)
goto _LLD1;else{_tmp1E3=(void*)_tmp1E2->f1;_tmp1E4=_tmp1E2->f2;}}_LLD0: {
# 849
struct Cyc_List_List*fields;
# 850
{void*_tmp223=Cyc_Tcutil_compress(_tmp1E3);struct Cyc_List_List*_tmp225;_LLF7: {
struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp224=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)
_tmp223;if(_tmp224->tag != 14)goto _LLF9;else{_tmp225=_tmp224->f1;}}_LLF8:
# 851
 fields=_tmp225;goto _LLF6;_LLF9:;_LLFA: {
# 852
const char*_tmp512;void*_tmp511;(_tmp511=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp512="bad type in AnonEnum_p",
_tag_dyneither(_tmp512,sizeof(char),23))),_tag_dyneither(_tmp511,sizeof(void*),0)));}
_LLF6:;}{
# 856
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
# 857
{int*_tmp513;s=Cyc_Tcpat_con_pat(r,*(*_tmp1E4->name).f2,((_tmp513=
_region_malloc(r,sizeof(*_tmp513)),((_tmp513[0]=span,_tmp513)))),0,p);}
# 858
goto _LLB4;};}_LLD1: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp1E5=(
struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1C1;if(_tmp1E5->tag != 14)
goto _LLD3;}_LLD2:
# 860
 goto _LLD4;_LLD3: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp1E6=(
struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1C1;if(_tmp1E6->tag != 15)
goto _LLD5;}_LLD4:
# 861
 goto _LLD6;_LLD5: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1E7=(struct
Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1C1;if(_tmp1E7->tag != 6)goto _LLD7;}
_LLD6:
# 862
 goto _LLD8;_LLD7: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp1E8=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)
_tmp1C1;if(_tmp1E8->tag != 16)goto _LLB4;}_LLD8: {
# 863
struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp516;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*
_tmp515;s=(void*)((_tmp515=_region_malloc(r,sizeof(*_tmp515)),((_tmp515[0]=((
_tmp516.tag=0,_tmp516)),_tmp515))));}_LLB4:;}
# 865
return s;}
# 873
typedef struct Cyc_List_List*Cyc_Tcpat_match_t;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{
int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{
int tag;struct Cyc_Set_Set*f1;};
# 884
typedef void*Cyc_Tcpat_term_desc_t;
# 888
typedef struct Cyc_List_List*Cyc_Tcpat_context_t;
# 895
typedef struct Cyc_List_List*Cyc_Tcpat_access_t;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{
int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;void*f1;};
struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct
Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};
# 904
typedef void*Cyc_Tcpat_decision_t;struct _tuple18{struct Cyc_List_List*f1;struct
Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 909
typedef struct _tuple18*Cyc_Tcpat_work_stack_frame_t;
# 912
typedef struct Cyc_List_List*Cyc_Tcpat_work_stack_t;
# 914
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes  = 0,Cyc_Tcpat_No  = 1,Cyc_Tcpat_Maybe  = 2};
# 915
typedef enum Cyc_Tcpat_Answer Cyc_Tcpat_answer_t;
# 917
static void Cyc_Tcpat_print_tab(int i){
# 918
for(0;i != 0;-- i){
# 919
const char*_tmp519;void*_tmp518;(_tmp518=0,Cyc_fprintf(Cyc_stderr,((_tmp519=" ",
_tag_dyneither(_tmp519,sizeof(char),2))),_tag_dyneither(_tmp518,sizeof(void*),0)));}}
# 923
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
# 924
union Cyc_Tcpat_Name_value _tmp22D=c->name;
# 925
union Cyc_Tcpat_Name_value _tmp22E=_tmp22D;struct _dyneither_ptr _tmp22F;int _tmp230;
_LLFC: if((_tmp22E.Name_v).tag != 1)goto _LLFE;_tmp22F=(struct _dyneither_ptr)(
_tmp22E.Name_v).val;_LLFD:
# 926
{const char*_tmp51D;void*_tmp51C[1];struct Cyc_String_pa_PrintArg_struct _tmp51B;(
_tmp51B.tag=0,((_tmp51B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp22F),((
_tmp51C[0]=& _tmp51B,Cyc_fprintf(Cyc_stderr,((_tmp51D="%s",_tag_dyneither(_tmp51D,
sizeof(char),3))),_tag_dyneither(_tmp51C,sizeof(void*),1)))))));}goto _LLFB;_LLFE:
if((_tmp22E.Int_v).tag != 2)goto _LLFB;_tmp230=(int)(_tmp22E.Int_v).val;_LLFF:
# 927
{const char*_tmp521;void*_tmp520[1];struct Cyc_Int_pa_PrintArg_struct _tmp51F;(
_tmp51F.tag=1,((_tmp51F.f1=(unsigned long)_tmp230,((_tmp520[0]=& _tmp51F,Cyc_fprintf(
Cyc_stderr,((_tmp521="%d",_tag_dyneither(_tmp521,sizeof(char),3))),
_tag_dyneither(_tmp520,sizeof(void*),1)))))));}goto _LLFB;_LLFB:;}
# 931
static void Cyc_Tcpat_print_dec_tree(void*d,void(*print_rhs)(void*),int tab){
# 932
void*_tmp237=d;void*_tmp239;struct Cyc_List_List*_tmp23C;struct Cyc_Tcpat_Con_s*
_tmp23D;void*_tmp23E;void*_tmp23F;_LL101: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*
_tmp238=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp237;if(_tmp238->tag
!= 1)goto _LL103;else{_tmp239=(void*)_tmp238->f1;}}_LL102:
# 934
 Cyc_Tcpat_print_tab(tab);
# 935
{const char*_tmp524;void*_tmp523;(_tmp523=0,Cyc_fprintf(Cyc_stderr,((_tmp524="Success(",
_tag_dyneither(_tmp524,sizeof(char),9))),_tag_dyneither(_tmp523,sizeof(void*),0)));}
print_rhs(_tmp239);{const char*_tmp527;void*_tmp526;(_tmp526=0,Cyc_fprintf(Cyc_stderr,((
_tmp527=")\n",_tag_dyneither(_tmp527,sizeof(char),3))),_tag_dyneither(_tmp526,
sizeof(void*),0)));}
# 936
goto _LL100;_LL103: {struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp23A=(
struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp237;if(_tmp23A->tag != 0)goto
_LL105;}_LL104:
# 937
{const char*_tmp52A;void*_tmp529;(_tmp529=0,Cyc_fprintf(Cyc_stderr,((_tmp52A="Failure\n",
_tag_dyneither(_tmp52A,sizeof(char),9))),_tag_dyneither(_tmp529,sizeof(void*),0)));}
goto _LL100;_LL105: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp23B=(struct
Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp237;if(_tmp23B->tag != 2)goto _LL100;
else{_tmp23C=_tmp23B->f1;_tmp23D=_tmp23B->f2;_tmp23E=(void*)_tmp23B->f3;_tmp23F=(
void*)_tmp23B->f4;}}_LL106:
# 939
 Cyc_Tcpat_print_tab(tab);
# 940
{const char*_tmp52D;void*_tmp52C;(_tmp52C=0,Cyc_fprintf(Cyc_stderr,((_tmp52D="Access[",
_tag_dyneither(_tmp52D,sizeof(char),8))),_tag_dyneither(_tmp52C,sizeof(void*),0)));}
# 941
for(0;_tmp23C != 0;_tmp23C=_tmp23C->tl){
# 942
{const char*_tmp531;void*_tmp530[1];struct Cyc_Int_pa_PrintArg_struct _tmp52F;(
_tmp52F.tag=1,((_tmp52F.f1=(unsigned long)((int)_tmp23C->hd),((_tmp530[0]=&
_tmp52F,Cyc_fprintf(Cyc_stderr,((_tmp531="%d",_tag_dyneither(_tmp531,sizeof(char),
3))),_tag_dyneither(_tmp530,sizeof(void*),1)))))));}
# 943
if(_tmp23C->tl != 0){const char*_tmp534;void*_tmp533;(_tmp533=0,Cyc_fprintf(Cyc_stderr,((
_tmp534=",",_tag_dyneither(_tmp534,sizeof(char),2))),_tag_dyneither(_tmp533,
sizeof(void*),0)));}}
# 945
{const char*_tmp537;void*_tmp536;(_tmp536=0,Cyc_fprintf(Cyc_stderr,((_tmp537="],",
_tag_dyneither(_tmp537,sizeof(char),3))),_tag_dyneither(_tmp536,sizeof(void*),0)));}
# 946
Cyc_Tcpat_print_con(_tmp23D);
# 947
{const char*_tmp53A;void*_tmp539;(_tmp539=0,Cyc_fprintf(Cyc_stderr,((_tmp53A=",\n",
_tag_dyneither(_tmp53A,sizeof(char),3))),_tag_dyneither(_tmp539,sizeof(void*),0)));}
# 948
Cyc_Tcpat_print_dec_tree(_tmp23E,print_rhs,tab + 7);
# 949
Cyc_Tcpat_print_dec_tree(_tmp23F,print_rhs,tab + 7);_LL100:;}struct _tuple19{int
f1;unsigned int f2;};
# 953
static void Cyc_Tcpat_print_swrhs(struct _tuple19*x){
# 954
return;}
# 961
static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*
c){
# 962
void*_tmp251=td;struct Cyc_Set_Set*_tmp253;_LL108: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*
_tmp252=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp251;if(_tmp252->tag != 1)
goto _LL10A;else{_tmp253=_tmp252->f1;}}_LL109: {
# 971
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp53D;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*
_tmp53C;return(void*)((_tmp53C=_region_malloc(r,sizeof(*_tmp53C)),((_tmp53C[0]=((
_tmp53D.tag=1,((_tmp53D.f1=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct
Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(r,_tmp253,c),_tmp53D)))),
_tmp53C))));}_LL10A: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp254=(struct
Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp251;if(_tmp254->tag != 0)goto _LL107;}
_LL10B: {
# 972
const char*_tmp540;void*_tmp53F;(_tmp53F=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp540="add_neg called when td is Positive",
_tag_dyneither(_tmp540,sizeof(char),35))),_tag_dyneither(_tmp53F,sizeof(void*),0)));}
_LL107:;}
# 978
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*
td){
# 979
void*_tmp259=td;struct Cyc_Tcpat_Con_s*_tmp25B;struct Cyc_Set_Set*_tmp25D;_LL10D: {
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp25A=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)
_tmp259;if(_tmp25A->tag != 0)goto _LL10F;else{_tmp25B=_tmp25A->f1;}}_LL10E:
# 982
 if(Cyc_Tcpat_compare_con(c,_tmp25B)== 0)return Cyc_Tcpat_Yes;else{
# 983
return Cyc_Tcpat_No;}_LL10F: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp25C=(
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp259;if(_tmp25C->tag != 1)goto
_LL10C;else{_tmp25D=_tmp25C->f1;}}_LL110:
# 986
 if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp25D,c))return Cyc_Tcpat_No;else{
# 989
if(c->span != 0  && *((int*)_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))
Cyc_Set_cardinality)(_tmp25D)+ 1)
# 990
return Cyc_Tcpat_Yes;else{
# 992
return Cyc_Tcpat_Maybe;}}_LL10C:;}struct _tuple20{struct Cyc_Tcpat_Con_s*f1;struct
Cyc_List_List*f2;};
# 1000
static struct Cyc_List_List*Cyc_Tcpat_augment(struct _RegionHandle*r,struct Cyc_List_List*
ctxt,void*dsc){
# 1002
struct Cyc_List_List*_tmp25E=ctxt;struct Cyc_List_List _tmp25F;struct _tuple20*
_tmp260;struct _tuple20 _tmp261;struct Cyc_Tcpat_Con_s*_tmp262;struct Cyc_List_List*
_tmp263;struct Cyc_List_List*_tmp264;_LL112: if(_tmp25E != 0)goto _LL114;_LL113:
# 1003
 return 0;_LL114: if(_tmp25E == 0)goto _LL111;_tmp25F=*_tmp25E;_tmp260=(struct
_tuple20*)_tmp25F.hd;_tmp261=*_tmp260;_tmp262=_tmp261.f1;_tmp263=_tmp261.f2;
_tmp264=_tmp25F.tl;_LL115: {
# 1005
struct _tuple20*_tmp546;struct Cyc_List_List*_tmp545;struct Cyc_List_List*_tmp544;
return(_tmp544=_region_malloc(r,sizeof(*_tmp544)),((_tmp544->hd=(
# 1006
(_tmp546=_region_malloc(r,sizeof(*_tmp546)),((_tmp546->f1=_tmp262,((_tmp546->f2=(
struct Cyc_List_List*)((_tmp545=_region_malloc(r,sizeof(*_tmp545)),((_tmp545->hd=
dsc,((_tmp545->tl=_tmp263,_tmp545)))))),_tmp546)))))),((_tmp544->tl=_tmp264,
_tmp544)))));}_LL111:;}
# 1013
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct _RegionHandle*r,struct
Cyc_List_List*ctxt){
# 1014
struct Cyc_List_List*_tmp268=ctxt;struct Cyc_List_List _tmp269;struct _tuple20*
_tmp26A;struct _tuple20 _tmp26B;struct Cyc_Tcpat_Con_s*_tmp26C;struct Cyc_List_List*
_tmp26D;struct Cyc_List_List*_tmp26E;_LL117: if(_tmp268 != 0)goto _LL119;_LL118: {
# 1015
const char*_tmp549;void*_tmp548;(_tmp548=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp549="norm_context: empty context",
_tag_dyneither(_tmp549,sizeof(char),28))),_tag_dyneither(_tmp548,sizeof(void*),0)));}
_LL119: if(_tmp268 == 0)goto _LL116;_tmp269=*_tmp268;_tmp26A=(struct _tuple20*)
_tmp269.hd;_tmp26B=*_tmp26A;_tmp26C=_tmp26B.f1;_tmp26D=_tmp26B.f2;_tmp26E=
_tmp269.tl;_LL11A: {
# 1017
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp54C;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*
_tmp54B;return Cyc_Tcpat_augment(r,_tmp26E,(void*)((_tmp54B=_region_malloc(r,
sizeof(*_tmp54B)),((_tmp54B[0]=((_tmp54C.tag=0,((_tmp54C.f1=_tmp26C,((_tmp54C.f2=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(
r,_tmp26D),_tmp54C)))))),_tmp54B)))));}_LL116:;}
# 1026
static void*Cyc_Tcpat_build_desc(struct _RegionHandle*r,struct Cyc_List_List*ctxt,
void*dsc,struct Cyc_List_List*work){
# 1028
struct _tuple13 _tmp54D;struct _tuple13 _tmp274=(_tmp54D.f1=ctxt,((_tmp54D.f2=work,
_tmp54D)));struct Cyc_List_List*_tmp275;struct Cyc_List_List*_tmp276;struct Cyc_List_List*
_tmp277;struct Cyc_List_List*_tmp278;struct Cyc_List_List*_tmp279;struct Cyc_List_List
_tmp27A;struct _tuple20*_tmp27B;struct _tuple20 _tmp27C;struct Cyc_Tcpat_Con_s*
_tmp27D;struct Cyc_List_List*_tmp27E;struct Cyc_List_List*_tmp27F;struct Cyc_List_List*
_tmp280;struct Cyc_List_List _tmp281;struct _tuple18*_tmp282;struct _tuple18 _tmp283;
struct Cyc_List_List*_tmp284;struct Cyc_List_List*_tmp285;_LL11C: _tmp275=_tmp274.f1;
if(_tmp275 != 0)goto _LL11E;_tmp276=_tmp274.f2;if(_tmp276 != 0)goto _LL11E;_LL11D:
# 1029
 return dsc;_LL11E: _tmp277=_tmp274.f1;if(_tmp277 != 0)goto _LL120;_LL11F:
# 1030
 goto _LL121;_LL120: _tmp278=_tmp274.f2;if(_tmp278 != 0)goto _LL122;_LL121: {
# 1031
const char*_tmp550;void*_tmp54F;(_tmp54F=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp550="build_desc: ctxt and work don't match",
_tag_dyneither(_tmp550,sizeof(char),38))),_tag_dyneither(_tmp54F,sizeof(void*),0)));}
_LL122: _tmp279=_tmp274.f1;if(_tmp279 == 0)goto _LL11B;_tmp27A=*_tmp279;_tmp27B=(
struct _tuple20*)_tmp27A.hd;_tmp27C=*_tmp27B;_tmp27D=_tmp27C.f1;_tmp27E=_tmp27C.f2;
_tmp27F=_tmp27A.tl;_tmp280=_tmp274.f2;if(_tmp280 == 0)goto _LL11B;_tmp281=*_tmp280;
_tmp282=(struct _tuple18*)_tmp281.hd;_tmp283=*_tmp282;_tmp284=_tmp283.f3;_tmp285=
_tmp281.tl;_LL123: {
# 1033
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp556;struct Cyc_List_List*_tmp555;
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp554;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*
_tmp288=(_tmp554=_region_malloc(r,sizeof(*_tmp554)),((_tmp554[0]=((_tmp556.tag=0,((
_tmp556.f1=_tmp27D,((_tmp556.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp27E),(
# 1034
(_tmp555=_region_malloc(r,sizeof(*_tmp555)),((_tmp555->hd=dsc,((_tmp555->tl=
_tmp284,_tmp555))))))),_tmp556)))))),_tmp554)));
# 1035
return Cyc_Tcpat_build_desc(r,_tmp27F,(void*)_tmp288,_tmp285);}_LL11B:;}
# 1039
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,
void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,
struct Cyc_List_List*rules);
# 1046
static void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*dsc,struct Cyc_List_List*
allmrules){
# 1048
struct Cyc_List_List*_tmp28C=allmrules;struct Cyc_List_List _tmp28D;struct _tuple0*
_tmp28E;struct _tuple0 _tmp28F;void*_tmp290;void*_tmp291;struct Cyc_List_List*
_tmp292;_LL125: if(_tmp28C != 0)goto _LL127;_LL126: {
# 1049
struct Cyc_Tcpat_Failure_Tcpat_Decision_struct _tmp559;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*
_tmp558;return(void*)((_tmp558=_cycalloc(sizeof(*_tmp558)),((_tmp558[0]=((
_tmp559.tag=0,((_tmp559.f1=(void*)dsc,_tmp559)))),_tmp558))));}_LL127: if(_tmp28C
== 0)goto _LL124;_tmp28D=*_tmp28C;_tmp28E=(struct _tuple0*)_tmp28D.hd;_tmp28F=*
_tmp28E;_tmp290=_tmp28F.f1;_tmp291=_tmp28F.f2;_tmp292=_tmp28D.tl;_LL128:
# 1051
 return Cyc_Tcpat_match(r,_tmp290,0,dsc,0,0,_tmp291,_tmp292);_LL124:;}
# 1056
static void*Cyc_Tcpat_match_compile(struct _RegionHandle*r,struct Cyc_List_List*
allmrules){
# 1058
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp55C;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*
_tmp55B;return Cyc_Tcpat_or_match(r,(void*)((_tmp55B=_region_malloc(r,sizeof(*
_tmp55B)),((_tmp55B[0]=((_tmp55C.tag=1,((_tmp55C.f1=Cyc_Tcpat_empty_con_set(r),
_tmp55C)))),_tmp55B)))),allmrules);}
# 1064
static void*Cyc_Tcpat_and_match(struct _RegionHandle*r,struct Cyc_List_List*ctx,
struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
# 1068
struct Cyc_List_List*_tmp297=work;struct Cyc_List_List _tmp298;struct _tuple18*
_tmp299;struct _tuple18 _tmp29A;struct Cyc_List_List*_tmp29B;struct Cyc_List_List*
_tmp29C;struct Cyc_List_List*_tmp29D;struct Cyc_List_List*_tmp29E;struct Cyc_List_List
_tmp29F;struct _tuple18*_tmp2A0;struct _tuple18 _tmp2A1;struct Cyc_List_List*_tmp2A2;
struct Cyc_List_List*_tmp2A3;struct Cyc_List_List*_tmp2A4;struct Cyc_List_List*
_tmp2A5;_LL12A: if(_tmp297 != 0)goto _LL12C;_LL12B: {
# 1069
struct Cyc_Tcpat_Success_Tcpat_Decision_struct _tmp55F;struct Cyc_Tcpat_Success_Tcpat_Decision_struct*
_tmp55E;return(void*)((_tmp55E=_region_malloc(r,sizeof(*_tmp55E)),((_tmp55E[0]=((
_tmp55F.tag=1,((_tmp55F.f1=(void*)right_hand_side,_tmp55F)))),_tmp55E))));}
_LL12C: if(_tmp297 == 0)goto _LL12E;_tmp298=*_tmp297;_tmp299=(struct _tuple18*)
_tmp298.hd;_tmp29A=*_tmp299;_tmp29B=_tmp29A.f1;if(_tmp29B != 0)goto _LL12E;_tmp29C=
_tmp29A.f2;if(_tmp29C != 0)goto _LL12E;_tmp29D=_tmp29A.f3;if(_tmp29D != 0)goto
_LL12E;_tmp29E=_tmp298.tl;_LL12D:
# 1071
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(r,ctx),_tmp29E,
right_hand_side,rules);_LL12E: if(_tmp297 == 0)goto _LL129;_tmp29F=*_tmp297;_tmp2A0=(
struct _tuple18*)_tmp29F.hd;_tmp2A1=*_tmp2A0;_tmp2A2=_tmp2A1.f1;_tmp2A3=_tmp2A1.f2;
_tmp2A4=_tmp2A1.f3;_tmp2A5=_tmp29F.tl;_LL12F:
# 1073
 if((_tmp2A2 == 0  || _tmp2A3 == 0) || _tmp2A4 == 0){
# 1074
const char*_tmp562;void*_tmp561;(_tmp561=0,Cyc_Tcutil_impos(((_tmp562="tcpat:and_match: malformed work frame",
_tag_dyneither(_tmp562,sizeof(char),38))),_tag_dyneither(_tmp561,sizeof(void*),0)));}{
# 1075
struct Cyc_List_List _tmp2AB;void*_tmp2AC;struct Cyc_List_List*_tmp2AD;struct Cyc_List_List*
_tmp2AA=(struct Cyc_List_List*)_tmp2A2;_tmp2AB=*_tmp2AA;_tmp2AC=(void*)_tmp2AB.hd;
_tmp2AD=_tmp2AB.tl;{
# 1076
struct Cyc_List_List _tmp2AF;struct Cyc_List_List*_tmp2B0;struct Cyc_List_List*
_tmp2B1;struct Cyc_List_List*_tmp2AE=(struct Cyc_List_List*)_tmp2A3;_tmp2AF=*
_tmp2AE;_tmp2B0=(struct Cyc_List_List*)_tmp2AF.hd;_tmp2B1=_tmp2AF.tl;{
# 1077
struct Cyc_List_List _tmp2B3;void*_tmp2B4;struct Cyc_List_List*_tmp2B5;struct Cyc_List_List*
_tmp2B2=(struct Cyc_List_List*)_tmp2A4;_tmp2B3=*_tmp2B2;_tmp2B4=(void*)_tmp2B3.hd;
_tmp2B5=_tmp2B3.tl;{
# 1078
struct _tuple18*_tmp563;struct _tuple18*_tmp2B6=(_tmp563=_region_malloc(r,sizeof(*
_tmp563)),((_tmp563->f1=_tmp2AD,((_tmp563->f2=_tmp2B1,((_tmp563->f3=_tmp2B5,
_tmp563)))))));
# 1079
struct Cyc_List_List*_tmp564;return Cyc_Tcpat_match(r,_tmp2AC,_tmp2B0,_tmp2B4,ctx,((
_tmp564=_region_malloc(r,sizeof(*_tmp564)),((_tmp564->hd=_tmp2B6,((_tmp564->tl=
_tmp2A5,_tmp564)))))),right_hand_side,rules);};};};};_LL129:;}
# 1085
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*
pcon,void*dsc){
# 1087
void*_tmp2B9=dsc;struct Cyc_Set_Set*_tmp2BB;struct Cyc_List_List*_tmp2BD;_LL131: {
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2BA=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)
_tmp2B9;if(_tmp2BA->tag != 1)goto _LL133;else{_tmp2BB=_tmp2BA->f1;}}_LL132: {
# 1092
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp567;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*
_tmp566;void*any=(void*)((_tmp566=_region_malloc(r,sizeof(*_tmp566)),((_tmp566[0]=((
_tmp567.tag=1,((_tmp567.f1=Cyc_Tcpat_empty_con_set(r),_tmp567)))),_tmp566))));
# 1093
struct Cyc_List_List*_tmp2BE=0;
# 1094
{int i=0;for(0;i < pcon->arity;++ i){
# 1095
struct Cyc_List_List*_tmp568;_tmp2BE=((_tmp568=_region_malloc(r,sizeof(*_tmp568)),((
_tmp568->hd=any,((_tmp568->tl=_tmp2BE,_tmp568))))));}}
# 1096
return _tmp2BE;}_LL133: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2BC=(
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2B9;if(_tmp2BC->tag != 0)goto
_LL130;else{_tmp2BD=_tmp2BC->f2;}}_LL134:
# 1097
 return _tmp2BD;_LL130:;}struct _tuple21{struct _RegionHandle*f1;struct Cyc_List_List*
f2;};
# 1102
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple21*env,int i){
# 1103
struct _tuple21 _tmp2C3;struct _RegionHandle*_tmp2C4;struct Cyc_List_List*_tmp2C5;
struct _tuple21*_tmp2C2=env;_tmp2C3=*_tmp2C2;_tmp2C4=_tmp2C3.f1;_tmp2C5=_tmp2C3.f2;{
# 1104
struct Cyc_List_List*_tmp569;return(_tmp569=_region_malloc(_tmp2C4,sizeof(*
_tmp569)),((_tmp569->hd=(void*)(i + 1),((_tmp569->tl=_tmp2C5,_tmp569)))));};}
# 1106
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*
pcon,struct Cyc_List_List*obj){
# 1108
struct _tuple21 _tmp56A;struct _tuple21 _tmp2C7=(_tmp56A.f1=r,((_tmp56A.f2=obj,
_tmp56A)));
# 1109
return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*
f)(struct _tuple21*,int),struct _tuple21*env))Cyc_List_rtabulate_c)(r,pcon->arity,
Cyc_Tcpat_getoarg,& _tmp2C7);}
# 1115
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,
void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,
struct Cyc_List_List*rules){
# 1119
void*_tmp2C9=p;struct Cyc_Tcpat_Con_s*_tmp2CC;struct Cyc_List_List*_tmp2CD;_LL136: {
struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp2CA=(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)
_tmp2C9;if(_tmp2CA->tag != 0)goto _LL138;}_LL137:
# 1121
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,
rules);_LL138: {struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp2CB=(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)
_tmp2C9;if(_tmp2CB->tag != 1)goto _LL135;else{_tmp2CC=_tmp2CB->f1;_tmp2CD=_tmp2CB->f2;}}
_LL139:
# 1123
 switch(Cyc_Tcpat_static_match(_tmp2CC,dsc)){case Cyc_Tcpat_Yes: _LL13A: {
# 1125
struct _tuple20*_tmp56D;struct Cyc_List_List*_tmp56C;struct Cyc_List_List*ctx2=(
_tmp56C=_region_malloc(r,sizeof(*_tmp56C)),((_tmp56C->hd=((_tmp56D=
_region_malloc(r,sizeof(*_tmp56D)),((_tmp56D->f1=_tmp2CC,((_tmp56D->f2=0,_tmp56D)))))),((
_tmp56C->tl=ctx,_tmp56C)))));
# 1126
struct _tuple18*_tmp56E;struct _tuple18*work_frame=(_tmp56E=_region_malloc(r,
sizeof(*_tmp56E)),((_tmp56E->f1=_tmp2CD,((_tmp56E->f2=Cyc_Tcpat_getoargs(r,
_tmp2CC,obj),((_tmp56E->f3=
# 1127
Cyc_Tcpat_getdargs(r,_tmp2CC,dsc),_tmp56E)))))));
# 1128
struct Cyc_List_List*_tmp56F;struct Cyc_List_List*work2=(_tmp56F=_region_malloc(r,
sizeof(*_tmp56F)),((_tmp56F->hd=work_frame,((_tmp56F->tl=work,_tmp56F)))));
# 1129
return Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No:
_LL13B:
# 1131
 return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);case Cyc_Tcpat_Maybe:
_LL13C: {
# 1133
struct _tuple20*_tmp572;struct Cyc_List_List*_tmp571;struct Cyc_List_List*ctx2=(
_tmp571=_region_malloc(r,sizeof(*_tmp571)),((_tmp571->hd=((_tmp572=
_region_malloc(r,sizeof(*_tmp572)),((_tmp572->f1=_tmp2CC,((_tmp572->f2=0,_tmp572)))))),((
_tmp571->tl=ctx,_tmp571)))));
# 1134
struct _tuple18*_tmp573;struct _tuple18*work_frame=(_tmp573=_region_malloc(r,
sizeof(*_tmp573)),((_tmp573->f1=_tmp2CD,((_tmp573->f2=Cyc_Tcpat_getoargs(r,
_tmp2CC,obj),((_tmp573->f3=
# 1135
Cyc_Tcpat_getdargs(r,_tmp2CC,dsc),_tmp573)))))));
# 1136
struct Cyc_List_List*_tmp574;struct Cyc_List_List*work2=(_tmp574=_region_malloc(r,
sizeof(*_tmp574)),((_tmp574->hd=work_frame,((_tmp574->tl=work,_tmp574)))));
# 1137
void*_tmp2D2=Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);
# 1138
void*_tmp2D3=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(
r,dsc,_tmp2CC),work),rules);
# 1140
struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct _tmp577;struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*
_tmp576;return(void*)((_tmp576=_region_malloc(r,sizeof(*_tmp576)),((_tmp576[0]=((
_tmp577.tag=2,((_tmp577.f1=obj,((_tmp577.f2=_tmp2CC,((_tmp577.f3=(void*)_tmp2D2,((
_tmp577.f4=(void*)_tmp2D3,_tmp577)))))))))),_tmp576))));}}_LL135:;}
# 1150
static void Cyc_Tcpat_check_exhaust_overlap(struct _RegionHandle*r,void*d,void(*
not_exhaust)(struct _RegionHandle*,void*,void*),void*env1,void(*rhs_appears)(void*,
void*),void*env2,int*exhaust_done){
# 1156
void*_tmp2DA=d;void*_tmp2DC;void*_tmp2DE;void*_tmp2E0;void*_tmp2E1;_LL13F: {
struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp2DB=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)
_tmp2DA;if(_tmp2DB->tag != 0)goto _LL141;else{_tmp2DC=(void*)_tmp2DB->f1;}}_LL140:
# 1158
 if(!(*exhaust_done)){not_exhaust(r,env1,_tmp2DC);*exhaust_done=1;}
# 1159
goto _LL13E;_LL141: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp2DD=(
struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp2DA;if(_tmp2DD->tag != 1)goto
_LL143;else{_tmp2DE=(void*)_tmp2DD->f1;}}_LL142:
# 1160
 rhs_appears(env2,_tmp2DE);goto _LL13E;_LL143: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*
_tmp2DF=(struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp2DA;if(_tmp2DF->tag != 2)
goto _LL13E;else{_tmp2E0=(void*)_tmp2DF->f3;_tmp2E1=(void*)_tmp2DF->f4;}}_LL144:
# 1162
 Cyc_Tcpat_check_exhaust_overlap(r,_tmp2E0,not_exhaust,env1,rhs_appears,env2,
exhaust_done);
# 1164
Cyc_Tcpat_check_exhaust_overlap(r,_tmp2E1,not_exhaust,env1,rhs_appears,env2,
exhaust_done);
# 1166
goto _LL13E;_LL13E:;}struct _tuple22{void*f1;struct _tuple19*f2;};struct _tuple23{
struct _RegionHandle*f1;int*f2;};
# 1174
static struct _tuple22*Cyc_Tcpat_get_match(struct _tuple23*env,struct Cyc_Absyn_Switch_clause*
swc){
# 1177
struct _tuple23 _tmp2E3;struct _RegionHandle*_tmp2E4;int*_tmp2E5;struct _tuple23*
_tmp2E2=env;_tmp2E3=*_tmp2E2;_tmp2E4=_tmp2E3.f1;_tmp2E5=_tmp2E3.f2;{
# 1178
void*sp0=Cyc_Tcpat_compile_pat(_tmp2E4,swc->pattern);
# 1179
struct _tuple19*_tmp578;struct _tuple19*rhs=(_tmp578=_region_malloc(_tmp2E4,
sizeof(*_tmp578)),((_tmp578->f1=0,((_tmp578->f2=(swc->pattern)->loc,_tmp578)))));
# 1180
void*sp;
# 1181
if(swc->where_clause != 0){
# 1182
{struct Cyc_List_List*_tmp57B;struct Cyc_List_List*_tmp57A;sp=Cyc_Tcpat_tuple_pat(
_tmp2E4,((_tmp57A=_region_malloc(_tmp2E4,sizeof(*_tmp57A)),((_tmp57A->hd=sp0,((
_tmp57A->tl=(
# 1183
(_tmp57B=_region_malloc(_tmp2E4,sizeof(*_tmp57B)),((_tmp57B->hd=Cyc_Tcpat_int_pat(
_tmp2E4,*_tmp2E5,0),((_tmp57B->tl=0,_tmp57B)))))),_tmp57A)))))),0);}
# 1184
*_tmp2E5=*_tmp2E5 + 1;}else{
# 1186
struct Cyc_List_List*_tmp585;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp584;
struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp583;struct Cyc_List_List*_tmp582;
sp=Cyc_Tcpat_tuple_pat(_tmp2E4,((_tmp582=_region_malloc(_tmp2E4,sizeof(*_tmp582)),((
_tmp582->hd=sp0,((_tmp582->tl=((_tmp585=_region_malloc(_tmp2E4,sizeof(*_tmp585)),((
_tmp585->hd=(void*)((_tmp583=_region_malloc(_tmp2E4,sizeof(*_tmp583)),((_tmp583[
0]=((_tmp584.tag=0,_tmp584)),_tmp583)))),((_tmp585->tl=0,_tmp585)))))),_tmp582)))))),
0);}{
# 1187
struct _tuple22*_tmp586;return(_tmp586=_region_malloc(_tmp2E4,sizeof(*_tmp586)),((
_tmp586->f1=sp,((_tmp586->f2=rhs,_tmp586)))));};};}char Cyc_Tcpat_Desc2string[12]="Desc2string";
struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1192
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1194
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct
Cyc_List_List*);
# 1195
static struct _dyneither_ptr Cyc_Tcpat_desc2string(struct _RegionHandle*r,void*d){
# 1196
void*_tmp2EF=d;struct Cyc_Tcpat_Con_s*_tmp2F1;struct Cyc_List_List*_tmp2F2;struct
Cyc_Set_Set*_tmp2F4;_LL146: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2F0=(
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2EF;if(_tmp2F0->tag != 0)goto
_LL148;else{_tmp2F1=_tmp2F0->f1;_tmp2F2=_tmp2F0->f2;}}_LL147: {
# 1198
union Cyc_Tcpat_Name_value _tmp2F5=_tmp2F1->name;
# 1199
struct Cyc_Absyn_Pat*_tmp2F6=_tmp2F1->orig_pat;
# 1200
if(_tmp2F6 == 0)
# 1201
return Cyc_Tcpat_desc2string(r,(void*)((struct Cyc_List_List*)_check_null(_tmp2F2))->hd);{
# 1203
void*_tmp2F7=_tmp2F6->r;struct Cyc_Absyn_Vardecl*_tmp2FA;struct Cyc_Absyn_Vardecl*
_tmp2FC;struct Cyc_Absyn_Tvar*_tmp2FE;struct Cyc_Absyn_Vardecl*_tmp2FF;struct Cyc_Absyn_AggrInfo*
_tmp303;struct Cyc_Absyn_AggrInfo _tmp304;union Cyc_Absyn_AggrInfoU _tmp305;struct
Cyc_Absyn_Aggrdecl**_tmp306;struct Cyc_Absyn_Aggrdecl*_tmp307;struct Cyc_List_List*
_tmp308;struct Cyc_Absyn_Datatypefield*_tmp30A;int _tmp30D;char _tmp30F;struct
_dyneither_ptr _tmp311;int _tmp312;struct Cyc_Absyn_Enumfield*_tmp314;struct Cyc_Absyn_Enumfield*
_tmp316;struct Cyc_Absyn_Exp*_tmp318;_LL14B: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*
_tmp2F8=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp2F7;if(_tmp2F8->tag != 
0)goto _LL14D;}_LL14C: {
# 1204
const char*_tmp587;return(_tmp587="_",_tag_dyneither(_tmp587,sizeof(char),2));}
_LL14D: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp2F9=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)
_tmp2F7;if(_tmp2F9->tag != 1)goto _LL14F;else{_tmp2FA=_tmp2F9->f1;}}_LL14E:
# 1205
 return Cyc_Absynpp_qvar2string(_tmp2FA->name);_LL14F: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*
_tmp2FB=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp2F7;if(_tmp2FB->tag
!= 2)goto _LL151;else{_tmp2FC=_tmp2FB->f1;}}_LL150: {
# 1206
const char*_tmp58B;void*_tmp58A[1];struct Cyc_String_pa_PrintArg_struct _tmp589;
return(struct _dyneither_ptr)((_tmp589.tag=0,((_tmp589.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2FC->name)),((_tmp58A[0]=&
_tmp589,Cyc_aprintf(((_tmp58B="*%s",_tag_dyneither(_tmp58B,sizeof(char),4))),
_tag_dyneither(_tmp58A,sizeof(void*),1))))))));}_LL151: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*
_tmp2FD=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp2F7;if(_tmp2FD->tag
!= 3)goto _LL153;else{_tmp2FE=_tmp2FD->f1;_tmp2FF=_tmp2FD->f2;}}_LL152: {
# 1207
const char*_tmp590;void*_tmp58F[2];struct Cyc_String_pa_PrintArg_struct _tmp58E;
struct Cyc_String_pa_PrintArg_struct _tmp58D;return(struct _dyneither_ptr)((_tmp58D.tag=
0,((_tmp58D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2FE->name),((
_tmp58E.tag=0,((_tmp58E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp2FF->name)),((_tmp58F[0]=& _tmp58E,((_tmp58F[1]=& _tmp58D,Cyc_aprintf(((
_tmp590="%s<`%s>",_tag_dyneither(_tmp590,sizeof(char),8))),_tag_dyneither(
_tmp58F,sizeof(void*),2))))))))))))));}_LL153: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*
_tmp300=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp2F7;if(_tmp300->tag != 
4)goto _LL155;}_LL154: {
# 1209
const char*_tmp594;void*_tmp593[1];struct Cyc_String_pa_PrintArg_struct _tmp592;
return(struct _dyneither_ptr)((_tmp592.tag=0,((_tmp592.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2F2)),((_tmp593[0]=& _tmp592,
Cyc_aprintf(((_tmp594="$(%s)",_tag_dyneither(_tmp594,sizeof(char),6))),
_tag_dyneither(_tmp593,sizeof(void*),1))))))));}_LL155: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*
_tmp301=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp2F7;if(_tmp301->tag
!= 5)goto _LL157;}_LL156: {
# 1211
const char*_tmp598;void*_tmp597[1];struct Cyc_String_pa_PrintArg_struct _tmp596;
return(struct _dyneither_ptr)((_tmp596.tag=0,((_tmp596.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2F2)),((_tmp597[0]=& _tmp596,
Cyc_aprintf(((_tmp598="&%s",_tag_dyneither(_tmp598,sizeof(char),4))),
_tag_dyneither(_tmp597,sizeof(void*),1))))))));}_LL157: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*
_tmp302=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2F7;if(_tmp302->tag != 
6)goto _LL159;else{_tmp303=_tmp302->f1;if(_tmp303 == 0)goto _LL159;_tmp304=*_tmp303;
_tmp305=_tmp304.aggr_info;if((_tmp305.KnownAggr).tag != 2)goto _LL159;_tmp306=(
struct Cyc_Absyn_Aggrdecl**)(_tmp305.KnownAggr).val;_tmp307=*_tmp306;_tmp308=
_tmp302->f3;}}_LL158:
# 1213
 if(_tmp307->kind == Cyc_Absyn_UnionA){
# 1214
struct Cyc_List_List*_tmp327=_tmp308;struct Cyc_List_List _tmp328;struct _tuple15*
_tmp329;struct _tuple15 _tmp32A;struct Cyc_List_List*_tmp32B;struct Cyc_List_List
_tmp32C;void*_tmp32D;struct _dyneither_ptr*_tmp32F;_LL16C: if(_tmp327 == 0)goto
_LL16E;_tmp328=*_tmp327;_tmp329=(struct _tuple15*)_tmp328.hd;_tmp32A=*_tmp329;
_tmp32B=_tmp32A.f1;if(_tmp32B == 0)goto _LL16E;_tmp32C=*_tmp32B;_tmp32D=(void*)
_tmp32C.hd;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp32E=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)
_tmp32D;if(_tmp32E->tag != 1)goto _LL16E;else{_tmp32F=_tmp32E->f1;}};_LL16D: {
# 1216
const char*_tmp59E;void*_tmp59D[3];struct Cyc_String_pa_PrintArg_struct _tmp59C;
struct Cyc_String_pa_PrintArg_struct _tmp59B;struct Cyc_String_pa_PrintArg_struct
_tmp59A;return(struct _dyneither_ptr)((_tmp59A.tag=0,((_tmp59A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)
# 1217
Cyc_Tcpat_descs2string(r,_tmp2F2)),((_tmp59B.tag=0,((_tmp59B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp32F),((_tmp59C.tag=0,((_tmp59C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)
# 1216
Cyc_Absynpp_qvar2string(_tmp307->name)),((_tmp59D[0]=& _tmp59C,((_tmp59D[1]=&
_tmp59B,((_tmp59D[2]=& _tmp59A,Cyc_aprintf(((_tmp59E="%s{.%s = %s}",
_tag_dyneither(_tmp59E,sizeof(char),13))),_tag_dyneither(_tmp59D,sizeof(void*),3))))))))))))))))))));}
_LL16E:;_LL16F:
# 1218
 goto _LL16B;_LL16B:;}{
# 1221
const char*_tmp5A3;void*_tmp5A2[2];struct Cyc_String_pa_PrintArg_struct _tmp5A1;
struct Cyc_String_pa_PrintArg_struct _tmp5A0;return(struct _dyneither_ptr)((_tmp5A0.tag=
0,((_tmp5A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(
r,_tmp2F2)),((_tmp5A1.tag=0,((_tmp5A1.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp307->name)),((_tmp5A2[0]=& _tmp5A1,((
_tmp5A2[1]=& _tmp5A0,Cyc_aprintf(((_tmp5A3="%s{%s}",_tag_dyneither(_tmp5A3,
sizeof(char),7))),_tag_dyneither(_tmp5A2,sizeof(void*),2))))))))))))));};_LL159: {
struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp309=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)
_tmp2F7;if(_tmp309->tag != 7)goto _LL15B;else{_tmp30A=_tmp309->f2;}}_LL15A:
# 1223
 if(_tmp2F2 == 0){
# 1224
const char*_tmp5A7;void*_tmp5A6[1];struct Cyc_String_pa_PrintArg_struct _tmp5A5;
return(struct _dyneither_ptr)((_tmp5A5.tag=0,((_tmp5A5.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp30A->name)),((_tmp5A6[0]=&
_tmp5A5,Cyc_aprintf(((_tmp5A7="%s",_tag_dyneither(_tmp5A7,sizeof(char),3))),
_tag_dyneither(_tmp5A6,sizeof(void*),1))))))));}else{
# 1226
const char*_tmp5AC;void*_tmp5AB[2];struct Cyc_String_pa_PrintArg_struct _tmp5AA;
struct Cyc_String_pa_PrintArg_struct _tmp5A9;return(struct _dyneither_ptr)((_tmp5A9.tag=
0,((_tmp5A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(
r,_tmp2F2)),((_tmp5AA.tag=0,((_tmp5AA.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp30A->name)),((_tmp5AB[0]=& _tmp5AA,((
_tmp5AB[1]=& _tmp5A9,Cyc_aprintf(((_tmp5AC="%s(%s)",_tag_dyneither(_tmp5AC,
sizeof(char),7))),_tag_dyneither(_tmp5AB,sizeof(void*),2))))))))))))));}_LL15B: {
struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp30B=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)
_tmp2F7;if(_tmp30B->tag != 8)goto _LL15D;}_LL15C: {
# 1227
const char*_tmp5AD;return(_tmp5AD="NULL",_tag_dyneither(_tmp5AD,sizeof(char),5));}
_LL15D: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp30C=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)
_tmp2F7;if(_tmp30C->tag != 9)goto _LL15F;else{_tmp30D=_tmp30C->f2;}}_LL15E: {
# 1228
const char*_tmp5B1;void*_tmp5B0[1];struct Cyc_Int_pa_PrintArg_struct _tmp5AF;
return(struct _dyneither_ptr)((_tmp5AF.tag=1,((_tmp5AF.f1=(unsigned long)_tmp30D,((
_tmp5B0[0]=& _tmp5AF,Cyc_aprintf(((_tmp5B1="%d",_tag_dyneither(_tmp5B1,sizeof(
char),3))),_tag_dyneither(_tmp5B0,sizeof(void*),1))))))));}_LL15F: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*
_tmp30E=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp2F7;if(_tmp30E->tag != 
10)goto _LL161;else{_tmp30F=_tmp30E->f1;}}_LL160: {
# 1229
const char*_tmp5B5;void*_tmp5B4[1];struct Cyc_Int_pa_PrintArg_struct _tmp5B3;
return(struct _dyneither_ptr)((_tmp5B3.tag=1,((_tmp5B3.f1=(unsigned long)((int)
_tmp30F),((_tmp5B4[0]=& _tmp5B3,Cyc_aprintf(((_tmp5B5="%d",_tag_dyneither(_tmp5B5,
sizeof(char),3))),_tag_dyneither(_tmp5B4,sizeof(void*),1))))))));}_LL161: {
struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp310=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)
_tmp2F7;if(_tmp310->tag != 11)goto _LL163;else{_tmp311=_tmp310->f1;_tmp312=_tmp310->f2;}}
_LL162:
# 1230
 return _tmp311;_LL163: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp313=(
struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp2F7;if(_tmp313->tag != 12)goto
_LL165;else{_tmp314=_tmp313->f2;}}_LL164:
# 1231
 _tmp316=_tmp314;goto _LL166;_LL165: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*
_tmp315=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp2F7;if(_tmp315->tag
!= 13)goto _LL167;else{_tmp316=_tmp315->f2;}}_LL166:
# 1232
 return Cyc_Absynpp_qvar2string(_tmp316->name);_LL167: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*
_tmp317=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp2F7;if(_tmp317->tag != 
16)goto _LL169;else{_tmp318=_tmp317->f1;}}_LL168:
# 1233
 return Cyc_Absynpp_exp2string(_tmp318);_LL169:;_LL16A:
# 1234
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL14A:;};}_LL148: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*
_tmp2F3=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2EF;if(_tmp2F3->tag != 1)
goto _LL145;else{_tmp2F4=_tmp2F3->f1;}}_LL149:
# 1237
 if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp2F4)){const char*_tmp5B6;
return(_tmp5B6="_",_tag_dyneither(_tmp5B6,sizeof(char),2));}{
# 1238
struct Cyc_Tcpat_Con_s*_tmp348=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))
Cyc_Set_choose)(_tmp2F4);
# 1239
struct Cyc_Absyn_Pat*_tmp349=_tmp348->orig_pat;
# 1242
if(_tmp349 == 0)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
# 1243
void*_tmp34A=Cyc_Tcutil_compress((void*)_check_null(_tmp349->topt));enum Cyc_Absyn_Size_of
_tmp34C;struct Cyc_Absyn_PtrInfo _tmp34F;struct Cyc_Absyn_PtrAtts _tmp350;struct Cyc_Absyn_DatatypeInfo
_tmp352;union Cyc_Absyn_DatatypeInfoU _tmp353;struct Cyc_Absyn_Datatypedecl**
_tmp354;struct Cyc_Absyn_Datatypedecl*_tmp355;struct Cyc_Absyn_AggrInfo _tmp357;
union Cyc_Absyn_AggrInfoU _tmp358;struct Cyc_Absyn_Aggrdecl**_tmp359;struct Cyc_Absyn_Aggrdecl*
_tmp35A;_LL171: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp34B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)
_tmp34A;if(_tmp34B->tag != 6)goto _LL173;else{_tmp34C=_tmp34B->f2;if(_tmp34C != Cyc_Absyn_Char_sz)
goto _LL173;}}_LL172:
# 1246
{int i=0;for(0;i < 256;++ i){
# 1247
struct Cyc_Tcpat_Con_s*_tmp35B=Cyc_Tcpat_char_con(r,(char)i,(struct Cyc_Absyn_Pat*)
_tmp349);
# 1248
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp2F4,_tmp35B)){
# 1249
const char*_tmp5BA;void*_tmp5B9[1];struct Cyc_Int_pa_PrintArg_struct _tmp5B8;
return(struct _dyneither_ptr)((_tmp5B8.tag=1,((_tmp5B8.f1=(unsigned long)i,((
_tmp5B9[0]=& _tmp5B8,Cyc_aprintf(((_tmp5BA="%d",_tag_dyneither(_tmp5BA,sizeof(
char),3))),_tag_dyneither(_tmp5B9,sizeof(void*),1))))))));}}}
# 1251
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL173: {struct Cyc_Absyn_IntType_Absyn_Type_struct*
_tmp34D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp34A;if(_tmp34D->tag != 6)
goto _LL175;}_LL174:
# 1254
{unsigned int i=0;for(0;i < 0 - 1;++ i){
# 1255
struct Cyc_Tcpat_Con_s*_tmp35F=Cyc_Tcpat_int_con(r,(int)i,_tmp349);
# 1256
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp2F4,_tmp35F)){
# 1257
const char*_tmp5BE;void*_tmp5BD[1];struct Cyc_Int_pa_PrintArg_struct _tmp5BC;
return(struct _dyneither_ptr)((_tmp5BC.tag=1,((_tmp5BC.f1=(unsigned long)((int)i),((
_tmp5BD[0]=& _tmp5BC,Cyc_aprintf(((_tmp5BE="%d",_tag_dyneither(_tmp5BE,sizeof(
char),3))),_tag_dyneither(_tmp5BD,sizeof(void*),1))))))));}}}
# 1259
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL175: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*
_tmp34E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34A;if(_tmp34E->tag
!= 5)goto _LL177;else{_tmp34F=_tmp34E->f1;_tmp350=_tmp34F.ptr_atts;}}_LL176: {
# 1261
union Cyc_Absyn_Constraint*_tmp363=_tmp350.nullable;
# 1262
int is_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp363);
# 1263
if(is_nullable){
# 1264
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp2F4,Cyc_Tcpat_null_con(r,(struct Cyc_Absyn_Pat*)_tmp349))){
# 1265
const char*_tmp5BF;return(_tmp5BF="NULL",_tag_dyneither(_tmp5BF,sizeof(char),5));}}{
# 1267
const char*_tmp5C0;return(_tmp5C0="&_",_tag_dyneither(_tmp5C0,sizeof(char),3));};}
_LL177: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp351=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)
_tmp34A;if(_tmp351->tag != 3)goto _LL179;else{_tmp352=_tmp351->f1;_tmp353=_tmp352.datatype_info;
if((_tmp353.KnownDatatype).tag != 2)goto _LL179;_tmp354=(struct Cyc_Absyn_Datatypedecl**)(
_tmp353.KnownDatatype).val;_tmp355=*_tmp354;}}_LL178:
# 1269
 if(_tmp355->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
# 1270
struct Cyc_List_List*_tmp366=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp355->fields))->v;
# 1271
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp366);
# 1272
for(0;(unsigned int)_tmp366;_tmp366=_tmp366->tl){
# 1273
struct _dyneither_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)_tmp366->hd)->name).f2;
# 1274
struct Cyc_List_List*_tmp367=((struct Cyc_Absyn_Datatypefield*)_tmp366->hd)->typs;
# 1275
struct Cyc_Tcpat_Con_s*_tmp5C1;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_member)(_tmp2F4,(struct Cyc_Tcpat_Con_s*)((_tmp5C1=_cycalloc(sizeof(*
_tmp5C1)),((_tmp5C1->name=Cyc_Tcpat_Name_v(n),((_tmp5C1->arity=0,((_tmp5C1->span=
0,((_tmp5C1->orig_pat=_tmp349,_tmp5C1)))))))))))){
# 1276
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp367)== 0)
# 1277
return n;else{
# 1279
const char*_tmp5C5;void*_tmp5C4[1];struct Cyc_String_pa_PrintArg_struct _tmp5C3;
return(struct _dyneither_ptr)((_tmp5C3.tag=0,((_tmp5C3.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)n),((_tmp5C4[0]=& _tmp5C3,Cyc_aprintf(((_tmp5C5="%s(...)",
_tag_dyneither(_tmp5C5,sizeof(char),8))),_tag_dyneither(_tmp5C4,sizeof(void*),1))))))));}}}
# 1282
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};_LL179:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*
_tmp356=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp34A;if(_tmp356->tag != 
11)goto _LL17B;else{_tmp357=_tmp356->f1;_tmp358=_tmp357.aggr_info;if((_tmp358.KnownAggr).tag
!= 2)goto _LL17B;_tmp359=(struct Cyc_Absyn_Aggrdecl**)(_tmp358.KnownAggr).val;
_tmp35A=*_tmp359;}}if(!(_tmp35A->kind == Cyc_Absyn_UnionA))goto _LL17B;_LL17A: {
# 1284
struct Cyc_List_List*_tmp36C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp35A->impl))->fields;
# 1285
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp36C);
# 1286
struct _tuple1 _tmp36E;struct _dyneither_ptr*_tmp36F;struct _dyneither_ptr _tmp370;
struct _tuple1*_tmp36D=_tmp35A->name;_tmp36E=*_tmp36D;_tmp36F=_tmp36E.f2;_tmp370=*
_tmp36F;
# 1287
for(0;(unsigned int)_tmp36C;_tmp36C=_tmp36C->tl){
# 1288
struct _dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp36C->hd)->name;
# 1289
struct Cyc_Tcpat_Con_s*_tmp5C6;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_member)(_tmp2F4,(struct Cyc_Tcpat_Con_s*)((_tmp5C6=_cycalloc(sizeof(*
_tmp5C6)),((_tmp5C6->name=Cyc_Tcpat_Name_v(n),((_tmp5C6->arity=0,((_tmp5C6->span=
0,((_tmp5C6->orig_pat=_tmp349,_tmp5C6)))))))))))){
# 1290
const char*_tmp5CB;void*_tmp5CA[2];struct Cyc_String_pa_PrintArg_struct _tmp5C9;
struct Cyc_String_pa_PrintArg_struct _tmp5C8;return(struct _dyneither_ptr)((_tmp5C8.tag=
0,((_tmp5C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp5C9.tag=0,((
_tmp5C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp370),((_tmp5CA[0]=&
_tmp5C9,((_tmp5CA[1]=& _tmp5C8,Cyc_aprintf(((_tmp5CB="%s{.%s = _}",_tag_dyneither(
_tmp5CB,sizeof(char),12))),_tag_dyneither(_tmp5CA,sizeof(void*),2))))))))))))));}}
# 1292
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL17B:;_LL17C:
# 1293
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL170:;};};_LL145:;}
# 1297
static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(struct _RegionHandle*r,void*
d){
# 1298
struct _dyneither_ptr*_tmp5CC;return(_tmp5CC=_cycalloc(sizeof(*_tmp5CC)),((
_tmp5CC[0]=Cyc_Tcpat_desc2string(r,d),_tmp5CC)));}
# 1300
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct
Cyc_List_List*ds){
# 1301
struct Cyc_List_List*_tmp377=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr*(*f)(struct _RegionHandle*,void*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_desc2stringptr,r,ds);
# 1302
struct _dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct
_dyneither_ptr)),",",sizeof(char),2);
# 1303
{struct Cyc_List_List*_tmp378=_tmp377;for(0;_tmp378 != 0;_tmp378=((struct Cyc_List_List*)
_check_null(_tmp378))->tl){
# 1304
if(_tmp378->tl != 0){
# 1305
{struct Cyc_List_List*_tmp5CD;_tmp378->tl=(struct Cyc_List_List*)((_tmp5CD=
_cycalloc(sizeof(*_tmp5CD)),((_tmp5CD->hd=comma,((_tmp5CD->tl=_tmp378->tl,
_tmp5CD))))));}
# 1306
_tmp378=_tmp378->tl;}}}
# 1309
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp377);}
# 1312
static void Cyc_Tcpat_not_exhaust_err(struct _RegionHandle*r,unsigned int loc,void*
desc){
# 1313
struct _handler_cons _tmp37B;_push_handler(& _tmp37B);{int _tmp37D=0;if(setjmp(
_tmp37B.handler))_tmp37D=1;if(!_tmp37D){
# 1314
{struct _dyneither_ptr _tmp37E=Cyc_Tcpat_desc2string(r,desc);
# 1315
const char*_tmp5D1;void*_tmp5D0[1];struct Cyc_String_pa_PrintArg_struct _tmp5CF;(
_tmp5CF.tag=0,((_tmp5CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp37E),((
_tmp5D0[0]=& _tmp5CF,Cyc_Tcutil_terr(loc,((_tmp5D1="patterns may not be exhaustive.\n\tmissing case for %s",
_tag_dyneither(_tmp5D1,sizeof(char),53))),_tag_dyneither(_tmp5D0,sizeof(void*),1)))))));}
# 1314
;_pop_handler();}else{void*_tmp37C=(void*)_exn_thrown;void*_tmp383=_tmp37C;
_LL17E: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp384=(struct Cyc_Tcpat_Desc2string_exn_struct*)
_tmp383;if(_tmp384->tag != Cyc_Tcpat_Desc2string)goto _LL180;}_LL17F:
# 1318
{const char*_tmp5D4;void*_tmp5D3;(_tmp5D3=0,Cyc_Tcutil_terr(loc,((_tmp5D4="patterns may not be exhaustive.",
_tag_dyneither(_tmp5D4,sizeof(char),32))),_tag_dyneither(_tmp5D3,sizeof(void*),0)));}
# 1319
goto _LL17D;_LL180:;_LL181:(void)_throw(_tmp383);_LL17D:;}};}
# 1322
static void Cyc_Tcpat_rule_occurs(int dummy,struct _tuple19*rhs){
# 1323
(*rhs).f1=1;}
# 1326
void Cyc_Tcpat_check_switch_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*swcs){
# 1331
struct _RegionHandle _tmp387=_new_region("r");struct _RegionHandle*r=& _tmp387;
_push_region(r);
# 1332
{int _tmp388=0;
# 1333
struct _tuple23 _tmp5D5;struct _tuple23 _tmp389=(_tmp5D5.f1=r,((_tmp5D5.f2=& _tmp388,
_tmp5D5)));
# 1334
struct Cyc_List_List*_tmp38A=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple22*(*f)(struct _tuple23*,struct Cyc_Absyn_Switch_clause*),struct
_tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match,&
_tmp389,swcs);
# 1335
void*_tmp38B=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))
Cyc_Tcpat_match_compile)(r,_tmp38A);
# 1336
int _tmp38C=0;
# 1337
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,
unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple19*),int
env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp38B,Cyc_Tcpat_not_exhaust_err,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp38C);
# 1339
for(0;_tmp38A != 0;_tmp38A=_tmp38A->tl){
# 1340
struct _tuple22 _tmp38E;struct _tuple19*_tmp38F;struct _tuple19 _tmp390;int _tmp391;
unsigned int _tmp392;struct _tuple22*_tmp38D=(struct _tuple22*)_tmp38A->hd;_tmp38E=*
_tmp38D;_tmp38F=_tmp38E.f2;_tmp390=*_tmp38F;_tmp391=_tmp390.f1;_tmp392=_tmp390.f2;
# 1341
if(!_tmp391){
# 1342
{const char*_tmp5D8;void*_tmp5D7;(_tmp5D7=0,Cyc_Tcutil_terr(_tmp392,((_tmp5D8="redundant pattern (check for misspelled constructors in earlier patterns)",
_tag_dyneither(_tmp5D8,sizeof(char),74))),_tag_dyneither(_tmp5D7,sizeof(void*),0)));}
# 1344
break;}}}
# 1332
;_pop_region(r);}
# 1354
static void Cyc_Tcpat_not_exhaust_warn(struct _RegionHandle*r,struct _tuple14*pr,
void*desc){
# 1355
(*pr).f2=0;{
# 1356
struct _handler_cons _tmp396;_push_handler(& _tmp396);{int _tmp398=0;if(setjmp(
_tmp396.handler))_tmp398=1;if(!_tmp398){
# 1357
{struct _dyneither_ptr _tmp399=Cyc_Tcpat_desc2string(r,desc);
# 1358
const char*_tmp5DC;void*_tmp5DB[1];struct Cyc_String_pa_PrintArg_struct _tmp5DA;(
_tmp5DA.tag=0,((_tmp5DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp399),((
_tmp5DB[0]=& _tmp5DA,Cyc_Tcutil_warn((*pr).f1,((_tmp5DC="pattern not exhaustive.\n\tmissing case for %s",
_tag_dyneither(_tmp5DC,sizeof(char),45))),_tag_dyneither(_tmp5DB,sizeof(void*),1)))))));}
# 1357
;_pop_handler();}else{void*_tmp397=(void*)_exn_thrown;void*_tmp39E=_tmp397;
_LL183: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp39F=(struct Cyc_Tcpat_Desc2string_exn_struct*)
_tmp39E;if(_tmp39F->tag != Cyc_Tcpat_Desc2string)goto _LL185;}_LL184:
# 1361
{const char*_tmp5DF;void*_tmp5DE;(_tmp5DE=0,Cyc_Tcutil_warn((*pr).f1,((_tmp5DF="pattern not exhaustive.",
_tag_dyneither(_tmp5DF,sizeof(char),24))),_tag_dyneither(_tmp5DE,sizeof(void*),0)));}
# 1362
goto _LL182;_LL185:;_LL186:(void)_throw(_tmp39E);_LL182:;}};};}
# 1365
static void Cyc_Tcpat_dummy_fn(int i,int j){
# 1366
return;}struct _tuple24{void*f1;int f2;};
# 1368
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Pat*p){
# 1369
struct _RegionHandle _tmp3A2=_new_region("r");struct _RegionHandle*r=& _tmp3A2;
_push_region(r);
# 1370
{struct _tuple24*_tmp5E2;struct Cyc_List_List*_tmp5E1;struct Cyc_List_List*_tmp3A3=(
_tmp5E1=_region_malloc(r,sizeof(*_tmp5E1)),((_tmp5E1->hd=((_tmp5E2=
_region_malloc(r,sizeof(*_tmp5E2)),((_tmp5E2->f1=Cyc_Tcpat_compile_pat(r,p),((
_tmp5E2->f2=0,_tmp5E2)))))),((_tmp5E1->tl=0,_tmp5E1)))));
# 1371
void*_tmp3A4=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))
Cyc_Tcpat_match_compile)(r,_tmp3A3);
# 1372
struct _tuple14 _tmp5E3;struct _tuple14 _tmp3A5=(_tmp5E3.f1=loc,((_tmp5E3.f2=1,
_tmp5E3)));
# 1373
int _tmp3A6=0;
# 1374
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,
struct _tuple14*,void*),struct _tuple14*env1,void(*rhs_appears)(int,int),int env2,
int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp3A4,Cyc_Tcpat_not_exhaust_warn,&
_tmp3A5,Cyc_Tcpat_dummy_fn,0,& _tmp3A6);{
# 1376
int _tmp3A7=_tmp3A5.f2;_npop_handler(0);return _tmp3A7;};}
# 1370
;_pop_region(r);}
# 1384
static struct _tuple22*Cyc_Tcpat_get_match2(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
swc){
# 1386
void*sp0=Cyc_Tcpat_compile_pat(r,swc->pattern);
# 1387
struct _tuple19*_tmp5E4;struct _tuple19*rhs=(_tmp5E4=_region_malloc(r,sizeof(*
_tmp5E4)),((_tmp5E4->f1=0,((_tmp5E4->f2=(swc->pattern)->loc,_tmp5E4)))));
# 1388
struct _tuple22*_tmp5E5;return(_tmp5E5=_region_malloc(r,sizeof(*_tmp5E5)),((
_tmp5E5->f1=sp0,((_tmp5E5->f2=rhs,_tmp5E5)))));}
# 1390
static void Cyc_Tcpat_not_exhaust_err2(struct _RegionHandle*r,unsigned int loc,void*
d){;}
# 1393
void Cyc_Tcpat_check_catch_overlap(unsigned int loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*swcs){
# 1395
struct _RegionHandle _tmp3AD=_new_region("r");struct _RegionHandle*r=& _tmp3AD;
_push_region(r);
# 1396
{struct Cyc_List_List*_tmp3AE=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple22*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match2,
r,swcs);
# 1397
void*_tmp3AF=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))
Cyc_Tcpat_match_compile)(r,_tmp3AE);
# 1398
int _tmp3B0=0;
# 1399
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,
unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple19*),int
env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp3AF,Cyc_Tcpat_not_exhaust_err2,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp3B0);
# 1401
for(0;_tmp3AE != 0;_tmp3AE=_tmp3AE->tl){
# 1402
struct _tuple22 _tmp3B2;struct _tuple19*_tmp3B3;struct _tuple19 _tmp3B4;int _tmp3B5;
unsigned int _tmp3B6;struct _tuple22*_tmp3B1=(struct _tuple22*)_tmp3AE->hd;_tmp3B2=*
_tmp3B1;_tmp3B3=_tmp3B2.f2;_tmp3B4=*_tmp3B3;_tmp3B5=_tmp3B4.f1;_tmp3B6=_tmp3B4.f2;
# 1403
if(!_tmp3B5){
# 1404
{const char*_tmp5E8;void*_tmp5E7;(_tmp5E7=0,Cyc_Tcutil_terr(_tmp3B6,((_tmp5E8="redundant pattern (check for misspelled constructors in earlier patterns)",
_tag_dyneither(_tmp5E8,sizeof(char),74))),_tag_dyneither(_tmp5E7,sizeof(void*),0)));}
# 1405
break;}}}
# 1396
;_pop_region(r);}
