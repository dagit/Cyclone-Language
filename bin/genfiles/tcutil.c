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
typedef struct Cyc___cycFILE Cyc_FILE;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 120 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 131
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 70
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 94
struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 121
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 149
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 183
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 193
struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*,struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 230
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 249
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 253
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 259
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 286
struct _tuple1 Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);
# 305
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 319
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 366
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;
# 41
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);
# 42
struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
# 56
struct Cyc_Position_Error*Cyc_Position_mk_err_elab(unsigned int,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 62
extern int Cyc_Position_num_errors;
# 63
extern int Cyc_Position_max_errors;
# 64
void Cyc_Position_post_error(struct Cyc_Position_Error*);
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple2*Cyc_Absyn_qvar_t;typedef struct _tuple2*Cyc_Absyn_qvar_opt_t;
# 100
typedef struct _tuple2*Cyc_Absyn_typedef_name_t;
# 101
typedef struct _tuple2*Cyc_Absyn_typedef_name_opt_t;
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 321
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
# 322
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 340
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 352
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);
# 353
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 431
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
# 432
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
# 433
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 507
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 522
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
# 533
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 540
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 558
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 904 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 906
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 914
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 917
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
# 918
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 919
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
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
# 932
void*Cyc_Absyn_compress_kb(void*);
# 937
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 942
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 944
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
# 948
extern void*Cyc_Absyn_empty_effect;
# 958
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
# 959
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 967
extern void*Cyc_Absyn_bounds_one;
# 972
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 998
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 999
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
# 1000
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple2*,void*,unsigned int);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
# 1037
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1043
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1045
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1046
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1050
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,unsigned int);
# 1051
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1060
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1100
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,unsigned int);
# 1101
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1145
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1147
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1149
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1151
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1153
int Cyc_Absyn_fntype_att(void*a);struct _tuple13{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1157
struct _tuple13 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1159
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 64 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 66
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 67
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);
# 68
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
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
# 123 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*,int is_use);
# 124
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 125
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 127
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 128
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 130
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 137
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 138
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 204
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
# 50
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);struct _tuple14{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 42
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 45
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 44
void Cyc_Tcutil_flush_warnings();
# 48
extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;
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
# 64
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
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
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);struct _tuple15{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 81
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 83
extern int Cyc_Tcutil_warn_alias_coerce;
# 86
extern int Cyc_Tcutil_warn_region_coerce;
# 89
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 90
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
# 91
int Cyc_Tcutil_is_function_type(void*t);
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
# 99
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 100
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
# 101
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 102
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
# 103
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
# 104
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 106
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
# 107
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
# 108
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
# 109
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 111
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
# 112
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
# 113
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
# 114
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 116
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 117
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
# 118
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 119
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
# 120
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
# 121
extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
# 123
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 124
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
# 125
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;
# 126
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 128
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
# 129
extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
# 130
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;
# 131
extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
# 133
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
# 134
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 135
int Cyc_Tcutil_unify_kindbound(void*,void*);struct _tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 137
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 142
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 144
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 148
void Cyc_Tcutil_explain_failure();
# 150
int Cyc_Tcutil_unify(void*,void*);
# 152
int Cyc_Tcutil_typecmp(void*,void*);
# 153
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 155
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 158
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 162
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 164
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 168
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple17{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
# 169
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*,struct Cyc_Absyn_Tvar*);
# 174
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 178
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
# 204 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 206
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 214
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 217
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
# 218
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 224
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 226
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 228
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 230
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 232
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 238
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 240
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 242
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 246
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type);
# 252
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 257
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 260
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 265
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 270
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 275
void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);struct _tuple18{int f1;void*f2;};
# 279
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 287
void*Cyc_Tcutil_normalize_effect(void*e);
# 290
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 292
int Cyc_Tcutil_new_tvar_id();
# 294
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
# 295
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 297
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 299
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 302
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 306
int Cyc_Tcutil_bits_only(void*t);
# 309
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 312
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 316
int Cyc_Tcutil_supports_default(void*);
# 320
int Cyc_Tcutil_admits_zero(void*t);
# 323
int Cyc_Tcutil_is_noreturn(void*);
# 327
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 331
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 335
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 338
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
# 40
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 41 "tc.h"
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);
# 42
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
# 43
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Enumdecl*);
# 25 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 27
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};char Cyc_Tcutil_Unify[6]="Unify";struct Cyc_Tcutil_Unify_exn_struct{char*tag;};
# 46 "tcutil.cyc"
struct Cyc_Tcutil_Unify_exn_struct Cyc_Tcutil_Unify_val={Cyc_Tcutil_Unify};
# 48
void Cyc_Tcutil_unify_it(void*t1,void*t2);
# 52
int Cyc_Tcutil_warn_region_coerce=0;
# 55
void*Cyc_Tcutil_t1_failure=(void*)& Cyc_Absyn_VoidType_val;
# 56
int Cyc_Tcutil_tq1_const=0;
# 57
void*Cyc_Tcutil_t2_failure=(void*)& Cyc_Absyn_VoidType_val;
# 58
int Cyc_Tcutil_tq2_const=0;
# 60
struct _dyneither_ptr Cyc_Tcutil_failure_reason={(void*)0,(void*)0,(void*)(0 + 0)};
# 64
void Cyc_Tcutil_explain_failure(){
# 65
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;
# 66
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
# 67
const char*_tmp1074;void*_tmp1073[2];const char*_tmp1072;const char*_tmp1071;struct Cyc_String_pa_PrintArg_struct _tmp1070;struct Cyc_String_pa_PrintArg_struct _tmp106F;struct _dyneither_ptr s1=(struct _dyneither_ptr)((_tmp106F.tag=0,((_tmp106F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 68
Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure)),((_tmp1070.tag=0,((_tmp1070.f1=(struct _dyneither_ptr)(
# 67
Cyc_Tcutil_tq1_const?(_tmp1071="const ",_tag_dyneither(_tmp1071,sizeof(char),7)):((_tmp1072="",_tag_dyneither(_tmp1072,sizeof(char),1)))),((_tmp1073[0]=& _tmp1070,((_tmp1073[1]=& _tmp106F,Cyc_aprintf(((_tmp1074="%s%s",_tag_dyneither(_tmp1074,sizeof(char),5))),_tag_dyneither(_tmp1073,sizeof(void*),2))))))))))))));
# 69
const char*_tmp107D;void*_tmp107C[2];const char*_tmp107B;const char*_tmp107A;struct Cyc_String_pa_PrintArg_struct _tmp1079;struct Cyc_String_pa_PrintArg_struct _tmp1078;struct _dyneither_ptr s2=(struct _dyneither_ptr)((_tmp1078.tag=0,((_tmp1078.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 70
Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure)),((_tmp1079.tag=0,((_tmp1079.f1=(struct _dyneither_ptr)(
# 69
Cyc_Tcutil_tq2_const?(_tmp107A="const ",_tag_dyneither(_tmp107A,sizeof(char),7)):((_tmp107B="",_tag_dyneither(_tmp107B,sizeof(char),1)))),((_tmp107C[0]=& _tmp1079,((_tmp107C[1]=& _tmp1078,Cyc_aprintf(((_tmp107D="%s%s",_tag_dyneither(_tmp107D,sizeof(char),5))),_tag_dyneither(_tmp107C,sizeof(void*),2))))))))))))));
# 71
int pos=2;
# 72
{const char*_tmp1081;void*_tmp1080[1];struct Cyc_String_pa_PrintArg_struct _tmp107F;(_tmp107F.tag=0,((_tmp107F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp1080[0]=& _tmp107F,Cyc_fprintf(Cyc_stderr,((_tmp1081="  %s",_tag_dyneither(_tmp1081,sizeof(char),5))),_tag_dyneither(_tmp1080,sizeof(void*),1)))))));}
# 73
pos +=_get_dyneither_size(s1,sizeof(char));
# 74
if(pos + 5 >= 80){
# 75
{const char*_tmp1084;void*_tmp1083;(_tmp1083=0,Cyc_fprintf(Cyc_stderr,((_tmp1084="\n\t",_tag_dyneither(_tmp1084,sizeof(char),3))),_tag_dyneither(_tmp1083,sizeof(void*),0)));}
# 76
pos=8;}else{
# 78
{const char*_tmp1087;void*_tmp1086;(_tmp1086=0,Cyc_fprintf(Cyc_stderr,((_tmp1087=" ",_tag_dyneither(_tmp1087,sizeof(char),2))),_tag_dyneither(_tmp1086,sizeof(void*),0)));}
# 79
++ pos;}
# 81
{const char*_tmp108A;void*_tmp1089;(_tmp1089=0,Cyc_fprintf(Cyc_stderr,((_tmp108A="and ",_tag_dyneither(_tmp108A,sizeof(char),5))),_tag_dyneither(_tmp1089,sizeof(void*),0)));}
# 82
pos +=4;
# 83
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
# 84
{const char*_tmp108D;void*_tmp108C;(_tmp108C=0,Cyc_fprintf(Cyc_stderr,((_tmp108D="\n\t",_tag_dyneither(_tmp108D,sizeof(char),3))),_tag_dyneither(_tmp108C,sizeof(void*),0)));}
# 85
pos=8;}
# 87
{const char*_tmp1091;void*_tmp1090[1];struct Cyc_String_pa_PrintArg_struct _tmp108F;(_tmp108F.tag=0,((_tmp108F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmp1090[0]=& _tmp108F,Cyc_fprintf(Cyc_stderr,((_tmp1091="%s ",_tag_dyneither(_tmp1091,sizeof(char),4))),_tag_dyneither(_tmp1090,sizeof(void*),1)))))));}
# 88
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
# 89
if(pos + 17 >= 80){
# 90
{const char*_tmp1094;void*_tmp1093;(_tmp1093=0,Cyc_fprintf(Cyc_stderr,((_tmp1094="\n\t",_tag_dyneither(_tmp1094,sizeof(char),3))),_tag_dyneither(_tmp1093,sizeof(void*),0)));}
# 91
pos=8;}
# 93
{const char*_tmp1097;void*_tmp1096;(_tmp1096=0,Cyc_fprintf(Cyc_stderr,((_tmp1097="are not compatible. ",_tag_dyneither(_tmp1097,sizeof(char),21))),_tag_dyneither(_tmp1096,sizeof(void*),0)));}
# 94
pos +=17;
# 95
if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
# 96
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){
# 97
const char*_tmp109A;void*_tmp1099;(_tmp1099=0,Cyc_fprintf(Cyc_stderr,((_tmp109A="\n\t",_tag_dyneither(_tmp109A,sizeof(char),3))),_tag_dyneither(_tmp1099,sizeof(void*),0)));}{
# 99
const char*_tmp109E;void*_tmp109D[1];struct Cyc_String_pa_PrintArg_struct _tmp109C;(_tmp109C.tag=0,((_tmp109C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmp109D[0]=& _tmp109C,Cyc_fprintf(Cyc_stderr,((_tmp109E="%s",_tag_dyneither(_tmp109E,sizeof(char),3))),_tag_dyneither(_tmp109D,sizeof(void*),1)))))));};}
# 101
{const char*_tmp10A1;void*_tmp10A0;(_tmp10A0=0,Cyc_fprintf(Cyc_stderr,((_tmp10A1="\n",_tag_dyneither(_tmp10A1,sizeof(char),2))),_tag_dyneither(_tmp10A0,sizeof(void*),0)));}
# 102
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);};}
# 105
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 108
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 111
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 114
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
# 115
{const char*_tmp10A5;void*_tmp10A4[1];struct Cyc_String_pa_PrintArg_struct _tmp10A3;(_tmp10A3.tag=0,((_tmp10A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp10A4[0]=& _tmp10A3,Cyc_fprintf(Cyc_stderr,((_tmp10A5="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(_tmp10A5,sizeof(char),36))),_tag_dyneither(_tmp10A4,sizeof(void*),1)))))));}
# 116
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
# 117
struct Cyc_Core_Impossible_exn_struct _tmp10A8;struct Cyc_Core_Impossible_exn_struct*_tmp10A7;(int)_throw((void*)((_tmp10A7=_cycalloc(sizeof(*_tmp10A7)),((_tmp10A7[0]=((_tmp10A8.tag=Cyc_Core_Impossible,((_tmp10A8.f1=msg,_tmp10A8)))),_tmp10A7)))));};}
# 120
static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){
# 121
return*tv->name;}
# 124
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){
# 125
for(0;tvs != 0;tvs=tvs->tl){
# 126
const char*_tmp10AD;void*_tmp10AC[2];struct Cyc_String_pa_PrintArg_struct _tmp10AB;struct Cyc_String_pa_PrintArg_struct _tmp10AA;(_tmp10AA.tag=0,((_tmp10AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 127
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmp10AB.tag=0,((_tmp10AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmp10AC[0]=& _tmp10AB,((_tmp10AC[1]=& _tmp10AA,Cyc_fprintf(Cyc_stderr,((_tmp10AD="%s::%s ",_tag_dyneither(_tmp10AD,sizeof(char),8))),_tag_dyneither(_tmp10AC,sizeof(void*),2)))))))))))));}
# 128
{const char*_tmp10B0;void*_tmp10AF;(_tmp10AF=0,Cyc_fprintf(Cyc_stderr,((_tmp10B0="\n",_tag_dyneither(_tmp10B0,sizeof(char),2))),_tag_dyneither(_tmp10AF,sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}
# 131
static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
# 132
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;
# 136
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 139
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
# 140
{struct Cyc_List_List*_tmp10B1;Cyc_Tcutil_warning_segs=((_tmp10B1=_cycalloc(sizeof(*_tmp10B1)),((_tmp10B1->hd=(void*)sg,((_tmp10B1->tl=Cyc_Tcutil_warning_segs,_tmp10B1))))));}{
# 141
struct _dyneither_ptr*_tmp10B4;struct Cyc_List_List*_tmp10B3;Cyc_Tcutil_warning_msgs=((_tmp10B3=_cycalloc(sizeof(*_tmp10B3)),((_tmp10B3->hd=((_tmp10B4=_cycalloc(sizeof(*_tmp10B4)),((_tmp10B4[0]=msg,_tmp10B4)))),((_tmp10B3->tl=Cyc_Tcutil_warning_msgs,_tmp10B3))))));};}
# 143
void Cyc_Tcutil_flush_warnings(){
# 144
if(Cyc_Tcutil_warning_segs == 0)
# 145
return;
# 146
{const char*_tmp10B7;void*_tmp10B6;(_tmp10B6=0,Cyc_fprintf(Cyc_stderr,((_tmp10B7="***Warnings***\n",_tag_dyneither(_tmp10B7,sizeof(char),16))),_tag_dyneither(_tmp10B6,sizeof(void*),0)));}{
# 147
struct Cyc_List_List*_tmp36=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
# 148
Cyc_Tcutil_warning_segs=0;
# 149
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
# 150
while(Cyc_Tcutil_warning_msgs != 0){
# 151
{const char*_tmp10BC;void*_tmp10BB[2];struct Cyc_String_pa_PrintArg_struct _tmp10BA;struct Cyc_String_pa_PrintArg_struct _tmp10B9;(_tmp10B9.tag=0,((_tmp10B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmp10BA.tag=0,((_tmp10BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp36))->hd)),((_tmp10BB[0]=& _tmp10BA,((_tmp10BB[1]=& _tmp10B9,Cyc_fprintf(Cyc_stderr,((_tmp10BC="%s: %s\n",_tag_dyneither(_tmp10BC,sizeof(char),8))),_tag_dyneither(_tmp10BB,sizeof(void*),2)))))))))))));}
# 152
_tmp36=_tmp36->tl;
# 153
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}
# 155
{const char*_tmp10BF;void*_tmp10BE;(_tmp10BE=0,Cyc_fprintf(Cyc_stderr,((_tmp10BF="**************\n",_tag_dyneither(_tmp10BF,sizeof(char),16))),_tag_dyneither(_tmp10BE,sizeof(void*),0)));}
# 156
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);};}
# 160
struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=0;
# 164
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
# 165
return tv1->identity - tv2->identity;}
# 169
void*Cyc_Tcutil_compress(void*t){
# 170
void*_tmp3D=t;void*_tmp3F;void*_tmp41;void*_tmp43;void**_tmp44;void*_tmp46;void**_tmp47;struct Cyc_Absyn_Exp*_tmp49;void**_tmp4B;void*_tmp4C;_LL1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D;if(_tmp3E->tag != 1)goto _LL3;else{_tmp3F=(void*)_tmp3E->f2;if(_tmp3F != 0)goto _LL3;}}_LL2:
# 171
 goto _LL4;_LL3: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp40=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3D;if(_tmp40->tag != 17)goto _LL5;else{_tmp41=(void*)_tmp40->f4;if(_tmp41 != 0)goto _LL5;}}_LL4:
# 172
 return t;_LL5: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp42=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3D;if(_tmp42->tag != 17)goto _LL7;else{_tmp43=(void**)& _tmp42->f4;_tmp44=(void**)((void**)& _tmp42->f4);}}_LL6: {
# 174
void*ta=(void*)_check_null(*_tmp44);
# 175
void*t2=Cyc_Tcutil_compress(ta);
# 176
if(t2 != ta)
# 177
*_tmp44=(void*)t2;
# 178
return t2;}_LL7: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp45=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D;if(_tmp45->tag != 1)goto _LL9;else{_tmp46=(void**)& _tmp45->f2;_tmp47=(void**)((void**)& _tmp45->f2);}}_LL8: {
# 180
void*ta=(void*)_check_null(*_tmp47);
# 181
void*t2=Cyc_Tcutil_compress(ta);
# 182
if(t2 != ta)
# 183
*_tmp47=(void*)t2;
# 184
return t2;}_LL9: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp48=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3D;if(_tmp48->tag != 18)goto _LLB;else{_tmp49=_tmp48->f1;}}_LLA:
# 186
 Cyc_Evexp_eval_const_uint_exp(_tmp49);{
# 187
void*_tmp4D=_tmp49->r;void*_tmp4F;_LL10: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp4E=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4D;if(_tmp4E->tag != 37)goto _LL12;else{_tmp4F=(void*)_tmp4E->f1;}}_LL11:
# 188
 return Cyc_Tcutil_compress(_tmp4F);_LL12:;_LL13:
# 189
 return t;_LLF:;};_LLB: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp4A=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp3D;if(_tmp4A->tag != 26)goto _LLD;else{_tmp4B=_tmp4A->f2;if(_tmp4B == 0)goto _LLD;_tmp4C=*_tmp4B;}}_LLC:
# 192
 return Cyc_Tcutil_compress(_tmp4C);_LLD:;_LLE:
# 193
 return t;_LL0:;}
# 200
void*Cyc_Tcutil_copy_type(void*t);
# 201
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
# 202
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 204
static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(union Cyc_Absyn_Constraint*cptr){
# 205
union Cyc_Absyn_Constraint*_tmp50=cptr;union Cyc_Absyn_Constraint _tmp51;int _tmp52;union Cyc_Absyn_Constraint _tmp53;void*_tmp54;union Cyc_Absyn_Constraint _tmp55;union Cyc_Absyn_Constraint*_tmp56;_LL15: _tmp51=*_tmp50;if((_tmp51.No_constr).tag != 3)goto _LL17;_tmp52=(int)(_tmp51.No_constr).val;_LL16:
# 206
 return Cyc_Absyn_empty_conref();_LL17: _tmp53=*_tmp50;if((_tmp53.Eq_constr).tag != 1)goto _LL19;_tmp54=(void*)(_tmp53.Eq_constr).val;_LL18:
# 207
 return Cyc_Absyn_new_conref(_tmp54);_LL19: _tmp55=*_tmp50;if((_tmp55.Forward_constr).tag != 2)goto _LL14;_tmp56=(union Cyc_Absyn_Constraint*)(_tmp55.Forward_constr).val;_LL1A:
# 208
 return Cyc_Tcutil_copy_conref(_tmp56);_LL14:;}
# 211
static void*Cyc_Tcutil_copy_kindbound(void*kb){
# 212
void*_tmp57=Cyc_Absyn_compress_kb(kb);struct Cyc_Absyn_Kind*_tmp5A;_LL1C: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp58=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp57;if(_tmp58->tag != 1)goto _LL1E;}_LL1D: {
# 213
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp10C2;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp10C1;return(void*)((_tmp10C1=_cycalloc(sizeof(*_tmp10C1)),((_tmp10C1[0]=((_tmp10C2.tag=1,((_tmp10C2.f1=0,_tmp10C2)))),_tmp10C1))));}_LL1E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp59=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp57;if(_tmp59->tag != 2)goto _LL20;else{_tmp5A=_tmp59->f2;}}_LL1F: {
# 214
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp10C5;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp10C4;return(void*)((_tmp10C4=_cycalloc(sizeof(*_tmp10C4)),((_tmp10C4[0]=((_tmp10C5.tag=2,((_tmp10C5.f1=0,((_tmp10C5.f2=_tmp5A,_tmp10C5)))))),_tmp10C4))));}_LL20:;_LL21:
# 215
 return kb;_LL1B:;}
# 218
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 220
struct Cyc_Absyn_Tvar*_tmp10C6;return(_tmp10C6=_cycalloc(sizeof(*_tmp10C6)),((_tmp10C6->name=tv->name,((_tmp10C6->identity=- 1,((_tmp10C6->kind=Cyc_Tcutil_copy_kindbound(tv->kind),_tmp10C6)))))));}
# 222
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 224
struct _tuple10 _tmp61;struct _dyneither_ptr*_tmp62;struct Cyc_Absyn_Tqual _tmp63;void*_tmp64;struct _tuple10*_tmp60=arg;_tmp61=*_tmp60;_tmp62=_tmp61.f1;_tmp63=_tmp61.f2;_tmp64=_tmp61.f3;{
# 225
struct _tuple10*_tmp10C7;return(_tmp10C7=_cycalloc(sizeof(*_tmp10C7)),((_tmp10C7->f1=_tmp62,((_tmp10C7->f2=_tmp63,((_tmp10C7->f3=Cyc_Tcutil_copy_type(_tmp64),_tmp10C7)))))));};}
# 227
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
# 228
struct _tuple12 _tmp67;struct Cyc_Absyn_Tqual _tmp68;void*_tmp69;struct _tuple12*_tmp66=arg;_tmp67=*_tmp66;_tmp68=_tmp67.f1;_tmp69=_tmp67.f2;{
# 229
struct _tuple12*_tmp10C8;return(_tmp10C8=_cycalloc(sizeof(*_tmp10C8)),((_tmp10C8->f1=_tmp68,((_tmp10C8->f2=Cyc_Tcutil_copy_type(_tmp69),_tmp10C8)))));};}
# 231
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
# 232
struct Cyc_Absyn_Aggrfield*_tmp10C9;return(_tmp10C9=_cycalloc(sizeof(*_tmp10C9)),((_tmp10C9->name=f->name,((_tmp10C9->tq=f->tq,((_tmp10C9->type=Cyc_Tcutil_copy_type(f->type),((_tmp10C9->width=f->width,((_tmp10C9->attributes=f->attributes,_tmp10C9)))))))))));}
# 234
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
# 235
struct _tuple0 _tmp6D;void*_tmp6E;void*_tmp6F;struct _tuple0*_tmp6C=x;_tmp6D=*_tmp6C;_tmp6E=_tmp6D.f1;_tmp6F=_tmp6D.f2;{
# 236
struct _tuple0*_tmp10CA;return(_tmp10CA=_cycalloc(sizeof(*_tmp10CA)),((_tmp10CA->f1=Cyc_Tcutil_copy_type(_tmp6E),((_tmp10CA->f2=Cyc_Tcutil_copy_type(_tmp6F),_tmp10CA)))));};}
# 238
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
# 239
struct Cyc_Absyn_Enumfield*_tmp10CB;return(_tmp10CB=_cycalloc(sizeof(*_tmp10CB)),((_tmp10CB->name=f->name,((_tmp10CB->tag=f->tag,((_tmp10CB->loc=f->loc,_tmp10CB)))))));}
# 241
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
# 242
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10CE;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp10CD;return(void*)((_tmp10CD=_cycalloc(sizeof(*_tmp10CD)),((_tmp10CD[0]=((_tmp10CE.tag=2,((_tmp10CE.f1=Cyc_Tcutil_copy_tvar(t),_tmp10CE)))),_tmp10CD))));}
# 244
void*Cyc_Tcutil_copy_type(void*t){
# 245
void*_tmp74=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp78;struct Cyc_Absyn_DatatypeInfo _tmp7A;union Cyc_Absyn_DatatypeInfoU _tmp7B;struct Cyc_List_List*_tmp7C;struct Cyc_Absyn_DatatypeFieldInfo _tmp7E;union Cyc_Absyn_DatatypeFieldInfoU _tmp7F;struct Cyc_List_List*_tmp80;struct Cyc_Absyn_PtrInfo _tmp82;void*_tmp83;struct Cyc_Absyn_Tqual _tmp84;struct Cyc_Absyn_PtrAtts _tmp85;void*_tmp86;union Cyc_Absyn_Constraint*_tmp87;union Cyc_Absyn_Constraint*_tmp88;union Cyc_Absyn_Constraint*_tmp89;struct Cyc_Absyn_PtrLoc*_tmp8A;struct Cyc_Absyn_ArrayInfo _tmp8E;void*_tmp8F;struct Cyc_Absyn_Tqual _tmp90;struct Cyc_Absyn_Exp*_tmp91;union Cyc_Absyn_Constraint*_tmp92;unsigned int _tmp93;struct Cyc_Absyn_FnInfo _tmp95;struct Cyc_List_List*_tmp96;void*_tmp97;struct Cyc_Absyn_Tqual _tmp98;void*_tmp99;struct Cyc_List_List*_tmp9A;int _tmp9B;struct Cyc_Absyn_VarargInfo*_tmp9C;struct Cyc_List_List*_tmp9D;struct Cyc_List_List*_tmp9E;struct Cyc_List_List*_tmpA0;struct Cyc_Absyn_AggrInfo _tmpA2;union Cyc_Absyn_AggrInfoU _tmpA3;struct _tuple4 _tmpA4;enum Cyc_Absyn_AggrKind _tmpA5;struct _tuple2*_tmpA6;struct Cyc_Core_Opt*_tmpA7;struct Cyc_List_List*_tmpA8;struct Cyc_Absyn_AggrInfo _tmpAA;union Cyc_Absyn_AggrInfoU _tmpAB;struct Cyc_Absyn_Aggrdecl**_tmpAC;struct Cyc_List_List*_tmpAD;enum Cyc_Absyn_AggrKind _tmpAF;struct Cyc_List_List*_tmpB0;struct _tuple2*_tmpB2;struct Cyc_Absyn_Enumdecl*_tmpB3;struct Cyc_List_List*_tmpB5;void*_tmpB7;struct Cyc_Absyn_Exp*_tmpB9;void*_tmpBB;void*_tmpBD;void*_tmpBE;struct _tuple2*_tmpC0;struct Cyc_List_List*_tmpC1;struct Cyc_Absyn_Typedefdecl*_tmpC2;void*_tmpC7;struct Cyc_List_List*_tmpC9;void*_tmpCB;struct Cyc_Absyn_TypeDecl*_tmpCD;struct Cyc_Absyn_TypeDecl _tmpCE;void*_tmpCF;struct Cyc_Absyn_Aggrdecl*_tmpD1;struct Cyc_Absyn_TypeDecl*_tmpD3;struct Cyc_Absyn_TypeDecl _tmpD4;void*_tmpD5;struct Cyc_Absyn_Enumdecl*_tmpD7;struct Cyc_Absyn_TypeDecl*_tmpD9;struct Cyc_Absyn_TypeDecl _tmpDA;void*_tmpDB;struct Cyc_Absyn_Datatypedecl*_tmpDD;_LL23: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp75=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp74;if(_tmp75->tag != 0)goto _LL25;}_LL24:
# 246
 goto _LL26;_LL25: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp76=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp74;if(_tmp76->tag != 1)goto _LL27;}_LL26:
# 247
 return t;_LL27: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp77=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp74;if(_tmp77->tag != 2)goto _LL29;else{_tmp78=_tmp77->f1;}}_LL28: {
# 248
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10D1;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp10D0;return(void*)((_tmp10D0=_cycalloc(sizeof(*_tmp10D0)),((_tmp10D0[0]=((_tmp10D1.tag=2,((_tmp10D1.f1=Cyc_Tcutil_copy_tvar(_tmp78),_tmp10D1)))),_tmp10D0))));}_LL29: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp79=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp74;if(_tmp79->tag != 3)goto _LL2B;else{_tmp7A=_tmp79->f1;_tmp7B=_tmp7A.datatype_info;_tmp7C=_tmp7A.targs;}}_LL2A: {
# 250
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp10D7;struct Cyc_Absyn_DatatypeInfo _tmp10D6;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp10D5;return(void*)((_tmp10D5=_cycalloc(sizeof(*_tmp10D5)),((_tmp10D5[0]=((_tmp10D7.tag=3,((_tmp10D7.f1=((_tmp10D6.datatype_info=_tmp7B,((_tmp10D6.targs=Cyc_Tcutil_copy_types(_tmp7C),_tmp10D6)))),_tmp10D7)))),_tmp10D5))));}_LL2B: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp7D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp74;if(_tmp7D->tag != 4)goto _LL2D;else{_tmp7E=_tmp7D->f1;_tmp7F=_tmp7E.field_info;_tmp80=_tmp7E.targs;}}_LL2C: {
# 252
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp10DD;struct Cyc_Absyn_DatatypeFieldInfo _tmp10DC;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp10DB;return(void*)((_tmp10DB=_cycalloc(sizeof(*_tmp10DB)),((_tmp10DB[0]=((_tmp10DD.tag=4,((_tmp10DD.f1=((_tmp10DC.field_info=_tmp7F,((_tmp10DC.targs=Cyc_Tcutil_copy_types(_tmp80),_tmp10DC)))),_tmp10DD)))),_tmp10DB))));}_LL2D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp81=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp74;if(_tmp81->tag != 5)goto _LL2F;else{_tmp82=_tmp81->f1;_tmp83=_tmp82.elt_typ;_tmp84=_tmp82.elt_tq;_tmp85=_tmp82.ptr_atts;_tmp86=_tmp85.rgn;_tmp87=_tmp85.nullable;_tmp88=_tmp85.bounds;_tmp89=_tmp85.zero_term;_tmp8A=_tmp85.ptrloc;}}_LL2E: {
# 254
void*_tmpE6=Cyc_Tcutil_copy_type(_tmp83);
# 255
void*_tmpE7=Cyc_Tcutil_copy_type(_tmp86);
# 256
union Cyc_Absyn_Constraint*_tmpE8=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp87);
# 257
struct Cyc_Absyn_Tqual _tmpE9=_tmp84;
# 258
union Cyc_Absyn_Constraint*_tmpEA=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp88);
# 259
union Cyc_Absyn_Constraint*_tmpEB=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp89);
# 260
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp10E7;struct Cyc_Absyn_PtrAtts _tmp10E6;struct Cyc_Absyn_PtrInfo _tmp10E5;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp10E4;return(void*)((_tmp10E4=_cycalloc(sizeof(*_tmp10E4)),((_tmp10E4[0]=((_tmp10E7.tag=5,((_tmp10E7.f1=((_tmp10E5.elt_typ=_tmpE6,((_tmp10E5.elt_tq=_tmpE9,((_tmp10E5.ptr_atts=((_tmp10E6.rgn=_tmpE7,((_tmp10E6.nullable=_tmpE8,((_tmp10E6.bounds=_tmpEA,((_tmp10E6.zero_term=_tmpEB,((_tmp10E6.ptrloc=_tmp8A,_tmp10E6)))))))))),_tmp10E5)))))),_tmp10E7)))),_tmp10E4))));}_LL2F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74;if(_tmp8B->tag != 6)goto _LL31;}_LL30:
# 261
 goto _LL32;_LL31: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp8C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74;if(_tmp8C->tag != 7)goto _LL33;}_LL32:
# 262
 return t;_LL33: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp74;if(_tmp8D->tag != 8)goto _LL35;else{_tmp8E=_tmp8D->f1;_tmp8F=_tmp8E.elt_type;_tmp90=_tmp8E.tq;_tmp91=_tmp8E.num_elts;_tmp92=_tmp8E.zero_term;_tmp93=_tmp8E.zt_loc;}}_LL34: {
# 265
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp10ED;struct Cyc_Absyn_ArrayInfo _tmp10EC;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp10EB;return(void*)((_tmp10EB=_cycalloc(sizeof(*_tmp10EB)),((_tmp10EB[0]=((_tmp10ED.tag=8,((_tmp10ED.f1=((_tmp10EC.elt_type=Cyc_Tcutil_copy_type(_tmp8F),((_tmp10EC.tq=_tmp90,((_tmp10EC.num_elts=_tmp91,((_tmp10EC.zero_term=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp92),((_tmp10EC.zt_loc=_tmp93,_tmp10EC)))))))))),_tmp10ED)))),_tmp10EB))));}_LL35: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp94=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp74;if(_tmp94->tag != 9)goto _LL37;else{_tmp95=_tmp94->f1;_tmp96=_tmp95.tvars;_tmp97=_tmp95.effect;_tmp98=_tmp95.ret_tqual;_tmp99=_tmp95.ret_typ;_tmp9A=_tmp95.args;_tmp9B=_tmp95.c_varargs;_tmp9C=_tmp95.cyc_varargs;_tmp9D=_tmp95.rgn_po;_tmp9E=_tmp95.attributes;}}_LL36: {
# 267
struct Cyc_List_List*_tmpF3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp96);
# 268
void*effopt2=_tmp97 == 0?0:(void*)Cyc_Tcutil_copy_type((void*)_tmp97);
# 269
void*_tmpF4=Cyc_Tcutil_copy_type(_tmp99);
# 270
struct Cyc_List_List*_tmpF5=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp9A);
# 271
int _tmpF6=_tmp9B;
# 272
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
# 273
if(_tmp9C != 0){
# 274
struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)_tmp9C;
# 275
struct Cyc_Absyn_VarargInfo*_tmp10EE;cyc_varargs2=((_tmp10EE=_cycalloc(sizeof(*_tmp10EE)),((_tmp10EE->name=cv->name,((_tmp10EE->tq=cv->tq,((_tmp10EE->type=Cyc_Tcutil_copy_type(cv->type),((_tmp10EE->inject=cv->inject,_tmp10EE))))))))));}{
# 278
struct Cyc_List_List*_tmpF8=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp9D);
# 279
struct Cyc_List_List*_tmpF9=_tmp9E;
# 280
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp10F4;struct Cyc_Absyn_FnInfo _tmp10F3;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp10F2;return(void*)((_tmp10F2=_cycalloc(sizeof(*_tmp10F2)),((_tmp10F2[0]=((_tmp10F4.tag=9,((_tmp10F4.f1=((_tmp10F3.tvars=_tmpF3,((_tmp10F3.effect=effopt2,((_tmp10F3.ret_tqual=_tmp98,((_tmp10F3.ret_typ=_tmpF4,((_tmp10F3.args=_tmpF5,((_tmp10F3.c_varargs=_tmpF6,((_tmp10F3.cyc_varargs=cyc_varargs2,((_tmp10F3.rgn_po=_tmpF8,((_tmp10F3.attributes=_tmpF9,_tmp10F3)))))))))))))))))),_tmp10F4)))),_tmp10F2))));};}_LL37: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp9F=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp74;if(_tmp9F->tag != 10)goto _LL39;else{_tmpA0=_tmp9F->f1;}}_LL38: {
# 282
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp10F7;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp10F6;return(void*)((_tmp10F6=_cycalloc(sizeof(*_tmp10F6)),((_tmp10F6[0]=((_tmp10F7.tag=10,((_tmp10F7.f1=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpA0),_tmp10F7)))),_tmp10F6))));}_LL39: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpA1=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp74;if(_tmpA1->tag != 11)goto _LL3B;else{_tmpA2=_tmpA1->f1;_tmpA3=_tmpA2.aggr_info;if((_tmpA3.UnknownAggr).tag != 1)goto _LL3B;_tmpA4=(struct _tuple4)(_tmpA3.UnknownAggr).val;_tmpA5=_tmpA4.f1;_tmpA6=_tmpA4.f2;_tmpA7=_tmpA4.f3;_tmpA8=_tmpA2.targs;}}_LL3A: {
# 284
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp10FD;struct Cyc_Absyn_AggrInfo _tmp10FC;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp10FB;return(void*)((_tmp10FB=_cycalloc(sizeof(*_tmp10FB)),((_tmp10FB[0]=((_tmp10FD.tag=11,((_tmp10FD.f1=((_tmp10FC.aggr_info=Cyc_Absyn_UnknownAggr(_tmpA5,_tmpA6,_tmpA7),((_tmp10FC.targs=Cyc_Tcutil_copy_types(_tmpA8),_tmp10FC)))),_tmp10FD)))),_tmp10FB))));}_LL3B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpA9=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp74;if(_tmpA9->tag != 11)goto _LL3D;else{_tmpAA=_tmpA9->f1;_tmpAB=_tmpAA.aggr_info;if((_tmpAB.KnownAggr).tag != 2)goto _LL3D;_tmpAC=(struct Cyc_Absyn_Aggrdecl**)(_tmpAB.KnownAggr).val;_tmpAD=_tmpAA.targs;}}_LL3C: {
# 286
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp1103;struct Cyc_Absyn_AggrInfo _tmp1102;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1101;return(void*)((_tmp1101=_cycalloc(sizeof(*_tmp1101)),((_tmp1101[0]=((_tmp1103.tag=11,((_tmp1103.f1=((_tmp1102.aggr_info=Cyc_Absyn_KnownAggr(_tmpAC),((_tmp1102.targs=Cyc_Tcutil_copy_types(_tmpAD),_tmp1102)))),_tmp1103)))),_tmp1101))));}_LL3D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAE=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp74;if(_tmpAE->tag != 12)goto _LL3F;else{_tmpAF=_tmpAE->f1;_tmpB0=_tmpAE->f2;}}_LL3E: {
# 287
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp1106;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1105;return(void*)((_tmp1105=_cycalloc(sizeof(*_tmp1105)),((_tmp1105[0]=((_tmp1106.tag=12,((_tmp1106.f1=_tmpAF,((_tmp1106.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpB0),_tmp1106)))))),_tmp1105))));}_LL3F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpB1=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp74;if(_tmpB1->tag != 13)goto _LL41;else{_tmpB2=_tmpB1->f1;_tmpB3=_tmpB1->f2;}}_LL40: {
# 288
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp1109;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp1108;return(void*)((_tmp1108=_cycalloc(sizeof(*_tmp1108)),((_tmp1108[0]=((_tmp1109.tag=13,((_tmp1109.f1=_tmpB2,((_tmp1109.f2=_tmpB3,_tmp1109)))))),_tmp1108))));}_LL41: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpB4=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp74;if(_tmpB4->tag != 14)goto _LL43;else{_tmpB5=_tmpB4->f1;}}_LL42: {
# 289
struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmp110C;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp110B;return(void*)((_tmp110B=_cycalloc(sizeof(*_tmp110B)),((_tmp110B[0]=((_tmp110C.tag=14,((_tmp110C.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpB5),_tmp110C)))),_tmp110B))));}_LL43: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB6=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp74;if(_tmpB6->tag != 19)goto _LL45;else{_tmpB7=(void*)_tmpB6->f1;}}_LL44: {
# 290
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp110F;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp110E;return(void*)((_tmp110E=_cycalloc(sizeof(*_tmp110E)),((_tmp110E[0]=((_tmp110F.tag=19,((_tmp110F.f1=(void*)Cyc_Tcutil_copy_type(_tmpB7),_tmp110F)))),_tmp110E))));}_LL45: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpB8=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp74;if(_tmpB8->tag != 18)goto _LL47;else{_tmpB9=_tmpB8->f1;}}_LL46: {
# 293
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp1112;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1111;return(void*)((_tmp1111=_cycalloc(sizeof(*_tmp1111)),((_tmp1111[0]=((_tmp1112.tag=18,((_tmp1112.f1=_tmpB9,_tmp1112)))),_tmp1111))));}_LL47: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpBA=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp74;if(_tmpBA->tag != 15)goto _LL49;else{_tmpBB=(void*)_tmpBA->f1;}}_LL48: {
# 294
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp1115;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp1114;return(void*)((_tmp1114=_cycalloc(sizeof(*_tmp1114)),((_tmp1114[0]=((_tmp1115.tag=15,((_tmp1115.f1=(void*)Cyc_Tcutil_copy_type(_tmpBB),_tmp1115)))),_tmp1114))));}_LL49: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpBC=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp74;if(_tmpBC->tag != 16)goto _LL4B;else{_tmpBD=(void*)_tmpBC->f1;_tmpBE=(void*)_tmpBC->f2;}}_LL4A: {
# 295
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp1118;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp1117;return(void*)((_tmp1117=_cycalloc(sizeof(*_tmp1117)),((_tmp1117[0]=((_tmp1118.tag=16,((_tmp1118.f1=(void*)Cyc_Tcutil_copy_type(_tmpBD),((_tmp1118.f2=(void*)Cyc_Tcutil_copy_type(_tmpBE),_tmp1118)))))),_tmp1117))));}_LL4B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpBF=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp74;if(_tmpBF->tag != 17)goto _LL4D;else{_tmpC0=_tmpBF->f1;_tmpC1=_tmpBF->f2;_tmpC2=_tmpBF->f3;}}_LL4C: {
# 297
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp111B;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp111A;return(void*)((_tmp111A=_cycalloc(sizeof(*_tmp111A)),((_tmp111A[0]=((_tmp111B.tag=17,((_tmp111B.f1=_tmpC0,((_tmp111B.f2=Cyc_Tcutil_copy_types(_tmpC1),((_tmp111B.f3=_tmpC2,((_tmp111B.f4=(void*)0,_tmp111B)))))))))),_tmp111A))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpC3=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp74;if(_tmpC3->tag != 21)goto _LL4F;}_LL4E:
# 298
 goto _LL50;_LL4F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpC4=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp74;if(_tmpC4->tag != 22)goto _LL51;}_LL50:
# 299
 goto _LL52;_LL51: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpC5=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp74;if(_tmpC5->tag != 20)goto _LL53;}_LL52:
# 300
 return t;_LL53: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpC6=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp74;if(_tmpC6->tag != 23)goto _LL55;else{_tmpC7=(void*)_tmpC6->f1;}}_LL54: {
# 301
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp111E;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp111D;return(void*)((_tmp111D=_cycalloc(sizeof(*_tmp111D)),((_tmp111D[0]=((_tmp111E.tag=23,((_tmp111E.f1=(void*)Cyc_Tcutil_copy_type(_tmpC7),_tmp111E)))),_tmp111D))));}_LL55: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC8=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp74;if(_tmpC8->tag != 24)goto _LL57;else{_tmpC9=_tmpC8->f1;}}_LL56: {
# 302
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1121;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1120;return(void*)((_tmp1120=_cycalloc(sizeof(*_tmp1120)),((_tmp1120[0]=((_tmp1121.tag=24,((_tmp1121.f1=Cyc_Tcutil_copy_types(_tmpC9),_tmp1121)))),_tmp1120))));}_LL57: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpCA=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp74;if(_tmpCA->tag != 25)goto _LL59;else{_tmpCB=(void*)_tmpCA->f1;}}_LL58: {
# 303
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp1124;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1123;return(void*)((_tmp1123=_cycalloc(sizeof(*_tmp1123)),((_tmp1123[0]=((_tmp1124.tag=25,((_tmp1124.f1=(void*)Cyc_Tcutil_copy_type(_tmpCB),_tmp1124)))),_tmp1123))));}_LL59: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpCC=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp74;if(_tmpCC->tag != 26)goto _LL5B;else{_tmpCD=_tmpCC->f1;_tmpCE=*_tmpCD;_tmpCF=_tmpCE.r;{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpD0=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmpCF;if(_tmpD0->tag != 0)goto _LL5B;else{_tmpD1=_tmpD0->f1;}};}}_LL5A: {
# 306
struct Cyc_List_List*_tmp11B=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD1->tvs);
# 307
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp112A;struct Cyc_Absyn_AggrInfo _tmp1129;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1128;return(void*)((_tmp1128=_cycalloc(sizeof(*_tmp1128)),((_tmp1128[0]=((_tmp112A.tag=11,((_tmp112A.f1=((_tmp1129.aggr_info=Cyc_Absyn_UnknownAggr(_tmpD1->kind,_tmpD1->name,0),((_tmp1129.targs=_tmp11B,_tmp1129)))),_tmp112A)))),_tmp1128))));}_LL5B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpD2=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp74;if(_tmpD2->tag != 26)goto _LL5D;else{_tmpD3=_tmpD2->f1;_tmpD4=*_tmpD3;_tmpD5=_tmpD4.r;{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpD6=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmpD5;if(_tmpD6->tag != 1)goto _LL5D;else{_tmpD7=_tmpD6->f1;}};}}_LL5C: {
# 309
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp112D;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp112C;return(void*)((_tmp112C=_cycalloc(sizeof(*_tmp112C)),((_tmp112C[0]=((_tmp112D.tag=13,((_tmp112D.f1=_tmpD7->name,((_tmp112D.f2=0,_tmp112D)))))),_tmp112C))));}_LL5D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpD8=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp74;if(_tmpD8->tag != 26)goto _LL22;else{_tmpD9=_tmpD8->f1;_tmpDA=*_tmpD9;_tmpDB=_tmpDA.r;{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpDC=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmpDB;if(_tmpDC->tag != 2)goto _LL22;else{_tmpDD=_tmpDC->f1;}};}}_LL5E: {
# 311
struct Cyc_List_List*_tmp121=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpDD->tvs);
# 312
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp1137;struct Cyc_Absyn_UnknownDatatypeInfo _tmp1136;struct Cyc_Absyn_DatatypeInfo _tmp1135;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1134;return(void*)((_tmp1134=_cycalloc(sizeof(*_tmp1134)),((_tmp1134[0]=((_tmp1137.tag=3,((_tmp1137.f1=((_tmp1135.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmp1136.name=_tmpDD->name,((_tmp1136.is_extensible=0,_tmp1136))))),((_tmp1135.targs=_tmp121,_tmp1135)))),_tmp1137)))),_tmp1134))));}_LL22:;}
# 326 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(void*d){
# 327
void*_tmp126=d;struct Cyc_Absyn_Exp*_tmp128;struct _dyneither_ptr*_tmp12A;_LL60: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp127=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp126;if(_tmp127->tag != 0)goto _LL62;else{_tmp128=_tmp127->f1;}}_LL61: {
# 328
struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmp113A;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp1139;return(void*)((_tmp1139=_cycalloc(sizeof(*_tmp1139)),((_tmp1139[0]=((_tmp113A.tag=0,((_tmp113A.f1=Cyc_Tcutil_deep_copy_exp(_tmp128),_tmp113A)))),_tmp1139))));}_LL62: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp129=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp126;if(_tmp129->tag != 1)goto _LL5F;else{_tmp12A=_tmp129->f1;}}_LL63:
# 329
 return d;_LL5F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 332
static struct _tuple19*Cyc_Tcutil_copy_eds(struct _tuple19*e){
# 334
struct _tuple19*_tmp113B;return(_tmp113B=_cycalloc(sizeof(*_tmp113B)),((_tmp113B->f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_designator,(e[0]).f1),((_tmp113B->f2=Cyc_Tcutil_deep_copy_exp((e[0]).f2),_tmp113B)))));}
# 337
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*e){
# 338
void*_tmp12E=e->r;union Cyc_Absyn_Cnst _tmp130;struct _tuple2*_tmp132;void*_tmp133;enum Cyc_Absyn_Primop _tmp135;struct Cyc_List_List*_tmp136;struct Cyc_Absyn_Exp*_tmp138;struct Cyc_Core_Opt*_tmp139;struct Cyc_Absyn_Exp*_tmp13A;struct Cyc_Absyn_Exp*_tmp13C;enum Cyc_Absyn_Incrementor _tmp13D;struct Cyc_Absyn_Exp*_tmp13F;struct Cyc_Absyn_Exp*_tmp140;struct Cyc_Absyn_Exp*_tmp141;struct Cyc_Absyn_Exp*_tmp143;struct Cyc_Absyn_Exp*_tmp144;struct Cyc_Absyn_Exp*_tmp146;struct Cyc_Absyn_Exp*_tmp147;struct Cyc_Absyn_Exp*_tmp149;struct Cyc_Absyn_Exp*_tmp14A;struct Cyc_Absyn_Exp*_tmp14C;struct Cyc_List_List*_tmp14D;struct Cyc_Absyn_VarargCallInfo*_tmp14E;int _tmp14F;struct Cyc_Absyn_Exp*_tmp151;struct Cyc_Absyn_Exp*_tmp153;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_List_List*_tmp156;void*_tmp158;struct Cyc_Absyn_Exp*_tmp159;int _tmp15A;enum Cyc_Absyn_Coercion _tmp15B;struct Cyc_Absyn_Exp*_tmp15D;struct Cyc_Absyn_Exp*_tmp15F;struct Cyc_Absyn_Exp*_tmp160;void*_tmp162;struct Cyc_Absyn_Exp*_tmp164;void*_tmp166;void*_tmp167;struct Cyc_Absyn_Exp*_tmp169;struct Cyc_Absyn_Exp*_tmp16B;struct _dyneither_ptr*_tmp16C;int _tmp16D;int _tmp16E;struct Cyc_Absyn_Exp*_tmp170;struct _dyneither_ptr*_tmp171;int _tmp172;int _tmp173;struct Cyc_Absyn_Exp*_tmp175;struct Cyc_Absyn_Exp*_tmp176;struct Cyc_List_List*_tmp178;struct _tuple10*_tmp17A;struct _tuple10 _tmp17B;struct _dyneither_ptr*_tmp17C;struct Cyc_Absyn_Tqual _tmp17D;void*_tmp17E;struct Cyc_List_List*_tmp17F;struct Cyc_List_List*_tmp181;struct Cyc_Absyn_Vardecl*_tmp183;struct Cyc_Absyn_Exp*_tmp184;struct Cyc_Absyn_Exp*_tmp185;int _tmp186;struct _tuple2*_tmp188;struct Cyc_List_List*_tmp189;struct Cyc_List_List*_tmp18A;struct Cyc_Absyn_Aggrdecl*_tmp18B;void*_tmp18D;struct Cyc_List_List*_tmp18E;struct Cyc_List_List*_tmp190;struct Cyc_Absyn_Datatypedecl*_tmp191;struct Cyc_Absyn_Datatypefield*_tmp192;struct _tuple2*_tmp194;struct Cyc_Absyn_Enumdecl*_tmp195;struct Cyc_Absyn_Enumfield*_tmp196;struct _tuple2*_tmp198;void*_tmp199;struct Cyc_Absyn_Enumfield*_tmp19A;struct Cyc_Absyn_MallocInfo _tmp19C;int _tmp19D;struct Cyc_Absyn_Exp*_tmp19E;void**_tmp19F;struct Cyc_Absyn_Exp*_tmp1A0;int _tmp1A1;struct Cyc_Absyn_Exp*_tmp1A3;struct Cyc_Absyn_Exp*_tmp1A4;struct Cyc_Core_Opt*_tmp1A6;struct Cyc_List_List*_tmp1A7;struct Cyc_Absyn_Exp*_tmp1AA;struct _dyneither_ptr*_tmp1AB;void*_tmp1AD;int _tmp1AF;struct _dyneither_ptr _tmp1B0;_LL65: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp12F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp12F->tag != 0)goto _LL67;else{_tmp130=_tmp12F->f1;}}_LL66:
# 339
 return Cyc_Absyn_const_exp(_tmp130,e->loc);_LL67: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp131=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp131->tag != 1)goto _LL69;else{_tmp132=_tmp131->f1;_tmp133=(void*)_tmp131->f2;}}_LL68:
# 340
 return Cyc_Absyn_varb_exp(_tmp132,_tmp133,e->loc);_LL69: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp134=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp134->tag != 2)goto _LL6B;else{_tmp135=_tmp134->f1;_tmp136=_tmp134->f2;}}_LL6A:
# 341
 return Cyc_Absyn_primop_exp(_tmp135,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp136),e->loc);_LL6B: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp137=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp137->tag != 3)goto _LL6D;else{_tmp138=_tmp137->f1;_tmp139=_tmp137->f2;_tmp13A=_tmp137->f3;}}_LL6C: {
# 343
struct Cyc_Core_Opt*_tmp113C;return Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmp138),(unsigned int)_tmp139?(_tmp113C=_cycalloc_atomic(sizeof(*_tmp113C)),((_tmp113C->v=(void*)((enum Cyc_Absyn_Primop)_tmp139->v),_tmp113C))): 0,Cyc_Tcutil_deep_copy_exp(_tmp13A),e->loc);}_LL6D: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp13B=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp13B->tag != 4)goto _LL6F;else{_tmp13C=_tmp13B->f1;_tmp13D=_tmp13B->f2;}}_LL6E:
# 344
 return Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmp13C),_tmp13D,e->loc);_LL6F: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp13E=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp13E->tag != 5)goto _LL71;else{_tmp13F=_tmp13E->f1;_tmp140=_tmp13E->f2;_tmp141=_tmp13E->f3;}}_LL70:
# 346
 return Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(_tmp13F),Cyc_Tcutil_deep_copy_exp(_tmp140),Cyc_Tcutil_deep_copy_exp(_tmp141),e->loc);_LL71: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp142=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp142->tag != 6)goto _LL73;else{_tmp143=_tmp142->f1;_tmp144=_tmp142->f2;}}_LL72:
# 347
 return Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmp143),Cyc_Tcutil_deep_copy_exp(_tmp144),e->loc);_LL73: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp145=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp145->tag != 7)goto _LL75;else{_tmp146=_tmp145->f1;_tmp147=_tmp145->f2;}}_LL74:
# 348
 return Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmp146),Cyc_Tcutil_deep_copy_exp(_tmp147),e->loc);_LL75: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp148=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp148->tag != 8)goto _LL77;else{_tmp149=_tmp148->f1;_tmp14A=_tmp148->f2;}}_LL76:
# 349
 return Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmp149),Cyc_Tcutil_deep_copy_exp(_tmp14A),e->loc);_LL77: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp14B=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp14B->tag != 9)goto _LL79;else{_tmp14C=_tmp14B->f1;_tmp14D=_tmp14B->f2;_tmp14E=_tmp14B->f3;_tmp14F=_tmp14B->f4;}}_LL78: {
# 351
struct Cyc_Absyn_VarargCallInfo*_tmp1B2=_tmp14E;struct Cyc_Absyn_VarargCallInfo _tmp1B3;int _tmp1B4;struct Cyc_List_List*_tmp1B5;struct Cyc_Absyn_VarargInfo*_tmp1B6;_LLB4: if(_tmp1B2 == 0)goto _LLB6;_tmp1B3=*_tmp1B2;_tmp1B4=_tmp1B3.num_varargs;_tmp1B5=_tmp1B3.injectors;_tmp1B6=_tmp1B3.vai;_LLB5: {
# 353
struct Cyc_Absyn_VarargInfo _tmp1B8;struct _dyneither_ptr*_tmp1B9;struct Cyc_Absyn_Tqual _tmp1BA;void*_tmp1BB;int _tmp1BC;struct Cyc_Absyn_VarargInfo*_tmp1B7=_tmp1B6;_tmp1B8=*_tmp1B7;_tmp1B9=_tmp1B8.name;_tmp1BA=_tmp1B8.tq;_tmp1BB=_tmp1B8.type;_tmp1BC=_tmp1B8.inject;{
# 354
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp1146;struct Cyc_Absyn_VarargInfo*_tmp1145;struct Cyc_Absyn_VarargCallInfo*_tmp1144;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp1143;return Cyc_Absyn_new_exp((void*)((_tmp1143=_cycalloc(sizeof(*_tmp1143)),((_tmp1143[0]=((_tmp1146.tag=9,((_tmp1146.f1=
# 355
Cyc_Tcutil_deep_copy_exp(_tmp14C),((_tmp1146.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp14D),((_tmp1146.f3=(
# 356
(_tmp1144=_cycalloc(sizeof(*_tmp1144)),((_tmp1144->num_varargs=_tmp1B4,((_tmp1144->injectors=_tmp1B5,((_tmp1144->vai=(
# 357
(_tmp1145=_cycalloc(sizeof(*_tmp1145)),((_tmp1145->name=_tmp1B9,((_tmp1145->tq=_tmp1BA,((_tmp1145->type=Cyc_Tcutil_copy_type(_tmp1BB),((_tmp1145->inject=_tmp1BC,_tmp1145)))))))))),_tmp1144)))))))),((_tmp1146.f4=_tmp14F,_tmp1146)))))))))),_tmp1143)))),e->loc);};}_LLB6:;_LLB7: {
# 360
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp1149;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp1148;return Cyc_Absyn_new_exp((void*)((_tmp1148=_cycalloc(sizeof(*_tmp1148)),((_tmp1148[0]=((_tmp1149.tag=9,((_tmp1149.f1=Cyc_Tcutil_deep_copy_exp(_tmp14C),((_tmp1149.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp14D),((_tmp1149.f3=_tmp14E,((_tmp1149.f4=_tmp14F,_tmp1149)))))))))),_tmp1148)))),e->loc);}_LLB3:;}_LL79: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp150=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp150->tag != 10)goto _LL7B;else{_tmp151=_tmp150->f1;}}_LL7A:
# 362
 return Cyc_Absyn_throw_exp(Cyc_Tcutil_deep_copy_exp(_tmp151),e->loc);_LL7B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp152=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp152->tag != 11)goto _LL7D;else{_tmp153=_tmp152->f1;}}_LL7C:
# 363
 return Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp153),e->loc);_LL7D: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp154=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp154->tag != 12)goto _LL7F;else{_tmp155=_tmp154->f1;_tmp156=_tmp154->f2;}}_LL7E:
# 365
 return Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp155),((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp156),e->loc);_LL7F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp157=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp157->tag != 13)goto _LL81;else{_tmp158=(void*)_tmp157->f1;_tmp159=_tmp157->f2;_tmp15A=_tmp157->f3;_tmp15B=_tmp157->f4;}}_LL80:
# 367
 return Cyc_Absyn_cast_exp(Cyc_Tcutil_copy_type(_tmp158),Cyc_Tcutil_deep_copy_exp(_tmp159),_tmp15A,_tmp15B,e->loc);_LL81: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp15C=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp15C->tag != 14)goto _LL83;else{_tmp15D=_tmp15C->f1;}}_LL82:
# 368
 return Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmp15D),e->loc);_LL83: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp15E=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp15E->tag != 15)goto _LL85;else{_tmp15F=_tmp15E->f1;_tmp160=_tmp15E->f2;}}_LL84: {
# 370
struct Cyc_Absyn_Exp*eo1=_tmp15F;if(_tmp15F != 0)eo1=(struct Cyc_Absyn_Exp*)Cyc_Tcutil_deep_copy_exp((struct Cyc_Absyn_Exp*)_tmp15F);
# 371
return Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(_tmp160),e->loc);}_LL85: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp161=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp161->tag != 16)goto _LL87;else{_tmp162=(void*)_tmp161->f1;}}_LL86:
# 372
 return Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp162),e->loc);_LL87: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp163=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp163->tag != 17)goto _LL89;else{_tmp164=_tmp163->f1;}}_LL88:
# 373
 return Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmp164),e->loc);_LL89: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp165=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp165->tag != 18)goto _LL8B;else{_tmp166=(void*)_tmp165->f1;_tmp167=(void*)_tmp165->f2;}}_LL8A:
# 375
 return Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp166),_tmp167,e->loc);_LL8B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp168=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp168->tag != 19)goto _LL8D;else{_tmp169=_tmp168->f1;}}_LL8C:
# 376
 return Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmp169),e->loc);_LL8D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp16A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp16A->tag != 20)goto _LL8F;else{_tmp16B=_tmp16A->f1;_tmp16C=_tmp16A->f2;_tmp16D=_tmp16A->f3;_tmp16E=_tmp16A->f4;}}_LL8E: {
# 378
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp114C;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp114B;return Cyc_Absyn_new_exp((void*)((_tmp114B=_cycalloc(sizeof(*_tmp114B)),((_tmp114B[0]=((_tmp114C.tag=20,((_tmp114C.f1=Cyc_Tcutil_deep_copy_exp(_tmp16B),((_tmp114C.f2=_tmp16C,((_tmp114C.f3=_tmp16D,((_tmp114C.f4=_tmp16E,_tmp114C)))))))))),_tmp114B)))),e->loc);}_LL8F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp16F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp16F->tag != 21)goto _LL91;else{_tmp170=_tmp16F->f1;_tmp171=_tmp16F->f2;_tmp172=_tmp16F->f3;_tmp173=_tmp16F->f4;}}_LL90: {
# 380
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp114F;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp114E;return Cyc_Absyn_new_exp((void*)((_tmp114E=_cycalloc(sizeof(*_tmp114E)),((_tmp114E[0]=((_tmp114F.tag=21,((_tmp114F.f1=Cyc_Tcutil_deep_copy_exp(_tmp170),((_tmp114F.f2=_tmp171,((_tmp114F.f3=_tmp172,((_tmp114F.f4=_tmp173,_tmp114F)))))))))),_tmp114E)))),e->loc);}_LL91: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp174=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp174->tag != 22)goto _LL93;else{_tmp175=_tmp174->f1;_tmp176=_tmp174->f2;}}_LL92:
# 381
 return Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmp175),Cyc_Tcutil_deep_copy_exp(_tmp176),e->loc);_LL93: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp177=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp177->tag != 23)goto _LL95;else{_tmp178=_tmp177->f1;}}_LL94:
# 382
 return Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp178),e->loc);_LL95: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp179=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp179->tag != 24)goto _LL97;else{_tmp17A=_tmp179->f1;_tmp17B=*_tmp17A;_tmp17C=_tmp17B.f1;_tmp17D=_tmp17B.f2;_tmp17E=_tmp17B.f3;_tmp17F=_tmp179->f2;}}_LL96: {
# 384
struct _dyneither_ptr*vopt1=_tmp17C;
# 385
if(_tmp17C != 0)vopt1=_tmp17C;{
# 386
struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmp1155;struct _tuple10*_tmp1154;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp1153;return Cyc_Absyn_new_exp((void*)((_tmp1153=_cycalloc(sizeof(*_tmp1153)),((_tmp1153[0]=((_tmp1155.tag=24,((_tmp1155.f1=((_tmp1154=_cycalloc(sizeof(*_tmp1154)),((_tmp1154->f1=vopt1,((_tmp1154->f2=_tmp17D,((_tmp1154->f3=Cyc_Tcutil_copy_type(_tmp17E),_tmp1154)))))))),((_tmp1155.f2=
# 387
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp17F),_tmp1155)))))),_tmp1153)))),e->loc);};}_LL97: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp180=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp180->tag != 25)goto _LL99;else{_tmp181=_tmp180->f1;}}_LL98: {
# 388
struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp1158;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1157;return Cyc_Absyn_new_exp((void*)((_tmp1157=_cycalloc(sizeof(*_tmp1157)),((_tmp1157[0]=((_tmp1158.tag=25,((_tmp1158.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp181),_tmp1158)))),_tmp1157)))),e->loc);}_LL99: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp182=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp182->tag != 26)goto _LL9B;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;_tmp185=_tmp182->f3;_tmp186=_tmp182->f4;}}_LL9A: {
# 390
struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmp115B;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp115A;return Cyc_Absyn_new_exp((void*)((_tmp115A=_cycalloc(sizeof(*_tmp115A)),((_tmp115A[0]=((_tmp115B.tag=26,((_tmp115B.f1=_tmp183,((_tmp115B.f2=Cyc_Tcutil_deep_copy_exp(_tmp184),((_tmp115B.f3=Cyc_Tcutil_deep_copy_exp(_tmp185),((_tmp115B.f4=_tmp186,_tmp115B)))))))))),_tmp115A)))),e->loc);}_LL9B: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp187=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp187->tag != 27)goto _LL9D;else{_tmp188=_tmp187->f1;_tmp189=_tmp187->f2;_tmp18A=_tmp187->f3;_tmp18B=_tmp187->f4;}}_LL9C: {
# 392
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp115E;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp115D;return Cyc_Absyn_new_exp((void*)((_tmp115D=_cycalloc(sizeof(*_tmp115D)),((_tmp115D[0]=((_tmp115E.tag=27,((_tmp115E.f1=_tmp188,((_tmp115E.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp189),((_tmp115E.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp18A),((_tmp115E.f4=_tmp18B,_tmp115E)))))))))),_tmp115D)))),e->loc);}_LL9D: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp18C=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp18C->tag != 28)goto _LL9F;else{_tmp18D=(void*)_tmp18C->f1;_tmp18E=_tmp18C->f2;}}_LL9E: {
# 395
struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp1161;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp1160;return Cyc_Absyn_new_exp((void*)((_tmp1160=_cycalloc(sizeof(*_tmp1160)),((_tmp1160[0]=((_tmp1161.tag=28,((_tmp1161.f1=(void*)Cyc_Tcutil_copy_type(_tmp18D),((_tmp1161.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp18E),_tmp1161)))))),_tmp1160)))),e->loc);}_LL9F: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp18F=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp18F->tag != 29)goto _LLA1;else{_tmp190=_tmp18F->f1;_tmp191=_tmp18F->f2;_tmp192=_tmp18F->f3;}}_LLA0: {
# 397
struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp1164;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1163;return Cyc_Absyn_new_exp((void*)((_tmp1163=_cycalloc(sizeof(*_tmp1163)),((_tmp1163[0]=((_tmp1164.tag=29,((_tmp1164.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp190),((_tmp1164.f2=_tmp191,((_tmp1164.f3=_tmp192,_tmp1164)))))))),_tmp1163)))),e->loc);}_LLA1: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp193=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp193->tag != 30)goto _LLA3;else{_tmp194=_tmp193->f1;_tmp195=_tmp193->f2;_tmp196=_tmp193->f3;}}_LLA2:
# 398
 return e;_LLA3: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp197=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp197->tag != 31)goto _LLA5;else{_tmp198=_tmp197->f1;_tmp199=(void*)_tmp197->f2;_tmp19A=_tmp197->f3;}}_LLA4: {
# 400
struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp1167;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp1166;return Cyc_Absyn_new_exp((void*)((_tmp1166=_cycalloc(sizeof(*_tmp1166)),((_tmp1166[0]=((_tmp1167.tag=31,((_tmp1167.f1=_tmp198,((_tmp1167.f2=(void*)Cyc_Tcutil_copy_type(_tmp199),((_tmp1167.f3=_tmp19A,_tmp1167)))))))),_tmp1166)))),e->loc);}_LLA5: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp19B=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp19B->tag != 32)goto _LLA7;else{_tmp19C=_tmp19B->f1;_tmp19D=_tmp19C.is_calloc;_tmp19E=_tmp19C.rgn;_tmp19F=_tmp19C.elt_type;_tmp1A0=_tmp19C.num_elts;_tmp1A1=_tmp19C.fat_result;}}_LLA6: {
# 402
struct Cyc_Absyn_Exp*_tmp1D6=Cyc_Absyn_copy_exp(e);
# 403
struct Cyc_Absyn_Exp*r1=_tmp19E;if(_tmp19E != 0)r1=(struct Cyc_Absyn_Exp*)Cyc_Tcutil_deep_copy_exp((struct Cyc_Absyn_Exp*)_tmp19E);{
# 404
void**t1=_tmp19F;if(_tmp19F != 0){void**_tmp1168;t1=((_tmp1168=_cycalloc(sizeof(*_tmp1168)),((_tmp1168[0]=Cyc_Tcutil_copy_type(*_tmp19F),_tmp1168))));}
# 405
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp116E;struct Cyc_Absyn_MallocInfo _tmp116D;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp116C;_tmp1D6->r=(void*)((_tmp116C=_cycalloc(sizeof(*_tmp116C)),((_tmp116C[0]=((_tmp116E.tag=32,((_tmp116E.f1=((_tmp116D.is_calloc=_tmp19D,((_tmp116D.rgn=r1,((_tmp116D.elt_type=t1,((_tmp116D.num_elts=_tmp1A0,((_tmp116D.fat_result=_tmp1A1,_tmp116D)))))))))),_tmp116E)))),_tmp116C))));}
# 406
return _tmp1D6;};}_LLA7: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp1A2=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp1A2->tag != 33)goto _LLA9;else{_tmp1A3=_tmp1A2->f1;_tmp1A4=_tmp1A2->f2;}}_LLA8:
# 407
 return Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp1A3),Cyc_Tcutil_deep_copy_exp(_tmp1A4),e->loc);_LLA9: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp1A5=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp1A5->tag != 34)goto _LLAB;else{_tmp1A6=_tmp1A5->f1;_tmp1A7=_tmp1A5->f2;}}_LLAA: {
# 409
struct Cyc_Core_Opt*nopt1=_tmp1A6;
# 410
if(_tmp1A6 != 0){struct Cyc_Core_Opt*_tmp116F;nopt1=((_tmp116F=_cycalloc(sizeof(*_tmp116F)),((_tmp116F->v=(struct _tuple2*)_tmp1A6->v,_tmp116F))));}{
# 411
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp1172;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp1171;return Cyc_Absyn_new_exp((void*)((_tmp1171=_cycalloc(sizeof(*_tmp1171)),((_tmp1171[0]=((_tmp1172.tag=34,((_tmp1172.f1=nopt1,((_tmp1172.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp1A7),_tmp1172)))))),_tmp1171)))),e->loc);};}_LLAB: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp1A8=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp1A8->tag != 35)goto _LLAD;}_LLAC: {
# 413
struct Cyc_Core_Failure_exn_struct _tmp1178;const char*_tmp1177;struct Cyc_Core_Failure_exn_struct*_tmp1176;(int)_throw((void*)((_tmp1176=_cycalloc(sizeof(*_tmp1176)),((_tmp1176[0]=((_tmp1178.tag=Cyc_Core_Failure,((_tmp1178.f1=((_tmp1177="deep_copy: statement expressions unsupported",_tag_dyneither(_tmp1177,sizeof(char),45))),_tmp1178)))),_tmp1176)))));}_LLAD: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp1A9=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp1A9->tag != 36)goto _LLAF;else{_tmp1AA=_tmp1A9->f1;_tmp1AB=_tmp1A9->f2;}}_LLAE: {
# 414
struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmp117B;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp117A;return Cyc_Absyn_new_exp((void*)((_tmp117A=_cycalloc(sizeof(*_tmp117A)),((_tmp117A[0]=((_tmp117B.tag=36,((_tmp117B.f1=Cyc_Tcutil_deep_copy_exp(_tmp1AA),((_tmp117B.f2=_tmp1AB,_tmp117B)))))),_tmp117A)))),e->loc);}_LLAF: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1AC=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp1AC->tag != 37)goto _LLB1;else{_tmp1AD=(void*)_tmp1AC->f1;}}_LLB0:
# 415
 return Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(_tmp1AD),e->loc);_LLB1: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp1AE=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp1AE->tag != 38)goto _LL64;else{_tmp1AF=_tmp1AE->f1;_tmp1B0=_tmp1AE->f2;}}_LLB2:
# 416
 return Cyc_Absyn_asm_exp(_tmp1AF,_tmp1B0,e->loc);_LL64:;}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 428 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
# 429
struct Cyc_Absyn_Kind _tmp1E4;enum Cyc_Absyn_KindQual _tmp1E5;enum Cyc_Absyn_AliasQual _tmp1E6;struct Cyc_Absyn_Kind*_tmp1E3=ka1;_tmp1E4=*_tmp1E3;_tmp1E5=_tmp1E4.kind;_tmp1E6=_tmp1E4.aliasqual;{
# 430
struct Cyc_Absyn_Kind _tmp1E8;enum Cyc_Absyn_KindQual _tmp1E9;enum Cyc_Absyn_AliasQual _tmp1EA;struct Cyc_Absyn_Kind*_tmp1E7=ka2;_tmp1E8=*_tmp1E7;_tmp1E9=_tmp1E8.kind;_tmp1EA=_tmp1E8.aliasqual;
# 432
if(_tmp1E5 != _tmp1E9){
# 433
struct _tuple20 _tmp117C;struct _tuple20 _tmp1EC=(_tmp117C.f1=_tmp1E5,((_tmp117C.f2=_tmp1E9,_tmp117C)));enum Cyc_Absyn_KindQual _tmp1ED;enum Cyc_Absyn_KindQual _tmp1EE;enum Cyc_Absyn_KindQual _tmp1EF;enum Cyc_Absyn_KindQual _tmp1F0;enum Cyc_Absyn_KindQual _tmp1F1;enum Cyc_Absyn_KindQual _tmp1F2;_LLB9: _tmp1ED=_tmp1EC.f1;if(_tmp1ED != Cyc_Absyn_BoxKind)goto _LLBB;_tmp1EE=_tmp1EC.f2;if(_tmp1EE != Cyc_Absyn_MemKind)goto _LLBB;_LLBA:
# 434
 goto _LLBC;_LLBB: _tmp1EF=_tmp1EC.f1;if(_tmp1EF != Cyc_Absyn_BoxKind)goto _LLBD;_tmp1F0=_tmp1EC.f2;if(_tmp1F0 != Cyc_Absyn_AnyKind)goto _LLBD;_LLBC:
# 435
 goto _LLBE;_LLBD: _tmp1F1=_tmp1EC.f1;if(_tmp1F1 != Cyc_Absyn_MemKind)goto _LLBF;_tmp1F2=_tmp1EC.f2;if(_tmp1F2 != Cyc_Absyn_AnyKind)goto _LLBF;_LLBE:
# 436
 goto _LLB8;_LLBF:;_LLC0:
# 437
 return 0;_LLB8:;}
# 441
if(_tmp1E6 != _tmp1EA){
# 442
struct _tuple21 _tmp117D;struct _tuple21 _tmp1F4=(_tmp117D.f1=_tmp1E6,((_tmp117D.f2=_tmp1EA,_tmp117D)));enum Cyc_Absyn_AliasQual _tmp1F5;enum Cyc_Absyn_AliasQual _tmp1F6;enum Cyc_Absyn_AliasQual _tmp1F7;enum Cyc_Absyn_AliasQual _tmp1F8;_LLC2: _tmp1F5=_tmp1F4.f1;if(_tmp1F5 != Cyc_Absyn_Aliasable)goto _LLC4;_tmp1F6=_tmp1F4.f2;if(_tmp1F6 != Cyc_Absyn_Top)goto _LLC4;_LLC3:
# 443
 goto _LLC5;_LLC4: _tmp1F7=_tmp1F4.f1;if(_tmp1F7 != Cyc_Absyn_Unique)goto _LLC6;_tmp1F8=_tmp1F4.f2;if(_tmp1F8 != Cyc_Absyn_Top)goto _LLC6;_LLC5:
# 444
 return 1;_LLC6:;_LLC7:
# 445
 return 0;_LLC1:;}
# 448
return 1;};}
# 451
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
# 452
void*_tmp1F9=Cyc_Absyn_compress_kb(tv->kind);struct Cyc_Absyn_Kind*_tmp1FB;struct Cyc_Absyn_Kind*_tmp1FD;_LLC9: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1FA=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1F9;if(_tmp1FA->tag != 0)goto _LLCB;else{_tmp1FB=_tmp1FA->f1;}}_LLCA:
# 453
 return _tmp1FB;_LLCB: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1FC=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1F9;if(_tmp1FC->tag != 2)goto _LLCD;else{_tmp1FD=_tmp1FC->f2;}}_LLCC:
# 454
 return _tmp1FD;_LLCD:;_LLCE:
# 456
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1180;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp117F;tv->kind=(void*)((_tmp117F=_cycalloc(sizeof(*_tmp117F)),((_tmp117F[0]=((_tmp1180.tag=2,((_tmp1180.f1=0,((_tmp1180.f2=def,_tmp1180)))))),_tmp117F))));}
# 457
return def;_LLC8:;}
# 461
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
# 462
struct _tuple0 _tmp1181;struct _tuple0 _tmp201=(_tmp1181.f1=Cyc_Absyn_compress_kb(kb1),((_tmp1181.f2=Cyc_Absyn_compress_kb(kb2),_tmp1181)));void*_tmp202;struct Cyc_Absyn_Kind*_tmp204;void*_tmp205;struct Cyc_Absyn_Kind*_tmp207;void*_tmp208;struct Cyc_Core_Opt*_tmp20A;struct Cyc_Core_Opt**_tmp20B;struct Cyc_Absyn_Kind*_tmp20C;void*_tmp20D;struct Cyc_Absyn_Kind*_tmp20F;void*_tmp210;struct Cyc_Absyn_Kind*_tmp212;void*_tmp213;struct Cyc_Core_Opt*_tmp215;struct Cyc_Core_Opt**_tmp216;struct Cyc_Absyn_Kind*_tmp217;void*_tmp218;struct Cyc_Core_Opt*_tmp21A;struct Cyc_Core_Opt**_tmp21B;struct Cyc_Absyn_Kind*_tmp21C;void*_tmp21D;struct Cyc_Core_Opt*_tmp21F;struct Cyc_Core_Opt**_tmp220;struct Cyc_Absyn_Kind*_tmp221;void*_tmp222;struct Cyc_Core_Opt*_tmp224;struct Cyc_Core_Opt**_tmp225;void*_tmp226;void*_tmp227;void*_tmp228;struct Cyc_Core_Opt*_tmp22A;struct Cyc_Core_Opt**_tmp22B;_LLD0: _tmp202=_tmp201.f1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp203=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp202;if(_tmp203->tag != 0)goto _LLD2;else{_tmp204=_tmp203->f1;}};_tmp205=_tmp201.f2;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp206=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp205;if(_tmp206->tag != 0)goto _LLD2;else{_tmp207=_tmp206->f1;}};_LLD1:
# 463
 return _tmp204 == _tmp207;_LLD2: _tmp208=_tmp201.f1;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp209=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp208;if(_tmp209->tag != 2)goto _LLD4;else{_tmp20A=_tmp209->f1;_tmp20B=(struct Cyc_Core_Opt**)& _tmp209->f1;_tmp20C=_tmp209->f2;}};_tmp20D=_tmp201.f2;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp20E=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp20D;if(_tmp20E->tag != 0)goto _LLD4;else{_tmp20F=_tmp20E->f1;}};_LLD3:
# 465
 if(Cyc_Tcutil_kind_leq(_tmp20F,_tmp20C)){
# 466
{struct Cyc_Core_Opt*_tmp1182;*_tmp20B=((_tmp1182=_cycalloc(sizeof(*_tmp1182)),((_tmp1182->v=kb2,_tmp1182))));}
# 467
return 1;}else{
# 468
return 0;}_LLD4: _tmp210=_tmp201.f1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp211=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp210;if(_tmp211->tag != 0)goto _LLD6;else{_tmp212=_tmp211->f1;}};_tmp213=_tmp201.f2;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp214=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp213;if(_tmp214->tag != 2)goto _LLD6;else{_tmp215=_tmp214->f1;_tmp216=(struct Cyc_Core_Opt**)& _tmp214->f1;_tmp217=_tmp214->f2;}};_LLD5:
# 470
 if(Cyc_Tcutil_kind_leq(_tmp212,_tmp217)){
# 471
{struct Cyc_Core_Opt*_tmp1183;*_tmp216=((_tmp1183=_cycalloc(sizeof(*_tmp1183)),((_tmp1183->v=kb1,_tmp1183))));}
# 472
return 1;}else{
# 473
return 0;}_LLD6: _tmp218=_tmp201.f1;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp219=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp218;if(_tmp219->tag != 2)goto _LLD8;else{_tmp21A=_tmp219->f1;_tmp21B=(struct Cyc_Core_Opt**)& _tmp219->f1;_tmp21C=_tmp219->f2;}};_tmp21D=_tmp201.f2;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp21E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp21D;if(_tmp21E->tag != 2)goto _LLD8;else{_tmp21F=_tmp21E->f1;_tmp220=(struct Cyc_Core_Opt**)& _tmp21E->f1;_tmp221=_tmp21E->f2;}};_LLD7:
# 475
 if(Cyc_Tcutil_kind_leq(_tmp21C,_tmp221)){
# 476
{struct Cyc_Core_Opt*_tmp1184;*_tmp220=((_tmp1184=_cycalloc(sizeof(*_tmp1184)),((_tmp1184->v=kb1,_tmp1184))));}
# 477
return 1;}else{
# 478
if(Cyc_Tcutil_kind_leq(_tmp221,_tmp21C)){
# 479
{struct Cyc_Core_Opt*_tmp1185;*_tmp21B=((_tmp1185=_cycalloc(sizeof(*_tmp1185)),((_tmp1185->v=kb2,_tmp1185))));}
# 480
return 1;}else{
# 481
return 0;}}_LLD8: _tmp222=_tmp201.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp223=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp222;if(_tmp223->tag != 1)goto _LLDA;else{_tmp224=_tmp223->f1;_tmp225=(struct Cyc_Core_Opt**)& _tmp223->f1;}};_tmp226=_tmp201.f2;_LLD9:
# 482
 _tmp227=_tmp226;_tmp22B=_tmp225;goto _LLDB;_LLDA: _tmp227=_tmp201.f1;_tmp228=_tmp201.f2;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp229=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp228;if(_tmp229->tag != 1)goto _LLCF;else{_tmp22A=_tmp229->f1;_tmp22B=(struct Cyc_Core_Opt**)& _tmp229->f1;}};_LLDB:
# 484
{struct Cyc_Core_Opt*_tmp1186;*_tmp22B=((_tmp1186=_cycalloc(sizeof(*_tmp1186)),((_tmp1186->v=_tmp227,_tmp1186))));}
# 485
return 1;_LLCF:;}
# 489
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
# 490
void*_tmp231=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp233;_LLDD: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp232=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp231;if(_tmp232->tag != 2)goto _LLDF;else{_tmp233=_tmp232->f1;}}_LLDE: {
# 492
void*_tmp234=_tmp233->kind;
# 493
_tmp233->kind=kb;{
# 494
struct _tuple16 _tmp1187;return(_tmp1187.f1=_tmp233,((_tmp1187.f2=_tmp234,_tmp1187)));};}_LLDF:;_LLE0: {
# 496
const char*_tmp118B;void*_tmp118A[1];struct Cyc_String_pa_PrintArg_struct _tmp1189;(_tmp1189.tag=0,((_tmp1189.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp118A[0]=& _tmp1189,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp118B="swap_kind: cannot update the kind of %s",_tag_dyneither(_tmp118B,sizeof(char),40))),_tag_dyneither(_tmp118A,sizeof(void*),1)))))));}_LLDC:;}
# 504
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
# 505
void*_tmp239=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp23B;struct Cyc_Absyn_Tvar*_tmp23D;enum Cyc_Absyn_Size_of _tmp240;struct Cyc_Absyn_DatatypeFieldInfo _tmp24C;union Cyc_Absyn_DatatypeFieldInfoU _tmp24D;struct _tuple3 _tmp24E;struct Cyc_Absyn_Datatypedecl*_tmp24F;struct Cyc_Absyn_Datatypefield*_tmp250;struct Cyc_Absyn_DatatypeFieldInfo _tmp252;union Cyc_Absyn_DatatypeFieldInfoU _tmp253;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp254;struct Cyc_Absyn_AggrInfo _tmp256;union Cyc_Absyn_AggrInfoU _tmp257;struct _tuple4 _tmp258;struct Cyc_Absyn_AggrInfo _tmp25A;union Cyc_Absyn_AggrInfoU _tmp25B;struct Cyc_Absyn_Aggrdecl**_tmp25C;struct Cyc_Absyn_Aggrdecl*_tmp25D;struct Cyc_Absyn_Aggrdecl _tmp25E;struct Cyc_List_List*_tmp25F;struct Cyc_Absyn_AggrdeclImpl*_tmp260;struct Cyc_List_List*_tmp261;struct Cyc_Absyn_PtrInfo _tmp264;struct Cyc_Absyn_Typedefdecl*_tmp26A;struct Cyc_Absyn_TypeDecl*_tmp26C;struct Cyc_Absyn_TypeDecl _tmp26D;void*_tmp26E;struct Cyc_Absyn_TypeDecl*_tmp271;struct Cyc_Absyn_TypeDecl _tmp272;void*_tmp273;struct Cyc_Absyn_TypeDecl*_tmp276;struct Cyc_Absyn_TypeDecl _tmp277;void*_tmp278;_LLE2: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp23A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp239;if(_tmp23A->tag != 1)goto _LLE4;else{_tmp23B=_tmp23A->f1;}}_LLE3:
# 506
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp23B))->v;_LLE4: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp23C=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp239;if(_tmp23C->tag != 2)goto _LLE6;else{_tmp23D=_tmp23C->f1;}}_LLE5:
# 507
 return Cyc_Tcutil_tvar_kind(_tmp23D,& Cyc_Tcutil_bk);_LLE6: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp23E=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp239;if(_tmp23E->tag != 0)goto _LLE8;}_LLE7:
# 508
 return& Cyc_Tcutil_mk;_LLE8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp23F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp239;if(_tmp23F->tag != 6)goto _LLEA;else{_tmp240=_tmp23F->f2;}}_LLE9:
# 510
 return(_tmp240 == (enum Cyc_Absyn_Size_of)Cyc_Absyn_Int_sz  || _tmp240 == (enum Cyc_Absyn_Size_of)Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LLEA: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp241=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp239;if(_tmp241->tag != 7)goto _LLEC;}_LLEB:
# 512
 goto _LLED;_LLEC: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp242=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp239;if(_tmp242->tag != 9)goto _LLEE;}_LLED:
# 513
 return& Cyc_Tcutil_mk;_LLEE: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp243=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp239;if(_tmp243->tag != 16)goto _LLF0;}_LLEF:
# 514
 goto _LLF1;_LLF0: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp244=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp239;if(_tmp244->tag != 13)goto _LLF2;}_LLF1:
# 515
 goto _LLF3;_LLF2: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp245=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp239;if(_tmp245->tag != 14)goto _LLF4;}_LLF3:
# 516
 goto _LLF5;_LLF4: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp246=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp239;if(_tmp246->tag != 15)goto _LLF6;}_LLF5:
# 517
 return& Cyc_Tcutil_bk;_LLF6: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp247=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp239;if(_tmp247->tag != 21)goto _LLF8;}_LLF7:
# 518
 return& Cyc_Tcutil_urk;_LLF8: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp248=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp239;if(_tmp248->tag != 20)goto _LLFA;}_LLF9:
# 519
 return& Cyc_Tcutil_rk;_LLFA: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp249=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp239;if(_tmp249->tag != 22)goto _LLFC;}_LLFB:
# 520
 return& Cyc_Tcutil_trk;_LLFC: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp24A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp239;if(_tmp24A->tag != 3)goto _LLFE;}_LLFD:
# 523
 return& Cyc_Tcutil_ak;_LLFE: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp24B=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp239;if(_tmp24B->tag != 4)goto _LL100;else{_tmp24C=_tmp24B->f1;_tmp24D=_tmp24C.field_info;if((_tmp24D.KnownDatatypefield).tag != 2)goto _LL100;_tmp24E=(struct _tuple3)(_tmp24D.KnownDatatypefield).val;_tmp24F=_tmp24E.f1;_tmp250=_tmp24E.f2;}}_LLFF:
# 525
 return& Cyc_Tcutil_mk;_LL100: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp251=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp239;if(_tmp251->tag != 4)goto _LL102;else{_tmp252=_tmp251->f1;_tmp253=_tmp252.field_info;if((_tmp253.UnknownDatatypefield).tag != 1)goto _LL102;_tmp254=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp253.UnknownDatatypefield).val;}}_LL101: {
# 527
const char*_tmp118E;void*_tmp118D;(_tmp118D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp118E="typ_kind: Unresolved DatatypeFieldType",_tag_dyneither(_tmp118E,sizeof(char),39))),_tag_dyneither(_tmp118D,sizeof(void*),0)));}_LL102: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp255=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp239;if(_tmp255->tag != 11)goto _LL104;else{_tmp256=_tmp255->f1;_tmp257=_tmp256.aggr_info;if((_tmp257.UnknownAggr).tag != 1)goto _LL104;_tmp258=(struct _tuple4)(_tmp257.UnknownAggr).val;}}_LL103:
# 530
 return& Cyc_Tcutil_ak;_LL104: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp259=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp239;if(_tmp259->tag != 11)goto _LL106;else{_tmp25A=_tmp259->f1;_tmp25B=_tmp25A.aggr_info;if((_tmp25B.KnownAggr).tag != 2)goto _LL106;_tmp25C=(struct Cyc_Absyn_Aggrdecl**)(_tmp25B.KnownAggr).val;_tmp25D=*_tmp25C;_tmp25E=*_tmp25D;_tmp25F=_tmp25E.tvs;_tmp260=_tmp25E.impl;_tmp261=_tmp25A.targs;}}_LL105:
# 532
 if(_tmp260 == 0)return& Cyc_Tcutil_ak;{
# 534
struct Cyc_List_List*_tmp27F=_tmp260->fields;
# 535
if(_tmp27F == 0)return& Cyc_Tcutil_mk;
# 536
for(0;_tmp27F->tl != 0;_tmp27F=_tmp27F->tl){;}{
# 537
void*_tmp280=((struct Cyc_Absyn_Aggrfield*)_tmp27F->hd)->type;
# 538
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(_tmp280);
# 539
if(_tmp261 != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 542
struct _RegionHandle _tmp281=_new_region("temp");struct _RegionHandle*temp=& _tmp281;_push_region(temp);
# 543
{struct Cyc_List_List*_tmp282=0;
# 545
for(0;_tmp25F != 0;(_tmp25F=_tmp25F->tl,_tmp261=_tmp261->tl)){
# 546
struct Cyc_Absyn_Tvar*_tmp283=(struct Cyc_Absyn_Tvar*)_tmp25F->hd;
# 547
void*_tmp284=(void*)((struct Cyc_List_List*)_check_null(_tmp261))->hd;
# 548
struct Cyc_Absyn_Kind*_tmp285=Cyc_Tcutil_tvar_kind(_tmp283,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind _tmp286;enum Cyc_Absyn_KindQual _tmp287;_LL121: _tmp286=*_tmp285;_tmp287=_tmp286.kind;if(_tmp287 != Cyc_Absyn_AnyKind)goto _LL123;_LL122:
# 550
{struct _tuple16*_tmp1191;struct Cyc_List_List*_tmp1190;_tmp282=((_tmp1190=_region_malloc(temp,sizeof(*_tmp1190)),((_tmp1190->hd=((_tmp1191=_region_malloc(temp,sizeof(*_tmp1191)),((_tmp1191->f1=_tmp283,((_tmp1191->f2=_tmp284,_tmp1191)))))),((_tmp1190->tl=_tmp282,_tmp1190))))));}goto _LL120;_LL123:;_LL124:
# 551
 goto _LL120;_LL120:;}
# 554
if(_tmp282 != 0){
# 555
_tmp280=Cyc_Tcutil_rsubstitute(temp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp282),_tmp280);
# 556
k=Cyc_Tcutil_typ_kind(_tmp280);}
# 558
if(k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak){struct Cyc_Absyn_Kind*_tmp28A=k;_npop_handler(0);return _tmp28A;}}
# 543
;_pop_region(temp);}
# 560
return& Cyc_Tcutil_mk;};};_LL106: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp262=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp239;if(_tmp262->tag != 12)goto _LL108;}_LL107:
# 561
 return& Cyc_Tcutil_mk;_LL108: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp263=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp239;if(_tmp263->tag != 5)goto _LL10A;else{_tmp264=_tmp263->f1;}}_LL109: {
# 563
void*_tmp28B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp264.ptr_atts).bounds);_LL126: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp28C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp28B;if(_tmp28C->tag != 0)goto _LL128;}_LL127:
# 565
 switch((Cyc_Tcutil_typ_kind((_tmp264.ptr_atts).rgn))->aliasqual){case Cyc_Absyn_Aliasable: _LL12A:
# 566
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL12B:
# 567
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL12C:
# 568
 return& Cyc_Tcutil_tmk;}_LL128: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp28D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp28B;if(_tmp28D->tag != 1)goto _LL125;}_LL129:
# 571
 switch((Cyc_Tcutil_typ_kind((_tmp264.ptr_atts).rgn))->aliasqual){case Cyc_Absyn_Aliasable: _LL12E:
# 572
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL12F:
# 573
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL130:
# 574
 return& Cyc_Tcutil_tbk;}_LL125:;}_LL10A: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp265=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp239;if(_tmp265->tag != 18)goto _LL10C;}_LL10B:
# 577
 return& Cyc_Tcutil_ik;_LL10C: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp266=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp239;if(_tmp266->tag != 19)goto _LL10E;}_LL10D:
# 578
 return& Cyc_Tcutil_bk;_LL10E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp267=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp239;if(_tmp267->tag != 8)goto _LL110;}_LL10F:
# 580
 goto _LL111;_LL110: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp268=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp239;if(_tmp268->tag != 10)goto _LL112;}_LL111:
# 581
 return& Cyc_Tcutil_mk;_LL112: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp269=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp239;if(_tmp269->tag != 17)goto _LL114;else{_tmp26A=_tmp269->f3;}}_LL113:
# 583
 if(_tmp26A == 0  || _tmp26A->kind == 0){
# 584
const char*_tmp1195;void*_tmp1194[1];struct Cyc_String_pa_PrintArg_struct _tmp1193;(_tmp1193.tag=0,((_tmp1193.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1194[0]=& _tmp1193,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1195="typ_kind: typedef found: %s",_tag_dyneither(_tmp1195,sizeof(char),28))),_tag_dyneither(_tmp1194,sizeof(void*),1)))))));}
# 585
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp26A->kind))->v;_LL114: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp26B=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp239;if(_tmp26B->tag != 26)goto _LL116;else{_tmp26C=_tmp26B->f1;_tmp26D=*_tmp26C;_tmp26E=_tmp26D.r;{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp26F=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp26E;if(_tmp26F->tag != 0)goto _LL116;};}}_LL115:
# 586
 return& Cyc_Tcutil_ak;_LL116: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp270=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp239;if(_tmp270->tag != 26)goto _LL118;else{_tmp271=_tmp270->f1;_tmp272=*_tmp271;_tmp273=_tmp272.r;{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp274=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp273;if(_tmp274->tag != 1)goto _LL118;};}}_LL117:
# 587
 return& Cyc_Tcutil_bk;_LL118: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp275=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp239;if(_tmp275->tag != 26)goto _LL11A;else{_tmp276=_tmp275->f1;_tmp277=*_tmp276;_tmp278=_tmp277.r;{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp279=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp278;if(_tmp279->tag != 2)goto _LL11A;};}}_LL119:
# 588
 return& Cyc_Tcutil_ak;_LL11A: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp27A=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp239;if(_tmp27A->tag != 23)goto _LL11C;}_LL11B:
# 589
 goto _LL11D;_LL11C: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp27B=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp239;if(_tmp27B->tag != 24)goto _LL11E;}_LL11D:
# 590
 goto _LL11F;_LL11E: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp27C=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp239;if(_tmp27C->tag != 25)goto _LLE1;}_LL11F:
# 591
 return& Cyc_Tcutil_ek;_LLE1:;}
# 595
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
# 596
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 600
int Cyc_Tcutil_unify(void*t1,void*t2){
# 601
struct _handler_cons _tmp291;_push_handler(& _tmp291);{int _tmp293=0;if(setjmp(_tmp291.handler))_tmp293=1;if(!_tmp293){
# 602
Cyc_Tcutil_unify_it(t1,t2);{
# 603
int _tmp294=1;_npop_handler(0);return _tmp294;};
# 602
;_pop_handler();}else{void*_tmp292=(void*)_exn_thrown;void*_tmp296=_tmp292;_LL133: {struct Cyc_Tcutil_Unify_exn_struct*_tmp297=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp296;if(_tmp297->tag != Cyc_Tcutil_Unify)goto _LL135;}_LL134:
# 604
 return 0;_LL135:;_LL136:(void)_throw(_tmp296);_LL132:;}};}
# 609
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
# 610
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
# 611
t=Cyc_Tcutil_compress(t);{
# 612
void*_tmp298=t;struct Cyc_Absyn_Tvar*_tmp29A;void*_tmp29C;struct Cyc_Core_Opt*_tmp29D;struct Cyc_Core_Opt**_tmp29E;struct Cyc_Absyn_PtrInfo _tmp2A0;struct Cyc_Absyn_ArrayInfo _tmp2A2;void*_tmp2A3;struct Cyc_Absyn_FnInfo _tmp2A5;struct Cyc_List_List*_tmp2A6;void*_tmp2A7;struct Cyc_Absyn_Tqual _tmp2A8;void*_tmp2A9;struct Cyc_List_List*_tmp2AA;int _tmp2AB;struct Cyc_Absyn_VarargInfo*_tmp2AC;struct Cyc_List_List*_tmp2AD;struct Cyc_List_List*_tmp2AE;struct Cyc_List_List*_tmp2B0;struct Cyc_Absyn_DatatypeInfo _tmp2B2;struct Cyc_List_List*_tmp2B3;struct Cyc_List_List*_tmp2B5;struct Cyc_Absyn_DatatypeFieldInfo _tmp2B7;struct Cyc_List_List*_tmp2B8;struct Cyc_Absyn_AggrInfo _tmp2BA;struct Cyc_List_List*_tmp2BB;struct Cyc_List_List*_tmp2BD;void*_tmp2BF;void*_tmp2C1;void*_tmp2C3;void*_tmp2C5;struct Cyc_List_List*_tmp2C7;_LL138: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp299=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp298;if(_tmp299->tag != 2)goto _LL13A;else{_tmp29A=_tmp299->f1;}}_LL139:
# 614
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp29A)){
# 615
{const char*_tmp1196;Cyc_Tcutil_failure_reason=((_tmp1196="(type variable would escape scope)",_tag_dyneither(_tmp1196,sizeof(char),35)));}
# 616
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 618
goto _LL137;_LL13A: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp29B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp298;if(_tmp29B->tag != 1)goto _LL13C;else{_tmp29C=(void*)_tmp29B->f2;_tmp29D=_tmp29B->f4;_tmp29E=(struct Cyc_Core_Opt**)& _tmp29B->f4;}}_LL13B:
# 620
 if(t == evar){
# 621
{const char*_tmp1197;Cyc_Tcutil_failure_reason=((_tmp1197="(occurs check)",_tag_dyneither(_tmp1197,sizeof(char),15)));}
# 622
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 624
if(_tmp29C != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp29C);else{
# 627
int problem=0;
# 628
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp29E))->v;for(0;s != 0;s=s->tl){
# 629
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
# 630
problem=1;break;}}}
# 634
if(problem){
# 635
struct Cyc_List_List*_tmp2CA=0;
# 636
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp29E))->v;for(0;s != 0;s=s->tl){
# 637
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
# 638
struct Cyc_List_List*_tmp1198;_tmp2CA=((_tmp1198=_cycalloc(sizeof(*_tmp1198)),((_tmp1198->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmp1198->tl=_tmp2CA,_tmp1198))))));}}}{
# 640
struct Cyc_Core_Opt*_tmp1199;*_tmp29E=((_tmp1199=_cycalloc(sizeof(*_tmp1199)),((_tmp1199->v=_tmp2CA,_tmp1199))));};}}}
# 643
goto _LL137;_LL13C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp29F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp298;if(_tmp29F->tag != 5)goto _LL13E;else{_tmp2A0=_tmp29F->f1;}}_LL13D:
# 645
 Cyc_Tcutil_occurs(evar,r,env,_tmp2A0.elt_typ);
# 646
Cyc_Tcutil_occurs(evar,r,env,(_tmp2A0.ptr_atts).rgn);
# 647
goto _LL137;_LL13E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2A1=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp298;if(_tmp2A1->tag != 8)goto _LL140;else{_tmp2A2=_tmp2A1->f1;_tmp2A3=_tmp2A2.elt_type;}}_LL13F:
# 648
 Cyc_Tcutil_occurs(evar,r,env,_tmp2A3);goto _LL137;_LL140: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2A4=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp298;if(_tmp2A4->tag != 9)goto _LL142;else{_tmp2A5=_tmp2A4->f1;_tmp2A6=_tmp2A5.tvars;_tmp2A7=_tmp2A5.effect;_tmp2A8=_tmp2A5.ret_tqual;_tmp2A9=_tmp2A5.ret_typ;_tmp2AA=_tmp2A5.args;_tmp2AB=_tmp2A5.c_varargs;_tmp2AC=_tmp2A5.cyc_varargs;_tmp2AD=_tmp2A5.rgn_po;_tmp2AE=_tmp2A5.attributes;}}_LL141:
# 650
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp2A6,env);
# 651
if(_tmp2A7 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp2A7);
# 652
Cyc_Tcutil_occurs(evar,r,env,_tmp2A9);
# 653
for(0;_tmp2AA != 0;_tmp2AA=_tmp2AA->tl){
# 654
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp2AA->hd)).f3);}
# 655
if(_tmp2AC != 0)
# 656
Cyc_Tcutil_occurs(evar,r,env,_tmp2AC->type);
# 657
for(0;_tmp2AD != 0;_tmp2AD=_tmp2AD->tl){
# 658
struct _tuple0 _tmp2CE;void*_tmp2CF;void*_tmp2D0;struct _tuple0*_tmp2CD=(struct _tuple0*)_tmp2AD->hd;_tmp2CE=*_tmp2CD;_tmp2CF=_tmp2CE.f1;_tmp2D0=_tmp2CE.f2;
# 659
Cyc_Tcutil_occurs(evar,r,env,_tmp2CF);
# 660
Cyc_Tcutil_occurs(evar,r,env,_tmp2D0);}
# 662
goto _LL137;_LL142: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2AF=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp298;if(_tmp2AF->tag != 10)goto _LL144;else{_tmp2B0=_tmp2AF->f1;}}_LL143:
# 664
 for(0;_tmp2B0 != 0;_tmp2B0=_tmp2B0->tl){
# 665
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp2B0->hd)).f2);}
# 666
goto _LL137;_LL144: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp2B1=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp298;if(_tmp2B1->tag != 3)goto _LL146;else{_tmp2B2=_tmp2B1->f1;_tmp2B3=_tmp2B2.targs;}}_LL145:
# 668
 Cyc_Tcutil_occurslist(evar,r,env,_tmp2B3);goto _LL137;_LL146: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp2B4=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp298;if(_tmp2B4->tag != 17)goto _LL148;else{_tmp2B5=_tmp2B4->f2;}}_LL147:
# 669
 _tmp2B8=_tmp2B5;goto _LL149;_LL148: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp2B6=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp298;if(_tmp2B6->tag != 4)goto _LL14A;else{_tmp2B7=_tmp2B6->f1;_tmp2B8=_tmp2B7.targs;}}_LL149:
# 670
 _tmp2BB=_tmp2B8;goto _LL14B;_LL14A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2B9=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp298;if(_tmp2B9->tag != 11)goto _LL14C;else{_tmp2BA=_tmp2B9->f1;_tmp2BB=_tmp2BA.targs;}}_LL14B:
# 671
 Cyc_Tcutil_occurslist(evar,r,env,_tmp2BB);goto _LL137;_LL14C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2BC=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp298;if(_tmp2BC->tag != 12)goto _LL14E;else{_tmp2BD=_tmp2BC->f2;}}_LL14D:
# 673
 for(0;_tmp2BD != 0;_tmp2BD=_tmp2BD->tl){
# 674
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp2BD->hd)->type);}
# 675
goto _LL137;_LL14E: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2BE=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp298;if(_tmp2BE->tag != 19)goto _LL150;else{_tmp2BF=(void*)_tmp2BE->f1;}}_LL14F:
# 676
 _tmp2C1=_tmp2BF;goto _LL151;_LL150: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2C0=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp298;if(_tmp2C0->tag != 23)goto _LL152;else{_tmp2C1=(void*)_tmp2C0->f1;}}_LL151:
# 677
 _tmp2C3=_tmp2C1;goto _LL153;_LL152: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp2C2=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp298;if(_tmp2C2->tag != 15)goto _LL154;else{_tmp2C3=(void*)_tmp2C2->f1;}}_LL153:
# 678
 _tmp2C5=_tmp2C3;goto _LL155;_LL154: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp2C4=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp298;if(_tmp2C4->tag != 25)goto _LL156;else{_tmp2C5=(void*)_tmp2C4->f1;}}_LL155:
# 679
 Cyc_Tcutil_occurs(evar,r,env,_tmp2C5);goto _LL137;_LL156: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2C6=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp298;if(_tmp2C6->tag != 24)goto _LL158;else{_tmp2C7=_tmp2C6->f1;}}_LL157:
# 680
 Cyc_Tcutil_occurslist(evar,r,env,_tmp2C7);goto _LL137;_LL158:;_LL159:
# 682
 goto _LL137;_LL137:;};}
# 685
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 687
for(0;ts != 0;ts=ts->tl){
# 688
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 692
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
# 693
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
# 694
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
# 695
if(t1 != 0  || t2 != 0)
# 696
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 700
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
# 701
if(tq1.print_const  && !tq1.real_const){
# 702
const char*_tmp119C;void*_tmp119B;(_tmp119B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp119C="tq1 real_const not set.",_tag_dyneither(_tmp119C,sizeof(char),24))),_tag_dyneither(_tmp119B,sizeof(void*),0)));}
# 703
if(tq2.print_const  && !tq2.real_const){
# 704
const char*_tmp119F;void*_tmp119E;(_tmp119E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp119F="tq2 real_const not set.",_tag_dyneither(_tmp119F,sizeof(char),24))),_tag_dyneither(_tmp119E,sizeof(void*),0)));}
# 706
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 709
Cyc_Tcutil_t1_failure=t1;
# 710
Cyc_Tcutil_t2_failure=t2;
# 711
Cyc_Tcutil_tq1_const=tq1.real_const;
# 712
Cyc_Tcutil_tq2_const=tq2.real_const;
# 713
{const char*_tmp11A0;Cyc_Tcutil_failure_reason=((_tmp11A0="(qualifiers don't match)",_tag_dyneither(_tmp11A0,sizeof(char),25)));}
# 714
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 717
Cyc_Tcutil_tq1_const=0;
# 718
Cyc_Tcutil_tq2_const=0;}
# 721
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
# 722
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}
# 728
static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){
# 730
x=Cyc_Absyn_compress_conref(x);
# 731
y=Cyc_Absyn_compress_conref(y);
# 732
if(x == y)return;{
# 733
union Cyc_Absyn_Constraint*_tmp2D6=x;union Cyc_Absyn_Constraint _tmp2D7;int _tmp2D8;union Cyc_Absyn_Constraint _tmp2D9;void*_tmp2DA;union Cyc_Absyn_Constraint _tmp2DB;union Cyc_Absyn_Constraint*_tmp2DC;_LL15B: _tmp2D7=*_tmp2D6;if((_tmp2D7.No_constr).tag != 3)goto _LL15D;_tmp2D8=(int)(_tmp2D7.No_constr).val;_LL15C:
# 735
{union Cyc_Absyn_Constraint _tmp11A1;*x=(((_tmp11A1.Forward_constr).val=y,(((_tmp11A1.Forward_constr).tag=2,_tmp11A1))));}return;_LL15D: _tmp2D9=*_tmp2D6;if((_tmp2D9.Eq_constr).tag != 1)goto _LL15F;_tmp2DA=(void*)(_tmp2D9.Eq_constr).val;_LL15E: {
# 737
union Cyc_Absyn_Constraint*_tmp2DE=y;union Cyc_Absyn_Constraint _tmp2DF;int _tmp2E0;union Cyc_Absyn_Constraint _tmp2E1;void*_tmp2E2;union Cyc_Absyn_Constraint _tmp2E3;union Cyc_Absyn_Constraint*_tmp2E4;_LL162: _tmp2DF=*_tmp2DE;if((_tmp2DF.No_constr).tag != 3)goto _LL164;_tmp2E0=(int)(_tmp2DF.No_constr).val;_LL163:
# 738
*y=*x;return;_LL164: _tmp2E1=*_tmp2DE;if((_tmp2E1.Eq_constr).tag != 1)goto _LL166;_tmp2E2=(void*)(_tmp2E1.Eq_constr).val;_LL165:
# 740
 if(cmp(_tmp2DA,_tmp2E2)!= 0){
# 741
Cyc_Tcutil_failure_reason=reason;
# 742
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 744
return;_LL166: _tmp2E3=*_tmp2DE;if((_tmp2E3.Forward_constr).tag != 2)goto _LL161;_tmp2E4=(union Cyc_Absyn_Constraint*)(_tmp2E3.Forward_constr).val;_LL167: {
# 745
const char*_tmp11A4;void*_tmp11A3;(_tmp11A3=0,Cyc_Tcutil_impos(((_tmp11A4="unify_conref: forward after compress(2)",_tag_dyneither(_tmp11A4,sizeof(char),40))),_tag_dyneither(_tmp11A3,sizeof(void*),0)));}_LL161:;}_LL15F: _tmp2DB=*_tmp2D6;if((_tmp2DB.Forward_constr).tag != 2)goto _LL15A;_tmp2DC=(union Cyc_Absyn_Constraint*)(_tmp2DB.Forward_constr).val;_LL160: {
# 747
const char*_tmp11A7;void*_tmp11A6;(_tmp11A6=0,Cyc_Tcutil_impos(((_tmp11A7="unify_conref: forward after compress",_tag_dyneither(_tmp11A7,sizeof(char),37))),_tag_dyneither(_tmp11A6,sizeof(void*),0)));}_LL15A:;};}
# 751
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
# 752
struct _handler_cons _tmp2E9;_push_handler(& _tmp2E9);{int _tmp2EB=0;if(setjmp(_tmp2E9.handler))_tmp2EB=1;if(!_tmp2EB){
# 753
Cyc_Tcutil_unify_it_conrefs(cmp,x,y,_tag_dyneither(0,0,0));{
# 754
int _tmp2EC=1;_npop_handler(0);return _tmp2EC;};
# 753
;_pop_handler();}else{void*_tmp2EA=(void*)_exn_thrown;void*_tmp2EE=_tmp2EA;_LL169: {struct Cyc_Tcutil_Unify_exn_struct*_tmp2EF=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp2EE;if(_tmp2EF->tag != Cyc_Tcutil_Unify)goto _LL16B;}_LL16A:
# 755
 return 0;_LL16B:;_LL16C:(void)_throw(_tmp2EE);_LL168:;}};}
# 758
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
# 759
struct _tuple0 _tmp11A8;struct _tuple0 _tmp2F1=(_tmp11A8.f1=b1,((_tmp11A8.f2=b2,_tmp11A8)));void*_tmp2F2;void*_tmp2F4;void*_tmp2F6;void*_tmp2F8;void*_tmp2FA;struct Cyc_Absyn_Exp*_tmp2FC;void*_tmp2FD;struct Cyc_Absyn_Exp*_tmp2FF;_LL16E: _tmp2F2=_tmp2F1.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2F3=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2F2;if(_tmp2F3->tag != 0)goto _LL170;};_tmp2F4=_tmp2F1.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2F5=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2F4;if(_tmp2F5->tag != 0)goto _LL170;};_LL16F:
# 760
 return 0;_LL170: _tmp2F6=_tmp2F1.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2F7=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2F6;if(_tmp2F7->tag != 0)goto _LL172;};_LL171:
# 761
 return - 1;_LL172: _tmp2F8=_tmp2F1.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2F9=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2F8;if(_tmp2F9->tag != 0)goto _LL174;};_LL173:
# 762
 return 1;_LL174: _tmp2FA=_tmp2F1.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2FB=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2FA;if(_tmp2FB->tag != 1)goto _LL16D;else{_tmp2FC=_tmp2FB->f1;}};_tmp2FD=_tmp2F1.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2FE=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2FD;if(_tmp2FE->tag != 1)goto _LL16D;else{_tmp2FF=_tmp2FE->f1;}};_LL175:
# 764
 return Cyc_Evexp_const_exp_cmp(_tmp2FC,_tmp2FF);_LL16D:;}
# 768
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
# 769
struct _tuple0 _tmp11A9;struct _tuple0 _tmp301=(_tmp11A9.f1=b1,((_tmp11A9.f2=b2,_tmp11A9)));void*_tmp302;void*_tmp304;void*_tmp306;void*_tmp308;void*_tmp30A;struct Cyc_Absyn_Exp*_tmp30C;void*_tmp30D;struct Cyc_Absyn_Exp*_tmp30F;_LL177: _tmp302=_tmp301.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp303=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp302;if(_tmp303->tag != 0)goto _LL179;};_tmp304=_tmp301.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp305=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp304;if(_tmp305->tag != 0)goto _LL179;};_LL178:
# 770
 return 0;_LL179: _tmp306=_tmp301.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp307=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp306;if(_tmp307->tag != 0)goto _LL17B;};_LL17A:
# 771
 return - 1;_LL17B: _tmp308=_tmp301.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp309=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp308;if(_tmp309->tag != 0)goto _LL17D;};_LL17C:
# 772
 return 1;_LL17D: _tmp30A=_tmp301.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp30B=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp30A;if(_tmp30B->tag != 1)goto _LL176;else{_tmp30C=_tmp30B->f1;}};_tmp30D=_tmp301.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp30E=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp30D;if(_tmp30E->tag != 1)goto _LL176;else{_tmp30F=_tmp30E->f1;}};_LL17E:
# 773
 return Cyc_Evexp_const_exp_cmp(_tmp30C,_tmp30F);_LL176:;}
# 777
static int Cyc_Tcutil_attribute_case_number(void*att){
# 778
void*_tmp310=att;_LL180: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp311=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp310;if(_tmp311->tag != 0)goto _LL182;}_LL181:
# 779
 return 0;_LL182: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp312=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp310;if(_tmp312->tag != 1)goto _LL184;}_LL183:
# 780
 return 1;_LL184: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp313=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp310;if(_tmp313->tag != 2)goto _LL186;}_LL185:
# 781
 return 2;_LL186: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp314=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp310;if(_tmp314->tag != 3)goto _LL188;}_LL187:
# 782
 return 3;_LL188: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp315=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp310;if(_tmp315->tag != 4)goto _LL18A;}_LL189:
# 783
 return 4;_LL18A: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp316=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp310;if(_tmp316->tag != 5)goto _LL18C;}_LL18B:
# 784
 return 5;_LL18C: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp317=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp310;if(_tmp317->tag != 6)goto _LL18E;}_LL18D:
# 785
 return 6;_LL18E: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp318=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp310;if(_tmp318->tag != 7)goto _LL190;}_LL18F:
# 786
 return 7;_LL190: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp319=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp310;if(_tmp319->tag != 8)goto _LL192;}_LL191:
# 787
 return 8;_LL192: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp31A=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp310;if(_tmp31A->tag != 9)goto _LL194;}_LL193:
# 788
 return 9;_LL194: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp31B=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp310;if(_tmp31B->tag != 10)goto _LL196;}_LL195:
# 789
 return 10;_LL196: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp31C=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp310;if(_tmp31C->tag != 11)goto _LL198;}_LL197:
# 790
 return 11;_LL198: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp31D=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp310;if(_tmp31D->tag != 12)goto _LL19A;}_LL199:
# 791
 return 12;_LL19A: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp31E=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp310;if(_tmp31E->tag != 13)goto _LL19C;}_LL19B:
# 792
 return 13;_LL19C: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp31F=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp310;if(_tmp31F->tag != 14)goto _LL19E;}_LL19D:
# 793
 return 14;_LL19E: {struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*_tmp320=(struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*)_tmp310;if(_tmp320->tag != 15)goto _LL1A0;}_LL19F:
# 794
 return 15;_LL1A0: {struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*_tmp321=(struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*)_tmp310;if(_tmp321->tag != 16)goto _LL1A2;}_LL1A1:
# 795
 return 16;_LL1A2: {struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*_tmp322=(struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*)_tmp310;if(_tmp322->tag != 17)goto _LL1A4;}_LL1A3:
# 796
 return 17;_LL1A4: {struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*_tmp323=(struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*)_tmp310;if(_tmp323->tag != 18)goto _LL1A6;}_LL1A5:
# 797
 return 18;_LL1A6: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp324=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp310;if(_tmp324->tag != 19)goto _LL1A8;}_LL1A7:
# 798
 return 19;_LL1A8: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp325=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp310;if(_tmp325->tag != 20)goto _LL1AA;}_LL1A9:
# 799
 return 20;_LL1AA:;_LL1AB:
# 800
 return 21;_LL17F:;}
# 804
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
# 805
struct _tuple0 _tmp11AA;struct _tuple0 _tmp327=(_tmp11AA.f1=att1,((_tmp11AA.f2=att2,_tmp11AA)));void*_tmp328;int _tmp32A;void*_tmp32B;int _tmp32D;void*_tmp32E;int _tmp330;void*_tmp331;int _tmp333;void*_tmp334;int _tmp336;void*_tmp337;int _tmp339;void*_tmp33A;struct _dyneither_ptr _tmp33C;void*_tmp33D;struct _dyneither_ptr _tmp33F;void*_tmp340;enum Cyc_Absyn_Format_Type _tmp342;int _tmp343;int _tmp344;void*_tmp345;enum Cyc_Absyn_Format_Type _tmp347;int _tmp348;int _tmp349;_LL1AD: _tmp328=_tmp327.f1;{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp329=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp328;if(_tmp329->tag != 0)goto _LL1AF;else{_tmp32A=_tmp329->f1;}};_tmp32B=_tmp327.f2;{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp32C=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp32B;if(_tmp32C->tag != 0)goto _LL1AF;else{_tmp32D=_tmp32C->f1;}};_LL1AE:
# 806
 _tmp330=_tmp32A;_tmp333=_tmp32D;goto _LL1B0;_LL1AF: _tmp32E=_tmp327.f1;{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp32F=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp32E;if(_tmp32F->tag != 20)goto _LL1B1;else{_tmp330=_tmp32F->f1;}};_tmp331=_tmp327.f2;{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp332=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp331;if(_tmp332->tag != 20)goto _LL1B1;else{_tmp333=_tmp332->f1;}};_LL1B0:
# 807
 _tmp336=_tmp330;_tmp339=_tmp333;goto _LL1B2;_LL1B1: _tmp334=_tmp327.f1;{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp335=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp334;if(_tmp335->tag != 6)goto _LL1B3;else{_tmp336=_tmp335->f1;}};_tmp337=_tmp327.f2;{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp338=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp337;if(_tmp338->tag != 6)goto _LL1B3;else{_tmp339=_tmp338->f1;}};_LL1B2:
# 808
 return Cyc_Core_intcmp(_tmp336,_tmp339);_LL1B3: _tmp33A=_tmp327.f1;{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp33B=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp33A;if(_tmp33B->tag != 8)goto _LL1B5;else{_tmp33C=_tmp33B->f1;}};_tmp33D=_tmp327.f2;{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp33E=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp33D;if(_tmp33E->tag != 8)goto _LL1B5;else{_tmp33F=_tmp33E->f1;}};_LL1B4:
# 809
 return Cyc_strcmp((struct _dyneither_ptr)_tmp33C,(struct _dyneither_ptr)_tmp33F);_LL1B5: _tmp340=_tmp327.f1;{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp341=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp340;if(_tmp341->tag != 19)goto _LL1B7;else{_tmp342=_tmp341->f1;_tmp343=_tmp341->f2;_tmp344=_tmp341->f3;}};_tmp345=_tmp327.f2;{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp346=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp345;if(_tmp346->tag != 19)goto _LL1B7;else{_tmp347=_tmp346->f1;_tmp348=_tmp346->f2;_tmp349=_tmp346->f3;}};_LL1B6: {
# 811
int _tmp34A=Cyc_Core_intcmp((int)((unsigned int)_tmp342),(int)((unsigned int)_tmp347));
# 812
if(_tmp34A != 0)return _tmp34A;{
# 813
int _tmp34B=Cyc_Core_intcmp(_tmp343,_tmp348);
# 814
if(_tmp34B != 0)return _tmp34B;
# 815
return Cyc_Core_intcmp(_tmp344,_tmp349);};}_LL1B7:;_LL1B8:
# 817
 return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));_LL1AC:;}
# 821
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
# 822
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 825
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
# 826
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
# 827
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
# 828
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
# 829
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
# 830
return 1;}
# 834
static void*Cyc_Tcutil_rgns_of(void*t);
# 836
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
# 837
return Cyc_Tcutil_rgns_of(af->type);}
# 840
static struct _tuple16*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
# 841
void*t;
# 842
{struct Cyc_Absyn_Kind*_tmp34C=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind _tmp34D;enum Cyc_Absyn_KindQual _tmp34E;enum Cyc_Absyn_AliasQual _tmp34F;struct Cyc_Absyn_Kind _tmp350;enum Cyc_Absyn_KindQual _tmp351;enum Cyc_Absyn_AliasQual _tmp352;struct Cyc_Absyn_Kind _tmp353;enum Cyc_Absyn_KindQual _tmp354;struct Cyc_Absyn_Kind _tmp355;enum Cyc_Absyn_KindQual _tmp356;_LL1BA: _tmp34D=*_tmp34C;_tmp34E=_tmp34D.kind;if(_tmp34E != Cyc_Absyn_RgnKind)goto _LL1BC;_tmp34F=_tmp34D.aliasqual;if(_tmp34F != Cyc_Absyn_Unique)goto _LL1BC;_LL1BB:
# 843
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1B9;_LL1BC: _tmp350=*_tmp34C;_tmp351=_tmp350.kind;if(_tmp351 != Cyc_Absyn_RgnKind)goto _LL1BE;_tmp352=_tmp350.aliasqual;if(_tmp352 != Cyc_Absyn_Aliasable)goto _LL1BE;_LL1BD:
# 844
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL1B9;_LL1BE: _tmp353=*_tmp34C;_tmp354=_tmp353.kind;if(_tmp354 != Cyc_Absyn_EffKind)goto _LL1C0;_LL1BF:
# 845
 t=Cyc_Absyn_empty_effect;goto _LL1B9;_LL1C0: _tmp355=*_tmp34C;_tmp356=_tmp355.kind;if(_tmp356 != Cyc_Absyn_IntKind)goto _LL1C2;_LL1C1:
# 846
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp11AD;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp11AC;t=(void*)((_tmp11AC=_cycalloc(sizeof(*_tmp11AC)),((_tmp11AC[0]=((_tmp11AD.tag=18,((_tmp11AD.f1=Cyc_Absyn_uint_exp(0,0),_tmp11AD)))),_tmp11AC))));}goto _LL1B9;_LL1C2:;_LL1C3:
# 847
 t=Cyc_Absyn_sint_typ;goto _LL1B9;_LL1B9:;}{
# 849
struct _tuple16*_tmp11AE;return(_tmp11AE=_cycalloc(sizeof(*_tmp11AE)),((_tmp11AE->f1=tv,((_tmp11AE->f2=t,_tmp11AE)))));};}
# 856
static void*Cyc_Tcutil_rgns_of(void*t){
# 857
void*_tmp35A=Cyc_Tcutil_compress(t);void*_tmp364;void*_tmp366;void*_tmp367;struct Cyc_Absyn_DatatypeInfo _tmp369;struct Cyc_List_List*_tmp36A;struct Cyc_Absyn_PtrInfo _tmp36C;void*_tmp36D;struct Cyc_Absyn_PtrAtts _tmp36E;void*_tmp36F;struct Cyc_Absyn_ArrayInfo _tmp371;void*_tmp372;struct Cyc_List_List*_tmp374;struct Cyc_Absyn_DatatypeFieldInfo _tmp376;struct Cyc_List_List*_tmp377;struct Cyc_Absyn_AggrInfo _tmp379;struct Cyc_List_List*_tmp37A;struct Cyc_List_List*_tmp37C;struct Cyc_Absyn_FnInfo _tmp37F;struct Cyc_List_List*_tmp380;void*_tmp381;struct Cyc_Absyn_Tqual _tmp382;void*_tmp383;struct Cyc_List_List*_tmp384;struct Cyc_Absyn_VarargInfo*_tmp385;struct Cyc_List_List*_tmp386;void*_tmp38D;struct Cyc_List_List*_tmp38F;_LL1C5: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp35B=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp35A;if(_tmp35B->tag != 0)goto _LL1C7;}_LL1C6:
# 858
 goto _LL1C8;_LL1C7: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp35C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp35A;if(_tmp35C->tag != 7)goto _LL1C9;}_LL1C8:
# 859
 goto _LL1CA;_LL1C9: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp35D=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp35A;if(_tmp35D->tag != 13)goto _LL1CB;}_LL1CA:
# 860
 goto _LL1CC;_LL1CB: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp35E=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp35A;if(_tmp35E->tag != 14)goto _LL1CD;}_LL1CC:
# 861
 goto _LL1CE;_LL1CD: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp35F=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp35A;if(_tmp35F->tag != 18)goto _LL1CF;}_LL1CE:
# 862
 goto _LL1D0;_LL1CF: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp360=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp35A;if(_tmp360->tag != 6)goto _LL1D1;}_LL1D0:
# 863
 return Cyc_Absyn_empty_effect;_LL1D1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp361=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp35A;if(_tmp361->tag != 1)goto _LL1D3;}_LL1D2:
# 864
 goto _LL1D4;_LL1D3: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp362=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp35A;if(_tmp362->tag != 2)goto _LL1D5;}_LL1D4: {
# 866
struct Cyc_Absyn_Kind*_tmp391=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind _tmp392;enum Cyc_Absyn_KindQual _tmp393;struct Cyc_Absyn_Kind _tmp394;enum Cyc_Absyn_KindQual _tmp395;struct Cyc_Absyn_Kind _tmp396;enum Cyc_Absyn_KindQual _tmp397;_LL1FC: _tmp392=*_tmp391;_tmp393=_tmp392.kind;if(_tmp393 != Cyc_Absyn_RgnKind)goto _LL1FE;_LL1FD: {
# 867
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11B1;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11B0;return(void*)((_tmp11B0=_cycalloc(sizeof(*_tmp11B0)),((_tmp11B0[0]=((_tmp11B1.tag=23,((_tmp11B1.f1=(void*)t,_tmp11B1)))),_tmp11B0))));}_LL1FE: _tmp394=*_tmp391;_tmp395=_tmp394.kind;if(_tmp395 != Cyc_Absyn_EffKind)goto _LL200;_LL1FF:
# 868
 return t;_LL200: _tmp396=*_tmp391;_tmp397=_tmp396.kind;if(_tmp397 != Cyc_Absyn_IntKind)goto _LL202;_LL201:
# 869
 return Cyc_Absyn_empty_effect;_LL202:;_LL203: {
# 870
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp11B4;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp11B3;return(void*)((_tmp11B3=_cycalloc(sizeof(*_tmp11B3)),((_tmp11B3[0]=((_tmp11B4.tag=25,((_tmp11B4.f1=(void*)t,_tmp11B4)))),_tmp11B3))));}_LL1FB:;}_LL1D5: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp363=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp35A;if(_tmp363->tag != 15)goto _LL1D7;else{_tmp364=(void*)_tmp363->f1;}}_LL1D6: {
# 872
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11B7;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11B6;return(void*)((_tmp11B6=_cycalloc(sizeof(*_tmp11B6)),((_tmp11B6[0]=((_tmp11B7.tag=23,((_tmp11B7.f1=(void*)_tmp364,_tmp11B7)))),_tmp11B6))));}_LL1D7: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp365=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp35A;if(_tmp365->tag != 16)goto _LL1D9;else{_tmp366=(void*)_tmp365->f1;_tmp367=(void*)_tmp365->f2;}}_LL1D8: {
# 876
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11BA;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11B9;return(void*)((_tmp11B9=_cycalloc(sizeof(*_tmp11B9)),((_tmp11B9[0]=((_tmp11BA.tag=23,((_tmp11BA.f1=(void*)_tmp367,_tmp11BA)))),_tmp11B9))));}_LL1D9: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp368=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp35A;if(_tmp368->tag != 3)goto _LL1DB;else{_tmp369=_tmp368->f1;_tmp36A=_tmp369.targs;}}_LL1DA: {
# 878
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp36A);
# 879
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11BD;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11BC;return Cyc_Tcutil_normalize_effect((void*)((_tmp11BC=_cycalloc(sizeof(*_tmp11BC)),((_tmp11BC[0]=((_tmp11BD.tag=24,((_tmp11BD.f1=ts,_tmp11BD)))),_tmp11BC)))));}_LL1DB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp36B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35A;if(_tmp36B->tag != 5)goto _LL1DD;else{_tmp36C=_tmp36B->f1;_tmp36D=_tmp36C.elt_typ;_tmp36E=_tmp36C.ptr_atts;_tmp36F=_tmp36E.rgn;}}_LL1DC: {
# 882
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11CC;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11CB;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11CA;void*_tmp11C9[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11C8;return Cyc_Tcutil_normalize_effect((void*)((_tmp11C8=_cycalloc(sizeof(*_tmp11C8)),((_tmp11C8[0]=((_tmp11CC.tag=24,((_tmp11CC.f1=((_tmp11C9[1]=Cyc_Tcutil_rgns_of(_tmp36D),((_tmp11C9[0]=(void*)((_tmp11CB=_cycalloc(sizeof(*_tmp11CB)),((_tmp11CB[0]=((_tmp11CA.tag=23,((_tmp11CA.f1=(void*)_tmp36F,_tmp11CA)))),_tmp11CB)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp11C9,sizeof(void*),2)))))),_tmp11CC)))),_tmp11C8)))));}_LL1DD: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp370=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp35A;if(_tmp370->tag != 8)goto _LL1DF;else{_tmp371=_tmp370->f1;_tmp372=_tmp371.elt_type;}}_LL1DE:
# 884
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp372));_LL1DF: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp373=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp35A;if(_tmp373->tag != 10)goto _LL1E1;else{_tmp374=_tmp373->f1;}}_LL1E0: {
# 886
struct Cyc_List_List*_tmp3A7=0;
# 887
for(0;_tmp374 != 0;_tmp374=_tmp374->tl){
# 888
struct Cyc_List_List*_tmp11CD;_tmp3A7=((_tmp11CD=_cycalloc(sizeof(*_tmp11CD)),((_tmp11CD->hd=(*((struct _tuple12*)_tmp374->hd)).f2,((_tmp11CD->tl=_tmp3A7,_tmp11CD))))));}
# 889
_tmp377=_tmp3A7;goto _LL1E2;}_LL1E1: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp375=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp35A;if(_tmp375->tag != 4)goto _LL1E3;else{_tmp376=_tmp375->f1;_tmp377=_tmp376.targs;}}_LL1E2:
# 890
 _tmp37A=_tmp377;goto _LL1E4;_LL1E3: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp378=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp35A;if(_tmp378->tag != 11)goto _LL1E5;else{_tmp379=_tmp378->f1;_tmp37A=_tmp379.targs;}}_LL1E4: {
# 892
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11D0;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11CF;return Cyc_Tcutil_normalize_effect((void*)((_tmp11CF=_cycalloc(sizeof(*_tmp11CF)),((_tmp11CF[0]=((_tmp11D0.tag=24,((_tmp11D0.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp37A),_tmp11D0)))),_tmp11CF)))));}_LL1E5: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp37B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp35A;if(_tmp37B->tag != 12)goto _LL1E7;else{_tmp37C=_tmp37B->f2;}}_LL1E6: {
# 894
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11D3;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11D2;return Cyc_Tcutil_normalize_effect((void*)((_tmp11D2=_cycalloc(sizeof(*_tmp11D2)),((_tmp11D2[0]=((_tmp11D3.tag=24,((_tmp11D3.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp37C),_tmp11D3)))),_tmp11D2)))));}_LL1E7: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp37D=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp35A;if(_tmp37D->tag != 19)goto _LL1E9;}_LL1E8:
# 895
 return Cyc_Absyn_empty_effect;_LL1E9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp37E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp35A;if(_tmp37E->tag != 9)goto _LL1EB;else{_tmp37F=_tmp37E->f1;_tmp380=_tmp37F.tvars;_tmp381=_tmp37F.effect;_tmp382=_tmp37F.ret_tqual;_tmp383=_tmp37F.ret_typ;_tmp384=_tmp37F.args;_tmp385=_tmp37F.cyc_varargs;_tmp386=_tmp37F.rgn_po;}}_LL1EA: {
# 904
void*_tmp3AD=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp380),(void*)_check_null(_tmp381));
# 905
return Cyc_Tcutil_normalize_effect(_tmp3AD);}_LL1EB: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp387=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp35A;if(_tmp387->tag != 21)goto _LL1ED;}_LL1EC:
# 906
 goto _LL1EE;_LL1ED: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp388=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp35A;if(_tmp388->tag != 22)goto _LL1EF;}_LL1EE:
# 907
 goto _LL1F0;_LL1EF: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp389=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp35A;if(_tmp389->tag != 20)goto _LL1F1;}_LL1F0:
# 908
 return Cyc_Absyn_empty_effect;_LL1F1: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp38A=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp35A;if(_tmp38A->tag != 23)goto _LL1F3;}_LL1F2:
# 909
 goto _LL1F4;_LL1F3: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp38B=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp35A;if(_tmp38B->tag != 24)goto _LL1F5;}_LL1F4:
# 910
 return t;_LL1F5: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp38C=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp35A;if(_tmp38C->tag != 25)goto _LL1F7;else{_tmp38D=(void*)_tmp38C->f1;}}_LL1F6:
# 911
 return Cyc_Tcutil_rgns_of(_tmp38D);_LL1F7: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp38E=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp35A;if(_tmp38E->tag != 17)goto _LL1F9;else{_tmp38F=_tmp38E->f2;}}_LL1F8: {
# 913
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11D6;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11D5;return Cyc_Tcutil_normalize_effect((void*)((_tmp11D5=_cycalloc(sizeof(*_tmp11D5)),((_tmp11D5[0]=((_tmp11D6.tag=24,((_tmp11D6.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp38F),_tmp11D6)))),_tmp11D5)))));}_LL1F9: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp390=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp35A;if(_tmp390->tag != 26)goto _LL1C4;}_LL1FA: {
# 914
const char*_tmp11D9;void*_tmp11D8;(_tmp11D8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11D9="typedecl in rgns_of",_tag_dyneither(_tmp11D9,sizeof(char),20))),_tag_dyneither(_tmp11D8,sizeof(void*),0)));}_LL1C4:;}
# 921
void*Cyc_Tcutil_normalize_effect(void*e){
# 922
e=Cyc_Tcutil_compress(e);{
# 923
void*_tmp3B2=e;struct Cyc_List_List*_tmp3B4;struct Cyc_List_List**_tmp3B5;void*_tmp3B7;_LL205: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3B3=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3B2;if(_tmp3B3->tag != 24)goto _LL207;else{_tmp3B4=_tmp3B3->f1;_tmp3B5=(struct Cyc_List_List**)& _tmp3B3->f1;}}_LL206: {
# 925
int redo_join=0;
# 926
{struct Cyc_List_List*effs=*_tmp3B5;for(0;effs != 0;effs=effs->tl){
# 927
void*_tmp3B8=(void*)effs->hd;
# 928
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp3B8));{
# 929
void*_tmp3B9=(void*)effs->hd;void*_tmp3BC;void*_tmp3BF;void*_tmp3C2;_LL20C: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3BA=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3B9;if(_tmp3BA->tag != 24)goto _LL20E;}_LL20D:
# 930
 goto _LL20F;_LL20E: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3BB=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3B9;if(_tmp3BB->tag != 23)goto _LL210;else{_tmp3BC=(void*)_tmp3BB->f1;{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp3BD=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3BC;if(_tmp3BD->tag != 20)goto _LL210;};}}_LL20F:
# 931
 redo_join=1;goto _LL20B;_LL210: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3BE=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3B9;if(_tmp3BE->tag != 23)goto _LL212;else{_tmp3BF=(void*)_tmp3BE->f1;{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp3C0=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3BF;if(_tmp3C0->tag != 22)goto _LL212;};}}_LL211:
# 932
 redo_join=1;goto _LL20B;_LL212: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3C1=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3B9;if(_tmp3C1->tag != 23)goto _LL214;else{_tmp3C2=(void*)_tmp3C1->f1;{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp3C3=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3C2;if(_tmp3C3->tag != 21)goto _LL214;};}}_LL213:
# 933
 redo_join=1;goto _LL20B;_LL214:;_LL215:
# 934
 goto _LL20B;_LL20B:;};}}
# 937
if(!redo_join)return e;{
# 938
struct Cyc_List_List*effects=0;
# 939
{struct Cyc_List_List*effs=*_tmp3B5;for(0;effs != 0;effs=effs->tl){
# 940
void*_tmp3C4=Cyc_Tcutil_compress((void*)effs->hd);struct Cyc_List_List*_tmp3C6;void*_tmp3C8;void*_tmp3CB;void*_tmp3CE;_LL217: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3C5=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3C4;if(_tmp3C5->tag != 24)goto _LL219;else{_tmp3C6=_tmp3C5->f1;}}_LL218:
# 942
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp3C6,effects);
# 943
goto _LL216;_LL219: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3C7=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3C4;if(_tmp3C7->tag != 23)goto _LL21B;else{_tmp3C8=(void*)_tmp3C7->f1;{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp3C9=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3C8;if(_tmp3C9->tag != 20)goto _LL21B;};}}_LL21A:
# 944
 goto _LL216;_LL21B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3CA=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3C4;if(_tmp3CA->tag != 23)goto _LL21D;else{_tmp3CB=(void*)_tmp3CA->f1;{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp3CC=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3CB;if(_tmp3CC->tag != 22)goto _LL21D;};}}_LL21C:
# 945
 goto _LL216;_LL21D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3CD=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3C4;if(_tmp3CD->tag != 23)goto _LL21F;else{_tmp3CE=(void*)_tmp3CD->f1;{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp3CF=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3CE;if(_tmp3CF->tag != 21)goto _LL21F;};}}_LL21E:
# 946
 goto _LL216;_LL21F:;_LL220:
# 947
{struct Cyc_List_List*_tmp11DA;effects=((_tmp11DA=_cycalloc(sizeof(*_tmp11DA)),((_tmp11DA->hd=_tmp3C4,((_tmp11DA->tl=effects,_tmp11DA))))));}goto _LL216;_LL216:;}}
# 950
*_tmp3B5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
# 951
return e;};}_LL207: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3B6=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3B2;if(_tmp3B6->tag != 25)goto _LL209;else{_tmp3B7=(void*)_tmp3B6->f1;}}_LL208: {
# 953
void*_tmp3D1=Cyc_Tcutil_compress(_tmp3B7);_LL222: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3D2=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D1;if(_tmp3D2->tag != 1)goto _LL224;}_LL223:
# 954
 goto _LL225;_LL224: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3D3=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3D1;if(_tmp3D3->tag != 2)goto _LL226;}_LL225:
# 955
 return e;_LL226:;_LL227:
# 956
 return Cyc_Tcutil_rgns_of(_tmp3B7);_LL221:;}_LL209:;_LL20A:
# 958
 return e;_LL204:;};}
# 963
static void*Cyc_Tcutil_dummy_fntype(void*eff){
# 964
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp11E0;struct Cyc_Absyn_FnInfo _tmp11DF;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp11DE;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3D4=(_tmp11DE=_cycalloc(sizeof(*_tmp11DE)),((_tmp11DE[0]=((_tmp11E0.tag=9,((_tmp11E0.f1=((_tmp11DF.tvars=0,((_tmp11DF.effect=(void*)eff,((_tmp11DF.ret_tqual=
# 965
Cyc_Absyn_empty_tqual(0),((_tmp11DF.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmp11DF.args=0,((_tmp11DF.c_varargs=0,((_tmp11DF.cyc_varargs=0,((_tmp11DF.rgn_po=0,((_tmp11DF.attributes=0,_tmp11DF)))))))))))))))))),_tmp11E0)))),_tmp11DE)));
# 969
return Cyc_Absyn_atb_typ((void*)_tmp3D4,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
# 976
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
# 977
r=Cyc_Tcutil_compress(r);
# 978
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
# 979
return 1;{
# 980
void*_tmp3D8=Cyc_Tcutil_compress(e);void*_tmp3DA;struct Cyc_List_List*_tmp3DC;void*_tmp3DE;struct Cyc_Core_Opt*_tmp3E0;void*_tmp3E1;void**_tmp3E2;struct Cyc_Core_Opt*_tmp3E3;_LL229: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3D9=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3D8;if(_tmp3D9->tag != 23)goto _LL22B;else{_tmp3DA=(void*)_tmp3D9->f1;}}_LL22A:
# 983
 if(constrain)return Cyc_Tcutil_unify(r,_tmp3DA);
# 984
_tmp3DA=Cyc_Tcutil_compress(_tmp3DA);
# 985
if(r == _tmp3DA)return 1;{
# 986
struct _tuple0 _tmp11E1;struct _tuple0 _tmp3E5=(_tmp11E1.f1=r,((_tmp11E1.f2=_tmp3DA,_tmp11E1)));void*_tmp3E6;struct Cyc_Absyn_Tvar*_tmp3E8;void*_tmp3E9;struct Cyc_Absyn_Tvar*_tmp3EB;_LL234: _tmp3E6=_tmp3E5.f1;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3E7=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3E6;if(_tmp3E7->tag != 2)goto _LL236;else{_tmp3E8=_tmp3E7->f1;}};_tmp3E9=_tmp3E5.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3EA=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3E9;if(_tmp3EA->tag != 2)goto _LL236;else{_tmp3EB=_tmp3EA->f1;}};_LL235:
# 987
 return Cyc_Absyn_tvar_cmp(_tmp3E8,_tmp3EB)== 0;_LL236:;_LL237:
# 988
 return 0;_LL233:;};_LL22B: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3DB=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3D8;if(_tmp3DB->tag != 24)goto _LL22D;else{_tmp3DC=_tmp3DB->f1;}}_LL22C:
# 991
 for(0;_tmp3DC != 0;_tmp3DC=_tmp3DC->tl){
# 992
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp3DC->hd))return 1;}
# 993
return 0;_LL22D: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3DD=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3D8;if(_tmp3DD->tag != 25)goto _LL22F;else{_tmp3DE=(void*)_tmp3DD->f1;}}_LL22E: {
# 995
void*_tmp3EC=Cyc_Tcutil_rgns_of(_tmp3DE);void*_tmp3EE;_LL239: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3ED=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3EC;if(_tmp3ED->tag != 25)goto _LL23B;else{_tmp3EE=(void*)_tmp3ED->f1;}}_LL23A:
# 997
 if(!constrain)return 0;{
# 998
void*_tmp3EF=Cyc_Tcutil_compress(_tmp3EE);struct Cyc_Core_Opt*_tmp3F1;void*_tmp3F2;void**_tmp3F3;struct Cyc_Core_Opt*_tmp3F4;_LL23E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3F0=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3EF;if(_tmp3F0->tag != 1)goto _LL240;else{_tmp3F1=_tmp3F0->f1;_tmp3F2=(void**)& _tmp3F0->f2;_tmp3F3=(void**)((void**)& _tmp3F0->f2);_tmp3F4=_tmp3F0->f4;}}_LL23F: {
# 1002
void*_tmp3F5=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp3F4);
# 1005
Cyc_Tcutil_occurs(_tmp3F5,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3F4))->v,r);{
# 1006
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11F0;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11EF;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11EE;void*_tmp11ED[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11EC;void*_tmp3F6=Cyc_Tcutil_dummy_fntype((void*)((_tmp11EC=_cycalloc(sizeof(*_tmp11EC)),((_tmp11EC[0]=((_tmp11F0.tag=24,((_tmp11F0.f1=((_tmp11ED[1]=(void*)((_tmp11EF=_cycalloc(sizeof(*_tmp11EF)),((_tmp11EF[0]=((_tmp11EE.tag=23,((_tmp11EE.f1=(void*)r,_tmp11EE)))),_tmp11EF)))),((_tmp11ED[0]=_tmp3F5,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp11ED,sizeof(void*),2)))))),_tmp11F0)))),_tmp11EC)))));
# 1007
*_tmp3F3=(void*)_tmp3F6;
# 1008
return 1;};}_LL240:;_LL241:
# 1009
 return 0;_LL23D:;};_LL23B:;_LL23C:
# 1011
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp3EC);_LL238:;}_LL22F: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3DF=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D8;if(_tmp3DF->tag != 1)goto _LL231;else{_tmp3E0=_tmp3DF->f1;_tmp3E1=(void**)& _tmp3DF->f2;_tmp3E2=(void**)((void**)& _tmp3DF->f2);_tmp3E3=_tmp3DF->f4;}}_LL230:
# 1014
 if(_tmp3E0 == 0  || ((struct Cyc_Absyn_Kind*)_tmp3E0->v)->kind != Cyc_Absyn_EffKind){
# 1015
const char*_tmp11F3;void*_tmp11F2;(_tmp11F2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11F3="effect evar has wrong kind",_tag_dyneither(_tmp11F3,sizeof(char),27))),_tag_dyneither(_tmp11F2,sizeof(void*),0)));}
# 1016
if(!constrain)return 0;{
# 1019
void*_tmp3FE=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp3E3);
# 1022
Cyc_Tcutil_occurs(_tmp3FE,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3E3))->v,r);{
# 1023
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1208;struct Cyc_List_List*_tmp1207;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1206;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1205;struct Cyc_List_List*_tmp1204;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1203;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3FF=(_tmp1203=_cycalloc(sizeof(*_tmp1203)),((_tmp1203[0]=((_tmp1208.tag=24,((_tmp1208.f1=((_tmp1204=_cycalloc(sizeof(*_tmp1204)),((_tmp1204->hd=_tmp3FE,((_tmp1204->tl=((_tmp1207=_cycalloc(sizeof(*_tmp1207)),((_tmp1207->hd=(void*)((_tmp1205=_cycalloc(sizeof(*_tmp1205)),((_tmp1205[0]=((_tmp1206.tag=23,((_tmp1206.f1=(void*)r,_tmp1206)))),_tmp1205)))),((_tmp1207->tl=0,_tmp1207)))))),_tmp1204)))))),_tmp1208)))),_tmp1203)));
# 1024
*_tmp3E2=(void*)_tmp3FF;
# 1025
return 1;};};_LL231:;_LL232:
# 1026
 return 0;_LL228:;};}
# 1033
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
# 1034
t=Cyc_Tcutil_compress(t);{
# 1035
void*_tmp406=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp409;void*_tmp40B;struct Cyc_Core_Opt*_tmp40D;void*_tmp40E;void**_tmp40F;struct Cyc_Core_Opt*_tmp410;_LL243: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp407=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp406;if(_tmp407->tag != 23)goto _LL245;}_LL244:
# 1036
 return 0;_LL245: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp408=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp406;if(_tmp408->tag != 24)goto _LL247;else{_tmp409=_tmp408->f1;}}_LL246:
# 1038
 for(0;_tmp409 != 0;_tmp409=_tmp409->tl){
# 1039
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp409->hd))
# 1040
return 1;}
# 1041
return 0;_LL247: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp40A=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp406;if(_tmp40A->tag != 25)goto _LL249;else{_tmp40B=(void*)_tmp40A->f1;}}_LL248:
# 1043
 _tmp40B=Cyc_Tcutil_compress(_tmp40B);
# 1044
if(t == _tmp40B)return 1;
# 1045
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp40B);{
# 1046
void*_tmp411=Cyc_Tcutil_rgns_of(t);void*_tmp413;_LL24E: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp412=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp411;if(_tmp412->tag != 25)goto _LL250;else{_tmp413=(void*)_tmp412->f1;}}_LL24F: {
# 1048
struct _tuple0 _tmp1209;struct _tuple0 _tmp415=(_tmp1209.f1=Cyc_Tcutil_compress(_tmp413),((_tmp1209.f2=_tmp40B,_tmp1209)));void*_tmp416;struct Cyc_Absyn_Tvar*_tmp418;void*_tmp419;struct Cyc_Absyn_Tvar*_tmp41B;_LL253: _tmp416=_tmp415.f1;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp417=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp416;if(_tmp417->tag != 2)goto _LL255;else{_tmp418=_tmp417->f1;}};_tmp419=_tmp415.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp41A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp419;if(_tmp41A->tag != 2)goto _LL255;else{_tmp41B=_tmp41A->f1;}};_LL254:
# 1049
 return Cyc_Tcutil_unify(t,_tmp40B);_LL255:;_LL256:
# 1050
 return _tmp413 == _tmp40B;_LL252:;}_LL250:;_LL251:
# 1052
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp411);_LL24D:;};_LL249: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp40C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp406;if(_tmp40C->tag != 1)goto _LL24B;else{_tmp40D=_tmp40C->f1;_tmp40E=(void**)& _tmp40C->f2;_tmp40F=(void**)((void**)& _tmp40C->f2);_tmp410=_tmp40C->f4;}}_LL24A:
# 1055
 if(_tmp40D == 0  || ((struct Cyc_Absyn_Kind*)_tmp40D->v)->kind != Cyc_Absyn_EffKind){
# 1056
const char*_tmp120C;void*_tmp120B;(_tmp120B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp120C="effect evar has wrong kind",_tag_dyneither(_tmp120C,sizeof(char),27))),_tag_dyneither(_tmp120B,sizeof(void*),0)));}
# 1057
if(!may_constrain_evars)return 0;{
# 1060
void*_tmp41E=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp410);
# 1063
Cyc_Tcutil_occurs(_tmp41E,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp410))->v,t);{
# 1064
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1221;struct Cyc_List_List*_tmp1220;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp121F;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp121E;struct Cyc_List_List*_tmp121D;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp121C;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp41F=(_tmp121C=_cycalloc(sizeof(*_tmp121C)),((_tmp121C[0]=((_tmp1221.tag=24,((_tmp1221.f1=((_tmp121D=_cycalloc(sizeof(*_tmp121D)),((_tmp121D->hd=_tmp41E,((_tmp121D->tl=((_tmp1220=_cycalloc(sizeof(*_tmp1220)),((_tmp1220->hd=(void*)((_tmp121E=_cycalloc(sizeof(*_tmp121E)),((_tmp121E[0]=((_tmp121F.tag=25,((_tmp121F.f1=(void*)t,_tmp121F)))),_tmp121E)))),((_tmp1220->tl=0,_tmp1220)))))),_tmp121D)))))),_tmp1221)))),_tmp121C)));
# 1065
*_tmp40F=(void*)_tmp41F;
# 1066
return 1;};};_LL24B:;_LL24C:
# 1067
 return 0;_LL242:;};}
# 1074
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
# 1075
e=Cyc_Tcutil_compress(e);{
# 1076
void*_tmp426=e;struct Cyc_Absyn_Tvar*_tmp428;struct Cyc_List_List*_tmp42A;void*_tmp42C;struct Cyc_Core_Opt*_tmp42E;void*_tmp42F;void**_tmp430;struct Cyc_Core_Opt*_tmp431;_LL258: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp427=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp426;if(_tmp427->tag != 2)goto _LL25A;else{_tmp428=_tmp427->f1;}}_LL259:
# 1077
 return Cyc_Absyn_tvar_cmp(v,_tmp428)== 0;_LL25A: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp429=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp426;if(_tmp429->tag != 24)goto _LL25C;else{_tmp42A=_tmp429->f1;}}_LL25B:
# 1079
 for(0;_tmp42A != 0;_tmp42A=_tmp42A->tl){
# 1080
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp42A->hd))
# 1081
return 1;}
# 1082
return 0;_LL25C: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp42B=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp426;if(_tmp42B->tag != 25)goto _LL25E;else{_tmp42C=(void*)_tmp42B->f1;}}_LL25D: {
# 1084
void*_tmp432=Cyc_Tcutil_rgns_of(_tmp42C);void*_tmp434;_LL263: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp433=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp432;if(_tmp433->tag != 25)goto _LL265;else{_tmp434=(void*)_tmp433->f1;}}_LL264:
# 1086
 if(!may_constrain_evars)return 0;{
# 1087
void*_tmp435=Cyc_Tcutil_compress(_tmp434);struct Cyc_Core_Opt*_tmp437;void*_tmp438;void**_tmp439;struct Cyc_Core_Opt*_tmp43A;_LL268: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp436=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp435;if(_tmp436->tag != 1)goto _LL26A;else{_tmp437=_tmp436->f1;_tmp438=(void**)& _tmp436->f2;_tmp439=(void**)((void**)& _tmp436->f2);_tmp43A=_tmp436->f4;}}_LL269: {
# 1092
void*_tmp43B=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp43A);
# 1094
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp43A))->v,v))return 0;{
# 1095
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1230;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp122F;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp122E;void*_tmp122D[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp122C;void*_tmp43C=Cyc_Tcutil_dummy_fntype((void*)((_tmp122C=_cycalloc(sizeof(*_tmp122C)),((_tmp122C[0]=((_tmp1230.tag=24,((_tmp1230.f1=((_tmp122D[1]=(void*)((_tmp122F=_cycalloc(sizeof(*_tmp122F)),((_tmp122F[0]=((_tmp122E.tag=2,((_tmp122E.f1=v,_tmp122E)))),_tmp122F)))),((_tmp122D[0]=_tmp43B,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp122D,sizeof(void*),2)))))),_tmp1230)))),_tmp122C)))));
# 1096
*_tmp439=(void*)_tmp43C;
# 1097
return 1;};}_LL26A:;_LL26B:
# 1098
 return 0;_LL267:;};_LL265:;_LL266:
# 1100
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp432);_LL262:;}_LL25E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp42D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp426;if(_tmp42D->tag != 1)goto _LL260;else{_tmp42E=_tmp42D->f1;_tmp42F=(void**)& _tmp42D->f2;_tmp430=(void**)((void**)& _tmp42D->f2);_tmp431=_tmp42D->f4;}}_LL25F:
# 1103
 if(_tmp42E == 0  || ((struct Cyc_Absyn_Kind*)_tmp42E->v)->kind != Cyc_Absyn_EffKind){
# 1104
const char*_tmp1233;void*_tmp1232;(_tmp1232=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1233="effect evar has wrong kind",_tag_dyneither(_tmp1233,sizeof(char),27))),_tag_dyneither(_tmp1232,sizeof(void*),0)));}{
# 1107
void*_tmp444=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp431);
# 1109
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp431))->v,v))
# 1110
return 0;{
# 1111
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1248;struct Cyc_List_List*_tmp1247;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1246;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1245;struct Cyc_List_List*_tmp1244;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1243;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp445=(_tmp1243=_cycalloc(sizeof(*_tmp1243)),((_tmp1243[0]=((_tmp1248.tag=24,((_tmp1248.f1=((_tmp1244=_cycalloc(sizeof(*_tmp1244)),((_tmp1244->hd=_tmp444,((_tmp1244->tl=((_tmp1247=_cycalloc(sizeof(*_tmp1247)),((_tmp1247->hd=(void*)((_tmp1245=_cycalloc(sizeof(*_tmp1245)),((_tmp1245[0]=((_tmp1246.tag=2,((_tmp1246.f1=v,_tmp1246)))),_tmp1245)))),((_tmp1247->tl=0,_tmp1247)))))),_tmp1244)))))),_tmp1248)))),_tmp1243)));
# 1112
*_tmp430=(void*)_tmp445;
# 1113
return 1;};};_LL260:;_LL261:
# 1114
 return 0;_LL257:;};}
# 1119
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
# 1120
e=Cyc_Tcutil_compress(e);{
# 1121
void*_tmp44C=e;struct Cyc_List_List*_tmp44E;void*_tmp450;_LL26D: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp44D=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp44C;if(_tmp44D->tag != 24)goto _LL26F;else{_tmp44E=_tmp44D->f1;}}_LL26E:
# 1123
 for(0;_tmp44E != 0;_tmp44E=_tmp44E->tl){
# 1124
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp44E->hd))
# 1125
return 1;}
# 1126
return 0;_LL26F: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp44F=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp44C;if(_tmp44F->tag != 25)goto _LL271;else{_tmp450=(void*)_tmp44F->f1;}}_LL270: {
# 1128
void*_tmp452=Cyc_Tcutil_rgns_of(_tmp450);void*_tmp454;_LL276: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp453=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp452;if(_tmp453->tag != 25)goto _LL278;else{_tmp454=(void*)_tmp453->f1;}}_LL277:
# 1129
 return 0;_LL278:;_LL279:
# 1130
 return Cyc_Tcutil_evar_in_effect(evar,_tmp452);_LL275:;}_LL271: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp451=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp44C;if(_tmp451->tag != 1)goto _LL273;}_LL272:
# 1132
 return evar == e;_LL273:;_LL274:
# 1133
 return 0;_LL26C:;};}
# 1146 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1149
void*_tmp455=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp457;void*_tmp459;struct Cyc_Absyn_Tvar*_tmp45B;void*_tmp45D;void*_tmp45F;void**_tmp460;struct Cyc_Core_Opt*_tmp461;_LL27B: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp456=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp455;if(_tmp456->tag != 24)goto _LL27D;else{_tmp457=_tmp456->f1;}}_LL27C:
# 1151
 for(0;_tmp457 != 0;_tmp457=_tmp457->tl){
# 1152
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp457->hd,e2))
# 1153
return 0;}
# 1154
return 1;_LL27D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp458=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp455;if(_tmp458->tag != 23)goto _LL27F;else{_tmp459=(void*)_tmp458->f1;}}_LL27E:
# 1162
 return Cyc_Tcutil_region_in_effect(0,_tmp459,e2) || 
# 1163
may_constrain_evars  && Cyc_Tcutil_unify(_tmp459,(void*)& Cyc_Absyn_HeapRgn_val);_LL27F: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp45A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp455;if(_tmp45A->tag != 2)goto _LL281;else{_tmp45B=_tmp45A->f1;}}_LL280:
# 1164
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp45B,e2);_LL281: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp45C=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp455;if(_tmp45C->tag != 25)goto _LL283;else{_tmp45D=(void*)_tmp45C->f1;}}_LL282: {
# 1166
void*_tmp462=Cyc_Tcutil_rgns_of(_tmp45D);void*_tmp464;_LL288: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp463=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp462;if(_tmp463->tag != 25)goto _LL28A;else{_tmp464=(void*)_tmp463->f1;}}_LL289:
# 1171
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp464,e2) || 
# 1172
may_constrain_evars  && Cyc_Tcutil_unify(_tmp464,Cyc_Absyn_sint_typ);_LL28A:;_LL28B:
# 1173
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp462,e2);_LL287:;}_LL283: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp45E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp455;if(_tmp45E->tag != 1)goto _LL285;else{_tmp45F=(void**)& _tmp45E->f2;_tmp460=(void**)((void**)& _tmp45E->f2);_tmp461=_tmp45E->f4;}}_LL284:
# 1176
 if(!Cyc_Tcutil_evar_in_effect(e1,e2))
# 1180
*_tmp460=(void*)Cyc_Absyn_empty_effect;
# 1184
return 1;_LL285:;_LL286: {
# 1185
const char*_tmp124C;void*_tmp124B[1];struct Cyc_String_pa_PrintArg_struct _tmp124A;(_tmp124A.tag=0,((_tmp124A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((_tmp124B[0]=& _tmp124A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp124C="subset_effect: bad effect: %s",_tag_dyneither(_tmp124C,sizeof(char),30))),_tag_dyneither(_tmp124B,sizeof(void*),1)))))));}_LL27A:;}
# 1200 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
# 1201
e1=Cyc_Tcutil_normalize_effect(e1);
# 1202
e2=Cyc_Tcutil_normalize_effect(e2);
# 1203
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
# 1204
return 1;
# 1205
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
# 1206
return 1;
# 1207
return 0;}
# 1216
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1218
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
# 1219
struct _tuple0 _tmp469;void*_tmp46A;void*_tmp46B;struct _tuple0*_tmp468=(struct _tuple0*)r1->hd;_tmp469=*_tmp468;_tmp46A=_tmp469.f1;_tmp46B=_tmp469.f2;{
# 1220
int found=_tmp46A == (void*)& Cyc_Absyn_HeapRgn_val;
# 1221
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
# 1222
struct _tuple0 _tmp46D;void*_tmp46E;void*_tmp46F;struct _tuple0*_tmp46C=(struct _tuple0*)r2->hd;_tmp46D=*_tmp46C;_tmp46E=_tmp46D.f1;_tmp46F=_tmp46D.f2;
# 1223
if(Cyc_Tcutil_unify(_tmp46A,_tmp46E) && Cyc_Tcutil_unify(_tmp46B,_tmp46F)){
# 1224
found=1;
# 1225
break;}}}
# 1228
if(!found)return 0;};}}
# 1230
return 1;}
# 1234
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1236
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple22{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1240
void Cyc_Tcutil_unify_it(void*t1,void*t2){
# 1241
Cyc_Tcutil_t1_failure=t1;
# 1242
Cyc_Tcutil_t2_failure=t2;
# 1243
Cyc_Tcutil_failure_reason=_tag_dyneither(0,0,0);
# 1244
t1=Cyc_Tcutil_compress(t1);
# 1245
t2=Cyc_Tcutil_compress(t2);
# 1246
if(t1 == t2)return;
# 1247
{void*_tmp470=t1;struct Cyc_Core_Opt*_tmp472;void*_tmp473;void**_tmp474;struct Cyc_Core_Opt*_tmp475;_LL28D: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp471=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp470;if(_tmp471->tag != 1)goto _LL28F;else{_tmp472=_tmp471->f1;_tmp473=(void**)& _tmp471->f2;_tmp474=(void**)((void**)& _tmp471->f2);_tmp475=_tmp471->f4;}}_LL28E:
# 1251
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp475))->v,t2);{
# 1252
struct Cyc_Absyn_Kind*_tmp476=Cyc_Tcutil_typ_kind(t2);
# 1256
if(Cyc_Tcutil_kind_leq(_tmp476,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp472))->v)){
# 1257
*_tmp474=(void*)t2;
# 1258
return;}else{
# 1260
{void*_tmp477=t2;void*_tmp479;void**_tmp47A;struct Cyc_Core_Opt*_tmp47B;struct Cyc_Absyn_PtrInfo _tmp47D;_LL292: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp478=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp477;if(_tmp478->tag != 1)goto _LL294;else{_tmp479=(void**)& _tmp478->f2;_tmp47A=(void**)((void**)& _tmp478->f2);_tmp47B=_tmp478->f4;}}_LL293: {
# 1263
struct Cyc_List_List*_tmp47E=(struct Cyc_List_List*)_tmp475->v;
# 1264
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp47B))->v;for(0;s2 != 0;s2=s2->tl){
# 1265
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp47E,(struct Cyc_Absyn_Tvar*)s2->hd)){
# 1266
{const char*_tmp124D;Cyc_Tcutil_failure_reason=((_tmp124D="(type variable would escape scope)",_tag_dyneither(_tmp124D,sizeof(char),35)));}
# 1267
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1270
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp472->v,_tmp476)){
# 1271
*_tmp47A=(void*)t1;return;}
# 1273
{const char*_tmp124E;Cyc_Tcutil_failure_reason=((_tmp124E="(kinds are incompatible)",_tag_dyneither(_tmp124E,sizeof(char),25)));}
# 1274
goto _LL291;}_LL294:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp47C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp477;if(_tmp47C->tag != 5)goto _LL296;else{_tmp47D=_tmp47C->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp472->v)->kind == Cyc_Absyn_BoxKind))goto _LL296;_LL295: {
# 1279
union Cyc_Absyn_Constraint*_tmp481=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp47D.ptr_atts).bounds);
# 1280
{union Cyc_Absyn_Constraint*_tmp482=_tmp481;union Cyc_Absyn_Constraint _tmp483;int _tmp484;_LL299: _tmp483=*_tmp482;if((_tmp483.No_constr).tag != 3)goto _LL29B;_tmp484=(int)(_tmp483.No_constr).val;_LL29A:
# 1283
{struct _union_Constraint_Eq_constr*_tmp124F;(_tmp124F=& _tmp481->Eq_constr,((_tmp124F->tag=1,_tmp124F->val=Cyc_Absyn_bounds_one)));}
# 1284
*_tmp474=(void*)t2;
# 1285
return;_LL29B:;_LL29C:
# 1286
 goto _LL298;_LL298:;}
# 1288
goto _LL291;}_LL296:;_LL297:
# 1289
 goto _LL291;_LL291:;}
# 1291
{const char*_tmp1250;Cyc_Tcutil_failure_reason=((_tmp1250="(kinds are incompatible)",_tag_dyneither(_tmp1250,sizeof(char),25)));}
# 1292
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};_LL28F:;_LL290:
# 1294
 goto _LL28C;_LL28C:;}
# 1299
{struct _tuple0 _tmp1251;struct _tuple0 _tmp488=(_tmp1251.f1=t2,((_tmp1251.f2=t1,_tmp1251)));void*_tmp489;void*_tmp48B;void*_tmp48D;void*_tmp48F;struct Cyc_Absyn_Tvar*_tmp491;void*_tmp492;struct Cyc_Absyn_Tvar*_tmp494;void*_tmp495;struct Cyc_Absyn_AggrInfo _tmp497;union Cyc_Absyn_AggrInfoU _tmp498;struct Cyc_List_List*_tmp499;void*_tmp49A;struct Cyc_Absyn_AggrInfo _tmp49C;union Cyc_Absyn_AggrInfoU _tmp49D;struct Cyc_List_List*_tmp49E;void*_tmp49F;struct _tuple2*_tmp4A1;void*_tmp4A2;struct _tuple2*_tmp4A4;void*_tmp4A5;struct Cyc_List_List*_tmp4A7;void*_tmp4A8;struct Cyc_List_List*_tmp4AA;void*_tmp4AB;struct Cyc_Absyn_DatatypeInfo _tmp4AD;union Cyc_Absyn_DatatypeInfoU _tmp4AE;struct Cyc_Absyn_Datatypedecl**_tmp4AF;struct Cyc_Absyn_Datatypedecl*_tmp4B0;struct Cyc_List_List*_tmp4B1;void*_tmp4B2;struct Cyc_Absyn_DatatypeInfo _tmp4B4;union Cyc_Absyn_DatatypeInfoU _tmp4B5;struct Cyc_Absyn_Datatypedecl**_tmp4B6;struct Cyc_Absyn_Datatypedecl*_tmp4B7;struct Cyc_List_List*_tmp4B8;void*_tmp4B9;struct Cyc_Absyn_DatatypeFieldInfo _tmp4BB;union Cyc_Absyn_DatatypeFieldInfoU _tmp4BC;struct _tuple3 _tmp4BD;struct Cyc_Absyn_Datatypedecl*_tmp4BE;struct Cyc_Absyn_Datatypefield*_tmp4BF;struct Cyc_List_List*_tmp4C0;void*_tmp4C1;struct Cyc_Absyn_DatatypeFieldInfo _tmp4C3;union Cyc_Absyn_DatatypeFieldInfoU _tmp4C4;struct _tuple3 _tmp4C5;struct Cyc_Absyn_Datatypedecl*_tmp4C6;struct Cyc_Absyn_Datatypefield*_tmp4C7;struct Cyc_List_List*_tmp4C8;void*_tmp4C9;struct Cyc_Absyn_PtrInfo _tmp4CB;void*_tmp4CC;struct Cyc_Absyn_Tqual _tmp4CD;struct Cyc_Absyn_PtrAtts _tmp4CE;void*_tmp4CF;union Cyc_Absyn_Constraint*_tmp4D0;union Cyc_Absyn_Constraint*_tmp4D1;union Cyc_Absyn_Constraint*_tmp4D2;void*_tmp4D3;struct Cyc_Absyn_PtrInfo _tmp4D5;void*_tmp4D6;struct Cyc_Absyn_Tqual _tmp4D7;struct Cyc_Absyn_PtrAtts _tmp4D8;void*_tmp4D9;union Cyc_Absyn_Constraint*_tmp4DA;union Cyc_Absyn_Constraint*_tmp4DB;union Cyc_Absyn_Constraint*_tmp4DC;void*_tmp4DD;enum Cyc_Absyn_Sign _tmp4DF;enum Cyc_Absyn_Size_of _tmp4E0;void*_tmp4E1;enum Cyc_Absyn_Sign _tmp4E3;enum Cyc_Absyn_Size_of _tmp4E4;void*_tmp4E5;int _tmp4E7;void*_tmp4E8;int _tmp4EA;void*_tmp4EB;void*_tmp4ED;void*_tmp4EE;void*_tmp4F0;void*_tmp4F1;struct Cyc_Absyn_Exp*_tmp4F3;void*_tmp4F4;struct Cyc_Absyn_Exp*_tmp4F6;void*_tmp4F7;struct Cyc_Absyn_ArrayInfo _tmp4F9;void*_tmp4FA;struct Cyc_Absyn_Tqual _tmp4FB;struct Cyc_Absyn_Exp*_tmp4FC;union Cyc_Absyn_Constraint*_tmp4FD;void*_tmp4FE;struct Cyc_Absyn_ArrayInfo _tmp500;void*_tmp501;struct Cyc_Absyn_Tqual _tmp502;struct Cyc_Absyn_Exp*_tmp503;union Cyc_Absyn_Constraint*_tmp504;void*_tmp505;struct Cyc_Absyn_FnInfo _tmp507;struct Cyc_List_List*_tmp508;void*_tmp509;struct Cyc_Absyn_Tqual _tmp50A;void*_tmp50B;struct Cyc_List_List*_tmp50C;int _tmp50D;struct Cyc_Absyn_VarargInfo*_tmp50E;struct Cyc_List_List*_tmp50F;struct Cyc_List_List*_tmp510;void*_tmp511;struct Cyc_Absyn_FnInfo _tmp513;struct Cyc_List_List*_tmp514;void*_tmp515;struct Cyc_Absyn_Tqual _tmp516;void*_tmp517;struct Cyc_List_List*_tmp518;int _tmp519;struct Cyc_Absyn_VarargInfo*_tmp51A;struct Cyc_List_List*_tmp51B;struct Cyc_List_List*_tmp51C;void*_tmp51D;struct Cyc_List_List*_tmp51F;void*_tmp520;struct Cyc_List_List*_tmp522;void*_tmp523;enum Cyc_Absyn_AggrKind _tmp525;struct Cyc_List_List*_tmp526;void*_tmp527;enum Cyc_Absyn_AggrKind _tmp529;struct Cyc_List_List*_tmp52A;void*_tmp52B;struct Cyc_List_List*_tmp52D;struct Cyc_Absyn_Typedefdecl*_tmp52E;void*_tmp52F;struct Cyc_List_List*_tmp531;struct Cyc_Absyn_Typedefdecl*_tmp532;void*_tmp533;void*_tmp535;void*_tmp537;void*_tmp539;void*_tmp53B;void*_tmp53D;void*_tmp53F;void*_tmp541;void*_tmp542;void*_tmp544;void*_tmp545;void*_tmp547;void*_tmp548;void*_tmp549;void*_tmp54B;void*_tmp54C;void*_tmp54D;void*_tmp54F;void*_tmp551;void*_tmp553;void*_tmp555;void*_tmp557;_LL29E: _tmp489=_tmp488.f1;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp48A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp489;if(_tmp48A->tag != 1)goto _LL2A0;};_LL29F:
# 1302
 Cyc_Tcutil_unify_it(t2,t1);
# 1303
return;_LL2A0: _tmp48B=_tmp488.f1;{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp48C=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp48B;if(_tmp48C->tag != 0)goto _LL2A2;};_tmp48D=_tmp488.f2;{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp48E=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp48D;if(_tmp48E->tag != 0)goto _LL2A2;};_LL2A1:
# 1305
 return;_LL2A2: _tmp48F=_tmp488.f1;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp490=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp48F;if(_tmp490->tag != 2)goto _LL2A4;else{_tmp491=_tmp490->f1;}};_tmp492=_tmp488.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp493=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp492;if(_tmp493->tag != 2)goto _LL2A4;else{_tmp494=_tmp493->f1;}};_LL2A3: {
# 1308
struct _dyneither_ptr*_tmp559=_tmp491->name;
# 1309
struct _dyneither_ptr*_tmp55A=_tmp494->name;
# 1311
int _tmp55B=_tmp491->identity;
# 1312
int _tmp55C=_tmp494->identity;
# 1313
if(_tmp55C == _tmp55B)return;
# 1314
{const char*_tmp1252;Cyc_Tcutil_failure_reason=((_tmp1252="(variable types are not the same)",_tag_dyneither(_tmp1252,sizeof(char),34)));}
# 1315
goto _LL29D;}_LL2A4: _tmp495=_tmp488.f1;{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp496=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp495;if(_tmp496->tag != 11)goto _LL2A6;else{_tmp497=_tmp496->f1;_tmp498=_tmp497.aggr_info;_tmp499=_tmp497.targs;}};_tmp49A=_tmp488.f2;{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp49B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp49A;if(_tmp49B->tag != 11)goto _LL2A6;else{_tmp49C=_tmp49B->f1;_tmp49D=_tmp49C.aggr_info;_tmp49E=_tmp49C.targs;}};_LL2A5: {
# 1318
enum Cyc_Absyn_AggrKind _tmp55F;struct _tuple2*_tmp560;struct _tuple13 _tmp55E=Cyc_Absyn_aggr_kinded_name(_tmp49D);_tmp55F=_tmp55E.f1;_tmp560=_tmp55E.f2;{
# 1319
enum Cyc_Absyn_AggrKind _tmp562;struct _tuple2*_tmp563;struct _tuple13 _tmp561=Cyc_Absyn_aggr_kinded_name(_tmp498);_tmp562=_tmp561.f1;_tmp563=_tmp561.f2;
# 1320
if(_tmp55F != _tmp562){{const char*_tmp1253;Cyc_Tcutil_failure_reason=((_tmp1253="(struct and union type)",_tag_dyneither(_tmp1253,sizeof(char),24)));}goto _LL29D;}
# 1321
if(Cyc_Absyn_qvar_cmp(_tmp560,_tmp563)!= 0){{const char*_tmp1254;Cyc_Tcutil_failure_reason=((_tmp1254="(different type name)",_tag_dyneither(_tmp1254,sizeof(char),22)));}goto _LL29D;}
# 1322
Cyc_Tcutil_unify_list(_tmp49E,_tmp499);
# 1323
return;};}_LL2A6: _tmp49F=_tmp488.f1;{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp4A0=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp49F;if(_tmp4A0->tag != 13)goto _LL2A8;else{_tmp4A1=_tmp4A0->f1;}};_tmp4A2=_tmp488.f2;{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp4A3=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4A2;if(_tmp4A3->tag != 13)goto _LL2A8;else{_tmp4A4=_tmp4A3->f1;}};_LL2A7:
# 1327
 if(Cyc_Absyn_qvar_cmp(_tmp4A1,_tmp4A4)== 0)return;
# 1328
{const char*_tmp1255;Cyc_Tcutil_failure_reason=((_tmp1255="(different enum types)",_tag_dyneither(_tmp1255,sizeof(char),23)));}
# 1329
goto _LL29D;_LL2A8: _tmp4A5=_tmp488.f1;{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp4A6=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4A5;if(_tmp4A6->tag != 14)goto _LL2AA;else{_tmp4A7=_tmp4A6->f1;}};_tmp4A8=_tmp488.f2;{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp4A9=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4A8;if(_tmp4A9->tag != 14)goto _LL2AA;else{_tmp4AA=_tmp4A9->f1;}};_LL2A9: {
# 1332
int bad=0;
# 1333
for(0;_tmp4A7 != 0  && _tmp4AA != 0;(_tmp4A7=_tmp4A7->tl,_tmp4AA=_tmp4AA->tl)){
# 1334
struct Cyc_Absyn_Enumfield*_tmp567=(struct Cyc_Absyn_Enumfield*)_tmp4A7->hd;
# 1335
struct Cyc_Absyn_Enumfield*_tmp568=(struct Cyc_Absyn_Enumfield*)_tmp4AA->hd;
# 1336
if(Cyc_Absyn_qvar_cmp(_tmp567->name,_tmp568->name)!= 0){
# 1337
{const char*_tmp1256;Cyc_Tcutil_failure_reason=((_tmp1256="(different names for enum fields)",_tag_dyneither(_tmp1256,sizeof(char),34)));}
# 1338
bad=1;
# 1339
break;}
# 1341
if(_tmp567->tag == _tmp568->tag)continue;
# 1342
if(_tmp567->tag == 0  || _tmp568->tag == 0){
# 1343
{const char*_tmp1257;Cyc_Tcutil_failure_reason=((_tmp1257="(different tag values for enum fields)",_tag_dyneither(_tmp1257,sizeof(char),39)));}
# 1344
bad=1;
# 1345
break;}
# 1347
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp567->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp568->tag))){
# 1348
{const char*_tmp1258;Cyc_Tcutil_failure_reason=((_tmp1258="(different tag values for enum fields)",_tag_dyneither(_tmp1258,sizeof(char),39)));}
# 1349
bad=1;
# 1350
break;}}
# 1353
if(bad)goto _LL29D;
# 1354
if(_tmp4A7 == 0  && _tmp4AA == 0)return;
# 1355
{const char*_tmp1259;Cyc_Tcutil_failure_reason=((_tmp1259="(different number of fields for enums)",_tag_dyneither(_tmp1259,sizeof(char),39)));}
# 1356
goto _LL29D;}_LL2AA: _tmp4AB=_tmp488.f1;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4AC=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4AB;if(_tmp4AC->tag != 3)goto _LL2AC;else{_tmp4AD=_tmp4AC->f1;_tmp4AE=_tmp4AD.datatype_info;if((_tmp4AE.KnownDatatype).tag != 2)goto _LL2AC;_tmp4AF=(struct Cyc_Absyn_Datatypedecl**)(_tmp4AE.KnownDatatype).val;_tmp4B0=*_tmp4AF;_tmp4B1=_tmp4AD.targs;}};_tmp4B2=_tmp488.f2;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4B3=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4B2;if(_tmp4B3->tag != 3)goto _LL2AC;else{_tmp4B4=_tmp4B3->f1;_tmp4B5=_tmp4B4.datatype_info;if((_tmp4B5.KnownDatatype).tag != 2)goto _LL2AC;_tmp4B6=(struct Cyc_Absyn_Datatypedecl**)(_tmp4B5.KnownDatatype).val;_tmp4B7=*_tmp4B6;_tmp4B8=_tmp4B4.targs;}};_LL2AB:
# 1360
 if(_tmp4B0 == _tmp4B7  || Cyc_Absyn_qvar_cmp(_tmp4B0->name,_tmp4B7->name)== 0){
# 1361
Cyc_Tcutil_unify_list(_tmp4B8,_tmp4B1);
# 1362
return;}
# 1364
goto _LL29D;_LL2AC: _tmp4B9=_tmp488.f1;{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4BA=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4B9;if(_tmp4BA->tag != 4)goto _LL2AE;else{_tmp4BB=_tmp4BA->f1;_tmp4BC=_tmp4BB.field_info;if((_tmp4BC.KnownDatatypefield).tag != 2)goto _LL2AE;_tmp4BD=(struct _tuple3)(_tmp4BC.KnownDatatypefield).val;_tmp4BE=_tmp4BD.f1;_tmp4BF=_tmp4BD.f2;_tmp4C0=_tmp4BB.targs;}};_tmp4C1=_tmp488.f2;{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4C2=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4C1;if(_tmp4C2->tag != 4)goto _LL2AE;else{_tmp4C3=_tmp4C2->f1;_tmp4C4=_tmp4C3.field_info;if((_tmp4C4.KnownDatatypefield).tag != 2)goto _LL2AE;_tmp4C5=(struct _tuple3)(_tmp4C4.KnownDatatypefield).val;_tmp4C6=_tmp4C5.f1;_tmp4C7=_tmp4C5.f2;_tmp4C8=_tmp4C3.targs;}};_LL2AD:
# 1368
 if((_tmp4BE == _tmp4C6  || Cyc_Absyn_qvar_cmp(_tmp4BE->name,_tmp4C6->name)== 0) && (
# 1369
_tmp4BF == _tmp4C7  || Cyc_Absyn_qvar_cmp(_tmp4BF->name,_tmp4C7->name)== 0)){
# 1370
Cyc_Tcutil_unify_list(_tmp4C8,_tmp4C0);
# 1371
return;}
# 1373
{const char*_tmp125A;Cyc_Tcutil_failure_reason=((_tmp125A="(different datatype field types)",_tag_dyneither(_tmp125A,sizeof(char),33)));}
# 1374
goto _LL29D;_LL2AE: _tmp4C9=_tmp488.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4CA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C9;if(_tmp4CA->tag != 5)goto _LL2B0;else{_tmp4CB=_tmp4CA->f1;_tmp4CC=_tmp4CB.elt_typ;_tmp4CD=_tmp4CB.elt_tq;_tmp4CE=_tmp4CB.ptr_atts;_tmp4CF=_tmp4CE.rgn;_tmp4D0=_tmp4CE.nullable;_tmp4D1=_tmp4CE.bounds;_tmp4D2=_tmp4CE.zero_term;}};_tmp4D3=_tmp488.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4D4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D3;if(_tmp4D4->tag != 5)goto _LL2B0;else{_tmp4D5=_tmp4D4->f1;_tmp4D6=_tmp4D5.elt_typ;_tmp4D7=_tmp4D5.elt_tq;_tmp4D8=_tmp4D5.ptr_atts;_tmp4D9=_tmp4D8.rgn;_tmp4DA=_tmp4D8.nullable;_tmp4DB=_tmp4D8.bounds;_tmp4DC=_tmp4D8.zero_term;}};_LL2AF:
# 1378
 Cyc_Tcutil_unify_it(_tmp4D6,_tmp4CC);
# 1379
Cyc_Tcutil_unify_it(_tmp4CF,_tmp4D9);
# 1380
Cyc_Tcutil_t1_failure=t1;
# 1381
Cyc_Tcutil_t2_failure=t2;
# 1382
{const char*_tmp125B;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp4DC,_tmp4D2,((_tmp125B="(not both zero terminated)",_tag_dyneither(_tmp125B,sizeof(char),27))));}
# 1383
Cyc_Tcutil_unify_tqual(_tmp4D7,_tmp4D6,_tmp4CD,_tmp4CC);
# 1384
{const char*_tmp125C;((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp4DB,_tmp4D1,((_tmp125C="(different pointer bounds)",_tag_dyneither(_tmp125C,sizeof(char),27))));}
# 1386
{void*_tmp570=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4DB);_LL2DB: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp571=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp570;if(_tmp571->tag != 0)goto _LL2DD;}_LL2DC:
# 1387
 return;_LL2DD:;_LL2DE:
# 1388
 goto _LL2DA;_LL2DA:;}
# 1390
{const char*_tmp125D;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp4DA,_tmp4D0,((_tmp125D="(incompatible pointer types)",_tag_dyneither(_tmp125D,sizeof(char),29))));}
# 1391
return;_LL2B0: _tmp4DD=_tmp488.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4DE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4DD;if(_tmp4DE->tag != 6)goto _LL2B2;else{_tmp4DF=_tmp4DE->f1;_tmp4E0=_tmp4DE->f2;}};_tmp4E1=_tmp488.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4E2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4E1;if(_tmp4E2->tag != 6)goto _LL2B2;else{_tmp4E3=_tmp4E2->f1;_tmp4E4=_tmp4E2->f2;}};_LL2B1:
# 1395
 if(_tmp4E3 == _tmp4DF  && ((_tmp4E4 == _tmp4E0  || _tmp4E4 == Cyc_Absyn_Int_sz  && _tmp4E0 == Cyc_Absyn_Long_sz) || 
# 1396
_tmp4E4 == Cyc_Absyn_Long_sz  && _tmp4E0 == Cyc_Absyn_Int_sz))return;
# 1397
{const char*_tmp125E;Cyc_Tcutil_failure_reason=((_tmp125E="(different integral types)",_tag_dyneither(_tmp125E,sizeof(char),27)));}
# 1398
goto _LL29D;_LL2B2: _tmp4E5=_tmp488.f1;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4E6=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4E5;if(_tmp4E6->tag != 7)goto _LL2B4;else{_tmp4E7=_tmp4E6->f1;}};_tmp4E8=_tmp488.f2;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4E9=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4E8;if(_tmp4E9->tag != 7)goto _LL2B4;else{_tmp4EA=_tmp4E9->f1;}};_LL2B3:
# 1401
 if(_tmp4EA == 0  && _tmp4E7 == 0)return;else{
# 1402
if(_tmp4EA == 1  && _tmp4E7 == 1)return;else{
# 1404
if(((_tmp4EA != 0  && _tmp4EA != 1) && _tmp4E7 != 0) && _tmp4E7 != 1)return;}}
# 1405
goto _LL29D;_LL2B4: _tmp4EB=_tmp488.f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4EC=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4EB;if(_tmp4EC->tag != 19)goto _LL2B6;else{_tmp4ED=(void*)_tmp4EC->f1;}};_tmp4EE=_tmp488.f2;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4EF=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4EE;if(_tmp4EF->tag != 19)goto _LL2B6;else{_tmp4F0=(void*)_tmp4EF->f1;}};_LL2B5:
# 1407
 Cyc_Tcutil_unify_it(_tmp4ED,_tmp4F0);return;_LL2B6: _tmp4F1=_tmp488.f1;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4F2=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4F1;if(_tmp4F2->tag != 18)goto _LL2B8;else{_tmp4F3=_tmp4F2->f1;}};_tmp4F4=_tmp488.f2;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4F5=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4F4;if(_tmp4F5->tag != 18)goto _LL2B8;else{_tmp4F6=_tmp4F5->f1;}};_LL2B7:
# 1410
 if(!Cyc_Evexp_same_const_exp(_tmp4F3,_tmp4F6)){
# 1411
{const char*_tmp125F;Cyc_Tcutil_failure_reason=((_tmp125F="(cannot prove expressions are the same)",_tag_dyneither(_tmp125F,sizeof(char),40)));}
# 1412
goto _LL29D;}
# 1414
return;_LL2B8: _tmp4F7=_tmp488.f1;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4F8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4F7;if(_tmp4F8->tag != 8)goto _LL2BA;else{_tmp4F9=_tmp4F8->f1;_tmp4FA=_tmp4F9.elt_type;_tmp4FB=_tmp4F9.tq;_tmp4FC=_tmp4F9.num_elts;_tmp4FD=_tmp4F9.zero_term;}};_tmp4FE=_tmp488.f2;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4FF=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FE;if(_tmp4FF->tag != 8)goto _LL2BA;else{_tmp500=_tmp4FF->f1;_tmp501=_tmp500.elt_type;_tmp502=_tmp500.tq;_tmp503=_tmp500.num_elts;_tmp504=_tmp500.zero_term;}};_LL2B9:
# 1418
 Cyc_Tcutil_unify_it(_tmp501,_tmp4FA);
# 1419
Cyc_Tcutil_unify_tqual(_tmp502,_tmp501,_tmp4FB,_tmp4FA);
# 1420
Cyc_Tcutil_t1_failure=t1;
# 1421
Cyc_Tcutil_t2_failure=t2;
# 1422
{const char*_tmp1260;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp4FD,_tmp504,((_tmp1260="(not both zero terminated)",_tag_dyneither(_tmp1260,sizeof(char),27))));}
# 1423
if(_tmp4FC == _tmp503)return;
# 1424
if(_tmp4FC == 0  || _tmp503 == 0)goto _LL29D;
# 1425
if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp4FC,(struct Cyc_Absyn_Exp*)_tmp503))
# 1426
return;
# 1427
{const char*_tmp1261;Cyc_Tcutil_failure_reason=((_tmp1261="(different array sizes)",_tag_dyneither(_tmp1261,sizeof(char),24)));}
# 1428
goto _LL29D;_LL2BA: _tmp505=_tmp488.f1;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp506=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp505;if(_tmp506->tag != 9)goto _LL2BC;else{_tmp507=_tmp506->f1;_tmp508=_tmp507.tvars;_tmp509=_tmp507.effect;_tmp50A=_tmp507.ret_tqual;_tmp50B=_tmp507.ret_typ;_tmp50C=_tmp507.args;_tmp50D=_tmp507.c_varargs;_tmp50E=_tmp507.cyc_varargs;_tmp50F=_tmp507.rgn_po;_tmp510=_tmp507.attributes;}};_tmp511=_tmp488.f2;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp512=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp511;if(_tmp512->tag != 9)goto _LL2BC;else{_tmp513=_tmp512->f1;_tmp514=_tmp513.tvars;_tmp515=_tmp513.effect;_tmp516=_tmp513.ret_tqual;_tmp517=_tmp513.ret_typ;_tmp518=_tmp513.args;_tmp519=_tmp513.c_varargs;_tmp51A=_tmp513.cyc_varargs;_tmp51B=_tmp513.rgn_po;_tmp51C=_tmp513.attributes;}};_LL2BB: {
# 1432
int done=0;
# 1433
struct _RegionHandle _tmp577=_new_region("rgn");struct _RegionHandle*rgn=& _tmp577;_push_region(rgn);{
# 1434
struct Cyc_List_List*inst=0;
# 1435
while(_tmp514 != 0){
# 1436
if(_tmp508 == 0){
# 1437
{const char*_tmp1262;Cyc_Tcutil_failure_reason=((_tmp1262="(second function type has too few type variables)",_tag_dyneither(_tmp1262,sizeof(char),50)));}
# 1438
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1440
void*_tmp579=((struct Cyc_Absyn_Tvar*)_tmp514->hd)->kind;
# 1441
void*_tmp57A=((struct Cyc_Absyn_Tvar*)_tmp508->hd)->kind;
# 1442
if(!Cyc_Tcutil_unify_kindbound(_tmp579,_tmp57A)){
# 1443
{const char*_tmp1268;void*_tmp1267[3];struct Cyc_String_pa_PrintArg_struct _tmp1266;struct Cyc_String_pa_PrintArg_struct _tmp1265;struct Cyc_String_pa_PrintArg_struct _tmp1264;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)((_tmp1264.tag=0,((_tmp1264.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1446
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp508->hd,& Cyc_Tcutil_bk))),((_tmp1265.tag=0,((_tmp1265.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1445
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp514->hd,& Cyc_Tcutil_bk))),((_tmp1266.tag=0,((_tmp1266.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1444
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp514->hd)),((_tmp1267[0]=& _tmp1266,((_tmp1267[1]=& _tmp1265,((_tmp1267[2]=& _tmp1264,Cyc_aprintf(((_tmp1268="(type var %s has different kinds %s and %s)",_tag_dyneither(_tmp1268,sizeof(char),44))),_tag_dyneither(_tmp1267,sizeof(void*),3))))))))))))))))))));}
# 1447
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1449
{struct _tuple16*_tmp1272;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1271;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1270;struct Cyc_List_List*_tmp126F;inst=((_tmp126F=_region_malloc(rgn,sizeof(*_tmp126F)),((_tmp126F->hd=((_tmp1272=_region_malloc(rgn,sizeof(*_tmp1272)),((_tmp1272->f1=(struct Cyc_Absyn_Tvar*)_tmp508->hd,((_tmp1272->f2=(void*)((_tmp1270=_cycalloc(sizeof(*_tmp1270)),((_tmp1270[0]=((_tmp1271.tag=2,((_tmp1271.f1=(struct Cyc_Absyn_Tvar*)_tmp514->hd,_tmp1271)))),_tmp1270)))),_tmp1272)))))),((_tmp126F->tl=inst,_tmp126F))))));}
# 1450
_tmp514=_tmp514->tl;
# 1451
_tmp508=_tmp508->tl;};}
# 1453
if(_tmp508 != 0){
# 1454
{const char*_tmp1273;Cyc_Tcutil_failure_reason=((_tmp1273="(second function type has too many type variables)",_tag_dyneither(_tmp1273,sizeof(char),51)));}
# 1455
_npop_handler(0);goto _LL29D;}
# 1457
if(inst != 0){
# 1458
{struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp127F;struct Cyc_Absyn_FnInfo _tmp127E;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp127D;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1279;struct Cyc_Absyn_FnInfo _tmp1278;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1277;Cyc_Tcutil_unify_it((void*)((_tmp1277=_cycalloc(sizeof(*_tmp1277)),((_tmp1277[0]=((_tmp1279.tag=9,((_tmp1279.f1=((_tmp1278.tvars=0,((_tmp1278.effect=_tmp515,((_tmp1278.ret_tqual=_tmp516,((_tmp1278.ret_typ=_tmp517,((_tmp1278.args=_tmp518,((_tmp1278.c_varargs=_tmp519,((_tmp1278.cyc_varargs=_tmp51A,((_tmp1278.rgn_po=_tmp51B,((_tmp1278.attributes=_tmp51C,_tmp1278)))))))))))))))))),_tmp1279)))),_tmp1277)))),
# 1460
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
# 1461
(_tmp127D=_cycalloc(sizeof(*_tmp127D)),((_tmp127D[0]=((_tmp127F.tag=9,((_tmp127F.f1=((_tmp127E.tvars=0,((_tmp127E.effect=_tmp509,((_tmp127E.ret_tqual=_tmp50A,((_tmp127E.ret_typ=_tmp50B,((_tmp127E.args=_tmp50C,((_tmp127E.c_varargs=_tmp50D,((_tmp127E.cyc_varargs=_tmp50E,((_tmp127E.rgn_po=_tmp50F,((_tmp127E.attributes=_tmp510,_tmp127E)))))))))))))))))),_tmp127F)))),_tmp127D))))));}
# 1463
done=1;}}
# 1466
if(done){
# 1467
_npop_handler(0);return;}
# 1468
Cyc_Tcutil_unify_it(_tmp517,_tmp50B);
# 1469
Cyc_Tcutil_unify_tqual(_tmp516,_tmp517,_tmp50A,_tmp50B);
# 1470
for(0;_tmp518 != 0  && _tmp50C != 0;(_tmp518=_tmp518->tl,_tmp50C=_tmp50C->tl)){
# 1471
struct Cyc_Absyn_Tqual _tmp58C;void*_tmp58D;struct _tuple10 _tmp58B=*((struct _tuple10*)_tmp518->hd);_tmp58C=_tmp58B.f2;_tmp58D=_tmp58B.f3;{
# 1472
struct Cyc_Absyn_Tqual _tmp58F;void*_tmp590;struct _tuple10 _tmp58E=*((struct _tuple10*)_tmp50C->hd);_tmp58F=_tmp58E.f2;_tmp590=_tmp58E.f3;
# 1473
Cyc_Tcutil_unify_it(_tmp58D,_tmp590);
# 1474
Cyc_Tcutil_unify_tqual(_tmp58C,_tmp58D,_tmp58F,_tmp590);};}
# 1476
Cyc_Tcutil_t1_failure=t1;
# 1477
Cyc_Tcutil_t2_failure=t2;
# 1478
if(_tmp518 != 0  || _tmp50C != 0){
# 1479
{const char*_tmp1280;Cyc_Tcutil_failure_reason=((_tmp1280="(function types have different number of arguments)",_tag_dyneither(_tmp1280,sizeof(char),52)));}
# 1480
_npop_handler(0);goto _LL29D;}
# 1482
if(_tmp519 != _tmp50D){
# 1483
{const char*_tmp1281;Cyc_Tcutil_failure_reason=((_tmp1281="(only one function type takes C varargs)",_tag_dyneither(_tmp1281,sizeof(char),41)));}
# 1484
_npop_handler(0);goto _LL29D;}{
# 1487
int bad_cyc_vararg=0;
# 1488
{struct _tuple22 _tmp1282;struct _tuple22 _tmp594=(_tmp1282.f1=_tmp51A,((_tmp1282.f2=_tmp50E,_tmp1282)));struct Cyc_Absyn_VarargInfo*_tmp595;struct Cyc_Absyn_VarargInfo*_tmp596;struct Cyc_Absyn_VarargInfo*_tmp597;struct Cyc_Absyn_VarargInfo*_tmp598;struct Cyc_Absyn_VarargInfo*_tmp599;struct Cyc_Absyn_VarargInfo _tmp59A;struct _dyneither_ptr*_tmp59B;struct Cyc_Absyn_Tqual _tmp59C;void*_tmp59D;int _tmp59E;struct Cyc_Absyn_VarargInfo*_tmp59F;struct Cyc_Absyn_VarargInfo _tmp5A0;struct _dyneither_ptr*_tmp5A1;struct Cyc_Absyn_Tqual _tmp5A2;void*_tmp5A3;int _tmp5A4;_LL2E0: _tmp595=_tmp594.f1;if(_tmp595 != 0)goto _LL2E2;_tmp596=_tmp594.f2;if(_tmp596 != 0)goto _LL2E2;_LL2E1:
# 1489
 goto _LL2DF;_LL2E2: _tmp597=_tmp594.f1;if(_tmp597 != 0)goto _LL2E4;_LL2E3:
# 1490
 goto _LL2E5;_LL2E4: _tmp598=_tmp594.f2;if(_tmp598 != 0)goto _LL2E6;_LL2E5:
# 1492
 bad_cyc_vararg=1;
# 1493
{const char*_tmp1283;Cyc_Tcutil_failure_reason=((_tmp1283="(only one function type takes varargs)",_tag_dyneither(_tmp1283,sizeof(char),39)));}
# 1494
goto _LL2DF;_LL2E6: _tmp599=_tmp594.f1;if(_tmp599 == 0)goto _LL2DF;_tmp59A=*_tmp599;_tmp59B=_tmp59A.name;_tmp59C=_tmp59A.tq;_tmp59D=_tmp59A.type;_tmp59E=_tmp59A.inject;_tmp59F=_tmp594.f2;if(_tmp59F == 0)goto _LL2DF;_tmp5A0=*_tmp59F;_tmp5A1=_tmp5A0.name;_tmp5A2=_tmp5A0.tq;_tmp5A3=_tmp5A0.type;_tmp5A4=_tmp5A0.inject;_LL2E7:
# 1496
 Cyc_Tcutil_unify_it(_tmp59D,_tmp5A3);
# 1497
Cyc_Tcutil_unify_tqual(_tmp59C,_tmp59D,_tmp5A2,_tmp5A3);
# 1498
if(_tmp59E != _tmp5A4){
# 1499
bad_cyc_vararg=1;{
# 1500
const char*_tmp1284;Cyc_Tcutil_failure_reason=((_tmp1284="(only one function type injects varargs)",_tag_dyneither(_tmp1284,sizeof(char),41)));};}
# 1502
goto _LL2DF;_LL2DF:;}
# 1504
if(bad_cyc_vararg){_npop_handler(0);goto _LL29D;}{
# 1507
int bad_effect=0;
# 1508
{struct _tuple0 _tmp1285;struct _tuple0 _tmp5A8=(_tmp1285.f1=_tmp515,((_tmp1285.f2=_tmp509,_tmp1285)));void*_tmp5A9;void*_tmp5AA;void*_tmp5AB;void*_tmp5AC;_LL2E9: _tmp5A9=_tmp5A8.f1;if(_tmp5A9 != 0)goto _LL2EB;_tmp5AA=_tmp5A8.f2;if(_tmp5AA != 0)goto _LL2EB;_LL2EA:
# 1509
 goto _LL2E8;_LL2EB: _tmp5AB=_tmp5A8.f1;if(_tmp5AB != 0)goto _LL2ED;_LL2EC:
# 1510
 goto _LL2EE;_LL2ED: _tmp5AC=_tmp5A8.f2;if(_tmp5AC != 0)goto _LL2EF;_LL2EE:
# 1511
 bad_effect=1;goto _LL2E8;_LL2EF:;_LL2F0:
# 1512
 bad_effect=!Cyc_Tcutil_unify_effect((void*)_check_null(_tmp515),(void*)_check_null(_tmp509));goto _LL2E8;_LL2E8:;}
# 1514
Cyc_Tcutil_t1_failure=t1;
# 1515
Cyc_Tcutil_t2_failure=t2;
# 1516
if(bad_effect){
# 1517
{const char*_tmp1286;Cyc_Tcutil_failure_reason=((_tmp1286="(function type effects do not unify)",_tag_dyneither(_tmp1286,sizeof(char),37)));}
# 1518
_npop_handler(0);goto _LL29D;}
# 1520
if(!Cyc_Tcutil_same_atts(_tmp510,_tmp51C)){
# 1521
{const char*_tmp1287;Cyc_Tcutil_failure_reason=((_tmp1287="(function types have different attributes)",_tag_dyneither(_tmp1287,sizeof(char),43)));}
# 1522
_npop_handler(0);goto _LL29D;}
# 1524
if(!Cyc_Tcutil_same_rgn_po(_tmp50F,_tmp51B)){
# 1525
{const char*_tmp1288;Cyc_Tcutil_failure_reason=((_tmp1288="(function types have different region lifetime orderings)",_tag_dyneither(_tmp1288,sizeof(char),58)));}
# 1526
_npop_handler(0);goto _LL29D;}
# 1528
_npop_handler(0);return;};};
# 1433
;_pop_region(rgn);}_LL2BC: _tmp51D=_tmp488.f1;{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp51E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp51D;if(_tmp51E->tag != 10)goto _LL2BE;else{_tmp51F=_tmp51E->f1;}};_tmp520=_tmp488.f2;{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp521=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp520;if(_tmp521->tag != 10)goto _LL2BE;else{_tmp522=_tmp521->f1;}};_LL2BD:
# 1531
 for(0;_tmp522 != 0  && _tmp51F != 0;(_tmp522=_tmp522->tl,_tmp51F=_tmp51F->tl)){
# 1532
struct Cyc_Absyn_Tqual _tmp5B1;void*_tmp5B2;struct _tuple12 _tmp5B0=*((struct _tuple12*)_tmp522->hd);_tmp5B1=_tmp5B0.f1;_tmp5B2=_tmp5B0.f2;{
# 1533
struct Cyc_Absyn_Tqual _tmp5B4;void*_tmp5B5;struct _tuple12 _tmp5B3=*((struct _tuple12*)_tmp51F->hd);_tmp5B4=_tmp5B3.f1;_tmp5B5=_tmp5B3.f2;
# 1534
Cyc_Tcutil_unify_it(_tmp5B2,_tmp5B5);
# 1535
Cyc_Tcutil_unify_tqual(_tmp5B1,_tmp5B2,_tmp5B4,_tmp5B5);};}
# 1537
if(_tmp522 == 0  && _tmp51F == 0)return;
# 1538
Cyc_Tcutil_t1_failure=t1;
# 1539
Cyc_Tcutil_t2_failure=t2;
# 1540
{const char*_tmp1289;Cyc_Tcutil_failure_reason=((_tmp1289="(tuple types have different numbers of components)",_tag_dyneither(_tmp1289,sizeof(char),51)));}
# 1541
goto _LL29D;_LL2BE: _tmp523=_tmp488.f1;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp524=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp523;if(_tmp524->tag != 12)goto _LL2C0;else{_tmp525=_tmp524->f1;_tmp526=_tmp524->f2;}};_tmp527=_tmp488.f2;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp528=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp527;if(_tmp528->tag != 12)goto _LL2C0;else{_tmp529=_tmp528->f1;_tmp52A=_tmp528->f2;}};_LL2BF:
# 1544
 if(_tmp529 != _tmp525){{const char*_tmp128A;Cyc_Tcutil_failure_reason=((_tmp128A="(struct and union type)",_tag_dyneither(_tmp128A,sizeof(char),24)));}goto _LL29D;}
# 1545
for(0;_tmp52A != 0  && _tmp526 != 0;(_tmp52A=_tmp52A->tl,_tmp526=_tmp526->tl)){
# 1546
struct Cyc_Absyn_Aggrfield*_tmp5B8=(struct Cyc_Absyn_Aggrfield*)_tmp52A->hd;
# 1547
struct Cyc_Absyn_Aggrfield*_tmp5B9=(struct Cyc_Absyn_Aggrfield*)_tmp526->hd;
# 1548
if(Cyc_strptrcmp(_tmp5B8->name,_tmp5B9->name)!= 0){
# 1549
{const char*_tmp128B;Cyc_Tcutil_failure_reason=((_tmp128B="(different member names)",_tag_dyneither(_tmp128B,sizeof(char),25)));}
# 1550
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1552
Cyc_Tcutil_unify_it(_tmp5B8->type,_tmp5B9->type);
# 1553
Cyc_Tcutil_unify_tqual(_tmp5B8->tq,_tmp5B8->type,_tmp5B9->tq,_tmp5B9->type);
# 1554
if(!Cyc_Tcutil_same_atts(_tmp5B8->attributes,_tmp5B9->attributes)){
# 1555
Cyc_Tcutil_t1_failure=t1;
# 1556
Cyc_Tcutil_t2_failure=t2;
# 1557
{const char*_tmp128C;Cyc_Tcutil_failure_reason=((_tmp128C="(different attributes on member)",_tag_dyneither(_tmp128C,sizeof(char),33)));}
# 1558
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1560
if((_tmp5B8->width != 0  && _tmp5B9->width == 0  || 
# 1561
_tmp5B9->width != 0  && _tmp5B8->width == 0) || 
# 1562
(_tmp5B8->width != 0  && _tmp5B9->width != 0) && !
# 1563
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp5B8->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp5B9->width))){
# 1564
Cyc_Tcutil_t1_failure=t1;
# 1565
Cyc_Tcutil_t2_failure=t2;
# 1566
{const char*_tmp128D;Cyc_Tcutil_failure_reason=((_tmp128D="(different bitfield widths on member)",_tag_dyneither(_tmp128D,sizeof(char),38)));}
# 1567
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1570
if(_tmp52A == 0  && _tmp526 == 0)return;
# 1571
Cyc_Tcutil_t1_failure=t1;
# 1572
Cyc_Tcutil_t2_failure=t2;
# 1573
{const char*_tmp128E;Cyc_Tcutil_failure_reason=((_tmp128E="(different number of members)",_tag_dyneither(_tmp128E,sizeof(char),30)));}
# 1574
goto _LL29D;_LL2C0: _tmp52B=_tmp488.f1;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp52C=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp52B;if(_tmp52C->tag != 17)goto _LL2C2;else{_tmp52D=_tmp52C->f2;_tmp52E=_tmp52C->f3;}};_tmp52F=_tmp488.f2;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp530=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp52F;if(_tmp530->tag != 17)goto _LL2C2;else{_tmp531=_tmp530->f2;_tmp532=_tmp530->f3;}};_LL2C1:
# 1576
 if(_tmp52E != _tmp532){
# 1577
{const char*_tmp128F;Cyc_Tcutil_failure_reason=((_tmp128F="(different abstract typedefs)",_tag_dyneither(_tmp128F,sizeof(char),30)));}
# 1578
goto _LL29D;}
# 1580
{const char*_tmp1290;Cyc_Tcutil_failure_reason=((_tmp1290="(type parameters to typedef differ)",_tag_dyneither(_tmp1290,sizeof(char),36)));}
# 1581
Cyc_Tcutil_unify_list(_tmp52D,_tmp531);
# 1582
return;_LL2C2: _tmp533=_tmp488.f1;{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp534=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp533;if(_tmp534->tag != 20)goto _LL2C4;};_tmp535=_tmp488.f2;{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp536=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp535;if(_tmp536->tag != 20)goto _LL2C4;};_LL2C3:
# 1583
 return;_LL2C4: _tmp537=_tmp488.f1;{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp538=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp537;if(_tmp538->tag != 21)goto _LL2C6;};_tmp539=_tmp488.f2;{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp53A=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp539;if(_tmp53A->tag != 21)goto _LL2C6;};_LL2C5:
# 1584
 return;_LL2C6: _tmp53B=_tmp488.f1;{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp53C=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp53B;if(_tmp53C->tag != 22)goto _LL2C8;};_tmp53D=_tmp488.f2;{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp53E=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp53D;if(_tmp53E->tag != 22)goto _LL2C8;};_LL2C7:
# 1585
 return;_LL2C8: _tmp53F=_tmp488.f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp540=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp53F;if(_tmp540->tag != 15)goto _LL2CA;else{_tmp541=(void*)_tmp540->f1;}};_tmp542=_tmp488.f2;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp543=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp542;if(_tmp543->tag != 15)goto _LL2CA;else{_tmp544=(void*)_tmp543->f1;}};_LL2C9:
# 1587
 Cyc_Tcutil_unify_it(_tmp541,_tmp544);
# 1588
return;_LL2CA: _tmp545=_tmp488.f1;{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp546=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp545;if(_tmp546->tag != 16)goto _LL2CC;else{_tmp547=(void*)_tmp546->f1;_tmp548=(void*)_tmp546->f2;}};_tmp549=_tmp488.f2;{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp54A=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp549;if(_tmp54A->tag != 16)goto _LL2CC;else{_tmp54B=(void*)_tmp54A->f1;_tmp54C=(void*)_tmp54A->f2;}};_LL2CB:
# 1590
 Cyc_Tcutil_unify_it(_tmp547,_tmp54B);
# 1591
Cyc_Tcutil_unify_it(_tmp548,_tmp54C);
# 1592
return;_LL2CC: _tmp54D=_tmp488.f1;{struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp54E=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp54D;if(_tmp54E->tag != 24)goto _LL2CE;};_LL2CD:
# 1593
 goto _LL2CF;_LL2CE: _tmp54F=_tmp488.f2;{struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp550=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp54F;if(_tmp550->tag != 24)goto _LL2D0;};_LL2CF:
# 1594
 goto _LL2D1;_LL2D0: _tmp551=_tmp488.f1;{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp552=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp551;if(_tmp552->tag != 23)goto _LL2D2;};_LL2D1:
# 1595
 goto _LL2D3;_LL2D2: _tmp553=_tmp488.f1;{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp554=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp553;if(_tmp554->tag != 25)goto _LL2D4;};_LL2D3:
# 1596
 goto _LL2D5;_LL2D4: _tmp555=_tmp488.f2;{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp556=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp555;if(_tmp556->tag != 25)goto _LL2D6;};_LL2D5:
# 1597
 goto _LL2D7;_LL2D6: _tmp557=_tmp488.f2;{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp558=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp557;if(_tmp558->tag != 23)goto _LL2D8;};_LL2D7:
# 1599
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
# 1600
{const char*_tmp1291;Cyc_Tcutil_failure_reason=((_tmp1291="(effects don't unify)",_tag_dyneither(_tmp1291,sizeof(char),22)));}
# 1601
goto _LL29D;_LL2D8:;_LL2D9:
# 1602
 goto _LL29D;_LL29D:;}
# 1604
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1607
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
# 1608
if(a1 == a2)return 0;
# 1609
if(a1 == 0  && a2 != 0)return - 1;
# 1610
if(a1 != 0  && a2 == 0)return 1;
# 1611
return cmp((void*)_check_null(a1),(void*)_check_null(a2));}
# 1614
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
# 1615
int _tmp5C1=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
# 1616
int _tmp5C2=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
# 1617
return Cyc_Core_intcmp(_tmp5C1,_tmp5C2);}
# 1621
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
# 1622
x=Cyc_Absyn_compress_conref(x);
# 1623
y=Cyc_Absyn_compress_conref(y);
# 1624
if(x == y)return 0;{
# 1625
union Cyc_Absyn_Constraint*_tmp5C3=x;union Cyc_Absyn_Constraint _tmp5C4;int _tmp5C5;union Cyc_Absyn_Constraint _tmp5C6;void*_tmp5C7;union Cyc_Absyn_Constraint _tmp5C8;union Cyc_Absyn_Constraint*_tmp5C9;_LL2F2: _tmp5C4=*_tmp5C3;if((_tmp5C4.No_constr).tag != 3)goto _LL2F4;_tmp5C5=(int)(_tmp5C4.No_constr).val;_LL2F3:
# 1626
 return - 1;_LL2F4: _tmp5C6=*_tmp5C3;if((_tmp5C6.Eq_constr).tag != 1)goto _LL2F6;_tmp5C7=(void*)(_tmp5C6.Eq_constr).val;_LL2F5: {
# 1628
union Cyc_Absyn_Constraint*_tmp5CA=y;union Cyc_Absyn_Constraint _tmp5CB;int _tmp5CC;union Cyc_Absyn_Constraint _tmp5CD;void*_tmp5CE;union Cyc_Absyn_Constraint _tmp5CF;union Cyc_Absyn_Constraint*_tmp5D0;_LL2F9: _tmp5CB=*_tmp5CA;if((_tmp5CB.No_constr).tag != 3)goto _LL2FB;_tmp5CC=(int)(_tmp5CB.No_constr).val;_LL2FA:
# 1629
 return 1;_LL2FB: _tmp5CD=*_tmp5CA;if((_tmp5CD.Eq_constr).tag != 1)goto _LL2FD;_tmp5CE=(void*)(_tmp5CD.Eq_constr).val;_LL2FC:
# 1630
 return cmp(_tmp5C7,_tmp5CE);_LL2FD: _tmp5CF=*_tmp5CA;if((_tmp5CF.Forward_constr).tag != 2)goto _LL2F8;_tmp5D0=(union Cyc_Absyn_Constraint*)(_tmp5CF.Forward_constr).val;_LL2FE: {
# 1631
const char*_tmp1294;void*_tmp1293;(_tmp1293=0,Cyc_Tcutil_impos(((_tmp1294="unify_conref: forward after compress(2)",_tag_dyneither(_tmp1294,sizeof(char),40))),_tag_dyneither(_tmp1293,sizeof(void*),0)));}_LL2F8:;}_LL2F6: _tmp5C8=*_tmp5C3;if((_tmp5C8.Forward_constr).tag != 2)goto _LL2F1;_tmp5C9=(union Cyc_Absyn_Constraint*)(_tmp5C8.Forward_constr).val;_LL2F7: {
# 1633
const char*_tmp1297;void*_tmp1296;(_tmp1296=0,Cyc_Tcutil_impos(((_tmp1297="unify_conref: forward after compress",_tag_dyneither(_tmp1297,sizeof(char),37))),_tag_dyneither(_tmp1296,sizeof(void*),0)));}_LL2F1:;};}
# 1637
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
# 1638
struct _tuple12 _tmp5D6;struct Cyc_Absyn_Tqual _tmp5D7;void*_tmp5D8;struct _tuple12*_tmp5D5=tqt1;_tmp5D6=*_tmp5D5;_tmp5D7=_tmp5D6.f1;_tmp5D8=_tmp5D6.f2;{
# 1639
struct _tuple12 _tmp5DA;struct Cyc_Absyn_Tqual _tmp5DB;void*_tmp5DC;struct _tuple12*_tmp5D9=tqt2;_tmp5DA=*_tmp5D9;_tmp5DB=_tmp5DA.f1;_tmp5DC=_tmp5DA.f2;{
# 1640
int _tmp5DD=Cyc_Tcutil_tqual_cmp(_tmp5D7,_tmp5DB);
# 1641
if(_tmp5DD != 0)return _tmp5DD;
# 1642
return Cyc_Tcutil_typecmp(_tmp5D8,_tmp5DC);};};}
# 1645
static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
# 1646
int _tmp5DE=Cyc_strptrcmp(f1->name,f2->name);
# 1647
if(_tmp5DE != 0)return _tmp5DE;{
# 1648
int _tmp5DF=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
# 1649
if(_tmp5DF != 0)return _tmp5DF;{
# 1650
int _tmp5E0=Cyc_Tcutil_typecmp(f1->type,f2->type);
# 1651
if(_tmp5E0 != 0)return _tmp5E0;{
# 1652
int _tmp5E1=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
# 1653
if(_tmp5E1 != 0)return _tmp5E1;
# 1654
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);};};};}
# 1657
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
# 1658
int _tmp5E2=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
# 1659
if(_tmp5E2 != 0)return _tmp5E2;
# 1660
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1663
static int Cyc_Tcutil_type_case_number(void*t){
# 1664
void*_tmp5E3=t;int _tmp5EC;_LL300: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp5E4=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp5E3;if(_tmp5E4->tag != 0)goto _LL302;}_LL301:
# 1665
 return 0;_LL302: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5E5=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5E3;if(_tmp5E5->tag != 1)goto _LL304;}_LL303:
# 1666
 return 1;_LL304: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp5E6=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5E3;if(_tmp5E6->tag != 2)goto _LL306;}_LL305:
# 1667
 return 2;_LL306: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5E7=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5E3;if(_tmp5E7->tag != 3)goto _LL308;}_LL307:
# 1668
 return 3;_LL308: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp5E8=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5E3;if(_tmp5E8->tag != 4)goto _LL30A;}_LL309:
# 1669
 return 4;_LL30A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5E9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E3;if(_tmp5E9->tag != 5)goto _LL30C;}_LL30B:
# 1670
 return 5;_LL30C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5EA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5E3;if(_tmp5EA->tag != 6)goto _LL30E;}_LL30D:
# 1671
 return 6;_LL30E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5EB=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5E3;if(_tmp5EB->tag != 7)goto _LL310;else{_tmp5EC=_tmp5EB->f1;if(_tmp5EC != 0)goto _LL310;}}_LL30F:
# 1672
 return 7;_LL310: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5ED=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5E3;if(_tmp5ED->tag != 7)goto _LL312;}_LL311:
# 1673
 return 8;_LL312: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5EE=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E3;if(_tmp5EE->tag != 8)goto _LL314;}_LL313:
# 1674
 return 9;_LL314: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5EF=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E3;if(_tmp5EF->tag != 9)goto _LL316;}_LL315:
# 1675
 return 10;_LL316: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp5F0=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5E3;if(_tmp5F0->tag != 10)goto _LL318;}_LL317:
# 1676
 return 11;_LL318: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp5F1=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E3;if(_tmp5F1->tag != 11)goto _LL31A;}_LL319:
# 1677
 return 12;_LL31A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp5F2=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5E3;if(_tmp5F2->tag != 12)goto _LL31C;}_LL31B:
# 1678
 return 14;_LL31C: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp5F3=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5E3;if(_tmp5F3->tag != 13)goto _LL31E;}_LL31D:
# 1679
 return 16;_LL31E: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp5F4=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp5E3;if(_tmp5F4->tag != 14)goto _LL320;}_LL31F:
# 1680
 return 17;_LL320: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp5F5=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5E3;if(_tmp5F5->tag != 15)goto _LL322;}_LL321:
# 1681
 return 18;_LL322: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5F6=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E3;if(_tmp5F6->tag != 17)goto _LL324;}_LL323:
# 1682
 return 19;_LL324: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp5F7=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp5E3;if(_tmp5F7->tag != 21)goto _LL326;}_LL325:
# 1683
 return 20;_LL326: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp5F8=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp5E3;if(_tmp5F8->tag != 20)goto _LL328;}_LL327:
# 1684
 return 21;_LL328: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp5F9=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp5E3;if(_tmp5F9->tag != 23)goto _LL32A;}_LL329:
# 1685
 return 22;_LL32A: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp5FA=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp5E3;if(_tmp5FA->tag != 24)goto _LL32C;}_LL32B:
# 1686
 return 23;_LL32C: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp5FB=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp5E3;if(_tmp5FB->tag != 25)goto _LL32E;}_LL32D:
# 1687
 return 24;_LL32E: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp5FC=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp5E3;if(_tmp5FC->tag != 22)goto _LL330;}_LL32F:
# 1688
 return 25;_LL330: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5FD=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5E3;if(_tmp5FD->tag != 19)goto _LL332;}_LL331:
# 1689
 return 26;_LL332: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp5FE=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp5E3;if(_tmp5FE->tag != 16)goto _LL334;}_LL333:
# 1690
 return 27;_LL334: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp5FF=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp5E3;if(_tmp5FF->tag != 18)goto _LL336;}_LL335:
# 1691
 return 28;_LL336: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp600=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E3;if(_tmp600->tag != 26)goto _LL2FF;}_LL337:
# 1692
 return 29;_LL2FF:;}
# 1698
int Cyc_Tcutil_typecmp(void*t1,void*t2){
# 1699
t1=Cyc_Tcutil_compress(t1);
# 1700
t2=Cyc_Tcutil_compress(t2);
# 1701
if(t1 == t2)return 0;{
# 1702
int _tmp601=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));
# 1703
if(_tmp601 != 0)
# 1704
return _tmp601;{
# 1707
struct _tuple0 _tmp1298;struct _tuple0 _tmp603=(_tmp1298.f1=t2,((_tmp1298.f2=t1,_tmp1298)));void*_tmp604;void*_tmp606;void*_tmp608;struct Cyc_Absyn_Tvar*_tmp60A;void*_tmp60B;struct Cyc_Absyn_Tvar*_tmp60D;void*_tmp60E;struct Cyc_Absyn_AggrInfo _tmp610;union Cyc_Absyn_AggrInfoU _tmp611;struct Cyc_List_List*_tmp612;void*_tmp613;struct Cyc_Absyn_AggrInfo _tmp615;union Cyc_Absyn_AggrInfoU _tmp616;struct Cyc_List_List*_tmp617;void*_tmp618;struct _tuple2*_tmp61A;void*_tmp61B;struct _tuple2*_tmp61D;void*_tmp61E;struct Cyc_List_List*_tmp620;void*_tmp621;struct Cyc_List_List*_tmp623;void*_tmp624;struct Cyc_Absyn_DatatypeInfo _tmp626;union Cyc_Absyn_DatatypeInfoU _tmp627;struct Cyc_Absyn_Datatypedecl**_tmp628;struct Cyc_Absyn_Datatypedecl*_tmp629;struct Cyc_List_List*_tmp62A;void*_tmp62B;struct Cyc_Absyn_DatatypeInfo _tmp62D;union Cyc_Absyn_DatatypeInfoU _tmp62E;struct Cyc_Absyn_Datatypedecl**_tmp62F;struct Cyc_Absyn_Datatypedecl*_tmp630;struct Cyc_List_List*_tmp631;void*_tmp632;struct Cyc_Absyn_DatatypeFieldInfo _tmp634;union Cyc_Absyn_DatatypeFieldInfoU _tmp635;struct _tuple3 _tmp636;struct Cyc_Absyn_Datatypedecl*_tmp637;struct Cyc_Absyn_Datatypefield*_tmp638;struct Cyc_List_List*_tmp639;void*_tmp63A;struct Cyc_Absyn_DatatypeFieldInfo _tmp63C;union Cyc_Absyn_DatatypeFieldInfoU _tmp63D;struct _tuple3 _tmp63E;struct Cyc_Absyn_Datatypedecl*_tmp63F;struct Cyc_Absyn_Datatypefield*_tmp640;struct Cyc_List_List*_tmp641;void*_tmp642;struct Cyc_Absyn_PtrInfo _tmp644;void*_tmp645;struct Cyc_Absyn_Tqual _tmp646;struct Cyc_Absyn_PtrAtts _tmp647;void*_tmp648;union Cyc_Absyn_Constraint*_tmp649;union Cyc_Absyn_Constraint*_tmp64A;union Cyc_Absyn_Constraint*_tmp64B;void*_tmp64C;struct Cyc_Absyn_PtrInfo _tmp64E;void*_tmp64F;struct Cyc_Absyn_Tqual _tmp650;struct Cyc_Absyn_PtrAtts _tmp651;void*_tmp652;union Cyc_Absyn_Constraint*_tmp653;union Cyc_Absyn_Constraint*_tmp654;union Cyc_Absyn_Constraint*_tmp655;void*_tmp656;enum Cyc_Absyn_Sign _tmp658;enum Cyc_Absyn_Size_of _tmp659;void*_tmp65A;enum Cyc_Absyn_Sign _tmp65C;enum Cyc_Absyn_Size_of _tmp65D;void*_tmp65E;int _tmp660;void*_tmp661;int _tmp663;void*_tmp664;struct Cyc_Absyn_ArrayInfo _tmp666;void*_tmp667;struct Cyc_Absyn_Tqual _tmp668;struct Cyc_Absyn_Exp*_tmp669;union Cyc_Absyn_Constraint*_tmp66A;void*_tmp66B;struct Cyc_Absyn_ArrayInfo _tmp66D;void*_tmp66E;struct Cyc_Absyn_Tqual _tmp66F;struct Cyc_Absyn_Exp*_tmp670;union Cyc_Absyn_Constraint*_tmp671;void*_tmp672;struct Cyc_Absyn_FnInfo _tmp674;struct Cyc_List_List*_tmp675;void*_tmp676;struct Cyc_Absyn_Tqual _tmp677;void*_tmp678;struct Cyc_List_List*_tmp679;int _tmp67A;struct Cyc_Absyn_VarargInfo*_tmp67B;struct Cyc_List_List*_tmp67C;struct Cyc_List_List*_tmp67D;void*_tmp67E;struct Cyc_Absyn_FnInfo _tmp680;struct Cyc_List_List*_tmp681;void*_tmp682;struct Cyc_Absyn_Tqual _tmp683;void*_tmp684;struct Cyc_List_List*_tmp685;int _tmp686;struct Cyc_Absyn_VarargInfo*_tmp687;struct Cyc_List_List*_tmp688;struct Cyc_List_List*_tmp689;void*_tmp68A;struct Cyc_List_List*_tmp68C;void*_tmp68D;struct Cyc_List_List*_tmp68F;void*_tmp690;enum Cyc_Absyn_AggrKind _tmp692;struct Cyc_List_List*_tmp693;void*_tmp694;enum Cyc_Absyn_AggrKind _tmp696;struct Cyc_List_List*_tmp697;void*_tmp698;void*_tmp69A;void*_tmp69B;void*_tmp69D;void*_tmp69E;void*_tmp6A0;void*_tmp6A1;void*_tmp6A2;void*_tmp6A4;void*_tmp6A5;void*_tmp6A6;void*_tmp6A8;void*_tmp6A9;void*_tmp6AB;void*_tmp6AC;struct Cyc_Absyn_Exp*_tmp6AE;void*_tmp6AF;struct Cyc_Absyn_Exp*_tmp6B1;void*_tmp6B2;void*_tmp6B4;void*_tmp6B6;void*_tmp6B8;void*_tmp6BA;void*_tmp6BC;_LL339: _tmp604=_tmp603.f1;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp605=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp604;if(_tmp605->tag != 1)goto _LL33B;};_tmp606=_tmp603.f2;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp607=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp606;if(_tmp607->tag != 1)goto _LL33B;};_LL33A: {
# 1709
const char*_tmp129B;void*_tmp129A;(_tmp129A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp129B="typecmp: can only compare closed types",_tag_dyneither(_tmp129B,sizeof(char),39))),_tag_dyneither(_tmp129A,sizeof(void*),0)));}_LL33B: _tmp608=_tmp603.f1;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp609=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp608;if(_tmp609->tag != 2)goto _LL33D;else{_tmp60A=_tmp609->f1;}};_tmp60B=_tmp603.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp60C=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp60B;if(_tmp60C->tag != 2)goto _LL33D;else{_tmp60D=_tmp60C->f1;}};_LL33C:
# 1713
 return Cyc_Core_intcmp(_tmp60D->identity,_tmp60A->identity);_LL33D: _tmp60E=_tmp603.f1;{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp60F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp60E;if(_tmp60F->tag != 11)goto _LL33F;else{_tmp610=_tmp60F->f1;_tmp611=_tmp610.aggr_info;_tmp612=_tmp610.targs;}};_tmp613=_tmp603.f2;{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp614=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp613;if(_tmp614->tag != 11)goto _LL33F;else{_tmp615=_tmp614->f1;_tmp616=_tmp615.aggr_info;_tmp617=_tmp615.targs;}};_LL33E: {
# 1717
struct _tuple2*_tmp6C1;struct _tuple13 _tmp6C0=Cyc_Absyn_aggr_kinded_name(_tmp611);_tmp6C1=_tmp6C0.f2;{
# 1718
struct _tuple2*_tmp6C3;struct _tuple13 _tmp6C2=Cyc_Absyn_aggr_kinded_name(_tmp616);_tmp6C3=_tmp6C2.f2;{
# 1719
int _tmp6C4=Cyc_Absyn_qvar_cmp(_tmp6C1,_tmp6C3);
# 1720
if(_tmp6C4 != 0)return _tmp6C4;else{
# 1721
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp612,_tmp617);}};};}_LL33F: _tmp618=_tmp603.f1;{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp619=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp618;if(_tmp619->tag != 13)goto _LL341;else{_tmp61A=_tmp619->f1;}};_tmp61B=_tmp603.f2;{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp61C=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp61B;if(_tmp61C->tag != 13)goto _LL341;else{_tmp61D=_tmp61C->f1;}};_LL340:
# 1724
 return Cyc_Absyn_qvar_cmp(_tmp61A,_tmp61D);_LL341: _tmp61E=_tmp603.f1;{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp61F=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp61E;if(_tmp61F->tag != 14)goto _LL343;else{_tmp620=_tmp61F->f1;}};_tmp621=_tmp603.f2;{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp622=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp621;if(_tmp622->tag != 14)goto _LL343;else{_tmp623=_tmp622->f1;}};_LL342:
# 1727
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp620,_tmp623);_LL343: _tmp624=_tmp603.f1;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp625=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp624;if(_tmp625->tag != 3)goto _LL345;else{_tmp626=_tmp625->f1;_tmp627=_tmp626.datatype_info;if((_tmp627.KnownDatatype).tag != 2)goto _LL345;_tmp628=(struct Cyc_Absyn_Datatypedecl**)(_tmp627.KnownDatatype).val;_tmp629=*_tmp628;_tmp62A=_tmp626.targs;}};_tmp62B=_tmp603.f2;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp62C=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp62B;if(_tmp62C->tag != 3)goto _LL345;else{_tmp62D=_tmp62C->f1;_tmp62E=_tmp62D.datatype_info;if((_tmp62E.KnownDatatype).tag != 2)goto _LL345;_tmp62F=(struct Cyc_Absyn_Datatypedecl**)(_tmp62E.KnownDatatype).val;_tmp630=*_tmp62F;_tmp631=_tmp62D.targs;}};_LL344:
# 1731
 if(_tmp630 == _tmp629)return 0;{
# 1732
int _tmp6C5=Cyc_Absyn_qvar_cmp(_tmp630->name,_tmp629->name);
# 1733
if(_tmp6C5 != 0)return _tmp6C5;
# 1734
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp631,_tmp62A);};_LL345: _tmp632=_tmp603.f1;{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp633=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp632;if(_tmp633->tag != 4)goto _LL347;else{_tmp634=_tmp633->f1;_tmp635=_tmp634.field_info;if((_tmp635.KnownDatatypefield).tag != 2)goto _LL347;_tmp636=(struct _tuple3)(_tmp635.KnownDatatypefield).val;_tmp637=_tmp636.f1;_tmp638=_tmp636.f2;_tmp639=_tmp634.targs;}};_tmp63A=_tmp603.f2;{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp63B=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp63A;if(_tmp63B->tag != 4)goto _LL347;else{_tmp63C=_tmp63B->f1;_tmp63D=_tmp63C.field_info;if((_tmp63D.KnownDatatypefield).tag != 2)goto _LL347;_tmp63E=(struct _tuple3)(_tmp63D.KnownDatatypefield).val;_tmp63F=_tmp63E.f1;_tmp640=_tmp63E.f2;_tmp641=_tmp63C.targs;}};_LL346:
# 1738
 if(_tmp63F == _tmp637)return 0;{
# 1739
int _tmp6C6=Cyc_Absyn_qvar_cmp(_tmp637->name,_tmp63F->name);
# 1740
if(_tmp6C6 != 0)return _tmp6C6;{
# 1741
int _tmp6C7=Cyc_Absyn_qvar_cmp(_tmp638->name,_tmp640->name);
# 1742
if(_tmp6C7 != 0)return _tmp6C7;
# 1743
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp641,_tmp639);};};_LL347: _tmp642=_tmp603.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp643=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp642;if(_tmp643->tag != 5)goto _LL349;else{_tmp644=_tmp643->f1;_tmp645=_tmp644.elt_typ;_tmp646=_tmp644.elt_tq;_tmp647=_tmp644.ptr_atts;_tmp648=_tmp647.rgn;_tmp649=_tmp647.nullable;_tmp64A=_tmp647.bounds;_tmp64B=_tmp647.zero_term;}};_tmp64C=_tmp603.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp64D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp64C;if(_tmp64D->tag != 5)goto _LL349;else{_tmp64E=_tmp64D->f1;_tmp64F=_tmp64E.elt_typ;_tmp650=_tmp64E.elt_tq;_tmp651=_tmp64E.ptr_atts;_tmp652=_tmp651.rgn;_tmp653=_tmp651.nullable;_tmp654=_tmp651.bounds;_tmp655=_tmp651.zero_term;}};_LL348: {
# 1747
int _tmp6C8=Cyc_Tcutil_typecmp(_tmp64F,_tmp645);
# 1748
if(_tmp6C8 != 0)return _tmp6C8;{
# 1749
int _tmp6C9=Cyc_Tcutil_typecmp(_tmp652,_tmp648);
# 1750
if(_tmp6C9 != 0)return _tmp6C9;{
# 1751
int _tmp6CA=Cyc_Tcutil_tqual_cmp(_tmp650,_tmp646);
# 1752
if(_tmp6CA != 0)return _tmp6CA;{
# 1753
int _tmp6CB=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp654,_tmp64A);
# 1754
if(_tmp6CB != 0)return _tmp6CB;{
# 1755
int _tmp6CC=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp655,_tmp64B);
# 1756
if(_tmp6CC != 0)return _tmp6CC;
# 1757
{void*_tmp6CD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp654);_LL36C: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6CE=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6CD;if(_tmp6CE->tag != 0)goto _LL36E;}_LL36D:
# 1758
 return 0;_LL36E:;_LL36F:
# 1759
 goto _LL36B;_LL36B:;}
# 1761
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp653,_tmp649);};};};};}_LL349: _tmp656=_tmp603.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp657=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp656;if(_tmp657->tag != 6)goto _LL34B;else{_tmp658=_tmp657->f1;_tmp659=_tmp657->f2;}};_tmp65A=_tmp603.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp65B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp65A;if(_tmp65B->tag != 6)goto _LL34B;else{_tmp65C=_tmp65B->f1;_tmp65D=_tmp65B->f2;}};_LL34A:
# 1764
 if(_tmp65C != _tmp658)return Cyc_Core_intcmp((int)((unsigned int)_tmp65C),(int)((unsigned int)_tmp658));
# 1765
if(_tmp65D != _tmp659)return Cyc_Core_intcmp((int)((unsigned int)_tmp65D),(int)((unsigned int)_tmp659));
# 1766
return 0;_LL34B: _tmp65E=_tmp603.f1;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp65F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp65E;if(_tmp65F->tag != 7)goto _LL34D;else{_tmp660=_tmp65F->f1;}};_tmp661=_tmp603.f2;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp662=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp661;if(_tmp662->tag != 7)goto _LL34D;else{_tmp663=_tmp662->f1;}};_LL34C:
# 1769
 if(_tmp660 == _tmp663)return 0;else{
# 1770
if(_tmp663 == 0)return - 1;else{
# 1771
if(_tmp663 == 1  && _tmp660 == 0)return - 1;else{
# 1772
return 1;}}}_LL34D: _tmp664=_tmp603.f1;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp665=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp664;if(_tmp665->tag != 8)goto _LL34F;else{_tmp666=_tmp665->f1;_tmp667=_tmp666.elt_type;_tmp668=_tmp666.tq;_tmp669=_tmp666.num_elts;_tmp66A=_tmp666.zero_term;}};_tmp66B=_tmp603.f2;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp66C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp66B;if(_tmp66C->tag != 8)goto _LL34F;else{_tmp66D=_tmp66C->f1;_tmp66E=_tmp66D.elt_type;_tmp66F=_tmp66D.tq;_tmp670=_tmp66D.num_elts;_tmp671=_tmp66D.zero_term;}};_LL34E: {
# 1776
int _tmp6CF=Cyc_Tcutil_tqual_cmp(_tmp66F,_tmp668);
# 1777
if(_tmp6CF != 0)return _tmp6CF;{
# 1778
int _tmp6D0=Cyc_Tcutil_typecmp(_tmp66E,_tmp667);
# 1779
if(_tmp6D0 != 0)return _tmp6D0;{
# 1780
int _tmp6D1=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp66A,_tmp671);
# 1781
if(_tmp6D1 != 0)return _tmp6D1;
# 1782
if(_tmp669 == _tmp670)return 0;
# 1783
if(_tmp669 == 0  || _tmp670 == 0){
# 1784
const char*_tmp129E;void*_tmp129D;(_tmp129D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp129E="missing expression in array index",_tag_dyneither(_tmp129E,sizeof(char),34))),_tag_dyneither(_tmp129D,sizeof(void*),0)));}
# 1786
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp669,_tmp670);};};}_LL34F: _tmp672=_tmp603.f1;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp673=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp672;if(_tmp673->tag != 9)goto _LL351;else{_tmp674=_tmp673->f1;_tmp675=_tmp674.tvars;_tmp676=_tmp674.effect;_tmp677=_tmp674.ret_tqual;_tmp678=_tmp674.ret_typ;_tmp679=_tmp674.args;_tmp67A=_tmp674.c_varargs;_tmp67B=_tmp674.cyc_varargs;_tmp67C=_tmp674.rgn_po;_tmp67D=_tmp674.attributes;}};_tmp67E=_tmp603.f2;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp67F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp67E;if(_tmp67F->tag != 9)goto _LL351;else{_tmp680=_tmp67F->f1;_tmp681=_tmp680.tvars;_tmp682=_tmp680.effect;_tmp683=_tmp680.ret_tqual;_tmp684=_tmp680.ret_typ;_tmp685=_tmp680.args;_tmp686=_tmp680.c_varargs;_tmp687=_tmp680.cyc_varargs;_tmp688=_tmp680.rgn_po;_tmp689=_tmp680.attributes;}};_LL350: {
# 1790
const char*_tmp12A1;void*_tmp12A0;(_tmp12A0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12A1="typecmp: function types not handled",_tag_dyneither(_tmp12A1,sizeof(char),36))),_tag_dyneither(_tmp12A0,sizeof(void*),0)));}_LL351: _tmp68A=_tmp603.f1;{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp68B=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp68A;if(_tmp68B->tag != 10)goto _LL353;else{_tmp68C=_tmp68B->f1;}};_tmp68D=_tmp603.f2;{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp68E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp68D;if(_tmp68E->tag != 10)goto _LL353;else{_tmp68F=_tmp68E->f1;}};_LL352:
# 1793
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp68F,_tmp68C);_LL353: _tmp690=_tmp603.f1;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp691=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp690;if(_tmp691->tag != 12)goto _LL355;else{_tmp692=_tmp691->f1;_tmp693=_tmp691->f2;}};_tmp694=_tmp603.f2;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp695=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp694;if(_tmp695->tag != 12)goto _LL355;else{_tmp696=_tmp695->f1;_tmp697=_tmp695->f2;}};_LL354:
# 1796
 if(_tmp696 != _tmp692){
# 1797
if(_tmp696 == Cyc_Absyn_StructA)return - 1;else{
# 1798
return 1;}}
# 1799
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp697,_tmp693);_LL355: _tmp698=_tmp603.f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp699=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp698;if(_tmp699->tag != 15)goto _LL357;else{_tmp69A=(void*)_tmp699->f1;}};_tmp69B=_tmp603.f2;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp69C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp69B;if(_tmp69C->tag != 15)goto _LL357;else{_tmp69D=(void*)_tmp69C->f1;}};_LL356:
# 1801
 return Cyc_Tcutil_typecmp(_tmp69A,_tmp69D);_LL357: _tmp69E=_tmp603.f1;{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp69F=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp69E;if(_tmp69F->tag != 16)goto _LL359;else{_tmp6A0=(void*)_tmp69F->f1;_tmp6A1=(void*)_tmp69F->f2;}};_tmp6A2=_tmp603.f2;{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp6A3=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp6A2;if(_tmp6A3->tag != 16)goto _LL359;else{_tmp6A4=(void*)_tmp6A3->f1;_tmp6A5=(void*)_tmp6A3->f2;}};_LL358: {
# 1803
int _tmp6D6=Cyc_Tcutil_typecmp(_tmp6A0,_tmp6A4);
# 1804
if(_tmp6D6 != 0)return _tmp6D6;else{
# 1805
return Cyc_Tcutil_typecmp(_tmp6A1,_tmp6A5);}}_LL359: _tmp6A6=_tmp603.f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6A7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp6A6;if(_tmp6A7->tag != 19)goto _LL35B;else{_tmp6A8=(void*)_tmp6A7->f1;}};_tmp6A9=_tmp603.f2;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6AA=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp6A9;if(_tmp6AA->tag != 19)goto _LL35B;else{_tmp6AB=(void*)_tmp6AA->f1;}};_LL35A:
# 1806
 return Cyc_Tcutil_typecmp(_tmp6A8,_tmp6AB);_LL35B: _tmp6AC=_tmp603.f1;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp6AD=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6AC;if(_tmp6AD->tag != 18)goto _LL35D;else{_tmp6AE=_tmp6AD->f1;}};_tmp6AF=_tmp603.f2;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp6B0=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6AF;if(_tmp6B0->tag != 18)goto _LL35D;else{_tmp6B1=_tmp6B0->f1;}};_LL35C:
# 1808
 return Cyc_Evexp_const_exp_cmp(_tmp6AE,_tmp6B1);_LL35D: _tmp6B2=_tmp603.f1;{struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6B3=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp6B2;if(_tmp6B3->tag != 24)goto _LL35F;};_LL35E:
# 1809
 goto _LL360;_LL35F: _tmp6B4=_tmp603.f2;{struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6B5=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp6B4;if(_tmp6B5->tag != 24)goto _LL361;};_LL360:
# 1810
 goto _LL362;_LL361: _tmp6B6=_tmp603.f1;{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp6B7=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp6B6;if(_tmp6B7->tag != 23)goto _LL363;};_LL362:
# 1811
 goto _LL364;_LL363: _tmp6B8=_tmp603.f1;{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp6B9=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp6B8;if(_tmp6B9->tag != 25)goto _LL365;};_LL364:
# 1812
 goto _LL366;_LL365: _tmp6BA=_tmp603.f2;{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp6BB=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp6BA;if(_tmp6BB->tag != 25)goto _LL367;};_LL366:
# 1813
 goto _LL368;_LL367: _tmp6BC=_tmp603.f2;{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp6BD=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp6BC;if(_tmp6BD->tag != 23)goto _LL369;};_LL368: {
# 1814
const char*_tmp12A4;void*_tmp12A3;(_tmp12A3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12A4="typecmp: effects not handled",_tag_dyneither(_tmp12A4,sizeof(char),29))),_tag_dyneither(_tmp12A3,sizeof(void*),0)));}_LL369:;_LL36A: {
# 1815
const char*_tmp12A7;void*_tmp12A6;(_tmp12A6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12A7="Unmatched case in typecmp",_tag_dyneither(_tmp12A7,sizeof(char),26))),_tag_dyneither(_tmp12A6,sizeof(void*),0)));}_LL338:;};};}
# 1819
int Cyc_Tcutil_is_arithmetic_type(void*t){
# 1820
void*_tmp6DB=Cyc_Tcutil_compress(t);_LL371: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6DC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6DB;if(_tmp6DC->tag != 6)goto _LL373;}_LL372:
# 1821
 goto _LL374;_LL373: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp6DD=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6DB;if(_tmp6DD->tag != 7)goto _LL375;}_LL374:
# 1822
 goto _LL376;_LL375: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp6DE=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp6DB;if(_tmp6DE->tag != 13)goto _LL377;}_LL376:
# 1823
 goto _LL378;_LL377: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp6DF=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp6DB;if(_tmp6DF->tag != 14)goto _LL379;}_LL378:
# 1824
 return 1;_LL379:;_LL37A:
# 1825
 return 0;_LL370:;}
# 1831
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
# 1832
t1=Cyc_Tcutil_compress(t1);
# 1833
t2=Cyc_Tcutil_compress(t2);{
# 1834
struct _tuple0 _tmp12A8;struct _tuple0 _tmp6E1=(_tmp12A8.f1=t1,((_tmp12A8.f2=t2,_tmp12A8)));void*_tmp6E2;int _tmp6E4;void*_tmp6E5;int _tmp6E7;void*_tmp6E8;void*_tmp6EA;void*_tmp6EC;void*_tmp6EE;void*_tmp6F0;enum Cyc_Absyn_Size_of _tmp6F2;void*_tmp6F3;enum Cyc_Absyn_Size_of _tmp6F5;void*_tmp6F6;enum Cyc_Absyn_Size_of _tmp6F8;void*_tmp6F9;enum Cyc_Absyn_Size_of _tmp6FB;void*_tmp6FC;enum Cyc_Absyn_Size_of _tmp6FE;void*_tmp6FF;enum Cyc_Absyn_Size_of _tmp701;void*_tmp702;enum Cyc_Absyn_Size_of _tmp704;void*_tmp705;enum Cyc_Absyn_Size_of _tmp707;void*_tmp708;int _tmp70A;void*_tmp70B;enum Cyc_Absyn_Size_of _tmp70D;void*_tmp70E;int _tmp710;void*_tmp711;enum Cyc_Absyn_Size_of _tmp713;void*_tmp714;enum Cyc_Absyn_Size_of _tmp716;void*_tmp717;enum Cyc_Absyn_Size_of _tmp719;void*_tmp71A;enum Cyc_Absyn_Size_of _tmp71C;void*_tmp71D;void*_tmp71F;enum Cyc_Absyn_Size_of _tmp721;void*_tmp722;enum Cyc_Absyn_Size_of _tmp724;void*_tmp725;enum Cyc_Absyn_Size_of _tmp727;void*_tmp728;enum Cyc_Absyn_Size_of _tmp72A;void*_tmp72B;enum Cyc_Absyn_Size_of _tmp72D;void*_tmp72E;enum Cyc_Absyn_Size_of _tmp730;void*_tmp731;enum Cyc_Absyn_Size_of _tmp733;void*_tmp734;void*_tmp736;enum Cyc_Absyn_Size_of _tmp738;_LL37C: _tmp6E2=_tmp6E1.f1;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp6E3=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6E2;if(_tmp6E3->tag != 7)goto _LL37E;else{_tmp6E4=_tmp6E3->f1;}};_tmp6E5=_tmp6E1.f2;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp6E6=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6E5;if(_tmp6E6->tag != 7)goto _LL37E;else{_tmp6E7=_tmp6E6->f1;}};_LL37D:
# 1836
 return _tmp6E7 == 0  && _tmp6E4 != 0  || (_tmp6E7 == 1  && _tmp6E4 != 0) && _tmp6E4 != 1;_LL37E: _tmp6E8=_tmp6E1.f1;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp6E9=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6E8;if(_tmp6E9->tag != 7)goto _LL380;};_tmp6EA=_tmp6E1.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6EB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6EA;if(_tmp6EB->tag != 6)goto _LL380;};_LL37F:
# 1837
 goto _LL381;_LL380: _tmp6EC=_tmp6E1.f1;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp6ED=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6EC;if(_tmp6ED->tag != 7)goto _LL382;};_tmp6EE=_tmp6E1.f2;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6EF=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp6EE;if(_tmp6EF->tag != 19)goto _LL382;};_LL381:
# 1838
 return 1;_LL382: _tmp6F0=_tmp6E1.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6F1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6F0;if(_tmp6F1->tag != 6)goto _LL384;else{_tmp6F2=_tmp6F1->f2;if(_tmp6F2 != Cyc_Absyn_LongLong_sz)goto _LL384;}};_tmp6F3=_tmp6E1.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6F4=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6F3;if(_tmp6F4->tag != 6)goto _LL384;else{_tmp6F5=_tmp6F4->f2;if(_tmp6F5 != Cyc_Absyn_LongLong_sz)goto _LL384;}};_LL383:
# 1839
 return 0;_LL384: _tmp6F6=_tmp6E1.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6F7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6F6;if(_tmp6F7->tag != 6)goto _LL386;else{_tmp6F8=_tmp6F7->f2;if(_tmp6F8 != Cyc_Absyn_LongLong_sz)goto _LL386;}};_LL385:
# 1840
 return 1;_LL386: _tmp6F9=_tmp6E1.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6FA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6F9;if(_tmp6FA->tag != 6)goto _LL388;else{_tmp6FB=_tmp6FA->f2;if(_tmp6FB != Cyc_Absyn_Long_sz)goto _LL388;}};_tmp6FC=_tmp6E1.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6FD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6FC;if(_tmp6FD->tag != 6)goto _LL388;else{_tmp6FE=_tmp6FD->f2;if(_tmp6FE != Cyc_Absyn_Int_sz)goto _LL388;}};_LL387:
# 1843
 goto _LL389;_LL388: _tmp6FF=_tmp6E1.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp700=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6FF;if(_tmp700->tag != 6)goto _LL38A;else{_tmp701=_tmp700->f2;if(_tmp701 != Cyc_Absyn_Int_sz)goto _LL38A;}};_tmp702=_tmp6E1.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp703=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp702;if(_tmp703->tag != 6)goto _LL38A;else{_tmp704=_tmp703->f2;if(_tmp704 != Cyc_Absyn_Long_sz)goto _LL38A;}};_LL389:
# 1844
 return 0;_LL38A: _tmp705=_tmp6E1.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp706=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp705;if(_tmp706->tag != 6)goto _LL38C;else{_tmp707=_tmp706->f2;if(_tmp707 != Cyc_Absyn_Long_sz)goto _LL38C;}};_tmp708=_tmp6E1.f2;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp709=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp708;if(_tmp709->tag != 7)goto _LL38C;else{_tmp70A=_tmp709->f1;if(_tmp70A != 0)goto _LL38C;}};_LL38B:
# 1846
 goto _LL38D;_LL38C: _tmp70B=_tmp6E1.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp70C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp70B;if(_tmp70C->tag != 6)goto _LL38E;else{_tmp70D=_tmp70C->f2;if(_tmp70D != Cyc_Absyn_Int_sz)goto _LL38E;}};_tmp70E=_tmp6E1.f2;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp70F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp70E;if(_tmp70F->tag != 7)goto _LL38E;else{_tmp710=_tmp70F->f1;if(_tmp710 != 0)goto _LL38E;}};_LL38D:
# 1847
 goto _LL38F;_LL38E: _tmp711=_tmp6E1.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp712=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp711;if(_tmp712->tag != 6)goto _LL390;else{_tmp713=_tmp712->f2;if(_tmp713 != Cyc_Absyn_Long_sz)goto _LL390;}};_tmp714=_tmp6E1.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp715=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp714;if(_tmp715->tag != 6)goto _LL390;else{_tmp716=_tmp715->f2;if(_tmp716 != Cyc_Absyn_Short_sz)goto _LL390;}};_LL38F:
# 1848
 goto _LL391;_LL390: _tmp717=_tmp6E1.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp718=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp717;if(_tmp718->tag != 6)goto _LL392;else{_tmp719=_tmp718->f2;if(_tmp719 != Cyc_Absyn_Int_sz)goto _LL392;}};_tmp71A=_tmp6E1.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp71B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp71A;if(_tmp71B->tag != 6)goto _LL392;else{_tmp71C=_tmp71B->f2;if(_tmp71C != Cyc_Absyn_Short_sz)goto _LL392;}};_LL391:
# 1849
 goto _LL393;_LL392: _tmp71D=_tmp6E1.f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp71E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp71D;if(_tmp71E->tag != 19)goto _LL394;};_tmp71F=_tmp6E1.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp720=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp71F;if(_tmp720->tag != 6)goto _LL394;else{_tmp721=_tmp720->f2;if(_tmp721 != Cyc_Absyn_Short_sz)goto _LL394;}};_LL393:
# 1850
 goto _LL395;_LL394: _tmp722=_tmp6E1.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp723=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp722;if(_tmp723->tag != 6)goto _LL396;else{_tmp724=_tmp723->f2;if(_tmp724 != Cyc_Absyn_Long_sz)goto _LL396;}};_tmp725=_tmp6E1.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp726=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp725;if(_tmp726->tag != 6)goto _LL396;else{_tmp727=_tmp726->f2;if(_tmp727 != Cyc_Absyn_Char_sz)goto _LL396;}};_LL395:
# 1851
 goto _LL397;_LL396: _tmp728=_tmp6E1.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp729=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp728;if(_tmp729->tag != 6)goto _LL398;else{_tmp72A=_tmp729->f2;if(_tmp72A != Cyc_Absyn_Int_sz)goto _LL398;}};_tmp72B=_tmp6E1.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp72C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp72B;if(_tmp72C->tag != 6)goto _LL398;else{_tmp72D=_tmp72C->f2;if(_tmp72D != Cyc_Absyn_Char_sz)goto _LL398;}};_LL397:
# 1852
 goto _LL399;_LL398: _tmp72E=_tmp6E1.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp72F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp72E;if(_tmp72F->tag != 6)goto _LL39A;else{_tmp730=_tmp72F->f2;if(_tmp730 != Cyc_Absyn_Short_sz)goto _LL39A;}};_tmp731=_tmp6E1.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp732=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp731;if(_tmp732->tag != 6)goto _LL39A;else{_tmp733=_tmp732->f2;if(_tmp733 != Cyc_Absyn_Char_sz)goto _LL39A;}};_LL399:
# 1853
 goto _LL39B;_LL39A: _tmp734=_tmp6E1.f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp735=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp734;if(_tmp735->tag != 19)goto _LL39C;};_tmp736=_tmp6E1.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp737=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp736;if(_tmp737->tag != 6)goto _LL39C;else{_tmp738=_tmp737->f2;if(_tmp738 != Cyc_Absyn_Char_sz)goto _LL39C;}};_LL39B:
# 1854
 return 1;_LL39C:;_LL39D:
# 1856
 return 0;_LL37B:;};}
# 1862
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 1865
struct _RegionHandle _tmp739=_new_region("r");struct _RegionHandle*r=& _tmp739;_push_region(r);{
# 1866
struct Cyc_Core_Opt*max_arith_type=0;
# 1867
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
# 1868
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
# 1869
if(Cyc_Tcutil_is_arithmetic_type(t1)){
# 1870
if(max_arith_type == 0  || 
# 1871
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v)){
# 1872
struct Cyc_Core_Opt*_tmp12A9;max_arith_type=((_tmp12A9=_region_malloc(r,sizeof(*_tmp12A9)),((_tmp12A9->v=t1,_tmp12A9))));}}}}
# 1875
if(max_arith_type != 0){
# 1876
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){
# 1877
int _tmp73B=0;_npop_handler(0);return _tmp73B;}}}
# 1880
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
# 1881
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
# 1882
{const char*_tmp12AE;void*_tmp12AD[2];struct Cyc_String_pa_PrintArg_struct _tmp12AC;struct Cyc_String_pa_PrintArg_struct _tmp12AB;(_tmp12AB.tag=0,((_tmp12AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1883
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))),((_tmp12AC.tag=0,((_tmp12AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp12AD[0]=& _tmp12AC,((_tmp12AD[1]=& _tmp12AB,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,((_tmp12AE="type mismatch: expecting %s but found %s",_tag_dyneither(_tmp12AE,sizeof(char),41))),_tag_dyneither(_tmp12AD,sizeof(void*),2)))))))))))));}{
# 1884
int _tmp740=0;_npop_handler(0);return _tmp740;};}}}{
# 1886
int _tmp741=1;_npop_handler(0);return _tmp741;};
# 1865
;_pop_region(r);}
# 1891
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 1892
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
# 1893
void*_tmp742=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL39F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp743=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp742;if(_tmp743->tag != 5)goto _LL3A1;}_LL3A0:
# 1894
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL39E;_LL3A1:;_LL3A2:
# 1895
 return 0;_LL39E:;}
# 1897
return 1;}
# 1900
int Cyc_Tcutil_is_integral_type(void*t){
# 1901
void*_tmp744=Cyc_Tcutil_compress(t);_LL3A4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp745=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp744;if(_tmp745->tag != 6)goto _LL3A6;}_LL3A5:
# 1902
 goto _LL3A7;_LL3A6: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp746=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp744;if(_tmp746->tag != 19)goto _LL3A8;}_LL3A7:
# 1903
 goto _LL3A9;_LL3A8: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp747=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp744;if(_tmp747->tag != 13)goto _LL3AA;}_LL3A9:
# 1904
 goto _LL3AB;_LL3AA: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp748=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp744;if(_tmp748->tag != 14)goto _LL3AC;}_LL3AB:
# 1905
 return 1;_LL3AC:;_LL3AD:
# 1906
 return 0;_LL3A3:;}
# 1911
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 1912
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
# 1913
return 1;
# 1915
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
# 1916
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ)){
# 1917
const char*_tmp12B1;void*_tmp12B0;(_tmp12B0=0,Cyc_Tcutil_warn(e->loc,((_tmp12B1="integral size mismatch; conversion supplied",_tag_dyneither(_tmp12B1,sizeof(char),44))),_tag_dyneither(_tmp12B0,sizeof(void*),0)));}
# 1918
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
# 1919
return 1;}
# 1921
return 0;}
# 1925
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 1926
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
# 1927
return 1;
# 1929
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
# 1930
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_typ)){
# 1931
const char*_tmp12B4;void*_tmp12B3;(_tmp12B3=0,Cyc_Tcutil_warn(e->loc,((_tmp12B4="integral size mismatch; conversion supplied",_tag_dyneither(_tmp12B4,sizeof(char),44))),_tag_dyneither(_tmp12B3,sizeof(void*),0)));}
# 1932
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
# 1933
return 1;}
# 1935
return 0;}
# 1940
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 1948
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 1950
t1=Cyc_Tcutil_compress(t1);
# 1951
t2=Cyc_Tcutil_compress(t2);{
# 1952
struct _tuple0 _tmp12B5;struct _tuple0 _tmp74E=(_tmp12B5.f1=t1,((_tmp12B5.f2=t2,_tmp12B5)));void*_tmp74F;struct Cyc_Absyn_PtrInfo _tmp751;void*_tmp752;struct Cyc_Absyn_PtrInfo _tmp754;void*_tmp755;struct Cyc_Absyn_ArrayInfo _tmp757;void*_tmp758;struct Cyc_Absyn_Tqual _tmp759;struct Cyc_Absyn_Exp*_tmp75A;union Cyc_Absyn_Constraint*_tmp75B;void*_tmp75C;struct Cyc_Absyn_ArrayInfo _tmp75E;void*_tmp75F;struct Cyc_Absyn_Tqual _tmp760;struct Cyc_Absyn_Exp*_tmp761;union Cyc_Absyn_Constraint*_tmp762;void*_tmp763;void*_tmp765;_LL3AF: _tmp74F=_tmp74E.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp750=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp74F;if(_tmp750->tag != 5)goto _LL3B1;else{_tmp751=_tmp750->f1;}};_tmp752=_tmp74E.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp753=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp752;if(_tmp753->tag != 5)goto _LL3B1;else{_tmp754=_tmp753->f1;}};_LL3B0: {
# 1954
int okay=1;
# 1956
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp751.ptr_atts).nullable,(_tmp754.ptr_atts).nullable))
# 1959
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp751.ptr_atts).nullable);
# 1961
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp751.ptr_atts).bounds,(_tmp754.ptr_atts).bounds)){
# 1964
struct _tuple0 _tmp12B6;struct _tuple0 _tmp768=(_tmp12B6.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp751.ptr_atts).bounds),((_tmp12B6.f2=
# 1965
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp754.ptr_atts).bounds),_tmp12B6)));void*_tmp769;void*_tmp76B;void*_tmp76D;void*_tmp76F;void*_tmp771;struct Cyc_Absyn_Exp*_tmp773;void*_tmp774;struct Cyc_Absyn_Exp*_tmp776;void*_tmp777;void*_tmp779;struct Cyc_Absyn_Exp*_tmp77B;_LL3B8: _tmp769=_tmp768.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp76A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp769;if(_tmp76A->tag != 1)goto _LL3BA;};_tmp76B=_tmp768.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp76C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp76B;if(_tmp76C->tag != 0)goto _LL3BA;};_LL3B9:
# 1966
 goto _LL3BB;_LL3BA: _tmp76D=_tmp768.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp76E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp76D;if(_tmp76E->tag != 0)goto _LL3BC;};_tmp76F=_tmp768.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp770=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp76F;if(_tmp770->tag != 0)goto _LL3BC;};_LL3BB:
# 1967
 okay=1;goto _LL3B7;_LL3BC: _tmp771=_tmp768.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp772=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp771;if(_tmp772->tag != 1)goto _LL3BE;else{_tmp773=_tmp772->f1;}};_tmp774=_tmp768.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp775=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp774;if(_tmp775->tag != 1)goto _LL3BE;else{_tmp776=_tmp775->f1;}};_LL3BD:
# 1969
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp776,_tmp773);
# 1973
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp754.ptr_atts).zero_term)){
# 1974
const char*_tmp12B9;void*_tmp12B8;(_tmp12B8=0,Cyc_Tcutil_warn(loc,((_tmp12B9="implicit cast to shorter array",_tag_dyneither(_tmp12B9,sizeof(char),31))),_tag_dyneither(_tmp12B8,sizeof(void*),0)));}
# 1975
goto _LL3B7;_LL3BE: _tmp777=_tmp768.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp778=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp777;if(_tmp778->tag != 0)goto _LL3B7;};_tmp779=_tmp768.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp77A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp779;if(_tmp77A->tag != 1)goto _LL3B7;else{_tmp77B=_tmp77A->f1;}};_LL3BF:
# 1978
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp751.ptr_atts).zero_term) && 
# 1979
Cyc_Tcutil_is_bound_one((_tmp754.ptr_atts).bounds))
# 1980
goto _LL3B7;
# 1981
okay=0;
# 1982
goto _LL3B7;_LL3B7:;}
# 1987
okay=okay  && (!(_tmp751.elt_tq).real_const  || (_tmp754.elt_tq).real_const);
# 1990
if(!Cyc_Tcutil_unify((_tmp751.ptr_atts).rgn,(_tmp754.ptr_atts).rgn)){
# 1991
if(Cyc_Tcenv_region_outlives(te,(_tmp751.ptr_atts).rgn,(_tmp754.ptr_atts).rgn)){
# 1992
if(Cyc_Tcutil_warn_region_coerce){
# 1993
const char*_tmp12BE;void*_tmp12BD[2];struct Cyc_String_pa_PrintArg_struct _tmp12BC;struct Cyc_String_pa_PrintArg_struct _tmp12BB;(_tmp12BB.tag=0,((_tmp12BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1995
Cyc_Absynpp_typ2string((_tmp754.ptr_atts).rgn)),((_tmp12BC.tag=0,((_tmp12BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1994
Cyc_Absynpp_typ2string((_tmp751.ptr_atts).rgn)),((_tmp12BD[0]=& _tmp12BC,((_tmp12BD[1]=& _tmp12BB,Cyc_Tcutil_warn(loc,((_tmp12BE="implicit cast form region %s to region %s",_tag_dyneither(_tmp12BE,sizeof(char),42))),_tag_dyneither(_tmp12BD,sizeof(void*),2)))))))))))));}}else{
# 1996
okay=0;}}
# 2000
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp751.ptr_atts).zero_term,(_tmp754.ptr_atts).zero_term) || 
# 2003
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp751.ptr_atts).zero_term) && (_tmp754.elt_tq).real_const);{
# 2011
int _tmp782=
# 2012
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp754.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2014
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp754.ptr_atts).zero_term);
# 2017
okay=okay  && (Cyc_Tcutil_unify(_tmp751.elt_typ,_tmp754.elt_typ) || 
# 2018
(_tmp782  && ((_tmp754.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp754.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp751.elt_typ,_tmp754.elt_typ));
# 2020
return okay;};}_LL3B1: _tmp755=_tmp74E.f1;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp756=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp755;if(_tmp756->tag != 8)goto _LL3B3;else{_tmp757=_tmp756->f1;_tmp758=_tmp757.elt_type;_tmp759=_tmp757.tq;_tmp75A=_tmp757.num_elts;_tmp75B=_tmp757.zero_term;}};_tmp75C=_tmp74E.f2;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp75D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp75C;if(_tmp75D->tag != 8)goto _LL3B3;else{_tmp75E=_tmp75D->f1;_tmp75F=_tmp75E.elt_type;_tmp760=_tmp75E.tq;_tmp761=_tmp75E.num_elts;_tmp762=_tmp75E.zero_term;}};_LL3B2: {
# 2024
int okay;
# 2026
okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp75B,_tmp762) && (
# 2027
(_tmp75A != 0  && _tmp761 != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp75A,(struct Cyc_Absyn_Exp*)_tmp761));
# 2029
return(okay  && Cyc_Tcutil_unify(_tmp758,_tmp75F)) && (!_tmp759.real_const  || _tmp760.real_const);}_LL3B3: _tmp763=_tmp74E.f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp764=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp763;if(_tmp764->tag != 19)goto _LL3B5;};_tmp765=_tmp74E.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp766=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp765;if(_tmp766->tag != 6)goto _LL3B5;};_LL3B4:
# 2031
 return 0;_LL3B5:;_LL3B6:
# 2033
 return Cyc_Tcutil_unify(t1,t2);_LL3AE:;};}
# 2037
int Cyc_Tcutil_is_pointer_type(void*t){
# 2038
void*_tmp783=Cyc_Tcutil_compress(t);_LL3C1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp784=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp783;if(_tmp784->tag != 5)goto _LL3C3;}_LL3C2:
# 2039
 return 1;_LL3C3:;_LL3C4:
# 2040
 return 0;_LL3C0:;}
# 2043
void*Cyc_Tcutil_pointer_elt_type(void*t){
# 2044
void*_tmp785=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp787;void*_tmp788;_LL3C6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp786=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp785;if(_tmp786->tag != 5)goto _LL3C8;else{_tmp787=_tmp786->f1;_tmp788=_tmp787.elt_typ;}}_LL3C7:
# 2045
 return _tmp788;_LL3C8:;_LL3C9: {
# 2046
const char*_tmp12C1;void*_tmp12C0;(_tmp12C0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12C1="pointer_elt_type",_tag_dyneither(_tmp12C1,sizeof(char),17))),_tag_dyneither(_tmp12C0,sizeof(void*),0)));}_LL3C5:;}
# 2049
void*Cyc_Tcutil_pointer_region(void*t){
# 2050
void*_tmp78B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp78D;struct Cyc_Absyn_PtrAtts _tmp78E;struct Cyc_Absyn_PtrAtts*_tmp78F;_LL3CB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp78C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp78B;if(_tmp78C->tag != 5)goto _LL3CD;else{_tmp78D=_tmp78C->f1;_tmp78E=_tmp78D.ptr_atts;_tmp78F=(struct Cyc_Absyn_PtrAtts*)&(_tmp78C->f1).ptr_atts;}}_LL3CC:
# 2051
 return _tmp78F->rgn;_LL3CD:;_LL3CE: {
# 2052
const char*_tmp12C4;void*_tmp12C3;(_tmp12C3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12C4="pointer_elt_type",_tag_dyneither(_tmp12C4,sizeof(char),17))),_tag_dyneither(_tmp12C3,sizeof(void*),0)));}_LL3CA:;}
# 2056
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
# 2057
void*_tmp792=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp794;struct Cyc_Absyn_PtrAtts _tmp795;void*_tmp796;_LL3D0: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp793=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp792;if(_tmp793->tag != 5)goto _LL3D2;else{_tmp794=_tmp793->f1;_tmp795=_tmp794.ptr_atts;_tmp796=_tmp795.rgn;}}_LL3D1:
# 2059
*rgn=_tmp796;
# 2060
return 1;_LL3D2:;_LL3D3:
# 2061
 return 0;_LL3CF:;}
# 2067
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr){
# 2068
void*_tmp797=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp799;struct Cyc_Absyn_PtrAtts _tmp79A;union Cyc_Absyn_Constraint*_tmp79B;_LL3D5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp798=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp797;if(_tmp798->tag != 5)goto _LL3D7;else{_tmp799=_tmp798->f1;_tmp79A=_tmp799.ptr_atts;_tmp79B=_tmp79A.bounds;}}_LL3D6:
# 2070
*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp79B)== (void*)& Cyc_Absyn_DynEither_b_val;
# 2071
return 1;_LL3D7:;_LL3D8:
# 2073
 return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;_LL3D4:;}
# 2078
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
# 2079
void*_tmp79C=e->r;union Cyc_Absyn_Cnst _tmp79E;struct _tuple7 _tmp79F;int _tmp7A0;union Cyc_Absyn_Cnst _tmp7A2;struct _tuple5 _tmp7A3;char _tmp7A4;union Cyc_Absyn_Cnst _tmp7A6;struct _tuple6 _tmp7A7;short _tmp7A8;union Cyc_Absyn_Cnst _tmp7AA;struct _tuple8 _tmp7AB;long long _tmp7AC;union Cyc_Absyn_Cnst _tmp7AE;struct _dyneither_ptr _tmp7AF;void*_tmp7B1;struct Cyc_Absyn_Exp*_tmp7B2;_LL3DA: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp79D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp79C;if(_tmp79D->tag != 0)goto _LL3DC;else{_tmp79E=_tmp79D->f1;if((_tmp79E.Int_c).tag != 5)goto _LL3DC;_tmp79F=(struct _tuple7)(_tmp79E.Int_c).val;_tmp7A0=_tmp79F.f2;if(_tmp7A0 != 0)goto _LL3DC;}}_LL3DB:
# 2080
 goto _LL3DD;_LL3DC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7A1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp79C;if(_tmp7A1->tag != 0)goto _LL3DE;else{_tmp7A2=_tmp7A1->f1;if((_tmp7A2.Char_c).tag != 2)goto _LL3DE;_tmp7A3=(struct _tuple5)(_tmp7A2.Char_c).val;_tmp7A4=_tmp7A3.f2;if(_tmp7A4 != 0)goto _LL3DE;}}_LL3DD:
# 2081
 goto _LL3DF;_LL3DE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7A5=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp79C;if(_tmp7A5->tag != 0)goto _LL3E0;else{_tmp7A6=_tmp7A5->f1;if((_tmp7A6.Short_c).tag != 4)goto _LL3E0;_tmp7A7=(struct _tuple6)(_tmp7A6.Short_c).val;_tmp7A8=_tmp7A7.f2;if(_tmp7A8 != 0)goto _LL3E0;}}_LL3DF:
# 2082
 goto _LL3E1;_LL3E0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7A9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp79C;if(_tmp7A9->tag != 0)goto _LL3E2;else{_tmp7AA=_tmp7A9->f1;if((_tmp7AA.LongLong_c).tag != 6)goto _LL3E2;_tmp7AB=(struct _tuple8)(_tmp7AA.LongLong_c).val;_tmp7AC=_tmp7AB.f2;if(_tmp7AC != 0)goto _LL3E2;}}_LL3E1:
# 2084
 return 1;_LL3E2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7AD=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp79C;if(_tmp7AD->tag != 0)goto _LL3E4;else{_tmp7AE=_tmp7AD->f1;if((_tmp7AE.Wchar_c).tag != 3)goto _LL3E4;_tmp7AF=(struct _dyneither_ptr)(_tmp7AE.Wchar_c).val;}}_LL3E3: {
# 2086
unsigned long _tmp7B3=Cyc_strlen((struct _dyneither_ptr)_tmp7AF);
# 2087
int i=0;
# 2088
if(_tmp7B3 >= 2  && *((const char*)_check_dyneither_subscript(_tmp7AF,sizeof(char),0))== '\\'){
# 2089
if(*((const char*)_check_dyneither_subscript(_tmp7AF,sizeof(char),1))== '0')i=2;else{
# 2090
if((*((const char*)_check_dyneither_subscript(_tmp7AF,sizeof(char),1))== 'x'  && _tmp7B3 >= 3) && *((const char*)_check_dyneither_subscript(_tmp7AF,sizeof(char),2))== '0')i=3;else{
# 2091
return 0;}}
# 2092
for(0;i < _tmp7B3;++ i){
# 2093
if(*((const char*)_check_dyneither_subscript(_tmp7AF,sizeof(char),i))!= '0')return 0;}
# 2094
return 1;}else{
# 2096
return 0;}}_LL3E4: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp7B0=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp79C;if(_tmp7B0->tag != 13)goto _LL3E6;else{_tmp7B1=(void*)_tmp7B0->f1;_tmp7B2=_tmp7B0->f2;}}_LL3E5:
# 2097
 return Cyc_Tcutil_is_zero(_tmp7B2) && Cyc_Tcutil_admits_zero(_tmp7B1);_LL3E6:;_LL3E7:
# 2098
 return 0;_LL3D9:;}
# 2102
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
# 2103
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
# 2104
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
# 2105
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
# 2106
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
# 2107
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
# 2109
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
# 2110
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
# 2111
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
# 2112
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2114
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
# 2115
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
# 2116
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
# 2117
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2119
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
# 2120
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
# 2121
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
# 2122
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
# 2123
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
# 2124
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
# 2126
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
# 2127
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
# 2128
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
# 2129
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2131
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
# 2132
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
# 2133
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
# 2134
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2136
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
# 2137
struct Cyc_Absyn_Kind _tmp7B5;enum Cyc_Absyn_KindQual _tmp7B6;enum Cyc_Absyn_AliasQual _tmp7B7;struct Cyc_Absyn_Kind*_tmp7B4=ka;_tmp7B5=*_tmp7B4;_tmp7B6=_tmp7B5.kind;_tmp7B7=_tmp7B5.aliasqual;
# 2138
switch(_tmp7B7){case Cyc_Absyn_Aliasable: _LL3E8:
# 2140
 switch(_tmp7B6){case Cyc_Absyn_AnyKind: _LL3EA:
# 2141
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL3EB:
# 2142
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL3EC:
# 2143
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3ED:
# 2144
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3EE:
# 2145
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3EF:
# 2146
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_iko;}case Cyc_Absyn_Unique: _LL3E9:
# 2149
 switch(_tmp7B6){case Cyc_Absyn_AnyKind: _LL3F2:
# 2150
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL3F3:
# 2151
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3F4:
# 2152
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3F5:
# 2153
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_urko;default: _LL3F6:
# 2154
 break;}
# 2156
break;case Cyc_Absyn_Top: _LL3F1:
# 2158
 switch(_tmp7B6){case Cyc_Absyn_AnyKind: _LL3F9:
# 2159
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL3FA:
# 2160
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL3FB:
# 2161
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL3FC:
# 2162
 return(struct Cyc_Core_Opt*)& Cyc_Tcutil_trko;default: _LL3FD:
# 2163
 break;}
# 2165
break;}{
# 2167
const char*_tmp12C8;void*_tmp12C7[1];struct Cyc_String_pa_PrintArg_struct _tmp12C6;(_tmp12C6.tag=0,((_tmp12C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp12C7[0]=& _tmp12C6,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12C8="kind_to_opt: bad kind %s\n",_tag_dyneither(_tmp12C8,sizeof(char),26))),_tag_dyneither(_tmp12C7,sizeof(void*),1)))))));};}
# 2170
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
# 2171
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0,& Cyc_Tcutil_ak};
# 2172
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0,& Cyc_Tcutil_mk};
# 2173
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0,& Cyc_Tcutil_bk};
# 2174
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0,& Cyc_Tcutil_rk};
# 2175
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0,& Cyc_Tcutil_ek};
# 2176
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0,& Cyc_Tcutil_ik};
# 2178
static void*ab=(void*)& ab_v;
# 2179
static void*mb=(void*)& mb_v;
# 2180
static void*bb=(void*)& bb_v;
# 2181
static void*rb=(void*)& rb_v;
# 2182
static void*eb=(void*)& eb_v;
# 2183
static void*ib=(void*)& ib_v;
# 2185
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0,& Cyc_Tcutil_tak};
# 2186
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0,& Cyc_Tcutil_tmk};
# 2187
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0,& Cyc_Tcutil_tbk};
# 2188
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0,& Cyc_Tcutil_trk};
# 2190
static void*tab=(void*)& tab_v;
# 2191
static void*tmb=(void*)& tmb_v;
# 2192
static void*tbb=(void*)& tbb_v;
# 2193
static void*trb=(void*)& trb_v;
# 2195
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0,& Cyc_Tcutil_uak};
# 2196
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0,& Cyc_Tcutil_umk};
# 2197
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0,& Cyc_Tcutil_ubk};
# 2198
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0,& Cyc_Tcutil_urk};
# 2200
static void*uab=(void*)& uab_v;
# 2201
static void*umb=(void*)& umb_v;
# 2202
static void*ubb=(void*)& ubb_v;
# 2203
static void*urb=(void*)& urb_v;
# 2205
struct Cyc_Absyn_Kind _tmp7BC;enum Cyc_Absyn_KindQual _tmp7BD;enum Cyc_Absyn_AliasQual _tmp7BE;struct Cyc_Absyn_Kind*_tmp7BB=ka;_tmp7BC=*_tmp7BB;_tmp7BD=_tmp7BC.kind;_tmp7BE=_tmp7BC.aliasqual;
# 2206
switch(_tmp7BE){case Cyc_Absyn_Aliasable: _LL3FF:
# 2208
 switch(_tmp7BD){case Cyc_Absyn_AnyKind: _LL401:
# 2209
 return& ab;case Cyc_Absyn_MemKind: _LL402:
# 2210
 return& mb;case Cyc_Absyn_BoxKind: _LL403:
# 2211
 return& bb;case Cyc_Absyn_RgnKind: _LL404:
# 2212
 return& rb;case Cyc_Absyn_EffKind: _LL405:
# 2213
 return& eb;case Cyc_Absyn_IntKind: _LL406:
# 2214
 return& ib;}case Cyc_Absyn_Unique: _LL400:
# 2217
 switch(_tmp7BD){case Cyc_Absyn_AnyKind: _LL409:
# 2218
 return& uab;case Cyc_Absyn_MemKind: _LL40A:
# 2219
 return& umb;case Cyc_Absyn_BoxKind: _LL40B:
# 2220
 return& ubb;case Cyc_Absyn_RgnKind: _LL40C:
# 2221
 return& urb;default: _LL40D:
# 2222
 break;}
# 2224
break;case Cyc_Absyn_Top: _LL408:
# 2226
 switch(_tmp7BD){case Cyc_Absyn_AnyKind: _LL410:
# 2227
 return& tab;case Cyc_Absyn_MemKind: _LL411:
# 2228
 return& tmb;case Cyc_Absyn_BoxKind: _LL412:
# 2229
 return& tbb;case Cyc_Absyn_RgnKind: _LL413:
# 2230
 return& trb;default: _LL414:
# 2231
 break;}
# 2233
break;}{
# 2235
const char*_tmp12CC;void*_tmp12CB[1];struct Cyc_String_pa_PrintArg_struct _tmp12CA;(_tmp12CA.tag=0,((_tmp12CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp12CB[0]=& _tmp12CA,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12CC="kind_to_b: bad kind %s\n",_tag_dyneither(_tmp12CC,sizeof(char),24))),_tag_dyneither(_tmp12CB,sizeof(void*),1)))))));};}
# 2238
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
# 2239
return*Cyc_Tcutil_kind_to_b(k);}
# 2241
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2243
struct Cyc_Core_Opt*_tmp12CD;return(_tmp12CD=_cycalloc(sizeof(*_tmp12CD)),((_tmp12CD->v=Cyc_Tcutil_kind_to_bound(k),_tmp12CD)));}
# 2248
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
# 2249
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
# 2250
{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp12D0;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp12CF;e1->r=(void*)((_tmp12CF=_cycalloc(sizeof(*_tmp12CF)),((_tmp12CF[0]=((_tmp12D0.tag=0,((_tmp12D0.f1=Cyc_Absyn_Null_c,_tmp12D0)))),_tmp12CF))));}{
# 2251
struct Cyc_Core_Opt*_tmp7D3=Cyc_Tcenv_lookup_opt_type_vars(te);
# 2252
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp12DA;struct Cyc_Absyn_PtrAtts _tmp12D9;struct Cyc_Absyn_PtrInfo _tmp12D8;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp12D7;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp7D4=(_tmp12D7=_cycalloc(sizeof(*_tmp12D7)),((_tmp12D7[0]=((_tmp12DA.tag=5,((_tmp12DA.f1=((_tmp12D8.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ako,_tmp7D3),((_tmp12D8.elt_tq=
# 2253
Cyc_Absyn_empty_tqual(0),((_tmp12D8.ptr_atts=(
# 2254
(_tmp12D9.rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,_tmp7D3),((_tmp12D9.nullable=Cyc_Absyn_true_conref,((_tmp12D9.bounds=
# 2256
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp12D9.zero_term=
# 2257
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp12D9.ptrloc=0,_tmp12D9)))))))))),_tmp12D8)))))),_tmp12DA)))),_tmp12D7)));
# 2258
e1->topt=(void*)_tmp7D4;{
# 2259
int bogus=0;
# 2260
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
# 2261
if(bogus != 0){
# 2262
const char*_tmp12DF;void*_tmp12DE[2];struct Cyc_String_pa_PrintArg_struct _tmp12DD;struct Cyc_String_pa_PrintArg_struct _tmp12DC;(_tmp12DC.tag=0,((_tmp12DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2263
Cyc_Position_string_of_segment(e1->loc)),((_tmp12DD.tag=0,((_tmp12DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp12DE[0]=& _tmp12DD,((_tmp12DE[1]=& _tmp12DC,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12DF="zero_to_null resulted in an alias coercion on %s at %s\n",_tag_dyneither(_tmp12DF,sizeof(char),56))),_tag_dyneither(_tmp12DE,sizeof(void*),2)))))))))))));}
# 2264
return retv;};};}
# 2266
return 0;}
# 2269
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
# 2270
switch(c){case Cyc_Absyn_Unknown_coercion: _LL416: {
# 2271
const char*_tmp12E0;return(_tmp12E0="unknown",_tag_dyneither(_tmp12E0,sizeof(char),8));}case Cyc_Absyn_No_coercion: _LL417: {
# 2272
const char*_tmp12E1;return(_tmp12E1="no coercion",_tag_dyneither(_tmp12E1,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL418: {
# 2273
const char*_tmp12E2;return(_tmp12E2="null check",_tag_dyneither(_tmp12E2,sizeof(char),11));}case Cyc_Absyn_Other_coercion: _LL419: {
# 2274
const char*_tmp12E3;return(_tmp12E3="other coercion",_tag_dyneither(_tmp12E3,sizeof(char),15));}}}
# 2278
int Cyc_Tcutil_warn_alias_coerce=0;
# 2284
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
# 2285
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2288
static int counter=0;
# 2289
struct _dyneither_ptr*_tmp12F0;const char*_tmp12EF;void*_tmp12EE[1];struct Cyc_Int_pa_PrintArg_struct _tmp12ED;struct _tuple2*_tmp12EC;struct _tuple2*v=(_tmp12EC=_cycalloc(sizeof(*_tmp12EC)),((_tmp12EC->f1=Cyc_Absyn_Loc_n,((_tmp12EC->f2=((_tmp12F0=_cycalloc(sizeof(*_tmp12F0)),((_tmp12F0[0]=(struct _dyneither_ptr)((_tmp12ED.tag=1,((_tmp12ED.f1=(unsigned long)counter ++,((_tmp12EE[0]=& _tmp12ED,Cyc_aprintf(((_tmp12EF="__aliasvar%d",_tag_dyneither(_tmp12EF,sizeof(char),13))),_tag_dyneither(_tmp12EE,sizeof(void*),1)))))))),_tmp12F0)))),_tmp12EC)))));
# 2290
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,(void*)& Cyc_Absyn_VoidType_val,0);
# 2291
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp12F3;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp12F2;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmp12F2=_cycalloc(sizeof(*_tmp12F2)),((_tmp12F2[0]=((_tmp12F3.tag=4,((_tmp12F3.f1=vd,_tmp12F3)))),_tmp12F2)))),e->loc);
# 2294
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2295
{void*_tmp7E1=Cyc_Tcutil_compress(e_typ);struct Cyc_Absyn_PtrInfo _tmp7E3;struct Cyc_Absyn_PtrAtts _tmp7E4;void*_tmp7E5;void**_tmp7E6;_LL41C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp7E2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7E1;if(_tmp7E2->tag != 5)goto _LL41E;else{_tmp7E3=_tmp7E2->f1;_tmp7E4=_tmp7E3.ptr_atts;_tmp7E5=_tmp7E4.rgn;_tmp7E6=(void**)&((_tmp7E2->f1).ptr_atts).rgn;}}_LL41D:
# 2297
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp12F6;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp12F5;*_tmp7E6=(void*)((_tmp12F5=_cycalloc(sizeof(*_tmp12F5)),((_tmp12F5[0]=((_tmp12F6.tag=2,((_tmp12F6.f1=tv,_tmp12F6)))),_tmp12F5))));}goto _LL41B;_LL41E:;_LL41F:
# 2298
 goto _LL41B;_LL41B:;}
# 2300
e->topt=0;
# 2301
e=Cyc_Absyn_cast_exp(e_typ,e,0,Cyc_Absyn_Unknown_coercion,e->loc);{
# 2304
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(e,tv,vd,e->loc);
# 2306
struct _tuple15 _tmp12F7;return(_tmp12F7.f1=d,((_tmp12F7.f2=ve,_tmp12F7)));};}
# 2311
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2314
struct _RegionHandle _tmp7F2=_new_region("r");struct _RegionHandle*r=& _tmp7F2;_push_region(r);
# 2315
if((Cyc_Tcutil_is_noalias_path(r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ)) && 
# 2316
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2319
void*_tmp7F3=Cyc_Tcutil_compress(wants_typ);struct Cyc_Absyn_PtrInfo _tmp7F5;struct Cyc_Absyn_PtrAtts _tmp7F6;void*_tmp7F7;_LL421: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp7F4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7F3;if(_tmp7F4->tag != 5)goto _LL423;else{_tmp7F5=_tmp7F4->f1;_tmp7F6=_tmp7F5.ptr_atts;_tmp7F7=_tmp7F6.rgn;}}_LL422: {
# 2321
void*_tmp7F9=Cyc_Tcutil_compress(_tmp7F7);_LL428: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp7FA=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp7F9;if(_tmp7FA->tag != 20)goto _LL42A;}_LL429: {
# 2322
int _tmp7FB=0;_npop_handler(0);return _tmp7FB;}_LL42A:;_LL42B: {
# 2324
struct Cyc_Absyn_Kind*_tmp7FC=Cyc_Tcutil_typ_kind(_tmp7F7);
# 2325
int _tmp7FD=_tmp7FC->kind == Cyc_Absyn_RgnKind  && _tmp7FC->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp7FD;}_LL427:;}_LL423: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp7F8=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp7F3;if(_tmp7F8->tag != 17)goto _LL425;}_LL424: {
# 2328
const char*_tmp12FC;void*_tmp12FB[2];struct Cyc_String_pa_PrintArg_struct _tmp12FA;struct Cyc_String_pa_PrintArg_struct _tmp12F9;(_tmp12F9.tag=0,((_tmp12F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2329
Cyc_Position_string_of_segment(loc)),((_tmp12FA.tag=0,((_tmp12FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ))),((_tmp12FB[0]=& _tmp12FA,((_tmp12FB[1]=& _tmp12F9,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12FC="got typedef %s in can_insert_alias at %s\n",_tag_dyneither(_tmp12FC,sizeof(char),42))),_tag_dyneither(_tmp12FB,sizeof(void*),2)))))))))))));}_LL425:;_LL426: {
# 2330
int _tmp802=0;_npop_handler(0);return _tmp802;}_LL420:;}{
# 2333
int _tmp803=0;_npop_handler(0);return _tmp803;};
# 2315
;_pop_region(r);}
# 2337
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
# 2338
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 2339
enum Cyc_Absyn_Coercion c;
# 2340
int do_alias_coercion=0;
# 2342
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2344
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2346
if(Cyc_Tcutil_will_lose_precision(t1,t2)){
# 2347
const char*_tmp1301;void*_tmp1300[2];struct Cyc_String_pa_PrintArg_struct _tmp12FF;struct Cyc_String_pa_PrintArg_struct _tmp12FE;(_tmp12FE.tag=0,((_tmp12FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2348
Cyc_Absynpp_typ2string(t2)),((_tmp12FF.tag=0,((_tmp12FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp1300[0]=& _tmp12FF,((_tmp1300[1]=& _tmp12FE,Cyc_Tcutil_warn(e->loc,((_tmp1301="integral size mismatch; %s -> %s conversion supplied",_tag_dyneither(_tmp1301,sizeof(char),53))),_tag_dyneither(_tmp1300,sizeof(void*),2)))))))))))));}
# 2349
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
# 2350
return 1;}else{
# 2355
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
# 2356
if(Cyc_Tcutil_warn_alias_coerce){
# 2357
const char*_tmp1307;void*_tmp1306[3];struct Cyc_String_pa_PrintArg_struct _tmp1305;struct Cyc_String_pa_PrintArg_struct _tmp1304;struct Cyc_String_pa_PrintArg_struct _tmp1303;(_tmp1303.tag=0,((_tmp1303.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2358
Cyc_Absynpp_typ2string(t2)),((_tmp1304.tag=0,((_tmp1304.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp1305.tag=0,((_tmp1305.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp1306[0]=& _tmp1305,((_tmp1306[1]=& _tmp1304,((_tmp1306[2]=& _tmp1303,Cyc_Tcutil_warn(e->loc,((_tmp1307="implicit alias coercion for %s:%s to %s",_tag_dyneither(_tmp1307,sizeof(char),40))),_tag_dyneither(_tmp1306,sizeof(void*),3)))))))))))))))))));}
# 2359
*alias_coercion=1;}
# 2362
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
# 2363
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
# 2364
return 1;}else{
# 2365
if(Cyc_Tcutil_zero_to_null(te,t2,e))
# 2366
return 1;else{
# 2367
if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){
# 2369
Cyc_Tcutil_unchecked_cast(te,e,t2,c);
# 2371
if(c != Cyc_Absyn_NonNull_to_Null){
# 2372
const char*_tmp130C;void*_tmp130B[2];struct Cyc_String_pa_PrintArg_struct _tmp130A;struct Cyc_String_pa_PrintArg_struct _tmp1309;(_tmp1309.tag=0,((_tmp1309.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2373
Cyc_Absynpp_typ2string(t2)),((_tmp130A.tag=0,((_tmp130A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp130B[0]=& _tmp130A,((_tmp130B[1]=& _tmp1309,Cyc_Tcutil_warn(e->loc,((_tmp130C="implicit cast from %s to %s",_tag_dyneither(_tmp130C,sizeof(char),28))),_tag_dyneither(_tmp130B,sizeof(void*),2)))))))))))));}
# 2374
return 1;}else{
# 2376
return 0;}}}}}
# 2383
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2386
int bogus=0;
# 2387
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2390
int Cyc_Tcutil_coerceable(void*t){
# 2391
void*_tmp811=Cyc_Tcutil_compress(t);_LL42D: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp812=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp811;if(_tmp812->tag != 6)goto _LL42F;}_LL42E:
# 2392
 goto _LL430;_LL42F: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp813=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp811;if(_tmp813->tag != 7)goto _LL431;}_LL430:
# 2393
 return 1;_LL431:;_LL432:
# 2394
 return 0;_LL42C:;}
# 2408 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2412
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2415
struct Cyc_List_List*_tmp815;struct _RegionHandle*_tmp816;struct Cyc_Tcenv_Tenv*_tmp817;int _tmp818;struct _tuple23 _tmp814=*env;_tmp815=_tmp814.f1;_tmp816=_tmp814.f2;_tmp817=_tmp814.f3;_tmp818=_tmp814.f4;{
# 2417
void*_tmp819=_tmp815 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp816,_tmp815,x->type);
# 2418
struct Cyc_List_List*_tmp81A=Cyc_Tcutil_flatten_typ(_tmp816,_tmp818,_tmp817,_tmp819);
# 2419
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp81A)== 1){
# 2420
struct _tuple12*_tmp130F;struct Cyc_List_List*_tmp130E;return(_tmp130E=_region_malloc(_tmp816,sizeof(*_tmp130E)),((_tmp130E->hd=((_tmp130F=_region_malloc(_tmp816,sizeof(*_tmp130F)),((_tmp130F->f1=x->tq,((_tmp130F->f2=_tmp819,_tmp130F)))))),((_tmp130E->tl=0,_tmp130E)))));}else{
# 2421
return _tmp81A;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2423
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2425
struct _RegionHandle*_tmp81E;struct Cyc_Tcenv_Tenv*_tmp81F;int _tmp820;struct _tuple24 _tmp81D=*env;_tmp81E=_tmp81D.f1;_tmp81F=_tmp81D.f2;_tmp820=_tmp81D.f3;{
# 2426
struct Cyc_Absyn_Tqual _tmp822;void*_tmp823;struct _tuple12 _tmp821=*x;_tmp822=_tmp821.f1;_tmp823=_tmp821.f2;{
# 2427
struct Cyc_List_List*_tmp824=Cyc_Tcutil_flatten_typ(_tmp81E,_tmp820,_tmp81F,_tmp823);
# 2428
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp824)== 1){
# 2429
struct _tuple12*_tmp1312;struct Cyc_List_List*_tmp1311;return(_tmp1311=_region_malloc(_tmp81E,sizeof(*_tmp1311)),((_tmp1311->hd=((_tmp1312=_region_malloc(_tmp81E,sizeof(*_tmp1312)),((_tmp1312->f1=_tmp822,((_tmp1312->f2=_tmp823,_tmp1312)))))),((_tmp1311->tl=0,_tmp1311)))));}else{
# 2430
return _tmp824;}};};}
# 2432
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2436
if(flatten){
# 2437
t1=Cyc_Tcutil_compress(t1);{
# 2438
void*_tmp827=t1;struct Cyc_List_List*_tmp82A;struct Cyc_Absyn_AggrInfo _tmp82C;union Cyc_Absyn_AggrInfoU _tmp82D;struct Cyc_Absyn_Aggrdecl**_tmp82E;struct Cyc_Absyn_Aggrdecl*_tmp82F;struct Cyc_List_List*_tmp830;enum Cyc_Absyn_AggrKind _tmp832;struct Cyc_List_List*_tmp833;_LL434: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp828=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp827;if(_tmp828->tag != 0)goto _LL436;}_LL435:
# 2439
 return 0;_LL436: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp829=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp827;if(_tmp829->tag != 10)goto _LL438;else{_tmp82A=_tmp829->f1;}}_LL437: {
# 2441
struct _tuple24 _tmp1313;struct _tuple24 _tmp834=(_tmp1313.f1=r,((_tmp1313.f2=te,((_tmp1313.f3=flatten,_tmp1313)))));
# 2443
struct Cyc_List_List*_tmp835=_tmp82A;struct Cyc_List_List _tmp836;struct _tuple12*_tmp837;struct Cyc_List_List*_tmp838;_LL43F: if(_tmp835 != 0)goto _LL441;_LL440:
# 2444
 return 0;_LL441: if(_tmp835 == 0)goto _LL43E;_tmp836=*_tmp835;_tmp837=(struct _tuple12*)_tmp836.hd;_tmp838=_tmp836.tl;_LL442: {
# 2446
struct Cyc_List_List*_tmp839=Cyc_Tcutil_rcopy_tqt(& _tmp834,_tmp837);
# 2447
_tmp834.f3=0;{
# 2448
struct Cyc_List_List*_tmp83A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp834,_tmp82A);
# 2449
struct Cyc_List_List*_tmp1314;struct Cyc_List_List*_tmp83B=(_tmp1314=_region_malloc(r,sizeof(*_tmp1314)),((_tmp1314->hd=_tmp839,((_tmp1314->tl=_tmp83A,_tmp1314)))));
# 2450
struct Cyc_List_List*_tmp1315;return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp1315=_region_malloc(r,sizeof(*_tmp1315)),((_tmp1315->hd=_tmp839,((_tmp1315->tl=_tmp83A,_tmp1315)))))));};}_LL43E:;}_LL438: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp82B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp827;if(_tmp82B->tag != 11)goto _LL43A;else{_tmp82C=_tmp82B->f1;_tmp82D=_tmp82C.aggr_info;if((_tmp82D.KnownAggr).tag != 2)goto _LL43A;_tmp82E=(struct Cyc_Absyn_Aggrdecl**)(_tmp82D.KnownAggr).val;_tmp82F=*_tmp82E;_tmp830=_tmp82C.targs;}}_LL439:
# 2454
 if(((_tmp82F->kind == Cyc_Absyn_UnionA  || _tmp82F->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp82F->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp82F->impl))->rgn_po != 0){
# 2456
struct _tuple12*_tmp1318;struct Cyc_List_List*_tmp1317;return(_tmp1317=_region_malloc(r,sizeof(*_tmp1317)),((_tmp1317->hd=((_tmp1318=_region_malloc(r,sizeof(*_tmp1318)),((_tmp1318->f1=Cyc_Absyn_empty_tqual(0),((_tmp1318->f2=t1,_tmp1318)))))),((_tmp1317->tl=0,_tmp1317)))));}{
# 2457
struct Cyc_List_List*_tmp841=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp82F->tvs,_tmp830);
# 2458
struct _tuple23 _tmp1319;struct _tuple23 env=(_tmp1319.f1=_tmp841,((_tmp1319.f2=r,((_tmp1319.f3=te,((_tmp1319.f4=flatten,_tmp1319)))))));
# 2459
struct Cyc_List_List*_tmp842=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp82F->impl))->fields;struct Cyc_List_List _tmp843;struct Cyc_Absyn_Aggrfield*_tmp844;struct Cyc_List_List*_tmp845;_LL444: if(_tmp842 != 0)goto _LL446;_LL445:
# 2460
 return 0;_LL446: if(_tmp842 == 0)goto _LL443;_tmp843=*_tmp842;_tmp844=(struct Cyc_Absyn_Aggrfield*)_tmp843.hd;_tmp845=_tmp843.tl;_LL447: {
# 2462
struct Cyc_List_List*_tmp846=Cyc_Tcutil_flatten_typ_f(& env,_tmp844);
# 2463
env.f4=0;{
# 2464
struct Cyc_List_List*_tmp847=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp845);
# 2465
struct Cyc_List_List*_tmp131A;struct Cyc_List_List*_tmp848=(_tmp131A=_region_malloc(r,sizeof(*_tmp131A)),((_tmp131A->hd=_tmp846,((_tmp131A->tl=_tmp847,_tmp131A)))));
# 2466
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp848);};}_LL443:;};_LL43A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp831=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp827;if(_tmp831->tag != 12)goto _LL43C;else{_tmp832=_tmp831->f1;if(_tmp832 != Cyc_Absyn_StructA)goto _LL43C;_tmp833=_tmp831->f2;}}_LL43B: {
# 2469
struct _tuple23 _tmp131B;struct _tuple23 env=(_tmp131B.f1=0,((_tmp131B.f2=r,((_tmp131B.f3=te,((_tmp131B.f4=flatten,_tmp131B)))))));
# 2470
struct Cyc_List_List*_tmp84B=_tmp833;struct Cyc_List_List _tmp84C;struct Cyc_Absyn_Aggrfield*_tmp84D;struct Cyc_List_List*_tmp84E;_LL449: if(_tmp84B != 0)goto _LL44B;_LL44A:
# 2471
 return 0;_LL44B: if(_tmp84B == 0)goto _LL448;_tmp84C=*_tmp84B;_tmp84D=(struct Cyc_Absyn_Aggrfield*)_tmp84C.hd;_tmp84E=_tmp84C.tl;_LL44C: {
# 2473
struct Cyc_List_List*_tmp84F=Cyc_Tcutil_flatten_typ_f(& env,_tmp84D);
# 2474
env.f4=0;{
# 2475
struct Cyc_List_List*_tmp850=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp84E);
# 2476
struct Cyc_List_List*_tmp131C;struct Cyc_List_List*_tmp851=(_tmp131C=_region_malloc(r,sizeof(*_tmp131C)),((_tmp131C->hd=_tmp84F,((_tmp131C->tl=_tmp850,_tmp131C)))));
# 2477
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp851);};}_LL448:;}_LL43C:;_LL43D:
# 2479
 goto _LL433;_LL433:;};}{
# 2482
struct _tuple12*_tmp131F;struct Cyc_List_List*_tmp131E;return(_tmp131E=_region_malloc(r,sizeof(*_tmp131E)),((_tmp131E->hd=((_tmp131F=_region_malloc(r,sizeof(*_tmp131F)),((_tmp131F->f1=Cyc_Absyn_empty_tqual(0),((_tmp131F->f2=t1,_tmp131F)))))),((_tmp131E->tl=0,_tmp131E)))));};}
# 2486
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
# 2487
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
# 2488
void*_tmp856=(void*)t->hd;_LL44E: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp857=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp856;if(_tmp857->tag != 22)goto _LL450;}_LL44F:
# 2489
 goto _LL451;_LL450: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp858=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp856;if(_tmp858->tag != 4)goto _LL452;}_LL451:
# 2490
 goto _LL453;_LL452: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp859=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp856;if(_tmp859->tag != 20)goto _LL454;}_LL453:
# 2492
 continue;_LL454:;_LL455:
# 2494
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;_LL44D:;}}
# 2497
for(0;a2 != 0;a2=a2->tl){
# 2498
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2500
return 1;}
# 2503
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
# 2504
struct _tuple0 _tmp1320;struct _tuple0 _tmp85B=(_tmp1320.f1=Cyc_Tcutil_compress(t1),((_tmp1320.f2=Cyc_Tcutil_compress(t2),_tmp1320)));void*_tmp85C;enum Cyc_Absyn_Size_of _tmp85E;void*_tmp85F;enum Cyc_Absyn_Size_of _tmp861;_LL457: _tmp85C=_tmp85B.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp85D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp85C;if(_tmp85D->tag != 6)goto _LL459;else{_tmp85E=_tmp85D->f2;}};_tmp85F=_tmp85B.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp860=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp85F;if(_tmp860->tag != 6)goto _LL459;else{_tmp861=_tmp860->f2;}};_LL458:
# 2506
 return(_tmp85E == _tmp861  || _tmp85E == Cyc_Absyn_Int_sz  && _tmp861 == Cyc_Absyn_Long_sz) || 
# 2507
_tmp85E == Cyc_Absyn_Long_sz  && _tmp861 == Cyc_Absyn_Int_sz;_LL459:;_LL45A:
# 2508
 return 0;_LL456:;}
# 2514
static int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2517
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2518
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
# 2519
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
# 2520
return 1;}}
# 2522
t1=Cyc_Tcutil_compress(t1);
# 2523
t2=Cyc_Tcutil_compress(t2);{
# 2524
struct _tuple0 _tmp1321;struct _tuple0 _tmp863=(_tmp1321.f1=t1,((_tmp1321.f2=t2,_tmp1321)));void*_tmp864;struct Cyc_Absyn_PtrInfo _tmp866;void*_tmp867;struct Cyc_Absyn_Tqual _tmp868;struct Cyc_Absyn_PtrAtts _tmp869;void*_tmp86A;union Cyc_Absyn_Constraint*_tmp86B;union Cyc_Absyn_Constraint*_tmp86C;union Cyc_Absyn_Constraint*_tmp86D;void*_tmp86E;struct Cyc_Absyn_PtrInfo _tmp870;void*_tmp871;struct Cyc_Absyn_Tqual _tmp872;struct Cyc_Absyn_PtrAtts _tmp873;void*_tmp874;union Cyc_Absyn_Constraint*_tmp875;union Cyc_Absyn_Constraint*_tmp876;union Cyc_Absyn_Constraint*_tmp877;void*_tmp878;struct Cyc_Absyn_DatatypeFieldInfo _tmp87A;union Cyc_Absyn_DatatypeFieldInfoU _tmp87B;struct _tuple3 _tmp87C;struct Cyc_Absyn_Datatypedecl*_tmp87D;struct Cyc_Absyn_Datatypefield*_tmp87E;struct Cyc_List_List*_tmp87F;void*_tmp880;struct Cyc_Absyn_DatatypeInfo _tmp882;union Cyc_Absyn_DatatypeInfoU _tmp883;struct Cyc_Absyn_Datatypedecl**_tmp884;struct Cyc_Absyn_Datatypedecl*_tmp885;struct Cyc_List_List*_tmp886;void*_tmp887;struct Cyc_Absyn_FnInfo _tmp889;void*_tmp88A;struct Cyc_Absyn_FnInfo _tmp88C;_LL45C: _tmp864=_tmp863.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp865=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp864;if(_tmp865->tag != 5)goto _LL45E;else{_tmp866=_tmp865->f1;_tmp867=_tmp866.elt_typ;_tmp868=_tmp866.elt_tq;_tmp869=_tmp866.ptr_atts;_tmp86A=_tmp869.rgn;_tmp86B=_tmp869.nullable;_tmp86C=_tmp869.bounds;_tmp86D=_tmp869.zero_term;}};_tmp86E=_tmp863.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp86F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp86E;if(_tmp86F->tag != 5)goto _LL45E;else{_tmp870=_tmp86F->f1;_tmp871=_tmp870.elt_typ;_tmp872=_tmp870.elt_tq;_tmp873=_tmp870.ptr_atts;_tmp874=_tmp873.rgn;_tmp875=_tmp873.nullable;_tmp876=_tmp873.bounds;_tmp877=_tmp873.zero_term;}};_LL45D:
# 2530
 if(_tmp868.real_const  && !_tmp872.real_const)
# 2531
return 0;
# 2533
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp86B,_tmp875) && 
# 2534
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp86B)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp875))
# 2535
return 0;
# 2537
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp86D,_tmp877) && !
# 2538
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp86D)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp877))
# 2539
return 0;
# 2541
if(!Cyc_Tcutil_unify(_tmp86A,_tmp874) && !Cyc_Tcenv_region_outlives(te,_tmp86A,_tmp874))
# 2542
return 0;
# 2544
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp86C,_tmp876)){
# 2545
struct _tuple0 _tmp1322;struct _tuple0 _tmp88E=(_tmp1322.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp86C),((_tmp1322.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp876),_tmp1322)));void*_tmp88F;void*_tmp891;void*_tmp893;struct Cyc_Absyn_Exp*_tmp895;void*_tmp896;struct Cyc_Absyn_Exp*_tmp898;_LL465: _tmp88F=_tmp88E.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp890=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp88F;if(_tmp890->tag != 1)goto _LL467;};_tmp891=_tmp88E.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp892=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp891;if(_tmp892->tag != 0)goto _LL467;};_LL466:
# 2546
 goto _LL464;_LL467: _tmp893=_tmp88E.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp894=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp893;if(_tmp894->tag != 1)goto _LL469;else{_tmp895=_tmp894->f1;}};_tmp896=_tmp88E.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp897=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp896;if(_tmp897->tag != 1)goto _LL469;else{_tmp898=_tmp897->f1;}};_LL468:
# 2548
 if(!Cyc_Evexp_lte_const_exp(_tmp898,_tmp895))
# 2549
return 0;
# 2550
goto _LL464;_LL469:;_LL46A:
# 2551
 return 0;_LL464:;}
# 2556
if(!_tmp872.real_const  && _tmp868.real_const){
# 2557
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp871)))
# 2558
return 0;}{
# 2562
int _tmp899=
# 2563
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp876,Cyc_Absyn_bounds_one_conref) && !
# 2564
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp877);
# 2568
struct _tuple0*_tmp1325;struct Cyc_List_List*_tmp1324;return(_tmp899  && Cyc_Tcutil_ptrsubtype(te,((_tmp1324=_cycalloc(sizeof(*_tmp1324)),((_tmp1324->hd=((_tmp1325=_cycalloc(sizeof(*_tmp1325)),((_tmp1325->f1=t1,((_tmp1325->f2=t2,_tmp1325)))))),((_tmp1324->tl=assume,_tmp1324)))))),_tmp867,_tmp871) || Cyc_Tcutil_unify(t1,t2)) || Cyc_Tcutil_isomorphic(t1,t2);};_LL45E: _tmp878=_tmp863.f1;{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp879=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp878;if(_tmp879->tag != 4)goto _LL460;else{_tmp87A=_tmp879->f1;_tmp87B=_tmp87A.field_info;if((_tmp87B.KnownDatatypefield).tag != 2)goto _LL460;_tmp87C=(struct _tuple3)(_tmp87B.KnownDatatypefield).val;_tmp87D=_tmp87C.f1;_tmp87E=_tmp87C.f2;_tmp87F=_tmp87A.targs;}};_tmp880=_tmp863.f2;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp881=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp880;if(_tmp881->tag != 3)goto _LL460;else{_tmp882=_tmp881->f1;_tmp883=_tmp882.datatype_info;if((_tmp883.KnownDatatype).tag != 2)goto _LL460;_tmp884=(struct Cyc_Absyn_Datatypedecl**)(_tmp883.KnownDatatype).val;_tmp885=*_tmp884;_tmp886=_tmp882.targs;}};_LL45F:
# 2574
 if(_tmp87D != _tmp885  && Cyc_Absyn_qvar_cmp(_tmp87D->name,_tmp885->name)!= 0)return 0;
# 2576
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp87F)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp886))return 0;
# 2577
for(0;_tmp87F != 0;(_tmp87F=_tmp87F->tl,_tmp886=_tmp886->tl)){
# 2578
if(!Cyc_Tcutil_unify((void*)_tmp87F->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp886))->hd))return 0;}
# 2579
return 1;_LL460: _tmp887=_tmp863.f1;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp888=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp887;if(_tmp888->tag != 9)goto _LL462;else{_tmp889=_tmp888->f1;}};_tmp88A=_tmp863.f2;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp88B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp88A;if(_tmp88B->tag != 9)goto _LL462;else{_tmp88C=_tmp88B->f1;}};_LL461:
# 2583
 if(_tmp889.tvars != 0  || _tmp88C.tvars != 0){
# 2584
struct Cyc_List_List*_tmp89C=_tmp889.tvars;
# 2585
struct Cyc_List_List*_tmp89D=_tmp88C.tvars;
# 2586
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp89C)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp89D))return 0;{
# 2587
struct _RegionHandle _tmp89E=_new_region("r");struct _RegionHandle*r=& _tmp89E;_push_region(r);
# 2588
{struct Cyc_List_List*inst=0;
# 2589
while(_tmp89C != 0){
# 2590
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp89C->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp89D))->hd)->kind)){int _tmp89F=0;_npop_handler(0);return _tmp89F;}
# 2591
{struct _tuple16*_tmp132F;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp132E;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp132D;struct Cyc_List_List*_tmp132C;inst=((_tmp132C=_region_malloc(r,sizeof(*_tmp132C)),((_tmp132C->hd=((_tmp132F=_region_malloc(r,sizeof(*_tmp132F)),((_tmp132F->f1=(struct Cyc_Absyn_Tvar*)_tmp89D->hd,((_tmp132F->f2=(void*)((_tmp132D=_cycalloc(sizeof(*_tmp132D)),((_tmp132D[0]=((_tmp132E.tag=2,((_tmp132E.f1=(struct Cyc_Absyn_Tvar*)_tmp89C->hd,_tmp132E)))),_tmp132D)))),_tmp132F)))))),((_tmp132C->tl=inst,_tmp132C))))));}
# 2592
_tmp89C=_tmp89C->tl;
# 2593
_tmp89D=_tmp89D->tl;}
# 2595
if(inst != 0){
# 2596
_tmp889.tvars=0;
# 2597
_tmp88C.tvars=0;{
# 2598
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1335;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1334;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1332;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1331;int _tmp8A8=Cyc_Tcutil_subtype(te,assume,(void*)((_tmp1331=_cycalloc(sizeof(*_tmp1331)),((_tmp1331[0]=((_tmp1332.tag=9,((_tmp1332.f1=_tmp889,_tmp1332)))),_tmp1331)))),(void*)((_tmp1334=_cycalloc(sizeof(*_tmp1334)),((_tmp1334[0]=((_tmp1335.tag=9,((_tmp1335.f1=_tmp88C,_tmp1335)))),_tmp1334)))));_npop_handler(0);return _tmp8A8;};}}
# 2588
;_pop_region(r);};}
# 2603
if(!Cyc_Tcutil_subtype(te,assume,_tmp889.ret_typ,_tmp88C.ret_typ))return 0;{
# 2604
struct Cyc_List_List*_tmp8A9=_tmp889.args;
# 2605
struct Cyc_List_List*_tmp8AA=_tmp88C.args;
# 2608
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8A9)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8AA))return 0;
# 2610
for(0;_tmp8A9 != 0;(_tmp8A9=_tmp8A9->tl,_tmp8AA=_tmp8AA->tl)){
# 2611
struct Cyc_Absyn_Tqual _tmp8AC;void*_tmp8AD;struct _tuple10 _tmp8AB=*((struct _tuple10*)_tmp8A9->hd);_tmp8AC=_tmp8AB.f2;_tmp8AD=_tmp8AB.f3;{
# 2612
struct Cyc_Absyn_Tqual _tmp8AF;void*_tmp8B0;struct _tuple10 _tmp8AE=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp8AA))->hd);_tmp8AF=_tmp8AE.f2;_tmp8B0=_tmp8AE.f3;
# 2614
if(_tmp8AF.real_const  && !_tmp8AC.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp8B0,_tmp8AD))
# 2615
return 0;};}
# 2618
if(_tmp889.c_varargs != _tmp88C.c_varargs)return 0;
# 2619
if(_tmp889.cyc_varargs != 0  && _tmp88C.cyc_varargs != 0){
# 2620
struct Cyc_Absyn_VarargInfo _tmp8B1=*_tmp889.cyc_varargs;
# 2621
struct Cyc_Absyn_VarargInfo _tmp8B2=*_tmp88C.cyc_varargs;
# 2623
if((_tmp8B2.tq).real_const  && !(_tmp8B1.tq).real_const  || !
# 2624
Cyc_Tcutil_subtype(te,assume,_tmp8B2.type,_tmp8B1.type))
# 2625
return 0;}else{
# 2626
if(_tmp889.cyc_varargs != 0  || _tmp88C.cyc_varargs != 0)return 0;}
# 2628
if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp889.effect),(void*)_check_null(_tmp88C.effect)))return 0;
# 2630
if(!Cyc_Tcutil_sub_rgnpo(_tmp889.rgn_po,_tmp88C.rgn_po))return 0;
# 2632
if(!Cyc_Tcutil_sub_attributes(_tmp889.attributes,_tmp88C.attributes))return 0;
# 2634
return 1;};_LL462:;_LL463:
# 2635
 return 0;_LL45B:;};}
# 2646 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2648
struct _RegionHandle _tmp8B3=_new_region("temp");struct _RegionHandle*temp=& _tmp8B3;_push_region(temp);
# 2649
{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,1,te,t1);
# 2650
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);
# 2651
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
# 2652
if(tqs1 == 0){int _tmp8B4=0;_npop_handler(0);return _tmp8B4;}{
# 2653
struct _tuple12 _tmp8B6;struct Cyc_Absyn_Tqual _tmp8B7;void*_tmp8B8;struct _tuple12*_tmp8B5=(struct _tuple12*)tqs1->hd;_tmp8B6=*_tmp8B5;_tmp8B7=_tmp8B6.f1;_tmp8B8=_tmp8B6.f2;{
# 2654
struct _tuple12 _tmp8BA;struct Cyc_Absyn_Tqual _tmp8BB;void*_tmp8BC;struct _tuple12*_tmp8B9=(struct _tuple12*)tqs2->hd;_tmp8BA=*_tmp8B9;_tmp8BB=_tmp8BA.f1;_tmp8BC=_tmp8BA.f2;
# 2656
if(_tmp8B7.real_const  && !_tmp8BB.real_const){int _tmp8BD=0;_npop_handler(0);return _tmp8BD;}
# 2658
if((_tmp8BB.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp8BC))) && 
# 2659
Cyc_Tcutil_subtype(te,assume,_tmp8B8,_tmp8BC))
# 2661
continue;
# 2663
if(Cyc_Tcutil_unify(_tmp8B8,_tmp8BC))
# 2665
continue;
# 2667
if(Cyc_Tcutil_isomorphic(_tmp8B8,_tmp8BC))
# 2669
continue;{
# 2672
int _tmp8BE=0;_npop_handler(0);return _tmp8BE;};};};}{
# 2674
int _tmp8BF=1;_npop_handler(0);return _tmp8BF;};}
# 2649
;_pop_region(temp);}
# 2678
static int Cyc_Tcutil_is_char_type(void*t){
# 2679
void*_tmp8C0=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Size_of _tmp8C2;_LL46C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8C1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8C0;if(_tmp8C1->tag != 6)goto _LL46E;else{_tmp8C2=_tmp8C1->f2;if(_tmp8C2 != Cyc_Absyn_Char_sz)goto _LL46E;}}_LL46D:
# 2680
 return 1;_LL46E:;_LL46F:
# 2681
 return 0;_LL46B:;}
# 2687
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2688
if(Cyc_Tcutil_unify(t1,t2))
# 2689
return Cyc_Absyn_No_coercion;
# 2690
t1=Cyc_Tcutil_compress(t1);
# 2691
t2=Cyc_Tcutil_compress(t2);
# 2693
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
# 2694
return Cyc_Absyn_No_coercion;
# 2695
{void*_tmp8C3=t2;enum Cyc_Absyn_Size_of _tmp8C5;enum Cyc_Absyn_Size_of _tmp8C7;_LL471: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8C4=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8C3;if(_tmp8C4->tag != 6)goto _LL473;else{_tmp8C5=_tmp8C4->f2;if(_tmp8C5 != Cyc_Absyn_Int_sz)goto _LL473;}}_LL472:
# 2697
 goto _LL474;_LL473: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8C6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8C3;if(_tmp8C6->tag != 6)goto _LL475;else{_tmp8C7=_tmp8C6->f2;if(_tmp8C7 != Cyc_Absyn_Long_sz)goto _LL475;}}_LL474:
# 2699
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_No_coercion;
# 2700
goto _LL470;_LL475:;_LL476:
# 2701
 goto _LL470;_LL470:;}{
# 2703
void*_tmp8C8=t1;struct Cyc_Absyn_PtrInfo _tmp8CA;void*_tmp8CB;struct Cyc_Absyn_Tqual _tmp8CC;struct Cyc_Absyn_PtrAtts _tmp8CD;void*_tmp8CE;union Cyc_Absyn_Constraint*_tmp8CF;union Cyc_Absyn_Constraint*_tmp8D0;union Cyc_Absyn_Constraint*_tmp8D1;struct Cyc_Absyn_ArrayInfo _tmp8D3;void*_tmp8D4;struct Cyc_Absyn_Tqual _tmp8D5;struct Cyc_Absyn_Exp*_tmp8D6;union Cyc_Absyn_Constraint*_tmp8D7;struct Cyc_Absyn_Enumdecl*_tmp8D9;void*_tmp8DD;_LL478: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8C9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8C8;if(_tmp8C9->tag != 5)goto _LL47A;else{_tmp8CA=_tmp8C9->f1;_tmp8CB=_tmp8CA.elt_typ;_tmp8CC=_tmp8CA.elt_tq;_tmp8CD=_tmp8CA.ptr_atts;_tmp8CE=_tmp8CD.rgn;_tmp8CF=_tmp8CD.nullable;_tmp8D0=_tmp8CD.bounds;_tmp8D1=_tmp8CD.zero_term;}}_LL479:
# 2711
{void*_tmp8DE=t2;struct Cyc_Absyn_PtrInfo _tmp8E0;void*_tmp8E1;struct Cyc_Absyn_Tqual _tmp8E2;struct Cyc_Absyn_PtrAtts _tmp8E3;void*_tmp8E4;union Cyc_Absyn_Constraint*_tmp8E5;union Cyc_Absyn_Constraint*_tmp8E6;union Cyc_Absyn_Constraint*_tmp8E7;_LL487: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8DF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8DE;if(_tmp8DF->tag != 5)goto _LL489;else{_tmp8E0=_tmp8DF->f1;_tmp8E1=_tmp8E0.elt_typ;_tmp8E2=_tmp8E0.elt_tq;_tmp8E3=_tmp8E0.ptr_atts;_tmp8E4=_tmp8E3.rgn;_tmp8E5=_tmp8E3.nullable;_tmp8E6=_tmp8E3.bounds;_tmp8E7=_tmp8E3.zero_term;}}_LL488: {
# 2715
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
# 2716
struct _tuple0*_tmp1338;struct Cyc_List_List*_tmp1337;struct Cyc_List_List*_tmp8E8=(_tmp1337=_cycalloc(sizeof(*_tmp1337)),((_tmp1337->hd=((_tmp1338=_cycalloc(sizeof(*_tmp1338)),((_tmp1338->f1=t1,((_tmp1338->f2=t2,_tmp1338)))))),((_tmp1337->tl=0,_tmp1337)))));
# 2717
int _tmp8E9=_tmp8E2.real_const  || !_tmp8CC.real_const;
# 2729 "tcutil.cyc"
int _tmp8EA=
# 2730
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp8E6,Cyc_Absyn_bounds_one_conref) && !
# 2731
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp8E7);
# 2733
int _tmp8EB=_tmp8E9  && (
# 2734
((_tmp8EA  && Cyc_Tcutil_ptrsubtype(te,_tmp8E8,_tmp8CB,_tmp8E1) || 
# 2735
Cyc_Tcutil_unify(_tmp8CB,_tmp8E1)) || Cyc_Tcutil_isomorphic(_tmp8CB,_tmp8E1)) || Cyc_Tcutil_unify(_tmp8E1,(void*)& Cyc_Absyn_VoidType_val));
# 2737
Cyc_Tcutil_t1_failure=t1;
# 2738
Cyc_Tcutil_t2_failure=t2;{
# 2739
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8D1,_tmp8E7) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8E7);
# 2741
int _tmp8EC=_tmp8EB?0:((Cyc_Tcutil_bits_only(_tmp8CB) && Cyc_Tcutil_is_char_type(_tmp8E1)) && !
# 2742
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8E7)) && (
# 2743
_tmp8E2.real_const  || !_tmp8CC.real_const);
# 2744
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp8D0,_tmp8E6);
# 2745
if(!bounds_ok  && !_tmp8EC){
# 2746
struct _tuple0 _tmp1339;struct _tuple0 _tmp8EE=(_tmp1339.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8D0),((_tmp1339.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8E6),_tmp1339)));void*_tmp8EF;struct Cyc_Absyn_Exp*_tmp8F1;void*_tmp8F2;struct Cyc_Absyn_Exp*_tmp8F4;_LL48C: _tmp8EF=_tmp8EE.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp8F0=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp8EF;if(_tmp8F0->tag != 1)goto _LL48E;else{_tmp8F1=_tmp8F0->f1;}};_tmp8F2=_tmp8EE.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp8F3=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp8F2;if(_tmp8F3->tag != 1)goto _LL48E;else{_tmp8F4=_tmp8F3->f1;}};_LL48D:
# 2751
 if(Cyc_Evexp_lte_const_exp(_tmp8F4,_tmp8F1))
# 2752
bounds_ok=1;
# 2753
goto _LL48B;_LL48E:;_LL48F:
# 2756
 bounds_ok=1;goto _LL48B;_LL48B:;}
# 2758
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8CF) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8E5))
# 2759
coercion=Cyc_Absyn_NonNull_to_Null;
# 2763
if(((bounds_ok  && zeroterm_ok) && (_tmp8EB  || _tmp8EC)) && (
# 2764
Cyc_Tcutil_unify(_tmp8CE,_tmp8E4) || Cyc_Tcenv_region_outlives(te,_tmp8CE,_tmp8E4)))
# 2765
return coercion;else{
# 2766
return Cyc_Absyn_Unknown_coercion;}};}_LL489:;_LL48A:
# 2767
 goto _LL486;_LL486:;}
# 2769
return Cyc_Absyn_Unknown_coercion;_LL47A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8D2=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8C8;if(_tmp8D2->tag != 8)goto _LL47C;else{_tmp8D3=_tmp8D2->f1;_tmp8D4=_tmp8D3.elt_type;_tmp8D5=_tmp8D3.tq;_tmp8D6=_tmp8D3.num_elts;_tmp8D7=_tmp8D3.zero_term;}}_LL47B:
# 2771
{void*_tmp8F7=t2;struct Cyc_Absyn_ArrayInfo _tmp8F9;void*_tmp8FA;struct Cyc_Absyn_Tqual _tmp8FB;struct Cyc_Absyn_Exp*_tmp8FC;union Cyc_Absyn_Constraint*_tmp8FD;_LL491: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8F8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8F7;if(_tmp8F8->tag != 8)goto _LL493;else{_tmp8F9=_tmp8F8->f1;_tmp8FA=_tmp8F9.elt_type;_tmp8FB=_tmp8F9.tq;_tmp8FC=_tmp8F9.num_elts;_tmp8FD=_tmp8F9.zero_term;}}_LL492: {
# 2773
int okay;
# 2774
okay=
# 2775
((_tmp8D6 != 0  && _tmp8FC != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8D7,_tmp8FD)) && 
# 2776
Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp8FC,(struct Cyc_Absyn_Exp*)_tmp8D6);
# 2777
return
# 2779
(okay  && Cyc_Tcutil_unify(_tmp8D4,_tmp8FA)) && (!_tmp8D5.real_const  || _tmp8FB.real_const)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;}_LL493:;_LL494:
# 2781
 return Cyc_Absyn_Unknown_coercion;_LL490:;}
# 2783
return Cyc_Absyn_Unknown_coercion;_LL47C: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp8D8=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp8C8;if(_tmp8D8->tag != 13)goto _LL47E;else{_tmp8D9=_tmp8D8->f2;}}_LL47D:
# 2787
{void*_tmp8FE=t2;struct Cyc_Absyn_Enumdecl*_tmp900;_LL496: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp8FF=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp8FE;if(_tmp8FF->tag != 13)goto _LL498;else{_tmp900=_tmp8FF->f2;}}_LL497:
# 2789
 if((_tmp8D9->fields != 0  && _tmp900->fields != 0) && 
# 2790
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp8D9->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp900->fields))->v))
# 2791
return Cyc_Absyn_No_coercion;
# 2792
goto _LL495;_LL498:;_LL499:
# 2793
 goto _LL495;_LL495:;}
# 2795
goto _LL47F;_LL47E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8DA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8C8;if(_tmp8DA->tag != 6)goto _LL480;}_LL47F:
# 2796
 goto _LL481;_LL480: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp8DB=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp8C8;if(_tmp8DB->tag != 7)goto _LL482;}_LL481:
# 2797
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;_LL482: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp8DC=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp8C8;if(_tmp8DC->tag != 15)goto _LL484;else{_tmp8DD=(void*)_tmp8DC->f1;}}_LL483:
# 2800
{void*_tmp901=t2;void*_tmp903;_LL49B: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp902=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp901;if(_tmp902->tag != 15)goto _LL49D;else{_tmp903=(void*)_tmp902->f1;}}_LL49C:
# 2802
 if(Cyc_Tcenv_region_outlives(te,_tmp8DD,_tmp903))return Cyc_Absyn_No_coercion;
# 2803
goto _LL49A;_LL49D:;_LL49E:
# 2804
 goto _LL49A;_LL49A:;}
# 2806
return Cyc_Absyn_Unknown_coercion;_LL484:;_LL485:
# 2807
 return Cyc_Absyn_Unknown_coercion;_LL477:;};}
# 2812
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
# 2813
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
# 2814
struct Cyc_Absyn_Exp*_tmp904=Cyc_Absyn_copy_exp(e);
# 2815
{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp133C;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp133B;e->r=(void*)((_tmp133B=_cycalloc(sizeof(*_tmp133B)),((_tmp133B[0]=((_tmp133C.tag=13,((_tmp133C.f1=(void*)t,((_tmp133C.f2=_tmp904,((_tmp133C.f3=0,((_tmp133C.f4=c,_tmp133C)))))))))),_tmp133B))));}
# 2816
e->topt=(void*)t;}}
# 2820
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
# 2821
void*_tmp907=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL4A0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp908=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp907;if(_tmp908->tag != 6)goto _LL4A2;}_LL4A1:
# 2822
 goto _LL4A3;_LL4A2: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp909=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp907;if(_tmp909->tag != 13)goto _LL4A4;}_LL4A3:
# 2823
 goto _LL4A5;_LL4A4: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp90A=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp907;if(_tmp90A->tag != 14)goto _LL4A6;}_LL4A5:
# 2824
 goto _LL4A7;_LL4A6: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp90B=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp907;if(_tmp90B->tag != 19)goto _LL4A8;}_LL4A7:
# 2825
 return 1;_LL4A8: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp90C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp907;if(_tmp90C->tag != 1)goto _LL4AA;}_LL4A9:
# 2826
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);_LL4AA:;_LL4AB:
# 2827
 return 0;_LL49F:;}
# 2831
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
# 2832
if(Cyc_Tcutil_is_integral(e))
# 2833
return 1;{
# 2834
void*_tmp90D=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL4AD: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp90E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp90D;if(_tmp90E->tag != 7)goto _LL4AF;}_LL4AE:
# 2835
 return 1;_LL4AF:;_LL4B0:
# 2836
 return 0;_LL4AC:;};}
# 2840
int Cyc_Tcutil_is_function_type(void*t){
# 2841
void*_tmp90F=Cyc_Tcutil_compress(t);_LL4B2: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp910=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp90F;if(_tmp910->tag != 9)goto _LL4B4;}_LL4B3:
# 2842
 return 1;_LL4B4:;_LL4B5:
# 2843
 return 0;_LL4B1:;}
# 2847
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
# 2848
struct _tuple0 _tmp133D;struct _tuple0 _tmp912=(_tmp133D.f1=t1,((_tmp133D.f2=t2,_tmp133D)));void*_tmp913;int _tmp915;void*_tmp916;int _tmp918;void*_tmp919;void*_tmp91B;void*_tmp91D;enum Cyc_Absyn_Sign _tmp91F;enum Cyc_Absyn_Size_of _tmp920;void*_tmp921;enum Cyc_Absyn_Sign _tmp923;enum Cyc_Absyn_Size_of _tmp924;void*_tmp925;enum Cyc_Absyn_Size_of _tmp927;void*_tmp928;enum Cyc_Absyn_Size_of _tmp92A;void*_tmp92B;enum Cyc_Absyn_Sign _tmp92D;enum Cyc_Absyn_Size_of _tmp92E;void*_tmp92F;enum Cyc_Absyn_Sign _tmp931;enum Cyc_Absyn_Size_of _tmp932;void*_tmp933;enum Cyc_Absyn_Sign _tmp935;enum Cyc_Absyn_Size_of _tmp936;void*_tmp937;enum Cyc_Absyn_Sign _tmp939;enum Cyc_Absyn_Size_of _tmp93A;void*_tmp93B;enum Cyc_Absyn_Size_of _tmp93D;void*_tmp93E;enum Cyc_Absyn_Size_of _tmp940;_LL4B7: _tmp913=_tmp912.f1;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp914=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp913;if(_tmp914->tag != 7)goto _LL4B9;else{_tmp915=_tmp914->f1;}};_tmp916=_tmp912.f2;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp917=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp916;if(_tmp917->tag != 7)goto _LL4B9;else{_tmp918=_tmp917->f1;}};_LL4B8:
# 2850
 if(_tmp915 != 0  && _tmp915 != 1)return t1;else{
# 2851
if(_tmp918 != 0  && _tmp918 != 1)return t2;else{
# 2852
if(_tmp915 >= _tmp918)return t1;else{
# 2853
return t2;}}}_LL4B9: _tmp919=_tmp912.f1;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp91A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp919;if(_tmp91A->tag != 7)goto _LL4BB;};_LL4BA:
# 2854
 return t1;_LL4BB: _tmp91B=_tmp912.f2;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp91C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp91B;if(_tmp91C->tag != 7)goto _LL4BD;};_LL4BC:
# 2855
 return t2;_LL4BD: _tmp91D=_tmp912.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp91E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp91D;if(_tmp91E->tag != 6)goto _LL4BF;else{_tmp91F=_tmp91E->f1;if(_tmp91F != Cyc_Absyn_Unsigned)goto _LL4BF;_tmp920=_tmp91E->f2;if(_tmp920 != Cyc_Absyn_LongLong_sz)goto _LL4BF;}};_LL4BE:
# 2856
 goto _LL4C0;_LL4BF: _tmp921=_tmp912.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp922=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp921;if(_tmp922->tag != 6)goto _LL4C1;else{_tmp923=_tmp922->f1;if(_tmp923 != Cyc_Absyn_Unsigned)goto _LL4C1;_tmp924=_tmp922->f2;if(_tmp924 != Cyc_Absyn_LongLong_sz)goto _LL4C1;}};_LL4C0:
# 2857
 return Cyc_Absyn_ulonglong_typ;_LL4C1: _tmp925=_tmp912.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp926=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp925;if(_tmp926->tag != 6)goto _LL4C3;else{_tmp927=_tmp926->f2;if(_tmp927 != Cyc_Absyn_LongLong_sz)goto _LL4C3;}};_LL4C2:
# 2858
 goto _LL4C4;_LL4C3: _tmp928=_tmp912.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp929=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp928;if(_tmp929->tag != 6)goto _LL4C5;else{_tmp92A=_tmp929->f2;if(_tmp92A != Cyc_Absyn_LongLong_sz)goto _LL4C5;}};_LL4C4:
# 2859
 return Cyc_Absyn_slonglong_typ;_LL4C5: _tmp92B=_tmp912.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp92C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp92B;if(_tmp92C->tag != 6)goto _LL4C7;else{_tmp92D=_tmp92C->f1;if(_tmp92D != Cyc_Absyn_Unsigned)goto _LL4C7;_tmp92E=_tmp92C->f2;if(_tmp92E != Cyc_Absyn_Long_sz)goto _LL4C7;}};_LL4C6:
# 2860
 goto _LL4C8;_LL4C7: _tmp92F=_tmp912.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp930=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp92F;if(_tmp930->tag != 6)goto _LL4C9;else{_tmp931=_tmp930->f1;if(_tmp931 != Cyc_Absyn_Unsigned)goto _LL4C9;_tmp932=_tmp930->f2;if(_tmp932 != Cyc_Absyn_Long_sz)goto _LL4C9;}};_LL4C8:
# 2861
 return Cyc_Absyn_ulong_typ;_LL4C9: _tmp933=_tmp912.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp934=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp933;if(_tmp934->tag != 6)goto _LL4CB;else{_tmp935=_tmp934->f1;if(_tmp935 != Cyc_Absyn_Unsigned)goto _LL4CB;_tmp936=_tmp934->f2;if(_tmp936 != Cyc_Absyn_Int_sz)goto _LL4CB;}};_LL4CA:
# 2863
 goto _LL4CC;_LL4CB: _tmp937=_tmp912.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp938=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp937;if(_tmp938->tag != 6)goto _LL4CD;else{_tmp939=_tmp938->f1;if(_tmp939 != Cyc_Absyn_Unsigned)goto _LL4CD;_tmp93A=_tmp938->f2;if(_tmp93A != Cyc_Absyn_Int_sz)goto _LL4CD;}};_LL4CC:
# 2864
 return Cyc_Absyn_uint_typ;_LL4CD: _tmp93B=_tmp912.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp93C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp93B;if(_tmp93C->tag != 6)goto _LL4CF;else{_tmp93D=_tmp93C->f2;if(_tmp93D != Cyc_Absyn_Long_sz)goto _LL4CF;}};_LL4CE:
# 2865
 goto _LL4D0;_LL4CF: _tmp93E=_tmp912.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp93F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp93E;if(_tmp93F->tag != 6)goto _LL4D1;else{_tmp940=_tmp93F->f2;if(_tmp940 != Cyc_Absyn_Long_sz)goto _LL4D1;}};_LL4D0:
# 2866
 return Cyc_Absyn_slong_typ;_LL4D1:;_LL4D2:
# 2867
 return Cyc_Absyn_sint_typ;_LL4B6:;}
# 2872
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*e){
# 2873
void*_tmp941=e->r;struct Cyc_Core_Opt*_tmp943;_LL4D4: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp942=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp941;if(_tmp942->tag != 3)goto _LL4D6;else{_tmp943=_tmp942->f2;if(_tmp943 != 0)goto _LL4D6;}}_LL4D5:
# 2874
{const char*_tmp1340;void*_tmp133F;(_tmp133F=0,Cyc_Tcutil_warn(e->loc,((_tmp1340="assignment in test",_tag_dyneither(_tmp1340,sizeof(char),19))),_tag_dyneither(_tmp133F,sizeof(void*),0)));}goto _LL4D3;_LL4D6:;_LL4D7:
# 2875
 goto _LL4D3;_LL4D3:;}
# 2886 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
# 2887
c1=Cyc_Absyn_compress_kb(c1);
# 2888
c2=Cyc_Absyn_compress_kb(c2);{
# 2889
struct _tuple0 _tmp1341;struct _tuple0 _tmp947=(_tmp1341.f1=c1,((_tmp1341.f2=c2,_tmp1341)));void*_tmp948;struct Cyc_Absyn_Kind*_tmp94A;void*_tmp94B;struct Cyc_Absyn_Kind*_tmp94D;void*_tmp94E;struct Cyc_Core_Opt*_tmp950;struct Cyc_Core_Opt**_tmp951;void*_tmp952;struct Cyc_Core_Opt*_tmp954;struct Cyc_Core_Opt**_tmp955;void*_tmp956;struct Cyc_Core_Opt*_tmp958;struct Cyc_Core_Opt**_tmp959;struct Cyc_Absyn_Kind*_tmp95A;void*_tmp95B;struct Cyc_Absyn_Kind*_tmp95D;void*_tmp95E;struct Cyc_Absyn_Kind*_tmp960;void*_tmp961;struct Cyc_Core_Opt*_tmp963;struct Cyc_Core_Opt**_tmp964;struct Cyc_Absyn_Kind*_tmp965;void*_tmp966;struct Cyc_Core_Opt*_tmp968;struct Cyc_Core_Opt**_tmp969;struct Cyc_Absyn_Kind*_tmp96A;void*_tmp96B;struct Cyc_Core_Opt*_tmp96D;struct Cyc_Core_Opt**_tmp96E;struct Cyc_Absyn_Kind*_tmp96F;_LL4D9: _tmp948=_tmp947.f1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp949=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp948;if(_tmp949->tag != 0)goto _LL4DB;else{_tmp94A=_tmp949->f1;}};_tmp94B=_tmp947.f2;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp94C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp94B;if(_tmp94C->tag != 0)goto _LL4DB;else{_tmp94D=_tmp94C->f1;}};_LL4DA:
# 2890
 return _tmp94A == _tmp94D;_LL4DB: _tmp94E=_tmp947.f2;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp94F=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp94E;if(_tmp94F->tag != 1)goto _LL4DD;else{_tmp950=_tmp94F->f1;_tmp951=(struct Cyc_Core_Opt**)& _tmp94F->f1;}};_LL4DC:
# 2891
{struct Cyc_Core_Opt*_tmp1342;*_tmp951=((_tmp1342=_cycalloc(sizeof(*_tmp1342)),((_tmp1342->v=c1,_tmp1342))));}return 1;_LL4DD: _tmp952=_tmp947.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp953=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp952;if(_tmp953->tag != 1)goto _LL4DF;else{_tmp954=_tmp953->f1;_tmp955=(struct Cyc_Core_Opt**)& _tmp953->f1;}};_LL4DE:
# 2892
{struct Cyc_Core_Opt*_tmp1343;*_tmp955=((_tmp1343=_cycalloc(sizeof(*_tmp1343)),((_tmp1343->v=c2,_tmp1343))));}return 1;_LL4DF: _tmp956=_tmp947.f1;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp957=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp956;if(_tmp957->tag != 2)goto _LL4E1;else{_tmp958=_tmp957->f1;_tmp959=(struct Cyc_Core_Opt**)& _tmp957->f1;_tmp95A=_tmp957->f2;}};_tmp95B=_tmp947.f2;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp95C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp95B;if(_tmp95C->tag != 0)goto _LL4E1;else{_tmp95D=_tmp95C->f1;}};_LL4E0:
# 2894
 if(Cyc_Tcutil_kind_leq(_tmp95D,_tmp95A)){
# 2895
{struct Cyc_Core_Opt*_tmp1344;*_tmp959=((_tmp1344=_cycalloc(sizeof(*_tmp1344)),((_tmp1344->v=c2,_tmp1344))));}return 1;}else{
# 2896
return 0;}_LL4E1: _tmp95E=_tmp947.f1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp95F=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp95E;if(_tmp95F->tag != 0)goto _LL4E3;else{_tmp960=_tmp95F->f1;}};_tmp961=_tmp947.f2;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp962=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp961;if(_tmp962->tag != 2)goto _LL4E3;else{_tmp963=_tmp962->f1;_tmp964=(struct Cyc_Core_Opt**)& _tmp962->f1;_tmp965=_tmp962->f2;}};_LL4E2:
# 2898
 if(Cyc_Tcutil_kind_leq(_tmp960,_tmp965)){
# 2899
{struct Cyc_Core_Opt*_tmp1345;*_tmp964=((_tmp1345=_cycalloc(sizeof(*_tmp1345)),((_tmp1345->v=c1,_tmp1345))));}return 1;}else{
# 2900
return 0;}_LL4E3: _tmp966=_tmp947.f1;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp967=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp966;if(_tmp967->tag != 2)goto _LL4D8;else{_tmp968=_tmp967->f1;_tmp969=(struct Cyc_Core_Opt**)& _tmp967->f1;_tmp96A=_tmp967->f2;}};_tmp96B=_tmp947.f2;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp96C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp96B;if(_tmp96C->tag != 2)goto _LL4D8;else{_tmp96D=_tmp96C->f1;_tmp96E=(struct Cyc_Core_Opt**)& _tmp96C->f1;_tmp96F=_tmp96C->f2;}};_LL4E4:
# 2902
 if(Cyc_Tcutil_kind_leq(_tmp96A,_tmp96F)){
# 2903
{struct Cyc_Core_Opt*_tmp1346;*_tmp96E=((_tmp1346=_cycalloc(sizeof(*_tmp1346)),((_tmp1346->v=c1,_tmp1346))));}return 1;}else{
# 2904
if(Cyc_Tcutil_kind_leq(_tmp96F,_tmp96A)){
# 2905
{struct Cyc_Core_Opt*_tmp1347;*_tmp969=((_tmp1347=_cycalloc(sizeof(*_tmp1347)),((_tmp1347->v=c2,_tmp1347))));}return 1;}else{
# 2906
return 0;}}_LL4D8:;};}
# 2911
static int Cyc_Tcutil_tvar_id_counter=0;
# 2912
int Cyc_Tcutil_new_tvar_id(){
# 2913
return Cyc_Tcutil_tvar_id_counter ++;}
# 2916
static int Cyc_Tcutil_tvar_counter=0;
# 2917
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
# 2918
int i=Cyc_Tcutil_tvar_counter ++;
# 2919
const char*_tmp134B;void*_tmp134A[1];struct Cyc_Int_pa_PrintArg_struct _tmp1349;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp1349.tag=1,((_tmp1349.f1=(unsigned long)i,((_tmp134A[0]=& _tmp1349,Cyc_aprintf(((_tmp134B="#%d",_tag_dyneither(_tmp134B,sizeof(char),4))),_tag_dyneither(_tmp134A,sizeof(void*),1))))))));
# 2920
struct _dyneither_ptr*_tmp134E;struct Cyc_Absyn_Tvar*_tmp134D;return(_tmp134D=_cycalloc(sizeof(*_tmp134D)),((_tmp134D->name=((_tmp134E=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp134E[0]=s,_tmp134E)))),((_tmp134D->identity=- 1,((_tmp134D->kind=k,_tmp134D)))))));}
# 2923
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
# 2924
struct _dyneither_ptr _tmp97B=*t->name;
# 2925
return*((const char*)_check_dyneither_subscript(_tmp97B,sizeof(char),0))== '#';}
# 2928
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
# 2929
{const char*_tmp1352;void*_tmp1351[1];struct Cyc_String_pa_PrintArg_struct _tmp1350;(_tmp1350.tag=0,((_tmp1350.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmp1351[0]=& _tmp1350,Cyc_printf(((_tmp1352="%s",_tag_dyneither(_tmp1352,sizeof(char),3))),_tag_dyneither(_tmp1351,sizeof(void*),1)))))));}
# 2930
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
# 2931
const char*_tmp1353;struct _dyneither_ptr _tmp97F=Cyc_strconcat(((_tmp1353="`",_tag_dyneither(_tmp1353,sizeof(char),2))),(struct _dyneither_ptr)*t->name);
# 2932
{char _tmp1356;char _tmp1355;struct _dyneither_ptr _tmp1354;(_tmp1354=_dyneither_ptr_plus(_tmp97F,sizeof(char),1),((_tmp1355=*((char*)_check_dyneither_subscript(_tmp1354,sizeof(char),0)),((_tmp1356='t',((_get_dyneither_size(_tmp1354,sizeof(char))== 1  && (_tmp1355 == '\000'  && _tmp1356 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp1354.curr)=_tmp1356)))))));}{
# 2933
struct _dyneither_ptr*_tmp1357;t->name=((_tmp1357=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp1357[0]=(struct _dyneither_ptr)_tmp97F,_tmp1357))));};};}
# 2937
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 2939
struct _tuple10*_tmp1358;return(_tmp1358=_cycalloc(sizeof(*_tmp1358)),((_tmp1358->f1=(struct _dyneither_ptr*)(*x).f1,((_tmp1358->f2=(*x).f2,((_tmp1358->f3=(*x).f3,_tmp1358)))))));}
# 2942
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
# 2943
if(fd->cached_typ == 0){
# 2949
struct Cyc_List_List*_tmp986=0;
# 2950
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
# 2951
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
# 2952
struct Cyc_List_List*_tmp1359;_tmp986=((_tmp1359=_cycalloc(sizeof(*_tmp1359)),((_tmp1359->hd=(void*)atts->hd,((_tmp1359->tl=_tmp986,_tmp1359))))));}}}{
# 2953
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp135F;struct Cyc_Absyn_FnInfo _tmp135E;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp135D;return(void*)((_tmp135D=_cycalloc(sizeof(*_tmp135D)),((_tmp135D[0]=((_tmp135F.tag=9,((_tmp135F.f1=((_tmp135E.tvars=fd->tvs,((_tmp135E.effect=fd->effect,((_tmp135E.ret_tqual=fd->ret_tqual,((_tmp135E.ret_typ=fd->ret_type,((_tmp135E.args=
# 2954
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args),((_tmp135E.c_varargs=fd->c_varargs,((_tmp135E.cyc_varargs=fd->cyc_varargs,((_tmp135E.rgn_po=fd->rgn_po,((_tmp135E.attributes=_tmp986,_tmp135E)))))))))))))))))),_tmp135F)))),_tmp135D))));};}
# 2958
return(void*)_check_null(fd->cached_typ);}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 2962
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
# 2963
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
# 2964
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
# 2965
struct _tuple12 _tmp98C;struct Cyc_Absyn_Tqual _tmp98D;void*_tmp98E;struct _tuple12*_tmp98B=pr;_tmp98C=*_tmp98B;_tmp98D=_tmp98C.f1;_tmp98E=_tmp98C.f2;
# 2966
if(_tmp98E == t)return pr;else{
# 2967
struct _tuple12*_tmp1360;return(_tmp1360=_cycalloc(sizeof(*_tmp1360)),((_tmp1360->f1=_tmp98D,((_tmp1360->f2=t,_tmp1360)))));}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 2969
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 2971
struct _tuple26*_tmp1363;struct _tuple27*_tmp1362;return(_tmp1362=_region_malloc(rgn,sizeof(*_tmp1362)),((_tmp1362->f1=((_tmp1363=_region_malloc(rgn,sizeof(*_tmp1363)),((_tmp1363->f1=(*y).f1,((_tmp1363->f2=(*y).f2,_tmp1363)))))),((_tmp1362->f2=(*y).f3,_tmp1362)))));}
# 2973
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 2975
struct _dyneither_ptr*_tmp993;struct Cyc_Absyn_Tqual _tmp994;void*_tmp995;struct _tuple10 _tmp992=*orig_arg;_tmp993=_tmp992.f1;_tmp994=_tmp992.f2;_tmp995=_tmp992.f3;
# 2976
if(t == _tmp995)return orig_arg;{
# 2977
struct _tuple10*_tmp1364;return(_tmp1364=_cycalloc(sizeof(*_tmp1364)),((_tmp1364->f1=_tmp993,((_tmp1364->f2=_tmp994,((_tmp1364->f3=t,_tmp1364)))))));};}
# 2979
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
# 2980
return f->type;}
# 2982
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(struct Cyc_Absyn_Aggrfield*f,void*t){
# 2983
if(f->type == t)return f;{
# 2984
struct Cyc_Absyn_Aggrfield*_tmp1365;return(_tmp1365=_cycalloc(sizeof(*_tmp1365)),((_tmp1365->name=f->name,((_tmp1365->tq=f->tq,((_tmp1365->type=t,((_tmp1365->width=f->width,((_tmp1365->attributes=f->attributes,_tmp1365)))))))))));};}
# 2988
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 2993
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 2995
struct Cyc_Absyn_Exp*_tmp1366;return(_tmp1366=_cycalloc(sizeof(*_tmp1366)),((_tmp1366->topt=old->topt,((_tmp1366->r=r,((_tmp1366->loc=old->loc,((_tmp1366->annot=old->annot,_tmp1366)))))))));}
# 3000
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3002
void*_tmp999=e->r;enum Cyc_Absyn_Primop _tmp99F;struct Cyc_List_List*_tmp9A0;struct Cyc_Absyn_Exp*_tmp9A2;struct Cyc_Absyn_Exp*_tmp9A3;struct Cyc_Absyn_Exp*_tmp9A4;struct Cyc_Absyn_Exp*_tmp9A6;struct Cyc_Absyn_Exp*_tmp9A7;struct Cyc_Absyn_Exp*_tmp9A9;struct Cyc_Absyn_Exp*_tmp9AA;struct Cyc_Absyn_Exp*_tmp9AC;struct Cyc_Absyn_Exp*_tmp9AD;void*_tmp9AF;struct Cyc_Absyn_Exp*_tmp9B0;int _tmp9B1;enum Cyc_Absyn_Coercion _tmp9B2;void*_tmp9B4;struct Cyc_Absyn_Exp*_tmp9B6;void*_tmp9B8;void*_tmp9B9;void*_tmp9BB;_LL4E6: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp99A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp99A->tag != 0)goto _LL4E8;}_LL4E7:
# 3003
 goto _LL4E9;_LL4E8: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp99B=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp99B->tag != 30)goto _LL4EA;}_LL4E9:
# 3004
 goto _LL4EB;_LL4EA: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp99C=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp99C->tag != 31)goto _LL4EC;}_LL4EB:
# 3005
 goto _LL4ED;_LL4EC: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp99D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp99D->tag != 1)goto _LL4EE;}_LL4ED:
# 3006
 return e;_LL4EE: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp99E=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp99E->tag != 2)goto _LL4F0;else{_tmp99F=_tmp99E->f1;_tmp9A0=_tmp99E->f2;}}_LL4EF:
# 3009
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9A0)== 1){
# 3010
struct Cyc_Absyn_Exp*_tmp9BC=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp9A0))->hd;
# 3011
struct Cyc_Absyn_Exp*_tmp9BD=Cyc_Tcutil_rsubsexp(r,inst,_tmp9BC);
# 3012
if(_tmp9BD == _tmp9BC)return e;{
# 3013
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp136C;struct Cyc_Absyn_Exp*_tmp136B[1];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp136A;return Cyc_Tcutil_copye(e,(void*)((_tmp136A=_cycalloc(sizeof(*_tmp136A)),((_tmp136A[0]=((_tmp136C.tag=2,((_tmp136C.f1=_tmp99F,((_tmp136C.f2=((_tmp136B[0]=_tmp9BD,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp136B,sizeof(struct Cyc_Absyn_Exp*),1)))),_tmp136C)))))),_tmp136A)))));};}else{
# 3014
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9A0)== 2){
# 3015
struct Cyc_Absyn_Exp*_tmp9C1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp9A0))->hd;
# 3016
struct Cyc_Absyn_Exp*_tmp9C2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp9A0->tl))->hd;
# 3017
struct Cyc_Absyn_Exp*_tmp9C3=Cyc_Tcutil_rsubsexp(r,inst,_tmp9C1);
# 3018
struct Cyc_Absyn_Exp*_tmp9C4=Cyc_Tcutil_rsubsexp(r,inst,_tmp9C2);
# 3019
if(_tmp9C3 == _tmp9C1  && _tmp9C4 == _tmp9C2)return e;{
# 3020
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp1372;struct Cyc_Absyn_Exp*_tmp1371[2];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1370;return Cyc_Tcutil_copye(e,(void*)((_tmp1370=_cycalloc(sizeof(*_tmp1370)),((_tmp1370[0]=((_tmp1372.tag=2,((_tmp1372.f1=_tmp99F,((_tmp1372.f2=((_tmp1371[1]=_tmp9C4,((_tmp1371[0]=_tmp9C3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1371,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp1372)))))),_tmp1370)))));};}else{
# 3021
const char*_tmp1375;void*_tmp1374;return(_tmp1374=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1375="primop does not have 1 or 2 args!",_tag_dyneither(_tmp1375,sizeof(char),34))),_tag_dyneither(_tmp1374,sizeof(void*),0)));}}_LL4F0: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp9A1=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp9A1->tag != 5)goto _LL4F2;else{_tmp9A2=_tmp9A1->f1;_tmp9A3=_tmp9A1->f2;_tmp9A4=_tmp9A1->f3;}}_LL4F1: {
# 3023
struct Cyc_Absyn_Exp*_tmp9CA=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A2);
# 3024
struct Cyc_Absyn_Exp*_tmp9CB=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A3);
# 3025
struct Cyc_Absyn_Exp*_tmp9CC=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A4);
# 3026
if((_tmp9CA == _tmp9A2  && _tmp9CB == _tmp9A3) && _tmp9CC == _tmp9A4)return e;{
# 3027
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp1378;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1377;return Cyc_Tcutil_copye(e,(void*)((_tmp1377=_cycalloc(sizeof(*_tmp1377)),((_tmp1377[0]=((_tmp1378.tag=5,((_tmp1378.f1=_tmp9CA,((_tmp1378.f2=_tmp9CB,((_tmp1378.f3=_tmp9CC,_tmp1378)))))))),_tmp1377)))));};}_LL4F2: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp9A5=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp9A5->tag != 6)goto _LL4F4;else{_tmp9A6=_tmp9A5->f1;_tmp9A7=_tmp9A5->f2;}}_LL4F3: {
# 3029
struct Cyc_Absyn_Exp*_tmp9CF=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A6);
# 3030
struct Cyc_Absyn_Exp*_tmp9D0=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A7);
# 3031
if(_tmp9CF == _tmp9A6  && _tmp9D0 == _tmp9A7)return e;{
# 3032
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp137B;struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp137A;return Cyc_Tcutil_copye(e,(void*)((_tmp137A=_cycalloc(sizeof(*_tmp137A)),((_tmp137A[0]=((_tmp137B.tag=6,((_tmp137B.f1=_tmp9CF,((_tmp137B.f2=_tmp9D0,_tmp137B)))))),_tmp137A)))));};}_LL4F4: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp9A8=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp9A8->tag != 7)goto _LL4F6;else{_tmp9A9=_tmp9A8->f1;_tmp9AA=_tmp9A8->f2;}}_LL4F5: {
# 3034
struct Cyc_Absyn_Exp*_tmp9D3=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A9);
# 3035
struct Cyc_Absyn_Exp*_tmp9D4=Cyc_Tcutil_rsubsexp(r,inst,_tmp9AA);
# 3036
if(_tmp9D3 == _tmp9A9  && _tmp9D4 == _tmp9AA)return e;{
# 3037
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp137E;struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp137D;return Cyc_Tcutil_copye(e,(void*)((_tmp137D=_cycalloc(sizeof(*_tmp137D)),((_tmp137D[0]=((_tmp137E.tag=7,((_tmp137E.f1=_tmp9D3,((_tmp137E.f2=_tmp9D4,_tmp137E)))))),_tmp137D)))));};}_LL4F6: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp9AB=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp9AB->tag != 8)goto _LL4F8;else{_tmp9AC=_tmp9AB->f1;_tmp9AD=_tmp9AB->f2;}}_LL4F7: {
# 3039
struct Cyc_Absyn_Exp*_tmp9D7=Cyc_Tcutil_rsubsexp(r,inst,_tmp9AC);
# 3040
struct Cyc_Absyn_Exp*_tmp9D8=Cyc_Tcutil_rsubsexp(r,inst,_tmp9AD);
# 3041
if(_tmp9D7 == _tmp9AC  && _tmp9D8 == _tmp9AD)return e;{
# 3042
struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp1381;struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp1380;return Cyc_Tcutil_copye(e,(void*)((_tmp1380=_cycalloc(sizeof(*_tmp1380)),((_tmp1380[0]=((_tmp1381.tag=8,((_tmp1381.f1=_tmp9D7,((_tmp1381.f2=_tmp9D8,_tmp1381)))))),_tmp1380)))));};}_LL4F8: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp9AE=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp9AE->tag != 13)goto _LL4FA;else{_tmp9AF=(void*)_tmp9AE->f1;_tmp9B0=_tmp9AE->f2;_tmp9B1=_tmp9AE->f3;_tmp9B2=_tmp9AE->f4;}}_LL4F9: {
# 3044
struct Cyc_Absyn_Exp*_tmp9DB=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B0);
# 3045
void*_tmp9DC=Cyc_Tcutil_rsubstitute(r,inst,_tmp9AF);
# 3046
if(_tmp9DB == _tmp9B0  && _tmp9DC == _tmp9AF)return e;{
# 3047
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp1384;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1383;return Cyc_Tcutil_copye(e,(void*)((_tmp1383=_cycalloc(sizeof(*_tmp1383)),((_tmp1383[0]=((_tmp1384.tag=13,((_tmp1384.f1=(void*)_tmp9DC,((_tmp1384.f2=_tmp9DB,((_tmp1384.f3=_tmp9B1,((_tmp1384.f4=_tmp9B2,_tmp1384)))))))))),_tmp1383)))));};}_LL4FA: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp9B3=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp9B3->tag != 16)goto _LL4FC;else{_tmp9B4=(void*)_tmp9B3->f1;}}_LL4FB: {
# 3049
void*_tmp9DF=Cyc_Tcutil_rsubstitute(r,inst,_tmp9B4);
# 3050
if(_tmp9DF == _tmp9B4)return e;{
# 3051
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp1387;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp1386;return Cyc_Tcutil_copye(e,(void*)((_tmp1386=_cycalloc(sizeof(*_tmp1386)),((_tmp1386[0]=((_tmp1387.tag=16,((_tmp1387.f1=(void*)_tmp9DF,_tmp1387)))),_tmp1386)))));};}_LL4FC: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp9B5=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp9B5->tag != 17)goto _LL4FE;else{_tmp9B6=_tmp9B5->f1;}}_LL4FD: {
# 3053
struct Cyc_Absyn_Exp*_tmp9E2=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B6);
# 3054
if(_tmp9E2 == _tmp9B6)return e;{
# 3055
struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp138A;struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp1389;return Cyc_Tcutil_copye(e,(void*)((_tmp1389=_cycalloc(sizeof(*_tmp1389)),((_tmp1389[0]=((_tmp138A.tag=17,((_tmp138A.f1=_tmp9E2,_tmp138A)))),_tmp1389)))));};}_LL4FE: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp9B7=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp9B7->tag != 18)goto _LL500;else{_tmp9B8=(void*)_tmp9B7->f1;_tmp9B9=(void*)_tmp9B7->f2;}}_LL4FF: {
# 3057
void*_tmp9E5=Cyc_Tcutil_rsubstitute(r,inst,_tmp9B8);
# 3058
if(_tmp9E5 == _tmp9B8)return e;{
# 3059
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp138D;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp138C;return Cyc_Tcutil_copye(e,(void*)((_tmp138C=_cycalloc(sizeof(*_tmp138C)),((_tmp138C[0]=((_tmp138D.tag=18,((_tmp138D.f1=(void*)_tmp9E5,((_tmp138D.f2=(void*)_tmp9B9,_tmp138D)))))),_tmp138C)))));};}_LL500: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp9BA=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp9BA->tag != 37)goto _LL502;else{_tmp9BB=(void*)_tmp9BA->f1;}}_LL501: {
# 3061
void*_tmp9E8=Cyc_Tcutil_rsubstitute(r,inst,_tmp9BB);
# 3062
if(_tmp9E8 == _tmp9BB)return e;{
# 3063
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp1390;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp138F;return Cyc_Tcutil_copye(e,(void*)((_tmp138F=_cycalloc(sizeof(*_tmp138F)),((_tmp138F[0]=((_tmp1390.tag=37,((_tmp1390.f1=(void*)_tmp9E8,_tmp1390)))),_tmp138F)))));};}_LL502:;_LL503: {
# 3065
const char*_tmp1393;void*_tmp1392;return(_tmp1392=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1393="non-type-level-expression in Tcutil::rsubsexp",_tag_dyneither(_tmp1393,sizeof(char),46))),_tag_dyneither(_tmp1392,sizeof(void*),0)));}_LL4E5:;}
# 3069
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3072
void*_tmp9ED=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp9EF;struct Cyc_Absyn_AggrInfo _tmp9F1;union Cyc_Absyn_AggrInfoU _tmp9F2;struct Cyc_List_List*_tmp9F3;struct Cyc_Absyn_DatatypeInfo _tmp9F5;union Cyc_Absyn_DatatypeInfoU _tmp9F6;struct Cyc_List_List*_tmp9F7;struct Cyc_Absyn_DatatypeFieldInfo _tmp9F9;union Cyc_Absyn_DatatypeFieldInfoU _tmp9FA;struct Cyc_List_List*_tmp9FB;struct _tuple2*_tmp9FD;struct Cyc_List_List*_tmp9FE;struct Cyc_Absyn_Typedefdecl*_tmp9FF;void*_tmpA00;struct Cyc_Absyn_ArrayInfo _tmpA02;void*_tmpA03;struct Cyc_Absyn_Tqual _tmpA04;struct Cyc_Absyn_Exp*_tmpA05;union Cyc_Absyn_Constraint*_tmpA06;unsigned int _tmpA07;struct Cyc_Absyn_PtrInfo _tmpA09;void*_tmpA0A;struct Cyc_Absyn_Tqual _tmpA0B;struct Cyc_Absyn_PtrAtts _tmpA0C;void*_tmpA0D;union Cyc_Absyn_Constraint*_tmpA0E;union Cyc_Absyn_Constraint*_tmpA0F;union Cyc_Absyn_Constraint*_tmpA10;struct Cyc_Absyn_FnInfo _tmpA12;struct Cyc_List_List*_tmpA13;void*_tmpA14;struct Cyc_Absyn_Tqual _tmpA15;void*_tmpA16;struct Cyc_List_List*_tmpA17;int _tmpA18;struct Cyc_Absyn_VarargInfo*_tmpA19;struct Cyc_List_List*_tmpA1A;struct Cyc_List_List*_tmpA1B;struct Cyc_List_List*_tmpA1D;enum Cyc_Absyn_AggrKind _tmpA1F;struct Cyc_List_List*_tmpA20;void*_tmpA22;void*_tmpA24;void*_tmpA26;void*_tmpA27;void*_tmpA29;struct Cyc_Absyn_Exp*_tmpA2B;void*_tmpA35;void*_tmpA37;struct Cyc_List_List*_tmpA39;_LL505: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9EE=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9ED;if(_tmp9EE->tag != 2)goto _LL507;else{_tmp9EF=_tmp9EE->f1;}}_LL506: {
# 3075
struct _handler_cons _tmpA3B;_push_handler(& _tmpA3B);{int _tmpA3D=0;if(setjmp(_tmpA3B.handler))_tmpA3D=1;if(!_tmpA3D){{void*_tmpA3E=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp9EF);_npop_handler(0);return _tmpA3E;};_pop_handler();}else{void*_tmpA3C=(void*)_exn_thrown;void*_tmpA40=_tmpA3C;_LL53C: {struct Cyc_Core_Not_found_exn_struct*_tmpA41=(struct Cyc_Core_Not_found_exn_struct*)_tmpA40;if(_tmpA41->tag != Cyc_Core_Not_found)goto _LL53E;}_LL53D:
# 3076
 return t;_LL53E:;_LL53F:(void)_throw(_tmpA40);_LL53B:;}};}_LL507: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp9F0=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9ED;if(_tmp9F0->tag != 11)goto _LL509;else{_tmp9F1=_tmp9F0->f1;_tmp9F2=_tmp9F1.aggr_info;_tmp9F3=_tmp9F1.targs;}}_LL508: {
# 3078
struct Cyc_List_List*_tmpA42=Cyc_Tcutil_substs(rgn,inst,_tmp9F3);
# 3079
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp1399;struct Cyc_Absyn_AggrInfo _tmp1398;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1397;return _tmpA42 == _tmp9F3?t:(void*)((_tmp1397=_cycalloc(sizeof(*_tmp1397)),((_tmp1397[0]=((_tmp1399.tag=11,((_tmp1399.f1=((_tmp1398.aggr_info=_tmp9F2,((_tmp1398.targs=_tmpA42,_tmp1398)))),_tmp1399)))),_tmp1397))));}_LL509: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp9F4=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp9ED;if(_tmp9F4->tag != 3)goto _LL50B;else{_tmp9F5=_tmp9F4->f1;_tmp9F6=_tmp9F5.datatype_info;_tmp9F7=_tmp9F5.targs;}}_LL50A: {
# 3081
struct Cyc_List_List*_tmpA46=Cyc_Tcutil_substs(rgn,inst,_tmp9F7);
# 3082
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp139F;struct Cyc_Absyn_DatatypeInfo _tmp139E;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp139D;return _tmpA46 == _tmp9F7?t:(void*)(
# 3083
(_tmp139D=_cycalloc(sizeof(*_tmp139D)),((_tmp139D[0]=((_tmp139F.tag=3,((_tmp139F.f1=((_tmp139E.datatype_info=_tmp9F6,((_tmp139E.targs=_tmpA46,_tmp139E)))),_tmp139F)))),_tmp139D))));}_LL50B: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp9F8=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp9ED;if(_tmp9F8->tag != 4)goto _LL50D;else{_tmp9F9=_tmp9F8->f1;_tmp9FA=_tmp9F9.field_info;_tmp9FB=_tmp9F9.targs;}}_LL50C: {
# 3085
struct Cyc_List_List*_tmpA4A=Cyc_Tcutil_substs(rgn,inst,_tmp9FB);
# 3086
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp13A5;struct Cyc_Absyn_DatatypeFieldInfo _tmp13A4;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp13A3;return _tmpA4A == _tmp9FB?t:(void*)(
# 3087
(_tmp13A3=_cycalloc(sizeof(*_tmp13A3)),((_tmp13A3[0]=((_tmp13A5.tag=4,((_tmp13A5.f1=((_tmp13A4.field_info=_tmp9FA,((_tmp13A4.targs=_tmpA4A,_tmp13A4)))),_tmp13A5)))),_tmp13A3))));}_LL50D: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp9FC=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9ED;if(_tmp9FC->tag != 17)goto _LL50F;else{_tmp9FD=_tmp9FC->f1;_tmp9FE=_tmp9FC->f2;_tmp9FF=_tmp9FC->f3;_tmpA00=(void*)_tmp9FC->f4;}}_LL50E: {
# 3089
struct Cyc_List_List*_tmpA4E=Cyc_Tcutil_substs(rgn,inst,_tmp9FE);
# 3090
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp13A8;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp13A7;return _tmpA4E == _tmp9FE?t:(void*)((_tmp13A7=_cycalloc(sizeof(*_tmp13A7)),((_tmp13A7[0]=((_tmp13A8.tag=17,((_tmp13A8.f1=_tmp9FD,((_tmp13A8.f2=_tmpA4E,((_tmp13A8.f3=_tmp9FF,((_tmp13A8.f4=(void*)_tmpA00,_tmp13A8)))))))))),_tmp13A7))));}_LL50F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpA01=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9ED;if(_tmpA01->tag != 8)goto _LL511;else{_tmpA02=_tmpA01->f1;_tmpA03=_tmpA02.elt_type;_tmpA04=_tmpA02.tq;_tmpA05=_tmpA02.num_elts;_tmpA06=_tmpA02.zero_term;_tmpA07=_tmpA02.zt_loc;}}_LL510: {
# 3092
void*_tmpA51=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA03);
# 3093
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp13AE;struct Cyc_Absyn_ArrayInfo _tmp13AD;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp13AC;return _tmpA51 == _tmpA03?t:(void*)((_tmp13AC=_cycalloc(sizeof(*_tmp13AC)),((_tmp13AC[0]=((_tmp13AE.tag=8,((_tmp13AE.f1=((_tmp13AD.elt_type=_tmpA51,((_tmp13AD.tq=_tmpA04,((_tmp13AD.num_elts=_tmpA05,((_tmp13AD.zero_term=_tmpA06,((_tmp13AD.zt_loc=_tmpA07,_tmp13AD)))))))))),_tmp13AE)))),_tmp13AC))));}_LL511: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA08=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9ED;if(_tmpA08->tag != 5)goto _LL513;else{_tmpA09=_tmpA08->f1;_tmpA0A=_tmpA09.elt_typ;_tmpA0B=_tmpA09.elt_tq;_tmpA0C=_tmpA09.ptr_atts;_tmpA0D=_tmpA0C.rgn;_tmpA0E=_tmpA0C.nullable;_tmpA0F=_tmpA0C.bounds;_tmpA10=_tmpA0C.zero_term;}}_LL512: {
# 3095
void*_tmpA55=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA0A);
# 3096
void*_tmpA56=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA0D);
# 3097
union Cyc_Absyn_Constraint*_tmpA57=_tmpA0F;
# 3098
{void*_tmpA58=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpA0F);struct Cyc_Absyn_Exp*_tmpA5A;_LL541: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA59=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpA58;if(_tmpA59->tag != 1)goto _LL543;else{_tmpA5A=_tmpA59->f1;}}_LL542: {
# 3100
struct Cyc_Absyn_Exp*_tmpA5B=Cyc_Tcutil_rsubsexp(rgn,inst,_tmpA5A);
# 3101
if(_tmpA5B != _tmpA5A){
# 3102
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp13B1;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp13B0;_tmpA57=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp13B0=_cycalloc(sizeof(*_tmp13B0)),((_tmp13B0[0]=((_tmp13B1.tag=1,((_tmp13B1.f1=_tmpA5B,_tmp13B1)))),_tmp13B0)))));}
# 3103
goto _LL540;}_LL543:;_LL544:
# 3104
 goto _LL540;_LL540:;}
# 3106
if((_tmpA55 == _tmpA0A  && _tmpA56 == _tmpA0D) && _tmpA57 == _tmpA0F)
# 3107
return t;{
# 3108
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp13BB;struct Cyc_Absyn_PtrAtts _tmp13BA;struct Cyc_Absyn_PtrInfo _tmp13B9;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp13B8;return(void*)((_tmp13B8=_cycalloc(sizeof(*_tmp13B8)),((_tmp13B8[0]=((_tmp13BB.tag=5,((_tmp13BB.f1=((_tmp13B9.elt_typ=_tmpA55,((_tmp13B9.elt_tq=_tmpA0B,((_tmp13B9.ptr_atts=((_tmp13BA.rgn=_tmpA56,((_tmp13BA.nullable=_tmpA0E,((_tmp13BA.bounds=_tmpA57,((_tmp13BA.zero_term=_tmpA10,((_tmp13BA.ptrloc=0,_tmp13BA)))))))))),_tmp13B9)))))),_tmp13BB)))),_tmp13B8))));};}_LL513: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA11=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9ED;if(_tmpA11->tag != 9)goto _LL515;else{_tmpA12=_tmpA11->f1;_tmpA13=_tmpA12.tvars;_tmpA14=_tmpA12.effect;_tmpA15=_tmpA12.ret_tqual;_tmpA16=_tmpA12.ret_typ;_tmpA17=_tmpA12.args;_tmpA18=_tmpA12.c_varargs;_tmpA19=_tmpA12.cyc_varargs;_tmpA1A=_tmpA12.rgn_po;_tmpA1B=_tmpA12.attributes;}}_LL514:
# 3111
{struct Cyc_List_List*_tmpA62=_tmpA13;for(0;_tmpA62 != 0;_tmpA62=_tmpA62->tl){
# 3112
struct _tuple16*_tmp13C5;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp13C4;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp13C3;struct Cyc_List_List*_tmp13C2;inst=((_tmp13C2=_region_malloc(rgn,sizeof(*_tmp13C2)),((_tmp13C2->hd=((_tmp13C5=_region_malloc(rgn,sizeof(*_tmp13C5)),((_tmp13C5->f1=(struct Cyc_Absyn_Tvar*)_tmpA62->hd,((_tmp13C5->f2=(void*)((_tmp13C3=_cycalloc(sizeof(*_tmp13C3)),((_tmp13C3[0]=((_tmp13C4.tag=2,((_tmp13C4.f1=(struct Cyc_Absyn_Tvar*)_tmpA62->hd,_tmp13C4)))),_tmp13C3)))),_tmp13C5)))))),((_tmp13C2->tl=inst,_tmp13C2))))));}}{
# 3113
struct Cyc_List_List*_tmpA68;struct Cyc_List_List*_tmpA69;struct _tuple1 _tmpA67=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
# 3114
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmpA17));_tmpA68=_tmpA67.f1;_tmpA69=_tmpA67.f2;{
# 3115
struct Cyc_List_List*_tmpA6A=_tmpA17;
# 3116
struct Cyc_List_List*_tmpA6B=Cyc_Tcutil_substs(rgn,inst,_tmpA69);
# 3117
if(_tmpA6B != _tmpA69)
# 3118
_tmpA6A=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmpA17,_tmpA6B);{
# 3119
void*eff2;
# 3120
if(_tmpA14 == 0)
# 3121
eff2=0;else{
# 3123
void*new_eff=(void*)Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmpA14);
# 3124
if(new_eff == _tmpA14)
# 3125
eff2=_tmpA14;else{
# 3127
eff2=new_eff;}}{
# 3129
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
# 3130
if(_tmpA19 == 0)
# 3131
cyc_varargs2=0;else{
# 3133
struct _dyneither_ptr*_tmpA6D;struct Cyc_Absyn_Tqual _tmpA6E;void*_tmpA6F;int _tmpA70;struct Cyc_Absyn_VarargInfo _tmpA6C=*_tmpA19;_tmpA6D=_tmpA6C.name;_tmpA6E=_tmpA6C.tq;_tmpA6F=_tmpA6C.type;_tmpA70=_tmpA6C.inject;{
# 3134
void*_tmpA71=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA6F);
# 3135
if(_tmpA71 == _tmpA6F)cyc_varargs2=_tmpA19;else{
# 3137
struct Cyc_Absyn_VarargInfo*_tmp13C6;cyc_varargs2=((_tmp13C6=_cycalloc(sizeof(*_tmp13C6)),((_tmp13C6->name=_tmpA6D,((_tmp13C6->tq=_tmpA6E,((_tmp13C6->type=_tmpA71,((_tmp13C6->inject=_tmpA70,_tmp13C6))))))))));}};}{
# 3139
struct Cyc_List_List*rgn_po2;
# 3140
struct Cyc_List_List*_tmpA74;struct Cyc_List_List*_tmpA75;struct _tuple1 _tmpA73=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,_tmpA1A);_tmpA74=_tmpA73.f1;_tmpA75=_tmpA73.f2;{
# 3141
struct Cyc_List_List*_tmpA76=Cyc_Tcutil_substs(rgn,inst,_tmpA74);
# 3142
struct Cyc_List_List*_tmpA77=Cyc_Tcutil_substs(rgn,inst,_tmpA75);
# 3143
if(_tmpA76 == _tmpA74  && _tmpA77 == _tmpA75)
# 3144
rgn_po2=_tmpA1A;else{
# 3146
rgn_po2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmpA76,_tmpA77);}{
# 3147
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp13CC;struct Cyc_Absyn_FnInfo _tmp13CB;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp13CA;return(void*)(
# 3148
(_tmp13CA=_cycalloc(sizeof(*_tmp13CA)),((_tmp13CA[0]=((_tmp13CC.tag=9,((_tmp13CC.f1=((_tmp13CB.tvars=_tmpA13,((_tmp13CB.effect=eff2,((_tmp13CB.ret_tqual=_tmpA15,((_tmp13CB.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA16),((_tmp13CB.args=_tmpA6A,((_tmp13CB.c_varargs=_tmpA18,((_tmp13CB.cyc_varargs=cyc_varargs2,((_tmp13CB.rgn_po=rgn_po2,((_tmp13CB.attributes=_tmpA1B,_tmp13CB)))))))))))))))))),_tmp13CC)))),_tmp13CA))));};};};};};};};_LL515: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpA1C=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp9ED;if(_tmpA1C->tag != 10)goto _LL517;else{_tmpA1D=_tmpA1C->f1;}}_LL516: {
# 3151
struct Cyc_List_List*ts2=0;
# 3152
int change=0;
# 3153
{struct Cyc_List_List*_tmpA7B=_tmpA1D;for(0;_tmpA7B != 0;_tmpA7B=_tmpA7B->tl){
# 3154
void*_tmpA7C=(*((struct _tuple12*)_tmpA7B->hd)).f2;
# 3155
void*_tmpA7D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA7C);
# 3156
if(_tmpA7C != _tmpA7D)
# 3157
change=1;{
# 3159
struct Cyc_List_List*_tmp13CD;ts2=((_tmp13CD=_region_malloc(rgn,sizeof(*_tmp13CD)),((_tmp13CD->hd=_tmpA7D,((_tmp13CD->tl=ts2,_tmp13CD))))));};}}
# 3161
if(!change)
# 3162
return t;{
# 3163
struct Cyc_List_List*_tmpA7F=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmpA1D,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
# 3164
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp13D0;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp13CF;return(void*)((_tmp13CF=_cycalloc(sizeof(*_tmp13CF)),((_tmp13CF[0]=((_tmp13D0.tag=10,((_tmp13D0.f1=_tmpA7F,_tmp13D0)))),_tmp13CF))));};}_LL517: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpA1E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp9ED;if(_tmpA1E->tag != 12)goto _LL519;else{_tmpA1F=_tmpA1E->f1;_tmpA20=_tmpA1E->f2;}}_LL518: {
# 3166
struct Cyc_List_List*_tmpA82=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,_tmpA20);
# 3167
struct Cyc_List_List*_tmpA83=Cyc_Tcutil_substs(rgn,inst,_tmpA82);
# 3168
if(_tmpA83 == _tmpA82)
# 3169
return t;{
# 3170
struct Cyc_List_List*_tmpA84=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmpA20,_tmpA83);
# 3171
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp13D3;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp13D2;return(void*)((_tmp13D2=_cycalloc(sizeof(*_tmp13D2)),((_tmp13D2[0]=((_tmp13D3.tag=12,((_tmp13D3.f1=_tmpA1F,((_tmp13D3.f2=_tmpA84,_tmp13D3)))))),_tmp13D2))));};}_LL519: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpA21=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp9ED;if(_tmpA21->tag != 1)goto _LL51B;else{_tmpA22=(void*)_tmpA21->f2;}}_LL51A:
# 3173
 if(_tmpA22 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmpA22);else{
# 3174
return t;}_LL51B: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA23=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp9ED;if(_tmpA23->tag != 15)goto _LL51D;else{_tmpA24=(void*)_tmpA23->f1;}}_LL51C: {
# 3176
void*_tmpA87=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA24);
# 3177
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp13D6;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp13D5;return _tmpA87 == _tmpA24?t:(void*)((_tmp13D5=_cycalloc(sizeof(*_tmp13D5)),((_tmp13D5[0]=((_tmp13D6.tag=15,((_tmp13D6.f1=(void*)_tmpA87,_tmp13D6)))),_tmp13D5))));}_LL51D: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpA25=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp9ED;if(_tmpA25->tag != 16)goto _LL51F;else{_tmpA26=(void*)_tmpA25->f1;_tmpA27=(void*)_tmpA25->f2;}}_LL51E: {
# 3179
void*_tmpA8A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA26);
# 3180
void*_tmpA8B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA27);
# 3181
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp13D9;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp13D8;return _tmpA8A == _tmpA26  && _tmpA8B == _tmpA27?t:(void*)((_tmp13D8=_cycalloc(sizeof(*_tmp13D8)),((_tmp13D8[0]=((_tmp13D9.tag=16,((_tmp13D9.f1=(void*)_tmpA8A,((_tmp13D9.f2=(void*)_tmpA8B,_tmp13D9)))))),_tmp13D8))));}_LL51F: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpA28=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp9ED;if(_tmpA28->tag != 19)goto _LL521;else{_tmpA29=(void*)_tmpA28->f1;}}_LL520: {
# 3183
void*_tmpA8E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA29);
# 3184
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp13DC;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp13DB;return _tmpA8E == _tmpA29?t:(void*)((_tmp13DB=_cycalloc(sizeof(*_tmp13DB)),((_tmp13DB[0]=((_tmp13DC.tag=19,((_tmp13DC.f1=(void*)_tmpA8E,_tmp13DC)))),_tmp13DB))));}_LL521: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpA2A=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp9ED;if(_tmpA2A->tag != 18)goto _LL523;else{_tmpA2B=_tmpA2A->f1;}}_LL522: {
# 3186
struct Cyc_Absyn_Exp*_tmpA91=Cyc_Tcutil_rsubsexp(rgn,inst,_tmpA2B);
# 3187
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp13DF;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp13DE;return _tmpA91 == _tmpA2B?t:(void*)((_tmp13DE=_cycalloc(sizeof(*_tmp13DE)),((_tmp13DE[0]=((_tmp13DF.tag=18,((_tmp13DF.f1=_tmpA91,_tmp13DF)))),_tmp13DE))));}_LL523: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpA2C=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp9ED;if(_tmpA2C->tag != 13)goto _LL525;}_LL524:
# 3188
 goto _LL526;_LL525: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpA2D=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp9ED;if(_tmpA2D->tag != 14)goto _LL527;}_LL526:
# 3189
 goto _LL528;_LL527: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpA2E=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp9ED;if(_tmpA2E->tag != 0)goto _LL529;}_LL528:
# 3190
 goto _LL52A;_LL529: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpA2F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp9ED;if(_tmpA2F->tag != 6)goto _LL52B;}_LL52A:
# 3191
 goto _LL52C;_LL52B: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpA30=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp9ED;if(_tmpA30->tag != 7)goto _LL52D;}_LL52C:
# 3192
 goto _LL52E;_LL52D: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpA31=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp9ED;if(_tmpA31->tag != 22)goto _LL52F;}_LL52E:
# 3193
 goto _LL530;_LL52F: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpA32=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp9ED;if(_tmpA32->tag != 21)goto _LL531;}_LL530:
# 3194
 goto _LL532;_LL531: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpA33=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp9ED;if(_tmpA33->tag != 20)goto _LL533;}_LL532:
# 3195
 return t;_LL533: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpA34=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp9ED;if(_tmpA34->tag != 25)goto _LL535;else{_tmpA35=(void*)_tmpA34->f1;}}_LL534: {
# 3197
void*_tmpA94=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA35);
# 3198
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp13E2;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp13E1;return _tmpA94 == _tmpA35?t:(void*)((_tmp13E1=_cycalloc(sizeof(*_tmp13E1)),((_tmp13E1[0]=((_tmp13E2.tag=25,((_tmp13E2.f1=(void*)_tmpA94,_tmp13E2)))),_tmp13E1))));}_LL535: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpA36=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp9ED;if(_tmpA36->tag != 23)goto _LL537;else{_tmpA37=(void*)_tmpA36->f1;}}_LL536: {
# 3200
void*_tmpA97=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA37);
# 3201
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp13E5;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp13E4;return _tmpA97 == _tmpA37?t:(void*)((_tmp13E4=_cycalloc(sizeof(*_tmp13E4)),((_tmp13E4[0]=((_tmp13E5.tag=23,((_tmp13E5.f1=(void*)_tmpA97,_tmp13E5)))),_tmp13E4))));}_LL537: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpA38=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp9ED;if(_tmpA38->tag != 24)goto _LL539;else{_tmpA39=_tmpA38->f1;}}_LL538: {
# 3203
struct Cyc_List_List*_tmpA9A=Cyc_Tcutil_substs(rgn,inst,_tmpA39);
# 3204
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp13E8;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp13E7;return _tmpA9A == _tmpA39?t:(void*)((_tmp13E7=_cycalloc(sizeof(*_tmp13E7)),((_tmp13E7[0]=((_tmp13E8.tag=24,((_tmp13E8.f1=_tmpA9A,_tmp13E8)))),_tmp13E7))));}_LL539: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpA3A=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp9ED;if(_tmpA3A->tag != 26)goto _LL504;}_LL53A: {
# 3205
const char*_tmp13EB;void*_tmp13EA;(_tmp13EA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13EB="found typedecltype in rsubs",_tag_dyneither(_tmp13EB,sizeof(char),28))),_tag_dyneither(_tmp13EA,sizeof(void*),0)));}_LL504:;}
# 3209
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3212
if(ts == 0)
# 3213
return 0;{
# 3214
void*_tmpA9F=(void*)ts->hd;
# 3215
struct Cyc_List_List*_tmpAA0=ts->tl;
# 3216
void*_tmpAA1=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA9F);
# 3217
struct Cyc_List_List*_tmpAA2=Cyc_Tcutil_substs(rgn,inst,_tmpAA0);
# 3218
if(_tmpA9F == _tmpAA1  && _tmpAA0 == _tmpAA2)
# 3219
return ts;{
# 3220
struct Cyc_List_List*_tmp13EC;return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmp13EC=_cycalloc(sizeof(*_tmp13EC)),((_tmp13EC->hd=_tmpAA1,((_tmp13EC->tl=_tmpAA2,_tmp13EC)))))));};};}
# 3223
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
# 3224
if(inst != 0)
# 3225
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
# 3226
return t;}}
# 3230
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
# 3231
struct Cyc_Core_Opt*_tmpAA4=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
# 3232
struct Cyc_Core_Opt*_tmp13EF;struct _tuple16*_tmp13EE;return(_tmp13EE=_cycalloc(sizeof(*_tmp13EE)),((_tmp13EE->f1=tv,((_tmp13EE->f2=Cyc_Absyn_new_evar(_tmpAA4,((_tmp13EF=_cycalloc(sizeof(*_tmp13EF)),((_tmp13EF->v=s,_tmp13EF))))),_tmp13EE)))));}
# 3235
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3237
struct _tuple17 _tmpAA8;struct Cyc_List_List*_tmpAA9;struct _RegionHandle*_tmpAAA;struct _tuple17*_tmpAA7=env;_tmpAA8=*_tmpAA7;_tmpAA9=_tmpAA8.f1;_tmpAAA=_tmpAA8.f2;{
# 3238
struct Cyc_Core_Opt*_tmpAAB=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
# 3239
struct Cyc_Core_Opt*_tmp13F2;struct _tuple16*_tmp13F1;return(_tmp13F1=_region_malloc(_tmpAAA,sizeof(*_tmp13F1)),((_tmp13F1->f1=tv,((_tmp13F1->f2=Cyc_Absyn_new_evar(_tmpAAB,((_tmp13F2=_cycalloc(sizeof(*_tmp13F2)),((_tmp13F2->v=_tmpAA9,_tmp13F2))))),_tmp13F1)))));};}
# 3247
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3251
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3252
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
# 3253
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
# 3254
void*k2=tv->kind;
# 3255
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){
# 3256
const char*_tmp13F8;void*_tmp13F7[3];struct Cyc_String_pa_PrintArg_struct _tmp13F6;struct Cyc_String_pa_PrintArg_struct _tmp13F5;struct Cyc_String_pa_PrintArg_struct _tmp13F4;(_tmp13F4.tag=0,((_tmp13F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3257
Cyc_Absynpp_kindbound2string(k2)),((_tmp13F5.tag=0,((_tmp13F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((_tmp13F6.tag=0,((_tmp13F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp13F7[0]=& _tmp13F6,((_tmp13F7[1]=& _tmp13F5,((_tmp13F7[2]=& _tmp13F4,Cyc_Tcutil_terr(loc,((_tmp13F8="type variable %s is used with inconsistent kinds %s and %s",_tag_dyneither(_tmp13F8,sizeof(char),59))),_tag_dyneither(_tmp13F7,sizeof(void*),3)))))))))))))))))));}
# 3258
if(tv->identity == - 1)
# 3259
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
# 3260
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
# 3261
const char*_tmp13FB;void*_tmp13FA;(_tmp13FA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13FB="same type variable has different identity!",_tag_dyneither(_tmp13FB,sizeof(char),43))),_tag_dyneither(_tmp13FA,sizeof(void*),0)));}}
# 3262
return tvs;}}}
# 3265
tv->identity=Cyc_Tcutil_new_tvar_id();{
# 3266
struct Cyc_List_List*_tmp13FC;return(_tmp13FC=_cycalloc(sizeof(*_tmp13FC)),((_tmp13FC->hd=tv,((_tmp13FC->tl=tvs,_tmp13FC)))));};}
# 3271
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3273
if(tv->identity == - 1){
# 3274
const char*_tmp13FF;void*_tmp13FE;(_tmp13FE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13FF="fast_add_free_tvar: bad identity in tv",_tag_dyneither(_tmp13FF,sizeof(char),39))),_tag_dyneither(_tmp13FE,sizeof(void*),0)));}
# 3275
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3277
struct Cyc_Absyn_Tvar*_tmpAB8=(struct Cyc_Absyn_Tvar*)tvs2->hd;
# 3278
if(_tmpAB8->identity == - 1){
# 3279
const char*_tmp1402;void*_tmp1401;(_tmp1401=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1402="fast_add_free_tvar: bad identity in tvs2",_tag_dyneither(_tmp1402,sizeof(char),41))),_tag_dyneither(_tmp1401,sizeof(void*),0)));}
# 3280
if(_tmpAB8->identity == tv->identity)
# 3281
return tvs;}}{
# 3284
struct Cyc_List_List*_tmp1403;return(_tmp1403=_cycalloc(sizeof(*_tmp1403)),((_tmp1403->hd=tv,((_tmp1403->tl=tvs,_tmp1403)))));};}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3290
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3295
if(tv->identity == - 1){
# 3296
const char*_tmp1406;void*_tmp1405;(_tmp1405=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1406="fast_add_free_tvar_bool: bad identity in tv",_tag_dyneither(_tmp1406,sizeof(char),44))),_tag_dyneither(_tmp1405,sizeof(void*),0)));}
# 3297
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3299
struct _tuple28 _tmpABF;struct Cyc_Absyn_Tvar*_tmpAC0;int _tmpAC1;int*_tmpAC2;struct _tuple28*_tmpABE=(struct _tuple28*)tvs2->hd;_tmpABF=*_tmpABE;_tmpAC0=_tmpABF.f1;_tmpAC1=_tmpABF.f2;_tmpAC2=(int*)&(*_tmpABE).f2;
# 3300
if(_tmpAC0->identity == - 1){
# 3301
const char*_tmp1409;void*_tmp1408;(_tmp1408=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1409="fast_add_free_tvar_bool: bad identity in tvs2",_tag_dyneither(_tmp1409,sizeof(char),46))),_tag_dyneither(_tmp1408,sizeof(void*),0)));}
# 3302
if(_tmpAC0->identity == tv->identity){
# 3303
*_tmpAC2=*_tmpAC2  || b;
# 3304
return tvs;}}}{
# 3307
struct _tuple28*_tmp140C;struct Cyc_List_List*_tmp140B;return(_tmp140B=_region_malloc(r,sizeof(*_tmp140B)),((_tmp140B->hd=((_tmp140C=_region_malloc(r,sizeof(*_tmp140C)),((_tmp140C->f1=tv,((_tmp140C->f2=b,_tmp140C)))))),((_tmp140B->tl=tvs,_tmp140B)))));};}
# 3311
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3313
if(tv->identity == - 1){
# 3314
const char*_tmp1410;void*_tmp140F[1];struct Cyc_String_pa_PrintArg_struct _tmp140E;(_tmp140E.tag=0,((_tmp140E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv)),((_tmp140F[0]=& _tmp140E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1410="bound tvar id for %s is NULL",_tag_dyneither(_tmp1410,sizeof(char),29))),_tag_dyneither(_tmp140F,sizeof(void*),1)))))));}{
# 3315
struct Cyc_List_List*_tmp1411;return(_tmp1411=_cycalloc(sizeof(*_tmp1411)),((_tmp1411->hd=tv,((_tmp1411->tl=tvs,_tmp1411)))));};}struct _tuple29{void*f1;int f2;};
# 3322
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3325
void*_tmpACB=Cyc_Tcutil_compress(e);int _tmpACD;_LL546: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpACC=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpACB;if(_tmpACC->tag != 1)goto _LL548;else{_tmpACD=_tmpACC->f3;}}_LL547:
# 3327
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
# 3328
struct _tuple29 _tmpACF;void*_tmpAD0;int _tmpAD1;int*_tmpAD2;struct _tuple29*_tmpACE=(struct _tuple29*)es2->hd;_tmpACF=*_tmpACE;_tmpAD0=_tmpACF.f1;_tmpAD1=_tmpACF.f2;_tmpAD2=(int*)&(*_tmpACE).f2;{
# 3329
void*_tmpAD3=Cyc_Tcutil_compress(_tmpAD0);int _tmpAD5;_LL54B: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpAD4=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpAD3;if(_tmpAD4->tag != 1)goto _LL54D;else{_tmpAD5=_tmpAD4->f3;}}_LL54C:
# 3331
 if(_tmpACD == _tmpAD5){
# 3332
if(b != *_tmpAD2)*_tmpAD2=1;
# 3333
return es;}
# 3335
goto _LL54A;_LL54D:;_LL54E:
# 3336
 goto _LL54A;_LL54A:;};}}{
# 3339
struct _tuple29*_tmp1414;struct Cyc_List_List*_tmp1413;return(_tmp1413=_region_malloc(r,sizeof(*_tmp1413)),((_tmp1413->hd=((_tmp1414=_region_malloc(r,sizeof(*_tmp1414)),((_tmp1414->f1=e,((_tmp1414->f2=b,_tmp1414)))))),((_tmp1413->tl=es,_tmp1413)))));};_LL548:;_LL549:
# 3340
 return es;_LL545:;}
# 3344
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3347
struct Cyc_List_List*r=0;
# 3348
for(0;tvs != 0;tvs=tvs->tl){
# 3349
int present=0;
# 3350
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
# 3351
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
# 3352
present=1;
# 3353
break;}}}
# 3356
if(!present){struct Cyc_List_List*_tmp1415;r=((_tmp1415=_region_malloc(rgn,sizeof(*_tmp1415)),((_tmp1415->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1415->tl=r,_tmp1415))))));}}
# 3358
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
# 3359
return r;}
# 3363
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3367
struct Cyc_List_List*res=0;
# 3368
for(0;tvs != 0;tvs=tvs->tl){
# 3369
struct Cyc_Absyn_Tvar*_tmpADA;int _tmpADB;struct _tuple28 _tmpAD9=*((struct _tuple28*)tvs->hd);_tmpADA=_tmpAD9.f1;_tmpADB=_tmpAD9.f2;{
# 3370
int present=0;
# 3371
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
# 3372
if(_tmpADA->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
# 3373
present=1;
# 3374
break;}}}
# 3377
if(!present){struct Cyc_List_List*_tmp1416;res=((_tmp1416=_region_malloc(r,sizeof(*_tmp1416)),((_tmp1416->hd=(struct _tuple28*)tvs->hd,((_tmp1416->tl=res,_tmp1416))))));}};}
# 3379
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
# 3380
return res;}
# 3383
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3385
if(width != 0){
# 3386
unsigned int w=0;
# 3387
if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)width)){
# 3388
const char*_tmp141A;void*_tmp1419[1];struct Cyc_String_pa_PrintArg_struct _tmp1418;(_tmp1418.tag=0,((_tmp1418.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1419[0]=& _tmp1418,Cyc_Tcutil_terr(loc,((_tmp141A="bitfield %s does not have constant width",_tag_dyneither(_tmp141A,sizeof(char),41))),_tag_dyneither(_tmp1419,sizeof(void*),1)))))));}else{
# 3390
unsigned int _tmpAE1;int _tmpAE2;struct _tuple14 _tmpAE0=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)width);_tmpAE1=_tmpAE0.f1;_tmpAE2=_tmpAE0.f2;
# 3391
if(!_tmpAE2){
# 3392
const char*_tmp141D;void*_tmp141C;(_tmp141C=0,Cyc_Tcutil_terr(loc,((_tmp141D="bitfield width cannot use sizeof or offsetof",_tag_dyneither(_tmp141D,sizeof(char),45))),_tag_dyneither(_tmp141C,sizeof(void*),0)));}
# 3393
w=_tmpAE1;}{
# 3395
void*_tmpAE5=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of _tmpAE7;_LL550: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpAE6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpAE5;if(_tmpAE6->tag != 6)goto _LL552;else{_tmpAE7=_tmpAE6->f2;}}_LL551:
# 3398
 switch(_tmpAE7){case Cyc_Absyn_Char_sz: _LL554:
# 3399
 if(w > 8){const char*_tmp1420;void*_tmp141F;(_tmp141F=0,Cyc_Tcutil_terr(loc,((_tmp1420="bitfield larger than type",_tag_dyneither(_tmp1420,sizeof(char),26))),_tag_dyneither(_tmp141F,sizeof(void*),0)));}break;case Cyc_Absyn_Short_sz: _LL555:
# 3400
 if(w > 16){const char*_tmp1423;void*_tmp1422;(_tmp1422=0,Cyc_Tcutil_terr(loc,((_tmp1423="bitfield larger than type",_tag_dyneither(_tmp1423,sizeof(char),26))),_tag_dyneither(_tmp1422,sizeof(void*),0)));}break;case Cyc_Absyn_Long_sz: _LL556:
# 3401
 goto _LL557;case Cyc_Absyn_Int_sz: _LL557:
# 3403
 if(w > 32){const char*_tmp1426;void*_tmp1425;(_tmp1425=0,Cyc_Tcutil_terr(loc,((_tmp1426="bitfield larger than type",_tag_dyneither(_tmp1426,sizeof(char),26))),_tag_dyneither(_tmp1425,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz: _LL558:
# 3405
 if(w > 64){const char*_tmp1429;void*_tmp1428;(_tmp1428=0,Cyc_Tcutil_terr(loc,((_tmp1429="bitfield larger than type",_tag_dyneither(_tmp1429,sizeof(char),26))),_tag_dyneither(_tmp1428,sizeof(void*),0)));}break;}
# 3407
goto _LL54F;_LL552:;_LL553:
# 3409
{const char*_tmp142E;void*_tmp142D[2];struct Cyc_String_pa_PrintArg_struct _tmp142C;struct Cyc_String_pa_PrintArg_struct _tmp142B;(_tmp142B.tag=0,((_tmp142B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3410
Cyc_Absynpp_typ2string(field_typ)),((_tmp142C.tag=0,((_tmp142C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp142D[0]=& _tmp142C,((_tmp142D[1]=& _tmp142B,Cyc_Tcutil_terr(loc,((_tmp142E="bitfield %s must have integral type but has type %s",_tag_dyneither(_tmp142E,sizeof(char),52))),_tag_dyneither(_tmp142D,sizeof(void*),2)))))))))))));}
# 3411
goto _LL54F;_LL54F:;};}}
# 3416
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
# 3417
for(0;atts != 0;atts=atts->tl){
# 3418
void*_tmpAF4=(void*)atts->hd;_LL55B: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmpAF5=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmpAF4;if(_tmpAF5->tag != 7)goto _LL55D;}_LL55C:
# 3419
 continue;_LL55D: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpAF6=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmpAF4;if(_tmpAF6->tag != 6)goto _LL55F;}_LL55E:
# 3420
 continue;_LL55F:;_LL560: {
# 3421
const char*_tmp1433;void*_tmp1432[2];struct Cyc_String_pa_PrintArg_struct _tmp1431;struct Cyc_String_pa_PrintArg_struct _tmp1430;(_tmp1430.tag=0,((_tmp1430.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1431.tag=0,((_tmp1431.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3422
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp1432[0]=& _tmp1431,((_tmp1432[1]=& _tmp1430,Cyc_Tcutil_terr(loc,((_tmp1433="bad attribute %s on member %s",_tag_dyneither(_tmp1433,sizeof(char),30))),_tag_dyneither(_tmp1432,sizeof(void*),2)))))))))))));}_LL55A:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3440
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3444
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
# 3445
void*_tmpAFB=t;struct Cyc_Absyn_Typedefdecl*_tmpAFD;void*_tmpAFE;_LL562: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAFC=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpAFB;if(_tmpAFC->tag != 17)goto _LL564;else{_tmpAFD=_tmpAFC->f3;_tmpAFE=(void*)_tmpAFC->f4;}}_LL563:
# 3447
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmpAFD))->tq).real_const  || (_tmpAFD->tq).print_const){
# 3448
if(declared_const){const char*_tmp1436;void*_tmp1435;(_tmp1435=0,Cyc_Tcutil_warn(loc,((_tmp1436="extra const",_tag_dyneither(_tmp1436,sizeof(char),12))),_tag_dyneither(_tmp1435,sizeof(void*),0)));}
# 3449
return 1;}
# 3452
if((unsigned int)_tmpAFE)
# 3453
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,(void*)_tmpAFE);else{
# 3454
return declared_const;}_LL564:;_LL565:
# 3455
 return declared_const;_LL561:;}
# 3459
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
# 3460
if(td != 0){
# 3461
if(td->defn != 0){
# 3462
void*_tmpB01=Cyc_Tcutil_compress((void*)_check_null(td->defn));struct Cyc_Absyn_PtrInfo _tmpB03;struct Cyc_Absyn_PtrAtts _tmpB04;void*_tmpB05;_LL567: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpB02=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB01;if(_tmpB02->tag != 5)goto _LL569;else{_tmpB03=_tmpB02->f1;_tmpB04=_tmpB03.ptr_atts;_tmpB05=_tmpB04.rgn;}}_LL568:
# 3464
{void*_tmpB06=Cyc_Tcutil_compress(_tmpB05);struct Cyc_Absyn_Tvar*_tmpB08;_LL56C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpB07=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpB06;if(_tmpB07->tag != 2)goto _LL56E;else{_tmpB08=_tmpB07->f1;}}_LL56D:
# 3466
 return Cyc_Absyn_tvar_cmp(tvar,_tmpB08)== 0;_LL56E:;_LL56F:
# 3467
 goto _LL56B;_LL56B:;}
# 3469
goto _LL566;_LL569:;_LL56A:
# 3470
 goto _LL566;_LL566:;}}else{
# 3475
return 1;}
# 3476
return 0;}
# 3479
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3482
void*_tmpB09=Cyc_Absyn_compress_kb(tvar->kind);struct Cyc_Absyn_Kind*_tmpB0B;struct Cyc_Absyn_Kind _tmpB0C;enum Cyc_Absyn_KindQual _tmpB0D;enum Cyc_Absyn_AliasQual _tmpB0E;struct Cyc_Absyn_Kind*_tmpB10;struct Cyc_Absyn_Kind _tmpB11;enum Cyc_Absyn_KindQual _tmpB12;enum Cyc_Absyn_AliasQual _tmpB13;_LL571: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB0A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB09;if(_tmpB0A->tag != 2)goto _LL573;else{_tmpB0B=_tmpB0A->f2;_tmpB0C=*_tmpB0B;_tmpB0D=_tmpB0C.kind;if(_tmpB0D != Cyc_Absyn_RgnKind)goto _LL573;_tmpB0E=_tmpB0C.aliasqual;if(_tmpB0E != Cyc_Absyn_Top)goto _LL573;}}_LL572:
# 3483
 goto _LL574;_LL573: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB0F=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB09;if(_tmpB0F->tag != 0)goto _LL575;else{_tmpB10=_tmpB0F->f1;_tmpB11=*_tmpB10;_tmpB12=_tmpB11.kind;if(_tmpB12 != Cyc_Absyn_RgnKind)goto _LL575;_tmpB13=_tmpB11.aliasqual;if(_tmpB13 != Cyc_Absyn_Top)goto _LL575;}}_LL574:
# 3491
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3494
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
# 3495
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
# 3496
return& Cyc_Tcutil_rk;else{
# 3497
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
# 3498
return& Cyc_Tcutil_urk;}}
# 3500
return& Cyc_Tcutil_trk;_LL575:;_LL576:
# 3501
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);_LL570:;}
# 3535 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3538
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect){
# 3545
{void*_tmpB14=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpB17;struct Cyc_Core_Opt**_tmpB18;void*_tmpB19;void**_tmpB1A;struct Cyc_Absyn_Tvar*_tmpB1C;struct Cyc_Absyn_TypeDecl*_tmpB1E;struct Cyc_Absyn_TypeDecl _tmpB1F;void*_tmpB20;void**_tmpB21;void***_tmpB22;struct Cyc_List_List*_tmpB24;struct _tuple2*_tmpB26;struct Cyc_Absyn_Enumdecl*_tmpB27;struct Cyc_Absyn_Enumdecl**_tmpB28;struct Cyc_Absyn_DatatypeInfo _tmpB2A;union Cyc_Absyn_DatatypeInfoU _tmpB2B;union Cyc_Absyn_DatatypeInfoU*_tmpB2C;struct Cyc_List_List*_tmpB2D;struct Cyc_List_List**_tmpB2E;struct Cyc_Absyn_DatatypeFieldInfo _tmpB30;union Cyc_Absyn_DatatypeFieldInfoU _tmpB31;union Cyc_Absyn_DatatypeFieldInfoU*_tmpB32;struct Cyc_List_List*_tmpB33;struct Cyc_Absyn_PtrInfo _tmpB35;void*_tmpB36;struct Cyc_Absyn_Tqual _tmpB37;struct Cyc_Absyn_Tqual*_tmpB38;struct Cyc_Absyn_PtrAtts _tmpB39;void*_tmpB3A;union Cyc_Absyn_Constraint*_tmpB3B;union Cyc_Absyn_Constraint*_tmpB3C;union Cyc_Absyn_Constraint*_tmpB3D;void*_tmpB3F;struct Cyc_Absyn_Exp*_tmpB41;struct Cyc_Absyn_ArrayInfo _tmpB45;void*_tmpB46;struct Cyc_Absyn_Tqual _tmpB47;struct Cyc_Absyn_Tqual*_tmpB48;struct Cyc_Absyn_Exp*_tmpB49;struct Cyc_Absyn_Exp**_tmpB4A;union Cyc_Absyn_Constraint*_tmpB4B;unsigned int _tmpB4C;struct Cyc_Absyn_FnInfo _tmpB4E;struct Cyc_List_List*_tmpB4F;struct Cyc_List_List**_tmpB50;void*_tmpB51;void**_tmpB52;struct Cyc_Absyn_Tqual _tmpB53;struct Cyc_Absyn_Tqual*_tmpB54;void*_tmpB55;struct Cyc_List_List*_tmpB56;int _tmpB57;struct Cyc_Absyn_VarargInfo*_tmpB58;struct Cyc_List_List*_tmpB59;struct Cyc_List_List*_tmpB5A;struct Cyc_List_List*_tmpB5C;enum Cyc_Absyn_AggrKind _tmpB5E;struct Cyc_List_List*_tmpB5F;struct Cyc_Absyn_AggrInfo _tmpB61;union Cyc_Absyn_AggrInfoU _tmpB62;union Cyc_Absyn_AggrInfoU*_tmpB63;struct Cyc_List_List*_tmpB64;struct Cyc_List_List**_tmpB65;struct _tuple2*_tmpB67;struct Cyc_List_List*_tmpB68;struct Cyc_List_List**_tmpB69;struct Cyc_Absyn_Typedefdecl*_tmpB6A;struct Cyc_Absyn_Typedefdecl**_tmpB6B;void*_tmpB6C;void**_tmpB6D;void*_tmpB72;void*_tmpB74;void*_tmpB75;void*_tmpB77;void*_tmpB79;struct Cyc_List_List*_tmpB7B;_LL578: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpB15=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpB14;if(_tmpB15->tag != 0)goto _LL57A;}_LL579:
# 3546
 goto _LL577;_LL57A: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpB16=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpB14;if(_tmpB16->tag != 1)goto _LL57C;else{_tmpB17=_tmpB16->f1;_tmpB18=(struct Cyc_Core_Opt**)& _tmpB16->f1;_tmpB19=(void**)& _tmpB16->f2;_tmpB1A=(void**)((void**)& _tmpB16->f2);}}_LL57B:
# 3549
 if(*_tmpB18 == 0  || 
# 3550
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmpB18))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmpB18))->v,expected_kind))
# 3551
*_tmpB18=Cyc_Tcutil_kind_to_opt(expected_kind);
# 3552
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3554
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
# 3555
*_tmpB1A=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3557
*_tmpB1A=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
# 3558
if(cvtenv.generalize_evars){
# 3559
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1439;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1438;struct Cyc_Absyn_Tvar*_tmpB7C=Cyc_Tcutil_new_tvar((void*)((_tmp1438=_cycalloc(sizeof(*_tmp1438)),((_tmp1438[0]=((_tmp1439.tag=2,((_tmp1439.f1=0,((_tmp1439.f2=expected_kind,_tmp1439)))))),_tmp1438)))));
# 3560
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp143C;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp143B;*_tmpB1A=(void*)((_tmp143B=_cycalloc(sizeof(*_tmp143B)),((_tmp143B[0]=((_tmp143C.tag=2,((_tmp143C.f1=_tmpB7C,_tmp143C)))),_tmp143B))));}
# 3561
_tmpB1C=_tmpB7C;goto _LL57D;}else{
# 3563
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3565
goto _LL577;_LL57C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpB1B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpB14;if(_tmpB1B->tag != 2)goto _LL57E;else{_tmpB1C=_tmpB1B->f1;}}_LL57D:
# 3567
{void*_tmpB81=Cyc_Absyn_compress_kb(_tmpB1C->kind);struct Cyc_Core_Opt*_tmpB83;struct Cyc_Core_Opt**_tmpB84;_LL5AF: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB82=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB81;if(_tmpB82->tag != 1)goto _LL5B1;else{_tmpB83=_tmpB82->f1;_tmpB84=(struct Cyc_Core_Opt**)& _tmpB82->f1;}}_LL5B0:
# 3569
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1442;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1441;struct Cyc_Core_Opt*_tmp1440;*_tmpB84=((_tmp1440=_cycalloc(sizeof(*_tmp1440)),((_tmp1440->v=(void*)((_tmp1442=_cycalloc(sizeof(*_tmp1442)),((_tmp1442[0]=((_tmp1441.tag=2,((_tmp1441.f1=0,((_tmp1441.f2=expected_kind,_tmp1441)))))),_tmp1442)))),_tmp1440))));}goto _LL5AE;_LL5B1:;_LL5B2:
# 3570
 goto _LL5AE;_LL5AE:;}
# 3574
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpB1C);
# 3577
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpB1C,put_in_effect);
# 3579
{void*_tmpB88=Cyc_Absyn_compress_kb(_tmpB1C->kind);struct Cyc_Core_Opt*_tmpB8A;struct Cyc_Core_Opt**_tmpB8B;struct Cyc_Absyn_Kind*_tmpB8C;_LL5B4: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB89=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB88;if(_tmpB89->tag != 2)goto _LL5B6;else{_tmpB8A=_tmpB89->f1;_tmpB8B=(struct Cyc_Core_Opt**)& _tmpB89->f1;_tmpB8C=_tmpB89->f2;}}_LL5B5:
# 3581
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmpB8C)){
# 3582
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1448;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1447;struct Cyc_Core_Opt*_tmp1446;*_tmpB8B=((_tmp1446=_cycalloc(sizeof(*_tmp1446)),((_tmp1446->v=(void*)((_tmp1448=_cycalloc(sizeof(*_tmp1448)),((_tmp1448[0]=((_tmp1447.tag=2,((_tmp1447.f1=0,((_tmp1447.f2=expected_kind,_tmp1447)))))),_tmp1448)))),_tmp1446))));}
# 3583
goto _LL5B3;_LL5B6:;_LL5B7:
# 3584
 goto _LL5B3;_LL5B3:;}
# 3586
goto _LL577;_LL57E: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB1D=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmpB14;if(_tmpB1D->tag != 26)goto _LL580;else{_tmpB1E=_tmpB1D->f1;_tmpB1F=*_tmpB1E;_tmpB20=_tmpB1F.r;_tmpB21=_tmpB1D->f2;_tmpB22=(void***)& _tmpB1D->f2;}}_LL57F: {
# 3592
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
# 3593
{void*_tmpB90=_tmpB20;struct Cyc_Absyn_Aggrdecl*_tmpB92;struct Cyc_Absyn_Enumdecl*_tmpB94;struct Cyc_Absyn_Datatypedecl*_tmpB96;_LL5B9: {struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpB91=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmpB90;if(_tmpB91->tag != 0)goto _LL5BB;else{_tmpB92=_tmpB91->f1;}}_LL5BA:
# 3595
 Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmpB92);goto _LL5B8;_LL5BB: {struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB93=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmpB90;if(_tmpB93->tag != 1)goto _LL5BD;else{_tmpB94=_tmpB93->f1;}}_LL5BC:
# 3597
 Cyc_Tc_tcEnumdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmpB94);goto _LL5B8;_LL5BD: {struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpB95=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmpB90;if(_tmpB95->tag != 2)goto _LL5B8;else{_tmpB96=_tmpB95->f1;}}_LL5BE:
# 3599
 Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmpB96);goto _LL5B8;_LL5B8:;}
# 3601
{void**_tmp1449;*_tmpB22=((_tmp1449=_cycalloc(sizeof(*_tmp1449)),((_tmp1449[0]=new_t,_tmp1449))));}
# 3602
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect);}_LL580: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpB23=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpB14;if(_tmpB23->tag != 14)goto _LL582;else{_tmpB24=_tmpB23->f1;}}_LL581: {
# 3606
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
# 3607
struct _RegionHandle _tmpB98=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpB98;_push_region(uprev_rgn);{
# 3608
struct Cyc_List_List*prev_fields=0;
# 3609
unsigned int tag_count=0;
# 3610
for(0;_tmpB24 != 0;_tmpB24=_tmpB24->tl){
# 3611
struct Cyc_Absyn_Enumfield*_tmpB99=(struct Cyc_Absyn_Enumfield*)_tmpB24->hd;
# 3612
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmpB99->name).f2)){
# 3613
const char*_tmp144D;void*_tmp144C[1];struct Cyc_String_pa_PrintArg_struct _tmp144B;(_tmp144B.tag=0,((_tmp144B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpB99->name).f2),((_tmp144C[0]=& _tmp144B,Cyc_Tcutil_terr(_tmpB99->loc,((_tmp144D="duplicate enum field name %s",_tag_dyneither(_tmp144D,sizeof(char),29))),_tag_dyneither(_tmp144C,sizeof(void*),1)))))));}else{
# 3615
struct Cyc_List_List*_tmp144E;prev_fields=((_tmp144E=_region_malloc(uprev_rgn,sizeof(*_tmp144E)),((_tmp144E->hd=(*_tmpB99->name).f2,((_tmp144E->tl=prev_fields,_tmp144E))))));}
# 3617
if(_tmpB99->tag == 0)
# 3618
_tmpB99->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmpB99->loc);else{
# 3619
if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_check_null(_tmpB99->tag))){
# 3620
const char*_tmp1452;void*_tmp1451[1];struct Cyc_String_pa_PrintArg_struct _tmp1450;(_tmp1450.tag=0,((_tmp1450.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpB99->name).f2),((_tmp1451[0]=& _tmp1450,Cyc_Tcutil_terr(loc,((_tmp1452="enum field %s: expression is not constant",_tag_dyneither(_tmp1452,sizeof(char),42))),_tag_dyneither(_tmp1451,sizeof(void*),1)))))));}}{
# 3622
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmpB99->tag))).f1;
# 3623
tag_count=t1 + 1;
# 3624
{union Cyc_Absyn_Nmspace _tmpBA1=(*_tmpB99->name).f1;struct Cyc_List_List*_tmpBA2;_LL5C0: if((_tmpBA1.Rel_n).tag != 1)goto _LL5C2;_tmpBA2=(struct Cyc_List_List*)(_tmpBA1.Rel_n).val;_LL5C1:
# 3626
(*_tmpB99->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LL5BF;_LL5C2:;_LL5C3:
# 3627
 goto _LL5BF;_LL5BF:;}{
# 3629
struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp1458;struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct _tmp1457;struct _tuple29*_tmp1456;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple29*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmpB99->name).f2,(struct _tuple29*)(
# 3630
(_tmp1456=_cycalloc(sizeof(*_tmp1456)),((_tmp1456->f1=(void*)((_tmp1458=_cycalloc(sizeof(*_tmp1458)),((_tmp1458[0]=((_tmp1457.tag=4,((_tmp1457.f1=(void*)t,((_tmp1457.f2=_tmpB99,_tmp1457)))))),_tmp1458)))),((_tmp1456->f2=1,_tmp1456)))))));};};}}
# 3633
_npop_handler(0);goto _LL577;
# 3607
;_pop_region(uprev_rgn);}_LL582: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpB25=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpB14;if(_tmpB25->tag != 13)goto _LL584;else{_tmpB26=_tmpB25->f1;_tmpB27=_tmpB25->f2;_tmpB28=(struct Cyc_Absyn_Enumdecl**)& _tmpB25->f2;}}_LL583:
# 3635
 if(*_tmpB28 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmpB28))->fields == 0){
# 3636
struct _handler_cons _tmpBA6;_push_handler(& _tmpBA6);{int _tmpBA8=0;if(setjmp(_tmpBA6.handler))_tmpBA8=1;if(!_tmpBA8){
# 3637
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmpB26);
# 3638
*_tmpB28=(struct Cyc_Absyn_Enumdecl*)*ed;}
# 3637
;_pop_handler();}else{void*_tmpBA7=(void*)_exn_thrown;void*_tmpBAA=_tmpBA7;_LL5C5: {struct Cyc_Dict_Absent_exn_struct*_tmpBAB=(struct Cyc_Dict_Absent_exn_struct*)_tmpBAA;if(_tmpBAB->tag != Cyc_Dict_Absent)goto _LL5C7;}_LL5C6: {
# 3641
struct Cyc_Tcenv_Genv*_tmpBAC=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
# 3642
struct Cyc_Absyn_Enumdecl*_tmp1459;struct Cyc_Absyn_Enumdecl*_tmpBAD=(_tmp1459=_cycalloc(sizeof(*_tmp1459)),((_tmp1459->sc=Cyc_Absyn_Extern,((_tmp1459->name=_tmpB26,((_tmp1459->fields=0,_tmp1459)))))));
# 3643
Cyc_Tc_tcEnumdecl(te,_tmpBAC,loc,_tmpBAD);{
# 3644
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmpB26);
# 3645
*_tmpB28=(struct Cyc_Absyn_Enumdecl*)*ed;
# 3646
goto _LL5C4;};}_LL5C7:;_LL5C8:(void)_throw(_tmpBAA);_LL5C4:;}};}{
# 3650
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmpB28);
# 3652
*_tmpB26=(ed->name)[0];
# 3653
goto _LL577;};_LL584: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB29=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpB14;if(_tmpB29->tag != 3)goto _LL586;else{_tmpB2A=_tmpB29->f1;_tmpB2B=_tmpB2A.datatype_info;_tmpB2C=(union Cyc_Absyn_DatatypeInfoU*)&(_tmpB29->f1).datatype_info;_tmpB2D=_tmpB2A.targs;_tmpB2E=(struct Cyc_List_List**)&(_tmpB29->f1).targs;}}_LL585: {
# 3655
struct Cyc_List_List*_tmpBAF=*_tmpB2E;
# 3656
{union Cyc_Absyn_DatatypeInfoU _tmpBB0=*_tmpB2C;struct Cyc_Absyn_UnknownDatatypeInfo _tmpBB1;struct _tuple2*_tmpBB2;int _tmpBB3;struct Cyc_Absyn_Datatypedecl**_tmpBB4;struct Cyc_Absyn_Datatypedecl*_tmpBB5;_LL5CA: if((_tmpBB0.UnknownDatatype).tag != 1)goto _LL5CC;_tmpBB1=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpBB0.UnknownDatatype).val;_tmpBB2=_tmpBB1.name;_tmpBB3=_tmpBB1.is_extensible;_LL5CB: {
# 3658
struct Cyc_Absyn_Datatypedecl**tudp;
# 3659
{struct _handler_cons _tmpBB6;_push_handler(& _tmpBB6);{int _tmpBB8=0;if(setjmp(_tmpBB6.handler))_tmpBB8=1;if(!_tmpBB8){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpBB2);;_pop_handler();}else{void*_tmpBB7=(void*)_exn_thrown;void*_tmpBBA=_tmpBB7;_LL5CF: {struct Cyc_Dict_Absent_exn_struct*_tmpBBB=(struct Cyc_Dict_Absent_exn_struct*)_tmpBBA;if(_tmpBBB->tag != Cyc_Dict_Absent)goto _LL5D1;}_LL5D0: {
# 3662
struct Cyc_Tcenv_Genv*_tmpBBC=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
# 3663
struct Cyc_Absyn_Datatypedecl*_tmp145A;struct Cyc_Absyn_Datatypedecl*_tmpBBD=(_tmp145A=_cycalloc(sizeof(*_tmp145A)),((_tmp145A->sc=Cyc_Absyn_Extern,((_tmp145A->name=_tmpBB2,((_tmp145A->tvs=0,((_tmp145A->fields=0,((_tmp145A->is_extensible=_tmpBB3,_tmp145A)))))))))));
# 3664
Cyc_Tc_tcDatatypedecl(te,_tmpBBC,loc,_tmpBBD);
# 3665
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpBB2);
# 3667
if(_tmpBAF != 0){
# 3668
{const char*_tmp145E;void*_tmp145D[1];struct Cyc_String_pa_PrintArg_struct _tmp145C;(_tmp145C.tag=0,((_tmp145C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3669
Cyc_Absynpp_qvar2string(_tmpBB2)),((_tmp145D[0]=& _tmp145C,Cyc_Tcutil_terr(loc,((_tmp145E="declare parameterized datatype %s before using",_tag_dyneither(_tmp145E,sizeof(char),47))),_tag_dyneither(_tmp145D,sizeof(void*),1)))))));}
# 3670
return cvtenv;}
# 3672
goto _LL5CE;}_LL5D1:;_LL5D2:(void)_throw(_tmpBBA);_LL5CE:;}};}
# 3678
if(_tmpBB3  && !(*tudp)->is_extensible){
# 3679
const char*_tmp1462;void*_tmp1461[1];struct Cyc_String_pa_PrintArg_struct _tmp1460;(_tmp1460.tag=0,((_tmp1460.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3680
Cyc_Absynpp_qvar2string(_tmpBB2)),((_tmp1461[0]=& _tmp1460,Cyc_Tcutil_terr(loc,((_tmp1462="datatype %s was not declared @extensible",_tag_dyneither(_tmp1462,sizeof(char),41))),_tag_dyneither(_tmp1461,sizeof(void*),1)))))));}
# 3681
*_tmpB2C=Cyc_Absyn_KnownDatatype(tudp);
# 3682
_tmpBB5=*tudp;goto _LL5CD;}_LL5CC: if((_tmpBB0.KnownDatatype).tag != 2)goto _LL5C9;_tmpBB4=(struct Cyc_Absyn_Datatypedecl**)(_tmpBB0.KnownDatatype).val;_tmpBB5=*_tmpBB4;_LL5CD: {
# 3685
struct Cyc_List_List*tvs=_tmpBB5->tvs;
# 3686
for(0;_tmpBAF != 0  && tvs != 0;(_tmpBAF=_tmpBAF->tl,tvs=tvs->tl)){
# 3687
void*t=(void*)_tmpBAF->hd;
# 3688
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3691
{struct _tuple0 _tmp1463;struct _tuple0 _tmpBC6=(_tmp1463.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp1463.f2=t,_tmp1463)));void*_tmpBC7;void*_tmpBC9;struct Cyc_Absyn_Tvar*_tmpBCB;_LL5D4: _tmpBC7=_tmpBC6.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpBC8=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpBC7;if(_tmpBC8->tag != 1)goto _LL5D6;};_tmpBC9=_tmpBC6.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpBCA=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpBC9;if(_tmpBCA->tag != 2)goto _LL5D6;else{_tmpBCB=_tmpBCA->f1;}};_LL5D5:
# 3693
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpBCB);
# 3694
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpBCB,1);
# 3695
continue;_LL5D6:;_LL5D7:
# 3696
 goto _LL5D3;_LL5D3:;}{
# 3698
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
# 3699
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
# 3700
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3702
if(_tmpBAF != 0){
# 3703
const char*_tmp1467;void*_tmp1466[1];struct Cyc_String_pa_PrintArg_struct _tmp1465;(_tmp1465.tag=0,((_tmp1465.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3704
Cyc_Absynpp_qvar2string(_tmpBB5->name)),((_tmp1466[0]=& _tmp1465,Cyc_Tcutil_terr(loc,((_tmp1467="too many type arguments for datatype %s",_tag_dyneither(_tmp1467,sizeof(char),40))),_tag_dyneither(_tmp1466,sizeof(void*),1)))))));}
# 3705
if(tvs != 0){
# 3707
struct Cyc_List_List*hidden_ts=0;
# 3708
for(0;tvs != 0;tvs=tvs->tl){
# 3709
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
# 3710
void*e=Cyc_Absyn_new_evar(0,0);
# 3711
{struct Cyc_List_List*_tmp1468;hidden_ts=((_tmp1468=_cycalloc(sizeof(*_tmp1468)),((_tmp1468->hd=e,((_tmp1468->tl=hidden_ts,_tmp1468))))));}
# 3712
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1);}
# 3714
*_tmpB2E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpB2E,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 3716
goto _LL5C9;}_LL5C9:;}
# 3718
goto _LL577;}_LL586: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpB2F=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpB14;if(_tmpB2F->tag != 4)goto _LL588;else{_tmpB30=_tmpB2F->f1;_tmpB31=_tmpB30.field_info;_tmpB32=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmpB2F->f1).field_info;_tmpB33=_tmpB30.targs;}}_LL587:
# 3721
{union Cyc_Absyn_DatatypeFieldInfoU _tmpBD0=*_tmpB32;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpBD1;struct _tuple2*_tmpBD2;struct _tuple2*_tmpBD3;int _tmpBD4;struct _tuple3 _tmpBD5;struct Cyc_Absyn_Datatypedecl*_tmpBD6;struct Cyc_Absyn_Datatypefield*_tmpBD7;_LL5D9: if((_tmpBD0.UnknownDatatypefield).tag != 1)goto _LL5DB;_tmpBD1=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpBD0.UnknownDatatypefield).val;_tmpBD2=_tmpBD1.datatype_name;_tmpBD3=_tmpBD1.field_name;_tmpBD4=_tmpBD1.is_extensible;_LL5DA: {
# 3723
struct Cyc_Absyn_Datatypedecl*tud;
# 3724
struct Cyc_Absyn_Datatypefield*tuf;
# 3725
{struct _handler_cons _tmpBD8;_push_handler(& _tmpBD8);{int _tmpBDA=0;if(setjmp(_tmpBD8.handler))_tmpBDA=1;if(!_tmpBDA){*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpBD2);;_pop_handler();}else{void*_tmpBD9=(void*)_exn_thrown;void*_tmpBDC=_tmpBD9;_LL5DE: {struct Cyc_Dict_Absent_exn_struct*_tmpBDD=(struct Cyc_Dict_Absent_exn_struct*)_tmpBDC;if(_tmpBDD->tag != Cyc_Dict_Absent)goto _LL5E0;}_LL5DF:
# 3727
{const char*_tmp146C;void*_tmp146B[1];struct Cyc_String_pa_PrintArg_struct _tmp146A;(_tmp146A.tag=0,((_tmp146A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD2)),((_tmp146B[0]=& _tmp146A,Cyc_Tcutil_terr(loc,((_tmp146C="unbound datatype %s",_tag_dyneither(_tmp146C,sizeof(char),20))),_tag_dyneither(_tmp146B,sizeof(void*),1)))))));}
# 3728
return cvtenv;_LL5E0:;_LL5E1:(void)_throw(_tmpBDC);_LL5DD:;}};}
# 3730
{struct _handler_cons _tmpBE1;_push_handler(& _tmpBE1);{int _tmpBE3=0;if(setjmp(_tmpBE1.handler))_tmpBE3=1;if(!_tmpBE3){
# 3731
{struct _RegionHandle _tmpBE4=_new_region("r");struct _RegionHandle*r=& _tmpBE4;_push_region(r);
# 3732
{void*_tmpBE5=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmpBD3,0);struct Cyc_Absyn_Datatypedecl*_tmpBE7;struct Cyc_Absyn_Datatypefield*_tmpBE8;_LL5E3: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpBE6=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpBE5;if(_tmpBE6->tag != 2)goto _LL5E5;else{_tmpBE7=_tmpBE6->f1;_tmpBE8=_tmpBE6->f2;}}_LL5E4:
# 3734
 tuf=_tmpBE8;
# 3735
tud=_tmpBE7;
# 3736
if(_tmpBD4  && !tud->is_extensible){
# 3737
const char*_tmp1470;void*_tmp146F[1];struct Cyc_String_pa_PrintArg_struct _tmp146E;(_tmp146E.tag=0,((_tmp146E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3738
Cyc_Absynpp_qvar2string(_tmpBD2)),((_tmp146F[0]=& _tmp146E,Cyc_Tcutil_terr(loc,((_tmp1470="datatype %s was not declared @extensible",_tag_dyneither(_tmp1470,sizeof(char),41))),_tag_dyneither(_tmp146F,sizeof(void*),1)))))));}
# 3739
goto _LL5E2;_LL5E5:;_LL5E6:
# 3740
{const char*_tmp1475;void*_tmp1474[2];struct Cyc_String_pa_PrintArg_struct _tmp1473;struct Cyc_String_pa_PrintArg_struct _tmp1472;(_tmp1472.tag=0,((_tmp1472.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3741
Cyc_Absynpp_qvar2string(_tmpBD2)),((_tmp1473.tag=0,((_tmp1473.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD3)),((_tmp1474[0]=& _tmp1473,((_tmp1474[1]=& _tmp1472,Cyc_Tcutil_terr(loc,((_tmp1475="unbound field %s in type datatype %s",_tag_dyneither(_tmp1475,sizeof(char),37))),_tag_dyneither(_tmp1474,sizeof(void*),2)))))))))))));}{
# 3742
struct Cyc_Tcutil_CVTEnv _tmpBF0=cvtenv;_npop_handler(1);return _tmpBF0;};_LL5E2:;}
# 3732
;_pop_region(r);}
# 3731
;_pop_handler();}else{void*_tmpBE2=(void*)_exn_thrown;void*_tmpBF2=_tmpBE2;_LL5E8: {struct Cyc_Dict_Absent_exn_struct*_tmpBF3=(struct Cyc_Dict_Absent_exn_struct*)_tmpBF2;if(_tmpBF3->tag != Cyc_Dict_Absent)goto _LL5EA;}_LL5E9:
# 3747
{const char*_tmp147A;void*_tmp1479[2];struct Cyc_String_pa_PrintArg_struct _tmp1478;struct Cyc_String_pa_PrintArg_struct _tmp1477;(_tmp1477.tag=0,((_tmp1477.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3748
Cyc_Absynpp_qvar2string(_tmpBD2)),((_tmp1478.tag=0,((_tmp1478.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD3)),((_tmp1479[0]=& _tmp1478,((_tmp1479[1]=& _tmp1477,Cyc_Tcutil_terr(loc,((_tmp147A="unbound field %s in type datatype %s",_tag_dyneither(_tmp147A,sizeof(char),37))),_tag_dyneither(_tmp1479,sizeof(void*),2)))))))))))));}
# 3749
return cvtenv;_LL5EA:;_LL5EB:(void)_throw(_tmpBF2);_LL5E7:;}};}
# 3751
*_tmpB32=Cyc_Absyn_KnownDatatypefield(tud,tuf);
# 3752
_tmpBD6=tud;_tmpBD7=tuf;goto _LL5DC;}_LL5DB: if((_tmpBD0.KnownDatatypefield).tag != 2)goto _LL5D8;_tmpBD5=(struct _tuple3)(_tmpBD0.KnownDatatypefield).val;_tmpBD6=_tmpBD5.f1;_tmpBD7=_tmpBD5.f2;_LL5DC: {
# 3755
struct Cyc_List_List*tvs=_tmpBD6->tvs;
# 3756
for(0;_tmpB33 != 0  && tvs != 0;(_tmpB33=_tmpB33->tl,tvs=tvs->tl)){
# 3757
void*t=(void*)_tmpB33->hd;
# 3758
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3761
{struct _tuple0 _tmp147B;struct _tuple0 _tmpBF9=(_tmp147B.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp147B.f2=t,_tmp147B)));void*_tmpBFA;void*_tmpBFC;struct Cyc_Absyn_Tvar*_tmpBFE;_LL5ED: _tmpBFA=_tmpBF9.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpBFB=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpBFA;if(_tmpBFB->tag != 1)goto _LL5EF;};_tmpBFC=_tmpBF9.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpBFD=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpBFC;if(_tmpBFD->tag != 2)goto _LL5EF;else{_tmpBFE=_tmpBFD->f1;}};_LL5EE:
# 3763
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpBFE);
# 3764
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpBFE,1);
# 3765
continue;_LL5EF:;_LL5F0:
# 3766
 goto _LL5EC;_LL5EC:;}{
# 3768
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
# 3769
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
# 3770
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3772
if(_tmpB33 != 0){
# 3773
const char*_tmp1480;void*_tmp147F[2];struct Cyc_String_pa_PrintArg_struct _tmp147E;struct Cyc_String_pa_PrintArg_struct _tmp147D;(_tmp147D.tag=0,((_tmp147D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3774
Cyc_Absynpp_qvar2string(_tmpBD7->name)),((_tmp147E.tag=0,((_tmp147E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD6->name)),((_tmp147F[0]=& _tmp147E,((_tmp147F[1]=& _tmp147D,Cyc_Tcutil_terr(loc,((_tmp1480="too many type arguments for datatype %s.%s",_tag_dyneither(_tmp1480,sizeof(char),43))),_tag_dyneither(_tmp147F,sizeof(void*),2)))))))))))));}
# 3775
if(tvs != 0){
# 3776
const char*_tmp1485;void*_tmp1484[2];struct Cyc_String_pa_PrintArg_struct _tmp1483;struct Cyc_String_pa_PrintArg_struct _tmp1482;(_tmp1482.tag=0,((_tmp1482.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3777
Cyc_Absynpp_qvar2string(_tmpBD7->name)),((_tmp1483.tag=0,((_tmp1483.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD6->name)),((_tmp1484[0]=& _tmp1483,((_tmp1484[1]=& _tmp1482,Cyc_Tcutil_terr(loc,((_tmp1485="too few type arguments for datatype %s.%s",_tag_dyneither(_tmp1485,sizeof(char),42))),_tag_dyneither(_tmp1484,sizeof(void*),2)))))))))))));}
# 3778
goto _LL5D8;}_LL5D8:;}
# 3780
goto _LL577;_LL588: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpB34=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB14;if(_tmpB34->tag != 5)goto _LL58A;else{_tmpB35=_tmpB34->f1;_tmpB36=_tmpB35.elt_typ;_tmpB37=_tmpB35.elt_tq;_tmpB38=(struct Cyc_Absyn_Tqual*)&(_tmpB34->f1).elt_tq;_tmpB39=_tmpB35.ptr_atts;_tmpB3A=_tmpB39.rgn;_tmpB3B=_tmpB39.nullable;_tmpB3C=_tmpB39.bounds;_tmpB3D=_tmpB39.zero_term;}}_LL589: {
# 3783
int is_zero_terminated;
# 3785
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB36,1);
# 3786
_tmpB38->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpB38->print_const,_tmpB36);{
# 3787
struct Cyc_Absyn_Kind*k;
# 3788
switch(expected_kind->aliasqual){case Cyc_Absyn_Aliasable: _LL5F1:
# 3789
 k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique: _LL5F2:
# 3790
 k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL5F3:
# 3791
 k=& Cyc_Tcutil_trk;break;}
# 3793
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmpB3A,1);
# 3794
{union Cyc_Absyn_Constraint*_tmpC07=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmpB3D);union Cyc_Absyn_Constraint _tmpC08;int _tmpC09;union Cyc_Absyn_Constraint _tmpC0A;int _tmpC0B;_LL5F6: _tmpC08=*_tmpC07;if((_tmpC08.No_constr).tag != 3)goto _LL5F8;_tmpC09=(int)(_tmpC08.No_constr).val;_LL5F7:
# 3798
{void*_tmpC0C=Cyc_Tcutil_compress(_tmpB36);enum Cyc_Absyn_Size_of _tmpC0E;_LL5FD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpC0D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpC0C;if(_tmpC0D->tag != 6)goto _LL5FF;else{_tmpC0E=_tmpC0D->f2;if(_tmpC0E != Cyc_Absyn_Char_sz)goto _LL5FF;}}_LL5FE:
# 3800
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB3D,Cyc_Absyn_true_conref);
# 3801
is_zero_terminated=1;
# 3802
goto _LL5FC;_LL5FF:;_LL600:
# 3804
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB3D,Cyc_Absyn_false_conref);
# 3805
is_zero_terminated=0;
# 3806
goto _LL5FC;_LL5FC:;}
# 3808
goto _LL5F5;_LL5F8: _tmpC0A=*_tmpC07;if((_tmpC0A.Eq_constr).tag != 1)goto _LL5FA;_tmpC0B=(int)(_tmpC0A.Eq_constr).val;if(_tmpC0B != 1)goto _LL5FA;_LL5F9:
# 3811
 if(!Cyc_Tcutil_admits_zero(_tmpB36)){
# 3812
const char*_tmp1489;void*_tmp1488[1];struct Cyc_String_pa_PrintArg_struct _tmp1487;(_tmp1487.tag=0,((_tmp1487.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3813
Cyc_Absynpp_typ2string(_tmpB36)),((_tmp1488[0]=& _tmp1487,Cyc_Tcutil_terr(loc,((_tmp1489="cannot have a pointer to zero-terminated %s elements",_tag_dyneither(_tmp1489,sizeof(char),53))),_tag_dyneither(_tmp1488,sizeof(void*),1)))))));}
# 3814
is_zero_terminated=1;
# 3815
goto _LL5F5;_LL5FA:;_LL5FB:
# 3817
 is_zero_terminated=0;
# 3818
goto _LL5F5;_LL5F5:;}
# 3821
{void*_tmpC12=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpB3C);struct Cyc_Absyn_Exp*_tmpC15;_LL602: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpC13=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpC12;if(_tmpC13->tag != 0)goto _LL604;}_LL603:
# 3822
 goto _LL601;_LL604: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpC14=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpC12;if(_tmpC14->tag != 1)goto _LL601;else{_tmpC15=_tmpC14->f1;}}_LL605: {
# 3824
struct _RegionHandle _tmpC16=_new_region("temp");struct _RegionHandle*temp=& _tmpC16;_push_region(temp);{
# 3825
struct Cyc_Tcenv_Tenv*_tmpC17=Cyc_Tcenv_allow_valueof(temp,te);
# 3826
Cyc_Tcexp_tcExp(_tmpC17,0,_tmpC15);}
# 3828
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC15,te,cvtenv);
# 3829
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpC15)){
# 3830
const char*_tmp148C;void*_tmp148B;(_tmp148B=0,Cyc_Tcutil_terr(loc,((_tmp148C="pointer bounds expression is not an unsigned int",_tag_dyneither(_tmp148C,sizeof(char),49))),_tag_dyneither(_tmp148B,sizeof(void*),0)));}{
# 3831
unsigned int _tmpC1B;int _tmpC1C;struct _tuple14 _tmpC1A=Cyc_Evexp_eval_const_uint_exp(_tmpC15);_tmpC1B=_tmpC1A.f1;_tmpC1C=_tmpC1A.f2;
# 3832
if(is_zero_terminated  && (!_tmpC1C  || _tmpC1B < 1)){
# 3833
const char*_tmp148F;void*_tmp148E;(_tmp148E=0,Cyc_Tcutil_terr(loc,((_tmp148F="zero-terminated pointer cannot point to empty sequence",_tag_dyneither(_tmp148F,sizeof(char),55))),_tag_dyneither(_tmp148E,sizeof(void*),0)));}
# 3834
_npop_handler(0);goto _LL601;};
# 3824
;_pop_region(temp);}_LL601:;}
# 3836
goto _LL577;};}_LL58A: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB3E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpB14;if(_tmpB3E->tag != 19)goto _LL58C;else{_tmpB3F=(void*)_tmpB3E->f1;}}_LL58B:
# 3838
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpB3F,1);goto _LL577;_LL58C: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpB40=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmpB14;if(_tmpB40->tag != 18)goto _LL58E;else{_tmpB41=_tmpB40->f1;}}_LL58D: {
# 3843
struct _RegionHandle _tmpC1F=_new_region("temp");struct _RegionHandle*temp=& _tmpC1F;_push_region(temp);{
# 3844
struct Cyc_Tcenv_Tenv*_tmpC20=Cyc_Tcenv_allow_valueof(temp,te);
# 3845
Cyc_Tcexp_tcExp(_tmpC20,0,_tmpB41);}
# 3847
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpB41)){
# 3848
const char*_tmp1492;void*_tmp1491;(_tmp1491=0,Cyc_Tcutil_terr(loc,((_tmp1492="valueof_t requires an int expression",_tag_dyneither(_tmp1492,sizeof(char),37))),_tag_dyneither(_tmp1491,sizeof(void*),0)));}
# 3849
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB41,te,cvtenv);
# 3850
_npop_handler(0);goto _LL577;
# 3843
;_pop_region(temp);}_LL58E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpB42=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpB14;if(_tmpB42->tag != 6)goto _LL590;}_LL58F:
# 3851
 goto _LL591;_LL590: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpB43=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpB14;if(_tmpB43->tag != 7)goto _LL592;}_LL591:
# 3852
 goto _LL577;_LL592: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpB44=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB14;if(_tmpB44->tag != 8)goto _LL594;else{_tmpB45=_tmpB44->f1;_tmpB46=_tmpB45.elt_type;_tmpB47=_tmpB45.tq;_tmpB48=(struct Cyc_Absyn_Tqual*)&(_tmpB44->f1).tq;_tmpB49=_tmpB45.num_elts;_tmpB4A=(struct Cyc_Absyn_Exp**)&(_tmpB44->f1).num_elts;_tmpB4B=_tmpB45.zero_term;_tmpB4C=_tmpB45.zt_loc;}}_LL593: {
# 3856
struct Cyc_Absyn_Exp*_tmpC23=*_tmpB4A;
# 3857
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpB46,1);
# 3858
_tmpB48->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpB48->print_const,_tmpB46);{
# 3859
int is_zero_terminated;
# 3860
{union Cyc_Absyn_Constraint*_tmpC24=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmpB4B);union Cyc_Absyn_Constraint _tmpC25;int _tmpC26;union Cyc_Absyn_Constraint _tmpC27;int _tmpC28;_LL607: _tmpC25=*_tmpC24;if((_tmpC25.No_constr).tag != 3)goto _LL609;_tmpC26=(int)(_tmpC25.No_constr).val;_LL608:
# 3863
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB4B,Cyc_Absyn_false_conref);
# 3864
is_zero_terminated=0;
# 3878 "tcutil.cyc"
goto _LL606;_LL609: _tmpC27=*_tmpC24;if((_tmpC27.Eq_constr).tag != 1)goto _LL60B;_tmpC28=(int)(_tmpC27.Eq_constr).val;if(_tmpC28 != 1)goto _LL60B;_LL60A:
# 3881
 if(!Cyc_Tcutil_admits_zero(_tmpB46)){
# 3882
const char*_tmp1496;void*_tmp1495[1];struct Cyc_String_pa_PrintArg_struct _tmp1494;(_tmp1494.tag=0,((_tmp1494.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3883
Cyc_Absynpp_typ2string(_tmpB46)),((_tmp1495[0]=& _tmp1494,Cyc_Tcutil_terr(loc,((_tmp1496="cannot have a zero-terminated array of %s elements",_tag_dyneither(_tmp1496,sizeof(char),51))),_tag_dyneither(_tmp1495,sizeof(void*),1)))))));}
# 3884
is_zero_terminated=1;
# 3885
goto _LL606;_LL60B:;_LL60C:
# 3887
 is_zero_terminated=0;
# 3888
goto _LL606;_LL606:;}
# 3892
if(_tmpC23 == 0){
# 3894
if(is_zero_terminated)
# 3896
*_tmpB4A=(_tmpC23=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{
# 3899
{const char*_tmp1499;void*_tmp1498;(_tmp1498=0,Cyc_Tcutil_warn(loc,((_tmp1499="array bound defaults to 1 here",_tag_dyneither(_tmp1499,sizeof(char),31))),_tag_dyneither(_tmp1498,sizeof(void*),0)));}
# 3900
*_tmpB4A=(_tmpC23=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}
# 3903
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmpC23);
# 3904
if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_tmpC23)){
# 3905
const char*_tmp149C;void*_tmp149B;(_tmp149B=0,Cyc_Tcutil_terr(loc,((_tmp149C="array bounds expression is not constant",_tag_dyneither(_tmp149C,sizeof(char),40))),_tag_dyneither(_tmp149B,sizeof(void*),0)));}
# 3906
if(!Cyc_Tcutil_coerce_uint_typ(te,(struct Cyc_Absyn_Exp*)_tmpC23)){
# 3907
const char*_tmp149F;void*_tmp149E;(_tmp149E=0,Cyc_Tcutil_terr(loc,((_tmp149F="array bounds expression is not an unsigned int",_tag_dyneither(_tmp149F,sizeof(char),47))),_tag_dyneither(_tmp149E,sizeof(void*),0)));}{
# 3908
unsigned int _tmpC33;int _tmpC34;struct _tuple14 _tmpC32=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmpC23);_tmpC33=_tmpC32.f1;_tmpC34=_tmpC32.f2;
# 3910
if((is_zero_terminated  && _tmpC34) && _tmpC33 < 1){
# 3911
const char*_tmp14A2;void*_tmp14A1;(_tmp14A1=0,Cyc_Tcutil_warn(loc,((_tmp14A2="zero terminated array cannot have zero elements",_tag_dyneither(_tmp14A2,sizeof(char),48))),_tag_dyneither(_tmp14A1,sizeof(void*),0)));}
# 3913
if((_tmpC34  && _tmpC33 < 1) && Cyc_Cyclone_tovc_r){
# 3914
{const char*_tmp14A5;void*_tmp14A4;(_tmp14A4=0,Cyc_Tcutil_warn(loc,((_tmp14A5="arrays with 0 elements are not supported except with gcc -- changing to 1.",_tag_dyneither(_tmp14A5,sizeof(char),75))),_tag_dyneither(_tmp14A4,sizeof(void*),0)));}
# 3915
*_tmpB4A=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}
# 3917
goto _LL577;};};}_LL594: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB4D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB14;if(_tmpB4D->tag != 9)goto _LL596;else{_tmpB4E=_tmpB4D->f1;_tmpB4F=_tmpB4E.tvars;_tmpB50=(struct Cyc_List_List**)&(_tmpB4D->f1).tvars;_tmpB51=_tmpB4E.effect;_tmpB52=(void**)&(_tmpB4D->f1).effect;_tmpB53=_tmpB4E.ret_tqual;_tmpB54=(struct Cyc_Absyn_Tqual*)&(_tmpB4D->f1).ret_tqual;_tmpB55=_tmpB4E.ret_typ;_tmpB56=_tmpB4E.args;_tmpB57=_tmpB4E.c_varargs;_tmpB58=_tmpB4E.cyc_varargs;_tmpB59=_tmpB4E.rgn_po;_tmpB5A=_tmpB4E.attributes;}}_LL595: {
# 3924
int num_convs=0;
# 3925
int seen_cdecl=0;
# 3926
int seen_stdcall=0;
# 3927
int seen_fastcall=0;
# 3928
int seen_format=0;
# 3929
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
# 3930
int fmt_desc_arg=- 1;
# 3931
int fmt_arg_start=- 1;
# 3932
for(0;_tmpB5A != 0;_tmpB5A=_tmpB5A->tl){
# 3933
if(!Cyc_Absyn_fntype_att((void*)_tmpB5A->hd)){
# 3934
const char*_tmp14A9;void*_tmp14A8[1];struct Cyc_String_pa_PrintArg_struct _tmp14A7;(_tmp14A7.tag=0,((_tmp14A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmpB5A->hd)),((_tmp14A8[0]=& _tmp14A7,Cyc_Tcutil_terr(loc,((_tmp14A9="bad function type attribute %s",_tag_dyneither(_tmp14A9,sizeof(char),31))),_tag_dyneither(_tmp14A8,sizeof(void*),1)))))));}{
# 3935
void*_tmpC3C=(void*)_tmpB5A->hd;enum Cyc_Absyn_Format_Type _tmpC41;int _tmpC42;int _tmpC43;_LL60E: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpC3D=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpC3C;if(_tmpC3D->tag != 1)goto _LL610;}_LL60F:
# 3937
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL60D;_LL610: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpC3E=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpC3C;if(_tmpC3E->tag != 2)goto _LL612;}_LL611:
# 3939
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL60D;_LL612: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpC3F=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpC3C;if(_tmpC3F->tag != 3)goto _LL614;}_LL613:
# 3941
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL60D;_LL614: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpC40=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC3C;if(_tmpC40->tag != 19)goto _LL616;else{_tmpC41=_tmpC40->f1;_tmpC42=_tmpC40->f2;_tmpC43=_tmpC40->f3;}}_LL615:
# 3943
 if(!seen_format){
# 3944
seen_format=1;
# 3945
ft=_tmpC41;
# 3946
fmt_desc_arg=_tmpC42;
# 3947
fmt_arg_start=_tmpC43;}else{
# 3949
const char*_tmp14AC;void*_tmp14AB;(_tmp14AB=0,Cyc_Tcutil_terr(loc,((_tmp14AC="function can't have multiple format attributes",_tag_dyneither(_tmp14AC,sizeof(char),47))),_tag_dyneither(_tmp14AB,sizeof(void*),0)));}
# 3950
goto _LL60D;_LL616:;_LL617:
# 3951
 goto _LL60D;_LL60D:;};}
# 3954
if(num_convs > 1){
# 3955
const char*_tmp14AF;void*_tmp14AE;(_tmp14AE=0,Cyc_Tcutil_terr(loc,((_tmp14AF="function can't have multiple calling conventions",_tag_dyneither(_tmp14AF,sizeof(char),49))),_tag_dyneither(_tmp14AE,sizeof(void*),0)));}
# 3959
Cyc_Tcutil_check_unique_tvars(loc,*_tmpB50);
# 3960
{struct Cyc_List_List*b=*_tmpB50;for(0;b != 0;b=b->tl){
# 3961
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
# 3962
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
# 3963
void*_tmpC48=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);struct Cyc_Absyn_Kind*_tmpC4A;struct Cyc_Absyn_Kind _tmpC4B;enum Cyc_Absyn_KindQual _tmpC4C;_LL619: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpC49=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpC48;if(_tmpC49->tag != 0)goto _LL61B;else{_tmpC4A=_tmpC49->f1;_tmpC4B=*_tmpC4A;_tmpC4C=_tmpC4B.kind;if(_tmpC4C != Cyc_Absyn_MemKind)goto _LL61B;}}_LL61A:
# 3965
{const char*_tmp14B3;void*_tmp14B2[1];struct Cyc_String_pa_PrintArg_struct _tmp14B1;(_tmp14B1.tag=0,((_tmp14B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name),((_tmp14B2[0]=& _tmp14B1,Cyc_Tcutil_terr(loc,((_tmp14B3="function attempts to abstract Mem type variable %s",_tag_dyneither(_tmp14B3,sizeof(char),51))),_tag_dyneither(_tmp14B2,sizeof(void*),1)))))));}
# 3966
goto _LL618;_LL61B:;_LL61C:
# 3967
 goto _LL618;_LL618:;};}}{
# 3973
struct _RegionHandle _tmpC50=_new_region("newr");struct _RegionHandle*newr=& _tmpC50;_push_region(newr);{
# 3974
struct Cyc_Tcutil_CVTEnv _tmp14B4;struct Cyc_Tcutil_CVTEnv _tmpC51=
# 3975
(_tmp14B4.r=newr,((_tmp14B4.kind_env=cvtenv.kind_env,((_tmp14B4.free_vars=0,((_tmp14B4.free_evars=0,((_tmp14B4.generalize_evars=cvtenv.generalize_evars,((_tmp14B4.fn_result=1,_tmp14B4)))))))))));
# 3979
_tmpC51=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC51,& Cyc_Tcutil_tmk,_tmpB55,1);
# 3980
_tmpB54->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpB54->print_const,_tmpB55);
# 3981
_tmpC51.fn_result=0;
# 3982
{struct Cyc_List_List*a=_tmpB56;for(0;a != 0;a=a->tl){
# 3983
struct _tuple10*_tmpC52=(struct _tuple10*)a->hd;
# 3984
void*_tmpC53=(*_tmpC52).f3;
# 3985
_tmpC51=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC51,& Cyc_Tcutil_tmk,_tmpC53,1);{
# 3986
int _tmpC54=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpC52).f2).print_const,_tmpC53);
# 3987
((*_tmpC52).f2).real_const=_tmpC54;{
# 3990
void*_tmpC55=Cyc_Tcutil_compress(_tmpC53);struct Cyc_Absyn_ArrayInfo _tmpC57;void*_tmpC58;struct Cyc_Absyn_Tqual _tmpC59;struct Cyc_Absyn_Exp*_tmpC5A;union Cyc_Absyn_Constraint*_tmpC5B;unsigned int _tmpC5C;_LL61E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC56=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC55;if(_tmpC56->tag != 8)goto _LL620;else{_tmpC57=_tmpC56->f1;_tmpC58=_tmpC57.elt_type;_tmpC59=_tmpC57.tq;_tmpC5A=_tmpC57.num_elts;_tmpC5B=_tmpC57.zero_term;_tmpC5C=_tmpC57.zt_loc;}}_LL61F: {
# 3993
void*_tmpC5D=Cyc_Absyn_new_evar(0,0);
# 3994
_tmpC51=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC51,& Cyc_Tcutil_rk,_tmpC5D,1);{
# 3995
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp14B7;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp14B6;void*_tmpC5E=Cyc_Absyn_atb_typ(_tmpC58,_tmpC5D,_tmpC59,(void*)(
# 3996
(_tmp14B6=_cycalloc(sizeof(*_tmp14B6)),((_tmp14B6[0]=((_tmp14B7.tag=1,((_tmp14B7.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpC5A),_tmp14B7)))),_tmp14B6)))),_tmpC5B);
# 3997
(*_tmpC52).f3=_tmpC5E;
# 3998
goto _LL61D;};}_LL620:;_LL621:
# 3999
 goto _LL61D;_LL61D:;};};}}
# 4004
if(_tmpB58 != 0){
# 4005
if(_tmpB57){const char*_tmp14BA;void*_tmp14B9;(_tmp14B9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp14BA="both c_vararg and cyc_vararg",_tag_dyneither(_tmp14BA,sizeof(char),29))),_tag_dyneither(_tmp14B9,sizeof(void*),0)));}{
# 4006
void*_tmpC64;int _tmpC65;struct Cyc_Absyn_VarargInfo _tmpC63=*_tmpB58;_tmpC64=_tmpC63.type;_tmpC65=_tmpC63.inject;
# 4007
_tmpC51=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC51,& Cyc_Tcutil_tmk,_tmpC64,1);
# 4008
(_tmpB58->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmpB58->tq).print_const,_tmpC64);
# 4010
if(_tmpC65){
# 4011
void*_tmpC66=Cyc_Tcutil_compress(_tmpC64);struct Cyc_Absyn_PtrInfo _tmpC68;void*_tmpC69;struct Cyc_Absyn_PtrAtts _tmpC6A;union Cyc_Absyn_Constraint*_tmpC6B;union Cyc_Absyn_Constraint*_tmpC6C;_LL623: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC67=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC66;if(_tmpC67->tag != 5)goto _LL625;else{_tmpC68=_tmpC67->f1;_tmpC69=_tmpC68.elt_typ;_tmpC6A=_tmpC68.ptr_atts;_tmpC6B=_tmpC6A.bounds;_tmpC6C=_tmpC6A.zero_term;}}_LL624:
# 4013
{void*_tmpC6D=Cyc_Tcutil_compress(_tmpC69);_LL628: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpC6E=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpC6D;if(_tmpC6E->tag != 3)goto _LL62A;}_LL629:
# 4015
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmpC6C)){
# 4016
const char*_tmp14BD;void*_tmp14BC;(_tmp14BC=0,Cyc_Tcutil_terr(loc,((_tmp14BD="can't inject into a zeroterm pointer",_tag_dyneither(_tmp14BD,sizeof(char),37))),_tag_dyneither(_tmp14BC,sizeof(void*),0)));}
# 4017
{void*_tmpC71=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpC6B);_LL62D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpC72=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpC71;if(_tmpC72->tag != 0)goto _LL62F;}_LL62E:
# 4019
{const char*_tmp14C0;void*_tmp14BF;(_tmp14BF=0,Cyc_Tcutil_terr(loc,((_tmp14C0="can't inject into a fat pointer to datatype",_tag_dyneither(_tmp14C0,sizeof(char),44))),_tag_dyneither(_tmp14BF,sizeof(void*),0)));}
# 4020
goto _LL62C;_LL62F:;_LL630:
# 4021
 goto _LL62C;_LL62C:;}
# 4023
goto _LL627;_LL62A:;_LL62B:
# 4024
{const char*_tmp14C3;void*_tmp14C2;(_tmp14C2=0,Cyc_Tcutil_terr(loc,((_tmp14C3="can't inject a non-datatype type",_tag_dyneither(_tmp14C3,sizeof(char),33))),_tag_dyneither(_tmp14C2,sizeof(void*),0)));}goto _LL627;_LL627:;}
# 4026
goto _LL622;_LL625:;_LL626:
# 4027
{const char*_tmp14C6;void*_tmp14C5;(_tmp14C5=0,Cyc_Tcutil_terr(loc,((_tmp14C6="expecting a datatype pointer type",_tag_dyneither(_tmp14C6,sizeof(char),34))),_tag_dyneither(_tmp14C5,sizeof(void*),0)));}goto _LL622;_LL622:;}};}
# 4032
if(seen_format){
# 4033
int _tmpC79=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpB56);
# 4034
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpC79) || fmt_arg_start < 0) || 
# 4036
_tmpB58 == 0  && fmt_arg_start != 0) || 
# 4037
_tmpB58 != 0  && fmt_arg_start != _tmpC79 + 1){
# 4038
const char*_tmp14C9;void*_tmp14C8;(_tmp14C8=0,Cyc_Tcutil_terr(loc,((_tmp14C9="bad format descriptor",_tag_dyneither(_tmp14C9,sizeof(char),22))),_tag_dyneither(_tmp14C8,sizeof(void*),0)));}else{
# 4041
void*_tmpC7D;struct _tuple10 _tmpC7C=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpB56,fmt_desc_arg - 1);_tmpC7D=_tmpC7C.f3;
# 4043
{void*_tmpC7E=Cyc_Tcutil_compress(_tmpC7D);struct Cyc_Absyn_PtrInfo _tmpC80;void*_tmpC81;struct Cyc_Absyn_PtrAtts _tmpC82;union Cyc_Absyn_Constraint*_tmpC83;union Cyc_Absyn_Constraint*_tmpC84;_LL632: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC7F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC7E;if(_tmpC7F->tag != 5)goto _LL634;else{_tmpC80=_tmpC7F->f1;_tmpC81=_tmpC80.elt_typ;_tmpC82=_tmpC80.ptr_atts;_tmpC83=_tmpC82.bounds;_tmpC84=_tmpC82.zero_term;}}_LL633:
# 4046
{struct _tuple0 _tmp14CA;struct _tuple0 _tmpC86=(_tmp14CA.f1=Cyc_Tcutil_compress(_tmpC81),((_tmp14CA.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpC83),_tmp14CA)));void*_tmpC87;enum Cyc_Absyn_Sign _tmpC89;enum Cyc_Absyn_Size_of _tmpC8A;void*_tmpC8B;_LL637: _tmpC87=_tmpC86.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpC88=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpC87;if(_tmpC88->tag != 6)goto _LL639;else{_tmpC89=_tmpC88->f1;if(_tmpC89 != Cyc_Absyn_None)goto _LL639;_tmpC8A=_tmpC88->f2;if(_tmpC8A != Cyc_Absyn_Char_sz)goto _LL639;}};_tmpC8B=_tmpC86.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpC8C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpC8B;if(_tmpC8C->tag != 0)goto _LL639;};_LL638:
# 4047
 goto _LL636;_LL639:;_LL63A:
# 4048
{const char*_tmp14CD;void*_tmp14CC;(_tmp14CC=0,Cyc_Tcutil_terr(loc,((_tmp14CD="format descriptor is not a char ? type",_tag_dyneither(_tmp14CD,sizeof(char),39))),_tag_dyneither(_tmp14CC,sizeof(void*),0)));}goto _LL636;_LL636:;}
# 4050
goto _LL631;_LL634:;_LL635:
# 4051
{const char*_tmp14D0;void*_tmp14CF;(_tmp14CF=0,Cyc_Tcutil_terr(loc,((_tmp14D0="format descriptor is not a char ? type",_tag_dyneither(_tmp14D0,sizeof(char),39))),_tag_dyneither(_tmp14CF,sizeof(void*),0)));}goto _LL631;_LL631:;}
# 4053
if(fmt_arg_start != 0){
# 4057
int problem;
# 4058
{struct _tuple30 _tmp14D1;struct _tuple30 _tmpC92=(_tmp14D1.f1=ft,((_tmp14D1.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmpB58))->type)),_tmp14D1)));enum Cyc_Absyn_Format_Type _tmpC93;void*_tmpC94;struct Cyc_Absyn_DatatypeInfo _tmpC96;union Cyc_Absyn_DatatypeInfoU _tmpC97;struct Cyc_Absyn_Datatypedecl**_tmpC98;struct Cyc_Absyn_Datatypedecl*_tmpC99;enum Cyc_Absyn_Format_Type _tmpC9A;void*_tmpC9B;struct Cyc_Absyn_DatatypeInfo _tmpC9D;union Cyc_Absyn_DatatypeInfoU _tmpC9E;struct Cyc_Absyn_Datatypedecl**_tmpC9F;struct Cyc_Absyn_Datatypedecl*_tmpCA0;_LL63C: _tmpC93=_tmpC92.f1;if(_tmpC93 != Cyc_Absyn_Printf_ft)goto _LL63E;_tmpC94=_tmpC92.f2;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpC95=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpC94;if(_tmpC95->tag != 3)goto _LL63E;else{_tmpC96=_tmpC95->f1;_tmpC97=_tmpC96.datatype_info;if((_tmpC97.KnownDatatype).tag != 2)goto _LL63E;_tmpC98=(struct Cyc_Absyn_Datatypedecl**)(_tmpC97.KnownDatatype).val;_tmpC99=*_tmpC98;}};_LL63D:
# 4060
 problem=Cyc_Absyn_qvar_cmp(_tmpC99->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL63B;_LL63E: _tmpC9A=_tmpC92.f1;if(_tmpC9A != Cyc_Absyn_Scanf_ft)goto _LL640;_tmpC9B=_tmpC92.f2;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpC9C=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpC9B;if(_tmpC9C->tag != 3)goto _LL640;else{_tmpC9D=_tmpC9C->f1;_tmpC9E=_tmpC9D.datatype_info;if((_tmpC9E.KnownDatatype).tag != 2)goto _LL640;_tmpC9F=(struct Cyc_Absyn_Datatypedecl**)(_tmpC9E.KnownDatatype).val;_tmpCA0=*_tmpC9F;}};_LL63F:
# 4062
 problem=Cyc_Absyn_qvar_cmp(_tmpCA0->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL63B;_LL640:;_LL641:
# 4064
 problem=1;goto _LL63B;_LL63B:;}
# 4066
if(problem){
# 4067
const char*_tmp14D4;void*_tmp14D3;(_tmp14D3=0,Cyc_Tcutil_terr(loc,((_tmp14D4="format attribute and vararg types don't match",_tag_dyneither(_tmp14D4,sizeof(char),46))),_tag_dyneither(_tmp14D3,sizeof(void*),0)));}}}}
# 4074
{struct Cyc_List_List*rpo=_tmpB59;for(0;rpo != 0;rpo=rpo->tl){
# 4075
struct _tuple0 _tmpCA4;void*_tmpCA5;void*_tmpCA6;struct _tuple0*_tmpCA3=(struct _tuple0*)rpo->hd;_tmpCA4=*_tmpCA3;_tmpCA5=_tmpCA4.f1;_tmpCA6=_tmpCA4.f2;
# 4076
_tmpC51=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC51,& Cyc_Tcutil_ek,_tmpCA5,1);
# 4077
_tmpC51=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC51,& Cyc_Tcutil_trk,_tmpCA6,1);}}
# 4080
if(*_tmpB52 != 0)
# 4081
_tmpC51=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC51,& Cyc_Tcutil_ek,(void*)_check_null(*_tmpB52),1);else{
# 4083
struct Cyc_List_List*effect=0;
# 4088
{struct Cyc_List_List*tvs=_tmpC51.free_vars;for(0;tvs != 0;tvs=tvs->tl){
# 4089
struct Cyc_Absyn_Tvar*_tmpCA8;int _tmpCA9;struct _tuple28 _tmpCA7=*((struct _tuple28*)tvs->hd);_tmpCA8=_tmpCA7.f1;_tmpCA9=_tmpCA7.f2;
# 4090
if(!_tmpCA9)continue;{
# 4091
void*_tmpCAA=Cyc_Absyn_compress_kb(_tmpCA8->kind);struct Cyc_Core_Opt*_tmpCAC;struct Cyc_Core_Opt**_tmpCAD;struct Cyc_Absyn_Kind*_tmpCAE;struct Cyc_Absyn_Kind*_tmpCB0;struct Cyc_Absyn_Kind*_tmpCB2;struct Cyc_Absyn_Kind _tmpCB3;enum Cyc_Absyn_KindQual _tmpCB4;struct Cyc_Absyn_Kind*_tmpCB6;struct Cyc_Absyn_Kind _tmpCB7;enum Cyc_Absyn_KindQual _tmpCB8;struct Cyc_Core_Opt*_tmpCBA;struct Cyc_Core_Opt**_tmpCBB;struct Cyc_Absyn_Kind*_tmpCBC;struct Cyc_Absyn_Kind _tmpCBD;enum Cyc_Absyn_KindQual _tmpCBE;struct Cyc_Absyn_Kind*_tmpCC0;struct Cyc_Absyn_Kind _tmpCC1;enum Cyc_Absyn_KindQual _tmpCC2;struct Cyc_Core_Opt*_tmpCC4;struct Cyc_Core_Opt**_tmpCC5;_LL643:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCAB=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCAA;if(_tmpCAB->tag != 2)goto _LL645;else{_tmpCAC=_tmpCAB->f1;_tmpCAD=(struct Cyc_Core_Opt**)& _tmpCAB->f1;_tmpCAE=_tmpCAB->f2;}}if(!(_tmpCAE->kind == Cyc_Absyn_RgnKind))goto _LL645;_LL644:
# 4094
 if(_tmpCAE->aliasqual == Cyc_Absyn_Top){
# 4095
*_tmpCAD=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmpCB0=_tmpCAE;goto _LL646;}
# 4097
*_tmpCAD=Cyc_Tcutil_kind_to_bound_opt(_tmpCAE);_tmpCB0=_tmpCAE;goto _LL646;_LL645:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpCAF=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpCAA;if(_tmpCAF->tag != 0)goto _LL647;else{_tmpCB0=_tmpCAF->f1;}}if(!(_tmpCB0->kind == Cyc_Absyn_RgnKind))goto _LL647;_LL646:
# 4099
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp14E3;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp14E2;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp14E1;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp14E0;struct Cyc_List_List*_tmp14DF;effect=((_tmp14DF=_cycalloc(sizeof(*_tmp14DF)),((_tmp14DF->hd=(void*)((_tmp14E3=_cycalloc(sizeof(*_tmp14E3)),((_tmp14E3[0]=((_tmp14E0.tag=23,((_tmp14E0.f1=(void*)((void*)((_tmp14E2=_cycalloc(sizeof(*_tmp14E2)),((_tmp14E2[0]=((_tmp14E1.tag=2,((_tmp14E1.f1=_tmpCA8,_tmp14E1)))),_tmp14E2))))),_tmp14E0)))),_tmp14E3)))),((_tmp14DF->tl=effect,_tmp14DF))))));}goto _LL642;_LL647: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCB1=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCAA;if(_tmpCB1->tag != 2)goto _LL649;else{_tmpCB2=_tmpCB1->f2;_tmpCB3=*_tmpCB2;_tmpCB4=_tmpCB3.kind;if(_tmpCB4 != Cyc_Absyn_IntKind)goto _LL649;}}_LL648:
# 4100
 goto _LL64A;_LL649: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpCB5=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpCAA;if(_tmpCB5->tag != 0)goto _LL64B;else{_tmpCB6=_tmpCB5->f1;_tmpCB7=*_tmpCB6;_tmpCB8=_tmpCB7.kind;if(_tmpCB8 != Cyc_Absyn_IntKind)goto _LL64B;}}_LL64A:
# 4101
 goto _LL642;_LL64B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCB9=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCAA;if(_tmpCB9->tag != 2)goto _LL64D;else{_tmpCBA=_tmpCB9->f1;_tmpCBB=(struct Cyc_Core_Opt**)& _tmpCB9->f1;_tmpCBC=_tmpCB9->f2;_tmpCBD=*_tmpCBC;_tmpCBE=_tmpCBD.kind;if(_tmpCBE != Cyc_Absyn_EffKind)goto _LL64D;}}_LL64C:
# 4103
*_tmpCBB=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL64E;_LL64D: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpCBF=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpCAA;if(_tmpCBF->tag != 0)goto _LL64F;else{_tmpCC0=_tmpCBF->f1;_tmpCC1=*_tmpCC0;_tmpCC2=_tmpCC1.kind;if(_tmpCC2 != Cyc_Absyn_EffKind)goto _LL64F;}}_LL64E:
# 4105
{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp14E9;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp14E8;struct Cyc_List_List*_tmp14E7;effect=((_tmp14E7=_cycalloc(sizeof(*_tmp14E7)),((_tmp14E7->hd=(void*)((_tmp14E9=_cycalloc(sizeof(*_tmp14E9)),((_tmp14E9[0]=((_tmp14E8.tag=2,((_tmp14E8.f1=_tmpCA8,_tmp14E8)))),_tmp14E9)))),((_tmp14E7->tl=effect,_tmp14E7))))));}goto _LL642;_LL64F: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpCC3=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpCAA;if(_tmpCC3->tag != 1)goto _LL651;else{_tmpCC4=_tmpCC3->f1;_tmpCC5=(struct Cyc_Core_Opt**)& _tmpCC3->f1;}}_LL650:
# 4107
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp14EF;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp14EE;struct Cyc_Core_Opt*_tmp14ED;*_tmpCC5=((_tmp14ED=_cycalloc(sizeof(*_tmp14ED)),((_tmp14ED->v=(void*)((_tmp14EF=_cycalloc(sizeof(*_tmp14EF)),((_tmp14EF[0]=((_tmp14EE.tag=2,((_tmp14EE.f1=0,((_tmp14EE.f2=& Cyc_Tcutil_ak,_tmp14EE)))))),_tmp14EF)))),_tmp14ED))));}goto _LL652;_LL651:;_LL652:
# 4110
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp14FE;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp14FD;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp14FC;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp14FB;struct Cyc_List_List*_tmp14FA;effect=((_tmp14FA=_cycalloc(sizeof(*_tmp14FA)),((_tmp14FA->hd=(void*)((_tmp14FE=_cycalloc(sizeof(*_tmp14FE)),((_tmp14FE[0]=((_tmp14FB.tag=25,((_tmp14FB.f1=(void*)((void*)((_tmp14FD=_cycalloc(sizeof(*_tmp14FD)),((_tmp14FD[0]=((_tmp14FC.tag=2,((_tmp14FC.f1=_tmpCA8,_tmp14FC)))),_tmp14FD))))),_tmp14FB)))),_tmp14FE)))),((_tmp14FA->tl=effect,_tmp14FA))))));}goto _LL642;_LL642:;};}}
# 4114
{struct Cyc_List_List*ts=_tmpC51.free_evars;for(0;ts != 0;ts=ts->tl){
# 4115
void*_tmpCD7;int _tmpCD8;struct _tuple29 _tmpCD6=*((struct _tuple29*)ts->hd);_tmpCD7=_tmpCD6.f1;_tmpCD8=_tmpCD6.f2;
# 4116
if(!_tmpCD8)continue;{
# 4117
struct Cyc_Absyn_Kind*_tmpCD9=Cyc_Tcutil_typ_kind(_tmpCD7);struct Cyc_Absyn_Kind _tmpCDA;enum Cyc_Absyn_KindQual _tmpCDB;struct Cyc_Absyn_Kind _tmpCDC;enum Cyc_Absyn_KindQual _tmpCDD;struct Cyc_Absyn_Kind _tmpCDE;enum Cyc_Absyn_KindQual _tmpCDF;_LL654: _tmpCDA=*_tmpCD9;_tmpCDB=_tmpCDA.kind;if(_tmpCDB != Cyc_Absyn_RgnKind)goto _LL656;_LL655:
# 4119
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1504;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1503;struct Cyc_List_List*_tmp1502;effect=((_tmp1502=_cycalloc(sizeof(*_tmp1502)),((_tmp1502->hd=(void*)((_tmp1504=_cycalloc(sizeof(*_tmp1504)),((_tmp1504[0]=((_tmp1503.tag=23,((_tmp1503.f1=(void*)_tmpCD7,_tmp1503)))),_tmp1504)))),((_tmp1502->tl=effect,_tmp1502))))));}goto _LL653;_LL656: _tmpCDC=*_tmpCD9;_tmpCDD=_tmpCDC.kind;if(_tmpCDD != Cyc_Absyn_EffKind)goto _LL658;_LL657:
# 4121
{struct Cyc_List_List*_tmp1505;effect=((_tmp1505=_cycalloc(sizeof(*_tmp1505)),((_tmp1505->hd=_tmpCD7,((_tmp1505->tl=effect,_tmp1505))))));}goto _LL653;_LL658: _tmpCDE=*_tmpCD9;_tmpCDF=_tmpCDE.kind;if(_tmpCDF != Cyc_Absyn_IntKind)goto _LL65A;_LL659:
# 4122
 goto _LL653;_LL65A:;_LL65B:
# 4124
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp150B;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp150A;struct Cyc_List_List*_tmp1509;effect=((_tmp1509=_cycalloc(sizeof(*_tmp1509)),((_tmp1509->hd=(void*)((_tmp150B=_cycalloc(sizeof(*_tmp150B)),((_tmp150B[0]=((_tmp150A.tag=25,((_tmp150A.f1=(void*)_tmpCD7,_tmp150A)))),_tmp150B)))),((_tmp1509->tl=effect,_tmp1509))))));}goto _LL653;_LL653:;};}}{
# 4127
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp150E;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp150D;*_tmpB52=(void*)((_tmp150D=_cycalloc(sizeof(*_tmp150D)),((_tmp150D[0]=((_tmp150E.tag=24,((_tmp150E.f1=effect,_tmp150E)))),_tmp150D))));};}
# 4133
if(*_tmpB50 != 0){
# 4134
struct Cyc_List_List*bs=*_tmpB50;for(0;bs != 0;bs=bs->tl){
# 4135
void*_tmpCE9=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*_tmpCEB;struct Cyc_Core_Opt**_tmpCEC;struct Cyc_Core_Opt*_tmpCEE;struct Cyc_Core_Opt**_tmpCEF;struct Cyc_Absyn_Kind*_tmpCF0;struct Cyc_Absyn_Kind _tmpCF1;enum Cyc_Absyn_KindQual _tmpCF2;enum Cyc_Absyn_AliasQual _tmpCF3;struct Cyc_Core_Opt*_tmpCF5;struct Cyc_Core_Opt**_tmpCF6;struct Cyc_Absyn_Kind*_tmpCF7;struct Cyc_Absyn_Kind _tmpCF8;enum Cyc_Absyn_KindQual _tmpCF9;enum Cyc_Absyn_AliasQual _tmpCFA;struct Cyc_Core_Opt*_tmpCFC;struct Cyc_Core_Opt**_tmpCFD;struct Cyc_Absyn_Kind*_tmpCFE;struct Cyc_Absyn_Kind _tmpCFF;enum Cyc_Absyn_KindQual _tmpD00;enum Cyc_Absyn_AliasQual _tmpD01;struct Cyc_Core_Opt*_tmpD03;struct Cyc_Core_Opt**_tmpD04;struct Cyc_Absyn_Kind*_tmpD05;struct Cyc_Absyn_Kind _tmpD06;enum Cyc_Absyn_KindQual _tmpD07;enum Cyc_Absyn_AliasQual _tmpD08;struct Cyc_Core_Opt*_tmpD0A;struct Cyc_Core_Opt**_tmpD0B;struct Cyc_Absyn_Kind*_tmpD0C;struct Cyc_Absyn_Kind _tmpD0D;enum Cyc_Absyn_KindQual _tmpD0E;enum Cyc_Absyn_AliasQual _tmpD0F;struct Cyc_Core_Opt*_tmpD11;struct Cyc_Core_Opt**_tmpD12;struct Cyc_Absyn_Kind*_tmpD13;struct Cyc_Absyn_Kind _tmpD14;enum Cyc_Absyn_KindQual _tmpD15;enum Cyc_Absyn_AliasQual _tmpD16;struct Cyc_Core_Opt*_tmpD18;struct Cyc_Core_Opt**_tmpD19;struct Cyc_Absyn_Kind*_tmpD1A;struct Cyc_Absyn_Kind _tmpD1B;enum Cyc_Absyn_KindQual _tmpD1C;enum Cyc_Absyn_AliasQual _tmpD1D;struct Cyc_Core_Opt*_tmpD1F;struct Cyc_Core_Opt**_tmpD20;struct Cyc_Absyn_Kind*_tmpD21;struct Cyc_Absyn_Kind _tmpD22;enum Cyc_Absyn_KindQual _tmpD23;enum Cyc_Absyn_AliasQual _tmpD24;struct Cyc_Core_Opt*_tmpD26;struct Cyc_Core_Opt**_tmpD27;struct Cyc_Absyn_Kind*_tmpD28;struct Cyc_Absyn_Kind*_tmpD2A;struct Cyc_Absyn_Kind _tmpD2B;enum Cyc_Absyn_KindQual _tmpD2C;_LL65D: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpCEA=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpCE9;if(_tmpCEA->tag != 1)goto _LL65F;else{_tmpCEB=_tmpCEA->f1;_tmpCEC=(struct Cyc_Core_Opt**)& _tmpCEA->f1;}}_LL65E:
# 4137
{const char*_tmp1512;void*_tmp1511[1];struct Cyc_String_pa_PrintArg_struct _tmp1510;(_tmp1510.tag=0,((_tmp1510.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp1511[0]=& _tmp1510,Cyc_Tcutil_warn(loc,((_tmp1512="Type variable %s unconstrained, assuming boxed",_tag_dyneither(_tmp1512,sizeof(char),47))),_tag_dyneither(_tmp1511,sizeof(void*),1)))))));}
# 4139
_tmpCEF=_tmpCEC;goto _LL660;_LL65F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCED=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCE9;if(_tmpCED->tag != 2)goto _LL661;else{_tmpCEE=_tmpCED->f1;_tmpCEF=(struct Cyc_Core_Opt**)& _tmpCED->f1;_tmpCF0=_tmpCED->f2;_tmpCF1=*_tmpCF0;_tmpCF2=_tmpCF1.kind;if(_tmpCF2 != Cyc_Absyn_BoxKind)goto _LL661;_tmpCF3=_tmpCF1.aliasqual;if(_tmpCF3 != Cyc_Absyn_Top)goto _LL661;}}_LL660:
# 4140
 _tmpCF6=_tmpCEF;goto _LL662;_LL661: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCF4=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCE9;if(_tmpCF4->tag != 2)goto _LL663;else{_tmpCF5=_tmpCF4->f1;_tmpCF6=(struct Cyc_Core_Opt**)& _tmpCF4->f1;_tmpCF7=_tmpCF4->f2;_tmpCF8=*_tmpCF7;_tmpCF9=_tmpCF8.kind;if(_tmpCF9 != Cyc_Absyn_MemKind)goto _LL663;_tmpCFA=_tmpCF8.aliasqual;if(_tmpCFA != Cyc_Absyn_Top)goto _LL663;}}_LL662:
# 4141
 _tmpCFD=_tmpCF6;goto _LL664;_LL663: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCFB=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCE9;if(_tmpCFB->tag != 2)goto _LL665;else{_tmpCFC=_tmpCFB->f1;_tmpCFD=(struct Cyc_Core_Opt**)& _tmpCFB->f1;_tmpCFE=_tmpCFB->f2;_tmpCFF=*_tmpCFE;_tmpD00=_tmpCFF.kind;if(_tmpD00 != Cyc_Absyn_MemKind)goto _LL665;_tmpD01=_tmpCFF.aliasqual;if(_tmpD01 != Cyc_Absyn_Aliasable)goto _LL665;}}_LL664:
# 4142
 _tmpD04=_tmpCFD;goto _LL666;_LL665: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD02=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCE9;if(_tmpD02->tag != 2)goto _LL667;else{_tmpD03=_tmpD02->f1;_tmpD04=(struct Cyc_Core_Opt**)& _tmpD02->f1;_tmpD05=_tmpD02->f2;_tmpD06=*_tmpD05;_tmpD07=_tmpD06.kind;if(_tmpD07 != Cyc_Absyn_AnyKind)goto _LL667;_tmpD08=_tmpD06.aliasqual;if(_tmpD08 != Cyc_Absyn_Top)goto _LL667;}}_LL666:
# 4143
 _tmpD0B=_tmpD04;goto _LL668;_LL667: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD09=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCE9;if(_tmpD09->tag != 2)goto _LL669;else{_tmpD0A=_tmpD09->f1;_tmpD0B=(struct Cyc_Core_Opt**)& _tmpD09->f1;_tmpD0C=_tmpD09->f2;_tmpD0D=*_tmpD0C;_tmpD0E=_tmpD0D.kind;if(_tmpD0E != Cyc_Absyn_AnyKind)goto _LL669;_tmpD0F=_tmpD0D.aliasqual;if(_tmpD0F != Cyc_Absyn_Aliasable)goto _LL669;}}_LL668:
# 4145
*_tmpD0B=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL65C;_LL669: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD10=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCE9;if(_tmpD10->tag != 2)goto _LL66B;else{_tmpD11=_tmpD10->f1;_tmpD12=(struct Cyc_Core_Opt**)& _tmpD10->f1;_tmpD13=_tmpD10->f2;_tmpD14=*_tmpD13;_tmpD15=_tmpD14.kind;if(_tmpD15 != Cyc_Absyn_MemKind)goto _LL66B;_tmpD16=_tmpD14.aliasqual;if(_tmpD16 != Cyc_Absyn_Unique)goto _LL66B;}}_LL66A:
# 4146
 _tmpD19=_tmpD12;goto _LL66C;_LL66B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD17=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCE9;if(_tmpD17->tag != 2)goto _LL66D;else{_tmpD18=_tmpD17->f1;_tmpD19=(struct Cyc_Core_Opt**)& _tmpD17->f1;_tmpD1A=_tmpD17->f2;_tmpD1B=*_tmpD1A;_tmpD1C=_tmpD1B.kind;if(_tmpD1C != Cyc_Absyn_AnyKind)goto _LL66D;_tmpD1D=_tmpD1B.aliasqual;if(_tmpD1D != Cyc_Absyn_Unique)goto _LL66D;}}_LL66C:
# 4148
*_tmpD19=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL65C;_LL66D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD1E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCE9;if(_tmpD1E->tag != 2)goto _LL66F;else{_tmpD1F=_tmpD1E->f1;_tmpD20=(struct Cyc_Core_Opt**)& _tmpD1E->f1;_tmpD21=_tmpD1E->f2;_tmpD22=*_tmpD21;_tmpD23=_tmpD22.kind;if(_tmpD23 != Cyc_Absyn_RgnKind)goto _LL66F;_tmpD24=_tmpD22.aliasqual;if(_tmpD24 != Cyc_Absyn_Top)goto _LL66F;}}_LL66E:
# 4150
*_tmpD20=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL65C;_LL66F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD25=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCE9;if(_tmpD25->tag != 2)goto _LL671;else{_tmpD26=_tmpD25->f1;_tmpD27=(struct Cyc_Core_Opt**)& _tmpD25->f1;_tmpD28=_tmpD25->f2;}}_LL670:
# 4152
*_tmpD27=Cyc_Tcutil_kind_to_bound_opt(_tmpD28);goto _LL65C;_LL671: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpD29=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpCE9;if(_tmpD29->tag != 0)goto _LL673;else{_tmpD2A=_tmpD29->f1;_tmpD2B=*_tmpD2A;_tmpD2C=_tmpD2B.kind;if(_tmpD2C != Cyc_Absyn_MemKind)goto _LL673;}}_LL672:
# 4154
{const char*_tmp1515;void*_tmp1514;(_tmp1514=0,Cyc_Tcutil_terr(loc,((_tmp1515="functions can't abstract M types",_tag_dyneither(_tmp1515,sizeof(char),33))),_tag_dyneither(_tmp1514,sizeof(void*),0)));}goto _LL65C;_LL673:;_LL674:
# 4155
 goto _LL65C;_LL65C:;}}
# 4159
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmpC51.kind_env,*_tmpB50);
# 4160
_tmpC51.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmpC51.r,_tmpC51.free_vars,*_tmpB50);
# 4162
{struct Cyc_List_List*tvs=_tmpC51.free_vars;for(0;tvs != 0;tvs=tvs->tl){
# 4163
struct Cyc_Absyn_Tvar*_tmpD33;int _tmpD34;struct _tuple28 _tmpD32=*((struct _tuple28*)tvs->hd);_tmpD33=_tmpD32.f1;_tmpD34=_tmpD32.f2;
# 4164
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpD33,_tmpD34);}}{
# 4167
struct Cyc_List_List*evs=_tmpC51.free_evars;for(0;evs != 0;evs=evs->tl){
# 4168
void*_tmpD36;int _tmpD37;struct _tuple29 _tmpD35=*((struct _tuple29*)evs->hd);_tmpD36=_tmpD35.f1;_tmpD37=_tmpD35.f2;
# 4169
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmpD36,_tmpD37);}};}
# 4172
_npop_handler(0);goto _LL577;
# 3973
;_pop_region(newr);};}_LL596: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpB5B=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpB14;if(_tmpB5B->tag != 10)goto _LL598;else{_tmpB5C=_tmpB5B->f1;}}_LL597:
# 4175
 for(0;_tmpB5C != 0;_tmpB5C=_tmpB5C->tl){
# 4176
struct _tuple12*_tmpD39=(struct _tuple12*)_tmpB5C->hd;
# 4177
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpD39).f2,1);
# 4178
((*_tmpD39).f1).real_const=
# 4179
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpD39).f1).print_const,(*_tmpD39).f2);}
# 4181
goto _LL577;_LL598: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB5D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpB14;if(_tmpB5D->tag != 12)goto _LL59A;else{_tmpB5E=_tmpB5D->f1;_tmpB5F=_tmpB5D->f2;}}_LL599: {
# 4185
struct _RegionHandle _tmpD3A=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmpD3A;_push_region(aprev_rgn);{
# 4186
struct Cyc_List_List*prev_fields=0;
# 4187
for(0;_tmpB5F != 0;_tmpB5F=_tmpB5F->tl){
# 4188
struct Cyc_Absyn_Aggrfield _tmpD3C;struct _dyneither_ptr*_tmpD3D;struct Cyc_Absyn_Tqual _tmpD3E;struct Cyc_Absyn_Tqual*_tmpD3F;void*_tmpD40;struct Cyc_Absyn_Exp*_tmpD41;struct Cyc_List_List*_tmpD42;struct Cyc_Absyn_Aggrfield*_tmpD3B=(struct Cyc_Absyn_Aggrfield*)_tmpB5F->hd;_tmpD3C=*_tmpD3B;_tmpD3D=_tmpD3C.name;_tmpD3E=_tmpD3C.tq;_tmpD3F=(struct Cyc_Absyn_Tqual*)&(*_tmpD3B).tq;_tmpD40=_tmpD3C.type;_tmpD41=_tmpD3C.width;_tmpD42=_tmpD3C.attributes;
# 4189
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpD3D)){
# 4190
const char*_tmp1519;void*_tmp1518[1];struct Cyc_String_pa_PrintArg_struct _tmp1517;(_tmp1517.tag=0,((_tmp1517.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpD3D),((_tmp1518[0]=& _tmp1517,Cyc_Tcutil_terr(loc,((_tmp1519="duplicate field %s",_tag_dyneither(_tmp1519,sizeof(char),19))),_tag_dyneither(_tmp1518,sizeof(void*),1)))))));}
# 4191
{const char*_tmp151A;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpD3D,((_tmp151A="",_tag_dyneither(_tmp151A,sizeof(char),1))))!= 0){
# 4192
struct Cyc_List_List*_tmp151B;prev_fields=((_tmp151B=_region_malloc(aprev_rgn,sizeof(*_tmp151B)),((_tmp151B->hd=_tmpD3D,((_tmp151B->tl=prev_fields,_tmp151B))))));}}
# 4193
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpD40,1);
# 4194
_tmpD3F->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpD3F->print_const,_tmpD40);
# 4195
if(_tmpB5E == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmpD40)){
# 4197
const char*_tmp151F;void*_tmp151E[1];struct Cyc_String_pa_PrintArg_struct _tmp151D;(_tmp151D.tag=0,((_tmp151D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpD3D),((_tmp151E[0]=& _tmp151D,Cyc_Tcutil_warn(loc,((_tmp151F="union member %s is not `bits-only' so it can only be written and not read",_tag_dyneither(_tmp151F,sizeof(char),74))),_tag_dyneither(_tmp151E,sizeof(void*),1)))))));}
# 4198
Cyc_Tcutil_check_bitfield(loc,te,_tmpD40,_tmpD41,_tmpD3D);
# 4199
Cyc_Tcutil_check_field_atts(loc,_tmpD3D,_tmpD42);}}
# 4202
_npop_handler(0);goto _LL577;
# 4185
;_pop_region(aprev_rgn);}_LL59A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB60=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB14;if(_tmpB60->tag != 11)goto _LL59C;else{_tmpB61=_tmpB60->f1;_tmpB62=_tmpB61.aggr_info;_tmpB63=(union Cyc_Absyn_AggrInfoU*)&(_tmpB60->f1).aggr_info;_tmpB64=_tmpB61.targs;_tmpB65=(struct Cyc_List_List**)&(_tmpB60->f1).targs;}}_LL59B:
# 4205
{union Cyc_Absyn_AggrInfoU _tmpD4B=*_tmpB63;struct _tuple4 _tmpD4C;enum Cyc_Absyn_AggrKind _tmpD4D;struct _tuple2*_tmpD4E;struct Cyc_Core_Opt*_tmpD4F;struct Cyc_Absyn_Aggrdecl**_tmpD50;struct Cyc_Absyn_Aggrdecl*_tmpD51;_LL676: if((_tmpD4B.UnknownAggr).tag != 1)goto _LL678;_tmpD4C=(struct _tuple4)(_tmpD4B.UnknownAggr).val;_tmpD4D=_tmpD4C.f1;_tmpD4E=_tmpD4C.f2;_tmpD4F=_tmpD4C.f3;_LL677: {
# 4207
struct Cyc_Absyn_Aggrdecl**adp;
# 4208
{struct _handler_cons _tmpD52;_push_handler(& _tmpD52);{int _tmpD54=0;if(setjmp(_tmpD52.handler))_tmpD54=1;if(!_tmpD54){
# 4209
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpD4E);{
# 4210
struct Cyc_Absyn_Aggrdecl*_tmpD55=*adp;
# 4211
if(_tmpD55->kind != _tmpD4D){
# 4212
if(_tmpD55->kind == Cyc_Absyn_StructA){
# 4213
const char*_tmp1524;void*_tmp1523[2];struct Cyc_String_pa_PrintArg_struct _tmp1522;struct Cyc_String_pa_PrintArg_struct _tmp1521;(_tmp1521.tag=0,((_tmp1521.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4214
Cyc_Absynpp_qvar2string(_tmpD4E)),((_tmp1522.tag=0,((_tmp1522.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4213
Cyc_Absynpp_qvar2string(_tmpD4E)),((_tmp1523[0]=& _tmp1522,((_tmp1523[1]=& _tmp1521,Cyc_Tcutil_terr(loc,((_tmp1524="expecting struct %s instead of union %s",_tag_dyneither(_tmp1524,sizeof(char),40))),_tag_dyneither(_tmp1523,sizeof(void*),2)))))))))))));}else{
# 4216
const char*_tmp1529;void*_tmp1528[2];struct Cyc_String_pa_PrintArg_struct _tmp1527;struct Cyc_String_pa_PrintArg_struct _tmp1526;(_tmp1526.tag=0,((_tmp1526.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4217
Cyc_Absynpp_qvar2string(_tmpD4E)),((_tmp1527.tag=0,((_tmp1527.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4216
Cyc_Absynpp_qvar2string(_tmpD4E)),((_tmp1528[0]=& _tmp1527,((_tmp1528[1]=& _tmp1526,Cyc_Tcutil_terr(loc,((_tmp1529="expecting union %s instead of struct %s",_tag_dyneither(_tmp1529,sizeof(char),40))),_tag_dyneither(_tmp1528,sizeof(void*),2)))))))))))));}}
# 4219
if((unsigned int)_tmpD4F  && (int)_tmpD4F->v){
# 4220
if(!((unsigned int)_tmpD55->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD55->impl))->tagged){
# 4221
const char*_tmp152D;void*_tmp152C[1];struct Cyc_String_pa_PrintArg_struct _tmp152B;(_tmp152B.tag=0,((_tmp152B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4222
Cyc_Absynpp_qvar2string(_tmpD4E)),((_tmp152C[0]=& _tmp152B,Cyc_Tcutil_terr(loc,((_tmp152D="@tagged qualfiers don't agree on union %s",_tag_dyneither(_tmp152D,sizeof(char),42))),_tag_dyneither(_tmp152C,sizeof(void*),1)))))));}}
# 4225
*_tmpB63=Cyc_Absyn_KnownAggr(adp);};
# 4209
;_pop_handler();}else{void*_tmpD53=(void*)_exn_thrown;void*_tmpD62=_tmpD53;_LL67B: {struct Cyc_Dict_Absent_exn_struct*_tmpD63=(struct Cyc_Dict_Absent_exn_struct*)_tmpD62;if(_tmpD63->tag != Cyc_Dict_Absent)goto _LL67D;}_LL67C: {
# 4229
struct Cyc_Tcenv_Genv*_tmpD64=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
# 4230
struct Cyc_Absyn_Aggrdecl*_tmp152E;struct Cyc_Absyn_Aggrdecl*_tmpD65=(_tmp152E=_cycalloc(sizeof(*_tmp152E)),((_tmp152E->kind=_tmpD4D,((_tmp152E->sc=Cyc_Absyn_Extern,((_tmp152E->name=_tmpD4E,((_tmp152E->tvs=0,((_tmp152E->impl=0,((_tmp152E->attributes=0,_tmp152E)))))))))))));
# 4231
Cyc_Tc_tcAggrdecl(te,_tmpD64,loc,_tmpD65);
# 4232
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpD4E);
# 4233
*_tmpB63=Cyc_Absyn_KnownAggr(adp);
# 4235
if(*_tmpB65 != 0){
# 4236
{const char*_tmp1532;void*_tmp1531[1];struct Cyc_String_pa_PrintArg_struct _tmp1530;(_tmp1530.tag=0,((_tmp1530.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4237
Cyc_Absynpp_qvar2string(_tmpD4E)),((_tmp1531[0]=& _tmp1530,Cyc_Tcutil_terr(loc,((_tmp1532="declare parameterized type %s before using",_tag_dyneither(_tmp1532,sizeof(char),43))),_tag_dyneither(_tmp1531,sizeof(void*),1)))))));}
# 4238
return cvtenv;}
# 4240
goto _LL67A;}_LL67D:;_LL67E:(void)_throw(_tmpD62);_LL67A:;}};}
# 4242
_tmpD51=*adp;goto _LL679;}_LL678: if((_tmpD4B.KnownAggr).tag != 2)goto _LL675;_tmpD50=(struct Cyc_Absyn_Aggrdecl**)(_tmpD4B.KnownAggr).val;_tmpD51=*_tmpD50;_LL679: {
# 4244
struct Cyc_List_List*tvs=_tmpD51->tvs;
# 4245
struct Cyc_List_List*ts=*_tmpB65;
# 4246
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
# 4247
struct Cyc_Absyn_Tvar*_tmpD6A=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4248
void*_tmpD6B=(void*)ts->hd;
# 4252
{struct _tuple0 _tmp1533;struct _tuple0 _tmpD6D=(_tmp1533.f1=Cyc_Absyn_compress_kb(_tmpD6A->kind),((_tmp1533.f2=_tmpD6B,_tmp1533)));void*_tmpD6E;void*_tmpD70;struct Cyc_Absyn_Tvar*_tmpD72;_LL680: _tmpD6E=_tmpD6D.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpD6F=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpD6E;if(_tmpD6F->tag != 1)goto _LL682;};_tmpD70=_tmpD6D.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpD71=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpD70;if(_tmpD71->tag != 2)goto _LL682;else{_tmpD72=_tmpD71->f1;}};_LL681:
# 4254
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpD72);
# 4255
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpD72,1);
# 4256
continue;_LL682:;_LL683:
# 4257
 goto _LL67F;_LL67F:;}{
# 4259
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
# 4260
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
# 4261
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4263
if(ts != 0){
# 4264
const char*_tmp1537;void*_tmp1536[1];struct Cyc_String_pa_PrintArg_struct _tmp1535;(_tmp1535.tag=0,((_tmp1535.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD51->name)),((_tmp1536[0]=& _tmp1535,Cyc_Tcutil_terr(loc,((_tmp1537="too many parameters for type %s",_tag_dyneither(_tmp1537,sizeof(char),32))),_tag_dyneither(_tmp1536,sizeof(void*),1)))))));}
# 4265
if(tvs != 0){
# 4267
struct Cyc_List_List*hidden_ts=0;
# 4268
for(0;tvs != 0;tvs=tvs->tl){
# 4269
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
# 4270
void*e=Cyc_Absyn_new_evar(0,0);
# 4271
{struct Cyc_List_List*_tmp1538;hidden_ts=((_tmp1538=_cycalloc(sizeof(*_tmp1538)),((_tmp1538->hd=e,((_tmp1538->tl=hidden_ts,_tmp1538))))));}
# 4272
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);}
# 4274
*_tmpB65=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpB65,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}}_LL675:;}
# 4277
goto _LL577;_LL59C: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB66=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpB14;if(_tmpB66->tag != 17)goto _LL59E;else{_tmpB67=_tmpB66->f1;_tmpB68=_tmpB66->f2;_tmpB69=(struct Cyc_List_List**)& _tmpB66->f2;_tmpB6A=_tmpB66->f3;_tmpB6B=(struct Cyc_Absyn_Typedefdecl**)& _tmpB66->f3;_tmpB6C=(void**)& _tmpB66->f4;_tmpB6D=(void**)((void**)& _tmpB66->f4);}}_LL59D: {
# 4280
struct Cyc_List_List*targs=*_tmpB69;
# 4282
struct Cyc_Absyn_Typedefdecl*td;
# 4283
{struct _handler_cons _tmpD77;_push_handler(& _tmpD77);{int _tmpD79=0;if(setjmp(_tmpD77.handler))_tmpD79=1;if(!_tmpD79){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmpB67);;_pop_handler();}else{void*_tmpD78=(void*)_exn_thrown;void*_tmpD7B=_tmpD78;_LL685: {struct Cyc_Dict_Absent_exn_struct*_tmpD7C=(struct Cyc_Dict_Absent_exn_struct*)_tmpD7B;if(_tmpD7C->tag != Cyc_Dict_Absent)goto _LL687;}_LL686:
# 4285
{const char*_tmp153C;void*_tmp153B[1];struct Cyc_String_pa_PrintArg_struct _tmp153A;(_tmp153A.tag=0,((_tmp153A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB67)),((_tmp153B[0]=& _tmp153A,Cyc_Tcutil_terr(loc,((_tmp153C="unbound typedef name %s",_tag_dyneither(_tmp153C,sizeof(char),24))),_tag_dyneither(_tmp153B,sizeof(void*),1)))))));}
# 4286
return cvtenv;_LL687:;_LL688:(void)_throw(_tmpD7B);_LL684:;}};}
# 4288
*_tmpB6B=(struct Cyc_Absyn_Typedefdecl*)td;
# 4290
_tmpB67[0]=(td->name)[0];{
# 4291
struct Cyc_List_List*tvs=td->tvs;
# 4292
struct Cyc_List_List*ts=targs;
# 4293
struct _RegionHandle _tmpD80=_new_region("temp");struct _RegionHandle*temp=& _tmpD80;_push_region(temp);{
# 4294
struct Cyc_List_List*inst=0;
# 4296
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
# 4297
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);
# 4301
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
# 4302
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
# 4303
struct _tuple16*_tmp153F;struct Cyc_List_List*_tmp153E;inst=((_tmp153E=_region_malloc(temp,sizeof(*_tmp153E)),((_tmp153E->hd=((_tmp153F=_region_malloc(temp,sizeof(*_tmp153F)),((_tmp153F->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp153F->f2=(void*)ts->hd,_tmp153F)))))),((_tmp153E->tl=inst,_tmp153E))))));};}
# 4305
if(ts != 0){
# 4306
const char*_tmp1543;void*_tmp1542[1];struct Cyc_String_pa_PrintArg_struct _tmp1541;(_tmp1541.tag=0,((_tmp1541.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB67)),((_tmp1542[0]=& _tmp1541,Cyc_Tcutil_terr(loc,((_tmp1543="too many parameters for typedef %s",_tag_dyneither(_tmp1543,sizeof(char),35))),_tag_dyneither(_tmp1542,sizeof(void*),1)))))));}
# 4307
if(tvs != 0){
# 4308
struct Cyc_List_List*hidden_ts=0;
# 4310
for(0;tvs != 0;tvs=tvs->tl){
# 4311
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);
# 4312
void*e=Cyc_Absyn_new_evar(0,0);
# 4313
{struct Cyc_List_List*_tmp1544;hidden_ts=((_tmp1544=_cycalloc(sizeof(*_tmp1544)),((_tmp1544->hd=e,((_tmp1544->tl=hidden_ts,_tmp1544))))));}
# 4314
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);{
# 4315
struct _tuple16*_tmp1547;struct Cyc_List_List*_tmp1546;inst=(struct Cyc_List_List*)((_tmp1546=_cycalloc(sizeof(*_tmp1546)),((_tmp1546->hd=(struct _tuple16*)((_tmp1547=_cycalloc(sizeof(*_tmp1547)),((_tmp1547->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1547->f2=e,_tmp1547)))))),((_tmp1546->tl=inst,_tmp1546))))));};}
# 4318
*_tmpB69=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4320
if(td->defn != 0){
# 4321
void*new_typ=
# 4322
inst == 0?(void*)_check_null(td->defn):
# 4323
 Cyc_Tcutil_rsubstitute(temp,inst,(void*)_check_null(td->defn));
# 4324
*_tmpB6D=(void*)new_typ;}}
# 4327
_npop_handler(0);goto _LL577;
# 4293
;_pop_region(temp);};}_LL59E: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpB6E=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmpB14;if(_tmpB6E->tag != 22)goto _LL5A0;}_LL59F:
# 4328
 goto _LL5A1;_LL5A0: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpB6F=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmpB14;if(_tmpB6F->tag != 21)goto _LL5A2;}_LL5A1:
# 4329
 goto _LL5A3;_LL5A2: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpB70=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmpB14;if(_tmpB70->tag != 20)goto _LL5A4;}_LL5A3:
# 4330
 goto _LL577;_LL5A4: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpB71=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpB14;if(_tmpB71->tag != 15)goto _LL5A6;else{_tmpB72=(void*)_tmpB71->f1;}}_LL5A5:
# 4332
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpB72,1);goto _LL577;_LL5A6: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpB73=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmpB14;if(_tmpB73->tag != 16)goto _LL5A8;else{_tmpB74=(void*)_tmpB73->f1;_tmpB75=(void*)_tmpB73->f2;}}_LL5A7:
# 4335
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpB74,0);
# 4336
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpB75,1);
# 4337
goto _LL577;_LL5A8: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpB76=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmpB14;if(_tmpB76->tag != 23)goto _LL5AA;else{_tmpB77=(void*)_tmpB76->f1;}}_LL5A9:
# 4339
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpB77,1);goto _LL577;_LL5AA: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpB78=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmpB14;if(_tmpB78->tag != 25)goto _LL5AC;else{_tmpB79=(void*)_tmpB78->f1;}}_LL5AB:
# 4341
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB79,1);goto _LL577;_LL5AC: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpB7A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmpB14;if(_tmpB7A->tag != 24)goto _LL577;else{_tmpB7B=_tmpB7A->f1;}}_LL5AD:
# 4343
 for(0;_tmpB7B != 0;_tmpB7B=_tmpB7B->tl){
# 4344
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmpB7B->hd,1);}
# 4345
goto _LL577;_LL577:;}
# 4347
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){
# 4348
{void*_tmpD89=t;struct Cyc_Core_Opt*_tmpD8B;void*_tmpD8C;_LL68A: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpD8A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpD89;if(_tmpD8A->tag != 1)goto _LL68C;else{_tmpD8B=_tmpD8A->f1;_tmpD8C=(void*)_tmpD8A->f2;}}_LL68B: {
# 4350
struct _dyneither_ptr s;
# 4351
{struct Cyc_Core_Opt*_tmpD8D=_tmpD8B;struct Cyc_Core_Opt _tmpD8E;struct Cyc_Absyn_Kind*_tmpD8F;_LL68F: if(_tmpD8D != 0)goto _LL691;_LL690:
# 4352
{const char*_tmp1548;s=((_tmp1548="kind=NULL ",_tag_dyneither(_tmp1548,sizeof(char),11)));}goto _LL68E;_LL691: if(_tmpD8D == 0)goto _LL68E;_tmpD8E=*_tmpD8D;_tmpD8F=(struct Cyc_Absyn_Kind*)_tmpD8E.v;_LL692:
# 4353
{const char*_tmp154C;void*_tmp154B[1];struct Cyc_String_pa_PrintArg_struct _tmp154A;s=(struct _dyneither_ptr)((_tmp154A.tag=0,((_tmp154A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmpD8F)),((_tmp154B[0]=& _tmp154A,Cyc_aprintf(((_tmp154C="kind=%s ",_tag_dyneither(_tmp154C,sizeof(char),9))),_tag_dyneither(_tmp154B,sizeof(void*),1))))))));}goto _LL68E;_LL68E:;}
# 4355
if(_tmpD8C == 0){
# 4356
const char*_tmp1550;void*_tmp154F[1];struct Cyc_String_pa_PrintArg_struct _tmp154E;s=(struct _dyneither_ptr)((_tmp154E.tag=0,((_tmp154E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp154F[0]=& _tmp154E,Cyc_aprintf(((_tmp1550="%s ref=NULL",_tag_dyneither(_tmp1550,sizeof(char),12))),_tag_dyneither(_tmp154F,sizeof(void*),1))))))));}else{
# 4358
const char*_tmp1555;void*_tmp1554[2];struct Cyc_String_pa_PrintArg_struct _tmp1553;struct Cyc_String_pa_PrintArg_struct _tmp1552;s=(struct _dyneither_ptr)((_tmp1552.tag=0,((_tmp1552.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_tmpD8C)),((_tmp1553.tag=0,((_tmp1553.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1554[0]=& _tmp1553,((_tmp1554[1]=& _tmp1552,Cyc_aprintf(((_tmp1555="%s ref=%s",_tag_dyneither(_tmp1555,sizeof(char),10))),_tag_dyneither(_tmp1554,sizeof(void*),2))))))))))))));}
# 4360
goto _LL689;}_LL68C:;_LL68D:
# 4361
 goto _LL689;_LL689:;}{
# 4363
const char*_tmp155B;void*_tmp155A[3];struct Cyc_String_pa_PrintArg_struct _tmp1559;struct Cyc_String_pa_PrintArg_struct _tmp1558;struct Cyc_String_pa_PrintArg_struct _tmp1557;(_tmp1557.tag=0,((_tmp1557.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4364
Cyc_Absynpp_kind2string(expected_kind)),((_tmp1558.tag=0,((_tmp1558.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp1559.tag=0,((_tmp1559.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp155A[0]=& _tmp1559,((_tmp155A[1]=& _tmp1558,((_tmp155A[2]=& _tmp1557,Cyc_Tcutil_terr(loc,((_tmp155B="type %s has kind %s but as used here needs kind %s",_tag_dyneither(_tmp155B,sizeof(char),51))),_tag_dyneither(_tmp155A,sizeof(void*),3)))))))))))))))))));};}
# 4366
return cvtenv;}
# 4374
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4376
{void*_tmpDA0=e->r;struct Cyc_List_List*_tmpDA6;struct Cyc_Absyn_Exp*_tmpDA8;struct Cyc_Absyn_Exp*_tmpDA9;struct Cyc_Absyn_Exp*_tmpDAA;struct Cyc_Absyn_Exp*_tmpDAC;struct Cyc_Absyn_Exp*_tmpDAD;struct Cyc_Absyn_Exp*_tmpDAF;struct Cyc_Absyn_Exp*_tmpDB0;struct Cyc_Absyn_Exp*_tmpDB2;struct Cyc_Absyn_Exp*_tmpDB3;void*_tmpDB5;struct Cyc_Absyn_Exp*_tmpDB6;void*_tmpDB8;void*_tmpDBA;void*_tmpDBC;struct Cyc_Absyn_Exp*_tmpDBE;_LL694: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpDA1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDA0;if(_tmpDA1->tag != 0)goto _LL696;}_LL695:
# 4377
 goto _LL697;_LL696: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpDA2=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpDA0;if(_tmpDA2->tag != 30)goto _LL698;}_LL697:
# 4378
 goto _LL699;_LL698: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpDA3=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpDA0;if(_tmpDA3->tag != 31)goto _LL69A;}_LL699:
# 4379
 goto _LL69B;_LL69A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpDA4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDA0;if(_tmpDA4->tag != 1)goto _LL69C;}_LL69B:
# 4380
 goto _LL693;_LL69C: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpDA5=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDA0;if(_tmpDA5->tag != 2)goto _LL69E;else{_tmpDA6=_tmpDA5->f2;}}_LL69D:
# 4382
 for(0;_tmpDA6 != 0;_tmpDA6=_tmpDA6->tl){
# 4383
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmpDA6->hd,te,cvtenv);}
# 4384
goto _LL693;_LL69E: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpDA7=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDA0;if(_tmpDA7->tag != 5)goto _LL6A0;else{_tmpDA8=_tmpDA7->f1;_tmpDA9=_tmpDA7->f2;_tmpDAA=_tmpDA7->f3;}}_LL69F:
# 4386
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDA8,te,cvtenv);
# 4387
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDA9,te,cvtenv);
# 4388
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDAA,te,cvtenv);
# 4389
goto _LL693;_LL6A0: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpDAB=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDA0;if(_tmpDAB->tag != 6)goto _LL6A2;else{_tmpDAC=_tmpDAB->f1;_tmpDAD=_tmpDAB->f2;}}_LL6A1:
# 4390
 _tmpDAF=_tmpDAC;_tmpDB0=_tmpDAD;goto _LL6A3;_LL6A2: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpDAE=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDA0;if(_tmpDAE->tag != 7)goto _LL6A4;else{_tmpDAF=_tmpDAE->f1;_tmpDB0=_tmpDAE->f2;}}_LL6A3:
# 4391
 _tmpDB2=_tmpDAF;_tmpDB3=_tmpDB0;goto _LL6A5;_LL6A4: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpDB1=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDA0;if(_tmpDB1->tag != 8)goto _LL6A6;else{_tmpDB2=_tmpDB1->f1;_tmpDB3=_tmpDB1->f2;}}_LL6A5:
# 4393
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDB2,te,cvtenv);
# 4394
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDB3,te,cvtenv);
# 4395
goto _LL693;_LL6A6: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpDB4=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDA0;if(_tmpDB4->tag != 13)goto _LL6A8;else{_tmpDB5=(void*)_tmpDB4->f1;_tmpDB6=_tmpDB4->f2;}}_LL6A7:
# 4397
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDB6,te,cvtenv);
# 4398
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpDB5,1);
# 4399
goto _LL693;_LL6A8: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpDB7=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDA0;if(_tmpDB7->tag != 18)goto _LL6AA;else{_tmpDB8=(void*)_tmpDB7->f1;}}_LL6A9:
# 4400
 _tmpDBA=_tmpDB8;goto _LL6AB;_LL6AA: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpDB9=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpDA0;if(_tmpDB9->tag != 16)goto _LL6AC;else{_tmpDBA=(void*)_tmpDB9->f1;}}_LL6AB:
# 4402
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpDBA,1);
# 4403
goto _LL693;_LL6AC: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpDBB=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpDA0;if(_tmpDBB->tag != 37)goto _LL6AE;else{_tmpDBC=(void*)_tmpDBB->f1;}}_LL6AD:
# 4405
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpDBC,1);
# 4406
goto _LL693;_LL6AE: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpDBD=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpDA0;if(_tmpDBD->tag != 17)goto _LL6B0;else{_tmpDBE=_tmpDBD->f1;}}_LL6AF:
# 4408
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDBE,te,cvtenv);
# 4409
goto _LL693;_LL6B0:;_LL6B1: {
# 4411
const char*_tmp155E;void*_tmp155D;(_tmp155D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp155E="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",_tag_dyneither(_tmp155E,sizeof(char),66))),_tag_dyneither(_tmp155D,sizeof(void*),0)));}_LL693:;}
# 4413
return cvtenv;}
# 4416
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){
# 4421
struct Cyc_List_List*_tmpDC1=cvt.kind_env;
# 4422
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);
# 4424
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
# 4425
struct Cyc_Absyn_Tvar*_tmpDC3;struct _tuple28 _tmpDC2=*((struct _tuple28*)vs->hd);_tmpDC3=_tmpDC2.f1;
# 4426
cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmpDC1,_tmpDC3);}}
# 4432
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
# 4433
void*_tmpDC5;struct _tuple29 _tmpDC4=*((struct _tuple29*)evs->hd);_tmpDC5=_tmpDC4.f1;{
# 4434
void*_tmpDC6=Cyc_Tcutil_compress(_tmpDC5);struct Cyc_Core_Opt*_tmpDC8;struct Cyc_Core_Opt**_tmpDC9;_LL6B3: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpDC7=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpDC6;if(_tmpDC7->tag != 1)goto _LL6B5;else{_tmpDC8=_tmpDC7->f4;_tmpDC9=(struct Cyc_Core_Opt**)& _tmpDC7->f4;}}_LL6B4:
# 4436
 if(*_tmpDC9 == 0){
# 4437
struct Cyc_Core_Opt*_tmp155F;*_tmpDC9=((_tmp155F=_cycalloc(sizeof(*_tmp155F)),((_tmp155F->v=_tmpDC1,_tmp155F))));}else{
# 4440
struct Cyc_List_List*_tmpDCB=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmpDC9))->v;
# 4441
struct Cyc_List_List*_tmpDCC=0;
# 4442
for(0;_tmpDCB != 0;_tmpDCB=_tmpDCB->tl){
# 4443
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmpDC1,(struct Cyc_Absyn_Tvar*)_tmpDCB->hd)){
# 4444
struct Cyc_List_List*_tmp1560;_tmpDCC=((_tmp1560=_cycalloc(sizeof(*_tmp1560)),((_tmp1560->hd=(struct Cyc_Absyn_Tvar*)_tmpDCB->hd,((_tmp1560->tl=_tmpDCC,_tmp1560))))));}}{
# 4445
struct Cyc_Core_Opt*_tmp1561;*_tmpDC9=((_tmp1561=_cycalloc(sizeof(*_tmp1561)),((_tmp1561->v=_tmpDCC,_tmp1561))));};}
# 4447
goto _LL6B2;_LL6B5:;_LL6B6:
# 4448
 goto _LL6B2;_LL6B2:;};}}
# 4451
return cvt;}
# 4458
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
# 4459
int generalize_evars=Cyc_Tcutil_is_function_type(t);
# 4460
struct Cyc_List_List*_tmpDCF=Cyc_Tcenv_lookup_type_vars(te);
# 4461
struct _RegionHandle _tmpDD0=_new_region("temp");struct _RegionHandle*temp=& _tmpDD0;_push_region(temp);
# 4462
{struct Cyc_Tcutil_CVTEnv _tmp1562;struct Cyc_Tcutil_CVTEnv _tmpDD1=Cyc_Tcutil_check_valid_type(loc,te,(
# 4463
(_tmp1562.r=temp,((_tmp1562.kind_env=_tmpDCF,((_tmp1562.free_vars=0,((_tmp1562.free_evars=0,((_tmp1562.generalize_evars=generalize_evars,((_tmp1562.fn_result=0,_tmp1562)))))))))))),& Cyc_Tcutil_tmk,t);
# 4466
struct Cyc_List_List*_tmpDD2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpDD1.free_vars);
# 4467
struct Cyc_List_List*_tmpDD3=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpDD1.free_evars);
# 4470
if(_tmpDCF != 0){
# 4471
struct Cyc_List_List*_tmpDD4=0;
# 4472
{struct Cyc_List_List*_tmpDD5=_tmpDD2;for(0;(unsigned int)_tmpDD5;_tmpDD5=_tmpDD5->tl){
# 4473
struct Cyc_Absyn_Tvar*_tmpDD6=(struct Cyc_Absyn_Tvar*)_tmpDD5->hd;
# 4474
int found=0;
# 4475
{struct Cyc_List_List*_tmpDD7=_tmpDCF;for(0;(unsigned int)_tmpDD7;_tmpDD7=_tmpDD7->tl){
# 4476
if(Cyc_Absyn_tvar_cmp(_tmpDD6,(struct Cyc_Absyn_Tvar*)_tmpDD7->hd)== 0){found=1;break;}}}
# 4477
if(!found){
# 4478
struct Cyc_List_List*_tmp1563;_tmpDD4=((_tmp1563=_region_malloc(temp,sizeof(*_tmp1563)),((_tmp1563->hd=(struct Cyc_Absyn_Tvar*)_tmpDD5->hd,((_tmp1563->tl=_tmpDD4,_tmp1563))))));}}}
# 4480
_tmpDD2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpDD4);}
# 4486
{struct Cyc_List_List*x=_tmpDD2;for(0;x != 0;x=x->tl){
# 4487
void*_tmpDD9=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmpDDB;struct Cyc_Core_Opt**_tmpDDC;struct Cyc_Core_Opt*_tmpDDE;struct Cyc_Core_Opt**_tmpDDF;struct Cyc_Absyn_Kind*_tmpDE0;struct Cyc_Absyn_Kind _tmpDE1;enum Cyc_Absyn_KindQual _tmpDE2;enum Cyc_Absyn_AliasQual _tmpDE3;struct Cyc_Core_Opt*_tmpDE5;struct Cyc_Core_Opt**_tmpDE6;struct Cyc_Absyn_Kind*_tmpDE7;struct Cyc_Absyn_Kind _tmpDE8;enum Cyc_Absyn_KindQual _tmpDE9;enum Cyc_Absyn_AliasQual _tmpDEA;struct Cyc_Core_Opt*_tmpDEC;struct Cyc_Core_Opt**_tmpDED;struct Cyc_Absyn_Kind*_tmpDEE;struct Cyc_Absyn_Kind _tmpDEF;enum Cyc_Absyn_KindQual _tmpDF0;enum Cyc_Absyn_AliasQual _tmpDF1;struct Cyc_Core_Opt*_tmpDF3;struct Cyc_Core_Opt**_tmpDF4;struct Cyc_Absyn_Kind*_tmpDF5;struct Cyc_Absyn_Kind _tmpDF6;enum Cyc_Absyn_KindQual _tmpDF7;enum Cyc_Absyn_AliasQual _tmpDF8;struct Cyc_Core_Opt*_tmpDFA;struct Cyc_Core_Opt**_tmpDFB;struct Cyc_Absyn_Kind*_tmpDFC;struct Cyc_Absyn_Kind _tmpDFD;enum Cyc_Absyn_KindQual _tmpDFE;enum Cyc_Absyn_AliasQual _tmpDFF;struct Cyc_Core_Opt*_tmpE01;struct Cyc_Core_Opt**_tmpE02;struct Cyc_Absyn_Kind*_tmpE03;struct Cyc_Absyn_Kind*_tmpE05;struct Cyc_Absyn_Kind _tmpE06;enum Cyc_Absyn_KindQual _tmpE07;enum Cyc_Absyn_AliasQual _tmpE08;_LL6B8: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpDDA=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpDD9;if(_tmpDDA->tag != 1)goto _LL6BA;else{_tmpDDB=_tmpDDA->f1;_tmpDDC=(struct Cyc_Core_Opt**)& _tmpDDA->f1;}}_LL6B9:
# 4488
 _tmpDDF=_tmpDDC;goto _LL6BB;_LL6BA: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDDD=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDD9;if(_tmpDDD->tag != 2)goto _LL6BC;else{_tmpDDE=_tmpDDD->f1;_tmpDDF=(struct Cyc_Core_Opt**)& _tmpDDD->f1;_tmpDE0=_tmpDDD->f2;_tmpDE1=*_tmpDE0;_tmpDE2=_tmpDE1.kind;if(_tmpDE2 != Cyc_Absyn_BoxKind)goto _LL6BC;_tmpDE3=_tmpDE1.aliasqual;if(_tmpDE3 != Cyc_Absyn_Top)goto _LL6BC;}}_LL6BB:
# 4489
 _tmpDE6=_tmpDDF;goto _LL6BD;_LL6BC: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDE4=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDD9;if(_tmpDE4->tag != 2)goto _LL6BE;else{_tmpDE5=_tmpDE4->f1;_tmpDE6=(struct Cyc_Core_Opt**)& _tmpDE4->f1;_tmpDE7=_tmpDE4->f2;_tmpDE8=*_tmpDE7;_tmpDE9=_tmpDE8.kind;if(_tmpDE9 != Cyc_Absyn_MemKind)goto _LL6BE;_tmpDEA=_tmpDE8.aliasqual;if(_tmpDEA != Cyc_Absyn_Top)goto _LL6BE;}}_LL6BD:
# 4490
 _tmpDED=_tmpDE6;goto _LL6BF;_LL6BE: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDEB=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDD9;if(_tmpDEB->tag != 2)goto _LL6C0;else{_tmpDEC=_tmpDEB->f1;_tmpDED=(struct Cyc_Core_Opt**)& _tmpDEB->f1;_tmpDEE=_tmpDEB->f2;_tmpDEF=*_tmpDEE;_tmpDF0=_tmpDEF.kind;if(_tmpDF0 != Cyc_Absyn_MemKind)goto _LL6C0;_tmpDF1=_tmpDEF.aliasqual;if(_tmpDF1 != Cyc_Absyn_Aliasable)goto _LL6C0;}}_LL6BF:
# 4492
*_tmpDED=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6B7;_LL6C0: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDF2=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDD9;if(_tmpDF2->tag != 2)goto _LL6C2;else{_tmpDF3=_tmpDF2->f1;_tmpDF4=(struct Cyc_Core_Opt**)& _tmpDF2->f1;_tmpDF5=_tmpDF2->f2;_tmpDF6=*_tmpDF5;_tmpDF7=_tmpDF6.kind;if(_tmpDF7 != Cyc_Absyn_MemKind)goto _LL6C2;_tmpDF8=_tmpDF6.aliasqual;if(_tmpDF8 != Cyc_Absyn_Unique)goto _LL6C2;}}_LL6C1:
# 4494
*_tmpDF4=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL6B7;_LL6C2: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDF9=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDD9;if(_tmpDF9->tag != 2)goto _LL6C4;else{_tmpDFA=_tmpDF9->f1;_tmpDFB=(struct Cyc_Core_Opt**)& _tmpDF9->f1;_tmpDFC=_tmpDF9->f2;_tmpDFD=*_tmpDFC;_tmpDFE=_tmpDFD.kind;if(_tmpDFE != Cyc_Absyn_RgnKind)goto _LL6C4;_tmpDFF=_tmpDFD.aliasqual;if(_tmpDFF != Cyc_Absyn_Top)goto _LL6C4;}}_LL6C3:
# 4496
*_tmpDFB=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL6B7;_LL6C4: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpE00=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDD9;if(_tmpE00->tag != 2)goto _LL6C6;else{_tmpE01=_tmpE00->f1;_tmpE02=(struct Cyc_Core_Opt**)& _tmpE00->f1;_tmpE03=_tmpE00->f2;}}_LL6C5:
# 4498
*_tmpE02=Cyc_Tcutil_kind_to_bound_opt(_tmpE03);goto _LL6B7;_LL6C6: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpE04=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpDD9;if(_tmpE04->tag != 0)goto _LL6C8;else{_tmpE05=_tmpE04->f1;_tmpE06=*_tmpE05;_tmpE07=_tmpE06.kind;if(_tmpE07 != Cyc_Absyn_MemKind)goto _LL6C8;_tmpE08=_tmpE06.aliasqual;}}_LL6C7:
# 4500
{const char*_tmp156A;void*_tmp1569[2];struct Cyc_String_pa_PrintArg_struct _tmp1568;struct Cyc_Absyn_Kind*_tmp1567;struct Cyc_String_pa_PrintArg_struct _tmp1566;(_tmp1566.tag=0,((_tmp1566.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4501
Cyc_Absynpp_kind2string(((_tmp1567=_cycalloc_atomic(sizeof(*_tmp1567)),((_tmp1567->kind=Cyc_Absyn_MemKind,((_tmp1567->aliasqual=_tmpE08,_tmp1567)))))))),((_tmp1568.tag=0,((_tmp1568.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd)),((_tmp1569[0]=& _tmp1568,((_tmp1569[1]=& _tmp1566,Cyc_Tcutil_terr(loc,((_tmp156A="type variable %s cannot have kind %s",_tag_dyneither(_tmp156A,sizeof(char),37))),_tag_dyneither(_tmp1569,sizeof(void*),2)))))))))))));}
# 4502
goto _LL6B7;_LL6C8:;_LL6C9:
# 4503
 goto _LL6B7;_LL6B7:;}}
# 4507
if(_tmpDD2 != 0  || _tmpDD3 != 0){
# 4508
{void*_tmpE0E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpE10;struct Cyc_List_List*_tmpE11;struct Cyc_List_List**_tmpE12;void*_tmpE13;struct Cyc_Absyn_Tqual _tmpE14;void*_tmpE15;struct Cyc_List_List*_tmpE16;int _tmpE17;struct Cyc_Absyn_VarargInfo*_tmpE18;struct Cyc_List_List*_tmpE19;struct Cyc_List_List*_tmpE1A;_LL6CB: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE0F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE0E;if(_tmpE0F->tag != 9)goto _LL6CD;else{_tmpE10=_tmpE0F->f1;_tmpE11=_tmpE10.tvars;_tmpE12=(struct Cyc_List_List**)&(_tmpE0F->f1).tvars;_tmpE13=_tmpE10.effect;_tmpE14=_tmpE10.ret_tqual;_tmpE15=_tmpE10.ret_typ;_tmpE16=_tmpE10.args;_tmpE17=_tmpE10.c_varargs;_tmpE18=_tmpE10.cyc_varargs;_tmpE19=_tmpE10.rgn_po;_tmpE1A=_tmpE10.attributes;}}_LL6CC:
# 4510
 if(*_tmpE12 == 0){
# 4512
*_tmpE12=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmpDD2);
# 4513
_tmpDD2=0;}
# 4515
goto _LL6CA;_LL6CD:;_LL6CE:
# 4516
 goto _LL6CA;_LL6CA:;}
# 4518
if(_tmpDD2 != 0){
# 4519
const char*_tmp156E;void*_tmp156D[1];struct Cyc_String_pa_PrintArg_struct _tmp156C;(_tmp156C.tag=0,((_tmp156C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpDD2->hd)->name),((_tmp156D[0]=& _tmp156C,Cyc_Tcutil_terr(loc,((_tmp156E="unbound type variable %s",_tag_dyneither(_tmp156E,sizeof(char),25))),_tag_dyneither(_tmp156D,sizeof(void*),1)))))));}
# 4523
if(_tmpDD3 != 0)
# 4524
for(0;_tmpDD3 != 0;_tmpDD3=_tmpDD3->tl){
# 4525
void*e=(void*)_tmpDD3->hd;
# 4526
struct Cyc_Absyn_Kind*_tmpE1E=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind _tmpE1F;enum Cyc_Absyn_KindQual _tmpE20;enum Cyc_Absyn_AliasQual _tmpE21;struct Cyc_Absyn_Kind _tmpE22;enum Cyc_Absyn_KindQual _tmpE23;enum Cyc_Absyn_AliasQual _tmpE24;struct Cyc_Absyn_Kind _tmpE25;enum Cyc_Absyn_KindQual _tmpE26;enum Cyc_Absyn_AliasQual _tmpE27;struct Cyc_Absyn_Kind _tmpE28;enum Cyc_Absyn_KindQual _tmpE29;_LL6D0: _tmpE1F=*_tmpE1E;_tmpE20=_tmpE1F.kind;if(_tmpE20 != Cyc_Absyn_RgnKind)goto _LL6D2;_tmpE21=_tmpE1F.aliasqual;if(_tmpE21 != Cyc_Absyn_Unique)goto _LL6D2;_LL6D1:
# 4528
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
# 4529
const char*_tmp1571;void*_tmp1570;(_tmp1570=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1571="can't unify evar with unique region!",_tag_dyneither(_tmp1571,sizeof(char),37))),_tag_dyneither(_tmp1570,sizeof(void*),0)));}
# 4530
goto _LL6CF;_LL6D2: _tmpE22=*_tmpE1E;_tmpE23=_tmpE22.kind;if(_tmpE23 != Cyc_Absyn_RgnKind)goto _LL6D4;_tmpE24=_tmpE22.aliasqual;if(_tmpE24 != Cyc_Absyn_Aliasable)goto _LL6D4;_LL6D3:
# 4531
 goto _LL6D5;_LL6D4: _tmpE25=*_tmpE1E;_tmpE26=_tmpE25.kind;if(_tmpE26 != Cyc_Absyn_RgnKind)goto _LL6D6;_tmpE27=_tmpE25.aliasqual;if(_tmpE27 != Cyc_Absyn_Top)goto _LL6D6;_LL6D5:
# 4533
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp1574;void*_tmp1573;(_tmp1573=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1574="can't unify evar with heap!",_tag_dyneither(_tmp1574,sizeof(char),28))),_tag_dyneither(_tmp1573,sizeof(void*),0)));}
# 4534
goto _LL6CF;_LL6D6: _tmpE28=*_tmpE1E;_tmpE29=_tmpE28.kind;if(_tmpE29 != Cyc_Absyn_EffKind)goto _LL6D8;_LL6D7:
# 4536
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp1577;void*_tmp1576;(_tmp1576=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1577="can't unify evar with {}!",_tag_dyneither(_tmp1577,sizeof(char),26))),_tag_dyneither(_tmp1576,sizeof(void*),0)));}
# 4537
goto _LL6CF;_LL6D8:;_LL6D9:
# 4539
{const char*_tmp157C;void*_tmp157B[2];struct Cyc_String_pa_PrintArg_struct _tmp157A;struct Cyc_String_pa_PrintArg_struct _tmp1579;(_tmp1579.tag=0,((_tmp1579.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4540
Cyc_Absynpp_typ2string(t)),((_tmp157A.tag=0,((_tmp157A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp157B[0]=& _tmp157A,((_tmp157B[1]=& _tmp1579,Cyc_Tcutil_terr(loc,((_tmp157C="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp157C,sizeof(char),52))),_tag_dyneither(_tmp157B,sizeof(void*),2)))))))))))));}
# 4541
goto _LL6CF;_LL6CF:;}}}
# 4462
;_pop_region(temp);}
# 4552
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
# 4553
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4555
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
# 4556
{void*_tmpE35=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpE37;struct Cyc_List_List*_tmpE38;void*_tmpE39;struct Cyc_Absyn_Tqual _tmpE3A;void*_tmpE3B;struct Cyc_List_List*_tmpE3C;_LL6DB: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE36=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE35;if(_tmpE36->tag != 9)goto _LL6DD;else{_tmpE37=_tmpE36->f1;_tmpE38=_tmpE37.tvars;_tmpE39=_tmpE37.effect;_tmpE3A=_tmpE37.ret_tqual;_tmpE3B=_tmpE37.ret_typ;_tmpE3C=_tmpE37.args;}}_LL6DC:
# 4558
 fd->tvs=_tmpE38;
# 4559
fd->effect=_tmpE39;
# 4560
{struct Cyc_List_List*_tmpE3D=fd->args;for(0;_tmpE3D != 0;(_tmpE3D=_tmpE3D->tl,_tmpE3C=_tmpE3C->tl)){
# 4562
(*((struct _tuple10*)_tmpE3D->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmpE3C))->hd)).f2;
# 4563
(*((struct _tuple10*)_tmpE3D->hd)).f3=(*((struct _tuple10*)_tmpE3C->hd)).f3;}}
# 4565
fd->ret_tqual=_tmpE3A;
# 4566
fd->ret_type=_tmpE3B;
# 4567
(fd->ret_tqual).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpE3B);
# 4568
goto _LL6DA;_LL6DD:;_LL6DE: {
# 4569
const char*_tmp157F;void*_tmp157E;(_tmp157E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp157F="check_fndecl_valid_type: not a FnType",_tag_dyneither(_tmp157F,sizeof(char),38))),_tag_dyneither(_tmp157E,sizeof(void*),0)));}_LL6DA:;}{
# 4572
struct _RegionHandle _tmpE40=_new_region("r");struct _RegionHandle*r=& _tmpE40;_push_region(r);{
# 4573
const char*_tmp1580;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,(
# 4574
(_tmp1580="function declaration has repeated parameter",_tag_dyneither(_tmp1580,sizeof(char),44))));}
# 4577
fd->cached_typ=(void*)t;
# 4572
;_pop_region(r);};}
# 4582
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){
# 4585
struct _RegionHandle _tmpE42=_new_region("r");struct _RegionHandle*r=& _tmpE42;_push_region(r);
# 4586
{struct Cyc_Tcutil_CVTEnv _tmp1581;struct Cyc_Tcutil_CVTEnv _tmpE43=Cyc_Tcutil_check_valid_type(loc,te,(
# 4587
(_tmp1581.r=r,((_tmp1581.kind_env=bound_tvars,((_tmp1581.free_vars=0,((_tmp1581.free_evars=0,((_tmp1581.generalize_evars=0,((_tmp1581.fn_result=0,_tmp1581)))))))))))),expected_kind,t);
# 4589
struct Cyc_List_List*_tmpE44=Cyc_Tcutil_remove_bound_tvars(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpE43.free_vars),bound_tvars);
# 4591
struct Cyc_List_List*_tmpE45=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpE43.free_evars);
# 4592
{struct Cyc_List_List*fs=_tmpE44;for(0;fs != 0;fs=fs->tl){
# 4593
struct _dyneither_ptr*_tmpE46=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
# 4594
const char*_tmp1586;void*_tmp1585[2];struct Cyc_String_pa_PrintArg_struct _tmp1584;struct Cyc_String_pa_PrintArg_struct _tmp1583;(_tmp1583.tag=0,((_tmp1583.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1584.tag=0,((_tmp1584.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE46),((_tmp1585[0]=& _tmp1584,((_tmp1585[1]=& _tmp1583,Cyc_Tcutil_terr(loc,((_tmp1586="unbound type variable %s in type %s",_tag_dyneither(_tmp1586,sizeof(char),36))),_tag_dyneither(_tmp1585,sizeof(void*),2)))))))))))));}}
# 4596
if(!allow_evars  && _tmpE45 != 0)
# 4597
for(0;_tmpE45 != 0;_tmpE45=_tmpE45->tl){
# 4598
void*e=(void*)_tmpE45->hd;
# 4599
struct Cyc_Absyn_Kind*_tmpE4B=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind _tmpE4C;enum Cyc_Absyn_KindQual _tmpE4D;enum Cyc_Absyn_AliasQual _tmpE4E;struct Cyc_Absyn_Kind _tmpE4F;enum Cyc_Absyn_KindQual _tmpE50;enum Cyc_Absyn_AliasQual _tmpE51;struct Cyc_Absyn_Kind _tmpE52;enum Cyc_Absyn_KindQual _tmpE53;enum Cyc_Absyn_AliasQual _tmpE54;struct Cyc_Absyn_Kind _tmpE55;enum Cyc_Absyn_KindQual _tmpE56;_LL6E0: _tmpE4C=*_tmpE4B;_tmpE4D=_tmpE4C.kind;if(_tmpE4D != Cyc_Absyn_RgnKind)goto _LL6E2;_tmpE4E=_tmpE4C.aliasqual;if(_tmpE4E != Cyc_Absyn_Unique)goto _LL6E2;_LL6E1:
# 4601
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
# 4602
const char*_tmp1589;void*_tmp1588;(_tmp1588=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1589="can't unify evar with unique region!",_tag_dyneither(_tmp1589,sizeof(char),37))),_tag_dyneither(_tmp1588,sizeof(void*),0)));}
# 4603
goto _LL6DF;_LL6E2: _tmpE4F=*_tmpE4B;_tmpE50=_tmpE4F.kind;if(_tmpE50 != Cyc_Absyn_RgnKind)goto _LL6E4;_tmpE51=_tmpE4F.aliasqual;if(_tmpE51 != Cyc_Absyn_Aliasable)goto _LL6E4;_LL6E3:
# 4604
 goto _LL6E5;_LL6E4: _tmpE52=*_tmpE4B;_tmpE53=_tmpE52.kind;if(_tmpE53 != Cyc_Absyn_RgnKind)goto _LL6E6;_tmpE54=_tmpE52.aliasqual;if(_tmpE54 != Cyc_Absyn_Top)goto _LL6E6;_LL6E5:
# 4606
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp158C;void*_tmp158B;(_tmp158B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp158C="can't unify evar with heap!",_tag_dyneither(_tmp158C,sizeof(char),28))),_tag_dyneither(_tmp158B,sizeof(void*),0)));}
# 4607
goto _LL6DF;_LL6E6: _tmpE55=*_tmpE4B;_tmpE56=_tmpE55.kind;if(_tmpE56 != Cyc_Absyn_EffKind)goto _LL6E8;_LL6E7:
# 4609
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp158F;void*_tmp158E;(_tmp158E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp158F="can't unify evar with {}!",_tag_dyneither(_tmp158F,sizeof(char),26))),_tag_dyneither(_tmp158E,sizeof(void*),0)));}
# 4610
goto _LL6DF;_LL6E8:;_LL6E9:
# 4612
{const char*_tmp1594;void*_tmp1593[2];struct Cyc_String_pa_PrintArg_struct _tmp1592;struct Cyc_String_pa_PrintArg_struct _tmp1591;(_tmp1591.tag=0,((_tmp1591.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4613
Cyc_Absynpp_typ2string(t)),((_tmp1592.tag=0,((_tmp1592.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp1593[0]=& _tmp1592,((_tmp1593[1]=& _tmp1591,Cyc_Tcutil_terr(loc,((_tmp1594="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp1594,sizeof(char),52))),_tag_dyneither(_tmp1593,sizeof(void*),2)))))))))))));}
# 4614
goto _LL6DF;_LL6DF:;}}
# 4586
;_pop_region(r);}
# 4621
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
# 4622
if(tv->identity == - 1)
# 4623
tv->identity=Cyc_Tcutil_new_tvar_id();}
# 4626
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
# 4627
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4632
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4635
for(0;vs != 0;vs=vs->tl){
# 4636
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
# 4637
if(cmp((void*)vs->hd,(void*)vs2->hd)== 0){
# 4638
const char*_tmp1599;void*_tmp1598[2];struct Cyc_String_pa_PrintArg_struct _tmp1597;struct Cyc_String_pa_PrintArg_struct _tmp1596;(_tmp1596.tag=0,((_tmp1596.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd)),((_tmp1597.tag=0,((_tmp1597.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp1598[0]=& _tmp1597,((_tmp1598[1]=& _tmp1596,Cyc_Tcutil_terr(loc,((_tmp1599="%s: %s",_tag_dyneither(_tmp1599,sizeof(char),7))),_tag_dyneither(_tmp1598,sizeof(void*),2)))))))))))));}}}}
# 4641
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
# 4642
return*s;}
# 4645
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
# 4646
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4649
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
# 4650
const char*_tmp159A;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp159A="duplicate type variable",_tag_dyneither(_tmp159A,sizeof(char),24))));}struct _tuple31{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple32{struct Cyc_List_List*f1;void*f2;};struct _tuple33{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4663 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 4668
struct _RegionHandle _tmpE67=_new_region("temp");struct _RegionHandle*temp=& _tmpE67;_push_region(temp);
# 4672
{struct Cyc_List_List*fields=0;
# 4673
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
# 4674
const char*_tmp159B;if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp159B="",_tag_dyneither(_tmp159B,sizeof(char),1))))!= 0){
# 4675
struct _tuple31*_tmp159E;struct Cyc_List_List*_tmp159D;fields=((_tmp159D=_region_malloc(temp,sizeof(*_tmp159D)),((_tmp159D->hd=((_tmp159E=_region_malloc(temp,sizeof(*_tmp159E)),((_tmp159E->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp159E->f2=0,_tmp159E)))))),((_tmp159D->tl=fields,_tmp159D))))));}}}
# 4677
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 4679
const char*_tmp15A0;const char*_tmp159F;struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp15A0="struct",_tag_dyneither(_tmp15A0,sizeof(char),7)):((_tmp159F="union",_tag_dyneither(_tmp159F,sizeof(char),6)));
# 4682
struct Cyc_List_List*ans=0;
# 4683
for(0;des != 0;des=des->tl){
# 4684
struct _tuple32 _tmpE6C;struct Cyc_List_List*_tmpE6D;void*_tmpE6E;struct _tuple32*_tmpE6B=(struct _tuple32*)des->hd;_tmpE6C=*_tmpE6B;_tmpE6D=_tmpE6C.f1;_tmpE6E=_tmpE6C.f2;
# 4685
if(_tmpE6D == 0){
# 4687
struct Cyc_List_List*_tmpE6F=fields;
# 4688
for(0;_tmpE6F != 0;_tmpE6F=_tmpE6F->tl){
# 4689
if(!(*((struct _tuple31*)_tmpE6F->hd)).f2){
# 4690
(*((struct _tuple31*)_tmpE6F->hd)).f2=1;
# 4691
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp15A6;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp15A5;struct Cyc_List_List*_tmp15A4;(*((struct _tuple32*)des->hd)).f1=(struct Cyc_List_List*)((_tmp15A4=_cycalloc(sizeof(*_tmp15A4)),((_tmp15A4->hd=(void*)((_tmp15A6=_cycalloc(sizeof(*_tmp15A6)),((_tmp15A6[0]=((_tmp15A5.tag=1,((_tmp15A5.f1=((*((struct _tuple31*)_tmpE6F->hd)).f1)->name,_tmp15A5)))),_tmp15A6)))),((_tmp15A4->tl=0,_tmp15A4))))));}
# 4692
{struct _tuple33*_tmp15A9;struct Cyc_List_List*_tmp15A8;ans=((_tmp15A8=_region_malloc(rgn,sizeof(*_tmp15A8)),((_tmp15A8->hd=((_tmp15A9=_region_malloc(rgn,sizeof(*_tmp15A9)),((_tmp15A9->f1=(*((struct _tuple31*)_tmpE6F->hd)).f1,((_tmp15A9->f2=_tmpE6E,_tmp15A9)))))),((_tmp15A8->tl=ans,_tmp15A8))))));}
# 4693
break;}}
# 4695
if(_tmpE6F == 0){
# 4696
const char*_tmp15AD;void*_tmp15AC[1];struct Cyc_String_pa_PrintArg_struct _tmp15AB;(_tmp15AB.tag=0,((_tmp15AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp15AC[0]=& _tmp15AB,Cyc_Tcutil_terr(loc,((_tmp15AD="too many arguments to %s",_tag_dyneither(_tmp15AD,sizeof(char),25))),_tag_dyneither(_tmp15AC,sizeof(void*),1)))))));}}else{
# 4697
if(_tmpE6D->tl != 0){
# 4699
const char*_tmp15B0;void*_tmp15AF;(_tmp15AF=0,Cyc_Tcutil_terr(loc,((_tmp15B0="multiple designators are not yet supported",_tag_dyneither(_tmp15B0,sizeof(char),43))),_tag_dyneither(_tmp15AF,sizeof(void*),0)));}else{
# 4702
void*_tmpE7A=(void*)_tmpE6D->hd;struct _dyneither_ptr*_tmpE7D;_LL6EB: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpE7B=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpE7A;if(_tmpE7B->tag != 0)goto _LL6ED;}_LL6EC:
# 4704
{const char*_tmp15B4;void*_tmp15B3[1];struct Cyc_String_pa_PrintArg_struct _tmp15B2;(_tmp15B2.tag=0,((_tmp15B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp15B3[0]=& _tmp15B2,Cyc_Tcutil_terr(loc,((_tmp15B4="array designator used in argument to %s",_tag_dyneither(_tmp15B4,sizeof(char),40))),_tag_dyneither(_tmp15B3,sizeof(void*),1)))))));}
# 4705
goto _LL6EA;_LL6ED: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpE7C=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpE7A;if(_tmpE7C->tag != 1)goto _LL6EA;else{_tmpE7D=_tmpE7C->f1;}}_LL6EE: {
# 4707
struct Cyc_List_List*_tmpE81=fields;
# 4708
for(0;_tmpE81 != 0;_tmpE81=_tmpE81->tl){
# 4709
if(Cyc_strptrcmp(_tmpE7D,((*((struct _tuple31*)_tmpE81->hd)).f1)->name)== 0){
# 4710
if((*((struct _tuple31*)_tmpE81->hd)).f2){
# 4711
const char*_tmp15B8;void*_tmp15B7[1];struct Cyc_String_pa_PrintArg_struct _tmp15B6;(_tmp15B6.tag=0,((_tmp15B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE7D),((_tmp15B7[0]=& _tmp15B6,Cyc_Tcutil_terr(loc,((_tmp15B8="member %s has already been used as an argument",_tag_dyneither(_tmp15B8,sizeof(char),47))),_tag_dyneither(_tmp15B7,sizeof(void*),1)))))));}
# 4712
(*((struct _tuple31*)_tmpE81->hd)).f2=1;
# 4713
{struct _tuple33*_tmp15BB;struct Cyc_List_List*_tmp15BA;ans=((_tmp15BA=_region_malloc(rgn,sizeof(*_tmp15BA)),((_tmp15BA->hd=((_tmp15BB=_region_malloc(rgn,sizeof(*_tmp15BB)),((_tmp15BB->f1=(*((struct _tuple31*)_tmpE81->hd)).f1,((_tmp15BB->f2=_tmpE6E,_tmp15BB)))))),((_tmp15BA->tl=ans,_tmp15BA))))));}
# 4714
break;}}
# 4716
if(_tmpE81 == 0){
# 4717
const char*_tmp15BF;void*_tmp15BE[1];struct Cyc_String_pa_PrintArg_struct _tmp15BD;(_tmp15BD.tag=0,((_tmp15BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE7D),((_tmp15BE[0]=& _tmp15BD,Cyc_Tcutil_terr(loc,((_tmp15BF="bad field designator %s",_tag_dyneither(_tmp15BF,sizeof(char),24))),_tag_dyneither(_tmp15BE,sizeof(void*),1)))))));}
# 4718
goto _LL6EA;}_LL6EA:;}}}
# 4721
if(aggr_kind == Cyc_Absyn_StructA)
# 4723
for(0;fields != 0;fields=fields->tl){
# 4724
if(!(*((struct _tuple31*)fields->hd)).f2){
# 4725
{const char*_tmp15C2;void*_tmp15C1;(_tmp15C1=0,Cyc_Tcutil_terr(loc,((_tmp15C2="too few arguments to struct",_tag_dyneither(_tmp15C2,sizeof(char),28))),_tag_dyneither(_tmp15C1,sizeof(void*),0)));}
# 4726
break;}}else{
# 4730
int found=0;
# 4731
for(0;fields != 0;fields=fields->tl){
# 4732
if((*((struct _tuple31*)fields->hd)).f2){
# 4733
if(found){const char*_tmp15C5;void*_tmp15C4;(_tmp15C4=0,Cyc_Tcutil_terr(loc,((_tmp15C5="only one member of a union is allowed",_tag_dyneither(_tmp15C5,sizeof(char),38))),_tag_dyneither(_tmp15C4,sizeof(void*),0)));}
# 4734
found=1;}}
# 4737
if(!found){const char*_tmp15C8;void*_tmp15C7;(_tmp15C7=0,Cyc_Tcutil_terr(loc,((_tmp15C8="missing member for union",_tag_dyneither(_tmp15C8,sizeof(char),25))),_tag_dyneither(_tmp15C7,sizeof(void*),0)));}}{
# 4740
struct Cyc_List_List*_tmpE90=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmpE90;};};}
# 4672
;_pop_region(temp);}
# 4743
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
# 4744
void*_tmpE93=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpE95;void*_tmpE96;struct Cyc_Absyn_PtrAtts _tmpE97;union Cyc_Absyn_Constraint*_tmpE98;_LL6F0: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE94=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE93;if(_tmpE94->tag != 5)goto _LL6F2;else{_tmpE95=_tmpE94->f1;_tmpE96=_tmpE95.elt_typ;_tmpE97=_tmpE95.ptr_atts;_tmpE98=_tmpE97.bounds;}}_LL6F1: {
# 4746
void*_tmpE99=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpE98);_LL6F5: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpE9A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpE99;if(_tmpE9A->tag != 0)goto _LL6F7;}_LL6F6:
# 4748
*elt_typ_dest=_tmpE96;
# 4749
return 1;_LL6F7:;_LL6F8:
# 4750
 return 0;_LL6F4:;}_LL6F2:;_LL6F3:
# 4752
 return 0;_LL6EF:;}
# 4756
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
# 4757
void*_tmpE9B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpE9D;void*_tmpE9E;struct Cyc_Absyn_PtrAtts _tmpE9F;union Cyc_Absyn_Constraint*_tmpEA0;_LL6FA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE9C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE9B;if(_tmpE9C->tag != 5)goto _LL6FC;else{_tmpE9D=_tmpE9C->f1;_tmpE9E=_tmpE9D.elt_typ;_tmpE9F=_tmpE9D.ptr_atts;_tmpEA0=_tmpE9F.zero_term;}}_LL6FB:
# 4759
*elt_typ_dest=_tmpE9E;
# 4760
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEA0);_LL6FC:;_LL6FD:
# 4761
 return 0;_LL6F9:;}
# 4767
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 4769
void*_tmpEA1=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpEA3;void*_tmpEA4;struct Cyc_Absyn_PtrAtts _tmpEA5;union Cyc_Absyn_Constraint*_tmpEA6;union Cyc_Absyn_Constraint*_tmpEA7;struct Cyc_Absyn_ArrayInfo _tmpEA9;void*_tmpEAA;struct Cyc_Absyn_Tqual _tmpEAB;struct Cyc_Absyn_Exp*_tmpEAC;union Cyc_Absyn_Constraint*_tmpEAD;_LL6FF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpEA2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEA1;if(_tmpEA2->tag != 5)goto _LL701;else{_tmpEA3=_tmpEA2->f1;_tmpEA4=_tmpEA3.elt_typ;_tmpEA5=_tmpEA3.ptr_atts;_tmpEA6=_tmpEA5.bounds;_tmpEA7=_tmpEA5.zero_term;}}_LL700:
# 4771
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEA7)){
# 4772
*ptr_type=t;
# 4773
*elt_type=_tmpEA4;
# 4774
{void*_tmpEAE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpEA6);_LL706: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpEAF=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpEAE;if(_tmpEAF->tag != 0)goto _LL708;}_LL707:
# 4775
*is_dyneither=1;goto _LL705;_LL708:;_LL709:
# 4776
*is_dyneither=0;goto _LL705;_LL705:;}
# 4778
return 1;}else{
# 4779
return 0;}_LL701: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpEA8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpEA1;if(_tmpEA8->tag != 8)goto _LL703;else{_tmpEA9=_tmpEA8->f1;_tmpEAA=_tmpEA9.elt_type;_tmpEAB=_tmpEA9.tq;_tmpEAC=_tmpEA9.num_elts;_tmpEAD=_tmpEA9.zero_term;}}_LL702:
# 4781
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEAD)){
# 4782
*elt_type=_tmpEAA;
# 4783
*is_dyneither=0;
# 4784
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp15DD;struct Cyc_Absyn_PtrAtts _tmp15DC;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp15DB;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp15DA;struct Cyc_Absyn_PtrInfo _tmp15D9;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp15D8;*ptr_type=(void*)(
# 4785
(_tmp15D8=_cycalloc(sizeof(*_tmp15D8)),((_tmp15D8[0]=((_tmp15DD.tag=5,((_tmp15DD.f1=((_tmp15D9.elt_typ=_tmpEAA,((_tmp15D9.elt_tq=_tmpEAB,((_tmp15D9.ptr_atts=(
# 4786
(_tmp15DC.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((_tmp15DC.nullable=Cyc_Absyn_false_conref,((_tmp15DC.bounds=
# 4787
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp15DA=_cycalloc(sizeof(*_tmp15DA)),((_tmp15DA[0]=((_tmp15DB.tag=1,((_tmp15DB.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpEAC),_tmp15DB)))),_tmp15DA))))),((_tmp15DC.zero_term=_tmpEAD,((_tmp15DC.ptrloc=0,_tmp15DC)))))))))),_tmp15D9)))))),_tmp15DD)))),_tmp15D8))));}
# 4789
return 1;}else{
# 4790
return 0;}_LL703:;_LL704:
# 4791
 return 0;_LL6FE:;}
# 4798
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 4800
void*_tmpEB6=e1->r;struct Cyc_Absyn_Exp*_tmpEB9;struct Cyc_Absyn_Exp*_tmpEBB;struct Cyc_Absyn_Exp*_tmpEBD;struct Cyc_Absyn_Exp*_tmpEBF;struct Cyc_Absyn_Exp*_tmpEC1;struct Cyc_Absyn_Exp*_tmpEC3;_LL70B: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpEB7=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpEB6;if(_tmpEB7->tag != 13)goto _LL70D;}_LL70C: {
# 4802
const char*_tmp15E1;void*_tmp15E0[1];struct Cyc_String_pa_PrintArg_struct _tmp15DF;(_tmp15DF.tag=0,((_tmp15DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp15E0[0]=& _tmp15DF,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15E1="we have a cast in a lhs:  %s",_tag_dyneither(_tmp15E1,sizeof(char),29))),_tag_dyneither(_tmp15E0,sizeof(void*),1)))))));}_LL70D: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpEB8=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpEB6;if(_tmpEB8->tag != 19)goto _LL70F;else{_tmpEB9=_tmpEB8->f1;}}_LL70E:
# 4803
 _tmpEBB=_tmpEB9;goto _LL710;_LL70F: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpEBA=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpEB6;if(_tmpEBA->tag != 22)goto _LL711;else{_tmpEBB=_tmpEBA->f1;}}_LL710:
# 4805
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpEBB->topt),ptr_type,is_dyneither,elt_type);_LL711: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpEBC=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpEB6;if(_tmpEBC->tag != 21)goto _LL713;else{_tmpEBD=_tmpEBC->f1;}}_LL712:
# 4806
 _tmpEBF=_tmpEBD;goto _LL714;_LL713: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpEBE=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpEB6;if(_tmpEBE->tag != 20)goto _LL715;else{_tmpEBF=_tmpEBE->f1;}}_LL714:
# 4809
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpEBF->topt),ptr_type,is_dyneither,elt_type)){
# 4810
const char*_tmp15E5;void*_tmp15E4[1];struct Cyc_String_pa_PrintArg_struct _tmp15E3;(_tmp15E3.tag=0,((_tmp15E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4811
Cyc_Absynpp_exp2string(e1)),((_tmp15E4[0]=& _tmp15E3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15E5="found zero pointer aggregate member assignment: %s",_tag_dyneither(_tmp15E5,sizeof(char),51))),_tag_dyneither(_tmp15E4,sizeof(void*),1)))))));}
# 4812
return 0;_LL715: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpEC0=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpEB6;if(_tmpEC0->tag != 12)goto _LL717;else{_tmpEC1=_tmpEC0->f1;}}_LL716:
# 4813
 _tmpEC3=_tmpEC1;goto _LL718;_LL717: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpEC2=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpEB6;if(_tmpEC2->tag != 11)goto _LL719;else{_tmpEC3=_tmpEC2->f1;}}_LL718:
# 4815
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpEC3->topt),ptr_type,is_dyneither,elt_type)){
# 4816
const char*_tmp15E9;void*_tmp15E8[1];struct Cyc_String_pa_PrintArg_struct _tmp15E7;(_tmp15E7.tag=0,((_tmp15E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4817
Cyc_Absynpp_exp2string(e1)),((_tmp15E8[0]=& _tmp15E7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15E9="found zero pointer instantiate/noinstantiate: %s",_tag_dyneither(_tmp15E9,sizeof(char),49))),_tag_dyneither(_tmp15E8,sizeof(void*),1)))))));}
# 4818
return 0;_LL719: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpEC4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpEB6;if(_tmpEC4->tag != 1)goto _LL71B;}_LL71A:
# 4819
 return 0;_LL71B:;_LL71C: {
# 4821
const char*_tmp15ED;void*_tmp15EC[1];struct Cyc_String_pa_PrintArg_struct _tmp15EB;(_tmp15EB.tag=0,((_tmp15EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp15EC[0]=& _tmp15EB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15ED="found bad lhs in is_zero_ptr_deref: %s",_tag_dyneither(_tmp15ED,sizeof(char),39))),_tag_dyneither(_tmp15EC,sizeof(void*),1)))))));}_LL70A:;}
# 4825
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
# 4826
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
# 4827
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 4836
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 4837
void*_tmpED1=Cyc_Tcutil_compress(r);struct Cyc_Absyn_Tvar*_tmpED5;_LL71E: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpED2=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmpED1;if(_tmpED2->tag != 22)goto _LL720;}_LL71F:
# 4838
 return !must_be_unique;_LL720: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpED3=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmpED1;if(_tmpED3->tag != 21)goto _LL722;}_LL721:
# 4839
 return 1;_LL722: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpED4=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpED1;if(_tmpED4->tag != 2)goto _LL724;else{_tmpED5=_tmpED4->f1;}}_LL723: {
# 4841
struct Cyc_Absyn_Kind _tmpED7;enum Cyc_Absyn_KindQual _tmpED8;enum Cyc_Absyn_AliasQual _tmpED9;struct Cyc_Absyn_Kind*_tmpED6=Cyc_Tcutil_tvar_kind(_tmpED5,& Cyc_Tcutil_rk);_tmpED7=*_tmpED6;_tmpED8=_tmpED7.kind;_tmpED9=_tmpED7.aliasqual;
# 4842
if(_tmpED8 == Cyc_Absyn_RgnKind  && (_tmpED9 == Cyc_Absyn_Unique  || _tmpED9 == Cyc_Absyn_Top  && !must_be_unique)){
# 4843
void*_tmpEDA=Cyc_Absyn_compress_kb(_tmpED5->kind);struct Cyc_Core_Opt*_tmpEDC;struct Cyc_Core_Opt**_tmpEDD;struct Cyc_Absyn_Kind*_tmpEDE;struct Cyc_Absyn_Kind _tmpEDF;enum Cyc_Absyn_KindQual _tmpEE0;enum Cyc_Absyn_AliasQual _tmpEE1;_LL727: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpEDB=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpEDA;if(_tmpEDB->tag != 2)goto _LL729;else{_tmpEDC=_tmpEDB->f1;_tmpEDD=(struct Cyc_Core_Opt**)& _tmpEDB->f1;_tmpEDE=_tmpEDB->f2;_tmpEDF=*_tmpEDE;_tmpEE0=_tmpEDF.kind;if(_tmpEE0 != Cyc_Absyn_RgnKind)goto _LL729;_tmpEE1=_tmpEDF.aliasqual;if(_tmpEE1 != Cyc_Absyn_Top)goto _LL729;}}_LL728:
# 4845
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp15F3;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp15F2;struct Cyc_Core_Opt*_tmp15F1;*_tmpEDD=((_tmp15F1=_cycalloc(sizeof(*_tmp15F1)),((_tmp15F1->v=(void*)((_tmp15F3=_cycalloc(sizeof(*_tmp15F3)),((_tmp15F3[0]=((_tmp15F2.tag=2,((_tmp15F2.f1=0,((_tmp15F2.f2=& Cyc_Tcutil_rk,_tmp15F2)))))),_tmp15F3)))),_tmp15F1))));}
# 4846
return 0;_LL729:;_LL72A:
# 4847
 return 1;_LL726:;}
# 4850
return 0;}_LL724:;_LL725:
# 4851
 return 0;_LL71D:;}
# 4857
static int Cyc_Tcutil_is_noalias_pointer_aux(void*t,int must_be_unique){
# 4858
void*_tmpEE5=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpEE7;struct Cyc_Absyn_PtrAtts _tmpEE8;void*_tmpEE9;struct Cyc_Absyn_Tvar*_tmpEEB;_LL72C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpEE6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEE5;if(_tmpEE6->tag != 5)goto _LL72E;else{_tmpEE7=_tmpEE6->f1;_tmpEE8=_tmpEE7.ptr_atts;_tmpEE9=_tmpEE8.rgn;}}_LL72D:
# 4860
 return Cyc_Tcutil_is_noalias_region(_tmpEE9,must_be_unique);_LL72E: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpEEA=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpEE5;if(_tmpEEA->tag != 2)goto _LL730;else{_tmpEEB=_tmpEEA->f1;}}_LL72F: {
# 4862
struct Cyc_Absyn_Kind _tmpEED;enum Cyc_Absyn_KindQual _tmpEEE;enum Cyc_Absyn_AliasQual _tmpEEF;struct Cyc_Absyn_Kind*_tmpEEC=Cyc_Tcutil_tvar_kind(_tmpEEB,& Cyc_Tcutil_bk);_tmpEED=*_tmpEEC;_tmpEEE=_tmpEED.kind;_tmpEEF=_tmpEED.aliasqual;
# 4863
switch(_tmpEEE){case Cyc_Absyn_BoxKind: _LL732:
# 4864
 goto _LL733;case Cyc_Absyn_AnyKind: _LL733: goto _LL734;case Cyc_Absyn_MemKind: _LL734:
# 4865
 if(_tmpEEF == Cyc_Absyn_Unique  || _tmpEEF == Cyc_Absyn_Top){
# 4866
void*_tmpEF0=Cyc_Absyn_compress_kb(_tmpEEB->kind);struct Cyc_Core_Opt*_tmpEF2;struct Cyc_Core_Opt**_tmpEF3;struct Cyc_Absyn_Kind*_tmpEF4;struct Cyc_Absyn_Kind _tmpEF5;enum Cyc_Absyn_KindQual _tmpEF6;enum Cyc_Absyn_AliasQual _tmpEF7;_LL737: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpEF1=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpEF0;if(_tmpEF1->tag != 2)goto _LL739;else{_tmpEF2=_tmpEF1->f1;_tmpEF3=(struct Cyc_Core_Opt**)& _tmpEF1->f1;_tmpEF4=_tmpEF1->f2;_tmpEF5=*_tmpEF4;_tmpEF6=_tmpEF5.kind;_tmpEF7=_tmpEF5.aliasqual;if(_tmpEF7 != Cyc_Absyn_Top)goto _LL739;}}_LL738:
# 4868
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp15FD;struct Cyc_Absyn_Kind*_tmp15FC;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp15FB;struct Cyc_Core_Opt*_tmp15FA;*_tmpEF3=((_tmp15FA=_cycalloc(sizeof(*_tmp15FA)),((_tmp15FA->v=(void*)((_tmp15FD=_cycalloc(sizeof(*_tmp15FD)),((_tmp15FD[0]=((_tmp15FB.tag=2,((_tmp15FB.f1=0,((_tmp15FB.f2=((_tmp15FC=_cycalloc_atomic(sizeof(*_tmp15FC)),((_tmp15FC->kind=_tmpEF6,((_tmp15FC->aliasqual=Cyc_Absyn_Aliasable,_tmp15FC)))))),_tmp15FB)))))),_tmp15FD)))),_tmp15FA))));}
# 4869
return 0;_LL739:;_LL73A:
# 4872
 return 1;_LL736:;}
# 4875
return 0;default: _LL735:
# 4876
 return 0;}}_LL730:;_LL731:
# 4878
 return 0;_LL72B:;}
# 4881
int Cyc_Tcutil_is_noalias_pointer(void*t){return Cyc_Tcutil_is_noalias_pointer_aux(t,0);}
# 4883
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
# 4884
void*_tmpEFC=Cyc_Tcutil_compress(t);
# 4885
if(Cyc_Tcutil_is_noalias_pointer(_tmpEFC))return 1;{
# 4886
void*_tmpEFD=_tmpEFC;struct Cyc_List_List*_tmpEFF;struct Cyc_Absyn_AggrInfo _tmpF01;union Cyc_Absyn_AggrInfoU _tmpF02;struct Cyc_Absyn_Aggrdecl**_tmpF03;struct Cyc_List_List*_tmpF04;struct Cyc_List_List*_tmpF06;struct Cyc_Absyn_AggrInfo _tmpF08;union Cyc_Absyn_AggrInfoU _tmpF09;struct _tuple4 _tmpF0A;struct Cyc_Absyn_DatatypeInfo _tmpF0C;union Cyc_Absyn_DatatypeInfoU _tmpF0D;struct Cyc_List_List*_tmpF0E;struct Cyc_Absyn_DatatypeFieldInfo _tmpF10;union Cyc_Absyn_DatatypeFieldInfoU _tmpF11;struct Cyc_List_List*_tmpF12;_LL73D: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpEFE=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpEFD;if(_tmpEFE->tag != 10)goto _LL73F;else{_tmpEFF=_tmpEFE->f1;}}_LL73E:
# 4888
 while(_tmpEFF != 0){
# 4889
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpEFF->hd)).f2))return 1;
# 4890
_tmpEFF=_tmpEFF->tl;}
# 4892
return 0;_LL73F: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF00=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpEFD;if(_tmpF00->tag != 11)goto _LL741;else{_tmpF01=_tmpF00->f1;_tmpF02=_tmpF01.aggr_info;if((_tmpF02.KnownAggr).tag != 2)goto _LL741;_tmpF03=(struct Cyc_Absyn_Aggrdecl**)(_tmpF02.KnownAggr).val;_tmpF04=_tmpF01.targs;}}_LL740:
# 4894
 if((*_tmpF03)->impl == 0)return 0;else{
# 4896
struct Cyc_List_List*_tmpF13=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpF03)->tvs,_tmpF04);
# 4897
struct Cyc_List_List*_tmpF14=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpF03)->impl))->fields;
# 4898
void*t;
# 4899
while(_tmpF14 != 0){
# 4900
t=_tmpF13 == 0?((struct Cyc_Absyn_Aggrfield*)_tmpF14->hd)->type: Cyc_Tcutil_rsubstitute(rgn,_tmpF13,((struct Cyc_Absyn_Aggrfield*)_tmpF14->hd)->type);
# 4901
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
# 4902
_tmpF14=_tmpF14->tl;}
# 4904
return 0;}_LL741: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpF05=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpEFD;if(_tmpF05->tag != 12)goto _LL743;else{_tmpF06=_tmpF05->f2;}}_LL742:
# 4907
 while(_tmpF06 != 0){
# 4908
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpF06->hd)->type))return 1;
# 4909
_tmpF06=_tmpF06->tl;}
# 4911
return 0;_LL743: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF07=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpEFD;if(_tmpF07->tag != 11)goto _LL745;else{_tmpF08=_tmpF07->f1;_tmpF09=_tmpF08.aggr_info;if((_tmpF09.UnknownAggr).tag != 1)goto _LL745;_tmpF0A=(struct _tuple4)(_tmpF09.UnknownAggr).val;}}_LL744: {
# 4913
const char*_tmp1600;void*_tmp15FF;(_tmp15FF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1600="got unknown aggr in is_noalias_aggr",_tag_dyneither(_tmp1600,sizeof(char),36))),_tag_dyneither(_tmp15FF,sizeof(void*),0)));}_LL745: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpF0B=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpEFD;if(_tmpF0B->tag != 3)goto _LL747;else{_tmpF0C=_tmpF0B->f1;_tmpF0D=_tmpF0C.datatype_info;_tmpF0E=_tmpF0C.targs;}}_LL746: {
# 4915
union Cyc_Absyn_DatatypeInfoU _tmpF17=_tmpF0D;struct Cyc_Absyn_UnknownDatatypeInfo _tmpF18;struct _tuple2*_tmpF19;int _tmpF1A;struct Cyc_Absyn_Datatypedecl**_tmpF1B;struct Cyc_Absyn_Datatypedecl*_tmpF1C;struct Cyc_Absyn_Datatypedecl _tmpF1D;struct Cyc_List_List*_tmpF1E;struct Cyc_Core_Opt*_tmpF1F;_LL74C: if((_tmpF17.UnknownDatatype).tag != 1)goto _LL74E;_tmpF18=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpF17.UnknownDatatype).val;_tmpF19=_tmpF18.name;_tmpF1A=_tmpF18.is_extensible;_LL74D: {
# 4917
const char*_tmp1603;void*_tmp1602;(_tmp1602=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1603="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp1603,sizeof(char),40))),_tag_dyneither(_tmp1602,sizeof(void*),0)));}_LL74E: if((_tmpF17.KnownDatatype).tag != 2)goto _LL74B;_tmpF1B=(struct Cyc_Absyn_Datatypedecl**)(_tmpF17.KnownDatatype).val;_tmpF1C=*_tmpF1B;_tmpF1D=*_tmpF1C;_tmpF1E=_tmpF1D.tvs;_tmpF1F=_tmpF1D.fields;_LL74F:
# 4920
 return 0;_LL74B:;}_LL747: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpF0F=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpEFD;if(_tmpF0F->tag != 4)goto _LL749;else{_tmpF10=_tmpF0F->f1;_tmpF11=_tmpF10.field_info;_tmpF12=_tmpF10.targs;}}_LL748: {
# 4923
union Cyc_Absyn_DatatypeFieldInfoU _tmpF22=_tmpF11;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpF23;struct _tuple3 _tmpF24;struct Cyc_Absyn_Datatypedecl*_tmpF25;struct Cyc_Absyn_Datatypefield*_tmpF26;_LL751: if((_tmpF22.UnknownDatatypefield).tag != 1)goto _LL753;_tmpF23=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpF22.UnknownDatatypefield).val;_LL752: {
# 4925
const char*_tmp1606;void*_tmp1605;(_tmp1605=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1606="got unknown datatype field in is_noalias_aggr",_tag_dyneither(_tmp1606,sizeof(char),46))),_tag_dyneither(_tmp1605,sizeof(void*),0)));}_LL753: if((_tmpF22.KnownDatatypefield).tag != 2)goto _LL750;_tmpF24=(struct _tuple3)(_tmpF22.KnownDatatypefield).val;_tmpF25=_tmpF24.f1;_tmpF26=_tmpF24.f2;_LL754: {
# 4927
struct Cyc_List_List*_tmpF29=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpF25->tvs,_tmpF12);
# 4928
struct Cyc_List_List*_tmpF2A=_tmpF26->typs;
# 4929
while(_tmpF2A != 0){
# 4930
_tmpEFC=_tmpF29 == 0?(*((struct _tuple12*)_tmpF2A->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpF29,(*((struct _tuple12*)_tmpF2A->hd)).f2);
# 4931
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpEFC))return 1;
# 4932
_tmpF2A=_tmpF2A->tl;}
# 4934
return 0;}_LL750:;}_LL749:;_LL74A:
# 4936
 return 0;_LL73C:;};}
# 4940
static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,struct _dyneither_ptr*f){
# 4941
void*_tmpF2B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmpF2D;union Cyc_Absyn_AggrInfoU _tmpF2E;struct Cyc_Absyn_Aggrdecl**_tmpF2F;struct Cyc_Absyn_Aggrdecl*_tmpF30;struct Cyc_List_List*_tmpF31;struct Cyc_List_List*_tmpF33;_LL756: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF2C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpF2B;if(_tmpF2C->tag != 11)goto _LL758;else{_tmpF2D=_tmpF2C->f1;_tmpF2E=_tmpF2D.aggr_info;if((_tmpF2E.KnownAggr).tag != 2)goto _LL758;_tmpF2F=(struct Cyc_Absyn_Aggrdecl**)(_tmpF2E.KnownAggr).val;_tmpF30=*_tmpF2F;_tmpF31=_tmpF2D.targs;}}_LL757:
# 4943
 _tmpF33=_tmpF30->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF30->impl))->fields;goto _LL759;_LL758: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpF32=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF2B;if(_tmpF32->tag != 12)goto _LL75A;else{_tmpF33=_tmpF32->f2;}}_LL759: {
# 4945
struct Cyc_Absyn_Aggrfield*_tmpF34=Cyc_Absyn_lookup_field(_tmpF33,f);
# 4946
{struct Cyc_Absyn_Aggrfield*_tmpF35=_tmpF34;struct Cyc_Absyn_Aggrfield _tmpF36;void*_tmpF37;_LL75D: if(_tmpF35 != 0)goto _LL75F;_LL75E: {
# 4947
const char*_tmp1609;void*_tmp1608;(_tmp1608=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1609="is_noalias_field: missing field",_tag_dyneither(_tmp1609,sizeof(char),32))),_tag_dyneither(_tmp1608,sizeof(void*),0)));}_LL75F: if(_tmpF35 == 0)goto _LL75C;_tmpF36=*_tmpF35;_tmpF37=_tmpF36.type;_LL760:
# 4949
 return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpF37);_LL75C:;}
# 4951
return 0;}_LL75A:;_LL75B: {
# 4953
const char*_tmp160D;void*_tmp160C[1];struct Cyc_String_pa_PrintArg_struct _tmp160B;(_tmp160B.tag=0,((_tmp160B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4954
Cyc_Absynpp_typ2string(t)),((_tmp160C[0]=& _tmp160B,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp160D="is_noalias_field: invalid type |%s|",_tag_dyneither(_tmp160D,sizeof(char),36))),_tag_dyneither(_tmp160C,sizeof(void*),1)))))));}_LL755:;}
# 4961
static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
# 4962
void*_tmpF3D=e->r;void*_tmpF3F;struct Cyc_Absyn_Exp*_tmpF42;struct Cyc_Absyn_Exp*_tmpF44;struct Cyc_Absyn_Exp*_tmpF46;struct _dyneither_ptr*_tmpF47;struct Cyc_Absyn_Exp*_tmpF49;struct Cyc_Absyn_Exp*_tmpF4A;struct Cyc_Absyn_Exp*_tmpF4C;struct Cyc_Absyn_Exp*_tmpF4D;struct Cyc_Absyn_Exp*_tmpF4F;struct Cyc_Absyn_Exp*_tmpF51;struct Cyc_Absyn_Stmt*_tmpF53;_LL762: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpF3E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF3D;if(_tmpF3E->tag != 1)goto _LL764;else{_tmpF3F=(void*)_tmpF3E->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpF40=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpF3F;if(_tmpF40->tag != 1)goto _LL764;};}}_LL763:
# 4963
 return 0;_LL764: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpF41=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF3D;if(_tmpF41->tag != 21)goto _LL766;else{_tmpF42=_tmpF41->f1;}}_LL765:
# 4964
 _tmpF44=_tmpF42;goto _LL767;_LL766: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpF43=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpF3D;if(_tmpF43->tag != 19)goto _LL768;else{_tmpF44=_tmpF43->f1;}}_LL767:
# 4966
 return Cyc_Tcutil_is_noalias_pointer_aux((void*)_check_null(_tmpF44->topt),1) && 
# 4967
Cyc_Tcutil_is_noalias_path_aux(r,_tmpF44);_LL768: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpF45=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF3D;if(_tmpF45->tag != 20)goto _LL76A;else{_tmpF46=_tmpF45->f1;_tmpF47=_tmpF45->f2;}}_LL769:
# 4968
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF46);_LL76A: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpF48=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF3D;if(_tmpF48->tag != 22)goto _LL76C;else{_tmpF49=_tmpF48->f1;_tmpF4A=_tmpF48->f2;}}_LL76B: {
# 4970
void*_tmpF54=Cyc_Tcutil_compress((void*)_check_null(_tmpF49->topt));_LL777: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpF55=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpF54;if(_tmpF55->tag != 10)goto _LL779;}_LL778:
# 4971
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF49);_LL779:;_LL77A:
# 4972
 return 0;_LL776:;}_LL76C: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpF4B=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF3D;if(_tmpF4B->tag != 5)goto _LL76E;else{_tmpF4C=_tmpF4B->f2;_tmpF4D=_tmpF4B->f3;}}_LL76D:
# 4975
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF4C) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpF4D);_LL76E: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpF4E=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpF3D;if(_tmpF4E->tag != 8)goto _LL770;else{_tmpF4F=_tmpF4E->f2;}}_LL76F:
# 4976
 _tmpF51=_tmpF4F;goto _LL771;_LL770: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpF50=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF3D;if(_tmpF50->tag != 13)goto _LL772;else{_tmpF51=_tmpF50->f2;}}_LL771:
# 4977
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF51);_LL772: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpF52=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpF3D;if(_tmpF52->tag != 35)goto _LL774;else{_tmpF53=_tmpF52->f1;}}_LL773:
# 4979
 while(1){
# 4980
void*_tmpF56=_tmpF53->r;struct Cyc_Absyn_Stmt*_tmpF58;struct Cyc_Absyn_Stmt*_tmpF59;struct Cyc_Absyn_Decl*_tmpF5B;struct Cyc_Absyn_Stmt*_tmpF5C;struct Cyc_Absyn_Exp*_tmpF5E;_LL77C: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpF57=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpF56;if(_tmpF57->tag != 2)goto _LL77E;else{_tmpF58=_tmpF57->f1;_tmpF59=_tmpF57->f2;}}_LL77D:
# 4981
 _tmpF53=_tmpF59;goto _LL77B;_LL77E: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpF5A=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpF56;if(_tmpF5A->tag != 12)goto _LL780;else{_tmpF5B=_tmpF5A->f1;_tmpF5C=_tmpF5A->f2;}}_LL77F:
# 4982
 _tmpF53=_tmpF5C;goto _LL77B;_LL780: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpF5D=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpF56;if(_tmpF5D->tag != 1)goto _LL782;else{_tmpF5E=_tmpF5D->f1;}}_LL781:
# 4983
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF5E);_LL782:;_LL783: {
# 4984
const char*_tmp1610;void*_tmp160F;(_tmp160F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1610="is_noalias_stmt_exp: ill-formed StmtExp",_tag_dyneither(_tmp1610,sizeof(char),40))),_tag_dyneither(_tmp160F,sizeof(void*),0)));}_LL77B:;}_LL774:;_LL775:
# 4987
 return 1;_LL761:;}
# 4990
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
# 4991
return Cyc_Tcutil_is_noalias_path_aux(r,e);}
# 5007 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5009
struct _tuple18 _tmp1611;struct _tuple18 bogus_ans=(_tmp1611.f1=0,((_tmp1611.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1611)));
# 5010
void*_tmpF61=e->r;struct _tuple2*_tmpF63;void*_tmpF64;struct Cyc_Absyn_Exp*_tmpF66;struct _dyneither_ptr*_tmpF67;int _tmpF68;struct Cyc_Absyn_Exp*_tmpF6A;struct _dyneither_ptr*_tmpF6B;int _tmpF6C;struct Cyc_Absyn_Exp*_tmpF6E;struct Cyc_Absyn_Exp*_tmpF70;struct Cyc_Absyn_Exp*_tmpF71;_LL785: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpF62=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF61;if(_tmpF62->tag != 1)goto _LL787;else{_tmpF63=_tmpF62->f1;_tmpF64=(void*)_tmpF62->f2;}}_LL786: {
# 5013
void*_tmpF72=_tmpF64;struct Cyc_Absyn_Vardecl*_tmpF76;struct Cyc_Absyn_Vardecl*_tmpF78;struct Cyc_Absyn_Vardecl*_tmpF7A;struct Cyc_Absyn_Vardecl*_tmpF7C;_LL792: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpF73=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpF72;if(_tmpF73->tag != 0)goto _LL794;}_LL793:
# 5014
 goto _LL795;_LL794: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpF74=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpF72;if(_tmpF74->tag != 2)goto _LL796;}_LL795:
# 5015
 return bogus_ans;_LL796: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpF75=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpF72;if(_tmpF75->tag != 1)goto _LL798;else{_tmpF76=_tmpF75->f1;}}_LL797: {
# 5017
void*_tmpF7D=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL79F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpF7E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF7D;if(_tmpF7E->tag != 8)goto _LL7A1;}_LL7A0: {
# 5019
struct _tuple18 _tmp1612;return(_tmp1612.f1=1,((_tmp1612.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1612)));}_LL7A1:;_LL7A2: {
# 5020
struct _tuple18 _tmp1613;return(_tmp1613.f1=(_tmpF76->tq).real_const,((_tmp1613.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1613)));}_LL79E:;}_LL798: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpF77=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpF72;if(_tmpF77->tag != 4)goto _LL79A;else{_tmpF78=_tmpF77->f1;}}_LL799: {
# 5023
void*_tmpF81=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL7A4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpF82=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF81;if(_tmpF82->tag != 8)goto _LL7A6;}_LL7A5: {
# 5024
struct _tuple18 _tmp1614;return(_tmp1614.f1=1,((_tmp1614.f2=(void*)_check_null(_tmpF78->rgn),_tmp1614)));}_LL7A6:;_LL7A7:
# 5026
 _tmpF78->escapes=1;{
# 5027
struct _tuple18 _tmp1615;return(_tmp1615.f1=(_tmpF78->tq).real_const,((_tmp1615.f2=(void*)_check_null(_tmpF78->rgn),_tmp1615)));};_LL7A3:;}_LL79A: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpF79=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpF72;if(_tmpF79->tag != 5)goto _LL79C;else{_tmpF7A=_tmpF79->f1;}}_LL79B:
# 5029
 _tmpF7C=_tmpF7A;goto _LL79D;_LL79C: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpF7B=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpF72;if(_tmpF7B->tag != 3)goto _LL791;else{_tmpF7C=_tmpF7B->f1;}}_LL79D:
# 5031
 _tmpF7C->escapes=1;{
# 5032
struct _tuple18 _tmp1616;return(_tmp1616.f1=(_tmpF7C->tq).real_const,((_tmp1616.f2=(void*)_check_null(_tmpF7C->rgn),_tmp1616)));};_LL791:;}_LL787: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpF65=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF61;if(_tmpF65->tag != 20)goto _LL789;else{_tmpF66=_tmpF65->f1;_tmpF67=_tmpF65->f2;_tmpF68=_tmpF65->f3;}}_LL788:
# 5036
 if(_tmpF68)return bogus_ans;{
# 5039
void*_tmpF86=Cyc_Tcutil_compress((void*)_check_null(_tmpF66->topt));struct Cyc_List_List*_tmpF88;struct Cyc_Absyn_AggrInfo _tmpF8A;union Cyc_Absyn_AggrInfoU _tmpF8B;struct Cyc_Absyn_Aggrdecl**_tmpF8C;struct Cyc_Absyn_Aggrdecl*_tmpF8D;_LL7A9: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpF87=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF86;if(_tmpF87->tag != 12)goto _LL7AB;else{_tmpF88=_tmpF87->f2;}}_LL7AA: {
# 5041
struct Cyc_Absyn_Aggrfield*_tmpF8E=Cyc_Absyn_lookup_field(_tmpF88,_tmpF67);
# 5042
if(_tmpF8E != 0  && _tmpF8E->width == 0){
# 5043
struct _tuple18 _tmp1617;return(_tmp1617.f1=(_tmpF8E->tq).real_const,((_tmp1617.f2=(Cyc_Tcutil_addressof_props(te,_tmpF66)).f2,_tmp1617)));}
# 5044
return bogus_ans;}_LL7AB: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF89=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpF86;if(_tmpF89->tag != 11)goto _LL7AD;else{_tmpF8A=_tmpF89->f1;_tmpF8B=_tmpF8A.aggr_info;if((_tmpF8B.KnownAggr).tag != 2)goto _LL7AD;_tmpF8C=(struct Cyc_Absyn_Aggrdecl**)(_tmpF8B.KnownAggr).val;_tmpF8D=*_tmpF8C;}}_LL7AC: {
# 5046
struct Cyc_Absyn_Aggrfield*_tmpF90=Cyc_Absyn_lookup_decl_field(_tmpF8D,_tmpF67);
# 5047
if(_tmpF90 != 0  && _tmpF90->width == 0){
# 5048
struct _tuple18 _tmp1618;return(_tmp1618.f1=(_tmpF90->tq).real_const,((_tmp1618.f2=(Cyc_Tcutil_addressof_props(te,_tmpF66)).f2,_tmp1618)));}
# 5049
return bogus_ans;}_LL7AD:;_LL7AE:
# 5050
 return bogus_ans;_LL7A8:;};_LL789: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpF69=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF61;if(_tmpF69->tag != 21)goto _LL78B;else{_tmpF6A=_tmpF69->f1;_tmpF6B=_tmpF69->f2;_tmpF6C=_tmpF69->f3;}}_LL78A:
# 5054
 if(_tmpF6C)return bogus_ans;{
# 5058
void*_tmpF92=Cyc_Tcutil_compress((void*)_check_null(_tmpF6A->topt));struct Cyc_Absyn_PtrInfo _tmpF94;void*_tmpF95;struct Cyc_Absyn_PtrAtts _tmpF96;void*_tmpF97;_LL7B0: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF93=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF92;if(_tmpF93->tag != 5)goto _LL7B2;else{_tmpF94=_tmpF93->f1;_tmpF95=_tmpF94.elt_typ;_tmpF96=_tmpF94.ptr_atts;_tmpF97=_tmpF96.rgn;}}_LL7B1: {
# 5060
struct Cyc_Absyn_Aggrfield*finfo;
# 5061
{void*_tmpF98=Cyc_Tcutil_compress(_tmpF95);struct Cyc_List_List*_tmpF9A;struct Cyc_Absyn_AggrInfo _tmpF9C;union Cyc_Absyn_AggrInfoU _tmpF9D;struct Cyc_Absyn_Aggrdecl**_tmpF9E;struct Cyc_Absyn_Aggrdecl*_tmpF9F;_LL7B5: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpF99=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF98;if(_tmpF99->tag != 12)goto _LL7B7;else{_tmpF9A=_tmpF99->f2;}}_LL7B6:
# 5063
 finfo=Cyc_Absyn_lookup_field(_tmpF9A,_tmpF6B);goto _LL7B4;_LL7B7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF9B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpF98;if(_tmpF9B->tag != 11)goto _LL7B9;else{_tmpF9C=_tmpF9B->f1;_tmpF9D=_tmpF9C.aggr_info;if((_tmpF9D.KnownAggr).tag != 2)goto _LL7B9;_tmpF9E=(struct Cyc_Absyn_Aggrdecl**)(_tmpF9D.KnownAggr).val;_tmpF9F=*_tmpF9E;}}_LL7B8:
# 5065
 finfo=Cyc_Absyn_lookup_decl_field(_tmpF9F,_tmpF6B);goto _LL7B4;_LL7B9:;_LL7BA:
# 5066
 return bogus_ans;_LL7B4:;}
# 5068
if(finfo != 0  && finfo->width == 0){
# 5069
struct _tuple18 _tmp1619;return(_tmp1619.f1=(finfo->tq).real_const,((_tmp1619.f2=_tmpF97,_tmp1619)));}
# 5070
return bogus_ans;}_LL7B2:;_LL7B3:
# 5071
 return bogus_ans;_LL7AF:;};_LL78B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpF6D=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpF61;if(_tmpF6D->tag != 19)goto _LL78D;else{_tmpF6E=_tmpF6D->f1;}}_LL78C: {
# 5075
void*_tmpFA1=Cyc_Tcutil_compress((void*)_check_null(_tmpF6E->topt));struct Cyc_Absyn_PtrInfo _tmpFA3;struct Cyc_Absyn_Tqual _tmpFA4;struct Cyc_Absyn_PtrAtts _tmpFA5;void*_tmpFA6;_LL7BC: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpFA2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpFA1;if(_tmpFA2->tag != 5)goto _LL7BE;else{_tmpFA3=_tmpFA2->f1;_tmpFA4=_tmpFA3.elt_tq;_tmpFA5=_tmpFA3.ptr_atts;_tmpFA6=_tmpFA5.rgn;}}_LL7BD: {
# 5077
struct _tuple18 _tmp161A;return(_tmp161A.f1=_tmpFA4.real_const,((_tmp161A.f2=_tmpFA6,_tmp161A)));}_LL7BE:;_LL7BF:
# 5078
 return bogus_ans;_LL7BB:;}_LL78D: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpF6F=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF61;if(_tmpF6F->tag != 22)goto _LL78F;else{_tmpF70=_tmpF6F->f1;_tmpF71=_tmpF6F->f2;}}_LL78E: {
# 5083
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpF70->topt));
# 5084
void*_tmpFA8=t;struct Cyc_List_List*_tmpFAA;struct Cyc_Absyn_PtrInfo _tmpFAC;struct Cyc_Absyn_Tqual _tmpFAD;struct Cyc_Absyn_PtrAtts _tmpFAE;void*_tmpFAF;struct Cyc_Absyn_ArrayInfo _tmpFB1;struct Cyc_Absyn_Tqual _tmpFB2;_LL7C1: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpFA9=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpFA8;if(_tmpFA9->tag != 10)goto _LL7C3;else{_tmpFAA=_tmpFA9->f1;}}_LL7C2: {
# 5087
unsigned int _tmpFB4;int _tmpFB5;struct _tuple14 _tmpFB3=Cyc_Evexp_eval_const_uint_exp(_tmpF71);_tmpFB4=_tmpFB3.f1;_tmpFB5=_tmpFB3.f2;
# 5088
if(!_tmpFB5)
# 5089
return bogus_ans;{
# 5090
struct _tuple12*_tmpFB6=Cyc_Absyn_lookup_tuple_field(_tmpFAA,(int)_tmpFB4);
# 5091
if(_tmpFB6 != 0){
# 5092
struct _tuple18 _tmp161B;return(_tmp161B.f1=((*_tmpFB6).f1).real_const,((_tmp161B.f2=(Cyc_Tcutil_addressof_props(te,_tmpF70)).f2,_tmp161B)));}
# 5093
return bogus_ans;};}_LL7C3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpFAB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpFA8;if(_tmpFAB->tag != 5)goto _LL7C5;else{_tmpFAC=_tmpFAB->f1;_tmpFAD=_tmpFAC.elt_tq;_tmpFAE=_tmpFAC.ptr_atts;_tmpFAF=_tmpFAE.rgn;}}_LL7C4: {
# 5095
struct _tuple18 _tmp161C;return(_tmp161C.f1=_tmpFAD.real_const,((_tmp161C.f2=_tmpFAF,_tmp161C)));}_LL7C5: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpFB0=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpFA8;if(_tmpFB0->tag != 8)goto _LL7C7;else{_tmpFB1=_tmpFB0->f1;_tmpFB2=_tmpFB1.tq;}}_LL7C6: {
# 5101
struct _tuple18 _tmp161D;return(_tmp161D.f1=_tmpFB2.real_const,((_tmp161D.f2=(Cyc_Tcutil_addressof_props(te,_tmpF70)).f2,_tmp161D)));}_LL7C7:;_LL7C8:
# 5102
 return bogus_ans;_LL7C0:;}_LL78F:;_LL790:
# 5105
{const char*_tmp1620;void*_tmp161F;(_tmp161F=0,Cyc_Tcutil_terr(e->loc,((_tmp1620="unary & applied to non-lvalue",_tag_dyneither(_tmp1620,sizeof(char),30))),_tag_dyneither(_tmp161F,sizeof(void*),0)));}
# 5106
return bogus_ans;_LL784:;}
# 5117 "tcutil.cyc"
void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){
# 5118
void*_tmpFBD=Cyc_Tcutil_compress(e_typ);struct Cyc_Absyn_ArrayInfo _tmpFBF;void*_tmpFC0;struct Cyc_Absyn_Tqual _tmpFC1;union Cyc_Absyn_Constraint*_tmpFC2;_LL7CA: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpFBE=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpFBD;if(_tmpFBE->tag != 8)goto _LL7CC;else{_tmpFBF=_tmpFBE->f1;_tmpFC0=_tmpFBF.elt_type;_tmpFC1=_tmpFBF.tq;_tmpFC2=_tmpFBF.zero_term;}}_LL7CB: {
# 5120
void*_tmpFC4;struct _tuple18 _tmpFC3=Cyc_Tcutil_addressof_props(te,e);_tmpFC4=_tmpFC3.f2;{
# 5121
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp1623;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp1622;return Cyc_Absyn_atb_typ(_tmpFC0,_tmpFC4,_tmpFC1,(void*)((_tmp1622=_cycalloc(sizeof(*_tmp1622)),((_tmp1622[0]=((_tmp1623.tag=1,((_tmp1623.f1=e,_tmp1623)))),_tmp1622)))),_tmpFC2);};}_LL7CC:;_LL7CD:
# 5123
 return e_typ;_LL7C9:;}
# 5129
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
# 5130
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
# 5131
void*_tmpFC7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*_tmpFCA;_LL7CF: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpFC8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpFC7;if(_tmpFC8->tag != 0)goto _LL7D1;}_LL7D0:
# 5132
 return;_LL7D1: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpFC9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpFC7;if(_tmpFC9->tag != 1)goto _LL7CE;else{_tmpFCA=_tmpFC9->f1;}}_LL7D2: {
# 5134
unsigned int _tmpFCC;int _tmpFCD;struct _tuple14 _tmpFCB=Cyc_Evexp_eval_const_uint_exp(_tmpFCA);_tmpFCC=_tmpFCB.f1;_tmpFCD=_tmpFCB.f2;
# 5135
if(_tmpFCD  && _tmpFCC <= i){
# 5136
const char*_tmp1628;void*_tmp1627[2];struct Cyc_Int_pa_PrintArg_struct _tmp1626;struct Cyc_Int_pa_PrintArg_struct _tmp1625;(_tmp1625.tag=1,((_tmp1625.f1=(unsigned long)((int)i),((_tmp1626.tag=1,((_tmp1626.f1=(unsigned long)((int)_tmpFCC),((_tmp1627[0]=& _tmp1626,((_tmp1627[1]=& _tmp1625,Cyc_Tcutil_terr(loc,((_tmp1628="dereference is out of bounds: %d <= %d",_tag_dyneither(_tmp1628,sizeof(char),39))),_tag_dyneither(_tmp1627,sizeof(void*),2)))))))))))));}
# 5137
return;}_LL7CE:;};}
# 5141
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
# 5142
Cyc_Tcutil_check_bound(loc,0,b);}
# 5145
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
# 5146
void*_tmpFD2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);struct Cyc_Absyn_Exp*_tmpFD4;_LL7D4: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpFD3=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpFD2;if(_tmpFD3->tag != 1)goto _LL7D6;else{_tmpFD4=_tmpFD3->f1;}}_LL7D5: {
# 5148
unsigned int _tmpFD6;int _tmpFD7;struct _tuple14 _tmpFD5=Cyc_Evexp_eval_const_uint_exp(_tmpFD4);_tmpFD6=_tmpFD5.f1;_tmpFD7=_tmpFD5.f2;
# 5149
return _tmpFD7  && _tmpFD6 == 1;}_LL7D6:;_LL7D7:
# 5150
 return 0;_LL7D3:;}
# 5154
int Cyc_Tcutil_bits_only(void*t){
# 5155
void*_tmpFD8=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmpFDF;void*_tmpFE0;union Cyc_Absyn_Constraint*_tmpFE1;struct Cyc_List_List*_tmpFE3;struct Cyc_Absyn_AggrInfo _tmpFE5;union Cyc_Absyn_AggrInfoU _tmpFE6;struct _tuple4 _tmpFE7;struct Cyc_Absyn_AggrInfo _tmpFE9;union Cyc_Absyn_AggrInfoU _tmpFEA;struct Cyc_Absyn_Aggrdecl**_tmpFEB;struct Cyc_Absyn_Aggrdecl*_tmpFEC;struct Cyc_List_List*_tmpFED;struct Cyc_List_List*_tmpFEF;_LL7D9: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpFD9=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpFD8;if(_tmpFD9->tag != 0)goto _LL7DB;}_LL7DA:
# 5156
 goto _LL7DC;_LL7DB: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpFDA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpFD8;if(_tmpFDA->tag != 6)goto _LL7DD;}_LL7DC:
# 5157
 goto _LL7DE;_LL7DD: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpFDB=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpFD8;if(_tmpFDB->tag != 7)goto _LL7DF;}_LL7DE:
# 5158
 return 1;_LL7DF: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpFDC=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpFD8;if(_tmpFDC->tag != 13)goto _LL7E1;}_LL7E0:
# 5159
 goto _LL7E2;_LL7E1: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpFDD=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpFD8;if(_tmpFDD->tag != 14)goto _LL7E3;}_LL7E2:
# 5160
 return 0;_LL7E3: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpFDE=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpFD8;if(_tmpFDE->tag != 8)goto _LL7E5;else{_tmpFDF=_tmpFDE->f1;_tmpFE0=_tmpFDF.elt_type;_tmpFE1=_tmpFDF.zero_term;}}_LL7E4:
# 5164
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpFE1) && Cyc_Tcutil_bits_only(_tmpFE0);_LL7E5: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpFE2=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpFD8;if(_tmpFE2->tag != 10)goto _LL7E7;else{_tmpFE3=_tmpFE2->f1;}}_LL7E6:
# 5166
 for(0;_tmpFE3 != 0;_tmpFE3=_tmpFE3->tl){
# 5167
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpFE3->hd)).f2))return 0;}
# 5168
return 1;_LL7E7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpFE4=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpFD8;if(_tmpFE4->tag != 11)goto _LL7E9;else{_tmpFE5=_tmpFE4->f1;_tmpFE6=_tmpFE5.aggr_info;if((_tmpFE6.UnknownAggr).tag != 1)goto _LL7E9;_tmpFE7=(struct _tuple4)(_tmpFE6.UnknownAggr).val;}}_LL7E8:
# 5169
 return 0;_LL7E9: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpFE8=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpFD8;if(_tmpFE8->tag != 11)goto _LL7EB;else{_tmpFE9=_tmpFE8->f1;_tmpFEA=_tmpFE9.aggr_info;if((_tmpFEA.KnownAggr).tag != 2)goto _LL7EB;_tmpFEB=(struct Cyc_Absyn_Aggrdecl**)(_tmpFEA.KnownAggr).val;_tmpFEC=*_tmpFEB;_tmpFED=_tmpFE9.targs;}}_LL7EA:
# 5171
 if(_tmpFEC->impl == 0)
# 5172
return 0;{
# 5173
int okay=1;
# 5174
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpFEC->impl))->fields;for(0;fs != 0;fs=fs->tl){
# 5175
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
# 5176
if(okay)return 1;{
# 5177
struct _RegionHandle _tmpFF0=_new_region("rgn");struct _RegionHandle*rgn=& _tmpFF0;_push_region(rgn);
# 5178
{struct Cyc_List_List*_tmpFF1=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpFEC->tvs,_tmpFED);
# 5179
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpFEC->impl))->fields;for(0;fs != 0;fs=fs->tl){
# 5180
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpFF1,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpFF2=0;_npop_handler(0);return _tmpFF2;}}}{
# 5181
int _tmpFF3=1;_npop_handler(0);return _tmpFF3;};}
# 5178
;_pop_region(rgn);};};_LL7EB: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpFEE=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpFD8;if(_tmpFEE->tag != 12)goto _LL7ED;else{_tmpFEF=_tmpFEE->f2;}}_LL7EC:
# 5184
 for(0;_tmpFEF != 0;_tmpFEF=_tmpFEF->tl){
# 5185
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpFEF->hd)->type))return 0;}
# 5186
return 1;_LL7ED:;_LL7EE:
# 5187
 return 0;_LL7D8:;}
# 5196
static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){
# 5197
void*_tmpFF4=e->r;struct _tuple2*_tmpFFC;void*_tmpFFD;struct Cyc_Absyn_Exp*_tmpFFF;struct Cyc_Absyn_Exp*_tmp1000;struct Cyc_Absyn_Exp*_tmp1001;struct Cyc_Absyn_Exp*_tmp1003;struct Cyc_Absyn_Exp*_tmp1004;struct Cyc_Absyn_Exp*_tmp1006;struct Cyc_Absyn_Exp*_tmp1008;void*_tmp100A;struct Cyc_Absyn_Exp*_tmp100B;enum Cyc_Absyn_Coercion _tmp100C;void*_tmp100E;struct Cyc_Absyn_Exp*_tmp100F;struct Cyc_Absyn_Exp*_tmp1011;struct Cyc_Absyn_Exp*_tmp1013;struct Cyc_Absyn_Exp*_tmp1014;struct Cyc_List_List*_tmp1016;struct Cyc_List_List*_tmp1018;struct Cyc_List_List*_tmp101A;enum Cyc_Absyn_Primop _tmp101C;struct Cyc_List_List*_tmp101D;struct Cyc_List_List*_tmp101F;struct Cyc_List_List*_tmp1021;_LL7F0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpFF5=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpFF4;if(_tmpFF5->tag != 0)goto _LL7F2;}_LL7F1:
# 5198
 goto _LL7F3;_LL7F2: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpFF6=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpFF4;if(_tmpFF6->tag != 16)goto _LL7F4;}_LL7F3:
# 5199
 goto _LL7F5;_LL7F4: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpFF7=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpFF4;if(_tmpFF7->tag != 17)goto _LL7F6;}_LL7F5:
# 5200
 goto _LL7F7;_LL7F6: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpFF8=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpFF4;if(_tmpFF8->tag != 18)goto _LL7F8;}_LL7F7:
# 5201
 goto _LL7F9;_LL7F8: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpFF9=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpFF4;if(_tmpFF9->tag != 30)goto _LL7FA;}_LL7F9:
# 5202
 goto _LL7FB;_LL7FA: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpFFA=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpFF4;if(_tmpFFA->tag != 31)goto _LL7FC;}_LL7FB:
# 5203
 return 1;_LL7FC: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpFFB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpFF4;if(_tmpFFB->tag != 1)goto _LL7FE;else{_tmpFFC=_tmpFFB->f1;_tmpFFD=(void*)_tmpFFB->f2;}}_LL7FD: {
# 5207
void*_tmp1022=_tmpFFD;struct Cyc_Absyn_Vardecl*_tmp1025;_LL81D: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp1023=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp1022;if(_tmp1023->tag != 2)goto _LL81F;}_LL81E:
# 5208
 return 1;_LL81F: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp1024=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp1022;if(_tmp1024->tag != 1)goto _LL821;else{_tmp1025=_tmp1024->f1;}}_LL820: {
# 5210
void*_tmp1027=Cyc_Tcutil_compress(_tmp1025->type);_LL826: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1028=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1027;if(_tmp1028->tag != 8)goto _LL828;}_LL827:
# 5211
 goto _LL829;_LL828: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1029=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1027;if(_tmp1029->tag != 9)goto _LL82A;}_LL829:
# 5212
 return 1;_LL82A:;_LL82B:
# 5213
 return var_okay;_LL825:;}_LL821: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp1026=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp1022;if(_tmp1026->tag != 0)goto _LL823;}_LL822:
# 5215
 return 0;_LL823:;_LL824:
# 5216
 return var_okay;_LL81C:;}_LL7FE: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpFFE=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpFF4;if(_tmpFFE->tag != 5)goto _LL800;else{_tmpFFF=_tmpFFE->f1;_tmp1000=_tmpFFE->f2;_tmp1001=_tmpFFE->f3;}}_LL7FF:
# 5220
 return(Cyc_Tcutil_cnst_exp(te,0,_tmpFFF) && 
# 5221
Cyc_Tcutil_cnst_exp(te,0,_tmp1000)) && 
# 5222
Cyc_Tcutil_cnst_exp(te,0,_tmp1001);_LL800: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp1002=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpFF4;if(_tmp1002->tag != 8)goto _LL802;else{_tmp1003=_tmp1002->f1;_tmp1004=_tmp1002->f2;}}_LL801:
# 5224
 return Cyc_Tcutil_cnst_exp(te,0,_tmp1003) && Cyc_Tcutil_cnst_exp(te,0,_tmp1004);_LL802: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp1005=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpFF4;if(_tmp1005->tag != 11)goto _LL804;else{_tmp1006=_tmp1005->f1;}}_LL803:
# 5225
 _tmp1008=_tmp1006;goto _LL805;_LL804: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp1007=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpFF4;if(_tmp1007->tag != 12)goto _LL806;else{_tmp1008=_tmp1007->f1;}}_LL805:
# 5227
 return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp1008);_LL806: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1009=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpFF4;if(_tmp1009->tag != 13)goto _LL808;else{_tmp100A=(void*)_tmp1009->f1;_tmp100B=_tmp1009->f2;_tmp100C=_tmp1009->f4;if(_tmp100C != Cyc_Absyn_No_coercion)goto _LL808;}}_LL807:
# 5229
 return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp100B);_LL808: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp100D=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpFF4;if(_tmp100D->tag != 13)goto _LL80A;else{_tmp100E=(void*)_tmp100D->f1;_tmp100F=_tmp100D->f2;}}_LL809:
# 5232
 return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp100F);_LL80A: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp1010=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpFF4;if(_tmp1010->tag != 14)goto _LL80C;else{_tmp1011=_tmp1010->f1;}}_LL80B:
# 5234
 return Cyc_Tcutil_cnst_exp(te,1,_tmp1011);_LL80C: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp1012=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpFF4;if(_tmp1012->tag != 26)goto _LL80E;else{_tmp1013=_tmp1012->f2;_tmp1014=_tmp1012->f3;}}_LL80D:
# 5236
 return Cyc_Tcutil_cnst_exp(te,0,_tmp1013) && Cyc_Tcutil_cnst_exp(te,0,_tmp1014);_LL80E: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1015=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpFF4;if(_tmp1015->tag != 25)goto _LL810;else{_tmp1016=_tmp1015->f1;}}_LL80F:
# 5237
 _tmp1018=_tmp1016;goto _LL811;_LL810: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp1017=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpFF4;if(_tmp1017->tag != 28)goto _LL812;else{_tmp1018=_tmp1017->f2;}}_LL811:
# 5238
 _tmp101A=_tmp1018;goto _LL813;_LL812: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1019=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpFF4;if(_tmp1019->tag != 27)goto _LL814;else{_tmp101A=_tmp1019->f3;}}_LL813:
# 5240
 for(0;_tmp101A != 0;_tmp101A=_tmp101A->tl){
# 5241
if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple19*)_tmp101A->hd)).f2))
# 5242
return 0;}
# 5243
return 1;_LL814: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp101B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpFF4;if(_tmp101B->tag != 2)goto _LL816;else{_tmp101C=_tmp101B->f1;_tmp101D=_tmp101B->f2;}}_LL815:
# 5245
 _tmp101F=_tmp101D;goto _LL817;_LL816: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp101E=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpFF4;if(_tmp101E->tag != 23)goto _LL818;else{_tmp101F=_tmp101E->f1;}}_LL817:
# 5246
 _tmp1021=_tmp101F;goto _LL819;_LL818: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1020=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpFF4;if(_tmp1020->tag != 29)goto _LL81A;else{_tmp1021=_tmp1020->f1;}}_LL819:
# 5248
 for(0;_tmp1021 != 0;_tmp1021=_tmp1021->tl){
# 5249
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmp1021->hd))
# 5250
return 0;}
# 5251
return 1;_LL81A:;_LL81B:
# 5252
 return 0;_LL7EF:;}
# 5256
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5257
return Cyc_Tcutil_cnst_exp(te,0,e);}
# 5260
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5262
int Cyc_Tcutil_supports_default(void*t){
# 5263
void*_tmp102A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp102F;struct Cyc_Absyn_PtrAtts _tmp1030;union Cyc_Absyn_Constraint*_tmp1031;union Cyc_Absyn_Constraint*_tmp1032;struct Cyc_Absyn_ArrayInfo _tmp1034;void*_tmp1035;struct Cyc_List_List*_tmp1037;struct Cyc_Absyn_AggrInfo _tmp1039;union Cyc_Absyn_AggrInfoU _tmp103A;struct Cyc_List_List*_tmp103B;struct Cyc_List_List*_tmp103D;_LL82D: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp102B=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp102A;if(_tmp102B->tag != 0)goto _LL82F;}_LL82E:
# 5264
 goto _LL830;_LL82F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp102C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp102A;if(_tmp102C->tag != 6)goto _LL831;}_LL830:
# 5265
 goto _LL832;_LL831: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp102D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp102A;if(_tmp102D->tag != 7)goto _LL833;}_LL832:
# 5266
 return 1;_LL833: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp102E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp102A;if(_tmp102E->tag != 5)goto _LL835;else{_tmp102F=_tmp102E->f1;_tmp1030=_tmp102F.ptr_atts;_tmp1031=_tmp1030.nullable;_tmp1032=_tmp1030.bounds;}}_LL834: {
# 5269
void*_tmp1040=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp1032);_LL844: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp1041=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1040;if(_tmp1041->tag != 0)goto _LL846;}_LL845:
# 5270
 return 1;_LL846:;_LL847:
# 5272
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp1031);_LL843:;}_LL835: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1033=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp102A;if(_tmp1033->tag != 8)goto _LL837;else{_tmp1034=_tmp1033->f1;_tmp1035=_tmp1034.elt_type;}}_LL836:
# 5275
 return Cyc_Tcutil_supports_default(_tmp1035);_LL837: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1036=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp102A;if(_tmp1036->tag != 10)goto _LL839;else{_tmp1037=_tmp1036->f1;}}_LL838:
# 5277
 for(0;_tmp1037 != 0;_tmp1037=_tmp1037->tl){
# 5278
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmp1037->hd)).f2))return 0;}
# 5279
return 1;_LL839: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1038=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp102A;if(_tmp1038->tag != 11)goto _LL83B;else{_tmp1039=_tmp1038->f1;_tmp103A=_tmp1039.aggr_info;_tmp103B=_tmp1039.targs;}}_LL83A: {
# 5281
struct Cyc_Absyn_Aggrdecl*_tmp1042=Cyc_Absyn_get_known_aggrdecl(_tmp103A);
# 5282
if(_tmp1042->impl == 0)return 0;
# 5283
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1042->impl))->exist_vars != 0)return 0;
# 5284
return Cyc_Tcutil_fields_support_default(_tmp1042->tvs,_tmp103B,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1042->impl))->fields);}_LL83B: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp103C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp102A;if(_tmp103C->tag != 12)goto _LL83D;else{_tmp103D=_tmp103C->f2;}}_LL83C:
# 5285
 return Cyc_Tcutil_fields_support_default(0,0,_tmp103D);_LL83D: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp103E=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp102A;if(_tmp103E->tag != 14)goto _LL83F;}_LL83E:
# 5287
 goto _LL840;_LL83F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp103F=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp102A;if(_tmp103F->tag != 13)goto _LL841;}_LL840:
# 5288
 return 1;_LL841:;_LL842:
# 5290
 return 0;_LL82C:;}
# 5295
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
# 5296
void*_tmp1043=t;struct Cyc_Absyn_Typedefdecl*_tmp1045;_LL849: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1044=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1043;if(_tmp1044->tag != 17)goto _LL84B;else{_tmp1045=_tmp1044->f3;}}_LL84A:
# 5298
 if(_tmp1045 != 0){
# 5299
struct Cyc_Absyn_Tqual _tmp1046=_tmp1045->tq;
# 5300
if(((_tmp1046.print_const  || _tmp1046.q_volatile) || _tmp1046.q_restrict) || _tmp1046.real_const){
# 5303
const char*_tmp162C;void*_tmp162B[1];struct Cyc_String_pa_PrintArg_struct _tmp162A;(_tmp162A.tag=0,((_tmp162A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp162B[0]=& _tmp162A,Cyc_Tcutil_warn(loc,((_tmp162C="qualifier within typedef type %s is ignored",_tag_dyneither(_tmp162C,sizeof(char),44))),_tag_dyneither(_tmp162B,sizeof(void*),1)))))));}}
# 5306
goto _LL848;_LL84B:;_LL84C:
# 5307
 goto _LL848;_LL848:;}
# 5311
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5313
struct _RegionHandle _tmp104A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp104A;_push_region(rgn);{
# 5314
struct Cyc_List_List*_tmp104B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
# 5315
for(0;fs != 0;fs=fs->tl){
# 5316
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
# 5317
if(Cyc_Tcutil_supports_default(t)){int _tmp104C=1;_npop_handler(0);return _tmp104C;}
# 5318
t=Cyc_Tcutil_rsubstitute(rgn,_tmp104B,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
# 5319
if(!Cyc_Tcutil_supports_default(t)){int _tmp104D=0;_npop_handler(0);return _tmp104D;}}}{
# 5322
int _tmp104E=1;_npop_handler(0);return _tmp104E;};
# 5313
;_pop_region(rgn);}
# 5328
int Cyc_Tcutil_admits_zero(void*t){
# 5329
void*_tmp104F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1053;struct Cyc_Absyn_PtrAtts _tmp1054;union Cyc_Absyn_Constraint*_tmp1055;union Cyc_Absyn_Constraint*_tmp1056;_LL84E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1050=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp104F;if(_tmp1050->tag != 6)goto _LL850;}_LL84F:
# 5330
 goto _LL851;_LL850: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp1051=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp104F;if(_tmp1051->tag != 7)goto _LL852;}_LL851:
# 5331
 return 1;_LL852: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1052=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp104F;if(_tmp1052->tag != 5)goto _LL854;else{_tmp1053=_tmp1052->f1;_tmp1054=_tmp1053.ptr_atts;_tmp1055=_tmp1054.nullable;_tmp1056=_tmp1054.bounds;}}_LL853: {
# 5333
void*_tmp1057=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp1056);_LL857: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp1058=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1057;if(_tmp1058->tag != 0)goto _LL859;}_LL858:
# 5337
 return 0;_LL859:;_LL85A:
# 5338
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp1055);_LL856:;}_LL854:;_LL855:
# 5340
 return 0;_LL84D:;}
# 5344
int Cyc_Tcutil_is_noreturn(void*t){
# 5345
{void*_tmp1059=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp105B;void*_tmp105C;struct Cyc_Absyn_FnInfo _tmp105E;struct Cyc_List_List*_tmp105F;_LL85C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp105A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1059;if(_tmp105A->tag != 5)goto _LL85E;else{_tmp105B=_tmp105A->f1;_tmp105C=_tmp105B.elt_typ;}}_LL85D:
# 5346
 return Cyc_Tcutil_is_noreturn(_tmp105C);_LL85E: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp105D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1059;if(_tmp105D->tag != 9)goto _LL860;else{_tmp105E=_tmp105D->f1;_tmp105F=_tmp105E.attributes;}}_LL85F:
# 5348
 for(0;_tmp105F != 0;_tmp105F=_tmp105F->tl){
# 5349
void*_tmp1060=(void*)_tmp105F->hd;_LL863: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp1061=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp1060;if(_tmp1061->tag != 4)goto _LL865;}_LL864:
# 5350
 return 1;_LL865:;_LL866:
# 5351
 continue;_LL862:;}
# 5353
goto _LL85B;_LL860:;_LL861:
# 5354
 goto _LL85B;_LL85B:;}
# 5356
return 0;}
# 5361
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
# 5362
void*_tmp1062=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp1064;struct Cyc_List_List*_tmp1065;struct Cyc_List_List**_tmp1066;_LL868: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1063=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1062;if(_tmp1063->tag != 9)goto _LL86A;else{_tmp1064=_tmp1063->f1;_tmp1065=_tmp1064.attributes;_tmp1066=(struct Cyc_List_List**)&(_tmp1063->f1).attributes;}}_LL869: {
# 5364
struct Cyc_List_List*_tmp1067=0;
# 5365
for(0;atts != 0;atts=atts->tl){
# 5366
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
# 5367
struct Cyc_List_List*_tmp162D;*_tmp1066=((_tmp162D=_cycalloc(sizeof(*_tmp162D)),((_tmp162D->hd=(void*)atts->hd,((_tmp162D->tl=*_tmp1066,_tmp162D))))));}else{
# 5369
struct Cyc_List_List*_tmp162E;_tmp1067=((_tmp162E=_cycalloc(sizeof(*_tmp162E)),((_tmp162E->hd=(void*)atts->hd,((_tmp162E->tl=_tmp1067,_tmp162E))))));}}
# 5370
return _tmp1067;}_LL86A:;_LL86B: {
# 5371
const char*_tmp1631;void*_tmp1630;(_tmp1630=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1631="transfer_fn_type_atts",_tag_dyneither(_tmp1631,sizeof(char),22))),_tag_dyneither(_tmp1630,sizeof(void*),0)));}_LL867:;}
