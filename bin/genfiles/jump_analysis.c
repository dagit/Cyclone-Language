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
 struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};
# 128 "core.h"
int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Position_Error;
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
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
# 1183 "absyn.h"
extern int Cyc_Absyn_no_regions;struct Cyc_RgnOrder_RgnPO;
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
# 84 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};struct _tuple10{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple10*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned int f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _dyneither_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned int pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 115 "tcpat.h"
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 78
int Cyc_Hashtable_hash_stringptr(struct _dyneither_ptr*p);struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 46 "jump_analysis.h"
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds);struct Cyc_JumpAnalysis_NotLoop_j_JumpAnalysis_Jumpee_struct{int tag;};struct Cyc_JumpAnalysis_CaseEnd_j_JumpAnalysis_Jumpee_struct{int tag;};struct Cyc_JumpAnalysis_FnEnd_j_JumpAnalysis_Jumpee_struct{int tag;};struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_JumpAnalysis_Fallthru_j_JumpAnalysis_Jumpee_struct{int tag;struct Cyc_Absyn_Switch_clause*f1;};struct Cyc_JumpAnalysis_Return_j_JumpAnalysis_Jumpee_struct{int tag;};struct Cyc_JumpAnalysis_Lab_j_JumpAnalysis_Jumpee_struct{int tag;struct _dyneither_ptr*f1;};
# 54 "jump_analysis.cyc"
struct Cyc_JumpAnalysis_NotLoop_j_JumpAnalysis_Jumpee_struct Cyc_JumpAnalysis_NotLoop_j_val={0U};
struct Cyc_JumpAnalysis_CaseEnd_j_JumpAnalysis_Jumpee_struct Cyc_JumpAnalysis_CaseEnd_j_val={1U};
struct Cyc_JumpAnalysis_FnEnd_j_JumpAnalysis_Jumpee_struct Cyc_JumpAnalysis_FnEnd_j_val={2U};
struct Cyc_JumpAnalysis_Return_j_JumpAnalysis_Jumpee_struct Cyc_JumpAnalysis_Return_j_val={5U};struct Cyc_JumpAnalysis_SaveEnv{int try_depth;int pat_var_depth;struct Cyc_List_List*enclosers;};struct Cyc_JumpAnalysis_FnEnv{struct Cyc_JumpAnalysis_Jump_Anal_Result*tables;struct Cyc_Hashtable_Table*stmt_info;struct Cyc_List_List*jumpers;struct Cyc_Hashtable_Table*labels;};struct Cyc_JumpAnalysis_StmtEnv{struct Cyc_JumpAnalysis_FnEnv*fenv;void*continue_stmt;void*break_stmt;void*next_stmt;struct Cyc_JumpAnalysis_SaveEnv save_env;};
# 81
static void Cyc_JumpAnalysis_anal_exp(struct Cyc_JumpAnalysis_StmtEnv env,struct Cyc_Absyn_Exp*e);
static void Cyc_JumpAnalysis_anal_stmt(struct Cyc_JumpAnalysis_StmtEnv env,struct Cyc_Absyn_Stmt*s);
static void Cyc_JumpAnalysis_anal_fd(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd);
# 85
static int Cyc_JumpAnalysis_hash_ptr(void*s){
return(int)s;}
# 88
static struct Cyc_Hashtable_Table*Cyc_JumpAnalysis_make_ptr_table(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(void*,void*),int(*hash)(void*)))Cyc_Hashtable_create)(33,Cyc_Core_ptrcmp,(int(*)(void*s))Cyc_JumpAnalysis_hash_ptr);}
# 91
static struct Cyc_Hashtable_Table*Cyc_JumpAnalysis_make_var_table(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(33,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
# 94
static struct Cyc_JumpAnalysis_StmtEnv Cyc_JumpAnalysis_add_encloser(struct Cyc_Absyn_Stmt*s,struct Cyc_JumpAnalysis_StmtEnv env){
struct Cyc_JumpAnalysis_StmtEnv _tmp0=env;
({struct Cyc_List_List*_tmpB0=({struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));((*_tmp1).hd=s,(*_tmp1).tl=(_tmp0.save_env).enclosers);_tmp1;});(_tmp0.save_env).enclosers=_tmpB0;});
return _tmp0;}
# 99
static void Cyc_JumpAnalysis_anal_exps(struct Cyc_JumpAnalysis_StmtEnv env,struct Cyc_List_List*es){
for(0;es != 0;es=es->tl){
Cyc_JumpAnalysis_anal_exp(env,(struct Cyc_Absyn_Exp*)es->hd);}}struct _tuple11{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 103
static void Cyc_JumpAnalysis_anal_exp(struct Cyc_JumpAnalysis_StmtEnv env,struct Cyc_Absyn_Exp*e){
void*_tmp2=e->r;void*_tmp3=_tmp2;struct Cyc_List_List*_tmp31;struct Cyc_List_List*_tmp30;struct Cyc_List_List*_tmp2F;struct Cyc_List_List*_tmp2E;struct Cyc_Absyn_Exp*_tmp2D;struct Cyc_Absyn_Exp*_tmp2C;struct Cyc_Absyn_Exp*_tmp2B;struct Cyc_Absyn_Exp*_tmp2A;struct Cyc_Absyn_Exp*_tmp29;struct Cyc_Absyn_Exp*_tmp28;struct Cyc_Absyn_Exp*_tmp27;struct Cyc_Absyn_Exp*_tmp26;struct Cyc_Absyn_Exp*_tmp25;struct Cyc_Absyn_Exp*_tmp24;struct Cyc_Absyn_Exp*_tmp23;struct Cyc_Absyn_Exp*_tmp22;struct Cyc_Absyn_Exp*_tmp21;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_Absyn_Exp*_tmp1F;struct Cyc_Absyn_Exp*_tmp1E;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_Absyn_Exp*_tmp1B;struct Cyc_Absyn_Exp*_tmp1A;struct Cyc_Absyn_Exp*_tmp19;struct Cyc_Absyn_Exp*_tmp18;struct Cyc_Absyn_Exp*_tmp17;struct Cyc_Absyn_Exp*_tmp16;struct Cyc_Absyn_Exp*_tmp15;struct Cyc_Absyn_Exp*_tmp14;struct Cyc_Absyn_Exp*_tmp13;struct Cyc_Absyn_Exp*_tmp12;struct Cyc_Absyn_Exp*_tmp11;struct Cyc_Absyn_Exp*_tmp10;struct Cyc_Absyn_Exp*_tmpF;struct Cyc_Absyn_Exp*_tmpE;struct Cyc_Absyn_Exp*_tmpD;struct Cyc_List_List*_tmpC;struct Cyc_List_List*_tmpB;struct Cyc_List_List*_tmpA;struct Cyc_List_List*_tmp9;struct Cyc_Absyn_Stmt*_tmp8;switch(*((int*)_tmp3)){case 37U: _LL1: _tmp8=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL2:
({struct Cyc_JumpAnalysis_StmtEnv _tmpB1=Cyc_JumpAnalysis_add_encloser(_tmp8,env);Cyc_JumpAnalysis_anal_stmt(_tmpB1,_tmp8);});goto _LL0;case 0U: _LL3: _LL4:
# 107
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 39U: _LLB: _LLC:
 goto _LLE;case 32U: _LLD: _LLE:
 goto _LL10;case 33U: _LLF: _LL10:
 goto _LL12;case 2U: _LL11: _LL12:
 goto _LL14;case 1U: _LL13: _LL14:
 goto _LL0;case 31U: _LL15: _tmp9=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL16:
# 117
 _tmpA=_tmp9;goto _LL18;case 24U: _LL17: _tmpA=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL18:
 _tmpB=_tmpA;goto _LL1A;case 3U: _LL19: _tmpB=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL1A:
 Cyc_JumpAnalysis_anal_exps(env,_tmpB);goto _LL0;case 10U: _LL1B: _tmpD=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmpC=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL1C:
# 121
 Cyc_JumpAnalysis_anal_exp(env,_tmpD);Cyc_JumpAnalysis_anal_exps(env,_tmpC);goto _LL0;case 28U: _LL1D: _tmpE=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL1E:
# 123
 _tmpF=_tmpE;goto _LL20;case 38U: _LL1F: _tmpF=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL20:
 _tmp10=_tmpF;goto _LL22;case 15U: _LL21: _tmp10=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL22:
 _tmp11=_tmp10;goto _LL24;case 20U: _LL23: _tmp11=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL24:
 _tmp12=_tmp11;goto _LL26;case 21U: _LL25: _tmp12=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL26:
 _tmp13=_tmp12;goto _LL28;case 22U: _LL27: _tmp13=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL28:
 _tmp14=_tmp13;goto _LL2A;case 14U: _LL29: _tmp14=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL2A:
 _tmp15=_tmp14;goto _LL2C;case 12U: _LL2B: _tmp15=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL2C:
 _tmp16=_tmp15;goto _LL2E;case 13U: _LL2D: _tmp16=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL2E:
 _tmp17=_tmp16;goto _LL30;case 11U: _LL2F: _tmp17=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL30:
 _tmp18=_tmp17;goto _LL32;case 5U: _LL31: _tmp18=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL32:
 Cyc_JumpAnalysis_anal_exp(env,_tmp18);goto _LL0;case 6U: _LL33: _tmp1B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmp1A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_tmp19=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3)->f3;_LL34:
# 136
 Cyc_JumpAnalysis_anal_exp(env,_tmp1B);Cyc_JumpAnalysis_anal_exp(env,_tmp1A);Cyc_JumpAnalysis_anal_exp(env,_tmp19);goto _LL0;case 27U: _LL35: _tmp1D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_tmp1C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3)->f3;_LL36:
# 138
 _tmp1F=_tmp1D;_tmp1E=_tmp1C;goto _LL38;case 4U: _LL37: _tmp1F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmp1E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp3)->f3;_LL38:
 _tmp21=_tmp1F;_tmp20=_tmp1E;goto _LL3A;case 35U: _LL39: _tmp21=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmp20=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL3A:
 _tmp23=_tmp21;_tmp22=_tmp20;goto _LL3C;case 23U: _LL3B: _tmp23=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmp22=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL3C:
 _tmp25=_tmp23;_tmp24=_tmp22;goto _LL3E;case 7U: _LL3D: _tmp25=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmp24=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL3E:
 _tmp27=_tmp25;_tmp26=_tmp24;goto _LL40;case 8U: _LL3F: _tmp27=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmp26=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL40:
 _tmp29=_tmp27;_tmp28=_tmp26;goto _LL42;case 9U: _LL41: _tmp29=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmp28=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL42:
 Cyc_JumpAnalysis_anal_exp(env,_tmp29);Cyc_JumpAnalysis_anal_exp(env,_tmp28);goto _LL0;case 34U: _LL43: _tmp2B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp3)->f1).rgn;_tmp2A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp3)->f1).num_elts;_LL44:
# 146
 _tmp2D=_tmp2B;_tmp2C=_tmp2A;goto _LL46;case 16U: _LL45: _tmp2D=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmp2C=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL46:
# 148
 if(_tmp2D != 0)Cyc_JumpAnalysis_anal_exp(env,_tmp2D);
Cyc_JumpAnalysis_anal_exp(env,_tmp2C);
goto _LL0;case 30U: _LL47: _tmp2E=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL48:
# 152
 _tmp2F=_tmp2E;goto _LL4A;case 29U: _LL49: _tmp2F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp3)->f3;_LL4A:
 _tmp30=_tmp2F;goto _LL4C;case 25U: _LL4B: _tmp30=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL4C:
 _tmp31=_tmp30;goto _LL4E;case 26U: _LL4D: _tmp31=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL4E:
# 156
 for(0;_tmp31 != 0;_tmp31=_tmp31->tl){
Cyc_JumpAnalysis_anal_exp(env,(*((struct _tuple11*)_tmp31->hd)).f2);}
goto _LL0;case 36U: _LL4F: _LL50:
# 160
({void*_tmp4=0U;({struct _dyneither_ptr _tmpB2=({const char*_tmp5="jump_anaysis found UnresolvedMem_e";_tag_dyneither(_tmp5,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpB2,_tag_dyneither(_tmp4,sizeof(void*),0U));});});default: _LL51: _LL52:
({void*_tmp6=0U;({struct _dyneither_ptr _tmpB3=({const char*_tmp7="jump_analysis found Asm_e";_tag_dyneither(_tmp7,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpB3,_tag_dyneither(_tmp6,sizeof(void*),0U));});});}_LL0:;}
# 164
static void Cyc_JumpAnalysis_anal_scs(struct Cyc_JumpAnalysis_StmtEnv env,struct Cyc_List_List*scs){
# 167
env.break_stmt=env.next_stmt;
env.next_stmt=(void*)& Cyc_JumpAnalysis_CaseEnd_j_val;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp32=(struct Cyc_Absyn_Switch_clause*)scs->hd;
struct Cyc_Absyn_Exp*_tmp33=_tmp32->where_clause;
if(_tmp33 != 0)
Cyc_JumpAnalysis_anal_exp(env,_tmp33);{
struct Cyc_JumpAnalysis_StmtEnv _tmp34=env;
if(Cyc_Tcpat_has_vars(_tmp32->pat_vars)){
({struct Cyc_JumpAnalysis_StmtEnv _tmpB4=Cyc_JumpAnalysis_add_encloser(_tmp32->body,_tmp34);_tmp34=_tmpB4;});
++(_tmp34.save_env).pat_var_depth;}
# 179
Cyc_JumpAnalysis_anal_stmt(_tmp34,_tmp32->body);};}}struct _tuple12{struct Cyc_Absyn_Stmt*f1;void*f2;};
# 182
static void Cyc_JumpAnalysis_anal_stmt(struct Cyc_JumpAnalysis_StmtEnv env,struct Cyc_Absyn_Stmt*s){
({struct Cyc_Hashtable_Table*_tmpB6=(env.fenv)->stmt_info;struct Cyc_Absyn_Stmt*_tmpB5=s;((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,struct Cyc_JumpAnalysis_SaveEnv*val))Cyc_Hashtable_insert)(_tmpB6,_tmpB5,({struct Cyc_JumpAnalysis_SaveEnv*_tmp35=_cycalloc(sizeof(*_tmp35));*_tmp35=env.save_env;_tmp35;}));});{
void*_tmp36=s->r;void*_tmp37=_tmp36;struct Cyc_Absyn_Decl*_tmp7B;struct Cyc_Absyn_Stmt*_tmp7A;struct Cyc_Absyn_Stmt*_tmp79;struct Cyc_List_List*_tmp78;struct Cyc_Absyn_Exp*_tmp77;struct Cyc_List_List*_tmp76;struct Cyc_List_List*_tmp75;struct Cyc_Absyn_Switch_clause**_tmp74;struct _dyneither_ptr*_tmp73;struct Cyc_Absyn_Stmt*_tmp72;struct _dyneither_ptr*_tmp71;struct Cyc_Absyn_Stmt*_tmp70;struct Cyc_Absyn_Exp*_tmp6F;struct Cyc_Absyn_Stmt*_tmp6E;struct Cyc_Absyn_Exp*_tmp6D;struct Cyc_Absyn_Exp*_tmp6C;struct Cyc_Absyn_Stmt*_tmp6B;struct Cyc_Absyn_Exp*_tmp6A;struct Cyc_Absyn_Stmt*_tmp69;struct Cyc_Absyn_Stmt*_tmp68;struct Cyc_Absyn_Exp*_tmp67;struct Cyc_Absyn_Stmt*_tmp66;struct Cyc_Absyn_Stmt*_tmp65;struct Cyc_Absyn_Exp*_tmp64;struct Cyc_Absyn_Exp*_tmp63;struct Cyc_Absyn_Stmt*_tmp62;struct Cyc_Absyn_Stmt*_tmp61;struct Cyc_Absyn_Stmt*_tmp60;struct Cyc_Absyn_Stmt*_tmp5F;struct Cyc_Absyn_Exp*_tmp5E;switch(*((int*)_tmp37)){case 0U: _LL1: _LL2:
 goto _LL0;case 1U: _LL3: _tmp5E=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp37)->f1;_LL4:
 Cyc_JumpAnalysis_anal_exp(env,_tmp5E);goto _LL0;case 2U: _LL5: _tmp60=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp37)->f1;_tmp5F=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp37)->f2;_LL6: {
# 188
struct Cyc_JumpAnalysis_StmtEnv _tmp38=env;
({void*_tmpB7=(void*)({struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*_tmp39=_cycalloc(sizeof(*_tmp39));((*_tmp39).tag=3U,(*_tmp39).f1=_tmp5F);_tmp39;});_tmp38.next_stmt=_tmpB7;});
Cyc_JumpAnalysis_anal_stmt(_tmp38,_tmp60);
Cyc_JumpAnalysis_anal_stmt(env,_tmp5F);
goto _LL0;}case 4U: _LL7: _tmp63=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp37)->f1;_tmp62=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp37)->f2;_tmp61=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp37)->f3;_LL8:
# 194
 Cyc_JumpAnalysis_anal_exp(env,_tmp63);
Cyc_JumpAnalysis_anal_stmt(env,_tmp62);
Cyc_JumpAnalysis_anal_stmt(env,_tmp61);
goto _LL0;case 3U: _LL9: _tmp64=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp37)->f1;_LLA:
# 200
 if(_tmp64 != 0)Cyc_JumpAnalysis_anal_exp(env,_tmp64);
({struct Cyc_List_List*_tmpB9=({struct Cyc_List_List*_tmp3B=_cycalloc(sizeof(*_tmp3B));(({struct _tuple12*_tmpB8=({struct _tuple12*_tmp3A=_cycalloc(sizeof(*_tmp3A));((*_tmp3A).f1=s,(*_tmp3A).f2=(void*)& Cyc_JumpAnalysis_Return_j_val);_tmp3A;});(*_tmp3B).hd=_tmpB8;}),(*_tmp3B).tl=(env.fenv)->jumpers);_tmp3B;});(env.fenv)->jumpers=_tmpB9;});
goto _LL0;case 5U: _LLB: _tmp67=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp37)->f1).f1;_tmp66=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp37)->f1).f2;_tmp65=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp37)->f2;_LLC:
# 205
({struct Cyc_Hashtable_Table*_tmpBB=(env.fenv)->stmt_info;struct Cyc_Absyn_Stmt*_tmpBA=_tmp66;((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,struct Cyc_JumpAnalysis_SaveEnv*val))Cyc_Hashtable_insert)(_tmpBB,_tmpBA,({struct Cyc_JumpAnalysis_SaveEnv*_tmp3C=_cycalloc(sizeof(*_tmp3C));*_tmp3C=env.save_env;_tmp3C;}));});
Cyc_JumpAnalysis_anal_exp(env,_tmp67);{
struct Cyc_JumpAnalysis_StmtEnv _tmp3D=env;
({void*_tmpBC=(void*)({struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*_tmp3E=_cycalloc(sizeof(*_tmp3E));((*_tmp3E).tag=3U,(*_tmp3E).f1=_tmp66);_tmp3E;});_tmp3D.continue_stmt=_tmpBC;});
_tmp3D.next_stmt=_tmp3D.continue_stmt;
_tmp3D.break_stmt=env.next_stmt;
Cyc_JumpAnalysis_anal_stmt(_tmp3D,_tmp65);
goto _LL0;};case 9U: _LLD: _tmp6D=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp37)->f1;_tmp6C=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp37)->f2).f1;_tmp6B=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp37)->f2).f2;_tmp6A=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp37)->f3).f1;_tmp69=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp37)->f3).f2;_tmp68=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp37)->f4;_LLE:
# 214
({struct Cyc_Hashtable_Table*_tmpBE=(env.fenv)->stmt_info;struct Cyc_Absyn_Stmt*_tmpBD=_tmp6B;((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,struct Cyc_JumpAnalysis_SaveEnv*val))Cyc_Hashtable_insert)(_tmpBE,_tmpBD,({struct Cyc_JumpAnalysis_SaveEnv*_tmp3F=_cycalloc(sizeof(*_tmp3F));*_tmp3F=env.save_env;_tmp3F;}));});
({struct Cyc_Hashtable_Table*_tmpC0=(env.fenv)->stmt_info;struct Cyc_Absyn_Stmt*_tmpBF=_tmp69;((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,struct Cyc_JumpAnalysis_SaveEnv*val))Cyc_Hashtable_insert)(_tmpC0,_tmpBF,({struct Cyc_JumpAnalysis_SaveEnv*_tmp40=_cycalloc(sizeof(*_tmp40));*_tmp40=env.save_env;_tmp40;}));});
Cyc_JumpAnalysis_anal_exp(env,_tmp6D);
Cyc_JumpAnalysis_anal_exp(env,_tmp6C);{
# 219
struct Cyc_JumpAnalysis_StmtEnv _tmp41=env;
({void*_tmpC1=(void*)({struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*_tmp42=_cycalloc(sizeof(*_tmp42));((*_tmp42).tag=3U,(*_tmp42).f1=_tmp69);_tmp42;});_tmp41.continue_stmt=_tmpC1;});
_tmp41.next_stmt=_tmp41.continue_stmt;
_tmp41.break_stmt=env.next_stmt;
Cyc_JumpAnalysis_anal_stmt(_tmp41,_tmp68);
Cyc_JumpAnalysis_anal_exp(_tmp41,_tmp6A);
goto _LL0;};case 14U: _LLF: _tmp70=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp37)->f1;_tmp6F=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp37)->f2).f1;_tmp6E=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp37)->f2).f2;_LL10:
# 227
({struct Cyc_Hashtable_Table*_tmpC3=(env.fenv)->stmt_info;struct Cyc_Absyn_Stmt*_tmpC2=_tmp6E;((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,struct Cyc_JumpAnalysis_SaveEnv*val))Cyc_Hashtable_insert)(_tmpC3,_tmpC2,({struct Cyc_JumpAnalysis_SaveEnv*_tmp43=_cycalloc(sizeof(*_tmp43));*_tmp43=env.save_env;_tmp43;}));});{
struct Cyc_JumpAnalysis_StmtEnv _tmp44=env;
({void*_tmpC4=(void*)({struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*_tmp45=_cycalloc(sizeof(*_tmp45));((*_tmp45).tag=3U,(*_tmp45).f1=_tmp6E);_tmp45;});_tmp44.continue_stmt=_tmpC4;});
_tmp44.next_stmt=_tmp44.continue_stmt;
_tmp44.break_stmt=env.next_stmt;
Cyc_JumpAnalysis_anal_stmt(_tmp44,_tmp70);
Cyc_JumpAnalysis_anal_exp(env,_tmp6F);
goto _LL0;};case 6U: _LL11: _LL12:
# 237
({struct Cyc_List_List*_tmpC6=({struct Cyc_List_List*_tmp47=_cycalloc(sizeof(*_tmp47));(({struct _tuple12*_tmpC5=({struct _tuple12*_tmp46=_cycalloc(sizeof(*_tmp46));((*_tmp46).f1=s,(*_tmp46).f2=env.break_stmt);_tmp46;});(*_tmp47).hd=_tmpC5;}),(*_tmp47).tl=(env.fenv)->jumpers);_tmp47;});(env.fenv)->jumpers=_tmpC6;});
goto _LL0;case 7U: _LL13: _LL14:
# 240
({struct Cyc_List_List*_tmpC8=({struct Cyc_List_List*_tmp49=_cycalloc(sizeof(*_tmp49));(({struct _tuple12*_tmpC7=({struct _tuple12*_tmp48=_cycalloc(sizeof(*_tmp48));((*_tmp48).f1=s,(*_tmp48).f2=env.continue_stmt);_tmp48;});(*_tmp49).hd=_tmpC7;}),(*_tmp49).tl=(env.fenv)->jumpers);_tmp49;});(env.fenv)->jumpers=_tmpC8;});
goto _LL0;case 8U: _LL15: _tmp71=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp37)->f1;_LL16:
# 243
({struct Cyc_List_List*_tmpCB=({struct Cyc_List_List*_tmp4C=_cycalloc(sizeof(*_tmp4C));(({struct _tuple12*_tmpCA=({struct _tuple12*_tmp4B=_cycalloc(sizeof(*_tmp4B));((*_tmp4B).f1=s,({void*_tmpC9=(void*)({struct Cyc_JumpAnalysis_Lab_j_JumpAnalysis_Jumpee_struct*_tmp4A=_cycalloc(sizeof(*_tmp4A));((*_tmp4A).tag=6U,(*_tmp4A).f1=_tmp71);_tmp4A;});(*_tmp4B).f2=_tmpC9;}));_tmp4B;});(*_tmp4C).hd=_tmpCA;}),(*_tmp4C).tl=(env.fenv)->jumpers);_tmp4C;});(env.fenv)->jumpers=_tmpCB;});
goto _LL0;case 13U: _LL17: _tmp73=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp37)->f1;_tmp72=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp37)->f2;_LL18:
# 247
{struct _handler_cons _tmp4D;_push_handler(& _tmp4D);{int _tmp4F=0;if(setjmp(_tmp4D.handler))_tmp4F=1;if(!_tmp4F){
((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)((env.fenv)->labels,_tmp73);
({void*_tmp50=0U;({unsigned int _tmpCD=s->loc;struct _dyneither_ptr _tmpCC=({const char*_tmp51="repated label";_tag_dyneither(_tmp51,sizeof(char),14U);});Cyc_Warn_err(_tmpCD,_tmpCC,_tag_dyneither(_tmp50,sizeof(void*),0U));});});
# 248
;_pop_handler();}else{void*_tmp4E=(void*)_exn_thrown;void*_tmp52=_tmp4E;void*_tmp53;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp52)->tag == Cyc_Core_Not_found){_LL22: _LL23:
# 250
 goto _LL21;}else{_LL24: _tmp53=_tmp52;_LL25:(int)_rethrow(_tmp53);}_LL21:;}};}
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Absyn_Stmt*val))Cyc_Hashtable_insert)((env.fenv)->labels,_tmp73,s);
Cyc_JumpAnalysis_anal_stmt(env,_tmp72);
goto _LL0;case 11U: _LL19: _tmp75=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp37)->f1;_tmp74=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp37)->f2;_LL1A:
# 257
 Cyc_JumpAnalysis_anal_exps(env,_tmp75);
({struct Cyc_List_List*_tmpD0=({struct Cyc_List_List*_tmp56=_cycalloc(sizeof(*_tmp56));(({struct _tuple12*_tmpCF=({struct _tuple12*_tmp55=_cycalloc(sizeof(*_tmp55));((*_tmp55).f1=s,({void*_tmpCE=(void*)({struct Cyc_JumpAnalysis_Fallthru_j_JumpAnalysis_Jumpee_struct*_tmp54=_cycalloc(sizeof(*_tmp54));((*_tmp54).tag=4U,(*_tmp54).f1=*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp74)));_tmp54;});(*_tmp55).f2=_tmpCE;}));_tmp55;});(*_tmp56).hd=_tmpCF;}),(*_tmp56).tl=(env.fenv)->jumpers);_tmp56;});(env.fenv)->jumpers=_tmpD0;});
# 260
goto _LL0;case 10U: _LL1B: _tmp77=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp37)->f1;_tmp76=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp37)->f2;_LL1C:
# 262
 Cyc_JumpAnalysis_anal_exp(env,_tmp77);
Cyc_JumpAnalysis_anal_scs(env,_tmp76);
goto _LL0;case 15U: _LL1D: _tmp79=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp37)->f1;_tmp78=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp37)->f2;_LL1E: {
# 266
struct Cyc_JumpAnalysis_StmtEnv _tmp57=Cyc_JumpAnalysis_add_encloser(_tmp79,env);
++(_tmp57.save_env).try_depth;
Cyc_JumpAnalysis_anal_stmt(_tmp57,_tmp79);
Cyc_JumpAnalysis_anal_scs(env,_tmp78);
goto _LL0;}default: _LL1F: _tmp7B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp37)->f1;_tmp7A=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp37)->f2;_LL20:
# 275
({struct Cyc_JumpAnalysis_StmtEnv _tmpD1=Cyc_JumpAnalysis_add_encloser(s,env);env=_tmpD1;});
{void*_tmp58=_tmp7B->r;void*_tmp59=_tmp58;struct Cyc_Absyn_Exp*_tmp5D;struct Cyc_Absyn_Fndecl*_tmp5C;struct Cyc_Absyn_Exp*_tmp5B;struct Cyc_Absyn_Exp*_tmp5A;switch(*((int*)_tmp59)){case 4U: _LL27: _tmp5A=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp59)->f3;_LL28:
# 279
 if(_tmp5A != 0){
Cyc_JumpAnalysis_anal_exp(env,_tmp5A);
++(env.save_env).pat_var_depth;}
# 283
if(!Cyc_Absyn_no_regions)
++(env.save_env).try_depth;
goto _LL26;case 0U: _LL29: _tmp5B=(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp59)->f1)->initializer;_LL2A:
# 287
 if(_tmp5B != 0)Cyc_JumpAnalysis_anal_exp(env,_tmp5B);
goto _LL26;case 1U: _LL2B: _tmp5C=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp59)->f1;_LL2C:
# 290
 Cyc_JumpAnalysis_anal_fd((env.fenv)->tables,_tmp5C);
goto _LL26;case 2U: _LL2D: _tmp5D=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp59)->f3;_LL2E:
# 293
 Cyc_JumpAnalysis_anal_exp(env,_tmp5D);
++(env.save_env).pat_var_depth;
goto _LL26;case 3U: _LL2F: _LL30:
# 297
 goto _LL32;case 8U: _LL31: _LL32:
 goto _LL34;case 5U: _LL33: _LL34:
 goto _LL36;case 6U: _LL35: _LL36:
 goto _LL38;case 7U: _LL37: _LL38:
 goto _LL3A;case 13U: _LL39: _LL3A:
 goto _LL3C;case 14U: _LL3B: _LL3C:
 goto _LL3E;case 11U: _LL3D: _LL3E:
 goto _LL40;case 12U: _LL3F: _LL40:
 goto _LL42;case 9U: _LL41: _LL42:
 goto _LL44;default: _LL43: _LL44:
 goto _LL26;}_LL26:;}
# 309
Cyc_JumpAnalysis_anal_stmt(env,_tmp7A);
goto _LL0;}_LL0:;};}
# 315
static void Cyc_JumpAnalysis_anal_fd(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_JumpAnalysis_FnEnv*_tmp7C=({struct Cyc_JumpAnalysis_FnEnv*_tmpA7=_cycalloc(sizeof(*_tmpA7));((((((*_tmpA7).tables=tables,({
struct Cyc_Hashtable_Table*_tmpD3=((struct Cyc_Hashtable_Table*(*)())Cyc_JumpAnalysis_make_ptr_table)();(*_tmpA7).stmt_info=_tmpD3;}))),(*_tmpA7).jumpers=0)),({
# 319
struct Cyc_Hashtable_Table*_tmpD2=((struct Cyc_Hashtable_Table*(*)())Cyc_JumpAnalysis_make_var_table)();(*_tmpA7).labels=_tmpD2;}));_tmpA7;});
struct Cyc_JumpAnalysis_StmtEnv _tmp7D=({struct Cyc_JumpAnalysis_StmtEnv _tmpAF;(((((((_tmpAF.fenv=_tmp7C,_tmpAF.continue_stmt=(void*)& Cyc_JumpAnalysis_NotLoop_j_val)),_tmpAF.break_stmt=(void*)& Cyc_JumpAnalysis_NotLoop_j_val)),_tmpAF.next_stmt=(void*)& Cyc_JumpAnalysis_FnEnd_j_val)),(
# 324
((((_tmpAF.save_env).try_depth=0,(_tmpAF.save_env).pat_var_depth=0)),(_tmpAF.save_env).enclosers=0)));_tmpAF;});
# 327
Cyc_JumpAnalysis_anal_stmt(_tmp7D,fd->body);{
# 329
struct Cyc_Hashtable_Table*pop_table=((struct Cyc_Hashtable_Table*(*)())Cyc_JumpAnalysis_make_ptr_table)();
struct Cyc_Hashtable_Table*succ_table=((struct Cyc_Hashtable_Table*(*)())Cyc_JumpAnalysis_make_ptr_table)();
struct Cyc_Hashtable_Table*pat_pop_table=((struct Cyc_Hashtable_Table*(*)())Cyc_JumpAnalysis_make_ptr_table)();
# 333
struct Cyc_Hashtable_Table*_tmp7E=_tmp7C->stmt_info;
{struct Cyc_List_List*_tmp7F=_tmp7C->jumpers;for(0;_tmp7F != 0;_tmp7F=_tmp7F->tl){
struct _tuple12*_tmp80=(struct _tuple12*)_tmp7F->hd;struct _tuple12*_tmp81=_tmp80;struct Cyc_Absyn_Stmt*_tmpA6;void*_tmpA5;_LL1: _tmpA6=_tmp81->f1;_tmpA5=_tmp81->f2;_LL2:;{
struct Cyc_JumpAnalysis_SaveEnv*_tmp82=
# 338
((struct Cyc_JumpAnalysis_SaveEnv*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(_tmp7E,_tmpA6);
# 336
struct Cyc_JumpAnalysis_SaveEnv*_tmp83=_tmp82;int _tmpA4;int _tmpA3;struct Cyc_List_List*_tmpA2;_LL4: _tmpA4=_tmp83->try_depth;_tmpA3=_tmp83->pat_var_depth;_tmpA2=_tmp83->enclosers;_LL5:;{
# 339
void*_tmp84=_tmpA5;struct Cyc_Absyn_Switch_clause*_tmpA1;struct Cyc_Absyn_Stmt*_tmpA0;struct _dyneither_ptr*_tmp9F;switch(*((int*)_tmp84)){case 0U: _LL7: _LL8:
# 341
({void*_tmp85=0U;({unsigned int _tmpD5=_tmpA6->loc;struct _dyneither_ptr _tmpD4=({const char*_tmp86="jump has no target";_tag_dyneither(_tmp86,sizeof(char),19U);});Cyc_Warn_err(_tmpD5,_tmpD4,_tag_dyneither(_tmp85,sizeof(void*),0U));});});
goto _LL6;case 1U: _LL9: _LLA:
# 344
({void*_tmp87=0U;({unsigned int _tmpD7=_tmpA6->loc;struct _dyneither_ptr _tmpD6=({const char*_tmp88="break causes outer switch to implicitly fallthru";_tag_dyneither(_tmp88,sizeof(char),49U);});Cyc_Warn_err(_tmpD7,_tmpD6,_tag_dyneither(_tmp87,sizeof(void*),0U));});});
# 346
goto _LL6;case 2U: _LLB: _LLC:
# 348
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,struct Cyc_Absyn_Stmt*val))Cyc_Hashtable_insert)(succ_table,_tmpA6,0);
goto _LLE;case 5U: _LLD: _LLE:
# 351
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,int val))Cyc_Hashtable_insert)(pop_table,_tmpA6,_tmpA4);
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,int val))Cyc_Hashtable_insert)(pat_pop_table,_tmpA6,_tmpA3);
goto _LL6;case 6U: _LLF: _tmp9F=((struct Cyc_JumpAnalysis_Lab_j_JumpAnalysis_Jumpee_struct*)_tmp84)->f1;_LL10: {
# 355
struct Cyc_Absyn_Stmt*dest_stmt;
{struct _handler_cons _tmp89;_push_handler(& _tmp89);{int _tmp8B=0;if(setjmp(_tmp89.handler))_tmp8B=1;if(!_tmp8B){({struct Cyc_Absyn_Stmt*_tmpD8=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(_tmp7C->labels,_tmp9F);dest_stmt=_tmpD8;});;_pop_handler();}else{void*_tmp8A=(void*)_exn_thrown;void*_tmp8C=_tmp8A;void*_tmp90;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp8C)->tag == Cyc_Core_Not_found){_LL16: _LL17:
# 359
({struct Cyc_String_pa_PrintArg_struct _tmp8F;_tmp8F.tag=0U;_tmp8F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9F);({void*_tmp8D[1U];_tmp8D[0]=& _tmp8F;({unsigned int _tmpDA=_tmpA6->loc;struct _dyneither_ptr _tmpD9=({const char*_tmp8E="label %s undefined";_tag_dyneither(_tmp8E,sizeof(char),19U);});Cyc_Warn_err(_tmpDA,_tmpD9,_tag_dyneither(_tmp8D,sizeof(void*),1U));});});});
goto L;}else{_LL18: _tmp90=_tmp8C;_LL19:(int)_rethrow(_tmp90);}_LL15:;}};}{
# 362
struct Cyc_JumpAnalysis_SaveEnv*_tmp91=
((struct Cyc_JumpAnalysis_SaveEnv*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(_tmp7E,dest_stmt);
# 362
struct Cyc_JumpAnalysis_SaveEnv*_tmp92=_tmp91;struct Cyc_List_List*_tmp96;_LL1B: _tmp96=_tmp92->enclosers;_LL1C:;
# 364
if(_tmp96 != 0){
struct Cyc_Absyn_Stmt*_tmp93=(struct Cyc_Absyn_Stmt*)_tmp96->hd;
for(0;_tmpA2 != 0;_tmpA2=_tmpA2->tl){
if((struct Cyc_Absyn_Stmt*)_tmpA2->hd == _tmp93)
break;}
if(_tmpA2 == 0){
({void*_tmp94=0U;({unsigned int _tmpDC=_tmpA6->loc;struct _dyneither_ptr _tmpDB=({const char*_tmp95="goto enters local scope or exception handler";_tag_dyneither(_tmp95,sizeof(char),45U);});Cyc_Warn_err(_tmpDC,_tmpDB,_tag_dyneither(_tmp94,sizeof(void*),0U));});});
# 372
goto _LL6;}}
# 375
_tmpA0=dest_stmt;goto _LL12;};}case 3U: _LL11: _tmpA0=((struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*)_tmp84)->f1;_LL12: {
# 377
struct Cyc_JumpAnalysis_SaveEnv*_tmp97=
((struct Cyc_JumpAnalysis_SaveEnv*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(_tmp7E,_tmpA0);
# 377
struct Cyc_JumpAnalysis_SaveEnv*_tmp98=_tmp97;int _tmp9A;int _tmp99;_LL1E: _tmp9A=_tmp98->try_depth;_tmp99=_tmp98->pat_var_depth;_LL1F:;
# 379
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,int val))Cyc_Hashtable_insert)(pop_table,_tmpA6,_tmpA4 - _tmp9A);
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,int val))Cyc_Hashtable_insert)(pat_pop_table,_tmpA6,_tmpA3 - _tmp99);
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,struct Cyc_Absyn_Stmt*val))Cyc_Hashtable_insert)(succ_table,_tmpA6,_tmpA0);
goto _LL6;}default: _LL13: _tmpA1=((struct Cyc_JumpAnalysis_Fallthru_j_JumpAnalysis_Jumpee_struct*)_tmp84)->f1;_LL14: {
# 384
struct Cyc_JumpAnalysis_SaveEnv*_tmp9B=
((struct Cyc_JumpAnalysis_SaveEnv*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(_tmp7E,_tmpA1->body);
# 384
struct Cyc_JumpAnalysis_SaveEnv*_tmp9C=_tmp9B;int _tmp9E;int _tmp9D;_LL21: _tmp9E=_tmp9C->try_depth;_tmp9D=_tmp9C->pat_var_depth;_LL22:;
# 386
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,int val))Cyc_Hashtable_insert)(pop_table,_tmpA6,_tmpA4 - _tmp9E);{
int pv_pop;
if(Cyc_Tcpat_has_vars(_tmpA1->pat_vars))
pv_pop=(1 + _tmpA3)- _tmp9D;else{
# 391
pv_pop=_tmpA3 - _tmp9D;}
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,int val))Cyc_Hashtable_insert)(pat_pop_table,_tmpA6,pv_pop);
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,struct Cyc_Absyn_Stmt*val))Cyc_Hashtable_insert)(succ_table,_tmpA6,_tmpA1->body);
goto _LL6;};}}_LL6:;};};}}
# 397
L:((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key,struct Cyc_Hashtable_Table*val))Cyc_Hashtable_insert)(tables->pop_tables,fd,pop_table);
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key,struct Cyc_Hashtable_Table*val))Cyc_Hashtable_insert)(tables->succ_tables,fd,succ_table);
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key,struct Cyc_Hashtable_Table*val))Cyc_Hashtable_insert)(tables->pat_pop_tables,fd,pat_pop_table);};}
# 402
static void Cyc_JumpAnalysis_jump_analysis_iter(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*tds){
# 405
for(0;tds != 0;tds=tds->tl){
void*_tmpA8=((struct Cyc_Absyn_Decl*)tds->hd)->r;void*_tmpA9=_tmpA8;struct Cyc_Absyn_Fndecl*_tmpAC;struct Cyc_List_List*_tmpAB;struct Cyc_List_List*_tmpAA;switch(*((int*)_tmpA9)){case 2U: _LL1: _LL2:
 goto _LL4;case 3U: _LL3: _LL4:
 goto _LL6;case 4U: _LL5: _LL6:
 goto _LL8;case 0U: _LL7: _LL8:
 goto _LLA;case 8U: _LL9: _LLA:
 goto _LLC;case 5U: _LLB: _LLC:
 goto _LLE;case 6U: _LLD: _LLE:
 goto _LL10;case 7U: _LLF: _LL10:
 goto _LL12;case 13U: _LL11: _LL12:
 goto _LL14;case 14U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;case 12U: _LL17: _LL18:
 continue;case 9U: _LL19: _tmpAA=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpA9)->f2;_LL1A:
 _tmpAB=_tmpAA;goto _LL1C;case 10U: _LL1B: _tmpAB=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpA9)->f2;_LL1C:
 Cyc_JumpAnalysis_jump_analysis_iter(tables,_tmpAB);goto _LL0;default: _LL1D: _tmpAC=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpA9)->f1;_LL1E:
 Cyc_JumpAnalysis_anal_fd(tables,_tmpAC);goto _LL0;}_LL0:;}}
# 425
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds){
struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmpAD=({struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmpAE=_cycalloc(sizeof(*_tmpAE));(((({struct Cyc_Hashtable_Table*_tmpDF=((struct Cyc_Hashtable_Table*(*)())Cyc_JumpAnalysis_make_ptr_table)();(*_tmpAE).pop_tables=_tmpDF;}),({
struct Cyc_Hashtable_Table*_tmpDE=((struct Cyc_Hashtable_Table*(*)())Cyc_JumpAnalysis_make_ptr_table)();(*_tmpAE).succ_tables=_tmpDE;}))),({
struct Cyc_Hashtable_Table*_tmpDD=((struct Cyc_Hashtable_Table*(*)())Cyc_JumpAnalysis_make_ptr_table)();(*_tmpAE).pat_pop_tables=_tmpDD;}));_tmpAE;});
Cyc_JumpAnalysis_jump_analysis_iter(_tmpAD,tds);
return _tmpAD;}
