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

# 69 "core.h"
 void exit(int);struct Cyc___cycFILE;
# 51 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stdout;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 79
int Cyc_fclose(struct Cyc___cycFILE*);
# 84
int Cyc_feof(struct Cyc___cycFILE*);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 98
struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
# 108
unsigned long Cyc_fread(struct _dyneither_ptr,unsigned long,unsigned long,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 140 "cycboot.h"
unsigned long Cyc_fwrite(struct _dyneither_ptr,unsigned long,unsigned long,struct Cyc___cycFILE*);
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);
# 167
int remove(const char*);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 271 "cycboot.h"
struct Cyc___cycFILE*Cyc_file_open(struct _dyneither_ptr,struct _dyneither_ptr);
void Cyc_file_close(struct Cyc___cycFILE*);
# 313
char* getenv(const char*);
# 318
int system(const char*);struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 265 "core.h"
void Cyc_Core_rethrow(void*);
# 269
const char*Cyc_Core_get_exn_name(void*);
# 271
const char*Cyc_Core_get_exn_filename();
# 274
int Cyc_Core_get_exn_lineno();struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 391
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);extern char Cyc_Arg_Bad[4U];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6U];struct Cyc_Arg_Error_exn_struct{char*tag;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
# 66 "arg.h"
void Cyc_Arg_usage(struct Cyc_List_List*,struct _dyneither_ptr);
# 69
extern int Cyc_Arg_current;
# 71
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),int(*anonflagfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr args);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
# 51
int Cyc_strncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 64
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);
# 103 "string.h"
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);
# 108
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);
# 119
struct _dyneither_ptr Cyc_strchr(struct _dyneither_ptr s,char c);
# 30 "filename.h"
struct _dyneither_ptr Cyc_Filename_concat(struct _dyneither_ptr,struct _dyneither_ptr);
# 34
struct _dyneither_ptr Cyc_Filename_chop_extension(struct _dyneither_ptr);
# 40
struct _dyneither_ptr Cyc_Filename_dirname(struct _dyneither_ptr);
# 43
struct _dyneither_ptr Cyc_Filename_basename(struct _dyneither_ptr);
# 46
int Cyc_Filename_check_suffix(struct _dyneither_ptr,struct _dyneither_ptr);
# 28 "position.h"
void Cyc_Position_reset_position(struct _dyneither_ptr);struct Cyc_Position_Error;
# 46
extern int Cyc_Position_use_gcc_style_location;
# 48
extern int Cyc_Position_max_errors;
# 50
int Cyc_Position_error_p();struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0U,Cyc_Absyn_Abstract  = 1U,Cyc_Absyn_Public  = 2U,Cyc_Absyn_Extern  = 3U,Cyc_Absyn_ExternC  = 4U,Cyc_Absyn_Register  = 5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned int loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0U,Cyc_Absyn_Short_sz  = 1U,Cyc_Absyn_Int_sz  = 2U,Cyc_Absyn_Long_sz  = 3U,Cyc_Absyn_LongLong_sz  = 4U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0U,Cyc_Absyn_Unique  = 1U,Cyc_Absyn_Top  = 2U};
# 190
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0U,Cyc_Absyn_MemKind  = 1U,Cyc_Absyn_BoxKind  = 2U,Cyc_Absyn_RgnKind  = 3U,Cyc_Absyn_EffKind  = 4U,Cyc_Absyn_IntKind  = 5U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 210
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0U,Cyc_Absyn_Unsigned  = 1U,Cyc_Absyn_None  = 2U};
# 212
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1181 "absyn.h"
extern int Cyc_Absyn_porting_c_code;
# 1183
extern int Cyc_Absyn_no_regions;extern char Cyc_Lexing_Error[6U];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 28 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
extern int Cyc_Parse_no_register;extern char Cyc_Parse_Exit[5U];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_FlatList{struct Cyc_FlatList*tl;void*hd[0U] __attribute__((aligned )) ;};struct Cyc_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};struct Cyc_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};struct _tuple11{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple10{struct _tuple10*tl;struct _tuple11 hd  __attribute__((aligned )) ;};
# 52
enum Cyc_Storage_class{Cyc_Typedef_sc  = 0U,Cyc_Extern_sc  = 1U,Cyc_ExternC_sc  = 2U,Cyc_Static_sc  = 3U,Cyc_Auto_sc  = 4U,Cyc_Register_sc  = 5U,Cyc_Abstract_sc  = 6U};struct Cyc_Declaration_spec{enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};struct Cyc_Abstractdeclarator{struct Cyc_List_List*tms;};struct Cyc_Numelts_ptrqual_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Region_ptrqual_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Thin_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Fat_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Zeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nozeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Notnull_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nullable_ptrqual_Pointer_qual_struct{int tag;};struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple12{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple12 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple13{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple14{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple10*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Declarator val;};struct _tuple16{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple17{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple17 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple18*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 57
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 26 "absyndump.h"
void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs);
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 29 "binding.h"
extern int Cyc_Binding_warn_override;
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds);struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};
# 46
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*,void*(*mkdata)(void*));
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*,struct _dyneither_ptr*,void*,void*(*mkdata)(void*));
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename);
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt);
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt);
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned int,struct Cyc_Binding_NSCtxt*,struct Cyc_List_List*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
# 32 "rgnorder.h"
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int);
# 39
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*,void*eff,void*rgn,unsigned int);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Tvar*rgn,int opened);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_unordered(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Tvar*rgn);
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 45
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*,void*eff1,void*eff2);
# 48
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;};
# 66 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init();
# 84
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 77 "tcutil.h"
extern int Cyc_Tcutil_warn_alias_coerce;
# 80
extern int Cyc_Tcutil_warn_region_coerce;
# 31 "tc.h"
extern int Cyc_Tc_aggressive_warn;
# 33
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 37
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);struct Cyc_Hashtable_Table;
# 34 "toc.h"
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds);
# 38
void*Cyc_Toc_typ_to_c(void*);extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 47
extern int Cyc_Toc_warn_bounds_checks;
extern int Cyc_Toc_warn_all_null_deref;
void Cyc_Toc_finish();
# 26 "remove_aggregates.h"
struct Cyc_List_List*Cyc_RemoveAggrs_remove_aggrs(struct Cyc_List_List*decls);
# 27 "toseqc.h"
struct Cyc_List_List*Cyc_Toseqc_toseqc(struct Cyc_List_List*decls);
# 27 "tovc.h"
extern int Cyc_Tovc_elim_array_initializers;
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*decls);struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 46 "jump_analysis.h"
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 49 "relations.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0U,Cyc_CfFlowInfo_AllIL  = 1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple19{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple19 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds);
# 37
extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_Interface_I;
# 36 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_empty();
# 45 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_final();
# 49
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Tcenv_Genv*,struct Cyc_List_List*);struct _tuple20{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
# 57
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple20*info);
# 72 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo);
# 78
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);
# 81
void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
# 84
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);
# 25 "warn.h"
void Cyc_Warn_vwarn(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 27
void Cyc_Warn_warn(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 29
void Cyc_Warn_flush_warnings();
# 31
void Cyc_Warn_verr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 33
void Cyc_Warn_err(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 36
void*Cyc_Warn_vimpos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 38
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 29 "specsfile.h"
struct _dyneither_ptr Cyc_Specsfile_target_arch;
struct _dyneither_ptr Cyc_Specsfile_btarget;
void Cyc_Specsfile_set_target_arch(struct _dyneither_ptr s);
struct Cyc_List_List*Cyc_Specsfile_cyclone_exec_path;
void Cyc_Specsfile_add_cyclone_exec_path(struct _dyneither_ptr s);
# 35
struct Cyc_List_List*Cyc_Specsfile_read_specs(struct _dyneither_ptr file);
struct _dyneither_ptr Cyc_Specsfile_split_specs(struct _dyneither_ptr cmdline);
struct _dyneither_ptr Cyc_Specsfile_get_spec(struct Cyc_List_List*specs,struct _dyneither_ptr spec_name);
struct Cyc_List_List*Cyc_Specsfile_cyclone_arch_path;
struct _dyneither_ptr Cyc_Specsfile_def_lib_path;
struct _dyneither_ptr Cyc_Specsfile_parse_b(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),int(*anonflagfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr argv);
# 45
struct _dyneither_ptr Cyc_Specsfile_find_in_arch_path(struct _dyneither_ptr s);
struct _dyneither_ptr Cyc_Specsfile_find_in_exec_path(struct _dyneither_ptr s);
# 24 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 26
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0U,Cyc_Cyclone_Vc_c  = 1U};
# 32
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;
# 40 "cyclone.cyc"
void Cyc_Port_port(struct Cyc_List_List*);
# 43
extern int Cyc_ParseErrors_print_state_and_token;
# 46
extern int Cyc_Lex_compile_for_boot_r;
void Cyc_Lex_pos_init();
void Cyc_Lex_lex_init(int use_cyclone_keywords);
# 57
static int Cyc_pp_r=0;
int Cyc_noexpand_r=0;
static int Cyc_noshake_r=0;
static int Cyc_stop_after_cpp_r=0;
static int Cyc_parseonly_r=0;
static int Cyc_tc_r=0;
static int Cyc_cf_r=0;
static int Cyc_noprint_r=0;
static int Cyc_ic_r=0;
static int Cyc_toc_r=0;
static int Cyc_stop_after_objectfile_r=0;
static int Cyc_stop_after_asmfile_r=0;
static int Cyc_inline_functions_r=0;
static int Cyc_elim_se_r=0;
static int Cyc_v_r=0;
static int Cyc_save_temps_r=0;
static int Cyc_save_c_r=0;
static int Cyc_nogc_r=0;
static int Cyc_pa_r=0;
static int Cyc_pg_r=0;
static int Cyc_nocheck_r=0;
static int Cyc_add_cyc_namespace_r=1;
static int Cyc_generate_line_directives_r=1;
static int Cyc_print_full_evars_r=0;
static int Cyc_print_all_tvars_r=0;
static int Cyc_print_all_kinds_r=0;
static int Cyc_print_all_effects_r=0;
static int Cyc_nocyc_setjmp_r=0;
static int Cyc_generate_interface_r=0;
static int Cyc_toseqc_r=1;
# 89
static struct Cyc_List_List*Cyc_ccargs=0;
static void Cyc_add_ccarg(struct _dyneither_ptr s){
({struct Cyc_List_List*_tmp27B=({struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));(({struct _dyneither_ptr*_tmp27A=({struct _dyneither_ptr*_tmp0=_cycalloc(sizeof(*_tmp0));*_tmp0=s;_tmp0;});(*_tmp1).hd=_tmp27A;}),(*_tmp1).tl=Cyc_ccargs);_tmp1;});Cyc_ccargs=_tmp27B;});}
# 97
void Cyc_set_c_compiler(struct _dyneither_ptr s){
if(({struct _dyneither_ptr _tmp27C=(struct _dyneither_ptr)s;Cyc_strcmp(_tmp27C,({const char*_tmp2="vc";_tag_dyneither(_tmp2,sizeof(char),3U);}));})== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Vc_c;
Cyc_add_ccarg(({const char*_tmp3="-DVC_C";_tag_dyneither(_tmp3,sizeof(char),7U);}));}else{
# 102
if(({struct _dyneither_ptr _tmp27D=(struct _dyneither_ptr)s;Cyc_strcmp(_tmp27D,({const char*_tmp4="gcc";_tag_dyneither(_tmp4,sizeof(char),4U);}));})== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;
Cyc_add_ccarg(({const char*_tmp5="-DGCC_C";_tag_dyneither(_tmp5,sizeof(char),8U);}));}}}
# 109
void Cyc_set_port_c_code(){
Cyc_Absyn_porting_c_code=1;
Cyc_Position_max_errors=65535;
Cyc_save_c_r=1;
Cyc_parseonly_r=1;}
# 116
static struct _dyneither_ptr*Cyc_output_file=0;
static void Cyc_set_output_file(struct _dyneither_ptr s){
({struct _dyneither_ptr*_tmp27E=({struct _dyneither_ptr*_tmp6=_cycalloc(sizeof(*_tmp6));*_tmp6=s;_tmp6;});Cyc_output_file=_tmp27E;});}
# 121
static struct _dyneither_ptr Cyc_specified_interface={(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_set_specified_interface(struct _dyneither_ptr s){
Cyc_specified_interface=s;}
# 131
extern char* Cdef_lib_path;
extern char* Carch;
extern char* Cversion;static char _tmp7[1U]="";
# 136
static struct _dyneither_ptr Cyc_cpp={_tmp7,_tmp7,_tmp7 + 1U};
static void Cyc_set_cpp(struct _dyneither_ptr s){
Cyc_cpp=s;}
# 141
static struct Cyc_List_List*Cyc_cppargs=0;
static void Cyc_add_cpparg(struct _dyneither_ptr s){
({struct Cyc_List_List*_tmp280=({struct Cyc_List_List*_tmp9=_cycalloc(sizeof(*_tmp9));(({struct _dyneither_ptr*_tmp27F=({struct _dyneither_ptr*_tmp8=_cycalloc(sizeof(*_tmp8));*_tmp8=s;_tmp8;});(*_tmp9).hd=_tmp27F;}),(*_tmp9).tl=Cyc_cppargs);_tmp9;});Cyc_cppargs=_tmp280;});}
# 146
static void Cyc_add_cpp_and_ccarg(struct _dyneither_ptr s){
Cyc_add_cpparg(s);
Cyc_add_ccarg(s);}
# 151
static void Cyc_add_iprefix(struct _dyneither_ptr s){
Cyc_add_cpparg((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpC;_tmpC.tag=0U;_tmpC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmpA[1U];_tmpA[0]=& _tmpC;({struct _dyneither_ptr _tmp281=({const char*_tmpB="-iprefix %s";_tag_dyneither(_tmpB,sizeof(char),12U);});Cyc_aprintf(_tmp281,_tag_dyneither(_tmpA,sizeof(void*),1U));});});}));}
# 154
static void Cyc_add_iwithprefix(struct _dyneither_ptr s){
Cyc_add_cpparg((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpF;_tmpF.tag=0U;_tmpF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmpD[1U];_tmpD[0]=& _tmpF;({struct _dyneither_ptr _tmp282=({const char*_tmpE="-iwithprefix %s";_tag_dyneither(_tmpE,sizeof(char),16U);});Cyc_aprintf(_tmp282,_tag_dyneither(_tmpD,sizeof(void*),1U));});});}));}
# 157
static void Cyc_add_iwithprefixbefore(struct _dyneither_ptr s){
Cyc_add_cpparg((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp12;_tmp12.tag=0U;_tmp12.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp10[1U];_tmp10[0]=& _tmp12;({struct _dyneither_ptr _tmp283=({const char*_tmp11="-iwithprefixbefore %s";_tag_dyneither(_tmp11,sizeof(char),22U);});Cyc_aprintf(_tmp283,_tag_dyneither(_tmp10,sizeof(void*),1U));});});}));}
# 160
static void Cyc_add_isystem(struct _dyneither_ptr s){
Cyc_add_cpparg((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp15;_tmp15.tag=0U;_tmp15.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp13[1U];_tmp13[0]=& _tmp15;({struct _dyneither_ptr _tmp284=({const char*_tmp14="-isystem %s";_tag_dyneither(_tmp14,sizeof(char),12U);});Cyc_aprintf(_tmp284,_tag_dyneither(_tmp13,sizeof(void*),1U));});});}));}
# 163
static void Cyc_add_idirafter(struct _dyneither_ptr s){
Cyc_add_cpparg((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp18;_tmp18.tag=0U;_tmp18.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp16[1U];_tmp16[0]=& _tmp18;({struct _dyneither_ptr _tmp285=({const char*_tmp17="-idirafter %s";_tag_dyneither(_tmp17,sizeof(char),14U);});Cyc_aprintf(_tmp285,_tag_dyneither(_tmp16,sizeof(void*),1U));});});}));}
# 167
static void Cyc_set_many_errors(){
Cyc_Position_max_errors=65535;}static char _tmp19[20U]="_see_cycspecs_file_";
# 171
static struct _dyneither_ptr Cyc_def_inc_path={_tmp19,_tmp19,_tmp19 + 20U};
# 174
static void Cyc_print_version(){
({struct Cyc_String_pa_PrintArg_struct _tmp1C;_tmp1C.tag=0U;({struct _dyneither_ptr _tmp286=(struct _dyneither_ptr)({char*_tmp1D= Cversion;_tag_dyneither(_tmp1D,sizeof(char),_get_zero_arr_size_char((void*)_tmp1D,1U));});_tmp1C.f1=_tmp286;});({void*_tmp1A[1U];_tmp1A[0]=& _tmp1C;({struct _dyneither_ptr _tmp287=({const char*_tmp1B="The Cyclone compiler, version %s\n";_tag_dyneither(_tmp1B,sizeof(char),34U);});Cyc_printf(_tmp287,_tag_dyneither(_tmp1A,sizeof(void*),1U));});});});
({struct Cyc_String_pa_PrintArg_struct _tmp20;_tmp20.tag=0U;({struct _dyneither_ptr _tmp288=(struct _dyneither_ptr)({char*_tmp21= Carch;_tag_dyneither(_tmp21,sizeof(char),_get_zero_arr_size_char((void*)_tmp21,1U));});_tmp20.f1=_tmp288;});({void*_tmp1E[1U];_tmp1E[0]=& _tmp20;({struct _dyneither_ptr _tmp289=({const char*_tmp1F="Compiled for architecture: %s\n";_tag_dyneither(_tmp1F,sizeof(char),31U);});Cyc_printf(_tmp289,_tag_dyneither(_tmp1E,sizeof(void*),1U));});});});
({struct Cyc_String_pa_PrintArg_struct _tmp24;_tmp24.tag=0U;({struct _dyneither_ptr _tmp28A=(struct _dyneither_ptr)({char*_tmp25= Cdef_lib_path;_tag_dyneither(_tmp25,sizeof(char),_get_zero_arr_size_char((void*)_tmp25,1U));});_tmp24.f1=_tmp28A;});({void*_tmp22[1U];_tmp22[0]=& _tmp24;({struct _dyneither_ptr _tmp28B=({const char*_tmp23="Standard library directory: %s\n";_tag_dyneither(_tmp23,sizeof(char),32U);});Cyc_printf(_tmp28B,_tag_dyneither(_tmp22,sizeof(void*),1U));});});});
# 179
({struct Cyc_String_pa_PrintArg_struct _tmp28;_tmp28.tag=0U;_tmp28.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_def_inc_path);({void*_tmp26[1U];_tmp26[0]=& _tmp28;({struct _dyneither_ptr _tmp28C=({const char*_tmp27="Standard include directory: %s\n";_tag_dyneither(_tmp27,sizeof(char),32U);});Cyc_printf(_tmp28C,_tag_dyneither(_tmp26,sizeof(void*),1U));});});});
 exit(0);}
# 183
static int Cyc_is_cyclone_sourcefile(struct _dyneither_ptr s){
unsigned long _tmp29=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp29 <= 4)return 0;else{
return({struct _dyneither_ptr _tmp28D=(struct _dyneither_ptr)_dyneither_ptr_plus(s,sizeof(char),(int)(_tmp29 - 4));Cyc_strcmp(_tmp28D,({const char*_tmp2A=".cyc";_tag_dyneither(_tmp2A,sizeof(char),5U);}));})== 0;}}
# 199 "cyclone.cyc"
static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _dyneither_ptr s){
if(({struct _dyneither_ptr _tmp28E=(struct _dyneither_ptr)s;Cyc_strcmp(_tmp28E,({const char*_tmp2B="-lxml";_tag_dyneither(_tmp2B,sizeof(char),6U);}));})== 0){
if(!Cyc_pa_r)
Cyc_add_ccarg(s);else{
# 205
Cyc_add_ccarg(({const char*_tmp2C="-lxml_a";_tag_dyneither(_tmp2C,sizeof(char),8U);}));}}else{
if(({struct _dyneither_ptr _tmp28F=(struct _dyneither_ptr)s;Cyc_strncmp(_tmp28F,({const char*_tmp2D="-lcyc";_tag_dyneither(_tmp2D,sizeof(char),6U);}),5U);})== 0)
Cyc_add_ccarg(s);else{
# 209
({struct Cyc_List_List*_tmp291=({struct Cyc_List_List*_tmp2F=_cycalloc(sizeof(*_tmp2F));(({struct _dyneither_ptr*_tmp290=({struct _dyneither_ptr*_tmp2E=_cycalloc(sizeof(*_tmp2E));*_tmp2E=s;_tmp2E;});(*_tmp2F).hd=_tmp290;}),(*_tmp2F).tl=Cyc_libargs);_tmp2F;});Cyc_libargs=_tmp291;});}}}
# 212
static void Cyc_add_marg(struct _dyneither_ptr s){
Cyc_add_ccarg(s);}
# 216
static void Cyc_set_save_temps(){
Cyc_save_temps_r=1;
Cyc_add_ccarg(({const char*_tmp30="-save-temps";_tag_dyneither(_tmp30,sizeof(char),12U);}));}
# 221
static int Cyc_produce_dependencies=0;
static void Cyc_set_produce_dependencies(){
Cyc_stop_after_cpp_r=1;
Cyc_produce_dependencies=1;
Cyc_add_cpparg(({const char*_tmp31="-M";_tag_dyneither(_tmp31,sizeof(char),3U);}));}
# 228
static struct _dyneither_ptr*Cyc_dependencies_target=0;
static void Cyc_set_dependencies_target(struct _dyneither_ptr s){
({struct _dyneither_ptr*_tmp292=({struct _dyneither_ptr*_tmp32=_cycalloc(sizeof(*_tmp32));*_tmp32=s;_tmp32;});Cyc_dependencies_target=_tmp292;});}
# 233
static void Cyc_set_stop_after_objectfile(){
Cyc_stop_after_objectfile_r=1;
Cyc_add_ccarg(({const char*_tmp33="-c";_tag_dyneither(_tmp33,sizeof(char),3U);}));}
# 238
static void Cyc_set_nocppprecomp(){
Cyc_add_cpp_and_ccarg(({const char*_tmp34="-no-cpp-precomp";_tag_dyneither(_tmp34,sizeof(char),16U);}));}
# 242
static void Cyc_clear_lineno(){
Cyc_generate_line_directives_r=0;
Cyc_set_save_temps();}
# 246
static void Cyc_set_inline_functions(){
Cyc_inline_functions_r=1;}
# 249
static void Cyc_set_elim_se(){
Cyc_elim_se_r=1;
Cyc_set_inline_functions();}
# 253
static void Cyc_set_tovc(){
Cyc_Cyclone_tovc_r=1;
Cyc_add_ccarg(({const char*_tmp35="-DCYC_ANSI_OUTPUT";_tag_dyneither(_tmp35,sizeof(char),18U);}));
Cyc_set_elim_se();}
# 258
static void Cyc_set_notoseqc(){
Cyc_toseqc_r=0;}
# 261
static void Cyc_set_noboundschecks(){
Cyc_add_ccarg(({const char*_tmp36="-DNO_CYC_BOUNDS_CHECKS";_tag_dyneither(_tmp36,sizeof(char),23U);}));}
# 264
static void Cyc_set_nonullchecks(){
Cyc_add_ccarg(({const char*_tmp37="-DNO_CYC_NULL_CHECKS";_tag_dyneither(_tmp37,sizeof(char),21U);}));}
# 267
static void Cyc_set_nochecks(){
Cyc_set_noboundschecks();
Cyc_set_nonullchecks();
Cyc_nocheck_r=1;}
# 273
static void Cyc_set_nocyc(){
Cyc_add_cyc_namespace_r=0;
Cyc_add_ccarg(({const char*_tmp38="-DNO_CYC_PREFIX";_tag_dyneither(_tmp38,sizeof(char),16U);}));}
# 278
static void Cyc_set_pa(){
Cyc_pa_r=1;
Cyc_add_ccarg(({const char*_tmp39="-DCYC_REGION_PROFILE";_tag_dyneither(_tmp39,sizeof(char),21U);}));
Cyc_add_cpparg(({const char*_tmp3A="-DCYC_REGION_PROFILE";_tag_dyneither(_tmp3A,sizeof(char),21U);}));}
# 284
static void Cyc_set_pg(){
Cyc_pg_r=1;
Cyc_add_ccarg(({const char*_tmp3B="-pg";_tag_dyneither(_tmp3B,sizeof(char),4U);}));}
# 289
static void Cyc_set_stop_after_asmfile(){
Cyc_stop_after_asmfile_r=1;
Cyc_add_ccarg(({const char*_tmp3C="-S";_tag_dyneither(_tmp3C,sizeof(char),3U);}));}
# 294
static void Cyc_set_all_warnings(){
Cyc_Toc_warn_bounds_checks=1;
Cyc_Toc_warn_all_null_deref=1;
Cyc_NewControlFlow_warn_lose_unique=1;
Cyc_Tcutil_warn_alias_coerce=1;
Cyc_Tcutil_warn_region_coerce=1;
Cyc_Tc_aggressive_warn=1;
Cyc_Binding_warn_override=1;}
# 305
enum Cyc_inputtype{Cyc_DEFAULTINPUT  = 0U,Cyc_CYCLONEFILE  = 1U};
# 310
static enum Cyc_inputtype Cyc_intype=Cyc_DEFAULTINPUT;
static void Cyc_set_inputtype(struct _dyneither_ptr s){
if(({struct _dyneither_ptr _tmp293=(struct _dyneither_ptr)s;Cyc_strcmp(_tmp293,({const char*_tmp3D="cyc";_tag_dyneither(_tmp3D,sizeof(char),4U);}));})== 0)
Cyc_intype=Cyc_CYCLONEFILE;else{
if(({struct _dyneither_ptr _tmp294=(struct _dyneither_ptr)s;Cyc_strcmp(_tmp294,({const char*_tmp3E="none";_tag_dyneither(_tmp3E,sizeof(char),5U);}));})== 0)
Cyc_intype=Cyc_DEFAULTINPUT;else{
# 317
({struct Cyc_String_pa_PrintArg_struct _tmp41;_tmp41.tag=0U;_tmp41.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp3F[1U];_tmp3F[0]=& _tmp41;({struct Cyc___cycFILE*_tmp296=Cyc_stderr;struct _dyneither_ptr _tmp295=({const char*_tmp40="Input type '%s' not supported\n";_tag_dyneither(_tmp40,sizeof(char),31U);});Cyc_fprintf(_tmp296,_tmp295,_tag_dyneither(_tmp3F,sizeof(void*),1U));});});});}}}
# 320
struct _dyneither_ptr Cyc_make_base_filename(struct _dyneither_ptr s,struct _dyneither_ptr*output_file){
struct _dyneither_ptr _tmp42=Cyc_Filename_dirname(output_file == 0?s:*output_file);
struct _dyneither_ptr _tmp43=Cyc_Filename_chop_extension((struct _dyneither_ptr)Cyc_Filename_basename(s));
# 326
struct _dyneither_ptr _tmp44=Cyc_strlen((struct _dyneither_ptr)_tmp42)> 0?Cyc_Filename_concat((struct _dyneither_ptr)_tmp42,(struct _dyneither_ptr)_tmp43): _tmp43;
return(struct _dyneither_ptr)_tmp44;}
# 332
static struct Cyc_List_List*Cyc_cyclone_files=0;
# 334
static int Cyc_gcc_optarg=0;
static void Cyc_add_other(struct _dyneither_ptr s){
# 337
if(Cyc_gcc_optarg > 0){
Cyc_add_ccarg(s);
-- Cyc_gcc_optarg;}else{
# 341
if(Cyc_intype == Cyc_CYCLONEFILE  || Cyc_is_cyclone_sourcefile(s)){
# 343
({struct Cyc_List_List*_tmp298=({struct Cyc_List_List*_tmp46=_cycalloc(sizeof(*_tmp46));(({struct _dyneither_ptr*_tmp297=({struct _dyneither_ptr*_tmp45=_cycalloc(sizeof(*_tmp45));*_tmp45=s;_tmp45;});(*_tmp46).hd=_tmp297;}),(*_tmp46).tl=Cyc_cyclone_files);_tmp46;});Cyc_cyclone_files=_tmp298;});{
# 346
struct _dyneither_ptr _tmp47=Cyc_make_base_filename(s,Cyc_output_file);
struct _dyneither_ptr _tmp48=({struct _dyneither_ptr _tmp299=(struct _dyneither_ptr)_tmp47;Cyc_strconcat(_tmp299,({const char*_tmp49=".c";_tag_dyneither(_tmp49,sizeof(char),3U);}));});
Cyc_add_ccarg((struct _dyneither_ptr)_tmp48);};}else{
# 350
Cyc_add_ccarg(s);}}}
# 355
static int Cyc_assume_gcc_flag=1;struct _tuple21{struct _dyneither_ptr flag;int optargs;};static char _tmp4A[3U]="-s";static char _tmp4B[3U]="-O";static char _tmp4C[4U]="-O0";static char _tmp4D[4U]="-O2";static char _tmp4E[4U]="-O3";static char _tmp4F[21U]="-fomit-frame-pointer";static char _tmp50[13U]="-fno-builtin";static char _tmp51[3U]="-g";static char _tmp52[4U]="-pg";static char _tmp53[3U]="-p";static char _tmp54[8U]="-static";
static int Cyc_add_other_flag(struct _dyneither_ptr s){
static struct _tuple21 known_gcc_flags[11U]={{.flag={_tmp4A,_tmp4A,_tmp4A + 3U},.optargs=0},{.flag={_tmp4B,_tmp4B,_tmp4B + 3U},.optargs=0},{.flag={_tmp4C,_tmp4C,_tmp4C + 4U},.optargs=0},{.flag={_tmp4D,_tmp4D,_tmp4D + 4U},.optargs=0},{.flag={_tmp4E,_tmp4E,_tmp4E + 4U},.optargs=0},{.flag={_tmp4F,_tmp4F,_tmp4F + 21U},.optargs=0},{.flag={_tmp50,_tmp50,_tmp50 + 13U},.optargs=0},{.flag={_tmp51,_tmp51,_tmp51 + 3U},.optargs=0},{.flag={_tmp52,_tmp52,_tmp52 + 4U},.optargs=0},{.flag={_tmp53,_tmp53,_tmp53 + 3U},.optargs=0},{.flag={_tmp54,_tmp54,_tmp54 + 8U},.optargs=0}};
# 370
if(Cyc_assume_gcc_flag)
Cyc_add_ccarg(s);else{
# 373
{int i=0;for(0;i < 11U;++ i){
if(!Cyc_strcmp((struct _dyneither_ptr)(known_gcc_flags[_check_known_subscript_notnull(11U,i)]).flag,(struct _dyneither_ptr)s)){
Cyc_add_ccarg(s);
Cyc_gcc_optarg=(known_gcc_flags[i]).optargs;
break;}}}
# 380
return 0;}
# 382
return 1;}
# 385
static void Cyc_noassume_gcc_flag(){
Cyc_assume_gcc_flag=0;}
# 389
static void Cyc_remove_file(struct _dyneither_ptr s){
if(Cyc_save_temps_r)return;else{
 remove((const char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1U)));}}
# 396
int Cyc_compile_failure=0;
# 398
struct Cyc___cycFILE*Cyc_try_file_open(struct _dyneither_ptr filename,struct _dyneither_ptr mode,struct _dyneither_ptr msg_part){
struct _handler_cons _tmp55;_push_handler(& _tmp55);{int _tmp57=0;if(setjmp(_tmp55.handler))_tmp57=1;if(!_tmp57){{struct Cyc___cycFILE*_tmp58=Cyc_file_open(filename,mode);_npop_handler(0U);return _tmp58;};_pop_handler();}else{void*_tmp56=(void*)_exn_thrown;void*_tmp59=_tmp56;_LL1: _LL2:
# 402
 Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _tmp5D;_tmp5D.tag=0U;_tmp5D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename);({struct Cyc_String_pa_PrintArg_struct _tmp5C;_tmp5C.tag=0U;_tmp5C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part);({void*_tmp5A[2U];(_tmp5A[0]=& _tmp5C,_tmp5A[1]=& _tmp5D);({struct Cyc___cycFILE*_tmp29B=Cyc_stderr;struct _dyneither_ptr _tmp29A=({const char*_tmp5B="\nError: couldn't open %s %s\n";_tag_dyneither(_tmp5B,sizeof(char),29U);});Cyc_fprintf(_tmp29B,_tmp29A,_tag_dyneither(_tmp5A,sizeof(void*),2U));});});});});
Cyc_fflush(Cyc_stderr);
return 0;_LL0:;}};}
# 409
void CYCALLOCPROFILE_mark(const char*s);
# 411
void*Cyc_do_stage(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,void*(*f)(void*,struct Cyc_List_List*),void*env,void(*on_fail)(void*),void*failenv){
# 414
 CYCALLOCPROFILE_mark((const char*)_untag_dyneither_ptr(stage_name,sizeof(char),1U));{
# 416
void*ans;
{struct _handler_cons _tmp5E;_push_handler(& _tmp5E);{int _tmp60=0;if(setjmp(_tmp5E.handler))_tmp60=1;if(!_tmp60){({void*_tmp29C=f(env,tds);ans=_tmp29C;});;_pop_handler();}else{void*_tmp5F=(void*)_exn_thrown;void*_tmp61=_tmp5F;void*_tmp65;_LL1: _tmp65=_tmp61;_LL2:
# 420
({struct Cyc_String_pa_PrintArg_struct _tmp64;_tmp64.tag=0U;_tmp64.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name);({void*_tmp62[1U];_tmp62[0]=& _tmp64;({struct Cyc___cycFILE*_tmp29E=Cyc_stderr;struct _dyneither_ptr _tmp29D=({const char*_tmp63="COMPILER STAGE %s\n";_tag_dyneither(_tmp63,sizeof(char),19U);});Cyc_fprintf(_tmp29E,_tmp29D,_tag_dyneither(_tmp62,sizeof(void*),1U));});});});
on_fail(failenv);
Cyc_Core_rethrow(_tmp65);_LL0:;}};}
# 424
if(Cyc_Position_error_p())
Cyc_compile_failure=1;
if(Cyc_compile_failure){
on_fail(failenv);
({void*_tmp66=0U;({struct Cyc___cycFILE*_tmp2A0=Cyc_stderr;struct _dyneither_ptr _tmp29F=({const char*_tmp67="\nCOMPILATION FAILED!\n";_tag_dyneither(_tmp67,sizeof(char),22U);});Cyc_fprintf(_tmp2A0,_tmp29F,_tag_dyneither(_tmp66,sizeof(void*),0U));});});
Cyc_fflush(Cyc_stderr);
return ans;}else{
# 432
if(Cyc_v_r){
({struct Cyc_String_pa_PrintArg_struct _tmp6A;_tmp6A.tag=0U;_tmp6A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name);({void*_tmp68[1U];_tmp68[0]=& _tmp6A;({struct Cyc___cycFILE*_tmp2A2=Cyc_stderr;struct _dyneither_ptr _tmp2A1=({const char*_tmp69="%s completed.\n";_tag_dyneither(_tmp69,sizeof(char),15U);});Cyc_fprintf(_tmp2A2,_tmp2A1,_tag_dyneither(_tmp68,sizeof(void*),1U));});});});
Cyc_fflush(Cyc_stderr);
return ans;}}
# 437
return ans;};}
# 440
static void Cyc_ignore(void*x){;}
static void Cyc_remove_fileptr(struct _dyneither_ptr*s){Cyc_remove_file((struct _dyneither_ptr)*s);}
# 443
struct Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore){
Cyc_Lex_lex_init(1);
Cyc_Lex_pos_init();{
struct Cyc_List_List*ans=0;
{struct _handler_cons _tmp6B;_push_handler(& _tmp6B);{int _tmp6D=0;if(setjmp(_tmp6B.handler))_tmp6D=1;if(!_tmp6D){
({struct Cyc_List_List*_tmp2A3=Cyc_Parse_parse_file(f);ans=_tmp2A3;});
Cyc_file_close(f);
# 448
;_pop_handler();}else{void*_tmp6C=(void*)_exn_thrown;void*_tmp6E=_tmp6C;void*_tmp6F;if(((struct Cyc_Parse_Exit_exn_struct*)_tmp6E)->tag == Cyc_Parse_Exit){_LL1: _LL2:
# 451
 Cyc_file_close(f);Cyc_compile_failure=1;goto _LL0;}else{_LL3: _tmp6F=_tmp6E;_LL4:
 Cyc_file_close(f);Cyc_Core_rethrow(_tmp6F);}_LL0:;}};}
# 454
Cyc_Lex_lex_init(1);
return ans;};}
# 458
int Cyc_do_binding(int ignore,struct Cyc_List_List*tds){
Cyc_Binding_resolve_all(tds);
return 1;}
# 463
struct Cyc_List_List*Cyc_do_typecheck(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tds){
# 465
Cyc_Tc_tc(te,1,tds);
if(!Cyc_noshake_r)
({struct Cyc_List_List*_tmp2A4=Cyc_Tc_treeshake(te,tds);tds=_tmp2A4;});
return tds;}
# 471
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_do_jumpanalysis(int ignore,struct Cyc_List_List*tds){
# 473
return Cyc_JumpAnalysis_jump_analysis(tds);}
# 476
struct Cyc_List_List*Cyc_do_cfcheck(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*tds){
# 478
Cyc_NewControlFlow_cf_check(tables,tds);
return tds;}struct _tuple22{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*f3;};
# 484
struct Cyc_List_List*Cyc_do_interface(struct _tuple22*params,struct Cyc_List_List*tds){
# 486
struct _tuple22*_tmp70=params;struct Cyc_Tcenv_Tenv*_tmp78;struct Cyc___cycFILE*_tmp77;struct Cyc___cycFILE*_tmp76;_LL1: _tmp78=_tmp70->f1;_tmp77=_tmp70->f2;_tmp76=_tmp70->f3;_LL2:;{
struct Cyc_Interface_I*_tmp71=Cyc_Interface_extract(_tmp78->ae,tds);
if(_tmp77 == 0)
Cyc_Interface_save(_tmp71,_tmp76);else{
# 491
struct Cyc_Interface_I*_tmp72=Cyc_Interface_parse(_tmp77);
if(!({struct Cyc_Interface_I*_tmp2A8=_tmp72;struct Cyc_Interface_I*_tmp2A7=_tmp71;Cyc_Interface_is_subinterface(_tmp2A8,_tmp2A7,({struct _tuple20*_tmp75=_cycalloc(sizeof(*_tmp75));(({struct _dyneither_ptr _tmp2A6=({const char*_tmp73="written interface";_tag_dyneither(_tmp73,sizeof(char),18U);});(*_tmp75).f1=_tmp2A6;}),({struct _dyneither_ptr _tmp2A5=({const char*_tmp74="maximal interface";_tag_dyneither(_tmp74,sizeof(char),18U);});(*_tmp75).f2=_tmp2A5;}));_tmp75;}));}))
Cyc_compile_failure=1;else{
# 495
Cyc_Interface_save(_tmp72,_tmp76);}}
# 497
return tds;};}
# 499
void Cyc_interface_fail(struct _tuple22*params){
struct _tuple22*_tmp79=params;struct Cyc___cycFILE*_tmp7B;struct Cyc___cycFILE*_tmp7A;_LL1: _tmp7B=_tmp79->f2;_tmp7A=_tmp79->f3;_LL2:;
if(_tmp7B != 0)
Cyc_file_close(_tmp7B);
Cyc_file_close(_tmp7A);}
# 506
struct Cyc_List_List*Cyc_do_translate(struct Cyc_Hashtable_Table*pops,struct Cyc_List_List*tds){
# 509
return Cyc_Toc_toc(pops,tds);}
# 511
struct Cyc_List_List*Cyc_do_removeaggrs(int ignore,struct Cyc_List_List*tds){
return Cyc_RemoveAggrs_remove_aggrs(tds);}
# 515
struct Cyc_List_List*Cyc_do_toseqc(int ignore,struct Cyc_List_List*tds){
return Cyc_Toseqc_toseqc(tds);}
# 518
struct Cyc_List_List*Cyc_do_tovc(int ignore,struct Cyc_List_List*tds){
Cyc_Tovc_elim_array_initializers=Cyc_Cyclone_tovc_r;
return Cyc_Tovc_tovc(tds);}
# 523
static struct _dyneither_ptr Cyc_cyc_setjmp={(void*)0,(void*)0,(void*)(0 + 0)};
# 525
static struct _dyneither_ptr Cyc_cyc_include={(void*)0,(void*)0,(void*)(0 + 0)};
# 527
static void Cyc_set_cyc_include(struct _dyneither_ptr f){
Cyc_cyc_include=f;}
# 532
int Cyc_copy_internal_file(struct _dyneither_ptr file,struct Cyc___cycFILE*out_file){
# 535
if(({char*_tmp2A9=(char*)file.curr;_tmp2A9 == (char*)(_tag_dyneither(0,0,0)).curr;})){
({void*_tmp7C=0U;({struct Cyc___cycFILE*_tmp2AB=Cyc_stderr;struct _dyneither_ptr _tmp2AA=({const char*_tmp7D="Internal error: copy_internal_file called with NULL\n";_tag_dyneither(_tmp7D,sizeof(char),53U);});Cyc_fprintf(_tmp2AB,_tmp2AA,_tag_dyneither(_tmp7C,sizeof(void*),0U));});});
return 1;}{
# 539
struct Cyc___cycFILE*_tmp7E=({struct _dyneither_ptr _tmp2AD=file;struct _dyneither_ptr _tmp2AC=({const char*_tmp85="r";_tag_dyneither(_tmp85,sizeof(char),2U);});Cyc_try_file_open(_tmp2AD,_tmp2AC,({const char*_tmp86="internal compiler file";_tag_dyneither(_tmp86,sizeof(char),23U);}));});
if(_tmp7E == 0)return 1;{
unsigned long n_read=1024U;
unsigned long n_written;
char buf[1024U];({{unsigned int _tmp277=1024U;unsigned int i;for(i=0;i < _tmp277;++ i){buf[i]='\000';}}0;});
while(n_read == 1024){
({unsigned long _tmp2AF=({struct _dyneither_ptr _tmp2AE=_tag_dyneither(buf,sizeof(char),1024U);Cyc_fread(_tmp2AE,1U,1024U,_tmp7E);});n_read=_tmp2AF;});
if(n_read != 1024  && !Cyc_feof(_tmp7E)){
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _tmp81;_tmp81.tag=0U;_tmp81.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);({void*_tmp7F[1U];_tmp7F[0]=& _tmp81;({struct Cyc___cycFILE*_tmp2B1=Cyc_stderr;struct _dyneither_ptr _tmp2B0=({const char*_tmp80="\nError: problem copying %s\n";_tag_dyneither(_tmp80,sizeof(char),28U);});Cyc_fprintf(_tmp2B1,_tmp2B0,_tag_dyneither(_tmp7F,sizeof(void*),1U));});});});
Cyc_fflush(Cyc_stderr);
return 1;}
# 552
({unsigned long _tmp2B4=({struct _dyneither_ptr _tmp2B3=_tag_dyneither(buf,sizeof(char),1024U);unsigned long _tmp2B2=n_read;Cyc_fwrite(_tmp2B3,1U,_tmp2B2,out_file);});n_written=_tmp2B4;});
if(n_read != n_written){
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _tmp84;_tmp84.tag=0U;_tmp84.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);({void*_tmp82[1U];_tmp82[0]=& _tmp84;({struct Cyc___cycFILE*_tmp2B6=Cyc_stderr;struct _dyneither_ptr _tmp2B5=({const char*_tmp83="\nError: problem copying %s\n";_tag_dyneither(_tmp83,sizeof(char),28U);});Cyc_fprintf(_tmp2B6,_tmp2B5,_tag_dyneither(_tmp82,sizeof(void*),1U));});});});
Cyc_fflush(Cyc_stderr);
return 1;}}
# 560
Cyc_fclose(_tmp7E);
return 0;};};}
# 566
static struct Cyc_List_List*Cyc_cfiles=0;
static void Cyc_remove_cfiles(){
if(!Cyc_save_c_r)
for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){
Cyc_remove_file((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}
# 572
static void Cyc_find_fail(struct _dyneither_ptr file){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp8A=_cycalloc(sizeof(*_tmp8A));((*_tmp8A).tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmp2B8=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp89;_tmp89.tag=0U;_tmp89.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);({void*_tmp87[1U];_tmp87[0]=& _tmp89;({struct _dyneither_ptr _tmp2B7=({const char*_tmp88="Error: can't find internal compiler file %s\n";_tag_dyneither(_tmp88,sizeof(char),45U);});Cyc_aprintf(_tmp2B7,_tag_dyneither(_tmp87,sizeof(void*),1U));});});});(*_tmp8A).f1=_tmp2B8;}));_tmp8A;}));}
# 577
static struct _dyneither_ptr Cyc_find_in_arch_path(struct _dyneither_ptr s){
struct _dyneither_ptr _tmp8B=Cyc_Specsfile_find_in_arch_path(s);
if(!((unsigned int)_tmp8B.curr))Cyc_find_fail(s);
return _tmp8B;}
# 582
static struct _dyneither_ptr Cyc_find_in_exec_path(struct _dyneither_ptr s){
struct _dyneither_ptr _tmp8C=Cyc_Specsfile_find_in_exec_path(s);
if(!((unsigned int)_tmp8C.curr))Cyc_find_fail(s);
return _tmp8C;}struct _tuple23{struct Cyc___cycFILE*f1;struct _dyneither_ptr f2;};
# 588
struct Cyc_List_List*Cyc_do_print(struct _tuple23*env,struct Cyc_List_List*tds){
# 590
struct _tuple23*_tmp8D=env;struct Cyc___cycFILE*_tmp96;struct _dyneither_ptr _tmp95;_LL1: _tmp96=_tmp8D->f1;_tmp95=_tmp8D->f2;_LL2:;{
struct Cyc_Absynpp_Params _tmp8E=Cyc_tc_r?Cyc_Absynpp_cyc_params_r: Cyc_Absynpp_c_params_r;
_tmp8E.expand_typedefs=!Cyc_noexpand_r;
_tmp8E.to_VC=Cyc_Cyclone_tovc_r;
_tmp8E.add_cyc_prefix=Cyc_add_cyc_namespace_r;
_tmp8E.generate_line_directives=Cyc_generate_line_directives_r;
_tmp8E.print_full_evars=Cyc_print_full_evars_r;
_tmp8E.print_all_tvars=Cyc_print_all_tvars_r;
_tmp8E.print_all_kinds=Cyc_print_all_kinds_r;
_tmp8E.print_all_effects=Cyc_print_all_effects_r;
# 601
if(Cyc_inline_functions_r)
({void*_tmp8F=0U;({struct Cyc___cycFILE*_tmp2BA=_tmp96;struct _dyneither_ptr _tmp2B9=({const char*_tmp90="#define _INLINE_FUNCTIONS\n";_tag_dyneither(_tmp90,sizeof(char),27U);});Cyc_fprintf(_tmp2BA,_tmp2B9,_tag_dyneither(_tmp8F,sizeof(void*),0U));});});
# 605
if((!Cyc_parseonly_r  && !Cyc_tc_r) && !Cyc_cf_r){
if(!Cyc_nocyc_setjmp_r){
if(Cyc_Lex_compile_for_boot_r)
({void*_tmp91=0U;({struct Cyc___cycFILE*_tmp2BC=_tmp96;struct _dyneither_ptr _tmp2BB=({const char*_tmp92="#include <setjmp.h>\n";_tag_dyneither(_tmp92,sizeof(char),21U);});Cyc_fprintf(_tmp2BC,_tmp2BB,_tag_dyneither(_tmp91,sizeof(void*),0U));});});else{
if(Cyc_copy_internal_file(Cyc_cyc_setjmp,_tmp96))return tds;}}
# 611
if(Cyc_copy_internal_file(Cyc_cyc_include,_tmp96))return tds;}
# 613
if(Cyc_pp_r){
Cyc_Absynpp_set_params(& _tmp8E);
Cyc_Absynpp_decllist2file(tds,_tmp96);}else{
# 617
Cyc_Absyndump_set_params(& _tmp8E);
Cyc_Absyndump_dumpdecllist2file(tds,_tmp96);}
# 620
Cyc_fflush(_tmp96);
Cyc_file_close(_tmp96);
({struct Cyc_List_List*_tmp2BE=({struct Cyc_List_List*_tmp94=_cycalloc(sizeof(*_tmp94));(({struct _dyneither_ptr*_tmp2BD=({struct _dyneither_ptr*_tmp93=_cycalloc(sizeof(*_tmp93));*_tmp93=_tmp95;_tmp93;});(*_tmp94).hd=_tmp2BD;}),(*_tmp94).tl=Cyc_cfiles);_tmp94;});Cyc_cfiles=_tmp2BE;});
return tds;};}
# 625
void Cyc_print_fail(struct _tuple23*env){
struct _tuple23*_tmp97=env;struct Cyc___cycFILE*_tmp9B;struct _dyneither_ptr _tmp9A;_LL1: _tmp9B=_tmp97->f1;_tmp9A=_tmp97->f2;_LL2:;
Cyc_file_close(_tmp9B);
({struct Cyc_List_List*_tmp2C1=({struct Cyc_List_List*_tmp99=_cycalloc(sizeof(*_tmp99));(({struct _dyneither_ptr*_tmp2C0=({struct _dyneither_ptr*_tmp98=_cycalloc(sizeof(*_tmp98));({struct _dyneither_ptr _tmp2BF=Cyc_strdup((struct _dyneither_ptr)_tmp9A);*_tmp98=_tmp2BF;});_tmp98;});(*_tmp99).hd=_tmp2C0;}),(*_tmp99).tl=Cyc_cfiles);_tmp99;});Cyc_cfiles=_tmp2C1;});}
# 632
static struct Cyc_List_List*Cyc_split_by_char(struct _dyneither_ptr s,char c){
if(({char*_tmp2C2=(char*)s.curr;_tmp2C2 == (char*)(_tag_dyneither(0,0,0)).curr;}))return 0;{
struct Cyc_List_List*_tmp9C=0;
unsigned long _tmp9D=Cyc_strlen((struct _dyneither_ptr)s);
# 637
while(_tmp9D > 0){
struct _dyneither_ptr _tmp9E=Cyc_strchr(s,c);
if(({char*_tmp2C3=(char*)_tmp9E.curr;_tmp2C3 == (char*)(_tag_dyneither(0,0,0)).curr;})){
({struct Cyc_List_List*_tmp2C5=({struct Cyc_List_List*_tmpA0=_cycalloc(sizeof(*_tmpA0));(({struct _dyneither_ptr*_tmp2C4=({struct _dyneither_ptr*_tmp9F=_cycalloc(sizeof(*_tmp9F));*_tmp9F=s;_tmp9F;});(*_tmpA0).hd=_tmp2C4;}),(*_tmpA0).tl=_tmp9C);_tmpA0;});_tmp9C=_tmp2C5;});
break;}else{
# 644
({struct Cyc_List_List*_tmp2C8=({struct Cyc_List_List*_tmpA2=_cycalloc(sizeof(*_tmpA2));(({struct _dyneither_ptr*_tmp2C7=({struct _dyneither_ptr*_tmpA1=_cycalloc(sizeof(*_tmpA1));({struct _dyneither_ptr _tmp2C6=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,0,(unsigned long)((((struct _dyneither_ptr)_tmp9E).curr - s.curr)/ sizeof(char)));*_tmpA1=_tmp2C6;});_tmpA1;});(*_tmpA2).hd=_tmp2C7;}),(*_tmpA2).tl=_tmp9C);_tmpA2;});_tmp9C=_tmp2C8;});
_tmp9D -=(((struct _dyneither_ptr)_tmp9E).curr - s.curr)/ sizeof(char);
({struct _dyneither_ptr _tmp2C9=_dyneither_ptr_plus(_tmp9E,sizeof(char),1);s=_tmp2C9;});}}
# 649
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp9C);};}
# 654
static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 656
struct Cyc_List_List*_tmpA3=0;
for(0;dirs != 0;dirs=dirs->tl){
({struct Cyc_List_List*_tmp2CC=({struct Cyc_List_List*_tmpA5=_cycalloc(sizeof(*_tmpA5));(({struct _dyneither_ptr*_tmp2CB=({struct _dyneither_ptr*_tmpA4=_cycalloc(sizeof(*_tmpA4));({struct _dyneither_ptr _tmp2CA=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir);*_tmpA4=_tmp2CA;});_tmpA4;});(*_tmpA5).hd=_tmp2CB;}),(*_tmpA5).tl=_tmpA3);_tmpA5;});_tmpA3=_tmp2CC;});}
# 660
({struct Cyc_List_List*_tmp2CD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpA3);_tmpA3=_tmp2CD;});
return _tmpA3;}
# 665
static int Cyc_is_other_special(char c){
char _tmpA6=c;switch(_tmpA6){case 92U: _LL1: _LL2:
 goto _LL4;case 34U: _LL3: _LL4:
 goto _LL6;case 59U: _LL5: _LL6:
 goto _LL8;case 38U: _LL7: _LL8:
 goto _LLA;case 40U: _LL9: _LLA:
 goto _LLC;case 41U: _LLB: _LLC:
 goto _LLE;case 124U: _LLD: _LLE:
 goto _LL10;case 94U: _LLF: _LL10:
 goto _LL12;case 60U: _LL11: _LL12:
 goto _LL14;case 62U: _LL13: _LL14:
 goto _LL16;case 10U: _LL15: _LL16:
# 680
 goto _LL18;case 9U: _LL17: _LL18:
 return 1;default: _LL19: _LL1A:
 return 0;}_LL0:;}
# 687
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned long _tmpA7=Cyc_strlen((struct _dyneither_ptr)s);
# 691
int _tmpA8=0;
int _tmpA9=0;
{int i=0;for(0;i < _tmpA7;++ i){
char _tmpAA=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmpAA == '\'')++ _tmpA8;else{
if(Cyc_is_other_special(_tmpAA))++ _tmpA9;}}}
# 700
if(_tmpA8 == 0  && _tmpA9 == 0)
return s;
# 704
if(_tmpA8 == 0)
return(struct _dyneither_ptr)Cyc_strconcat_l(({struct _dyneither_ptr*_tmpAB[3U];(((({struct _dyneither_ptr*_tmp2D2=({struct _dyneither_ptr*_tmpAD=_cycalloc(sizeof(*_tmpAD));({struct _dyneither_ptr _tmp2D1=({const char*_tmpAC="'";_tag_dyneither(_tmpAC,sizeof(char),2U);});*_tmpAD=_tmp2D1;});_tmpAD;});_tmpAB[0]=_tmp2D2;}),({struct _dyneither_ptr*_tmp2D0=({struct _dyneither_ptr*_tmpAE=_cycalloc(sizeof(*_tmpAE));*_tmpAE=(struct _dyneither_ptr)s;_tmpAE;});_tmpAB[1]=_tmp2D0;}))),({struct _dyneither_ptr*_tmp2CF=({struct _dyneither_ptr*_tmpB0=_cycalloc(sizeof(*_tmpB0));({struct _dyneither_ptr _tmp2CE=({const char*_tmpAF="'";_tag_dyneither(_tmpAF,sizeof(char),2U);});*_tmpB0=_tmp2CE;});_tmpB0;});_tmpAB[2]=_tmp2CF;}));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAB,sizeof(struct _dyneither_ptr*),3U));}));{
# 708
unsigned long _tmpB1=(_tmpA7 + _tmpA8)+ _tmpA9;
struct _dyneither_ptr s2=({unsigned int _tmpBC=(_tmpB1 + 1)+ 1U;char*_tmpBB=_cycalloc_atomic(_check_times(_tmpBC,sizeof(char)));({{unsigned int _tmp278=_tmpB1 + 1;unsigned int i;for(i=0;i < _tmp278;++ i){_tmpBB[i]='\000';}_tmpBB[_tmp278]=0;}0;});_tag_dyneither(_tmpBB,sizeof(char),_tmpBC);});
int _tmpB2=0;
int _tmpB3=0;
for(0;_tmpB2 < _tmpA7;++ _tmpB2){
char _tmpB4=*((const char*)_check_dyneither_subscript(s,sizeof(char),_tmpB2));
if(_tmpB4 == '\''  || Cyc_is_other_special(_tmpB4))
({struct _dyneither_ptr _tmpB5=_dyneither_ptr_plus(s2,sizeof(char),_tmpB3 ++);char _tmpB6=*((char*)_check_dyneither_subscript(_tmpB5,sizeof(char),0U));char _tmpB7='\\';if(_get_dyneither_size(_tmpB5,sizeof(char))== 1U  && (_tmpB6 == '\000'  && _tmpB7 != '\000'))_throw_arraybounds();*((char*)_tmpB5.curr)=_tmpB7;});
({struct _dyneither_ptr _tmpB8=_dyneither_ptr_plus(s2,sizeof(char),_tmpB3 ++);char _tmpB9=*((char*)_check_dyneither_subscript(_tmpB8,sizeof(char),0U));char _tmpBA=_tmpB4;if(_get_dyneither_size(_tmpB8,sizeof(char))== 1U  && (_tmpB9 == '\000'  && _tmpBA != '\000'))_throw_arraybounds();*((char*)_tmpB8.curr)=_tmpBA;});}
# 718
return(struct _dyneither_ptr)s2;};}
# 720
static struct _dyneither_ptr*Cyc_sh_escape_stringptr(struct _dyneither_ptr*sp){
return({struct _dyneither_ptr*_tmpBD=_cycalloc(sizeof(*_tmpBD));({struct _dyneither_ptr _tmp2D3=Cyc_sh_escape_string(*sp);*_tmpBD=_tmp2D3;});_tmpBD;});}
# 724
static void Cyc_process_file(struct _dyneither_ptr filename){
# 726
struct _dyneither_ptr _tmpBE=Cyc_make_base_filename(filename,Cyc_output_file);
struct _dyneither_ptr _tmpBF=({struct _dyneither_ptr _tmp2D4=(struct _dyneither_ptr)_tmpBE;Cyc_strconcat(_tmp2D4,({const char*_tmp112=".cyp";_tag_dyneither(_tmp112,sizeof(char),5U);}));});
struct _dyneither_ptr _tmpC0=({char*_tmp2D6=(char*)Cyc_specified_interface.curr;_tmp2D6 != (char*)(_tag_dyneither(0,0,0)).curr;})?Cyc_specified_interface:(struct _dyneither_ptr)({
struct _dyneither_ptr _tmp2D5=(struct _dyneither_ptr)_tmpBE;Cyc_strconcat(_tmp2D5,({const char*_tmp111=".cyci";_tag_dyneither(_tmp111,sizeof(char),6U);}));});
struct _dyneither_ptr _tmpC1=({struct _dyneither_ptr _tmp2D7=(struct _dyneither_ptr)_tmpBE;Cyc_strconcat(_tmp2D7,({const char*_tmp110=".cycio";_tag_dyneither(_tmp110,sizeof(char),7U);}));});
struct _dyneither_ptr _tmpC2=({struct _dyneither_ptr _tmp2D8=(struct _dyneither_ptr)_tmpBE;Cyc_strconcat(_tmp2D8,({const char*_tmp10F=".c";_tag_dyneither(_tmp10F,sizeof(char),3U);}));});
struct _dyneither_ptr*_tmpC3=({struct _dyneither_ptr*_tmp10E=_cycalloc(sizeof(*_tmp10E));*_tmp10E=_tmpBF;_tmp10E;});
# 734
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _tmpC6;_tmpC6.tag=0U;_tmpC6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename);({void*_tmpC4[1U];_tmpC4[0]=& _tmpC6;({struct Cyc___cycFILE*_tmp2DA=Cyc_stderr;struct _dyneither_ptr _tmp2D9=({const char*_tmpC5="Compiling %s\n";_tag_dyneither(_tmpC5,sizeof(char),14U);});Cyc_fprintf(_tmp2DA,_tmp2D9,_tag_dyneither(_tmpC4,sizeof(void*),1U));});});});{
# 737
struct Cyc___cycFILE*f0=({struct _dyneither_ptr _tmp2DC=filename;struct _dyneither_ptr _tmp2DB=({const char*_tmp10C="r";_tag_dyneither(_tmp10C,sizeof(char),2U);});Cyc_try_file_open(_tmp2DC,_tmp2DB,({const char*_tmp10D="input file";_tag_dyneither(_tmp10D,sizeof(char),11U);}));});
if(Cyc_compile_failure  || !((unsigned int)f0))
return;
Cyc_fclose(f0);{
# 744
struct _dyneither_ptr _tmpC7=({
struct Cyc_List_List*_tmp2E0=({struct Cyc_List_List*_tmp10A=_cycalloc(sizeof(*_tmp10A));(({struct _dyneither_ptr*_tmp2DF=({struct _dyneither_ptr*_tmp109=_cycalloc(sizeof(*_tmp109));({struct _dyneither_ptr _tmp2DE=(struct _dyneither_ptr)({const char*_tmp108="";_tag_dyneither(_tmp108,sizeof(char),1U);});*_tmp109=_tmp2DE;});_tmp109;});(*_tmp10A).hd=_tmp2DF;}),({
struct Cyc_List_List*_tmp2DD=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs));(*_tmp10A).tl=_tmp2DD;}));_tmp10A;});
# 745
Cyc_str_sepstr(_tmp2E0,({const char*_tmp10B=" ";_tag_dyneither(_tmp10B,sizeof(char),2U);}));});
# 754
struct Cyc_List_List*_tmpC8=Cyc_add_subdir(Cyc_Specsfile_cyclone_exec_path,Cyc_Specsfile_target_arch);
({struct Cyc_List_List*_tmp2E2=({struct Cyc_List_List*_tmp2E1=_tmpC8;Cyc_add_subdir(_tmp2E1,({const char*_tmpC9="include";_tag_dyneither(_tmpC9,sizeof(char),8U);}));});_tmpC8=_tmp2E2;});
if(Cyc_strlen((struct _dyneither_ptr)Cyc_def_inc_path)> 0)
({struct Cyc_List_List*_tmp2E4=({struct Cyc_List_List*_tmpCB=_cycalloc(sizeof(*_tmpCB));(({struct _dyneither_ptr*_tmp2E3=({struct _dyneither_ptr*_tmpCA=_cycalloc(sizeof(*_tmpCA));*_tmpCA=Cyc_def_inc_path;_tmpCA;});(*_tmpCB).hd=_tmp2E3;}),(*_tmpCB).tl=_tmpC8);_tmpCB;});_tmpC8=_tmp2E4;});{
# 759
char*_tmpCC= getenv("CYCLONE_INCLUDE_PATH");
if(_tmpCC != 0)
({struct Cyc_List_List*_tmp2E6=({struct Cyc_List_List*_tmp2E5=Cyc_split_by_char(({char*_tmpCD=_tmpCC;_tag_dyneither(_tmpCD,sizeof(char),_get_zero_arr_size_char((void*)_tmpCD,1U));}),':');((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2E5,_tmpC8);});_tmpC8=_tmp2E6;});{
# 764
struct _dyneither_ptr stdinc_string=(struct _dyneither_ptr)({
struct Cyc_List_List*_tmp2EA=({struct Cyc_List_List*_tmp106=_cycalloc(sizeof(*_tmp106));(({struct _dyneither_ptr*_tmp2E9=({struct _dyneither_ptr*_tmp105=_cycalloc(sizeof(*_tmp105));({struct _dyneither_ptr _tmp2E8=(struct _dyneither_ptr)({const char*_tmp104="";_tag_dyneither(_tmp104,sizeof(char),1U);});*_tmp105=_tmp2E8;});_tmp105;});(*_tmp106).hd=_tmp2E9;}),({
struct Cyc_List_List*_tmp2E7=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,_tmpC8);(*_tmp106).tl=_tmp2E7;}));_tmp106;});
# 765
Cyc_str_sepstr(_tmp2EA,({const char*_tmp107=" -I";_tag_dyneither(_tmp107,sizeof(char),4U);}));});
# 768
struct _dyneither_ptr ofile_string;
if(Cyc_stop_after_cpp_r){
if(Cyc_output_file != 0)
({struct _dyneither_ptr _tmp2EC=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpD0;_tmpD0.tag=0U;_tmpD0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_output_file)));({void*_tmpCE[1U];_tmpCE[0]=& _tmpD0;({struct _dyneither_ptr _tmp2EB=({const char*_tmpCF=" > %s";_tag_dyneither(_tmpCF,sizeof(char),6U);});Cyc_aprintf(_tmp2EB,_tag_dyneither(_tmpCE,sizeof(void*),1U));});});});ofile_string=_tmp2EC;});else{
# 773
({struct _dyneither_ptr _tmp2ED=({const char*_tmpD1="";_tag_dyneither(_tmpD1,sizeof(char),1U);});ofile_string=_tmp2ED;});}}else{
# 775
({struct _dyneither_ptr _tmp2F0=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpD4;_tmpD4.tag=0U;({struct _dyneither_ptr _tmp2EE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string((struct _dyneither_ptr)_tmpBF));_tmpD4.f1=_tmp2EE;});({void*_tmpD2[1U];_tmpD2[0]=& _tmpD4;({struct _dyneither_ptr _tmp2EF=({const char*_tmpD3=" > %s";_tag_dyneither(_tmpD3,sizeof(char),6U);});Cyc_aprintf(_tmp2EF,_tag_dyneither(_tmpD2,sizeof(void*),1U));});});});ofile_string=_tmp2F0;});}{
# 777
struct _dyneither_ptr fixup_string;
if(Cyc_produce_dependencies){
# 781
if(Cyc_dependencies_target == 0)
# 785
({struct _dyneither_ptr _tmp2F1=({const char*_tmpD5=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'";_tag_dyneither(_tmpD5,sizeof(char),35U);});fixup_string=_tmp2F1;});else{
# 790
({struct _dyneither_ptr _tmp2F3=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpD8;_tmpD8.tag=0U;_tmpD8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_dependencies_target)));({void*_tmpD6[1U];_tmpD6[0]=& _tmpD8;({struct _dyneither_ptr _tmp2F2=({const char*_tmpD7=" | sed 's/^.*\\.cyc\\.o:/%s:/'";_tag_dyneither(_tmpD7,sizeof(char),29U);});Cyc_aprintf(_tmp2F2,_tag_dyneither(_tmpD6,sizeof(void*),1U));});});});fixup_string=_tmp2F3;});}}else{
# 794
({struct _dyneither_ptr _tmp2F4=({const char*_tmpD9="";_tag_dyneither(_tmpD9,sizeof(char),1U);});fixup_string=_tmp2F4;});}{
# 796
struct _dyneither_ptr _tmpDA=({struct Cyc_String_pa_PrintArg_struct _tmp103;_tmp103.tag=0U;_tmp103.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ofile_string);({struct Cyc_String_pa_PrintArg_struct _tmp102;_tmp102.tag=0U;_tmp102.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)fixup_string);({struct Cyc_String_pa_PrintArg_struct _tmp101;_tmp101.tag=0U;({struct _dyneither_ptr _tmp2F5=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 799
Cyc_sh_escape_string(filename));_tmp101.f1=_tmp2F5;});({struct Cyc_String_pa_PrintArg_struct _tmp100;_tmp100.tag=0U;_tmp100.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdinc_string);({struct Cyc_String_pa_PrintArg_struct _tmpFF;_tmpFF.tag=0U;_tmpFF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpC7);({struct Cyc_String_pa_PrintArg_struct _tmpFE;_tmpFE.tag=0U;_tmpFE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cpp);({void*_tmpFC[6U];(((((((((_tmpFC[0]=& _tmpFE,_tmpFC[1]=& _tmpFF)),_tmpFC[2]=& _tmp100)),_tmpFC[3]=& _tmp101)),_tmpFC[4]=& _tmp102)),_tmpFC[5]=& _tmp103);({struct _dyneither_ptr _tmp2F6=({const char*_tmpFD="%s %s%s %s%s%s";_tag_dyneither(_tmpFD,sizeof(char),15U);});Cyc_aprintf(_tmp2F6,_tag_dyneither(_tmpFC,sizeof(void*),6U));});});});});});});});});
# 802
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _tmpDD;_tmpDD.tag=0U;_tmpDD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpDA);({void*_tmpDB[1U];_tmpDB[0]=& _tmpDD;({struct Cyc___cycFILE*_tmp2F8=Cyc_stderr;struct _dyneither_ptr _tmp2F7=({const char*_tmpDC="%s\n";_tag_dyneither(_tmpDC,sizeof(char),4U);});Cyc_fprintf(_tmp2F8,_tmp2F7,_tag_dyneither(_tmpDB,sizeof(void*),1U));});});});
if( system((const char*)_check_null(_untag_dyneither_ptr(_tmpDA,sizeof(char),1U)))!= 0){
Cyc_compile_failure=1;
({void*_tmpDE=0U;({struct Cyc___cycFILE*_tmp2FA=Cyc_stderr;struct _dyneither_ptr _tmp2F9=({const char*_tmpDF="\nError: preprocessing\n";_tag_dyneither(_tmpDF,sizeof(char),23U);});Cyc_fprintf(_tmp2FA,_tmp2F9,_tag_dyneither(_tmpDE,sizeof(void*),0U));});});
if(!Cyc_stop_after_cpp_r)Cyc_remove_file((struct _dyneither_ptr)_tmpBF);
return;}
# 809
if(Cyc_stop_after_cpp_r)return;
# 812
Cyc_Position_reset_position((struct _dyneither_ptr)_tmpBF);{
struct Cyc___cycFILE*in_file=({struct _dyneither_ptr _tmp2FC=(struct _dyneither_ptr)_tmpBF;struct _dyneither_ptr _tmp2FB=({const char*_tmpFA="r";_tag_dyneither(_tmpFA,sizeof(char),2U);});Cyc_try_file_open(_tmp2FC,_tmp2FB,({const char*_tmpFB="file";_tag_dyneither(_tmpFB,sizeof(char),5U);}));});
if(Cyc_compile_failure)return;{
# 816
struct Cyc_List_List*tds=0;
# 819
({struct Cyc_List_List*_tmp300=({struct _dyneither_ptr _tmp2FF=({const char*_tmpE0="parsing";_tag_dyneither(_tmpE0,sizeof(char),8U);});struct Cyc_List_List*_tmp2FE=tds;struct Cyc___cycFILE*_tmp2FD=(struct Cyc___cycFILE*)_check_null(in_file);((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,void(*on_fail)(struct _dyneither_ptr*),struct _dyneither_ptr*failenv))Cyc_do_stage)(_tmp2FF,_tmp2FE,Cyc_do_parse,_tmp2FD,Cyc_remove_fileptr,_tmpC3);});tds=_tmp300;});
# 821
if(Cyc_compile_failure)return;
# 823
if(Cyc_Absyn_porting_c_code){
Cyc_Port_port(tds);
return;}
# 829
({struct _dyneither_ptr _tmp302=({const char*_tmpE1="binding";_tag_dyneither(_tmpE1,sizeof(char),8U);});struct Cyc_List_List*_tmp301=tds;((int(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,int(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(struct _dyneither_ptr*),struct _dyneither_ptr*failenv))Cyc_do_stage)(_tmp302,_tmp301,Cyc_do_binding,1,Cyc_remove_fileptr,_tmpC3);});
if(Cyc_compile_failure)return;{
# 832
struct Cyc_JumpAnalysis_Jump_Anal_Result*jump_tables;
{struct Cyc_Tcenv_Tenv*_tmpE2=Cyc_Tcenv_tc_init();
if(Cyc_parseonly_r)goto PRINTC;
# 837
({struct Cyc_List_List*_tmp306=({struct _dyneither_ptr _tmp305=({const char*_tmpE3="type checking";_tag_dyneither(_tmpE3,sizeof(char),14U);});struct Cyc_List_List*_tmp304=tds;struct Cyc_Tcenv_Tenv*_tmp303=_tmpE2;((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*),struct Cyc_Tcenv_Tenv*env,void(*on_fail)(struct _dyneither_ptr*),struct _dyneither_ptr*failenv))Cyc_do_stage)(_tmp305,_tmp304,Cyc_do_typecheck,_tmp303,Cyc_remove_fileptr,_tmpC3);});tds=_tmp306;});
# 839
if(Cyc_compile_failure)return;
# 842
({struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmp309=({struct _dyneither_ptr _tmp308=({const char*_tmpE4="jump checking";_tag_dyneither(_tmpE4,sizeof(char),14U);});struct Cyc_List_List*_tmp307=tds;((struct Cyc_JumpAnalysis_Jump_Anal_Result*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_JumpAnalysis_Jump_Anal_Result*(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(struct _dyneither_ptr*),struct _dyneither_ptr*failenv))Cyc_do_stage)(_tmp308,_tmp307,Cyc_do_jumpanalysis,1,Cyc_remove_fileptr,_tmpC3);});jump_tables=_tmp309;});
# 844
if(Cyc_compile_failure)return;
# 847
if(Cyc_tc_r)goto PRINTC;
({struct Cyc_List_List*_tmp30D=({struct _dyneither_ptr _tmp30C=({const char*_tmpE5="control-flow checking";_tag_dyneither(_tmpE5,sizeof(char),22U);});struct Cyc_List_List*_tmp30B=tds;struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmp30A=jump_tables;((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*),struct Cyc_JumpAnalysis_Jump_Anal_Result*env,void(*on_fail)(struct _dyneither_ptr*),struct _dyneither_ptr*failenv))Cyc_do_stage)(_tmp30C,_tmp30B,Cyc_do_cfcheck,_tmp30A,Cyc_remove_fileptr,_tmpC3);});tds=_tmp30D;});
# 850
if(Cyc_compile_failure)return;
# 853
if(Cyc_generate_interface_r){
struct Cyc___cycFILE*inter_file=({struct _dyneither_ptr _tmp30F=({const char*_tmpE6=(const char*)_check_null(_untag_dyneither_ptr(_tmpC0,sizeof(char),1U));_tag_dyneither(_tmpE6,sizeof(char),_get_zero_arr_size_char((void*)_tmpE6,1U));});struct _dyneither_ptr _tmp30E=({const char*_tmpE7="w";_tag_dyneither(_tmpE7,sizeof(char),2U);});Cyc_try_file_open(_tmp30F,_tmp30E,({const char*_tmpE8="interface file";_tag_dyneither(_tmpE8,sizeof(char),15U);}));});
if(inter_file == 0){
Cyc_compile_failure=1;
return;}
# 859
Cyc_Absyndump_set_params(& Cyc_Absynpp_cyci_params_r);
Cyc_Absyndump_dump_interface(tds,inter_file);
Cyc_fclose(inter_file);
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);}
# 866
if(Cyc_ic_r){
struct Cyc___cycFILE*inter_file=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(_tmpC0,sizeof(char),1U)),"r");
struct Cyc___cycFILE*inter_objfile=({struct _dyneither_ptr _tmp311=(struct _dyneither_ptr)_tmpC1;struct _dyneither_ptr _tmp310=({const char*_tmpEB="w";_tag_dyneither(_tmpEB,sizeof(char),2U);});Cyc_try_file_open(_tmp311,_tmp310,({const char*_tmpEC="interface object file";_tag_dyneither(_tmpEC,sizeof(char),22U);}));});
if(inter_objfile == 0){
Cyc_compile_failure=1;
return;}
# 873
Cyc_Position_reset_position(_tmpC0);{
struct _tuple22 _tmpE9=({struct _tuple22 _tmp279;(((_tmp279.f1=_tmpE2,_tmp279.f2=inter_file)),_tmp279.f3=inter_objfile);_tmp279;});
({struct Cyc_List_List*_tmp313=({struct _dyneither_ptr _tmp312=({const char*_tmpEA="interface checking";_tag_dyneither(_tmpEA,sizeof(char),19U);});((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct _tuple22*,struct Cyc_List_List*),struct _tuple22*env,void(*on_fail)(struct _tuple22*),struct _tuple22*failenv))Cyc_do_stage)(_tmp312,tds,Cyc_do_interface,& _tmpE9,Cyc_interface_fail,& _tmpE9);});tds=_tmp313;});};}}
# 879
if(Cyc_cf_r)goto PRINTC;
# 882
({struct Cyc_List_List*_tmp317=({struct _dyneither_ptr _tmp316=({const char*_tmpED="translation to C";_tag_dyneither(_tmpED,sizeof(char),17U);});struct Cyc_List_List*_tmp315=tds;struct Cyc_Hashtable_Table*_tmp314=jump_tables->pop_tables;((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct Cyc_Hashtable_Table*,struct Cyc_List_List*),struct Cyc_Hashtable_Table*env,void(*on_fail)(struct _dyneither_ptr*),struct _dyneither_ptr*failenv))Cyc_do_stage)(_tmp316,_tmp315,Cyc_do_translate,_tmp314,Cyc_remove_fileptr,_tmpC3);});tds=_tmp317;});
# 885
({struct Cyc_List_List*_tmp319=({struct _dyneither_ptr _tmp318=({const char*_tmpEE="aggregate removal";_tag_dyneither(_tmpEE,sizeof(char),18U);});((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(int),int failenv))Cyc_do_stage)(_tmp318,tds,Cyc_do_removeaggrs,1,(void(*)(int x))Cyc_ignore,1);});tds=_tmp319;});
if(Cyc_compile_failure)return;
# 889
if(Cyc_toseqc_r)
({struct Cyc_List_List*_tmp31B=({struct _dyneither_ptr _tmp31A=({const char*_tmpEF="post-pass to L-to-R evaluation order";_tag_dyneither(_tmpEF,sizeof(char),37U);});((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(int),int failenv))Cyc_do_stage)(_tmp31A,tds,Cyc_do_toseqc,1,(void(*)(int x))Cyc_ignore,1);});tds=_tmp31B;});
# 893
Cyc_Toc_finish();
# 895
if(Cyc_compile_failure)return;
# 898
if(Cyc_Cyclone_tovc_r  || Cyc_elim_se_r)
({struct Cyc_List_List*_tmp31D=({struct _dyneither_ptr _tmp31C=({const char*_tmpF0="post-pass to VC";_tag_dyneither(_tmpF0,sizeof(char),16U);});((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(int),int failenv))Cyc_do_stage)(_tmp31C,tds,Cyc_do_tovc,1,(void(*)(int x))Cyc_ignore,1);});tds=_tmp31D;});
if(Cyc_compile_failure)return;
# 902
Cyc_Warn_flush_warnings();
Cyc_remove_file((struct _dyneither_ptr)_tmpBF);
# 906
PRINTC: {
struct Cyc___cycFILE*out_file;
if((Cyc_parseonly_r  || Cyc_tc_r) || Cyc_cf_r){
if(Cyc_output_file != 0)
({struct Cyc___cycFILE*_tmp320=({struct _dyneither_ptr _tmp31F=*((struct _dyneither_ptr*)_check_null(Cyc_output_file));struct _dyneither_ptr _tmp31E=({const char*_tmpF1="w";_tag_dyneither(_tmpF1,sizeof(char),2U);});Cyc_try_file_open(_tmp31F,_tmp31E,({const char*_tmpF2="output file";_tag_dyneither(_tmpF2,sizeof(char),12U);}));});out_file=_tmp320;});else{
# 912
out_file=Cyc_stdout;}}else{
if(Cyc_toc_r  && Cyc_output_file != 0)
({struct Cyc___cycFILE*_tmp323=({struct _dyneither_ptr _tmp322=*((struct _dyneither_ptr*)_check_null(Cyc_output_file));struct _dyneither_ptr _tmp321=({const char*_tmpF3="w";_tag_dyneither(_tmpF3,sizeof(char),2U);});Cyc_try_file_open(_tmp322,_tmp321,({const char*_tmpF4="output file";_tag_dyneither(_tmpF4,sizeof(char),12U);}));});out_file=_tmp323;});else{
# 916
({struct Cyc___cycFILE*_tmp326=({struct _dyneither_ptr _tmp325=(struct _dyneither_ptr)_tmpC2;struct _dyneither_ptr _tmp324=({const char*_tmpF5="w";_tag_dyneither(_tmpF5,sizeof(char),2U);});Cyc_try_file_open(_tmp325,_tmp324,({const char*_tmpF6="output file";_tag_dyneither(_tmpF6,sizeof(char),12U);}));});out_file=_tmp326;});}}
# 918
if(Cyc_compile_failure  || !((unsigned int)out_file))return;
# 920
if(!Cyc_noprint_r){
struct _tuple23*_tmpF7=({struct _tuple23*_tmpF9=_cycalloc(sizeof(*_tmpF9));((*_tmpF9).f1=out_file,(*_tmpF9).f2=_tmpC2);_tmpF9;});
({struct Cyc_List_List*_tmp32A=({struct _dyneither_ptr _tmp329=({const char*_tmpF8="printing";_tag_dyneither(_tmpF8,sizeof(char),9U);});struct Cyc_List_List*_tmp328=tds;struct _tuple23*_tmp327=_tmpF7;((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_List_List*),struct _tuple23*env,void(*on_fail)(struct _tuple23*),struct _tuple23*failenv))Cyc_do_stage)(_tmp329,_tmp328,Cyc_do_print,_tmp327,Cyc_print_fail,_tmpF7);});tds=_tmp32A;});}}};};};};};};};};};}static char _tmp113[8U]="<final>";
# 927
static struct _dyneither_ptr Cyc_final_str={_tmp113,_tmp113,_tmp113 + 8U};
static struct _dyneither_ptr*Cyc_final_strptr=& Cyc_final_str;
# 930
static struct Cyc_Interface_I*Cyc_read_cycio(struct _dyneither_ptr*n){
if(n == Cyc_final_strptr)
return Cyc_Interface_final();{
# 934
struct _dyneither_ptr basename;
{struct _handler_cons _tmp114;_push_handler(& _tmp114);{int _tmp116=0;if(setjmp(_tmp114.handler))_tmp116=1;if(!_tmp116){({struct _dyneither_ptr _tmp32B=(struct _dyneither_ptr)Cyc_Filename_chop_extension(*n);basename=_tmp32B;});;_pop_handler();}else{void*_tmp115=(void*)_exn_thrown;void*_tmp117=_tmp115;void*_tmp118;if(((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp117)->tag == Cyc_Core_Invalid_argument){_LL1: _LL2:
 basename=*n;goto _LL0;}else{_LL3: _tmp118=_tmp117;_LL4:(int)_rethrow(_tmp118);}_LL0:;}};}{
# 938
struct _dyneither_ptr _tmp119=({struct _dyneither_ptr _tmp32C=(struct _dyneither_ptr)basename;Cyc_strconcat(_tmp32C,({const char*_tmp11E=".cycio";_tag_dyneither(_tmp11E,sizeof(char),7U);}));});
struct Cyc___cycFILE*_tmp11A=({struct _dyneither_ptr _tmp32E=(struct _dyneither_ptr)_tmp119;struct _dyneither_ptr _tmp32D=({const char*_tmp11C="rb";_tag_dyneither(_tmp11C,sizeof(char),3U);});Cyc_try_file_open(_tmp32E,_tmp32D,({const char*_tmp11D="interface object file";_tag_dyneither(_tmp11D,sizeof(char),22U);}));});
if(_tmp11A == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
 exit(1);}
# 945
Cyc_Position_reset_position((struct _dyneither_ptr)_tmp119);{
# 947
struct Cyc_Interface_I*_tmp11B=Cyc_Interface_load(_tmp11A);
Cyc_file_close(_tmp11A);
return _tmp11B;};};};}
# 952
static int Cyc_is_cfile(struct _dyneither_ptr*n){
return*((const char*)_check_dyneither_subscript(*n,sizeof(char),0))!= '-';}
# 956
void GC_blacklist_warn_clear();struct _tuple24{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
# 966
void Cyc_print_options();
# 973
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt1_spec={3U,& Cyc_v_r};static char _tmp11F[3U]="-v";static char _tmp120[1U]="";static char _tmp121[35U]="print compilation stages verbosely";struct _tuple24 Cyc_opt1_tuple={{_tmp11F,_tmp11F,_tmp11F + 3U},0,{_tmp120,_tmp120,_tmp120 + 1U},(void*)& Cyc_opt1_spec,{_tmp121,_tmp121,_tmp121 + 35U}};struct Cyc_List_List Cyc_opt1={(void*)& Cyc_opt1_tuple,0};
# 975
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt2_spec={0U,Cyc_print_version};static char _tmp122[10U]="--version";static char _tmp123[1U]="";static char _tmp124[35U]="Print version information and exit";struct _tuple24 Cyc_opt2_tuple={{_tmp122,_tmp122,_tmp122 + 10U},0,{_tmp123,_tmp123,_tmp123 + 1U},(void*)& Cyc_opt2_spec,{_tmp124,_tmp124,_tmp124 + 35U}};struct Cyc_List_List Cyc_opt2={(void*)& Cyc_opt2_tuple,0};
# 978
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt3_spec={5U,Cyc_set_output_file};static char _tmp125[3U]="-o";static char _tmp126[8U]=" <file>";static char _tmp127[35U]="Set the output file name to <file>";struct _tuple24 Cyc_opt3_tuple={{_tmp125,_tmp125,_tmp125 + 3U},0,{_tmp126,_tmp126,_tmp126 + 8U},(void*)& Cyc_opt3_spec,{_tmp127,_tmp127,_tmp127 + 35U}};struct Cyc_List_List Cyc_opt3={(void*)& Cyc_opt3_tuple,0};
# 981
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt4_spec={1U,Cyc_add_cpparg};static char _tmp128[3U]="-D";static char _tmp129[17U]="<name>[=<value>]";static char _tmp12A[32U]="Pass definition to preprocessor";struct _tuple24 Cyc_opt4_tuple={{_tmp128,_tmp128,_tmp128 + 3U},1,{_tmp129,_tmp129,_tmp129 + 17U},(void*)& Cyc_opt4_spec,{_tmp12A,_tmp12A,_tmp12A + 32U}};struct Cyc_List_List Cyc_opt4={(void*)& Cyc_opt4_tuple,0};
# 984
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt5_spec={1U,Cyc_Specsfile_add_cyclone_exec_path};static char _tmp12B[3U]="-B";static char _tmp12C[7U]="<file>";static char _tmp12D[60U]="Add to the list of directories to search for compiler files";struct _tuple24 Cyc_opt5_tuple={{_tmp12B,_tmp12B,_tmp12B + 3U},1,{_tmp12C,_tmp12C,_tmp12C + 7U},(void*)& Cyc_opt5_spec,{_tmp12D,_tmp12D,_tmp12D + 60U}};struct Cyc_List_List Cyc_opt5={(void*)& Cyc_opt5_tuple,0};
# 987
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt6_spec={1U,Cyc_add_cpparg};static char _tmp12E[3U]="-I";static char _tmp12F[6U]="<dir>";static char _tmp130[59U]="Add to the list of directories to search for include files";struct _tuple24 Cyc_opt6_tuple={{_tmp12E,_tmp12E,_tmp12E + 3U},1,{_tmp12F,_tmp12F,_tmp12F + 6U},(void*)& Cyc_opt6_spec,{_tmp130,_tmp130,_tmp130 + 59U}};struct Cyc_List_List Cyc_opt6={(void*)& Cyc_opt6_tuple,0};
# 990
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt7_spec={1U,Cyc_add_ccarg};static char _tmp131[3U]="-L";static char _tmp132[6U]="<dir>";static char _tmp133[38U]="Add to the list of directories for -l";struct _tuple24 Cyc_opt7_tuple={{_tmp131,_tmp131,_tmp131 + 3U},1,{_tmp132,_tmp132,_tmp132 + 6U},(void*)& Cyc_opt7_spec,{_tmp133,_tmp133,_tmp133 + 38U}};struct Cyc_List_List Cyc_opt7={(void*)& Cyc_opt7_tuple,0};
# 993
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt8_spec={1U,Cyc_add_libarg};static char _tmp134[3U]="-l";static char _tmp135[10U]="<libname>";static char _tmp136[13U]="Library file";struct _tuple24 Cyc_opt8_tuple={{_tmp134,_tmp134,_tmp134 + 3U},1,{_tmp135,_tmp135,_tmp135 + 10U},(void*)& Cyc_opt8_spec,{_tmp136,_tmp136,_tmp136 + 13U}};struct Cyc_List_List Cyc_opt8={(void*)& Cyc_opt8_tuple,0};
# 996
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt9_spec={1U,Cyc_add_marg};static char _tmp137[3U]="-m";static char _tmp138[9U]="<option>";static char _tmp139[52U]="GCC specific: pass machine-dependent flag on to gcc";struct _tuple24 Cyc_opt9_tuple={{_tmp137,_tmp137,_tmp137 + 3U},1,{_tmp138,_tmp138,_tmp138 + 9U},(void*)& Cyc_opt9_spec,{_tmp139,_tmp139,_tmp139 + 52U}};struct Cyc_List_List Cyc_opt9={(void*)& Cyc_opt9_tuple,0};
# 999
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt10_spec={0U,Cyc_set_stop_after_objectfile};static char _tmp13A[3U]="-c";static char _tmp13B[1U]="";static char _tmp13C[61U]="Produce an object file instead of an executable; do not link";struct _tuple24 Cyc_opt10_tuple={{_tmp13A,_tmp13A,_tmp13A + 3U},0,{_tmp13B,_tmp13B,_tmp13B + 1U},(void*)& Cyc_opt10_spec,{_tmp13C,_tmp13C,_tmp13C + 61U}};struct Cyc_List_List Cyc_opt10={(void*)& Cyc_opt10_tuple,0};
# 1002
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt11_spec={5U,Cyc_set_inputtype};static char _tmp13D[3U]="-x";static char _tmp13E[12U]=" <language>";static char _tmp13F[49U]="Specify <language> for the following input files";struct _tuple24 Cyc_opt11_tuple={{_tmp13D,_tmp13D,_tmp13D + 3U},0,{_tmp13E,_tmp13E,_tmp13E + 12U},(void*)& Cyc_opt11_spec,{_tmp13F,_tmp13F,_tmp13F + 49U}};struct Cyc_List_List Cyc_opt11={(void*)& Cyc_opt11_tuple,0};
# 1005
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt12_spec={0U,Cyc_set_pa};static char _tmp140[4U]="-pa";static char _tmp141[1U]="";static char _tmp142[33U]="Compile for profiling with aprof";struct _tuple24 Cyc_opt12_tuple={{_tmp140,_tmp140,_tmp140 + 4U},0,{_tmp141,_tmp141,_tmp141 + 1U},(void*)& Cyc_opt12_spec,{_tmp142,_tmp142,_tmp142 + 33U}};struct Cyc_List_List Cyc_opt12={(void*)& Cyc_opt12_tuple,0};
# 1008
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt13_spec={0U,Cyc_set_stop_after_asmfile};static char _tmp143[3U]="-S";static char _tmp144[1U]="";static char _tmp145[35U]="Stop after producing assembly code";struct _tuple24 Cyc_opt13_tuple={{_tmp143,_tmp143,_tmp143 + 3U},0,{_tmp144,_tmp144,_tmp144 + 1U},(void*)& Cyc_opt13_spec,{_tmp145,_tmp145,_tmp145 + 35U}};struct Cyc_List_List Cyc_opt13={(void*)& Cyc_opt13_tuple,0};
# 1011
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt14_spec={0U,Cyc_set_produce_dependencies};static char _tmp146[3U]="-M";static char _tmp147[1U]="";static char _tmp148[21U]="Produce dependencies";struct _tuple24 Cyc_opt14_tuple={{_tmp146,_tmp146,_tmp146 + 3U},0,{_tmp147,_tmp147,_tmp147 + 1U},(void*)& Cyc_opt14_spec,{_tmp148,_tmp148,_tmp148 + 21U}};struct Cyc_List_List Cyc_opt14={(void*)& Cyc_opt14_tuple,0};
# 1014
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt15_spec={1U,Cyc_add_cpparg};static char _tmp149[4U]="-MG";static char _tmp14A[1U]="";static char _tmp14B[68U]="When producing dependencies assume that missing files are generated";struct _tuple24 Cyc_opt15_tuple={{_tmp149,_tmp149,_tmp149 + 4U},0,{_tmp14A,_tmp14A,_tmp14A + 1U},(void*)& Cyc_opt15_spec,{_tmp14B,_tmp14B,_tmp14B + 68U}};struct Cyc_List_List Cyc_opt15={(void*)& Cyc_opt15_tuple,0};
# 1018
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt16_spec={5U,Cyc_set_dependencies_target};static char _tmp14C[4U]="-MT";static char _tmp14D[10U]=" <target>";static char _tmp14E[29U]="Give target for dependencies";struct _tuple24 Cyc_opt16_tuple={{_tmp14C,_tmp14C,_tmp14C + 4U},0,{_tmp14D,_tmp14D,_tmp14D + 10U},(void*)& Cyc_opt16_spec,{_tmp14E,_tmp14E,_tmp14E + 29U}};struct Cyc_List_List Cyc_opt16={(void*)& Cyc_opt16_tuple,0};
# 1021
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt17_spec={5U,Cyc_Specsfile_set_target_arch};static char _tmp14F[3U]="-b";static char _tmp150[10U]="<machine>";static char _tmp151[19U]="Set target machine";struct _tuple24 Cyc_opt17_tuple={{_tmp14F,_tmp14F,_tmp14F + 3U},0,{_tmp150,_tmp150,_tmp150 + 10U},(void*)& Cyc_opt17_spec,{_tmp151,_tmp151,_tmp151 + 19U}};struct Cyc_List_List Cyc_opt17={(void*)& Cyc_opt17_tuple,0};
# 1024
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt18_spec={3U,& Cyc_NewControlFlow_warn_lose_unique};static char _tmp152[14U]="-Wlose-unique";static char _tmp153[1U]="";static char _tmp154[49U]="Try to warn when a unique pointer might get lost";struct _tuple24 Cyc_opt18_tuple={{_tmp152,_tmp152,_tmp152 + 14U},0,{_tmp153,_tmp153,_tmp153 + 1U},(void*)& Cyc_opt18_spec,{_tmp154,_tmp154,_tmp154 + 49U}};struct Cyc_List_List Cyc_opt18={(void*)& Cyc_opt18_tuple,0};
# 1027
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt19_spec={3U,& Cyc_Binding_warn_override};static char _tmp155[11U]="-Woverride";static char _tmp156[1U]="";static char _tmp157[58U]="Warn when a local variable overrides an existing variable";struct _tuple24 Cyc_opt19_tuple={{_tmp155,_tmp155,_tmp155 + 11U},0,{_tmp156,_tmp156,_tmp156 + 1U},(void*)& Cyc_opt19_spec,{_tmp157,_tmp157,_tmp157 + 58U}};struct Cyc_List_List Cyc_opt19={(void*)& Cyc_opt19_tuple,0};
# 1030
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt20_spec={0U,Cyc_set_all_warnings};static char _tmp158[6U]="-Wall";static char _tmp159[1U]="";static char _tmp15A[21U]="Turn on all warnings";struct _tuple24 Cyc_opt20_tuple={{_tmp158,_tmp158,_tmp158 + 6U},0,{_tmp159,_tmp159,_tmp159 + 1U},(void*)& Cyc_opt20_spec,{_tmp15A,_tmp15A,_tmp15A + 21U}};struct Cyc_List_List Cyc_opt20={(void*)& Cyc_opt20_tuple,0};
# 1033
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt21_spec={3U,& Cyc_stop_after_cpp_r};static char _tmp15B[3U]="-E";static char _tmp15C[1U]="";static char _tmp15D[25U]="Stop after preprocessing";struct _tuple24 Cyc_opt21_tuple={{_tmp15B,_tmp15B,_tmp15B + 3U},0,{_tmp15C,_tmp15C,_tmp15C + 1U},(void*)& Cyc_opt21_spec,{_tmp15D,_tmp15D,_tmp15D + 25U}};struct Cyc_List_List Cyc_opt21={(void*)& Cyc_opt21_tuple,0};
# 1036
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt22_spec={3U,& Cyc_parseonly_r};static char _tmp15E[17U]="-stopafter-parse";static char _tmp15F[1U]="";static char _tmp160[19U]="Stop after parsing";struct _tuple24 Cyc_opt22_tuple={{_tmp15E,_tmp15E,_tmp15E + 17U},0,{_tmp15F,_tmp15F,_tmp15F + 1U},(void*)& Cyc_opt22_spec,{_tmp160,_tmp160,_tmp160 + 19U}};struct Cyc_List_List Cyc_opt22={(void*)& Cyc_opt22_tuple,0};
# 1039
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt23_spec={3U,& Cyc_tc_r};static char _tmp161[14U]="-stopafter-tc";static char _tmp162[1U]="";static char _tmp163[25U]="Stop after type checking";struct _tuple24 Cyc_opt23_tuple={{_tmp161,_tmp161,_tmp161 + 14U},0,{_tmp162,_tmp162,_tmp162 + 1U},(void*)& Cyc_opt23_spec,{_tmp163,_tmp163,_tmp163 + 25U}};struct Cyc_List_List Cyc_opt23={(void*)& Cyc_opt23_tuple,0};
# 1042
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt24_spec={3U,& Cyc_noprint_r};static char _tmp164[9U]="-noprint";static char _tmp165[1U]="";static char _tmp166[42U]="Do not print output (when stopping early)";struct _tuple24 Cyc_opt24_tuple={{_tmp164,_tmp164,_tmp164 + 9U},0,{_tmp165,_tmp165,_tmp165 + 1U},(void*)& Cyc_opt24_spec,{_tmp166,_tmp166,_tmp166 + 42U}};struct Cyc_List_List Cyc_opt24={(void*)& Cyc_opt24_tuple,0};
# 1045
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt25_spec={3U,& Cyc_cf_r};static char _tmp167[14U]="-stopafter-cf";static char _tmp168[1U]="";static char _tmp169[33U]="Stop after control-flow checking";struct _tuple24 Cyc_opt25_tuple={{_tmp167,_tmp167,_tmp167 + 14U},0,{_tmp168,_tmp168,_tmp168 + 1U},(void*)& Cyc_opt25_spec,{_tmp169,_tmp169,_tmp169 + 33U}};struct Cyc_List_List Cyc_opt25={(void*)& Cyc_opt25_tuple,0};
# 1048
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt26_spec={3U,& Cyc_toc_r};static char _tmp16A[15U]="-stopafter-toc";static char _tmp16B[1U]="";static char _tmp16C[28U]="Stop after translation to C";struct _tuple24 Cyc_opt26_tuple={{_tmp16A,_tmp16A,_tmp16A + 15U},0,{_tmp16B,_tmp16B,_tmp16B + 1U},(void*)& Cyc_opt26_spec,{_tmp16C,_tmp16C,_tmp16C + 28U}};struct Cyc_List_List Cyc_opt26={(void*)& Cyc_opt26_tuple,0};
# 1051
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt27_spec={3U,& Cyc_ic_r};static char _tmp16D[4U]="-ic";static char _tmp16E[1U]="";static char _tmp16F[26U]="Activate the link-checker";struct _tuple24 Cyc_opt27_tuple={{_tmp16D,_tmp16D,_tmp16D + 4U},0,{_tmp16E,_tmp16E,_tmp16E + 1U},(void*)& Cyc_opt27_spec,{_tmp16F,_tmp16F,_tmp16F + 26U}};struct Cyc_List_List Cyc_opt27={(void*)& Cyc_opt27_tuple,0};
# 1054
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt28_spec={3U,& Cyc_pp_r};static char _tmp170[4U]="-pp";static char _tmp171[1U]="";static char _tmp172[47U]="Pretty print the C code that Cyclone generates";struct _tuple24 Cyc_opt28_tuple={{_tmp170,_tmp170,_tmp170 + 4U},0,{_tmp171,_tmp171,_tmp171 + 1U},(void*)& Cyc_opt28_spec,{_tmp172,_tmp172,_tmp172 + 47U}};struct Cyc_List_List Cyc_opt28={(void*)& Cyc_opt28_tuple,0};
# 1057
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt29_spec={4U,& Cyc_pp_r};static char _tmp173[4U]="-up";static char _tmp174[1U]="";static char _tmp175[55U]="Ugly print the C code that Cyclone generates (default)";struct _tuple24 Cyc_opt29_tuple={{_tmp173,_tmp173,_tmp173 + 4U},0,{_tmp174,_tmp174,_tmp174 + 1U},(void*)& Cyc_opt29_spec,{_tmp175,_tmp175,_tmp175 + 55U}};struct Cyc_List_List Cyc_opt29={(void*)& Cyc_opt29_tuple,0};
# 1060
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt30_spec={3U,& Cyc_elim_se_r};static char _tmp176[28U]="-elim-statement-expressions";static char _tmp177[1U]="";static char _tmp178[66U]="Avoid statement expressions in C output (implies --inline-checks)";struct _tuple24 Cyc_opt30_tuple={{_tmp176,_tmp176,_tmp176 + 28U},0,{_tmp177,_tmp177,_tmp177 + 1U},(void*)& Cyc_opt30_spec,{_tmp178,_tmp178,_tmp178 + 66U}};struct Cyc_List_List Cyc_opt30={(void*)& Cyc_opt30_tuple,0};
# 1064
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt31_spec={4U,& Cyc_elim_se_r};static char _tmp179[31U]="-no-elim-statement-expressions";static char _tmp17A[1U]="";static char _tmp17B[47U]="Do not avoid statement expressions in C output";struct _tuple24 Cyc_opt31_tuple={{_tmp179,_tmp179,_tmp179 + 31U},0,{_tmp17A,_tmp17A,_tmp17A + 1U},(void*)& Cyc_opt31_spec,{_tmp17B,_tmp17B,_tmp17B + 47U}};struct Cyc_List_List Cyc_opt31={(void*)& Cyc_opt31_tuple,0};
# 1067
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt32_spec={0U,Cyc_set_tovc};static char _tmp17C[8U]="-un-gcc";static char _tmp17D[1U]="";static char _tmp17E[57U]="Avoid gcc extensions in C output (except for attributes)";struct _tuple24 Cyc_opt32_tuple={{_tmp17C,_tmp17C,_tmp17C + 8U},0,{_tmp17D,_tmp17D,_tmp17D + 1U},(void*)& Cyc_opt32_spec,{_tmp17E,_tmp17E,_tmp17E + 57U}};struct Cyc_List_List Cyc_opt32={(void*)& Cyc_opt32_tuple,0};
# 1070
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt33_spec={5U,Cyc_set_c_compiler};static char _tmp17F[8U]="-c-comp";static char _tmp180[12U]=" <compiler>";static char _tmp181[40U]="Produce C output for the given compiler";struct _tuple24 Cyc_opt33_tuple={{_tmp17F,_tmp17F,_tmp17F + 8U},0,{_tmp180,_tmp180,_tmp180 + 12U},(void*)& Cyc_opt33_spec,{_tmp181,_tmp181,_tmp181 + 40U}};struct Cyc_List_List Cyc_opt33={(void*)& Cyc_opt33_tuple,0};
# 1073
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt34_spec={0U,Cyc_set_save_temps};static char _tmp182[12U]="-save-temps";static char _tmp183[1U]="";static char _tmp184[29U]="Don't delete temporary files";struct _tuple24 Cyc_opt34_tuple={{_tmp182,_tmp182,_tmp182 + 12U},0,{_tmp183,_tmp183,_tmp183 + 1U},(void*)& Cyc_opt34_spec,{_tmp184,_tmp184,_tmp184 + 29U}};struct Cyc_List_List Cyc_opt34={(void*)& Cyc_opt34_tuple,0};
# 1076
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt35_spec={3U,& Cyc_save_c_r};static char _tmp185[8U]="-save-c";static char _tmp186[1U]="";static char _tmp187[31U]="Don't delete temporary C files";struct _tuple24 Cyc_opt35_tuple={{_tmp185,_tmp185,_tmp185 + 8U},0,{_tmp186,_tmp186,_tmp186 + 1U},(void*)& Cyc_opt35_spec,{_tmp187,_tmp187,_tmp187 + 31U}};struct Cyc_List_List Cyc_opt35={(void*)& Cyc_opt35_tuple,0};
# 1079
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt36_spec={0U,Cyc_clear_lineno};static char _tmp188[11U]="--nolineno";static char _tmp189[1U]="";static char _tmp18A[63U]="Don't generate line numbers for debugging (automatic with -pp)";struct _tuple24 Cyc_opt36_tuple={{_tmp188,_tmp188,_tmp188 + 11U},0,{_tmp189,_tmp189,_tmp189 + 1U},(void*)& Cyc_opt36_spec,{_tmp18A,_tmp18A,_tmp18A + 63U}};struct Cyc_List_List Cyc_opt36={(void*)& Cyc_opt36_tuple,0};
# 1082
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt37_spec={0U,Cyc_set_nochecks};static char _tmp18B[11U]="--nochecks";static char _tmp18C[1U]="";static char _tmp18D[27U]="Disable bounds/null checks";struct _tuple24 Cyc_opt37_tuple={{_tmp18B,_tmp18B,_tmp18B + 11U},0,{_tmp18C,_tmp18C,_tmp18C + 1U},(void*)& Cyc_opt37_spec,{_tmp18D,_tmp18D,_tmp18D + 27U}};struct Cyc_List_List Cyc_opt37={(void*)& Cyc_opt37_tuple,0};
# 1085
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt38_spec={0U,Cyc_set_nonullchecks};static char _tmp18E[15U]="--nonullchecks";static char _tmp18F[1U]="";static char _tmp190[20U]="Disable null checks";struct _tuple24 Cyc_opt38_tuple={{_tmp18E,_tmp18E,_tmp18E + 15U},0,{_tmp18F,_tmp18F,_tmp18F + 1U},(void*)& Cyc_opt38_spec,{_tmp190,_tmp190,_tmp190 + 20U}};struct Cyc_List_List Cyc_opt38={(void*)& Cyc_opt38_tuple,0};
# 1088
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt39_spec={0U,Cyc_set_noboundschecks};static char _tmp191[17U]="--noboundschecks";static char _tmp192[1U]="";static char _tmp193[22U]="Disable bounds checks";struct _tuple24 Cyc_opt39_tuple={{_tmp191,_tmp191,_tmp191 + 17U},0,{_tmp192,_tmp192,_tmp192 + 1U},(void*)& Cyc_opt39_spec,{_tmp193,_tmp193,_tmp193 + 22U}};struct Cyc_List_List Cyc_opt39={(void*)& Cyc_opt39_tuple,0};
# 1091
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt40_spec={0U,Cyc_set_inline_functions};static char _tmp194[16U]="--inline-checks";static char _tmp195[1U]="";static char _tmp196[40U]="Inline bounds checks instead of #define";struct _tuple24 Cyc_opt40_tuple={{_tmp194,_tmp194,_tmp194 + 16U},0,{_tmp195,_tmp195,_tmp195 + 1U},(void*)& Cyc_opt40_spec,{_tmp196,_tmp196,_tmp196 + 40U}};struct Cyc_List_List Cyc_opt40={(void*)& Cyc_opt40_tuple,0};
# 1094
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt41_spec={5U,Cyc_set_cpp};static char _tmp197[9U]="-use-cpp";static char _tmp198[7U]="<path>";static char _tmp199[35U]="Indicate which preprocessor to use";struct _tuple24 Cyc_opt41_tuple={{_tmp197,_tmp197,_tmp197 + 9U},0,{_tmp198,_tmp198,_tmp198 + 7U},(void*)& Cyc_opt41_spec,{_tmp199,_tmp199,_tmp199 + 35U}};struct Cyc_List_List Cyc_opt41={(void*)& Cyc_opt41_tuple,0};
# 1097
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt42_spec={0U,Cyc_set_nocppprecomp};static char _tmp19A[16U]="-no-cpp-precomp";static char _tmp19B[1U]="";static char _tmp19C[40U]="Don't do smart preprocessing (mac only)";struct _tuple24 Cyc_opt42_tuple={{_tmp19A,_tmp19A,_tmp19A + 16U},0,{_tmp19B,_tmp19B,_tmp19B + 1U},(void*)& Cyc_opt42_spec,{_tmp19C,_tmp19C,_tmp19C + 40U}};struct Cyc_List_List Cyc_opt42={(void*)& Cyc_opt42_tuple,0};
# 1100
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt43_spec={0U,Cyc_set_nocyc};static char _tmp19D[7U]="-nocyc";static char _tmp19E[1U]="";static char _tmp19F[33U]="Don't add implicit namespace Cyc";struct _tuple24 Cyc_opt43_tuple={{_tmp19D,_tmp19D,_tmp19D + 7U},0,{_tmp19E,_tmp19E,_tmp19E + 1U},(void*)& Cyc_opt43_spec,{_tmp19F,_tmp19F,_tmp19F + 33U}};struct Cyc_List_List Cyc_opt43={(void*)& Cyc_opt43_tuple,0};
# 1103
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt44_spec={3U,& Cyc_nogc_r};static char _tmp1A0[6U]="-nogc";static char _tmp1A1[1U]="";static char _tmp1A2[36U]="Don't link in the garbage collector";struct _tuple24 Cyc_opt44_tuple={{_tmp1A0,_tmp1A0,_tmp1A0 + 6U},0,{_tmp1A1,_tmp1A1,_tmp1A1 + 1U},(void*)& Cyc_opt44_spec,{_tmp1A2,_tmp1A2,_tmp1A2 + 36U}};struct Cyc_List_List Cyc_opt44={(void*)& Cyc_opt44_tuple,0};
# 1106
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt45_spec={3U,& Cyc_noshake_r};static char _tmp1A3[16U]="-noremoveunused";static char _tmp1A4[1U]="";static char _tmp1A5[49U]="Don't remove externed variables that aren't used";struct _tuple24 Cyc_opt45_tuple={{_tmp1A3,_tmp1A3,_tmp1A3 + 16U},0,{_tmp1A4,_tmp1A4,_tmp1A4 + 1U},(void*)& Cyc_opt45_spec,{_tmp1A5,_tmp1A5,_tmp1A5 + 49U}};struct Cyc_List_List Cyc_opt45={(void*)& Cyc_opt45_tuple,0};
# 1109
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt46_spec={3U,& Cyc_noexpand_r};static char _tmp1A6[18U]="-noexpandtypedefs";static char _tmp1A7[1U]="";static char _tmp1A8[41U]="Don't expand typedefs in pretty printing";struct _tuple24 Cyc_opt46_tuple={{_tmp1A6,_tmp1A6,_tmp1A6 + 18U},0,{_tmp1A7,_tmp1A7,_tmp1A7 + 1U},(void*)& Cyc_opt46_spec,{_tmp1A8,_tmp1A8,_tmp1A8 + 41U}};struct Cyc_List_List Cyc_opt46={(void*)& Cyc_opt46_tuple,0};
# 1112
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt47_spec={3U,& Cyc_print_all_tvars_r};static char _tmp1A9[15U]="-printalltvars";static char _tmp1AA[1U]="";static char _tmp1AB[57U]="Print all type variables (even implicit default effects)";struct _tuple24 Cyc_opt47_tuple={{_tmp1A9,_tmp1A9,_tmp1A9 + 15U},0,{_tmp1AA,_tmp1AA,_tmp1AA + 1U},(void*)& Cyc_opt47_spec,{_tmp1AB,_tmp1AB,_tmp1AB + 57U}};struct Cyc_List_List Cyc_opt47={(void*)& Cyc_opt47_tuple,0};
# 1115
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt48_spec={3U,& Cyc_print_all_kinds_r};static char _tmp1AC[15U]="-printallkinds";static char _tmp1AD[1U]="";static char _tmp1AE[37U]="Always print kinds of type variables";struct _tuple24 Cyc_opt48_tuple={{_tmp1AC,_tmp1AC,_tmp1AC + 15U},0,{_tmp1AD,_tmp1AD,_tmp1AD + 1U},(void*)& Cyc_opt48_spec,{_tmp1AE,_tmp1AE,_tmp1AE + 37U}};struct Cyc_List_List Cyc_opt48={(void*)& Cyc_opt48_tuple,0};
# 1118
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt49_spec={3U,& Cyc_print_full_evars_r};static char _tmp1AF[16U]="-printfullevars";static char _tmp1B0[1U]="";static char _tmp1B1[50U]="Print full information for evars (type debugging)";struct _tuple24 Cyc_opt49_tuple={{_tmp1AF,_tmp1AF,_tmp1AF + 16U},0,{_tmp1B0,_tmp1B0,_tmp1B0 + 1U},(void*)& Cyc_opt49_spec,{_tmp1B1,_tmp1B1,_tmp1B1 + 50U}};struct Cyc_List_List Cyc_opt49={(void*)& Cyc_opt49_tuple,0};
# 1121
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt50_spec={3U,& Cyc_print_all_effects_r};static char _tmp1B2[17U]="-printalleffects";static char _tmp1B3[1U]="";static char _tmp1B4[45U]="Print effects for functions (type debugging)";struct _tuple24 Cyc_opt50_tuple={{_tmp1B2,_tmp1B2,_tmp1B2 + 17U},0,{_tmp1B3,_tmp1B3,_tmp1B3 + 1U},(void*)& Cyc_opt50_spec,{_tmp1B4,_tmp1B4,_tmp1B4 + 45U}};struct Cyc_List_List Cyc_opt50={(void*)& Cyc_opt50_tuple,0};
# 1124
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt51_spec={3U,& Cyc_nocyc_setjmp_r};static char _tmp1B5[14U]="-nocyc_setjmp";static char _tmp1B6[1U]="";static char _tmp1B7[46U]="Do not use compiler special file cyc_setjmp.h";struct _tuple24 Cyc_opt51_tuple={{_tmp1B5,_tmp1B5,_tmp1B5 + 14U},0,{_tmp1B6,_tmp1B6,_tmp1B6 + 1U},(void*)& Cyc_opt51_spec,{_tmp1B7,_tmp1B7,_tmp1B7 + 46U}};struct Cyc_List_List Cyc_opt51={(void*)& Cyc_opt51_tuple,0};
# 1127
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt52_spec={3U,& Cyc_Lex_compile_for_boot_r};static char _tmp1B8[18U]="-compile-for-boot";static char _tmp1B9[1U]="";static char _tmp1BA[71U]="Compile using the special boot library instead of the standard library";struct _tuple24 Cyc_opt52_tuple={{_tmp1B8,_tmp1B8,_tmp1B8 + 18U},0,{_tmp1B9,_tmp1B9,_tmp1B9 + 1U},(void*)& Cyc_opt52_spec,{_tmp1BA,_tmp1BA,_tmp1BA + 71U}};struct Cyc_List_List Cyc_opt52={(void*)& Cyc_opt52_tuple,0};
# 1130
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt53_spec={5U,Cyc_set_cyc_include};static char _tmp1BB[4U]="-CI";static char _tmp1BC[8U]=" <file>";static char _tmp1BD[31U]="Set cyc_include.h to be <file>";struct _tuple24 Cyc_opt53_tuple={{_tmp1BB,_tmp1BB,_tmp1BB + 4U},0,{_tmp1BC,_tmp1BC,_tmp1BC + 8U},(void*)& Cyc_opt53_spec,{_tmp1BD,_tmp1BD,_tmp1BD + 31U}};struct Cyc_List_List Cyc_opt53={(void*)& Cyc_opt53_tuple,0};
# 1133
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt54_spec={3U,& Cyc_Toc_warn_bounds_checks};static char _tmp1BE[18U]="-warnboundschecks";static char _tmp1BF[1U]="";static char _tmp1C0[44U]="Warn when bounds checks can't be eliminated";struct _tuple24 Cyc_opt54_tuple={{_tmp1BE,_tmp1BE,_tmp1BE + 18U},0,{_tmp1BF,_tmp1BF,_tmp1BF + 1U},(void*)& Cyc_opt54_spec,{_tmp1C0,_tmp1C0,_tmp1C0 + 44U}};struct Cyc_List_List Cyc_opt54={(void*)& Cyc_opt54_tuple,0};
# 1136
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt55_spec={3U,& Cyc_Toc_warn_all_null_deref};static char _tmp1C1[16U]="-warnnullchecks";static char _tmp1C2[1U]="";static char _tmp1C3[45U]="Warn when any null check can't be eliminated";struct _tuple24 Cyc_opt55_tuple={{_tmp1C1,_tmp1C1,_tmp1C1 + 16U},0,{_tmp1C2,_tmp1C2,_tmp1C2 + 1U},(void*)& Cyc_opt55_spec,{_tmp1C3,_tmp1C3,_tmp1C3 + 45U}};struct Cyc_List_List Cyc_opt55={(void*)& Cyc_opt55_tuple,0};
# 1139
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt56_spec={3U,& Cyc_Tcutil_warn_alias_coerce};static char _tmp1C4[19U]="-warnaliascoercion";static char _tmp1C5[1U]="";static char _tmp1C6[41U]="Warn when any alias coercion is inserted";struct _tuple24 Cyc_opt56_tuple={{_tmp1C4,_tmp1C4,_tmp1C4 + 19U},0,{_tmp1C5,_tmp1C5,_tmp1C5 + 1U},(void*)& Cyc_opt56_spec,{_tmp1C6,_tmp1C6,_tmp1C6 + 41U}};struct Cyc_List_List Cyc_opt56={(void*)& Cyc_opt56_tuple,0};
# 1142
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt57_spec={3U,& Cyc_Tcutil_warn_region_coerce};static char _tmp1C7[18U]="-warnregioncoerce";static char _tmp1C8[1U]="";static char _tmp1C9[42U]="Warn when any region coercion is inserted";struct _tuple24 Cyc_opt57_tuple={{_tmp1C7,_tmp1C7,_tmp1C7 + 18U},0,{_tmp1C8,_tmp1C8,_tmp1C8 + 1U},(void*)& Cyc_opt57_spec,{_tmp1C9,_tmp1C9,_tmp1C9 + 42U}};struct Cyc_List_List Cyc_opt57={(void*)& Cyc_opt57_tuple,0};
# 1145
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt58_spec={3U,& Cyc_Parse_no_register};static char _tmp1CA[12U]="-noregister";static char _tmp1CB[1U]="";static char _tmp1CC[39U]="Treat register storage class as public";struct _tuple24 Cyc_opt58_tuple={{_tmp1CA,_tmp1CA,_tmp1CA + 12U},0,{_tmp1CB,_tmp1CB,_tmp1CB + 1U},(void*)& Cyc_opt58_spec,{_tmp1CC,_tmp1CC,_tmp1CC + 39U}};struct Cyc_List_List Cyc_opt58={(void*)& Cyc_opt58_tuple,0};
# 1148
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt59_spec={4U,& Cyc_Position_use_gcc_style_location};static char _tmp1CD[18U]="-detailedlocation";static char _tmp1CE[1U]="";static char _tmp1CF[58U]="Try to give more detailed location information for errors";struct _tuple24 Cyc_opt59_tuple={{_tmp1CD,_tmp1CD,_tmp1CD + 18U},0,{_tmp1CE,_tmp1CE,_tmp1CE + 1U},(void*)& Cyc_opt59_spec,{_tmp1CF,_tmp1CF,_tmp1CF + 58U}};struct Cyc_List_List Cyc_opt59={(void*)& Cyc_opt59_tuple,0};
# 1151
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt60_spec={0U,Cyc_set_port_c_code};static char _tmp1D0[6U]="-port";static char _tmp1D1[1U]="";static char _tmp1D2[38U]="Suggest how to port C code to Cyclone";struct _tuple24 Cyc_opt60_tuple={{_tmp1D0,_tmp1D0,_tmp1D0 + 6U},0,{_tmp1D1,_tmp1D1,_tmp1D1 + 1U},(void*)& Cyc_opt60_spec,{_tmp1D2,_tmp1D2,_tmp1D2 + 38U}};struct Cyc_List_List Cyc_opt60={(void*)& Cyc_opt60_tuple,0};
# 1154
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt61_spec={3U,& Cyc_Absyn_no_regions};static char _tmp1D3[11U]="-noregions";static char _tmp1D4[1U]="";static char _tmp1D5[39U]="Generate code that doesn't use regions";struct _tuple24 Cyc_opt61_tuple={{_tmp1D3,_tmp1D3,_tmp1D3 + 11U},0,{_tmp1D4,_tmp1D4,_tmp1D4 + 1U},(void*)& Cyc_opt61_spec,{_tmp1D5,_tmp1D5,_tmp1D5 + 39U}};struct Cyc_List_List Cyc_opt61={(void*)& Cyc_opt61_tuple,0};
# 1157
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt62_spec={5U,Cyc_add_iprefix};static char _tmp1D6[9U]="-iprefix";static char _tmp1D7[9U]="<prefix>";static char _tmp1D8[67U]="Specify <prefix> as the prefix for subsequent -iwithprefix options";struct _tuple24 Cyc_opt62_tuple={{_tmp1D6,_tmp1D6,_tmp1D6 + 9U},0,{_tmp1D7,_tmp1D7,_tmp1D7 + 9U},(void*)& Cyc_opt62_spec,{_tmp1D8,_tmp1D8,_tmp1D8 + 67U}};struct Cyc_List_List Cyc_opt62={(void*)& Cyc_opt62_tuple,0};
# 1160
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt63_spec={5U,Cyc_add_iwithprefix};static char _tmp1D9[13U]="-iwithprefix";static char _tmp1DA[6U]="<dir>";static char _tmp1DB[47U]="Add <prefix>/<dir> to the second include path.";struct _tuple24 Cyc_opt63_tuple={{_tmp1D9,_tmp1D9,_tmp1D9 + 13U},0,{_tmp1DA,_tmp1DA,_tmp1DA + 6U},(void*)& Cyc_opt63_spec,{_tmp1DB,_tmp1DB,_tmp1DB + 47U}};struct Cyc_List_List Cyc_opt63={(void*)& Cyc_opt63_tuple,0};
# 1163
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt64_spec={5U,Cyc_add_iwithprefixbefore};static char _tmp1DC[19U]="-iwithprefixbefore";static char _tmp1DD[6U]="<dir>";static char _tmp1DE[45U]="Add <prefix>/<dir> to the main include path.";struct _tuple24 Cyc_opt64_tuple={{_tmp1DC,_tmp1DC,_tmp1DC + 19U},0,{_tmp1DD,_tmp1DD,_tmp1DD + 6U},(void*)& Cyc_opt64_spec,{_tmp1DE,_tmp1DE,_tmp1DE + 45U}};struct Cyc_List_List Cyc_opt64={(void*)& Cyc_opt64_tuple,0};
# 1166
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt65_spec={5U,Cyc_add_isystem};static char _tmp1DF[9U]="-isystem";static char _tmp1E0[6U]="<dir>";static char _tmp1E1[90U]="Add <dir> to the beginning of the second include path and treat it as a\nsystem directory.";struct _tuple24 Cyc_opt65_tuple={{_tmp1DF,_tmp1DF,_tmp1DF + 9U},0,{_tmp1E0,_tmp1E0,_tmp1E0 + 6U},(void*)& Cyc_opt65_spec,{_tmp1E1,_tmp1E1,_tmp1E1 + 90U}};struct Cyc_List_List Cyc_opt65={(void*)& Cyc_opt65_tuple,0};
# 1169
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt66_spec={5U,Cyc_add_idirafter};static char _tmp1E2[11U]="-idirafter";static char _tmp1E3[6U]="<dir>";static char _tmp1E4[46U]="Add the directory to the second include path.";struct _tuple24 Cyc_opt66_tuple={{_tmp1E2,_tmp1E2,_tmp1E2 + 11U},0,{_tmp1E3,_tmp1E3,_tmp1E3 + 6U},(void*)& Cyc_opt66_spec,{_tmp1E4,_tmp1E4,_tmp1E4 + 46U}};struct Cyc_List_List Cyc_opt66={(void*)& Cyc_opt66_tuple,0};
# 1172
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt67_spec={3U,& Cyc_generate_interface_r};static char _tmp1E5[15U]="--geninterface";static char _tmp1E6[1U]="";static char _tmp1E7[25U]="Generate interface files";struct _tuple24 Cyc_opt67_tuple={{_tmp1E5,_tmp1E5,_tmp1E5 + 15U},0,{_tmp1E6,_tmp1E6,_tmp1E6 + 1U},(void*)& Cyc_opt67_spec,{_tmp1E7,_tmp1E7,_tmp1E7 + 25U}};struct Cyc_List_List Cyc_opt67={(void*)& Cyc_opt67_tuple,0};
# 1175
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt68_spec={5U,Cyc_set_specified_interface};static char _tmp1E8[12U]="--interface";static char _tmp1E9[8U]=" <file>";static char _tmp1EA[37U]="Set the interface file to be <file>.";struct _tuple24 Cyc_opt68_tuple={{_tmp1E8,_tmp1E8,_tmp1E8 + 12U},0,{_tmp1E9,_tmp1E9,_tmp1E9 + 8U},(void*)& Cyc_opt68_spec,{_tmp1EA,_tmp1EA,_tmp1EA + 37U}};struct Cyc_List_List Cyc_opt68={(void*)& Cyc_opt68_tuple,0};
# 1178
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt69_spec={0U,Cyc_set_many_errors};static char _tmp1EB[13U]="--manyerrors";static char _tmp1EC[1U]="";static char _tmp1ED[36U]="don't stop after only a few errors.";struct _tuple24 Cyc_opt69_tuple={{_tmp1EB,_tmp1EB,_tmp1EB + 13U},0,{_tmp1EC,_tmp1EC,_tmp1EC + 1U},(void*)& Cyc_opt69_spec,{_tmp1ED,_tmp1ED,_tmp1ED + 36U}};struct Cyc_List_List Cyc_opt69={(void*)& Cyc_opt69_tuple,0};
# 1181
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt70_spec={3U,& Cyc_ParseErrors_print_state_and_token};static char _tmp1EE[13U]="--parsestate";static char _tmp1EF[1U]="";static char _tmp1F0[50U]="print the parse state and token on a syntax error";struct _tuple24 Cyc_opt70_tuple={{_tmp1EE,_tmp1EE,_tmp1EE + 13U},0,{_tmp1EF,_tmp1EF,_tmp1EF + 1U},(void*)& Cyc_opt70_spec,{_tmp1F0,_tmp1F0,_tmp1F0 + 50U}};struct Cyc_List_List Cyc_opt70={(void*)& Cyc_opt70_tuple,0};
# 1184
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt71_spec={0U,Cyc_noassume_gcc_flag};static char _tmp1F1[22U]="-known-gcc-flags-only";static char _tmp1F2[1U]="";static char _tmp1F3[57U]="do not assume that unknown flags should be passed to gcc";struct _tuple24 Cyc_opt71_tuple={{_tmp1F1,_tmp1F1,_tmp1F1 + 22U},0,{_tmp1F2,_tmp1F2,_tmp1F2 + 1U},(void*)& Cyc_opt71_spec,{_tmp1F3,_tmp1F3,_tmp1F3 + 57U}};struct Cyc_List_List Cyc_opt71={(void*)& Cyc_opt71_tuple,0};
# 1187
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt72_spec={0U,Cyc_print_options};static char _tmp1F4[6U]="-help";static char _tmp1F5[1U]="";static char _tmp1F6[32U]="print out the available options";struct _tuple24 Cyc_opt72_tuple={{_tmp1F4,_tmp1F4,_tmp1F4 + 6U},0,{_tmp1F5,_tmp1F5,_tmp1F5 + 1U},(void*)& Cyc_opt72_spec,{_tmp1F6,_tmp1F6,_tmp1F6 + 32U}};struct Cyc_List_List Cyc_opt72={(void*)& Cyc_opt72_tuple,0};
# 1190
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt73_spec={0U,Cyc_print_options};static char _tmp1F7[7U]="-usage";static char _tmp1F8[1U]="";static char _tmp1F9[32U]="print out the available options";struct _tuple24 Cyc_opt73_tuple={{_tmp1F7,_tmp1F7,_tmp1F7 + 7U},0,{_tmp1F8,_tmp1F8,_tmp1F8 + 1U},(void*)& Cyc_opt73_spec,{_tmp1F9,_tmp1F9,_tmp1F9 + 32U}};struct Cyc_List_List Cyc_opt73={(void*)& Cyc_opt73_tuple,0};
# 1193
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt74_spec={0U,Cyc_set_notoseqc};static char _tmp1FA[10U]="-no-seq-c";static char _tmp1FB[1U]="";static char _tmp1FC[71U]="Do not force left-to-right evaluation order of generated code (unsafe)";struct _tuple24 Cyc_opt74_tuple={{_tmp1FA,_tmp1FA,_tmp1FA + 10U},0,{_tmp1FB,_tmp1FB,_tmp1FB + 1U},(void*)& Cyc_opt74_spec,{_tmp1FC,_tmp1FC,_tmp1FC + 71U}};struct Cyc_List_List Cyc_opt74={(void*)& Cyc_opt74_tuple,0};
# 1197
struct Cyc_List_List*Cyc_global_options[74U]={& Cyc_opt1,& Cyc_opt2,& Cyc_opt3,& Cyc_opt4,& Cyc_opt5,& Cyc_opt6,& Cyc_opt7,& Cyc_opt8,& Cyc_opt9,& Cyc_opt10,& Cyc_opt11,& Cyc_opt12,& Cyc_opt13,& Cyc_opt14,& Cyc_opt15,& Cyc_opt16,& Cyc_opt17,& Cyc_opt18,& Cyc_opt19,& Cyc_opt20,& Cyc_opt21,& Cyc_opt22,& Cyc_opt23,& Cyc_opt24,& Cyc_opt25,& Cyc_opt26,& Cyc_opt27,& Cyc_opt28,& Cyc_opt29,& Cyc_opt30,& Cyc_opt31,& Cyc_opt32,& Cyc_opt74,& Cyc_opt33,& Cyc_opt34,& Cyc_opt35,& Cyc_opt36,& Cyc_opt37,& Cyc_opt38,& Cyc_opt39,& Cyc_opt40,& Cyc_opt41,& Cyc_opt42,& Cyc_opt43,& Cyc_opt44,& Cyc_opt45,& Cyc_opt46,& Cyc_opt47,& Cyc_opt48,& Cyc_opt49,& Cyc_opt50,& Cyc_opt51,& Cyc_opt52,& Cyc_opt53,& Cyc_opt54,& Cyc_opt55,& Cyc_opt56,& Cyc_opt57,& Cyc_opt58,& Cyc_opt59,& Cyc_opt60,& Cyc_opt61,& Cyc_opt62,& Cyc_opt63,& Cyc_opt64,& Cyc_opt65,& Cyc_opt66,& Cyc_opt67,& Cyc_opt68,& Cyc_opt69,& Cyc_opt70,& Cyc_opt71,& Cyc_opt72,& Cyc_opt73};
# 1208
void Cyc_print_options(){
({struct Cyc_List_List*_tmp32F=Cyc_global_options[0];Cyc_Arg_usage(_tmp32F,({const char*_tmp1FD="<program.cyc>";_tag_dyneither(_tmp1FD,sizeof(char),14U);}));});}
# 1213
int Cyc_main(int argc,struct _dyneither_ptr argv){
# 1218
 GC_blacklist_warn_clear();{
# 1221
struct _dyneither_ptr optstring=({const char*_tmp276="Options:";_tag_dyneither(_tmp276,sizeof(char),9U);});
# 1223
{int i=1;for(0;i < 74U;++ i){
(Cyc_global_options[_check_known_subscript_notnull(74U,i - 1)])->tl=Cyc_global_options[i];}}{
# 1226
struct Cyc_List_List*options=Cyc_global_options[0];
# 1228
struct _dyneither_ptr _tmp1FE=Cyc_Specsfile_parse_b(options,Cyc_add_other,Cyc_add_other_flag,optstring,argv);
# 1234
struct _dyneither_ptr _tmp1FF=Cyc_find_in_arch_path(({const char*_tmp275="cycspecs";_tag_dyneither(_tmp275,sizeof(char),9U);}));
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _tmp202;_tmp202.tag=0U;_tmp202.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1FF);({void*_tmp200[1U];_tmp200[0]=& _tmp202;({struct Cyc___cycFILE*_tmp331=Cyc_stderr;struct _dyneither_ptr _tmp330=({const char*_tmp201="Reading from specs file %s\n";_tag_dyneither(_tmp201,sizeof(char),28U);});Cyc_fprintf(_tmp331,_tmp330,_tag_dyneither(_tmp200,sizeof(void*),1U));});});});{
struct Cyc_List_List*_tmp203=Cyc_Specsfile_read_specs(_tmp1FF);
# 1238
struct _dyneither_ptr _tmp204=Cyc_Specsfile_split_specs(({struct Cyc_List_List*_tmp332=_tmp203;Cyc_Specsfile_get_spec(_tmp332,({const char*_tmp274="cyclone";_tag_dyneither(_tmp274,sizeof(char),8U);}));}));
if(({struct _dyneither_ptr*_tmp333=(struct _dyneither_ptr*)_tmp204.curr;_tmp333 != (struct _dyneither_ptr*)(_tag_dyneither(0,0,0)).curr;})){
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,Cyc_add_other_flag,optstring,_tmp204);}{
# 1243
struct _dyneither_ptr _tmp205=({struct Cyc_List_List*_tmp334=_tmp203;Cyc_Specsfile_get_spec(_tmp334,({const char*_tmp273="cyclone_target_cflags";_tag_dyneither(_tmp273,sizeof(char),22U);}));});
struct _dyneither_ptr _tmp206=({struct Cyc_List_List*_tmp335=_tmp203;Cyc_Specsfile_get_spec(_tmp335,({const char*_tmp272="cyclone_cc";_tag_dyneither(_tmp272,sizeof(char),11U);}));});
if(!((unsigned int)_tmp206.curr))({struct _dyneither_ptr _tmp336=({const char*_tmp207="gcc";_tag_dyneither(_tmp207,sizeof(char),4U);});_tmp206=_tmp336;});
({struct _dyneither_ptr _tmp338=({struct Cyc_List_List*_tmp337=_tmp203;Cyc_Specsfile_get_spec(_tmp337,({const char*_tmp208="cyclone_inc_path";_tag_dyneither(_tmp208,sizeof(char),17U);}));});Cyc_def_inc_path=_tmp338;});
# 1249
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,Cyc_add_other_flag,optstring,_tmp1FE);
if(({struct _dyneither_ptr _tmp339=(struct _dyneither_ptr)Cyc_cpp;Cyc_strcmp(_tmp339,({const char*_tmp209="";_tag_dyneither(_tmp209,sizeof(char),1U);}));})== 0){
# 1267 "cyclone.cyc"
const char*_tmp20A=Cyc_produce_dependencies?"":" -E";
Cyc_set_cpp((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp211;_tmp211.tag=0U;_tmp211.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1FF);({struct Cyc_String_pa_PrintArg_struct _tmp210;_tmp210.tag=0U;({struct _dyneither_ptr _tmp33A=(struct _dyneither_ptr)({const char*_tmp212=_tmp20A;_tag_dyneither(_tmp212,sizeof(char),_get_zero_arr_size_char((void*)_tmp212,1U));});_tmp210.f1=_tmp33A;});({struct Cyc_String_pa_PrintArg_struct _tmp20F;_tmp20F.tag=0U;_tmp20F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp205);({struct Cyc_String_pa_PrintArg_struct _tmp20E;_tmp20E.tag=0U;_tmp20E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Specsfile_btarget);({struct Cyc_String_pa_PrintArg_struct _tmp20D;_tmp20D.tag=0U;_tmp20D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp206);({void*_tmp20B[5U];(((((((_tmp20B[0]=& _tmp20D,_tmp20B[1]=& _tmp20E)),_tmp20B[2]=& _tmp20F)),_tmp20B[3]=& _tmp210)),_tmp20B[4]=& _tmp211);({struct _dyneither_ptr _tmp33B=({const char*_tmp20C="%s %s %s -w -x c%s -specs %s";_tag_dyneither(_tmp20C,sizeof(char),29U);});Cyc_aprintf(_tmp33B,_tag_dyneither(_tmp20B,sizeof(void*),5U));});});});});});});}));}
# 1272
if(Cyc_cyclone_files == 0  && Cyc_ccargs == 0){
({void*_tmp213=0U;({struct Cyc___cycFILE*_tmp33D=Cyc_stderr;struct _dyneither_ptr _tmp33C=({const char*_tmp214="missing file\n";_tag_dyneither(_tmp214,sizeof(char),14U);});Cyc_fprintf(_tmp33D,_tmp33C,_tag_dyneither(_tmp213,sizeof(void*),0U));});});
 exit(1);}
# 1281
if(!Cyc_stop_after_cpp_r  && !Cyc_nocyc_setjmp_r)
({struct _dyneither_ptr _tmp33E=Cyc_find_in_arch_path(({const char*_tmp215="cyc_setjmp.h";_tag_dyneither(_tmp215,sizeof(char),13U);}));Cyc_cyc_setjmp=_tmp33E;});
if(!Cyc_stop_after_cpp_r  && Cyc_strlen((struct _dyneither_ptr)Cyc_cyc_include)== 0)
({struct _dyneither_ptr _tmp33F=Cyc_find_in_exec_path(({const char*_tmp216="cyc_include.h";_tag_dyneither(_tmp216,sizeof(char),14U);}));Cyc_cyc_include=_tmp33F;});
# 1287
{struct Cyc_List_List*_tmp217=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmp217 != 0;_tmp217=_tmp217->tl){
Cyc_process_file(*((struct _dyneither_ptr*)_tmp217->hd));
if(Cyc_compile_failure)return 1;}}
# 1292
if(((Cyc_stop_after_cpp_r  || Cyc_parseonly_r) || Cyc_tc_r) || Cyc_toc_r)return 0;
# 1298
if(Cyc_ccargs == 0)return 0;
# 1301
if((unsigned int)Cyc_Specsfile_target_arch.curr  && (unsigned int)Cyc_Specsfile_cyclone_exec_path)
Cyc_add_ccarg((struct _dyneither_ptr)({struct Cyc_List_List*_tmp343=({struct Cyc_List_List*_tmp21A=_cycalloc(sizeof(*_tmp21A));(({struct _dyneither_ptr*_tmp342=({struct _dyneither_ptr*_tmp219=_cycalloc(sizeof(*_tmp219));({struct _dyneither_ptr _tmp341=({const char*_tmp218="";_tag_dyneither(_tmp218,sizeof(char),1U);});*_tmp219=_tmp341;});_tmp219;});(*_tmp21A).hd=_tmp342;}),({
struct Cyc_List_List*_tmp340=Cyc_add_subdir(Cyc_Specsfile_cyclone_exec_path,Cyc_Specsfile_target_arch);(*_tmp21A).tl=_tmp340;}));_tmp21A;});
# 1302
Cyc_str_sepstr(_tmp343,({const char*_tmp21B=" -L";_tag_dyneither(_tmp21B,sizeof(char),4U);}));}));
# 1305
Cyc_add_ccarg((struct _dyneither_ptr)({struct _dyneither_ptr _tmp344=({const char*_tmp21C="-L";_tag_dyneither(_tmp21C,sizeof(char),3U);});Cyc_strconcat(_tmp344,(struct _dyneither_ptr)Cyc_Specsfile_def_lib_path);}));
# 1307
({struct Cyc_List_List*_tmp345=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);Cyc_ccargs=_tmp345;});{
struct _dyneither_ptr _tmp21D=({struct Cyc_List_List*_tmp346=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_ccargs);Cyc_str_sepstr(_tmp346,({const char*_tmp271=" ";_tag_dyneither(_tmp271,sizeof(char),2U);}));});
({struct Cyc_List_List*_tmp347=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_libargs);Cyc_libargs=_tmp347;});{
struct _dyneither_ptr _tmp21E=({struct Cyc_List_List*_tmp34B=({struct Cyc_List_List*_tmp26F=_cycalloc(sizeof(*_tmp26F));(({struct _dyneither_ptr*_tmp34A=({struct _dyneither_ptr*_tmp26E=_cycalloc(sizeof(*_tmp26E));({struct _dyneither_ptr _tmp349=({const char*_tmp26D="";_tag_dyneither(_tmp26D,sizeof(char),1U);});*_tmp26E=_tmp349;});_tmp26E;});(*_tmp26F).hd=_tmp34A;}),({
struct Cyc_List_List*_tmp348=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_libargs);(*_tmp26F).tl=_tmp348;}));_tmp26F;});
# 1310
Cyc_str_sepstr(_tmp34B,({const char*_tmp270=" ";_tag_dyneither(_tmp270,sizeof(char),2U);}));});
# 1313
struct Cyc_List_List*stdlib;
struct _dyneither_ptr stdlib_string;
int _tmp21F=
((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r) || 
# 1318
Cyc_output_file != 0  && ({struct _dyneither_ptr _tmp34D=*((struct _dyneither_ptr*)_check_null(Cyc_output_file));Cyc_Filename_check_suffix(_tmp34D,({const char*_tmp26B=".a";_tag_dyneither(_tmp26B,sizeof(char),3U);}));})) || 
Cyc_output_file != 0  && ({struct _dyneither_ptr _tmp34C=*((struct _dyneither_ptr*)_check_null(Cyc_output_file));Cyc_Filename_check_suffix(_tmp34C,({const char*_tmp26C=".lib";_tag_dyneither(_tmp26C,sizeof(char),5U);}));});
if(_tmp21F){
stdlib=0;
({struct _dyneither_ptr _tmp34E=(struct _dyneither_ptr)({const char*_tmp220="";_tag_dyneither(_tmp220,sizeof(char),1U);});stdlib_string=_tmp34E;});}else{
# 1325
struct _dyneither_ptr libcyc_flag;
struct _dyneither_ptr nogc_filename;
struct _dyneither_ptr runtime_filename;
struct _dyneither_ptr gc_filename;
if(Cyc_pa_r){
({struct _dyneither_ptr _tmp34F=({const char*_tmp221="-lcyc_a";_tag_dyneither(_tmp221,sizeof(char),8U);});libcyc_flag=_tmp34F;});
({struct _dyneither_ptr _tmp350=({const char*_tmp222="nogc_a.a";_tag_dyneither(_tmp222,sizeof(char),9U);});nogc_filename=_tmp350;});
({struct _dyneither_ptr _tmp351=({const char*_tmp223="runtime_cyc_a.a";_tag_dyneither(_tmp223,sizeof(char),16U);});runtime_filename=_tmp351;});}else{
if(Cyc_nocheck_r){
({struct _dyneither_ptr _tmp352=({const char*_tmp224="-lcyc_nocheck";_tag_dyneither(_tmp224,sizeof(char),14U);});libcyc_flag=_tmp352;});
({struct _dyneither_ptr _tmp353=({const char*_tmp225="nogc.a";_tag_dyneither(_tmp225,sizeof(char),7U);});nogc_filename=_tmp353;});
({struct _dyneither_ptr _tmp354=({const char*_tmp226="runtime_cyc.a";_tag_dyneither(_tmp226,sizeof(char),14U);});runtime_filename=_tmp354;});}else{
if(Cyc_pg_r){
({struct _dyneither_ptr _tmp355=({const char*_tmp227="-lcyc_pg";_tag_dyneither(_tmp227,sizeof(char),9U);});libcyc_flag=_tmp355;});
({struct _dyneither_ptr _tmp356=({const char*_tmp228="runtime_cyc_pg.a";_tag_dyneither(_tmp228,sizeof(char),17U);});runtime_filename=_tmp356;});
({struct _dyneither_ptr _tmp357=({const char*_tmp229="nogc_pg.a";_tag_dyneither(_tmp229,sizeof(char),10U);});nogc_filename=_tmp357;});}else{
if(Cyc_Lex_compile_for_boot_r){
# 1343
({struct _dyneither_ptr _tmp358=({const char*_tmp22A="-lcycboot";_tag_dyneither(_tmp22A,sizeof(char),10U);});libcyc_flag=_tmp358;});
({struct _dyneither_ptr _tmp359=({const char*_tmp22B="nogc.a";_tag_dyneither(_tmp22B,sizeof(char),7U);});nogc_filename=_tmp359;});
({struct _dyneither_ptr _tmp35A=({const char*_tmp22C="runtime_cyc.a";_tag_dyneither(_tmp22C,sizeof(char),14U);});runtime_filename=_tmp35A;});}else{
# 1347
({struct _dyneither_ptr _tmp35B=({const char*_tmp22D="-lcyc";_tag_dyneither(_tmp22D,sizeof(char),6U);});libcyc_flag=_tmp35B;});
({struct _dyneither_ptr _tmp35C=({const char*_tmp22E="nogc.a";_tag_dyneither(_tmp22E,sizeof(char),7U);});nogc_filename=_tmp35C;});
({struct _dyneither_ptr _tmp35D=({const char*_tmp22F="runtime_cyc.a";_tag_dyneither(_tmp22F,sizeof(char),14U);});runtime_filename=_tmp35D;});}}}}
# 1351
({struct _dyneither_ptr _tmp35E=({const char*_tmp230="gc.a";_tag_dyneither(_tmp230,sizeof(char),5U);});gc_filename=_tmp35E;});{
# 1353
struct _dyneither_ptr _tmp231=Cyc_nogc_r?Cyc_find_in_arch_path(nogc_filename): Cyc_find_in_arch_path(gc_filename);
struct _dyneither_ptr _tmp232=Cyc_find_in_arch_path(runtime_filename);
# 1358
stdlib=0;
({struct _dyneither_ptr _tmp360=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp237;_tmp237.tag=0U;_tmp237.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp231);({struct Cyc_String_pa_PrintArg_struct _tmp236;_tmp236.tag=0U;_tmp236.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)libcyc_flag);({struct Cyc_String_pa_PrintArg_struct _tmp235;_tmp235.tag=0U;_tmp235.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp232);({void*_tmp233[3U];(((_tmp233[0]=& _tmp235,_tmp233[1]=& _tmp236)),_tmp233[2]=& _tmp237);({struct _dyneither_ptr _tmp35F=({const char*_tmp234=" %s %s %s";_tag_dyneither(_tmp234,sizeof(char),10U);});Cyc_aprintf(_tmp35F,_tag_dyneither(_tmp233,sizeof(void*),3U));});});});});});stdlib_string=_tmp360;});};}
# 1362
if(Cyc_ic_r){struct _handler_cons _tmp238;_push_handler(& _tmp238);{int _tmp23A=0;if(setjmp(_tmp238.handler))_tmp23A=1;if(!_tmp23A){
({struct Cyc_List_List*_tmp361=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_ccargs);Cyc_ccargs=_tmp361;});
({struct Cyc_List_List*_tmp362=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);Cyc_libargs=_tmp362;});{
struct Cyc_List_List*_tmp23B=({struct Cyc_List_List*_tmp363=stdlib;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp363,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));});
if(!_tmp21F)
({struct Cyc_List_List*_tmp364=({struct Cyc_List_List*_tmp23C=_cycalloc(sizeof(*_tmp23C));((*_tmp23C).hd=Cyc_final_strptr,(*_tmp23C).tl=_tmp23B);_tmp23C;});_tmp23B=_tmp364;});{
# 1370
struct Cyc_Interface_I*_tmp23D=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct _dyneither_ptr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(Cyc_read_cycio,_tmp23B,_tmp23B);
if(_tmp23D == 0){
({void*_tmp23E=0U;({struct Cyc___cycFILE*_tmp366=Cyc_stderr;struct _dyneither_ptr _tmp365=({const char*_tmp23F="Error: interfaces incompatible\n";_tag_dyneither(_tmp23F,sizeof(char),32U);});Cyc_fprintf(_tmp366,_tmp365,_tag_dyneither(_tmp23E,sizeof(void*),0U));});});
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp240=1;_npop_handler(0U);return _tmp240;};}
# 1377
if(_tmp21F){
if(Cyc_output_file != 0){
struct _dyneither_ptr _tmp241=({struct Cyc_String_pa_PrintArg_struct _tmp248;_tmp248.tag=0U;({struct _dyneither_ptr _tmp367=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Filename_chop_extension(*((struct _dyneither_ptr*)_check_null(Cyc_output_file))));_tmp248.f1=_tmp367;});({void*_tmp246[1U];_tmp246[0]=& _tmp248;({struct _dyneither_ptr _tmp368=({const char*_tmp247="%s.cycio";_tag_dyneither(_tmp247,sizeof(char),9U);});Cyc_aprintf(_tmp368,_tag_dyneither(_tmp246,sizeof(void*),1U));});});});
struct Cyc___cycFILE*_tmp242=({struct _dyneither_ptr _tmp36A=(struct _dyneither_ptr)_tmp241;struct _dyneither_ptr _tmp369=({const char*_tmp244="wb";_tag_dyneither(_tmp244,sizeof(char),3U);});Cyc_try_file_open(_tmp36A,_tmp369,({const char*_tmp245="interface object file";_tag_dyneither(_tmp245,sizeof(char),22U);}));});
if(_tmp242 == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp243=1;_npop_handler(0U);return _tmp243;};}
# 1386
Cyc_Interface_save(_tmp23D,_tmp242);
Cyc_file_close(_tmp242);}}else{
# 1390
if(!({struct Cyc_Interface_I*_tmp36E=Cyc_Interface_empty();struct Cyc_Interface_I*_tmp36D=_tmp23D;Cyc_Interface_is_subinterface(_tmp36E,_tmp36D,({struct _tuple20*_tmp24B=_cycalloc(sizeof(*_tmp24B));
(({struct _dyneither_ptr _tmp36C=({const char*_tmp249="empty interface";_tag_dyneither(_tmp249,sizeof(char),16U);});(*_tmp24B).f1=_tmp36C;}),({struct _dyneither_ptr _tmp36B=({const char*_tmp24A="global interface";_tag_dyneither(_tmp24A,sizeof(char),17U);});(*_tmp24B).f2=_tmp36B;}));_tmp24B;}));})){
({void*_tmp24C=0U;({struct Cyc___cycFILE*_tmp370=Cyc_stderr;struct _dyneither_ptr _tmp36F=({const char*_tmp24D="Error: some objects are still undefined\n";_tag_dyneither(_tmp24D,sizeof(char),41U);});Cyc_fprintf(_tmp370,_tmp36F,_tag_dyneither(_tmp24C,sizeof(void*),0U));});});
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp24E=1;_npop_handler(0U);return _tmp24E;};}}};};
# 1363
;_pop_handler();}else{void*_tmp239=(void*)_exn_thrown;void*_tmp24F=_tmp239;void*_tmp257;_LL1: _tmp257=_tmp24F;_LL2:
# 1400
({struct Cyc_Int_pa_PrintArg_struct _tmp254;_tmp254.tag=1U;({unsigned long _tmp371=(unsigned long)Cyc_Core_get_exn_lineno();_tmp254.f1=_tmp371;});({struct Cyc_String_pa_PrintArg_struct _tmp253;_tmp253.tag=0U;({struct _dyneither_ptr _tmp372=(struct _dyneither_ptr)({const char*_tmp256=Cyc_Core_get_exn_filename();_tag_dyneither(_tmp256,sizeof(char),_get_zero_arr_size_char((void*)_tmp256,1U));});_tmp253.f1=_tmp372;});({struct Cyc_String_pa_PrintArg_struct _tmp252;_tmp252.tag=0U;({struct _dyneither_ptr _tmp373=(struct _dyneither_ptr)({const char*_tmp255=Cyc_Core_get_exn_name(_tmp257);_tag_dyneither(_tmp255,sizeof(char),_get_zero_arr_size_char((void*)_tmp255,1U));});_tmp252.f1=_tmp373;});({void*_tmp250[3U];(((_tmp250[0]=& _tmp252,_tmp250[1]=& _tmp253)),_tmp250[2]=& _tmp254);({struct Cyc___cycFILE*_tmp375=Cyc_stderr;struct _dyneither_ptr _tmp374=({const char*_tmp251="INTERNAL COMPILER FAILURE:  exception %s from around %s:%d thrown.\n  Please send bug report to cyclone-bugs-l@lists.cs.cornell.edu";_tag_dyneither(_tmp251,sizeof(char),131U);});Cyc_fprintf(_tmp375,_tmp374,_tag_dyneither(_tmp250,sizeof(void*),3U));});});});});});
Cyc_compile_failure=1;
Cyc_remove_cfiles();
return 1;_LL0:;}};}{
# 1406
struct _dyneither_ptr outfile_str=({const char*_tmp26A="";_tag_dyneither(_tmp26A,sizeof(char),1U);});
if(Cyc_output_file != 0)
({struct _dyneither_ptr _tmp378=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp25A;_tmp25A.tag=0U;({struct _dyneither_ptr _tmp376=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(*((struct _dyneither_ptr*)_check_null(Cyc_output_file))));_tmp25A.f1=_tmp376;});({void*_tmp258[1U];_tmp258[0]=& _tmp25A;({struct _dyneither_ptr _tmp377=({const char*_tmp259=" -o %s";_tag_dyneither(_tmp259,sizeof(char),7U);});Cyc_aprintf(_tmp377,_tag_dyneither(_tmp258,sizeof(void*),1U));});});});outfile_str=_tmp378;});{
# 1410
struct _dyneither_ptr _tmp25B=({struct Cyc_String_pa_PrintArg_struct _tmp269;_tmp269.tag=0U;_tmp269.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp21E);({struct Cyc_String_pa_PrintArg_struct _tmp268;_tmp268.tag=0U;_tmp268.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdlib_string);({struct Cyc_String_pa_PrintArg_struct _tmp267;_tmp267.tag=0U;_tmp267.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)outfile_str);({struct Cyc_String_pa_PrintArg_struct _tmp266;_tmp266.tag=0U;_tmp266.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp21D);({struct Cyc_String_pa_PrintArg_struct _tmp265;_tmp265.tag=0U;_tmp265.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp205);({struct Cyc_String_pa_PrintArg_struct _tmp264;_tmp264.tag=0U;_tmp264.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Specsfile_btarget);({struct Cyc_String_pa_PrintArg_struct _tmp263;_tmp263.tag=0U;_tmp263.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp206);({void*_tmp261[7U];(((((((((((_tmp261[0]=& _tmp263,_tmp261[1]=& _tmp264)),_tmp261[2]=& _tmp265)),_tmp261[3]=& _tmp266)),_tmp261[4]=& _tmp267)),_tmp261[5]=& _tmp268)),_tmp261[6]=& _tmp269);({struct _dyneither_ptr _tmp379=({const char*_tmp262="%s %s %s %s%s %s%s";_tag_dyneither(_tmp262,sizeof(char),19U);});Cyc_aprintf(_tmp379,_tag_dyneither(_tmp261,sizeof(void*),7U));});});});});});});});});});
# 1415
if(Cyc_v_r){({struct Cyc_String_pa_PrintArg_struct _tmp25E;_tmp25E.tag=0U;_tmp25E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp25B);({void*_tmp25C[1U];_tmp25C[0]=& _tmp25E;({struct Cyc___cycFILE*_tmp37B=Cyc_stderr;struct _dyneither_ptr _tmp37A=({const char*_tmp25D="%s\n";_tag_dyneither(_tmp25D,sizeof(char),4U);});Cyc_fprintf(_tmp37B,_tmp37A,_tag_dyneither(_tmp25C,sizeof(void*),1U));});});});Cyc_fflush(Cyc_stderr);}
if( system((const char*)_check_null(_untag_dyneither_ptr(_tmp25B,sizeof(char),1U)))!= 0){
({void*_tmp25F=0U;({struct Cyc___cycFILE*_tmp37D=Cyc_stderr;struct _dyneither_ptr _tmp37C=({const char*_tmp260="Error: C compiler failed\n";_tag_dyneither(_tmp260,sizeof(char),26U);});Cyc_fprintf(_tmp37D,_tmp37C,_tag_dyneither(_tmp25F,sizeof(void*),0U));});});
Cyc_compile_failure=1;
Cyc_remove_cfiles();
return 1;}
# 1422
Cyc_remove_cfiles();
# 1424
return Cyc_compile_failure?1: 0;};};};};};};};};}
