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
({void*_tmp64D=(void*)({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp2=_cycalloc_atomic(sizeof(*_tmp2));({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp64C=({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp3;_tmp3.tag=Cyc_NewControlFlow_CFAnnot;({struct Cyc_NewControlFlow_CFStmtAnnot _tmp64B=({struct Cyc_NewControlFlow_CFStmtAnnot _tmp4;_tmp4.visited=0;_tmp4;});_tmp3.f1=_tmp64B;});_tmp3;});_tmp2[0]=_tmp64C;});_tmp2;});s->annot=_tmp64D;});return Cyc_NewControlFlow_get_stmt_annot(s);}_LL0:;}
# 142
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*res=({union Cyc_CfFlowInfo_FlowInfo*_tmp6=_cycalloc(sizeof(*_tmp6));({union Cyc_CfFlowInfo_FlowInfo _tmp64E=Cyc_CfFlowInfo_BottomFL();_tmp6[0]=_tmp64E;});_tmp6;});
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);
return res;}
# 149
return*sflow;}struct _tuple20{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 152
static struct _tuple20 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp7=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp8=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 160
({union Cyc_CfFlowInfo_FlowInfo _tmp64F=Cyc_CfFlowInfo_join_flow(env->fenv,inflow,*_tmp8);*_tmp8=_tmp64F;});
# 166
_tmp7->visited=env->iteration_num;
return({struct _tuple20 _tmp9;_tmp9.f1=_tmp7;_tmp9.f2=_tmp8;_tmp9;});}
# 176
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 184
({union Cyc_CfFlowInfo_FlowInfo _tmp650=Cyc_CfFlowInfo_join_flow(env->fenv,new_flow,env->tryflow);env->tryflow=_tmp650;});}struct _tuple21{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned int f2;struct Cyc_Dict_Dict f3;};
# 191
static void Cyc_NewControlFlow_check_unique_root(struct _tuple21*ckenv,void*root,void*rval){
# 193
struct _tuple21*_tmpA=ckenv;struct Cyc_NewControlFlow_AnalEnv*_tmp14;unsigned int _tmp13;struct Cyc_Dict_Dict _tmp12;_LL1: _tmp14=_tmpA->f1;_tmp13=_tmpA->f2;_tmp12=_tmpA->f3;_LL2:;{
void*_tmpB=root;struct Cyc_Absyn_Vardecl*_tmp11;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpB)->tag == 0U){_LL4: _tmp11=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpB)->f1;_LL5:
# 196
 if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(_tmp12,root,& rval) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp11->type)){
retry: {void*_tmpC=rval;void*_tmp10;switch(*((int*)_tmpC)){case 8U: _LL9: _tmp10=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpC)->f2;_LLA:
 rval=_tmp10;goto retry;case 7U: _LLB: _LLC:
 goto _LLE;case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpC)->f1 == Cyc_CfFlowInfo_NoneIL){_LLD: _LLE:
 goto _LL10;}else{goto _LL11;}case 0U: _LLF: _LL10:
 goto _LL8;default: _LL11: _LL12:
# 205
({struct Cyc_String_pa_PrintArg_struct _tmpF;_tmpF.tag=0U;({struct _dyneither_ptr _tmp651=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp11->name));_tmpF.f1=_tmp651;});({void*_tmpD[1U]={& _tmpF};({unsigned int _tmp653=_tmp13;struct _dyneither_ptr _tmp652=({const char*_tmpE="alias-free pointer(s) reachable from %s may become unreachable.";_tag_dyneither(_tmpE,sizeof(char),64U);});Cyc_Warn_warn(_tmp653,_tmp652,_tag_dyneither(_tmpD,sizeof(void*),1U));});});});
goto _LL8;}_LL8:;}}
# 209
goto _LL3;}else{_LL6: _LL7:
 goto _LL3;}_LL3:;};}
# 217
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp15=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp16=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp17=Cyc_CfFlowInfo_join_flow(env->fenv,flow,*_tmp16);
# 223
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple19 _tmp18=({struct _tuple19 _tmp1E;_tmp1E.f1=flow;_tmp1E.f2=_tmp17;_tmp1E;});struct _tuple19 _tmp19=_tmp18;struct Cyc_Dict_Dict _tmp1D;struct Cyc_Dict_Dict _tmp1C;if(((_tmp19.f1).ReachableFL).tag == 2){if(((_tmp19.f2).ReachableFL).tag == 2){_LL1: _tmp1D=(((_tmp19.f1).ReachableFL).val).f1;_tmp1C=(((_tmp19.f2).ReachableFL).val).f1;_LL2: {
# 226
struct _tuple21 _tmp1A=({struct _tuple21 _tmp1B;_tmp1B.f1=env;_tmp1B.f2=s->loc;_tmp1B.f3=_tmp1C;_tmp1B;});
((void(*)(void(*f)(struct _tuple21*,void*,void*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& _tmp1A,_tmp1D);
goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 232
if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp17,*_tmp16)){
# 239
*_tmp16=_tmp17;
# 243
if(_tmp15->visited == env->iteration_num)
# 245
env->iterate_again=1;}}
# 250
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned int loc,int nameit){
# 254
union Cyc_CfFlowInfo_FlowInfo _tmp1F=inflow;struct Cyc_Dict_Dict _tmp27;struct Cyc_List_List*_tmp26;if((_tmp1F.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmp27=((_tmp1F.ReachableFL).val).f1;_tmp26=((_tmp1F.ReachableFL).val).f2;_LL4:
# 257
 for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp20=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp24=_cycalloc(sizeof(*_tmp24));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp654=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp25;_tmp25.tag=0U;_tmp25.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp25;});_tmp24[0]=_tmp654;});_tmp24;});
void*_tmp21=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit)
({void*_tmp656=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp22=_cycalloc(sizeof(*_tmp22));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp655=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp23;_tmp23.tag=8U;_tmp23.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp23.f2=_tmp21;_tmp23;});_tmp22[0]=_tmp655;});_tmp22;});_tmp21=_tmp656;});
# 264
({struct Cyc_Dict_Dict _tmp657=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp27,(void*)_tmp20,_tmp21);_tmp27=_tmp657;});}
# 266
return Cyc_CfFlowInfo_ReachableFL(_tmp27,_tmp26);}_LL0:;}
# 270
static int Cyc_NewControlFlow_relns_ok(struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 277
for(0;(unsigned int)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp28=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)req->hd);
if(Cyc_Relations_consistent_relations(({struct Cyc_List_List*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->hd=_tmp28;_tmp29->tl=assume;_tmp29;})))
return 0;}
# 282
return 1;}
# 285
static struct Cyc_Absyn_Exp*Cyc_NewControlFlow_strip_cast(struct Cyc_Absyn_Exp*e){
void*_tmp2A=e->r;void*_tmp2B=_tmp2A;struct Cyc_Absyn_Exp*_tmp2C;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2B)->tag == 14U){_LL1: _tmp2C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2B)->f2;_LL2:
 return _tmp2C;}else{_LL3: _LL4:
 return e;}_LL0:;}
# 292
static void Cyc_NewControlFlow_check_union_requires(unsigned int loc,void*t,struct _dyneither_ptr*f,union Cyc_CfFlowInfo_FlowInfo inflow){
# 294
union Cyc_CfFlowInfo_FlowInfo _tmp2D=inflow;struct Cyc_List_List*_tmp4B;if((_tmp2D.BottomFL).tag == 1){_LL1: _LL2:
 return;}else{_LL3: _tmp4B=((_tmp2D.ReachableFL).val).f2;_LL4:
# 297
{void*_tmp2E=Cyc_Tcutil_compress(t);void*_tmp2F=_tmp2E;struct Cyc_List_List*_tmp4A;union Cyc_Absyn_AggrInfoU _tmp49;struct Cyc_List_List*_tmp48;switch(*((int*)_tmp2F)){case 11U: _LL6: _tmp49=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F)->f1).aggr_info;_tmp48=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F)->f1).targs;_LL7: {
# 299
struct Cyc_Absyn_Aggrdecl*_tmp30=Cyc_Absyn_get_known_aggrdecl(_tmp49);
struct Cyc_Absyn_Aggrfield*_tmp31=Cyc_Absyn_lookup_decl_field(_tmp30,f);
struct Cyc_Absyn_Exp*_tmp32=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp31))->requires_clause;
if(_tmp32 != 0){
struct _RegionHandle _tmp33=_new_region("temp");struct _RegionHandle*temp=& _tmp33;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp34=({struct _RegionHandle*_tmp659=temp;struct Cyc_List_List*_tmp658=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp30->tvs,_tmp48);Cyc_Tcutil_rsubsexp(_tmp659,_tmp658,_tmp32);});
# 306
if(!({struct Cyc_List_List*_tmp65A=_tmp4B;Cyc_NewControlFlow_relns_ok(_tmp65A,Cyc_Relations_exp2relns(temp,_tmp34));})){
({struct Cyc_String_pa_PrintArg_struct _tmp38;_tmp38.tag=0U;_tmp38.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp37;_tmp37.tag=0U;({struct _dyneither_ptr _tmp65B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp34)));_tmp37.f1=_tmp65B;});({void*_tmp35[2U]={& _tmp37,& _tmp38};({unsigned int _tmp65D=loc;struct _dyneither_ptr _tmp65C=({const char*_tmp36="unable to prove %s, required to access %s";_tag_dyneither(_tmp36,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp65D,_tmp65C,_tag_dyneither(_tmp35,sizeof(void*),2U));});});});});
({void*_tmp39=0U;({struct Cyc___cycFILE*_tmp65F=Cyc_stderr;struct _dyneither_ptr _tmp65E=({const char*_tmp3A="  [recorded facts on path: ";_tag_dyneither(_tmp3A,sizeof(char),28U);});Cyc_fprintf(_tmp65F,_tmp65E,_tag_dyneither(_tmp39,sizeof(void*),0U));});});
Cyc_Relations_print_relns(Cyc_stderr,_tmp4B);
({void*_tmp3B=0U;({struct Cyc___cycFILE*_tmp661=Cyc_stderr;struct _dyneither_ptr _tmp660=({const char*_tmp3C="]\n";_tag_dyneither(_tmp3C,sizeof(char),3U);});Cyc_fprintf(_tmp661,_tmp660,_tag_dyneither(_tmp3B,sizeof(void*),0U));});});}}
# 304
;_pop_region(temp);}
# 313
goto _LL5;}case 12U: _LL8: _tmp4A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2F)->f2;_LL9: {
# 315
struct Cyc_Absyn_Aggrfield*_tmp3D=Cyc_Absyn_lookup_field(_tmp4A,f);
struct Cyc_Absyn_Exp*_tmp3E=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3D))->requires_clause;
if(_tmp3E != 0){
struct _RegionHandle _tmp3F=_new_region("temp");struct _RegionHandle*temp=& _tmp3F;_push_region(temp);
if(!({struct Cyc_List_List*_tmp662=_tmp4B;Cyc_NewControlFlow_relns_ok(_tmp662,Cyc_Relations_exp2relns(temp,_tmp3E));})){
({struct Cyc_String_pa_PrintArg_struct _tmp43;_tmp43.tag=0U;_tmp43.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp42;_tmp42.tag=0U;({struct _dyneither_ptr _tmp663=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp3E)));_tmp42.f1=_tmp663;});({void*_tmp40[2U]={& _tmp42,& _tmp43};({unsigned int _tmp665=loc;struct _dyneither_ptr _tmp664=({const char*_tmp41="unable to prove %s, required to access %s";_tag_dyneither(_tmp41,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp665,_tmp664,_tag_dyneither(_tmp40,sizeof(void*),2U));});});});});
({void*_tmp44=0U;({struct Cyc___cycFILE*_tmp667=Cyc_stderr;struct _dyneither_ptr _tmp666=({const char*_tmp45="  [recorded facts on path: ";_tag_dyneither(_tmp45,sizeof(char),28U);});Cyc_fprintf(_tmp667,_tmp666,_tag_dyneither(_tmp44,sizeof(void*),0U));});});
Cyc_Relations_print_relns(Cyc_stderr,_tmp4B);
({void*_tmp46=0U;({struct Cyc___cycFILE*_tmp669=Cyc_stderr;struct _dyneither_ptr _tmp668=({const char*_tmp47="]\n";_tag_dyneither(_tmp47,sizeof(char),3U);});Cyc_fprintf(_tmp669,_tmp668,_tag_dyneither(_tmp46,sizeof(void*),0U));});});}
# 319
;_pop_region(temp);}
# 326
goto _LL5;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 329
goto _LL0;}_LL0:;}
# 333
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp4C=inflow;struct Cyc_Dict_Dict _tmp52;struct Cyc_List_List*_tmp51;if((_tmp4C.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmp52=((_tmp4C.ReachableFL).val).f1;_tmp51=((_tmp4C.ReachableFL).val).f2;_LL4:
# 337
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp52,r)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp4D=0U;({unsigned int _tmp66B=loc;struct _dyneither_ptr _tmp66A=({const char*_tmp4E="expression may not be fully initialized";_tag_dyneither(_tmp4E,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp66B,_tmp66A,_tag_dyneither(_tmp4D,sizeof(void*),0U));});});{
struct Cyc_Dict_Dict _tmp4F=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp52,r);
if(_tmp52.t == _tmp4F.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp50=Cyc_CfFlowInfo_ReachableFL(_tmp4F,_tmp51);
Cyc_NewControlFlow_update_tryflow(env,_tmp50);
return _tmp50;};};}_LL0:;}struct _tuple22{struct Cyc_Absyn_Tqual f1;void*f2;};
# 347
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp53=({struct _tuple0 _tmp66;({void*_tmp66C=Cyc_Tcutil_compress(t);_tmp66.f1=_tmp66C;});_tmp66.f2=r;_tmp66;});struct _tuple0 _tmp54=_tmp53;enum Cyc_Absyn_AggrKind _tmp65;struct Cyc_List_List*_tmp64;struct _dyneither_ptr _tmp63;union Cyc_Absyn_AggrInfoU _tmp62;struct Cyc_List_List*_tmp61;struct _dyneither_ptr _tmp60;struct Cyc_List_List*_tmp5F;struct _dyneither_ptr _tmp5E;struct Cyc_Absyn_Datatypefield*_tmp5D;struct _dyneither_ptr _tmp5C;void*_tmp5B;switch(*((int*)_tmp54.f2)){case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f1 == Cyc_CfFlowInfo_NoneIL){_LL1: _LL2:
 return;}else{switch(*((int*)_tmp54.f1)){case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp54.f1)->f1).field_info).KnownDatatypefield).tag == 2)goto _LL13;else{goto _LL13;}case 10U: goto _LL13;case 11U: goto _LL13;case 12U: goto _LL13;case 5U: goto _LL11;default: goto _LL13;}}case 0U: _LL3: _LL4:
 return;case 7U: _LL5: _LL6:
 return;case 8U: _LL7: _tmp5B=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f2;_LL8:
 Cyc_NewControlFlow_check_nounique(env,loc,t,_tmp5B);return;default: switch(*((int*)_tmp54.f1)){case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp54.f1)->f1).field_info).KnownDatatypefield).tag == 2){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->tag == 6U){_LL9: _tmp5D=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp54.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp5C=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f2;_LLA:
# 354
 if(_tmp5D->typs == 0)
return;
_tmp5F=_tmp5D->typs;_tmp5E=_tmp5C;goto _LLC;}else{goto _LL13;}}else{goto _LL13;}case 10U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->tag == 6U){_LLB: _tmp5F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp54.f1)->f1;_tmp5E=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f2;_LLC: {
# 358
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5F);
{int i=0;for(0;i < sz;(i ++,_tmp5F=_tmp5F->tl)){
({struct Cyc_NewControlFlow_AnalEnv*_tmp66F=env;unsigned int _tmp66E=loc;void*_tmp66D=(*((struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp5F))->hd)).f2;Cyc_NewControlFlow_check_nounique(_tmp66F,_tmp66E,_tmp66D,*((void**)_check_dyneither_subscript(_tmp5E,sizeof(void*),i)));});}}
# 362
return;}}else{goto _LL13;}case 11U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->tag == 6U){_LLD: _tmp62=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp54.f1)->f1).aggr_info;_tmp61=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp54.f1)->f1).targs;_tmp60=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f2;_LLE: {
# 365
struct Cyc_Absyn_Aggrdecl*_tmp55=Cyc_Absyn_get_known_aggrdecl(_tmp62);
if(_tmp55->impl == 0)return;{
struct Cyc_List_List*_tmp56=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp55->impl))->fields;
struct _RegionHandle _tmp57=_new_region("temp");struct _RegionHandle*temp=& _tmp57;_push_region(temp);
{struct Cyc_List_List*_tmp58=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp55->tvs,_tmp61);
{int i=0;for(0;i < _get_dyneither_size(_tmp60,sizeof(void*));(i ++,_tmp56=_tmp56->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp56))->hd)->type;
if(_tmp58 != 0)({void*_tmp670=Cyc_Tcutil_rsubstitute(temp,_tmp58,t);t=_tmp670;});
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)_tmp60.curr)[i]);}}
# 375
_npop_handler(0U);return;}
# 369
;_pop_region(temp);};}}else{goto _LL13;}case 12U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->tag == 6U){_LLF: _tmp65=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp54.f1)->f1;_tmp64=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp54.f1)->f2;_tmp63=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f2;_LL10:
# 378
{int i=0;for(0;i < _get_dyneither_size(_tmp63,sizeof(void*));(i ++,_tmp64=_tmp64->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp64))->hd)->type,((void**)_tmp63.curr)[i]);}}
return;}else{goto _LL13;}case 5U: _LL11: _LL12:
# 382
 if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp59=0U;({unsigned int _tmp672=loc;struct _dyneither_ptr _tmp671=({const char*_tmp5A="argument may still contain alias-free pointers";_tag_dyneither(_tmp5A,sizeof(char),47U);});Cyc_Warn_warn(_tmp672,_tmp671,_tag_dyneither(_tmp59,sizeof(void*),0U));});});
return;default: _LL13: _LL14:
 return;}}_LL0:;}
# 389
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp67=inflow;struct Cyc_Dict_Dict _tmp74;struct Cyc_List_List*_tmp73;if((_tmp67.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmp74=((_tmp67.ReachableFL).val).f1;_tmp73=((_tmp67.ReachableFL).val).f2;_LL4:
# 393
 if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
({void*_tmp68=0U;({struct _dyneither_ptr _tmp673=({const char*_tmp69="noliveunique attribute requires alias-free pointer";_tag_dyneither(_tmp69,sizeof(char),51U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp673,_tag_dyneither(_tmp68,sizeof(void*),0U));});});
return Cyc_CfFlowInfo_BottomFL();}{
# 397
void*_tmp6A=Cyc_Tcutil_pointer_elt_type(t);
retry: {void*_tmp6B=r;void*_tmp70;struct Cyc_CfFlowInfo_Place*_tmp6F;void*_tmp6E;switch(*((int*)_tmp6B)){case 8U: _LL6: _tmp6E=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp6B)->f2;_LL7:
 r=_tmp6E;goto retry;case 4U: _LL8: _tmp6F=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp6B)->f1;_LL9:
({void*_tmp674=Cyc_CfFlowInfo_lookup_place(_tmp74,_tmp6F);_tmp70=_tmp674;});goto _LLB;case 5U: _LLA: _tmp70=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp6B)->f1;_LLB:
 Cyc_NewControlFlow_check_nounique(env,loc,_tmp6A,_tmp70);goto _LL5;default: _LLC: _LLD:
# 403
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp6A))
({void*_tmp6C=0U;({unsigned int _tmp676=loc;struct _dyneither_ptr _tmp675=({const char*_tmp6D="argument may contain live alias-free pointers";_tag_dyneither(_tmp6D,sizeof(char),46U);});Cyc_Warn_warn(_tmp676,_tmp675,_tag_dyneither(_tmp6C,sizeof(void*),0U));});});
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);}_LL5:;}{
# 407
struct Cyc_Dict_Dict _tmp71=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp74,r);
if(_tmp74.t == _tmp71.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp72=Cyc_CfFlowInfo_ReachableFL(_tmp71,_tmp73);
Cyc_NewControlFlow_update_tryflow(env,_tmp72);
return _tmp72;};};};}_LL0:;}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 417
static struct _tuple23 Cyc_NewControlFlow_anal_Rexps(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 422
struct Cyc_List_List*rvals=0;
if(es == 0)
return({struct _tuple23 _tmp75;_tmp75.f1=inflow;_tmp75.f2=0;_tmp75;});
# 427
do{
struct _tuple16 _tmp76=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);struct _tuple16 _tmp77=_tmp76;union Cyc_CfFlowInfo_FlowInfo _tmp7A;void*_tmp79;_LL1: _tmp7A=_tmp77.f1;_tmp79=_tmp77.f2;_LL2:;
inflow=_tmp7A;
({struct Cyc_List_List*_tmp677=({struct Cyc_List_List*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->hd=_tmp79;_tmp78->tl=rvals;_tmp78;});rvals=_tmp677;});
es=es->tl;
first_is_copy=others_are_copy;}while(es != 0);
# 435
Cyc_NewControlFlow_update_tryflow(env,inflow);
return({struct _tuple23 _tmp7B;_tmp7B.f1=inflow;({struct Cyc_List_List*_tmp678=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals);_tmp7B.f2=_tmp678;});_tmp7B;});}
# 441
static struct _tuple16 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 443
struct _tuple23 _tmp7C=
Cyc_NewControlFlow_anal_Rexps(env,inflow,es,0,0);
# 443
struct _tuple23 _tmp7D=_tmp7C;union Cyc_CfFlowInfo_FlowInfo _tmp84;struct Cyc_List_List*_tmp83;_LL1: _tmp84=_tmp7D.f1;_tmp83=_tmp7D.f2;_LL2:;
# 445
{union Cyc_CfFlowInfo_FlowInfo _tmp7E=_tmp84;struct Cyc_Dict_Dict _tmp81;if((_tmp7E.ReachableFL).tag == 2){_LL4: _tmp81=((_tmp7E.ReachableFL).val).f1;_LL5:
# 447
 for(0;_tmp83 != 0;(_tmp83=_tmp83->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp81,(void*)_tmp83->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp7F=0U;({unsigned int _tmp67A=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _dyneither_ptr _tmp679=({const char*_tmp80="expression may not be initialized";_tag_dyneither(_tmp80,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp67A,_tmp679,_tag_dyneither(_tmp7F,sizeof(void*),0U));});});}
goto _LL3;}else{_LL6: _LL7:
 goto _LL3;}_LL3:;}
# 453
return({struct _tuple16 _tmp82;_tmp82.f1=_tmp84;_tmp82.f2=(env->fenv)->unknown_all;_tmp82;});}
# 460
static void*Cyc_NewControlFlow_consume_zero_rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Dict_Dict new_dict,struct Cyc_CfFlowInfo_Place*p,struct Cyc_Absyn_Exp*e,void*new_rval){
# 467
int needs_unconsume=0;
void*_tmp85=Cyc_CfFlowInfo_lookup_place(new_dict,p);
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp85,& needs_unconsume))
({void*_tmp86=0U;({struct _dyneither_ptr _tmp67B=({const char*_tmp87="consume_zero_rval";_tag_dyneither(_tmp87,sizeof(char),18U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp67B,_tag_dyneither(_tmp86,sizeof(void*),0U));});});else{
# 472
if(needs_unconsume)
return Cyc_CfFlowInfo_make_unique_consumed(env->fenv,(void*)_check_null(e->topt),e,env->iteration_num,new_rval);else{
# 476
return new_rval;}}}
# 489 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 494
union Cyc_CfFlowInfo_FlowInfo _tmp88=outflow;struct Cyc_Dict_Dict _tmp90;struct Cyc_List_List*_tmp8F;if((_tmp88.BottomFL).tag == 1){_LL1: _LL2:
 return outflow;}else{_LL3: _tmp90=((_tmp88.ReachableFL).val).f1;_tmp8F=((_tmp88.ReachableFL).val).f2;_LL4: {
# 497
union Cyc_CfFlowInfo_AbsLVal _tmp89=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmp8A=_tmp89;struct Cyc_CfFlowInfo_Place*_tmp8E;if((_tmp8A.UnknownL).tag == 2){_LL6: _LL7:
# 501
 return outflow;}else{_LL8: _tmp8E=(_tmp8A.PlaceL).val;_LL9: {
# 505
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->unknown_none;
for(0;names != 0;names=names->tl){
({void*_tmp67D=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp67C=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp8C;_tmp8C.tag=8U;_tmp8C.f1=(struct Cyc_Absyn_Vardecl*)names->hd;_tmp8C.f2=nzval;_tmp8C;});_tmp8B[0]=_tmp67C;});_tmp8B;});nzval=_tmp67D;});}
# 509
({void*_tmp67E=Cyc_NewControlFlow_consume_zero_rval(env,_tmp90,_tmp8E,e,nzval);nzval=_tmp67E;});{
union Cyc_CfFlowInfo_FlowInfo _tmp8D=({struct Cyc_Dict_Dict _tmp67F=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp90,_tmp8E,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp67F,_tmp8F);});
# 514
return _tmp8D;};}}_LL5:;}}_LL0:;}
# 523
static struct _tuple19 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 525
union Cyc_CfFlowInfo_FlowInfo _tmp91=outflow;struct Cyc_Dict_Dict _tmp9D;struct Cyc_List_List*_tmp9C;if((_tmp91.BottomFL).tag == 1){_LL1: _LL2:
 return({struct _tuple19 _tmp92;_tmp92.f1=outflow;_tmp92.f2=outflow;_tmp92;});}else{_LL3: _tmp9D=((_tmp91.ReachableFL).val).f1;_tmp9C=((_tmp91.ReachableFL).val).f2;_LL4: {
# 528
union Cyc_CfFlowInfo_AbsLVal _tmp93=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmp94=_tmp93;struct Cyc_CfFlowInfo_Place*_tmp9B;if((_tmp94.UnknownL).tag == 2){_LL6: _LL7:
 return({struct _tuple19 _tmp95;_tmp95.f1=outflow;_tmp95.f2=outflow;_tmp95;});}else{_LL8: _tmp9B=(_tmp94.PlaceL).val;_LL9: {
# 531
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->unknown_none;
void*zval=(env->fenv)->zero;
for(0;names != 0;names=names->tl){
({void*_tmp681=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp96=_cycalloc(sizeof(*_tmp96));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp680=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp97;_tmp97.tag=8U;_tmp97.f1=(struct Cyc_Absyn_Vardecl*)names->hd;_tmp97.f2=nzval;_tmp97;});_tmp96[0]=_tmp680;});_tmp96;});nzval=_tmp681;});
({void*_tmp683=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp98=_cycalloc(sizeof(*_tmp98));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp682=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp99;_tmp99.tag=8U;_tmp99.f1=(struct Cyc_Absyn_Vardecl*)names->hd;_tmp99.f2=zval;_tmp99;});_tmp98[0]=_tmp682;});_tmp98;});zval=_tmp683;});}
# 537
({void*_tmp684=Cyc_NewControlFlow_consume_zero_rval(env,_tmp9D,_tmp9B,e,nzval);nzval=_tmp684;});
({void*_tmp685=Cyc_NewControlFlow_consume_zero_rval(env,_tmp9D,_tmp9B,e,zval);zval=_tmp685;});
return({struct _tuple19 _tmp9A;({union Cyc_CfFlowInfo_FlowInfo _tmp687=({
struct Cyc_Dict_Dict _tmp686=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp9D,_tmp9B,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp686,_tmp9C);});_tmp9A.f1=_tmp687;});({union Cyc_CfFlowInfo_FlowInfo _tmp689=({
struct Cyc_Dict_Dict _tmp688=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp9D,_tmp9B,zval);Cyc_CfFlowInfo_ReachableFL(_tmp688,_tmp9C);});_tmp9A.f2=_tmp689;});_tmp9A;});}}_LL5:;}}_LL0:;}
# 546
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 549
static void Cyc_NewControlFlow_update_relns(struct Cyc_Absyn_Exp*e,int possibly_null,struct Cyc_List_List*relns){
void*_tmpA0=e->r;void*_tmpA1=_tmpA0;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA1)->tag == 23U){_LL1: _LL2:
# 553
{void*_tmpA2=e->annot;void*_tmpA3=_tmpA2;struct Cyc_List_List*_tmpA5;struct Cyc_List_List*_tmpA4;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpA3)->tag == Cyc_CfFlowInfo_UnknownZ){_LL6: _tmpA4=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpA3)->f1;_LL7:
# 555
 if(possibly_null  && Cyc_Relations_same_relns(relns,_tmpA4))return;
goto _LL5;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpA3)->tag == Cyc_CfFlowInfo_NotZero){_LL8: _tmpA5=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpA3)->f1;_LL9:
# 558
 possibly_null=0;
if(Cyc_Relations_same_relns(relns,_tmpA5))return;
goto _LL5;}else{_LLA: _LLB:
 goto _LL5;}}_LL5:;}
# 563
if(possibly_null)
({void*_tmp68B=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpA6=_cycalloc(sizeof(*_tmpA6));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp68A=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmpA7;_tmpA7.tag=Cyc_CfFlowInfo_UnknownZ;_tmpA7.f1=relns;_tmpA7;});_tmpA6[0]=_tmp68A;});_tmpA6;});e->annot=_tmp68B;});else{
# 567
({void*_tmp68D=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpA8=_cycalloc(sizeof(*_tmpA8));({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp68C=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmpA9;_tmpA9.tag=Cyc_CfFlowInfo_NotZero;_tmpA9.f1=relns;_tmpA9;});_tmpA8[0]=_tmp68C;});_tmpA8;});e->annot=_tmp68D;});}
# 569
goto _LL0;}else{_LL3: _LL4:
# 573
 if(possibly_null){
void*_tmpAA=e->annot;void*_tmpAB=_tmpAA;struct Cyc_List_List*_tmpAC;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpAB)->tag == Cyc_CfFlowInfo_UnknownZ){_LLD: _tmpAC=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpAB)->f1;_LLE:
# 576
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
return;}else{_LLF: _LL10:
 goto _LLC;}_LLC:;}
# 581
e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL0;}_LL0:;}
# 591
static struct _tuple16 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r){
# 594
void*_tmpAD=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpAE=_tmpAD;void*_tmpD9;union Cyc_Absyn_Constraint*_tmpD8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAE)->tag == 5U){_LL1: _tmpD9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAE)->f1).elt_typ;_tmpD8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAE)->f1).ptr_atts).bounds;_LL2: {
# 596
union Cyc_CfFlowInfo_FlowInfo _tmpAF=f;struct Cyc_Dict_Dict _tmpD5;struct Cyc_List_List*_tmpD4;if((_tmpAF.BottomFL).tag == 1){_LL6: _LL7:
# 598
 return({struct _tuple16 _tmpB0;_tmpB0.f1=f;({void*_tmp68E=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpD9,0,(env->fenv)->unknown_all);_tmpB0.f2=_tmp68E;});_tmpB0;});}else{_LL8: _tmpD5=((_tmpAF.ReachableFL).val).f1;_tmpD4=((_tmpAF.ReachableFL).val).f2;_LL9:
# 601
 if(Cyc_Tcutil_is_bound_one(_tmpD8)){
struct _tuple15 _tmpB1=Cyc_CfFlowInfo_unname_rval(r);struct _tuple15 _tmpB2=_tmpB1;void*_tmpC3;struct Cyc_List_List*_tmpC2;_LLB: _tmpC3=_tmpB2.f1;_tmpC2=_tmpB2.f2;_LLC:;{
void*_tmpB3=_tmpC3;enum Cyc_CfFlowInfo_InitLevel _tmpC1;void*_tmpC0;struct Cyc_CfFlowInfo_Place*_tmpBF;struct Cyc_Absyn_Vardecl*_tmpBE;void*_tmpBD;switch(*((int*)_tmpB3)){case 8U: _LLE: _tmpBE=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpB3)->f1;_tmpBD=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpB3)->f2;_LLF:
# 605
({void*_tmpB4=0U;({struct _dyneither_ptr _tmp68F=({const char*_tmpB5="named location in anal_derefR";_tag_dyneither(_tmpB5,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp68F,_tag_dyneither(_tmpB4,sizeof(void*),0U));});});case 1U: _LL10: _LL11:
# 607
 Cyc_NewControlFlow_update_relns(e,0,_tmpD4);
goto _LLD;case 4U: _LL12: _tmpBF=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpB3)->f1;_LL13:
# 610
 Cyc_NewControlFlow_update_relns(e,0,_tmpD4);
return({struct _tuple16 _tmpB6;_tmpB6.f1=f;({void*_tmp690=Cyc_CfFlowInfo_lookup_place(_tmpD5,_tmpBF);_tmpB6.f2=_tmp690;});_tmpB6;});case 5U: _LL14: _tmpC0=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmpB3)->f1;_LL15:
# 613
 Cyc_NewControlFlow_update_relns(e,1,_tmpD4);
return({struct _tuple16 _tmpB7;_tmpB7.f1=f;_tmpB7.f2=_tmpC0;_tmpB7;});case 0U: _LL16: _LL17:
# 616
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple16 _tmpB8;({union Cyc_CfFlowInfo_FlowInfo _tmp691=Cyc_CfFlowInfo_BottomFL();_tmpB8.f1=_tmp691;});({void*_tmp692=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpD9,0,(env->fenv)->unknown_all);_tmpB8.f2=_tmp692;});_tmpB8;});case 2U: _LL18: _tmpC1=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpB3)->f1;_LL19:
# 620
({union Cyc_CfFlowInfo_FlowInfo _tmp693=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpC1,_tmpC2);f=_tmp693;});
goto _LL1B;default: _LL1A: _LL1B:
# 623
{void*_tmpB9=e->r;void*_tmpBA=_tmpB9;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBA)->tag == 23U){_LL1D: _LL1E:
# 625
({void*_tmp695=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpBB=_cycalloc(sizeof(*_tmpBB));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp694=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmpBC;_tmpBC.tag=Cyc_CfFlowInfo_UnknownZ;_tmpBC.f1=_tmpD4;_tmpBC;});_tmpBB[0]=_tmp694;});_tmpBB;});e->annot=_tmp695;});
goto _LL1C;}else{_LL1F: _LL20:
# 628
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL1C;}_LL1C:;}
# 631
goto _LLD;}_LLD:;};}else{
# 634
void*_tmpC4=e->annot;void*_tmpC5=_tmpC4;struct Cyc_List_List*_tmpC8;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpC5)->tag == Cyc_CfFlowInfo_UnknownZ){_LL22: _tmpC8=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpC5)->f1;_LL23:
# 636
 if(!Cyc_Relations_same_relns(_tmpD4,_tmpC8))goto _LL25;
goto _LL21;}else{_LL24: _LL25:
# 639
({void*_tmp697=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp696=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmpC7;_tmpC7.tag=Cyc_CfFlowInfo_UnknownZ;_tmpC7.f1=_tmpD4;_tmpC7;});_tmpC6[0]=_tmp696;});_tmpC6;});e->annot=_tmp697;});
goto _LL21;}_LL21:;}{
# 643
enum Cyc_CfFlowInfo_InitLevel _tmpC9=Cyc_CfFlowInfo_initlevel(env->fenv,_tmpD5,r);enum Cyc_CfFlowInfo_InitLevel _tmpCA=_tmpC9;if(_tmpCA == Cyc_CfFlowInfo_NoneIL){_LL27: _LL28: {
# 645
struct _tuple15 _tmpCB=Cyc_CfFlowInfo_unname_rval(r);struct _tuple15 _tmpCC=_tmpCB;void*_tmpD2;_LL2C: _tmpD2=_tmpCC.f1;_LL2D:;
{void*_tmpCD=_tmpD2;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpCD)->tag == 7U){_LL2F: _LL30:
# 648
({void*_tmpCE=0U;({unsigned int _tmp699=e->loc;struct _dyneither_ptr _tmp698=({const char*_tmpCF="attempt to dereference a consumed alias-free pointer";_tag_dyneither(_tmpCF,sizeof(char),53U);});Cyc_CfFlowInfo_aerr(_tmp699,_tmp698,_tag_dyneither(_tmpCE,sizeof(void*),0U));});});
goto _LL2E;}else{_LL31: _LL32:
# 651
({void*_tmpD0=0U;({unsigned int _tmp69B=e->loc;struct _dyneither_ptr _tmp69A=({const char*_tmpD1="dereference of possibly uninitialized pointer";_tag_dyneither(_tmpD1,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp69B,_tmp69A,_tag_dyneither(_tmpD0,sizeof(void*),0U));});});}_LL2E:;}
# 653
goto _LL2A;}}else{_LL29: _LL2A:
# 655
 return({struct _tuple16 _tmpD3;_tmpD3.f1=f;({void*_tmp69C=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpD9,0,(env->fenv)->unknown_all);_tmpD3.f2=_tmp69C;});_tmpD3;});}_LL26:;};}_LL5:;}}else{_LL3: _LL4:
# 658
({void*_tmpD6=0U;({struct _dyneither_ptr _tmp69D=({const char*_tmpD7="right deref of non-pointer-type";_tag_dyneither(_tmpD7,sizeof(char),32U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp69D,_tag_dyneither(_tmpD6,sizeof(void*),0U));});});}_LL0:;}
# 665
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 667
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
int e2_valid_op=Cyc_Relations_exp2relnop(e2,& n2);
# 670
{void*_tmpDA=e1->r;void*_tmpDB=_tmpDA;void*_tmpDD;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDB)->tag == 1U){_LL1: _tmpDD=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL2: {
# 672
struct Cyc_Absyn_Vardecl*_tmpDC=Cyc_Tcutil_nonesc_vardecl(_tmpDD);
if(_tmpDC != 0){
union Cyc_Relations_RelnOp n1=Cyc_Relations_RNumelts(_tmpDC);
if(e2_valid_op)
({struct Cyc_List_List*_tmp69E=Cyc_Relations_add_relation(Cyc_Core_heap_region,n2,Cyc_Relations_Rlt,n1,relns);relns=_tmp69E;});}
# 678
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 682
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound((void*)_check_null(e1->topt));
if(bound != 0){
union Cyc_Relations_RelnOp rbound=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(bound,& rbound))
({struct Cyc_List_List*_tmp69F=Cyc_Relations_add_relation(Cyc_Core_heap_region,n2,Cyc_Relations_Rlt,rbound,relns);relns=_tmp69F;});}
# 688
return relns;};}
# 696
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_restore_noconsume_arg(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*exp,unsigned int loc,void*old_rval){
# 701
struct _tuple18 _tmpDE=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);struct _tuple18 _tmpDF=_tmpDE;union Cyc_CfFlowInfo_AbsLVal _tmpEF;_LL1: _tmpEF=_tmpDF.f2;_LL2:;
{struct _tuple18 _tmpE0=({struct _tuple18 _tmpEE;_tmpEE.f1=inflow;_tmpEE.f2=_tmpEF;_tmpEE;});struct _tuple18 _tmpE1=_tmpE0;struct Cyc_Dict_Dict _tmpED;struct Cyc_List_List*_tmpEC;struct Cyc_CfFlowInfo_Place*_tmpEB;if(((_tmpE1.f1).ReachableFL).tag == 2){if(((_tmpE1.f2).PlaceL).tag == 1){_LL4: _tmpED=(((_tmpE1.f1).ReachableFL).val).f1;_tmpEC=(((_tmpE1.f1).ReachableFL).val).f2;_tmpEB=((_tmpE1.f2).PlaceL).val;_LL5: {
# 704
void*_tmpE2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,(void*)_check_null(exp->topt),0,(env->fenv)->unknown_all);
# 706
struct _tuple15 _tmpE3=Cyc_CfFlowInfo_unname_rval(old_rval);struct _tuple15 _tmpE4=_tmpE3;void*_tmpE8;struct Cyc_List_List*_tmpE7;_LLB: _tmpE8=_tmpE4.f1;_tmpE7=_tmpE4.f2;_LLC:;
for(0;_tmpE7 != 0;_tmpE7=_tmpE7->tl){
({void*_tmp6A1=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpE5=_cycalloc(sizeof(*_tmpE5));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6A0=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmpE6;_tmpE6.tag=8U;_tmpE6.f1=(struct Cyc_Absyn_Vardecl*)_tmpE7->hd;_tmpE6.f2=_tmpE2;_tmpE6;});_tmpE5[0]=_tmp6A0;});_tmpE5;});_tmpE2=_tmp6A1;});}
# 711
({struct Cyc_Dict_Dict _tmp6A2=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmpED,_tmpEB,_tmpE2);_tmpED=_tmp6A2;});
({union Cyc_CfFlowInfo_FlowInfo _tmp6A3=Cyc_CfFlowInfo_ReachableFL(_tmpED,_tmpEC);inflow=_tmp6A3;});
Cyc_NewControlFlow_update_tryflow(env,inflow);
# 717
goto _LL3;}}else{_LL8: _LL9:
# 720
({void*_tmpE9=0U;({struct _dyneither_ptr _tmp6A4=({const char*_tmpEA="noconsume() parameters must be l-values";_tag_dyneither(_tmpEA,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6A4,_tag_dyneither(_tmpE9,sizeof(void*),0U));});});
goto _LL3;}}else{_LL6: _LL7:
# 718
 goto _LL3;}_LL3:;}
# 723
return inflow;}
# 728
static struct _tuple16 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 735
union Cyc_CfFlowInfo_FlowInfo _tmpF0=outflow;struct Cyc_Dict_Dict _tmpFE;struct Cyc_List_List*_tmpFD;if((_tmpF0.BottomFL).tag == 1){_LL1: _LL2:
# 737
 return({struct _tuple16 _tmpF1;({union Cyc_CfFlowInfo_FlowInfo _tmp6A5=Cyc_CfFlowInfo_BottomFL();_tmpF1.f1=_tmp6A5;});_tmpF1.f2=rval;_tmpF1;});}else{_LL3: _tmpFE=((_tmpF0.ReachableFL).val).f1;_tmpFD=((_tmpF0.ReachableFL).val).f2;_LL4: {
# 739
union Cyc_CfFlowInfo_AbsLVal _tmpF2=lval;struct Cyc_CfFlowInfo_Place*_tmpFC;if((_tmpF2.PlaceL).tag == 1){_LL6: _tmpFC=(_tmpF2.PlaceL).val;_LL7: {
# 747
struct Cyc_Dict_Dict _tmpF3=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmpFE,_tmpFC,rval);
({struct Cyc_List_List*_tmp6A6=Cyc_Relations_reln_assign_exp(Cyc_Core_heap_region,_tmpFD,lexp,rexp);_tmpFD=_tmp6A6;});
({union Cyc_CfFlowInfo_FlowInfo _tmp6A7=Cyc_CfFlowInfo_ReachableFL(_tmpF3,_tmpFD);outflow=_tmp6A7;});
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(lexp->topt))){
# 753
struct _tuple15 _tmpF4=Cyc_CfFlowInfo_unname_rval(Cyc_CfFlowInfo_lookup_place(_tmpFE,_tmpFC));struct _tuple15 _tmpF5=_tmpF4;void*_tmpF9;_LLB: _tmpF9=_tmpF5.f1;_LLC:;{
void*_tmpF6=_tmpF9;switch(*((int*)_tmpF6)){case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpF6)->f1 == Cyc_CfFlowInfo_NoneIL){_LLE: _LLF:
 goto _LL11;}else{goto _LL14;}case 7U: _LL10: _LL11:
 goto _LL13;case 0U: _LL12: _LL13:
 goto _LLD;default: _LL14: _LL15:
# 759
({void*_tmpF7=0U;({unsigned int _tmp6A9=lexp->loc;struct _dyneither_ptr _tmp6A8=({const char*_tmpF8="assignment may overwrite alias-free pointer(s)";_tag_dyneither(_tmpF8,sizeof(char),47U);});Cyc_Warn_warn(_tmp6A9,_tmp6A8,_tag_dyneither(_tmpF7,sizeof(void*),0U));});});
goto _LLD;}_LLD:;};}
# 764
Cyc_NewControlFlow_update_tryflow(env,outflow);
return({struct _tuple16 _tmpFA;_tmpFA.f1=outflow;_tmpFA.f2=rval;_tmpFA;});}}else{_LL8: _LL9:
# 770
 return({struct _tuple16 _tmpFB;({union Cyc_CfFlowInfo_FlowInfo _tmp6AA=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval);_tmpFB.f1=_tmp6AA;});_tmpFB.f2=rval;_tmpFB;});}_LL5:;}}_LL0:;}
# 777
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 783
union Cyc_CfFlowInfo_FlowInfo _tmpFF=f;struct Cyc_Dict_Dict _tmp105;struct Cyc_List_List*_tmp104;if((_tmpFF.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmp105=((_tmpFF.ReachableFL).val).f1;_tmp104=((_tmpFF.ReachableFL).val).f2;_LL4:
# 788
({struct Cyc_Dict_Dict _tmp6B1=({struct Cyc_CfFlowInfo_FlowEnv*_tmp6B0=fenv;unsigned int _tmp6AF=loc;struct Cyc_Dict_Dict _tmp6AE=_tmp105;struct Cyc_CfFlowInfo_Place*_tmp6AD=({struct Cyc_CfFlowInfo_Place*_tmp100=_cycalloc(sizeof(*_tmp100));({void*_tmp6AC=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp101=_cycalloc(sizeof(*_tmp101));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp6AB=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp102;_tmp102.tag=0U;_tmp102.f1=vd;_tmp102;});_tmp101[0]=_tmp6AB;});_tmp101;});_tmp100->root=_tmp6AC;});_tmp100->path=0;_tmp100;});Cyc_CfFlowInfo_assign_place(_tmp6B0,_tmp6AF,_tmp6AE,_tmp6AD,rval);});_tmp105=_tmp6B1;});
# 791
({struct Cyc_List_List*_tmp6B2=Cyc_Relations_reln_assign_var(Cyc_Core_heap_region,_tmp104,vd,rexp);_tmp104=_tmp6B2;});{
union Cyc_CfFlowInfo_FlowInfo _tmp103=Cyc_CfFlowInfo_ReachableFL(_tmp105,_tmp104);
Cyc_NewControlFlow_update_tryflow(env,_tmp103);
# 796
return _tmp103;};}_LL0:;}struct _tuple24{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 800
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned int pat_loc,unsigned int exp_loc){
# 807
if(vds == 0)return inflow;{
# 810
struct Cyc_List_List*_tmp106=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(vds)).f1);
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple24*)x->hd)).f1 == 0)({struct Cyc_List_List*_tmp6B3=({struct Cyc_List_List*_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple24*)x->hd)).f2);_tmp107->tl=es;_tmp107;});es=_tmp6B3;});}}
# 816
({union Cyc_CfFlowInfo_FlowInfo _tmp6B4=Cyc_NewControlFlow_add_vars(fenv,inflow,_tmp106,fenv->unknown_all,pat_loc,name_locs);inflow=_tmp6B4;});
# 818
({union Cyc_CfFlowInfo_FlowInfo _tmp6B5=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);inflow=_tmp6B5;});
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 822
struct _tuple16 _tmp108=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple16 _tmp109=_tmp108;union Cyc_CfFlowInfo_FlowInfo _tmp10B;void*_tmp10A;_LL1: _tmp10B=_tmp109.f1;_tmp10A=_tmp109.f2;_LL2:;
({union Cyc_CfFlowInfo_FlowInfo _tmp6B6=Cyc_NewControlFlow_use_Rval(env,exp_loc,_tmp10B,_tmp10A);inflow=_tmp6B6;});}}{
# 830
struct Cyc_List_List*_tmp10C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);
for(0;_tmp10C != 0;_tmp10C=_tmp10C->tl){
struct _tuple24*_tmp10D=(struct _tuple24*)_tmp10C->hd;struct _tuple24*_tmp10E=_tmp10D;struct Cyc_Absyn_Vardecl**_tmp129;struct Cyc_Absyn_Exp*_tmp128;_LL4: _tmp129=_tmp10E->f1;_tmp128=_tmp10E->f2;_LL5:;
if(_tmp129 != 0  && _tmp128 != 0){
if(_tmp128->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp111;_tmp111.tag=0U;({struct _dyneither_ptr _tmp6B7=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp128));_tmp111.f1=_tmp6B7;});({void*_tmp10F[1U]={& _tmp111};({struct _dyneither_ptr _tmp6B8=({const char*_tmp110="oops! pattern init expr %s has no type!\n";_tag_dyneither(_tmp110,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6B8,_tag_dyneither(_tmp10F,sizeof(void*),1U));});});});{
# 844
struct Cyc_List_List l=({struct Cyc_List_List _tmp127;_tmp127.hd=_tmp128;_tmp127.tl=0;_tmp127;});
union Cyc_CfFlowInfo_FlowInfo _tmp112=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp113=Cyc_NewControlFlow_anal_Lexp(env,_tmp112,0,0,_tmp128);struct _tuple18 _tmp114=_tmp113;union Cyc_CfFlowInfo_AbsLVal _tmp126;_LL7: _tmp126=_tmp114.f2;_LL8:;{
struct _tuple16 _tmp115=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp112,_tmp128);struct _tuple16 _tmp116=_tmp115;union Cyc_CfFlowInfo_FlowInfo _tmp125;void*_tmp124;_LLA: _tmp125=_tmp116.f1;_tmp124=_tmp116.f2;_LLB:;{
union Cyc_CfFlowInfo_FlowInfo _tmp117=_tmp125;struct Cyc_Dict_Dict _tmp123;struct Cyc_List_List*_tmp122;if((_tmp117.ReachableFL).tag == 2){_LLD: _tmp123=((_tmp117.ReachableFL).val).f1;_tmp122=((_tmp117.ReachableFL).val).f2;_LLE:
# 850
 if(name_locs){
union Cyc_CfFlowInfo_AbsLVal _tmp118=_tmp126;struct Cyc_CfFlowInfo_Place*_tmp121;if((_tmp118.PlaceL).tag == 1){_LL12: _tmp121=(_tmp118.PlaceL).val;_LL13:
# 853
({void*_tmp6BA=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp119=_cycalloc(sizeof(*_tmp119));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6B9=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp11A;_tmp11A.tag=8U;_tmp11A.f1=*_tmp129;_tmp11A.f2=_tmp124;_tmp11A;});_tmp119[0]=_tmp6B9;});_tmp119;});_tmp124=_tmp6BA;});{
# 856
void*_tmp11B=Cyc_CfFlowInfo_lookup_place(_tmp123,_tmp121);
({void*_tmp6BC=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp11C=_cycalloc(sizeof(*_tmp11C));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6BB=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp11D;_tmp11D.tag=8U;_tmp11D.f1=*_tmp129;_tmp11D.f2=_tmp11B;_tmp11D;});_tmp11C[0]=_tmp6BB;});_tmp11C;});_tmp11B=_tmp6BC;});
({struct Cyc_Dict_Dict _tmp6BD=Cyc_CfFlowInfo_assign_place(fenv,exp_loc,_tmp123,_tmp121,_tmp11B);_tmp123=_tmp6BD;});
({union Cyc_CfFlowInfo_FlowInfo _tmp6BE=Cyc_CfFlowInfo_ReachableFL(_tmp123,_tmp122);_tmp125=_tmp6BE;});
goto _LL11;};}else{_LL14: _LL15:
# 863
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(_tmp128->topt)) && !
Cyc_Tcutil_is_noalias_pointer_or_aggr((*_tmp129)->type))
({struct Cyc_String_pa_PrintArg_struct _tmp120;_tmp120.tag=0U;({struct _dyneither_ptr _tmp6BF=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp128));_tmp120.f1=_tmp6BF;});({void*_tmp11E[1U]={& _tmp120};({unsigned int _tmp6C1=exp_loc;struct _dyneither_ptr _tmp6C0=({const char*_tmp11F="aliased pattern expression not an l-value: %s";_tag_dyneither(_tmp11F,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp6C1,_tmp6C0,_tag_dyneither(_tmp11E,sizeof(void*),1U));});});});}_LL11:;}
# 873
({union Cyc_CfFlowInfo_FlowInfo _tmp6C2=Cyc_NewControlFlow_do_initialize_var(fenv,env,_tmp125,*_tmp129,_tmp128,_tmp124,pat_loc);inflow=_tmp6C2;});
goto _LLC;}else{_LLF: _LL10:
# 876
 goto _LLC;}_LLC:;};};};}}
# 881
return inflow;};};}
# 884
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e,int cast_ok){
# 886
void*_tmp12A=e->r;void*_tmp12B=_tmp12A;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*_tmp12E;switch(*((int*)_tmp12B)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp12B)->f1)){case 4U: _LL1: _LL2:
 goto _LL4;case 3U: _LL3: _LL4:
 goto _LL6;case 5U: _LL5: _LL6:
 return 1;default: goto _LL11;}case 20U: _LL7: _tmp12E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp12B)->f1;_LL8:
 _tmp12F=_tmp12E;goto _LLA;case 21U: _LL9: _tmp12F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp12B)->f1;_LLA:
 _tmp130=_tmp12F;goto _LLC;case 22U: _LLB: _tmp130=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp12B)->f1;_LLC:
# 893
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp130,cast_ok);case 23U: _LLD: _tmp131=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp12B)->f1;_LLE: {
# 895
void*_tmp12C=Cyc_Tcutil_compress((void*)_check_null(_tmp131->topt));void*_tmp12D=_tmp12C;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp12D)->tag == 10U){_LL14: _LL15:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp131,cast_ok);}else{_LL16: _LL17:
 return 0;}_LL13:;}case 14U: _LLF: _tmp132=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp12B)->f2;_LL10:
# 900
 if(cast_ok)return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp132,cast_ok);else{
return 0;}default: _LL11: _LL12:
 return 0;}_LL0:;}
# 906
static int Cyc_NewControlFlow_subst_param(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp*rop){
union Cyc_Relations_RelnOp _tmp133=*rop;union Cyc_Relations_RelnOp _tmp134=_tmp133;unsigned int _tmp138;unsigned int _tmp137;switch((_tmp134.RParamNumelts).tag){case 5U: _LL1: _tmp137=(_tmp134.RParam).val;_LL2: {
# 909
struct Cyc_Absyn_Exp*_tmp135=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp137);
return Cyc_Relations_exp2relnop(_tmp135,rop);}case 6U: _LL3: _tmp138=(_tmp134.RParamNumelts).val;_LL4: {
# 912
struct Cyc_Absyn_Exp*_tmp136=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp138);
return({struct Cyc_Absyn_Exp*_tmp6C3=Cyc_Absyn_prim1_exp(Cyc_Absyn_Numelts,_tmp136,0U);Cyc_Relations_exp2relnop(_tmp6C3,rop);});}default: _LL5: _LL6:
 return 1;}_LL0:;}
# 918
static struct _dyneither_ptr Cyc_NewControlFlow_subst_param_string(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp rop){
union Cyc_Relations_RelnOp _tmp139=rop;unsigned int _tmp13B;unsigned int _tmp13A;switch((_tmp139.RParamNumelts).tag){case 5U: _LL1: _tmp13A=(_tmp139.RParam).val;_LL2:
# 921
 return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp13A));case 6U: _LL3: _tmp13B=(_tmp139.RParamNumelts).val;_LL4:
# 923
 return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp13B));default: _LL5: _LL6:
 return Cyc_Relations_rop2string(rop);}_LL0:;}
# 928
static void Cyc_NewControlFlow_check_fn_requires(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*exps,struct Cyc_List_List*req,unsigned int loc){
# 931
union Cyc_CfFlowInfo_FlowInfo _tmp13C=inflow;struct Cyc_Dict_Dict _tmp146;struct Cyc_List_List*_tmp145;if((_tmp13C.BottomFL).tag == 1){_LL1: _LL2:
 return;}else{_LL3: _tmp146=((_tmp13C.ReachableFL).val).f1;_tmp145=((_tmp13C.ReachableFL).val).f2;_LL4:
# 934
 for(0;req != 0;req=req->tl){
struct Cyc_Relations_Reln*_tmp13D=(struct Cyc_Relations_Reln*)req->hd;
union Cyc_Relations_RelnOp rop1=_tmp13D->rop1;
union Cyc_Relations_RelnOp rop2=_tmp13D->rop2;
enum Cyc_Relations_Relation _tmp13E=Cyc_Relations_flip_relation(_tmp13D->relation);
if((!Cyc_NewControlFlow_subst_param(exps,& rop1) || !Cyc_NewControlFlow_subst_param(exps,& rop2)) || 
Cyc_Relations_consistent_relations(Cyc_Relations_add_relation(Cyc_Core_heap_region,rop2,_tmp13E,rop1,_tmp145))){
struct _dyneither_ptr s1=Cyc_NewControlFlow_subst_param_string(exps,rop1);
struct _dyneither_ptr s2=Cyc_Relations_relation2string(_tmp13D->relation);
struct _dyneither_ptr s3=Cyc_NewControlFlow_subst_param_string(exps,rop2);
({struct Cyc_String_pa_PrintArg_struct _tmp144;_tmp144.tag=0U;({struct _dyneither_ptr _tmp6C4=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Relations_relns2string(_tmp145));_tmp144.f1=_tmp6C4;});({struct Cyc_String_pa_PrintArg_struct _tmp143;_tmp143.tag=0U;_tmp143.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);({struct Cyc_String_pa_PrintArg_struct _tmp142;_tmp142.tag=0U;_tmp142.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({struct Cyc_String_pa_PrintArg_struct _tmp141;_tmp141.tag=0U;_tmp141.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({void*_tmp13F[4U]={& _tmp141,& _tmp142,& _tmp143,& _tmp144};({unsigned int _tmp6C6=loc;struct _dyneither_ptr _tmp6C5=({const char*_tmp140="cannot prove that @requires clause %s %s %s is satisfied\n (all I know is %s)";_tag_dyneither(_tmp140,sizeof(char),77U);});Cyc_CfFlowInfo_aerr(_tmp6C6,_tmp6C5,_tag_dyneither(_tmp13F,sizeof(void*),4U));});});});});});});
break;}}
# 949
goto _LL0;}_LL0:;}struct _tuple25{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple26{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 954
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 958
struct Cyc_CfFlowInfo_FlowEnv*_tmp147=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 971
{union Cyc_CfFlowInfo_FlowInfo _tmp148=inflow;struct Cyc_Dict_Dict _tmp14B;struct Cyc_List_List*_tmp14A;if((_tmp148.BottomFL).tag == 1){_LL1: _LL2:
 return({struct _tuple16 _tmp149;({union Cyc_CfFlowInfo_FlowInfo _tmp6C7=Cyc_CfFlowInfo_BottomFL();_tmp149.f1=_tmp6C7;});_tmp149.f2=_tmp147->unknown_all;_tmp149;});}else{_LL3: _tmp14B=((_tmp148.ReachableFL).val).f1;_tmp14A=((_tmp148.ReachableFL).val).f2;_LL4:
 d=_tmp14B;relns=_tmp14A;}_LL0:;}
# 986 "new_control_flow.cyc"
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e,0)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
# 1007 "new_control_flow.cyc"
struct _tuple18 _tmp14C=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);struct _tuple18 _tmp14D=_tmp14C;union Cyc_CfFlowInfo_FlowInfo _tmp15D;union Cyc_CfFlowInfo_AbsLVal _tmp15C;_LL6: _tmp15D=_tmp14D.f1;_tmp15C=_tmp14D.f2;_LL7:;{
struct _tuple18 _tmp14E=({struct _tuple18 _tmp15B;_tmp15B.f1=_tmp15D;_tmp15B.f2=_tmp15C;_tmp15B;});struct _tuple18 _tmp14F=_tmp14E;struct Cyc_Dict_Dict _tmp15A;struct Cyc_List_List*_tmp159;struct Cyc_CfFlowInfo_Place*_tmp158;if(((_tmp14F.f1).ReachableFL).tag == 2){if(((_tmp14F.f2).PlaceL).tag == 1){_LL9: _tmp15A=(((_tmp14F.f1).ReachableFL).val).f1;_tmp159=(((_tmp14F.f1).ReachableFL).val).f2;_tmp158=((_tmp14F.f2).PlaceL).val;_LLA: {
# 1010
void*_tmp150=Cyc_CfFlowInfo_lookup_place(_tmp15A,_tmp158);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp150,& needs_unconsume)){
({void*_tmp151=0U;({unsigned int _tmp6C9=e->loc;struct _dyneither_ptr _tmp6C8=({const char*_tmp152="expression attempts to copy a consumed alias-free value";_tag_dyneither(_tmp152,sizeof(char),56U);});Cyc_CfFlowInfo_aerr(_tmp6C9,_tmp6C8,_tag_dyneither(_tmp151,sizeof(void*),0U));});});
return({struct _tuple16 _tmp153;({union Cyc_CfFlowInfo_FlowInfo _tmp6CA=Cyc_CfFlowInfo_BottomFL();_tmp153.f1=_tmp6CA;});_tmp153.f2=_tmp147->unknown_all;_tmp153;});}else{
# 1016
if(needs_unconsume)
# 1018
return({struct _tuple16 _tmp154;_tmp154.f1=_tmp15D;({void*_tmp6CB=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp147,_tmp150);_tmp154.f2=_tmp6CB;});_tmp154;});else{
# 1021
void*_tmp155=Cyc_CfFlowInfo_make_unique_consumed(_tmp147,(void*)_check_null(e->topt),e,env->iteration_num,_tmp150);
struct Cyc_Dict_Dict _tmp156=Cyc_CfFlowInfo_assign_place(_tmp147,e->loc,_tmp15A,_tmp158,_tmp155);
# 1033
return({struct _tuple16 _tmp157;({union Cyc_CfFlowInfo_FlowInfo _tmp6CC=Cyc_CfFlowInfo_ReachableFL(_tmp156,_tmp159);_tmp157.f1=_tmp6CC;});_tmp157.f2=_tmp150;_tmp157;});}}}}else{goto _LLB;}}else{_LLB: _LLC:
# 1037
 goto _LL8;}_LL8:;};}{
# 1040
void*_tmp15E=e->r;void*_tmp15F=_tmp15E;struct Cyc_Absyn_Stmt*_tmp362;struct Cyc_Absyn_Exp*_tmp361;void*_tmp360;int _tmp35F;struct Cyc_Absyn_Vardecl*_tmp35E;struct Cyc_Absyn_Exp*_tmp35D;struct Cyc_Absyn_Exp*_tmp35C;int _tmp35B;struct Cyc_List_List*_tmp35A;struct Cyc_List_List*_tmp359;enum Cyc_Absyn_AggrKind _tmp358;struct Cyc_List_List*_tmp357;struct Cyc_List_List*_tmp356;struct Cyc_List_List*_tmp355;struct Cyc_Absyn_Exp*_tmp354;struct Cyc_Absyn_Exp*_tmp353;struct Cyc_Absyn_Exp*_tmp352;struct Cyc_Absyn_Exp*_tmp351;struct Cyc_Absyn_Exp*_tmp350;struct Cyc_Absyn_Exp*_tmp34F;struct Cyc_Absyn_Exp*_tmp34E;struct Cyc_Absyn_Exp*_tmp34D;struct Cyc_Absyn_Exp*_tmp34C;struct Cyc_Absyn_Exp*_tmp34B;struct _dyneither_ptr*_tmp34A;struct Cyc_Absyn_Exp*_tmp349;struct _dyneither_ptr*_tmp348;struct Cyc_Absyn_Exp*_tmp347;struct _dyneither_ptr*_tmp346;struct Cyc_Absyn_Exp*_tmp345;struct Cyc_Absyn_Exp*_tmp344;struct Cyc_Absyn_Exp*_tmp343;struct Cyc_Absyn_Exp*_tmp342;struct Cyc_Absyn_Exp*_tmp341;struct Cyc_Absyn_Exp*_tmp340;int _tmp33F;struct Cyc_Absyn_Exp*_tmp33E;void**_tmp33D;struct Cyc_Absyn_Exp*_tmp33C;int _tmp33B;struct Cyc_Absyn_Exp*_tmp33A;struct Cyc_List_List*_tmp339;struct Cyc_Absyn_Exp*_tmp338;struct Cyc_Absyn_Exp*_tmp337;struct Cyc_Absyn_Exp*_tmp336;struct Cyc_Absyn_Exp*_tmp335;struct Cyc_Absyn_Exp*_tmp334;struct Cyc_Absyn_Exp*_tmp333;struct Cyc_Absyn_Exp*_tmp332;struct Cyc_Absyn_Exp*_tmp331;enum Cyc_Absyn_Primop _tmp330;struct Cyc_List_List*_tmp32F;struct Cyc_List_List*_tmp32E;struct Cyc_Absyn_Datatypedecl*_tmp32D;struct Cyc_Absyn_Vardecl*_tmp32C;struct Cyc_Absyn_Vardecl*_tmp32B;struct Cyc_Absyn_Vardecl*_tmp32A;struct _dyneither_ptr _tmp329;struct Cyc_Absyn_Exp*_tmp328;struct Cyc_Absyn_Exp*_tmp327;struct Cyc_Absyn_Exp*_tmp326;struct Cyc_Absyn_Exp*_tmp325;switch(*((int*)_tmp15F)){case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp15F)->f4 == Cyc_Absyn_Null_to_NonNull){_LLE: _tmp325=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LLF: {
# 1044
struct _tuple16 _tmp160=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp325);struct _tuple16 _tmp161=_tmp160;union Cyc_CfFlowInfo_FlowInfo _tmp168;void*_tmp167;_LL7D: _tmp168=_tmp161.f1;_tmp167=_tmp161.f2;_LL7E:;{
struct _tuple16 _tmp162=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp168,_tmp325,_tmp167);struct _tuple16 _tmp163=_tmp162;union Cyc_CfFlowInfo_FlowInfo _tmp166;void*_tmp165;_LL80: _tmp166=_tmp163.f1;_tmp165=_tmp163.f2;_LL81:;
return({struct _tuple16 _tmp164;_tmp164.f1=_tmp166;_tmp164.f2=_tmp167;_tmp164;});};}}else{_LL10: _tmp326=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL11:
# 1050
 _tmp327=_tmp326;goto _LL13;}case 12U: _LL12: _tmp327=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_LL13:
 _tmp328=_tmp327;goto _LL15;case 13U: _LL14: _tmp328=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_LL15:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp328);case 2U: _LL16: _tmp329=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_LL17:
# 1055
 if(!({struct _dyneither_ptr _tmp6CD=(struct _dyneither_ptr)_tmp329;Cyc_NewControlFlow_strcmp(_tmp6CD,({const char*_tmp169="print_flow";_tag_dyneither(_tmp169,sizeof(char),11U);}));})){
struct _dyneither_ptr _tmp16A=Cyc_Position_string_of_segment(e->loc);
({struct Cyc_String_pa_PrintArg_struct _tmp16D;_tmp16D.tag=0U;_tmp16D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp16A);({void*_tmp16B[1U]={& _tmp16D};({struct Cyc___cycFILE*_tmp6CF=Cyc_stderr;struct _dyneither_ptr _tmp6CE=({const char*_tmp16C="%s: current flow is\n";_tag_dyneither(_tmp16C,sizeof(char),21U);});Cyc_fprintf(_tmp6CF,_tmp6CE,_tag_dyneither(_tmp16B,sizeof(void*),1U));});});});
Cyc_CfFlowInfo_print_flow(inflow);
({void*_tmp16E=0U;({struct Cyc___cycFILE*_tmp6D1=Cyc_stderr;struct _dyneither_ptr _tmp6D0=({const char*_tmp16F="\n";_tag_dyneither(_tmp16F,sizeof(char),2U);});Cyc_fprintf(_tmp6D1,_tmp6D0,_tag_dyneither(_tmp16E,sizeof(void*),0U));});});}else{
# 1061
if(!({struct _dyneither_ptr _tmp6D2=(struct _dyneither_ptr)_tmp329;Cyc_NewControlFlow_strcmp(_tmp6D2,({const char*_tmp170="debug_on";_tag_dyneither(_tmp170,sizeof(char),9U);}));}));else{
# 1066
if(!({struct _dyneither_ptr _tmp6D3=(struct _dyneither_ptr)_tmp329;Cyc_NewControlFlow_strcmp(_tmp6D3,({const char*_tmp171="debug_off";_tag_dyneither(_tmp171,sizeof(char),10U);}));}));else{
# 1072
({struct Cyc_String_pa_PrintArg_struct _tmp174;_tmp174.tag=0U;_tmp174.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp329);({void*_tmp172[1U]={& _tmp174};({unsigned int _tmp6D5=e->loc;struct _dyneither_ptr _tmp6D4=({const char*_tmp173="unknown pragma %s";_tag_dyneither(_tmp173,sizeof(char),18U);});Cyc_Warn_warn(_tmp6D5,_tmp6D4,_tag_dyneither(_tmp172,sizeof(void*),1U));});});});}}}
return({struct _tuple16 _tmp175;_tmp175.f1=inflow;_tmp175.f2=_tmp147->zero;_tmp175;});case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp15F)->f1).Wstring_c).tag){case 1U: _LL18: _LL19:
# 1075
 goto _LL1B;case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp15F)->f1).Int_c).val).f2 == 0){_LL1A: _LL1B:
 return({struct _tuple16 _tmp176;_tmp176.f1=inflow;_tmp176.f2=_tmp147->zero;_tmp176;});}else{_LL1C: _LL1D:
 goto _LL1F;}case 8U: _LL1E: _LL1F:
 goto _LL21;case 9U: _LL20: _LL21:
 goto _LL23;default: _LL26: _LL27:
# 1083
 goto _LL29;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp15F)->f1)){case 2U: _LL22: _LL23:
# 1080
 return({struct _tuple16 _tmp177;_tmp177.f1=inflow;_tmp177.f2=_tmp147->notzeroall;_tmp177;});case 1U: _LL32: _LL33:
# 1091
 return({struct _tuple16 _tmp179;_tmp179.f1=inflow;({void*_tmp6D6=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),0,_tmp147->unknown_all);_tmp179.f2=_tmp6D6;});_tmp179;});case 3U: _LL34: _tmp32A=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp15F)->f1)->f1;_LL35:
# 1094
 _tmp32B=_tmp32A;goto _LL37;case 4U: _LL36: _tmp32B=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp15F)->f1)->f1;_LL37:
 _tmp32C=_tmp32B;goto _LL39;case 5U: _LL38: _tmp32C=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp15F)->f1)->f1;_LL39: {
# 1098
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp17C;_tmp17C.tag=0U;_tmp17C.f1=_tmp32C;_tmp17C;});
return({struct _tuple16 _tmp17A;_tmp17A.f1=inflow;({void*_tmp6D8=({struct Cyc_Dict_Dict _tmp6D7=d;((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp6D7,Cyc_CfFlowInfo_root_cmp,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B[0]=vdroot;_tmp17B;}));});_tmp17A.f2=_tmp6D8;});_tmp17A;});}default: _LL72: _LL73:
# 1678
 goto _LL75;}case 31U: if(((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp15F)->f1 == 0){_LL24: _LL25:
# 1082
 goto _LL27;}else{_LL60: _tmp32E=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp32D=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL61:
# 1541
 _tmp355=_tmp32E;goto _LL63;}case 18U: _LL28: _LL29:
# 1084
 goto _LL2B;case 17U: _LL2A: _LL2B:
 goto _LL2D;case 19U: _LL2C: _LL2D:
 goto _LL2F;case 33U: _LL2E: _LL2F:
 goto _LL31;case 32U: _LL30: _LL31:
 return({struct _tuple16 _tmp178;_tmp178.f1=inflow;_tmp178.f2=_tmp147->unknown_all;_tmp178;});case 3U: _LL3A: _tmp330=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp32F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL3B: {
# 1105
struct _tuple16 _tmp17D=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp32F);struct _tuple16 _tmp17E=_tmp17D;union Cyc_CfFlowInfo_FlowInfo _tmp181;void*_tmp180;_LL83: _tmp181=_tmp17E.f1;_tmp180=_tmp17E.f2;_LL84:;
return({struct _tuple16 _tmp17F;_tmp17F.f1=_tmp181;_tmp17F.f2=_tmp180;_tmp17F;});}case 5U: _LL3C: _tmp331=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_LL3D: {
# 1109
struct Cyc_List_List _tmp182=({struct Cyc_List_List _tmp195;_tmp195.hd=_tmp331;_tmp195.tl=0;_tmp195;});
struct _tuple16 _tmp183=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp182);struct _tuple16 _tmp184=_tmp183;union Cyc_CfFlowInfo_FlowInfo _tmp194;_LL86: _tmp194=_tmp184.f1;_LL87:;{
struct _tuple18 _tmp185=Cyc_NewControlFlow_anal_Lexp(env,_tmp194,0,0,_tmp331);struct _tuple18 _tmp186=_tmp185;union Cyc_CfFlowInfo_AbsLVal _tmp193;_LL89: _tmp193=_tmp186.f2;_LL8A:;{
int iszt=Cyc_Tcutil_is_zeroterm_pointer_type((void*)_check_null(_tmp331->topt));
if(iszt){
# 1116
struct _tuple16 _tmp187=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp331);struct _tuple16 _tmp188=_tmp187;union Cyc_CfFlowInfo_FlowInfo _tmp18A;void*_tmp189;_LL8C: _tmp18A=_tmp188.f1;_tmp189=_tmp188.f2;_LL8D:;
Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp18A,_tmp331,_tmp189);}
# 1119
{struct _tuple25 _tmp18B=({struct _tuple25 _tmp190;_tmp190.f1=_tmp193;_tmp190.f2=_tmp194;_tmp190;});struct _tuple25 _tmp18C=_tmp18B;struct Cyc_CfFlowInfo_Place*_tmp18F;struct Cyc_Dict_Dict _tmp18E;struct Cyc_List_List*_tmp18D;if(((_tmp18C.f1).PlaceL).tag == 1){if(((_tmp18C.f2).ReachableFL).tag == 2){_LL8F: _tmp18F=((_tmp18C.f1).PlaceL).val;_tmp18E=(((_tmp18C.f2).ReachableFL).val).f1;_tmp18D=(((_tmp18C.f2).ReachableFL).val).f2;_LL90:
# 1121
({struct Cyc_List_List*_tmp6D9=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp18D,_tmp331);_tmp18D=_tmp6D9;});
({union Cyc_CfFlowInfo_FlowInfo _tmp6DB=({struct Cyc_Dict_Dict _tmp6DA=Cyc_CfFlowInfo_assign_place(_tmp147,_tmp331->loc,_tmp18E,_tmp18F,_tmp147->unknown_all);Cyc_CfFlowInfo_ReachableFL(_tmp6DA,_tmp18D);});_tmp194=_tmp6DB;});
# 1126
goto _LL8E;}else{goto _LL91;}}else{_LL91: _LL92:
 goto _LL8E;}_LL8E:;}
# 1130
if(iszt)
return({struct _tuple16 _tmp191;_tmp191.f1=_tmp194;_tmp191.f2=_tmp147->notzeroall;_tmp191;});else{
return({struct _tuple16 _tmp192;_tmp192.f1=_tmp194;_tmp192.f2=_tmp147->unknown_all;_tmp192;});}};};}case 4U: if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp15F)->f2 != 0){_LL3E: _tmp333=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp332=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp15F)->f3;_LL3F:
# 1135
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp196=0U;({unsigned int _tmp6DD=e->loc;struct _dyneither_ptr _tmp6DC=({const char*_tmp197="cannot track alias-free pointers through multiple assignments";_tag_dyneither(_tmp197,sizeof(char),62U);});Cyc_CfFlowInfo_aerr(_tmp6DD,_tmp6DC,_tag_dyneither(_tmp196,sizeof(void*),0U));});});
return({struct _tuple16 _tmp198;({union Cyc_CfFlowInfo_FlowInfo _tmp6DE=Cyc_CfFlowInfo_BottomFL();_tmp198.f1=_tmp6DE;});_tmp198.f2=_tmp147->unknown_all;_tmp198;});}{
# 1139
struct _tuple16 _tmp199=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp332);struct _tuple16 _tmp19A=_tmp199;union Cyc_CfFlowInfo_FlowInfo _tmp1A6;void*_tmp1A5;_LL94: _tmp1A6=_tmp19A.f1;_tmp1A5=_tmp19A.f2;_LL95:;{
struct _tuple18 _tmp19B=Cyc_NewControlFlow_anal_Lexp(env,_tmp1A6,0,0,_tmp333);struct _tuple18 _tmp19C=_tmp19B;union Cyc_CfFlowInfo_FlowInfo _tmp1A4;union Cyc_CfFlowInfo_AbsLVal _tmp1A3;_LL97: _tmp1A4=_tmp19C.f1;_tmp1A3=_tmp19C.f2;_LL98:;
{union Cyc_CfFlowInfo_FlowInfo _tmp19D=_tmp1A4;struct Cyc_Dict_Dict _tmp1A1;struct Cyc_List_List*_tmp1A0;if((_tmp19D.ReachableFL).tag == 2){_LL9A: _tmp1A1=((_tmp19D.ReachableFL).val).f1;_tmp1A0=((_tmp19D.ReachableFL).val).f2;_LL9B:
# 1143
{union Cyc_CfFlowInfo_AbsLVal _tmp19E=_tmp1A3;struct Cyc_CfFlowInfo_Place*_tmp19F;if((_tmp19E.PlaceL).tag == 1){_LL9F: _tmp19F=(_tmp19E.PlaceL).val;_LLA0:
# 1147
({struct Cyc_List_List*_tmp6DF=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp1A0,_tmp333);_tmp1A0=_tmp6DF;});
({struct Cyc_Dict_Dict _tmp6E0=Cyc_CfFlowInfo_assign_place(_tmp147,_tmp333->loc,_tmp1A1,_tmp19F,_tmp147->unknown_all);_tmp1A1=_tmp6E0;});
# 1150
({union Cyc_CfFlowInfo_FlowInfo _tmp6E1=Cyc_CfFlowInfo_ReachableFL(_tmp1A1,_tmp1A0);_tmp1A4=_tmp6E1;});
# 1154
goto _LL9E;}else{_LLA1: _LLA2:
# 1157
 goto _LL9E;}_LL9E:;}
# 1159
goto _LL99;}else{_LL9C: _LL9D:
 goto _LL99;}_LL99:;}
# 1162
return({struct _tuple16 _tmp1A2;_tmp1A2.f1=_tmp1A4;_tmp1A2.f2=_tmp147->unknown_all;_tmp1A2;});};};}else{_LL40: _tmp335=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp334=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp15F)->f3;_LL41:
# 1165
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp1A7=0U;({unsigned int _tmp6E3=e->loc;struct _dyneither_ptr _tmp6E2=({const char*_tmp1A8="cannot track alias-free pointers through multiple assignments";_tag_dyneither(_tmp1A8,sizeof(char),62U);});Cyc_CfFlowInfo_aerr(_tmp6E3,_tmp6E2,_tag_dyneither(_tmp1A7,sizeof(void*),0U));});});
return({struct _tuple16 _tmp1A9;({union Cyc_CfFlowInfo_FlowInfo _tmp6E4=Cyc_CfFlowInfo_BottomFL();_tmp1A9.f1=_tmp6E4;});_tmp1A9.f2=_tmp147->unknown_all;_tmp1A9;});}
# 1171
({union Cyc_CfFlowInfo_FlowInfo _tmp6E7=({struct Cyc_NewControlFlow_AnalEnv*_tmp6E6=env;union Cyc_CfFlowInfo_FlowInfo _tmp6E5=inflow;Cyc_NewControlFlow_expand_unique_places(_tmp6E6,_tmp6E5,({struct Cyc_Absyn_Exp*_tmp1AA[2U];_tmp1AA[1U]=_tmp334;_tmp1AA[0U]=_tmp335;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1AA,sizeof(struct Cyc_Absyn_Exp*),2U));}));});inflow=_tmp6E7;});{
# 1173
struct _tuple16 _tmp1AB=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp334);struct _tuple16 _tmp1AC=_tmp1AB;union Cyc_CfFlowInfo_FlowInfo _tmp1B7;void*_tmp1B6;_LLA4: _tmp1B7=_tmp1AC.f1;_tmp1B6=_tmp1AC.f2;_LLA5:;{
struct _tuple18 _tmp1AD=Cyc_NewControlFlow_anal_Lexp(env,_tmp1B7,0,0,_tmp335);struct _tuple18 _tmp1AE=_tmp1AD;union Cyc_CfFlowInfo_FlowInfo _tmp1B5;union Cyc_CfFlowInfo_AbsLVal _tmp1B4;_LLA7: _tmp1B5=_tmp1AE.f1;_tmp1B4=_tmp1AE.f2;_LLA8:;{
struct _tuple16 _tmp1AF=Cyc_NewControlFlow_do_assign(_tmp147,env,_tmp1B5,_tmp335,_tmp1B4,_tmp334,_tmp1B6,e->loc);struct _tuple16 _tmp1B0=_tmp1AF;union Cyc_CfFlowInfo_FlowInfo _tmp1B3;void*_tmp1B2;_LLAA: _tmp1B3=_tmp1B0.f1;_tmp1B2=_tmp1B0.f2;_LLAB:;
# 1179
return({struct _tuple16 _tmp1B1;_tmp1B1.f1=_tmp1B3;_tmp1B1.f2=_tmp1B2;_tmp1B1;});};};};}case 9U: _LL42: _tmp337=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp336=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL43: {
# 1182
struct _tuple16 _tmp1B8=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp337);struct _tuple16 _tmp1B9=_tmp1B8;union Cyc_CfFlowInfo_FlowInfo _tmp1BB;void*_tmp1BA;_LLAD: _tmp1BB=_tmp1B9.f1;_tmp1BA=_tmp1B9.f2;_LLAE:;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp1BB,_tmp336);}case 11U: _LL44: _tmp338=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_LL45: {
# 1186
struct _tuple16 _tmp1BC=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp338);struct _tuple16 _tmp1BD=_tmp1BC;union Cyc_CfFlowInfo_FlowInfo _tmp1C0;void*_tmp1BF;_LLB0: _tmp1C0=_tmp1BD.f1;_tmp1BF=_tmp1BD.f2;_LLB1:;
Cyc_NewControlFlow_use_Rval(env,_tmp338->loc,_tmp1C0,_tmp1BF);
return({struct _tuple16 _tmp1BE;({union Cyc_CfFlowInfo_FlowInfo _tmp6E8=Cyc_CfFlowInfo_BottomFL();_tmp1BE.f1=_tmp6E8;});({void*_tmp6E9=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),0,_tmp147->unknown_all);_tmp1BE.f2=_tmp6E9;});_tmp1BE;});}case 10U: _LL46: _tmp33A=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp339=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL47: {
# 1191
struct Cyc_List_List*_tmp1C1=_tmp339;
struct _tuple23 _tmp1C2=({
struct Cyc_NewControlFlow_AnalEnv*_tmp6EC=env;union Cyc_CfFlowInfo_FlowInfo _tmp6EB=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6EC,_tmp6EB,({struct Cyc_List_List*_tmp1F2=_cycalloc(sizeof(*_tmp1F2));_tmp1F2->hd=_tmp33A;({struct Cyc_List_List*_tmp6EA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp339);_tmp1F2->tl=_tmp6EA;});_tmp1F2;}),0,1);});
# 1192
struct _tuple23 _tmp1C3=_tmp1C2;union Cyc_CfFlowInfo_FlowInfo _tmp1F1;struct Cyc_List_List*_tmp1F0;_LLB3: _tmp1F1=_tmp1C3.f1;_tmp1F0=_tmp1C3.f2;_LLB4:;{
# 1195
union Cyc_CfFlowInfo_FlowInfo _tmp1C4=Cyc_NewControlFlow_use_Rval(env,_tmp33A->loc,_tmp1F1,(void*)((struct Cyc_List_List*)_check_null(_tmp1F0))->hd);
_tmp1F0=_tmp1F0->tl;{
# 1198
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
struct Cyc_List_List*requires;
struct Cyc_List_List*ensures;
{void*_tmp1C5=Cyc_Tcutil_compress((void*)_check_null(_tmp33A->topt));void*_tmp1C6=_tmp1C5;void*_tmp1D8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C6)->tag == 5U){_LLB6: _tmp1D8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C6)->f1).elt_typ;_LLB7:
# 1205
{void*_tmp1C7=Cyc_Tcutil_compress(_tmp1D8);void*_tmp1C8=_tmp1C7;struct Cyc_List_List*_tmp1D5;struct Cyc_List_List*_tmp1D4;struct Cyc_List_List*_tmp1D3;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->tag == 9U){_LLBB: _tmp1D5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).attributes;_tmp1D4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).requires_relns;_tmp1D3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).ensures_relns;_LLBC:
# 1207
 requires=_tmp1D4;
ensures=_tmp1D3;
for(0;_tmp1D5 != 0;_tmp1D5=_tmp1D5->tl){
# 1211
void*_tmp1C9=(void*)_tmp1D5->hd;void*_tmp1CA=_tmp1C9;int _tmp1D0;int _tmp1CF;int _tmp1CE;switch(*((int*)_tmp1CA)){case 20U: _LLC0: _tmp1CE=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1CA)->f1;_LLC1:
# 1213
({struct Cyc_List_List*_tmp6ED=({struct Cyc_List_List*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB->hd=(void*)_tmp1CE;_tmp1CB->tl=init_params;_tmp1CB;});init_params=_tmp6ED;});goto _LLBF;case 21U: _LLC2: _tmp1CF=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp1CA)->f1;_LLC3:
# 1215
({struct Cyc_List_List*_tmp6EE=({struct Cyc_List_List*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->hd=(void*)_tmp1CF;_tmp1CC->tl=nolive_unique_params;_tmp1CC;});nolive_unique_params=_tmp6EE;});
goto _LLBF;case 22U: _LLC4: _tmp1D0=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp1CA)->f1;_LLC5:
# 1219
({struct Cyc_List_List*_tmp6EF=({struct Cyc_List_List*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD->hd=(void*)_tmp1D0;_tmp1CD->tl=noconsume_params;_tmp1CD;});noconsume_params=_tmp6EF;});
goto _LLBF;default: _LLC6: _LLC7:
 goto _LLBF;}_LLBF:;}
# 1223
goto _LLBA;}else{_LLBD: _LLBE:
({void*_tmp1D1=0U;({struct _dyneither_ptr _tmp6F0=({const char*_tmp1D2="anal_Rexp: bad function type";_tag_dyneither(_tmp1D2,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6F0,_tag_dyneither(_tmp1D1,sizeof(void*),0U));});});}_LLBA:;}
# 1226
goto _LLB5;}else{_LLB8: _LLB9:
({void*_tmp1D6=0U;({struct _dyneither_ptr _tmp6F1=({const char*_tmp1D7="anal_Rexp: bad function type";_tag_dyneither(_tmp1D7,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6F1,_tag_dyneither(_tmp1D6,sizeof(void*),0U));});});}_LLB5:;}
# 1229
{int i=1;for(0;_tmp1F0 != 0;(((_tmp1F0=_tmp1F0->tl,_tmp339=((struct Cyc_List_List*)_check_null(_tmp339))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp1D9=_tmp1F1;struct Cyc_Dict_Dict _tmp1ED;if((_tmp1D9.BottomFL).tag == 1){_LLC9: _LLCA:
 goto _LLC8;}else{_LLCB: _tmp1ED=((_tmp1D9.ReachableFL).val).f1;_LLCC: {
# 1234
struct _tuple15 _tmp1DA=Cyc_CfFlowInfo_unname_rval((void*)_tmp1F0->hd);struct _tuple15 _tmp1DB=_tmp1DA;void*_tmp1EC;_LLCE: _tmp1EC=_tmp1DB.f1;_LLCF:;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp1ED,(void*)_tmp1F0->hd)== Cyc_CfFlowInfo_NoneIL  && !Cyc_CfFlowInfo_is_init_pointer((void*)_tmp1F0->hd))
({void*_tmp1DC=0U;({unsigned int _tmp6F3=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp339))->hd)->loc;struct _dyneither_ptr _tmp6F2=({const char*_tmp1DD="expression may not be initialized";_tag_dyneither(_tmp1DD,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp6F3,_tmp6F2,_tag_dyneither(_tmp1DC,sizeof(void*),0U));});});
{union Cyc_CfFlowInfo_FlowInfo _tmp1DE=_tmp1C4;struct Cyc_Dict_Dict _tmp1EB;struct Cyc_List_List*_tmp1EA;if((_tmp1DE.BottomFL).tag == 1){_LLD1: _LLD2:
 goto _LLD0;}else{_LLD3: _tmp1EB=((_tmp1DE.ReachableFL).val).f1;_tmp1EA=((_tmp1DE.ReachableFL).val).f2;_LLD4: {
# 1242
struct Cyc_Dict_Dict _tmp1DF=Cyc_CfFlowInfo_escape_deref(_tmp147,_tmp1EB,(void*)_tmp1F0->hd);
{void*_tmp1E0=(void*)_tmp1F0->hd;void*_tmp1E1=_tmp1E0;struct Cyc_CfFlowInfo_Place*_tmp1E9;switch(*((int*)_tmp1E1)){case 4U: _LLD6: _tmp1E9=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E1)->f1;_LLD7:
# 1245
{void*_tmp1E2=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp339))->hd)->topt));void*_tmp1E3=_tmp1E2;void*_tmp1E6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E3)->tag == 5U){_LLDD: _tmp1E6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E3)->f1).elt_typ;_LLDE:
# 1247
({struct Cyc_Dict_Dict _tmp6F8=({struct Cyc_CfFlowInfo_FlowEnv*_tmp6F7=_tmp147;unsigned int _tmp6F6=((struct Cyc_Absyn_Exp*)_tmp339->hd)->loc;struct Cyc_Dict_Dict _tmp6F5=_tmp1DF;struct Cyc_CfFlowInfo_Place*_tmp6F4=_tmp1E9;Cyc_CfFlowInfo_assign_place(_tmp6F7,_tmp6F6,_tmp6F5,_tmp6F4,
Cyc_CfFlowInfo_typ_to_absrval(_tmp147,_tmp1E6,0,_tmp147->esc_all));});
# 1247
_tmp1DF=_tmp6F8;});
# 1251
goto _LLDC;}else{_LLDF: _LLE0:
({void*_tmp1E4=0U;({struct _dyneither_ptr _tmp6F9=({const char*_tmp1E5="anal_Rexp:bad type for initialized arg";_tag_dyneither(_tmp1E5,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6F9,_tag_dyneither(_tmp1E4,sizeof(void*),0U));});});}_LLDC:;}
# 1254
goto _LLD5;case 5U: _LLD8: _LLD9:
# 1256
({void*_tmp1E7=0U;({struct _dyneither_ptr _tmp6FA=({const char*_tmp1E8="anal_Rexp:initialize attribute on unique pointers not yet supported";_tag_dyneither(_tmp1E8,sizeof(char),68U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6FA,_tag_dyneither(_tmp1E7,sizeof(void*),0U));});});default: _LLDA: _LLDB:
# 1258
 goto _LLD5;}_LLD5:;}
# 1260
({union Cyc_CfFlowInfo_FlowInfo _tmp6FB=Cyc_CfFlowInfo_ReachableFL(_tmp1DF,_tmp1EA);_tmp1C4=_tmp6FB;});
goto _LLD0;}}_LLD0:;}
# 1263
goto _LLC8;}}_LLC8:;}else{
# 1266
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1271
({union Cyc_CfFlowInfo_FlowInfo _tmp700=({struct Cyc_NewControlFlow_AnalEnv*_tmp6FF=env;unsigned int _tmp6FE=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp339))->hd)->loc;void*_tmp6FD=(void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp339->hd)->topt);union Cyc_CfFlowInfo_FlowInfo _tmp6FC=_tmp1C4;Cyc_NewControlFlow_use_nounique_Rval(_tmp6FF,_tmp6FE,_tmp6FD,_tmp6FC,(void*)_tmp1F0->hd);});_tmp1C4=_tmp700;});else{
# 1281
({union Cyc_CfFlowInfo_FlowInfo _tmp701=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp339))->hd)->loc,_tmp1C4,(void*)_tmp1F0->hd);_tmp1C4=_tmp701;});
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp339->hd)->topt),0))
({union Cyc_CfFlowInfo_FlowInfo _tmp702=Cyc_NewControlFlow_restore_noconsume_arg(env,_tmp1C4,(struct Cyc_Absyn_Exp*)_tmp339->hd,((struct Cyc_Absyn_Exp*)_tmp339->hd)->loc,(void*)_tmp1F0->hd);_tmp1C4=_tmp702;});}}}}
# 1288
Cyc_NewControlFlow_check_fn_requires(env,_tmp1C4,_tmp1C1,requires,e->loc);
# 1291
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp33A->topt)))
return({struct _tuple16 _tmp1EE;({union Cyc_CfFlowInfo_FlowInfo _tmp703=Cyc_CfFlowInfo_BottomFL();_tmp1EE.f1=_tmp703;});({void*_tmp704=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),0,_tmp147->unknown_all);_tmp1EE.f2=_tmp704;});_tmp1EE;});else{
# 1294
return({struct _tuple16 _tmp1EF;_tmp1EF.f1=_tmp1C4;({void*_tmp705=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),0,_tmp147->unknown_all);_tmp1EF.f2=_tmp705;});_tmp1EF;});}};};}case 34U: _LL48: _tmp33F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp15F)->f1).is_calloc;_tmp33E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp15F)->f1).rgn;_tmp33D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp15F)->f1).elt_type;_tmp33C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp15F)->f1).num_elts;_tmp33B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp15F)->f1).fat_result;_LL49: {
# 1297
void*place_val;
if(_tmp33B)place_val=_tmp147->notzeroall;else{
if(_tmp33F)({void*_tmp706=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,*((void**)_check_null(_tmp33D)),0,_tmp147->zero);place_val=_tmp706;});else{
({void*_tmp707=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,*((void**)_check_null(_tmp33D)),0,_tmp147->unknown_none);place_val=_tmp707;});}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
if(_tmp33E != 0){
struct _tuple23 _tmp1F3=({struct Cyc_NewControlFlow_AnalEnv*_tmp709=env;union Cyc_CfFlowInfo_FlowInfo _tmp708=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp709,_tmp708,({struct Cyc_Absyn_Exp*_tmp1F7[2U];_tmp1F7[1U]=_tmp33C;_tmp1F7[0U]=_tmp33E;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1F7,sizeof(struct Cyc_Absyn_Exp*),2U));}),1,1);});struct _tuple23 _tmp1F4=_tmp1F3;union Cyc_CfFlowInfo_FlowInfo _tmp1F6;struct Cyc_List_List*_tmp1F5;_LLE2: _tmp1F6=_tmp1F4.f1;_tmp1F5=_tmp1F4.f2;_LLE3:;
outflow=_tmp1F6;}else{
# 1306
struct _tuple16 _tmp1F8=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp33C);struct _tuple16 _tmp1F9=_tmp1F8;union Cyc_CfFlowInfo_FlowInfo _tmp1FA;_LLE5: _tmp1FA=_tmp1F9.f1;_LLE6:;
outflow=_tmp1FA;}
# 1311
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e->topt),1))
return({struct _tuple16 _tmp1FB;_tmp1FB.f1=outflow;({void*_tmp70B=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp1FC=_cycalloc(sizeof(*_tmp1FC));({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp70A=({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp1FD;_tmp1FD.tag=5U;_tmp1FD.f1=place_val;_tmp1FD;});_tmp1FC[0]=_tmp70A;});_tmp1FC;});_tmp1FB.f2=_tmp70B;});_tmp1FB;});else{
# 1315
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp206=_cycalloc(sizeof(*_tmp206));({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp70C=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp207;_tmp207.tag=1U;_tmp207.f1=_tmp33C;_tmp207.f2=(void*)_check_null(e->topt);_tmp207;});_tmp206[0]=_tmp70C;});_tmp206;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp205=_cycalloc(sizeof(*_tmp205));_tmp205->root=root;_tmp205->path=0;_tmp205;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp203=_cycalloc(sizeof(*_tmp203));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp70D=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp204;_tmp204.tag=4U;_tmp204.f1=place;_tmp204;});_tmp203[0]=_tmp70D;});_tmp203;});
union Cyc_CfFlowInfo_FlowInfo _tmp1FE=outflow;struct Cyc_Dict_Dict _tmp202;struct Cyc_List_List*_tmp201;if((_tmp1FE.BottomFL).tag == 1){_LLE8: _LLE9:
 return({struct _tuple16 _tmp1FF;_tmp1FF.f1=outflow;_tmp1FF.f2=rval;_tmp1FF;});}else{_LLEA: _tmp202=((_tmp1FE.ReachableFL).val).f1;_tmp201=((_tmp1FE.ReachableFL).val).f2;_LLEB:
# 1321
 return({struct _tuple16 _tmp200;({union Cyc_CfFlowInfo_FlowInfo _tmp70F=({struct Cyc_Dict_Dict _tmp70E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp202,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp70E,_tmp201);});_tmp200.f1=_tmp70F;});_tmp200.f2=rval;_tmp200;});}_LLE7:;}};}case 35U: _LL4A: _tmp341=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp340=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL4B: {
# 1326
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1332
struct _tuple23 _tmp208=({struct Cyc_NewControlFlow_AnalEnv*_tmp711=env;union Cyc_CfFlowInfo_FlowInfo _tmp710=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp711,_tmp710,({struct Cyc_Absyn_Exp*_tmp21E[2U];_tmp21E[1U]=_tmp340;_tmp21E[0U]=_tmp341;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp21E,sizeof(struct Cyc_Absyn_Exp*),2U));}),0,0);});struct _tuple23 _tmp209=_tmp208;union Cyc_CfFlowInfo_FlowInfo _tmp21D;struct Cyc_List_List*_tmp21C;_LLED: _tmp21D=_tmp209.f1;_tmp21C=_tmp209.f2;_LLEE:;
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp21C))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp21C->tl))->hd;
outflow=_tmp21D;{
# 1337
struct _tuple18 _tmp20A=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp341);struct _tuple18 _tmp20B=_tmp20A;union Cyc_CfFlowInfo_AbsLVal _tmp21B;_LLF0: _tmp21B=_tmp20B.f2;_LLF1:;{
struct _tuple18 _tmp20C=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp340);struct _tuple18 _tmp20D=_tmp20C;union Cyc_CfFlowInfo_AbsLVal _tmp21A;_LLF3: _tmp21A=_tmp20D.f2;_LLF4:;
{union Cyc_CfFlowInfo_FlowInfo _tmp20E=outflow;struct Cyc_Dict_Dict _tmp218;struct Cyc_List_List*_tmp217;if((_tmp20E.ReachableFL).tag == 2){_LLF6: _tmp218=((_tmp20E.ReachableFL).val).f1;_tmp217=((_tmp20E.ReachableFL).val).f2;_LLF7:
# 1341
{union Cyc_CfFlowInfo_AbsLVal _tmp20F=_tmp21B;struct Cyc_CfFlowInfo_Place*_tmp212;if((_tmp20F.PlaceL).tag == 1){_LLFB: _tmp212=(_tmp20F.PlaceL).val;_LLFC:
# 1343
({struct Cyc_Dict_Dict _tmp712=Cyc_CfFlowInfo_assign_place(_tmp147,_tmp341->loc,_tmp218,_tmp212,right_rval);_tmp218=_tmp712;});
goto _LLFA;}else{_LLFD: _LLFE:
# 1349
 if(Cyc_CfFlowInfo_initlevel(_tmp147,_tmp218,right_rval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp210=0U;({unsigned int _tmp714=_tmp340->loc;struct _dyneither_ptr _tmp713=({const char*_tmp211="expression may not be fully initialized";_tag_dyneither(_tmp211,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp714,_tmp713,_tag_dyneither(_tmp210,sizeof(void*),0U));});});
goto _LLFA;}_LLFA:;}
# 1353
{union Cyc_CfFlowInfo_AbsLVal _tmp213=_tmp21A;struct Cyc_CfFlowInfo_Place*_tmp216;if((_tmp213.PlaceL).tag == 1){_LL100: _tmp216=(_tmp213.PlaceL).val;_LL101:
# 1355
({struct Cyc_Dict_Dict _tmp715=Cyc_CfFlowInfo_assign_place(_tmp147,_tmp340->loc,_tmp218,_tmp216,left_rval);_tmp218=_tmp715;});
goto _LLFF;}else{_LL102: _LL103:
# 1358
 if(Cyc_CfFlowInfo_initlevel(_tmp147,_tmp218,left_rval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp214=0U;({unsigned int _tmp717=_tmp341->loc;struct _dyneither_ptr _tmp716=({const char*_tmp215="expression may not be fully initialized";_tag_dyneither(_tmp215,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp717,_tmp716,_tag_dyneither(_tmp214,sizeof(void*),0U));});});
goto _LLFF;}_LLFF:;}
# 1363
({struct Cyc_List_List*_tmp718=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp217,_tmp341);_tmp217=_tmp718;});
({struct Cyc_List_List*_tmp719=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp217,_tmp340);_tmp217=_tmp719;});
# 1366
({union Cyc_CfFlowInfo_FlowInfo _tmp71A=Cyc_CfFlowInfo_ReachableFL(_tmp218,_tmp217);outflow=_tmp71A;});
Cyc_NewControlFlow_update_tryflow(env,outflow);
goto _LLF5;}else{_LLF8: _LLF9:
 goto _LLF5;}_LLF5:;}
# 1373
return({struct _tuple16 _tmp219;_tmp219.f1=outflow;_tmp219.f2=_tmp147->unknown_all;_tmp219;});};};}case 16U: _LL4C: _tmp343=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp342=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL4D: {
# 1376
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp343 != 0){
struct _tuple23 _tmp21F=({struct Cyc_NewControlFlow_AnalEnv*_tmp71C=env;union Cyc_CfFlowInfo_FlowInfo _tmp71B=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp71C,_tmp71B,({struct Cyc_Absyn_Exp*_tmp223[2U];_tmp223[1U]=_tmp342;_tmp223[0U]=_tmp343;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp223,sizeof(struct Cyc_Absyn_Exp*),2U));}),1,1);});struct _tuple23 _tmp220=_tmp21F;union Cyc_CfFlowInfo_FlowInfo _tmp222;struct Cyc_List_List*_tmp221;_LL105: _tmp222=_tmp220.f1;_tmp221=_tmp220.f2;_LL106:;
outflow=_tmp222;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp221))->tl))->hd;}else{
# 1383
struct _tuple16 _tmp224=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp342);struct _tuple16 _tmp225=_tmp224;union Cyc_CfFlowInfo_FlowInfo _tmp227;void*_tmp226;_LL108: _tmp227=_tmp225.f1;_tmp226=_tmp225.f2;_LL109:;
outflow=_tmp227;
place_val=_tmp226;}
# 1388
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e->topt),1))
return({struct _tuple16 _tmp228;_tmp228.f1=outflow;({void*_tmp71E=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp229=_cycalloc(sizeof(*_tmp229));({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp71D=({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp22A;_tmp22A.tag=5U;_tmp22A.f1=place_val;_tmp22A;});_tmp229[0]=_tmp71D;});_tmp229;});_tmp228.f2=_tmp71E;});_tmp228;});else{
# 1392
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp233=_cycalloc(sizeof(*_tmp233));({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp71F=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp234;_tmp234.tag=1U;_tmp234.f1=_tmp342;_tmp234.f2=(void*)_check_null(e->topt);_tmp234;});_tmp233[0]=_tmp71F;});_tmp233;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp232=_cycalloc(sizeof(*_tmp232));_tmp232->root=root;_tmp232->path=0;_tmp232;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp230=_cycalloc(sizeof(*_tmp230));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp720=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp231;_tmp231.tag=4U;_tmp231.f1=place;_tmp231;});_tmp230[0]=_tmp720;});_tmp230;});
union Cyc_CfFlowInfo_FlowInfo _tmp22B=outflow;struct Cyc_Dict_Dict _tmp22F;struct Cyc_List_List*_tmp22E;if((_tmp22B.BottomFL).tag == 1){_LL10B: _LL10C:
 return({struct _tuple16 _tmp22C;_tmp22C.f1=outflow;_tmp22C.f2=rval;_tmp22C;});}else{_LL10D: _tmp22F=((_tmp22B.ReachableFL).val).f1;_tmp22E=((_tmp22B.ReachableFL).val).f2;_LL10E:
# 1398
 return({struct _tuple16 _tmp22D;({union Cyc_CfFlowInfo_FlowInfo _tmp722=({struct Cyc_Dict_Dict _tmp721=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp22F,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp721,_tmp22E);});_tmp22D.f1=_tmp722;});_tmp22D.f2=rval;_tmp22D;});}_LL10A:;}}case 15U: _LL4E: _tmp344=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_LL4F: {
# 1403
struct _tuple18 _tmp235=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp344);struct _tuple18 _tmp236=_tmp235;union Cyc_CfFlowInfo_FlowInfo _tmp23E;union Cyc_CfFlowInfo_AbsLVal _tmp23D;_LL110: _tmp23E=_tmp236.f1;_tmp23D=_tmp236.f2;_LL111:;{
union Cyc_CfFlowInfo_AbsLVal _tmp237=_tmp23D;struct Cyc_CfFlowInfo_Place*_tmp23C;if((_tmp237.UnknownL).tag == 2){_LL113: _LL114:
 return({struct _tuple16 _tmp238;_tmp238.f1=_tmp23E;_tmp238.f2=_tmp147->notzeroall;_tmp238;});}else{_LL115: _tmp23C=(_tmp237.PlaceL).val;_LL116:
 return({struct _tuple16 _tmp239;_tmp239.f1=_tmp23E;({void*_tmp724=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp23A=_cycalloc(sizeof(*_tmp23A));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp723=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp23B;_tmp23B.tag=4U;_tmp23B.f1=_tmp23C;_tmp23B;});_tmp23A[0]=_tmp723;});_tmp23A;});_tmp239.f2=_tmp724;});_tmp239;});}_LL112:;};}case 20U: _LL50: _tmp345=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_LL51: {
# 1410
struct _tuple16 _tmp23F=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp345);struct _tuple16 _tmp240=_tmp23F;union Cyc_CfFlowInfo_FlowInfo _tmp242;void*_tmp241;_LL118: _tmp242=_tmp240.f1;_tmp241=_tmp240.f2;_LL119:;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp242,_tmp345,_tmp241);}case 21U: _LL52: _tmp347=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp346=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL53: {
# 1414
struct _tuple16 _tmp243=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp347);struct _tuple16 _tmp244=_tmp243;union Cyc_CfFlowInfo_FlowInfo _tmp257;void*_tmp256;_LL11B: _tmp257=_tmp244.f1;_tmp256=_tmp244.f2;_LL11C:;{
void*_tmp245=(void*)_check_null(_tmp347->topt);
# 1417
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp245))
return({struct _tuple16 _tmp246;_tmp246.f1=_tmp257;({void*_tmp725=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),0,_tmp147->unknown_all);_tmp246.f2=_tmp725;});_tmp246;});
if(Cyc_Absyn_is_require_union_type(_tmp245))
Cyc_NewControlFlow_check_union_requires(_tmp347->loc,_tmp245,_tmp346,_tmp257);{
struct _tuple15 _tmp247=Cyc_CfFlowInfo_unname_rval(_tmp256);struct _tuple15 _tmp248=_tmp247;void*_tmp255;_LL11E: _tmp255=_tmp248.f1;_LL11F:;{
void*_tmp249=_tmp255;int _tmp254;int _tmp253;struct _dyneither_ptr _tmp252;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp249)->tag == 6U){_LL121: _tmp254=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp249)->f1).is_union;_tmp253=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp249)->f1).fieldnum;_tmp252=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp249)->f2;_LL122: {
# 1424
int _tmp24A=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp347->topt),_tmp346);
if((_tmp254  && _tmp253 != - 1) && _tmp253 != _tmp24A)
return({struct _tuple16 _tmp24B;_tmp24B.f1=_tmp257;({void*_tmp726=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),1,_tmp147->unknown_none);_tmp24B.f2=_tmp726;});_tmp24B;});
return({struct _tuple16 _tmp24C;_tmp24C.f1=_tmp257;_tmp24C.f2=*((void**)_check_dyneither_subscript(_tmp252,sizeof(void*),_tmp24A));_tmp24C;});}}else{_LL123: _LL124:
# 1429
({void*_tmp24D=0U;({struct Cyc___cycFILE*_tmp728=Cyc_stderr;struct _dyneither_ptr _tmp727=({const char*_tmp24E="the bad rexp is :";_tag_dyneither(_tmp24E,sizeof(char),18U);});Cyc_fprintf(_tmp728,_tmp727,_tag_dyneither(_tmp24D,sizeof(void*),0U));});});
Cyc_CfFlowInfo_print_absrval(_tmp255);
({struct Cyc_String_pa_PrintArg_struct _tmp251;_tmp251.tag=0U;({struct _dyneither_ptr _tmp729=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp251.f1=_tmp729;});({void*_tmp24F[1U]={& _tmp251};({struct _dyneither_ptr _tmp72A=({const char*_tmp250="\nanal_Rexp: AggrMember: %s";_tag_dyneither(_tmp250,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp72A,_tag_dyneither(_tmp24F,sizeof(void*),1U));});});});}_LL120:;};};};}case 38U: _LL54: _tmp349=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp348=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL55: {
# 1437
struct _tuple16 _tmp258=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp349);struct _tuple16 _tmp259=_tmp258;union Cyc_CfFlowInfo_FlowInfo _tmp26A;void*_tmp269;_LL126: _tmp26A=_tmp259.f1;_tmp269=_tmp259.f2;_LL127:;
# 1439
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp349->topt)))
return({struct _tuple16 _tmp25A;_tmp25A.f1=_tmp26A;_tmp25A.f2=_tmp147->unknown_all;_tmp25A;});{
struct _tuple15 _tmp25B=Cyc_CfFlowInfo_unname_rval(_tmp269);struct _tuple15 _tmp25C=_tmp25B;void*_tmp268;_LL129: _tmp268=_tmp25C.f1;_LL12A:;{
void*_tmp25D=_tmp268;int _tmp267;int _tmp266;struct _dyneither_ptr _tmp265;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp25D)->tag == 6U){_LL12C: _tmp267=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp25D)->f1).is_union;_tmp266=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp25D)->f1).fieldnum;_tmp265=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp25D)->f2;_LL12D: {
# 1444
int _tmp25E=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp349->topt),_tmp348);
if(_tmp267  && _tmp266 != - 1){
if(_tmp266 != _tmp25E)
return({struct _tuple16 _tmp25F;_tmp25F.f1=_tmp26A;_tmp25F.f2=_tmp147->zero;_tmp25F;});else{
# 1449
return({struct _tuple16 _tmp260;_tmp260.f1=_tmp26A;_tmp260.f2=_tmp147->notzeroall;_tmp260;});}}else{
# 1451
return({struct _tuple16 _tmp261;_tmp261.f1=_tmp26A;_tmp261.f2=_tmp147->unknown_all;_tmp261;});}}}else{_LL12E: _LL12F:
# 1453
({struct Cyc_String_pa_PrintArg_struct _tmp264;_tmp264.tag=0U;({struct _dyneither_ptr _tmp72B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp264.f1=_tmp72B;});({void*_tmp262[1U]={& _tmp264};({struct _dyneither_ptr _tmp72C=({const char*_tmp263="anal_Rexp: TagCheck_e: %s";_tag_dyneither(_tmp263,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp72C,_tag_dyneither(_tmp262,sizeof(void*),1U));});});});}_LL12B:;};};}case 22U: _LL56: _tmp34B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp34A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL57: {
# 1457
struct _tuple16 _tmp26B=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp34B);struct _tuple16 _tmp26C=_tmp26B;union Cyc_CfFlowInfo_FlowInfo _tmp284;void*_tmp283;_LL131: _tmp284=_tmp26C.f1;_tmp283=_tmp26C.f2;_LL132:;{
# 1460
struct _tuple16 _tmp26D=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp284,_tmp34B,_tmp283);struct _tuple16 _tmp26E=_tmp26D;union Cyc_CfFlowInfo_FlowInfo _tmp282;void*_tmp281;_LL134: _tmp282=_tmp26E.f1;_tmp281=_tmp26E.f2;_LL135:;{
# 1463
void*_tmp26F=Cyc_Tcutil_compress((void*)_check_null(_tmp34B->topt));void*_tmp270=_tmp26F;void*_tmp280;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp270)->tag == 5U){_LL137: _tmp280=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp270)->f1).elt_typ;_LL138:
# 1465
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp280))
# 1467
return({struct _tuple16 _tmp271;_tmp271.f1=_tmp282;({void*_tmp72D=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),0,_tmp147->unknown_all);_tmp271.f2=_tmp72D;});_tmp271;});
# 1469
if(Cyc_Absyn_is_require_union_type(_tmp280))
Cyc_NewControlFlow_check_union_requires(_tmp34B->loc,_tmp280,_tmp34A,_tmp284);{
# 1472
struct _tuple15 _tmp272=Cyc_CfFlowInfo_unname_rval(_tmp281);struct _tuple15 _tmp273=_tmp272;void*_tmp27D;_LL13C: _tmp27D=_tmp273.f1;_LL13D:;{
void*_tmp274=_tmp27D;int _tmp27C;int _tmp27B;struct _dyneither_ptr _tmp27A;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp274)->tag == 6U){_LL13F: _tmp27C=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp274)->f1).is_union;_tmp27B=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp274)->f1).fieldnum;_tmp27A=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp274)->f2;_LL140: {
# 1475
int _tmp275=Cyc_CfFlowInfo_get_field_index(_tmp280,_tmp34A);
if((_tmp27C  && _tmp27B != - 1) && _tmp27B != _tmp275)
return({struct _tuple16 _tmp276;_tmp276.f1=_tmp282;({void*_tmp72E=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),1,_tmp147->unknown_none);_tmp276.f2=_tmp72E;});_tmp276;});
return({struct _tuple16 _tmp277;_tmp277.f1=_tmp282;_tmp277.f2=*((void**)_check_dyneither_subscript(_tmp27A,sizeof(void*),_tmp275));_tmp277;});}}else{_LL141: _LL142:
({void*_tmp278=0U;({struct _dyneither_ptr _tmp72F=({const char*_tmp279="anal_Rexp: AggrArrow";_tag_dyneither(_tmp279,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp72F,_tag_dyneither(_tmp278,sizeof(void*),0U));});});}_LL13E:;};};}else{_LL139: _LL13A:
# 1481
({void*_tmp27E=0U;({struct _dyneither_ptr _tmp730=({const char*_tmp27F="anal_Rexp: AggrArrow ptr";_tag_dyneither(_tmp27F,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp730,_tag_dyneither(_tmp27E,sizeof(void*),0U));});});}_LL136:;};};}case 6U: _LL58: _tmp34E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp34D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_tmp34C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp15F)->f3;_LL59: {
# 1485
struct _tuple19 _tmp285=Cyc_NewControlFlow_anal_test(env,inflow,_tmp34E);struct _tuple19 _tmp286=_tmp285;union Cyc_CfFlowInfo_FlowInfo _tmp28A;union Cyc_CfFlowInfo_FlowInfo _tmp289;_LL144: _tmp28A=_tmp286.f1;_tmp289=_tmp286.f2;_LL145:;{
struct _tuple16 _tmp287=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp28A,_tmp34D);
struct _tuple16 _tmp288=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp289,_tmp34C);
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp147,_tmp287,_tmp288);};}case 7U: _LL5A: _tmp350=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp34F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL5B: {
# 1491
struct _tuple19 _tmp28B=Cyc_NewControlFlow_anal_test(env,inflow,_tmp350);struct _tuple19 _tmp28C=_tmp28B;union Cyc_CfFlowInfo_FlowInfo _tmp296;union Cyc_CfFlowInfo_FlowInfo _tmp295;_LL147: _tmp296=_tmp28C.f1;_tmp295=_tmp28C.f2;_LL148:;{
struct _tuple16 _tmp28D=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp296,_tmp34F);struct _tuple16 _tmp28E=_tmp28D;union Cyc_CfFlowInfo_FlowInfo _tmp294;void*_tmp293;_LL14A: _tmp294=_tmp28E.f1;_tmp293=_tmp28E.f2;_LL14B:;{
struct _tuple16 _tmp28F=({struct _tuple16 _tmp292;_tmp292.f1=_tmp294;_tmp292.f2=_tmp293;_tmp292;});
struct _tuple16 _tmp290=({struct _tuple16 _tmp291;_tmp291.f1=_tmp295;_tmp291.f2=_tmp147->zero;_tmp291;});
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp147,_tmp28F,_tmp290);};};}case 8U: _LL5C: _tmp352=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp351=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL5D: {
# 1498
struct _tuple19 _tmp297=Cyc_NewControlFlow_anal_test(env,inflow,_tmp352);struct _tuple19 _tmp298=_tmp297;union Cyc_CfFlowInfo_FlowInfo _tmp2A2;union Cyc_CfFlowInfo_FlowInfo _tmp2A1;_LL14D: _tmp2A2=_tmp298.f1;_tmp2A1=_tmp298.f2;_LL14E:;{
struct _tuple16 _tmp299=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2A1,_tmp351);struct _tuple16 _tmp29A=_tmp299;union Cyc_CfFlowInfo_FlowInfo _tmp2A0;void*_tmp29F;_LL150: _tmp2A0=_tmp29A.f1;_tmp29F=_tmp29A.f2;_LL151:;{
struct _tuple16 _tmp29B=({struct _tuple16 _tmp29E;_tmp29E.f1=_tmp2A0;_tmp29E.f2=_tmp29F;_tmp29E;});
struct _tuple16 _tmp29C=({struct _tuple16 _tmp29D;_tmp29D.f1=_tmp2A2;_tmp29D.f2=_tmp147->notzeroall;_tmp29D;});
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp147,_tmp29B,_tmp29C);};};}case 23U: _LL5E: _tmp354=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp353=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL5F: {
# 1505
struct _tuple23 _tmp2A3=({struct Cyc_NewControlFlow_AnalEnv*_tmp732=env;union Cyc_CfFlowInfo_FlowInfo _tmp731=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp732,_tmp731,({struct Cyc_Absyn_Exp*_tmp2C2[2U];_tmp2C2[1U]=_tmp353;_tmp2C2[0U]=_tmp354;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2C2,sizeof(struct Cyc_Absyn_Exp*),2U));}),0,1);});struct _tuple23 _tmp2A4=_tmp2A3;union Cyc_CfFlowInfo_FlowInfo _tmp2C1;struct Cyc_List_List*_tmp2C0;_LL153: _tmp2C1=_tmp2A4.f1;_tmp2C0=_tmp2A4.f2;_LL154:;{
# 1509
union Cyc_CfFlowInfo_FlowInfo _tmp2A5=_tmp2C1;
{union Cyc_CfFlowInfo_FlowInfo _tmp2A6=_tmp2C1;struct Cyc_Dict_Dict _tmp2AB;struct Cyc_List_List*_tmp2AA;if((_tmp2A6.ReachableFL).tag == 2){_LL156: _tmp2AB=((_tmp2A6.ReachableFL).val).f1;_tmp2AA=((_tmp2A6.ReachableFL).val).f2;_LL157:
# 1514
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2AB,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp2C0))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp2A7=0U;({unsigned int _tmp734=_tmp353->loc;struct _dyneither_ptr _tmp733=({const char*_tmp2A8="expression may not be initialized";_tag_dyneither(_tmp2A8,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp734,_tmp733,_tag_dyneither(_tmp2A7,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp2A9=Cyc_NewControlFlow_add_subscript_reln(_tmp2AA,_tmp354,_tmp353);
if(_tmp2AA != _tmp2A9)
({union Cyc_CfFlowInfo_FlowInfo _tmp735=Cyc_CfFlowInfo_ReachableFL(_tmp2AB,_tmp2A9);_tmp2A5=_tmp735;});
goto _LL155;};}else{_LL158: _LL159:
 goto _LL155;}_LL155:;}{
# 1522
void*_tmp2AC=Cyc_Tcutil_compress((void*)_check_null(_tmp354->topt));void*_tmp2AD=_tmp2AC;struct Cyc_List_List*_tmp2BF;switch(*((int*)_tmp2AD)){case 10U: _LL15B: _tmp2BF=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2AD)->f1;_LL15C: {
# 1524
struct _tuple15 _tmp2AE=Cyc_CfFlowInfo_unname_rval((void*)((struct Cyc_List_List*)_check_null(_tmp2C0))->hd);struct _tuple15 _tmp2AF=_tmp2AE;void*_tmp2B5;_LL162: _tmp2B5=_tmp2AF.f1;_LL163:;{
void*_tmp2B0=_tmp2B5;struct _dyneither_ptr _tmp2B4;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2B0)->tag == 6U){_LL165: _tmp2B4=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2B0)->f2;_LL166: {
# 1527
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp353)).f1;
return({struct _tuple16 _tmp2B1;_tmp2B1.f1=_tmp2A5;_tmp2B1.f2=*((void**)_check_dyneither_subscript(_tmp2B4,sizeof(void*),(int)i));_tmp2B1;});}}else{_LL167: _LL168:
({void*_tmp2B2=0U;({struct _dyneither_ptr _tmp736=({const char*_tmp2B3="anal_Rexp: Subscript";_tag_dyneither(_tmp2B3,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp736,_tag_dyneither(_tmp2B2,sizeof(void*),0U));});});}_LL164:;};}case 5U: _LL15D: _LL15E: {
# 1532
struct _tuple16 _tmp2B6=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2C1,_tmp354,(void*)((struct Cyc_List_List*)_check_null(_tmp2C0))->hd);struct _tuple16 _tmp2B7=_tmp2B6;union Cyc_CfFlowInfo_FlowInfo _tmp2BC;void*_tmp2BB;_LL16A: _tmp2BC=_tmp2B7.f1;_tmp2BB=_tmp2B7.f2;_LL16B:;{
union Cyc_CfFlowInfo_FlowInfo _tmp2B8=_tmp2BC;if((_tmp2B8.BottomFL).tag == 1){_LL16D: _LL16E:
 return({struct _tuple16 _tmp2B9;_tmp2B9.f1=_tmp2BC;_tmp2B9.f2=_tmp2BB;_tmp2B9;});}else{_LL16F: _LL170:
 return({struct _tuple16 _tmp2BA;_tmp2BA.f1=_tmp2A5;_tmp2BA.f2=_tmp2BB;_tmp2BA;});}_LL16C:;};}default: _LL15F: _LL160:
# 1537
({void*_tmp2BD=0U;({struct _dyneither_ptr _tmp737=({const char*_tmp2BE="anal_Rexp: Subscript -- bad type";_tag_dyneither(_tmp2BE,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp737,_tag_dyneither(_tmp2BD,sizeof(void*),0U));});});}_LL15A:;};};}case 24U: _LL62: _tmp355=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_LL63: {
# 1543
struct _tuple23 _tmp2C3=Cyc_NewControlFlow_anal_Rexps(env,inflow,_tmp355,1,1);struct _tuple23 _tmp2C4=_tmp2C3;union Cyc_CfFlowInfo_FlowInfo _tmp2CF;struct Cyc_List_List*_tmp2CE;_LL172: _tmp2CF=_tmp2C4.f1;_tmp2CE=_tmp2C4.f2;_LL173:;{
struct _dyneither_ptr aggrdict=({unsigned int _tmp2C9=(unsigned int)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp355);void**_tmp2CA=(void**)_cycalloc(_check_times(sizeof(void*),_tmp2C9));struct _dyneither_ptr _tmp2CD=_tag_dyneither(_tmp2CA,sizeof(void*),_tmp2C9);{unsigned int _tmp2CB=_tmp2C9;unsigned int i;for(i=0;i < _tmp2CB;i ++){({typeof((void*)({
void*_tmp2CC=(void*)((struct Cyc_List_List*)_check_null(_tmp2CE))->hd;
_tmp2CE=_tmp2CE->tl;
_tmp2CC;}))_tmp738=(void*)({
# 1546
void*_tmp2CC=(void*)((struct Cyc_List_List*)_check_null(_tmp2CE))->hd;
_tmp2CE=_tmp2CE->tl;
_tmp2CC;});_tmp2CA[i]=_tmp738;});}}_tmp2CD;});
# 1550
return({struct _tuple16 _tmp2C5;_tmp2C5.f1=_tmp2CF;({void*_tmp73B=(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2C6=_cycalloc(sizeof(*_tmp2C6));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp73A=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2C7;_tmp2C7.tag=6U;({struct Cyc_CfFlowInfo_UnionRInfo _tmp739=({struct Cyc_CfFlowInfo_UnionRInfo _tmp2C8;_tmp2C8.is_union=0;_tmp2C8.fieldnum=- 1;_tmp2C8;});_tmp2C7.f1=_tmp739;});_tmp2C7.f2=aggrdict;_tmp2C7;});_tmp2C6[0]=_tmp73A;});_tmp2C6;});_tmp2C5.f2=_tmp73B;});_tmp2C5;});};}case 30U: _LL64: _tmp356=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_LL65: {
# 1553
struct Cyc_List_List*fs;
{void*_tmp2D0=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2D1=_tmp2D0;struct Cyc_List_List*_tmp2D4;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D1)->tag == 12U){_LL175: _tmp2D4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D1)->f2;_LL176:
 fs=_tmp2D4;goto _LL174;}else{_LL177: _LL178:
({void*_tmp2D2=0U;({struct _dyneither_ptr _tmp73C=({const char*_tmp2D3="anal_Rexp:anon struct has bad type";_tag_dyneither(_tmp2D3,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp73C,_tag_dyneither(_tmp2D2,sizeof(void*),0U));});});}_LL174:;}
# 1558
_tmp359=_tmp356;_tmp358=Cyc_Absyn_StructA;_tmp357=fs;goto _LL67;}case 29U: if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp15F)->f4 != 0){if(((struct Cyc_Absyn_Aggrdecl*)((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp15F)->f4)->impl != 0){_LL66: _tmp359=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp15F)->f3;_tmp358=(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp15F)->f4)->kind;_tmp357=((((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp15F)->f4)->impl)->fields;_LL67: {
# 1560
void*exp_type=(void*)_check_null(e->topt);
struct _tuple23 _tmp2D5=({struct Cyc_NewControlFlow_AnalEnv*_tmp73E=env;union Cyc_CfFlowInfo_FlowInfo _tmp73D=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp73E,_tmp73D,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp359),1,1);});struct _tuple23 _tmp2D6=_tmp2D5;union Cyc_CfFlowInfo_FlowInfo _tmp2E3;struct Cyc_List_List*_tmp2E2;_LL17A: _tmp2E3=_tmp2D6.f1;_tmp2E2=_tmp2D6.f2;_LL17B:;{
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp147,_tmp357,_tmp358 == Cyc_Absyn_UnionA,_tmp147->unknown_all);
# 1565
int field_no=-1;
{int i=0;for(0;_tmp2E2 != 0;(((_tmp2E2=_tmp2E2->tl,_tmp359=_tmp359->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp359))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp2D7=(void*)ds->hd;void*_tmp2D8=_tmp2D7;struct _dyneither_ptr*_tmp2DC;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2D8)->tag == 0U){_LL17D: _LL17E:
({void*_tmp2D9=0U;({struct _dyneither_ptr _tmp73F=({const char*_tmp2DA="anal_Rexp:Aggregate_e";_tag_dyneither(_tmp2DA,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp73F,_tag_dyneither(_tmp2D9,sizeof(void*),0U));});});}else{_LL17F: _tmp2DC=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2D8)->f1;_LL180:
# 1572
({int _tmp740=Cyc_CfFlowInfo_get_field_index_fs(_tmp357,_tmp2DC);field_no=_tmp740;});
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp2E2->hd;
# 1575
if(_tmp358 == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp2DB=(*((struct _tuple26*)_tmp359->hd)).f2;
({union Cyc_CfFlowInfo_FlowInfo _tmp741=Cyc_NewControlFlow_use_Rval(env,_tmp2DB->loc,_tmp2E3,(void*)_tmp2E2->hd);_tmp2E3=_tmp741;});
# 1579
Cyc_NewControlFlow_check_union_requires(_tmp2DB->loc,exp_type,_tmp2DC,_tmp2E3);}}_LL17C:;}}}{
# 1582
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2DD=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2DF=_cycalloc(sizeof(*_tmp2DF));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp743=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2E0;_tmp2E0.tag=6U;({struct Cyc_CfFlowInfo_UnionRInfo _tmp742=({struct Cyc_CfFlowInfo_UnionRInfo _tmp2E1;_tmp2E1.is_union=_tmp358 == Cyc_Absyn_UnionA;_tmp2E1.fieldnum=field_no;_tmp2E1;});_tmp2E0.f1=_tmp742;});_tmp2E0.f2=aggrdict;_tmp2E0;});_tmp2DF[0]=_tmp743;});_tmp2DF;});
return({struct _tuple16 _tmp2DE;_tmp2DE.f1=_tmp2E3;_tmp2DE.f2=(void*)_tmp2DD;_tmp2DE;});};};}}else{goto _LL68;}}else{_LL68: _LL69:
# 1586
({struct Cyc_String_pa_PrintArg_struct _tmp2E6;_tmp2E6.tag=0U;({struct _dyneither_ptr _tmp744=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp2E6.f1=_tmp744;});({void*_tmp2E4[1U]={& _tmp2E6};({struct _dyneither_ptr _tmp745=({const char*_tmp2E5="anal_Rexp:missing aggrdeclimpl in %s";_tag_dyneither(_tmp2E5,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp745,_tag_dyneither(_tmp2E4,sizeof(void*),1U));});});});}case 26U: _LL6A: _tmp35A=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_LL6B: {
# 1588
struct Cyc_List_List*_tmp2E7=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp35A);
struct _tuple23 _tmp2E8=Cyc_NewControlFlow_anal_Rexps(env,inflow,_tmp2E7,1,1);struct _tuple23 _tmp2E9=_tmp2E8;union Cyc_CfFlowInfo_FlowInfo _tmp2EC;struct Cyc_List_List*_tmp2EB;_LL182: _tmp2EC=_tmp2E9.f1;_tmp2EB=_tmp2E9.f2;_LL183:;
for(0;_tmp2EB != 0;(_tmp2EB=_tmp2EB->tl,_tmp2E7=_tmp2E7->tl)){
({union Cyc_CfFlowInfo_FlowInfo _tmp746=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2E7))->hd)->loc,_tmp2EC,(void*)_tmp2EB->hd);_tmp2EC=_tmp746;});}
return({struct _tuple16 _tmp2EA;_tmp2EA.f1=_tmp2EC;({void*_tmp747=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,(void*)_check_null(e->topt),0,_tmp147->unknown_all);_tmp2EA.f2=_tmp747;});_tmp2EA;});}case 27U: _LL6C: _tmp35E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp35D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_tmp35C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp15F)->f3;_tmp35B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp15F)->f4;_LL6D: {
# 1596
struct _tuple16 _tmp2ED=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp35D);struct _tuple16 _tmp2EE=_tmp2ED;union Cyc_CfFlowInfo_FlowInfo _tmp311;void*_tmp310;_LL185: _tmp311=_tmp2EE.f1;_tmp310=_tmp2EE.f2;_LL186:;{
union Cyc_CfFlowInfo_FlowInfo _tmp2EF=_tmp311;struct Cyc_Dict_Dict _tmp30F;struct Cyc_List_List*_tmp30E;if((_tmp2EF.BottomFL).tag == 1){_LL188: _LL189:
 return({struct _tuple16 _tmp2F0;_tmp2F0.f1=_tmp311;_tmp2F0.f2=_tmp147->unknown_all;_tmp2F0;});}else{_LL18A: _tmp30F=((_tmp2EF.ReachableFL).val).f1;_tmp30E=((_tmp2EF.ReachableFL).val).f2;_LL18B:
# 1600
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp30F,_tmp310)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp2F1=0U;({unsigned int _tmp749=_tmp35D->loc;struct _dyneither_ptr _tmp748=({const char*_tmp2F2="expression may not be initialized";_tag_dyneither(_tmp2F2,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp749,_tmp748,_tag_dyneither(_tmp2F1,sizeof(void*),0U));});});{
# 1604
struct Cyc_List_List*new_relns=_tmp30E;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(_tmp35E);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(_tmp35D,& n2))
({struct Cyc_List_List*_tmp74A=Cyc_Relations_add_relation(Cyc_Core_heap_region,n1,Cyc_Relations_Rlt,n2,_tmp30E);new_relns=_tmp74A;});
if(_tmp30E != new_relns)
({union Cyc_CfFlowInfo_FlowInfo _tmp74B=Cyc_CfFlowInfo_ReachableFL(_tmp30F,new_relns);_tmp311=_tmp74B;});{
# 1613
void*_tmp2F3=_tmp310;switch(*((int*)_tmp2F3)){case 0U: _LL18D: _LL18E:
 return({struct _tuple16 _tmp2F4;_tmp2F4.f1=_tmp311;_tmp2F4.f2=_tmp147->unknown_all;_tmp2F4;});case 1U: _LL18F: _LL190:
 goto _LL192;case 4U: _LL191: _LL192: {
# 1617
struct Cyc_List_List _tmp2F5=({struct Cyc_List_List _tmp300;_tmp300.hd=_tmp35E;_tmp300.tl=0;_tmp300;});
({union Cyc_CfFlowInfo_FlowInfo _tmp74C=Cyc_NewControlFlow_add_vars(_tmp147,_tmp311,& _tmp2F5,_tmp147->unknown_all,e->loc,0);_tmp311=_tmp74C;});{
# 1620
struct _tuple16 _tmp2F6=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp311,_tmp35C);struct _tuple16 _tmp2F7=_tmp2F6;union Cyc_CfFlowInfo_FlowInfo _tmp2FF;void*_tmp2FE;_LL196: _tmp2FF=_tmp2F7.f1;_tmp2FE=_tmp2F7.f2;_LL197:;
{union Cyc_CfFlowInfo_FlowInfo _tmp2F8=_tmp2FF;struct Cyc_Dict_Dict _tmp2FD;if((_tmp2F8.BottomFL).tag == 1){_LL199: _LL19A:
 return({struct _tuple16 _tmp2F9;_tmp2F9.f1=_tmp2FF;_tmp2F9.f2=_tmp147->unknown_all;_tmp2F9;});}else{_LL19B: _tmp2FD=((_tmp2F8.ReachableFL).val).f1;_LL19C:
# 1624
 if(Cyc_CfFlowInfo_initlevel(_tmp147,_tmp2FD,_tmp2FE)!= Cyc_CfFlowInfo_AllIL){
({void*_tmp2FA=0U;({unsigned int _tmp74E=_tmp35D->loc;struct _dyneither_ptr _tmp74D=({const char*_tmp2FB="expression may not be initialized";_tag_dyneither(_tmp2FB,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp74E,_tmp74D,_tag_dyneither(_tmp2FA,sizeof(void*),0U));});});
return({struct _tuple16 _tmp2FC;({union Cyc_CfFlowInfo_FlowInfo _tmp74F=Cyc_CfFlowInfo_BottomFL();_tmp2FC.f1=_tmp74F;});_tmp2FC.f2=_tmp147->unknown_all;_tmp2FC;});}}_LL198:;}
# 1629
_tmp311=_tmp2FF;
goto _LL194;};}default: _LL193: _LL194:
# 1632
 while(1){
struct Cyc_List_List _tmp301=({struct Cyc_List_List _tmp30C;_tmp30C.hd=_tmp35E;_tmp30C.tl=0;_tmp30C;});
({union Cyc_CfFlowInfo_FlowInfo _tmp750=Cyc_NewControlFlow_add_vars(_tmp147,_tmp311,& _tmp301,_tmp147->unknown_all,e->loc,0);_tmp311=_tmp750;});{
struct _tuple16 _tmp302=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp311,_tmp35C);struct _tuple16 _tmp303=_tmp302;union Cyc_CfFlowInfo_FlowInfo _tmp30B;void*_tmp30A;_LL19E: _tmp30B=_tmp303.f1;_tmp30A=_tmp303.f2;_LL19F:;
{union Cyc_CfFlowInfo_FlowInfo _tmp304=_tmp30B;struct Cyc_Dict_Dict _tmp308;if((_tmp304.BottomFL).tag == 1){_LL1A1: _LL1A2:
 goto _LL1A0;}else{_LL1A3: _tmp308=((_tmp304.ReachableFL).val).f1;_LL1A4:
# 1639
 if(Cyc_CfFlowInfo_initlevel(_tmp147,_tmp308,_tmp30A)!= Cyc_CfFlowInfo_AllIL){
({void*_tmp305=0U;({unsigned int _tmp752=_tmp35D->loc;struct _dyneither_ptr _tmp751=({const char*_tmp306="expression may not be initialized";_tag_dyneither(_tmp306,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp752,_tmp751,_tag_dyneither(_tmp305,sizeof(void*),0U));});});
return({struct _tuple16 _tmp307;({union Cyc_CfFlowInfo_FlowInfo _tmp753=Cyc_CfFlowInfo_BottomFL();_tmp307.f1=_tmp753;});_tmp307.f2=_tmp147->unknown_all;_tmp307;});}}_LL1A0:;}{
# 1644
union Cyc_CfFlowInfo_FlowInfo _tmp309=Cyc_CfFlowInfo_join_flow(_tmp147,_tmp311,_tmp30B);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp309,_tmp311))
break;
_tmp311=_tmp309;};};}
# 1649
return({struct _tuple16 _tmp30D;_tmp30D.f1=_tmp311;_tmp30D.f2=_tmp147->unknown_all;_tmp30D;});}_LL18C:;};};}_LL187:;};}case 28U: _LL6E: _tmp361=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_tmp360=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp15F)->f2;_tmp35F=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp15F)->f3;_LL6F: {
# 1655
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp31D=_cycalloc(sizeof(*_tmp31D));({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp754=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp31E;_tmp31E.tag=1U;_tmp31E.f1=_tmp361;_tmp31E.f2=(void*)_check_null(e->topt);_tmp31E;});_tmp31D[0]=_tmp754;});_tmp31D;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp31C=_cycalloc(sizeof(*_tmp31C));_tmp31C->root=root;_tmp31C->path=0;_tmp31C;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp31A=_cycalloc(sizeof(*_tmp31A));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp755=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp31B;_tmp31B.tag=4U;_tmp31B.f1=place;_tmp31B;});_tmp31A[0]=_tmp755;});_tmp31A;});
void*place_val;
# 1663
({void*_tmp756=Cyc_CfFlowInfo_typ_to_absrval(_tmp147,_tmp360,0,_tmp147->unknown_none);place_val=_tmp756;});{
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple16 _tmp312=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp361);struct _tuple16 _tmp313=_tmp312;union Cyc_CfFlowInfo_FlowInfo _tmp319;_LL1A6: _tmp319=_tmp313.f1;_LL1A7:;
outflow=_tmp319;{
union Cyc_CfFlowInfo_FlowInfo _tmp314=outflow;struct Cyc_Dict_Dict _tmp318;struct Cyc_List_List*_tmp317;if((_tmp314.BottomFL).tag == 1){_LL1A9: _LL1AA:
 return({struct _tuple16 _tmp315;_tmp315.f1=outflow;_tmp315.f2=rval;_tmp315;});}else{_LL1AB: _tmp318=((_tmp314.ReachableFL).val).f1;_tmp317=((_tmp314.ReachableFL).val).f2;_LL1AC:
# 1670
 return({struct _tuple16 _tmp316;({union Cyc_CfFlowInfo_FlowInfo _tmp758=({struct Cyc_Dict_Dict _tmp757=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp318,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp757,_tmp317);});_tmp316.f1=_tmp758;});_tmp316.f2=rval;_tmp316;});}_LL1A8:;};};}case 37U: _LL70: _tmp362=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp15F)->f1;_LL71: {
# 1674
struct _tuple17 _tmp31F=({struct _tuple17 _tmp322;_tmp322.f1=(env->fenv)->unknown_all;_tmp322.f2=copy_ctxt;_tmp322;});
union Cyc_CfFlowInfo_FlowInfo _tmp320=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp362,& _tmp31F);
return({struct _tuple16 _tmp321;_tmp321.f1=_tmp320;_tmp321.f2=_tmp31F.f1;_tmp321;});}case 36U: _LL74: _LL75:
# 1679
 goto _LL77;case 25U: _LL76: _LL77:
 goto _LL79;case 39U: _LL78: _LL79:
 goto _LL7B;default: _LL7A: _LL7B:
({void*_tmp323=0U;({struct _dyneither_ptr _tmp759=({const char*_tmp324="anal_Rexp, unexpected exp form";_tag_dyneither(_tmp324,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp759,_tag_dyneither(_tmp323,sizeof(void*),0U));});});}_LLD:;};}
# 1694 "new_control_flow.cyc"
static struct _tuple18 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,int expanded_place_possibly_null,struct Cyc_List_List*path){
# 1703
struct Cyc_CfFlowInfo_FlowEnv*_tmp363=env->fenv;
void*_tmp364=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp365=_tmp364;union Cyc_Absyn_Constraint*_tmp395;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp365)->tag == 5U){_LL1: _tmp395=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp365)->f1).ptr_atts).bounds;_LL2: {
# 1706
union Cyc_CfFlowInfo_FlowInfo _tmp366=f;struct Cyc_Dict_Dict _tmp392;struct Cyc_List_List*_tmp391;if((_tmp366.BottomFL).tag == 1){_LL6: _LL7:
 return({struct _tuple18 _tmp367;_tmp367.f1=f;({union Cyc_CfFlowInfo_AbsLVal _tmp75A=Cyc_CfFlowInfo_UnknownL();_tmp367.f2=_tmp75A;});_tmp367;});}else{_LL8: _tmp392=((_tmp366.ReachableFL).val).f1;_tmp391=((_tmp366.ReachableFL).val).f2;_LL9: {
# 1710
struct _tuple15 _tmp368=Cyc_CfFlowInfo_unname_rval(r);struct _tuple15 _tmp369=_tmp368;void*_tmp390;struct Cyc_List_List*_tmp38F;_LLB: _tmp390=_tmp369.f1;_tmp38F=_tmp369.f2;_LLC:;
retry: {void*_tmp36A=_tmp390;void*_tmp385;enum Cyc_CfFlowInfo_InitLevel _tmp384;void*_tmp383;struct Cyc_List_List*_tmp382;void*_tmp381;switch(*((int*)_tmp36A)){case 8U: _LLE: _LLF:
# 1713
({void*_tmp36B=0U;({struct _dyneither_ptr _tmp75B=({const char*_tmp36C="named location in anal_derefL";_tag_dyneither(_tmp36C,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp75B,_tag_dyneither(_tmp36B,sizeof(void*),0U));});});case 1U: _LL10: _LL11:
# 1715
({void*_tmp75D=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp36D=_cycalloc(sizeof(*_tmp36D));({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp75C=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp36E;_tmp36E.tag=Cyc_CfFlowInfo_NotZero;_tmp36E.f1=_tmp391;_tmp36E;});_tmp36D[0]=_tmp75C;});_tmp36D;});e->annot=_tmp75D;});goto _LLD;case 5U: _LL12: _tmp381=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp36A)->f1;_LL13:
# 1718
 if(expanded_place_possibly_null)
# 1721
({void*_tmp75F=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp36F=_cycalloc(sizeof(*_tmp36F));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp75E=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp370;_tmp370.tag=Cyc_CfFlowInfo_UnknownZ;_tmp370.f1=_tmp391;_tmp370;});_tmp36F[0]=_tmp75E;});_tmp36F;});e->annot=_tmp75F;});else{
# 1724
void*_tmp371=e->annot;void*_tmp372=_tmp371;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp372)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1F: _LL20:
# 1728
({void*_tmp761=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp373=_cycalloc(sizeof(*_tmp373));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp760=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp374;_tmp374.tag=Cyc_CfFlowInfo_UnknownZ;_tmp374.f1=_tmp391;_tmp374;});_tmp373[0]=_tmp760;});_tmp373;});e->annot=_tmp761;});
goto _LL1E;}else{_LL21: _LL22:
# 1731
({void*_tmp763=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp375=_cycalloc(sizeof(*_tmp375));({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp762=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp376;_tmp376.tag=Cyc_CfFlowInfo_NotZero;_tmp376.f1=_tmp391;_tmp376;});_tmp375[0]=_tmp762;});_tmp375;});e->annot=_tmp763;});
goto _LL1E;}_LL1E:;}
# 1736
if(Cyc_Tcutil_is_bound_one(_tmp395))return({struct _tuple18 _tmp377;_tmp377.f1=f;({union Cyc_CfFlowInfo_AbsLVal _tmp764=Cyc_CfFlowInfo_UnknownL();_tmp377.f2=_tmp764;});_tmp377;});
goto _LLD;case 4U: _LL14: _tmp383=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp36A)->f1)->root;_tmp382=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp36A)->f1)->path;_LL15:
# 1740
({void*_tmp766=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp378=_cycalloc(sizeof(*_tmp378));({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp765=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp379;_tmp379.tag=Cyc_CfFlowInfo_NotZero;_tmp379.f1=_tmp391;_tmp379;});_tmp378[0]=_tmp765;});_tmp378;});e->annot=_tmp766;});
if(Cyc_Tcutil_is_bound_one(_tmp395))
return({struct _tuple18 _tmp37A;_tmp37A.f1=f;({union Cyc_CfFlowInfo_AbsLVal _tmp768=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp37B=_cycalloc(sizeof(*_tmp37B));_tmp37B->root=_tmp383;({struct Cyc_List_List*_tmp767=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp382,path);_tmp37B->path=_tmp767;});_tmp37B;}));_tmp37A.f2=_tmp768;});_tmp37A;});
goto _LLD;case 0U: _LL16: _LL17:
# 1745
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple18 _tmp37C;({union Cyc_CfFlowInfo_FlowInfo _tmp769=Cyc_CfFlowInfo_BottomFL();_tmp37C.f1=_tmp769;});({union Cyc_CfFlowInfo_AbsLVal _tmp76A=Cyc_CfFlowInfo_UnknownL();_tmp37C.f2=_tmp76A;});_tmp37C;});case 2U: _LL18: _tmp384=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp36A)->f1;_LL19:
# 1750
 if(Cyc_Tcutil_is_bound_one(_tmp395))
({union Cyc_CfFlowInfo_FlowInfo _tmp76B=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp384,_tmp38F);f=_tmp76B;});
({void*_tmp76D=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp37D=_cycalloc(sizeof(*_tmp37D));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp76C=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp37E;_tmp37E.tag=Cyc_CfFlowInfo_UnknownZ;_tmp37E.f1=_tmp391;_tmp37E;});_tmp37D[0]=_tmp76C;});_tmp37D;});e->annot=_tmp76D;});
goto _LLD;case 7U: _LL1A: _tmp385=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp36A)->f3;_LL1B:
# 1755
 if(passthrough_consumes){
_tmp390=_tmp385;goto retry;}
# 1758
goto _LL1D;default: _LL1C: _LL1D:
# 1760
({void*_tmp76F=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp37F=_cycalloc(sizeof(*_tmp37F));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp76E=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp380;_tmp380.tag=Cyc_CfFlowInfo_UnknownZ;_tmp380.f1=_tmp391;_tmp380;});_tmp37F[0]=_tmp76E;});_tmp37F;});e->annot=_tmp76F;});
goto _LLD;}_LLD:;}
# 1763
if(Cyc_CfFlowInfo_initlevel(_tmp363,_tmp392,_tmp390)== Cyc_CfFlowInfo_NoneIL){
struct _tuple15 _tmp386=Cyc_CfFlowInfo_unname_rval(_tmp390);struct _tuple15 _tmp387=_tmp386;void*_tmp38D;_LL24: _tmp38D=_tmp387.f1;_LL25:;{
void*_tmp388=_tmp38D;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp388)->tag == 7U){_LL27: _LL28:
# 1767
({void*_tmp389=0U;({unsigned int _tmp771=e->loc;struct _dyneither_ptr _tmp770=({const char*_tmp38A="attempt to dereference an alias-free that has already been copied";_tag_dyneither(_tmp38A,sizeof(char),66U);});Cyc_CfFlowInfo_aerr(_tmp771,_tmp770,_tag_dyneither(_tmp389,sizeof(void*),0U));});});
goto _LL26;}else{_LL29: _LL2A:
# 1770
({void*_tmp38B=0U;({unsigned int _tmp773=e->loc;struct _dyneither_ptr _tmp772=({const char*_tmp38C="dereference of possibly uninitialized pointer";_tag_dyneither(_tmp38C,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp773,_tmp772,_tag_dyneither(_tmp38B,sizeof(void*),0U));});});
goto _LL26;}_LL26:;};}
# 1774
return({struct _tuple18 _tmp38E;_tmp38E.f1=f;({union Cyc_CfFlowInfo_AbsLVal _tmp774=Cyc_CfFlowInfo_UnknownL();_tmp38E.f2=_tmp774;});_tmp38E;});}}_LL5:;}}else{_LL3: _LL4:
# 1776
({void*_tmp393=0U;({struct _dyneither_ptr _tmp775=({const char*_tmp394="left deref of non-pointer-type";_tag_dyneither(_tmp394,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp775,_tag_dyneither(_tmp393,sizeof(void*),0U));});});}_LL0:;}
# 1782
static struct Cyc_CfFlowInfo_Place*Cyc_NewControlFlow_make_expanded_place(struct Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*path){
struct Cyc_CfFlowInfo_Place*_tmp396=p;void*_tmp39C;struct Cyc_List_List*_tmp39B;_LL1: _tmp39C=_tmp396->root;_tmp39B=_tmp396->path;_LL2:;
({struct Cyc_List_List*_tmp778=({struct Cyc_List_List*_tmp397=_cycalloc(sizeof(*_tmp397));({void*_tmp777=(void*)({struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*_tmp398=_cycalloc_atomic(sizeof(*_tmp398));({struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct _tmp776=({struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct _tmp399;_tmp399.tag=1U;_tmp399;});_tmp398[0]=_tmp776;});_tmp398;});_tmp397->hd=_tmp777;});_tmp397->tl=path;_tmp397;});path=_tmp778;});
return({struct Cyc_CfFlowInfo_Place*_tmp39A=_cycalloc(sizeof(*_tmp39A));_tmp39A->root=_tmp39C;({struct Cyc_List_List*_tmp779=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp39B,path);_tmp39A->path=_tmp779;});_tmp39A;});}
# 1792
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*path){
# 1796
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp39D=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp39E=inflow;struct Cyc_Dict_Dict _tmp3A1;struct Cyc_List_List*_tmp3A0;if((_tmp39E.BottomFL).tag == 1){_LL1: _LL2:
 return({struct _tuple18 _tmp39F;({union Cyc_CfFlowInfo_FlowInfo _tmp77A=Cyc_CfFlowInfo_BottomFL();_tmp39F.f1=_tmp77A;});({union Cyc_CfFlowInfo_AbsLVal _tmp77B=Cyc_CfFlowInfo_UnknownL();_tmp39F.f2=_tmp77B;});_tmp39F;});}else{_LL3: _tmp3A1=((_tmp39E.ReachableFL).val).f1;_tmp3A0=((_tmp39E.ReachableFL).val).f2;_LL4:
# 1801
 d=_tmp3A1;}_LL0:;}{
# 1805
void*_tmp3A2=e->r;void*_tmp3A3=_tmp3A2;struct Cyc_Absyn_Exp*_tmp40A;struct _dyneither_ptr*_tmp409;struct Cyc_Absyn_Exp*_tmp408;struct Cyc_Absyn_Exp*_tmp407;struct Cyc_Absyn_Exp*_tmp406;struct Cyc_Absyn_Exp*_tmp405;struct _dyneither_ptr*_tmp404;struct Cyc_Absyn_Vardecl*_tmp403;struct Cyc_Absyn_Vardecl*_tmp402;struct Cyc_Absyn_Vardecl*_tmp401;struct Cyc_Absyn_Vardecl*_tmp400;struct Cyc_Absyn_Exp*_tmp3FF;struct Cyc_Absyn_Exp*_tmp3FE;struct Cyc_Absyn_Exp*_tmp3FD;switch(*((int*)_tmp3A3)){case 14U: _LL6: _tmp3FD=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3A3)->f2;_LL7:
 _tmp3FE=_tmp3FD;goto _LL9;case 12U: _LL8: _tmp3FE=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp3A3)->f1;_LL9:
 _tmp3FF=_tmp3FE;goto _LLB;case 13U: _LLA: _tmp3FF=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp3A3)->f1;_LLB:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp3FF,path);case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A3)->f1)){case 3U: _LLC: _tmp400=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A3)->f1)->f1;_LLD:
# 1810
 _tmp401=_tmp400;goto _LLF;case 4U: _LLE: _tmp401=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A3)->f1)->f1;_LLF:
 _tmp402=_tmp401;goto _LL11;case 5U: _LL10: _tmp402=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A3)->f1)->f1;_LL11:
# 1813
 return({struct _tuple18 _tmp3A4;_tmp3A4.f1=inflow;({union Cyc_CfFlowInfo_AbsLVal _tmp77E=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));({void*_tmp77D=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp77C=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp3A7;_tmp3A7.tag=0U;_tmp3A7.f1=_tmp402;_tmp3A7;});_tmp3A6[0]=_tmp77C;});_tmp3A6;});_tmp3A5->root=_tmp77D;});_tmp3A5->path=path;_tmp3A5;}));_tmp3A4.f2=_tmp77E;});_tmp3A4;});case 1U: _LL12: _tmp403=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3A3)->f1)->f1;_LL13:
# 1815
 return({struct _tuple18 _tmp3A8;_tmp3A8.f1=inflow;({union Cyc_CfFlowInfo_AbsLVal _tmp77F=Cyc_CfFlowInfo_UnknownL();_tmp3A8.f2=_tmp77F;});_tmp3A8;});default: goto _LL1C;}case 22U: _LL14: _tmp405=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3A3)->f1;_tmp404=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3A3)->f2;_LL15:
# 1818
{void*_tmp3A9=Cyc_Tcutil_compress((void*)_check_null(_tmp405->topt));void*_tmp3AA=_tmp3A9;void*_tmp3B0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AA)->tag == 5U){_LL1F: _tmp3B0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AA)->f1).elt_typ;_LL20:
# 1820
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp3B0)){
Cyc_NewControlFlow_check_union_requires(_tmp405->loc,_tmp3B0,_tmp404,inflow);
({struct Cyc_List_List*_tmp783=({struct Cyc_List_List*_tmp3AB=_cycalloc(sizeof(*_tmp3AB));({void*_tmp782=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp3AC=_cycalloc_atomic(sizeof(*_tmp3AC));({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp781=({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp3AD;_tmp3AD.tag=0U;({int _tmp780=Cyc_CfFlowInfo_get_field_index(_tmp3B0,_tmp404);_tmp3AD.f1=_tmp780;});_tmp3AD;});_tmp3AC[0]=_tmp781;});_tmp3AC;});_tmp3AB->hd=_tmp782;});_tmp3AB->tl=path;_tmp3AB;});path=_tmp783;});}
# 1824
goto _LL1E;}else{_LL21: _LL22:
({void*_tmp3AE=0U;({struct _dyneither_ptr _tmp784=({const char*_tmp3AF="anal_Lexp: AggrArrow ptr";_tag_dyneither(_tmp3AF,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp784,_tag_dyneither(_tmp3AE,sizeof(void*),0U));});});}_LL1E:;}
# 1827
_tmp406=_tmp405;goto _LL17;case 20U: _LL16: _tmp406=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3A3)->f1;_LL17:
# 1831
 if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp406->topt),1)){
# 1833
struct _tuple18 _tmp3B1=Cyc_NewControlFlow_anal_Lexp(env,inflow,expand_unique,passthrough_consumes,_tmp406);struct _tuple18 _tmp3B2=_tmp3B1;union Cyc_CfFlowInfo_FlowInfo _tmp3D7;union Cyc_CfFlowInfo_AbsLVal _tmp3D6;_LL24: _tmp3D7=_tmp3B2.f1;_tmp3D6=_tmp3B2.f2;_LL25:;{
struct _tuple18 _tmp3B3=({struct _tuple18 _tmp3D5;_tmp3D5.f1=_tmp3D7;_tmp3D5.f2=_tmp3D6;_tmp3D5;});struct _tuple18 _tmp3B4=_tmp3B3;struct Cyc_Dict_Dict _tmp3D4;struct Cyc_List_List*_tmp3D3;struct Cyc_CfFlowInfo_Place*_tmp3D2;if(((_tmp3B4.f1).ReachableFL).tag == 2){if(((_tmp3B4.f2).PlaceL).tag == 1){_LL27: _tmp3D4=(((_tmp3B4.f1).ReachableFL).val).f1;_tmp3D3=(((_tmp3B4.f1).ReachableFL).val).f2;_tmp3D2=((_tmp3B4.f2).PlaceL).val;_LL28: {
# 1836
void*_tmp3B5=Cyc_CfFlowInfo_lookup_place(_tmp3D4,_tmp3D2);
struct _tuple15 _tmp3B6=Cyc_CfFlowInfo_unname_rval(_tmp3B5);struct _tuple15 _tmp3B7=_tmp3B6;void*_tmp3D1;struct Cyc_List_List*_tmp3D0;_LL2C: _tmp3D1=_tmp3B7.f1;_tmp3D0=_tmp3B7.f2;_LL2D:;
if(expand_unique){
int _tmp3B8=1;
void*_tmp3B9=_tmp3D1;struct Cyc_Absyn_Vardecl*_tmp3CA;void*_tmp3C9;switch(*((int*)_tmp3B9)){case 8U: _LL2F: _tmp3CA=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3B9)->f1;_tmp3C9=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3B9)->f2;_LL30:
# 1842
({void*_tmp3BA=0U;({struct _dyneither_ptr _tmp785=({const char*_tmp3BB="bad named location in anal_Lexp:deref";_tag_dyneither(_tmp3BB,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp785,_tag_dyneither(_tmp3BA,sizeof(void*),0U));});});case 7U: switch(*((int*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3B9)->f3)){case 4U: _LL31: _LL32:
 goto _LL34;case 5U: _LL35: _LL36:
# 1846
 goto _LL38;default: goto _LL3B;}case 4U: _LL33: _LL34:
# 1845
 return Cyc_NewControlFlow_anal_derefL(env,_tmp3D7,_tmp406,_tmp3D7,_tmp3B5,passthrough_consumes,0,path);case 5U: _LL37: _LL38: {
# 1850
struct _tuple18 _tmp3BC=Cyc_NewControlFlow_anal_derefL(env,_tmp3D7,_tmp406,_tmp3D7,_tmp3B5,passthrough_consumes,0,path);struct _tuple18 _tmp3BD=_tmp3BC;union Cyc_CfFlowInfo_FlowInfo _tmp3BF;_LL3E: _tmp3BF=_tmp3BD.f1;_LL3F:;
return({struct _tuple18 _tmp3BE;_tmp3BE.f1=_tmp3BF;({union Cyc_CfFlowInfo_AbsLVal _tmp786=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp3D2,path));_tmp3BE.f2=_tmp786;});_tmp3BE;});}case 1U: _LL39: _LL3A:
# 1853
 _tmp3B8=0;goto _LL3C;default: _LL3B: _LL3C: {
# 1860
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp39D,_tmp3D4,_tmp3D1);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp39D->unknown_all: _tmp39D->unknown_none;
void*res=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp3C7=_cycalloc(sizeof(*_tmp3C7));({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp78A=({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp3C8;_tmp3C8.tag=5U;({void*_tmp789=({struct Cyc_CfFlowInfo_FlowEnv*_tmp788=_tmp39D;void*_tmp787=Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp406->topt));Cyc_CfFlowInfo_typ_to_absrval(_tmp788,_tmp787,0,leaf);});_tmp3C8.f1=_tmp789;});_tmp3C8;});_tmp3C7[0]=_tmp78A;});_tmp3C7;});
for(0;_tmp3D0 != 0;_tmp3D0=_tmp3D0->tl){
({void*_tmp78C=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp78B=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp3C1;_tmp3C1.tag=8U;_tmp3C1.f1=(struct Cyc_Absyn_Vardecl*)_tmp3D0->hd;_tmp3C1.f2=res;_tmp3C1;});_tmp3C0[0]=_tmp78B;});_tmp3C0;});res=_tmp78C;});}
({struct Cyc_Dict_Dict _tmp78D=Cyc_CfFlowInfo_assign_place(_tmp39D,e->loc,_tmp3D4,_tmp3D2,res);_tmp3D4=_tmp78D;});{
union Cyc_CfFlowInfo_FlowInfo _tmp3C2=Cyc_CfFlowInfo_ReachableFL(_tmp3D4,_tmp3D3);
# 1873
struct _tuple18 _tmp3C3=Cyc_NewControlFlow_anal_derefL(env,_tmp3C2,_tmp406,_tmp3C2,res,passthrough_consumes,_tmp3B8,path);struct _tuple18 _tmp3C4=_tmp3C3;union Cyc_CfFlowInfo_FlowInfo _tmp3C6;_LL41: _tmp3C6=_tmp3C4.f1;_LL42:;
# 1875
return({struct _tuple18 _tmp3C5;_tmp3C5.f1=_tmp3C6;({union Cyc_CfFlowInfo_AbsLVal _tmp78E=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp3D2,path));_tmp3C5.f2=_tmp78E;});_tmp3C5;});};}}_LL2E:;}else{
# 1879
void*_tmp3CB=_tmp3D1;switch(*((int*)_tmp3CB)){case 7U: if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3CB)->f3)->tag == 5U){_LL44: _LL45:
 goto _LL47;}else{goto _LL48;}case 5U: _LL46: _LL47: {
# 1884
struct _tuple18 _tmp3CC=Cyc_NewControlFlow_anal_derefL(env,_tmp3D7,_tmp406,_tmp3D7,_tmp3B5,passthrough_consumes,0,path);struct _tuple18 _tmp3CD=_tmp3CC;union Cyc_CfFlowInfo_FlowInfo _tmp3CF;_LL4B: _tmp3CF=_tmp3CD.f1;_LL4C:;
return({struct _tuple18 _tmp3CE;_tmp3CE.f1=_tmp3CF;({union Cyc_CfFlowInfo_AbsLVal _tmp78F=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp3D2,path));_tmp3CE.f2=_tmp78F;});_tmp3CE;});}default: _LL48: _LL49:
# 1888
 goto _LL43;}_LL43:;}
# 1891
goto _LL26;}}else{goto _LL29;}}else{_LL29: _LL2A:
 goto _LL26;}_LL26:;};}{
# 1896
struct _tuple16 _tmp3D8=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp406);struct _tuple16 _tmp3D9=_tmp3D8;union Cyc_CfFlowInfo_FlowInfo _tmp3DB;void*_tmp3DA;_LL4E: _tmp3DB=_tmp3D9.f1;_tmp3DA=_tmp3D9.f2;_LL4F:;
# 1898
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp406,_tmp3DB,_tmp3DA,passthrough_consumes,0,path);};case 23U: _LL18: _tmp408=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3A3)->f1;_tmp407=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3A3)->f2;_LL19: {
# 1901
void*_tmp3DC=Cyc_Tcutil_compress((void*)_check_null(_tmp408->topt));void*_tmp3DD=_tmp3DC;switch(*((int*)_tmp3DD)){case 10U: _LL51: _LL52: {
# 1903
unsigned int _tmp3DE=(Cyc_Evexp_eval_const_uint_exp(_tmp407)).f1;
return({struct Cyc_NewControlFlow_AnalEnv*_tmp796=env;union Cyc_CfFlowInfo_FlowInfo _tmp795=inflow;int _tmp794=expand_unique;int _tmp793=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp792=_tmp408;Cyc_NewControlFlow_anal_Lexp_rec(_tmp796,_tmp795,_tmp794,_tmp793,_tmp792,({struct Cyc_List_List*_tmp3DF=_cycalloc(sizeof(*_tmp3DF));({void*_tmp791=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp3E0=_cycalloc_atomic(sizeof(*_tmp3E0));({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp790=({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp3E1;_tmp3E1.tag=0U;_tmp3E1.f1=(int)_tmp3DE;_tmp3E1;});_tmp3E0[0]=_tmp790;});_tmp3E0;});_tmp3DF->hd=_tmp791;});_tmp3DF->tl=path;_tmp3DF;}));});}case 5U: _LL53: _LL54: {
# 1907
struct _tuple23 _tmp3E2=({struct Cyc_NewControlFlow_AnalEnv*_tmp798=env;union Cyc_CfFlowInfo_FlowInfo _tmp797=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp798,_tmp797,({struct Cyc_Absyn_Exp*_tmp3F4[2U];_tmp3F4[1U]=_tmp407;_tmp3F4[0U]=_tmp408;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3F4,sizeof(struct Cyc_Absyn_Exp*),2U));}),0,1);});struct _tuple23 _tmp3E3=_tmp3E2;union Cyc_CfFlowInfo_FlowInfo _tmp3F3;struct Cyc_List_List*_tmp3F2;_LL58: _tmp3F3=_tmp3E3.f1;_tmp3F2=_tmp3E3.f2;_LL59:;{
union Cyc_CfFlowInfo_FlowInfo _tmp3E4=_tmp3F3;
{union Cyc_CfFlowInfo_FlowInfo _tmp3E5=_tmp3F3;struct Cyc_Dict_Dict _tmp3EA;struct Cyc_List_List*_tmp3E9;if((_tmp3E5.ReachableFL).tag == 2){_LL5B: _tmp3EA=((_tmp3E5.ReachableFL).val).f1;_tmp3E9=((_tmp3E5.ReachableFL).val).f2;_LL5C:
# 1911
 if(Cyc_CfFlowInfo_initlevel(_tmp39D,_tmp3EA,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp3F2))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp3E6=0U;({unsigned int _tmp79A=_tmp407->loc;struct _dyneither_ptr _tmp799=({const char*_tmp3E7="expression may not be initialized";_tag_dyneither(_tmp3E7,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp79A,_tmp799,_tag_dyneither(_tmp3E6,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp3E8=Cyc_NewControlFlow_add_subscript_reln(_tmp3E9,_tmp408,_tmp407);
if(_tmp3E9 != _tmp3E8)
({union Cyc_CfFlowInfo_FlowInfo _tmp79B=Cyc_CfFlowInfo_ReachableFL(_tmp3EA,_tmp3E8);_tmp3E4=_tmp79B;});
goto _LL5A;};}else{_LL5D: _LL5E:
 goto _LL5A;}_LL5A:;}{
# 1920
struct _tuple18 _tmp3EB=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp408,_tmp3F3,(void*)((struct Cyc_List_List*)_check_null(_tmp3F2))->hd,passthrough_consumes,0,path);struct _tuple18 _tmp3EC=_tmp3EB;union Cyc_CfFlowInfo_FlowInfo _tmp3F1;union Cyc_CfFlowInfo_AbsLVal _tmp3F0;_LL60: _tmp3F1=_tmp3EC.f1;_tmp3F0=_tmp3EC.f2;_LL61:;{
union Cyc_CfFlowInfo_FlowInfo _tmp3ED=_tmp3F1;if((_tmp3ED.BottomFL).tag == 1){_LL63: _LL64:
 return({struct _tuple18 _tmp3EE;_tmp3EE.f1=_tmp3F1;_tmp3EE.f2=_tmp3F0;_tmp3EE;});}else{_LL65: _LL66:
 return({struct _tuple18 _tmp3EF;_tmp3EF.f1=_tmp3E4;_tmp3EF.f2=_tmp3F0;_tmp3EF;});}_LL62:;};};};}default: _LL55: _LL56:
# 1925
({void*_tmp3F5=0U;({struct _dyneither_ptr _tmp79C=({const char*_tmp3F6="anal_Lexp: Subscript -- bad type";_tag_dyneither(_tmp3F6,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp79C,_tag_dyneither(_tmp3F5,sizeof(void*),0U));});});}_LL50:;}case 21U: _LL1A: _tmp40A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3A3)->f1;_tmp409=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3A3)->f2;_LL1B: {
# 1929
void*_tmp3F7=(void*)_check_null(_tmp40A->topt);
if(Cyc_Absyn_is_require_union_type(_tmp3F7))
Cyc_NewControlFlow_check_union_requires(_tmp40A->loc,_tmp3F7,_tmp409,inflow);
# 1933
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp3F7))
return({struct _tuple18 _tmp3F8;_tmp3F8.f1=inflow;({union Cyc_CfFlowInfo_AbsLVal _tmp79D=Cyc_CfFlowInfo_UnknownL();_tmp3F8.f2=_tmp79D;});_tmp3F8;});
# 1936
return({struct Cyc_NewControlFlow_AnalEnv*_tmp7A5=env;union Cyc_CfFlowInfo_FlowInfo _tmp7A4=inflow;int _tmp7A3=expand_unique;int _tmp7A2=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp7A1=_tmp40A;Cyc_NewControlFlow_anal_Lexp_rec(_tmp7A5,_tmp7A4,_tmp7A3,_tmp7A2,_tmp7A1,({struct Cyc_List_List*_tmp3F9=_cycalloc(sizeof(*_tmp3F9));({void*_tmp7A0=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp3FA=_cycalloc_atomic(sizeof(*_tmp3FA));({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp79F=({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp3FB;_tmp3FB.tag=0U;({int _tmp79E=
Cyc_CfFlowInfo_get_field_index(_tmp3F7,_tmp409);_tmp3FB.f1=_tmp79E;});_tmp3FB;});_tmp3FA[0]=_tmp79F;});_tmp3FA;});_tmp3F9->hd=_tmp7A0;});_tmp3F9->tl=path;_tmp3F9;}));});}default: _LL1C: _LL1D:
# 1940
 return({struct _tuple18 _tmp3FC;({union Cyc_CfFlowInfo_FlowInfo _tmp7A6=Cyc_CfFlowInfo_BottomFL();_tmp3FC.f1=_tmp7A6;});({union Cyc_CfFlowInfo_AbsLVal _tmp7A7=Cyc_CfFlowInfo_UnknownL();_tmp3FC.f2=_tmp7A7;});_tmp3FC;});}_LL5:;};}
# 1944
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1948
struct _tuple18 _tmp40B=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);struct _tuple18 _tmp40C=_tmp40B;union Cyc_CfFlowInfo_FlowInfo _tmp40F;union Cyc_CfFlowInfo_AbsLVal _tmp40E;_LL1: _tmp40F=_tmp40C.f1;_tmp40E=_tmp40C.f2;_LL2:;
return({struct _tuple18 _tmp40D;_tmp40D.f1=_tmp40F;_tmp40D.f2=_tmp40E;_tmp40D;});}
# 1955
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1958
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd,1) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
struct _tuple18 _tmp410=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple18 _tmp411=_tmp410;union Cyc_CfFlowInfo_FlowInfo _tmp412;_LL1: _tmp412=_tmp411.f1;_LL2:;
inflow=_tmp412;}}}
# 1966
return inflow;}
# 1972
static struct _tuple19 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1974
struct Cyc_CfFlowInfo_FlowEnv*_tmp413=env->fenv;
# 1977
void*r1;void*r2;
union Cyc_CfFlowInfo_FlowInfo f;
struct _tuple23 _tmp414=Cyc_NewControlFlow_anal_Rexps(env,inflow,es,0,0);struct _tuple23 _tmp415=_tmp414;union Cyc_CfFlowInfo_FlowInfo _tmp458;struct Cyc_List_List*_tmp457;_LL1: _tmp458=_tmp415.f1;_tmp457=_tmp415.f2;_LL2:;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp457))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp457->tl))->hd;
f=_tmp458;{
# 1985
union Cyc_CfFlowInfo_FlowInfo _tmp416=f;struct Cyc_Dict_Dict _tmp456;struct Cyc_List_List*_tmp455;if((_tmp416.BottomFL).tag == 1){_LL4: _LL5:
 return({struct _tuple19 _tmp417;_tmp417.f1=f;_tmp417.f2=f;_tmp417;});}else{_LL6: _tmp456=((_tmp416.ReachableFL).val).f1;_tmp455=((_tmp416.ReachableFL).val).f2;_LL7: {
# 1988
struct Cyc_Absyn_Exp*_tmp418=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*_tmp419=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp456,r1)== Cyc_CfFlowInfo_NoneIL  && !Cyc_CfFlowInfo_is_init_pointer(r1))
({void*_tmp41A=0U;({unsigned int _tmp7A9=((struct Cyc_Absyn_Exp*)es->hd)->loc;struct _dyneither_ptr _tmp7A8=({const char*_tmp41B="expression may not be initialized";_tag_dyneither(_tmp41B,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp7A9,_tmp7A8,_tag_dyneither(_tmp41A,sizeof(void*),0U));});});
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp456,r2)== Cyc_CfFlowInfo_NoneIL  && !Cyc_CfFlowInfo_is_init_pointer(r1))
({void*_tmp41C=0U;({unsigned int _tmp7AB=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc;struct _dyneither_ptr _tmp7AA=({const char*_tmp41D="expression may not be initialized";_tag_dyneither(_tmp41D,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp7AB,_tmp7AA,_tag_dyneither(_tmp41C,sizeof(void*),0U));});});{
# 1995
union Cyc_CfFlowInfo_FlowInfo _tmp41E=f;
union Cyc_CfFlowInfo_FlowInfo _tmp41F=f;
# 2000
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct _tuple15 _tmp420=Cyc_CfFlowInfo_unname_rval(r1);struct _tuple15 _tmp421=_tmp420;void*_tmp43A;struct Cyc_List_List*_tmp439;_LL9: _tmp43A=_tmp421.f1;_tmp439=_tmp421.f2;_LLA:;{
struct _tuple15 _tmp422=Cyc_CfFlowInfo_unname_rval(r2);struct _tuple15 _tmp423=_tmp422;void*_tmp438;struct Cyc_List_List*_tmp437;_LLC: _tmp438=_tmp423.f1;_tmp437=_tmp423.f2;_LLD:;{
struct _tuple0 _tmp424=({struct _tuple0 _tmp436;_tmp436.f1=_tmp43A;_tmp436.f2=_tmp438;_tmp436;});struct _tuple0 _tmp425=_tmp424;enum Cyc_CfFlowInfo_InitLevel _tmp435;enum Cyc_CfFlowInfo_InitLevel _tmp434;switch(*((int*)_tmp425.f1)){case 2U: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp425.f2)->tag == 0U){_LLF: _tmp434=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp425.f1)->f1;_LL10: {
# 2005
struct _tuple19 _tmp426=Cyc_NewControlFlow_splitzero(env,f,f,_tmp418,_tmp434,_tmp439);struct _tuple19 _tmp427=_tmp426;union Cyc_CfFlowInfo_FlowInfo _tmp42C;union Cyc_CfFlowInfo_FlowInfo _tmp42B;_LL20: _tmp42C=_tmp427.f1;_tmp42B=_tmp427.f2;_LL21:;
{enum Cyc_Absyn_Primop _tmp428=p;switch(_tmp428){case Cyc_Absyn_Eq: _LL23: _LL24:
 _tmp41E=_tmp42B;_tmp41F=_tmp42C;goto _LL22;case Cyc_Absyn_Neq: _LL25: _LL26:
 _tmp41E=_tmp42C;_tmp41F=_tmp42B;goto _LL22;default: _LL27: _LL28:
({void*_tmp429=0U;({struct _dyneither_ptr _tmp7AC=({const char*_tmp42A="anal_test, zero-split";_tag_dyneither(_tmp42A,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7AC,_tag_dyneither(_tmp429,sizeof(void*),0U));});});}_LL22:;}
# 2011
goto _LLE;}}else{goto _LL1D;}case 0U: switch(*((int*)_tmp425.f2)){case 2U: _LL11: _tmp435=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp425.f2)->f1;_LL12: {
# 2013
struct _tuple19 _tmp42D=Cyc_NewControlFlow_splitzero(env,f,f,_tmp419,_tmp435,_tmp437);struct _tuple19 _tmp42E=_tmp42D;union Cyc_CfFlowInfo_FlowInfo _tmp433;union Cyc_CfFlowInfo_FlowInfo _tmp432;_LL2A: _tmp433=_tmp42E.f1;_tmp432=_tmp42E.f2;_LL2B:;
{enum Cyc_Absyn_Primop _tmp42F=p;switch(_tmp42F){case Cyc_Absyn_Eq: _LL2D: _LL2E:
 _tmp41E=_tmp432;_tmp41F=_tmp433;goto _LL2C;case Cyc_Absyn_Neq: _LL2F: _LL30:
 _tmp41E=_tmp433;_tmp41F=_tmp432;goto _LL2C;default: _LL31: _LL32:
({void*_tmp430=0U;({struct _dyneither_ptr _tmp7AD=({const char*_tmp431="anal_test, zero-split";_tag_dyneither(_tmp431,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7AD,_tag_dyneither(_tmp430,sizeof(void*),0U));});});}_LL2C:;}
# 2019
goto _LLE;}case 0U: _LL13: _LL14:
# 2021
 if(p == Cyc_Absyn_Eq)({union Cyc_CfFlowInfo_FlowInfo _tmp7AE=Cyc_CfFlowInfo_BottomFL();_tmp41F=_tmp7AE;});else{
({union Cyc_CfFlowInfo_FlowInfo _tmp7AF=Cyc_CfFlowInfo_BottomFL();_tmp41E=_tmp7AF;});}
goto _LLE;case 1U: _LL15: _LL16:
 goto _LL18;case 4U: _LL17: _LL18:
 goto _LL1A;default: goto _LL1D;}case 1U: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp425.f2)->tag == 0U){_LL19: _LL1A:
 goto _LL1C;}else{goto _LL1D;}case 4U: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp425.f2)->tag == 0U){_LL1B: _LL1C:
# 2028
 if(p == Cyc_Absyn_Neq)({union Cyc_CfFlowInfo_FlowInfo _tmp7B0=Cyc_CfFlowInfo_BottomFL();_tmp41F=_tmp7B0;});else{
({union Cyc_CfFlowInfo_FlowInfo _tmp7B1=Cyc_CfFlowInfo_BottomFL();_tmp41E=_tmp7B1;});}
goto _LLE;}else{goto _LL1D;}default: _LL1D: _LL1E:
 goto _LLE;}_LLE:;};};}
# 2039
{struct _tuple0 _tmp43B=({struct _tuple0 _tmp43E;({void*_tmp7B2=Cyc_Tcutil_compress((void*)_check_null(_tmp418->topt));_tmp43E.f1=_tmp7B2;});({void*_tmp7B3=
Cyc_Tcutil_compress((void*)_check_null(_tmp419->topt));_tmp43E.f2=_tmp7B3;});_tmp43E;});
# 2039
struct _tuple0 _tmp43C=_tmp43B;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp43C.f1)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp43C.f1)->f1 == Cyc_Absyn_Unsigned){_LL34: _LL35:
# 2041
 goto _LL37;}else{switch(*((int*)_tmp43C.f2)){case 6U: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp43C.f2)->f1 == Cyc_Absyn_Unsigned)goto _LL36;else{goto _LL3C;}case 19U: goto _LL3A;default: goto _LL3C;}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp43C.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp43C.f2)->f1 == Cyc_Absyn_Unsigned){_LL36: _LL37:
 goto _LL39;}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp43C.f1)->tag == 19U)goto _LL38;else{goto _LL3C;}}}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp43C.f1)->tag == 19U){_LL38: _LL39:
 goto _LL3B;}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp43C.f2)->tag == 19U){_LL3A: _LL3B:
 goto _LL33;}else{_LL3C: _LL3D:
 return({struct _tuple19 _tmp43D;_tmp43D.f1=_tmp41E;_tmp43D.f2=_tmp41F;_tmp43D;});}}}}_LL33:;}
# 2049
{enum Cyc_Absyn_Primop _tmp43F=p;switch(_tmp43F){case Cyc_Absyn_Eq: _LL3F: _LL40:
 goto _LL42;case Cyc_Absyn_Neq: _LL41: _LL42: goto _LL44;case Cyc_Absyn_Gt: _LL43: _LL44: goto _LL46;case Cyc_Absyn_Gte: _LL45: _LL46: goto _LL48;case Cyc_Absyn_Lt: _LL47: _LL48: goto _LL4A;case Cyc_Absyn_Lte: _LL49: _LL4A: goto _LL3E;default: _LL4B: _LL4C:
 return({struct _tuple19 _tmp440;_tmp440.f1=_tmp41E;_tmp440.f2=_tmp41F;_tmp440;});}_LL3E:;}{
# 2054
struct _RegionHandle*_tmp441=Cyc_Core_heap_region;
struct _tuple13 _tmp442=Cyc_Relations_primop2relation(_tmp418,p,_tmp419);struct _tuple13 _tmp443=_tmp442;struct Cyc_Absyn_Exp*_tmp454;enum Cyc_Relations_Relation _tmp453;struct Cyc_Absyn_Exp*_tmp452;_LL4E: _tmp454=_tmp443.f1;_tmp453=_tmp443.f2;_tmp452=_tmp443.f3;_LL4F:;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
# 2059
if(Cyc_Relations_exp2relnop(_tmp454,& n1) && Cyc_Relations_exp2relnop(_tmp452,& n2)){
# 2061
struct Cyc_List_List*_tmp444=Cyc_Relations_add_relation(_tmp441,n1,_tmp453,n2,_tmp455);
# 2065
struct Cyc_List_List*_tmp445=({struct _RegionHandle*_tmp7B7=_tmp441;union Cyc_Relations_RelnOp _tmp7B6=n2;enum Cyc_Relations_Relation _tmp7B5=Cyc_Relations_flip_relation(_tmp453);union Cyc_Relations_RelnOp _tmp7B4=n1;Cyc_Relations_add_relation(_tmp7B7,_tmp7B6,_tmp7B5,_tmp7B4,_tmp455);});
struct _tuple19 _tmp446=({struct _tuple19 _tmp450;_tmp450.f1=_tmp41E;_tmp450.f2=_tmp41F;_tmp450;});struct _tuple19 _tmp447=_tmp446;struct Cyc_Dict_Dict _tmp44F;struct Cyc_Dict_Dict _tmp44E;struct Cyc_Dict_Dict _tmp44D;struct Cyc_Dict_Dict _tmp44C;if(((_tmp447.f1).ReachableFL).tag == 2){if(((_tmp447.f2).ReachableFL).tag == 2){_LL51: _tmp44D=(((_tmp447.f1).ReachableFL).val).f1;_tmp44C=(((_tmp447.f2).ReachableFL).val).f1;_LL52:
# 2068
 return({struct _tuple19 _tmp448;({union Cyc_CfFlowInfo_FlowInfo _tmp7B8=Cyc_CfFlowInfo_ReachableFL(_tmp44D,_tmp444);_tmp448.f1=_tmp7B8;});({union Cyc_CfFlowInfo_FlowInfo _tmp7B9=Cyc_CfFlowInfo_ReachableFL(_tmp44C,_tmp445);_tmp448.f2=_tmp7B9;});_tmp448;});}else{_LL55: _tmp44E=(((_tmp447.f1).ReachableFL).val).f1;_LL56:
# 2072
 return({struct _tuple19 _tmp44A;({union Cyc_CfFlowInfo_FlowInfo _tmp7BA=Cyc_CfFlowInfo_ReachableFL(_tmp44E,_tmp444);_tmp44A.f1=_tmp7BA;});_tmp44A.f2=_tmp41F;_tmp44A;});}}else{if(((_tmp447.f2).ReachableFL).tag == 2){_LL53: _tmp44F=(((_tmp447.f2).ReachableFL).val).f1;_LL54:
# 2070
 return({struct _tuple19 _tmp449;_tmp449.f1=_tmp41E;({union Cyc_CfFlowInfo_FlowInfo _tmp7BB=Cyc_CfFlowInfo_ReachableFL(_tmp44F,_tmp445);_tmp449.f2=_tmp7BB;});_tmp449;});}else{_LL57: _LL58:
# 2074
 return({struct _tuple19 _tmp44B;_tmp44B.f1=_tmp41E;_tmp44B.f2=_tmp41F;_tmp44B;});}}_LL50:;}else{
# 2077
return({struct _tuple19 _tmp451;_tmp451.f1=_tmp41E;_tmp451.f2=_tmp41F;_tmp451;});}};};};}}_LL3:;};}
# 2083
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
struct Cyc_CfFlowInfo_FlowEnv*_tmp459=env->fenv;
void*_tmp45A=e->r;void*_tmp45B=_tmp45A;enum Cyc_Absyn_Primop _tmp4A8;struct Cyc_List_List*_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A6;struct Cyc_Absyn_Exp*_tmp4A5;struct Cyc_Absyn_Exp*_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A3;struct Cyc_Absyn_Exp*_tmp4A2;struct Cyc_Absyn_Exp*_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A0;struct Cyc_Absyn_Exp*_tmp49F;struct Cyc_Absyn_Exp*_tmp49E;struct Cyc_Absyn_Exp*_tmp49D;switch(*((int*)_tmp45B)){case 6U: _LL1: _tmp49F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp45B)->f1;_tmp49E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp45B)->f2;_tmp49D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp45B)->f3;_LL2: {
# 2087
struct _tuple19 _tmp45C=Cyc_NewControlFlow_anal_test(env,inflow,_tmp49F);struct _tuple19 _tmp45D=_tmp45C;union Cyc_CfFlowInfo_FlowInfo _tmp468;union Cyc_CfFlowInfo_FlowInfo _tmp467;_LL10: _tmp468=_tmp45D.f1;_tmp467=_tmp45D.f2;_LL11:;{
struct _tuple19 _tmp45E=Cyc_NewControlFlow_anal_test(env,_tmp468,_tmp49E);struct _tuple19 _tmp45F=_tmp45E;union Cyc_CfFlowInfo_FlowInfo _tmp466;union Cyc_CfFlowInfo_FlowInfo _tmp465;_LL13: _tmp466=_tmp45F.f1;_tmp465=_tmp45F.f2;_LL14:;{
struct _tuple19 _tmp460=Cyc_NewControlFlow_anal_test(env,_tmp467,_tmp49D);struct _tuple19 _tmp461=_tmp460;union Cyc_CfFlowInfo_FlowInfo _tmp464;union Cyc_CfFlowInfo_FlowInfo _tmp463;_LL16: _tmp464=_tmp461.f1;_tmp463=_tmp461.f2;_LL17:;
return({struct _tuple19 _tmp462;({union Cyc_CfFlowInfo_FlowInfo _tmp7BC=Cyc_CfFlowInfo_join_flow(_tmp459,_tmp466,_tmp464);_tmp462.f1=_tmp7BC;});({union Cyc_CfFlowInfo_FlowInfo _tmp7BD=
Cyc_CfFlowInfo_join_flow(_tmp459,_tmp465,_tmp463);_tmp462.f2=_tmp7BD;});_tmp462;});};};}case 7U: _LL3: _tmp4A1=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp45B)->f1;_tmp4A0=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp45B)->f2;_LL4: {
# 2093
struct _tuple19 _tmp469=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4A1);struct _tuple19 _tmp46A=_tmp469;union Cyc_CfFlowInfo_FlowInfo _tmp471;union Cyc_CfFlowInfo_FlowInfo _tmp470;_LL19: _tmp471=_tmp46A.f1;_tmp470=_tmp46A.f2;_LL1A:;{
struct _tuple19 _tmp46B=Cyc_NewControlFlow_anal_test(env,_tmp471,_tmp4A0);struct _tuple19 _tmp46C=_tmp46B;union Cyc_CfFlowInfo_FlowInfo _tmp46F;union Cyc_CfFlowInfo_FlowInfo _tmp46E;_LL1C: _tmp46F=_tmp46C.f1;_tmp46E=_tmp46C.f2;_LL1D:;
return({struct _tuple19 _tmp46D;_tmp46D.f1=_tmp46F;({union Cyc_CfFlowInfo_FlowInfo _tmp7BE=Cyc_CfFlowInfo_join_flow(_tmp459,_tmp470,_tmp46E);_tmp46D.f2=_tmp7BE;});_tmp46D;});};}case 8U: _LL5: _tmp4A3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp45B)->f1;_tmp4A2=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp45B)->f2;_LL6: {
# 2097
struct _tuple19 _tmp472=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4A3);struct _tuple19 _tmp473=_tmp472;union Cyc_CfFlowInfo_FlowInfo _tmp47A;union Cyc_CfFlowInfo_FlowInfo _tmp479;_LL1F: _tmp47A=_tmp473.f1;_tmp479=_tmp473.f2;_LL20:;{
struct _tuple19 _tmp474=Cyc_NewControlFlow_anal_test(env,_tmp479,_tmp4A2);struct _tuple19 _tmp475=_tmp474;union Cyc_CfFlowInfo_FlowInfo _tmp478;union Cyc_CfFlowInfo_FlowInfo _tmp477;_LL22: _tmp478=_tmp475.f1;_tmp477=_tmp475.f2;_LL23:;
return({struct _tuple19 _tmp476;({union Cyc_CfFlowInfo_FlowInfo _tmp7BF=Cyc_CfFlowInfo_join_flow(_tmp459,_tmp47A,_tmp478);_tmp476.f1=_tmp7BF;});_tmp476.f2=_tmp477;_tmp476;});};}case 9U: _LL7: _tmp4A5=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp45B)->f1;_tmp4A4=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp45B)->f2;_LL8: {
# 2101
struct _tuple16 _tmp47B=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp4A5);struct _tuple16 _tmp47C=_tmp47B;union Cyc_CfFlowInfo_FlowInfo _tmp47E;void*_tmp47D;_LL25: _tmp47E=_tmp47C.f1;_tmp47D=_tmp47C.f2;_LL26:;
return Cyc_NewControlFlow_anal_test(env,_tmp47E,_tmp4A4);}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp45B)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp45B)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp45B)->f2)->tl == 0){_LL9: _tmp4A6=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp45B)->f2)->hd;_LLA: {
# 2104
struct _tuple19 _tmp47F=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4A6);struct _tuple19 _tmp480=_tmp47F;union Cyc_CfFlowInfo_FlowInfo _tmp483;union Cyc_CfFlowInfo_FlowInfo _tmp482;_LL28: _tmp483=_tmp480.f1;_tmp482=_tmp480.f2;_LL29:;
return({struct _tuple19 _tmp481;_tmp481.f1=_tmp482;_tmp481.f2=_tmp483;_tmp481;});}}else{goto _LLB;}}else{goto _LLB;}}else{_LLB: _tmp4A8=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp45B)->f1;_tmp4A7=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp45B)->f2;_LLC:
# 2107
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp4A8,_tmp4A7);}default: _LLD: _LLE: {
# 2111
struct _tuple16 _tmp484=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);struct _tuple16 _tmp485=_tmp484;union Cyc_CfFlowInfo_FlowInfo _tmp49C;void*_tmp49B;_LL2B: _tmp49C=_tmp485.f1;_tmp49B=_tmp485.f2;_LL2C:;{
union Cyc_CfFlowInfo_FlowInfo _tmp486=_tmp49C;struct Cyc_Dict_Dict _tmp49A;if((_tmp486.BottomFL).tag == 1){_LL2E: _LL2F:
 return({struct _tuple19 _tmp487;_tmp487.f1=_tmp49C;_tmp487.f2=_tmp49C;_tmp487;});}else{_LL30: _tmp49A=((_tmp486.ReachableFL).val).f1;_LL31: {
# 2115
struct _tuple15 _tmp488=Cyc_CfFlowInfo_unname_rval(_tmp49B);struct _tuple15 _tmp489=_tmp488;void*_tmp499;struct Cyc_List_List*_tmp498;_LL33: _tmp499=_tmp489.f1;_tmp498=_tmp489.f2;_LL34:;{
void*_tmp48A=_tmp499;enum Cyc_CfFlowInfo_InitLevel _tmp497;struct Cyc_Absyn_Vardecl*_tmp496;void*_tmp495;switch(*((int*)_tmp48A)){case 8U: _LL36: _tmp496=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp48A)->f1;_tmp495=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp48A)->f2;_LL37:
# 2118
({void*_tmp48B=0U;({struct _dyneither_ptr _tmp7C0=({const char*_tmp48C="anal_test: bad namedlocation";_tag_dyneither(_tmp48C,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7C0,_tag_dyneither(_tmp48B,sizeof(void*),0U));});});case 0U: _LL38: _LL39:
 return({struct _tuple19 _tmp48D;({union Cyc_CfFlowInfo_FlowInfo _tmp7C1=Cyc_CfFlowInfo_BottomFL();_tmp48D.f1=_tmp7C1;});_tmp48D.f2=_tmp49C;_tmp48D;});case 1U: _LL3A: _LL3B:
 goto _LL3D;case 5U: _LL3C: _LL3D:
 goto _LL3F;case 4U: _LL3E: _LL3F:
 return({struct _tuple19 _tmp48E;_tmp48E.f1=_tmp49C;({union Cyc_CfFlowInfo_FlowInfo _tmp7C2=Cyc_CfFlowInfo_BottomFL();_tmp48E.f2=_tmp7C2;});_tmp48E;});case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp48A)->f1 == Cyc_CfFlowInfo_NoneIL){_LL40: _LL41:
 goto _LL43;}else{_LL46: _tmp497=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp48A)->f1;_LL47:
# 2128
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp49C,e,_tmp497,_tmp498);}case 3U: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp48A)->f1 == Cyc_CfFlowInfo_NoneIL){_LL42: _LL43:
# 2124
 goto _LL45;}else{_LL48: _LL49:
# 2129
 return({struct _tuple19 _tmp492;_tmp492.f1=_tmp49C;_tmp492.f2=_tmp49C;_tmp492;});}case 7U: _LL44: _LL45:
# 2126
({void*_tmp48F=0U;({unsigned int _tmp7C4=e->loc;struct _dyneither_ptr _tmp7C3=({const char*_tmp490="expression may not be initialized";_tag_dyneither(_tmp490,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp7C4,_tmp7C3,_tag_dyneither(_tmp48F,sizeof(void*),0U));});});
return({struct _tuple19 _tmp491;({union Cyc_CfFlowInfo_FlowInfo _tmp7C5=Cyc_CfFlowInfo_BottomFL();_tmp491.f1=_tmp7C5;});({union Cyc_CfFlowInfo_FlowInfo _tmp7C6=Cyc_CfFlowInfo_BottomFL();_tmp491.f2=_tmp7C6;});_tmp491;});default: _LL4A: _LL4B:
# 2130
({void*_tmp493=0U;({struct _dyneither_ptr _tmp7C7=({const char*_tmp494="anal_test";_tag_dyneither(_tmp494,sizeof(char),10U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7C7,_tag_dyneither(_tmp493,sizeof(void*),0U));});});}_LL35:;};}}_LL2D:;};}}_LL0:;}struct _tuple27{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 2136
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple27*ckenv,void*root,void*rval){
# 2138
struct _tuple27*_tmp4A9=ckenv;unsigned int _tmp4B5;struct Cyc_NewControlFlow_AnalEnv*_tmp4B4;struct Cyc_Dict_Dict _tmp4B3;_LL1: _tmp4B5=_tmp4A9->f1;_tmp4B4=_tmp4A9->f2;_tmp4B3=_tmp4A9->f3;_LL2:;{
void*_tmp4AA=root;struct Cyc_Absyn_Vardecl*_tmp4B2;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4AA)->tag == 0U){_LL4: _tmp4B2=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4AA)->f1;_LL5:
# 2141
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp4B2->type)){
struct _tuple15 _tmp4AB=Cyc_CfFlowInfo_unname_rval(rval);struct _tuple15 _tmp4AC=_tmp4AB;void*_tmp4B1;_LL9: _tmp4B1=_tmp4AC.f1;_LLA:;{
void*_tmp4AD=_tmp4B1;switch(*((int*)_tmp4AD)){case 7U: _LLC: _LLD:
 goto _LLF;case 0U: _LLE: _LLF:
 goto _LL11;case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4AD)->f1 == Cyc_CfFlowInfo_NoneIL){_LL10: _LL11:
 goto _LLB;}else{goto _LL12;}default: _LL12: _LL13:
# 2148
({struct Cyc_String_pa_PrintArg_struct _tmp4B0;_tmp4B0.tag=0U;({struct _dyneither_ptr _tmp7C8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4B2->name));_tmp4B0.f1=_tmp7C8;});({void*_tmp4AE[1U]={& _tmp4B0};({unsigned int _tmp7CA=_tmp4B5;struct _dyneither_ptr _tmp7C9=({const char*_tmp4AF="unique pointers reachable from %s may become unreachable";_tag_dyneither(_tmp4AF,sizeof(char),57U);});Cyc_Warn_warn(_tmp7CA,_tmp7C9,_tag_dyneither(_tmp4AE,sizeof(void*),1U));});});});
goto _LLB;}_LLB:;};}
# 2152
goto _LL3;}else{_LL6: _LL7:
 goto _LL3;}_LL3:;};}
# 2157
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp4B6=flow;struct Cyc_Dict_Dict _tmp4BC;if((_tmp4B6.BottomFL).tag == 1){_LL1: _LL2:
 return;}else{_LL3: _tmp4BC=((_tmp4B6.ReachableFL).val).f1;_LL4:
# 2161
{struct Cyc_List_List*_tmp4B7=env->param_roots;for(0;_tmp4B7 != 0;_tmp4B7=_tmp4B7->tl){
if(({struct Cyc_CfFlowInfo_FlowEnv*_tmp7CC=env->fenv;struct Cyc_Dict_Dict _tmp7CB=_tmp4BC;Cyc_CfFlowInfo_initlevel(_tmp7CC,_tmp7CB,Cyc_CfFlowInfo_lookup_place(_tmp4BC,(struct Cyc_CfFlowInfo_Place*)_tmp4B7->hd));})!= Cyc_CfFlowInfo_AllIL)
({void*_tmp4B8=0U;({unsigned int _tmp7CE=loc;struct _dyneither_ptr _tmp7CD=({const char*_tmp4B9="function may not initialize all the parameters with attribute 'initializes'";_tag_dyneither(_tmp4B9,sizeof(char),76U);});Cyc_CfFlowInfo_aerr(_tmp7CE,_tmp7CD,_tag_dyneither(_tmp4B8,sizeof(void*),0U));});});}}
# 2165
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple27 _tmp4BA=({struct _tuple27 _tmp4BB;_tmp4BB.f1=loc;_tmp4BB.f2=env;_tmp4BB.f3=_tmp4BC;_tmp4BB;});
((void(*)(void(*f)(struct _tuple27*,void*,void*),struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp4BA,_tmp4BC);}
# 2169
return;}_LL0:;}
# 2178
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_pat_vars(struct Cyc_List_List*vds){
# 2180
struct Cyc_List_List*_tmp4BD=0;
struct Cyc_List_List*_tmp4BE=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp4BF=(struct _tuple24*)x->hd;struct _tuple24*_tmp4C0=_tmp4BF;struct Cyc_Absyn_Vardecl**_tmp4C9;struct Cyc_Absyn_Exp*_tmp4C8;_LL1: _tmp4C9=_tmp4C0->f1;_tmp4C8=_tmp4C0->f2;_LL2:;
if((_tmp4C9 != 0  && _tmp4C8 != 0) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp4C8->topt),0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4C1=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4C6=_cycalloc(sizeof(*_tmp4C6));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp7CF=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp4C7;_tmp4C7.tag=0U;_tmp4C7.f1=*_tmp4C9;_tmp4C7;});_tmp4C6[0]=_tmp7CF;});_tmp4C6;});
struct Cyc_CfFlowInfo_Place*_tmp4C2=({struct Cyc_CfFlowInfo_Place*_tmp4C5=_cycalloc(sizeof(*_tmp4C5));_tmp4C5->root=(void*)_tmp4C1;_tmp4C5->path=0;_tmp4C5;});
({struct Cyc_List_List*_tmp7D0=({struct Cyc_List_List*_tmp4C3=_cycalloc(sizeof(*_tmp4C3));_tmp4C3->hd=_tmp4C2;_tmp4C3->tl=_tmp4BD;_tmp4C3;});_tmp4BD=_tmp7D0;});
({struct Cyc_List_List*_tmp7D1=({struct Cyc_List_List*_tmp4C4=_cycalloc(sizeof(*_tmp4C4));_tmp4C4->hd=_tmp4C8;_tmp4C4->tl=_tmp4BE;_tmp4C4;});_tmp4BE=_tmp7D1;});}}}
# 2192
return({struct _tuple1 _tmp4CA;_tmp4CA.f1=_tmp4BD;_tmp4CA.f2=_tmp4BE;_tmp4CA;});}struct _tuple28{int f1;void*f2;};
# 2198
static struct _tuple28 Cyc_NewControlFlow_noconsume_place_ok(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*place,int do_unconsume,struct Cyc_Absyn_Vardecl*vd,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2205
union Cyc_CfFlowInfo_FlowInfo _tmp4CB=flow;struct Cyc_Dict_Dict _tmp4E0;if((_tmp4CB.BottomFL).tag == 1){_LL1: _LL2:
({void*_tmp4CC=0U;({struct _dyneither_ptr _tmp7D2=({const char*_tmp4CD="noconsume_place_ok: flow became Bottom!";_tag_dyneither(_tmp4CD,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7D2,_tag_dyneither(_tmp4CC,sizeof(void*),0U));});});}else{_LL3: _tmp4E0=((_tmp4CB.ReachableFL).val).f1;_LL4: {
# 2213
struct Cyc_Absyn_Exp*_tmp4CE=Cyc_Absyn_exp_unsigned_one;
int _tmp4CF=0;
int _tmp4D0=1;
void*_tmp4D1=Cyc_CfFlowInfo_lookup_place(_tmp4E0,place);
void*_tmp4D2=_tmp4D1;
# 2225
int varok=0;
{void*_tmp4D3=_tmp4D1;struct Cyc_Absyn_Vardecl*_tmp4DE;void*_tmp4DD;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4D3)->tag == 8U){_LL6: _tmp4DE=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4D3)->f1;_tmp4DD=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4D3)->f2;_LL7:
# 2228
 if(vd == _tmp4DE){
_tmp4D2=_tmp4DD;
# 2231
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2233
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp4CE,_tmp4D0,_tmp4D2,& _tmp4CF)){
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp4D6;_tmp4D6.tag=0U;({struct _dyneither_ptr _tmp7D3=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2237
Cyc_Absynpp_qvar2string(vd->name));_tmp4D6.f1=_tmp7D3;});({void*_tmp4D4[1U]={& _tmp4D6};({unsigned int _tmp7D5=loc;struct _dyneither_ptr _tmp7D4=({const char*_tmp4D5="function consumes parameter %s with attribute 'noconsume'";_tag_dyneither(_tmp4D5,sizeof(char),58U);});Cyc_CfFlowInfo_aerr(_tmp7D5,_tmp7D4,_tag_dyneither(_tmp4D4,sizeof(void*),1U));});});});}else{
# 2240
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4E0,_tmp4D2)!= Cyc_CfFlowInfo_AllIL  && !do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp4D9;_tmp4D9.tag=0U;({struct _dyneither_ptr _tmp7D6=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2243
Cyc_Absynpp_qvar2string(vd->name));_tmp4D9.f1=_tmp7D6;});({void*_tmp4D7[1U]={& _tmp4D9};({unsigned int _tmp7D8=loc;struct _dyneither_ptr _tmp7D7=({const char*_tmp4D8="function consumes value pointed to by parameter %s, which has attribute 'noconsume'";_tag_dyneither(_tmp4D8,sizeof(char),84U);});Cyc_CfFlowInfo_aerr(_tmp7D8,_tmp7D7,_tag_dyneither(_tmp4D7,sizeof(void*),1U));});});});else{
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
({struct Cyc_String_pa_PrintArg_struct _tmp4DC;_tmp4DC.tag=0U;({struct _dyneither_ptr _tmp7D9=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2260
Cyc_Absynpp_qvar2string(vd->name));_tmp4DC.f1=_tmp7D9;});({void*_tmp4DA[1U]={& _tmp4DC};({unsigned int _tmp7DB=loc;struct _dyneither_ptr _tmp7DA=({const char*_tmp4DB="function parameter %s with attribute 'noconsume' no longer set to its original value";_tag_dyneither(_tmp4DB,sizeof(char),85U);});Cyc_CfFlowInfo_aerr(_tmp7DB,_tmp7DA,_tag_dyneither(_tmp4DA,sizeof(void*),1U));});});});}
goto _LL5;}_LL5:;}
# 2267
return({struct _tuple28 _tmp4DF;_tmp4DF.f1=varok;_tmp4DF.f2=_tmp4D2;_tmp4DF;});}}_LL0:;}
# 2273
static struct Cyc_Absyn_Vardecl*Cyc_NewControlFlow_get_vd_from_place(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_CfFlowInfo_Place*_tmp4E1=p;void*_tmp4E9;struct Cyc_List_List*_tmp4E8;_LL1: _tmp4E9=_tmp4E1->root;_tmp4E8=_tmp4E1->path;_LL2:;
if(_tmp4E8 != 0)
({void*_tmp4E2=0U;({struct _dyneither_ptr _tmp7DC=({const char*_tmp4E3="unconsume_params: param to unconsume is non-variable";_tag_dyneither(_tmp4E3,sizeof(char),53U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7DC,_tag_dyneither(_tmp4E2,sizeof(void*),0U));});});{
struct Cyc_Absyn_Vardecl*vd;
void*_tmp4E4=_tmp4E9;struct Cyc_Absyn_Vardecl*_tmp4E7;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4E4)->tag == 0U){_LL4: _tmp4E7=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4E4)->f1;_LL5:
 return _tmp4E7;}else{_LL6: _LL7:
({void*_tmp4E5=0U;({struct _dyneither_ptr _tmp7DD=({const char*_tmp4E6="unconsume_params: root is not a varroot";_tag_dyneither(_tmp4E6,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7DD,_tag_dyneither(_tmp4E5,sizeof(void*),0U));});});}_LL3:;};}
# 2292 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_exp(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Exp*unconsume_exp,struct Cyc_Absyn_Vardecl*vd,int varok,void*ropt,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2300
{union Cyc_CfFlowInfo_FlowInfo _tmp4EA=flow;struct Cyc_Dict_Dict _tmp503;if((_tmp4EA.BottomFL).tag == 1){_LL1: _LL2:
 return flow;}else{_LL3: _tmp503=((_tmp4EA.ReachableFL).val).f1;_LL4: {
# 2307
struct _tuple18 _tmp4EB=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,unconsume_exp);struct _tuple18 _tmp4EC=_tmp4EB;union Cyc_CfFlowInfo_FlowInfo _tmp502;union Cyc_CfFlowInfo_AbsLVal _tmp501;_LL6: _tmp502=_tmp4EC.f1;_tmp501=_tmp4EC.f2;_LL7:;
# 2310
{union Cyc_CfFlowInfo_AbsLVal _tmp4ED=_tmp501;struct Cyc_CfFlowInfo_Place*_tmp500;if((_tmp4ED.PlaceL).tag == 1){_LL9: _tmp500=(_tmp4ED.PlaceL).val;_LLA: {
# 2314
void*_tmp4EE=Cyc_CfFlowInfo_lookup_place(_tmp503,_tmp500);
{void*_tmp4EF=_tmp4EE;struct Cyc_Absyn_Vardecl*_tmp4FF;void*_tmp4FE;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4EF)->tag == 8U){_LLE: _tmp4FF=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4EF)->f1;_tmp4FE=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4EF)->f2;_LLF: {
# 2320
void*new_rval;
if(_tmp4FF == vd){
# 2324
if(varok){
# 2326
({void*_tmp7DE=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,_tmp4FE);_tmp4EE=_tmp7DE;});
# 2331
if(ropt != 0){
# 2337
struct _tuple16 _tmp4F0=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp7E0=env->fenv;struct _tuple16 _tmp7DF=({struct _tuple16 _tmp4F4;_tmp4F4.f1=_tmp502;_tmp4F4.f2=_tmp4EE;_tmp4F4;});Cyc_CfFlowInfo_join_flow_and_rval(_tmp7E0,_tmp7DF,({struct _tuple16 _tmp4F5;_tmp4F5.f1=_tmp502;_tmp4F5.f2=ropt;_tmp4F5;}));});
# 2337
struct _tuple16 _tmp4F1=_tmp4F0;union Cyc_CfFlowInfo_FlowInfo _tmp4F3;void*_tmp4F2;_LL13: _tmp4F3=_tmp4F1.f1;_tmp4F2=_tmp4F1.f2;_LL14:;
# 2341
_tmp502=_tmp4F3;new_rval=_tmp4F2;}else{
# 2346
new_rval=_tmp4EE;}}else{
# 2349
new_rval=_tmp4FE;}
# 2351
{union Cyc_CfFlowInfo_FlowInfo _tmp4F6=_tmp502;struct Cyc_Dict_Dict _tmp4FA;struct Cyc_List_List*_tmp4F9;if((_tmp4F6.ReachableFL).tag == 2){_LL16: _tmp4FA=((_tmp4F6.ReachableFL).val).f1;_tmp4F9=((_tmp4F6.ReachableFL).val).f2;_LL17:
# 2353
({union Cyc_CfFlowInfo_FlowInfo _tmp7E2=({struct Cyc_Dict_Dict _tmp7E1=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp4FA,_tmp500,new_rval);Cyc_CfFlowInfo_ReachableFL(_tmp7E1,_tmp4F9);});flow=_tmp7E2;});
# 2357
goto _LL15;}else{_LL18: _LL19:
# 2359
({void*_tmp4F7=0U;({struct _dyneither_ptr _tmp7E3=({const char*_tmp4F8="unconsume_params: joined flow became bot!";_tag_dyneither(_tmp4F8,sizeof(char),42U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7E3,_tag_dyneither(_tmp4F7,sizeof(void*),0U));});});}_LL15:;}
# 2361
goto _LLD;}else{
# 2363
goto _LL11;}
goto _LLD;}}else{_LL10: _LL11:
# 2370
 if(ropt != 0  && !
Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
({struct Cyc_String_pa_PrintArg_struct _tmp4FD;_tmp4FD.tag=0U;({struct _dyneither_ptr _tmp7E4=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(unconsume_exp));_tmp4FD.f1=_tmp7E4;});({void*_tmp4FB[1U]={& _tmp4FD};({unsigned int _tmp7E6=loc;struct _dyneither_ptr _tmp7E5=({const char*_tmp4FC="aliased expression %s was overwritten";_tag_dyneither(_tmp4FC,sizeof(char),38U);});Cyc_CfFlowInfo_aerr(_tmp7E6,_tmp7E5,_tag_dyneither(_tmp4FB,sizeof(void*),1U));});});});
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
{union Cyc_CfFlowInfo_FlowInfo _tmp504=flow;if((_tmp504.BottomFL).tag == 1){_LL1: _LL2:
 return flow;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 2422
int _tmp505=unconsume_exps != 0;
struct Cyc_List_List*_tmp506=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(consumed_vals);
struct Cyc_List_List*_tmp507=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(unconsume_exps);
{struct Cyc_List_List*_tmp508=_tmp506;for(0;_tmp508 != 0;
(_tmp508=_tmp508->tl,
_tmp507 != 0?_tmp507=_tmp507->tl: 0)){
# 2431
struct Cyc_Absyn_Vardecl*_tmp509=Cyc_NewControlFlow_get_vd_from_place((struct Cyc_CfFlowInfo_Place*)_tmp508->hd);
struct _tuple28 _tmp50A=
is_region_open?({struct _tuple28 _tmp50E;_tmp50E.f1=1;_tmp50E.f2=0;_tmp50E;}):
 Cyc_NewControlFlow_noconsume_place_ok(env,(struct Cyc_CfFlowInfo_Place*)_tmp508->hd,_tmp505,_tmp509,flow,loc);
# 2432
struct _tuple28 _tmp50B=_tmp50A;int _tmp50D;void*_tmp50C;_LL6: _tmp50D=_tmp50B.f1;_tmp50C=_tmp50B.f2;_LL7:;
# 2438
if(_tmp505)
({union Cyc_CfFlowInfo_FlowInfo _tmp7E7=Cyc_NewControlFlow_unconsume_exp(env,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp507))->hd,_tmp509,_tmp50D,_tmp50C,flow,loc);flow=_tmp7E7;});}}
# 2441
Cyc_NewControlFlow_update_tryflow(env,flow);
return flow;};}struct _tuple29{int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 2445
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned int exp_loc){
# 2447
struct Cyc_CfFlowInfo_FlowEnv*_tmp50F=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp510=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_tmp511=_tmp510;struct Cyc_Core_Opt*_tmp525;struct Cyc_Absyn_Exp*_tmp524;struct Cyc_Absyn_Stmt*_tmp523;unsigned int _tmp522;_LL1: _tmp525=_tmp511->pat_vars;_tmp524=_tmp511->where_clause;_tmp523=_tmp511->body;_tmp522=_tmp511->loc;_LL2:;{
struct _tuple1 _tmp512=Cyc_NewControlFlow_get_unconsume_pat_vars((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp525))->v);struct _tuple1 _tmp513=_tmp512;struct Cyc_List_List*_tmp521;struct Cyc_List_List*_tmp520;_LL4: _tmp521=_tmp513.f1;_tmp520=_tmp513.f2;_LL5:;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)_tmp525->v,_tmp521 != 0,_tmp522,exp_loc);
# 2455
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp514=env->unique_pat_vars;
# 2458
if(Cyc_Tcpat_has_vars(_tmp525))
({struct Cyc_List_List*_tmp7E9=({struct Cyc_List_List*_tmp515=_cycalloc(sizeof(*_tmp515));({struct _tuple29*_tmp7E8=({struct _tuple29*_tmp516=_cycalloc(sizeof(*_tmp516));_tmp516->f1=0;_tmp516->f2=_tmp523;_tmp516->f3=_tmp521;_tmp516->f4=_tmp520;_tmp516;});_tmp515->hd=_tmp7E8;});_tmp515->tl=_tmp514;_tmp515;});env->unique_pat_vars=_tmp7E9;});
# 2462
if(_tmp524 != 0){
struct Cyc_Absyn_Exp*wexp=_tmp524;
struct _tuple19 _tmp517=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);struct _tuple19 _tmp518=_tmp517;union Cyc_CfFlowInfo_FlowInfo _tmp51A;union Cyc_CfFlowInfo_FlowInfo _tmp519;_LL7: _tmp51A=_tmp518.f1;_tmp519=_tmp518.f2;_LL8:;
inflow=_tmp519;
({union Cyc_CfFlowInfo_FlowInfo _tmp7EA=Cyc_NewControlFlow_anal_stmt(env,_tmp51A,_tmp523,0);clause_outflow=_tmp7EA;});}else{
# 2468
({union Cyc_CfFlowInfo_FlowInfo _tmp7EB=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp523,0);clause_outflow=_tmp7EB;});}
# 2470
env->unique_pat_vars=_tmp514;{
union Cyc_CfFlowInfo_FlowInfo _tmp51B=clause_outflow;if((_tmp51B.BottomFL).tag == 1){_LLA: _LLB:
 goto _LL9;}else{_LLC: _LLD:
# 2475
({union Cyc_CfFlowInfo_FlowInfo _tmp7EC=Cyc_NewControlFlow_unconsume_params(env,_tmp521,_tmp520,0,clause_outflow,_tmp522);clause_outflow=_tmp7EC;});
# 2477
if(scs->tl == 0)
return clause_outflow;else{
# 2482
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0)
({void*_tmp51C=0U;({unsigned int _tmp7EE=_tmp523->loc;struct _dyneither_ptr _tmp7ED=({const char*_tmp51D="switch clause may implicitly fallthru";_tag_dyneither(_tmp51D,sizeof(char),38U);});Cyc_CfFlowInfo_aerr(_tmp7EE,_tmp7ED,_tag_dyneither(_tmp51C,sizeof(void*),0U));});});else{
# 2485
({void*_tmp51E=0U;({unsigned int _tmp7F0=_tmp523->loc;struct _dyneither_ptr _tmp7EF=({const char*_tmp51F="switch clause may implicitly fallthru";_tag_dyneither(_tmp51F,sizeof(char),38U);});Cyc_Warn_warn(_tmp7F0,_tmp7EF,_tag_dyneither(_tmp51E,sizeof(void*),0U));});});}
# 2487
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2489
goto _LL9;}_LL9:;};};};}
# 2492
return Cyc_CfFlowInfo_BottomFL();}struct _tuple30{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2497
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple30*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2499
struct _tuple30*_tmp526=vdenv;struct Cyc_NewControlFlow_AnalEnv*_tmp532;struct Cyc_Dict_Dict _tmp531;unsigned int _tmp530;_LL1: _tmp532=_tmp526->f1;_tmp531=_tmp526->f2;_tmp530=_tmp526->f3;_LL2:;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2502
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp52F;_tmp52F.tag=0U;_tmp52F.f1=vd;_tmp52F;});
struct _tuple15 _tmp527=Cyc_CfFlowInfo_unname_rval(({struct Cyc_Dict_Dict _tmp7F1=_tmp531;((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp7F1,Cyc_CfFlowInfo_root_cmp,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp52E=_cycalloc(sizeof(*_tmp52E));_tmp52E[0]=vdroot;_tmp52E;}));}));struct _tuple15 _tmp528=_tmp527;void*_tmp52D;_LL4: _tmp52D=_tmp528.f1;_LL5:;{
void*_tmp529=_tmp52D;switch(*((int*)_tmp529)){case 7U: _LL7: _LL8:
 goto _LLA;case 0U: _LL9: _LLA:
 goto _LLC;case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp529)->f1 == Cyc_CfFlowInfo_NoneIL){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
# 2509
({struct Cyc_String_pa_PrintArg_struct _tmp52C;_tmp52C.tag=0U;({struct _dyneither_ptr _tmp7F2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp52C.f1=_tmp7F2;});({void*_tmp52A[1U]={& _tmp52C};({unsigned int _tmp7F4=_tmp530;struct _dyneither_ptr _tmp7F3=({const char*_tmp52B="unique pointers may still exist after variable %s goes out of scope";_tag_dyneither(_tmp52B,sizeof(char),68U);});Cyc_Warn_warn(_tmp7F4,_tmp7F3,_tag_dyneither(_tmp52A,sizeof(void*),1U));});});});
# 2511
goto _LL6;}_LL6:;};}}
# 2516
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
{union Cyc_CfFlowInfo_FlowInfo _tmp533=inflow;struct Cyc_Dict_Dict _tmp540;if((_tmp533.ReachableFL).tag == 2){_LL1: _tmp540=((_tmp533.ReachableFL).val).f1;_LL2: {
# 2519
struct _tuple30 _tmp534=({struct _tuple30 _tmp53F;_tmp53F.f1=env;_tmp53F.f2=_tmp540;_tmp53F.f3=decl->loc;_tmp53F;});
struct Cyc_CfFlowInfo_FlowEnv*_tmp535=env->fenv;
{void*_tmp536=decl->r;void*_tmp537=_tmp536;struct Cyc_List_List*_tmp53E;struct Cyc_List_List*_tmp53D;struct Cyc_Absyn_Vardecl*_tmp53C;switch(*((int*)_tmp537)){case 0U: _LL6: _tmp53C=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp537)->f1;_LL7:
# 2523
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp534,_tmp53C);
goto _LL5;case 2U: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp537)->f2 != 0){_LL8: _tmp53D=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp537)->f2)->v;_LL9: {
# 2526
struct _tuple1 _tmp538=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp53D);struct _tuple1 _tmp539=_tmp538;struct Cyc_List_List*_tmp53B;_LLF: _tmp53B=_tmp539.f1;_LL10:;{
struct Cyc_List_List*_tmp53A=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)(_tmp53B);
_tmp53E=_tmp53A;goto _LLB;};}}else{goto _LLC;}case 3U: _LLA: _tmp53E=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp537)->f1;_LLB:
# 2530
((void(*)(void(*f)(struct _tuple30*,struct Cyc_Absyn_Vardecl*),struct _tuple30*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp534,_tmp53E);
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
struct _tuple29*_tmp541=(struct _tuple29*)((struct Cyc_List_List*)_check_null(x))->hd;struct _tuple29*_tmp542=_tmp541;int _tmp546;struct Cyc_Absyn_Stmt*_tmp545;struct Cyc_List_List*_tmp544;struct Cyc_List_List*_tmp543;_LL1: _tmp546=_tmp542->f1;_tmp545=_tmp542->f2;_tmp544=_tmp542->f3;_tmp543=_tmp542->f4;_LL2:;
({union Cyc_CfFlowInfo_FlowInfo _tmp7F5=Cyc_NewControlFlow_unconsume_params(env,_tmp544,_tmp543,_tmp546,inflow,dest->loc);inflow=_tmp7F5;});}}
# 2552
return inflow;}
# 2558
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple17*rval_opt){
# 2560
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple20 _tmp547=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct _tuple20 _tmp548=_tmp547;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5E8;union Cyc_CfFlowInfo_FlowInfo*_tmp5E7;_LL1: _tmp5E8=_tmp548.f1;_tmp5E7=_tmp548.f2;_LL2:;
inflow=*_tmp5E7;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp549=env->fenv;
# 2567
void*_tmp54A=s->r;void*_tmp54B=_tmp54A;struct Cyc_Absyn_Stmt*_tmp5E6;struct Cyc_Absyn_Decl*_tmp5E5;struct Cyc_Absyn_Stmt*_tmp5E4;struct Cyc_Absyn_Vardecl*_tmp5E3;struct Cyc_Absyn_Exp*_tmp5E2;unsigned int _tmp5E1;struct Cyc_Absyn_Stmt*_tmp5E0;struct Cyc_List_List*_tmp5DF;struct Cyc_Absyn_Exp*_tmp5DE;unsigned int _tmp5DD;struct Cyc_Absyn_Stmt*_tmp5DC;struct Cyc_Absyn_Stmt*_tmp5DB;struct Cyc_List_List*_tmp5DA;void*_tmp5D9;struct Cyc_Absyn_Exp*_tmp5D8;struct Cyc_List_List*_tmp5D7;void*_tmp5D6;struct Cyc_List_List*_tmp5D5;struct Cyc_Absyn_Switch_clause*_tmp5D4;struct Cyc_Absyn_Exp*_tmp5D3;struct Cyc_Absyn_Exp*_tmp5D2;struct Cyc_Absyn_Stmt*_tmp5D1;struct Cyc_Absyn_Exp*_tmp5D0;struct Cyc_Absyn_Stmt*_tmp5CF;struct Cyc_Absyn_Stmt*_tmp5CE;struct Cyc_Absyn_Stmt*_tmp5CD;struct Cyc_Absyn_Exp*_tmp5CC;struct Cyc_Absyn_Stmt*_tmp5CB;struct Cyc_Absyn_Exp*_tmp5CA;struct Cyc_Absyn_Stmt*_tmp5C9;struct Cyc_Absyn_Stmt*_tmp5C8;struct Cyc_Absyn_Exp*_tmp5C7;struct Cyc_Absyn_Stmt*_tmp5C6;struct Cyc_Absyn_Stmt*_tmp5C5;struct Cyc_Absyn_Stmt*_tmp5C4;struct Cyc_Absyn_Stmt*_tmp5C3;struct Cyc_Absyn_Exp*_tmp5C2;struct Cyc_Absyn_Exp*_tmp5C1;switch(*((int*)_tmp54B)){case 0U: _LL4: _LL5:
 return inflow;case 3U: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp54B)->f1 == 0){_LL6: _LL7:
# 2571
 if(env->noreturn)
({void*_tmp54C=0U;({unsigned int _tmp7F7=s->loc;struct _dyneither_ptr _tmp7F6=({const char*_tmp54D="`noreturn' function might return";_tag_dyneither(_tmp54D,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp7F7,_tmp7F6,_tag_dyneither(_tmp54C,sizeof(void*),0U));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}else{_LL8: _tmp5C1=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp54B)->f1;_LL9:
# 2577
 if(env->noreturn)
({void*_tmp54E=0U;({unsigned int _tmp7F9=s->loc;struct _dyneither_ptr _tmp7F8=({const char*_tmp54F="`noreturn' function might return";_tag_dyneither(_tmp54F,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp7F9,_tmp7F8,_tag_dyneither(_tmp54E,sizeof(void*),0U));});});{
struct _tuple16 _tmp550=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp5C1));struct _tuple16 _tmp551=_tmp550;union Cyc_CfFlowInfo_FlowInfo _tmp553;void*_tmp552;_LL2D: _tmp553=_tmp551.f1;_tmp552=_tmp551.f2;_LL2E:;
({union Cyc_CfFlowInfo_FlowInfo _tmp7FA=Cyc_NewControlFlow_use_Rval(env,_tmp5C1->loc,_tmp553,_tmp552);_tmp553=_tmp7FA;});
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp553);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,_tmp553,s->loc);
return Cyc_CfFlowInfo_BottomFL();};}case 1U: _LLA: _tmp5C2=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp54B)->f1;_LLB: {
# 2586
struct _tuple17*_tmp554=rval_opt;void**_tmp55A;int _tmp559;if(_tmp554 != 0){_LL30: _tmp55A=(void**)& _tmp554->f1;_tmp559=_tmp554->f2;_LL31: {
# 2588
struct _tuple16 _tmp555=Cyc_NewControlFlow_anal_Rexp(env,_tmp559,inflow,_tmp5C2);struct _tuple16 _tmp556=_tmp555;union Cyc_CfFlowInfo_FlowInfo _tmp558;void*_tmp557;_LL35: _tmp558=_tmp556.f1;_tmp557=_tmp556.f2;_LL36:;
*_tmp55A=_tmp557;
return _tmp558;}}else{_LL32: _LL33:
# 2592
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp5C2)).f1;}_LL2F:;}case 2U: _LLC: _tmp5C4=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp54B)->f1;_tmp5C3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp54B)->f2;_LLD:
# 2596
 return({struct Cyc_NewControlFlow_AnalEnv*_tmp7FD=env;union Cyc_CfFlowInfo_FlowInfo _tmp7FC=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5C4,0);struct Cyc_Absyn_Stmt*_tmp7FB=_tmp5C3;Cyc_NewControlFlow_anal_stmt(_tmp7FD,_tmp7FC,_tmp7FB,rval_opt);});case 4U: _LLE: _tmp5C7=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp54B)->f1;_tmp5C6=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp54B)->f2;_tmp5C5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp54B)->f3;_LLF: {
# 2599
struct _tuple19 _tmp55B=Cyc_NewControlFlow_anal_test(env,inflow,_tmp5C7);struct _tuple19 _tmp55C=_tmp55B;union Cyc_CfFlowInfo_FlowInfo _tmp560;union Cyc_CfFlowInfo_FlowInfo _tmp55F;_LL38: _tmp560=_tmp55C.f1;_tmp55F=_tmp55C.f2;_LL39:;{
# 2606
union Cyc_CfFlowInfo_FlowInfo _tmp55D=Cyc_NewControlFlow_anal_stmt(env,_tmp55F,_tmp5C5,0);
union Cyc_CfFlowInfo_FlowInfo _tmp55E=Cyc_NewControlFlow_anal_stmt(env,_tmp560,_tmp5C6,0);
return Cyc_CfFlowInfo_join_flow(_tmp549,_tmp55E,_tmp55D);};}case 5U: _LL10: _tmp5CA=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp54B)->f1).f1;_tmp5C9=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp54B)->f1).f2;_tmp5C8=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp54B)->f2;_LL11: {
# 2614
struct _tuple20 _tmp561=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp5C9);struct _tuple20 _tmp562=_tmp561;union Cyc_CfFlowInfo_FlowInfo*_tmp569;_LL3B: _tmp569=_tmp562.f2;_LL3C:;{
union Cyc_CfFlowInfo_FlowInfo _tmp563=*_tmp569;
struct _tuple19 _tmp564=Cyc_NewControlFlow_anal_test(env,_tmp563,_tmp5CA);struct _tuple19 _tmp565=_tmp564;union Cyc_CfFlowInfo_FlowInfo _tmp568;union Cyc_CfFlowInfo_FlowInfo _tmp567;_LL3E: _tmp568=_tmp565.f1;_tmp567=_tmp565.f2;_LL3F:;{
union Cyc_CfFlowInfo_FlowInfo _tmp566=Cyc_NewControlFlow_anal_stmt(env,_tmp568,_tmp5C8,0);
Cyc_NewControlFlow_update_flow(env,_tmp5C9,_tmp566);
return _tmp567;};};}case 14U: _LL12: _tmp5CD=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp54B)->f1;_tmp5CC=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp54B)->f2).f1;_tmp5CB=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp54B)->f2).f2;_LL13: {
# 2624
union Cyc_CfFlowInfo_FlowInfo _tmp56A=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5CD,0);
struct _tuple20 _tmp56B=Cyc_NewControlFlow_pre_stmt_check(env,_tmp56A,_tmp5CB);struct _tuple20 _tmp56C=_tmp56B;union Cyc_CfFlowInfo_FlowInfo*_tmp572;_LL41: _tmp572=_tmp56C.f2;_LL42:;{
union Cyc_CfFlowInfo_FlowInfo _tmp56D=*_tmp572;
struct _tuple19 _tmp56E=Cyc_NewControlFlow_anal_test(env,_tmp56D,_tmp5CC);struct _tuple19 _tmp56F=_tmp56E;union Cyc_CfFlowInfo_FlowInfo _tmp571;union Cyc_CfFlowInfo_FlowInfo _tmp570;_LL44: _tmp571=_tmp56F.f1;_tmp570=_tmp56F.f2;_LL45:;
Cyc_NewControlFlow_update_flow(env,_tmp5CD,_tmp571);
return _tmp570;};}case 9U: _LL14: _tmp5D3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp54B)->f1;_tmp5D2=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp54B)->f2).f1;_tmp5D1=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp54B)->f2).f2;_tmp5D0=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp54B)->f3).f1;_tmp5CF=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp54B)->f3).f2;_tmp5CE=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp54B)->f4;_LL15: {
# 2633
union Cyc_CfFlowInfo_FlowInfo _tmp573=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp5D3)).f1;
struct _tuple20 _tmp574=Cyc_NewControlFlow_pre_stmt_check(env,_tmp573,_tmp5D1);struct _tuple20 _tmp575=_tmp574;union Cyc_CfFlowInfo_FlowInfo*_tmp581;_LL47: _tmp581=_tmp575.f2;_LL48:;{
union Cyc_CfFlowInfo_FlowInfo _tmp576=*_tmp581;
struct _tuple19 _tmp577=Cyc_NewControlFlow_anal_test(env,_tmp576,_tmp5D2);struct _tuple19 _tmp578=_tmp577;union Cyc_CfFlowInfo_FlowInfo _tmp580;union Cyc_CfFlowInfo_FlowInfo _tmp57F;_LL4A: _tmp580=_tmp578.f1;_tmp57F=_tmp578.f2;_LL4B:;{
union Cyc_CfFlowInfo_FlowInfo _tmp579=Cyc_NewControlFlow_anal_stmt(env,_tmp580,_tmp5CE,0);
struct _tuple20 _tmp57A=Cyc_NewControlFlow_pre_stmt_check(env,_tmp579,_tmp5CF);struct _tuple20 _tmp57B=_tmp57A;union Cyc_CfFlowInfo_FlowInfo*_tmp57E;_LL4D: _tmp57E=_tmp57B.f2;_LL4E:;{
union Cyc_CfFlowInfo_FlowInfo _tmp57C=*_tmp57E;
union Cyc_CfFlowInfo_FlowInfo _tmp57D=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp57C,_tmp5D0)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp5D1,_tmp57D);
return _tmp57F;};};};}case 11U: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp54B)->f2 != 0){_LL16: _tmp5D5=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp54B)->f1;_tmp5D4=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp54B)->f2;_LL17: {
# 2645
struct _tuple23 _tmp582=Cyc_NewControlFlow_anal_Rexps(env,inflow,_tmp5D5,1,1);struct _tuple23 _tmp583=_tmp582;union Cyc_CfFlowInfo_FlowInfo _tmp58A;struct Cyc_List_List*_tmp589;_LL50: _tmp58A=_tmp583.f1;_tmp589=_tmp583.f2;_LL51:;
# 2647
({union Cyc_CfFlowInfo_FlowInfo _tmp7FE=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,_tmp5D4->body);inflow=_tmp7FE;});{
# 2649
struct Cyc_List_List*_tmp584=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5D4->pat_vars))->v)).f1);
({union Cyc_CfFlowInfo_FlowInfo _tmp7FF=Cyc_NewControlFlow_add_vars(_tmp549,_tmp58A,_tmp584,_tmp549->unknown_all,s->loc,0);_tmp58A=_tmp7FF;});
# 2652
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5D4->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp585=(struct _tuple24*)x->hd;struct _tuple24*_tmp586=_tmp585;struct Cyc_Absyn_Vardecl**_tmp588;struct Cyc_Absyn_Exp*_tmp587;_LL53: _tmp588=_tmp586->f1;_tmp587=_tmp586->f2;_LL54:;
if(_tmp588 != 0){
({union Cyc_CfFlowInfo_FlowInfo _tmp806=({struct Cyc_CfFlowInfo_FlowEnv*_tmp805=_tmp549;struct Cyc_NewControlFlow_AnalEnv*_tmp804=env;union Cyc_CfFlowInfo_FlowInfo _tmp803=_tmp58A;struct Cyc_Absyn_Vardecl*_tmp802=*_tmp588;struct Cyc_Absyn_Exp*_tmp801=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5D5))->hd;void*_tmp800=(void*)((struct Cyc_List_List*)_check_null(_tmp589))->hd;Cyc_NewControlFlow_do_initialize_var(_tmp805,_tmp804,_tmp803,_tmp802,_tmp801,_tmp800,s->loc);});_tmp58A=_tmp806;});
_tmp589=_tmp589->tl;
_tmp5D5=_tmp5D5->tl;}}}
# 2660
Cyc_NewControlFlow_update_flow(env,_tmp5D4->body,_tmp58A);
return Cyc_CfFlowInfo_BottomFL();};}}else{_LL2A: _LL2B:
# 2827
({void*_tmp5BF=0U;({struct _dyneither_ptr _tmp807=({const char*_tmp5C0="anal_stmt: bad stmt syntax or unimplemented stmt form";_tag_dyneither(_tmp5C0,sizeof(char),54U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp807,_tag_dyneither(_tmp5BF,sizeof(void*),0U));});});}case 6U: _LL18: _LL19:
# 2666
 if(((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->succ_table,s)== 0){
{union Cyc_CfFlowInfo_FlowInfo _tmp58B=inflow;if((_tmp58B.ReachableFL).tag == 2){_LL56: _LL57:
# 2669
{void*_tmp58C=Cyc_Tcutil_compress(env->return_type);void*_tmp58D=_tmp58C;switch(*((int*)_tmp58D)){case 0U: _LL5B: _LL5C:
 goto _LL5A;case 7U: _LL5D: _LL5E:
 goto _LL60;case 6U: _LL5F: _LL60:
# 2673
({void*_tmp58E=0U;({unsigned int _tmp809=s->loc;struct _dyneither_ptr _tmp808=({const char*_tmp58F="break may cause function not to return a value";_tag_dyneither(_tmp58F,sizeof(char),47U);});Cyc_Warn_warn(_tmp809,_tmp808,_tag_dyneither(_tmp58E,sizeof(void*),0U));});});
goto _LL5A;default: _LL61: _LL62:
# 2676
({void*_tmp590=0U;({unsigned int _tmp80B=s->loc;struct _dyneither_ptr _tmp80A=({const char*_tmp591="break may cause function not to return a value";_tag_dyneither(_tmp591,sizeof(char),47U);});Cyc_CfFlowInfo_aerr(_tmp80B,_tmp80A,_tag_dyneither(_tmp590,sizeof(void*),0U));});});
goto _LL5A;}_LL5A:;}
# 2679
goto _LL55;}else{_LL58: _LL59:
 goto _LL55;}_LL55:;}
# 2682
if(env->noreturn)
({void*_tmp592=0U;({unsigned int _tmp80D=s->loc;struct _dyneither_ptr _tmp80C=({const char*_tmp593="`noreturn' function might return";_tag_dyneither(_tmp593,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp80D,_tmp80C,_tag_dyneither(_tmp592,sizeof(void*),0U));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}
# 2688
goto _LL1B;case 7U: _LL1A: _LL1B:
 goto _LL1D;case 8U: _LL1C: _LL1D: {
# 2692
struct Cyc_Absyn_Stmt*_tmp594=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->succ_table,s);
if(_tmp594 == 0)
({void*_tmp595=0U;({unsigned int _tmp80F=s->loc;struct _dyneither_ptr _tmp80E=({const char*_tmp596="jump has no target (should be impossible)";_tag_dyneither(_tmp596,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp80F,_tmp80E,_tag_dyneither(_tmp595,sizeof(void*),0U));});});
({union Cyc_CfFlowInfo_FlowInfo _tmp810=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,(struct Cyc_Absyn_Stmt*)_check_null(_tmp594));inflow=_tmp810;});
# 2697
Cyc_NewControlFlow_update_flow(env,_tmp594,inflow);
return Cyc_CfFlowInfo_BottomFL();}case 10U: _LL1E: _tmp5D8=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp54B)->f1;_tmp5D7=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp54B)->f2;_tmp5D6=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp54B)->f3;_LL1F:
# 2703
 return Cyc_NewControlFlow_anal_scs(env,inflow,_tmp5D7,_tmp5D8->loc);case 15U: _LL20: _tmp5DB=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp54B)->f1;_tmp5DA=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp54B)->f2;_tmp5D9=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp54B)->f3;_LL21: {
# 2708
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5DB,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2716
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2719
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp5DA,0U);
{union Cyc_CfFlowInfo_FlowInfo _tmp597=scs_outflow;if((_tmp597.BottomFL).tag == 1){_LL64: _LL65:
 goto _LL63;}else{_LL66: _LL67:
({void*_tmp598=0U;({unsigned int _tmp812=s->loc;struct _dyneither_ptr _tmp811=({const char*_tmp599="last catch clause may implicitly fallthru";_tag_dyneither(_tmp599,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp812,_tmp811,_tag_dyneither(_tmp598,sizeof(void*),0U));});});}_LL63:;}
# 2725
outflow=s1_outflow;
# 2727
return outflow;};};}case 12U: switch(*((int*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54B)->f1)->r)){case 2U: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54B)->f1)->r)->f2 != 0){_LL22: _tmp5DF=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54B)->f1)->r)->f2)->v;_tmp5DE=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54B)->f1)->r)->f3;_tmp5DD=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54B)->f1)->loc;_tmp5DC=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54B)->f2;_LL23: {
# 2737
struct _tuple1 _tmp59A=Cyc_NewControlFlow_get_unconsume_pat_vars(_tmp5DF);struct _tuple1 _tmp59B=_tmp59A;struct Cyc_List_List*_tmp5A0;struct Cyc_List_List*_tmp59F;_LL69: _tmp5A0=_tmp59B.f1;_tmp59F=_tmp59B.f2;_LL6A:;
({union Cyc_CfFlowInfo_FlowInfo _tmp813=Cyc_NewControlFlow_initialize_pat_vars(_tmp549,env,inflow,_tmp5DF,_tmp5A0 != 0,_tmp5DD,_tmp5DE->loc);inflow=_tmp813;});{
struct Cyc_List_List*_tmp59C=env->unique_pat_vars;
# 2741
({struct Cyc_List_List*_tmp815=({struct Cyc_List_List*_tmp59D=_cycalloc(sizeof(*_tmp59D));({struct _tuple29*_tmp814=({struct _tuple29*_tmp59E=_cycalloc(sizeof(*_tmp59E));_tmp59E->f1=0;_tmp59E->f2=s;_tmp59E->f3=_tmp5A0;_tmp59E->f4=_tmp59F;_tmp59E;});_tmp59D->hd=_tmp814;});_tmp59D->tl=_tmp59C;_tmp59D;});env->unique_pat_vars=_tmp815;});
# 2746
({union Cyc_CfFlowInfo_FlowInfo _tmp816=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5DC,rval_opt);inflow=_tmp816;});
env->unique_pat_vars=_tmp59C;
# 2750
({union Cyc_CfFlowInfo_FlowInfo _tmp817=Cyc_NewControlFlow_unconsume_params(env,_tmp5A0,_tmp59F,0,inflow,_tmp5DD);inflow=_tmp817;});
# 2754
return inflow;};}}else{goto _LL26;}case 4U: _LL24: _tmp5E3=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54B)->f1)->r)->f2;_tmp5E2=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54B)->f1)->r)->f3;_tmp5E1=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54B)->f1)->loc;_tmp5E0=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54B)->f2;if(_tmp5E2 != 0){_LL25: {
# 2766
struct Cyc_List_List l=({struct Cyc_List_List _tmp5BE;_tmp5BE.hd=_tmp5E2;_tmp5BE.tl=0;_tmp5BE;});
union Cyc_CfFlowInfo_FlowInfo _tmp5A1=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp5A2=Cyc_NewControlFlow_anal_Lexp(env,_tmp5A1,0,0,_tmp5E2);struct _tuple18 _tmp5A3=_tmp5A2;union Cyc_CfFlowInfo_AbsLVal _tmp5BD;_LL6C: _tmp5BD=_tmp5A3.f2;_LL6D:;{
struct _tuple16 _tmp5A4=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp5A1,_tmp5E2);struct _tuple16 _tmp5A5=_tmp5A4;union Cyc_CfFlowInfo_FlowInfo _tmp5BC;_LL6F: _tmp5BC=_tmp5A5.f1;_LL70:;{
struct Cyc_List_List*_tmp5A6=0;
struct Cyc_List_List*_tmp5A7=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp5A8=_tmp5BC;struct Cyc_Dict_Dict _tmp5B6;struct Cyc_List_List*_tmp5B5;if((_tmp5A8.ReachableFL).tag == 2){_LL72: _tmp5B6=((_tmp5A8.ReachableFL).val).f1;_tmp5B5=((_tmp5A8.ReachableFL).val).f2;_LL73:
# 2774
{union Cyc_CfFlowInfo_AbsLVal _tmp5A9=_tmp5BD;struct Cyc_CfFlowInfo_Place*_tmp5B4;if((_tmp5A9.PlaceL).tag == 1){_LL77: _tmp5B4=(_tmp5A9.PlaceL).val;_LL78: {
# 2778
void*_tmp5AA=Cyc_CfFlowInfo_lookup_place(_tmp5B6,_tmp5B4);
({void*_tmp819=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5AB=_cycalloc(sizeof(*_tmp5AB));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp818=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp5AC;_tmp5AC.tag=8U;_tmp5AC.f1=_tmp5E3;_tmp5AC.f2=_tmp5AA;_tmp5AC;});_tmp5AB[0]=_tmp818;});_tmp5AB;});_tmp5AA=_tmp819;});
({struct Cyc_Dict_Dict _tmp81A=Cyc_CfFlowInfo_assign_place(_tmp549,_tmp5E2->loc,_tmp5B6,_tmp5B4,_tmp5AA);_tmp5B6=_tmp81A;});
({union Cyc_CfFlowInfo_FlowInfo _tmp81B=Cyc_CfFlowInfo_ReachableFL(_tmp5B6,_tmp5B5);_tmp5BC=_tmp81B;});{
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp5AD=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp5B2=_cycalloc(sizeof(*_tmp5B2));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp81C=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp5B3;_tmp5B3.tag=0U;_tmp5B3.f1=_tmp5E3;_tmp5B3;});_tmp5B2[0]=_tmp81C;});_tmp5B2;});
struct Cyc_CfFlowInfo_Place*_tmp5AE=({struct Cyc_CfFlowInfo_Place*_tmp5B1=_cycalloc(sizeof(*_tmp5B1));_tmp5B1->root=(void*)_tmp5AD;_tmp5B1->path=0;_tmp5B1;});
({struct Cyc_List_List*_tmp81D=({struct Cyc_List_List*_tmp5AF=_cycalloc(sizeof(*_tmp5AF));_tmp5AF->hd=_tmp5AE;_tmp5AF->tl=_tmp5A6;_tmp5AF;});_tmp5A6=_tmp81D;});
({struct Cyc_List_List*_tmp81E=({struct Cyc_List_List*_tmp5B0=_cycalloc(sizeof(*_tmp5B0));_tmp5B0->hd=_tmp5E2;_tmp5B0->tl=_tmp5A7;_tmp5B0;});_tmp5A7=_tmp81E;});
goto _LL76;};}}else{_LL79: _LL7A:
# 2793
 goto _LL76;}_LL76:;}
# 2795
goto _LL71;}else{_LL74: _LL75:
# 2797
 goto _LL71;}_LL71:;}{
# 2800
struct Cyc_List_List _tmp5B7=({struct Cyc_List_List _tmp5BB;_tmp5BB.hd=_tmp5E3;_tmp5BB.tl=0;_tmp5BB;});
({union Cyc_CfFlowInfo_FlowInfo _tmp81F=Cyc_NewControlFlow_add_vars(_tmp549,_tmp5BC,& _tmp5B7,_tmp549->unknown_all,_tmp5E1,0);_tmp5BC=_tmp81F;});{
# 2804
struct Cyc_List_List*_tmp5B8=env->unique_pat_vars;
({struct Cyc_List_List*_tmp821=({struct Cyc_List_List*_tmp5B9=_cycalloc(sizeof(*_tmp5B9));({struct _tuple29*_tmp820=({struct _tuple29*_tmp5BA=_cycalloc(sizeof(*_tmp5BA));_tmp5BA->f1=1;_tmp5BA->f2=s;_tmp5BA->f3=_tmp5A6;_tmp5BA->f4=_tmp5A7;_tmp5BA;});_tmp5B9->hd=_tmp820;});_tmp5B9->tl=_tmp5B8;_tmp5B9;});env->unique_pat_vars=_tmp821;});
# 2809
({union Cyc_CfFlowInfo_FlowInfo _tmp822=Cyc_NewControlFlow_anal_stmt(env,_tmp5BC,_tmp5E0,rval_opt);_tmp5BC=_tmp822;});
env->unique_pat_vars=_tmp5B8;
# 2813
({union Cyc_CfFlowInfo_FlowInfo _tmp823=Cyc_NewControlFlow_unconsume_params(env,_tmp5A6,_tmp5A7,1,_tmp5BC,_tmp5E1);_tmp5BC=_tmp823;});
# 2817
return _tmp5BC;};};};};}}else{goto _LL26;}default: _LL26: _tmp5E5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54B)->f1;_tmp5E4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54B)->f2;_LL27:
# 2820
({union Cyc_CfFlowInfo_FlowInfo _tmp827=({struct Cyc_NewControlFlow_AnalEnv*_tmp826=env;union Cyc_CfFlowInfo_FlowInfo _tmp825=Cyc_NewControlFlow_anal_decl(env,inflow,_tmp5E5);struct Cyc_Absyn_Stmt*_tmp824=_tmp5E4;Cyc_NewControlFlow_anal_stmt(_tmp826,_tmp825,_tmp824,rval_opt);});outflow=_tmp827;});
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp5E5);
return outflow;}default: _LL28: _tmp5E6=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp54B)->f2;_LL29:
# 2825
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5E6,rval_opt);}_LL3:;};}
# 2831
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2835
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp5E9=env->fenv;
void*_tmp5EA=decl->r;void*_tmp5EB=_tmp5EA;struct Cyc_Absyn_Tvar*_tmp600;struct Cyc_Absyn_Vardecl*_tmp5FF;struct Cyc_Absyn_Exp*_tmp5FE;struct Cyc_Absyn_Fndecl*_tmp5FD;struct Cyc_List_List*_tmp5FC;struct Cyc_Absyn_Vardecl*_tmp5FB;switch(*((int*)_tmp5EB)){case 0U: _LL1: _tmp5FB=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp5EB)->f1;_LL2: {
# 2845
struct Cyc_List_List _tmp5EC=({struct Cyc_List_List _tmp5F2;_tmp5F2.hd=_tmp5FB;_tmp5F2.tl=0;_tmp5F2;});
({union Cyc_CfFlowInfo_FlowInfo _tmp828=Cyc_NewControlFlow_add_vars(_tmp5E9,inflow,& _tmp5EC,_tmp5E9->unknown_none,decl->loc,0);inflow=_tmp828;});{
struct Cyc_Absyn_Exp*_tmp5ED=_tmp5FB->initializer;
if(_tmp5ED == 0)
return inflow;{
struct _tuple16 _tmp5EE=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp5ED);struct _tuple16 _tmp5EF=_tmp5EE;union Cyc_CfFlowInfo_FlowInfo _tmp5F1;void*_tmp5F0;_LLC: _tmp5F1=_tmp5EF.f1;_tmp5F0=_tmp5EF.f2;_LLD:;
return Cyc_NewControlFlow_do_initialize_var(_tmp5E9,env,_tmp5F1,_tmp5FB,_tmp5ED,_tmp5F0,decl->loc);};};}case 3U: _LL3: _tmp5FC=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp5EB)->f1;_LL4:
# 2854
 return Cyc_NewControlFlow_add_vars(_tmp5E9,inflow,_tmp5FC,_tmp5E9->unknown_none,decl->loc,0);case 1U: _LL5: _tmp5FD=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp5EB)->f1;_LL6:
# 2857
 Cyc_NewControlFlow_check_nested_fun(env->all_tables,_tmp5E9,inflow,_tmp5FD);{
void*t=(void*)_check_null(_tmp5FD->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp5F3=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp5FD->fn_vardecl);
# 2863
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp82C=_tmp5E9;union Cyc_CfFlowInfo_FlowInfo _tmp82B=inflow;struct Cyc_List_List*_tmp82A=({struct Cyc_List_List*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4->hd=_tmp5F3;_tmp5F4->tl=0;_tmp5F4;});void*_tmp829=_tmp5E9->unknown_all;Cyc_NewControlFlow_add_vars(_tmp82C,_tmp82B,_tmp82A,_tmp829,decl->loc,0);});};case 4U: _LL7: _tmp600=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5EB)->f1;_tmp5FF=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5EB)->f2;_tmp5FE=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5EB)->f3;_LL8:
# 2866
 if(_tmp5FE != 0)
({void*_tmp5F5=0U;({struct _dyneither_ptr _tmp82D=({const char*_tmp5F6="found open expression in declaration!";_tag_dyneither(_tmp5F6,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp82D,_tag_dyneither(_tmp5F5,sizeof(void*),0U));});});{
struct Cyc_List_List _tmp5F7=({struct Cyc_List_List _tmp5F8;_tmp5F8.hd=_tmp5FF;_tmp5F8.tl=0;_tmp5F8;});
return Cyc_NewControlFlow_add_vars(_tmp5E9,inflow,& _tmp5F7,_tmp5E9->unknown_all,decl->loc,0);};default: _LL9: _LLA:
# 2872
({void*_tmp5F9=0U;({struct _dyneither_ptr _tmp82E=({const char*_tmp5FA="anal_decl: unexpected decl variant";_tag_dyneither(_tmp5FA,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp82E,_tag_dyneither(_tmp5F9,sizeof(void*),0U));});});}_LL0:;}
# 2880
static void Cyc_NewControlFlow_check_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd){
struct _handler_cons _tmp601;_push_handler(& _tmp601);{int _tmp603=0;if(setjmp(_tmp601.handler))_tmp603=1;if(!_tmp603){
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env();
({struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmp831=tables;struct Cyc_CfFlowInfo_FlowEnv*_tmp830=fenv;union Cyc_CfFlowInfo_FlowInfo _tmp82F=Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0);Cyc_NewControlFlow_check_nested_fun(_tmp831,_tmp830,_tmp82F,fd);});}
# 2882
;_pop_handler();}else{void*_tmp602=(void*)_exn_thrown;void*_tmp604=_tmp602;void*_tmp607;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp604)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 2888
 if(Cyc_Position_num_errors > 0)
goto _LL0;else{
Cyc_Core_rethrow((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp605=_cycalloc_atomic(sizeof(*_tmp605));({struct Cyc_Dict_Absent_exn_struct _tmp832=({struct Cyc_Dict_Absent_exn_struct _tmp606;_tmp606.tag=Cyc_Dict_Absent;_tmp606;});_tmp605[0]=_tmp832;});_tmp605;}));}}else{_LL3: _tmp607=_tmp604;_LL4:(int)_rethrow(_tmp607);}_LL0:;}};}
# 2894
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2899
static union Cyc_Relations_RelnOp Cyc_NewControlFlow_translate_rop(struct Cyc_List_List*vds,union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp608=r;unsigned int _tmp60A;if((_tmp608.RParam).tag == 5){_LL1: _tmp60A=(_tmp608.RParam).val;_LL2: {
# 2902
struct Cyc_Absyn_Vardecl*_tmp609=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(vds,(int)_tmp60A);
if(!_tmp609->escapes)
return Cyc_Relations_RVar(_tmp609);
return r;}}else{_LL3: _LL4:
 return r;}_LL0:;}
# 2910
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2914
unsigned int _tmp60B=(fd->body)->loc;
({union Cyc_CfFlowInfo_FlowInfo _tmp833=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp60B,1);inflow=_tmp833;});{
# 2919
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_roots=0;
struct _tuple14 _tmp60C=({union Cyc_CfFlowInfo_FlowInfo _tmp644=inflow;if((_tmp644.ReachableFL).tag != 2)_throw_match();(_tmp644.ReachableFL).val;});struct _tuple14 _tmp60D=_tmp60C;struct Cyc_Dict_Dict _tmp643;struct Cyc_List_List*_tmp642;_LL1: _tmp643=_tmp60D.f1;_tmp642=_tmp60D.f2;_LL2:;{
# 2924
struct Cyc_List_List*_tmp60E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;
{struct Cyc_List_List*_tmp60F=fd->requires_relns;for(0;_tmp60F != 0;_tmp60F=_tmp60F->tl){
struct Cyc_Relations_Reln*_tmp610=(struct Cyc_Relations_Reln*)_tmp60F->hd;
({struct Cyc_List_List*_tmp838=({struct _RegionHandle*_tmp837=Cyc_Core_heap_region;union Cyc_Relations_RelnOp _tmp836=Cyc_NewControlFlow_translate_rop(_tmp60E,_tmp610->rop1);enum Cyc_Relations_Relation _tmp835=_tmp610->relation;union Cyc_Relations_RelnOp _tmp834=
Cyc_NewControlFlow_translate_rop(_tmp60E,_tmp610->rop2);
# 2927
Cyc_Relations_add_relation(_tmp837,_tmp836,_tmp835,_tmp834,_tmp642);});_tmp642=_tmp838;});}}{
# 2933
struct Cyc_List_List*atts;
{void*_tmp611=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));void*_tmp612=_tmp611;struct Cyc_List_List*_tmp615;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp612)->tag == 9U){_LL4: _tmp615=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp612)->f1).attributes;_LL5:
 atts=_tmp615;goto _LL3;}else{_LL6: _LL7:
({void*_tmp613=0U;({struct _dyneither_ptr _tmp839=({const char*_tmp614="check_fun: non-function type cached with fndecl_t";_tag_dyneither(_tmp614,sizeof(char),50U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp839,_tag_dyneither(_tmp613,sizeof(void*),0U));});});}_LL3:;}
# 2940
for(0;atts != 0;atts=atts->tl){
void*_tmp616=(void*)atts->hd;void*_tmp617=_tmp616;int _tmp637;int _tmp636;int _tmp635;switch(*((int*)_tmp617)){case 21U: _LL9: _tmp635=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp617)->f1;_LLA: {
# 2943
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0U);
struct Cyc_Absyn_Vardecl*_tmp618=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp635 - 1);
void*t=Cyc_Tcutil_compress(_tmp618->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp619=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp83C=fenv;void*_tmp83B=elttype;struct Cyc_Absyn_Exp*_tmp83A=bogus_exp;Cyc_CfFlowInfo_make_unique_consumed(_tmp83C,_tmp83B,_tmp83A,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));});
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp61A=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp621=_cycalloc(sizeof(*_tmp621));({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp83D=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp622;_tmp622.tag=2U;_tmp622.f1=_tmp635;_tmp622.f2=t;_tmp622;});_tmp621[0]=_tmp83D;});_tmp621;});
struct Cyc_CfFlowInfo_Place*_tmp61B=({struct Cyc_CfFlowInfo_Place*_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620->root=(void*)_tmp61A;_tmp620->path=0;_tmp620;});
({struct Cyc_Dict_Dict _tmp83E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp643,(void*)_tmp61A,_tmp619);_tmp643=_tmp83E;});
({struct Cyc_Dict_Dict _tmp843=({struct Cyc_Dict_Dict _tmp842=_tmp643;void*_tmp841=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp61C=_cycalloc(sizeof(*_tmp61C));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp83F=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp61D;_tmp61D.tag=0U;_tmp61D.f1=_tmp618;_tmp61D;});_tmp61C[0]=_tmp83F;});_tmp61C;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp842,_tmp841,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp61E=_cycalloc(sizeof(*_tmp61E));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp840=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp61F;_tmp61F.tag=4U;_tmp61F.f1=_tmp61B;_tmp61F;});_tmp61E[0]=_tmp840;});_tmp61E;}));});_tmp643=_tmp843;});
goto _LL8;}case 20U: _LLB: _tmp636=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp617)->f1;_LLC: {
# 2956
struct Cyc_Absyn_Vardecl*_tmp623=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp636 - 1);
void*elttype=Cyc_Tcutil_pointer_elt_type(_tmp623->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp624=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp62C=_cycalloc(sizeof(*_tmp62C));({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp844=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp62D;_tmp62D.tag=2U;_tmp62D.f1=_tmp636;_tmp62D.f2=elttype;_tmp62D;});_tmp62C[0]=_tmp844;});_tmp62C;});
struct Cyc_CfFlowInfo_Place*_tmp625=({struct Cyc_CfFlowInfo_Place*_tmp62B=_cycalloc(sizeof(*_tmp62B));_tmp62B->root=(void*)_tmp624;_tmp62B->path=0;_tmp62B;});
({struct Cyc_Dict_Dict _tmp847=({struct Cyc_Dict_Dict _tmp846=_tmp643;void*_tmp845=(void*)_tmp624;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp846,_tmp845,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));});_tmp643=_tmp847;});
({struct Cyc_Dict_Dict _tmp84C=({struct Cyc_Dict_Dict _tmp84B=_tmp643;void*_tmp84A=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp626=_cycalloc(sizeof(*_tmp626));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp848=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp627;_tmp627.tag=0U;_tmp627.f1=_tmp623;_tmp627;});_tmp626[0]=_tmp848;});_tmp626;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp84B,_tmp84A,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp628=_cycalloc(sizeof(*_tmp628));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp849=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp629;_tmp629.tag=4U;_tmp629.f1=_tmp625;_tmp629;});_tmp628[0]=_tmp849;});_tmp628;}));});_tmp643=_tmp84C;});
({struct Cyc_List_List*_tmp84D=({struct Cyc_List_List*_tmp62A=_cycalloc(sizeof(*_tmp62A));_tmp62A->hd=_tmp625;_tmp62A->tl=param_roots;_tmp62A;});param_roots=_tmp84D;});
goto _LL8;}case 22U: _LLD: _tmp637=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp617)->f1;_LLE: {
# 2965
struct Cyc_Absyn_Vardecl*_tmp62E=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp637 - 1);
if(Cyc_Tcutil_is_noalias_pointer(_tmp62E->type,0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp62F=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp633=_cycalloc(sizeof(*_tmp633));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp84E=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp634;_tmp634.tag=0U;_tmp634.f1=_tmp62E;_tmp634;});_tmp633[0]=_tmp84E;});_tmp633;});
struct Cyc_CfFlowInfo_Place*_tmp630=({struct Cyc_CfFlowInfo_Place*_tmp632=_cycalloc(sizeof(*_tmp632));_tmp632->root=(void*)_tmp62F;_tmp632->path=0;_tmp632;});
({struct Cyc_List_List*_tmp84F=({struct Cyc_List_List*_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631->hd=_tmp630;_tmp631->tl=noconsume_roots;_tmp631;});noconsume_roots=_tmp84F;});}
# 2971
goto _LL8;}default: _LLF: _LL10:
 goto _LL8;}_LL8:;}
# 2974
({union Cyc_CfFlowInfo_FlowInfo _tmp850=Cyc_CfFlowInfo_ReachableFL(_tmp643,_tmp642);inflow=_tmp850;});{
# 2976
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create)(33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*_tmp641=_cycalloc(sizeof(*_tmp641));_tmp641->all_tables=tables;({struct Cyc_Hashtable_Table*_tmp851=
# 2981
((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->succ_tables,fd);_tmp641->succ_table=_tmp851;});({struct Cyc_Hashtable_Table*_tmp852=
((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->pat_pop_tables,fd);_tmp641->pat_pop_table=_tmp852;});_tmp641->fenv=fenv;_tmp641->iterate_again=1;_tmp641->iteration_num=0;_tmp641->in_try=0;_tmp641->tryflow=inflow;_tmp641->noreturn=noreturn;_tmp641->return_type=fd->ret_type;_tmp641->unique_pat_vars=0;_tmp641->param_roots=param_roots;_tmp641->noconsume_params=noconsume_roots;_tmp641->flow_table=flow_table;_tmp641->return_relns=fd->ensures_relns;_tmp641;});
# 2986
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 2992
env->iterate_again=0;
({union Cyc_CfFlowInfo_FlowInfo _tmp853=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);outflow=_tmp853;});}{
# 2995
union Cyc_CfFlowInfo_FlowInfo _tmp638=outflow;if((_tmp638.BottomFL).tag == 1){_LL12: _LL13:
 goto _LL11;}else{_LL14: _LL15:
# 2998
 Cyc_NewControlFlow_check_init_params(_tmp60B,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,outflow,_tmp60B);
# 3002
if(noreturn)
({void*_tmp639=0U;({unsigned int _tmp855=_tmp60B;struct _dyneither_ptr _tmp854=({const char*_tmp63A="`noreturn' function might (implicitly) return";_tag_dyneither(_tmp63A,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp855,_tmp854,_tag_dyneither(_tmp639,sizeof(void*),0U));});});else{
# 3005
void*_tmp63B=Cyc_Tcutil_compress(fd->ret_type);void*_tmp63C=_tmp63B;switch(*((int*)_tmp63C)){case 0U: _LL17: _LL18:
 goto _LL16;case 7U: _LL19: _LL1A:
 goto _LL1C;case 6U: _LL1B: _LL1C:
# 3009
({void*_tmp63D=0U;({unsigned int _tmp857=_tmp60B;struct _dyneither_ptr _tmp856=({const char*_tmp63E="function may not return a value";_tag_dyneither(_tmp63E,sizeof(char),32U);});Cyc_Warn_warn(_tmp857,_tmp856,_tag_dyneither(_tmp63D,sizeof(void*),0U));});});goto _LL16;default: _LL1D: _LL1E:
# 3011
({void*_tmp63F=0U;({unsigned int _tmp859=_tmp60B;struct _dyneither_ptr _tmp858=({const char*_tmp640="function may not return a value";_tag_dyneither(_tmp640,sizeof(char),32U);});Cyc_CfFlowInfo_aerr(_tmp859,_tmp858,_tag_dyneither(_tmp63F,sizeof(void*),0U));});});goto _LL16;}_LL16:;}
# 3013
goto _LL11;}_LL11:;};};};};};}
# 3017
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_tmp645=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_tmp646=_tmp645;struct Cyc_List_List*_tmp64A;struct Cyc_List_List*_tmp649;struct Cyc_List_List*_tmp648;struct Cyc_Absyn_Fndecl*_tmp647;switch(*((int*)_tmp646)){case 1U: _LL1: _tmp647=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp646)->f1;_LL2:
 Cyc_NewControlFlow_check_fun(tables,_tmp647);goto _LL0;case 11U: _LL3: _tmp648=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp646)->f1;_LL4:
 _tmp649=_tmp648;goto _LL6;case 10U: _LL5: _tmp649=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp646)->f2;_LL6:
 _tmp64A=_tmp649;goto _LL8;case 9U: _LL7: _tmp64A=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp646)->f2;_LL8:
 Cyc_NewControlFlow_cf_check(tables,_tmp64A);goto _LL0;case 12U: _LL9: _LLA:
 goto _LL0;default: _LLB: _LLC:
 goto _LL0;}_LL0:;};}}
