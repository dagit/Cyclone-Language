#include <setjmp.h>
/* This is a C header file to be used by the output of the Cyclone to
   C translator.  The corresponding definitions are in file
   lib/runtime_cyc.c
*/
#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

/***********************************************************************/
/* Runtime Stack routines (runtime_stack.c).                           */
/***********************************************************************/

/* Need one of these per thread (we don't have threads)
   The runtime maintains a stack that contains either _handler_cons
   structs or _RegionHandle structs.  The tag is 0 for a handler_cons
   and 1 for a region handle.  */
struct _RuntimeStack {
  int tag; /* 0 for an exception handler, 1 for a region handle */
  struct _RuntimeStack *next;
  void (*cleanup)(struct _RuntimeStack *frame);
};

// pushes a frame on the stack
void _push_frame(struct _RuntimeStack *frame);

// pop N+1 frames from the stack (error if stack_size < N+1)
void _npop_frame(unsigned int n);

// returns top frame on the stack (NULL if stack is empty)
struct _RuntimeStack * _top_frame();

// pops off frames until a frame with the given tag is reached.  This
// frame is returned, or else NULL if none found.
struct _RuntimeStack * _pop_frame_until(int tag);

/***********************************************************************/
/* Low-level representations etc.                                      */
/***********************************************************************/

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
#else
  unsigned used_bytes;
  unsigned wasted_bytes;
#endif
};

struct _DynRegionFrame {
  struct _RuntimeStack s;
  struct _DynRegionHandle *x;
};

// A dynamic region is just a region handle.  We have the
// wrapper struct for type abstraction reasons.
struct Cyc_Core_DynamicRegion {
  struct _RegionHandle h;
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
extern int _rethrow(void* e);
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
_zero_arr_plus_char_fn(char *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE short *
_zero_arr_plus_short_fn(short *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE int *
_zero_arr_plus_int_fn(int *orig_x, unsigned int orig_sz, int orig_i, const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE float *
_zero_arr_plus_float_fn(float *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE double *
_zero_arr_plus_double_fn(double *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE long double *
_zero_arr_plus_longdouble_fn(long double *orig_x, unsigned int orig_sz, int orig_i, const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE void *
_zero_arr_plus_voidstar_fn(void **orig_x, unsigned int orig_sz, int orig_i,const char *filename,unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
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
static _INLINE char *
_zero_arr_inplace_plus_char_fn(char **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_char_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_char(x,i) \
  _zero_arr_inplace_plus_char_fn((char **)(x),i,__FILE__,__LINE__)
static _INLINE short *
_zero_arr_inplace_plus_short_fn(short **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_short_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_short(x,i) \
  _zero_arr_inplace_plus_short_fn((short **)(x),i,__FILE__,__LINE__)
static _INLINE int *
_zero_arr_inplace_plus_int(int **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_int_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_int(x,i) \
  _zero_arr_inplace_plus_int_fn((int **)(x),i,__FILE__,__LINE__)
static _INLINE float *
_zero_arr_inplace_plus_float_fn(float **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_float_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_float(x,i) \
  _zero_arr_inplace_plus_float_fn((float **)(x),i,__FILE__,__LINE__)
static _INLINE double *
_zero_arr_inplace_plus_double_fn(double **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_double_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_double(x,i) \
  _zero_arr_inplace_plus_double_fn((double **)(x),i,__FILE__,__LINE__)
static _INLINE long double *
_zero_arr_inplace_plus_longdouble_fn(long double **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_longdouble_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_longdouble(x,i) \
  _zero_arr_inplace_plus_longdouble_fn((long double **)(x),i,__FILE__,__LINE__)
static _INLINE void *
_zero_arr_inplace_plus_voidstar_fn(void ***x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_voidstar_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_voidstar(x,i) \
  _zero_arr_inplace_plus_voidstar_fn((void ***)(x),i,__FILE__,__LINE__)

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
  if ((_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one) &&
      _curr != (unsigned char *)0)
    _throw_arraybounds_fn(filename,lineno);
  return _curr;
}
#define _untag_dyneither_ptr(a,s,e) \
  _untag_dyneither_ptr_fn(a,s,e,__FILE__,__LINE__)
#else
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if ((_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one) &&\
      _curr != (unsigned char *)0) \
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

#define _CYC_MAX_REGION_CONST 2
#define _CYC_MIN_ALIGNMENT (sizeof(double))

#ifdef CYC_REGION_PROFILE
extern int rgn_total_bytes;
#endif

static _INLINE void *_fast_region_malloc(struct _RegionHandle *r, unsigned orig_s) {  
  if (r > (struct _RegionHandle *)_CYC_MAX_REGION_CONST && r->curr != 0) { 
#ifdef CYC_NOALIGN
    unsigned s =  orig_s;
#else
    unsigned s =  (orig_s + _CYC_MIN_ALIGNMENT - 1) & (~(_CYC_MIN_ALIGNMENT -1)); 
#endif
    char *result; 
    result = r->offset; 
    if (s <= (r->last_plus_one - result)) {
      r->offset = result + s; 
#ifdef CYC_REGION_PROFILE
    r->curr->free_bytes = r->curr->free_bytes - s;
    rgn_total_bytes += s;
#endif
      return result;
    }
  } 
  return _region_malloc(r,orig_s); 
}

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

# 35 "core.h"
 typedef char*Cyc_Cstring;
typedef char*Cyc_CstringNN;
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
typedef int Cyc_bool;
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
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;
# 121 "core.h"
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;
# 205
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_region_key_t;
# 211
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_uregion_key_t;
# 216
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_rcregion_key_t;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 290 "core.h"
typedef void*Cyc_Core___cyclone_internal_array_t;
typedef void*Cyc_Core___nn_cyclone_internal_array_t;
typedef unsigned int Cyc_Core___cyclone_internal_singleton;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 400
struct Cyc_List_List*Cyc_List_rfilter_c(struct _RegionHandle*r,int(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 68
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 147 "dict.h"
void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict d);
# 195
struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 214
struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 224
int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct _tuple0{void*f1;void*f2;};
# 231
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
# 287
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
struct Cyc_Position_Error*Cyc_Position_mk_err(unsigned int,struct _dyneither_ptr);
# 49
void Cyc_Position_post_error(struct Cyc_Position_Error*);struct Cyc_Relations_Reln;
# 69 "absyn.h"
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;
# 74
typedef void*Cyc_Tcpat_decision_opt_t;
# 81
typedef unsigned int Cyc_Absyn_seg_t;
# 83
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 95
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 102
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
# 107
typedef enum Cyc_Absyn_Scope Cyc_Absyn_scope_t;
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t;
typedef enum Cyc_Absyn_Size_of Cyc_Absyn_size_of_t;
typedef struct Cyc_Absyn_Kind*Cyc_Absyn_kind_t;
typedef void*Cyc_Absyn_kindbound_t;
typedef struct Cyc_Absyn_Tvar*Cyc_Absyn_tvar_t;
typedef enum Cyc_Absyn_Sign Cyc_Absyn_sign_t;
typedef enum Cyc_Absyn_AggrKind Cyc_Absyn_aggr_kind_t;
typedef void*Cyc_Absyn_bounds_t;
typedef struct Cyc_Absyn_PtrAtts Cyc_Absyn_ptr_atts_t;
typedef struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t;
typedef struct Cyc_Absyn_VarargInfo Cyc_Absyn_vararg_info_t;
typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;
typedef struct Cyc_Absyn_DatatypeInfo Cyc_Absyn_datatype_info_t;
typedef struct Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_datatype_field_info_t;
typedef struct Cyc_Absyn_AggrInfo Cyc_Absyn_aggr_info_t;
typedef struct Cyc_Absyn_ArrayInfo Cyc_Absyn_array_info_t;
typedef void*Cyc_Absyn_type_t;typedef void*Cyc_Absyn_rgntype_t;typedef void*Cyc_Absyn_type_opt_t;
typedef union Cyc_Absyn_Cnst Cyc_Absyn_cnst_t;
typedef enum Cyc_Absyn_Primop Cyc_Absyn_primop_t;
typedef enum Cyc_Absyn_Incrementor Cyc_Absyn_incrementor_t;
typedef struct Cyc_Absyn_VarargCallInfo Cyc_Absyn_vararg_call_info_t;
typedef void*Cyc_Absyn_raw_exp_t;
typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_t;typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_opt_t;
typedef void*Cyc_Absyn_raw_stmt_t;
typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_t;typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_opt_t;
typedef void*Cyc_Absyn_raw_pat_t;
typedef struct Cyc_Absyn_Pat*Cyc_Absyn_pat_t;
typedef void*Cyc_Absyn_binding_t;
typedef struct Cyc_Absyn_Switch_clause*Cyc_Absyn_switch_clause_t;
typedef struct Cyc_Absyn_Fndecl*Cyc_Absyn_fndecl_t;
typedef struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_aggrdecl_t;
typedef struct Cyc_Absyn_Datatypefield*Cyc_Absyn_datatypefield_t;
typedef struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_datatypedecl_t;
typedef struct Cyc_Absyn_Typedefdecl*Cyc_Absyn_typedefdecl_t;
typedef struct Cyc_Absyn_Enumfield*Cyc_Absyn_enumfield_t;
typedef struct Cyc_Absyn_Enumdecl*Cyc_Absyn_enumdecl_t;
typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_t;typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_opt_t;
typedef void*Cyc_Absyn_raw_decl_t;
typedef struct Cyc_Absyn_Decl*Cyc_Absyn_decl_t;
typedef void*Cyc_Absyn_designator_t;
typedef void*Cyc_Absyn_absyn_annot_t;
typedef void*Cyc_Absyn_attribute_t;
typedef struct Cyc_List_List*Cyc_Absyn_attributes_t;
typedef struct Cyc_Absyn_Aggrfield*Cyc_Absyn_aggrfield_t;
typedef void*Cyc_Absyn_offsetof_field_t;
typedef struct Cyc_Absyn_MallocInfo Cyc_Absyn_malloc_info_t;
typedef enum Cyc_Absyn_Coercion Cyc_Absyn_coercion_t;
typedef struct Cyc_Absyn_PtrLoc*Cyc_Absyn_ptrloc_t;
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned int loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 190
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 210
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 212
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 221
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 367
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 372
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 443
typedef void*Cyc_Absyn_funcparams_t;
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 510
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 908 "absyn.h"
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 935
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 1173
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1177
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);struct Cyc_RgnOrder_RgnPO;
# 30 "rgnorder.h"
typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;
# 32
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int);
# 39
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int loc);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int opened);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_unordered(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn);
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 45
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);
# 48
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 48 "tcenv.h"
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 52
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 62
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 86
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 52
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
# 55
void*Cyc_Tcutil_compress(void*t);
# 96
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 150
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 163
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 265 "tcutil.h"
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 306
int Cyc_Tcutil_bits_only(void*t);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 38 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 49
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 113
int Cyc_Relations_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*r1s);
# 115
struct Cyc_List_List*Cyc_Relations_join_relns(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*);
# 117
void Cyc_Relations_print_relns(struct Cyc___cycFILE*,struct Cyc_List_List*);
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 52
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 63
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 65
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 70
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;char Cyc_CfFlowInfo_IsZero[7]="IsZero";struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};char Cyc_CfFlowInfo_NotZero[8]="NotZero";struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[9]="UnknownZ";struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 79
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 85
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();
# 90
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef void*Cyc_CfFlowInfo_absRval_opt_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 98
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 119
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 121
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,unsigned int loc);
# 123
int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);
int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);
# 126
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple11{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple11 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 142 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 161
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);
# 164
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f);
# 167
int Cyc_CfFlowInfo_root_cmp(void*,void*);
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
# 170
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);struct _tuple12{void*f1;struct Cyc_List_List*f2;};
struct _tuple12 Cyc_CfFlowInfo_unname_rval(struct _RegionHandle*rgn,void*rv);
# 178
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 183
void Cyc_CfFlowInfo_print_absrval(void*rval);
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il);
void Cyc_CfFlowInfo_print_root(void*root);
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);
void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*));
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*p,void(*pr)(void*));
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f);
# 204 "cf_flowinfo.h"
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);
# 208
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);
# 213
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple13{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,struct _tuple13 pr2);
# 219
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 223
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place);
# 33 "cf_flowinfo.cyc"
int Cyc_CfFlowInfo_anal_error=0;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 36
Cyc_CfFlowInfo_anal_error=1;
Cyc_Position_post_error(({unsigned int _tmp2D2=loc;Cyc_Position_mk_err(_tmp2D2,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap));}));}
# 49 "cf_flowinfo.cyc"
struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val={Cyc_CfFlowInfo_IsZero};
# 51
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*x){
return({union Cyc_CfFlowInfo_AbsLVal _tmp1;(_tmp1.PlaceL).val=x;(_tmp1.PlaceL).tag=1;_tmp1;});}
# 54
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL(){
return({union Cyc_CfFlowInfo_AbsLVal _tmp2;(_tmp2.UnknownL).val=0;(_tmp2.UnknownL).tag=2;_tmp2;});}
# 57
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL(){return({union Cyc_CfFlowInfo_FlowInfo _tmp3;(_tmp3.BottomFL).val=0;(_tmp3.BottomFL).tag=1;_tmp3;});}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,struct Cyc_List_List*r){
return({union Cyc_CfFlowInfo_FlowInfo _tmp4;({struct _tuple11 _tmp2D3=({struct _tuple11 _tmp5;_tmp5.f1=fd;_tmp5.f2=r;_tmp5;});(_tmp4.ReachableFL).val=_tmp2D3;});(_tmp4.ReachableFL).tag=2;_tmp4;});}
# 63
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2);
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct dummy_rawexp={0,{.Null_c={1,0}}};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp6=_region_malloc(r,sizeof(*_tmp6));_tmp6->r=r;({void*_tmp2D5=(void*)({struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp1A=_region_malloc(r,sizeof(*_tmp1A));({struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct _tmp2D4=({struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct _tmp1B;_tmp1B.tag=0;_tmp1B;});_tmp1A[0]=_tmp2D4;});_tmp1A;});_tmp6->zero=_tmp2D5;});({void*_tmp2D7=(void*)({struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp18=_region_malloc(r,sizeof(*_tmp18));({struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct _tmp2D6=({struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct _tmp19;_tmp19.tag=1;_tmp19;});_tmp18[0]=_tmp2D6;});_tmp18;});_tmp6->notzeroall=_tmp2D7;});({void*_tmp2D9=(void*)({struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp16=_region_malloc(r,sizeof(*_tmp16));({struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct _tmp2D8=({struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct _tmp17;_tmp17.tag=2;_tmp17;});_tmp16[0]=_tmp2D8;});_tmp16;});_tmp6->notzerothis=_tmp2D9;});({void*_tmp2DB=(void*)({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp14=_region_malloc(r,sizeof(*_tmp14));({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp2DA=({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp15;_tmp15.tag=3;_tmp15.f1=Cyc_CfFlowInfo_NoneIL;_tmp15;});_tmp14[0]=_tmp2DA;});_tmp14;});_tmp6->unknown_none=_tmp2DB;});({void*_tmp2DD=(void*)({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp12=_region_malloc(r,sizeof(*_tmp12));({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp2DC=({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp13;_tmp13.tag=3;_tmp13.f1=Cyc_CfFlowInfo_ThisIL;_tmp13;});_tmp12[0]=_tmp2DC;});_tmp12;});_tmp6->unknown_this=_tmp2DD;});({void*_tmp2DF=(void*)({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp10=_region_malloc(r,sizeof(*_tmp10));({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp2DE=({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp11;_tmp11.tag=3;_tmp11.f1=Cyc_CfFlowInfo_AllIL;_tmp11;});_tmp10[0]=_tmp2DE;});_tmp10;});_tmp6->unknown_all=_tmp2DF;});({void*_tmp2E1=(void*)({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmpE=_region_malloc(r,sizeof(*_tmpE));({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp2E0=({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmpF;_tmpF.tag=4;_tmpF.f1=Cyc_CfFlowInfo_NoneIL;_tmpF;});_tmpE[0]=_tmp2E0;});_tmpE;});_tmp6->esc_none=_tmp2E1;});({void*_tmp2E3=(void*)({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmpC=_region_malloc(r,sizeof(*_tmpC));({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp2E2=({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmpD;_tmpD.tag=4;_tmpD.f1=Cyc_CfFlowInfo_ThisIL;_tmpD;});_tmpC[0]=_tmp2E2;});_tmpC;});_tmp6->esc_this=_tmp2E3;});({void*_tmp2E5=(void*)({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmpA=_region_malloc(r,sizeof(*_tmpA));({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp2E4=({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmpB;_tmpB.tag=4;_tmpB.f1=Cyc_CfFlowInfo_AllIL;_tmpB;});_tmpA[0]=_tmp2E4;});_tmpA;});_tmp6->esc_all=_tmp2E5;});({struct Cyc_Dict_Dict _tmp2E6=
# 79
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(void*,void*)))Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_root_cmp);_tmp6->mt_flowdict=_tmp2E6;});({struct Cyc_Dict_Dict _tmp2E7=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp);_tmp6->mt_place_set=_tmp2E7;});({struct Cyc_CfFlowInfo_Place*_tmp2EA=({struct Cyc_CfFlowInfo_Place*_tmp7=_region_malloc(r,sizeof(*_tmp7));({void*_tmp2E9=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp8=_region_malloc(r,sizeof(*_tmp8));({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp2E8=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp9;_tmp9.tag=1;_tmp9.f1=& dummy_exp;_tmp9.f2=(void*)& Cyc_Absyn_VoidType_val;_tmp9;});_tmp8[0]=_tmp2E8;});_tmp8;});_tmp7->root=_tmp2E9;});_tmp7->fields=0;_tmp7;});_tmp6->dummy_place=_tmp2EA;});_tmp6;});}
# 87
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,unsigned int loc){
if(set != 0){
# 90
if(((int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(*set,place))
return 1;else{
# 93
({struct Cyc_Dict_Dict _tmp2EB=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(*set,place,loc);*set=_tmp2EB;});}}
# 95
return 0;}
# 99
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _tmp1D=*place;struct Cyc_CfFlowInfo_Place _tmp1E=_tmp1D;void*_tmp27;struct Cyc_List_List*_tmp26;_LL1: _tmp27=_tmp1E.root;_tmp26=_tmp1E.fields;_LL2:;{
void*_tmp1F=_tmp27;struct Cyc_Absyn_Vardecl*_tmp25;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp1F)->tag == 0){_LL4: _tmp25=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp1F)->f1;_LL5:
# 103
 if(_tmp26 == 0)
return Cyc_Absynpp_qvar2string(_tmp25->name);else{
# 106
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp22;_tmp22.tag=0;({struct _dyneither_ptr _tmp2EC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp25->name));_tmp22.f1=_tmp2EC;});({void*_tmp20[1]={& _tmp22};({struct _dyneither_ptr _tmp2ED=({const char*_tmp21="reachable from %s";_tag_dyneither(_tmp21,sizeof(char),18);});Cyc_aprintf(_tmp2ED,_tag_dyneither(_tmp20,sizeof(void*),1));});});});}}else{_LL6: _LL7:
({void*_tmp23=0;({struct _dyneither_ptr _tmp2EE=({const char*_tmp24="error locations not for VarRoots";_tag_dyneither(_tmp24,sizeof(char),33);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp2EE,_tag_dyneither(_tmp23,sizeof(void*),0));});});}_LL3:;};}
# 112
static unsigned int Cyc_CfFlowInfo_combine_consume_f(void*env,void*place,unsigned int loc1,unsigned int loc2){
# 116
return loc1;}
# 121
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint){
# 128
int _tmp28=0;
struct Cyc_Dict_Dict _tmp29=((struct Cyc_Dict_Dict(*)(unsigned int(*f)(int*,struct Cyc_CfFlowInfo_Place*,unsigned int,unsigned int),int*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)((unsigned int(*)(int*env,struct Cyc_CfFlowInfo_Place*place,unsigned int loc1,unsigned int loc2))Cyc_CfFlowInfo_combine_consume_f,& _tmp28,s1,s2);
# 133
return _tmp29;}struct _tuple14{void*f1;unsigned int f2;};
# 136
int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
if((int)s1.t == (int)s2.t)return 1;{
struct _handler_cons _tmp2A;_push_handler(& _tmp2A);{int _tmp2C=0;if(setjmp(_tmp2A.handler))_tmp2C=1;if(!_tmp2C){
{struct _RegionHandle _tmp2D=_new_region("temp");struct _RegionHandle*temp=& _tmp2D;_push_region(temp);
{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(temp,s1);
struct _tuple14 _tmp2E=*((struct _tuple14*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(temp,s1);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple14*))Cyc_Iter_next)(iter,& _tmp2E)){
void*_tmp2F=_tmp2E.f1;
if(!((int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(s2,_tmp2F)){
# 148
int _tmp30=0;_npop_handler(1);return _tmp30;}}{
# 151
int _tmp31=1;_npop_handler(1);return _tmp31;};}
# 140
;_pop_region(temp);}
# 139
;_pop_handler();}else{void*_tmp2B=(void*)_exn_thrown;void*_tmp32=_tmp2B;void*_tmp33;_LL9: _tmp33=_tmp32;_LLA:
# 155
 return 1;_LL8:;}};};}
# 159
int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
return Cyc_CfFlowInfo_place_set_subset(s1,s2) && Cyc_CfFlowInfo_place_set_subset(s2,s1);}
# 164
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f){
int n=0;
for(0;(unsigned int)fs;fs=fs->tl){
struct _dyneither_ptr*_tmp34=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;
if(({Cyc_strptrcmp(_tmp34,f)== 0;}))return n;
n=n + 1;}
# 171
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp35=_cycalloc(sizeof(*_tmp35));({struct Cyc_Core_Impossible_exn_struct _tmp2F1=({struct Cyc_Core_Impossible_exn_struct _tmp36;_tmp36.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp2F0=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp39;_tmp39.tag=0;_tmp39.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp37[1]={& _tmp39};({struct _dyneither_ptr _tmp2EF=({const char*_tmp38="get_field_index_fs failed: %s";_tag_dyneither(_tmp38,sizeof(char),30);});Cyc_aprintf(_tmp2EF,_tag_dyneither(_tmp37,sizeof(void*),1));});});});_tmp36.f1=_tmp2F0;});_tmp36;});_tmp35[0]=_tmp2F1;});_tmp35;}));}
# 174
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f){
void*_tmp3A=Cyc_Tcutil_compress(t);void*_tmp3B=_tmp3A;struct Cyc_List_List*_tmp43;union Cyc_Absyn_AggrInfoU _tmp42;switch(*((int*)_tmp3B)){case 11: _LLE: _tmp42=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3B)->f1).aggr_info;_LLF: {
# 177
struct Cyc_List_List*_tmp3C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp42))->impl))->fields;
_tmp43=_tmp3C;goto _LL11;}case 12: _LL10: _tmp43=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3B)->f2;_LL11:
# 180
 return Cyc_CfFlowInfo_get_field_index_fs(_tmp43,f);default: _LL12: _LL13:
# 182
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp3D=_cycalloc(sizeof(*_tmp3D));({struct Cyc_Core_Impossible_exn_struct _tmp2F5=({struct Cyc_Core_Impossible_exn_struct _tmp3E;_tmp3E.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp2F4=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp41;_tmp41.tag=0;({struct _dyneither_ptr _tmp2F2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp41.f1=_tmp2F2;});({void*_tmp3F[1]={& _tmp41};({struct _dyneither_ptr _tmp2F3=({const char*_tmp40="get_field_index failed: %s";_tag_dyneither(_tmp40,sizeof(char),27);});Cyc_aprintf(_tmp2F3,_tag_dyneither(_tmp3F,sizeof(void*),1));});});});_tmp3E.f1=_tmp2F4;});_tmp3E;});_tmp3D[0]=_tmp2F5;});_tmp3D;}));}_LLD:;}
# 187
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){
if((int)r1 == (int)r2)
return 0;{
struct _tuple0 _tmp44=({struct _tuple0 _tmp4C;_tmp4C.f1=r1;_tmp4C.f2=r2;_tmp4C;});struct _tuple0 _tmp45=_tmp44;int _tmp4B;int _tmp4A;struct Cyc_Absyn_Exp*_tmp49;struct Cyc_Absyn_Exp*_tmp48;struct Cyc_Absyn_Vardecl*_tmp47;struct Cyc_Absyn_Vardecl*_tmp46;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp45.f1)->tag == 0){if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp45.f2)->tag == 0){_LL15: _tmp47=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp45.f1)->f1;_tmp46=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp45.f2)->f1;_LL16:
 return(int)_tmp47 - (int)_tmp46;}else{_LL17: _LL18:
 return - 1;}}else{if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp45.f2)->tag == 0){_LL19: _LL1A:
 return 1;}else{if(((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp45.f1)->tag == 1){if(((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp45.f2)->tag == 1){_LL1B: _tmp49=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp45.f1)->f1;_tmp48=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp45.f2)->f1;_LL1C:
 return(int)_tmp49 - (int)_tmp48;}else{_LL1D: _LL1E:
 return - 1;}}else{if(((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp45.f2)->tag == 1){_LL1F: _LL20:
 return 1;}else{_LL21: _tmp4B=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp45.f1)->f1;_tmp4A=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp45.f2)->f1;_LL22:
 return _tmp4B - _tmp4A;}}}}_LL14:;};}
# 201
static int Cyc_CfFlowInfo_intlist_cmp(struct Cyc_List_List*list1,struct Cyc_List_List*list2){
# 203
return((int(*)(int(*cmp)(int,int),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Core_intcmp,list1,list2);}
# 206
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){
if((int)p1 == (int)p2)
return 0;{
struct Cyc_CfFlowInfo_Place _tmp4D=*p1;struct Cyc_CfFlowInfo_Place _tmp4E=_tmp4D;void*_tmp54;struct Cyc_List_List*_tmp53;_LL24: _tmp54=_tmp4E.root;_tmp53=_tmp4E.fields;_LL25:;{
struct Cyc_CfFlowInfo_Place _tmp4F=*p2;struct Cyc_CfFlowInfo_Place _tmp50=_tmp4F;void*_tmp52;struct Cyc_List_List*_tmp51;_LL27: _tmp52=_tmp50.root;_tmp51=_tmp50.fields;_LL28:;{
int i=Cyc_CfFlowInfo_root_cmp(_tmp54,_tmp52);
if(i != 0)
return i;
return Cyc_CfFlowInfo_intlist_cmp(_tmp53,_tmp51);};};};}
# 218
static struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_List_List*sl=0;
struct Cyc_CfFlowInfo_Place _tmp55=*p;struct Cyc_CfFlowInfo_Place _tmp56=_tmp55;void*_tmp7A;struct Cyc_List_List*_tmp79;_LL2A: _tmp7A=_tmp56.root;_tmp79=_tmp56.fields;_LL2B:;
{void*_tmp57=_tmp7A;int _tmp69;struct Cyc_Absyn_Exp*_tmp68;struct Cyc_Absyn_Vardecl*_tmp67;switch(*((int*)_tmp57)){case 0: _LL2D: _tmp67=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp57)->f1;_LL2E:
({struct Cyc_List_List*_tmp2F9=({struct Cyc_List_List*_tmp58=_cycalloc(sizeof(*_tmp58));({struct _dyneither_ptr*_tmp2F8=({struct _dyneither_ptr*_tmp59=_cycalloc(sizeof(*_tmp59));({struct _dyneither_ptr _tmp2F7=({struct Cyc_String_pa_PrintArg_struct _tmp5C;_tmp5C.tag=0;_tmp5C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp67->name).f2);({void*_tmp5A[1]={& _tmp5C};({struct _dyneither_ptr _tmp2F6=({const char*_tmp5B="%s";_tag_dyneither(_tmp5B,sizeof(char),3);});Cyc_aprintf(_tmp2F6,_tag_dyneither(_tmp5A,sizeof(void*),1));});});});_tmp59[0]=_tmp2F7;});_tmp59;});_tmp58->hd=_tmp2F8;});_tmp58->tl=sl;_tmp58;});sl=_tmp2F9;});goto _LL2C;case 1: _LL2F: _tmp68=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp57)->f1;_LL30:
({struct Cyc_List_List*_tmp2FD=({struct Cyc_List_List*_tmp5D=_cycalloc(sizeof(*_tmp5D));({struct _dyneither_ptr*_tmp2FC=({struct _dyneither_ptr*_tmp5E=_cycalloc(sizeof(*_tmp5E));({struct _dyneither_ptr _tmp2FB=({struct Cyc_Int_pa_PrintArg_struct _tmp61;_tmp61.tag=1;_tmp61.f1=(unsigned long)((int)_tmp68);({void*_tmp5F[1]={& _tmp61};({struct _dyneither_ptr _tmp2FA=({const char*_tmp60="mpt%d";_tag_dyneither(_tmp60,sizeof(char),6);});Cyc_aprintf(_tmp2FA,_tag_dyneither(_tmp5F,sizeof(void*),1));});});});_tmp5E[0]=_tmp2FB;});_tmp5E;});_tmp5D->hd=_tmp2FC;});_tmp5D->tl=sl;_tmp5D;});sl=_tmp2FD;});goto _LL2C;default: _LL31: _tmp69=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp57)->f1;_LL32:
({struct Cyc_List_List*_tmp301=({struct Cyc_List_List*_tmp62=_cycalloc(sizeof(*_tmp62));({struct _dyneither_ptr*_tmp300=({struct _dyneither_ptr*_tmp63=_cycalloc(sizeof(*_tmp63));({struct _dyneither_ptr _tmp2FF=({struct Cyc_Int_pa_PrintArg_struct _tmp66;_tmp66.tag=1;_tmp66.f1=(unsigned long)_tmp69;({void*_tmp64[1]={& _tmp66};({struct _dyneither_ptr _tmp2FE=({const char*_tmp65="param%d";_tag_dyneither(_tmp65,sizeof(char),8);});Cyc_aprintf(_tmp2FE,_tag_dyneither(_tmp64,sizeof(void*),1));});});});_tmp63[0]=_tmp2FF;});_tmp63;});_tmp62->hd=_tmp300;});_tmp62->tl=sl;_tmp62;});sl=_tmp301;});goto _LL2C;}_LL2C:;}
# 226
for(0;_tmp79 != 0;_tmp79=_tmp79->tl){
({struct Cyc_List_List*_tmp305=({struct Cyc_List_List*_tmp6A=_cycalloc(sizeof(*_tmp6A));({struct _dyneither_ptr*_tmp304=({struct _dyneither_ptr*_tmp6B=_cycalloc(sizeof(*_tmp6B));({struct _dyneither_ptr _tmp303=({struct Cyc_Int_pa_PrintArg_struct _tmp6E;_tmp6E.tag=1;_tmp6E.f1=(unsigned long)((int)_tmp79->hd);({void*_tmp6C[1]={& _tmp6E};({struct _dyneither_ptr _tmp302=({const char*_tmp6D="%d";_tag_dyneither(_tmp6D,sizeof(char),3);});Cyc_aprintf(_tmp302,_tag_dyneither(_tmp6C,sizeof(void*),1));});});});_tmp6B[0]=_tmp303;});_tmp6B;});_tmp6A->hd=_tmp304;});_tmp6A->tl=sl;_tmp6A;});sl=_tmp305;});}{
struct _dyneither_ptr*_tmp6F=({struct _dyneither_ptr*_tmp74=_cycalloc(sizeof(*_tmp74));({struct _dyneither_ptr _tmp308=({struct Cyc_String_pa_PrintArg_struct _tmp77;_tmp77.tag=0;({struct _dyneither_ptr _tmp306=(struct _dyneither_ptr)({const char*_tmp78="";_tag_dyneither(_tmp78,sizeof(char),1);});_tmp77.f1=_tmp306;});({void*_tmp75[1]={& _tmp77};({struct _dyneither_ptr _tmp307=({const char*_tmp76="%s";_tag_dyneither(_tmp76,sizeof(char),3);});Cyc_aprintf(_tmp307,_tag_dyneither(_tmp75,sizeof(void*),1));});});});_tmp74[0]=_tmp308;});_tmp74;});
for(0;sl != 0;sl=sl->tl){
({struct _dyneither_ptr _tmp30A=({struct Cyc_String_pa_PrintArg_struct _tmp73;_tmp73.tag=0;_tmp73.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp6F);({struct Cyc_String_pa_PrintArg_struct _tmp72;_tmp72.tag=0;_tmp72.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)sl->hd));({void*_tmp70[2]={& _tmp72,& _tmp73};({struct _dyneither_ptr _tmp309=({const char*_tmp71="%s.%s";_tag_dyneither(_tmp71,sizeof(char),6);});Cyc_aprintf(_tmp309,_tag_dyneither(_tmp70,sizeof(void*),2));});});});});*_tmp6F=_tmp30A;});}
return _tmp6F;};}
# 244 "cf_flowinfo.cyc"
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);
# 249
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 255
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fs);
struct _dyneither_ptr d=({unsigned int _tmp7F=sz;void**_tmp80=(void**)({struct _RegionHandle*_tmp30B=fenv->r;_region_malloc(_tmp30B,_check_times(sizeof(void*),_tmp7F));});struct _dyneither_ptr _tmp82=_tag_dyneither(_tmp80,sizeof(void*),_tmp7F);{unsigned int _tmp81=_tmp7F;unsigned int i;for(i=0;i < _tmp81;i ++){_tmp80[i]=(void*)fenv->unknown_all;}}_tmp82;});
{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield*_tmp7B=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd;struct Cyc_Absyn_Aggrfield*_tmp7C=_tmp7B;struct _dyneither_ptr*_tmp7E;void*_tmp7D;_LL34: _tmp7E=_tmp7C->name;_tmp7D=_tmp7C->type;_LL35:;
if(_get_dyneither_size(*_tmp7E,sizeof(char))!= 1)
({void*_tmp30C=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,_tmp7D,leafval);*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=_tmp30C;});}}
# 263
return d;}struct _tuple15{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 267
static struct Cyc_Absyn_Aggrfield*Cyc_CfFlowInfo_substitute_field(struct _tuple15*env,struct Cyc_Absyn_Aggrfield*f){
# 271
struct _tuple15*_tmp83=env;struct _RegionHandle*_tmp89;struct Cyc_List_List*_tmp88;_LL37: _tmp89=_tmp83->f1;_tmp88=_tmp83->f2;_LL38:;{
void*_tmp84=Cyc_Tcutil_rsubstitute(_tmp89,_tmp88,f->type);
struct Cyc_Absyn_Exp*_tmp85=f->requires_clause;
struct Cyc_Absyn_Exp*_tmp86=_tmp85 == 0?0: Cyc_Tcutil_rsubsexp(_tmp89,_tmp88,_tmp85);
return({struct Cyc_Absyn_Aggrfield*_tmp87=_region_malloc(_tmp89,sizeof(*_tmp87));_tmp87->name=f->name;_tmp87->tq=f->tq;_tmp87->type=_tmp84;_tmp87->width=f->width;_tmp87->attributes=f->attributes;_tmp87->requires_clause=_tmp86;_tmp87;});};}struct _tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 282
static struct _dyneither_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*targs,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 288
struct _RegionHandle _tmp8A=_new_region("temp");struct _RegionHandle*temp=& _tmp8A;_push_region(temp);
# 292
{struct Cyc_List_List*inst=0;
for(0;tvs != 0;(tvs=tvs->tl,targs=targs->tl)){
struct Cyc_Absyn_Tvar*_tmp8B=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp8C=(void*)((struct Cyc_List_List*)_check_null(targs))->hd;
{struct Cyc_Absyn_Kind*_tmp8D=Cyc_Tcutil_tvar_kind(_tmp8B,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp8E=_tmp8D;switch(((struct Cyc_Absyn_Kind*)_tmp8E)->kind){case Cyc_Absyn_RgnKind: _LL3A: _LL3B:
 goto _LL3D;case Cyc_Absyn_EffKind: _LL3C: _LL3D:
 continue;default: _LL3E: _LL3F:
 goto _LL39;}_LL39:;}
# 301
({struct Cyc_List_List*_tmp30E=({struct Cyc_List_List*_tmp8F=_region_malloc(temp,sizeof(*_tmp8F));({struct _tuple16*_tmp30D=({struct _tuple16*_tmp90=_region_malloc(temp,sizeof(*_tmp90));_tmp90->f1=_tmp8B;_tmp90->f2=_tmp8C;_tmp90;});_tmp8F->hd=_tmp30D;});_tmp8F->tl=inst;_tmp8F;});inst=_tmp30E;});}
# 303
if(inst != 0){
struct _tuple15 _tmp91=({struct _tuple15 _tmp93;_tmp93.f1=temp;({struct Cyc_List_List*_tmp30F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(inst);_tmp93.f2=_tmp30F;});_tmp93;});
struct Cyc_List_List*subs_fs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*f)(struct _tuple15*,struct Cyc_Absyn_Aggrfield*),struct _tuple15*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(temp,Cyc_CfFlowInfo_substitute_field,& _tmp91,fs);
struct _dyneither_ptr _tmp92=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,subs_fs,no_init_bits_only,leafval);_npop_handler(0);return _tmp92;}else{
# 308
struct _dyneither_ptr _tmp94=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,no_init_bits_only,leafval);_npop_handler(0);return _tmp94;}}
# 292
;_pop_region(temp);}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 312
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval){
# 318
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(t))return fenv->unknown_all;
# 320
{void*_tmp95=Cyc_Tcutil_compress(t);void*_tmp96=_tmp95;union Cyc_Absyn_Constraint*_tmpB0;void*_tmpAF;void*_tmpAE;union Cyc_Absyn_Constraint*_tmpAD;enum Cyc_Absyn_AggrKind _tmpAC;struct Cyc_List_List*_tmpAB;union Cyc_Absyn_AggrInfoU _tmpAA;struct Cyc_List_List*_tmpA9;struct Cyc_List_List*_tmpA8;struct Cyc_Absyn_Datatypefield*_tmpA7;switch(*((int*)_tmp96)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp96)->f1).field_info).KnownDatatypefield).tag == 2){_LL41: _tmpA7=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp96)->f1).field_info).KnownDatatypefield).val).f2;_LL42:
# 322
 if(_tmpA7->typs == 0)
return leafval;
_tmpA8=_tmpA7->typs;goto _LL44;}else{goto _LL4F;}case 10: _LL43: _tmpA8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp96)->f1;_LL44: {
# 326
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA8);
# 329
struct _dyneither_ptr d=({unsigned int _tmp9A=sz;void**_tmp9B=(void**)({struct _RegionHandle*_tmp310=fenv->r;_region_malloc(_tmp310,_check_times(sizeof(void*),_tmp9A));});struct _dyneither_ptr _tmp9D=_tag_dyneither(_tmp9B,sizeof(void*),_tmp9A);{unsigned int _tmp9C=_tmp9A;unsigned int i;for(i=0;i < _tmp9C;i ++){_tmp9B[i]=(void*)fenv->unknown_all;}}_tmp9D;});
{int i=0;for(0;i < sz;++ i){
({void*_tmp311=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmpA8))->hd)).f2,leafval);*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=_tmp311;});
_tmpA8=_tmpA8->tl;}}
# 334
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp97=_region_malloc(fenv->r,sizeof(*_tmp97));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp313=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp98;_tmp98.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp312=({struct Cyc_CfFlowInfo_UnionRInfo _tmp99;_tmp99.is_union=0;_tmp99.fieldnum=- 1;_tmp99;});_tmp98.f1=_tmp312;});_tmp98.f2=d;_tmp98;});_tmp97[0]=_tmp313;});_tmp97;});}case 11: _LL45: _tmpAA=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp96)->f1).aggr_info;_tmpA9=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp96)->f1).targs;_LL46: {
# 336
struct Cyc_Absyn_Aggrdecl*_tmp9E=Cyc_Absyn_get_known_aggrdecl(_tmpAA);
if(_tmp9E->impl == 0)
goto _LL40;{
struct Cyc_List_List*_tmp9F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp9E->impl))->fields;
if(_tmpA9 == 0){_tmpAC=_tmp9E->kind;_tmpAB=_tmp9F;goto _LL48;}
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpA0=_region_malloc(fenv->r,sizeof(*_tmpA0));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp316=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmpA1;_tmpA1.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp314=({struct Cyc_CfFlowInfo_UnionRInfo _tmpA2;_tmpA2.is_union=_tmp9E->kind == Cyc_Absyn_UnionA;_tmpA2.fieldnum=- 1;_tmpA2;});_tmpA1.f1=_tmp314;});({struct _dyneither_ptr _tmp315=
Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,_tmp9E->tvs,_tmpA9,_tmp9F,_tmp9E->kind == Cyc_Absyn_UnionA,leafval);_tmpA1.f2=_tmp315;});_tmpA1;});_tmpA0[0]=_tmp316;});_tmpA0;});};}case 12: _LL47: _tmpAC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp96)->f1;_tmpAB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp96)->f2;_LL48:
# 347
 return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpA3=_region_malloc(fenv->r,sizeof(*_tmpA3));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp319=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmpA4;_tmpA4.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp317=({struct Cyc_CfFlowInfo_UnionRInfo _tmpA5;_tmpA5.is_union=_tmpAC == Cyc_Absyn_UnionA;_tmpA5.fieldnum=- 1;_tmpA5;});_tmpA4.f1=_tmp317;});({struct _dyneither_ptr _tmp318=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,_tmpAB,_tmpAC == Cyc_Absyn_UnionA,leafval);_tmpA4.f2=_tmp318;});_tmpA4;});_tmpA3[0]=_tmp319;});_tmpA3;});case 8: _LL49: _tmpAE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp96)->f1).elt_type;_tmpAD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp96)->f1).zero_term;if(
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpAD)){_LL4A:
# 351
 return(allow_zeroterm  && !no_init_bits_only) && Cyc_Tcutil_bits_only(_tmpAE)?fenv->unknown_all: leafval;}else{goto _LL4F;}case 19: _LL4B: _tmpAF=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp96)->f1;_LL4C:
 return leafval;case 5: _LL4D: _tmpB0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp96)->f1).ptr_atts).nullable;if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB0)){_LL4E:
{void*_tmpA6=leafval;if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpA6)->tag == 3)switch(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpA6)->f1){case Cyc_CfFlowInfo_ThisIL: _LL52: _LL53:
 return fenv->notzerothis;case Cyc_CfFlowInfo_AllIL: _LL54: _LL55:
 return fenv->notzeroall;default: goto _LL56;}else{_LL56: _LL57:
 goto _LL51;}_LL51:;}
# 359
goto _LL40;}else{goto _LL4F;}default: _LL4F: _LL50:
 goto _LL40;}_LL40:;}
# 363
return !no_init_bits_only  && Cyc_Tcutil_bits_only(t)?fenv->unknown_all: leafval;}
# 366
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,int no_init_bits_only,void*leafval){
return Cyc_CfFlowInfo_i_typ_to_absrval(fenv,1,no_init_bits_only,t,leafval);}
# 372
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume){
void*_tmpB1=r;void*_tmpB9;int _tmpB8;int _tmpB7;struct _dyneither_ptr _tmpB6;struct Cyc_Absyn_Exp*_tmpB5;int _tmpB4;void*_tmpB3;switch(*((int*)_tmpB1)){case 7: _LL59: _tmpB5=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpB1)->f1;_tmpB4=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpB1)->f2;_tmpB3=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpB1)->f3;_LL5A:
# 375
 if(_tmpB5 == e  && _tmpB4 == env_iteration){
*needs_unconsume=1;
return 0;}
# 379
return 1;case 6: _LL5B: _tmpB8=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpB1)->f1).is_union;_tmpB7=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpB1)->f1).fieldnum;_tmpB6=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpB1)->f2;_LL5C:
# 381
 if(!_tmpB8  || _tmpB7 == - 1){
unsigned int _tmpB2=_get_dyneither_size(_tmpB6,sizeof(void*));
{int i=0;for(0;i < _tmpB2;++ i){
if(Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,((void**)_tmpB6.curr)[i],needs_unconsume))
return 1;}}
# 387
return 0;}else{
# 390
return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)_check_dyneither_subscript(_tmpB6,sizeof(void*),_tmpB7)),needs_unconsume);}case 8: _LL5D: _tmpB9=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpB1)->f2;_LL5E:
# 392
 return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,_tmpB9,needs_unconsume);default: _LL5F: _LL60:
 return 0;}_LL58:;}
# 399
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r){
void*_tmpBA=r;struct Cyc_Absyn_Vardecl*_tmpCB;void*_tmpCA;struct Cyc_CfFlowInfo_UnionRInfo _tmpC9;struct _dyneither_ptr _tmpC8;struct Cyc_Absyn_Exp*_tmpC7;int _tmpC6;void*_tmpC5;switch(*((int*)_tmpBA)){case 7: _LL62: _tmpC7=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpBA)->f1;_tmpC6=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpBA)->f2;_tmpC5=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpBA)->f3;_LL63:
# 402
 return _tmpC5;case 6: _LL64: _tmpC9=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpBA)->f1;_tmpC8=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpBA)->f2;_LL65: {
# 404
unsigned int _tmpBB=_get_dyneither_size(_tmpC8,sizeof(void*));
int change=0;
struct _dyneither_ptr d2=({unsigned int _tmpBE=_tmpBB;void**_tmpBF=(void**)({struct _RegionHandle*_tmp31A=fenv->r;_region_malloc(_tmp31A,_check_times(sizeof(void*),_tmpBE));});struct _dyneither_ptr _tmpC1=_tag_dyneither(_tmpBF,sizeof(void*),_tmpBE);{unsigned int _tmpC0=_tmpBE;unsigned int i;for(i=0;i < _tmpC0;i ++){_tmpBF[i]=(void*)((void**)_tmpC8.curr)[(int)i];}}_tmpC1;});
{int i=0;for(0;i < _tmpBB;++ i){
({void*_tmp31B=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,((void**)_tmpC8.curr)[i]);*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=_tmp31B;});
if(*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))!= ((void**)_tmpC8.curr)[i])
change=1;}}
# 412
if(change)
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpBC=_region_malloc(fenv->r,sizeof(*_tmpBC));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp31C=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmpBD;_tmpBD.tag=6;_tmpBD.f1=_tmpC9;_tmpBD.f2=d2;_tmpBD;});_tmpBC[0]=_tmp31C;});_tmpBC;});else{
return r;}}case 8: _LL66: _tmpCB=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpBA)->f1;_tmpCA=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpBA)->f2;_LL67: {
# 416
void*_tmpC2=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,_tmpCA);
if(_tmpC2 != _tmpCA)
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpC3=_region_malloc(fenv->r,sizeof(*_tmpC3));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp31D=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmpC4;_tmpC4.tag=8;_tmpC4.f1=_tmpCB;_tmpC4.f2=_tmpC2;_tmpC4;});_tmpC3[0]=_tmp31D;});_tmpC3;});else{
return r;}}default: _LL68: _LL69:
 return r;}_LL61:;}
# 426
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r){
struct _tuple0 _tmpCC=({struct _tuple0 _tmpF1;({void*_tmp31E=Cyc_Tcutil_compress(t);_tmpF1.f1=_tmp31E;});_tmpF1.f2=r;_tmpF1;});struct _tuple0 _tmpCD=_tmpCC;enum Cyc_Absyn_AggrKind _tmpF0;struct Cyc_List_List*_tmpEF;struct Cyc_CfFlowInfo_UnionRInfo _tmpEE;struct _dyneither_ptr _tmpED;union Cyc_Absyn_AggrInfoU _tmpEC;struct Cyc_CfFlowInfo_UnionRInfo _tmpEB;struct _dyneither_ptr _tmpEA;struct Cyc_List_List*_tmpE9;struct Cyc_CfFlowInfo_UnionRInfo _tmpE8;struct _dyneither_ptr _tmpE7;struct Cyc_Absyn_Vardecl*_tmpE6;void*_tmpE5;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpCD.f2)->tag == 8){_LL6B: _tmpE6=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpCD.f2)->f1;_tmpE5=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpCD.f2)->f2;_LL6C: {
# 429
void*_tmpCE=Cyc_CfFlowInfo_make_unique_consumed(fenv,t,consumer,iteration,_tmpE5);
if(_tmpCE != _tmpE5)return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpCF=_region_malloc(fenv->r,sizeof(*_tmpCF));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp31F=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmpD0;_tmpD0.tag=8;_tmpD0.f1=_tmpE6;_tmpD0.f2=_tmpCE;_tmpD0;});_tmpCF[0]=_tmp31F;});_tmpCF;});else{
return r;}}}else{switch(*((int*)_tmpCD.f1)){case 10: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCD.f2)->tag == 6){_LL6D: _tmpE9=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCD.f1)->f1;_tmpE8=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCD.f2)->f1;_tmpE7=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCD.f2)->f2;_LL6E: {
# 433
unsigned int _tmpD1=_get_dyneither_size(_tmpE7,sizeof(void*));
struct _dyneither_ptr d2=({unsigned int _tmpD4=_tmpD1;void**_tmpD5=(void**)({struct _RegionHandle*_tmp320=fenv->r;_region_malloc(_tmp320,_check_times(sizeof(void*),_tmpD4));});struct _dyneither_ptr _tmpD7=_tag_dyneither(_tmpD5,sizeof(void*),_tmpD4);{unsigned int _tmpD6=_tmpD4;unsigned int i;for(i=0;i < _tmpD6;i ++){_tmpD5[i]=(void*)fenv->unknown_all;}}_tmpD7;});
{int i=0;for(0;i < _tmpD1;++ i){
({void*_tmp321=Cyc_CfFlowInfo_make_unique_consumed(fenv,(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmpE9))->hd)).f2,consumer,iteration,((void**)_tmpE7.curr)[i]);*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=_tmp321;});
_tmpE9=_tmpE9->tl;}}
# 439
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpD2=_region_malloc(fenv->r,sizeof(*_tmpD2));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp322=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmpD3;_tmpD3.tag=6;_tmpD3.f1=_tmpE8;_tmpD3.f2=d2;_tmpD3;});_tmpD2[0]=_tmp322;});_tmpD2;});}}else{goto _LL73;}case 11: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCD.f2)->tag == 6){_LL6F: _tmpEC=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCD.f1)->f1).aggr_info;_tmpEB=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCD.f2)->f1;_tmpEA=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCD.f2)->f2;_LL70: {
# 441
struct Cyc_Absyn_Aggrdecl*_tmpD8=Cyc_Absyn_get_known_aggrdecl(_tmpEC);
if(_tmpD8->impl == 0)return r;
_tmpF0=_tmpD8->kind;_tmpEF=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD8->impl))->fields;_tmpEE=_tmpEB;_tmpED=_tmpEA;goto _LL72;}}else{goto _LL73;}case 12: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCD.f2)->tag == 6){_LL71: _tmpF0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCD.f1)->f1;_tmpEF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCD.f1)->f2;_tmpEE=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCD.f2)->f1;_tmpED=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCD.f2)->f2;_LL72: {
# 445
struct _dyneither_ptr d2=({unsigned int _tmpDF=_get_dyneither_size(_tmpED,sizeof(void*));void**_tmpE0=(void**)({struct _RegionHandle*_tmp323=fenv->r;_region_malloc(_tmp323,_check_times(sizeof(void*),_tmpDF));});struct _dyneither_ptr _tmpE2=_tag_dyneither(_tmpE0,sizeof(void*),_tmpDF);{unsigned int _tmpE1=_tmpDF;unsigned int i;for(i=0;i < _tmpE1;i ++){_tmpE0[i]=(void*)((void**)_tmpED.curr)[(int)i];}}_tmpE2;});
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpEF);
{int i=0;for(0;i < sz;(i ++,_tmpEF=_tmpEF->tl)){
struct Cyc_Absyn_Aggrfield*_tmpD9=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmpEF))->hd;struct Cyc_Absyn_Aggrfield*_tmpDA=_tmpD9;struct _dyneither_ptr*_tmpDC;void*_tmpDB;_LL76: _tmpDC=_tmpDA->name;_tmpDB=_tmpDA->type;_LL77:;
if(_get_dyneither_size(*_tmpDC,sizeof(char))!= 1)
({void*_tmp324=Cyc_CfFlowInfo_make_unique_consumed(fenv,_tmpDB,consumer,iteration,*((void**)_check_dyneither_subscript(_tmpED,sizeof(void*),i)));*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=_tmp324;});}}
# 452
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpDD=_region_malloc(fenv->r,sizeof(*_tmpDD));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp325=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmpDE;_tmpDE.tag=6;_tmpDE.f1=_tmpEE;_tmpDE.f2=d2;_tmpDE;});_tmpDD[0]=_tmp325;});_tmpDD;});}}else{goto _LL73;}default: _LL73: _LL74:
# 454
 if(Cyc_Tcutil_is_noalias_pointer(t,0))
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpE3=_region_malloc(fenv->r,sizeof(*_tmpE3));({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp326=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmpE4;_tmpE4.tag=7;_tmpE4.f1=consumer;_tmpE4.f2=iteration;_tmpE4.f3=r;_tmpE4;});_tmpE3[0]=_tmp326;});_tmpE3;});else{
return r;}}}_LL6A:;}struct _tuple18{struct Cyc_CfFlowInfo_Place*f1;unsigned int f2;};
# 460
static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){
# 463
struct _RegionHandle _tmpF2=_new_region("r");struct _RegionHandle*r=& _tmpF2;_push_region(r);
{struct _tuple18 elem=({struct _tuple18 _tmpFE;_tmpFE.f1=place;_tmpFE.f2=0;_tmpFE;});
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,set);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(iter,& elem)){
struct Cyc_CfFlowInfo_Place*_tmpF3=elem.f1;
struct Cyc_CfFlowInfo_Place _tmpF4=*place;struct Cyc_CfFlowInfo_Place _tmpF5=_tmpF4;void*_tmpFC;struct Cyc_List_List*_tmpFB;_LL79: _tmpFC=_tmpF5.root;_tmpFB=_tmpF5.fields;_LL7A:;{
struct Cyc_CfFlowInfo_Place _tmpF6=*_tmpF3;struct Cyc_CfFlowInfo_Place _tmpF7=_tmpF6;void*_tmpFA;struct Cyc_List_List*_tmpF9;_LL7C: _tmpFA=_tmpF7.root;_tmpF9=_tmpF7.fields;_LL7D:;
if(({Cyc_CfFlowInfo_root_cmp(_tmpFC,_tmpFA)!= 0;}))
continue;
for(0;_tmpFB != 0  && _tmpF9 != 0;(_tmpFB=_tmpFB->tl,_tmpF9=_tmpF9->tl)){
if((int)_tmpFB->hd != (int)_tmpF9->hd)break;}
if(_tmpFB == 0){
int _tmpF8=1;_npop_handler(0);return _tmpF8;}};}{
# 477
int _tmpFD=0;_npop_handler(0);return _tmpFD;};}
# 464
;_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*rgn;struct Cyc_List_List*places;};
# 490
typedef struct Cyc_CfFlowInfo_EscPile*Cyc_CfFlowInfo_escpile_t;
# 492
static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){
# 495
if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place))
({struct Cyc_List_List*_tmp327=({struct Cyc_List_List*_tmpFF=_region_malloc(pile->rgn,sizeof(*_tmpFF));_tmpFF->hd=place;_tmpFF->tl=pile->places;_tmpFF;});pile->places=_tmp327;});}
# 498
static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*r){
void*_tmp100=r;struct Cyc_CfFlowInfo_UnionRInfo _tmp105;struct _dyneither_ptr _tmp104;struct Cyc_CfFlowInfo_Place*_tmp103;void*_tmp102;void*_tmp101;switch(*((int*)_tmp100)){case 7: _LL7F: _tmp101=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp100)->f3;_LL80:
 Cyc_CfFlowInfo_add_places(pile,_tmp101);return;case 8: _LL81: _tmp102=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp100)->f2;_LL82:
 Cyc_CfFlowInfo_add_places(pile,_tmp102);return;case 5: _LL83: _tmp103=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp100)->f1;_LL84:
 Cyc_CfFlowInfo_add_place(pile,_tmp103);return;case 6: _LL85: _tmp105=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp100)->f1;_tmp104=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp100)->f2;_LL86:
# 504
{int i=0;for(0;i < _get_dyneither_size(_tmp104,sizeof(void*));++ i){
Cyc_CfFlowInfo_add_places(pile,((void**)_tmp104.curr)[i]);}}
return;default: _LL87: _LL88:
 return;}_LL7E:;}
# 514
static void*Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*old_val){
# 516
void*_tmp106=old_val;struct Cyc_Absyn_Vardecl*_tmp118;void*_tmp117;struct Cyc_Absyn_Exp*_tmp116;int _tmp115;void*_tmp114;int _tmp113;struct _dyneither_ptr _tmp112;switch(*((int*)_tmp106)){case 6: _LL8A: _tmp113=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp106)->f1).is_union;_tmp112=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp106)->f2;_LL8B: {
# 518
struct _dyneither_ptr d2=({unsigned int _tmp10A=_get_dyneither_size(_tmp112,sizeof(void*));void**_tmp10B=(void**)({struct _RegionHandle*_tmp328=r;_region_malloc(_tmp328,_check_times(sizeof(void*),_tmp10A));});struct _dyneither_ptr _tmp10D=_tag_dyneither(_tmp10B,sizeof(void*),_tmp10A);{unsigned int _tmp10C=_tmp10A;unsigned int i;for(i=0;i < _tmp10C;i ++){({typeof((void*)
Cyc_CfFlowInfo_insert_place_inner(r,new_val,((void**)_tmp112.curr)[(int)i]))_tmp329=(void*)Cyc_CfFlowInfo_insert_place_inner(r,new_val,((void**)_tmp112.curr)[(int)i]);_tmp10B[i]=_tmp329;});}}_tmp10D;});
# 522
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp107=_region_malloc(r,sizeof(*_tmp107));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp32B=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp108;_tmp108.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp32A=({struct Cyc_CfFlowInfo_UnionRInfo _tmp109;_tmp109.is_union=_tmp113;_tmp109.fieldnum=- 1;_tmp109;});_tmp108.f1=_tmp32A;});_tmp108.f2=d2;_tmp108;});_tmp107[0]=_tmp32B;});_tmp107;});}case 7: _LL8C: _tmp116=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp106)->f1;_tmp115=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp106)->f2;_tmp114=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp106)->f3;_LL8D:
# 524
 return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp10E=_region_malloc(r,sizeof(*_tmp10E));({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp32D=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp10F;_tmp10F.tag=7;_tmp10F.f1=_tmp116;_tmp10F.f2=_tmp115;({void*_tmp32C=Cyc_CfFlowInfo_insert_place_inner(r,new_val,_tmp114);_tmp10F.f3=_tmp32C;});_tmp10F;});_tmp10E[0]=_tmp32D;});_tmp10E;});case 8: _LL8E: _tmp118=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp106)->f1;_tmp117=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp106)->f2;_LL8F:
# 526
 return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp110=_region_malloc(r,sizeof(*_tmp110));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp32F=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp111;_tmp111.tag=8;_tmp111.f1=_tmp118;({void*_tmp32E=Cyc_CfFlowInfo_insert_place_inner(r,new_val,_tmp117);_tmp111.f2=_tmp32E;});_tmp111;});_tmp110[0]=_tmp32F;});_tmp110;});default: _LL90: _LL91:
 return new_val;}_LL89:;}
# 533
static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){
# 535
void*_tmp119=*((void**)_check_dyneither_subscript(d,sizeof(void*),n));
if(_tmp119 == rval)return d;{
struct _dyneither_ptr res=({unsigned int _tmp11A=_get_dyneither_size(d,sizeof(void*));void**_tmp11B=(void**)({struct _RegionHandle*_tmp330=r;_region_malloc(_tmp330,_check_times(sizeof(void*),_tmp11A));});struct _dyneither_ptr _tmp11D=_tag_dyneither(_tmp11B,sizeof(void*),_tmp11A);{unsigned int _tmp11C=_tmp11A;unsigned int i;for(i=0;i < _tmp11C;i ++){_tmp11B[i]=(void*)((void**)d.curr)[(int)i];}}_tmp11D;});
*((void**)_check_dyneither_subscript(res,sizeof(void*),n))=rval;
return res;};}struct _tuple19{struct Cyc_List_List*f1;void*f2;};
# 547
static void*Cyc_CfFlowInfo_insert_place_outer(struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val){
# 551
if(fs == 0)
return Cyc_CfFlowInfo_insert_place_inner(r,new_val,old_val);{
struct _tuple19 _tmp11E=({struct _tuple19 _tmp131;_tmp131.f1=fs;_tmp131.f2=old_val;_tmp131;});struct _tuple19 _tmp11F=_tmp11E;struct Cyc_Absyn_Vardecl*_tmp130;void*_tmp12F;int _tmp12E;struct Cyc_List_List*_tmp12D;int _tmp12C;struct _dyneither_ptr _tmp12B;if(_tmp11F.f1 != 0)switch(*((int*)_tmp11F.f2)){case 6: _LL93: _tmp12E=(int)(_tmp11F.f1)->hd;_tmp12D=(_tmp11F.f1)->tl;_tmp12C=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp11F.f2)->f1).is_union;_tmp12B=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp11F.f2)->f2;_LL94: {
# 555
void*_tmp120=Cyc_CfFlowInfo_insert_place_outer(r,_tmp12D,*((void**)_check_dyneither_subscript(_tmp12B,sizeof(void*),_tmp12E)),new_val);
struct _dyneither_ptr _tmp121=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmp12B,_tmp12E,_tmp120);
if((void**)_tmp121.curr == (void**)_tmp12B.curr)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp122=_region_malloc(r,sizeof(*_tmp122));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp332=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp123;_tmp123.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp331=({struct Cyc_CfFlowInfo_UnionRInfo _tmp124;_tmp124.is_union=_tmp12C;_tmp124.fieldnum=- 1;_tmp124;});_tmp123.f1=_tmp331;});_tmp123.f2=_tmp121;_tmp123;});_tmp122[0]=_tmp332;});_tmp122;});}}case 8: goto _LL95;default: goto _LL97;}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp11F.f2)->tag == 8){_LL95: _tmp130=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp11F.f2)->f1;_tmp12F=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp11F.f2)->f2;_LL96: {
# 560
void*_tmp125=Cyc_CfFlowInfo_insert_place_outer(r,fs,_tmp12F,new_val);
if(_tmp125 == _tmp12F)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp126=_region_malloc(r,sizeof(*_tmp126));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp333=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp127;_tmp127.tag=8;_tmp127.f1=_tmp130;_tmp127.f2=_tmp125;_tmp127;});_tmp126[0]=_tmp333;});_tmp126;});}}}else{_LL97: _LL98:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp128=_cycalloc(sizeof(*_tmp128));({struct Cyc_Core_Impossible_exn_struct _tmp335=({struct Cyc_Core_Impossible_exn_struct _tmp129;_tmp129.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp334=({const char*_tmp12A="bad insert place";_tag_dyneither(_tmp12A,sizeof(char),17);});_tmp129.f1=_tmp334;});_tmp129;});_tmp128[0]=_tmp335;});_tmp128;}));}}_LL92:;};}
# 570
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*all_changed,struct Cyc_Dict_Dict d){
# 574
while(pile->places != 0){
struct Cyc_CfFlowInfo_Place*_tmp132=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(all_changed,_tmp132,0);{
void*oldval;void*newval;
{struct _handler_cons _tmp133;_push_handler(& _tmp133);{int _tmp135=0;if(setjmp(_tmp133.handler))_tmp135=1;if(!_tmp135){({void*_tmp336=Cyc_CfFlowInfo_lookup_place(d,_tmp132);oldval=_tmp336;});;_pop_handler();}else{void*_tmp134=(void*)_exn_thrown;void*_tmp136=_tmp134;void*_tmp137;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp136)->tag == Cyc_Dict_Absent){_LL9A: _LL9B:
 continue;}else{_LL9C: _tmp137=_tmp136;_LL9D:(int)_rethrow(_tmp137);}_LL99:;}};}
{enum Cyc_CfFlowInfo_InitLevel _tmp138=Cyc_CfFlowInfo_initlevel(fenv,d,oldval);enum Cyc_CfFlowInfo_InitLevel _tmp139=_tmp138;switch(_tmp139){case Cyc_CfFlowInfo_AllIL: _LL9F: _LLA0:
 newval=fenv->esc_all;goto _LL9E;case Cyc_CfFlowInfo_ThisIL: _LLA1: _LLA2:
 newval=fenv->esc_this;goto _LL9E;default: _LLA3: _LLA4:
 newval=fenv->esc_none;goto _LL9E;}_LL9E:;}
# 586
Cyc_CfFlowInfo_add_places(pile,oldval);{
struct Cyc_CfFlowInfo_Place _tmp13A=*_tmp132;struct Cyc_CfFlowInfo_Place _tmp13B=_tmp13A;void*_tmp13D;struct Cyc_List_List*_tmp13C;_LLA6: _tmp13D=_tmp13B.root;_tmp13C=_tmp13B.fields;_LLA7:;
({struct Cyc_Dict_Dict _tmp33C=({struct Cyc_Dict_Dict _tmp33B=d;void*_tmp33A=_tmp13D;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp33B,_tmp33A,({
struct _RegionHandle*_tmp339=fenv->r;struct Cyc_List_List*_tmp338=_tmp13C;void*_tmp337=
# 591
((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp13D);
# 589
Cyc_CfFlowInfo_insert_place_outer(_tmp339,_tmp338,_tmp337,newval);}));});
# 588
d=_tmp33C;});};};}
# 594
return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct _RegionHandle*e;struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};
# 606
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(void*r){
void*_tmp13E=r;void*_tmp144;enum Cyc_CfFlowInfo_InitLevel _tmp143;enum Cyc_CfFlowInfo_InitLevel _tmp142;switch(*((int*)_tmp13E)){case 3: _LLA9: _tmp142=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp13E)->f1;_LLAA:
 return _tmp142;case 4: _LLAB: _tmp143=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp13E)->f1;_LLAC:
 return _tmp143;case 0: _LLAD: _LLAE:
 goto _LLB0;case 1: _LLAF: _LLB0:
 return Cyc_CfFlowInfo_AllIL;case 2: _LLB1: _LLB2:
 return Cyc_CfFlowInfo_ThisIL;case 7: _LLB3: _tmp144=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13E)->f3;_LLB4:
 return Cyc_CfFlowInfo_NoneIL;default: _LLB5: _LLB6:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp13F=_cycalloc(sizeof(*_tmp13F));({struct Cyc_Core_Impossible_exn_struct _tmp33E=({struct Cyc_Core_Impossible_exn_struct _tmp140;_tmp140.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp33D=({const char*_tmp141="initlevel_approx";_tag_dyneither(_tmp141,sizeof(char),17);});_tmp140.f1=_tmp33D;});_tmp140;});_tmp13F[0]=_tmp33E;});_tmp13F;}));}_LLA8:;}
# 617
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){
# 619
enum Cyc_CfFlowInfo_InitLevel this_ans;
if(acc == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;
{void*_tmp145=r;struct Cyc_CfFlowInfo_Place*_tmp14E;struct _dyneither_ptr _tmp14D;int _tmp14C;int _tmp14B;struct _dyneither_ptr _tmp14A;void*_tmp149;switch(*((int*)_tmp145)){case 8: _LLB8: _tmp149=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp145)->f2;_LLB9:
 return Cyc_CfFlowInfo_initlevel_rec(env,_tmp149,acc);case 6: _LLBA: _tmp14C=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp145)->f1).is_union;_tmp14B=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp145)->f1).fieldnum;_tmp14A=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp145)->f2;if(_tmp14C){_LLBB: {
# 627
unsigned int _tmp146=_get_dyneither_size(_tmp14A,sizeof(void*));
this_ans=Cyc_CfFlowInfo_NoneIL;
if(_tmp14B == - 1){
int i=0;for(0;i < _tmp146;++ i){
if(({Cyc_CfFlowInfo_initlevel_rec(env,((void**)_tmp14A.curr)[i],Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL;})){
this_ans=Cyc_CfFlowInfo_AllIL;
break;}}}else{
# 637
if(({Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_dyneither_subscript(_tmp14A,sizeof(void*),_tmp14B)),Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL;}))
this_ans=Cyc_CfFlowInfo_AllIL;}
goto _LLB7;}}else{_LLBC: _tmp14D=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp145)->f2;_LLBD: {
# 641
unsigned int _tmp147=_get_dyneither_size(_tmp14D,sizeof(void*));
this_ans=Cyc_CfFlowInfo_AllIL;
{int i=0;for(0;i < _tmp147;++ i){
({enum Cyc_CfFlowInfo_InitLevel _tmp33F=Cyc_CfFlowInfo_initlevel_rec(env,((void**)_tmp14D.curr)[i],this_ans);this_ans=_tmp33F;});}}
goto _LLB7;}}case 5: _LLBE: _tmp14E=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp145)->f1;_LLBF:
# 647
 if(((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,_tmp14E))
this_ans=Cyc_CfFlowInfo_AllIL;else{
# 650
({struct Cyc_List_List*_tmp340=({struct Cyc_List_List*_tmp148=_region_malloc(env->e,sizeof(*_tmp148));_tmp148->hd=_tmp14E;_tmp148->tl=env->seen;_tmp148;});env->seen=_tmp340;});
({enum Cyc_CfFlowInfo_InitLevel _tmp342=({struct Cyc_CfFlowInfo_InitlevelEnv*_tmp341=env;Cyc_CfFlowInfo_initlevel_rec(_tmp341,Cyc_CfFlowInfo_lookup_place(env->d,_tmp14E),Cyc_CfFlowInfo_AllIL);});this_ans=_tmp342;});
env->seen=((struct Cyc_List_List*)_check_null(env->seen))->tl;
if(this_ans == Cyc_CfFlowInfo_NoneIL)
this_ans=Cyc_CfFlowInfo_ThisIL;}
# 656
goto _LLB7;default: _LLC0: _LLC1:
({enum Cyc_CfFlowInfo_InitLevel _tmp343=Cyc_CfFlowInfo_initlevel_approx(r);this_ans=_tmp343;});}_LLB7:;}
# 659
if(this_ans == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;
if(this_ans == Cyc_CfFlowInfo_ThisIL  || acc == Cyc_CfFlowInfo_ThisIL)return Cyc_CfFlowInfo_ThisIL;
return Cyc_CfFlowInfo_AllIL;}
# 663
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*env,struct Cyc_Dict_Dict d,void*r){
struct _RegionHandle*_tmp14F=env->r;
struct Cyc_CfFlowInfo_InitlevelEnv _tmp150=({struct Cyc_CfFlowInfo_InitlevelEnv _tmp151;_tmp151.e=_tmp14F;_tmp151.d=d;_tmp151.seen=0;_tmp151;});
return Cyc_CfFlowInfo_initlevel_rec(& _tmp150,r,Cyc_CfFlowInfo_AllIL);}struct _tuple20{void*f1;int f2;};
# 670
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _tmp152=*place;struct Cyc_CfFlowInfo_Place _tmp153=_tmp152;void*_tmp161;struct Cyc_List_List*_tmp160;_LLC3: _tmp161=_tmp153.root;_tmp160=_tmp153.fields;_LLC4:;{
void*_tmp154=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp161);
for(0;_tmp160 != 0;_tmp160=_tmp160->tl){
retry: {struct _tuple20 _tmp155=({struct _tuple20 _tmp15F;_tmp15F.f1=_tmp154;_tmp15F.f2=(int)_tmp160->hd;_tmp15F;});struct _tuple20 _tmp156=_tmp155;struct Cyc_CfFlowInfo_UnionRInfo _tmp15E;struct _dyneither_ptr _tmp15D;int _tmp15C;void*_tmp15B;int _tmp15A;switch(*((int*)_tmp156.f1)){case 8: _LLC6: _tmp15B=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp156.f1)->f2;_tmp15A=_tmp156.f2;_LLC7:
# 676
 _tmp154=_tmp15B;goto retry;case 6: _LLC8: _tmp15E=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp156.f1)->f1;_tmp15D=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp156.f1)->f2;_tmp15C=_tmp156.f2;_LLC9:
# 678
 _tmp154=*((void**)_check_dyneither_subscript(_tmp15D,sizeof(void*),_tmp15C));
goto _LLC5;default: _LLCA: _LLCB:
# 684
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp157=_cycalloc(sizeof(*_tmp157));({struct Cyc_Core_Impossible_exn_struct _tmp345=({struct Cyc_Core_Impossible_exn_struct _tmp158;_tmp158.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp344=({const char*_tmp159="bad lookup_place";_tag_dyneither(_tmp159,sizeof(char),17);});_tmp158.f1=_tmp344;});_tmp158;});_tmp157[0]=_tmp345;});_tmp157;}));}_LLC5:;}}
# 686
return _tmp154;};}
# 689
static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){
void*_tmp162=rval;int _tmp167;int _tmp166;struct _dyneither_ptr _tmp165;void*_tmp164;void*_tmp163;switch(*((int*)_tmp162)){case 4: _LLCD: _LLCE:
 return 0;case 7: _LLCF: _tmp163=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp162)->f3;_LLD0:
 return Cyc_CfFlowInfo_is_rval_unescaped(_tmp163);case 8: _LLD1: _tmp164=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp162)->f2;_LLD2:
 return Cyc_CfFlowInfo_is_rval_unescaped(_tmp164);case 6: _LLD3: _tmp167=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp162)->f1).is_union;_tmp166=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp162)->f1).fieldnum;_tmp165=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp162)->f2;_LLD4:
# 695
 if(_tmp167  && _tmp166 != - 1)
return !Cyc_CfFlowInfo_is_rval_unescaped(*((void**)_check_dyneither_subscript(_tmp165,sizeof(void*),_tmp166)));else{
# 698
unsigned int sz=_get_dyneither_size(_tmp165,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)_tmp165.curr)[i]))return 0;}}
return 1;}default: _LLD5: _LLD6:
# 703
 return 1;}_LLCC:;}
# 706
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(d,place));}
# 711
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r){
struct _RegionHandle*_tmp168=fenv->r;
struct Cyc_CfFlowInfo_EscPile*pile=({struct Cyc_CfFlowInfo_EscPile*_tmp169=_region_malloc(_tmp168,sizeof(*_tmp169));_tmp169->rgn=_tmp168;_tmp169->places=0;_tmp169;});
Cyc_CfFlowInfo_add_places(pile,r);
return Cyc_CfFlowInfo_escape_these(fenv,pile,all_changed,d);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;unsigned int loc;struct Cyc_CfFlowInfo_Place*root_place;struct Cyc_Dict_Dict*all_changed;};
# 729
static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){
# 734
struct _tuple0 _tmp16A=({struct _tuple0 _tmp181;_tmp181.f1=oldval;_tmp181.f2=newval;_tmp181;});struct _tuple0 _tmp16B=_tmp16A;enum Cyc_CfFlowInfo_InitLevel _tmp180;struct Cyc_CfFlowInfo_UnionRInfo _tmp17F;struct _dyneither_ptr _tmp17E;struct Cyc_CfFlowInfo_UnionRInfo _tmp17D;struct _dyneither_ptr _tmp17C;struct Cyc_CfFlowInfo_Place*_tmp17B;struct Cyc_Absyn_Vardecl*_tmp17A;void*_tmp179;void*_tmp178;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp16B.f1)->tag == 8){_LLD8: _tmp178=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp16B.f1)->f2;_LLD9:
# 736
 return Cyc_CfFlowInfo_assign_place_inner(env,_tmp178,newval);}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp16B.f2)->tag == 8){_LLDA: _tmp17A=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp16B.f2)->f1;_tmp179=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp16B.f2)->f2;_LLDB: {
# 738
void*_tmp16C=Cyc_CfFlowInfo_assign_place_inner(env,oldval,_tmp179);
if(_tmp16C == _tmp179)return newval;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp16D=_region_malloc((env->fenv)->r,sizeof(*_tmp16D));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp346=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp16E;_tmp16E.tag=8;_tmp16E.f1=_tmp17A;_tmp16E.f2=_tmp16C;_tmp16E;});_tmp16D[0]=_tmp346;});_tmp16D;});}}}else{switch(*((int*)_tmp16B.f1)){case 4: if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp16B.f2)->tag == 5){_LLDC: _tmp17B=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp16B.f2)->f1;_LLDD:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp17B);goto _LLDF;}else{_LLDE: _LLDF:
# 743
 if(({Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= Cyc_CfFlowInfo_AllIL;}))
({void*_tmp16F=0;({unsigned int _tmp348=env->loc;struct _dyneither_ptr _tmp347=({const char*_tmp170="assignment puts possibly-uninitialized data in an escaped location";_tag_dyneither(_tmp170,sizeof(char),67);});Cyc_CfFlowInfo_aerr(_tmp348,_tmp347,_tag_dyneither(_tmp16F,sizeof(void*),0));});});
# 746
return(env->fenv)->esc_all;}case 6: switch(*((int*)_tmp16B.f2)){case 6: _LLE0: _tmp17F=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp16B.f1)->f1;_tmp17E=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp16B.f1)->f2;_tmp17D=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp16B.f2)->f1;_tmp17C=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp16B.f2)->f2;_LLE1: {
# 748
struct _dyneither_ptr new_d=({unsigned int _tmp173=
_get_dyneither_size(_tmp17E,sizeof(void*));void**_tmp174=(void**)({struct _RegionHandle*_tmp349=(env->fenv)->r;_region_malloc(_tmp349,_check_times(sizeof(void*),_tmp173));});struct _dyneither_ptr _tmp176=_tag_dyneither(_tmp174,sizeof(void*),_tmp173);{unsigned int _tmp175=_tmp173;unsigned int i;for(i=0;i < _tmp175;i ++){({typeof((void*)Cyc_CfFlowInfo_assign_place_inner(env,((void**)_tmp17E.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp17C,sizeof(void*),(int)i))))_tmp34A=(void*)Cyc_CfFlowInfo_assign_place_inner(env,((void**)_tmp17E.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp17C,sizeof(void*),(int)i)));_tmp174[i]=_tmp34A;});}}_tmp176;});
# 752
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp17E,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp17E.curr)[i]){
change=1;break;}}}
# 757
if(!change){
if(!_tmp17F.is_union)return oldval;
new_d=_tmp17E;}else{
# 762
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp17E,sizeof(void*));++ i){
if(({void*_tmp34B=*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i));_tmp34B != *((void**)_check_dyneither_subscript(_tmp17C,sizeof(void*),i));})){
change=1;break;}}}
# 767
if(!change){
if(!_tmp17F.is_union)return newval;
new_d=_tmp17C;}}
# 772
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp171=_region_malloc((env->fenv)->r,sizeof(*_tmp171));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp34C=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp172;_tmp172.tag=6;_tmp172.f1=_tmp17D;_tmp172.f2=new_d;_tmp172;});_tmp171[0]=_tmp34C;});_tmp171;});}case 4: goto _LLE2;default: goto _LLE4;}default: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp16B.f2)->tag == 4){_LLE2: _tmp180=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp16B.f2)->f1;_LLE3: {
# 774
enum Cyc_CfFlowInfo_InitLevel _tmp177=_tmp180;switch(_tmp177){case Cyc_CfFlowInfo_NoneIL: _LLE7: _LLE8:
 return(env->fenv)->unknown_none;case Cyc_CfFlowInfo_ThisIL: _LLE9: _LLEA:
 return(env->fenv)->unknown_this;default: _LLEB: _LLEC:
 return(env->fenv)->unknown_all;}_LLE6:;}}else{_LLE4: _LLE5:
# 779
 return newval;}}}}_LLD7:;}
# 785
static int Cyc_CfFlowInfo_nprefix(int*n,void*unused){
if(*n > 0){*n=*n - 1;return 1;}else{
return 0;}}
# 789
static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*fs,int fs_prefix,void*oldval,void*newval){
# 794
if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple19 _tmp182=({struct _tuple19 _tmp19F;_tmp19F.f1=fs;_tmp19F.f2=oldval;_tmp19F;});struct _tuple19 _tmp183=_tmp182;int _tmp19E;struct Cyc_List_List*_tmp19D;int _tmp19C;int _tmp19B;struct _dyneither_ptr _tmp19A;struct Cyc_Absyn_Vardecl*_tmp199;void*_tmp198;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp183.f2)->tag == 8){_LLEE: _tmp199=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp183.f2)->f1;_tmp198=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp183.f2)->f2;_LLEF: {
# 797
void*_tmp184=Cyc_CfFlowInfo_assign_place_outer(env,fs,fs_prefix,_tmp198,newval);
if(_tmp184 == _tmp198)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp185=_region_malloc((env->fenv)->r,sizeof(*_tmp185));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp34D=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp186;_tmp186.tag=8;_tmp186.f1=_tmp199;_tmp186.f2=_tmp184;_tmp186;});_tmp185[0]=_tmp34D;});_tmp185;});}}}else{if(_tmp183.f1 != 0){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp183.f2)->tag == 6){_LLF0: _tmp19E=(int)(_tmp183.f1)->hd;_tmp19D=(_tmp183.f1)->tl;_tmp19C=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp183.f2)->f1).is_union;_tmp19B=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp183.f2)->f1).fieldnum;_tmp19A=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp183.f2)->f2;_LLF1: {
# 801
void*_tmp187=Cyc_CfFlowInfo_assign_place_outer(env,_tmp19D,fs_prefix + 1,*((void**)_check_dyneither_subscript(_tmp19A,sizeof(void*),_tmp19E)),newval);
# 803
struct _dyneither_ptr _tmp188=Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,_tmp19A,_tmp19E,_tmp187);
if((void**)_tmp188.curr == (void**)_tmp19A.curr  && (!_tmp19C  || _tmp19B == _tmp19E))return oldval;
_tmp19A=_tmp188;
# 808
if(_tmp19C){
int changed=0;
int sz=(int)_get_dyneither_size(_tmp19A,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(i != _tmp19E){
struct _dyneither_ptr _tmp189=({
struct _RegionHandle*_tmp350=(env->fenv)->r;struct _dyneither_ptr _tmp34F=_tmp19A;int _tmp34E=i;Cyc_CfFlowInfo_aggr_dict_insert(_tmp350,_tmp34F,_tmp34E,
Cyc_CfFlowInfo_insert_place_inner((env->fenv)->r,(env->fenv)->unknown_all,*((void**)_check_dyneither_subscript(_tmp19A,sizeof(void*),i))));});
# 818
if((void**)_tmp189.curr != (void**)_tmp19A.curr){
_tmp19A=_tmp189;
changed=1;}}}}
# 826
if(changed){
struct Cyc_CfFlowInfo_Place*_tmp18A=env->root_place;struct Cyc_CfFlowInfo_Place*_tmp18B=_tmp18A;void*_tmp191;struct Cyc_List_List*_tmp190;_LLF5: _tmp191=_tmp18B->root;_tmp190=_tmp18B->fields;_LLF6:;{
struct Cyc_List_List*_tmp18C=({struct _RegionHandle*_tmp352=(env->pile)->rgn;int*_tmp351=({int*_tmp18F=_cycalloc_atomic(sizeof(*_tmp18F));_tmp18F[0]=fs_prefix;_tmp18F;});((struct Cyc_List_List*(*)(struct _RegionHandle*r,int(*f)(int*,int),int*env,struct Cyc_List_List*x))Cyc_List_rfilter_c)(_tmp352,(int(*)(int*n,int unused))Cyc_CfFlowInfo_nprefix,_tmp351,_tmp190);});
struct Cyc_CfFlowInfo_Place*_tmp18D=({struct Cyc_CfFlowInfo_Place*_tmp18E=_region_malloc((env->pile)->rgn,sizeof(*_tmp18E));_tmp18E->root=_tmp191;_tmp18E->fields=_tmp18C;_tmp18E;});
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,_tmp18D,0);};}}
# 833
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp192=_region_malloc((env->fenv)->r,sizeof(*_tmp192));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp354=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp193;_tmp193.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp353=({struct Cyc_CfFlowInfo_UnionRInfo _tmp194;_tmp194.is_union=_tmp19C;_tmp194.fieldnum=_tmp19E;_tmp194;});_tmp193.f1=_tmp353;});_tmp193.f2=_tmp19A;_tmp193;});_tmp192[0]=_tmp354;});_tmp192;});}}else{goto _LLF2;}}else{_LLF2: _LLF3:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp195=_cycalloc(sizeof(*_tmp195));({struct Cyc_Core_Impossible_exn_struct _tmp356=({struct Cyc_Core_Impossible_exn_struct _tmp196;_tmp196.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp355=({const char*_tmp197="bad assign place";_tag_dyneither(_tmp197,sizeof(char),17);});_tmp196.f1=_tmp355;});_tmp196;});_tmp195[0]=_tmp356;});_tmp195;}));}}_LLED:;};}
# 837
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){
# 848
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(all_changed,place,0);{
struct _RegionHandle*_tmp1A0=fenv->r;
struct Cyc_CfFlowInfo_Place*_tmp1A1=place;void*_tmp1A5;struct Cyc_List_List*_tmp1A4;_LLF8: _tmp1A5=_tmp1A1->root;_tmp1A4=_tmp1A1->fields;_LLF9:;{
struct Cyc_CfFlowInfo_AssignEnv env=({struct Cyc_CfFlowInfo_AssignEnv _tmp1A2;_tmp1A2.fenv=fenv;({struct Cyc_CfFlowInfo_EscPile*_tmp357=({struct Cyc_CfFlowInfo_EscPile*_tmp1A3=_region_malloc(_tmp1A0,sizeof(*_tmp1A3));_tmp1A3->rgn=_tmp1A0;_tmp1A3->places=0;_tmp1A3;});_tmp1A2.pile=_tmp357;});_tmp1A2.d=d;_tmp1A2.loc=loc;_tmp1A2.root_place=place;_tmp1A2.all_changed=all_changed;_tmp1A2;});
void*newval=({struct Cyc_List_List*_tmp359=_tmp1A4;void*_tmp358=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp1A5);Cyc_CfFlowInfo_assign_place_outer(& env,_tmp359,0,_tmp358,r);});
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp35C=fenv;struct Cyc_CfFlowInfo_EscPile*_tmp35B=env.pile;struct Cyc_Dict_Dict*_tmp35A=all_changed;Cyc_CfFlowInfo_escape_these(_tmp35C,_tmp35B,_tmp35A,((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp1A5,newval));});};};}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};
# 864
typedef struct Cyc_CfFlowInfo_JoinEnv*Cyc_CfFlowInfo_joinenv_t;
enum Cyc_CfFlowInfo_WhoIsChanged{Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict chg1;struct Cyc_Dict_Dict chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};
# 874
typedef struct Cyc_CfFlowInfo_AfterEnv*Cyc_CfFlowInfo_afterenv_t;
# 876
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2);struct _tuple21{enum Cyc_CfFlowInfo_InitLevel f1;enum Cyc_CfFlowInfo_InitLevel f2;};
# 886 "cf_flowinfo.cyc"
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){
if(r1 == r2)return r1;{
struct _RegionHandle*_tmp1A6=(env->fenv)->r;
# 890
{struct _tuple0 _tmp1A7=({struct _tuple0 _tmp1E2;_tmp1E2.f1=r1;_tmp1E2.f2=r2;_tmp1E2;});struct _tuple0 _tmp1A8=_tmp1A7;int _tmp1E1;int _tmp1E0;struct _dyneither_ptr _tmp1DF;int _tmp1DE;int _tmp1DD;struct _dyneither_ptr _tmp1DC;struct Cyc_CfFlowInfo_Place*_tmp1DB;struct Cyc_CfFlowInfo_Place*_tmp1DA;struct Cyc_CfFlowInfo_Place*_tmp1D9;struct Cyc_CfFlowInfo_Place*_tmp1D8;struct Cyc_CfFlowInfo_Place*_tmp1D7;struct Cyc_CfFlowInfo_Place*_tmp1D6;struct Cyc_CfFlowInfo_Place*_tmp1D5;struct Cyc_CfFlowInfo_Place*_tmp1D4;struct Cyc_Absyn_Exp*_tmp1D3;int _tmp1D2;void*_tmp1D1;struct Cyc_Absyn_Exp*_tmp1D0;int _tmp1CF;void*_tmp1CE;struct Cyc_Absyn_Exp*_tmp1CD;int _tmp1CC;void*_tmp1CB;struct Cyc_Absyn_Exp*_tmp1CA;int _tmp1C9;void*_tmp1C8;struct Cyc_Absyn_Vardecl*_tmp1C7;void*_tmp1C6;struct Cyc_Absyn_Vardecl*_tmp1C5;void*_tmp1C4;struct Cyc_Absyn_Vardecl*_tmp1C3;void*_tmp1C2;struct Cyc_Absyn_Vardecl*_tmp1C1;void*_tmp1C0;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f1)->tag == 8){if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f2)->tag == 8){_LLFB: _tmp1C3=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f1)->f1;_tmp1C2=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f1)->f2;_tmp1C1=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f2)->f1;_tmp1C0=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f2)->f2;_LLFC:
# 893
 if(_tmp1C3 == _tmp1C1)
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1A9=_region_malloc(_tmp1A6,sizeof(*_tmp1A9));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp35E=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp1AA;_tmp1AA.tag=8;_tmp1AA.f1=_tmp1C3;({void*_tmp35D=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1C2,_tmp1C0);_tmp1AA.f2=_tmp35D;});_tmp1AA;});_tmp1A9[0]=_tmp35E;});_tmp1A9;});else{
# 896
return Cyc_CfFlowInfo_join_absRval(env,a,_tmp1C2,_tmp1C0);}}else{_LLFD: _tmp1C5=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f1)->f1;_tmp1C4=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f1)->f2;_LLFE:
 return Cyc_CfFlowInfo_join_absRval(env,a,_tmp1C4,r2);}}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f2)->tag == 8){_LLFF: _tmp1C7=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f2)->f1;_tmp1C6=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f2)->f2;_LL100:
 return Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1C6);}else{if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f1)->tag == 7){if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f2)->tag == 7){_LL101: _tmp1CD=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f1)->f1;_tmp1CC=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f1)->f2;_tmp1CB=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f1)->f3;_tmp1CA=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f2)->f1;_tmp1C9=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f2)->f2;_tmp1C8=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f2)->f3;_LL102:
# 901
 if(_tmp1CD == _tmp1CA)
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1AB=_region_malloc(_tmp1A6,sizeof(*_tmp1AB));({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp360=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp1AC;_tmp1AC.tag=7;_tmp1AC.f1=_tmp1CD;_tmp1AC.f2=_tmp1CC > _tmp1C9?_tmp1CC: _tmp1C9;({void*_tmp35F=
Cyc_CfFlowInfo_join_absRval(env,a,_tmp1CB,_tmp1C8);_tmp1AC.f3=_tmp35F;});_tmp1AC;});_tmp1AB[0]=_tmp360;});_tmp1AB;});
{void*_tmp1AD=_tmp1CB;struct Cyc_CfFlowInfo_Place*_tmp1AE;if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AD)->tag == 5){_LL11E: _tmp1AE=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AD)->f1;_LL11F:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1AE);goto _LL11D;}else{_LL120: _LL121:
 goto _LL11D;}_LL11D:;}
# 908
{void*_tmp1AF=_tmp1C8;struct Cyc_CfFlowInfo_Place*_tmp1B0;if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AF)->tag == 5){_LL123: _tmp1B0=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AF)->f1;_LL124:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1B0);goto _LL122;}else{_LL125: _LL126:
 goto _LL122;}_LL122:;}
# 912
goto _LLFA;}else{_LL103: _tmp1D0=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f1)->f1;_tmp1CF=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f1)->f2;_tmp1CE=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f1)->f3;_LL104:
# 914
 return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1B1=_region_malloc(_tmp1A6,sizeof(*_tmp1B1));({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp362=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp1B2;_tmp1B2.tag=7;_tmp1B2.f1=_tmp1D0;_tmp1B2.f2=_tmp1CF;({void*_tmp361=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1CE,r2);_tmp1B2.f3=_tmp361;});_tmp1B2;});_tmp1B1[0]=_tmp362;});_tmp1B1;});}}else{if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f2)->tag == 7){_LL105: _tmp1D3=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f2)->f1;_tmp1D2=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f2)->f2;_tmp1D1=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f2)->f3;_LL106:
# 916
 return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1B3=_region_malloc(_tmp1A6,sizeof(*_tmp1B3));({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp364=({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp1B4;_tmp1B4.tag=7;_tmp1B4.f1=_tmp1D3;_tmp1B4.f2=_tmp1D2;({void*_tmp363=Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp1D1);_tmp1B4.f3=_tmp363;});_tmp1B4;});_tmp1B3[0]=_tmp364;});_tmp1B3;});}else{switch(*((int*)_tmp1A8.f1)){case 5: switch(*((int*)_tmp1A8.f2)){case 5: _LL107: _tmp1D5=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f1)->f1;_tmp1D4=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f2)->f1;_LL108:
# 919
 if(({Cyc_CfFlowInfo_place_cmp(_tmp1D5,_tmp1D4)== 0;}))return r1;
Cyc_CfFlowInfo_add_place(env->pile,_tmp1D5);
Cyc_CfFlowInfo_add_place(env->pile,_tmp1D4);
goto _LLFA;case 2: _LL109: _tmp1D6=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f1)->f1;_LL10A:
# 924
 _tmp1D9=_tmp1D6;goto _LL10C;case 1: _LL10D: _tmp1D7=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f1)->f1;_LL10E:
# 930
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1D7);{
enum Cyc_CfFlowInfo_InitLevel _tmp1B5=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel _tmp1B6=_tmp1B5;if(_tmp1B6 == Cyc_CfFlowInfo_AllIL){_LL128: _LL129:
 return(env->fenv)->notzeroall;}else{_LL12A: _LL12B:
 return(env->fenv)->notzerothis;}_LL127:;};default: _LL111: _tmp1D8=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f1)->f1;_LL112:
# 942
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1D8);goto _LLFA;}case 2: switch(*((int*)_tmp1A8.f2)){case 5: _LL10B: _tmp1D9=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f2)->f1;_LL10C:
# 926
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1D9);
return(env->fenv)->notzerothis;case 1: _LL117: _LL118:
# 947
 return(env->fenv)->notzerothis;default: goto _LL11B;}case 1: switch(*((int*)_tmp1A8.f2)){case 5: _LL10F: _tmp1DA=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f2)->f1;_LL110:
# 936
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1DA);{
enum Cyc_CfFlowInfo_InitLevel _tmp1B7=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);enum Cyc_CfFlowInfo_InitLevel _tmp1B8=_tmp1B7;if(_tmp1B8 == Cyc_CfFlowInfo_AllIL){_LL12D: _LL12E:
 return(env->fenv)->notzeroall;}else{_LL12F: _LL130:
 return(env->fenv)->notzerothis;}_LL12C:;};case 2: _LL115: _LL116:
# 946
 goto _LL118;default: goto _LL11B;}default: if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f2)->tag == 5){_LL113: _tmp1DB=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f2)->f1;_LL114:
# 943
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1DB);goto _LLFA;}else{if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f1)->tag == 6){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f2)->tag == 6){_LL119: _tmp1E1=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f1)->f1).is_union;_tmp1E0=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f1)->f1).fieldnum;_tmp1DF=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f1)->f2;_tmp1DE=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f2)->f1).is_union;_tmp1DD=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f2)->f1).fieldnum;_tmp1DC=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1A8.f2)->f2;_LL11A: {
# 950
struct _dyneither_ptr new_d=({unsigned int _tmp1BC=
_get_dyneither_size(_tmp1DF,sizeof(void*));void**_tmp1BD=(void**)({struct _RegionHandle*_tmp365=_tmp1A6;_region_malloc(_tmp365,_check_times(sizeof(void*),_tmp1BC));});struct _dyneither_ptr _tmp1BF=_tag_dyneither(_tmp1BD,sizeof(void*),_tmp1BC);{unsigned int _tmp1BE=_tmp1BC;unsigned int i;for(i=0;i < _tmp1BE;i ++){({typeof((void*)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(env,0,((void**)_tmp1DF.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp1DC,sizeof(void*),(int)i))))_tmp366=(void*)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(env,0,((void**)_tmp1DF.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp1DC,sizeof(void*),(int)i)));_tmp1BD[i]=_tmp366;});}}_tmp1BF;});
# 955
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp1DF,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp1DF.curr)[i]){
change=1;break;}}}
# 960
if(!change){
if(!_tmp1E1)return r1;
new_d=_tmp1DF;}else{
# 965
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp1DF,sizeof(void*));++ i){
if(({void*_tmp367=*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i));_tmp367 != *((void**)_check_dyneither_subscript(_tmp1DC,sizeof(void*),i));})){
change=1;break;}}}
# 970
if(!change){
if(!_tmp1E1)return r2;
new_d=_tmp1DC;}}
# 975
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp1B9=_region_malloc(_tmp1A6,sizeof(*_tmp1B9));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp369=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp1BA;_tmp1BA.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp368=({struct Cyc_CfFlowInfo_UnionRInfo _tmp1BB;_tmp1BB.is_union=_tmp1E1;_tmp1BB.fieldnum=_tmp1E0 == _tmp1DD?_tmp1E0: - 1;_tmp1BB;});_tmp1BA.f1=_tmp368;});_tmp1BA.f2=new_d;_tmp1BA;});_tmp1B9[0]=_tmp369;});_tmp1B9;});}}else{goto _LL11B;}}else{_LL11B: _LL11C:
# 977
 goto _LLFA;}}}}}}}_LLFA:;}{
# 979
enum Cyc_CfFlowInfo_InitLevel il1=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);
enum Cyc_CfFlowInfo_InitLevel il2=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);
struct _tuple0 _tmp1E3=({struct _tuple0 _tmp1EB;_tmp1EB.f1=r1;_tmp1EB.f2=r2;_tmp1EB;});struct _tuple0 _tmp1E4=_tmp1E3;if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1E4.f1)->tag == 4){_LL132: _LL133:
 goto _LL135;}else{if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1E4.f2)->tag == 4){_LL134: _LL135: {
# 984
struct _tuple21 _tmp1E5=({struct _tuple21 _tmp1E7;_tmp1E7.f1=il1;_tmp1E7.f2=il2;_tmp1E7;});struct _tuple21 _tmp1E6=_tmp1E5;if(_tmp1E6.f2 == Cyc_CfFlowInfo_NoneIL){_LL139: _LL13A:
 goto _LL13C;}else{if(_tmp1E6.f1 == Cyc_CfFlowInfo_NoneIL){_LL13B: _LL13C: return(env->fenv)->esc_none;}else{if(_tmp1E6.f2 == Cyc_CfFlowInfo_ThisIL){_LL13D: _LL13E:
 goto _LL140;}else{if(_tmp1E6.f1 == Cyc_CfFlowInfo_ThisIL){_LL13F: _LL140: return(env->fenv)->esc_this;}else{_LL141: _LL142:
 return(env->fenv)->esc_all;}}}}_LL138:;}}else{_LL136: _LL137: {
# 990
struct _tuple21 _tmp1E8=({struct _tuple21 _tmp1EA;_tmp1EA.f1=il1;_tmp1EA.f2=il2;_tmp1EA;});struct _tuple21 _tmp1E9=_tmp1E8;if(_tmp1E9.f2 == Cyc_CfFlowInfo_NoneIL){_LL144: _LL145:
 goto _LL147;}else{if(_tmp1E9.f1 == Cyc_CfFlowInfo_NoneIL){_LL146: _LL147: return(env->fenv)->unknown_none;}else{if(_tmp1E9.f2 == Cyc_CfFlowInfo_ThisIL){_LL148: _LL149:
 goto _LL14B;}else{if(_tmp1E9.f1 == Cyc_CfFlowInfo_ThisIL){_LL14A: _LL14B: return(env->fenv)->unknown_this;}else{_LL14C: _LL14D:
 return(env->fenv)->unknown_all;}}}}_LL143:;}}}_LL131:;};};}struct _tuple22{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
# 998
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 1001
struct _tuple22 _tmp1EC=({struct _tuple22 _tmp1F8;_tmp1F8.f1=f1;_tmp1F8.f2=f2;_tmp1F8;});struct _tuple22 _tmp1ED=_tmp1EC;struct Cyc_Dict_Dict _tmp1F7;struct Cyc_List_List*_tmp1F6;struct Cyc_Dict_Dict _tmp1F5;struct Cyc_List_List*_tmp1F4;if(((_tmp1ED.f1).BottomFL).tag == 1){_LL14F: _LL150:
 return f2;}else{if(((_tmp1ED.f2).BottomFL).tag == 1){_LL151: _LL152:
 return f1;}else{_LL153: _tmp1F7=(((_tmp1ED.f1).ReachableFL).val).f1;_tmp1F6=(((_tmp1ED.f1).ReachableFL).val).f2;_tmp1F5=(((_tmp1ED.f2).ReachableFL).val).f1;_tmp1F4=(((_tmp1ED.f2).ReachableFL).val).f2;_LL154:
# 1007
 if(_tmp1F7.t == _tmp1F5.t  && _tmp1F6 == _tmp1F4)return f1;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{
struct _RegionHandle*_tmp1EE=fenv->r;
struct Cyc_CfFlowInfo_JoinEnv _tmp1EF=({struct Cyc_CfFlowInfo_JoinEnv _tmp1F2;_tmp1F2.fenv=fenv;({struct Cyc_CfFlowInfo_EscPile*_tmp36A=({struct Cyc_CfFlowInfo_EscPile*_tmp1F3=_region_malloc(_tmp1EE,sizeof(*_tmp1F3));_tmp1F3->rgn=_tmp1EE;_tmp1F3->places=0;_tmp1F3;});_tmp1F2.pile=_tmp36A;});_tmp1F2.d1=_tmp1F7;_tmp1F2.d2=_tmp1F5;_tmp1F2;});
struct Cyc_Dict_Dict _tmp1F0=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,& _tmp1EF,_tmp1F7,_tmp1F5);
struct Cyc_List_List*_tmp1F1=Cyc_Relations_join_relns(fenv->r,_tmp1F6,_tmp1F4);
return({struct Cyc_Dict_Dict _tmp36B=Cyc_CfFlowInfo_escape_these(fenv,_tmp1EF.pile,all_changed,_tmp1F0);Cyc_CfFlowInfo_ReachableFL(_tmp36B,_tmp1F1);});};}}_LL14E:;}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo f3;};
# 1019
struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,struct _tuple13 pr2){
# 1026
struct _tuple13 _tmp1F9=pr1;union Cyc_CfFlowInfo_FlowInfo _tmp212;void*_tmp211;_LL156: _tmp212=_tmp1F9.f1;_tmp211=_tmp1F9.f2;_LL157:;{
struct _tuple13 _tmp1FA=pr2;union Cyc_CfFlowInfo_FlowInfo _tmp210;void*_tmp20F;_LL159: _tmp210=_tmp1FA.f1;_tmp20F=_tmp1FA.f2;_LL15A:;{
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,all_changed,_tmp212,_tmp210);
struct _tuple23 _tmp1FB=({struct _tuple23 _tmp20E;_tmp20E.f1=_tmp212;_tmp20E.f2=_tmp210;_tmp20E.f3=outflow;_tmp20E;});struct _tuple23 _tmp1FC=_tmp1FB;struct Cyc_Dict_Dict _tmp20D;struct Cyc_Dict_Dict _tmp20C;struct Cyc_Dict_Dict _tmp20B;struct Cyc_List_List*_tmp20A;if(((_tmp1FC.f1).BottomFL).tag == 1){_LL15C: _LL15D:
 return({struct _tuple13 _tmp1FD;_tmp1FD.f1=outflow;_tmp1FD.f2=_tmp20F;_tmp1FD;});}else{if(((_tmp1FC.f2).BottomFL).tag == 1){_LL15E: _LL15F:
 return({struct _tuple13 _tmp1FE;_tmp1FE.f1=outflow;_tmp1FE.f2=_tmp211;_tmp1FE;});}else{if(((_tmp1FC.f3).ReachableFL).tag == 2){_LL160: _tmp20D=(((_tmp1FC.f1).ReachableFL).val).f1;_tmp20C=(((_tmp1FC.f2).ReachableFL).val).f1;_tmp20B=(((_tmp1FC.f3).ReachableFL).val).f1;_tmp20A=(((_tmp1FC.f3).ReachableFL).val).f2;_LL161:
# 1034
 if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp211,_tmp20F))return({struct _tuple13 _tmp1FF;_tmp1FF.f1=outflow;_tmp1FF.f2=_tmp20F;_tmp1FF;});
if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp20F,_tmp211))return({struct _tuple13 _tmp200;_tmp200.f1=outflow;_tmp200.f2=_tmp211;_tmp200;});{
struct _RegionHandle*_tmp201=fenv->r;
struct Cyc_CfFlowInfo_JoinEnv _tmp202=({struct Cyc_CfFlowInfo_JoinEnv _tmp205;_tmp205.fenv=fenv;({struct Cyc_CfFlowInfo_EscPile*_tmp36C=({struct Cyc_CfFlowInfo_EscPile*_tmp206=_region_malloc(_tmp201,sizeof(*_tmp206));_tmp206->rgn=_tmp201;_tmp206->places=0;_tmp206;});_tmp205.pile=_tmp36C;});_tmp205.d1=_tmp20D;_tmp205.d2=_tmp20C;_tmp205;});
void*_tmp203=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp202,0,_tmp211,_tmp20F);
return({struct _tuple13 _tmp204;({union Cyc_CfFlowInfo_FlowInfo _tmp36E=({struct Cyc_Dict_Dict _tmp36D=Cyc_CfFlowInfo_escape_these(fenv,_tmp202.pile,all_changed,_tmp20B);Cyc_CfFlowInfo_ReachableFL(_tmp36D,_tmp20A);});_tmp204.f1=_tmp36E;});_tmp204.f2=_tmp203;_tmp204;});};}else{_LL162: _LL163:
# 1042
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp207=_cycalloc(sizeof(*_tmp207));({struct Cyc_Core_Impossible_exn_struct _tmp370=({struct Cyc_Core_Impossible_exn_struct _tmp208;_tmp208.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp36F=({const char*_tmp209="join_flow_and_rval: BottomFL outflow";_tag_dyneither(_tmp209,sizeof(char),37);});_tmp208.f1=_tmp36F;});_tmp208;});_tmp207[0]=_tmp370;});_tmp207;}));}}}_LL15B:;};};}
# 1046
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*,int,void*,void*);
static void*Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){
int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,env->curr_place);
int changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,env->curr_place);
if(changed1  && changed2)
return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);
# 1055
if(changed1){
if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))
return r1;
env->changed=Cyc_CfFlowInfo_One;}
# 1060
if(changed2){
if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg1))
return r2;
env->changed=Cyc_CfFlowInfo_Two;}{
# 1068
struct _tuple0 _tmp213=({struct _tuple0 _tmp22C;_tmp22C.f1=r1;_tmp22C.f2=r2;_tmp22C;});struct _tuple0 _tmp214=_tmp213;int _tmp22B;int _tmp22A;struct _dyneither_ptr _tmp229;int _tmp228;int _tmp227;struct _dyneither_ptr _tmp226;struct Cyc_Absyn_Vardecl*_tmp225;void*_tmp224;struct Cyc_Absyn_Vardecl*_tmp223;void*_tmp222;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp214.f1)->tag == 7){_LL165: _LL166:
# 1070
 goto _LL168;}else{if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp214.f2)->tag == 7){_LL167: _LL168:
 return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp214.f1)->tag == 8){if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp214.f2)->tag == 8){_LL169: _tmp225=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp214.f1)->f1;_tmp224=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp214.f1)->f2;_tmp223=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp214.f2)->f1;_tmp222=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp214.f2)->f2;_LL16A:
# 1073
 if(_tmp225 == _tmp223){
void*_tmp215=Cyc_CfFlowInfo_after_absRval(env,_tmp224,_tmp222);
if(_tmp215 == _tmp224)return r1;
if(_tmp215 == _tmp222)return r2;
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp216=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp216));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp371=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp217;_tmp217.tag=8;_tmp217.f1=_tmp225;_tmp217.f2=_tmp215;_tmp217;});_tmp216[0]=_tmp371;});_tmp216;});}else{
# 1079
return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);}}else{_LL16D: _LL16E:
# 1082
 return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);}}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp214.f2)->tag == 8){_LL16B: _LL16C:
# 1080
 goto _LL16E;}else{if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp214.f1)->tag == 6){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp214.f2)->tag == 6){_LL16F: _tmp22B=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp214.f1)->f1).is_union;_tmp22A=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp214.f1)->f1).fieldnum;_tmp229=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp214.f1)->f2;_tmp228=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp214.f2)->f1).is_union;_tmp227=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp214.f2)->f1).fieldnum;_tmp226=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp214.f2)->f2;_LL170: {
# 1085
struct _dyneither_ptr new_d=({unsigned int _tmp21B=
# 1087
_get_dyneither_size(_tmp229,sizeof(void*));void**_tmp21C=(void**)({struct _RegionHandle*_tmp372=((env->joinenv).fenv)->r;_region_malloc(_tmp372,_check_times(sizeof(void*),_tmp21B));});struct _dyneither_ptr _tmp21E=_tag_dyneither(_tmp21C,sizeof(void*),_tmp21B);{unsigned int _tmp21D=_tmp21B;unsigned int i;for(i=0;i < _tmp21D;i ++){({typeof((void*)Cyc_CfFlowInfo_after_absRval_d(env,(int)i,((void**)_tmp229.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp226,sizeof(void*),(int)i))))_tmp373=(void*)Cyc_CfFlowInfo_after_absRval_d(env,(int)i,((void**)_tmp229.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp226,sizeof(void*),(int)i)));_tmp21C[i]=_tmp373;});}}_tmp21E;});
# 1092
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp229,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp229.curr)[i]){
change=1;break;}}}
# 1097
if(!change)
return r1;
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp229,sizeof(void*));++ i){
if(({void*_tmp374=*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i));_tmp374 != *((void**)_check_dyneither_subscript(_tmp226,sizeof(void*),i));})){
change=1;break;}}}
# 1104
if(!change)
return r2;
# 1108
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp218=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp218));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp376=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp219;_tmp219.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp375=({struct Cyc_CfFlowInfo_UnionRInfo _tmp21A;_tmp21A.is_union=_tmp22B;_tmp21A.fieldnum=_tmp22A == _tmp227?_tmp22A: - 1;_tmp21A;});_tmp219.f1=_tmp375;});_tmp219.f2=new_d;_tmp219;});_tmp218[0]=_tmp376;});_tmp218;});}}else{goto _LL171;}}else{_LL171: _LL172:
# 1116
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp21F=_cycalloc(sizeof(*_tmp21F));({struct Cyc_Core_Impossible_exn_struct _tmp378=({struct Cyc_Core_Impossible_exn_struct _tmp220;_tmp220.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp377=({const char*_tmp221="after_absRval -- non-aggregates!";_tag_dyneither(_tmp221,sizeof(char),33);});_tmp220.f1=_tmp377;});_tmp220;});_tmp21F[0]=_tmp378;});_tmp21F;}));}}}}}_LL164:;};}
# 1119
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2){
# 1122
if(r1 == r2)return r1;{
struct Cyc_List_List**_tmp22D=env->last_field_cell;
enum Cyc_CfFlowInfo_WhoIsChanged _tmp22E=env->changed;
({struct Cyc_List_List*_tmp379=({struct Cyc_List_List*_tmp22F=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp22F));_tmp22F->hd=(void*)key;_tmp22F->tl=0;_tmp22F;});*env->last_field_cell=_tmp379;});
# 1128
env->last_field_cell=&((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{
void*_tmp230=Cyc_CfFlowInfo_after_absRval(env,r1,r2);
env->last_field_cell=_tmp22D;
((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=0;
env->changed=_tmp22E;
return _tmp230;};};}
# 1135
static void*Cyc_CfFlowInfo_after_root(struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){
# 1137
if(r1 == r2)return r1;
({struct Cyc_CfFlowInfo_Place _tmp37A=({struct Cyc_CfFlowInfo_Place _tmp231;_tmp231.root=root;_tmp231.fields=0;_tmp231;});*env->curr_place=_tmp37A;});
env->last_field_cell=&(env->curr_place)->fields;
env->changed=Cyc_CfFlowInfo_Neither;
return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}
# 1144
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2){
# 1150
struct _tuple22 _tmp232=({struct _tuple22 _tmp23F;_tmp23F.f1=f1;_tmp23F.f2=f2;_tmp23F;});struct _tuple22 _tmp233=_tmp232;struct Cyc_Dict_Dict _tmp23E;struct Cyc_List_List*_tmp23D;struct Cyc_Dict_Dict _tmp23C;struct Cyc_List_List*_tmp23B;if(((_tmp233.f1).BottomFL).tag == 1){_LL174: _LL175:
 goto _LL177;}else{if(((_tmp233.f2).BottomFL).tag == 1){_LL176: _LL177:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL178: _tmp23E=(((_tmp233.f1).ReachableFL).val).f1;_tmp23D=(((_tmp233.f1).ReachableFL).val).f2;_tmp23C=(((_tmp233.f2).ReachableFL).val).f1;_tmp23B=(((_tmp233.f2).ReachableFL).val).f2;_LL179:
# 1154
 if(_tmp23E.t == _tmp23C.t  && _tmp23D == _tmp23B)return f1;{
# 1157
struct _RegionHandle*_tmp234=fenv->r;
struct Cyc_CfFlowInfo_Place*_tmp235=fenv->dummy_place;
struct Cyc_CfFlowInfo_AfterEnv _tmp236=({struct Cyc_CfFlowInfo_AfterEnv _tmp239;({struct Cyc_CfFlowInfo_JoinEnv _tmp37C=({(_tmp239.joinenv).fenv=fenv;({struct Cyc_CfFlowInfo_EscPile*_tmp37B=({struct Cyc_CfFlowInfo_EscPile*_tmp23A=_region_malloc(_tmp234,sizeof(*_tmp23A));_tmp23A->rgn=_tmp234;_tmp23A->places=0;_tmp23A;});(_tmp239.joinenv).pile=_tmp37B;});(_tmp239.joinenv).d1=_tmp23E;(_tmp239.joinenv).d2=_tmp23C;_tmp239.joinenv;});_tmp239.joinenv=_tmp37C;});_tmp239.chg1=chg1;_tmp239.chg2=chg2;_tmp239.curr_place=_tmp235;_tmp239.last_field_cell=& _tmp235->fields;_tmp239.changed=Cyc_CfFlowInfo_Neither;_tmp239;});
# 1163
struct Cyc_Dict_Dict _tmp237=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,& _tmp236,_tmp23E,_tmp23C);
struct Cyc_List_List*_tmp238=Cyc_Relations_join_relns(fenv->r,_tmp23D,_tmp23B);
return({struct Cyc_Dict_Dict _tmp37D=Cyc_CfFlowInfo_escape_these(fenv,(_tmp236.joinenv).pile,all_changed,_tmp237);Cyc_CfFlowInfo_ReachableFL(_tmp37D,_tmp238);});};}}_LL173:;}
# 1172
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2){
if(r1 == r2)return 1;
# 1176
{struct _tuple0 _tmp240=({struct _tuple0 _tmp24E;_tmp24E.f1=r1;_tmp24E.f2=r2;_tmp24E;});struct _tuple0 _tmp241=_tmp240;int _tmp24D;int _tmp24C;struct _dyneither_ptr _tmp24B;int _tmp24A;int _tmp249;struct _dyneither_ptr _tmp248;struct Cyc_CfFlowInfo_Place*_tmp247;struct Cyc_CfFlowInfo_Place*_tmp246;struct Cyc_Absyn_Vardecl*_tmp245;void*_tmp244;struct Cyc_Absyn_Vardecl*_tmp243;void*_tmp242;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp241.f1)->tag == 8){if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp241.f2)->tag == 8){_LL17B: _tmp245=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp241.f1)->f1;_tmp244=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp241.f1)->f2;_tmp243=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp241.f2)->f1;_tmp242=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp241.f2)->f2;_LL17C:
# 1178
 return _tmp245 == _tmp243  && Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,_tmp244,_tmp242);}else{_LL17D: _LL17E:
 goto _LL180;}}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp241.f2)->tag == 8){_LL17F: _LL180:
 return 0;}else{if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp241.f1)->tag == 5){if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp241.f2)->tag == 5){_LL181: _tmp247=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp241.f1)->f1;_tmp246=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp241.f2)->f1;_LL182:
 return({Cyc_CfFlowInfo_place_cmp(_tmp247,_tmp246)== 0;});}else{_LL183: _LL184:
 goto _LL186;}}else{if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp241.f2)->tag == 5){_LL185: _LL186:
 return 0;}else{switch(*((int*)_tmp241.f1)){case 6: switch(*((int*)_tmp241.f2)){case 6: _LL187: _tmp24D=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp241.f1)->f1).is_union;_tmp24C=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp241.f1)->f1).fieldnum;_tmp24B=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp241.f1)->f2;_tmp24A=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp241.f2)->f1).is_union;_tmp249=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp241.f2)->f1).fieldnum;_tmp248=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp241.f2)->f2;_LL188:
# 1186
 if(_tmp24D  && _tmp24C != _tmp249)return 0;
if((void**)_tmp24B.curr == (void**)_tmp248.curr)return 1;
{int i=0;for(0;i < _get_dyneither_size(_tmp24B,sizeof(void*));++ i){
if(!((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp24B.curr)[i],*((void**)_check_dyneither_subscript(_tmp248,sizeof(void*),i))))return 0;}}
return 1;case 2: goto _LL18B;case 0: goto _LL18D;case 1: goto _LL18F;default: goto _LL195;}case 1: switch(*((int*)_tmp241.f2)){case 2: _LL189: _LL18A:
 return 1;case 0: goto _LL18D;case 1: goto _LL18F;default: goto _LL195;}default: switch(*((int*)_tmp241.f2)){case 2: _LL18B: _LL18C:
 return 0;case 0: _LL18D: _LL18E:
 goto _LL190;case 1: _LL18F: _LL190:
 return 0;default: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp241.f1)->tag == 4){if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp241.f2)->tag == 4){_LL191: _LL192:
 goto _LL17A;}else{_LL193: _LL194:
 return 0;}}else{_LL195: _LL196:
 goto _LL17A;}}}}}}}_LL17A:;}{
# 1199
struct _tuple21 _tmp24F=({struct _tuple21 _tmp251;({enum Cyc_CfFlowInfo_InitLevel _tmp37E=Cyc_CfFlowInfo_initlevel_approx(r1);_tmp251.f1=_tmp37E;});({enum Cyc_CfFlowInfo_InitLevel _tmp37F=Cyc_CfFlowInfo_initlevel_approx(r2);_tmp251.f2=_tmp37F;});_tmp251;});struct _tuple21 _tmp250=_tmp24F;if(_tmp250.f1 == Cyc_CfFlowInfo_AllIL)switch(_tmp250.f2){case Cyc_CfFlowInfo_AllIL: _LL198: _LL199:
 return 1;case Cyc_CfFlowInfo_NoneIL: goto _LL19A;default: goto _LL19E;}else{if(_tmp250.f2 == Cyc_CfFlowInfo_NoneIL){_LL19A: _LL19B:
 return 1;}else{if(_tmp250.f1 == Cyc_CfFlowInfo_NoneIL){_LL19C: _LL19D:
 return 0;}else{if(_tmp250.f2 == Cyc_CfFlowInfo_ThisIL){_LL19E: _LL19F:
 return 1;}else{_LL1A0: _LL1A1:
 return 0;}}}}_LL197:;};}
# 1214
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 1216
struct _tuple22 _tmp252=({struct _tuple22 _tmp258;_tmp258.f1=f1;_tmp258.f2=f2;_tmp258;});struct _tuple22 _tmp253=_tmp252;struct Cyc_Dict_Dict _tmp257;struct Cyc_List_List*_tmp256;struct Cyc_Dict_Dict _tmp255;struct Cyc_List_List*_tmp254;if(((_tmp253.f1).BottomFL).tag == 1){_LL1A3: _LL1A4:
 return 1;}else{if(((_tmp253.f2).BottomFL).tag == 1){_LL1A5: _LL1A6:
 return 0;}else{_LL1A7: _tmp257=(((_tmp253.f1).ReachableFL).val).f1;_tmp256=(((_tmp253.f1).ReachableFL).val).f2;_tmp255=(((_tmp253.f2).ReachableFL).val).f1;_tmp254=(((_tmp253.f2).ReachableFL).val).f2;_LL1A8:
# 1220
 if(_tmp257.t == _tmp255.t  && _tmp256 == _tmp254)return 1;
return((int(*)(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_forall_intersect)((int(*)(void*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,_tmp257,_tmp255) && 
Cyc_Relations_relns_approx(_tmp256,_tmp254);}}_LL1A2:;}
# 1226
struct _tuple12 Cyc_CfFlowInfo_unname_rval(struct _RegionHandle*rgn,void*rv){
# 1228
struct Cyc_List_List*names=0;
int done=0;
while(!done){
void*_tmp259=rv;struct Cyc_Absyn_Vardecl*_tmp25C;void*_tmp25B;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp259)->tag == 8){_LL1AA: _tmp25C=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp259)->f1;_tmp25B=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp259)->f2;_LL1AB:
# 1233
({struct Cyc_List_List*_tmp380=({struct Cyc_List_List*_tmp25A=_region_malloc(rgn,sizeof(*_tmp25A));_tmp25A->hd=_tmp25C;_tmp25A->tl=names;_tmp25A;});names=_tmp380;});rv=_tmp25B;goto _LL1A9;}else{_LL1AC: _LL1AD:
# 1235
 done=1;goto _LL1A9;}_LL1A9:;}
# 1238
return({struct _tuple12 _tmp25D;_tmp25D.f1=rv;_tmp25D.f2=names;_tmp25D;});}
# 1241
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il){
enum Cyc_CfFlowInfo_InitLevel _tmp25E=il;switch(_tmp25E){case Cyc_CfFlowInfo_NoneIL: _LL1AF: _LL1B0:
({void*_tmp25F=0;({struct Cyc___cycFILE*_tmp382=Cyc_stderr;struct _dyneither_ptr _tmp381=({const char*_tmp260="uninitialized";_tag_dyneither(_tmp260,sizeof(char),14);});Cyc_fprintf(_tmp382,_tmp381,_tag_dyneither(_tmp25F,sizeof(void*),0));});});goto _LL1AE;case Cyc_CfFlowInfo_ThisIL: _LL1B1: _LL1B2:
({void*_tmp261=0;({struct Cyc___cycFILE*_tmp384=Cyc_stderr;struct _dyneither_ptr _tmp383=({const char*_tmp262="this-initialized";_tag_dyneither(_tmp262,sizeof(char),17);});Cyc_fprintf(_tmp384,_tmp383,_tag_dyneither(_tmp261,sizeof(void*),0));});});goto _LL1AE;default: _LL1B3: _LL1B4:
({void*_tmp263=0;({struct Cyc___cycFILE*_tmp386=Cyc_stderr;struct _dyneither_ptr _tmp385=({const char*_tmp264="all-initialized";_tag_dyneither(_tmp264,sizeof(char),16);});Cyc_fprintf(_tmp386,_tmp385,_tag_dyneither(_tmp263,sizeof(void*),0));});});goto _LL1AE;}_LL1AE:;}
# 1249
void Cyc_CfFlowInfo_print_root(void*root){
void*_tmp265=root;struct Cyc_Absyn_Exp*_tmp271;void*_tmp270;struct Cyc_Absyn_Vardecl*_tmp26F;switch(*((int*)_tmp265)){case 0: _LL1B6: _tmp26F=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp265)->f1;_LL1B7:
# 1252
({struct Cyc_String_pa_PrintArg_struct _tmp268;_tmp268.tag=0;({struct _dyneither_ptr _tmp387=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp26F->name));_tmp268.f1=_tmp387;});({void*_tmp266[1]={& _tmp268};({struct Cyc___cycFILE*_tmp389=Cyc_stderr;struct _dyneither_ptr _tmp388=({const char*_tmp267="Root(%s)";_tag_dyneither(_tmp267,sizeof(char),9);});Cyc_fprintf(_tmp389,_tmp388,_tag_dyneither(_tmp266,sizeof(void*),1));});});});goto _LL1B5;case 1: _LL1B8: _tmp271=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp265)->f1;_tmp270=(void*)((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp265)->f2;_LL1B9:
# 1254
({struct Cyc_String_pa_PrintArg_struct _tmp26C;_tmp26C.tag=0;({struct _dyneither_ptr _tmp38A=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp270));_tmp26C.f1=_tmp38A;});({struct Cyc_String_pa_PrintArg_struct _tmp26B;_tmp26B.tag=0;({struct _dyneither_ptr _tmp38B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp271));_tmp26B.f1=_tmp38B;});({void*_tmp269[2]={& _tmp26B,& _tmp26C};({struct Cyc___cycFILE*_tmp38D=Cyc_stderr;struct _dyneither_ptr _tmp38C=({const char*_tmp26A="MallocPt(%s,%s)";_tag_dyneither(_tmp26A,sizeof(char),16);});Cyc_fprintf(_tmp38D,_tmp38C,_tag_dyneither(_tmp269,sizeof(void*),2));});});});});goto _LL1B5;default: _LL1BA: _LL1BB:
# 1257
({void*_tmp26D=0;({struct Cyc___cycFILE*_tmp38F=Cyc_stderr;struct _dyneither_ptr _tmp38E=({const char*_tmp26E="InitParam(_,_)";_tag_dyneither(_tmp26E,sizeof(char),15);});Cyc_fprintf(_tmp38F,_tmp38E,_tag_dyneither(_tmp26D,sizeof(void*),0));});});goto _LL1B5;}_LL1B5:;}
# 1261
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){
Cyc_CfFlowInfo_print_root(p->root);{
struct Cyc_List_List*_tmp272=p->fields;
while(_tmp272 != 0){
({struct Cyc_Int_pa_PrintArg_struct _tmp275;_tmp275.tag=1;_tmp275.f1=(unsigned long)((int)_tmp272->hd);({void*_tmp273[1]={& _tmp275};({struct Cyc___cycFILE*_tmp391=Cyc_stderr;struct _dyneither_ptr _tmp390=({const char*_tmp274=".%d";_tag_dyneither(_tmp274,sizeof(char),4);});Cyc_fprintf(_tmp391,_tmp390,_tag_dyneither(_tmp273,sizeof(void*),1));});});});
_tmp272=_tmp272->tl;}
# 1268
({void*_tmp276=0;({struct Cyc___cycFILE*_tmp393=Cyc_stderr;struct _dyneither_ptr _tmp392=({const char*_tmp277=" ";_tag_dyneither(_tmp277,sizeof(char),2);});Cyc_fprintf(_tmp393,_tmp392,_tag_dyneither(_tmp276,sizeof(void*),0));});});};}
# 1271
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){
int _tmp278=1;
while(x != 0){
if(!_tmp278){({void*_tmp279=0;({struct Cyc___cycFILE*_tmp395=Cyc_stderr;struct _dyneither_ptr _tmp394=({const char*_tmp27A=", ";_tag_dyneither(_tmp27A,sizeof(char),3);});Cyc_fprintf(_tmp395,_tmp394,_tag_dyneither(_tmp279,sizeof(void*),0));});});_tmp278=0;}
pr(x->hd);
x=x->tl;}
# 1278
({void*_tmp27B=0;({struct Cyc___cycFILE*_tmp397=Cyc_stderr;struct _dyneither_ptr _tmp396=({const char*_tmp27C="\n";_tag_dyneither(_tmp27C,sizeof(char),2);});Cyc_fprintf(_tmp397,_tmp396,_tag_dyneither(_tmp27B,sizeof(void*),0));});});}
# 1281
void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*)){
struct _RegionHandle _tmp27D=_new_region("r");struct _RegionHandle*r=& _tmp27D;_push_region(r);
{struct _handler_cons _tmp27E;_push_handler(& _tmp27E);{int _tmp280=0;if(setjmp(_tmp27E.handler))_tmp280=1;if(!_tmp280){
{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);
struct _tuple14 _tmp281=*((struct _tuple14*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(r,p);
int first=1;
({void*_tmp282=0;({struct Cyc___cycFILE*_tmp399=Cyc_stderr;struct _dyneither_ptr _tmp398=({const char*_tmp283="{ ";_tag_dyneither(_tmp283,sizeof(char),3);});Cyc_fprintf(_tmp399,_tmp398,_tag_dyneither(_tmp282,sizeof(void*),0));});});
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple14*))Cyc_Iter_next)(iter,& _tmp281)){
void*_tmp284=_tmp281.f1;
if(!first){
({void*_tmp285=0;({struct Cyc___cycFILE*_tmp39B=Cyc_stderr;struct _dyneither_ptr _tmp39A=({const char*_tmp286=", ";_tag_dyneither(_tmp286,sizeof(char),3);});Cyc_fprintf(_tmp39B,_tmp39A,_tag_dyneither(_tmp285,sizeof(void*),0));});});
first=0;}
# 1294
pr(_tmp284);}
# 1296
({void*_tmp287=0;({struct Cyc___cycFILE*_tmp39D=Cyc_stderr;struct _dyneither_ptr _tmp39C=({const char*_tmp288="}\n";_tag_dyneither(_tmp288,sizeof(char),3);});Cyc_fprintf(_tmp39D,_tmp39C,_tag_dyneither(_tmp287,sizeof(void*),0));});});}
# 1284
;_pop_handler();}else{void*_tmp27F=(void*)_exn_thrown;void*_tmp289=_tmp27F;void*_tmp28C;_LL1BD: _tmp28C=_tmp289;_LL1BE:
# 1299
({void*_tmp28A=0;({struct Cyc___cycFILE*_tmp39F=Cyc_stderr;struct _dyneither_ptr _tmp39E=({const char*_tmp28B="{ }\n";_tag_dyneither(_tmp28B,sizeof(char),5);});Cyc_fprintf(_tmp39F,_tmp39E,_tag_dyneither(_tmp28A,sizeof(void*),0));});});
_npop_handler(0);return;_LL1BC:;}};}
# 1283
;_pop_region(r);}
# 1305
void Cyc_CfFlowInfo_print_absrval(void*rval){
void*_tmp28D=rval;struct Cyc_Absyn_Vardecl*_tmp2C0;void*_tmp2BF;struct Cyc_Absyn_Exp*_tmp2BE;int _tmp2BD;void*_tmp2BC;int _tmp2BB;int _tmp2BA;struct _dyneither_ptr _tmp2B9;struct Cyc_CfFlowInfo_Place*_tmp2B8;enum Cyc_CfFlowInfo_InitLevel _tmp2B7;enum Cyc_CfFlowInfo_InitLevel _tmp2B6;switch(*((int*)_tmp28D)){case 0: _LL1C2: _LL1C3:
({void*_tmp28E=0;({struct Cyc___cycFILE*_tmp3A1=Cyc_stderr;struct _dyneither_ptr _tmp3A0=({const char*_tmp28F="Zero";_tag_dyneither(_tmp28F,sizeof(char),5);});Cyc_fprintf(_tmp3A1,_tmp3A0,_tag_dyneither(_tmp28E,sizeof(void*),0));});});goto _LL1C1;case 1: _LL1C4: _LL1C5:
({void*_tmp290=0;({struct Cyc___cycFILE*_tmp3A3=Cyc_stderr;struct _dyneither_ptr _tmp3A2=({const char*_tmp291="NotZeroAll";_tag_dyneither(_tmp291,sizeof(char),11);});Cyc_fprintf(_tmp3A3,_tmp3A2,_tag_dyneither(_tmp290,sizeof(void*),0));});});goto _LL1C1;case 2: _LL1C6: _LL1C7:
({void*_tmp292=0;({struct Cyc___cycFILE*_tmp3A5=Cyc_stderr;struct _dyneither_ptr _tmp3A4=({const char*_tmp293="NotZeroThis";_tag_dyneither(_tmp293,sizeof(char),12);});Cyc_fprintf(_tmp3A5,_tmp3A4,_tag_dyneither(_tmp292,sizeof(void*),0));});});goto _LL1C1;case 3: _LL1C8: _tmp2B6=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp28D)->f1;_LL1C9:
({void*_tmp294=0;({struct Cyc___cycFILE*_tmp3A7=Cyc_stderr;struct _dyneither_ptr _tmp3A6=({const char*_tmp295="Unknown(";_tag_dyneither(_tmp295,sizeof(char),9);});Cyc_fprintf(_tmp3A7,_tmp3A6,_tag_dyneither(_tmp294,sizeof(void*),0));});});Cyc_CfFlowInfo_print_initlevel(_tmp2B6);
({void*_tmp296=0;({struct Cyc___cycFILE*_tmp3A9=Cyc_stderr;struct _dyneither_ptr _tmp3A8=({const char*_tmp297=")";_tag_dyneither(_tmp297,sizeof(char),2);});Cyc_fprintf(_tmp3A9,_tmp3A8,_tag_dyneither(_tmp296,sizeof(void*),0));});});goto _LL1C1;case 4: _LL1CA: _tmp2B7=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp28D)->f1;_LL1CB:
({void*_tmp298=0;({struct Cyc___cycFILE*_tmp3AB=Cyc_stderr;struct _dyneither_ptr _tmp3AA=({const char*_tmp299="Esc(";_tag_dyneither(_tmp299,sizeof(char),5);});Cyc_fprintf(_tmp3AB,_tmp3AA,_tag_dyneither(_tmp298,sizeof(void*),0));});});Cyc_CfFlowInfo_print_initlevel(_tmp2B7);
({void*_tmp29A=0;({struct Cyc___cycFILE*_tmp3AD=Cyc_stderr;struct _dyneither_ptr _tmp3AC=({const char*_tmp29B=")";_tag_dyneither(_tmp29B,sizeof(char),2);});Cyc_fprintf(_tmp3AD,_tmp3AC,_tag_dyneither(_tmp29A,sizeof(void*),0));});});goto _LL1C1;case 5: _LL1CC: _tmp2B8=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp28D)->f1;_LL1CD:
({void*_tmp29C=0;({struct Cyc___cycFILE*_tmp3AF=Cyc_stderr;struct _dyneither_ptr _tmp3AE=({const char*_tmp29D="AddrOf(";_tag_dyneither(_tmp29D,sizeof(char),8);});Cyc_fprintf(_tmp3AF,_tmp3AE,_tag_dyneither(_tmp29C,sizeof(void*),0));});});Cyc_CfFlowInfo_print_place(_tmp2B8);
({void*_tmp29E=0;({struct Cyc___cycFILE*_tmp3B1=Cyc_stderr;struct _dyneither_ptr _tmp3B0=({const char*_tmp29F=")";_tag_dyneither(_tmp29F,sizeof(char),2);});Cyc_fprintf(_tmp3B1,_tmp3B0,_tag_dyneither(_tmp29E,sizeof(void*),0));});});goto _LL1C1;case 6: _LL1CE: _tmp2BB=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp28D)->f1).is_union;_tmp2BA=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp28D)->f1).fieldnum;_tmp2B9=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp28D)->f2;_LL1CF:
# 1317
 if(_tmp2BB){
({void*_tmp2A0=0;({struct Cyc___cycFILE*_tmp3B3=Cyc_stderr;struct _dyneither_ptr _tmp3B2=({const char*_tmp2A1="AggrUnion{";_tag_dyneither(_tmp2A1,sizeof(char),11);});Cyc_fprintf(_tmp3B3,_tmp3B2,_tag_dyneither(_tmp2A0,sizeof(void*),0));});});
if(_tmp2BA != - 1)
({struct Cyc_Int_pa_PrintArg_struct _tmp2A4;_tmp2A4.tag=1;_tmp2A4.f1=(unsigned long)_tmp2BA;({void*_tmp2A2[1]={& _tmp2A4};({struct Cyc___cycFILE*_tmp3B5=Cyc_stderr;struct _dyneither_ptr _tmp3B4=({const char*_tmp2A3="tag == %d;";_tag_dyneither(_tmp2A3,sizeof(char),11);});Cyc_fprintf(_tmp3B5,_tmp3B4,_tag_dyneither(_tmp2A2,sizeof(void*),1));});});});}else{
# 1322
({void*_tmp2A5=0;({struct Cyc___cycFILE*_tmp3B7=Cyc_stderr;struct _dyneither_ptr _tmp3B6=({const char*_tmp2A6="AggrStruct{";_tag_dyneither(_tmp2A6,sizeof(char),12);});Cyc_fprintf(_tmp3B7,_tmp3B6,_tag_dyneither(_tmp2A5,sizeof(void*),0));});});}
{int i=0;for(0;i < _get_dyneither_size(_tmp2B9,sizeof(void*));++ i){
Cyc_CfFlowInfo_print_absrval(((void**)_tmp2B9.curr)[i]);
if(i + 1 < _get_dyneither_size(_tmp2B9,sizeof(void*)))({void*_tmp2A7=0;({struct Cyc___cycFILE*_tmp3B9=Cyc_stderr;struct _dyneither_ptr _tmp3B8=({const char*_tmp2A8=",";_tag_dyneither(_tmp2A8,sizeof(char),2);});Cyc_fprintf(_tmp3B9,_tmp3B8,_tag_dyneither(_tmp2A7,sizeof(void*),0));});});}}
# 1327
({void*_tmp2A9=0;({struct Cyc___cycFILE*_tmp3BB=Cyc_stderr;struct _dyneither_ptr _tmp3BA=({const char*_tmp2AA="}";_tag_dyneither(_tmp2AA,sizeof(char),2);});Cyc_fprintf(_tmp3BB,_tmp3BA,_tag_dyneither(_tmp2A9,sizeof(void*),0));});});
goto _LL1C1;case 7: _LL1D0: _tmp2BE=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp28D)->f1;_tmp2BD=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp28D)->f2;_tmp2BC=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp28D)->f3;_LL1D1:
# 1330
({struct Cyc_Int_pa_PrintArg_struct _tmp2AE;_tmp2AE.tag=1;_tmp2AE.f1=(unsigned long)_tmp2BD;({struct Cyc_String_pa_PrintArg_struct _tmp2AD;_tmp2AD.tag=0;({struct _dyneither_ptr _tmp3BC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp2BE));_tmp2AD.f1=_tmp3BC;});({void*_tmp2AB[2]={& _tmp2AD,& _tmp2AE};({struct Cyc___cycFILE*_tmp3BE=Cyc_stderr;struct _dyneither_ptr _tmp3BD=({const char*_tmp2AC="[Consumed(%s,%d,";_tag_dyneither(_tmp2AC,sizeof(char),17);});Cyc_fprintf(_tmp3BE,_tmp3BD,_tag_dyneither(_tmp2AB,sizeof(void*),2));});});});});
Cyc_CfFlowInfo_print_absrval(_tmp2BC);({void*_tmp2AF=0;({struct Cyc___cycFILE*_tmp3C0=Cyc_stderr;struct _dyneither_ptr _tmp3BF=({const char*_tmp2B0=")]";_tag_dyneither(_tmp2B0,sizeof(char),3);});Cyc_fprintf(_tmp3C0,_tmp3BF,_tag_dyneither(_tmp2AF,sizeof(void*),0));});});
goto _LL1C1;default: _LL1D2: _tmp2C0=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp28D)->f1;_tmp2BF=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp28D)->f2;_LL1D3:
# 1334
({struct Cyc_String_pa_PrintArg_struct _tmp2B3;_tmp2B3.tag=0;({struct _dyneither_ptr _tmp3C1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2C0->name));_tmp2B3.f1=_tmp3C1;});({void*_tmp2B1[1]={& _tmp2B3};({struct Cyc___cycFILE*_tmp3C3=Cyc_stderr;struct _dyneither_ptr _tmp3C2=({const char*_tmp2B2="[NamedLocation(%s,";_tag_dyneither(_tmp2B2,sizeof(char),19);});Cyc_fprintf(_tmp3C3,_tmp3C2,_tag_dyneither(_tmp2B1,sizeof(void*),1));});});});
Cyc_CfFlowInfo_print_absrval(_tmp2BF);({void*_tmp2B4=0;({struct Cyc___cycFILE*_tmp3C5=Cyc_stderr;struct _dyneither_ptr _tmp3C4=({const char*_tmp2B5=")]";_tag_dyneither(_tmp2B5,sizeof(char),3);});Cyc_fprintf(_tmp3C5,_tmp3C4,_tag_dyneither(_tmp2B4,sizeof(void*),0));});});
goto _LL1C1;}_LL1C1:;}
# 1340
static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){
({void*_tmp2C1=0;({struct Cyc___cycFILE*_tmp3C7=Cyc_stderr;struct _dyneither_ptr _tmp3C6=({const char*_tmp2C2="    ";_tag_dyneither(_tmp2C2,sizeof(char),5);});Cyc_fprintf(_tmp3C7,_tmp3C6,_tag_dyneither(_tmp2C1,sizeof(void*),0));});});
Cyc_CfFlowInfo_print_root(root);
({void*_tmp2C3=0;({struct Cyc___cycFILE*_tmp3C9=Cyc_stderr;struct _dyneither_ptr _tmp3C8=({const char*_tmp2C4=" --> ";_tag_dyneither(_tmp2C4,sizeof(char),6);});Cyc_fprintf(_tmp3C9,_tmp3C8,_tag_dyneither(_tmp2C3,sizeof(void*),0));});});
Cyc_CfFlowInfo_print_absrval(rval);
({void*_tmp2C5=0;({struct Cyc___cycFILE*_tmp3CB=Cyc_stderr;struct _dyneither_ptr _tmp3CA=({const char*_tmp2C6="\n";_tag_dyneither(_tmp2C6,sizeof(char),2);});Cyc_fprintf(_tmp3CB,_tmp3CA,_tag_dyneither(_tmp2C5,sizeof(void*),0));});});}
# 1348
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){
((void(*)(void(*f)(void*,void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,d);}
# 1352
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){
union Cyc_CfFlowInfo_FlowInfo _tmp2C7=f;struct Cyc_Dict_Dict _tmp2D1;struct Cyc_List_List*_tmp2D0;if((_tmp2C7.BottomFL).tag == 1){_LL1D5: _LL1D6:
({void*_tmp2C8=0;({struct Cyc___cycFILE*_tmp3CD=Cyc_stderr;struct _dyneither_ptr _tmp3CC=({const char*_tmp2C9="  BottomFL\n";_tag_dyneither(_tmp2C9,sizeof(char),12);});Cyc_fprintf(_tmp3CD,_tmp3CC,_tag_dyneither(_tmp2C8,sizeof(void*),0));});});goto _LL1D4;}else{_LL1D7: _tmp2D1=((_tmp2C7.ReachableFL).val).f1;_tmp2D0=((_tmp2C7.ReachableFL).val).f2;_LL1D8:
# 1356
({void*_tmp2CA=0;({struct Cyc___cycFILE*_tmp3CF=Cyc_stderr;struct _dyneither_ptr _tmp3CE=({const char*_tmp2CB="  ReachableFL:\n";_tag_dyneither(_tmp2CB,sizeof(char),16);});Cyc_fprintf(_tmp3CF,_tmp3CE,_tag_dyneither(_tmp2CA,sizeof(void*),0));});});
Cyc_CfFlowInfo_print_flowdict(_tmp2D1);
({void*_tmp2CC=0;({struct Cyc___cycFILE*_tmp3D1=Cyc_stderr;struct _dyneither_ptr _tmp3D0=({const char*_tmp2CD="\n  Relations: ";_tag_dyneither(_tmp2CD,sizeof(char),15);});Cyc_fprintf(_tmp3D1,_tmp3D0,_tag_dyneither(_tmp2CC,sizeof(void*),0));});});
Cyc_Relations_print_relns(Cyc_stderr,_tmp2D0);
({void*_tmp2CE=0;({struct Cyc___cycFILE*_tmp3D3=Cyc_stderr;struct _dyneither_ptr _tmp3D2=({const char*_tmp2CF="\n";_tag_dyneither(_tmp2CF,sizeof(char),2);});Cyc_fprintf(_tmp3D3,_tmp3D2,_tag_dyneither(_tmp2CE,sizeof(void*),0));});});
goto _LL1D4;}_LL1D4:;}
