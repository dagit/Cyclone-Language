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
typedef struct Cyc___cycFILE Cyc_FILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 252 "cycboot.h"
int Cyc_getw(struct Cyc___cycFILE*);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167 "core.h"
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
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 68
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 72
struct Cyc_Dict_Dict Cyc_Dict_rshare(struct _RegionHandle*,struct Cyc_Dict_Dict);
# 77
int Cyc_Dict_is_empty(struct Cyc_Dict_Dict d);
# 80
int Cyc_Dict_cardinality(struct Cyc_Dict_Dict d);
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 92
struct Cyc_Dict_Dict Cyc_Dict_insert_new(struct Cyc_Dict_Dict d,void*k,void*v);
# 96
struct Cyc_Dict_Dict Cyc_Dict_inserts(struct Cyc_Dict_Dict d,struct Cyc_List_List*l);
# 101
struct Cyc_Dict_Dict Cyc_Dict_singleton(int(*cmp)(void*,void*),void*k,void*v);
# 104
struct Cyc_Dict_Dict Cyc_Dict_rsingleton(struct _RegionHandle*,int(*cmp)(void*,void*),void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 113
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
# 126
int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**ans);
# 131
void*Cyc_Dict_fold(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d,void*accum);
# 135
void*Cyc_Dict_fold_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d,void*accum);
# 139
void Cyc_Dict_app(void*(*f)(void*,void*),struct Cyc_Dict_Dict d);
# 143
void Cyc_Dict_app_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 147
void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict d);
# 149
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 153
void Cyc_Dict_iter2(void(*f)(void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 158
void Cyc_Dict_iter2_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 162
void*Cyc_Dict_fold2_c(void*(*f)(void*,void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2,void*accum);
# 170
struct Cyc_Dict_Dict Cyc_Dict_rcopy(struct _RegionHandle*,struct Cyc_Dict_Dict);
# 173
struct Cyc_Dict_Dict Cyc_Dict_copy(struct Cyc_Dict_Dict);
# 176
struct Cyc_Dict_Dict Cyc_Dict_map(void*(*f)(void*),struct Cyc_Dict_Dict d);
# 181
struct Cyc_Dict_Dict Cyc_Dict_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_Dict_Dict d);
# 186
struct Cyc_Dict_Dict Cyc_Dict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d);
# 189
struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d);
# 195
struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 205
struct Cyc_Dict_Dict Cyc_Dict_intersect(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 214
struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 220
int Cyc_Dict_forall_c(int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 224
int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct _tuple0{void*f1;void*f2;};
# 231
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
# 235
struct Cyc_List_List*Cyc_Dict_to_list(struct Cyc_Dict_Dict d);
# 238
struct Cyc_List_List*Cyc_Dict_rto_list(struct _RegionHandle*,struct Cyc_Dict_Dict d);
# 242
struct Cyc_Dict_Dict Cyc_Dict_filter(int(*f)(void*,void*),struct Cyc_Dict_Dict d);
# 246
struct Cyc_Dict_Dict Cyc_Dict_rfilter(struct _RegionHandle*,int(*f)(void*,void*),struct Cyc_Dict_Dict d);
# 250
struct Cyc_Dict_Dict Cyc_Dict_filter_c(int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 254
struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(struct _RegionHandle*,int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 261
struct Cyc_Dict_Dict Cyc_Dict_difference(struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 267
struct Cyc_Dict_Dict Cyc_Dict_rdifference(struct _RegionHandle*,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 274
struct Cyc_Dict_Dict Cyc_Dict_delete(struct Cyc_Dict_Dict,void*);
# 278
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict,void*);
# 281
struct Cyc_Dict_Dict Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict,void*);
# 287
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);
# 294
void*Cyc_Dict_marshal(struct _RegionHandle*rgn,void*env,void*(*write_key)(void*,struct Cyc___cycFILE*,void*),void*(*write_val)(void*,struct Cyc___cycFILE*,void*),struct Cyc___cycFILE*fp,struct Cyc_Dict_Dict dict);
# 301
struct Cyc_Dict_Dict Cyc_Dict_unmarshal(struct _RegionHandle*rgn,void*env,int(*cmp)(void*,void*),void*(*read_key)(void*,struct Cyc___cycFILE*),void*(*read_val)(void*,struct Cyc___cycFILE*),struct Cyc___cycFILE*fp);char Cyc_Dict_Absent[7]="Absent";char Cyc_Dict_Present[8]="Present";
# 27 "dict.cyc"
struct Cyc_Dict_Absent_exn_struct Cyc_Dict_Absent_val={Cyc_Dict_Absent};
struct Cyc_Dict_Present_exn_struct Cyc_Dict_Present_val={Cyc_Dict_Present};
# 30
enum Cyc_Dict_Color{Cyc_Dict_R  = 0,Cyc_Dict_B  = 1};struct Cyc_Dict_T{enum Cyc_Dict_Color color;const struct Cyc_Dict_T*left;const struct Cyc_Dict_T*right;struct _tuple0 key_val;};
# 39
typedef const struct Cyc_Dict_T*Cyc_Dict_Tree_t;
# 41
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*r,int(*comp)(void*,void*)){
return({struct Cyc_Dict_Dict _tmp2;_tmp2.rel=comp;_tmp2.r=r;_tmp2.t=0;_tmp2;});}
# 44
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*comp)(void*,void*)){
return Cyc_Dict_rempty(Cyc_Core_heap_region,comp);}
# 47
struct Cyc_Dict_Dict Cyc_Dict_rsingleton(struct _RegionHandle*r,int(*comp)(void*,void*),void*key,void*data){
# 49
return({struct Cyc_Dict_Dict _tmp3;_tmp3.rel=comp;_tmp3.r=r;_tmp3.t=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp4=_region_malloc(r,sizeof(*_tmp4));_tmp4->color=Cyc_Dict_B;_tmp4->left=0;_tmp4->right=0;_tmp4->key_val=({struct _tuple0 _tmp5;_tmp5.f1=key;_tmp5.f2=data;_tmp5;});_tmp4;});_tmp3;});}
# 51
struct Cyc_Dict_Dict Cyc_Dict_singleton(int(*comp)(void*,void*),void*key,void*data){
return Cyc_Dict_rsingleton(Cyc_Core_heap_region,comp,key,data);}
# 55
struct Cyc_Dict_Dict Cyc_Dict_rshare(struct _RegionHandle*r,struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*t=d.t;
const struct Cyc_Dict_T*t2=t;
return({struct Cyc_Dict_Dict _tmp6;_tmp6.rel=d.rel;_tmp6.r=r;_tmp6.t=t2;_tmp6;});}
# 61
int Cyc_Dict_is_empty(struct Cyc_Dict_Dict d){
return d.t == (const struct Cyc_Dict_T*)0;}
# 65
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*key){
int(*_tmp7)(void*,void*)=d.rel;
const struct Cyc_Dict_T*_tmp8=d.t;
while(_tmp8 != (const struct Cyc_Dict_T*)0){
int _tmp9=_tmp7(key,(_tmp8->key_val).f1);
if(_tmp9 < 0)_tmp8=_tmp8->left;else{
if(_tmp9 > 0)_tmp8=_tmp8->right;else{
return 1;}}}
# 74
return 0;}
# 78
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*key){
int(*_tmpA)(void*,void*)=d.rel;
const struct Cyc_Dict_T*_tmpB=d.t;
while(_tmpB != (const struct Cyc_Dict_T*)0){
int _tmpC=_tmpA(key,(_tmpB->key_val).f1);
if(_tmpC < 0)_tmpB=_tmpB->left;else{
if(_tmpC > 0)_tmpB=_tmpB->right;else{
return(_tmpB->key_val).f2;}}}
# 87
(int)_throw((void*)& Cyc_Dict_Absent_val);}
# 90
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*key){
const struct Cyc_Dict_T*_tmpD=d.t;
while(_tmpD != (const struct Cyc_Dict_T*)0){
int _tmpE=cmp(key,(_tmpD->key_val).f1);
if(_tmpE < 0)_tmpD=_tmpD->left;else{
if(_tmpE > 0)_tmpD=_tmpD->right;else{
return(_tmpD->key_val).f2;}}}
# 98
(int)_throw((void*)& Cyc_Dict_Absent_val);}
# 101
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*key){
int(*_tmpF)(void*,void*)=d.rel;
const struct Cyc_Dict_T*_tmp10=d.t;
while(_tmp10 != (const struct Cyc_Dict_T*)0){
int _tmp11=_tmpF(key,(_tmp10->key_val).f1);
if(_tmp11 < 0)_tmp10=_tmp10->left;else{
if(_tmp11 > 0)_tmp10=_tmp10->right;else{
return&(_tmp10->key_val).f2;}}}
# 110
return 0;}
# 113
int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*key,void**ans_place){
int(*_tmp12)(void*,void*)=d.rel;
const struct Cyc_Dict_T*_tmp13=d.t;
while(_tmp13 != (const struct Cyc_Dict_T*)0){
int _tmp14=_tmp12(key,(_tmp13->key_val).f1);
if(_tmp14 < 0)_tmp13=_tmp13->left;else{
if(_tmp14 > 0)_tmp13=_tmp13->right;else{
# 121
*ans_place=(_tmp13->key_val).f2;
return 1;}}}
# 125
return 0;}struct _tuple1{enum Cyc_Dict_Color f1;const struct Cyc_Dict_T*f2;const struct Cyc_Dict_T*f3;struct _tuple0 f4;};
# 128
static const struct Cyc_Dict_T*Cyc_Dict_balance(struct _RegionHandle*r,struct _tuple1 quad){
# 131
struct _tuple1 _tmp15=quad;enum Cyc_Dict_Color _tmp42;const struct Cyc_Dict_T*_tmp41;const struct Cyc_Dict_T*_tmp40;struct _tuple0 _tmp3F;const struct Cyc_Dict_T*_tmp3E;const struct Cyc_Dict_T*_tmp3D;const struct Cyc_Dict_T*_tmp3C;const struct Cyc_Dict_T*_tmp3B;struct _tuple0 _tmp3A;struct _tuple0 _tmp39;struct _tuple0 _tmp38;const struct Cyc_Dict_T*_tmp37;const struct Cyc_Dict_T*_tmp36;const struct Cyc_Dict_T*_tmp35;struct _tuple0 _tmp34;const struct Cyc_Dict_T*_tmp33;struct _tuple0 _tmp32;struct _tuple0 _tmp31;const struct Cyc_Dict_T*_tmp30;const struct Cyc_Dict_T*_tmp2F;const struct Cyc_Dict_T*_tmp2E;struct _tuple0 _tmp2D;struct _tuple0 _tmp2C;const struct Cyc_Dict_T*_tmp2B;struct _tuple0 _tmp2A;const struct Cyc_Dict_T*_tmp29;const struct Cyc_Dict_T*_tmp28;struct _tuple0 _tmp27;const struct Cyc_Dict_T*_tmp26;struct _tuple0 _tmp25;const struct Cyc_Dict_T*_tmp24;struct _tuple0 _tmp23;if(_tmp15.f1 == Cyc_Dict_B){if(_tmp15.f2 != 0){if(((const struct Cyc_Dict_T*)_tmp15.f2)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmp15.f2)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmp15.f2)->left)->color == Cyc_Dict_R){_LL1: _tmp29=((_tmp15.f2)->left)->left;_tmp28=((_tmp15.f2)->left)->right;_tmp27=((_tmp15.f2)->left)->key_val;_tmp26=(_tmp15.f2)->right;_tmp25=(_tmp15.f2)->key_val;_tmp24=_tmp15.f3;_tmp23=_tmp15.f4;_LL2:
# 133
 return(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp16=_region_malloc(r,sizeof(*_tmp16));_tmp16->color=Cyc_Dict_R;_tmp16->left=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp18=_region_malloc(r,sizeof(*_tmp18));_tmp18->color=Cyc_Dict_B;_tmp18->left=_tmp29;_tmp18->right=_tmp28;_tmp18->key_val=_tmp27;_tmp18;});_tmp16->right=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp17=_region_malloc(r,sizeof(*_tmp17));_tmp17->color=Cyc_Dict_B;_tmp17->left=_tmp26;_tmp17->right=_tmp24;_tmp17->key_val=_tmp23;_tmp17;});_tmp16->key_val=_tmp25;_tmp16;});}else{if(((const struct Cyc_Dict_T*)_tmp15.f2)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmp15.f2)->right)->color == Cyc_Dict_R)goto _LL3;else{if(_tmp15.f3 != 0){if(((const struct Cyc_Dict_T*)_tmp15.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmp15.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmp15.f3)->left)->color == Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)_tmp15.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmp15.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_tmp15.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmp15.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}else{if(_tmp15.f3 != 0){if(((const struct Cyc_Dict_T*)_tmp15.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmp15.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmp15.f3)->left)->color == Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)_tmp15.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmp15.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_tmp15.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmp15.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}}else{if(((const struct Cyc_Dict_T*)_tmp15.f2)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmp15.f2)->right)->color == Cyc_Dict_R){_LL3: _tmp30=(_tmp15.f2)->left;_tmp2F=((_tmp15.f2)->right)->left;_tmp2E=((_tmp15.f2)->right)->right;_tmp2D=((_tmp15.f2)->right)->key_val;_tmp2C=(_tmp15.f2)->key_val;_tmp2B=_tmp15.f3;_tmp2A=_tmp15.f4;_LL4:
# 135
 return(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp19=_region_malloc(r,sizeof(*_tmp19));_tmp19->color=Cyc_Dict_R;_tmp19->left=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp1B=_region_malloc(r,sizeof(*_tmp1B));_tmp1B->color=Cyc_Dict_B;_tmp1B->left=_tmp30;_tmp1B->right=_tmp2F;_tmp1B->key_val=_tmp2C;_tmp1B;});_tmp19->right=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp1A=_region_malloc(r,sizeof(*_tmp1A));_tmp1A->color=Cyc_Dict_B;_tmp1A->left=_tmp2E;_tmp1A->right=_tmp2B;_tmp1A->key_val=_tmp2A;_tmp1A;});_tmp19->key_val=_tmp2D;_tmp19;});}else{if(_tmp15.f3 != 0){if(((const struct Cyc_Dict_T*)_tmp15.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmp15.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmp15.f3)->left)->color == Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)_tmp15.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmp15.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_tmp15.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmp15.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}else{if(_tmp15.f3 != 0){if(((const struct Cyc_Dict_T*)_tmp15.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmp15.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmp15.f3)->left)->color == Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)_tmp15.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmp15.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_tmp15.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmp15.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}}else{if(_tmp15.f3 != 0){if(((const struct Cyc_Dict_T*)_tmp15.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmp15.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmp15.f3)->left)->color == Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)_tmp15.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmp15.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_tmp15.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmp15.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}else{if(_tmp15.f3 != 0){if(((const struct Cyc_Dict_T*)_tmp15.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmp15.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmp15.f3)->left)->color == Cyc_Dict_R){_LL5: _tmp37=_tmp15.f2;_tmp36=((_tmp15.f3)->left)->left;_tmp35=((_tmp15.f3)->left)->right;_tmp34=((_tmp15.f3)->left)->key_val;_tmp33=(_tmp15.f3)->right;_tmp32=(_tmp15.f3)->key_val;_tmp31=_tmp15.f4;_LL6:
# 137
 return(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp1C=_region_malloc(r,sizeof(*_tmp1C));_tmp1C->color=Cyc_Dict_R;_tmp1C->left=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp1E=_region_malloc(r,sizeof(*_tmp1E));_tmp1E->color=Cyc_Dict_B;_tmp1E->left=_tmp37;_tmp1E->right=_tmp36;_tmp1E->key_val=_tmp31;_tmp1E;});_tmp1C->right=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp1D=_region_malloc(r,sizeof(*_tmp1D));_tmp1D->color=Cyc_Dict_B;_tmp1D->left=_tmp35;_tmp1D->right=_tmp33;_tmp1D->key_val=_tmp32;_tmp1D;});_tmp1C->key_val=_tmp34;_tmp1C;});}else{if(((const struct Cyc_Dict_T*)_tmp15.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmp15.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_tmp15.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmp15.f3)->right)->color == Cyc_Dict_R){_LL7: _tmp3E=_tmp15.f2;_tmp3D=(_tmp15.f3)->left;_tmp3C=((_tmp15.f3)->right)->left;_tmp3B=((_tmp15.f3)->right)->right;_tmp3A=((_tmp15.f3)->right)->key_val;_tmp39=(_tmp15.f3)->key_val;_tmp38=_tmp15.f4;_LL8:
# 139
 return(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp1F=_region_malloc(r,sizeof(*_tmp1F));_tmp1F->color=Cyc_Dict_R;_tmp1F->left=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp21=_region_malloc(r,sizeof(*_tmp21));_tmp21->color=Cyc_Dict_B;_tmp21->left=_tmp3E;_tmp21->right=_tmp3D;_tmp21->key_val=_tmp38;_tmp21;});_tmp1F->right=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp20=_region_malloc(r,sizeof(*_tmp20));_tmp20->color=Cyc_Dict_B;_tmp20->left=_tmp3C;_tmp20->right=_tmp3B;_tmp20->key_val=_tmp3A;_tmp20;});_tmp1F->key_val=_tmp39;_tmp1F;});}else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}else{_LL9: _tmp42=_tmp15.f1;_tmp41=_tmp15.f2;_tmp40=_tmp15.f3;_tmp3F=_tmp15.f4;_LLA:
# 141
 return(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp22=_region_malloc(r,sizeof(*_tmp22));_tmp22->color=_tmp42;_tmp22->left=_tmp41;_tmp22->right=_tmp40;_tmp22->key_val=_tmp3F;_tmp22;});}_LL0:;}
# 145
static const struct Cyc_Dict_T*Cyc_Dict_ins(struct _RegionHandle*r,int(*rel)(void*,void*),struct _tuple0 key_val,const struct Cyc_Dict_T*t){
# 147
const struct Cyc_Dict_T*_tmp43=t;enum Cyc_Dict_Color _tmp4C;const struct Cyc_Dict_T*_tmp4B;const struct Cyc_Dict_T*_tmp4A;struct _tuple0 _tmp49;if(_tmp43 == 0){_LLC: _LLD:
 return(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp44=_region_malloc(r,sizeof(*_tmp44));_tmp44->color=Cyc_Dict_R;_tmp44->left=0;_tmp44->right=0;_tmp44->key_val=key_val;_tmp44;});}else{_LLE: _tmp4C=_tmp43->color;_tmp4B=_tmp43->left;_tmp4A=_tmp43->right;_tmp49=_tmp43->key_val;_LLF: {
# 150
int _tmp45=rel(key_val.f1,_tmp49.f1);
if(_tmp45 < 0)return Cyc_Dict_balance(r,({struct _tuple1 _tmp46;_tmp46.f1=_tmp4C;_tmp46.f2=Cyc_Dict_ins(r,rel,key_val,_tmp4B);_tmp46.f3=_tmp4A;_tmp46.f4=_tmp49;_tmp46;}));else{
if(_tmp45 > 0)return Cyc_Dict_balance(r,({struct _tuple1 _tmp47;_tmp47.f1=_tmp4C;_tmp47.f2=_tmp4B;_tmp47.f3=Cyc_Dict_ins(r,rel,key_val,_tmp4A);_tmp47.f4=_tmp49;_tmp47;}));else{
return(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp48=_region_malloc(r,sizeof(*_tmp48));_tmp48->color=_tmp4C;_tmp48->left=_tmp4B;_tmp48->right=_tmp4A;_tmp48->key_val=key_val;_tmp48;});}}}}_LLB:;}
# 157
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*key,void*data){
const struct Cyc_Dict_T*_tmp4D=Cyc_Dict_ins(d.r,d.rel,({struct _tuple0 _tmp50;_tmp50.f1=key;_tmp50.f2=data;_tmp50;}),d.t);
((const struct Cyc_Dict_T*)_check_null(_tmp4D))->color=Cyc_Dict_B;{
struct Cyc_Dict_Dict _tmp4E=({struct Cyc_Dict_Dict _tmp4F;_tmp4F.rel=d.rel;_tmp4F.r=d.r;_tmp4F.t=_tmp4D;_tmp4F;});
return _tmp4E;};}
# 165
struct Cyc_Dict_Dict Cyc_Dict_insert_new(struct Cyc_Dict_Dict d,void*key,void*data){
if(Cyc_Dict_member(d,key))
(int)_throw((void*)& Cyc_Dict_Absent_val);
return Cyc_Dict_insert(d,key,data);}
# 171
struct Cyc_Dict_Dict Cyc_Dict_inserts(struct Cyc_Dict_Dict d,struct Cyc_List_List*kds){
# 173
for(0;kds != 0;kds=kds->tl){
d=Cyc_Dict_insert(d,(((struct _tuple0*)kds->hd)[0]).f1,(((struct _tuple0*)kds->hd)[0]).f2);}
return d;}
# 178
static void*Cyc_Dict_fold_tree(void*(*f)(void*,void*,void*),const struct Cyc_Dict_T*t,void*accum){
const struct Cyc_Dict_T*_tmp51=t;const struct Cyc_Dict_T*_tmp55;const struct Cyc_Dict_T*_tmp54;void*_tmp53;void*_tmp52;_LL11: _tmp55=_tmp51->left;_tmp54=_tmp51->right;_tmp53=(_tmp51->key_val).f1;_tmp52=(_tmp51->key_val).f2;_LL12:;
if(_tmp55 != (const struct Cyc_Dict_T*)0)accum=Cyc_Dict_fold_tree(f,_tmp55,accum);
accum=f(_tmp53,_tmp52,accum);
if(_tmp54 != (const struct Cyc_Dict_T*)0)accum=Cyc_Dict_fold_tree(f,_tmp54,accum);
return accum;}
# 186
void*Cyc_Dict_fold(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d,void*accum){
const struct Cyc_Dict_T*_tmp56=d.t;
if(_tmp56 == (const struct Cyc_Dict_T*)0)
return accum;
return Cyc_Dict_fold_tree(f,_tmp56,accum);}
# 193
static void*Cyc_Dict_fold_tree_c(void*(*f)(void*,void*,void*,void*),void*env,const struct Cyc_Dict_T*t,void*accum){
# 195
const struct Cyc_Dict_T*_tmp57=t;const struct Cyc_Dict_T*_tmp5B;const struct Cyc_Dict_T*_tmp5A;void*_tmp59;void*_tmp58;_LL14: _tmp5B=_tmp57->left;_tmp5A=_tmp57->right;_tmp59=(_tmp57->key_val).f1;_tmp58=(_tmp57->key_val).f2;_LL15:;
if(_tmp5B != (const struct Cyc_Dict_T*)0)
accum=Cyc_Dict_fold_tree_c(f,env,_tmp5B,accum);
accum=f(env,_tmp59,_tmp58,accum);
if(_tmp5A != (const struct Cyc_Dict_T*)0)
accum=Cyc_Dict_fold_tree_c(f,env,_tmp5A,accum);
return accum;}
# 204
void*Cyc_Dict_fold_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d,void*accum){
const struct Cyc_Dict_T*_tmp5C=d.t;
if(_tmp5C == (const struct Cyc_Dict_T*)0)
return accum;
return Cyc_Dict_fold_tree_c(f,env,_tmp5C,accum);}
# 211
static void Cyc_Dict_app_tree(void*(*f)(void*,void*),const struct Cyc_Dict_T*t){
const struct Cyc_Dict_T*_tmp5D=t;const struct Cyc_Dict_T*_tmp61;const struct Cyc_Dict_T*_tmp60;void*_tmp5F;void*_tmp5E;_LL17: _tmp61=_tmp5D->left;_tmp60=_tmp5D->right;_tmp5F=(_tmp5D->key_val).f1;_tmp5E=(_tmp5D->key_val).f2;_LL18:;
if(_tmp61 != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree(f,_tmp61);
f(_tmp5F,_tmp5E);
if(_tmp60 != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree(f,_tmp60);}
# 218
void Cyc_Dict_app(void*(*f)(void*,void*),struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*_tmp62=d.t;
if(_tmp62 != (const struct Cyc_Dict_T*)0)
Cyc_Dict_app_tree(f,_tmp62);}
# 224
static void Cyc_Dict_app_tree_c(void*(*f)(void*,void*,void*),void*env,const struct Cyc_Dict_T*t){
const struct Cyc_Dict_T*_tmp63=t;const struct Cyc_Dict_T*_tmp67;const struct Cyc_Dict_T*_tmp66;void*_tmp65;void*_tmp64;_LL1A: _tmp67=_tmp63->left;_tmp66=_tmp63->right;_tmp65=(_tmp63->key_val).f1;_tmp64=(_tmp63->key_val).f2;_LL1B:;
if(_tmp67 != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree_c(f,env,_tmp67);
f(env,_tmp65,_tmp64);
if(_tmp66 != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree_c(f,env,_tmp66);}
# 231
void Cyc_Dict_app_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*_tmp68=d.t;
if(_tmp68 != (const struct Cyc_Dict_T*)0)
Cyc_Dict_app_tree_c(f,env,_tmp68);}
# 237
static void Cyc_Dict_iter_tree(void(*f)(void*,void*),const struct Cyc_Dict_T*t){
const struct Cyc_Dict_T*_tmp69=t;const struct Cyc_Dict_T*_tmp6D;const struct Cyc_Dict_T*_tmp6C;void*_tmp6B;void*_tmp6A;_LL1D: _tmp6D=_tmp69->left;_tmp6C=_tmp69->right;_tmp6B=(_tmp69->key_val).f1;_tmp6A=(_tmp69->key_val).f2;_LL1E:;
if(_tmp6D != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree(f,_tmp6D);
f(_tmp6B,_tmp6A);
if(_tmp6C != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree(f,_tmp6C);}
# 244
void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*_tmp6E=d.t;
if(_tmp6E != (const struct Cyc_Dict_T*)0)
Cyc_Dict_iter_tree(f,_tmp6E);}
# 250
static void Cyc_Dict_iter_tree_c(void(*f)(void*,void*,void*),void*env,const struct Cyc_Dict_T*t){
const struct Cyc_Dict_T*_tmp6F=t;const struct Cyc_Dict_T*_tmp73;const struct Cyc_Dict_T*_tmp72;void*_tmp71;void*_tmp70;_LL20: _tmp73=_tmp6F->left;_tmp72=_tmp6F->right;_tmp71=(_tmp6F->key_val).f1;_tmp70=(_tmp6F->key_val).f2;_LL21:;
if(_tmp73 != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree_c(f,env,_tmp73);
f(env,_tmp71,_tmp70);
if(_tmp72 != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree_c(f,env,_tmp72);}
# 257
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*_tmp74=d.t;
if(_tmp74 != (const struct Cyc_Dict_T*)0)
Cyc_Dict_iter_tree_c(f,env,_tmp74);}
# 263
static void Cyc_Dict_count_elem(int*cnt,void*a,void*b){
*cnt=*cnt + 1;}
# 266
int Cyc_Dict_cardinality(struct Cyc_Dict_Dict d){
int num=0;
((void(*)(void(*f)(int*,void*,void*),int*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Dict_count_elem,& num,d);
return num;}struct _tuple2{void(*f1)(void*,void*);struct Cyc_Dict_Dict f2;};
# 272
static void Cyc_Dict_iter2_f(struct _tuple2*env,void*a,void*b1){
# 274
struct _tuple2*_tmp75=env;void(*_tmp77)(void*,void*);struct Cyc_Dict_Dict _tmp76;_LL23: _tmp77=_tmp75->f1;_tmp76=_tmp75->f2;_LL24:;
_tmp77(b1,Cyc_Dict_lookup(_tmp76,a));}
# 278
void Cyc_Dict_iter2(void(*f)(void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 281
struct _tuple2 _tmp78=({struct _tuple2 _tmp79;_tmp79.f1=f;_tmp79.f2=d2;_tmp79;});
((void(*)(void(*f)(struct _tuple2*,void*,void*),struct _tuple2*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Dict_iter2_f,& _tmp78,d1);}struct _tuple3{void(*f1)(void*,void*,void*);struct Cyc_Dict_Dict f2;void*f3;};
# 285
static void Cyc_Dict_iter2_c_f(struct _tuple3*env,void*a,void*b1){
# 287
struct _tuple3*_tmp7A=env;void(*_tmp7D)(void*,void*,void*);struct Cyc_Dict_Dict _tmp7C;void*_tmp7B;_LL26: _tmp7D=_tmp7A->f1;_tmp7C=_tmp7A->f2;_tmp7B=_tmp7A->f3;_LL27:;
_tmp7D(_tmp7B,b1,Cyc_Dict_lookup(_tmp7C,a));}
# 291
void Cyc_Dict_iter2_c(void(*f)(void*,void*,void*),void*inner_env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 294
struct _tuple3 _tmp7E=({struct _tuple3 _tmp7F;_tmp7F.f1=f;_tmp7F.f2=d2;_tmp7F.f3=inner_env;_tmp7F;});
((void(*)(void(*f)(struct _tuple3*,void*,void*),struct _tuple3*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Dict_iter2_c_f,& _tmp7E,d1);}struct _tuple4{void*(*f1)(void*,void*,void*,void*,void*);struct Cyc_Dict_Dict f2;void*f3;};
# 298
static void*Cyc_Dict_fold2_c_f(struct _tuple4*env,void*a,void*b1,void*accum){
# 302
struct _tuple4*_tmp80=env;void*(*_tmp83)(void*,void*,void*,void*,void*);struct Cyc_Dict_Dict _tmp82;void*_tmp81;_LL29: _tmp83=_tmp80->f1;_tmp82=_tmp80->f2;_tmp81=_tmp80->f3;_LL2A:;
return _tmp83(_tmp81,a,b1,Cyc_Dict_lookup(_tmp82,a),accum);}
# 306
void*Cyc_Dict_fold2_c(void*(*f)(void*,void*,void*,void*,void*),void*inner_env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2,void*accum){
# 310
struct _tuple4 _tmp84=({struct _tuple4 _tmp85;_tmp85.f1=f;_tmp85.f2=d2;_tmp85.f3=inner_env;_tmp85;});
return((void*(*)(void*(*f)(struct _tuple4*,void*,void*,void*),struct _tuple4*env,struct Cyc_Dict_Dict d,void*accum))Cyc_Dict_fold_c)(Cyc_Dict_fold2_c_f,& _tmp84,d1,accum);}
# 314
static const struct Cyc_Dict_T*Cyc_Dict_copy_tree(struct _RegionHandle*r2,const struct Cyc_Dict_T*t){
if(t == (const struct Cyc_Dict_T*)0)return 0;else{
# 317
struct Cyc_Dict_T _tmp86=*t;struct Cyc_Dict_T _tmp87=_tmp86;enum Cyc_Dict_Color _tmp8E;const struct Cyc_Dict_T*_tmp8D;const struct Cyc_Dict_T*_tmp8C;struct _tuple0 _tmp8B;_LL2C: _tmp8E=_tmp87.color;_tmp8D=_tmp87.left;_tmp8C=_tmp87.right;_tmp8B=_tmp87.key_val;_LL2D:;{
const struct Cyc_Dict_T*_tmp88=Cyc_Dict_copy_tree(r2,_tmp8D);
const struct Cyc_Dict_T*_tmp89=Cyc_Dict_copy_tree(r2,_tmp8C);
return(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp8A=_region_malloc(r2,sizeof(*_tmp8A));_tmp8A->color=_tmp8E;_tmp8A->left=_tmp88;_tmp8A->right=_tmp89;_tmp8A->key_val=_tmp8B;_tmp8A;});};}}
# 324
struct Cyc_Dict_Dict Cyc_Dict_rcopy(struct _RegionHandle*r2,struct Cyc_Dict_Dict d){
return({struct Cyc_Dict_Dict _tmp8F;_tmp8F.rel=d.rel;_tmp8F.r=r2;_tmp8F.t=Cyc_Dict_copy_tree(r2,d.t);_tmp8F;});}
# 328
struct Cyc_Dict_Dict Cyc_Dict_copy(struct Cyc_Dict_Dict d){
return Cyc_Dict_rcopy(Cyc_Core_heap_region,d);}
# 332
static const struct Cyc_Dict_T*Cyc_Dict_map_tree(struct _RegionHandle*r,void*(*f)(void*),const struct Cyc_Dict_T*t){
# 334
const struct Cyc_Dict_T*_tmp90=t;enum Cyc_Dict_Color _tmp9A;const struct Cyc_Dict_T*_tmp99;const struct Cyc_Dict_T*_tmp98;void*_tmp97;void*_tmp96;_LL2F: _tmp9A=_tmp90->color;_tmp99=_tmp90->left;_tmp98=_tmp90->right;_tmp97=(_tmp90->key_val).f1;_tmp96=(_tmp90->key_val).f2;_LL30:;{
const struct Cyc_Dict_T*_tmp91=_tmp99 == (const struct Cyc_Dict_T*)0?0: Cyc_Dict_map_tree(r,f,_tmp99);
void*_tmp92=f(_tmp96);
const struct Cyc_Dict_T*_tmp93=_tmp98 == (const struct Cyc_Dict_T*)0?0: Cyc_Dict_map_tree(r,f,_tmp98);
return(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp94=_region_malloc(r,sizeof(*_tmp94));_tmp94->color=_tmp9A;_tmp94->left=_tmp91;_tmp94->right=_tmp93;_tmp94->key_val=({struct _tuple0 _tmp95;_tmp95.f1=_tmp97;_tmp95.f2=_tmp92;_tmp95;});_tmp94;});};}
# 341
struct Cyc_Dict_Dict Cyc_Dict_rmap(struct _RegionHandle*r,void*(*f)(void*),struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*_tmp9B=d.t;
if(_tmp9B == (const struct Cyc_Dict_T*)0)
return({struct Cyc_Dict_Dict _tmp9C;_tmp9C.rel=d.rel;_tmp9C.r=r;_tmp9C.t=0;_tmp9C;});
return({struct Cyc_Dict_Dict _tmp9D;_tmp9D.rel=d.rel;_tmp9D.r=r;_tmp9D.t=Cyc_Dict_map_tree(r,f,_tmp9B);_tmp9D;});}
# 348
struct Cyc_Dict_Dict Cyc_Dict_map(void*(*f)(void*),struct Cyc_Dict_Dict d){
return Cyc_Dict_rmap(Cyc_Core_heap_region,f,d);}
# 352
static const struct Cyc_Dict_T*Cyc_Dict_map_tree_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,const struct Cyc_Dict_T*t){
# 354
const struct Cyc_Dict_T*_tmp9E=t;enum Cyc_Dict_Color _tmpA8;const struct Cyc_Dict_T*_tmpA7;const struct Cyc_Dict_T*_tmpA6;void*_tmpA5;void*_tmpA4;_LL32: _tmpA8=_tmp9E->color;_tmpA7=_tmp9E->left;_tmpA6=_tmp9E->right;_tmpA5=(_tmp9E->key_val).f1;_tmpA4=(_tmp9E->key_val).f2;_LL33:;{
const struct Cyc_Dict_T*_tmp9F=
_tmpA7 == (const struct Cyc_Dict_T*)0?0: Cyc_Dict_map_tree_c(r,f,env,_tmpA7);
void*_tmpA0=f(env,_tmpA4);
const struct Cyc_Dict_T*_tmpA1=
_tmpA6 == (const struct Cyc_Dict_T*)0?0: Cyc_Dict_map_tree_c(r,f,env,_tmpA6);
return(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmpA2=_region_malloc(r,sizeof(*_tmpA2));_tmpA2->color=_tmpA8;_tmpA2->left=_tmp9F;_tmpA2->right=_tmpA1;_tmpA2->key_val=({struct _tuple0 _tmpA3;_tmpA3.f1=_tmpA5;_tmpA3.f2=_tmpA0;_tmpA3;});_tmpA2;});};}
# 363
struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d){
# 365
const struct Cyc_Dict_T*_tmpA9=d.t;
if(_tmpA9 == (const struct Cyc_Dict_T*)0)
return({struct Cyc_Dict_Dict _tmpAA;_tmpAA.rel=d.rel;_tmpAA.r=r;_tmpAA.t=0;_tmpAA;});
return({struct Cyc_Dict_Dict _tmpAB;_tmpAB.rel=d.rel;_tmpAB.r=r;_tmpAB.t=Cyc_Dict_map_tree_c(r,f,env,_tmpA9);_tmpAB;});}
# 371
struct Cyc_Dict_Dict Cyc_Dict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d){
return Cyc_Dict_rmap_c(Cyc_Core_heap_region,f,env,d);}
# 375
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d){
if(d.t == (const struct Cyc_Dict_T*)0)
(int)_throw((void*)& Cyc_Dict_Absent_val);
return({struct _tuple0*_tmpAC=_region_malloc(r,sizeof(*_tmpAC));_tmpAC->f1=((d.t)->key_val).f1;_tmpAC->f2=((d.t)->key_val).f2;_tmpAC;});}
# 381
static int Cyc_Dict_forall_tree_c(int(*f)(void*,void*,void*),void*env,const struct Cyc_Dict_T*t){
const struct Cyc_Dict_T*_tmpAD=t;const struct Cyc_Dict_T*_tmpB1;const struct Cyc_Dict_T*_tmpB0;void*_tmpAF;void*_tmpAE;_LL35: _tmpB1=_tmpAD->left;_tmpB0=_tmpAD->right;_tmpAF=(_tmpAD->key_val).f1;_tmpAE=(_tmpAD->key_val).f2;_LL36:;
return
((_tmpB1 == (const struct Cyc_Dict_T*)0  || Cyc_Dict_forall_tree_c(f,env,_tmpB1)) && 
f(env,_tmpAF,_tmpAE)) && (
_tmpB0 == (const struct Cyc_Dict_T*)0  || Cyc_Dict_forall_tree_c(f,env,_tmpB0));}
# 389
int Cyc_Dict_forall_c(int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*_tmpB2=d.t;
if(_tmpB2 == (const struct Cyc_Dict_T*)0)
return 1;
return Cyc_Dict_forall_tree_c(f,env,_tmpB2);}struct _tuple5{int(*f1)(void*,void*,void*);struct Cyc_Dict_Dict f2;};
# 396
static int Cyc_Dict_forall_intersect_f(struct _tuple5*env,void*a,void*b){
# 398
struct _tuple5*_tmpB3=env;int(*_tmpB5)(void*,void*,void*);struct Cyc_Dict_Dict _tmpB4;_LL38: _tmpB5=_tmpB3->f1;_tmpB4=_tmpB3->f2;_LL39:;
if(Cyc_Dict_member(_tmpB4,a))
return _tmpB5(a,b,Cyc_Dict_lookup(_tmpB4,a));
return 1;}
# 405
int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
struct _tuple5 _tmpB6=({struct _tuple5 _tmpB7;_tmpB7.f1=f;_tmpB7.f2=d2;_tmpB7;});
return((int(*)(int(*f)(struct _tuple5*,void*,void*),struct _tuple5*env,struct Cyc_Dict_Dict d))Cyc_Dict_forall_c)(Cyc_Dict_forall_intersect_f,& _tmpB6,d1);}struct _tuple6{void*(*f1)(void*,void*,void*,void*);void*f2;};
# 410
static struct Cyc_Dict_Dict*Cyc_Dict_union_f(struct _tuple6*env,void*a,void*b,struct Cyc_Dict_Dict*d1){
# 414
if(Cyc_Dict_member(*d1,a)){
# 416
void*_tmpB8=Cyc_Dict_lookup(*d1,a);
void*_tmpB9=((*env).f1)((*env).f2,a,_tmpB8,b);
if(_tmpB9 != _tmpB8)
*d1=Cyc_Dict_insert(*d1,a,_tmpB9);
return d1;}
# 422
*d1=Cyc_Dict_insert(*d1,a,b);
return d1;}
# 426
struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 429
if((int)d1.t == (int)d2.t)return d1;
if(d1.t == (const struct Cyc_Dict_T*)0)return d2;
if(d2.t == (const struct Cyc_Dict_T*)0)return d1;{
struct _tuple6 _tmpBA=({struct _tuple6 _tmpBB;_tmpBB.f1=f;_tmpBB.f2=env;_tmpBB;});
((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*f)(struct _tuple6*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple6*env,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*accum))Cyc_Dict_fold_c)(Cyc_Dict_union_f,& _tmpBA,d2,& d1);
return d1;};}
# 437
struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 440
const struct Cyc_Dict_T*_tmpBC=d2.t;
if(_tmpBC == (const struct Cyc_Dict_T*)0)return d2;
if((int)d1.t == (int)_tmpBC)return d2;{
const struct Cyc_Dict_T*ans_tree=0;
struct _RegionHandle _tmpBD=_new_region("temp");struct _RegionHandle*temp=& _tmpBD;_push_region(temp);{
# 446
struct _dyneither_ptr queue=_tag_dyneither(({unsigned int _tmpC9=(unsigned int)16;const struct Cyc_Dict_T**_tmpCA=(const struct Cyc_Dict_T**)_region_malloc(temp,_check_times(sizeof(const struct Cyc_Dict_T*),_tmpC9));{unsigned int _tmpCB=_tmpC9;unsigned int i;for(i=0;i < _tmpCB;i ++){_tmpCA[i]=(const struct Cyc_Dict_T*)_tmpBC;}}_tmpCA;}),sizeof(const struct Cyc_Dict_T*),(unsigned int)16);
int ind=0;
while(ind != - 1){
const struct Cyc_Dict_T*_tmpBE=*((const struct Cyc_Dict_T**)_check_dyneither_subscript(queue,sizeof(const struct Cyc_Dict_T*),ind --));const struct Cyc_Dict_T*_tmpBF=_tmpBE;const struct Cyc_Dict_T*_tmpC8;const struct Cyc_Dict_T*_tmpC7;void*_tmpC6;void*_tmpC5;_LL3B: _tmpC8=_tmpBF->left;_tmpC7=_tmpBF->right;_tmpC6=(_tmpBF->key_val).f1;_tmpC5=(_tmpBF->key_val).f2;_LL3C:;
if(ind + 2 >= _get_dyneither_size(queue,sizeof(const struct Cyc_Dict_T*)))
queue=({unsigned int _tmpC0=_get_dyneither_size(queue,sizeof(const struct Cyc_Dict_T*))* 2;const struct Cyc_Dict_T**_tmpC1=(const struct Cyc_Dict_T**)_region_malloc(temp,_check_times(sizeof(const struct Cyc_Dict_T*),_tmpC0));struct _dyneither_ptr _tmpC3=_tag_dyneither(_tmpC1,sizeof(const struct Cyc_Dict_T*),_tmpC0);{unsigned int _tmpC2=_tmpC0;unsigned int i;for(i=0;i < _tmpC2;i ++){_tmpC1[i]=(const struct Cyc_Dict_T*)(
i < _get_dyneither_size(queue,sizeof(const struct Cyc_Dict_T*))?((const struct Cyc_Dict_T**)queue.curr)[(int)i]: _tmpBC);}}_tmpC3;});
if(_tmpC8 != (const struct Cyc_Dict_T*)0)*((const struct Cyc_Dict_T**)_check_dyneither_subscript(queue,sizeof(const struct Cyc_Dict_T*),++ ind))=_tmpC8;
if(_tmpC7 != (const struct Cyc_Dict_T*)0)*((const struct Cyc_Dict_T**)_check_dyneither_subscript(queue,sizeof(const struct Cyc_Dict_T*),++ ind))=_tmpC7;
if(Cyc_Dict_member(d1,_tmpC6))
ans_tree=Cyc_Dict_ins(d2.r,d2.rel,({struct _tuple0 _tmpC4;_tmpC4.f1=_tmpC6;_tmpC4.f2=f(env,_tmpC6,Cyc_Dict_lookup(d1,_tmpC6),_tmpC5);_tmpC4;}),ans_tree);}}{
# 459
struct Cyc_Dict_Dict _tmpCD=({struct Cyc_Dict_Dict _tmpCC;_tmpCC.rel=d2.rel;_tmpCC.r=d2.r;_tmpCC.t=ans_tree;_tmpCC;});_npop_handler(0);return _tmpCD;};
# 444
;_pop_region(temp);};}
# 462
static void*Cyc_Dict_intersect_f(void*(*f)(void*,void*,void*),void*a,void*b1,void*b2){
return f(a,b1,b2);}
# 466
struct Cyc_Dict_Dict Cyc_Dict_intersect(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 469
return((struct Cyc_Dict_Dict(*)(void*(*f)(void*(*)(void*,void*,void*),void*,void*,void*),void*(*env)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)(Cyc_Dict_intersect_f,f,d1,d2);}
# 472
static struct Cyc_List_List*Cyc_Dict_to_list_f(struct _RegionHandle*r,void*k,void*v,struct Cyc_List_List*accum){
# 474
return({struct Cyc_List_List*_tmpCE=_region_malloc(r,sizeof(*_tmpCE));_tmpCE->hd=({struct _tuple0*_tmpCF=_region_malloc(r,sizeof(*_tmpCF));_tmpCF->f1=k;_tmpCF->f2=v;_tmpCF;});_tmpCE->tl=accum;_tmpCE;});}
# 477
struct Cyc_List_List*Cyc_Dict_rto_list(struct _RegionHandle*r,struct Cyc_Dict_Dict d){
return((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _RegionHandle*,void*,void*,struct Cyc_List_List*),struct _RegionHandle*env,struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold_c)(Cyc_Dict_to_list_f,r,d,0);}
# 480
struct Cyc_List_List*Cyc_Dict_to_list(struct Cyc_Dict_Dict d){
return Cyc_Dict_rto_list(Cyc_Core_heap_region,d);}struct _tuple7{int(*f1)(void*,void*);struct _RegionHandle*f2;};
# 484
static struct Cyc_Dict_Dict*Cyc_Dict_filter_f(struct _tuple7*env,void*x,void*y,struct Cyc_Dict_Dict*acc){
# 487
struct _tuple7*_tmpD0=env;int(*_tmpD2)(void*,void*);struct _RegionHandle*_tmpD1;_LL3E: _tmpD2=_tmpD0->f1;_tmpD1=_tmpD0->f2;_LL3F:;
if(_tmpD2(x,y))
*acc=Cyc_Dict_insert(*acc,x,y);
# 491
return acc;}
# 494
struct Cyc_Dict_Dict Cyc_Dict_rfilter(struct _RegionHandle*r2,int(*f)(void*,void*),struct Cyc_Dict_Dict d){
# 496
struct _tuple7 _tmpD3=({struct _tuple7 _tmpD5;_tmpD5.f1=f;_tmpD5.f2=r2;_tmpD5;});
struct Cyc_Dict_Dict _tmpD4=Cyc_Dict_rempty(r2,d.rel);
return*((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*f)(struct _tuple7*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple7*env,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*accum))Cyc_Dict_fold_c)(Cyc_Dict_filter_f,& _tmpD3,d,& _tmpD4);}
# 506
struct Cyc_Dict_Dict Cyc_Dict_filter(int(*f)(void*,void*),struct Cyc_Dict_Dict d){
return Cyc_Dict_rfilter(Cyc_Core_heap_region,f,d);}struct _tuple8{int(*f1)(void*,void*,void*);void*f2;struct _RegionHandle*f3;};
# 511
static struct Cyc_Dict_Dict*Cyc_Dict_filter_c_f(struct _tuple8*env,void*x,void*y,struct Cyc_Dict_Dict*acc){
# 515
struct _tuple8*_tmpD6=env;int(*_tmpD9)(void*,void*,void*);void*_tmpD8;struct _RegionHandle*_tmpD7;_LL41: _tmpD9=_tmpD6->f1;_tmpD8=_tmpD6->f2;_tmpD7=_tmpD6->f3;_LL42:;
if(_tmpD9(_tmpD8,x,y))
*acc=Cyc_Dict_insert(*acc,x,y);
return acc;}
# 521
struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(struct _RegionHandle*r2,int(*f)(void*,void*,void*),void*f_env,struct Cyc_Dict_Dict d){
# 523
struct _tuple8 _tmpDA=({struct _tuple8 _tmpDC;_tmpDC.f1=f;_tmpDC.f2=f_env;_tmpDC.f3=r2;_tmpDC;});
struct Cyc_Dict_Dict _tmpDB=Cyc_Dict_rempty(r2,d.rel);
return*((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*f)(struct _tuple8*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple8*env,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*accum))Cyc_Dict_fold_c)(Cyc_Dict_filter_c_f,& _tmpDA,d,& _tmpDB);}
# 527
struct Cyc_Dict_Dict Cyc_Dict_filter_c(int(*f)(void*,void*,void*),void*f_env,struct Cyc_Dict_Dict d){
# 529
return Cyc_Dict_rfilter_c(Cyc_Core_heap_region,f,f_env,d);}
# 533
static int Cyc_Dict_difference_f(struct Cyc_Dict_Dict*d,void*x,void*y){
return !Cyc_Dict_member(*d,x);}
# 537
struct Cyc_Dict_Dict Cyc_Dict_rdifference(struct _RegionHandle*r2,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 539
return((struct Cyc_Dict_Dict(*)(struct _RegionHandle*r2,int(*f)(struct Cyc_Dict_Dict*,void*,void*),struct Cyc_Dict_Dict*f_env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(r2,Cyc_Dict_difference_f,& d2,d1);}
# 541
struct Cyc_Dict_Dict Cyc_Dict_difference(struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 543
return Cyc_Dict_rdifference(Cyc_Core_heap_region,d1,d2);}struct _tuple9{int(*f1)(void*,void*);void*f2;};
# 546
static int Cyc_Dict_delete_f(struct _tuple9*env,void*x,void*y){
struct _tuple9*_tmpDD=env;int(*_tmpDF)(void*,void*);void*_tmpDE;_LL44: _tmpDF=_tmpDD->f1;_tmpDE=_tmpDD->f2;_LL45:;
return _tmpDF(_tmpDE,x)!= 0;}
# 551
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*r2,struct Cyc_Dict_Dict d,void*x){
if(!Cyc_Dict_member(d,x))return Cyc_Dict_rcopy(r2,d);{
struct _tuple9 _tmpE0=({struct _tuple9 _tmpE1;_tmpE1.f1=d.rel;_tmpE1.f2=x;_tmpE1;});
return((struct Cyc_Dict_Dict(*)(struct _RegionHandle*r2,int(*f)(struct _tuple9*,void*,void*),struct _tuple9*f_env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(r2,Cyc_Dict_delete_f,& _tmpE0,d);};}
# 557
struct Cyc_Dict_Dict Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict d,void*x){
if(!Cyc_Dict_member(d,x))return d;{
struct _tuple9 _tmpE2=({struct _tuple9 _tmpE3;_tmpE3.f1=d.rel;_tmpE3.f2=x;_tmpE3;});
return((struct Cyc_Dict_Dict(*)(struct _RegionHandle*r2,int(*f)(struct _tuple9*,void*,void*),struct _tuple9*f_env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(d.r,Cyc_Dict_delete_f,& _tmpE2,d);};}
# 563
struct Cyc_Dict_Dict Cyc_Dict_delete(struct Cyc_Dict_Dict d,void*x){
return Cyc_Dict_rdelete(Cyc_Core_heap_region,d,x);}struct _tuple10{struct _dyneither_ptr f1;int f2;};
# 567
int Cyc_Dict_iter_f(struct _tuple10*stk,struct _tuple0*dest){
# 570
struct _tuple10*_tmpE4=stk;struct _dyneither_ptr _tmpE8;int*_tmpE7;_LL47: _tmpE8=_tmpE4->f1;_tmpE7=(int*)& _tmpE4->f2;_LL48:;{
int _tmpE5=*_tmpE7;
if(_tmpE5 == - 1)
return 0;{
const struct Cyc_Dict_T*_tmpE6=*((const struct Cyc_Dict_T**)_check_dyneither_subscript(_tmpE8,sizeof(const struct Cyc_Dict_T*),_tmpE5));
*dest=((const struct Cyc_Dict_T*)_check_null(_tmpE6))->key_val;
-- _tmpE5;
if((unsigned int)_tmpE6->left)
*((const struct Cyc_Dict_T**)_check_dyneither_subscript(_tmpE8,sizeof(const struct Cyc_Dict_T*),++ _tmpE5))=_tmpE6->left;
if((unsigned int)_tmpE6->right)
*((const struct Cyc_Dict_T**)_check_dyneither_subscript(_tmpE8,sizeof(const struct Cyc_Dict_T*),++ _tmpE5))=_tmpE6->right;
*_tmpE7=_tmpE5;
return 1;};};}
# 585
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d){
# 590
int half_max_size=1;
const struct Cyc_Dict_T*_tmpE9=d.t;
while(_tmpE9 != (const struct Cyc_Dict_T*)0){
_tmpE9=_tmpE9->left;
++ half_max_size;}
# 596
_tmpE9=d.t;{
struct _dyneither_ptr _tmpEA=({unsigned int _tmpED=(unsigned int)(2 * half_max_size);const struct Cyc_Dict_T**_tmpEE=(const struct Cyc_Dict_T**)_region_malloc(rgn,_check_times(sizeof(const struct Cyc_Dict_T*),_tmpED));struct _dyneither_ptr _tmpF0=_tag_dyneither(_tmpEE,sizeof(const struct Cyc_Dict_T*),_tmpED);{unsigned int _tmpEF=_tmpED;unsigned int i;for(i=0;i < _tmpEF;i ++){_tmpEE[i]=(const struct Cyc_Dict_T*)_tmpE9;}}_tmpF0;});
return({struct Cyc_Iter_Iter _tmpEB;_tmpEB.env=(void*)({struct _tuple10*_tmpEC=_region_malloc(rgn,sizeof(*_tmpEC));_tmpEC->f1=_tmpEA;_tmpEC->f2=(unsigned int)_tmpE9?0: - 1;_tmpEC;});_tmpEB.next=(int(*)(void*env,void*dest))Cyc_Dict_iter_f;_tmpEB;});};}
# 606
void*Cyc_Dict_marshal(struct _RegionHandle*rgn,void*env,void*(*write_key)(void*,struct Cyc___cycFILE*,void*),void*(*write_val)(void*,struct Cyc___cycFILE*,void*),struct Cyc___cycFILE*fp,struct Cyc_Dict_Dict dict){
# 613
struct Cyc_List_List*dict_list=Cyc_Dict_rto_list(rgn,dict);
int len=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dict_list);
# 617
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1[0]=({struct Cyc_Core_Failure_exn_struct _tmpF2;_tmpF2.tag=Cyc_Core_Failure;_tmpF2.f1=({const char*_tmpF3="Dict::marshal: Write failure";_tag_dyneither(_tmpF3,sizeof(char),29);});_tmpF2;});_tmpF1;}));
# 619
while(dict_list != 0){
env=((void*(*)(void*,struct Cyc___cycFILE*,struct _tuple0*))write_key)(env,fp,(struct _tuple0*)((struct Cyc_List_List*)_check_null(dict_list))->hd);
env=((void*(*)(void*,struct Cyc___cycFILE*,struct _tuple0*))write_val)(env,fp,(struct _tuple0*)((struct Cyc_List_List*)_check_null(dict_list))->hd);
dict_list=((struct Cyc_List_List*)_check_null(dict_list))->tl;}
# 624
return env;}
# 627
struct Cyc_Dict_Dict Cyc_Dict_unmarshal(struct _RegionHandle*rgn,void*env,int(*cmp)(void*,void*),void*(*read_key)(void*,struct Cyc___cycFILE*),void*(*read_val)(void*,struct Cyc___cycFILE*),struct Cyc___cycFILE*fp){
# 634
struct Cyc_Dict_Dict dict=Cyc_Dict_empty(cmp);
int len=Cyc_getw(fp);
if(len == - 1)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4[0]=({struct Cyc_Core_Failure_exn_struct _tmpF5;_tmpF5.tag=Cyc_Core_Failure;_tmpF5.f1=({const char*_tmpF6="Dict::unmarshal: list length is -1";_tag_dyneither(_tmpF6,sizeof(char),35);});_tmpF5;});_tmpF4;}));
# 639
{int i=0;for(0;i < len;++ i){
void*key=read_key(env,fp);
void*val=read_val(env,fp);
dict=Cyc_Dict_insert(dict,key,val);}}
# 644
return dict;}
