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
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 523
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 905 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 907
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 915
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 918
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
# 919
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 920
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
# 922
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 925
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 927
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
# 928
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
# 929
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 930
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 933
void*Cyc_Absyn_compress_kb(void*);
# 938
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 943
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 945
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
# 949
extern void*Cyc_Absyn_empty_effect;
# 959
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
# 960
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 968
extern void*Cyc_Absyn_bounds_one;
# 973
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 999
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1000
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 1002
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1016
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple2*,void*,unsigned int);
# 1018
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1031
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1033
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1045
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1046
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1050
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1051
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,unsigned int);
# 1052
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
# 1062
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1101
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,unsigned int);
# 1102
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1146
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1148
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1150
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1152
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1154
int Cyc_Absyn_fntype_att(void*a);struct _tuple13{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1158
struct _tuple13 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1160
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
const char*_tmp107B;void*_tmp107A[2];const char*_tmp1079;const char*_tmp1078;struct Cyc_String_pa_PrintArg_struct _tmp1077;struct Cyc_String_pa_PrintArg_struct _tmp1076;struct _dyneither_ptr s1=(struct _dyneither_ptr)((_tmp1076.tag=0,((_tmp1076.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 68
Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure)),((_tmp1077.tag=0,((_tmp1077.f1=(struct _dyneither_ptr)(
# 67
Cyc_Tcutil_tq1_const?(_tmp1078="const ",_tag_dyneither(_tmp1078,sizeof(char),7)):((_tmp1079="",_tag_dyneither(_tmp1079,sizeof(char),1)))),((_tmp107A[0]=& _tmp1077,((_tmp107A[1]=& _tmp1076,Cyc_aprintf(((_tmp107B="%s%s",_tag_dyneither(_tmp107B,sizeof(char),5))),_tag_dyneither(_tmp107A,sizeof(void*),2))))))))))))));
# 69
const char*_tmp1084;void*_tmp1083[2];const char*_tmp1082;const char*_tmp1081;struct Cyc_String_pa_PrintArg_struct _tmp1080;struct Cyc_String_pa_PrintArg_struct _tmp107F;struct _dyneither_ptr s2=(struct _dyneither_ptr)((_tmp107F.tag=0,((_tmp107F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 70
Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure)),((_tmp1080.tag=0,((_tmp1080.f1=(struct _dyneither_ptr)(
# 69
Cyc_Tcutil_tq2_const?(_tmp1081="const ",_tag_dyneither(_tmp1081,sizeof(char),7)):((_tmp1082="",_tag_dyneither(_tmp1082,sizeof(char),1)))),((_tmp1083[0]=& _tmp1080,((_tmp1083[1]=& _tmp107F,Cyc_aprintf(((_tmp1084="%s%s",_tag_dyneither(_tmp1084,sizeof(char),5))),_tag_dyneither(_tmp1083,sizeof(void*),2))))))))))))));
# 71
int pos=2;
# 72
{const char*_tmp1088;void*_tmp1087[1];struct Cyc_String_pa_PrintArg_struct _tmp1086;(_tmp1086.tag=0,((_tmp1086.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp1087[0]=& _tmp1086,Cyc_fprintf(Cyc_stderr,((_tmp1088="  %s",_tag_dyneither(_tmp1088,sizeof(char),5))),_tag_dyneither(_tmp1087,sizeof(void*),1)))))));}
# 73
pos +=_get_dyneither_size(s1,sizeof(char));
# 74
if(pos + 5 >= 80){
# 75
{const char*_tmp108B;void*_tmp108A;(_tmp108A=0,Cyc_fprintf(Cyc_stderr,((_tmp108B="\n\t",_tag_dyneither(_tmp108B,sizeof(char),3))),_tag_dyneither(_tmp108A,sizeof(void*),0)));}
# 76
pos=8;}else{
# 78
{const char*_tmp108E;void*_tmp108D;(_tmp108D=0,Cyc_fprintf(Cyc_stderr,((_tmp108E=" ",_tag_dyneither(_tmp108E,sizeof(char),2))),_tag_dyneither(_tmp108D,sizeof(void*),0)));}
# 79
++ pos;}
# 81
{const char*_tmp1091;void*_tmp1090;(_tmp1090=0,Cyc_fprintf(Cyc_stderr,((_tmp1091="and ",_tag_dyneither(_tmp1091,sizeof(char),5))),_tag_dyneither(_tmp1090,sizeof(void*),0)));}
# 82
pos +=4;
# 83
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
# 84
{const char*_tmp1094;void*_tmp1093;(_tmp1093=0,Cyc_fprintf(Cyc_stderr,((_tmp1094="\n\t",_tag_dyneither(_tmp1094,sizeof(char),3))),_tag_dyneither(_tmp1093,sizeof(void*),0)));}
# 85
pos=8;}
# 87
{const char*_tmp1098;void*_tmp1097[1];struct Cyc_String_pa_PrintArg_struct _tmp1096;(_tmp1096.tag=0,((_tmp1096.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmp1097[0]=& _tmp1096,Cyc_fprintf(Cyc_stderr,((_tmp1098="%s ",_tag_dyneither(_tmp1098,sizeof(char),4))),_tag_dyneither(_tmp1097,sizeof(void*),1)))))));}
# 88
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
# 89
if(pos + 17 >= 80){
# 90
{const char*_tmp109B;void*_tmp109A;(_tmp109A=0,Cyc_fprintf(Cyc_stderr,((_tmp109B="\n\t",_tag_dyneither(_tmp109B,sizeof(char),3))),_tag_dyneither(_tmp109A,sizeof(void*),0)));}
# 91
pos=8;}
# 93
{const char*_tmp109E;void*_tmp109D;(_tmp109D=0,Cyc_fprintf(Cyc_stderr,((_tmp109E="are not compatible. ",_tag_dyneither(_tmp109E,sizeof(char),21))),_tag_dyneither(_tmp109D,sizeof(void*),0)));}
# 94
pos +=17;
# 95
if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
# 96
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){
# 97
const char*_tmp10A1;void*_tmp10A0;(_tmp10A0=0,Cyc_fprintf(Cyc_stderr,((_tmp10A1="\n\t",_tag_dyneither(_tmp10A1,sizeof(char),3))),_tag_dyneither(_tmp10A0,sizeof(void*),0)));}{
# 99
const char*_tmp10A5;void*_tmp10A4[1];struct Cyc_String_pa_PrintArg_struct _tmp10A3;(_tmp10A3.tag=0,((_tmp10A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmp10A4[0]=& _tmp10A3,Cyc_fprintf(Cyc_stderr,((_tmp10A5="%s",_tag_dyneither(_tmp10A5,sizeof(char),3))),_tag_dyneither(_tmp10A4,sizeof(void*),1)))))));};}
# 101
{const char*_tmp10A8;void*_tmp10A7;(_tmp10A7=0,Cyc_fprintf(Cyc_stderr,((_tmp10A8="\n",_tag_dyneither(_tmp10A8,sizeof(char),2))),_tag_dyneither(_tmp10A7,sizeof(void*),0)));}
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
{const char*_tmp10AC;void*_tmp10AB[1];struct Cyc_String_pa_PrintArg_struct _tmp10AA;(_tmp10AA.tag=0,((_tmp10AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp10AB[0]=& _tmp10AA,Cyc_fprintf(Cyc_stderr,((_tmp10AC="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(_tmp10AC,sizeof(char),36))),_tag_dyneither(_tmp10AB,sizeof(void*),1)))))));}
# 116
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
# 117
struct Cyc_Core_Impossible_exn_struct _tmp10AF;struct Cyc_Core_Impossible_exn_struct*_tmp10AE;(int)_throw((void*)((_tmp10AE=_cycalloc(sizeof(*_tmp10AE)),((_tmp10AE[0]=((_tmp10AF.tag=Cyc_Core_Impossible,((_tmp10AF.f1=msg,_tmp10AF)))),_tmp10AE)))));};}
# 120
static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){
# 121
return*tv->name;}
# 124
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){
# 125
for(0;tvs != 0;tvs=tvs->tl){
# 126
const char*_tmp10B4;void*_tmp10B3[2];struct Cyc_String_pa_PrintArg_struct _tmp10B2;struct Cyc_String_pa_PrintArg_struct _tmp10B1;(_tmp10B1.tag=0,((_tmp10B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 127
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmp10B2.tag=0,((_tmp10B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmp10B3[0]=& _tmp10B2,((_tmp10B3[1]=& _tmp10B1,Cyc_fprintf(Cyc_stderr,((_tmp10B4="%s::%s ",_tag_dyneither(_tmp10B4,sizeof(char),8))),_tag_dyneither(_tmp10B3,sizeof(void*),2)))))))))))));}
# 128
{const char*_tmp10B7;void*_tmp10B6;(_tmp10B6=0,Cyc_fprintf(Cyc_stderr,((_tmp10B7="\n",_tag_dyneither(_tmp10B7,sizeof(char),2))),_tag_dyneither(_tmp10B6,sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}
# 131
static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
# 132
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;
# 136
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 139
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
# 140
{struct Cyc_List_List*_tmp10B8;Cyc_Tcutil_warning_segs=((_tmp10B8=_cycalloc(sizeof(*_tmp10B8)),((_tmp10B8->hd=(void*)sg,((_tmp10B8->tl=Cyc_Tcutil_warning_segs,_tmp10B8))))));}{
# 141
struct _dyneither_ptr*_tmp10BB;struct Cyc_List_List*_tmp10BA;Cyc_Tcutil_warning_msgs=((_tmp10BA=_cycalloc(sizeof(*_tmp10BA)),((_tmp10BA->hd=((_tmp10BB=_cycalloc(sizeof(*_tmp10BB)),((_tmp10BB[0]=msg,_tmp10BB)))),((_tmp10BA->tl=Cyc_Tcutil_warning_msgs,_tmp10BA))))));};}
# 143
void Cyc_Tcutil_flush_warnings(){
# 144
if(Cyc_Tcutil_warning_segs == 0)
# 145
return;
# 146
{const char*_tmp10BE;void*_tmp10BD;(_tmp10BD=0,Cyc_fprintf(Cyc_stderr,((_tmp10BE="***Warnings***\n",_tag_dyneither(_tmp10BE,sizeof(char),16))),_tag_dyneither(_tmp10BD,sizeof(void*),0)));}{
# 147
struct Cyc_List_List*_tmp36=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
# 148
Cyc_Tcutil_warning_segs=0;
# 149
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
# 150
while(Cyc_Tcutil_warning_msgs != 0){
# 151
{const char*_tmp10C3;void*_tmp10C2[2];struct Cyc_String_pa_PrintArg_struct _tmp10C1;struct Cyc_String_pa_PrintArg_struct _tmp10C0;(_tmp10C0.tag=0,((_tmp10C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmp10C1.tag=0,((_tmp10C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp36))->hd)),((_tmp10C2[0]=& _tmp10C1,((_tmp10C2[1]=& _tmp10C0,Cyc_fprintf(Cyc_stderr,((_tmp10C3="%s: %s\n",_tag_dyneither(_tmp10C3,sizeof(char),8))),_tag_dyneither(_tmp10C2,sizeof(void*),2)))))))))))));}
# 152
_tmp36=_tmp36->tl;
# 153
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}
# 155
{const char*_tmp10C6;void*_tmp10C5;(_tmp10C5=0,Cyc_fprintf(Cyc_stderr,((_tmp10C6="**************\n",_tag_dyneither(_tmp10C6,sizeof(char),16))),_tag_dyneither(_tmp10C5,sizeof(void*),0)));}
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
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp10C9;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp10C8;return(void*)((_tmp10C8=_cycalloc(sizeof(*_tmp10C8)),((_tmp10C8[0]=((_tmp10C9.tag=1,((_tmp10C9.f1=0,_tmp10C9)))),_tmp10C8))));}_LL1E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp59=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp57;if(_tmp59->tag != 2)goto _LL20;else{_tmp5A=_tmp59->f2;}}_LL1F: {
# 214
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp10CC;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp10CB;return(void*)((_tmp10CB=_cycalloc(sizeof(*_tmp10CB)),((_tmp10CB[0]=((_tmp10CC.tag=2,((_tmp10CC.f1=0,((_tmp10CC.f2=_tmp5A,_tmp10CC)))))),_tmp10CB))));}_LL20:;_LL21:
# 215
 return kb;_LL1B:;}
# 218
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 220
struct Cyc_Absyn_Tvar*_tmp10CD;return(_tmp10CD=_cycalloc(sizeof(*_tmp10CD)),((_tmp10CD->name=tv->name,((_tmp10CD->identity=- 1,((_tmp10CD->kind=Cyc_Tcutil_copy_kindbound(tv->kind),_tmp10CD)))))));}
# 222
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 224
struct _tuple10 _tmp61;struct _dyneither_ptr*_tmp62;struct Cyc_Absyn_Tqual _tmp63;void*_tmp64;struct _tuple10*_tmp60=arg;_tmp61=*_tmp60;_tmp62=_tmp61.f1;_tmp63=_tmp61.f2;_tmp64=_tmp61.f3;{
# 225
struct _tuple10*_tmp10CE;return(_tmp10CE=_cycalloc(sizeof(*_tmp10CE)),((_tmp10CE->f1=_tmp62,((_tmp10CE->f2=_tmp63,((_tmp10CE->f3=Cyc_Tcutil_copy_type(_tmp64),_tmp10CE)))))));};}
# 227
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
# 228
struct _tuple12 _tmp67;struct Cyc_Absyn_Tqual _tmp68;void*_tmp69;struct _tuple12*_tmp66=arg;_tmp67=*_tmp66;_tmp68=_tmp67.f1;_tmp69=_tmp67.f2;{
# 229
struct _tuple12*_tmp10CF;return(_tmp10CF=_cycalloc(sizeof(*_tmp10CF)),((_tmp10CF->f1=_tmp68,((_tmp10CF->f2=Cyc_Tcutil_copy_type(_tmp69),_tmp10CF)))));};}
# 231
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
# 232
struct Cyc_Absyn_Aggrfield*_tmp10D0;return(_tmp10D0=_cycalloc(sizeof(*_tmp10D0)),((_tmp10D0->name=f->name,((_tmp10D0->tq=f->tq,((_tmp10D0->type=Cyc_Tcutil_copy_type(f->type),((_tmp10D0->width=f->width,((_tmp10D0->attributes=f->attributes,_tmp10D0)))))))))));}
# 234
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
# 235
struct _tuple0 _tmp6D;void*_tmp6E;void*_tmp6F;struct _tuple0*_tmp6C=x;_tmp6D=*_tmp6C;_tmp6E=_tmp6D.f1;_tmp6F=_tmp6D.f2;{
# 236
struct _tuple0*_tmp10D1;return(_tmp10D1=_cycalloc(sizeof(*_tmp10D1)),((_tmp10D1->f1=Cyc_Tcutil_copy_type(_tmp6E),((_tmp10D1->f2=Cyc_Tcutil_copy_type(_tmp6F),_tmp10D1)))));};}
# 238
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
# 239
struct Cyc_Absyn_Enumfield*_tmp10D2;return(_tmp10D2=_cycalloc(sizeof(*_tmp10D2)),((_tmp10D2->name=f->name,((_tmp10D2->tag=f->tag,((_tmp10D2->loc=f->loc,_tmp10D2)))))));}
# 241
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
# 242
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10D5;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp10D4;return(void*)((_tmp10D4=_cycalloc(sizeof(*_tmp10D4)),((_tmp10D4[0]=((_tmp10D5.tag=2,((_tmp10D5.f1=Cyc_Tcutil_copy_tvar(t),_tmp10D5)))),_tmp10D4))));}
# 244
void*Cyc_Tcutil_copy_type(void*t){
# 245
void*_tmp74=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp78;struct Cyc_Absyn_DatatypeInfo _tmp7A;union Cyc_Absyn_DatatypeInfoU _tmp7B;struct Cyc_List_List*_tmp7C;struct Cyc_Absyn_DatatypeFieldInfo _tmp7E;union Cyc_Absyn_DatatypeFieldInfoU _tmp7F;struct Cyc_List_List*_tmp80;struct Cyc_Absyn_PtrInfo _tmp82;void*_tmp83;struct Cyc_Absyn_Tqual _tmp84;struct Cyc_Absyn_PtrAtts _tmp85;void*_tmp86;union Cyc_Absyn_Constraint*_tmp87;union Cyc_Absyn_Constraint*_tmp88;union Cyc_Absyn_Constraint*_tmp89;struct Cyc_Absyn_PtrLoc*_tmp8A;struct Cyc_Absyn_ArrayInfo _tmp8E;void*_tmp8F;struct Cyc_Absyn_Tqual _tmp90;struct Cyc_Absyn_Exp*_tmp91;union Cyc_Absyn_Constraint*_tmp92;unsigned int _tmp93;struct Cyc_Absyn_FnInfo _tmp95;struct Cyc_List_List*_tmp96;void*_tmp97;struct Cyc_Absyn_Tqual _tmp98;void*_tmp99;struct Cyc_List_List*_tmp9A;int _tmp9B;struct Cyc_Absyn_VarargInfo*_tmp9C;struct Cyc_List_List*_tmp9D;struct Cyc_List_List*_tmp9E;struct Cyc_List_List*_tmpA0;struct Cyc_Absyn_AggrInfo _tmpA2;union Cyc_Absyn_AggrInfoU _tmpA3;struct _tuple4 _tmpA4;enum Cyc_Absyn_AggrKind _tmpA5;struct _tuple2*_tmpA6;struct Cyc_Core_Opt*_tmpA7;struct Cyc_List_List*_tmpA8;struct Cyc_Absyn_AggrInfo _tmpAA;union Cyc_Absyn_AggrInfoU _tmpAB;struct Cyc_Absyn_Aggrdecl**_tmpAC;struct Cyc_List_List*_tmpAD;enum Cyc_Absyn_AggrKind _tmpAF;struct Cyc_List_List*_tmpB0;struct _tuple2*_tmpB2;struct Cyc_Absyn_Enumdecl*_tmpB3;struct Cyc_List_List*_tmpB5;void*_tmpB7;struct Cyc_Absyn_Exp*_tmpB9;void*_tmpBB;void*_tmpBD;void*_tmpBE;struct _tuple2*_tmpC0;struct Cyc_List_List*_tmpC1;struct Cyc_Absyn_Typedefdecl*_tmpC2;void*_tmpC7;struct Cyc_List_List*_tmpC9;void*_tmpCB;struct Cyc_Absyn_TypeDecl*_tmpCD;struct Cyc_Absyn_TypeDecl _tmpCE;void*_tmpCF;struct Cyc_Absyn_Aggrdecl*_tmpD1;struct Cyc_Absyn_TypeDecl*_tmpD3;struct Cyc_Absyn_TypeDecl _tmpD4;void*_tmpD5;struct Cyc_Absyn_Enumdecl*_tmpD7;struct Cyc_Absyn_TypeDecl*_tmpD9;struct Cyc_Absyn_TypeDecl _tmpDA;void*_tmpDB;struct Cyc_Absyn_Datatypedecl*_tmpDD;_LL23: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp75=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp74;if(_tmp75->tag != 0)goto _LL25;}_LL24:
# 246
 goto _LL26;_LL25: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp76=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp74;if(_tmp76->tag != 1)goto _LL27;}_LL26:
# 247
 return t;_LL27: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp77=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp74;if(_tmp77->tag != 2)goto _LL29;else{_tmp78=_tmp77->f1;}}_LL28: {
# 248
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10D8;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp10D7;return(void*)((_tmp10D7=_cycalloc(sizeof(*_tmp10D7)),((_tmp10D7[0]=((_tmp10D8.tag=2,((_tmp10D8.f1=Cyc_Tcutil_copy_tvar(_tmp78),_tmp10D8)))),_tmp10D7))));}_LL29: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp79=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp74;if(_tmp79->tag != 3)goto _LL2B;else{_tmp7A=_tmp79->f1;_tmp7B=_tmp7A.datatype_info;_tmp7C=_tmp7A.targs;}}_LL2A: {
# 250
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp10DE;struct Cyc_Absyn_DatatypeInfo _tmp10DD;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp10DC;return(void*)((_tmp10DC=_cycalloc(sizeof(*_tmp10DC)),((_tmp10DC[0]=((_tmp10DE.tag=3,((_tmp10DE.f1=((_tmp10DD.datatype_info=_tmp7B,((_tmp10DD.targs=Cyc_Tcutil_copy_types(_tmp7C),_tmp10DD)))),_tmp10DE)))),_tmp10DC))));}_LL2B: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp7D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp74;if(_tmp7D->tag != 4)goto _LL2D;else{_tmp7E=_tmp7D->f1;_tmp7F=_tmp7E.field_info;_tmp80=_tmp7E.targs;}}_LL2C: {
# 252
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp10E4;struct Cyc_Absyn_DatatypeFieldInfo _tmp10E3;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp10E2;return(void*)((_tmp10E2=_cycalloc(sizeof(*_tmp10E2)),((_tmp10E2[0]=((_tmp10E4.tag=4,((_tmp10E4.f1=((_tmp10E3.field_info=_tmp7F,((_tmp10E3.targs=Cyc_Tcutil_copy_types(_tmp80),_tmp10E3)))),_tmp10E4)))),_tmp10E2))));}_LL2D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp81=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp74;if(_tmp81->tag != 5)goto _LL2F;else{_tmp82=_tmp81->f1;_tmp83=_tmp82.elt_typ;_tmp84=_tmp82.elt_tq;_tmp85=_tmp82.ptr_atts;_tmp86=_tmp85.rgn;_tmp87=_tmp85.nullable;_tmp88=_tmp85.bounds;_tmp89=_tmp85.zero_term;_tmp8A=_tmp85.ptrloc;}}_LL2E: {
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
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp10EE;struct Cyc_Absyn_PtrAtts _tmp10ED;struct Cyc_Absyn_PtrInfo _tmp10EC;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp10EB;return(void*)((_tmp10EB=_cycalloc(sizeof(*_tmp10EB)),((_tmp10EB[0]=((_tmp10EE.tag=5,((_tmp10EE.f1=((_tmp10EC.elt_typ=_tmpE6,((_tmp10EC.elt_tq=_tmpE9,((_tmp10EC.ptr_atts=((_tmp10ED.rgn=_tmpE7,((_tmp10ED.nullable=_tmpE8,((_tmp10ED.bounds=_tmpEA,((_tmp10ED.zero_term=_tmpEB,((_tmp10ED.ptrloc=_tmp8A,_tmp10ED)))))))))),_tmp10EC)))))),_tmp10EE)))),_tmp10EB))));}_LL2F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74;if(_tmp8B->tag != 6)goto _LL31;}_LL30:
# 261
 goto _LL32;_LL31: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp8C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74;if(_tmp8C->tag != 7)goto _LL33;}_LL32:
# 262
 return t;_LL33: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp74;if(_tmp8D->tag != 8)goto _LL35;else{_tmp8E=_tmp8D->f1;_tmp8F=_tmp8E.elt_type;_tmp90=_tmp8E.tq;_tmp91=_tmp8E.num_elts;_tmp92=_tmp8E.zero_term;_tmp93=_tmp8E.zt_loc;}}_LL34: {
# 265
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp10F4;struct Cyc_Absyn_ArrayInfo _tmp10F3;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp10F2;return(void*)((_tmp10F2=_cycalloc(sizeof(*_tmp10F2)),((_tmp10F2[0]=((_tmp10F4.tag=8,((_tmp10F4.f1=((_tmp10F3.elt_type=Cyc_Tcutil_copy_type(_tmp8F),((_tmp10F3.tq=_tmp90,((_tmp10F3.num_elts=_tmp91,((_tmp10F3.zero_term=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp92),((_tmp10F3.zt_loc=_tmp93,_tmp10F3)))))))))),_tmp10F4)))),_tmp10F2))));}_LL35: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp94=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp74;if(_tmp94->tag != 9)goto _LL37;else{_tmp95=_tmp94->f1;_tmp96=_tmp95.tvars;_tmp97=_tmp95.effect;_tmp98=_tmp95.ret_tqual;_tmp99=_tmp95.ret_typ;_tmp9A=_tmp95.args;_tmp9B=_tmp95.c_varargs;_tmp9C=_tmp95.cyc_varargs;_tmp9D=_tmp95.rgn_po;_tmp9E=_tmp95.attributes;}}_LL36: {
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
struct Cyc_Absyn_VarargInfo*_tmp10F5;cyc_varargs2=((_tmp10F5=_cycalloc(sizeof(*_tmp10F5)),((_tmp10F5->name=cv->name,((_tmp10F5->tq=cv->tq,((_tmp10F5->type=Cyc_Tcutil_copy_type(cv->type),((_tmp10F5->inject=cv->inject,_tmp10F5))))))))));}{
# 278
struct Cyc_List_List*_tmpF8=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp9D);
# 279
struct Cyc_List_List*_tmpF9=_tmp9E;
# 280
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp10FB;struct Cyc_Absyn_FnInfo _tmp10FA;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp10F9;return(void*)((_tmp10F9=_cycalloc(sizeof(*_tmp10F9)),((_tmp10F9[0]=((_tmp10FB.tag=9,((_tmp10FB.f1=((_tmp10FA.tvars=_tmpF3,((_tmp10FA.effect=effopt2,((_tmp10FA.ret_tqual=_tmp98,((_tmp10FA.ret_typ=_tmpF4,((_tmp10FA.args=_tmpF5,((_tmp10FA.c_varargs=_tmpF6,((_tmp10FA.cyc_varargs=cyc_varargs2,((_tmp10FA.rgn_po=_tmpF8,((_tmp10FA.attributes=_tmpF9,_tmp10FA)))))))))))))))))),_tmp10FB)))),_tmp10F9))));};}_LL37: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp9F=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp74;if(_tmp9F->tag != 10)goto _LL39;else{_tmpA0=_tmp9F->f1;}}_LL38: {
# 282
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp10FE;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp10FD;return(void*)((_tmp10FD=_cycalloc(sizeof(*_tmp10FD)),((_tmp10FD[0]=((_tmp10FE.tag=10,((_tmp10FE.f1=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpA0),_tmp10FE)))),_tmp10FD))));}_LL39: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpA1=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp74;if(_tmpA1->tag != 11)goto _LL3B;else{_tmpA2=_tmpA1->f1;_tmpA3=_tmpA2.aggr_info;if((_tmpA3.UnknownAggr).tag != 1)goto _LL3B;_tmpA4=(struct _tuple4)(_tmpA3.UnknownAggr).val;_tmpA5=_tmpA4.f1;_tmpA6=_tmpA4.f2;_tmpA7=_tmpA4.f3;_tmpA8=_tmpA2.targs;}}_LL3A: {
# 284
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp1104;struct Cyc_Absyn_AggrInfo _tmp1103;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1102;return(void*)((_tmp1102=_cycalloc(sizeof(*_tmp1102)),((_tmp1102[0]=((_tmp1104.tag=11,((_tmp1104.f1=((_tmp1103.aggr_info=Cyc_Absyn_UnknownAggr(_tmpA5,_tmpA6,_tmpA7),((_tmp1103.targs=Cyc_Tcutil_copy_types(_tmpA8),_tmp1103)))),_tmp1104)))),_tmp1102))));}_LL3B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpA9=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp74;if(_tmpA9->tag != 11)goto _LL3D;else{_tmpAA=_tmpA9->f1;_tmpAB=_tmpAA.aggr_info;if((_tmpAB.KnownAggr).tag != 2)goto _LL3D;_tmpAC=(struct Cyc_Absyn_Aggrdecl**)(_tmpAB.KnownAggr).val;_tmpAD=_tmpAA.targs;}}_LL3C: {
# 286
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp110A;struct Cyc_Absyn_AggrInfo _tmp1109;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1108;return(void*)((_tmp1108=_cycalloc(sizeof(*_tmp1108)),((_tmp1108[0]=((_tmp110A.tag=11,((_tmp110A.f1=((_tmp1109.aggr_info=Cyc_Absyn_KnownAggr(_tmpAC),((_tmp1109.targs=Cyc_Tcutil_copy_types(_tmpAD),_tmp1109)))),_tmp110A)))),_tmp1108))));}_LL3D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAE=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp74;if(_tmpAE->tag != 12)goto _LL3F;else{_tmpAF=_tmpAE->f1;_tmpB0=_tmpAE->f2;}}_LL3E: {
# 287
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp110D;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp110C;return(void*)((_tmp110C=_cycalloc(sizeof(*_tmp110C)),((_tmp110C[0]=((_tmp110D.tag=12,((_tmp110D.f1=_tmpAF,((_tmp110D.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpB0),_tmp110D)))))),_tmp110C))));}_LL3F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpB1=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp74;if(_tmpB1->tag != 13)goto _LL41;else{_tmpB2=_tmpB1->f1;_tmpB3=_tmpB1->f2;}}_LL40: {
# 288
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp1110;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp110F;return(void*)((_tmp110F=_cycalloc(sizeof(*_tmp110F)),((_tmp110F[0]=((_tmp1110.tag=13,((_tmp1110.f1=_tmpB2,((_tmp1110.f2=_tmpB3,_tmp1110)))))),_tmp110F))));}_LL41: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpB4=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp74;if(_tmpB4->tag != 14)goto _LL43;else{_tmpB5=_tmpB4->f1;}}_LL42: {
# 289
struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmp1113;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp1112;return(void*)((_tmp1112=_cycalloc(sizeof(*_tmp1112)),((_tmp1112[0]=((_tmp1113.tag=14,((_tmp1113.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpB5),_tmp1113)))),_tmp1112))));}_LL43: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB6=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp74;if(_tmpB6->tag != 19)goto _LL45;else{_tmpB7=(void*)_tmpB6->f1;}}_LL44: {
# 290
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp1116;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1115;return(void*)((_tmp1115=_cycalloc(sizeof(*_tmp1115)),((_tmp1115[0]=((_tmp1116.tag=19,((_tmp1116.f1=(void*)Cyc_Tcutil_copy_type(_tmpB7),_tmp1116)))),_tmp1115))));}_LL45: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpB8=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp74;if(_tmpB8->tag != 18)goto _LL47;else{_tmpB9=_tmpB8->f1;}}_LL46: {
# 293
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp1119;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1118;return(void*)((_tmp1118=_cycalloc(sizeof(*_tmp1118)),((_tmp1118[0]=((_tmp1119.tag=18,((_tmp1119.f1=_tmpB9,_tmp1119)))),_tmp1118))));}_LL47: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpBA=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp74;if(_tmpBA->tag != 15)goto _LL49;else{_tmpBB=(void*)_tmpBA->f1;}}_LL48: {
# 294
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp111C;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp111B;return(void*)((_tmp111B=_cycalloc(sizeof(*_tmp111B)),((_tmp111B[0]=((_tmp111C.tag=15,((_tmp111C.f1=(void*)Cyc_Tcutil_copy_type(_tmpBB),_tmp111C)))),_tmp111B))));}_LL49: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpBC=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp74;if(_tmpBC->tag != 16)goto _LL4B;else{_tmpBD=(void*)_tmpBC->f1;_tmpBE=(void*)_tmpBC->f2;}}_LL4A: {
# 295
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp111F;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp111E;return(void*)((_tmp111E=_cycalloc(sizeof(*_tmp111E)),((_tmp111E[0]=((_tmp111F.tag=16,((_tmp111F.f1=(void*)Cyc_Tcutil_copy_type(_tmpBD),((_tmp111F.f2=(void*)Cyc_Tcutil_copy_type(_tmpBE),_tmp111F)))))),_tmp111E))));}_LL4B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpBF=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp74;if(_tmpBF->tag != 17)goto _LL4D;else{_tmpC0=_tmpBF->f1;_tmpC1=_tmpBF->f2;_tmpC2=_tmpBF->f3;}}_LL4C: {
# 297
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp1122;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1121;return(void*)((_tmp1121=_cycalloc(sizeof(*_tmp1121)),((_tmp1121[0]=((_tmp1122.tag=17,((_tmp1122.f1=_tmpC0,((_tmp1122.f2=Cyc_Tcutil_copy_types(_tmpC1),((_tmp1122.f3=_tmpC2,((_tmp1122.f4=(void*)0,_tmp1122)))))))))),_tmp1121))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpC3=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp74;if(_tmpC3->tag != 21)goto _LL4F;}_LL4E:
# 298
 goto _LL50;_LL4F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpC4=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp74;if(_tmpC4->tag != 22)goto _LL51;}_LL50:
# 299
 goto _LL52;_LL51: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpC5=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp74;if(_tmpC5->tag != 20)goto _LL53;}_LL52:
# 300
 return t;_LL53: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpC6=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp74;if(_tmpC6->tag != 23)goto _LL55;else{_tmpC7=(void*)_tmpC6->f1;}}_LL54: {
# 301
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1125;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1124;return(void*)((_tmp1124=_cycalloc(sizeof(*_tmp1124)),((_tmp1124[0]=((_tmp1125.tag=23,((_tmp1125.f1=(void*)Cyc_Tcutil_copy_type(_tmpC7),_tmp1125)))),_tmp1124))));}_LL55: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC8=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp74;if(_tmpC8->tag != 24)goto _LL57;else{_tmpC9=_tmpC8->f1;}}_LL56: {
# 302
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1128;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1127;return(void*)((_tmp1127=_cycalloc(sizeof(*_tmp1127)),((_tmp1127[0]=((_tmp1128.tag=24,((_tmp1128.f1=Cyc_Tcutil_copy_types(_tmpC9),_tmp1128)))),_tmp1127))));}_LL57: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpCA=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp74;if(_tmpCA->tag != 25)goto _LL59;else{_tmpCB=(void*)_tmpCA->f1;}}_LL58: {
# 303
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp112B;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp112A;return(void*)((_tmp112A=_cycalloc(sizeof(*_tmp112A)),((_tmp112A[0]=((_tmp112B.tag=25,((_tmp112B.f1=(void*)Cyc_Tcutil_copy_type(_tmpCB),_tmp112B)))),_tmp112A))));}_LL59: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpCC=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp74;if(_tmpCC->tag != 26)goto _LL5B;else{_tmpCD=_tmpCC->f1;_tmpCE=*_tmpCD;_tmpCF=_tmpCE.r;{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpD0=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmpCF;if(_tmpD0->tag != 0)goto _LL5B;else{_tmpD1=_tmpD0->f1;}};}}_LL5A: {
# 306
struct Cyc_List_List*_tmp11B=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD1->tvs);
# 307
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp1131;struct Cyc_Absyn_AggrInfo _tmp1130;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp112F;return(void*)((_tmp112F=_cycalloc(sizeof(*_tmp112F)),((_tmp112F[0]=((_tmp1131.tag=11,((_tmp1131.f1=((_tmp1130.aggr_info=Cyc_Absyn_UnknownAggr(_tmpD1->kind,_tmpD1->name,0),((_tmp1130.targs=_tmp11B,_tmp1130)))),_tmp1131)))),_tmp112F))));}_LL5B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpD2=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp74;if(_tmpD2->tag != 26)goto _LL5D;else{_tmpD3=_tmpD2->f1;_tmpD4=*_tmpD3;_tmpD5=_tmpD4.r;{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpD6=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmpD5;if(_tmpD6->tag != 1)goto _LL5D;else{_tmpD7=_tmpD6->f1;}};}}_LL5C: {
# 309
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp1134;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp1133;return(void*)((_tmp1133=_cycalloc(sizeof(*_tmp1133)),((_tmp1133[0]=((_tmp1134.tag=13,((_tmp1134.f1=_tmpD7->name,((_tmp1134.f2=0,_tmp1134)))))),_tmp1133))));}_LL5D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpD8=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp74;if(_tmpD8->tag != 26)goto _LL22;else{_tmpD9=_tmpD8->f1;_tmpDA=*_tmpD9;_tmpDB=_tmpDA.r;{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpDC=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmpDB;if(_tmpDC->tag != 2)goto _LL22;else{_tmpDD=_tmpDC->f1;}};}}_LL5E: {
# 311
struct Cyc_List_List*_tmp121=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpDD->tvs);
# 312
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp113E;struct Cyc_Absyn_UnknownDatatypeInfo _tmp113D;struct Cyc_Absyn_DatatypeInfo _tmp113C;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp113B;return(void*)((_tmp113B=_cycalloc(sizeof(*_tmp113B)),((_tmp113B[0]=((_tmp113E.tag=3,((_tmp113E.f1=((_tmp113C.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmp113D.name=_tmpDD->name,((_tmp113D.is_extensible=0,_tmp113D))))),((_tmp113C.targs=_tmp121,_tmp113C)))),_tmp113E)))),_tmp113B))));}_LL22:;}
# 326 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(void*d){
# 327
void*_tmp126=d;struct Cyc_Absyn_Exp*_tmp128;struct _dyneither_ptr*_tmp12A;_LL60: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp127=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp126;if(_tmp127->tag != 0)goto _LL62;else{_tmp128=_tmp127->f1;}}_LL61: {
# 328
struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmp1141;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp1140;return(void*)((_tmp1140=_cycalloc(sizeof(*_tmp1140)),((_tmp1140[0]=((_tmp1141.tag=0,((_tmp1141.f1=Cyc_Tcutil_deep_copy_exp(_tmp128),_tmp1141)))),_tmp1140))));}_LL62: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp129=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp126;if(_tmp129->tag != 1)goto _LL5F;else{_tmp12A=_tmp129->f1;}}_LL63:
# 329
 return d;_LL5F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 332
static struct _tuple19*Cyc_Tcutil_copy_eds(struct _tuple19*e){
# 334
struct _tuple19*_tmp1142;return(_tmp1142=_cycalloc(sizeof(*_tmp1142)),((_tmp1142->f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_designator,(e[0]).f1),((_tmp1142->f2=Cyc_Tcutil_deep_copy_exp((e[0]).f2),_tmp1142)))));}
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
struct Cyc_Core_Opt*_tmp1143;return Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmp138),(unsigned int)_tmp139?(_tmp1143=_cycalloc_atomic(sizeof(*_tmp1143)),((_tmp1143->v=(void*)((enum Cyc_Absyn_Primop)_tmp139->v),_tmp1143))): 0,Cyc_Tcutil_deep_copy_exp(_tmp13A),e->loc);}_LL6D: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp13B=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp13B->tag != 4)goto _LL6F;else{_tmp13C=_tmp13B->f1;_tmp13D=_tmp13B->f2;}}_LL6E:
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
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp114D;struct Cyc_Absyn_VarargInfo*_tmp114C;struct Cyc_Absyn_VarargCallInfo*_tmp114B;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp114A;return Cyc_Absyn_new_exp((void*)((_tmp114A=_cycalloc(sizeof(*_tmp114A)),((_tmp114A[0]=((_tmp114D.tag=9,((_tmp114D.f1=
# 355
Cyc_Tcutil_deep_copy_exp(_tmp14C),((_tmp114D.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp14D),((_tmp114D.f3=(
# 356
(_tmp114B=_cycalloc(sizeof(*_tmp114B)),((_tmp114B->num_varargs=_tmp1B4,((_tmp114B->injectors=_tmp1B5,((_tmp114B->vai=(
# 357
(_tmp114C=_cycalloc(sizeof(*_tmp114C)),((_tmp114C->name=_tmp1B9,((_tmp114C->tq=_tmp1BA,((_tmp114C->type=Cyc_Tcutil_copy_type(_tmp1BB),((_tmp114C->inject=_tmp1BC,_tmp114C)))))))))),_tmp114B)))))))),((_tmp114D.f4=_tmp14F,_tmp114D)))))))))),_tmp114A)))),e->loc);};}_LLB6:;_LLB7: {
# 360
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp1150;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp114F;return Cyc_Absyn_new_exp((void*)((_tmp114F=_cycalloc(sizeof(*_tmp114F)),((_tmp114F[0]=((_tmp1150.tag=9,((_tmp1150.f1=Cyc_Tcutil_deep_copy_exp(_tmp14C),((_tmp1150.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp14D),((_tmp1150.f3=_tmp14E,((_tmp1150.f4=_tmp14F,_tmp1150)))))))))),_tmp114F)))),e->loc);}_LLB3:;}_LL79: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp150=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp150->tag != 10)goto _LL7B;else{_tmp151=_tmp150->f1;}}_LL7A:
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
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp1153;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp1152;return Cyc_Absyn_new_exp((void*)((_tmp1152=_cycalloc(sizeof(*_tmp1152)),((_tmp1152[0]=((_tmp1153.tag=20,((_tmp1153.f1=Cyc_Tcutil_deep_copy_exp(_tmp16B),((_tmp1153.f2=_tmp16C,((_tmp1153.f3=_tmp16D,((_tmp1153.f4=_tmp16E,_tmp1153)))))))))),_tmp1152)))),e->loc);}_LL8F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp16F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp16F->tag != 21)goto _LL91;else{_tmp170=_tmp16F->f1;_tmp171=_tmp16F->f2;_tmp172=_tmp16F->f3;_tmp173=_tmp16F->f4;}}_LL90: {
# 380
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp1156;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp1155;return Cyc_Absyn_new_exp((void*)((_tmp1155=_cycalloc(sizeof(*_tmp1155)),((_tmp1155[0]=((_tmp1156.tag=21,((_tmp1156.f1=Cyc_Tcutil_deep_copy_exp(_tmp170),((_tmp1156.f2=_tmp171,((_tmp1156.f3=_tmp172,((_tmp1156.f4=_tmp173,_tmp1156)))))))))),_tmp1155)))),e->loc);}_LL91: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp174=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp174->tag != 22)goto _LL93;else{_tmp175=_tmp174->f1;_tmp176=_tmp174->f2;}}_LL92:
# 381
 return Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmp175),Cyc_Tcutil_deep_copy_exp(_tmp176),e->loc);_LL93: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp177=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp177->tag != 23)goto _LL95;else{_tmp178=_tmp177->f1;}}_LL94:
# 382
 return Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp178),e->loc);_LL95: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp179=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp179->tag != 24)goto _LL97;else{_tmp17A=_tmp179->f1;_tmp17B=*_tmp17A;_tmp17C=_tmp17B.f1;_tmp17D=_tmp17B.f2;_tmp17E=_tmp17B.f3;_tmp17F=_tmp179->f2;}}_LL96: {
# 384
struct _dyneither_ptr*vopt1=_tmp17C;
# 385
if(_tmp17C != 0)vopt1=_tmp17C;{
# 386
struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmp115C;struct _tuple10*_tmp115B;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp115A;return Cyc_Absyn_new_exp((void*)((_tmp115A=_cycalloc(sizeof(*_tmp115A)),((_tmp115A[0]=((_tmp115C.tag=24,((_tmp115C.f1=((_tmp115B=_cycalloc(sizeof(*_tmp115B)),((_tmp115B->f1=vopt1,((_tmp115B->f2=_tmp17D,((_tmp115B->f3=Cyc_Tcutil_copy_type(_tmp17E),_tmp115B)))))))),((_tmp115C.f2=
# 387
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp17F),_tmp115C)))))),_tmp115A)))),e->loc);};}_LL97: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp180=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp180->tag != 25)goto _LL99;else{_tmp181=_tmp180->f1;}}_LL98: {
# 388
struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp115F;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp115E;return Cyc_Absyn_new_exp((void*)((_tmp115E=_cycalloc(sizeof(*_tmp115E)),((_tmp115E[0]=((_tmp115F.tag=25,((_tmp115F.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp181),_tmp115F)))),_tmp115E)))),e->loc);}_LL99: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp182=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp182->tag != 26)goto _LL9B;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;_tmp185=_tmp182->f3;_tmp186=_tmp182->f4;}}_LL9A: {
# 390
struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmp1162;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp1161;return Cyc_Absyn_new_exp((void*)((_tmp1161=_cycalloc(sizeof(*_tmp1161)),((_tmp1161[0]=((_tmp1162.tag=26,((_tmp1162.f1=_tmp183,((_tmp1162.f2=Cyc_Tcutil_deep_copy_exp(_tmp184),((_tmp1162.f3=Cyc_Tcutil_deep_copy_exp(_tmp185),((_tmp1162.f4=_tmp186,_tmp1162)))))))))),_tmp1161)))),e->loc);}_LL9B: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp187=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp187->tag != 27)goto _LL9D;else{_tmp188=_tmp187->f1;_tmp189=_tmp187->f2;_tmp18A=_tmp187->f3;_tmp18B=_tmp187->f4;}}_LL9C: {
# 392
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp1165;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1164;return Cyc_Absyn_new_exp((void*)((_tmp1164=_cycalloc(sizeof(*_tmp1164)),((_tmp1164[0]=((_tmp1165.tag=27,((_tmp1165.f1=_tmp188,((_tmp1165.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp189),((_tmp1165.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp18A),((_tmp1165.f4=_tmp18B,_tmp1165)))))))))),_tmp1164)))),e->loc);}_LL9D: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp18C=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp18C->tag != 28)goto _LL9F;else{_tmp18D=(void*)_tmp18C->f1;_tmp18E=_tmp18C->f2;}}_LL9E: {
# 395
struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp1168;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp1167;return Cyc_Absyn_new_exp((void*)((_tmp1167=_cycalloc(sizeof(*_tmp1167)),((_tmp1167[0]=((_tmp1168.tag=28,((_tmp1168.f1=(void*)Cyc_Tcutil_copy_type(_tmp18D),((_tmp1168.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp18E),_tmp1168)))))),_tmp1167)))),e->loc);}_LL9F: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp18F=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp18F->tag != 29)goto _LLA1;else{_tmp190=_tmp18F->f1;_tmp191=_tmp18F->f2;_tmp192=_tmp18F->f3;}}_LLA0: {
# 397
struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp116B;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp116A;return Cyc_Absyn_new_exp((void*)((_tmp116A=_cycalloc(sizeof(*_tmp116A)),((_tmp116A[0]=((_tmp116B.tag=29,((_tmp116B.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp190),((_tmp116B.f2=_tmp191,((_tmp116B.f3=_tmp192,_tmp116B)))))))),_tmp116A)))),e->loc);}_LLA1: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp193=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp193->tag != 30)goto _LLA3;else{_tmp194=_tmp193->f1;_tmp195=_tmp193->f2;_tmp196=_tmp193->f3;}}_LLA2:
# 398
 return e;_LLA3: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp197=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp197->tag != 31)goto _LLA5;else{_tmp198=_tmp197->f1;_tmp199=(void*)_tmp197->f2;_tmp19A=_tmp197->f3;}}_LLA4: {
# 400
struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp116E;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp116D;return Cyc_Absyn_new_exp((void*)((_tmp116D=_cycalloc(sizeof(*_tmp116D)),((_tmp116D[0]=((_tmp116E.tag=31,((_tmp116E.f1=_tmp198,((_tmp116E.f2=(void*)Cyc_Tcutil_copy_type(_tmp199),((_tmp116E.f3=_tmp19A,_tmp116E)))))))),_tmp116D)))),e->loc);}_LLA5: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp19B=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp19B->tag != 32)goto _LLA7;else{_tmp19C=_tmp19B->f1;_tmp19D=_tmp19C.is_calloc;_tmp19E=_tmp19C.rgn;_tmp19F=_tmp19C.elt_type;_tmp1A0=_tmp19C.num_elts;_tmp1A1=_tmp19C.fat_result;}}_LLA6: {
# 402
struct Cyc_Absyn_Exp*_tmp1D6=Cyc_Absyn_copy_exp(e);
# 403
struct Cyc_Absyn_Exp*r1=_tmp19E;if(_tmp19E != 0)r1=(struct Cyc_Absyn_Exp*)Cyc_Tcutil_deep_copy_exp((struct Cyc_Absyn_Exp*)_tmp19E);{
# 404
void**t1=_tmp19F;if(_tmp19F != 0){void**_tmp116F;t1=((_tmp116F=_cycalloc(sizeof(*_tmp116F)),((_tmp116F[0]=Cyc_Tcutil_copy_type(*_tmp19F),_tmp116F))));}
# 405
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp1175;struct Cyc_Absyn_MallocInfo _tmp1174;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp1173;_tmp1D6->r=(void*)((_tmp1173=_cycalloc(sizeof(*_tmp1173)),((_tmp1173[0]=((_tmp1175.tag=32,((_tmp1175.f1=((_tmp1174.is_calloc=_tmp19D,((_tmp1174.rgn=r1,((_tmp1174.elt_type=t1,((_tmp1174.num_elts=_tmp1A0,((_tmp1174.fat_result=_tmp1A1,_tmp1174)))))))))),_tmp1175)))),_tmp1173))));}
# 406
return _tmp1D6;};}_LLA7: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp1A2=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp1A2->tag != 33)goto _LLA9;else{_tmp1A3=_tmp1A2->f1;_tmp1A4=_tmp1A2->f2;}}_LLA8:
# 407
 return Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp1A3),Cyc_Tcutil_deep_copy_exp(_tmp1A4),e->loc);_LLA9: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp1A5=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp1A5->tag != 34)goto _LLAB;else{_tmp1A6=_tmp1A5->f1;_tmp1A7=_tmp1A5->f2;}}_LLAA: {
# 409
struct Cyc_Core_Opt*nopt1=_tmp1A6;
# 410
if(_tmp1A6 != 0){struct Cyc_Core_Opt*_tmp1176;nopt1=((_tmp1176=_cycalloc(sizeof(*_tmp1176)),((_tmp1176->v=(struct _tuple2*)_tmp1A6->v,_tmp1176))));}{
# 411
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp1179;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp1178;return Cyc_Absyn_new_exp((void*)((_tmp1178=_cycalloc(sizeof(*_tmp1178)),((_tmp1178[0]=((_tmp1179.tag=34,((_tmp1179.f1=nopt1,((_tmp1179.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp1A7),_tmp1179)))))),_tmp1178)))),e->loc);};}_LLAB: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp1A8=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp1A8->tag != 35)goto _LLAD;}_LLAC: {
# 413
struct Cyc_Core_Failure_exn_struct _tmp117F;const char*_tmp117E;struct Cyc_Core_Failure_exn_struct*_tmp117D;(int)_throw((void*)((_tmp117D=_cycalloc(sizeof(*_tmp117D)),((_tmp117D[0]=((_tmp117F.tag=Cyc_Core_Failure,((_tmp117F.f1=((_tmp117E="deep_copy: statement expressions unsupported",_tag_dyneither(_tmp117E,sizeof(char),45))),_tmp117F)))),_tmp117D)))));}_LLAD: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp1A9=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp1A9->tag != 36)goto _LLAF;else{_tmp1AA=_tmp1A9->f1;_tmp1AB=_tmp1A9->f2;}}_LLAE: {
# 414
struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmp1182;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp1181;return Cyc_Absyn_new_exp((void*)((_tmp1181=_cycalloc(sizeof(*_tmp1181)),((_tmp1181[0]=((_tmp1182.tag=36,((_tmp1182.f1=Cyc_Tcutil_deep_copy_exp(_tmp1AA),((_tmp1182.f2=_tmp1AB,_tmp1182)))))),_tmp1181)))),e->loc);}_LLAF: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1AC=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp12E;if(_tmp1AC->tag != 37)goto _LLB1;else{_tmp1AD=(void*)_tmp1AC->f1;}}_LLB0:
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
struct _tuple20 _tmp1183;struct _tuple20 _tmp1EC=(_tmp1183.f1=_tmp1E5,((_tmp1183.f2=_tmp1E9,_tmp1183)));enum Cyc_Absyn_KindQual _tmp1ED;enum Cyc_Absyn_KindQual _tmp1EE;enum Cyc_Absyn_KindQual _tmp1EF;enum Cyc_Absyn_KindQual _tmp1F0;enum Cyc_Absyn_KindQual _tmp1F1;enum Cyc_Absyn_KindQual _tmp1F2;_LLB9: _tmp1ED=_tmp1EC.f1;if(_tmp1ED != Cyc_Absyn_BoxKind)goto _LLBB;_tmp1EE=_tmp1EC.f2;if(_tmp1EE != Cyc_Absyn_MemKind)goto _LLBB;_LLBA:
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
struct _tuple21 _tmp1184;struct _tuple21 _tmp1F4=(_tmp1184.f1=_tmp1E6,((_tmp1184.f2=_tmp1EA,_tmp1184)));enum Cyc_Absyn_AliasQual _tmp1F5;enum Cyc_Absyn_AliasQual _tmp1F6;enum Cyc_Absyn_AliasQual _tmp1F7;enum Cyc_Absyn_AliasQual _tmp1F8;_LLC2: _tmp1F5=_tmp1F4.f1;if(_tmp1F5 != Cyc_Absyn_Aliasable)goto _LLC4;_tmp1F6=_tmp1F4.f2;if(_tmp1F6 != Cyc_Absyn_Top)goto _LLC4;_LLC3:
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
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1187;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1186;tv->kind=(void*)((_tmp1186=_cycalloc(sizeof(*_tmp1186)),((_tmp1186[0]=((_tmp1187.tag=2,((_tmp1187.f1=0,((_tmp1187.f2=def,_tmp1187)))))),_tmp1186))));}
# 457
return def;_LLC8:;}
# 461
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
# 462
struct _tuple0 _tmp1188;struct _tuple0 _tmp201=(_tmp1188.f1=Cyc_Absyn_compress_kb(kb1),((_tmp1188.f2=Cyc_Absyn_compress_kb(kb2),_tmp1188)));void*_tmp202;struct Cyc_Absyn_Kind*_tmp204;void*_tmp205;struct Cyc_Absyn_Kind*_tmp207;void*_tmp208;struct Cyc_Core_Opt*_tmp20A;struct Cyc_Core_Opt**_tmp20B;struct Cyc_Absyn_Kind*_tmp20C;void*_tmp20D;struct Cyc_Absyn_Kind*_tmp20F;void*_tmp210;struct Cyc_Absyn_Kind*_tmp212;void*_tmp213;struct Cyc_Core_Opt*_tmp215;struct Cyc_Core_Opt**_tmp216;struct Cyc_Absyn_Kind*_tmp217;void*_tmp218;struct Cyc_Core_Opt*_tmp21A;struct Cyc_Core_Opt**_tmp21B;struct Cyc_Absyn_Kind*_tmp21C;void*_tmp21D;struct Cyc_Core_Opt*_tmp21F;struct Cyc_Core_Opt**_tmp220;struct Cyc_Absyn_Kind*_tmp221;void*_tmp222;struct Cyc_Core_Opt*_tmp224;struct Cyc_Core_Opt**_tmp225;void*_tmp226;void*_tmp227;void*_tmp228;struct Cyc_Core_Opt*_tmp22A;struct Cyc_Core_Opt**_tmp22B;_LLD0: _tmp202=_tmp201.f1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp203=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp202;if(_tmp203->tag != 0)goto _LLD2;else{_tmp204=_tmp203->f1;}};_tmp205=_tmp201.f2;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp206=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp205;if(_tmp206->tag != 0)goto _LLD2;else{_tmp207=_tmp206->f1;}};_LLD1:
# 463
 return _tmp204 == _tmp207;_LLD2: _tmp208=_tmp201.f1;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp209=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp208;if(_tmp209->tag != 2)goto _LLD4;else{_tmp20A=_tmp209->f1;_tmp20B=(struct Cyc_Core_Opt**)& _tmp209->f1;_tmp20C=_tmp209->f2;}};_tmp20D=_tmp201.f2;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp20E=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp20D;if(_tmp20E->tag != 0)goto _LLD4;else{_tmp20F=_tmp20E->f1;}};_LLD3:
# 465
 if(Cyc_Tcutil_kind_leq(_tmp20F,_tmp20C)){
# 466
{struct Cyc_Core_Opt*_tmp1189;*_tmp20B=((_tmp1189=_cycalloc(sizeof(*_tmp1189)),((_tmp1189->v=kb2,_tmp1189))));}
# 467
return 1;}else{
# 468
return 0;}_LLD4: _tmp210=_tmp201.f1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp211=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp210;if(_tmp211->tag != 0)goto _LLD6;else{_tmp212=_tmp211->f1;}};_tmp213=_tmp201.f2;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp214=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp213;if(_tmp214->tag != 2)goto _LLD6;else{_tmp215=_tmp214->f1;_tmp216=(struct Cyc_Core_Opt**)& _tmp214->f1;_tmp217=_tmp214->f2;}};_LLD5:
# 470
 if(Cyc_Tcutil_kind_leq(_tmp212,_tmp217)){
# 471
{struct Cyc_Core_Opt*_tmp118A;*_tmp216=((_tmp118A=_cycalloc(sizeof(*_tmp118A)),((_tmp118A->v=kb1,_tmp118A))));}
# 472
return 1;}else{
# 473
return 0;}_LLD6: _tmp218=_tmp201.f1;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp219=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp218;if(_tmp219->tag != 2)goto _LLD8;else{_tmp21A=_tmp219->f1;_tmp21B=(struct Cyc_Core_Opt**)& _tmp219->f1;_tmp21C=_tmp219->f2;}};_tmp21D=_tmp201.f2;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp21E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp21D;if(_tmp21E->tag != 2)goto _LLD8;else{_tmp21F=_tmp21E->f1;_tmp220=(struct Cyc_Core_Opt**)& _tmp21E->f1;_tmp221=_tmp21E->f2;}};_LLD7:
# 475
 if(Cyc_Tcutil_kind_leq(_tmp21C,_tmp221)){
# 476
{struct Cyc_Core_Opt*_tmp118B;*_tmp220=((_tmp118B=_cycalloc(sizeof(*_tmp118B)),((_tmp118B->v=kb1,_tmp118B))));}
# 477
return 1;}else{
# 478
if(Cyc_Tcutil_kind_leq(_tmp221,_tmp21C)){
# 479
{struct Cyc_Core_Opt*_tmp118C;*_tmp21B=((_tmp118C=_cycalloc(sizeof(*_tmp118C)),((_tmp118C->v=kb2,_tmp118C))));}
# 480
return 1;}else{
# 481
return 0;}}_LLD8: _tmp222=_tmp201.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp223=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp222;if(_tmp223->tag != 1)goto _LLDA;else{_tmp224=_tmp223->f1;_tmp225=(struct Cyc_Core_Opt**)& _tmp223->f1;}};_tmp226=_tmp201.f2;_LLD9:
# 482
 _tmp227=_tmp226;_tmp22B=_tmp225;goto _LLDB;_LLDA: _tmp227=_tmp201.f1;_tmp228=_tmp201.f2;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp229=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp228;if(_tmp229->tag != 1)goto _LLCF;else{_tmp22A=_tmp229->f1;_tmp22B=(struct Cyc_Core_Opt**)& _tmp229->f1;}};_LLDB:
# 484
{struct Cyc_Core_Opt*_tmp118D;*_tmp22B=((_tmp118D=_cycalloc(sizeof(*_tmp118D)),((_tmp118D->v=_tmp227,_tmp118D))));}
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
struct _tuple16 _tmp118E;return(_tmp118E.f1=_tmp233,((_tmp118E.f2=_tmp234,_tmp118E)));};}_LLDF:;_LLE0: {
# 496
const char*_tmp1192;void*_tmp1191[1];struct Cyc_String_pa_PrintArg_struct _tmp1190;(_tmp1190.tag=0,((_tmp1190.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1191[0]=& _tmp1190,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1192="swap_kind: cannot update the kind of %s",_tag_dyneither(_tmp1192,sizeof(char),40))),_tag_dyneither(_tmp1191,sizeof(void*),1)))))));}_LLDC:;}
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
const char*_tmp1195;void*_tmp1194;(_tmp1194=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1195="typ_kind: Unresolved DatatypeFieldType",_tag_dyneither(_tmp1195,sizeof(char),39))),_tag_dyneither(_tmp1194,sizeof(void*),0)));}_LL102: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp255=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp239;if(_tmp255->tag != 11)goto _LL104;else{_tmp256=_tmp255->f1;_tmp257=_tmp256.aggr_info;if((_tmp257.UnknownAggr).tag != 1)goto _LL104;_tmp258=(struct _tuple4)(_tmp257.UnknownAggr).val;}}_LL103:
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
{struct _tuple16*_tmp1198;struct Cyc_List_List*_tmp1197;_tmp282=((_tmp1197=_region_malloc(temp,sizeof(*_tmp1197)),((_tmp1197->hd=((_tmp1198=_region_malloc(temp,sizeof(*_tmp1198)),((_tmp1198->f1=_tmp283,((_tmp1198->f2=_tmp284,_tmp1198)))))),((_tmp1197->tl=_tmp282,_tmp1197))))));}goto _LL120;_LL123:;_LL124:
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
const char*_tmp119C;void*_tmp119B[1];struct Cyc_String_pa_PrintArg_struct _tmp119A;(_tmp119A.tag=0,((_tmp119A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp119B[0]=& _tmp119A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp119C="typ_kind: typedef found: %s",_tag_dyneither(_tmp119C,sizeof(char),28))),_tag_dyneither(_tmp119B,sizeof(void*),1)))))));}
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
{const char*_tmp119D;Cyc_Tcutil_failure_reason=((_tmp119D="(type variable would escape scope)",_tag_dyneither(_tmp119D,sizeof(char),35)));}
# 616
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 618
goto _LL137;_LL13A: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp29B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp298;if(_tmp29B->tag != 1)goto _LL13C;else{_tmp29C=(void*)_tmp29B->f2;_tmp29D=_tmp29B->f4;_tmp29E=(struct Cyc_Core_Opt**)& _tmp29B->f4;}}_LL13B:
# 620
 if(t == evar){
# 621
{const char*_tmp119E;Cyc_Tcutil_failure_reason=((_tmp119E="(occurs check)",_tag_dyneither(_tmp119E,sizeof(char),15)));}
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
struct Cyc_List_List*_tmp119F;_tmp2CA=((_tmp119F=_cycalloc(sizeof(*_tmp119F)),((_tmp119F->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmp119F->tl=_tmp2CA,_tmp119F))))));}}}{
# 640
struct Cyc_Core_Opt*_tmp11A0;*_tmp29E=((_tmp11A0=_cycalloc(sizeof(*_tmp11A0)),((_tmp11A0->v=_tmp2CA,_tmp11A0))));};}}}
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
const char*_tmp11A3;void*_tmp11A2;(_tmp11A2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11A3="tq1 real_const not set.",_tag_dyneither(_tmp11A3,sizeof(char),24))),_tag_dyneither(_tmp11A2,sizeof(void*),0)));}
# 703
if(tq2.print_const  && !tq2.real_const){
# 704
const char*_tmp11A6;void*_tmp11A5;(_tmp11A5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11A6="tq2 real_const not set.",_tag_dyneither(_tmp11A6,sizeof(char),24))),_tag_dyneither(_tmp11A5,sizeof(void*),0)));}
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
{const char*_tmp11A7;Cyc_Tcutil_failure_reason=((_tmp11A7="(qualifiers don't match)",_tag_dyneither(_tmp11A7,sizeof(char),25)));}
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
{union Cyc_Absyn_Constraint _tmp11A8;*x=(((_tmp11A8.Forward_constr).val=y,(((_tmp11A8.Forward_constr).tag=2,_tmp11A8))));}return;_LL15D: _tmp2D9=*_tmp2D6;if((_tmp2D9.Eq_constr).tag != 1)goto _LL15F;_tmp2DA=(void*)(_tmp2D9.Eq_constr).val;_LL15E: {
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
const char*_tmp11AB;void*_tmp11AA;(_tmp11AA=0,Cyc_Tcutil_impos(((_tmp11AB="unify_conref: forward after compress(2)",_tag_dyneither(_tmp11AB,sizeof(char),40))),_tag_dyneither(_tmp11AA,sizeof(void*),0)));}_LL161:;}_LL15F: _tmp2DB=*_tmp2D6;if((_tmp2DB.Forward_constr).tag != 2)goto _LL15A;_tmp2DC=(union Cyc_Absyn_Constraint*)(_tmp2DB.Forward_constr).val;_LL160: {
# 747
const char*_tmp11AE;void*_tmp11AD;(_tmp11AD=0,Cyc_Tcutil_impos(((_tmp11AE="unify_conref: forward after compress",_tag_dyneither(_tmp11AE,sizeof(char),37))),_tag_dyneither(_tmp11AD,sizeof(void*),0)));}_LL15A:;};}
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
struct _tuple0 _tmp11AF;struct _tuple0 _tmp2F1=(_tmp11AF.f1=b1,((_tmp11AF.f2=b2,_tmp11AF)));void*_tmp2F2;void*_tmp2F4;void*_tmp2F6;void*_tmp2F8;void*_tmp2FA;struct Cyc_Absyn_Exp*_tmp2FC;void*_tmp2FD;struct Cyc_Absyn_Exp*_tmp2FF;_LL16E: _tmp2F2=_tmp2F1.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2F3=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2F2;if(_tmp2F3->tag != 0)goto _LL170;};_tmp2F4=_tmp2F1.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2F5=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2F4;if(_tmp2F5->tag != 0)goto _LL170;};_LL16F:
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
struct _tuple0 _tmp11B0;struct _tuple0 _tmp301=(_tmp11B0.f1=b1,((_tmp11B0.f2=b2,_tmp11B0)));void*_tmp302;void*_tmp304;void*_tmp306;void*_tmp308;void*_tmp30A;struct Cyc_Absyn_Exp*_tmp30C;void*_tmp30D;struct Cyc_Absyn_Exp*_tmp30F;_LL177: _tmp302=_tmp301.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp303=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp302;if(_tmp303->tag != 0)goto _LL179;};_tmp304=_tmp301.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp305=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp304;if(_tmp305->tag != 0)goto _LL179;};_LL178:
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
struct _tuple0 _tmp11B1;struct _tuple0 _tmp327=(_tmp11B1.f1=att1,((_tmp11B1.f2=att2,_tmp11B1)));void*_tmp328;int _tmp32A;void*_tmp32B;int _tmp32D;void*_tmp32E;int _tmp330;void*_tmp331;int _tmp333;void*_tmp334;int _tmp336;void*_tmp337;int _tmp339;void*_tmp33A;struct _dyneither_ptr _tmp33C;void*_tmp33D;struct _dyneither_ptr _tmp33F;void*_tmp340;enum Cyc_Absyn_Format_Type _tmp342;int _tmp343;int _tmp344;void*_tmp345;enum Cyc_Absyn_Format_Type _tmp347;int _tmp348;int _tmp349;_LL1AD: _tmp328=_tmp327.f1;{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp329=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp328;if(_tmp329->tag != 0)goto _LL1AF;else{_tmp32A=_tmp329->f1;}};_tmp32B=_tmp327.f2;{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp32C=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp32B;if(_tmp32C->tag != 0)goto _LL1AF;else{_tmp32D=_tmp32C->f1;}};_LL1AE:
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
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp11B4;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp11B3;t=(void*)((_tmp11B3=_cycalloc(sizeof(*_tmp11B3)),((_tmp11B3[0]=((_tmp11B4.tag=18,((_tmp11B4.f1=Cyc_Absyn_uint_exp(0,0),_tmp11B4)))),_tmp11B3))));}goto _LL1B9;_LL1C2:;_LL1C3:
# 847
 t=Cyc_Absyn_sint_typ;goto _LL1B9;_LL1B9:;}{
# 849
struct _tuple16*_tmp11B5;return(_tmp11B5=_cycalloc(sizeof(*_tmp11B5)),((_tmp11B5->f1=tv,((_tmp11B5->f2=t,_tmp11B5)))));};}
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
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11B8;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11B7;return(void*)((_tmp11B7=_cycalloc(sizeof(*_tmp11B7)),((_tmp11B7[0]=((_tmp11B8.tag=23,((_tmp11B8.f1=(void*)t,_tmp11B8)))),_tmp11B7))));}_LL1FE: _tmp394=*_tmp391;_tmp395=_tmp394.kind;if(_tmp395 != Cyc_Absyn_EffKind)goto _LL200;_LL1FF:
# 868
 return t;_LL200: _tmp396=*_tmp391;_tmp397=_tmp396.kind;if(_tmp397 != Cyc_Absyn_IntKind)goto _LL202;_LL201:
# 869
 return Cyc_Absyn_empty_effect;_LL202:;_LL203: {
# 870
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp11BB;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp11BA;return(void*)((_tmp11BA=_cycalloc(sizeof(*_tmp11BA)),((_tmp11BA[0]=((_tmp11BB.tag=25,((_tmp11BB.f1=(void*)t,_tmp11BB)))),_tmp11BA))));}_LL1FB:;}_LL1D5: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp363=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp35A;if(_tmp363->tag != 15)goto _LL1D7;else{_tmp364=(void*)_tmp363->f1;}}_LL1D6: {
# 872
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11BE;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11BD;return(void*)((_tmp11BD=_cycalloc(sizeof(*_tmp11BD)),((_tmp11BD[0]=((_tmp11BE.tag=23,((_tmp11BE.f1=(void*)_tmp364,_tmp11BE)))),_tmp11BD))));}_LL1D7: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp365=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp35A;if(_tmp365->tag != 16)goto _LL1D9;else{_tmp366=(void*)_tmp365->f1;_tmp367=(void*)_tmp365->f2;}}_LL1D8: {
# 876
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11C1;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11C0;return(void*)((_tmp11C0=_cycalloc(sizeof(*_tmp11C0)),((_tmp11C0[0]=((_tmp11C1.tag=23,((_tmp11C1.f1=(void*)_tmp367,_tmp11C1)))),_tmp11C0))));}_LL1D9: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp368=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp35A;if(_tmp368->tag != 3)goto _LL1DB;else{_tmp369=_tmp368->f1;_tmp36A=_tmp369.targs;}}_LL1DA: {
# 878
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp36A);
# 879
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11C4;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11C3;return Cyc_Tcutil_normalize_effect((void*)((_tmp11C3=_cycalloc(sizeof(*_tmp11C3)),((_tmp11C3[0]=((_tmp11C4.tag=24,((_tmp11C4.f1=ts,_tmp11C4)))),_tmp11C3)))));}_LL1DB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp36B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35A;if(_tmp36B->tag != 5)goto _LL1DD;else{_tmp36C=_tmp36B->f1;_tmp36D=_tmp36C.elt_typ;_tmp36E=_tmp36C.ptr_atts;_tmp36F=_tmp36E.rgn;}}_LL1DC: {
# 882
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11D3;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11D2;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11D1;void*_tmp11D0[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11CF;return Cyc_Tcutil_normalize_effect((void*)((_tmp11CF=_cycalloc(sizeof(*_tmp11CF)),((_tmp11CF[0]=((_tmp11D3.tag=24,((_tmp11D3.f1=((_tmp11D0[1]=Cyc_Tcutil_rgns_of(_tmp36D),((_tmp11D0[0]=(void*)((_tmp11D2=_cycalloc(sizeof(*_tmp11D2)),((_tmp11D2[0]=((_tmp11D1.tag=23,((_tmp11D1.f1=(void*)_tmp36F,_tmp11D1)))),_tmp11D2)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp11D0,sizeof(void*),2)))))),_tmp11D3)))),_tmp11CF)))));}_LL1DD: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp370=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp35A;if(_tmp370->tag != 8)goto _LL1DF;else{_tmp371=_tmp370->f1;_tmp372=_tmp371.elt_type;}}_LL1DE:
# 884
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp372));_LL1DF: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp373=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp35A;if(_tmp373->tag != 10)goto _LL1E1;else{_tmp374=_tmp373->f1;}}_LL1E0: {
# 886
struct Cyc_List_List*_tmp3A7=0;
# 887
for(0;_tmp374 != 0;_tmp374=_tmp374->tl){
# 888
struct Cyc_List_List*_tmp11D4;_tmp3A7=((_tmp11D4=_cycalloc(sizeof(*_tmp11D4)),((_tmp11D4->hd=(*((struct _tuple12*)_tmp374->hd)).f2,((_tmp11D4->tl=_tmp3A7,_tmp11D4))))));}
# 889
_tmp377=_tmp3A7;goto _LL1E2;}_LL1E1: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp375=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp35A;if(_tmp375->tag != 4)goto _LL1E3;else{_tmp376=_tmp375->f1;_tmp377=_tmp376.targs;}}_LL1E2:
# 890
 _tmp37A=_tmp377;goto _LL1E4;_LL1E3: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp378=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp35A;if(_tmp378->tag != 11)goto _LL1E5;else{_tmp379=_tmp378->f1;_tmp37A=_tmp379.targs;}}_LL1E4: {
# 892
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11D7;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11D6;return Cyc_Tcutil_normalize_effect((void*)((_tmp11D6=_cycalloc(sizeof(*_tmp11D6)),((_tmp11D6[0]=((_tmp11D7.tag=24,((_tmp11D7.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp37A),_tmp11D7)))),_tmp11D6)))));}_LL1E5: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp37B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp35A;if(_tmp37B->tag != 12)goto _LL1E7;else{_tmp37C=_tmp37B->f2;}}_LL1E6: {
# 894
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11DA;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11D9;return Cyc_Tcutil_normalize_effect((void*)((_tmp11D9=_cycalloc(sizeof(*_tmp11D9)),((_tmp11D9[0]=((_tmp11DA.tag=24,((_tmp11DA.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp37C),_tmp11DA)))),_tmp11D9)))));}_LL1E7: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp37D=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp35A;if(_tmp37D->tag != 19)goto _LL1E9;}_LL1E8:
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
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11DD;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11DC;return Cyc_Tcutil_normalize_effect((void*)((_tmp11DC=_cycalloc(sizeof(*_tmp11DC)),((_tmp11DC[0]=((_tmp11DD.tag=24,((_tmp11DD.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp38F),_tmp11DD)))),_tmp11DC)))));}_LL1F9: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp390=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp35A;if(_tmp390->tag != 26)goto _LL1C4;}_LL1FA: {
# 914
const char*_tmp11E0;void*_tmp11DF;(_tmp11DF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11E0="typedecl in rgns_of",_tag_dyneither(_tmp11E0,sizeof(char),20))),_tag_dyneither(_tmp11DF,sizeof(void*),0)));}_LL1C4:;}
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
{struct Cyc_List_List*_tmp11E1;effects=((_tmp11E1=_cycalloc(sizeof(*_tmp11E1)),((_tmp11E1->hd=_tmp3C4,((_tmp11E1->tl=effects,_tmp11E1))))));}goto _LL216;_LL216:;}}
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
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp11E7;struct Cyc_Absyn_FnInfo _tmp11E6;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp11E5;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3D4=(_tmp11E5=_cycalloc(sizeof(*_tmp11E5)),((_tmp11E5[0]=((_tmp11E7.tag=9,((_tmp11E7.f1=((_tmp11E6.tvars=0,((_tmp11E6.effect=(void*)eff,((_tmp11E6.ret_tqual=
# 965
Cyc_Absyn_empty_tqual(0),((_tmp11E6.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmp11E6.args=0,((_tmp11E6.c_varargs=0,((_tmp11E6.cyc_varargs=0,((_tmp11E6.rgn_po=0,((_tmp11E6.attributes=0,_tmp11E6)))))))))))))))))),_tmp11E7)))),_tmp11E5)));
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
struct _tuple0 _tmp11E8;struct _tuple0 _tmp3E5=(_tmp11E8.f1=r,((_tmp11E8.f2=_tmp3DA,_tmp11E8)));void*_tmp3E6;struct Cyc_Absyn_Tvar*_tmp3E8;void*_tmp3E9;struct Cyc_Absyn_Tvar*_tmp3EB;_LL234: _tmp3E6=_tmp3E5.f1;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3E7=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3E6;if(_tmp3E7->tag != 2)goto _LL236;else{_tmp3E8=_tmp3E7->f1;}};_tmp3E9=_tmp3E5.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3EA=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3E9;if(_tmp3EA->tag != 2)goto _LL236;else{_tmp3EB=_tmp3EA->f1;}};_LL235:
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
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11F7;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11F6;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11F5;void*_tmp11F4[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11F3;void*_tmp3F6=Cyc_Tcutil_dummy_fntype((void*)((_tmp11F3=_cycalloc(sizeof(*_tmp11F3)),((_tmp11F3[0]=((_tmp11F7.tag=24,((_tmp11F7.f1=((_tmp11F4[1]=(void*)((_tmp11F6=_cycalloc(sizeof(*_tmp11F6)),((_tmp11F6[0]=((_tmp11F5.tag=23,((_tmp11F5.f1=(void*)r,_tmp11F5)))),_tmp11F6)))),((_tmp11F4[0]=_tmp3F5,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp11F4,sizeof(void*),2)))))),_tmp11F7)))),_tmp11F3)))));
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
const char*_tmp11FA;void*_tmp11F9;(_tmp11F9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11FA="effect evar has wrong kind",_tag_dyneither(_tmp11FA,sizeof(char),27))),_tag_dyneither(_tmp11F9,sizeof(void*),0)));}
# 1016
if(!constrain)return 0;{
# 1019
void*_tmp3FE=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp3E3);
# 1022
Cyc_Tcutil_occurs(_tmp3FE,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3E3))->v,r);{
# 1023
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp120F;struct Cyc_List_List*_tmp120E;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp120D;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp120C;struct Cyc_List_List*_tmp120B;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp120A;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3FF=(_tmp120A=_cycalloc(sizeof(*_tmp120A)),((_tmp120A[0]=((_tmp120F.tag=24,((_tmp120F.f1=((_tmp120B=_cycalloc(sizeof(*_tmp120B)),((_tmp120B->hd=_tmp3FE,((_tmp120B->tl=((_tmp120E=_cycalloc(sizeof(*_tmp120E)),((_tmp120E->hd=(void*)((_tmp120C=_cycalloc(sizeof(*_tmp120C)),((_tmp120C[0]=((_tmp120D.tag=23,((_tmp120D.f1=(void*)r,_tmp120D)))),_tmp120C)))),((_tmp120E->tl=0,_tmp120E)))))),_tmp120B)))))),_tmp120F)))),_tmp120A)));
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
struct _tuple0 _tmp1210;struct _tuple0 _tmp415=(_tmp1210.f1=Cyc_Tcutil_compress(_tmp413),((_tmp1210.f2=_tmp40B,_tmp1210)));void*_tmp416;struct Cyc_Absyn_Tvar*_tmp418;void*_tmp419;struct Cyc_Absyn_Tvar*_tmp41B;_LL253: _tmp416=_tmp415.f1;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp417=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp416;if(_tmp417->tag != 2)goto _LL255;else{_tmp418=_tmp417->f1;}};_tmp419=_tmp415.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp41A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp419;if(_tmp41A->tag != 2)goto _LL255;else{_tmp41B=_tmp41A->f1;}};_LL254:
# 1049
 return Cyc_Tcutil_unify(t,_tmp40B);_LL255:;_LL256:
# 1050
 return _tmp413 == _tmp40B;_LL252:;}_LL250:;_LL251:
# 1052
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp411);_LL24D:;};_LL249: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp40C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp406;if(_tmp40C->tag != 1)goto _LL24B;else{_tmp40D=_tmp40C->f1;_tmp40E=(void**)& _tmp40C->f2;_tmp40F=(void**)((void**)& _tmp40C->f2);_tmp410=_tmp40C->f4;}}_LL24A:
# 1055
 if(_tmp40D == 0  || ((struct Cyc_Absyn_Kind*)_tmp40D->v)->kind != Cyc_Absyn_EffKind){
# 1056
const char*_tmp1213;void*_tmp1212;(_tmp1212=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1213="effect evar has wrong kind",_tag_dyneither(_tmp1213,sizeof(char),27))),_tag_dyneither(_tmp1212,sizeof(void*),0)));}
# 1057
if(!may_constrain_evars)return 0;{
# 1060
void*_tmp41E=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp410);
# 1063
Cyc_Tcutil_occurs(_tmp41E,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp410))->v,t);{
# 1064
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1228;struct Cyc_List_List*_tmp1227;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp1226;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1225;struct Cyc_List_List*_tmp1224;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1223;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp41F=(_tmp1223=_cycalloc(sizeof(*_tmp1223)),((_tmp1223[0]=((_tmp1228.tag=24,((_tmp1228.f1=((_tmp1224=_cycalloc(sizeof(*_tmp1224)),((_tmp1224->hd=_tmp41E,((_tmp1224->tl=((_tmp1227=_cycalloc(sizeof(*_tmp1227)),((_tmp1227->hd=(void*)((_tmp1225=_cycalloc(sizeof(*_tmp1225)),((_tmp1225[0]=((_tmp1226.tag=25,((_tmp1226.f1=(void*)t,_tmp1226)))),_tmp1225)))),((_tmp1227->tl=0,_tmp1227)))))),_tmp1224)))))),_tmp1228)))),_tmp1223)));
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
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1237;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1236;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1235;void*_tmp1234[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1233;void*_tmp43C=Cyc_Tcutil_dummy_fntype((void*)((_tmp1233=_cycalloc(sizeof(*_tmp1233)),((_tmp1233[0]=((_tmp1237.tag=24,((_tmp1237.f1=((_tmp1234[1]=(void*)((_tmp1236=_cycalloc(sizeof(*_tmp1236)),((_tmp1236[0]=((_tmp1235.tag=2,((_tmp1235.f1=v,_tmp1235)))),_tmp1236)))),((_tmp1234[0]=_tmp43B,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1234,sizeof(void*),2)))))),_tmp1237)))),_tmp1233)))));
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
const char*_tmp123A;void*_tmp1239;(_tmp1239=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp123A="effect evar has wrong kind",_tag_dyneither(_tmp123A,sizeof(char),27))),_tag_dyneither(_tmp1239,sizeof(void*),0)));}{
# 1107
void*_tmp444=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp431);
# 1109
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp431))->v,v))
# 1110
return 0;{
# 1111
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp124F;struct Cyc_List_List*_tmp124E;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp124D;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp124C;struct Cyc_List_List*_tmp124B;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp124A;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp445=(_tmp124A=_cycalloc(sizeof(*_tmp124A)),((_tmp124A[0]=((_tmp124F.tag=24,((_tmp124F.f1=((_tmp124B=_cycalloc(sizeof(*_tmp124B)),((_tmp124B->hd=_tmp444,((_tmp124B->tl=((_tmp124E=_cycalloc(sizeof(*_tmp124E)),((_tmp124E->hd=(void*)((_tmp124C=_cycalloc(sizeof(*_tmp124C)),((_tmp124C[0]=((_tmp124D.tag=2,((_tmp124D.f1=v,_tmp124D)))),_tmp124C)))),((_tmp124E->tl=0,_tmp124E)))))),_tmp124B)))))),_tmp124F)))),_tmp124A)));
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
const char*_tmp1253;void*_tmp1252[1];struct Cyc_String_pa_PrintArg_struct _tmp1251;(_tmp1251.tag=0,((_tmp1251.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((_tmp1252[0]=& _tmp1251,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1253="subset_effect: bad effect: %s",_tag_dyneither(_tmp1253,sizeof(char),30))),_tag_dyneither(_tmp1252,sizeof(void*),1)))))));}_LL27A:;}
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
{const char*_tmp1254;Cyc_Tcutil_failure_reason=((_tmp1254="(type variable would escape scope)",_tag_dyneither(_tmp1254,sizeof(char),35)));}
# 1267
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1270
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp472->v,_tmp476)){
# 1271
*_tmp47A=(void*)t1;return;}
# 1273
{const char*_tmp1255;Cyc_Tcutil_failure_reason=((_tmp1255="(kinds are incompatible)",_tag_dyneither(_tmp1255,sizeof(char),25)));}
# 1274
goto _LL291;}_LL294:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp47C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp477;if(_tmp47C->tag != 5)goto _LL296;else{_tmp47D=_tmp47C->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp472->v)->kind == Cyc_Absyn_BoxKind))goto _LL296;_LL295: {
# 1279
union Cyc_Absyn_Constraint*_tmp481=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp47D.ptr_atts).bounds);
# 1280
{union Cyc_Absyn_Constraint*_tmp482=_tmp481;union Cyc_Absyn_Constraint _tmp483;int _tmp484;_LL299: _tmp483=*_tmp482;if((_tmp483.No_constr).tag != 3)goto _LL29B;_tmp484=(int)(_tmp483.No_constr).val;_LL29A:
# 1283
{struct _union_Constraint_Eq_constr*_tmp1256;(_tmp1256=& _tmp481->Eq_constr,((_tmp1256->tag=1,_tmp1256->val=Cyc_Absyn_bounds_one)));}
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
{const char*_tmp1257;Cyc_Tcutil_failure_reason=((_tmp1257="(kinds are incompatible)",_tag_dyneither(_tmp1257,sizeof(char),25)));}
# 1292
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};_LL28F:;_LL290:
# 1294
 goto _LL28C;_LL28C:;}
# 1299
{struct _tuple0 _tmp1258;struct _tuple0 _tmp488=(_tmp1258.f1=t2,((_tmp1258.f2=t1,_tmp1258)));void*_tmp489;void*_tmp48B;void*_tmp48D;void*_tmp48F;struct Cyc_Absyn_Tvar*_tmp491;void*_tmp492;struct Cyc_Absyn_Tvar*_tmp494;void*_tmp495;struct Cyc_Absyn_AggrInfo _tmp497;union Cyc_Absyn_AggrInfoU _tmp498;struct Cyc_List_List*_tmp499;void*_tmp49A;struct Cyc_Absyn_AggrInfo _tmp49C;union Cyc_Absyn_AggrInfoU _tmp49D;struct Cyc_List_List*_tmp49E;void*_tmp49F;struct _tuple2*_tmp4A1;void*_tmp4A2;struct _tuple2*_tmp4A4;void*_tmp4A5;struct Cyc_List_List*_tmp4A7;void*_tmp4A8;struct Cyc_List_List*_tmp4AA;void*_tmp4AB;struct Cyc_Absyn_DatatypeInfo _tmp4AD;union Cyc_Absyn_DatatypeInfoU _tmp4AE;struct Cyc_Absyn_Datatypedecl**_tmp4AF;struct Cyc_Absyn_Datatypedecl*_tmp4B0;struct Cyc_List_List*_tmp4B1;void*_tmp4B2;struct Cyc_Absyn_DatatypeInfo _tmp4B4;union Cyc_Absyn_DatatypeInfoU _tmp4B5;struct Cyc_Absyn_Datatypedecl**_tmp4B6;struct Cyc_Absyn_Datatypedecl*_tmp4B7;struct Cyc_List_List*_tmp4B8;void*_tmp4B9;struct Cyc_Absyn_DatatypeFieldInfo _tmp4BB;union Cyc_Absyn_DatatypeFieldInfoU _tmp4BC;struct _tuple3 _tmp4BD;struct Cyc_Absyn_Datatypedecl*_tmp4BE;struct Cyc_Absyn_Datatypefield*_tmp4BF;struct Cyc_List_List*_tmp4C0;void*_tmp4C1;struct Cyc_Absyn_DatatypeFieldInfo _tmp4C3;union Cyc_Absyn_DatatypeFieldInfoU _tmp4C4;struct _tuple3 _tmp4C5;struct Cyc_Absyn_Datatypedecl*_tmp4C6;struct Cyc_Absyn_Datatypefield*_tmp4C7;struct Cyc_List_List*_tmp4C8;void*_tmp4C9;struct Cyc_Absyn_PtrInfo _tmp4CB;void*_tmp4CC;struct Cyc_Absyn_Tqual _tmp4CD;struct Cyc_Absyn_PtrAtts _tmp4CE;void*_tmp4CF;union Cyc_Absyn_Constraint*_tmp4D0;union Cyc_Absyn_Constraint*_tmp4D1;union Cyc_Absyn_Constraint*_tmp4D2;void*_tmp4D3;struct Cyc_Absyn_PtrInfo _tmp4D5;void*_tmp4D6;struct Cyc_Absyn_Tqual _tmp4D7;struct Cyc_Absyn_PtrAtts _tmp4D8;void*_tmp4D9;union Cyc_Absyn_Constraint*_tmp4DA;union Cyc_Absyn_Constraint*_tmp4DB;union Cyc_Absyn_Constraint*_tmp4DC;void*_tmp4DD;enum Cyc_Absyn_Sign _tmp4DF;enum Cyc_Absyn_Size_of _tmp4E0;void*_tmp4E1;enum Cyc_Absyn_Sign _tmp4E3;enum Cyc_Absyn_Size_of _tmp4E4;void*_tmp4E5;int _tmp4E7;void*_tmp4E8;int _tmp4EA;void*_tmp4EB;void*_tmp4ED;void*_tmp4EE;void*_tmp4F0;void*_tmp4F1;struct Cyc_Absyn_Exp*_tmp4F3;void*_tmp4F4;struct Cyc_Absyn_Exp*_tmp4F6;void*_tmp4F7;struct Cyc_Absyn_ArrayInfo _tmp4F9;void*_tmp4FA;struct Cyc_Absyn_Tqual _tmp4FB;struct Cyc_Absyn_Exp*_tmp4FC;union Cyc_Absyn_Constraint*_tmp4FD;void*_tmp4FE;struct Cyc_Absyn_ArrayInfo _tmp500;void*_tmp501;struct Cyc_Absyn_Tqual _tmp502;struct Cyc_Absyn_Exp*_tmp503;union Cyc_Absyn_Constraint*_tmp504;void*_tmp505;struct Cyc_Absyn_FnInfo _tmp507;struct Cyc_List_List*_tmp508;void*_tmp509;struct Cyc_Absyn_Tqual _tmp50A;void*_tmp50B;struct Cyc_List_List*_tmp50C;int _tmp50D;struct Cyc_Absyn_VarargInfo*_tmp50E;struct Cyc_List_List*_tmp50F;struct Cyc_List_List*_tmp510;void*_tmp511;struct Cyc_Absyn_FnInfo _tmp513;struct Cyc_List_List*_tmp514;void*_tmp515;struct Cyc_Absyn_Tqual _tmp516;void*_tmp517;struct Cyc_List_List*_tmp518;int _tmp519;struct Cyc_Absyn_VarargInfo*_tmp51A;struct Cyc_List_List*_tmp51B;struct Cyc_List_List*_tmp51C;void*_tmp51D;struct Cyc_List_List*_tmp51F;void*_tmp520;struct Cyc_List_List*_tmp522;void*_tmp523;enum Cyc_Absyn_AggrKind _tmp525;struct Cyc_List_List*_tmp526;void*_tmp527;enum Cyc_Absyn_AggrKind _tmp529;struct Cyc_List_List*_tmp52A;void*_tmp52B;struct Cyc_List_List*_tmp52D;struct Cyc_Absyn_Typedefdecl*_tmp52E;void*_tmp52F;struct Cyc_List_List*_tmp531;struct Cyc_Absyn_Typedefdecl*_tmp532;void*_tmp533;void*_tmp535;void*_tmp537;void*_tmp539;void*_tmp53B;void*_tmp53D;void*_tmp53F;void*_tmp541;void*_tmp542;void*_tmp544;void*_tmp545;void*_tmp547;void*_tmp548;void*_tmp549;void*_tmp54B;void*_tmp54C;void*_tmp54D;void*_tmp54F;void*_tmp551;void*_tmp553;void*_tmp555;void*_tmp557;_LL29E: _tmp489=_tmp488.f1;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp48A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp489;if(_tmp48A->tag != 1)goto _LL2A0;};_LL29F:
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
{const char*_tmp1259;Cyc_Tcutil_failure_reason=((_tmp1259="(variable types are not the same)",_tag_dyneither(_tmp1259,sizeof(char),34)));}
# 1315
goto _LL29D;}_LL2A4: _tmp495=_tmp488.f1;{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp496=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp495;if(_tmp496->tag != 11)goto _LL2A6;else{_tmp497=_tmp496->f1;_tmp498=_tmp497.aggr_info;_tmp499=_tmp497.targs;}};_tmp49A=_tmp488.f2;{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp49B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp49A;if(_tmp49B->tag != 11)goto _LL2A6;else{_tmp49C=_tmp49B->f1;_tmp49D=_tmp49C.aggr_info;_tmp49E=_tmp49C.targs;}};_LL2A5: {
# 1318
enum Cyc_Absyn_AggrKind _tmp55F;struct _tuple2*_tmp560;struct _tuple13 _tmp55E=Cyc_Absyn_aggr_kinded_name(_tmp49D);_tmp55F=_tmp55E.f1;_tmp560=_tmp55E.f2;{
# 1319
enum Cyc_Absyn_AggrKind _tmp562;struct _tuple2*_tmp563;struct _tuple13 _tmp561=Cyc_Absyn_aggr_kinded_name(_tmp498);_tmp562=_tmp561.f1;_tmp563=_tmp561.f2;
# 1320
if(_tmp55F != _tmp562){{const char*_tmp125A;Cyc_Tcutil_failure_reason=((_tmp125A="(struct and union type)",_tag_dyneither(_tmp125A,sizeof(char),24)));}goto _LL29D;}
# 1321
if(Cyc_Absyn_qvar_cmp(_tmp560,_tmp563)!= 0){{const char*_tmp125B;Cyc_Tcutil_failure_reason=((_tmp125B="(different type name)",_tag_dyneither(_tmp125B,sizeof(char),22)));}goto _LL29D;}
# 1322
Cyc_Tcutil_unify_list(_tmp49E,_tmp499);
# 1323
return;};}_LL2A6: _tmp49F=_tmp488.f1;{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp4A0=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp49F;if(_tmp4A0->tag != 13)goto _LL2A8;else{_tmp4A1=_tmp4A0->f1;}};_tmp4A2=_tmp488.f2;{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp4A3=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4A2;if(_tmp4A3->tag != 13)goto _LL2A8;else{_tmp4A4=_tmp4A3->f1;}};_LL2A7:
# 1327
 if(Cyc_Absyn_qvar_cmp(_tmp4A1,_tmp4A4)== 0)return;
# 1328
{const char*_tmp125C;Cyc_Tcutil_failure_reason=((_tmp125C="(different enum types)",_tag_dyneither(_tmp125C,sizeof(char),23)));}
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
{const char*_tmp125D;Cyc_Tcutil_failure_reason=((_tmp125D="(different names for enum fields)",_tag_dyneither(_tmp125D,sizeof(char),34)));}
# 1338
bad=1;
# 1339
break;}
# 1341
if(_tmp567->tag == _tmp568->tag)continue;
# 1342
if(_tmp567->tag == 0  || _tmp568->tag == 0){
# 1343
{const char*_tmp125E;Cyc_Tcutil_failure_reason=((_tmp125E="(different tag values for enum fields)",_tag_dyneither(_tmp125E,sizeof(char),39)));}
# 1344
bad=1;
# 1345
break;}
# 1347
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp567->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp568->tag))){
# 1348
{const char*_tmp125F;Cyc_Tcutil_failure_reason=((_tmp125F="(different tag values for enum fields)",_tag_dyneither(_tmp125F,sizeof(char),39)));}
# 1349
bad=1;
# 1350
break;}}
# 1353
if(bad)goto _LL29D;
# 1354
if(_tmp4A7 == 0  && _tmp4AA == 0)return;
# 1355
{const char*_tmp1260;Cyc_Tcutil_failure_reason=((_tmp1260="(different number of fields for enums)",_tag_dyneither(_tmp1260,sizeof(char),39)));}
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
{const char*_tmp1261;Cyc_Tcutil_failure_reason=((_tmp1261="(different datatype field types)",_tag_dyneither(_tmp1261,sizeof(char),33)));}
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
{const char*_tmp1262;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp4DC,_tmp4D2,((_tmp1262="(not both zero terminated)",_tag_dyneither(_tmp1262,sizeof(char),27))));}
# 1383
Cyc_Tcutil_unify_tqual(_tmp4D7,_tmp4D6,_tmp4CD,_tmp4CC);
# 1384
{const char*_tmp1263;((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp4DB,_tmp4D1,((_tmp1263="(different pointer bounds)",_tag_dyneither(_tmp1263,sizeof(char),27))));}
# 1386
{void*_tmp570=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4DB);_LL2DB: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp571=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp570;if(_tmp571->tag != 0)goto _LL2DD;}_LL2DC:
# 1387
 return;_LL2DD:;_LL2DE:
# 1388
 goto _LL2DA;_LL2DA:;}
# 1390
{const char*_tmp1264;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp4DA,_tmp4D0,((_tmp1264="(incompatible pointer types)",_tag_dyneither(_tmp1264,sizeof(char),29))));}
# 1391
return;_LL2B0: _tmp4DD=_tmp488.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4DE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4DD;if(_tmp4DE->tag != 6)goto _LL2B2;else{_tmp4DF=_tmp4DE->f1;_tmp4E0=_tmp4DE->f2;}};_tmp4E1=_tmp488.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4E2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4E1;if(_tmp4E2->tag != 6)goto _LL2B2;else{_tmp4E3=_tmp4E2->f1;_tmp4E4=_tmp4E2->f2;}};_LL2B1:
# 1395
 if(_tmp4E3 == _tmp4DF  && ((_tmp4E4 == _tmp4E0  || _tmp4E4 == Cyc_Absyn_Int_sz  && _tmp4E0 == Cyc_Absyn_Long_sz) || 
# 1396
_tmp4E4 == Cyc_Absyn_Long_sz  && _tmp4E0 == Cyc_Absyn_Int_sz))return;
# 1397
{const char*_tmp1265;Cyc_Tcutil_failure_reason=((_tmp1265="(different integral types)",_tag_dyneither(_tmp1265,sizeof(char),27)));}
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
{const char*_tmp1266;Cyc_Tcutil_failure_reason=((_tmp1266="(cannot prove expressions are the same)",_tag_dyneither(_tmp1266,sizeof(char),40)));}
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
{const char*_tmp1267;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp4FD,_tmp504,((_tmp1267="(not both zero terminated)",_tag_dyneither(_tmp1267,sizeof(char),27))));}
# 1423
if(_tmp4FC == _tmp503)return;
# 1424
if(_tmp4FC == 0  || _tmp503 == 0)goto _LL29D;
# 1425
if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp4FC,(struct Cyc_Absyn_Exp*)_tmp503))
# 1426
return;
# 1427
{const char*_tmp1268;Cyc_Tcutil_failure_reason=((_tmp1268="(different array sizes)",_tag_dyneither(_tmp1268,sizeof(char),24)));}
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
{const char*_tmp1269;Cyc_Tcutil_failure_reason=((_tmp1269="(second function type has too few type variables)",_tag_dyneither(_tmp1269,sizeof(char),50)));}
# 1438
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1440
void*_tmp579=((struct Cyc_Absyn_Tvar*)_tmp514->hd)->kind;
# 1441
void*_tmp57A=((struct Cyc_Absyn_Tvar*)_tmp508->hd)->kind;
# 1442
if(!Cyc_Tcutil_unify_kindbound(_tmp579,_tmp57A)){
# 1443
{const char*_tmp126F;void*_tmp126E[3];struct Cyc_String_pa_PrintArg_struct _tmp126D;struct Cyc_String_pa_PrintArg_struct _tmp126C;struct Cyc_String_pa_PrintArg_struct _tmp126B;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)((_tmp126B.tag=0,((_tmp126B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1446
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp508->hd,& Cyc_Tcutil_bk))),((_tmp126C.tag=0,((_tmp126C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1445
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp514->hd,& Cyc_Tcutil_bk))),((_tmp126D.tag=0,((_tmp126D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1444
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp514->hd)),((_tmp126E[0]=& _tmp126D,((_tmp126E[1]=& _tmp126C,((_tmp126E[2]=& _tmp126B,Cyc_aprintf(((_tmp126F="(type var %s has different kinds %s and %s)",_tag_dyneither(_tmp126F,sizeof(char),44))),_tag_dyneither(_tmp126E,sizeof(void*),3))))))))))))))))))));}
# 1447
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1449
{struct _tuple16*_tmp1279;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1278;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1277;struct Cyc_List_List*_tmp1276;inst=((_tmp1276=_region_malloc(rgn,sizeof(*_tmp1276)),((_tmp1276->hd=((_tmp1279=_region_malloc(rgn,sizeof(*_tmp1279)),((_tmp1279->f1=(struct Cyc_Absyn_Tvar*)_tmp508->hd,((_tmp1279->f2=(void*)((_tmp1277=_cycalloc(sizeof(*_tmp1277)),((_tmp1277[0]=((_tmp1278.tag=2,((_tmp1278.f1=(struct Cyc_Absyn_Tvar*)_tmp514->hd,_tmp1278)))),_tmp1277)))),_tmp1279)))))),((_tmp1276->tl=inst,_tmp1276))))));}
# 1450
_tmp514=_tmp514->tl;
# 1451
_tmp508=_tmp508->tl;};}
# 1453
if(_tmp508 != 0){
# 1454
{const char*_tmp127A;Cyc_Tcutil_failure_reason=((_tmp127A="(second function type has too many type variables)",_tag_dyneither(_tmp127A,sizeof(char),51)));}
# 1455
_npop_handler(0);goto _LL29D;}
# 1457
if(inst != 0){
# 1458
{struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1286;struct Cyc_Absyn_FnInfo _tmp1285;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1284;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1280;struct Cyc_Absyn_FnInfo _tmp127F;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp127E;Cyc_Tcutil_unify_it((void*)((_tmp127E=_cycalloc(sizeof(*_tmp127E)),((_tmp127E[0]=((_tmp1280.tag=9,((_tmp1280.f1=((_tmp127F.tvars=0,((_tmp127F.effect=_tmp515,((_tmp127F.ret_tqual=_tmp516,((_tmp127F.ret_typ=_tmp517,((_tmp127F.args=_tmp518,((_tmp127F.c_varargs=_tmp519,((_tmp127F.cyc_varargs=_tmp51A,((_tmp127F.rgn_po=_tmp51B,((_tmp127F.attributes=_tmp51C,_tmp127F)))))))))))))))))),_tmp1280)))),_tmp127E)))),
# 1460
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
# 1461
(_tmp1284=_cycalloc(sizeof(*_tmp1284)),((_tmp1284[0]=((_tmp1286.tag=9,((_tmp1286.f1=((_tmp1285.tvars=0,((_tmp1285.effect=_tmp509,((_tmp1285.ret_tqual=_tmp50A,((_tmp1285.ret_typ=_tmp50B,((_tmp1285.args=_tmp50C,((_tmp1285.c_varargs=_tmp50D,((_tmp1285.cyc_varargs=_tmp50E,((_tmp1285.rgn_po=_tmp50F,((_tmp1285.attributes=_tmp510,_tmp1285)))))))))))))))))),_tmp1286)))),_tmp1284))))));}
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
{const char*_tmp1287;Cyc_Tcutil_failure_reason=((_tmp1287="(function types have different number of arguments)",_tag_dyneither(_tmp1287,sizeof(char),52)));}
# 1480
_npop_handler(0);goto _LL29D;}
# 1482
if(_tmp519 != _tmp50D){
# 1483
{const char*_tmp1288;Cyc_Tcutil_failure_reason=((_tmp1288="(only one function type takes C varargs)",_tag_dyneither(_tmp1288,sizeof(char),41)));}
# 1484
_npop_handler(0);goto _LL29D;}{
# 1487
int bad_cyc_vararg=0;
# 1488
{struct _tuple22 _tmp1289;struct _tuple22 _tmp594=(_tmp1289.f1=_tmp51A,((_tmp1289.f2=_tmp50E,_tmp1289)));struct Cyc_Absyn_VarargInfo*_tmp595;struct Cyc_Absyn_VarargInfo*_tmp596;struct Cyc_Absyn_VarargInfo*_tmp597;struct Cyc_Absyn_VarargInfo*_tmp598;struct Cyc_Absyn_VarargInfo*_tmp599;struct Cyc_Absyn_VarargInfo _tmp59A;struct _dyneither_ptr*_tmp59B;struct Cyc_Absyn_Tqual _tmp59C;void*_tmp59D;int _tmp59E;struct Cyc_Absyn_VarargInfo*_tmp59F;struct Cyc_Absyn_VarargInfo _tmp5A0;struct _dyneither_ptr*_tmp5A1;struct Cyc_Absyn_Tqual _tmp5A2;void*_tmp5A3;int _tmp5A4;_LL2E0: _tmp595=_tmp594.f1;if(_tmp595 != 0)goto _LL2E2;_tmp596=_tmp594.f2;if(_tmp596 != 0)goto _LL2E2;_LL2E1:
# 1489
 goto _LL2DF;_LL2E2: _tmp597=_tmp594.f1;if(_tmp597 != 0)goto _LL2E4;_LL2E3:
# 1490
 goto _LL2E5;_LL2E4: _tmp598=_tmp594.f2;if(_tmp598 != 0)goto _LL2E6;_LL2E5:
# 1492
 bad_cyc_vararg=1;
# 1493
{const char*_tmp128A;Cyc_Tcutil_failure_reason=((_tmp128A="(only one function type takes varargs)",_tag_dyneither(_tmp128A,sizeof(char),39)));}
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
const char*_tmp128B;Cyc_Tcutil_failure_reason=((_tmp128B="(only one function type injects varargs)",_tag_dyneither(_tmp128B,sizeof(char),41)));};}
# 1502
goto _LL2DF;_LL2DF:;}
# 1504
if(bad_cyc_vararg){_npop_handler(0);goto _LL29D;}{
# 1507
int bad_effect=0;
# 1508
{struct _tuple0 _tmp128C;struct _tuple0 _tmp5A8=(_tmp128C.f1=_tmp515,((_tmp128C.f2=_tmp509,_tmp128C)));void*_tmp5A9;void*_tmp5AA;void*_tmp5AB;void*_tmp5AC;_LL2E9: _tmp5A9=_tmp5A8.f1;if(_tmp5A9 != 0)goto _LL2EB;_tmp5AA=_tmp5A8.f2;if(_tmp5AA != 0)goto _LL2EB;_LL2EA:
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
{const char*_tmp128D;Cyc_Tcutil_failure_reason=((_tmp128D="(function type effects do not unify)",_tag_dyneither(_tmp128D,sizeof(char),37)));}
# 1518
_npop_handler(0);goto _LL29D;}
# 1520
if(!Cyc_Tcutil_same_atts(_tmp510,_tmp51C)){
# 1521
{const char*_tmp128E;Cyc_Tcutil_failure_reason=((_tmp128E="(function types have different attributes)",_tag_dyneither(_tmp128E,sizeof(char),43)));}
# 1522
_npop_handler(0);goto _LL29D;}
# 1524
if(!Cyc_Tcutil_same_rgn_po(_tmp50F,_tmp51B)){
# 1525
{const char*_tmp128F;Cyc_Tcutil_failure_reason=((_tmp128F="(function types have different region lifetime orderings)",_tag_dyneither(_tmp128F,sizeof(char),58)));}
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
{const char*_tmp1290;Cyc_Tcutil_failure_reason=((_tmp1290="(tuple types have different numbers of components)",_tag_dyneither(_tmp1290,sizeof(char),51)));}
# 1541
goto _LL29D;_LL2BE: _tmp523=_tmp488.f1;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp524=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp523;if(_tmp524->tag != 12)goto _LL2C0;else{_tmp525=_tmp524->f1;_tmp526=_tmp524->f2;}};_tmp527=_tmp488.f2;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp528=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp527;if(_tmp528->tag != 12)goto _LL2C0;else{_tmp529=_tmp528->f1;_tmp52A=_tmp528->f2;}};_LL2BF:
# 1544
 if(_tmp529 != _tmp525){{const char*_tmp1291;Cyc_Tcutil_failure_reason=((_tmp1291="(struct and union type)",_tag_dyneither(_tmp1291,sizeof(char),24)));}goto _LL29D;}
# 1545
for(0;_tmp52A != 0  && _tmp526 != 0;(_tmp52A=_tmp52A->tl,_tmp526=_tmp526->tl)){
# 1546
struct Cyc_Absyn_Aggrfield*_tmp5B8=(struct Cyc_Absyn_Aggrfield*)_tmp52A->hd;
# 1547
struct Cyc_Absyn_Aggrfield*_tmp5B9=(struct Cyc_Absyn_Aggrfield*)_tmp526->hd;
# 1548
if(Cyc_strptrcmp(_tmp5B8->name,_tmp5B9->name)!= 0){
# 1549
{const char*_tmp1292;Cyc_Tcutil_failure_reason=((_tmp1292="(different member names)",_tag_dyneither(_tmp1292,sizeof(char),25)));}
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
{const char*_tmp1293;Cyc_Tcutil_failure_reason=((_tmp1293="(different attributes on member)",_tag_dyneither(_tmp1293,sizeof(char),33)));}
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
{const char*_tmp1294;Cyc_Tcutil_failure_reason=((_tmp1294="(different bitfield widths on member)",_tag_dyneither(_tmp1294,sizeof(char),38)));}
# 1567
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1570
if(_tmp52A == 0  && _tmp526 == 0)return;
# 1571
Cyc_Tcutil_t1_failure=t1;
# 1572
Cyc_Tcutil_t2_failure=t2;
# 1573
{const char*_tmp1295;Cyc_Tcutil_failure_reason=((_tmp1295="(different number of members)",_tag_dyneither(_tmp1295,sizeof(char),30)));}
# 1574
goto _LL29D;_LL2C0: _tmp52B=_tmp488.f1;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp52C=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp52B;if(_tmp52C->tag != 17)goto _LL2C2;else{_tmp52D=_tmp52C->f2;_tmp52E=_tmp52C->f3;}};_tmp52F=_tmp488.f2;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp530=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp52F;if(_tmp530->tag != 17)goto _LL2C2;else{_tmp531=_tmp530->f2;_tmp532=_tmp530->f3;}};_LL2C1:
# 1576
 if(_tmp52E != _tmp532){
# 1577
{const char*_tmp1296;Cyc_Tcutil_failure_reason=((_tmp1296="(different abstract typedefs)",_tag_dyneither(_tmp1296,sizeof(char),30)));}
# 1578
goto _LL29D;}
# 1580
{const char*_tmp1297;Cyc_Tcutil_failure_reason=((_tmp1297="(type parameters to typedef differ)",_tag_dyneither(_tmp1297,sizeof(char),36)));}
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
{const char*_tmp1298;Cyc_Tcutil_failure_reason=((_tmp1298="(effects don't unify)",_tag_dyneither(_tmp1298,sizeof(char),22)));}
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
const char*_tmp129B;void*_tmp129A;(_tmp129A=0,Cyc_Tcutil_impos(((_tmp129B="unify_conref: forward after compress(2)",_tag_dyneither(_tmp129B,sizeof(char),40))),_tag_dyneither(_tmp129A,sizeof(void*),0)));}_LL2F8:;}_LL2F6: _tmp5C8=*_tmp5C3;if((_tmp5C8.Forward_constr).tag != 2)goto _LL2F1;_tmp5C9=(union Cyc_Absyn_Constraint*)(_tmp5C8.Forward_constr).val;_LL2F7: {
# 1633
const char*_tmp129E;void*_tmp129D;(_tmp129D=0,Cyc_Tcutil_impos(((_tmp129E="unify_conref: forward after compress",_tag_dyneither(_tmp129E,sizeof(char),37))),_tag_dyneither(_tmp129D,sizeof(void*),0)));}_LL2F1:;};}
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
struct _tuple0 _tmp129F;struct _tuple0 _tmp603=(_tmp129F.f1=t2,((_tmp129F.f2=t1,_tmp129F)));void*_tmp604;void*_tmp606;void*_tmp608;struct Cyc_Absyn_Tvar*_tmp60A;void*_tmp60B;struct Cyc_Absyn_Tvar*_tmp60D;void*_tmp60E;struct Cyc_Absyn_AggrInfo _tmp610;union Cyc_Absyn_AggrInfoU _tmp611;struct Cyc_List_List*_tmp612;void*_tmp613;struct Cyc_Absyn_AggrInfo _tmp615;union Cyc_Absyn_AggrInfoU _tmp616;struct Cyc_List_List*_tmp617;void*_tmp618;struct _tuple2*_tmp61A;void*_tmp61B;struct _tuple2*_tmp61D;void*_tmp61E;struct Cyc_List_List*_tmp620;void*_tmp621;struct Cyc_List_List*_tmp623;void*_tmp624;struct Cyc_Absyn_DatatypeInfo _tmp626;union Cyc_Absyn_DatatypeInfoU _tmp627;struct Cyc_Absyn_Datatypedecl**_tmp628;struct Cyc_Absyn_Datatypedecl*_tmp629;struct Cyc_List_List*_tmp62A;void*_tmp62B;struct Cyc_Absyn_DatatypeInfo _tmp62D;union Cyc_Absyn_DatatypeInfoU _tmp62E;struct Cyc_Absyn_Datatypedecl**_tmp62F;struct Cyc_Absyn_Datatypedecl*_tmp630;struct Cyc_List_List*_tmp631;void*_tmp632;struct Cyc_Absyn_DatatypeFieldInfo _tmp634;union Cyc_Absyn_DatatypeFieldInfoU _tmp635;struct _tuple3 _tmp636;struct Cyc_Absyn_Datatypedecl*_tmp637;struct Cyc_Absyn_Datatypefield*_tmp638;struct Cyc_List_List*_tmp639;void*_tmp63A;struct Cyc_Absyn_DatatypeFieldInfo _tmp63C;union Cyc_Absyn_DatatypeFieldInfoU _tmp63D;struct _tuple3 _tmp63E;struct Cyc_Absyn_Datatypedecl*_tmp63F;struct Cyc_Absyn_Datatypefield*_tmp640;struct Cyc_List_List*_tmp641;void*_tmp642;struct Cyc_Absyn_PtrInfo _tmp644;void*_tmp645;struct Cyc_Absyn_Tqual _tmp646;struct Cyc_Absyn_PtrAtts _tmp647;void*_tmp648;union Cyc_Absyn_Constraint*_tmp649;union Cyc_Absyn_Constraint*_tmp64A;union Cyc_Absyn_Constraint*_tmp64B;void*_tmp64C;struct Cyc_Absyn_PtrInfo _tmp64E;void*_tmp64F;struct Cyc_Absyn_Tqual _tmp650;struct Cyc_Absyn_PtrAtts _tmp651;void*_tmp652;union Cyc_Absyn_Constraint*_tmp653;union Cyc_Absyn_Constraint*_tmp654;union Cyc_Absyn_Constraint*_tmp655;void*_tmp656;enum Cyc_Absyn_Sign _tmp658;enum Cyc_Absyn_Size_of _tmp659;void*_tmp65A;enum Cyc_Absyn_Sign _tmp65C;enum Cyc_Absyn_Size_of _tmp65D;void*_tmp65E;int _tmp660;void*_tmp661;int _tmp663;void*_tmp664;struct Cyc_Absyn_ArrayInfo _tmp666;void*_tmp667;struct Cyc_Absyn_Tqual _tmp668;struct Cyc_Absyn_Exp*_tmp669;union Cyc_Absyn_Constraint*_tmp66A;void*_tmp66B;struct Cyc_Absyn_ArrayInfo _tmp66D;void*_tmp66E;struct Cyc_Absyn_Tqual _tmp66F;struct Cyc_Absyn_Exp*_tmp670;union Cyc_Absyn_Constraint*_tmp671;void*_tmp672;struct Cyc_Absyn_FnInfo _tmp674;struct Cyc_List_List*_tmp675;void*_tmp676;struct Cyc_Absyn_Tqual _tmp677;void*_tmp678;struct Cyc_List_List*_tmp679;int _tmp67A;struct Cyc_Absyn_VarargInfo*_tmp67B;struct Cyc_List_List*_tmp67C;struct Cyc_List_List*_tmp67D;void*_tmp67E;struct Cyc_Absyn_FnInfo _tmp680;struct Cyc_List_List*_tmp681;void*_tmp682;struct Cyc_Absyn_Tqual _tmp683;void*_tmp684;struct Cyc_List_List*_tmp685;int _tmp686;struct Cyc_Absyn_VarargInfo*_tmp687;struct Cyc_List_List*_tmp688;struct Cyc_List_List*_tmp689;void*_tmp68A;struct Cyc_List_List*_tmp68C;void*_tmp68D;struct Cyc_List_List*_tmp68F;void*_tmp690;enum Cyc_Absyn_AggrKind _tmp692;struct Cyc_List_List*_tmp693;void*_tmp694;enum Cyc_Absyn_AggrKind _tmp696;struct Cyc_List_List*_tmp697;void*_tmp698;void*_tmp69A;void*_tmp69B;void*_tmp69D;void*_tmp69E;void*_tmp6A0;void*_tmp6A1;void*_tmp6A2;void*_tmp6A4;void*_tmp6A5;void*_tmp6A6;void*_tmp6A8;void*_tmp6A9;void*_tmp6AB;void*_tmp6AC;struct Cyc_Absyn_Exp*_tmp6AE;void*_tmp6AF;struct Cyc_Absyn_Exp*_tmp6B1;void*_tmp6B2;void*_tmp6B4;void*_tmp6B6;void*_tmp6B8;void*_tmp6BA;void*_tmp6BC;_LL339: _tmp604=_tmp603.f1;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp605=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp604;if(_tmp605->tag != 1)goto _LL33B;};_tmp606=_tmp603.f2;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp607=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp606;if(_tmp607->tag != 1)goto _LL33B;};_LL33A: {
# 1709
const char*_tmp12A2;void*_tmp12A1;(_tmp12A1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12A2="typecmp: can only compare closed types",_tag_dyneither(_tmp12A2,sizeof(char),39))),_tag_dyneither(_tmp12A1,sizeof(void*),0)));}_LL33B: _tmp608=_tmp603.f1;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp609=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp608;if(_tmp609->tag != 2)goto _LL33D;else{_tmp60A=_tmp609->f1;}};_tmp60B=_tmp603.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp60C=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp60B;if(_tmp60C->tag != 2)goto _LL33D;else{_tmp60D=_tmp60C->f1;}};_LL33C:
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
const char*_tmp12A5;void*_tmp12A4;(_tmp12A4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12A5="missing expression in array index",_tag_dyneither(_tmp12A5,sizeof(char),34))),_tag_dyneither(_tmp12A4,sizeof(void*),0)));}
# 1786
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp669,_tmp670);};};}_LL34F: _tmp672=_tmp603.f1;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp673=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp672;if(_tmp673->tag != 9)goto _LL351;else{_tmp674=_tmp673->f1;_tmp675=_tmp674.tvars;_tmp676=_tmp674.effect;_tmp677=_tmp674.ret_tqual;_tmp678=_tmp674.ret_typ;_tmp679=_tmp674.args;_tmp67A=_tmp674.c_varargs;_tmp67B=_tmp674.cyc_varargs;_tmp67C=_tmp674.rgn_po;_tmp67D=_tmp674.attributes;}};_tmp67E=_tmp603.f2;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp67F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp67E;if(_tmp67F->tag != 9)goto _LL351;else{_tmp680=_tmp67F->f1;_tmp681=_tmp680.tvars;_tmp682=_tmp680.effect;_tmp683=_tmp680.ret_tqual;_tmp684=_tmp680.ret_typ;_tmp685=_tmp680.args;_tmp686=_tmp680.c_varargs;_tmp687=_tmp680.cyc_varargs;_tmp688=_tmp680.rgn_po;_tmp689=_tmp680.attributes;}};_LL350: {
# 1790
const char*_tmp12A8;void*_tmp12A7;(_tmp12A7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12A8="typecmp: function types not handled",_tag_dyneither(_tmp12A8,sizeof(char),36))),_tag_dyneither(_tmp12A7,sizeof(void*),0)));}_LL351: _tmp68A=_tmp603.f1;{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp68B=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp68A;if(_tmp68B->tag != 10)goto _LL353;else{_tmp68C=_tmp68B->f1;}};_tmp68D=_tmp603.f2;{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp68E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp68D;if(_tmp68E->tag != 10)goto _LL353;else{_tmp68F=_tmp68E->f1;}};_LL352:
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
const char*_tmp12AB;void*_tmp12AA;(_tmp12AA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12AB="typecmp: effects not handled",_tag_dyneither(_tmp12AB,sizeof(char),29))),_tag_dyneither(_tmp12AA,sizeof(void*),0)));}_LL369:;_LL36A: {
# 1815
const char*_tmp12AE;void*_tmp12AD;(_tmp12AD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12AE="Unmatched case in typecmp",_tag_dyneither(_tmp12AE,sizeof(char),26))),_tag_dyneither(_tmp12AD,sizeof(void*),0)));}_LL338:;};};}
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
struct _tuple0 _tmp12AF;struct _tuple0 _tmp6E1=(_tmp12AF.f1=t1,((_tmp12AF.f2=t2,_tmp12AF)));void*_tmp6E2;int _tmp6E4;void*_tmp6E5;int _tmp6E7;void*_tmp6E8;void*_tmp6EA;void*_tmp6EC;void*_tmp6EE;void*_tmp6F0;enum Cyc_Absyn_Size_of _tmp6F2;void*_tmp6F3;enum Cyc_Absyn_Size_of _tmp6F5;void*_tmp6F6;enum Cyc_Absyn_Size_of _tmp6F8;void*_tmp6F9;enum Cyc_Absyn_Size_of _tmp6FB;void*_tmp6FC;enum Cyc_Absyn_Size_of _tmp6FE;void*_tmp6FF;enum Cyc_Absyn_Size_of _tmp701;void*_tmp702;enum Cyc_Absyn_Size_of _tmp704;void*_tmp705;enum Cyc_Absyn_Size_of _tmp707;void*_tmp708;int _tmp70A;void*_tmp70B;enum Cyc_Absyn_Size_of _tmp70D;void*_tmp70E;int _tmp710;void*_tmp711;enum Cyc_Absyn_Size_of _tmp713;void*_tmp714;enum Cyc_Absyn_Size_of _tmp716;void*_tmp717;enum Cyc_Absyn_Size_of _tmp719;void*_tmp71A;enum Cyc_Absyn_Size_of _tmp71C;void*_tmp71D;void*_tmp71F;enum Cyc_Absyn_Size_of _tmp721;void*_tmp722;enum Cyc_Absyn_Size_of _tmp724;void*_tmp725;enum Cyc_Absyn_Size_of _tmp727;void*_tmp728;enum Cyc_Absyn_Size_of _tmp72A;void*_tmp72B;enum Cyc_Absyn_Size_of _tmp72D;void*_tmp72E;enum Cyc_Absyn_Size_of _tmp730;void*_tmp731;enum Cyc_Absyn_Size_of _tmp733;void*_tmp734;void*_tmp736;enum Cyc_Absyn_Size_of _tmp738;_LL37C: _tmp6E2=_tmp6E1.f1;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp6E3=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6E2;if(_tmp6E3->tag != 7)goto _LL37E;else{_tmp6E4=_tmp6E3->f1;}};_tmp6E5=_tmp6E1.f2;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp6E6=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6E5;if(_tmp6E6->tag != 7)goto _LL37E;else{_tmp6E7=_tmp6E6->f1;}};_LL37D:
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
struct Cyc_Core_Opt*_tmp12B0;max_arith_type=((_tmp12B0=_region_malloc(r,sizeof(*_tmp12B0)),((_tmp12B0->v=t1,_tmp12B0))));}}}}
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
{const char*_tmp12B5;void*_tmp12B4[2];struct Cyc_String_pa_PrintArg_struct _tmp12B3;struct Cyc_String_pa_PrintArg_struct _tmp12B2;(_tmp12B2.tag=0,((_tmp12B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1883
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))),((_tmp12B3.tag=0,((_tmp12B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp12B4[0]=& _tmp12B3,((_tmp12B4[1]=& _tmp12B2,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,((_tmp12B5="type mismatch: expecting %s but found %s",_tag_dyneither(_tmp12B5,sizeof(char),41))),_tag_dyneither(_tmp12B4,sizeof(void*),2)))))))))))));}{
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
const char*_tmp12B8;void*_tmp12B7;(_tmp12B7=0,Cyc_Tcutil_warn(e->loc,((_tmp12B8="integral size mismatch; conversion supplied",_tag_dyneither(_tmp12B8,sizeof(char),44))),_tag_dyneither(_tmp12B7,sizeof(void*),0)));}
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
const char*_tmp12BB;void*_tmp12BA;(_tmp12BA=0,Cyc_Tcutil_warn(e->loc,((_tmp12BB="integral size mismatch; conversion supplied",_tag_dyneither(_tmp12BB,sizeof(char),44))),_tag_dyneither(_tmp12BA,sizeof(void*),0)));}
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
struct _tuple0 _tmp12BC;struct _tuple0 _tmp74E=(_tmp12BC.f1=t1,((_tmp12BC.f2=t2,_tmp12BC)));void*_tmp74F;struct Cyc_Absyn_PtrInfo _tmp751;void*_tmp752;struct Cyc_Absyn_PtrInfo _tmp754;void*_tmp755;struct Cyc_Absyn_ArrayInfo _tmp757;void*_tmp758;struct Cyc_Absyn_Tqual _tmp759;struct Cyc_Absyn_Exp*_tmp75A;union Cyc_Absyn_Constraint*_tmp75B;void*_tmp75C;struct Cyc_Absyn_ArrayInfo _tmp75E;void*_tmp75F;struct Cyc_Absyn_Tqual _tmp760;struct Cyc_Absyn_Exp*_tmp761;union Cyc_Absyn_Constraint*_tmp762;void*_tmp763;void*_tmp765;_LL3AF: _tmp74F=_tmp74E.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp750=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp74F;if(_tmp750->tag != 5)goto _LL3B1;else{_tmp751=_tmp750->f1;}};_tmp752=_tmp74E.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp753=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp752;if(_tmp753->tag != 5)goto _LL3B1;else{_tmp754=_tmp753->f1;}};_LL3B0: {
# 1954
int okay=1;
# 1956
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp751.ptr_atts).nullable,(_tmp754.ptr_atts).nullable))
# 1959
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp751.ptr_atts).nullable);
# 1961
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp751.ptr_atts).bounds,(_tmp754.ptr_atts).bounds)){
# 1964
struct _tuple0 _tmp12BD;struct _tuple0 _tmp768=(_tmp12BD.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp751.ptr_atts).bounds),((_tmp12BD.f2=
# 1965
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp754.ptr_atts).bounds),_tmp12BD)));void*_tmp769;void*_tmp76B;void*_tmp76D;void*_tmp76F;void*_tmp771;struct Cyc_Absyn_Exp*_tmp773;void*_tmp774;struct Cyc_Absyn_Exp*_tmp776;void*_tmp777;void*_tmp779;struct Cyc_Absyn_Exp*_tmp77B;_LL3B8: _tmp769=_tmp768.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp76A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp769;if(_tmp76A->tag != 1)goto _LL3BA;};_tmp76B=_tmp768.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp76C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp76B;if(_tmp76C->tag != 0)goto _LL3BA;};_LL3B9:
# 1966
 goto _LL3BB;_LL3BA: _tmp76D=_tmp768.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp76E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp76D;if(_tmp76E->tag != 0)goto _LL3BC;};_tmp76F=_tmp768.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp770=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp76F;if(_tmp770->tag != 0)goto _LL3BC;};_LL3BB:
# 1967
 okay=1;goto _LL3B7;_LL3BC: _tmp771=_tmp768.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp772=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp771;if(_tmp772->tag != 1)goto _LL3BE;else{_tmp773=_tmp772->f1;}};_tmp774=_tmp768.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp775=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp774;if(_tmp775->tag != 1)goto _LL3BE;else{_tmp776=_tmp775->f1;}};_LL3BD:
# 1969
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp776,_tmp773);
# 1973
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp754.ptr_atts).zero_term)){
# 1974
const char*_tmp12C0;void*_tmp12BF;(_tmp12BF=0,Cyc_Tcutil_warn(loc,((_tmp12C0="implicit cast to shorter array",_tag_dyneither(_tmp12C0,sizeof(char),31))),_tag_dyneither(_tmp12BF,sizeof(void*),0)));}
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
const char*_tmp12C5;void*_tmp12C4[2];struct Cyc_String_pa_PrintArg_struct _tmp12C3;struct Cyc_String_pa_PrintArg_struct _tmp12C2;(_tmp12C2.tag=0,((_tmp12C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1995
Cyc_Absynpp_typ2string((_tmp754.ptr_atts).rgn)),((_tmp12C3.tag=0,((_tmp12C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1994
Cyc_Absynpp_typ2string((_tmp751.ptr_atts).rgn)),((_tmp12C4[0]=& _tmp12C3,((_tmp12C4[1]=& _tmp12C2,Cyc_Tcutil_warn(loc,((_tmp12C5="implicit cast form region %s to region %s",_tag_dyneither(_tmp12C5,sizeof(char),42))),_tag_dyneither(_tmp12C4,sizeof(void*),2)))))))))))));}}else{
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
const char*_tmp12C8;void*_tmp12C7;(_tmp12C7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12C8="pointer_elt_type",_tag_dyneither(_tmp12C8,sizeof(char),17))),_tag_dyneither(_tmp12C7,sizeof(void*),0)));}_LL3C5:;}
# 2049
void*Cyc_Tcutil_pointer_region(void*t){
# 2050
void*_tmp78B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp78D;struct Cyc_Absyn_PtrAtts _tmp78E;struct Cyc_Absyn_PtrAtts*_tmp78F;_LL3CB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp78C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp78B;if(_tmp78C->tag != 5)goto _LL3CD;else{_tmp78D=_tmp78C->f1;_tmp78E=_tmp78D.ptr_atts;_tmp78F=(struct Cyc_Absyn_PtrAtts*)&(_tmp78C->f1).ptr_atts;}}_LL3CC:
# 2051
 return _tmp78F->rgn;_LL3CD:;_LL3CE: {
# 2052
const char*_tmp12CB;void*_tmp12CA;(_tmp12CA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12CB="pointer_elt_type",_tag_dyneither(_tmp12CB,sizeof(char),17))),_tag_dyneither(_tmp12CA,sizeof(void*),0)));}_LL3CA:;}
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
const char*_tmp12CF;void*_tmp12CE[1];struct Cyc_String_pa_PrintArg_struct _tmp12CD;(_tmp12CD.tag=0,((_tmp12CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp12CE[0]=& _tmp12CD,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12CF="kind_to_opt: bad kind %s\n",_tag_dyneither(_tmp12CF,sizeof(char),26))),_tag_dyneither(_tmp12CE,sizeof(void*),1)))))));};}
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
const char*_tmp12D3;void*_tmp12D2[1];struct Cyc_String_pa_PrintArg_struct _tmp12D1;(_tmp12D1.tag=0,((_tmp12D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp12D2[0]=& _tmp12D1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12D3="kind_to_b: bad kind %s\n",_tag_dyneither(_tmp12D3,sizeof(char),24))),_tag_dyneither(_tmp12D2,sizeof(void*),1)))))));};}
# 2238
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
# 2239
return*Cyc_Tcutil_kind_to_b(k);}
# 2241
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2243
struct Cyc_Core_Opt*_tmp12D4;return(_tmp12D4=_cycalloc(sizeof(*_tmp12D4)),((_tmp12D4->v=Cyc_Tcutil_kind_to_bound(k),_tmp12D4)));}
# 2248
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
# 2249
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
# 2250
{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp12D7;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp12D6;e1->r=(void*)((_tmp12D6=_cycalloc(sizeof(*_tmp12D6)),((_tmp12D6[0]=((_tmp12D7.tag=0,((_tmp12D7.f1=Cyc_Absyn_Null_c,_tmp12D7)))),_tmp12D6))));}{
# 2251
struct Cyc_Core_Opt*_tmp7D3=Cyc_Tcenv_lookup_opt_type_vars(te);
# 2252
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp12E1;struct Cyc_Absyn_PtrAtts _tmp12E0;struct Cyc_Absyn_PtrInfo _tmp12DF;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp12DE;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp7D4=(_tmp12DE=_cycalloc(sizeof(*_tmp12DE)),((_tmp12DE[0]=((_tmp12E1.tag=5,((_tmp12E1.f1=((_tmp12DF.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ako,_tmp7D3),((_tmp12DF.elt_tq=
# 2253
Cyc_Absyn_empty_tqual(0),((_tmp12DF.ptr_atts=(
# 2254
(_tmp12E0.rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,_tmp7D3),((_tmp12E0.nullable=Cyc_Absyn_true_conref,((_tmp12E0.bounds=
# 2256
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp12E0.zero_term=
# 2257
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp12E0.ptrloc=0,_tmp12E0)))))))))),_tmp12DF)))))),_tmp12E1)))),_tmp12DE)));
# 2258
e1->topt=(void*)_tmp7D4;{
# 2259
int bogus=0;
# 2260
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
# 2261
if(bogus != 0){
# 2262
const char*_tmp12E6;void*_tmp12E5[2];struct Cyc_String_pa_PrintArg_struct _tmp12E4;struct Cyc_String_pa_PrintArg_struct _tmp12E3;(_tmp12E3.tag=0,((_tmp12E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2263
Cyc_Position_string_of_segment(e1->loc)),((_tmp12E4.tag=0,((_tmp12E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp12E5[0]=& _tmp12E4,((_tmp12E5[1]=& _tmp12E3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12E6="zero_to_null resulted in an alias coercion on %s at %s\n",_tag_dyneither(_tmp12E6,sizeof(char),56))),_tag_dyneither(_tmp12E5,sizeof(void*),2)))))))))))));}
# 2264
return retv;};};}
# 2266
return 0;}
# 2269
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
# 2270
switch(c){case Cyc_Absyn_Unknown_coercion: _LL416: {
# 2271
const char*_tmp12E7;return(_tmp12E7="unknown",_tag_dyneither(_tmp12E7,sizeof(char),8));}case Cyc_Absyn_No_coercion: _LL417: {
# 2272
const char*_tmp12E8;return(_tmp12E8="no coercion",_tag_dyneither(_tmp12E8,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL418: {
# 2273
const char*_tmp12E9;return(_tmp12E9="null check",_tag_dyneither(_tmp12E9,sizeof(char),11));}case Cyc_Absyn_Other_coercion: _LL419: {
# 2274
const char*_tmp12EA;return(_tmp12EA="other coercion",_tag_dyneither(_tmp12EA,sizeof(char),15));}}}
# 2278
int Cyc_Tcutil_warn_alias_coerce=0;
# 2284
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
# 2285
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2288
static int counter=0;
# 2289
struct _dyneither_ptr*_tmp12F7;const char*_tmp12F6;void*_tmp12F5[1];struct Cyc_Int_pa_PrintArg_struct _tmp12F4;struct _tuple2*_tmp12F3;struct _tuple2*v=(_tmp12F3=_cycalloc(sizeof(*_tmp12F3)),((_tmp12F3->f1=Cyc_Absyn_Loc_n,((_tmp12F3->f2=((_tmp12F7=_cycalloc(sizeof(*_tmp12F7)),((_tmp12F7[0]=(struct _dyneither_ptr)((_tmp12F4.tag=1,((_tmp12F4.f1=(unsigned long)counter ++,((_tmp12F5[0]=& _tmp12F4,Cyc_aprintf(((_tmp12F6="__aliasvar%d",_tag_dyneither(_tmp12F6,sizeof(char),13))),_tag_dyneither(_tmp12F5,sizeof(void*),1)))))))),_tmp12F7)))),_tmp12F3)))));
# 2290
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,(void*)& Cyc_Absyn_VoidType_val,0);
# 2291
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp12FA;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp12F9;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmp12F9=_cycalloc(sizeof(*_tmp12F9)),((_tmp12F9[0]=((_tmp12FA.tag=4,((_tmp12FA.f1=vd,_tmp12FA)))),_tmp12F9)))),e->loc);
# 2294
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2295
{void*_tmp7E1=Cyc_Tcutil_compress(e_typ);struct Cyc_Absyn_PtrInfo _tmp7E3;struct Cyc_Absyn_PtrAtts _tmp7E4;void*_tmp7E5;void**_tmp7E6;_LL41C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp7E2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7E1;if(_tmp7E2->tag != 5)goto _LL41E;else{_tmp7E3=_tmp7E2->f1;_tmp7E4=_tmp7E3.ptr_atts;_tmp7E5=_tmp7E4.rgn;_tmp7E6=(void**)&((_tmp7E2->f1).ptr_atts).rgn;}}_LL41D:
# 2297
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp12FD;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp12FC;*_tmp7E6=(void*)((_tmp12FC=_cycalloc(sizeof(*_tmp12FC)),((_tmp12FC[0]=((_tmp12FD.tag=2,((_tmp12FD.f1=tv,_tmp12FD)))),_tmp12FC))));}goto _LL41B;_LL41E:;_LL41F:
# 2298
 goto _LL41B;_LL41B:;}
# 2300
e->topt=0;
# 2301
e=Cyc_Absyn_cast_exp(e_typ,e,0,Cyc_Absyn_Unknown_coercion,e->loc);{
# 2304
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(e,tv,vd,e->loc);
# 2306
struct _tuple15 _tmp12FE;return(_tmp12FE.f1=d,((_tmp12FE.f2=ve,_tmp12FE)));};}
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
const char*_tmp1303;void*_tmp1302[2];struct Cyc_String_pa_PrintArg_struct _tmp1301;struct Cyc_String_pa_PrintArg_struct _tmp1300;(_tmp1300.tag=0,((_tmp1300.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2329
Cyc_Position_string_of_segment(loc)),((_tmp1301.tag=0,((_tmp1301.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ))),((_tmp1302[0]=& _tmp1301,((_tmp1302[1]=& _tmp1300,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1303="got typedef %s in can_insert_alias at %s\n",_tag_dyneither(_tmp1303,sizeof(char),42))),_tag_dyneither(_tmp1302,sizeof(void*),2)))))))))))));}_LL425:;_LL426: {
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
const char*_tmp1308;void*_tmp1307[2];struct Cyc_String_pa_PrintArg_struct _tmp1306;struct Cyc_String_pa_PrintArg_struct _tmp1305;(_tmp1305.tag=0,((_tmp1305.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2348
Cyc_Absynpp_typ2string(t2)),((_tmp1306.tag=0,((_tmp1306.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp1307[0]=& _tmp1306,((_tmp1307[1]=& _tmp1305,Cyc_Tcutil_warn(e->loc,((_tmp1308="integral size mismatch; %s -> %s conversion supplied",_tag_dyneither(_tmp1308,sizeof(char),53))),_tag_dyneither(_tmp1307,sizeof(void*),2)))))))))))));}
# 2349
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
# 2350
return 1;}else{
# 2355
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
# 2356
if(Cyc_Tcutil_warn_alias_coerce){
# 2357
const char*_tmp130E;void*_tmp130D[3];struct Cyc_String_pa_PrintArg_struct _tmp130C;struct Cyc_String_pa_PrintArg_struct _tmp130B;struct Cyc_String_pa_PrintArg_struct _tmp130A;(_tmp130A.tag=0,((_tmp130A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2358
Cyc_Absynpp_typ2string(t2)),((_tmp130B.tag=0,((_tmp130B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp130C.tag=0,((_tmp130C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp130D[0]=& _tmp130C,((_tmp130D[1]=& _tmp130B,((_tmp130D[2]=& _tmp130A,Cyc_Tcutil_warn(e->loc,((_tmp130E="implicit alias coercion for %s:%s to %s",_tag_dyneither(_tmp130E,sizeof(char),40))),_tag_dyneither(_tmp130D,sizeof(void*),3)))))))))))))))))));}
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
# 2370
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
# 2372
if(c != Cyc_Absyn_NonNull_to_Null){
# 2373
const char*_tmp1313;void*_tmp1312[2];struct Cyc_String_pa_PrintArg_struct _tmp1311;struct Cyc_String_pa_PrintArg_struct _tmp1310;(_tmp1310.tag=0,((_tmp1310.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2374
Cyc_Absynpp_typ2string(t2)),((_tmp1311.tag=0,((_tmp1311.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp1312[0]=& _tmp1311,((_tmp1312[1]=& _tmp1310,Cyc_Tcutil_warn(e->loc,((_tmp1313="implicit cast from %s to %s",_tag_dyneither(_tmp1313,sizeof(char),28))),_tag_dyneither(_tmp1312,sizeof(void*),2)))))))))))));}
# 2375
return 1;}else{
# 2377
return 0;}}}}}
# 2384
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2387
int bogus=0;
# 2388
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2391
int Cyc_Tcutil_coerceable(void*t){
# 2392
void*_tmp811=Cyc_Tcutil_compress(t);_LL42D: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp812=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp811;if(_tmp812->tag != 6)goto _LL42F;}_LL42E:
# 2393
 goto _LL430;_LL42F: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp813=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp811;if(_tmp813->tag != 7)goto _LL431;}_LL430:
# 2394
 return 1;_LL431:;_LL432:
# 2395
 return 0;_LL42C:;}
# 2409 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2413
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2416
struct Cyc_List_List*_tmp815;struct _RegionHandle*_tmp816;struct Cyc_Tcenv_Tenv*_tmp817;int _tmp818;struct _tuple23 _tmp814=*env;_tmp815=_tmp814.f1;_tmp816=_tmp814.f2;_tmp817=_tmp814.f3;_tmp818=_tmp814.f4;{
# 2418
void*_tmp819=_tmp815 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp816,_tmp815,x->type);
# 2419
struct Cyc_List_List*_tmp81A=Cyc_Tcutil_flatten_typ(_tmp816,_tmp818,_tmp817,_tmp819);
# 2420
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp81A)== 1){
# 2421
struct _tuple12*_tmp1316;struct Cyc_List_List*_tmp1315;return(_tmp1315=_region_malloc(_tmp816,sizeof(*_tmp1315)),((_tmp1315->hd=((_tmp1316=_region_malloc(_tmp816,sizeof(*_tmp1316)),((_tmp1316->f1=x->tq,((_tmp1316->f2=_tmp819,_tmp1316)))))),((_tmp1315->tl=0,_tmp1315)))));}else{
# 2422
return _tmp81A;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2424
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2426
struct _RegionHandle*_tmp81E;struct Cyc_Tcenv_Tenv*_tmp81F;int _tmp820;struct _tuple24 _tmp81D=*env;_tmp81E=_tmp81D.f1;_tmp81F=_tmp81D.f2;_tmp820=_tmp81D.f3;{
# 2427
struct Cyc_Absyn_Tqual _tmp822;void*_tmp823;struct _tuple12 _tmp821=*x;_tmp822=_tmp821.f1;_tmp823=_tmp821.f2;{
# 2428
struct Cyc_List_List*_tmp824=Cyc_Tcutil_flatten_typ(_tmp81E,_tmp820,_tmp81F,_tmp823);
# 2429
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp824)== 1){
# 2430
struct _tuple12*_tmp1319;struct Cyc_List_List*_tmp1318;return(_tmp1318=_region_malloc(_tmp81E,sizeof(*_tmp1318)),((_tmp1318->hd=((_tmp1319=_region_malloc(_tmp81E,sizeof(*_tmp1319)),((_tmp1319->f1=_tmp822,((_tmp1319->f2=_tmp823,_tmp1319)))))),((_tmp1318->tl=0,_tmp1318)))));}else{
# 2431
return _tmp824;}};};}
# 2433
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2437
if(flatten){
# 2438
t1=Cyc_Tcutil_compress(t1);{
# 2439
void*_tmp827=t1;struct Cyc_List_List*_tmp82A;struct Cyc_Absyn_AggrInfo _tmp82C;union Cyc_Absyn_AggrInfoU _tmp82D;struct Cyc_Absyn_Aggrdecl**_tmp82E;struct Cyc_Absyn_Aggrdecl*_tmp82F;struct Cyc_List_List*_tmp830;enum Cyc_Absyn_AggrKind _tmp832;struct Cyc_List_List*_tmp833;_LL434: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp828=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp827;if(_tmp828->tag != 0)goto _LL436;}_LL435:
# 2440
 return 0;_LL436: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp829=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp827;if(_tmp829->tag != 10)goto _LL438;else{_tmp82A=_tmp829->f1;}}_LL437: {
# 2442
struct _tuple24 _tmp131A;struct _tuple24 _tmp834=(_tmp131A.f1=r,((_tmp131A.f2=te,((_tmp131A.f3=flatten,_tmp131A)))));
# 2444
struct Cyc_List_List*_tmp835=_tmp82A;struct Cyc_List_List _tmp836;struct _tuple12*_tmp837;struct Cyc_List_List*_tmp838;_LL43F: if(_tmp835 != 0)goto _LL441;_LL440:
# 2445
 return 0;_LL441: if(_tmp835 == 0)goto _LL43E;_tmp836=*_tmp835;_tmp837=(struct _tuple12*)_tmp836.hd;_tmp838=_tmp836.tl;_LL442: {
# 2447
struct Cyc_List_List*_tmp839=Cyc_Tcutil_rcopy_tqt(& _tmp834,_tmp837);
# 2448
_tmp834.f3=0;{
# 2449
struct Cyc_List_List*_tmp83A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp834,_tmp82A);
# 2450
struct Cyc_List_List*_tmp131B;struct Cyc_List_List*_tmp83B=(_tmp131B=_region_malloc(r,sizeof(*_tmp131B)),((_tmp131B->hd=_tmp839,((_tmp131B->tl=_tmp83A,_tmp131B)))));
# 2451
struct Cyc_List_List*_tmp131C;return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp131C=_region_malloc(r,sizeof(*_tmp131C)),((_tmp131C->hd=_tmp839,((_tmp131C->tl=_tmp83A,_tmp131C)))))));};}_LL43E:;}_LL438: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp82B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp827;if(_tmp82B->tag != 11)goto _LL43A;else{_tmp82C=_tmp82B->f1;_tmp82D=_tmp82C.aggr_info;if((_tmp82D.KnownAggr).tag != 2)goto _LL43A;_tmp82E=(struct Cyc_Absyn_Aggrdecl**)(_tmp82D.KnownAggr).val;_tmp82F=*_tmp82E;_tmp830=_tmp82C.targs;}}_LL439:
# 2455
 if(((_tmp82F->kind == Cyc_Absyn_UnionA  || _tmp82F->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp82F->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp82F->impl))->rgn_po != 0){
# 2457
struct _tuple12*_tmp131F;struct Cyc_List_List*_tmp131E;return(_tmp131E=_region_malloc(r,sizeof(*_tmp131E)),((_tmp131E->hd=((_tmp131F=_region_malloc(r,sizeof(*_tmp131F)),((_tmp131F->f1=Cyc_Absyn_empty_tqual(0),((_tmp131F->f2=t1,_tmp131F)))))),((_tmp131E->tl=0,_tmp131E)))));}{
# 2458
struct Cyc_List_List*_tmp841=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp82F->tvs,_tmp830);
# 2459
struct _tuple23 _tmp1320;struct _tuple23 env=(_tmp1320.f1=_tmp841,((_tmp1320.f2=r,((_tmp1320.f3=te,((_tmp1320.f4=flatten,_tmp1320)))))));
# 2460
struct Cyc_List_List*_tmp842=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp82F->impl))->fields;struct Cyc_List_List _tmp843;struct Cyc_Absyn_Aggrfield*_tmp844;struct Cyc_List_List*_tmp845;_LL444: if(_tmp842 != 0)goto _LL446;_LL445:
# 2461
 return 0;_LL446: if(_tmp842 == 0)goto _LL443;_tmp843=*_tmp842;_tmp844=(struct Cyc_Absyn_Aggrfield*)_tmp843.hd;_tmp845=_tmp843.tl;_LL447: {
# 2463
struct Cyc_List_List*_tmp846=Cyc_Tcutil_flatten_typ_f(& env,_tmp844);
# 2464
env.f4=0;{
# 2465
struct Cyc_List_List*_tmp847=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp845);
# 2466
struct Cyc_List_List*_tmp1321;struct Cyc_List_List*_tmp848=(_tmp1321=_region_malloc(r,sizeof(*_tmp1321)),((_tmp1321->hd=_tmp846,((_tmp1321->tl=_tmp847,_tmp1321)))));
# 2467
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp848);};}_LL443:;};_LL43A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp831=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp827;if(_tmp831->tag != 12)goto _LL43C;else{_tmp832=_tmp831->f1;if(_tmp832 != Cyc_Absyn_StructA)goto _LL43C;_tmp833=_tmp831->f2;}}_LL43B: {
# 2470
struct _tuple23 _tmp1322;struct _tuple23 env=(_tmp1322.f1=0,((_tmp1322.f2=r,((_tmp1322.f3=te,((_tmp1322.f4=flatten,_tmp1322)))))));
# 2471
struct Cyc_List_List*_tmp84B=_tmp833;struct Cyc_List_List _tmp84C;struct Cyc_Absyn_Aggrfield*_tmp84D;struct Cyc_List_List*_tmp84E;_LL449: if(_tmp84B != 0)goto _LL44B;_LL44A:
# 2472
 return 0;_LL44B: if(_tmp84B == 0)goto _LL448;_tmp84C=*_tmp84B;_tmp84D=(struct Cyc_Absyn_Aggrfield*)_tmp84C.hd;_tmp84E=_tmp84C.tl;_LL44C: {
# 2474
struct Cyc_List_List*_tmp84F=Cyc_Tcutil_flatten_typ_f(& env,_tmp84D);
# 2475
env.f4=0;{
# 2476
struct Cyc_List_List*_tmp850=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp84E);
# 2477
struct Cyc_List_List*_tmp1323;struct Cyc_List_List*_tmp851=(_tmp1323=_region_malloc(r,sizeof(*_tmp1323)),((_tmp1323->hd=_tmp84F,((_tmp1323->tl=_tmp850,_tmp1323)))));
# 2478
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp851);};}_LL448:;}_LL43C:;_LL43D:
# 2480
 goto _LL433;_LL433:;};}{
# 2483
struct _tuple12*_tmp1326;struct Cyc_List_List*_tmp1325;return(_tmp1325=_region_malloc(r,sizeof(*_tmp1325)),((_tmp1325->hd=((_tmp1326=_region_malloc(r,sizeof(*_tmp1326)),((_tmp1326->f1=Cyc_Absyn_empty_tqual(0),((_tmp1326->f2=t1,_tmp1326)))))),((_tmp1325->tl=0,_tmp1325)))));};}
# 2487
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
# 2488
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
# 2489
void*_tmp856=(void*)t->hd;_LL44E: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp857=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp856;if(_tmp857->tag != 23)goto _LL450;}_LL44F:
# 2490
 goto _LL451;_LL450: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp858=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp856;if(_tmp858->tag != 4)goto _LL452;}_LL451:
# 2491
 goto _LL453;_LL452: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp859=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp856;if(_tmp859->tag != 20)goto _LL454;}_LL453:
# 2493
 continue;_LL454:;_LL455:
# 2495
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;_LL44D:;}}
# 2498
for(0;a2 != 0;a2=a2->tl){
# 2499
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2501
return 1;}
# 2504
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
# 2505
struct _tuple0 _tmp1327;struct _tuple0 _tmp85B=(_tmp1327.f1=Cyc_Tcutil_compress(t1),((_tmp1327.f2=Cyc_Tcutil_compress(t2),_tmp1327)));void*_tmp85C;enum Cyc_Absyn_Size_of _tmp85E;void*_tmp85F;enum Cyc_Absyn_Size_of _tmp861;_LL457: _tmp85C=_tmp85B.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp85D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp85C;if(_tmp85D->tag != 6)goto _LL459;else{_tmp85E=_tmp85D->f2;}};_tmp85F=_tmp85B.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp860=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp85F;if(_tmp860->tag != 6)goto _LL459;else{_tmp861=_tmp860->f2;}};_LL458:
# 2507
 return(_tmp85E == _tmp861  || _tmp85E == Cyc_Absyn_Int_sz  && _tmp861 == Cyc_Absyn_Long_sz) || 
# 2508
_tmp85E == Cyc_Absyn_Long_sz  && _tmp861 == Cyc_Absyn_Int_sz;_LL459:;_LL45A:
# 2509
 return 0;_LL456:;}
# 2515
static int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2518
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2519
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
# 2520
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
# 2521
return 1;}}
# 2523
t1=Cyc_Tcutil_compress(t1);
# 2524
t2=Cyc_Tcutil_compress(t2);{
# 2525
struct _tuple0 _tmp1328;struct _tuple0 _tmp863=(_tmp1328.f1=t1,((_tmp1328.f2=t2,_tmp1328)));void*_tmp864;struct Cyc_Absyn_PtrInfo _tmp866;void*_tmp867;struct Cyc_Absyn_Tqual _tmp868;struct Cyc_Absyn_PtrAtts _tmp869;void*_tmp86A;union Cyc_Absyn_Constraint*_tmp86B;union Cyc_Absyn_Constraint*_tmp86C;union Cyc_Absyn_Constraint*_tmp86D;void*_tmp86E;struct Cyc_Absyn_PtrInfo _tmp870;void*_tmp871;struct Cyc_Absyn_Tqual _tmp872;struct Cyc_Absyn_PtrAtts _tmp873;void*_tmp874;union Cyc_Absyn_Constraint*_tmp875;union Cyc_Absyn_Constraint*_tmp876;union Cyc_Absyn_Constraint*_tmp877;void*_tmp878;struct Cyc_Absyn_DatatypeFieldInfo _tmp87A;union Cyc_Absyn_DatatypeFieldInfoU _tmp87B;struct _tuple3 _tmp87C;struct Cyc_Absyn_Datatypedecl*_tmp87D;struct Cyc_Absyn_Datatypefield*_tmp87E;struct Cyc_List_List*_tmp87F;void*_tmp880;struct Cyc_Absyn_DatatypeInfo _tmp882;union Cyc_Absyn_DatatypeInfoU _tmp883;struct Cyc_Absyn_Datatypedecl**_tmp884;struct Cyc_Absyn_Datatypedecl*_tmp885;struct Cyc_List_List*_tmp886;void*_tmp887;struct Cyc_Absyn_FnInfo _tmp889;void*_tmp88A;struct Cyc_Absyn_FnInfo _tmp88C;_LL45C: _tmp864=_tmp863.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp865=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp864;if(_tmp865->tag != 5)goto _LL45E;else{_tmp866=_tmp865->f1;_tmp867=_tmp866.elt_typ;_tmp868=_tmp866.elt_tq;_tmp869=_tmp866.ptr_atts;_tmp86A=_tmp869.rgn;_tmp86B=_tmp869.nullable;_tmp86C=_tmp869.bounds;_tmp86D=_tmp869.zero_term;}};_tmp86E=_tmp863.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp86F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp86E;if(_tmp86F->tag != 5)goto _LL45E;else{_tmp870=_tmp86F->f1;_tmp871=_tmp870.elt_typ;_tmp872=_tmp870.elt_tq;_tmp873=_tmp870.ptr_atts;_tmp874=_tmp873.rgn;_tmp875=_tmp873.nullable;_tmp876=_tmp873.bounds;_tmp877=_tmp873.zero_term;}};_LL45D:
# 2531
 if(_tmp868.real_const  && !_tmp872.real_const)
# 2532
return 0;
# 2534
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp86B,_tmp875) && 
# 2535
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp86B)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp875))
# 2536
return 0;
# 2538
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp86D,_tmp877) && !
# 2539
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp86D)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp877))
# 2540
return 0;
# 2542
if(!Cyc_Tcutil_unify(_tmp86A,_tmp874) && !Cyc_Tcenv_region_outlives(te,_tmp86A,_tmp874))
# 2543
return 0;
# 2545
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp86C,_tmp876)){
# 2546
struct _tuple0 _tmp1329;struct _tuple0 _tmp88E=(_tmp1329.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp86C),((_tmp1329.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp876),_tmp1329)));void*_tmp88F;void*_tmp891;void*_tmp893;struct Cyc_Absyn_Exp*_tmp895;void*_tmp896;struct Cyc_Absyn_Exp*_tmp898;_LL465: _tmp88F=_tmp88E.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp890=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp88F;if(_tmp890->tag != 1)goto _LL467;};_tmp891=_tmp88E.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp892=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp891;if(_tmp892->tag != 0)goto _LL467;};_LL466:
# 2547
 goto _LL464;_LL467: _tmp893=_tmp88E.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp894=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp893;if(_tmp894->tag != 1)goto _LL469;else{_tmp895=_tmp894->f1;}};_tmp896=_tmp88E.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp897=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp896;if(_tmp897->tag != 1)goto _LL469;else{_tmp898=_tmp897->f1;}};_LL468:
# 2549
 if(!Cyc_Evexp_lte_const_exp(_tmp898,_tmp895))
# 2550
return 0;
# 2551
goto _LL464;_LL469:;_LL46A:
# 2552
 return 0;_LL464:;}
# 2557
if(!_tmp872.real_const  && _tmp868.real_const){
# 2558
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp871)))
# 2559
return 0;}{
# 2563
int _tmp899=
# 2564
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp876,Cyc_Absyn_bounds_one_conref) && !
# 2565
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp877);
# 2569
struct _tuple0*_tmp132C;struct Cyc_List_List*_tmp132B;return(_tmp899  && Cyc_Tcutil_ptrsubtype(te,((_tmp132B=_cycalloc(sizeof(*_tmp132B)),((_tmp132B->hd=((_tmp132C=_cycalloc(sizeof(*_tmp132C)),((_tmp132C->f1=t1,((_tmp132C->f2=t2,_tmp132C)))))),((_tmp132B->tl=assume,_tmp132B)))))),_tmp867,_tmp871) || Cyc_Tcutil_unify(t1,t2)) || Cyc_Tcutil_isomorphic(t1,t2);};_LL45E: _tmp878=_tmp863.f1;{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp879=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp878;if(_tmp879->tag != 4)goto _LL460;else{_tmp87A=_tmp879->f1;_tmp87B=_tmp87A.field_info;if((_tmp87B.KnownDatatypefield).tag != 2)goto _LL460;_tmp87C=(struct _tuple3)(_tmp87B.KnownDatatypefield).val;_tmp87D=_tmp87C.f1;_tmp87E=_tmp87C.f2;_tmp87F=_tmp87A.targs;}};_tmp880=_tmp863.f2;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp881=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp880;if(_tmp881->tag != 3)goto _LL460;else{_tmp882=_tmp881->f1;_tmp883=_tmp882.datatype_info;if((_tmp883.KnownDatatype).tag != 2)goto _LL460;_tmp884=(struct Cyc_Absyn_Datatypedecl**)(_tmp883.KnownDatatype).val;_tmp885=*_tmp884;_tmp886=_tmp882.targs;}};_LL45F:
# 2575
 if(_tmp87D != _tmp885  && Cyc_Absyn_qvar_cmp(_tmp87D->name,_tmp885->name)!= 0)return 0;
# 2577
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp87F)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp886))return 0;
# 2578
for(0;_tmp87F != 0;(_tmp87F=_tmp87F->tl,_tmp886=_tmp886->tl)){
# 2579
if(!Cyc_Tcutil_unify((void*)_tmp87F->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp886))->hd))return 0;}
# 2580
return 1;_LL460: _tmp887=_tmp863.f1;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp888=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp887;if(_tmp888->tag != 9)goto _LL462;else{_tmp889=_tmp888->f1;}};_tmp88A=_tmp863.f2;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp88B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp88A;if(_tmp88B->tag != 9)goto _LL462;else{_tmp88C=_tmp88B->f1;}};_LL461:
# 2584
 if(_tmp889.tvars != 0  || _tmp88C.tvars != 0){
# 2585
struct Cyc_List_List*_tmp89C=_tmp889.tvars;
# 2586
struct Cyc_List_List*_tmp89D=_tmp88C.tvars;
# 2587
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp89C)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp89D))return 0;{
# 2588
struct _RegionHandle _tmp89E=_new_region("r");struct _RegionHandle*r=& _tmp89E;_push_region(r);
# 2589
{struct Cyc_List_List*inst=0;
# 2590
while(_tmp89C != 0){
# 2591
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp89C->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp89D))->hd)->kind)){int _tmp89F=0;_npop_handler(0);return _tmp89F;}
# 2592
{struct _tuple16*_tmp1336;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1335;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1334;struct Cyc_List_List*_tmp1333;inst=((_tmp1333=_region_malloc(r,sizeof(*_tmp1333)),((_tmp1333->hd=((_tmp1336=_region_malloc(r,sizeof(*_tmp1336)),((_tmp1336->f1=(struct Cyc_Absyn_Tvar*)_tmp89D->hd,((_tmp1336->f2=(void*)((_tmp1334=_cycalloc(sizeof(*_tmp1334)),((_tmp1334[0]=((_tmp1335.tag=2,((_tmp1335.f1=(struct Cyc_Absyn_Tvar*)_tmp89C->hd,_tmp1335)))),_tmp1334)))),_tmp1336)))))),((_tmp1333->tl=inst,_tmp1333))))));}
# 2593
_tmp89C=_tmp89C->tl;
# 2594
_tmp89D=_tmp89D->tl;}
# 2596
if(inst != 0){
# 2597
_tmp889.tvars=0;
# 2598
_tmp88C.tvars=0;{
# 2599
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp133C;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp133B;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1339;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1338;int _tmp8A8=Cyc_Tcutil_subtype(te,assume,(void*)((_tmp1338=_cycalloc(sizeof(*_tmp1338)),((_tmp1338[0]=((_tmp1339.tag=9,((_tmp1339.f1=_tmp889,_tmp1339)))),_tmp1338)))),(void*)((_tmp133B=_cycalloc(sizeof(*_tmp133B)),((_tmp133B[0]=((_tmp133C.tag=9,((_tmp133C.f1=_tmp88C,_tmp133C)))),_tmp133B)))));_npop_handler(0);return _tmp8A8;};}}
# 2589
;_pop_region(r);};}
# 2604
if(!Cyc_Tcutil_subtype(te,assume,_tmp889.ret_typ,_tmp88C.ret_typ))return 0;{
# 2605
struct Cyc_List_List*_tmp8A9=_tmp889.args;
# 2606
struct Cyc_List_List*_tmp8AA=_tmp88C.args;
# 2609
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8A9)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8AA))return 0;
# 2611
for(0;_tmp8A9 != 0;(_tmp8A9=_tmp8A9->tl,_tmp8AA=_tmp8AA->tl)){
# 2612
struct Cyc_Absyn_Tqual _tmp8AC;void*_tmp8AD;struct _tuple10 _tmp8AB=*((struct _tuple10*)_tmp8A9->hd);_tmp8AC=_tmp8AB.f2;_tmp8AD=_tmp8AB.f3;{
# 2613
struct Cyc_Absyn_Tqual _tmp8AF;void*_tmp8B0;struct _tuple10 _tmp8AE=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp8AA))->hd);_tmp8AF=_tmp8AE.f2;_tmp8B0=_tmp8AE.f3;
# 2615
if(_tmp8AF.real_const  && !_tmp8AC.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp8B0,_tmp8AD))
# 2616
return 0;};}
# 2619
if(_tmp889.c_varargs != _tmp88C.c_varargs)return 0;
# 2620
if(_tmp889.cyc_varargs != 0  && _tmp88C.cyc_varargs != 0){
# 2621
struct Cyc_Absyn_VarargInfo _tmp8B1=*_tmp889.cyc_varargs;
# 2622
struct Cyc_Absyn_VarargInfo _tmp8B2=*_tmp88C.cyc_varargs;
# 2624
if((_tmp8B2.tq).real_const  && !(_tmp8B1.tq).real_const  || !
# 2625
Cyc_Tcutil_subtype(te,assume,_tmp8B2.type,_tmp8B1.type))
# 2626
return 0;}else{
# 2627
if(_tmp889.cyc_varargs != 0  || _tmp88C.cyc_varargs != 0)return 0;}
# 2629
if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp889.effect),(void*)_check_null(_tmp88C.effect)))return 0;
# 2631
if(!Cyc_Tcutil_sub_rgnpo(_tmp889.rgn_po,_tmp88C.rgn_po))return 0;
# 2633
if(!Cyc_Tcutil_sub_attributes(_tmp889.attributes,_tmp88C.attributes))return 0;
# 2635
return 1;};_LL462:;_LL463:
# 2636
 return 0;_LL45B:;};}
# 2647 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2649
struct _RegionHandle _tmp8B3=_new_region("temp");struct _RegionHandle*temp=& _tmp8B3;_push_region(temp);
# 2650
{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,1,te,t1);
# 2651
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);
# 2652
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
# 2653
if(tqs1 == 0){int _tmp8B4=0;_npop_handler(0);return _tmp8B4;}{
# 2654
struct _tuple12 _tmp8B6;struct Cyc_Absyn_Tqual _tmp8B7;void*_tmp8B8;struct _tuple12*_tmp8B5=(struct _tuple12*)tqs1->hd;_tmp8B6=*_tmp8B5;_tmp8B7=_tmp8B6.f1;_tmp8B8=_tmp8B6.f2;{
# 2655
struct _tuple12 _tmp8BA;struct Cyc_Absyn_Tqual _tmp8BB;void*_tmp8BC;struct _tuple12*_tmp8B9=(struct _tuple12*)tqs2->hd;_tmp8BA=*_tmp8B9;_tmp8BB=_tmp8BA.f1;_tmp8BC=_tmp8BA.f2;
# 2657
if(_tmp8B7.real_const  && !_tmp8BB.real_const){int _tmp8BD=0;_npop_handler(0);return _tmp8BD;}
# 2659
if((_tmp8BB.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp8BC))) && 
# 2660
Cyc_Tcutil_subtype(te,assume,_tmp8B8,_tmp8BC))
# 2662
continue;
# 2664
if(Cyc_Tcutil_unify(_tmp8B8,_tmp8BC))
# 2666
continue;
# 2668
if(Cyc_Tcutil_isomorphic(_tmp8B8,_tmp8BC))
# 2670
continue;{
# 2673
int _tmp8BE=0;_npop_handler(0);return _tmp8BE;};};};}{
# 2675
int _tmp8BF=1;_npop_handler(0);return _tmp8BF;};}
# 2650
;_pop_region(temp);}
# 2679
static int Cyc_Tcutil_is_char_type(void*t){
# 2680
void*_tmp8C0=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Size_of _tmp8C2;_LL46C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8C1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8C0;if(_tmp8C1->tag != 6)goto _LL46E;else{_tmp8C2=_tmp8C1->f2;if(_tmp8C2 != Cyc_Absyn_Char_sz)goto _LL46E;}}_LL46D:
# 2681
 return 1;_LL46E:;_LL46F:
# 2682
 return 0;_LL46B:;}
# 2688
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2689
if(Cyc_Tcutil_unify(t1,t2))
# 2690
return Cyc_Absyn_No_coercion;
# 2691
t1=Cyc_Tcutil_compress(t1);
# 2692
t2=Cyc_Tcutil_compress(t2);
# 2694
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
# 2695
return Cyc_Absyn_No_coercion;
# 2696
{void*_tmp8C3=t2;enum Cyc_Absyn_Size_of _tmp8C5;enum Cyc_Absyn_Size_of _tmp8C7;_LL471: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8C4=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8C3;if(_tmp8C4->tag != 6)goto _LL473;else{_tmp8C5=_tmp8C4->f2;if(_tmp8C5 != Cyc_Absyn_Int_sz)goto _LL473;}}_LL472:
# 2698
 goto _LL474;_LL473: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8C6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8C3;if(_tmp8C6->tag != 6)goto _LL475;else{_tmp8C7=_tmp8C6->f2;if(_tmp8C7 != Cyc_Absyn_Long_sz)goto _LL475;}}_LL474:
# 2700
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
# 2701
goto _LL470;_LL475:;_LL476:
# 2702
 goto _LL470;_LL470:;}{
# 2704
void*_tmp8C8=t1;struct Cyc_Absyn_PtrInfo _tmp8CA;void*_tmp8CB;struct Cyc_Absyn_Tqual _tmp8CC;struct Cyc_Absyn_PtrAtts _tmp8CD;void*_tmp8CE;union Cyc_Absyn_Constraint*_tmp8CF;union Cyc_Absyn_Constraint*_tmp8D0;union Cyc_Absyn_Constraint*_tmp8D1;struct Cyc_Absyn_ArrayInfo _tmp8D3;void*_tmp8D4;struct Cyc_Absyn_Tqual _tmp8D5;struct Cyc_Absyn_Exp*_tmp8D6;union Cyc_Absyn_Constraint*_tmp8D7;struct Cyc_Absyn_Enumdecl*_tmp8D9;void*_tmp8DD;_LL478: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8C9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8C8;if(_tmp8C9->tag != 5)goto _LL47A;else{_tmp8CA=_tmp8C9->f1;_tmp8CB=_tmp8CA.elt_typ;_tmp8CC=_tmp8CA.elt_tq;_tmp8CD=_tmp8CA.ptr_atts;_tmp8CE=_tmp8CD.rgn;_tmp8CF=_tmp8CD.nullable;_tmp8D0=_tmp8CD.bounds;_tmp8D1=_tmp8CD.zero_term;}}_LL479:
# 2712
{void*_tmp8DE=t2;struct Cyc_Absyn_PtrInfo _tmp8E0;void*_tmp8E1;struct Cyc_Absyn_Tqual _tmp8E2;struct Cyc_Absyn_PtrAtts _tmp8E3;void*_tmp8E4;union Cyc_Absyn_Constraint*_tmp8E5;union Cyc_Absyn_Constraint*_tmp8E6;union Cyc_Absyn_Constraint*_tmp8E7;_LL487: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8DF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8DE;if(_tmp8DF->tag != 5)goto _LL489;else{_tmp8E0=_tmp8DF->f1;_tmp8E1=_tmp8E0.elt_typ;_tmp8E2=_tmp8E0.elt_tq;_tmp8E3=_tmp8E0.ptr_atts;_tmp8E4=_tmp8E3.rgn;_tmp8E5=_tmp8E3.nullable;_tmp8E6=_tmp8E3.bounds;_tmp8E7=_tmp8E3.zero_term;}}_LL488: {
# 2716
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
# 2717
struct _tuple0*_tmp133F;struct Cyc_List_List*_tmp133E;struct Cyc_List_List*_tmp8E8=(_tmp133E=_cycalloc(sizeof(*_tmp133E)),((_tmp133E->hd=((_tmp133F=_cycalloc(sizeof(*_tmp133F)),((_tmp133F->f1=t1,((_tmp133F->f2=t2,_tmp133F)))))),((_tmp133E->tl=0,_tmp133E)))));
# 2718
int _tmp8E9=_tmp8E2.real_const  || !_tmp8CC.real_const;
# 2730 "tcutil.cyc"
int _tmp8EA=
# 2731
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp8E6,Cyc_Absyn_bounds_one_conref) && !
# 2732
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp8E7);
# 2734
int _tmp8EB=_tmp8E9  && (
# 2735
((_tmp8EA  && Cyc_Tcutil_ptrsubtype(te,_tmp8E8,_tmp8CB,_tmp8E1) || 
# 2736
Cyc_Tcutil_unify(_tmp8CB,_tmp8E1)) || Cyc_Tcutil_isomorphic(_tmp8CB,_tmp8E1)) || Cyc_Tcutil_unify(_tmp8E1,(void*)& Cyc_Absyn_VoidType_val));
# 2738
Cyc_Tcutil_t1_failure=t1;
# 2739
Cyc_Tcutil_t2_failure=t2;{
# 2740
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8D1,_tmp8E7) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8E7);
# 2742
int _tmp8EC=_tmp8EB?0:((Cyc_Tcutil_bits_only(_tmp8CB) && Cyc_Tcutil_is_char_type(_tmp8E1)) && !
# 2743
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8E7)) && (
# 2744
_tmp8E2.real_const  || !_tmp8CC.real_const);
# 2745
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp8D0,_tmp8E6);
# 2746
if(!bounds_ok  && !_tmp8EC){
# 2747
struct _tuple0 _tmp1340;struct _tuple0 _tmp8EE=(_tmp1340.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8D0),((_tmp1340.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8E6),_tmp1340)));void*_tmp8EF;struct Cyc_Absyn_Exp*_tmp8F1;void*_tmp8F2;struct Cyc_Absyn_Exp*_tmp8F4;_LL48C: _tmp8EF=_tmp8EE.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp8F0=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp8EF;if(_tmp8F0->tag != 1)goto _LL48E;else{_tmp8F1=_tmp8F0->f1;}};_tmp8F2=_tmp8EE.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp8F3=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp8F2;if(_tmp8F3->tag != 1)goto _LL48E;else{_tmp8F4=_tmp8F3->f1;}};_LL48D:
# 2752
 if(Cyc_Evexp_lte_const_exp(_tmp8F4,_tmp8F1))
# 2753
bounds_ok=1;
# 2754
goto _LL48B;_LL48E:;_LL48F:
# 2757
 bounds_ok=1;goto _LL48B;_LL48B:;}
# 2759
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8CF) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8E5))
# 2760
coercion=Cyc_Absyn_NonNull_to_Null;
# 2764
if(((bounds_ok  && zeroterm_ok) && (_tmp8EB  || _tmp8EC)) && (
# 2765
Cyc_Tcutil_unify(_tmp8CE,_tmp8E4) || Cyc_Tcenv_region_outlives(te,_tmp8CE,_tmp8E4)))
# 2766
return coercion;else{
# 2767
return Cyc_Absyn_Unknown_coercion;}};}_LL489:;_LL48A:
# 2768
 goto _LL486;_LL486:;}
# 2770
return Cyc_Absyn_Unknown_coercion;_LL47A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8D2=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8C8;if(_tmp8D2->tag != 8)goto _LL47C;else{_tmp8D3=_tmp8D2->f1;_tmp8D4=_tmp8D3.elt_type;_tmp8D5=_tmp8D3.tq;_tmp8D6=_tmp8D3.num_elts;_tmp8D7=_tmp8D3.zero_term;}}_LL47B:
# 2772
{void*_tmp8F7=t2;struct Cyc_Absyn_ArrayInfo _tmp8F9;void*_tmp8FA;struct Cyc_Absyn_Tqual _tmp8FB;struct Cyc_Absyn_Exp*_tmp8FC;union Cyc_Absyn_Constraint*_tmp8FD;_LL491: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8F8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8F7;if(_tmp8F8->tag != 8)goto _LL493;else{_tmp8F9=_tmp8F8->f1;_tmp8FA=_tmp8F9.elt_type;_tmp8FB=_tmp8F9.tq;_tmp8FC=_tmp8F9.num_elts;_tmp8FD=_tmp8F9.zero_term;}}_LL492: {
# 2774
int okay;
# 2775
okay=
# 2776
((_tmp8D6 != 0  && _tmp8FC != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8D7,_tmp8FD)) && 
# 2777
Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp8FC,(struct Cyc_Absyn_Exp*)_tmp8D6);
# 2778
return
# 2780
(okay  && Cyc_Tcutil_unify(_tmp8D4,_tmp8FA)) && (!_tmp8D5.real_const  || _tmp8FB.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}_LL493:;_LL494:
# 2782
 return Cyc_Absyn_Unknown_coercion;_LL490:;}
# 2784
return Cyc_Absyn_Unknown_coercion;_LL47C: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp8D8=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp8C8;if(_tmp8D8->tag != 13)goto _LL47E;else{_tmp8D9=_tmp8D8->f2;}}_LL47D:
# 2788
{void*_tmp8FE=t2;struct Cyc_Absyn_Enumdecl*_tmp900;_LL496: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp8FF=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp8FE;if(_tmp8FF->tag != 13)goto _LL498;else{_tmp900=_tmp8FF->f2;}}_LL497:
# 2790
 if((_tmp8D9->fields != 0  && _tmp900->fields != 0) && 
# 2791
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp8D9->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp900->fields))->v))
# 2792
return Cyc_Absyn_Other_coercion;
# 2793
goto _LL495;_LL498:;_LL499:
# 2794
 goto _LL495;_LL495:;}
# 2796
goto _LL47F;_LL47E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8DA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8C8;if(_tmp8DA->tag != 6)goto _LL480;}_LL47F:
# 2797
 goto _LL481;_LL480: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp8DB=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp8C8;if(_tmp8DB->tag != 7)goto _LL482;}_LL481:
# 2798
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;_LL482: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp8DC=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp8C8;if(_tmp8DC->tag != 15)goto _LL484;else{_tmp8DD=(void*)_tmp8DC->f1;}}_LL483:
# 2801
{void*_tmp901=t2;void*_tmp903;_LL49B: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp902=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp901;if(_tmp902->tag != 15)goto _LL49D;else{_tmp903=(void*)_tmp902->f1;}}_LL49C:
# 2803
 if(Cyc_Tcenv_region_outlives(te,_tmp8DD,_tmp903))return Cyc_Absyn_No_coercion;
# 2804
goto _LL49A;_LL49D:;_LL49E:
# 2805
 goto _LL49A;_LL49A:;}
# 2807
return Cyc_Absyn_Unknown_coercion;_LL484:;_LL485:
# 2808
 return Cyc_Absyn_Unknown_coercion;_LL477:;};}
# 2813
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
# 2814
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
# 2815
struct Cyc_Absyn_Exp*_tmp904=Cyc_Absyn_copy_exp(e);
# 2816
{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp1343;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1342;e->r=(void*)((_tmp1342=_cycalloc(sizeof(*_tmp1342)),((_tmp1342[0]=((_tmp1343.tag=13,((_tmp1343.f1=(void*)t,((_tmp1343.f2=_tmp904,((_tmp1343.f3=0,((_tmp1343.f4=c,_tmp1343)))))))))),_tmp1342))));}
# 2817
e->topt=(void*)t;}}
# 2821
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
# 2822
void*_tmp907=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL4A0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp908=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp907;if(_tmp908->tag != 6)goto _LL4A2;}_LL4A1:
# 2823
 goto _LL4A3;_LL4A2: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp909=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp907;if(_tmp909->tag != 13)goto _LL4A4;}_LL4A3:
# 2824
 goto _LL4A5;_LL4A4: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp90A=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp907;if(_tmp90A->tag != 14)goto _LL4A6;}_LL4A5:
# 2825
 goto _LL4A7;_LL4A6: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp90B=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp907;if(_tmp90B->tag != 19)goto _LL4A8;}_LL4A7:
# 2826
 return 1;_LL4A8: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp90C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp907;if(_tmp90C->tag != 1)goto _LL4AA;}_LL4A9:
# 2827
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);_LL4AA:;_LL4AB:
# 2828
 return 0;_LL49F:;}
# 2832
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
# 2833
if(Cyc_Tcutil_is_integral(e))
# 2834
return 1;{
# 2835
void*_tmp90D=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL4AD: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp90E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp90D;if(_tmp90E->tag != 7)goto _LL4AF;}_LL4AE:
# 2836
 return 1;_LL4AF:;_LL4B0:
# 2837
 return 0;_LL4AC:;};}
# 2841
int Cyc_Tcutil_is_function_type(void*t){
# 2842
void*_tmp90F=Cyc_Tcutil_compress(t);_LL4B2: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp910=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp90F;if(_tmp910->tag != 9)goto _LL4B4;}_LL4B3:
# 2843
 return 1;_LL4B4:;_LL4B5:
# 2844
 return 0;_LL4B1:;}
# 2848
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
# 2849
struct _tuple0 _tmp1344;struct _tuple0 _tmp912=(_tmp1344.f1=t1,((_tmp1344.f2=t2,_tmp1344)));void*_tmp913;int _tmp915;void*_tmp916;int _tmp918;void*_tmp919;void*_tmp91B;void*_tmp91D;enum Cyc_Absyn_Sign _tmp91F;enum Cyc_Absyn_Size_of _tmp920;void*_tmp921;enum Cyc_Absyn_Sign _tmp923;enum Cyc_Absyn_Size_of _tmp924;void*_tmp925;enum Cyc_Absyn_Size_of _tmp927;void*_tmp928;enum Cyc_Absyn_Size_of _tmp92A;void*_tmp92B;enum Cyc_Absyn_Sign _tmp92D;enum Cyc_Absyn_Size_of _tmp92E;void*_tmp92F;enum Cyc_Absyn_Sign _tmp931;enum Cyc_Absyn_Size_of _tmp932;void*_tmp933;enum Cyc_Absyn_Sign _tmp935;enum Cyc_Absyn_Size_of _tmp936;void*_tmp937;enum Cyc_Absyn_Sign _tmp939;enum Cyc_Absyn_Size_of _tmp93A;void*_tmp93B;enum Cyc_Absyn_Size_of _tmp93D;void*_tmp93E;enum Cyc_Absyn_Size_of _tmp940;_LL4B7: _tmp913=_tmp912.f1;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp914=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp913;if(_tmp914->tag != 7)goto _LL4B9;else{_tmp915=_tmp914->f1;}};_tmp916=_tmp912.f2;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp917=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp916;if(_tmp917->tag != 7)goto _LL4B9;else{_tmp918=_tmp917->f1;}};_LL4B8:
# 2851
 if(_tmp915 != 0  && _tmp915 != 1)return t1;else{
# 2852
if(_tmp918 != 0  && _tmp918 != 1)return t2;else{
# 2853
if(_tmp915 >= _tmp918)return t1;else{
# 2854
return t2;}}}_LL4B9: _tmp919=_tmp912.f1;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp91A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp919;if(_tmp91A->tag != 7)goto _LL4BB;};_LL4BA:
# 2855
 return t1;_LL4BB: _tmp91B=_tmp912.f2;{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp91C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp91B;if(_tmp91C->tag != 7)goto _LL4BD;};_LL4BC:
# 2856
 return t2;_LL4BD: _tmp91D=_tmp912.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp91E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp91D;if(_tmp91E->tag != 6)goto _LL4BF;else{_tmp91F=_tmp91E->f1;if(_tmp91F != Cyc_Absyn_Unsigned)goto _LL4BF;_tmp920=_tmp91E->f2;if(_tmp920 != Cyc_Absyn_LongLong_sz)goto _LL4BF;}};_LL4BE:
# 2857
 goto _LL4C0;_LL4BF: _tmp921=_tmp912.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp922=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp921;if(_tmp922->tag != 6)goto _LL4C1;else{_tmp923=_tmp922->f1;if(_tmp923 != Cyc_Absyn_Unsigned)goto _LL4C1;_tmp924=_tmp922->f2;if(_tmp924 != Cyc_Absyn_LongLong_sz)goto _LL4C1;}};_LL4C0:
# 2858
 return Cyc_Absyn_ulonglong_typ;_LL4C1: _tmp925=_tmp912.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp926=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp925;if(_tmp926->tag != 6)goto _LL4C3;else{_tmp927=_tmp926->f2;if(_tmp927 != Cyc_Absyn_LongLong_sz)goto _LL4C3;}};_LL4C2:
# 2859
 goto _LL4C4;_LL4C3: _tmp928=_tmp912.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp929=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp928;if(_tmp929->tag != 6)goto _LL4C5;else{_tmp92A=_tmp929->f2;if(_tmp92A != Cyc_Absyn_LongLong_sz)goto _LL4C5;}};_LL4C4:
# 2860
 return Cyc_Absyn_slonglong_typ;_LL4C5: _tmp92B=_tmp912.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp92C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp92B;if(_tmp92C->tag != 6)goto _LL4C7;else{_tmp92D=_tmp92C->f1;if(_tmp92D != Cyc_Absyn_Unsigned)goto _LL4C7;_tmp92E=_tmp92C->f2;if(_tmp92E != Cyc_Absyn_Long_sz)goto _LL4C7;}};_LL4C6:
# 2861
 goto _LL4C8;_LL4C7: _tmp92F=_tmp912.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp930=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp92F;if(_tmp930->tag != 6)goto _LL4C9;else{_tmp931=_tmp930->f1;if(_tmp931 != Cyc_Absyn_Unsigned)goto _LL4C9;_tmp932=_tmp930->f2;if(_tmp932 != Cyc_Absyn_Long_sz)goto _LL4C9;}};_LL4C8:
# 2862
 return Cyc_Absyn_ulong_typ;_LL4C9: _tmp933=_tmp912.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp934=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp933;if(_tmp934->tag != 6)goto _LL4CB;else{_tmp935=_tmp934->f1;if(_tmp935 != Cyc_Absyn_Unsigned)goto _LL4CB;_tmp936=_tmp934->f2;if(_tmp936 != Cyc_Absyn_Int_sz)goto _LL4CB;}};_LL4CA:
# 2864
 goto _LL4CC;_LL4CB: _tmp937=_tmp912.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp938=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp937;if(_tmp938->tag != 6)goto _LL4CD;else{_tmp939=_tmp938->f1;if(_tmp939 != Cyc_Absyn_Unsigned)goto _LL4CD;_tmp93A=_tmp938->f2;if(_tmp93A != Cyc_Absyn_Int_sz)goto _LL4CD;}};_LL4CC:
# 2865
 return Cyc_Absyn_uint_typ;_LL4CD: _tmp93B=_tmp912.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp93C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp93B;if(_tmp93C->tag != 6)goto _LL4CF;else{_tmp93D=_tmp93C->f2;if(_tmp93D != Cyc_Absyn_Long_sz)goto _LL4CF;}};_LL4CE:
# 2866
 goto _LL4D0;_LL4CF: _tmp93E=_tmp912.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp93F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp93E;if(_tmp93F->tag != 6)goto _LL4D1;else{_tmp940=_tmp93F->f2;if(_tmp940 != Cyc_Absyn_Long_sz)goto _LL4D1;}};_LL4D0:
# 2867
 return Cyc_Absyn_slong_typ;_LL4D1:;_LL4D2:
# 2868
 return Cyc_Absyn_sint_typ;_LL4B6:;}
# 2873
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*e){
# 2874
void*_tmp941=e->r;struct Cyc_Core_Opt*_tmp943;_LL4D4: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp942=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp941;if(_tmp942->tag != 3)goto _LL4D6;else{_tmp943=_tmp942->f2;if(_tmp943 != 0)goto _LL4D6;}}_LL4D5:
# 2875
{const char*_tmp1347;void*_tmp1346;(_tmp1346=0,Cyc_Tcutil_warn(e->loc,((_tmp1347="assignment in test",_tag_dyneither(_tmp1347,sizeof(char),19))),_tag_dyneither(_tmp1346,sizeof(void*),0)));}goto _LL4D3;_LL4D6:;_LL4D7:
# 2876
 goto _LL4D3;_LL4D3:;}
# 2887 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
# 2888
c1=Cyc_Absyn_compress_kb(c1);
# 2889
c2=Cyc_Absyn_compress_kb(c2);{
# 2890
struct _tuple0 _tmp1348;struct _tuple0 _tmp947=(_tmp1348.f1=c1,((_tmp1348.f2=c2,_tmp1348)));void*_tmp948;struct Cyc_Absyn_Kind*_tmp94A;void*_tmp94B;struct Cyc_Absyn_Kind*_tmp94D;void*_tmp94E;struct Cyc_Core_Opt*_tmp950;struct Cyc_Core_Opt**_tmp951;void*_tmp952;struct Cyc_Core_Opt*_tmp954;struct Cyc_Core_Opt**_tmp955;void*_tmp956;struct Cyc_Core_Opt*_tmp958;struct Cyc_Core_Opt**_tmp959;struct Cyc_Absyn_Kind*_tmp95A;void*_tmp95B;struct Cyc_Absyn_Kind*_tmp95D;void*_tmp95E;struct Cyc_Absyn_Kind*_tmp960;void*_tmp961;struct Cyc_Core_Opt*_tmp963;struct Cyc_Core_Opt**_tmp964;struct Cyc_Absyn_Kind*_tmp965;void*_tmp966;struct Cyc_Core_Opt*_tmp968;struct Cyc_Core_Opt**_tmp969;struct Cyc_Absyn_Kind*_tmp96A;void*_tmp96B;struct Cyc_Core_Opt*_tmp96D;struct Cyc_Core_Opt**_tmp96E;struct Cyc_Absyn_Kind*_tmp96F;_LL4D9: _tmp948=_tmp947.f1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp949=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp948;if(_tmp949->tag != 0)goto _LL4DB;else{_tmp94A=_tmp949->f1;}};_tmp94B=_tmp947.f2;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp94C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp94B;if(_tmp94C->tag != 0)goto _LL4DB;else{_tmp94D=_tmp94C->f1;}};_LL4DA:
# 2891
 return _tmp94A == _tmp94D;_LL4DB: _tmp94E=_tmp947.f2;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp94F=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp94E;if(_tmp94F->tag != 1)goto _LL4DD;else{_tmp950=_tmp94F->f1;_tmp951=(struct Cyc_Core_Opt**)& _tmp94F->f1;}};_LL4DC:
# 2892
{struct Cyc_Core_Opt*_tmp1349;*_tmp951=((_tmp1349=_cycalloc(sizeof(*_tmp1349)),((_tmp1349->v=c1,_tmp1349))));}return 1;_LL4DD: _tmp952=_tmp947.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp953=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp952;if(_tmp953->tag != 1)goto _LL4DF;else{_tmp954=_tmp953->f1;_tmp955=(struct Cyc_Core_Opt**)& _tmp953->f1;}};_LL4DE:
# 2893
{struct Cyc_Core_Opt*_tmp134A;*_tmp955=((_tmp134A=_cycalloc(sizeof(*_tmp134A)),((_tmp134A->v=c2,_tmp134A))));}return 1;_LL4DF: _tmp956=_tmp947.f1;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp957=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp956;if(_tmp957->tag != 2)goto _LL4E1;else{_tmp958=_tmp957->f1;_tmp959=(struct Cyc_Core_Opt**)& _tmp957->f1;_tmp95A=_tmp957->f2;}};_tmp95B=_tmp947.f2;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp95C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp95B;if(_tmp95C->tag != 0)goto _LL4E1;else{_tmp95D=_tmp95C->f1;}};_LL4E0:
# 2895
 if(Cyc_Tcutil_kind_leq(_tmp95D,_tmp95A)){
# 2896
{struct Cyc_Core_Opt*_tmp134B;*_tmp959=((_tmp134B=_cycalloc(sizeof(*_tmp134B)),((_tmp134B->v=c2,_tmp134B))));}return 1;}else{
# 2897
return 0;}_LL4E1: _tmp95E=_tmp947.f1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp95F=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp95E;if(_tmp95F->tag != 0)goto _LL4E3;else{_tmp960=_tmp95F->f1;}};_tmp961=_tmp947.f2;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp962=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp961;if(_tmp962->tag != 2)goto _LL4E3;else{_tmp963=_tmp962->f1;_tmp964=(struct Cyc_Core_Opt**)& _tmp962->f1;_tmp965=_tmp962->f2;}};_LL4E2:
# 2899
 if(Cyc_Tcutil_kind_leq(_tmp960,_tmp965)){
# 2900
{struct Cyc_Core_Opt*_tmp134C;*_tmp964=((_tmp134C=_cycalloc(sizeof(*_tmp134C)),((_tmp134C->v=c1,_tmp134C))));}return 1;}else{
# 2901
return 0;}_LL4E3: _tmp966=_tmp947.f1;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp967=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp966;if(_tmp967->tag != 2)goto _LL4D8;else{_tmp968=_tmp967->f1;_tmp969=(struct Cyc_Core_Opt**)& _tmp967->f1;_tmp96A=_tmp967->f2;}};_tmp96B=_tmp947.f2;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp96C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp96B;if(_tmp96C->tag != 2)goto _LL4D8;else{_tmp96D=_tmp96C->f1;_tmp96E=(struct Cyc_Core_Opt**)& _tmp96C->f1;_tmp96F=_tmp96C->f2;}};_LL4E4:
# 2903
 if(Cyc_Tcutil_kind_leq(_tmp96A,_tmp96F)){
# 2904
{struct Cyc_Core_Opt*_tmp134D;*_tmp96E=((_tmp134D=_cycalloc(sizeof(*_tmp134D)),((_tmp134D->v=c1,_tmp134D))));}return 1;}else{
# 2905
if(Cyc_Tcutil_kind_leq(_tmp96F,_tmp96A)){
# 2906
{struct Cyc_Core_Opt*_tmp134E;*_tmp969=((_tmp134E=_cycalloc(sizeof(*_tmp134E)),((_tmp134E->v=c2,_tmp134E))));}return 1;}else{
# 2907
return 0;}}_LL4D8:;};}
# 2912
static int Cyc_Tcutil_tvar_id_counter=0;
# 2913
int Cyc_Tcutil_new_tvar_id(){
# 2914
return Cyc_Tcutil_tvar_id_counter ++;}
# 2917
static int Cyc_Tcutil_tvar_counter=0;
# 2918
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
# 2919
int i=Cyc_Tcutil_tvar_counter ++;
# 2920
const char*_tmp1352;void*_tmp1351[1];struct Cyc_Int_pa_PrintArg_struct _tmp1350;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp1350.tag=1,((_tmp1350.f1=(unsigned long)i,((_tmp1351[0]=& _tmp1350,Cyc_aprintf(((_tmp1352="#%d",_tag_dyneither(_tmp1352,sizeof(char),4))),_tag_dyneither(_tmp1351,sizeof(void*),1))))))));
# 2921
struct _dyneither_ptr*_tmp1355;struct Cyc_Absyn_Tvar*_tmp1354;return(_tmp1354=_cycalloc(sizeof(*_tmp1354)),((_tmp1354->name=((_tmp1355=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp1355[0]=s,_tmp1355)))),((_tmp1354->identity=- 1,((_tmp1354->kind=k,_tmp1354)))))));}
# 2924
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
# 2925
struct _dyneither_ptr _tmp97B=*t->name;
# 2926
return*((const char*)_check_dyneither_subscript(_tmp97B,sizeof(char),0))== '#';}
# 2929
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
# 2930
{const char*_tmp1359;void*_tmp1358[1];struct Cyc_String_pa_PrintArg_struct _tmp1357;(_tmp1357.tag=0,((_tmp1357.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmp1358[0]=& _tmp1357,Cyc_printf(((_tmp1359="%s",_tag_dyneither(_tmp1359,sizeof(char),3))),_tag_dyneither(_tmp1358,sizeof(void*),1)))))));}
# 2931
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
# 2932
const char*_tmp135A;struct _dyneither_ptr _tmp97F=Cyc_strconcat(((_tmp135A="`",_tag_dyneither(_tmp135A,sizeof(char),2))),(struct _dyneither_ptr)*t->name);
# 2933
{char _tmp135D;char _tmp135C;struct _dyneither_ptr _tmp135B;(_tmp135B=_dyneither_ptr_plus(_tmp97F,sizeof(char),1),((_tmp135C=*((char*)_check_dyneither_subscript(_tmp135B,sizeof(char),0)),((_tmp135D='t',((_get_dyneither_size(_tmp135B,sizeof(char))== 1  && (_tmp135C == '\000'  && _tmp135D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp135B.curr)=_tmp135D)))))));}{
# 2934
struct _dyneither_ptr*_tmp135E;t->name=((_tmp135E=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp135E[0]=(struct _dyneither_ptr)_tmp97F,_tmp135E))));};};}
# 2938
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 2940
struct _tuple10*_tmp135F;return(_tmp135F=_cycalloc(sizeof(*_tmp135F)),((_tmp135F->f1=(struct _dyneither_ptr*)(*x).f1,((_tmp135F->f2=(*x).f2,((_tmp135F->f3=(*x).f3,_tmp135F)))))));}
# 2943
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
# 2944
if(fd->cached_typ == 0){
# 2950
struct Cyc_List_List*_tmp986=0;
# 2951
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
# 2952
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
# 2953
struct Cyc_List_List*_tmp1360;_tmp986=((_tmp1360=_cycalloc(sizeof(*_tmp1360)),((_tmp1360->hd=(void*)atts->hd,((_tmp1360->tl=_tmp986,_tmp1360))))));}}}{
# 2954
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1366;struct Cyc_Absyn_FnInfo _tmp1365;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1364;return(void*)((_tmp1364=_cycalloc(sizeof(*_tmp1364)),((_tmp1364[0]=((_tmp1366.tag=9,((_tmp1366.f1=((_tmp1365.tvars=fd->tvs,((_tmp1365.effect=fd->effect,((_tmp1365.ret_tqual=fd->ret_tqual,((_tmp1365.ret_typ=fd->ret_type,((_tmp1365.args=
# 2955
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args),((_tmp1365.c_varargs=fd->c_varargs,((_tmp1365.cyc_varargs=fd->cyc_varargs,((_tmp1365.rgn_po=fd->rgn_po,((_tmp1365.attributes=_tmp986,_tmp1365)))))))))))))))))),_tmp1366)))),_tmp1364))));};}
# 2959
return(void*)_check_null(fd->cached_typ);}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 2963
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
# 2964
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
# 2965
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
# 2966
struct _tuple12 _tmp98C;struct Cyc_Absyn_Tqual _tmp98D;void*_tmp98E;struct _tuple12*_tmp98B=pr;_tmp98C=*_tmp98B;_tmp98D=_tmp98C.f1;_tmp98E=_tmp98C.f2;
# 2967
if(_tmp98E == t)return pr;else{
# 2968
struct _tuple12*_tmp1367;return(_tmp1367=_cycalloc(sizeof(*_tmp1367)),((_tmp1367->f1=_tmp98D,((_tmp1367->f2=t,_tmp1367)))));}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 2970
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 2972
struct _tuple26*_tmp136A;struct _tuple27*_tmp1369;return(_tmp1369=_region_malloc(rgn,sizeof(*_tmp1369)),((_tmp1369->f1=((_tmp136A=_region_malloc(rgn,sizeof(*_tmp136A)),((_tmp136A->f1=(*y).f1,((_tmp136A->f2=(*y).f2,_tmp136A)))))),((_tmp1369->f2=(*y).f3,_tmp1369)))));}
# 2974
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 2976
struct _dyneither_ptr*_tmp993;struct Cyc_Absyn_Tqual _tmp994;void*_tmp995;struct _tuple10 _tmp992=*orig_arg;_tmp993=_tmp992.f1;_tmp994=_tmp992.f2;_tmp995=_tmp992.f3;
# 2977
if(t == _tmp995)return orig_arg;{
# 2978
struct _tuple10*_tmp136B;return(_tmp136B=_cycalloc(sizeof(*_tmp136B)),((_tmp136B->f1=_tmp993,((_tmp136B->f2=_tmp994,((_tmp136B->f3=t,_tmp136B)))))));};}
# 2980
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
# 2981
return f->type;}
# 2983
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(struct Cyc_Absyn_Aggrfield*f,void*t){
# 2984
if(f->type == t)return f;{
# 2985
struct Cyc_Absyn_Aggrfield*_tmp136C;return(_tmp136C=_cycalloc(sizeof(*_tmp136C)),((_tmp136C->name=f->name,((_tmp136C->tq=f->tq,((_tmp136C->type=t,((_tmp136C->width=f->width,((_tmp136C->attributes=f->attributes,_tmp136C)))))))))));};}
# 2989
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 2994
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 2996
struct Cyc_Absyn_Exp*_tmp136D;return(_tmp136D=_cycalloc(sizeof(*_tmp136D)),((_tmp136D->topt=old->topt,((_tmp136D->r=r,((_tmp136D->loc=old->loc,((_tmp136D->annot=old->annot,_tmp136D)))))))));}
# 3001
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3003
void*_tmp999=e->r;enum Cyc_Absyn_Primop _tmp99F;struct Cyc_List_List*_tmp9A0;struct Cyc_Absyn_Exp*_tmp9A2;struct Cyc_Absyn_Exp*_tmp9A3;struct Cyc_Absyn_Exp*_tmp9A4;struct Cyc_Absyn_Exp*_tmp9A6;struct Cyc_Absyn_Exp*_tmp9A7;struct Cyc_Absyn_Exp*_tmp9A9;struct Cyc_Absyn_Exp*_tmp9AA;struct Cyc_Absyn_Exp*_tmp9AC;struct Cyc_Absyn_Exp*_tmp9AD;void*_tmp9AF;struct Cyc_Absyn_Exp*_tmp9B0;int _tmp9B1;enum Cyc_Absyn_Coercion _tmp9B2;void*_tmp9B4;struct Cyc_Absyn_Exp*_tmp9B6;void*_tmp9B8;void*_tmp9B9;void*_tmp9BB;_LL4E6: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp99A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp99A->tag != 0)goto _LL4E8;}_LL4E7:
# 3004
 goto _LL4E9;_LL4E8: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp99B=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp99B->tag != 30)goto _LL4EA;}_LL4E9:
# 3005
 goto _LL4EB;_LL4EA: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp99C=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp99C->tag != 31)goto _LL4EC;}_LL4EB:
# 3006
 goto _LL4ED;_LL4EC: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp99D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp99D->tag != 1)goto _LL4EE;}_LL4ED:
# 3007
 return e;_LL4EE: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp99E=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp99E->tag != 2)goto _LL4F0;else{_tmp99F=_tmp99E->f1;_tmp9A0=_tmp99E->f2;}}_LL4EF:
# 3010
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9A0)== 1){
# 3011
struct Cyc_Absyn_Exp*_tmp9BC=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp9A0))->hd;
# 3012
struct Cyc_Absyn_Exp*_tmp9BD=Cyc_Tcutil_rsubsexp(r,inst,_tmp9BC);
# 3013
if(_tmp9BD == _tmp9BC)return e;{
# 3014
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp1373;struct Cyc_Absyn_Exp*_tmp1372[1];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1371;return Cyc_Tcutil_copye(e,(void*)((_tmp1371=_cycalloc(sizeof(*_tmp1371)),((_tmp1371[0]=((_tmp1373.tag=2,((_tmp1373.f1=_tmp99F,((_tmp1373.f2=((_tmp1372[0]=_tmp9BD,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1372,sizeof(struct Cyc_Absyn_Exp*),1)))),_tmp1373)))))),_tmp1371)))));};}else{
# 3015
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9A0)== 2){
# 3016
struct Cyc_Absyn_Exp*_tmp9C1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp9A0))->hd;
# 3017
struct Cyc_Absyn_Exp*_tmp9C2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp9A0->tl))->hd;
# 3018
struct Cyc_Absyn_Exp*_tmp9C3=Cyc_Tcutil_rsubsexp(r,inst,_tmp9C1);
# 3019
struct Cyc_Absyn_Exp*_tmp9C4=Cyc_Tcutil_rsubsexp(r,inst,_tmp9C2);
# 3020
if(_tmp9C3 == _tmp9C1  && _tmp9C4 == _tmp9C2)return e;{
# 3021
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp1379;struct Cyc_Absyn_Exp*_tmp1378[2];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1377;return Cyc_Tcutil_copye(e,(void*)((_tmp1377=_cycalloc(sizeof(*_tmp1377)),((_tmp1377[0]=((_tmp1379.tag=2,((_tmp1379.f1=_tmp99F,((_tmp1379.f2=((_tmp1378[1]=_tmp9C4,((_tmp1378[0]=_tmp9C3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1378,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp1379)))))),_tmp1377)))));};}else{
# 3022
const char*_tmp137C;void*_tmp137B;return(_tmp137B=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp137C="primop does not have 1 or 2 args!",_tag_dyneither(_tmp137C,sizeof(char),34))),_tag_dyneither(_tmp137B,sizeof(void*),0)));}}_LL4F0: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp9A1=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp9A1->tag != 5)goto _LL4F2;else{_tmp9A2=_tmp9A1->f1;_tmp9A3=_tmp9A1->f2;_tmp9A4=_tmp9A1->f3;}}_LL4F1: {
# 3024
struct Cyc_Absyn_Exp*_tmp9CA=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A2);
# 3025
struct Cyc_Absyn_Exp*_tmp9CB=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A3);
# 3026
struct Cyc_Absyn_Exp*_tmp9CC=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A4);
# 3027
if((_tmp9CA == _tmp9A2  && _tmp9CB == _tmp9A3) && _tmp9CC == _tmp9A4)return e;{
# 3028
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp137F;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp137E;return Cyc_Tcutil_copye(e,(void*)((_tmp137E=_cycalloc(sizeof(*_tmp137E)),((_tmp137E[0]=((_tmp137F.tag=5,((_tmp137F.f1=_tmp9CA,((_tmp137F.f2=_tmp9CB,((_tmp137F.f3=_tmp9CC,_tmp137F)))))))),_tmp137E)))));};}_LL4F2: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp9A5=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp9A5->tag != 6)goto _LL4F4;else{_tmp9A6=_tmp9A5->f1;_tmp9A7=_tmp9A5->f2;}}_LL4F3: {
# 3030
struct Cyc_Absyn_Exp*_tmp9CF=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A6);
# 3031
struct Cyc_Absyn_Exp*_tmp9D0=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A7);
# 3032
if(_tmp9CF == _tmp9A6  && _tmp9D0 == _tmp9A7)return e;{
# 3033
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp1382;struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1381;return Cyc_Tcutil_copye(e,(void*)((_tmp1381=_cycalloc(sizeof(*_tmp1381)),((_tmp1381[0]=((_tmp1382.tag=6,((_tmp1382.f1=_tmp9CF,((_tmp1382.f2=_tmp9D0,_tmp1382)))))),_tmp1381)))));};}_LL4F4: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp9A8=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp9A8->tag != 7)goto _LL4F6;else{_tmp9A9=_tmp9A8->f1;_tmp9AA=_tmp9A8->f2;}}_LL4F5: {
# 3035
struct Cyc_Absyn_Exp*_tmp9D3=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A9);
# 3036
struct Cyc_Absyn_Exp*_tmp9D4=Cyc_Tcutil_rsubsexp(r,inst,_tmp9AA);
# 3037
if(_tmp9D3 == _tmp9A9  && _tmp9D4 == _tmp9AA)return e;{
# 3038
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp1385;struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1384;return Cyc_Tcutil_copye(e,(void*)((_tmp1384=_cycalloc(sizeof(*_tmp1384)),((_tmp1384[0]=((_tmp1385.tag=7,((_tmp1385.f1=_tmp9D3,((_tmp1385.f2=_tmp9D4,_tmp1385)))))),_tmp1384)))));};}_LL4F6: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp9AB=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp9AB->tag != 8)goto _LL4F8;else{_tmp9AC=_tmp9AB->f1;_tmp9AD=_tmp9AB->f2;}}_LL4F7: {
# 3040
struct Cyc_Absyn_Exp*_tmp9D7=Cyc_Tcutil_rsubsexp(r,inst,_tmp9AC);
# 3041
struct Cyc_Absyn_Exp*_tmp9D8=Cyc_Tcutil_rsubsexp(r,inst,_tmp9AD);
# 3042
if(_tmp9D7 == _tmp9AC  && _tmp9D8 == _tmp9AD)return e;{
# 3043
struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp1388;struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp1387;return Cyc_Tcutil_copye(e,(void*)((_tmp1387=_cycalloc(sizeof(*_tmp1387)),((_tmp1387[0]=((_tmp1388.tag=8,((_tmp1388.f1=_tmp9D7,((_tmp1388.f2=_tmp9D8,_tmp1388)))))),_tmp1387)))));};}_LL4F8: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp9AE=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp9AE->tag != 13)goto _LL4FA;else{_tmp9AF=(void*)_tmp9AE->f1;_tmp9B0=_tmp9AE->f2;_tmp9B1=_tmp9AE->f3;_tmp9B2=_tmp9AE->f4;}}_LL4F9: {
# 3045
struct Cyc_Absyn_Exp*_tmp9DB=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B0);
# 3046
void*_tmp9DC=Cyc_Tcutil_rsubstitute(r,inst,_tmp9AF);
# 3047
if(_tmp9DB == _tmp9B0  && _tmp9DC == _tmp9AF)return e;{
# 3048
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp138B;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp138A;return Cyc_Tcutil_copye(e,(void*)((_tmp138A=_cycalloc(sizeof(*_tmp138A)),((_tmp138A[0]=((_tmp138B.tag=13,((_tmp138B.f1=(void*)_tmp9DC,((_tmp138B.f2=_tmp9DB,((_tmp138B.f3=_tmp9B1,((_tmp138B.f4=_tmp9B2,_tmp138B)))))))))),_tmp138A)))));};}_LL4FA: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp9B3=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp9B3->tag != 16)goto _LL4FC;else{_tmp9B4=(void*)_tmp9B3->f1;}}_LL4FB: {
# 3050
void*_tmp9DF=Cyc_Tcutil_rsubstitute(r,inst,_tmp9B4);
# 3051
if(_tmp9DF == _tmp9B4)return e;{
# 3052
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp138E;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp138D;return Cyc_Tcutil_copye(e,(void*)((_tmp138D=_cycalloc(sizeof(*_tmp138D)),((_tmp138D[0]=((_tmp138E.tag=16,((_tmp138E.f1=(void*)_tmp9DF,_tmp138E)))),_tmp138D)))));};}_LL4FC: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp9B5=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp9B5->tag != 17)goto _LL4FE;else{_tmp9B6=_tmp9B5->f1;}}_LL4FD: {
# 3054
struct Cyc_Absyn_Exp*_tmp9E2=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B6);
# 3055
if(_tmp9E2 == _tmp9B6)return e;{
# 3056
struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp1391;struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp1390;return Cyc_Tcutil_copye(e,(void*)((_tmp1390=_cycalloc(sizeof(*_tmp1390)),((_tmp1390[0]=((_tmp1391.tag=17,((_tmp1391.f1=_tmp9E2,_tmp1391)))),_tmp1390)))));};}_LL4FE: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp9B7=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp9B7->tag != 18)goto _LL500;else{_tmp9B8=(void*)_tmp9B7->f1;_tmp9B9=(void*)_tmp9B7->f2;}}_LL4FF: {
# 3058
void*_tmp9E5=Cyc_Tcutil_rsubstitute(r,inst,_tmp9B8);
# 3059
if(_tmp9E5 == _tmp9B8)return e;{
# 3060
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp1394;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp1393;return Cyc_Tcutil_copye(e,(void*)((_tmp1393=_cycalloc(sizeof(*_tmp1393)),((_tmp1393[0]=((_tmp1394.tag=18,((_tmp1394.f1=(void*)_tmp9E5,((_tmp1394.f2=(void*)_tmp9B9,_tmp1394)))))),_tmp1393)))));};}_LL500: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp9BA=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp999;if(_tmp9BA->tag != 37)goto _LL502;else{_tmp9BB=(void*)_tmp9BA->f1;}}_LL501: {
# 3062
void*_tmp9E8=Cyc_Tcutil_rsubstitute(r,inst,_tmp9BB);
# 3063
if(_tmp9E8 == _tmp9BB)return e;{
# 3064
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp1397;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1396;return Cyc_Tcutil_copye(e,(void*)((_tmp1396=_cycalloc(sizeof(*_tmp1396)),((_tmp1396[0]=((_tmp1397.tag=37,((_tmp1397.f1=(void*)_tmp9E8,_tmp1397)))),_tmp1396)))));};}_LL502:;_LL503: {
# 3066
const char*_tmp139A;void*_tmp1399;return(_tmp1399=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp139A="non-type-level-expression in Tcutil::rsubsexp",_tag_dyneither(_tmp139A,sizeof(char),46))),_tag_dyneither(_tmp1399,sizeof(void*),0)));}_LL4E5:;}
# 3070
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3073
void*_tmp9ED=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp9EF;struct Cyc_Absyn_AggrInfo _tmp9F1;union Cyc_Absyn_AggrInfoU _tmp9F2;struct Cyc_List_List*_tmp9F3;struct Cyc_Absyn_DatatypeInfo _tmp9F5;union Cyc_Absyn_DatatypeInfoU _tmp9F6;struct Cyc_List_List*_tmp9F7;struct Cyc_Absyn_DatatypeFieldInfo _tmp9F9;union Cyc_Absyn_DatatypeFieldInfoU _tmp9FA;struct Cyc_List_List*_tmp9FB;struct _tuple2*_tmp9FD;struct Cyc_List_List*_tmp9FE;struct Cyc_Absyn_Typedefdecl*_tmp9FF;void*_tmpA00;struct Cyc_Absyn_ArrayInfo _tmpA02;void*_tmpA03;struct Cyc_Absyn_Tqual _tmpA04;struct Cyc_Absyn_Exp*_tmpA05;union Cyc_Absyn_Constraint*_tmpA06;unsigned int _tmpA07;struct Cyc_Absyn_PtrInfo _tmpA09;void*_tmpA0A;struct Cyc_Absyn_Tqual _tmpA0B;struct Cyc_Absyn_PtrAtts _tmpA0C;void*_tmpA0D;union Cyc_Absyn_Constraint*_tmpA0E;union Cyc_Absyn_Constraint*_tmpA0F;union Cyc_Absyn_Constraint*_tmpA10;struct Cyc_Absyn_FnInfo _tmpA12;struct Cyc_List_List*_tmpA13;void*_tmpA14;struct Cyc_Absyn_Tqual _tmpA15;void*_tmpA16;struct Cyc_List_List*_tmpA17;int _tmpA18;struct Cyc_Absyn_VarargInfo*_tmpA19;struct Cyc_List_List*_tmpA1A;struct Cyc_List_List*_tmpA1B;struct Cyc_List_List*_tmpA1D;enum Cyc_Absyn_AggrKind _tmpA1F;struct Cyc_List_List*_tmpA20;void*_tmpA22;void*_tmpA24;void*_tmpA26;void*_tmpA27;void*_tmpA29;struct Cyc_Absyn_Exp*_tmpA2B;void*_tmpA35;void*_tmpA37;struct Cyc_List_List*_tmpA39;_LL505: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9EE=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9ED;if(_tmp9EE->tag != 2)goto _LL507;else{_tmp9EF=_tmp9EE->f1;}}_LL506: {
# 3076
struct _handler_cons _tmpA3B;_push_handler(& _tmpA3B);{int _tmpA3D=0;if(setjmp(_tmpA3B.handler))_tmpA3D=1;if(!_tmpA3D){{void*_tmpA3E=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp9EF);_npop_handler(0);return _tmpA3E;};_pop_handler();}else{void*_tmpA3C=(void*)_exn_thrown;void*_tmpA40=_tmpA3C;_LL53C: {struct Cyc_Core_Not_found_exn_struct*_tmpA41=(struct Cyc_Core_Not_found_exn_struct*)_tmpA40;if(_tmpA41->tag != Cyc_Core_Not_found)goto _LL53E;}_LL53D:
# 3077
 return t;_LL53E:;_LL53F:(void)_throw(_tmpA40);_LL53B:;}};}_LL507: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp9F0=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9ED;if(_tmp9F0->tag != 11)goto _LL509;else{_tmp9F1=_tmp9F0->f1;_tmp9F2=_tmp9F1.aggr_info;_tmp9F3=_tmp9F1.targs;}}_LL508: {
# 3079
struct Cyc_List_List*_tmpA42=Cyc_Tcutil_substs(rgn,inst,_tmp9F3);
# 3080
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp13A0;struct Cyc_Absyn_AggrInfo _tmp139F;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp139E;return _tmpA42 == _tmp9F3?t:(void*)((_tmp139E=_cycalloc(sizeof(*_tmp139E)),((_tmp139E[0]=((_tmp13A0.tag=11,((_tmp13A0.f1=((_tmp139F.aggr_info=_tmp9F2,((_tmp139F.targs=_tmpA42,_tmp139F)))),_tmp13A0)))),_tmp139E))));}_LL509: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp9F4=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp9ED;if(_tmp9F4->tag != 3)goto _LL50B;else{_tmp9F5=_tmp9F4->f1;_tmp9F6=_tmp9F5.datatype_info;_tmp9F7=_tmp9F5.targs;}}_LL50A: {
# 3082
struct Cyc_List_List*_tmpA46=Cyc_Tcutil_substs(rgn,inst,_tmp9F7);
# 3083
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp13A6;struct Cyc_Absyn_DatatypeInfo _tmp13A5;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp13A4;return _tmpA46 == _tmp9F7?t:(void*)(
# 3084
(_tmp13A4=_cycalloc(sizeof(*_tmp13A4)),((_tmp13A4[0]=((_tmp13A6.tag=3,((_tmp13A6.f1=((_tmp13A5.datatype_info=_tmp9F6,((_tmp13A5.targs=_tmpA46,_tmp13A5)))),_tmp13A6)))),_tmp13A4))));}_LL50B: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp9F8=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp9ED;if(_tmp9F8->tag != 4)goto _LL50D;else{_tmp9F9=_tmp9F8->f1;_tmp9FA=_tmp9F9.field_info;_tmp9FB=_tmp9F9.targs;}}_LL50C: {
# 3086
struct Cyc_List_List*_tmpA4A=Cyc_Tcutil_substs(rgn,inst,_tmp9FB);
# 3087
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp13AC;struct Cyc_Absyn_DatatypeFieldInfo _tmp13AB;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp13AA;return _tmpA4A == _tmp9FB?t:(void*)(
# 3088
(_tmp13AA=_cycalloc(sizeof(*_tmp13AA)),((_tmp13AA[0]=((_tmp13AC.tag=4,((_tmp13AC.f1=((_tmp13AB.field_info=_tmp9FA,((_tmp13AB.targs=_tmpA4A,_tmp13AB)))),_tmp13AC)))),_tmp13AA))));}_LL50D: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp9FC=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9ED;if(_tmp9FC->tag != 17)goto _LL50F;else{_tmp9FD=_tmp9FC->f1;_tmp9FE=_tmp9FC->f2;_tmp9FF=_tmp9FC->f3;_tmpA00=(void*)_tmp9FC->f4;}}_LL50E: {
# 3090
struct Cyc_List_List*_tmpA4E=Cyc_Tcutil_substs(rgn,inst,_tmp9FE);
# 3091
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp13AF;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp13AE;return _tmpA4E == _tmp9FE?t:(void*)((_tmp13AE=_cycalloc(sizeof(*_tmp13AE)),((_tmp13AE[0]=((_tmp13AF.tag=17,((_tmp13AF.f1=_tmp9FD,((_tmp13AF.f2=_tmpA4E,((_tmp13AF.f3=_tmp9FF,((_tmp13AF.f4=(void*)_tmpA00,_tmp13AF)))))))))),_tmp13AE))));}_LL50F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpA01=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9ED;if(_tmpA01->tag != 8)goto _LL511;else{_tmpA02=_tmpA01->f1;_tmpA03=_tmpA02.elt_type;_tmpA04=_tmpA02.tq;_tmpA05=_tmpA02.num_elts;_tmpA06=_tmpA02.zero_term;_tmpA07=_tmpA02.zt_loc;}}_LL510: {
# 3093
void*_tmpA51=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA03);
# 3094
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp13B5;struct Cyc_Absyn_ArrayInfo _tmp13B4;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp13B3;return _tmpA51 == _tmpA03?t:(void*)((_tmp13B3=_cycalloc(sizeof(*_tmp13B3)),((_tmp13B3[0]=((_tmp13B5.tag=8,((_tmp13B5.f1=((_tmp13B4.elt_type=_tmpA51,((_tmp13B4.tq=_tmpA04,((_tmp13B4.num_elts=_tmpA05,((_tmp13B4.zero_term=_tmpA06,((_tmp13B4.zt_loc=_tmpA07,_tmp13B4)))))))))),_tmp13B5)))),_tmp13B3))));}_LL511: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA08=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9ED;if(_tmpA08->tag != 5)goto _LL513;else{_tmpA09=_tmpA08->f1;_tmpA0A=_tmpA09.elt_typ;_tmpA0B=_tmpA09.elt_tq;_tmpA0C=_tmpA09.ptr_atts;_tmpA0D=_tmpA0C.rgn;_tmpA0E=_tmpA0C.nullable;_tmpA0F=_tmpA0C.bounds;_tmpA10=_tmpA0C.zero_term;}}_LL512: {
# 3096
void*_tmpA55=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA0A);
# 3097
void*_tmpA56=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA0D);
# 3098
union Cyc_Absyn_Constraint*_tmpA57=_tmpA0F;
# 3099
{void*_tmpA58=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpA0F);struct Cyc_Absyn_Exp*_tmpA5A;_LL541: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA59=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpA58;if(_tmpA59->tag != 1)goto _LL543;else{_tmpA5A=_tmpA59->f1;}}_LL542: {
# 3101
struct Cyc_Absyn_Exp*_tmpA5B=Cyc_Tcutil_rsubsexp(rgn,inst,_tmpA5A);
# 3102
if(_tmpA5B != _tmpA5A){
# 3103
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp13B8;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp13B7;_tmpA57=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp13B7=_cycalloc(sizeof(*_tmp13B7)),((_tmp13B7[0]=((_tmp13B8.tag=1,((_tmp13B8.f1=_tmpA5B,_tmp13B8)))),_tmp13B7)))));}
# 3104
goto _LL540;}_LL543:;_LL544:
# 3105
 goto _LL540;_LL540:;}
# 3107
if((_tmpA55 == _tmpA0A  && _tmpA56 == _tmpA0D) && _tmpA57 == _tmpA0F)
# 3108
return t;{
# 3109
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp13C2;struct Cyc_Absyn_PtrAtts _tmp13C1;struct Cyc_Absyn_PtrInfo _tmp13C0;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp13BF;return(void*)((_tmp13BF=_cycalloc(sizeof(*_tmp13BF)),((_tmp13BF[0]=((_tmp13C2.tag=5,((_tmp13C2.f1=((_tmp13C0.elt_typ=_tmpA55,((_tmp13C0.elt_tq=_tmpA0B,((_tmp13C0.ptr_atts=((_tmp13C1.rgn=_tmpA56,((_tmp13C1.nullable=_tmpA0E,((_tmp13C1.bounds=_tmpA57,((_tmp13C1.zero_term=_tmpA10,((_tmp13C1.ptrloc=0,_tmp13C1)))))))))),_tmp13C0)))))),_tmp13C2)))),_tmp13BF))));};}_LL513: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA11=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9ED;if(_tmpA11->tag != 9)goto _LL515;else{_tmpA12=_tmpA11->f1;_tmpA13=_tmpA12.tvars;_tmpA14=_tmpA12.effect;_tmpA15=_tmpA12.ret_tqual;_tmpA16=_tmpA12.ret_typ;_tmpA17=_tmpA12.args;_tmpA18=_tmpA12.c_varargs;_tmpA19=_tmpA12.cyc_varargs;_tmpA1A=_tmpA12.rgn_po;_tmpA1B=_tmpA12.attributes;}}_LL514:
# 3112
{struct Cyc_List_List*_tmpA62=_tmpA13;for(0;_tmpA62 != 0;_tmpA62=_tmpA62->tl){
# 3113
struct _tuple16*_tmp13CC;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp13CB;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp13CA;struct Cyc_List_List*_tmp13C9;inst=((_tmp13C9=_region_malloc(rgn,sizeof(*_tmp13C9)),((_tmp13C9->hd=((_tmp13CC=_region_malloc(rgn,sizeof(*_tmp13CC)),((_tmp13CC->f1=(struct Cyc_Absyn_Tvar*)_tmpA62->hd,((_tmp13CC->f2=(void*)((_tmp13CA=_cycalloc(sizeof(*_tmp13CA)),((_tmp13CA[0]=((_tmp13CB.tag=2,((_tmp13CB.f1=(struct Cyc_Absyn_Tvar*)_tmpA62->hd,_tmp13CB)))),_tmp13CA)))),_tmp13CC)))))),((_tmp13C9->tl=inst,_tmp13C9))))));}}{
# 3114
struct Cyc_List_List*_tmpA68;struct Cyc_List_List*_tmpA69;struct _tuple1 _tmpA67=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
# 3115
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmpA17));_tmpA68=_tmpA67.f1;_tmpA69=_tmpA67.f2;{
# 3116
struct Cyc_List_List*_tmpA6A=_tmpA17;
# 3117
struct Cyc_List_List*_tmpA6B=Cyc_Tcutil_substs(rgn,inst,_tmpA69);
# 3118
if(_tmpA6B != _tmpA69)
# 3119
_tmpA6A=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmpA17,_tmpA6B);{
# 3120
void*eff2;
# 3121
if(_tmpA14 == 0)
# 3122
eff2=0;else{
# 3124
void*new_eff=(void*)Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmpA14);
# 3125
if(new_eff == _tmpA14)
# 3126
eff2=_tmpA14;else{
# 3128
eff2=new_eff;}}{
# 3130
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
# 3131
if(_tmpA19 == 0)
# 3132
cyc_varargs2=0;else{
# 3134
struct _dyneither_ptr*_tmpA6D;struct Cyc_Absyn_Tqual _tmpA6E;void*_tmpA6F;int _tmpA70;struct Cyc_Absyn_VarargInfo _tmpA6C=*_tmpA19;_tmpA6D=_tmpA6C.name;_tmpA6E=_tmpA6C.tq;_tmpA6F=_tmpA6C.type;_tmpA70=_tmpA6C.inject;{
# 3135
void*_tmpA71=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA6F);
# 3136
if(_tmpA71 == _tmpA6F)cyc_varargs2=_tmpA19;else{
# 3138
struct Cyc_Absyn_VarargInfo*_tmp13CD;cyc_varargs2=((_tmp13CD=_cycalloc(sizeof(*_tmp13CD)),((_tmp13CD->name=_tmpA6D,((_tmp13CD->tq=_tmpA6E,((_tmp13CD->type=_tmpA71,((_tmp13CD->inject=_tmpA70,_tmp13CD))))))))));}};}{
# 3140
struct Cyc_List_List*rgn_po2;
# 3141
struct Cyc_List_List*_tmpA74;struct Cyc_List_List*_tmpA75;struct _tuple1 _tmpA73=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,_tmpA1A);_tmpA74=_tmpA73.f1;_tmpA75=_tmpA73.f2;{
# 3142
struct Cyc_List_List*_tmpA76=Cyc_Tcutil_substs(rgn,inst,_tmpA74);
# 3143
struct Cyc_List_List*_tmpA77=Cyc_Tcutil_substs(rgn,inst,_tmpA75);
# 3144
if(_tmpA76 == _tmpA74  && _tmpA77 == _tmpA75)
# 3145
rgn_po2=_tmpA1A;else{
# 3147
rgn_po2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmpA76,_tmpA77);}{
# 3148
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp13D3;struct Cyc_Absyn_FnInfo _tmp13D2;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp13D1;return(void*)(
# 3149
(_tmp13D1=_cycalloc(sizeof(*_tmp13D1)),((_tmp13D1[0]=((_tmp13D3.tag=9,((_tmp13D3.f1=((_tmp13D2.tvars=_tmpA13,((_tmp13D2.effect=eff2,((_tmp13D2.ret_tqual=_tmpA15,((_tmp13D2.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA16),((_tmp13D2.args=_tmpA6A,((_tmp13D2.c_varargs=_tmpA18,((_tmp13D2.cyc_varargs=cyc_varargs2,((_tmp13D2.rgn_po=rgn_po2,((_tmp13D2.attributes=_tmpA1B,_tmp13D2)))))))))))))))))),_tmp13D3)))),_tmp13D1))));};};};};};};};_LL515: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpA1C=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp9ED;if(_tmpA1C->tag != 10)goto _LL517;else{_tmpA1D=_tmpA1C->f1;}}_LL516: {
# 3152
struct Cyc_List_List*ts2=0;
# 3153
int change=0;
# 3154
{struct Cyc_List_List*_tmpA7B=_tmpA1D;for(0;_tmpA7B != 0;_tmpA7B=_tmpA7B->tl){
# 3155
void*_tmpA7C=(*((struct _tuple12*)_tmpA7B->hd)).f2;
# 3156
void*_tmpA7D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA7C);
# 3157
if(_tmpA7C != _tmpA7D)
# 3158
change=1;{
# 3160
struct Cyc_List_List*_tmp13D4;ts2=((_tmp13D4=_region_malloc(rgn,sizeof(*_tmp13D4)),((_tmp13D4->hd=_tmpA7D,((_tmp13D4->tl=ts2,_tmp13D4))))));};}}
# 3162
if(!change)
# 3163
return t;{
# 3164
struct Cyc_List_List*_tmpA7F=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmpA1D,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
# 3165
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp13D7;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp13D6;return(void*)((_tmp13D6=_cycalloc(sizeof(*_tmp13D6)),((_tmp13D6[0]=((_tmp13D7.tag=10,((_tmp13D7.f1=_tmpA7F,_tmp13D7)))),_tmp13D6))));};}_LL517: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpA1E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp9ED;if(_tmpA1E->tag != 12)goto _LL519;else{_tmpA1F=_tmpA1E->f1;_tmpA20=_tmpA1E->f2;}}_LL518: {
# 3167
struct Cyc_List_List*_tmpA82=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,_tmpA20);
# 3168
struct Cyc_List_List*_tmpA83=Cyc_Tcutil_substs(rgn,inst,_tmpA82);
# 3169
if(_tmpA83 == _tmpA82)
# 3170
return t;{
# 3171
struct Cyc_List_List*_tmpA84=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmpA20,_tmpA83);
# 3172
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp13DA;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp13D9;return(void*)((_tmp13D9=_cycalloc(sizeof(*_tmp13D9)),((_tmp13D9[0]=((_tmp13DA.tag=12,((_tmp13DA.f1=_tmpA1F,((_tmp13DA.f2=_tmpA84,_tmp13DA)))))),_tmp13D9))));};}_LL519: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpA21=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp9ED;if(_tmpA21->tag != 1)goto _LL51B;else{_tmpA22=(void*)_tmpA21->f2;}}_LL51A:
# 3174
 if(_tmpA22 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmpA22);else{
# 3175
return t;}_LL51B: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA23=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp9ED;if(_tmpA23->tag != 15)goto _LL51D;else{_tmpA24=(void*)_tmpA23->f1;}}_LL51C: {
# 3177
void*_tmpA87=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA24);
# 3178
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp13DD;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp13DC;return _tmpA87 == _tmpA24?t:(void*)((_tmp13DC=_cycalloc(sizeof(*_tmp13DC)),((_tmp13DC[0]=((_tmp13DD.tag=15,((_tmp13DD.f1=(void*)_tmpA87,_tmp13DD)))),_tmp13DC))));}_LL51D: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpA25=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp9ED;if(_tmpA25->tag != 16)goto _LL51F;else{_tmpA26=(void*)_tmpA25->f1;_tmpA27=(void*)_tmpA25->f2;}}_LL51E: {
# 3180
void*_tmpA8A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA26);
# 3181
void*_tmpA8B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA27);
# 3182
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp13E0;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp13DF;return _tmpA8A == _tmpA26  && _tmpA8B == _tmpA27?t:(void*)((_tmp13DF=_cycalloc(sizeof(*_tmp13DF)),((_tmp13DF[0]=((_tmp13E0.tag=16,((_tmp13E0.f1=(void*)_tmpA8A,((_tmp13E0.f2=(void*)_tmpA8B,_tmp13E0)))))),_tmp13DF))));}_LL51F: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpA28=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp9ED;if(_tmpA28->tag != 19)goto _LL521;else{_tmpA29=(void*)_tmpA28->f1;}}_LL520: {
# 3184
void*_tmpA8E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA29);
# 3185
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp13E3;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp13E2;return _tmpA8E == _tmpA29?t:(void*)((_tmp13E2=_cycalloc(sizeof(*_tmp13E2)),((_tmp13E2[0]=((_tmp13E3.tag=19,((_tmp13E3.f1=(void*)_tmpA8E,_tmp13E3)))),_tmp13E2))));}_LL521: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpA2A=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp9ED;if(_tmpA2A->tag != 18)goto _LL523;else{_tmpA2B=_tmpA2A->f1;}}_LL522: {
# 3187
struct Cyc_Absyn_Exp*_tmpA91=Cyc_Tcutil_rsubsexp(rgn,inst,_tmpA2B);
# 3188
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp13E6;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp13E5;return _tmpA91 == _tmpA2B?t:(void*)((_tmp13E5=_cycalloc(sizeof(*_tmp13E5)),((_tmp13E5[0]=((_tmp13E6.tag=18,((_tmp13E6.f1=_tmpA91,_tmp13E6)))),_tmp13E5))));}_LL523: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpA2C=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp9ED;if(_tmpA2C->tag != 13)goto _LL525;}_LL524:
# 3189
 goto _LL526;_LL525: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpA2D=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp9ED;if(_tmpA2D->tag != 14)goto _LL527;}_LL526:
# 3190
 goto _LL528;_LL527: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpA2E=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp9ED;if(_tmpA2E->tag != 0)goto _LL529;}_LL528:
# 3191
 goto _LL52A;_LL529: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpA2F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp9ED;if(_tmpA2F->tag != 6)goto _LL52B;}_LL52A:
# 3192
 goto _LL52C;_LL52B: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpA30=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp9ED;if(_tmpA30->tag != 7)goto _LL52D;}_LL52C:
# 3193
 goto _LL52E;_LL52D: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpA31=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp9ED;if(_tmpA31->tag != 22)goto _LL52F;}_LL52E:
# 3194
 goto _LL530;_LL52F: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpA32=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp9ED;if(_tmpA32->tag != 21)goto _LL531;}_LL530:
# 3195
 goto _LL532;_LL531: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpA33=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp9ED;if(_tmpA33->tag != 20)goto _LL533;}_LL532:
# 3196
 return t;_LL533: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpA34=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp9ED;if(_tmpA34->tag != 25)goto _LL535;else{_tmpA35=(void*)_tmpA34->f1;}}_LL534: {
# 3198
void*_tmpA94=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA35);
# 3199
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp13E9;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp13E8;return _tmpA94 == _tmpA35?t:(void*)((_tmp13E8=_cycalloc(sizeof(*_tmp13E8)),((_tmp13E8[0]=((_tmp13E9.tag=25,((_tmp13E9.f1=(void*)_tmpA94,_tmp13E9)))),_tmp13E8))));}_LL535: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpA36=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp9ED;if(_tmpA36->tag != 23)goto _LL537;else{_tmpA37=(void*)_tmpA36->f1;}}_LL536: {
# 3201
void*_tmpA97=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA37);
# 3202
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp13EC;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp13EB;return _tmpA97 == _tmpA37?t:(void*)((_tmp13EB=_cycalloc(sizeof(*_tmp13EB)),((_tmp13EB[0]=((_tmp13EC.tag=23,((_tmp13EC.f1=(void*)_tmpA97,_tmp13EC)))),_tmp13EB))));}_LL537: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpA38=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp9ED;if(_tmpA38->tag != 24)goto _LL539;else{_tmpA39=_tmpA38->f1;}}_LL538: {
# 3204
struct Cyc_List_List*_tmpA9A=Cyc_Tcutil_substs(rgn,inst,_tmpA39);
# 3205
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp13EF;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp13EE;return _tmpA9A == _tmpA39?t:(void*)((_tmp13EE=_cycalloc(sizeof(*_tmp13EE)),((_tmp13EE[0]=((_tmp13EF.tag=24,((_tmp13EF.f1=_tmpA9A,_tmp13EF)))),_tmp13EE))));}_LL539: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpA3A=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp9ED;if(_tmpA3A->tag != 26)goto _LL504;}_LL53A: {
# 3206
const char*_tmp13F2;void*_tmp13F1;(_tmp13F1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13F2="found typedecltype in rsubs",_tag_dyneither(_tmp13F2,sizeof(char),28))),_tag_dyneither(_tmp13F1,sizeof(void*),0)));}_LL504:;}
# 3210
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3213
if(ts == 0)
# 3214
return 0;{
# 3215
void*_tmpA9F=(void*)ts->hd;
# 3216
struct Cyc_List_List*_tmpAA0=ts->tl;
# 3217
void*_tmpAA1=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA9F);
# 3218
struct Cyc_List_List*_tmpAA2=Cyc_Tcutil_substs(rgn,inst,_tmpAA0);
# 3219
if(_tmpA9F == _tmpAA1  && _tmpAA0 == _tmpAA2)
# 3220
return ts;{
# 3221
struct Cyc_List_List*_tmp13F3;return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmp13F3=_cycalloc(sizeof(*_tmp13F3)),((_tmp13F3->hd=_tmpAA1,((_tmp13F3->tl=_tmpAA2,_tmp13F3)))))));};};}
# 3224
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
# 3225
if(inst != 0)
# 3226
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
# 3227
return t;}}
# 3231
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
# 3232
struct Cyc_Core_Opt*_tmpAA4=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
# 3233
struct Cyc_Core_Opt*_tmp13F6;struct _tuple16*_tmp13F5;return(_tmp13F5=_cycalloc(sizeof(*_tmp13F5)),((_tmp13F5->f1=tv,((_tmp13F5->f2=Cyc_Absyn_new_evar(_tmpAA4,((_tmp13F6=_cycalloc(sizeof(*_tmp13F6)),((_tmp13F6->v=s,_tmp13F6))))),_tmp13F5)))));}
# 3236
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3238
struct _tuple17 _tmpAA8;struct Cyc_List_List*_tmpAA9;struct _RegionHandle*_tmpAAA;struct _tuple17*_tmpAA7=env;_tmpAA8=*_tmpAA7;_tmpAA9=_tmpAA8.f1;_tmpAAA=_tmpAA8.f2;{
# 3239
struct Cyc_Core_Opt*_tmpAAB=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
# 3240
struct Cyc_Core_Opt*_tmp13F9;struct _tuple16*_tmp13F8;return(_tmp13F8=_region_malloc(_tmpAAA,sizeof(*_tmp13F8)),((_tmp13F8->f1=tv,((_tmp13F8->f2=Cyc_Absyn_new_evar(_tmpAAB,((_tmp13F9=_cycalloc(sizeof(*_tmp13F9)),((_tmp13F9->v=_tmpAA9,_tmp13F9))))),_tmp13F8)))));};}
# 3248
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3252
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3253
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
# 3254
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
# 3255
void*k2=tv->kind;
# 3256
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){
# 3257
const char*_tmp13FF;void*_tmp13FE[3];struct Cyc_String_pa_PrintArg_struct _tmp13FD;struct Cyc_String_pa_PrintArg_struct _tmp13FC;struct Cyc_String_pa_PrintArg_struct _tmp13FB;(_tmp13FB.tag=0,((_tmp13FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3258
Cyc_Absynpp_kindbound2string(k2)),((_tmp13FC.tag=0,((_tmp13FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((_tmp13FD.tag=0,((_tmp13FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp13FE[0]=& _tmp13FD,((_tmp13FE[1]=& _tmp13FC,((_tmp13FE[2]=& _tmp13FB,Cyc_Tcutil_terr(loc,((_tmp13FF="type variable %s is used with inconsistent kinds %s and %s",_tag_dyneither(_tmp13FF,sizeof(char),59))),_tag_dyneither(_tmp13FE,sizeof(void*),3)))))))))))))))))));}
# 3259
if(tv->identity == - 1)
# 3260
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
# 3261
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
# 3262
const char*_tmp1402;void*_tmp1401;(_tmp1401=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1402="same type variable has different identity!",_tag_dyneither(_tmp1402,sizeof(char),43))),_tag_dyneither(_tmp1401,sizeof(void*),0)));}}
# 3263
return tvs;}}}
# 3266
tv->identity=Cyc_Tcutil_new_tvar_id();{
# 3267
struct Cyc_List_List*_tmp1403;return(_tmp1403=_cycalloc(sizeof(*_tmp1403)),((_tmp1403->hd=tv,((_tmp1403->tl=tvs,_tmp1403)))));};}
# 3272
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3274
if(tv->identity == - 1){
# 3275
const char*_tmp1406;void*_tmp1405;(_tmp1405=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1406="fast_add_free_tvar: bad identity in tv",_tag_dyneither(_tmp1406,sizeof(char),39))),_tag_dyneither(_tmp1405,sizeof(void*),0)));}
# 3276
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3278
struct Cyc_Absyn_Tvar*_tmpAB8=(struct Cyc_Absyn_Tvar*)tvs2->hd;
# 3279
if(_tmpAB8->identity == - 1){
# 3280
const char*_tmp1409;void*_tmp1408;(_tmp1408=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1409="fast_add_free_tvar: bad identity in tvs2",_tag_dyneither(_tmp1409,sizeof(char),41))),_tag_dyneither(_tmp1408,sizeof(void*),0)));}
# 3281
if(_tmpAB8->identity == tv->identity)
# 3282
return tvs;}}{
# 3285
struct Cyc_List_List*_tmp140A;return(_tmp140A=_cycalloc(sizeof(*_tmp140A)),((_tmp140A->hd=tv,((_tmp140A->tl=tvs,_tmp140A)))));};}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3291
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3296
if(tv->identity == - 1){
# 3297
const char*_tmp140D;void*_tmp140C;(_tmp140C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp140D="fast_add_free_tvar_bool: bad identity in tv",_tag_dyneither(_tmp140D,sizeof(char),44))),_tag_dyneither(_tmp140C,sizeof(void*),0)));}
# 3298
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3300
struct _tuple28 _tmpABF;struct Cyc_Absyn_Tvar*_tmpAC0;int _tmpAC1;int*_tmpAC2;struct _tuple28*_tmpABE=(struct _tuple28*)tvs2->hd;_tmpABF=*_tmpABE;_tmpAC0=_tmpABF.f1;_tmpAC1=_tmpABF.f2;_tmpAC2=(int*)&(*_tmpABE).f2;
# 3301
if(_tmpAC0->identity == - 1){
# 3302
const char*_tmp1410;void*_tmp140F;(_tmp140F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1410="fast_add_free_tvar_bool: bad identity in tvs2",_tag_dyneither(_tmp1410,sizeof(char),46))),_tag_dyneither(_tmp140F,sizeof(void*),0)));}
# 3303
if(_tmpAC0->identity == tv->identity){
# 3304
*_tmpAC2=*_tmpAC2  || b;
# 3305
return tvs;}}}{
# 3308
struct _tuple28*_tmp1413;struct Cyc_List_List*_tmp1412;return(_tmp1412=_region_malloc(r,sizeof(*_tmp1412)),((_tmp1412->hd=((_tmp1413=_region_malloc(r,sizeof(*_tmp1413)),((_tmp1413->f1=tv,((_tmp1413->f2=b,_tmp1413)))))),((_tmp1412->tl=tvs,_tmp1412)))));};}
# 3312
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3314
if(tv->identity == - 1){
# 3315
const char*_tmp1417;void*_tmp1416[1];struct Cyc_String_pa_PrintArg_struct _tmp1415;(_tmp1415.tag=0,((_tmp1415.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv)),((_tmp1416[0]=& _tmp1415,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1417="bound tvar id for %s is NULL",_tag_dyneither(_tmp1417,sizeof(char),29))),_tag_dyneither(_tmp1416,sizeof(void*),1)))))));}{
# 3316
struct Cyc_List_List*_tmp1418;return(_tmp1418=_cycalloc(sizeof(*_tmp1418)),((_tmp1418->hd=tv,((_tmp1418->tl=tvs,_tmp1418)))));};}struct _tuple29{void*f1;int f2;};
# 3323
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3326
void*_tmpACB=Cyc_Tcutil_compress(e);int _tmpACD;_LL546: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpACC=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpACB;if(_tmpACC->tag != 1)goto _LL548;else{_tmpACD=_tmpACC->f3;}}_LL547:
# 3328
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
# 3329
struct _tuple29 _tmpACF;void*_tmpAD0;int _tmpAD1;int*_tmpAD2;struct _tuple29*_tmpACE=(struct _tuple29*)es2->hd;_tmpACF=*_tmpACE;_tmpAD0=_tmpACF.f1;_tmpAD1=_tmpACF.f2;_tmpAD2=(int*)&(*_tmpACE).f2;{
# 3330
void*_tmpAD3=Cyc_Tcutil_compress(_tmpAD0);int _tmpAD5;_LL54B: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpAD4=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpAD3;if(_tmpAD4->tag != 1)goto _LL54D;else{_tmpAD5=_tmpAD4->f3;}}_LL54C:
# 3332
 if(_tmpACD == _tmpAD5){
# 3333
if(b != *_tmpAD2)*_tmpAD2=1;
# 3334
return es;}
# 3336
goto _LL54A;_LL54D:;_LL54E:
# 3337
 goto _LL54A;_LL54A:;};}}{
# 3340
struct _tuple29*_tmp141B;struct Cyc_List_List*_tmp141A;return(_tmp141A=_region_malloc(r,sizeof(*_tmp141A)),((_tmp141A->hd=((_tmp141B=_region_malloc(r,sizeof(*_tmp141B)),((_tmp141B->f1=e,((_tmp141B->f2=b,_tmp141B)))))),((_tmp141A->tl=es,_tmp141A)))));};_LL548:;_LL549:
# 3341
 return es;_LL545:;}
# 3345
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3348
struct Cyc_List_List*r=0;
# 3349
for(0;tvs != 0;tvs=tvs->tl){
# 3350
int present=0;
# 3351
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
# 3352
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
# 3353
present=1;
# 3354
break;}}}
# 3357
if(!present){struct Cyc_List_List*_tmp141C;r=((_tmp141C=_region_malloc(rgn,sizeof(*_tmp141C)),((_tmp141C->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp141C->tl=r,_tmp141C))))));}}
# 3359
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
# 3360
return r;}
# 3364
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3368
struct Cyc_List_List*res=0;
# 3369
for(0;tvs != 0;tvs=tvs->tl){
# 3370
struct Cyc_Absyn_Tvar*_tmpADA;int _tmpADB;struct _tuple28 _tmpAD9=*((struct _tuple28*)tvs->hd);_tmpADA=_tmpAD9.f1;_tmpADB=_tmpAD9.f2;{
# 3371
int present=0;
# 3372
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
# 3373
if(_tmpADA->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
# 3374
present=1;
# 3375
break;}}}
# 3378
if(!present){struct Cyc_List_List*_tmp141D;res=((_tmp141D=_region_malloc(r,sizeof(*_tmp141D)),((_tmp141D->hd=(struct _tuple28*)tvs->hd,((_tmp141D->tl=res,_tmp141D))))));}};}
# 3380
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
# 3381
return res;}
# 3384
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3386
if(width != 0){
# 3387
unsigned int w=0;
# 3388
if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)width)){
# 3389
const char*_tmp1421;void*_tmp1420[1];struct Cyc_String_pa_PrintArg_struct _tmp141F;(_tmp141F.tag=0,((_tmp141F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1420[0]=& _tmp141F,Cyc_Tcutil_terr(loc,((_tmp1421="bitfield %s does not have constant width",_tag_dyneither(_tmp1421,sizeof(char),41))),_tag_dyneither(_tmp1420,sizeof(void*),1)))))));}else{
# 3391
unsigned int _tmpAE1;int _tmpAE2;struct _tuple14 _tmpAE0=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)width);_tmpAE1=_tmpAE0.f1;_tmpAE2=_tmpAE0.f2;
# 3392
if(!_tmpAE2){
# 3393
const char*_tmp1424;void*_tmp1423;(_tmp1423=0,Cyc_Tcutil_terr(loc,((_tmp1424="bitfield width cannot use sizeof or offsetof",_tag_dyneither(_tmp1424,sizeof(char),45))),_tag_dyneither(_tmp1423,sizeof(void*),0)));}
# 3394
w=_tmpAE1;}{
# 3396
void*_tmpAE5=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of _tmpAE7;_LL550: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpAE6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpAE5;if(_tmpAE6->tag != 6)goto _LL552;else{_tmpAE7=_tmpAE6->f2;}}_LL551:
# 3399
 switch(_tmpAE7){case Cyc_Absyn_Char_sz: _LL554:
# 3400
 if(w > 8){const char*_tmp1427;void*_tmp1426;(_tmp1426=0,Cyc_Tcutil_terr(loc,((_tmp1427="bitfield larger than type",_tag_dyneither(_tmp1427,sizeof(char),26))),_tag_dyneither(_tmp1426,sizeof(void*),0)));}break;case Cyc_Absyn_Short_sz: _LL555:
# 3401
 if(w > 16){const char*_tmp142A;void*_tmp1429;(_tmp1429=0,Cyc_Tcutil_terr(loc,((_tmp142A="bitfield larger than type",_tag_dyneither(_tmp142A,sizeof(char),26))),_tag_dyneither(_tmp1429,sizeof(void*),0)));}break;case Cyc_Absyn_Long_sz: _LL556:
# 3402
 goto _LL557;case Cyc_Absyn_Int_sz: _LL557:
# 3404
 if(w > 32){const char*_tmp142D;void*_tmp142C;(_tmp142C=0,Cyc_Tcutil_terr(loc,((_tmp142D="bitfield larger than type",_tag_dyneither(_tmp142D,sizeof(char),26))),_tag_dyneither(_tmp142C,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz: _LL558:
# 3406
 if(w > 64){const char*_tmp1430;void*_tmp142F;(_tmp142F=0,Cyc_Tcutil_terr(loc,((_tmp1430="bitfield larger than type",_tag_dyneither(_tmp1430,sizeof(char),26))),_tag_dyneither(_tmp142F,sizeof(void*),0)));}break;}
# 3408
goto _LL54F;_LL552:;_LL553:
# 3410
{const char*_tmp1435;void*_tmp1434[2];struct Cyc_String_pa_PrintArg_struct _tmp1433;struct Cyc_String_pa_PrintArg_struct _tmp1432;(_tmp1432.tag=0,((_tmp1432.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3411
Cyc_Absynpp_typ2string(field_typ)),((_tmp1433.tag=0,((_tmp1433.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1434[0]=& _tmp1433,((_tmp1434[1]=& _tmp1432,Cyc_Tcutil_terr(loc,((_tmp1435="bitfield %s must have integral type but has type %s",_tag_dyneither(_tmp1435,sizeof(char),52))),_tag_dyneither(_tmp1434,sizeof(void*),2)))))))))))));}
# 3412
goto _LL54F;_LL54F:;};}}
# 3417
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
# 3418
for(0;atts != 0;atts=atts->tl){
# 3419
void*_tmpAF4=(void*)atts->hd;_LL55B: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmpAF5=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmpAF4;if(_tmpAF5->tag != 7)goto _LL55D;}_LL55C:
# 3420
 continue;_LL55D: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpAF6=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmpAF4;if(_tmpAF6->tag != 6)goto _LL55F;}_LL55E:
# 3421
 continue;_LL55F:;_LL560: {
# 3422
const char*_tmp143A;void*_tmp1439[2];struct Cyc_String_pa_PrintArg_struct _tmp1438;struct Cyc_String_pa_PrintArg_struct _tmp1437;(_tmp1437.tag=0,((_tmp1437.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1438.tag=0,((_tmp1438.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3423
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp1439[0]=& _tmp1438,((_tmp1439[1]=& _tmp1437,Cyc_Tcutil_terr(loc,((_tmp143A="bad attribute %s on member %s",_tag_dyneither(_tmp143A,sizeof(char),30))),_tag_dyneither(_tmp1439,sizeof(void*),2)))))))))))));}_LL55A:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3441
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3445
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
# 3446
void*_tmpAFB=t;struct Cyc_Absyn_Typedefdecl*_tmpAFD;void*_tmpAFE;_LL562: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAFC=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpAFB;if(_tmpAFC->tag != 17)goto _LL564;else{_tmpAFD=_tmpAFC->f3;_tmpAFE=(void*)_tmpAFC->f4;}}_LL563:
# 3448
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmpAFD))->tq).real_const  || (_tmpAFD->tq).print_const){
# 3449
if(declared_const){const char*_tmp143D;void*_tmp143C;(_tmp143C=0,Cyc_Tcutil_warn(loc,((_tmp143D="extra const",_tag_dyneither(_tmp143D,sizeof(char),12))),_tag_dyneither(_tmp143C,sizeof(void*),0)));}
# 3450
return 1;}
# 3453
if((unsigned int)_tmpAFE)
# 3454
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,(void*)_tmpAFE);else{
# 3455
return declared_const;}_LL564:;_LL565:
# 3456
 return declared_const;_LL561:;}
# 3460
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
# 3461
if(td != 0){
# 3462
if(td->defn != 0){
# 3463
void*_tmpB01=Cyc_Tcutil_compress((void*)_check_null(td->defn));struct Cyc_Absyn_PtrInfo _tmpB03;struct Cyc_Absyn_PtrAtts _tmpB04;void*_tmpB05;_LL567: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpB02=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB01;if(_tmpB02->tag != 5)goto _LL569;else{_tmpB03=_tmpB02->f1;_tmpB04=_tmpB03.ptr_atts;_tmpB05=_tmpB04.rgn;}}_LL568:
# 3465
{void*_tmpB06=Cyc_Tcutil_compress(_tmpB05);struct Cyc_Absyn_Tvar*_tmpB08;_LL56C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpB07=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpB06;if(_tmpB07->tag != 2)goto _LL56E;else{_tmpB08=_tmpB07->f1;}}_LL56D:
# 3467
 return Cyc_Absyn_tvar_cmp(tvar,_tmpB08)== 0;_LL56E:;_LL56F:
# 3468
 goto _LL56B;_LL56B:;}
# 3470
goto _LL566;_LL569:;_LL56A:
# 3471
 goto _LL566;_LL566:;}}else{
# 3476
return 1;}
# 3477
return 0;}
# 3480
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3483
void*_tmpB09=Cyc_Absyn_compress_kb(tvar->kind);struct Cyc_Absyn_Kind*_tmpB0B;struct Cyc_Absyn_Kind _tmpB0C;enum Cyc_Absyn_KindQual _tmpB0D;enum Cyc_Absyn_AliasQual _tmpB0E;struct Cyc_Absyn_Kind*_tmpB10;struct Cyc_Absyn_Kind _tmpB11;enum Cyc_Absyn_KindQual _tmpB12;enum Cyc_Absyn_AliasQual _tmpB13;_LL571: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB0A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB09;if(_tmpB0A->tag != 2)goto _LL573;else{_tmpB0B=_tmpB0A->f2;_tmpB0C=*_tmpB0B;_tmpB0D=_tmpB0C.kind;if(_tmpB0D != Cyc_Absyn_RgnKind)goto _LL573;_tmpB0E=_tmpB0C.aliasqual;if(_tmpB0E != Cyc_Absyn_Top)goto _LL573;}}_LL572:
# 3484
 goto _LL574;_LL573: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB0F=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB09;if(_tmpB0F->tag != 0)goto _LL575;else{_tmpB10=_tmpB0F->f1;_tmpB11=*_tmpB10;_tmpB12=_tmpB11.kind;if(_tmpB12 != Cyc_Absyn_RgnKind)goto _LL575;_tmpB13=_tmpB11.aliasqual;if(_tmpB13 != Cyc_Absyn_Top)goto _LL575;}}_LL574:
# 3492
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3495
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
# 3496
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
# 3497
return& Cyc_Tcutil_rk;else{
# 3498
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
# 3499
return& Cyc_Tcutil_urk;}}
# 3501
return& Cyc_Tcutil_trk;_LL575:;_LL576:
# 3502
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);_LL570:;}
# 3536 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3539
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect){
# 3546
{void*_tmpB14=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpB17;struct Cyc_Core_Opt**_tmpB18;void*_tmpB19;void**_tmpB1A;struct Cyc_Absyn_Tvar*_tmpB1C;struct Cyc_Absyn_TypeDecl*_tmpB1E;struct Cyc_Absyn_TypeDecl _tmpB1F;void*_tmpB20;void**_tmpB21;void***_tmpB22;struct Cyc_List_List*_tmpB24;struct _tuple2*_tmpB26;struct Cyc_Absyn_Enumdecl*_tmpB27;struct Cyc_Absyn_Enumdecl**_tmpB28;struct Cyc_Absyn_DatatypeInfo _tmpB2A;union Cyc_Absyn_DatatypeInfoU _tmpB2B;union Cyc_Absyn_DatatypeInfoU*_tmpB2C;struct Cyc_List_List*_tmpB2D;struct Cyc_List_List**_tmpB2E;struct Cyc_Absyn_DatatypeFieldInfo _tmpB30;union Cyc_Absyn_DatatypeFieldInfoU _tmpB31;union Cyc_Absyn_DatatypeFieldInfoU*_tmpB32;struct Cyc_List_List*_tmpB33;struct Cyc_Absyn_PtrInfo _tmpB35;void*_tmpB36;struct Cyc_Absyn_Tqual _tmpB37;struct Cyc_Absyn_Tqual*_tmpB38;struct Cyc_Absyn_PtrAtts _tmpB39;void*_tmpB3A;union Cyc_Absyn_Constraint*_tmpB3B;union Cyc_Absyn_Constraint*_tmpB3C;union Cyc_Absyn_Constraint*_tmpB3D;void*_tmpB3F;struct Cyc_Absyn_Exp*_tmpB41;struct Cyc_Absyn_ArrayInfo _tmpB45;void*_tmpB46;struct Cyc_Absyn_Tqual _tmpB47;struct Cyc_Absyn_Tqual*_tmpB48;struct Cyc_Absyn_Exp*_tmpB49;struct Cyc_Absyn_Exp**_tmpB4A;union Cyc_Absyn_Constraint*_tmpB4B;unsigned int _tmpB4C;struct Cyc_Absyn_FnInfo _tmpB4E;struct Cyc_List_List*_tmpB4F;struct Cyc_List_List**_tmpB50;void*_tmpB51;void**_tmpB52;struct Cyc_Absyn_Tqual _tmpB53;struct Cyc_Absyn_Tqual*_tmpB54;void*_tmpB55;struct Cyc_List_List*_tmpB56;int _tmpB57;struct Cyc_Absyn_VarargInfo*_tmpB58;struct Cyc_List_List*_tmpB59;struct Cyc_List_List*_tmpB5A;struct Cyc_List_List*_tmpB5C;enum Cyc_Absyn_AggrKind _tmpB5E;struct Cyc_List_List*_tmpB5F;struct Cyc_Absyn_AggrInfo _tmpB61;union Cyc_Absyn_AggrInfoU _tmpB62;union Cyc_Absyn_AggrInfoU*_tmpB63;struct Cyc_List_List*_tmpB64;struct Cyc_List_List**_tmpB65;struct _tuple2*_tmpB67;struct Cyc_List_List*_tmpB68;struct Cyc_List_List**_tmpB69;struct Cyc_Absyn_Typedefdecl*_tmpB6A;struct Cyc_Absyn_Typedefdecl**_tmpB6B;void*_tmpB6C;void**_tmpB6D;void*_tmpB72;void*_tmpB74;void*_tmpB75;void*_tmpB77;void*_tmpB79;struct Cyc_List_List*_tmpB7B;_LL578: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpB15=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpB14;if(_tmpB15->tag != 0)goto _LL57A;}_LL579:
# 3547
 goto _LL577;_LL57A: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpB16=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpB14;if(_tmpB16->tag != 1)goto _LL57C;else{_tmpB17=_tmpB16->f1;_tmpB18=(struct Cyc_Core_Opt**)& _tmpB16->f1;_tmpB19=(void**)& _tmpB16->f2;_tmpB1A=(void**)((void**)& _tmpB16->f2);}}_LL57B:
# 3550
 if(*_tmpB18 == 0  || 
# 3551
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmpB18))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmpB18))->v,expected_kind))
# 3552
*_tmpB18=Cyc_Tcutil_kind_to_opt(expected_kind);
# 3553
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3555
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
# 3556
*_tmpB1A=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3558
*_tmpB1A=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
# 3559
if(cvtenv.generalize_evars){
# 3560
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1440;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp143F;struct Cyc_Absyn_Tvar*_tmpB7C=Cyc_Tcutil_new_tvar((void*)((_tmp143F=_cycalloc(sizeof(*_tmp143F)),((_tmp143F[0]=((_tmp1440.tag=2,((_tmp1440.f1=0,((_tmp1440.f2=expected_kind,_tmp1440)))))),_tmp143F)))));
# 3561
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1443;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1442;*_tmpB1A=(void*)((_tmp1442=_cycalloc(sizeof(*_tmp1442)),((_tmp1442[0]=((_tmp1443.tag=2,((_tmp1443.f1=_tmpB7C,_tmp1443)))),_tmp1442))));}
# 3562
_tmpB1C=_tmpB7C;goto _LL57D;}else{
# 3564
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3566
goto _LL577;_LL57C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpB1B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpB14;if(_tmpB1B->tag != 2)goto _LL57E;else{_tmpB1C=_tmpB1B->f1;}}_LL57D:
# 3568
{void*_tmpB81=Cyc_Absyn_compress_kb(_tmpB1C->kind);struct Cyc_Core_Opt*_tmpB83;struct Cyc_Core_Opt**_tmpB84;_LL5AF: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB82=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB81;if(_tmpB82->tag != 1)goto _LL5B1;else{_tmpB83=_tmpB82->f1;_tmpB84=(struct Cyc_Core_Opt**)& _tmpB82->f1;}}_LL5B0:
# 3570
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1449;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1448;struct Cyc_Core_Opt*_tmp1447;*_tmpB84=((_tmp1447=_cycalloc(sizeof(*_tmp1447)),((_tmp1447->v=(void*)((_tmp1449=_cycalloc(sizeof(*_tmp1449)),((_tmp1449[0]=((_tmp1448.tag=2,((_tmp1448.f1=0,((_tmp1448.f2=expected_kind,_tmp1448)))))),_tmp1449)))),_tmp1447))));}goto _LL5AE;_LL5B1:;_LL5B2:
# 3571
 goto _LL5AE;_LL5AE:;}
# 3575
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpB1C);
# 3578
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpB1C,put_in_effect);
# 3580
{void*_tmpB88=Cyc_Absyn_compress_kb(_tmpB1C->kind);struct Cyc_Core_Opt*_tmpB8A;struct Cyc_Core_Opt**_tmpB8B;struct Cyc_Absyn_Kind*_tmpB8C;_LL5B4: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB89=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB88;if(_tmpB89->tag != 2)goto _LL5B6;else{_tmpB8A=_tmpB89->f1;_tmpB8B=(struct Cyc_Core_Opt**)& _tmpB89->f1;_tmpB8C=_tmpB89->f2;}}_LL5B5:
# 3582
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmpB8C)){
# 3583
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp144F;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp144E;struct Cyc_Core_Opt*_tmp144D;*_tmpB8B=((_tmp144D=_cycalloc(sizeof(*_tmp144D)),((_tmp144D->v=(void*)((_tmp144F=_cycalloc(sizeof(*_tmp144F)),((_tmp144F[0]=((_tmp144E.tag=2,((_tmp144E.f1=0,((_tmp144E.f2=expected_kind,_tmp144E)))))),_tmp144F)))),_tmp144D))));}
# 3584
goto _LL5B3;_LL5B6:;_LL5B7:
# 3585
 goto _LL5B3;_LL5B3:;}
# 3587
goto _LL577;_LL57E: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB1D=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmpB14;if(_tmpB1D->tag != 26)goto _LL580;else{_tmpB1E=_tmpB1D->f1;_tmpB1F=*_tmpB1E;_tmpB20=_tmpB1F.r;_tmpB21=_tmpB1D->f2;_tmpB22=(void***)& _tmpB1D->f2;}}_LL57F: {
# 3593
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
# 3594
{void*_tmpB90=_tmpB20;struct Cyc_Absyn_Aggrdecl*_tmpB92;struct Cyc_Absyn_Enumdecl*_tmpB94;struct Cyc_Absyn_Datatypedecl*_tmpB96;_LL5B9: {struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpB91=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmpB90;if(_tmpB91->tag != 0)goto _LL5BB;else{_tmpB92=_tmpB91->f1;}}_LL5BA:
# 3596
 Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmpB92);goto _LL5B8;_LL5BB: {struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB93=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmpB90;if(_tmpB93->tag != 1)goto _LL5BD;else{_tmpB94=_tmpB93->f1;}}_LL5BC:
# 3598
 Cyc_Tc_tcEnumdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmpB94);goto _LL5B8;_LL5BD: {struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpB95=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmpB90;if(_tmpB95->tag != 2)goto _LL5B8;else{_tmpB96=_tmpB95->f1;}}_LL5BE:
# 3600
 Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmpB96);goto _LL5B8;_LL5B8:;}
# 3602
{void**_tmp1450;*_tmpB22=((_tmp1450=_cycalloc(sizeof(*_tmp1450)),((_tmp1450[0]=new_t,_tmp1450))));}
# 3603
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect);}_LL580: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpB23=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpB14;if(_tmpB23->tag != 14)goto _LL582;else{_tmpB24=_tmpB23->f1;}}_LL581: {
# 3607
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
# 3608
struct _RegionHandle _tmpB98=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpB98;_push_region(uprev_rgn);{
# 3609
struct Cyc_List_List*prev_fields=0;
# 3610
unsigned int tag_count=0;
# 3611
for(0;_tmpB24 != 0;_tmpB24=_tmpB24->tl){
# 3612
struct Cyc_Absyn_Enumfield*_tmpB99=(struct Cyc_Absyn_Enumfield*)_tmpB24->hd;
# 3613
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmpB99->name).f2)){
# 3614
const char*_tmp1454;void*_tmp1453[1];struct Cyc_String_pa_PrintArg_struct _tmp1452;(_tmp1452.tag=0,((_tmp1452.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpB99->name).f2),((_tmp1453[0]=& _tmp1452,Cyc_Tcutil_terr(_tmpB99->loc,((_tmp1454="duplicate enum field name %s",_tag_dyneither(_tmp1454,sizeof(char),29))),_tag_dyneither(_tmp1453,sizeof(void*),1)))))));}else{
# 3616
struct Cyc_List_List*_tmp1455;prev_fields=((_tmp1455=_region_malloc(uprev_rgn,sizeof(*_tmp1455)),((_tmp1455->hd=(*_tmpB99->name).f2,((_tmp1455->tl=prev_fields,_tmp1455))))));}
# 3618
if(_tmpB99->tag == 0)
# 3619
_tmpB99->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmpB99->loc);else{
# 3620
if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_check_null(_tmpB99->tag))){
# 3621
const char*_tmp1459;void*_tmp1458[1];struct Cyc_String_pa_PrintArg_struct _tmp1457;(_tmp1457.tag=0,((_tmp1457.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpB99->name).f2),((_tmp1458[0]=& _tmp1457,Cyc_Tcutil_terr(loc,((_tmp1459="enum field %s: expression is not constant",_tag_dyneither(_tmp1459,sizeof(char),42))),_tag_dyneither(_tmp1458,sizeof(void*),1)))))));}}{
# 3623
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmpB99->tag))).f1;
# 3624
tag_count=t1 + 1;
# 3625
{union Cyc_Absyn_Nmspace _tmpBA1=(*_tmpB99->name).f1;struct Cyc_List_List*_tmpBA2;_LL5C0: if((_tmpBA1.Rel_n).tag != 1)goto _LL5C2;_tmpBA2=(struct Cyc_List_List*)(_tmpBA1.Rel_n).val;_LL5C1:
# 3627
(*_tmpB99->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LL5BF;_LL5C2:;_LL5C3:
# 3628
 goto _LL5BF;_LL5BF:;}{
# 3630
struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp145F;struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct _tmp145E;struct _tuple29*_tmp145D;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple29*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmpB99->name).f2,(struct _tuple29*)(
# 3631
(_tmp145D=_cycalloc(sizeof(*_tmp145D)),((_tmp145D->f1=(void*)((_tmp145F=_cycalloc(sizeof(*_tmp145F)),((_tmp145F[0]=((_tmp145E.tag=4,((_tmp145E.f1=(void*)t,((_tmp145E.f2=_tmpB99,_tmp145E)))))),_tmp145F)))),((_tmp145D->f2=1,_tmp145D)))))));};};}}
# 3634
_npop_handler(0);goto _LL577;
# 3608
;_pop_region(uprev_rgn);}_LL582: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpB25=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpB14;if(_tmpB25->tag != 13)goto _LL584;else{_tmpB26=_tmpB25->f1;_tmpB27=_tmpB25->f2;_tmpB28=(struct Cyc_Absyn_Enumdecl**)& _tmpB25->f2;}}_LL583:
# 3636
 if(*_tmpB28 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmpB28))->fields == 0){
# 3637
struct _handler_cons _tmpBA6;_push_handler(& _tmpBA6);{int _tmpBA8=0;if(setjmp(_tmpBA6.handler))_tmpBA8=1;if(!_tmpBA8){
# 3638
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmpB26);
# 3639
*_tmpB28=(struct Cyc_Absyn_Enumdecl*)*ed;}
# 3638
;_pop_handler();}else{void*_tmpBA7=(void*)_exn_thrown;void*_tmpBAA=_tmpBA7;_LL5C5: {struct Cyc_Dict_Absent_exn_struct*_tmpBAB=(struct Cyc_Dict_Absent_exn_struct*)_tmpBAA;if(_tmpBAB->tag != Cyc_Dict_Absent)goto _LL5C7;}_LL5C6: {
# 3642
struct Cyc_Tcenv_Genv*_tmpBAC=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
# 3643
struct Cyc_Absyn_Enumdecl*_tmp1460;struct Cyc_Absyn_Enumdecl*_tmpBAD=(_tmp1460=_cycalloc(sizeof(*_tmp1460)),((_tmp1460->sc=Cyc_Absyn_Extern,((_tmp1460->name=_tmpB26,((_tmp1460->fields=0,_tmp1460)))))));
# 3644
Cyc_Tc_tcEnumdecl(te,_tmpBAC,loc,_tmpBAD);{
# 3645
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmpB26);
# 3646
*_tmpB28=(struct Cyc_Absyn_Enumdecl*)*ed;
# 3647
goto _LL5C4;};}_LL5C7:;_LL5C8:(void)_throw(_tmpBAA);_LL5C4:;}};}{
# 3651
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmpB28);
# 3653
*_tmpB26=(ed->name)[0];
# 3654
goto _LL577;};_LL584: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB29=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpB14;if(_tmpB29->tag != 3)goto _LL586;else{_tmpB2A=_tmpB29->f1;_tmpB2B=_tmpB2A.datatype_info;_tmpB2C=(union Cyc_Absyn_DatatypeInfoU*)&(_tmpB29->f1).datatype_info;_tmpB2D=_tmpB2A.targs;_tmpB2E=(struct Cyc_List_List**)&(_tmpB29->f1).targs;}}_LL585: {
# 3656
struct Cyc_List_List*_tmpBAF=*_tmpB2E;
# 3657
{union Cyc_Absyn_DatatypeInfoU _tmpBB0=*_tmpB2C;struct Cyc_Absyn_UnknownDatatypeInfo _tmpBB1;struct _tuple2*_tmpBB2;int _tmpBB3;struct Cyc_Absyn_Datatypedecl**_tmpBB4;struct Cyc_Absyn_Datatypedecl*_tmpBB5;_LL5CA: if((_tmpBB0.UnknownDatatype).tag != 1)goto _LL5CC;_tmpBB1=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpBB0.UnknownDatatype).val;_tmpBB2=_tmpBB1.name;_tmpBB3=_tmpBB1.is_extensible;_LL5CB: {
# 3659
struct Cyc_Absyn_Datatypedecl**tudp;
# 3660
{struct _handler_cons _tmpBB6;_push_handler(& _tmpBB6);{int _tmpBB8=0;if(setjmp(_tmpBB6.handler))_tmpBB8=1;if(!_tmpBB8){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpBB2);;_pop_handler();}else{void*_tmpBB7=(void*)_exn_thrown;void*_tmpBBA=_tmpBB7;_LL5CF: {struct Cyc_Dict_Absent_exn_struct*_tmpBBB=(struct Cyc_Dict_Absent_exn_struct*)_tmpBBA;if(_tmpBBB->tag != Cyc_Dict_Absent)goto _LL5D1;}_LL5D0: {
# 3663
struct Cyc_Tcenv_Genv*_tmpBBC=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
# 3664
struct Cyc_Absyn_Datatypedecl*_tmp1461;struct Cyc_Absyn_Datatypedecl*_tmpBBD=(_tmp1461=_cycalloc(sizeof(*_tmp1461)),((_tmp1461->sc=Cyc_Absyn_Extern,((_tmp1461->name=_tmpBB2,((_tmp1461->tvs=0,((_tmp1461->fields=0,((_tmp1461->is_extensible=_tmpBB3,_tmp1461)))))))))));
# 3665
Cyc_Tc_tcDatatypedecl(te,_tmpBBC,loc,_tmpBBD);
# 3666
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpBB2);
# 3668
if(_tmpBAF != 0){
# 3669
{const char*_tmp1465;void*_tmp1464[1];struct Cyc_String_pa_PrintArg_struct _tmp1463;(_tmp1463.tag=0,((_tmp1463.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3670
Cyc_Absynpp_qvar2string(_tmpBB2)),((_tmp1464[0]=& _tmp1463,Cyc_Tcutil_terr(loc,((_tmp1465="declare parameterized datatype %s before using",_tag_dyneither(_tmp1465,sizeof(char),47))),_tag_dyneither(_tmp1464,sizeof(void*),1)))))));}
# 3671
return cvtenv;}
# 3673
goto _LL5CE;}_LL5D1:;_LL5D2:(void)_throw(_tmpBBA);_LL5CE:;}};}
# 3679
if(_tmpBB3  && !(*tudp)->is_extensible){
# 3680
const char*_tmp1469;void*_tmp1468[1];struct Cyc_String_pa_PrintArg_struct _tmp1467;(_tmp1467.tag=0,((_tmp1467.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3681
Cyc_Absynpp_qvar2string(_tmpBB2)),((_tmp1468[0]=& _tmp1467,Cyc_Tcutil_terr(loc,((_tmp1469="datatype %s was not declared @extensible",_tag_dyneither(_tmp1469,sizeof(char),41))),_tag_dyneither(_tmp1468,sizeof(void*),1)))))));}
# 3682
*_tmpB2C=Cyc_Absyn_KnownDatatype(tudp);
# 3683
_tmpBB5=*tudp;goto _LL5CD;}_LL5CC: if((_tmpBB0.KnownDatatype).tag != 2)goto _LL5C9;_tmpBB4=(struct Cyc_Absyn_Datatypedecl**)(_tmpBB0.KnownDatatype).val;_tmpBB5=*_tmpBB4;_LL5CD: {
# 3686
struct Cyc_List_List*tvs=_tmpBB5->tvs;
# 3687
for(0;_tmpBAF != 0  && tvs != 0;(_tmpBAF=_tmpBAF->tl,tvs=tvs->tl)){
# 3688
void*t=(void*)_tmpBAF->hd;
# 3689
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3692
{struct _tuple0 _tmp146A;struct _tuple0 _tmpBC6=(_tmp146A.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp146A.f2=t,_tmp146A)));void*_tmpBC7;void*_tmpBC9;struct Cyc_Absyn_Tvar*_tmpBCB;_LL5D4: _tmpBC7=_tmpBC6.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpBC8=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpBC7;if(_tmpBC8->tag != 1)goto _LL5D6;};_tmpBC9=_tmpBC6.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpBCA=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpBC9;if(_tmpBCA->tag != 2)goto _LL5D6;else{_tmpBCB=_tmpBCA->f1;}};_LL5D5:
# 3694
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpBCB);
# 3695
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpBCB,1);
# 3696
continue;_LL5D6:;_LL5D7:
# 3697
 goto _LL5D3;_LL5D3:;}{
# 3699
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
# 3700
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
# 3701
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3703
if(_tmpBAF != 0){
# 3704
const char*_tmp146E;void*_tmp146D[1];struct Cyc_String_pa_PrintArg_struct _tmp146C;(_tmp146C.tag=0,((_tmp146C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3705
Cyc_Absynpp_qvar2string(_tmpBB5->name)),((_tmp146D[0]=& _tmp146C,Cyc_Tcutil_terr(loc,((_tmp146E="too many type arguments for datatype %s",_tag_dyneither(_tmp146E,sizeof(char),40))),_tag_dyneither(_tmp146D,sizeof(void*),1)))))));}
# 3706
if(tvs != 0){
# 3708
struct Cyc_List_List*hidden_ts=0;
# 3709
for(0;tvs != 0;tvs=tvs->tl){
# 3710
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
# 3711
void*e=Cyc_Absyn_new_evar(0,0);
# 3712
{struct Cyc_List_List*_tmp146F;hidden_ts=((_tmp146F=_cycalloc(sizeof(*_tmp146F)),((_tmp146F->hd=e,((_tmp146F->tl=hidden_ts,_tmp146F))))));}
# 3713
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1);}
# 3715
*_tmpB2E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpB2E,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 3717
goto _LL5C9;}_LL5C9:;}
# 3719
goto _LL577;}_LL586: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpB2F=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpB14;if(_tmpB2F->tag != 4)goto _LL588;else{_tmpB30=_tmpB2F->f1;_tmpB31=_tmpB30.field_info;_tmpB32=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmpB2F->f1).field_info;_tmpB33=_tmpB30.targs;}}_LL587:
# 3722
{union Cyc_Absyn_DatatypeFieldInfoU _tmpBD0=*_tmpB32;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpBD1;struct _tuple2*_tmpBD2;struct _tuple2*_tmpBD3;int _tmpBD4;struct _tuple3 _tmpBD5;struct Cyc_Absyn_Datatypedecl*_tmpBD6;struct Cyc_Absyn_Datatypefield*_tmpBD7;_LL5D9: if((_tmpBD0.UnknownDatatypefield).tag != 1)goto _LL5DB;_tmpBD1=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpBD0.UnknownDatatypefield).val;_tmpBD2=_tmpBD1.datatype_name;_tmpBD3=_tmpBD1.field_name;_tmpBD4=_tmpBD1.is_extensible;_LL5DA: {
# 3724
struct Cyc_Absyn_Datatypedecl*tud;
# 3725
struct Cyc_Absyn_Datatypefield*tuf;
# 3726
{struct _handler_cons _tmpBD8;_push_handler(& _tmpBD8);{int _tmpBDA=0;if(setjmp(_tmpBD8.handler))_tmpBDA=1;if(!_tmpBDA){*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpBD2);;_pop_handler();}else{void*_tmpBD9=(void*)_exn_thrown;void*_tmpBDC=_tmpBD9;_LL5DE: {struct Cyc_Dict_Absent_exn_struct*_tmpBDD=(struct Cyc_Dict_Absent_exn_struct*)_tmpBDC;if(_tmpBDD->tag != Cyc_Dict_Absent)goto _LL5E0;}_LL5DF:
# 3728
{const char*_tmp1473;void*_tmp1472[1];struct Cyc_String_pa_PrintArg_struct _tmp1471;(_tmp1471.tag=0,((_tmp1471.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD2)),((_tmp1472[0]=& _tmp1471,Cyc_Tcutil_terr(loc,((_tmp1473="unbound datatype %s",_tag_dyneither(_tmp1473,sizeof(char),20))),_tag_dyneither(_tmp1472,sizeof(void*),1)))))));}
# 3729
return cvtenv;_LL5E0:;_LL5E1:(void)_throw(_tmpBDC);_LL5DD:;}};}
# 3731
{struct _handler_cons _tmpBE1;_push_handler(& _tmpBE1);{int _tmpBE3=0;if(setjmp(_tmpBE1.handler))_tmpBE3=1;if(!_tmpBE3){
# 3732
{struct _RegionHandle _tmpBE4=_new_region("r");struct _RegionHandle*r=& _tmpBE4;_push_region(r);
# 3733
{void*_tmpBE5=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmpBD3,0);struct Cyc_Absyn_Datatypedecl*_tmpBE7;struct Cyc_Absyn_Datatypefield*_tmpBE8;_LL5E3: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpBE6=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpBE5;if(_tmpBE6->tag != 2)goto _LL5E5;else{_tmpBE7=_tmpBE6->f1;_tmpBE8=_tmpBE6->f2;}}_LL5E4:
# 3735
 tuf=_tmpBE8;
# 3736
tud=_tmpBE7;
# 3737
if(_tmpBD4  && !tud->is_extensible){
# 3738
const char*_tmp1477;void*_tmp1476[1];struct Cyc_String_pa_PrintArg_struct _tmp1475;(_tmp1475.tag=0,((_tmp1475.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3739
Cyc_Absynpp_qvar2string(_tmpBD2)),((_tmp1476[0]=& _tmp1475,Cyc_Tcutil_terr(loc,((_tmp1477="datatype %s was not declared @extensible",_tag_dyneither(_tmp1477,sizeof(char),41))),_tag_dyneither(_tmp1476,sizeof(void*),1)))))));}
# 3740
goto _LL5E2;_LL5E5:;_LL5E6:
# 3741
{const char*_tmp147C;void*_tmp147B[2];struct Cyc_String_pa_PrintArg_struct _tmp147A;struct Cyc_String_pa_PrintArg_struct _tmp1479;(_tmp1479.tag=0,((_tmp1479.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3742
Cyc_Absynpp_qvar2string(_tmpBD2)),((_tmp147A.tag=0,((_tmp147A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD3)),((_tmp147B[0]=& _tmp147A,((_tmp147B[1]=& _tmp1479,Cyc_Tcutil_terr(loc,((_tmp147C="unbound field %s in type datatype %s",_tag_dyneither(_tmp147C,sizeof(char),37))),_tag_dyneither(_tmp147B,sizeof(void*),2)))))))))))));}{
# 3743
struct Cyc_Tcutil_CVTEnv _tmpBF0=cvtenv;_npop_handler(1);return _tmpBF0;};_LL5E2:;}
# 3733
;_pop_region(r);}
# 3732
;_pop_handler();}else{void*_tmpBE2=(void*)_exn_thrown;void*_tmpBF2=_tmpBE2;_LL5E8: {struct Cyc_Dict_Absent_exn_struct*_tmpBF3=(struct Cyc_Dict_Absent_exn_struct*)_tmpBF2;if(_tmpBF3->tag != Cyc_Dict_Absent)goto _LL5EA;}_LL5E9:
# 3748
{const char*_tmp1481;void*_tmp1480[2];struct Cyc_String_pa_PrintArg_struct _tmp147F;struct Cyc_String_pa_PrintArg_struct _tmp147E;(_tmp147E.tag=0,((_tmp147E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3749
Cyc_Absynpp_qvar2string(_tmpBD2)),((_tmp147F.tag=0,((_tmp147F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD3)),((_tmp1480[0]=& _tmp147F,((_tmp1480[1]=& _tmp147E,Cyc_Tcutil_terr(loc,((_tmp1481="unbound field %s in type datatype %s",_tag_dyneither(_tmp1481,sizeof(char),37))),_tag_dyneither(_tmp1480,sizeof(void*),2)))))))))))));}
# 3750
return cvtenv;_LL5EA:;_LL5EB:(void)_throw(_tmpBF2);_LL5E7:;}};}
# 3752
*_tmpB32=Cyc_Absyn_KnownDatatypefield(tud,tuf);
# 3753
_tmpBD6=tud;_tmpBD7=tuf;goto _LL5DC;}_LL5DB: if((_tmpBD0.KnownDatatypefield).tag != 2)goto _LL5D8;_tmpBD5=(struct _tuple3)(_tmpBD0.KnownDatatypefield).val;_tmpBD6=_tmpBD5.f1;_tmpBD7=_tmpBD5.f2;_LL5DC: {
# 3756
struct Cyc_List_List*tvs=_tmpBD6->tvs;
# 3757
for(0;_tmpB33 != 0  && tvs != 0;(_tmpB33=_tmpB33->tl,tvs=tvs->tl)){
# 3758
void*t=(void*)_tmpB33->hd;
# 3759
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3762
{struct _tuple0 _tmp1482;struct _tuple0 _tmpBF9=(_tmp1482.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp1482.f2=t,_tmp1482)));void*_tmpBFA;void*_tmpBFC;struct Cyc_Absyn_Tvar*_tmpBFE;_LL5ED: _tmpBFA=_tmpBF9.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpBFB=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpBFA;if(_tmpBFB->tag != 1)goto _LL5EF;};_tmpBFC=_tmpBF9.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpBFD=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpBFC;if(_tmpBFD->tag != 2)goto _LL5EF;else{_tmpBFE=_tmpBFD->f1;}};_LL5EE:
# 3764
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpBFE);
# 3765
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpBFE,1);
# 3766
continue;_LL5EF:;_LL5F0:
# 3767
 goto _LL5EC;_LL5EC:;}{
# 3769
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
# 3770
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
# 3771
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3773
if(_tmpB33 != 0){
# 3774
const char*_tmp1487;void*_tmp1486[2];struct Cyc_String_pa_PrintArg_struct _tmp1485;struct Cyc_String_pa_PrintArg_struct _tmp1484;(_tmp1484.tag=0,((_tmp1484.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3775
Cyc_Absynpp_qvar2string(_tmpBD7->name)),((_tmp1485.tag=0,((_tmp1485.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD6->name)),((_tmp1486[0]=& _tmp1485,((_tmp1486[1]=& _tmp1484,Cyc_Tcutil_terr(loc,((_tmp1487="too many type arguments for datatype %s.%s",_tag_dyneither(_tmp1487,sizeof(char),43))),_tag_dyneither(_tmp1486,sizeof(void*),2)))))))))))));}
# 3776
if(tvs != 0){
# 3777
const char*_tmp148C;void*_tmp148B[2];struct Cyc_String_pa_PrintArg_struct _tmp148A;struct Cyc_String_pa_PrintArg_struct _tmp1489;(_tmp1489.tag=0,((_tmp1489.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3778
Cyc_Absynpp_qvar2string(_tmpBD7->name)),((_tmp148A.tag=0,((_tmp148A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD6->name)),((_tmp148B[0]=& _tmp148A,((_tmp148B[1]=& _tmp1489,Cyc_Tcutil_terr(loc,((_tmp148C="too few type arguments for datatype %s.%s",_tag_dyneither(_tmp148C,sizeof(char),42))),_tag_dyneither(_tmp148B,sizeof(void*),2)))))))))))));}
# 3779
goto _LL5D8;}_LL5D8:;}
# 3781
goto _LL577;_LL588: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpB34=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB14;if(_tmpB34->tag != 5)goto _LL58A;else{_tmpB35=_tmpB34->f1;_tmpB36=_tmpB35.elt_typ;_tmpB37=_tmpB35.elt_tq;_tmpB38=(struct Cyc_Absyn_Tqual*)&(_tmpB34->f1).elt_tq;_tmpB39=_tmpB35.ptr_atts;_tmpB3A=_tmpB39.rgn;_tmpB3B=_tmpB39.nullable;_tmpB3C=_tmpB39.bounds;_tmpB3D=_tmpB39.zero_term;}}_LL589: {
# 3784
int is_zero_terminated;
# 3786
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB36,1);
# 3787
_tmpB38->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpB38->print_const,_tmpB36);{
# 3788
struct Cyc_Absyn_Kind*k;
# 3789
switch(expected_kind->aliasqual){case Cyc_Absyn_Aliasable: _LL5F1:
# 3790
 k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique: _LL5F2:
# 3791
 k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL5F3:
# 3792
 k=& Cyc_Tcutil_trk;break;}
# 3794
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmpB3A,1);
# 3795
{union Cyc_Absyn_Constraint*_tmpC07=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmpB3D);union Cyc_Absyn_Constraint _tmpC08;int _tmpC09;union Cyc_Absyn_Constraint _tmpC0A;int _tmpC0B;_LL5F6: _tmpC08=*_tmpC07;if((_tmpC08.No_constr).tag != 3)goto _LL5F8;_tmpC09=(int)(_tmpC08.No_constr).val;_LL5F7:
# 3799
{void*_tmpC0C=Cyc_Tcutil_compress(_tmpB36);enum Cyc_Absyn_Size_of _tmpC0E;_LL5FD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpC0D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpC0C;if(_tmpC0D->tag != 6)goto _LL5FF;else{_tmpC0E=_tmpC0D->f2;if(_tmpC0E != Cyc_Absyn_Char_sz)goto _LL5FF;}}_LL5FE:
# 3801
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB3D,Cyc_Absyn_true_conref);
# 3802
is_zero_terminated=1;
# 3803
goto _LL5FC;_LL5FF:;_LL600:
# 3805
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB3D,Cyc_Absyn_false_conref);
# 3806
is_zero_terminated=0;
# 3807
goto _LL5FC;_LL5FC:;}
# 3809
goto _LL5F5;_LL5F8: _tmpC0A=*_tmpC07;if((_tmpC0A.Eq_constr).tag != 1)goto _LL5FA;_tmpC0B=(int)(_tmpC0A.Eq_constr).val;if(_tmpC0B != 1)goto _LL5FA;_LL5F9:
# 3812
 if(!Cyc_Tcutil_admits_zero(_tmpB36)){
# 3813
const char*_tmp1490;void*_tmp148F[1];struct Cyc_String_pa_PrintArg_struct _tmp148E;(_tmp148E.tag=0,((_tmp148E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3814
Cyc_Absynpp_typ2string(_tmpB36)),((_tmp148F[0]=& _tmp148E,Cyc_Tcutil_terr(loc,((_tmp1490="cannot have a pointer to zero-terminated %s elements",_tag_dyneither(_tmp1490,sizeof(char),53))),_tag_dyneither(_tmp148F,sizeof(void*),1)))))));}
# 3815
is_zero_terminated=1;
# 3816
goto _LL5F5;_LL5FA:;_LL5FB:
# 3818
 is_zero_terminated=0;
# 3819
goto _LL5F5;_LL5F5:;}
# 3822
{void*_tmpC12=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpB3C);struct Cyc_Absyn_Exp*_tmpC15;_LL602: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpC13=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpC12;if(_tmpC13->tag != 0)goto _LL604;}_LL603:
# 3823
 goto _LL601;_LL604: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpC14=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpC12;if(_tmpC14->tag != 1)goto _LL601;else{_tmpC15=_tmpC14->f1;}}_LL605: {
# 3825
struct _RegionHandle _tmpC16=_new_region("temp");struct _RegionHandle*temp=& _tmpC16;_push_region(temp);{
# 3826
struct Cyc_Tcenv_Tenv*_tmpC17=Cyc_Tcenv_allow_valueof(temp,te);
# 3827
Cyc_Tcexp_tcExp(_tmpC17,0,_tmpC15);}
# 3829
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC15,te,cvtenv);
# 3830
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpC15)){
# 3831
const char*_tmp1493;void*_tmp1492;(_tmp1492=0,Cyc_Tcutil_terr(loc,((_tmp1493="pointer bounds expression is not an unsigned int",_tag_dyneither(_tmp1493,sizeof(char),49))),_tag_dyneither(_tmp1492,sizeof(void*),0)));}{
# 3832
unsigned int _tmpC1B;int _tmpC1C;struct _tuple14 _tmpC1A=Cyc_Evexp_eval_const_uint_exp(_tmpC15);_tmpC1B=_tmpC1A.f1;_tmpC1C=_tmpC1A.f2;
# 3833
if(is_zero_terminated  && (!_tmpC1C  || _tmpC1B < 1)){
# 3834
const char*_tmp1496;void*_tmp1495;(_tmp1495=0,Cyc_Tcutil_terr(loc,((_tmp1496="zero-terminated pointer cannot point to empty sequence",_tag_dyneither(_tmp1496,sizeof(char),55))),_tag_dyneither(_tmp1495,sizeof(void*),0)));}
# 3835
_npop_handler(0);goto _LL601;};
# 3825
;_pop_region(temp);}_LL601:;}
# 3837
goto _LL577;};}_LL58A: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB3E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpB14;if(_tmpB3E->tag != 19)goto _LL58C;else{_tmpB3F=(void*)_tmpB3E->f1;}}_LL58B:
# 3839
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpB3F,1);goto _LL577;_LL58C: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpB40=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmpB14;if(_tmpB40->tag != 18)goto _LL58E;else{_tmpB41=_tmpB40->f1;}}_LL58D: {
# 3844
struct _RegionHandle _tmpC1F=_new_region("temp");struct _RegionHandle*temp=& _tmpC1F;_push_region(temp);{
# 3845
struct Cyc_Tcenv_Tenv*_tmpC20=Cyc_Tcenv_allow_valueof(temp,te);
# 3846
Cyc_Tcexp_tcExp(_tmpC20,0,_tmpB41);}
# 3848
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpB41)){
# 3849
const char*_tmp1499;void*_tmp1498;(_tmp1498=0,Cyc_Tcutil_terr(loc,((_tmp1499="valueof_t requires an int expression",_tag_dyneither(_tmp1499,sizeof(char),37))),_tag_dyneither(_tmp1498,sizeof(void*),0)));}
# 3850
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB41,te,cvtenv);
# 3851
_npop_handler(0);goto _LL577;
# 3844
;_pop_region(temp);}_LL58E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpB42=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpB14;if(_tmpB42->tag != 6)goto _LL590;}_LL58F:
# 3852
 goto _LL591;_LL590: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpB43=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpB14;if(_tmpB43->tag != 7)goto _LL592;}_LL591:
# 3853
 goto _LL577;_LL592: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpB44=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB14;if(_tmpB44->tag != 8)goto _LL594;else{_tmpB45=_tmpB44->f1;_tmpB46=_tmpB45.elt_type;_tmpB47=_tmpB45.tq;_tmpB48=(struct Cyc_Absyn_Tqual*)&(_tmpB44->f1).tq;_tmpB49=_tmpB45.num_elts;_tmpB4A=(struct Cyc_Absyn_Exp**)&(_tmpB44->f1).num_elts;_tmpB4B=_tmpB45.zero_term;_tmpB4C=_tmpB45.zt_loc;}}_LL593: {
# 3857
struct Cyc_Absyn_Exp*_tmpC23=*_tmpB4A;
# 3858
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpB46,1);
# 3859
_tmpB48->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpB48->print_const,_tmpB46);{
# 3860
int is_zero_terminated;
# 3861
{union Cyc_Absyn_Constraint*_tmpC24=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmpB4B);union Cyc_Absyn_Constraint _tmpC25;int _tmpC26;union Cyc_Absyn_Constraint _tmpC27;int _tmpC28;_LL607: _tmpC25=*_tmpC24;if((_tmpC25.No_constr).tag != 3)goto _LL609;_tmpC26=(int)(_tmpC25.No_constr).val;_LL608:
# 3864
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB4B,Cyc_Absyn_false_conref);
# 3865
is_zero_terminated=0;
# 3879 "tcutil.cyc"
goto _LL606;_LL609: _tmpC27=*_tmpC24;if((_tmpC27.Eq_constr).tag != 1)goto _LL60B;_tmpC28=(int)(_tmpC27.Eq_constr).val;if(_tmpC28 != 1)goto _LL60B;_LL60A:
# 3882
 if(!Cyc_Tcutil_admits_zero(_tmpB46)){
# 3883
const char*_tmp149D;void*_tmp149C[1];struct Cyc_String_pa_PrintArg_struct _tmp149B;(_tmp149B.tag=0,((_tmp149B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 3884
Cyc_Absynpp_typ2string(_tmpB46)),((_tmp149C[0]=& _tmp149B,Cyc_Tcutil_terr(loc,((_tmp149D="cannot have a zero-terminated array of %s elements",_tag_dyneither(_tmp149D,sizeof(char),51))),_tag_dyneither(_tmp149C,sizeof(void*),1)))))));}
# 3885
is_zero_terminated=1;
# 3886
goto _LL606;_LL60B:;_LL60C:
# 3888
 is_zero_terminated=0;
# 3889
goto _LL606;_LL606:;}
# 3893
if(_tmpC23 == 0){
# 3895
if(is_zero_terminated)
# 3897
*_tmpB4A=(_tmpC23=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{
# 3900
{const char*_tmp14A0;void*_tmp149F;(_tmp149F=0,Cyc_Tcutil_warn(loc,((_tmp14A0="array bound defaults to 1 here",_tag_dyneither(_tmp14A0,sizeof(char),31))),_tag_dyneither(_tmp149F,sizeof(void*),0)));}
# 3901
*_tmpB4A=(_tmpC23=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}{
# 3904
struct _RegionHandle _tmpC2E=_new_region("temp");struct _RegionHandle*temp=& _tmpC2E;_push_region(temp);{
# 3905
struct Cyc_Tcenv_Tenv*_tmpC2F=Cyc_Tcenv_allow_valueof(temp,te);
# 3906
Cyc_Tcexp_tcExp(_tmpC2F,0,(struct Cyc_Absyn_Exp*)_tmpC23);}
# 3908
if(!Cyc_Tcutil_coerce_uint_typ(te,(struct Cyc_Absyn_Exp*)_tmpC23)){
# 3909
const char*_tmp14A3;void*_tmp14A2;(_tmp14A2=0,Cyc_Tcutil_terr(loc,((_tmp14A3="array bounds expression is not an unsigned int",_tag_dyneither(_tmp14A3,sizeof(char),47))),_tag_dyneither(_tmp14A2,sizeof(void*),0)));}
# 3910
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmpC23,te,cvtenv);
# 3911
if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_tmpC23)){
# 3912
const char*_tmp14A6;void*_tmp14A5;(_tmp14A5=0,Cyc_Tcutil_terr(loc,((_tmp14A6="array bounds expression is not constant",_tag_dyneither(_tmp14A6,sizeof(char),40))),_tag_dyneither(_tmp14A5,sizeof(void*),0)));}{
# 3913
unsigned int _tmpC35;int _tmpC36;struct _tuple14 _tmpC34=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmpC23);_tmpC35=_tmpC34.f1;_tmpC36=_tmpC34.f2;
# 3915
if((is_zero_terminated  && _tmpC36) && _tmpC35 < 1){
# 3916
const char*_tmp14A9;void*_tmp14A8;(_tmp14A8=0,Cyc_Tcutil_warn(loc,((_tmp14A9="zero terminated array cannot have zero elements",_tag_dyneither(_tmp14A9,sizeof(char),48))),_tag_dyneither(_tmp14A8,sizeof(void*),0)));}
# 3918
if((_tmpC36  && _tmpC35 < 1) && Cyc_Cyclone_tovc_r){
# 3919
{const char*_tmp14AC;void*_tmp14AB;(_tmp14AB=0,Cyc_Tcutil_warn(loc,((_tmp14AC="arrays with 0 elements are not supported except with gcc -- changing to 1.",_tag_dyneither(_tmp14AC,sizeof(char),75))),_tag_dyneither(_tmp14AB,sizeof(void*),0)));}
# 3920
*_tmpB4A=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}
# 3922
_npop_handler(0);goto _LL577;};
# 3904
;_pop_region(temp);};};}_LL594: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB4D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB14;if(_tmpB4D->tag != 9)goto _LL596;else{_tmpB4E=_tmpB4D->f1;_tmpB4F=_tmpB4E.tvars;_tmpB50=(struct Cyc_List_List**)&(_tmpB4D->f1).tvars;_tmpB51=_tmpB4E.effect;_tmpB52=(void**)&(_tmpB4D->f1).effect;_tmpB53=_tmpB4E.ret_tqual;_tmpB54=(struct Cyc_Absyn_Tqual*)&(_tmpB4D->f1).ret_tqual;_tmpB55=_tmpB4E.ret_typ;_tmpB56=_tmpB4E.args;_tmpB57=_tmpB4E.c_varargs;_tmpB58=_tmpB4E.cyc_varargs;_tmpB59=_tmpB4E.rgn_po;_tmpB5A=_tmpB4E.attributes;}}_LL595: {
# 3929
int num_convs=0;
# 3930
int seen_cdecl=0;
# 3931
int seen_stdcall=0;
# 3932
int seen_fastcall=0;
# 3933
int seen_format=0;
# 3934
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
# 3935
int fmt_desc_arg=- 1;
# 3936
int fmt_arg_start=- 1;
# 3937
for(0;_tmpB5A != 0;_tmpB5A=_tmpB5A->tl){
# 3938
if(!Cyc_Absyn_fntype_att((void*)_tmpB5A->hd)){
# 3939
const char*_tmp14B0;void*_tmp14AF[1];struct Cyc_String_pa_PrintArg_struct _tmp14AE;(_tmp14AE.tag=0,((_tmp14AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmpB5A->hd)),((_tmp14AF[0]=& _tmp14AE,Cyc_Tcutil_terr(loc,((_tmp14B0="bad function type attribute %s",_tag_dyneither(_tmp14B0,sizeof(char),31))),_tag_dyneither(_tmp14AF,sizeof(void*),1)))))));}{
# 3940
void*_tmpC3E=(void*)_tmpB5A->hd;enum Cyc_Absyn_Format_Type _tmpC43;int _tmpC44;int _tmpC45;_LL60E: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpC3F=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpC3E;if(_tmpC3F->tag != 1)goto _LL610;}_LL60F:
# 3942
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL60D;_LL610: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpC40=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpC3E;if(_tmpC40->tag != 2)goto _LL612;}_LL611:
# 3944
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL60D;_LL612: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpC41=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpC3E;if(_tmpC41->tag != 3)goto _LL614;}_LL613:
# 3946
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL60D;_LL614: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpC42=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC3E;if(_tmpC42->tag != 19)goto _LL616;else{_tmpC43=_tmpC42->f1;_tmpC44=_tmpC42->f2;_tmpC45=_tmpC42->f3;}}_LL615:
# 3948
 if(!seen_format){
# 3949
seen_format=1;
# 3950
ft=_tmpC43;
# 3951
fmt_desc_arg=_tmpC44;
# 3952
fmt_arg_start=_tmpC45;}else{
# 3954
const char*_tmp14B3;void*_tmp14B2;(_tmp14B2=0,Cyc_Tcutil_terr(loc,((_tmp14B3="function can't have multiple format attributes",_tag_dyneither(_tmp14B3,sizeof(char),47))),_tag_dyneither(_tmp14B2,sizeof(void*),0)));}
# 3955
goto _LL60D;_LL616:;_LL617:
# 3956
 goto _LL60D;_LL60D:;};}
# 3959
if(num_convs > 1){
# 3960
const char*_tmp14B6;void*_tmp14B5;(_tmp14B5=0,Cyc_Tcutil_terr(loc,((_tmp14B6="function can't have multiple calling conventions",_tag_dyneither(_tmp14B6,sizeof(char),49))),_tag_dyneither(_tmp14B5,sizeof(void*),0)));}
# 3964
Cyc_Tcutil_check_unique_tvars(loc,*_tmpB50);
# 3965
{struct Cyc_List_List*b=*_tmpB50;for(0;b != 0;b=b->tl){
# 3966
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
# 3967
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
# 3968
void*_tmpC4A=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);struct Cyc_Absyn_Kind*_tmpC4C;struct Cyc_Absyn_Kind _tmpC4D;enum Cyc_Absyn_KindQual _tmpC4E;_LL619: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpC4B=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpC4A;if(_tmpC4B->tag != 0)goto _LL61B;else{_tmpC4C=_tmpC4B->f1;_tmpC4D=*_tmpC4C;_tmpC4E=_tmpC4D.kind;if(_tmpC4E != Cyc_Absyn_MemKind)goto _LL61B;}}_LL61A:
# 3970
{const char*_tmp14BA;void*_tmp14B9[1];struct Cyc_String_pa_PrintArg_struct _tmp14B8;(_tmp14B8.tag=0,((_tmp14B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name),((_tmp14B9[0]=& _tmp14B8,Cyc_Tcutil_terr(loc,((_tmp14BA="function attempts to abstract Mem type variable %s",_tag_dyneither(_tmp14BA,sizeof(char),51))),_tag_dyneither(_tmp14B9,sizeof(void*),1)))))));}
# 3971
goto _LL618;_LL61B:;_LL61C:
# 3972
 goto _LL618;_LL618:;};}}{
# 3978
struct _RegionHandle _tmpC52=_new_region("newr");struct _RegionHandle*newr=& _tmpC52;_push_region(newr);{
# 3979
struct Cyc_Tcutil_CVTEnv _tmp14BB;struct Cyc_Tcutil_CVTEnv _tmpC53=
# 3980
(_tmp14BB.r=newr,((_tmp14BB.kind_env=cvtenv.kind_env,((_tmp14BB.free_vars=0,((_tmp14BB.free_evars=0,((_tmp14BB.generalize_evars=cvtenv.generalize_evars,((_tmp14BB.fn_result=1,_tmp14BB)))))))))));
# 3984
_tmpC53=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC53,& Cyc_Tcutil_tmk,_tmpB55,1);
# 3985
_tmpB54->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpB54->print_const,_tmpB55);
# 3986
_tmpC53.fn_result=0;
# 3987
{struct Cyc_List_List*a=_tmpB56;for(0;a != 0;a=a->tl){
# 3988
struct _tuple10*_tmpC54=(struct _tuple10*)a->hd;
# 3989
void*_tmpC55=(*_tmpC54).f3;
# 3990
_tmpC53=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC53,& Cyc_Tcutil_tmk,_tmpC55,1);{
# 3991
int _tmpC56=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpC54).f2).print_const,_tmpC55);
# 3992
((*_tmpC54).f2).real_const=_tmpC56;{
# 3995
void*_tmpC57=Cyc_Tcutil_compress(_tmpC55);struct Cyc_Absyn_ArrayInfo _tmpC59;void*_tmpC5A;struct Cyc_Absyn_Tqual _tmpC5B;struct Cyc_Absyn_Exp*_tmpC5C;union Cyc_Absyn_Constraint*_tmpC5D;unsigned int _tmpC5E;_LL61E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC58=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC57;if(_tmpC58->tag != 8)goto _LL620;else{_tmpC59=_tmpC58->f1;_tmpC5A=_tmpC59.elt_type;_tmpC5B=_tmpC59.tq;_tmpC5C=_tmpC59.num_elts;_tmpC5D=_tmpC59.zero_term;_tmpC5E=_tmpC59.zt_loc;}}_LL61F: {
# 3998
void*_tmpC5F=Cyc_Absyn_new_evar(0,0);
# 3999
_tmpC53=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC53,& Cyc_Tcutil_rk,_tmpC5F,1);{
# 4000
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp14BE;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp14BD;void*_tmpC60=Cyc_Absyn_atb_typ(_tmpC5A,_tmpC5F,_tmpC5B,(void*)(
# 4001
(_tmp14BD=_cycalloc(sizeof(*_tmp14BD)),((_tmp14BD[0]=((_tmp14BE.tag=1,((_tmp14BE.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpC5C),_tmp14BE)))),_tmp14BD)))),_tmpC5D);
# 4002
(*_tmpC54).f3=_tmpC60;
# 4003
goto _LL61D;};}_LL620:;_LL621:
# 4004
 goto _LL61D;_LL61D:;};};}}
# 4009
if(_tmpB58 != 0){
# 4010
if(_tmpB57){const char*_tmp14C1;void*_tmp14C0;(_tmp14C0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp14C1="both c_vararg and cyc_vararg",_tag_dyneither(_tmp14C1,sizeof(char),29))),_tag_dyneither(_tmp14C0,sizeof(void*),0)));}{
# 4011
void*_tmpC66;int _tmpC67;struct Cyc_Absyn_VarargInfo _tmpC65=*_tmpB58;_tmpC66=_tmpC65.type;_tmpC67=_tmpC65.inject;
# 4012
_tmpC53=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC53,& Cyc_Tcutil_tmk,_tmpC66,1);
# 4013
(_tmpB58->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmpB58->tq).print_const,_tmpC66);
# 4015
if(_tmpC67){
# 4016
void*_tmpC68=Cyc_Tcutil_compress(_tmpC66);struct Cyc_Absyn_PtrInfo _tmpC6A;void*_tmpC6B;struct Cyc_Absyn_PtrAtts _tmpC6C;union Cyc_Absyn_Constraint*_tmpC6D;union Cyc_Absyn_Constraint*_tmpC6E;_LL623: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC69=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC68;if(_tmpC69->tag != 5)goto _LL625;else{_tmpC6A=_tmpC69->f1;_tmpC6B=_tmpC6A.elt_typ;_tmpC6C=_tmpC6A.ptr_atts;_tmpC6D=_tmpC6C.bounds;_tmpC6E=_tmpC6C.zero_term;}}_LL624:
# 4018
{void*_tmpC6F=Cyc_Tcutil_compress(_tmpC6B);_LL628: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpC70=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpC6F;if(_tmpC70->tag != 3)goto _LL62A;}_LL629:
# 4020
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmpC6E)){
# 4021
const char*_tmp14C4;void*_tmp14C3;(_tmp14C3=0,Cyc_Tcutil_terr(loc,((_tmp14C4="can't inject into a zeroterm pointer",_tag_dyneither(_tmp14C4,sizeof(char),37))),_tag_dyneither(_tmp14C3,sizeof(void*),0)));}
# 4022
{void*_tmpC73=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpC6D);_LL62D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpC74=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpC73;if(_tmpC74->tag != 0)goto _LL62F;}_LL62E:
# 4024
{const char*_tmp14C7;void*_tmp14C6;(_tmp14C6=0,Cyc_Tcutil_terr(loc,((_tmp14C7="can't inject into a fat pointer to datatype",_tag_dyneither(_tmp14C7,sizeof(char),44))),_tag_dyneither(_tmp14C6,sizeof(void*),0)));}
# 4025
goto _LL62C;_LL62F:;_LL630:
# 4026
 goto _LL62C;_LL62C:;}
# 4028
goto _LL627;_LL62A:;_LL62B:
# 4029
{const char*_tmp14CA;void*_tmp14C9;(_tmp14C9=0,Cyc_Tcutil_terr(loc,((_tmp14CA="can't inject a non-datatype type",_tag_dyneither(_tmp14CA,sizeof(char),33))),_tag_dyneither(_tmp14C9,sizeof(void*),0)));}goto _LL627;_LL627:;}
# 4031
goto _LL622;_LL625:;_LL626:
# 4032
{const char*_tmp14CD;void*_tmp14CC;(_tmp14CC=0,Cyc_Tcutil_terr(loc,((_tmp14CD="expecting a datatype pointer type",_tag_dyneither(_tmp14CD,sizeof(char),34))),_tag_dyneither(_tmp14CC,sizeof(void*),0)));}goto _LL622;_LL622:;}};}
# 4037
if(seen_format){
# 4038
int _tmpC7B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpB56);
# 4039
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpC7B) || fmt_arg_start < 0) || 
# 4041
_tmpB58 == 0  && fmt_arg_start != 0) || 
# 4042
_tmpB58 != 0  && fmt_arg_start != _tmpC7B + 1){
# 4043
const char*_tmp14D0;void*_tmp14CF;(_tmp14CF=0,Cyc_Tcutil_terr(loc,((_tmp14D0="bad format descriptor",_tag_dyneither(_tmp14D0,sizeof(char),22))),_tag_dyneither(_tmp14CF,sizeof(void*),0)));}else{
# 4046
void*_tmpC7F;struct _tuple10 _tmpC7E=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpB56,fmt_desc_arg - 1);_tmpC7F=_tmpC7E.f3;
# 4048
{void*_tmpC80=Cyc_Tcutil_compress(_tmpC7F);struct Cyc_Absyn_PtrInfo _tmpC82;void*_tmpC83;struct Cyc_Absyn_PtrAtts _tmpC84;union Cyc_Absyn_Constraint*_tmpC85;union Cyc_Absyn_Constraint*_tmpC86;_LL632: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC81=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC80;if(_tmpC81->tag != 5)goto _LL634;else{_tmpC82=_tmpC81->f1;_tmpC83=_tmpC82.elt_typ;_tmpC84=_tmpC82.ptr_atts;_tmpC85=_tmpC84.bounds;_tmpC86=_tmpC84.zero_term;}}_LL633:
# 4051
{struct _tuple0 _tmp14D1;struct _tuple0 _tmpC88=(_tmp14D1.f1=Cyc_Tcutil_compress(_tmpC83),((_tmp14D1.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpC85),_tmp14D1)));void*_tmpC89;enum Cyc_Absyn_Sign _tmpC8B;enum Cyc_Absyn_Size_of _tmpC8C;void*_tmpC8D;_LL637: _tmpC89=_tmpC88.f1;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpC8A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpC89;if(_tmpC8A->tag != 6)goto _LL639;else{_tmpC8B=_tmpC8A->f1;if(_tmpC8B != Cyc_Absyn_None)goto _LL639;_tmpC8C=_tmpC8A->f2;if(_tmpC8C != Cyc_Absyn_Char_sz)goto _LL639;}};_tmpC8D=_tmpC88.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpC8E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpC8D;if(_tmpC8E->tag != 0)goto _LL639;};_LL638:
# 4052
 goto _LL636;_LL639:;_LL63A:
# 4053
{const char*_tmp14D4;void*_tmp14D3;(_tmp14D3=0,Cyc_Tcutil_terr(loc,((_tmp14D4="format descriptor is not a char ? type",_tag_dyneither(_tmp14D4,sizeof(char),39))),_tag_dyneither(_tmp14D3,sizeof(void*),0)));}goto _LL636;_LL636:;}
# 4055
goto _LL631;_LL634:;_LL635:
# 4056
{const char*_tmp14D7;void*_tmp14D6;(_tmp14D6=0,Cyc_Tcutil_terr(loc,((_tmp14D7="format descriptor is not a char ? type",_tag_dyneither(_tmp14D7,sizeof(char),39))),_tag_dyneither(_tmp14D6,sizeof(void*),0)));}goto _LL631;_LL631:;}
# 4058
if(fmt_arg_start != 0){
# 4062
int problem;
# 4063
{struct _tuple30 _tmp14D8;struct _tuple30 _tmpC94=(_tmp14D8.f1=ft,((_tmp14D8.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmpB58))->type)),_tmp14D8)));enum Cyc_Absyn_Format_Type _tmpC95;void*_tmpC96;struct Cyc_Absyn_DatatypeInfo _tmpC98;union Cyc_Absyn_DatatypeInfoU _tmpC99;struct Cyc_Absyn_Datatypedecl**_tmpC9A;struct Cyc_Absyn_Datatypedecl*_tmpC9B;enum Cyc_Absyn_Format_Type _tmpC9C;void*_tmpC9D;struct Cyc_Absyn_DatatypeInfo _tmpC9F;union Cyc_Absyn_DatatypeInfoU _tmpCA0;struct Cyc_Absyn_Datatypedecl**_tmpCA1;struct Cyc_Absyn_Datatypedecl*_tmpCA2;_LL63C: _tmpC95=_tmpC94.f1;if(_tmpC95 != Cyc_Absyn_Printf_ft)goto _LL63E;_tmpC96=_tmpC94.f2;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpC97=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpC96;if(_tmpC97->tag != 3)goto _LL63E;else{_tmpC98=_tmpC97->f1;_tmpC99=_tmpC98.datatype_info;if((_tmpC99.KnownDatatype).tag != 2)goto _LL63E;_tmpC9A=(struct Cyc_Absyn_Datatypedecl**)(_tmpC99.KnownDatatype).val;_tmpC9B=*_tmpC9A;}};_LL63D:
# 4065
 problem=Cyc_Absyn_qvar_cmp(_tmpC9B->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL63B;_LL63E: _tmpC9C=_tmpC94.f1;if(_tmpC9C != Cyc_Absyn_Scanf_ft)goto _LL640;_tmpC9D=_tmpC94.f2;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpC9E=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpC9D;if(_tmpC9E->tag != 3)goto _LL640;else{_tmpC9F=_tmpC9E->f1;_tmpCA0=_tmpC9F.datatype_info;if((_tmpCA0.KnownDatatype).tag != 2)goto _LL640;_tmpCA1=(struct Cyc_Absyn_Datatypedecl**)(_tmpCA0.KnownDatatype).val;_tmpCA2=*_tmpCA1;}};_LL63F:
# 4067
 problem=Cyc_Absyn_qvar_cmp(_tmpCA2->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL63B;_LL640:;_LL641:
# 4069
 problem=1;goto _LL63B;_LL63B:;}
# 4071
if(problem){
# 4072
const char*_tmp14DB;void*_tmp14DA;(_tmp14DA=0,Cyc_Tcutil_terr(loc,((_tmp14DB="format attribute and vararg types don't match",_tag_dyneither(_tmp14DB,sizeof(char),46))),_tag_dyneither(_tmp14DA,sizeof(void*),0)));}}}}
# 4079
{struct Cyc_List_List*rpo=_tmpB59;for(0;rpo != 0;rpo=rpo->tl){
# 4080
struct _tuple0 _tmpCA6;void*_tmpCA7;void*_tmpCA8;struct _tuple0*_tmpCA5=(struct _tuple0*)rpo->hd;_tmpCA6=*_tmpCA5;_tmpCA7=_tmpCA6.f1;_tmpCA8=_tmpCA6.f2;
# 4081
_tmpC53=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC53,& Cyc_Tcutil_ek,_tmpCA7,1);
# 4082
_tmpC53=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC53,& Cyc_Tcutil_trk,_tmpCA8,1);}}
# 4085
if(*_tmpB52 != 0)
# 4086
_tmpC53=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC53,& Cyc_Tcutil_ek,(void*)_check_null(*_tmpB52),1);else{
# 4088
struct Cyc_List_List*effect=0;
# 4093
{struct Cyc_List_List*tvs=_tmpC53.free_vars;for(0;tvs != 0;tvs=tvs->tl){
# 4094
struct Cyc_Absyn_Tvar*_tmpCAA;int _tmpCAB;struct _tuple28 _tmpCA9=*((struct _tuple28*)tvs->hd);_tmpCAA=_tmpCA9.f1;_tmpCAB=_tmpCA9.f2;
# 4095
if(!_tmpCAB)continue;{
# 4096
void*_tmpCAC=Cyc_Absyn_compress_kb(_tmpCAA->kind);struct Cyc_Core_Opt*_tmpCAE;struct Cyc_Core_Opt**_tmpCAF;struct Cyc_Absyn_Kind*_tmpCB0;struct Cyc_Absyn_Kind*_tmpCB2;struct Cyc_Absyn_Kind*_tmpCB4;struct Cyc_Absyn_Kind _tmpCB5;enum Cyc_Absyn_KindQual _tmpCB6;struct Cyc_Absyn_Kind*_tmpCB8;struct Cyc_Absyn_Kind _tmpCB9;enum Cyc_Absyn_KindQual _tmpCBA;struct Cyc_Core_Opt*_tmpCBC;struct Cyc_Core_Opt**_tmpCBD;struct Cyc_Absyn_Kind*_tmpCBE;struct Cyc_Absyn_Kind _tmpCBF;enum Cyc_Absyn_KindQual _tmpCC0;struct Cyc_Absyn_Kind*_tmpCC2;struct Cyc_Absyn_Kind _tmpCC3;enum Cyc_Absyn_KindQual _tmpCC4;struct Cyc_Core_Opt*_tmpCC6;struct Cyc_Core_Opt**_tmpCC7;_LL643:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCAD=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCAC;if(_tmpCAD->tag != 2)goto _LL645;else{_tmpCAE=_tmpCAD->f1;_tmpCAF=(struct Cyc_Core_Opt**)& _tmpCAD->f1;_tmpCB0=_tmpCAD->f2;}}if(!(_tmpCB0->kind == Cyc_Absyn_RgnKind))goto _LL645;_LL644:
# 4099
 if(_tmpCB0->aliasqual == Cyc_Absyn_Top){
# 4100
*_tmpCAF=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmpCB2=_tmpCB0;goto _LL646;}
# 4102
*_tmpCAF=Cyc_Tcutil_kind_to_bound_opt(_tmpCB0);_tmpCB2=_tmpCB0;goto _LL646;_LL645:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpCB1=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpCAC;if(_tmpCB1->tag != 0)goto _LL647;else{_tmpCB2=_tmpCB1->f1;}}if(!(_tmpCB2->kind == Cyc_Absyn_RgnKind))goto _LL647;_LL646:
# 4104
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp14EA;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp14E9;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp14E8;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp14E7;struct Cyc_List_List*_tmp14E6;effect=((_tmp14E6=_cycalloc(sizeof(*_tmp14E6)),((_tmp14E6->hd=(void*)((_tmp14EA=_cycalloc(sizeof(*_tmp14EA)),((_tmp14EA[0]=((_tmp14E7.tag=23,((_tmp14E7.f1=(void*)((void*)((_tmp14E9=_cycalloc(sizeof(*_tmp14E9)),((_tmp14E9[0]=((_tmp14E8.tag=2,((_tmp14E8.f1=_tmpCAA,_tmp14E8)))),_tmp14E9))))),_tmp14E7)))),_tmp14EA)))),((_tmp14E6->tl=effect,_tmp14E6))))));}goto _LL642;_LL647: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCB3=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCAC;if(_tmpCB3->tag != 2)goto _LL649;else{_tmpCB4=_tmpCB3->f2;_tmpCB5=*_tmpCB4;_tmpCB6=_tmpCB5.kind;if(_tmpCB6 != Cyc_Absyn_IntKind)goto _LL649;}}_LL648:
# 4105
 goto _LL64A;_LL649: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpCB7=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpCAC;if(_tmpCB7->tag != 0)goto _LL64B;else{_tmpCB8=_tmpCB7->f1;_tmpCB9=*_tmpCB8;_tmpCBA=_tmpCB9.kind;if(_tmpCBA != Cyc_Absyn_IntKind)goto _LL64B;}}_LL64A:
# 4106
 goto _LL642;_LL64B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCBB=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCAC;if(_tmpCBB->tag != 2)goto _LL64D;else{_tmpCBC=_tmpCBB->f1;_tmpCBD=(struct Cyc_Core_Opt**)& _tmpCBB->f1;_tmpCBE=_tmpCBB->f2;_tmpCBF=*_tmpCBE;_tmpCC0=_tmpCBF.kind;if(_tmpCC0 != Cyc_Absyn_EffKind)goto _LL64D;}}_LL64C:
# 4108
*_tmpCBD=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL64E;_LL64D: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpCC1=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpCAC;if(_tmpCC1->tag != 0)goto _LL64F;else{_tmpCC2=_tmpCC1->f1;_tmpCC3=*_tmpCC2;_tmpCC4=_tmpCC3.kind;if(_tmpCC4 != Cyc_Absyn_EffKind)goto _LL64F;}}_LL64E:
# 4110
{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp14F0;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp14EF;struct Cyc_List_List*_tmp14EE;effect=((_tmp14EE=_cycalloc(sizeof(*_tmp14EE)),((_tmp14EE->hd=(void*)((_tmp14F0=_cycalloc(sizeof(*_tmp14F0)),((_tmp14F0[0]=((_tmp14EF.tag=2,((_tmp14EF.f1=_tmpCAA,_tmp14EF)))),_tmp14F0)))),((_tmp14EE->tl=effect,_tmp14EE))))));}goto _LL642;_LL64F: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpCC5=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpCAC;if(_tmpCC5->tag != 1)goto _LL651;else{_tmpCC6=_tmpCC5->f1;_tmpCC7=(struct Cyc_Core_Opt**)& _tmpCC5->f1;}}_LL650:
# 4112
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp14F6;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp14F5;struct Cyc_Core_Opt*_tmp14F4;*_tmpCC7=((_tmp14F4=_cycalloc(sizeof(*_tmp14F4)),((_tmp14F4->v=(void*)((_tmp14F6=_cycalloc(sizeof(*_tmp14F6)),((_tmp14F6[0]=((_tmp14F5.tag=2,((_tmp14F5.f1=0,((_tmp14F5.f2=& Cyc_Tcutil_ak,_tmp14F5)))))),_tmp14F6)))),_tmp14F4))));}goto _LL652;_LL651:;_LL652:
# 4115
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1505;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1504;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1503;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp1502;struct Cyc_List_List*_tmp1501;effect=((_tmp1501=_cycalloc(sizeof(*_tmp1501)),((_tmp1501->hd=(void*)((_tmp1505=_cycalloc(sizeof(*_tmp1505)),((_tmp1505[0]=((_tmp1502.tag=25,((_tmp1502.f1=(void*)((void*)((_tmp1504=_cycalloc(sizeof(*_tmp1504)),((_tmp1504[0]=((_tmp1503.tag=2,((_tmp1503.f1=_tmpCAA,_tmp1503)))),_tmp1504))))),_tmp1502)))),_tmp1505)))),((_tmp1501->tl=effect,_tmp1501))))));}goto _LL642;_LL642:;};}}
# 4119
{struct Cyc_List_List*ts=_tmpC53.free_evars;for(0;ts != 0;ts=ts->tl){
# 4120
void*_tmpCD9;int _tmpCDA;struct _tuple29 _tmpCD8=*((struct _tuple29*)ts->hd);_tmpCD9=_tmpCD8.f1;_tmpCDA=_tmpCD8.f2;
# 4121
if(!_tmpCDA)continue;{
# 4122
struct Cyc_Absyn_Kind*_tmpCDB=Cyc_Tcutil_typ_kind(_tmpCD9);struct Cyc_Absyn_Kind _tmpCDC;enum Cyc_Absyn_KindQual _tmpCDD;struct Cyc_Absyn_Kind _tmpCDE;enum Cyc_Absyn_KindQual _tmpCDF;struct Cyc_Absyn_Kind _tmpCE0;enum Cyc_Absyn_KindQual _tmpCE1;_LL654: _tmpCDC=*_tmpCDB;_tmpCDD=_tmpCDC.kind;if(_tmpCDD != Cyc_Absyn_RgnKind)goto _LL656;_LL655:
# 4124
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp150B;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp150A;struct Cyc_List_List*_tmp1509;effect=((_tmp1509=_cycalloc(sizeof(*_tmp1509)),((_tmp1509->hd=(void*)((_tmp150B=_cycalloc(sizeof(*_tmp150B)),((_tmp150B[0]=((_tmp150A.tag=23,((_tmp150A.f1=(void*)_tmpCD9,_tmp150A)))),_tmp150B)))),((_tmp1509->tl=effect,_tmp1509))))));}goto _LL653;_LL656: _tmpCDE=*_tmpCDB;_tmpCDF=_tmpCDE.kind;if(_tmpCDF != Cyc_Absyn_EffKind)goto _LL658;_LL657:
# 4126
{struct Cyc_List_List*_tmp150C;effect=((_tmp150C=_cycalloc(sizeof(*_tmp150C)),((_tmp150C->hd=_tmpCD9,((_tmp150C->tl=effect,_tmp150C))))));}goto _LL653;_LL658: _tmpCE0=*_tmpCDB;_tmpCE1=_tmpCE0.kind;if(_tmpCE1 != Cyc_Absyn_IntKind)goto _LL65A;_LL659:
# 4127
 goto _LL653;_LL65A:;_LL65B:
# 4129
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1512;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp1511;struct Cyc_List_List*_tmp1510;effect=((_tmp1510=_cycalloc(sizeof(*_tmp1510)),((_tmp1510->hd=(void*)((_tmp1512=_cycalloc(sizeof(*_tmp1512)),((_tmp1512[0]=((_tmp1511.tag=25,((_tmp1511.f1=(void*)_tmpCD9,_tmp1511)))),_tmp1512)))),((_tmp1510->tl=effect,_tmp1510))))));}goto _LL653;_LL653:;};}}{
# 4132
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1515;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1514;*_tmpB52=(void*)((_tmp1514=_cycalloc(sizeof(*_tmp1514)),((_tmp1514[0]=((_tmp1515.tag=24,((_tmp1515.f1=effect,_tmp1515)))),_tmp1514))));};}
# 4138
if(*_tmpB50 != 0){
# 4139
struct Cyc_List_List*bs=*_tmpB50;for(0;bs != 0;bs=bs->tl){
# 4140
void*_tmpCEB=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*_tmpCED;struct Cyc_Core_Opt**_tmpCEE;struct Cyc_Core_Opt*_tmpCF0;struct Cyc_Core_Opt**_tmpCF1;struct Cyc_Absyn_Kind*_tmpCF2;struct Cyc_Absyn_Kind _tmpCF3;enum Cyc_Absyn_KindQual _tmpCF4;enum Cyc_Absyn_AliasQual _tmpCF5;struct Cyc_Core_Opt*_tmpCF7;struct Cyc_Core_Opt**_tmpCF8;struct Cyc_Absyn_Kind*_tmpCF9;struct Cyc_Absyn_Kind _tmpCFA;enum Cyc_Absyn_KindQual _tmpCFB;enum Cyc_Absyn_AliasQual _tmpCFC;struct Cyc_Core_Opt*_tmpCFE;struct Cyc_Core_Opt**_tmpCFF;struct Cyc_Absyn_Kind*_tmpD00;struct Cyc_Absyn_Kind _tmpD01;enum Cyc_Absyn_KindQual _tmpD02;enum Cyc_Absyn_AliasQual _tmpD03;struct Cyc_Core_Opt*_tmpD05;struct Cyc_Core_Opt**_tmpD06;struct Cyc_Absyn_Kind*_tmpD07;struct Cyc_Absyn_Kind _tmpD08;enum Cyc_Absyn_KindQual _tmpD09;enum Cyc_Absyn_AliasQual _tmpD0A;struct Cyc_Core_Opt*_tmpD0C;struct Cyc_Core_Opt**_tmpD0D;struct Cyc_Absyn_Kind*_tmpD0E;struct Cyc_Absyn_Kind _tmpD0F;enum Cyc_Absyn_KindQual _tmpD10;enum Cyc_Absyn_AliasQual _tmpD11;struct Cyc_Core_Opt*_tmpD13;struct Cyc_Core_Opt**_tmpD14;struct Cyc_Absyn_Kind*_tmpD15;struct Cyc_Absyn_Kind _tmpD16;enum Cyc_Absyn_KindQual _tmpD17;enum Cyc_Absyn_AliasQual _tmpD18;struct Cyc_Core_Opt*_tmpD1A;struct Cyc_Core_Opt**_tmpD1B;struct Cyc_Absyn_Kind*_tmpD1C;struct Cyc_Absyn_Kind _tmpD1D;enum Cyc_Absyn_KindQual _tmpD1E;enum Cyc_Absyn_AliasQual _tmpD1F;struct Cyc_Core_Opt*_tmpD21;struct Cyc_Core_Opt**_tmpD22;struct Cyc_Absyn_Kind*_tmpD23;struct Cyc_Absyn_Kind _tmpD24;enum Cyc_Absyn_KindQual _tmpD25;enum Cyc_Absyn_AliasQual _tmpD26;struct Cyc_Core_Opt*_tmpD28;struct Cyc_Core_Opt**_tmpD29;struct Cyc_Absyn_Kind*_tmpD2A;struct Cyc_Absyn_Kind*_tmpD2C;struct Cyc_Absyn_Kind _tmpD2D;enum Cyc_Absyn_KindQual _tmpD2E;_LL65D: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpCEC=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpCEB;if(_tmpCEC->tag != 1)goto _LL65F;else{_tmpCED=_tmpCEC->f1;_tmpCEE=(struct Cyc_Core_Opt**)& _tmpCEC->f1;}}_LL65E:
# 4142
{const char*_tmp1519;void*_tmp1518[1];struct Cyc_String_pa_PrintArg_struct _tmp1517;(_tmp1517.tag=0,((_tmp1517.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp1518[0]=& _tmp1517,Cyc_Tcutil_warn(loc,((_tmp1519="Type variable %s unconstrained, assuming boxed",_tag_dyneither(_tmp1519,sizeof(char),47))),_tag_dyneither(_tmp1518,sizeof(void*),1)))))));}
# 4144
_tmpCF1=_tmpCEE;goto _LL660;_LL65F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCEF=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCEB;if(_tmpCEF->tag != 2)goto _LL661;else{_tmpCF0=_tmpCEF->f1;_tmpCF1=(struct Cyc_Core_Opt**)& _tmpCEF->f1;_tmpCF2=_tmpCEF->f2;_tmpCF3=*_tmpCF2;_tmpCF4=_tmpCF3.kind;if(_tmpCF4 != Cyc_Absyn_BoxKind)goto _LL661;_tmpCF5=_tmpCF3.aliasqual;if(_tmpCF5 != Cyc_Absyn_Top)goto _LL661;}}_LL660:
# 4145
 _tmpCF8=_tmpCF1;goto _LL662;_LL661: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCF6=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCEB;if(_tmpCF6->tag != 2)goto _LL663;else{_tmpCF7=_tmpCF6->f1;_tmpCF8=(struct Cyc_Core_Opt**)& _tmpCF6->f1;_tmpCF9=_tmpCF6->f2;_tmpCFA=*_tmpCF9;_tmpCFB=_tmpCFA.kind;if(_tmpCFB != Cyc_Absyn_MemKind)goto _LL663;_tmpCFC=_tmpCFA.aliasqual;if(_tmpCFC != Cyc_Absyn_Top)goto _LL663;}}_LL662:
# 4146
 _tmpCFF=_tmpCF8;goto _LL664;_LL663: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCFD=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCEB;if(_tmpCFD->tag != 2)goto _LL665;else{_tmpCFE=_tmpCFD->f1;_tmpCFF=(struct Cyc_Core_Opt**)& _tmpCFD->f1;_tmpD00=_tmpCFD->f2;_tmpD01=*_tmpD00;_tmpD02=_tmpD01.kind;if(_tmpD02 != Cyc_Absyn_MemKind)goto _LL665;_tmpD03=_tmpD01.aliasqual;if(_tmpD03 != Cyc_Absyn_Aliasable)goto _LL665;}}_LL664:
# 4147
 _tmpD06=_tmpCFF;goto _LL666;_LL665: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD04=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCEB;if(_tmpD04->tag != 2)goto _LL667;else{_tmpD05=_tmpD04->f1;_tmpD06=(struct Cyc_Core_Opt**)& _tmpD04->f1;_tmpD07=_tmpD04->f2;_tmpD08=*_tmpD07;_tmpD09=_tmpD08.kind;if(_tmpD09 != Cyc_Absyn_AnyKind)goto _LL667;_tmpD0A=_tmpD08.aliasqual;if(_tmpD0A != Cyc_Absyn_Top)goto _LL667;}}_LL666:
# 4148
 _tmpD0D=_tmpD06;goto _LL668;_LL667: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD0B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCEB;if(_tmpD0B->tag != 2)goto _LL669;else{_tmpD0C=_tmpD0B->f1;_tmpD0D=(struct Cyc_Core_Opt**)& _tmpD0B->f1;_tmpD0E=_tmpD0B->f2;_tmpD0F=*_tmpD0E;_tmpD10=_tmpD0F.kind;if(_tmpD10 != Cyc_Absyn_AnyKind)goto _LL669;_tmpD11=_tmpD0F.aliasqual;if(_tmpD11 != Cyc_Absyn_Aliasable)goto _LL669;}}_LL668:
# 4150
*_tmpD0D=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL65C;_LL669: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD12=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCEB;if(_tmpD12->tag != 2)goto _LL66B;else{_tmpD13=_tmpD12->f1;_tmpD14=(struct Cyc_Core_Opt**)& _tmpD12->f1;_tmpD15=_tmpD12->f2;_tmpD16=*_tmpD15;_tmpD17=_tmpD16.kind;if(_tmpD17 != Cyc_Absyn_MemKind)goto _LL66B;_tmpD18=_tmpD16.aliasqual;if(_tmpD18 != Cyc_Absyn_Unique)goto _LL66B;}}_LL66A:
# 4151
 _tmpD1B=_tmpD14;goto _LL66C;_LL66B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD19=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCEB;if(_tmpD19->tag != 2)goto _LL66D;else{_tmpD1A=_tmpD19->f1;_tmpD1B=(struct Cyc_Core_Opt**)& _tmpD19->f1;_tmpD1C=_tmpD19->f2;_tmpD1D=*_tmpD1C;_tmpD1E=_tmpD1D.kind;if(_tmpD1E != Cyc_Absyn_AnyKind)goto _LL66D;_tmpD1F=_tmpD1D.aliasqual;if(_tmpD1F != Cyc_Absyn_Unique)goto _LL66D;}}_LL66C:
# 4153
*_tmpD1B=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL65C;_LL66D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD20=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCEB;if(_tmpD20->tag != 2)goto _LL66F;else{_tmpD21=_tmpD20->f1;_tmpD22=(struct Cyc_Core_Opt**)& _tmpD20->f1;_tmpD23=_tmpD20->f2;_tmpD24=*_tmpD23;_tmpD25=_tmpD24.kind;if(_tmpD25 != Cyc_Absyn_RgnKind)goto _LL66F;_tmpD26=_tmpD24.aliasqual;if(_tmpD26 != Cyc_Absyn_Top)goto _LL66F;}}_LL66E:
# 4155
*_tmpD22=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL65C;_LL66F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD27=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCEB;if(_tmpD27->tag != 2)goto _LL671;else{_tmpD28=_tmpD27->f1;_tmpD29=(struct Cyc_Core_Opt**)& _tmpD27->f1;_tmpD2A=_tmpD27->f2;}}_LL670:
# 4157
*_tmpD29=Cyc_Tcutil_kind_to_bound_opt(_tmpD2A);goto _LL65C;_LL671: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpD2B=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpCEB;if(_tmpD2B->tag != 0)goto _LL673;else{_tmpD2C=_tmpD2B->f1;_tmpD2D=*_tmpD2C;_tmpD2E=_tmpD2D.kind;if(_tmpD2E != Cyc_Absyn_MemKind)goto _LL673;}}_LL672:
# 4159
{const char*_tmp151C;void*_tmp151B;(_tmp151B=0,Cyc_Tcutil_terr(loc,((_tmp151C="functions can't abstract M types",_tag_dyneither(_tmp151C,sizeof(char),33))),_tag_dyneither(_tmp151B,sizeof(void*),0)));}goto _LL65C;_LL673:;_LL674:
# 4160
 goto _LL65C;_LL65C:;}}
# 4164
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmpC53.kind_env,*_tmpB50);
# 4165
_tmpC53.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmpC53.r,_tmpC53.free_vars,*_tmpB50);
# 4167
{struct Cyc_List_List*tvs=_tmpC53.free_vars;for(0;tvs != 0;tvs=tvs->tl){
# 4168
struct Cyc_Absyn_Tvar*_tmpD35;int _tmpD36;struct _tuple28 _tmpD34=*((struct _tuple28*)tvs->hd);_tmpD35=_tmpD34.f1;_tmpD36=_tmpD34.f2;
# 4169
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpD35,_tmpD36);}}{
# 4172
struct Cyc_List_List*evs=_tmpC53.free_evars;for(0;evs != 0;evs=evs->tl){
# 4173
void*_tmpD38;int _tmpD39;struct _tuple29 _tmpD37=*((struct _tuple29*)evs->hd);_tmpD38=_tmpD37.f1;_tmpD39=_tmpD37.f2;
# 4174
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmpD38,_tmpD39);}};}
# 4177
_npop_handler(0);goto _LL577;
# 3978
;_pop_region(newr);};}_LL596: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpB5B=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpB14;if(_tmpB5B->tag != 10)goto _LL598;else{_tmpB5C=_tmpB5B->f1;}}_LL597:
# 4180
 for(0;_tmpB5C != 0;_tmpB5C=_tmpB5C->tl){
# 4181
struct _tuple12*_tmpD3B=(struct _tuple12*)_tmpB5C->hd;
# 4182
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpD3B).f2,1);
# 4183
((*_tmpD3B).f1).real_const=
# 4184
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpD3B).f1).print_const,(*_tmpD3B).f2);}
# 4186
goto _LL577;_LL598: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB5D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpB14;if(_tmpB5D->tag != 12)goto _LL59A;else{_tmpB5E=_tmpB5D->f1;_tmpB5F=_tmpB5D->f2;}}_LL599: {
# 4190
struct _RegionHandle _tmpD3C=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmpD3C;_push_region(aprev_rgn);{
# 4191
struct Cyc_List_List*prev_fields=0;
# 4192
for(0;_tmpB5F != 0;_tmpB5F=_tmpB5F->tl){
# 4193
struct Cyc_Absyn_Aggrfield _tmpD3E;struct _dyneither_ptr*_tmpD3F;struct Cyc_Absyn_Tqual _tmpD40;struct Cyc_Absyn_Tqual*_tmpD41;void*_tmpD42;struct Cyc_Absyn_Exp*_tmpD43;struct Cyc_List_List*_tmpD44;struct Cyc_Absyn_Aggrfield*_tmpD3D=(struct Cyc_Absyn_Aggrfield*)_tmpB5F->hd;_tmpD3E=*_tmpD3D;_tmpD3F=_tmpD3E.name;_tmpD40=_tmpD3E.tq;_tmpD41=(struct Cyc_Absyn_Tqual*)&(*_tmpD3D).tq;_tmpD42=_tmpD3E.type;_tmpD43=_tmpD3E.width;_tmpD44=_tmpD3E.attributes;
# 4194
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpD3F)){
# 4195
const char*_tmp1520;void*_tmp151F[1];struct Cyc_String_pa_PrintArg_struct _tmp151E;(_tmp151E.tag=0,((_tmp151E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpD3F),((_tmp151F[0]=& _tmp151E,Cyc_Tcutil_terr(loc,((_tmp1520="duplicate field %s",_tag_dyneither(_tmp1520,sizeof(char),19))),_tag_dyneither(_tmp151F,sizeof(void*),1)))))));}
# 4196
{const char*_tmp1521;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpD3F,((_tmp1521="",_tag_dyneither(_tmp1521,sizeof(char),1))))!= 0){
# 4197
struct Cyc_List_List*_tmp1522;prev_fields=((_tmp1522=_region_malloc(aprev_rgn,sizeof(*_tmp1522)),((_tmp1522->hd=_tmpD3F,((_tmp1522->tl=prev_fields,_tmp1522))))));}}
# 4198
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpD42,1);
# 4199
_tmpD41->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpD41->print_const,_tmpD42);
# 4200
if(_tmpB5E == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmpD42)){
# 4202
const char*_tmp1526;void*_tmp1525[1];struct Cyc_String_pa_PrintArg_struct _tmp1524;(_tmp1524.tag=0,((_tmp1524.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpD3F),((_tmp1525[0]=& _tmp1524,Cyc_Tcutil_warn(loc,((_tmp1526="union member %s is not `bits-only' so it can only be written and not read",_tag_dyneither(_tmp1526,sizeof(char),74))),_tag_dyneither(_tmp1525,sizeof(void*),1)))))));}
# 4203
Cyc_Tcutil_check_bitfield(loc,te,_tmpD42,_tmpD43,_tmpD3F);
# 4204
Cyc_Tcutil_check_field_atts(loc,_tmpD3F,_tmpD44);}}
# 4207
_npop_handler(0);goto _LL577;
# 4190
;_pop_region(aprev_rgn);}_LL59A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB60=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB14;if(_tmpB60->tag != 11)goto _LL59C;else{_tmpB61=_tmpB60->f1;_tmpB62=_tmpB61.aggr_info;_tmpB63=(union Cyc_Absyn_AggrInfoU*)&(_tmpB60->f1).aggr_info;_tmpB64=_tmpB61.targs;_tmpB65=(struct Cyc_List_List**)&(_tmpB60->f1).targs;}}_LL59B:
# 4210
{union Cyc_Absyn_AggrInfoU _tmpD4D=*_tmpB63;struct _tuple4 _tmpD4E;enum Cyc_Absyn_AggrKind _tmpD4F;struct _tuple2*_tmpD50;struct Cyc_Core_Opt*_tmpD51;struct Cyc_Absyn_Aggrdecl**_tmpD52;struct Cyc_Absyn_Aggrdecl*_tmpD53;_LL676: if((_tmpD4D.UnknownAggr).tag != 1)goto _LL678;_tmpD4E=(struct _tuple4)(_tmpD4D.UnknownAggr).val;_tmpD4F=_tmpD4E.f1;_tmpD50=_tmpD4E.f2;_tmpD51=_tmpD4E.f3;_LL677: {
# 4212
struct Cyc_Absyn_Aggrdecl**adp;
# 4213
{struct _handler_cons _tmpD54;_push_handler(& _tmpD54);{int _tmpD56=0;if(setjmp(_tmpD54.handler))_tmpD56=1;if(!_tmpD56){
# 4214
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpD50);{
# 4215
struct Cyc_Absyn_Aggrdecl*_tmpD57=*adp;
# 4216
if(_tmpD57->kind != _tmpD4F){
# 4217
if(_tmpD57->kind == Cyc_Absyn_StructA){
# 4218
const char*_tmp152B;void*_tmp152A[2];struct Cyc_String_pa_PrintArg_struct _tmp1529;struct Cyc_String_pa_PrintArg_struct _tmp1528;(_tmp1528.tag=0,((_tmp1528.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4219
Cyc_Absynpp_qvar2string(_tmpD50)),((_tmp1529.tag=0,((_tmp1529.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4218
Cyc_Absynpp_qvar2string(_tmpD50)),((_tmp152A[0]=& _tmp1529,((_tmp152A[1]=& _tmp1528,Cyc_Tcutil_terr(loc,((_tmp152B="expecting struct %s instead of union %s",_tag_dyneither(_tmp152B,sizeof(char),40))),_tag_dyneither(_tmp152A,sizeof(void*),2)))))))))))));}else{
# 4221
const char*_tmp1530;void*_tmp152F[2];struct Cyc_String_pa_PrintArg_struct _tmp152E;struct Cyc_String_pa_PrintArg_struct _tmp152D;(_tmp152D.tag=0,((_tmp152D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4222
Cyc_Absynpp_qvar2string(_tmpD50)),((_tmp152E.tag=0,((_tmp152E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4221
Cyc_Absynpp_qvar2string(_tmpD50)),((_tmp152F[0]=& _tmp152E,((_tmp152F[1]=& _tmp152D,Cyc_Tcutil_terr(loc,((_tmp1530="expecting union %s instead of struct %s",_tag_dyneither(_tmp1530,sizeof(char),40))),_tag_dyneither(_tmp152F,sizeof(void*),2)))))))))))));}}
# 4224
if((unsigned int)_tmpD51  && (int)_tmpD51->v){
# 4225
if(!((unsigned int)_tmpD57->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD57->impl))->tagged){
# 4226
const char*_tmp1534;void*_tmp1533[1];struct Cyc_String_pa_PrintArg_struct _tmp1532;(_tmp1532.tag=0,((_tmp1532.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4227
Cyc_Absynpp_qvar2string(_tmpD50)),((_tmp1533[0]=& _tmp1532,Cyc_Tcutil_terr(loc,((_tmp1534="@tagged qualfiers don't agree on union %s",_tag_dyneither(_tmp1534,sizeof(char),42))),_tag_dyneither(_tmp1533,sizeof(void*),1)))))));}}
# 4230
*_tmpB63=Cyc_Absyn_KnownAggr(adp);};
# 4214
;_pop_handler();}else{void*_tmpD55=(void*)_exn_thrown;void*_tmpD64=_tmpD55;_LL67B: {struct Cyc_Dict_Absent_exn_struct*_tmpD65=(struct Cyc_Dict_Absent_exn_struct*)_tmpD64;if(_tmpD65->tag != Cyc_Dict_Absent)goto _LL67D;}_LL67C: {
# 4234
struct Cyc_Tcenv_Genv*_tmpD66=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
# 4235
struct Cyc_Absyn_Aggrdecl*_tmp1535;struct Cyc_Absyn_Aggrdecl*_tmpD67=(_tmp1535=_cycalloc(sizeof(*_tmp1535)),((_tmp1535->kind=_tmpD4F,((_tmp1535->sc=Cyc_Absyn_Extern,((_tmp1535->name=_tmpD50,((_tmp1535->tvs=0,((_tmp1535->impl=0,((_tmp1535->attributes=0,_tmp1535)))))))))))));
# 4236
Cyc_Tc_tcAggrdecl(te,_tmpD66,loc,_tmpD67);
# 4237
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpD50);
# 4238
*_tmpB63=Cyc_Absyn_KnownAggr(adp);
# 4240
if(*_tmpB65 != 0){
# 4241
{const char*_tmp1539;void*_tmp1538[1];struct Cyc_String_pa_PrintArg_struct _tmp1537;(_tmp1537.tag=0,((_tmp1537.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4242
Cyc_Absynpp_qvar2string(_tmpD50)),((_tmp1538[0]=& _tmp1537,Cyc_Tcutil_terr(loc,((_tmp1539="declare parameterized type %s before using",_tag_dyneither(_tmp1539,sizeof(char),43))),_tag_dyneither(_tmp1538,sizeof(void*),1)))))));}
# 4243
return cvtenv;}
# 4245
goto _LL67A;}_LL67D:;_LL67E:(void)_throw(_tmpD64);_LL67A:;}};}
# 4247
_tmpD53=*adp;goto _LL679;}_LL678: if((_tmpD4D.KnownAggr).tag != 2)goto _LL675;_tmpD52=(struct Cyc_Absyn_Aggrdecl**)(_tmpD4D.KnownAggr).val;_tmpD53=*_tmpD52;_LL679: {
# 4249
struct Cyc_List_List*tvs=_tmpD53->tvs;
# 4250
struct Cyc_List_List*ts=*_tmpB65;
# 4251
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
# 4252
struct Cyc_Absyn_Tvar*_tmpD6C=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4253
void*_tmpD6D=(void*)ts->hd;
# 4257
{struct _tuple0 _tmp153A;struct _tuple0 _tmpD6F=(_tmp153A.f1=Cyc_Absyn_compress_kb(_tmpD6C->kind),((_tmp153A.f2=_tmpD6D,_tmp153A)));void*_tmpD70;void*_tmpD72;struct Cyc_Absyn_Tvar*_tmpD74;_LL680: _tmpD70=_tmpD6F.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpD71=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpD70;if(_tmpD71->tag != 1)goto _LL682;};_tmpD72=_tmpD6F.f2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpD73=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpD72;if(_tmpD73->tag != 2)goto _LL682;else{_tmpD74=_tmpD73->f1;}};_LL681:
# 4259
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpD74);
# 4260
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpD74,1);
# 4261
continue;_LL682:;_LL683:
# 4262
 goto _LL67F;_LL67F:;}{
# 4264
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
# 4265
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
# 4266
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4268
if(ts != 0){
# 4269
const char*_tmp153E;void*_tmp153D[1];struct Cyc_String_pa_PrintArg_struct _tmp153C;(_tmp153C.tag=0,((_tmp153C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD53->name)),((_tmp153D[0]=& _tmp153C,Cyc_Tcutil_terr(loc,((_tmp153E="too many parameters for type %s",_tag_dyneither(_tmp153E,sizeof(char),32))),_tag_dyneither(_tmp153D,sizeof(void*),1)))))));}
# 4270
if(tvs != 0){
# 4272
struct Cyc_List_List*hidden_ts=0;
# 4273
for(0;tvs != 0;tvs=tvs->tl){
# 4274
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
# 4275
void*e=Cyc_Absyn_new_evar(0,0);
# 4276
{struct Cyc_List_List*_tmp153F;hidden_ts=((_tmp153F=_cycalloc(sizeof(*_tmp153F)),((_tmp153F->hd=e,((_tmp153F->tl=hidden_ts,_tmp153F))))));}
# 4277
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);}
# 4279
*_tmpB65=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpB65,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}}_LL675:;}
# 4282
goto _LL577;_LL59C: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB66=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpB14;if(_tmpB66->tag != 17)goto _LL59E;else{_tmpB67=_tmpB66->f1;_tmpB68=_tmpB66->f2;_tmpB69=(struct Cyc_List_List**)& _tmpB66->f2;_tmpB6A=_tmpB66->f3;_tmpB6B=(struct Cyc_Absyn_Typedefdecl**)& _tmpB66->f3;_tmpB6C=(void**)& _tmpB66->f4;_tmpB6D=(void**)((void**)& _tmpB66->f4);}}_LL59D: {
# 4285
struct Cyc_List_List*targs=*_tmpB69;
# 4287
struct Cyc_Absyn_Typedefdecl*td;
# 4288
{struct _handler_cons _tmpD79;_push_handler(& _tmpD79);{int _tmpD7B=0;if(setjmp(_tmpD79.handler))_tmpD7B=1;if(!_tmpD7B){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmpB67);;_pop_handler();}else{void*_tmpD7A=(void*)_exn_thrown;void*_tmpD7D=_tmpD7A;_LL685: {struct Cyc_Dict_Absent_exn_struct*_tmpD7E=(struct Cyc_Dict_Absent_exn_struct*)_tmpD7D;if(_tmpD7E->tag != Cyc_Dict_Absent)goto _LL687;}_LL686:
# 4290
{const char*_tmp1543;void*_tmp1542[1];struct Cyc_String_pa_PrintArg_struct _tmp1541;(_tmp1541.tag=0,((_tmp1541.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB67)),((_tmp1542[0]=& _tmp1541,Cyc_Tcutil_terr(loc,((_tmp1543="unbound typedef name %s",_tag_dyneither(_tmp1543,sizeof(char),24))),_tag_dyneither(_tmp1542,sizeof(void*),1)))))));}
# 4291
return cvtenv;_LL687:;_LL688:(void)_throw(_tmpD7D);_LL684:;}};}
# 4293
*_tmpB6B=(struct Cyc_Absyn_Typedefdecl*)td;
# 4295
_tmpB67[0]=(td->name)[0];{
# 4296
struct Cyc_List_List*tvs=td->tvs;
# 4297
struct Cyc_List_List*ts=targs;
# 4298
struct _RegionHandle _tmpD82=_new_region("temp");struct _RegionHandle*temp=& _tmpD82;_push_region(temp);{
# 4299
struct Cyc_List_List*inst=0;
# 4301
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
# 4302
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);
# 4306
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
# 4307
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
# 4308
struct _tuple16*_tmp1546;struct Cyc_List_List*_tmp1545;inst=((_tmp1545=_region_malloc(temp,sizeof(*_tmp1545)),((_tmp1545->hd=((_tmp1546=_region_malloc(temp,sizeof(*_tmp1546)),((_tmp1546->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1546->f2=(void*)ts->hd,_tmp1546)))))),((_tmp1545->tl=inst,_tmp1545))))));};}
# 4310
if(ts != 0){
# 4311
const char*_tmp154A;void*_tmp1549[1];struct Cyc_String_pa_PrintArg_struct _tmp1548;(_tmp1548.tag=0,((_tmp1548.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB67)),((_tmp1549[0]=& _tmp1548,Cyc_Tcutil_terr(loc,((_tmp154A="too many parameters for typedef %s",_tag_dyneither(_tmp154A,sizeof(char),35))),_tag_dyneither(_tmp1549,sizeof(void*),1)))))));}
# 4312
if(tvs != 0){
# 4313
struct Cyc_List_List*hidden_ts=0;
# 4315
for(0;tvs != 0;tvs=tvs->tl){
# 4316
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);
# 4317
void*e=Cyc_Absyn_new_evar(0,0);
# 4318
{struct Cyc_List_List*_tmp154B;hidden_ts=((_tmp154B=_cycalloc(sizeof(*_tmp154B)),((_tmp154B->hd=e,((_tmp154B->tl=hidden_ts,_tmp154B))))));}
# 4319
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);{
# 4320
struct _tuple16*_tmp154E;struct Cyc_List_List*_tmp154D;inst=(struct Cyc_List_List*)((_tmp154D=_cycalloc(sizeof(*_tmp154D)),((_tmp154D->hd=(struct _tuple16*)((_tmp154E=_cycalloc(sizeof(*_tmp154E)),((_tmp154E->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp154E->f2=e,_tmp154E)))))),((_tmp154D->tl=inst,_tmp154D))))));};}
# 4323
*_tmpB69=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4325
if(td->defn != 0){
# 4326
void*new_typ=
# 4327
inst == 0?(void*)_check_null(td->defn):
# 4328
 Cyc_Tcutil_rsubstitute(temp,inst,(void*)_check_null(td->defn));
# 4329
*_tmpB6D=(void*)new_typ;}}
# 4332
_npop_handler(0);goto _LL577;
# 4298
;_pop_region(temp);};}_LL59E: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpB6E=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmpB14;if(_tmpB6E->tag != 22)goto _LL5A0;}_LL59F:
# 4333
 goto _LL5A1;_LL5A0: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpB6F=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmpB14;if(_tmpB6F->tag != 21)goto _LL5A2;}_LL5A1:
# 4334
 goto _LL5A3;_LL5A2: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpB70=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmpB14;if(_tmpB70->tag != 20)goto _LL5A4;}_LL5A3:
# 4335
 goto _LL577;_LL5A4: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpB71=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpB14;if(_tmpB71->tag != 15)goto _LL5A6;else{_tmpB72=(void*)_tmpB71->f1;}}_LL5A5:
# 4337
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpB72,1);goto _LL577;_LL5A6: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpB73=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmpB14;if(_tmpB73->tag != 16)goto _LL5A8;else{_tmpB74=(void*)_tmpB73->f1;_tmpB75=(void*)_tmpB73->f2;}}_LL5A7:
# 4340
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpB74,0);
# 4341
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpB75,1);
# 4342
goto _LL577;_LL5A8: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpB76=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmpB14;if(_tmpB76->tag != 23)goto _LL5AA;else{_tmpB77=(void*)_tmpB76->f1;}}_LL5A9:
# 4344
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpB77,1);goto _LL577;_LL5AA: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpB78=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmpB14;if(_tmpB78->tag != 25)goto _LL5AC;else{_tmpB79=(void*)_tmpB78->f1;}}_LL5AB:
# 4346
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB79,1);goto _LL577;_LL5AC: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpB7A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmpB14;if(_tmpB7A->tag != 24)goto _LL577;else{_tmpB7B=_tmpB7A->f1;}}_LL5AD:
# 4348
 for(0;_tmpB7B != 0;_tmpB7B=_tmpB7B->tl){
# 4349
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmpB7B->hd,1);}
# 4350
goto _LL577;_LL577:;}
# 4352
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){
# 4353
{void*_tmpD8B=t;struct Cyc_Core_Opt*_tmpD8D;void*_tmpD8E;_LL68A: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpD8C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpD8B;if(_tmpD8C->tag != 1)goto _LL68C;else{_tmpD8D=_tmpD8C->f1;_tmpD8E=(void*)_tmpD8C->f2;}}_LL68B: {
# 4355
struct _dyneither_ptr s;
# 4356
{struct Cyc_Core_Opt*_tmpD8F=_tmpD8D;struct Cyc_Core_Opt _tmpD90;struct Cyc_Absyn_Kind*_tmpD91;_LL68F: if(_tmpD8F != 0)goto _LL691;_LL690:
# 4357
{const char*_tmp154F;s=((_tmp154F="kind=NULL ",_tag_dyneither(_tmp154F,sizeof(char),11)));}goto _LL68E;_LL691: if(_tmpD8F == 0)goto _LL68E;_tmpD90=*_tmpD8F;_tmpD91=(struct Cyc_Absyn_Kind*)_tmpD90.v;_LL692:
# 4358
{const char*_tmp1553;void*_tmp1552[1];struct Cyc_String_pa_PrintArg_struct _tmp1551;s=(struct _dyneither_ptr)((_tmp1551.tag=0,((_tmp1551.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmpD91)),((_tmp1552[0]=& _tmp1551,Cyc_aprintf(((_tmp1553="kind=%s ",_tag_dyneither(_tmp1553,sizeof(char),9))),_tag_dyneither(_tmp1552,sizeof(void*),1))))))));}goto _LL68E;_LL68E:;}
# 4360
if(_tmpD8E == 0){
# 4361
const char*_tmp1557;void*_tmp1556[1];struct Cyc_String_pa_PrintArg_struct _tmp1555;s=(struct _dyneither_ptr)((_tmp1555.tag=0,((_tmp1555.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1556[0]=& _tmp1555,Cyc_aprintf(((_tmp1557="%s ref=NULL",_tag_dyneither(_tmp1557,sizeof(char),12))),_tag_dyneither(_tmp1556,sizeof(void*),1))))))));}else{
# 4363
const char*_tmp155C;void*_tmp155B[2];struct Cyc_String_pa_PrintArg_struct _tmp155A;struct Cyc_String_pa_PrintArg_struct _tmp1559;s=(struct _dyneither_ptr)((_tmp1559.tag=0,((_tmp1559.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_tmpD8E)),((_tmp155A.tag=0,((_tmp155A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp155B[0]=& _tmp155A,((_tmp155B[1]=& _tmp1559,Cyc_aprintf(((_tmp155C="%s ref=%s",_tag_dyneither(_tmp155C,sizeof(char),10))),_tag_dyneither(_tmp155B,sizeof(void*),2))))))))))))));}
# 4365
goto _LL689;}_LL68C:;_LL68D:
# 4366
 goto _LL689;_LL689:;}{
# 4368
const char*_tmp1562;void*_tmp1561[3];struct Cyc_String_pa_PrintArg_struct _tmp1560;struct Cyc_String_pa_PrintArg_struct _tmp155F;struct Cyc_String_pa_PrintArg_struct _tmp155E;(_tmp155E.tag=0,((_tmp155E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4369
Cyc_Absynpp_kind2string(expected_kind)),((_tmp155F.tag=0,((_tmp155F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp1560.tag=0,((_tmp1560.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1561[0]=& _tmp1560,((_tmp1561[1]=& _tmp155F,((_tmp1561[2]=& _tmp155E,Cyc_Tcutil_terr(loc,((_tmp1562="type %s has kind %s but as used here needs kind %s",_tag_dyneither(_tmp1562,sizeof(char),51))),_tag_dyneither(_tmp1561,sizeof(void*),3)))))))))))))))))));};}
# 4371
return cvtenv;}
# 4379
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4381
{void*_tmpDA2=e->r;struct Cyc_List_List*_tmpDA8;struct Cyc_Absyn_Exp*_tmpDAA;struct Cyc_Absyn_Exp*_tmpDAB;struct Cyc_Absyn_Exp*_tmpDAC;struct Cyc_Absyn_Exp*_tmpDAE;struct Cyc_Absyn_Exp*_tmpDAF;struct Cyc_Absyn_Exp*_tmpDB1;struct Cyc_Absyn_Exp*_tmpDB2;struct Cyc_Absyn_Exp*_tmpDB4;struct Cyc_Absyn_Exp*_tmpDB5;void*_tmpDB7;struct Cyc_Absyn_Exp*_tmpDB8;void*_tmpDBA;void*_tmpDBC;void*_tmpDBE;struct Cyc_Absyn_Exp*_tmpDC0;_LL694: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpDA3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDA2;if(_tmpDA3->tag != 0)goto _LL696;}_LL695:
# 4382
 goto _LL697;_LL696: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpDA4=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpDA2;if(_tmpDA4->tag != 30)goto _LL698;}_LL697:
# 4383
 goto _LL699;_LL698: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpDA5=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpDA2;if(_tmpDA5->tag != 31)goto _LL69A;}_LL699:
# 4384
 goto _LL69B;_LL69A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpDA6=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDA2;if(_tmpDA6->tag != 1)goto _LL69C;}_LL69B:
# 4385
 goto _LL693;_LL69C: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpDA7=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDA2;if(_tmpDA7->tag != 2)goto _LL69E;else{_tmpDA8=_tmpDA7->f2;}}_LL69D:
# 4387
 for(0;_tmpDA8 != 0;_tmpDA8=_tmpDA8->tl){
# 4388
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmpDA8->hd,te,cvtenv);}
# 4389
goto _LL693;_LL69E: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpDA9=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDA2;if(_tmpDA9->tag != 5)goto _LL6A0;else{_tmpDAA=_tmpDA9->f1;_tmpDAB=_tmpDA9->f2;_tmpDAC=_tmpDA9->f3;}}_LL69F:
# 4391
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDAA,te,cvtenv);
# 4392
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDAB,te,cvtenv);
# 4393
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDAC,te,cvtenv);
# 4394
goto _LL693;_LL6A0: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpDAD=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDA2;if(_tmpDAD->tag != 6)goto _LL6A2;else{_tmpDAE=_tmpDAD->f1;_tmpDAF=_tmpDAD->f2;}}_LL6A1:
# 4395
 _tmpDB1=_tmpDAE;_tmpDB2=_tmpDAF;goto _LL6A3;_LL6A2: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpDB0=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDA2;if(_tmpDB0->tag != 7)goto _LL6A4;else{_tmpDB1=_tmpDB0->f1;_tmpDB2=_tmpDB0->f2;}}_LL6A3:
# 4396
 _tmpDB4=_tmpDB1;_tmpDB5=_tmpDB2;goto _LL6A5;_LL6A4: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpDB3=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDA2;if(_tmpDB3->tag != 8)goto _LL6A6;else{_tmpDB4=_tmpDB3->f1;_tmpDB5=_tmpDB3->f2;}}_LL6A5:
# 4398
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDB4,te,cvtenv);
# 4399
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDB5,te,cvtenv);
# 4400
goto _LL693;_LL6A6: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpDB6=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDA2;if(_tmpDB6->tag != 13)goto _LL6A8;else{_tmpDB7=(void*)_tmpDB6->f1;_tmpDB8=_tmpDB6->f2;}}_LL6A7:
# 4402
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDB8,te,cvtenv);
# 4403
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpDB7,1);
# 4404
goto _LL693;_LL6A8: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpDB9=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDA2;if(_tmpDB9->tag != 18)goto _LL6AA;else{_tmpDBA=(void*)_tmpDB9->f1;}}_LL6A9:
# 4405
 _tmpDBC=_tmpDBA;goto _LL6AB;_LL6AA: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpDBB=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpDA2;if(_tmpDBB->tag != 16)goto _LL6AC;else{_tmpDBC=(void*)_tmpDBB->f1;}}_LL6AB:
# 4407
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpDBC,1);
# 4408
goto _LL693;_LL6AC: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpDBD=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpDA2;if(_tmpDBD->tag != 37)goto _LL6AE;else{_tmpDBE=(void*)_tmpDBD->f1;}}_LL6AD:
# 4410
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpDBE,1);
# 4411
goto _LL693;_LL6AE: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpDBF=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpDA2;if(_tmpDBF->tag != 17)goto _LL6B0;else{_tmpDC0=_tmpDBF->f1;}}_LL6AF:
# 4413
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDC0,te,cvtenv);
# 4414
goto _LL693;_LL6B0:;_LL6B1: {
# 4416
const char*_tmp1565;void*_tmp1564;(_tmp1564=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1565="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",_tag_dyneither(_tmp1565,sizeof(char),66))),_tag_dyneither(_tmp1564,sizeof(void*),0)));}_LL693:;}
# 4418
return cvtenv;}
# 4421
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){
# 4426
struct Cyc_List_List*_tmpDC3=cvt.kind_env;
# 4427
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);
# 4429
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
# 4430
struct Cyc_Absyn_Tvar*_tmpDC5;struct _tuple28 _tmpDC4=*((struct _tuple28*)vs->hd);_tmpDC5=_tmpDC4.f1;
# 4431
cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmpDC3,_tmpDC5);}}
# 4437
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
# 4438
void*_tmpDC7;struct _tuple29 _tmpDC6=*((struct _tuple29*)evs->hd);_tmpDC7=_tmpDC6.f1;{
# 4439
void*_tmpDC8=Cyc_Tcutil_compress(_tmpDC7);struct Cyc_Core_Opt*_tmpDCA;struct Cyc_Core_Opt**_tmpDCB;_LL6B3: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpDC9=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpDC8;if(_tmpDC9->tag != 1)goto _LL6B5;else{_tmpDCA=_tmpDC9->f4;_tmpDCB=(struct Cyc_Core_Opt**)& _tmpDC9->f4;}}_LL6B4:
# 4441
 if(*_tmpDCB == 0){
# 4442
struct Cyc_Core_Opt*_tmp1566;*_tmpDCB=((_tmp1566=_cycalloc(sizeof(*_tmp1566)),((_tmp1566->v=_tmpDC3,_tmp1566))));}else{
# 4445
struct Cyc_List_List*_tmpDCD=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmpDCB))->v;
# 4446
struct Cyc_List_List*_tmpDCE=0;
# 4447
for(0;_tmpDCD != 0;_tmpDCD=_tmpDCD->tl){
# 4448
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmpDC3,(struct Cyc_Absyn_Tvar*)_tmpDCD->hd)){
# 4449
struct Cyc_List_List*_tmp1567;_tmpDCE=((_tmp1567=_cycalloc(sizeof(*_tmp1567)),((_tmp1567->hd=(struct Cyc_Absyn_Tvar*)_tmpDCD->hd,((_tmp1567->tl=_tmpDCE,_tmp1567))))));}}{
# 4450
struct Cyc_Core_Opt*_tmp1568;*_tmpDCB=((_tmp1568=_cycalloc(sizeof(*_tmp1568)),((_tmp1568->v=_tmpDCE,_tmp1568))));};}
# 4452
goto _LL6B2;_LL6B5:;_LL6B6:
# 4453
 goto _LL6B2;_LL6B2:;};}}
# 4456
return cvt;}
# 4463
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
# 4464
int generalize_evars=Cyc_Tcutil_is_function_type(t);
# 4465
struct Cyc_List_List*_tmpDD1=Cyc_Tcenv_lookup_type_vars(te);
# 4466
struct _RegionHandle _tmpDD2=_new_region("temp");struct _RegionHandle*temp=& _tmpDD2;_push_region(temp);
# 4467
{struct Cyc_Tcutil_CVTEnv _tmp1569;struct Cyc_Tcutil_CVTEnv _tmpDD3=Cyc_Tcutil_check_valid_type(loc,te,(
# 4468
(_tmp1569.r=temp,((_tmp1569.kind_env=_tmpDD1,((_tmp1569.free_vars=0,((_tmp1569.free_evars=0,((_tmp1569.generalize_evars=generalize_evars,((_tmp1569.fn_result=0,_tmp1569)))))))))))),& Cyc_Tcutil_tmk,t);
# 4471
struct Cyc_List_List*_tmpDD4=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpDD3.free_vars);
# 4472
struct Cyc_List_List*_tmpDD5=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpDD3.free_evars);
# 4475
if(_tmpDD1 != 0){
# 4476
struct Cyc_List_List*_tmpDD6=0;
# 4477
{struct Cyc_List_List*_tmpDD7=_tmpDD4;for(0;(unsigned int)_tmpDD7;_tmpDD7=_tmpDD7->tl){
# 4478
struct Cyc_Absyn_Tvar*_tmpDD8=(struct Cyc_Absyn_Tvar*)_tmpDD7->hd;
# 4479
int found=0;
# 4480
{struct Cyc_List_List*_tmpDD9=_tmpDD1;for(0;(unsigned int)_tmpDD9;_tmpDD9=_tmpDD9->tl){
# 4481
if(Cyc_Absyn_tvar_cmp(_tmpDD8,(struct Cyc_Absyn_Tvar*)_tmpDD9->hd)== 0){found=1;break;}}}
# 4482
if(!found){
# 4483
struct Cyc_List_List*_tmp156A;_tmpDD6=((_tmp156A=_region_malloc(temp,sizeof(*_tmp156A)),((_tmp156A->hd=(struct Cyc_Absyn_Tvar*)_tmpDD7->hd,((_tmp156A->tl=_tmpDD6,_tmp156A))))));}}}
# 4485
_tmpDD4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpDD6);}
# 4491
{struct Cyc_List_List*x=_tmpDD4;for(0;x != 0;x=x->tl){
# 4492
void*_tmpDDB=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmpDDD;struct Cyc_Core_Opt**_tmpDDE;struct Cyc_Core_Opt*_tmpDE0;struct Cyc_Core_Opt**_tmpDE1;struct Cyc_Absyn_Kind*_tmpDE2;struct Cyc_Absyn_Kind _tmpDE3;enum Cyc_Absyn_KindQual _tmpDE4;enum Cyc_Absyn_AliasQual _tmpDE5;struct Cyc_Core_Opt*_tmpDE7;struct Cyc_Core_Opt**_tmpDE8;struct Cyc_Absyn_Kind*_tmpDE9;struct Cyc_Absyn_Kind _tmpDEA;enum Cyc_Absyn_KindQual _tmpDEB;enum Cyc_Absyn_AliasQual _tmpDEC;struct Cyc_Core_Opt*_tmpDEE;struct Cyc_Core_Opt**_tmpDEF;struct Cyc_Absyn_Kind*_tmpDF0;struct Cyc_Absyn_Kind _tmpDF1;enum Cyc_Absyn_KindQual _tmpDF2;enum Cyc_Absyn_AliasQual _tmpDF3;struct Cyc_Core_Opt*_tmpDF5;struct Cyc_Core_Opt**_tmpDF6;struct Cyc_Absyn_Kind*_tmpDF7;struct Cyc_Absyn_Kind _tmpDF8;enum Cyc_Absyn_KindQual _tmpDF9;enum Cyc_Absyn_AliasQual _tmpDFA;struct Cyc_Core_Opt*_tmpDFC;struct Cyc_Core_Opt**_tmpDFD;struct Cyc_Absyn_Kind*_tmpDFE;struct Cyc_Absyn_Kind _tmpDFF;enum Cyc_Absyn_KindQual _tmpE00;enum Cyc_Absyn_AliasQual _tmpE01;struct Cyc_Core_Opt*_tmpE03;struct Cyc_Core_Opt**_tmpE04;struct Cyc_Absyn_Kind*_tmpE05;struct Cyc_Absyn_Kind*_tmpE07;struct Cyc_Absyn_Kind _tmpE08;enum Cyc_Absyn_KindQual _tmpE09;enum Cyc_Absyn_AliasQual _tmpE0A;_LL6B8: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpDDC=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpDDB;if(_tmpDDC->tag != 1)goto _LL6BA;else{_tmpDDD=_tmpDDC->f1;_tmpDDE=(struct Cyc_Core_Opt**)& _tmpDDC->f1;}}_LL6B9:
# 4493
 _tmpDE1=_tmpDDE;goto _LL6BB;_LL6BA: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDDF=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDDB;if(_tmpDDF->tag != 2)goto _LL6BC;else{_tmpDE0=_tmpDDF->f1;_tmpDE1=(struct Cyc_Core_Opt**)& _tmpDDF->f1;_tmpDE2=_tmpDDF->f2;_tmpDE3=*_tmpDE2;_tmpDE4=_tmpDE3.kind;if(_tmpDE4 != Cyc_Absyn_BoxKind)goto _LL6BC;_tmpDE5=_tmpDE3.aliasqual;if(_tmpDE5 != Cyc_Absyn_Top)goto _LL6BC;}}_LL6BB:
# 4494
 _tmpDE8=_tmpDE1;goto _LL6BD;_LL6BC: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDE6=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDDB;if(_tmpDE6->tag != 2)goto _LL6BE;else{_tmpDE7=_tmpDE6->f1;_tmpDE8=(struct Cyc_Core_Opt**)& _tmpDE6->f1;_tmpDE9=_tmpDE6->f2;_tmpDEA=*_tmpDE9;_tmpDEB=_tmpDEA.kind;if(_tmpDEB != Cyc_Absyn_MemKind)goto _LL6BE;_tmpDEC=_tmpDEA.aliasqual;if(_tmpDEC != Cyc_Absyn_Top)goto _LL6BE;}}_LL6BD:
# 4495
 _tmpDEF=_tmpDE8;goto _LL6BF;_LL6BE: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDED=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDDB;if(_tmpDED->tag != 2)goto _LL6C0;else{_tmpDEE=_tmpDED->f1;_tmpDEF=(struct Cyc_Core_Opt**)& _tmpDED->f1;_tmpDF0=_tmpDED->f2;_tmpDF1=*_tmpDF0;_tmpDF2=_tmpDF1.kind;if(_tmpDF2 != Cyc_Absyn_MemKind)goto _LL6C0;_tmpDF3=_tmpDF1.aliasqual;if(_tmpDF3 != Cyc_Absyn_Aliasable)goto _LL6C0;}}_LL6BF:
# 4497
*_tmpDEF=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6B7;_LL6C0: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDF4=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDDB;if(_tmpDF4->tag != 2)goto _LL6C2;else{_tmpDF5=_tmpDF4->f1;_tmpDF6=(struct Cyc_Core_Opt**)& _tmpDF4->f1;_tmpDF7=_tmpDF4->f2;_tmpDF8=*_tmpDF7;_tmpDF9=_tmpDF8.kind;if(_tmpDF9 != Cyc_Absyn_MemKind)goto _LL6C2;_tmpDFA=_tmpDF8.aliasqual;if(_tmpDFA != Cyc_Absyn_Unique)goto _LL6C2;}}_LL6C1:
# 4499
*_tmpDF6=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL6B7;_LL6C2: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDFB=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDDB;if(_tmpDFB->tag != 2)goto _LL6C4;else{_tmpDFC=_tmpDFB->f1;_tmpDFD=(struct Cyc_Core_Opt**)& _tmpDFB->f1;_tmpDFE=_tmpDFB->f2;_tmpDFF=*_tmpDFE;_tmpE00=_tmpDFF.kind;if(_tmpE00 != Cyc_Absyn_RgnKind)goto _LL6C4;_tmpE01=_tmpDFF.aliasqual;if(_tmpE01 != Cyc_Absyn_Top)goto _LL6C4;}}_LL6C3:
# 4501
*_tmpDFD=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL6B7;_LL6C4: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpE02=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDDB;if(_tmpE02->tag != 2)goto _LL6C6;else{_tmpE03=_tmpE02->f1;_tmpE04=(struct Cyc_Core_Opt**)& _tmpE02->f1;_tmpE05=_tmpE02->f2;}}_LL6C5:
# 4503
*_tmpE04=Cyc_Tcutil_kind_to_bound_opt(_tmpE05);goto _LL6B7;_LL6C6: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpE06=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpDDB;if(_tmpE06->tag != 0)goto _LL6C8;else{_tmpE07=_tmpE06->f1;_tmpE08=*_tmpE07;_tmpE09=_tmpE08.kind;if(_tmpE09 != Cyc_Absyn_MemKind)goto _LL6C8;_tmpE0A=_tmpE08.aliasqual;}}_LL6C7:
# 4505
{const char*_tmp1571;void*_tmp1570[2];struct Cyc_String_pa_PrintArg_struct _tmp156F;struct Cyc_Absyn_Kind*_tmp156E;struct Cyc_String_pa_PrintArg_struct _tmp156D;(_tmp156D.tag=0,((_tmp156D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4506
Cyc_Absynpp_kind2string(((_tmp156E=_cycalloc_atomic(sizeof(*_tmp156E)),((_tmp156E->kind=Cyc_Absyn_MemKind,((_tmp156E->aliasqual=_tmpE0A,_tmp156E)))))))),((_tmp156F.tag=0,((_tmp156F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd)),((_tmp1570[0]=& _tmp156F,((_tmp1570[1]=& _tmp156D,Cyc_Tcutil_terr(loc,((_tmp1571="type variable %s cannot have kind %s",_tag_dyneither(_tmp1571,sizeof(char),37))),_tag_dyneither(_tmp1570,sizeof(void*),2)))))))))))));}
# 4507
goto _LL6B7;_LL6C8:;_LL6C9:
# 4508
 goto _LL6B7;_LL6B7:;}}
# 4512
if(_tmpDD4 != 0  || _tmpDD5 != 0){
# 4513
{void*_tmpE10=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpE12;struct Cyc_List_List*_tmpE13;struct Cyc_List_List**_tmpE14;void*_tmpE15;struct Cyc_Absyn_Tqual _tmpE16;void*_tmpE17;struct Cyc_List_List*_tmpE18;int _tmpE19;struct Cyc_Absyn_VarargInfo*_tmpE1A;struct Cyc_List_List*_tmpE1B;struct Cyc_List_List*_tmpE1C;_LL6CB: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE11=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE10;if(_tmpE11->tag != 9)goto _LL6CD;else{_tmpE12=_tmpE11->f1;_tmpE13=_tmpE12.tvars;_tmpE14=(struct Cyc_List_List**)&(_tmpE11->f1).tvars;_tmpE15=_tmpE12.effect;_tmpE16=_tmpE12.ret_tqual;_tmpE17=_tmpE12.ret_typ;_tmpE18=_tmpE12.args;_tmpE19=_tmpE12.c_varargs;_tmpE1A=_tmpE12.cyc_varargs;_tmpE1B=_tmpE12.rgn_po;_tmpE1C=_tmpE12.attributes;}}_LL6CC:
# 4515
 if(*_tmpE14 == 0){
# 4517
*_tmpE14=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmpDD4);
# 4518
_tmpDD4=0;}
# 4520
goto _LL6CA;_LL6CD:;_LL6CE:
# 4521
 goto _LL6CA;_LL6CA:;}
# 4523
if(_tmpDD4 != 0){
# 4524
const char*_tmp1575;void*_tmp1574[1];struct Cyc_String_pa_PrintArg_struct _tmp1573;(_tmp1573.tag=0,((_tmp1573.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpDD4->hd)->name),((_tmp1574[0]=& _tmp1573,Cyc_Tcutil_terr(loc,((_tmp1575="unbound type variable %s",_tag_dyneither(_tmp1575,sizeof(char),25))),_tag_dyneither(_tmp1574,sizeof(void*),1)))))));}
# 4528
if(_tmpDD5 != 0)
# 4529
for(0;_tmpDD5 != 0;_tmpDD5=_tmpDD5->tl){
# 4530
void*e=(void*)_tmpDD5->hd;
# 4531
struct Cyc_Absyn_Kind*_tmpE20=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind _tmpE21;enum Cyc_Absyn_KindQual _tmpE22;enum Cyc_Absyn_AliasQual _tmpE23;struct Cyc_Absyn_Kind _tmpE24;enum Cyc_Absyn_KindQual _tmpE25;enum Cyc_Absyn_AliasQual _tmpE26;struct Cyc_Absyn_Kind _tmpE27;enum Cyc_Absyn_KindQual _tmpE28;enum Cyc_Absyn_AliasQual _tmpE29;struct Cyc_Absyn_Kind _tmpE2A;enum Cyc_Absyn_KindQual _tmpE2B;_LL6D0: _tmpE21=*_tmpE20;_tmpE22=_tmpE21.kind;if(_tmpE22 != Cyc_Absyn_RgnKind)goto _LL6D2;_tmpE23=_tmpE21.aliasqual;if(_tmpE23 != Cyc_Absyn_Unique)goto _LL6D2;_LL6D1:
# 4533
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
# 4534
const char*_tmp1578;void*_tmp1577;(_tmp1577=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1578="can't unify evar with unique region!",_tag_dyneither(_tmp1578,sizeof(char),37))),_tag_dyneither(_tmp1577,sizeof(void*),0)));}
# 4535
goto _LL6CF;_LL6D2: _tmpE24=*_tmpE20;_tmpE25=_tmpE24.kind;if(_tmpE25 != Cyc_Absyn_RgnKind)goto _LL6D4;_tmpE26=_tmpE24.aliasqual;if(_tmpE26 != Cyc_Absyn_Aliasable)goto _LL6D4;_LL6D3:
# 4536
 goto _LL6D5;_LL6D4: _tmpE27=*_tmpE20;_tmpE28=_tmpE27.kind;if(_tmpE28 != Cyc_Absyn_RgnKind)goto _LL6D6;_tmpE29=_tmpE27.aliasqual;if(_tmpE29 != Cyc_Absyn_Top)goto _LL6D6;_LL6D5:
# 4538
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp157B;void*_tmp157A;(_tmp157A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp157B="can't unify evar with heap!",_tag_dyneither(_tmp157B,sizeof(char),28))),_tag_dyneither(_tmp157A,sizeof(void*),0)));}
# 4539
goto _LL6CF;_LL6D6: _tmpE2A=*_tmpE20;_tmpE2B=_tmpE2A.kind;if(_tmpE2B != Cyc_Absyn_EffKind)goto _LL6D8;_LL6D7:
# 4541
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp157E;void*_tmp157D;(_tmp157D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp157E="can't unify evar with {}!",_tag_dyneither(_tmp157E,sizeof(char),26))),_tag_dyneither(_tmp157D,sizeof(void*),0)));}
# 4542
goto _LL6CF;_LL6D8:;_LL6D9:
# 4544
{const char*_tmp1583;void*_tmp1582[2];struct Cyc_String_pa_PrintArg_struct _tmp1581;struct Cyc_String_pa_PrintArg_struct _tmp1580;(_tmp1580.tag=0,((_tmp1580.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4545
Cyc_Absynpp_typ2string(t)),((_tmp1581.tag=0,((_tmp1581.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp1582[0]=& _tmp1581,((_tmp1582[1]=& _tmp1580,Cyc_Tcutil_terr(loc,((_tmp1583="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp1583,sizeof(char),52))),_tag_dyneither(_tmp1582,sizeof(void*),2)))))))))))));}
# 4546
goto _LL6CF;_LL6CF:;}}}
# 4467
;_pop_region(temp);}
# 4557
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
# 4558
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4560
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
# 4561
{void*_tmpE37=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpE39;struct Cyc_List_List*_tmpE3A;void*_tmpE3B;struct Cyc_Absyn_Tqual _tmpE3C;void*_tmpE3D;struct Cyc_List_List*_tmpE3E;_LL6DB: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE38=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE37;if(_tmpE38->tag != 9)goto _LL6DD;else{_tmpE39=_tmpE38->f1;_tmpE3A=_tmpE39.tvars;_tmpE3B=_tmpE39.effect;_tmpE3C=_tmpE39.ret_tqual;_tmpE3D=_tmpE39.ret_typ;_tmpE3E=_tmpE39.args;}}_LL6DC:
# 4563
 fd->tvs=_tmpE3A;
# 4564
fd->effect=_tmpE3B;
# 4565
{struct Cyc_List_List*_tmpE3F=fd->args;for(0;_tmpE3F != 0;(_tmpE3F=_tmpE3F->tl,_tmpE3E=_tmpE3E->tl)){
# 4567
(*((struct _tuple10*)_tmpE3F->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmpE3E))->hd)).f2;
# 4568
(*((struct _tuple10*)_tmpE3F->hd)).f3=(*((struct _tuple10*)_tmpE3E->hd)).f3;}}
# 4570
fd->ret_tqual=_tmpE3C;
# 4571
fd->ret_type=_tmpE3D;
# 4572
(fd->ret_tqual).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpE3D);
# 4573
goto _LL6DA;_LL6DD:;_LL6DE: {
# 4574
const char*_tmp1586;void*_tmp1585;(_tmp1585=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1586="check_fndecl_valid_type: not a FnType",_tag_dyneither(_tmp1586,sizeof(char),38))),_tag_dyneither(_tmp1585,sizeof(void*),0)));}_LL6DA:;}{
# 4577
struct _RegionHandle _tmpE42=_new_region("r");struct _RegionHandle*r=& _tmpE42;_push_region(r);{
# 4578
const char*_tmp1587;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,(
# 4579
(_tmp1587="function declaration has repeated parameter",_tag_dyneither(_tmp1587,sizeof(char),44))));}
# 4582
fd->cached_typ=(void*)t;
# 4577
;_pop_region(r);};}
# 4587
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){
# 4590
struct _RegionHandle _tmpE44=_new_region("r");struct _RegionHandle*r=& _tmpE44;_push_region(r);
# 4591
{struct Cyc_Tcutil_CVTEnv _tmp1588;struct Cyc_Tcutil_CVTEnv _tmpE45=Cyc_Tcutil_check_valid_type(loc,te,(
# 4592
(_tmp1588.r=r,((_tmp1588.kind_env=bound_tvars,((_tmp1588.free_vars=0,((_tmp1588.free_evars=0,((_tmp1588.generalize_evars=0,((_tmp1588.fn_result=0,_tmp1588)))))))))))),expected_kind,t);
# 4594
struct Cyc_List_List*_tmpE46=Cyc_Tcutil_remove_bound_tvars(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpE45.free_vars),bound_tvars);
# 4596
struct Cyc_List_List*_tmpE47=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpE45.free_evars);
# 4597
{struct Cyc_List_List*fs=_tmpE46;for(0;fs != 0;fs=fs->tl){
# 4598
struct _dyneither_ptr*_tmpE48=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
# 4599
const char*_tmp158D;void*_tmp158C[2];struct Cyc_String_pa_PrintArg_struct _tmp158B;struct Cyc_String_pa_PrintArg_struct _tmp158A;(_tmp158A.tag=0,((_tmp158A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp158B.tag=0,((_tmp158B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE48),((_tmp158C[0]=& _tmp158B,((_tmp158C[1]=& _tmp158A,Cyc_Tcutil_terr(loc,((_tmp158D="unbound type variable %s in type %s",_tag_dyneither(_tmp158D,sizeof(char),36))),_tag_dyneither(_tmp158C,sizeof(void*),2)))))))))))));}}
# 4601
if(!allow_evars  && _tmpE47 != 0)
# 4602
for(0;_tmpE47 != 0;_tmpE47=_tmpE47->tl){
# 4603
void*e=(void*)_tmpE47->hd;
# 4604
struct Cyc_Absyn_Kind*_tmpE4D=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind _tmpE4E;enum Cyc_Absyn_KindQual _tmpE4F;enum Cyc_Absyn_AliasQual _tmpE50;struct Cyc_Absyn_Kind _tmpE51;enum Cyc_Absyn_KindQual _tmpE52;enum Cyc_Absyn_AliasQual _tmpE53;struct Cyc_Absyn_Kind _tmpE54;enum Cyc_Absyn_KindQual _tmpE55;enum Cyc_Absyn_AliasQual _tmpE56;struct Cyc_Absyn_Kind _tmpE57;enum Cyc_Absyn_KindQual _tmpE58;_LL6E0: _tmpE4E=*_tmpE4D;_tmpE4F=_tmpE4E.kind;if(_tmpE4F != Cyc_Absyn_RgnKind)goto _LL6E2;_tmpE50=_tmpE4E.aliasqual;if(_tmpE50 != Cyc_Absyn_Unique)goto _LL6E2;_LL6E1:
# 4606
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
# 4607
const char*_tmp1590;void*_tmp158F;(_tmp158F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1590="can't unify evar with unique region!",_tag_dyneither(_tmp1590,sizeof(char),37))),_tag_dyneither(_tmp158F,sizeof(void*),0)));}
# 4608
goto _LL6DF;_LL6E2: _tmpE51=*_tmpE4D;_tmpE52=_tmpE51.kind;if(_tmpE52 != Cyc_Absyn_RgnKind)goto _LL6E4;_tmpE53=_tmpE51.aliasqual;if(_tmpE53 != Cyc_Absyn_Aliasable)goto _LL6E4;_LL6E3:
# 4609
 goto _LL6E5;_LL6E4: _tmpE54=*_tmpE4D;_tmpE55=_tmpE54.kind;if(_tmpE55 != Cyc_Absyn_RgnKind)goto _LL6E6;_tmpE56=_tmpE54.aliasqual;if(_tmpE56 != Cyc_Absyn_Top)goto _LL6E6;_LL6E5:
# 4611
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp1593;void*_tmp1592;(_tmp1592=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1593="can't unify evar with heap!",_tag_dyneither(_tmp1593,sizeof(char),28))),_tag_dyneither(_tmp1592,sizeof(void*),0)));}
# 4612
goto _LL6DF;_LL6E6: _tmpE57=*_tmpE4D;_tmpE58=_tmpE57.kind;if(_tmpE58 != Cyc_Absyn_EffKind)goto _LL6E8;_LL6E7:
# 4614
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp1596;void*_tmp1595;(_tmp1595=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1596="can't unify evar with {}!",_tag_dyneither(_tmp1596,sizeof(char),26))),_tag_dyneither(_tmp1595,sizeof(void*),0)));}
# 4615
goto _LL6DF;_LL6E8:;_LL6E9:
# 4617
{const char*_tmp159B;void*_tmp159A[2];struct Cyc_String_pa_PrintArg_struct _tmp1599;struct Cyc_String_pa_PrintArg_struct _tmp1598;(_tmp1598.tag=0,((_tmp1598.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4618
Cyc_Absynpp_typ2string(t)),((_tmp1599.tag=0,((_tmp1599.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp159A[0]=& _tmp1599,((_tmp159A[1]=& _tmp1598,Cyc_Tcutil_terr(loc,((_tmp159B="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp159B,sizeof(char),52))),_tag_dyneither(_tmp159A,sizeof(void*),2)))))))))))));}
# 4619
goto _LL6DF;_LL6DF:;}}
# 4591
;_pop_region(r);}
# 4626
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
# 4627
if(tv->identity == - 1)
# 4628
tv->identity=Cyc_Tcutil_new_tvar_id();}
# 4631
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
# 4632
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4637
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4640
for(0;vs != 0;vs=vs->tl){
# 4641
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
# 4642
if(cmp((void*)vs->hd,(void*)vs2->hd)== 0){
# 4643
const char*_tmp15A0;void*_tmp159F[2];struct Cyc_String_pa_PrintArg_struct _tmp159E;struct Cyc_String_pa_PrintArg_struct _tmp159D;(_tmp159D.tag=0,((_tmp159D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd)),((_tmp159E.tag=0,((_tmp159E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp159F[0]=& _tmp159E,((_tmp159F[1]=& _tmp159D,Cyc_Tcutil_terr(loc,((_tmp15A0="%s: %s",_tag_dyneither(_tmp15A0,sizeof(char),7))),_tag_dyneither(_tmp159F,sizeof(void*),2)))))))))))));}}}}
# 4646
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
# 4647
return*s;}
# 4650
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
# 4651
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4654
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
# 4655
const char*_tmp15A1;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp15A1="duplicate type variable",_tag_dyneither(_tmp15A1,sizeof(char),24))));}struct _tuple31{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple32{struct Cyc_List_List*f1;void*f2;};struct _tuple33{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4668 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 4673
struct _RegionHandle _tmpE69=_new_region("temp");struct _RegionHandle*temp=& _tmpE69;_push_region(temp);
# 4677
{struct Cyc_List_List*fields=0;
# 4678
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
# 4679
const char*_tmp15A2;if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp15A2="",_tag_dyneither(_tmp15A2,sizeof(char),1))))!= 0){
# 4680
struct _tuple31*_tmp15A5;struct Cyc_List_List*_tmp15A4;fields=((_tmp15A4=_region_malloc(temp,sizeof(*_tmp15A4)),((_tmp15A4->hd=((_tmp15A5=_region_malloc(temp,sizeof(*_tmp15A5)),((_tmp15A5->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp15A5->f2=0,_tmp15A5)))))),((_tmp15A4->tl=fields,_tmp15A4))))));}}}
# 4682
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 4684
const char*_tmp15A7;const char*_tmp15A6;struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp15A7="struct",_tag_dyneither(_tmp15A7,sizeof(char),7)):((_tmp15A6="union",_tag_dyneither(_tmp15A6,sizeof(char),6)));
# 4687
struct Cyc_List_List*ans=0;
# 4688
for(0;des != 0;des=des->tl){
# 4689
struct _tuple32 _tmpE6E;struct Cyc_List_List*_tmpE6F;void*_tmpE70;struct _tuple32*_tmpE6D=(struct _tuple32*)des->hd;_tmpE6E=*_tmpE6D;_tmpE6F=_tmpE6E.f1;_tmpE70=_tmpE6E.f2;
# 4690
if(_tmpE6F == 0){
# 4692
struct Cyc_List_List*_tmpE71=fields;
# 4693
for(0;_tmpE71 != 0;_tmpE71=_tmpE71->tl){
# 4694
if(!(*((struct _tuple31*)_tmpE71->hd)).f2){
# 4695
(*((struct _tuple31*)_tmpE71->hd)).f2=1;
# 4696
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp15AD;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp15AC;struct Cyc_List_List*_tmp15AB;(*((struct _tuple32*)des->hd)).f1=(struct Cyc_List_List*)((_tmp15AB=_cycalloc(sizeof(*_tmp15AB)),((_tmp15AB->hd=(void*)((_tmp15AD=_cycalloc(sizeof(*_tmp15AD)),((_tmp15AD[0]=((_tmp15AC.tag=1,((_tmp15AC.f1=((*((struct _tuple31*)_tmpE71->hd)).f1)->name,_tmp15AC)))),_tmp15AD)))),((_tmp15AB->tl=0,_tmp15AB))))));}
# 4697
{struct _tuple33*_tmp15B0;struct Cyc_List_List*_tmp15AF;ans=((_tmp15AF=_region_malloc(rgn,sizeof(*_tmp15AF)),((_tmp15AF->hd=((_tmp15B0=_region_malloc(rgn,sizeof(*_tmp15B0)),((_tmp15B0->f1=(*((struct _tuple31*)_tmpE71->hd)).f1,((_tmp15B0->f2=_tmpE70,_tmp15B0)))))),((_tmp15AF->tl=ans,_tmp15AF))))));}
# 4698
break;}}
# 4700
if(_tmpE71 == 0){
# 4701
const char*_tmp15B4;void*_tmp15B3[1];struct Cyc_String_pa_PrintArg_struct _tmp15B2;(_tmp15B2.tag=0,((_tmp15B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp15B3[0]=& _tmp15B2,Cyc_Tcutil_terr(loc,((_tmp15B4="too many arguments to %s",_tag_dyneither(_tmp15B4,sizeof(char),25))),_tag_dyneither(_tmp15B3,sizeof(void*),1)))))));}}else{
# 4702
if(_tmpE6F->tl != 0){
# 4704
const char*_tmp15B7;void*_tmp15B6;(_tmp15B6=0,Cyc_Tcutil_terr(loc,((_tmp15B7="multiple designators are not yet supported",_tag_dyneither(_tmp15B7,sizeof(char),43))),_tag_dyneither(_tmp15B6,sizeof(void*),0)));}else{
# 4707
void*_tmpE7C=(void*)_tmpE6F->hd;struct _dyneither_ptr*_tmpE7F;_LL6EB: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpE7D=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpE7C;if(_tmpE7D->tag != 0)goto _LL6ED;}_LL6EC:
# 4709
{const char*_tmp15BB;void*_tmp15BA[1];struct Cyc_String_pa_PrintArg_struct _tmp15B9;(_tmp15B9.tag=0,((_tmp15B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp15BA[0]=& _tmp15B9,Cyc_Tcutil_terr(loc,((_tmp15BB="array designator used in argument to %s",_tag_dyneither(_tmp15BB,sizeof(char),40))),_tag_dyneither(_tmp15BA,sizeof(void*),1)))))));}
# 4710
goto _LL6EA;_LL6ED: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpE7E=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpE7C;if(_tmpE7E->tag != 1)goto _LL6EA;else{_tmpE7F=_tmpE7E->f1;}}_LL6EE: {
# 4712
struct Cyc_List_List*_tmpE83=fields;
# 4713
for(0;_tmpE83 != 0;_tmpE83=_tmpE83->tl){
# 4714
if(Cyc_strptrcmp(_tmpE7F,((*((struct _tuple31*)_tmpE83->hd)).f1)->name)== 0){
# 4715
if((*((struct _tuple31*)_tmpE83->hd)).f2){
# 4716
const char*_tmp15BF;void*_tmp15BE[1];struct Cyc_String_pa_PrintArg_struct _tmp15BD;(_tmp15BD.tag=0,((_tmp15BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE7F),((_tmp15BE[0]=& _tmp15BD,Cyc_Tcutil_terr(loc,((_tmp15BF="member %s has already been used as an argument",_tag_dyneither(_tmp15BF,sizeof(char),47))),_tag_dyneither(_tmp15BE,sizeof(void*),1)))))));}
# 4717
(*((struct _tuple31*)_tmpE83->hd)).f2=1;
# 4718
{struct _tuple33*_tmp15C2;struct Cyc_List_List*_tmp15C1;ans=((_tmp15C1=_region_malloc(rgn,sizeof(*_tmp15C1)),((_tmp15C1->hd=((_tmp15C2=_region_malloc(rgn,sizeof(*_tmp15C2)),((_tmp15C2->f1=(*((struct _tuple31*)_tmpE83->hd)).f1,((_tmp15C2->f2=_tmpE70,_tmp15C2)))))),((_tmp15C1->tl=ans,_tmp15C1))))));}
# 4719
break;}}
# 4721
if(_tmpE83 == 0){
# 4722
const char*_tmp15C6;void*_tmp15C5[1];struct Cyc_String_pa_PrintArg_struct _tmp15C4;(_tmp15C4.tag=0,((_tmp15C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE7F),((_tmp15C5[0]=& _tmp15C4,Cyc_Tcutil_terr(loc,((_tmp15C6="bad field designator %s",_tag_dyneither(_tmp15C6,sizeof(char),24))),_tag_dyneither(_tmp15C5,sizeof(void*),1)))))));}
# 4723
goto _LL6EA;}_LL6EA:;}}}
# 4726
if(aggr_kind == Cyc_Absyn_StructA)
# 4728
for(0;fields != 0;fields=fields->tl){
# 4729
if(!(*((struct _tuple31*)fields->hd)).f2){
# 4730
{const char*_tmp15C9;void*_tmp15C8;(_tmp15C8=0,Cyc_Tcutil_terr(loc,((_tmp15C9="too few arguments to struct",_tag_dyneither(_tmp15C9,sizeof(char),28))),_tag_dyneither(_tmp15C8,sizeof(void*),0)));}
# 4731
break;}}else{
# 4735
int found=0;
# 4736
for(0;fields != 0;fields=fields->tl){
# 4737
if((*((struct _tuple31*)fields->hd)).f2){
# 4738
if(found){const char*_tmp15CC;void*_tmp15CB;(_tmp15CB=0,Cyc_Tcutil_terr(loc,((_tmp15CC="only one member of a union is allowed",_tag_dyneither(_tmp15CC,sizeof(char),38))),_tag_dyneither(_tmp15CB,sizeof(void*),0)));}
# 4739
found=1;}}
# 4742
if(!found){const char*_tmp15CF;void*_tmp15CE;(_tmp15CE=0,Cyc_Tcutil_terr(loc,((_tmp15CF="missing member for union",_tag_dyneither(_tmp15CF,sizeof(char),25))),_tag_dyneither(_tmp15CE,sizeof(void*),0)));}}{
# 4745
struct Cyc_List_List*_tmpE92=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmpE92;};};}
# 4677
;_pop_region(temp);}
# 4748
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
# 4749
void*_tmpE95=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpE97;void*_tmpE98;struct Cyc_Absyn_PtrAtts _tmpE99;union Cyc_Absyn_Constraint*_tmpE9A;_LL6F0: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE96=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE95;if(_tmpE96->tag != 5)goto _LL6F2;else{_tmpE97=_tmpE96->f1;_tmpE98=_tmpE97.elt_typ;_tmpE99=_tmpE97.ptr_atts;_tmpE9A=_tmpE99.bounds;}}_LL6F1: {
# 4751
void*_tmpE9B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpE9A);_LL6F5: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpE9C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpE9B;if(_tmpE9C->tag != 0)goto _LL6F7;}_LL6F6:
# 4753
*elt_typ_dest=_tmpE98;
# 4754
return 1;_LL6F7:;_LL6F8:
# 4755
 return 0;_LL6F4:;}_LL6F2:;_LL6F3:
# 4757
 return 0;_LL6EF:;}
# 4761
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
# 4762
void*_tmpE9D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpE9F;void*_tmpEA0;struct Cyc_Absyn_PtrAtts _tmpEA1;union Cyc_Absyn_Constraint*_tmpEA2;_LL6FA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE9E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE9D;if(_tmpE9E->tag != 5)goto _LL6FC;else{_tmpE9F=_tmpE9E->f1;_tmpEA0=_tmpE9F.elt_typ;_tmpEA1=_tmpE9F.ptr_atts;_tmpEA2=_tmpEA1.zero_term;}}_LL6FB:
# 4764
*elt_typ_dest=_tmpEA0;
# 4765
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEA2);_LL6FC:;_LL6FD:
# 4766
 return 0;_LL6F9:;}
# 4772
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 4774
void*_tmpEA3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpEA5;void*_tmpEA6;struct Cyc_Absyn_PtrAtts _tmpEA7;union Cyc_Absyn_Constraint*_tmpEA8;union Cyc_Absyn_Constraint*_tmpEA9;struct Cyc_Absyn_ArrayInfo _tmpEAB;void*_tmpEAC;struct Cyc_Absyn_Tqual _tmpEAD;struct Cyc_Absyn_Exp*_tmpEAE;union Cyc_Absyn_Constraint*_tmpEAF;_LL6FF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpEA4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEA3;if(_tmpEA4->tag != 5)goto _LL701;else{_tmpEA5=_tmpEA4->f1;_tmpEA6=_tmpEA5.elt_typ;_tmpEA7=_tmpEA5.ptr_atts;_tmpEA8=_tmpEA7.bounds;_tmpEA9=_tmpEA7.zero_term;}}_LL700:
# 4776
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEA9)){
# 4777
*ptr_type=t;
# 4778
*elt_type=_tmpEA6;
# 4779
{void*_tmpEB0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpEA8);_LL706: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpEB1=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpEB0;if(_tmpEB1->tag != 0)goto _LL708;}_LL707:
# 4780
*is_dyneither=1;goto _LL705;_LL708:;_LL709:
# 4781
*is_dyneither=0;goto _LL705;_LL705:;}
# 4783
return 1;}else{
# 4784
return 0;}_LL701: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpEAA=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpEA3;if(_tmpEAA->tag != 8)goto _LL703;else{_tmpEAB=_tmpEAA->f1;_tmpEAC=_tmpEAB.elt_type;_tmpEAD=_tmpEAB.tq;_tmpEAE=_tmpEAB.num_elts;_tmpEAF=_tmpEAB.zero_term;}}_LL702:
# 4786
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEAF)){
# 4787
*elt_type=_tmpEAC;
# 4788
*is_dyneither=0;
# 4789
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp15E4;struct Cyc_Absyn_PtrAtts _tmp15E3;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp15E2;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp15E1;struct Cyc_Absyn_PtrInfo _tmp15E0;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp15DF;*ptr_type=(void*)(
# 4790
(_tmp15DF=_cycalloc(sizeof(*_tmp15DF)),((_tmp15DF[0]=((_tmp15E4.tag=5,((_tmp15E4.f1=((_tmp15E0.elt_typ=_tmpEAC,((_tmp15E0.elt_tq=_tmpEAD,((_tmp15E0.ptr_atts=(
# 4791
(_tmp15E3.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((_tmp15E3.nullable=Cyc_Absyn_false_conref,((_tmp15E3.bounds=
# 4792
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp15E1=_cycalloc(sizeof(*_tmp15E1)),((_tmp15E1[0]=((_tmp15E2.tag=1,((_tmp15E2.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpEAE),_tmp15E2)))),_tmp15E1))))),((_tmp15E3.zero_term=_tmpEAF,((_tmp15E3.ptrloc=0,_tmp15E3)))))))))),_tmp15E0)))))),_tmp15E4)))),_tmp15DF))));}
# 4794
return 1;}else{
# 4795
return 0;}_LL703:;_LL704:
# 4796
 return 0;_LL6FE:;}
# 4803
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 4805
void*_tmpEB8=e1->r;struct Cyc_Absyn_Exp*_tmpEBB;struct Cyc_Absyn_Exp*_tmpEBD;struct Cyc_Absyn_Exp*_tmpEBF;struct Cyc_Absyn_Exp*_tmpEC1;struct Cyc_Absyn_Exp*_tmpEC3;struct Cyc_Absyn_Exp*_tmpEC5;_LL70B: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpEB9=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpEB8;if(_tmpEB9->tag != 13)goto _LL70D;}_LL70C: {
# 4807
const char*_tmp15E8;void*_tmp15E7[1];struct Cyc_String_pa_PrintArg_struct _tmp15E6;(_tmp15E6.tag=0,((_tmp15E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp15E7[0]=& _tmp15E6,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15E8="we have a cast in a lhs:  %s",_tag_dyneither(_tmp15E8,sizeof(char),29))),_tag_dyneither(_tmp15E7,sizeof(void*),1)))))));}_LL70D: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpEBA=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpEB8;if(_tmpEBA->tag != 19)goto _LL70F;else{_tmpEBB=_tmpEBA->f1;}}_LL70E:
# 4808
 _tmpEBD=_tmpEBB;goto _LL710;_LL70F: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpEBC=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpEB8;if(_tmpEBC->tag != 22)goto _LL711;else{_tmpEBD=_tmpEBC->f1;}}_LL710:
# 4810
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpEBD->topt),ptr_type,is_dyneither,elt_type);_LL711: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpEBE=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpEB8;if(_tmpEBE->tag != 21)goto _LL713;else{_tmpEBF=_tmpEBE->f1;}}_LL712:
# 4811
 _tmpEC1=_tmpEBF;goto _LL714;_LL713: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpEC0=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpEB8;if(_tmpEC0->tag != 20)goto _LL715;else{_tmpEC1=_tmpEC0->f1;}}_LL714:
# 4814
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpEC1->topt),ptr_type,is_dyneither,elt_type)){
# 4815
const char*_tmp15EC;void*_tmp15EB[1];struct Cyc_String_pa_PrintArg_struct _tmp15EA;(_tmp15EA.tag=0,((_tmp15EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4816
Cyc_Absynpp_exp2string(e1)),((_tmp15EB[0]=& _tmp15EA,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15EC="found zero pointer aggregate member assignment: %s",_tag_dyneither(_tmp15EC,sizeof(char),51))),_tag_dyneither(_tmp15EB,sizeof(void*),1)))))));}
# 4817
return 0;_LL715: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpEC2=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpEB8;if(_tmpEC2->tag != 12)goto _LL717;else{_tmpEC3=_tmpEC2->f1;}}_LL716:
# 4818
 _tmpEC5=_tmpEC3;goto _LL718;_LL717: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpEC4=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpEB8;if(_tmpEC4->tag != 11)goto _LL719;else{_tmpEC5=_tmpEC4->f1;}}_LL718:
# 4820
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpEC5->topt),ptr_type,is_dyneither,elt_type)){
# 4821
const char*_tmp15F0;void*_tmp15EF[1];struct Cyc_String_pa_PrintArg_struct _tmp15EE;(_tmp15EE.tag=0,((_tmp15EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4822
Cyc_Absynpp_exp2string(e1)),((_tmp15EF[0]=& _tmp15EE,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15F0="found zero pointer instantiate/noinstantiate: %s",_tag_dyneither(_tmp15F0,sizeof(char),49))),_tag_dyneither(_tmp15EF,sizeof(void*),1)))))));}
# 4823
return 0;_LL719: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpEC6=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpEB8;if(_tmpEC6->tag != 1)goto _LL71B;}_LL71A:
# 4824
 return 0;_LL71B:;_LL71C: {
# 4826
const char*_tmp15F4;void*_tmp15F3[1];struct Cyc_String_pa_PrintArg_struct _tmp15F2;(_tmp15F2.tag=0,((_tmp15F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp15F3[0]=& _tmp15F2,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15F4="found bad lhs in is_zero_ptr_deref: %s",_tag_dyneither(_tmp15F4,sizeof(char),39))),_tag_dyneither(_tmp15F3,sizeof(void*),1)))))));}_LL70A:;}
# 4830
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
# 4831
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
# 4832
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 4841
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 4842
void*_tmpED3=Cyc_Tcutil_compress(r);struct Cyc_Absyn_Tvar*_tmpED7;_LL71E: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpED4=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmpED3;if(_tmpED4->tag != 22)goto _LL720;}_LL71F:
# 4843
 return !must_be_unique;_LL720: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpED5=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmpED3;if(_tmpED5->tag != 21)goto _LL722;}_LL721:
# 4844
 return 1;_LL722: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpED6=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpED3;if(_tmpED6->tag != 2)goto _LL724;else{_tmpED7=_tmpED6->f1;}}_LL723: {
# 4846
struct Cyc_Absyn_Kind _tmpED9;enum Cyc_Absyn_KindQual _tmpEDA;enum Cyc_Absyn_AliasQual _tmpEDB;struct Cyc_Absyn_Kind*_tmpED8=Cyc_Tcutil_tvar_kind(_tmpED7,& Cyc_Tcutil_rk);_tmpED9=*_tmpED8;_tmpEDA=_tmpED9.kind;_tmpEDB=_tmpED9.aliasqual;
# 4847
if(_tmpEDA == Cyc_Absyn_RgnKind  && (_tmpEDB == Cyc_Absyn_Unique  || _tmpEDB == Cyc_Absyn_Top  && !must_be_unique)){
# 4848
void*_tmpEDC=Cyc_Absyn_compress_kb(_tmpED7->kind);struct Cyc_Core_Opt*_tmpEDE;struct Cyc_Core_Opt**_tmpEDF;struct Cyc_Absyn_Kind*_tmpEE0;struct Cyc_Absyn_Kind _tmpEE1;enum Cyc_Absyn_KindQual _tmpEE2;enum Cyc_Absyn_AliasQual _tmpEE3;_LL727: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpEDD=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpEDC;if(_tmpEDD->tag != 2)goto _LL729;else{_tmpEDE=_tmpEDD->f1;_tmpEDF=(struct Cyc_Core_Opt**)& _tmpEDD->f1;_tmpEE0=_tmpEDD->f2;_tmpEE1=*_tmpEE0;_tmpEE2=_tmpEE1.kind;if(_tmpEE2 != Cyc_Absyn_RgnKind)goto _LL729;_tmpEE3=_tmpEE1.aliasqual;if(_tmpEE3 != Cyc_Absyn_Top)goto _LL729;}}_LL728:
# 4850
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp15FA;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp15F9;struct Cyc_Core_Opt*_tmp15F8;*_tmpEDF=((_tmp15F8=_cycalloc(sizeof(*_tmp15F8)),((_tmp15F8->v=(void*)((_tmp15FA=_cycalloc(sizeof(*_tmp15FA)),((_tmp15FA[0]=((_tmp15F9.tag=2,((_tmp15F9.f1=0,((_tmp15F9.f2=& Cyc_Tcutil_rk,_tmp15F9)))))),_tmp15FA)))),_tmp15F8))));}
# 4851
return 0;_LL729:;_LL72A:
# 4852
 return 1;_LL726:;}
# 4855
return 0;}_LL724:;_LL725:
# 4856
 return 0;_LL71D:;}
# 4862
static int Cyc_Tcutil_is_noalias_pointer_aux(void*t,int must_be_unique){
# 4863
void*_tmpEE7=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpEE9;struct Cyc_Absyn_PtrAtts _tmpEEA;void*_tmpEEB;struct Cyc_Absyn_Tvar*_tmpEED;_LL72C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpEE8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEE7;if(_tmpEE8->tag != 5)goto _LL72E;else{_tmpEE9=_tmpEE8->f1;_tmpEEA=_tmpEE9.ptr_atts;_tmpEEB=_tmpEEA.rgn;}}_LL72D:
# 4865
 return Cyc_Tcutil_is_noalias_region(_tmpEEB,must_be_unique);_LL72E: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpEEC=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpEE7;if(_tmpEEC->tag != 2)goto _LL730;else{_tmpEED=_tmpEEC->f1;}}_LL72F: {
# 4867
struct Cyc_Absyn_Kind _tmpEEF;enum Cyc_Absyn_KindQual _tmpEF0;enum Cyc_Absyn_AliasQual _tmpEF1;struct Cyc_Absyn_Kind*_tmpEEE=Cyc_Tcutil_tvar_kind(_tmpEED,& Cyc_Tcutil_bk);_tmpEEF=*_tmpEEE;_tmpEF0=_tmpEEF.kind;_tmpEF1=_tmpEEF.aliasqual;
# 4868
switch(_tmpEF0){case Cyc_Absyn_BoxKind: _LL732:
# 4869
 goto _LL733;case Cyc_Absyn_AnyKind: _LL733: goto _LL734;case Cyc_Absyn_MemKind: _LL734:
# 4870
 if(_tmpEF1 == Cyc_Absyn_Unique  || _tmpEF1 == Cyc_Absyn_Top){
# 4871
void*_tmpEF2=Cyc_Absyn_compress_kb(_tmpEED->kind);struct Cyc_Core_Opt*_tmpEF4;struct Cyc_Core_Opt**_tmpEF5;struct Cyc_Absyn_Kind*_tmpEF6;struct Cyc_Absyn_Kind _tmpEF7;enum Cyc_Absyn_KindQual _tmpEF8;enum Cyc_Absyn_AliasQual _tmpEF9;_LL737: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpEF3=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpEF2;if(_tmpEF3->tag != 2)goto _LL739;else{_tmpEF4=_tmpEF3->f1;_tmpEF5=(struct Cyc_Core_Opt**)& _tmpEF3->f1;_tmpEF6=_tmpEF3->f2;_tmpEF7=*_tmpEF6;_tmpEF8=_tmpEF7.kind;_tmpEF9=_tmpEF7.aliasqual;if(_tmpEF9 != Cyc_Absyn_Top)goto _LL739;}}_LL738:
# 4873
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1604;struct Cyc_Absyn_Kind*_tmp1603;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1602;struct Cyc_Core_Opt*_tmp1601;*_tmpEF5=((_tmp1601=_cycalloc(sizeof(*_tmp1601)),((_tmp1601->v=(void*)((_tmp1604=_cycalloc(sizeof(*_tmp1604)),((_tmp1604[0]=((_tmp1602.tag=2,((_tmp1602.f1=0,((_tmp1602.f2=((_tmp1603=_cycalloc_atomic(sizeof(*_tmp1603)),((_tmp1603->kind=_tmpEF8,((_tmp1603->aliasqual=Cyc_Absyn_Aliasable,_tmp1603)))))),_tmp1602)))))),_tmp1604)))),_tmp1601))));}
# 4874
return 0;_LL739:;_LL73A:
# 4877
 return 1;_LL736:;}
# 4880
return 0;default: _LL735:
# 4881
 return 0;}}_LL730:;_LL731:
# 4883
 return 0;_LL72B:;}
# 4886
int Cyc_Tcutil_is_noalias_pointer(void*t){return Cyc_Tcutil_is_noalias_pointer_aux(t,0);}
# 4888
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
# 4889
void*_tmpEFE=Cyc_Tcutil_compress(t);
# 4890
if(Cyc_Tcutil_is_noalias_pointer(_tmpEFE))return 1;{
# 4891
void*_tmpEFF=_tmpEFE;struct Cyc_List_List*_tmpF01;struct Cyc_Absyn_AggrInfo _tmpF03;union Cyc_Absyn_AggrInfoU _tmpF04;struct Cyc_Absyn_Aggrdecl**_tmpF05;struct Cyc_List_List*_tmpF06;struct Cyc_List_List*_tmpF08;struct Cyc_Absyn_AggrInfo _tmpF0A;union Cyc_Absyn_AggrInfoU _tmpF0B;struct _tuple4 _tmpF0C;struct Cyc_Absyn_DatatypeInfo _tmpF0E;union Cyc_Absyn_DatatypeInfoU _tmpF0F;struct Cyc_List_List*_tmpF10;struct Cyc_Absyn_DatatypeFieldInfo _tmpF12;union Cyc_Absyn_DatatypeFieldInfoU _tmpF13;struct Cyc_List_List*_tmpF14;_LL73D: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpF00=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpEFF;if(_tmpF00->tag != 10)goto _LL73F;else{_tmpF01=_tmpF00->f1;}}_LL73E:
# 4893
 while(_tmpF01 != 0){
# 4894
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpF01->hd)).f2))return 1;
# 4895
_tmpF01=_tmpF01->tl;}
# 4897
return 0;_LL73F: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF02=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpEFF;if(_tmpF02->tag != 11)goto _LL741;else{_tmpF03=_tmpF02->f1;_tmpF04=_tmpF03.aggr_info;if((_tmpF04.KnownAggr).tag != 2)goto _LL741;_tmpF05=(struct Cyc_Absyn_Aggrdecl**)(_tmpF04.KnownAggr).val;_tmpF06=_tmpF03.targs;}}_LL740:
# 4899
 if((*_tmpF05)->impl == 0)return 0;else{
# 4901
struct Cyc_List_List*_tmpF15=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpF05)->tvs,_tmpF06);
# 4902
struct Cyc_List_List*_tmpF16=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpF05)->impl))->fields;
# 4903
void*t;
# 4904
while(_tmpF16 != 0){
# 4905
t=_tmpF15 == 0?((struct Cyc_Absyn_Aggrfield*)_tmpF16->hd)->type: Cyc_Tcutil_rsubstitute(rgn,_tmpF15,((struct Cyc_Absyn_Aggrfield*)_tmpF16->hd)->type);
# 4906
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
# 4907
_tmpF16=_tmpF16->tl;}
# 4909
return 0;}_LL741: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpF07=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpEFF;if(_tmpF07->tag != 12)goto _LL743;else{_tmpF08=_tmpF07->f2;}}_LL742:
# 4912
 while(_tmpF08 != 0){
# 4913
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpF08->hd)->type))return 1;
# 4914
_tmpF08=_tmpF08->tl;}
# 4916
return 0;_LL743: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF09=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpEFF;if(_tmpF09->tag != 11)goto _LL745;else{_tmpF0A=_tmpF09->f1;_tmpF0B=_tmpF0A.aggr_info;if((_tmpF0B.UnknownAggr).tag != 1)goto _LL745;_tmpF0C=(struct _tuple4)(_tmpF0B.UnknownAggr).val;}}_LL744: {
# 4918
const char*_tmp1607;void*_tmp1606;(_tmp1606=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1607="got unknown aggr in is_noalias_aggr",_tag_dyneither(_tmp1607,sizeof(char),36))),_tag_dyneither(_tmp1606,sizeof(void*),0)));}_LL745: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpF0D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpEFF;if(_tmpF0D->tag != 3)goto _LL747;else{_tmpF0E=_tmpF0D->f1;_tmpF0F=_tmpF0E.datatype_info;_tmpF10=_tmpF0E.targs;}}_LL746: {
# 4920
union Cyc_Absyn_DatatypeInfoU _tmpF19=_tmpF0F;struct Cyc_Absyn_UnknownDatatypeInfo _tmpF1A;struct _tuple2*_tmpF1B;int _tmpF1C;struct Cyc_Absyn_Datatypedecl**_tmpF1D;struct Cyc_Absyn_Datatypedecl*_tmpF1E;struct Cyc_Absyn_Datatypedecl _tmpF1F;struct Cyc_List_List*_tmpF20;struct Cyc_Core_Opt*_tmpF21;_LL74C: if((_tmpF19.UnknownDatatype).tag != 1)goto _LL74E;_tmpF1A=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpF19.UnknownDatatype).val;_tmpF1B=_tmpF1A.name;_tmpF1C=_tmpF1A.is_extensible;_LL74D: {
# 4922
const char*_tmp160A;void*_tmp1609;(_tmp1609=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp160A="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp160A,sizeof(char),40))),_tag_dyneither(_tmp1609,sizeof(void*),0)));}_LL74E: if((_tmpF19.KnownDatatype).tag != 2)goto _LL74B;_tmpF1D=(struct Cyc_Absyn_Datatypedecl**)(_tmpF19.KnownDatatype).val;_tmpF1E=*_tmpF1D;_tmpF1F=*_tmpF1E;_tmpF20=_tmpF1F.tvs;_tmpF21=_tmpF1F.fields;_LL74F:
# 4925
 return 0;_LL74B:;}_LL747: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpF11=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpEFF;if(_tmpF11->tag != 4)goto _LL749;else{_tmpF12=_tmpF11->f1;_tmpF13=_tmpF12.field_info;_tmpF14=_tmpF12.targs;}}_LL748: {
# 4928
union Cyc_Absyn_DatatypeFieldInfoU _tmpF24=_tmpF13;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpF25;struct _tuple3 _tmpF26;struct Cyc_Absyn_Datatypedecl*_tmpF27;struct Cyc_Absyn_Datatypefield*_tmpF28;_LL751: if((_tmpF24.UnknownDatatypefield).tag != 1)goto _LL753;_tmpF25=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpF24.UnknownDatatypefield).val;_LL752: {
# 4930
const char*_tmp160D;void*_tmp160C;(_tmp160C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp160D="got unknown datatype field in is_noalias_aggr",_tag_dyneither(_tmp160D,sizeof(char),46))),_tag_dyneither(_tmp160C,sizeof(void*),0)));}_LL753: if((_tmpF24.KnownDatatypefield).tag != 2)goto _LL750;_tmpF26=(struct _tuple3)(_tmpF24.KnownDatatypefield).val;_tmpF27=_tmpF26.f1;_tmpF28=_tmpF26.f2;_LL754: {
# 4932
struct Cyc_List_List*_tmpF2B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpF27->tvs,_tmpF14);
# 4933
struct Cyc_List_List*_tmpF2C=_tmpF28->typs;
# 4934
while(_tmpF2C != 0){
# 4935
_tmpEFE=_tmpF2B == 0?(*((struct _tuple12*)_tmpF2C->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpF2B,(*((struct _tuple12*)_tmpF2C->hd)).f2);
# 4936
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpEFE))return 1;
# 4937
_tmpF2C=_tmpF2C->tl;}
# 4939
return 0;}_LL750:;}_LL749:;_LL74A:
# 4941
 return 0;_LL73C:;};}
# 4945
static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,struct _dyneither_ptr*f){
# 4946
void*_tmpF2D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmpF2F;union Cyc_Absyn_AggrInfoU _tmpF30;struct Cyc_Absyn_Aggrdecl**_tmpF31;struct Cyc_Absyn_Aggrdecl*_tmpF32;struct Cyc_List_List*_tmpF33;struct Cyc_List_List*_tmpF35;_LL756: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF2E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpF2D;if(_tmpF2E->tag != 11)goto _LL758;else{_tmpF2F=_tmpF2E->f1;_tmpF30=_tmpF2F.aggr_info;if((_tmpF30.KnownAggr).tag != 2)goto _LL758;_tmpF31=(struct Cyc_Absyn_Aggrdecl**)(_tmpF30.KnownAggr).val;_tmpF32=*_tmpF31;_tmpF33=_tmpF2F.targs;}}_LL757:
# 4948
 _tmpF35=_tmpF32->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF32->impl))->fields;goto _LL759;_LL758: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpF34=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF2D;if(_tmpF34->tag != 12)goto _LL75A;else{_tmpF35=_tmpF34->f2;}}_LL759: {
# 4950
struct Cyc_Absyn_Aggrfield*_tmpF36=Cyc_Absyn_lookup_field(_tmpF35,f);
# 4951
{struct Cyc_Absyn_Aggrfield*_tmpF37=_tmpF36;struct Cyc_Absyn_Aggrfield _tmpF38;void*_tmpF39;_LL75D: if(_tmpF37 != 0)goto _LL75F;_LL75E: {
# 4952
const char*_tmp1610;void*_tmp160F;(_tmp160F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1610="is_noalias_field: missing field",_tag_dyneither(_tmp1610,sizeof(char),32))),_tag_dyneither(_tmp160F,sizeof(void*),0)));}_LL75F: if(_tmpF37 == 0)goto _LL75C;_tmpF38=*_tmpF37;_tmpF39=_tmpF38.type;_LL760:
# 4954
 return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpF39);_LL75C:;}
# 4956
return 0;}_LL75A:;_LL75B: {
# 4958
const char*_tmp1614;void*_tmp1613[1];struct Cyc_String_pa_PrintArg_struct _tmp1612;(_tmp1612.tag=0,((_tmp1612.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4959
Cyc_Absynpp_typ2string(t)),((_tmp1613[0]=& _tmp1612,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1614="is_noalias_field: invalid type |%s|",_tag_dyneither(_tmp1614,sizeof(char),36))),_tag_dyneither(_tmp1613,sizeof(void*),1)))))));}_LL755:;}
# 4966
static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
# 4967
void*_tmpF3F=e->r;void*_tmpF41;struct Cyc_Absyn_Exp*_tmpF44;struct Cyc_Absyn_Exp*_tmpF46;struct Cyc_Absyn_Exp*_tmpF48;struct _dyneither_ptr*_tmpF49;struct Cyc_Absyn_Exp*_tmpF4B;struct Cyc_Absyn_Exp*_tmpF4C;struct Cyc_Absyn_Exp*_tmpF4E;struct Cyc_Absyn_Exp*_tmpF4F;struct Cyc_Absyn_Exp*_tmpF51;struct Cyc_Absyn_Exp*_tmpF53;struct Cyc_Absyn_Stmt*_tmpF55;_LL762: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpF40=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF3F;if(_tmpF40->tag != 1)goto _LL764;else{_tmpF41=(void*)_tmpF40->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpF42=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpF41;if(_tmpF42->tag != 1)goto _LL764;};}}_LL763:
# 4968
 return 0;_LL764: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpF43=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF3F;if(_tmpF43->tag != 21)goto _LL766;else{_tmpF44=_tmpF43->f1;}}_LL765:
# 4969
 _tmpF46=_tmpF44;goto _LL767;_LL766: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpF45=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpF3F;if(_tmpF45->tag != 19)goto _LL768;else{_tmpF46=_tmpF45->f1;}}_LL767:
# 4971
 return Cyc_Tcutil_is_noalias_pointer_aux((void*)_check_null(_tmpF46->topt),1) && 
# 4972
Cyc_Tcutil_is_noalias_path_aux(r,_tmpF46);_LL768: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpF47=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF3F;if(_tmpF47->tag != 20)goto _LL76A;else{_tmpF48=_tmpF47->f1;_tmpF49=_tmpF47->f2;}}_LL769:
# 4973
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF48);_LL76A: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpF4A=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF3F;if(_tmpF4A->tag != 22)goto _LL76C;else{_tmpF4B=_tmpF4A->f1;_tmpF4C=_tmpF4A->f2;}}_LL76B: {
# 4975
void*_tmpF56=Cyc_Tcutil_compress((void*)_check_null(_tmpF4B->topt));_LL777: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpF57=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpF56;if(_tmpF57->tag != 10)goto _LL779;}_LL778:
# 4976
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF4B);_LL779:;_LL77A:
# 4977
 return 0;_LL776:;}_LL76C: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpF4D=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF3F;if(_tmpF4D->tag != 5)goto _LL76E;else{_tmpF4E=_tmpF4D->f2;_tmpF4F=_tmpF4D->f3;}}_LL76D:
# 4980
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF4E) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpF4F);_LL76E: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpF50=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpF3F;if(_tmpF50->tag != 8)goto _LL770;else{_tmpF51=_tmpF50->f2;}}_LL76F:
# 4981
 _tmpF53=_tmpF51;goto _LL771;_LL770: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpF52=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF3F;if(_tmpF52->tag != 13)goto _LL772;else{_tmpF53=_tmpF52->f2;}}_LL771:
# 4982
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF53);_LL772: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpF54=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpF3F;if(_tmpF54->tag != 35)goto _LL774;else{_tmpF55=_tmpF54->f1;}}_LL773:
# 4984
 while(1){
# 4985
void*_tmpF58=_tmpF55->r;struct Cyc_Absyn_Stmt*_tmpF5A;struct Cyc_Absyn_Stmt*_tmpF5B;struct Cyc_Absyn_Decl*_tmpF5D;struct Cyc_Absyn_Stmt*_tmpF5E;struct Cyc_Absyn_Exp*_tmpF60;_LL77C: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpF59=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpF58;if(_tmpF59->tag != 2)goto _LL77E;else{_tmpF5A=_tmpF59->f1;_tmpF5B=_tmpF59->f2;}}_LL77D:
# 4986
 _tmpF55=_tmpF5B;goto _LL77B;_LL77E: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpF5C=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpF58;if(_tmpF5C->tag != 12)goto _LL780;else{_tmpF5D=_tmpF5C->f1;_tmpF5E=_tmpF5C->f2;}}_LL77F:
# 4987
 _tmpF55=_tmpF5E;goto _LL77B;_LL780: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpF5F=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpF58;if(_tmpF5F->tag != 1)goto _LL782;else{_tmpF60=_tmpF5F->f1;}}_LL781:
# 4988
 return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF60);_LL782:;_LL783: {
# 4989
const char*_tmp1617;void*_tmp1616;(_tmp1616=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1617="is_noalias_stmt_exp: ill-formed StmtExp",_tag_dyneither(_tmp1617,sizeof(char),40))),_tag_dyneither(_tmp1616,sizeof(void*),0)));}_LL77B:;}_LL774:;_LL775:
# 4992
 return 1;_LL761:;}
# 4995
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
# 4996
return Cyc_Tcutil_is_noalias_path_aux(r,e);}
# 5012 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5014
struct _tuple18 _tmp1618;struct _tuple18 bogus_ans=(_tmp1618.f1=0,((_tmp1618.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1618)));
# 5015
void*_tmpF63=e->r;struct _tuple2*_tmpF65;void*_tmpF66;struct Cyc_Absyn_Exp*_tmpF68;struct _dyneither_ptr*_tmpF69;int _tmpF6A;struct Cyc_Absyn_Exp*_tmpF6C;struct _dyneither_ptr*_tmpF6D;int _tmpF6E;struct Cyc_Absyn_Exp*_tmpF70;struct Cyc_Absyn_Exp*_tmpF72;struct Cyc_Absyn_Exp*_tmpF73;_LL785: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpF64=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF63;if(_tmpF64->tag != 1)goto _LL787;else{_tmpF65=_tmpF64->f1;_tmpF66=(void*)_tmpF64->f2;}}_LL786: {
# 5018
void*_tmpF74=_tmpF66;struct Cyc_Absyn_Vardecl*_tmpF78;struct Cyc_Absyn_Vardecl*_tmpF7A;struct Cyc_Absyn_Vardecl*_tmpF7C;struct Cyc_Absyn_Vardecl*_tmpF7E;_LL792: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpF75=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpF74;if(_tmpF75->tag != 0)goto _LL794;}_LL793:
# 5019
 goto _LL795;_LL794: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpF76=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpF74;if(_tmpF76->tag != 2)goto _LL796;}_LL795:
# 5020
 return bogus_ans;_LL796: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpF77=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpF74;if(_tmpF77->tag != 1)goto _LL798;else{_tmpF78=_tmpF77->f1;}}_LL797: {
# 5022
void*_tmpF7F=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL79F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpF80=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF7F;if(_tmpF80->tag != 8)goto _LL7A1;}_LL7A0: {
# 5024
struct _tuple18 _tmp1619;return(_tmp1619.f1=1,((_tmp1619.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1619)));}_LL7A1:;_LL7A2: {
# 5025
struct _tuple18 _tmp161A;return(_tmp161A.f1=(_tmpF78->tq).real_const,((_tmp161A.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp161A)));}_LL79E:;}_LL798: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpF79=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpF74;if(_tmpF79->tag != 4)goto _LL79A;else{_tmpF7A=_tmpF79->f1;}}_LL799: {
# 5028
void*_tmpF83=Cyc_Tcutil_compress((void*)_check_null(e->topt));_LL7A4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpF84=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF83;if(_tmpF84->tag != 8)goto _LL7A6;}_LL7A5: {
# 5029
struct _tuple18 _tmp161B;return(_tmp161B.f1=1,((_tmp161B.f2=(void*)_check_null(_tmpF7A->rgn),_tmp161B)));}_LL7A6:;_LL7A7:
# 5031
 _tmpF7A->escapes=1;{
# 5032
struct _tuple18 _tmp161C;return(_tmp161C.f1=(_tmpF7A->tq).real_const,((_tmp161C.f2=(void*)_check_null(_tmpF7A->rgn),_tmp161C)));};_LL7A3:;}_LL79A: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpF7B=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpF74;if(_tmpF7B->tag != 5)goto _LL79C;else{_tmpF7C=_tmpF7B->f1;}}_LL79B:
# 5034
 _tmpF7E=_tmpF7C;goto _LL79D;_LL79C: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpF7D=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpF74;if(_tmpF7D->tag != 3)goto _LL791;else{_tmpF7E=_tmpF7D->f1;}}_LL79D:
# 5036
 _tmpF7E->escapes=1;{
# 5037
struct _tuple18 _tmp161D;return(_tmp161D.f1=(_tmpF7E->tq).real_const,((_tmp161D.f2=(void*)_check_null(_tmpF7E->rgn),_tmp161D)));};_LL791:;}_LL787: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpF67=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF63;if(_tmpF67->tag != 20)goto _LL789;else{_tmpF68=_tmpF67->f1;_tmpF69=_tmpF67->f2;_tmpF6A=_tmpF67->f3;}}_LL788:
# 5041
 if(_tmpF6A)return bogus_ans;{
# 5044
void*_tmpF88=Cyc_Tcutil_compress((void*)_check_null(_tmpF68->topt));struct Cyc_List_List*_tmpF8A;struct Cyc_Absyn_AggrInfo _tmpF8C;union Cyc_Absyn_AggrInfoU _tmpF8D;struct Cyc_Absyn_Aggrdecl**_tmpF8E;struct Cyc_Absyn_Aggrdecl*_tmpF8F;_LL7A9: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpF89=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF88;if(_tmpF89->tag != 12)goto _LL7AB;else{_tmpF8A=_tmpF89->f2;}}_LL7AA: {
# 5046
struct Cyc_Absyn_Aggrfield*_tmpF90=Cyc_Absyn_lookup_field(_tmpF8A,_tmpF69);
# 5047
if(_tmpF90 != 0  && _tmpF90->width == 0){
# 5048
struct _tuple18 _tmp161E;return(_tmp161E.f1=(_tmpF90->tq).real_const,((_tmp161E.f2=(Cyc_Tcutil_addressof_props(te,_tmpF68)).f2,_tmp161E)));}
# 5049
return bogus_ans;}_LL7AB: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF8B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpF88;if(_tmpF8B->tag != 11)goto _LL7AD;else{_tmpF8C=_tmpF8B->f1;_tmpF8D=_tmpF8C.aggr_info;if((_tmpF8D.KnownAggr).tag != 2)goto _LL7AD;_tmpF8E=(struct Cyc_Absyn_Aggrdecl**)(_tmpF8D.KnownAggr).val;_tmpF8F=*_tmpF8E;}}_LL7AC: {
# 5051
struct Cyc_Absyn_Aggrfield*_tmpF92=Cyc_Absyn_lookup_decl_field(_tmpF8F,_tmpF69);
# 5052
if(_tmpF92 != 0  && _tmpF92->width == 0){
# 5053
struct _tuple18 _tmp161F;return(_tmp161F.f1=(_tmpF92->tq).real_const,((_tmp161F.f2=(Cyc_Tcutil_addressof_props(te,_tmpF68)).f2,_tmp161F)));}
# 5054
return bogus_ans;}_LL7AD:;_LL7AE:
# 5055
 return bogus_ans;_LL7A8:;};_LL789: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpF6B=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF63;if(_tmpF6B->tag != 21)goto _LL78B;else{_tmpF6C=_tmpF6B->f1;_tmpF6D=_tmpF6B->f2;_tmpF6E=_tmpF6B->f3;}}_LL78A:
# 5059
 if(_tmpF6E)return bogus_ans;{
# 5063
void*_tmpF94=Cyc_Tcutil_compress((void*)_check_null(_tmpF6C->topt));struct Cyc_Absyn_PtrInfo _tmpF96;void*_tmpF97;struct Cyc_Absyn_PtrAtts _tmpF98;void*_tmpF99;_LL7B0: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF95=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF94;if(_tmpF95->tag != 5)goto _LL7B2;else{_tmpF96=_tmpF95->f1;_tmpF97=_tmpF96.elt_typ;_tmpF98=_tmpF96.ptr_atts;_tmpF99=_tmpF98.rgn;}}_LL7B1: {
# 5065
struct Cyc_Absyn_Aggrfield*finfo;
# 5066
{void*_tmpF9A=Cyc_Tcutil_compress(_tmpF97);struct Cyc_List_List*_tmpF9C;struct Cyc_Absyn_AggrInfo _tmpF9E;union Cyc_Absyn_AggrInfoU _tmpF9F;struct Cyc_Absyn_Aggrdecl**_tmpFA0;struct Cyc_Absyn_Aggrdecl*_tmpFA1;_LL7B5: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpF9B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF9A;if(_tmpF9B->tag != 12)goto _LL7B7;else{_tmpF9C=_tmpF9B->f2;}}_LL7B6:
# 5068
 finfo=Cyc_Absyn_lookup_field(_tmpF9C,_tmpF6D);goto _LL7B4;_LL7B7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF9D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpF9A;if(_tmpF9D->tag != 11)goto _LL7B9;else{_tmpF9E=_tmpF9D->f1;_tmpF9F=_tmpF9E.aggr_info;if((_tmpF9F.KnownAggr).tag != 2)goto _LL7B9;_tmpFA0=(struct Cyc_Absyn_Aggrdecl**)(_tmpF9F.KnownAggr).val;_tmpFA1=*_tmpFA0;}}_LL7B8:
# 5070
 finfo=Cyc_Absyn_lookup_decl_field(_tmpFA1,_tmpF6D);goto _LL7B4;_LL7B9:;_LL7BA:
# 5071
 return bogus_ans;_LL7B4:;}
# 5073
if(finfo != 0  && finfo->width == 0){
# 5074
struct _tuple18 _tmp1620;return(_tmp1620.f1=(finfo->tq).real_const,((_tmp1620.f2=_tmpF99,_tmp1620)));}
# 5075
return bogus_ans;}_LL7B2:;_LL7B3:
# 5076
 return bogus_ans;_LL7AF:;};_LL78B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpF6F=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpF63;if(_tmpF6F->tag != 19)goto _LL78D;else{_tmpF70=_tmpF6F->f1;}}_LL78C: {
# 5080
void*_tmpFA3=Cyc_Tcutil_compress((void*)_check_null(_tmpF70->topt));struct Cyc_Absyn_PtrInfo _tmpFA5;struct Cyc_Absyn_Tqual _tmpFA6;struct Cyc_Absyn_PtrAtts _tmpFA7;void*_tmpFA8;_LL7BC: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpFA4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpFA3;if(_tmpFA4->tag != 5)goto _LL7BE;else{_tmpFA5=_tmpFA4->f1;_tmpFA6=_tmpFA5.elt_tq;_tmpFA7=_tmpFA5.ptr_atts;_tmpFA8=_tmpFA7.rgn;}}_LL7BD: {
# 5082
struct _tuple18 _tmp1621;return(_tmp1621.f1=_tmpFA6.real_const,((_tmp1621.f2=_tmpFA8,_tmp1621)));}_LL7BE:;_LL7BF:
# 5083
 return bogus_ans;_LL7BB:;}_LL78D: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpF71=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF63;if(_tmpF71->tag != 22)goto _LL78F;else{_tmpF72=_tmpF71->f1;_tmpF73=_tmpF71->f2;}}_LL78E: {
# 5088
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpF72->topt));
# 5089
void*_tmpFAA=t;struct Cyc_List_List*_tmpFAC;struct Cyc_Absyn_PtrInfo _tmpFAE;struct Cyc_Absyn_Tqual _tmpFAF;struct Cyc_Absyn_PtrAtts _tmpFB0;void*_tmpFB1;struct Cyc_Absyn_ArrayInfo _tmpFB3;struct Cyc_Absyn_Tqual _tmpFB4;_LL7C1: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpFAB=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpFAA;if(_tmpFAB->tag != 10)goto _LL7C3;else{_tmpFAC=_tmpFAB->f1;}}_LL7C2: {
# 5092
unsigned int _tmpFB6;int _tmpFB7;struct _tuple14 _tmpFB5=Cyc_Evexp_eval_const_uint_exp(_tmpF73);_tmpFB6=_tmpFB5.f1;_tmpFB7=_tmpFB5.f2;
# 5093
if(!_tmpFB7)
# 5094
return bogus_ans;{
# 5095
struct _tuple12*_tmpFB8=Cyc_Absyn_lookup_tuple_field(_tmpFAC,(int)_tmpFB6);
# 5096
if(_tmpFB8 != 0){
# 5097
struct _tuple18 _tmp1622;return(_tmp1622.f1=((*_tmpFB8).f1).real_const,((_tmp1622.f2=(Cyc_Tcutil_addressof_props(te,_tmpF72)).f2,_tmp1622)));}
# 5098
return bogus_ans;};}_LL7C3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpFAD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpFAA;if(_tmpFAD->tag != 5)goto _LL7C5;else{_tmpFAE=_tmpFAD->f1;_tmpFAF=_tmpFAE.elt_tq;_tmpFB0=_tmpFAE.ptr_atts;_tmpFB1=_tmpFB0.rgn;}}_LL7C4: {
# 5100
struct _tuple18 _tmp1623;return(_tmp1623.f1=_tmpFAF.real_const,((_tmp1623.f2=_tmpFB1,_tmp1623)));}_LL7C5: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpFB2=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpFAA;if(_tmpFB2->tag != 8)goto _LL7C7;else{_tmpFB3=_tmpFB2->f1;_tmpFB4=_tmpFB3.tq;}}_LL7C6: {
# 5106
struct _tuple18 _tmp1624;return(_tmp1624.f1=_tmpFB4.real_const,((_tmp1624.f2=(Cyc_Tcutil_addressof_props(te,_tmpF72)).f2,_tmp1624)));}_LL7C7:;_LL7C8:
# 5107
 return bogus_ans;_LL7C0:;}_LL78F:;_LL790:
# 5110
{const char*_tmp1627;void*_tmp1626;(_tmp1626=0,Cyc_Tcutil_terr(e->loc,((_tmp1627="unary & applied to non-lvalue",_tag_dyneither(_tmp1627,sizeof(char),30))),_tag_dyneither(_tmp1626,sizeof(void*),0)));}
# 5111
return bogus_ans;_LL784:;}
# 5122 "tcutil.cyc"
void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){
# 5123
void*_tmpFBF=Cyc_Tcutil_compress(e_typ);struct Cyc_Absyn_ArrayInfo _tmpFC1;void*_tmpFC2;struct Cyc_Absyn_Tqual _tmpFC3;union Cyc_Absyn_Constraint*_tmpFC4;_LL7CA: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpFC0=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpFBF;if(_tmpFC0->tag != 8)goto _LL7CC;else{_tmpFC1=_tmpFC0->f1;_tmpFC2=_tmpFC1.elt_type;_tmpFC3=_tmpFC1.tq;_tmpFC4=_tmpFC1.zero_term;}}_LL7CB: {
# 5125
void*_tmpFC6;struct _tuple18 _tmpFC5=Cyc_Tcutil_addressof_props(te,e);_tmpFC6=_tmpFC5.f2;{
# 5126
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp162A;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp1629;return Cyc_Absyn_atb_typ(_tmpFC2,_tmpFC6,_tmpFC3,(void*)((_tmp1629=_cycalloc(sizeof(*_tmp1629)),((_tmp1629[0]=((_tmp162A.tag=1,((_tmp162A.f1=e,_tmp162A)))),_tmp1629)))),_tmpFC4);};}_LL7CC:;_LL7CD:
# 5128
 return e_typ;_LL7C9:;}
# 5134
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
# 5135
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
# 5136
void*_tmpFC9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*_tmpFCC;_LL7CF: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpFCA=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpFC9;if(_tmpFCA->tag != 0)goto _LL7D1;}_LL7D0:
# 5137
 return;_LL7D1: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpFCB=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpFC9;if(_tmpFCB->tag != 1)goto _LL7CE;else{_tmpFCC=_tmpFCB->f1;}}_LL7D2: {
# 5139
unsigned int _tmpFCE;int _tmpFCF;struct _tuple14 _tmpFCD=Cyc_Evexp_eval_const_uint_exp(_tmpFCC);_tmpFCE=_tmpFCD.f1;_tmpFCF=_tmpFCD.f2;
# 5140
if(_tmpFCF  && _tmpFCE <= i){
# 5141
const char*_tmp162F;void*_tmp162E[2];struct Cyc_Int_pa_PrintArg_struct _tmp162D;struct Cyc_Int_pa_PrintArg_struct _tmp162C;(_tmp162C.tag=1,((_tmp162C.f1=(unsigned long)((int)i),((_tmp162D.tag=1,((_tmp162D.f1=(unsigned long)((int)_tmpFCE),((_tmp162E[0]=& _tmp162D,((_tmp162E[1]=& _tmp162C,Cyc_Tcutil_terr(loc,((_tmp162F="dereference is out of bounds: %d <= %d",_tag_dyneither(_tmp162F,sizeof(char),39))),_tag_dyneither(_tmp162E,sizeof(void*),2)))))))))))));}
# 5142
return;}_LL7CE:;};}
# 5146
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
# 5147
Cyc_Tcutil_check_bound(loc,0,b);}
# 5150
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
# 5151
void*_tmpFD4=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);struct Cyc_Absyn_Exp*_tmpFD6;_LL7D4: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpFD5=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpFD4;if(_tmpFD5->tag != 1)goto _LL7D6;else{_tmpFD6=_tmpFD5->f1;}}_LL7D5: {
# 5153
unsigned int _tmpFD8;int _tmpFD9;struct _tuple14 _tmpFD7=Cyc_Evexp_eval_const_uint_exp(_tmpFD6);_tmpFD8=_tmpFD7.f1;_tmpFD9=_tmpFD7.f2;
# 5154
return _tmpFD9  && _tmpFD8 == 1;}_LL7D6:;_LL7D7:
# 5155
 return 0;_LL7D3:;}
# 5159
int Cyc_Tcutil_bits_only(void*t){
# 5160
void*_tmpFDA=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmpFE1;void*_tmpFE2;union Cyc_Absyn_Constraint*_tmpFE3;struct Cyc_List_List*_tmpFE5;struct Cyc_Absyn_AggrInfo _tmpFE7;union Cyc_Absyn_AggrInfoU _tmpFE8;struct _tuple4 _tmpFE9;struct Cyc_Absyn_AggrInfo _tmpFEB;union Cyc_Absyn_AggrInfoU _tmpFEC;struct Cyc_Absyn_Aggrdecl**_tmpFED;struct Cyc_Absyn_Aggrdecl*_tmpFEE;struct Cyc_List_List*_tmpFEF;struct Cyc_List_List*_tmpFF1;_LL7D9: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpFDB=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpFDA;if(_tmpFDB->tag != 0)goto _LL7DB;}_LL7DA:
# 5161
 goto _LL7DC;_LL7DB: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpFDC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpFDA;if(_tmpFDC->tag != 6)goto _LL7DD;}_LL7DC:
# 5162
 goto _LL7DE;_LL7DD: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpFDD=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpFDA;if(_tmpFDD->tag != 7)goto _LL7DF;}_LL7DE:
# 5163
 return 1;_LL7DF: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpFDE=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpFDA;if(_tmpFDE->tag != 13)goto _LL7E1;}_LL7E0:
# 5164
 goto _LL7E2;_LL7E1: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpFDF=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpFDA;if(_tmpFDF->tag != 14)goto _LL7E3;}_LL7E2:
# 5165
 return 0;_LL7E3: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpFE0=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpFDA;if(_tmpFE0->tag != 8)goto _LL7E5;else{_tmpFE1=_tmpFE0->f1;_tmpFE2=_tmpFE1.elt_type;_tmpFE3=_tmpFE1.zero_term;}}_LL7E4:
# 5169
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpFE3) && Cyc_Tcutil_bits_only(_tmpFE2);_LL7E5: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpFE4=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpFDA;if(_tmpFE4->tag != 10)goto _LL7E7;else{_tmpFE5=_tmpFE4->f1;}}_LL7E6:
# 5171
 for(0;_tmpFE5 != 0;_tmpFE5=_tmpFE5->tl){
# 5172
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpFE5->hd)).f2))return 0;}
# 5173
return 1;_LL7E7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpFE6=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpFDA;if(_tmpFE6->tag != 11)goto _LL7E9;else{_tmpFE7=_tmpFE6->f1;_tmpFE8=_tmpFE7.aggr_info;if((_tmpFE8.UnknownAggr).tag != 1)goto _LL7E9;_tmpFE9=(struct _tuple4)(_tmpFE8.UnknownAggr).val;}}_LL7E8:
# 5174
 return 0;_LL7E9: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpFEA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpFDA;if(_tmpFEA->tag != 11)goto _LL7EB;else{_tmpFEB=_tmpFEA->f1;_tmpFEC=_tmpFEB.aggr_info;if((_tmpFEC.KnownAggr).tag != 2)goto _LL7EB;_tmpFED=(struct Cyc_Absyn_Aggrdecl**)(_tmpFEC.KnownAggr).val;_tmpFEE=*_tmpFED;_tmpFEF=_tmpFEB.targs;}}_LL7EA:
# 5176
 if(_tmpFEE->impl == 0)
# 5177
return 0;{
# 5178
int okay=1;
# 5179
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpFEE->impl))->fields;for(0;fs != 0;fs=fs->tl){
# 5180
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
# 5181
if(okay)return 1;{
# 5182
struct _RegionHandle _tmpFF2=_new_region("rgn");struct _RegionHandle*rgn=& _tmpFF2;_push_region(rgn);
# 5183
{struct Cyc_List_List*_tmpFF3=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpFEE->tvs,_tmpFEF);
# 5184
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpFEE->impl))->fields;for(0;fs != 0;fs=fs->tl){
# 5185
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpFF3,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpFF4=0;_npop_handler(0);return _tmpFF4;}}}{
# 5186
int _tmpFF5=1;_npop_handler(0);return _tmpFF5;};}
# 5183
;_pop_region(rgn);};};_LL7EB: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpFF0=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpFDA;if(_tmpFF0->tag != 12)goto _LL7ED;else{_tmpFF1=_tmpFF0->f2;}}_LL7EC:
# 5189
 for(0;_tmpFF1 != 0;_tmpFF1=_tmpFF1->tl){
# 5190
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpFF1->hd)->type))return 0;}
# 5191
return 1;_LL7ED:;_LL7EE:
# 5192
 return 0;_LL7D8:;}
# 5201
static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){
# 5202
void*_tmpFF6=e->r;struct _tuple2*_tmpFFE;void*_tmpFFF;struct Cyc_Absyn_Exp*_tmp1001;struct Cyc_Absyn_Exp*_tmp1002;struct Cyc_Absyn_Exp*_tmp1003;struct Cyc_Absyn_Exp*_tmp1005;struct Cyc_Absyn_Exp*_tmp1006;struct Cyc_Absyn_Exp*_tmp1008;struct Cyc_Absyn_Exp*_tmp100A;void*_tmp100C;struct Cyc_Absyn_Exp*_tmp100D;enum Cyc_Absyn_Coercion _tmp100E;void*_tmp1010;struct Cyc_Absyn_Exp*_tmp1011;struct Cyc_Absyn_Exp*_tmp1013;struct Cyc_Absyn_Exp*_tmp1015;struct Cyc_Absyn_Exp*_tmp1016;struct Cyc_List_List*_tmp1018;struct Cyc_List_List*_tmp101A;struct Cyc_List_List*_tmp101C;enum Cyc_Absyn_Primop _tmp101E;struct Cyc_List_List*_tmp101F;struct Cyc_List_List*_tmp1021;struct Cyc_List_List*_tmp1023;_LL7F0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpFF7=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpFF6;if(_tmpFF7->tag != 0)goto _LL7F2;}_LL7F1:
# 5203
 goto _LL7F3;_LL7F2: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpFF8=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpFF6;if(_tmpFF8->tag != 16)goto _LL7F4;}_LL7F3:
# 5204
 goto _LL7F5;_LL7F4: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpFF9=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpFF6;if(_tmpFF9->tag != 17)goto _LL7F6;}_LL7F5:
# 5205
 goto _LL7F7;_LL7F6: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpFFA=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpFF6;if(_tmpFFA->tag != 18)goto _LL7F8;}_LL7F7:
# 5206
 goto _LL7F9;_LL7F8: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpFFB=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpFF6;if(_tmpFFB->tag != 30)goto _LL7FA;}_LL7F9:
# 5207
 goto _LL7FB;_LL7FA: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpFFC=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpFF6;if(_tmpFFC->tag != 31)goto _LL7FC;}_LL7FB:
# 5208
 return 1;_LL7FC: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpFFD=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpFF6;if(_tmpFFD->tag != 1)goto _LL7FE;else{_tmpFFE=_tmpFFD->f1;_tmpFFF=(void*)_tmpFFD->f2;}}_LL7FD: {
# 5212
void*_tmp1024=_tmpFFF;struct Cyc_Absyn_Vardecl*_tmp1027;struct Cyc_Absyn_Vardecl*_tmp1029;_LL81D: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp1025=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp1024;if(_tmp1025->tag != 2)goto _LL81F;}_LL81E:
# 5213
 return 1;_LL81F: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp1026=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp1024;if(_tmp1026->tag != 1)goto _LL821;else{_tmp1027=_tmp1026->f1;}}_LL820: {
# 5215
void*_tmp102B=Cyc_Tcutil_compress(_tmp1027->type);_LL828: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp102C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp102B;if(_tmp102C->tag != 8)goto _LL82A;}_LL829:
# 5216
 goto _LL82B;_LL82A: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp102D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp102B;if(_tmp102D->tag != 9)goto _LL82C;}_LL82B:
# 5217
 return 1;_LL82C:;_LL82D:
# 5218
 return var_okay;_LL827:;}_LL821: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp1028=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp1024;if(_tmp1028->tag != 4)goto _LL823;else{_tmp1029=_tmp1028->f1;}}_LL822:
# 5222
 if(_tmp1029->sc == Cyc_Absyn_Static){
# 5223
void*_tmp102E=Cyc_Tcutil_compress(_tmp1029->type);_LL82F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp102F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp102E;if(_tmp102F->tag != 8)goto _LL831;}_LL830:
# 5224
 goto _LL832;_LL831: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1030=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp102E;if(_tmp1030->tag != 9)goto _LL833;}_LL832:
# 5225
 return 1;_LL833:;_LL834:
# 5226
 return var_okay;_LL82E:;}else{
# 5229
return var_okay;}_LL823: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp102A=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp1024;if(_tmp102A->tag != 0)goto _LL825;}_LL824:
# 5230
 return 0;_LL825:;_LL826:
# 5231
 return var_okay;_LL81C:;}_LL7FE: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1000=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpFF6;if(_tmp1000->tag != 5)goto _LL800;else{_tmp1001=_tmp1000->f1;_tmp1002=_tmp1000->f2;_tmp1003=_tmp1000->f3;}}_LL7FF:
# 5235
 return(Cyc_Tcutil_cnst_exp(te,0,_tmp1001) && 
# 5236
Cyc_Tcutil_cnst_exp(te,0,_tmp1002)) && 
# 5237
Cyc_Tcutil_cnst_exp(te,0,_tmp1003);_LL800: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp1004=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpFF6;if(_tmp1004->tag != 8)goto _LL802;else{_tmp1005=_tmp1004->f1;_tmp1006=_tmp1004->f2;}}_LL801:
# 5239
 return Cyc_Tcutil_cnst_exp(te,0,_tmp1005) && Cyc_Tcutil_cnst_exp(te,0,_tmp1006);_LL802: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp1007=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpFF6;if(_tmp1007->tag != 11)goto _LL804;else{_tmp1008=_tmp1007->f1;}}_LL803:
# 5240
 _tmp100A=_tmp1008;goto _LL805;_LL804: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp1009=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpFF6;if(_tmp1009->tag != 12)goto _LL806;else{_tmp100A=_tmp1009->f1;}}_LL805:
# 5242
 return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp100A);_LL806: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp100B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpFF6;if(_tmp100B->tag != 13)goto _LL808;else{_tmp100C=(void*)_tmp100B->f1;_tmp100D=_tmp100B->f2;_tmp100E=_tmp100B->f4;if(_tmp100E != Cyc_Absyn_No_coercion)goto _LL808;}}_LL807:
# 5244
 return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp100D);_LL808: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp100F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpFF6;if(_tmp100F->tag != 13)goto _LL80A;else{_tmp1010=(void*)_tmp100F->f1;_tmp1011=_tmp100F->f2;}}_LL809:
# 5247
 return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp1011);_LL80A: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp1012=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpFF6;if(_tmp1012->tag != 14)goto _LL80C;else{_tmp1013=_tmp1012->f1;}}_LL80B:
# 5249
 return Cyc_Tcutil_cnst_exp(te,1,_tmp1013);_LL80C: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp1014=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpFF6;if(_tmp1014->tag != 26)goto _LL80E;else{_tmp1015=_tmp1014->f2;_tmp1016=_tmp1014->f3;}}_LL80D:
# 5251
 return Cyc_Tcutil_cnst_exp(te,0,_tmp1015) && Cyc_Tcutil_cnst_exp(te,0,_tmp1016);_LL80E: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1017=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpFF6;if(_tmp1017->tag != 25)goto _LL810;else{_tmp1018=_tmp1017->f1;}}_LL80F:
# 5252
 _tmp101A=_tmp1018;goto _LL811;_LL810: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp1019=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpFF6;if(_tmp1019->tag != 28)goto _LL812;else{_tmp101A=_tmp1019->f2;}}_LL811:
# 5253
 _tmp101C=_tmp101A;goto _LL813;_LL812: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp101B=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpFF6;if(_tmp101B->tag != 27)goto _LL814;else{_tmp101C=_tmp101B->f3;}}_LL813:
# 5255
 for(0;_tmp101C != 0;_tmp101C=_tmp101C->tl){
# 5256
if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple19*)_tmp101C->hd)).f2))
# 5257
return 0;}
# 5258
return 1;_LL814: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp101D=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpFF6;if(_tmp101D->tag != 2)goto _LL816;else{_tmp101E=_tmp101D->f1;_tmp101F=_tmp101D->f2;}}_LL815:
# 5260
 _tmp1021=_tmp101F;goto _LL817;_LL816: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp1020=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpFF6;if(_tmp1020->tag != 23)goto _LL818;else{_tmp1021=_tmp1020->f1;}}_LL817:
# 5261
 _tmp1023=_tmp1021;goto _LL819;_LL818: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1022=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpFF6;if(_tmp1022->tag != 29)goto _LL81A;else{_tmp1023=_tmp1022->f1;}}_LL819:
# 5263
 for(0;_tmp1023 != 0;_tmp1023=_tmp1023->tl){
# 5264
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmp1023->hd))
# 5265
return 0;}
# 5266
return 1;_LL81A:;_LL81B:
# 5267
 return 0;_LL7EF:;}
# 5271
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5272
return Cyc_Tcutil_cnst_exp(te,0,e);}
# 5275
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5277
int Cyc_Tcutil_supports_default(void*t){
# 5278
void*_tmp1031=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1036;struct Cyc_Absyn_PtrAtts _tmp1037;union Cyc_Absyn_Constraint*_tmp1038;union Cyc_Absyn_Constraint*_tmp1039;struct Cyc_Absyn_ArrayInfo _tmp103B;void*_tmp103C;struct Cyc_List_List*_tmp103E;struct Cyc_Absyn_AggrInfo _tmp1040;union Cyc_Absyn_AggrInfoU _tmp1041;struct Cyc_List_List*_tmp1042;struct Cyc_List_List*_tmp1044;_LL836: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp1032=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1031;if(_tmp1032->tag != 0)goto _LL838;}_LL837:
# 5279
 goto _LL839;_LL838: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1033=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1031;if(_tmp1033->tag != 6)goto _LL83A;}_LL839:
# 5280
 goto _LL83B;_LL83A: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp1034=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1031;if(_tmp1034->tag != 7)goto _LL83C;}_LL83B:
# 5281
 return 1;_LL83C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1035=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1031;if(_tmp1035->tag != 5)goto _LL83E;else{_tmp1036=_tmp1035->f1;_tmp1037=_tmp1036.ptr_atts;_tmp1038=_tmp1037.nullable;_tmp1039=_tmp1037.bounds;}}_LL83D: {
# 5284
void*_tmp1047=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp1039);_LL84D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp1048=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1047;if(_tmp1048->tag != 0)goto _LL84F;}_LL84E:
# 5285
 return 1;_LL84F:;_LL850:
# 5287
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp1038);_LL84C:;}_LL83E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp103A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1031;if(_tmp103A->tag != 8)goto _LL840;else{_tmp103B=_tmp103A->f1;_tmp103C=_tmp103B.elt_type;}}_LL83F:
# 5290
 return Cyc_Tcutil_supports_default(_tmp103C);_LL840: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp103D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1031;if(_tmp103D->tag != 10)goto _LL842;else{_tmp103E=_tmp103D->f1;}}_LL841:
# 5292
 for(0;_tmp103E != 0;_tmp103E=_tmp103E->tl){
# 5293
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmp103E->hd)).f2))return 0;}
# 5294
return 1;_LL842: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp103F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1031;if(_tmp103F->tag != 11)goto _LL844;else{_tmp1040=_tmp103F->f1;_tmp1041=_tmp1040.aggr_info;_tmp1042=_tmp1040.targs;}}_LL843: {
# 5296
struct Cyc_Absyn_Aggrdecl*_tmp1049=Cyc_Absyn_get_known_aggrdecl(_tmp1041);
# 5297
if(_tmp1049->impl == 0)return 0;
# 5298
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1049->impl))->exist_vars != 0)return 0;
# 5299
return Cyc_Tcutil_fields_support_default(_tmp1049->tvs,_tmp1042,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1049->impl))->fields);}_LL844: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1043=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1031;if(_tmp1043->tag != 12)goto _LL846;else{_tmp1044=_tmp1043->f2;}}_LL845:
# 5300
 return Cyc_Tcutil_fields_support_default(0,0,_tmp1044);_LL846: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp1045=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1031;if(_tmp1045->tag != 14)goto _LL848;}_LL847:
# 5302
 goto _LL849;_LL848: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp1046=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1031;if(_tmp1046->tag != 13)goto _LL84A;}_LL849:
# 5303
 return 1;_LL84A:;_LL84B:
# 5305
 return 0;_LL835:;}
# 5310
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
# 5311
void*_tmp104A=t;struct Cyc_Absyn_Typedefdecl*_tmp104C;_LL852: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp104B=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp104A;if(_tmp104B->tag != 17)goto _LL854;else{_tmp104C=_tmp104B->f3;}}_LL853:
# 5313
 if(_tmp104C != 0){
# 5314
struct Cyc_Absyn_Tqual _tmp104D=_tmp104C->tq;
# 5315
if(((_tmp104D.print_const  || _tmp104D.q_volatile) || _tmp104D.q_restrict) || _tmp104D.real_const){
# 5318
const char*_tmp1633;void*_tmp1632[1];struct Cyc_String_pa_PrintArg_struct _tmp1631;(_tmp1631.tag=0,((_tmp1631.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1632[0]=& _tmp1631,Cyc_Tcutil_warn(loc,((_tmp1633="qualifier within typedef type %s is ignored",_tag_dyneither(_tmp1633,sizeof(char),44))),_tag_dyneither(_tmp1632,sizeof(void*),1)))))));}}
# 5321
goto _LL851;_LL854:;_LL855:
# 5322
 goto _LL851;_LL851:;}
# 5326
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5328
struct _RegionHandle _tmp1051=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1051;_push_region(rgn);{
# 5329
struct Cyc_List_List*_tmp1052=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
# 5330
for(0;fs != 0;fs=fs->tl){
# 5331
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
# 5332
if(Cyc_Tcutil_supports_default(t)){int _tmp1053=1;_npop_handler(0);return _tmp1053;}
# 5333
t=Cyc_Tcutil_rsubstitute(rgn,_tmp1052,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
# 5334
if(!Cyc_Tcutil_supports_default(t)){int _tmp1054=0;_npop_handler(0);return _tmp1054;}}}{
# 5337
int _tmp1055=1;_npop_handler(0);return _tmp1055;};
# 5328
;_pop_region(rgn);}
# 5343
int Cyc_Tcutil_admits_zero(void*t){
# 5344
void*_tmp1056=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp105A;struct Cyc_Absyn_PtrAtts _tmp105B;union Cyc_Absyn_Constraint*_tmp105C;union Cyc_Absyn_Constraint*_tmp105D;_LL857: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1057=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1056;if(_tmp1057->tag != 6)goto _LL859;}_LL858:
# 5345
 goto _LL85A;_LL859: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp1058=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1056;if(_tmp1058->tag != 7)goto _LL85B;}_LL85A:
# 5346
 return 1;_LL85B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1059=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1056;if(_tmp1059->tag != 5)goto _LL85D;else{_tmp105A=_tmp1059->f1;_tmp105B=_tmp105A.ptr_atts;_tmp105C=_tmp105B.nullable;_tmp105D=_tmp105B.bounds;}}_LL85C: {
# 5348
void*_tmp105E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp105D);_LL860: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp105F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp105E;if(_tmp105F->tag != 0)goto _LL862;}_LL861:
# 5352
 return 0;_LL862:;_LL863:
# 5353
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp105C);_LL85F:;}_LL85D:;_LL85E:
# 5355
 return 0;_LL856:;}
# 5359
int Cyc_Tcutil_is_noreturn(void*t){
# 5360
{void*_tmp1060=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1062;void*_tmp1063;struct Cyc_Absyn_FnInfo _tmp1065;struct Cyc_List_List*_tmp1066;_LL865: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1061=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1060;if(_tmp1061->tag != 5)goto _LL867;else{_tmp1062=_tmp1061->f1;_tmp1063=_tmp1062.elt_typ;}}_LL866:
# 5361
 return Cyc_Tcutil_is_noreturn(_tmp1063);_LL867: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1064=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1060;if(_tmp1064->tag != 9)goto _LL869;else{_tmp1065=_tmp1064->f1;_tmp1066=_tmp1065.attributes;}}_LL868:
# 5363
 for(0;_tmp1066 != 0;_tmp1066=_tmp1066->tl){
# 5364
void*_tmp1067=(void*)_tmp1066->hd;_LL86C: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp1068=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp1067;if(_tmp1068->tag != 4)goto _LL86E;}_LL86D:
# 5365
 return 1;_LL86E:;_LL86F:
# 5366
 continue;_LL86B:;}
# 5368
goto _LL864;_LL869:;_LL86A:
# 5369
 goto _LL864;_LL864:;}
# 5371
return 0;}
# 5376
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
# 5377
void*_tmp1069=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp106B;struct Cyc_List_List*_tmp106C;struct Cyc_List_List**_tmp106D;_LL871: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp106A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1069;if(_tmp106A->tag != 9)goto _LL873;else{_tmp106B=_tmp106A->f1;_tmp106C=_tmp106B.attributes;_tmp106D=(struct Cyc_List_List**)&(_tmp106A->f1).attributes;}}_LL872: {
# 5379
struct Cyc_List_List*_tmp106E=0;
# 5380
for(0;atts != 0;atts=atts->tl){
# 5381
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
# 5382
struct Cyc_List_List*_tmp1634;*_tmp106D=((_tmp1634=_cycalloc(sizeof(*_tmp1634)),((_tmp1634->hd=(void*)atts->hd,((_tmp1634->tl=*_tmp106D,_tmp1634))))));}else{
# 5384
struct Cyc_List_List*_tmp1635;_tmp106E=((_tmp1635=_cycalloc(sizeof(*_tmp1635)),((_tmp1635->hd=(void*)atts->hd,((_tmp1635->tl=_tmp106E,_tmp1635))))));}}
# 5385
return _tmp106E;}_LL873:;_LL874: {
# 5386
const char*_tmp1638;void*_tmp1637;(_tmp1637=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1638="transfer_fn_type_atts",_tag_dyneither(_tmp1638,sizeof(char),22))),_tag_dyneither(_tmp1637,sizeof(void*),0)));}_LL870:;}
