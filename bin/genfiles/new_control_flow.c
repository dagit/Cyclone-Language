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
 struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 113 "core.h"
void*Cyc_Core_snd(struct _tuple0*);
# 128
int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 265 "core.h"
void Cyc_Core_rethrow(void*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 70
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple1 Cyc_List_split(struct Cyc_List_List*x);
# 319
int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 87 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 113
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
# 126 "dict.h"
int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**ans);
# 149
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 37 "position.h"
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);struct Cyc_Position_Error;
# 47
extern int Cyc_Position_num_errors;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 972 "absyn.h"
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;
# 1013
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
# 1155
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1157
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
# 1169
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1171
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 1173
int Cyc_Absyn_is_require_union_type(void*);struct Cyc_RgnOrder_RgnPO;
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
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 55
void*Cyc_Tcutil_compress(void*t);
# 87
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t);
# 92
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 152
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 165
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 174
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 235 "tcutil.h"
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 267
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 277
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);
# 328
int Cyc_Tcutil_is_noreturn(void*);
# 346
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 350
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 353
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);struct _tuple12{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations.h"
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 49
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct _tuple13{struct Cyc_Absyn_Exp*f1;enum Cyc_Relations_Relation f2;struct Cyc_Absyn_Exp*f3;};
# 63
struct _tuple13 Cyc_Relations_primop2relation(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e2);
# 67
enum Cyc_Relations_Relation Cyc_Relations_flip_relation(enum Cyc_Relations_Relation r);
# 69
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*,struct Cyc_Relations_Reln*);
# 75
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p);
# 83
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 86
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation r,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns);
# 99
struct Cyc_List_List*Cyc_Relations_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
# 102
struct Cyc_List_List*Cyc_Relations_reln_assign_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 107
struct Cyc_List_List*Cyc_Relations_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 111
int Cyc_Relations_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);
# 117
void Cyc_Relations_print_relns(struct Cyc___cycFILE*,struct Cyc_List_List*);
# 120
struct _dyneither_ptr Cyc_Relations_rop2string(union Cyc_Relations_RelnOp r);
struct _dyneither_ptr Cyc_Relations_relation2string(enum Cyc_Relations_Relation r);
struct _dyneither_ptr Cyc_Relations_relns2string(struct Cyc_List_List*r);
# 128
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 56
void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key);struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 46 "jump_analysis.h"
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds);
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0U,Cyc_CfFlowInfo_AllIL  = 1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 87
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 94
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 138 "cf_flowinfo.h"
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 153
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env();
# 155
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f);
# 158
int Cyc_CfFlowInfo_root_cmp(void*,void*);
# 161
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);struct _tuple15{void*f1;struct Cyc_List_List*f2;};
struct _tuple15 Cyc_CfFlowInfo_unname_rval(void*rv);
# 168
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
# 171
int Cyc_CfFlowInfo_is_init_pointer(void*r);
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 174
void Cyc_CfFlowInfo_print_absrval(void*rval);
# 181
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f);
# 196 "cf_flowinfo.h"
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,void*r);
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place,void*r);
# 201
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple16{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple16 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct _tuple16 pr1,struct _tuple16 pr2);
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds);
# 37
extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned int f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _dyneither_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned int pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 115 "tcpat.h"
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars);
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
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 67 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 51 "new_control_flow.cyc"
static int Cyc_NewControlFlow_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2){
if((char*)s1.curr == (char*)s2.curr)
return 0;{
int i=0;
unsigned int sz1=_get_dyneither_size(s1,sizeof(char));
unsigned int sz2=_get_dyneither_size(s2,sizeof(char));
unsigned int minsz=sz1 < sz2?sz1: sz2;
# 59
while(i < minsz){
char c1=*((const char*)_check_dyneither_subscript(s1,sizeof(char),i));
char c2=*((const char*)_check_dyneither_subscript(s2,sizeof(char),i));
if(c1 == '\000'){
if(c2 == '\000')return 0;else{
return - 1;}}else{
if(c2 == '\000')return 1;else{
# 67
int diff=c1 - c2;
if(diff != 0)return diff;}}
# 70
++ i;}
# 72
if(sz1 == sz2)return 0;
if(minsz < sz2){
if(*((const char*)_check_dyneither_subscript(s2,sizeof(char),i))== '\000')return 0;else{
return - 1;}}else{
# 77
if(*((const char*)_check_dyneither_subscript(s1,sizeof(char),i))== '\000')return 0;else{
return 1;}}};}
# 83
int Cyc_NewControlFlow_warn_lose_unique=0;struct Cyc_NewControlFlow_CFStmtAnnot{int visited;};static char Cyc_NewControlFlow_CFAnnot[8U]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};struct Cyc_NewControlFlow_AnalEnv{struct Cyc_JumpAnalysis_Jump_Anal_Result*all_tables;struct Cyc_Hashtable_Table*succ_table;struct Cyc_Hashtable_Table*pat_pop_table;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;int noreturn;void*return_type;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;struct Cyc_List_List*return_relns;};struct _tuple17{void*f1;int f2;};
# 125 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*,struct _tuple17*);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple18{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal f2;};
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*);
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple19{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*,struct Cyc_CfFlowInfo_Place*,void*);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es);
# 135
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){
void*_tmp0=s->annot;void*_tmp1=_tmp0;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5;if(((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp1)->tag == Cyc_NewControlFlow_CFAnnot){_LL1: _tmp5=(struct Cyc_NewControlFlow_CFStmtAnnot*)&((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp1)->f1;_LL2:
 return _tmp5;}else{_LL3: _LL4:
({void*_tmp654=(void*)({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp2=_cycalloc_atomic(sizeof(*_tmp2));({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp653=({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp3;_tmp3.tag=Cyc_NewControlFlow_CFAnnot;({struct Cyc_NewControlFlow_CFStmtAnnot _tmp652=({struct Cyc_NewControlFlow_CFStmtAnnot _tmp4;_tmp4.visited=0;_tmp4;});_tmp3.f1=_tmp652;});_tmp3;});_tmp2[0]=_tmp653;});_tmp2;});s->annot=_tmp654;});return Cyc_NewControlFlow_get_stmt_annot(s);}_LL0:;}
# 142
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*res=({union Cyc_CfFlowInfo_FlowInfo*_tmp6=_cycalloc(sizeof(*_tmp6));({union Cyc_CfFlowInfo_FlowInfo _tmp655=Cyc_CfFlowInfo_BottomFL();_tmp6[0]=_tmp655;});_tmp6;});
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);
return res;}
# 149
return*sflow;}
# 152
static struct Cyc_List_List*Cyc_NewControlFlow_flowrelns(union Cyc_CfFlowInfo_FlowInfo f){
union Cyc_CfFlowInfo_FlowInfo _tmp7=f;struct Cyc_List_List*_tmp8;if((_tmp7.BottomFL).tag == 1){_LL1: _LL2:
 return 0;}else{_LL3: _tmp8=((_tmp7.ReachableFL).val).f2;_LL4:
 return _tmp8;}_LL0:;}struct _tuple20{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 159
static struct _tuple20 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp9=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmpA=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 163
({union Cyc_CfFlowInfo_FlowInfo _tmp656=Cyc_CfFlowInfo_join_flow(env->fenv,inflow,*_tmpA);*_tmpA=_tmp656;});
# 169
_tmp9->visited=env->iteration_num;
return({struct _tuple20 _tmpB;_tmpB.f1=_tmp9;_tmpB.f2=_tmpA;_tmpB;});}
# 179
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 187
({union Cyc_CfFlowInfo_FlowInfo _tmp657=Cyc_CfFlowInfo_join_flow(env->fenv,new_flow,env->tryflow);env->tryflow=_tmp657;});}struct _tuple21{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned int f2;struct Cyc_Dict_Dict f3;};
# 194
static void Cyc_NewControlFlow_check_unique_root(struct _tuple21*ckenv,void*root,void*rval){
# 196
struct _tuple21*_tmpC=ckenv;struct Cyc_NewControlFlow_AnalEnv*_tmp16;unsigned int _tmp15;struct Cyc_Dict_Dict _tmp14;_LL1: _tmp16=_tmpC->f1;_tmp15=_tmpC->f2;_tmp14=_tmpC->f3;_LL2:;{
void*_tmpD=root;struct Cyc_Absyn_Vardecl*_tmp13;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpD)->tag == 0U){_LL4: _tmp13=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpD)->f1;_LL5:
# 199
 if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(_tmp14,root,& rval) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp13->type)){
retry: {void*_tmpE=rval;void*_tmp12;switch(*((int*)_tmpE)){case 8U: _LL9: _tmp12=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpE)->f2;_LLA:
 rval=_tmp12;goto retry;case 7U: _LLB: _LLC:
 goto _LLE;case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpE)->f1 == Cyc_CfFlowInfo_NoneIL){_LLD: _LLE:
 goto _LL10;}else{goto _LL11;}case 0U: _LLF: _LL10:
 goto _LL8;default: _LL11: _LL12:
# 208
({struct Cyc_String_pa_PrintArg_struct _tmp11;_tmp11.tag=0U;({struct _dyneither_ptr _tmp658=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp13->name));_tmp11.f1=_tmp658;});({void*_tmpF[1U]={& _tmp11};({unsigned int _tmp65A=_tmp15;struct _dyneither_ptr _tmp659=({const char*_tmp10="alias-free pointer(s) reachable from %s may become unreachable.";_tag_dyneither(_tmp10,sizeof(char),64U);});Cyc_Warn_warn(_tmp65A,_tmp659,_tag_dyneither(_tmpF,sizeof(void*),1U));});});});
goto _LL8;}_LL8:;}}
# 212
goto _LL3;}else{_LL6: _LL7:
 goto _LL3;}_LL3:;};}
# 220
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp17=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp18=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp19=Cyc_CfFlowInfo_join_flow(env->fenv,flow,*_tmp18);
# 226
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple19 _tmp1A=({struct _tuple19 _tmp20;_tmp20.f1=flow;_tmp20.f2=_tmp19;_tmp20;});struct _tuple19 _tmp1B=_tmp1A;struct Cyc_Dict_Dict _tmp1F;struct Cyc_Dict_Dict _tmp1E;if(((_tmp1B.f1).ReachableFL).tag == 2){if(((_tmp1B.f2).ReachableFL).tag == 2){_LL1: _tmp1F=(((_tmp1B.f1).ReachableFL).val).f1;_tmp1E=(((_tmp1B.f2).ReachableFL).val).f1;_LL2: {
# 229
struct _tuple21 _tmp1C=({struct _tuple21 _tmp1D;_tmp1D.f1=env;_tmp1D.f2=s->loc;_tmp1D.f3=_tmp1E;_tmp1D;});
((void(*)(void(*f)(struct _tuple21*,void*,void*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& _tmp1C,_tmp1F);
goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 235
if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp19,*_tmp18)){
# 242
*_tmp18=_tmp19;
# 246
if(_tmp17->visited == env->iteration_num)
# 248
env->iterate_again=1;}}
# 253
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned int loc,int nameit){
# 257
union Cyc_CfFlowInfo_FlowInfo _tmp21=inflow;struct Cyc_Dict_Dict _tmp29;struct Cyc_List_List*_tmp28;if((_tmp21.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmp29=((_tmp21.ReachableFL).val).f1;_tmp28=((_tmp21.ReachableFL).val).f2;_LL4:
# 260
 for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp22=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp26=_cycalloc(sizeof(*_tmp26));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp65B=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp27;_tmp27.tag=0U;_tmp27.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp27;});_tmp26[0]=_tmp65B;});_tmp26;});
void*_tmp23=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit)
({void*_tmp65D=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp24=_cycalloc(sizeof(*_tmp24));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp65C=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp25;_tmp25.tag=8U;_tmp25.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp25.f2=_tmp23;_tmp25;});_tmp24[0]=_tmp65C;});_tmp24;});_tmp23=_tmp65D;});
# 267
({struct Cyc_Dict_Dict _tmp65E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp29,(void*)_tmp22,_tmp23);_tmp29=_tmp65E;});}
# 269
return Cyc_CfFlowInfo_ReachableFL(_tmp29,_tmp28);}_LL0:;}
# 273
static int Cyc_NewControlFlow_relns_ok(struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 280
for(0;(unsigned int)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp2A=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)req->hd);
if(Cyc_Relations_consistent_relations(({struct Cyc_List_List*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->hd=_tmp2A;_tmp2B->tl=assume;_tmp2B;})))
return 0;}
# 285
return 1;}
# 288
static struct Cyc_Absyn_Exp*Cyc_NewControlFlow_strip_cast(struct Cyc_Absyn_Exp*e){
void*_tmp2C=e->r;void*_tmp2D=_tmp2C;struct Cyc_Absyn_Exp*_tmp2E;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2D)->tag == 14U){_LL1: _tmp2E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2D)->f2;_LL2:
 return _tmp2E;}else{_LL3: _LL4:
 return e;}_LL0:;}
# 295
static void Cyc_NewControlFlow_check_union_requires(unsigned int loc,void*t,struct _dyneither_ptr*f,union Cyc_CfFlowInfo_FlowInfo inflow){
# 297
union Cyc_CfFlowInfo_FlowInfo _tmp2F=inflow;struct Cyc_List_List*_tmp4D;if((_tmp2F.BottomFL).tag == 1){_LL1: _LL2:
 return;}else{_LL3: _tmp4D=((_tmp2F.ReachableFL).val).f2;_LL4:
# 300
{void*_tmp30=Cyc_Tcutil_compress(t);void*_tmp31=_tmp30;struct Cyc_List_List*_tmp4C;union Cyc_Absyn_AggrInfoU _tmp4B;struct Cyc_List_List*_tmp4A;switch(*((int*)_tmp31)){case 11U: _LL6: _tmp4B=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp31)->f1).aggr_info;_tmp4A=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp31)->f1).targs;_LL7: {
# 302
struct Cyc_Absyn_Aggrdecl*_tmp32=Cyc_Absyn_get_known_aggrdecl(_tmp4B);
struct Cyc_Absyn_Aggrfield*_tmp33=Cyc_Absyn_lookup_decl_field(_tmp32,f);
struct Cyc_Absyn_Exp*_tmp34=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp33))->requires_clause;
if(_tmp34 != 0){
struct _RegionHandle _tmp35=_new_region("temp");struct _RegionHandle*temp=& _tmp35;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp36=({struct _RegionHandle*_tmp660=temp;struct Cyc_List_List*_tmp65F=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp32->tvs,_tmp4A);Cyc_Tcutil_rsubsexp(_tmp660,_tmp65F,_tmp34);});
# 309
if(!({struct Cyc_List_List*_tmp661=_tmp4D;Cyc_NewControlFlow_relns_ok(_tmp661,Cyc_Relations_exp2relns(temp,_tmp36));})){
({struct Cyc_String_pa_PrintArg_struct _tmp3A;_tmp3A.tag=0U;_tmp3A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp39;_tmp39.tag=0U;({struct _dyneither_ptr _tmp662=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp36)));_tmp39.f1=_tmp662;});({void*_tmp37[2U]={& _tmp39,& _tmp3A};({unsigned int _tmp664=loc;struct _dyneither_ptr _tmp663=({const char*_tmp38="unable to prove %s, required to access %s";_tag_dyneither(_tmp38,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp664,_tmp663,_tag_dyneither(_tmp37,sizeof(void*),2U));});});});});
({void*_tmp3B=0U;({struct Cyc___cycFILE*_tmp666=Cyc_stderr;struct _dyneither_ptr _tmp665=({const char*_tmp3C="  [recorded facts on path: ";_tag_dyneither(_tmp3C,sizeof(char),28U);});Cyc_fprintf(_tmp666,_tmp665,_tag_dyneither(_tmp3B,sizeof(void*),0U));});});
Cyc_Relations_print_relns(Cyc_stderr,_tmp4D);
({void*_tmp3D=0U;({struct Cyc___cycFILE*_tmp668=Cyc_stderr;struct _dyneither_ptr _tmp667=({const char*_tmp3E="]\n";_tag_dyneither(_tmp3E,sizeof(char),3U);});Cyc_fprintf(_tmp668,_tmp667,_tag_dyneither(_tmp3D,sizeof(void*),0U));});});}}
# 307
;_pop_region(temp);}
# 316
goto _LL5;}case 12U: _LL8: _tmp4C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp31)->f2;_LL9: {
# 318
struct Cyc_Absyn_Aggrfield*_tmp3F=Cyc_Absyn_lookup_field(_tmp4C,f);
struct Cyc_Absyn_Exp*_tmp40=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3F))->requires_clause;
if(_tmp40 != 0){
struct _RegionHandle _tmp41=_new_region("temp");struct _RegionHandle*temp=& _tmp41;_push_region(temp);
if(!({struct Cyc_List_List*_tmp669=_tmp4D;Cyc_NewControlFlow_relns_ok(_tmp669,Cyc_Relations_exp2relns(temp,_tmp40));})){
({struct Cyc_String_pa_PrintArg_struct _tmp45;_tmp45.tag=0U;_tmp45.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp44;_tmp44.tag=0U;({struct _dyneither_ptr _tmp66A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp40)));_tmp44.f1=_tmp66A;});({void*_tmp42[2U]={& _tmp44,& _tmp45};({unsigned int _tmp66C=loc;struct _dyneither_ptr _tmp66B=({const char*_tmp43="unable to prove %s, required to access %s";_tag_dyneither(_tmp43,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp66C,_tmp66B,_tag_dyneither(_tmp42,sizeof(void*),2U));});});});});
({void*_tmp46=0U;({struct Cyc___cycFILE*_tmp66E=Cyc_stderr;struct _dyneither_ptr _tmp66D=({const char*_tmp47="  [recorded facts on path: ";_tag_dyneither(_tmp47,sizeof(char),28U);});Cyc_fprintf(_tmp66E,_tmp66D,_tag_dyneither(_tmp46,sizeof(void*),0U));});});
Cyc_Relations_print_relns(Cyc_stderr,_tmp4D);
({void*_tmp48=0U;({struct Cyc___cycFILE*_tmp670=Cyc_stderr;struct _dyneither_ptr _tmp66F=({const char*_tmp49="]\n";_tag_dyneither(_tmp49,sizeof(char),3U);});Cyc_fprintf(_tmp670,_tmp66F,_tag_dyneither(_tmp48,sizeof(void*),0U));});});}
# 322
;_pop_region(temp);}
# 329
goto _LL5;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 332
goto _LL0;}_LL0:;}
# 336
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp4E=inflow;struct Cyc_Dict_Dict _tmp54;struct Cyc_List_List*_tmp53;if((_tmp4E.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmp54=((_tmp4E.ReachableFL).val).f1;_tmp53=((_tmp4E.ReachableFL).val).f2;_LL4:
# 340
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp54,r)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp4F=0U;({unsigned int _tmp672=loc;struct _dyneither_ptr _tmp671=({const char*_tmp50="expression may not be fully initialized";_tag_dyneither(_tmp50,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp672,_tmp671,_tag_dyneither(_tmp4F,sizeof(void*),0U));});});{
struct Cyc_Dict_Dict _tmp51=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp54,r);
if(_tmp54.t == _tmp51.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp52=Cyc_CfFlowInfo_ReachableFL(_tmp51,_tmp53);
Cyc_NewControlFlow_update_tryflow(env,_tmp52);
return _tmp52;};};}_LL0:;}struct _tuple22{struct Cyc_Absyn_Tqual f1;void*f2;};
# 350
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp55=({struct _tuple0 _tmp68;({void*_tmp673=Cyc_Tcutil_compress(t);_tmp68.f1=_tmp673;});_tmp68.f2=r;_tmp68;});struct _tuple0 _tmp56=_tmp55;enum Cyc_Absyn_AggrKind _tmp67;struct Cyc_List_List*_tmp66;struct _dyneither_ptr _tmp65;union Cyc_Absyn_AggrInfoU _tmp64;struct Cyc_List_List*_tmp63;struct _dyneither_ptr _tmp62;struct Cyc_List_List*_tmp61;struct _dyneither_ptr _tmp60;struct Cyc_Absyn_Datatypefield*_tmp5F;struct _dyneither_ptr _tmp5E;void*_tmp5D;switch(*((int*)_tmp56.f2)){case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp56.f2)->f1 == Cyc_CfFlowInfo_NoneIL){_LL1: _LL2:
 return;}else{switch(*((int*)_tmp56.f1)){case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp56.f1)->f1).field_info).KnownDatatypefield).tag == 2)goto _LL13;else{goto _LL13;}case 10U: goto _LL13;case 11U: goto _LL13;case 12U: goto _LL13;case 5U: goto _LL11;default: goto _LL13;}}case 0U: _LL3: _LL4:
 return;case 7U: _LL5: _LL6:
 return;case 8U: _LL7: _tmp5D=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp56.f2)->f2;_LL8:
 Cyc_NewControlFlow_check_nounique(env,loc,t,_tmp5D);return;default: switch(*((int*)_tmp56.f1)){case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp56.f1)->f1).field_info).KnownDatatypefield).tag == 2){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp56.f2)->tag == 6U){_LL9: _tmp5F=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp56.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp5E=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp56.f2)->f2;_LLA:
# 357
 if(_tmp5F->typs == 0)
return;
_tmp61=_tmp5F->typs;_tmp60=_tmp5E;goto _LLC;}else{goto _LL13;}}else{goto _LL13;}case 10U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp56.f2)->tag == 6U){_LLB: _tmp61=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp56.f1)->f1;_tmp60=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp56.f2)->f2;_LLC: {
# 361
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp61);
{int i=0;for(0;i < sz;(i ++,_tmp61=_tmp61->tl)){
({struct Cyc_NewControlFlow_AnalEnv*_tmp676=env;unsigned int _tmp675=loc;void*_tmp674=(*((struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp61))->hd)).f2;Cyc_NewControlFlow_check_nounique(_tmp676,_tmp675,_tmp674,*((void**)_check_dyneither_subscript(_tmp60,sizeof(void*),i)));});}}
# 365
return;}}else{goto _LL13;}case 11U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp56.f2)->tag == 6U){_LLD: _tmp64=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp56.f1)->f1).aggr_info;_tmp63=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp56.f1)->f1).targs;_tmp62=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp56.f2)->f2;_LLE: {
# 368
struct Cyc_Absyn_Aggrdecl*_tmp57=Cyc_Absyn_get_known_aggrdecl(_tmp64);
if(_tmp57->impl == 0)return;{
struct Cyc_List_List*_tmp58=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp57->impl))->fields;
struct _RegionHandle _tmp59=_new_region("temp");struct _RegionHandle*temp=& _tmp59;_push_region(temp);
{struct Cyc_List_List*_tmp5A=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp57->tvs,_tmp63);
{int i=0;for(0;i < _get_dyneither_size(_tmp62,sizeof(void*));(i ++,_tmp58=_tmp58->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp58))->hd)->type;
if(_tmp5A != 0)({void*_tmp677=Cyc_Tcutil_rsubstitute(temp,_tmp5A,t);t=_tmp677;});
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)_tmp62.curr)[i]);}}
# 378
_npop_handler(0U);return;}
# 372
;_pop_region(temp);};}}else{goto _LL13;}case 12U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp56.f2)->tag == 6U){_LLF: _tmp67=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp56.f1)->f1;_tmp66=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp56.f1)->f2;_tmp65=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp56.f2)->f2;_LL10:
# 381
{int i=0;for(0;i < _get_dyneither_size(_tmp65,sizeof(void*));(i ++,_tmp66=_tmp66->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp66))->hd)->type,((void**)_tmp65.curr)[i]);}}
return;}else{goto _LL13;}case 5U: _LL11: _LL12:
# 385
 if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp5B=0U;({unsigned int _tmp679=loc;struct _dyneither_ptr _tmp678=({const char*_tmp5C="argument may still contain alias-free pointers";_tag_dyneither(_tmp5C,sizeof(char),47U);});Cyc_Warn_warn(_tmp679,_tmp678,_tag_dyneither(_tmp5B,sizeof(void*),0U));});});
return;default: _LL13: _LL14:
 return;}}_LL0:;}
# 392
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp69=inflow;struct Cyc_Dict_Dict _tmp76;struct Cyc_List_List*_tmp75;if((_tmp69.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmp76=((_tmp69.ReachableFL).val).f1;_tmp75=((_tmp69.ReachableFL).val).f2;_LL4:
# 396
 if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
({void*_tmp6A=0U;({struct _dyneither_ptr _tmp67A=({const char*_tmp6B="noliveunique attribute requires alias-free pointer";_tag_dyneither(_tmp6B,sizeof(char),51U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp67A,_tag_dyneither(_tmp6A,sizeof(void*),0U));});});
return Cyc_CfFlowInfo_BottomFL();}{
# 400
void*_tmp6C=Cyc_Tcutil_pointer_elt_type(t);
retry: {void*_tmp6D=r;void*_tmp72;struct Cyc_CfFlowInfo_Place*_tmp71;void*_tmp70;switch(*((int*)_tmp6D)){case 8U: _LL6: _tmp70=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp6D)->f2;_LL7:
 r=_tmp70;goto retry;case 4U: _LL8: _tmp71=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp6D)->f1;_LL9:
({void*_tmp67B=Cyc_CfFlowInfo_lookup_place(_tmp76,_tmp71);_tmp72=_tmp67B;});goto _LLB;case 5U: _LLA: _tmp72=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp6D)->f1;_LLB:
 Cyc_NewControlFlow_check_nounique(env,loc,_tmp6C,_tmp72);goto _LL5;default: _LLC: _LLD:
# 406
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp6C))
({void*_tmp6E=0U;({unsigned int _tmp67D=loc;struct _dyneither_ptr _tmp67C=({const char*_tmp6F="argument may contain live alias-free pointers";_tag_dyneither(_tmp6F,sizeof(char),46U);});Cyc_Warn_warn(_tmp67D,_tmp67C,_tag_dyneither(_tmp6E,sizeof(void*),0U));});});
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);}_LL5:;}{
# 410
struct Cyc_Dict_Dict _tmp73=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp76,r);
if(_tmp76.t == _tmp73.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp74=Cyc_CfFlowInfo_ReachableFL(_tmp73,_tmp75);
Cyc_NewControlFlow_update_tryflow(env,_tmp74);
return _tmp74;};};};}_LL0:;}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 420
static struct _tuple23 Cyc_NewControlFlow_anal_Rexps(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 425
struct Cyc_List_List*rvals=0;
if(es == 0)
return({struct _tuple23 _tmp77;_tmp77.f1=inflow;_tmp77.f2=0;_tmp77;});
# 430
do{
struct _tuple16 _tmp78=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);struct _tuple16 _tmp79=_tmp78;union Cyc_CfFlowInfo_FlowInfo _tmp7C;void*_tmp7B;_LL1: _tmp7C=_tmp79.f1;_tmp7B=_tmp79.f2;_LL2:;
inflow=_tmp7C;
({struct Cyc_List_List*_tmp67E=({struct Cyc_List_List*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->hd=_tmp7B;_tmp7A->tl=rvals;_tmp7A;});rvals=_tmp67E;});
es=es->tl;
first_is_copy=others_are_copy;}while(es != 0);
# 438
Cyc_NewControlFlow_update_tryflow(env,inflow);
return({struct _tuple23 _tmp7D;_tmp7D.f1=inflow;({struct Cyc_List_List*_tmp67F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals);_tmp7D.f2=_tmp67F;});_tmp7D;});}
# 444
static struct _tuple16 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 446
struct _tuple23 _tmp7E=
Cyc_NewControlFlow_anal_Rexps(env,inflow,es,0,0);
# 446
struct _tuple23 _tmp7F=_tmp7E;union Cyc_CfFlowInfo_FlowInfo _tmp86;struct Cyc_List_List*_tmp85;_LL1: _tmp86=_tmp7F.f1;_tmp85=_tmp7F.f2;_LL2:;
# 448
{union Cyc_CfFlowInfo_FlowInfo _tmp80=_tmp86;struct Cyc_Dict_Dict _tmp83;if((_tmp80.ReachableFL).tag == 2){_LL4: _tmp83=((_tmp80.ReachableFL).val).f1;_LL5:
# 450
 for(0;_tmp85 != 0;(_tmp85=_tmp85->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp83,(void*)_tmp85->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp81=0U;({unsigned int _tmp681=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _dyneither_ptr _tmp680=({const char*_tmp82="expression may not be initialized";_tag_dyneither(_tmp82,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp681,_tmp680,_tag_dyneither(_tmp81,sizeof(void*),0U));});});}
goto _LL3;}else{_LL6: _LL7:
 goto _LL3;}_LL3:;}
# 456
return({struct _tuple16 _tmp84;_tmp84.f1=_tmp86;_tmp84.f2=(env->fenv)->unknown_all;_tmp84;});}
# 463
static void*Cyc_NewControlFlow_consume_zero_rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Dict_Dict new_dict,struct Cyc_CfFlowInfo_Place*p,struct Cyc_Absyn_Exp*e,void*new_rval){
# 470
int needs_unconsume=0;
void*_tmp87=Cyc_CfFlowInfo_lookup_place(new_dict,p);
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp87,& needs_unconsume))
({void*_tmp88=0U;({struct _dyneither_ptr _tmp682=({const char*_tmp89="consume_zero_rval";_tag_dyneither(_tmp89,sizeof(char),18U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp682,_tag_dyneither(_tmp88,sizeof(void*),0U));});});else{
# 475
if(needs_unconsume)
return Cyc_CfFlowInfo_make_unique_consumed(env->fenv,(void*)_check_null(e->topt),e,env->iteration_num,new_rval);else{
# 479
return new_rval;}}}
# 492 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 497
union Cyc_CfFlowInfo_FlowInfo _tmp8A=outflow;struct Cyc_Dict_Dict _tmp92;struct Cyc_List_List*_tmp91;if((_tmp8A.BottomFL).tag == 1){_LL1: _LL2:
 return outflow;}else{_LL3: _tmp92=((_tmp8A.ReachableFL).val).f1;_tmp91=((_tmp8A.ReachableFL).val).f2;_LL4: {
# 500
union Cyc_CfFlowInfo_AbsLVal _tmp8B=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmp8C=_tmp8B;struct Cyc_CfFlowInfo_Place*_tmp90;if((_tmp8C.UnknownL).tag == 2){_LL6: _LL7:
# 504
 return outflow;}else{_LL8: _tmp90=(_tmp8C.PlaceL).val;_LL9: {
# 508
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->unknown_none;
for(0;names != 0;names=names->tl){
({void*_tmp684=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp683=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp8E;_tmp8E.tag=8U;_tmp8E.f1=(struct Cyc_Absyn_Vardecl*)names->hd;_tmp8E.f2=nzval;_tmp8E;});_tmp8D[0]=_tmp683;});_tmp8D;});nzval=_tmp684;});}
# 512
({void*_tmp685=Cyc_NewControlFlow_consume_zero_rval(env,_tmp92,_tmp90,e,nzval);nzval=_tmp685;});{
union Cyc_CfFlowInfo_FlowInfo _tmp8F=({struct Cyc_Dict_Dict _tmp686=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp92,_tmp90,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp686,_tmp91);});
# 517
return _tmp8F;};}}_LL5:;}}_LL0:;}
# 526
static struct _tuple19 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 528
union Cyc_CfFlowInfo_FlowInfo _tmp93=outflow;struct Cyc_Dict_Dict _tmp9F;struct Cyc_List_List*_tmp9E;if((_tmp93.BottomFL).tag == 1){_LL1: _LL2:
 return({struct _tuple19 _tmp94;_tmp94.f1=outflow;_tmp94.f2=outflow;_tmp94;});}else{_LL3: _tmp9F=((_tmp93.ReachableFL).val).f1;_tmp9E=((_tmp93.ReachableFL).val).f2;_LL4: {
# 531
union Cyc_CfFlowInfo_AbsLVal _tmp95=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmp96=_tmp95;struct Cyc_CfFlowInfo_Place*_tmp9D;if((_tmp96.UnknownL).tag == 2){_LL6: _LL7:
 return({struct _tuple19 _tmp97;_tmp97.f1=outflow;_tmp97.f2=outflow;_tmp97;});}else{_LL8: _tmp9D=(_tmp96.PlaceL).val;_LL9: {
# 534
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->unknown_none;
void*zval=(env->fenv)->zero;
for(0;names != 0;names=names->tl){
({void*_tmp688=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp98=_cycalloc(sizeof(*_tmp98));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp687=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp99;_tmp99.tag=8U;_tmp99.f1=(struct Cyc_Absyn_Vardecl*)names->hd;_tmp99.f2=nzval;_tmp99;});_tmp98[0]=_tmp687;});_tmp98;});nzval=_tmp688;});
({void*_tmp68A=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp689=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp9B;_tmp9B.tag=8U;_tmp9B.f1=(struct Cyc_Absyn_Vardecl*)names->hd;_tmp9B.f2=zval;_tmp9B;});_tmp9A[0]=_tmp689;});_tmp9A;});zval=_tmp68A;});}
# 540
({void*_tmp68B=Cyc_NewControlFlow_consume_zero_rval(env,_tmp9F,_tmp9D,e,nzval);nzval=_tmp68B;});
({void*_tmp68C=Cyc_NewControlFlow_consume_zero_rval(env,_tmp9F,_tmp9D,e,zval);zval=_tmp68C;});
return({struct _tuple19 _tmp9C;({union Cyc_CfFlowInfo_FlowInfo _tmp68E=({
struct Cyc_Dict_Dict _tmp68D=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp9F,_tmp9D,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp68D,_tmp9E);});_tmp9C.f1=_tmp68E;});({union Cyc_CfFlowInfo_FlowInfo _tmp690=({
struct Cyc_Dict_Dict _tmp68F=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp9F,_tmp9D,zval);Cyc_CfFlowInfo_ReachableFL(_tmp68F,_tmp9E);});_tmp9C.f2=_tmp690;});_tmp9C;});}}_LL5:;}}_LL0:;}
# 549
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 552
static void Cyc_NewControlFlow_update_relns(struct Cyc_Absyn_Exp*e,int possibly_null,struct Cyc_List_List*relns){
void*_tmpA2=e->r;void*_tmpA3=_tmpA2;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA3)->tag == 23U){_LL1: _LL2:
# 556
{void*_tmpA4=e->annot;void*_tmpA5=_tmpA4;struct Cyc_List_List*_tmpA7;struct Cyc_List_List*_tmpA6;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpA5)->tag == Cyc_CfFlowInfo_UnknownZ){_LL6: _tmpA6=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpA5)->f1;_LL7:
# 558
 if(possibly_null  && Cyc_Relations_same_relns(relns,_tmpA6))return;
goto _LL5;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpA5)->tag == Cyc_CfFlowInfo_NotZero){_LL8: _tmpA7=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpA5)->f1;_LL9:
# 561
 possibly_null=0;
if(Cyc_Relations_same_relns(relns,_tmpA7))return;
goto _LL5;}else{_LLA: _LLB:
 goto _LL5;}}_LL5:;}
# 566
if(possibly_null)
({void*_tmp692=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpA8=_cycalloc(sizeof(*_tmpA8));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp691=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmpA9;_tmpA9.tag=Cyc_CfFlowInfo_UnknownZ;_tmpA9.f1=relns;_tmpA9;});_tmpA8[0]=_tmp691;});_tmpA8;});e->annot=_tmp692;});else{
# 570
({void*_tmp694=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpAA=_cycalloc(sizeof(*_tmpAA));({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp693=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmpAB;_tmpAB.tag=Cyc_CfFlowInfo_NotZero;_tmpAB.f1=relns;_tmpAB;});_tmpAA[0]=_tmp693;});_tmpAA;});e->annot=_tmp694;});}
# 572
goto _LL0;}else{_LL3: _LL4:
# 576
 if(possibly_null){
void*_tmpAC=e->annot;void*_tmpAD=_tmpAC;struct Cyc_List_List*_tmpAE;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpAD)->tag == Cyc_CfFlowInfo_UnknownZ){_LLD: _tmpAE=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpAD)->f1;_LLE:
# 579
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
return;}else{_LLF: _LL10:
 goto _LLC;}_LLC:;}
# 584
e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL0;}_LL0:;}
# 594
static struct _tuple16 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r){
# 597
void*_tmpAF=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpB0=_tmpAF;void*_tmpDB;union Cyc_Absyn_Constraint*_tmpDA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB0)->tag == 5U){_LL1: _tmpDB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB0)->f1).elt_typ;_tmpDA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB0)->f1).ptr_atts).bounds;_LL2: {
# 599
union Cyc_CfFlowInfo_FlowInfo _tmpB1=f;struct Cyc_Dict_Dict _tmpD7;struct Cyc_List_List*_tmpD6;if((_tmpB1.BottomFL).tag == 1){_LL6: _LL7:
# 601
 return({struct _tuple16 _tmpB2;_tmpB2.f1=f;({void*_tmp695=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpDB,0,(env->fenv)->unknown_all);_tmpB2.f2=_tmp695;});_tmpB2;});}else{_LL8: _tmpD7=((_tmpB1.ReachableFL).val).f1;_tmpD6=((_tmpB1.ReachableFL).val).f2;_LL9:
# 604
 if(Cyc_Tcutil_is_bound_one(_tmpDA)){
struct _tuple15 _tmpB3=Cyc_CfFlowInfo_unname_rval(r);struct _tuple15 _tmpB4=_tmpB3;void*_tmpC5;struct Cyc_List_List*_tmpC4;_LLB: _tmpC5=_tmpB4.f1;_tmpC4=_tmpB4.f2;_LLC:;{
void*_tmpB5=_tmpC5;enum Cyc_CfFlowInfo_InitLevel _tmpC3;void*_tmpC2;struct Cyc_CfFlowInfo_Place*_tmpC1;struct Cyc_Absyn_Vardecl*_tmpC0;void*_tmpBF;switch(*((int*)_tmpB5)){case 8U: _LLE: _tmpC0=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpB5)->f1;_tmpBF=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpB5)->f2;_LLF:
# 608
({void*_tmpB6=0U;({struct _dyneither_ptr _tmp696=({const char*_tmpB7="named location in anal_derefR";_tag_dyneither(_tmpB7,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp696,_tag_dyneither(_tmpB6,sizeof(void*),0U));});});case 1U: _LL10: _LL11:
# 610
 Cyc_NewControlFlow_update_relns(e,0,_tmpD6);
goto _LLD;case 4U: _LL12: _tmpC1=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpB5)->f1;_LL13:
# 613
 Cyc_NewControlFlow_update_relns(e,0,_tmpD6);
return({struct _tuple16 _tmpB8;_tmpB8.f1=f;({void*_tmp697=Cyc_CfFlowInfo_lookup_place(_tmpD7,_tmpC1);_tmpB8.f2=_tmp697;});_tmpB8;});case 5U: _LL14: _tmpC2=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmpB5)->f1;_LL15:
# 616
 Cyc_NewControlFlow_update_relns(e,1,_tmpD6);
return({struct _tuple16 _tmpB9;_tmpB9.f1=f;_tmpB9.f2=_tmpC2;_tmpB9;});case 0U: _LL16: _LL17:
# 619
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple16 _tmpBA;({union Cyc_CfFlowInfo_FlowInfo _tmp698=Cyc_CfFlowInfo_BottomFL();_tmpBA.f1=_tmp698;});({void*_tmp699=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpDB,0,(env->fenv)->unknown_all);_tmpBA.f2=_tmp699;});_tmpBA;});case 2U: _LL18: _tmpC3=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpB5)->f1;_LL19:
# 623
({union Cyc_CfFlowInfo_FlowInfo _tmp69A=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpC3,_tmpC4);f=_tmp69A;});
goto _LL1B;default: _LL1A: _LL1B:
# 626
{void*_tmpBB=e->r;void*_tmpBC=_tmpBB;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBC)->tag == 23U){_LL1D: _LL1E:
# 628
({void*_tmp69C=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpBD=_cycalloc(sizeof(*_tmpBD));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp69B=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmpBE;_tmpBE.tag=Cyc_CfFlowInfo_UnknownZ;_tmpBE.f1=_tmpD6;_tmpBE;});_tmpBD[0]=_tmp69B;});_tmpBD;});e->annot=_tmp69C;});
goto _LL1C;}else{_LL1F: _LL20:
# 631
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL1C;}_LL1C:;}
# 634
goto _LLD;}_LLD:;};}else{
# 637
void*_tmpC6=e->annot;void*_tmpC7=_tmpC6;struct Cyc_List_List*_tmpCA;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpC7)->tag == Cyc_CfFlowInfo_UnknownZ){_LL22: _tmpCA=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpC7)->f1;_LL23:
# 639
 if(!Cyc_Relations_same_relns(_tmpD6,_tmpCA))goto _LL25;
goto _LL21;}else{_LL24: _LL25:
# 642
({void*_tmp69E=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpC8=_cycalloc(sizeof(*_tmpC8));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp69D=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmpC9;_tmpC9.tag=Cyc_CfFlowInfo_UnknownZ;_tmpC9.f1=_tmpD6;_tmpC9;});_tmpC8[0]=_tmp69D;});_tmpC8;});e->annot=_tmp69E;});
goto _LL21;}_LL21:;}{
# 646
enum Cyc_CfFlowInfo_InitLevel _tmpCB=Cyc_CfFlowInfo_initlevel(env->fenv,_tmpD7,r);enum Cyc_CfFlowInfo_InitLevel _tmpCC=_tmpCB;if(_tmpCC == Cyc_CfFlowInfo_NoneIL){_LL27: _LL28: {
# 648
struct _tuple15 _tmpCD=Cyc_CfFlowInfo_unname_rval(r);struct _tuple15 _tmpCE=_tmpCD;void*_tmpD4;_LL2C: _tmpD4=_tmpCE.f1;_LL2D:;
{void*_tmpCF=_tmpD4;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpCF)->tag == 7U){_LL2F: _LL30:
# 651
({void*_tmpD0=0U;({unsigned int _tmp6A0=e->loc;struct _dyneither_ptr _tmp69F=({const char*_tmpD1="attempt to dereference a consumed alias-free pointer";_tag_dyneither(_tmpD1,sizeof(char),53U);});Cyc_CfFlowInfo_aerr(_tmp6A0,_tmp69F,_tag_dyneither(_tmpD0,sizeof(void*),0U));});});
goto _LL2E;}else{_LL31: _LL32:
# 654
({void*_tmpD2=0U;({unsigned int _tmp6A2=e->loc;struct _dyneither_ptr _tmp6A1=({const char*_tmpD3="dereference of possibly uninitialized pointer";_tag_dyneither(_tmpD3,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp6A2,_tmp6A1,_tag_dyneither(_tmpD2,sizeof(void*),0U));});});}_LL2E:;}
# 656
goto _LL2A;}}else{_LL29: _LL2A:
# 658
 return({struct _tuple16 _tmpD5;_tmpD5.f1=f;({void*_tmp6A3=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpDB,0,(env->fenv)->unknown_all);_tmpD5.f2=_tmp6A3;});_tmpD5;});}_LL26:;};}_LL5:;}}else{_LL3: _LL4:
# 661
({void*_tmpD8=0U;({struct _dyneither_ptr _tmp6A4=({const char*_tmpD9="right deref of non-pointer-type";_tag_dyneither(_tmpD9,sizeof(char),32U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6A4,_tag_dyneither(_tmpD8,sizeof(void*),0U));});});}_LL0:;}
# 668
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 670
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
int e2_valid_op=Cyc_Relations_exp2relnop(e2,& n2);
# 673
{void*_tmpDC=e1->r;void*_tmpDD=_tmpDC;void*_tmpDF;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDD)->tag == 1U){_LL1: _tmpDF=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDD)->f1;_LL2: {
# 675
struct Cyc_Absyn_Vardecl*_tmpDE=Cyc_Tcutil_nonesc_vardecl(_tmpDF);
if(_tmpDE != 0){
union Cyc_Relations_RelnOp n1=Cyc_Relations_RNumelts(_tmpDE);
if(e2_valid_op)
({struct Cyc_List_List*_tmp6A5=Cyc_Relations_add_relation(Cyc_Core_heap_region,n2,Cyc_Relations_Rlt,n1,relns);relns=_tmp6A5;});}
# 681
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 685
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound((void*)_check_null(e1->topt));
if(bound != 0){
union Cyc_Relations_RelnOp rbound=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(bound,& rbound))
({struct Cyc_List_List*_tmp6A6=Cyc_Relations_add_relation(Cyc_Core_heap_region,n2,Cyc_Relations_Rlt,rbound,relns);relns=_tmp6A6;});}
# 691
return relns;};}
# 699
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_restore_noconsume_arg(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*exp,unsigned int loc,void*old_rval){
# 704
struct _tuple18 _tmpE0=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);struct _tuple18 _tmpE1=_tmpE0;union Cyc_CfFlowInfo_AbsLVal _tmpF1;_LL1: _tmpF1=_tmpE1.f2;_LL2:;
{struct _tuple18 _tmpE2=({struct _tuple18 _tmpF0;_tmpF0.f1=inflow;_tmpF0.f2=_tmpF1;_tmpF0;});struct _tuple18 _tmpE3=_tmpE2;struct Cyc_Dict_Dict _tmpEF;struct Cyc_List_List*_tmpEE;struct Cyc_CfFlowInfo_Place*_tmpED;if(((_tmpE3.f1).ReachableFL).tag == 2){if(((_tmpE3.f2).PlaceL).tag == 1){_LL4: _tmpEF=(((_tmpE3.f1).ReachableFL).val).f1;_tmpEE=(((_tmpE3.f1).ReachableFL).val).f2;_tmpED=((_tmpE3.f2).PlaceL).val;_LL5: {
# 707
void*_tmpE4=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,(void*)_check_null(exp->topt),0,(env->fenv)->unknown_all);
# 709
struct _tuple15 _tmpE5=Cyc_CfFlowInfo_unname_rval(old_rval);struct _tuple15 _tmpE6=_tmpE5;void*_tmpEA;struct Cyc_List_List*_tmpE9;_LLB: _tmpEA=_tmpE6.f1;_tmpE9=_tmpE6.f2;_LLC:;
for(0;_tmpE9 != 0;_tmpE9=_tmpE9->tl){
({void*_tmp6A8=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpE7=_cycalloc(sizeof(*_tmpE7));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6A7=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmpE8;_tmpE8.tag=8U;_tmpE8.f1=(struct Cyc_Absyn_Vardecl*)_tmpE9->hd;_tmpE8.f2=_tmpE4;_tmpE8;});_tmpE7[0]=_tmp6A7;});_tmpE7;});_tmpE4=_tmp6A8;});}
# 714
({struct Cyc_Dict_Dict _tmp6A9=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmpEF,_tmpED,_tmpE4);_tmpEF=_tmp6A9;});
({union Cyc_CfFlowInfo_FlowInfo _tmp6AA=Cyc_CfFlowInfo_ReachableFL(_tmpEF,_tmpEE);inflow=_tmp6AA;});
Cyc_NewControlFlow_update_tryflow(env,inflow);
# 720
goto _LL3;}}else{_LL8: _LL9:
# 723
({void*_tmpEB=0U;({struct _dyneither_ptr _tmp6AB=({const char*_tmpEC="noconsume() parameters must be l-values";_tag_dyneither(_tmpEC,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6AB,_tag_dyneither(_tmpEB,sizeof(void*),0U));});});
goto _LL3;}}else{_LL6: _LL7:
# 721
 goto _LL3;}_LL3:;}
# 726
return inflow;}
# 731
static struct _tuple16 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 738
union Cyc_CfFlowInfo_FlowInfo _tmpF2=outflow;struct Cyc_Dict_Dict _tmp100;struct Cyc_List_List*_tmpFF;if((_tmpF2.BottomFL).tag == 1){_LL1: _LL2:
# 740
 return({struct _tuple16 _tmpF3;({union Cyc_CfFlowInfo_FlowInfo _tmp6AC=Cyc_CfFlowInfo_BottomFL();_tmpF3.f1=_tmp6AC;});_tmpF3.f2=rval;_tmpF3;});}else{_LL3: _tmp100=((_tmpF2.ReachableFL).val).f1;_tmpFF=((_tmpF2.ReachableFL).val).f2;_LL4: {
# 742
union Cyc_CfFlowInfo_AbsLVal _tmpF4=lval;struct Cyc_CfFlowInfo_Place*_tmpFE;if((_tmpF4.PlaceL).tag == 1){_LL6: _tmpFE=(_tmpF4.PlaceL).val;_LL7: {
# 750
struct Cyc_Dict_Dict _tmpF5=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp100,_tmpFE,rval);
({struct Cyc_List_List*_tmp6AD=Cyc_Relations_reln_assign_exp(Cyc_Core_heap_region,_tmpFF,lexp,rexp);_tmpFF=_tmp6AD;});
({union Cyc_CfFlowInfo_FlowInfo _tmp6AE=Cyc_CfFlowInfo_ReachableFL(_tmpF5,_tmpFF);outflow=_tmp6AE;});
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(lexp->topt))){
# 756
struct _tuple15 _tmpF6=Cyc_CfFlowInfo_unname_rval(Cyc_CfFlowInfo_lookup_place(_tmp100,_tmpFE));struct _tuple15 _tmpF7=_tmpF6;void*_tmpFB;_LLB: _tmpFB=_tmpF7.f1;_LLC:;{
void*_tmpF8=_tmpFB;switch(*((int*)_tmpF8)){case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpF8)->f1 == Cyc_CfFlowInfo_NoneIL){_LLE: _LLF:
 goto _LL11;}else{goto _LL14;}case 7U: _LL10: _LL11:
 goto _LL13;case 0U: _LL12: _LL13:
 goto _LLD;default: _LL14: _LL15:
# 762
({void*_tmpF9=0U;({unsigned int _tmp6B0=lexp->loc;struct _dyneither_ptr _tmp6AF=({const char*_tmpFA="assignment may overwrite alias-free pointer(s)";_tag_dyneither(_tmpFA,sizeof(char),47U);});Cyc_Warn_warn(_tmp6B0,_tmp6AF,_tag_dyneither(_tmpF9,sizeof(void*),0U));});});
goto _LLD;}_LLD:;};}
# 767
Cyc_NewControlFlow_update_tryflow(env,outflow);
return({struct _tuple16 _tmpFC;_tmpFC.f1=outflow;_tmpFC.f2=rval;_tmpFC;});}}else{_LL8: _LL9:
# 773
 return({struct _tuple16 _tmpFD;({union Cyc_CfFlowInfo_FlowInfo _tmp6B1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval);_tmpFD.f1=_tmp6B1;});_tmpFD.f2=rval;_tmpFD;});}_LL5:;}}_LL0:;}
# 780
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 786
union Cyc_CfFlowInfo_FlowInfo _tmp101=f;struct Cyc_Dict_Dict _tmp107;struct Cyc_List_List*_tmp106;if((_tmp101.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmp107=((_tmp101.ReachableFL).val).f1;_tmp106=((_tmp101.ReachableFL).val).f2;_LL4:
# 791
({struct Cyc_Dict_Dict _tmp6B8=({struct Cyc_CfFlowInfo_FlowEnv*_tmp6B7=fenv;unsigned int _tmp6B6=loc;struct Cyc_Dict_Dict _tmp6B5=_tmp107;struct Cyc_CfFlowInfo_Place*_tmp6B4=({struct Cyc_CfFlowInfo_Place*_tmp102=_cycalloc(sizeof(*_tmp102));({void*_tmp6B3=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp103=_cycalloc(sizeof(*_tmp103));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp6B2=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp104;_tmp104.tag=0U;_tmp104.f1=vd;_tmp104;});_tmp103[0]=_tmp6B2;});_tmp103;});_tmp102->root=_tmp6B3;});_tmp102->path=0;_tmp102;});Cyc_CfFlowInfo_assign_place(_tmp6B7,_tmp6B6,_tmp6B5,_tmp6B4,rval);});_tmp107=_tmp6B8;});
# 794
({struct Cyc_List_List*_tmp6B9=Cyc_Relations_reln_assign_var(Cyc_Core_heap_region,_tmp106,vd,rexp);_tmp106=_tmp6B9;});{
union Cyc_CfFlowInfo_FlowInfo _tmp105=Cyc_CfFlowInfo_ReachableFL(_tmp107,_tmp106);
Cyc_NewControlFlow_update_tryflow(env,_tmp105);
# 799
return _tmp105;};}_LL0:;}struct _tuple24{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 803
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned int pat_loc,unsigned int exp_loc){
# 810
if(vds == 0)return inflow;{
# 813
struct Cyc_List_List*_tmp108=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(vds)).f1);
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple24*)x->hd)).f1 == 0)({struct Cyc_List_List*_tmp6BA=({struct Cyc_List_List*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple24*)x->hd)).f2);_tmp109->tl=es;_tmp109;});es=_tmp6BA;});}}
# 819
({union Cyc_CfFlowInfo_FlowInfo _tmp6BB=Cyc_NewControlFlow_add_vars(fenv,inflow,_tmp108,fenv->unknown_all,pat_loc,name_locs);inflow=_tmp6BB;});
# 821
({union Cyc_CfFlowInfo_FlowInfo _tmp6BC=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);inflow=_tmp6BC;});
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 825
struct _tuple16 _tmp10A=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple16 _tmp10B=_tmp10A;union Cyc_CfFlowInfo_FlowInfo _tmp10D;void*_tmp10C;_LL1: _tmp10D=_tmp10B.f1;_tmp10C=_tmp10B.f2;_LL2:;
({union Cyc_CfFlowInfo_FlowInfo _tmp6BD=Cyc_NewControlFlow_use_Rval(env,exp_loc,_tmp10D,_tmp10C);inflow=_tmp6BD;});}}{
# 833
struct Cyc_List_List*_tmp10E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);
for(0;_tmp10E != 0;_tmp10E=_tmp10E->tl){
struct _tuple24*_tmp10F=(struct _tuple24*)_tmp10E->hd;struct _tuple24*_tmp110=_tmp10F;struct Cyc_Absyn_Vardecl**_tmp12B;struct Cyc_Absyn_Exp*_tmp12A;_LL4: _tmp12B=_tmp110->f1;_tmp12A=_tmp110->f2;_LL5:;
if(_tmp12B != 0  && _tmp12A != 0){
if(_tmp12A->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp113;_tmp113.tag=0U;({struct _dyneither_ptr _tmp6BE=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp12A));_tmp113.f1=_tmp6BE;});({void*_tmp111[1U]={& _tmp113};({struct _dyneither_ptr _tmp6BF=({const char*_tmp112="oops! pattern init expr %s has no type!\n";_tag_dyneither(_tmp112,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6BF,_tag_dyneither(_tmp111,sizeof(void*),1U));});});});{
# 847
struct Cyc_List_List l=({struct Cyc_List_List _tmp129;_tmp129.hd=_tmp12A;_tmp129.tl=0;_tmp129;});
union Cyc_CfFlowInfo_FlowInfo _tmp114=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp115=Cyc_NewControlFlow_anal_Lexp(env,_tmp114,0,0,_tmp12A);struct _tuple18 _tmp116=_tmp115;union Cyc_CfFlowInfo_AbsLVal _tmp128;_LL7: _tmp128=_tmp116.f2;_LL8:;{
struct _tuple16 _tmp117=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp114,_tmp12A);struct _tuple16 _tmp118=_tmp117;union Cyc_CfFlowInfo_FlowInfo _tmp127;void*_tmp126;_LLA: _tmp127=_tmp118.f1;_tmp126=_tmp118.f2;_LLB:;{
union Cyc_CfFlowInfo_FlowInfo _tmp119=_tmp127;struct Cyc_Dict_Dict _tmp125;struct Cyc_List_List*_tmp124;if((_tmp119.ReachableFL).tag == 2){_LLD: _tmp125=((_tmp119.ReachableFL).val).f1;_tmp124=((_tmp119.ReachableFL).val).f2;_LLE:
# 853
 if(name_locs){
union Cyc_CfFlowInfo_AbsLVal _tmp11A=_tmp128;struct Cyc_CfFlowInfo_Place*_tmp123;if((_tmp11A.PlaceL).tag == 1){_LL12: _tmp123=(_tmp11A.PlaceL).val;_LL13:
# 856
({void*_tmp6C1=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp11B=_cycalloc(sizeof(*_tmp11B));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6C0=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp11C;_tmp11C.tag=8U;_tmp11C.f1=*_tmp12B;_tmp11C.f2=_tmp126;_tmp11C;});_tmp11B[0]=_tmp6C0;});_tmp11B;});_tmp126=_tmp6C1;});{
# 859
void*_tmp11D=Cyc_CfFlowInfo_lookup_place(_tmp125,_tmp123);
({void*_tmp6C3=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp11E=_cycalloc(sizeof(*_tmp11E));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6C2=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp11F;_tmp11F.tag=8U;_tmp11F.f1=*_tmp12B;_tmp11F.f2=_tmp11D;_tmp11F;});_tmp11E[0]=_tmp6C2;});_tmp11E;});_tmp11D=_tmp6C3;});
({struct Cyc_Dict_Dict _tmp6C4=Cyc_CfFlowInfo_assign_place(fenv,exp_loc,_tmp125,_tmp123,_tmp11D);_tmp125=_tmp6C4;});
({union Cyc_CfFlowInfo_FlowInfo _tmp6C5=Cyc_CfFlowInfo_ReachableFL(_tmp125,_tmp124);_tmp127=_tmp6C5;});
goto _LL11;};}else{_LL14: _LL15:
# 866
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(_tmp12A->topt)) && !
Cyc_Tcutil_is_noalias_pointer_or_aggr((*_tmp12B)->type))
({struct Cyc_String_pa_PrintArg_struct _tmp122;_tmp122.tag=0U;({struct _dyneither_ptr _tmp6C6=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp12A));_tmp122.f1=_tmp6C6;});({void*_tmp120[1U]={& _tmp122};({unsigned int _tmp6C8=exp_loc;struct _dyneither_ptr _tmp6C7=({const char*_tmp121="aliased pattern expression not an l-value: %s";_tag_dyneither(_tmp121,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp6C8,_tmp6C7,_tag_dyneither(_tmp120,sizeof(void*),1U));});});});}_LL11:;}
# 876
({union Cyc_CfFlowInfo_FlowInfo _tmp6C9=Cyc_NewControlFlow_do_initialize_var(fenv,env,_tmp127,*_tmp12B,_tmp12A,_tmp126,pat_loc);inflow=_tmp6C9;});
goto _LLC;}else{_LLF: _LL10:
# 879
 goto _LLC;}_LLC:;};};};}}
# 884
return inflow;};};}
# 887
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e,int cast_ok){
# 889
void*_tmp12C=e->r;void*_tmp12D=_tmp12C;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp130;switch(*((int*)_tmp12D)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp12D)->f1)){case 4U: _LL1: _LL2:
 goto _LL4;case 3U: _LL3: _LL4:
 goto _LL6;case 5U: _LL5: _LL6:
 return 1;default: goto _LL11;}case 20U: _LL7: _tmp130=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp12D)->f1;_LL8:
 _tmp131=_tmp130;goto _LLA;case 21U: _LL9: _tmp131=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp12D)->f1;_LLA:
 _tmp132=_tmp131;goto _LLC;case 22U: _LLB: _tmp132=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp12D)->f1;_LLC:
# 896
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp132,cast_ok);case 23U: _LLD: _tmp133=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp12D)->f1;_LLE: {
# 898
void*_tmp12E=Cyc_Tcutil_compress((void*)_check_null(_tmp133->topt));void*_tmp12F=_tmp12E;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp12F)->tag == 10U){_LL14: _LL15:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp133,cast_ok);}else{_LL16: _LL17:
 return 0;}_LL13:;}case 14U: _LLF: _tmp134=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp12D)->f2;_LL10:
# 903
 if(cast_ok)return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp134,cast_ok);else{
return 0;}default: _LL11: _LL12:
 return 0;}_LL0:;}
# 909
static int Cyc_NewControlFlow_subst_param(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp*rop){
union Cyc_Relations_RelnOp _tmp135=*rop;union Cyc_Relations_RelnOp _tmp136=_tmp135;unsigned int _tmp13A;unsigned int _tmp139;switch((_tmp136.RParamNumelts).tag){case 5U: _LL1: _tmp139=(_tmp136.RParam).val;_LL2: {
# 912
struct Cyc_Absyn_Exp*_tmp137=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp139);
return Cyc_Relations_exp2relnop(_tmp137,rop);}case 6U: _LL3: _tmp13A=(_tmp136.RParamNumelts).val;_LL4: {
# 915
struct Cyc_Absyn_Exp*_tmp138=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp13A);
return({struct Cyc_Absyn_Exp*_tmp6CA=Cyc_Absyn_prim1_exp(Cyc_Absyn_Numelts,_tmp138,0U);Cyc_Relations_exp2relnop(_tmp6CA,rop);});}default: _LL5: _LL6:
 return 1;}_LL0:;}
# 921
static struct _dyneither_ptr Cyc_NewControlFlow_subst_param_string(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp rop){
union Cyc_Relations_RelnOp _tmp13B=rop;unsigned int _tmp13D;unsigned int _tmp13C;switch((_tmp13B.RParamNumelts).tag){case 5U: _LL1: _tmp13C=(_tmp13B.RParam).val;_LL2:
# 924
 return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp13C));case 6U: _LL3: _tmp13D=(_tmp13B.RParamNumelts).val;_LL4:
# 926
 return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp13D));default: _LL5: _LL6:
 return Cyc_Relations_rop2string(rop);}_LL0:;}
# 931
static void Cyc_NewControlFlow_check_fn_requires(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*exps,struct Cyc_List_List*req,unsigned int loc){
# 934
union Cyc_CfFlowInfo_FlowInfo _tmp13E=inflow;struct Cyc_Dict_Dict _tmp148;struct Cyc_List_List*_tmp147;if((_tmp13E.BottomFL).tag == 1){_LL1: _LL2:
 return;}else{_LL3: _tmp148=((_tmp13E.ReachableFL).val).f1;_tmp147=((_tmp13E.ReachableFL).val).f2;_LL4:
# 937
 for(0;req != 0;req=req->tl){
struct Cyc_Relations_Reln*_tmp13F=(struct Cyc_Relations_Reln*)req->hd;
union Cyc_Relations_RelnOp rop1=_tmp13F->rop1;
union Cyc_Relations_RelnOp rop2=_tmp13F->rop2;
enum Cyc_Relations_Relation _tmp140=Cyc_Relations_flip_relation(_tmp13F->relation);
if((!Cyc_NewControlFlow_subst_param(exps,& rop1) || !Cyc_NewControlFlow_subst_param(exps,& rop2)) || 
Cyc_Relations_consistent_relations(Cyc_Relations_add_relation(Cyc_Core_heap_region,rop2,_tmp140,rop1,_tmp147))){
struct _dyneither_ptr s1=Cyc_NewControlFlow_subst_param_string(exps,rop1);
struct _dyneither_ptr s2=Cyc_Relations_relation2string(_tmp13F->relation);
struct _dyneither_ptr s3=Cyc_NewControlFlow_subst_param_string(exps,rop2);
({struct Cyc_String_pa_PrintArg_struct _tmp146;_tmp146.tag=0U;({struct _dyneither_ptr _tmp6CB=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Relations_relns2string(_tmp147));_tmp146.f1=_tmp6CB;});({struct Cyc_String_pa_PrintArg_struct _tmp145;_tmp145.tag=0U;_tmp145.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);({struct Cyc_String_pa_PrintArg_struct _tmp144;_tmp144.tag=0U;_tmp144.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({struct Cyc_String_pa_PrintArg_struct _tmp143;_tmp143.tag=0U;_tmp143.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({void*_tmp141[4U]={& _tmp143,& _tmp144,& _tmp145,& _tmp146};({unsigned int _tmp6CD=loc;struct _dyneither_ptr _tmp6CC=({const char*_tmp142="cannot prove that @requires clause %s %s %s is satisfied\n (all I know is %s)";_tag_dyneither(_tmp142,sizeof(char),77U);});Cyc_CfFlowInfo_aerr(_tmp6CD,_tmp6CC,_tag_dyneither(_tmp141,sizeof(void*),4U));});});});});});});
break;}}
# 952
goto _LL0;}_LL0:;}struct _tuple25{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple26{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 957
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 961
struct Cyc_CfFlowInfo_FlowEnv*_tmp149=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 974
{union Cyc_CfFlowInfo_FlowInfo _tmp14A=inflow;struct Cyc_Dict_Dict _tmp14D;struct Cyc_List_List*_tmp14C;if((_tmp14A.BottomFL).tag == 1){_LL1: _LL2:
 return({struct _tuple16 _tmp14B;({union Cyc_CfFlowInfo_FlowInfo _tmp6CE=Cyc_CfFlowInfo_BottomFL();_tmp14B.f1=_tmp6CE;});_tmp14B.f2=_tmp149->unknown_all;_tmp14B;});}else{_LL3: _tmp14D=((_tmp14A.ReachableFL).val).f1;_tmp14C=((_tmp14A.ReachableFL).val).f2;_LL4:
 d=_tmp14D;relns=_tmp14C;}_LL0:;}
# 989 "new_control_flow.cyc"
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e,0)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
# 1010 "new_control_flow.cyc"
struct _tuple18 _tmp14E=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);struct _tuple18 _tmp14F=_tmp14E;union Cyc_CfFlowInfo_FlowInfo _tmp15F;union Cyc_CfFlowInfo_AbsLVal _tmp15E;_LL6: _tmp15F=_tmp14F.f1;_tmp15E=_tmp14F.f2;_LL7:;{
struct _tuple18 _tmp150=({struct _tuple18 _tmp15D;_tmp15D.f1=_tmp15F;_tmp15D.f2=_tmp15E;_tmp15D;});struct _tuple18 _tmp151=_tmp150;struct Cyc_Dict_Dict _tmp15C;struct Cyc_List_List*_tmp15B;struct Cyc_CfFlowInfo_Place*_tmp15A;if(((_tmp151.f1).ReachableFL).tag == 2){if(((_tmp151.f2).PlaceL).tag == 1){_LL9: _tmp15C=(((_tmp151.f1).ReachableFL).val).f1;_tmp15B=(((_tmp151.f1).ReachableFL).val).f2;_tmp15A=((_tmp151.f2).PlaceL).val;_LLA: {
# 1013
void*_tmp152=Cyc_CfFlowInfo_lookup_place(_tmp15C,_tmp15A);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp152,& needs_unconsume)){
({void*_tmp153=0U;({unsigned int _tmp6D0=e->loc;struct _dyneither_ptr _tmp6CF=({const char*_tmp154="expression attempts to copy a consumed alias-free value";_tag_dyneither(_tmp154,sizeof(char),56U);});Cyc_CfFlowInfo_aerr(_tmp6D0,_tmp6CF,_tag_dyneither(_tmp153,sizeof(void*),0U));});});
return({struct _tuple16 _tmp155;({union Cyc_CfFlowInfo_FlowInfo _tmp6D1=Cyc_CfFlowInfo_BottomFL();_tmp155.f1=_tmp6D1;});_tmp155.f2=_tmp149->unknown_all;_tmp155;});}else{
# 1019
if(needs_unconsume)
# 1021
return({struct _tuple16 _tmp156;_tmp156.f1=_tmp15F;({void*_tmp6D2=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp149,_tmp152);_tmp156.f2=_tmp6D2;});_tmp156;});else{
# 1024
void*_tmp157=Cyc_CfFlowInfo_make_unique_consumed(_tmp149,(void*)_check_null(e->topt),e,env->iteration_num,_tmp152);
struct Cyc_Dict_Dict _tmp158=Cyc_CfFlowInfo_assign_place(_tmp149,e->loc,_tmp15C,_tmp15A,_tmp157);
# 1036
return({struct _tuple16 _tmp159;({union Cyc_CfFlowInfo_FlowInfo _tmp6D3=Cyc_CfFlowInfo_ReachableFL(_tmp158,_tmp15B);_tmp159.f1=_tmp6D3;});_tmp159.f2=_tmp152;_tmp159;});}}}}else{goto _LLB;}}else{_LLB: _LLC:
# 1040
 goto _LL8;}_LL8:;};}{
# 1043
void*_tmp160=e->r;void*_tmp161=_tmp160;struct Cyc_Absyn_Stmt*_tmp364;struct Cyc_Absyn_Exp*_tmp363;void*_tmp362;int _tmp361;struct Cyc_Absyn_Vardecl*_tmp360;struct Cyc_Absyn_Exp*_tmp35F;struct Cyc_Absyn_Exp*_tmp35E;int _tmp35D;struct Cyc_List_List*_tmp35C;struct Cyc_List_List*_tmp35B;enum Cyc_Absyn_AggrKind _tmp35A;struct Cyc_List_List*_tmp359;struct Cyc_List_List*_tmp358;struct Cyc_List_List*_tmp357;struct Cyc_Absyn_Exp*_tmp356;struct Cyc_Absyn_Exp*_tmp355;struct Cyc_Absyn_Exp*_tmp354;struct Cyc_Absyn_Exp*_tmp353;struct Cyc_Absyn_Exp*_tmp352;struct Cyc_Absyn_Exp*_tmp351;struct Cyc_Absyn_Exp*_tmp350;struct Cyc_Absyn_Exp*_tmp34F;struct Cyc_Absyn_Exp*_tmp34E;struct Cyc_Absyn_Exp*_tmp34D;struct _dyneither_ptr*_tmp34C;struct Cyc_Absyn_Exp*_tmp34B;struct _dyneither_ptr*_tmp34A;struct Cyc_Absyn_Exp*_tmp349;struct _dyneither_ptr*_tmp348;struct Cyc_Absyn_Exp*_tmp347;struct Cyc_Absyn_Exp*_tmp346;struct Cyc_Absyn_Exp*_tmp345;struct Cyc_Absyn_Exp*_tmp344;struct Cyc_Absyn_Exp*_tmp343;struct Cyc_Absyn_Exp*_tmp342;int _tmp341;struct Cyc_Absyn_Exp*_tmp340;void**_tmp33F;struct Cyc_Absyn_Exp*_tmp33E;int _tmp33D;struct Cyc_Absyn_Exp*_tmp33C;struct Cyc_List_List*_tmp33B;struct Cyc_Absyn_Exp*_tmp33A;struct Cyc_Absyn_Exp*_tmp339;struct Cyc_Absyn_Exp*_tmp338;struct Cyc_Absyn_Exp*_tmp337;struct Cyc_Absyn_Exp*_tmp336;struct Cyc_Absyn_Exp*_tmp335;struct Cyc_Absyn_Exp*_tmp334;struct Cyc_Absyn_Exp*_tmp333;enum Cyc_Absyn_Primop _tmp332;struct Cyc_List_List*_tmp331;struct Cyc_List_List*_tmp330;struct Cyc_Absyn_Datatypedecl*_tmp32F;struct Cyc_Absyn_Vardecl*_tmp32E;struct Cyc_Absyn_Vardecl*_tmp32D;struct Cyc_Absyn_Vardecl*_tmp32C;struct _dyneither_ptr _tmp32B;struct Cyc_Absyn_Exp*_tmp32A;struct Cyc_Absyn_Exp*_tmp329;struct Cyc_Absyn_Exp*_tmp328;struct Cyc_Absyn_Exp*_tmp327;switch(*((int*)_tmp161)){case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp161)->f4 == Cyc_Absyn_Null_to_NonNull){_LLE: _tmp327=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp161)->f2;_LLF: {
# 1047
struct _tuple16 _tmp162=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp327);struct _tuple16 _tmp163=_tmp162;union Cyc_CfFlowInfo_FlowInfo _tmp16A;void*_tmp169;_LL7D: _tmp16A=_tmp163.f1;_tmp169=_tmp163.f2;_LL7E:;{
struct _tuple16 _tmp164=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp16A,_tmp327,_tmp169);struct _tuple16 _tmp165=_tmp164;union Cyc_CfFlowInfo_FlowInfo _tmp168;void*_tmp167;_LL80: _tmp168=_tmp165.f1;_tmp167=_tmp165.f2;_LL81:;
return({struct _tuple16 _tmp166;_tmp166.f1=_tmp168;_tmp166.f2=_tmp169;_tmp166;});};}}else{_LL10: _tmp328=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp161)->f2;_LL11:
# 1053
 _tmp329=_tmp328;goto _LL13;}case 12U: _LL12: _tmp329=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_LL13:
 _tmp32A=_tmp329;goto _LL15;case 13U: _LL14: _tmp32A=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_LL15:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp32A);case 2U: _LL16: _tmp32B=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_LL17:
# 1058
 if(!({struct _dyneither_ptr _tmp6D4=(struct _dyneither_ptr)_tmp32B;Cyc_NewControlFlow_strcmp(_tmp6D4,({const char*_tmp16B="print_flow";_tag_dyneither(_tmp16B,sizeof(char),11U);}));})){
struct _dyneither_ptr _tmp16C=Cyc_Position_string_of_segment(e->loc);
({struct Cyc_String_pa_PrintArg_struct _tmp16F;_tmp16F.tag=0U;_tmp16F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp16C);({void*_tmp16D[1U]={& _tmp16F};({struct Cyc___cycFILE*_tmp6D6=Cyc_stderr;struct _dyneither_ptr _tmp6D5=({const char*_tmp16E="%s: current flow is\n";_tag_dyneither(_tmp16E,sizeof(char),21U);});Cyc_fprintf(_tmp6D6,_tmp6D5,_tag_dyneither(_tmp16D,sizeof(void*),1U));});});});
Cyc_CfFlowInfo_print_flow(inflow);
({void*_tmp170=0U;({struct Cyc___cycFILE*_tmp6D8=Cyc_stderr;struct _dyneither_ptr _tmp6D7=({const char*_tmp171="\n";_tag_dyneither(_tmp171,sizeof(char),2U);});Cyc_fprintf(_tmp6D8,_tmp6D7,_tag_dyneither(_tmp170,sizeof(void*),0U));});});}else{
# 1064
if(!({struct _dyneither_ptr _tmp6D9=(struct _dyneither_ptr)_tmp32B;Cyc_NewControlFlow_strcmp(_tmp6D9,({const char*_tmp172="debug_on";_tag_dyneither(_tmp172,sizeof(char),9U);}));}));else{
# 1069
if(!({struct _dyneither_ptr _tmp6DA=(struct _dyneither_ptr)_tmp32B;Cyc_NewControlFlow_strcmp(_tmp6DA,({const char*_tmp173="debug_off";_tag_dyneither(_tmp173,sizeof(char),10U);}));}));else{
# 1075
({struct Cyc_String_pa_PrintArg_struct _tmp176;_tmp176.tag=0U;_tmp176.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32B);({void*_tmp174[1U]={& _tmp176};({unsigned int _tmp6DC=e->loc;struct _dyneither_ptr _tmp6DB=({const char*_tmp175="unknown pragma %s";_tag_dyneither(_tmp175,sizeof(char),18U);});Cyc_Warn_warn(_tmp6DC,_tmp6DB,_tag_dyneither(_tmp174,sizeof(void*),1U));});});});}}}
return({struct _tuple16 _tmp177;_tmp177.f1=inflow;_tmp177.f2=_tmp149->zero;_tmp177;});case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp161)->f1).Wstring_c).tag){case 1U: _LL18: _LL19:
# 1078
 goto _LL1B;case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp161)->f1).Int_c).val).f2 == 0){_LL1A: _LL1B:
 return({struct _tuple16 _tmp178;_tmp178.f1=inflow;_tmp178.f2=_tmp149->zero;_tmp178;});}else{_LL1C: _LL1D:
 goto _LL1F;}case 8U: _LL1E: _LL1F:
 goto _LL21;case 9U: _LL20: _LL21:
 goto _LL23;default: _LL26: _LL27:
# 1086
 goto _LL29;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp161)->f1)){case 2U: _LL22: _LL23:
# 1083
 return({struct _tuple16 _tmp179;_tmp179.f1=inflow;_tmp179.f2=_tmp149->notzeroall;_tmp179;});case 1U: _LL32: _LL33:
# 1094
 return({struct _tuple16 _tmp17B;_tmp17B.f1=inflow;({void*_tmp6DD=Cyc_CfFlowInfo_typ_to_absrval(_tmp149,(void*)_check_null(e->topt),0,_tmp149->unknown_all);_tmp17B.f2=_tmp6DD;});_tmp17B;});case 3U: _LL34: _tmp32C=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp161)->f1)->f1;_LL35:
# 1097
 _tmp32D=_tmp32C;goto _LL37;case 4U: _LL36: _tmp32D=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp161)->f1)->f1;_LL37:
 _tmp32E=_tmp32D;goto _LL39;case 5U: _LL38: _tmp32E=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp161)->f1)->f1;_LL39: {
# 1101
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp17E;_tmp17E.tag=0U;_tmp17E.f1=_tmp32E;_tmp17E;});
return({struct _tuple16 _tmp17C;_tmp17C.f1=inflow;({void*_tmp6DF=({struct Cyc_Dict_Dict _tmp6DE=d;((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp6DE,Cyc_CfFlowInfo_root_cmp,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D[0]=vdroot;_tmp17D;}));});_tmp17C.f2=_tmp6DF;});_tmp17C;});}default: _LL72: _LL73:
# 1681
 goto _LL75;}case 31U: if(((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp161)->f1 == 0){_LL24: _LL25:
# 1085
 goto _LL27;}else{_LL60: _tmp330=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_tmp32F=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp161)->f2;_LL61:
# 1544
 _tmp357=_tmp330;goto _LL63;}case 18U: _LL28: _LL29:
# 1087
 goto _LL2B;case 17U: _LL2A: _LL2B:
 goto _LL2D;case 19U: _LL2C: _LL2D:
 goto _LL2F;case 33U: _LL2E: _LL2F:
 goto _LL31;case 32U: _LL30: _LL31:
 return({struct _tuple16 _tmp17A;_tmp17A.f1=inflow;_tmp17A.f2=_tmp149->unknown_all;_tmp17A;});case 3U: _LL3A: _tmp332=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_tmp331=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp161)->f2;_LL3B: {
# 1108
struct _tuple16 _tmp17F=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp331);struct _tuple16 _tmp180=_tmp17F;union Cyc_CfFlowInfo_FlowInfo _tmp183;void*_tmp182;_LL83: _tmp183=_tmp180.f1;_tmp182=_tmp180.f2;_LL84:;
return({struct _tuple16 _tmp181;_tmp181.f1=_tmp183;_tmp181.f2=_tmp182;_tmp181;});}case 5U: _LL3C: _tmp333=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_LL3D: {
# 1112
struct Cyc_List_List _tmp184=({struct Cyc_List_List _tmp197;_tmp197.hd=_tmp333;_tmp197.tl=0;_tmp197;});
struct _tuple16 _tmp185=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp184);struct _tuple16 _tmp186=_tmp185;union Cyc_CfFlowInfo_FlowInfo _tmp196;_LL86: _tmp196=_tmp186.f1;_LL87:;{
struct _tuple18 _tmp187=Cyc_NewControlFlow_anal_Lexp(env,_tmp196,0,0,_tmp333);struct _tuple18 _tmp188=_tmp187;union Cyc_CfFlowInfo_AbsLVal _tmp195;_LL89: _tmp195=_tmp188.f2;_LL8A:;{
int iszt=Cyc_Tcutil_is_zeroterm_pointer_type((void*)_check_null(_tmp333->topt));
if(iszt){
# 1119
struct _tuple16 _tmp189=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp333);struct _tuple16 _tmp18A=_tmp189;union Cyc_CfFlowInfo_FlowInfo _tmp18C;void*_tmp18B;_LL8C: _tmp18C=_tmp18A.f1;_tmp18B=_tmp18A.f2;_LL8D:;
Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp18C,_tmp333,_tmp18B);}
# 1122
{struct _tuple25 _tmp18D=({struct _tuple25 _tmp192;_tmp192.f1=_tmp195;_tmp192.f2=_tmp196;_tmp192;});struct _tuple25 _tmp18E=_tmp18D;struct Cyc_CfFlowInfo_Place*_tmp191;struct Cyc_Dict_Dict _tmp190;struct Cyc_List_List*_tmp18F;if(((_tmp18E.f1).PlaceL).tag == 1){if(((_tmp18E.f2).ReachableFL).tag == 2){_LL8F: _tmp191=((_tmp18E.f1).PlaceL).val;_tmp190=(((_tmp18E.f2).ReachableFL).val).f1;_tmp18F=(((_tmp18E.f2).ReachableFL).val).f2;_LL90:
# 1124
({struct Cyc_List_List*_tmp6E0=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp18F,_tmp333);_tmp18F=_tmp6E0;});
({union Cyc_CfFlowInfo_FlowInfo _tmp6E2=({struct Cyc_Dict_Dict _tmp6E1=Cyc_CfFlowInfo_assign_place(_tmp149,_tmp333->loc,_tmp190,_tmp191,_tmp149->unknown_all);Cyc_CfFlowInfo_ReachableFL(_tmp6E1,_tmp18F);});_tmp196=_tmp6E2;});
# 1129
goto _LL8E;}else{goto _LL91;}}else{_LL91: _LL92:
 goto _LL8E;}_LL8E:;}
# 1133
if(iszt)
return({struct _tuple16 _tmp193;_tmp193.f1=_tmp196;_tmp193.f2=_tmp149->notzeroall;_tmp193;});else{
return({struct _tuple16 _tmp194;_tmp194.f1=_tmp196;_tmp194.f2=_tmp149->unknown_all;_tmp194;});}};};}case 4U: if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp161)->f2 != 0){_LL3E: _tmp335=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_tmp334=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp161)->f3;_LL3F:
# 1138
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp198=0U;({unsigned int _tmp6E4=e->loc;struct _dyneither_ptr _tmp6E3=({const char*_tmp199="cannot track alias-free pointers through multiple assignments";_tag_dyneither(_tmp199,sizeof(char),62U);});Cyc_CfFlowInfo_aerr(_tmp6E4,_tmp6E3,_tag_dyneither(_tmp198,sizeof(void*),0U));});});
return({struct _tuple16 _tmp19A;({union Cyc_CfFlowInfo_FlowInfo _tmp6E5=Cyc_CfFlowInfo_BottomFL();_tmp19A.f1=_tmp6E5;});_tmp19A.f2=_tmp149->unknown_all;_tmp19A;});}{
# 1142
struct _tuple16 _tmp19B=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp334);struct _tuple16 _tmp19C=_tmp19B;union Cyc_CfFlowInfo_FlowInfo _tmp1A8;void*_tmp1A7;_LL94: _tmp1A8=_tmp19C.f1;_tmp1A7=_tmp19C.f2;_LL95:;{
struct _tuple18 _tmp19D=Cyc_NewControlFlow_anal_Lexp(env,_tmp1A8,0,0,_tmp335);struct _tuple18 _tmp19E=_tmp19D;union Cyc_CfFlowInfo_FlowInfo _tmp1A6;union Cyc_CfFlowInfo_AbsLVal _tmp1A5;_LL97: _tmp1A6=_tmp19E.f1;_tmp1A5=_tmp19E.f2;_LL98:;
{union Cyc_CfFlowInfo_FlowInfo _tmp19F=_tmp1A6;struct Cyc_Dict_Dict _tmp1A3;struct Cyc_List_List*_tmp1A2;if((_tmp19F.ReachableFL).tag == 2){_LL9A: _tmp1A3=((_tmp19F.ReachableFL).val).f1;_tmp1A2=((_tmp19F.ReachableFL).val).f2;_LL9B:
# 1146
{union Cyc_CfFlowInfo_AbsLVal _tmp1A0=_tmp1A5;struct Cyc_CfFlowInfo_Place*_tmp1A1;if((_tmp1A0.PlaceL).tag == 1){_LL9F: _tmp1A1=(_tmp1A0.PlaceL).val;_LLA0:
# 1150
({struct Cyc_List_List*_tmp6E6=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp1A2,_tmp335);_tmp1A2=_tmp6E6;});
({struct Cyc_Dict_Dict _tmp6E7=Cyc_CfFlowInfo_assign_place(_tmp149,_tmp335->loc,_tmp1A3,_tmp1A1,_tmp149->unknown_all);_tmp1A3=_tmp6E7;});
# 1153
({union Cyc_CfFlowInfo_FlowInfo _tmp6E8=Cyc_CfFlowInfo_ReachableFL(_tmp1A3,_tmp1A2);_tmp1A6=_tmp6E8;});
# 1157
goto _LL9E;}else{_LLA1: _LLA2:
# 1160
 goto _LL9E;}_LL9E:;}
# 1162
goto _LL99;}else{_LL9C: _LL9D:
 goto _LL99;}_LL99:;}
# 1165
return({struct _tuple16 _tmp1A4;_tmp1A4.f1=_tmp1A6;_tmp1A4.f2=_tmp149->unknown_all;_tmp1A4;});};};}else{_LL40: _tmp337=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_tmp336=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp161)->f3;_LL41:
# 1168
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp1A9=0U;({unsigned int _tmp6EA=e->loc;struct _dyneither_ptr _tmp6E9=({const char*_tmp1AA="cannot track alias-free pointers through multiple assignments";_tag_dyneither(_tmp1AA,sizeof(char),62U);});Cyc_CfFlowInfo_aerr(_tmp6EA,_tmp6E9,_tag_dyneither(_tmp1A9,sizeof(void*),0U));});});
return({struct _tuple16 _tmp1AB;({union Cyc_CfFlowInfo_FlowInfo _tmp6EB=Cyc_CfFlowInfo_BottomFL();_tmp1AB.f1=_tmp6EB;});_tmp1AB.f2=_tmp149->unknown_all;_tmp1AB;});}
# 1174
({union Cyc_CfFlowInfo_FlowInfo _tmp6EE=({struct Cyc_NewControlFlow_AnalEnv*_tmp6ED=env;union Cyc_CfFlowInfo_FlowInfo _tmp6EC=inflow;Cyc_NewControlFlow_expand_unique_places(_tmp6ED,_tmp6EC,({struct Cyc_Absyn_Exp*_tmp1AC[2U];_tmp1AC[1U]=_tmp336;_tmp1AC[0U]=_tmp337;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1AC,sizeof(struct Cyc_Absyn_Exp*),2U));}));});inflow=_tmp6EE;});{
# 1176
struct _tuple16 _tmp1AD=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp336);struct _tuple16 _tmp1AE=_tmp1AD;union Cyc_CfFlowInfo_FlowInfo _tmp1B9;void*_tmp1B8;_LLA4: _tmp1B9=_tmp1AE.f1;_tmp1B8=_tmp1AE.f2;_LLA5:;{
struct _tuple18 _tmp1AF=Cyc_NewControlFlow_anal_Lexp(env,_tmp1B9,0,0,_tmp337);struct _tuple18 _tmp1B0=_tmp1AF;union Cyc_CfFlowInfo_FlowInfo _tmp1B7;union Cyc_CfFlowInfo_AbsLVal _tmp1B6;_LLA7: _tmp1B7=_tmp1B0.f1;_tmp1B6=_tmp1B0.f2;_LLA8:;{
struct _tuple16 _tmp1B1=Cyc_NewControlFlow_do_assign(_tmp149,env,_tmp1B7,_tmp337,_tmp1B6,_tmp336,_tmp1B8,e->loc);struct _tuple16 _tmp1B2=_tmp1B1;union Cyc_CfFlowInfo_FlowInfo _tmp1B5;void*_tmp1B4;_LLAA: _tmp1B5=_tmp1B2.f1;_tmp1B4=_tmp1B2.f2;_LLAB:;
# 1182
return({struct _tuple16 _tmp1B3;_tmp1B3.f1=_tmp1B5;_tmp1B3.f2=_tmp1B4;_tmp1B3;});};};};}case 9U: _LL42: _tmp339=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_tmp338=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp161)->f2;_LL43: {
# 1185
struct _tuple16 _tmp1BA=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp339);struct _tuple16 _tmp1BB=_tmp1BA;union Cyc_CfFlowInfo_FlowInfo _tmp1BD;void*_tmp1BC;_LLAD: _tmp1BD=_tmp1BB.f1;_tmp1BC=_tmp1BB.f2;_LLAE:;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp1BD,_tmp338);}case 11U: _LL44: _tmp33A=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_LL45: {
# 1189
struct _tuple16 _tmp1BE=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp33A);struct _tuple16 _tmp1BF=_tmp1BE;union Cyc_CfFlowInfo_FlowInfo _tmp1C2;void*_tmp1C1;_LLB0: _tmp1C2=_tmp1BF.f1;_tmp1C1=_tmp1BF.f2;_LLB1:;
Cyc_NewControlFlow_use_Rval(env,_tmp33A->loc,_tmp1C2,_tmp1C1);
return({struct _tuple16 _tmp1C0;({union Cyc_CfFlowInfo_FlowInfo _tmp6EF=Cyc_CfFlowInfo_BottomFL();_tmp1C0.f1=_tmp6EF;});({void*_tmp6F0=Cyc_CfFlowInfo_typ_to_absrval(_tmp149,(void*)_check_null(e->topt),0,_tmp149->unknown_all);_tmp1C0.f2=_tmp6F0;});_tmp1C0;});}case 10U: _LL46: _tmp33C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_tmp33B=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp161)->f2;_LL47: {
# 1194
struct Cyc_List_List*_tmp1C3=_tmp33B;
struct _tuple23 _tmp1C4=({
struct Cyc_NewControlFlow_AnalEnv*_tmp6F3=env;union Cyc_CfFlowInfo_FlowInfo _tmp6F2=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6F3,_tmp6F2,({struct Cyc_List_List*_tmp1F4=_cycalloc(sizeof(*_tmp1F4));_tmp1F4->hd=_tmp33C;({struct Cyc_List_List*_tmp6F1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp33B);_tmp1F4->tl=_tmp6F1;});_tmp1F4;}),0,1);});
# 1195
struct _tuple23 _tmp1C5=_tmp1C4;union Cyc_CfFlowInfo_FlowInfo _tmp1F3;struct Cyc_List_List*_tmp1F2;_LLB3: _tmp1F3=_tmp1C5.f1;_tmp1F2=_tmp1C5.f2;_LLB4:;{
# 1198
union Cyc_CfFlowInfo_FlowInfo _tmp1C6=Cyc_NewControlFlow_use_Rval(env,_tmp33C->loc,_tmp1F3,(void*)((struct Cyc_List_List*)_check_null(_tmp1F2))->hd);
_tmp1F2=_tmp1F2->tl;{
# 1201
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
struct Cyc_List_List*requires;
struct Cyc_List_List*ensures;
{void*_tmp1C7=Cyc_Tcutil_compress((void*)_check_null(_tmp33C->topt));void*_tmp1C8=_tmp1C7;void*_tmp1DA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C8)->tag == 5U){_LLB6: _tmp1DA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C8)->f1).elt_typ;_LLB7:
# 1208
{void*_tmp1C9=Cyc_Tcutil_compress(_tmp1DA);void*_tmp1CA=_tmp1C9;struct Cyc_List_List*_tmp1D7;struct Cyc_List_List*_tmp1D6;struct Cyc_List_List*_tmp1D5;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CA)->tag == 9U){_LLBB: _tmp1D7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CA)->f1).attributes;_tmp1D6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CA)->f1).requires_relns;_tmp1D5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CA)->f1).ensures_relns;_LLBC:
# 1210
 requires=_tmp1D6;
ensures=_tmp1D5;
for(0;_tmp1D7 != 0;_tmp1D7=_tmp1D7->tl){
# 1214
void*_tmp1CB=(void*)_tmp1D7->hd;void*_tmp1CC=_tmp1CB;int _tmp1D2;int _tmp1D1;int _tmp1D0;switch(*((int*)_tmp1CC)){case 20U: _LLC0: _tmp1D0=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1CC)->f1;_LLC1:
# 1216
({struct Cyc_List_List*_tmp6F4=({struct Cyc_List_List*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD->hd=(void*)_tmp1D0;_tmp1CD->tl=init_params;_tmp1CD;});init_params=_tmp6F4;});goto _LLBF;case 21U: _LLC2: _tmp1D1=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp1CC)->f1;_LLC3:
# 1218
({struct Cyc_List_List*_tmp6F5=({struct Cyc_List_List*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE->hd=(void*)_tmp1D1;_tmp1CE->tl=nolive_unique_params;_tmp1CE;});nolive_unique_params=_tmp6F5;});
goto _LLBF;case 22U: _LLC4: _tmp1D2=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp1CC)->f1;_LLC5:
# 1222
({struct Cyc_List_List*_tmp6F6=({struct Cyc_List_List*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF->hd=(void*)_tmp1D2;_tmp1CF->tl=noconsume_params;_tmp1CF;});noconsume_params=_tmp6F6;});
goto _LLBF;default: _LLC6: _LLC7:
 goto _LLBF;}_LLBF:;}
# 1226
goto _LLBA;}else{_LLBD: _LLBE:
({void*_tmp1D3=0U;({struct _dyneither_ptr _tmp6F7=({const char*_tmp1D4="anal_Rexp: bad function type";_tag_dyneither(_tmp1D4,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6F7,_tag_dyneither(_tmp1D3,sizeof(void*),0U));});});}_LLBA:;}
# 1229
goto _LLB5;}else{_LLB8: _LLB9:
({void*_tmp1D8=0U;({struct _dyneither_ptr _tmp6F8=({const char*_tmp1D9="anal_Rexp: bad function type";_tag_dyneither(_tmp1D9,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6F8,_tag_dyneither(_tmp1D8,sizeof(void*),0U));});});}_LLB5:;}
# 1232
{int i=1;for(0;_tmp1F2 != 0;(((_tmp1F2=_tmp1F2->tl,_tmp33B=((struct Cyc_List_List*)_check_null(_tmp33B))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp1DB=_tmp1F3;struct Cyc_Dict_Dict _tmp1EF;if((_tmp1DB.BottomFL).tag == 1){_LLC9: _LLCA:
 goto _LLC8;}else{_LLCB: _tmp1EF=((_tmp1DB.ReachableFL).val).f1;_LLCC: {
# 1237
struct _tuple15 _tmp1DC=Cyc_CfFlowInfo_unname_rval((void*)_tmp1F2->hd);struct _tuple15 _tmp1DD=_tmp1DC;void*_tmp1EE;_LLCE: _tmp1EE=_tmp1DD.f1;_LLCF:;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp1EF,(void*)_tmp1F2->hd)== Cyc_CfFlowInfo_NoneIL  && !Cyc_CfFlowInfo_is_init_pointer((void*)_tmp1F2->hd))
({void*_tmp1DE=0U;({unsigned int _tmp6FA=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp33B))->hd)->loc;struct _dyneither_ptr _tmp6F9=({const char*_tmp1DF="expression may not be initialized";_tag_dyneither(_tmp1DF,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp6FA,_tmp6F9,_tag_dyneither(_tmp1DE,sizeof(void*),0U));});});
{union Cyc_CfFlowInfo_FlowInfo _tmp1E0=_tmp1C6;struct Cyc_Dict_Dict _tmp1ED;struct Cyc_List_List*_tmp1EC;if((_tmp1E0.BottomFL).tag == 1){_LLD1: _LLD2:
 goto _LLD0;}else{_LLD3: _tmp1ED=((_tmp1E0.ReachableFL).val).f1;_tmp1EC=((_tmp1E0.ReachableFL).val).f2;_LLD4: {
# 1245
struct Cyc_Dict_Dict _tmp1E1=Cyc_CfFlowInfo_escape_deref(_tmp149,_tmp1ED,(void*)_tmp1F2->hd);
{void*_tmp1E2=(void*)_tmp1F2->hd;void*_tmp1E3=_tmp1E2;struct Cyc_CfFlowInfo_Place*_tmp1EB;switch(*((int*)_tmp1E3)){case 4U: _LLD6: _tmp1EB=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E3)->f1;_LLD7:
# 1248
{void*_tmp1E4=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp33B))->hd)->topt));void*_tmp1E5=_tmp1E4;void*_tmp1E8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E5)->tag == 5U){_LLDD: _tmp1E8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E5)->f1).elt_typ;_LLDE:
# 1250
({struct Cyc_Dict_Dict _tmp6FF=({struct Cyc_CfFlowInfo_FlowEnv*_tmp6FE=_tmp149;unsigned int _tmp6FD=((struct Cyc_Absyn_Exp*)_tmp33B->hd)->loc;struct Cyc_Dict_Dict _tmp6FC=_tmp1E1;struct Cyc_CfFlowInfo_Place*_tmp6FB=_tmp1EB;Cyc_CfFlowInfo_assign_place(_tmp6FE,_tmp6FD,_tmp6FC,_tmp6FB,
Cyc_CfFlowInfo_typ_to_absrval(_tmp149,_tmp1E8,0,_tmp149->esc_all));});
# 1250
_tmp1E1=_tmp6FF;});
# 1254
goto _LLDC;}else{_LLDF: _LLE0:
({void*_tmp1E6=0U;({struct _dyneither_ptr _tmp700=({const char*_tmp1E7="anal_Rexp:bad type for initialized arg";_tag_dyneither(_tmp1E7,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp700,_tag_dyneither(_tmp1E6,sizeof(void*),0U));});});}_LLDC:;}
# 1257
goto _LLD5;case 5U: _LLD8: _LLD9:
# 1259
({void*_tmp1E9=0U;({struct _dyneither_ptr _tmp701=({const char*_tmp1EA="anal_Rexp:initialize attribute on unique pointers not yet supported";_tag_dyneither(_tmp1EA,sizeof(char),68U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp701,_tag_dyneither(_tmp1E9,sizeof(void*),0U));});});default: _LLDA: _LLDB:
# 1261
 goto _LLD5;}_LLD5:;}
# 1263
({union Cyc_CfFlowInfo_FlowInfo _tmp702=Cyc_CfFlowInfo_ReachableFL(_tmp1E1,_tmp1EC);_tmp1C6=_tmp702;});
goto _LLD0;}}_LLD0:;}
# 1266
goto _LLC8;}}_LLC8:;}else{
# 1269
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1274
({union Cyc_CfFlowInfo_FlowInfo _tmp707=({struct Cyc_NewControlFlow_AnalEnv*_tmp706=env;unsigned int _tmp705=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp33B))->hd)->loc;void*_tmp704=(void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp33B->hd)->topt);union Cyc_CfFlowInfo_FlowInfo _tmp703=_tmp1C6;Cyc_NewControlFlow_use_nounique_Rval(_tmp706,_tmp705,_tmp704,_tmp703,(void*)_tmp1F2->hd);});_tmp1C6=_tmp707;});else{
# 1284
({union Cyc_CfFlowInfo_FlowInfo _tmp708=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp33B))->hd)->loc,_tmp1C6,(void*)_tmp1F2->hd);_tmp1C6=_tmp708;});
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp33B->hd)->topt),0))
({union Cyc_CfFlowInfo_FlowInfo _tmp709=Cyc_NewControlFlow_restore_noconsume_arg(env,_tmp1C6,(struct Cyc_Absyn_Exp*)_tmp33B->hd,((struct Cyc_Absyn_Exp*)_tmp33B->hd)->loc,(void*)_tmp1F2->hd);_tmp1C6=_tmp709;});}}}}
# 1291
Cyc_NewControlFlow_check_fn_requires(env,_tmp1C6,_tmp1C3,requires,e->loc);
# 1294
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp33C->topt)))
return({struct _tuple16 _tmp1F0;({union Cyc_CfFlowInfo_FlowInfo _tmp70A=Cyc_CfFlowInfo_BottomFL();_tmp1F0.f1=_tmp70A;});({void*_tmp70B=Cyc_CfFlowInfo_typ_to_absrval(_tmp149,(void*)_check_null(e->topt),0,_tmp149->unknown_all);_tmp1F0.f2=_tmp70B;});_tmp1F0;});else{
# 1297
return({struct _tuple16 _tmp1F1;_tmp1F1.f1=_tmp1C6;({void*_tmp70C=Cyc_CfFlowInfo_typ_to_absrval(_tmp149,(void*)_check_null(e->topt),0,_tmp149->unknown_all);_tmp1F1.f2=_tmp70C;});_tmp1F1;});}};};}case 34U: _LL48: _tmp341=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp161)->f1).is_calloc;_tmp340=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp161)->f1).rgn;_tmp33F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp161)->f1).elt_type;_tmp33E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp161)->f1).num_elts;_tmp33D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp161)->f1).fat_result;_LL49: {
# 1300
void*place_val;
if(_tmp33D)place_val=_tmp149->notzeroall;else{
if(_tmp341)({void*_tmp70D=Cyc_CfFlowInfo_typ_to_absrval(_tmp149,*((void**)_check_null(_tmp33F)),0,_tmp149->zero);place_val=_tmp70D;});else{
({void*_tmp70E=Cyc_CfFlowInfo_typ_to_absrval(_tmp149,*((void**)_check_null(_tmp33F)),0,_tmp149->unknown_none);place_val=_tmp70E;});}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
if(_tmp340 != 0){
struct _tuple23 _tmp1F5=({struct Cyc_NewControlFlow_AnalEnv*_tmp710=env;union Cyc_CfFlowInfo_FlowInfo _tmp70F=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp710,_tmp70F,({struct Cyc_Absyn_Exp*_tmp1F9[2U];_tmp1F9[1U]=_tmp33E;_tmp1F9[0U]=_tmp340;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1F9,sizeof(struct Cyc_Absyn_Exp*),2U));}),1,1);});struct _tuple23 _tmp1F6=_tmp1F5;union Cyc_CfFlowInfo_FlowInfo _tmp1F8;struct Cyc_List_List*_tmp1F7;_LLE2: _tmp1F8=_tmp1F6.f1;_tmp1F7=_tmp1F6.f2;_LLE3:;
outflow=_tmp1F8;}else{
# 1309
struct _tuple16 _tmp1FA=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp33E);struct _tuple16 _tmp1FB=_tmp1FA;union Cyc_CfFlowInfo_FlowInfo _tmp1FC;_LLE5: _tmp1FC=_tmp1FB.f1;_LLE6:;
outflow=_tmp1FC;}
# 1314
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e->topt),1))
return({struct _tuple16 _tmp1FD;_tmp1FD.f1=outflow;({void*_tmp712=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp1FE=_cycalloc(sizeof(*_tmp1FE));({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp711=({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp1FF;_tmp1FF.tag=5U;_tmp1FF.f1=place_val;_tmp1FF;});_tmp1FE[0]=_tmp711;});_tmp1FE;});_tmp1FD.f2=_tmp712;});_tmp1FD;});else{
# 1318
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp208=_cycalloc(sizeof(*_tmp208));({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp713=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp209;_tmp209.tag=1U;_tmp209.f1=_tmp33E;_tmp209.f2=(void*)_check_null(e->topt);_tmp209;});_tmp208[0]=_tmp713;});_tmp208;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp207=_cycalloc(sizeof(*_tmp207));_tmp207->root=root;_tmp207->path=0;_tmp207;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp205=_cycalloc(sizeof(*_tmp205));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp714=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp206;_tmp206.tag=4U;_tmp206.f1=place;_tmp206;});_tmp205[0]=_tmp714;});_tmp205;});
union Cyc_CfFlowInfo_FlowInfo _tmp200=outflow;struct Cyc_Dict_Dict _tmp204;struct Cyc_List_List*_tmp203;if((_tmp200.BottomFL).tag == 1){_LLE8: _LLE9:
 return({struct _tuple16 _tmp201;_tmp201.f1=outflow;_tmp201.f2=rval;_tmp201;});}else{_LLEA: _tmp204=((_tmp200.ReachableFL).val).f1;_tmp203=((_tmp200.ReachableFL).val).f2;_LLEB:
# 1324
 return({struct _tuple16 _tmp202;({union Cyc_CfFlowInfo_FlowInfo _tmp716=({struct Cyc_Dict_Dict _tmp715=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp204,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp715,_tmp203);});_tmp202.f1=_tmp716;});_tmp202.f2=rval;_tmp202;});}_LLE7:;}};}case 35U: _LL4A: _tmp343=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_tmp342=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp161)->f2;_LL4B: {
# 1329
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1335
struct _tuple23 _tmp20A=({struct Cyc_NewControlFlow_AnalEnv*_tmp718=env;union Cyc_CfFlowInfo_FlowInfo _tmp717=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp718,_tmp717,({struct Cyc_Absyn_Exp*_tmp220[2U];_tmp220[1U]=_tmp342;_tmp220[0U]=_tmp343;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp220,sizeof(struct Cyc_Absyn_Exp*),2U));}),0,0);});struct _tuple23 _tmp20B=_tmp20A;union Cyc_CfFlowInfo_FlowInfo _tmp21F;struct Cyc_List_List*_tmp21E;_LLED: _tmp21F=_tmp20B.f1;_tmp21E=_tmp20B.f2;_LLEE:;
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp21E))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp21E->tl))->hd;
outflow=_tmp21F;{
# 1340
struct _tuple18 _tmp20C=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp343);struct _tuple18 _tmp20D=_tmp20C;union Cyc_CfFlowInfo_AbsLVal _tmp21D;_LLF0: _tmp21D=_tmp20D.f2;_LLF1:;{
struct _tuple18 _tmp20E=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp342);struct _tuple18 _tmp20F=_tmp20E;union Cyc_CfFlowInfo_AbsLVal _tmp21C;_LLF3: _tmp21C=_tmp20F.f2;_LLF4:;
{union Cyc_CfFlowInfo_FlowInfo _tmp210=outflow;struct Cyc_Dict_Dict _tmp21A;struct Cyc_List_List*_tmp219;if((_tmp210.ReachableFL).tag == 2){_LLF6: _tmp21A=((_tmp210.ReachableFL).val).f1;_tmp219=((_tmp210.ReachableFL).val).f2;_LLF7:
# 1344
{union Cyc_CfFlowInfo_AbsLVal _tmp211=_tmp21D;struct Cyc_CfFlowInfo_Place*_tmp214;if((_tmp211.PlaceL).tag == 1){_LLFB: _tmp214=(_tmp211.PlaceL).val;_LLFC:
# 1346
({struct Cyc_Dict_Dict _tmp719=Cyc_CfFlowInfo_assign_place(_tmp149,_tmp343->loc,_tmp21A,_tmp214,right_rval);_tmp21A=_tmp719;});
goto _LLFA;}else{_LLFD: _LLFE:
# 1352
 if(Cyc_CfFlowInfo_initlevel(_tmp149,_tmp21A,right_rval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp212=0U;({unsigned int _tmp71B=_tmp342->loc;struct _dyneither_ptr _tmp71A=({const char*_tmp213="expression may not be fully initialized";_tag_dyneither(_tmp213,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp71B,_tmp71A,_tag_dyneither(_tmp212,sizeof(void*),0U));});});
goto _LLFA;}_LLFA:;}
# 1356
{union Cyc_CfFlowInfo_AbsLVal _tmp215=_tmp21C;struct Cyc_CfFlowInfo_Place*_tmp218;if((_tmp215.PlaceL).tag == 1){_LL100: _tmp218=(_tmp215.PlaceL).val;_LL101:
# 1358
({struct Cyc_Dict_Dict _tmp71C=Cyc_CfFlowInfo_assign_place(_tmp149,_tmp342->loc,_tmp21A,_tmp218,left_rval);_tmp21A=_tmp71C;});
goto _LLFF;}else{_LL102: _LL103:
# 1361
 if(Cyc_CfFlowInfo_initlevel(_tmp149,_tmp21A,left_rval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp216=0U;({unsigned int _tmp71E=_tmp343->loc;struct _dyneither_ptr _tmp71D=({const char*_tmp217="expression may not be fully initialized";_tag_dyneither(_tmp217,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp71E,_tmp71D,_tag_dyneither(_tmp216,sizeof(void*),0U));});});
goto _LLFF;}_LLFF:;}
# 1366
({struct Cyc_List_List*_tmp71F=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp219,_tmp343);_tmp219=_tmp71F;});
({struct Cyc_List_List*_tmp720=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp219,_tmp342);_tmp219=_tmp720;});
# 1369
({union Cyc_CfFlowInfo_FlowInfo _tmp721=Cyc_CfFlowInfo_ReachableFL(_tmp21A,_tmp219);outflow=_tmp721;});
Cyc_NewControlFlow_update_tryflow(env,outflow);
goto _LLF5;}else{_LLF8: _LLF9:
 goto _LLF5;}_LLF5:;}
# 1376
return({struct _tuple16 _tmp21B;_tmp21B.f1=outflow;_tmp21B.f2=_tmp149->unknown_all;_tmp21B;});};};}case 16U: _LL4C: _tmp345=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_tmp344=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp161)->f2;_LL4D: {
# 1379
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp345 != 0){
struct _tuple23 _tmp221=({struct Cyc_NewControlFlow_AnalEnv*_tmp723=env;union Cyc_CfFlowInfo_FlowInfo _tmp722=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp723,_tmp722,({struct Cyc_Absyn_Exp*_tmp225[2U];_tmp225[1U]=_tmp344;_tmp225[0U]=_tmp345;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp225,sizeof(struct Cyc_Absyn_Exp*),2U));}),1,1);});struct _tuple23 _tmp222=_tmp221;union Cyc_CfFlowInfo_FlowInfo _tmp224;struct Cyc_List_List*_tmp223;_LL105: _tmp224=_tmp222.f1;_tmp223=_tmp222.f2;_LL106:;
outflow=_tmp224;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp223))->tl))->hd;}else{
# 1386
struct _tuple16 _tmp226=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp344);struct _tuple16 _tmp227=_tmp226;union Cyc_CfFlowInfo_FlowInfo _tmp229;void*_tmp228;_LL108: _tmp229=_tmp227.f1;_tmp228=_tmp227.f2;_LL109:;
outflow=_tmp229;
place_val=_tmp228;}
# 1391
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e->topt),1))
return({struct _tuple16 _tmp22A;_tmp22A.f1=outflow;({void*_tmp725=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp22B=_cycalloc(sizeof(*_tmp22B));({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp724=({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp22C;_tmp22C.tag=5U;_tmp22C.f1=place_val;_tmp22C;});_tmp22B[0]=_tmp724;});_tmp22B;});_tmp22A.f2=_tmp725;});_tmp22A;});else{
# 1395
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp235=_cycalloc(sizeof(*_tmp235));({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp726=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp236;_tmp236.tag=1U;_tmp236.f1=_tmp344;_tmp236.f2=(void*)_check_null(e->topt);_tmp236;});_tmp235[0]=_tmp726;});_tmp235;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp234=_cycalloc(sizeof(*_tmp234));_tmp234->root=root;_tmp234->path=0;_tmp234;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp232=_cycalloc(sizeof(*_tmp232));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp727=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp233;_tmp233.tag=4U;_tmp233.f1=place;_tmp233;});_tmp232[0]=_tmp727;});_tmp232;});
union Cyc_CfFlowInfo_FlowInfo _tmp22D=outflow;struct Cyc_Dict_Dict _tmp231;struct Cyc_List_List*_tmp230;if((_tmp22D.BottomFL).tag == 1){_LL10B: _LL10C:
 return({struct _tuple16 _tmp22E;_tmp22E.f1=outflow;_tmp22E.f2=rval;_tmp22E;});}else{_LL10D: _tmp231=((_tmp22D.ReachableFL).val).f1;_tmp230=((_tmp22D.ReachableFL).val).f2;_LL10E:
# 1401
 return({struct _tuple16 _tmp22F;({union Cyc_CfFlowInfo_FlowInfo _tmp729=({struct Cyc_Dict_Dict _tmp728=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp231,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp728,_tmp230);});_tmp22F.f1=_tmp729;});_tmp22F.f2=rval;_tmp22F;});}_LL10A:;}}case 15U: _LL4E: _tmp346=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_LL4F: {
# 1406
struct _tuple18 _tmp237=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp346);struct _tuple18 _tmp238=_tmp237;union Cyc_CfFlowInfo_FlowInfo _tmp240;union Cyc_CfFlowInfo_AbsLVal _tmp23F;_LL110: _tmp240=_tmp238.f1;_tmp23F=_tmp238.f2;_LL111:;{
union Cyc_CfFlowInfo_AbsLVal _tmp239=_tmp23F;struct Cyc_CfFlowInfo_Place*_tmp23E;if((_tmp239.UnknownL).tag == 2){_LL113: _LL114:
 return({struct _tuple16 _tmp23A;_tmp23A.f1=_tmp240;_tmp23A.f2=_tmp149->notzeroall;_tmp23A;});}else{_LL115: _tmp23E=(_tmp239.PlaceL).val;_LL116:
 return({struct _tuple16 _tmp23B;_tmp23B.f1=_tmp240;({void*_tmp72B=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp23C=_cycalloc(sizeof(*_tmp23C));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp72A=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp23D;_tmp23D.tag=4U;_tmp23D.f1=_tmp23E;_tmp23D;});_tmp23C[0]=_tmp72A;});_tmp23C;});_tmp23B.f2=_tmp72B;});_tmp23B;});}_LL112:;};}case 20U: _LL50: _tmp347=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_LL51: {
# 1413
struct _tuple16 _tmp241=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp347);struct _tuple16 _tmp242=_tmp241;union Cyc_CfFlowInfo_FlowInfo _tmp244;void*_tmp243;_LL118: _tmp244=_tmp242.f1;_tmp243=_tmp242.f2;_LL119:;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp244,_tmp347,_tmp243);}case 21U: _LL52: _tmp349=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_tmp348=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp161)->f2;_LL53: {
# 1417
struct _tuple16 _tmp245=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp349);struct _tuple16 _tmp246=_tmp245;union Cyc_CfFlowInfo_FlowInfo _tmp259;void*_tmp258;_LL11B: _tmp259=_tmp246.f1;_tmp258=_tmp246.f2;_LL11C:;{
void*_tmp247=(void*)_check_null(_tmp349->topt);
# 1420
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp247))
return({struct _tuple16 _tmp248;_tmp248.f1=_tmp259;({void*_tmp72C=Cyc_CfFlowInfo_typ_to_absrval(_tmp149,(void*)_check_null(e->topt),0,_tmp149->unknown_all);_tmp248.f2=_tmp72C;});_tmp248;});
if(Cyc_Absyn_is_require_union_type(_tmp247))
Cyc_NewControlFlow_check_union_requires(_tmp349->loc,_tmp247,_tmp348,_tmp259);{
struct _tuple15 _tmp249=Cyc_CfFlowInfo_unname_rval(_tmp258);struct _tuple15 _tmp24A=_tmp249;void*_tmp257;_LL11E: _tmp257=_tmp24A.f1;_LL11F:;{
void*_tmp24B=_tmp257;int _tmp256;int _tmp255;struct _dyneither_ptr _tmp254;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp24B)->tag == 6U){_LL121: _tmp256=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp24B)->f1).is_union;_tmp255=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp24B)->f1).fieldnum;_tmp254=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp24B)->f2;_LL122: {
# 1427
int _tmp24C=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp349->topt),_tmp348);
if((_tmp256  && _tmp255 != - 1) && _tmp255 != _tmp24C)
return({struct _tuple16 _tmp24D;_tmp24D.f1=_tmp259;({void*_tmp72D=Cyc_CfFlowInfo_typ_to_absrval(_tmp149,(void*)_check_null(e->topt),1,_tmp149->unknown_none);_tmp24D.f2=_tmp72D;});_tmp24D;});
return({struct _tuple16 _tmp24E;_tmp24E.f1=_tmp259;_tmp24E.f2=*((void**)_check_dyneither_subscript(_tmp254,sizeof(void*),_tmp24C));_tmp24E;});}}else{_LL123: _LL124:
# 1432
({void*_tmp24F=0U;({struct Cyc___cycFILE*_tmp72F=Cyc_stderr;struct _dyneither_ptr _tmp72E=({const char*_tmp250="the bad rexp is :";_tag_dyneither(_tmp250,sizeof(char),18U);});Cyc_fprintf(_tmp72F,_tmp72E,_tag_dyneither(_tmp24F,sizeof(void*),0U));});});
Cyc_CfFlowInfo_print_absrval(_tmp257);
({struct Cyc_String_pa_PrintArg_struct _tmp253;_tmp253.tag=0U;({struct _dyneither_ptr _tmp730=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp253.f1=_tmp730;});({void*_tmp251[1U]={& _tmp253};({struct _dyneither_ptr _tmp731=({const char*_tmp252="\nanal_Rexp: AggrMember: %s";_tag_dyneither(_tmp252,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp731,_tag_dyneither(_tmp251,sizeof(void*),1U));});});});}_LL120:;};};};}case 38U: _LL54: _tmp34B=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_tmp34A=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp161)->f2;_LL55: {
# 1440
struct _tuple16 _tmp25A=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp34B);struct _tuple16 _tmp25B=_tmp25A;union Cyc_CfFlowInfo_FlowInfo _tmp26C;void*_tmp26B;_LL126: _tmp26C=_tmp25B.f1;_tmp26B=_tmp25B.f2;_LL127:;
# 1442
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp34B->topt)))
return({struct _tuple16 _tmp25C;_tmp25C.f1=_tmp26C;_tmp25C.f2=_tmp149->unknown_all;_tmp25C;});{
struct _tuple15 _tmp25D=Cyc_CfFlowInfo_unname_rval(_tmp26B);struct _tuple15 _tmp25E=_tmp25D;void*_tmp26A;_LL129: _tmp26A=_tmp25E.f1;_LL12A:;{
void*_tmp25F=_tmp26A;int _tmp269;int _tmp268;struct _dyneither_ptr _tmp267;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp25F)->tag == 6U){_LL12C: _tmp269=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp25F)->f1).is_union;_tmp268=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp25F)->f1).fieldnum;_tmp267=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp25F)->f2;_LL12D: {
# 1447
int _tmp260=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp34B->topt),_tmp34A);
if(_tmp269  && _tmp268 != - 1){
if(_tmp268 != _tmp260)
return({struct _tuple16 _tmp261;_tmp261.f1=_tmp26C;_tmp261.f2=_tmp149->zero;_tmp261;});else{
# 1452
return({struct _tuple16 _tmp262;_tmp262.f1=_tmp26C;_tmp262.f2=_tmp149->notzeroall;_tmp262;});}}else{
# 1454
return({struct _tuple16 _tmp263;_tmp263.f1=_tmp26C;_tmp263.f2=_tmp149->unknown_all;_tmp263;});}}}else{_LL12E: _LL12F:
# 1456
({struct Cyc_String_pa_PrintArg_struct _tmp266;_tmp266.tag=0U;({struct _dyneither_ptr _tmp732=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp266.f1=_tmp732;});({void*_tmp264[1U]={& _tmp266};({struct _dyneither_ptr _tmp733=({const char*_tmp265="anal_Rexp: TagCheck_e: %s";_tag_dyneither(_tmp265,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp733,_tag_dyneither(_tmp264,sizeof(void*),1U));});});});}_LL12B:;};};}case 22U: _LL56: _tmp34D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_tmp34C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp161)->f2;_LL57: {
# 1460
struct _tuple16 _tmp26D=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp34D);struct _tuple16 _tmp26E=_tmp26D;union Cyc_CfFlowInfo_FlowInfo _tmp286;void*_tmp285;_LL131: _tmp286=_tmp26E.f1;_tmp285=_tmp26E.f2;_LL132:;{
# 1463
struct _tuple16 _tmp26F=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp286,_tmp34D,_tmp285);struct _tuple16 _tmp270=_tmp26F;union Cyc_CfFlowInfo_FlowInfo _tmp284;void*_tmp283;_LL134: _tmp284=_tmp270.f1;_tmp283=_tmp270.f2;_LL135:;{
# 1466
void*_tmp271=Cyc_Tcutil_compress((void*)_check_null(_tmp34D->topt));void*_tmp272=_tmp271;void*_tmp282;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp272)->tag == 5U){_LL137: _tmp282=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp272)->f1).elt_typ;_LL138:
# 1468
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp282))
# 1470
return({struct _tuple16 _tmp273;_tmp273.f1=_tmp284;({void*_tmp734=Cyc_CfFlowInfo_typ_to_absrval(_tmp149,(void*)_check_null(e->topt),0,_tmp149->unknown_all);_tmp273.f2=_tmp734;});_tmp273;});
# 1472
if(Cyc_Absyn_is_require_union_type(_tmp282))
Cyc_NewControlFlow_check_union_requires(_tmp34D->loc,_tmp282,_tmp34C,_tmp286);{
# 1475
struct _tuple15 _tmp274=Cyc_CfFlowInfo_unname_rval(_tmp283);struct _tuple15 _tmp275=_tmp274;void*_tmp27F;_LL13C: _tmp27F=_tmp275.f1;_LL13D:;{
void*_tmp276=_tmp27F;int _tmp27E;int _tmp27D;struct _dyneither_ptr _tmp27C;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp276)->tag == 6U){_LL13F: _tmp27E=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp276)->f1).is_union;_tmp27D=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp276)->f1).fieldnum;_tmp27C=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp276)->f2;_LL140: {
# 1478
int _tmp277=Cyc_CfFlowInfo_get_field_index(_tmp282,_tmp34C);
if((_tmp27E  && _tmp27D != - 1) && _tmp27D != _tmp277)
return({struct _tuple16 _tmp278;_tmp278.f1=_tmp284;({void*_tmp735=Cyc_CfFlowInfo_typ_to_absrval(_tmp149,(void*)_check_null(e->topt),1,_tmp149->unknown_none);_tmp278.f2=_tmp735;});_tmp278;});
return({struct _tuple16 _tmp279;_tmp279.f1=_tmp284;_tmp279.f2=*((void**)_check_dyneither_subscript(_tmp27C,sizeof(void*),_tmp277));_tmp279;});}}else{_LL141: _LL142:
({void*_tmp27A=0U;({struct _dyneither_ptr _tmp736=({const char*_tmp27B="anal_Rexp: AggrArrow";_tag_dyneither(_tmp27B,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp736,_tag_dyneither(_tmp27A,sizeof(void*),0U));});});}_LL13E:;};};}else{_LL139: _LL13A:
# 1484
({void*_tmp280=0U;({struct _dyneither_ptr _tmp737=({const char*_tmp281="anal_Rexp: AggrArrow ptr";_tag_dyneither(_tmp281,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp737,_tag_dyneither(_tmp280,sizeof(void*),0U));});});}_LL136:;};};}case 6U: _LL58: _tmp350=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_tmp34F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp161)->f2;_tmp34E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp161)->f3;_LL59: {
# 1488
struct _tuple19 _tmp287=Cyc_NewControlFlow_anal_test(env,inflow,_tmp350);struct _tuple19 _tmp288=_tmp287;union Cyc_CfFlowInfo_FlowInfo _tmp28C;union Cyc_CfFlowInfo_FlowInfo _tmp28B;_LL144: _tmp28C=_tmp288.f1;_tmp28B=_tmp288.f2;_LL145:;{
struct _tuple16 _tmp289=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp28C,_tmp34F);
struct _tuple16 _tmp28A=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp28B,_tmp34E);
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp149,_tmp289,_tmp28A);};}case 7U: _LL5A: _tmp352=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_tmp351=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp161)->f2;_LL5B: {
# 1494
struct _tuple19 _tmp28D=Cyc_NewControlFlow_anal_test(env,inflow,_tmp352);struct _tuple19 _tmp28E=_tmp28D;union Cyc_CfFlowInfo_FlowInfo _tmp298;union Cyc_CfFlowInfo_FlowInfo _tmp297;_LL147: _tmp298=_tmp28E.f1;_tmp297=_tmp28E.f2;_LL148:;{
struct _tuple16 _tmp28F=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp298,_tmp351);struct _tuple16 _tmp290=_tmp28F;union Cyc_CfFlowInfo_FlowInfo _tmp296;void*_tmp295;_LL14A: _tmp296=_tmp290.f1;_tmp295=_tmp290.f2;_LL14B:;{
struct _tuple16 _tmp291=({struct _tuple16 _tmp294;_tmp294.f1=_tmp296;_tmp294.f2=_tmp295;_tmp294;});
struct _tuple16 _tmp292=({struct _tuple16 _tmp293;_tmp293.f1=_tmp297;_tmp293.f2=_tmp149->zero;_tmp293;});
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp149,_tmp291,_tmp292);};};}case 8U: _LL5C: _tmp354=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_tmp353=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp161)->f2;_LL5D: {
# 1501
struct _tuple19 _tmp299=Cyc_NewControlFlow_anal_test(env,inflow,_tmp354);struct _tuple19 _tmp29A=_tmp299;union Cyc_CfFlowInfo_FlowInfo _tmp2A4;union Cyc_CfFlowInfo_FlowInfo _tmp2A3;_LL14D: _tmp2A4=_tmp29A.f1;_tmp2A3=_tmp29A.f2;_LL14E:;{
struct _tuple16 _tmp29B=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2A3,_tmp353);struct _tuple16 _tmp29C=_tmp29B;union Cyc_CfFlowInfo_FlowInfo _tmp2A2;void*_tmp2A1;_LL150: _tmp2A2=_tmp29C.f1;_tmp2A1=_tmp29C.f2;_LL151:;{
struct _tuple16 _tmp29D=({struct _tuple16 _tmp2A0;_tmp2A0.f1=_tmp2A2;_tmp2A0.f2=_tmp2A1;_tmp2A0;});
struct _tuple16 _tmp29E=({struct _tuple16 _tmp29F;_tmp29F.f1=_tmp2A4;_tmp29F.f2=_tmp149->notzeroall;_tmp29F;});
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp149,_tmp29D,_tmp29E);};};}case 23U: _LL5E: _tmp356=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_tmp355=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp161)->f2;_LL5F: {
# 1508
struct _tuple23 _tmp2A5=({struct Cyc_NewControlFlow_AnalEnv*_tmp739=env;union Cyc_CfFlowInfo_FlowInfo _tmp738=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp739,_tmp738,({struct Cyc_Absyn_Exp*_tmp2C4[2U];_tmp2C4[1U]=_tmp355;_tmp2C4[0U]=_tmp356;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2C4,sizeof(struct Cyc_Absyn_Exp*),2U));}),0,1);});struct _tuple23 _tmp2A6=_tmp2A5;union Cyc_CfFlowInfo_FlowInfo _tmp2C3;struct Cyc_List_List*_tmp2C2;_LL153: _tmp2C3=_tmp2A6.f1;_tmp2C2=_tmp2A6.f2;_LL154:;{
# 1512
union Cyc_CfFlowInfo_FlowInfo _tmp2A7=_tmp2C3;
{union Cyc_CfFlowInfo_FlowInfo _tmp2A8=_tmp2C3;struct Cyc_Dict_Dict _tmp2AD;struct Cyc_List_List*_tmp2AC;if((_tmp2A8.ReachableFL).tag == 2){_LL156: _tmp2AD=((_tmp2A8.ReachableFL).val).f1;_tmp2AC=((_tmp2A8.ReachableFL).val).f2;_LL157:
# 1517
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2AD,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp2C2))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp2A9=0U;({unsigned int _tmp73B=_tmp355->loc;struct _dyneither_ptr _tmp73A=({const char*_tmp2AA="expression may not be initialized";_tag_dyneither(_tmp2AA,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp73B,_tmp73A,_tag_dyneither(_tmp2A9,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp2AB=Cyc_NewControlFlow_add_subscript_reln(_tmp2AC,_tmp356,_tmp355);
if(_tmp2AC != _tmp2AB)
({union Cyc_CfFlowInfo_FlowInfo _tmp73C=Cyc_CfFlowInfo_ReachableFL(_tmp2AD,_tmp2AB);_tmp2A7=_tmp73C;});
goto _LL155;};}else{_LL158: _LL159:
 goto _LL155;}_LL155:;}{
# 1525
void*_tmp2AE=Cyc_Tcutil_compress((void*)_check_null(_tmp356->topt));void*_tmp2AF=_tmp2AE;struct Cyc_List_List*_tmp2C1;switch(*((int*)_tmp2AF)){case 10U: _LL15B: _tmp2C1=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2AF)->f1;_LL15C: {
# 1527
struct _tuple15 _tmp2B0=Cyc_CfFlowInfo_unname_rval((void*)((struct Cyc_List_List*)_check_null(_tmp2C2))->hd);struct _tuple15 _tmp2B1=_tmp2B0;void*_tmp2B7;_LL162: _tmp2B7=_tmp2B1.f1;_LL163:;{
void*_tmp2B2=_tmp2B7;struct _dyneither_ptr _tmp2B6;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2B2)->tag == 6U){_LL165: _tmp2B6=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2B2)->f2;_LL166: {
# 1530
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp355)).f1;
return({struct _tuple16 _tmp2B3;_tmp2B3.f1=_tmp2A7;_tmp2B3.f2=*((void**)_check_dyneither_subscript(_tmp2B6,sizeof(void*),(int)i));_tmp2B3;});}}else{_LL167: _LL168:
({void*_tmp2B4=0U;({struct _dyneither_ptr _tmp73D=({const char*_tmp2B5="anal_Rexp: Subscript";_tag_dyneither(_tmp2B5,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp73D,_tag_dyneither(_tmp2B4,sizeof(void*),0U));});});}_LL164:;};}case 5U: _LL15D: _LL15E: {
# 1535
struct _tuple16 _tmp2B8=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2C3,_tmp356,(void*)((struct Cyc_List_List*)_check_null(_tmp2C2))->hd);struct _tuple16 _tmp2B9=_tmp2B8;union Cyc_CfFlowInfo_FlowInfo _tmp2BE;void*_tmp2BD;_LL16A: _tmp2BE=_tmp2B9.f1;_tmp2BD=_tmp2B9.f2;_LL16B:;{
union Cyc_CfFlowInfo_FlowInfo _tmp2BA=_tmp2BE;if((_tmp2BA.BottomFL).tag == 1){_LL16D: _LL16E:
 return({struct _tuple16 _tmp2BB;_tmp2BB.f1=_tmp2BE;_tmp2BB.f2=_tmp2BD;_tmp2BB;});}else{_LL16F: _LL170:
 return({struct _tuple16 _tmp2BC;_tmp2BC.f1=_tmp2A7;_tmp2BC.f2=_tmp2BD;_tmp2BC;});}_LL16C:;};}default: _LL15F: _LL160:
# 1540
({void*_tmp2BF=0U;({struct _dyneither_ptr _tmp73E=({const char*_tmp2C0="anal_Rexp: Subscript -- bad type";_tag_dyneither(_tmp2C0,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp73E,_tag_dyneither(_tmp2BF,sizeof(void*),0U));});});}_LL15A:;};};}case 24U: _LL62: _tmp357=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_LL63: {
# 1546
struct _tuple23 _tmp2C5=Cyc_NewControlFlow_anal_Rexps(env,inflow,_tmp357,1,1);struct _tuple23 _tmp2C6=_tmp2C5;union Cyc_CfFlowInfo_FlowInfo _tmp2D1;struct Cyc_List_List*_tmp2D0;_LL172: _tmp2D1=_tmp2C6.f1;_tmp2D0=_tmp2C6.f2;_LL173:;{
struct _dyneither_ptr aggrdict=({unsigned int _tmp2CB=(unsigned int)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp357);void**_tmp2CC=(void**)_cycalloc(_check_times(sizeof(void*),_tmp2CB));struct _dyneither_ptr _tmp2CF=_tag_dyneither(_tmp2CC,sizeof(void*),_tmp2CB);{unsigned int _tmp2CD=_tmp2CB;unsigned int i;for(i=0;i < _tmp2CD;i ++){({typeof((void*)({
void*_tmp2CE=(void*)((struct Cyc_List_List*)_check_null(_tmp2D0))->hd;
_tmp2D0=_tmp2D0->tl;
_tmp2CE;}))_tmp73F=(void*)({
# 1549
void*_tmp2CE=(void*)((struct Cyc_List_List*)_check_null(_tmp2D0))->hd;
_tmp2D0=_tmp2D0->tl;
_tmp2CE;});_tmp2CC[i]=_tmp73F;});}}_tmp2CF;});
# 1553
return({struct _tuple16 _tmp2C7;_tmp2C7.f1=_tmp2D1;({void*_tmp742=(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2C8=_cycalloc(sizeof(*_tmp2C8));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp741=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2C9;_tmp2C9.tag=6U;({struct Cyc_CfFlowInfo_UnionRInfo _tmp740=({struct Cyc_CfFlowInfo_UnionRInfo _tmp2CA;_tmp2CA.is_union=0;_tmp2CA.fieldnum=- 1;_tmp2CA;});_tmp2C9.f1=_tmp740;});_tmp2C9.f2=aggrdict;_tmp2C9;});_tmp2C8[0]=_tmp741;});_tmp2C8;});_tmp2C7.f2=_tmp742;});_tmp2C7;});};}case 30U: _LL64: _tmp358=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp161)->f2;_LL65: {
# 1556
struct Cyc_List_List*fs;
{void*_tmp2D2=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2D3=_tmp2D2;struct Cyc_List_List*_tmp2D6;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D3)->tag == 12U){_LL175: _tmp2D6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D3)->f2;_LL176:
 fs=_tmp2D6;goto _LL174;}else{_LL177: _LL178:
({void*_tmp2D4=0U;({struct _dyneither_ptr _tmp743=({const char*_tmp2D5="anal_Rexp:anon struct has bad type";_tag_dyneither(_tmp2D5,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp743,_tag_dyneither(_tmp2D4,sizeof(void*),0U));});});}_LL174:;}
# 1561
_tmp35B=_tmp358;_tmp35A=Cyc_Absyn_StructA;_tmp359=fs;goto _LL67;}case 29U: if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp161)->f4 != 0){if(((struct Cyc_Absyn_Aggrdecl*)((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp161)->f4)->impl != 0){_LL66: _tmp35B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp161)->f3;_tmp35A=(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp161)->f4)->kind;_tmp359=((((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp161)->f4)->impl)->fields;_LL67: {
# 1563
void*exp_type=(void*)_check_null(e->topt);
struct _tuple23 _tmp2D7=({struct Cyc_NewControlFlow_AnalEnv*_tmp745=env;union Cyc_CfFlowInfo_FlowInfo _tmp744=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp745,_tmp744,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp35B),1,1);});struct _tuple23 _tmp2D8=_tmp2D7;union Cyc_CfFlowInfo_FlowInfo _tmp2E5;struct Cyc_List_List*_tmp2E4;_LL17A: _tmp2E5=_tmp2D8.f1;_tmp2E4=_tmp2D8.f2;_LL17B:;{
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp149,_tmp359,_tmp35A == Cyc_Absyn_UnionA,_tmp149->unknown_all);
# 1568
int field_no=-1;
{int i=0;for(0;_tmp2E4 != 0;(((_tmp2E4=_tmp2E4->tl,_tmp35B=_tmp35B->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp35B))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp2D9=(void*)ds->hd;void*_tmp2DA=_tmp2D9;struct _dyneither_ptr*_tmp2DE;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2DA)->tag == 0U){_LL17D: _LL17E:
({void*_tmp2DB=0U;({struct _dyneither_ptr _tmp746=({const char*_tmp2DC="anal_Rexp:Aggregate_e";_tag_dyneither(_tmp2DC,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp746,_tag_dyneither(_tmp2DB,sizeof(void*),0U));});});}else{_LL17F: _tmp2DE=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2DA)->f1;_LL180:
# 1575
({int _tmp747=Cyc_CfFlowInfo_get_field_index_fs(_tmp359,_tmp2DE);field_no=_tmp747;});
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp2E4->hd;
# 1578
if(_tmp35A == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp2DD=(*((struct _tuple26*)_tmp35B->hd)).f2;
({union Cyc_CfFlowInfo_FlowInfo _tmp748=Cyc_NewControlFlow_use_Rval(env,_tmp2DD->loc,_tmp2E5,(void*)_tmp2E4->hd);_tmp2E5=_tmp748;});
# 1582
Cyc_NewControlFlow_check_union_requires(_tmp2DD->loc,exp_type,_tmp2DE,_tmp2E5);}}_LL17C:;}}}{
# 1585
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2DF=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2E1=_cycalloc(sizeof(*_tmp2E1));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp74A=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2E2;_tmp2E2.tag=6U;({struct Cyc_CfFlowInfo_UnionRInfo _tmp749=({struct Cyc_CfFlowInfo_UnionRInfo _tmp2E3;_tmp2E3.is_union=_tmp35A == Cyc_Absyn_UnionA;_tmp2E3.fieldnum=field_no;_tmp2E3;});_tmp2E2.f1=_tmp749;});_tmp2E2.f2=aggrdict;_tmp2E2;});_tmp2E1[0]=_tmp74A;});_tmp2E1;});
return({struct _tuple16 _tmp2E0;_tmp2E0.f1=_tmp2E5;_tmp2E0.f2=(void*)_tmp2DF;_tmp2E0;});};};}}else{goto _LL68;}}else{_LL68: _LL69:
# 1589
({struct Cyc_String_pa_PrintArg_struct _tmp2E8;_tmp2E8.tag=0U;({struct _dyneither_ptr _tmp74B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp2E8.f1=_tmp74B;});({void*_tmp2E6[1U]={& _tmp2E8};({struct _dyneither_ptr _tmp74C=({const char*_tmp2E7="anal_Rexp:missing aggrdeclimpl in %s";_tag_dyneither(_tmp2E7,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp74C,_tag_dyneither(_tmp2E6,sizeof(void*),1U));});});});}case 26U: _LL6A: _tmp35C=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_LL6B: {
# 1591
struct Cyc_List_List*_tmp2E9=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp35C);
struct _tuple23 _tmp2EA=Cyc_NewControlFlow_anal_Rexps(env,inflow,_tmp2E9,1,1);struct _tuple23 _tmp2EB=_tmp2EA;union Cyc_CfFlowInfo_FlowInfo _tmp2EE;struct Cyc_List_List*_tmp2ED;_LL182: _tmp2EE=_tmp2EB.f1;_tmp2ED=_tmp2EB.f2;_LL183:;
for(0;_tmp2ED != 0;(_tmp2ED=_tmp2ED->tl,_tmp2E9=_tmp2E9->tl)){
({union Cyc_CfFlowInfo_FlowInfo _tmp74D=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2E9))->hd)->loc,_tmp2EE,(void*)_tmp2ED->hd);_tmp2EE=_tmp74D;});}
return({struct _tuple16 _tmp2EC;_tmp2EC.f1=_tmp2EE;({void*_tmp74E=Cyc_CfFlowInfo_typ_to_absrval(_tmp149,(void*)_check_null(e->topt),0,_tmp149->unknown_all);_tmp2EC.f2=_tmp74E;});_tmp2EC;});}case 27U: _LL6C: _tmp360=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_tmp35F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp161)->f2;_tmp35E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp161)->f3;_tmp35D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp161)->f4;_LL6D: {
# 1599
struct _tuple16 _tmp2EF=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp35F);struct _tuple16 _tmp2F0=_tmp2EF;union Cyc_CfFlowInfo_FlowInfo _tmp313;void*_tmp312;_LL185: _tmp313=_tmp2F0.f1;_tmp312=_tmp2F0.f2;_LL186:;{
union Cyc_CfFlowInfo_FlowInfo _tmp2F1=_tmp313;struct Cyc_Dict_Dict _tmp311;struct Cyc_List_List*_tmp310;if((_tmp2F1.BottomFL).tag == 1){_LL188: _LL189:
 return({struct _tuple16 _tmp2F2;_tmp2F2.f1=_tmp313;_tmp2F2.f2=_tmp149->unknown_all;_tmp2F2;});}else{_LL18A: _tmp311=((_tmp2F1.ReachableFL).val).f1;_tmp310=((_tmp2F1.ReachableFL).val).f2;_LL18B:
# 1603
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp311,_tmp312)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp2F3=0U;({unsigned int _tmp750=_tmp35F->loc;struct _dyneither_ptr _tmp74F=({const char*_tmp2F4="expression may not be initialized";_tag_dyneither(_tmp2F4,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp750,_tmp74F,_tag_dyneither(_tmp2F3,sizeof(void*),0U));});});{
# 1607
struct Cyc_List_List*new_relns=_tmp310;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(_tmp360);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(_tmp35F,& n2))
({struct Cyc_List_List*_tmp751=Cyc_Relations_add_relation(Cyc_Core_heap_region,n1,Cyc_Relations_Rlt,n2,_tmp310);new_relns=_tmp751;});
if(_tmp310 != new_relns)
({union Cyc_CfFlowInfo_FlowInfo _tmp752=Cyc_CfFlowInfo_ReachableFL(_tmp311,new_relns);_tmp313=_tmp752;});{
# 1616
void*_tmp2F5=_tmp312;switch(*((int*)_tmp2F5)){case 0U: _LL18D: _LL18E:
 return({struct _tuple16 _tmp2F6;_tmp2F6.f1=_tmp313;_tmp2F6.f2=_tmp149->unknown_all;_tmp2F6;});case 1U: _LL18F: _LL190:
 goto _LL192;case 4U: _LL191: _LL192: {
# 1620
struct Cyc_List_List _tmp2F7=({struct Cyc_List_List _tmp302;_tmp302.hd=_tmp360;_tmp302.tl=0;_tmp302;});
({union Cyc_CfFlowInfo_FlowInfo _tmp753=Cyc_NewControlFlow_add_vars(_tmp149,_tmp313,& _tmp2F7,_tmp149->unknown_all,e->loc,0);_tmp313=_tmp753;});{
# 1623
struct _tuple16 _tmp2F8=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp313,_tmp35E);struct _tuple16 _tmp2F9=_tmp2F8;union Cyc_CfFlowInfo_FlowInfo _tmp301;void*_tmp300;_LL196: _tmp301=_tmp2F9.f1;_tmp300=_tmp2F9.f2;_LL197:;
{union Cyc_CfFlowInfo_FlowInfo _tmp2FA=_tmp301;struct Cyc_Dict_Dict _tmp2FF;if((_tmp2FA.BottomFL).tag == 1){_LL199: _LL19A:
 return({struct _tuple16 _tmp2FB;_tmp2FB.f1=_tmp301;_tmp2FB.f2=_tmp149->unknown_all;_tmp2FB;});}else{_LL19B: _tmp2FF=((_tmp2FA.ReachableFL).val).f1;_LL19C:
# 1627
 if(Cyc_CfFlowInfo_initlevel(_tmp149,_tmp2FF,_tmp300)!= Cyc_CfFlowInfo_AllIL){
({void*_tmp2FC=0U;({unsigned int _tmp755=_tmp35F->loc;struct _dyneither_ptr _tmp754=({const char*_tmp2FD="expression may not be initialized";_tag_dyneither(_tmp2FD,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp755,_tmp754,_tag_dyneither(_tmp2FC,sizeof(void*),0U));});});
return({struct _tuple16 _tmp2FE;({union Cyc_CfFlowInfo_FlowInfo _tmp756=Cyc_CfFlowInfo_BottomFL();_tmp2FE.f1=_tmp756;});_tmp2FE.f2=_tmp149->unknown_all;_tmp2FE;});}}_LL198:;}
# 1632
_tmp313=_tmp301;
goto _LL194;};}default: _LL193: _LL194:
# 1635
 while(1){
struct Cyc_List_List _tmp303=({struct Cyc_List_List _tmp30E;_tmp30E.hd=_tmp360;_tmp30E.tl=0;_tmp30E;});
({union Cyc_CfFlowInfo_FlowInfo _tmp757=Cyc_NewControlFlow_add_vars(_tmp149,_tmp313,& _tmp303,_tmp149->unknown_all,e->loc,0);_tmp313=_tmp757;});{
struct _tuple16 _tmp304=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp313,_tmp35E);struct _tuple16 _tmp305=_tmp304;union Cyc_CfFlowInfo_FlowInfo _tmp30D;void*_tmp30C;_LL19E: _tmp30D=_tmp305.f1;_tmp30C=_tmp305.f2;_LL19F:;
{union Cyc_CfFlowInfo_FlowInfo _tmp306=_tmp30D;struct Cyc_Dict_Dict _tmp30A;if((_tmp306.BottomFL).tag == 1){_LL1A1: _LL1A2:
 goto _LL1A0;}else{_LL1A3: _tmp30A=((_tmp306.ReachableFL).val).f1;_LL1A4:
# 1642
 if(Cyc_CfFlowInfo_initlevel(_tmp149,_tmp30A,_tmp30C)!= Cyc_CfFlowInfo_AllIL){
({void*_tmp307=0U;({unsigned int _tmp759=_tmp35F->loc;struct _dyneither_ptr _tmp758=({const char*_tmp308="expression may not be initialized";_tag_dyneither(_tmp308,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp759,_tmp758,_tag_dyneither(_tmp307,sizeof(void*),0U));});});
return({struct _tuple16 _tmp309;({union Cyc_CfFlowInfo_FlowInfo _tmp75A=Cyc_CfFlowInfo_BottomFL();_tmp309.f1=_tmp75A;});_tmp309.f2=_tmp149->unknown_all;_tmp309;});}}_LL1A0:;}{
# 1647
union Cyc_CfFlowInfo_FlowInfo _tmp30B=Cyc_CfFlowInfo_join_flow(_tmp149,_tmp313,_tmp30D);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp30B,_tmp313))
break;
_tmp313=_tmp30B;};};}
# 1652
return({struct _tuple16 _tmp30F;_tmp30F.f1=_tmp313;_tmp30F.f2=_tmp149->unknown_all;_tmp30F;});}_LL18C:;};};}_LL187:;};}case 28U: _LL6E: _tmp363=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_tmp362=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp161)->f2;_tmp361=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp161)->f3;_LL6F: {
# 1658
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp31F=_cycalloc(sizeof(*_tmp31F));({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp75B=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp320;_tmp320.tag=1U;_tmp320.f1=_tmp363;_tmp320.f2=(void*)_check_null(e->topt);_tmp320;});_tmp31F[0]=_tmp75B;});_tmp31F;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp31E=_cycalloc(sizeof(*_tmp31E));_tmp31E->root=root;_tmp31E->path=0;_tmp31E;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp31C=_cycalloc(sizeof(*_tmp31C));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp75C=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp31D;_tmp31D.tag=4U;_tmp31D.f1=place;_tmp31D;});_tmp31C[0]=_tmp75C;});_tmp31C;});
void*place_val;
# 1666
({void*_tmp75D=Cyc_CfFlowInfo_typ_to_absrval(_tmp149,_tmp362,0,_tmp149->unknown_none);place_val=_tmp75D;});{
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple16 _tmp314=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp363);struct _tuple16 _tmp315=_tmp314;union Cyc_CfFlowInfo_FlowInfo _tmp31B;_LL1A6: _tmp31B=_tmp315.f1;_LL1A7:;
outflow=_tmp31B;{
union Cyc_CfFlowInfo_FlowInfo _tmp316=outflow;struct Cyc_Dict_Dict _tmp31A;struct Cyc_List_List*_tmp319;if((_tmp316.BottomFL).tag == 1){_LL1A9: _LL1AA:
 return({struct _tuple16 _tmp317;_tmp317.f1=outflow;_tmp317.f2=rval;_tmp317;});}else{_LL1AB: _tmp31A=((_tmp316.ReachableFL).val).f1;_tmp319=((_tmp316.ReachableFL).val).f2;_LL1AC:
# 1673
 return({struct _tuple16 _tmp318;({union Cyc_CfFlowInfo_FlowInfo _tmp75F=({struct Cyc_Dict_Dict _tmp75E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp31A,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp75E,_tmp319);});_tmp318.f1=_tmp75F;});_tmp318.f2=rval;_tmp318;});}_LL1A8:;};};}case 37U: _LL70: _tmp364=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp161)->f1;_LL71: {
# 1677
struct _tuple17 _tmp321=({struct _tuple17 _tmp324;_tmp324.f1=(env->fenv)->unknown_all;_tmp324.f2=copy_ctxt;_tmp324;});
union Cyc_CfFlowInfo_FlowInfo _tmp322=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp364,& _tmp321);
return({struct _tuple16 _tmp323;_tmp323.f1=_tmp322;_tmp323.f2=_tmp321.f1;_tmp323;});}case 36U: _LL74: _LL75:
# 1682
 goto _LL77;case 25U: _LL76: _LL77:
 goto _LL79;case 39U: _LL78: _LL79:
 goto _LL7B;default: _LL7A: _LL7B:
({void*_tmp325=0U;({struct _dyneither_ptr _tmp760=({const char*_tmp326="anal_Rexp, unexpected exp form";_tag_dyneither(_tmp326,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp760,_tag_dyneither(_tmp325,sizeof(void*),0U));});});}_LLD:;};}
# 1697 "new_control_flow.cyc"
static struct _tuple18 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,int expanded_place_possibly_null,struct Cyc_List_List*path){
# 1706
struct Cyc_CfFlowInfo_FlowEnv*_tmp365=env->fenv;
void*_tmp366=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp367=_tmp366;union Cyc_Absyn_Constraint*_tmp397;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp367)->tag == 5U){_LL1: _tmp397=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp367)->f1).ptr_atts).bounds;_LL2: {
# 1709
union Cyc_CfFlowInfo_FlowInfo _tmp368=f;struct Cyc_Dict_Dict _tmp394;struct Cyc_List_List*_tmp393;if((_tmp368.BottomFL).tag == 1){_LL6: _LL7:
 return({struct _tuple18 _tmp369;_tmp369.f1=f;({union Cyc_CfFlowInfo_AbsLVal _tmp761=Cyc_CfFlowInfo_UnknownL();_tmp369.f2=_tmp761;});_tmp369;});}else{_LL8: _tmp394=((_tmp368.ReachableFL).val).f1;_tmp393=((_tmp368.ReachableFL).val).f2;_LL9: {
# 1713
struct _tuple15 _tmp36A=Cyc_CfFlowInfo_unname_rval(r);struct _tuple15 _tmp36B=_tmp36A;void*_tmp392;struct Cyc_List_List*_tmp391;_LLB: _tmp392=_tmp36B.f1;_tmp391=_tmp36B.f2;_LLC:;
retry: {void*_tmp36C=_tmp392;void*_tmp387;enum Cyc_CfFlowInfo_InitLevel _tmp386;void*_tmp385;struct Cyc_List_List*_tmp384;void*_tmp383;switch(*((int*)_tmp36C)){case 8U: _LLE: _LLF:
# 1716
({void*_tmp36D=0U;({struct _dyneither_ptr _tmp762=({const char*_tmp36E="named location in anal_derefL";_tag_dyneither(_tmp36E,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp762,_tag_dyneither(_tmp36D,sizeof(void*),0U));});});case 1U: _LL10: _LL11:
# 1718
({void*_tmp764=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp36F=_cycalloc(sizeof(*_tmp36F));({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp763=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp370;_tmp370.tag=Cyc_CfFlowInfo_NotZero;_tmp370.f1=_tmp393;_tmp370;});_tmp36F[0]=_tmp763;});_tmp36F;});e->annot=_tmp764;});goto _LLD;case 5U: _LL12: _tmp383=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp36C)->f1;_LL13:
# 1721
 if(expanded_place_possibly_null)
# 1724
({void*_tmp766=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp371=_cycalloc(sizeof(*_tmp371));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp765=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp372;_tmp372.tag=Cyc_CfFlowInfo_UnknownZ;_tmp372.f1=_tmp393;_tmp372;});_tmp371[0]=_tmp765;});_tmp371;});e->annot=_tmp766;});else{
# 1727
void*_tmp373=e->annot;void*_tmp374=_tmp373;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp374)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1F: _LL20:
# 1731
({void*_tmp768=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp375=_cycalloc(sizeof(*_tmp375));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp767=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp376;_tmp376.tag=Cyc_CfFlowInfo_UnknownZ;_tmp376.f1=_tmp393;_tmp376;});_tmp375[0]=_tmp767;});_tmp375;});e->annot=_tmp768;});
goto _LL1E;}else{_LL21: _LL22:
# 1734
({void*_tmp76A=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp377=_cycalloc(sizeof(*_tmp377));({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp769=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp378;_tmp378.tag=Cyc_CfFlowInfo_NotZero;_tmp378.f1=_tmp393;_tmp378;});_tmp377[0]=_tmp769;});_tmp377;});e->annot=_tmp76A;});
goto _LL1E;}_LL1E:;}
# 1739
if(Cyc_Tcutil_is_bound_one(_tmp397))return({struct _tuple18 _tmp379;_tmp379.f1=f;({union Cyc_CfFlowInfo_AbsLVal _tmp76B=Cyc_CfFlowInfo_UnknownL();_tmp379.f2=_tmp76B;});_tmp379;});
goto _LLD;case 4U: _LL14: _tmp385=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp36C)->f1)->root;_tmp384=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp36C)->f1)->path;_LL15:
# 1743
({void*_tmp76D=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp37A=_cycalloc(sizeof(*_tmp37A));({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp76C=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp37B;_tmp37B.tag=Cyc_CfFlowInfo_NotZero;_tmp37B.f1=_tmp393;_tmp37B;});_tmp37A[0]=_tmp76C;});_tmp37A;});e->annot=_tmp76D;});
if(Cyc_Tcutil_is_bound_one(_tmp397))
return({struct _tuple18 _tmp37C;_tmp37C.f1=f;({union Cyc_CfFlowInfo_AbsLVal _tmp76F=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp37D=_cycalloc(sizeof(*_tmp37D));_tmp37D->root=_tmp385;({struct Cyc_List_List*_tmp76E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp384,path);_tmp37D->path=_tmp76E;});_tmp37D;}));_tmp37C.f2=_tmp76F;});_tmp37C;});
goto _LLD;case 0U: _LL16: _LL17:
# 1748
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple18 _tmp37E;({union Cyc_CfFlowInfo_FlowInfo _tmp770=Cyc_CfFlowInfo_BottomFL();_tmp37E.f1=_tmp770;});({union Cyc_CfFlowInfo_AbsLVal _tmp771=Cyc_CfFlowInfo_UnknownL();_tmp37E.f2=_tmp771;});_tmp37E;});case 2U: _LL18: _tmp386=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp36C)->f1;_LL19:
# 1753
 if(Cyc_Tcutil_is_bound_one(_tmp397))
({union Cyc_CfFlowInfo_FlowInfo _tmp772=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp386,_tmp391);f=_tmp772;});
({void*_tmp774=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp37F=_cycalloc(sizeof(*_tmp37F));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp773=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp380;_tmp380.tag=Cyc_CfFlowInfo_UnknownZ;_tmp380.f1=_tmp393;_tmp380;});_tmp37F[0]=_tmp773;});_tmp37F;});e->annot=_tmp774;});
goto _LLD;case 7U: _LL1A: _tmp387=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp36C)->f3;_LL1B:
# 1758
 if(passthrough_consumes){
_tmp392=_tmp387;goto retry;}
# 1761
goto _LL1D;default: _LL1C: _LL1D:
# 1763
({void*_tmp776=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp381=_cycalloc(sizeof(*_tmp381));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp775=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp382;_tmp382.tag=Cyc_CfFlowInfo_UnknownZ;_tmp382.f1=_tmp393;_tmp382;});_tmp381[0]=_tmp775;});_tmp381;});e->annot=_tmp776;});
goto _LLD;}_LLD:;}
# 1766
if(Cyc_CfFlowInfo_initlevel(_tmp365,_tmp394,_tmp392)== Cyc_CfFlowInfo_NoneIL){
struct _tuple15 _tmp388=Cyc_CfFlowInfo_unname_rval(_tmp392);struct _tuple15 _tmp389=_tmp388;void*_tmp38F;_LL24: _tmp38F=_tmp389.f1;_LL25:;{
void*_tmp38A=_tmp38F;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp38A)->tag == 7U){_LL27: _LL28:
# 1770
({void*_tmp38B=0U;({unsigned int _tmp778=e->loc;struct _dyneither_ptr _tmp777=({const char*_tmp38C="attempt to dereference an alias-free that has already been copied";_tag_dyneither(_tmp38C,sizeof(char),66U);});Cyc_CfFlowInfo_aerr(_tmp778,_tmp777,_tag_dyneither(_tmp38B,sizeof(void*),0U));});});
goto _LL26;}else{_LL29: _LL2A:
# 1773
({void*_tmp38D=0U;({unsigned int _tmp77A=e->loc;struct _dyneither_ptr _tmp779=({const char*_tmp38E="dereference of possibly uninitialized pointer";_tag_dyneither(_tmp38E,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp77A,_tmp779,_tag_dyneither(_tmp38D,sizeof(void*),0U));});});
goto _LL26;}_LL26:;};}
# 1777
return({struct _tuple18 _tmp390;_tmp390.f1=f;({union Cyc_CfFlowInfo_AbsLVal _tmp77B=Cyc_CfFlowInfo_UnknownL();_tmp390.f2=_tmp77B;});_tmp390;});}}_LL5:;}}else{_LL3: _LL4:
# 1779
({void*_tmp395=0U;({struct _dyneither_ptr _tmp77C=({const char*_tmp396="left deref of non-pointer-type";_tag_dyneither(_tmp396,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp77C,_tag_dyneither(_tmp395,sizeof(void*),0U));});});}_LL0:;}
# 1785
static struct Cyc_CfFlowInfo_Place*Cyc_NewControlFlow_make_expanded_place(struct Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*path){
struct Cyc_CfFlowInfo_Place*_tmp398=p;void*_tmp39E;struct Cyc_List_List*_tmp39D;_LL1: _tmp39E=_tmp398->root;_tmp39D=_tmp398->path;_LL2:;
({struct Cyc_List_List*_tmp77F=({struct Cyc_List_List*_tmp399=_cycalloc(sizeof(*_tmp399));({void*_tmp77E=(void*)({struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*_tmp39A=_cycalloc_atomic(sizeof(*_tmp39A));({struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct _tmp77D=({struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct _tmp39B;_tmp39B.tag=1U;_tmp39B;});_tmp39A[0]=_tmp77D;});_tmp39A;});_tmp399->hd=_tmp77E;});_tmp399->tl=path;_tmp399;});path=_tmp77F;});
return({struct Cyc_CfFlowInfo_Place*_tmp39C=_cycalloc(sizeof(*_tmp39C));_tmp39C->root=_tmp39E;({struct Cyc_List_List*_tmp780=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp39D,path);_tmp39C->path=_tmp780;});_tmp39C;});}
# 1795
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*path){
# 1799
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp39F=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp3A0=inflow;struct Cyc_Dict_Dict _tmp3A3;struct Cyc_List_List*_tmp3A2;if((_tmp3A0.BottomFL).tag == 1){_LL1: _LL2:
 return({struct _tuple18 _tmp3A1;({union Cyc_CfFlowInfo_FlowInfo _tmp781=Cyc_CfFlowInfo_BottomFL();_tmp3A1.f1=_tmp781;});({union Cyc_CfFlowInfo_AbsLVal _tmp782=Cyc_CfFlowInfo_UnknownL();_tmp3A1.f2=_tmp782;});_tmp3A1;});}else{_LL3: _tmp3A3=((_tmp3A0.ReachableFL).val).f1;_tmp3A2=((_tmp3A0.ReachableFL).val).f2;_LL4:
# 1804
 d=_tmp3A3;}_LL0:;}{
# 1808
void*_tmp3A4=e->r;void*_tmp3A5=_tmp3A4;struct Cyc_Absyn_Exp*_tmp40C;struct _dyneither_ptr*_tmp40B;struct Cyc_Absyn_Exp*_tmp40A;struct Cyc_Absyn_Exp*_tmp409;struct Cyc_Absyn_Exp*_tmp408;struct Cyc_Absyn_Exp*_tmp407;struct _dyneither_ptr*_tmp406;struct Cyc_Absyn_Vardecl*_tmp405;struct Cyc_Absyn_Vardecl*_tmp404;struct Cyc_Absyn_Vardecl*_tmp403;struct Cyc_Absyn_Vardecl*_tmp402;struct Cyc_Absyn_Exp*_tmp401;struct Cyc_Absyn_Exp*_tmp400;struct Cyc_Absyn_Exp*_tmp3FF;switch(*((int*)_tmp3A5)){case 14U: _LL6: _tmp3FF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3A5)->f2;_LL7:
 _tmp400=_tmp3FF;goto _LL9;case 12U: _LL8: _tmp400=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp3A5)->f1;_LL9:
 _tmp401=_tmp400;goto _LLB;case 13U: _LLA: _tmp401=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp3A5)->f1;_LLB:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp401,path);case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A5)->f1)){case 3U: _LLC: _tmp402=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A5)->f1)->f1;_LLD:
# 1813
 _tmp403=_tmp402;goto _LLF;case 4U: _LLE: _tmp403=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A5)->f1)->f1;_LLF:
 _tmp404=_tmp403;goto _LL11;case 5U: _LL10: _tmp404=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A5)->f1)->f1;_LL11:
# 1816
 return({struct _tuple18 _tmp3A6;_tmp3A6.f1=inflow;({union Cyc_CfFlowInfo_AbsLVal _tmp785=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp3A7=_cycalloc(sizeof(*_tmp3A7));({void*_tmp784=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp3A8=_cycalloc(sizeof(*_tmp3A8));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp783=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp3A9;_tmp3A9.tag=0U;_tmp3A9.f1=_tmp404;_tmp3A9;});_tmp3A8[0]=_tmp783;});_tmp3A8;});_tmp3A7->root=_tmp784;});_tmp3A7->path=path;_tmp3A7;}));_tmp3A6.f2=_tmp785;});_tmp3A6;});case 1U: _LL12: _tmp405=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A5)->f1)->f1;_LL13:
# 1818
 return({struct _tuple18 _tmp3AA;_tmp3AA.f1=inflow;({union Cyc_CfFlowInfo_AbsLVal _tmp786=Cyc_CfFlowInfo_UnknownL();_tmp3AA.f2=_tmp786;});_tmp3AA;});default: goto _LL1C;}case 22U: _LL14: _tmp407=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3A5)->f1;_tmp406=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3A5)->f2;_LL15:
# 1821
{void*_tmp3AB=Cyc_Tcutil_compress((void*)_check_null(_tmp407->topt));void*_tmp3AC=_tmp3AB;void*_tmp3B2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AC)->tag == 5U){_LL1F: _tmp3B2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AC)->f1).elt_typ;_LL20:
# 1823
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp3B2)){
Cyc_NewControlFlow_check_union_requires(_tmp407->loc,_tmp3B2,_tmp406,inflow);
({struct Cyc_List_List*_tmp78A=({struct Cyc_List_List*_tmp3AD=_cycalloc(sizeof(*_tmp3AD));({void*_tmp789=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp3AE=_cycalloc_atomic(sizeof(*_tmp3AE));({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp788=({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp3AF;_tmp3AF.tag=0U;({int _tmp787=Cyc_CfFlowInfo_get_field_index(_tmp3B2,_tmp406);_tmp3AF.f1=_tmp787;});_tmp3AF;});_tmp3AE[0]=_tmp788;});_tmp3AE;});_tmp3AD->hd=_tmp789;});_tmp3AD->tl=path;_tmp3AD;});path=_tmp78A;});}
# 1827
goto _LL1E;}else{_LL21: _LL22:
({void*_tmp3B0=0U;({struct _dyneither_ptr _tmp78B=({const char*_tmp3B1="anal_Lexp: AggrArrow ptr";_tag_dyneither(_tmp3B1,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp78B,_tag_dyneither(_tmp3B0,sizeof(void*),0U));});});}_LL1E:;}
# 1830
_tmp408=_tmp407;goto _LL17;case 20U: _LL16: _tmp408=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3A5)->f1;_LL17:
# 1834
 if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp408->topt),1)){
# 1836
struct _tuple18 _tmp3B3=Cyc_NewControlFlow_anal_Lexp(env,inflow,expand_unique,passthrough_consumes,_tmp408);struct _tuple18 _tmp3B4=_tmp3B3;union Cyc_CfFlowInfo_FlowInfo _tmp3D9;union Cyc_CfFlowInfo_AbsLVal _tmp3D8;_LL24: _tmp3D9=_tmp3B4.f1;_tmp3D8=_tmp3B4.f2;_LL25:;{
struct _tuple18 _tmp3B5=({struct _tuple18 _tmp3D7;_tmp3D7.f1=_tmp3D9;_tmp3D7.f2=_tmp3D8;_tmp3D7;});struct _tuple18 _tmp3B6=_tmp3B5;struct Cyc_Dict_Dict _tmp3D6;struct Cyc_List_List*_tmp3D5;struct Cyc_CfFlowInfo_Place*_tmp3D4;if(((_tmp3B6.f1).ReachableFL).tag == 2){if(((_tmp3B6.f2).PlaceL).tag == 1){_LL27: _tmp3D6=(((_tmp3B6.f1).ReachableFL).val).f1;_tmp3D5=(((_tmp3B6.f1).ReachableFL).val).f2;_tmp3D4=((_tmp3B6.f2).PlaceL).val;_LL28: {
# 1839
void*_tmp3B7=Cyc_CfFlowInfo_lookup_place(_tmp3D6,_tmp3D4);
struct _tuple15 _tmp3B8=Cyc_CfFlowInfo_unname_rval(_tmp3B7);struct _tuple15 _tmp3B9=_tmp3B8;void*_tmp3D3;struct Cyc_List_List*_tmp3D2;_LL2C: _tmp3D3=_tmp3B9.f1;_tmp3D2=_tmp3B9.f2;_LL2D:;
if(expand_unique){
int _tmp3BA=1;
void*_tmp3BB=_tmp3D3;struct Cyc_Absyn_Vardecl*_tmp3CC;void*_tmp3CB;switch(*((int*)_tmp3BB)){case 8U: _LL2F: _tmp3CC=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3BB)->f1;_tmp3CB=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3BB)->f2;_LL30:
# 1845
({void*_tmp3BC=0U;({struct _dyneither_ptr _tmp78C=({const char*_tmp3BD="bad named location in anal_Lexp:deref";_tag_dyneither(_tmp3BD,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp78C,_tag_dyneither(_tmp3BC,sizeof(void*),0U));});});case 7U: switch(*((int*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3BB)->f3)){case 4U: _LL31: _LL32:
 goto _LL34;case 5U: _LL35: _LL36:
# 1849
 goto _LL38;default: goto _LL3B;}case 4U: _LL33: _LL34:
# 1848
 return Cyc_NewControlFlow_anal_derefL(env,_tmp3D9,_tmp408,_tmp3D9,_tmp3B7,passthrough_consumes,0,path);case 5U: _LL37: _LL38: {
# 1853
struct _tuple18 _tmp3BE=Cyc_NewControlFlow_anal_derefL(env,_tmp3D9,_tmp408,_tmp3D9,_tmp3B7,passthrough_consumes,0,path);struct _tuple18 _tmp3BF=_tmp3BE;union Cyc_CfFlowInfo_FlowInfo _tmp3C1;_LL3E: _tmp3C1=_tmp3BF.f1;_LL3F:;
return({struct _tuple18 _tmp3C0;_tmp3C0.f1=_tmp3C1;({union Cyc_CfFlowInfo_AbsLVal _tmp78D=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp3D4,path));_tmp3C0.f2=_tmp78D;});_tmp3C0;});}case 1U: _LL39: _LL3A:
# 1856
 _tmp3BA=0;goto _LL3C;default: _LL3B: _LL3C: {
# 1863
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp39F,_tmp3D6,_tmp3D3);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp39F->unknown_all: _tmp39F->unknown_none;
void*res=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp3C9=_cycalloc(sizeof(*_tmp3C9));({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp791=({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp3CA;_tmp3CA.tag=5U;({void*_tmp790=({struct Cyc_CfFlowInfo_FlowEnv*_tmp78F=_tmp39F;void*_tmp78E=Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp408->topt));Cyc_CfFlowInfo_typ_to_absrval(_tmp78F,_tmp78E,0,leaf);});_tmp3CA.f1=_tmp790;});_tmp3CA;});_tmp3C9[0]=_tmp791;});_tmp3C9;});
for(0;_tmp3D2 != 0;_tmp3D2=_tmp3D2->tl){
({void*_tmp793=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp792=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp3C3;_tmp3C3.tag=8U;_tmp3C3.f1=(struct Cyc_Absyn_Vardecl*)_tmp3D2->hd;_tmp3C3.f2=res;_tmp3C3;});_tmp3C2[0]=_tmp792;});_tmp3C2;});res=_tmp793;});}
({struct Cyc_Dict_Dict _tmp794=Cyc_CfFlowInfo_assign_place(_tmp39F,e->loc,_tmp3D6,_tmp3D4,res);_tmp3D6=_tmp794;});{
union Cyc_CfFlowInfo_FlowInfo _tmp3C4=Cyc_CfFlowInfo_ReachableFL(_tmp3D6,_tmp3D5);
# 1876
struct _tuple18 _tmp3C5=Cyc_NewControlFlow_anal_derefL(env,_tmp3C4,_tmp408,_tmp3C4,res,passthrough_consumes,_tmp3BA,path);struct _tuple18 _tmp3C6=_tmp3C5;union Cyc_CfFlowInfo_FlowInfo _tmp3C8;_LL41: _tmp3C8=_tmp3C6.f1;_LL42:;
# 1878
return({struct _tuple18 _tmp3C7;_tmp3C7.f1=_tmp3C8;({union Cyc_CfFlowInfo_AbsLVal _tmp795=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp3D4,path));_tmp3C7.f2=_tmp795;});_tmp3C7;});};}}_LL2E:;}else{
# 1882
void*_tmp3CD=_tmp3D3;switch(*((int*)_tmp3CD)){case 7U: if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3CD)->f3)->tag == 5U){_LL44: _LL45:
 goto _LL47;}else{goto _LL48;}case 5U: _LL46: _LL47: {
# 1887
struct _tuple18 _tmp3CE=Cyc_NewControlFlow_anal_derefL(env,_tmp3D9,_tmp408,_tmp3D9,_tmp3B7,passthrough_consumes,0,path);struct _tuple18 _tmp3CF=_tmp3CE;union Cyc_CfFlowInfo_FlowInfo _tmp3D1;_LL4B: _tmp3D1=_tmp3CF.f1;_LL4C:;
return({struct _tuple18 _tmp3D0;_tmp3D0.f1=_tmp3D1;({union Cyc_CfFlowInfo_AbsLVal _tmp796=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp3D4,path));_tmp3D0.f2=_tmp796;});_tmp3D0;});}default: _LL48: _LL49:
# 1891
 goto _LL43;}_LL43:;}
# 1894
goto _LL26;}}else{goto _LL29;}}else{_LL29: _LL2A:
 goto _LL26;}_LL26:;};}{
# 1899
struct _tuple16 _tmp3DA=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp408);struct _tuple16 _tmp3DB=_tmp3DA;union Cyc_CfFlowInfo_FlowInfo _tmp3DD;void*_tmp3DC;_LL4E: _tmp3DD=_tmp3DB.f1;_tmp3DC=_tmp3DB.f2;_LL4F:;
# 1901
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp408,_tmp3DD,_tmp3DC,passthrough_consumes,0,path);};case 23U: _LL18: _tmp40A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3A5)->f1;_tmp409=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3A5)->f2;_LL19: {
# 1904
void*_tmp3DE=Cyc_Tcutil_compress((void*)_check_null(_tmp40A->topt));void*_tmp3DF=_tmp3DE;switch(*((int*)_tmp3DF)){case 10U: _LL51: _LL52: {
# 1906
unsigned int _tmp3E0=(Cyc_Evexp_eval_const_uint_exp(_tmp409)).f1;
return({struct Cyc_NewControlFlow_AnalEnv*_tmp79D=env;union Cyc_CfFlowInfo_FlowInfo _tmp79C=inflow;int _tmp79B=expand_unique;int _tmp79A=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp799=_tmp40A;Cyc_NewControlFlow_anal_Lexp_rec(_tmp79D,_tmp79C,_tmp79B,_tmp79A,_tmp799,({struct Cyc_List_List*_tmp3E1=_cycalloc(sizeof(*_tmp3E1));({void*_tmp798=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp3E2=_cycalloc_atomic(sizeof(*_tmp3E2));({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp797=({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp3E3;_tmp3E3.tag=0U;_tmp3E3.f1=(int)_tmp3E0;_tmp3E3;});_tmp3E2[0]=_tmp797;});_tmp3E2;});_tmp3E1->hd=_tmp798;});_tmp3E1->tl=path;_tmp3E1;}));});}case 5U: _LL53: _LL54: {
# 1910
struct _tuple23 _tmp3E4=({struct Cyc_NewControlFlow_AnalEnv*_tmp79F=env;union Cyc_CfFlowInfo_FlowInfo _tmp79E=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp79F,_tmp79E,({struct Cyc_Absyn_Exp*_tmp3F6[2U];_tmp3F6[1U]=_tmp409;_tmp3F6[0U]=_tmp40A;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3F6,sizeof(struct Cyc_Absyn_Exp*),2U));}),0,1);});struct _tuple23 _tmp3E5=_tmp3E4;union Cyc_CfFlowInfo_FlowInfo _tmp3F5;struct Cyc_List_List*_tmp3F4;_LL58: _tmp3F5=_tmp3E5.f1;_tmp3F4=_tmp3E5.f2;_LL59:;{
union Cyc_CfFlowInfo_FlowInfo _tmp3E6=_tmp3F5;
{union Cyc_CfFlowInfo_FlowInfo _tmp3E7=_tmp3F5;struct Cyc_Dict_Dict _tmp3EC;struct Cyc_List_List*_tmp3EB;if((_tmp3E7.ReachableFL).tag == 2){_LL5B: _tmp3EC=((_tmp3E7.ReachableFL).val).f1;_tmp3EB=((_tmp3E7.ReachableFL).val).f2;_LL5C:
# 1914
 if(Cyc_CfFlowInfo_initlevel(_tmp39F,_tmp3EC,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp3F4))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp3E8=0U;({unsigned int _tmp7A1=_tmp409->loc;struct _dyneither_ptr _tmp7A0=({const char*_tmp3E9="expression may not be initialized";_tag_dyneither(_tmp3E9,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp7A1,_tmp7A0,_tag_dyneither(_tmp3E8,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp3EA=Cyc_NewControlFlow_add_subscript_reln(_tmp3EB,_tmp40A,_tmp409);
if(_tmp3EB != _tmp3EA)
({union Cyc_CfFlowInfo_FlowInfo _tmp7A2=Cyc_CfFlowInfo_ReachableFL(_tmp3EC,_tmp3EA);_tmp3E6=_tmp7A2;});
goto _LL5A;};}else{_LL5D: _LL5E:
 goto _LL5A;}_LL5A:;}{
# 1923
struct _tuple18 _tmp3ED=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp40A,_tmp3F5,(void*)((struct Cyc_List_List*)_check_null(_tmp3F4))->hd,passthrough_consumes,0,path);struct _tuple18 _tmp3EE=_tmp3ED;union Cyc_CfFlowInfo_FlowInfo _tmp3F3;union Cyc_CfFlowInfo_AbsLVal _tmp3F2;_LL60: _tmp3F3=_tmp3EE.f1;_tmp3F2=_tmp3EE.f2;_LL61:;{
union Cyc_CfFlowInfo_FlowInfo _tmp3EF=_tmp3F3;if((_tmp3EF.BottomFL).tag == 1){_LL63: _LL64:
 return({struct _tuple18 _tmp3F0;_tmp3F0.f1=_tmp3F3;_tmp3F0.f2=_tmp3F2;_tmp3F0;});}else{_LL65: _LL66:
 return({struct _tuple18 _tmp3F1;_tmp3F1.f1=_tmp3E6;_tmp3F1.f2=_tmp3F2;_tmp3F1;});}_LL62:;};};};}default: _LL55: _LL56:
# 1928
({void*_tmp3F7=0U;({struct _dyneither_ptr _tmp7A3=({const char*_tmp3F8="anal_Lexp: Subscript -- bad type";_tag_dyneither(_tmp3F8,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7A3,_tag_dyneither(_tmp3F7,sizeof(void*),0U));});});}_LL50:;}case 21U: _LL1A: _tmp40C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3A5)->f1;_tmp40B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3A5)->f2;_LL1B: {
# 1932
void*_tmp3F9=(void*)_check_null(_tmp40C->topt);
if(Cyc_Absyn_is_require_union_type(_tmp3F9))
Cyc_NewControlFlow_check_union_requires(_tmp40C->loc,_tmp3F9,_tmp40B,inflow);
# 1936
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp3F9))
return({struct _tuple18 _tmp3FA;_tmp3FA.f1=inflow;({union Cyc_CfFlowInfo_AbsLVal _tmp7A4=Cyc_CfFlowInfo_UnknownL();_tmp3FA.f2=_tmp7A4;});_tmp3FA;});
# 1939
return({struct Cyc_NewControlFlow_AnalEnv*_tmp7AC=env;union Cyc_CfFlowInfo_FlowInfo _tmp7AB=inflow;int _tmp7AA=expand_unique;int _tmp7A9=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp7A8=_tmp40C;Cyc_NewControlFlow_anal_Lexp_rec(_tmp7AC,_tmp7AB,_tmp7AA,_tmp7A9,_tmp7A8,({struct Cyc_List_List*_tmp3FB=_cycalloc(sizeof(*_tmp3FB));({void*_tmp7A7=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp3FC=_cycalloc_atomic(sizeof(*_tmp3FC));({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp7A6=({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp3FD;_tmp3FD.tag=0U;({int _tmp7A5=
Cyc_CfFlowInfo_get_field_index(_tmp3F9,_tmp40B);_tmp3FD.f1=_tmp7A5;});_tmp3FD;});_tmp3FC[0]=_tmp7A6;});_tmp3FC;});_tmp3FB->hd=_tmp7A7;});_tmp3FB->tl=path;_tmp3FB;}));});}default: _LL1C: _LL1D:
# 1943
 return({struct _tuple18 _tmp3FE;({union Cyc_CfFlowInfo_FlowInfo _tmp7AD=Cyc_CfFlowInfo_BottomFL();_tmp3FE.f1=_tmp7AD;});({union Cyc_CfFlowInfo_AbsLVal _tmp7AE=Cyc_CfFlowInfo_UnknownL();_tmp3FE.f2=_tmp7AE;});_tmp3FE;});}_LL5:;};}
# 1947
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1951
struct _tuple18 _tmp40D=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);struct _tuple18 _tmp40E=_tmp40D;union Cyc_CfFlowInfo_FlowInfo _tmp411;union Cyc_CfFlowInfo_AbsLVal _tmp410;_LL1: _tmp411=_tmp40E.f1;_tmp410=_tmp40E.f2;_LL2:;
return({struct _tuple18 _tmp40F;_tmp40F.f1=_tmp411;_tmp40F.f2=_tmp410;_tmp40F;});}
# 1958
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1961
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd,1) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
struct _tuple18 _tmp412=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple18 _tmp413=_tmp412;union Cyc_CfFlowInfo_FlowInfo _tmp414;_LL1: _tmp414=_tmp413.f1;_LL2:;
inflow=_tmp414;}}}
# 1969
return inflow;}
# 1975
static struct _tuple19 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1977
struct Cyc_CfFlowInfo_FlowEnv*_tmp415=env->fenv;
# 1980
struct _tuple16 _tmp416=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);struct _tuple16 _tmp417=_tmp416;union Cyc_CfFlowInfo_FlowInfo _tmp45F;void*_tmp45E;_LL1: _tmp45F=_tmp417.f1;_tmp45E=_tmp417.f2;_LL2:;{
struct _tuple16 _tmp418=Cyc_NewControlFlow_anal_Rexp(env,0,_tmp45F,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);struct _tuple16 _tmp419=_tmp418;union Cyc_CfFlowInfo_FlowInfo _tmp45D;void*_tmp45C;_LL4: _tmp45D=_tmp419.f1;_tmp45C=_tmp419.f2;_LL5:;{
union Cyc_CfFlowInfo_FlowInfo _tmp41A=_tmp45D;
# 1985
union Cyc_CfFlowInfo_FlowInfo _tmp41B=_tmp41A;struct Cyc_Dict_Dict _tmp45B;struct Cyc_List_List*_tmp45A;if((_tmp41B.BottomFL).tag == 1){_LL7: _LL8:
 return({struct _tuple19 _tmp41C;_tmp41C.f1=_tmp41A;_tmp41C.f2=_tmp41A;_tmp41C;});}else{_LL9: _tmp45B=((_tmp41B.ReachableFL).val).f1;_tmp45A=((_tmp41B.ReachableFL).val).f2;_LLA: {
# 1988
struct Cyc_Absyn_Exp*_tmp41D=(struct Cyc_Absyn_Exp*)es->hd;
struct Cyc_Absyn_Exp*_tmp41E=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp45B,_tmp45E)== Cyc_CfFlowInfo_NoneIL  && !Cyc_CfFlowInfo_is_init_pointer(_tmp45E))
({void*_tmp41F=0U;({unsigned int _tmp7B0=((struct Cyc_Absyn_Exp*)es->hd)->loc;struct _dyneither_ptr _tmp7AF=({const char*_tmp420="expression may not be initialized";_tag_dyneither(_tmp420,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp7B0,_tmp7AF,_tag_dyneither(_tmp41F,sizeof(void*),0U));});});
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp45B,_tmp45C)== Cyc_CfFlowInfo_NoneIL  && !Cyc_CfFlowInfo_is_init_pointer(_tmp45E))
({void*_tmp421=0U;({unsigned int _tmp7B2=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc;struct _dyneither_ptr _tmp7B1=({const char*_tmp422="expression may not be initialized";_tag_dyneither(_tmp422,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp7B2,_tmp7B1,_tag_dyneither(_tmp421,sizeof(void*),0U));});});{
# 1995
union Cyc_CfFlowInfo_FlowInfo _tmp423=_tmp41A;
union Cyc_CfFlowInfo_FlowInfo _tmp424=_tmp41A;
# 2000
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct _tuple15 _tmp425=Cyc_CfFlowInfo_unname_rval(_tmp45E);struct _tuple15 _tmp426=_tmp425;void*_tmp43F;struct Cyc_List_List*_tmp43E;_LLC: _tmp43F=_tmp426.f1;_tmp43E=_tmp426.f2;_LLD:;{
struct _tuple15 _tmp427=Cyc_CfFlowInfo_unname_rval(_tmp45C);struct _tuple15 _tmp428=_tmp427;void*_tmp43D;struct Cyc_List_List*_tmp43C;_LLF: _tmp43D=_tmp428.f1;_tmp43C=_tmp428.f2;_LL10:;{
struct _tuple0 _tmp429=({struct _tuple0 _tmp43B;_tmp43B.f1=_tmp43F;_tmp43B.f2=_tmp43D;_tmp43B;});struct _tuple0 _tmp42A=_tmp429;enum Cyc_CfFlowInfo_InitLevel _tmp43A;enum Cyc_CfFlowInfo_InitLevel _tmp439;switch(*((int*)_tmp42A.f1)){case 2U: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp42A.f2)->tag == 0U){_LL12: _tmp439=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp42A.f1)->f1;_LL13: {
# 2005
struct _tuple19 _tmp42B=Cyc_NewControlFlow_splitzero(env,inflow,_tmp41A,_tmp41D,_tmp439,_tmp43E);struct _tuple19 _tmp42C=_tmp42B;union Cyc_CfFlowInfo_FlowInfo _tmp431;union Cyc_CfFlowInfo_FlowInfo _tmp430;_LL23: _tmp431=_tmp42C.f1;_tmp430=_tmp42C.f2;_LL24:;
{enum Cyc_Absyn_Primop _tmp42D=p;switch(_tmp42D){case Cyc_Absyn_Eq: _LL26: _LL27:
 _tmp423=_tmp430;_tmp424=_tmp431;goto _LL25;case Cyc_Absyn_Neq: _LL28: _LL29:
 _tmp423=_tmp431;_tmp424=_tmp430;goto _LL25;default: _LL2A: _LL2B:
({void*_tmp42E=0U;({struct _dyneither_ptr _tmp7B3=({const char*_tmp42F="anal_test, zero-split";_tag_dyneither(_tmp42F,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7B3,_tag_dyneither(_tmp42E,sizeof(void*),0U));});});}_LL25:;}
# 2011
goto _LL11;}}else{goto _LL20;}case 0U: switch(*((int*)_tmp42A.f2)){case 2U: _LL14: _tmp43A=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp42A.f2)->f1;_LL15: {
# 2013
struct _tuple19 _tmp432=Cyc_NewControlFlow_splitzero(env,_tmp45D,_tmp41A,_tmp41E,_tmp43A,_tmp43C);struct _tuple19 _tmp433=_tmp432;union Cyc_CfFlowInfo_FlowInfo _tmp438;union Cyc_CfFlowInfo_FlowInfo _tmp437;_LL2D: _tmp438=_tmp433.f1;_tmp437=_tmp433.f2;_LL2E:;
{enum Cyc_Absyn_Primop _tmp434=p;switch(_tmp434){case Cyc_Absyn_Eq: _LL30: _LL31:
 _tmp423=_tmp437;_tmp424=_tmp438;goto _LL2F;case Cyc_Absyn_Neq: _LL32: _LL33:
 _tmp423=_tmp438;_tmp424=_tmp437;goto _LL2F;default: _LL34: _LL35:
({void*_tmp435=0U;({struct _dyneither_ptr _tmp7B4=({const char*_tmp436="anal_test, zero-split";_tag_dyneither(_tmp436,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7B4,_tag_dyneither(_tmp435,sizeof(void*),0U));});});}_LL2F:;}
# 2019
goto _LL11;}case 0U: _LL16: _LL17:
# 2021
 if(p == Cyc_Absyn_Eq)({union Cyc_CfFlowInfo_FlowInfo _tmp7B5=Cyc_CfFlowInfo_BottomFL();_tmp424=_tmp7B5;});else{
({union Cyc_CfFlowInfo_FlowInfo _tmp7B6=Cyc_CfFlowInfo_BottomFL();_tmp423=_tmp7B6;});}
goto _LL11;case 1U: _LL18: _LL19:
 goto _LL1B;case 4U: _LL1A: _LL1B:
 goto _LL1D;default: goto _LL20;}case 1U: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp42A.f2)->tag == 0U){_LL1C: _LL1D:
 goto _LL1F;}else{goto _LL20;}case 4U: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp42A.f2)->tag == 0U){_LL1E: _LL1F:
# 2028
 if(p == Cyc_Absyn_Neq)({union Cyc_CfFlowInfo_FlowInfo _tmp7B7=Cyc_CfFlowInfo_BottomFL();_tmp424=_tmp7B7;});else{
({union Cyc_CfFlowInfo_FlowInfo _tmp7B8=Cyc_CfFlowInfo_BottomFL();_tmp423=_tmp7B8;});}
goto _LL11;}else{goto _LL20;}default: _LL20: _LL21:
 goto _LL11;}_LL11:;};};}
# 2039
{struct _tuple0 _tmp440=({struct _tuple0 _tmp443;({void*_tmp7B9=Cyc_Tcutil_compress((void*)_check_null(_tmp41D->topt));_tmp443.f1=_tmp7B9;});({void*_tmp7BA=
Cyc_Tcutil_compress((void*)_check_null(_tmp41E->topt));_tmp443.f2=_tmp7BA;});_tmp443;});
# 2039
struct _tuple0 _tmp441=_tmp440;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp441.f1)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp441.f1)->f1 == Cyc_Absyn_Unsigned){_LL37: _LL38:
# 2041
 goto _LL3A;}else{switch(*((int*)_tmp441.f2)){case 6U: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp441.f2)->f1 == Cyc_Absyn_Unsigned)goto _LL39;else{goto _LL3F;}case 19U: goto _LL3D;default: goto _LL3F;}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp441.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp441.f2)->f1 == Cyc_Absyn_Unsigned){_LL39: _LL3A:
 goto _LL3C;}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp441.f1)->tag == 19U)goto _LL3B;else{goto _LL3F;}}}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp441.f1)->tag == 19U){_LL3B: _LL3C:
 goto _LL3E;}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp441.f2)->tag == 19U){_LL3D: _LL3E:
 goto _LL36;}else{_LL3F: _LL40:
 return({struct _tuple19 _tmp442;_tmp442.f1=_tmp423;_tmp442.f2=_tmp424;_tmp442;});}}}}_LL36:;}
# 2049
{enum Cyc_Absyn_Primop _tmp444=p;switch(_tmp444){case Cyc_Absyn_Eq: _LL42: _LL43:
 goto _LL45;case Cyc_Absyn_Neq: _LL44: _LL45: goto _LL47;case Cyc_Absyn_Gt: _LL46: _LL47: goto _LL49;case Cyc_Absyn_Gte: _LL48: _LL49: goto _LL4B;case Cyc_Absyn_Lt: _LL4A: _LL4B: goto _LL4D;case Cyc_Absyn_Lte: _LL4C: _LL4D: goto _LL41;default: _LL4E: _LL4F:
 return({struct _tuple19 _tmp445;_tmp445.f1=_tmp423;_tmp445.f2=_tmp424;_tmp445;});}_LL41:;}{
# 2054
struct _RegionHandle*_tmp446=Cyc_Core_heap_region;
struct _tuple13 _tmp447=Cyc_Relations_primop2relation(_tmp41D,p,_tmp41E);struct _tuple13 _tmp448=_tmp447;struct Cyc_Absyn_Exp*_tmp459;enum Cyc_Relations_Relation _tmp458;struct Cyc_Absyn_Exp*_tmp457;_LL51: _tmp459=_tmp448.f1;_tmp458=_tmp448.f2;_tmp457=_tmp448.f3;_LL52:;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
# 2059
if(Cyc_Relations_exp2relnop(_tmp459,& n1) && Cyc_Relations_exp2relnop(_tmp457,& n2)){
# 2061
struct Cyc_List_List*_tmp449=Cyc_Relations_add_relation(_tmp446,n1,_tmp458,n2,_tmp45A);
# 2065
struct Cyc_List_List*_tmp44A=({struct _RegionHandle*_tmp7BE=_tmp446;union Cyc_Relations_RelnOp _tmp7BD=n2;enum Cyc_Relations_Relation _tmp7BC=Cyc_Relations_flip_relation(_tmp458);union Cyc_Relations_RelnOp _tmp7BB=n1;Cyc_Relations_add_relation(_tmp7BE,_tmp7BD,_tmp7BC,_tmp7BB,_tmp45A);});
struct _tuple19 _tmp44B=({struct _tuple19 _tmp455;_tmp455.f1=_tmp423;_tmp455.f2=_tmp424;_tmp455;});struct _tuple19 _tmp44C=_tmp44B;struct Cyc_Dict_Dict _tmp454;struct Cyc_Dict_Dict _tmp453;struct Cyc_Dict_Dict _tmp452;struct Cyc_Dict_Dict _tmp451;if(((_tmp44C.f1).ReachableFL).tag == 2){if(((_tmp44C.f2).ReachableFL).tag == 2){_LL54: _tmp452=(((_tmp44C.f1).ReachableFL).val).f1;_tmp451=(((_tmp44C.f2).ReachableFL).val).f1;_LL55:
# 2068
 return({struct _tuple19 _tmp44D;({union Cyc_CfFlowInfo_FlowInfo _tmp7BF=Cyc_CfFlowInfo_ReachableFL(_tmp452,_tmp449);_tmp44D.f1=_tmp7BF;});({union Cyc_CfFlowInfo_FlowInfo _tmp7C0=Cyc_CfFlowInfo_ReachableFL(_tmp451,_tmp44A);_tmp44D.f2=_tmp7C0;});_tmp44D;});}else{_LL58: _tmp453=(((_tmp44C.f1).ReachableFL).val).f1;_LL59:
# 2072
 return({struct _tuple19 _tmp44F;({union Cyc_CfFlowInfo_FlowInfo _tmp7C1=Cyc_CfFlowInfo_ReachableFL(_tmp453,_tmp449);_tmp44F.f1=_tmp7C1;});_tmp44F.f2=_tmp424;_tmp44F;});}}else{if(((_tmp44C.f2).ReachableFL).tag == 2){_LL56: _tmp454=(((_tmp44C.f2).ReachableFL).val).f1;_LL57:
# 2070
 return({struct _tuple19 _tmp44E;_tmp44E.f1=_tmp423;({union Cyc_CfFlowInfo_FlowInfo _tmp7C2=Cyc_CfFlowInfo_ReachableFL(_tmp454,_tmp44A);_tmp44E.f2=_tmp7C2;});_tmp44E;});}else{_LL5A: _LL5B:
# 2074
 return({struct _tuple19 _tmp450;_tmp450.f1=_tmp423;_tmp450.f2=_tmp424;_tmp450;});}}_LL53:;}else{
# 2077
return({struct _tuple19 _tmp456;_tmp456.f1=_tmp423;_tmp456.f2=_tmp424;_tmp456;});}};};};}}_LL6:;};};}
# 2083
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
struct Cyc_CfFlowInfo_FlowEnv*_tmp460=env->fenv;
void*_tmp461=e->r;void*_tmp462=_tmp461;enum Cyc_Absyn_Primop _tmp4AF;struct Cyc_List_List*_tmp4AE;struct Cyc_Absyn_Exp*_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AC;struct Cyc_Absyn_Exp*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AA;struct Cyc_Absyn_Exp*_tmp4A9;struct Cyc_Absyn_Exp*_tmp4A8;struct Cyc_Absyn_Exp*_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A6;struct Cyc_Absyn_Exp*_tmp4A5;struct Cyc_Absyn_Exp*_tmp4A4;switch(*((int*)_tmp462)){case 6U: _LL1: _tmp4A6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp462)->f1;_tmp4A5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp462)->f2;_tmp4A4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp462)->f3;_LL2: {
# 2087
struct _tuple19 _tmp463=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4A6);struct _tuple19 _tmp464=_tmp463;union Cyc_CfFlowInfo_FlowInfo _tmp46F;union Cyc_CfFlowInfo_FlowInfo _tmp46E;_LL10: _tmp46F=_tmp464.f1;_tmp46E=_tmp464.f2;_LL11:;{
struct _tuple19 _tmp465=Cyc_NewControlFlow_anal_test(env,_tmp46F,_tmp4A5);struct _tuple19 _tmp466=_tmp465;union Cyc_CfFlowInfo_FlowInfo _tmp46D;union Cyc_CfFlowInfo_FlowInfo _tmp46C;_LL13: _tmp46D=_tmp466.f1;_tmp46C=_tmp466.f2;_LL14:;{
struct _tuple19 _tmp467=Cyc_NewControlFlow_anal_test(env,_tmp46E,_tmp4A4);struct _tuple19 _tmp468=_tmp467;union Cyc_CfFlowInfo_FlowInfo _tmp46B;union Cyc_CfFlowInfo_FlowInfo _tmp46A;_LL16: _tmp46B=_tmp468.f1;_tmp46A=_tmp468.f2;_LL17:;
return({struct _tuple19 _tmp469;({union Cyc_CfFlowInfo_FlowInfo _tmp7C3=Cyc_CfFlowInfo_join_flow(_tmp460,_tmp46D,_tmp46B);_tmp469.f1=_tmp7C3;});({union Cyc_CfFlowInfo_FlowInfo _tmp7C4=
Cyc_CfFlowInfo_join_flow(_tmp460,_tmp46C,_tmp46A);_tmp469.f2=_tmp7C4;});_tmp469;});};};}case 7U: _LL3: _tmp4A8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp462)->f1;_tmp4A7=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp462)->f2;_LL4: {
# 2093
struct _tuple19 _tmp470=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4A8);struct _tuple19 _tmp471=_tmp470;union Cyc_CfFlowInfo_FlowInfo _tmp478;union Cyc_CfFlowInfo_FlowInfo _tmp477;_LL19: _tmp478=_tmp471.f1;_tmp477=_tmp471.f2;_LL1A:;{
struct _tuple19 _tmp472=Cyc_NewControlFlow_anal_test(env,_tmp478,_tmp4A7);struct _tuple19 _tmp473=_tmp472;union Cyc_CfFlowInfo_FlowInfo _tmp476;union Cyc_CfFlowInfo_FlowInfo _tmp475;_LL1C: _tmp476=_tmp473.f1;_tmp475=_tmp473.f2;_LL1D:;
return({struct _tuple19 _tmp474;_tmp474.f1=_tmp476;({union Cyc_CfFlowInfo_FlowInfo _tmp7C5=Cyc_CfFlowInfo_join_flow(_tmp460,_tmp477,_tmp475);_tmp474.f2=_tmp7C5;});_tmp474;});};}case 8U: _LL5: _tmp4AA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp462)->f1;_tmp4A9=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp462)->f2;_LL6: {
# 2097
struct _tuple19 _tmp479=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4AA);struct _tuple19 _tmp47A=_tmp479;union Cyc_CfFlowInfo_FlowInfo _tmp481;union Cyc_CfFlowInfo_FlowInfo _tmp480;_LL1F: _tmp481=_tmp47A.f1;_tmp480=_tmp47A.f2;_LL20:;{
struct _tuple19 _tmp47B=Cyc_NewControlFlow_anal_test(env,_tmp480,_tmp4A9);struct _tuple19 _tmp47C=_tmp47B;union Cyc_CfFlowInfo_FlowInfo _tmp47F;union Cyc_CfFlowInfo_FlowInfo _tmp47E;_LL22: _tmp47F=_tmp47C.f1;_tmp47E=_tmp47C.f2;_LL23:;
return({struct _tuple19 _tmp47D;({union Cyc_CfFlowInfo_FlowInfo _tmp7C6=Cyc_CfFlowInfo_join_flow(_tmp460,_tmp481,_tmp47F);_tmp47D.f1=_tmp7C6;});_tmp47D.f2=_tmp47E;_tmp47D;});};}case 9U: _LL7: _tmp4AC=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp462)->f1;_tmp4AB=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp462)->f2;_LL8: {
# 2101
struct _tuple16 _tmp482=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp4AC);struct _tuple16 _tmp483=_tmp482;union Cyc_CfFlowInfo_FlowInfo _tmp485;void*_tmp484;_LL25: _tmp485=_tmp483.f1;_tmp484=_tmp483.f2;_LL26:;
return Cyc_NewControlFlow_anal_test(env,_tmp485,_tmp4AB);}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp462)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp462)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp462)->f2)->tl == 0){_LL9: _tmp4AD=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp462)->f2)->hd;_LLA: {
# 2104
struct _tuple19 _tmp486=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4AD);struct _tuple19 _tmp487=_tmp486;union Cyc_CfFlowInfo_FlowInfo _tmp48A;union Cyc_CfFlowInfo_FlowInfo _tmp489;_LL28: _tmp48A=_tmp487.f1;_tmp489=_tmp487.f2;_LL29:;
return({struct _tuple19 _tmp488;_tmp488.f1=_tmp489;_tmp488.f2=_tmp48A;_tmp488;});}}else{goto _LLB;}}else{goto _LLB;}}else{_LLB: _tmp4AF=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp462)->f1;_tmp4AE=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp462)->f2;_LLC:
# 2107
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp4AF,_tmp4AE);}default: _LLD: _LLE: {
# 2111
struct _tuple16 _tmp48B=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);struct _tuple16 _tmp48C=_tmp48B;union Cyc_CfFlowInfo_FlowInfo _tmp4A3;void*_tmp4A2;_LL2B: _tmp4A3=_tmp48C.f1;_tmp4A2=_tmp48C.f2;_LL2C:;{
union Cyc_CfFlowInfo_FlowInfo _tmp48D=_tmp4A3;struct Cyc_Dict_Dict _tmp4A1;if((_tmp48D.BottomFL).tag == 1){_LL2E: _LL2F:
 return({struct _tuple19 _tmp48E;_tmp48E.f1=_tmp4A3;_tmp48E.f2=_tmp4A3;_tmp48E;});}else{_LL30: _tmp4A1=((_tmp48D.ReachableFL).val).f1;_LL31: {
# 2115
struct _tuple15 _tmp48F=Cyc_CfFlowInfo_unname_rval(_tmp4A2);struct _tuple15 _tmp490=_tmp48F;void*_tmp4A0;struct Cyc_List_List*_tmp49F;_LL33: _tmp4A0=_tmp490.f1;_tmp49F=_tmp490.f2;_LL34:;{
void*_tmp491=_tmp4A0;enum Cyc_CfFlowInfo_InitLevel _tmp49E;struct Cyc_Absyn_Vardecl*_tmp49D;void*_tmp49C;switch(*((int*)_tmp491)){case 8U: _LL36: _tmp49D=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp491)->f1;_tmp49C=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp491)->f2;_LL37:
# 2118
({void*_tmp492=0U;({struct _dyneither_ptr _tmp7C7=({const char*_tmp493="anal_test: bad namedlocation";_tag_dyneither(_tmp493,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7C7,_tag_dyneither(_tmp492,sizeof(void*),0U));});});case 0U: _LL38: _LL39:
 return({struct _tuple19 _tmp494;({union Cyc_CfFlowInfo_FlowInfo _tmp7C8=Cyc_CfFlowInfo_BottomFL();_tmp494.f1=_tmp7C8;});_tmp494.f2=_tmp4A3;_tmp494;});case 1U: _LL3A: _LL3B:
 goto _LL3D;case 5U: _LL3C: _LL3D:
 goto _LL3F;case 4U: _LL3E: _LL3F:
 return({struct _tuple19 _tmp495;_tmp495.f1=_tmp4A3;({union Cyc_CfFlowInfo_FlowInfo _tmp7C9=Cyc_CfFlowInfo_BottomFL();_tmp495.f2=_tmp7C9;});_tmp495;});case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp491)->f1 == Cyc_CfFlowInfo_NoneIL){_LL40: _LL41:
 goto _LL43;}else{_LL46: _tmp49E=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp491)->f1;_LL47:
# 2128
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp4A3,e,_tmp49E,_tmp49F);}case 3U: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp491)->f1 == Cyc_CfFlowInfo_NoneIL){_LL42: _LL43:
# 2124
 goto _LL45;}else{_LL48: _LL49:
# 2129
 return({struct _tuple19 _tmp499;_tmp499.f1=_tmp4A3;_tmp499.f2=_tmp4A3;_tmp499;});}case 7U: _LL44: _LL45:
# 2126
({void*_tmp496=0U;({unsigned int _tmp7CB=e->loc;struct _dyneither_ptr _tmp7CA=({const char*_tmp497="expression may not be initialized";_tag_dyneither(_tmp497,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp7CB,_tmp7CA,_tag_dyneither(_tmp496,sizeof(void*),0U));});});
return({struct _tuple19 _tmp498;({union Cyc_CfFlowInfo_FlowInfo _tmp7CC=Cyc_CfFlowInfo_BottomFL();_tmp498.f1=_tmp7CC;});({union Cyc_CfFlowInfo_FlowInfo _tmp7CD=Cyc_CfFlowInfo_BottomFL();_tmp498.f2=_tmp7CD;});_tmp498;});default: _LL4A: _LL4B:
# 2130
({void*_tmp49A=0U;({struct _dyneither_ptr _tmp7CE=({const char*_tmp49B="anal_test";_tag_dyneither(_tmp49B,sizeof(char),10U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7CE,_tag_dyneither(_tmp49A,sizeof(void*),0U));});});}_LL35:;};}}_LL2D:;};}}_LL0:;}struct _tuple27{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 2136
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple27*ckenv,void*root,void*rval){
# 2138
struct _tuple27*_tmp4B0=ckenv;unsigned int _tmp4BC;struct Cyc_NewControlFlow_AnalEnv*_tmp4BB;struct Cyc_Dict_Dict _tmp4BA;_LL1: _tmp4BC=_tmp4B0->f1;_tmp4BB=_tmp4B0->f2;_tmp4BA=_tmp4B0->f3;_LL2:;{
void*_tmp4B1=root;struct Cyc_Absyn_Vardecl*_tmp4B9;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4B1)->tag == 0U){_LL4: _tmp4B9=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4B1)->f1;_LL5:
# 2141
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp4B9->type)){
struct _tuple15 _tmp4B2=Cyc_CfFlowInfo_unname_rval(rval);struct _tuple15 _tmp4B3=_tmp4B2;void*_tmp4B8;_LL9: _tmp4B8=_tmp4B3.f1;_LLA:;{
void*_tmp4B4=_tmp4B8;switch(*((int*)_tmp4B4)){case 7U: _LLC: _LLD:
 goto _LLF;case 0U: _LLE: _LLF:
 goto _LL11;case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4B4)->f1 == Cyc_CfFlowInfo_NoneIL){_LL10: _LL11:
 goto _LLB;}else{goto _LL12;}default: _LL12: _LL13:
# 2148
({struct Cyc_String_pa_PrintArg_struct _tmp4B7;_tmp4B7.tag=0U;({struct _dyneither_ptr _tmp7CF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4B9->name));_tmp4B7.f1=_tmp7CF;});({void*_tmp4B5[1U]={& _tmp4B7};({unsigned int _tmp7D1=_tmp4BC;struct _dyneither_ptr _tmp7D0=({const char*_tmp4B6="unique pointers reachable from %s may become unreachable";_tag_dyneither(_tmp4B6,sizeof(char),57U);});Cyc_Warn_warn(_tmp7D1,_tmp7D0,_tag_dyneither(_tmp4B5,sizeof(void*),1U));});});});
goto _LLB;}_LLB:;};}
# 2152
goto _LL3;}else{_LL6: _LL7:
 goto _LL3;}_LL3:;};}
# 2157
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp4BD=flow;struct Cyc_Dict_Dict _tmp4C3;if((_tmp4BD.BottomFL).tag == 1){_LL1: _LL2:
 return;}else{_LL3: _tmp4C3=((_tmp4BD.ReachableFL).val).f1;_LL4:
# 2161
{struct Cyc_List_List*_tmp4BE=env->param_roots;for(0;_tmp4BE != 0;_tmp4BE=_tmp4BE->tl){
if(({struct Cyc_CfFlowInfo_FlowEnv*_tmp7D3=env->fenv;struct Cyc_Dict_Dict _tmp7D2=_tmp4C3;Cyc_CfFlowInfo_initlevel(_tmp7D3,_tmp7D2,Cyc_CfFlowInfo_lookup_place(_tmp4C3,(struct Cyc_CfFlowInfo_Place*)_tmp4BE->hd));})!= Cyc_CfFlowInfo_AllIL)
({void*_tmp4BF=0U;({unsigned int _tmp7D5=loc;struct _dyneither_ptr _tmp7D4=({const char*_tmp4C0="function may not initialize all the parameters with attribute 'initializes'";_tag_dyneither(_tmp4C0,sizeof(char),76U);});Cyc_CfFlowInfo_aerr(_tmp7D5,_tmp7D4,_tag_dyneither(_tmp4BF,sizeof(void*),0U));});});}}
# 2165
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple27 _tmp4C1=({struct _tuple27 _tmp4C2;_tmp4C2.f1=loc;_tmp4C2.f2=env;_tmp4C2.f3=_tmp4C3;_tmp4C2;});
((void(*)(void(*f)(struct _tuple27*,void*,void*),struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp4C1,_tmp4C3);}
# 2169
return;}_LL0:;}
# 2178
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_pat_vars(struct Cyc_List_List*vds){
# 2180
struct Cyc_List_List*_tmp4C4=0;
struct Cyc_List_List*_tmp4C5=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp4C6=(struct _tuple24*)x->hd;struct _tuple24*_tmp4C7=_tmp4C6;struct Cyc_Absyn_Vardecl**_tmp4D0;struct Cyc_Absyn_Exp*_tmp4CF;_LL1: _tmp4D0=_tmp4C7->f1;_tmp4CF=_tmp4C7->f2;_LL2:;
if((_tmp4D0 != 0  && _tmp4CF != 0) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp4CF->topt),0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4C8=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4CD=_cycalloc(sizeof(*_tmp4CD));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp7D6=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp4CE;_tmp4CE.tag=0U;_tmp4CE.f1=*_tmp4D0;_tmp4CE;});_tmp4CD[0]=_tmp7D6;});_tmp4CD;});
struct Cyc_CfFlowInfo_Place*_tmp4C9=({struct Cyc_CfFlowInfo_Place*_tmp4CC=_cycalloc(sizeof(*_tmp4CC));_tmp4CC->root=(void*)_tmp4C8;_tmp4CC->path=0;_tmp4CC;});
({struct Cyc_List_List*_tmp7D7=({struct Cyc_List_List*_tmp4CA=_cycalloc(sizeof(*_tmp4CA));_tmp4CA->hd=_tmp4C9;_tmp4CA->tl=_tmp4C4;_tmp4CA;});_tmp4C4=_tmp7D7;});
({struct Cyc_List_List*_tmp7D8=({struct Cyc_List_List*_tmp4CB=_cycalloc(sizeof(*_tmp4CB));_tmp4CB->hd=_tmp4CF;_tmp4CB->tl=_tmp4C5;_tmp4CB;});_tmp4C5=_tmp7D8;});}}}
# 2192
return({struct _tuple1 _tmp4D1;_tmp4D1.f1=_tmp4C4;_tmp4D1.f2=_tmp4C5;_tmp4D1;});}struct _tuple28{int f1;void*f2;};
# 2198
static struct _tuple28 Cyc_NewControlFlow_noconsume_place_ok(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*place,int do_unconsume,struct Cyc_Absyn_Vardecl*vd,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2205
union Cyc_CfFlowInfo_FlowInfo _tmp4D2=flow;struct Cyc_Dict_Dict _tmp4E7;if((_tmp4D2.BottomFL).tag == 1){_LL1: _LL2:
({void*_tmp4D3=0U;({struct _dyneither_ptr _tmp7D9=({const char*_tmp4D4="noconsume_place_ok: flow became Bottom!";_tag_dyneither(_tmp4D4,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7D9,_tag_dyneither(_tmp4D3,sizeof(void*),0U));});});}else{_LL3: _tmp4E7=((_tmp4D2.ReachableFL).val).f1;_LL4: {
# 2213
struct Cyc_Absyn_Exp*_tmp4D5=Cyc_Absyn_exp_unsigned_one;
int _tmp4D6=0;
int _tmp4D7=1;
void*_tmp4D8=Cyc_CfFlowInfo_lookup_place(_tmp4E7,place);
void*_tmp4D9=_tmp4D8;
# 2225
int varok=0;
{void*_tmp4DA=_tmp4D8;struct Cyc_Absyn_Vardecl*_tmp4E5;void*_tmp4E4;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4DA)->tag == 8U){_LL6: _tmp4E5=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4DA)->f1;_tmp4E4=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4DA)->f2;_LL7:
# 2228
 if(vd == _tmp4E5){
_tmp4D9=_tmp4E4;
# 2231
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2233
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp4D5,_tmp4D7,_tmp4D9,& _tmp4D6)){
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp4DD;_tmp4DD.tag=0U;({struct _dyneither_ptr _tmp7DA=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2237
Cyc_Absynpp_qvar2string(vd->name));_tmp4DD.f1=_tmp7DA;});({void*_tmp4DB[1U]={& _tmp4DD};({unsigned int _tmp7DC=loc;struct _dyneither_ptr _tmp7DB=({const char*_tmp4DC="function consumes parameter %s with attribute 'noconsume'";_tag_dyneither(_tmp4DC,sizeof(char),58U);});Cyc_CfFlowInfo_aerr(_tmp7DC,_tmp7DB,_tag_dyneither(_tmp4DB,sizeof(void*),1U));});});});}else{
# 2240
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4E7,_tmp4D9)!= Cyc_CfFlowInfo_AllIL  && !do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp4E0;_tmp4E0.tag=0U;({struct _dyneither_ptr _tmp7DD=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2243
Cyc_Absynpp_qvar2string(vd->name));_tmp4E0.f1=_tmp7DD;});({void*_tmp4DE[1U]={& _tmp4E0};({unsigned int _tmp7DF=loc;struct _dyneither_ptr _tmp7DE=({const char*_tmp4DF="function consumes value pointed to by parameter %s, which has attribute 'noconsume'";_tag_dyneither(_tmp4DF,sizeof(char),84U);});Cyc_CfFlowInfo_aerr(_tmp7DF,_tmp7DE,_tag_dyneither(_tmp4DE,sizeof(void*),1U));});});});else{
# 2245
varok=1;}}}else{
# 2248
varok=1;}}else{
# 2251
goto _LL9;}
goto _LL5;}else{_LL8: _LL9:
# 2255
 if(!Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
varok=1;else{
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp4E3;_tmp4E3.tag=0U;({struct _dyneither_ptr _tmp7E0=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2260
Cyc_Absynpp_qvar2string(vd->name));_tmp4E3.f1=_tmp7E0;});({void*_tmp4E1[1U]={& _tmp4E3};({unsigned int _tmp7E2=loc;struct _dyneither_ptr _tmp7E1=({const char*_tmp4E2="function parameter %s with attribute 'noconsume' no longer set to its original value";_tag_dyneither(_tmp4E2,sizeof(char),85U);});Cyc_CfFlowInfo_aerr(_tmp7E2,_tmp7E1,_tag_dyneither(_tmp4E1,sizeof(void*),1U));});});});}
goto _LL5;}_LL5:;}
# 2267
return({struct _tuple28 _tmp4E6;_tmp4E6.f1=varok;_tmp4E6.f2=_tmp4D9;_tmp4E6;});}}_LL0:;}
# 2273
static struct Cyc_Absyn_Vardecl*Cyc_NewControlFlow_get_vd_from_place(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_CfFlowInfo_Place*_tmp4E8=p;void*_tmp4F0;struct Cyc_List_List*_tmp4EF;_LL1: _tmp4F0=_tmp4E8->root;_tmp4EF=_tmp4E8->path;_LL2:;
if(_tmp4EF != 0)
({void*_tmp4E9=0U;({struct _dyneither_ptr _tmp7E3=({const char*_tmp4EA="unconsume_params: param to unconsume is non-variable";_tag_dyneither(_tmp4EA,sizeof(char),53U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7E3,_tag_dyneither(_tmp4E9,sizeof(void*),0U));});});{
struct Cyc_Absyn_Vardecl*vd;
void*_tmp4EB=_tmp4F0;struct Cyc_Absyn_Vardecl*_tmp4EE;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4EB)->tag == 0U){_LL4: _tmp4EE=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4EB)->f1;_LL5:
 return _tmp4EE;}else{_LL6: _LL7:
({void*_tmp4EC=0U;({struct _dyneither_ptr _tmp7E4=({const char*_tmp4ED="unconsume_params: root is not a varroot";_tag_dyneither(_tmp4ED,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7E4,_tag_dyneither(_tmp4EC,sizeof(void*),0U));});});}_LL3:;};}
# 2292 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_exp(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Exp*unconsume_exp,struct Cyc_Absyn_Vardecl*vd,int varok,void*ropt,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2300
{union Cyc_CfFlowInfo_FlowInfo _tmp4F1=flow;struct Cyc_Dict_Dict _tmp50A;if((_tmp4F1.BottomFL).tag == 1){_LL1: _LL2:
 return flow;}else{_LL3: _tmp50A=((_tmp4F1.ReachableFL).val).f1;_LL4: {
# 2307
struct _tuple18 _tmp4F2=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,unconsume_exp);struct _tuple18 _tmp4F3=_tmp4F2;union Cyc_CfFlowInfo_FlowInfo _tmp509;union Cyc_CfFlowInfo_AbsLVal _tmp508;_LL6: _tmp509=_tmp4F3.f1;_tmp508=_tmp4F3.f2;_LL7:;
# 2310
{union Cyc_CfFlowInfo_AbsLVal _tmp4F4=_tmp508;struct Cyc_CfFlowInfo_Place*_tmp507;if((_tmp4F4.PlaceL).tag == 1){_LL9: _tmp507=(_tmp4F4.PlaceL).val;_LLA: {
# 2314
void*_tmp4F5=Cyc_CfFlowInfo_lookup_place(_tmp50A,_tmp507);
{void*_tmp4F6=_tmp4F5;struct Cyc_Absyn_Vardecl*_tmp506;void*_tmp505;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4F6)->tag == 8U){_LLE: _tmp506=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4F6)->f1;_tmp505=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4F6)->f2;_LLF: {
# 2320
void*new_rval;
if(_tmp506 == vd){
# 2324
if(varok){
# 2326
({void*_tmp7E5=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,_tmp505);_tmp4F5=_tmp7E5;});
# 2331
if(ropt != 0){
# 2337
struct _tuple16 _tmp4F7=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp7E7=env->fenv;struct _tuple16 _tmp7E6=({struct _tuple16 _tmp4FB;_tmp4FB.f1=_tmp509;_tmp4FB.f2=_tmp4F5;_tmp4FB;});Cyc_CfFlowInfo_join_flow_and_rval(_tmp7E7,_tmp7E6,({struct _tuple16 _tmp4FC;_tmp4FC.f1=_tmp509;_tmp4FC.f2=ropt;_tmp4FC;}));});
# 2337
struct _tuple16 _tmp4F8=_tmp4F7;union Cyc_CfFlowInfo_FlowInfo _tmp4FA;void*_tmp4F9;_LL13: _tmp4FA=_tmp4F8.f1;_tmp4F9=_tmp4F8.f2;_LL14:;
# 2341
_tmp509=_tmp4FA;new_rval=_tmp4F9;}else{
# 2346
new_rval=_tmp4F5;}}else{
# 2349
new_rval=_tmp505;}
# 2351
{union Cyc_CfFlowInfo_FlowInfo _tmp4FD=_tmp509;struct Cyc_Dict_Dict _tmp501;struct Cyc_List_List*_tmp500;if((_tmp4FD.ReachableFL).tag == 2){_LL16: _tmp501=((_tmp4FD.ReachableFL).val).f1;_tmp500=((_tmp4FD.ReachableFL).val).f2;_LL17:
# 2353
({union Cyc_CfFlowInfo_FlowInfo _tmp7E9=({struct Cyc_Dict_Dict _tmp7E8=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp501,_tmp507,new_rval);Cyc_CfFlowInfo_ReachableFL(_tmp7E8,_tmp500);});flow=_tmp7E9;});
# 2357
goto _LL15;}else{_LL18: _LL19:
# 2359
({void*_tmp4FE=0U;({struct _dyneither_ptr _tmp7EA=({const char*_tmp4FF="unconsume_params: joined flow became bot!";_tag_dyneither(_tmp4FF,sizeof(char),42U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7EA,_tag_dyneither(_tmp4FE,sizeof(void*),0U));});});}_LL15:;}
# 2361
goto _LLD;}else{
# 2363
goto _LL11;}
goto _LLD;}}else{_LL10: _LL11:
# 2370
 if(ropt != 0  && !
Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
({struct Cyc_String_pa_PrintArg_struct _tmp504;_tmp504.tag=0U;({struct _dyneither_ptr _tmp7EB=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(unconsume_exp));_tmp504.f1=_tmp7EB;});({void*_tmp502[1U]={& _tmp504};({unsigned int _tmp7ED=loc;struct _dyneither_ptr _tmp7EC=({const char*_tmp503="aliased expression %s was overwritten";_tag_dyneither(_tmp503,sizeof(char),38U);});Cyc_CfFlowInfo_aerr(_tmp7ED,_tmp7EC,_tag_dyneither(_tmp502,sizeof(void*),1U));});});});
# 2387 "new_control_flow.cyc"
goto _LLD;}_LLD:;}
# 2389
goto _LL8;}}else{_LLB: _LLC:
# 2395
 goto _LL8;}_LL8:;}
# 2397
goto _LL0;}}_LL0:;}
# 2399
return flow;}
# 2412 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,int is_region_open,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2418
{union Cyc_CfFlowInfo_FlowInfo _tmp50B=flow;if((_tmp50B.BottomFL).tag == 1){_LL1: _LL2:
 return flow;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 2422
int _tmp50C=unconsume_exps != 0;
struct Cyc_List_List*_tmp50D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(consumed_vals);
struct Cyc_List_List*_tmp50E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(unconsume_exps);
{struct Cyc_List_List*_tmp50F=_tmp50D;for(0;_tmp50F != 0;
(_tmp50F=_tmp50F->tl,
_tmp50E != 0?_tmp50E=_tmp50E->tl: 0)){
# 2431
struct Cyc_Absyn_Vardecl*_tmp510=Cyc_NewControlFlow_get_vd_from_place((struct Cyc_CfFlowInfo_Place*)_tmp50F->hd);
struct _tuple28 _tmp511=
is_region_open?({struct _tuple28 _tmp515;_tmp515.f1=1;_tmp515.f2=0;_tmp515;}):
 Cyc_NewControlFlow_noconsume_place_ok(env,(struct Cyc_CfFlowInfo_Place*)_tmp50F->hd,_tmp50C,_tmp510,flow,loc);
# 2432
struct _tuple28 _tmp512=_tmp511;int _tmp514;void*_tmp513;_LL6: _tmp514=_tmp512.f1;_tmp513=_tmp512.f2;_LL7:;
# 2438
if(_tmp50C)
({union Cyc_CfFlowInfo_FlowInfo _tmp7EE=Cyc_NewControlFlow_unconsume_exp(env,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp50E))->hd,_tmp510,_tmp514,_tmp513,flow,loc);flow=_tmp7EE;});}}
# 2441
Cyc_NewControlFlow_update_tryflow(env,flow);
return flow;};}struct _tuple29{int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 2445
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned int exp_loc){
# 2447
struct Cyc_CfFlowInfo_FlowEnv*_tmp516=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp517=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_tmp518=_tmp517;struct Cyc_Core_Opt*_tmp52C;struct Cyc_Absyn_Exp*_tmp52B;struct Cyc_Absyn_Stmt*_tmp52A;unsigned int _tmp529;_LL1: _tmp52C=_tmp518->pat_vars;_tmp52B=_tmp518->where_clause;_tmp52A=_tmp518->body;_tmp529=_tmp518->loc;_LL2:;{
struct _tuple1 _tmp519=Cyc_NewControlFlow_get_unconsume_pat_vars((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp52C))->v);struct _tuple1 _tmp51A=_tmp519;struct Cyc_List_List*_tmp528;struct Cyc_List_List*_tmp527;_LL4: _tmp528=_tmp51A.f1;_tmp527=_tmp51A.f2;_LL5:;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)_tmp52C->v,_tmp528 != 0,_tmp529,exp_loc);
# 2455
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp51B=env->unique_pat_vars;
# 2458
if(Cyc_Tcpat_has_vars(_tmp52C))
({struct Cyc_List_List*_tmp7F0=({struct Cyc_List_List*_tmp51C=_cycalloc(sizeof(*_tmp51C));({struct _tuple29*_tmp7EF=({struct _tuple29*_tmp51D=_cycalloc(sizeof(*_tmp51D));_tmp51D->f1=0;_tmp51D->f2=_tmp52A;_tmp51D->f3=_tmp528;_tmp51D->f4=_tmp527;_tmp51D;});_tmp51C->hd=_tmp7EF;});_tmp51C->tl=_tmp51B;_tmp51C;});env->unique_pat_vars=_tmp7F0;});
# 2462
if(_tmp52B != 0){
struct Cyc_Absyn_Exp*wexp=_tmp52B;
struct _tuple19 _tmp51E=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);struct _tuple19 _tmp51F=_tmp51E;union Cyc_CfFlowInfo_FlowInfo _tmp521;union Cyc_CfFlowInfo_FlowInfo _tmp520;_LL7: _tmp521=_tmp51F.f1;_tmp520=_tmp51F.f2;_LL8:;
inflow=_tmp520;
({union Cyc_CfFlowInfo_FlowInfo _tmp7F1=Cyc_NewControlFlow_anal_stmt(env,_tmp521,_tmp52A,0);clause_outflow=_tmp7F1;});}else{
# 2468
({union Cyc_CfFlowInfo_FlowInfo _tmp7F2=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp52A,0);clause_outflow=_tmp7F2;});}
# 2470
env->unique_pat_vars=_tmp51B;{
union Cyc_CfFlowInfo_FlowInfo _tmp522=clause_outflow;if((_tmp522.BottomFL).tag == 1){_LLA: _LLB:
 goto _LL9;}else{_LLC: _LLD:
# 2475
({union Cyc_CfFlowInfo_FlowInfo _tmp7F3=Cyc_NewControlFlow_unconsume_params(env,_tmp528,_tmp527,0,clause_outflow,_tmp529);clause_outflow=_tmp7F3;});
# 2477
if(scs->tl == 0)
return clause_outflow;else{
# 2482
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0)
({void*_tmp523=0U;({unsigned int _tmp7F5=_tmp52A->loc;struct _dyneither_ptr _tmp7F4=({const char*_tmp524="switch clause may implicitly fallthru";_tag_dyneither(_tmp524,sizeof(char),38U);});Cyc_CfFlowInfo_aerr(_tmp7F5,_tmp7F4,_tag_dyneither(_tmp523,sizeof(void*),0U));});});else{
# 2485
({void*_tmp525=0U;({unsigned int _tmp7F7=_tmp52A->loc;struct _dyneither_ptr _tmp7F6=({const char*_tmp526="switch clause may implicitly fallthru";_tag_dyneither(_tmp526,sizeof(char),38U);});Cyc_Warn_warn(_tmp7F7,_tmp7F6,_tag_dyneither(_tmp525,sizeof(void*),0U));});});}
# 2487
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2489
goto _LL9;}_LL9:;};};};}
# 2492
return Cyc_CfFlowInfo_BottomFL();}struct _tuple30{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2497
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple30*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2499
struct _tuple30*_tmp52D=vdenv;struct Cyc_NewControlFlow_AnalEnv*_tmp539;struct Cyc_Dict_Dict _tmp538;unsigned int _tmp537;_LL1: _tmp539=_tmp52D->f1;_tmp538=_tmp52D->f2;_tmp537=_tmp52D->f3;_LL2:;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2502
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp536;_tmp536.tag=0U;_tmp536.f1=vd;_tmp536;});
struct _tuple15 _tmp52E=Cyc_CfFlowInfo_unname_rval(({struct Cyc_Dict_Dict _tmp7F8=_tmp538;((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp7F8,Cyc_CfFlowInfo_root_cmp,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp535=_cycalloc(sizeof(*_tmp535));_tmp535[0]=vdroot;_tmp535;}));}));struct _tuple15 _tmp52F=_tmp52E;void*_tmp534;_LL4: _tmp534=_tmp52F.f1;_LL5:;{
void*_tmp530=_tmp534;switch(*((int*)_tmp530)){case 7U: _LL7: _LL8:
 goto _LLA;case 0U: _LL9: _LLA:
 goto _LLC;case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp530)->f1 == Cyc_CfFlowInfo_NoneIL){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
# 2509
({struct Cyc_String_pa_PrintArg_struct _tmp533;_tmp533.tag=0U;({struct _dyneither_ptr _tmp7F9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp533.f1=_tmp7F9;});({void*_tmp531[1U]={& _tmp533};({unsigned int _tmp7FB=_tmp537;struct _dyneither_ptr _tmp7FA=({const char*_tmp532="unique pointers may still exist after variable %s goes out of scope";_tag_dyneither(_tmp532,sizeof(char),68U);});Cyc_Warn_warn(_tmp7FB,_tmp7FA,_tag_dyneither(_tmp531,sizeof(void*),1U));});});});
# 2511
goto _LL6;}_LL6:;};}}
# 2516
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
{union Cyc_CfFlowInfo_FlowInfo _tmp53A=inflow;struct Cyc_Dict_Dict _tmp547;if((_tmp53A.ReachableFL).tag == 2){_LL1: _tmp547=((_tmp53A.ReachableFL).val).f1;_LL2: {
# 2519
struct _tuple30 _tmp53B=({struct _tuple30 _tmp546;_tmp546.f1=env;_tmp546.f2=_tmp547;_tmp546.f3=decl->loc;_tmp546;});
struct Cyc_CfFlowInfo_FlowEnv*_tmp53C=env->fenv;
{void*_tmp53D=decl->r;void*_tmp53E=_tmp53D;struct Cyc_List_List*_tmp545;struct Cyc_List_List*_tmp544;struct Cyc_Absyn_Vardecl*_tmp543;switch(*((int*)_tmp53E)){case 0U: _LL6: _tmp543=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp53E)->f1;_LL7:
# 2523
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp53B,_tmp543);
goto _LL5;case 2U: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp53E)->f2 != 0){_LL8: _tmp544=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp53E)->f2)->v;_LL9: {
# 2526
struct _tuple1 _tmp53F=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp544);struct _tuple1 _tmp540=_tmp53F;struct Cyc_List_List*_tmp542;_LLF: _tmp542=_tmp540.f1;_LL10:;{
struct Cyc_List_List*_tmp541=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)(_tmp542);
_tmp545=_tmp541;goto _LLB;};}}else{goto _LLC;}case 3U: _LLA: _tmp545=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp53E)->f1;_LLB:
# 2530
((void(*)(void(*f)(struct _tuple30*,struct Cyc_Absyn_Vardecl*),struct _tuple30*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp53B,_tmp545);
goto _LL5;default: _LLC: _LLD:
 goto _LL5;}_LL5:;}
# 2534
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2537
return;}
# 2543
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_pat_vars(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*src,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2547
int num_pop=((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->pat_pop_table,src);
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;num_pop > 0;(x=x->tl,-- num_pop)){
struct _tuple29*_tmp548=(struct _tuple29*)((struct Cyc_List_List*)_check_null(x))->hd;struct _tuple29*_tmp549=_tmp548;int _tmp54D;struct Cyc_Absyn_Stmt*_tmp54C;struct Cyc_List_List*_tmp54B;struct Cyc_List_List*_tmp54A;_LL1: _tmp54D=_tmp549->f1;_tmp54C=_tmp549->f2;_tmp54B=_tmp549->f3;_tmp54A=_tmp549->f4;_LL2:;
({union Cyc_CfFlowInfo_FlowInfo _tmp7FC=Cyc_NewControlFlow_unconsume_params(env,_tmp54B,_tmp54A,_tmp54D,inflow,dest->loc);inflow=_tmp7FC;});}}
# 2552
return inflow;}
# 2558
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple17*rval_opt){
# 2560
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple20 _tmp54E=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct _tuple20 _tmp54F=_tmp54E;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5EF;union Cyc_CfFlowInfo_FlowInfo*_tmp5EE;_LL1: _tmp5EF=_tmp54F.f1;_tmp5EE=_tmp54F.f2;_LL2:;
inflow=*_tmp5EE;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp550=env->fenv;
# 2567
void*_tmp551=s->r;void*_tmp552=_tmp551;struct Cyc_Absyn_Stmt*_tmp5ED;struct Cyc_Absyn_Decl*_tmp5EC;struct Cyc_Absyn_Stmt*_tmp5EB;struct Cyc_Absyn_Vardecl*_tmp5EA;struct Cyc_Absyn_Exp*_tmp5E9;unsigned int _tmp5E8;struct Cyc_Absyn_Stmt*_tmp5E7;struct Cyc_List_List*_tmp5E6;struct Cyc_Absyn_Exp*_tmp5E5;unsigned int _tmp5E4;struct Cyc_Absyn_Stmt*_tmp5E3;struct Cyc_Absyn_Stmt*_tmp5E2;struct Cyc_List_List*_tmp5E1;void*_tmp5E0;struct Cyc_Absyn_Exp*_tmp5DF;struct Cyc_List_List*_tmp5DE;void*_tmp5DD;struct Cyc_List_List*_tmp5DC;struct Cyc_Absyn_Switch_clause*_tmp5DB;struct Cyc_Absyn_Exp*_tmp5DA;struct Cyc_Absyn_Exp*_tmp5D9;struct Cyc_Absyn_Stmt*_tmp5D8;struct Cyc_Absyn_Exp*_tmp5D7;struct Cyc_Absyn_Stmt*_tmp5D6;struct Cyc_Absyn_Stmt*_tmp5D5;struct Cyc_Absyn_Stmt*_tmp5D4;struct Cyc_Absyn_Exp*_tmp5D3;struct Cyc_Absyn_Stmt*_tmp5D2;struct Cyc_Absyn_Exp*_tmp5D1;struct Cyc_Absyn_Stmt*_tmp5D0;struct Cyc_Absyn_Stmt*_tmp5CF;struct Cyc_Absyn_Exp*_tmp5CE;struct Cyc_Absyn_Stmt*_tmp5CD;struct Cyc_Absyn_Stmt*_tmp5CC;struct Cyc_Absyn_Stmt*_tmp5CB;struct Cyc_Absyn_Stmt*_tmp5CA;struct Cyc_Absyn_Exp*_tmp5C9;struct Cyc_Absyn_Exp*_tmp5C8;switch(*((int*)_tmp552)){case 0U: _LL4: _LL5:
 return inflow;case 3U: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp552)->f1 == 0){_LL6: _LL7:
# 2571
 if(env->noreturn)
({void*_tmp553=0U;({unsigned int _tmp7FE=s->loc;struct _dyneither_ptr _tmp7FD=({const char*_tmp554="`noreturn' function might return";_tag_dyneither(_tmp554,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp7FE,_tmp7FD,_tag_dyneither(_tmp553,sizeof(void*),0U));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}else{_LL8: _tmp5C8=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp552)->f1;_LL9:
# 2577
 if(env->noreturn)
({void*_tmp555=0U;({unsigned int _tmp800=s->loc;struct _dyneither_ptr _tmp7FF=({const char*_tmp556="`noreturn' function might return";_tag_dyneither(_tmp556,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp800,_tmp7FF,_tag_dyneither(_tmp555,sizeof(void*),0U));});});{
struct _tuple16 _tmp557=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp5C8));struct _tuple16 _tmp558=_tmp557;union Cyc_CfFlowInfo_FlowInfo _tmp55A;void*_tmp559;_LL2D: _tmp55A=_tmp558.f1;_tmp559=_tmp558.f2;_LL2E:;
({union Cyc_CfFlowInfo_FlowInfo _tmp801=Cyc_NewControlFlow_use_Rval(env,_tmp5C8->loc,_tmp55A,_tmp559);_tmp55A=_tmp801;});
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp55A);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,_tmp55A,s->loc);
return Cyc_CfFlowInfo_BottomFL();};}case 1U: _LLA: _tmp5C9=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp552)->f1;_LLB: {
# 2586
struct _tuple17*_tmp55B=rval_opt;void**_tmp561;int _tmp560;if(_tmp55B != 0){_LL30: _tmp561=(void**)& _tmp55B->f1;_tmp560=_tmp55B->f2;_LL31: {
# 2588
struct _tuple16 _tmp55C=Cyc_NewControlFlow_anal_Rexp(env,_tmp560,inflow,_tmp5C9);struct _tuple16 _tmp55D=_tmp55C;union Cyc_CfFlowInfo_FlowInfo _tmp55F;void*_tmp55E;_LL35: _tmp55F=_tmp55D.f1;_tmp55E=_tmp55D.f2;_LL36:;
*_tmp561=_tmp55E;
return _tmp55F;}}else{_LL32: _LL33:
# 2592
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp5C9)).f1;}_LL2F:;}case 2U: _LLC: _tmp5CB=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp552)->f1;_tmp5CA=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp552)->f2;_LLD:
# 2596
 return({struct Cyc_NewControlFlow_AnalEnv*_tmp804=env;union Cyc_CfFlowInfo_FlowInfo _tmp803=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5CB,0);struct Cyc_Absyn_Stmt*_tmp802=_tmp5CA;Cyc_NewControlFlow_anal_stmt(_tmp804,_tmp803,_tmp802,rval_opt);});case 4U: _LLE: _tmp5CE=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp552)->f1;_tmp5CD=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp552)->f2;_tmp5CC=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp552)->f3;_LLF: {
# 2599
struct _tuple19 _tmp562=Cyc_NewControlFlow_anal_test(env,inflow,_tmp5CE);struct _tuple19 _tmp563=_tmp562;union Cyc_CfFlowInfo_FlowInfo _tmp567;union Cyc_CfFlowInfo_FlowInfo _tmp566;_LL38: _tmp567=_tmp563.f1;_tmp566=_tmp563.f2;_LL39:;{
# 2606
union Cyc_CfFlowInfo_FlowInfo _tmp564=Cyc_NewControlFlow_anal_stmt(env,_tmp566,_tmp5CC,0);
union Cyc_CfFlowInfo_FlowInfo _tmp565=Cyc_NewControlFlow_anal_stmt(env,_tmp567,_tmp5CD,0);
return Cyc_CfFlowInfo_join_flow(_tmp550,_tmp565,_tmp564);};}case 5U: _LL10: _tmp5D1=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp552)->f1).f1;_tmp5D0=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp552)->f1).f2;_tmp5CF=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp552)->f2;_LL11: {
# 2614
struct _tuple20 _tmp568=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp5D0);struct _tuple20 _tmp569=_tmp568;union Cyc_CfFlowInfo_FlowInfo*_tmp570;_LL3B: _tmp570=_tmp569.f2;_LL3C:;{
union Cyc_CfFlowInfo_FlowInfo _tmp56A=*_tmp570;
struct _tuple19 _tmp56B=Cyc_NewControlFlow_anal_test(env,_tmp56A,_tmp5D1);struct _tuple19 _tmp56C=_tmp56B;union Cyc_CfFlowInfo_FlowInfo _tmp56F;union Cyc_CfFlowInfo_FlowInfo _tmp56E;_LL3E: _tmp56F=_tmp56C.f1;_tmp56E=_tmp56C.f2;_LL3F:;{
union Cyc_CfFlowInfo_FlowInfo _tmp56D=Cyc_NewControlFlow_anal_stmt(env,_tmp56F,_tmp5CF,0);
Cyc_NewControlFlow_update_flow(env,_tmp5D0,_tmp56D);
return _tmp56E;};};}case 14U: _LL12: _tmp5D4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp552)->f1;_tmp5D3=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp552)->f2).f1;_tmp5D2=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp552)->f2).f2;_LL13: {
# 2624
union Cyc_CfFlowInfo_FlowInfo _tmp571=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5D4,0);
struct _tuple20 _tmp572=Cyc_NewControlFlow_pre_stmt_check(env,_tmp571,_tmp5D2);struct _tuple20 _tmp573=_tmp572;union Cyc_CfFlowInfo_FlowInfo*_tmp579;_LL41: _tmp579=_tmp573.f2;_LL42:;{
union Cyc_CfFlowInfo_FlowInfo _tmp574=*_tmp579;
struct _tuple19 _tmp575=Cyc_NewControlFlow_anal_test(env,_tmp574,_tmp5D3);struct _tuple19 _tmp576=_tmp575;union Cyc_CfFlowInfo_FlowInfo _tmp578;union Cyc_CfFlowInfo_FlowInfo _tmp577;_LL44: _tmp578=_tmp576.f1;_tmp577=_tmp576.f2;_LL45:;
Cyc_NewControlFlow_update_flow(env,_tmp5D4,_tmp578);
return _tmp577;};}case 9U: _LL14: _tmp5DA=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp552)->f1;_tmp5D9=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp552)->f2).f1;_tmp5D8=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp552)->f2).f2;_tmp5D7=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp552)->f3).f1;_tmp5D6=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp552)->f3).f2;_tmp5D5=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp552)->f4;_LL15: {
# 2633
union Cyc_CfFlowInfo_FlowInfo _tmp57A=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp5DA)).f1;
struct _tuple20 _tmp57B=Cyc_NewControlFlow_pre_stmt_check(env,_tmp57A,_tmp5D8);struct _tuple20 _tmp57C=_tmp57B;union Cyc_CfFlowInfo_FlowInfo*_tmp588;_LL47: _tmp588=_tmp57C.f2;_LL48:;{
union Cyc_CfFlowInfo_FlowInfo _tmp57D=*_tmp588;
struct _tuple19 _tmp57E=Cyc_NewControlFlow_anal_test(env,_tmp57D,_tmp5D9);struct _tuple19 _tmp57F=_tmp57E;union Cyc_CfFlowInfo_FlowInfo _tmp587;union Cyc_CfFlowInfo_FlowInfo _tmp586;_LL4A: _tmp587=_tmp57F.f1;_tmp586=_tmp57F.f2;_LL4B:;{
union Cyc_CfFlowInfo_FlowInfo _tmp580=Cyc_NewControlFlow_anal_stmt(env,_tmp587,_tmp5D5,0);
struct _tuple20 _tmp581=Cyc_NewControlFlow_pre_stmt_check(env,_tmp580,_tmp5D6);struct _tuple20 _tmp582=_tmp581;union Cyc_CfFlowInfo_FlowInfo*_tmp585;_LL4D: _tmp585=_tmp582.f2;_LL4E:;{
union Cyc_CfFlowInfo_FlowInfo _tmp583=*_tmp585;
union Cyc_CfFlowInfo_FlowInfo _tmp584=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp583,_tmp5D7)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp5D8,_tmp584);
return _tmp586;};};};}case 11U: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp552)->f2 != 0){_LL16: _tmp5DC=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp552)->f1;_tmp5DB=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp552)->f2;_LL17: {
# 2645
struct _tuple23 _tmp589=Cyc_NewControlFlow_anal_Rexps(env,inflow,_tmp5DC,1,1);struct _tuple23 _tmp58A=_tmp589;union Cyc_CfFlowInfo_FlowInfo _tmp591;struct Cyc_List_List*_tmp590;_LL50: _tmp591=_tmp58A.f1;_tmp590=_tmp58A.f2;_LL51:;
# 2647
({union Cyc_CfFlowInfo_FlowInfo _tmp805=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,_tmp5DB->body);inflow=_tmp805;});{
# 2649
struct Cyc_List_List*_tmp58B=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5DB->pat_vars))->v)).f1);
({union Cyc_CfFlowInfo_FlowInfo _tmp806=Cyc_NewControlFlow_add_vars(_tmp550,_tmp591,_tmp58B,_tmp550->unknown_all,s->loc,0);_tmp591=_tmp806;});
# 2652
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5DB->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp58C=(struct _tuple24*)x->hd;struct _tuple24*_tmp58D=_tmp58C;struct Cyc_Absyn_Vardecl**_tmp58F;struct Cyc_Absyn_Exp*_tmp58E;_LL53: _tmp58F=_tmp58D->f1;_tmp58E=_tmp58D->f2;_LL54:;
if(_tmp58F != 0){
({union Cyc_CfFlowInfo_FlowInfo _tmp80D=({struct Cyc_CfFlowInfo_FlowEnv*_tmp80C=_tmp550;struct Cyc_NewControlFlow_AnalEnv*_tmp80B=env;union Cyc_CfFlowInfo_FlowInfo _tmp80A=_tmp591;struct Cyc_Absyn_Vardecl*_tmp809=*_tmp58F;struct Cyc_Absyn_Exp*_tmp808=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5DC))->hd;void*_tmp807=(void*)((struct Cyc_List_List*)_check_null(_tmp590))->hd;Cyc_NewControlFlow_do_initialize_var(_tmp80C,_tmp80B,_tmp80A,_tmp809,_tmp808,_tmp807,s->loc);});_tmp591=_tmp80D;});
_tmp590=_tmp590->tl;
_tmp5DC=_tmp5DC->tl;}}}
# 2660
Cyc_NewControlFlow_update_flow(env,_tmp5DB->body,_tmp591);
return Cyc_CfFlowInfo_BottomFL();};}}else{_LL2A: _LL2B:
# 2827
({void*_tmp5C6=0U;({struct _dyneither_ptr _tmp80E=({const char*_tmp5C7="anal_stmt: bad stmt syntax or unimplemented stmt form";_tag_dyneither(_tmp5C7,sizeof(char),54U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp80E,_tag_dyneither(_tmp5C6,sizeof(void*),0U));});});}case 6U: _LL18: _LL19:
# 2666
 if(((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->succ_table,s)== 0){
{union Cyc_CfFlowInfo_FlowInfo _tmp592=inflow;if((_tmp592.ReachableFL).tag == 2){_LL56: _LL57:
# 2669
{void*_tmp593=Cyc_Tcutil_compress(env->return_type);void*_tmp594=_tmp593;switch(*((int*)_tmp594)){case 0U: _LL5B: _LL5C:
 goto _LL5A;case 7U: _LL5D: _LL5E:
 goto _LL60;case 6U: _LL5F: _LL60:
# 2673
({void*_tmp595=0U;({unsigned int _tmp810=s->loc;struct _dyneither_ptr _tmp80F=({const char*_tmp596="break may cause function not to return a value";_tag_dyneither(_tmp596,sizeof(char),47U);});Cyc_Warn_warn(_tmp810,_tmp80F,_tag_dyneither(_tmp595,sizeof(void*),0U));});});
goto _LL5A;default: _LL61: _LL62:
# 2676
({void*_tmp597=0U;({unsigned int _tmp812=s->loc;struct _dyneither_ptr _tmp811=({const char*_tmp598="break may cause function not to return a value";_tag_dyneither(_tmp598,sizeof(char),47U);});Cyc_CfFlowInfo_aerr(_tmp812,_tmp811,_tag_dyneither(_tmp597,sizeof(void*),0U));});});
goto _LL5A;}_LL5A:;}
# 2679
goto _LL55;}else{_LL58: _LL59:
 goto _LL55;}_LL55:;}
# 2682
if(env->noreturn)
({void*_tmp599=0U;({unsigned int _tmp814=s->loc;struct _dyneither_ptr _tmp813=({const char*_tmp59A="`noreturn' function might return";_tag_dyneither(_tmp59A,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp814,_tmp813,_tag_dyneither(_tmp599,sizeof(void*),0U));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}
# 2688
goto _LL1B;case 7U: _LL1A: _LL1B:
 goto _LL1D;case 8U: _LL1C: _LL1D: {
# 2692
struct Cyc_Absyn_Stmt*_tmp59B=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->succ_table,s);
if(_tmp59B == 0)
({void*_tmp59C=0U;({unsigned int _tmp816=s->loc;struct _dyneither_ptr _tmp815=({const char*_tmp59D="jump has no target (should be impossible)";_tag_dyneither(_tmp59D,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp816,_tmp815,_tag_dyneither(_tmp59C,sizeof(void*),0U));});});
({union Cyc_CfFlowInfo_FlowInfo _tmp817=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,(struct Cyc_Absyn_Stmt*)_check_null(_tmp59B));inflow=_tmp817;});
# 2697
Cyc_NewControlFlow_update_flow(env,_tmp59B,inflow);
return Cyc_CfFlowInfo_BottomFL();}case 10U: _LL1E: _tmp5DF=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp552)->f1;_tmp5DE=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp552)->f2;_tmp5DD=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp552)->f3;_LL1F:
# 2703
 return Cyc_NewControlFlow_anal_scs(env,inflow,_tmp5DE,_tmp5DF->loc);case 15U: _LL20: _tmp5E2=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp552)->f1;_tmp5E1=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp552)->f2;_tmp5E0=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp552)->f3;_LL21: {
# 2708
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5E2,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2716
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2719
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp5E1,0U);
{union Cyc_CfFlowInfo_FlowInfo _tmp59E=scs_outflow;if((_tmp59E.BottomFL).tag == 1){_LL64: _LL65:
 goto _LL63;}else{_LL66: _LL67:
({void*_tmp59F=0U;({unsigned int _tmp819=s->loc;struct _dyneither_ptr _tmp818=({const char*_tmp5A0="last catch clause may implicitly fallthru";_tag_dyneither(_tmp5A0,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp819,_tmp818,_tag_dyneither(_tmp59F,sizeof(void*),0U));});});}_LL63:;}
# 2725
outflow=s1_outflow;
# 2727
return outflow;};};}case 12U: switch(*((int*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp552)->f1)->r)){case 2U: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp552)->f1)->r)->f2 != 0){_LL22: _tmp5E6=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp552)->f1)->r)->f2)->v;_tmp5E5=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp552)->f1)->r)->f3;_tmp5E4=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp552)->f1)->loc;_tmp5E3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp552)->f2;_LL23: {
# 2737
struct _tuple1 _tmp5A1=Cyc_NewControlFlow_get_unconsume_pat_vars(_tmp5E6);struct _tuple1 _tmp5A2=_tmp5A1;struct Cyc_List_List*_tmp5A7;struct Cyc_List_List*_tmp5A6;_LL69: _tmp5A7=_tmp5A2.f1;_tmp5A6=_tmp5A2.f2;_LL6A:;
({union Cyc_CfFlowInfo_FlowInfo _tmp81A=Cyc_NewControlFlow_initialize_pat_vars(_tmp550,env,inflow,_tmp5E6,_tmp5A7 != 0,_tmp5E4,_tmp5E5->loc);inflow=_tmp81A;});{
struct Cyc_List_List*_tmp5A3=env->unique_pat_vars;
# 2741
({struct Cyc_List_List*_tmp81C=({struct Cyc_List_List*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));({struct _tuple29*_tmp81B=({struct _tuple29*_tmp5A5=_cycalloc(sizeof(*_tmp5A5));_tmp5A5->f1=0;_tmp5A5->f2=s;_tmp5A5->f3=_tmp5A7;_tmp5A5->f4=_tmp5A6;_tmp5A5;});_tmp5A4->hd=_tmp81B;});_tmp5A4->tl=_tmp5A3;_tmp5A4;});env->unique_pat_vars=_tmp81C;});
# 2746
({union Cyc_CfFlowInfo_FlowInfo _tmp81D=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5E3,rval_opt);inflow=_tmp81D;});
env->unique_pat_vars=_tmp5A3;
# 2750
({union Cyc_CfFlowInfo_FlowInfo _tmp81E=Cyc_NewControlFlow_unconsume_params(env,_tmp5A7,_tmp5A6,0,inflow,_tmp5E4);inflow=_tmp81E;});
# 2754
return inflow;};}}else{goto _LL26;}case 4U: _LL24: _tmp5EA=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp552)->f1)->r)->f2;_tmp5E9=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp552)->f1)->r)->f3;_tmp5E8=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp552)->f1)->loc;_tmp5E7=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp552)->f2;if(_tmp5E9 != 0){_LL25: {
# 2766
struct Cyc_List_List l=({struct Cyc_List_List _tmp5C5;_tmp5C5.hd=_tmp5E9;_tmp5C5.tl=0;_tmp5C5;});
union Cyc_CfFlowInfo_FlowInfo _tmp5A8=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp5A9=Cyc_NewControlFlow_anal_Lexp(env,_tmp5A8,0,0,_tmp5E9);struct _tuple18 _tmp5AA=_tmp5A9;union Cyc_CfFlowInfo_AbsLVal _tmp5C4;_LL6C: _tmp5C4=_tmp5AA.f2;_LL6D:;{
struct _tuple16 _tmp5AB=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp5A8,_tmp5E9);struct _tuple16 _tmp5AC=_tmp5AB;union Cyc_CfFlowInfo_FlowInfo _tmp5C3;_LL6F: _tmp5C3=_tmp5AC.f1;_LL70:;{
struct Cyc_List_List*_tmp5AD=0;
struct Cyc_List_List*_tmp5AE=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp5AF=_tmp5C3;struct Cyc_Dict_Dict _tmp5BD;struct Cyc_List_List*_tmp5BC;if((_tmp5AF.ReachableFL).tag == 2){_LL72: _tmp5BD=((_tmp5AF.ReachableFL).val).f1;_tmp5BC=((_tmp5AF.ReachableFL).val).f2;_LL73:
# 2774
{union Cyc_CfFlowInfo_AbsLVal _tmp5B0=_tmp5C4;struct Cyc_CfFlowInfo_Place*_tmp5BB;if((_tmp5B0.PlaceL).tag == 1){_LL77: _tmp5BB=(_tmp5B0.PlaceL).val;_LL78: {
# 2778
void*_tmp5B1=Cyc_CfFlowInfo_lookup_place(_tmp5BD,_tmp5BB);
({void*_tmp820=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5B2=_cycalloc(sizeof(*_tmp5B2));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp81F=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp5B3;_tmp5B3.tag=8U;_tmp5B3.f1=_tmp5EA;_tmp5B3.f2=_tmp5B1;_tmp5B3;});_tmp5B2[0]=_tmp81F;});_tmp5B2;});_tmp5B1=_tmp820;});
({struct Cyc_Dict_Dict _tmp821=Cyc_CfFlowInfo_assign_place(_tmp550,_tmp5E9->loc,_tmp5BD,_tmp5BB,_tmp5B1);_tmp5BD=_tmp821;});
({union Cyc_CfFlowInfo_FlowInfo _tmp822=Cyc_CfFlowInfo_ReachableFL(_tmp5BD,_tmp5BC);_tmp5C3=_tmp822;});{
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp5B4=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp5B9=_cycalloc(sizeof(*_tmp5B9));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp823=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp5BA;_tmp5BA.tag=0U;_tmp5BA.f1=_tmp5EA;_tmp5BA;});_tmp5B9[0]=_tmp823;});_tmp5B9;});
struct Cyc_CfFlowInfo_Place*_tmp5B5=({struct Cyc_CfFlowInfo_Place*_tmp5B8=_cycalloc(sizeof(*_tmp5B8));_tmp5B8->root=(void*)_tmp5B4;_tmp5B8->path=0;_tmp5B8;});
({struct Cyc_List_List*_tmp824=({struct Cyc_List_List*_tmp5B6=_cycalloc(sizeof(*_tmp5B6));_tmp5B6->hd=_tmp5B5;_tmp5B6->tl=_tmp5AD;_tmp5B6;});_tmp5AD=_tmp824;});
({struct Cyc_List_List*_tmp825=({struct Cyc_List_List*_tmp5B7=_cycalloc(sizeof(*_tmp5B7));_tmp5B7->hd=_tmp5E9;_tmp5B7->tl=_tmp5AE;_tmp5B7;});_tmp5AE=_tmp825;});
goto _LL76;};}}else{_LL79: _LL7A:
# 2793
 goto _LL76;}_LL76:;}
# 2795
goto _LL71;}else{_LL74: _LL75:
# 2797
 goto _LL71;}_LL71:;}{
# 2800
struct Cyc_List_List _tmp5BE=({struct Cyc_List_List _tmp5C2;_tmp5C2.hd=_tmp5EA;_tmp5C2.tl=0;_tmp5C2;});
({union Cyc_CfFlowInfo_FlowInfo _tmp826=Cyc_NewControlFlow_add_vars(_tmp550,_tmp5C3,& _tmp5BE,_tmp550->unknown_all,_tmp5E8,0);_tmp5C3=_tmp826;});{
# 2804
struct Cyc_List_List*_tmp5BF=env->unique_pat_vars;
({struct Cyc_List_List*_tmp828=({struct Cyc_List_List*_tmp5C0=_cycalloc(sizeof(*_tmp5C0));({struct _tuple29*_tmp827=({struct _tuple29*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));_tmp5C1->f1=1;_tmp5C1->f2=s;_tmp5C1->f3=_tmp5AD;_tmp5C1->f4=_tmp5AE;_tmp5C1;});_tmp5C0->hd=_tmp827;});_tmp5C0->tl=_tmp5BF;_tmp5C0;});env->unique_pat_vars=_tmp828;});
# 2809
({union Cyc_CfFlowInfo_FlowInfo _tmp829=Cyc_NewControlFlow_anal_stmt(env,_tmp5C3,_tmp5E7,rval_opt);_tmp5C3=_tmp829;});
env->unique_pat_vars=_tmp5BF;
# 2813
({union Cyc_CfFlowInfo_FlowInfo _tmp82A=Cyc_NewControlFlow_unconsume_params(env,_tmp5AD,_tmp5AE,1,_tmp5C3,_tmp5E8);_tmp5C3=_tmp82A;});
# 2817
return _tmp5C3;};};};};}}else{goto _LL26;}default: _LL26: _tmp5EC=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp552)->f1;_tmp5EB=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp552)->f2;_LL27:
# 2820
({union Cyc_CfFlowInfo_FlowInfo _tmp82E=({struct Cyc_NewControlFlow_AnalEnv*_tmp82D=env;union Cyc_CfFlowInfo_FlowInfo _tmp82C=Cyc_NewControlFlow_anal_decl(env,inflow,_tmp5EC);struct Cyc_Absyn_Stmt*_tmp82B=_tmp5EB;Cyc_NewControlFlow_anal_stmt(_tmp82D,_tmp82C,_tmp82B,rval_opt);});outflow=_tmp82E;});
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp5EC);
return outflow;}default: _LL28: _tmp5ED=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp552)->f2;_LL29:
# 2825
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5ED,rval_opt);}_LL3:;};}
# 2831
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2835
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp5F0=env->fenv;
void*_tmp5F1=decl->r;void*_tmp5F2=_tmp5F1;struct Cyc_Absyn_Tvar*_tmp607;struct Cyc_Absyn_Vardecl*_tmp606;struct Cyc_Absyn_Exp*_tmp605;struct Cyc_Absyn_Fndecl*_tmp604;struct Cyc_List_List*_tmp603;struct Cyc_Absyn_Vardecl*_tmp602;switch(*((int*)_tmp5F2)){case 0U: _LL1: _tmp602=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp5F2)->f1;_LL2: {
# 2845
struct Cyc_List_List _tmp5F3=({struct Cyc_List_List _tmp5F9;_tmp5F9.hd=_tmp602;_tmp5F9.tl=0;_tmp5F9;});
({union Cyc_CfFlowInfo_FlowInfo _tmp82F=Cyc_NewControlFlow_add_vars(_tmp5F0,inflow,& _tmp5F3,_tmp5F0->unknown_none,decl->loc,0);inflow=_tmp82F;});{
struct Cyc_Absyn_Exp*_tmp5F4=_tmp602->initializer;
if(_tmp5F4 == 0)
return inflow;{
struct _tuple16 _tmp5F5=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp5F4);struct _tuple16 _tmp5F6=_tmp5F5;union Cyc_CfFlowInfo_FlowInfo _tmp5F8;void*_tmp5F7;_LLC: _tmp5F8=_tmp5F6.f1;_tmp5F7=_tmp5F6.f2;_LLD:;
return Cyc_NewControlFlow_do_initialize_var(_tmp5F0,env,_tmp5F8,_tmp602,_tmp5F4,_tmp5F7,decl->loc);};};}case 3U: _LL3: _tmp603=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp5F2)->f1;_LL4:
# 2854
 return Cyc_NewControlFlow_add_vars(_tmp5F0,inflow,_tmp603,_tmp5F0->unknown_none,decl->loc,0);case 1U: _LL5: _tmp604=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp5F2)->f1;_LL6:
# 2857
 Cyc_NewControlFlow_check_nested_fun(env->all_tables,_tmp5F0,inflow,_tmp604);{
void*t=(void*)_check_null(_tmp604->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp5FA=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp604->fn_vardecl);
# 2863
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp833=_tmp5F0;union Cyc_CfFlowInfo_FlowInfo _tmp832=inflow;struct Cyc_List_List*_tmp831=({struct Cyc_List_List*_tmp5FB=_cycalloc(sizeof(*_tmp5FB));_tmp5FB->hd=_tmp5FA;_tmp5FB->tl=0;_tmp5FB;});void*_tmp830=_tmp5F0->unknown_all;Cyc_NewControlFlow_add_vars(_tmp833,_tmp832,_tmp831,_tmp830,decl->loc,0);});};case 4U: _LL7: _tmp607=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5F2)->f1;_tmp606=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5F2)->f2;_tmp605=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5F2)->f3;_LL8:
# 2866
 if(_tmp605 != 0)
({void*_tmp5FC=0U;({struct _dyneither_ptr _tmp834=({const char*_tmp5FD="found open expression in declaration!";_tag_dyneither(_tmp5FD,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp834,_tag_dyneither(_tmp5FC,sizeof(void*),0U));});});{
struct Cyc_List_List _tmp5FE=({struct Cyc_List_List _tmp5FF;_tmp5FF.hd=_tmp606;_tmp5FF.tl=0;_tmp5FF;});
return Cyc_NewControlFlow_add_vars(_tmp5F0,inflow,& _tmp5FE,_tmp5F0->unknown_all,decl->loc,0);};default: _LL9: _LLA:
# 2872
({void*_tmp600=0U;({struct _dyneither_ptr _tmp835=({const char*_tmp601="anal_decl: unexpected decl variant";_tag_dyneither(_tmp601,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp835,_tag_dyneither(_tmp600,sizeof(void*),0U));});});}_LL0:;}
# 2880
static void Cyc_NewControlFlow_check_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd){
struct _handler_cons _tmp608;_push_handler(& _tmp608);{int _tmp60A=0;if(setjmp(_tmp608.handler))_tmp60A=1;if(!_tmp60A){
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env();
({struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmp838=tables;struct Cyc_CfFlowInfo_FlowEnv*_tmp837=fenv;union Cyc_CfFlowInfo_FlowInfo _tmp836=Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0);Cyc_NewControlFlow_check_nested_fun(_tmp838,_tmp837,_tmp836,fd);});}
# 2882
;_pop_handler();}else{void*_tmp609=(void*)_exn_thrown;void*_tmp60B=_tmp609;void*_tmp60E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp60B)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 2888
 if(Cyc_Position_num_errors > 0)
goto _LL0;else{
Cyc_Core_rethrow((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp60C=_cycalloc_atomic(sizeof(*_tmp60C));({struct Cyc_Dict_Absent_exn_struct _tmp839=({struct Cyc_Dict_Absent_exn_struct _tmp60D;_tmp60D.tag=Cyc_Dict_Absent;_tmp60D;});_tmp60C[0]=_tmp839;});_tmp60C;}));}}else{_LL3: _tmp60E=_tmp60B;_LL4:(int)_rethrow(_tmp60E);}_LL0:;}};}
# 2894
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2899
static union Cyc_Relations_RelnOp Cyc_NewControlFlow_translate_rop(struct Cyc_List_List*vds,union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp60F=r;unsigned int _tmp611;if((_tmp60F.RParam).tag == 5){_LL1: _tmp611=(_tmp60F.RParam).val;_LL2: {
# 2902
struct Cyc_Absyn_Vardecl*_tmp610=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(vds,(int)_tmp611);
if(!_tmp610->escapes)
return Cyc_Relations_RVar(_tmp610);
return r;}}else{_LL3: _LL4:
 return r;}_LL0:;}
# 2910
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2914
unsigned int _tmp612=(fd->body)->loc;
({union Cyc_CfFlowInfo_FlowInfo _tmp83A=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp612,1);inflow=_tmp83A;});{
# 2919
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_roots=0;
struct _tuple14 _tmp613=({union Cyc_CfFlowInfo_FlowInfo _tmp64B=inflow;if((_tmp64B.ReachableFL).tag != 2)_throw_match();(_tmp64B.ReachableFL).val;});struct _tuple14 _tmp614=_tmp613;struct Cyc_Dict_Dict _tmp64A;struct Cyc_List_List*_tmp649;_LL1: _tmp64A=_tmp614.f1;_tmp649=_tmp614.f2;_LL2:;{
# 2924
struct Cyc_List_List*_tmp615=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;
{struct Cyc_List_List*_tmp616=fd->requires_relns;for(0;_tmp616 != 0;_tmp616=_tmp616->tl){
struct Cyc_Relations_Reln*_tmp617=(struct Cyc_Relations_Reln*)_tmp616->hd;
({struct Cyc_List_List*_tmp83F=({struct _RegionHandle*_tmp83E=Cyc_Core_heap_region;union Cyc_Relations_RelnOp _tmp83D=Cyc_NewControlFlow_translate_rop(_tmp615,_tmp617->rop1);enum Cyc_Relations_Relation _tmp83C=_tmp617->relation;union Cyc_Relations_RelnOp _tmp83B=
Cyc_NewControlFlow_translate_rop(_tmp615,_tmp617->rop2);
# 2927
Cyc_Relations_add_relation(_tmp83E,_tmp83D,_tmp83C,_tmp83B,_tmp649);});_tmp649=_tmp83F;});}}{
# 2933
struct Cyc_List_List*atts;
{void*_tmp618=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));void*_tmp619=_tmp618;struct Cyc_List_List*_tmp61C;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp619)->tag == 9U){_LL4: _tmp61C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp619)->f1).attributes;_LL5:
 atts=_tmp61C;goto _LL3;}else{_LL6: _LL7:
({void*_tmp61A=0U;({struct _dyneither_ptr _tmp840=({const char*_tmp61B="check_fun: non-function type cached with fndecl_t";_tag_dyneither(_tmp61B,sizeof(char),50U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp840,_tag_dyneither(_tmp61A,sizeof(void*),0U));});});}_LL3:;}
# 2940
for(0;atts != 0;atts=atts->tl){
void*_tmp61D=(void*)atts->hd;void*_tmp61E=_tmp61D;int _tmp63E;int _tmp63D;int _tmp63C;switch(*((int*)_tmp61E)){case 21U: _LL9: _tmp63C=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp61E)->f1;_LLA: {
# 2943
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0U);
struct Cyc_Absyn_Vardecl*_tmp61F=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp63C - 1);
void*t=Cyc_Tcutil_compress(_tmp61F->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp620=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp843=fenv;void*_tmp842=elttype;struct Cyc_Absyn_Exp*_tmp841=bogus_exp;Cyc_CfFlowInfo_make_unique_consumed(_tmp843,_tmp842,_tmp841,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));});
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp621=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp628=_cycalloc(sizeof(*_tmp628));({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp844=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp629;_tmp629.tag=2U;_tmp629.f1=_tmp63C;_tmp629.f2=t;_tmp629;});_tmp628[0]=_tmp844;});_tmp628;});
struct Cyc_CfFlowInfo_Place*_tmp622=({struct Cyc_CfFlowInfo_Place*_tmp627=_cycalloc(sizeof(*_tmp627));_tmp627->root=(void*)_tmp621;_tmp627->path=0;_tmp627;});
({struct Cyc_Dict_Dict _tmp845=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp64A,(void*)_tmp621,_tmp620);_tmp64A=_tmp845;});
({struct Cyc_Dict_Dict _tmp84A=({struct Cyc_Dict_Dict _tmp849=_tmp64A;void*_tmp848=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp623=_cycalloc(sizeof(*_tmp623));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp846=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp624;_tmp624.tag=0U;_tmp624.f1=_tmp61F;_tmp624;});_tmp623[0]=_tmp846;});_tmp623;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp849,_tmp848,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp625=_cycalloc(sizeof(*_tmp625));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp847=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp626;_tmp626.tag=4U;_tmp626.f1=_tmp622;_tmp626;});_tmp625[0]=_tmp847;});_tmp625;}));});_tmp64A=_tmp84A;});
goto _LL8;}case 20U: _LLB: _tmp63D=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp61E)->f1;_LLC: {
# 2956
struct Cyc_Absyn_Vardecl*_tmp62A=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp63D - 1);
void*elttype=Cyc_Tcutil_pointer_elt_type(_tmp62A->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp62B=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp633=_cycalloc(sizeof(*_tmp633));({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp84B=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp634;_tmp634.tag=2U;_tmp634.f1=_tmp63D;_tmp634.f2=elttype;_tmp634;});_tmp633[0]=_tmp84B;});_tmp633;});
struct Cyc_CfFlowInfo_Place*_tmp62C=({struct Cyc_CfFlowInfo_Place*_tmp632=_cycalloc(sizeof(*_tmp632));_tmp632->root=(void*)_tmp62B;_tmp632->path=0;_tmp632;});
({struct Cyc_Dict_Dict _tmp84E=({struct Cyc_Dict_Dict _tmp84D=_tmp64A;void*_tmp84C=(void*)_tmp62B;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp84D,_tmp84C,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));});_tmp64A=_tmp84E;});
({struct Cyc_Dict_Dict _tmp853=({struct Cyc_Dict_Dict _tmp852=_tmp64A;void*_tmp851=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp62D=_cycalloc(sizeof(*_tmp62D));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp84F=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp62E;_tmp62E.tag=0U;_tmp62E.f1=_tmp62A;_tmp62E;});_tmp62D[0]=_tmp84F;});_tmp62D;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp852,_tmp851,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp62F=_cycalloc(sizeof(*_tmp62F));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp850=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp630;_tmp630.tag=4U;_tmp630.f1=_tmp62C;_tmp630;});_tmp62F[0]=_tmp850;});_tmp62F;}));});_tmp64A=_tmp853;});
({struct Cyc_List_List*_tmp854=({struct Cyc_List_List*_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631->hd=_tmp62C;_tmp631->tl=param_roots;_tmp631;});param_roots=_tmp854;});
goto _LL8;}case 22U: _LLD: _tmp63E=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp61E)->f1;_LLE: {
# 2965
struct Cyc_Absyn_Vardecl*_tmp635=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp63E - 1);
if(Cyc_Tcutil_is_noalias_pointer(_tmp635->type,0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp636=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp63A=_cycalloc(sizeof(*_tmp63A));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp855=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp63B;_tmp63B.tag=0U;_tmp63B.f1=_tmp635;_tmp63B;});_tmp63A[0]=_tmp855;});_tmp63A;});
struct Cyc_CfFlowInfo_Place*_tmp637=({struct Cyc_CfFlowInfo_Place*_tmp639=_cycalloc(sizeof(*_tmp639));_tmp639->root=(void*)_tmp636;_tmp639->path=0;_tmp639;});
({struct Cyc_List_List*_tmp856=({struct Cyc_List_List*_tmp638=_cycalloc(sizeof(*_tmp638));_tmp638->hd=_tmp637;_tmp638->tl=noconsume_roots;_tmp638;});noconsume_roots=_tmp856;});}
# 2971
goto _LL8;}default: _LLF: _LL10:
 goto _LL8;}_LL8:;}
# 2974
({union Cyc_CfFlowInfo_FlowInfo _tmp857=Cyc_CfFlowInfo_ReachableFL(_tmp64A,_tmp649);inflow=_tmp857;});{
# 2976
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create)(33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*_tmp648=_cycalloc(sizeof(*_tmp648));_tmp648->all_tables=tables;({struct Cyc_Hashtable_Table*_tmp858=
# 2981
((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->succ_tables,fd);_tmp648->succ_table=_tmp858;});({struct Cyc_Hashtable_Table*_tmp859=
((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->pat_pop_tables,fd);_tmp648->pat_pop_table=_tmp859;});_tmp648->fenv=fenv;_tmp648->iterate_again=1;_tmp648->iteration_num=0;_tmp648->in_try=0;_tmp648->tryflow=inflow;_tmp648->noreturn=noreturn;_tmp648->return_type=fd->ret_type;_tmp648->unique_pat_vars=0;_tmp648->param_roots=param_roots;_tmp648->noconsume_params=noconsume_roots;_tmp648->flow_table=flow_table;_tmp648->return_relns=fd->ensures_relns;_tmp648;});
# 2986
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 2992
env->iterate_again=0;
({union Cyc_CfFlowInfo_FlowInfo _tmp85A=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);outflow=_tmp85A;});}{
# 2995
union Cyc_CfFlowInfo_FlowInfo _tmp63F=outflow;if((_tmp63F.BottomFL).tag == 1){_LL12: _LL13:
 goto _LL11;}else{_LL14: _LL15:
# 2998
 Cyc_NewControlFlow_check_init_params(_tmp612,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,outflow,_tmp612);
# 3002
if(noreturn)
({void*_tmp640=0U;({unsigned int _tmp85C=_tmp612;struct _dyneither_ptr _tmp85B=({const char*_tmp641="`noreturn' function might (implicitly) return";_tag_dyneither(_tmp641,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp85C,_tmp85B,_tag_dyneither(_tmp640,sizeof(void*),0U));});});else{
# 3005
void*_tmp642=Cyc_Tcutil_compress(fd->ret_type);void*_tmp643=_tmp642;switch(*((int*)_tmp643)){case 0U: _LL17: _LL18:
 goto _LL16;case 7U: _LL19: _LL1A:
 goto _LL1C;case 6U: _LL1B: _LL1C:
# 3009
({void*_tmp644=0U;({unsigned int _tmp85E=_tmp612;struct _dyneither_ptr _tmp85D=({const char*_tmp645="function may not return a value";_tag_dyneither(_tmp645,sizeof(char),32U);});Cyc_Warn_warn(_tmp85E,_tmp85D,_tag_dyneither(_tmp644,sizeof(void*),0U));});});goto _LL16;default: _LL1D: _LL1E:
# 3011
({void*_tmp646=0U;({unsigned int _tmp860=_tmp612;struct _dyneither_ptr _tmp85F=({const char*_tmp647="function may not return a value";_tag_dyneither(_tmp647,sizeof(char),32U);});Cyc_CfFlowInfo_aerr(_tmp860,_tmp85F,_tag_dyneither(_tmp646,sizeof(void*),0U));});});goto _LL16;}_LL16:;}
# 3013
goto _LL11;}_LL11:;};};};};};}
# 3017
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_tmp64C=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_tmp64D=_tmp64C;struct Cyc_List_List*_tmp651;struct Cyc_List_List*_tmp650;struct Cyc_List_List*_tmp64F;struct Cyc_Absyn_Fndecl*_tmp64E;switch(*((int*)_tmp64D)){case 1U: _LL1: _tmp64E=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp64D)->f1;_LL2:
 Cyc_NewControlFlow_check_fun(tables,_tmp64E);goto _LL0;case 11U: _LL3: _tmp64F=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp64D)->f1;_LL4:
 _tmp650=_tmp64F;goto _LL6;case 10U: _LL5: _tmp650=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp64D)->f2;_LL6:
 _tmp651=_tmp650;goto _LL8;case 9U: _LL7: _tmp651=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp64D)->f2;_LL8:
 Cyc_NewControlFlow_cf_check(tables,_tmp651);goto _LL0;case 12U: _LL9: _LLA:
 goto _LL0;default: _LLB: _LLC:
 goto _LL0;}_LL0:;};}}
