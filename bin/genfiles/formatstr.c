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
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 290 "cycboot.h"
int isdigit(int);
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170 "core.h"
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
typedef struct Cyc_List_List*Cyc_List_List_t;extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 135 "string.h"
struct _dyneither_ptr Cyc_implode(struct Cyc_List_List*c);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;struct Cyc_Relations_Reln;
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 102
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 367
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 372
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 443 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 510
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 923 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 925
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 939
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 948
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 953
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;
# 955
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;
# 957
void*Cyc_Absyn_float_typ(int);
# 989
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 994
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);struct Cyc_RgnOrder_RgnPO;
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
# 84
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 96
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 32 "tcutil.h"
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 111
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 30 "formatstr.h"
struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
# 33
struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);struct _tuple10{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;char f5;int f6;};
# 36 "formatstr.cyc"
struct Cyc_Core_Opt*Cyc_Formatstr_parse_conversionspecification(struct _RegionHandle*r,struct _dyneither_ptr s,int i){
# 40
unsigned long _tmp0=Cyc_strlen((struct _dyneither_ptr)s);
if(i < 0  || i >= _tmp0)return 0;{
# 44
struct Cyc_List_List*_tmp1=0;
char c=' ';
for(0;i < _tmp0;++ i){
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
{char _tmp2=c;switch(_tmp2){case 43: _LL1: _LL2:
 goto _LL4;case 45: _LL3: _LL4:
 goto _LL6;case 32: _LL5: _LL6:
 goto _LL8;case 35: _LL7: _LL8:
 goto _LLA;case 48: _LL9: _LLA:
({struct Cyc_List_List*_tmp100=({struct Cyc_List_List*_tmp3=_region_malloc(r,sizeof(*_tmp3));_tmp3->hd=(void*)((int)c);_tmp3->tl=_tmp1;_tmp3;});_tmp1=_tmp100;});continue;default: _LLB: _LLC:
 goto _LL0;}_LL0:;}
# 56
break;}
# 58
if(i >= _tmp0)return 0;
({struct Cyc_List_List*_tmp101=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1);_tmp1=_tmp101;});{
# 62
struct Cyc_List_List*_tmp4=0;
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(c == '*'){
({struct Cyc_List_List*_tmp102=({struct Cyc_List_List*_tmp5=_region_malloc(r,sizeof(*_tmp5));_tmp5->hd=(void*)((int)c);_tmp5->tl=_tmp4;_tmp5;});_tmp4=_tmp102;});
++ i;}else{
# 68
for(0;i < _tmp0;++ i){
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if( isdigit((int)c))({struct Cyc_List_List*_tmp103=({struct Cyc_List_List*_tmp6=_region_malloc(r,sizeof(*_tmp6));_tmp6->hd=(void*)((int)c);_tmp6->tl=_tmp4;_tmp6;});_tmp4=_tmp103;});else{
break;}}}
# 74
if(i >= _tmp0)return 0;
({struct Cyc_List_List*_tmp104=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4);_tmp4=_tmp104;});{
# 78
struct Cyc_List_List*_tmp7=0;
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(c == '.'){
({struct Cyc_List_List*_tmp105=({struct Cyc_List_List*_tmp8=_region_malloc(r,sizeof(*_tmp8));_tmp8->hd=(void*)((int)c);_tmp8->tl=_tmp7;_tmp8;});_tmp7=_tmp105;});
++ i;
if(i >= _tmp0)return 0;
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(c == '*'){
({struct Cyc_List_List*_tmp106=({struct Cyc_List_List*_tmp9=_region_malloc(r,sizeof(*_tmp9));_tmp9->hd=(void*)((int)c);_tmp9->tl=_tmp7;_tmp9;});_tmp7=_tmp106;});
++ i;}else{
# 89
for(0;i < _tmp0;++ i){
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if( isdigit((int)c))({struct Cyc_List_List*_tmp107=({struct Cyc_List_List*_tmpA=_region_malloc(r,sizeof(*_tmpA));_tmpA->hd=(void*)((int)c);_tmpA->tl=_tmp7;_tmpA;});_tmp7=_tmp107;});else{
break;}}}}
# 95
if(i >= _tmp0)return 0;
({struct Cyc_List_List*_tmp108=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp7);_tmp7=_tmp108;});{
# 100
struct Cyc_List_List*_tmpB=0;
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
{char _tmpC=c;switch(_tmpC){case 104: _LLE: _LLF:
# 104
({struct Cyc_List_List*_tmp109=({struct Cyc_List_List*_tmpD=_region_malloc(r,sizeof(*_tmpD));_tmpD->hd=(void*)((int)c);_tmpD->tl=_tmpB;_tmpD;});_tmpB=_tmp109;});
++ i;
if(i >= _tmp0)return 0;
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(c == 'h'){({struct Cyc_List_List*_tmp10A=({struct Cyc_List_List*_tmpE=_region_malloc(r,sizeof(*_tmpE));_tmpE->hd=(void*)((int)c);_tmpE->tl=_tmpB;_tmpE;});_tmpB=_tmp10A;});++ i;}
goto _LLD;case 108: _LL10: _LL11:
# 111
({struct Cyc_List_List*_tmp10B=({struct Cyc_List_List*_tmpF=_region_malloc(r,sizeof(*_tmpF));_tmpF->hd=(void*)((int)c);_tmpF->tl=_tmpB;_tmpF;});_tmpB=_tmp10B;});
++ i;
if(i >= _tmp0)return 0;
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(c == 'l'){({struct Cyc_List_List*_tmp10C=({struct Cyc_List_List*_tmp10=_region_malloc(r,sizeof(*_tmp10));_tmp10->hd=(void*)((int)c);_tmp10->tl=_tmpB;_tmp10;});_tmpB=_tmp10C;});++ i;}
goto _LLD;case 106: _LL12: _LL13:
 goto _LL15;case 122: _LL14: _LL15:
 goto _LL17;case 116: _LL16: _LL17:
 goto _LL19;case 76: _LL18: _LL19:
# 121
({struct Cyc_List_List*_tmp10D=({struct Cyc_List_List*_tmp11=_region_malloc(r,sizeof(*_tmp11));_tmp11->hd=(void*)((int)c);_tmp11->tl=_tmpB;_tmp11;});_tmpB=_tmp10D;});
++ i;
goto _LLD;default: _LL1A: _LL1B:
 goto _LLD;}_LLD:;}
# 126
if(i >= _tmp0)return 0;
({struct Cyc_List_List*_tmp10E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpB);_tmpB=_tmp10E;});
# 130
c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
{char _tmp12=c;switch(_tmp12){case 100: _LL1D: _LL1E:
 goto _LL20;case 105: _LL1F: _LL20:
 goto _LL22;case 111: _LL21: _LL22:
 goto _LL24;case 117: _LL23: _LL24:
 goto _LL26;case 120: _LL25: _LL26:
 goto _LL28;case 88: _LL27: _LL28:
 goto _LL2A;case 102: _LL29: _LL2A:
 goto _LL2C;case 70: _LL2B: _LL2C:
 goto _LL2E;case 101: _LL2D: _LL2E:
 goto _LL30;case 69: _LL2F: _LL30:
 goto _LL32;case 103: _LL31: _LL32:
 goto _LL34;case 71: _LL33: _LL34:
 goto _LL36;case 97: _LL35: _LL36:
 goto _LL38;case 65: _LL37: _LL38:
 goto _LL3A;case 99: _LL39: _LL3A:
 goto _LL3C;case 115: _LL3B: _LL3C:
 goto _LL3E;case 112: _LL3D: _LL3E:
 goto _LL40;case 110: _LL3F: _LL40:
 goto _LL42;case 37: _LL41: _LL42:
 goto _LL1C;default: _LL43: _LL44:
 return 0;}_LL1C:;}
# 162 "formatstr.cyc"
return({struct Cyc_Core_Opt*_tmp13=_region_malloc(r,sizeof(*_tmp13));({struct _tuple10*_tmp10F=({struct _tuple10*_tmp14=_region_malloc(r,sizeof(*_tmp14));_tmp14->f1=_tmp1;_tmp14->f2=_tmp4;_tmp14->f3=_tmp7;_tmp14->f4=_tmpB;_tmp14->f5=c;_tmp14->f6=i + 1;_tmp14;});_tmp13->v=_tmp10F;});_tmp13;});};};};};}
# 165
struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr s,unsigned int loc){
# 168
unsigned long _tmp15=Cyc_strlen((struct _dyneither_ptr)s);
struct Cyc_List_List*_tmp16=0;
int i;
struct _RegionHandle _tmp17=_new_region("temp");struct _RegionHandle*temp=& _tmp17;_push_region(temp);
for(i=0;i < _tmp15;++ i){
if(*((const char*)_check_dyneither_subscript(s,sizeof(char),i))!= '%')continue;{
struct Cyc_Core_Opt*_tmp18=Cyc_Formatstr_parse_conversionspecification(temp,s,i + 1);
if(_tmp18 == 0){
({void*_tmp19=0;({unsigned int _tmp111=loc;struct _dyneither_ptr _tmp110=({const char*_tmp1A="bad format string";_tag_dyneither(_tmp1A,sizeof(char),18);});Cyc_Tcutil_terr(_tmp111,_tmp110,_tag_dyneither(_tmp19,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp1B=0;_npop_handler(0);return _tmp1B;};}{
# 179
struct _tuple10*_tmp1C=(struct _tuple10*)_tmp18->v;struct _tuple10*_tmp1D=_tmp1C;struct Cyc_List_List*_tmp93;struct Cyc_List_List*_tmp92;struct Cyc_List_List*_tmp91;struct Cyc_List_List*_tmp90;char _tmp8F;int _tmp8E;_LL46: _tmp93=_tmp1D->f1;_tmp92=_tmp1D->f2;_tmp91=_tmp1D->f3;_tmp90=_tmp1D->f4;_tmp8F=_tmp1D->f5;_tmp8E=_tmp1D->f6;_LL47:;
i=_tmp8E - 1;
{struct Cyc_List_List*_tmp1E=_tmp90;int _tmp23;if(_tmp1E != 0){if(((struct Cyc_List_List*)_tmp1E)->tl == 0){_LL49: _tmp23=(int)_tmp1E->hd;if(
(_tmp23 == 'j'  || _tmp23 == 'z') || _tmp23 == 't'){_LL4A:
# 185
({struct Cyc_Int_pa_PrintArg_struct _tmp21;_tmp21.tag=1;_tmp21.f1=(unsigned long)_tmp23;({void*_tmp1F[1]={& _tmp21};({unsigned int _tmp113=loc;struct _dyneither_ptr _tmp112=({const char*_tmp20="length modifier '%c' is not supported";_tag_dyneither(_tmp20,sizeof(char),38);});Cyc_Tcutil_terr(_tmp113,_tmp112,_tag_dyneither(_tmp1F,sizeof(void*),1));});});});{
struct Cyc_List_List*_tmp22=0;_npop_handler(0);return _tmp22;};}else{goto _LL4B;}}else{goto _LL4B;}}else{_LL4B: _LL4C:
 goto _LL48;}_LL48:;}
# 189
{struct Cyc_List_List*_tmp24=_tmp92;int _tmp26;if(_tmp24 != 0){if(((struct Cyc_List_List*)_tmp24)->tl == 0){_LL4E: _tmp26=(int)_tmp24->hd;if(_tmp26 == '*'){_LL4F:
({struct Cyc_List_List*_tmp114=({struct Cyc_List_List*_tmp25=_region_malloc(r,sizeof(*_tmp25));_tmp25->hd=Cyc_Absyn_sint_typ;_tmp25->tl=_tmp16;_tmp25;});_tmp16=_tmp114;});goto _LL4D;}else{goto _LL50;}}else{goto _LL50;}}else{_LL50: _LL51:
 goto _LL4D;}_LL4D:;}
# 193
{struct Cyc_List_List*_tmp27=_tmp91;int _tmp2A;int _tmp29;if(_tmp27 != 0){if(((struct Cyc_List_List*)_tmp27)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmp27)->tl)->tl == 0){_LL53: _tmp2A=(int)_tmp27->hd;_tmp29=(int)(_tmp27->tl)->hd;if(
_tmp2A == '.'  && _tmp29 == '*'){_LL54:
({struct Cyc_List_List*_tmp115=({struct Cyc_List_List*_tmp28=_region_malloc(r,sizeof(*_tmp28));_tmp28->hd=Cyc_Absyn_sint_typ;_tmp28->tl=_tmp16;_tmp28;});_tmp16=_tmp115;});goto _LL52;}else{goto _LL55;}}else{goto _LL55;}}else{goto _LL55;}}else{_LL55: _LL56:
 goto _LL52;}_LL52:;}{
# 198
void*t;
char _tmp2B=_tmp8F;switch(_tmp2B){case 100: _LL58: _LL59:
 goto _LL5B;case 105: _LL5A: _LL5B:
# 202
{struct Cyc_List_List*f=_tmp93;for(0;f != 0;f=f->tl){
if((int)f->hd == '#'){
({struct Cyc_Int_pa_PrintArg_struct _tmp2E;_tmp2E.tag=1;_tmp2E.f1=(unsigned long)((int)_tmp8F);({void*_tmp2C[1]={& _tmp2E};({unsigned int _tmp117=loc;struct _dyneither_ptr _tmp116=({const char*_tmp2D="flag '#' is not valid with %%%c";_tag_dyneither(_tmp2D,sizeof(char),32);});Cyc_Tcutil_terr(_tmp117,_tmp116,_tag_dyneither(_tmp2C,sizeof(void*),1));});});});{
struct Cyc_List_List*_tmp2F=0;_npop_handler(0);return _tmp2F;};}}}
# 207
{struct Cyc_List_List*_tmp30=_tmp90;int _tmp39;int _tmp38;int _tmp37;int _tmp36;if(_tmp30 == 0){_LL81: _LL82:
 t=Cyc_Absyn_sint_typ;goto _LL80;}else{if(((struct Cyc_List_List*)_tmp30)->tl == 0){_LL83: _tmp36=(int)_tmp30->hd;if(_tmp36 == 'l'){_LL84:
 t=Cyc_Absyn_slong_typ;goto _LL80;}else{_LL85: _tmp37=(int)_tmp30->hd;if(_tmp37 == 'h'){_LL86:
 t=Cyc_Absyn_sshort_typ;goto _LL80;}else{goto _LL89;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmp30)->tl)->tl == 0){_LL87: _tmp39=(int)_tmp30->hd;_tmp38=(int)(_tmp30->tl)->hd;if(
_tmp39 == 'h'  && _tmp38 == 'h'){_LL88:
 t=Cyc_Absyn_schar_typ;goto _LL80;}else{goto _LL89;}}else{_LL89: _LL8A:
# 214
({struct Cyc_Int_pa_PrintArg_struct _tmp34;_tmp34.tag=1;_tmp34.f1=(unsigned long)((int)_tmp8F);({struct Cyc_String_pa_PrintArg_struct _tmp33;_tmp33.tag=0;({struct _dyneither_ptr _tmp118=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmp90));_tmp33.f1=_tmp118;});({void*_tmp31[2]={& _tmp33,& _tmp34};({unsigned int _tmp11A=loc;struct _dyneither_ptr _tmp119=({const char*_tmp32="length modifier '%s' is not allowed with %%%c";_tag_dyneither(_tmp32,sizeof(char),46);});Cyc_Tcutil_terr(_tmp11A,_tmp119,_tag_dyneither(_tmp31,sizeof(void*),2));});});});});{
struct Cyc_List_List*_tmp35=0;_npop_handler(0);return _tmp35;};}}}_LL80:;}
# 218
({struct Cyc_List_List*_tmp11B=({struct Cyc_List_List*_tmp3A=_region_malloc(r,sizeof(*_tmp3A));_tmp3A->hd=t;_tmp3A->tl=_tmp16;_tmp3A;});_tmp16=_tmp11B;});
goto _LL57;case 117: _LL5C: _LL5D:
# 221
{struct Cyc_List_List*f=_tmp93;for(0;f != 0;f=f->tl){
if((int)f->hd == '#'){
({void*_tmp3B=0;({unsigned int _tmp11D=loc;struct _dyneither_ptr _tmp11C=({const char*_tmp3C="Flag '#' not valid with %%u";_tag_dyneither(_tmp3C,sizeof(char),28);});Cyc_Tcutil_terr(_tmp11D,_tmp11C,_tag_dyneither(_tmp3B,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp3D=0;_npop_handler(0);return _tmp3D;};}}}
# 226
goto _LL5F;case 111: _LL5E: _LL5F:
 goto _LL61;case 120: _LL60: _LL61:
 goto _LL63;case 88: _LL62: _LL63:
# 230
{struct Cyc_List_List*_tmp3E=_tmp90;int _tmp47;int _tmp46;int _tmp45;int _tmp44;if(_tmp3E == 0){_LL8C: _LL8D:
 t=Cyc_Absyn_uint_typ;goto _LL8B;}else{if(((struct Cyc_List_List*)_tmp3E)->tl == 0){_LL8E: _tmp44=(int)_tmp3E->hd;if(_tmp44 == 'l'){_LL8F:
 t=Cyc_Absyn_ulong_typ;goto _LL8B;}else{_LL90: _tmp45=(int)_tmp3E->hd;if(_tmp45 == 'h'){_LL91:
 t=Cyc_Absyn_ushort_typ;goto _LL8B;}else{goto _LL94;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmp3E)->tl)->tl == 0){_LL92: _tmp47=(int)_tmp3E->hd;_tmp46=(int)(_tmp3E->tl)->hd;if(
_tmp47 == 'h'  && _tmp46 == 'h'){_LL93:
 t=Cyc_Absyn_uchar_typ;goto _LL8B;}else{goto _LL94;}}else{_LL94: _LL95:
# 238
({struct Cyc_Int_pa_PrintArg_struct _tmp42;_tmp42.tag=1;_tmp42.f1=(unsigned long)((int)_tmp8F);({struct Cyc_String_pa_PrintArg_struct _tmp41;_tmp41.tag=0;({struct _dyneither_ptr _tmp11E=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmp90));_tmp41.f1=_tmp11E;});({void*_tmp3F[2]={& _tmp41,& _tmp42};({unsigned int _tmp120=loc;struct _dyneither_ptr _tmp11F=({const char*_tmp40="length modifier '%s' is not allowed with %%%c";_tag_dyneither(_tmp40,sizeof(char),46);});Cyc_Tcutil_terr(_tmp120,_tmp11F,_tag_dyneither(_tmp3F,sizeof(void*),2));});});});});{
struct Cyc_List_List*_tmp43=0;_npop_handler(0);return _tmp43;};}}}_LL8B:;}
# 242
({struct Cyc_List_List*_tmp121=({struct Cyc_List_List*_tmp48=_region_malloc(r,sizeof(*_tmp48));_tmp48->hd=t;_tmp48->tl=_tmp16;_tmp48;});_tmp16=_tmp121;});
goto _LL57;case 102: _LL64: _LL65:
 goto _LL67;case 70: _LL66: _LL67:
 goto _LL69;case 101: _LL68: _LL69:
 goto _LL6B;case 69: _LL6A: _LL6B:
 goto _LL6D;case 103: _LL6C: _LL6D:
 goto _LL6F;case 71: _LL6E: _LL6F:
 goto _LL71;case 97: _LL70: _LL71:
 goto _LL73;case 65: _LL72: _LL73:
# 258
{struct Cyc_List_List*_tmp49=_tmp90;int _tmp51;if(_tmp49 == 0){_LL97: _LL98:
({struct Cyc_List_List*_tmp123=({struct Cyc_List_List*_tmp4A=_region_malloc(r,sizeof(*_tmp4A));({void*_tmp122=Cyc_Absyn_float_typ(1);_tmp4A->hd=_tmp122;});_tmp4A->tl=_tmp16;_tmp4A;});_tmp16=_tmp123;});goto _LL96;}else{if(((struct Cyc_List_List*)_tmp49)->tl == 0){_LL99: _tmp51=(int)_tmp49->hd;if(_tmp51 == 'l'){_LL9A:
# 261
({struct Cyc_List_List*_tmp125=({struct Cyc_List_List*_tmp4B=_region_malloc(r,sizeof(*_tmp4B));({void*_tmp124=Cyc_Absyn_float_typ(2);_tmp4B->hd=_tmp124;});_tmp4B->tl=_tmp16;_tmp4B;});_tmp16=_tmp125;});goto _LL96;}else{goto _LL9B;}}else{_LL9B: _LL9C:
# 263
({struct Cyc_Int_pa_PrintArg_struct _tmp4F;_tmp4F.tag=1;_tmp4F.f1=(unsigned long)((int)_tmp8F);({struct Cyc_String_pa_PrintArg_struct _tmp4E;_tmp4E.tag=0;({struct _dyneither_ptr _tmp126=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmp90));_tmp4E.f1=_tmp126;});({void*_tmp4C[2]={& _tmp4E,& _tmp4F};({unsigned int _tmp128=loc;struct _dyneither_ptr _tmp127=({const char*_tmp4D="length modifier '%s' is not allowed with %%%c";_tag_dyneither(_tmp4D,sizeof(char),46);});Cyc_Tcutil_terr(_tmp128,_tmp127,_tag_dyneither(_tmp4C,sizeof(void*),2));});});});});{
struct Cyc_List_List*_tmp50=0;_npop_handler(0);return _tmp50;};}}_LL96:;}
# 267
goto _LL57;case 99: _LL74: _LL75:
# 269
{struct Cyc_List_List*f=_tmp93;for(0;f != 0;f=f->tl){
if((int)f->hd == '#'  || (int)f->hd == '0'){
({struct Cyc_Int_pa_PrintArg_struct _tmp54;_tmp54.tag=1;_tmp54.f1=(unsigned long)((int)f->hd);({void*_tmp52[1]={& _tmp54};({unsigned int _tmp12A=loc;struct _dyneither_ptr _tmp129=({const char*_tmp53="flag '%c' not allowed with %%c";_tag_dyneither(_tmp53,sizeof(char),31);});Cyc_Tcutil_terr(_tmp12A,_tmp129,_tag_dyneither(_tmp52,sizeof(void*),1));});});});{
struct Cyc_List_List*_tmp55=0;_npop_handler(0);return _tmp55;};}}}
# 276
if(_tmp90 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp58;_tmp58.tag=0;({struct _dyneither_ptr _tmp12B=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmp90));_tmp58.f1=_tmp12B;});({void*_tmp56[1]={& _tmp58};({unsigned int _tmp12D=loc;struct _dyneither_ptr _tmp12C=({const char*_tmp57="length modifier '%s' not allowed with %%c";_tag_dyneither(_tmp57,sizeof(char),42);});Cyc_Tcutil_terr(_tmp12D,_tmp12C,_tag_dyneither(_tmp56,sizeof(void*),1));});});});{
struct Cyc_List_List*_tmp59=0;_npop_handler(0);return _tmp59;};}
# 281
if(_tmp91 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp5C;_tmp5C.tag=0;({struct _dyneither_ptr _tmp12E=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmp91));_tmp5C.f1=_tmp12E;});({void*_tmp5A[1]={& _tmp5C};({unsigned int _tmp130=loc;struct _dyneither_ptr _tmp12F=({const char*_tmp5B="precision '%s' not allowed with %%c";_tag_dyneither(_tmp5B,sizeof(char),36);});Cyc_Tcutil_terr(_tmp130,_tmp12F,_tag_dyneither(_tmp5A,sizeof(void*),1));});});});{
struct Cyc_List_List*_tmp5D=0;_npop_handler(0);return _tmp5D;};}
# 286
({struct Cyc_List_List*_tmp131=({struct Cyc_List_List*_tmp5E=_region_malloc(r,sizeof(*_tmp5E));_tmp5E->hd=Cyc_Absyn_sint_typ;_tmp5E->tl=_tmp16;_tmp5E;});_tmp16=_tmp131;});
goto _LL57;case 115: _LL76: _LL77:
# 290
{struct Cyc_List_List*f=_tmp93;for(0;f != 0;f=f->tl){
if((int)f->hd != '-'){
({void*_tmp5F=0;({unsigned int _tmp133=loc;struct _dyneither_ptr _tmp132=({const char*_tmp60="a flag not allowed with %%s";_tag_dyneither(_tmp60,sizeof(char),28);});Cyc_Tcutil_terr(_tmp133,_tmp132,_tag_dyneither(_tmp5F,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp61=0;_npop_handler(0);return _tmp61;};}}}
# 297
if(_tmp90 != 0){
({void*_tmp62=0;({unsigned int _tmp135=loc;struct _dyneither_ptr _tmp134=({const char*_tmp63="length modifiers not allowed with %%s";_tag_dyneither(_tmp63,sizeof(char),38);});Cyc_Tcutil_terr(_tmp135,_tmp134,_tag_dyneither(_tmp62,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp64=0;_npop_handler(0);return _tmp64;};}{
# 303
void*ptr=({void*_tmp139=Cyc_Absyn_char_typ;void*_tmp138=({
Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,({struct Cyc_Core_Opt*_tmp66=_cycalloc(sizeof(*_tmp66));({struct Cyc_List_List*_tmp136=
Cyc_Tcenv_lookup_type_vars(te);_tmp66->v=_tmp136;});_tmp66;}));});
# 303
struct Cyc_Absyn_Tqual _tmp137=
# 306
Cyc_Absyn_const_tqual(0);
# 303
Cyc_Absyn_dyneither_typ(_tmp139,_tmp138,_tmp137,Cyc_Absyn_false_conref);});
# 307
({struct Cyc_List_List*_tmp13A=({struct Cyc_List_List*_tmp65=_region_malloc(r,sizeof(*_tmp65));_tmp65->hd=ptr;_tmp65->tl=_tmp16;_tmp65;});_tmp16=_tmp13A;});
goto _LL57;};case 112: _LL78: _LL79:
# 311
({struct Cyc_List_List*_tmp13B=({struct Cyc_List_List*_tmp67=_region_malloc(r,sizeof(*_tmp67));_tmp67->hd=Cyc_Absyn_uint_typ;_tmp67->tl=_tmp16;_tmp67;});_tmp16=_tmp13B;});
goto _LL57;case 110: _LL7A: _LL7B:
# 314
{struct Cyc_List_List*f=_tmp93;for(0;f != 0;f=f->tl){
if((int)f->hd == '#'  || (int)f->hd == '0'){
({struct Cyc_Int_pa_PrintArg_struct _tmp6A;_tmp6A.tag=1;_tmp6A.f1=(unsigned long)((int)f->hd);({void*_tmp68[1]={& _tmp6A};({unsigned int _tmp13D=loc;struct _dyneither_ptr _tmp13C=({const char*_tmp69="flag '%c' not allowed with %%n";_tag_dyneither(_tmp69,sizeof(char),31);});Cyc_Tcutil_terr(_tmp13D,_tmp13C,_tag_dyneither(_tmp68,sizeof(void*),1));});});});{
struct Cyc_List_List*_tmp6B=0;_npop_handler(0);return _tmp6B;};}}}
# 319
if(_tmp91 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp6E;_tmp6E.tag=0;({struct _dyneither_ptr _tmp13E=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmp91));_tmp6E.f1=_tmp13E;});({void*_tmp6C[1]={& _tmp6E};({unsigned int _tmp140=loc;struct _dyneither_ptr _tmp13F=({const char*_tmp6D="precision '%s' not allowed with %%n";_tag_dyneither(_tmp6D,sizeof(char),36);});Cyc_Tcutil_terr(_tmp140,_tmp13F,_tag_dyneither(_tmp6C,sizeof(void*),1));});});});{
struct Cyc_List_List*_tmp6F=0;_npop_handler(0);return _tmp6F;};}{
# 324
struct Cyc_Core_Opt*_tmp70=({struct Cyc_Core_Opt*_tmp7C=_cycalloc(sizeof(*_tmp7C));({struct Cyc_List_List*_tmp141=Cyc_Tcenv_lookup_type_vars(te);_tmp7C->v=_tmp141;});_tmp7C;});
{struct Cyc_List_List*_tmp71=_tmp90;int _tmp7A;int _tmp79;int _tmp78;int _tmp77;if(_tmp71 == 0){_LL9E: _LL9F:
 t=Cyc_Absyn_sint_typ;goto _LL9D;}else{if(((struct Cyc_List_List*)_tmp71)->tl == 0){_LLA0: _tmp77=(int)_tmp71->hd;if(_tmp77 == 'l'){_LLA1:
# 328
 t=Cyc_Absyn_ulong_typ;goto _LL9D;}else{_LLA2: _tmp78=(int)_tmp71->hd;if(_tmp78 == 'h'){_LLA3:
 t=Cyc_Absyn_sshort_typ;goto _LL9D;}else{goto _LLA6;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmp71)->tl)->tl == 0){_LLA4: _tmp7A=(int)_tmp71->hd;_tmp79=(int)(_tmp71->tl)->hd;if(
_tmp7A == 'h'  && _tmp79 == 'h'){_LLA5:
 t=Cyc_Absyn_schar_typ;goto _LL9D;}else{goto _LLA6;}}else{_LLA6: _LLA7:
# 333
({struct Cyc_Int_pa_PrintArg_struct _tmp75;_tmp75.tag=1;_tmp75.f1=(unsigned long)((int)_tmp8F);({struct Cyc_String_pa_PrintArg_struct _tmp74;_tmp74.tag=0;({struct _dyneither_ptr _tmp142=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmp90));_tmp74.f1=_tmp142;});({void*_tmp72[2]={& _tmp74,& _tmp75};({unsigned int _tmp144=loc;struct _dyneither_ptr _tmp143=({const char*_tmp73="length modifier '%s' is not allowed with %%%c";_tag_dyneither(_tmp73,sizeof(char),46);});Cyc_Tcutil_terr(_tmp144,_tmp143,_tag_dyneither(_tmp72,sizeof(void*),2));});});});});{
struct Cyc_List_List*_tmp76=0;_npop_handler(0);return _tmp76;};}}}_LL9D:;}
# 337
({void*_tmp148=({void*_tmp147=t;void*_tmp146=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,_tmp70);struct Cyc_Absyn_Tqual _tmp145=Cyc_Absyn_empty_tqual(0);Cyc_Absyn_at_typ(_tmp147,_tmp146,_tmp145,Cyc_Absyn_false_conref);});t=_tmp148;});
({struct Cyc_List_List*_tmp149=({struct Cyc_List_List*_tmp7B=_region_malloc(r,sizeof(*_tmp7B));_tmp7B->hd=t;_tmp7B->tl=_tmp16;_tmp7B;});_tmp16=_tmp149;});
goto _LL57;};case 37: _LL7C: _LL7D:
# 341
 if(_tmp93 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp7F;_tmp7F.tag=0;({struct _dyneither_ptr _tmp14A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmp93));_tmp7F.f1=_tmp14A;});({void*_tmp7D[1]={& _tmp7F};({unsigned int _tmp14C=loc;struct _dyneither_ptr _tmp14B=({const char*_tmp7E="flags '%s' not allowed with %%%%";_tag_dyneither(_tmp7E,sizeof(char),33);});Cyc_Tcutil_terr(_tmp14C,_tmp14B,_tag_dyneither(_tmp7D,sizeof(void*),1));});});});{
struct Cyc_List_List*_tmp80=0;_npop_handler(0);return _tmp80;};}
# 345
if(_tmp92 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp83;_tmp83.tag=0;({struct _dyneither_ptr _tmp14D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmp92));_tmp83.f1=_tmp14D;});({void*_tmp81[1]={& _tmp83};({unsigned int _tmp14F=loc;struct _dyneither_ptr _tmp14E=({const char*_tmp82="width '%s' not allowed with %%%%";_tag_dyneither(_tmp82,sizeof(char),33);});Cyc_Tcutil_terr(_tmp14F,_tmp14E,_tag_dyneither(_tmp81,sizeof(void*),1));});});});{
struct Cyc_List_List*_tmp84=0;_npop_handler(0);return _tmp84;};}
# 349
if(_tmp91 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp87;_tmp87.tag=0;({struct _dyneither_ptr _tmp150=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmp91));_tmp87.f1=_tmp150;});({void*_tmp85[1]={& _tmp87};({unsigned int _tmp152=loc;struct _dyneither_ptr _tmp151=({const char*_tmp86="precision '%s' not allowed with %%%%";_tag_dyneither(_tmp86,sizeof(char),37);});Cyc_Tcutil_terr(_tmp152,_tmp151,_tag_dyneither(_tmp85,sizeof(void*),1));});});});{
struct Cyc_List_List*_tmp88=0;_npop_handler(0);return _tmp88;};}
# 353
if(_tmp90 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp8B;_tmp8B.tag=0;({struct _dyneither_ptr _tmp153=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmp90));_tmp8B.f1=_tmp153;});({void*_tmp89[1]={& _tmp8B};({unsigned int _tmp155=loc;struct _dyneither_ptr _tmp154=({const char*_tmp8A="length modifier '%s' not allowed with %%%%";_tag_dyneither(_tmp8A,sizeof(char),43);});Cyc_Tcutil_terr(_tmp155,_tmp154,_tag_dyneither(_tmp89,sizeof(void*),1));});});});{
struct Cyc_List_List*_tmp8C=0;_npop_handler(0);return _tmp8C;};}
# 357
goto _LL57;default: _LL7E: _LL7F: {
# 359
struct Cyc_List_List*_tmp8D=0;_npop_handler(0);return _tmp8D;}}_LL57:;};};};}{
# 362
struct Cyc_List_List*_tmp94=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp16);_npop_handler(0);return _tmp94;};
# 172
;_pop_region(temp);}struct _tuple11{int f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;char f4;int f5;};
# 373 "formatstr.cyc"
struct Cyc_Core_Opt*Cyc_Formatstr_parse_inputformat(struct _RegionHandle*r,struct _dyneither_ptr s,int i){
# 375
unsigned long _tmp95=Cyc_strlen((struct _dyneither_ptr)s);
if(i < 0  || i >= _tmp95)return 0;{
# 378
int _tmp96=0;
char _tmp97=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmp97 == '*'){
_tmp96=1;
++ i;
if(i >= _tmp95)return 0;}{
# 386
struct Cyc_List_List*_tmp98=0;
for(0;i < _tmp95;++ i){
_tmp97=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if( isdigit((int)_tmp97))({struct Cyc_List_List*_tmp156=({struct Cyc_List_List*_tmp99=_region_malloc(r,sizeof(*_tmp99));_tmp99->hd=(void*)((int)_tmp97);_tmp99->tl=_tmp98;_tmp99;});_tmp98=_tmp156;});else{
break;}}
# 392
if(i >= _tmp95)return 0;
({struct Cyc_List_List*_tmp157=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp98);_tmp98=_tmp157;});{
# 397
struct Cyc_List_List*_tmp9A=0;
_tmp97=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
{char _tmp9B=_tmp97;switch(_tmp9B){case 104: _LLA9: _LLAA:
# 401
({struct Cyc_List_List*_tmp158=({struct Cyc_List_List*_tmp9C=_region_malloc(r,sizeof(*_tmp9C));_tmp9C->hd=(void*)((int)_tmp97);_tmp9C->tl=_tmp9A;_tmp9C;});_tmp9A=_tmp158;});
++ i;
if(i >= _tmp95)return 0;
_tmp97=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmp97 == 'h'){({struct Cyc_List_List*_tmp159=({struct Cyc_List_List*_tmp9D=_region_malloc(r,sizeof(*_tmp9D));_tmp9D->hd=(void*)((int)_tmp97);_tmp9D->tl=_tmp9A;_tmp9D;});_tmp9A=_tmp159;});++ i;}
goto _LLA8;case 108: _LLAB: _LLAC:
# 408
({struct Cyc_List_List*_tmp15A=({struct Cyc_List_List*_tmp9E=_region_malloc(r,sizeof(*_tmp9E));_tmp9E->hd=(void*)((int)_tmp97);_tmp9E->tl=_tmp9A;_tmp9E;});_tmp9A=_tmp15A;});
++ i;
if(i >= _tmp95)return 0;
_tmp97=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmp97 == 'l'){({struct Cyc_List_List*_tmp15B=({struct Cyc_List_List*_tmp9F=_region_malloc(r,sizeof(*_tmp9F));_tmp9F->hd=(void*)((int)_tmp97);_tmp9F->tl=_tmp9A;_tmp9F;});_tmp9A=_tmp15B;});++ i;}
goto _LLA8;case 106: _LLAD: _LLAE:
 goto _LLB0;case 122: _LLAF: _LLB0:
 goto _LLB2;case 116: _LLB1: _LLB2:
 goto _LLB4;case 76: _LLB3: _LLB4:
# 418
({struct Cyc_List_List*_tmp15C=({struct Cyc_List_List*_tmpA0=_region_malloc(r,sizeof(*_tmpA0));_tmpA0->hd=(void*)((int)_tmp97);_tmpA0->tl=_tmp9A;_tmpA0;});_tmp9A=_tmp15C;});
++ i;
goto _LLA8;default: _LLB5: _LLB6:
 goto _LLA8;}_LLA8:;}
# 423
if(i >= _tmp95)return 0;
({struct Cyc_List_List*_tmp15D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp9A);_tmp9A=_tmp15D;});
# 427
_tmp97=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
{char _tmpA1=_tmp97;switch(_tmpA1){case 100: _LLB8: _LLB9:
 goto _LLBB;case 105: _LLBA: _LLBB:
 goto _LLBD;case 111: _LLBC: _LLBD:
 goto _LLBF;case 117: _LLBE: _LLBF:
 goto _LLC1;case 120: _LLC0: _LLC1:
 goto _LLC3;case 88: _LLC2: _LLC3:
 goto _LLC5;case 102: _LLC4: _LLC5:
 goto _LLC7;case 70: _LLC6: _LLC7:
 goto _LLC9;case 101: _LLC8: _LLC9:
 goto _LLCB;case 69: _LLCA: _LLCB:
 goto _LLCD;case 103: _LLCC: _LLCD:
 goto _LLCF;case 71: _LLCE: _LLCF:
 goto _LLD1;case 97: _LLD0: _LLD1:
 goto _LLD3;case 65: _LLD2: _LLD3:
 goto _LLD5;case 99: _LLD4: _LLD5:
 goto _LLD7;case 115: _LLD6: _LLD7:
 goto _LLD9;case 112: _LLD8: _LLD9:
 goto _LLDB;case 110: _LLDA: _LLDB:
 goto _LLDD;case 37: _LLDC: _LLDD:
 goto _LLB7;default: _LLDE: _LLDF:
 return 0;}_LLB7:;}
# 450
return({struct Cyc_Core_Opt*_tmpA2=_region_malloc(r,sizeof(*_tmpA2));({struct _tuple11*_tmp15E=({struct _tuple11*_tmpA3=_region_malloc(r,sizeof(*_tmpA3));_tmpA3->f1=_tmp96;_tmpA3->f2=_tmp98;_tmpA3->f3=_tmp9A;_tmpA3->f4=_tmp97;_tmpA3->f5=i + 1;_tmpA3;});_tmpA2->v=_tmp15E;});_tmpA2;});};};};}
# 452
struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr s,unsigned int loc){
# 455
unsigned long _tmpA4=Cyc_strlen((struct _dyneither_ptr)s);
struct Cyc_List_List*_tmpA5=0;
int i;
{struct _RegionHandle _tmpA6=_new_region("temp");struct _RegionHandle*temp=& _tmpA6;_push_region(temp);
for(i=0;i < _tmpA4;++ i){
if(*((const char*)_check_dyneither_subscript(s,sizeof(char),i))!= '%')continue;{
struct Cyc_Core_Opt*_tmpA7=Cyc_Formatstr_parse_inputformat(temp,s,i + 1);
if(_tmpA7 == 0){
({void*_tmpA8=0;({unsigned int _tmp160=loc;struct _dyneither_ptr _tmp15F=({const char*_tmpA9="bad format string";_tag_dyneither(_tmpA9,sizeof(char),18);});Cyc_Tcutil_terr(_tmp160,_tmp15F,_tag_dyneither(_tmpA8,sizeof(void*),0));});});{
struct Cyc_List_List*_tmpAA=0;_npop_handler(0);return _tmpAA;};}{
# 466
struct _tuple11*_tmpAB=(struct _tuple11*)_tmpA7->v;struct _tuple11*_tmpAC=_tmpAB;int _tmpFF;struct Cyc_List_List*_tmpFE;struct Cyc_List_List*_tmpFD;char _tmpFC;int _tmpFB;_LLE1: _tmpFF=_tmpAC->f1;_tmpFE=_tmpAC->f2;_tmpFD=_tmpAC->f3;_tmpFC=_tmpAC->f4;_tmpFB=_tmpAC->f5;_LLE2:;
i=_tmpFB - 1;
{struct Cyc_List_List*_tmpAD=_tmpFD;int _tmpB2;if(_tmpAD != 0){if(((struct Cyc_List_List*)_tmpAD)->tl == 0){_LLE4: _tmpB2=(int)_tmpAD->hd;if(
(_tmpB2 == 'j'  || _tmpB2 == 'z') || _tmpB2 == 't'){_LLE5:
# 471
({struct Cyc_Int_pa_PrintArg_struct _tmpB0;_tmpB0.tag=1;_tmpB0.f1=(unsigned long)_tmpB2;({void*_tmpAE[1]={& _tmpB0};({unsigned int _tmp162=loc;struct _dyneither_ptr _tmp161=({const char*_tmpAF="length modifier '%c' is not supported";_tag_dyneither(_tmpAF,sizeof(char),38);});Cyc_Tcutil_terr(_tmp162,_tmp161,_tag_dyneither(_tmpAE,sizeof(void*),1));});});});{
struct Cyc_List_List*_tmpB1=0;_npop_handler(0);return _tmpB1;};}else{goto _LLE6;}}else{goto _LLE6;}}else{_LLE6: _LLE7:
 goto _LLE3;}_LLE3:;}
# 475
if(_tmpFF)continue;{
void*t;
char _tmpB3=_tmpFC;switch(_tmpB3){case 100: _LLE9: _LLEA:
 goto _LLEC;case 105: _LLEB: _LLEC: {
# 480
struct Cyc_Core_Opt*_tmpB4=({struct Cyc_Core_Opt*_tmpC0=_cycalloc(sizeof(*_tmpC0));({struct Cyc_List_List*_tmp163=Cyc_Tcenv_lookup_type_vars(te);_tmpC0->v=_tmp163;});_tmpC0;});
{struct Cyc_List_List*_tmpB5=_tmpFD;int _tmpBE;int _tmpBD;int _tmpBC;int _tmpBB;if(_tmpB5 == 0){_LL114: _LL115:
 t=Cyc_Absyn_sint_typ;goto _LL113;}else{if(((struct Cyc_List_List*)_tmpB5)->tl == 0){_LL116: _tmpBB=(int)_tmpB5->hd;if(_tmpBB == 'l'){_LL117:
 t=Cyc_Absyn_slong_typ;goto _LL113;}else{_LL118: _tmpBC=(int)_tmpB5->hd;if(_tmpBC == 'h'){_LL119:
 t=Cyc_Absyn_sshort_typ;goto _LL113;}else{goto _LL11C;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmpB5)->tl)->tl == 0){_LL11A: _tmpBE=(int)_tmpB5->hd;_tmpBD=(int)(_tmpB5->tl)->hd;if(
_tmpBE == 'h'  && _tmpBD == 'h'){_LL11B: t=Cyc_Absyn_schar_typ;goto _LL113;}else{goto _LL11C;}}else{_LL11C: _LL11D:
# 487
({struct Cyc_Int_pa_PrintArg_struct _tmpB9;_tmpB9.tag=1;_tmpB9.f1=(unsigned long)((int)_tmpFC);({struct Cyc_String_pa_PrintArg_struct _tmpB8;_tmpB8.tag=0;({struct _dyneither_ptr _tmp164=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmpFD));_tmpB8.f1=_tmp164;});({void*_tmpB6[2]={& _tmpB8,& _tmpB9};({unsigned int _tmp166=loc;struct _dyneither_ptr _tmp165=({const char*_tmpB7="length modifier '%s' is not allowed with %%%c";_tag_dyneither(_tmpB7,sizeof(char),46);});Cyc_Tcutil_terr(_tmp166,_tmp165,_tag_dyneither(_tmpB6,sizeof(void*),2));});});});});{
struct Cyc_List_List*_tmpBA=0;_npop_handler(0);return _tmpBA;};}}}_LL113:;}
# 491
({void*_tmp16A=({void*_tmp169=t;void*_tmp168=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,_tmpB4);struct Cyc_Absyn_Tqual _tmp167=Cyc_Absyn_empty_tqual(0);Cyc_Absyn_at_typ(_tmp169,_tmp168,_tmp167,Cyc_Absyn_false_conref);});t=_tmp16A;});
({struct Cyc_List_List*_tmp16B=({struct Cyc_List_List*_tmpBF=_region_malloc(r,sizeof(*_tmpBF));_tmpBF->hd=t;_tmpBF->tl=_tmpA5;_tmpBF;});_tmpA5=_tmp16B;});
goto _LLE8;}case 117: _LLED: _LLEE:
 goto _LLF0;case 111: _LLEF: _LLF0:
 goto _LLF2;case 120: _LLF1: _LLF2:
 goto _LLF4;case 88: _LLF3: _LLF4: {
# 498
struct Cyc_Core_Opt*_tmpC1=({struct Cyc_Core_Opt*_tmpCD=_cycalloc(sizeof(*_tmpCD));({struct Cyc_List_List*_tmp16C=Cyc_Tcenv_lookup_type_vars(te);_tmpCD->v=_tmp16C;});_tmpCD;});
{struct Cyc_List_List*_tmpC2=_tmpFD;int _tmpCB;int _tmpCA;int _tmpC9;int _tmpC8;if(_tmpC2 == 0){_LL11F: _LL120:
 t=Cyc_Absyn_uint_typ;goto _LL11E;}else{if(((struct Cyc_List_List*)_tmpC2)->tl == 0){_LL121: _tmpC8=(int)_tmpC2->hd;if(_tmpC8 == 'l'){_LL122:
 t=Cyc_Absyn_ulong_typ;goto _LL11E;}else{_LL123: _tmpC9=(int)_tmpC2->hd;if(_tmpC9 == 'h'){_LL124:
 t=Cyc_Absyn_ushort_typ;goto _LL11E;}else{goto _LL127;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmpC2)->tl)->tl == 0){_LL125: _tmpCB=(int)_tmpC2->hd;_tmpCA=(int)(_tmpC2->tl)->hd;if(
_tmpCB == 'h'  && _tmpCA == 'h'){_LL126: t=Cyc_Absyn_uchar_typ;goto _LL11E;}else{goto _LL127;}}else{_LL127: _LL128:
# 505
({struct Cyc_Int_pa_PrintArg_struct _tmpC6;_tmpC6.tag=1;_tmpC6.f1=(unsigned long)((int)_tmpFC);({struct Cyc_String_pa_PrintArg_struct _tmpC5;_tmpC5.tag=0;({struct _dyneither_ptr _tmp16D=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmpFD));_tmpC5.f1=_tmp16D;});({void*_tmpC3[2]={& _tmpC5,& _tmpC6};({unsigned int _tmp16F=loc;struct _dyneither_ptr _tmp16E=({const char*_tmpC4="length modifier '%s' is not allowed with %%%c";_tag_dyneither(_tmpC4,sizeof(char),46);});Cyc_Tcutil_terr(_tmp16F,_tmp16E,_tag_dyneither(_tmpC3,sizeof(void*),2));});});});});{
struct Cyc_List_List*_tmpC7=0;_npop_handler(0);return _tmpC7;};}}}_LL11E:;}
# 509
({void*_tmp173=({void*_tmp172=t;void*_tmp171=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,_tmpC1);struct Cyc_Absyn_Tqual _tmp170=Cyc_Absyn_empty_tqual(0);Cyc_Absyn_at_typ(_tmp172,_tmp171,_tmp170,Cyc_Absyn_false_conref);});t=_tmp173;});
({struct Cyc_List_List*_tmp174=({struct Cyc_List_List*_tmpCC=_region_malloc(r,sizeof(*_tmpCC));_tmpCC->hd=t;_tmpCC->tl=_tmpA5;_tmpCC;});_tmpA5=_tmp174;});
goto _LLE8;}case 102: _LLF5: _LLF6:
 goto _LLF8;case 70: _LLF7: _LLF8:
 goto _LLFA;case 101: _LLF9: _LLFA:
 goto _LLFC;case 69: _LLFB: _LLFC:
 goto _LLFE;case 103: _LLFD: _LLFE:
 goto _LL100;case 71: _LLFF: _LL100:
 goto _LL102;case 97: _LL101: _LL102:
 goto _LL104;case 65: _LL103: _LL104: {
# 520
struct Cyc_Core_Opt*_tmpCE=({struct Cyc_Core_Opt*_tmpD7=_cycalloc(sizeof(*_tmpD7));({struct Cyc_List_List*_tmp175=Cyc_Tcenv_lookup_type_vars(te);_tmpD7->v=_tmp175;});_tmpD7;});
{struct Cyc_List_List*_tmpCF=_tmpFD;int _tmpD5;if(_tmpCF == 0){_LL12A: _LL12B:
({void*_tmp176=Cyc_Absyn_float_typ(0);t=_tmp176;});goto _LL129;}else{if(((struct Cyc_List_List*)_tmpCF)->tl == 0){_LL12C: _tmpD5=(int)_tmpCF->hd;if(_tmpD5 == 'l'){_LL12D:
# 524
({void*_tmp177=Cyc_Absyn_float_typ(1);t=_tmp177;});goto _LL129;}else{goto _LL12E;}}else{_LL12E: _LL12F:
# 526
({struct Cyc_Int_pa_PrintArg_struct _tmpD3;_tmpD3.tag=1;_tmpD3.f1=(unsigned long)((int)_tmpFC);({struct Cyc_String_pa_PrintArg_struct _tmpD2;_tmpD2.tag=0;({struct _dyneither_ptr _tmp178=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmpFD));_tmpD2.f1=_tmp178;});({void*_tmpD0[2]={& _tmpD2,& _tmpD3};({unsigned int _tmp17A=loc;struct _dyneither_ptr _tmp179=({const char*_tmpD1="length modifier '%s' is not allowed with %%%c";_tag_dyneither(_tmpD1,sizeof(char),46);});Cyc_Tcutil_terr(_tmp17A,_tmp179,_tag_dyneither(_tmpD0,sizeof(void*),2));});});});});{
struct Cyc_List_List*_tmpD4=0;_npop_handler(0);return _tmpD4;};}}_LL129:;}
# 530
({void*_tmp17E=({void*_tmp17D=t;void*_tmp17C=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,_tmpCE);struct Cyc_Absyn_Tqual _tmp17B=Cyc_Absyn_empty_tqual(0);Cyc_Absyn_at_typ(_tmp17D,_tmp17C,_tmp17B,Cyc_Absyn_false_conref);});t=_tmp17E;});
({struct Cyc_List_List*_tmp17F=({struct Cyc_List_List*_tmpD6=_region_malloc(r,sizeof(*_tmpD6));_tmpD6->hd=t;_tmpD6->tl=_tmpA5;_tmpD6;});_tmpA5=_tmp17F;});
goto _LLE8;}case 99: _LL105: _LL106: {
# 535
struct Cyc_Core_Opt*_tmpD8=({struct Cyc_Core_Opt*_tmpDA=_cycalloc(sizeof(*_tmpDA));({struct Cyc_List_List*_tmp180=Cyc_Tcenv_lookup_type_vars(te);_tmpDA->v=_tmp180;});_tmpDA;});
({struct Cyc_List_List*_tmp185=({struct Cyc_List_List*_tmpD9=_region_malloc(r,sizeof(*_tmpD9));({void*_tmp184=({void*_tmp183=Cyc_Absyn_char_typ;void*_tmp182=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,_tmpD8);struct Cyc_Absyn_Tqual _tmp181=
Cyc_Absyn_empty_tqual(0);
# 536
Cyc_Absyn_dyneither_typ(_tmp183,_tmp182,_tmp181,Cyc_Absyn_false_conref);});_tmpD9->hd=_tmp184;});_tmpD9->tl=_tmpA5;_tmpD9;});_tmpA5=_tmp185;});
# 539
goto _LLE8;}case 115: _LL107: _LL108: {
# 541
struct Cyc_Core_Opt*_tmpDB=({struct Cyc_Core_Opt*_tmpDD=_cycalloc(sizeof(*_tmpDD));({struct Cyc_List_List*_tmp186=Cyc_Tcenv_lookup_type_vars(te);_tmpDD->v=_tmp186;});_tmpDD;});
# 543
void*ptr=({void*_tmp189=Cyc_Absyn_char_typ;void*_tmp188=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,_tmpDB);struct Cyc_Absyn_Tqual _tmp187=
Cyc_Absyn_empty_tqual(0);
# 543
Cyc_Absyn_dyneither_typ(_tmp189,_tmp188,_tmp187,Cyc_Absyn_false_conref);});
# 545
({struct Cyc_List_List*_tmp18A=({struct Cyc_List_List*_tmpDC=_region_malloc(r,sizeof(*_tmpDC));_tmpDC->hd=ptr;_tmpDC->tl=_tmpA5;_tmpDC;});_tmpA5=_tmp18A;});
goto _LLE8;}case 91: _LL109: _LL10A:
 goto _LL10C;case 112: _LL10B: _LL10C:
# 549
({struct Cyc_Int_pa_PrintArg_struct _tmpE0;_tmpE0.tag=1;_tmpE0.f1=(unsigned long)((int)_tmpFC);({void*_tmpDE[1]={& _tmpE0};({unsigned int _tmp18C=loc;struct _dyneither_ptr _tmp18B=({const char*_tmpDF="%%%c is not supported";_tag_dyneither(_tmpDF,sizeof(char),22);});Cyc_Tcutil_terr(_tmp18C,_tmp18B,_tag_dyneither(_tmpDE,sizeof(void*),1));});});});{
struct Cyc_List_List*_tmpE1=0;_npop_handler(0);return _tmpE1;};case 110: _LL10D: _LL10E: {
# 552
struct Cyc_Core_Opt*_tmpE2=({struct Cyc_Core_Opt*_tmpEE=_cycalloc(sizeof(*_tmpEE));({struct Cyc_List_List*_tmp18D=Cyc_Tcenv_lookup_type_vars(te);_tmpEE->v=_tmp18D;});_tmpEE;});
{struct Cyc_List_List*_tmpE3=_tmpFD;int _tmpEC;int _tmpEB;int _tmpEA;int _tmpE9;if(_tmpE3 == 0){_LL131: _LL132:
 t=Cyc_Absyn_sint_typ;goto _LL130;}else{if(((struct Cyc_List_List*)_tmpE3)->tl == 0){_LL133: _tmpE9=(int)_tmpE3->hd;if(_tmpE9 == 'l'){_LL134:
 t=Cyc_Absyn_ulong_typ;goto _LL130;}else{_LL135: _tmpEA=(int)_tmpE3->hd;if(_tmpEA == 'h'){_LL136:
 t=Cyc_Absyn_sshort_typ;goto _LL130;}else{goto _LL139;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmpE3)->tl)->tl == 0){_LL137: _tmpEC=(int)_tmpE3->hd;_tmpEB=(int)(_tmpE3->tl)->hd;if(
_tmpEC == 'h'  && _tmpEB == 'h'){_LL138: t=Cyc_Absyn_schar_typ;goto _LL130;}else{goto _LL139;}}else{_LL139: _LL13A:
# 559
({struct Cyc_Int_pa_PrintArg_struct _tmpE7;_tmpE7.tag=1;_tmpE7.f1=(unsigned long)((int)_tmpFC);({struct Cyc_String_pa_PrintArg_struct _tmpE6;_tmpE6.tag=0;({struct _dyneither_ptr _tmp18E=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmpFD));_tmpE6.f1=_tmp18E;});({void*_tmpE4[2]={& _tmpE6,& _tmpE7};({unsigned int _tmp190=loc;struct _dyneither_ptr _tmp18F=({const char*_tmpE5="length modifier '%s' is not allowed with %%%c";_tag_dyneither(_tmpE5,sizeof(char),46);});Cyc_Tcutil_terr(_tmp190,_tmp18F,_tag_dyneither(_tmpE4,sizeof(void*),2));});});});});{
struct Cyc_List_List*_tmpE8=0;_npop_handler(0);return _tmpE8;};}}}_LL130:;}
# 563
({void*_tmp194=({void*_tmp193=t;void*_tmp192=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,_tmpE2);struct Cyc_Absyn_Tqual _tmp191=Cyc_Absyn_empty_tqual(0);Cyc_Absyn_at_typ(_tmp193,_tmp192,_tmp191,Cyc_Absyn_false_conref);});t=_tmp194;});
({struct Cyc_List_List*_tmp195=({struct Cyc_List_List*_tmpED=_region_malloc(r,sizeof(*_tmpED));_tmpED->hd=t;_tmpED->tl=_tmpA5;_tmpED;});_tmpA5=_tmp195;});
goto _LLE8;}case 37: _LL10F: _LL110:
# 567
 if(_tmpFF){
({void*_tmpEF=0;({unsigned int _tmp197=loc;struct _dyneither_ptr _tmp196=({const char*_tmpF0="Assignment suppression (*) is not allowed with %%%%";_tag_dyneither(_tmpF0,sizeof(char),52);});Cyc_Tcutil_terr(_tmp197,_tmp196,_tag_dyneither(_tmpEF,sizeof(void*),0));});});{
struct Cyc_List_List*_tmpF1=0;_npop_handler(0);return _tmpF1;};}
# 571
if(_tmpFE != 0){
({struct Cyc_String_pa_PrintArg_struct _tmpF4;_tmpF4.tag=0;({struct _dyneither_ptr _tmp198=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmpFE));_tmpF4.f1=_tmp198;});({void*_tmpF2[1]={& _tmpF4};({unsigned int _tmp19A=loc;struct _dyneither_ptr _tmp199=({const char*_tmpF3="width '%s' not allowed with %%%%";_tag_dyneither(_tmpF3,sizeof(char),33);});Cyc_Tcutil_terr(_tmp19A,_tmp199,_tag_dyneither(_tmpF2,sizeof(void*),1));});});});{
struct Cyc_List_List*_tmpF5=0;_npop_handler(0);return _tmpF5;};}
# 575
if(_tmpFD != 0){
({struct Cyc_String_pa_PrintArg_struct _tmpF8;_tmpF8.tag=0;({struct _dyneither_ptr _tmp19B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmpFD));_tmpF8.f1=_tmp19B;});({void*_tmpF6[1]={& _tmpF8};({unsigned int _tmp19D=loc;struct _dyneither_ptr _tmp19C=({const char*_tmpF7="length modifier '%s' not allowed with %%%%";_tag_dyneither(_tmpF7,sizeof(char),43);});Cyc_Tcutil_terr(_tmp19D,_tmp19C,_tag_dyneither(_tmpF6,sizeof(void*),1));});});});{
struct Cyc_List_List*_tmpF9=0;_npop_handler(0);return _tmpF9;};}
# 579
goto _LLE8;default: _LL111: _LL112: {
# 581
struct Cyc_List_List*_tmpFA=0;_npop_handler(0);return _tmpFA;}}_LLE8:;};};};}
# 459
;_pop_region(temp);}
# 585
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpA5);}
