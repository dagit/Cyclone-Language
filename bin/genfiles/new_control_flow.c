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
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 113 "core.h"
void*Cyc_Core_snd(struct _tuple0*);
# 128
int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 265 "core.h"
void Cyc_Core_rethrow(void*);
# 290 "core.h"
typedef void*Cyc_Core___cyclone_internal_array_t;
typedef void*Cyc_Core___nn_cyclone_internal_array_t;
typedef unsigned int Cyc_Core___cyclone_internal_singleton;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 72
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 175
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple1 Cyc_List_split(struct Cyc_List_List*x);
# 319
int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 113
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
# 126 "dict.h"
int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**ans);
# 149
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;
# 37
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
# 47
extern int Cyc_Position_num_errors;struct Cyc_Relations_Reln;
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 102
typedef struct _tuple2*Cyc_Absyn_qvar_t;typedef struct _tuple2*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_opt_t;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 367
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 372
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 443 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 510
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
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
# 30 "rgnorder.h"
typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;
# 32
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
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 48 "tcenv.h"
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 52
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;};
# 62
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 84
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 32
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 34
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 55
void*Cyc_Tcutil_compress(void*t);
# 90
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 150
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 163
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 172
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 233 "tcutil.h"
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 265
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 275
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);
# 326
int Cyc_Tcutil_is_noreturn(void*);
# 344
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 348
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 351
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);struct _tuple12{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 38 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 40
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 49
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct _tuple13{struct Cyc_Absyn_Exp*f1;enum Cyc_Relations_Relation f2;struct Cyc_Absyn_Exp*f3;};
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
# 109
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
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
# 35 "hashtable.h"
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
# 47
struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 56
void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key);
# 29 "jump_analysis.h"
typedef struct Cyc_Hashtable_Table*Cyc_JumpAnalysis_table_t;struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 44 "jump_analysis.h"
typedef struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_anal_res_t;
# 46
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds);
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 52
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};
# 58
typedef void*Cyc_CfFlowInfo_pathcon_t;
typedef struct Cyc_List_List*Cyc_CfFlowInfo_path_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 72
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_AllIL  = 1};
# 78
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 87
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 93
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();
# 98
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef void*Cyc_CfFlowInfo_absRval_opt_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 106
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 137 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 152
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);
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
struct _tuple15 Cyc_CfFlowInfo_unname_rval(struct _RegionHandle*rgn,void*rv);
# 169
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
# 172
int Cyc_CfFlowInfo_is_init_pointer(void*r);
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 175
void Cyc_CfFlowInfo_print_absrval(void*rval);
# 182
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f);
# 197 "cf_flowinfo.h"
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,void*r);
# 200
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place,void*r);
# 204
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple16{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple16 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct _tuple16 pr1,struct _tuple16 pr2);
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds);
# 37
extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 49 "tcpat.h"
typedef struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcpat_result_t;struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};
# 70
typedef void*Cyc_Tcpat_pat_test_t;struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned int f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _dyneither_ptr*f2;};
# 78
typedef void*Cyc_Tcpat_access_t;struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};
# 88
typedef struct Cyc_Tcpat_PathNode*Cyc_Tcpat_path_node_t;
# 90
typedef void*Cyc_Tcpat_term_desc_t;
typedef struct Cyc_List_List*Cyc_Tcpat_path_t;struct Cyc_Tcpat_Rhs{int used;unsigned int pat_loc;struct Cyc_Absyn_Stmt*rhs;};
# 98
typedef struct Cyc_Tcpat_Rhs*Cyc_Tcpat_rhs_t;
# 100
typedef void*Cyc_Tcpat_decision_t;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 115
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 67 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 47 "new_control_flow.cyc"
typedef struct Cyc_Dict_Dict Cyc_NewControlFlow_dict_t;
# 50
static int Cyc_NewControlFlow_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2){
if((char*)s1.curr == (char*)s2.curr)
return 0;{
int i=0;
unsigned int sz1=_get_dyneither_size(s1,sizeof(char));
unsigned int sz2=_get_dyneither_size(s2,sizeof(char));
unsigned int minsz=sz1 < sz2?sz1: sz2;
# 58
while(i < minsz){
char c1=*((const char*)_check_dyneither_subscript(s1,sizeof(char),i));
char c2=*((const char*)_check_dyneither_subscript(s2,sizeof(char),i));
if(c1 == '\000'){
if(c2 == '\000')return 0;else{
return - 1;}}else{
if(c2 == '\000')return 1;else{
# 66
int diff=c1 - c2;
if(diff != 0)return diff;}}
# 69
++ i;}
# 71
if(sz1 == sz2)return 0;
if(minsz < sz2){
if(*((const char*)_check_dyneither_subscript(s2,sizeof(char),i))== '\000')return 0;else{
return - 1;}}else{
# 76
if(*((const char*)_check_dyneither_subscript(s1,sizeof(char),i))== '\000')return 0;else{
return 1;}}};}
# 83
int Cyc_NewControlFlow_warn_lose_unique=0;struct Cyc_NewControlFlow_CFStmtAnnot{int visited;};
# 90
typedef struct Cyc_NewControlFlow_CFStmtAnnot Cyc_NewControlFlow_cf_stmt_annot_t;static char Cyc_NewControlFlow_CFAnnot[8]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};struct Cyc_NewControlFlow_AnalEnv{struct Cyc_JumpAnalysis_Jump_Anal_Result*all_tables;struct Cyc_Hashtable_Table*succ_table;struct Cyc_Hashtable_Table*pat_pop_table;struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;int noreturn;void*return_type;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;struct Cyc_List_List*return_relns;};
# 124 "new_control_flow.cyc"
typedef struct Cyc_NewControlFlow_AnalEnv*Cyc_NewControlFlow_analenv_t;struct _tuple17{void*f1;int f2;};
# 132
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*,struct _tuple17*);
# 134
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple18{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal f2;};
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*);
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple19{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t);
# 141
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es);
# 146
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){
void*_tmp0=s->annot;void*_tmp1=_tmp0;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5;if(((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp1)->tag == Cyc_NewControlFlow_CFAnnot){_LL1: _tmp5=(struct Cyc_NewControlFlow_CFStmtAnnot*)&((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp1)->f1;_LL2:
 return _tmp5;}else{_LL3: _LL4:
({void*_tmp68B=(void*)({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp2=_cycalloc_atomic(sizeof(*_tmp2));({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp68A=({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp3;_tmp3.tag=Cyc_NewControlFlow_CFAnnot;({struct Cyc_NewControlFlow_CFStmtAnnot _tmp689=({struct Cyc_NewControlFlow_CFStmtAnnot _tmp4;_tmp4.visited=0;_tmp4;});_tmp3.f1=_tmp689;});_tmp3;});_tmp2[0]=_tmp68A;});_tmp2;});s->annot=_tmp68B;});return Cyc_NewControlFlow_get_stmt_annot(s);}_LL0:;}
# 153
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*res=({union Cyc_CfFlowInfo_FlowInfo*_tmp6=_region_malloc(env->r,sizeof(*_tmp6));({union Cyc_CfFlowInfo_FlowInfo _tmp68C=Cyc_CfFlowInfo_BottomFL();_tmp6[0]=_tmp68C;});_tmp6;});
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);
return res;}
# 160
return*sflow;}struct _tuple20{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 163
static struct _tuple20 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp7=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp8=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 171
({union Cyc_CfFlowInfo_FlowInfo _tmp68D=Cyc_CfFlowInfo_join_flow(env->fenv,inflow,*_tmp8);*_tmp8=_tmp68D;});
# 177
_tmp7->visited=env->iteration_num;
return({struct _tuple20 _tmp9;_tmp9.f1=_tmp7;_tmp9.f2=_tmp8;_tmp9;});}
# 187
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 195
({union Cyc_CfFlowInfo_FlowInfo _tmp68E=Cyc_CfFlowInfo_join_flow(env->fenv,new_flow,env->tryflow);env->tryflow=_tmp68E;});}struct _tuple21{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned int f2;struct Cyc_Dict_Dict f3;};
# 202
static void Cyc_NewControlFlow_check_unique_root(struct _tuple21*ckenv,void*root,void*rval){
# 204
struct _tuple21*_tmpA=ckenv;struct Cyc_NewControlFlow_AnalEnv*_tmp14;unsigned int _tmp13;struct Cyc_Dict_Dict _tmp12;_LL6: _tmp14=_tmpA->f1;_tmp13=_tmpA->f2;_tmp12=_tmpA->f3;_LL7:;{
void*_tmpB=root;struct Cyc_Absyn_Vardecl*_tmp11;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpB)->tag == 0){_LL9: _tmp11=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpB)->f1;_LLA:
# 207
 if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(_tmp12,root,& rval) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp11->type)){
retry: {void*_tmpC=rval;void*_tmp10;switch(*((int*)_tmpC)){case 8: _LLE: _tmp10=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpC)->f2;_LLF:
 rval=_tmp10;goto retry;case 7: _LL10: _LL11:
 goto _LL13;case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpC)->f1 == Cyc_CfFlowInfo_NoneIL){_LL12: _LL13:
 goto _LL15;}else{goto _LL16;}case 0: _LL14: _LL15:
 goto _LLD;default: _LL16: _LL17:
# 216
({struct Cyc_String_pa_PrintArg_struct _tmpF;_tmpF.tag=0;({struct _dyneither_ptr _tmp68F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp11->name));_tmpF.f1=_tmp68F;});({void*_tmpD[1]={& _tmpF};({unsigned int _tmp691=_tmp13;struct _dyneither_ptr _tmp690=({const char*_tmpE="alias-free pointer(s) reachable from %s may become unreachable.";_tag_dyneither(_tmpE,sizeof(char),64);});Cyc_Tcutil_warn(_tmp691,_tmp690,_tag_dyneither(_tmpD,sizeof(void*),1));});});});
goto _LLD;}_LLD:;}}
# 220
goto _LL8;}else{_LLB: _LLC:
 goto _LL8;}_LL8:;};}
# 228
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp15=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp16=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp17=Cyc_CfFlowInfo_join_flow(env->fenv,flow,*_tmp16);
# 234
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple19 _tmp18=({struct _tuple19 _tmp1E;_tmp1E.f1=flow;_tmp1E.f2=_tmp17;_tmp1E;});struct _tuple19 _tmp19=_tmp18;struct Cyc_Dict_Dict _tmp1D;struct Cyc_Dict_Dict _tmp1C;if(((_tmp19.f1).ReachableFL).tag == 2){if(((_tmp19.f2).ReachableFL).tag == 2){_LL19: _tmp1D=(((_tmp19.f1).ReachableFL).val).f1;_tmp1C=(((_tmp19.f2).ReachableFL).val).f1;_LL1A: {
# 237
struct _tuple21 _tmp1A=({struct _tuple21 _tmp1B;_tmp1B.f1=env;_tmp1B.f2=s->loc;_tmp1B.f3=_tmp1C;_tmp1B;});
((void(*)(void(*f)(struct _tuple21*,void*,void*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& _tmp1A,_tmp1D);
goto _LL18;}}else{goto _LL1B;}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 243
if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp17,*_tmp16)){
# 250
*_tmp16=_tmp17;
# 254
if(_tmp15->visited == env->iteration_num)
# 256
env->iterate_again=1;}}
# 261
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned int loc,int nameit){
# 265
union Cyc_CfFlowInfo_FlowInfo _tmp1F=inflow;struct Cyc_Dict_Dict _tmp27;struct Cyc_List_List*_tmp26;if((_tmp1F.BottomFL).tag == 1){_LL1E: _LL1F:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL20: _tmp27=((_tmp1F.ReachableFL).val).f1;_tmp26=((_tmp1F.ReachableFL).val).f2;_LL21:
# 268
 for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp20=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp24=_region_malloc(fenv->r,sizeof(*_tmp24));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp692=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp25;_tmp25.tag=0;_tmp25.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp25;});_tmp24[0]=_tmp692;});_tmp24;});
void*_tmp21=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit)
({void*_tmp694=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp22=_region_malloc(fenv->r,sizeof(*_tmp22));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp693=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp23;_tmp23.tag=8;_tmp23.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp23.f2=_tmp21;_tmp23;});_tmp22[0]=_tmp693;});_tmp22;});_tmp21=_tmp694;});
# 275
({struct Cyc_Dict_Dict _tmp695=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp27,(void*)_tmp20,_tmp21);_tmp27=_tmp695;});}
# 277
return Cyc_CfFlowInfo_ReachableFL(_tmp27,_tmp26);}_LL1D:;}
# 281
static int Cyc_NewControlFlow_relns_ok(struct _RegionHandle*r,struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 288
for(0;(unsigned int)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp28=Cyc_Relations_negate(r,(struct Cyc_Relations_Reln*)req->hd);
if(Cyc_Relations_consistent_relations(({struct Cyc_List_List*_tmp29=_region_malloc(r,sizeof(*_tmp29));_tmp29->hd=_tmp28;_tmp29->tl=assume;_tmp29;})))
return 0;}
# 293
return 1;}
# 296
static struct Cyc_Absyn_Exp*Cyc_NewControlFlow_strip_cast(struct Cyc_Absyn_Exp*e){
void*_tmp2A=e->r;void*_tmp2B=_tmp2A;struct Cyc_Absyn_Exp*_tmp2C;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2B)->tag == 14){_LL23: _tmp2C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2B)->f2;_LL24:
 return _tmp2C;}else{_LL25: _LL26:
 return e;}_LL22:;}
# 303
static void Cyc_NewControlFlow_check_union_requires(unsigned int loc,struct _RegionHandle*r,void*t,struct _dyneither_ptr*f,union Cyc_CfFlowInfo_FlowInfo inflow){
# 305
union Cyc_CfFlowInfo_FlowInfo _tmp2D=inflow;struct Cyc_List_List*_tmp4B;if((_tmp2D.BottomFL).tag == 1){_LL28: _LL29:
 return;}else{_LL2A: _tmp4B=((_tmp2D.ReachableFL).val).f2;_LL2B:
# 308
{void*_tmp2E=Cyc_Tcutil_compress(t);void*_tmp2F=_tmp2E;struct Cyc_List_List*_tmp4A;union Cyc_Absyn_AggrInfoU _tmp49;struct Cyc_List_List*_tmp48;switch(*((int*)_tmp2F)){case 11: _LL2D: _tmp49=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F)->f1).aggr_info;_tmp48=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F)->f1).targs;_LL2E: {
# 310
struct Cyc_Absyn_Aggrdecl*_tmp30=Cyc_Absyn_get_known_aggrdecl(_tmp49);
struct Cyc_Absyn_Aggrfield*_tmp31=Cyc_Absyn_lookup_decl_field(_tmp30,f);
struct Cyc_Absyn_Exp*_tmp32=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp31))->requires_clause;
if(_tmp32 != 0){
struct _RegionHandle _tmp33=_new_region("temp");struct _RegionHandle*temp=& _tmp33;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp34=({struct _RegionHandle*_tmp697=temp;struct Cyc_List_List*_tmp696=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp30->tvs,_tmp48);Cyc_Tcutil_rsubsexp(_tmp697,_tmp696,_tmp32);});
# 317
if(!({struct _RegionHandle*_tmp699=r;struct Cyc_List_List*_tmp698=_tmp4B;Cyc_NewControlFlow_relns_ok(_tmp699,_tmp698,Cyc_Relations_exp2relns(temp,_tmp34));})){
({struct Cyc_String_pa_PrintArg_struct _tmp38;_tmp38.tag=0;_tmp38.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp37;_tmp37.tag=0;({struct _dyneither_ptr _tmp69A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp34)));_tmp37.f1=_tmp69A;});({void*_tmp35[2]={& _tmp37,& _tmp38};({unsigned int _tmp69C=loc;struct _dyneither_ptr _tmp69B=({const char*_tmp36="unable to prove %s, required to access %s";_tag_dyneither(_tmp36,sizeof(char),42);});Cyc_CfFlowInfo_aerr(_tmp69C,_tmp69B,_tag_dyneither(_tmp35,sizeof(void*),2));});});});});
({void*_tmp39=0;({struct Cyc___cycFILE*_tmp69E=Cyc_stderr;struct _dyneither_ptr _tmp69D=({const char*_tmp3A="  [recorded facts on path: ";_tag_dyneither(_tmp3A,sizeof(char),28);});Cyc_fprintf(_tmp69E,_tmp69D,_tag_dyneither(_tmp39,sizeof(void*),0));});});
Cyc_Relations_print_relns(Cyc_stderr,_tmp4B);
({void*_tmp3B=0;({struct Cyc___cycFILE*_tmp6A0=Cyc_stderr;struct _dyneither_ptr _tmp69F=({const char*_tmp3C="]\n";_tag_dyneither(_tmp3C,sizeof(char),3);});Cyc_fprintf(_tmp6A0,_tmp69F,_tag_dyneither(_tmp3B,sizeof(void*),0));});});}}
# 315
;_pop_region(temp);}
# 324
goto _LL2C;}case 12: _LL2F: _tmp4A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2F)->f2;_LL30: {
# 326
struct Cyc_Absyn_Aggrfield*_tmp3D=Cyc_Absyn_lookup_field(_tmp4A,f);
struct Cyc_Absyn_Exp*_tmp3E=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3D))->requires_clause;
if(_tmp3E != 0){
struct _RegionHandle _tmp3F=_new_region("temp");struct _RegionHandle*temp=& _tmp3F;_push_region(temp);
if(!({struct _RegionHandle*_tmp6A2=r;struct Cyc_List_List*_tmp6A1=_tmp4B;Cyc_NewControlFlow_relns_ok(_tmp6A2,_tmp6A1,Cyc_Relations_exp2relns(temp,_tmp3E));})){
({struct Cyc_String_pa_PrintArg_struct _tmp43;_tmp43.tag=0;_tmp43.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp42;_tmp42.tag=0;({struct _dyneither_ptr _tmp6A3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp3E)));_tmp42.f1=_tmp6A3;});({void*_tmp40[2]={& _tmp42,& _tmp43};({unsigned int _tmp6A5=loc;struct _dyneither_ptr _tmp6A4=({const char*_tmp41="unable to prove %s, required to access %s";_tag_dyneither(_tmp41,sizeof(char),42);});Cyc_CfFlowInfo_aerr(_tmp6A5,_tmp6A4,_tag_dyneither(_tmp40,sizeof(void*),2));});});});});
({void*_tmp44=0;({struct Cyc___cycFILE*_tmp6A7=Cyc_stderr;struct _dyneither_ptr _tmp6A6=({const char*_tmp45="  [recorded facts on path: ";_tag_dyneither(_tmp45,sizeof(char),28);});Cyc_fprintf(_tmp6A7,_tmp6A6,_tag_dyneither(_tmp44,sizeof(void*),0));});});
Cyc_Relations_print_relns(Cyc_stderr,_tmp4B);
({void*_tmp46=0;({struct Cyc___cycFILE*_tmp6A9=Cyc_stderr;struct _dyneither_ptr _tmp6A8=({const char*_tmp47="]\n";_tag_dyneither(_tmp47,sizeof(char),3);});Cyc_fprintf(_tmp6A9,_tmp6A8,_tag_dyneither(_tmp46,sizeof(void*),0));});});}
# 330
;_pop_region(temp);}
# 337
goto _LL2C;}default: _LL31: _LL32:
 goto _LL2C;}_LL2C:;}
# 340
goto _LL27;}_LL27:;}
# 344
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp4C=inflow;struct Cyc_Dict_Dict _tmp52;struct Cyc_List_List*_tmp51;if((_tmp4C.BottomFL).tag == 1){_LL34: _LL35:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL36: _tmp52=((_tmp4C.ReachableFL).val).f1;_tmp51=((_tmp4C.ReachableFL).val).f2;_LL37:
# 348
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp52,r)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp4D=0;({unsigned int _tmp6AB=loc;struct _dyneither_ptr _tmp6AA=({const char*_tmp4E="expression may not be fully initialized";_tag_dyneither(_tmp4E,sizeof(char),40);});Cyc_CfFlowInfo_aerr(_tmp6AB,_tmp6AA,_tag_dyneither(_tmp4D,sizeof(void*),0));});});{
struct Cyc_Dict_Dict _tmp4F=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp52,r);
if(_tmp52.t == _tmp4F.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp50=Cyc_CfFlowInfo_ReachableFL(_tmp4F,_tmp51);
Cyc_NewControlFlow_update_tryflow(env,_tmp50);
return _tmp50;};};}_LL33:;}struct _tuple22{struct Cyc_Absyn_Tqual f1;void*f2;};
# 358
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp53=({struct _tuple0 _tmp66;({void*_tmp6AC=Cyc_Tcutil_compress(t);_tmp66.f1=_tmp6AC;});_tmp66.f2=r;_tmp66;});struct _tuple0 _tmp54=_tmp53;enum Cyc_Absyn_AggrKind _tmp65;struct Cyc_List_List*_tmp64;struct _dyneither_ptr _tmp63;union Cyc_Absyn_AggrInfoU _tmp62;struct Cyc_List_List*_tmp61;struct _dyneither_ptr _tmp60;struct Cyc_List_List*_tmp5F;struct _dyneither_ptr _tmp5E;struct Cyc_Absyn_Datatypefield*_tmp5D;struct _dyneither_ptr _tmp5C;void*_tmp5B;switch(*((int*)_tmp54.f2)){case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f1 == Cyc_CfFlowInfo_NoneIL){_LL39: _LL3A:
 return;}else{switch(*((int*)_tmp54.f1)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp54.f1)->f1).field_info).KnownDatatypefield).tag == 2)goto _LL4B;else{goto _LL4B;}case 10: goto _LL4B;case 11: goto _LL4B;case 12: goto _LL4B;case 5: goto _LL49;default: goto _LL4B;}}case 0: _LL3B: _LL3C:
 return;case 7: _LL3D: _LL3E:
 return;case 8: _LL3F: _tmp5B=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f2;_LL40:
 Cyc_NewControlFlow_check_nounique(env,loc,t,_tmp5B);return;default: switch(*((int*)_tmp54.f1)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp54.f1)->f1).field_info).KnownDatatypefield).tag == 2){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->tag == 6){_LL41: _tmp5D=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp54.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp5C=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f2;_LL42:
# 365
 if(_tmp5D->typs == 0)
return;
_tmp5F=_tmp5D->typs;_tmp5E=_tmp5C;goto _LL44;}else{goto _LL4B;}}else{goto _LL4B;}case 10: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->tag == 6){_LL43: _tmp5F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp54.f1)->f1;_tmp5E=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f2;_LL44: {
# 369
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5F);
{int i=0;for(0;i < sz;(i ++,_tmp5F=_tmp5F->tl)){
({struct Cyc_NewControlFlow_AnalEnv*_tmp6AF=env;unsigned int _tmp6AE=loc;void*_tmp6AD=(*((struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp5F))->hd)).f2;Cyc_NewControlFlow_check_nounique(_tmp6AF,_tmp6AE,_tmp6AD,*((void**)_check_dyneither_subscript(_tmp5E,sizeof(void*),i)));});}}
# 373
return;}}else{goto _LL4B;}case 11: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->tag == 6){_LL45: _tmp62=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp54.f1)->f1).aggr_info;_tmp61=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp54.f1)->f1).targs;_tmp60=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f2;_LL46: {
# 376
struct Cyc_Absyn_Aggrdecl*_tmp55=Cyc_Absyn_get_known_aggrdecl(_tmp62);
if(_tmp55->impl == 0)return;{
struct Cyc_List_List*_tmp56=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp55->impl))->fields;
struct _RegionHandle _tmp57=_new_region("temp");struct _RegionHandle*temp=& _tmp57;_push_region(temp);
{struct Cyc_List_List*_tmp58=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp55->tvs,_tmp61);
{int i=0;for(0;i < _get_dyneither_size(_tmp60,sizeof(void*));(i ++,_tmp56=_tmp56->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp56))->hd)->type;
if(_tmp58 != 0)({void*_tmp6B0=Cyc_Tcutil_rsubstitute(temp,_tmp58,t);t=_tmp6B0;});
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)_tmp60.curr)[i]);}}
# 386
_npop_handler(0);return;}
# 380
;_pop_region(temp);};}}else{goto _LL4B;}case 12: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->tag == 6){_LL47: _tmp65=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp54.f1)->f1;_tmp64=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp54.f1)->f2;_tmp63=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f2;_LL48:
# 389
{int i=0;for(0;i < _get_dyneither_size(_tmp63,sizeof(void*));(i ++,_tmp64=_tmp64->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp64))->hd)->type,((void**)_tmp63.curr)[i]);}}
return;}else{goto _LL4B;}case 5: _LL49: if(
Cyc_Tcutil_is_noalias_pointer(t,0)){_LL4A:
({void*_tmp59=0;({unsigned int _tmp6B2=loc;struct _dyneither_ptr _tmp6B1=({const char*_tmp5A="argument may still contain alias-free pointers";_tag_dyneither(_tmp5A,sizeof(char),47);});Cyc_Tcutil_warn(_tmp6B2,_tmp6B1,_tag_dyneither(_tmp59,sizeof(void*),0));});});
return;}else{goto _LL4B;}default: _LL4B: _LL4C:
 return;}}_LL38:;}
# 399
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp67=inflow;struct Cyc_Dict_Dict _tmp74;struct Cyc_List_List*_tmp73;if((_tmp67.BottomFL).tag == 1){_LL4E: _LL4F:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL50: _tmp74=((_tmp67.ReachableFL).val).f1;_tmp73=((_tmp67.ReachableFL).val).f2;_LL51:
# 403
 if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
({void*_tmp68=0;({struct _dyneither_ptr _tmp6B3=({const char*_tmp69="noliveunique attribute requires alias-free pointer";_tag_dyneither(_tmp69,sizeof(char),51);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp6B3,_tag_dyneither(_tmp68,sizeof(void*),0));});});
return Cyc_CfFlowInfo_BottomFL();}{
# 407
void*_tmp6A=Cyc_Tcutil_pointer_elt_type(t);
retry: {void*_tmp6B=r;void*_tmp70;struct Cyc_CfFlowInfo_Place*_tmp6F;void*_tmp6E;switch(*((int*)_tmp6B)){case 8: _LL53: _tmp6E=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp6B)->f2;_LL54:
 r=_tmp6E;goto retry;case 4: _LL55: _tmp6F=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp6B)->f1;_LL56:
({void*_tmp6B4=Cyc_CfFlowInfo_lookup_place(_tmp74,_tmp6F);_tmp70=_tmp6B4;});goto _LL58;case 5: _LL57: _tmp70=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp6B)->f1;_LL58:
 Cyc_NewControlFlow_check_nounique(env,loc,_tmp6A,_tmp70);goto _LL52;default: _LL59: _LL5A:
# 413
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp6A))
({void*_tmp6C=0;({unsigned int _tmp6B6=loc;struct _dyneither_ptr _tmp6B5=({const char*_tmp6D="argument may contain live alias-free pointers";_tag_dyneither(_tmp6D,sizeof(char),46);});Cyc_Tcutil_warn(_tmp6B6,_tmp6B5,_tag_dyneither(_tmp6C,sizeof(void*),0));});});
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);}_LL52:;}{
# 417
struct Cyc_Dict_Dict _tmp71=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp74,r);
if(_tmp74.t == _tmp71.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp72=Cyc_CfFlowInfo_ReachableFL(_tmp71,_tmp73);
Cyc_NewControlFlow_update_tryflow(env,_tmp72);
return _tmp72;};};};}_LL4D:;}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 427
static struct _tuple23 Cyc_NewControlFlow_anal_Rexps(struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 434
struct Cyc_List_List*rvals=0;
if(es == 0)
return({struct _tuple23 _tmp75;_tmp75.f1=inflow;_tmp75.f2=0;_tmp75;});
# 439
do{
struct _tuple16 _tmp76=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);struct _tuple16 _tmp77=_tmp76;union Cyc_CfFlowInfo_FlowInfo _tmp7A;void*_tmp79;_LL5C: _tmp7A=_tmp77.f1;_tmp79=_tmp77.f2;_LL5D:;
inflow=_tmp7A;
({struct Cyc_List_List*_tmp6B7=({struct Cyc_List_List*_tmp78=_region_malloc(rgn,sizeof(*_tmp78));_tmp78->hd=_tmp79;_tmp78->tl=rvals;_tmp78;});rvals=_tmp6B7;});
es=es->tl;
first_is_copy=others_are_copy;}while(es != 0);
# 447
Cyc_NewControlFlow_update_tryflow(env,inflow);
return({struct _tuple23 _tmp7B;_tmp7B.f1=inflow;({struct Cyc_List_List*_tmp6B8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals);_tmp7B.f2=_tmp6B8;});_tmp7B;});}
# 453
static struct _tuple16 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 456
struct _RegionHandle _tmp7C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp7C;_push_region(rgn);
{struct _tuple23 _tmp7D=
Cyc_NewControlFlow_anal_Rexps(rgn,env,inflow,es,0,0);
# 457
struct _tuple23 _tmp7E=_tmp7D;union Cyc_CfFlowInfo_FlowInfo _tmp86;struct Cyc_List_List*_tmp85;_LL5F: _tmp86=_tmp7E.f1;_tmp85=_tmp7E.f2;_LL60:;
# 459
{union Cyc_CfFlowInfo_FlowInfo _tmp7F=_tmp86;struct Cyc_Dict_Dict _tmp82;if((_tmp7F.ReachableFL).tag == 2){_LL62: _tmp82=((_tmp7F.ReachableFL).val).f1;_LL63:
# 461
 for(0;_tmp85 != 0;(_tmp85=_tmp85->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp82,(void*)_tmp85->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp80=0;({unsigned int _tmp6BA=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _dyneither_ptr _tmp6B9=({const char*_tmp81="expression may not be initialized";_tag_dyneither(_tmp81,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp6BA,_tmp6B9,_tag_dyneither(_tmp80,sizeof(void*),0));});});}
goto _LL61;}else{_LL64: _LL65:
 goto _LL61;}_LL61:;}{
# 467
struct _tuple16 _tmp84=({struct _tuple16 _tmp83;_tmp83.f1=_tmp86;_tmp83.f2=(env->fenv)->unknown_all;_tmp83;});_npop_handler(0);return _tmp84;};}
# 457
;_pop_region(rgn);}
# 475
static void*Cyc_NewControlFlow_consume_zero_rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Dict_Dict new_dict,struct Cyc_CfFlowInfo_Place*p,struct Cyc_Absyn_Exp*e,void*new_rval){
# 482
int needs_unconsume=0;
void*_tmp87=Cyc_CfFlowInfo_lookup_place(new_dict,p);
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp87,& needs_unconsume))
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp88=_cycalloc(sizeof(*_tmp88));({struct Cyc_Core_Impossible_exn_struct _tmp6BC=({struct Cyc_Core_Impossible_exn_struct _tmp89;_tmp89.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp6BB=({const char*_tmp8A="consume_zero_ral";_tag_dyneither(_tmp8A,sizeof(char),17);});_tmp89.f1=_tmp6BB;});_tmp89;});_tmp88[0]=_tmp6BC;});_tmp88;}));else{
# 487
if(needs_unconsume)
return Cyc_CfFlowInfo_make_unique_consumed(env->fenv,(void*)_check_null(e->topt),e,env->iteration_num,new_rval);else{
# 491
return new_rval;}}}
# 504 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 510
union Cyc_CfFlowInfo_FlowInfo _tmp8B=outflow;struct Cyc_Dict_Dict _tmp93;struct Cyc_List_List*_tmp92;if((_tmp8B.BottomFL).tag == 1){_LL67: _LL68:
 return outflow;}else{_LL69: _tmp93=((_tmp8B.ReachableFL).val).f1;_tmp92=((_tmp8B.ReachableFL).val).f2;_LL6A: {
# 513
union Cyc_CfFlowInfo_AbsLVal _tmp8C=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmp8D=_tmp8C;struct Cyc_CfFlowInfo_Place*_tmp91;if((_tmp8D.UnknownL).tag == 2){_LL6C: _LL6D:
# 517
 return outflow;}else{_LL6E: _tmp91=(_tmp8D.PlaceL).val;_LL6F: {
# 521
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->unknown_none;
for(0;names != 0;names=names->tl){
({void*_tmp6BE=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8E=_region_malloc((env->fenv)->r,sizeof(*_tmp8E));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6BD=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp8F;_tmp8F.tag=8;_tmp8F.f1=(struct Cyc_Absyn_Vardecl*)names->hd;_tmp8F.f2=nzval;_tmp8F;});_tmp8E[0]=_tmp6BD;});_tmp8E;});nzval=_tmp6BE;});}
# 525
({void*_tmp6BF=Cyc_NewControlFlow_consume_zero_rval(env,_tmp93,_tmp91,e,nzval);nzval=_tmp6BF;});{
union Cyc_CfFlowInfo_FlowInfo _tmp90=({struct Cyc_Dict_Dict _tmp6C0=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp93,_tmp91,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp6C0,_tmp92);});
# 530
return _tmp90;};}}_LL6B:;}}_LL66:;}
# 539
static struct _tuple19 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 542
union Cyc_CfFlowInfo_FlowInfo _tmp94=outflow;struct Cyc_Dict_Dict _tmpA0;struct Cyc_List_List*_tmp9F;if((_tmp94.BottomFL).tag == 1){_LL71: _LL72:
 return({struct _tuple19 _tmp95;_tmp95.f1=outflow;_tmp95.f2=outflow;_tmp95;});}else{_LL73: _tmpA0=((_tmp94.ReachableFL).val).f1;_tmp9F=((_tmp94.ReachableFL).val).f2;_LL74: {
# 545
union Cyc_CfFlowInfo_AbsLVal _tmp96=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmp97=_tmp96;struct Cyc_CfFlowInfo_Place*_tmp9E;if((_tmp97.UnknownL).tag == 2){_LL76: _LL77:
 return({struct _tuple19 _tmp98;_tmp98.f1=outflow;_tmp98.f2=outflow;_tmp98;});}else{_LL78: _tmp9E=(_tmp97.PlaceL).val;_LL79: {
# 548
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->unknown_none;
void*zval=(env->fenv)->zero;
for(0;names != 0;names=names->tl){
({void*_tmp6C2=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp99=_region_malloc((env->fenv)->r,sizeof(*_tmp99));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6C1=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp9A;_tmp9A.tag=8;_tmp9A.f1=(struct Cyc_Absyn_Vardecl*)names->hd;_tmp9A.f2=nzval;_tmp9A;});_tmp99[0]=_tmp6C1;});_tmp99;});nzval=_tmp6C2;});
({void*_tmp6C4=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp9B=_region_malloc((env->fenv)->r,sizeof(*_tmp9B));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6C3=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp9C;_tmp9C.tag=8;_tmp9C.f1=(struct Cyc_Absyn_Vardecl*)names->hd;_tmp9C.f2=zval;_tmp9C;});_tmp9B[0]=_tmp6C3;});_tmp9B;});zval=_tmp6C4;});}
# 554
({void*_tmp6C5=Cyc_NewControlFlow_consume_zero_rval(env,_tmpA0,_tmp9E,e,nzval);nzval=_tmp6C5;});
({void*_tmp6C6=Cyc_NewControlFlow_consume_zero_rval(env,_tmpA0,_tmp9E,e,zval);zval=_tmp6C6;});
return({struct _tuple19 _tmp9D;({union Cyc_CfFlowInfo_FlowInfo _tmp6C8=({
struct Cyc_Dict_Dict _tmp6C7=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA0,_tmp9E,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp6C7,_tmp9F);});_tmp9D.f1=_tmp6C8;});({union Cyc_CfFlowInfo_FlowInfo _tmp6CA=({
struct Cyc_Dict_Dict _tmp6C9=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA0,_tmp9E,zval);Cyc_CfFlowInfo_ReachableFL(_tmp6C9,_tmp9F);});_tmp9D.f2=_tmp6CA;});_tmp9D;});}}_LL75:;}}_LL70:;}
# 563
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 566
static void Cyc_NewControlFlow_update_relns(struct Cyc_Absyn_Exp*e,int possibly_null,struct Cyc_List_List*relns){
void*_tmpA3=e->r;void*_tmpA4=_tmpA3;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA4)->tag == 23){_LL7B: _LL7C:
# 570
{void*_tmpA5=e->annot;void*_tmpA6=_tmpA5;struct Cyc_List_List*_tmpA8;struct Cyc_List_List*_tmpA7;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpA6)->tag == Cyc_CfFlowInfo_UnknownZ){_LL80: _tmpA7=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpA6)->f1;_LL81:
# 572
 if(possibly_null  && Cyc_Relations_same_relns(relns,_tmpA7))return;
goto _LL7F;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpA6)->tag == Cyc_CfFlowInfo_NotZero){_LL82: _tmpA8=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpA6)->f1;_LL83:
# 575
 possibly_null=0;
if(Cyc_Relations_same_relns(relns,_tmpA8))return;
goto _LL7F;}else{_LL84: _LL85:
 goto _LL7F;}}_LL7F:;}
# 580
if(possibly_null)
({void*_tmp6CD=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6CC=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmpAA;_tmpAA.tag=Cyc_CfFlowInfo_UnknownZ;({struct Cyc_List_List*_tmp6CB=Cyc_Relations_copy_relns(Cyc_Core_heap_region,relns);_tmpAA.f1=_tmp6CB;});_tmpAA;});_tmpA9[0]=_tmp6CC;});_tmpA9;});e->annot=_tmp6CD;});else{
# 584
({void*_tmp6D0=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpAB=_cycalloc(sizeof(*_tmpAB));({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp6CF=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmpAC;_tmpAC.tag=Cyc_CfFlowInfo_NotZero;({struct Cyc_List_List*_tmp6CE=Cyc_Relations_copy_relns(Cyc_Core_heap_region,relns);_tmpAC.f1=_tmp6CE;});_tmpAC;});_tmpAB[0]=_tmp6CF;});_tmpAB;});e->annot=_tmp6D0;});}
# 586
goto _LL7A;}else{_LL7D: _LL7E:
# 590
 if(possibly_null){
void*_tmpAD=e->annot;void*_tmpAE=_tmpAD;struct Cyc_List_List*_tmpAF;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpAE)->tag == Cyc_CfFlowInfo_UnknownZ){_LL87: _tmpAF=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpAE)->f1;_LL88:
# 593
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
return;}else{_LL89: _LL8A:
 goto _LL86;}_LL86:;}
# 598
e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL7A;}_LL7A:;}
# 608
static struct _tuple16 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r){
# 612
void*_tmpB0=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpB1=_tmpB0;void*_tmpDE;union Cyc_Absyn_Constraint*_tmpDD;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB1)->tag == 5){_LL8C: _tmpDE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB1)->f1).elt_typ;_tmpDD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB1)->f1).ptr_atts).bounds;_LL8D: {
# 614
union Cyc_CfFlowInfo_FlowInfo _tmpB2=f;struct Cyc_Dict_Dict _tmpD9;struct Cyc_List_List*_tmpD8;if((_tmpB2.BottomFL).tag == 1){_LL91: _LL92:
# 616
 return({struct _tuple16 _tmpB3;_tmpB3.f1=f;({void*_tmp6D1=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpDE,0,(env->fenv)->unknown_all);_tmpB3.f2=_tmp6D1;});_tmpB3;});}else{_LL93: _tmpD9=((_tmpB2.ReachableFL).val).f1;_tmpD8=((_tmpB2.ReachableFL).val).f2;_LL94:
# 619
 if(Cyc_Tcutil_is_bound_one(_tmpDD)){
struct _tuple15 _tmpB4=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);struct _tuple15 _tmpB5=_tmpB4;void*_tmpC7;struct Cyc_List_List*_tmpC6;_LL96: _tmpC7=_tmpB5.f1;_tmpC6=_tmpB5.f2;_LL97:;{
void*_tmpB6=_tmpC7;enum Cyc_CfFlowInfo_InitLevel _tmpC5;void*_tmpC4;struct Cyc_CfFlowInfo_Place*_tmpC3;struct Cyc_Absyn_Vardecl*_tmpC2;void*_tmpC1;switch(*((int*)_tmpB6)){case 8: _LL99: _tmpC2=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpB6)->f1;_tmpC1=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpB6)->f2;_LL9A:
# 623
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpB7=_cycalloc(sizeof(*_tmpB7));({struct Cyc_Core_Impossible_exn_struct _tmp6D3=({struct Cyc_Core_Impossible_exn_struct _tmpB8;_tmpB8.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp6D2=({const char*_tmpB9="named location in anal_derefR";_tag_dyneither(_tmpB9,sizeof(char),30);});_tmpB8.f1=_tmp6D2;});_tmpB8;});_tmpB7[0]=_tmp6D3;});_tmpB7;}));case 1: _LL9B: _LL9C:
# 625
 Cyc_NewControlFlow_update_relns(e,0,_tmpD8);
goto _LL98;case 4: _LL9D: _tmpC3=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpB6)->f1;_LL9E:
# 628
 Cyc_NewControlFlow_update_relns(e,0,_tmpD8);
return({struct _tuple16 _tmpBA;_tmpBA.f1=f;({void*_tmp6D4=Cyc_CfFlowInfo_lookup_place(_tmpD9,_tmpC3);_tmpBA.f2=_tmp6D4;});_tmpBA;});case 5: _LL9F: _tmpC4=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmpB6)->f1;_LLA0:
# 631
 Cyc_NewControlFlow_update_relns(e,1,_tmpD8);
return({struct _tuple16 _tmpBB;_tmpBB.f1=f;_tmpBB.f2=_tmpC4;_tmpBB;});case 0: _LLA1: _LLA2:
# 634
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple16 _tmpBC;({union Cyc_CfFlowInfo_FlowInfo _tmp6D5=Cyc_CfFlowInfo_BottomFL();_tmpBC.f1=_tmp6D5;});({void*_tmp6D6=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpDE,0,(env->fenv)->unknown_all);_tmpBC.f2=_tmp6D6;});_tmpBC;});case 2: _LLA3: _tmpC5=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpB6)->f1;_LLA4:
# 638
({union Cyc_CfFlowInfo_FlowInfo _tmp6D7=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpC5,_tmpC6);f=_tmp6D7;});
goto _LLA6;default: _LLA5: _LLA6:
# 641
{void*_tmpBD=e->r;void*_tmpBE=_tmpBD;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBE)->tag == 23){_LLA8: _LLA9:
# 643
({void*_tmp6DA=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpBF=_cycalloc(sizeof(*_tmpBF));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6D9=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmpC0;_tmpC0.tag=Cyc_CfFlowInfo_UnknownZ;({struct Cyc_List_List*_tmp6D8=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD8);_tmpC0.f1=_tmp6D8;});_tmpC0;});_tmpBF[0]=_tmp6D9;});_tmpBF;});e->annot=_tmp6DA;});
goto _LLA7;}else{_LLAA: _LLAB:
# 646
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LLA7;}_LLA7:;}
# 649
goto _LL98;}_LL98:;};}else{
# 652
void*_tmpC8=e->annot;void*_tmpC9=_tmpC8;struct Cyc_List_List*_tmpCC;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpC9)->tag == Cyc_CfFlowInfo_UnknownZ){_LLAD: _tmpCC=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpC9)->f1;_LLAE:
# 654
 if(!Cyc_Relations_same_relns(_tmpD8,_tmpCC))goto _LLB0;
goto _LLAC;}else{_LLAF: _LLB0:
# 657
({void*_tmp6DD=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpCA=_cycalloc(sizeof(*_tmpCA));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6DC=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmpCB;_tmpCB.tag=Cyc_CfFlowInfo_UnknownZ;({struct Cyc_List_List*_tmp6DB=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD8);_tmpCB.f1=_tmp6DB;});_tmpCB;});_tmpCA[0]=_tmp6DC;});_tmpCA;});e->annot=_tmp6DD;});
goto _LLAC;}_LLAC:;}{
# 661
enum Cyc_CfFlowInfo_InitLevel _tmpCD=Cyc_CfFlowInfo_initlevel(env->fenv,_tmpD9,r);enum Cyc_CfFlowInfo_InitLevel _tmpCE=_tmpCD;if(_tmpCE == Cyc_CfFlowInfo_NoneIL){_LLB2: _LLB3: {
# 663
struct _tuple15 _tmpCF=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);struct _tuple15 _tmpD0=_tmpCF;void*_tmpD6;_LLB7: _tmpD6=_tmpD0.f1;_LLB8:;
{void*_tmpD1=_tmpD6;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpD1)->tag == 7){_LLBA: _LLBB:
# 666
({void*_tmpD2=0;({unsigned int _tmp6DF=e->loc;struct _dyneither_ptr _tmp6DE=({const char*_tmpD3="attempt to dereference a consumed alias-free pointer";_tag_dyneither(_tmpD3,sizeof(char),53);});Cyc_CfFlowInfo_aerr(_tmp6DF,_tmp6DE,_tag_dyneither(_tmpD2,sizeof(void*),0));});});
goto _LLB9;}else{_LLBC: _LLBD:
# 669
({void*_tmpD4=0;({unsigned int _tmp6E1=e->loc;struct _dyneither_ptr _tmp6E0=({const char*_tmpD5="dereference of possibly uninitialized pointer";_tag_dyneither(_tmpD5,sizeof(char),46);});Cyc_CfFlowInfo_aerr(_tmp6E1,_tmp6E0,_tag_dyneither(_tmpD4,sizeof(void*),0));});});}_LLB9:;}
# 671
goto _LLB5;}}else{_LLB4: _LLB5:
# 673
 return({struct _tuple16 _tmpD7;_tmpD7.f1=f;({void*_tmp6E2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpDE,0,(env->fenv)->unknown_all);_tmpD7.f2=_tmp6E2;});_tmpD7;});}_LLB1:;};}_LL90:;}}else{_LL8E: _LL8F:
# 676
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpDA=_cycalloc(sizeof(*_tmpDA));({struct Cyc_Core_Impossible_exn_struct _tmp6E4=({struct Cyc_Core_Impossible_exn_struct _tmpDB;_tmpDB.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp6E3=({const char*_tmpDC="right deref of non-pointer-type";_tag_dyneither(_tmpDC,sizeof(char),32);});_tmpDB.f1=_tmp6E3;});_tmpDB;});_tmpDA[0]=_tmp6E4;});_tmpDA;}));}_LL8B:;}
# 683
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 686
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
int e2_valid_op=Cyc_Relations_exp2relnop(e2,& n2);
# 689
{void*_tmpDF=e1->r;void*_tmpE0=_tmpDF;void*_tmpE2;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE0)->tag == 1){_LLBF: _tmpE2=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LLC0: {
# 691
struct Cyc_Absyn_Vardecl*_tmpE1=Cyc_Tcutil_nonesc_vardecl(_tmpE2);
if(_tmpE1 != 0){
union Cyc_Relations_RelnOp n1=Cyc_Relations_RNumelts(_tmpE1);
if(e2_valid_op)
({struct Cyc_List_List*_tmp6E5=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,n1,relns);relns=_tmp6E5;});}
# 697
goto _LLBE;}}else{_LLC1: _LLC2:
 goto _LLBE;}_LLBE:;}{
# 701
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound((void*)_check_null(e1->topt));
if(bound != 0){
union Cyc_Relations_RelnOp rbound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rbound))
({struct Cyc_List_List*_tmp6E6=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,rbound,relns);relns=_tmp6E6;});}
# 707
return relns;};}
# 715
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_restore_noconsume_arg(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*exp,unsigned int loc,void*old_rval){
# 720
struct _tuple18 _tmpE3=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);struct _tuple18 _tmpE4=_tmpE3;union Cyc_CfFlowInfo_AbsLVal _tmpF4;_LLC4: _tmpF4=_tmpE4.f2;_LLC5:;
{struct _tuple18 _tmpE5=({struct _tuple18 _tmpF3;_tmpF3.f1=inflow;_tmpF3.f2=_tmpF4;_tmpF3;});struct _tuple18 _tmpE6=_tmpE5;struct Cyc_Dict_Dict _tmpF2;struct Cyc_List_List*_tmpF1;struct Cyc_CfFlowInfo_Place*_tmpF0;if(((_tmpE6.f1).ReachableFL).tag == 2){if(((_tmpE6.f2).PlaceL).tag == 1){_LLC7: _tmpF2=(((_tmpE6.f1).ReachableFL).val).f1;_tmpF1=(((_tmpE6.f1).ReachableFL).val).f2;_tmpF0=((_tmpE6.f2).PlaceL).val;_LLC8: {
# 723
void*_tmpE7=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,(void*)_check_null(exp->topt),0,(env->fenv)->unknown_all);
# 725
struct _tuple15 _tmpE8=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,old_rval);struct _tuple15 _tmpE9=_tmpE8;void*_tmpED;struct Cyc_List_List*_tmpEC;_LLCE: _tmpED=_tmpE9.f1;_tmpEC=_tmpE9.f2;_LLCF:;
for(0;_tmpEC != 0;_tmpEC=_tmpEC->tl){
({void*_tmp6E8=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpEA=_region_malloc((env->fenv)->r,sizeof(*_tmpEA));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6E7=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmpEB;_tmpEB.tag=8;_tmpEB.f1=(struct Cyc_Absyn_Vardecl*)_tmpEC->hd;_tmpEB.f2=_tmpE7;_tmpEB;});_tmpEA[0]=_tmp6E7;});_tmpEA;});_tmpE7=_tmp6E8;});}
# 730
({struct Cyc_Dict_Dict _tmp6E9=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmpF2,_tmpF0,_tmpE7);_tmpF2=_tmp6E9;});
({union Cyc_CfFlowInfo_FlowInfo _tmp6EA=Cyc_CfFlowInfo_ReachableFL(_tmpF2,_tmpF1);inflow=_tmp6EA;});
Cyc_NewControlFlow_update_tryflow(env,inflow);
# 736
goto _LLC6;}}else{_LLCB: _LLCC:
# 739
({void*_tmpEE=0;({struct _dyneither_ptr _tmp6EB=({const char*_tmpEF="noconsume() parameters must be l-values";_tag_dyneither(_tmpEF,sizeof(char),40);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp6EB,_tag_dyneither(_tmpEE,sizeof(void*),0));});});
goto _LLC6;}}else{_LLC9: _LLCA:
# 737
 goto _LLC6;}_LLC6:;}
# 742
return inflow;}
# 747
static struct _tuple16 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 755
union Cyc_CfFlowInfo_FlowInfo _tmpF5=outflow;struct Cyc_Dict_Dict _tmp103;struct Cyc_List_List*_tmp102;if((_tmpF5.BottomFL).tag == 1){_LLD1: _LLD2:
# 757
 return({struct _tuple16 _tmpF6;({union Cyc_CfFlowInfo_FlowInfo _tmp6EC=Cyc_CfFlowInfo_BottomFL();_tmpF6.f1=_tmp6EC;});_tmpF6.f2=rval;_tmpF6;});}else{_LLD3: _tmp103=((_tmpF5.ReachableFL).val).f1;_tmp102=((_tmpF5.ReachableFL).val).f2;_LLD4: {
# 759
union Cyc_CfFlowInfo_AbsLVal _tmpF7=lval;struct Cyc_CfFlowInfo_Place*_tmp101;if((_tmpF7.PlaceL).tag == 1){_LLD6: _tmp101=(_tmpF7.PlaceL).val;_LLD7: {
# 767
struct Cyc_Dict_Dict _tmpF8=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp103,_tmp101,rval);
({struct Cyc_List_List*_tmp6ED=Cyc_Relations_reln_assign_exp(fenv->r,_tmp102,lexp,rexp);_tmp102=_tmp6ED;});
({union Cyc_CfFlowInfo_FlowInfo _tmp6EE=Cyc_CfFlowInfo_ReachableFL(_tmpF8,_tmp102);outflow=_tmp6EE;});
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(lexp->topt))){
# 773
struct _tuple15 _tmpF9=({struct _RegionHandle*_tmp6EF=fenv->r;Cyc_CfFlowInfo_unname_rval(_tmp6EF,Cyc_CfFlowInfo_lookup_place(_tmp103,_tmp101));});struct _tuple15 _tmpFA=_tmpF9;void*_tmpFE;_LLDB: _tmpFE=_tmpFA.f1;_LLDC:;{
void*_tmpFB=_tmpFE;switch(*((int*)_tmpFB)){case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpFB)->f1 == Cyc_CfFlowInfo_NoneIL){_LLDE: _LLDF:
 goto _LLE1;}else{goto _LLE4;}case 7: _LLE0: _LLE1:
 goto _LLE3;case 0: _LLE2: _LLE3:
 goto _LLDD;default: _LLE4: _LLE5:
# 779
({void*_tmpFC=0;({unsigned int _tmp6F1=lexp->loc;struct _dyneither_ptr _tmp6F0=({const char*_tmpFD="assignment may overwrite alias-free pointer(s)";_tag_dyneither(_tmpFD,sizeof(char),47);});Cyc_Tcutil_warn(_tmp6F1,_tmp6F0,_tag_dyneither(_tmpFC,sizeof(void*),0));});});
goto _LLDD;}_LLDD:;};}
# 784
Cyc_NewControlFlow_update_tryflow(env,outflow);
return({struct _tuple16 _tmpFF;_tmpFF.f1=outflow;_tmpFF.f2=rval;_tmpFF;});}}else{_LLD8: _LLD9:
# 790
 return({struct _tuple16 _tmp100;({union Cyc_CfFlowInfo_FlowInfo _tmp6F2=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval);_tmp100.f1=_tmp6F2;});_tmp100.f2=rval;_tmp100;});}_LLD5:;}}_LLD0:;}
# 797
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 803
union Cyc_CfFlowInfo_FlowInfo _tmp104=f;struct Cyc_Dict_Dict _tmp10A;struct Cyc_List_List*_tmp109;if((_tmp104.BottomFL).tag == 1){_LLE7: _LLE8:
 return Cyc_CfFlowInfo_BottomFL();}else{_LLE9: _tmp10A=((_tmp104.ReachableFL).val).f1;_tmp109=((_tmp104.ReachableFL).val).f2;_LLEA:
# 808
({struct Cyc_Dict_Dict _tmp6F9=({struct Cyc_CfFlowInfo_FlowEnv*_tmp6F8=fenv;unsigned int _tmp6F7=loc;struct Cyc_Dict_Dict _tmp6F6=_tmp10A;struct Cyc_CfFlowInfo_Place*_tmp6F5=({struct Cyc_CfFlowInfo_Place*_tmp105=_region_malloc(env->r,sizeof(*_tmp105));({void*_tmp6F4=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp106=_region_malloc(env->r,sizeof(*_tmp106));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp6F3=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp107;_tmp107.tag=0;_tmp107.f1=vd;_tmp107;});_tmp106[0]=_tmp6F3;});_tmp106;});_tmp105->root=_tmp6F4;});_tmp105->path=0;_tmp105;});Cyc_CfFlowInfo_assign_place(_tmp6F8,_tmp6F7,_tmp6F6,_tmp6F5,rval);});_tmp10A=_tmp6F9;});
# 811
({struct Cyc_List_List*_tmp6FA=Cyc_Relations_reln_assign_var(env->r,_tmp109,vd,rexp);_tmp109=_tmp6FA;});{
union Cyc_CfFlowInfo_FlowInfo _tmp108=Cyc_CfFlowInfo_ReachableFL(_tmp10A,_tmp109);
Cyc_NewControlFlow_update_tryflow(env,_tmp108);
# 816
return _tmp108;};}_LLE6:;}struct _tuple24{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 820
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned int pat_loc,unsigned int exp_loc){
# 827
if(vds == 0)return inflow;{
# 830
struct Cyc_List_List*_tmp10B=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(vds)).f1);
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple24*)x->hd)).f1 == 0)({struct Cyc_List_List*_tmp6FB=({struct Cyc_List_List*_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple24*)x->hd)).f2);_tmp10C->tl=es;_tmp10C;});es=_tmp6FB;});}}
# 836
({union Cyc_CfFlowInfo_FlowInfo _tmp6FC=Cyc_NewControlFlow_add_vars(fenv,inflow,_tmp10B,fenv->unknown_all,pat_loc,name_locs);inflow=_tmp6FC;});
# 838
({union Cyc_CfFlowInfo_FlowInfo _tmp6FD=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);inflow=_tmp6FD;});
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 842
struct _tuple16 _tmp10D=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple16 _tmp10E=_tmp10D;union Cyc_CfFlowInfo_FlowInfo _tmp110;void*_tmp10F;_LLEC: _tmp110=_tmp10E.f1;_tmp10F=_tmp10E.f2;_LLED:;
({union Cyc_CfFlowInfo_FlowInfo _tmp6FE=Cyc_NewControlFlow_use_Rval(env,exp_loc,_tmp110,_tmp10F);inflow=_tmp6FE;});}}{
# 850
struct Cyc_List_List*_tmp111=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);
for(0;_tmp111 != 0;_tmp111=_tmp111->tl){
struct _tuple24*_tmp112=(struct _tuple24*)_tmp111->hd;struct _tuple24*_tmp113=_tmp112;struct Cyc_Absyn_Vardecl**_tmp12E;struct Cyc_Absyn_Exp*_tmp12D;_LLEF: _tmp12E=_tmp113->f1;_tmp12D=_tmp113->f2;_LLF0:;
if(_tmp12E != 0  && _tmp12D != 0){
if(_tmp12D->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp116;_tmp116.tag=0;({struct _dyneither_ptr _tmp6FF=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp12D));_tmp116.f1=_tmp6FF;});({void*_tmp114[1]={& _tmp116};({struct _dyneither_ptr _tmp700=({const char*_tmp115="oops! pattern init expr %s has no type!\n";_tag_dyneither(_tmp115,sizeof(char),41);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp700,_tag_dyneither(_tmp114,sizeof(void*),1));});});});{
# 864
struct Cyc_List_List l=({struct Cyc_List_List _tmp12C;_tmp12C.hd=_tmp12D;_tmp12C.tl=0;_tmp12C;});
union Cyc_CfFlowInfo_FlowInfo _tmp117=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp118=Cyc_NewControlFlow_anal_Lexp(env,_tmp117,0,0,_tmp12D);struct _tuple18 _tmp119=_tmp118;union Cyc_CfFlowInfo_AbsLVal _tmp12B;_LLF2: _tmp12B=_tmp119.f2;_LLF3:;{
struct _tuple16 _tmp11A=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp117,_tmp12D);struct _tuple16 _tmp11B=_tmp11A;union Cyc_CfFlowInfo_FlowInfo _tmp12A;void*_tmp129;_LLF5: _tmp12A=_tmp11B.f1;_tmp129=_tmp11B.f2;_LLF6:;{
union Cyc_CfFlowInfo_FlowInfo _tmp11C=_tmp12A;struct Cyc_Dict_Dict _tmp128;struct Cyc_List_List*_tmp127;if((_tmp11C.ReachableFL).tag == 2){_LLF8: _tmp128=((_tmp11C.ReachableFL).val).f1;_tmp127=((_tmp11C.ReachableFL).val).f2;_LLF9:
# 870
 if(name_locs){
union Cyc_CfFlowInfo_AbsLVal _tmp11D=_tmp12B;struct Cyc_CfFlowInfo_Place*_tmp126;if((_tmp11D.PlaceL).tag == 1){_LLFD: _tmp126=(_tmp11D.PlaceL).val;_LLFE:
# 873
({void*_tmp702=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp11E=_region_malloc(fenv->r,sizeof(*_tmp11E));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp701=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp11F;_tmp11F.tag=8;_tmp11F.f1=*_tmp12E;_tmp11F.f2=_tmp129;_tmp11F;});_tmp11E[0]=_tmp701;});_tmp11E;});_tmp129=_tmp702;});{
# 876
void*_tmp120=Cyc_CfFlowInfo_lookup_place(_tmp128,_tmp126);
({void*_tmp704=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp121=_region_malloc(fenv->r,sizeof(*_tmp121));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp703=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp122;_tmp122.tag=8;_tmp122.f1=*_tmp12E;_tmp122.f2=_tmp120;_tmp122;});_tmp121[0]=_tmp703;});_tmp121;});_tmp120=_tmp704;});
({struct Cyc_Dict_Dict _tmp705=Cyc_CfFlowInfo_assign_place(fenv,exp_loc,_tmp128,_tmp126,_tmp120);_tmp128=_tmp705;});
({union Cyc_CfFlowInfo_FlowInfo _tmp706=Cyc_CfFlowInfo_ReachableFL(_tmp128,_tmp127);_tmp12A=_tmp706;});
goto _LLFC;};}else{_LLFF: _LL100:
# 883
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(_tmp12D->topt)) && !
Cyc_Tcutil_is_noalias_pointer_or_aggr((*_tmp12E)->type))
({struct Cyc_String_pa_PrintArg_struct _tmp125;_tmp125.tag=0;({struct _dyneither_ptr _tmp707=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp12D));_tmp125.f1=_tmp707;});({void*_tmp123[1]={& _tmp125};({unsigned int _tmp709=exp_loc;struct _dyneither_ptr _tmp708=({const char*_tmp124="aliased pattern expression not an l-value: %s";_tag_dyneither(_tmp124,sizeof(char),46);});Cyc_CfFlowInfo_aerr(_tmp709,_tmp708,_tag_dyneither(_tmp123,sizeof(void*),1));});});});}_LLFC:;}
# 893
({union Cyc_CfFlowInfo_FlowInfo _tmp70A=Cyc_NewControlFlow_do_initialize_var(fenv,env,_tmp12A,*_tmp12E,_tmp12D,_tmp129,pat_loc);inflow=_tmp70A;});
goto _LLF7;}else{_LLFA: _LLFB:
# 896
 goto _LLF7;}_LLF7:;};};};}}
# 901
return inflow;};};}
# 904
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e,int cast_ok){
# 906
void*_tmp12F=e->r;void*_tmp130=_tmp12F;struct Cyc_Absyn_Exp*_tmp137;struct Cyc_Absyn_Exp*_tmp136;struct Cyc_Absyn_Exp*_tmp135;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_Absyn_Exp*_tmp133;switch(*((int*)_tmp130)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp130)->f1)){case 4: _LL102: _LL103:
 goto _LL105;case 3: _LL104: _LL105:
 goto _LL107;case 5: _LL106: _LL107:
 return 1;default: goto _LL112;}case 20: _LL108: _tmp133=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp130)->f1;_LL109:
 _tmp134=_tmp133;goto _LL10B;case 21: _LL10A: _tmp134=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp130)->f1;_LL10B:
 _tmp135=_tmp134;goto _LL10D;case 22: _LL10C: _tmp135=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp130)->f1;_LL10D:
# 913
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp135,cast_ok);case 23: _LL10E: _tmp136=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp130)->f1;_LL10F: {
# 915
void*_tmp131=Cyc_Tcutil_compress((void*)_check_null(_tmp136->topt));void*_tmp132=_tmp131;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp132)->tag == 10){_LL115: _LL116:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp136,cast_ok);}else{_LL117: _LL118:
 return 0;}_LL114:;}case 14: _LL110: _tmp137=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp130)->f2;_LL111:
# 920
 if(cast_ok)return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp137,cast_ok);else{
return 0;}default: _LL112: _LL113:
 return 0;}_LL101:;}
# 926
static int Cyc_NewControlFlow_subst_param(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp*rop){
union Cyc_Relations_RelnOp _tmp138=*rop;union Cyc_Relations_RelnOp _tmp139=_tmp138;unsigned int _tmp13D;unsigned int _tmp13C;switch((_tmp139.RParamNumelts).tag){case 5: _LL11A: _tmp13C=(_tmp139.RParam).val;_LL11B: {
# 929
struct Cyc_Absyn_Exp*_tmp13A=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp13C);
return Cyc_Relations_exp2relnop(_tmp13A,rop);}case 6: _LL11C: _tmp13D=(_tmp139.RParamNumelts).val;_LL11D: {
# 932
struct Cyc_Absyn_Exp*_tmp13B=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp13D);
return({struct Cyc_Absyn_Exp*_tmp70B=Cyc_Absyn_prim1_exp(Cyc_Absyn_Numelts,_tmp13B,0);Cyc_Relations_exp2relnop(_tmp70B,rop);});}default: _LL11E: _LL11F:
 return 1;}_LL119:;}
# 938
static struct _dyneither_ptr Cyc_NewControlFlow_subst_param_string(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp rop){
union Cyc_Relations_RelnOp _tmp13E=rop;unsigned int _tmp140;unsigned int _tmp13F;switch((_tmp13E.RParamNumelts).tag){case 5: _LL121: _tmp13F=(_tmp13E.RParam).val;_LL122:
# 941
 return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp13F));case 6: _LL123: _tmp140=(_tmp13E.RParamNumelts).val;_LL124:
# 943
 return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp140));default: _LL125: _LL126:
 return Cyc_Relations_rop2string(rop);}_LL120:;}
# 948
static void Cyc_NewControlFlow_check_fn_requires(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*exps,struct Cyc_List_List*req,unsigned int loc){
# 951
union Cyc_CfFlowInfo_FlowInfo _tmp141=inflow;struct Cyc_Dict_Dict _tmp14B;struct Cyc_List_List*_tmp14A;if((_tmp141.BottomFL).tag == 1){_LL128: _LL129:
 return;}else{_LL12A: _tmp14B=((_tmp141.ReachableFL).val).f1;_tmp14A=((_tmp141.ReachableFL).val).f2;_LL12B:
# 954
 for(0;req != 0;req=req->tl){
struct Cyc_Relations_Reln*_tmp142=(struct Cyc_Relations_Reln*)req->hd;
union Cyc_Relations_RelnOp rop1=_tmp142->rop1;
union Cyc_Relations_RelnOp rop2=_tmp142->rop2;
enum Cyc_Relations_Relation _tmp143=Cyc_Relations_flip_relation(_tmp142->relation);
if((!Cyc_NewControlFlow_subst_param(exps,& rop1) || !Cyc_NewControlFlow_subst_param(exps,& rop2)) || 
Cyc_Relations_consistent_relations(Cyc_Relations_add_relation(env->r,rop2,_tmp143,rop1,_tmp14A))){
struct _dyneither_ptr s1=Cyc_NewControlFlow_subst_param_string(exps,rop1);
struct _dyneither_ptr s2=Cyc_Relations_relation2string(_tmp142->relation);
struct _dyneither_ptr s3=Cyc_NewControlFlow_subst_param_string(exps,rop2);
({struct Cyc_String_pa_PrintArg_struct _tmp149;_tmp149.tag=0;({struct _dyneither_ptr _tmp70C=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Relations_relns2string(_tmp14A));_tmp149.f1=_tmp70C;});({struct Cyc_String_pa_PrintArg_struct _tmp148;_tmp148.tag=0;_tmp148.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);({struct Cyc_String_pa_PrintArg_struct _tmp147;_tmp147.tag=0;_tmp147.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({struct Cyc_String_pa_PrintArg_struct _tmp146;_tmp146.tag=0;_tmp146.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({void*_tmp144[4]={& _tmp146,& _tmp147,& _tmp148,& _tmp149};({unsigned int _tmp70E=loc;struct _dyneither_ptr _tmp70D=({const char*_tmp145="cannot prove that @requires clause %s %s %s is satisfied\n (all I know is %s)";_tag_dyneither(_tmp145,sizeof(char),77);});Cyc_Tcutil_terr(_tmp70E,_tmp70D,_tag_dyneither(_tmp144,sizeof(void*),4));});});});});});});
break;}}
# 969
goto _LL127;}_LL127:;}struct _tuple25{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple26{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 974
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 978
struct Cyc_CfFlowInfo_FlowEnv*_tmp14C=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 991
{union Cyc_CfFlowInfo_FlowInfo _tmp14D=inflow;struct Cyc_Dict_Dict _tmp150;struct Cyc_List_List*_tmp14F;if((_tmp14D.BottomFL).tag == 1){_LL12D: _LL12E:
 return({struct _tuple16 _tmp14E;({union Cyc_CfFlowInfo_FlowInfo _tmp70F=Cyc_CfFlowInfo_BottomFL();_tmp14E.f1=_tmp70F;});_tmp14E.f2=_tmp14C->unknown_all;_tmp14E;});}else{_LL12F: _tmp150=((_tmp14D.ReachableFL).val).f1;_tmp14F=((_tmp14D.ReachableFL).val).f2;_LL130:
 d=_tmp150;relns=_tmp14F;}_LL12C:;}
# 1006 "new_control_flow.cyc"
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e,0)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
# 1027 "new_control_flow.cyc"
struct _tuple18 _tmp151=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);struct _tuple18 _tmp152=_tmp151;union Cyc_CfFlowInfo_FlowInfo _tmp162;union Cyc_CfFlowInfo_AbsLVal _tmp161;_LL132: _tmp162=_tmp152.f1;_tmp161=_tmp152.f2;_LL133:;{
struct _tuple18 _tmp153=({struct _tuple18 _tmp160;_tmp160.f1=_tmp162;_tmp160.f2=_tmp161;_tmp160;});struct _tuple18 _tmp154=_tmp153;struct Cyc_Dict_Dict _tmp15F;struct Cyc_List_List*_tmp15E;struct Cyc_CfFlowInfo_Place*_tmp15D;if(((_tmp154.f1).ReachableFL).tag == 2){if(((_tmp154.f2).PlaceL).tag == 1){_LL135: _tmp15F=(((_tmp154.f1).ReachableFL).val).f1;_tmp15E=(((_tmp154.f1).ReachableFL).val).f2;_tmp15D=((_tmp154.f2).PlaceL).val;_LL136: {
# 1030
void*_tmp155=Cyc_CfFlowInfo_lookup_place(_tmp15F,_tmp15D);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp155,& needs_unconsume)){
({void*_tmp156=0;({unsigned int _tmp711=e->loc;struct _dyneither_ptr _tmp710=({const char*_tmp157="expression attempts to copy a consumed alias-free value";_tag_dyneither(_tmp157,sizeof(char),56);});Cyc_CfFlowInfo_aerr(_tmp711,_tmp710,_tag_dyneither(_tmp156,sizeof(void*),0));});});
return({struct _tuple16 _tmp158;({union Cyc_CfFlowInfo_FlowInfo _tmp712=Cyc_CfFlowInfo_BottomFL();_tmp158.f1=_tmp712;});_tmp158.f2=_tmp14C->unknown_all;_tmp158;});}else{
# 1036
if(needs_unconsume)
# 1038
return({struct _tuple16 _tmp159;_tmp159.f1=_tmp162;({void*_tmp713=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp14C,_tmp155);_tmp159.f2=_tmp713;});_tmp159;});else{
# 1041
void*_tmp15A=Cyc_CfFlowInfo_make_unique_consumed(_tmp14C,(void*)_check_null(e->topt),e,env->iteration_num,_tmp155);
struct Cyc_Dict_Dict _tmp15B=Cyc_CfFlowInfo_assign_place(_tmp14C,e->loc,_tmp15F,_tmp15D,_tmp15A);
# 1053
return({struct _tuple16 _tmp15C;({union Cyc_CfFlowInfo_FlowInfo _tmp714=Cyc_CfFlowInfo_ReachableFL(_tmp15B,_tmp15E);_tmp15C.f1=_tmp714;});_tmp15C.f2=_tmp155;_tmp15C;});}}}}else{goto _LL137;}}else{_LL137: _LL138:
# 1057
 goto _LL134;}_LL134:;};}{
# 1060
void*_tmp163=e->r;void*_tmp164=_tmp163;struct Cyc_Absyn_Stmt*_tmp385;struct Cyc_Absyn_Exp*_tmp384;void*_tmp383;int _tmp382;struct Cyc_Absyn_Vardecl*_tmp381;struct Cyc_Absyn_Exp*_tmp380;struct Cyc_Absyn_Exp*_tmp37F;int _tmp37E;struct Cyc_List_List*_tmp37D;struct Cyc_List_List*_tmp37C;enum Cyc_Absyn_AggrKind _tmp37B;struct Cyc_List_List*_tmp37A;struct Cyc_List_List*_tmp379;struct Cyc_List_List*_tmp378;struct Cyc_Absyn_Exp*_tmp377;struct Cyc_Absyn_Exp*_tmp376;struct Cyc_Absyn_Exp*_tmp375;struct Cyc_Absyn_Exp*_tmp374;struct Cyc_Absyn_Exp*_tmp373;struct Cyc_Absyn_Exp*_tmp372;struct Cyc_Absyn_Exp*_tmp371;struct Cyc_Absyn_Exp*_tmp370;struct Cyc_Absyn_Exp*_tmp36F;struct Cyc_Absyn_Exp*_tmp36E;struct _dyneither_ptr*_tmp36D;struct Cyc_Absyn_Exp*_tmp36C;struct _dyneither_ptr*_tmp36B;struct Cyc_Absyn_Exp*_tmp36A;struct _dyneither_ptr*_tmp369;struct Cyc_Absyn_Exp*_tmp368;struct Cyc_Absyn_Exp*_tmp367;struct Cyc_Absyn_Exp*_tmp366;struct Cyc_Absyn_Exp*_tmp365;struct Cyc_Absyn_Exp*_tmp364;struct Cyc_Absyn_Exp*_tmp363;int _tmp362;struct Cyc_Absyn_Exp*_tmp361;void**_tmp360;struct Cyc_Absyn_Exp*_tmp35F;int _tmp35E;struct Cyc_Absyn_Exp*_tmp35D;struct Cyc_List_List*_tmp35C;struct Cyc_Absyn_Exp*_tmp35B;struct Cyc_Absyn_Exp*_tmp35A;struct Cyc_Absyn_Exp*_tmp359;struct Cyc_Absyn_Exp*_tmp358;struct Cyc_Absyn_Exp*_tmp357;struct Cyc_Absyn_Exp*_tmp356;struct Cyc_Absyn_Exp*_tmp355;struct Cyc_Absyn_Exp*_tmp354;enum Cyc_Absyn_Primop _tmp353;struct Cyc_List_List*_tmp352;struct Cyc_List_List*_tmp351;struct Cyc_Absyn_Datatypedecl*_tmp350;struct Cyc_Absyn_Vardecl*_tmp34F;struct Cyc_Absyn_Vardecl*_tmp34E;struct Cyc_Absyn_Vardecl*_tmp34D;struct _dyneither_ptr _tmp34C;struct Cyc_Absyn_Exp*_tmp34B;struct Cyc_Absyn_Exp*_tmp34A;struct Cyc_Absyn_Exp*_tmp349;struct Cyc_Absyn_Exp*_tmp348;switch(*((int*)_tmp164)){case 14: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp164)->f4 == Cyc_Absyn_Null_to_NonNull){_LL13A: _tmp348=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL13B: {
# 1064
struct _tuple16 _tmp165=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp348);struct _tuple16 _tmp166=_tmp165;union Cyc_CfFlowInfo_FlowInfo _tmp16D;void*_tmp16C;_LL1A5: _tmp16D=_tmp166.f1;_tmp16C=_tmp166.f2;_LL1A6:;{
struct _tuple16 _tmp167=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp16D,_tmp348,_tmp16C);struct _tuple16 _tmp168=_tmp167;union Cyc_CfFlowInfo_FlowInfo _tmp16B;void*_tmp16A;_LL1A8: _tmp16B=_tmp168.f1;_tmp16A=_tmp168.f2;_LL1A9:;
return({struct _tuple16 _tmp169;_tmp169.f1=_tmp16B;_tmp169.f2=_tmp16C;_tmp169;});};}}else{_LL13C: _tmp349=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL13D:
# 1070
 _tmp34A=_tmp349;goto _LL13F;}case 12: _LL13E: _tmp34A=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL13F:
 _tmp34B=_tmp34A;goto _LL141;case 13: _LL140: _tmp34B=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL141:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp34B);case 2: _LL142: _tmp34C=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL143:
# 1075
 if(!({struct _dyneither_ptr _tmp715=(struct _dyneither_ptr)_tmp34C;Cyc_NewControlFlow_strcmp(_tmp715,({const char*_tmp16E="print_flow";_tag_dyneither(_tmp16E,sizeof(char),11);}));})){
struct _dyneither_ptr _tmp16F=Cyc_Position_string_of_segment(e->loc);
({struct Cyc_String_pa_PrintArg_struct _tmp172;_tmp172.tag=0;_tmp172.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp16F);({void*_tmp170[1]={& _tmp172};({struct Cyc___cycFILE*_tmp717=Cyc_stderr;struct _dyneither_ptr _tmp716=({const char*_tmp171="%s: current flow is\n";_tag_dyneither(_tmp171,sizeof(char),21);});Cyc_fprintf(_tmp717,_tmp716,_tag_dyneither(_tmp170,sizeof(void*),1));});});});
Cyc_CfFlowInfo_print_flow(inflow);
({void*_tmp173=0;({struct Cyc___cycFILE*_tmp719=Cyc_stderr;struct _dyneither_ptr _tmp718=({const char*_tmp174="\n";_tag_dyneither(_tmp174,sizeof(char),2);});Cyc_fprintf(_tmp719,_tmp718,_tag_dyneither(_tmp173,sizeof(void*),0));});});}else{
# 1081
if(!({struct _dyneither_ptr _tmp71A=(struct _dyneither_ptr)_tmp34C;Cyc_NewControlFlow_strcmp(_tmp71A,({const char*_tmp175="debug_on";_tag_dyneither(_tmp175,sizeof(char),9);}));}));else{
# 1086
if(!({struct _dyneither_ptr _tmp71B=(struct _dyneither_ptr)_tmp34C;Cyc_NewControlFlow_strcmp(_tmp71B,({const char*_tmp176="debug_off";_tag_dyneither(_tmp176,sizeof(char),10);}));}));else{
# 1092
({struct Cyc_String_pa_PrintArg_struct _tmp179;_tmp179.tag=0;_tmp179.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34C);({void*_tmp177[1]={& _tmp179};({unsigned int _tmp71D=e->loc;struct _dyneither_ptr _tmp71C=({const char*_tmp178="unknown pragma %s";_tag_dyneither(_tmp178,sizeof(char),18);});Cyc_Tcutil_warn(_tmp71D,_tmp71C,_tag_dyneither(_tmp177,sizeof(void*),1));});});});}}}
return({struct _tuple16 _tmp17A;_tmp17A.f1=inflow;_tmp17A.f2=_tmp14C->zero;_tmp17A;});case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp164)->f1).Int_c).tag){case 1: _LL144: _LL145:
# 1095
 goto _LL147;case 5: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp164)->f1).Int_c).val).f2 == 0){_LL146: _LL147:
 return({struct _tuple16 _tmp17B;_tmp17B.f1=inflow;_tmp17B.f2=_tmp14C->zero;_tmp17B;});}else{_LL148: _LL149:
 goto _LL14B;}default: _LL14E: _LL14F:
# 1101
 goto _LL151;}case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp164)->f1)){case 2: _LL14A: _LL14B:
# 1098
 return({struct _tuple16 _tmp17C;_tmp17C.f1=inflow;_tmp17C.f2=_tmp14C->notzeroall;_tmp17C;});case 1: _LL15A: _LL15B:
# 1109
 return({struct _tuple16 _tmp17E;_tmp17E.f1=inflow;({void*_tmp71E=Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,(void*)_check_null(e->topt),0,_tmp14C->unknown_all);_tmp17E.f2=_tmp71E;});_tmp17E;});case 3: _LL15C: _tmp34D=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp164)->f1)->f1;_LL15D:
# 1112
 _tmp34E=_tmp34D;goto _LL15F;case 4: _LL15E: _tmp34E=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp164)->f1)->f1;_LL15F:
 _tmp34F=_tmp34E;goto _LL161;case 5: _LL160: _tmp34F=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp164)->f1)->f1;_LL161: {
# 1116
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp180;_tmp180.tag=0;_tmp180.f1=_tmp34F;_tmp180;});
return({struct _tuple16 _tmp17F;_tmp17F.f1=inflow;({void*_tmp71F=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot);_tmp17F.f2=_tmp71F;});_tmp17F;});}default: _LL19A: _LL19B:
# 1713
 goto _LL19D;}case 31: if(((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp164)->f1 == 0){_LL14C: _LL14D:
# 1100
 goto _LL14F;}else{_LL188: _tmp351=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp350=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL189:
# 1571
 _tmp378=_tmp351;goto _LL18B;}case 18: _LL150: _LL151:
# 1102
 goto _LL153;case 17: _LL152: _LL153:
 goto _LL155;case 19: _LL154: _LL155:
 goto _LL157;case 33: _LL156: _LL157:
 goto _LL159;case 32: _LL158: _LL159:
 return({struct _tuple16 _tmp17D;_tmp17D.f1=inflow;_tmp17D.f2=_tmp14C->unknown_all;_tmp17D;});case 3: _LL162: _tmp353=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp352=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL163: {
# 1123
struct _tuple16 _tmp181=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp352);struct _tuple16 _tmp182=_tmp181;union Cyc_CfFlowInfo_FlowInfo _tmp185;void*_tmp184;_LL1AB: _tmp185=_tmp182.f1;_tmp184=_tmp182.f2;_LL1AC:;
return({struct _tuple16 _tmp183;_tmp183.f1=_tmp185;_tmp183.f2=_tmp184;_tmp183;});}case 5: _LL164: _tmp354=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL165: {
# 1127
struct Cyc_List_List _tmp186=({struct Cyc_List_List _tmp194;_tmp194.hd=_tmp354;_tmp194.tl=0;_tmp194;});
struct _tuple16 _tmp187=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp186);struct _tuple16 _tmp188=_tmp187;union Cyc_CfFlowInfo_FlowInfo _tmp193;_LL1AE: _tmp193=_tmp188.f1;_LL1AF:;{
struct _tuple18 _tmp189=Cyc_NewControlFlow_anal_Lexp(env,_tmp193,0,0,_tmp354);struct _tuple18 _tmp18A=_tmp189;union Cyc_CfFlowInfo_AbsLVal _tmp192;_LL1B1: _tmp192=_tmp18A.f2;_LL1B2:;
{struct _tuple25 _tmp18B=({struct _tuple25 _tmp190;_tmp190.f1=_tmp192;_tmp190.f2=_tmp193;_tmp190;});struct _tuple25 _tmp18C=_tmp18B;struct Cyc_CfFlowInfo_Place*_tmp18F;struct Cyc_Dict_Dict _tmp18E;struct Cyc_List_List*_tmp18D;if(((_tmp18C.f1).PlaceL).tag == 1){if(((_tmp18C.f2).ReachableFL).tag == 2){_LL1B4: _tmp18F=((_tmp18C.f1).PlaceL).val;_tmp18E=(((_tmp18C.f2).ReachableFL).val).f1;_tmp18D=(((_tmp18C.f2).ReachableFL).val).f2;_LL1B5:
# 1132
({struct Cyc_List_List*_tmp720=Cyc_Relations_reln_kill_exp(_tmp14C->r,_tmp18D,_tmp354);_tmp18D=_tmp720;});
({union Cyc_CfFlowInfo_FlowInfo _tmp722=({struct Cyc_Dict_Dict _tmp721=Cyc_CfFlowInfo_assign_place(_tmp14C,_tmp354->loc,_tmp18E,_tmp18F,_tmp14C->unknown_all);Cyc_CfFlowInfo_ReachableFL(_tmp721,_tmp18D);});_tmp193=_tmp722;});
# 1137
goto _LL1B3;}else{goto _LL1B6;}}else{_LL1B6: _LL1B7:
 goto _LL1B3;}_LL1B3:;}
# 1140
return({struct _tuple16 _tmp191;_tmp191.f1=_tmp193;_tmp191.f2=_tmp14C->unknown_all;_tmp191;});};}case 4: if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp164)->f2 != 0){_LL166: _tmp356=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp355=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp164)->f3;_LL167:
# 1143
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp195=0;({unsigned int _tmp724=e->loc;struct _dyneither_ptr _tmp723=({const char*_tmp196="cannot track alias-free pointers through multiple assignments";_tag_dyneither(_tmp196,sizeof(char),62);});Cyc_CfFlowInfo_aerr(_tmp724,_tmp723,_tag_dyneither(_tmp195,sizeof(void*),0));});});
return({struct _tuple16 _tmp197;({union Cyc_CfFlowInfo_FlowInfo _tmp725=Cyc_CfFlowInfo_BottomFL();_tmp197.f1=_tmp725;});_tmp197.f2=_tmp14C->unknown_all;_tmp197;});}{
# 1147
struct _tuple16 _tmp198=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp355);struct _tuple16 _tmp199=_tmp198;union Cyc_CfFlowInfo_FlowInfo _tmp1A5;void*_tmp1A4;_LL1B9: _tmp1A5=_tmp199.f1;_tmp1A4=_tmp199.f2;_LL1BA:;{
struct _tuple18 _tmp19A=Cyc_NewControlFlow_anal_Lexp(env,_tmp1A5,0,0,_tmp356);struct _tuple18 _tmp19B=_tmp19A;union Cyc_CfFlowInfo_FlowInfo _tmp1A3;union Cyc_CfFlowInfo_AbsLVal _tmp1A2;_LL1BC: _tmp1A3=_tmp19B.f1;_tmp1A2=_tmp19B.f2;_LL1BD:;
{union Cyc_CfFlowInfo_FlowInfo _tmp19C=_tmp1A3;struct Cyc_Dict_Dict _tmp1A0;struct Cyc_List_List*_tmp19F;if((_tmp19C.ReachableFL).tag == 2){_LL1BF: _tmp1A0=((_tmp19C.ReachableFL).val).f1;_tmp19F=((_tmp19C.ReachableFL).val).f2;_LL1C0:
# 1151
{union Cyc_CfFlowInfo_AbsLVal _tmp19D=_tmp1A2;struct Cyc_CfFlowInfo_Place*_tmp19E;if((_tmp19D.PlaceL).tag == 1){_LL1C4: _tmp19E=(_tmp19D.PlaceL).val;_LL1C5:
# 1155
({struct Cyc_List_List*_tmp726=Cyc_Relations_reln_kill_exp(_tmp14C->r,_tmp19F,_tmp356);_tmp19F=_tmp726;});
({struct Cyc_Dict_Dict _tmp727=Cyc_CfFlowInfo_assign_place(_tmp14C,_tmp356->loc,_tmp1A0,_tmp19E,_tmp14C->unknown_all);_tmp1A0=_tmp727;});
# 1158
({union Cyc_CfFlowInfo_FlowInfo _tmp728=Cyc_CfFlowInfo_ReachableFL(_tmp1A0,_tmp19F);_tmp1A3=_tmp728;});
# 1162
goto _LL1C3;}else{_LL1C6: _LL1C7:
# 1165
 goto _LL1C3;}_LL1C3:;}
# 1167
goto _LL1BE;}else{_LL1C1: _LL1C2:
 goto _LL1BE;}_LL1BE:;}
# 1170
return({struct _tuple16 _tmp1A1;_tmp1A1.f1=_tmp1A3;_tmp1A1.f2=_tmp14C->unknown_all;_tmp1A1;});};};}else{_LL168: _tmp358=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp357=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp164)->f3;_LL169:
# 1173
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp1A6=0;({unsigned int _tmp72A=e->loc;struct _dyneither_ptr _tmp729=({const char*_tmp1A7="cannot track alias-free pointers through multiple assignments";_tag_dyneither(_tmp1A7,sizeof(char),62);});Cyc_CfFlowInfo_aerr(_tmp72A,_tmp729,_tag_dyneither(_tmp1A6,sizeof(void*),0));});});
return({struct _tuple16 _tmp1A8;({union Cyc_CfFlowInfo_FlowInfo _tmp72B=Cyc_CfFlowInfo_BottomFL();_tmp1A8.f1=_tmp72B;});_tmp1A8.f2=_tmp14C->unknown_all;_tmp1A8;});}
# 1179
({union Cyc_CfFlowInfo_FlowInfo _tmp72F=({struct Cyc_NewControlFlow_AnalEnv*_tmp72E=env;union Cyc_CfFlowInfo_FlowInfo _tmp72D=inflow;Cyc_NewControlFlow_expand_unique_places(_tmp72E,_tmp72D,({struct Cyc_Absyn_Exp*_tmp1A9[2];_tmp1A9[1]=_tmp357;_tmp1A9[0]=_tmp358;({struct _RegionHandle*_tmp72C=env->r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp72C,_tag_dyneither(_tmp1A9,sizeof(struct Cyc_Absyn_Exp*),2));});}));});inflow=_tmp72F;});{
# 1181
struct _tuple16 _tmp1AA=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp357);struct _tuple16 _tmp1AB=_tmp1AA;union Cyc_CfFlowInfo_FlowInfo _tmp1B6;void*_tmp1B5;_LL1C9: _tmp1B6=_tmp1AB.f1;_tmp1B5=_tmp1AB.f2;_LL1CA:;{
struct _tuple18 _tmp1AC=Cyc_NewControlFlow_anal_Lexp(env,_tmp1B6,0,0,_tmp358);struct _tuple18 _tmp1AD=_tmp1AC;union Cyc_CfFlowInfo_FlowInfo _tmp1B4;union Cyc_CfFlowInfo_AbsLVal _tmp1B3;_LL1CC: _tmp1B4=_tmp1AD.f1;_tmp1B3=_tmp1AD.f2;_LL1CD:;{
struct _tuple16 _tmp1AE=Cyc_NewControlFlow_do_assign(_tmp14C,env,_tmp1B4,_tmp358,_tmp1B3,_tmp357,_tmp1B5,e->loc);struct _tuple16 _tmp1AF=_tmp1AE;union Cyc_CfFlowInfo_FlowInfo _tmp1B2;void*_tmp1B1;_LL1CF: _tmp1B2=_tmp1AF.f1;_tmp1B1=_tmp1AF.f2;_LL1D0:;
# 1187
return({struct _tuple16 _tmp1B0;_tmp1B0.f1=_tmp1B2;_tmp1B0.f2=_tmp1B1;_tmp1B0;});};};};}case 9: _LL16A: _tmp35A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp359=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL16B: {
# 1190
struct _tuple16 _tmp1B7=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp35A);struct _tuple16 _tmp1B8=_tmp1B7;union Cyc_CfFlowInfo_FlowInfo _tmp1BA;void*_tmp1B9;_LL1D2: _tmp1BA=_tmp1B8.f1;_tmp1B9=_tmp1B8.f2;_LL1D3:;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp1BA,_tmp359);}case 11: _LL16C: _tmp35B=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL16D: {
# 1194
struct _tuple16 _tmp1BB=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp35B);struct _tuple16 _tmp1BC=_tmp1BB;union Cyc_CfFlowInfo_FlowInfo _tmp1BF;void*_tmp1BE;_LL1D5: _tmp1BF=_tmp1BC.f1;_tmp1BE=_tmp1BC.f2;_LL1D6:;
Cyc_NewControlFlow_use_Rval(env,_tmp35B->loc,_tmp1BF,_tmp1BE);
return({struct _tuple16 _tmp1BD;({union Cyc_CfFlowInfo_FlowInfo _tmp730=Cyc_CfFlowInfo_BottomFL();_tmp1BD.f1=_tmp730;});({void*_tmp731=Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,(void*)_check_null(e->topt),0,_tmp14C->unknown_all);_tmp1BD.f2=_tmp731;});_tmp1BD;});}case 10: _LL16E: _tmp35D=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp35C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL16F: {
# 1199
struct _RegionHandle _tmp1C0=_new_region("temp");struct _RegionHandle*temp=& _tmp1C0;_push_region(temp);
{struct Cyc_List_List*_tmp1C1=_tmp35C;
struct _tuple23 _tmp1C2=({struct _RegionHandle*_tmp735=temp;struct Cyc_NewControlFlow_AnalEnv*_tmp734=env;union Cyc_CfFlowInfo_FlowInfo _tmp733=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp735,_tmp734,_tmp733,({struct Cyc_List_List*_tmp1F4=_region_malloc(temp,sizeof(*_tmp1F4));_tmp1F4->hd=_tmp35D;({struct Cyc_List_List*_tmp732=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp35C);_tmp1F4->tl=_tmp732;});_tmp1F4;}),0,1);});struct _tuple23 _tmp1C3=_tmp1C2;union Cyc_CfFlowInfo_FlowInfo _tmp1F3;struct Cyc_List_List*_tmp1F2;_LL1D8: _tmp1F3=_tmp1C3.f1;_tmp1F2=_tmp1C3.f2;_LL1D9:;{
# 1203
union Cyc_CfFlowInfo_FlowInfo _tmp1C4=Cyc_NewControlFlow_use_Rval(env,_tmp35D->loc,_tmp1F3,(void*)((struct Cyc_List_List*)_check_null(_tmp1F2))->hd);
_tmp1F2=_tmp1F2->tl;{
# 1206
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
struct Cyc_List_List*requires;
struct Cyc_List_List*ensures;
{void*_tmp1C5=Cyc_Tcutil_compress((void*)_check_null(_tmp35D->topt));void*_tmp1C6=_tmp1C5;void*_tmp1D8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C6)->tag == 5){_LL1DB: _tmp1D8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C6)->f1).elt_typ;_LL1DC:
# 1213
{void*_tmp1C7=Cyc_Tcutil_compress(_tmp1D8);void*_tmp1C8=_tmp1C7;struct Cyc_List_List*_tmp1D5;struct Cyc_List_List*_tmp1D4;struct Cyc_List_List*_tmp1D3;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->tag == 9){_LL1E0: _tmp1D5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).attributes;_tmp1D4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).requires_relns;_tmp1D3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).ensures_relns;_LL1E1:
# 1215
 requires=_tmp1D4;
ensures=_tmp1D3;
for(0;_tmp1D5 != 0;_tmp1D5=_tmp1D5->tl){
# 1219
void*_tmp1C9=(void*)_tmp1D5->hd;void*_tmp1CA=_tmp1C9;int _tmp1D0;int _tmp1CF;int _tmp1CE;switch(*((int*)_tmp1CA)){case 20: _LL1E5: _tmp1CE=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1CA)->f1;_LL1E6:
# 1221
({struct Cyc_List_List*_tmp736=({struct Cyc_List_List*_tmp1CB=_region_malloc(temp,sizeof(*_tmp1CB));_tmp1CB->hd=(void*)_tmp1CE;_tmp1CB->tl=init_params;_tmp1CB;});init_params=_tmp736;});goto _LL1E4;case 21: _LL1E7: _tmp1CF=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp1CA)->f1;_LL1E8:
# 1223
({struct Cyc_List_List*_tmp737=({struct Cyc_List_List*_tmp1CC=_region_malloc(temp,sizeof(*_tmp1CC));_tmp1CC->hd=(void*)_tmp1CF;_tmp1CC->tl=nolive_unique_params;_tmp1CC;});nolive_unique_params=_tmp737;});
goto _LL1E4;case 22: _LL1E9: _tmp1D0=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp1CA)->f1;_LL1EA:
# 1227
({struct Cyc_List_List*_tmp738=({struct Cyc_List_List*_tmp1CD=_region_malloc(temp,sizeof(*_tmp1CD));_tmp1CD->hd=(void*)_tmp1D0;_tmp1CD->tl=noconsume_params;_tmp1CD;});noconsume_params=_tmp738;});
goto _LL1E4;default: _LL1EB: _LL1EC:
 goto _LL1E4;}_LL1E4:;}
# 1231
goto _LL1DF;}else{_LL1E2: _LL1E3:
({void*_tmp1D1=0;({struct _dyneither_ptr _tmp739=({const char*_tmp1D2="anal_Rexp: bad function type";_tag_dyneither(_tmp1D2,sizeof(char),29);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp739,_tag_dyneither(_tmp1D1,sizeof(void*),0));});});}_LL1DF:;}
# 1234
goto _LL1DA;}else{_LL1DD: _LL1DE:
({void*_tmp1D6=0;({struct _dyneither_ptr _tmp73A=({const char*_tmp1D7="anal_Rexp: bad function type";_tag_dyneither(_tmp1D7,sizeof(char),29);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp73A,_tag_dyneither(_tmp1D6,sizeof(void*),0));});});}_LL1DA:;}
# 1237
{int i=1;for(0;_tmp1F2 != 0;(((_tmp1F2=_tmp1F2->tl,_tmp35C=((struct Cyc_List_List*)_check_null(_tmp35C))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp1D9=_tmp1F3;struct Cyc_Dict_Dict _tmp1ED;if((_tmp1D9.BottomFL).tag == 1){_LL1EE: _LL1EF:
 goto _LL1ED;}else{_LL1F0: _tmp1ED=((_tmp1D9.ReachableFL).val).f1;_LL1F1: {
# 1242
struct _tuple15 _tmp1DA=Cyc_CfFlowInfo_unname_rval(_tmp14C->r,(void*)_tmp1F2->hd);struct _tuple15 _tmp1DB=_tmp1DA;void*_tmp1EC;_LL1F3: _tmp1EC=_tmp1DB.f1;_LL1F4:;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp1ED,(void*)_tmp1F2->hd)== Cyc_CfFlowInfo_NoneIL  && !Cyc_CfFlowInfo_is_init_pointer((void*)_tmp1F2->hd))
({void*_tmp1DC=0;({unsigned int _tmp73C=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp35C))->hd)->loc;struct _dyneither_ptr _tmp73B=({const char*_tmp1DD="expression may not be initialized";_tag_dyneither(_tmp1DD,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp73C,_tmp73B,_tag_dyneither(_tmp1DC,sizeof(void*),0));});});
{union Cyc_CfFlowInfo_FlowInfo _tmp1DE=_tmp1C4;struct Cyc_Dict_Dict _tmp1EB;struct Cyc_List_List*_tmp1EA;if((_tmp1DE.BottomFL).tag == 1){_LL1F6: _LL1F7:
 goto _LL1F5;}else{_LL1F8: _tmp1EB=((_tmp1DE.ReachableFL).val).f1;_tmp1EA=((_tmp1DE.ReachableFL).val).f2;_LL1F9: {
# 1250
struct Cyc_Dict_Dict _tmp1DF=Cyc_CfFlowInfo_escape_deref(_tmp14C,_tmp1EB,(void*)_tmp1F2->hd);
{void*_tmp1E0=(void*)_tmp1F2->hd;void*_tmp1E1=_tmp1E0;struct Cyc_CfFlowInfo_Place*_tmp1E9;switch(*((int*)_tmp1E1)){case 4: _LL1FB: _tmp1E9=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E1)->f1;_LL1FC:
# 1253
{void*_tmp1E2=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp35C))->hd)->topt));void*_tmp1E3=_tmp1E2;void*_tmp1E6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E3)->tag == 5){_LL202: _tmp1E6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E3)->f1).elt_typ;_LL203:
# 1255
({struct Cyc_Dict_Dict _tmp741=({struct Cyc_CfFlowInfo_FlowEnv*_tmp740=_tmp14C;unsigned int _tmp73F=((struct Cyc_Absyn_Exp*)_tmp35C->hd)->loc;struct Cyc_Dict_Dict _tmp73E=_tmp1DF;struct Cyc_CfFlowInfo_Place*_tmp73D=_tmp1E9;Cyc_CfFlowInfo_assign_place(_tmp740,_tmp73F,_tmp73E,_tmp73D,
Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,_tmp1E6,0,_tmp14C->esc_all));});
# 1255
_tmp1DF=_tmp741;});
# 1259
goto _LL201;}else{_LL204: _LL205:
({void*_tmp1E4=0;({struct _dyneither_ptr _tmp742=({const char*_tmp1E5="anal_Rexp:bad type for initialized arg";_tag_dyneither(_tmp1E5,sizeof(char),39);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp742,_tag_dyneither(_tmp1E4,sizeof(void*),0));});});}_LL201:;}
# 1262
goto _LL1FA;case 5: _LL1FD: _LL1FE:
# 1264
({void*_tmp1E7=0;({struct _dyneither_ptr _tmp743=({const char*_tmp1E8="anal_Rexp:initialize attribute on unique pointers not yet supported";_tag_dyneither(_tmp1E8,sizeof(char),68);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp743,_tag_dyneither(_tmp1E7,sizeof(void*),0));});});default: _LL1FF: _LL200:
# 1266
 goto _LL1FA;}_LL1FA:;}
# 1268
({union Cyc_CfFlowInfo_FlowInfo _tmp744=Cyc_CfFlowInfo_ReachableFL(_tmp1DF,_tmp1EA);_tmp1C4=_tmp744;});
goto _LL1F5;}}_LL1F5:;}
# 1271
goto _LL1ED;}}_LL1ED:;}else{
# 1274
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1279
({union Cyc_CfFlowInfo_FlowInfo _tmp749=({struct Cyc_NewControlFlow_AnalEnv*_tmp748=env;unsigned int _tmp747=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp35C))->hd)->loc;void*_tmp746=(void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp35C->hd)->topt);union Cyc_CfFlowInfo_FlowInfo _tmp745=_tmp1C4;Cyc_NewControlFlow_use_nounique_Rval(_tmp748,_tmp747,_tmp746,_tmp745,(void*)_tmp1F2->hd);});_tmp1C4=_tmp749;});else{
# 1289
({union Cyc_CfFlowInfo_FlowInfo _tmp74A=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp35C))->hd)->loc,_tmp1C4,(void*)_tmp1F2->hd);_tmp1C4=_tmp74A;});
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp35C->hd)->topt),0))
({union Cyc_CfFlowInfo_FlowInfo _tmp74B=Cyc_NewControlFlow_restore_noconsume_arg(env,_tmp1C4,(struct Cyc_Absyn_Exp*)_tmp35C->hd,((struct Cyc_Absyn_Exp*)_tmp35C->hd)->loc,(void*)_tmp1F2->hd);_tmp1C4=_tmp74B;});}}}}
# 1296
Cyc_NewControlFlow_check_fn_requires(env,_tmp1C4,_tmp1C1,requires,e->loc);
# 1299
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp35D->topt))){
struct _tuple16 _tmp1EF=({struct _tuple16 _tmp1EE;({union Cyc_CfFlowInfo_FlowInfo _tmp74C=Cyc_CfFlowInfo_BottomFL();_tmp1EE.f1=_tmp74C;});({void*_tmp74D=Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,(void*)_check_null(e->topt),0,_tmp14C->unknown_all);_tmp1EE.f2=_tmp74D;});_tmp1EE;});_npop_handler(0);return _tmp1EF;}else{
# 1302
struct _tuple16 _tmp1F1=({struct _tuple16 _tmp1F0;_tmp1F0.f1=_tmp1C4;({void*_tmp74E=Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,(void*)_check_null(e->topt),0,_tmp14C->unknown_all);_tmp1F0.f2=_tmp74E;});_tmp1F0;});_npop_handler(0);return _tmp1F1;}};};}
# 1200
;_pop_region(temp);}case 34: _LL170: _tmp362=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp164)->f1).is_calloc;_tmp361=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp164)->f1).rgn;_tmp360=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp164)->f1).elt_type;_tmp35F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp164)->f1).num_elts;_tmp35E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp164)->f1).fat_result;_LL171: {
# 1305
void*place_val;
if(_tmp35E)place_val=_tmp14C->notzeroall;else{
if(_tmp362)({void*_tmp74F=Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,*((void**)_check_null(_tmp360)),0,_tmp14C->zero);place_val=_tmp74F;});else{
({void*_tmp750=Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,*((void**)_check_null(_tmp360)),0,_tmp14C->unknown_none);place_val=_tmp750;});}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
if(_tmp361 != 0){
struct _RegionHandle _tmp1F5=_new_region("temp");struct _RegionHandle*temp=& _tmp1F5;_push_region(temp);
{struct _tuple23 _tmp1F6=({struct _RegionHandle*_tmp754=temp;struct Cyc_NewControlFlow_AnalEnv*_tmp753=env;union Cyc_CfFlowInfo_FlowInfo _tmp752=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp754,_tmp753,_tmp752,({struct Cyc_Absyn_Exp*_tmp1FA[2];_tmp1FA[1]=_tmp35F;_tmp1FA[0]=_tmp361;({struct _RegionHandle*_tmp751=temp;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp751,_tag_dyneither(_tmp1FA,sizeof(struct Cyc_Absyn_Exp*),2));});}),1,1);});struct _tuple23 _tmp1F7=_tmp1F6;union Cyc_CfFlowInfo_FlowInfo _tmp1F9;struct Cyc_List_List*_tmp1F8;_LL207: _tmp1F9=_tmp1F7.f1;_tmp1F8=_tmp1F7.f2;_LL208:;
# 1314
outflow=_tmp1F9;}
# 1312
;_pop_region(temp);}else{
# 1317
struct _tuple16 _tmp1FB=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp35F);struct _tuple16 _tmp1FC=_tmp1FB;union Cyc_CfFlowInfo_FlowInfo _tmp1FD;_LL20A: _tmp1FD=_tmp1FC.f1;_LL20B:;
outflow=_tmp1FD;}
# 1322
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e->topt),1))
return({struct _tuple16 _tmp1FE;_tmp1FE.f1=outflow;({void*_tmp756=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp1FF=_cycalloc(sizeof(*_tmp1FF));({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp755=({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp200;_tmp200.tag=5;_tmp200.f1=place_val;_tmp200;});_tmp1FF[0]=_tmp755;});_tmp1FF;});_tmp1FE.f2=_tmp756;});_tmp1FE;});else{
# 1326
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp209=_region_malloc(_tmp14C->r,sizeof(*_tmp209));({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp757=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp20A;_tmp20A.tag=1;_tmp20A.f1=_tmp35F;_tmp20A.f2=(void*)_check_null(e->topt);_tmp20A;});_tmp209[0]=_tmp757;});_tmp209;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp208=_region_malloc(_tmp14C->r,sizeof(*_tmp208));_tmp208->root=root;_tmp208->path=0;_tmp208;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp206=_region_malloc(_tmp14C->r,sizeof(*_tmp206));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp758=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp207;_tmp207.tag=4;_tmp207.f1=place;_tmp207;});_tmp206[0]=_tmp758;});_tmp206;});
union Cyc_CfFlowInfo_FlowInfo _tmp201=outflow;struct Cyc_Dict_Dict _tmp205;struct Cyc_List_List*_tmp204;if((_tmp201.BottomFL).tag == 1){_LL20D: _LL20E:
 return({struct _tuple16 _tmp202;_tmp202.f1=outflow;_tmp202.f2=rval;_tmp202;});}else{_LL20F: _tmp205=((_tmp201.ReachableFL).val).f1;_tmp204=((_tmp201.ReachableFL).val).f2;_LL210:
# 1332
 return({struct _tuple16 _tmp203;({union Cyc_CfFlowInfo_FlowInfo _tmp75A=({struct Cyc_Dict_Dict _tmp759=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp205,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp759,_tmp204);});_tmp203.f1=_tmp75A;});_tmp203.f2=rval;_tmp203;});}_LL20C:;}};}case 35: _LL172: _tmp364=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp363=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL173: {
# 1337
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1343
struct _RegionHandle _tmp20B=_new_region("temp");struct _RegionHandle*temp=& _tmp20B;_push_region(temp);{
struct _tuple23 _tmp20C=({struct _RegionHandle*_tmp75E=temp;struct Cyc_NewControlFlow_AnalEnv*_tmp75D=env;union Cyc_CfFlowInfo_FlowInfo _tmp75C=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp75E,_tmp75D,_tmp75C,({struct Cyc_Absyn_Exp*_tmp210[2];_tmp210[1]=_tmp363;_tmp210[0]=_tmp364;({struct _RegionHandle*_tmp75B=temp;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp75B,_tag_dyneither(_tmp210,sizeof(struct Cyc_Absyn_Exp*),2));});}),0,0);});struct _tuple23 _tmp20D=_tmp20C;union Cyc_CfFlowInfo_FlowInfo _tmp20F;struct Cyc_List_List*_tmp20E;_LL212: _tmp20F=_tmp20D.f1;_tmp20E=_tmp20D.f2;_LL213:;
# 1346
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp20E))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp20E->tl))->hd;
outflow=_tmp20F;}{
# 1351
struct _tuple18 _tmp211=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp364);struct _tuple18 _tmp212=_tmp211;union Cyc_CfFlowInfo_AbsLVal _tmp223;_LL215: _tmp223=_tmp212.f2;_LL216:;{
struct _tuple18 _tmp213=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp363);struct _tuple18 _tmp214=_tmp213;union Cyc_CfFlowInfo_AbsLVal _tmp222;_LL218: _tmp222=_tmp214.f2;_LL219:;
{union Cyc_CfFlowInfo_FlowInfo _tmp215=outflow;struct Cyc_Dict_Dict _tmp21F;struct Cyc_List_List*_tmp21E;if((_tmp215.ReachableFL).tag == 2){_LL21B: _tmp21F=((_tmp215.ReachableFL).val).f1;_tmp21E=((_tmp215.ReachableFL).val).f2;_LL21C:
# 1355
{union Cyc_CfFlowInfo_AbsLVal _tmp216=_tmp223;struct Cyc_CfFlowInfo_Place*_tmp219;if((_tmp216.PlaceL).tag == 1){_LL220: _tmp219=(_tmp216.PlaceL).val;_LL221:
# 1357
({struct Cyc_Dict_Dict _tmp75F=Cyc_CfFlowInfo_assign_place(_tmp14C,_tmp364->loc,_tmp21F,_tmp219,right_rval);_tmp21F=_tmp75F;});
goto _LL21F;}else{_LL222: _LL223:
# 1363
 if(Cyc_CfFlowInfo_initlevel(_tmp14C,_tmp21F,right_rval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp217=0;({unsigned int _tmp761=_tmp363->loc;struct _dyneither_ptr _tmp760=({const char*_tmp218="expression may not be fully initialized";_tag_dyneither(_tmp218,sizeof(char),40);});Cyc_Tcutil_terr(_tmp761,_tmp760,_tag_dyneither(_tmp217,sizeof(void*),0));});});
goto _LL21F;}_LL21F:;}
# 1367
{union Cyc_CfFlowInfo_AbsLVal _tmp21A=_tmp222;struct Cyc_CfFlowInfo_Place*_tmp21D;if((_tmp21A.PlaceL).tag == 1){_LL225: _tmp21D=(_tmp21A.PlaceL).val;_LL226:
# 1369
({struct Cyc_Dict_Dict _tmp762=Cyc_CfFlowInfo_assign_place(_tmp14C,_tmp363->loc,_tmp21F,_tmp21D,left_rval);_tmp21F=_tmp762;});
goto _LL224;}else{_LL227: _LL228:
# 1372
 if(Cyc_CfFlowInfo_initlevel(_tmp14C,_tmp21F,left_rval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp21B=0;({unsigned int _tmp764=_tmp364->loc;struct _dyneither_ptr _tmp763=({const char*_tmp21C="expression may not be fully initialized";_tag_dyneither(_tmp21C,sizeof(char),40);});Cyc_Tcutil_terr(_tmp764,_tmp763,_tag_dyneither(_tmp21B,sizeof(void*),0));});});
goto _LL224;}_LL224:;}
# 1377
({struct Cyc_List_List*_tmp765=Cyc_Relations_reln_kill_exp(_tmp14C->r,_tmp21E,_tmp364);_tmp21E=_tmp765;});
({struct Cyc_List_List*_tmp766=Cyc_Relations_reln_kill_exp(_tmp14C->r,_tmp21E,_tmp363);_tmp21E=_tmp766;});
# 1380
({union Cyc_CfFlowInfo_FlowInfo _tmp767=Cyc_CfFlowInfo_ReachableFL(_tmp21F,_tmp21E);outflow=_tmp767;});
Cyc_NewControlFlow_update_tryflow(env,outflow);
goto _LL21A;}else{_LL21D: _LL21E:
 goto _LL21A;}_LL21A:;}{
# 1387
struct _tuple16 _tmp221=({struct _tuple16 _tmp220;_tmp220.f1=outflow;_tmp220.f2=_tmp14C->unknown_all;_tmp220;});_npop_handler(0);return _tmp221;};};};
# 1343
;_pop_region(temp);}case 16: _LL174: _tmp366=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp365=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL175: {
# 1390
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp366 != 0){
struct _RegionHandle _tmp224=_new_region("temp");struct _RegionHandle*temp=& _tmp224;_push_region(temp);
{struct _tuple23 _tmp225=({struct _RegionHandle*_tmp76B=temp;struct Cyc_NewControlFlow_AnalEnv*_tmp76A=env;union Cyc_CfFlowInfo_FlowInfo _tmp769=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp76B,_tmp76A,_tmp769,({struct Cyc_Absyn_Exp*_tmp229[2];_tmp229[1]=_tmp365;_tmp229[0]=_tmp366;({struct _RegionHandle*_tmp768=temp;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp768,_tag_dyneither(_tmp229,sizeof(struct Cyc_Absyn_Exp*),2));});}),1,1);});struct _tuple23 _tmp226=_tmp225;union Cyc_CfFlowInfo_FlowInfo _tmp228;struct Cyc_List_List*_tmp227;_LL22A: _tmp228=_tmp226.f1;_tmp227=_tmp226.f2;_LL22B:;
# 1396
outflow=_tmp228;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp227))->tl))->hd;}
# 1394
;_pop_region(temp);}else{
# 1400
struct _tuple16 _tmp22A=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp365);struct _tuple16 _tmp22B=_tmp22A;union Cyc_CfFlowInfo_FlowInfo _tmp22D;void*_tmp22C;_LL22D: _tmp22D=_tmp22B.f1;_tmp22C=_tmp22B.f2;_LL22E:;
outflow=_tmp22D;
place_val=_tmp22C;}
# 1405
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e->topt),1))
return({struct _tuple16 _tmp22E;_tmp22E.f1=outflow;({void*_tmp76D=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp22F=_cycalloc(sizeof(*_tmp22F));({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp76C=({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp230;_tmp230.tag=5;_tmp230.f1=place_val;_tmp230;});_tmp22F[0]=_tmp76C;});_tmp22F;});_tmp22E.f2=_tmp76D;});_tmp22E;});else{
# 1409
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp239=_region_malloc(_tmp14C->r,sizeof(*_tmp239));({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp76E=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp23A;_tmp23A.tag=1;_tmp23A.f1=_tmp365;_tmp23A.f2=(void*)_check_null(e->topt);_tmp23A;});_tmp239[0]=_tmp76E;});_tmp239;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp238=_region_malloc(_tmp14C->r,sizeof(*_tmp238));_tmp238->root=root;_tmp238->path=0;_tmp238;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp236=_region_malloc(_tmp14C->r,sizeof(*_tmp236));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp76F=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp237;_tmp237.tag=4;_tmp237.f1=place;_tmp237;});_tmp236[0]=_tmp76F;});_tmp236;});
union Cyc_CfFlowInfo_FlowInfo _tmp231=outflow;struct Cyc_Dict_Dict _tmp235;struct Cyc_List_List*_tmp234;if((_tmp231.BottomFL).tag == 1){_LL230: _LL231:
 return({struct _tuple16 _tmp232;_tmp232.f1=outflow;_tmp232.f2=rval;_tmp232;});}else{_LL232: _tmp235=((_tmp231.ReachableFL).val).f1;_tmp234=((_tmp231.ReachableFL).val).f2;_LL233:
# 1415
 return({struct _tuple16 _tmp233;({union Cyc_CfFlowInfo_FlowInfo _tmp771=({struct Cyc_Dict_Dict _tmp770=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp235,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp770,_tmp234);});_tmp233.f1=_tmp771;});_tmp233.f2=rval;_tmp233;});}_LL22F:;}}case 15: _LL176: _tmp367=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL177: {
# 1420
struct _tuple18 _tmp23B=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp367);struct _tuple18 _tmp23C=_tmp23B;union Cyc_CfFlowInfo_FlowInfo _tmp244;union Cyc_CfFlowInfo_AbsLVal _tmp243;_LL235: _tmp244=_tmp23C.f1;_tmp243=_tmp23C.f2;_LL236:;{
union Cyc_CfFlowInfo_AbsLVal _tmp23D=_tmp243;struct Cyc_CfFlowInfo_Place*_tmp242;if((_tmp23D.UnknownL).tag == 2){_LL238: _LL239:
 return({struct _tuple16 _tmp23E;_tmp23E.f1=_tmp244;_tmp23E.f2=_tmp14C->notzeroall;_tmp23E;});}else{_LL23A: _tmp242=(_tmp23D.PlaceL).val;_LL23B:
 return({struct _tuple16 _tmp23F;_tmp23F.f1=_tmp244;({void*_tmp773=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp240=_region_malloc(env->r,sizeof(*_tmp240));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp772=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp241;_tmp241.tag=4;_tmp241.f1=_tmp242;_tmp241;});_tmp240[0]=_tmp772;});_tmp240;});_tmp23F.f2=_tmp773;});_tmp23F;});}_LL237:;};}case 20: _LL178: _tmp368=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL179: {
# 1427
struct _tuple16 _tmp245=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp368);struct _tuple16 _tmp246=_tmp245;union Cyc_CfFlowInfo_FlowInfo _tmp248;void*_tmp247;_LL23D: _tmp248=_tmp246.f1;_tmp247=_tmp246.f2;_LL23E:;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp248,_tmp368,_tmp247);}case 21: _LL17A: _tmp36A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp369=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL17B: {
# 1431
struct _tuple16 _tmp249=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp36A);struct _tuple16 _tmp24A=_tmp249;union Cyc_CfFlowInfo_FlowInfo _tmp267;void*_tmp266;_LL240: _tmp267=_tmp24A.f1;_tmp266=_tmp24A.f2;_LL241:;
if(_tmp36A->topt == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp24D;_tmp24D.tag=0;({struct _dyneither_ptr _tmp774=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp24D.f1=_tmp774;});({void*_tmp24B[1]={& _tmp24D};({struct Cyc___cycFILE*_tmp776=Cyc_stderr;struct _dyneither_ptr _tmp775=({const char*_tmp24C="aggrmember exp %s\n";_tag_dyneither(_tmp24C,sizeof(char),19);});Cyc_fprintf(_tmp776,_tmp775,_tag_dyneither(_tmp24B,sizeof(void*),1));});});});
({struct Cyc_String_pa_PrintArg_struct _tmp250;_tmp250.tag=0;({struct _dyneither_ptr _tmp777=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp36A));_tmp250.f1=_tmp777;});({void*_tmp24E[1]={& _tmp250};({struct Cyc___cycFILE*_tmp779=Cyc_stderr;struct _dyneither_ptr _tmp778=({const char*_tmp24F="oops! %s.topt = null!\n";_tag_dyneither(_tmp24F,sizeof(char),23);});Cyc_fprintf(_tmp779,_tmp778,_tag_dyneither(_tmp24E,sizeof(void*),1));});});});}{
# 1436
void*_tmp251=(void*)_check_null(_tmp36A->topt);
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp251))
# 1439
return({struct _tuple16 _tmp252;_tmp252.f1=_tmp267;({void*_tmp77A=Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,(void*)_check_null(e->topt),0,_tmp14C->unknown_all);_tmp252.f2=_tmp77A;});_tmp252;});
# 1441
if(Cyc_Absyn_is_require_union_type(_tmp251))
Cyc_NewControlFlow_check_union_requires(_tmp36A->loc,_tmp14C->r,_tmp251,_tmp369,_tmp267);{
# 1444
struct _tuple15 _tmp253=Cyc_CfFlowInfo_unname_rval(_tmp14C->r,_tmp266);struct _tuple15 _tmp254=_tmp253;void*_tmp265;_LL243: _tmp265=_tmp254.f1;_LL244:;{
void*_tmp255=_tmp265;int _tmp264;int _tmp263;struct _dyneither_ptr _tmp262;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp255)->tag == 6){_LL246: _tmp264=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp255)->f1).is_union;_tmp263=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp255)->f1).fieldnum;_tmp262=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp255)->f2;_LL247: {
# 1447
int _tmp256=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp36A->topt),_tmp369);
if((_tmp264  && _tmp263 != - 1) && _tmp263 != _tmp256)
return({struct _tuple16 _tmp257;_tmp257.f1=_tmp267;({void*_tmp77B=Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,(void*)_check_null(e->topt),1,_tmp14C->unknown_none);_tmp257.f2=_tmp77B;});_tmp257;});
return({struct _tuple16 _tmp258;_tmp258.f1=_tmp267;_tmp258.f2=*((void**)_check_dyneither_subscript(_tmp262,sizeof(void*),_tmp256));_tmp258;});}}else{_LL248: _LL249:
# 1452
({void*_tmp259=0;({struct Cyc___cycFILE*_tmp77D=Cyc_stderr;struct _dyneither_ptr _tmp77C=({const char*_tmp25A="the bad rexp is :";_tag_dyneither(_tmp25A,sizeof(char),18);});Cyc_fprintf(_tmp77D,_tmp77C,_tag_dyneither(_tmp259,sizeof(void*),0));});});
Cyc_CfFlowInfo_print_absrval(_tmp265);
({void*_tmp25B=0;({struct Cyc___cycFILE*_tmp77F=Cyc_stderr;struct _dyneither_ptr _tmp77E=({const char*_tmp25C="\n";_tag_dyneither(_tmp25C,sizeof(char),2);});Cyc_fprintf(_tmp77F,_tmp77E,_tag_dyneither(_tmp25B,sizeof(void*),0));});});
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp25D=_cycalloc(sizeof(*_tmp25D));({struct Cyc_Core_Impossible_exn_struct _tmp783=({struct Cyc_Core_Impossible_exn_struct _tmp25E;_tmp25E.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp782=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp261;_tmp261.tag=0;({struct _dyneither_ptr _tmp780=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp261.f1=_tmp780;});({void*_tmp25F[1]={& _tmp261};({struct _dyneither_ptr _tmp781=({const char*_tmp260="anal_Rexp: AggrMember: %s";_tag_dyneither(_tmp260,sizeof(char),26);});Cyc_aprintf(_tmp781,_tag_dyneither(_tmp25F,sizeof(void*),1));});});});_tmp25E.f1=_tmp782;});_tmp25E;});_tmp25D[0]=_tmp783;});_tmp25D;}));}_LL245:;};};};}case 38: _LL17C: _tmp36C=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp36B=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL17D: {
# 1462
struct _tuple16 _tmp268=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp36C);struct _tuple16 _tmp269=_tmp268;union Cyc_CfFlowInfo_FlowInfo _tmp27C;void*_tmp27B;_LL24B: _tmp27C=_tmp269.f1;_tmp27B=_tmp269.f2;_LL24C:;
# 1464
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp36C->topt)))
return({struct _tuple16 _tmp26A;_tmp26A.f1=_tmp27C;_tmp26A.f2=_tmp14C->unknown_all;_tmp26A;});{
struct _tuple15 _tmp26B=Cyc_CfFlowInfo_unname_rval(_tmp14C->r,_tmp27B);struct _tuple15 _tmp26C=_tmp26B;void*_tmp27A;_LL24E: _tmp27A=_tmp26C.f1;_LL24F:;{
void*_tmp26D=_tmp27A;int _tmp279;int _tmp278;struct _dyneither_ptr _tmp277;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp26D)->tag == 6){_LL251: _tmp279=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp26D)->f1).is_union;_tmp278=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp26D)->f1).fieldnum;_tmp277=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp26D)->f2;_LL252: {
# 1469
int _tmp26E=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp36C->topt),_tmp36B);
if(_tmp279  && _tmp278 != - 1){
if(_tmp278 != _tmp26E)
return({struct _tuple16 _tmp26F;_tmp26F.f1=_tmp27C;_tmp26F.f2=_tmp14C->zero;_tmp26F;});else{
# 1474
return({struct _tuple16 _tmp270;_tmp270.f1=_tmp27C;_tmp270.f2=_tmp14C->notzeroall;_tmp270;});}}else{
# 1476
return({struct _tuple16 _tmp271;_tmp271.f1=_tmp27C;_tmp271.f2=_tmp14C->unknown_all;_tmp271;});}}}else{_LL253: _LL254:
# 1478
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp272=_cycalloc(sizeof(*_tmp272));({struct Cyc_Core_Impossible_exn_struct _tmp787=({struct Cyc_Core_Impossible_exn_struct _tmp273;_tmp273.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp786=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp276;_tmp276.tag=0;({struct _dyneither_ptr _tmp784=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp276.f1=_tmp784;});({void*_tmp274[1]={& _tmp276};({struct _dyneither_ptr _tmp785=({const char*_tmp275="anal_Rexp: TagCheck_e: %s";_tag_dyneither(_tmp275,sizeof(char),26);});Cyc_aprintf(_tmp785,_tag_dyneither(_tmp274,sizeof(void*),1));});});});_tmp273.f1=_tmp786;});_tmp273;});_tmp272[0]=_tmp787;});_tmp272;}));}_LL250:;};};}case 22: _LL17E: _tmp36E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp36D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL17F: {
# 1483
struct _tuple16 _tmp27D=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp36E);struct _tuple16 _tmp27E=_tmp27D;union Cyc_CfFlowInfo_FlowInfo _tmp298;void*_tmp297;_LL256: _tmp298=_tmp27E.f1;_tmp297=_tmp27E.f2;_LL257:;{
# 1486
struct _tuple16 _tmp27F=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp298,_tmp36E,_tmp297);struct _tuple16 _tmp280=_tmp27F;union Cyc_CfFlowInfo_FlowInfo _tmp296;void*_tmp295;_LL259: _tmp296=_tmp280.f1;_tmp295=_tmp280.f2;_LL25A:;{
# 1489
void*_tmp281=Cyc_Tcutil_compress((void*)_check_null(_tmp36E->topt));void*_tmp282=_tmp281;void*_tmp294;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp282)->tag == 5){_LL25C: _tmp294=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp282)->f1).elt_typ;_LL25D:
# 1491
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp294))
# 1493
return({struct _tuple16 _tmp283;_tmp283.f1=_tmp296;({void*_tmp788=Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,(void*)_check_null(e->topt),0,_tmp14C->unknown_all);_tmp283.f2=_tmp788;});_tmp283;});
# 1495
if(Cyc_Absyn_is_require_union_type(_tmp294))
Cyc_NewControlFlow_check_union_requires(_tmp36E->loc,_tmp14C->r,_tmp294,_tmp36D,_tmp298);{
# 1498
struct _tuple15 _tmp284=Cyc_CfFlowInfo_unname_rval(_tmp14C->r,_tmp295);struct _tuple15 _tmp285=_tmp284;void*_tmp290;_LL261: _tmp290=_tmp285.f1;_LL262:;{
void*_tmp286=_tmp290;int _tmp28F;int _tmp28E;struct _dyneither_ptr _tmp28D;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp286)->tag == 6){_LL264: _tmp28F=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp286)->f1).is_union;_tmp28E=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp286)->f1).fieldnum;_tmp28D=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp286)->f2;_LL265: {
# 1501
int _tmp287=Cyc_CfFlowInfo_get_field_index(_tmp294,_tmp36D);
if((_tmp28F  && _tmp28E != - 1) && _tmp28E != _tmp287)
return({struct _tuple16 _tmp288;_tmp288.f1=_tmp296;({void*_tmp789=Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,(void*)_check_null(e->topt),1,_tmp14C->unknown_none);_tmp288.f2=_tmp789;});_tmp288;});
return({struct _tuple16 _tmp289;_tmp289.f1=_tmp296;_tmp289.f2=*((void**)_check_dyneither_subscript(_tmp28D,sizeof(void*),_tmp287));_tmp289;});}}else{_LL266: _LL267:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp28A=_cycalloc(sizeof(*_tmp28A));({struct Cyc_Core_Impossible_exn_struct _tmp78B=({struct Cyc_Core_Impossible_exn_struct _tmp28B;_tmp28B.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp78A=({const char*_tmp28C="anal_Rexp: AggrArrow";_tag_dyneither(_tmp28C,sizeof(char),21);});_tmp28B.f1=_tmp78A;});_tmp28B;});_tmp28A[0]=_tmp78B;});_tmp28A;}));}_LL263:;};};}else{_LL25E: _LL25F:
# 1507
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp291=_cycalloc(sizeof(*_tmp291));({struct Cyc_Core_Impossible_exn_struct _tmp78D=({struct Cyc_Core_Impossible_exn_struct _tmp292;_tmp292.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp78C=({const char*_tmp293="anal_Rexp: AggrArrow ptr";_tag_dyneither(_tmp293,sizeof(char),25);});_tmp292.f1=_tmp78C;});_tmp292;});_tmp291[0]=_tmp78D;});_tmp291;}));}_LL25B:;};};}case 6: _LL180: _tmp371=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp370=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_tmp36F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp164)->f3;_LL181: {
# 1511
struct _tuple19 _tmp299=Cyc_NewControlFlow_anal_test(env,inflow,_tmp371);struct _tuple19 _tmp29A=_tmp299;union Cyc_CfFlowInfo_FlowInfo _tmp29E;union Cyc_CfFlowInfo_FlowInfo _tmp29D;_LL269: _tmp29E=_tmp29A.f1;_tmp29D=_tmp29A.f2;_LL26A:;{
struct _tuple16 _tmp29B=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp29E,_tmp370);
struct _tuple16 _tmp29C=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp29D,_tmp36F);
# 1515
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp14C,_tmp29B,_tmp29C);};}case 7: _LL182: _tmp373=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp372=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL183: {
# 1518
struct _tuple19 _tmp29F=Cyc_NewControlFlow_anal_test(env,inflow,_tmp373);struct _tuple19 _tmp2A0=_tmp29F;union Cyc_CfFlowInfo_FlowInfo _tmp2AA;union Cyc_CfFlowInfo_FlowInfo _tmp2A9;_LL26C: _tmp2AA=_tmp2A0.f1;_tmp2A9=_tmp2A0.f2;_LL26D:;{
struct _tuple16 _tmp2A1=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2AA,_tmp372);struct _tuple16 _tmp2A2=_tmp2A1;union Cyc_CfFlowInfo_FlowInfo _tmp2A8;void*_tmp2A7;_LL26F: _tmp2A8=_tmp2A2.f1;_tmp2A7=_tmp2A2.f2;_LL270:;{
struct _tuple16 _tmp2A3=({struct _tuple16 _tmp2A6;_tmp2A6.f1=_tmp2A8;_tmp2A6.f2=_tmp2A7;_tmp2A6;});
struct _tuple16 _tmp2A4=({struct _tuple16 _tmp2A5;_tmp2A5.f1=_tmp2A9;_tmp2A5.f2=_tmp14C->zero;_tmp2A5;});
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp14C,_tmp2A3,_tmp2A4);};};}case 8: _LL184: _tmp375=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp374=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL185: {
# 1525
struct _tuple19 _tmp2AB=Cyc_NewControlFlow_anal_test(env,inflow,_tmp375);struct _tuple19 _tmp2AC=_tmp2AB;union Cyc_CfFlowInfo_FlowInfo _tmp2B6;union Cyc_CfFlowInfo_FlowInfo _tmp2B5;_LL272: _tmp2B6=_tmp2AC.f1;_tmp2B5=_tmp2AC.f2;_LL273:;{
struct _tuple16 _tmp2AD=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2B5,_tmp374);struct _tuple16 _tmp2AE=_tmp2AD;union Cyc_CfFlowInfo_FlowInfo _tmp2B4;void*_tmp2B3;_LL275: _tmp2B4=_tmp2AE.f1;_tmp2B3=_tmp2AE.f2;_LL276:;{
struct _tuple16 _tmp2AF=({struct _tuple16 _tmp2B2;_tmp2B2.f1=_tmp2B4;_tmp2B2.f2=_tmp2B3;_tmp2B2;});
struct _tuple16 _tmp2B0=({struct _tuple16 _tmp2B1;_tmp2B1.f1=_tmp2B6;_tmp2B1.f2=_tmp14C->notzeroall;_tmp2B1;});
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp14C,_tmp2AF,_tmp2B0);};};}case 23: _LL186: _tmp377=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp376=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL187: {
# 1532
struct _RegionHandle _tmp2B7=_new_region("temp");struct _RegionHandle*temp=& _tmp2B7;_push_region(temp);
{struct _tuple23 _tmp2B8=({struct _RegionHandle*_tmp791=temp;struct Cyc_NewControlFlow_AnalEnv*_tmp790=env;union Cyc_CfFlowInfo_FlowInfo _tmp78F=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp791,_tmp790,_tmp78F,({struct Cyc_Absyn_Exp*_tmp2DC[2];_tmp2DC[1]=_tmp376;_tmp2DC[0]=_tmp377;({struct _RegionHandle*_tmp78E=temp;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp78E,_tag_dyneither(_tmp2DC,sizeof(struct Cyc_Absyn_Exp*),2));});}),0,1);});struct _tuple23 _tmp2B9=_tmp2B8;union Cyc_CfFlowInfo_FlowInfo _tmp2DB;struct Cyc_List_List*_tmp2DA;_LL278: _tmp2DB=_tmp2B9.f1;_tmp2DA=_tmp2B9.f2;_LL279:;{
# 1538
union Cyc_CfFlowInfo_FlowInfo _tmp2BA=_tmp2DB;
{union Cyc_CfFlowInfo_FlowInfo _tmp2BB=_tmp2DB;struct Cyc_Dict_Dict _tmp2C0;struct Cyc_List_List*_tmp2BF;if((_tmp2BB.ReachableFL).tag == 2){_LL27B: _tmp2C0=((_tmp2BB.ReachableFL).val).f1;_tmp2BF=((_tmp2BB.ReachableFL).val).f2;_LL27C:
# 1543
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2C0,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp2DA))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp2BC=0;({unsigned int _tmp793=_tmp376->loc;struct _dyneither_ptr _tmp792=({const char*_tmp2BD="expression may not be initialized";_tag_dyneither(_tmp2BD,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp793,_tmp792,_tag_dyneither(_tmp2BC,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp2BE=Cyc_NewControlFlow_add_subscript_reln(_tmp14C->r,_tmp2BF,_tmp377,_tmp376);
if(_tmp2BF != _tmp2BE)
({union Cyc_CfFlowInfo_FlowInfo _tmp794=Cyc_CfFlowInfo_ReachableFL(_tmp2C0,_tmp2BE);_tmp2BA=_tmp794;});
goto _LL27A;};}else{_LL27D: _LL27E:
 goto _LL27A;}_LL27A:;}{
# 1551
void*_tmp2C1=Cyc_Tcutil_compress((void*)_check_null(_tmp377->topt));void*_tmp2C2=_tmp2C1;struct Cyc_List_List*_tmp2D9;switch(*((int*)_tmp2C2)){case 10: _LL280: _tmp2D9=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2C2)->f1;_LL281: {
# 1553
struct _tuple15 _tmp2C3=Cyc_CfFlowInfo_unname_rval(_tmp14C->r,(void*)((struct Cyc_List_List*)_check_null(_tmp2DA))->hd);struct _tuple15 _tmp2C4=_tmp2C3;void*_tmp2CC;_LL287: _tmp2CC=_tmp2C4.f1;_LL288:;{
void*_tmp2C5=_tmp2CC;struct _dyneither_ptr _tmp2CB;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2C5)->tag == 6){_LL28A: _tmp2CB=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2C5)->f2;_LL28B: {
# 1556
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp376)).f1;
struct _tuple16 _tmp2C7=({struct _tuple16 _tmp2C6;_tmp2C6.f1=_tmp2BA;_tmp2C6.f2=*((void**)_check_dyneither_subscript(_tmp2CB,sizeof(void*),(int)i));_tmp2C6;});_npop_handler(0);return _tmp2C7;}}else{_LL28C: _LL28D:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2C8=_cycalloc(sizeof(*_tmp2C8));({struct Cyc_Core_Impossible_exn_struct _tmp796=({struct Cyc_Core_Impossible_exn_struct _tmp2C9;_tmp2C9.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp795=({const char*_tmp2CA="anal_Rexp: Subscript";_tag_dyneither(_tmp2CA,sizeof(char),21);});_tmp2C9.f1=_tmp795;});_tmp2C9;});_tmp2C8[0]=_tmp796;});_tmp2C8;}));}_LL289:;};}case 5: _LL282: _LL283: {
# 1561
struct _tuple16 _tmp2CD=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2DB,_tmp377,(void*)((struct Cyc_List_List*)_check_null(_tmp2DA))->hd);struct _tuple16 _tmp2CE=_tmp2CD;union Cyc_CfFlowInfo_FlowInfo _tmp2D5;void*_tmp2D4;_LL28F: _tmp2D5=_tmp2CE.f1;_tmp2D4=_tmp2CE.f2;_LL290:;{
union Cyc_CfFlowInfo_FlowInfo _tmp2CF=_tmp2D5;if((_tmp2CF.BottomFL).tag == 1){_LL292: _LL293: {
struct _tuple16 _tmp2D1=({struct _tuple16 _tmp2D0;_tmp2D0.f1=_tmp2D5;_tmp2D0.f2=_tmp2D4;_tmp2D0;});_npop_handler(0);return _tmp2D1;}}else{_LL294: _LL295: {
struct _tuple16 _tmp2D3=({struct _tuple16 _tmp2D2;_tmp2D2.f1=_tmp2BA;_tmp2D2.f2=_tmp2D4;_tmp2D2;});_npop_handler(0);return _tmp2D3;}}_LL291:;};}default: _LL284: _LL285:
# 1566
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2D6=_cycalloc(sizeof(*_tmp2D6));({struct Cyc_Core_Impossible_exn_struct _tmp798=({struct Cyc_Core_Impossible_exn_struct _tmp2D7;_tmp2D7.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp797=({const char*_tmp2D8="anal_Rexp: Subscript -- bad type";_tag_dyneither(_tmp2D8,sizeof(char),33);});_tmp2D7.f1=_tmp797;});_tmp2D7;});_tmp2D6[0]=_tmp798;});_tmp2D6;}));}_LL27F:;};};}
# 1533
;_pop_region(temp);}case 24: _LL18A: _tmp378=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL18B: {
# 1573
struct _RegionHandle _tmp2DD=_new_region("temp");struct _RegionHandle*temp=& _tmp2DD;_push_region(temp);
{struct _tuple23 _tmp2DE=Cyc_NewControlFlow_anal_Rexps(temp,env,inflow,_tmp378,1,1);struct _tuple23 _tmp2DF=_tmp2DE;union Cyc_CfFlowInfo_FlowInfo _tmp2EB;struct Cyc_List_List*_tmp2EA;_LL297: _tmp2EB=_tmp2DF.f1;_tmp2EA=_tmp2DF.f2;_LL298:;{
struct _dyneither_ptr aggrdict=({unsigned int _tmp2E5=(unsigned int)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp378);void**_tmp2E6=(void**)({struct _RegionHandle*_tmp799=env->r;_region_malloc(_tmp799,_check_times(sizeof(void*),_tmp2E5));});struct _dyneither_ptr _tmp2E9=_tag_dyneither(_tmp2E6,sizeof(void*),_tmp2E5);{unsigned int _tmp2E7=_tmp2E5;unsigned int i;for(i=0;i < _tmp2E7;i ++){({typeof((void*)({
void*_tmp2E8=(void*)((struct Cyc_List_List*)_check_null(_tmp2EA))->hd;
_tmp2EA=_tmp2EA->tl;
_tmp2E8;}))_tmp79A=(void*)({
# 1577
void*_tmp2E8=(void*)((struct Cyc_List_List*)_check_null(_tmp2EA))->hd;
_tmp2EA=_tmp2EA->tl;
_tmp2E8;});_tmp2E6[i]=_tmp79A;});}}_tmp2E9;});
# 1581
struct _tuple16 _tmp2E4=({struct _tuple16 _tmp2E0;_tmp2E0.f1=_tmp2EB;({void*_tmp79D=(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2E1=_region_malloc(env->r,sizeof(*_tmp2E1));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp79C=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2E2;_tmp2E2.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp79B=({struct Cyc_CfFlowInfo_UnionRInfo _tmp2E3;_tmp2E3.is_union=0;_tmp2E3.fieldnum=- 1;_tmp2E3;});_tmp2E2.f1=_tmp79B;});_tmp2E2.f2=aggrdict;_tmp2E2;});_tmp2E1[0]=_tmp79C;});_tmp2E1;});_tmp2E0.f2=_tmp79D;});_tmp2E0;});_npop_handler(0);return _tmp2E4;};}
# 1574
;_pop_region(temp);}case 30: _LL18C: _tmp379=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL18D: {
# 1584
struct Cyc_List_List*fs;
{void*_tmp2EC=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2ED=_tmp2EC;struct Cyc_List_List*_tmp2F1;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2ED)->tag == 12){_LL29A: _tmp2F1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2ED)->f2;_LL29B:
 fs=_tmp2F1;goto _LL299;}else{_LL29C: _LL29D:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2EE=_cycalloc(sizeof(*_tmp2EE));({struct Cyc_Core_Impossible_exn_struct _tmp79F=({struct Cyc_Core_Impossible_exn_struct _tmp2EF;_tmp2EF.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp79E=({const char*_tmp2F0="anal_Rexp:anon struct has bad type";_tag_dyneither(_tmp2F0,sizeof(char),35);});_tmp2EF.f1=_tmp79E;});_tmp2EF;});_tmp2EE[0]=_tmp79F;});_tmp2EE;}));}_LL299:;}
# 1589
_tmp37C=_tmp379;_tmp37B=Cyc_Absyn_StructA;_tmp37A=fs;goto _LL18F;}case 29: if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp164)->f4 != 0){if(((struct Cyc_Absyn_Aggrdecl*)((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp164)->f4)->impl != 0){_LL18E: _tmp37C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp164)->f3;_tmp37B=(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp164)->f4)->kind;_tmp37A=((((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp164)->f4)->impl)->fields;_LL18F: {
# 1591
void*exp_type=(void*)_check_null(e->topt);
struct _RegionHandle _tmp2F2=_new_region("temp");struct _RegionHandle*temp=& _tmp2F2;_push_region(temp);
{struct _tuple23 _tmp2F3=({struct _RegionHandle*_tmp7A2=temp;struct Cyc_NewControlFlow_AnalEnv*_tmp7A1=env;union Cyc_CfFlowInfo_FlowInfo _tmp7A0=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp7A2,_tmp7A1,_tmp7A0,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp37C),1,1);});
# 1593
struct _tuple23 _tmp2F4=_tmp2F3;union Cyc_CfFlowInfo_FlowInfo _tmp303;struct Cyc_List_List*_tmp302;_LL29F: _tmp303=_tmp2F4.f1;_tmp302=_tmp2F4.f2;_LL2A0:;{
# 1595
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp14C,_tmp37A,_tmp37B == Cyc_Absyn_UnionA,_tmp14C->unknown_all);
# 1598
int field_no=-1;
{int i=0;for(0;_tmp302 != 0;(((_tmp302=_tmp302->tl,_tmp37C=_tmp37C->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp37C))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp2F5=(void*)ds->hd;void*_tmp2F6=_tmp2F5;struct _dyneither_ptr*_tmp2FB;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2F6)->tag == 0){_LL2A2: _LL2A3:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2F7=_cycalloc(sizeof(*_tmp2F7));({struct Cyc_Core_Impossible_exn_struct _tmp7A4=({struct Cyc_Core_Impossible_exn_struct _tmp2F8;_tmp2F8.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp7A3=({const char*_tmp2F9="anal_Rexp:Aggregate_e";_tag_dyneither(_tmp2F9,sizeof(char),22);});_tmp2F8.f1=_tmp7A3;});_tmp2F8;});_tmp2F7[0]=_tmp7A4;});_tmp2F7;}));}else{_LL2A4: _tmp2FB=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2F6)->f1;_LL2A5:
# 1605
({int _tmp7A5=Cyc_CfFlowInfo_get_field_index_fs(_tmp37A,_tmp2FB);field_no=_tmp7A5;});
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp302->hd;
# 1608
if(_tmp37B == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp2FA=(*((struct _tuple26*)_tmp37C->hd)).f2;
({union Cyc_CfFlowInfo_FlowInfo _tmp7A6=Cyc_NewControlFlow_use_Rval(env,_tmp2FA->loc,_tmp303,(void*)_tmp302->hd);_tmp303=_tmp7A6;});
# 1612
Cyc_NewControlFlow_check_union_requires(_tmp2FA->loc,_tmp14C->r,exp_type,_tmp2FB,_tmp303);}}_LL2A1:;}}}{
# 1615
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2FC=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2FF=_region_malloc(env->r,sizeof(*_tmp2FF));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp7A8=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp300;_tmp300.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp7A7=({struct Cyc_CfFlowInfo_UnionRInfo _tmp301;_tmp301.is_union=_tmp37B == Cyc_Absyn_UnionA;_tmp301.fieldnum=field_no;_tmp301;});_tmp300.f1=_tmp7A7;});_tmp300.f2=aggrdict;_tmp300;});_tmp2FF[0]=_tmp7A8;});_tmp2FF;});
struct _tuple16 _tmp2FE=({struct _tuple16 _tmp2FD;_tmp2FD.f1=_tmp303;_tmp2FD.f2=(void*)_tmp2FC;_tmp2FD;});_npop_handler(0);return _tmp2FE;};};}
# 1593
;_pop_region(temp);}}else{goto _LL190;}}else{_LL190: _LL191:
# 1619
({struct Cyc_String_pa_PrintArg_struct _tmp306;_tmp306.tag=0;({struct _dyneither_ptr _tmp7A9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp306.f1=_tmp7A9;});({void*_tmp304[1]={& _tmp306};({struct _dyneither_ptr _tmp7AA=({const char*_tmp305="anal_Rexp:missing aggrdeclimpl in %s";_tag_dyneither(_tmp305,sizeof(char),37);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7AA,_tag_dyneither(_tmp304,sizeof(void*),1));});});});}case 26: _LL192: _tmp37D=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL193: {
# 1621
struct _RegionHandle _tmp307=_new_region("temp");struct _RegionHandle*temp=& _tmp307;_push_region(temp);
{struct Cyc_List_List*_tmp308=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp37D);
struct _tuple23 _tmp309=Cyc_NewControlFlow_anal_Rexps(temp,env,inflow,_tmp308,1,1);struct _tuple23 _tmp30A=_tmp309;union Cyc_CfFlowInfo_FlowInfo _tmp30E;struct Cyc_List_List*_tmp30D;_LL2A7: _tmp30E=_tmp30A.f1;_tmp30D=_tmp30A.f2;_LL2A8:;
for(0;_tmp30D != 0;(_tmp30D=_tmp30D->tl,_tmp308=_tmp308->tl)){
({union Cyc_CfFlowInfo_FlowInfo _tmp7AB=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp308))->hd)->loc,_tmp30E,(void*)_tmp30D->hd);_tmp30E=_tmp7AB;});}{
struct _tuple16 _tmp30C=({struct _tuple16 _tmp30B;_tmp30B.f1=_tmp30E;({void*_tmp7AC=Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,(void*)_check_null(e->topt),0,_tmp14C->unknown_all);_tmp30B.f2=_tmp7AC;});_tmp30B;});_npop_handler(0);return _tmp30C;};}
# 1622
;_pop_region(temp);}case 27: _LL194: _tmp381=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp380=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_tmp37F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp164)->f3;_tmp37E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp164)->f4;_LL195: {
# 1630
struct _tuple16 _tmp30F=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp380);struct _tuple16 _tmp310=_tmp30F;union Cyc_CfFlowInfo_FlowInfo _tmp333;void*_tmp332;_LL2AA: _tmp333=_tmp310.f1;_tmp332=_tmp310.f2;_LL2AB:;{
union Cyc_CfFlowInfo_FlowInfo _tmp311=_tmp333;struct Cyc_Dict_Dict _tmp331;struct Cyc_List_List*_tmp330;if((_tmp311.BottomFL).tag == 1){_LL2AD: _LL2AE:
 return({struct _tuple16 _tmp312;_tmp312.f1=_tmp333;_tmp312.f2=_tmp14C->unknown_all;_tmp312;});}else{_LL2AF: _tmp331=((_tmp311.ReachableFL).val).f1;_tmp330=((_tmp311.ReachableFL).val).f2;_LL2B0:
# 1634
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp331,_tmp332)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp313=0;({unsigned int _tmp7AE=_tmp380->loc;struct _dyneither_ptr _tmp7AD=({const char*_tmp314="expression may not be initialized";_tag_dyneither(_tmp314,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp7AE,_tmp7AD,_tag_dyneither(_tmp313,sizeof(void*),0));});});{
# 1638
struct Cyc_List_List*new_relns=_tmp330;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(_tmp381);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp380,& n2))
({struct Cyc_List_List*_tmp7AF=Cyc_Relations_add_relation(env->r,n1,Cyc_Relations_Rlt,n2,_tmp330);new_relns=_tmp7AF;});
# 1644
if(_tmp330 != new_relns)
({union Cyc_CfFlowInfo_FlowInfo _tmp7B0=Cyc_CfFlowInfo_ReachableFL(_tmp331,new_relns);_tmp333=_tmp7B0;});{
# 1648
void*_tmp315=_tmp332;switch(*((int*)_tmp315)){case 0: _LL2B2: _LL2B3:
 return({struct _tuple16 _tmp316;_tmp316.f1=_tmp333;_tmp316.f2=_tmp14C->unknown_all;_tmp316;});case 1: _LL2B4: _LL2B5:
 goto _LL2B7;case 4: _LL2B6: _LL2B7: {
# 1652
struct Cyc_List_List _tmp317=({struct Cyc_List_List _tmp322;_tmp322.hd=_tmp381;_tmp322.tl=0;_tmp322;});
({union Cyc_CfFlowInfo_FlowInfo _tmp7B1=Cyc_NewControlFlow_add_vars(_tmp14C,_tmp333,& _tmp317,_tmp14C->unknown_all,e->loc,0);_tmp333=_tmp7B1;});{
# 1655
struct _tuple16 _tmp318=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp333,_tmp37F);struct _tuple16 _tmp319=_tmp318;union Cyc_CfFlowInfo_FlowInfo _tmp321;void*_tmp320;_LL2BB: _tmp321=_tmp319.f1;_tmp320=_tmp319.f2;_LL2BC:;
{union Cyc_CfFlowInfo_FlowInfo _tmp31A=_tmp321;struct Cyc_Dict_Dict _tmp31F;if((_tmp31A.BottomFL).tag == 1){_LL2BE: _LL2BF:
 return({struct _tuple16 _tmp31B;_tmp31B.f1=_tmp321;_tmp31B.f2=_tmp14C->unknown_all;_tmp31B;});}else{_LL2C0: _tmp31F=((_tmp31A.ReachableFL).val).f1;_LL2C1:
# 1659
 if(Cyc_CfFlowInfo_initlevel(_tmp14C,_tmp31F,_tmp320)!= Cyc_CfFlowInfo_AllIL){
({void*_tmp31C=0;({unsigned int _tmp7B3=_tmp380->loc;struct _dyneither_ptr _tmp7B2=({const char*_tmp31D="expression may not be initialized";_tag_dyneither(_tmp31D,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp7B3,_tmp7B2,_tag_dyneither(_tmp31C,sizeof(void*),0));});});
return({struct _tuple16 _tmp31E;({union Cyc_CfFlowInfo_FlowInfo _tmp7B4=Cyc_CfFlowInfo_BottomFL();_tmp31E.f1=_tmp7B4;});_tmp31E.f2=_tmp14C->unknown_all;_tmp31E;});}}_LL2BD:;}
# 1664
_tmp333=_tmp321;
goto _LL2B9;};}default: _LL2B8: _LL2B9:
# 1667
 while(1){
struct Cyc_List_List _tmp323=({struct Cyc_List_List _tmp32E;_tmp32E.hd=_tmp381;_tmp32E.tl=0;_tmp32E;});
({union Cyc_CfFlowInfo_FlowInfo _tmp7B5=Cyc_NewControlFlow_add_vars(_tmp14C,_tmp333,& _tmp323,_tmp14C->unknown_all,e->loc,0);_tmp333=_tmp7B5;});{
struct _tuple16 _tmp324=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp333,_tmp37F);struct _tuple16 _tmp325=_tmp324;union Cyc_CfFlowInfo_FlowInfo _tmp32D;void*_tmp32C;_LL2C3: _tmp32D=_tmp325.f1;_tmp32C=_tmp325.f2;_LL2C4:;
{union Cyc_CfFlowInfo_FlowInfo _tmp326=_tmp32D;struct Cyc_Dict_Dict _tmp32A;if((_tmp326.BottomFL).tag == 1){_LL2C6: _LL2C7:
 goto _LL2C5;}else{_LL2C8: _tmp32A=((_tmp326.ReachableFL).val).f1;_LL2C9:
# 1674
 if(Cyc_CfFlowInfo_initlevel(_tmp14C,_tmp32A,_tmp32C)!= Cyc_CfFlowInfo_AllIL){
({void*_tmp327=0;({unsigned int _tmp7B7=_tmp380->loc;struct _dyneither_ptr _tmp7B6=({const char*_tmp328="expression may not be initialized";_tag_dyneither(_tmp328,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp7B7,_tmp7B6,_tag_dyneither(_tmp327,sizeof(void*),0));});});
return({struct _tuple16 _tmp329;({union Cyc_CfFlowInfo_FlowInfo _tmp7B8=Cyc_CfFlowInfo_BottomFL();_tmp329.f1=_tmp7B8;});_tmp329.f2=_tmp14C->unknown_all;_tmp329;});}}_LL2C5:;}{
# 1679
union Cyc_CfFlowInfo_FlowInfo _tmp32B=Cyc_CfFlowInfo_join_flow(_tmp14C,_tmp333,_tmp32D);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp32B,_tmp333))
break;
_tmp333=_tmp32B;};};}
# 1684
return({struct _tuple16 _tmp32F;_tmp32F.f1=_tmp333;_tmp32F.f2=_tmp14C->unknown_all;_tmp32F;});}_LL2B1:;};};}_LL2AC:;};}case 28: _LL196: _tmp384=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp383=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_tmp382=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp164)->f3;_LL197: {
# 1690
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp33F=_region_malloc(_tmp14C->r,sizeof(*_tmp33F));({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp7B9=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp340;_tmp340.tag=1;_tmp340.f1=_tmp384;_tmp340.f2=(void*)_check_null(e->topt);_tmp340;});_tmp33F[0]=_tmp7B9;});_tmp33F;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp33E=_region_malloc(_tmp14C->r,sizeof(*_tmp33E));_tmp33E->root=root;_tmp33E->path=0;_tmp33E;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp33C=_region_malloc(_tmp14C->r,sizeof(*_tmp33C));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp7BA=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp33D;_tmp33D.tag=4;_tmp33D.f1=place;_tmp33D;});_tmp33C[0]=_tmp7BA;});_tmp33C;});
void*place_val;
# 1698
({void*_tmp7BB=Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,_tmp383,0,_tmp14C->unknown_none);place_val=_tmp7BB;});{
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple16 _tmp334=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp384);struct _tuple16 _tmp335=_tmp334;union Cyc_CfFlowInfo_FlowInfo _tmp33B;_LL2CB: _tmp33B=_tmp335.f1;_LL2CC:;
outflow=_tmp33B;{
union Cyc_CfFlowInfo_FlowInfo _tmp336=outflow;struct Cyc_Dict_Dict _tmp33A;struct Cyc_List_List*_tmp339;if((_tmp336.BottomFL).tag == 1){_LL2CE: _LL2CF:
 return({struct _tuple16 _tmp337;_tmp337.f1=outflow;_tmp337.f2=rval;_tmp337;});}else{_LL2D0: _tmp33A=((_tmp336.ReachableFL).val).f1;_tmp339=((_tmp336.ReachableFL).val).f2;_LL2D1:
# 1705
 return({struct _tuple16 _tmp338;({union Cyc_CfFlowInfo_FlowInfo _tmp7BD=({struct Cyc_Dict_Dict _tmp7BC=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp33A,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp7BC,_tmp339);});_tmp338.f1=_tmp7BD;});_tmp338.f2=rval;_tmp338;});}_LL2CD:;};};}case 37: _LL198: _tmp385=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL199: {
# 1709
struct _tuple17 _tmp341=({struct _tuple17 _tmp344;_tmp344.f1=(env->fenv)->unknown_all;_tmp344.f2=copy_ctxt;_tmp344;});
union Cyc_CfFlowInfo_FlowInfo _tmp342=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp385,& _tmp341);
return({struct _tuple16 _tmp343;_tmp343.f1=_tmp342;_tmp343.f2=_tmp341.f1;_tmp343;});}case 36: _LL19C: _LL19D:
# 1714
 goto _LL19F;case 25: _LL19E: _LL19F:
 goto _LL1A1;case 39: _LL1A0: _LL1A1:
 goto _LL1A3;default: _LL1A2: _LL1A3:
# 1718
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp345=_cycalloc(sizeof(*_tmp345));({struct Cyc_Core_Impossible_exn_struct _tmp7BF=({struct Cyc_Core_Impossible_exn_struct _tmp346;_tmp346.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp7BE=({const char*_tmp347="anal_Rexp, unexpected exp form";_tag_dyneither(_tmp347,sizeof(char),31);});_tmp346.f1=_tmp7BE;});_tmp346;});_tmp345[0]=_tmp7BF;});_tmp345;}));}_LL139:;};}
# 1730 "new_control_flow.cyc"
static struct _tuple18 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,int expanded_place_possibly_null,struct Cyc_List_List*path){
# 1740
struct Cyc_CfFlowInfo_FlowEnv*_tmp386=env->fenv;
void*_tmp387=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp388=_tmp387;union Cyc_Absyn_Constraint*_tmp3BA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp388)->tag == 5){_LL2D3: _tmp3BA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp388)->f1).ptr_atts).bounds;_LL2D4: {
# 1743
union Cyc_CfFlowInfo_FlowInfo _tmp389=f;struct Cyc_Dict_Dict _tmp3B6;struct Cyc_List_List*_tmp3B5;if((_tmp389.BottomFL).tag == 1){_LL2D8: _LL2D9:
 return({struct _tuple18 _tmp38A;_tmp38A.f1=f;({union Cyc_CfFlowInfo_AbsLVal _tmp7C0=Cyc_CfFlowInfo_UnknownL();_tmp38A.f2=_tmp7C0;});_tmp38A;});}else{_LL2DA: _tmp3B6=((_tmp389.ReachableFL).val).f1;_tmp3B5=((_tmp389.ReachableFL).val).f2;_LL2DB: {
# 1747
struct _tuple15 _tmp38B=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);struct _tuple15 _tmp38C=_tmp38B;void*_tmp3B4;struct Cyc_List_List*_tmp3B3;_LL2DD: _tmp3B4=_tmp38C.f1;_tmp3B3=_tmp38C.f2;_LL2DE:;
retry: {void*_tmp38D=_tmp3B4;void*_tmp3A9;enum Cyc_CfFlowInfo_InitLevel _tmp3A8;void*_tmp3A7;struct Cyc_List_List*_tmp3A6;void*_tmp3A5;switch(*((int*)_tmp38D)){case 8: _LL2E0: _LL2E1:
# 1750
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp38E=_cycalloc(sizeof(*_tmp38E));({struct Cyc_Core_Impossible_exn_struct _tmp7C2=({struct Cyc_Core_Impossible_exn_struct _tmp38F;_tmp38F.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp7C1=({const char*_tmp390="named location in anal_derefL";_tag_dyneither(_tmp390,sizeof(char),30);});_tmp38F.f1=_tmp7C1;});_tmp38F;});_tmp38E[0]=_tmp7C2;});_tmp38E;}));case 1: _LL2E2: _LL2E3:
# 1752
({void*_tmp7C5=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp391=_cycalloc(sizeof(*_tmp391));({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp7C4=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp392;_tmp392.tag=Cyc_CfFlowInfo_NotZero;({struct Cyc_List_List*_tmp7C3=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3B5);_tmp392.f1=_tmp7C3;});_tmp392;});_tmp391[0]=_tmp7C4;});_tmp391;});e->annot=_tmp7C5;});goto _LL2DF;case 5: _LL2E4: _tmp3A5=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp38D)->f1;_LL2E5:
# 1755
 if(expanded_place_possibly_null)
# 1758
({void*_tmp7C8=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp393=_cycalloc(sizeof(*_tmp393));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp7C7=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp394;_tmp394.tag=Cyc_CfFlowInfo_UnknownZ;({struct Cyc_List_List*_tmp7C6=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3B5);_tmp394.f1=_tmp7C6;});_tmp394;});_tmp393[0]=_tmp7C7;});_tmp393;});e->annot=_tmp7C8;});else{
# 1761
void*_tmp395=e->annot;void*_tmp396=_tmp395;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp396)->tag == Cyc_CfFlowInfo_UnknownZ){_LL2F1: _LL2F2:
# 1765
({void*_tmp7CB=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp397=_cycalloc(sizeof(*_tmp397));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp7CA=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp398;_tmp398.tag=Cyc_CfFlowInfo_UnknownZ;({struct Cyc_List_List*_tmp7C9=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3B5);_tmp398.f1=_tmp7C9;});_tmp398;});_tmp397[0]=_tmp7CA;});_tmp397;});e->annot=_tmp7CB;});
goto _LL2F0;}else{_LL2F3: _LL2F4:
# 1768
({void*_tmp7CE=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp399=_cycalloc(sizeof(*_tmp399));({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp7CD=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp39A;_tmp39A.tag=Cyc_CfFlowInfo_NotZero;({struct Cyc_List_List*_tmp7CC=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3B5);_tmp39A.f1=_tmp7CC;});_tmp39A;});_tmp399[0]=_tmp7CD;});_tmp399;});e->annot=_tmp7CE;});
goto _LL2F0;}_LL2F0:;}
# 1773
if(Cyc_Tcutil_is_bound_one(_tmp3BA))return({struct _tuple18 _tmp39B;_tmp39B.f1=f;({union Cyc_CfFlowInfo_AbsLVal _tmp7CF=Cyc_CfFlowInfo_UnknownL();_tmp39B.f2=_tmp7CF;});_tmp39B;});
goto _LL2DF;case 4: _LL2E6: _tmp3A7=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp38D)->f1)->root;_tmp3A6=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp38D)->f1)->path;_LL2E7:
# 1777
({void*_tmp7D2=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp39C=_cycalloc(sizeof(*_tmp39C));({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp7D1=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp39D;_tmp39D.tag=Cyc_CfFlowInfo_NotZero;({struct Cyc_List_List*_tmp7D0=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3B5);_tmp39D.f1=_tmp7D0;});_tmp39D;});_tmp39C[0]=_tmp7D1;});_tmp39C;});e->annot=_tmp7D2;});
if(Cyc_Tcutil_is_bound_one(_tmp3BA))
return({struct _tuple18 _tmp39E;_tmp39E.f1=f;({union Cyc_CfFlowInfo_AbsLVal _tmp7D4=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp39F=_region_malloc(_tmp386->r,sizeof(*_tmp39F));_tmp39F->root=_tmp3A7;({struct Cyc_List_List*_tmp7D3=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp386->r,_tmp3A6,path);_tmp39F->path=_tmp7D3;});_tmp39F;}));_tmp39E.f2=_tmp7D4;});_tmp39E;});
goto _LL2DF;case 0: _LL2E8: _LL2E9:
# 1782
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple18 _tmp3A0;({union Cyc_CfFlowInfo_FlowInfo _tmp7D5=Cyc_CfFlowInfo_BottomFL();_tmp3A0.f1=_tmp7D5;});({union Cyc_CfFlowInfo_AbsLVal _tmp7D6=Cyc_CfFlowInfo_UnknownL();_tmp3A0.f2=_tmp7D6;});_tmp3A0;});case 2: _LL2EA: _tmp3A8=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp38D)->f1;_LL2EB:
# 1787
 if(Cyc_Tcutil_is_bound_one(_tmp3BA))
({union Cyc_CfFlowInfo_FlowInfo _tmp7D7=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp3A8,_tmp3B3);f=_tmp7D7;});
({void*_tmp7DA=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp7D9=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp3A2;_tmp3A2.tag=Cyc_CfFlowInfo_UnknownZ;({struct Cyc_List_List*_tmp7D8=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3B5);_tmp3A2.f1=_tmp7D8;});_tmp3A2;});_tmp3A1[0]=_tmp7D9;});_tmp3A1;});e->annot=_tmp7DA;});
goto _LL2DF;case 7: _LL2EC: _tmp3A9=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp38D)->f3;_LL2ED:
# 1792
 if(passthrough_consumes){
_tmp3B4=_tmp3A9;goto retry;}
# 1795
goto _LL2EF;default: _LL2EE: _LL2EF:
# 1797
({void*_tmp7DD=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp7DC=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp3A4;_tmp3A4.tag=Cyc_CfFlowInfo_UnknownZ;({struct Cyc_List_List*_tmp7DB=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3B5);_tmp3A4.f1=_tmp7DB;});_tmp3A4;});_tmp3A3[0]=_tmp7DC;});_tmp3A3;});e->annot=_tmp7DD;});
goto _LL2DF;}_LL2DF:;}
# 1800
if(Cyc_CfFlowInfo_initlevel(_tmp386,_tmp3B6,_tmp3B4)== Cyc_CfFlowInfo_NoneIL){
struct _tuple15 _tmp3AA=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,_tmp3B4);struct _tuple15 _tmp3AB=_tmp3AA;void*_tmp3B1;_LL2F6: _tmp3B1=_tmp3AB.f1;_LL2F7:;{
void*_tmp3AC=_tmp3B1;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3AC)->tag == 7){_LL2F9: _LL2FA:
# 1804
({void*_tmp3AD=0;({unsigned int _tmp7DF=e->loc;struct _dyneither_ptr _tmp7DE=({const char*_tmp3AE="attempt to dereference an alias-free that has already been copied";_tag_dyneither(_tmp3AE,sizeof(char),66);});Cyc_CfFlowInfo_aerr(_tmp7DF,_tmp7DE,_tag_dyneither(_tmp3AD,sizeof(void*),0));});});
goto _LL2F8;}else{_LL2FB: _LL2FC:
# 1807
({void*_tmp3AF=0;({unsigned int _tmp7E1=e->loc;struct _dyneither_ptr _tmp7E0=({const char*_tmp3B0="dereference of possibly uninitialized pointer";_tag_dyneither(_tmp3B0,sizeof(char),46);});Cyc_CfFlowInfo_aerr(_tmp7E1,_tmp7E0,_tag_dyneither(_tmp3AF,sizeof(void*),0));});});
goto _LL2F8;}_LL2F8:;};}
# 1811
return({struct _tuple18 _tmp3B2;_tmp3B2.f1=f;({union Cyc_CfFlowInfo_AbsLVal _tmp7E2=Cyc_CfFlowInfo_UnknownL();_tmp3B2.f2=_tmp7E2;});_tmp3B2;});}}_LL2D7:;}}else{_LL2D5: _LL2D6:
# 1813
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));({struct Cyc_Core_Impossible_exn_struct _tmp7E4=({struct Cyc_Core_Impossible_exn_struct _tmp3B8;_tmp3B8.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp7E3=({const char*_tmp3B9="left deref of non-pointer-type";_tag_dyneither(_tmp3B9,sizeof(char),31);});_tmp3B8.f1=_tmp7E3;});_tmp3B8;});_tmp3B7[0]=_tmp7E4;});_tmp3B7;}));}_LL2D2:;}
# 1819
static struct Cyc_CfFlowInfo_Place*Cyc_NewControlFlow_make_expanded_place(struct _RegionHandle*r,struct Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*path){
# 1821
struct Cyc_CfFlowInfo_Place*_tmp3BB=p;void*_tmp3C1;struct Cyc_List_List*_tmp3C0;_LL2FE: _tmp3C1=_tmp3BB->root;_tmp3C0=_tmp3BB->path;_LL2FF:;
({struct Cyc_List_List*_tmp7E7=({struct Cyc_List_List*_tmp3BC=_region_malloc(r,sizeof(*_tmp3BC));({void*_tmp7E6=(void*)({struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*_tmp3BD=_region_malloc(r,sizeof(*_tmp3BD));({struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct _tmp7E5=({struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct _tmp3BE;_tmp3BE.tag=1;_tmp3BE;});_tmp3BD[0]=_tmp7E5;});_tmp3BD;});_tmp3BC->hd=_tmp7E6;});_tmp3BC->tl=path;_tmp3BC;});path=_tmp7E7;});
return({struct Cyc_CfFlowInfo_Place*_tmp3BF=_region_malloc(r,sizeof(*_tmp3BF));_tmp3BF->root=_tmp3C1;({struct Cyc_List_List*_tmp7E8=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp3C0,path);_tmp3BF->path=_tmp7E8;});_tmp3BF;});}
# 1830
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*path){
# 1833
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp3C2=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp3C3=inflow;struct Cyc_Dict_Dict _tmp3C6;struct Cyc_List_List*_tmp3C5;if((_tmp3C3.BottomFL).tag == 1){_LL301: _LL302:
 return({struct _tuple18 _tmp3C4;({union Cyc_CfFlowInfo_FlowInfo _tmp7E9=Cyc_CfFlowInfo_BottomFL();_tmp3C4.f1=_tmp7E9;});({union Cyc_CfFlowInfo_AbsLVal _tmp7EA=Cyc_CfFlowInfo_UnknownL();_tmp3C4.f2=_tmp7EA;});_tmp3C4;});}else{_LL303: _tmp3C6=((_tmp3C3.ReachableFL).val).f1;_tmp3C5=((_tmp3C3.ReachableFL).val).f2;_LL304:
# 1838
 d=_tmp3C6;}_LL300:;}{
# 1842
void*_tmp3C7=e->r;void*_tmp3C8=_tmp3C7;struct Cyc_Absyn_Exp*_tmp435;struct _dyneither_ptr*_tmp434;struct Cyc_Absyn_Exp*_tmp433;struct Cyc_Absyn_Exp*_tmp432;struct Cyc_Absyn_Exp*_tmp431;struct Cyc_Absyn_Exp*_tmp430;struct _dyneither_ptr*_tmp42F;struct Cyc_Absyn_Vardecl*_tmp42E;struct Cyc_Absyn_Vardecl*_tmp42D;struct Cyc_Absyn_Vardecl*_tmp42C;struct Cyc_Absyn_Vardecl*_tmp42B;struct Cyc_Absyn_Exp*_tmp42A;struct Cyc_Absyn_Exp*_tmp429;struct Cyc_Absyn_Exp*_tmp428;switch(*((int*)_tmp3C8)){case 14: _LL306: _tmp428=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3C8)->f2;_LL307:
 _tmp429=_tmp428;goto _LL309;case 12: _LL308: _tmp429=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp3C8)->f1;_LL309:
 _tmp42A=_tmp429;goto _LL30B;case 13: _LL30A: _tmp42A=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp3C8)->f1;_LL30B:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp42A,path);case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3C8)->f1)){case 3: _LL30C: _tmp42B=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3C8)->f1)->f1;_LL30D:
# 1847
 _tmp42C=_tmp42B;goto _LL30F;case 4: _LL30E: _tmp42C=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3C8)->f1)->f1;_LL30F:
 _tmp42D=_tmp42C;goto _LL311;case 5: _LL310: _tmp42D=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3C8)->f1)->f1;_LL311:
# 1850
 return({struct _tuple18 _tmp3C9;_tmp3C9.f1=inflow;({union Cyc_CfFlowInfo_AbsLVal _tmp7ED=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp3CA=_region_malloc(env->r,sizeof(*_tmp3CA));({void*_tmp7EC=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp3CB=_region_malloc(env->r,sizeof(*_tmp3CB));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp7EB=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp3CC;_tmp3CC.tag=0;_tmp3CC.f1=_tmp42D;_tmp3CC;});_tmp3CB[0]=_tmp7EB;});_tmp3CB;});_tmp3CA->root=_tmp7EC;});_tmp3CA->path=path;_tmp3CA;}));_tmp3C9.f2=_tmp7ED;});_tmp3C9;});case 1: _LL312: _tmp42E=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3C8)->f1)->f1;_LL313:
# 1852
 return({struct _tuple18 _tmp3CD;_tmp3CD.f1=inflow;({union Cyc_CfFlowInfo_AbsLVal _tmp7EE=Cyc_CfFlowInfo_UnknownL();_tmp3CD.f2=_tmp7EE;});_tmp3CD;});default: goto _LL31C;}case 22: _LL314: _tmp430=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3C8)->f1;_tmp42F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3C8)->f2;_LL315:
# 1855
{void*_tmp3CE=Cyc_Tcutil_compress((void*)_check_null(_tmp430->topt));void*_tmp3CF=_tmp3CE;void*_tmp3D6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3CF)->tag == 5){_LL31F: _tmp3D6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3CF)->f1).elt_typ;_LL320:
# 1857
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp3D6)){
Cyc_NewControlFlow_check_union_requires(_tmp430->loc,_tmp3C2->r,_tmp3D6,_tmp42F,inflow);
({struct Cyc_List_List*_tmp7F2=({struct Cyc_List_List*_tmp3D0=_region_malloc(env->r,sizeof(*_tmp3D0));({void*_tmp7F1=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp3D1=_region_malloc(_tmp3C2->r,sizeof(*_tmp3D1));({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp7F0=({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp3D2;_tmp3D2.tag=0;({int _tmp7EF=Cyc_CfFlowInfo_get_field_index(_tmp3D6,_tmp42F);_tmp3D2.f1=_tmp7EF;});_tmp3D2;});_tmp3D1[0]=_tmp7F0;});_tmp3D1;});_tmp3D0->hd=_tmp7F1;});_tmp3D0->tl=path;_tmp3D0;});path=_tmp7F2;});}
# 1861
goto _LL31E;}else{_LL321: _LL322:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));({struct Cyc_Core_Impossible_exn_struct _tmp7F4=({struct Cyc_Core_Impossible_exn_struct _tmp3D4;_tmp3D4.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp7F3=({const char*_tmp3D5="anal_Lexp: AggrArrow ptr";_tag_dyneither(_tmp3D5,sizeof(char),25);});_tmp3D4.f1=_tmp7F3;});_tmp3D4;});_tmp3D3[0]=_tmp7F4;});_tmp3D3;}));}_LL31E:;}
# 1864
_tmp431=_tmp430;goto _LL317;case 20: _LL316: _tmp431=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3C8)->f1;_LL317:
# 1868
 if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp431->topt),1)){
# 1870
struct _tuple18 _tmp3D7=Cyc_NewControlFlow_anal_Lexp(env,inflow,expand_unique,passthrough_consumes,_tmp431);struct _tuple18 _tmp3D8=_tmp3D7;union Cyc_CfFlowInfo_FlowInfo _tmp3FE;union Cyc_CfFlowInfo_AbsLVal _tmp3FD;_LL324: _tmp3FE=_tmp3D8.f1;_tmp3FD=_tmp3D8.f2;_LL325:;{
struct _tuple18 _tmp3D9=({struct _tuple18 _tmp3FC;_tmp3FC.f1=_tmp3FE;_tmp3FC.f2=_tmp3FD;_tmp3FC;});struct _tuple18 _tmp3DA=_tmp3D9;struct Cyc_Dict_Dict _tmp3FB;struct Cyc_List_List*_tmp3FA;struct Cyc_CfFlowInfo_Place*_tmp3F9;if(((_tmp3DA.f1).ReachableFL).tag == 2){if(((_tmp3DA.f2).PlaceL).tag == 1){_LL327: _tmp3FB=(((_tmp3DA.f1).ReachableFL).val).f1;_tmp3FA=(((_tmp3DA.f1).ReachableFL).val).f2;_tmp3F9=((_tmp3DA.f2).PlaceL).val;_LL328: {
# 1873
void*_tmp3DB=Cyc_CfFlowInfo_lookup_place(_tmp3FB,_tmp3F9);
struct _tuple15 _tmp3DC=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,_tmp3DB);struct _tuple15 _tmp3DD=_tmp3DC;void*_tmp3F8;struct Cyc_List_List*_tmp3F7;_LL32C: _tmp3F8=_tmp3DD.f1;_tmp3F7=_tmp3DD.f2;_LL32D:;
if(expand_unique){
int _tmp3DE=1;
void*_tmp3DF=_tmp3F8;struct Cyc_Absyn_Vardecl*_tmp3F1;void*_tmp3F0;switch(*((int*)_tmp3DF)){case 8: _LL32F: _tmp3F1=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3DF)->f1;_tmp3F0=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3DF)->f2;_LL330:
# 1879
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp3E0=_cycalloc(sizeof(*_tmp3E0));({struct Cyc_Core_Impossible_exn_struct _tmp7F6=({struct Cyc_Core_Impossible_exn_struct _tmp3E1;_tmp3E1.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp7F5=({const char*_tmp3E2="bad named location in anal_Lexp:deref";_tag_dyneither(_tmp3E2,sizeof(char),38);});_tmp3E1.f1=_tmp7F5;});_tmp3E1;});_tmp3E0[0]=_tmp7F6;});_tmp3E0;}));case 7: switch(*((int*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3DF)->f3)){case 4: _LL331: _LL332:
 goto _LL334;case 5: _LL335: _LL336:
# 1883
 goto _LL338;default: goto _LL33B;}case 4: _LL333: _LL334:
# 1882
 return Cyc_NewControlFlow_anal_derefL(env,_tmp3FE,_tmp431,_tmp3FE,_tmp3DB,passthrough_consumes,0,path);case 5: _LL337: _LL338: {
# 1887
struct _tuple18 _tmp3E3=Cyc_NewControlFlow_anal_derefL(env,_tmp3FE,_tmp431,_tmp3FE,_tmp3DB,passthrough_consumes,0,path);struct _tuple18 _tmp3E4=_tmp3E3;union Cyc_CfFlowInfo_FlowInfo _tmp3E6;_LL33E: _tmp3E6=_tmp3E4.f1;_LL33F:;
return({struct _tuple18 _tmp3E5;_tmp3E5.f1=_tmp3E6;({union Cyc_CfFlowInfo_AbsLVal _tmp7F7=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp3C2->r,_tmp3F9,path));_tmp3E5.f2=_tmp7F7;});_tmp3E5;});}case 1: _LL339: _LL33A:
# 1890
 _tmp3DE=0;goto _LL33C;default: _LL33B: _LL33C: {
# 1897
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp3C2,_tmp3FB,_tmp3F8);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp3C2->unknown_all: _tmp3C2->unknown_none;
void*res=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp3EE=_region_malloc(_tmp3C2->r,sizeof(*_tmp3EE));({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp7FB=({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp3EF;_tmp3EF.tag=5;({void*_tmp7FA=({struct Cyc_CfFlowInfo_FlowEnv*_tmp7F9=_tmp3C2;void*_tmp7F8=Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp431->topt));Cyc_CfFlowInfo_typ_to_absrval(_tmp7F9,_tmp7F8,0,leaf);});_tmp3EF.f1=_tmp7FA;});_tmp3EF;});_tmp3EE[0]=_tmp7FB;});_tmp3EE;});
for(0;_tmp3F7 != 0;_tmp3F7=_tmp3F7->tl){
({void*_tmp7FD=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3E7=_region_malloc(_tmp3C2->r,sizeof(*_tmp3E7));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp7FC=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp3E8;_tmp3E8.tag=8;_tmp3E8.f1=(struct Cyc_Absyn_Vardecl*)_tmp3F7->hd;_tmp3E8.f2=res;_tmp3E8;});_tmp3E7[0]=_tmp7FC;});_tmp3E7;});res=_tmp7FD;});}
({struct Cyc_Dict_Dict _tmp7FE=Cyc_CfFlowInfo_assign_place(_tmp3C2,e->loc,_tmp3FB,_tmp3F9,res);_tmp3FB=_tmp7FE;});{
union Cyc_CfFlowInfo_FlowInfo _tmp3E9=Cyc_CfFlowInfo_ReachableFL(_tmp3FB,_tmp3FA);
# 1910
struct _tuple18 _tmp3EA=Cyc_NewControlFlow_anal_derefL(env,_tmp3E9,_tmp431,_tmp3E9,res,passthrough_consumes,_tmp3DE,path);struct _tuple18 _tmp3EB=_tmp3EA;union Cyc_CfFlowInfo_FlowInfo _tmp3ED;_LL341: _tmp3ED=_tmp3EB.f1;_LL342:;
# 1912
return({struct _tuple18 _tmp3EC;_tmp3EC.f1=_tmp3ED;({union Cyc_CfFlowInfo_AbsLVal _tmp7FF=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp3C2->r,_tmp3F9,path));_tmp3EC.f2=_tmp7FF;});_tmp3EC;});};}}_LL32E:;}else{
# 1916
void*_tmp3F2=_tmp3F8;switch(*((int*)_tmp3F2)){case 7: if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3F2)->f3)->tag == 5){_LL344: _LL345:
 goto _LL347;}else{goto _LL348;}case 5: _LL346: _LL347: {
# 1921
struct _tuple18 _tmp3F3=Cyc_NewControlFlow_anal_derefL(env,_tmp3FE,_tmp431,_tmp3FE,_tmp3DB,passthrough_consumes,0,path);struct _tuple18 _tmp3F4=_tmp3F3;union Cyc_CfFlowInfo_FlowInfo _tmp3F6;_LL34B: _tmp3F6=_tmp3F4.f1;_LL34C:;
return({struct _tuple18 _tmp3F5;_tmp3F5.f1=_tmp3F6;({union Cyc_CfFlowInfo_AbsLVal _tmp800=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp3C2->r,_tmp3F9,path));_tmp3F5.f2=_tmp800;});_tmp3F5;});}default: _LL348: _LL349:
# 1925
 goto _LL343;}_LL343:;}
# 1928
goto _LL326;}}else{goto _LL329;}}else{_LL329: _LL32A:
 goto _LL326;}_LL326:;};}{
# 1933
struct _tuple16 _tmp3FF=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp431);struct _tuple16 _tmp400=_tmp3FF;union Cyc_CfFlowInfo_FlowInfo _tmp402;void*_tmp401;_LL34E: _tmp402=_tmp400.f1;_tmp401=_tmp400.f2;_LL34F:;
# 1935
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp431,_tmp402,_tmp401,passthrough_consumes,0,path);};case 23: _LL318: _tmp433=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3C8)->f1;_tmp432=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3C8)->f2;_LL319: {
# 1938
void*_tmp403=Cyc_Tcutil_compress((void*)_check_null(_tmp433->topt));void*_tmp404=_tmp403;switch(*((int*)_tmp404)){case 10: _LL351: _LL352: {
# 1940
unsigned int _tmp405=(Cyc_Evexp_eval_const_uint_exp(_tmp432)).f1;
return({struct Cyc_NewControlFlow_AnalEnv*_tmp807=env;union Cyc_CfFlowInfo_FlowInfo _tmp806=inflow;int _tmp805=expand_unique;int _tmp804=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp803=_tmp433;Cyc_NewControlFlow_anal_Lexp_rec(_tmp807,_tmp806,_tmp805,_tmp804,_tmp803,({struct Cyc_List_List*_tmp406=_region_malloc(env->r,sizeof(*_tmp406));({void*_tmp802=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp407=_region_malloc(env->r,sizeof(*_tmp407));({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp801=({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp408;_tmp408.tag=0;_tmp408.f1=(int)_tmp405;_tmp408;});_tmp407[0]=_tmp801;});_tmp407;});_tmp406->hd=_tmp802;});_tmp406->tl=path;_tmp406;}));});}case 5: _LL353: _LL354: {
# 1943
struct _RegionHandle _tmp409=_new_region("temp");struct _RegionHandle*temp=& _tmp409;_push_region(temp);
{struct _tuple23 _tmp40A=({struct _RegionHandle*_tmp80B=temp;struct Cyc_NewControlFlow_AnalEnv*_tmp80A=env;union Cyc_CfFlowInfo_FlowInfo _tmp809=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp80B,_tmp80A,_tmp809,({struct Cyc_Absyn_Exp*_tmp41E[2];_tmp41E[1]=_tmp432;_tmp41E[0]=_tmp433;({struct _RegionHandle*_tmp808=temp;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp808,_tag_dyneither(_tmp41E,sizeof(struct Cyc_Absyn_Exp*),2));});}),0,1);});struct _tuple23 _tmp40B=_tmp40A;union Cyc_CfFlowInfo_FlowInfo _tmp41D;struct Cyc_List_List*_tmp41C;_LL358: _tmp41D=_tmp40B.f1;_tmp41C=_tmp40B.f2;_LL359:;{
# 1946
union Cyc_CfFlowInfo_FlowInfo _tmp40C=_tmp41D;
{union Cyc_CfFlowInfo_FlowInfo _tmp40D=_tmp41D;struct Cyc_Dict_Dict _tmp412;struct Cyc_List_List*_tmp411;if((_tmp40D.ReachableFL).tag == 2){_LL35B: _tmp412=((_tmp40D.ReachableFL).val).f1;_tmp411=((_tmp40D.ReachableFL).val).f2;_LL35C:
# 1949
 if(Cyc_CfFlowInfo_initlevel(_tmp3C2,_tmp412,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp41C))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp40E=0;({unsigned int _tmp80D=_tmp432->loc;struct _dyneither_ptr _tmp80C=({const char*_tmp40F="expression may not be initialized";_tag_dyneither(_tmp40F,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp80D,_tmp80C,_tag_dyneither(_tmp40E,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp410=Cyc_NewControlFlow_add_subscript_reln(_tmp3C2->r,_tmp411,_tmp433,_tmp432);
if(_tmp411 != _tmp410)
({union Cyc_CfFlowInfo_FlowInfo _tmp80E=Cyc_CfFlowInfo_ReachableFL(_tmp412,_tmp410);_tmp40C=_tmp80E;});
goto _LL35A;};}else{_LL35D: _LL35E:
 goto _LL35A;}_LL35A:;}{
# 1958
struct _tuple18 _tmp413=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp433,_tmp41D,(void*)((struct Cyc_List_List*)_check_null(_tmp41C))->hd,passthrough_consumes,0,path);struct _tuple18 _tmp414=_tmp413;union Cyc_CfFlowInfo_FlowInfo _tmp41B;union Cyc_CfFlowInfo_AbsLVal _tmp41A;_LL360: _tmp41B=_tmp414.f1;_tmp41A=_tmp414.f2;_LL361:;{
union Cyc_CfFlowInfo_FlowInfo _tmp415=_tmp41B;if((_tmp415.BottomFL).tag == 1){_LL363: _LL364: {
struct _tuple18 _tmp417=({struct _tuple18 _tmp416;_tmp416.f1=_tmp41B;_tmp416.f2=_tmp41A;_tmp416;});_npop_handler(0);return _tmp417;}}else{_LL365: _LL366: {
struct _tuple18 _tmp419=({struct _tuple18 _tmp418;_tmp418.f1=_tmp40C;_tmp418.f2=_tmp41A;_tmp418;});_npop_handler(0);return _tmp419;}}_LL362:;};};};}
# 1944
;_pop_region(temp);}default: _LL355: _LL356:
# 1964
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp41F=_cycalloc(sizeof(*_tmp41F));({struct Cyc_Core_Impossible_exn_struct _tmp810=({struct Cyc_Core_Impossible_exn_struct _tmp420;_tmp420.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp80F=({const char*_tmp421="anal_Lexp: Subscript -- bad type";_tag_dyneither(_tmp421,sizeof(char),33);});_tmp420.f1=_tmp80F;});_tmp420;});_tmp41F[0]=_tmp810;});_tmp41F;}));}_LL350:;}case 21: _LL31A: _tmp435=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C8)->f1;_tmp434=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3C8)->f2;_LL31B: {
# 1968
void*_tmp422=(void*)_check_null(_tmp435->topt);
if(Cyc_Absyn_is_require_union_type(_tmp422))
Cyc_NewControlFlow_check_union_requires(_tmp435->loc,_tmp3C2->r,_tmp422,_tmp434,inflow);
# 1972
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp422))
return({struct _tuple18 _tmp423;_tmp423.f1=inflow;({union Cyc_CfFlowInfo_AbsLVal _tmp811=Cyc_CfFlowInfo_UnknownL();_tmp423.f2=_tmp811;});_tmp423;});
# 1975
return({struct Cyc_NewControlFlow_AnalEnv*_tmp819=env;union Cyc_CfFlowInfo_FlowInfo _tmp818=inflow;int _tmp817=expand_unique;int _tmp816=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp815=_tmp435;Cyc_NewControlFlow_anal_Lexp_rec(_tmp819,_tmp818,_tmp817,_tmp816,_tmp815,({struct Cyc_List_List*_tmp424=_region_malloc(env->r,sizeof(*_tmp424));({void*_tmp814=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp425=_region_malloc(env->r,sizeof(*_tmp425));({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp813=({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp426;_tmp426.tag=0;({int _tmp812=
Cyc_CfFlowInfo_get_field_index(_tmp422,_tmp434);_tmp426.f1=_tmp812;});_tmp426;});_tmp425[0]=_tmp813;});_tmp425;});_tmp424->hd=_tmp814;});_tmp424->tl=path;_tmp424;}));});}default: _LL31C: _LL31D:
# 1979
 return({struct _tuple18 _tmp427;({union Cyc_CfFlowInfo_FlowInfo _tmp81A=Cyc_CfFlowInfo_BottomFL();_tmp427.f1=_tmp81A;});({union Cyc_CfFlowInfo_AbsLVal _tmp81B=Cyc_CfFlowInfo_UnknownL();_tmp427.f2=_tmp81B;});_tmp427;});}_LL305:;};}
# 1983
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1986
struct _tuple18 _tmp436=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);struct _tuple18 _tmp437=_tmp436;union Cyc_CfFlowInfo_FlowInfo _tmp43A;union Cyc_CfFlowInfo_AbsLVal _tmp439;_LL368: _tmp43A=_tmp437.f1;_tmp439=_tmp437.f2;_LL369:;
return({struct _tuple18 _tmp438;_tmp438.f1=_tmp43A;_tmp438.f2=_tmp439;_tmp438;});}
# 1993
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1996
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd,1) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
struct _tuple18 _tmp43B=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple18 _tmp43C=_tmp43B;union Cyc_CfFlowInfo_FlowInfo _tmp43D;_LL36B: _tmp43D=_tmp43C.f1;_LL36C:;
inflow=_tmp43D;}}}
# 2004
return inflow;}
# 2010
static struct _tuple19 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 2013
struct Cyc_CfFlowInfo_FlowEnv*_tmp43E=env->fenv;
# 2016
void*r1;void*r2;
union Cyc_CfFlowInfo_FlowInfo f;
struct _RegionHandle _tmp43F=_new_region("temp");struct _RegionHandle*temp=& _tmp43F;_push_region(temp);{
struct _tuple23 _tmp440=Cyc_NewControlFlow_anal_Rexps(temp,env,inflow,es,0,0);struct _tuple23 _tmp441=_tmp440;union Cyc_CfFlowInfo_FlowInfo _tmp443;struct Cyc_List_List*_tmp442;_LL36E: _tmp443=_tmp441.f1;_tmp442=_tmp441.f2;_LL36F:;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp442))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp442->tl))->hd;
f=_tmp443;}{
# 2026
union Cyc_CfFlowInfo_FlowInfo _tmp444=f;struct Cyc_Dict_Dict _tmp48E;struct Cyc_List_List*_tmp48D;if((_tmp444.BottomFL).tag == 1){_LL371: _LL372: {
struct _tuple19 _tmp446=({struct _tuple19 _tmp445;_tmp445.f1=f;_tmp445.f2=f;_tmp445;});_npop_handler(0);return _tmp446;}}else{_LL373: _tmp48E=((_tmp444.ReachableFL).val).f1;_tmp48D=((_tmp444.ReachableFL).val).f2;_LL374: {
# 2029
struct Cyc_Absyn_Exp*_tmp447=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*_tmp448=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp48E,r1)== Cyc_CfFlowInfo_NoneIL  && !Cyc_CfFlowInfo_is_init_pointer(r1))
({void*_tmp449=0;({unsigned int _tmp81D=((struct Cyc_Absyn_Exp*)es->hd)->loc;struct _dyneither_ptr _tmp81C=({const char*_tmp44A="expression may not be initialized";_tag_dyneither(_tmp44A,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp81D,_tmp81C,_tag_dyneither(_tmp449,sizeof(void*),0));});});
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp48E,r2)== Cyc_CfFlowInfo_NoneIL  && !Cyc_CfFlowInfo_is_init_pointer(r1))
({void*_tmp44B=0;({unsigned int _tmp81F=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc;struct _dyneither_ptr _tmp81E=({const char*_tmp44C="expression may not be initialized";_tag_dyneither(_tmp44C,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp81F,_tmp81E,_tag_dyneither(_tmp44B,sizeof(void*),0));});});{
# 2036
union Cyc_CfFlowInfo_FlowInfo _tmp44D=f;
union Cyc_CfFlowInfo_FlowInfo _tmp44E=f;
# 2041
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct _tuple15 _tmp44F=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r1);struct _tuple15 _tmp450=_tmp44F;void*_tmp46B;struct Cyc_List_List*_tmp46A;_LL376: _tmp46B=_tmp450.f1;_tmp46A=_tmp450.f2;_LL377:;{
struct _tuple15 _tmp451=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r2);struct _tuple15 _tmp452=_tmp451;void*_tmp469;struct Cyc_List_List*_tmp468;_LL379: _tmp469=_tmp452.f1;_tmp468=_tmp452.f2;_LL37A:;{
struct _tuple0 _tmp453=({struct _tuple0 _tmp467;_tmp467.f1=_tmp46B;_tmp467.f2=_tmp469;_tmp467;});struct _tuple0 _tmp454=_tmp453;enum Cyc_CfFlowInfo_InitLevel _tmp466;enum Cyc_CfFlowInfo_InitLevel _tmp465;switch(*((int*)_tmp454.f1)){case 2: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp454.f2)->tag == 0){_LL37C: _tmp465=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp454.f1)->f1;_LL37D: {
# 2046
struct _tuple19 _tmp455=Cyc_NewControlFlow_splitzero(env,f,f,_tmp447,_tmp465,_tmp46A);struct _tuple19 _tmp456=_tmp455;union Cyc_CfFlowInfo_FlowInfo _tmp45C;union Cyc_CfFlowInfo_FlowInfo _tmp45B;_LL38D: _tmp45C=_tmp456.f1;_tmp45B=_tmp456.f2;_LL38E:;
{enum Cyc_Absyn_Primop _tmp457=p;switch(_tmp457){case Cyc_Absyn_Eq: _LL390: _LL391:
 _tmp44D=_tmp45B;_tmp44E=_tmp45C;goto _LL38F;case Cyc_Absyn_Neq: _LL392: _LL393:
 _tmp44D=_tmp45C;_tmp44E=_tmp45B;goto _LL38F;default: _LL394: _LL395:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp458=_cycalloc(sizeof(*_tmp458));({struct Cyc_Core_Impossible_exn_struct _tmp821=({struct Cyc_Core_Impossible_exn_struct _tmp459;_tmp459.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp820=({const char*_tmp45A="anal_test, zero-split";_tag_dyneither(_tmp45A,sizeof(char),22);});_tmp459.f1=_tmp820;});_tmp459;});_tmp458[0]=_tmp821;});_tmp458;}));}_LL38F:;}
# 2052
goto _LL37B;}}else{goto _LL38A;}case 0: switch(*((int*)_tmp454.f2)){case 2: _LL37E: _tmp466=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp454.f2)->f1;_LL37F: {
# 2054
struct _tuple19 _tmp45D=Cyc_NewControlFlow_splitzero(env,f,f,_tmp448,_tmp466,_tmp468);struct _tuple19 _tmp45E=_tmp45D;union Cyc_CfFlowInfo_FlowInfo _tmp464;union Cyc_CfFlowInfo_FlowInfo _tmp463;_LL397: _tmp464=_tmp45E.f1;_tmp463=_tmp45E.f2;_LL398:;
{enum Cyc_Absyn_Primop _tmp45F=p;switch(_tmp45F){case Cyc_Absyn_Eq: _LL39A: _LL39B:
 _tmp44D=_tmp463;_tmp44E=_tmp464;goto _LL399;case Cyc_Absyn_Neq: _LL39C: _LL39D:
 _tmp44D=_tmp464;_tmp44E=_tmp463;goto _LL399;default: _LL39E: _LL39F:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp460=_cycalloc(sizeof(*_tmp460));({struct Cyc_Core_Impossible_exn_struct _tmp823=({struct Cyc_Core_Impossible_exn_struct _tmp461;_tmp461.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp822=({const char*_tmp462="anal_test, zero-split";_tag_dyneither(_tmp462,sizeof(char),22);});_tmp461.f1=_tmp822;});_tmp461;});_tmp460[0]=_tmp823;});_tmp460;}));}_LL399:;}
# 2060
goto _LL37B;}case 0: _LL380: _LL381:
# 2062
 if(p == Cyc_Absyn_Eq)({union Cyc_CfFlowInfo_FlowInfo _tmp824=Cyc_CfFlowInfo_BottomFL();_tmp44E=_tmp824;});else{
({union Cyc_CfFlowInfo_FlowInfo _tmp825=Cyc_CfFlowInfo_BottomFL();_tmp44D=_tmp825;});}
goto _LL37B;case 1: _LL382: _LL383:
 goto _LL385;case 4: _LL384: _LL385:
 goto _LL387;default: goto _LL38A;}case 1: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp454.f2)->tag == 0){_LL386: _LL387:
 goto _LL389;}else{goto _LL38A;}case 4: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp454.f2)->tag == 0){_LL388: _LL389:
# 2069
 if(p == Cyc_Absyn_Neq)({union Cyc_CfFlowInfo_FlowInfo _tmp826=Cyc_CfFlowInfo_BottomFL();_tmp44E=_tmp826;});else{
({union Cyc_CfFlowInfo_FlowInfo _tmp827=Cyc_CfFlowInfo_BottomFL();_tmp44D=_tmp827;});}
goto _LL37B;}else{goto _LL38A;}default: _LL38A: _LL38B:
 goto _LL37B;}_LL37B:;};};}
# 2080
{struct _tuple0 _tmp46C=({struct _tuple0 _tmp470;({void*_tmp828=Cyc_Tcutil_compress((void*)_check_null(_tmp447->topt));_tmp470.f1=_tmp828;});({void*_tmp829=Cyc_Tcutil_compress((void*)_check_null(_tmp448->topt));_tmp470.f2=_tmp829;});_tmp470;});struct _tuple0 _tmp46D=_tmp46C;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46D.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46D.f1)->f1 == Cyc_Absyn_Unsigned){_LL3A1: _LL3A2:
 goto _LL3A4;}else{switch(*((int*)_tmp46D.f2)){case 6: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46D.f2)->f1 == Cyc_Absyn_Unsigned)goto _LL3A3;else{goto _LL3A9;}case 19: goto _LL3A7;default: goto _LL3A9;}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46D.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46D.f2)->f1 == Cyc_Absyn_Unsigned){_LL3A3: _LL3A4:
 goto _LL3A6;}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp46D.f1)->tag == 19)goto _LL3A5;else{goto _LL3A9;}}}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp46D.f1)->tag == 19){_LL3A5: _LL3A6:
 goto _LL3A8;}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp46D.f2)->tag == 19){_LL3A7: _LL3A8:
 goto _LL3A0;}else{_LL3A9: _LL3AA: {
struct _tuple19 _tmp46F=({struct _tuple19 _tmp46E;_tmp46E.f1=_tmp44D;_tmp46E.f2=_tmp44E;_tmp46E;});_npop_handler(0);return _tmp46F;}}}}}_LL3A0:;}
# 2089
{enum Cyc_Absyn_Primop _tmp471=p;switch(_tmp471){case Cyc_Absyn_Eq: _LL3AC: _LL3AD:
 goto _LL3AF;case Cyc_Absyn_Neq: _LL3AE: _LL3AF: goto _LL3B1;case Cyc_Absyn_Gt: _LL3B0: _LL3B1: goto _LL3B3;case Cyc_Absyn_Gte: _LL3B2: _LL3B3: goto _LL3B5;case Cyc_Absyn_Lt: _LL3B4: _LL3B5: goto _LL3B7;case Cyc_Absyn_Lte: _LL3B6: _LL3B7: goto _LL3AB;default: _LL3B8: _LL3B9: {
struct _tuple19 _tmp473=({struct _tuple19 _tmp472;_tmp472.f1=_tmp44D;_tmp472.f2=_tmp44E;_tmp472;});_npop_handler(0);return _tmp473;}}_LL3AB:;}{
# 2094
struct _RegionHandle*_tmp474=(env->fenv)->r;
struct _tuple13 _tmp475=Cyc_Relations_primop2relation(_tmp447,p,_tmp448);struct _tuple13 _tmp476=_tmp475;struct Cyc_Absyn_Exp*_tmp48C;enum Cyc_Relations_Relation _tmp48B;struct Cyc_Absyn_Exp*_tmp48A;_LL3BB: _tmp48C=_tmp476.f1;_tmp48B=_tmp476.f2;_tmp48A=_tmp476.f3;_LL3BC:;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
# 2099
if(Cyc_Relations_exp2relnop(_tmp48C,& n1) && Cyc_Relations_exp2relnop(_tmp48A,& n2)){
# 2101
struct Cyc_List_List*_tmp477=Cyc_Relations_add_relation(_tmp474,n1,_tmp48B,n2,_tmp48D);
# 2105
struct Cyc_List_List*_tmp478=({struct _RegionHandle*_tmp82D=_tmp474;union Cyc_Relations_RelnOp _tmp82C=n2;enum Cyc_Relations_Relation _tmp82B=Cyc_Relations_flip_relation(_tmp48B);union Cyc_Relations_RelnOp _tmp82A=n1;Cyc_Relations_add_relation(_tmp82D,_tmp82C,_tmp82B,_tmp82A,_tmp48D);});
struct _tuple19 _tmp479=({struct _tuple19 _tmp487;_tmp487.f1=_tmp44D;_tmp487.f2=_tmp44E;_tmp487;});struct _tuple19 _tmp47A=_tmp479;struct Cyc_Dict_Dict _tmp486;struct Cyc_Dict_Dict _tmp485;struct Cyc_Dict_Dict _tmp484;struct Cyc_Dict_Dict _tmp483;if(((_tmp47A.f1).ReachableFL).tag == 2){if(((_tmp47A.f2).ReachableFL).tag == 2){_LL3BE: _tmp484=(((_tmp47A.f1).ReachableFL).val).f1;_tmp483=(((_tmp47A.f2).ReachableFL).val).f1;_LL3BF: {
# 2108
struct _tuple19 _tmp47C=({struct _tuple19 _tmp47B;({union Cyc_CfFlowInfo_FlowInfo _tmp82E=Cyc_CfFlowInfo_ReachableFL(_tmp484,_tmp477);_tmp47B.f1=_tmp82E;});({union Cyc_CfFlowInfo_FlowInfo _tmp82F=Cyc_CfFlowInfo_ReachableFL(_tmp483,_tmp478);_tmp47B.f2=_tmp82F;});_tmp47B;});_npop_handler(0);return _tmp47C;}}else{_LL3C2: _tmp485=(((_tmp47A.f1).ReachableFL).val).f1;_LL3C3: {
# 2112
struct _tuple19 _tmp480=({struct _tuple19 _tmp47F;({union Cyc_CfFlowInfo_FlowInfo _tmp830=Cyc_CfFlowInfo_ReachableFL(_tmp485,_tmp477);_tmp47F.f1=_tmp830;});_tmp47F.f2=_tmp44E;_tmp47F;});_npop_handler(0);return _tmp480;}}}else{if(((_tmp47A.f2).ReachableFL).tag == 2){_LL3C0: _tmp486=(((_tmp47A.f2).ReachableFL).val).f1;_LL3C1: {
# 2110
struct _tuple19 _tmp47E=({struct _tuple19 _tmp47D;_tmp47D.f1=_tmp44D;({union Cyc_CfFlowInfo_FlowInfo _tmp831=Cyc_CfFlowInfo_ReachableFL(_tmp486,_tmp478);_tmp47D.f2=_tmp831;});_tmp47D;});_npop_handler(0);return _tmp47E;}}else{_LL3C4: _LL3C5: {
# 2114
struct _tuple19 _tmp482=({struct _tuple19 _tmp481;_tmp481.f1=_tmp44D;_tmp481.f2=_tmp44E;_tmp481;});_npop_handler(0);return _tmp482;}}}_LL3BD:;}else{
# 2117
struct _tuple19 _tmp489=({struct _tuple19 _tmp488;_tmp488.f1=_tmp44D;_tmp488.f2=_tmp44E;_tmp488;});_npop_handler(0);return _tmp489;}};};};}}_LL370:;};
# 2018
;_pop_region(temp);}
# 2123
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 2125
struct Cyc_CfFlowInfo_FlowEnv*_tmp48F=env->fenv;
void*_tmp490=e->r;void*_tmp491=_tmp490;enum Cyc_Absyn_Primop _tmp4E0;struct Cyc_List_List*_tmp4DF;struct Cyc_Absyn_Exp*_tmp4DE;struct Cyc_Absyn_Exp*_tmp4DD;struct Cyc_Absyn_Exp*_tmp4DC;struct Cyc_Absyn_Exp*_tmp4DB;struct Cyc_Absyn_Exp*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4D9;struct Cyc_Absyn_Exp*_tmp4D8;struct Cyc_Absyn_Exp*_tmp4D7;struct Cyc_Absyn_Exp*_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D5;switch(*((int*)_tmp491)){case 6: _LL3C7: _tmp4D7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp491)->f1;_tmp4D6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp491)->f2;_tmp4D5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp491)->f3;_LL3C8: {
# 2128
struct _tuple19 _tmp492=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4D7);struct _tuple19 _tmp493=_tmp492;union Cyc_CfFlowInfo_FlowInfo _tmp49E;union Cyc_CfFlowInfo_FlowInfo _tmp49D;_LL3D6: _tmp49E=_tmp493.f1;_tmp49D=_tmp493.f2;_LL3D7:;{
struct _tuple19 _tmp494=Cyc_NewControlFlow_anal_test(env,_tmp49E,_tmp4D6);struct _tuple19 _tmp495=_tmp494;union Cyc_CfFlowInfo_FlowInfo _tmp49C;union Cyc_CfFlowInfo_FlowInfo _tmp49B;_LL3D9: _tmp49C=_tmp495.f1;_tmp49B=_tmp495.f2;_LL3DA:;{
struct _tuple19 _tmp496=Cyc_NewControlFlow_anal_test(env,_tmp49D,_tmp4D5);struct _tuple19 _tmp497=_tmp496;union Cyc_CfFlowInfo_FlowInfo _tmp49A;union Cyc_CfFlowInfo_FlowInfo _tmp499;_LL3DC: _tmp49A=_tmp497.f1;_tmp499=_tmp497.f2;_LL3DD:;
return({struct _tuple19 _tmp498;({union Cyc_CfFlowInfo_FlowInfo _tmp832=Cyc_CfFlowInfo_join_flow(_tmp48F,_tmp49C,_tmp49A);_tmp498.f1=_tmp832;});({union Cyc_CfFlowInfo_FlowInfo _tmp833=
Cyc_CfFlowInfo_join_flow(_tmp48F,_tmp49B,_tmp499);_tmp498.f2=_tmp833;});_tmp498;});};};}case 7: _LL3C9: _tmp4D9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp491)->f1;_tmp4D8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp491)->f2;_LL3CA: {
# 2134
struct _tuple19 _tmp49F=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4D9);struct _tuple19 _tmp4A0=_tmp49F;union Cyc_CfFlowInfo_FlowInfo _tmp4A7;union Cyc_CfFlowInfo_FlowInfo _tmp4A6;_LL3DF: _tmp4A7=_tmp4A0.f1;_tmp4A6=_tmp4A0.f2;_LL3E0:;{
struct _tuple19 _tmp4A1=Cyc_NewControlFlow_anal_test(env,_tmp4A7,_tmp4D8);struct _tuple19 _tmp4A2=_tmp4A1;union Cyc_CfFlowInfo_FlowInfo _tmp4A5;union Cyc_CfFlowInfo_FlowInfo _tmp4A4;_LL3E2: _tmp4A5=_tmp4A2.f1;_tmp4A4=_tmp4A2.f2;_LL3E3:;
return({struct _tuple19 _tmp4A3;_tmp4A3.f1=_tmp4A5;({union Cyc_CfFlowInfo_FlowInfo _tmp834=Cyc_CfFlowInfo_join_flow(_tmp48F,_tmp4A6,_tmp4A4);_tmp4A3.f2=_tmp834;});_tmp4A3;});};}case 8: _LL3CB: _tmp4DB=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp491)->f1;_tmp4DA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp491)->f2;_LL3CC: {
# 2138
struct _tuple19 _tmp4A8=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4DB);struct _tuple19 _tmp4A9=_tmp4A8;union Cyc_CfFlowInfo_FlowInfo _tmp4B0;union Cyc_CfFlowInfo_FlowInfo _tmp4AF;_LL3E5: _tmp4B0=_tmp4A9.f1;_tmp4AF=_tmp4A9.f2;_LL3E6:;{
struct _tuple19 _tmp4AA=Cyc_NewControlFlow_anal_test(env,_tmp4AF,_tmp4DA);struct _tuple19 _tmp4AB=_tmp4AA;union Cyc_CfFlowInfo_FlowInfo _tmp4AE;union Cyc_CfFlowInfo_FlowInfo _tmp4AD;_LL3E8: _tmp4AE=_tmp4AB.f1;_tmp4AD=_tmp4AB.f2;_LL3E9:;
return({struct _tuple19 _tmp4AC;({union Cyc_CfFlowInfo_FlowInfo _tmp835=Cyc_CfFlowInfo_join_flow(_tmp48F,_tmp4B0,_tmp4AE);_tmp4AC.f1=_tmp835;});_tmp4AC.f2=_tmp4AD;_tmp4AC;});};}case 9: _LL3CD: _tmp4DD=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp491)->f1;_tmp4DC=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp491)->f2;_LL3CE: {
# 2142
struct _tuple16 _tmp4B1=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp4DD);struct _tuple16 _tmp4B2=_tmp4B1;union Cyc_CfFlowInfo_FlowInfo _tmp4B4;void*_tmp4B3;_LL3EB: _tmp4B4=_tmp4B2.f1;_tmp4B3=_tmp4B2.f2;_LL3EC:;
return Cyc_NewControlFlow_anal_test(env,_tmp4B4,_tmp4DC);}case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp491)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp491)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp491)->f2)->tl == 0){_LL3CF: _tmp4DE=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp491)->f2)->hd;_LL3D0: {
# 2145
struct _tuple19 _tmp4B5=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4DE);struct _tuple19 _tmp4B6=_tmp4B5;union Cyc_CfFlowInfo_FlowInfo _tmp4B9;union Cyc_CfFlowInfo_FlowInfo _tmp4B8;_LL3EE: _tmp4B9=_tmp4B6.f1;_tmp4B8=_tmp4B6.f2;_LL3EF:;
return({struct _tuple19 _tmp4B7;_tmp4B7.f1=_tmp4B8;_tmp4B7.f2=_tmp4B9;_tmp4B7;});}}else{goto _LL3D1;}}else{goto _LL3D1;}}else{_LL3D1: _tmp4E0=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp491)->f1;_tmp4DF=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp491)->f2;_LL3D2:
# 2148
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp4E0,_tmp4DF);}default: _LL3D3: _LL3D4: {
# 2152
struct _tuple16 _tmp4BA=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);struct _tuple16 _tmp4BB=_tmp4BA;union Cyc_CfFlowInfo_FlowInfo _tmp4D4;void*_tmp4D3;_LL3F1: _tmp4D4=_tmp4BB.f1;_tmp4D3=_tmp4BB.f2;_LL3F2:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4BC=_tmp4D4;struct Cyc_Dict_Dict _tmp4D2;if((_tmp4BC.BottomFL).tag == 1){_LL3F4: _LL3F5:
 return({struct _tuple19 _tmp4BD;_tmp4BD.f1=_tmp4D4;_tmp4BD.f2=_tmp4D4;_tmp4BD;});}else{_LL3F6: _tmp4D2=((_tmp4BC.ReachableFL).val).f1;_LL3F7: {
# 2156
struct _tuple15 _tmp4BE=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,_tmp4D3);struct _tuple15 _tmp4BF=_tmp4BE;void*_tmp4D1;struct Cyc_List_List*_tmp4D0;_LL3F9: _tmp4D1=_tmp4BF.f1;_tmp4D0=_tmp4BF.f2;_LL3FA:;{
void*_tmp4C0=_tmp4D1;enum Cyc_CfFlowInfo_InitLevel _tmp4CF;struct Cyc_Absyn_Vardecl*_tmp4CE;void*_tmp4CD;switch(*((int*)_tmp4C0)){case 8: _LL3FC: _tmp4CE=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4C0)->f1;_tmp4CD=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4C0)->f2;_LL3FD:
# 2159
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp4C1=_cycalloc(sizeof(*_tmp4C1));({struct Cyc_Core_Impossible_exn_struct _tmp837=({struct Cyc_Core_Impossible_exn_struct _tmp4C2;_tmp4C2.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp836=({const char*_tmp4C3="anal_test: bad namedlocation";_tag_dyneither(_tmp4C3,sizeof(char),29);});_tmp4C2.f1=_tmp836;});_tmp4C2;});_tmp4C1[0]=_tmp837;});_tmp4C1;}));case 0: _LL3FE: _LL3FF:
 return({struct _tuple19 _tmp4C4;({union Cyc_CfFlowInfo_FlowInfo _tmp838=Cyc_CfFlowInfo_BottomFL();_tmp4C4.f1=_tmp838;});_tmp4C4.f2=_tmp4D4;_tmp4C4;});case 1: _LL400: _LL401:
 goto _LL403;case 5: _LL402: _LL403:
 goto _LL405;case 4: _LL404: _LL405:
 return({struct _tuple19 _tmp4C5;_tmp4C5.f1=_tmp4D4;({union Cyc_CfFlowInfo_FlowInfo _tmp839=Cyc_CfFlowInfo_BottomFL();_tmp4C5.f2=_tmp839;});_tmp4C5;});case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4C0)->f1 == Cyc_CfFlowInfo_NoneIL){_LL406: _LL407:
 goto _LL409;}else{_LL40C: _tmp4CF=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4C0)->f1;_LL40D:
# 2169
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp4D4,e,_tmp4CF,_tmp4D0);}case 3: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp4C0)->f1 == Cyc_CfFlowInfo_NoneIL){_LL408: _LL409:
# 2165
 goto _LL40B;}else{_LL40E: _LL40F:
# 2170
 return({struct _tuple19 _tmp4C9;_tmp4C9.f1=_tmp4D4;_tmp4C9.f2=_tmp4D4;_tmp4C9;});}case 7: _LL40A: _LL40B:
# 2167
({void*_tmp4C6=0;({unsigned int _tmp83B=e->loc;struct _dyneither_ptr _tmp83A=({const char*_tmp4C7="expression may not be initialized";_tag_dyneither(_tmp4C7,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp83B,_tmp83A,_tag_dyneither(_tmp4C6,sizeof(void*),0));});});
return({struct _tuple19 _tmp4C8;({union Cyc_CfFlowInfo_FlowInfo _tmp83C=Cyc_CfFlowInfo_BottomFL();_tmp4C8.f1=_tmp83C;});({union Cyc_CfFlowInfo_FlowInfo _tmp83D=Cyc_CfFlowInfo_BottomFL();_tmp4C8.f2=_tmp83D;});_tmp4C8;});default: _LL410: _LL411:
# 2171
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp4CA=_cycalloc(sizeof(*_tmp4CA));({struct Cyc_Core_Impossible_exn_struct _tmp83F=({struct Cyc_Core_Impossible_exn_struct _tmp4CB;_tmp4CB.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp83E=({const char*_tmp4CC="anal_test";_tag_dyneither(_tmp4CC,sizeof(char),10);});_tmp4CB.f1=_tmp83E;});_tmp4CB;});_tmp4CA[0]=_tmp83F;});_tmp4CA;}));}_LL3FB:;};}}_LL3F3:;};}}_LL3C6:;}struct _tuple27{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 2177
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple27*ckenv,void*root,void*rval){
# 2179
struct _tuple27*_tmp4E1=ckenv;unsigned int _tmp4ED;struct Cyc_NewControlFlow_AnalEnv*_tmp4EC;struct Cyc_Dict_Dict _tmp4EB;_LL413: _tmp4ED=_tmp4E1->f1;_tmp4EC=_tmp4E1->f2;_tmp4EB=_tmp4E1->f3;_LL414:;{
void*_tmp4E2=root;struct Cyc_Absyn_Vardecl*_tmp4EA;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4E2)->tag == 0){_LL416: _tmp4EA=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4E2)->f1;_LL417:
# 2182
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp4EA->type)){
struct _tuple15 _tmp4E3=Cyc_CfFlowInfo_unname_rval((_tmp4EC->fenv)->r,rval);struct _tuple15 _tmp4E4=_tmp4E3;void*_tmp4E9;_LL41B: _tmp4E9=_tmp4E4.f1;_LL41C:;{
void*_tmp4E5=_tmp4E9;switch(*((int*)_tmp4E5)){case 7: _LL41E: _LL41F:
 goto _LL421;case 0: _LL420: _LL421:
 goto _LL423;case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4E5)->f1 == Cyc_CfFlowInfo_NoneIL){_LL422: _LL423:
 goto _LL41D;}else{goto _LL424;}default: _LL424: _LL425:
# 2189
({struct Cyc_String_pa_PrintArg_struct _tmp4E8;_tmp4E8.tag=0;({struct _dyneither_ptr _tmp840=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4EA->name));_tmp4E8.f1=_tmp840;});({void*_tmp4E6[1]={& _tmp4E8};({unsigned int _tmp842=_tmp4ED;struct _dyneither_ptr _tmp841=({const char*_tmp4E7="unique pointers reachable from %s may become unreachable";_tag_dyneither(_tmp4E7,sizeof(char),57);});Cyc_Tcutil_warn(_tmp842,_tmp841,_tag_dyneither(_tmp4E6,sizeof(void*),1));});});});
goto _LL41D;}_LL41D:;};}
# 2193
goto _LL415;}else{_LL418: _LL419:
 goto _LL415;}_LL415:;};}
# 2198
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp4EE=flow;struct Cyc_Dict_Dict _tmp4F4;if((_tmp4EE.BottomFL).tag == 1){_LL427: _LL428:
 return;}else{_LL429: _tmp4F4=((_tmp4EE.ReachableFL).val).f1;_LL42A:
# 2202
{struct Cyc_List_List*_tmp4EF=env->param_roots;for(0;_tmp4EF != 0;_tmp4EF=_tmp4EF->tl){
if(({struct Cyc_CfFlowInfo_FlowEnv*_tmp844=env->fenv;struct Cyc_Dict_Dict _tmp843=_tmp4F4;Cyc_CfFlowInfo_initlevel(_tmp844,_tmp843,Cyc_CfFlowInfo_lookup_place(_tmp4F4,(struct Cyc_CfFlowInfo_Place*)_tmp4EF->hd));})!= Cyc_CfFlowInfo_AllIL)
({void*_tmp4F0=0;({unsigned int _tmp846=loc;struct _dyneither_ptr _tmp845=({const char*_tmp4F1="function may not initialize all the parameters with attribute 'initializes'";_tag_dyneither(_tmp4F1,sizeof(char),76);});Cyc_CfFlowInfo_aerr(_tmp846,_tmp845,_tag_dyneither(_tmp4F0,sizeof(void*),0));});});}}
# 2206
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple27 _tmp4F2=({struct _tuple27 _tmp4F3;_tmp4F3.f1=loc;_tmp4F3.f2=env;_tmp4F3.f3=_tmp4F4;_tmp4F3;});
((void(*)(void(*f)(struct _tuple27*,void*,void*),struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp4F2,_tmp4F4);}
# 2210
return;}_LL426:;}
# 2219
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_pat_vars(struct _RegionHandle*frgn,struct Cyc_List_List*vds){
# 2222
struct Cyc_List_List*_tmp4F5=0;
struct Cyc_List_List*_tmp4F6=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp4F7=(struct _tuple24*)x->hd;struct _tuple24*_tmp4F8=_tmp4F7;struct Cyc_Absyn_Vardecl**_tmp501;struct Cyc_Absyn_Exp*_tmp500;_LL42C: _tmp501=_tmp4F8->f1;_tmp500=_tmp4F8->f2;_LL42D:;
if((_tmp501 != 0  && _tmp500 != 0) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp500->topt),0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4F9=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4FE=_region_malloc(frgn,sizeof(*_tmp4FE));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp847=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp4FF;_tmp4FF.tag=0;_tmp4FF.f1=*_tmp501;_tmp4FF;});_tmp4FE[0]=_tmp847;});_tmp4FE;});
struct Cyc_CfFlowInfo_Place*_tmp4FA=({struct Cyc_CfFlowInfo_Place*_tmp4FD=_region_malloc(frgn,sizeof(*_tmp4FD));_tmp4FD->root=(void*)_tmp4F9;_tmp4FD->path=0;_tmp4FD;});
({struct Cyc_List_List*_tmp848=({struct Cyc_List_List*_tmp4FB=_region_malloc(frgn,sizeof(*_tmp4FB));_tmp4FB->hd=_tmp4FA;_tmp4FB->tl=_tmp4F5;_tmp4FB;});_tmp4F5=_tmp848;});
({struct Cyc_List_List*_tmp849=({struct Cyc_List_List*_tmp4FC=_region_malloc(frgn,sizeof(*_tmp4FC));_tmp4FC->hd=_tmp500;_tmp4FC->tl=_tmp4F6;_tmp4FC;});_tmp4F6=_tmp849;});}}}
# 2234
return({struct _tuple1 _tmp502;_tmp502.f1=_tmp4F5;_tmp502.f2=_tmp4F6;_tmp502;});}struct _tuple28{int f1;void*f2;};
# 2240
static struct _tuple28 Cyc_NewControlFlow_noconsume_place_ok(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*place,int do_unconsume,struct Cyc_Absyn_Vardecl*vd,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2247
union Cyc_CfFlowInfo_FlowInfo _tmp503=flow;struct Cyc_Dict_Dict _tmp518;if((_tmp503.BottomFL).tag == 1){_LL42F: _LL430:
# 2249
({void*_tmp504=0;({struct _dyneither_ptr _tmp84A=({const char*_tmp505="noconsume_place_ok: flow became Bottom!";_tag_dyneither(_tmp505,sizeof(char),40);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp84A,_tag_dyneither(_tmp504,sizeof(void*),0));});});}else{_LL431: _tmp518=((_tmp503.ReachableFL).val).f1;_LL432: {
# 2256
struct Cyc_Absyn_Exp*_tmp506=Cyc_Absyn_exp_unsigned_one;
int _tmp507=0;
int _tmp508=1;
void*_tmp509=Cyc_CfFlowInfo_lookup_place(_tmp518,place);
void*_tmp50A=_tmp509;
# 2268
int varok=0;
{void*_tmp50B=_tmp509;struct Cyc_Absyn_Vardecl*_tmp516;void*_tmp515;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp50B)->tag == 8){_LL434: _tmp516=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp50B)->f1;_tmp515=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp50B)->f2;_LL435:
# 2271
 if(vd == _tmp516){
_tmp50A=_tmp515;
# 2274
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2276
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp506,_tmp508,_tmp50A,& _tmp507)){
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp50E;_tmp50E.tag=0;({struct _dyneither_ptr _tmp84B=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2280
Cyc_Absynpp_qvar2string(vd->name));_tmp50E.f1=_tmp84B;});({void*_tmp50C[1]={& _tmp50E};({unsigned int _tmp84D=loc;struct _dyneither_ptr _tmp84C=({const char*_tmp50D="function consumes parameter %s with attribute 'noconsume'";_tag_dyneither(_tmp50D,sizeof(char),58);});Cyc_CfFlowInfo_aerr(_tmp84D,_tmp84C,_tag_dyneither(_tmp50C,sizeof(void*),1));});});});}else{
# 2283
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp518,_tmp50A)!= Cyc_CfFlowInfo_AllIL  && !do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp511;_tmp511.tag=0;({struct _dyneither_ptr _tmp84E=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2286
Cyc_Absynpp_qvar2string(vd->name));_tmp511.f1=_tmp84E;});({void*_tmp50F[1]={& _tmp511};({unsigned int _tmp850=loc;struct _dyneither_ptr _tmp84F=({const char*_tmp510="function consumes value pointed to by parameter %s, which has attribute 'noconsume'";_tag_dyneither(_tmp510,sizeof(char),84);});Cyc_CfFlowInfo_aerr(_tmp850,_tmp84F,_tag_dyneither(_tmp50F,sizeof(void*),1));});});});else{
# 2288
varok=1;}}}else{
# 2291
varok=1;}}else{
# 2294
goto _LL437;}
goto _LL433;}else{_LL436: _LL437:
# 2298
 if(!Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
varok=1;else{
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp514;_tmp514.tag=0;({struct _dyneither_ptr _tmp851=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2303
Cyc_Absynpp_qvar2string(vd->name));_tmp514.f1=_tmp851;});({void*_tmp512[1]={& _tmp514};({unsigned int _tmp853=loc;struct _dyneither_ptr _tmp852=({const char*_tmp513="function parameter %s with attribute 'noconsume' no longer set to its original value";_tag_dyneither(_tmp513,sizeof(char),85);});Cyc_CfFlowInfo_aerr(_tmp853,_tmp852,_tag_dyneither(_tmp512,sizeof(void*),1));});});});}
goto _LL433;}_LL433:;}
# 2310
return({struct _tuple28 _tmp517;_tmp517.f1=varok;_tmp517.f2=_tmp50A;_tmp517;});}}_LL42E:;}
# 2316
static struct Cyc_Absyn_Vardecl*Cyc_NewControlFlow_get_vd_from_place(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_CfFlowInfo_Place*_tmp519=p;void*_tmp521;struct Cyc_List_List*_tmp520;_LL439: _tmp521=_tmp519->root;_tmp520=_tmp519->path;_LL43A:;
if(_tmp520 != 0)
({void*_tmp51A=0;({struct _dyneither_ptr _tmp854=({const char*_tmp51B="unconsume_params: param to unconsume is non-variable";_tag_dyneither(_tmp51B,sizeof(char),53);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp854,_tag_dyneither(_tmp51A,sizeof(void*),0));});});{
struct Cyc_Absyn_Vardecl*vd;
void*_tmp51C=_tmp521;struct Cyc_Absyn_Vardecl*_tmp51F;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp51C)->tag == 0){_LL43C: _tmp51F=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp51C)->f1;_LL43D:
 return _tmp51F;}else{_LL43E: _LL43F:
({void*_tmp51D=0;({struct _dyneither_ptr _tmp855=({const char*_tmp51E="unconsume_params: root is not a varroot";_tag_dyneither(_tmp51E,sizeof(char),40);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp855,_tag_dyneither(_tmp51D,sizeof(void*),0));});});}_LL43B:;};}
# 2335 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_exp(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Exp*unconsume_exp,struct Cyc_Absyn_Vardecl*vd,int varok,void*ropt,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2343
{union Cyc_CfFlowInfo_FlowInfo _tmp522=flow;struct Cyc_Dict_Dict _tmp53B;if((_tmp522.BottomFL).tag == 1){_LL441: _LL442:
 return flow;}else{_LL443: _tmp53B=((_tmp522.ReachableFL).val).f1;_LL444: {
# 2350
struct _tuple18 _tmp523=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,unconsume_exp);struct _tuple18 _tmp524=_tmp523;union Cyc_CfFlowInfo_FlowInfo _tmp53A;union Cyc_CfFlowInfo_AbsLVal _tmp539;_LL446: _tmp53A=_tmp524.f1;_tmp539=_tmp524.f2;_LL447:;
# 2353
{union Cyc_CfFlowInfo_AbsLVal _tmp525=_tmp539;struct Cyc_CfFlowInfo_Place*_tmp538;if((_tmp525.PlaceL).tag == 1){_LL449: _tmp538=(_tmp525.PlaceL).val;_LL44A: {
# 2357
void*_tmp526=Cyc_CfFlowInfo_lookup_place(_tmp53B,_tmp538);
{void*_tmp527=_tmp526;struct Cyc_Absyn_Vardecl*_tmp537;void*_tmp536;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp527)->tag == 8){_LL44E: _tmp537=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp527)->f1;_tmp536=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp527)->f2;_LL44F: {
# 2363
void*new_rval;
if(_tmp537 == vd){
# 2367
if(varok){
# 2369
({void*_tmp856=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,_tmp536);_tmp526=_tmp856;});
# 2374
if(ropt != 0){
# 2380
struct _tuple16 _tmp528=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp858=env->fenv;struct _tuple16 _tmp857=({struct _tuple16 _tmp52C;_tmp52C.f1=_tmp53A;_tmp52C.f2=_tmp526;_tmp52C;});Cyc_CfFlowInfo_join_flow_and_rval(_tmp858,_tmp857,({struct _tuple16 _tmp52D;_tmp52D.f1=_tmp53A;_tmp52D.f2=ropt;_tmp52D;}));});
# 2380
struct _tuple16 _tmp529=_tmp528;union Cyc_CfFlowInfo_FlowInfo _tmp52B;void*_tmp52A;_LL453: _tmp52B=_tmp529.f1;_tmp52A=_tmp529.f2;_LL454:;
# 2384
_tmp53A=_tmp52B;new_rval=_tmp52A;}else{
# 2389
new_rval=_tmp526;}}else{
# 2392
new_rval=_tmp536;}
# 2394
{union Cyc_CfFlowInfo_FlowInfo _tmp52E=_tmp53A;struct Cyc_Dict_Dict _tmp532;struct Cyc_List_List*_tmp531;if((_tmp52E.ReachableFL).tag == 2){_LL456: _tmp532=((_tmp52E.ReachableFL).val).f1;_tmp531=((_tmp52E.ReachableFL).val).f2;_LL457:
# 2396
({union Cyc_CfFlowInfo_FlowInfo _tmp85A=({struct Cyc_Dict_Dict _tmp859=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp532,_tmp538,new_rval);Cyc_CfFlowInfo_ReachableFL(_tmp859,_tmp531);});flow=_tmp85A;});
# 2400
goto _LL455;}else{_LL458: _LL459:
# 2402
({void*_tmp52F=0;({struct _dyneither_ptr _tmp85B=({const char*_tmp530="unconsume_params: joined flow became bot!";_tag_dyneither(_tmp530,sizeof(char),42);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp85B,_tag_dyneither(_tmp52F,sizeof(void*),0));});});}_LL455:;}
# 2404
goto _LL44D;}else{
# 2406
goto _LL451;}
goto _LL44D;}}else{_LL450: _LL451:
# 2413
 if(ropt != 0  && !
Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
({struct Cyc_String_pa_PrintArg_struct _tmp535;_tmp535.tag=0;({struct _dyneither_ptr _tmp85C=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(unconsume_exp));_tmp535.f1=_tmp85C;});({void*_tmp533[1]={& _tmp535};({unsigned int _tmp85E=loc;struct _dyneither_ptr _tmp85D=({const char*_tmp534="aliased expression %s was overwritten";_tag_dyneither(_tmp534,sizeof(char),38);});Cyc_CfFlowInfo_aerr(_tmp85E,_tmp85D,_tag_dyneither(_tmp533,sizeof(void*),1));});});});
# 2430 "new_control_flow.cyc"
goto _LL44D;}_LL44D:;}
# 2432
goto _LL448;}}else{_LL44B: _LL44C:
# 2438
 goto _LL448;}_LL448:;}
# 2440
goto _LL440;}}_LL440:;}
# 2442
return flow;}
# 2455 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,int is_region_open,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2461
{union Cyc_CfFlowInfo_FlowInfo _tmp53C=flow;if((_tmp53C.BottomFL).tag == 1){_LL45B: _LL45C:
 return flow;}else{_LL45D: _LL45E:
 goto _LL45A;}_LL45A:;}{
# 2465
int _tmp53D=unconsume_exps != 0;
struct Cyc_List_List*_tmp53E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(env->r,consumed_vals);
struct Cyc_List_List*_tmp53F=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(env->r,unconsume_exps);
{struct Cyc_List_List*_tmp540=_tmp53E;for(0;_tmp540 != 0;
(_tmp540=_tmp540->tl,
_tmp53F != 0?_tmp53F=_tmp53F->tl: 0)){
# 2474
struct Cyc_Absyn_Vardecl*_tmp541=Cyc_NewControlFlow_get_vd_from_place((struct Cyc_CfFlowInfo_Place*)_tmp540->hd);
struct _tuple28 _tmp542=
is_region_open?({struct _tuple28 _tmp546;_tmp546.f1=1;_tmp546.f2=0;_tmp546;}):
 Cyc_NewControlFlow_noconsume_place_ok(env,(struct Cyc_CfFlowInfo_Place*)_tmp540->hd,_tmp53D,_tmp541,flow,loc);
# 2475
struct _tuple28 _tmp543=_tmp542;int _tmp545;void*_tmp544;_LL460: _tmp545=_tmp543.f1;_tmp544=_tmp543.f2;_LL461:;
# 2481
if(_tmp53D)
({union Cyc_CfFlowInfo_FlowInfo _tmp85F=Cyc_NewControlFlow_unconsume_exp(env,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp53F))->hd,_tmp541,_tmp545,_tmp544,flow,loc);flow=_tmp85F;});}}
# 2484
Cyc_NewControlFlow_update_tryflow(env,flow);
return flow;};}struct _tuple29{int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 2488
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned int exp_loc){
# 2491
struct Cyc_CfFlowInfo_FlowEnv*_tmp547=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp548=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_tmp549=_tmp548;struct Cyc_Core_Opt*_tmp55D;struct Cyc_Absyn_Exp*_tmp55C;struct Cyc_Absyn_Stmt*_tmp55B;unsigned int _tmp55A;_LL463: _tmp55D=_tmp549->pat_vars;_tmp55C=_tmp549->where_clause;_tmp55B=_tmp549->body;_tmp55A=_tmp549->loc;_LL464:;{
struct _tuple1 _tmp54A=Cyc_NewControlFlow_get_unconsume_pat_vars((env->fenv)->r,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp55D))->v);struct _tuple1 _tmp54B=_tmp54A;struct Cyc_List_List*_tmp559;struct Cyc_List_List*_tmp558;_LL466: _tmp559=_tmp54B.f1;_tmp558=_tmp54B.f2;_LL467:;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)_tmp55D->v,_tmp559 != 0,_tmp55A,exp_loc);
# 2499
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp54C=env->unique_pat_vars;
# 2502
if(Cyc_Tcpat_has_vars(_tmp55D))
({struct Cyc_List_List*_tmp861=({struct Cyc_List_List*_tmp54D=_region_malloc(env->r,sizeof(*_tmp54D));({struct _tuple29*_tmp860=({struct _tuple29*_tmp54E=_region_malloc(env->r,sizeof(*_tmp54E));_tmp54E->f1=0;_tmp54E->f2=_tmp55B;_tmp54E->f3=_tmp559;_tmp54E->f4=_tmp558;_tmp54E;});_tmp54D->hd=_tmp860;});_tmp54D->tl=_tmp54C;_tmp54D;});env->unique_pat_vars=_tmp861;});
# 2507
if(_tmp55C != 0){
struct Cyc_Absyn_Exp*wexp=_tmp55C;
struct _tuple19 _tmp54F=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);struct _tuple19 _tmp550=_tmp54F;union Cyc_CfFlowInfo_FlowInfo _tmp552;union Cyc_CfFlowInfo_FlowInfo _tmp551;_LL469: _tmp552=_tmp550.f1;_tmp551=_tmp550.f2;_LL46A:;
inflow=_tmp551;
({union Cyc_CfFlowInfo_FlowInfo _tmp862=Cyc_NewControlFlow_anal_stmt(env,_tmp552,_tmp55B,0);clause_outflow=_tmp862;});}else{
# 2513
({union Cyc_CfFlowInfo_FlowInfo _tmp863=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp55B,0);clause_outflow=_tmp863;});}
# 2515
env->unique_pat_vars=_tmp54C;{
union Cyc_CfFlowInfo_FlowInfo _tmp553=clause_outflow;if((_tmp553.BottomFL).tag == 1){_LL46C: _LL46D:
 goto _LL46B;}else{_LL46E: _LL46F:
# 2520
({union Cyc_CfFlowInfo_FlowInfo _tmp864=Cyc_NewControlFlow_unconsume_params(env,_tmp559,_tmp558,0,clause_outflow,_tmp55A);clause_outflow=_tmp864;});
# 2522
if(scs->tl == 0)
return clause_outflow;else{
# 2527
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0)
({void*_tmp554=0;({unsigned int _tmp866=_tmp55B->loc;struct _dyneither_ptr _tmp865=({const char*_tmp555="switch clause may implicitly fallthru";_tag_dyneither(_tmp555,sizeof(char),38);});Cyc_CfFlowInfo_aerr(_tmp866,_tmp865,_tag_dyneither(_tmp554,sizeof(void*),0));});});else{
# 2530
({void*_tmp556=0;({unsigned int _tmp868=_tmp55B->loc;struct _dyneither_ptr _tmp867=({const char*_tmp557="switch clause may implicitly fallthru";_tag_dyneither(_tmp557,sizeof(char),38);});Cyc_Tcutil_warn(_tmp868,_tmp867,_tag_dyneither(_tmp556,sizeof(void*),0));});});}
# 2532
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2534
goto _LL46B;}_LL46B:;};};};}
# 2537
return Cyc_CfFlowInfo_BottomFL();}struct _tuple30{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2542
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple30*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2544
struct _tuple30*_tmp55E=vdenv;struct Cyc_NewControlFlow_AnalEnv*_tmp569;struct Cyc_Dict_Dict _tmp568;unsigned int _tmp567;_LL471: _tmp569=_tmp55E->f1;_tmp568=_tmp55E->f2;_tmp567=_tmp55E->f3;_LL472:;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2547
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp566;_tmp566.tag=0;_tmp566.f1=vd;_tmp566;});
# 2549
struct _tuple15 _tmp55F=({struct _RegionHandle*_tmp869=(_tmp569->fenv)->r;Cyc_CfFlowInfo_unname_rval(_tmp869,
((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp568,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot));});
# 2549
struct _tuple15 _tmp560=_tmp55F;void*_tmp565;_LL474: _tmp565=_tmp560.f1;_LL475:;{
# 2551
void*_tmp561=_tmp565;switch(*((int*)_tmp561)){case 7: _LL477: _LL478:
 goto _LL47A;case 0: _LL479: _LL47A:
 goto _LL47C;case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp561)->f1 == Cyc_CfFlowInfo_NoneIL){_LL47B: _LL47C:
 goto _LL476;}else{goto _LL47D;}default: _LL47D: _LL47E:
# 2556
({struct Cyc_String_pa_PrintArg_struct _tmp564;_tmp564.tag=0;({struct _dyneither_ptr _tmp86A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp564.f1=_tmp86A;});({void*_tmp562[1]={& _tmp564};({unsigned int _tmp86C=_tmp567;struct _dyneither_ptr _tmp86B=({const char*_tmp563="unique pointers may still exist after variable %s goes out of scope";_tag_dyneither(_tmp563,sizeof(char),68);});Cyc_Tcutil_warn(_tmp86C,_tmp86B,_tag_dyneither(_tmp562,sizeof(void*),1));});});});
# 2558
goto _LL476;}_LL476:;};}}
# 2563
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 2565
{union Cyc_CfFlowInfo_FlowInfo _tmp56A=inflow;struct Cyc_Dict_Dict _tmp577;if((_tmp56A.ReachableFL).tag == 2){_LL480: _tmp577=((_tmp56A.ReachableFL).val).f1;_LL481: {
# 2567
struct _tuple30 _tmp56B=({struct _tuple30 _tmp576;_tmp576.f1=env;_tmp576.f2=_tmp577;_tmp576.f3=decl->loc;_tmp576;});
struct Cyc_CfFlowInfo_FlowEnv*_tmp56C=env->fenv;
{void*_tmp56D=decl->r;void*_tmp56E=_tmp56D;struct Cyc_List_List*_tmp575;struct Cyc_List_List*_tmp574;struct Cyc_Absyn_Vardecl*_tmp573;switch(*((int*)_tmp56E)){case 0: _LL485: _tmp573=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp56E)->f1;_LL486:
# 2571
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp56B,_tmp573);
goto _LL484;case 2: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp56E)->f2 != 0){_LL487: _tmp574=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp56E)->f2)->v;_LL488: {
# 2574
struct _tuple1 _tmp56F=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp574);struct _tuple1 _tmp570=_tmp56F;struct Cyc_List_List*_tmp572;_LL48E: _tmp572=_tmp570.f1;_LL48F:;{
struct Cyc_List_List*_tmp571=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)(_tmp572);
_tmp575=_tmp571;goto _LL48A;};}}else{goto _LL48B;}case 3: _LL489: _tmp575=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp56E)->f1;_LL48A:
# 2578
((void(*)(void(*f)(struct _tuple30*,struct Cyc_Absyn_Vardecl*),struct _tuple30*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp56B,_tmp575);
goto _LL484;default: _LL48B: _LL48C:
 goto _LL484;}_LL484:;}
# 2582
goto _LL47F;}}else{_LL482: _LL483:
 goto _LL47F;}_LL47F:;}
# 2585
return;}
# 2591
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_pat_vars(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*src,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2595
int num_pop=((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->pat_pop_table,src);
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;num_pop > 0;(x=x->tl,-- num_pop)){
struct _tuple29*_tmp578=(struct _tuple29*)((struct Cyc_List_List*)_check_null(x))->hd;struct _tuple29*_tmp579=_tmp578;int _tmp57D;struct Cyc_Absyn_Stmt*_tmp57C;struct Cyc_List_List*_tmp57B;struct Cyc_List_List*_tmp57A;_LL491: _tmp57D=_tmp579->f1;_tmp57C=_tmp579->f2;_tmp57B=_tmp579->f3;_tmp57A=_tmp579->f4;_LL492:;
({union Cyc_CfFlowInfo_FlowInfo _tmp86D=Cyc_NewControlFlow_unconsume_params(env,_tmp57B,_tmp57A,_tmp57D,inflow,dest->loc);inflow=_tmp86D;});}}
# 2600
return inflow;}
# 2606
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple17*rval_opt){
# 2609
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple20 _tmp57E=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct _tuple20 _tmp57F=_tmp57E;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp622;union Cyc_CfFlowInfo_FlowInfo*_tmp621;_LL494: _tmp622=_tmp57F.f1;_tmp621=_tmp57F.f2;_LL495:;
inflow=*_tmp621;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp580=env->fenv;
# 2616
void*_tmp581=s->r;void*_tmp582=_tmp581;struct Cyc_Absyn_Stmt*_tmp620;struct Cyc_Absyn_Decl*_tmp61F;struct Cyc_Absyn_Stmt*_tmp61E;struct Cyc_Absyn_Vardecl*_tmp61D;struct Cyc_Absyn_Exp*_tmp61C;unsigned int _tmp61B;struct Cyc_Absyn_Stmt*_tmp61A;struct Cyc_List_List*_tmp619;struct Cyc_Absyn_Exp*_tmp618;unsigned int _tmp617;struct Cyc_Absyn_Stmt*_tmp616;struct Cyc_Absyn_Stmt*_tmp615;struct Cyc_List_List*_tmp614;void*_tmp613;struct Cyc_Absyn_Exp*_tmp612;struct Cyc_List_List*_tmp611;void*_tmp610;struct Cyc_List_List*_tmp60F;struct Cyc_Absyn_Switch_clause*_tmp60E;struct Cyc_Absyn_Exp*_tmp60D;struct Cyc_Absyn_Exp*_tmp60C;struct Cyc_Absyn_Stmt*_tmp60B;struct Cyc_Absyn_Exp*_tmp60A;struct Cyc_Absyn_Stmt*_tmp609;struct Cyc_Absyn_Stmt*_tmp608;struct Cyc_Absyn_Stmt*_tmp607;struct Cyc_Absyn_Exp*_tmp606;struct Cyc_Absyn_Stmt*_tmp605;struct Cyc_Absyn_Exp*_tmp604;struct Cyc_Absyn_Stmt*_tmp603;struct Cyc_Absyn_Stmt*_tmp602;struct Cyc_Absyn_Exp*_tmp601;struct Cyc_Absyn_Stmt*_tmp600;struct Cyc_Absyn_Stmt*_tmp5FF;struct Cyc_Absyn_Stmt*_tmp5FE;struct Cyc_Absyn_Stmt*_tmp5FD;struct Cyc_Absyn_Exp*_tmp5FC;struct Cyc_Absyn_Exp*_tmp5FB;switch(*((int*)_tmp582)){case 0: _LL497: _LL498:
 return inflow;case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp582)->f1 == 0){_LL499: _LL49A:
# 2620
 if(env->noreturn)
({void*_tmp583=0;({unsigned int _tmp86F=s->loc;struct _dyneither_ptr _tmp86E=({const char*_tmp584="`noreturn' function might return";_tag_dyneither(_tmp584,sizeof(char),33);});Cyc_CfFlowInfo_aerr(_tmp86F,_tmp86E,_tag_dyneither(_tmp583,sizeof(void*),0));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}else{_LL49B: _tmp5FB=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp582)->f1;_LL49C:
# 2626
 if(env->noreturn)
({void*_tmp585=0;({unsigned int _tmp871=s->loc;struct _dyneither_ptr _tmp870=({const char*_tmp586="`noreturn' function might return";_tag_dyneither(_tmp586,sizeof(char),33);});Cyc_CfFlowInfo_aerr(_tmp871,_tmp870,_tag_dyneither(_tmp585,sizeof(void*),0));});});{
struct _tuple16 _tmp587=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp5FB));struct _tuple16 _tmp588=_tmp587;union Cyc_CfFlowInfo_FlowInfo _tmp58A;void*_tmp589;_LL4C0: _tmp58A=_tmp588.f1;_tmp589=_tmp588.f2;_LL4C1:;
({union Cyc_CfFlowInfo_FlowInfo _tmp872=Cyc_NewControlFlow_use_Rval(env,_tmp5FB->loc,_tmp58A,_tmp589);_tmp58A=_tmp872;});
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp58A);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,_tmp58A,s->loc);
return Cyc_CfFlowInfo_BottomFL();};}case 1: _LL49D: _tmp5FC=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp582)->f1;_LL49E: {
# 2635
struct _tuple17*_tmp58B=rval_opt;void**_tmp591;int _tmp590;if(_tmp58B != 0){_LL4C3: _tmp591=(void**)& _tmp58B->f1;_tmp590=_tmp58B->f2;_LL4C4: {
# 2637
struct _tuple16 _tmp58C=Cyc_NewControlFlow_anal_Rexp(env,_tmp590,inflow,_tmp5FC);struct _tuple16 _tmp58D=_tmp58C;union Cyc_CfFlowInfo_FlowInfo _tmp58F;void*_tmp58E;_LL4C8: _tmp58F=_tmp58D.f1;_tmp58E=_tmp58D.f2;_LL4C9:;
*_tmp591=_tmp58E;
return _tmp58F;}}else{_LL4C5: _LL4C6:
# 2641
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp5FC)).f1;}_LL4C2:;}case 2: _LL49F: _tmp5FE=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp582)->f1;_tmp5FD=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp582)->f2;_LL4A0:
# 2645
 return({struct Cyc_NewControlFlow_AnalEnv*_tmp875=env;union Cyc_CfFlowInfo_FlowInfo _tmp874=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5FE,0);struct Cyc_Absyn_Stmt*_tmp873=_tmp5FD;Cyc_NewControlFlow_anal_stmt(_tmp875,_tmp874,_tmp873,rval_opt);});case 4: _LL4A1: _tmp601=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp582)->f1;_tmp600=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp582)->f2;_tmp5FF=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp582)->f3;_LL4A2: {
# 2648
struct _tuple19 _tmp592=Cyc_NewControlFlow_anal_test(env,inflow,_tmp601);struct _tuple19 _tmp593=_tmp592;union Cyc_CfFlowInfo_FlowInfo _tmp597;union Cyc_CfFlowInfo_FlowInfo _tmp596;_LL4CB: _tmp597=_tmp593.f1;_tmp596=_tmp593.f2;_LL4CC:;{
# 2655
union Cyc_CfFlowInfo_FlowInfo _tmp594=Cyc_NewControlFlow_anal_stmt(env,_tmp596,_tmp5FF,0);
union Cyc_CfFlowInfo_FlowInfo _tmp595=Cyc_NewControlFlow_anal_stmt(env,_tmp597,_tmp600,0);
return Cyc_CfFlowInfo_join_flow(_tmp580,_tmp595,_tmp594);};}case 5: _LL4A3: _tmp604=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp582)->f1).f1;_tmp603=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp582)->f1).f2;_tmp602=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp582)->f2;_LL4A4: {
# 2663
struct _tuple20 _tmp598=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp603);struct _tuple20 _tmp599=_tmp598;union Cyc_CfFlowInfo_FlowInfo*_tmp5A0;_LL4CE: _tmp5A0=_tmp599.f2;_LL4CF:;{
union Cyc_CfFlowInfo_FlowInfo _tmp59A=*_tmp5A0;
struct _tuple19 _tmp59B=Cyc_NewControlFlow_anal_test(env,_tmp59A,_tmp604);struct _tuple19 _tmp59C=_tmp59B;union Cyc_CfFlowInfo_FlowInfo _tmp59F;union Cyc_CfFlowInfo_FlowInfo _tmp59E;_LL4D1: _tmp59F=_tmp59C.f1;_tmp59E=_tmp59C.f2;_LL4D2:;{
union Cyc_CfFlowInfo_FlowInfo _tmp59D=Cyc_NewControlFlow_anal_stmt(env,_tmp59F,_tmp602,0);
Cyc_NewControlFlow_update_flow(env,_tmp603,_tmp59D);
return _tmp59E;};};}case 14: _LL4A5: _tmp607=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp582)->f1;_tmp606=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp582)->f2).f1;_tmp605=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp582)->f2).f2;_LL4A6: {
# 2673
union Cyc_CfFlowInfo_FlowInfo _tmp5A1=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp607,0);
struct _tuple20 _tmp5A2=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5A1,_tmp605);struct _tuple20 _tmp5A3=_tmp5A2;union Cyc_CfFlowInfo_FlowInfo*_tmp5A9;_LL4D4: _tmp5A9=_tmp5A3.f2;_LL4D5:;{
union Cyc_CfFlowInfo_FlowInfo _tmp5A4=*_tmp5A9;
struct _tuple19 _tmp5A5=Cyc_NewControlFlow_anal_test(env,_tmp5A4,_tmp606);struct _tuple19 _tmp5A6=_tmp5A5;union Cyc_CfFlowInfo_FlowInfo _tmp5A8;union Cyc_CfFlowInfo_FlowInfo _tmp5A7;_LL4D7: _tmp5A8=_tmp5A6.f1;_tmp5A7=_tmp5A6.f2;_LL4D8:;
Cyc_NewControlFlow_update_flow(env,_tmp607,_tmp5A8);
return _tmp5A7;};}case 9: _LL4A7: _tmp60D=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp582)->f1;_tmp60C=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp582)->f2).f1;_tmp60B=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp582)->f2).f2;_tmp60A=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp582)->f3).f1;_tmp609=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp582)->f3).f2;_tmp608=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp582)->f4;_LL4A8: {
# 2682
union Cyc_CfFlowInfo_FlowInfo _tmp5AA=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp60D)).f1;
struct _tuple20 _tmp5AB=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5AA,_tmp60B);struct _tuple20 _tmp5AC=_tmp5AB;union Cyc_CfFlowInfo_FlowInfo*_tmp5B8;_LL4DA: _tmp5B8=_tmp5AC.f2;_LL4DB:;{
union Cyc_CfFlowInfo_FlowInfo _tmp5AD=*_tmp5B8;
struct _tuple19 _tmp5AE=Cyc_NewControlFlow_anal_test(env,_tmp5AD,_tmp60C);struct _tuple19 _tmp5AF=_tmp5AE;union Cyc_CfFlowInfo_FlowInfo _tmp5B7;union Cyc_CfFlowInfo_FlowInfo _tmp5B6;_LL4DD: _tmp5B7=_tmp5AF.f1;_tmp5B6=_tmp5AF.f2;_LL4DE:;{
union Cyc_CfFlowInfo_FlowInfo _tmp5B0=Cyc_NewControlFlow_anal_stmt(env,_tmp5B7,_tmp608,0);
struct _tuple20 _tmp5B1=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5B0,_tmp609);struct _tuple20 _tmp5B2=_tmp5B1;union Cyc_CfFlowInfo_FlowInfo*_tmp5B5;_LL4E0: _tmp5B5=_tmp5B2.f2;_LL4E1:;{
union Cyc_CfFlowInfo_FlowInfo _tmp5B3=*_tmp5B5;
union Cyc_CfFlowInfo_FlowInfo _tmp5B4=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp5B3,_tmp60A)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp60B,_tmp5B4);
return _tmp5B6;};};};}case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp582)->f2 != 0){_LL4A9: _tmp60F=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp582)->f1;_tmp60E=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp582)->f2;_LL4AA: {
# 2694
struct _RegionHandle _tmp5B9=_new_region("temp");struct _RegionHandle*temp=& _tmp5B9;_push_region(temp);
{struct _tuple23 _tmp5BA=Cyc_NewControlFlow_anal_Rexps(temp,env,inflow,_tmp60F,1,1);struct _tuple23 _tmp5BB=_tmp5BA;union Cyc_CfFlowInfo_FlowInfo _tmp5C3;struct Cyc_List_List*_tmp5C2;_LL4E3: _tmp5C3=_tmp5BB.f1;_tmp5C2=_tmp5BB.f2;_LL4E4:;
# 2697
({union Cyc_CfFlowInfo_FlowInfo _tmp876=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,_tmp60E->body);inflow=_tmp876;});{
# 2699
struct Cyc_List_List*_tmp5BC=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp60E->pat_vars))->v)).f1);
({union Cyc_CfFlowInfo_FlowInfo _tmp877=Cyc_NewControlFlow_add_vars(_tmp580,_tmp5C3,_tmp5BC,_tmp580->unknown_all,s->loc,0);_tmp5C3=_tmp877;});
# 2702
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp60E->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp5BD=(struct _tuple24*)x->hd;struct _tuple24*_tmp5BE=_tmp5BD;struct Cyc_Absyn_Vardecl**_tmp5C0;struct Cyc_Absyn_Exp*_tmp5BF;_LL4E6: _tmp5C0=_tmp5BE->f1;_tmp5BF=_tmp5BE->f2;_LL4E7:;
if(_tmp5C0 != 0){
({union Cyc_CfFlowInfo_FlowInfo _tmp87E=({struct Cyc_CfFlowInfo_FlowEnv*_tmp87D=_tmp580;struct Cyc_NewControlFlow_AnalEnv*_tmp87C=env;union Cyc_CfFlowInfo_FlowInfo _tmp87B=_tmp5C3;struct Cyc_Absyn_Vardecl*_tmp87A=*_tmp5C0;struct Cyc_Absyn_Exp*_tmp879=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp60F))->hd;void*_tmp878=(void*)((struct Cyc_List_List*)_check_null(_tmp5C2))->hd;Cyc_NewControlFlow_do_initialize_var(_tmp87D,_tmp87C,_tmp87B,_tmp87A,_tmp879,_tmp878,s->loc);});_tmp5C3=_tmp87E;});
_tmp5C2=_tmp5C2->tl;
_tmp60F=_tmp60F->tl;}}}
# 2710
Cyc_NewControlFlow_update_flow(env,_tmp60E->body,_tmp5C3);{
union Cyc_CfFlowInfo_FlowInfo _tmp5C1=Cyc_CfFlowInfo_BottomFL();_npop_handler(0);return _tmp5C1;};};}
# 2695
;_pop_region(temp);}}else{_LL4BD: _LL4BE:
# 2879
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp5F8=_cycalloc(sizeof(*_tmp5F8));({struct Cyc_Core_Impossible_exn_struct _tmp880=({struct Cyc_Core_Impossible_exn_struct _tmp5F9;_tmp5F9.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp87F=({const char*_tmp5FA="anal_stmt -- bad stmt syntax or unimplemented stmt form";_tag_dyneither(_tmp5FA,sizeof(char),56);});_tmp5F9.f1=_tmp87F;});_tmp5F9;});_tmp5F8[0]=_tmp880;});_tmp5F8;}));}case 6: _LL4AB: _LL4AC:
# 2717
 if(((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->succ_table,s)== 0){
{union Cyc_CfFlowInfo_FlowInfo _tmp5C4=inflow;if((_tmp5C4.ReachableFL).tag == 2){_LL4E9: _LL4EA:
# 2720
{void*_tmp5C5=Cyc_Tcutil_compress(env->return_type);void*_tmp5C6=_tmp5C5;switch(*((int*)_tmp5C6)){case 0: _LL4EE: _LL4EF:
 goto _LL4ED;case 7: _LL4F0: _LL4F1:
 goto _LL4F3;case 6: _LL4F2: _LL4F3:
# 2724
({void*_tmp5C7=0;({unsigned int _tmp882=s->loc;struct _dyneither_ptr _tmp881=({const char*_tmp5C8="break may cause function not to return a value";_tag_dyneither(_tmp5C8,sizeof(char),47);});Cyc_Tcutil_warn(_tmp882,_tmp881,_tag_dyneither(_tmp5C7,sizeof(void*),0));});});
goto _LL4ED;default: _LL4F4: _LL4F5:
# 2727
({void*_tmp5C9=0;({unsigned int _tmp884=s->loc;struct _dyneither_ptr _tmp883=({const char*_tmp5CA="break may cause function not to return a value";_tag_dyneither(_tmp5CA,sizeof(char),47);});Cyc_Tcutil_terr(_tmp884,_tmp883,_tag_dyneither(_tmp5C9,sizeof(void*),0));});});
goto _LL4ED;}_LL4ED:;}
# 2730
goto _LL4E8;}else{_LL4EB: _LL4EC:
 goto _LL4E8;}_LL4E8:;}
# 2733
if(env->noreturn)
({void*_tmp5CB=0;({unsigned int _tmp886=s->loc;struct _dyneither_ptr _tmp885=({const char*_tmp5CC="`noreturn' function might return";_tag_dyneither(_tmp5CC,sizeof(char),33);});Cyc_CfFlowInfo_aerr(_tmp886,_tmp885,_tag_dyneither(_tmp5CB,sizeof(void*),0));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}
# 2739
goto _LL4AE;case 7: _LL4AD: _LL4AE:
 goto _LL4B0;case 8: _LL4AF: _LL4B0: {
# 2743
struct Cyc_Absyn_Stmt*_tmp5CD=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->succ_table,s);
if(_tmp5CD == 0)
({void*_tmp5CE=0;({unsigned int _tmp888=s->loc;struct _dyneither_ptr _tmp887=({const char*_tmp5CF="jump has no target (should be impossible)";_tag_dyneither(_tmp5CF,sizeof(char),42);});Cyc_Tcutil_terr(_tmp888,_tmp887,_tag_dyneither(_tmp5CE,sizeof(void*),0));});});
({union Cyc_CfFlowInfo_FlowInfo _tmp889=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,(struct Cyc_Absyn_Stmt*)_check_null(_tmp5CD));inflow=_tmp889;});
# 2748
Cyc_NewControlFlow_update_flow(env,_tmp5CD,inflow);
return Cyc_CfFlowInfo_BottomFL();}case 10: _LL4B1: _tmp612=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp582)->f1;_tmp611=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp582)->f2;_tmp610=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp582)->f3;_LL4B2:
# 2754
 return Cyc_NewControlFlow_anal_scs(env,inflow,_tmp611,_tmp612->loc);case 15: _LL4B3: _tmp615=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp582)->f1;_tmp614=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp582)->f2;_tmp613=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp582)->f3;_LL4B4: {
# 2759
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp615,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2767
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2770
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp614,0);
{union Cyc_CfFlowInfo_FlowInfo _tmp5D0=scs_outflow;if((_tmp5D0.BottomFL).tag == 1){_LL4F7: _LL4F8:
 goto _LL4F6;}else{_LL4F9: _LL4FA:
({void*_tmp5D1=0;({unsigned int _tmp88B=s->loc;struct _dyneither_ptr _tmp88A=({const char*_tmp5D2="last catch clause may implicitly fallthru";_tag_dyneither(_tmp5D2,sizeof(char),42);});Cyc_CfFlowInfo_aerr(_tmp88B,_tmp88A,_tag_dyneither(_tmp5D1,sizeof(void*),0));});});}_LL4F6:;}
# 2776
outflow=s1_outflow;
# 2778
return outflow;};};}case 12: switch(*((int*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp582)->f1)->r)){case 2: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp582)->f1)->r)->f2 != 0){_LL4B5: _tmp619=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp582)->f1)->r)->f2)->v;_tmp618=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp582)->f1)->r)->f3;_tmp617=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp582)->f1)->loc;_tmp616=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp582)->f2;_LL4B6: {
# 2788
struct _tuple1 _tmp5D3=Cyc_NewControlFlow_get_unconsume_pat_vars((env->fenv)->r,_tmp619);struct _tuple1 _tmp5D4=_tmp5D3;struct Cyc_List_List*_tmp5D9;struct Cyc_List_List*_tmp5D8;_LL4FC: _tmp5D9=_tmp5D4.f1;_tmp5D8=_tmp5D4.f2;_LL4FD:;
({union Cyc_CfFlowInfo_FlowInfo _tmp88C=Cyc_NewControlFlow_initialize_pat_vars(_tmp580,env,inflow,_tmp619,_tmp5D9 != 0,_tmp617,_tmp618->loc);inflow=_tmp88C;});{
struct Cyc_List_List*_tmp5D5=env->unique_pat_vars;
# 2792
({struct Cyc_List_List*_tmp88E=({struct Cyc_List_List*_tmp5D6=_region_malloc(env->r,sizeof(*_tmp5D6));({struct _tuple29*_tmp88D=({struct _tuple29*_tmp5D7=_region_malloc(env->r,sizeof(*_tmp5D7));_tmp5D7->f1=0;_tmp5D7->f2=s;_tmp5D7->f3=_tmp5D9;_tmp5D7->f4=_tmp5D8;_tmp5D7;});_tmp5D6->hd=_tmp88D;});_tmp5D6->tl=_tmp5D5;_tmp5D6;});env->unique_pat_vars=_tmp88E;});
# 2797
({union Cyc_CfFlowInfo_FlowInfo _tmp88F=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp616,rval_opt);inflow=_tmp88F;});
env->unique_pat_vars=_tmp5D5;
# 2801
({union Cyc_CfFlowInfo_FlowInfo _tmp890=Cyc_NewControlFlow_unconsume_params(env,_tmp5D9,_tmp5D8,0,inflow,_tmp617);inflow=_tmp890;});
# 2805
return inflow;};}}else{goto _LL4B9;}case 4: _LL4B7: _tmp61D=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp582)->f1)->r)->f2;_tmp61C=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp582)->f1)->r)->f3;_tmp61B=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp582)->f1)->loc;_tmp61A=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp582)->f2;if(_tmp61C != 0){_LL4B8: {
# 2817
struct Cyc_List_List l=({struct Cyc_List_List _tmp5F7;_tmp5F7.hd=_tmp61C;_tmp5F7.tl=0;_tmp5F7;});
union Cyc_CfFlowInfo_FlowInfo _tmp5DA=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp5DB=Cyc_NewControlFlow_anal_Lexp(env,_tmp5DA,0,0,_tmp61C);struct _tuple18 _tmp5DC=_tmp5DB;union Cyc_CfFlowInfo_AbsLVal _tmp5F6;_LL4FF: _tmp5F6=_tmp5DC.f2;_LL500:;{
struct _tuple16 _tmp5DD=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp5DA,_tmp61C);struct _tuple16 _tmp5DE=_tmp5DD;union Cyc_CfFlowInfo_FlowInfo _tmp5F5;_LL502: _tmp5F5=_tmp5DE.f1;_LL503:;{
struct Cyc_List_List*_tmp5DF=0;
struct Cyc_List_List*_tmp5E0=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp5E1=_tmp5F5;struct Cyc_Dict_Dict _tmp5EF;struct Cyc_List_List*_tmp5EE;if((_tmp5E1.ReachableFL).tag == 2){_LL505: _tmp5EF=((_tmp5E1.ReachableFL).val).f1;_tmp5EE=((_tmp5E1.ReachableFL).val).f2;_LL506:
# 2825
{union Cyc_CfFlowInfo_AbsLVal _tmp5E2=_tmp5F6;struct Cyc_CfFlowInfo_Place*_tmp5ED;if((_tmp5E2.PlaceL).tag == 1){_LL50A: _tmp5ED=(_tmp5E2.PlaceL).val;_LL50B: {
# 2829
void*_tmp5E3=Cyc_CfFlowInfo_lookup_place(_tmp5EF,_tmp5ED);
({void*_tmp892=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5E4=_region_malloc(_tmp580->r,sizeof(*_tmp5E4));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp891=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp5E5;_tmp5E5.tag=8;_tmp5E5.f1=_tmp61D;_tmp5E5.f2=_tmp5E3;_tmp5E5;});_tmp5E4[0]=_tmp891;});_tmp5E4;});_tmp5E3=_tmp892;});
({struct Cyc_Dict_Dict _tmp893=Cyc_CfFlowInfo_assign_place(_tmp580,_tmp61C->loc,_tmp5EF,_tmp5ED,_tmp5E3);_tmp5EF=_tmp893;});
({union Cyc_CfFlowInfo_FlowInfo _tmp894=Cyc_CfFlowInfo_ReachableFL(_tmp5EF,_tmp5EE);_tmp5F5=_tmp894;});{
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp5E6=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp5EB=_region_malloc(_tmp580->r,sizeof(*_tmp5EB));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp895=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp5EC;_tmp5EC.tag=0;_tmp5EC.f1=_tmp61D;_tmp5EC;});_tmp5EB[0]=_tmp895;});_tmp5EB;});
struct Cyc_CfFlowInfo_Place*_tmp5E7=({struct Cyc_CfFlowInfo_Place*_tmp5EA=_region_malloc(_tmp580->r,sizeof(*_tmp5EA));_tmp5EA->root=(void*)_tmp5E6;_tmp5EA->path=0;_tmp5EA;});
({struct Cyc_List_List*_tmp896=({struct Cyc_List_List*_tmp5E8=_region_malloc(_tmp580->r,sizeof(*_tmp5E8));_tmp5E8->hd=_tmp5E7;_tmp5E8->tl=_tmp5DF;_tmp5E8;});_tmp5DF=_tmp896;});
({struct Cyc_List_List*_tmp897=({struct Cyc_List_List*_tmp5E9=_region_malloc(_tmp580->r,sizeof(*_tmp5E9));_tmp5E9->hd=_tmp61C;_tmp5E9->tl=_tmp5E0;_tmp5E9;});_tmp5E0=_tmp897;});
goto _LL509;};}}else{_LL50C: _LL50D:
# 2844
 goto _LL509;}_LL509:;}
# 2846
goto _LL504;}else{_LL507: _LL508:
# 2848
 goto _LL504;}_LL504:;}{
# 2851
struct Cyc_List_List _tmp5F0=({struct Cyc_List_List _tmp5F4;_tmp5F4.hd=_tmp61D;_tmp5F4.tl=0;_tmp5F4;});
({union Cyc_CfFlowInfo_FlowInfo _tmp898=Cyc_NewControlFlow_add_vars(_tmp580,_tmp5F5,& _tmp5F0,_tmp580->unknown_all,_tmp61B,0);_tmp5F5=_tmp898;});{
# 2855
struct Cyc_List_List*_tmp5F1=env->unique_pat_vars;
({struct Cyc_List_List*_tmp89A=({struct Cyc_List_List*_tmp5F2=_region_malloc(env->r,sizeof(*_tmp5F2));({struct _tuple29*_tmp899=({struct _tuple29*_tmp5F3=_region_malloc(env->r,sizeof(*_tmp5F3));_tmp5F3->f1=1;_tmp5F3->f2=s;_tmp5F3->f3=_tmp5DF;_tmp5F3->f4=_tmp5E0;_tmp5F3;});_tmp5F2->hd=_tmp899;});_tmp5F2->tl=_tmp5F1;_tmp5F2;});env->unique_pat_vars=_tmp89A;});
# 2861
({union Cyc_CfFlowInfo_FlowInfo _tmp89B=Cyc_NewControlFlow_anal_stmt(env,_tmp5F5,_tmp61A,rval_opt);_tmp5F5=_tmp89B;});
env->unique_pat_vars=_tmp5F1;
# 2865
({union Cyc_CfFlowInfo_FlowInfo _tmp89C=Cyc_NewControlFlow_unconsume_params(env,_tmp5DF,_tmp5E0,1,_tmp5F5,_tmp61B);_tmp5F5=_tmp89C;});
# 2869
return _tmp5F5;};};};};}}else{goto _LL4B9;}default: _LL4B9: _tmp61F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp582)->f1;_tmp61E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp582)->f2;_LL4BA:
# 2872
({union Cyc_CfFlowInfo_FlowInfo _tmp8A0=({struct Cyc_NewControlFlow_AnalEnv*_tmp89F=env;union Cyc_CfFlowInfo_FlowInfo _tmp89E=Cyc_NewControlFlow_anal_decl(env,inflow,_tmp61F);struct Cyc_Absyn_Stmt*_tmp89D=_tmp61E;Cyc_NewControlFlow_anal_stmt(_tmp89F,_tmp89E,_tmp89D,rval_opt);});outflow=_tmp8A0;});
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp61F);
return outflow;}default: _LL4BB: _tmp620=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp582)->f2;_LL4BC:
# 2877
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp620,rval_opt);}_LL496:;};}
# 2884
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2888
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp623=env->fenv;
void*_tmp624=decl->r;void*_tmp625=_tmp624;struct Cyc_Absyn_Tvar*_tmp63C;struct Cyc_Absyn_Vardecl*_tmp63B;struct Cyc_Absyn_Exp*_tmp63A;struct Cyc_Absyn_Fndecl*_tmp639;struct Cyc_List_List*_tmp638;struct Cyc_Absyn_Vardecl*_tmp637;switch(*((int*)_tmp625)){case 0: _LL50F: _tmp637=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp625)->f1;_LL510: {
# 2898
struct Cyc_List_List _tmp626=({struct Cyc_List_List _tmp62C;_tmp62C.hd=_tmp637;_tmp62C.tl=0;_tmp62C;});
({union Cyc_CfFlowInfo_FlowInfo _tmp8A1=Cyc_NewControlFlow_add_vars(_tmp623,inflow,& _tmp626,_tmp623->unknown_none,decl->loc,0);inflow=_tmp8A1;});{
struct Cyc_Absyn_Exp*_tmp627=_tmp637->initializer;
if(_tmp627 == 0)
return inflow;{
struct _tuple16 _tmp628=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp627);struct _tuple16 _tmp629=_tmp628;union Cyc_CfFlowInfo_FlowInfo _tmp62B;void*_tmp62A;_LL51A: _tmp62B=_tmp629.f1;_tmp62A=_tmp629.f2;_LL51B:;
return Cyc_NewControlFlow_do_initialize_var(_tmp623,env,_tmp62B,_tmp637,_tmp627,_tmp62A,decl->loc);};};}case 3: _LL511: _tmp638=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp625)->f1;_LL512:
# 2907
 return Cyc_NewControlFlow_add_vars(_tmp623,inflow,_tmp638,_tmp623->unknown_none,decl->loc,0);case 1: _LL513: _tmp639=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp625)->f1;_LL514:
# 2910
 Cyc_NewControlFlow_check_nested_fun(env->all_tables,_tmp623,inflow,_tmp639);{
void*t=(void*)_check_null(_tmp639->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp62D=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp639->fn_vardecl);
# 2916
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp8A5=_tmp623;union Cyc_CfFlowInfo_FlowInfo _tmp8A4=inflow;struct Cyc_List_List*_tmp8A3=({struct Cyc_List_List*_tmp62E=_region_malloc(env->r,sizeof(*_tmp62E));_tmp62E->hd=_tmp62D;_tmp62E->tl=0;_tmp62E;});void*_tmp8A2=_tmp623->unknown_all;Cyc_NewControlFlow_add_vars(_tmp8A5,_tmp8A4,_tmp8A3,_tmp8A2,decl->loc,0);});};case 4: _LL515: _tmp63C=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp625)->f1;_tmp63B=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp625)->f2;_tmp63A=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp625)->f3;_LL516:
# 2919
 if(_tmp63A != 0)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp62F=_cycalloc(sizeof(*_tmp62F));({struct Cyc_Core_Impossible_exn_struct _tmp8A7=({struct Cyc_Core_Impossible_exn_struct _tmp630;_tmp630.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp8A6=({const char*_tmp631="found open expression in declaration!";_tag_dyneither(_tmp631,sizeof(char),38);});_tmp630.f1=_tmp8A6;});_tmp630;});_tmp62F[0]=_tmp8A7;});_tmp62F;}));{
struct Cyc_List_List _tmp632=({struct Cyc_List_List _tmp633;_tmp633.hd=_tmp63B;_tmp633.tl=0;_tmp633;});
return Cyc_NewControlFlow_add_vars(_tmp623,inflow,& _tmp632,_tmp623->unknown_all,decl->loc,0);};default: _LL517: _LL518:
# 2925
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp634=_cycalloc(sizeof(*_tmp634));({struct Cyc_Core_Impossible_exn_struct _tmp8A9=({struct Cyc_Core_Impossible_exn_struct _tmp635;_tmp635.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp8A8=({const char*_tmp636="anal_decl: unexpected decl variant";_tag_dyneither(_tmp636,sizeof(char),35);});_tmp635.f1=_tmp8A8;});_tmp635;});_tmp634[0]=_tmp8A9;});_tmp634;}));}_LL50E:;}
# 2933
static void Cyc_NewControlFlow_check_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd){
struct _handler_cons _tmp63D;_push_handler(& _tmp63D);{int _tmp63F=0;if(setjmp(_tmp63D.handler))_tmp63F=1;if(!_tmp63F){
{struct _RegionHandle _tmp640=_new_region("frgn");struct _RegionHandle*frgn=& _tmp640;_push_region(frgn);
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(frgn);
({struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmp8AC=tables;struct Cyc_CfFlowInfo_FlowEnv*_tmp8AB=fenv;union Cyc_CfFlowInfo_FlowInfo _tmp8AA=Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0);Cyc_NewControlFlow_check_nested_fun(_tmp8AC,_tmp8AB,_tmp8AA,fd);});}
# 2936
;_pop_region(frgn);}
# 2935
;_pop_handler();}else{void*_tmp63E=(void*)_exn_thrown;void*_tmp641=_tmp63E;void*_tmp644;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp641)->tag == Cyc_Dict_Absent){_LL51D: _LL51E:
# 2942
 if(Cyc_Position_num_errors > 0)
goto _LL51C;else{
Cyc_Core_rethrow((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp642=_cycalloc_atomic(sizeof(*_tmp642));({struct Cyc_Dict_Absent_exn_struct _tmp8AD=({struct Cyc_Dict_Absent_exn_struct _tmp643;_tmp643.tag=Cyc_Dict_Absent;_tmp643;});_tmp642[0]=_tmp8AD;});_tmp642;}));}}else{_LL51F: _tmp644=_tmp641;_LL520:(int)_rethrow(_tmp644);}_LL51C:;}};}
# 2948
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2953
static union Cyc_Relations_RelnOp Cyc_NewControlFlow_translate_rop(struct Cyc_List_List*vds,union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp645=r;unsigned int _tmp647;if((_tmp645.RParam).tag == 5){_LL522: _tmp647=(_tmp645.RParam).val;_LL523: {
# 2956
struct Cyc_Absyn_Vardecl*_tmp646=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(vds,(int)_tmp647);
if(!_tmp646->escapes)
return Cyc_Relations_RVar(_tmp646);
return r;}}else{_LL524: _LL525:
 return r;}_LL521:;}
# 2964
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2968
struct _RegionHandle*_tmp648=fenv->r;
unsigned int _tmp649=(fd->body)->loc;
({union Cyc_CfFlowInfo_FlowInfo _tmp8AE=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp649,1);inflow=_tmp8AE;});{
# 2974
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_roots=0;
struct _tuple14 _tmp64A=({union Cyc_CfFlowInfo_FlowInfo _tmp682=inflow;if((_tmp682.ReachableFL).tag != 2)_throw_match();(_tmp682.ReachableFL).val;});struct _tuple14 _tmp64B=_tmp64A;struct Cyc_Dict_Dict _tmp681;struct Cyc_List_List*_tmp680;_LL527: _tmp681=_tmp64B.f1;_tmp680=_tmp64B.f2;_LL528:;{
# 2979
struct Cyc_List_List*_tmp64C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;
{struct Cyc_List_List*_tmp64D=fd->requires_relns;for(0;_tmp64D != 0;_tmp64D=_tmp64D->tl){
struct Cyc_Relations_Reln*_tmp64E=(struct Cyc_Relations_Reln*)_tmp64D->hd;
({struct Cyc_List_List*_tmp8B3=({struct _RegionHandle*_tmp8B2=_tmp648;union Cyc_Relations_RelnOp _tmp8B1=Cyc_NewControlFlow_translate_rop(_tmp64C,_tmp64E->rop1);enum Cyc_Relations_Relation _tmp8B0=_tmp64E->relation;union Cyc_Relations_RelnOp _tmp8AF=
Cyc_NewControlFlow_translate_rop(_tmp64C,_tmp64E->rop2);
# 2982
Cyc_Relations_add_relation(_tmp8B2,_tmp8B1,_tmp8B0,_tmp8AF,_tmp680);});_tmp680=_tmp8B3;});}}{
# 2988
struct Cyc_List_List*atts;
{void*_tmp64F=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));void*_tmp650=_tmp64F;struct Cyc_List_List*_tmp653;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp650)->tag == 9){_LL52A: _tmp653=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp650)->f1).attributes;_LL52B:
 atts=_tmp653;goto _LL529;}else{_LL52C: _LL52D:
({void*_tmp651=0;({struct _dyneither_ptr _tmp8B4=({const char*_tmp652="check_fun: non-function type cached with fndecl_t";_tag_dyneither(_tmp652,sizeof(char),50);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp8B4,_tag_dyneither(_tmp651,sizeof(void*),0));});});}_LL529:;}
# 2995
for(0;atts != 0;atts=atts->tl){
void*_tmp654=(void*)atts->hd;void*_tmp655=_tmp654;int _tmp675;int _tmp674;int _tmp673;switch(*((int*)_tmp655)){case 21: _LL52F: _tmp673=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp655)->f1;_LL530: {
# 2998
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
struct Cyc_Absyn_Vardecl*_tmp656=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp673 - 1);
void*t=Cyc_Tcutil_compress(_tmp656->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp657=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp8B7=fenv;void*_tmp8B6=elttype;struct Cyc_Absyn_Exp*_tmp8B5=bogus_exp;Cyc_CfFlowInfo_make_unique_consumed(_tmp8B7,_tmp8B6,_tmp8B5,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));});
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp658=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp65F=_region_malloc(_tmp648,sizeof(*_tmp65F));({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp8B8=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp660;_tmp660.tag=2;_tmp660.f1=_tmp673;_tmp660.f2=t;_tmp660;});_tmp65F[0]=_tmp8B8;});_tmp65F;});
struct Cyc_CfFlowInfo_Place*_tmp659=({struct Cyc_CfFlowInfo_Place*_tmp65E=_region_malloc(_tmp648,sizeof(*_tmp65E));_tmp65E->root=(void*)_tmp658;_tmp65E->path=0;_tmp65E;});
({struct Cyc_Dict_Dict _tmp8B9=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp681,(void*)_tmp658,_tmp657);_tmp681=_tmp8B9;});
({struct Cyc_Dict_Dict _tmp8BE=({struct Cyc_Dict_Dict _tmp8BD=_tmp681;void*_tmp8BC=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp65A=_region_malloc(_tmp648,sizeof(*_tmp65A));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8BA=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp65B;_tmp65B.tag=0;_tmp65B.f1=_tmp656;_tmp65B;});_tmp65A[0]=_tmp8BA;});_tmp65A;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp8BD,_tmp8BC,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp65C=_region_malloc(_tmp648,sizeof(*_tmp65C));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp8BB=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp65D;_tmp65D.tag=4;_tmp65D.f1=_tmp659;_tmp65D;});_tmp65C[0]=_tmp8BB;});_tmp65C;}));});_tmp681=_tmp8BE;});
goto _LL52E;}case 20: _LL531: _tmp674=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp655)->f1;_LL532: {
# 3011
struct Cyc_Absyn_Vardecl*_tmp661=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp674 - 1);
void*elttype=Cyc_Tcutil_pointer_elt_type(_tmp661->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp662=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp66A=_region_malloc(_tmp648,sizeof(*_tmp66A));({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp8BF=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp66B;_tmp66B.tag=2;_tmp66B.f1=_tmp674;_tmp66B.f2=elttype;_tmp66B;});_tmp66A[0]=_tmp8BF;});_tmp66A;});
struct Cyc_CfFlowInfo_Place*_tmp663=({struct Cyc_CfFlowInfo_Place*_tmp669=_region_malloc(_tmp648,sizeof(*_tmp669));_tmp669->root=(void*)_tmp662;_tmp669->path=0;_tmp669;});
({struct Cyc_Dict_Dict _tmp8C2=({struct Cyc_Dict_Dict _tmp8C1=_tmp681;void*_tmp8C0=(void*)_tmp662;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp8C1,_tmp8C0,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));});_tmp681=_tmp8C2;});
({struct Cyc_Dict_Dict _tmp8C7=({struct Cyc_Dict_Dict _tmp8C6=_tmp681;void*_tmp8C5=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp664=_region_malloc(_tmp648,sizeof(*_tmp664));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8C3=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp665;_tmp665.tag=0;_tmp665.f1=_tmp661;_tmp665;});_tmp664[0]=_tmp8C3;});_tmp664;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp8C6,_tmp8C5,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp666=_region_malloc(_tmp648,sizeof(*_tmp666));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp8C4=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp667;_tmp667.tag=4;_tmp667.f1=_tmp663;_tmp667;});_tmp666[0]=_tmp8C4;});_tmp666;}));});_tmp681=_tmp8C7;});
({struct Cyc_List_List*_tmp8C8=({struct Cyc_List_List*_tmp668=_region_malloc(_tmp648,sizeof(*_tmp668));_tmp668->hd=_tmp663;_tmp668->tl=param_roots;_tmp668;});param_roots=_tmp8C8;});
goto _LL52E;}case 22: _LL533: _tmp675=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp655)->f1;_LL534: {
# 3020
struct Cyc_Absyn_Vardecl*_tmp66C=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp675 - 1);
if(Cyc_Tcutil_is_noalias_pointer(_tmp66C->type,0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp66D=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp671=_region_malloc(_tmp648,sizeof(*_tmp671));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8C9=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp672;_tmp672.tag=0;_tmp672.f1=_tmp66C;_tmp672;});_tmp671[0]=_tmp8C9;});_tmp671;});
struct Cyc_CfFlowInfo_Place*_tmp66E=({struct Cyc_CfFlowInfo_Place*_tmp670=_region_malloc(_tmp648,sizeof(*_tmp670));_tmp670->root=(void*)_tmp66D;_tmp670->path=0;_tmp670;});
({struct Cyc_List_List*_tmp8CA=({struct Cyc_List_List*_tmp66F=_region_malloc(_tmp648,sizeof(*_tmp66F));_tmp66F->hd=_tmp66E;_tmp66F->tl=noconsume_roots;_tmp66F;});noconsume_roots=_tmp8CA;});}
# 3026
goto _LL52E;}default: _LL535: _LL536:
 goto _LL52E;}_LL52E:;}
# 3029
({union Cyc_CfFlowInfo_FlowInfo _tmp8CB=Cyc_CfFlowInfo_ReachableFL(_tmp681,_tmp680);inflow=_tmp8CB;});{
# 3031
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp648,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*_tmp67F=_region_malloc(_tmp648,sizeof(*_tmp67F));_tmp67F->all_tables=tables;({struct Cyc_Hashtable_Table*_tmp8CC=
# 3036
((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->succ_tables,fd);_tmp67F->succ_table=_tmp8CC;});({struct Cyc_Hashtable_Table*_tmp8CD=
((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->pat_pop_tables,fd);_tmp67F->pat_pop_table=_tmp8CD;});_tmp67F->r=_tmp648;_tmp67F->fenv=fenv;_tmp67F->iterate_again=1;_tmp67F->iteration_num=0;_tmp67F->in_try=0;_tmp67F->tryflow=inflow;_tmp67F->noreturn=noreturn;_tmp67F->return_type=fd->ret_type;_tmp67F->unique_pat_vars=0;_tmp67F->param_roots=param_roots;_tmp67F->noconsume_params=noconsume_roots;_tmp67F->flow_table=flow_table;_tmp67F->return_relns=fd->ensures_relns;_tmp67F;});
# 3041
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 3047
env->iterate_again=0;
({union Cyc_CfFlowInfo_FlowInfo _tmp8CE=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);outflow=_tmp8CE;});}{
# 3050
union Cyc_CfFlowInfo_FlowInfo _tmp676=outflow;if((_tmp676.BottomFL).tag == 1){_LL538: _LL539:
 goto _LL537;}else{_LL53A: _LL53B:
# 3053
 Cyc_NewControlFlow_check_init_params(_tmp649,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,outflow,_tmp649);
# 3057
if(noreturn)
({void*_tmp677=0;({unsigned int _tmp8D0=_tmp649;struct _dyneither_ptr _tmp8CF=({const char*_tmp678="`noreturn' function might (implicitly) return";_tag_dyneither(_tmp678,sizeof(char),46);});Cyc_CfFlowInfo_aerr(_tmp8D0,_tmp8CF,_tag_dyneither(_tmp677,sizeof(void*),0));});});else{
# 3060
void*_tmp679=Cyc_Tcutil_compress(fd->ret_type);void*_tmp67A=_tmp679;switch(*((int*)_tmp67A)){case 0: _LL53D: _LL53E:
 goto _LL53C;case 7: _LL53F: _LL540:
 goto _LL542;case 6: _LL541: _LL542:
# 3064
({void*_tmp67B=0;({unsigned int _tmp8D2=_tmp649;struct _dyneither_ptr _tmp8D1=({const char*_tmp67C="function may not return a value";_tag_dyneither(_tmp67C,sizeof(char),32);});Cyc_Tcutil_warn(_tmp8D2,_tmp8D1,_tag_dyneither(_tmp67B,sizeof(void*),0));});});goto _LL53C;default: _LL543: _LL544:
# 3066
({void*_tmp67D=0;({unsigned int _tmp8D4=_tmp649;struct _dyneither_ptr _tmp8D3=({const char*_tmp67E="function may not return a value";_tag_dyneither(_tmp67E,sizeof(char),32);});Cyc_CfFlowInfo_aerr(_tmp8D4,_tmp8D3,_tag_dyneither(_tmp67D,sizeof(void*),0));});});goto _LL53C;}_LL53C:;}
# 3068
goto _LL537;}_LL537:;};};};};};}
# 3072
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_tmp683=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_tmp684=_tmp683;struct Cyc_List_List*_tmp688;struct Cyc_List_List*_tmp687;struct Cyc_List_List*_tmp686;struct Cyc_Absyn_Fndecl*_tmp685;switch(*((int*)_tmp684)){case 1: _LL546: _tmp685=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp684)->f1;_LL547:
 Cyc_NewControlFlow_check_fun(tables,_tmp685);goto _LL545;case 11: _LL548: _tmp686=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp684)->f1;_LL549:
 _tmp687=_tmp686;goto _LL54B;case 10: _LL54A: _tmp687=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp684)->f2;_LL54B:
 _tmp688=_tmp687;goto _LL54D;case 9: _LL54C: _tmp688=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp684)->f2;_LL54D:
 Cyc_NewControlFlow_cf_check(tables,_tmp688);goto _LL545;case 12: _LL54E: _LL54F:
 goto _LL545;default: _LL550: _LL551:
 goto _LL545;}_LL545:;};}}
