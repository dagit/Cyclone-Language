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
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 174
void Cyc_CfFlowInfo_print_absrval(void*rval);
# 181
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f);
# 196 "cf_flowinfo.h"
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,void*r);
# 199
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place,void*r);
# 203
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
({void*_tmp68D=(void*)({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp2=_cycalloc_atomic(sizeof(*_tmp2));({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp68C=({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp3;_tmp3.tag=Cyc_NewControlFlow_CFAnnot;({struct Cyc_NewControlFlow_CFStmtAnnot _tmp68B=({struct Cyc_NewControlFlow_CFStmtAnnot _tmp4;_tmp4.visited=0;_tmp4;});_tmp3.f1=_tmp68B;});_tmp3;});_tmp2[0]=_tmp68C;});_tmp2;});s->annot=_tmp68D;});return Cyc_NewControlFlow_get_stmt_annot(s);}_LL0:;}
# 153
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*res=({union Cyc_CfFlowInfo_FlowInfo*_tmp6=_region_malloc(env->r,sizeof(*_tmp6));({union Cyc_CfFlowInfo_FlowInfo _tmp68E=Cyc_CfFlowInfo_BottomFL();_tmp6[0]=_tmp68E;});_tmp6;});
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);
return res;}
# 160
return*sflow;}struct _tuple20{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 163
static struct _tuple20 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp7=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp8=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 171
({union Cyc_CfFlowInfo_FlowInfo _tmp68F=Cyc_CfFlowInfo_join_flow(env->fenv,inflow,*_tmp8);*_tmp8=_tmp68F;});
# 177
_tmp7->visited=env->iteration_num;
return({struct _tuple20 _tmp9;_tmp9.f1=_tmp7;_tmp9.f2=_tmp8;_tmp9;});}
# 187
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 195
({union Cyc_CfFlowInfo_FlowInfo _tmp690=Cyc_CfFlowInfo_join_flow(env->fenv,new_flow,env->tryflow);env->tryflow=_tmp690;});}struct _tuple21{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned int f2;struct Cyc_Dict_Dict f3;};
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
({struct Cyc_String_pa_PrintArg_struct _tmpF;_tmpF.tag=0;({struct _dyneither_ptr _tmp691=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp11->name));_tmpF.f1=_tmp691;});({void*_tmpD[1]={& _tmpF};({unsigned int _tmp693=_tmp13;struct _dyneither_ptr _tmp692=({const char*_tmpE="alias-free pointer(s) reachable from %s may become unreachable.";_tag_dyneither(_tmpE,sizeof(char),64);});Cyc_Tcutil_warn(_tmp693,_tmp692,_tag_dyneither(_tmpD,sizeof(void*),1));});});});
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
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp20=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp24=_region_malloc(fenv->r,sizeof(*_tmp24));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp694=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp25;_tmp25.tag=0;_tmp25.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp25;});_tmp24[0]=_tmp694;});_tmp24;});
void*_tmp21=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit)
({void*_tmp696=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp22=_region_malloc(fenv->r,sizeof(*_tmp22));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp695=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp23;_tmp23.tag=8;_tmp23.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp23.f2=_tmp21;_tmp23;});_tmp22[0]=_tmp695;});_tmp22;});_tmp21=_tmp696;});
# 275
({struct Cyc_Dict_Dict _tmp697=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp27,(void*)_tmp20,_tmp21);_tmp27=_tmp697;});}
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
{struct Cyc_Absyn_Exp*_tmp34=({struct _RegionHandle*_tmp699=temp;struct Cyc_List_List*_tmp698=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp30->tvs,_tmp48);Cyc_Tcutil_rsubsexp(_tmp699,_tmp698,_tmp32);});
# 317
if(!({struct _RegionHandle*_tmp69B=r;struct Cyc_List_List*_tmp69A=_tmp4B;Cyc_NewControlFlow_relns_ok(_tmp69B,_tmp69A,Cyc_Relations_exp2relns(temp,_tmp34));})){
({struct Cyc_String_pa_PrintArg_struct _tmp38;_tmp38.tag=0;_tmp38.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp37;_tmp37.tag=0;({struct _dyneither_ptr _tmp69C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp34)));_tmp37.f1=_tmp69C;});({void*_tmp35[2]={& _tmp37,& _tmp38};({unsigned int _tmp69E=loc;struct _dyneither_ptr _tmp69D=({const char*_tmp36="unable to prove %s, required to access %s";_tag_dyneither(_tmp36,sizeof(char),42);});Cyc_CfFlowInfo_aerr(_tmp69E,_tmp69D,_tag_dyneither(_tmp35,sizeof(void*),2));});});});});
({void*_tmp39=0;({struct Cyc___cycFILE*_tmp6A0=Cyc_stderr;struct _dyneither_ptr _tmp69F=({const char*_tmp3A="  [recorded facts on path: ";_tag_dyneither(_tmp3A,sizeof(char),28);});Cyc_fprintf(_tmp6A0,_tmp69F,_tag_dyneither(_tmp39,sizeof(void*),0));});});
Cyc_Relations_print_relns(Cyc_stderr,_tmp4B);
({void*_tmp3B=0;({struct Cyc___cycFILE*_tmp6A2=Cyc_stderr;struct _dyneither_ptr _tmp6A1=({const char*_tmp3C="]\n";_tag_dyneither(_tmp3C,sizeof(char),3);});Cyc_fprintf(_tmp6A2,_tmp6A1,_tag_dyneither(_tmp3B,sizeof(void*),0));});});}}
# 315
;_pop_region(temp);}
# 324
goto _LL2C;}case 12: _LL2F: _tmp4A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2F)->f2;_LL30: {
# 326
struct Cyc_Absyn_Aggrfield*_tmp3D=Cyc_Absyn_lookup_field(_tmp4A,f);
struct Cyc_Absyn_Exp*_tmp3E=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3D))->requires_clause;
if(_tmp3E != 0){
struct _RegionHandle _tmp3F=_new_region("temp");struct _RegionHandle*temp=& _tmp3F;_push_region(temp);
if(!({struct _RegionHandle*_tmp6A4=r;struct Cyc_List_List*_tmp6A3=_tmp4B;Cyc_NewControlFlow_relns_ok(_tmp6A4,_tmp6A3,Cyc_Relations_exp2relns(temp,_tmp3E));})){
({struct Cyc_String_pa_PrintArg_struct _tmp43;_tmp43.tag=0;_tmp43.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp42;_tmp42.tag=0;({struct _dyneither_ptr _tmp6A5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp3E)));_tmp42.f1=_tmp6A5;});({void*_tmp40[2]={& _tmp42,& _tmp43};({unsigned int _tmp6A7=loc;struct _dyneither_ptr _tmp6A6=({const char*_tmp41="unable to prove %s, required to access %s";_tag_dyneither(_tmp41,sizeof(char),42);});Cyc_CfFlowInfo_aerr(_tmp6A7,_tmp6A6,_tag_dyneither(_tmp40,sizeof(void*),2));});});});});
({void*_tmp44=0;({struct Cyc___cycFILE*_tmp6A9=Cyc_stderr;struct _dyneither_ptr _tmp6A8=({const char*_tmp45="  [recorded facts on path: ";_tag_dyneither(_tmp45,sizeof(char),28);});Cyc_fprintf(_tmp6A9,_tmp6A8,_tag_dyneither(_tmp44,sizeof(void*),0));});});
Cyc_Relations_print_relns(Cyc_stderr,_tmp4B);
({void*_tmp46=0;({struct Cyc___cycFILE*_tmp6AB=Cyc_stderr;struct _dyneither_ptr _tmp6AA=({const char*_tmp47="]\n";_tag_dyneither(_tmp47,sizeof(char),3);});Cyc_fprintf(_tmp6AB,_tmp6AA,_tag_dyneither(_tmp46,sizeof(void*),0));});});}
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
({void*_tmp4D=0;({unsigned int _tmp6AD=loc;struct _dyneither_ptr _tmp6AC=({const char*_tmp4E="expression may not be fully initialized";_tag_dyneither(_tmp4E,sizeof(char),40);});Cyc_CfFlowInfo_aerr(_tmp6AD,_tmp6AC,_tag_dyneither(_tmp4D,sizeof(void*),0));});});{
struct Cyc_Dict_Dict _tmp4F=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp52,r);
if(_tmp52.t == _tmp4F.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp50=Cyc_CfFlowInfo_ReachableFL(_tmp4F,_tmp51);
Cyc_NewControlFlow_update_tryflow(env,_tmp50);
return _tmp50;};};}_LL33:;}struct _tuple22{struct Cyc_Absyn_Tqual f1;void*f2;};
# 358
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp53=({struct _tuple0 _tmp66;({void*_tmp6AE=Cyc_Tcutil_compress(t);_tmp66.f1=_tmp6AE;});_tmp66.f2=r;_tmp66;});struct _tuple0 _tmp54=_tmp53;enum Cyc_Absyn_AggrKind _tmp65;struct Cyc_List_List*_tmp64;struct _dyneither_ptr _tmp63;union Cyc_Absyn_AggrInfoU _tmp62;struct Cyc_List_List*_tmp61;struct _dyneither_ptr _tmp60;struct Cyc_List_List*_tmp5F;struct _dyneither_ptr _tmp5E;struct Cyc_Absyn_Datatypefield*_tmp5D;struct _dyneither_ptr _tmp5C;void*_tmp5B;switch(*((int*)_tmp54.f2)){case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp54.f2)->f1 == Cyc_CfFlowInfo_NoneIL){_LL39: _LL3A:
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
({struct Cyc_NewControlFlow_AnalEnv*_tmp6B1=env;unsigned int _tmp6B0=loc;void*_tmp6AF=(*((struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp5F))->hd)).f2;Cyc_NewControlFlow_check_nounique(_tmp6B1,_tmp6B0,_tmp6AF,*((void**)_check_dyneither_subscript(_tmp5E,sizeof(void*),i)));});}}
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
if(_tmp58 != 0)({void*_tmp6B2=Cyc_Tcutil_rsubstitute(temp,_tmp58,t);t=_tmp6B2;});
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
({void*_tmp59=0;({unsigned int _tmp6B4=loc;struct _dyneither_ptr _tmp6B3=({const char*_tmp5A="argument may still contain alias-free pointers";_tag_dyneither(_tmp5A,sizeof(char),47);});Cyc_Tcutil_warn(_tmp6B4,_tmp6B3,_tag_dyneither(_tmp59,sizeof(void*),0));});});
return;}else{goto _LL4B;}default: _LL4B: _LL4C:
 return;}}_LL38:;}
# 399
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp67=inflow;struct Cyc_Dict_Dict _tmp74;struct Cyc_List_List*_tmp73;if((_tmp67.BottomFL).tag == 1){_LL4E: _LL4F:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL50: _tmp74=((_tmp67.ReachableFL).val).f1;_tmp73=((_tmp67.ReachableFL).val).f2;_LL51:
# 403
 if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
({void*_tmp68=0;({struct _dyneither_ptr _tmp6B5=({const char*_tmp69="noliveunique attribute requires alias-free pointer";_tag_dyneither(_tmp69,sizeof(char),51);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp6B5,_tag_dyneither(_tmp68,sizeof(void*),0));});});
return Cyc_CfFlowInfo_BottomFL();}{
# 407
void*_tmp6A=Cyc_Tcutil_pointer_elt_type(t);
retry: {void*_tmp6B=r;void*_tmp70;struct Cyc_CfFlowInfo_Place*_tmp6F;void*_tmp6E;switch(*((int*)_tmp6B)){case 8: _LL53: _tmp6E=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp6B)->f2;_LL54:
 r=_tmp6E;goto retry;case 4: _LL55: _tmp6F=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp6B)->f1;_LL56:
({void*_tmp6B6=Cyc_CfFlowInfo_lookup_place(_tmp74,_tmp6F);_tmp70=_tmp6B6;});goto _LL58;case 5: _LL57: _tmp70=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp6B)->f1;_LL58:
 Cyc_NewControlFlow_check_nounique(env,loc,_tmp6A,_tmp70);goto _LL52;default: _LL59: _LL5A:
# 413
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp6A))
({void*_tmp6C=0;({unsigned int _tmp6B8=loc;struct _dyneither_ptr _tmp6B7=({const char*_tmp6D="argument may contain live alias-free pointers";_tag_dyneither(_tmp6D,sizeof(char),46);});Cyc_Tcutil_warn(_tmp6B8,_tmp6B7,_tag_dyneither(_tmp6C,sizeof(void*),0));});});
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
({struct Cyc_List_List*_tmp6B9=({struct Cyc_List_List*_tmp78=_region_malloc(rgn,sizeof(*_tmp78));_tmp78->hd=_tmp79;_tmp78->tl=rvals;_tmp78;});rvals=_tmp6B9;});
es=es->tl;
first_is_copy=others_are_copy;}while(es != 0);
# 447
Cyc_NewControlFlow_update_tryflow(env,inflow);
return({struct _tuple23 _tmp7B;_tmp7B.f1=inflow;({struct Cyc_List_List*_tmp6BA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals);_tmp7B.f2=_tmp6BA;});_tmp7B;});}
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
({void*_tmp80=0;({unsigned int _tmp6BC=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _dyneither_ptr _tmp6BB=({const char*_tmp81="expression may not be initialized";_tag_dyneither(_tmp81,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp6BC,_tmp6BB,_tag_dyneither(_tmp80,sizeof(void*),0));});});}
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
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp88=_cycalloc(sizeof(*_tmp88));({struct Cyc_Core_Impossible_exn_struct _tmp6BE=({struct Cyc_Core_Impossible_exn_struct _tmp89;_tmp89.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp6BD=({const char*_tmp8A="consume_zero_ral";_tag_dyneither(_tmp8A,sizeof(char),17);});_tmp89.f1=_tmp6BD;});_tmp89;});_tmp88[0]=_tmp6BE;});_tmp88;}));else{
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
({void*_tmp6C0=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8E=_region_malloc((env->fenv)->r,sizeof(*_tmp8E));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6BF=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp8F;_tmp8F.tag=8;_tmp8F.f1=(struct Cyc_Absyn_Vardecl*)names->hd;_tmp8F.f2=nzval;_tmp8F;});_tmp8E[0]=_tmp6BF;});_tmp8E;});nzval=_tmp6C0;});}
# 525
({void*_tmp6C1=Cyc_NewControlFlow_consume_zero_rval(env,_tmp93,_tmp91,e,nzval);nzval=_tmp6C1;});{
union Cyc_CfFlowInfo_FlowInfo _tmp90=({struct Cyc_Dict_Dict _tmp6C2=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp93,_tmp91,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp6C2,_tmp92);});
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
({void*_tmp6C4=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp99=_region_malloc((env->fenv)->r,sizeof(*_tmp99));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6C3=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp9A;_tmp9A.tag=8;_tmp9A.f1=(struct Cyc_Absyn_Vardecl*)names->hd;_tmp9A.f2=nzval;_tmp9A;});_tmp99[0]=_tmp6C3;});_tmp99;});nzval=_tmp6C4;});
({void*_tmp6C6=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp9B=_region_malloc((env->fenv)->r,sizeof(*_tmp9B));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6C5=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp9C;_tmp9C.tag=8;_tmp9C.f1=(struct Cyc_Absyn_Vardecl*)names->hd;_tmp9C.f2=zval;_tmp9C;});_tmp9B[0]=_tmp6C5;});_tmp9B;});zval=_tmp6C6;});}
# 554
({void*_tmp6C7=Cyc_NewControlFlow_consume_zero_rval(env,_tmpA0,_tmp9E,e,nzval);nzval=_tmp6C7;});
({void*_tmp6C8=Cyc_NewControlFlow_consume_zero_rval(env,_tmpA0,_tmp9E,e,zval);zval=_tmp6C8;});
return({struct _tuple19 _tmp9D;({union Cyc_CfFlowInfo_FlowInfo _tmp6CA=({
struct Cyc_Dict_Dict _tmp6C9=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA0,_tmp9E,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp6C9,_tmp9F);});_tmp9D.f1=_tmp6CA;});({union Cyc_CfFlowInfo_FlowInfo _tmp6CC=({
struct Cyc_Dict_Dict _tmp6CB=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpA0,_tmp9E,zval);Cyc_CfFlowInfo_ReachableFL(_tmp6CB,_tmp9F);});_tmp9D.f2=_tmp6CC;});_tmp9D;});}}_LL75:;}}_LL70:;}
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
({void*_tmp6CF=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6CE=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmpAA;_tmpAA.tag=Cyc_CfFlowInfo_UnknownZ;({struct Cyc_List_List*_tmp6CD=Cyc_Relations_copy_relns(Cyc_Core_heap_region,relns);_tmpAA.f1=_tmp6CD;});_tmpAA;});_tmpA9[0]=_tmp6CE;});_tmpA9;});e->annot=_tmp6CF;});else{
# 584
({void*_tmp6D2=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpAB=_cycalloc(sizeof(*_tmpAB));({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp6D1=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmpAC;_tmpAC.tag=Cyc_CfFlowInfo_NotZero;({struct Cyc_List_List*_tmp6D0=Cyc_Relations_copy_relns(Cyc_Core_heap_region,relns);_tmpAC.f1=_tmp6D0;});_tmpAC;});_tmpAB[0]=_tmp6D1;});_tmpAB;});e->annot=_tmp6D2;});}
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
 return({struct _tuple16 _tmpB3;_tmpB3.f1=f;({void*_tmp6D3=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpDE,0,(env->fenv)->unknown_all);_tmpB3.f2=_tmp6D3;});_tmpB3;});}else{_LL93: _tmpD9=((_tmpB2.ReachableFL).val).f1;_tmpD8=((_tmpB2.ReachableFL).val).f2;_LL94:
# 619
 if(Cyc_Tcutil_is_bound_one(_tmpDD)){
struct _tuple15 _tmpB4=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);struct _tuple15 _tmpB5=_tmpB4;void*_tmpC7;struct Cyc_List_List*_tmpC6;_LL96: _tmpC7=_tmpB5.f1;_tmpC6=_tmpB5.f2;_LL97:;{
void*_tmpB6=_tmpC7;enum Cyc_CfFlowInfo_InitLevel _tmpC5;void*_tmpC4;struct Cyc_CfFlowInfo_Place*_tmpC3;struct Cyc_Absyn_Vardecl*_tmpC2;void*_tmpC1;switch(*((int*)_tmpB6)){case 8: _LL99: _tmpC2=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpB6)->f1;_tmpC1=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpB6)->f2;_LL9A:
# 623
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpB7=_cycalloc(sizeof(*_tmpB7));({struct Cyc_Core_Impossible_exn_struct _tmp6D5=({struct Cyc_Core_Impossible_exn_struct _tmpB8;_tmpB8.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp6D4=({const char*_tmpB9="named location in anal_derefR";_tag_dyneither(_tmpB9,sizeof(char),30);});_tmpB8.f1=_tmp6D4;});_tmpB8;});_tmpB7[0]=_tmp6D5;});_tmpB7;}));case 1: _LL9B: _LL9C:
# 625
 Cyc_NewControlFlow_update_relns(e,0,_tmpD8);
goto _LL98;case 4: _LL9D: _tmpC3=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpB6)->f1;_LL9E:
# 628
 Cyc_NewControlFlow_update_relns(e,0,_tmpD8);
return({struct _tuple16 _tmpBA;_tmpBA.f1=f;({void*_tmp6D6=Cyc_CfFlowInfo_lookup_place(_tmpD9,_tmpC3);_tmpBA.f2=_tmp6D6;});_tmpBA;});case 5: _LL9F: _tmpC4=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmpB6)->f1;_LLA0:
# 631
 Cyc_NewControlFlow_update_relns(e,1,_tmpD8);
return({struct _tuple16 _tmpBB;_tmpBB.f1=f;_tmpBB.f2=_tmpC4;_tmpBB;});case 0: _LLA1: _LLA2:
# 634
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple16 _tmpBC;({union Cyc_CfFlowInfo_FlowInfo _tmp6D7=Cyc_CfFlowInfo_BottomFL();_tmpBC.f1=_tmp6D7;});({void*_tmp6D8=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpDE,0,(env->fenv)->unknown_all);_tmpBC.f2=_tmp6D8;});_tmpBC;});case 2: _LLA3: _tmpC5=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpB6)->f1;_LLA4:
# 638
({union Cyc_CfFlowInfo_FlowInfo _tmp6D9=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpC5,_tmpC6);f=_tmp6D9;});
goto _LLA6;default: _LLA5: _LLA6:
# 641
{void*_tmpBD=e->r;void*_tmpBE=_tmpBD;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBE)->tag == 23){_LLA8: _LLA9:
# 643
({void*_tmp6DC=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpBF=_cycalloc(sizeof(*_tmpBF));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6DB=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmpC0;_tmpC0.tag=Cyc_CfFlowInfo_UnknownZ;({struct Cyc_List_List*_tmp6DA=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD8);_tmpC0.f1=_tmp6DA;});_tmpC0;});_tmpBF[0]=_tmp6DB;});_tmpBF;});e->annot=_tmp6DC;});
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
({void*_tmp6DF=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpCA=_cycalloc(sizeof(*_tmpCA));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp6DE=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmpCB;_tmpCB.tag=Cyc_CfFlowInfo_UnknownZ;({struct Cyc_List_List*_tmp6DD=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD8);_tmpCB.f1=_tmp6DD;});_tmpCB;});_tmpCA[0]=_tmp6DE;});_tmpCA;});e->annot=_tmp6DF;});
goto _LLAC;}_LLAC:;}{
# 661
enum Cyc_CfFlowInfo_InitLevel _tmpCD=Cyc_CfFlowInfo_initlevel(env->fenv,_tmpD9,r);enum Cyc_CfFlowInfo_InitLevel _tmpCE=_tmpCD;if(_tmpCE == Cyc_CfFlowInfo_NoneIL){_LLB2: _LLB3: {
# 663
struct _tuple15 _tmpCF=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);struct _tuple15 _tmpD0=_tmpCF;void*_tmpD6;_LLB7: _tmpD6=_tmpD0.f1;_LLB8:;
{void*_tmpD1=_tmpD6;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpD1)->tag == 7){_LLBA: _LLBB:
# 666
({void*_tmpD2=0;({unsigned int _tmp6E1=e->loc;struct _dyneither_ptr _tmp6E0=({const char*_tmpD3="attempt to dereference a consumed alias-free pointer";_tag_dyneither(_tmpD3,sizeof(char),53);});Cyc_CfFlowInfo_aerr(_tmp6E1,_tmp6E0,_tag_dyneither(_tmpD2,sizeof(void*),0));});});
goto _LLB9;}else{_LLBC: _LLBD:
# 669
({void*_tmpD4=0;({unsigned int _tmp6E3=e->loc;struct _dyneither_ptr _tmp6E2=({const char*_tmpD5="dereference of possibly uninitialized pointer";_tag_dyneither(_tmpD5,sizeof(char),46);});Cyc_CfFlowInfo_aerr(_tmp6E3,_tmp6E2,_tag_dyneither(_tmpD4,sizeof(void*),0));});});}_LLB9:;}
# 671
goto _LLB5;}}else{_LLB4: _LLB5:
# 673
 return({struct _tuple16 _tmpD7;_tmpD7.f1=f;({void*_tmp6E4=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpDE,0,(env->fenv)->unknown_all);_tmpD7.f2=_tmp6E4;});_tmpD7;});}_LLB1:;};}_LL90:;}}else{_LL8E: _LL8F:
# 676
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpDA=_cycalloc(sizeof(*_tmpDA));({struct Cyc_Core_Impossible_exn_struct _tmp6E6=({struct Cyc_Core_Impossible_exn_struct _tmpDB;_tmpDB.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp6E5=({const char*_tmpDC="right deref of non-pointer-type";_tag_dyneither(_tmpDC,sizeof(char),32);});_tmpDB.f1=_tmp6E5;});_tmpDB;});_tmpDA[0]=_tmp6E6;});_tmpDA;}));}_LL8B:;}
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
({struct Cyc_List_List*_tmp6E7=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,n1,relns);relns=_tmp6E7;});}
# 697
goto _LLBE;}}else{_LLC1: _LLC2:
 goto _LLBE;}_LLBE:;}{
# 701
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound((void*)_check_null(e1->topt));
if(bound != 0){
union Cyc_Relations_RelnOp rbound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rbound))
({struct Cyc_List_List*_tmp6E8=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,rbound,relns);relns=_tmp6E8;});}
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
({void*_tmp6EA=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpEA=_region_malloc((env->fenv)->r,sizeof(*_tmpEA));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp6E9=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmpEB;_tmpEB.tag=8;_tmpEB.f1=(struct Cyc_Absyn_Vardecl*)_tmpEC->hd;_tmpEB.f2=_tmpE7;_tmpEB;});_tmpEA[0]=_tmp6E9;});_tmpEA;});_tmpE7=_tmp6EA;});}
# 730
({struct Cyc_Dict_Dict _tmp6EB=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmpF2,_tmpF0,_tmpE7);_tmpF2=_tmp6EB;});
({union Cyc_CfFlowInfo_FlowInfo _tmp6EC=Cyc_CfFlowInfo_ReachableFL(_tmpF2,_tmpF1);inflow=_tmp6EC;});
Cyc_NewControlFlow_update_tryflow(env,inflow);
# 736
goto _LLC6;}}else{_LLCB: _LLCC:
# 739
({void*_tmpEE=0;({struct _dyneither_ptr _tmp6ED=({const char*_tmpEF="noconsume() parameters must be l-values";_tag_dyneither(_tmpEF,sizeof(char),40);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp6ED,_tag_dyneither(_tmpEE,sizeof(void*),0));});});
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
 return({struct _tuple16 _tmpF6;({union Cyc_CfFlowInfo_FlowInfo _tmp6EE=Cyc_CfFlowInfo_BottomFL();_tmpF6.f1=_tmp6EE;});_tmpF6.f2=rval;_tmpF6;});}else{_LLD3: _tmp103=((_tmpF5.ReachableFL).val).f1;_tmp102=((_tmpF5.ReachableFL).val).f2;_LLD4: {
# 759
union Cyc_CfFlowInfo_AbsLVal _tmpF7=lval;struct Cyc_CfFlowInfo_Place*_tmp101;if((_tmpF7.PlaceL).tag == 1){_LLD6: _tmp101=(_tmpF7.PlaceL).val;_LLD7: {
# 767
struct Cyc_Dict_Dict _tmpF8=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp103,_tmp101,rval);
({struct Cyc_List_List*_tmp6EF=Cyc_Relations_reln_assign_exp(fenv->r,_tmp102,lexp,rexp);_tmp102=_tmp6EF;});
({union Cyc_CfFlowInfo_FlowInfo _tmp6F0=Cyc_CfFlowInfo_ReachableFL(_tmpF8,_tmp102);outflow=_tmp6F0;});
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(lexp->topt))){
# 773
struct _tuple15 _tmpF9=({struct _RegionHandle*_tmp6F1=fenv->r;Cyc_CfFlowInfo_unname_rval(_tmp6F1,Cyc_CfFlowInfo_lookup_place(_tmp103,_tmp101));});struct _tuple15 _tmpFA=_tmpF9;void*_tmpFE;_LLDB: _tmpFE=_tmpFA.f1;_LLDC:;{
void*_tmpFB=_tmpFE;switch(*((int*)_tmpFB)){case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpFB)->f1 == Cyc_CfFlowInfo_NoneIL){_LLDE: _LLDF:
 goto _LLE1;}else{goto _LLE4;}case 7: _LLE0: _LLE1:
 goto _LLE3;case 0: _LLE2: _LLE3:
 goto _LLDD;default: _LLE4: _LLE5:
# 779
({void*_tmpFC=0;({unsigned int _tmp6F3=lexp->loc;struct _dyneither_ptr _tmp6F2=({const char*_tmpFD="assignment may overwrite alias-free pointer(s)";_tag_dyneither(_tmpFD,sizeof(char),47);});Cyc_Tcutil_warn(_tmp6F3,_tmp6F2,_tag_dyneither(_tmpFC,sizeof(void*),0));});});
goto _LLDD;}_LLDD:;};}
# 784
Cyc_NewControlFlow_update_tryflow(env,outflow);
return({struct _tuple16 _tmpFF;_tmpFF.f1=outflow;_tmpFF.f2=rval;_tmpFF;});}}else{_LLD8: _LLD9:
# 790
 return({struct _tuple16 _tmp100;({union Cyc_CfFlowInfo_FlowInfo _tmp6F4=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval);_tmp100.f1=_tmp6F4;});_tmp100.f2=rval;_tmp100;});}_LLD5:;}}_LLD0:;}
# 797
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 803
union Cyc_CfFlowInfo_FlowInfo _tmp104=f;struct Cyc_Dict_Dict _tmp10A;struct Cyc_List_List*_tmp109;if((_tmp104.BottomFL).tag == 1){_LLE7: _LLE8:
 return Cyc_CfFlowInfo_BottomFL();}else{_LLE9: _tmp10A=((_tmp104.ReachableFL).val).f1;_tmp109=((_tmp104.ReachableFL).val).f2;_LLEA:
# 808
({struct Cyc_Dict_Dict _tmp6FB=({struct Cyc_CfFlowInfo_FlowEnv*_tmp6FA=fenv;unsigned int _tmp6F9=loc;struct Cyc_Dict_Dict _tmp6F8=_tmp10A;struct Cyc_CfFlowInfo_Place*_tmp6F7=({struct Cyc_CfFlowInfo_Place*_tmp105=_region_malloc(env->r,sizeof(*_tmp105));({void*_tmp6F6=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp106=_region_malloc(env->r,sizeof(*_tmp106));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp6F5=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp107;_tmp107.tag=0;_tmp107.f1=vd;_tmp107;});_tmp106[0]=_tmp6F5;});_tmp106;});_tmp105->root=_tmp6F6;});_tmp105->path=0;_tmp105;});Cyc_CfFlowInfo_assign_place(_tmp6FA,_tmp6F9,_tmp6F8,_tmp6F7,rval);});_tmp10A=_tmp6FB;});
# 811
({struct Cyc_List_List*_tmp6FC=Cyc_Relations_reln_assign_var(env->r,_tmp109,vd,rexp);_tmp109=_tmp6FC;});{
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
if((*((struct _tuple24*)x->hd)).f1 == 0)({struct Cyc_List_List*_tmp6FD=({struct Cyc_List_List*_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple24*)x->hd)).f2);_tmp10C->tl=es;_tmp10C;});es=_tmp6FD;});}}
# 836
({union Cyc_CfFlowInfo_FlowInfo _tmp6FE=Cyc_NewControlFlow_add_vars(fenv,inflow,_tmp10B,fenv->unknown_all,pat_loc,name_locs);inflow=_tmp6FE;});
# 838
({union Cyc_CfFlowInfo_FlowInfo _tmp6FF=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);inflow=_tmp6FF;});
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 842
struct _tuple16 _tmp10D=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple16 _tmp10E=_tmp10D;union Cyc_CfFlowInfo_FlowInfo _tmp110;void*_tmp10F;_LLEC: _tmp110=_tmp10E.f1;_tmp10F=_tmp10E.f2;_LLED:;
({union Cyc_CfFlowInfo_FlowInfo _tmp700=Cyc_NewControlFlow_use_Rval(env,exp_loc,_tmp110,_tmp10F);inflow=_tmp700;});}}{
# 850
struct Cyc_List_List*_tmp111=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);
for(0;_tmp111 != 0;_tmp111=_tmp111->tl){
struct _tuple24*_tmp112=(struct _tuple24*)_tmp111->hd;struct _tuple24*_tmp113=_tmp112;struct Cyc_Absyn_Vardecl**_tmp12E;struct Cyc_Absyn_Exp*_tmp12D;_LLEF: _tmp12E=_tmp113->f1;_tmp12D=_tmp113->f2;_LLF0:;
if(_tmp12E != 0  && _tmp12D != 0){
if(_tmp12D->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp116;_tmp116.tag=0;({struct _dyneither_ptr _tmp701=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp12D));_tmp116.f1=_tmp701;});({void*_tmp114[1]={& _tmp116};({struct _dyneither_ptr _tmp702=({const char*_tmp115="oops! pattern init expr %s has no type!\n";_tag_dyneither(_tmp115,sizeof(char),41);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp702,_tag_dyneither(_tmp114,sizeof(void*),1));});});});{
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
({void*_tmp704=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp11E=_region_malloc(fenv->r,sizeof(*_tmp11E));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp703=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp11F;_tmp11F.tag=8;_tmp11F.f1=*_tmp12E;_tmp11F.f2=_tmp129;_tmp11F;});_tmp11E[0]=_tmp703;});_tmp11E;});_tmp129=_tmp704;});{
# 876
void*_tmp120=Cyc_CfFlowInfo_lookup_place(_tmp128,_tmp126);
({void*_tmp706=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp121=_region_malloc(fenv->r,sizeof(*_tmp121));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp705=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp122;_tmp122.tag=8;_tmp122.f1=*_tmp12E;_tmp122.f2=_tmp120;_tmp122;});_tmp121[0]=_tmp705;});_tmp121;});_tmp120=_tmp706;});
({struct Cyc_Dict_Dict _tmp707=Cyc_CfFlowInfo_assign_place(fenv,exp_loc,_tmp128,_tmp126,_tmp120);_tmp128=_tmp707;});
({union Cyc_CfFlowInfo_FlowInfo _tmp708=Cyc_CfFlowInfo_ReachableFL(_tmp128,_tmp127);_tmp12A=_tmp708;});
goto _LLFC;};}else{_LLFF: _LL100:
# 883
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(_tmp12D->topt)) && !
Cyc_Tcutil_is_noalias_pointer_or_aggr((*_tmp12E)->type))
({struct Cyc_String_pa_PrintArg_struct _tmp125;_tmp125.tag=0;({struct _dyneither_ptr _tmp709=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp12D));_tmp125.f1=_tmp709;});({void*_tmp123[1]={& _tmp125};({unsigned int _tmp70B=exp_loc;struct _dyneither_ptr _tmp70A=({const char*_tmp124="aliased pattern expression not an l-value: %s";_tag_dyneither(_tmp124,sizeof(char),46);});Cyc_CfFlowInfo_aerr(_tmp70B,_tmp70A,_tag_dyneither(_tmp123,sizeof(void*),1));});});});}_LLFC:;}
# 893
({union Cyc_CfFlowInfo_FlowInfo _tmp70C=Cyc_NewControlFlow_do_initialize_var(fenv,env,_tmp12A,*_tmp12E,_tmp12D,_tmp129,pat_loc);inflow=_tmp70C;});
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
return({struct Cyc_Absyn_Exp*_tmp70D=Cyc_Absyn_prim1_exp(Cyc_Absyn_Numelts,_tmp13B,0);Cyc_Relations_exp2relnop(_tmp70D,rop);});}default: _LL11E: _LL11F:
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
({struct Cyc_String_pa_PrintArg_struct _tmp149;_tmp149.tag=0;({struct _dyneither_ptr _tmp70E=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Relations_relns2string(_tmp14A));_tmp149.f1=_tmp70E;});({struct Cyc_String_pa_PrintArg_struct _tmp148;_tmp148.tag=0;_tmp148.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);({struct Cyc_String_pa_PrintArg_struct _tmp147;_tmp147.tag=0;_tmp147.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({struct Cyc_String_pa_PrintArg_struct _tmp146;_tmp146.tag=0;_tmp146.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({void*_tmp144[4]={& _tmp146,& _tmp147,& _tmp148,& _tmp149};({unsigned int _tmp710=loc;struct _dyneither_ptr _tmp70F=({const char*_tmp145="cannot prove that @requires clause %s %s %s is satisfied\n (all I know is %s)";_tag_dyneither(_tmp145,sizeof(char),77);});Cyc_Tcutil_terr(_tmp710,_tmp70F,_tag_dyneither(_tmp144,sizeof(void*),4));});});});});});});
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
 return({struct _tuple16 _tmp14E;({union Cyc_CfFlowInfo_FlowInfo _tmp711=Cyc_CfFlowInfo_BottomFL();_tmp14E.f1=_tmp711;});_tmp14E.f2=_tmp14C->unknown_all;_tmp14E;});}else{_LL12F: _tmp150=((_tmp14D.ReachableFL).val).f1;_tmp14F=((_tmp14D.ReachableFL).val).f2;_LL130:
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
({void*_tmp156=0;({unsigned int _tmp713=e->loc;struct _dyneither_ptr _tmp712=({const char*_tmp157="expression attempts to copy a consumed alias-free value";_tag_dyneither(_tmp157,sizeof(char),56);});Cyc_CfFlowInfo_aerr(_tmp713,_tmp712,_tag_dyneither(_tmp156,sizeof(void*),0));});});
return({struct _tuple16 _tmp158;({union Cyc_CfFlowInfo_FlowInfo _tmp714=Cyc_CfFlowInfo_BottomFL();_tmp158.f1=_tmp714;});_tmp158.f2=_tmp14C->unknown_all;_tmp158;});}else{
# 1036
if(needs_unconsume)
# 1038
return({struct _tuple16 _tmp159;_tmp159.f1=_tmp162;({void*_tmp715=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp14C,_tmp155);_tmp159.f2=_tmp715;});_tmp159;});else{
# 1041
void*_tmp15A=Cyc_CfFlowInfo_make_unique_consumed(_tmp14C,(void*)_check_null(e->topt),e,env->iteration_num,_tmp155);
struct Cyc_Dict_Dict _tmp15B=Cyc_CfFlowInfo_assign_place(_tmp14C,e->loc,_tmp15F,_tmp15D,_tmp15A);
# 1053
return({struct _tuple16 _tmp15C;({union Cyc_CfFlowInfo_FlowInfo _tmp716=Cyc_CfFlowInfo_ReachableFL(_tmp15B,_tmp15E);_tmp15C.f1=_tmp716;});_tmp15C.f2=_tmp155;_tmp15C;});}}}}else{goto _LL137;}}else{_LL137: _LL138:
# 1057
 goto _LL134;}_LL134:;};}{
# 1060
void*_tmp163=e->r;void*_tmp164=_tmp163;struct Cyc_Absyn_Stmt*_tmp387;struct Cyc_Absyn_Exp*_tmp386;void*_tmp385;int _tmp384;struct Cyc_Absyn_Vardecl*_tmp383;struct Cyc_Absyn_Exp*_tmp382;struct Cyc_Absyn_Exp*_tmp381;int _tmp380;struct Cyc_List_List*_tmp37F;struct Cyc_List_List*_tmp37E;enum Cyc_Absyn_AggrKind _tmp37D;struct Cyc_List_List*_tmp37C;struct Cyc_List_List*_tmp37B;struct Cyc_List_List*_tmp37A;struct Cyc_Absyn_Exp*_tmp379;struct Cyc_Absyn_Exp*_tmp378;struct Cyc_Absyn_Exp*_tmp377;struct Cyc_Absyn_Exp*_tmp376;struct Cyc_Absyn_Exp*_tmp375;struct Cyc_Absyn_Exp*_tmp374;struct Cyc_Absyn_Exp*_tmp373;struct Cyc_Absyn_Exp*_tmp372;struct Cyc_Absyn_Exp*_tmp371;struct Cyc_Absyn_Exp*_tmp370;struct _dyneither_ptr*_tmp36F;struct Cyc_Absyn_Exp*_tmp36E;struct _dyneither_ptr*_tmp36D;struct Cyc_Absyn_Exp*_tmp36C;struct _dyneither_ptr*_tmp36B;struct Cyc_Absyn_Exp*_tmp36A;struct Cyc_Absyn_Exp*_tmp369;struct Cyc_Absyn_Exp*_tmp368;struct Cyc_Absyn_Exp*_tmp367;struct Cyc_Absyn_Exp*_tmp366;struct Cyc_Absyn_Exp*_tmp365;int _tmp364;struct Cyc_Absyn_Exp*_tmp363;void**_tmp362;struct Cyc_Absyn_Exp*_tmp361;int _tmp360;struct Cyc_Absyn_Exp*_tmp35F;struct Cyc_List_List*_tmp35E;struct Cyc_Absyn_Exp*_tmp35D;struct Cyc_Absyn_Exp*_tmp35C;struct Cyc_Absyn_Exp*_tmp35B;struct Cyc_Absyn_Exp*_tmp35A;struct Cyc_Absyn_Exp*_tmp359;struct Cyc_Absyn_Exp*_tmp358;struct Cyc_Absyn_Exp*_tmp357;struct Cyc_Absyn_Exp*_tmp356;enum Cyc_Absyn_Primop _tmp355;struct Cyc_List_List*_tmp354;struct Cyc_List_List*_tmp353;struct Cyc_Absyn_Datatypedecl*_tmp352;struct Cyc_Absyn_Vardecl*_tmp351;struct Cyc_Absyn_Vardecl*_tmp350;struct Cyc_Absyn_Vardecl*_tmp34F;struct _dyneither_ptr _tmp34E;struct Cyc_Absyn_Exp*_tmp34D;struct Cyc_Absyn_Exp*_tmp34C;struct Cyc_Absyn_Exp*_tmp34B;struct Cyc_Absyn_Exp*_tmp34A;switch(*((int*)_tmp164)){case 14: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp164)->f4 == Cyc_Absyn_Null_to_NonNull){_LL13A: _tmp34A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL13B: {
# 1064
struct _tuple16 _tmp165=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp34A);struct _tuple16 _tmp166=_tmp165;union Cyc_CfFlowInfo_FlowInfo _tmp16D;void*_tmp16C;_LL1A5: _tmp16D=_tmp166.f1;_tmp16C=_tmp166.f2;_LL1A6:;{
struct _tuple16 _tmp167=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp16D,_tmp34A,_tmp16C);struct _tuple16 _tmp168=_tmp167;union Cyc_CfFlowInfo_FlowInfo _tmp16B;void*_tmp16A;_LL1A8: _tmp16B=_tmp168.f1;_tmp16A=_tmp168.f2;_LL1A9:;
return({struct _tuple16 _tmp169;_tmp169.f1=_tmp16B;_tmp169.f2=_tmp16C;_tmp169;});};}}else{_LL13C: _tmp34B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL13D:
# 1070
 _tmp34C=_tmp34B;goto _LL13F;}case 12: _LL13E: _tmp34C=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL13F:
 _tmp34D=_tmp34C;goto _LL141;case 13: _LL140: _tmp34D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL141:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp34D);case 2: _LL142: _tmp34E=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL143:
# 1075
 if(!({struct _dyneither_ptr _tmp717=(struct _dyneither_ptr)_tmp34E;Cyc_NewControlFlow_strcmp(_tmp717,({const char*_tmp16E="print_flow";_tag_dyneither(_tmp16E,sizeof(char),11);}));})){
struct _dyneither_ptr _tmp16F=Cyc_Position_string_of_segment(e->loc);
({struct Cyc_String_pa_PrintArg_struct _tmp172;_tmp172.tag=0;_tmp172.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp16F);({void*_tmp170[1]={& _tmp172};({struct Cyc___cycFILE*_tmp719=Cyc_stderr;struct _dyneither_ptr _tmp718=({const char*_tmp171="%s: current flow is\n";_tag_dyneither(_tmp171,sizeof(char),21);});Cyc_fprintf(_tmp719,_tmp718,_tag_dyneither(_tmp170,sizeof(void*),1));});});});
Cyc_CfFlowInfo_print_flow(inflow);
({void*_tmp173=0;({struct Cyc___cycFILE*_tmp71B=Cyc_stderr;struct _dyneither_ptr _tmp71A=({const char*_tmp174="\n";_tag_dyneither(_tmp174,sizeof(char),2);});Cyc_fprintf(_tmp71B,_tmp71A,_tag_dyneither(_tmp173,sizeof(void*),0));});});}else{
# 1081
if(!({struct _dyneither_ptr _tmp71C=(struct _dyneither_ptr)_tmp34E;Cyc_NewControlFlow_strcmp(_tmp71C,({const char*_tmp175="debug_on";_tag_dyneither(_tmp175,sizeof(char),9);}));}));else{
# 1086
if(!({struct _dyneither_ptr _tmp71D=(struct _dyneither_ptr)_tmp34E;Cyc_NewControlFlow_strcmp(_tmp71D,({const char*_tmp176="debug_off";_tag_dyneither(_tmp176,sizeof(char),10);}));}));else{
# 1092
({struct Cyc_String_pa_PrintArg_struct _tmp179;_tmp179.tag=0;_tmp179.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34E);({void*_tmp177[1]={& _tmp179};({unsigned int _tmp71F=e->loc;struct _dyneither_ptr _tmp71E=({const char*_tmp178="unknown pragma %s";_tag_dyneither(_tmp178,sizeof(char),18);});Cyc_Tcutil_warn(_tmp71F,_tmp71E,_tag_dyneither(_tmp177,sizeof(void*),1));});});});}}}
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
 return({struct _tuple16 _tmp17E;_tmp17E.f1=inflow;({void*_tmp720=Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,(void*)_check_null(e->topt),0,_tmp14C->unknown_all);_tmp17E.f2=_tmp720;});_tmp17E;});case 3: _LL15C: _tmp34F=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp164)->f1)->f1;_LL15D:
# 1112
 _tmp350=_tmp34F;goto _LL15F;case 4: _LL15E: _tmp350=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp164)->f1)->f1;_LL15F:
 _tmp351=_tmp350;goto _LL161;case 5: _LL160: _tmp351=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp164)->f1)->f1;_LL161: {
# 1116
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp180;_tmp180.tag=0;_tmp180.f1=_tmp351;_tmp180;});
return({struct _tuple16 _tmp17F;_tmp17F.f1=inflow;({void*_tmp721=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot);_tmp17F.f2=_tmp721;});_tmp17F;});}default: _LL19A: _LL19B:
# 1718
 goto _LL19D;}case 31: if(((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp164)->f1 == 0){_LL14C: _LL14D:
# 1100
 goto _LL14F;}else{_LL188: _tmp353=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp352=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL189:
# 1576
 _tmp37A=_tmp353;goto _LL18B;}case 18: _LL150: _LL151:
# 1102
 goto _LL153;case 17: _LL152: _LL153:
 goto _LL155;case 19: _LL154: _LL155:
 goto _LL157;case 33: _LL156: _LL157:
 goto _LL159;case 32: _LL158: _LL159:
 return({struct _tuple16 _tmp17D;_tmp17D.f1=inflow;_tmp17D.f2=_tmp14C->unknown_all;_tmp17D;});case 3: _LL162: _tmp355=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp354=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL163: {
# 1123
struct _tuple16 _tmp181=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp354);struct _tuple16 _tmp182=_tmp181;union Cyc_CfFlowInfo_FlowInfo _tmp185;void*_tmp184;_LL1AB: _tmp185=_tmp182.f1;_tmp184=_tmp182.f2;_LL1AC:;
return({struct _tuple16 _tmp183;_tmp183.f1=_tmp185;_tmp183.f2=_tmp184;_tmp183;});}case 5: _LL164: _tmp356=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL165: {
# 1127
struct Cyc_List_List _tmp186=({struct Cyc_List_List _tmp194;_tmp194.hd=_tmp356;_tmp194.tl=0;_tmp194;});
struct _tuple16 _tmp187=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp186);struct _tuple16 _tmp188=_tmp187;union Cyc_CfFlowInfo_FlowInfo _tmp193;_LL1AE: _tmp193=_tmp188.f1;_LL1AF:;{
struct _tuple18 _tmp189=Cyc_NewControlFlow_anal_Lexp(env,_tmp193,0,0,_tmp356);struct _tuple18 _tmp18A=_tmp189;union Cyc_CfFlowInfo_AbsLVal _tmp192;_LL1B1: _tmp192=_tmp18A.f2;_LL1B2:;
{struct _tuple25 _tmp18B=({struct _tuple25 _tmp190;_tmp190.f1=_tmp192;_tmp190.f2=_tmp193;_tmp190;});struct _tuple25 _tmp18C=_tmp18B;struct Cyc_CfFlowInfo_Place*_tmp18F;struct Cyc_Dict_Dict _tmp18E;struct Cyc_List_List*_tmp18D;if(((_tmp18C.f1).PlaceL).tag == 1){if(((_tmp18C.f2).ReachableFL).tag == 2){_LL1B4: _tmp18F=((_tmp18C.f1).PlaceL).val;_tmp18E=(((_tmp18C.f2).ReachableFL).val).f1;_tmp18D=(((_tmp18C.f2).ReachableFL).val).f2;_LL1B5:
# 1132
({struct Cyc_List_List*_tmp722=Cyc_Relations_reln_kill_exp(_tmp14C->r,_tmp18D,_tmp356);_tmp18D=_tmp722;});
({union Cyc_CfFlowInfo_FlowInfo _tmp724=({struct Cyc_Dict_Dict _tmp723=Cyc_CfFlowInfo_assign_place(_tmp14C,_tmp356->loc,_tmp18E,_tmp18F,_tmp14C->unknown_all);Cyc_CfFlowInfo_ReachableFL(_tmp723,_tmp18D);});_tmp193=_tmp724;});
# 1137
goto _LL1B3;}else{goto _LL1B6;}}else{_LL1B6: _LL1B7:
 goto _LL1B3;}_LL1B3:;}
# 1140
return({struct _tuple16 _tmp191;_tmp191.f1=_tmp193;_tmp191.f2=_tmp14C->unknown_all;_tmp191;});};}case 4: if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp164)->f2 != 0){_LL166: _tmp358=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp357=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp164)->f3;_LL167:
# 1143
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp195=0;({unsigned int _tmp726=e->loc;struct _dyneither_ptr _tmp725=({const char*_tmp196="cannot track alias-free pointers through multiple assignments";_tag_dyneither(_tmp196,sizeof(char),62);});Cyc_CfFlowInfo_aerr(_tmp726,_tmp725,_tag_dyneither(_tmp195,sizeof(void*),0));});});
return({struct _tuple16 _tmp197;({union Cyc_CfFlowInfo_FlowInfo _tmp727=Cyc_CfFlowInfo_BottomFL();_tmp197.f1=_tmp727;});_tmp197.f2=_tmp14C->unknown_all;_tmp197;});}{
# 1147
struct _tuple16 _tmp198=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp357);struct _tuple16 _tmp199=_tmp198;union Cyc_CfFlowInfo_FlowInfo _tmp1A5;void*_tmp1A4;_LL1B9: _tmp1A5=_tmp199.f1;_tmp1A4=_tmp199.f2;_LL1BA:;{
struct _tuple18 _tmp19A=Cyc_NewControlFlow_anal_Lexp(env,_tmp1A5,0,0,_tmp358);struct _tuple18 _tmp19B=_tmp19A;union Cyc_CfFlowInfo_FlowInfo _tmp1A3;union Cyc_CfFlowInfo_AbsLVal _tmp1A2;_LL1BC: _tmp1A3=_tmp19B.f1;_tmp1A2=_tmp19B.f2;_LL1BD:;
{union Cyc_CfFlowInfo_FlowInfo _tmp19C=_tmp1A3;struct Cyc_Dict_Dict _tmp1A0;struct Cyc_List_List*_tmp19F;if((_tmp19C.ReachableFL).tag == 2){_LL1BF: _tmp1A0=((_tmp19C.ReachableFL).val).f1;_tmp19F=((_tmp19C.ReachableFL).val).f2;_LL1C0:
# 1151
{union Cyc_CfFlowInfo_AbsLVal _tmp19D=_tmp1A2;struct Cyc_CfFlowInfo_Place*_tmp19E;if((_tmp19D.PlaceL).tag == 1){_LL1C4: _tmp19E=(_tmp19D.PlaceL).val;_LL1C5:
# 1155
({struct Cyc_List_List*_tmp728=Cyc_Relations_reln_kill_exp(_tmp14C->r,_tmp19F,_tmp358);_tmp19F=_tmp728;});
({struct Cyc_Dict_Dict _tmp729=Cyc_CfFlowInfo_assign_place(_tmp14C,_tmp358->loc,_tmp1A0,_tmp19E,_tmp14C->unknown_all);_tmp1A0=_tmp729;});
# 1158
({union Cyc_CfFlowInfo_FlowInfo _tmp72A=Cyc_CfFlowInfo_ReachableFL(_tmp1A0,_tmp19F);_tmp1A3=_tmp72A;});
# 1162
goto _LL1C3;}else{_LL1C6: _LL1C7:
# 1165
 goto _LL1C3;}_LL1C3:;}
# 1167
goto _LL1BE;}else{_LL1C1: _LL1C2:
 goto _LL1BE;}_LL1BE:;}
# 1170
return({struct _tuple16 _tmp1A1;_tmp1A1.f1=_tmp1A3;_tmp1A1.f2=_tmp14C->unknown_all;_tmp1A1;});};};}else{_LL168: _tmp35A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp359=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp164)->f3;_LL169:
# 1173
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp1A6=0;({unsigned int _tmp72C=e->loc;struct _dyneither_ptr _tmp72B=({const char*_tmp1A7="cannot track alias-free pointers through multiple assignments";_tag_dyneither(_tmp1A7,sizeof(char),62);});Cyc_CfFlowInfo_aerr(_tmp72C,_tmp72B,_tag_dyneither(_tmp1A6,sizeof(void*),0));});});
return({struct _tuple16 _tmp1A8;({union Cyc_CfFlowInfo_FlowInfo _tmp72D=Cyc_CfFlowInfo_BottomFL();_tmp1A8.f1=_tmp72D;});_tmp1A8.f2=_tmp14C->unknown_all;_tmp1A8;});}
# 1179
({union Cyc_CfFlowInfo_FlowInfo _tmp731=({struct Cyc_NewControlFlow_AnalEnv*_tmp730=env;union Cyc_CfFlowInfo_FlowInfo _tmp72F=inflow;Cyc_NewControlFlow_expand_unique_places(_tmp730,_tmp72F,({struct Cyc_Absyn_Exp*_tmp1A9[2];_tmp1A9[1]=_tmp359;_tmp1A9[0]=_tmp35A;({struct _RegionHandle*_tmp72E=env->r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp72E,_tag_dyneither(_tmp1A9,sizeof(struct Cyc_Absyn_Exp*),2));});}));});inflow=_tmp731;});{
# 1181
struct _tuple16 _tmp1AA=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp359);struct _tuple16 _tmp1AB=_tmp1AA;union Cyc_CfFlowInfo_FlowInfo _tmp1B6;void*_tmp1B5;_LL1C9: _tmp1B6=_tmp1AB.f1;_tmp1B5=_tmp1AB.f2;_LL1CA:;{
struct _tuple18 _tmp1AC=Cyc_NewControlFlow_anal_Lexp(env,_tmp1B6,0,0,_tmp35A);struct _tuple18 _tmp1AD=_tmp1AC;union Cyc_CfFlowInfo_FlowInfo _tmp1B4;union Cyc_CfFlowInfo_AbsLVal _tmp1B3;_LL1CC: _tmp1B4=_tmp1AD.f1;_tmp1B3=_tmp1AD.f2;_LL1CD:;{
struct _tuple16 _tmp1AE=Cyc_NewControlFlow_do_assign(_tmp14C,env,_tmp1B4,_tmp35A,_tmp1B3,_tmp359,_tmp1B5,e->loc);struct _tuple16 _tmp1AF=_tmp1AE;union Cyc_CfFlowInfo_FlowInfo _tmp1B2;void*_tmp1B1;_LL1CF: _tmp1B2=_tmp1AF.f1;_tmp1B1=_tmp1AF.f2;_LL1D0:;
# 1187
return({struct _tuple16 _tmp1B0;_tmp1B0.f1=_tmp1B2;_tmp1B0.f2=_tmp1B1;_tmp1B0;});};};};}case 9: _LL16A: _tmp35C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp35B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL16B: {
# 1190
struct _tuple16 _tmp1B7=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp35C);struct _tuple16 _tmp1B8=_tmp1B7;union Cyc_CfFlowInfo_FlowInfo _tmp1BA;void*_tmp1B9;_LL1D2: _tmp1BA=_tmp1B8.f1;_tmp1B9=_tmp1B8.f2;_LL1D3:;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp1BA,_tmp35B);}case 11: _LL16C: _tmp35D=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL16D: {
# 1194
struct _tuple16 _tmp1BB=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp35D);struct _tuple16 _tmp1BC=_tmp1BB;union Cyc_CfFlowInfo_FlowInfo _tmp1BF;void*_tmp1BE;_LL1D5: _tmp1BF=_tmp1BC.f1;_tmp1BE=_tmp1BC.f2;_LL1D6:;
Cyc_NewControlFlow_use_Rval(env,_tmp35D->loc,_tmp1BF,_tmp1BE);
return({struct _tuple16 _tmp1BD;({union Cyc_CfFlowInfo_FlowInfo _tmp732=Cyc_CfFlowInfo_BottomFL();_tmp1BD.f1=_tmp732;});({void*_tmp733=Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,(void*)_check_null(e->topt),0,_tmp14C->unknown_all);_tmp1BD.f2=_tmp733;});_tmp1BD;});}case 10: _LL16E: _tmp35F=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp35E=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL16F: {
# 1199
struct _RegionHandle _tmp1C0=_new_region("temp");struct _RegionHandle*temp=& _tmp1C0;_push_region(temp);
{struct Cyc_List_List*_tmp1C1=_tmp35E;
struct _tuple23 _tmp1C2=({struct _RegionHandle*_tmp737=temp;struct Cyc_NewControlFlow_AnalEnv*_tmp736=env;union Cyc_CfFlowInfo_FlowInfo _tmp735=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp737,_tmp736,_tmp735,({struct Cyc_List_List*_tmp1F6=_region_malloc(temp,sizeof(*_tmp1F6));_tmp1F6->hd=_tmp35F;({struct Cyc_List_List*_tmp734=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp35E);_tmp1F6->tl=_tmp734;});_tmp1F6;}),0,1);});struct _tuple23 _tmp1C3=_tmp1C2;union Cyc_CfFlowInfo_FlowInfo _tmp1F5;struct Cyc_List_List*_tmp1F4;_LL1D8: _tmp1F5=_tmp1C3.f1;_tmp1F4=_tmp1C3.f2;_LL1D9:;{
# 1203
union Cyc_CfFlowInfo_FlowInfo _tmp1C4=Cyc_NewControlFlow_use_Rval(env,_tmp35F->loc,_tmp1F5,(void*)((struct Cyc_List_List*)_check_null(_tmp1F4))->hd);
_tmp1F4=_tmp1F4->tl;{
# 1206
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
struct Cyc_List_List*requires;
struct Cyc_List_List*ensures;
{void*_tmp1C5=Cyc_Tcutil_compress((void*)_check_null(_tmp35F->topt));void*_tmp1C6=_tmp1C5;void*_tmp1D8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C6)->tag == 5){_LL1DB: _tmp1D8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C6)->f1).elt_typ;_LL1DC:
# 1213
{void*_tmp1C7=Cyc_Tcutil_compress(_tmp1D8);void*_tmp1C8=_tmp1C7;struct Cyc_List_List*_tmp1D5;struct Cyc_List_List*_tmp1D4;struct Cyc_List_List*_tmp1D3;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->tag == 9){_LL1E0: _tmp1D5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).attributes;_tmp1D4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).requires_relns;_tmp1D3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).ensures_relns;_LL1E1:
# 1215
 requires=_tmp1D4;
ensures=_tmp1D3;
for(0;_tmp1D5 != 0;_tmp1D5=_tmp1D5->tl){
# 1219
void*_tmp1C9=(void*)_tmp1D5->hd;void*_tmp1CA=_tmp1C9;int _tmp1D0;int _tmp1CF;int _tmp1CE;switch(*((int*)_tmp1CA)){case 20: _LL1E5: _tmp1CE=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1CA)->f1;_LL1E6:
# 1221
({struct Cyc_List_List*_tmp738=({struct Cyc_List_List*_tmp1CB=_region_malloc(temp,sizeof(*_tmp1CB));_tmp1CB->hd=(void*)_tmp1CE;_tmp1CB->tl=init_params;_tmp1CB;});init_params=_tmp738;});goto _LL1E4;case 21: _LL1E7: _tmp1CF=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp1CA)->f1;_LL1E8:
# 1223
({struct Cyc_List_List*_tmp739=({struct Cyc_List_List*_tmp1CC=_region_malloc(temp,sizeof(*_tmp1CC));_tmp1CC->hd=(void*)_tmp1CF;_tmp1CC->tl=nolive_unique_params;_tmp1CC;});nolive_unique_params=_tmp739;});
goto _LL1E4;case 22: _LL1E9: _tmp1D0=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp1CA)->f1;_LL1EA:
# 1227
({struct Cyc_List_List*_tmp73A=({struct Cyc_List_List*_tmp1CD=_region_malloc(temp,sizeof(*_tmp1CD));_tmp1CD->hd=(void*)_tmp1D0;_tmp1CD->tl=noconsume_params;_tmp1CD;});noconsume_params=_tmp73A;});
goto _LL1E4;default: _LL1EB: _LL1EC:
 goto _LL1E4;}_LL1E4:;}
# 1231
goto _LL1DF;}else{_LL1E2: _LL1E3:
({void*_tmp1D1=0;({struct _dyneither_ptr _tmp73B=({const char*_tmp1D2="anal_Rexp: bad function type";_tag_dyneither(_tmp1D2,sizeof(char),29);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp73B,_tag_dyneither(_tmp1D1,sizeof(void*),0));});});}_LL1DF:;}
# 1234
goto _LL1DA;}else{_LL1DD: _LL1DE:
({void*_tmp1D6=0;({struct _dyneither_ptr _tmp73C=({const char*_tmp1D7="anal_Rexp: bad function type";_tag_dyneither(_tmp1D7,sizeof(char),29);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp73C,_tag_dyneither(_tmp1D6,sizeof(void*),0));});});}_LL1DA:;}
# 1237
{int i=1;for(0;_tmp1F4 != 0;(((_tmp1F4=_tmp1F4->tl,_tmp35E=((struct Cyc_List_List*)_check_null(_tmp35E))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp1D9=_tmp1F5;struct Cyc_Dict_Dict _tmp1EF;if((_tmp1D9.BottomFL).tag == 1){_LL1EE: _LL1EF:
 goto _LL1ED;}else{_LL1F0: _tmp1EF=((_tmp1D9.ReachableFL).val).f1;_LL1F1: {
# 1242
struct _tuple15 _tmp1DA=Cyc_CfFlowInfo_unname_rval(_tmp14C->r,(void*)_tmp1F4->hd);struct _tuple15 _tmp1DB=_tmp1DA;void*_tmp1EE;_LL1F3: _tmp1EE=_tmp1DB.f1;_LL1F4:;
{void*_tmp1DC=(void*)_tmp1F4->hd;void*_tmp1DD=_tmp1DC;switch(*((int*)_tmp1DD)){case 4: _LL1F6: _LL1F7:
 goto _LL1F9;case 5: _LL1F8: _LL1F9:
 goto _LL1F5;default: _LL1FA: _LL1FB:
# 1247
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp1EF,(void*)_tmp1F4->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp1DE=0;({unsigned int _tmp73E=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp35E))->hd)->loc;struct _dyneither_ptr _tmp73D=({const char*_tmp1DF="expression may not be initialized";_tag_dyneither(_tmp1DF,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp73E,_tmp73D,_tag_dyneither(_tmp1DE,sizeof(void*),0));});});}_LL1F5:;}
# 1250
{union Cyc_CfFlowInfo_FlowInfo _tmp1E0=_tmp1C4;struct Cyc_Dict_Dict _tmp1ED;struct Cyc_List_List*_tmp1EC;if((_tmp1E0.BottomFL).tag == 1){_LL1FD: _LL1FE:
 goto _LL1FC;}else{_LL1FF: _tmp1ED=((_tmp1E0.ReachableFL).val).f1;_tmp1EC=((_tmp1E0.ReachableFL).val).f2;_LL200: {
# 1255
struct Cyc_Dict_Dict _tmp1E1=Cyc_CfFlowInfo_escape_deref(_tmp14C,_tmp1ED,(void*)_tmp1F4->hd);
{void*_tmp1E2=(void*)_tmp1F4->hd;void*_tmp1E3=_tmp1E2;struct Cyc_CfFlowInfo_Place*_tmp1EB;switch(*((int*)_tmp1E3)){case 4: _LL202: _tmp1EB=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1E3)->f1;_LL203:
# 1258
{void*_tmp1E4=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp35E))->hd)->topt));void*_tmp1E5=_tmp1E4;void*_tmp1E8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E5)->tag == 5){_LL209: _tmp1E8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E5)->f1).elt_typ;_LL20A:
# 1260
({struct Cyc_Dict_Dict _tmp743=({struct Cyc_CfFlowInfo_FlowEnv*_tmp742=_tmp14C;unsigned int _tmp741=((struct Cyc_Absyn_Exp*)_tmp35E->hd)->loc;struct Cyc_Dict_Dict _tmp740=_tmp1E1;struct Cyc_CfFlowInfo_Place*_tmp73F=_tmp1EB;Cyc_CfFlowInfo_assign_place(_tmp742,_tmp741,_tmp740,_tmp73F,
Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,_tmp1E8,0,_tmp14C->esc_all));});
# 1260
_tmp1E1=_tmp743;});
# 1264
goto _LL208;}else{_LL20B: _LL20C:
({void*_tmp1E6=0;({struct _dyneither_ptr _tmp744=({const char*_tmp1E7="anal_Rexp:bad type for initialized arg";_tag_dyneither(_tmp1E7,sizeof(char),39);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp744,_tag_dyneither(_tmp1E6,sizeof(void*),0));});});}_LL208:;}
# 1267
goto _LL201;case 5: _LL204: _LL205:
# 1269
({void*_tmp1E9=0;({struct _dyneither_ptr _tmp745=({const char*_tmp1EA="anal_Rexp:initialize attribute on unique pointers not yet supported";_tag_dyneither(_tmp1EA,sizeof(char),68);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp745,_tag_dyneither(_tmp1E9,sizeof(void*),0));});});default: _LL206: _LL207:
# 1271
 goto _LL201;}_LL201:;}
# 1273
({union Cyc_CfFlowInfo_FlowInfo _tmp746=Cyc_CfFlowInfo_ReachableFL(_tmp1E1,_tmp1EC);_tmp1C4=_tmp746;});
goto _LL1FC;}}_LL1FC:;}
# 1276
goto _LL1ED;}}_LL1ED:;}else{
# 1279
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1284
({union Cyc_CfFlowInfo_FlowInfo _tmp74B=({struct Cyc_NewControlFlow_AnalEnv*_tmp74A=env;unsigned int _tmp749=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp35E))->hd)->loc;void*_tmp748=(void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp35E->hd)->topt);union Cyc_CfFlowInfo_FlowInfo _tmp747=_tmp1C4;Cyc_NewControlFlow_use_nounique_Rval(_tmp74A,_tmp749,_tmp748,_tmp747,(void*)_tmp1F4->hd);});_tmp1C4=_tmp74B;});else{
# 1294
({union Cyc_CfFlowInfo_FlowInfo _tmp74C=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp35E))->hd)->loc,_tmp1C4,(void*)_tmp1F4->hd);_tmp1C4=_tmp74C;});
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp35E->hd)->topt),0))
({union Cyc_CfFlowInfo_FlowInfo _tmp74D=Cyc_NewControlFlow_restore_noconsume_arg(env,_tmp1C4,(struct Cyc_Absyn_Exp*)_tmp35E->hd,((struct Cyc_Absyn_Exp*)_tmp35E->hd)->loc,(void*)_tmp1F4->hd);_tmp1C4=_tmp74D;});}}}}
# 1301
Cyc_NewControlFlow_check_fn_requires(env,_tmp1C4,_tmp1C1,requires,e->loc);
# 1304
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp35F->topt))){
struct _tuple16 _tmp1F1=({struct _tuple16 _tmp1F0;({union Cyc_CfFlowInfo_FlowInfo _tmp74E=Cyc_CfFlowInfo_BottomFL();_tmp1F0.f1=_tmp74E;});({void*_tmp74F=Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,(void*)_check_null(e->topt),0,_tmp14C->unknown_all);_tmp1F0.f2=_tmp74F;});_tmp1F0;});_npop_handler(0);return _tmp1F1;}else{
# 1307
struct _tuple16 _tmp1F3=({struct _tuple16 _tmp1F2;_tmp1F2.f1=_tmp1C4;({void*_tmp750=Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,(void*)_check_null(e->topt),0,_tmp14C->unknown_all);_tmp1F2.f2=_tmp750;});_tmp1F2;});_npop_handler(0);return _tmp1F3;}};};}
# 1200
;_pop_region(temp);}case 34: _LL170: _tmp364=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp164)->f1).is_calloc;_tmp363=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp164)->f1).rgn;_tmp362=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp164)->f1).elt_type;_tmp361=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp164)->f1).num_elts;_tmp360=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp164)->f1).fat_result;_LL171: {
# 1310
void*place_val;
if(_tmp360)place_val=_tmp14C->notzeroall;else{
if(_tmp364)({void*_tmp751=Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,*((void**)_check_null(_tmp362)),0,_tmp14C->zero);place_val=_tmp751;});else{
({void*_tmp752=Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,*((void**)_check_null(_tmp362)),0,_tmp14C->unknown_none);place_val=_tmp752;});}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
if(_tmp363 != 0){
struct _RegionHandle _tmp1F7=_new_region("temp");struct _RegionHandle*temp=& _tmp1F7;_push_region(temp);
{struct _tuple23 _tmp1F8=({struct _RegionHandle*_tmp756=temp;struct Cyc_NewControlFlow_AnalEnv*_tmp755=env;union Cyc_CfFlowInfo_FlowInfo _tmp754=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp756,_tmp755,_tmp754,({struct Cyc_Absyn_Exp*_tmp1FC[2];_tmp1FC[1]=_tmp361;_tmp1FC[0]=_tmp363;({struct _RegionHandle*_tmp753=temp;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp753,_tag_dyneither(_tmp1FC,sizeof(struct Cyc_Absyn_Exp*),2));});}),1,1);});struct _tuple23 _tmp1F9=_tmp1F8;union Cyc_CfFlowInfo_FlowInfo _tmp1FB;struct Cyc_List_List*_tmp1FA;_LL20E: _tmp1FB=_tmp1F9.f1;_tmp1FA=_tmp1F9.f2;_LL20F:;
# 1319
outflow=_tmp1FB;}
# 1317
;_pop_region(temp);}else{
# 1322
struct _tuple16 _tmp1FD=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp361);struct _tuple16 _tmp1FE=_tmp1FD;union Cyc_CfFlowInfo_FlowInfo _tmp1FF;_LL211: _tmp1FF=_tmp1FE.f1;_LL212:;
outflow=_tmp1FF;}
# 1327
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e->topt),1))
return({struct _tuple16 _tmp200;_tmp200.f1=outflow;({void*_tmp758=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp201=_cycalloc(sizeof(*_tmp201));({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp757=({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp202;_tmp202.tag=5;_tmp202.f1=place_val;_tmp202;});_tmp201[0]=_tmp757;});_tmp201;});_tmp200.f2=_tmp758;});_tmp200;});else{
# 1331
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp20B=_region_malloc(_tmp14C->r,sizeof(*_tmp20B));({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp759=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp20C;_tmp20C.tag=1;_tmp20C.f1=_tmp361;_tmp20C.f2=(void*)_check_null(e->topt);_tmp20C;});_tmp20B[0]=_tmp759;});_tmp20B;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp20A=_region_malloc(_tmp14C->r,sizeof(*_tmp20A));_tmp20A->root=root;_tmp20A->path=0;_tmp20A;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp208=_region_malloc(_tmp14C->r,sizeof(*_tmp208));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp75A=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp209;_tmp209.tag=4;_tmp209.f1=place;_tmp209;});_tmp208[0]=_tmp75A;});_tmp208;});
union Cyc_CfFlowInfo_FlowInfo _tmp203=outflow;struct Cyc_Dict_Dict _tmp207;struct Cyc_List_List*_tmp206;if((_tmp203.BottomFL).tag == 1){_LL214: _LL215:
 return({struct _tuple16 _tmp204;_tmp204.f1=outflow;_tmp204.f2=rval;_tmp204;});}else{_LL216: _tmp207=((_tmp203.ReachableFL).val).f1;_tmp206=((_tmp203.ReachableFL).val).f2;_LL217:
# 1337
 return({struct _tuple16 _tmp205;({union Cyc_CfFlowInfo_FlowInfo _tmp75C=({struct Cyc_Dict_Dict _tmp75B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp207,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp75B,_tmp206);});_tmp205.f1=_tmp75C;});_tmp205.f2=rval;_tmp205;});}_LL213:;}};}case 35: _LL172: _tmp366=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp365=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL173: {
# 1342
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1348
struct _RegionHandle _tmp20D=_new_region("temp");struct _RegionHandle*temp=& _tmp20D;_push_region(temp);{
struct _tuple23 _tmp20E=({struct _RegionHandle*_tmp760=temp;struct Cyc_NewControlFlow_AnalEnv*_tmp75F=env;union Cyc_CfFlowInfo_FlowInfo _tmp75E=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp760,_tmp75F,_tmp75E,({struct Cyc_Absyn_Exp*_tmp212[2];_tmp212[1]=_tmp365;_tmp212[0]=_tmp366;({struct _RegionHandle*_tmp75D=temp;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp75D,_tag_dyneither(_tmp212,sizeof(struct Cyc_Absyn_Exp*),2));});}),0,0);});struct _tuple23 _tmp20F=_tmp20E;union Cyc_CfFlowInfo_FlowInfo _tmp211;struct Cyc_List_List*_tmp210;_LL219: _tmp211=_tmp20F.f1;_tmp210=_tmp20F.f2;_LL21A:;
# 1351
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp210))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp210->tl))->hd;
outflow=_tmp211;}{
# 1356
struct _tuple18 _tmp213=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp366);struct _tuple18 _tmp214=_tmp213;union Cyc_CfFlowInfo_AbsLVal _tmp225;_LL21C: _tmp225=_tmp214.f2;_LL21D:;{
struct _tuple18 _tmp215=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp365);struct _tuple18 _tmp216=_tmp215;union Cyc_CfFlowInfo_AbsLVal _tmp224;_LL21F: _tmp224=_tmp216.f2;_LL220:;
{union Cyc_CfFlowInfo_FlowInfo _tmp217=outflow;struct Cyc_Dict_Dict _tmp221;struct Cyc_List_List*_tmp220;if((_tmp217.ReachableFL).tag == 2){_LL222: _tmp221=((_tmp217.ReachableFL).val).f1;_tmp220=((_tmp217.ReachableFL).val).f2;_LL223:
# 1360
{union Cyc_CfFlowInfo_AbsLVal _tmp218=_tmp225;struct Cyc_CfFlowInfo_Place*_tmp21B;if((_tmp218.PlaceL).tag == 1){_LL227: _tmp21B=(_tmp218.PlaceL).val;_LL228:
# 1362
({struct Cyc_Dict_Dict _tmp761=Cyc_CfFlowInfo_assign_place(_tmp14C,_tmp366->loc,_tmp221,_tmp21B,right_rval);_tmp221=_tmp761;});
goto _LL226;}else{_LL229: _LL22A:
# 1368
 if(Cyc_CfFlowInfo_initlevel(_tmp14C,_tmp221,right_rval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp219=0;({unsigned int _tmp763=_tmp365->loc;struct _dyneither_ptr _tmp762=({const char*_tmp21A="expression may not be fully initialized";_tag_dyneither(_tmp21A,sizeof(char),40);});Cyc_Tcutil_terr(_tmp763,_tmp762,_tag_dyneither(_tmp219,sizeof(void*),0));});});
goto _LL226;}_LL226:;}
# 1372
{union Cyc_CfFlowInfo_AbsLVal _tmp21C=_tmp224;struct Cyc_CfFlowInfo_Place*_tmp21F;if((_tmp21C.PlaceL).tag == 1){_LL22C: _tmp21F=(_tmp21C.PlaceL).val;_LL22D:
# 1374
({struct Cyc_Dict_Dict _tmp764=Cyc_CfFlowInfo_assign_place(_tmp14C,_tmp365->loc,_tmp221,_tmp21F,left_rval);_tmp221=_tmp764;});
goto _LL22B;}else{_LL22E: _LL22F:
# 1377
 if(Cyc_CfFlowInfo_initlevel(_tmp14C,_tmp221,left_rval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp21D=0;({unsigned int _tmp766=_tmp366->loc;struct _dyneither_ptr _tmp765=({const char*_tmp21E="expression may not be fully initialized";_tag_dyneither(_tmp21E,sizeof(char),40);});Cyc_Tcutil_terr(_tmp766,_tmp765,_tag_dyneither(_tmp21D,sizeof(void*),0));});});
goto _LL22B;}_LL22B:;}
# 1382
({struct Cyc_List_List*_tmp767=Cyc_Relations_reln_kill_exp(_tmp14C->r,_tmp220,_tmp366);_tmp220=_tmp767;});
({struct Cyc_List_List*_tmp768=Cyc_Relations_reln_kill_exp(_tmp14C->r,_tmp220,_tmp365);_tmp220=_tmp768;});
# 1385
({union Cyc_CfFlowInfo_FlowInfo _tmp769=Cyc_CfFlowInfo_ReachableFL(_tmp221,_tmp220);outflow=_tmp769;});
Cyc_NewControlFlow_update_tryflow(env,outflow);
goto _LL221;}else{_LL224: _LL225:
 goto _LL221;}_LL221:;}{
# 1392
struct _tuple16 _tmp223=({struct _tuple16 _tmp222;_tmp222.f1=outflow;_tmp222.f2=_tmp14C->unknown_all;_tmp222;});_npop_handler(0);return _tmp223;};};};
# 1348
;_pop_region(temp);}case 16: _LL174: _tmp368=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp367=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL175: {
# 1395
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp368 != 0){
struct _RegionHandle _tmp226=_new_region("temp");struct _RegionHandle*temp=& _tmp226;_push_region(temp);
{struct _tuple23 _tmp227=({struct _RegionHandle*_tmp76D=temp;struct Cyc_NewControlFlow_AnalEnv*_tmp76C=env;union Cyc_CfFlowInfo_FlowInfo _tmp76B=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp76D,_tmp76C,_tmp76B,({struct Cyc_Absyn_Exp*_tmp22B[2];_tmp22B[1]=_tmp367;_tmp22B[0]=_tmp368;({struct _RegionHandle*_tmp76A=temp;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp76A,_tag_dyneither(_tmp22B,sizeof(struct Cyc_Absyn_Exp*),2));});}),1,1);});struct _tuple23 _tmp228=_tmp227;union Cyc_CfFlowInfo_FlowInfo _tmp22A;struct Cyc_List_List*_tmp229;_LL231: _tmp22A=_tmp228.f1;_tmp229=_tmp228.f2;_LL232:;
# 1401
outflow=_tmp22A;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp229))->tl))->hd;}
# 1399
;_pop_region(temp);}else{
# 1405
struct _tuple16 _tmp22C=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp367);struct _tuple16 _tmp22D=_tmp22C;union Cyc_CfFlowInfo_FlowInfo _tmp22F;void*_tmp22E;_LL234: _tmp22F=_tmp22D.f1;_tmp22E=_tmp22D.f2;_LL235:;
outflow=_tmp22F;
place_val=_tmp22E;}
# 1410
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e->topt),1))
return({struct _tuple16 _tmp230;_tmp230.f1=outflow;({void*_tmp76F=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp231=_cycalloc(sizeof(*_tmp231));({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp76E=({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp232;_tmp232.tag=5;_tmp232.f1=place_val;_tmp232;});_tmp231[0]=_tmp76E;});_tmp231;});_tmp230.f2=_tmp76F;});_tmp230;});else{
# 1414
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp23B=_region_malloc(_tmp14C->r,sizeof(*_tmp23B));({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp770=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp23C;_tmp23C.tag=1;_tmp23C.f1=_tmp367;_tmp23C.f2=(void*)_check_null(e->topt);_tmp23C;});_tmp23B[0]=_tmp770;});_tmp23B;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp23A=_region_malloc(_tmp14C->r,sizeof(*_tmp23A));_tmp23A->root=root;_tmp23A->path=0;_tmp23A;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp238=_region_malloc(_tmp14C->r,sizeof(*_tmp238));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp771=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp239;_tmp239.tag=4;_tmp239.f1=place;_tmp239;});_tmp238[0]=_tmp771;});_tmp238;});
union Cyc_CfFlowInfo_FlowInfo _tmp233=outflow;struct Cyc_Dict_Dict _tmp237;struct Cyc_List_List*_tmp236;if((_tmp233.BottomFL).tag == 1){_LL237: _LL238:
 return({struct _tuple16 _tmp234;_tmp234.f1=outflow;_tmp234.f2=rval;_tmp234;});}else{_LL239: _tmp237=((_tmp233.ReachableFL).val).f1;_tmp236=((_tmp233.ReachableFL).val).f2;_LL23A:
# 1420
 return({struct _tuple16 _tmp235;({union Cyc_CfFlowInfo_FlowInfo _tmp773=({struct Cyc_Dict_Dict _tmp772=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp237,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp772,_tmp236);});_tmp235.f1=_tmp773;});_tmp235.f2=rval;_tmp235;});}_LL236:;}}case 15: _LL176: _tmp369=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL177: {
# 1425
struct _tuple18 _tmp23D=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp369);struct _tuple18 _tmp23E=_tmp23D;union Cyc_CfFlowInfo_FlowInfo _tmp246;union Cyc_CfFlowInfo_AbsLVal _tmp245;_LL23C: _tmp246=_tmp23E.f1;_tmp245=_tmp23E.f2;_LL23D:;{
union Cyc_CfFlowInfo_AbsLVal _tmp23F=_tmp245;struct Cyc_CfFlowInfo_Place*_tmp244;if((_tmp23F.UnknownL).tag == 2){_LL23F: _LL240:
 return({struct _tuple16 _tmp240;_tmp240.f1=_tmp246;_tmp240.f2=_tmp14C->notzeroall;_tmp240;});}else{_LL241: _tmp244=(_tmp23F.PlaceL).val;_LL242:
 return({struct _tuple16 _tmp241;_tmp241.f1=_tmp246;({void*_tmp775=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp242=_region_malloc(env->r,sizeof(*_tmp242));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp774=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp243;_tmp243.tag=4;_tmp243.f1=_tmp244;_tmp243;});_tmp242[0]=_tmp774;});_tmp242;});_tmp241.f2=_tmp775;});_tmp241;});}_LL23E:;};}case 20: _LL178: _tmp36A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL179: {
# 1432
struct _tuple16 _tmp247=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp36A);struct _tuple16 _tmp248=_tmp247;union Cyc_CfFlowInfo_FlowInfo _tmp24A;void*_tmp249;_LL244: _tmp24A=_tmp248.f1;_tmp249=_tmp248.f2;_LL245:;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp24A,_tmp36A,_tmp249);}case 21: _LL17A: _tmp36C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp36B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL17B: {
# 1436
struct _tuple16 _tmp24B=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp36C);struct _tuple16 _tmp24C=_tmp24B;union Cyc_CfFlowInfo_FlowInfo _tmp269;void*_tmp268;_LL247: _tmp269=_tmp24C.f1;_tmp268=_tmp24C.f2;_LL248:;
if(_tmp36C->topt == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp24F;_tmp24F.tag=0;({struct _dyneither_ptr _tmp776=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp24F.f1=_tmp776;});({void*_tmp24D[1]={& _tmp24F};({struct Cyc___cycFILE*_tmp778=Cyc_stderr;struct _dyneither_ptr _tmp777=({const char*_tmp24E="aggrmember exp %s\n";_tag_dyneither(_tmp24E,sizeof(char),19);});Cyc_fprintf(_tmp778,_tmp777,_tag_dyneither(_tmp24D,sizeof(void*),1));});});});
({struct Cyc_String_pa_PrintArg_struct _tmp252;_tmp252.tag=0;({struct _dyneither_ptr _tmp779=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp36C));_tmp252.f1=_tmp779;});({void*_tmp250[1]={& _tmp252};({struct Cyc___cycFILE*_tmp77B=Cyc_stderr;struct _dyneither_ptr _tmp77A=({const char*_tmp251="oops! %s.topt = null!\n";_tag_dyneither(_tmp251,sizeof(char),23);});Cyc_fprintf(_tmp77B,_tmp77A,_tag_dyneither(_tmp250,sizeof(void*),1));});});});}{
# 1441
void*_tmp253=(void*)_check_null(_tmp36C->topt);
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp253))
# 1444
return({struct _tuple16 _tmp254;_tmp254.f1=_tmp269;({void*_tmp77C=Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,(void*)_check_null(e->topt),0,_tmp14C->unknown_all);_tmp254.f2=_tmp77C;});_tmp254;});
# 1446
if(Cyc_Absyn_is_require_union_type(_tmp253))
Cyc_NewControlFlow_check_union_requires(_tmp36C->loc,_tmp14C->r,_tmp253,_tmp36B,_tmp269);{
# 1449
struct _tuple15 _tmp255=Cyc_CfFlowInfo_unname_rval(_tmp14C->r,_tmp268);struct _tuple15 _tmp256=_tmp255;void*_tmp267;_LL24A: _tmp267=_tmp256.f1;_LL24B:;{
void*_tmp257=_tmp267;int _tmp266;int _tmp265;struct _dyneither_ptr _tmp264;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp257)->tag == 6){_LL24D: _tmp266=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp257)->f1).is_union;_tmp265=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp257)->f1).fieldnum;_tmp264=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp257)->f2;_LL24E: {
# 1452
int _tmp258=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp36C->topt),_tmp36B);
if((_tmp266  && _tmp265 != - 1) && _tmp265 != _tmp258)
return({struct _tuple16 _tmp259;_tmp259.f1=_tmp269;({void*_tmp77D=Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,(void*)_check_null(e->topt),1,_tmp14C->unknown_none);_tmp259.f2=_tmp77D;});_tmp259;});
return({struct _tuple16 _tmp25A;_tmp25A.f1=_tmp269;_tmp25A.f2=*((void**)_check_dyneither_subscript(_tmp264,sizeof(void*),_tmp258));_tmp25A;});}}else{_LL24F: _LL250:
# 1457
({void*_tmp25B=0;({struct Cyc___cycFILE*_tmp77F=Cyc_stderr;struct _dyneither_ptr _tmp77E=({const char*_tmp25C="the bad rexp is :";_tag_dyneither(_tmp25C,sizeof(char),18);});Cyc_fprintf(_tmp77F,_tmp77E,_tag_dyneither(_tmp25B,sizeof(void*),0));});});
Cyc_CfFlowInfo_print_absrval(_tmp267);
({void*_tmp25D=0;({struct Cyc___cycFILE*_tmp781=Cyc_stderr;struct _dyneither_ptr _tmp780=({const char*_tmp25E="\n";_tag_dyneither(_tmp25E,sizeof(char),2);});Cyc_fprintf(_tmp781,_tmp780,_tag_dyneither(_tmp25D,sizeof(void*),0));});});
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp25F=_cycalloc(sizeof(*_tmp25F));({struct Cyc_Core_Impossible_exn_struct _tmp785=({struct Cyc_Core_Impossible_exn_struct _tmp260;_tmp260.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp784=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp263;_tmp263.tag=0;({struct _dyneither_ptr _tmp782=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp263.f1=_tmp782;});({void*_tmp261[1]={& _tmp263};({struct _dyneither_ptr _tmp783=({const char*_tmp262="anal_Rexp: AggrMember: %s";_tag_dyneither(_tmp262,sizeof(char),26);});Cyc_aprintf(_tmp783,_tag_dyneither(_tmp261,sizeof(void*),1));});});});_tmp260.f1=_tmp784;});_tmp260;});_tmp25F[0]=_tmp785;});_tmp25F;}));}_LL24C:;};};};}case 38: _LL17C: _tmp36E=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp36D=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL17D: {
# 1467
struct _tuple16 _tmp26A=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp36E);struct _tuple16 _tmp26B=_tmp26A;union Cyc_CfFlowInfo_FlowInfo _tmp27E;void*_tmp27D;_LL252: _tmp27E=_tmp26B.f1;_tmp27D=_tmp26B.f2;_LL253:;
# 1469
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp36E->topt)))
return({struct _tuple16 _tmp26C;_tmp26C.f1=_tmp27E;_tmp26C.f2=_tmp14C->unknown_all;_tmp26C;});{
struct _tuple15 _tmp26D=Cyc_CfFlowInfo_unname_rval(_tmp14C->r,_tmp27D);struct _tuple15 _tmp26E=_tmp26D;void*_tmp27C;_LL255: _tmp27C=_tmp26E.f1;_LL256:;{
void*_tmp26F=_tmp27C;int _tmp27B;int _tmp27A;struct _dyneither_ptr _tmp279;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp26F)->tag == 6){_LL258: _tmp27B=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp26F)->f1).is_union;_tmp27A=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp26F)->f1).fieldnum;_tmp279=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp26F)->f2;_LL259: {
# 1474
int _tmp270=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp36E->topt),_tmp36D);
if(_tmp27B  && _tmp27A != - 1){
if(_tmp27A != _tmp270)
return({struct _tuple16 _tmp271;_tmp271.f1=_tmp27E;_tmp271.f2=_tmp14C->zero;_tmp271;});else{
# 1479
return({struct _tuple16 _tmp272;_tmp272.f1=_tmp27E;_tmp272.f2=_tmp14C->notzeroall;_tmp272;});}}else{
# 1481
return({struct _tuple16 _tmp273;_tmp273.f1=_tmp27E;_tmp273.f2=_tmp14C->unknown_all;_tmp273;});}}}else{_LL25A: _LL25B:
# 1483
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp274=_cycalloc(sizeof(*_tmp274));({struct Cyc_Core_Impossible_exn_struct _tmp789=({struct Cyc_Core_Impossible_exn_struct _tmp275;_tmp275.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp788=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp278;_tmp278.tag=0;({struct _dyneither_ptr _tmp786=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp278.f1=_tmp786;});({void*_tmp276[1]={& _tmp278};({struct _dyneither_ptr _tmp787=({const char*_tmp277="anal_Rexp: TagCheck_e: %s";_tag_dyneither(_tmp277,sizeof(char),26);});Cyc_aprintf(_tmp787,_tag_dyneither(_tmp276,sizeof(void*),1));});});});_tmp275.f1=_tmp788;});_tmp275;});_tmp274[0]=_tmp789;});_tmp274;}));}_LL257:;};};}case 22: _LL17E: _tmp370=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp36F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL17F: {
# 1488
struct _tuple16 _tmp27F=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp370);struct _tuple16 _tmp280=_tmp27F;union Cyc_CfFlowInfo_FlowInfo _tmp29A;void*_tmp299;_LL25D: _tmp29A=_tmp280.f1;_tmp299=_tmp280.f2;_LL25E:;{
# 1491
struct _tuple16 _tmp281=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp29A,_tmp370,_tmp299);struct _tuple16 _tmp282=_tmp281;union Cyc_CfFlowInfo_FlowInfo _tmp298;void*_tmp297;_LL260: _tmp298=_tmp282.f1;_tmp297=_tmp282.f2;_LL261:;{
# 1494
void*_tmp283=Cyc_Tcutil_compress((void*)_check_null(_tmp370->topt));void*_tmp284=_tmp283;void*_tmp296;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp284)->tag == 5){_LL263: _tmp296=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp284)->f1).elt_typ;_LL264:
# 1496
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp296))
# 1498
return({struct _tuple16 _tmp285;_tmp285.f1=_tmp298;({void*_tmp78A=Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,(void*)_check_null(e->topt),0,_tmp14C->unknown_all);_tmp285.f2=_tmp78A;});_tmp285;});
# 1500
if(Cyc_Absyn_is_require_union_type(_tmp296))
Cyc_NewControlFlow_check_union_requires(_tmp370->loc,_tmp14C->r,_tmp296,_tmp36F,_tmp29A);{
# 1503
struct _tuple15 _tmp286=Cyc_CfFlowInfo_unname_rval(_tmp14C->r,_tmp297);struct _tuple15 _tmp287=_tmp286;void*_tmp292;_LL268: _tmp292=_tmp287.f1;_LL269:;{
void*_tmp288=_tmp292;int _tmp291;int _tmp290;struct _dyneither_ptr _tmp28F;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp288)->tag == 6){_LL26B: _tmp291=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp288)->f1).is_union;_tmp290=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp288)->f1).fieldnum;_tmp28F=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp288)->f2;_LL26C: {
# 1506
int _tmp289=Cyc_CfFlowInfo_get_field_index(_tmp296,_tmp36F);
if((_tmp291  && _tmp290 != - 1) && _tmp290 != _tmp289)
return({struct _tuple16 _tmp28A;_tmp28A.f1=_tmp298;({void*_tmp78B=Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,(void*)_check_null(e->topt),1,_tmp14C->unknown_none);_tmp28A.f2=_tmp78B;});_tmp28A;});
return({struct _tuple16 _tmp28B;_tmp28B.f1=_tmp298;_tmp28B.f2=*((void**)_check_dyneither_subscript(_tmp28F,sizeof(void*),_tmp289));_tmp28B;});}}else{_LL26D: _LL26E:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp28C=_cycalloc(sizeof(*_tmp28C));({struct Cyc_Core_Impossible_exn_struct _tmp78D=({struct Cyc_Core_Impossible_exn_struct _tmp28D;_tmp28D.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp78C=({const char*_tmp28E="anal_Rexp: AggrArrow";_tag_dyneither(_tmp28E,sizeof(char),21);});_tmp28D.f1=_tmp78C;});_tmp28D;});_tmp28C[0]=_tmp78D;});_tmp28C;}));}_LL26A:;};};}else{_LL265: _LL266:
# 1512
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp293=_cycalloc(sizeof(*_tmp293));({struct Cyc_Core_Impossible_exn_struct _tmp78F=({struct Cyc_Core_Impossible_exn_struct _tmp294;_tmp294.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp78E=({const char*_tmp295="anal_Rexp: AggrArrow ptr";_tag_dyneither(_tmp295,sizeof(char),25);});_tmp294.f1=_tmp78E;});_tmp294;});_tmp293[0]=_tmp78F;});_tmp293;}));}_LL262:;};};}case 6: _LL180: _tmp373=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp372=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_tmp371=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp164)->f3;_LL181: {
# 1516
struct _tuple19 _tmp29B=Cyc_NewControlFlow_anal_test(env,inflow,_tmp373);struct _tuple19 _tmp29C=_tmp29B;union Cyc_CfFlowInfo_FlowInfo _tmp2A0;union Cyc_CfFlowInfo_FlowInfo _tmp29F;_LL270: _tmp2A0=_tmp29C.f1;_tmp29F=_tmp29C.f2;_LL271:;{
struct _tuple16 _tmp29D=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2A0,_tmp372);
struct _tuple16 _tmp29E=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp29F,_tmp371);
# 1520
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp14C,_tmp29D,_tmp29E);};}case 7: _LL182: _tmp375=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp374=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL183: {
# 1523
struct _tuple19 _tmp2A1=Cyc_NewControlFlow_anal_test(env,inflow,_tmp375);struct _tuple19 _tmp2A2=_tmp2A1;union Cyc_CfFlowInfo_FlowInfo _tmp2AC;union Cyc_CfFlowInfo_FlowInfo _tmp2AB;_LL273: _tmp2AC=_tmp2A2.f1;_tmp2AB=_tmp2A2.f2;_LL274:;{
struct _tuple16 _tmp2A3=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2AC,_tmp374);struct _tuple16 _tmp2A4=_tmp2A3;union Cyc_CfFlowInfo_FlowInfo _tmp2AA;void*_tmp2A9;_LL276: _tmp2AA=_tmp2A4.f1;_tmp2A9=_tmp2A4.f2;_LL277:;{
struct _tuple16 _tmp2A5=({struct _tuple16 _tmp2A8;_tmp2A8.f1=_tmp2AA;_tmp2A8.f2=_tmp2A9;_tmp2A8;});
struct _tuple16 _tmp2A6=({struct _tuple16 _tmp2A7;_tmp2A7.f1=_tmp2AB;_tmp2A7.f2=_tmp14C->zero;_tmp2A7;});
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp14C,_tmp2A5,_tmp2A6);};};}case 8: _LL184: _tmp377=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp376=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL185: {
# 1530
struct _tuple19 _tmp2AD=Cyc_NewControlFlow_anal_test(env,inflow,_tmp377);struct _tuple19 _tmp2AE=_tmp2AD;union Cyc_CfFlowInfo_FlowInfo _tmp2B8;union Cyc_CfFlowInfo_FlowInfo _tmp2B7;_LL279: _tmp2B8=_tmp2AE.f1;_tmp2B7=_tmp2AE.f2;_LL27A:;{
struct _tuple16 _tmp2AF=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp2B7,_tmp376);struct _tuple16 _tmp2B0=_tmp2AF;union Cyc_CfFlowInfo_FlowInfo _tmp2B6;void*_tmp2B5;_LL27C: _tmp2B6=_tmp2B0.f1;_tmp2B5=_tmp2B0.f2;_LL27D:;{
struct _tuple16 _tmp2B1=({struct _tuple16 _tmp2B4;_tmp2B4.f1=_tmp2B6;_tmp2B4.f2=_tmp2B5;_tmp2B4;});
struct _tuple16 _tmp2B2=({struct _tuple16 _tmp2B3;_tmp2B3.f1=_tmp2B8;_tmp2B3.f2=_tmp14C->notzeroall;_tmp2B3;});
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp14C,_tmp2B1,_tmp2B2);};};}case 23: _LL186: _tmp379=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp378=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL187: {
# 1537
struct _RegionHandle _tmp2B9=_new_region("temp");struct _RegionHandle*temp=& _tmp2B9;_push_region(temp);
{struct _tuple23 _tmp2BA=({struct _RegionHandle*_tmp793=temp;struct Cyc_NewControlFlow_AnalEnv*_tmp792=env;union Cyc_CfFlowInfo_FlowInfo _tmp791=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp793,_tmp792,_tmp791,({struct Cyc_Absyn_Exp*_tmp2DE[2];_tmp2DE[1]=_tmp378;_tmp2DE[0]=_tmp379;({struct _RegionHandle*_tmp790=temp;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp790,_tag_dyneither(_tmp2DE,sizeof(struct Cyc_Absyn_Exp*),2));});}),0,1);});struct _tuple23 _tmp2BB=_tmp2BA;union Cyc_CfFlowInfo_FlowInfo _tmp2DD;struct Cyc_List_List*_tmp2DC;_LL27F: _tmp2DD=_tmp2BB.f1;_tmp2DC=_tmp2BB.f2;_LL280:;{
# 1543
union Cyc_CfFlowInfo_FlowInfo _tmp2BC=_tmp2DD;
{union Cyc_CfFlowInfo_FlowInfo _tmp2BD=_tmp2DD;struct Cyc_Dict_Dict _tmp2C2;struct Cyc_List_List*_tmp2C1;if((_tmp2BD.ReachableFL).tag == 2){_LL282: _tmp2C2=((_tmp2BD.ReachableFL).val).f1;_tmp2C1=((_tmp2BD.ReachableFL).val).f2;_LL283:
# 1548
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2C2,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp2DC))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp2BE=0;({unsigned int _tmp795=_tmp378->loc;struct _dyneither_ptr _tmp794=({const char*_tmp2BF="expression may not be initialized";_tag_dyneither(_tmp2BF,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp795,_tmp794,_tag_dyneither(_tmp2BE,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp2C0=Cyc_NewControlFlow_add_subscript_reln(_tmp14C->r,_tmp2C1,_tmp379,_tmp378);
if(_tmp2C1 != _tmp2C0)
({union Cyc_CfFlowInfo_FlowInfo _tmp796=Cyc_CfFlowInfo_ReachableFL(_tmp2C2,_tmp2C0);_tmp2BC=_tmp796;});
goto _LL281;};}else{_LL284: _LL285:
 goto _LL281;}_LL281:;}{
# 1556
void*_tmp2C3=Cyc_Tcutil_compress((void*)_check_null(_tmp379->topt));void*_tmp2C4=_tmp2C3;struct Cyc_List_List*_tmp2DB;switch(*((int*)_tmp2C4)){case 10: _LL287: _tmp2DB=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2C4)->f1;_LL288: {
# 1558
struct _tuple15 _tmp2C5=Cyc_CfFlowInfo_unname_rval(_tmp14C->r,(void*)((struct Cyc_List_List*)_check_null(_tmp2DC))->hd);struct _tuple15 _tmp2C6=_tmp2C5;void*_tmp2CE;_LL28E: _tmp2CE=_tmp2C6.f1;_LL28F:;{
void*_tmp2C7=_tmp2CE;struct _dyneither_ptr _tmp2CD;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2C7)->tag == 6){_LL291: _tmp2CD=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2C7)->f2;_LL292: {
# 1561
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp378)).f1;
struct _tuple16 _tmp2C9=({struct _tuple16 _tmp2C8;_tmp2C8.f1=_tmp2BC;_tmp2C8.f2=*((void**)_check_dyneither_subscript(_tmp2CD,sizeof(void*),(int)i));_tmp2C8;});_npop_handler(0);return _tmp2C9;}}else{_LL293: _LL294:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2CA=_cycalloc(sizeof(*_tmp2CA));({struct Cyc_Core_Impossible_exn_struct _tmp798=({struct Cyc_Core_Impossible_exn_struct _tmp2CB;_tmp2CB.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp797=({const char*_tmp2CC="anal_Rexp: Subscript";_tag_dyneither(_tmp2CC,sizeof(char),21);});_tmp2CB.f1=_tmp797;});_tmp2CB;});_tmp2CA[0]=_tmp798;});_tmp2CA;}));}_LL290:;};}case 5: _LL289: _LL28A: {
# 1566
struct _tuple16 _tmp2CF=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2DD,_tmp379,(void*)((struct Cyc_List_List*)_check_null(_tmp2DC))->hd);struct _tuple16 _tmp2D0=_tmp2CF;union Cyc_CfFlowInfo_FlowInfo _tmp2D7;void*_tmp2D6;_LL296: _tmp2D7=_tmp2D0.f1;_tmp2D6=_tmp2D0.f2;_LL297:;{
union Cyc_CfFlowInfo_FlowInfo _tmp2D1=_tmp2D7;if((_tmp2D1.BottomFL).tag == 1){_LL299: _LL29A: {
struct _tuple16 _tmp2D3=({struct _tuple16 _tmp2D2;_tmp2D2.f1=_tmp2D7;_tmp2D2.f2=_tmp2D6;_tmp2D2;});_npop_handler(0);return _tmp2D3;}}else{_LL29B: _LL29C: {
struct _tuple16 _tmp2D5=({struct _tuple16 _tmp2D4;_tmp2D4.f1=_tmp2BC;_tmp2D4.f2=_tmp2D6;_tmp2D4;});_npop_handler(0);return _tmp2D5;}}_LL298:;};}default: _LL28B: _LL28C:
# 1571
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2D8=_cycalloc(sizeof(*_tmp2D8));({struct Cyc_Core_Impossible_exn_struct _tmp79A=({struct Cyc_Core_Impossible_exn_struct _tmp2D9;_tmp2D9.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp799=({const char*_tmp2DA="anal_Rexp: Subscript -- bad type";_tag_dyneither(_tmp2DA,sizeof(char),33);});_tmp2D9.f1=_tmp799;});_tmp2D9;});_tmp2D8[0]=_tmp79A;});_tmp2D8;}));}_LL286:;};};}
# 1538
;_pop_region(temp);}case 24: _LL18A: _tmp37A=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL18B: {
# 1578
struct _RegionHandle _tmp2DF=_new_region("temp");struct _RegionHandle*temp=& _tmp2DF;_push_region(temp);
{struct _tuple23 _tmp2E0=Cyc_NewControlFlow_anal_Rexps(temp,env,inflow,_tmp37A,1,1);struct _tuple23 _tmp2E1=_tmp2E0;union Cyc_CfFlowInfo_FlowInfo _tmp2ED;struct Cyc_List_List*_tmp2EC;_LL29E: _tmp2ED=_tmp2E1.f1;_tmp2EC=_tmp2E1.f2;_LL29F:;{
struct _dyneither_ptr aggrdict=({unsigned int _tmp2E7=(unsigned int)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp37A);void**_tmp2E8=(void**)({struct _RegionHandle*_tmp79B=env->r;_region_malloc(_tmp79B,_check_times(sizeof(void*),_tmp2E7));});struct _dyneither_ptr _tmp2EB=_tag_dyneither(_tmp2E8,sizeof(void*),_tmp2E7);{unsigned int _tmp2E9=_tmp2E7;unsigned int i;for(i=0;i < _tmp2E9;i ++){({typeof((void*)({
void*_tmp2EA=(void*)((struct Cyc_List_List*)_check_null(_tmp2EC))->hd;
_tmp2EC=_tmp2EC->tl;
_tmp2EA;}))_tmp79C=(void*)({
# 1582
void*_tmp2EA=(void*)((struct Cyc_List_List*)_check_null(_tmp2EC))->hd;
_tmp2EC=_tmp2EC->tl;
_tmp2EA;});_tmp2E8[i]=_tmp79C;});}}_tmp2EB;});
# 1586
struct _tuple16 _tmp2E6=({struct _tuple16 _tmp2E2;_tmp2E2.f1=_tmp2ED;({void*_tmp79F=(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2E3=_region_malloc(env->r,sizeof(*_tmp2E3));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp79E=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp2E4;_tmp2E4.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp79D=({struct Cyc_CfFlowInfo_UnionRInfo _tmp2E5;_tmp2E5.is_union=0;_tmp2E5.fieldnum=- 1;_tmp2E5;});_tmp2E4.f1=_tmp79D;});_tmp2E4.f2=aggrdict;_tmp2E4;});_tmp2E3[0]=_tmp79E;});_tmp2E3;});_tmp2E2.f2=_tmp79F;});_tmp2E2;});_npop_handler(0);return _tmp2E6;};}
# 1579
;_pop_region(temp);}case 30: _LL18C: _tmp37B=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL18D: {
# 1589
struct Cyc_List_List*fs;
{void*_tmp2EE=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2EF=_tmp2EE;struct Cyc_List_List*_tmp2F3;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2EF)->tag == 12){_LL2A1: _tmp2F3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2EF)->f2;_LL2A2:
 fs=_tmp2F3;goto _LL2A0;}else{_LL2A3: _LL2A4:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2F0=_cycalloc(sizeof(*_tmp2F0));({struct Cyc_Core_Impossible_exn_struct _tmp7A1=({struct Cyc_Core_Impossible_exn_struct _tmp2F1;_tmp2F1.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp7A0=({const char*_tmp2F2="anal_Rexp:anon struct has bad type";_tag_dyneither(_tmp2F2,sizeof(char),35);});_tmp2F1.f1=_tmp7A0;});_tmp2F1;});_tmp2F0[0]=_tmp7A1;});_tmp2F0;}));}_LL2A0:;}
# 1594
_tmp37E=_tmp37B;_tmp37D=Cyc_Absyn_StructA;_tmp37C=fs;goto _LL18F;}case 29: if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp164)->f4 != 0){if(((struct Cyc_Absyn_Aggrdecl*)((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp164)->f4)->impl != 0){_LL18E: _tmp37E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp164)->f3;_tmp37D=(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp164)->f4)->kind;_tmp37C=((((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp164)->f4)->impl)->fields;_LL18F: {
# 1596
void*exp_type=(void*)_check_null(e->topt);
struct _RegionHandle _tmp2F4=_new_region("temp");struct _RegionHandle*temp=& _tmp2F4;_push_region(temp);
{struct _tuple23 _tmp2F5=({struct _RegionHandle*_tmp7A4=temp;struct Cyc_NewControlFlow_AnalEnv*_tmp7A3=env;union Cyc_CfFlowInfo_FlowInfo _tmp7A2=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp7A4,_tmp7A3,_tmp7A2,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp37E),1,1);});
# 1598
struct _tuple23 _tmp2F6=_tmp2F5;union Cyc_CfFlowInfo_FlowInfo _tmp305;struct Cyc_List_List*_tmp304;_LL2A6: _tmp305=_tmp2F6.f1;_tmp304=_tmp2F6.f2;_LL2A7:;{
# 1600
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp14C,_tmp37C,_tmp37D == Cyc_Absyn_UnionA,_tmp14C->unknown_all);
# 1603
int field_no=-1;
{int i=0;for(0;_tmp304 != 0;(((_tmp304=_tmp304->tl,_tmp37E=_tmp37E->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp37E))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp2F7=(void*)ds->hd;void*_tmp2F8=_tmp2F7;struct _dyneither_ptr*_tmp2FD;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2F8)->tag == 0){_LL2A9: _LL2AA:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2F9=_cycalloc(sizeof(*_tmp2F9));({struct Cyc_Core_Impossible_exn_struct _tmp7A6=({struct Cyc_Core_Impossible_exn_struct _tmp2FA;_tmp2FA.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp7A5=({const char*_tmp2FB="anal_Rexp:Aggregate_e";_tag_dyneither(_tmp2FB,sizeof(char),22);});_tmp2FA.f1=_tmp7A5;});_tmp2FA;});_tmp2F9[0]=_tmp7A6;});_tmp2F9;}));}else{_LL2AB: _tmp2FD=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2F8)->f1;_LL2AC:
# 1610
({int _tmp7A7=Cyc_CfFlowInfo_get_field_index_fs(_tmp37C,_tmp2FD);field_no=_tmp7A7;});
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp304->hd;
# 1613
if(_tmp37D == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp2FC=(*((struct _tuple26*)_tmp37E->hd)).f2;
({union Cyc_CfFlowInfo_FlowInfo _tmp7A8=Cyc_NewControlFlow_use_Rval(env,_tmp2FC->loc,_tmp305,(void*)_tmp304->hd);_tmp305=_tmp7A8;});
# 1617
Cyc_NewControlFlow_check_union_requires(_tmp2FC->loc,_tmp14C->r,exp_type,_tmp2FD,_tmp305);}}_LL2A8:;}}}{
# 1620
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2FE=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp301=_region_malloc(env->r,sizeof(*_tmp301));({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp7AA=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp302;_tmp302.tag=6;({struct Cyc_CfFlowInfo_UnionRInfo _tmp7A9=({struct Cyc_CfFlowInfo_UnionRInfo _tmp303;_tmp303.is_union=_tmp37D == Cyc_Absyn_UnionA;_tmp303.fieldnum=field_no;_tmp303;});_tmp302.f1=_tmp7A9;});_tmp302.f2=aggrdict;_tmp302;});_tmp301[0]=_tmp7AA;});_tmp301;});
struct _tuple16 _tmp300=({struct _tuple16 _tmp2FF;_tmp2FF.f1=_tmp305;_tmp2FF.f2=(void*)_tmp2FE;_tmp2FF;});_npop_handler(0);return _tmp300;};};}
# 1598
;_pop_region(temp);}}else{goto _LL190;}}else{_LL190: _LL191:
# 1624
({struct Cyc_String_pa_PrintArg_struct _tmp308;_tmp308.tag=0;({struct _dyneither_ptr _tmp7AB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp308.f1=_tmp7AB;});({void*_tmp306[1]={& _tmp308};({struct _dyneither_ptr _tmp7AC=({const char*_tmp307="anal_Rexp:missing aggrdeclimpl in %s";_tag_dyneither(_tmp307,sizeof(char),37);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7AC,_tag_dyneither(_tmp306,sizeof(void*),1));});});});}case 26: _LL192: _tmp37F=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL193: {
# 1626
struct _RegionHandle _tmp309=_new_region("temp");struct _RegionHandle*temp=& _tmp309;_push_region(temp);
{struct Cyc_List_List*_tmp30A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp37F);
struct _tuple23 _tmp30B=Cyc_NewControlFlow_anal_Rexps(temp,env,inflow,_tmp30A,1,1);struct _tuple23 _tmp30C=_tmp30B;union Cyc_CfFlowInfo_FlowInfo _tmp310;struct Cyc_List_List*_tmp30F;_LL2AE: _tmp310=_tmp30C.f1;_tmp30F=_tmp30C.f2;_LL2AF:;
for(0;_tmp30F != 0;(_tmp30F=_tmp30F->tl,_tmp30A=_tmp30A->tl)){
({union Cyc_CfFlowInfo_FlowInfo _tmp7AD=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp30A))->hd)->loc,_tmp310,(void*)_tmp30F->hd);_tmp310=_tmp7AD;});}{
struct _tuple16 _tmp30E=({struct _tuple16 _tmp30D;_tmp30D.f1=_tmp310;({void*_tmp7AE=Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,(void*)_check_null(e->topt),0,_tmp14C->unknown_all);_tmp30D.f2=_tmp7AE;});_tmp30D;});_npop_handler(0);return _tmp30E;};}
# 1627
;_pop_region(temp);}case 27: _LL194: _tmp383=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp382=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_tmp381=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp164)->f3;_tmp380=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp164)->f4;_LL195: {
# 1635
struct _tuple16 _tmp311=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp382);struct _tuple16 _tmp312=_tmp311;union Cyc_CfFlowInfo_FlowInfo _tmp335;void*_tmp334;_LL2B1: _tmp335=_tmp312.f1;_tmp334=_tmp312.f2;_LL2B2:;{
union Cyc_CfFlowInfo_FlowInfo _tmp313=_tmp335;struct Cyc_Dict_Dict _tmp333;struct Cyc_List_List*_tmp332;if((_tmp313.BottomFL).tag == 1){_LL2B4: _LL2B5:
 return({struct _tuple16 _tmp314;_tmp314.f1=_tmp335;_tmp314.f2=_tmp14C->unknown_all;_tmp314;});}else{_LL2B6: _tmp333=((_tmp313.ReachableFL).val).f1;_tmp332=((_tmp313.ReachableFL).val).f2;_LL2B7:
# 1639
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp333,_tmp334)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp315=0;({unsigned int _tmp7B0=_tmp382->loc;struct _dyneither_ptr _tmp7AF=({const char*_tmp316="expression may not be initialized";_tag_dyneither(_tmp316,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp7B0,_tmp7AF,_tag_dyneither(_tmp315,sizeof(void*),0));});});{
# 1643
struct Cyc_List_List*new_relns=_tmp332;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(_tmp383);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp382,& n2))
({struct Cyc_List_List*_tmp7B1=Cyc_Relations_add_relation(env->r,n1,Cyc_Relations_Rlt,n2,_tmp332);new_relns=_tmp7B1;});
# 1649
if(_tmp332 != new_relns)
({union Cyc_CfFlowInfo_FlowInfo _tmp7B2=Cyc_CfFlowInfo_ReachableFL(_tmp333,new_relns);_tmp335=_tmp7B2;});{
# 1653
void*_tmp317=_tmp334;switch(*((int*)_tmp317)){case 0: _LL2B9: _LL2BA:
 return({struct _tuple16 _tmp318;_tmp318.f1=_tmp335;_tmp318.f2=_tmp14C->unknown_all;_tmp318;});case 1: _LL2BB: _LL2BC:
 goto _LL2BE;case 4: _LL2BD: _LL2BE: {
# 1657
struct Cyc_List_List _tmp319=({struct Cyc_List_List _tmp324;_tmp324.hd=_tmp383;_tmp324.tl=0;_tmp324;});
({union Cyc_CfFlowInfo_FlowInfo _tmp7B3=Cyc_NewControlFlow_add_vars(_tmp14C,_tmp335,& _tmp319,_tmp14C->unknown_all,e->loc,0);_tmp335=_tmp7B3;});{
# 1660
struct _tuple16 _tmp31A=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp335,_tmp381);struct _tuple16 _tmp31B=_tmp31A;union Cyc_CfFlowInfo_FlowInfo _tmp323;void*_tmp322;_LL2C2: _tmp323=_tmp31B.f1;_tmp322=_tmp31B.f2;_LL2C3:;
{union Cyc_CfFlowInfo_FlowInfo _tmp31C=_tmp323;struct Cyc_Dict_Dict _tmp321;if((_tmp31C.BottomFL).tag == 1){_LL2C5: _LL2C6:
 return({struct _tuple16 _tmp31D;_tmp31D.f1=_tmp323;_tmp31D.f2=_tmp14C->unknown_all;_tmp31D;});}else{_LL2C7: _tmp321=((_tmp31C.ReachableFL).val).f1;_LL2C8:
# 1664
 if(Cyc_CfFlowInfo_initlevel(_tmp14C,_tmp321,_tmp322)!= Cyc_CfFlowInfo_AllIL){
({void*_tmp31E=0;({unsigned int _tmp7B5=_tmp382->loc;struct _dyneither_ptr _tmp7B4=({const char*_tmp31F="expression may not be initialized";_tag_dyneither(_tmp31F,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp7B5,_tmp7B4,_tag_dyneither(_tmp31E,sizeof(void*),0));});});
return({struct _tuple16 _tmp320;({union Cyc_CfFlowInfo_FlowInfo _tmp7B6=Cyc_CfFlowInfo_BottomFL();_tmp320.f1=_tmp7B6;});_tmp320.f2=_tmp14C->unknown_all;_tmp320;});}}_LL2C4:;}
# 1669
_tmp335=_tmp323;
goto _LL2C0;};}default: _LL2BF: _LL2C0:
# 1672
 while(1){
struct Cyc_List_List _tmp325=({struct Cyc_List_List _tmp330;_tmp330.hd=_tmp383;_tmp330.tl=0;_tmp330;});
({union Cyc_CfFlowInfo_FlowInfo _tmp7B7=Cyc_NewControlFlow_add_vars(_tmp14C,_tmp335,& _tmp325,_tmp14C->unknown_all,e->loc,0);_tmp335=_tmp7B7;});{
struct _tuple16 _tmp326=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp335,_tmp381);struct _tuple16 _tmp327=_tmp326;union Cyc_CfFlowInfo_FlowInfo _tmp32F;void*_tmp32E;_LL2CA: _tmp32F=_tmp327.f1;_tmp32E=_tmp327.f2;_LL2CB:;
{union Cyc_CfFlowInfo_FlowInfo _tmp328=_tmp32F;struct Cyc_Dict_Dict _tmp32C;if((_tmp328.BottomFL).tag == 1){_LL2CD: _LL2CE:
 goto _LL2CC;}else{_LL2CF: _tmp32C=((_tmp328.ReachableFL).val).f1;_LL2D0:
# 1679
 if(Cyc_CfFlowInfo_initlevel(_tmp14C,_tmp32C,_tmp32E)!= Cyc_CfFlowInfo_AllIL){
({void*_tmp329=0;({unsigned int _tmp7B9=_tmp382->loc;struct _dyneither_ptr _tmp7B8=({const char*_tmp32A="expression may not be initialized";_tag_dyneither(_tmp32A,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp7B9,_tmp7B8,_tag_dyneither(_tmp329,sizeof(void*),0));});});
return({struct _tuple16 _tmp32B;({union Cyc_CfFlowInfo_FlowInfo _tmp7BA=Cyc_CfFlowInfo_BottomFL();_tmp32B.f1=_tmp7BA;});_tmp32B.f2=_tmp14C->unknown_all;_tmp32B;});}}_LL2CC:;}{
# 1684
union Cyc_CfFlowInfo_FlowInfo _tmp32D=Cyc_CfFlowInfo_join_flow(_tmp14C,_tmp335,_tmp32F);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp32D,_tmp335))
break;
_tmp335=_tmp32D;};};}
# 1689
return({struct _tuple16 _tmp331;_tmp331.f1=_tmp335;_tmp331.f2=_tmp14C->unknown_all;_tmp331;});}_LL2B8:;};};}_LL2B3:;};}case 28: _LL196: _tmp386=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp385=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_tmp384=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp164)->f3;_LL197: {
# 1695
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp341=_region_malloc(_tmp14C->r,sizeof(*_tmp341));({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp7BB=({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp342;_tmp342.tag=1;_tmp342.f1=_tmp386;_tmp342.f2=(void*)_check_null(e->topt);_tmp342;});_tmp341[0]=_tmp7BB;});_tmp341;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp340=_region_malloc(_tmp14C->r,sizeof(*_tmp340));_tmp340->root=root;_tmp340->path=0;_tmp340;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp33E=_region_malloc(_tmp14C->r,sizeof(*_tmp33E));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp7BC=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp33F;_tmp33F.tag=4;_tmp33F.f1=place;_tmp33F;});_tmp33E[0]=_tmp7BC;});_tmp33E;});
void*place_val;
# 1703
({void*_tmp7BD=Cyc_CfFlowInfo_typ_to_absrval(_tmp14C,_tmp385,0,_tmp14C->unknown_none);place_val=_tmp7BD;});{
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple16 _tmp336=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp386);struct _tuple16 _tmp337=_tmp336;union Cyc_CfFlowInfo_FlowInfo _tmp33D;_LL2D2: _tmp33D=_tmp337.f1;_LL2D3:;
outflow=_tmp33D;{
union Cyc_CfFlowInfo_FlowInfo _tmp338=outflow;struct Cyc_Dict_Dict _tmp33C;struct Cyc_List_List*_tmp33B;if((_tmp338.BottomFL).tag == 1){_LL2D5: _LL2D6:
 return({struct _tuple16 _tmp339;_tmp339.f1=outflow;_tmp339.f2=rval;_tmp339;});}else{_LL2D7: _tmp33C=((_tmp338.ReachableFL).val).f1;_tmp33B=((_tmp338.ReachableFL).val).f2;_LL2D8:
# 1710
 return({struct _tuple16 _tmp33A;({union Cyc_CfFlowInfo_FlowInfo _tmp7BF=({struct Cyc_Dict_Dict _tmp7BE=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp33C,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp7BE,_tmp33B);});_tmp33A.f1=_tmp7BF;});_tmp33A.f2=rval;_tmp33A;});}_LL2D4:;};};}case 37: _LL198: _tmp387=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL199: {
# 1714
struct _tuple17 _tmp343=({struct _tuple17 _tmp346;_tmp346.f1=(env->fenv)->unknown_all;_tmp346.f2=copy_ctxt;_tmp346;});
union Cyc_CfFlowInfo_FlowInfo _tmp344=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp387,& _tmp343);
return({struct _tuple16 _tmp345;_tmp345.f1=_tmp344;_tmp345.f2=_tmp343.f1;_tmp345;});}case 36: _LL19C: _LL19D:
# 1719
 goto _LL19F;case 25: _LL19E: _LL19F:
 goto _LL1A1;case 39: _LL1A0: _LL1A1:
 goto _LL1A3;default: _LL1A2: _LL1A3:
# 1723
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp347=_cycalloc(sizeof(*_tmp347));({struct Cyc_Core_Impossible_exn_struct _tmp7C1=({struct Cyc_Core_Impossible_exn_struct _tmp348;_tmp348.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp7C0=({const char*_tmp349="anal_Rexp, unexpected exp form";_tag_dyneither(_tmp349,sizeof(char),31);});_tmp348.f1=_tmp7C0;});_tmp348;});_tmp347[0]=_tmp7C1;});_tmp347;}));}_LL139:;};}
# 1735 "new_control_flow.cyc"
static struct _tuple18 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,int expanded_place_possibly_null,struct Cyc_List_List*path){
# 1745
struct Cyc_CfFlowInfo_FlowEnv*_tmp388=env->fenv;
void*_tmp389=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp38A=_tmp389;union Cyc_Absyn_Constraint*_tmp3BC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38A)->tag == 5){_LL2DA: _tmp3BC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38A)->f1).ptr_atts).bounds;_LL2DB: {
# 1748
union Cyc_CfFlowInfo_FlowInfo _tmp38B=f;struct Cyc_Dict_Dict _tmp3B8;struct Cyc_List_List*_tmp3B7;if((_tmp38B.BottomFL).tag == 1){_LL2DF: _LL2E0:
 return({struct _tuple18 _tmp38C;_tmp38C.f1=f;({union Cyc_CfFlowInfo_AbsLVal _tmp7C2=Cyc_CfFlowInfo_UnknownL();_tmp38C.f2=_tmp7C2;});_tmp38C;});}else{_LL2E1: _tmp3B8=((_tmp38B.ReachableFL).val).f1;_tmp3B7=((_tmp38B.ReachableFL).val).f2;_LL2E2: {
# 1752
struct _tuple15 _tmp38D=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);struct _tuple15 _tmp38E=_tmp38D;void*_tmp3B6;struct Cyc_List_List*_tmp3B5;_LL2E4: _tmp3B6=_tmp38E.f1;_tmp3B5=_tmp38E.f2;_LL2E5:;
retry: {void*_tmp38F=_tmp3B6;void*_tmp3AB;enum Cyc_CfFlowInfo_InitLevel _tmp3AA;void*_tmp3A9;struct Cyc_List_List*_tmp3A8;void*_tmp3A7;switch(*((int*)_tmp38F)){case 8: _LL2E7: _LL2E8:
# 1755
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp390=_cycalloc(sizeof(*_tmp390));({struct Cyc_Core_Impossible_exn_struct _tmp7C4=({struct Cyc_Core_Impossible_exn_struct _tmp391;_tmp391.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp7C3=({const char*_tmp392="named location in anal_derefL";_tag_dyneither(_tmp392,sizeof(char),30);});_tmp391.f1=_tmp7C3;});_tmp391;});_tmp390[0]=_tmp7C4;});_tmp390;}));case 1: _LL2E9: _LL2EA:
# 1757
({void*_tmp7C7=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp393=_cycalloc(sizeof(*_tmp393));({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp7C6=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp394;_tmp394.tag=Cyc_CfFlowInfo_NotZero;({struct Cyc_List_List*_tmp7C5=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3B7);_tmp394.f1=_tmp7C5;});_tmp394;});_tmp393[0]=_tmp7C6;});_tmp393;});e->annot=_tmp7C7;});goto _LL2E6;case 5: _LL2EB: _tmp3A7=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp38F)->f1;_LL2EC:
# 1760
 if(expanded_place_possibly_null)
# 1763
({void*_tmp7CA=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp395=_cycalloc(sizeof(*_tmp395));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp7C9=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp396;_tmp396.tag=Cyc_CfFlowInfo_UnknownZ;({struct Cyc_List_List*_tmp7C8=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3B7);_tmp396.f1=_tmp7C8;});_tmp396;});_tmp395[0]=_tmp7C9;});_tmp395;});e->annot=_tmp7CA;});else{
# 1766
void*_tmp397=e->annot;void*_tmp398=_tmp397;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp398)->tag == Cyc_CfFlowInfo_UnknownZ){_LL2F8: _LL2F9:
# 1770
({void*_tmp7CD=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp399=_cycalloc(sizeof(*_tmp399));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp7CC=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp39A;_tmp39A.tag=Cyc_CfFlowInfo_UnknownZ;({struct Cyc_List_List*_tmp7CB=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3B7);_tmp39A.f1=_tmp7CB;});_tmp39A;});_tmp399[0]=_tmp7CC;});_tmp399;});e->annot=_tmp7CD;});
goto _LL2F7;}else{_LL2FA: _LL2FB:
# 1773
({void*_tmp7D0=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp39B=_cycalloc(sizeof(*_tmp39B));({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp7CF=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp39C;_tmp39C.tag=Cyc_CfFlowInfo_NotZero;({struct Cyc_List_List*_tmp7CE=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3B7);_tmp39C.f1=_tmp7CE;});_tmp39C;});_tmp39B[0]=_tmp7CF;});_tmp39B;});e->annot=_tmp7D0;});
goto _LL2F7;}_LL2F7:;}
# 1778
if(Cyc_Tcutil_is_bound_one(_tmp3BC))return({struct _tuple18 _tmp39D;_tmp39D.f1=f;({union Cyc_CfFlowInfo_AbsLVal _tmp7D1=Cyc_CfFlowInfo_UnknownL();_tmp39D.f2=_tmp7D1;});_tmp39D;});
goto _LL2E6;case 4: _LL2ED: _tmp3A9=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp38F)->f1)->root;_tmp3A8=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp38F)->f1)->path;_LL2EE:
# 1782
({void*_tmp7D4=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp39E=_cycalloc(sizeof(*_tmp39E));({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp7D3=({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp39F;_tmp39F.tag=Cyc_CfFlowInfo_NotZero;({struct Cyc_List_List*_tmp7D2=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3B7);_tmp39F.f1=_tmp7D2;});_tmp39F;});_tmp39E[0]=_tmp7D3;});_tmp39E;});e->annot=_tmp7D4;});
if(Cyc_Tcutil_is_bound_one(_tmp3BC))
return({struct _tuple18 _tmp3A0;_tmp3A0.f1=f;({union Cyc_CfFlowInfo_AbsLVal _tmp7D6=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp3A1=_region_malloc(_tmp388->r,sizeof(*_tmp3A1));_tmp3A1->root=_tmp3A9;({struct Cyc_List_List*_tmp7D5=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp388->r,_tmp3A8,path);_tmp3A1->path=_tmp7D5;});_tmp3A1;}));_tmp3A0.f2=_tmp7D6;});_tmp3A0;});
goto _LL2E6;case 0: _LL2EF: _LL2F0:
# 1787
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple18 _tmp3A2;({union Cyc_CfFlowInfo_FlowInfo _tmp7D7=Cyc_CfFlowInfo_BottomFL();_tmp3A2.f1=_tmp7D7;});({union Cyc_CfFlowInfo_AbsLVal _tmp7D8=Cyc_CfFlowInfo_UnknownL();_tmp3A2.f2=_tmp7D8;});_tmp3A2;});case 2: _LL2F1: _tmp3AA=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp38F)->f1;_LL2F2:
# 1792
 if(Cyc_Tcutil_is_bound_one(_tmp3BC))
({union Cyc_CfFlowInfo_FlowInfo _tmp7D9=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp3AA,_tmp3B5);f=_tmp7D9;});
({void*_tmp7DC=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp7DB=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp3A4;_tmp3A4.tag=Cyc_CfFlowInfo_UnknownZ;({struct Cyc_List_List*_tmp7DA=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3B7);_tmp3A4.f1=_tmp7DA;});_tmp3A4;});_tmp3A3[0]=_tmp7DB;});_tmp3A3;});e->annot=_tmp7DC;});
goto _LL2E6;case 7: _LL2F3: _tmp3AB=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp38F)->f3;_LL2F4:
# 1797
 if(passthrough_consumes){
_tmp3B6=_tmp3AB;goto retry;}
# 1800
goto _LL2F6;default: _LL2F5: _LL2F6:
# 1802
({void*_tmp7DF=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp7DE=({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp3A6;_tmp3A6.tag=Cyc_CfFlowInfo_UnknownZ;({struct Cyc_List_List*_tmp7DD=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3B7);_tmp3A6.f1=_tmp7DD;});_tmp3A6;});_tmp3A5[0]=_tmp7DE;});_tmp3A5;});e->annot=_tmp7DF;});
goto _LL2E6;}_LL2E6:;}
# 1805
if(Cyc_CfFlowInfo_initlevel(_tmp388,_tmp3B8,_tmp3B6)== Cyc_CfFlowInfo_NoneIL){
struct _tuple15 _tmp3AC=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,_tmp3B6);struct _tuple15 _tmp3AD=_tmp3AC;void*_tmp3B3;_LL2FD: _tmp3B3=_tmp3AD.f1;_LL2FE:;{
void*_tmp3AE=_tmp3B3;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3AE)->tag == 7){_LL300: _LL301:
# 1809
({void*_tmp3AF=0;({unsigned int _tmp7E1=e->loc;struct _dyneither_ptr _tmp7E0=({const char*_tmp3B0="attempt to dereference an alias-free that has already been copied";_tag_dyneither(_tmp3B0,sizeof(char),66);});Cyc_CfFlowInfo_aerr(_tmp7E1,_tmp7E0,_tag_dyneither(_tmp3AF,sizeof(void*),0));});});
goto _LL2FF;}else{_LL302: _LL303:
# 1812
({void*_tmp3B1=0;({unsigned int _tmp7E3=e->loc;struct _dyneither_ptr _tmp7E2=({const char*_tmp3B2="dereference of possibly uninitialized pointer";_tag_dyneither(_tmp3B2,sizeof(char),46);});Cyc_CfFlowInfo_aerr(_tmp7E3,_tmp7E2,_tag_dyneither(_tmp3B1,sizeof(void*),0));});});
goto _LL2FF;}_LL2FF:;};}
# 1816
return({struct _tuple18 _tmp3B4;_tmp3B4.f1=f;({union Cyc_CfFlowInfo_AbsLVal _tmp7E4=Cyc_CfFlowInfo_UnknownL();_tmp3B4.f2=_tmp7E4;});_tmp3B4;});}}_LL2DE:;}}else{_LL2DC: _LL2DD:
# 1818
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));({struct Cyc_Core_Impossible_exn_struct _tmp7E6=({struct Cyc_Core_Impossible_exn_struct _tmp3BA;_tmp3BA.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp7E5=({const char*_tmp3BB="left deref of non-pointer-type";_tag_dyneither(_tmp3BB,sizeof(char),31);});_tmp3BA.f1=_tmp7E5;});_tmp3BA;});_tmp3B9[0]=_tmp7E6;});_tmp3B9;}));}_LL2D9:;}
# 1824
static struct Cyc_CfFlowInfo_Place*Cyc_NewControlFlow_make_expanded_place(struct _RegionHandle*r,struct Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*path){
# 1826
struct Cyc_CfFlowInfo_Place*_tmp3BD=p;void*_tmp3C3;struct Cyc_List_List*_tmp3C2;_LL305: _tmp3C3=_tmp3BD->root;_tmp3C2=_tmp3BD->path;_LL306:;
({struct Cyc_List_List*_tmp7E9=({struct Cyc_List_List*_tmp3BE=_region_malloc(r,sizeof(*_tmp3BE));({void*_tmp7E8=(void*)({struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*_tmp3BF=_region_malloc(r,sizeof(*_tmp3BF));({struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct _tmp7E7=({struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct _tmp3C0;_tmp3C0.tag=1;_tmp3C0;});_tmp3BF[0]=_tmp7E7;});_tmp3BF;});_tmp3BE->hd=_tmp7E8;});_tmp3BE->tl=path;_tmp3BE;});path=_tmp7E9;});
return({struct Cyc_CfFlowInfo_Place*_tmp3C1=_region_malloc(r,sizeof(*_tmp3C1));_tmp3C1->root=_tmp3C3;({struct Cyc_List_List*_tmp7EA=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp3C2,path);_tmp3C1->path=_tmp7EA;});_tmp3C1;});}
# 1835
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*path){
# 1838
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp3C4=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp3C5=inflow;struct Cyc_Dict_Dict _tmp3C8;struct Cyc_List_List*_tmp3C7;if((_tmp3C5.BottomFL).tag == 1){_LL308: _LL309:
 return({struct _tuple18 _tmp3C6;({union Cyc_CfFlowInfo_FlowInfo _tmp7EB=Cyc_CfFlowInfo_BottomFL();_tmp3C6.f1=_tmp7EB;});({union Cyc_CfFlowInfo_AbsLVal _tmp7EC=Cyc_CfFlowInfo_UnknownL();_tmp3C6.f2=_tmp7EC;});_tmp3C6;});}else{_LL30A: _tmp3C8=((_tmp3C5.ReachableFL).val).f1;_tmp3C7=((_tmp3C5.ReachableFL).val).f2;_LL30B:
# 1843
 d=_tmp3C8;}_LL307:;}{
# 1847
void*_tmp3C9=e->r;void*_tmp3CA=_tmp3C9;struct Cyc_Absyn_Exp*_tmp437;struct _dyneither_ptr*_tmp436;struct Cyc_Absyn_Exp*_tmp435;struct Cyc_Absyn_Exp*_tmp434;struct Cyc_Absyn_Exp*_tmp433;struct Cyc_Absyn_Exp*_tmp432;struct _dyneither_ptr*_tmp431;struct Cyc_Absyn_Vardecl*_tmp430;struct Cyc_Absyn_Vardecl*_tmp42F;struct Cyc_Absyn_Vardecl*_tmp42E;struct Cyc_Absyn_Vardecl*_tmp42D;struct Cyc_Absyn_Exp*_tmp42C;struct Cyc_Absyn_Exp*_tmp42B;struct Cyc_Absyn_Exp*_tmp42A;switch(*((int*)_tmp3CA)){case 14: _LL30D: _tmp42A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3CA)->f2;_LL30E:
 _tmp42B=_tmp42A;goto _LL310;case 12: _LL30F: _tmp42B=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp3CA)->f1;_LL310:
 _tmp42C=_tmp42B;goto _LL312;case 13: _LL311: _tmp42C=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp3CA)->f1;_LL312:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp42C,path);case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3CA)->f1)){case 3: _LL313: _tmp42D=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3CA)->f1)->f1;_LL314:
# 1852
 _tmp42E=_tmp42D;goto _LL316;case 4: _LL315: _tmp42E=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3CA)->f1)->f1;_LL316:
 _tmp42F=_tmp42E;goto _LL318;case 5: _LL317: _tmp42F=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3CA)->f1)->f1;_LL318:
# 1855
 return({struct _tuple18 _tmp3CB;_tmp3CB.f1=inflow;({union Cyc_CfFlowInfo_AbsLVal _tmp7EF=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp3CC=_region_malloc(env->r,sizeof(*_tmp3CC));({void*_tmp7EE=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp3CD=_region_malloc(env->r,sizeof(*_tmp3CD));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp7ED=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp3CE;_tmp3CE.tag=0;_tmp3CE.f1=_tmp42F;_tmp3CE;});_tmp3CD[0]=_tmp7ED;});_tmp3CD;});_tmp3CC->root=_tmp7EE;});_tmp3CC->path=path;_tmp3CC;}));_tmp3CB.f2=_tmp7EF;});_tmp3CB;});case 1: _LL319: _tmp430=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3CA)->f1)->f1;_LL31A:
# 1857
 return({struct _tuple18 _tmp3CF;_tmp3CF.f1=inflow;({union Cyc_CfFlowInfo_AbsLVal _tmp7F0=Cyc_CfFlowInfo_UnknownL();_tmp3CF.f2=_tmp7F0;});_tmp3CF;});default: goto _LL323;}case 22: _LL31B: _tmp432=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3CA)->f1;_tmp431=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3CA)->f2;_LL31C:
# 1860
{void*_tmp3D0=Cyc_Tcutil_compress((void*)_check_null(_tmp432->topt));void*_tmp3D1=_tmp3D0;void*_tmp3D8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D1)->tag == 5){_LL326: _tmp3D8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D1)->f1).elt_typ;_LL327:
# 1862
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp3D8)){
Cyc_NewControlFlow_check_union_requires(_tmp432->loc,_tmp3C4->r,_tmp3D8,_tmp431,inflow);
({struct Cyc_List_List*_tmp7F4=({struct Cyc_List_List*_tmp3D2=_region_malloc(env->r,sizeof(*_tmp3D2));({void*_tmp7F3=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp3D3=_region_malloc(_tmp3C4->r,sizeof(*_tmp3D3));({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp7F2=({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp3D4;_tmp3D4.tag=0;({int _tmp7F1=Cyc_CfFlowInfo_get_field_index(_tmp3D8,_tmp431);_tmp3D4.f1=_tmp7F1;});_tmp3D4;});_tmp3D3[0]=_tmp7F2;});_tmp3D3;});_tmp3D2->hd=_tmp7F3;});_tmp3D2->tl=path;_tmp3D2;});path=_tmp7F4;});}
# 1866
goto _LL325;}else{_LL328: _LL329:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp3D5=_cycalloc(sizeof(*_tmp3D5));({struct Cyc_Core_Impossible_exn_struct _tmp7F6=({struct Cyc_Core_Impossible_exn_struct _tmp3D6;_tmp3D6.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp7F5=({const char*_tmp3D7="anal_Lexp: AggrArrow ptr";_tag_dyneither(_tmp3D7,sizeof(char),25);});_tmp3D6.f1=_tmp7F5;});_tmp3D6;});_tmp3D5[0]=_tmp7F6;});_tmp3D5;}));}_LL325:;}
# 1869
_tmp433=_tmp432;goto _LL31E;case 20: _LL31D: _tmp433=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3CA)->f1;_LL31E:
# 1873
 if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp433->topt),1)){
# 1875
struct _tuple18 _tmp3D9=Cyc_NewControlFlow_anal_Lexp(env,inflow,expand_unique,passthrough_consumes,_tmp433);struct _tuple18 _tmp3DA=_tmp3D9;union Cyc_CfFlowInfo_FlowInfo _tmp400;union Cyc_CfFlowInfo_AbsLVal _tmp3FF;_LL32B: _tmp400=_tmp3DA.f1;_tmp3FF=_tmp3DA.f2;_LL32C:;{
struct _tuple18 _tmp3DB=({struct _tuple18 _tmp3FE;_tmp3FE.f1=_tmp400;_tmp3FE.f2=_tmp3FF;_tmp3FE;});struct _tuple18 _tmp3DC=_tmp3DB;struct Cyc_Dict_Dict _tmp3FD;struct Cyc_List_List*_tmp3FC;struct Cyc_CfFlowInfo_Place*_tmp3FB;if(((_tmp3DC.f1).ReachableFL).tag == 2){if(((_tmp3DC.f2).PlaceL).tag == 1){_LL32E: _tmp3FD=(((_tmp3DC.f1).ReachableFL).val).f1;_tmp3FC=(((_tmp3DC.f1).ReachableFL).val).f2;_tmp3FB=((_tmp3DC.f2).PlaceL).val;_LL32F: {
# 1878
void*_tmp3DD=Cyc_CfFlowInfo_lookup_place(_tmp3FD,_tmp3FB);
struct _tuple15 _tmp3DE=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,_tmp3DD);struct _tuple15 _tmp3DF=_tmp3DE;void*_tmp3FA;struct Cyc_List_List*_tmp3F9;_LL333: _tmp3FA=_tmp3DF.f1;_tmp3F9=_tmp3DF.f2;_LL334:;
if(expand_unique){
int _tmp3E0=1;
void*_tmp3E1=_tmp3FA;struct Cyc_Absyn_Vardecl*_tmp3F3;void*_tmp3F2;switch(*((int*)_tmp3E1)){case 8: _LL336: _tmp3F3=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3E1)->f1;_tmp3F2=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3E1)->f2;_LL337:
# 1884
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp3E2=_cycalloc(sizeof(*_tmp3E2));({struct Cyc_Core_Impossible_exn_struct _tmp7F8=({struct Cyc_Core_Impossible_exn_struct _tmp3E3;_tmp3E3.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp7F7=({const char*_tmp3E4="bad named location in anal_Lexp:deref";_tag_dyneither(_tmp3E4,sizeof(char),38);});_tmp3E3.f1=_tmp7F7;});_tmp3E3;});_tmp3E2[0]=_tmp7F8;});_tmp3E2;}));case 7: switch(*((int*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3E1)->f3)){case 4: _LL338: _LL339:
 goto _LL33B;case 5: _LL33C: _LL33D:
# 1888
 goto _LL33F;default: goto _LL342;}case 4: _LL33A: _LL33B:
# 1887
 return Cyc_NewControlFlow_anal_derefL(env,_tmp400,_tmp433,_tmp400,_tmp3DD,passthrough_consumes,0,path);case 5: _LL33E: _LL33F: {
# 1892
struct _tuple18 _tmp3E5=Cyc_NewControlFlow_anal_derefL(env,_tmp400,_tmp433,_tmp400,_tmp3DD,passthrough_consumes,0,path);struct _tuple18 _tmp3E6=_tmp3E5;union Cyc_CfFlowInfo_FlowInfo _tmp3E8;_LL345: _tmp3E8=_tmp3E6.f1;_LL346:;
return({struct _tuple18 _tmp3E7;_tmp3E7.f1=_tmp3E8;({union Cyc_CfFlowInfo_AbsLVal _tmp7F9=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp3C4->r,_tmp3FB,path));_tmp3E7.f2=_tmp7F9;});_tmp3E7;});}case 1: _LL340: _LL341:
# 1895
 _tmp3E0=0;goto _LL343;default: _LL342: _LL343: {
# 1902
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp3C4,_tmp3FD,_tmp3FA);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp3C4->unknown_all: _tmp3C4->unknown_none;
void*res=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp3F0=_region_malloc(_tmp3C4->r,sizeof(*_tmp3F0));({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp7FD=({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct _tmp3F1;_tmp3F1.tag=5;({void*_tmp7FC=({struct Cyc_CfFlowInfo_FlowEnv*_tmp7FB=_tmp3C4;void*_tmp7FA=Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp433->topt));Cyc_CfFlowInfo_typ_to_absrval(_tmp7FB,_tmp7FA,0,leaf);});_tmp3F1.f1=_tmp7FC;});_tmp3F1;});_tmp3F0[0]=_tmp7FD;});_tmp3F0;});
for(0;_tmp3F9 != 0;_tmp3F9=_tmp3F9->tl){
({void*_tmp7FF=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp3E9=_region_malloc(_tmp3C4->r,sizeof(*_tmp3E9));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp7FE=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp3EA;_tmp3EA.tag=8;_tmp3EA.f1=(struct Cyc_Absyn_Vardecl*)_tmp3F9->hd;_tmp3EA.f2=res;_tmp3EA;});_tmp3E9[0]=_tmp7FE;});_tmp3E9;});res=_tmp7FF;});}
({struct Cyc_Dict_Dict _tmp800=Cyc_CfFlowInfo_assign_place(_tmp3C4,e->loc,_tmp3FD,_tmp3FB,res);_tmp3FD=_tmp800;});{
union Cyc_CfFlowInfo_FlowInfo _tmp3EB=Cyc_CfFlowInfo_ReachableFL(_tmp3FD,_tmp3FC);
# 1915
struct _tuple18 _tmp3EC=Cyc_NewControlFlow_anal_derefL(env,_tmp3EB,_tmp433,_tmp3EB,res,passthrough_consumes,_tmp3E0,path);struct _tuple18 _tmp3ED=_tmp3EC;union Cyc_CfFlowInfo_FlowInfo _tmp3EF;_LL348: _tmp3EF=_tmp3ED.f1;_LL349:;
# 1917
return({struct _tuple18 _tmp3EE;_tmp3EE.f1=_tmp3EF;({union Cyc_CfFlowInfo_AbsLVal _tmp801=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp3C4->r,_tmp3FB,path));_tmp3EE.f2=_tmp801;});_tmp3EE;});};}}_LL335:;}else{
# 1921
void*_tmp3F4=_tmp3FA;switch(*((int*)_tmp3F4)){case 7: if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp3F4)->f3)->tag == 5){_LL34B: _LL34C:
 goto _LL34E;}else{goto _LL34F;}case 5: _LL34D: _LL34E: {
# 1926
struct _tuple18 _tmp3F5=Cyc_NewControlFlow_anal_derefL(env,_tmp400,_tmp433,_tmp400,_tmp3DD,passthrough_consumes,0,path);struct _tuple18 _tmp3F6=_tmp3F5;union Cyc_CfFlowInfo_FlowInfo _tmp3F8;_LL352: _tmp3F8=_tmp3F6.f1;_LL353:;
return({struct _tuple18 _tmp3F7;_tmp3F7.f1=_tmp3F8;({union Cyc_CfFlowInfo_AbsLVal _tmp802=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp3C4->r,_tmp3FB,path));_tmp3F7.f2=_tmp802;});_tmp3F7;});}default: _LL34F: _LL350:
# 1930
 goto _LL34A;}_LL34A:;}
# 1933
goto _LL32D;}}else{goto _LL330;}}else{_LL330: _LL331:
 goto _LL32D;}_LL32D:;};}{
# 1938
struct _tuple16 _tmp401=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp433);struct _tuple16 _tmp402=_tmp401;union Cyc_CfFlowInfo_FlowInfo _tmp404;void*_tmp403;_LL355: _tmp404=_tmp402.f1;_tmp403=_tmp402.f2;_LL356:;
# 1940
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp433,_tmp404,_tmp403,passthrough_consumes,0,path);};case 23: _LL31F: _tmp435=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3CA)->f1;_tmp434=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3CA)->f2;_LL320: {
# 1943
void*_tmp405=Cyc_Tcutil_compress((void*)_check_null(_tmp435->topt));void*_tmp406=_tmp405;switch(*((int*)_tmp406)){case 10: _LL358: _LL359: {
# 1945
unsigned int _tmp407=(Cyc_Evexp_eval_const_uint_exp(_tmp434)).f1;
return({struct Cyc_NewControlFlow_AnalEnv*_tmp809=env;union Cyc_CfFlowInfo_FlowInfo _tmp808=inflow;int _tmp807=expand_unique;int _tmp806=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp805=_tmp435;Cyc_NewControlFlow_anal_Lexp_rec(_tmp809,_tmp808,_tmp807,_tmp806,_tmp805,({struct Cyc_List_List*_tmp408=_region_malloc(env->r,sizeof(*_tmp408));({void*_tmp804=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp409=_region_malloc(env->r,sizeof(*_tmp409));({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp803=({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp40A;_tmp40A.tag=0;_tmp40A.f1=(int)_tmp407;_tmp40A;});_tmp409[0]=_tmp803;});_tmp409;});_tmp408->hd=_tmp804;});_tmp408->tl=path;_tmp408;}));});}case 5: _LL35A: _LL35B: {
# 1948
struct _RegionHandle _tmp40B=_new_region("temp");struct _RegionHandle*temp=& _tmp40B;_push_region(temp);
{struct _tuple23 _tmp40C=({struct _RegionHandle*_tmp80D=temp;struct Cyc_NewControlFlow_AnalEnv*_tmp80C=env;union Cyc_CfFlowInfo_FlowInfo _tmp80B=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp80D,_tmp80C,_tmp80B,({struct Cyc_Absyn_Exp*_tmp420[2];_tmp420[1]=_tmp434;_tmp420[0]=_tmp435;({struct _RegionHandle*_tmp80A=temp;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp80A,_tag_dyneither(_tmp420,sizeof(struct Cyc_Absyn_Exp*),2));});}),0,1);});struct _tuple23 _tmp40D=_tmp40C;union Cyc_CfFlowInfo_FlowInfo _tmp41F;struct Cyc_List_List*_tmp41E;_LL35F: _tmp41F=_tmp40D.f1;_tmp41E=_tmp40D.f2;_LL360:;{
# 1951
union Cyc_CfFlowInfo_FlowInfo _tmp40E=_tmp41F;
{union Cyc_CfFlowInfo_FlowInfo _tmp40F=_tmp41F;struct Cyc_Dict_Dict _tmp414;struct Cyc_List_List*_tmp413;if((_tmp40F.ReachableFL).tag == 2){_LL362: _tmp414=((_tmp40F.ReachableFL).val).f1;_tmp413=((_tmp40F.ReachableFL).val).f2;_LL363:
# 1954
 if(Cyc_CfFlowInfo_initlevel(_tmp3C4,_tmp414,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp41E))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp410=0;({unsigned int _tmp80F=_tmp434->loc;struct _dyneither_ptr _tmp80E=({const char*_tmp411="expression may not be initialized";_tag_dyneither(_tmp411,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp80F,_tmp80E,_tag_dyneither(_tmp410,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp412=Cyc_NewControlFlow_add_subscript_reln(_tmp3C4->r,_tmp413,_tmp435,_tmp434);
if(_tmp413 != _tmp412)
({union Cyc_CfFlowInfo_FlowInfo _tmp810=Cyc_CfFlowInfo_ReachableFL(_tmp414,_tmp412);_tmp40E=_tmp810;});
goto _LL361;};}else{_LL364: _LL365:
 goto _LL361;}_LL361:;}{
# 1963
struct _tuple18 _tmp415=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp435,_tmp41F,(void*)((struct Cyc_List_List*)_check_null(_tmp41E))->hd,passthrough_consumes,0,path);struct _tuple18 _tmp416=_tmp415;union Cyc_CfFlowInfo_FlowInfo _tmp41D;union Cyc_CfFlowInfo_AbsLVal _tmp41C;_LL367: _tmp41D=_tmp416.f1;_tmp41C=_tmp416.f2;_LL368:;{
union Cyc_CfFlowInfo_FlowInfo _tmp417=_tmp41D;if((_tmp417.BottomFL).tag == 1){_LL36A: _LL36B: {
struct _tuple18 _tmp419=({struct _tuple18 _tmp418;_tmp418.f1=_tmp41D;_tmp418.f2=_tmp41C;_tmp418;});_npop_handler(0);return _tmp419;}}else{_LL36C: _LL36D: {
struct _tuple18 _tmp41B=({struct _tuple18 _tmp41A;_tmp41A.f1=_tmp40E;_tmp41A.f2=_tmp41C;_tmp41A;});_npop_handler(0);return _tmp41B;}}_LL369:;};};};}
# 1949
;_pop_region(temp);}default: _LL35C: _LL35D:
# 1969
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp421=_cycalloc(sizeof(*_tmp421));({struct Cyc_Core_Impossible_exn_struct _tmp812=({struct Cyc_Core_Impossible_exn_struct _tmp422;_tmp422.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp811=({const char*_tmp423="anal_Lexp: Subscript -- bad type";_tag_dyneither(_tmp423,sizeof(char),33);});_tmp422.f1=_tmp811;});_tmp422;});_tmp421[0]=_tmp812;});_tmp421;}));}_LL357:;}case 21: _LL321: _tmp437=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3CA)->f1;_tmp436=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3CA)->f2;_LL322: {
# 1973
void*_tmp424=(void*)_check_null(_tmp437->topt);
if(Cyc_Absyn_is_require_union_type(_tmp424))
Cyc_NewControlFlow_check_union_requires(_tmp437->loc,_tmp3C4->r,_tmp424,_tmp436,inflow);
# 1977
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp424))
return({struct _tuple18 _tmp425;_tmp425.f1=inflow;({union Cyc_CfFlowInfo_AbsLVal _tmp813=Cyc_CfFlowInfo_UnknownL();_tmp425.f2=_tmp813;});_tmp425;});
# 1980
return({struct Cyc_NewControlFlow_AnalEnv*_tmp81B=env;union Cyc_CfFlowInfo_FlowInfo _tmp81A=inflow;int _tmp819=expand_unique;int _tmp818=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp817=_tmp437;Cyc_NewControlFlow_anal_Lexp_rec(_tmp81B,_tmp81A,_tmp819,_tmp818,_tmp817,({struct Cyc_List_List*_tmp426=_region_malloc(env->r,sizeof(*_tmp426));({void*_tmp816=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp427=_region_malloc(env->r,sizeof(*_tmp427));({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp815=({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct _tmp428;_tmp428.tag=0;({int _tmp814=
Cyc_CfFlowInfo_get_field_index(_tmp424,_tmp436);_tmp428.f1=_tmp814;});_tmp428;});_tmp427[0]=_tmp815;});_tmp427;});_tmp426->hd=_tmp816;});_tmp426->tl=path;_tmp426;}));});}default: _LL323: _LL324:
# 1984
 return({struct _tuple18 _tmp429;({union Cyc_CfFlowInfo_FlowInfo _tmp81C=Cyc_CfFlowInfo_BottomFL();_tmp429.f1=_tmp81C;});({union Cyc_CfFlowInfo_AbsLVal _tmp81D=Cyc_CfFlowInfo_UnknownL();_tmp429.f2=_tmp81D;});_tmp429;});}_LL30C:;};}
# 1988
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1991
struct _tuple18 _tmp438=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);struct _tuple18 _tmp439=_tmp438;union Cyc_CfFlowInfo_FlowInfo _tmp43C;union Cyc_CfFlowInfo_AbsLVal _tmp43B;_LL36F: _tmp43C=_tmp439.f1;_tmp43B=_tmp439.f2;_LL370:;
return({struct _tuple18 _tmp43A;_tmp43A.f1=_tmp43C;_tmp43A.f2=_tmp43B;_tmp43A;});}
# 1998
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 2001
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd,1) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
struct _tuple18 _tmp43D=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple18 _tmp43E=_tmp43D;union Cyc_CfFlowInfo_FlowInfo _tmp43F;_LL372: _tmp43F=_tmp43E.f1;_LL373:;
inflow=_tmp43F;}}}
# 2009
return inflow;}
# 2015
static struct _tuple19 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 2018
struct Cyc_CfFlowInfo_FlowEnv*_tmp440=env->fenv;
# 2021
void*r1;void*r2;
union Cyc_CfFlowInfo_FlowInfo f;
struct _RegionHandle _tmp441=_new_region("temp");struct _RegionHandle*temp=& _tmp441;_push_region(temp);{
struct _tuple23 _tmp442=Cyc_NewControlFlow_anal_Rexps(temp,env,inflow,es,0,0);struct _tuple23 _tmp443=_tmp442;union Cyc_CfFlowInfo_FlowInfo _tmp445;struct Cyc_List_List*_tmp444;_LL375: _tmp445=_tmp443.f1;_tmp444=_tmp443.f2;_LL376:;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp444))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp444->tl))->hd;
f=_tmp445;}{
# 2031
union Cyc_CfFlowInfo_FlowInfo _tmp446=f;struct Cyc_Dict_Dict _tmp490;struct Cyc_List_List*_tmp48F;if((_tmp446.BottomFL).tag == 1){_LL378: _LL379: {
struct _tuple19 _tmp448=({struct _tuple19 _tmp447;_tmp447.f1=f;_tmp447.f2=f;_tmp447;});_npop_handler(0);return _tmp448;}}else{_LL37A: _tmp490=((_tmp446.ReachableFL).val).f1;_tmp48F=((_tmp446.ReachableFL).val).f2;_LL37B: {
# 2034
struct Cyc_Absyn_Exp*_tmp449=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*_tmp44A=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp490,r1)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp44B=0;({unsigned int _tmp81F=((struct Cyc_Absyn_Exp*)es->hd)->loc;struct _dyneither_ptr _tmp81E=({const char*_tmp44C="expression may not be initialized";_tag_dyneither(_tmp44C,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp81F,_tmp81E,_tag_dyneither(_tmp44B,sizeof(void*),0));});});
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp490,r2)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp44D=0;({unsigned int _tmp821=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc;struct _dyneither_ptr _tmp820=({const char*_tmp44E="expression may not be initialized";_tag_dyneither(_tmp44E,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp821,_tmp820,_tag_dyneither(_tmp44D,sizeof(void*),0));});});{
# 2041
union Cyc_CfFlowInfo_FlowInfo _tmp44F=f;
union Cyc_CfFlowInfo_FlowInfo _tmp450=f;
# 2046
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct _tuple15 _tmp451=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r1);struct _tuple15 _tmp452=_tmp451;void*_tmp46D;struct Cyc_List_List*_tmp46C;_LL37D: _tmp46D=_tmp452.f1;_tmp46C=_tmp452.f2;_LL37E:;{
struct _tuple15 _tmp453=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r2);struct _tuple15 _tmp454=_tmp453;void*_tmp46B;struct Cyc_List_List*_tmp46A;_LL380: _tmp46B=_tmp454.f1;_tmp46A=_tmp454.f2;_LL381:;{
struct _tuple0 _tmp455=({struct _tuple0 _tmp469;_tmp469.f1=_tmp46D;_tmp469.f2=_tmp46B;_tmp469;});struct _tuple0 _tmp456=_tmp455;enum Cyc_CfFlowInfo_InitLevel _tmp468;enum Cyc_CfFlowInfo_InitLevel _tmp467;switch(*((int*)_tmp456.f1)){case 2: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp456.f2)->tag == 0){_LL383: _tmp467=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp456.f1)->f1;_LL384: {
# 2051
struct _tuple19 _tmp457=Cyc_NewControlFlow_splitzero(env,f,f,_tmp449,_tmp467,_tmp46C);struct _tuple19 _tmp458=_tmp457;union Cyc_CfFlowInfo_FlowInfo _tmp45E;union Cyc_CfFlowInfo_FlowInfo _tmp45D;_LL394: _tmp45E=_tmp458.f1;_tmp45D=_tmp458.f2;_LL395:;
{enum Cyc_Absyn_Primop _tmp459=p;switch(_tmp459){case Cyc_Absyn_Eq: _LL397: _LL398:
 _tmp44F=_tmp45D;_tmp450=_tmp45E;goto _LL396;case Cyc_Absyn_Neq: _LL399: _LL39A:
 _tmp44F=_tmp45E;_tmp450=_tmp45D;goto _LL396;default: _LL39B: _LL39C:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp45A=_cycalloc(sizeof(*_tmp45A));({struct Cyc_Core_Impossible_exn_struct _tmp823=({struct Cyc_Core_Impossible_exn_struct _tmp45B;_tmp45B.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp822=({const char*_tmp45C="anal_test, zero-split";_tag_dyneither(_tmp45C,sizeof(char),22);});_tmp45B.f1=_tmp822;});_tmp45B;});_tmp45A[0]=_tmp823;});_tmp45A;}));}_LL396:;}
# 2057
goto _LL382;}}else{goto _LL391;}case 0: switch(*((int*)_tmp456.f2)){case 2: _LL385: _tmp468=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp456.f2)->f1;_LL386: {
# 2059
struct _tuple19 _tmp45F=Cyc_NewControlFlow_splitzero(env,f,f,_tmp44A,_tmp468,_tmp46A);struct _tuple19 _tmp460=_tmp45F;union Cyc_CfFlowInfo_FlowInfo _tmp466;union Cyc_CfFlowInfo_FlowInfo _tmp465;_LL39E: _tmp466=_tmp460.f1;_tmp465=_tmp460.f2;_LL39F:;
{enum Cyc_Absyn_Primop _tmp461=p;switch(_tmp461){case Cyc_Absyn_Eq: _LL3A1: _LL3A2:
 _tmp44F=_tmp465;_tmp450=_tmp466;goto _LL3A0;case Cyc_Absyn_Neq: _LL3A3: _LL3A4:
 _tmp44F=_tmp466;_tmp450=_tmp465;goto _LL3A0;default: _LL3A5: _LL3A6:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp462=_cycalloc(sizeof(*_tmp462));({struct Cyc_Core_Impossible_exn_struct _tmp825=({struct Cyc_Core_Impossible_exn_struct _tmp463;_tmp463.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp824=({const char*_tmp464="anal_test, zero-split";_tag_dyneither(_tmp464,sizeof(char),22);});_tmp463.f1=_tmp824;});_tmp463;});_tmp462[0]=_tmp825;});_tmp462;}));}_LL3A0:;}
# 2065
goto _LL382;}case 0: _LL387: _LL388:
# 2067
 if(p == Cyc_Absyn_Eq)({union Cyc_CfFlowInfo_FlowInfo _tmp826=Cyc_CfFlowInfo_BottomFL();_tmp450=_tmp826;});else{
({union Cyc_CfFlowInfo_FlowInfo _tmp827=Cyc_CfFlowInfo_BottomFL();_tmp44F=_tmp827;});}
goto _LL382;case 1: _LL389: _LL38A:
 goto _LL38C;case 4: _LL38B: _LL38C:
 goto _LL38E;default: goto _LL391;}case 1: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp456.f2)->tag == 0){_LL38D: _LL38E:
 goto _LL390;}else{goto _LL391;}case 4: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp456.f2)->tag == 0){_LL38F: _LL390:
# 2074
 if(p == Cyc_Absyn_Neq)({union Cyc_CfFlowInfo_FlowInfo _tmp828=Cyc_CfFlowInfo_BottomFL();_tmp450=_tmp828;});else{
({union Cyc_CfFlowInfo_FlowInfo _tmp829=Cyc_CfFlowInfo_BottomFL();_tmp44F=_tmp829;});}
goto _LL382;}else{goto _LL391;}default: _LL391: _LL392:
 goto _LL382;}_LL382:;};};}
# 2085
{struct _tuple0 _tmp46E=({struct _tuple0 _tmp472;({void*_tmp82A=Cyc_Tcutil_compress((void*)_check_null(_tmp449->topt));_tmp472.f1=_tmp82A;});({void*_tmp82B=Cyc_Tcutil_compress((void*)_check_null(_tmp44A->topt));_tmp472.f2=_tmp82B;});_tmp472;});struct _tuple0 _tmp46F=_tmp46E;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46F.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46F.f1)->f1 == Cyc_Absyn_Unsigned){_LL3A8: _LL3A9:
 goto _LL3AB;}else{switch(*((int*)_tmp46F.f2)){case 6: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46F.f2)->f1 == Cyc_Absyn_Unsigned)goto _LL3AA;else{goto _LL3B0;}case 19: goto _LL3AE;default: goto _LL3B0;}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46F.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46F.f2)->f1 == Cyc_Absyn_Unsigned){_LL3AA: _LL3AB:
 goto _LL3AD;}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp46F.f1)->tag == 19)goto _LL3AC;else{goto _LL3B0;}}}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp46F.f1)->tag == 19){_LL3AC: _LL3AD:
 goto _LL3AF;}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp46F.f2)->tag == 19){_LL3AE: _LL3AF:
 goto _LL3A7;}else{_LL3B0: _LL3B1: {
struct _tuple19 _tmp471=({struct _tuple19 _tmp470;_tmp470.f1=_tmp44F;_tmp470.f2=_tmp450;_tmp470;});_npop_handler(0);return _tmp471;}}}}}_LL3A7:;}
# 2094
{enum Cyc_Absyn_Primop _tmp473=p;switch(_tmp473){case Cyc_Absyn_Eq: _LL3B3: _LL3B4:
 goto _LL3B6;case Cyc_Absyn_Neq: _LL3B5: _LL3B6: goto _LL3B8;case Cyc_Absyn_Gt: _LL3B7: _LL3B8: goto _LL3BA;case Cyc_Absyn_Gte: _LL3B9: _LL3BA: goto _LL3BC;case Cyc_Absyn_Lt: _LL3BB: _LL3BC: goto _LL3BE;case Cyc_Absyn_Lte: _LL3BD: _LL3BE: goto _LL3B2;default: _LL3BF: _LL3C0: {
struct _tuple19 _tmp475=({struct _tuple19 _tmp474;_tmp474.f1=_tmp44F;_tmp474.f2=_tmp450;_tmp474;});_npop_handler(0);return _tmp475;}}_LL3B2:;}{
# 2099
struct _RegionHandle*_tmp476=(env->fenv)->r;
struct _tuple13 _tmp477=Cyc_Relations_primop2relation(_tmp449,p,_tmp44A);struct _tuple13 _tmp478=_tmp477;struct Cyc_Absyn_Exp*_tmp48E;enum Cyc_Relations_Relation _tmp48D;struct Cyc_Absyn_Exp*_tmp48C;_LL3C2: _tmp48E=_tmp478.f1;_tmp48D=_tmp478.f2;_tmp48C=_tmp478.f3;_LL3C3:;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
# 2104
if(Cyc_Relations_exp2relnop(_tmp48E,& n1) && Cyc_Relations_exp2relnop(_tmp48C,& n2)){
# 2106
struct Cyc_List_List*_tmp479=Cyc_Relations_add_relation(_tmp476,n1,_tmp48D,n2,_tmp48F);
# 2110
struct Cyc_List_List*_tmp47A=({struct _RegionHandle*_tmp82F=_tmp476;union Cyc_Relations_RelnOp _tmp82E=n2;enum Cyc_Relations_Relation _tmp82D=Cyc_Relations_flip_relation(_tmp48D);union Cyc_Relations_RelnOp _tmp82C=n1;Cyc_Relations_add_relation(_tmp82F,_tmp82E,_tmp82D,_tmp82C,_tmp48F);});
struct _tuple19 _tmp47B=({struct _tuple19 _tmp489;_tmp489.f1=_tmp44F;_tmp489.f2=_tmp450;_tmp489;});struct _tuple19 _tmp47C=_tmp47B;struct Cyc_Dict_Dict _tmp488;struct Cyc_Dict_Dict _tmp487;struct Cyc_Dict_Dict _tmp486;struct Cyc_Dict_Dict _tmp485;if(((_tmp47C.f1).ReachableFL).tag == 2){if(((_tmp47C.f2).ReachableFL).tag == 2){_LL3C5: _tmp486=(((_tmp47C.f1).ReachableFL).val).f1;_tmp485=(((_tmp47C.f2).ReachableFL).val).f1;_LL3C6: {
# 2113
struct _tuple19 _tmp47E=({struct _tuple19 _tmp47D;({union Cyc_CfFlowInfo_FlowInfo _tmp830=Cyc_CfFlowInfo_ReachableFL(_tmp486,_tmp479);_tmp47D.f1=_tmp830;});({union Cyc_CfFlowInfo_FlowInfo _tmp831=Cyc_CfFlowInfo_ReachableFL(_tmp485,_tmp47A);_tmp47D.f2=_tmp831;});_tmp47D;});_npop_handler(0);return _tmp47E;}}else{_LL3C9: _tmp487=(((_tmp47C.f1).ReachableFL).val).f1;_LL3CA: {
# 2117
struct _tuple19 _tmp482=({struct _tuple19 _tmp481;({union Cyc_CfFlowInfo_FlowInfo _tmp832=Cyc_CfFlowInfo_ReachableFL(_tmp487,_tmp479);_tmp481.f1=_tmp832;});_tmp481.f2=_tmp450;_tmp481;});_npop_handler(0);return _tmp482;}}}else{if(((_tmp47C.f2).ReachableFL).tag == 2){_LL3C7: _tmp488=(((_tmp47C.f2).ReachableFL).val).f1;_LL3C8: {
# 2115
struct _tuple19 _tmp480=({struct _tuple19 _tmp47F;_tmp47F.f1=_tmp44F;({union Cyc_CfFlowInfo_FlowInfo _tmp833=Cyc_CfFlowInfo_ReachableFL(_tmp488,_tmp47A);_tmp47F.f2=_tmp833;});_tmp47F;});_npop_handler(0);return _tmp480;}}else{_LL3CB: _LL3CC: {
# 2119
struct _tuple19 _tmp484=({struct _tuple19 _tmp483;_tmp483.f1=_tmp44F;_tmp483.f2=_tmp450;_tmp483;});_npop_handler(0);return _tmp484;}}}_LL3C4:;}else{
# 2122
struct _tuple19 _tmp48B=({struct _tuple19 _tmp48A;_tmp48A.f1=_tmp44F;_tmp48A.f2=_tmp450;_tmp48A;});_npop_handler(0);return _tmp48B;}};};};}}_LL377:;};
# 2023
;_pop_region(temp);}
# 2128
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 2130
struct Cyc_CfFlowInfo_FlowEnv*_tmp491=env->fenv;
void*_tmp492=e->r;void*_tmp493=_tmp492;enum Cyc_Absyn_Primop _tmp4E2;struct Cyc_List_List*_tmp4E1;struct Cyc_Absyn_Exp*_tmp4E0;struct Cyc_Absyn_Exp*_tmp4DF;struct Cyc_Absyn_Exp*_tmp4DE;struct Cyc_Absyn_Exp*_tmp4DD;struct Cyc_Absyn_Exp*_tmp4DC;struct Cyc_Absyn_Exp*_tmp4DB;struct Cyc_Absyn_Exp*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4D9;struct Cyc_Absyn_Exp*_tmp4D8;struct Cyc_Absyn_Exp*_tmp4D7;switch(*((int*)_tmp493)){case 6: _LL3CE: _tmp4D9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp493)->f1;_tmp4D8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp493)->f2;_tmp4D7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp493)->f3;_LL3CF: {
# 2133
struct _tuple19 _tmp494=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4D9);struct _tuple19 _tmp495=_tmp494;union Cyc_CfFlowInfo_FlowInfo _tmp4A0;union Cyc_CfFlowInfo_FlowInfo _tmp49F;_LL3DD: _tmp4A0=_tmp495.f1;_tmp49F=_tmp495.f2;_LL3DE:;{
struct _tuple19 _tmp496=Cyc_NewControlFlow_anal_test(env,_tmp4A0,_tmp4D8);struct _tuple19 _tmp497=_tmp496;union Cyc_CfFlowInfo_FlowInfo _tmp49E;union Cyc_CfFlowInfo_FlowInfo _tmp49D;_LL3E0: _tmp49E=_tmp497.f1;_tmp49D=_tmp497.f2;_LL3E1:;{
struct _tuple19 _tmp498=Cyc_NewControlFlow_anal_test(env,_tmp49F,_tmp4D7);struct _tuple19 _tmp499=_tmp498;union Cyc_CfFlowInfo_FlowInfo _tmp49C;union Cyc_CfFlowInfo_FlowInfo _tmp49B;_LL3E3: _tmp49C=_tmp499.f1;_tmp49B=_tmp499.f2;_LL3E4:;
return({struct _tuple19 _tmp49A;({union Cyc_CfFlowInfo_FlowInfo _tmp834=Cyc_CfFlowInfo_join_flow(_tmp491,_tmp49E,_tmp49C);_tmp49A.f1=_tmp834;});({union Cyc_CfFlowInfo_FlowInfo _tmp835=
Cyc_CfFlowInfo_join_flow(_tmp491,_tmp49D,_tmp49B);_tmp49A.f2=_tmp835;});_tmp49A;});};};}case 7: _LL3D0: _tmp4DB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp493)->f1;_tmp4DA=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp493)->f2;_LL3D1: {
# 2139
struct _tuple19 _tmp4A1=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4DB);struct _tuple19 _tmp4A2=_tmp4A1;union Cyc_CfFlowInfo_FlowInfo _tmp4A9;union Cyc_CfFlowInfo_FlowInfo _tmp4A8;_LL3E6: _tmp4A9=_tmp4A2.f1;_tmp4A8=_tmp4A2.f2;_LL3E7:;{
struct _tuple19 _tmp4A3=Cyc_NewControlFlow_anal_test(env,_tmp4A9,_tmp4DA);struct _tuple19 _tmp4A4=_tmp4A3;union Cyc_CfFlowInfo_FlowInfo _tmp4A7;union Cyc_CfFlowInfo_FlowInfo _tmp4A6;_LL3E9: _tmp4A7=_tmp4A4.f1;_tmp4A6=_tmp4A4.f2;_LL3EA:;
return({struct _tuple19 _tmp4A5;_tmp4A5.f1=_tmp4A7;({union Cyc_CfFlowInfo_FlowInfo _tmp836=Cyc_CfFlowInfo_join_flow(_tmp491,_tmp4A8,_tmp4A6);_tmp4A5.f2=_tmp836;});_tmp4A5;});};}case 8: _LL3D2: _tmp4DD=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp493)->f1;_tmp4DC=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp493)->f2;_LL3D3: {
# 2143
struct _tuple19 _tmp4AA=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4DD);struct _tuple19 _tmp4AB=_tmp4AA;union Cyc_CfFlowInfo_FlowInfo _tmp4B2;union Cyc_CfFlowInfo_FlowInfo _tmp4B1;_LL3EC: _tmp4B2=_tmp4AB.f1;_tmp4B1=_tmp4AB.f2;_LL3ED:;{
struct _tuple19 _tmp4AC=Cyc_NewControlFlow_anal_test(env,_tmp4B1,_tmp4DC);struct _tuple19 _tmp4AD=_tmp4AC;union Cyc_CfFlowInfo_FlowInfo _tmp4B0;union Cyc_CfFlowInfo_FlowInfo _tmp4AF;_LL3EF: _tmp4B0=_tmp4AD.f1;_tmp4AF=_tmp4AD.f2;_LL3F0:;
return({struct _tuple19 _tmp4AE;({union Cyc_CfFlowInfo_FlowInfo _tmp837=Cyc_CfFlowInfo_join_flow(_tmp491,_tmp4B2,_tmp4B0);_tmp4AE.f1=_tmp837;});_tmp4AE.f2=_tmp4AF;_tmp4AE;});};}case 9: _LL3D4: _tmp4DF=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp493)->f1;_tmp4DE=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp493)->f2;_LL3D5: {
# 2147
struct _tuple16 _tmp4B3=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp4DF);struct _tuple16 _tmp4B4=_tmp4B3;union Cyc_CfFlowInfo_FlowInfo _tmp4B6;void*_tmp4B5;_LL3F2: _tmp4B6=_tmp4B4.f1;_tmp4B5=_tmp4B4.f2;_LL3F3:;
return Cyc_NewControlFlow_anal_test(env,_tmp4B6,_tmp4DE);}case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp493)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp493)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp493)->f2)->tl == 0){_LL3D6: _tmp4E0=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp493)->f2)->hd;_LL3D7: {
# 2150
struct _tuple19 _tmp4B7=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4E0);struct _tuple19 _tmp4B8=_tmp4B7;union Cyc_CfFlowInfo_FlowInfo _tmp4BB;union Cyc_CfFlowInfo_FlowInfo _tmp4BA;_LL3F5: _tmp4BB=_tmp4B8.f1;_tmp4BA=_tmp4B8.f2;_LL3F6:;
return({struct _tuple19 _tmp4B9;_tmp4B9.f1=_tmp4BA;_tmp4B9.f2=_tmp4BB;_tmp4B9;});}}else{goto _LL3D8;}}else{goto _LL3D8;}}else{_LL3D8: _tmp4E2=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp493)->f1;_tmp4E1=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp493)->f2;_LL3D9:
# 2153
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp4E2,_tmp4E1);}default: _LL3DA: _LL3DB: {
# 2157
struct _tuple16 _tmp4BC=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);struct _tuple16 _tmp4BD=_tmp4BC;union Cyc_CfFlowInfo_FlowInfo _tmp4D6;void*_tmp4D5;_LL3F8: _tmp4D6=_tmp4BD.f1;_tmp4D5=_tmp4BD.f2;_LL3F9:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4BE=_tmp4D6;struct Cyc_Dict_Dict _tmp4D4;if((_tmp4BE.BottomFL).tag == 1){_LL3FB: _LL3FC:
 return({struct _tuple19 _tmp4BF;_tmp4BF.f1=_tmp4D6;_tmp4BF.f2=_tmp4D6;_tmp4BF;});}else{_LL3FD: _tmp4D4=((_tmp4BE.ReachableFL).val).f1;_LL3FE: {
# 2161
struct _tuple15 _tmp4C0=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,_tmp4D5);struct _tuple15 _tmp4C1=_tmp4C0;void*_tmp4D3;struct Cyc_List_List*_tmp4D2;_LL400: _tmp4D3=_tmp4C1.f1;_tmp4D2=_tmp4C1.f2;_LL401:;{
void*_tmp4C2=_tmp4D3;enum Cyc_CfFlowInfo_InitLevel _tmp4D1;struct Cyc_Absyn_Vardecl*_tmp4D0;void*_tmp4CF;switch(*((int*)_tmp4C2)){case 8: _LL403: _tmp4D0=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4C2)->f1;_tmp4CF=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4C2)->f2;_LL404:
# 2164
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp4C3=_cycalloc(sizeof(*_tmp4C3));({struct Cyc_Core_Impossible_exn_struct _tmp839=({struct Cyc_Core_Impossible_exn_struct _tmp4C4;_tmp4C4.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp838=({const char*_tmp4C5="anal_test: bad namedlocation";_tag_dyneither(_tmp4C5,sizeof(char),29);});_tmp4C4.f1=_tmp838;});_tmp4C4;});_tmp4C3[0]=_tmp839;});_tmp4C3;}));case 0: _LL405: _LL406:
 return({struct _tuple19 _tmp4C6;({union Cyc_CfFlowInfo_FlowInfo _tmp83A=Cyc_CfFlowInfo_BottomFL();_tmp4C6.f1=_tmp83A;});_tmp4C6.f2=_tmp4D6;_tmp4C6;});case 1: _LL407: _LL408:
 goto _LL40A;case 5: _LL409: _LL40A:
 goto _LL40C;case 4: _LL40B: _LL40C:
 return({struct _tuple19 _tmp4C7;_tmp4C7.f1=_tmp4D6;({union Cyc_CfFlowInfo_FlowInfo _tmp83B=Cyc_CfFlowInfo_BottomFL();_tmp4C7.f2=_tmp83B;});_tmp4C7;});case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4C2)->f1 == Cyc_CfFlowInfo_NoneIL){_LL40D: _LL40E:
 goto _LL410;}else{_LL413: _tmp4D1=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4C2)->f1;_LL414:
# 2174
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp4D6,e,_tmp4D1,_tmp4D2);}case 3: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp4C2)->f1 == Cyc_CfFlowInfo_NoneIL){_LL40F: _LL410:
# 2170
 goto _LL412;}else{_LL415: _LL416:
# 2175
 return({struct _tuple19 _tmp4CB;_tmp4CB.f1=_tmp4D6;_tmp4CB.f2=_tmp4D6;_tmp4CB;});}case 7: _LL411: _LL412:
# 2172
({void*_tmp4C8=0;({unsigned int _tmp83D=e->loc;struct _dyneither_ptr _tmp83C=({const char*_tmp4C9="expression may not be initialized";_tag_dyneither(_tmp4C9,sizeof(char),34);});Cyc_CfFlowInfo_aerr(_tmp83D,_tmp83C,_tag_dyneither(_tmp4C8,sizeof(void*),0));});});
return({struct _tuple19 _tmp4CA;({union Cyc_CfFlowInfo_FlowInfo _tmp83E=Cyc_CfFlowInfo_BottomFL();_tmp4CA.f1=_tmp83E;});({union Cyc_CfFlowInfo_FlowInfo _tmp83F=Cyc_CfFlowInfo_BottomFL();_tmp4CA.f2=_tmp83F;});_tmp4CA;});default: _LL417: _LL418:
# 2176
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp4CC=_cycalloc(sizeof(*_tmp4CC));({struct Cyc_Core_Impossible_exn_struct _tmp841=({struct Cyc_Core_Impossible_exn_struct _tmp4CD;_tmp4CD.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp840=({const char*_tmp4CE="anal_test";_tag_dyneither(_tmp4CE,sizeof(char),10);});_tmp4CD.f1=_tmp840;});_tmp4CD;});_tmp4CC[0]=_tmp841;});_tmp4CC;}));}_LL402:;};}}_LL3FA:;};}}_LL3CD:;}struct _tuple27{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 2182
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple27*ckenv,void*root,void*rval){
# 2184
struct _tuple27*_tmp4E3=ckenv;unsigned int _tmp4EF;struct Cyc_NewControlFlow_AnalEnv*_tmp4EE;struct Cyc_Dict_Dict _tmp4ED;_LL41A: _tmp4EF=_tmp4E3->f1;_tmp4EE=_tmp4E3->f2;_tmp4ED=_tmp4E3->f3;_LL41B:;{
void*_tmp4E4=root;struct Cyc_Absyn_Vardecl*_tmp4EC;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4E4)->tag == 0){_LL41D: _tmp4EC=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4E4)->f1;_LL41E:
# 2187
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp4EC->type)){
struct _tuple15 _tmp4E5=Cyc_CfFlowInfo_unname_rval((_tmp4EE->fenv)->r,rval);struct _tuple15 _tmp4E6=_tmp4E5;void*_tmp4EB;_LL422: _tmp4EB=_tmp4E6.f1;_LL423:;{
void*_tmp4E7=_tmp4EB;switch(*((int*)_tmp4E7)){case 7: _LL425: _LL426:
 goto _LL428;case 0: _LL427: _LL428:
 goto _LL42A;case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4E7)->f1 == Cyc_CfFlowInfo_NoneIL){_LL429: _LL42A:
 goto _LL424;}else{goto _LL42B;}default: _LL42B: _LL42C:
# 2194
({struct Cyc_String_pa_PrintArg_struct _tmp4EA;_tmp4EA.tag=0;({struct _dyneither_ptr _tmp842=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4EC->name));_tmp4EA.f1=_tmp842;});({void*_tmp4E8[1]={& _tmp4EA};({unsigned int _tmp844=_tmp4EF;struct _dyneither_ptr _tmp843=({const char*_tmp4E9="unique pointers reachable from %s may become unreachable";_tag_dyneither(_tmp4E9,sizeof(char),57);});Cyc_Tcutil_warn(_tmp844,_tmp843,_tag_dyneither(_tmp4E8,sizeof(void*),1));});});});
goto _LL424;}_LL424:;};}
# 2198
goto _LL41C;}else{_LL41F: _LL420:
 goto _LL41C;}_LL41C:;};}
# 2203
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp4F0=flow;struct Cyc_Dict_Dict _tmp4F6;if((_tmp4F0.BottomFL).tag == 1){_LL42E: _LL42F:
 return;}else{_LL430: _tmp4F6=((_tmp4F0.ReachableFL).val).f1;_LL431:
# 2207
{struct Cyc_List_List*_tmp4F1=env->param_roots;for(0;_tmp4F1 != 0;_tmp4F1=_tmp4F1->tl){
if(({struct Cyc_CfFlowInfo_FlowEnv*_tmp846=env->fenv;struct Cyc_Dict_Dict _tmp845=_tmp4F6;Cyc_CfFlowInfo_initlevel(_tmp846,_tmp845,Cyc_CfFlowInfo_lookup_place(_tmp4F6,(struct Cyc_CfFlowInfo_Place*)_tmp4F1->hd));})!= Cyc_CfFlowInfo_AllIL)
({void*_tmp4F2=0;({unsigned int _tmp848=loc;struct _dyneither_ptr _tmp847=({const char*_tmp4F3="function may not initialize all the parameters with attribute 'initializes'";_tag_dyneither(_tmp4F3,sizeof(char),76);});Cyc_CfFlowInfo_aerr(_tmp848,_tmp847,_tag_dyneither(_tmp4F2,sizeof(void*),0));});});}}
# 2211
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple27 _tmp4F4=({struct _tuple27 _tmp4F5;_tmp4F5.f1=loc;_tmp4F5.f2=env;_tmp4F5.f3=_tmp4F6;_tmp4F5;});
((void(*)(void(*f)(struct _tuple27*,void*,void*),struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp4F4,_tmp4F6);}
# 2215
return;}_LL42D:;}
# 2224
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_pat_vars(struct _RegionHandle*frgn,struct Cyc_List_List*vds){
# 2227
struct Cyc_List_List*_tmp4F7=0;
struct Cyc_List_List*_tmp4F8=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp4F9=(struct _tuple24*)x->hd;struct _tuple24*_tmp4FA=_tmp4F9;struct Cyc_Absyn_Vardecl**_tmp503;struct Cyc_Absyn_Exp*_tmp502;_LL433: _tmp503=_tmp4FA->f1;_tmp502=_tmp4FA->f2;_LL434:;
if((_tmp503 != 0  && _tmp502 != 0) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp502->topt),0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4FB=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp500=_region_malloc(frgn,sizeof(*_tmp500));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp849=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp501;_tmp501.tag=0;_tmp501.f1=*_tmp503;_tmp501;});_tmp500[0]=_tmp849;});_tmp500;});
struct Cyc_CfFlowInfo_Place*_tmp4FC=({struct Cyc_CfFlowInfo_Place*_tmp4FF=_region_malloc(frgn,sizeof(*_tmp4FF));_tmp4FF->root=(void*)_tmp4FB;_tmp4FF->path=0;_tmp4FF;});
({struct Cyc_List_List*_tmp84A=({struct Cyc_List_List*_tmp4FD=_region_malloc(frgn,sizeof(*_tmp4FD));_tmp4FD->hd=_tmp4FC;_tmp4FD->tl=_tmp4F7;_tmp4FD;});_tmp4F7=_tmp84A;});
({struct Cyc_List_List*_tmp84B=({struct Cyc_List_List*_tmp4FE=_region_malloc(frgn,sizeof(*_tmp4FE));_tmp4FE->hd=_tmp502;_tmp4FE->tl=_tmp4F8;_tmp4FE;});_tmp4F8=_tmp84B;});}}}
# 2239
return({struct _tuple1 _tmp504;_tmp504.f1=_tmp4F7;_tmp504.f2=_tmp4F8;_tmp504;});}struct _tuple28{int f1;void*f2;};
# 2245
static struct _tuple28 Cyc_NewControlFlow_noconsume_place_ok(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*place,int do_unconsume,struct Cyc_Absyn_Vardecl*vd,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2252
union Cyc_CfFlowInfo_FlowInfo _tmp505=flow;struct Cyc_Dict_Dict _tmp51A;if((_tmp505.BottomFL).tag == 1){_LL436: _LL437:
# 2254
({void*_tmp506=0;({struct _dyneither_ptr _tmp84C=({const char*_tmp507="noconsume_place_ok: flow became Bottom!";_tag_dyneither(_tmp507,sizeof(char),40);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp84C,_tag_dyneither(_tmp506,sizeof(void*),0));});});}else{_LL438: _tmp51A=((_tmp505.ReachableFL).val).f1;_LL439: {
# 2261
struct Cyc_Absyn_Exp*_tmp508=Cyc_Absyn_exp_unsigned_one;
int _tmp509=0;
int _tmp50A=1;
void*_tmp50B=Cyc_CfFlowInfo_lookup_place(_tmp51A,place);
void*_tmp50C=_tmp50B;
# 2273
int varok=0;
{void*_tmp50D=_tmp50B;struct Cyc_Absyn_Vardecl*_tmp518;void*_tmp517;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp50D)->tag == 8){_LL43B: _tmp518=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp50D)->f1;_tmp517=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp50D)->f2;_LL43C:
# 2276
 if(vd == _tmp518){
_tmp50C=_tmp517;
# 2279
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2281
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp508,_tmp50A,_tmp50C,& _tmp509)){
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp510;_tmp510.tag=0;({struct _dyneither_ptr _tmp84D=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2285
Cyc_Absynpp_qvar2string(vd->name));_tmp510.f1=_tmp84D;});({void*_tmp50E[1]={& _tmp510};({unsigned int _tmp84F=loc;struct _dyneither_ptr _tmp84E=({const char*_tmp50F="function consumes parameter %s with attribute 'noconsume'";_tag_dyneither(_tmp50F,sizeof(char),58);});Cyc_CfFlowInfo_aerr(_tmp84F,_tmp84E,_tag_dyneither(_tmp50E,sizeof(void*),1));});});});}else{
# 2288
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp51A,_tmp50C)!= Cyc_CfFlowInfo_AllIL  && !do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp513;_tmp513.tag=0;({struct _dyneither_ptr _tmp850=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2291
Cyc_Absynpp_qvar2string(vd->name));_tmp513.f1=_tmp850;});({void*_tmp511[1]={& _tmp513};({unsigned int _tmp852=loc;struct _dyneither_ptr _tmp851=({const char*_tmp512="function consumes value pointed to by parameter %s, which has attribute 'noconsume'";_tag_dyneither(_tmp512,sizeof(char),84);});Cyc_CfFlowInfo_aerr(_tmp852,_tmp851,_tag_dyneither(_tmp511,sizeof(void*),1));});});});else{
# 2293
varok=1;}}}else{
# 2296
varok=1;}}else{
# 2299
goto _LL43E;}
goto _LL43A;}else{_LL43D: _LL43E:
# 2303
 if(!Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
varok=1;else{
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp516;_tmp516.tag=0;({struct _dyneither_ptr _tmp853=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2308
Cyc_Absynpp_qvar2string(vd->name));_tmp516.f1=_tmp853;});({void*_tmp514[1]={& _tmp516};({unsigned int _tmp855=loc;struct _dyneither_ptr _tmp854=({const char*_tmp515="function parameter %s with attribute 'noconsume' no longer set to its original value";_tag_dyneither(_tmp515,sizeof(char),85);});Cyc_CfFlowInfo_aerr(_tmp855,_tmp854,_tag_dyneither(_tmp514,sizeof(void*),1));});});});}
goto _LL43A;}_LL43A:;}
# 2315
return({struct _tuple28 _tmp519;_tmp519.f1=varok;_tmp519.f2=_tmp50C;_tmp519;});}}_LL435:;}
# 2321
static struct Cyc_Absyn_Vardecl*Cyc_NewControlFlow_get_vd_from_place(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_CfFlowInfo_Place*_tmp51B=p;void*_tmp523;struct Cyc_List_List*_tmp522;_LL440: _tmp523=_tmp51B->root;_tmp522=_tmp51B->path;_LL441:;
if(_tmp522 != 0)
({void*_tmp51C=0;({struct _dyneither_ptr _tmp856=({const char*_tmp51D="unconsume_params: param to unconsume is non-variable";_tag_dyneither(_tmp51D,sizeof(char),53);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp856,_tag_dyneither(_tmp51C,sizeof(void*),0));});});{
struct Cyc_Absyn_Vardecl*vd;
void*_tmp51E=_tmp523;struct Cyc_Absyn_Vardecl*_tmp521;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp51E)->tag == 0){_LL443: _tmp521=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp51E)->f1;_LL444:
 return _tmp521;}else{_LL445: _LL446:
({void*_tmp51F=0;({struct _dyneither_ptr _tmp857=({const char*_tmp520="unconsume_params: root is not a varroot";_tag_dyneither(_tmp520,sizeof(char),40);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp857,_tag_dyneither(_tmp51F,sizeof(void*),0));});});}_LL442:;};}
# 2340 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_exp(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Exp*unconsume_exp,struct Cyc_Absyn_Vardecl*vd,int varok,void*ropt,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2348
{union Cyc_CfFlowInfo_FlowInfo _tmp524=flow;struct Cyc_Dict_Dict _tmp53D;if((_tmp524.BottomFL).tag == 1){_LL448: _LL449:
 return flow;}else{_LL44A: _tmp53D=((_tmp524.ReachableFL).val).f1;_LL44B: {
# 2355
struct _tuple18 _tmp525=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,unconsume_exp);struct _tuple18 _tmp526=_tmp525;union Cyc_CfFlowInfo_FlowInfo _tmp53C;union Cyc_CfFlowInfo_AbsLVal _tmp53B;_LL44D: _tmp53C=_tmp526.f1;_tmp53B=_tmp526.f2;_LL44E:;
# 2358
{union Cyc_CfFlowInfo_AbsLVal _tmp527=_tmp53B;struct Cyc_CfFlowInfo_Place*_tmp53A;if((_tmp527.PlaceL).tag == 1){_LL450: _tmp53A=(_tmp527.PlaceL).val;_LL451: {
# 2362
void*_tmp528=Cyc_CfFlowInfo_lookup_place(_tmp53D,_tmp53A);
{void*_tmp529=_tmp528;struct Cyc_Absyn_Vardecl*_tmp539;void*_tmp538;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp529)->tag == 8){_LL455: _tmp539=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp529)->f1;_tmp538=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp529)->f2;_LL456: {
# 2368
void*new_rval;
if(_tmp539 == vd){
# 2372
if(varok){
# 2374
({void*_tmp858=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,_tmp538);_tmp528=_tmp858;});
# 2379
if(ropt != 0){
# 2385
struct _tuple16 _tmp52A=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp85A=env->fenv;struct _tuple16 _tmp859=({struct _tuple16 _tmp52E;_tmp52E.f1=_tmp53C;_tmp52E.f2=_tmp528;_tmp52E;});Cyc_CfFlowInfo_join_flow_and_rval(_tmp85A,_tmp859,({struct _tuple16 _tmp52F;_tmp52F.f1=_tmp53C;_tmp52F.f2=ropt;_tmp52F;}));});
# 2385
struct _tuple16 _tmp52B=_tmp52A;union Cyc_CfFlowInfo_FlowInfo _tmp52D;void*_tmp52C;_LL45A: _tmp52D=_tmp52B.f1;_tmp52C=_tmp52B.f2;_LL45B:;
# 2389
_tmp53C=_tmp52D;new_rval=_tmp52C;}else{
# 2394
new_rval=_tmp528;}}else{
# 2397
new_rval=_tmp538;}
# 2399
{union Cyc_CfFlowInfo_FlowInfo _tmp530=_tmp53C;struct Cyc_Dict_Dict _tmp534;struct Cyc_List_List*_tmp533;if((_tmp530.ReachableFL).tag == 2){_LL45D: _tmp534=((_tmp530.ReachableFL).val).f1;_tmp533=((_tmp530.ReachableFL).val).f2;_LL45E:
# 2401
({union Cyc_CfFlowInfo_FlowInfo _tmp85C=({struct Cyc_Dict_Dict _tmp85B=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp534,_tmp53A,new_rval);Cyc_CfFlowInfo_ReachableFL(_tmp85B,_tmp533);});flow=_tmp85C;});
# 2405
goto _LL45C;}else{_LL45F: _LL460:
# 2407
({void*_tmp531=0;({struct _dyneither_ptr _tmp85D=({const char*_tmp532="unconsume_params: joined flow became bot!";_tag_dyneither(_tmp532,sizeof(char),42);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp85D,_tag_dyneither(_tmp531,sizeof(void*),0));});});}_LL45C:;}
# 2409
goto _LL454;}else{
# 2411
goto _LL458;}
goto _LL454;}}else{_LL457: _LL458:
# 2418
 if(ropt != 0  && !
Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
({struct Cyc_String_pa_PrintArg_struct _tmp537;_tmp537.tag=0;({struct _dyneither_ptr _tmp85E=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(unconsume_exp));_tmp537.f1=_tmp85E;});({void*_tmp535[1]={& _tmp537};({unsigned int _tmp860=loc;struct _dyneither_ptr _tmp85F=({const char*_tmp536="aliased expression %s was overwritten";_tag_dyneither(_tmp536,sizeof(char),38);});Cyc_CfFlowInfo_aerr(_tmp860,_tmp85F,_tag_dyneither(_tmp535,sizeof(void*),1));});});});
# 2435 "new_control_flow.cyc"
goto _LL454;}_LL454:;}
# 2437
goto _LL44F;}}else{_LL452: _LL453:
# 2443
 goto _LL44F;}_LL44F:;}
# 2445
goto _LL447;}}_LL447:;}
# 2447
return flow;}
# 2460 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,int is_region_open,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2466
{union Cyc_CfFlowInfo_FlowInfo _tmp53E=flow;if((_tmp53E.BottomFL).tag == 1){_LL462: _LL463:
 return flow;}else{_LL464: _LL465:
 goto _LL461;}_LL461:;}{
# 2470
int _tmp53F=unconsume_exps != 0;
struct Cyc_List_List*_tmp540=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(env->r,consumed_vals);
struct Cyc_List_List*_tmp541=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(env->r,unconsume_exps);
{struct Cyc_List_List*_tmp542=_tmp540;for(0;_tmp542 != 0;
(_tmp542=_tmp542->tl,
_tmp541 != 0?_tmp541=_tmp541->tl: 0)){
# 2479
struct Cyc_Absyn_Vardecl*_tmp543=Cyc_NewControlFlow_get_vd_from_place((struct Cyc_CfFlowInfo_Place*)_tmp542->hd);
struct _tuple28 _tmp544=
is_region_open?({struct _tuple28 _tmp548;_tmp548.f1=1;_tmp548.f2=0;_tmp548;}):
 Cyc_NewControlFlow_noconsume_place_ok(env,(struct Cyc_CfFlowInfo_Place*)_tmp542->hd,_tmp53F,_tmp543,flow,loc);
# 2480
struct _tuple28 _tmp545=_tmp544;int _tmp547;void*_tmp546;_LL467: _tmp547=_tmp545.f1;_tmp546=_tmp545.f2;_LL468:;
# 2486
if(_tmp53F)
({union Cyc_CfFlowInfo_FlowInfo _tmp861=Cyc_NewControlFlow_unconsume_exp(env,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp541))->hd,_tmp543,_tmp547,_tmp546,flow,loc);flow=_tmp861;});}}
# 2489
Cyc_NewControlFlow_update_tryflow(env,flow);
return flow;};}struct _tuple29{int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 2493
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned int exp_loc){
# 2496
struct Cyc_CfFlowInfo_FlowEnv*_tmp549=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp54A=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_tmp54B=_tmp54A;struct Cyc_Core_Opt*_tmp55F;struct Cyc_Absyn_Exp*_tmp55E;struct Cyc_Absyn_Stmt*_tmp55D;unsigned int _tmp55C;_LL46A: _tmp55F=_tmp54B->pat_vars;_tmp55E=_tmp54B->where_clause;_tmp55D=_tmp54B->body;_tmp55C=_tmp54B->loc;_LL46B:;{
struct _tuple1 _tmp54C=Cyc_NewControlFlow_get_unconsume_pat_vars((env->fenv)->r,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp55F))->v);struct _tuple1 _tmp54D=_tmp54C;struct Cyc_List_List*_tmp55B;struct Cyc_List_List*_tmp55A;_LL46D: _tmp55B=_tmp54D.f1;_tmp55A=_tmp54D.f2;_LL46E:;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)_tmp55F->v,_tmp55B != 0,_tmp55C,exp_loc);
# 2504
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp54E=env->unique_pat_vars;
# 2507
if(Cyc_Tcpat_has_vars(_tmp55F))
({struct Cyc_List_List*_tmp863=({struct Cyc_List_List*_tmp54F=_region_malloc(env->r,sizeof(*_tmp54F));({struct _tuple29*_tmp862=({struct _tuple29*_tmp550=_region_malloc(env->r,sizeof(*_tmp550));_tmp550->f1=0;_tmp550->f2=_tmp55D;_tmp550->f3=_tmp55B;_tmp550->f4=_tmp55A;_tmp550;});_tmp54F->hd=_tmp862;});_tmp54F->tl=_tmp54E;_tmp54F;});env->unique_pat_vars=_tmp863;});
# 2512
if(_tmp55E != 0){
struct Cyc_Absyn_Exp*wexp=_tmp55E;
struct _tuple19 _tmp551=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);struct _tuple19 _tmp552=_tmp551;union Cyc_CfFlowInfo_FlowInfo _tmp554;union Cyc_CfFlowInfo_FlowInfo _tmp553;_LL470: _tmp554=_tmp552.f1;_tmp553=_tmp552.f2;_LL471:;
inflow=_tmp553;
({union Cyc_CfFlowInfo_FlowInfo _tmp864=Cyc_NewControlFlow_anal_stmt(env,_tmp554,_tmp55D,0);clause_outflow=_tmp864;});}else{
# 2518
({union Cyc_CfFlowInfo_FlowInfo _tmp865=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp55D,0);clause_outflow=_tmp865;});}
# 2520
env->unique_pat_vars=_tmp54E;{
union Cyc_CfFlowInfo_FlowInfo _tmp555=clause_outflow;if((_tmp555.BottomFL).tag == 1){_LL473: _LL474:
 goto _LL472;}else{_LL475: _LL476:
# 2525
({union Cyc_CfFlowInfo_FlowInfo _tmp866=Cyc_NewControlFlow_unconsume_params(env,_tmp55B,_tmp55A,0,clause_outflow,_tmp55C);clause_outflow=_tmp866;});
# 2527
if(scs->tl == 0)
return clause_outflow;else{
# 2532
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0)
({void*_tmp556=0;({unsigned int _tmp868=_tmp55D->loc;struct _dyneither_ptr _tmp867=({const char*_tmp557="switch clause may implicitly fallthru";_tag_dyneither(_tmp557,sizeof(char),38);});Cyc_CfFlowInfo_aerr(_tmp868,_tmp867,_tag_dyneither(_tmp556,sizeof(void*),0));});});else{
# 2535
({void*_tmp558=0;({unsigned int _tmp86A=_tmp55D->loc;struct _dyneither_ptr _tmp869=({const char*_tmp559="switch clause may implicitly fallthru";_tag_dyneither(_tmp559,sizeof(char),38);});Cyc_Tcutil_warn(_tmp86A,_tmp869,_tag_dyneither(_tmp558,sizeof(void*),0));});});}
# 2537
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2539
goto _LL472;}_LL472:;};};};}
# 2542
return Cyc_CfFlowInfo_BottomFL();}struct _tuple30{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2547
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple30*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2549
struct _tuple30*_tmp560=vdenv;struct Cyc_NewControlFlow_AnalEnv*_tmp56B;struct Cyc_Dict_Dict _tmp56A;unsigned int _tmp569;_LL478: _tmp56B=_tmp560->f1;_tmp56A=_tmp560->f2;_tmp569=_tmp560->f3;_LL479:;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2552
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp568;_tmp568.tag=0;_tmp568.f1=vd;_tmp568;});
# 2554
struct _tuple15 _tmp561=({struct _RegionHandle*_tmp86B=(_tmp56B->fenv)->r;Cyc_CfFlowInfo_unname_rval(_tmp86B,
((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp56A,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot));});
# 2554
struct _tuple15 _tmp562=_tmp561;void*_tmp567;_LL47B: _tmp567=_tmp562.f1;_LL47C:;{
# 2556
void*_tmp563=_tmp567;switch(*((int*)_tmp563)){case 7: _LL47E: _LL47F:
 goto _LL481;case 0: _LL480: _LL481:
 goto _LL483;case 2: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp563)->f1 == Cyc_CfFlowInfo_NoneIL){_LL482: _LL483:
 goto _LL47D;}else{goto _LL484;}default: _LL484: _LL485:
# 2561
({struct Cyc_String_pa_PrintArg_struct _tmp566;_tmp566.tag=0;({struct _dyneither_ptr _tmp86C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp566.f1=_tmp86C;});({void*_tmp564[1]={& _tmp566};({unsigned int _tmp86E=_tmp569;struct _dyneither_ptr _tmp86D=({const char*_tmp565="unique pointers may still exist after variable %s goes out of scope";_tag_dyneither(_tmp565,sizeof(char),68);});Cyc_Tcutil_warn(_tmp86E,_tmp86D,_tag_dyneither(_tmp564,sizeof(void*),1));});});});
# 2563
goto _LL47D;}_LL47D:;};}}
# 2568
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 2570
{union Cyc_CfFlowInfo_FlowInfo _tmp56C=inflow;struct Cyc_Dict_Dict _tmp579;if((_tmp56C.ReachableFL).tag == 2){_LL487: _tmp579=((_tmp56C.ReachableFL).val).f1;_LL488: {
# 2572
struct _tuple30 _tmp56D=({struct _tuple30 _tmp578;_tmp578.f1=env;_tmp578.f2=_tmp579;_tmp578.f3=decl->loc;_tmp578;});
struct Cyc_CfFlowInfo_FlowEnv*_tmp56E=env->fenv;
{void*_tmp56F=decl->r;void*_tmp570=_tmp56F;struct Cyc_List_List*_tmp577;struct Cyc_List_List*_tmp576;struct Cyc_Absyn_Vardecl*_tmp575;switch(*((int*)_tmp570)){case 0: _LL48C: _tmp575=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp570)->f1;_LL48D:
# 2576
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp56D,_tmp575);
goto _LL48B;case 2: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp570)->f2 != 0){_LL48E: _tmp576=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp570)->f2)->v;_LL48F: {
# 2579
struct _tuple1 _tmp571=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp576);struct _tuple1 _tmp572=_tmp571;struct Cyc_List_List*_tmp574;_LL495: _tmp574=_tmp572.f1;_LL496:;{
struct Cyc_List_List*_tmp573=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)(_tmp574);
_tmp577=_tmp573;goto _LL491;};}}else{goto _LL492;}case 3: _LL490: _tmp577=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp570)->f1;_LL491:
# 2583
((void(*)(void(*f)(struct _tuple30*,struct Cyc_Absyn_Vardecl*),struct _tuple30*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp56D,_tmp577);
goto _LL48B;default: _LL492: _LL493:
 goto _LL48B;}_LL48B:;}
# 2587
goto _LL486;}}else{_LL489: _LL48A:
 goto _LL486;}_LL486:;}
# 2590
return;}
# 2596
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_pat_vars(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*src,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2600
int num_pop=((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->pat_pop_table,src);
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;num_pop > 0;(x=x->tl,-- num_pop)){
struct _tuple29*_tmp57A=(struct _tuple29*)((struct Cyc_List_List*)_check_null(x))->hd;struct _tuple29*_tmp57B=_tmp57A;int _tmp57F;struct Cyc_Absyn_Stmt*_tmp57E;struct Cyc_List_List*_tmp57D;struct Cyc_List_List*_tmp57C;_LL498: _tmp57F=_tmp57B->f1;_tmp57E=_tmp57B->f2;_tmp57D=_tmp57B->f3;_tmp57C=_tmp57B->f4;_LL499:;
({union Cyc_CfFlowInfo_FlowInfo _tmp86F=Cyc_NewControlFlow_unconsume_params(env,_tmp57D,_tmp57C,_tmp57F,inflow,dest->loc);inflow=_tmp86F;});}}
# 2605
return inflow;}
# 2611
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple17*rval_opt){
# 2614
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple20 _tmp580=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct _tuple20 _tmp581=_tmp580;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp624;union Cyc_CfFlowInfo_FlowInfo*_tmp623;_LL49B: _tmp624=_tmp581.f1;_tmp623=_tmp581.f2;_LL49C:;
inflow=*_tmp623;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp582=env->fenv;
# 2621
void*_tmp583=s->r;void*_tmp584=_tmp583;struct Cyc_Absyn_Stmt*_tmp622;struct Cyc_Absyn_Decl*_tmp621;struct Cyc_Absyn_Stmt*_tmp620;struct Cyc_Absyn_Vardecl*_tmp61F;struct Cyc_Absyn_Exp*_tmp61E;unsigned int _tmp61D;struct Cyc_Absyn_Stmt*_tmp61C;struct Cyc_List_List*_tmp61B;struct Cyc_Absyn_Exp*_tmp61A;unsigned int _tmp619;struct Cyc_Absyn_Stmt*_tmp618;struct Cyc_Absyn_Stmt*_tmp617;struct Cyc_List_List*_tmp616;void*_tmp615;struct Cyc_Absyn_Exp*_tmp614;struct Cyc_List_List*_tmp613;void*_tmp612;struct Cyc_List_List*_tmp611;struct Cyc_Absyn_Switch_clause*_tmp610;struct Cyc_Absyn_Exp*_tmp60F;struct Cyc_Absyn_Exp*_tmp60E;struct Cyc_Absyn_Stmt*_tmp60D;struct Cyc_Absyn_Exp*_tmp60C;struct Cyc_Absyn_Stmt*_tmp60B;struct Cyc_Absyn_Stmt*_tmp60A;struct Cyc_Absyn_Stmt*_tmp609;struct Cyc_Absyn_Exp*_tmp608;struct Cyc_Absyn_Stmt*_tmp607;struct Cyc_Absyn_Exp*_tmp606;struct Cyc_Absyn_Stmt*_tmp605;struct Cyc_Absyn_Stmt*_tmp604;struct Cyc_Absyn_Exp*_tmp603;struct Cyc_Absyn_Stmt*_tmp602;struct Cyc_Absyn_Stmt*_tmp601;struct Cyc_Absyn_Stmt*_tmp600;struct Cyc_Absyn_Stmt*_tmp5FF;struct Cyc_Absyn_Exp*_tmp5FE;struct Cyc_Absyn_Exp*_tmp5FD;switch(*((int*)_tmp584)){case 0: _LL49E: _LL49F:
 return inflow;case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp584)->f1 == 0){_LL4A0: _LL4A1:
# 2625
 if(env->noreturn)
({void*_tmp585=0;({unsigned int _tmp871=s->loc;struct _dyneither_ptr _tmp870=({const char*_tmp586="`noreturn' function might return";_tag_dyneither(_tmp586,sizeof(char),33);});Cyc_CfFlowInfo_aerr(_tmp871,_tmp870,_tag_dyneither(_tmp585,sizeof(void*),0));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}else{_LL4A2: _tmp5FD=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp584)->f1;_LL4A3:
# 2631
 if(env->noreturn)
({void*_tmp587=0;({unsigned int _tmp873=s->loc;struct _dyneither_ptr _tmp872=({const char*_tmp588="`noreturn' function might return";_tag_dyneither(_tmp588,sizeof(char),33);});Cyc_CfFlowInfo_aerr(_tmp873,_tmp872,_tag_dyneither(_tmp587,sizeof(void*),0));});});{
struct _tuple16 _tmp589=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp5FD));struct _tuple16 _tmp58A=_tmp589;union Cyc_CfFlowInfo_FlowInfo _tmp58C;void*_tmp58B;_LL4C7: _tmp58C=_tmp58A.f1;_tmp58B=_tmp58A.f2;_LL4C8:;
({union Cyc_CfFlowInfo_FlowInfo _tmp874=Cyc_NewControlFlow_use_Rval(env,_tmp5FD->loc,_tmp58C,_tmp58B);_tmp58C=_tmp874;});
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp58C);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,_tmp58C,s->loc);
return Cyc_CfFlowInfo_BottomFL();};}case 1: _LL4A4: _tmp5FE=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp584)->f1;_LL4A5: {
# 2640
struct _tuple17*_tmp58D=rval_opt;void**_tmp593;int _tmp592;if(_tmp58D != 0){_LL4CA: _tmp593=(void**)& _tmp58D->f1;_tmp592=_tmp58D->f2;_LL4CB: {
# 2642
struct _tuple16 _tmp58E=Cyc_NewControlFlow_anal_Rexp(env,_tmp592,inflow,_tmp5FE);struct _tuple16 _tmp58F=_tmp58E;union Cyc_CfFlowInfo_FlowInfo _tmp591;void*_tmp590;_LL4CF: _tmp591=_tmp58F.f1;_tmp590=_tmp58F.f2;_LL4D0:;
*_tmp593=_tmp590;
return _tmp591;}}else{_LL4CC: _LL4CD:
# 2646
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp5FE)).f1;}_LL4C9:;}case 2: _LL4A6: _tmp600=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp584)->f1;_tmp5FF=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp584)->f2;_LL4A7:
# 2650
 return({struct Cyc_NewControlFlow_AnalEnv*_tmp877=env;union Cyc_CfFlowInfo_FlowInfo _tmp876=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp600,0);struct Cyc_Absyn_Stmt*_tmp875=_tmp5FF;Cyc_NewControlFlow_anal_stmt(_tmp877,_tmp876,_tmp875,rval_opt);});case 4: _LL4A8: _tmp603=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp584)->f1;_tmp602=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp584)->f2;_tmp601=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp584)->f3;_LL4A9: {
# 2653
struct _tuple19 _tmp594=Cyc_NewControlFlow_anal_test(env,inflow,_tmp603);struct _tuple19 _tmp595=_tmp594;union Cyc_CfFlowInfo_FlowInfo _tmp599;union Cyc_CfFlowInfo_FlowInfo _tmp598;_LL4D2: _tmp599=_tmp595.f1;_tmp598=_tmp595.f2;_LL4D3:;{
# 2660
union Cyc_CfFlowInfo_FlowInfo _tmp596=Cyc_NewControlFlow_anal_stmt(env,_tmp598,_tmp601,0);
union Cyc_CfFlowInfo_FlowInfo _tmp597=Cyc_NewControlFlow_anal_stmt(env,_tmp599,_tmp602,0);
return Cyc_CfFlowInfo_join_flow(_tmp582,_tmp597,_tmp596);};}case 5: _LL4AA: _tmp606=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp584)->f1).f1;_tmp605=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp584)->f1).f2;_tmp604=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp584)->f2;_LL4AB: {
# 2668
struct _tuple20 _tmp59A=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp605);struct _tuple20 _tmp59B=_tmp59A;union Cyc_CfFlowInfo_FlowInfo*_tmp5A2;_LL4D5: _tmp5A2=_tmp59B.f2;_LL4D6:;{
union Cyc_CfFlowInfo_FlowInfo _tmp59C=*_tmp5A2;
struct _tuple19 _tmp59D=Cyc_NewControlFlow_anal_test(env,_tmp59C,_tmp606);struct _tuple19 _tmp59E=_tmp59D;union Cyc_CfFlowInfo_FlowInfo _tmp5A1;union Cyc_CfFlowInfo_FlowInfo _tmp5A0;_LL4D8: _tmp5A1=_tmp59E.f1;_tmp5A0=_tmp59E.f2;_LL4D9:;{
union Cyc_CfFlowInfo_FlowInfo _tmp59F=Cyc_NewControlFlow_anal_stmt(env,_tmp5A1,_tmp604,0);
Cyc_NewControlFlow_update_flow(env,_tmp605,_tmp59F);
return _tmp5A0;};};}case 14: _LL4AC: _tmp609=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp584)->f1;_tmp608=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp584)->f2).f1;_tmp607=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp584)->f2).f2;_LL4AD: {
# 2678
union Cyc_CfFlowInfo_FlowInfo _tmp5A3=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp609,0);
struct _tuple20 _tmp5A4=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5A3,_tmp607);struct _tuple20 _tmp5A5=_tmp5A4;union Cyc_CfFlowInfo_FlowInfo*_tmp5AB;_LL4DB: _tmp5AB=_tmp5A5.f2;_LL4DC:;{
union Cyc_CfFlowInfo_FlowInfo _tmp5A6=*_tmp5AB;
struct _tuple19 _tmp5A7=Cyc_NewControlFlow_anal_test(env,_tmp5A6,_tmp608);struct _tuple19 _tmp5A8=_tmp5A7;union Cyc_CfFlowInfo_FlowInfo _tmp5AA;union Cyc_CfFlowInfo_FlowInfo _tmp5A9;_LL4DE: _tmp5AA=_tmp5A8.f1;_tmp5A9=_tmp5A8.f2;_LL4DF:;
Cyc_NewControlFlow_update_flow(env,_tmp609,_tmp5AA);
return _tmp5A9;};}case 9: _LL4AE: _tmp60F=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp584)->f1;_tmp60E=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp584)->f2).f1;_tmp60D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp584)->f2).f2;_tmp60C=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp584)->f3).f1;_tmp60B=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp584)->f3).f2;_tmp60A=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp584)->f4;_LL4AF: {
# 2687
union Cyc_CfFlowInfo_FlowInfo _tmp5AC=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp60F)).f1;
struct _tuple20 _tmp5AD=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5AC,_tmp60D);struct _tuple20 _tmp5AE=_tmp5AD;union Cyc_CfFlowInfo_FlowInfo*_tmp5BA;_LL4E1: _tmp5BA=_tmp5AE.f2;_LL4E2:;{
union Cyc_CfFlowInfo_FlowInfo _tmp5AF=*_tmp5BA;
struct _tuple19 _tmp5B0=Cyc_NewControlFlow_anal_test(env,_tmp5AF,_tmp60E);struct _tuple19 _tmp5B1=_tmp5B0;union Cyc_CfFlowInfo_FlowInfo _tmp5B9;union Cyc_CfFlowInfo_FlowInfo _tmp5B8;_LL4E4: _tmp5B9=_tmp5B1.f1;_tmp5B8=_tmp5B1.f2;_LL4E5:;{
union Cyc_CfFlowInfo_FlowInfo _tmp5B2=Cyc_NewControlFlow_anal_stmt(env,_tmp5B9,_tmp60A,0);
struct _tuple20 _tmp5B3=Cyc_NewControlFlow_pre_stmt_check(env,_tmp5B2,_tmp60B);struct _tuple20 _tmp5B4=_tmp5B3;union Cyc_CfFlowInfo_FlowInfo*_tmp5B7;_LL4E7: _tmp5B7=_tmp5B4.f2;_LL4E8:;{
union Cyc_CfFlowInfo_FlowInfo _tmp5B5=*_tmp5B7;
union Cyc_CfFlowInfo_FlowInfo _tmp5B6=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp5B5,_tmp60C)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp60D,_tmp5B6);
return _tmp5B8;};};};}case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp584)->f2 != 0){_LL4B0: _tmp611=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp584)->f1;_tmp610=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp584)->f2;_LL4B1: {
# 2699
struct _RegionHandle _tmp5BB=_new_region("temp");struct _RegionHandle*temp=& _tmp5BB;_push_region(temp);
{struct _tuple23 _tmp5BC=Cyc_NewControlFlow_anal_Rexps(temp,env,inflow,_tmp611,1,1);struct _tuple23 _tmp5BD=_tmp5BC;union Cyc_CfFlowInfo_FlowInfo _tmp5C5;struct Cyc_List_List*_tmp5C4;_LL4EA: _tmp5C5=_tmp5BD.f1;_tmp5C4=_tmp5BD.f2;_LL4EB:;
# 2702
({union Cyc_CfFlowInfo_FlowInfo _tmp878=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,_tmp610->body);inflow=_tmp878;});{
# 2704
struct Cyc_List_List*_tmp5BE=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp610->pat_vars))->v)).f1);
({union Cyc_CfFlowInfo_FlowInfo _tmp879=Cyc_NewControlFlow_add_vars(_tmp582,_tmp5C5,_tmp5BE,_tmp582->unknown_all,s->loc,0);_tmp5C5=_tmp879;});
# 2707
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp610->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp5BF=(struct _tuple24*)x->hd;struct _tuple24*_tmp5C0=_tmp5BF;struct Cyc_Absyn_Vardecl**_tmp5C2;struct Cyc_Absyn_Exp*_tmp5C1;_LL4ED: _tmp5C2=_tmp5C0->f1;_tmp5C1=_tmp5C0->f2;_LL4EE:;
if(_tmp5C2 != 0){
({union Cyc_CfFlowInfo_FlowInfo _tmp880=({struct Cyc_CfFlowInfo_FlowEnv*_tmp87F=_tmp582;struct Cyc_NewControlFlow_AnalEnv*_tmp87E=env;union Cyc_CfFlowInfo_FlowInfo _tmp87D=_tmp5C5;struct Cyc_Absyn_Vardecl*_tmp87C=*_tmp5C2;struct Cyc_Absyn_Exp*_tmp87B=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp611))->hd;void*_tmp87A=(void*)((struct Cyc_List_List*)_check_null(_tmp5C4))->hd;Cyc_NewControlFlow_do_initialize_var(_tmp87F,_tmp87E,_tmp87D,_tmp87C,_tmp87B,_tmp87A,s->loc);});_tmp5C5=_tmp880;});
_tmp5C4=_tmp5C4->tl;
_tmp611=_tmp611->tl;}}}
# 2715
Cyc_NewControlFlow_update_flow(env,_tmp610->body,_tmp5C5);{
union Cyc_CfFlowInfo_FlowInfo _tmp5C3=Cyc_CfFlowInfo_BottomFL();_npop_handler(0);return _tmp5C3;};};}
# 2700
;_pop_region(temp);}}else{_LL4C4: _LL4C5:
# 2884
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));({struct Cyc_Core_Impossible_exn_struct _tmp882=({struct Cyc_Core_Impossible_exn_struct _tmp5FB;_tmp5FB.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp881=({const char*_tmp5FC="anal_stmt -- bad stmt syntax or unimplemented stmt form";_tag_dyneither(_tmp5FC,sizeof(char),56);});_tmp5FB.f1=_tmp881;});_tmp5FB;});_tmp5FA[0]=_tmp882;});_tmp5FA;}));}case 6: _LL4B2: _LL4B3:
# 2722
 if(((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->succ_table,s)== 0){
{union Cyc_CfFlowInfo_FlowInfo _tmp5C6=inflow;if((_tmp5C6.ReachableFL).tag == 2){_LL4F0: _LL4F1:
# 2725
{void*_tmp5C7=Cyc_Tcutil_compress(env->return_type);void*_tmp5C8=_tmp5C7;switch(*((int*)_tmp5C8)){case 0: _LL4F5: _LL4F6:
 goto _LL4F4;case 7: _LL4F7: _LL4F8:
 goto _LL4FA;case 6: _LL4F9: _LL4FA:
# 2729
({void*_tmp5C9=0;({unsigned int _tmp884=s->loc;struct _dyneither_ptr _tmp883=({const char*_tmp5CA="break may cause function not to return a value";_tag_dyneither(_tmp5CA,sizeof(char),47);});Cyc_Tcutil_warn(_tmp884,_tmp883,_tag_dyneither(_tmp5C9,sizeof(void*),0));});});
goto _LL4F4;default: _LL4FB: _LL4FC:
# 2732
({void*_tmp5CB=0;({unsigned int _tmp886=s->loc;struct _dyneither_ptr _tmp885=({const char*_tmp5CC="break may cause function not to return a value";_tag_dyneither(_tmp5CC,sizeof(char),47);});Cyc_Tcutil_terr(_tmp886,_tmp885,_tag_dyneither(_tmp5CB,sizeof(void*),0));});});
goto _LL4F4;}_LL4F4:;}
# 2735
goto _LL4EF;}else{_LL4F2: _LL4F3:
 goto _LL4EF;}_LL4EF:;}
# 2738
if(env->noreturn)
({void*_tmp5CD=0;({unsigned int _tmp888=s->loc;struct _dyneither_ptr _tmp887=({const char*_tmp5CE="`noreturn' function might return";_tag_dyneither(_tmp5CE,sizeof(char),33);});Cyc_CfFlowInfo_aerr(_tmp888,_tmp887,_tag_dyneither(_tmp5CD,sizeof(void*),0));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}
# 2744
goto _LL4B5;case 7: _LL4B4: _LL4B5:
 goto _LL4B7;case 8: _LL4B6: _LL4B7: {
# 2748
struct Cyc_Absyn_Stmt*_tmp5CF=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->succ_table,s);
if(_tmp5CF == 0)
({void*_tmp5D0=0;({unsigned int _tmp88A=s->loc;struct _dyneither_ptr _tmp889=({const char*_tmp5D1="jump has no target (should be impossible)";_tag_dyneither(_tmp5D1,sizeof(char),42);});Cyc_Tcutil_terr(_tmp88A,_tmp889,_tag_dyneither(_tmp5D0,sizeof(void*),0));});});
({union Cyc_CfFlowInfo_FlowInfo _tmp88B=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,(struct Cyc_Absyn_Stmt*)_check_null(_tmp5CF));inflow=_tmp88B;});
# 2753
Cyc_NewControlFlow_update_flow(env,_tmp5CF,inflow);
return Cyc_CfFlowInfo_BottomFL();}case 10: _LL4B8: _tmp614=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp584)->f1;_tmp613=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp584)->f2;_tmp612=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp584)->f3;_LL4B9:
# 2759
 return Cyc_NewControlFlow_anal_scs(env,inflow,_tmp613,_tmp614->loc);case 15: _LL4BA: _tmp617=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp584)->f1;_tmp616=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp584)->f2;_tmp615=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp584)->f3;_LL4BB: {
# 2764
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp617,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2772
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2775
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp616,0);
{union Cyc_CfFlowInfo_FlowInfo _tmp5D2=scs_outflow;if((_tmp5D2.BottomFL).tag == 1){_LL4FE: _LL4FF:
 goto _LL4FD;}else{_LL500: _LL501:
({void*_tmp5D3=0;({unsigned int _tmp88D=s->loc;struct _dyneither_ptr _tmp88C=({const char*_tmp5D4="last catch clause may implicitly fallthru";_tag_dyneither(_tmp5D4,sizeof(char),42);});Cyc_CfFlowInfo_aerr(_tmp88D,_tmp88C,_tag_dyneither(_tmp5D3,sizeof(void*),0));});});}_LL4FD:;}
# 2781
outflow=s1_outflow;
# 2783
return outflow;};};}case 12: switch(*((int*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp584)->f1)->r)){case 2: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp584)->f1)->r)->f2 != 0){_LL4BC: _tmp61B=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp584)->f1)->r)->f2)->v;_tmp61A=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp584)->f1)->r)->f3;_tmp619=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp584)->f1)->loc;_tmp618=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp584)->f2;_LL4BD: {
# 2793
struct _tuple1 _tmp5D5=Cyc_NewControlFlow_get_unconsume_pat_vars((env->fenv)->r,_tmp61B);struct _tuple1 _tmp5D6=_tmp5D5;struct Cyc_List_List*_tmp5DB;struct Cyc_List_List*_tmp5DA;_LL503: _tmp5DB=_tmp5D6.f1;_tmp5DA=_tmp5D6.f2;_LL504:;
({union Cyc_CfFlowInfo_FlowInfo _tmp88E=Cyc_NewControlFlow_initialize_pat_vars(_tmp582,env,inflow,_tmp61B,_tmp5DB != 0,_tmp619,_tmp61A->loc);inflow=_tmp88E;});{
struct Cyc_List_List*_tmp5D7=env->unique_pat_vars;
# 2797
({struct Cyc_List_List*_tmp890=({struct Cyc_List_List*_tmp5D8=_region_malloc(env->r,sizeof(*_tmp5D8));({struct _tuple29*_tmp88F=({struct _tuple29*_tmp5D9=_region_malloc(env->r,sizeof(*_tmp5D9));_tmp5D9->f1=0;_tmp5D9->f2=s;_tmp5D9->f3=_tmp5DB;_tmp5D9->f4=_tmp5DA;_tmp5D9;});_tmp5D8->hd=_tmp88F;});_tmp5D8->tl=_tmp5D7;_tmp5D8;});env->unique_pat_vars=_tmp890;});
# 2802
({union Cyc_CfFlowInfo_FlowInfo _tmp891=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp618,rval_opt);inflow=_tmp891;});
env->unique_pat_vars=_tmp5D7;
# 2806
({union Cyc_CfFlowInfo_FlowInfo _tmp892=Cyc_NewControlFlow_unconsume_params(env,_tmp5DB,_tmp5DA,0,inflow,_tmp619);inflow=_tmp892;});
# 2810
return inflow;};}}else{goto _LL4C0;}case 4: _LL4BE: _tmp61F=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp584)->f1)->r)->f2;_tmp61E=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp584)->f1)->r)->f3;_tmp61D=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp584)->f1)->loc;_tmp61C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp584)->f2;if(_tmp61E != 0){_LL4BF: {
# 2822
struct Cyc_List_List l=({struct Cyc_List_List _tmp5F9;_tmp5F9.hd=_tmp61E;_tmp5F9.tl=0;_tmp5F9;});
union Cyc_CfFlowInfo_FlowInfo _tmp5DC=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp5DD=Cyc_NewControlFlow_anal_Lexp(env,_tmp5DC,0,0,_tmp61E);struct _tuple18 _tmp5DE=_tmp5DD;union Cyc_CfFlowInfo_AbsLVal _tmp5F8;_LL506: _tmp5F8=_tmp5DE.f2;_LL507:;{
struct _tuple16 _tmp5DF=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp5DC,_tmp61E);struct _tuple16 _tmp5E0=_tmp5DF;union Cyc_CfFlowInfo_FlowInfo _tmp5F7;_LL509: _tmp5F7=_tmp5E0.f1;_LL50A:;{
struct Cyc_List_List*_tmp5E1=0;
struct Cyc_List_List*_tmp5E2=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp5E3=_tmp5F7;struct Cyc_Dict_Dict _tmp5F1;struct Cyc_List_List*_tmp5F0;if((_tmp5E3.ReachableFL).tag == 2){_LL50C: _tmp5F1=((_tmp5E3.ReachableFL).val).f1;_tmp5F0=((_tmp5E3.ReachableFL).val).f2;_LL50D:
# 2830
{union Cyc_CfFlowInfo_AbsLVal _tmp5E4=_tmp5F8;struct Cyc_CfFlowInfo_Place*_tmp5EF;if((_tmp5E4.PlaceL).tag == 1){_LL511: _tmp5EF=(_tmp5E4.PlaceL).val;_LL512: {
# 2834
void*_tmp5E5=Cyc_CfFlowInfo_lookup_place(_tmp5F1,_tmp5EF);
({void*_tmp894=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5E6=_region_malloc(_tmp582->r,sizeof(*_tmp5E6));({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp893=({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp5E7;_tmp5E7.tag=8;_tmp5E7.f1=_tmp61F;_tmp5E7.f2=_tmp5E5;_tmp5E7;});_tmp5E6[0]=_tmp893;});_tmp5E6;});_tmp5E5=_tmp894;});
({struct Cyc_Dict_Dict _tmp895=Cyc_CfFlowInfo_assign_place(_tmp582,_tmp61E->loc,_tmp5F1,_tmp5EF,_tmp5E5);_tmp5F1=_tmp895;});
({union Cyc_CfFlowInfo_FlowInfo _tmp896=Cyc_CfFlowInfo_ReachableFL(_tmp5F1,_tmp5F0);_tmp5F7=_tmp896;});{
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp5E8=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp5ED=_region_malloc(_tmp582->r,sizeof(*_tmp5ED));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp897=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp5EE;_tmp5EE.tag=0;_tmp5EE.f1=_tmp61F;_tmp5EE;});_tmp5ED[0]=_tmp897;});_tmp5ED;});
struct Cyc_CfFlowInfo_Place*_tmp5E9=({struct Cyc_CfFlowInfo_Place*_tmp5EC=_region_malloc(_tmp582->r,sizeof(*_tmp5EC));_tmp5EC->root=(void*)_tmp5E8;_tmp5EC->path=0;_tmp5EC;});
({struct Cyc_List_List*_tmp898=({struct Cyc_List_List*_tmp5EA=_region_malloc(_tmp582->r,sizeof(*_tmp5EA));_tmp5EA->hd=_tmp5E9;_tmp5EA->tl=_tmp5E1;_tmp5EA;});_tmp5E1=_tmp898;});
({struct Cyc_List_List*_tmp899=({struct Cyc_List_List*_tmp5EB=_region_malloc(_tmp582->r,sizeof(*_tmp5EB));_tmp5EB->hd=_tmp61E;_tmp5EB->tl=_tmp5E2;_tmp5EB;});_tmp5E2=_tmp899;});
goto _LL510;};}}else{_LL513: _LL514:
# 2849
 goto _LL510;}_LL510:;}
# 2851
goto _LL50B;}else{_LL50E: _LL50F:
# 2853
 goto _LL50B;}_LL50B:;}{
# 2856
struct Cyc_List_List _tmp5F2=({struct Cyc_List_List _tmp5F6;_tmp5F6.hd=_tmp61F;_tmp5F6.tl=0;_tmp5F6;});
({union Cyc_CfFlowInfo_FlowInfo _tmp89A=Cyc_NewControlFlow_add_vars(_tmp582,_tmp5F7,& _tmp5F2,_tmp582->unknown_all,_tmp61D,0);_tmp5F7=_tmp89A;});{
# 2860
struct Cyc_List_List*_tmp5F3=env->unique_pat_vars;
({struct Cyc_List_List*_tmp89C=({struct Cyc_List_List*_tmp5F4=_region_malloc(env->r,sizeof(*_tmp5F4));({struct _tuple29*_tmp89B=({struct _tuple29*_tmp5F5=_region_malloc(env->r,sizeof(*_tmp5F5));_tmp5F5->f1=1;_tmp5F5->f2=s;_tmp5F5->f3=_tmp5E1;_tmp5F5->f4=_tmp5E2;_tmp5F5;});_tmp5F4->hd=_tmp89B;});_tmp5F4->tl=_tmp5F3;_tmp5F4;});env->unique_pat_vars=_tmp89C;});
# 2866
({union Cyc_CfFlowInfo_FlowInfo _tmp89D=Cyc_NewControlFlow_anal_stmt(env,_tmp5F7,_tmp61C,rval_opt);_tmp5F7=_tmp89D;});
env->unique_pat_vars=_tmp5F3;
# 2870
({union Cyc_CfFlowInfo_FlowInfo _tmp89E=Cyc_NewControlFlow_unconsume_params(env,_tmp5E1,_tmp5E2,1,_tmp5F7,_tmp61D);_tmp5F7=_tmp89E;});
# 2874
return _tmp5F7;};};};};}}else{goto _LL4C0;}default: _LL4C0: _tmp621=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp584)->f1;_tmp620=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp584)->f2;_LL4C1:
# 2877
({union Cyc_CfFlowInfo_FlowInfo _tmp8A2=({struct Cyc_NewControlFlow_AnalEnv*_tmp8A1=env;union Cyc_CfFlowInfo_FlowInfo _tmp8A0=Cyc_NewControlFlow_anal_decl(env,inflow,_tmp621);struct Cyc_Absyn_Stmt*_tmp89F=_tmp620;Cyc_NewControlFlow_anal_stmt(_tmp8A1,_tmp8A0,_tmp89F,rval_opt);});outflow=_tmp8A2;});
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp621);
return outflow;}default: _LL4C2: _tmp622=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp584)->f2;_LL4C3:
# 2882
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp622,rval_opt);}_LL49D:;};}
# 2889
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2893
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp625=env->fenv;
void*_tmp626=decl->r;void*_tmp627=_tmp626;struct Cyc_Absyn_Tvar*_tmp63E;struct Cyc_Absyn_Vardecl*_tmp63D;struct Cyc_Absyn_Exp*_tmp63C;struct Cyc_Absyn_Fndecl*_tmp63B;struct Cyc_List_List*_tmp63A;struct Cyc_Absyn_Vardecl*_tmp639;switch(*((int*)_tmp627)){case 0: _LL516: _tmp639=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp627)->f1;_LL517: {
# 2903
struct Cyc_List_List _tmp628=({struct Cyc_List_List _tmp62E;_tmp62E.hd=_tmp639;_tmp62E.tl=0;_tmp62E;});
({union Cyc_CfFlowInfo_FlowInfo _tmp8A3=Cyc_NewControlFlow_add_vars(_tmp625,inflow,& _tmp628,_tmp625->unknown_none,decl->loc,0);inflow=_tmp8A3;});{
struct Cyc_Absyn_Exp*_tmp629=_tmp639->initializer;
if(_tmp629 == 0)
return inflow;{
struct _tuple16 _tmp62A=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp629);struct _tuple16 _tmp62B=_tmp62A;union Cyc_CfFlowInfo_FlowInfo _tmp62D;void*_tmp62C;_LL521: _tmp62D=_tmp62B.f1;_tmp62C=_tmp62B.f2;_LL522:;
return Cyc_NewControlFlow_do_initialize_var(_tmp625,env,_tmp62D,_tmp639,_tmp629,_tmp62C,decl->loc);};};}case 3: _LL518: _tmp63A=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp627)->f1;_LL519:
# 2912
 return Cyc_NewControlFlow_add_vars(_tmp625,inflow,_tmp63A,_tmp625->unknown_none,decl->loc,0);case 1: _LL51A: _tmp63B=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp627)->f1;_LL51B:
# 2915
 Cyc_NewControlFlow_check_nested_fun(env->all_tables,_tmp625,inflow,_tmp63B);{
void*t=(void*)_check_null(_tmp63B->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp62F=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp63B->fn_vardecl);
# 2921
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp8A7=_tmp625;union Cyc_CfFlowInfo_FlowInfo _tmp8A6=inflow;struct Cyc_List_List*_tmp8A5=({struct Cyc_List_List*_tmp630=_region_malloc(env->r,sizeof(*_tmp630));_tmp630->hd=_tmp62F;_tmp630->tl=0;_tmp630;});void*_tmp8A4=_tmp625->unknown_all;Cyc_NewControlFlow_add_vars(_tmp8A7,_tmp8A6,_tmp8A5,_tmp8A4,decl->loc,0);});};case 4: _LL51C: _tmp63E=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp627)->f1;_tmp63D=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp627)->f2;_tmp63C=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp627)->f3;_LL51D:
# 2924
 if(_tmp63C != 0)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp631=_cycalloc(sizeof(*_tmp631));({struct Cyc_Core_Impossible_exn_struct _tmp8A9=({struct Cyc_Core_Impossible_exn_struct _tmp632;_tmp632.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp8A8=({const char*_tmp633="found open expression in declaration!";_tag_dyneither(_tmp633,sizeof(char),38);});_tmp632.f1=_tmp8A8;});_tmp632;});_tmp631[0]=_tmp8A9;});_tmp631;}));{
struct Cyc_List_List _tmp634=({struct Cyc_List_List _tmp635;_tmp635.hd=_tmp63D;_tmp635.tl=0;_tmp635;});
return Cyc_NewControlFlow_add_vars(_tmp625,inflow,& _tmp634,_tmp625->unknown_all,decl->loc,0);};default: _LL51E: _LL51F:
# 2930
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp636=_cycalloc(sizeof(*_tmp636));({struct Cyc_Core_Impossible_exn_struct _tmp8AB=({struct Cyc_Core_Impossible_exn_struct _tmp637;_tmp637.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp8AA=({const char*_tmp638="anal_decl: unexpected decl variant";_tag_dyneither(_tmp638,sizeof(char),35);});_tmp637.f1=_tmp8AA;});_tmp637;});_tmp636[0]=_tmp8AB;});_tmp636;}));}_LL515:;}
# 2938
static void Cyc_NewControlFlow_check_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd){
struct _handler_cons _tmp63F;_push_handler(& _tmp63F);{int _tmp641=0;if(setjmp(_tmp63F.handler))_tmp641=1;if(!_tmp641){
{struct _RegionHandle _tmp642=_new_region("frgn");struct _RegionHandle*frgn=& _tmp642;_push_region(frgn);
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(frgn);
({struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmp8AE=tables;struct Cyc_CfFlowInfo_FlowEnv*_tmp8AD=fenv;union Cyc_CfFlowInfo_FlowInfo _tmp8AC=Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0);Cyc_NewControlFlow_check_nested_fun(_tmp8AE,_tmp8AD,_tmp8AC,fd);});}
# 2941
;_pop_region(frgn);}
# 2940
;_pop_handler();}else{void*_tmp640=(void*)_exn_thrown;void*_tmp643=_tmp640;void*_tmp646;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp643)->tag == Cyc_Dict_Absent){_LL524: _LL525:
# 2947
 if(Cyc_Position_num_errors > 0)
goto _LL523;else{
Cyc_Core_rethrow((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp644=_cycalloc_atomic(sizeof(*_tmp644));({struct Cyc_Dict_Absent_exn_struct _tmp8AF=({struct Cyc_Dict_Absent_exn_struct _tmp645;_tmp645.tag=Cyc_Dict_Absent;_tmp645;});_tmp644[0]=_tmp8AF;});_tmp644;}));}}else{_LL526: _tmp646=_tmp643;_LL527:(int)_rethrow(_tmp646);}_LL523:;}};}
# 2953
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2958
static union Cyc_Relations_RelnOp Cyc_NewControlFlow_translate_rop(struct Cyc_List_List*vds,union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp647=r;unsigned int _tmp649;if((_tmp647.RParam).tag == 5){_LL529: _tmp649=(_tmp647.RParam).val;_LL52A: {
# 2961
struct Cyc_Absyn_Vardecl*_tmp648=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(vds,(int)_tmp649);
if(!_tmp648->escapes)
return Cyc_Relations_RVar(_tmp648);
return r;}}else{_LL52B: _LL52C:
 return r;}_LL528:;}
# 2969
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2973
struct _RegionHandle*_tmp64A=fenv->r;
unsigned int _tmp64B=(fd->body)->loc;
({union Cyc_CfFlowInfo_FlowInfo _tmp8B0=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp64B,1);inflow=_tmp8B0;});{
# 2979
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_roots=0;
struct _tuple14 _tmp64C=({union Cyc_CfFlowInfo_FlowInfo _tmp684=inflow;if((_tmp684.ReachableFL).tag != 2)_throw_match();(_tmp684.ReachableFL).val;});struct _tuple14 _tmp64D=_tmp64C;struct Cyc_Dict_Dict _tmp683;struct Cyc_List_List*_tmp682;_LL52E: _tmp683=_tmp64D.f1;_tmp682=_tmp64D.f2;_LL52F:;{
# 2984
struct Cyc_List_List*_tmp64E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;
{struct Cyc_List_List*_tmp64F=fd->requires_relns;for(0;_tmp64F != 0;_tmp64F=_tmp64F->tl){
struct Cyc_Relations_Reln*_tmp650=(struct Cyc_Relations_Reln*)_tmp64F->hd;
({struct Cyc_List_List*_tmp8B5=({struct _RegionHandle*_tmp8B4=_tmp64A;union Cyc_Relations_RelnOp _tmp8B3=Cyc_NewControlFlow_translate_rop(_tmp64E,_tmp650->rop1);enum Cyc_Relations_Relation _tmp8B2=_tmp650->relation;union Cyc_Relations_RelnOp _tmp8B1=
Cyc_NewControlFlow_translate_rop(_tmp64E,_tmp650->rop2);
# 2987
Cyc_Relations_add_relation(_tmp8B4,_tmp8B3,_tmp8B2,_tmp8B1,_tmp682);});_tmp682=_tmp8B5;});}}{
# 2993
struct Cyc_List_List*atts;
{void*_tmp651=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));void*_tmp652=_tmp651;struct Cyc_List_List*_tmp655;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp652)->tag == 9){_LL531: _tmp655=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp652)->f1).attributes;_LL532:
 atts=_tmp655;goto _LL530;}else{_LL533: _LL534:
({void*_tmp653=0;({struct _dyneither_ptr _tmp8B6=({const char*_tmp654="check_fun: non-function type cached with fndecl_t";_tag_dyneither(_tmp654,sizeof(char),50);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp8B6,_tag_dyneither(_tmp653,sizeof(void*),0));});});}_LL530:;}
# 3000
for(0;atts != 0;atts=atts->tl){
void*_tmp656=(void*)atts->hd;void*_tmp657=_tmp656;int _tmp677;int _tmp676;int _tmp675;switch(*((int*)_tmp657)){case 21: _LL536: _tmp675=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp657)->f1;_LL537: {
# 3003
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
struct Cyc_Absyn_Vardecl*_tmp658=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp675 - 1);
void*t=Cyc_Tcutil_compress(_tmp658->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp659=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp8B9=fenv;void*_tmp8B8=elttype;struct Cyc_Absyn_Exp*_tmp8B7=bogus_exp;Cyc_CfFlowInfo_make_unique_consumed(_tmp8B9,_tmp8B8,_tmp8B7,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));});
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp65A=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp661=_region_malloc(_tmp64A,sizeof(*_tmp661));({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp8BA=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp662;_tmp662.tag=2;_tmp662.f1=_tmp675;_tmp662.f2=t;_tmp662;});_tmp661[0]=_tmp8BA;});_tmp661;});
struct Cyc_CfFlowInfo_Place*_tmp65B=({struct Cyc_CfFlowInfo_Place*_tmp660=_region_malloc(_tmp64A,sizeof(*_tmp660));_tmp660->root=(void*)_tmp65A;_tmp660->path=0;_tmp660;});
({struct Cyc_Dict_Dict _tmp8BB=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp683,(void*)_tmp65A,_tmp659);_tmp683=_tmp8BB;});
({struct Cyc_Dict_Dict _tmp8C0=({struct Cyc_Dict_Dict _tmp8BF=_tmp683;void*_tmp8BE=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp65C=_region_malloc(_tmp64A,sizeof(*_tmp65C));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8BC=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp65D;_tmp65D.tag=0;_tmp65D.f1=_tmp658;_tmp65D;});_tmp65C[0]=_tmp8BC;});_tmp65C;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp8BF,_tmp8BE,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp65E=_region_malloc(_tmp64A,sizeof(*_tmp65E));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp8BD=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp65F;_tmp65F.tag=4;_tmp65F.f1=_tmp65B;_tmp65F;});_tmp65E[0]=_tmp8BD;});_tmp65E;}));});_tmp683=_tmp8C0;});
goto _LL535;}case 20: _LL538: _tmp676=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp657)->f1;_LL539: {
# 3016
struct Cyc_Absyn_Vardecl*_tmp663=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp676 - 1);
void*elttype=Cyc_Tcutil_pointer_elt_type(_tmp663->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp664=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp66C=_region_malloc(_tmp64A,sizeof(*_tmp66C));({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp8C1=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp66D;_tmp66D.tag=2;_tmp66D.f1=_tmp676;_tmp66D.f2=elttype;_tmp66D;});_tmp66C[0]=_tmp8C1;});_tmp66C;});
struct Cyc_CfFlowInfo_Place*_tmp665=({struct Cyc_CfFlowInfo_Place*_tmp66B=_region_malloc(_tmp64A,sizeof(*_tmp66B));_tmp66B->root=(void*)_tmp664;_tmp66B->path=0;_tmp66B;});
({struct Cyc_Dict_Dict _tmp8C4=({struct Cyc_Dict_Dict _tmp8C3=_tmp683;void*_tmp8C2=(void*)_tmp664;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp8C3,_tmp8C2,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));});_tmp683=_tmp8C4;});
({struct Cyc_Dict_Dict _tmp8C9=({struct Cyc_Dict_Dict _tmp8C8=_tmp683;void*_tmp8C7=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp666=_region_malloc(_tmp64A,sizeof(*_tmp666));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8C5=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp667;_tmp667.tag=0;_tmp667.f1=_tmp663;_tmp667;});_tmp666[0]=_tmp8C5;});_tmp666;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp8C8,_tmp8C7,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp668=_region_malloc(_tmp64A,sizeof(*_tmp668));({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp8C6=({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp669;_tmp669.tag=4;_tmp669.f1=_tmp665;_tmp669;});_tmp668[0]=_tmp8C6;});_tmp668;}));});_tmp683=_tmp8C9;});
({struct Cyc_List_List*_tmp8CA=({struct Cyc_List_List*_tmp66A=_region_malloc(_tmp64A,sizeof(*_tmp66A));_tmp66A->hd=_tmp665;_tmp66A->tl=param_roots;_tmp66A;});param_roots=_tmp8CA;});
goto _LL535;}case 22: _LL53A: _tmp677=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp657)->f1;_LL53B: {
# 3025
struct Cyc_Absyn_Vardecl*_tmp66E=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp677 - 1);
if(Cyc_Tcutil_is_noalias_pointer(_tmp66E->type,0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp66F=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp673=_region_malloc(_tmp64A,sizeof(*_tmp673));({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8CB=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp674;_tmp674.tag=0;_tmp674.f1=_tmp66E;_tmp674;});_tmp673[0]=_tmp8CB;});_tmp673;});
struct Cyc_CfFlowInfo_Place*_tmp670=({struct Cyc_CfFlowInfo_Place*_tmp672=_region_malloc(_tmp64A,sizeof(*_tmp672));_tmp672->root=(void*)_tmp66F;_tmp672->path=0;_tmp672;});
({struct Cyc_List_List*_tmp8CC=({struct Cyc_List_List*_tmp671=_region_malloc(_tmp64A,sizeof(*_tmp671));_tmp671->hd=_tmp670;_tmp671->tl=noconsume_roots;_tmp671;});noconsume_roots=_tmp8CC;});}
# 3031
goto _LL535;}default: _LL53C: _LL53D:
 goto _LL535;}_LL535:;}
# 3034
({union Cyc_CfFlowInfo_FlowInfo _tmp8CD=Cyc_CfFlowInfo_ReachableFL(_tmp683,_tmp682);inflow=_tmp8CD;});{
# 3036
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp64A,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*_tmp681=_region_malloc(_tmp64A,sizeof(*_tmp681));_tmp681->all_tables=tables;({struct Cyc_Hashtable_Table*_tmp8CE=
# 3041
((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->succ_tables,fd);_tmp681->succ_table=_tmp8CE;});({struct Cyc_Hashtable_Table*_tmp8CF=
((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->pat_pop_tables,fd);_tmp681->pat_pop_table=_tmp8CF;});_tmp681->r=_tmp64A;_tmp681->fenv=fenv;_tmp681->iterate_again=1;_tmp681->iteration_num=0;_tmp681->in_try=0;_tmp681->tryflow=inflow;_tmp681->noreturn=noreturn;_tmp681->return_type=fd->ret_type;_tmp681->unique_pat_vars=0;_tmp681->param_roots=param_roots;_tmp681->noconsume_params=noconsume_roots;_tmp681->flow_table=flow_table;_tmp681->return_relns=fd->ensures_relns;_tmp681;});
# 3046
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 3052
env->iterate_again=0;
({union Cyc_CfFlowInfo_FlowInfo _tmp8D0=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);outflow=_tmp8D0;});}{
# 3055
union Cyc_CfFlowInfo_FlowInfo _tmp678=outflow;if((_tmp678.BottomFL).tag == 1){_LL53F: _LL540:
 goto _LL53E;}else{_LL541: _LL542:
# 3058
 Cyc_NewControlFlow_check_init_params(_tmp64B,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,outflow,_tmp64B);
# 3062
if(noreturn)
({void*_tmp679=0;({unsigned int _tmp8D2=_tmp64B;struct _dyneither_ptr _tmp8D1=({const char*_tmp67A="`noreturn' function might (implicitly) return";_tag_dyneither(_tmp67A,sizeof(char),46);});Cyc_CfFlowInfo_aerr(_tmp8D2,_tmp8D1,_tag_dyneither(_tmp679,sizeof(void*),0));});});else{
# 3065
void*_tmp67B=Cyc_Tcutil_compress(fd->ret_type);void*_tmp67C=_tmp67B;switch(*((int*)_tmp67C)){case 0: _LL544: _LL545:
 goto _LL543;case 7: _LL546: _LL547:
 goto _LL549;case 6: _LL548: _LL549:
# 3069
({void*_tmp67D=0;({unsigned int _tmp8D4=_tmp64B;struct _dyneither_ptr _tmp8D3=({const char*_tmp67E="function may not return a value";_tag_dyneither(_tmp67E,sizeof(char),32);});Cyc_Tcutil_warn(_tmp8D4,_tmp8D3,_tag_dyneither(_tmp67D,sizeof(void*),0));});});goto _LL543;default: _LL54A: _LL54B:
# 3071
({void*_tmp67F=0;({unsigned int _tmp8D6=_tmp64B;struct _dyneither_ptr _tmp8D5=({const char*_tmp680="function may not return a value";_tag_dyneither(_tmp680,sizeof(char),32);});Cyc_CfFlowInfo_aerr(_tmp8D6,_tmp8D5,_tag_dyneither(_tmp67F,sizeof(void*),0));});});goto _LL543;}_LL543:;}
# 3073
goto _LL53E;}_LL53E:;};};};};};}
# 3077
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_tmp685=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_tmp686=_tmp685;struct Cyc_List_List*_tmp68A;struct Cyc_List_List*_tmp689;struct Cyc_List_List*_tmp688;struct Cyc_Absyn_Fndecl*_tmp687;switch(*((int*)_tmp686)){case 1: _LL54D: _tmp687=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp686)->f1;_LL54E:
 Cyc_NewControlFlow_check_fun(tables,_tmp687);goto _LL54C;case 11: _LL54F: _tmp688=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp686)->f1;_LL550:
 _tmp689=_tmp688;goto _LL552;case 10: _LL551: _tmp689=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp686)->f2;_LL552:
 _tmp68A=_tmp689;goto _LL554;case 9: _LL553: _tmp68A=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp686)->f2;_LL554:
 Cyc_NewControlFlow_cf_check(tables,_tmp68A);goto _LL54C;case 12: _LL555: _LL556:
 goto _LL54C;default: _LL557: _LL558:
 goto _LL54C;}_LL54C:;};}}
