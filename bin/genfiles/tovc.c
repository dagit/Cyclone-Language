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
# 72
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 282 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict,void*);
# 50 "string.h"
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
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 433 "absyn.h"
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 905 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 913
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 915
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 951
extern struct _tuple0*Cyc_Absyn_exn_name;
# 982
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 1005
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned int);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1043
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1052
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1069
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
# 1071
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1072
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1082
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1097
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 69 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 69
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 141
struct Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*s);struct Cyc_RgnOrder_RgnPO;
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
# 65 "tcutil.h"
void*Cyc_Tcutil_compress(void*t);
# 164
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 338 "tcutil.h"
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 35 "toc.h"
struct _tuple0*Cyc_Toc_temp_var();
# 36
extern struct _dyneither_ptr Cyc_Toc_globals;
# 27 "tovc.h"
extern int Cyc_Tovc_elim_array_initializers;
# 28
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*decls);
# 79 "tovc.cyc"
int Cyc_Tovc_elim_array_initializers=1;struct Cyc_Tovc_BoxingEnv{struct _RegionHandle*rgn;struct Cyc_List_List*all_locals;struct Cyc_Dict_Dict varmap;struct Cyc_List_List*boundvars;struct Cyc_Set_Set**freevars;};
# 92
typedef struct Cyc_Tovc_BoxingEnv Cyc_Tovc_boxingenv_t;struct Cyc_Tovc_ToExpEnv{struct _RegionHandle*rgn;struct Cyc_List_List*all_locals;struct Cyc_Dict_Dict varmap;struct Cyc_Absyn_Stmt*encloser;struct Cyc_List_List**gen_ds;};
# 101
typedef struct Cyc_Tovc_ToExpEnv Cyc_Tovc_toexpenv_t;struct _tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 103
static void Cyc_Tovc_box_free_vars_exp(struct Cyc_Tovc_BoxingEnv env,struct Cyc_Absyn_Exp*e){
# 104
while(1){
# 105
void*_tmp0=e->r;struct _tuple0*_tmp2;struct _tuple0 _tmp3;union Cyc_Absyn_Nmspace _tmp4;int _tmp5;struct _dyneither_ptr*_tmp6;struct Cyc_Absyn_Exp*_tmpA;struct Cyc_List_List*_tmpB;struct Cyc_List_List*_tmpD;struct Cyc_Absyn_Exp*_tmpF;struct Cyc_Absyn_Exp*_tmp10;struct Cyc_Absyn_Exp*_tmp11;struct Cyc_Absyn_Exp*_tmp13;struct Cyc_Absyn_Exp*_tmp14;struct Cyc_Absyn_Exp*_tmp16;struct Cyc_Absyn_Exp*_tmp17;struct Cyc_Absyn_Exp*_tmp19;struct Cyc_Absyn_Exp*_tmp1A;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Exp*_tmp1F;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_Absyn_Exp*_tmp22;struct Cyc_Absyn_Exp*_tmp24;struct Cyc_Absyn_Exp*_tmp26;struct Cyc_Absyn_Exp*_tmp28;struct Cyc_Absyn_Exp*_tmp2A;struct Cyc_Absyn_Exp*_tmp2C;struct Cyc_Absyn_Exp*_tmp2E;struct Cyc_Absyn_Exp*_tmp30;struct Cyc_Absyn_Exp*_tmp32;struct Cyc_List_List*_tmp37;_LL1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp1->tag != 1)goto _LL3;else{_tmp2=_tmp1->f1;_tmp3=*_tmp2;_tmp4=_tmp3.f1;if((_tmp4.Loc_n).tag != 4)goto _LL3;_tmp5=(int)(_tmp4.Loc_n).val;_tmp6=_tmp3.f2;}}_LL2:
# 109
 if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(env.varmap,_tmp6)){
# 110
e->r=(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(env.varmap,_tmp6))->r;
# 111
continue;}
# 114
{int i=0;for(0;i < _get_dyneither_size(Cyc_Toc_globals,sizeof(struct _dyneither_ptr*));++ i){
# 115
if(Cyc_strptrcmp(_tmp6,*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Toc_globals,sizeof(struct _dyneither_ptr*),i)))== 0)
# 116
return;}}
# 118
for(0;env.boundvars != 0;env.boundvars=(env.boundvars)->tl){
# 119
if(Cyc_strptrcmp(_tmp6,(*((struct Cyc_Absyn_Vardecl*)(env.boundvars)->hd)->name).f2)== 0)
# 120
return;}
# 122
for(0;env.all_locals != 0;env.all_locals=(env.all_locals)->tl){
# 123
struct Cyc_Absyn_Vardecl*_tmp38=(struct Cyc_Absyn_Vardecl*)(env.all_locals)->hd;
# 124
if(Cyc_strptrcmp(_tmp6,(*_tmp38->name).f2)== 0){
# 126
{void*_tmp39=_tmp38->type;_LL34: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39;if(_tmp3A->tag != 8)goto _LL36;}_LL35:
# 127
 goto _LL33;_LL36:;_LL37:
# 128
 e->r=(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp38->name,0),0))->r;goto _LL33;_LL33:;}
# 130
*env.freevars=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Absyn_Vardecl*elt))Cyc_Set_rinsert)(env.rgn,*env.freevars,_tmp38);
# 131
return;}}
# 137
for(0;env.all_locals != 0;env.all_locals=(env.all_locals)->tl){
# 138
if(Cyc_strptrcmp(_tmp6,(*((struct Cyc_Absyn_Vardecl*)(env.all_locals)->hd)->name).f2)== 0){
# 139
struct Cyc_Core_Impossible_exn_struct _tmp184;const char*_tmp183;void*_tmp182[1];struct Cyc_String_pa_PrintArg_struct _tmp181;struct Cyc_Core_Impossible_exn_struct*_tmp180;(int)_throw((void*)((_tmp180=_cycalloc(sizeof(*_tmp180)),((_tmp180[0]=((_tmp184.tag=Cyc_Core_Impossible,((_tmp184.f1=(struct _dyneither_ptr)((_tmp181.tag=0,((_tmp181.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp6),((_tmp182[0]=& _tmp181,Cyc_aprintf(((_tmp183="unbound variable %s found in box_free_vars_exp",_tag_dyneither(_tmp183,sizeof(char),47))),_tag_dyneither(_tmp182,sizeof(void*),1)))))))),_tmp184)))),_tmp180)))));}}
# 144
return;_LL3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp7->tag != 1)goto _LL5;}_LL4:
# 147
 return;_LL5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp8=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp8->tag != 0)goto _LL7;}_LL6:
# 150
 return;_LL7: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp9=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp9->tag != 9)goto _LL9;else{_tmpA=_tmp9->f1;_tmpB=_tmp9->f2;}}_LL8:
# 152
 Cyc_Tovc_box_free_vars_exp(env,_tmpA);_tmpD=_tmpB;goto _LLA;_LL9: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpC=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmpC->tag != 2)goto _LLB;else{_tmpD=_tmpC->f2;}}_LLA:
# 154
 for(0;_tmpD != 0;_tmpD=_tmpD->tl){
# 155
Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)_tmpD->hd);}
# 156
return;_LLB: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpE=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmpE->tag != 5)goto _LLD;else{_tmpF=_tmpE->f1;_tmp10=_tmpE->f2;_tmp11=_tmpE->f3;}}_LLC:
# 159
 Cyc_Tovc_box_free_vars_exp(env,_tmp11);_tmp13=_tmpF;_tmp14=_tmp10;goto _LLE;_LLD: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp12=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp12->tag != 6)goto _LLF;else{_tmp13=_tmp12->f1;_tmp14=_tmp12->f2;}}_LLE:
# 160
 _tmp16=_tmp13;_tmp17=_tmp14;goto _LL10;_LLF: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp15=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp15->tag != 7)goto _LL11;else{_tmp16=_tmp15->f1;_tmp17=_tmp15->f2;}}_LL10:
# 161
 _tmp19=_tmp16;_tmp1A=_tmp17;goto _LL12;_LL11: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp18=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp18->tag != 22)goto _LL13;else{_tmp19=_tmp18->f1;_tmp1A=_tmp18->f2;}}_LL12:
# 162
 _tmp1C=_tmp19;_tmp1D=_tmp1A;goto _LL14;_LL13: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp1B=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp1B->tag != 8)goto _LL15;else{_tmp1C=_tmp1B->f1;_tmp1D=_tmp1B->f2;}}_LL14:
# 163
 _tmp1F=_tmp1C;_tmp20=_tmp1D;goto _LL16;_LL15: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp1E=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp1E->tag != 3)goto _LL17;else{_tmp1F=_tmp1E->f1;_tmp20=_tmp1E->f3;}}_LL16:
# 165
 Cyc_Tovc_box_free_vars_exp(env,_tmp20);_tmp22=_tmp1F;goto _LL18;_LL17: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp21=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp21->tag != 11)goto _LL19;else{_tmp22=_tmp21->f1;}}_LL18:
# 167
 _tmp24=_tmp22;goto _LL1A;_LL19: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp23=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp23->tag != 12)goto _LL1B;else{_tmp24=_tmp23->f1;}}_LL1A:
# 168
 _tmp26=_tmp24;goto _LL1C;_LL1B: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp25=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp25->tag != 13)goto _LL1D;else{_tmp26=_tmp25->f2;}}_LL1C:
# 169
 _tmp28=_tmp26;goto _LL1E;_LL1D: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp27=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp27->tag != 14)goto _LL1F;else{_tmp28=_tmp27->f1;}}_LL1E:
# 170
 _tmp2A=_tmp28;goto _LL20;_LL1F: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp29=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp29->tag != 17)goto _LL21;else{_tmp2A=_tmp29->f1;}}_LL20:
# 171
 _tmp2C=_tmp2A;goto _LL22;_LL21: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp2B=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp2B->tag != 19)goto _LL23;else{_tmp2C=_tmp2B->f1;}}_LL22:
# 172
 _tmp2E=_tmp2C;goto _LL24;_LL23: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp2D=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp2D->tag != 20)goto _LL25;else{_tmp2E=_tmp2D->f1;}}_LL24:
# 173
 _tmp30=_tmp2E;goto _LL26;_LL25: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp2F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp2F->tag != 21)goto _LL27;else{_tmp30=_tmp2F->f1;}}_LL26:
# 174
 _tmp32=_tmp30;goto _LL28;_LL27: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp31=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp31->tag != 4)goto _LL29;else{_tmp32=_tmp31->f1;}}_LL28:
# 176
 Cyc_Tovc_box_free_vars_exp(env,_tmp32);
# 177
return;_LL29: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp33=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp33->tag != 16)goto _LL2B;}_LL2A:
# 179
 goto _LL2C;_LL2B: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp34=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp34->tag != 18)goto _LL2D;}_LL2C:
# 180
 goto _LL2E;_LL2D: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp35=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp35->tag != 30)goto _LL2F;}_LL2E:
# 181
 return;_LL2F: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp36=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp36->tag != 34)goto _LL31;else{_tmp37=_tmp36->f2;}}_LL30:
# 184
 for(0;_tmp37 != 0;_tmp37=_tmp37->tl){
# 185
Cyc_Tovc_box_free_vars_exp(env,(*((struct _tuple10*)_tmp37->hd)).f2);}
# 186
return;_LL31:;_LL32: {
# 188
struct Cyc_Core_Impossible_exn_struct _tmp18A;const char*_tmp189;struct Cyc_Core_Impossible_exn_struct*_tmp188;(int)_throw((void*)((_tmp188=_cycalloc(sizeof(*_tmp188)),((_tmp188[0]=((_tmp18A.tag=Cyc_Core_Impossible,((_tmp18A.f1=((_tmp189="bad exp form in Tocv::box_free_vars_exp",_tag_dyneither(_tmp189,sizeof(char),40))),_tmp18A)))),_tmp188)))));}_LL0:;}}
# 192
static void Cyc_Tovc_box_free_vars_stmt(struct Cyc_Tovc_BoxingEnv env,struct Cyc_Absyn_Stmt*s){
# 196
while(1){
# 197
void*_tmp43=s->r;struct Cyc_Absyn_Stmt*_tmp49;struct Cyc_Absyn_Exp*_tmp4B;struct Cyc_Absyn_Exp*_tmp4D;struct Cyc_Absyn_Stmt*_tmp4F;struct Cyc_Absyn_Stmt*_tmp50;struct Cyc_Absyn_Exp*_tmp52;struct Cyc_Absyn_Stmt*_tmp53;struct Cyc_Absyn_Stmt*_tmp54;struct _tuple9 _tmp56;struct Cyc_Absyn_Exp*_tmp57;struct Cyc_Absyn_Stmt*_tmp58;struct Cyc_Absyn_Exp*_tmp5A;struct _tuple9 _tmp5B;struct Cyc_Absyn_Exp*_tmp5C;struct _tuple9 _tmp5D;struct Cyc_Absyn_Exp*_tmp5E;struct Cyc_Absyn_Stmt*_tmp5F;struct Cyc_Absyn_Stmt*_tmp61;struct _tuple9 _tmp62;struct Cyc_Absyn_Exp*_tmp63;struct Cyc_Absyn_Exp*_tmp65;struct Cyc_List_List*_tmp66;struct Cyc_Absyn_Decl*_tmp68;struct Cyc_Absyn_Stmt*_tmp69;_LL39: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp44=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp43;if(_tmp44->tag != 0)goto _LL3B;}_LL3A:
# 198
 goto _LL3C;_LL3B: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp45=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp43;if(_tmp45->tag != 6)goto _LL3D;}_LL3C:
# 199
 goto _LL3E;_LL3D: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp46=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp43;if(_tmp46->tag != 7)goto _LL3F;}_LL3E:
# 200
 return;_LL3F: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp47=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp43;if(_tmp47->tag != 8)goto _LL41;}_LL40:
# 203
 return;_LL41: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp48=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp43;if(_tmp48->tag != 13)goto _LL43;else{_tmp49=_tmp48->f2;}}_LL42:
# 205
 s=_tmp49;
# 206
continue;_LL43: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp4A=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp43;if(_tmp4A->tag != 3)goto _LL45;else{_tmp4B=_tmp4A->f1;}}_LL44:
# 209
 if(_tmp4B == 0)
# 210
return;
# 211
_tmp4D=(struct Cyc_Absyn_Exp*)_tmp4B;goto _LL46;_LL45: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp4C=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp43;if(_tmp4C->tag != 1)goto _LL47;else{_tmp4D=_tmp4C->f1;}}_LL46:
# 213
 Cyc_Tovc_box_free_vars_exp(env,_tmp4D);
# 214
return;_LL47: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp4E=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp43;if(_tmp4E->tag != 2)goto _LL49;else{_tmp4F=_tmp4E->f1;_tmp50=_tmp4E->f2;}}_LL48:
# 216
 Cyc_Tovc_box_free_vars_stmt(env,_tmp4F);
# 217
s=_tmp50;
# 218
continue;_LL49: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp51=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp43;if(_tmp51->tag != 4)goto _LL4B;else{_tmp52=_tmp51->f1;_tmp53=_tmp51->f2;_tmp54=_tmp51->f3;}}_LL4A:
# 220
 Cyc_Tovc_box_free_vars_exp(env,_tmp52);
# 221
Cyc_Tovc_box_free_vars_stmt(env,_tmp53);
# 222
s=_tmp54;
# 223
continue;_LL4B: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp55=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp43;if(_tmp55->tag != 5)goto _LL4D;else{_tmp56=_tmp55->f1;_tmp57=_tmp56.f1;_tmp58=_tmp55->f2;}}_LL4C:
# 225
 Cyc_Tovc_box_free_vars_exp(env,_tmp57);
# 226
s=_tmp58;
# 227
continue;_LL4D: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp59=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp43;if(_tmp59->tag != 9)goto _LL4F;else{_tmp5A=_tmp59->f1;_tmp5B=_tmp59->f2;_tmp5C=_tmp5B.f1;_tmp5D=_tmp59->f3;_tmp5E=_tmp5D.f1;_tmp5F=_tmp59->f4;}}_LL4E:
# 229
 Cyc_Tovc_box_free_vars_exp(env,_tmp5A);
# 230
Cyc_Tovc_box_free_vars_exp(env,_tmp5C);
# 231
Cyc_Tovc_box_free_vars_exp(env,_tmp5E);
# 232
s=_tmp5F;
# 233
continue;_LL4F: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp60=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp43;if(_tmp60->tag != 14)goto _LL51;else{_tmp61=_tmp60->f1;_tmp62=_tmp60->f2;_tmp63=_tmp62.f1;}}_LL50:
# 235
 Cyc_Tovc_box_free_vars_exp(env,_tmp63);
# 236
s=_tmp61;
# 237
continue;_LL51: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp64=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp43;if(_tmp64->tag != 10)goto _LL53;else{_tmp65=_tmp64->f1;_tmp66=_tmp64->f2;}}_LL52:
# 240
 Cyc_Tovc_box_free_vars_exp(env,_tmp65);
# 241
for(0;_tmp66 != 0;_tmp66=_tmp66->tl){
# 242
Cyc_Tovc_box_free_vars_stmt(env,((struct Cyc_Absyn_Switch_clause*)_tmp66->hd)->body);}
# 243
return;_LL53: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp67=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp43;if(_tmp67->tag != 12)goto _LL55;else{_tmp68=_tmp67->f1;_tmp69=_tmp67->f2;}}_LL54:
# 245
{void*_tmp6A=_tmp68->r;struct Cyc_Absyn_Vardecl*_tmp6C;_LL58: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6B=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp6A;if(_tmp6B->tag != 0)goto _LL5A;else{_tmp6C=_tmp6B->f1;}}_LL59:
# 247
{struct Cyc_List_List*_tmp18B;env.boundvars=((_tmp18B=_region_malloc(env.rgn,sizeof(*_tmp18B)),((_tmp18B->hd=_tmp6C,((_tmp18B->tl=env.boundvars,_tmp18B))))));}
# 248
env.varmap=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _dyneither_ptr*))Cyc_Dict_rdelete_same)(env.varmap,(*_tmp6C->name).f2);
# 249
if(_tmp6C->initializer != 0)
# 250
Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp6C->initializer));
# 251
s=_tmp69;
# 252
continue;_LL5A:;_LL5B:
# 253
 goto _LL57;_LL57:;}
# 255
goto _LL56;_LL55:;_LL56: {
# 256
struct Cyc_Core_Impossible_exn_struct _tmp191;const char*_tmp190;struct Cyc_Core_Impossible_exn_struct*_tmp18F;(int)_throw((void*)((_tmp18F=_cycalloc(sizeof(*_tmp18F)),((_tmp18F[0]=((_tmp191.tag=Cyc_Core_Impossible,((_tmp191.f1=((_tmp190="bad stmt after xlation to C",_tag_dyneither(_tmp190,sizeof(char),28))),_tmp191)))),_tmp18F)))));}_LL38:;}}
# 267
static int Cyc_Tovc_stmt_to_fun_cmp(struct Cyc_Absyn_Vardecl*x,struct Cyc_Absyn_Vardecl*y){
# 268
return Cyc_Absyn_qvar_cmp(x->name,y->name);}
# 271
static struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_fun(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s,void*rettype){
# 274
struct Cyc_Set_Set*freevars=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))Cyc_Set_rempty)(env.rgn,Cyc_Tovc_stmt_to_fun_cmp);
# 275
struct Cyc_Tovc_BoxingEnv _tmp192;struct Cyc_Tovc_BoxingEnv _tmp71=(_tmp192.rgn=env.rgn,((_tmp192.all_locals=env.all_locals,((_tmp192.varmap=env.varmap,((_tmp192.boundvars=0,((_tmp192.freevars=& freevars,_tmp192)))))))));
# 276
Cyc_Tovc_box_free_vars_stmt(_tmp71,s);{
# 277
struct Cyc_List_List*params=0;
# 278
struct Cyc_List_List*args=0;
# 280
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,freevars);
# 282
struct Cyc_Absyn_Vardecl*_tmp193;struct Cyc_Absyn_Vardecl*_tmp72=(_tmp193=_cycalloc(sizeof(*_tmp193)),((_tmp193->sc=Cyc_Absyn_Public,((_tmp193->name=Cyc_Absyn_exn_name,((_tmp193->tq=Cyc_Absyn_empty_tqual(0),((_tmp193->type=(void*)& Cyc_Absyn_VoidType_val,((_tmp193->initializer=0,((_tmp193->rgn=0,((_tmp193->attributes=0,((_tmp193->escapes=0,_tmp193)))))))))))))))));
# 285
while(((int(*)(struct Cyc_Iter_Iter,struct Cyc_Absyn_Vardecl**))Cyc_Iter_next)(iter,& _tmp72)){
# 286
void*_tmp73=_tmp72->type;struct Cyc_Absyn_ArrayInfo _tmp75;void*_tmp76;_LL5D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp74=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp73;if(_tmp74->tag != 8)goto _LL5F;else{_tmp75=_tmp74->f1;_tmp76=_tmp75.elt_type;}}_LL5E:
# 288
{struct _tuple8*_tmp196;struct Cyc_List_List*_tmp195;params=((_tmp195=_cycalloc(sizeof(*_tmp195)),((_tmp195->hd=((_tmp196=_cycalloc(sizeof(*_tmp196)),((_tmp196->f1=(*_tmp72->name).f2,((_tmp196->f2=_tmp72->tq,((_tmp196->f3=
# 289
Cyc_Absyn_cstar_typ(_tmp76,Cyc_Absyn_empty_tqual(0)),_tmp196)))))))),((_tmp195->tl=params,_tmp195))))));}
# 291
{struct Cyc_List_List*_tmp197;args=((_tmp197=_cycalloc(sizeof(*_tmp197)),((_tmp197->hd=Cyc_Absyn_var_exp(_tmp72->name,0),((_tmp197->tl=args,_tmp197))))));}
# 292
goto _LL5C;_LL5F:;_LL60:
# 294
{struct _tuple8*_tmp19A;struct Cyc_List_List*_tmp199;params=((_tmp199=_cycalloc(sizeof(*_tmp199)),((_tmp199->hd=((_tmp19A=_cycalloc(sizeof(*_tmp19A)),((_tmp19A->f1=(*_tmp72->name).f2,((_tmp19A->f2=_tmp72->tq,((_tmp19A->f3=
# 295
Cyc_Absyn_cstar_typ(_tmp72->type,Cyc_Absyn_empty_tqual(0)),_tmp19A)))))))),((_tmp199->tl=params,_tmp199))))));}
# 297
{struct Cyc_List_List*_tmp19B;args=((_tmp19B=_cycalloc(sizeof(*_tmp19B)),((_tmp19B->hd=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(_tmp72->name,0),0),((_tmp19B->tl=args,_tmp19B))))));}
# 298
goto _LL5C;_LL5C:;}{
# 300
struct _tuple0*funname=Cyc_Toc_temp_var();
# 301
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp1A5;struct Cyc_Absyn_Fndecl*_tmp1A4;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmp1A3;struct Cyc_List_List*_tmp1A2;*env.gen_ds=((_tmp1A2=_cycalloc(sizeof(*_tmp1A2)),((_tmp1A2->hd=Cyc_Absyn_new_decl((void*)((_tmp1A5=_cycalloc(sizeof(*_tmp1A5)),((_tmp1A5[0]=((_tmp1A3.tag=1,((_tmp1A3.f1=((_tmp1A4=_cycalloc(sizeof(*_tmp1A4)),((_tmp1A4->sc=Cyc_Absyn_Static,((_tmp1A4->is_inline=0,((_tmp1A4->name=funname,((_tmp1A4->tvs=0,((_tmp1A4->effect=0,((_tmp1A4->ret_tqual=
# 302
Cyc_Absyn_empty_tqual(0),((_tmp1A4->ret_type=rettype,((_tmp1A4->args=params,((_tmp1A4->c_varargs=0,((_tmp1A4->cyc_varargs=0,((_tmp1A4->rgn_po=0,((_tmp1A4->body=s,((_tmp1A4->cached_typ=0,((_tmp1A4->param_vardecls=0,((_tmp1A4->fn_vardecl=0,((_tmp1A4->attributes=0,_tmp1A4)))))))))))))))))))))))))))))))))),_tmp1A3)))),_tmp1A5)))),0),((_tmp1A2->tl=*env.gen_ds,_tmp1A2))))));}
# 307
return Cyc_Absyn_fncall_exp(Cyc_Absyn_var_exp(funname,0),args,0);};};}
# 310
static void Cyc_Tovc_apply_varmap(struct Cyc_Dict_Dict varmap,struct Cyc_Absyn_Exp*e){
# 311
void*_tmp83=e->r;struct _tuple0*_tmp85;struct _tuple0 _tmp86;union Cyc_Absyn_Nmspace _tmp87;int _tmp88;struct _dyneither_ptr*_tmp89;struct Cyc_Absyn_Exp*_tmp8D;struct Cyc_List_List*_tmp8E;struct Cyc_List_List*_tmp90;struct Cyc_Absyn_Exp*_tmp92;struct Cyc_Absyn_Exp*_tmp93;struct Cyc_Absyn_Exp*_tmp94;struct Cyc_Absyn_Exp*_tmp96;struct Cyc_Absyn_Exp*_tmp97;struct Cyc_Absyn_Exp*_tmp99;struct Cyc_Absyn_Exp*_tmp9A;struct Cyc_Absyn_Exp*_tmp9C;struct Cyc_Absyn_Exp*_tmp9D;struct Cyc_Absyn_Exp*_tmp9F;struct Cyc_Absyn_Exp*_tmpA0;struct Cyc_Absyn_Exp*_tmpA2;struct Cyc_Absyn_Exp*_tmpA3;struct Cyc_Absyn_Exp*_tmpA5;struct Cyc_Absyn_Exp*_tmpA7;struct Cyc_Absyn_Exp*_tmpA9;struct Cyc_Absyn_Exp*_tmpAB;struct Cyc_Absyn_Exp*_tmpAD;struct Cyc_Absyn_Exp*_tmpAF;struct Cyc_Absyn_Exp*_tmpB1;struct Cyc_Absyn_Exp*_tmpB3;struct Cyc_Absyn_Exp*_tmpB5;struct Cyc_List_List*_tmpBA;_LL62: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp84=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp83;if(_tmp84->tag != 1)goto _LL64;else{_tmp85=_tmp84->f1;_tmp86=*_tmp85;_tmp87=_tmp86.f1;if((_tmp87.Loc_n).tag != 4)goto _LL64;_tmp88=(int)(_tmp87.Loc_n).val;_tmp89=_tmp86.f2;}}_LL63:
# 315
 if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(varmap,_tmp89))
# 316
e->r=(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(varmap,_tmp89))->r;
# 317
return;_LL64: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp8A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp83;if(_tmp8A->tag != 1)goto _LL66;}_LL65:
# 318
 return;_LL66: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp8B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp83;if(_tmp8B->tag != 0)goto _LL68;}_LL67:
# 321
 return;_LL68: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp8C=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp83;if(_tmp8C->tag != 9)goto _LL6A;else{_tmp8D=_tmp8C->f1;_tmp8E=_tmp8C->f2;}}_LL69:
# 323
 Cyc_Tovc_apply_varmap(varmap,_tmp8D);_tmp90=_tmp8E;goto _LL6B;_LL6A: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp8F=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp83;if(_tmp8F->tag != 2)goto _LL6C;else{_tmp90=_tmp8F->f2;}}_LL6B:
# 325
 for(0;_tmp90 != 0;_tmp90=_tmp90->tl){
# 326
Cyc_Tovc_apply_varmap(varmap,(struct Cyc_Absyn_Exp*)_tmp90->hd);}
# 327
return;_LL6C: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp91=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp83;if(_tmp91->tag != 5)goto _LL6E;else{_tmp92=_tmp91->f1;_tmp93=_tmp91->f2;_tmp94=_tmp91->f3;}}_LL6D:
# 330
 Cyc_Tovc_apply_varmap(varmap,_tmp94);_tmp96=_tmp92;_tmp97=_tmp93;goto _LL6F;_LL6E: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp95=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp83;if(_tmp95->tag != 6)goto _LL70;else{_tmp96=_tmp95->f1;_tmp97=_tmp95->f2;}}_LL6F:
# 331
 _tmp99=_tmp96;_tmp9A=_tmp97;goto _LL71;_LL70: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp98=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp83;if(_tmp98->tag != 7)goto _LL72;else{_tmp99=_tmp98->f1;_tmp9A=_tmp98->f2;}}_LL71:
# 332
 _tmp9C=_tmp99;_tmp9D=_tmp9A;goto _LL73;_LL72: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp9B=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp83;if(_tmp9B->tag != 22)goto _LL74;else{_tmp9C=_tmp9B->f1;_tmp9D=_tmp9B->f2;}}_LL73:
# 333
 _tmp9F=_tmp9C;_tmpA0=_tmp9D;goto _LL75;_LL74: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp9E=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp83;if(_tmp9E->tag != 8)goto _LL76;else{_tmp9F=_tmp9E->f1;_tmpA0=_tmp9E->f2;}}_LL75:
# 334
 _tmpA2=_tmp9F;_tmpA3=_tmpA0;goto _LL77;_LL76: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpA1=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp83;if(_tmpA1->tag != 3)goto _LL78;else{_tmpA2=_tmpA1->f1;_tmpA3=_tmpA1->f3;}}_LL77:
# 336
 Cyc_Tovc_apply_varmap(varmap,_tmpA3);_tmpA5=_tmpA2;goto _LL79;_LL78: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpA4=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp83;if(_tmpA4->tag != 11)goto _LL7A;else{_tmpA5=_tmpA4->f1;}}_LL79:
# 338
 _tmpA7=_tmpA5;goto _LL7B;_LL7A: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpA6=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp83;if(_tmpA6->tag != 12)goto _LL7C;else{_tmpA7=_tmpA6->f1;}}_LL7B:
# 339
 _tmpA9=_tmpA7;goto _LL7D;_LL7C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpA8=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp83;if(_tmpA8->tag != 13)goto _LL7E;else{_tmpA9=_tmpA8->f2;}}_LL7D:
# 340
 _tmpAB=_tmpA9;goto _LL7F;_LL7E: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmpAA=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp83;if(_tmpAA->tag != 14)goto _LL80;else{_tmpAB=_tmpAA->f1;}}_LL7F:
# 341
 _tmpAD=_tmpAB;goto _LL81;_LL80: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpAC=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp83;if(_tmpAC->tag != 17)goto _LL82;else{_tmpAD=_tmpAC->f1;}}_LL81:
# 342
 _tmpAF=_tmpAD;goto _LL83;_LL82: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpAE=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp83;if(_tmpAE->tag != 19)goto _LL84;else{_tmpAF=_tmpAE->f1;}}_LL83:
# 343
 _tmpB1=_tmpAF;goto _LL85;_LL84: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpB0=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp83;if(_tmpB0->tag != 20)goto _LL86;else{_tmpB1=_tmpB0->f1;}}_LL85:
# 344
 _tmpB3=_tmpB1;goto _LL87;_LL86: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpB2=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp83;if(_tmpB2->tag != 21)goto _LL88;else{_tmpB3=_tmpB2->f1;}}_LL87:
# 345
 _tmpB5=_tmpB3;goto _LL89;_LL88: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpB4=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp83;if(_tmpB4->tag != 4)goto _LL8A;else{_tmpB5=_tmpB4->f1;}}_LL89:
# 347
 Cyc_Tovc_apply_varmap(varmap,_tmpB5);
# 348
return;_LL8A: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpB6=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp83;if(_tmpB6->tag != 16)goto _LL8C;}_LL8B:
# 350
 goto _LL8D;_LL8C: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpB7=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp83;if(_tmpB7->tag != 18)goto _LL8E;}_LL8D:
# 351
 goto _LL8F;_LL8E: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpB8=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp83;if(_tmpB8->tag != 30)goto _LL90;}_LL8F:
# 352
 return;_LL90: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpB9=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp83;if(_tmpB9->tag != 34)goto _LL92;else{_tmpBA=_tmpB9->f2;}}_LL91:
# 355
 for(0;_tmpBA != 0;_tmpBA=_tmpBA->tl){
# 356
Cyc_Tovc_apply_varmap(varmap,(*((struct _tuple10*)_tmpBA->hd)).f2);}
# 357
return;_LL92:;_LL93: {
# 359
struct Cyc_Core_Impossible_exn_struct _tmp1AB;const char*_tmp1AA;struct Cyc_Core_Impossible_exn_struct*_tmp1A9;(int)_throw((void*)((_tmp1A9=_cycalloc(sizeof(*_tmp1A9)),((_tmp1A9[0]=((_tmp1AB.tag=Cyc_Core_Impossible,((_tmp1AB.f1=((_tmp1AA="bad exp form in Tocv::apply_varmap",_tag_dyneither(_tmp1AA,sizeof(char),35))),_tmp1AB)))),_tmp1A9)))));}_LL61:;}
# 363
static struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_exp(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s){
# 364
void*_tmpBE=s->r;struct Cyc_Absyn_Exp*_tmpC1;struct Cyc_Absyn_Stmt*_tmpC3;struct Cyc_Absyn_Stmt*_tmpC4;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_Absyn_Stmt*_tmpC7;struct Cyc_Absyn_Stmt*_tmpC8;struct Cyc_Absyn_Decl*_tmpCA;struct Cyc_Absyn_Stmt*_tmpCB;_LL95: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmpBF=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmpBE;if(_tmpBF->tag != 0)goto _LL97;}_LL96:
# 365
 return Cyc_Absyn_true_exp(0);_LL97: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpC0=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpBE;if(_tmpC0->tag != 1)goto _LL99;else{_tmpC1=_tmpC0->f1;}}_LL98:
# 367
 Cyc_Tovc_apply_varmap(env.varmap,_tmpC1);
# 368
return _tmpC1;_LL99: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpC2=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpBE;if(_tmpC2->tag != 2)goto _LL9B;else{_tmpC3=_tmpC2->f1;_tmpC4=_tmpC2->f2;}}_LL9A: {
# 371
struct Cyc_Absyn_Exp*_tmpCC=Cyc_Tovc_stmt_to_exp(env,_tmpC3);
# 372
struct Cyc_Absyn_Exp*_tmpCD=Cyc_Tovc_stmt_to_exp(env,_tmpC4);
# 373
return Cyc_Absyn_seq_exp(_tmpCC,_tmpCD,0);}_LL9B: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmpC5=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpBE;if(_tmpC5->tag != 4)goto _LL9D;else{_tmpC6=_tmpC5->f1;_tmpC7=_tmpC5->f2;_tmpC8=_tmpC5->f3;}}_LL9C:
# 375
 Cyc_Tovc_apply_varmap(env.varmap,_tmpC6);{
# 377
struct Cyc_Absyn_Exp*_tmpCE=Cyc_Tovc_stmt_to_exp(env,_tmpC7);
# 378
struct Cyc_Absyn_Exp*_tmpCF=Cyc_Tovc_stmt_to_exp(env,_tmpC8);
# 379
return Cyc_Absyn_conditional_exp(_tmpC6,_tmpCE,_tmpCF,0);};_LL9D: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpC9=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpBE;if(_tmpC9->tag != 12)goto _LL9F;else{_tmpCA=_tmpC9->f1;_tmpCB=_tmpC9->f2;}}_LL9E: {
# 381
void*_tmpD0=_tmpCA->r;struct Cyc_Absyn_Vardecl*_tmpD2;_LLA2: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD1=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpD0;if(_tmpD1->tag != 0)goto _LLA4;else{_tmpD2=_tmpD1->f1;}}_LLA3: {
# 384
struct _dyneither_ptr*_tmpD3=(*_tmpD2->name).f2;
# 385
struct _tuple0*_tmpD4=Cyc_Toc_temp_var();
# 386
_tmpD2->name=_tmpD4;
# 387
env.varmap=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(env.varmap,_tmpD3,Cyc_Absyn_var_exp(_tmpD4,0));{
# 388
struct Cyc_List_List*_tmpD5=0;
# 389
if(_tmpD2->initializer != 0){
# 390
Cyc_Tovc_apply_varmap(env.varmap,(struct Cyc_Absyn_Exp*)_check_null(_tmpD2->initializer));{
# 391
void*_tmpD6=((struct Cyc_Absyn_Exp*)_check_null(_tmpD2->initializer))->r;struct Cyc_List_List*_tmpD8;_LLA7: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpD7=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpD6;if(_tmpD7->tag != 34)goto _LLA9;else{_tmpD8=_tmpD7->f2;}}_LLA8: {
# 403 "tovc.cyc"
struct Cyc_Absyn_Exp*_tmpD9=Cyc_Absyn_var_exp(_tmpD2->name,0);
# 404
{unsigned int i=0;for(0;_tmpD8 != 0;(_tmpD8=_tmpD8->tl,++ i)){
# 405
struct Cyc_List_List*_tmp1AC;_tmpD5=((_tmp1AC=_cycalloc(sizeof(*_tmp1AC)),((_tmp1AC->hd=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(_tmpD9,Cyc_Absyn_uint_exp(i,0),0),(*((struct _tuple10*)_tmpD8->hd)).f2,0),((_tmp1AC->tl=_tmpD5,_tmp1AC))))));}}
# 409
_tmpD2->initializer=0;
# 410
goto _LLA6;}_LLA9:;_LLAA:
# 411
 goto _LLA6;_LLA6:;};}
# 414
{struct Cyc_List_List*_tmp1AD;env.all_locals=((_tmp1AD=_region_malloc(env.rgn,sizeof(*_tmp1AD)),((_tmp1AD->hd=_tmpD2,((_tmp1AD->tl=env.all_locals,_tmp1AD))))));}
# 416
(env.encloser)->r=(Cyc_Absyn_decl_stmt(_tmpCA,Cyc_Absyn_new_stmt((env.encloser)->r,0),0))->r;{
# 417
struct Cyc_Absyn_Exp*_tmpDC=Cyc_Tovc_stmt_to_exp(env,_tmpCB);
# 418
if(_tmpD2->initializer != 0){
# 419
_tmpDC=Cyc_Absyn_seq_exp(Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmpD2->name,0),(struct Cyc_Absyn_Exp*)_check_null(_tmpD2->initializer),0),_tmpDC,0);
# 421
_tmpD2->initializer=0;}
# 423
for(0;_tmpD5 != 0;_tmpD5=_tmpD5->tl){
# 424
_tmpDC=Cyc_Absyn_seq_exp((struct Cyc_Absyn_Exp*)_tmpD5->hd,_tmpDC,0);}
# 425
return _tmpDC;};};}_LLA4:;_LLA5: {
# 426
struct Cyc_Core_Impossible_exn_struct _tmp1B3;const char*_tmp1B2;struct Cyc_Core_Impossible_exn_struct*_tmp1B1;(int)_throw((void*)((_tmp1B1=_cycalloc(sizeof(*_tmp1B1)),((_tmp1B1[0]=((_tmp1B3.tag=Cyc_Core_Impossible,((_tmp1B3.f1=((_tmp1B2="bad local decl in Tovc::stmt_to_exp",_tag_dyneither(_tmp1B2,sizeof(char),36))),_tmp1B3)))),_tmp1B1)))));}_LLA1:;}_LL9F:;_LLA0:
# 429
 return Cyc_Tovc_stmt_to_fun(env,s,(void*)& Cyc_Absyn_VoidType_val);_LL94:;}
# 433
static void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s);
# 435
static void Cyc_Tovc_exp_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Exp*e){
# 436
void*_tmpE0=e->r;struct Cyc_Absyn_Exp*_tmpE4;struct Cyc_List_List*_tmpE5;struct Cyc_List_List*_tmpE7;struct Cyc_Absyn_Exp*_tmpE9;struct Cyc_Absyn_Exp*_tmpEA;struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_Absyn_Exp*_tmpED;struct Cyc_Absyn_Exp*_tmpEE;struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_Absyn_Exp*_tmpF1;struct Cyc_Absyn_Exp*_tmpF3;struct Cyc_Absyn_Exp*_tmpF4;struct Cyc_Absyn_Exp*_tmpF6;struct Cyc_Absyn_Exp*_tmpF7;struct Cyc_Absyn_Exp*_tmpF9;struct Cyc_Absyn_Exp*_tmpFA;struct Cyc_Absyn_Exp*_tmpFC;struct Cyc_Absyn_Exp*_tmpFE;struct Cyc_Absyn_Exp*_tmp100;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Absyn_Exp*_tmp104;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Absyn_Exp*_tmp108;struct Cyc_Absyn_Exp*_tmp10A;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_List_List*_tmp111;struct Cyc_Absyn_Stmt*_tmp113;_LLAC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmpE1->tag != 0)goto _LLAE;}_LLAD:
# 437
 return;_LLAE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpE2=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmpE2->tag != 1)goto _LLB0;}_LLAF:
# 438
 return;_LLB0: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE3=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmpE3->tag != 9)goto _LLB2;else{_tmpE4=_tmpE3->f1;_tmpE5=_tmpE3->f2;}}_LLB1:
# 441
 Cyc_Tovc_exp_to_vc(env,_tmpE4);_tmpE7=_tmpE5;goto _LLB3;_LLB2: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpE6=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmpE6->tag != 2)goto _LLB4;else{_tmpE7=_tmpE6->f2;}}_LLB3:
# 443
 for(0;_tmpE7 != 0;_tmpE7=_tmpE7->tl){
# 444
Cyc_Tovc_exp_to_vc(env,(struct Cyc_Absyn_Exp*)_tmpE7->hd);}
# 445
return;_LLB4: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpE8=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmpE8->tag != 5)goto _LLB6;else{_tmpE9=_tmpE8->f1;_tmpEA=_tmpE8->f2;_tmpEB=_tmpE8->f3;}}_LLB5:
# 448
 Cyc_Tovc_exp_to_vc(env,_tmpEB);_tmpED=_tmpE9;_tmpEE=_tmpEA;goto _LLB7;_LLB6: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpEC=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmpEC->tag != 6)goto _LLB8;else{_tmpED=_tmpEC->f1;_tmpEE=_tmpEC->f2;}}_LLB7:
# 449
 _tmpF0=_tmpED;_tmpF1=_tmpEE;goto _LLB9;_LLB8: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpEF=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmpEF->tag != 7)goto _LLBA;else{_tmpF0=_tmpEF->f1;_tmpF1=_tmpEF->f2;}}_LLB9:
# 450
 _tmpF3=_tmpF0;_tmpF4=_tmpF1;goto _LLBB;_LLBA: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpF2=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmpF2->tag != 22)goto _LLBC;else{_tmpF3=_tmpF2->f1;_tmpF4=_tmpF2->f2;}}_LLBB:
# 451
 _tmpF6=_tmpF3;_tmpF7=_tmpF4;goto _LLBD;_LLBC: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpF5=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmpF5->tag != 8)goto _LLBE;else{_tmpF6=_tmpF5->f1;_tmpF7=_tmpF5->f2;}}_LLBD:
# 452
 _tmpF9=_tmpF6;_tmpFA=_tmpF7;goto _LLBF;_LLBE: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpF8=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmpF8->tag != 3)goto _LLC0;else{_tmpF9=_tmpF8->f1;_tmpFA=_tmpF8->f3;}}_LLBF:
# 454
 Cyc_Tovc_exp_to_vc(env,_tmpFA);_tmpFC=_tmpF9;goto _LLC1;_LLC0: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpFB=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmpFB->tag != 11)goto _LLC2;else{_tmpFC=_tmpFB->f1;}}_LLC1:
# 456
 _tmpFE=_tmpFC;goto _LLC3;_LLC2: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpFD=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmpFD->tag != 12)goto _LLC4;else{_tmpFE=_tmpFD->f1;}}_LLC3:
# 457
 _tmp100=_tmpFE;goto _LLC5;_LLC4: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpFF=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmpFF->tag != 13)goto _LLC6;else{_tmp100=_tmpFF->f2;}}_LLC5:
# 458
 _tmp102=_tmp100;goto _LLC7;_LLC6: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp101=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmp101->tag != 14)goto _LLC8;else{_tmp102=_tmp101->f1;}}_LLC7:
# 459
 _tmp104=_tmp102;goto _LLC9;_LLC8: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp103=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmp103->tag != 17)goto _LLCA;else{_tmp104=_tmp103->f1;}}_LLC9:
# 460
 _tmp106=_tmp104;goto _LLCB;_LLCA: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp105=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmp105->tag != 19)goto _LLCC;else{_tmp106=_tmp105->f1;}}_LLCB:
# 461
 _tmp108=_tmp106;goto _LLCD;_LLCC: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp107=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmp107->tag != 20)goto _LLCE;else{_tmp108=_tmp107->f1;}}_LLCD:
# 462
 _tmp10A=_tmp108;goto _LLCF;_LLCE: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp109=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmp109->tag != 21)goto _LLD0;else{_tmp10A=_tmp109->f1;}}_LLCF:
# 463
 _tmp10C=_tmp10A;goto _LLD1;_LLD0: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp10B=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmp10B->tag != 4)goto _LLD2;else{_tmp10C=_tmp10B->f1;}}_LLD1:
# 465
 Cyc_Tovc_exp_to_vc(env,_tmp10C);
# 466
return;_LLD2: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp10D=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmp10D->tag != 16)goto _LLD4;}_LLD3:
# 468
 goto _LLD5;_LLD4: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp10E=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmp10E->tag != 18)goto _LLD6;}_LLD5:
# 469
 goto _LLD7;_LLD6: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp10F=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmp10F->tag != 30)goto _LLD8;}_LLD7:
# 470
 return;_LLD8: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp110=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmp110->tag != 34)goto _LLDA;else{_tmp111=_tmp110->f2;}}_LLD9:
# 476
 for(0;_tmp111 != 0;_tmp111=_tmp111->tl){
# 477
Cyc_Tovc_exp_to_vc(env,(*((struct _tuple10*)_tmp111->hd)).f2);}
# 478
return;_LLDA: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp112=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmp112->tag != 35)goto _LLDC;else{_tmp113=_tmp112->f1;}}_LLDB:
# 484
 Cyc_Tovc_stmt_to_vc(env,_tmp113);
# 486
e->r=(Cyc_Tovc_stmt_to_exp(env,_tmp113))->r;
# 487
return;_LLDC: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp114=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmp114->tag != 38)goto _LLDE;}_LLDD:
# 489
 return;_LLDE:;_LLDF: {
# 491
struct Cyc_Core_Impossible_exn_struct _tmp1B9;const char*_tmp1B8;struct Cyc_Core_Impossible_exn_struct*_tmp1B7;(int)_throw((void*)((_tmp1B7=_cycalloc(sizeof(*_tmp1B7)),((_tmp1B7[0]=((_tmp1B9.tag=Cyc_Core_Impossible,((_tmp1B9.f1=((_tmp1B8="bad exp form after xlation to C",_tag_dyneither(_tmp1B8,sizeof(char),32))),_tmp1B9)))),_tmp1B7)))));}_LLAB:;}
# 495
static void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s){
# 497
while(1){
# 498
env.encloser=s;{
# 499
void*_tmp118=s->r;struct Cyc_Absyn_Stmt*_tmp11E;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Stmt*_tmp124;struct Cyc_Absyn_Stmt*_tmp125;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Stmt*_tmp128;struct Cyc_Absyn_Stmt*_tmp129;struct _tuple9 _tmp12B;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Stmt*_tmp12D;struct Cyc_Absyn_Exp*_tmp12F;struct _tuple9 _tmp130;struct Cyc_Absyn_Exp*_tmp131;struct _tuple9 _tmp132;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Stmt*_tmp134;struct Cyc_Absyn_Stmt*_tmp136;struct _tuple9 _tmp137;struct Cyc_Absyn_Exp*_tmp138;struct Cyc_Absyn_Exp*_tmp13A;struct Cyc_List_List*_tmp13B;struct Cyc_Absyn_Decl*_tmp13D;struct Cyc_Absyn_Stmt*_tmp13E;_LLE1: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp119=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp118;if(_tmp119->tag != 0)goto _LLE3;}_LLE2:
# 500
 goto _LLE4;_LLE3: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp11A=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp118;if(_tmp11A->tag != 6)goto _LLE5;}_LLE4:
# 501
 goto _LLE6;_LLE5: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp11B=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp118;if(_tmp11B->tag != 7)goto _LLE7;}_LLE6:
# 502
 goto _LLE8;_LLE7: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp11C=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp118;if(_tmp11C->tag != 8)goto _LLE9;}_LLE8:
# 503
 return;_LLE9: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp11D=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp118;if(_tmp11D->tag != 13)goto _LLEB;else{_tmp11E=_tmp11D->f2;}}_LLEA:
# 505
 s=_tmp11E;
# 506
continue;_LLEB: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp11F=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp118;if(_tmp11F->tag != 3)goto _LLED;else{_tmp120=_tmp11F->f1;}}_LLEC:
# 508
 if(_tmp120 == 0)
# 509
return;
# 510
_tmp122=(struct Cyc_Absyn_Exp*)_tmp120;goto _LLEE;_LLED: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp121=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp118;if(_tmp121->tag != 1)goto _LLEF;else{_tmp122=_tmp121->f1;}}_LLEE:
# 512
 Cyc_Tovc_exp_to_vc(env,_tmp122);
# 513
return;_LLEF: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp123=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp118;if(_tmp123->tag != 2)goto _LLF1;else{_tmp124=_tmp123->f1;_tmp125=_tmp123->f2;}}_LLF0:
# 515
 Cyc_Tovc_stmt_to_vc(env,_tmp124);
# 516
s=_tmp125;
# 517
continue;_LLF1: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp126=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp118;if(_tmp126->tag != 4)goto _LLF3;else{_tmp127=_tmp126->f1;_tmp128=_tmp126->f2;_tmp129=_tmp126->f3;}}_LLF2:
# 519
 Cyc_Tovc_exp_to_vc(env,_tmp127);
# 520
Cyc_Tovc_stmt_to_vc(env,_tmp128);
# 521
s=_tmp129;
# 522
continue;_LLF3: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp12A=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp118;if(_tmp12A->tag != 5)goto _LLF5;else{_tmp12B=_tmp12A->f1;_tmp12C=_tmp12B.f1;_tmp12D=_tmp12A->f2;}}_LLF4:
# 524
 Cyc_Tovc_exp_to_vc(env,_tmp12C);
# 525
s=_tmp12D;
# 526
continue;_LLF5: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp12E=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp118;if(_tmp12E->tag != 9)goto _LLF7;else{_tmp12F=_tmp12E->f1;_tmp130=_tmp12E->f2;_tmp131=_tmp130.f1;_tmp132=_tmp12E->f3;_tmp133=_tmp132.f1;_tmp134=_tmp12E->f4;}}_LLF6:
# 528
 Cyc_Tovc_exp_to_vc(env,_tmp12F);
# 529
Cyc_Tovc_exp_to_vc(env,_tmp131);
# 530
Cyc_Tovc_exp_to_vc(env,_tmp133);
# 531
s=_tmp134;
# 532
continue;_LLF7: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp135=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp118;if(_tmp135->tag != 14)goto _LLF9;else{_tmp136=_tmp135->f1;_tmp137=_tmp135->f2;_tmp138=_tmp137.f1;}}_LLF8:
# 534
 Cyc_Tovc_exp_to_vc(env,_tmp138);
# 535
s=_tmp136;
# 536
continue;_LLF9: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp139=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp118;if(_tmp139->tag != 10)goto _LLFB;else{_tmp13A=_tmp139->f1;_tmp13B=_tmp139->f2;}}_LLFA:
# 540
 Cyc_Tovc_exp_to_vc(env,_tmp13A);
# 541
for(0;_tmp13B != 0;_tmp13B=_tmp13B->tl){
# 542
Cyc_Tovc_stmt_to_vc(env,((struct Cyc_Absyn_Switch_clause*)_tmp13B->hd)->body);}
# 543
return;_LLFB: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp13C=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp118;if(_tmp13C->tag != 12)goto _LLFD;else{_tmp13D=_tmp13C->f1;_tmp13E=_tmp13C->f2;}}_LLFC:
# 545
{void*_tmp13F=_tmp13D->r;struct Cyc_Absyn_Vardecl*_tmp141;_LL100: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp140=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp13F;if(_tmp140->tag != 0)goto _LL102;else{_tmp141=_tmp140->f1;}}_LL101:
# 547
 if(Cyc_Tovc_elim_array_initializers){
# 552
void*_tmp142=Cyc_Tcutil_compress(_tmp141->type);_LL105: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp143=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp142;if(_tmp143->tag != 8)goto _LL107;}_LL106:
# 554
 if(_tmp141->initializer != 0){
# 555
void*_tmp144=((struct Cyc_Absyn_Exp*)_check_null(_tmp141->initializer))->r;struct Cyc_List_List*_tmp146;_LL10A: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp145=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp144;if(_tmp145->tag != 34)goto _LL10C;else{_tmp146=_tmp145->f2;}}_LL10B:
# 557
 _tmp141->initializer=0;{
# 559
struct Cyc_Absyn_Exp*arr=Cyc_Absyn_var_exp(_tmp141->name,0);
# 560
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_new_stmt(_tmp13E->r,0);
# 561
{unsigned int i=0;for(0;_tmp146 != 0;(_tmp146=_tmp146->tl,++ i)){
# 562
s3=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(arr,
# 563
Cyc_Absyn_uint_exp(i,0),0),(*((struct _tuple10*)_tmp146->hd)).f2,0),0),s3,0);}}
# 567
_tmp13E->r=s3->r;
# 568
goto _LL109;};_LL10C:;_LL10D: {
# 570
const char*_tmp1BD;void*_tmp1BC[1];struct Cyc_String_pa_PrintArg_struct _tmp1BB;struct _dyneither_ptr _tmp147=(_tmp1BB.tag=0,((_tmp1BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 571
Cyc_Absynpp_stmt2string(s)),((_tmp1BC[0]=& _tmp1BB,Cyc_aprintf(((_tmp1BD="|%s|: array type bad initializer",_tag_dyneither(_tmp1BD,sizeof(char),33))),_tag_dyneither(_tmp1BC,sizeof(void*),1)))))));
# 572
struct Cyc_Core_Impossible_exn_struct _tmp1C0;struct Cyc_Core_Impossible_exn_struct*_tmp1BF;(int)_throw((void*)((_tmp1BF=_cycalloc(sizeof(*_tmp1BF)),((_tmp1BF[0]=((_tmp1C0.tag=Cyc_Core_Impossible,((_tmp1C0.f1=(struct _dyneither_ptr)_tmp147,_tmp1C0)))),_tmp1BF)))));}_LL109:;}
# 574
goto _LL104;_LL107:;_LL108:
# 575
 goto _LL104;_LL104:;}
# 578
if(_tmp141->initializer != 0)
# 579
Cyc_Tovc_exp_to_vc(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp141->initializer));
# 580
{struct Cyc_List_List*_tmp1C1;env.all_locals=((_tmp1C1=_region_malloc(env.rgn,sizeof(*_tmp1C1)),((_tmp1C1->hd=_tmp141,((_tmp1C1->tl=env.all_locals,_tmp1C1))))));}
# 581
s=_tmp13E;
# 582
continue;_LL102:;_LL103:
# 583
 goto _LLFF;_LLFF:;}
# 585
goto _LLFE;_LLFD:;_LLFE: {
# 586
struct Cyc_Core_Impossible_exn_struct _tmp1C7;const char*_tmp1C6;struct Cyc_Core_Impossible_exn_struct*_tmp1C5;(int)_throw((void*)((_tmp1C5=_cycalloc(sizeof(*_tmp1C5)),((_tmp1C5[0]=((_tmp1C7.tag=Cyc_Core_Impossible,((_tmp1C7.f1=((_tmp1C6="bad stmt after xlation to C",_tag_dyneither(_tmp1C6,sizeof(char),28))),_tmp1C7)))),_tmp1C5)))));}_LLE0:;};}}
# 591
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*old_ds){
# 592
struct Cyc_List_List*_tmp151=0;
# 593
for(0;old_ds != 0;old_ds=old_ds->tl){
# 594
struct Cyc_Absyn_Decl*_tmp152=(struct Cyc_Absyn_Decl*)old_ds->hd;
# 595
void*_tmp153=_tmp152->r;struct Cyc_Absyn_Fndecl*_tmp155;struct Cyc_List_List*_tmp160;struct Cyc_List_List*_tmp162;struct Cyc_List_List*_tmp164;struct Cyc_List_List*_tmp166;_LL10F: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp154=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp153;if(_tmp154->tag != 1)goto _LL111;else{_tmp155=_tmp154->f1;}}_LL110:
# 600
 _tmp155->cached_typ=0;
# 607
if(_tmp155->param_vardecls != 0){
# 608
struct _RegionHandle _tmp169=_new_region("rgn");struct _RegionHandle*rgn=& _tmp169;_push_region(rgn);
# 609
{struct Cyc_List_List**_tmp1C8;struct Cyc_List_List**_tmp16A=(_tmp1C8=_region_malloc(rgn,sizeof(*_tmp1C8)),((_tmp1C8[0]=0,_tmp1C8)));
# 610
if(_tmp155->param_vardecls == 0){
# 611
{const char*_tmp1CC;void*_tmp1CB[1];struct Cyc_String_pa_PrintArg_struct _tmp1CA;(_tmp1CA.tag=0,((_tmp1CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp155->name).f2),((_tmp1CB[0]=& _tmp1CA,Cyc_fprintf(Cyc_stderr,((_tmp1CC="No param vardecls for function %s\n",_tag_dyneither(_tmp1CC,sizeof(char),35))),_tag_dyneither(_tmp1CB,sizeof(void*),1)))))));}
# 612
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}
# 614
{struct Cyc_Tovc_ToExpEnv _tmp1CD;Cyc_Tovc_stmt_to_vc(((_tmp1CD.rgn=rgn,((_tmp1CD.all_locals=
# 615
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(rgn,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp155->param_vardecls))->v),((_tmp1CD.varmap=
# 616
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(rgn,Cyc_strptrcmp),((_tmp1CD.encloser=_tmp155->body,((_tmp1CD.gen_ds=_tmp16A,_tmp1CD)))))))))),_tmp155->body);}
# 620
if(*_tmp16A != 0){
# 621
struct Cyc_Absyn_Vardecl*_tmp1CE;struct Cyc_Absyn_Vardecl*vd=(_tmp1CE=_cycalloc(sizeof(*_tmp1CE)),((_tmp1CE->sc=_tmp155->sc,((_tmp1CE->name=_tmp155->name,((_tmp1CE->tq=Cyc_Absyn_const_tqual(0),((_tmp1CE->type=
# 622
Cyc_Tcutil_fndecl2typ(_tmp155),((_tmp1CE->initializer=0,((_tmp1CE->rgn=0,((_tmp1CE->attributes=_tmp155->attributes,((_tmp1CE->escapes=0,_tmp1CE)))))))))))))))));
# 623
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1D4;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp1D3;struct Cyc_List_List*_tmp1D2;_tmp151=((_tmp1D2=_cycalloc(sizeof(*_tmp1D2)),((_tmp1D2->hd=Cyc_Absyn_new_decl((void*)((_tmp1D4=_cycalloc(sizeof(*_tmp1D4)),((_tmp1D4[0]=((_tmp1D3.tag=0,((_tmp1D3.f1=vd,_tmp1D3)))),_tmp1D4)))),0),((_tmp1D2->tl=_tmp151,_tmp1D2))))));}
# 625
_tmp151=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(*_tmp16A,_tmp151);}}
# 609
;_pop_region(rgn);}
# 629
goto _LL112;_LL111: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp156=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp153;if(_tmp156->tag != 0)goto _LL113;}_LL112:
# 630
 goto _LL114;_LL113: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp157=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp153;if(_tmp157->tag != 2)goto _LL115;}_LL114:
# 631
 goto _LL116;_LL115: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp158=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp153;if(_tmp158->tag != 3)goto _LL117;}_LL116:
# 632
 goto _LL118;_LL117: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp159=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp153;if(_tmp159->tag != 4)goto _LL119;}_LL118:
# 633
 goto _LL11A;_LL119: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp15A=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp153;if(_tmp15A->tag != 5)goto _LL11B;}_LL11A:
# 634
 goto _LL11C;_LL11B: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp15B=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp153;if(_tmp15B->tag != 6)goto _LL11D;}_LL11C:
# 635
 goto _LL11E;_LL11D: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp15C=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp153;if(_tmp15C->tag != 7)goto _LL11F;}_LL11E:
# 636
 goto _LL120;_LL11F: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp15D=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp153;if(_tmp15D->tag != 8)goto _LL121;}_LL120:
# 637
 goto _LL122;_LL121: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp15E=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp153;if(_tmp15E->tag != 9)goto _LL123;}_LL122:
# 639
{struct Cyc_List_List*_tmp1D5;_tmp151=((_tmp1D5=_cycalloc(sizeof(*_tmp1D5)),((_tmp1D5->hd=_tmp152,((_tmp1D5->tl=_tmp151,_tmp1D5))))));}
# 640
goto _LL10E;_LL123: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp15F=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp153;if(_tmp15F->tag != 10)goto _LL125;else{_tmp160=_tmp15F->f2;}}_LL124:
# 641
 _tmp162=_tmp160;goto _LL126;_LL125: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp161=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp153;if(_tmp161->tag != 11)goto _LL127;else{_tmp162=_tmp161->f2;}}_LL126:
# 642
 _tmp164=_tmp162;goto _LL128;_LL127: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp163=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp153;if(_tmp163->tag != 12)goto _LL129;else{_tmp164=_tmp163->f1;}}_LL128:
# 643
 _tmp166=_tmp164;goto _LL12A;_LL129: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp165=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp153;if(_tmp165->tag != 13)goto _LL12B;else{_tmp166=_tmp165->f1;}}_LL12A:
# 644
 goto _LL12C;_LL12B: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp167=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp153;if(_tmp167->tag != 14)goto _LL12D;}_LL12C:
# 645
 goto _LL12E;_LL12D: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp168=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp153;if(_tmp168->tag != 15)goto _LL10E;}_LL12E: {
# 647
struct Cyc_Core_Impossible_exn_struct _tmp1DB;const char*_tmp1DA;struct Cyc_Core_Impossible_exn_struct*_tmp1D9;(int)_throw((void*)((_tmp1D9=_cycalloc(sizeof(*_tmp1D9)),((_tmp1D9[0]=((_tmp1DB.tag=Cyc_Core_Impossible,((_tmp1DB.f1=((_tmp1DA="nested translation unit after translation to C",_tag_dyneither(_tmp1DA,sizeof(char),47))),_tmp1DB)))),_tmp1D9)))));}_LL10E:;}
# 650
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp151);}
