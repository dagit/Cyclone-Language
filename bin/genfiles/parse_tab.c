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
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
typedef unsigned int Cyc_Core___cyclone_internal_singleton;extern char Cyc_Lexing_Error[6U];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 57 "lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
# 80
struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 153
void*Cyc_List_fold_right(void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 52
int Cyc_zstrcmp(struct _dyneither_ptr,struct _dyneither_ptr);
# 54
int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
# 60
struct _dyneither_ptr Cyc_strcat(struct _dyneither_ptr dest,struct _dyneither_ptr src);
# 71
struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,struct _dyneither_ptr src);
# 108 "string.h"
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
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
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Relations_Reln;
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 221
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 315
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 332
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 346
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 367
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 372
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 427 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 443
typedef void*Cyc_Absyn_funcparams_t;
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};
# 482
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
extern struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val;
extern struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val;
# 489
extern struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct Cyc_Absyn_Shared_att_val;
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val;
extern struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct Cyc_Absyn_Weak_att_val;
extern struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct Cyc_Absyn_Dllimport_att_val;
extern struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct Cyc_Absyn_Dllexport_att_val;
extern struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct Cyc_Absyn_No_instrument_function_att_val;
extern struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct Cyc_Absyn_Constructor_att_val;
extern struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct Cyc_Absyn_Destructor_att_val;
extern struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct Cyc_Absyn_No_check_memory_usage_att_val;
extern struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct Cyc_Absyn_Pure_att_val;
extern struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct Cyc_Absyn_Always_inline_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 510
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 724 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};
# 894
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 921
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 925
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 929
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 939
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 944
void*Cyc_Absyn_compress_kb(void*);
# 949
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 951
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 954
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 956
extern void*Cyc_Absyn_sint_typ;
# 958
void*Cyc_Absyn_float_typ(int);
# 1000
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
# 1007
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int);
# 1022
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _dyneither_ptr s,unsigned int loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1031
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1050
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1052
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1068
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1076
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1081
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc);
# 1094
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
# 1097
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1100
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1104
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*open_exp,unsigned int);
# 1109
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1111
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1118
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1127
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1132
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1138
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*requires_clause,struct Cyc_Absyn_Exp*ensures_clause);
# 1163
int Cyc_Absyn_fntype_att(void*a);
# 1178
extern int Cyc_Absyn_porting_c_code;struct Cyc_RgnOrder_RgnPO;
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
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 48 "tcenv.h"
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 52
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;};
# 62
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 84
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 42 "tcutil.h"
void*Cyc_Tcutil_copy_type(void*t);
# 55
void*Cyc_Tcutil_compress(void*t);
# 94
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 101
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 106
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 111
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 113
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 115
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
# 118
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 128
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 290 "tcutil.h"
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 354
int Cyc_Tcutil_is_array(void*t);
# 358
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 66 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst);
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 69 "parse.y"
void Cyc_Lex_register_typedef(struct _tuple0*s);
void Cyc_Lex_enter_namespace(struct _dyneither_ptr*);
void Cyc_Lex_leave_namespace();
void Cyc_Lex_enter_using(struct _tuple0*);
void Cyc_Lex_leave_using();
void Cyc_Lex_enter_extern_c();
void Cyc_Lex_leave_extern_c();struct Cyc_Parse_FlatList{struct Cyc_Parse_FlatList*tl;void*hd[0U] __attribute__((aligned )) ;};
# 92 "parse.y"
typedef struct Cyc_Parse_FlatList*Cyc_Parse_flat_list_t;
# 94
struct Cyc_Parse_FlatList*Cyc_Parse_flat_imp_rev(struct Cyc_Parse_FlatList*x){
if(x == 0)return x;else{
# 97
struct Cyc_Parse_FlatList*first=x;
struct Cyc_Parse_FlatList*second=x->tl;
x->tl=0;
while(second != 0){
struct Cyc_Parse_FlatList*temp=second->tl;
second->tl=first;
first=second;
second=temp;}
# 106
return first;}}
# 111
int Cyc_Parse_no_register=0;char Cyc_Parse_Exit[5U]="Exit";struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_Parse_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 126
typedef struct Cyc_Parse_Type_specifier Cyc_Parse_type_specifier_t;
# 128
enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc  = 0U,Cyc_Parse_Extern_sc  = 1U,Cyc_Parse_ExternC_sc  = 2U,Cyc_Parse_Static_sc  = 3U,Cyc_Parse_Auto_sc  = 4U,Cyc_Parse_Register_sc  = 5U,Cyc_Parse_Abstract_sc  = 6U};
# 131
typedef enum Cyc_Parse_Storage_class Cyc_Parse_storage_class_t;struct Cyc_Parse_Declaration_spec{enum Cyc_Parse_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Parse_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};
# 140
typedef struct Cyc_Parse_Declaration_spec Cyc_Parse_decl_spec_t;struct Cyc_Parse_Declarator{struct _tuple0*id;unsigned int varloc;struct Cyc_List_List*tms;};
# 147
typedef struct Cyc_Parse_Declarator Cyc_Parse_declarator_t;struct _tuple10{struct _tuple10*tl;struct Cyc_Parse_Declarator hd  __attribute__((aligned )) ;};
typedef struct _tuple10*Cyc_Parse_declarators_t;struct _tuple11{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple12{struct _tuple12*tl;struct _tuple11 hd  __attribute__((aligned )) ;};
typedef struct _tuple12*Cyc_Parse_declarator_list_t;struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct{int tag;};
# 161
typedef void*Cyc_Parse_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Parse_pointer_quals_t;
# 164
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple12*ds,struct _tuple10**decls,struct Cyc_List_List**es){
# 168
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
for(0;ds != 0;ds=ds->tl){
struct _tuple11 _tmp0=ds->hd;struct _tuple11 _tmp1=_tmp0;struct Cyc_Parse_Declarator _tmp5;struct Cyc_Absyn_Exp*_tmp4;_LL1: _tmp5=_tmp1.f1;_tmp4=_tmp1.f2;_LL2:;
({struct _tuple10*_tmp858=({struct _tuple10*_tmp2=_region_malloc(r,sizeof(*_tmp2));_tmp2->tl=declarators;_tmp2->hd=_tmp5;_tmp2;});declarators=_tmp858;});
({struct Cyc_List_List*_tmp859=({struct Cyc_List_List*_tmp3=_region_malloc(r,sizeof(*_tmp3));_tmp3->hd=_tmp4;_tmp3->tl=exprs;_tmp3;});exprs=_tmp859;});}
# 175
({struct Cyc_List_List*_tmp85A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(exprs);*es=_tmp85A;});
({struct _tuple10*_tmp85B=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(declarators);*decls=_tmp85B;});}struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
# 182
typedef struct Cyc_Parse_Abstractdeclarator Cyc_Parse_abstractdeclarator_t;
# 185
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc);struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 187
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
# 192
static struct Cyc_List_List*Cyc_Parse_parse_result=0;
# 194
static void*Cyc_Parse_parse_abort(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 196
Cyc_Warn_verr(loc,fmt,ap);
(int)_throw((void*)({struct Cyc_Parse_Exit_exn_struct*_tmp6=_cycalloc_atomic(sizeof(*_tmp6));({struct Cyc_Parse_Exit_exn_struct _tmp85C=({struct Cyc_Parse_Exit_exn_struct _tmp7;_tmp7.tag=Cyc_Parse_Exit;_tmp7;});_tmp6[0]=_tmp85C;});_tmp6;}));}
# 200
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 202
struct _tuple8*_tmp8=tqt;struct Cyc_Absyn_Tqual _tmpC;void*_tmpB;_LL4: _tmpC=_tmp8->f2;_tmpB=_tmp8->f3;_LL5:;
if((_tmpC.print_const  || _tmpC.q_volatile) || _tmpC.q_restrict){
if(_tmpC.loc != 0)loc=_tmpC.loc;
({void*_tmp9=0U;({unsigned int _tmp85E=loc;struct _dyneither_ptr _tmp85D=({const char*_tmpA="qualifier on type is ignored";_tag_dyneither(_tmpA,sizeof(char),29U);});Cyc_Warn_warn(_tmp85E,_tmp85D,_tag_dyneither(_tmp9,sizeof(void*),0U));});});}
# 207
return _tmpB;}struct _tuple14{union Cyc_Absyn_Constraint*f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;void*f4;};
# 210
static struct _tuple14 Cyc_Parse_collapse_pointer_quals(unsigned int loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*bound,void*rgn,struct Cyc_List_List*pqs){
# 216
union Cyc_Absyn_Constraint*zeroterm=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
for(0;pqs != 0;pqs=pqs->tl){
void*_tmpD=(void*)pqs->hd;void*_tmpE=_tmpD;void*_tmp12;struct Cyc_Absyn_Exp*_tmp11;switch(*((int*)_tmpE)){case 4U: _LL7: _LL8:
 zeroterm=Cyc_Absyn_true_conref;goto _LL6;case 5U: _LL9: _LLA:
 zeroterm=Cyc_Absyn_false_conref;goto _LL6;case 7U: _LLB: _LLC:
 nullable=Cyc_Absyn_true_conref;goto _LL6;case 6U: _LLD: _LLE:
 nullable=Cyc_Absyn_false_conref;goto _LL6;case 3U: _LLF: _LL10:
 bound=Cyc_Absyn_bounds_dyneither_conref;goto _LL6;case 2U: _LL11: _LL12:
 bound=Cyc_Absyn_bounds_one_conref;goto _LL6;case 0U: _LL13: _tmp11=((struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*)_tmpE)->f1;_LL14:
({union Cyc_Absyn_Constraint*_tmp860=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpF=_cycalloc(sizeof(*_tmpF));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp85F=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp10;_tmp10.tag=1U;_tmp10.f1=_tmp11;_tmp10;});_tmpF[0]=_tmp85F;});_tmpF;}));bound=_tmp860;});goto _LL6;default: _LL15: _tmp12=(void*)((struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*)_tmpE)->f1;_LL16:
 rgn=_tmp12;goto _LL6;}_LL6:;}
# 228
return({struct _tuple14 _tmp13;_tmp13.f1=nullable;_tmp13.f2=bound;_tmp13.f3=zeroterm;_tmp13.f4=rgn;_tmp13;});}
# 234
struct _tuple0*Cyc_Parse_gensym_enum(){
# 236
static int enum_counter=0;
return({struct _tuple0*_tmp14=_cycalloc(sizeof(*_tmp14));({union Cyc_Absyn_Nmspace _tmp861=Cyc_Absyn_Rel_n(0);_tmp14->f1=_tmp861;});({struct _dyneither_ptr*_tmp864=({struct _dyneither_ptr*_tmp15=_cycalloc(sizeof(*_tmp15));({struct _dyneither_ptr _tmp863=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp18;_tmp18.tag=1U;_tmp18.f1=(unsigned long)enum_counter ++;({void*_tmp16[1U]={& _tmp18};({struct _dyneither_ptr _tmp862=({const char*_tmp17="__anonymous_enum_%d__";_tag_dyneither(_tmp17,sizeof(char),22U);});Cyc_aprintf(_tmp862,_tag_dyneither(_tmp16,sizeof(void*),1U));});});});_tmp15[0]=_tmp863;});_tmp15;});_tmp14->f2=_tmp864;});_tmp14;});}struct _tuple15{unsigned int f1;struct _tuple0*f2;struct Cyc_Absyn_Tqual f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 241
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple17*field_info){
# 246
struct _tuple17*_tmp19=field_info;unsigned int _tmp26;struct _tuple0*_tmp25;struct Cyc_Absyn_Tqual _tmp24;void*_tmp23;struct Cyc_List_List*_tmp22;struct Cyc_List_List*_tmp21;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_Absyn_Exp*_tmp1F;_LL18: _tmp26=(_tmp19->f1)->f1;_tmp25=(_tmp19->f1)->f2;_tmp24=(_tmp19->f1)->f3;_tmp23=(_tmp19->f1)->f4;_tmp22=(_tmp19->f1)->f5;_tmp21=(_tmp19->f1)->f6;_tmp20=(_tmp19->f2)->f1;_tmp1F=(_tmp19->f2)->f2;_LL19:;
if(_tmp22 != 0)
({void*_tmp1A=0U;({unsigned int _tmp866=loc;struct _dyneither_ptr _tmp865=({const char*_tmp1B="bad type params in struct field";_tag_dyneither(_tmp1B,sizeof(char),32U);});Cyc_Warn_err(_tmp866,_tmp865,_tag_dyneither(_tmp1A,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp25))
({void*_tmp1C=0U;({unsigned int _tmp868=loc;struct _dyneither_ptr _tmp867=({const char*_tmp1D="struct or union field cannot be qualified with a namespace";_tag_dyneither(_tmp1D,sizeof(char),59U);});Cyc_Warn_err(_tmp868,_tmp867,_tag_dyneither(_tmp1C,sizeof(void*),0U));});});
return({struct Cyc_Absyn_Aggrfield*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->name=(*_tmp25).f2;_tmp1E->tq=_tmp24;_tmp1E->type=_tmp23;_tmp1E->width=_tmp20;_tmp1E->attributes=_tmp21;_tmp1E->requires_clause=_tmp1F;_tmp1E;});}
# 256
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
return({struct Cyc_Parse_Type_specifier _tmp27;_tmp27.Signed_spec=0;_tmp27.Unsigned_spec=0;_tmp27.Short_spec=0;_tmp27.Long_spec=0;_tmp27.Long_Long_spec=0;_tmp27.Valid_type_spec=0;_tmp27.Type_spec=Cyc_Absyn_sint_typ;_tmp27.loc=loc;_tmp27;});}
# 267
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp28=Cyc_Parse_empty_spec(loc);
_tmp28.Type_spec=t;
_tmp28.Valid_type_spec=1;
return _tmp28;}
# 273
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp29=Cyc_Parse_empty_spec(loc);
_tmp29.Signed_spec=1;
return _tmp29;}
# 278
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp2A=Cyc_Parse_empty_spec(loc);
_tmp2A.Unsigned_spec=1;
return _tmp2A;}
# 283
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp2B=Cyc_Parse_empty_spec(loc);
_tmp2B.Short_spec=1;
return _tmp2B;}
# 288
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp2C=Cyc_Parse_empty_spec(loc);
_tmp2C.Long_spec=1;
return _tmp2C;}
# 295
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 297
return Cyc_Tcutil_is_array(t)?({
void*_tmp869=t;Cyc_Tcutil_promote_array(_tmp869,argposn?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,0);}): t;}struct _tuple18{struct _dyneither_ptr*f1;void*f2;};
# 310 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp2D=0;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp2E=(struct _tuple8*)x->hd;struct _tuple8*_tmp2F=_tmp2E;struct _dyneither_ptr _tmp49;void**_tmp48;struct _dyneither_ptr*_tmp47;void*_tmp46;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)((struct _tuple8*)_tmp2F)->f3)->tag == 19U){_LL1B: _tmp47=_tmp2F->f1;_tmp46=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2F->f3)->f1;if(_tmp47 != 0){_LL1C:
# 315
{void*_tmp30=_tmp46;void**_tmp3A;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp30)->tag == 1U){_LL22: _tmp3A=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp30)->f2;_LL23: {
# 319
struct _dyneither_ptr*nm=({struct _dyneither_ptr*_tmp36=_cycalloc(sizeof(*_tmp36));({struct _dyneither_ptr _tmp86B=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp39;_tmp39.tag=0U;_tmp39.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp47);({void*_tmp37[1U]={& _tmp39};({struct _dyneither_ptr _tmp86A=({const char*_tmp38="`%s";_tag_dyneither(_tmp38,sizeof(char),4U);});Cyc_aprintf(_tmp86A,_tag_dyneither(_tmp37,sizeof(void*),1U));});});});_tmp36[0]=_tmp86B;});_tmp36;});
({void*_tmp870=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp31=_cycalloc(sizeof(*_tmp31));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp86F=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp32;_tmp32.tag=2U;({struct Cyc_Absyn_Tvar*_tmp86E=({struct Cyc_Absyn_Tvar*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->name=nm;_tmp33->identity=- 1;({void*_tmp86D=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp34=_cycalloc(sizeof(*_tmp34));({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp86C=({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp35;_tmp35.tag=0U;_tmp35.f1=& Cyc_Tcutil_ik;_tmp35;});_tmp34[0]=_tmp86C;});_tmp34;});_tmp33->kind=_tmp86D;});_tmp33;});_tmp32.f1=_tmp86E;});_tmp32;});_tmp31[0]=_tmp86F;});_tmp31;});*_tmp3A=_tmp870;});
goto _LL21;}}else{_LL24: _LL25:
 goto _LL21;}_LL21:;}
# 324
({struct Cyc_List_List*_tmp872=({struct Cyc_List_List*_tmp3B=_cycalloc(sizeof(*_tmp3B));({struct _tuple18*_tmp871=({struct _tuple18*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C->f1=_tmp47;_tmp3C->f2=_tmp46;_tmp3C;});_tmp3B->hd=_tmp871;});_tmp3B->tl=_tmp2D;_tmp3B;});_tmp2D=_tmp872;});goto _LL1A;}else{if(((struct _tuple8*)_tmp2F)->f1 != 0)goto _LL1F;else{goto _LL1F;}}}else{if(((struct _tuple8*)_tmp2F)->f1 != 0){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)((struct _tuple8*)_tmp2F)->f3)->tag == 15U){if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)((struct _tuple8*)_tmp2F)->f3)->f1)->tag == 1U){_LL1D: _tmp49=*_tmp2F->f1;_tmp48=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp2F->f3)->f1)->f2;_LL1E: {
# 328
struct _dyneither_ptr*nm=({struct _dyneither_ptr*_tmp42=_cycalloc(sizeof(*_tmp42));({struct _dyneither_ptr _tmp874=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp45;_tmp45.tag=0U;_tmp45.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp49);({void*_tmp43[1U]={& _tmp45};({struct _dyneither_ptr _tmp873=({const char*_tmp44="`%s";_tag_dyneither(_tmp44,sizeof(char),4U);});Cyc_aprintf(_tmp873,_tag_dyneither(_tmp43,sizeof(void*),1U));});});});_tmp42[0]=_tmp874;});_tmp42;});
({void*_tmp879=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3D=_cycalloc(sizeof(*_tmp3D));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp878=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp3E;_tmp3E.tag=2U;({struct Cyc_Absyn_Tvar*_tmp877=({struct Cyc_Absyn_Tvar*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->name=nm;_tmp3F->identity=- 1;({void*_tmp876=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp40=_cycalloc(sizeof(*_tmp40));({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp875=({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp41;_tmp41.tag=0U;_tmp41.f1=& Cyc_Tcutil_rk;_tmp41;});_tmp40[0]=_tmp875;});_tmp40;});_tmp3F->kind=_tmp876;});_tmp3F;});_tmp3E.f1=_tmp877;});_tmp3E;});_tmp3D[0]=_tmp878;});_tmp3D;});*_tmp48=_tmp879;});
goto _LL1A;}}else{goto _LL1F;}}else{goto _LL1F;}}else{_LL1F: _LL20:
 goto _LL1A;}}_LL1A:;}
# 334
return _tmp2D;}
# 338
static struct Cyc_List_List*Cyc_Parse_get_aggrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp4A=0;
for(0;x != 0;x=x->tl){
void*_tmp4B=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_tmp4C=_tmp4B;void*_tmp4F;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4C)->tag == 19U){_LL27: _tmp4F=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4C)->f1;_LL28:
# 343
({struct Cyc_List_List*_tmp87B=({struct Cyc_List_List*_tmp4D=_cycalloc(sizeof(*_tmp4D));({struct _tuple18*_tmp87A=({struct _tuple18*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name;_tmp4E->f2=_tmp4F;_tmp4E;});_tmp4D->hd=_tmp87A;});_tmp4D->tl=_tmp4A;_tmp4D;});_tmp4A=_tmp87B;});goto _LL26;}else{_LL29: _LL2A:
 goto _LL26;}_LL26:;}
# 347
return _tmp4A;}
# 351
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_tmp50=e->r;void*_tmp51=_tmp50;struct _dyneither_ptr*_tmp59;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp51)->tag == 1U){if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp51)->f1)->tag == 0U){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp51)->f1)->f1)->f1).Rel_n).tag == 1U){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp51)->f1)->f1)->f1).Rel_n).val == 0){_LL2C: _tmp59=(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp51)->f1)->f1)->f2;_LL2D:
# 354
{struct Cyc_List_List*_tmp52=tags;for(0;_tmp52 != 0;_tmp52=_tmp52->tl){
struct _tuple18*_tmp53=(struct _tuple18*)_tmp52->hd;struct _tuple18*_tmp54=_tmp53;struct _dyneither_ptr*_tmp58;void*_tmp57;_LL31: _tmp58=_tmp54->f1;_tmp57=_tmp54->f2;_LL32:;
if(Cyc_strptrcmp(_tmp58,_tmp59)== 0)
return({void*_tmp87E=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp55=_cycalloc(sizeof(*_tmp55));({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp87D=({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp56;_tmp56.tag=39U;({void*_tmp87C=Cyc_Tcutil_copy_type(_tmp57);_tmp56.f1=_tmp87C;});_tmp56;});_tmp55[0]=_tmp87D;});_tmp55;});Cyc_Absyn_new_exp(_tmp87E,e->loc);});}}
# 359
goto _LL2B;}else{goto _LL2E;}}else{goto _LL2E;}}else{goto _LL2E;}}else{_LL2E: _LL2F:
 goto _LL2B;}_LL2B:;}
# 362
return e;}
# 367
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_tmp5A=t;void*_tmp75;struct Cyc_Absyn_Tqual _tmp74;void*_tmp73;union Cyc_Absyn_Constraint*_tmp72;union Cyc_Absyn_Constraint*_tmp71;union Cyc_Absyn_Constraint*_tmp70;struct Cyc_Absyn_PtrLoc*_tmp6F;void*_tmp6E;struct Cyc_Absyn_Tqual _tmp6D;struct Cyc_Absyn_Exp*_tmp6C;union Cyc_Absyn_Constraint*_tmp6B;unsigned int _tmp6A;switch(*((int*)_tmp5A)){case 8U: _LL34: _tmp6E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A)->f1).elt_type;_tmp6D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A)->f1).tq;_tmp6C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A)->f1).num_elts;_tmp6B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A)->f1).zero_term;_tmp6A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A)->f1).zt_loc;_LL35: {
# 370
struct Cyc_Absyn_Exp*nelts2=_tmp6C;
if(_tmp6C != 0)
({struct Cyc_Absyn_Exp*_tmp87F=Cyc_Parse_substitute_tags_exp(tags,_tmp6C);nelts2=_tmp87F;});{
# 374
void*_tmp5B=Cyc_Parse_substitute_tags(tags,_tmp6E);
if(_tmp6C != nelts2  || _tmp6E != _tmp5B)
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5C=_cycalloc(sizeof(*_tmp5C));({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp881=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp5D;_tmp5D.tag=8U;({struct Cyc_Absyn_ArrayInfo _tmp880=({struct Cyc_Absyn_ArrayInfo _tmp5E;_tmp5E.elt_type=_tmp5B;_tmp5E.tq=_tmp6D;_tmp5E.num_elts=nelts2;_tmp5E.zero_term=_tmp6B;_tmp5E.zt_loc=_tmp6A;_tmp5E;});_tmp5D.f1=_tmp880;});_tmp5D;});_tmp5C[0]=_tmp881;});_tmp5C;});
goto _LL33;};}case 5U: _LL36: _tmp75=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A)->f1).elt_typ;_tmp74=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A)->f1).elt_tq;_tmp73=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A)->f1).ptr_atts).rgn;_tmp72=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A)->f1).ptr_atts).nullable;_tmp71=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A)->f1).ptr_atts).bounds;_tmp70=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A)->f1).ptr_atts).zero_term;_tmp6F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A)->f1).ptr_atts).ptrloc;_LL37: {
# 379
void*_tmp5F=Cyc_Parse_substitute_tags(tags,_tmp75);
union Cyc_Absyn_Constraint*_tmp60=_tmp71;
{union Cyc_Absyn_Constraint _tmp61=*_tmp71;union Cyc_Absyn_Constraint _tmp62=_tmp61;struct Cyc_Absyn_Exp*_tmp66;if((_tmp62.Eq_constr).tag == 1U){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)(_tmp62.Eq_constr).val)->tag == 1U){_LL3B: _tmp66=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)(_tmp62.Eq_constr).val)->f1;_LL3C:
# 383
({struct Cyc_Absyn_Exp*_tmp882=Cyc_Parse_substitute_tags_exp(tags,_tmp66);_tmp66=_tmp882;});
({union Cyc_Absyn_Constraint*_tmp885=({union Cyc_Absyn_Constraint*_tmp63=_cycalloc(sizeof(*_tmp63));({void*_tmp884=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp64=_cycalloc(sizeof(*_tmp64));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp883=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp65;_tmp65.tag=1U;_tmp65.f1=_tmp66;_tmp65;});_tmp64[0]=_tmp883;});_tmp64;});(_tmp63->Eq_constr).val=_tmp884;});(_tmp63->Eq_constr).tag=1;_tmp63;});_tmp60=_tmp885;});
goto _LL3A;}else{goto _LL3D;}}else{_LL3D: _LL3E:
 goto _LL3A;}_LL3A:;}
# 388
if(_tmp5F != _tmp75  || _tmp60 != _tmp71)
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp67=_cycalloc(sizeof(*_tmp67));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp888=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp68;_tmp68.tag=5U;({struct Cyc_Absyn_PtrInfo _tmp887=({struct Cyc_Absyn_PtrInfo _tmp69;_tmp69.elt_typ=_tmp5F;_tmp69.elt_tq=_tmp74;({struct Cyc_Absyn_PtrAtts _tmp886=({(_tmp69.ptr_atts).rgn=_tmp73;(_tmp69.ptr_atts).nullable=_tmp72;(_tmp69.ptr_atts).bounds=_tmp60;(_tmp69.ptr_atts).zero_term=_tmp70;(_tmp69.ptr_atts).ptrloc=_tmp6F;_tmp69.ptr_atts;});_tmp69.ptr_atts=_tmp886;});_tmp69;});_tmp68.f1=_tmp887;});_tmp68;});_tmp67[0]=_tmp888;});_tmp67;});
goto _LL33;}default: _LL38: _LL39:
# 393
 goto _LL33;}_LL33:;}
# 395
return t;}
# 400
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
({void*_tmp889=Cyc_Parse_substitute_tags(tags,x->type);x->type=_tmp889;});}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 407
static struct _tuple19*Cyc_Parse_get_tqual_typ(unsigned int loc,struct _tuple8*t){
# 409
return({struct _tuple19*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->f1=(*t).f2;_tmp76->f2=(*t).f3;_tmp76;});}
# 412
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp77=tm;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp77)->tag == 4U){_LL40: _LL41:
 return 1;}else{_LL42: _LL43:
 return 0;}_LL3F:;}
# 421
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
if(({struct _dyneither_ptr _tmp88A=(struct _dyneither_ptr)s;Cyc_zstrcmp(_tmp88A,({const char*_tmp78="`H";_tag_dyneither(_tmp78,sizeof(char),3U);}));})== 0)
return(void*)& Cyc_Absyn_HeapRgn_val;else{
if(({struct _dyneither_ptr _tmp88B=(struct _dyneither_ptr)s;Cyc_zstrcmp(_tmp88B,({const char*_tmp79="`U";_tag_dyneither(_tmp79,sizeof(char),3U);}));})== 0)
return(void*)& Cyc_Absyn_UniqueRgn_val;else{
if(({struct _dyneither_ptr _tmp88C=(struct _dyneither_ptr)s;Cyc_zstrcmp(_tmp88C,({const char*_tmp7A="`RC";_tag_dyneither(_tmp7A,sizeof(char),4U);}));})== 0)
return(void*)& Cyc_Absyn_RefCntRgn_val;else{
# 429
return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp88F=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp7C;_tmp7C.tag=2U;({struct Cyc_Absyn_Tvar*_tmp88E=({struct Cyc_Absyn_Tvar*_tmp7D=_cycalloc(sizeof(*_tmp7D));({struct _dyneither_ptr*_tmp88D=({struct _dyneither_ptr*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E[0]=s;_tmp7E;});_tmp7D->name=_tmp88D;});_tmp7D->identity=- 1;_tmp7D->kind=k;_tmp7D;});_tmp7C.f1=_tmp88E;});_tmp7C;});_tmp7B[0]=_tmp88F;});_tmp7B;});}}}}
# 436
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmp7F=t;struct Cyc_Absyn_Tvar*_tmp82;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7F)->tag == 2U){_LL45: _tmp82=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7F)->f1;_LL46:
 return _tmp82;}else{_LL47: _LL48:
({void*_tmp80=0U;({unsigned int _tmp891=loc;struct _dyneither_ptr _tmp890=({const char*_tmp81="expecting a list of type variables, not types";_tag_dyneither(_tmp81,sizeof(char),46U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp891,_tmp890,_tag_dyneither(_tmp80,sizeof(void*),0U));});});}_LL44:;}
# 444
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmp83=Cyc_Tcutil_compress(t);void*_tmp84=_tmp83;void**_tmp89;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp84)->tag == 2U){_LL4A: _tmp89=(void**)&(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp84)->f1)->kind;_LL4B: {
# 447
void*_tmp85=Cyc_Absyn_compress_kb(*_tmp89);void*_tmp86=_tmp85;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp86)->tag == 1U){_LL4F: _LL50:
# 449
 if(!leq)({void*_tmp892=Cyc_Tcutil_kind_to_bound(k);*_tmp89=_tmp892;});else{
({void*_tmp894=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp87=_cycalloc(sizeof(*_tmp87));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp893=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp88;_tmp88.tag=2U;_tmp88.f1=0;_tmp88.f2=k;_tmp88;});_tmp87[0]=_tmp893;});_tmp87;});*_tmp89=_tmp894;});}
return;}else{_LL51: _LL52:
 return;}_LL4E:;}}else{_LL4C: _LL4D:
# 454
 return;}_LL49:;}
# 459
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned int loc){
# 465
if(tds == 0)return tms;
# 470
if(tms == 0)return 0;{
# 472
void*_tmp8A=(void*)tms->hd;void*_tmp8B=_tmp8A;void*_tmpA8;if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp8B)->tag == 3U){_LL54: _tmpA8=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp8B)->f1;_LL55:
# 475
 if(tms->tl == 0  || 
Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 478
void*_tmp8C=_tmpA8;struct Cyc_List_List*_tmpA6;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp8C)->tag == 1U){_LL59: _LL5A:
# 480
({void*_tmp8D=0U;({unsigned int _tmp896=loc;struct _dyneither_ptr _tmp895=({const char*_tmp8E="function declaration with both new- and old-style parameter declarations; ignoring old-style";_tag_dyneither(_tmp8E,sizeof(char),93U);});Cyc_Warn_warn(_tmp896,_tmp895,_tag_dyneither(_tmp8D,sizeof(void*),0U));});});
# 482
return tms;}else{_LL5B: _tmpA6=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp8C)->f1;_LL5C:
# 484
 if(({int _tmp897=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA6);_tmp897 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds);}))
({void*_tmp8F=0U;({unsigned int _tmp899=loc;struct _dyneither_ptr _tmp898=({const char*_tmp90="wrong number of parameter declarations in old-style function declaration";_tag_dyneither(_tmp90,sizeof(char),73U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp899,_tmp898,_tag_dyneither(_tmp8F,sizeof(void*),0U));});});{
# 488
struct Cyc_List_List*rev_new_params=0;
for(0;_tmpA6 != 0;_tmpA6=_tmpA6->tl){
struct Cyc_List_List*_tmp91=tds;
for(0;_tmp91 != 0;_tmp91=_tmp91->tl){
struct Cyc_Absyn_Decl*_tmp92=(struct Cyc_Absyn_Decl*)_tmp91->hd;
void*_tmp93=_tmp92->r;void*_tmp94=_tmp93;struct Cyc_Absyn_Vardecl*_tmp9D;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp94)->tag == 0U){_LL5E: _tmp9D=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp94)->f1;_LL5F:
# 495
 if(Cyc_zstrptrcmp((*_tmp9D->name).f2,(struct _dyneither_ptr*)_tmpA6->hd)!= 0)
continue;
if(_tmp9D->initializer != 0)
({void*_tmp95=0U;({unsigned int _tmp89B=_tmp92->loc;struct _dyneither_ptr _tmp89A=({const char*_tmp96="initializer found in parameter declaration";_tag_dyneither(_tmp96,sizeof(char),43U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp89B,_tmp89A,_tag_dyneither(_tmp95,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp9D->name))
({void*_tmp97=0U;({unsigned int _tmp89D=_tmp92->loc;struct _dyneither_ptr _tmp89C=({const char*_tmp98="namespaces forbidden in parameter declarations";_tag_dyneither(_tmp98,sizeof(char),47U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp89D,_tmp89C,_tag_dyneither(_tmp97,sizeof(void*),0U));});});
({struct Cyc_List_List*_tmp89F=({struct Cyc_List_List*_tmp99=_cycalloc(sizeof(*_tmp99));({struct _tuple8*_tmp89E=({struct _tuple8*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->f1=(*_tmp9D->name).f2;_tmp9A->f2=_tmp9D->tq;_tmp9A->f3=_tmp9D->type;_tmp9A;});_tmp99->hd=_tmp89E;});_tmp99->tl=rev_new_params;_tmp99;});rev_new_params=_tmp89F;});
# 504
goto L;}else{_LL60: _LL61:
({void*_tmp9B=0U;({unsigned int _tmp8A1=_tmp92->loc;struct _dyneither_ptr _tmp8A0=({const char*_tmp9C="nonvariable declaration in parameter type";_tag_dyneither(_tmp9C,sizeof(char),42U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp8A1,_tmp8A0,_tag_dyneither(_tmp9B,sizeof(void*),0U));});});}_LL5D:;}
# 508
L: if(_tmp91 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmpA0;_tmpA0.tag=0U;_tmpA0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmpA6->hd));({void*_tmp9E[1U]={& _tmpA0};({unsigned int _tmp8A3=loc;struct _dyneither_ptr _tmp8A2=({const char*_tmp9F="%s is not given a type";_tag_dyneither(_tmp9F,sizeof(char),23U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp8A3,_tmp8A2,_tag_dyneither(_tmp9E,sizeof(void*),1U));});});});}
# 511
return({struct Cyc_List_List*_tmpA1=_region_malloc(yy,sizeof(*_tmpA1));({void*_tmp8A8=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpA2=_region_malloc(yy,sizeof(*_tmpA2));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp8A7=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpA3;_tmpA3.tag=3U;({void*_tmp8A6=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpA4=_region_malloc(yy,sizeof(*_tmpA4));({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp8A5=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpA5;_tmpA5.tag=1U;({struct Cyc_List_List*_tmp8A4=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params);_tmpA5.f1=_tmp8A4;});_tmpA5.f2=0;_tmpA5.f3=0;_tmpA5.f4=0;_tmpA5.f5=0;_tmpA5.f6=0;_tmpA5.f7=0;_tmpA5;});_tmpA4[0]=_tmp8A5;});_tmpA4;});_tmpA3.f1=_tmp8A6;});_tmpA3;});_tmpA2[0]=_tmp8A7;});_tmpA2;});_tmpA1->hd=_tmp8A8;});_tmpA1->tl=0;_tmpA1;});};}_LL58:;}
# 518
goto _LL57;}else{_LL56: _LL57:
 return({struct Cyc_List_List*_tmpA7=_region_malloc(yy,sizeof(*_tmpA7));_tmpA7->hd=(void*)tms->hd;({struct Cyc_List_List*_tmp8A9=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_tmpA7->tl=_tmp8A9;});_tmpA7;});}_LL53:;};}
# 526
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 530
if(tds != 0)
({struct Cyc_Parse_Declarator _tmp8AB=({struct Cyc_Parse_Declarator _tmpA9;_tmpA9.id=d.id;_tmpA9.varloc=d.varloc;({struct Cyc_List_List*_tmp8AA=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_tmpA9.tms=_tmp8AA;});_tmpA9;});d=_tmp8AB;});{
# 533
enum Cyc_Absyn_Scope sc=Cyc_Absyn_Public;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 539
if(dso != 0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;
# 545
if(dso->sc != 0){
enum Cyc_Parse_Storage_class _tmpAA=*((enum Cyc_Parse_Storage_class*)_check_null(dso->sc));enum Cyc_Parse_Storage_class _tmpAB=_tmpAA;switch(_tmpAB){case Cyc_Parse_Extern_sc: _LL63: _LL64:
 sc=Cyc_Absyn_Extern;goto _LL62;case Cyc_Parse_Static_sc: _LL65: _LL66:
 sc=Cyc_Absyn_Static;goto _LL62;default: _LL67: _LL68:
({void*_tmpAC=0U;({unsigned int _tmp8AD=loc;struct _dyneither_ptr _tmp8AC=({const char*_tmpAD="bad storage class on function";_tag_dyneither(_tmpAD,sizeof(char),30U);});Cyc_Warn_err(_tmp8AD,_tmp8AC,_tag_dyneither(_tmpAC,sizeof(void*),0U));});});goto _LL62;}_LL62:;}}{
# 552
void*_tmpAE=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple13 _tmpAF=Cyc_Parse_apply_tms(tq,_tmpAE,atts,d.tms);struct _tuple13 _tmpB0=_tmpAF;struct Cyc_Absyn_Tqual _tmpD5;void*_tmpD4;struct Cyc_List_List*_tmpD3;struct Cyc_List_List*_tmpD2;_LL6A: _tmpD5=_tmpB0.f1;_tmpD4=_tmpB0.f2;_tmpD3=_tmpB0.f3;_tmpD2=_tmpB0.f4;_LL6B:;
# 557
if(_tmpD3 != 0)
# 560
({void*_tmpB1=0U;({unsigned int _tmp8AF=loc;struct _dyneither_ptr _tmp8AE=({const char*_tmpB2="bad type params, ignoring";_tag_dyneither(_tmpB2,sizeof(char),26U);});Cyc_Warn_warn(_tmp8AF,_tmp8AE,_tag_dyneither(_tmpB1,sizeof(void*),0U));});});{
# 562
void*_tmpB3=_tmpD4;struct Cyc_List_List*_tmpD1;void*_tmpD0;struct Cyc_Absyn_Tqual _tmpCF;void*_tmpCE;struct Cyc_List_List*_tmpCD;int _tmpCC;struct Cyc_Absyn_VarargInfo*_tmpCB;struct Cyc_List_List*_tmpCA;struct Cyc_List_List*_tmpC9;struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_List_List*_tmpC7;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_List_List*_tmpC5;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->tag == 9U){_LL6D: _tmpD1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).tvars;_tmpD0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).effect;_tmpCF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).ret_tqual;_tmpCE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).ret_typ;_tmpCD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).args;_tmpCC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).c_varargs;_tmpCB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).cyc_varargs;_tmpCA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).rgn_po;_tmpC9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).attributes;_tmpC8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).requires_clause;_tmpC7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).requires_relns;_tmpC6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).ensures_clause;_tmpC5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB3)->f1).ensures_relns;_LL6E: {
# 566
struct Cyc_List_List*_tmpB4=0;
{struct Cyc_List_List*_tmpB5=_tmpCD;for(0;_tmpB5 != 0;_tmpB5=_tmpB5->tl){
struct _tuple8*_tmpB6=(struct _tuple8*)_tmpB5->hd;struct _tuple8*_tmpB7=_tmpB6;struct _dyneither_ptr*_tmpC1;struct Cyc_Absyn_Tqual _tmpC0;void*_tmpBF;_LL72: _tmpC1=_tmpB7->f1;_tmpC0=_tmpB7->f2;_tmpBF=_tmpB7->f3;_LL73:;
if(_tmpC1 == 0){
({void*_tmpB8=0U;({unsigned int _tmp8B1=loc;struct _dyneither_ptr _tmp8B0=({const char*_tmpB9="missing argument variable in function prototype";_tag_dyneither(_tmpB9,sizeof(char),48U);});Cyc_Warn_err(_tmp8B1,_tmp8B0,_tag_dyneither(_tmpB8,sizeof(void*),0U));});});
({struct Cyc_List_List*_tmp8B4=({struct Cyc_List_List*_tmpBA=_cycalloc(sizeof(*_tmpBA));({struct _tuple8*_tmp8B3=({struct _tuple8*_tmpBB=_cycalloc(sizeof(*_tmpBB));({struct _dyneither_ptr*_tmp8B2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"?",sizeof(char),2U);_tmpBB->f1=_tmp8B2;});_tmpBB->f2=_tmpC0;_tmpBB->f3=_tmpBF;_tmpBB;});_tmpBA->hd=_tmp8B3;});_tmpBA->tl=_tmpB4;_tmpBA;});_tmpB4=_tmp8B4;});}else{
# 573
({struct Cyc_List_List*_tmp8B6=({struct Cyc_List_List*_tmpBD=_cycalloc(sizeof(*_tmpBD));({struct _tuple8*_tmp8B5=({struct _tuple8*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE->f1=_tmpC1;_tmpBE->f2=_tmpC0;_tmpBE->f3=_tmpBF;_tmpBE;});_tmpBD->hd=_tmp8B5;});_tmpBD->tl=_tmpB4;_tmpBD;});_tmpB4=_tmp8B6;});}}}
# 577
return({struct Cyc_Absyn_Fndecl*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2->sc=sc;_tmpC2->name=d.id;_tmpC2->tvs=_tmpD1;_tmpC2->is_inline=is_inline;_tmpC2->effect=_tmpD0;_tmpC2->ret_tqual=_tmpCF;_tmpC2->ret_type=_tmpCE;({struct Cyc_List_List*_tmp8B7=
# 580
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpB4);_tmpC2->args=_tmp8B7;});_tmpC2->c_varargs=_tmpCC;_tmpC2->cyc_varargs=_tmpCB;_tmpC2->rgn_po=_tmpCA;_tmpC2->body=body;_tmpC2->cached_typ=0;_tmpC2->param_vardecls=0;_tmpC2->fn_vardecl=0;({struct Cyc_List_List*_tmp8B8=
# 587
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpC9,_tmpD2);_tmpC2->attributes=_tmp8B8;});_tmpC2->requires_clause=_tmpC8;_tmpC2->requires_relns=0;_tmpC2->ensures_clause=_tmpC6;_tmpC2->ensures_relns=0;_tmpC2;});}}else{_LL6F: _LL70:
# 592
({void*_tmpC3=0U;({unsigned int _tmp8BA=loc;struct _dyneither_ptr _tmp8B9=({const char*_tmpC4="declarator is not a function prototype";_tag_dyneither(_tmpC4,sizeof(char),39U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp8BA,_tmp8B9,_tag_dyneither(_tmpC3,sizeof(void*),0U));});});}_LL6C:;};};};}static char _tmpD6[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 596
static struct _dyneither_ptr Cyc_Parse_msg1={_tmpD6,_tmpD6,_tmpD6 + 76U};static char _tmpD7[87U]="const or volatile may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 598
static struct _dyneither_ptr Cyc_Parse_msg2={_tmpD7,_tmpD7,_tmpD7 + 87U};static char _tmpD8[74U]="type specifier includes more than one declaration \n\t(missing ';' or ','?)";
# 600
static struct _dyneither_ptr Cyc_Parse_msg3={_tmpD8,_tmpD8,_tmpD8 + 74U};static char _tmpD9[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 602
static struct _dyneither_ptr Cyc_Parse_msg4={_tmpD9,_tmpD9,_tmpD9 + 84U};
# 609
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned int loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 612
if(s1.Signed_spec  && s2.Signed_spec)
({void*_tmpDA=0U;({unsigned int _tmp8BC=loc;struct _dyneither_ptr _tmp8BB=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp8BC,_tmp8BB,_tag_dyneither(_tmpDA,sizeof(void*),0U));});});
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec)
({void*_tmpDB=0U;({unsigned int _tmp8BE=loc;struct _dyneither_ptr _tmp8BD=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp8BE,_tmp8BD,_tag_dyneither(_tmpDB,sizeof(void*),0U));});});
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec)
({void*_tmpDC=0U;({unsigned int _tmp8C0=loc;struct _dyneither_ptr _tmp8BF=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp8C0,_tmp8BF,_tag_dyneither(_tmpDC,sizeof(void*),0U));});});
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec)
({void*_tmpDD=0U;({unsigned int _tmp8C2=loc;struct _dyneither_ptr _tmp8C1=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp8C2,_tmp8C1,_tag_dyneither(_tmpDD,sizeof(void*),0U));});});
s1.Long_Long_spec=
(s1.Long_Long_spec  || s2.Long_Long_spec) || s1.Long_spec  && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec  && (s1.Long_spec  || s2.Long_spec);
if(s1.Valid_type_spec  && s2.Valid_type_spec)
({void*_tmpDE=0U;({unsigned int _tmp8C4=loc;struct _dyneither_ptr _tmp8C3=Cyc_Parse_msg1;Cyc_Warn_err(_tmp8C4,_tmp8C3,_tag_dyneither(_tmpDE,sizeof(void*),0U));});});else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 634
return s1;}
# 640
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc){
# 643
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec  || ts.Unsigned_spec;
int seen_size=(ts.Short_spec  || ts.Long_spec) || ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec:(void*)& Cyc_Absyn_VoidType_val;
enum Cyc_Absyn_Size_of sz=Cyc_Absyn_Int_sz;
enum Cyc_Absyn_Sign sgn=Cyc_Absyn_Signed;
# 650
if(ts.Signed_spec  && ts.Unsigned_spec)
({void*_tmpDF=0U;({unsigned int _tmp8C6=loc;struct _dyneither_ptr _tmp8C5=Cyc_Parse_msg4;Cyc_Warn_err(_tmp8C6,_tmp8C5,_tag_dyneither(_tmpDF,sizeof(void*),0U));});});
if(ts.Unsigned_spec)sgn=Cyc_Absyn_Unsigned;
if(ts.Short_spec  && (ts.Long_spec  || ts.Long_Long_spec) || 
ts.Long_spec  && ts.Long_Long_spec)
({void*_tmpE0=0U;({unsigned int _tmp8C8=loc;struct _dyneither_ptr _tmp8C7=Cyc_Parse_msg4;Cyc_Warn_err(_tmp8C8,_tmp8C7,_tag_dyneither(_tmpE0,sizeof(void*),0U));});});
if(ts.Short_spec)sz=Cyc_Absyn_Short_sz;
if(ts.Long_spec)sz=Cyc_Absyn_Long_sz;
if(ts.Long_Long_spec)sz=Cyc_Absyn_LongLong_sz;
# 662
if(!seen_type){
if(!seen_sign  && !seen_size)
({void*_tmpE1=0U;({unsigned int _tmp8CA=loc;struct _dyneither_ptr _tmp8C9=({const char*_tmpE2="missing type within specifier";_tag_dyneither(_tmpE2,sizeof(char),30U);});Cyc_Warn_warn(_tmp8CA,_tmp8C9,_tag_dyneither(_tmpE1,sizeof(void*),0U));});});
({void*_tmp8CB=Cyc_Absyn_int_typ(sgn,sz);t=_tmp8CB;});}else{
# 667
if(seen_sign){
void*_tmpE3=t;enum Cyc_Absyn_Sign _tmpE7;enum Cyc_Absyn_Size_of _tmpE6;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE3)->tag == 6U){_LL75: _tmpE7=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE3)->f1;_tmpE6=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE3)->f2;_LL76:
# 670
 if(_tmpE7 != sgn)
({void*_tmp8CC=Cyc_Absyn_int_typ(sgn,_tmpE6);t=_tmp8CC;});
goto _LL74;}else{_LL77: _LL78:
({void*_tmpE4=0U;({unsigned int _tmp8CE=loc;struct _dyneither_ptr _tmp8CD=({const char*_tmpE5="sign specification on non-integral type";_tag_dyneither(_tmpE5,sizeof(char),40U);});Cyc_Warn_err(_tmp8CE,_tmp8CD,_tag_dyneither(_tmpE4,sizeof(void*),0U));});});goto _LL74;}_LL74:;}
# 675
if(seen_size){
void*_tmpE8=t;enum Cyc_Absyn_Sign _tmpEC;enum Cyc_Absyn_Size_of _tmpEB;switch(*((int*)_tmpE8)){case 6U: _LL7A: _tmpEC=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE8)->f1;_tmpEB=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE8)->f2;_LL7B:
# 678
 if(_tmpEB != sz)
({void*_tmp8CF=Cyc_Absyn_int_typ(_tmpEC,sz);t=_tmp8CF;});
goto _LL79;case 7U: _LL7C: _LL7D:
# 682
({void*_tmp8D0=Cyc_Absyn_float_typ(2);t=_tmp8D0;});goto _LL79;default: _LL7E: _LL7F:
({void*_tmpE9=0U;({unsigned int _tmp8D2=loc;struct _dyneither_ptr _tmp8D1=({const char*_tmpEA="size qualifier on non-integral type";_tag_dyneither(_tmpEA,sizeof(char),36U);});Cyc_Warn_err(_tmp8D2,_tmp8D1,_tag_dyneither(_tmpE9,sizeof(void*),0U));});});goto _LL79;}_LL79:;}}
# 686
return t;}
# 689
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple10*ds,struct Cyc_List_List*shared_atts){
# 693
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmpED=d.id;
unsigned int _tmpEE=d.varloc;
struct _tuple13 _tmpEF=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct _tuple13 _tmpF0=_tmpEF;struct Cyc_Absyn_Tqual _tmpF8;void*_tmpF7;struct Cyc_List_List*_tmpF6;struct Cyc_List_List*_tmpF5;_LL81: _tmpF8=_tmpF0.f1;_tmpF7=_tmpF0.f2;_tmpF6=_tmpF0.f3;_tmpF5=_tmpF0.f4;_LL82:;
# 700
if(ds->tl == 0)
return({struct Cyc_List_List*_tmpF1=_region_malloc(r,sizeof(*_tmpF1));({struct _tuple15*_tmp8D3=({struct _tuple15*_tmpF2=_region_malloc(r,sizeof(*_tmpF2));_tmpF2->f1=_tmpEE;_tmpF2->f2=_tmpED;_tmpF2->f3=_tmpF8;_tmpF2->f4=_tmpF7;_tmpF2->f5=_tmpF6;_tmpF2->f6=_tmpF5;_tmpF2;});_tmpF1->hd=_tmp8D3;});_tmpF1->tl=0;_tmpF1;});else{
# 703
return({struct Cyc_List_List*_tmpF3=_region_malloc(r,sizeof(*_tmpF3));({struct _tuple15*_tmp8D4=({struct _tuple15*_tmpF4=_region_malloc(r,sizeof(*_tmpF4));_tmpF4->f1=_tmpEE;_tmpF4->f2=_tmpED;_tmpF4->f3=_tmpF8;_tmpF4->f4=_tmpF7;_tmpF4->f5=_tmpF6;_tmpF4->f6=_tmpF5;_tmpF4;});_tmpF3->hd=_tmp8D4;});({struct Cyc_List_List*_tmp8D9=({
struct _RegionHandle*_tmp8D8=r;struct Cyc_Absyn_Tqual _tmp8D7=tq;void*_tmp8D6=Cyc_Tcutil_copy_type(t);struct _tuple10*_tmp8D5=ds->tl;Cyc_Parse_apply_tmss(_tmp8D8,_tmp8D7,_tmp8D6,_tmp8D5,shared_atts);});_tmpF3->tl=_tmp8D9;});_tmpF3;});}};}
# 707
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 710
if(tms == 0)return({struct _tuple13 _tmpF9;_tmpF9.f1=tq;_tmpF9.f2=t;_tmpF9.f3=0;_tmpF9.f4=atts;_tmpF9;});{
void*_tmpFA=(void*)tms->hd;void*_tmpFB=_tmpFA;unsigned int _tmp124;struct Cyc_List_List*_tmp123;struct Cyc_Absyn_PtrAtts _tmp122;struct Cyc_Absyn_Tqual _tmp121;struct Cyc_List_List*_tmp120;unsigned int _tmp11F;void*_tmp11E;struct Cyc_Absyn_Exp*_tmp11D;union Cyc_Absyn_Constraint*_tmp11C;unsigned int _tmp11B;union Cyc_Absyn_Constraint*_tmp11A;unsigned int _tmp119;switch(*((int*)_tmpFB)){case 0U: _LL84: _tmp11A=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpFB)->f1;_tmp119=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpFB)->f2;_LL85:
# 713
 return({struct Cyc_Absyn_Tqual _tmp8DC=Cyc_Absyn_empty_tqual(0U);void*_tmp8DB=
Cyc_Absyn_array_typ(t,tq,0,_tmp11A,_tmp119);
# 713
struct Cyc_List_List*_tmp8DA=atts;Cyc_Parse_apply_tms(_tmp8DC,_tmp8DB,_tmp8DA,tms->tl);});case 1U: _LL86: _tmp11D=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpFB)->f1;_tmp11C=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpFB)->f2;_tmp11B=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpFB)->f3;_LL87:
# 716
 return({struct Cyc_Absyn_Tqual _tmp8DF=Cyc_Absyn_empty_tqual(0U);void*_tmp8DE=
Cyc_Absyn_array_typ(t,tq,_tmp11D,_tmp11C,_tmp11B);
# 716
struct Cyc_List_List*_tmp8DD=atts;Cyc_Parse_apply_tms(_tmp8DF,_tmp8DE,_tmp8DD,tms->tl);});case 3U: _LL88: _tmp11E=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpFB)->f1;_LL89: {
# 719
void*_tmpFC=_tmp11E;unsigned int _tmp112;struct Cyc_List_List*_tmp111;int _tmp110;struct Cyc_Absyn_VarargInfo*_tmp10F;void*_tmp10E;struct Cyc_List_List*_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10B;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpFC)->tag == 1U){_LL91: _tmp111=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpFC)->f1;_tmp110=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpFC)->f2;_tmp10F=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpFC)->f3;_tmp10E=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpFC)->f4;_tmp10D=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpFC)->f5;_tmp10C=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpFC)->f6;_tmp10B=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpFC)->f7;_LL92: {
# 721
struct Cyc_List_List*typvars=0;
# 723
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Absyn_fntype_att((void*)as->hd))
({struct Cyc_List_List*_tmp8E0=({struct Cyc_List_List*_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD->hd=(void*)as->hd;_tmpFD->tl=fn_atts;_tmpFD;});fn_atts=_tmp8E0;});else{
# 728
({struct Cyc_List_List*_tmp8E1=({struct Cyc_List_List*_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE->hd=(void*)as->hd;_tmpFE->tl=new_atts;_tmpFE;});new_atts=_tmp8E1;});}}}
# 731
if(tms->tl != 0){
void*_tmpFF=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmp100=_tmpFF;struct Cyc_List_List*_tmp101;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp100)->tag == 4U){_LL96: _tmp101=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp100)->f1;_LL97:
# 734
 typvars=_tmp101;
tms=tms->tl;
goto _LL95;}else{_LL98: _LL99:
# 738
 goto _LL95;}_LL95:;}
# 742
if(((((!_tmp110  && _tmp10F == 0) && _tmp111 != 0) && _tmp111->tl == 0) && (*((struct _tuple8*)_tmp111->hd)).f1 == 0) && (*((struct _tuple8*)_tmp111->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)
# 747
_tmp111=0;{
# 750
struct Cyc_List_List*_tmp102=Cyc_Parse_get_arg_tags(_tmp111);
# 752
if(_tmp102 != 0)
({void*_tmp8E2=Cyc_Parse_substitute_tags(_tmp102,t);t=_tmp8E2;});
({void*_tmp8E3=Cyc_Parse_array2ptr(t,0);t=_tmp8E3;});
# 757
{struct Cyc_List_List*_tmp103=_tmp111;for(0;_tmp103 != 0;_tmp103=_tmp103->tl){
struct _tuple8*_tmp104=(struct _tuple8*)_tmp103->hd;struct _tuple8*_tmp105=_tmp104;struct _dyneither_ptr*_tmp108;struct Cyc_Absyn_Tqual _tmp107;void**_tmp106;_LL9B: _tmp108=_tmp105->f1;_tmp107=_tmp105->f2;_tmp106=(void**)& _tmp105->f3;_LL9C:;
if(_tmp102 != 0)
({void*_tmp8E4=Cyc_Parse_substitute_tags(_tmp102,*_tmp106);*_tmp106=_tmp8E4;});
({void*_tmp8E5=Cyc_Parse_array2ptr(*_tmp106,1);*_tmp106=_tmp8E5;});}}
# 769
return({struct Cyc_Absyn_Tqual _tmp8E8=Cyc_Absyn_empty_tqual(tq.loc);void*_tmp8E7=
Cyc_Absyn_function_typ(typvars,_tmp10E,tq,t,_tmp111,_tmp110,_tmp10F,_tmp10D,fn_atts,_tmp10C,_tmp10B);
# 769
struct Cyc_List_List*_tmp8E6=new_atts;Cyc_Parse_apply_tms(_tmp8E8,_tmp8E7,_tmp8E6,((struct Cyc_List_List*)_check_null(tms))->tl);});};}}else{_LL93: _tmp112=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpFC)->f2;_LL94:
# 776
({void*_tmp109=0U;({unsigned int _tmp8EA=_tmp112;struct _dyneither_ptr _tmp8E9=({const char*_tmp10A="function declaration without parameter types";_tag_dyneither(_tmp10A,sizeof(char),45U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp8EA,_tmp8E9,_tag_dyneither(_tmp109,sizeof(void*),0U));});});}_LL90:;}case 4U: _LL8A: _tmp120=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpFB)->f1;_tmp11F=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpFB)->f2;_LL8B:
# 783
 if(tms->tl == 0)
return({struct _tuple13 _tmp113;_tmp113.f1=tq;_tmp113.f2=t;_tmp113.f3=_tmp120;_tmp113.f4=atts;_tmp113;});
# 788
({void*_tmp114=0U;({unsigned int _tmp8EC=_tmp11F;struct _dyneither_ptr _tmp8EB=({const char*_tmp115="type parameters must appear before function arguments in declarator";_tag_dyneither(_tmp115,sizeof(char),68U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp8EC,_tmp8EB,_tag_dyneither(_tmp114,sizeof(void*),0U));});});case 2U: _LL8C: _tmp122=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpFB)->f1;_tmp121=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpFB)->f2;_LL8D:
# 791
 return({struct Cyc_Absyn_Tqual _tmp8F1=_tmp121;void*_tmp8F0=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp116=_cycalloc(sizeof(*_tmp116));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp8EE=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp117;_tmp117.tag=5U;({struct Cyc_Absyn_PtrInfo _tmp8ED=({struct Cyc_Absyn_PtrInfo _tmp118;_tmp118.elt_typ=t;_tmp118.elt_tq=tq;_tmp118.ptr_atts=_tmp122;_tmp118;});_tmp117.f1=_tmp8ED;});_tmp117;});_tmp116[0]=_tmp8EE;});_tmp116;});struct Cyc_List_List*_tmp8EF=atts;Cyc_Parse_apply_tms(_tmp8F1,_tmp8F0,_tmp8EF,tms->tl);});default: _LL8E: _tmp124=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpFB)->f1;_tmp123=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpFB)->f2;_LL8F:
# 796
 return({struct Cyc_Absyn_Tqual _tmp8F4=tq;void*_tmp8F3=t;struct Cyc_List_List*_tmp8F2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmp123);Cyc_Parse_apply_tms(_tmp8F4,_tmp8F3,_tmp8F2,tms->tl);});}_LL83:;};}
# 802
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned int loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 811
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned int loc,struct _tuple15*t){
struct _tuple15*_tmp125=t;unsigned int _tmp130;struct _tuple0*_tmp12F;struct Cyc_Absyn_Tqual _tmp12E;void*_tmp12D;struct Cyc_List_List*_tmp12C;struct Cyc_List_List*_tmp12B;_LL9E: _tmp130=_tmp125->f1;_tmp12F=_tmp125->f2;_tmp12E=_tmp125->f3;_tmp12D=_tmp125->f4;_tmp12C=_tmp125->f5;_tmp12B=_tmp125->f6;_LL9F:;
# 814
Cyc_Lex_register_typedef(_tmp12F);{
# 816
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmp126=_tmp12D;struct Cyc_Core_Opt*_tmp127;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp126)->tag == 1U){_LLA1: _tmp127=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp126)->f1;_LLA2:
# 820
 type=0;
if(_tmp127 == 0)kind=& Cyc_Tcutil_bko;else{
kind=_tmp127;}
goto _LLA0;}else{_LLA3: _LLA4:
 kind=0;type=_tmp12D;goto _LLA0;}_LLA0:;}
# 826
return({void*_tmp8F7=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp128=_cycalloc(sizeof(*_tmp128));({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp8F6=({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp129;_tmp129.tag=8U;({struct Cyc_Absyn_Typedefdecl*_tmp8F5=({struct Cyc_Absyn_Typedefdecl*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->name=_tmp12F;_tmp12A->tvs=_tmp12C;_tmp12A->kind=kind;_tmp12A->defn=type;_tmp12A->atts=_tmp12B;_tmp12A->tq=_tmp12E;_tmp12A->extern_c=0;_tmp12A;});_tmp129.f1=_tmp8F5;});_tmp129;});_tmp128[0]=_tmp8F6;});_tmp128;});Cyc_Absyn_new_decl(_tmp8F7,loc);});};}
# 833
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return({void*_tmp8F9=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp131=_cycalloc(sizeof(*_tmp131));({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp8F8=({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp132;_tmp132.tag=12U;_tmp132.f1=d;_tmp132.f2=s;_tmp132;});_tmp131[0]=_tmp8F8;});_tmp131;});Cyc_Absyn_new_stmt(_tmp8F9,d->loc);});}
# 839
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 848
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple12*ids,unsigned int tqual_loc,unsigned int loc){
# 852
struct _RegionHandle _tmp133=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp133;_push_region(mkrgn);
{struct Cyc_Parse_Declaration_spec _tmp134=ds;struct Cyc_Absyn_Tqual _tmp19D;struct Cyc_Parse_Type_specifier _tmp19C;struct Cyc_List_List*_tmp19B;_LLA6: _tmp19D=_tmp134.tq;_tmp19C=_tmp134.type_specs;_tmp19B=_tmp134.attributes;_LLA7:;
if(_tmp19D.loc == 0)_tmp19D.loc=tqual_loc;
if(ds.is_inline)
({void*_tmp135=0U;({unsigned int _tmp8FB=loc;struct _dyneither_ptr _tmp8FA=({const char*_tmp136="inline qualifier on non-function definition";_tag_dyneither(_tmp136,sizeof(char),44U);});Cyc_Warn_warn(_tmp8FB,_tmp8FA,_tag_dyneither(_tmp135,sizeof(void*),0U));});});{
# 858
enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;
int istypedef=0;
if(ds.sc != 0){
enum Cyc_Parse_Storage_class _tmp137=*ds.sc;enum Cyc_Parse_Storage_class _tmp138=_tmp137;switch(_tmp138){case Cyc_Parse_Typedef_sc: _LLA9: _LLAA:
 istypedef=1;goto _LLA8;case Cyc_Parse_Extern_sc: _LLAB: _LLAC:
 s=Cyc_Absyn_Extern;goto _LLA8;case Cyc_Parse_ExternC_sc: _LLAD: _LLAE:
 s=Cyc_Absyn_ExternC;goto _LLA8;case Cyc_Parse_Static_sc: _LLAF: _LLB0:
 s=Cyc_Absyn_Static;goto _LLA8;case Cyc_Parse_Auto_sc: _LLB1: _LLB2:
 s=Cyc_Absyn_Public;goto _LLA8;case Cyc_Parse_Register_sc: _LLB3: _LLB4:
 if(Cyc_Parse_no_register)s=Cyc_Absyn_Public;else{s=Cyc_Absyn_Register;}goto _LLA8;default: _LLB5: _LLB6:
 s=Cyc_Absyn_Abstract;goto _LLA8;}_LLA8:;}{
# 874
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 878
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;
break;}}}{
# 886
void*_tmp139=Cyc_Parse_collapse_type_specifiers(_tmp19C,loc);
if(declarators == 0){
# 890
void*_tmp13A=_tmp139;struct Cyc_List_List*_tmp181;struct _tuple0*_tmp180;struct _tuple0*_tmp17F;int _tmp17E;struct Cyc_List_List*_tmp17D;struct Cyc_Absyn_Datatypedecl**_tmp17C;enum Cyc_Absyn_AggrKind _tmp17B;struct _tuple0*_tmp17A;struct Cyc_List_List*_tmp179;struct Cyc_Absyn_Datatypedecl*_tmp178;struct Cyc_Absyn_Enumdecl*_tmp177;struct Cyc_Absyn_Aggrdecl*_tmp176;switch(*((int*)_tmp13A)){case 26U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13A)->f1)->r)){case 0U: _LLB8: _tmp176=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13A)->f1)->r)->f1;_LLB9:
# 892
({struct Cyc_List_List*_tmp8FC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp176->attributes,_tmp19B);_tmp176->attributes=_tmp8FC;});
_tmp176->sc=s;{
struct Cyc_List_List*_tmp13E=({struct Cyc_List_List*_tmp13B=_cycalloc(sizeof(*_tmp13B));({struct Cyc_Absyn_Decl*_tmp8FF=({void*_tmp8FE=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp13C=_cycalloc(sizeof(*_tmp13C));({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp8FD=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp13D;_tmp13D.tag=5U;_tmp13D.f1=_tmp176;_tmp13D;});_tmp13C[0]=_tmp8FD;});_tmp13C;});Cyc_Absyn_new_decl(_tmp8FE,loc);});_tmp13B->hd=_tmp8FF;});_tmp13B->tl=0;_tmp13B;});_npop_handler(0U);return _tmp13E;};case 1U: _LLBA: _tmp177=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13A)->f1)->r)->f1;_LLBB:
# 896
 if(_tmp19B != 0)({void*_tmp13F=0U;({unsigned int _tmp901=loc;struct _dyneither_ptr _tmp900=({const char*_tmp140="attributes on enum not supported";_tag_dyneither(_tmp140,sizeof(char),33U);});Cyc_Warn_err(_tmp901,_tmp900,_tag_dyneither(_tmp13F,sizeof(void*),0U));});});
_tmp177->sc=s;{
struct Cyc_List_List*_tmp144=({struct Cyc_List_List*_tmp141=_cycalloc(sizeof(*_tmp141));({struct Cyc_Absyn_Decl*_tmp904=({void*_tmp903=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp142=_cycalloc(sizeof(*_tmp142));({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp902=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp143;_tmp143.tag=7U;_tmp143.f1=_tmp177;_tmp143;});_tmp142[0]=_tmp902;});_tmp142;});Cyc_Absyn_new_decl(_tmp903,loc);});_tmp141->hd=_tmp904;});_tmp141->tl=0;_tmp141;});_npop_handler(0U);return _tmp144;};default: _LLBC: _tmp178=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp13A)->f1)->r)->f1;_LLBD:
# 900
 if(_tmp19B != 0)({void*_tmp145=0U;({unsigned int _tmp906=loc;struct _dyneither_ptr _tmp905=({const char*_tmp146="attributes on datatypes not supported";_tag_dyneither(_tmp146,sizeof(char),38U);});Cyc_Warn_err(_tmp906,_tmp905,_tag_dyneither(_tmp145,sizeof(void*),0U));});});
_tmp178->sc=s;{
struct Cyc_List_List*_tmp14A=({struct Cyc_List_List*_tmp147=_cycalloc(sizeof(*_tmp147));({struct Cyc_Absyn_Decl*_tmp909=({void*_tmp908=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp148=_cycalloc(sizeof(*_tmp148));({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp907=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp149;_tmp149.tag=6U;_tmp149.f1=_tmp178;_tmp149;});_tmp148[0]=_tmp907;});_tmp148;});Cyc_Absyn_new_decl(_tmp908,loc);});_tmp147->hd=_tmp909;});_tmp147->tl=0;_tmp147;});_npop_handler(0U);return _tmp14A;};}case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp13A)->f1).aggr_info).UnknownAggr).tag == 1U){_LLBE: _tmp17B=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp13A)->f1).aggr_info).UnknownAggr).val).f1;_tmp17A=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp13A)->f1).aggr_info).UnknownAggr).val).f2;_tmp179=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp13A)->f1).targs;_LLBF: {
# 904
struct Cyc_List_List*_tmp14B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp179);
struct Cyc_Absyn_Aggrdecl*_tmp14C=({struct Cyc_Absyn_Aggrdecl*_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153->kind=_tmp17B;_tmp153->sc=s;_tmp153->name=_tmp17A;_tmp153->tvs=_tmp14B;_tmp153->impl=0;_tmp153->attributes=0;_tmp153->expected_mem_kind=0;_tmp153;});
if(_tmp19B != 0)({void*_tmp14D=0U;({unsigned int _tmp90B=loc;struct _dyneither_ptr _tmp90A=({const char*_tmp14E="bad attributes on type declaration";_tag_dyneither(_tmp14E,sizeof(char),35U);});Cyc_Warn_err(_tmp90B,_tmp90A,_tag_dyneither(_tmp14D,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp152=({struct Cyc_List_List*_tmp14F=_cycalloc(sizeof(*_tmp14F));({struct Cyc_Absyn_Decl*_tmp90E=({void*_tmp90D=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp150=_cycalloc(sizeof(*_tmp150));({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp90C=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp151;_tmp151.tag=5U;_tmp151.f1=_tmp14C;_tmp151;});_tmp150[0]=_tmp90C;});_tmp150;});Cyc_Absyn_new_decl(_tmp90D,loc);});_tmp14F->hd=_tmp90E;});_tmp14F->tl=0;_tmp14F;});_npop_handler(0U);return _tmp152;};}}else{goto _LLC8;}case 3U: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp13A)->f1).datatype_info).KnownDatatype).tag == 2U){_LLC0: _tmp17C=(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp13A)->f1).datatype_info).KnownDatatype).val;_LLC1:
# 909
 if(_tmp19B != 0)({void*_tmp154=0U;({unsigned int _tmp910=loc;struct _dyneither_ptr _tmp90F=({const char*_tmp155="bad attributes on datatype";_tag_dyneither(_tmp155,sizeof(char),27U);});Cyc_Warn_err(_tmp910,_tmp90F,_tag_dyneither(_tmp154,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp159=({struct Cyc_List_List*_tmp156=_cycalloc(sizeof(*_tmp156));({struct Cyc_Absyn_Decl*_tmp913=({void*_tmp912=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp157=_cycalloc(sizeof(*_tmp157));({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp911=({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp158;_tmp158.tag=6U;_tmp158.f1=*_tmp17C;_tmp158;});_tmp157[0]=_tmp911;});_tmp157;});Cyc_Absyn_new_decl(_tmp912,loc);});_tmp156->hd=_tmp913;});_tmp156->tl=0;_tmp156;});_npop_handler(0U);return _tmp159;};}else{_LLC2: _tmp17F=((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp13A)->f1).datatype_info).UnknownDatatype).val).name;_tmp17E=((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp13A)->f1).datatype_info).UnknownDatatype).val).is_extensible;_tmp17D=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp13A)->f1).targs;_LLC3: {
# 912
struct Cyc_List_List*_tmp15A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp17D);
struct Cyc_Absyn_Decl*_tmp15B=Cyc_Absyn_datatype_decl(s,_tmp17F,_tmp15A,0,_tmp17E,loc);
if(_tmp19B != 0)({void*_tmp15C=0U;({unsigned int _tmp915=loc;struct _dyneither_ptr _tmp914=({const char*_tmp15D="bad attributes on datatype";_tag_dyneither(_tmp15D,sizeof(char),27U);});Cyc_Warn_err(_tmp915,_tmp914,_tag_dyneither(_tmp15C,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp15F=({struct Cyc_List_List*_tmp15E=_cycalloc(sizeof(*_tmp15E));_tmp15E->hd=_tmp15B;_tmp15E->tl=0;_tmp15E;});_npop_handler(0U);return _tmp15F;};}}case 13U: _LLC4: _tmp180=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp13A)->f1;_LLC5: {
# 917
struct Cyc_Absyn_Enumdecl*_tmp160=({struct Cyc_Absyn_Enumdecl*_tmp168=_cycalloc(sizeof(*_tmp168));_tmp168->sc=s;_tmp168->name=_tmp180;_tmp168->fields=0;_tmp168;});
if(_tmp19B != 0)({void*_tmp161=0U;({unsigned int _tmp917=loc;struct _dyneither_ptr _tmp916=({const char*_tmp162="bad attributes on enum";_tag_dyneither(_tmp162,sizeof(char),23U);});Cyc_Warn_err(_tmp917,_tmp916,_tag_dyneither(_tmp161,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp167=({struct Cyc_List_List*_tmp163=_cycalloc(sizeof(*_tmp163));({struct Cyc_Absyn_Decl*_tmp91A=({struct Cyc_Absyn_Decl*_tmp164=_cycalloc(sizeof(*_tmp164));({void*_tmp919=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp165=_cycalloc(sizeof(*_tmp165));({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp918=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp166;_tmp166.tag=7U;_tmp166.f1=_tmp160;_tmp166;});_tmp165[0]=_tmp918;});_tmp165;});_tmp164->r=_tmp919;});_tmp164->loc=loc;_tmp164;});_tmp163->hd=_tmp91A;});_tmp163->tl=0;_tmp163;});_npop_handler(0U);return _tmp167;};}case 14U: _LLC6: _tmp181=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp13A)->f1;_LLC7: {
# 923
struct Cyc_Absyn_Enumdecl*_tmp169=({struct Cyc_Absyn_Enumdecl*_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171->sc=s;({struct _tuple0*_tmp91B=Cyc_Parse_gensym_enum();_tmp171->name=_tmp91B;});({struct Cyc_Core_Opt*_tmp91C=({struct Cyc_Core_Opt*_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172->v=_tmp181;_tmp172;});_tmp171->fields=_tmp91C;});_tmp171;});
if(_tmp19B != 0)({void*_tmp16A=0U;({unsigned int _tmp91E=loc;struct _dyneither_ptr _tmp91D=({const char*_tmp16B="bad attributes on enum";_tag_dyneither(_tmp16B,sizeof(char),23U);});Cyc_Warn_err(_tmp91E,_tmp91D,_tag_dyneither(_tmp16A,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp170=({struct Cyc_List_List*_tmp16C=_cycalloc(sizeof(*_tmp16C));({struct Cyc_Absyn_Decl*_tmp921=({struct Cyc_Absyn_Decl*_tmp16D=_cycalloc(sizeof(*_tmp16D));({void*_tmp920=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp16E=_cycalloc(sizeof(*_tmp16E));({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp91F=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp16F;_tmp16F.tag=7U;_tmp16F.f1=_tmp169;_tmp16F;});_tmp16E[0]=_tmp91F;});_tmp16E;});_tmp16D->r=_tmp920;});_tmp16D->loc=loc;_tmp16D;});_tmp16C->hd=_tmp921;});_tmp16C->tl=0;_tmp16C;});_npop_handler(0U);return _tmp170;};}default: _LLC8: _LLC9:
({void*_tmp173=0U;({unsigned int _tmp923=loc;struct _dyneither_ptr _tmp922=({const char*_tmp174="missing declarator";_tag_dyneither(_tmp174,sizeof(char),19U);});Cyc_Warn_err(_tmp923,_tmp922,_tag_dyneither(_tmp173,sizeof(void*),0U));});});{struct Cyc_List_List*_tmp175=0;_npop_handler(0U);return _tmp175;};}_LLB7:;}else{
# 930
struct Cyc_List_List*_tmp182=Cyc_Parse_apply_tmss(mkrgn,_tmp19D,_tmp139,declarators,_tmp19B);
if(istypedef){
# 935
if(!exps_empty)
({void*_tmp183=0U;({unsigned int _tmp925=loc;struct _dyneither_ptr _tmp924=({const char*_tmp184="initializer in typedef declaration";_tag_dyneither(_tmp184,sizeof(char),35U);});Cyc_Warn_err(_tmp925,_tmp924,_tag_dyneither(_tmp183,sizeof(void*),0U));});});{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp182);
struct Cyc_List_List*_tmp185=decls;_npop_handler(0U);return _tmp185;};}else{
# 941
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp186=_tmp182;for(0;_tmp186 != 0;(_tmp186=_tmp186->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple15*_tmp187=(struct _tuple15*)_tmp186->hd;struct _tuple15*_tmp188=_tmp187;unsigned int _tmp199;struct _tuple0*_tmp198;struct Cyc_Absyn_Tqual _tmp197;void*_tmp196;struct Cyc_List_List*_tmp195;struct Cyc_List_List*_tmp194;_LLCB: _tmp199=_tmp188->f1;_tmp198=_tmp188->f2;_tmp197=_tmp188->f3;_tmp196=_tmp188->f4;_tmp195=_tmp188->f5;_tmp194=_tmp188->f6;_LLCC:;
if(_tmp195 != 0)
({void*_tmp189=0U;({unsigned int _tmp927=loc;struct _dyneither_ptr _tmp926=({const char*_tmp18A="bad type params, ignoring";_tag_dyneither(_tmp18A,sizeof(char),26U);});Cyc_Warn_warn(_tmp927,_tmp926,_tag_dyneither(_tmp189,sizeof(void*),0U));});});
if(exprs == 0)
({void*_tmp18B=0U;({unsigned int _tmp929=loc;struct _dyneither_ptr _tmp928=({const char*_tmp18C="unexpected NULL in parse!";_tag_dyneither(_tmp18C,sizeof(char),26U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp929,_tmp928,_tag_dyneither(_tmp18B,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*_tmp18D=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp18E=Cyc_Absyn_new_vardecl(_tmp199,_tmp198,_tmp196,_tmp18D);
_tmp18E->tq=_tmp197;
_tmp18E->sc=s;
_tmp18E->attributes=_tmp194;{
struct Cyc_Absyn_Decl*_tmp18F=({struct Cyc_Absyn_Decl*_tmp191=_cycalloc(sizeof(*_tmp191));({void*_tmp92B=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp192=_cycalloc(sizeof(*_tmp192));({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp92A=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp193;_tmp193.tag=0U;_tmp193.f1=_tmp18E;_tmp193;});_tmp192[0]=_tmp92A;});_tmp192;});_tmp191->r=_tmp92B;});_tmp191->loc=loc;_tmp191;});
({struct Cyc_List_List*_tmp92C=({struct Cyc_List_List*_tmp190=_cycalloc(sizeof(*_tmp190));_tmp190->hd=_tmp18F;_tmp190->tl=decls;_tmp190;});decls=_tmp92C;});};};}}{
# 956
struct Cyc_List_List*_tmp19A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0U);return _tmp19A;};}}};};};};}
# 853
;_pop_region(mkrgn);}
# 962
static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,unsigned int loc){
if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || Cyc_strlen((struct _dyneither_ptr)s)== 2){
char _tmp19E=*((const char*)_check_dyneither_subscript(s,sizeof(char),0));char _tmp19F=_tmp19E;switch(_tmp19F){case 65U: _LLCE: _LLCF:
 return& Cyc_Tcutil_ak;case 77U: _LLD0: _LLD1:
 return& Cyc_Tcutil_mk;case 66U: _LLD2: _LLD3:
 return& Cyc_Tcutil_bk;case 82U: _LLD4: _LLD5:
 return& Cyc_Tcutil_rk;case 69U: _LLD6: _LLD7:
 return& Cyc_Tcutil_ek;case 73U: _LLD8: _LLD9:
 return& Cyc_Tcutil_ik;case 85U: _LLDA: _LLDB:
# 972
{char _tmp1A0=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));char _tmp1A1=_tmp1A0;switch(_tmp1A1){case 82U: _LLE1: _LLE2:
 return& Cyc_Tcutil_urk;case 65U: _LLE3: _LLE4:
 return& Cyc_Tcutil_uak;case 77U: _LLE5: _LLE6:
 return& Cyc_Tcutil_umk;case 66U: _LLE7: _LLE8:
 return& Cyc_Tcutil_ubk;default: _LLE9: _LLEA:
 goto _LLE0;}_LLE0:;}
# 979
goto _LLCD;case 84U: _LLDC: _LLDD:
# 981
{char _tmp1A2=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));char _tmp1A3=_tmp1A2;switch(_tmp1A3){case 82U: _LLEC: _LLED:
 return& Cyc_Tcutil_trk;case 65U: _LLEE: _LLEF:
 return& Cyc_Tcutil_tak;case 77U: _LLF0: _LLF1:
 return& Cyc_Tcutil_tmk;case 66U: _LLF2: _LLF3:
 return& Cyc_Tcutil_tbk;default: _LLF4: _LLF5:
 goto _LLEB;}_LLEB:;}
# 988
goto _LLCD;default: _LLDE: _LLDF:
 goto _LLCD;}_LLCD:;}
# 991
({struct Cyc_Int_pa_PrintArg_struct _tmp1A7;_tmp1A7.tag=1U;({unsigned long _tmp92D=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s));_tmp1A7.f1=_tmp92D;});({struct Cyc_String_pa_PrintArg_struct _tmp1A6;_tmp1A6.tag=0U;_tmp1A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp1A4[2U]={& _tmp1A6,& _tmp1A7};({unsigned int _tmp92F=loc;struct _dyneither_ptr _tmp92E=({const char*_tmp1A5="bad kind: %s; strlen=%d";_tag_dyneither(_tmp1A5,sizeof(char),24U);});Cyc_Warn_err(_tmp92F,_tmp92E,_tag_dyneither(_tmp1A4,sizeof(void*),2U));});});});});
return& Cyc_Tcutil_bk;}
# 996
static int Cyc_Parse_exp2int(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp1A8=e->r;void*_tmp1A9=_tmp1A8;int _tmp1AC;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1A9)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1).Int_c).tag == 5U){_LLF7: _tmp1AC=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1).Int_c).val).f2;_LLF8:
 return _tmp1AC;}else{goto _LLF9;}}else{_LLF9: _LLFA:
# 1000
({void*_tmp1AA=0U;({unsigned int _tmp931=loc;struct _dyneither_ptr _tmp930=({const char*_tmp1AB="expecting integer constant";_tag_dyneither(_tmp1AB,sizeof(char),27U);});Cyc_Warn_err(_tmp931,_tmp930,_tag_dyneither(_tmp1AA,sizeof(void*),0U));});});
return 0;}_LLF6:;}
# 1006
static struct _dyneither_ptr Cyc_Parse_exp2string(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp1AD=e->r;void*_tmp1AE=_tmp1AD;struct _dyneither_ptr _tmp1B1;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1AE)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1AE)->f1).String_c).tag == 8U){_LLFC: _tmp1B1=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1AE)->f1).String_c).val;_LLFD:
 return _tmp1B1;}else{goto _LLFE;}}else{_LLFE: _LLFF:
# 1010
({void*_tmp1AF=0U;({unsigned int _tmp933=loc;struct _dyneither_ptr _tmp932=({const char*_tmp1B0="expecting string constant";_tag_dyneither(_tmp1B0,sizeof(char),26U);});Cyc_Warn_err(_tmp933,_tmp932,_tag_dyneither(_tmp1AF,sizeof(void*),0U));});});
return _tag_dyneither(0U,0U,0U);}_LLFB:;}
# 1016
static unsigned int Cyc_Parse_cnst2uint(unsigned int loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp1B2=x;long long _tmp1BA;char _tmp1B9;int _tmp1B8;switch((_tmp1B2.LongLong_c).tag){case 5U: _LL101: _tmp1B8=((_tmp1B2.Int_c).val).f2;_LL102:
 return(unsigned int)_tmp1B8;case 2U: _LL103: _tmp1B9=((_tmp1B2.Char_c).val).f2;_LL104:
 return(unsigned int)_tmp1B9;case 6U: _LL105: _tmp1BA=((_tmp1B2.LongLong_c).val).f2;_LL106: {
# 1021
unsigned long long y=(unsigned long long)_tmp1BA;
if(y > -1)
({void*_tmp1B3=0U;({unsigned int _tmp935=loc;struct _dyneither_ptr _tmp934=({const char*_tmp1B4="integer constant too large";_tag_dyneither(_tmp1B4,sizeof(char),27U);});Cyc_Warn_err(_tmp935,_tmp934,_tag_dyneither(_tmp1B3,sizeof(void*),0U));});});
return(unsigned int)_tmp1BA;}default: _LL107: _LL108:
# 1026
({struct Cyc_String_pa_PrintArg_struct _tmp1B7;_tmp1B7.tag=0U;({struct _dyneither_ptr _tmp936=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(x));_tmp1B7.f1=_tmp936;});({void*_tmp1B5[1U]={& _tmp1B7};({unsigned int _tmp938=loc;struct _dyneither_ptr _tmp937=({const char*_tmp1B6="expected integer constant but found %s";_tag_dyneither(_tmp1B6,sizeof(char),39U);});Cyc_Warn_err(_tmp938,_tmp937,_tag_dyneither(_tmp1B5,sizeof(void*),1U));});});});
return 0U;}_LL100:;}
# 1032
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp1BB=p->r;void*_tmp1BC=_tmp1BB;struct Cyc_Absyn_Exp*_tmp1C9;struct _tuple0*_tmp1C8;struct Cyc_List_List*_tmp1C7;struct _dyneither_ptr _tmp1C6;int _tmp1C5;char _tmp1C4;enum Cyc_Absyn_Sign _tmp1C3;int _tmp1C2;struct Cyc_Absyn_Pat*_tmp1C1;struct Cyc_Absyn_Vardecl*_tmp1C0;struct _tuple0*_tmp1BF;switch(*((int*)_tmp1BC)){case 15U: _LL10A: _tmp1BF=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1BC)->f1;_LL10B:
 return Cyc_Absyn_unknownid_exp(_tmp1BF,p->loc);case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1BC)->f2)->r)->tag == 0U){_LL10C: _tmp1C0=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1BC)->f1;_LL10D:
# 1036
 return({struct Cyc_Absyn_Exp*_tmp939=Cyc_Absyn_unknownid_exp(_tmp1C0->name,p->loc);Cyc_Absyn_deref_exp(_tmp939,p->loc);});}else{goto _LL11C;}case 6U: _LL10E: _tmp1C1=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1BC)->f1;_LL10F:
 return({struct Cyc_Absyn_Exp*_tmp93A=Cyc_Parse_pat2exp(_tmp1C1);Cyc_Absyn_address_exp(_tmp93A,p->loc);});case 9U: _LL110: _LL111:
 return Cyc_Absyn_null_exp(p->loc);case 10U: _LL112: _tmp1C3=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1BC)->f1;_tmp1C2=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1BC)->f2;_LL113:
 return Cyc_Absyn_int_exp(_tmp1C3,_tmp1C2,p->loc);case 11U: _LL114: _tmp1C4=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1BC)->f1;_LL115:
 return Cyc_Absyn_char_exp(_tmp1C4,p->loc);case 12U: _LL116: _tmp1C6=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1BC)->f1;_tmp1C5=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1BC)->f2;_LL117:
 return Cyc_Absyn_float_exp(_tmp1C6,_tmp1C5,p->loc);case 16U: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1BC)->f3 == 0U){_LL118: _tmp1C8=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1BC)->f1;_tmp1C7=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1BC)->f2;_LL119: {
# 1043
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(_tmp1C8,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp1C7);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL11C;}case 17U: _LL11A: _tmp1C9=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1BC)->f1;_LL11B:
 return _tmp1C9;default: _LL11C: _LL11D:
# 1048
({void*_tmp1BD=0U;({unsigned int _tmp93C=p->loc;struct _dyneither_ptr _tmp93B=({const char*_tmp1BE="cannot mix patterns and expressions in case";_tag_dyneither(_tmp1BE,sizeof(char),44U);});Cyc_Warn_err(_tmp93C,_tmp93B,_tag_dyneither(_tmp1BD,sizeof(void*),0U));});});
return Cyc_Absyn_null_exp(p->loc);}_LL109:;}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple20{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple20 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple21{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple22{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Parse_Declarator val;};struct _tuple24{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple25{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp1CB[7U]="cnst_t";
# 1105 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp1CB,_tmp1CB,_tmp1CB + 7U}};
# 1107
union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Cnst yyzzz;
{union Cyc_YYSTYPE _tmp1CC=yy1;union Cyc_Absyn_Cnst _tmp1CD;if((_tmp1CC.Int_tok).tag == 1U){_LL11F: _tmp1CD=(_tmp1CC.Int_tok).val;_LL120:
# 1111
 yyzzz=_tmp1CD;
goto _LL11E;}else{_LL121: _LL122:
(int)_throw((void*)& Cyc_yyfail_Int_tok);}_LL11E:;}
# 1115
return yyzzz;}
# 1117
union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _tmp1CE;(_tmp1CE.Int_tok).val=yy1;(_tmp1CE.Int_tok).tag=1;_tmp1CE;});}static char _tmp1D0[5U]="char";
# 1106 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp1D0,_tmp1D0,_tmp1D0 + 5U}};
# 1108
char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){
char yyzzz;
{union Cyc_YYSTYPE _tmp1D1=yy1;char _tmp1D2;if((_tmp1D1.Char_tok).tag == 2U){_LL124: _tmp1D2=(_tmp1D1.Char_tok).val;_LL125:
# 1112
 yyzzz=_tmp1D2;
goto _LL123;}else{_LL126: _LL127:
(int)_throw((void*)& Cyc_yyfail_Char_tok);}_LL123:;}
# 1116
return yyzzz;}
# 1118
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _tmp1D3;(_tmp1D3.Char_tok).val=yy1;(_tmp1D3.Char_tok).tag=2;_tmp1D3;});}static char _tmp1D5[13U]="string_t<`H>";
# 1107 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp1D5,_tmp1D5,_tmp1D5 + 13U}};
# 1109
struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE yy1){
struct _dyneither_ptr yyzzz;
{union Cyc_YYSTYPE _tmp1D6=yy1;struct _dyneither_ptr _tmp1D7;if((_tmp1D6.String_tok).tag == 3U){_LL129: _tmp1D7=(_tmp1D6.String_tok).val;_LL12A:
# 1113
 yyzzz=_tmp1D7;
goto _LL128;}else{_LL12B: _LL12C:
(int)_throw((void*)& Cyc_yyfail_String_tok);}_LL128:;}
# 1117
return yyzzz;}
# 1119
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){return({union Cyc_YYSTYPE _tmp1D8;(_tmp1D8.String_tok).val=yy1;(_tmp1D8.String_tok).tag=3;_tmp1D8;});}static char _tmp1DA[56U]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
# 1110 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1DA,_tmp1DA,_tmp1DA + 56U}};
# 1112
struct _tuple21*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){
struct _tuple21*yyzzz;
{union Cyc_YYSTYPE _tmp1DB=yy1;struct _tuple21*_tmp1DC;if((_tmp1DB.YY1).tag == 9U){_LL12E: _tmp1DC=(_tmp1DB.YY1).val;_LL12F:
# 1116
 yyzzz=_tmp1DC;
goto _LL12D;}else{_LL130: _LL131:
(int)_throw((void*)& Cyc_yyfail_YY1);}_LL12D:;}
# 1120
return yyzzz;}
# 1122
union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){return({union Cyc_YYSTYPE _tmp1DD;(_tmp1DD.YY1).val=yy1;(_tmp1DD.YY1).tag=9;_tmp1DD;});}static char _tmp1DF[19U]="conref_t<bounds_t>";
# 1111 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp1DF,_tmp1DF,_tmp1DF + 19U}};
# 1113
union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp1E0=yy1;union Cyc_Absyn_Constraint*_tmp1E1;if((_tmp1E0.YY2).tag == 10U){_LL133: _tmp1E1=(_tmp1E0.YY2).val;_LL134:
# 1117
 yyzzz=_tmp1E1;
goto _LL132;}else{_LL135: _LL136:
(int)_throw((void*)& Cyc_yyfail_YY2);}_LL132:;}
# 1121
return yyzzz;}
# 1123
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){return({union Cyc_YYSTYPE _tmp1E2;(_tmp1E2.YY2).val=yy1;(_tmp1E2.YY2).tag=10;_tmp1E2;});}static char _tmp1E4[28U]="list_t<offsetof_field_t,`H>";
# 1112 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp1E4,_tmp1E4,_tmp1E4 + 28U}};
# 1114
struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1E5=yy1;struct Cyc_List_List*_tmp1E6;if((_tmp1E5.YY3).tag == 11U){_LL138: _tmp1E6=(_tmp1E5.YY3).val;_LL139:
# 1118
 yyzzz=_tmp1E6;
goto _LL137;}else{_LL13A: _LL13B:
(int)_throw((void*)& Cyc_yyfail_YY3);}_LL137:;}
# 1122
return yyzzz;}
# 1124
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp1E7;(_tmp1E7.YY3).val=yy1;(_tmp1E7.YY3).tag=11;_tmp1E7;});}static char _tmp1E9[6U]="exp_t";
# 1113 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Exp_tok={Cyc_Core_Failure,{_tmp1E9,_tmp1E9,_tmp1E9 + 6U}};
# 1115
struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp1EA=yy1;struct Cyc_Absyn_Exp*_tmp1EB;if((_tmp1EA.Exp_tok).tag == 7U){_LL13D: _tmp1EB=(_tmp1EA.Exp_tok).val;_LL13E:
# 1119
 yyzzz=_tmp1EB;
goto _LL13C;}else{_LL13F: _LL140:
(int)_throw((void*)& Cyc_yyfail_Exp_tok);}_LL13C:;}
# 1123
return yyzzz;}
# 1125
union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp1EC;(_tmp1EC.Exp_tok).val=yy1;(_tmp1EC.Exp_tok).tag=7;_tmp1EC;});}static char _tmp1EE[17U]="list_t<exp_t,`H>";
# 1121 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1EE,_tmp1EE,_tmp1EE + 17U}};
# 1123
struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1EF=yy1;struct Cyc_List_List*_tmp1F0;if((_tmp1EF.YY4).tag == 12U){_LL142: _tmp1F0=(_tmp1EF.YY4).val;_LL143:
# 1127
 yyzzz=_tmp1F0;
goto _LL141;}else{_LL144: _LL145:
(int)_throw((void*)& Cyc_yyfail_YY4);}_LL141:;}
# 1131
return yyzzz;}
# 1133
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp1F1;(_tmp1F1.YY4).val=yy1;(_tmp1F1.YY4).tag=12;_tmp1F1;});}static char _tmp1F3[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1122 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp1F3,_tmp1F3,_tmp1F3 + 47U}};
# 1124
struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1F4=yy1;struct Cyc_List_List*_tmp1F5;if((_tmp1F4.YY5).tag == 13U){_LL147: _tmp1F5=(_tmp1F4.YY5).val;_LL148:
# 1128
 yyzzz=_tmp1F5;
goto _LL146;}else{_LL149: _LL14A:
(int)_throw((void*)& Cyc_yyfail_YY5);}_LL146:;}
# 1132
return yyzzz;}
# 1134
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp1F6;(_tmp1F6.YY5).val=yy1;(_tmp1F6.YY5).tag=13;_tmp1F6;});}static char _tmp1F8[9U]="primop_t";
# 1123 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1F8,_tmp1F8,_tmp1F8 + 9U}};
# 1125
enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Primop yyzzz;
{union Cyc_YYSTYPE _tmp1F9=yy1;enum Cyc_Absyn_Primop _tmp1FA;if((_tmp1F9.YY6).tag == 14U){_LL14C: _tmp1FA=(_tmp1F9.YY6).val;_LL14D:
# 1129
 yyzzz=_tmp1FA;
goto _LL14B;}else{_LL14E: _LL14F:
(int)_throw((void*)& Cyc_yyfail_YY6);}_LL14B:;}
# 1133
return yyzzz;}
# 1135
union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _tmp1FB;(_tmp1FB.YY6).val=yy1;(_tmp1FB.YY6).tag=14;_tmp1FB;});}static char _tmp1FD[19U]="opt_t<primop_t,`H>";
# 1124 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp1FD,_tmp1FD,_tmp1FD + 19U}};
# 1126
struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;
{union Cyc_YYSTYPE _tmp1FE=yy1;struct Cyc_Core_Opt*_tmp1FF;if((_tmp1FE.YY7).tag == 15U){_LL151: _tmp1FF=(_tmp1FE.YY7).val;_LL152:
# 1130
 yyzzz=_tmp1FF;
goto _LL150;}else{_LL153: _LL154:
(int)_throw((void*)& Cyc_yyfail_YY7);}_LL150:;}
# 1134
return yyzzz;}
# 1136
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _tmp200;(_tmp200.YY7).val=yy1;(_tmp200.YY7).tag=15;_tmp200;});}static char _tmp202[7U]="qvar_t";
# 1125 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp202,_tmp202,_tmp202 + 7U}};
# 1127
struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple0*yyzzz;
{union Cyc_YYSTYPE _tmp203=yy1;struct _tuple0*_tmp204;if((_tmp203.QualId_tok).tag == 5U){_LL156: _tmp204=(_tmp203.QualId_tok).val;_LL157:
# 1131
 yyzzz=_tmp204;
goto _LL155;}else{_LL158: _LL159:
(int)_throw((void*)& Cyc_yyfail_QualId_tok);}_LL155:;}
# 1135
return yyzzz;}
# 1137
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _tmp205;(_tmp205.QualId_tok).val=yy1;(_tmp205.QualId_tok).tag=5;_tmp205;});}static char _tmp207[7U]="stmt_t";
# 1128 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Stmt_tok={Cyc_Core_Failure,{_tmp207,_tmp207,_tmp207 + 7U}};
# 1130
struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Stmt*yyzzz;
{union Cyc_YYSTYPE _tmp208=yy1;struct Cyc_Absyn_Stmt*_tmp209;if((_tmp208.Stmt_tok).tag == 8U){_LL15B: _tmp209=(_tmp208.Stmt_tok).val;_LL15C:
# 1134
 yyzzz=_tmp209;
goto _LL15A;}else{_LL15D: _LL15E:
(int)_throw((void*)& Cyc_yyfail_Stmt_tok);}_LL15A:;}
# 1138
return yyzzz;}
# 1140
union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _tmp20A;(_tmp20A.Stmt_tok).val=yy1;(_tmp20A.Stmt_tok).tag=8;_tmp20A;});}static char _tmp20C[27U]="list_t<switch_clause_t,`H>";
# 1132 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp20C,_tmp20C,_tmp20C + 27U}};
# 1134
struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp20D=yy1;struct Cyc_List_List*_tmp20E;if((_tmp20D.YY8).tag == 16U){_LL160: _tmp20E=(_tmp20D.YY8).val;_LL161:
# 1138
 yyzzz=_tmp20E;
goto _LL15F;}else{_LL162: _LL163:
(int)_throw((void*)& Cyc_yyfail_YY8);}_LL15F:;}
# 1142
return yyzzz;}
# 1144
union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp20F;(_tmp20F.YY8).val=yy1;(_tmp20F.YY8).tag=16;_tmp20F;});}static char _tmp211[6U]="pat_t";
# 1133 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp211,_tmp211,_tmp211 + 6U}};
# 1135
struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Pat*yyzzz;
{union Cyc_YYSTYPE _tmp212=yy1;struct Cyc_Absyn_Pat*_tmp213;if((_tmp212.YY9).tag == 17U){_LL165: _tmp213=(_tmp212.YY9).val;_LL166:
# 1139
 yyzzz=_tmp213;
goto _LL164;}else{_LL167: _LL168:
(int)_throw((void*)& Cyc_yyfail_YY9);}_LL164:;}
# 1143
return yyzzz;}
# 1145
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _tmp214;(_tmp214.YY9).val=yy1;(_tmp214.YY9).tag=17;_tmp214;});}static char _tmp216[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1138 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp216,_tmp216,_tmp216 + 28U}};
# 1140
struct _tuple22*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp217=yy1;struct _tuple22*_tmp218;if((_tmp217.YY10).tag == 18U){_LL16A: _tmp218=(_tmp217.YY10).val;_LL16B:
# 1144
 yyzzz=_tmp218;
goto _LL169;}else{_LL16C: _LL16D:
(int)_throw((void*)& Cyc_yyfail_YY10);}_LL169:;}
# 1148
return yyzzz;}
# 1150
union Cyc_YYSTYPE Cyc_YY10(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp219;(_tmp219.YY10).val=yy1;(_tmp219.YY10).tag=18;_tmp219;});}static char _tmp21B[17U]="list_t<pat_t,`H>";
# 1139 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp21B,_tmp21B,_tmp21B + 17U}};
# 1141
struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp21C=yy1;struct Cyc_List_List*_tmp21D;if((_tmp21C.YY11).tag == 19U){_LL16F: _tmp21D=(_tmp21C.YY11).val;_LL170:
# 1145
 yyzzz=_tmp21D;
goto _LL16E;}else{_LL171: _LL172:
(int)_throw((void*)& Cyc_yyfail_YY11);}_LL16E:;}
# 1149
return yyzzz;}
# 1151
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp21E;(_tmp21E.YY11).val=yy1;(_tmp21E.YY11).tag=19;_tmp21E;});}static char _tmp220[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1140 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp220,_tmp220,_tmp220 + 36U}};
# 1142
struct _tuple23*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){
struct _tuple23*yyzzz;
{union Cyc_YYSTYPE _tmp221=yy1;struct _tuple23*_tmp222;if((_tmp221.YY12).tag == 20U){_LL174: _tmp222=(_tmp221.YY12).val;_LL175:
# 1146
 yyzzz=_tmp222;
goto _LL173;}else{_LL176: _LL177:
(int)_throw((void*)& Cyc_yyfail_YY12);}_LL173:;}
# 1150
return yyzzz;}
# 1152
union Cyc_YYSTYPE Cyc_YY12(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp223;(_tmp223.YY12).val=yy1;(_tmp223.YY12).tag=20;_tmp223;});}static char _tmp225[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1141 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp225,_tmp225,_tmp225 + 47U}};
# 1143
struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp226=yy1;struct Cyc_List_List*_tmp227;if((_tmp226.YY13).tag == 21U){_LL179: _tmp227=(_tmp226.YY13).val;_LL17A:
# 1147
 yyzzz=_tmp227;
goto _LL178;}else{_LL17B: _LL17C:
(int)_throw((void*)& Cyc_yyfail_YY13);}_LL178:;}
# 1151
return yyzzz;}
# 1153
union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp228;(_tmp228.YY13).val=yy1;(_tmp228.YY13).tag=21;_tmp228;});}static char _tmp22A[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1142 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp22A,_tmp22A,_tmp22A + 58U}};
# 1144
struct _tuple22*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp22B=yy1;struct _tuple22*_tmp22C;if((_tmp22B.YY14).tag == 22U){_LL17E: _tmp22C=(_tmp22B.YY14).val;_LL17F:
# 1148
 yyzzz=_tmp22C;
goto _LL17D;}else{_LL180: _LL181:
(int)_throw((void*)& Cyc_yyfail_YY14);}_LL17D:;}
# 1152
return yyzzz;}
# 1154
union Cyc_YYSTYPE Cyc_YY14(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp22D;(_tmp22D.YY14).val=yy1;(_tmp22D.YY14).tag=22;_tmp22D;});}static char _tmp22F[9U]="fndecl_t";
# 1143 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp22F,_tmp22F,_tmp22F + 9U}};
# 1145
struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Fndecl*yyzzz;
{union Cyc_YYSTYPE _tmp230=yy1;struct Cyc_Absyn_Fndecl*_tmp231;if((_tmp230.YY15).tag == 23U){_LL183: _tmp231=(_tmp230.YY15).val;_LL184:
# 1149
 yyzzz=_tmp231;
goto _LL182;}else{_LL185: _LL186:
(int)_throw((void*)& Cyc_yyfail_YY15);}_LL182:;}
# 1153
return yyzzz;}
# 1155
union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _tmp232;(_tmp232.YY15).val=yy1;(_tmp232.YY15).tag=23;_tmp232;});}static char _tmp234[18U]="list_t<decl_t,`H>";
# 1144 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp234,_tmp234,_tmp234 + 18U}};
# 1146
struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp235=yy1;struct Cyc_List_List*_tmp236;if((_tmp235.YY16).tag == 24U){_LL188: _tmp236=(_tmp235.YY16).val;_LL189:
# 1150
 yyzzz=_tmp236;
goto _LL187;}else{_LL18A: _LL18B:
(int)_throw((void*)& Cyc_yyfail_YY16);}_LL187:;}
# 1154
return yyzzz;}
# 1156
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp237;(_tmp237.YY16).val=yy1;(_tmp237.YY16).tag=24;_tmp237;});}static char _tmp239[12U]="decl_spec_t";
# 1146 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp239,_tmp239,_tmp239 + 12U}};
# 1148
struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declaration_spec yyzzz;
{union Cyc_YYSTYPE _tmp23A=yy1;struct Cyc_Parse_Declaration_spec _tmp23B;if((_tmp23A.YY17).tag == 25U){_LL18D: _tmp23B=(_tmp23A.YY17).val;_LL18E:
# 1152
 yyzzz=_tmp23B;
goto _LL18C;}else{_LL18F: _LL190:
(int)_throw((void*)& Cyc_yyfail_YY17);}_LL18C:;}
# 1156
return yyzzz;}
# 1158
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _tmp23C;(_tmp23C.YY17).val=yy1;(_tmp23C.YY17).tag=25;_tmp23C;});}static char _tmp23E[31U]="$(declarator_t<`yy>,exp_opt_t)";
# 1147 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp23E,_tmp23E,_tmp23E + 31U}};
# 1149
struct _tuple11 Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){
struct _tuple11 yyzzz;
{union Cyc_YYSTYPE _tmp23F=yy1;struct _tuple11 _tmp240;if((_tmp23F.YY18).tag == 26U){_LL192: _tmp240=(_tmp23F.YY18).val;_LL193:
# 1153
 yyzzz=_tmp240;
goto _LL191;}else{_LL194: _LL195:
(int)_throw((void*)& Cyc_yyfail_YY18);}_LL191:;}
# 1157
return yyzzz;}
# 1159
union Cyc_YYSTYPE Cyc_YY18(struct _tuple11 yy1){return({union Cyc_YYSTYPE _tmp241;(_tmp241.YY18).val=yy1;(_tmp241.YY18).tag=26;_tmp241;});}static char _tmp243[23U]="declarator_list_t<`yy>";
# 1148 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp243,_tmp243,_tmp243 + 23U}};
# 1150
struct _tuple12*Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){
struct _tuple12*yyzzz;
{union Cyc_YYSTYPE _tmp244=yy1;struct _tuple12*_tmp245;if((_tmp244.YY19).tag == 27U){_LL197: _tmp245=(_tmp244.YY19).val;_LL198:
# 1154
 yyzzz=_tmp245;
goto _LL196;}else{_LL199: _LL19A:
(int)_throw((void*)& Cyc_yyfail_YY19);}_LL196:;}
# 1158
return yyzzz;}
# 1160
union Cyc_YYSTYPE Cyc_YY19(struct _tuple12*yy1){return({union Cyc_YYSTYPE _tmp246;(_tmp246.YY19).val=yy1;(_tmp246.YY19).tag=27;_tmp246;});}static char _tmp248[19U]="storage_class_t@`H";
# 1149 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp248,_tmp248,_tmp248 + 19U}};
# 1151
enum Cyc_Parse_Storage_class*Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){
enum Cyc_Parse_Storage_class*yyzzz;
{union Cyc_YYSTYPE _tmp249=yy1;enum Cyc_Parse_Storage_class*_tmp24A;if((_tmp249.YY20).tag == 28U){_LL19C: _tmp24A=(_tmp249.YY20).val;_LL19D:
# 1155
 yyzzz=_tmp24A;
goto _LL19B;}else{_LL19E: _LL19F:
(int)_throw((void*)& Cyc_yyfail_YY20);}_LL19B:;}
# 1159
return yyzzz;}
# 1161
union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class*yy1){return({union Cyc_YYSTYPE _tmp24B;(_tmp24B.YY20).val=yy1;(_tmp24B.YY20).tag=28;_tmp24B;});}static char _tmp24D[17U]="type_specifier_t";
# 1150 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp24D,_tmp24D,_tmp24D + 17U}};
# 1152
struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Type_specifier yyzzz;
{union Cyc_YYSTYPE _tmp24E=yy1;struct Cyc_Parse_Type_specifier _tmp24F;if((_tmp24E.YY21).tag == 29U){_LL1A1: _tmp24F=(_tmp24E.YY21).val;_LL1A2:
# 1156
 yyzzz=_tmp24F;
goto _LL1A0;}else{_LL1A3: _LL1A4:
(int)_throw((void*)& Cyc_yyfail_YY21);}_LL1A0:;}
# 1160
return yyzzz;}
# 1162
union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _tmp250;(_tmp250.YY21).val=yy1;(_tmp250.YY21).tag=29;_tmp250;});}static char _tmp252[12U]="aggr_kind_t";
# 1152 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp252,_tmp252,_tmp252 + 12U}};
# 1154
enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_AggrKind yyzzz;
{union Cyc_YYSTYPE _tmp253=yy1;enum Cyc_Absyn_AggrKind _tmp254;if((_tmp253.YY22).tag == 30U){_LL1A6: _tmp254=(_tmp253.YY22).val;_LL1A7:
# 1158
 yyzzz=_tmp254;
goto _LL1A5;}else{_LL1A8: _LL1A9:
(int)_throw((void*)& Cyc_yyfail_YY22);}_LL1A5:;}
# 1162
return yyzzz;}
# 1164
union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _tmp255;(_tmp255.YY22).val=yy1;(_tmp255.YY22).tag=30;_tmp255;});}static char _tmp257[8U]="tqual_t";
# 1153 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp257,_tmp257,_tmp257 + 8U}};
# 1155
struct Cyc_Absyn_Tqual Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Tqual yyzzz;
{union Cyc_YYSTYPE _tmp258=yy1;struct Cyc_Absyn_Tqual _tmp259;if((_tmp258.YY23).tag == 31U){_LL1AB: _tmp259=(_tmp258.YY23).val;_LL1AC:
# 1159
 yyzzz=_tmp259;
goto _LL1AA;}else{_LL1AD: _LL1AE:
(int)_throw((void*)& Cyc_yyfail_YY23);}_LL1AA:;}
# 1163
return yyzzz;}
# 1165
union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _tmp25A;(_tmp25A.YY23).val=yy1;(_tmp25A.YY23).tag=31;_tmp25A;});}static char _tmp25C[23U]="list_t<aggrfield_t,`H>";
# 1154 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp25C,_tmp25C,_tmp25C + 23U}};
# 1156
struct Cyc_List_List*Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp25D=yy1;struct Cyc_List_List*_tmp25E;if((_tmp25D.YY24).tag == 32U){_LL1B0: _tmp25E=(_tmp25D.YY24).val;_LL1B1:
# 1160
 yyzzz=_tmp25E;
goto _LL1AF;}else{_LL1B2: _LL1B3:
(int)_throw((void*)& Cyc_yyfail_YY24);}_LL1AF:;}
# 1164
return yyzzz;}
# 1166
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp25F;(_tmp25F.YY24).val=yy1;(_tmp25F.YY24).tag=32;_tmp25F;});}static char _tmp261[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1155 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp261,_tmp261,_tmp261 + 34U}};
# 1157
struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp262=yy1;struct Cyc_List_List*_tmp263;if((_tmp262.YY25).tag == 33U){_LL1B5: _tmp263=(_tmp262.YY25).val;_LL1B6:
# 1161
 yyzzz=_tmp263;
goto _LL1B4;}else{_LL1B7: _LL1B8:
(int)_throw((void*)& Cyc_yyfail_YY25);}_LL1B4:;}
# 1165
return yyzzz;}
# 1167
union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp264;(_tmp264.YY25).val=yy1;(_tmp264.YY25).tag=33;_tmp264;});}static char _tmp266[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1156 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp266,_tmp266,_tmp266 + 33U}};
# 1158
struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp267=yy1;struct Cyc_List_List*_tmp268;if((_tmp267.YY26).tag == 34U){_LL1BA: _tmp268=(_tmp267.YY26).val;_LL1BB:
# 1162
 yyzzz=_tmp268;
goto _LL1B9;}else{_LL1BC: _LL1BD:
(int)_throw((void*)& Cyc_yyfail_YY26);}_LL1B9:;}
# 1166
return yyzzz;}
# 1168
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp269;(_tmp269.YY26).val=yy1;(_tmp269.YY26).tag=34;_tmp269;});}static char _tmp26B[18U]="declarator_t<`yy>";
# 1157 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp26B,_tmp26B,_tmp26B + 18U}};
# 1159
struct Cyc_Parse_Declarator Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declarator yyzzz;
{union Cyc_YYSTYPE _tmp26C=yy1;struct Cyc_Parse_Declarator _tmp26D;if((_tmp26C.YY27).tag == 35U){_LL1BF: _tmp26D=(_tmp26C.YY27).val;_LL1C0:
# 1163
 yyzzz=_tmp26D;
goto _LL1BE;}else{_LL1C1: _LL1C2:
(int)_throw((void*)& Cyc_yyfail_YY27);}_LL1BE:;}
# 1167
return yyzzz;}
# 1169
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _tmp26E;(_tmp26E.YY27).val=yy1;(_tmp26E.YY27).tag=35;_tmp26E;});}static char _tmp270[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1158 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp270,_tmp270,_tmp270 + 45U}};
# 1160
struct _tuple24*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){
struct _tuple24*yyzzz;
{union Cyc_YYSTYPE _tmp271=yy1;struct _tuple24*_tmp272;if((_tmp271.YY28).tag == 36U){_LL1C4: _tmp272=(_tmp271.YY28).val;_LL1C5:
# 1164
 yyzzz=_tmp272;
goto _LL1C3;}else{_LL1C6: _LL1C7:
(int)_throw((void*)& Cyc_yyfail_YY28);}_LL1C3:;}
# 1168
return yyzzz;}
# 1170
union Cyc_YYSTYPE Cyc_YY28(struct _tuple24*yy1){return({union Cyc_YYSTYPE _tmp273;(_tmp273.YY28).val=yy1;(_tmp273.YY28).tag=36;_tmp273;});}static char _tmp275[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1159 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp275,_tmp275,_tmp275 + 57U}};
# 1161
struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp276=yy1;struct Cyc_List_List*_tmp277;if((_tmp276.YY29).tag == 37U){_LL1C9: _tmp277=(_tmp276.YY29).val;_LL1CA:
# 1165
 yyzzz=_tmp277;
goto _LL1C8;}else{_LL1CB: _LL1CC:
(int)_throw((void*)& Cyc_yyfail_YY29);}_LL1C8:;}
# 1169
return yyzzz;}
# 1171
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp278;(_tmp278.YY29).val=yy1;(_tmp278.YY29).tag=37;_tmp278;});}static char _tmp27A[26U]="abstractdeclarator_t<`yy>";
# 1160 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp27A,_tmp27A,_tmp27A + 26U}};
# 1162
struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Abstractdeclarator yyzzz;
{union Cyc_YYSTYPE _tmp27B=yy1;struct Cyc_Parse_Abstractdeclarator _tmp27C;if((_tmp27B.YY30).tag == 38U){_LL1CE: _tmp27C=(_tmp27B.YY30).val;_LL1CF:
# 1166
 yyzzz=_tmp27C;
goto _LL1CD;}else{_LL1D0: _LL1D1:
(int)_throw((void*)& Cyc_yyfail_YY30);}_LL1CD:;}
# 1170
return yyzzz;}
# 1172
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _tmp27D;(_tmp27D.YY30).val=yy1;(_tmp27D.YY30).tag=38;_tmp27D;});}static char _tmp27F[5U]="bool";
# 1161 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp27F,_tmp27F,_tmp27F + 5U}};
# 1163
int Cyc_yyget_YY31(union Cyc_YYSTYPE yy1){
int yyzzz;
{union Cyc_YYSTYPE _tmp280=yy1;int _tmp281;if((_tmp280.YY31).tag == 39U){_LL1D3: _tmp281=(_tmp280.YY31).val;_LL1D4:
# 1167
 yyzzz=_tmp281;
goto _LL1D2;}else{_LL1D5: _LL1D6:
(int)_throw((void*)& Cyc_yyfail_YY31);}_LL1D2:;}
# 1171
return yyzzz;}
# 1173
union Cyc_YYSTYPE Cyc_YY31(int yy1){return({union Cyc_YYSTYPE _tmp282;(_tmp282.YY31).val=yy1;(_tmp282.YY31).tag=39;_tmp282;});}static char _tmp284[8U]="scope_t";
# 1162 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp284,_tmp284,_tmp284 + 8U}};
# 1164
enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;
{union Cyc_YYSTYPE _tmp285=yy1;enum Cyc_Absyn_Scope _tmp286;if((_tmp285.YY32).tag == 40U){_LL1D8: _tmp286=(_tmp285.YY32).val;_LL1D9:
# 1168
 yyzzz=_tmp286;
goto _LL1D7;}else{_LL1DA: _LL1DB:
(int)_throw((void*)& Cyc_yyfail_YY32);}_LL1D7:;}
# 1172
return yyzzz;}
# 1174
union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _tmp287;(_tmp287.YY32).val=yy1;(_tmp287.YY32).tag=40;_tmp287;});}static char _tmp289[16U]="datatypefield_t";
# 1163 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp289,_tmp289,_tmp289 + 16U}};
# 1165
struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Datatypefield*yyzzz;
{union Cyc_YYSTYPE _tmp28A=yy1;struct Cyc_Absyn_Datatypefield*_tmp28B;if((_tmp28A.YY33).tag == 41U){_LL1DD: _tmp28B=(_tmp28A.YY33).val;_LL1DE:
# 1169
 yyzzz=_tmp28B;
goto _LL1DC;}else{_LL1DF: _LL1E0:
(int)_throw((void*)& Cyc_yyfail_YY33);}_LL1DC:;}
# 1173
return yyzzz;}
# 1175
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _tmp28C;(_tmp28C.YY33).val=yy1;(_tmp28C.YY33).tag=41;_tmp28C;});}static char _tmp28E[27U]="list_t<datatypefield_t,`H>";
# 1164 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp28E,_tmp28E,_tmp28E + 27U}};
# 1166
struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp28F=yy1;struct Cyc_List_List*_tmp290;if((_tmp28F.YY34).tag == 42U){_LL1E2: _tmp290=(_tmp28F.YY34).val;_LL1E3:
# 1170
 yyzzz=_tmp290;
goto _LL1E1;}else{_LL1E4: _LL1E5:
(int)_throw((void*)& Cyc_yyfail_YY34);}_LL1E1:;}
# 1174
return yyzzz;}
# 1176
union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp291;(_tmp291.YY34).val=yy1;(_tmp291.YY34).tag=42;_tmp291;});}static char _tmp293[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1165 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp293,_tmp293,_tmp293 + 41U}};
# 1167
struct _tuple25 Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){
struct _tuple25 yyzzz;
{union Cyc_YYSTYPE _tmp294=yy1;struct _tuple25 _tmp295;if((_tmp294.YY35).tag == 43U){_LL1E7: _tmp295=(_tmp294.YY35).val;_LL1E8:
# 1171
 yyzzz=_tmp295;
goto _LL1E6;}else{_LL1E9: _LL1EA:
(int)_throw((void*)& Cyc_yyfail_YY35);}_LL1E6:;}
# 1175
return yyzzz;}
# 1177
union Cyc_YYSTYPE Cyc_YY35(struct _tuple25 yy1){return({union Cyc_YYSTYPE _tmp296;(_tmp296.YY35).val=yy1;(_tmp296.YY35).tag=43;_tmp296;});}static char _tmp298[17U]="list_t<var_t,`H>";
# 1166 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp298,_tmp298,_tmp298 + 17U}};
# 1168
struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp299=yy1;struct Cyc_List_List*_tmp29A;if((_tmp299.YY36).tag == 44U){_LL1EC: _tmp29A=(_tmp299.YY36).val;_LL1ED:
# 1172
 yyzzz=_tmp29A;
goto _LL1EB;}else{_LL1EE: _LL1EF:
(int)_throw((void*)& Cyc_yyfail_YY36);}_LL1EB:;}
# 1176
return yyzzz;}
# 1178
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp29B;(_tmp29B.YY36).val=yy1;(_tmp29B.YY36).tag=44;_tmp29B;});}static char _tmp29D[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1167 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp29D,_tmp29D,_tmp29D + 31U}};
# 1169
struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){
struct _tuple8*yyzzz;
{union Cyc_YYSTYPE _tmp29E=yy1;struct _tuple8*_tmp29F;if((_tmp29E.YY37).tag == 45U){_LL1F1: _tmp29F=(_tmp29E.YY37).val;_LL1F2:
# 1173
 yyzzz=_tmp29F;
goto _LL1F0;}else{_LL1F3: _LL1F4:
(int)_throw((void*)& Cyc_yyfail_YY37);}_LL1F0:;}
# 1177
return yyzzz;}
# 1179
union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){return({union Cyc_YYSTYPE _tmp2A0;(_tmp2A0.YY37).val=yy1;(_tmp2A0.YY37).tag=45;_tmp2A0;});}static char _tmp2A2[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1168 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp2A2,_tmp2A2,_tmp2A2 + 42U}};
# 1170
struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2A3=yy1;struct Cyc_List_List*_tmp2A4;if((_tmp2A3.YY38).tag == 46U){_LL1F6: _tmp2A4=(_tmp2A3.YY38).val;_LL1F7:
# 1174
 yyzzz=_tmp2A4;
goto _LL1F5;}else{_LL1F8: _LL1F9:
(int)_throw((void*)& Cyc_yyfail_YY38);}_LL1F5:;}
# 1178
return yyzzz;}
# 1180
union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2A5;(_tmp2A5.YY38).val=yy1;(_tmp2A5.YY38).tag=46;_tmp2A5;});}static char _tmp2A7[115U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1169 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp2A7,_tmp2A7,_tmp2A7 + 115U}};
# 1171
struct _tuple26*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){
struct _tuple26*yyzzz;
{union Cyc_YYSTYPE _tmp2A8=yy1;struct _tuple26*_tmp2A9;if((_tmp2A8.YY39).tag == 47U){_LL1FB: _tmp2A9=(_tmp2A8.YY39).val;_LL1FC:
# 1175
 yyzzz=_tmp2A9;
goto _LL1FA;}else{_LL1FD: _LL1FE:
(int)_throw((void*)& Cyc_yyfail_YY39);}_LL1FA:;}
# 1179
return yyzzz;}
# 1181
union Cyc_YYSTYPE Cyc_YY39(struct _tuple26*yy1){return({union Cyc_YYSTYPE _tmp2AA;(_tmp2AA.YY39).val=yy1;(_tmp2AA.YY39).tag=47;_tmp2AA;});}static char _tmp2AC[18U]="list_t<type_t,`H>";
# 1170 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp2AC,_tmp2AC,_tmp2AC + 18U}};
# 1172
struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2AD=yy1;struct Cyc_List_List*_tmp2AE;if((_tmp2AD.YY40).tag == 48U){_LL200: _tmp2AE=(_tmp2AD.YY40).val;_LL201:
# 1176
 yyzzz=_tmp2AE;
goto _LL1FF;}else{_LL202: _LL203:
(int)_throw((void*)& Cyc_yyfail_YY40);}_LL1FF:;}
# 1180
return yyzzz;}
# 1182
union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2AF;(_tmp2AF.YY40).val=yy1;(_tmp2AF.YY40).tag=48;_tmp2AF;});}static char _tmp2B1[24U]="list_t<designator_t,`H>";
# 1172 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp2B1,_tmp2B1,_tmp2B1 + 24U}};
# 1174
struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2B2=yy1;struct Cyc_List_List*_tmp2B3;if((_tmp2B2.YY41).tag == 49U){_LL205: _tmp2B3=(_tmp2B2.YY41).val;_LL206:
# 1178
 yyzzz=_tmp2B3;
goto _LL204;}else{_LL207: _LL208:
(int)_throw((void*)& Cyc_yyfail_YY41);}_LL204:;}
# 1182
return yyzzz;}
# 1184
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2B4;(_tmp2B4.YY41).val=yy1;(_tmp2B4.YY41).tag=49;_tmp2B4;});}static char _tmp2B6[13U]="designator_t";
# 1173 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp2B6,_tmp2B6,_tmp2B6 + 13U}};
# 1175
void*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2B7=yy1;void*_tmp2B8;if((_tmp2B7.YY42).tag == 50U){_LL20A: _tmp2B8=(_tmp2B7.YY42).val;_LL20B:
# 1179
 yyzzz=_tmp2B8;
goto _LL209;}else{_LL20C: _LL20D:
(int)_throw((void*)& Cyc_yyfail_YY42);}_LL209:;}
# 1183
return yyzzz;}
# 1185
union Cyc_YYSTYPE Cyc_YY42(void*yy1){return({union Cyc_YYSTYPE _tmp2B9;(_tmp2B9.YY42).val=yy1;(_tmp2B9.YY42).tag=50;_tmp2B9;});}static char _tmp2BB[7U]="kind_t";
# 1174 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp2BB,_tmp2BB,_tmp2BB + 7U}};
# 1176
struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Kind*yyzzz;
{union Cyc_YYSTYPE _tmp2BC=yy1;struct Cyc_Absyn_Kind*_tmp2BD;if((_tmp2BC.YY43).tag == 51U){_LL20F: _tmp2BD=(_tmp2BC.YY43).val;_LL210:
# 1180
 yyzzz=_tmp2BD;
goto _LL20E;}else{_LL211: _LL212:
(int)_throw((void*)& Cyc_yyfail_YY43);}_LL20E:;}
# 1184
return yyzzz;}
# 1186
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _tmp2BE;(_tmp2BE.YY43).val=yy1;(_tmp2BE.YY43).tag=51;_tmp2BE;});}static char _tmp2C0[7U]="type_t";
# 1175 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp2C0,_tmp2C0,_tmp2C0 + 7U}};
# 1177
void*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2C1=yy1;void*_tmp2C2;if((_tmp2C1.YY44).tag == 52U){_LL214: _tmp2C2=(_tmp2C1.YY44).val;_LL215:
# 1181
 yyzzz=_tmp2C2;
goto _LL213;}else{_LL216: _LL217:
(int)_throw((void*)& Cyc_yyfail_YY44);}_LL213:;}
# 1185
return yyzzz;}
# 1187
union Cyc_YYSTYPE Cyc_YY44(void*yy1){return({union Cyc_YYSTYPE _tmp2C3;(_tmp2C3.YY44).val=yy1;(_tmp2C3.YY44).tag=52;_tmp2C3;});}static char _tmp2C5[23U]="list_t<attribute_t,`H>";
# 1176 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp2C5,_tmp2C5,_tmp2C5 + 23U}};
# 1178
struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2C6=yy1;struct Cyc_List_List*_tmp2C7;if((_tmp2C6.YY45).tag == 53U){_LL219: _tmp2C7=(_tmp2C6.YY45).val;_LL21A:
# 1182
 yyzzz=_tmp2C7;
goto _LL218;}else{_LL21B: _LL21C:
(int)_throw((void*)& Cyc_yyfail_YY45);}_LL218:;}
# 1186
return yyzzz;}
# 1188
union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2C8;(_tmp2C8.YY45).val=yy1;(_tmp2C8.YY45).tag=53;_tmp2C8;});}static char _tmp2CA[12U]="attribute_t";
# 1177 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp2CA,_tmp2CA,_tmp2CA + 12U}};
# 1179
void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2CB=yy1;void*_tmp2CC;if((_tmp2CB.YY46).tag == 54U){_LL21E: _tmp2CC=(_tmp2CB.YY46).val;_LL21F:
# 1183
 yyzzz=_tmp2CC;
goto _LL21D;}else{_LL220: _LL221:
(int)_throw((void*)& Cyc_yyfail_YY46);}_LL21D:;}
# 1187
return yyzzz;}
# 1189
union Cyc_YYSTYPE Cyc_YY46(void*yy1){return({union Cyc_YYSTYPE _tmp2CD;(_tmp2CD.YY46).val=yy1;(_tmp2CD.YY46).tag=54;_tmp2CD;});}static char _tmp2CF[12U]="enumfield_t";
# 1178 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp2CF,_tmp2CF,_tmp2CF + 12U}};
# 1180
struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Enumfield*yyzzz;
{union Cyc_YYSTYPE _tmp2D0=yy1;struct Cyc_Absyn_Enumfield*_tmp2D1;if((_tmp2D0.YY47).tag == 55U){_LL223: _tmp2D1=(_tmp2D0.YY47).val;_LL224:
# 1184
 yyzzz=_tmp2D1;
goto _LL222;}else{_LL225: _LL226:
(int)_throw((void*)& Cyc_yyfail_YY47);}_LL222:;}
# 1188
return yyzzz;}
# 1190
union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _tmp2D2;(_tmp2D2.YY47).val=yy1;(_tmp2D2.YY47).tag=55;_tmp2D2;});}static char _tmp2D4[23U]="list_t<enumfield_t,`H>";
# 1179 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp2D4,_tmp2D4,_tmp2D4 + 23U}};
# 1181
struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2D5=yy1;struct Cyc_List_List*_tmp2D6;if((_tmp2D5.YY48).tag == 56U){_LL228: _tmp2D6=(_tmp2D5.YY48).val;_LL229:
# 1185
 yyzzz=_tmp2D6;
goto _LL227;}else{_LL22A: _LL22B:
(int)_throw((void*)& Cyc_yyfail_YY48);}_LL227:;}
# 1189
return yyzzz;}
# 1191
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2D7;(_tmp2D7.YY48).val=yy1;(_tmp2D7.YY48).tag=56;_tmp2D7;});}static char _tmp2D9[11U]="type_opt_t";
# 1180 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp2D9,_tmp2D9,_tmp2D9 + 11U}};
# 1182
void*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2DA=yy1;void*_tmp2DB;if((_tmp2DA.YY49).tag == 57U){_LL22D: _tmp2DB=(_tmp2DA.YY49).val;_LL22E:
# 1186
 yyzzz=_tmp2DB;
goto _LL22C;}else{_LL22F: _LL230:
(int)_throw((void*)& Cyc_yyfail_YY49);}_LL22C:;}
# 1190
return yyzzz;}
# 1192
union Cyc_YYSTYPE Cyc_YY49(void*yy1){return({union Cyc_YYSTYPE _tmp2DC;(_tmp2DC.YY49).val=yy1;(_tmp2DC.YY49).tag=57;_tmp2DC;});}static char _tmp2DE[31U]="list_t<$(type_t,type_t)@`H,`H>";
# 1181 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp2DE,_tmp2DE,_tmp2DE + 31U}};
# 1183
struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2DF=yy1;struct Cyc_List_List*_tmp2E0;if((_tmp2DF.YY50).tag == 58U){_LL232: _tmp2E0=(_tmp2DF.YY50).val;_LL233:
# 1187
 yyzzz=_tmp2E0;
goto _LL231;}else{_LL234: _LL235:
(int)_throw((void*)& Cyc_yyfail_YY50);}_LL231:;}
# 1191
return yyzzz;}
# 1193
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2E1;(_tmp2E1.YY50).val=yy1;(_tmp2E1.YY50).tag=58;_tmp2E1;});}static char _tmp2E3[15U]="conref_t<bool>";
# 1182 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp2E3,_tmp2E3,_tmp2E3 + 15U}};
# 1184
union Cyc_Absyn_Constraint*Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp2E4=yy1;union Cyc_Absyn_Constraint*_tmp2E5;if((_tmp2E4.YY51).tag == 59U){_LL237: _tmp2E5=(_tmp2E4.YY51).val;_LL238:
# 1188
 yyzzz=_tmp2E5;
goto _LL236;}else{_LL239: _LL23A:
(int)_throw((void*)& Cyc_yyfail_YY51);}_LL236:;}
# 1192
return yyzzz;}
# 1194
union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){return({union Cyc_YYSTYPE _tmp2E6;(_tmp2E6.YY51).val=yy1;(_tmp2E6.YY51).tag=59;_tmp2E6;});}static char _tmp2E8[45U]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1183 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp2E8,_tmp2E8,_tmp2E8 + 45U}};
# 1185
struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2E9=yy1;struct Cyc_List_List*_tmp2EA;if((_tmp2E9.YY52).tag == 60U){_LL23C: _tmp2EA=(_tmp2E9.YY52).val;_LL23D:
# 1189
 yyzzz=_tmp2EA;
goto _LL23B;}else{_LL23E: _LL23F:
(int)_throw((void*)& Cyc_yyfail_YY52);}_LL23B:;}
# 1193
return yyzzz;}
# 1195
union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2EB;(_tmp2EB.YY52).val=yy1;(_tmp2EB.YY52).tag=60;_tmp2EB;});}static char _tmp2ED[20U]="pointer_qual_t<`yy>";
# 1184 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY53={Cyc_Core_Failure,{_tmp2ED,_tmp2ED,_tmp2ED + 20U}};
# 1186
void*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2EE=yy1;void*_tmp2EF;if((_tmp2EE.YY53).tag == 61U){_LL241: _tmp2EF=(_tmp2EE.YY53).val;_LL242:
# 1190
 yyzzz=_tmp2EF;
goto _LL240;}else{_LL243: _LL244:
(int)_throw((void*)& Cyc_yyfail_YY53);}_LL240:;}
# 1194
return yyzzz;}
# 1196
union Cyc_YYSTYPE Cyc_YY53(void*yy1){return({union Cyc_YYSTYPE _tmp2F0;(_tmp2F0.YY53).val=yy1;(_tmp2F0.YY53).tag=61;_tmp2F0;});}static char _tmp2F2[21U]="pointer_quals_t<`yy>";
# 1185 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp2F2,_tmp2F2,_tmp2F2 + 21U}};
# 1187
struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2F3=yy1;struct Cyc_List_List*_tmp2F4;if((_tmp2F3.YY54).tag == 62U){_LL246: _tmp2F4=(_tmp2F3.YY54).val;_LL247:
# 1191
 yyzzz=_tmp2F4;
goto _LL245;}else{_LL248: _LL249:
(int)_throw((void*)& Cyc_yyfail_YY54);}_LL245:;}
# 1195
return yyzzz;}
# 1197
union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2F5;(_tmp2F5.YY54).val=yy1;(_tmp2F5.YY54).tag=62;_tmp2F5;});}static char _tmp2F7[21U]="$(bool,string_t<`H>)";
# 1186 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp2F7,_tmp2F7,_tmp2F7 + 21U}};
# 1188
struct _tuple20 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){
struct _tuple20 yyzzz;
{union Cyc_YYSTYPE _tmp2F8=yy1;struct _tuple20 _tmp2F9;if((_tmp2F8.Asm_tok).tag == 6U){_LL24B: _tmp2F9=(_tmp2F8.Asm_tok).val;_LL24C:
# 1192
 yyzzz=_tmp2F9;
goto _LL24A;}else{_LL24D: _LL24E:
(int)_throw((void*)& Cyc_yyfail_Asm_tok);}_LL24A:;}
# 1196
return yyzzz;}
# 1198
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){return({union Cyc_YYSTYPE _tmp2FA;(_tmp2FA.Asm_tok).val=yy1;(_tmp2FA.Asm_tok).tag=6;_tmp2FA;});}static char _tmp2FC[10U]="exp_opt_t";
# 1187 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY55={Cyc_Core_Failure,{_tmp2FC,_tmp2FC,_tmp2FC + 10U}};
# 1189
struct Cyc_Absyn_Exp*Cyc_yyget_YY55(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp2FD=yy1;struct Cyc_Absyn_Exp*_tmp2FE;if((_tmp2FD.YY55).tag == 63U){_LL250: _tmp2FE=(_tmp2FD.YY55).val;_LL251:
# 1193
 yyzzz=_tmp2FE;
goto _LL24F;}else{_LL252: _LL253:
(int)_throw((void*)& Cyc_yyfail_YY55);}_LL24F:;}
# 1197
return yyzzz;}
# 1199
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp2FF;(_tmp2FF.YY55).val=yy1;(_tmp2FF.YY55).tag=63;_tmp2FF;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1211
typedef struct Cyc_Yyltype Cyc_yyltype;
# 1215
struct Cyc_Yyltype Cyc_yynewloc(){
return({struct Cyc_Yyltype _tmp300;_tmp300.timestamp=0;_tmp300.first_line=0;_tmp300.first_column=0;_tmp300.last_line=0;_tmp300.last_column=0;_tmp300;});}
# 1218
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1229 "parse.y"
static short Cyc_yytranslate[375U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,146,2,2,130,144,141,2,127,128,135,138,123,142,132,143,2,2,2,2,2,2,2,2,2,2,131,120,125,124,126,137,136,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,133,2,134,140,129,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,121,139,122,145,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119};static char _tmp301[2U]="$";static char _tmp302[6U]="error";static char _tmp303[12U]="$undefined.";static char _tmp304[5U]="AUTO";static char _tmp305[9U]="REGISTER";static char _tmp306[7U]="STATIC";static char _tmp307[7U]="EXTERN";static char _tmp308[8U]="TYPEDEF";static char _tmp309[5U]="VOID";static char _tmp30A[5U]="CHAR";static char _tmp30B[6U]="SHORT";static char _tmp30C[4U]="INT";static char _tmp30D[5U]="LONG";static char _tmp30E[6U]="FLOAT";static char _tmp30F[7U]="DOUBLE";static char _tmp310[7U]="SIGNED";static char _tmp311[9U]="UNSIGNED";static char _tmp312[6U]="CONST";static char _tmp313[9U]="VOLATILE";static char _tmp314[9U]="RESTRICT";static char _tmp315[7U]="STRUCT";static char _tmp316[6U]="UNION";static char _tmp317[5U]="CASE";static char _tmp318[8U]="DEFAULT";static char _tmp319[7U]="INLINE";static char _tmp31A[7U]="SIZEOF";static char _tmp31B[9U]="OFFSETOF";static char _tmp31C[3U]="IF";static char _tmp31D[5U]="ELSE";static char _tmp31E[7U]="SWITCH";static char _tmp31F[6U]="WHILE";static char _tmp320[3U]="DO";static char _tmp321[4U]="FOR";static char _tmp322[5U]="GOTO";static char _tmp323[9U]="CONTINUE";static char _tmp324[6U]="BREAK";static char _tmp325[7U]="RETURN";static char _tmp326[5U]="ENUM";static char _tmp327[7U]="TYPEOF";static char _tmp328[16U]="BUILTIN_VA_LIST";static char _tmp329[8U]="NULL_kw";static char _tmp32A[4U]="LET";static char _tmp32B[6U]="THROW";static char _tmp32C[4U]="TRY";static char _tmp32D[6U]="CATCH";static char _tmp32E[7U]="EXPORT";static char _tmp32F[4U]="NEW";static char _tmp330[9U]="ABSTRACT";static char _tmp331[9U]="FALLTHRU";static char _tmp332[6U]="USING";static char _tmp333[10U]="NAMESPACE";static char _tmp334[9U]="DATATYPE";static char _tmp335[7U]="MALLOC";static char _tmp336[8U]="RMALLOC";static char _tmp337[15U]="RMALLOC_INLINE";static char _tmp338[7U]="CALLOC";static char _tmp339[8U]="RCALLOC";static char _tmp33A[5U]="SWAP";static char _tmp33B[9U]="REGION_T";static char _tmp33C[6U]="TAG_T";static char _tmp33D[7U]="REGION";static char _tmp33E[5U]="RNEW";static char _tmp33F[8U]="REGIONS";static char _tmp340[7U]="PORTON";static char _tmp341[8U]="PORTOFF";static char _tmp342[7U]="PRAGMA";static char _tmp343[12U]="DYNREGION_T";static char _tmp344[8U]="NUMELTS";static char _tmp345[8U]="VALUEOF";static char _tmp346[10U]="VALUEOF_T";static char _tmp347[9U]="TAGCHECK";static char _tmp348[13U]="NUMELTS_QUAL";static char _tmp349[10U]="THIN_QUAL";static char _tmp34A[9U]="FAT_QUAL";static char _tmp34B[13U]="NOTNULL_QUAL";static char _tmp34C[14U]="NULLABLE_QUAL";static char _tmp34D[14U]="REQUIRES_QUAL";static char _tmp34E[13U]="ENSURES_QUAL";static char _tmp34F[12U]="REGION_QUAL";static char _tmp350[16U]="NOZEROTERM_QUAL";static char _tmp351[14U]="ZEROTERM_QUAL";static char _tmp352[12U]="TAGGED_QUAL";static char _tmp353[16U]="EXTENSIBLE_QUAL";static char _tmp354[7U]="PTR_OP";static char _tmp355[7U]="INC_OP";static char _tmp356[7U]="DEC_OP";static char _tmp357[8U]="LEFT_OP";static char _tmp358[9U]="RIGHT_OP";static char _tmp359[6U]="LE_OP";static char _tmp35A[6U]="GE_OP";static char _tmp35B[6U]="EQ_OP";static char _tmp35C[6U]="NE_OP";static char _tmp35D[7U]="AND_OP";static char _tmp35E[6U]="OR_OP";static char _tmp35F[11U]="MUL_ASSIGN";static char _tmp360[11U]="DIV_ASSIGN";static char _tmp361[11U]="MOD_ASSIGN";static char _tmp362[11U]="ADD_ASSIGN";static char _tmp363[11U]="SUB_ASSIGN";static char _tmp364[12U]="LEFT_ASSIGN";static char _tmp365[13U]="RIGHT_ASSIGN";static char _tmp366[11U]="AND_ASSIGN";static char _tmp367[11U]="XOR_ASSIGN";static char _tmp368[10U]="OR_ASSIGN";static char _tmp369[9U]="ELLIPSIS";static char _tmp36A[11U]="LEFT_RIGHT";static char _tmp36B[12U]="COLON_COLON";static char _tmp36C[11U]="IDENTIFIER";static char _tmp36D[17U]="INTEGER_CONSTANT";static char _tmp36E[7U]="STRING";static char _tmp36F[8U]="WSTRING";static char _tmp370[19U]="CHARACTER_CONSTANT";static char _tmp371[20U]="WCHARACTER_CONSTANT";static char _tmp372[18U]="FLOATING_CONSTANT";static char _tmp373[9U]="TYPE_VAR";static char _tmp374[13U]="TYPEDEF_NAME";static char _tmp375[16U]="QUAL_IDENTIFIER";static char _tmp376[18U]="QUAL_TYPEDEF_NAME";static char _tmp377[10U]="ATTRIBUTE";static char _tmp378[4U]="ASM";static char _tmp379[4U]="';'";static char _tmp37A[4U]="'{'";static char _tmp37B[4U]="'}'";static char _tmp37C[4U]="','";static char _tmp37D[4U]="'='";static char _tmp37E[4U]="'<'";static char _tmp37F[4U]="'>'";static char _tmp380[4U]="'('";static char _tmp381[4U]="')'";static char _tmp382[4U]="'_'";static char _tmp383[4U]="'$'";static char _tmp384[4U]="':'";static char _tmp385[4U]="'.'";static char _tmp386[4U]="'['";static char _tmp387[4U]="']'";static char _tmp388[4U]="'*'";static char _tmp389[4U]="'@'";static char _tmp38A[4U]="'?'";static char _tmp38B[4U]="'+'";static char _tmp38C[4U]="'|'";static char _tmp38D[4U]="'^'";static char _tmp38E[4U]="'&'";static char _tmp38F[4U]="'-'";static char _tmp390[4U]="'/'";static char _tmp391[4U]="'%'";static char _tmp392[4U]="'~'";static char _tmp393[4U]="'!'";static char _tmp394[5U]="prog";static char _tmp395[17U]="translation_unit";static char _tmp396[16U]="extern_c_action";static char _tmp397[13U]="end_extern_c";static char _tmp398[12U]="export_list";static char _tmp399[19U]="export_list_values";static char _tmp39A[21U]="external_declaration";static char _tmp39B[15U]="optional_comma";static char _tmp39C[20U]="function_definition";static char _tmp39D[21U]="function_definition2";static char _tmp39E[13U]="using_action";static char _tmp39F[15U]="unusing_action";static char _tmp3A0[17U]="namespace_action";static char _tmp3A1[19U]="unnamespace_action";static char _tmp3A2[12U]="declaration";static char _tmp3A3[17U]="declaration_list";static char _tmp3A4[23U]="declaration_specifiers";static char _tmp3A5[24U]="storage_class_specifier";static char _tmp3A6[15U]="attributes_opt";static char _tmp3A7[11U]="attributes";static char _tmp3A8[15U]="attribute_list";static char _tmp3A9[10U]="attribute";static char _tmp3AA[15U]="type_specifier";static char _tmp3AB[25U]="type_specifier_notypedef";static char _tmp3AC[5U]="kind";static char _tmp3AD[15U]="type_qualifier";static char _tmp3AE[15U]="enum_specifier";static char _tmp3AF[11U]="enum_field";static char _tmp3B0[22U]="enum_declaration_list";static char _tmp3B1[26U]="struct_or_union_specifier";static char _tmp3B2[16U]="type_params_opt";static char _tmp3B3[16U]="struct_or_union";static char _tmp3B4[24U]="struct_declaration_list";static char _tmp3B5[25U]="struct_declaration_list0";static char _tmp3B6[21U]="init_declarator_list";static char _tmp3B7[22U]="init_declarator_list0";static char _tmp3B8[16U]="init_declarator";static char _tmp3B9[19U]="struct_declaration";static char _tmp3BA[25U]="specifier_qualifier_list";static char _tmp3BB[35U]="notypedef_specifier_qualifier_list";static char _tmp3BC[23U]="struct_declarator_list";static char _tmp3BD[24U]="struct_declarator_list0";static char _tmp3BE[18U]="struct_declarator";static char _tmp3BF[20U]="requires_clause_opt";static char _tmp3C0[19U]="ensures_clause_opt";static char _tmp3C1[19U]="datatype_specifier";static char _tmp3C2[14U]="qual_datatype";static char _tmp3C3[19U]="datatypefield_list";static char _tmp3C4[20U]="datatypefield_scope";static char _tmp3C5[14U]="datatypefield";static char _tmp3C6[11U]="declarator";static char _tmp3C7[23U]="declarator_withtypedef";static char _tmp3C8[18U]="direct_declarator";static char _tmp3C9[30U]="direct_declarator_withtypedef";static char _tmp3CA[8U]="pointer";static char _tmp3CB[12U]="one_pointer";static char _tmp3CC[14U]="pointer_quals";static char _tmp3CD[13U]="pointer_qual";static char _tmp3CE[23U]="pointer_null_and_bound";static char _tmp3CF[14U]="pointer_bound";static char _tmp3D0[18U]="zeroterm_qual_opt";static char _tmp3D1[8U]="rgn_opt";static char _tmp3D2[11U]="tqual_list";static char _tmp3D3[20U]="parameter_type_list";static char _tmp3D4[9U]="type_var";static char _tmp3D5[16U]="optional_effect";static char _tmp3D6[19U]="optional_rgn_order";static char _tmp3D7[10U]="rgn_order";static char _tmp3D8[16U]="optional_inject";static char _tmp3D9[11U]="effect_set";static char _tmp3DA[14U]="atomic_effect";static char _tmp3DB[11U]="region_set";static char _tmp3DC[15U]="parameter_list";static char _tmp3DD[22U]="parameter_declaration";static char _tmp3DE[16U]="identifier_list";static char _tmp3DF[17U]="identifier_list0";static char _tmp3E0[12U]="initializer";static char _tmp3E1[18U]="array_initializer";static char _tmp3E2[17U]="initializer_list";static char _tmp3E3[12U]="designation";static char _tmp3E4[16U]="designator_list";static char _tmp3E5[11U]="designator";static char _tmp3E6[10U]="type_name";static char _tmp3E7[14U]="any_type_name";static char _tmp3E8[15U]="type_name_list";static char _tmp3E9[20U]="abstract_declarator";static char _tmp3EA[27U]="direct_abstract_declarator";static char _tmp3EB[10U]="statement";static char _tmp3EC[18U]="labeled_statement";static char _tmp3ED[21U]="expression_statement";static char _tmp3EE[19U]="compound_statement";static char _tmp3EF[16U]="block_item_list";static char _tmp3F0[20U]="selection_statement";static char _tmp3F1[15U]="switch_clauses";static char _tmp3F2[20U]="iteration_statement";static char _tmp3F3[15U]="jump_statement";static char _tmp3F4[12U]="exp_pattern";static char _tmp3F5[20U]="conditional_pattern";static char _tmp3F6[19U]="logical_or_pattern";static char _tmp3F7[20U]="logical_and_pattern";static char _tmp3F8[21U]="inclusive_or_pattern";static char _tmp3F9[21U]="exclusive_or_pattern";static char _tmp3FA[12U]="and_pattern";static char _tmp3FB[17U]="equality_pattern";static char _tmp3FC[19U]="relational_pattern";static char _tmp3FD[14U]="shift_pattern";static char _tmp3FE[17U]="additive_pattern";static char _tmp3FF[23U]="multiplicative_pattern";static char _tmp400[13U]="cast_pattern";static char _tmp401[14U]="unary_pattern";static char _tmp402[16U]="postfix_pattern";static char _tmp403[16U]="primary_pattern";static char _tmp404[8U]="pattern";static char _tmp405[19U]="tuple_pattern_list";static char _tmp406[20U]="tuple_pattern_list0";static char _tmp407[14U]="field_pattern";static char _tmp408[19U]="field_pattern_list";static char _tmp409[20U]="field_pattern_list0";static char _tmp40A[11U]="expression";static char _tmp40B[22U]="assignment_expression";static char _tmp40C[20U]="assignment_operator";static char _tmp40D[23U]="conditional_expression";static char _tmp40E[20U]="constant_expression";static char _tmp40F[22U]="logical_or_expression";static char _tmp410[23U]="logical_and_expression";static char _tmp411[24U]="inclusive_or_expression";static char _tmp412[24U]="exclusive_or_expression";static char _tmp413[15U]="and_expression";static char _tmp414[20U]="equality_expression";static char _tmp415[22U]="relational_expression";static char _tmp416[17U]="shift_expression";static char _tmp417[20U]="additive_expression";static char _tmp418[26U]="multiplicative_expression";static char _tmp419[16U]="cast_expression";static char _tmp41A[17U]="unary_expression";static char _tmp41B[15U]="unary_operator";static char _tmp41C[19U]="postfix_expression";static char _tmp41D[17U]="field_expression";static char _tmp41E[19U]="primary_expression";static char _tmp41F[25U]="argument_expression_list";static char _tmp420[26U]="argument_expression_list0";static char _tmp421[9U]="constant";static char _tmp422[20U]="qual_opt_identifier";static char _tmp423[17U]="qual_opt_typedef";static char _tmp424[18U]="struct_union_name";static char _tmp425[11U]="field_name";static char _tmp426[12U]="right_angle";
# 1583 "parse.y"
static struct _dyneither_ptr Cyc_yytname[294U]={{_tmp301,_tmp301,_tmp301 + 2U},{_tmp302,_tmp302,_tmp302 + 6U},{_tmp303,_tmp303,_tmp303 + 12U},{_tmp304,_tmp304,_tmp304 + 5U},{_tmp305,_tmp305,_tmp305 + 9U},{_tmp306,_tmp306,_tmp306 + 7U},{_tmp307,_tmp307,_tmp307 + 7U},{_tmp308,_tmp308,_tmp308 + 8U},{_tmp309,_tmp309,_tmp309 + 5U},{_tmp30A,_tmp30A,_tmp30A + 5U},{_tmp30B,_tmp30B,_tmp30B + 6U},{_tmp30C,_tmp30C,_tmp30C + 4U},{_tmp30D,_tmp30D,_tmp30D + 5U},{_tmp30E,_tmp30E,_tmp30E + 6U},{_tmp30F,_tmp30F,_tmp30F + 7U},{_tmp310,_tmp310,_tmp310 + 7U},{_tmp311,_tmp311,_tmp311 + 9U},{_tmp312,_tmp312,_tmp312 + 6U},{_tmp313,_tmp313,_tmp313 + 9U},{_tmp314,_tmp314,_tmp314 + 9U},{_tmp315,_tmp315,_tmp315 + 7U},{_tmp316,_tmp316,_tmp316 + 6U},{_tmp317,_tmp317,_tmp317 + 5U},{_tmp318,_tmp318,_tmp318 + 8U},{_tmp319,_tmp319,_tmp319 + 7U},{_tmp31A,_tmp31A,_tmp31A + 7U},{_tmp31B,_tmp31B,_tmp31B + 9U},{_tmp31C,_tmp31C,_tmp31C + 3U},{_tmp31D,_tmp31D,_tmp31D + 5U},{_tmp31E,_tmp31E,_tmp31E + 7U},{_tmp31F,_tmp31F,_tmp31F + 6U},{_tmp320,_tmp320,_tmp320 + 3U},{_tmp321,_tmp321,_tmp321 + 4U},{_tmp322,_tmp322,_tmp322 + 5U},{_tmp323,_tmp323,_tmp323 + 9U},{_tmp324,_tmp324,_tmp324 + 6U},{_tmp325,_tmp325,_tmp325 + 7U},{_tmp326,_tmp326,_tmp326 + 5U},{_tmp327,_tmp327,_tmp327 + 7U},{_tmp328,_tmp328,_tmp328 + 16U},{_tmp329,_tmp329,_tmp329 + 8U},{_tmp32A,_tmp32A,_tmp32A + 4U},{_tmp32B,_tmp32B,_tmp32B + 6U},{_tmp32C,_tmp32C,_tmp32C + 4U},{_tmp32D,_tmp32D,_tmp32D + 6U},{_tmp32E,_tmp32E,_tmp32E + 7U},{_tmp32F,_tmp32F,_tmp32F + 4U},{_tmp330,_tmp330,_tmp330 + 9U},{_tmp331,_tmp331,_tmp331 + 9U},{_tmp332,_tmp332,_tmp332 + 6U},{_tmp333,_tmp333,_tmp333 + 10U},{_tmp334,_tmp334,_tmp334 + 9U},{_tmp335,_tmp335,_tmp335 + 7U},{_tmp336,_tmp336,_tmp336 + 8U},{_tmp337,_tmp337,_tmp337 + 15U},{_tmp338,_tmp338,_tmp338 + 7U},{_tmp339,_tmp339,_tmp339 + 8U},{_tmp33A,_tmp33A,_tmp33A + 5U},{_tmp33B,_tmp33B,_tmp33B + 9U},{_tmp33C,_tmp33C,_tmp33C + 6U},{_tmp33D,_tmp33D,_tmp33D + 7U},{_tmp33E,_tmp33E,_tmp33E + 5U},{_tmp33F,_tmp33F,_tmp33F + 8U},{_tmp340,_tmp340,_tmp340 + 7U},{_tmp341,_tmp341,_tmp341 + 8U},{_tmp342,_tmp342,_tmp342 + 7U},{_tmp343,_tmp343,_tmp343 + 12U},{_tmp344,_tmp344,_tmp344 + 8U},{_tmp345,_tmp345,_tmp345 + 8U},{_tmp346,_tmp346,_tmp346 + 10U},{_tmp347,_tmp347,_tmp347 + 9U},{_tmp348,_tmp348,_tmp348 + 13U},{_tmp349,_tmp349,_tmp349 + 10U},{_tmp34A,_tmp34A,_tmp34A + 9U},{_tmp34B,_tmp34B,_tmp34B + 13U},{_tmp34C,_tmp34C,_tmp34C + 14U},{_tmp34D,_tmp34D,_tmp34D + 14U},{_tmp34E,_tmp34E,_tmp34E + 13U},{_tmp34F,_tmp34F,_tmp34F + 12U},{_tmp350,_tmp350,_tmp350 + 16U},{_tmp351,_tmp351,_tmp351 + 14U},{_tmp352,_tmp352,_tmp352 + 12U},{_tmp353,_tmp353,_tmp353 + 16U},{_tmp354,_tmp354,_tmp354 + 7U},{_tmp355,_tmp355,_tmp355 + 7U},{_tmp356,_tmp356,_tmp356 + 7U},{_tmp357,_tmp357,_tmp357 + 8U},{_tmp358,_tmp358,_tmp358 + 9U},{_tmp359,_tmp359,_tmp359 + 6U},{_tmp35A,_tmp35A,_tmp35A + 6U},{_tmp35B,_tmp35B,_tmp35B + 6U},{_tmp35C,_tmp35C,_tmp35C + 6U},{_tmp35D,_tmp35D,_tmp35D + 7U},{_tmp35E,_tmp35E,_tmp35E + 6U},{_tmp35F,_tmp35F,_tmp35F + 11U},{_tmp360,_tmp360,_tmp360 + 11U},{_tmp361,_tmp361,_tmp361 + 11U},{_tmp362,_tmp362,_tmp362 + 11U},{_tmp363,_tmp363,_tmp363 + 11U},{_tmp364,_tmp364,_tmp364 + 12U},{_tmp365,_tmp365,_tmp365 + 13U},{_tmp366,_tmp366,_tmp366 + 11U},{_tmp367,_tmp367,_tmp367 + 11U},{_tmp368,_tmp368,_tmp368 + 10U},{_tmp369,_tmp369,_tmp369 + 9U},{_tmp36A,_tmp36A,_tmp36A + 11U},{_tmp36B,_tmp36B,_tmp36B + 12U},{_tmp36C,_tmp36C,_tmp36C + 11U},{_tmp36D,_tmp36D,_tmp36D + 17U},{_tmp36E,_tmp36E,_tmp36E + 7U},{_tmp36F,_tmp36F,_tmp36F + 8U},{_tmp370,_tmp370,_tmp370 + 19U},{_tmp371,_tmp371,_tmp371 + 20U},{_tmp372,_tmp372,_tmp372 + 18U},{_tmp373,_tmp373,_tmp373 + 9U},{_tmp374,_tmp374,_tmp374 + 13U},{_tmp375,_tmp375,_tmp375 + 16U},{_tmp376,_tmp376,_tmp376 + 18U},{_tmp377,_tmp377,_tmp377 + 10U},{_tmp378,_tmp378,_tmp378 + 4U},{_tmp379,_tmp379,_tmp379 + 4U},{_tmp37A,_tmp37A,_tmp37A + 4U},{_tmp37B,_tmp37B,_tmp37B + 4U},{_tmp37C,_tmp37C,_tmp37C + 4U},{_tmp37D,_tmp37D,_tmp37D + 4U},{_tmp37E,_tmp37E,_tmp37E + 4U},{_tmp37F,_tmp37F,_tmp37F + 4U},{_tmp380,_tmp380,_tmp380 + 4U},{_tmp381,_tmp381,_tmp381 + 4U},{_tmp382,_tmp382,_tmp382 + 4U},{_tmp383,_tmp383,_tmp383 + 4U},{_tmp384,_tmp384,_tmp384 + 4U},{_tmp385,_tmp385,_tmp385 + 4U},{_tmp386,_tmp386,_tmp386 + 4U},{_tmp387,_tmp387,_tmp387 + 4U},{_tmp388,_tmp388,_tmp388 + 4U},{_tmp389,_tmp389,_tmp389 + 4U},{_tmp38A,_tmp38A,_tmp38A + 4U},{_tmp38B,_tmp38B,_tmp38B + 4U},{_tmp38C,_tmp38C,_tmp38C + 4U},{_tmp38D,_tmp38D,_tmp38D + 4U},{_tmp38E,_tmp38E,_tmp38E + 4U},{_tmp38F,_tmp38F,_tmp38F + 4U},{_tmp390,_tmp390,_tmp390 + 4U},{_tmp391,_tmp391,_tmp391 + 4U},{_tmp392,_tmp392,_tmp392 + 4U},{_tmp393,_tmp393,_tmp393 + 4U},{_tmp394,_tmp394,_tmp394 + 5U},{_tmp395,_tmp395,_tmp395 + 17U},{_tmp396,_tmp396,_tmp396 + 16U},{_tmp397,_tmp397,_tmp397 + 13U},{_tmp398,_tmp398,_tmp398 + 12U},{_tmp399,_tmp399,_tmp399 + 19U},{_tmp39A,_tmp39A,_tmp39A + 21U},{_tmp39B,_tmp39B,_tmp39B + 15U},{_tmp39C,_tmp39C,_tmp39C + 20U},{_tmp39D,_tmp39D,_tmp39D + 21U},{_tmp39E,_tmp39E,_tmp39E + 13U},{_tmp39F,_tmp39F,_tmp39F + 15U},{_tmp3A0,_tmp3A0,_tmp3A0 + 17U},{_tmp3A1,_tmp3A1,_tmp3A1 + 19U},{_tmp3A2,_tmp3A2,_tmp3A2 + 12U},{_tmp3A3,_tmp3A3,_tmp3A3 + 17U},{_tmp3A4,_tmp3A4,_tmp3A4 + 23U},{_tmp3A5,_tmp3A5,_tmp3A5 + 24U},{_tmp3A6,_tmp3A6,_tmp3A6 + 15U},{_tmp3A7,_tmp3A7,_tmp3A7 + 11U},{_tmp3A8,_tmp3A8,_tmp3A8 + 15U},{_tmp3A9,_tmp3A9,_tmp3A9 + 10U},{_tmp3AA,_tmp3AA,_tmp3AA + 15U},{_tmp3AB,_tmp3AB,_tmp3AB + 25U},{_tmp3AC,_tmp3AC,_tmp3AC + 5U},{_tmp3AD,_tmp3AD,_tmp3AD + 15U},{_tmp3AE,_tmp3AE,_tmp3AE + 15U},{_tmp3AF,_tmp3AF,_tmp3AF + 11U},{_tmp3B0,_tmp3B0,_tmp3B0 + 22U},{_tmp3B1,_tmp3B1,_tmp3B1 + 26U},{_tmp3B2,_tmp3B2,_tmp3B2 + 16U},{_tmp3B3,_tmp3B3,_tmp3B3 + 16U},{_tmp3B4,_tmp3B4,_tmp3B4 + 24U},{_tmp3B5,_tmp3B5,_tmp3B5 + 25U},{_tmp3B6,_tmp3B6,_tmp3B6 + 21U},{_tmp3B7,_tmp3B7,_tmp3B7 + 22U},{_tmp3B8,_tmp3B8,_tmp3B8 + 16U},{_tmp3B9,_tmp3B9,_tmp3B9 + 19U},{_tmp3BA,_tmp3BA,_tmp3BA + 25U},{_tmp3BB,_tmp3BB,_tmp3BB + 35U},{_tmp3BC,_tmp3BC,_tmp3BC + 23U},{_tmp3BD,_tmp3BD,_tmp3BD + 24U},{_tmp3BE,_tmp3BE,_tmp3BE + 18U},{_tmp3BF,_tmp3BF,_tmp3BF + 20U},{_tmp3C0,_tmp3C0,_tmp3C0 + 19U},{_tmp3C1,_tmp3C1,_tmp3C1 + 19U},{_tmp3C2,_tmp3C2,_tmp3C2 + 14U},{_tmp3C3,_tmp3C3,_tmp3C3 + 19U},{_tmp3C4,_tmp3C4,_tmp3C4 + 20U},{_tmp3C5,_tmp3C5,_tmp3C5 + 14U},{_tmp3C6,_tmp3C6,_tmp3C6 + 11U},{_tmp3C7,_tmp3C7,_tmp3C7 + 23U},{_tmp3C8,_tmp3C8,_tmp3C8 + 18U},{_tmp3C9,_tmp3C9,_tmp3C9 + 30U},{_tmp3CA,_tmp3CA,_tmp3CA + 8U},{_tmp3CB,_tmp3CB,_tmp3CB + 12U},{_tmp3CC,_tmp3CC,_tmp3CC + 14U},{_tmp3CD,_tmp3CD,_tmp3CD + 13U},{_tmp3CE,_tmp3CE,_tmp3CE + 23U},{_tmp3CF,_tmp3CF,_tmp3CF + 14U},{_tmp3D0,_tmp3D0,_tmp3D0 + 18U},{_tmp3D1,_tmp3D1,_tmp3D1 + 8U},{_tmp3D2,_tmp3D2,_tmp3D2 + 11U},{_tmp3D3,_tmp3D3,_tmp3D3 + 20U},{_tmp3D4,_tmp3D4,_tmp3D4 + 9U},{_tmp3D5,_tmp3D5,_tmp3D5 + 16U},{_tmp3D6,_tmp3D6,_tmp3D6 + 19U},{_tmp3D7,_tmp3D7,_tmp3D7 + 10U},{_tmp3D8,_tmp3D8,_tmp3D8 + 16U},{_tmp3D9,_tmp3D9,_tmp3D9 + 11U},{_tmp3DA,_tmp3DA,_tmp3DA + 14U},{_tmp3DB,_tmp3DB,_tmp3DB + 11U},{_tmp3DC,_tmp3DC,_tmp3DC + 15U},{_tmp3DD,_tmp3DD,_tmp3DD + 22U},{_tmp3DE,_tmp3DE,_tmp3DE + 16U},{_tmp3DF,_tmp3DF,_tmp3DF + 17U},{_tmp3E0,_tmp3E0,_tmp3E0 + 12U},{_tmp3E1,_tmp3E1,_tmp3E1 + 18U},{_tmp3E2,_tmp3E2,_tmp3E2 + 17U},{_tmp3E3,_tmp3E3,_tmp3E3 + 12U},{_tmp3E4,_tmp3E4,_tmp3E4 + 16U},{_tmp3E5,_tmp3E5,_tmp3E5 + 11U},{_tmp3E6,_tmp3E6,_tmp3E6 + 10U},{_tmp3E7,_tmp3E7,_tmp3E7 + 14U},{_tmp3E8,_tmp3E8,_tmp3E8 + 15U},{_tmp3E9,_tmp3E9,_tmp3E9 + 20U},{_tmp3EA,_tmp3EA,_tmp3EA + 27U},{_tmp3EB,_tmp3EB,_tmp3EB + 10U},{_tmp3EC,_tmp3EC,_tmp3EC + 18U},{_tmp3ED,_tmp3ED,_tmp3ED + 21U},{_tmp3EE,_tmp3EE,_tmp3EE + 19U},{_tmp3EF,_tmp3EF,_tmp3EF + 16U},{_tmp3F0,_tmp3F0,_tmp3F0 + 20U},{_tmp3F1,_tmp3F1,_tmp3F1 + 15U},{_tmp3F2,_tmp3F2,_tmp3F2 + 20U},{_tmp3F3,_tmp3F3,_tmp3F3 + 15U},{_tmp3F4,_tmp3F4,_tmp3F4 + 12U},{_tmp3F5,_tmp3F5,_tmp3F5 + 20U},{_tmp3F6,_tmp3F6,_tmp3F6 + 19U},{_tmp3F7,_tmp3F7,_tmp3F7 + 20U},{_tmp3F8,_tmp3F8,_tmp3F8 + 21U},{_tmp3F9,_tmp3F9,_tmp3F9 + 21U},{_tmp3FA,_tmp3FA,_tmp3FA + 12U},{_tmp3FB,_tmp3FB,_tmp3FB + 17U},{_tmp3FC,_tmp3FC,_tmp3FC + 19U},{_tmp3FD,_tmp3FD,_tmp3FD + 14U},{_tmp3FE,_tmp3FE,_tmp3FE + 17U},{_tmp3FF,_tmp3FF,_tmp3FF + 23U},{_tmp400,_tmp400,_tmp400 + 13U},{_tmp401,_tmp401,_tmp401 + 14U},{_tmp402,_tmp402,_tmp402 + 16U},{_tmp403,_tmp403,_tmp403 + 16U},{_tmp404,_tmp404,_tmp404 + 8U},{_tmp405,_tmp405,_tmp405 + 19U},{_tmp406,_tmp406,_tmp406 + 20U},{_tmp407,_tmp407,_tmp407 + 14U},{_tmp408,_tmp408,_tmp408 + 19U},{_tmp409,_tmp409,_tmp409 + 20U},{_tmp40A,_tmp40A,_tmp40A + 11U},{_tmp40B,_tmp40B,_tmp40B + 22U},{_tmp40C,_tmp40C,_tmp40C + 20U},{_tmp40D,_tmp40D,_tmp40D + 23U},{_tmp40E,_tmp40E,_tmp40E + 20U},{_tmp40F,_tmp40F,_tmp40F + 22U},{_tmp410,_tmp410,_tmp410 + 23U},{_tmp411,_tmp411,_tmp411 + 24U},{_tmp412,_tmp412,_tmp412 + 24U},{_tmp413,_tmp413,_tmp413 + 15U},{_tmp414,_tmp414,_tmp414 + 20U},{_tmp415,_tmp415,_tmp415 + 22U},{_tmp416,_tmp416,_tmp416 + 17U},{_tmp417,_tmp417,_tmp417 + 20U},{_tmp418,_tmp418,_tmp418 + 26U},{_tmp419,_tmp419,_tmp419 + 16U},{_tmp41A,_tmp41A,_tmp41A + 17U},{_tmp41B,_tmp41B,_tmp41B + 15U},{_tmp41C,_tmp41C,_tmp41C + 19U},{_tmp41D,_tmp41D,_tmp41D + 17U},{_tmp41E,_tmp41E,_tmp41E + 19U},{_tmp41F,_tmp41F,_tmp41F + 25U},{_tmp420,_tmp420,_tmp420 + 26U},{_tmp421,_tmp421,_tmp421 + 9U},{_tmp422,_tmp422,_tmp422 + 20U},{_tmp423,_tmp423,_tmp423 + 17U},{_tmp424,_tmp424,_tmp424 + 18U},{_tmp425,_tmp425,_tmp425 + 11U},{_tmp426,_tmp426,_tmp426 + 12U}};
# 1638
static short Cyc_yyr1[528U]={0,147,148,148,148,148,148,148,148,148,148,148,149,150,151,151,152,152,152,153,153,153,154,154,155,155,155,155,156,156,157,158,159,160,161,161,161,161,161,161,161,162,162,163,163,163,163,163,163,163,163,163,163,164,164,164,164,164,164,164,165,165,166,167,167,168,168,168,168,169,169,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,171,172,172,172,173,173,173,174,174,175,175,175,176,176,176,176,176,177,177,178,178,179,179,180,180,181,182,182,183,183,184,185,185,185,185,185,185,186,186,186,186,186,186,187,188,188,189,189,189,189,190,190,191,191,192,192,192,193,193,194,194,194,194,195,195,195,196,196,197,197,198,198,199,199,199,199,199,199,199,199,199,199,200,200,200,200,200,200,200,200,200,200,200,201,201,202,203,203,204,204,204,204,204,204,204,204,205,205,205,206,206,207,207,207,208,208,208,209,209,210,210,210,210,211,211,212,212,213,213,214,214,215,215,216,216,217,217,217,217,218,218,219,219,220,220,220,221,222,222,223,223,224,224,224,224,224,225,225,225,225,226,226,227,227,228,228,229,229,230,230,230,230,230,231,231,232,232,232,233,233,233,233,233,233,233,233,233,233,233,234,234,234,234,234,234,235,236,236,237,237,238,238,238,238,238,238,238,238,239,239,239,239,239,239,240,240,240,240,240,240,241,241,241,241,241,241,241,241,241,241,241,241,241,241,242,242,242,242,242,242,242,242,243,244,244,245,245,246,246,247,247,248,248,249,249,250,250,250,251,251,251,251,251,252,252,252,253,253,253,254,254,254,254,255,255,256,256,256,256,256,256,257,258,259,259,259,259,259,259,259,259,259,259,259,259,259,259,259,259,260,260,260,261,261,262,262,263,263,263,264,264,265,265,266,266,266,267,267,267,267,267,267,267,267,267,267,267,268,268,268,268,268,268,268,269,270,270,271,271,272,272,273,273,274,274,275,275,275,276,276,276,276,276,277,277,277,278,278,278,279,279,279,279,280,280,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,282,282,282,283,283,283,283,283,283,283,283,283,283,283,284,284,284,284,285,285,285,285,285,285,285,285,285,285,285,286,287,287,288,288,288,288,288,289,289,290,290,291,291,292,292,293,293};
# 1694
static short Cyc_yyr2[528U]={0,1,2,3,5,3,5,5,6,3,3,0,2,1,4,3,1,2,3,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,6,3,8,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,2,2,0,3,4,0,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,6,7,4,4,2,1,1,3,4,4,5,6,7,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,6,5,7,6,4,2,1,1,1,1,1,1,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,6,9,11,4,6,6,4,1,1,1,1,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,4,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1750
static short Cyc_yydefact[1074U]={0,21,53,54,55,56,58,71,72,73,74,75,76,77,78,79,97,98,99,115,116,49,0,0,83,0,59,0,0,153,90,94,0,0,0,0,0,0,0,518,219,520,519,521,0,0,86,0,205,205,204,1,0,0,19,0,0,20,0,43,51,45,69,47,80,81,0,84,0,0,164,0,189,192,85,168,113,57,56,50,0,101,0,517,0,518,513,514,515,516,113,0,379,0,0,0,0,242,0,381,382,30,32,0,0,0,0,0,0,0,0,0,154,0,0,0,0,0,0,0,202,203,0,2,0,0,0,0,34,0,121,122,124,44,52,46,48,117,522,523,113,113,0,41,0,0,23,0,221,0,177,165,190,0,196,197,200,201,0,199,198,210,192,0,70,57,105,0,103,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,502,503,480,0,0,0,0,0,483,481,482,0,407,409,423,431,433,435,437,439,441,444,449,452,455,459,0,461,484,501,499,518,390,0,0,0,0,0,391,389,37,0,0,113,0,0,0,131,127,129,262,264,0,0,39,0,0,9,10,0,0,113,524,525,220,96,0,0,169,87,240,0,237,0,0,3,0,5,0,35,0,0,0,23,0,118,119,144,112,0,151,0,0,0,0,0,0,0,0,0,0,0,518,292,294,0,302,296,0,300,285,286,287,0,288,289,290,0,42,23,124,22,24,269,0,227,243,0,0,223,221,0,207,0,0,0,212,60,211,193,0,106,102,0,0,0,469,0,425,459,0,426,427,0,0,0,0,0,0,0,0,0,0,0,462,463,0,0,0,0,465,466,464,0,82,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,413,414,415,416,417,418,419,420,421,422,412,0,467,0,490,491,0,0,0,505,0,113,383,0,0,404,518,0,0,0,0,258,400,405,0,402,0,380,397,398,0,395,0,244,0,0,0,0,265,0,235,132,137,133,135,128,130,221,0,271,263,272,527,526,0,89,93,0,0,0,91,95,111,66,65,0,63,170,221,239,166,271,241,178,179,0,88,206,13,0,31,0,33,0,123,125,246,245,23,25,108,120,0,0,0,139,140,147,0,113,113,159,0,0,0,0,0,518,0,0,0,331,332,333,0,0,335,0,0,0,303,297,124,301,295,293,26,0,176,228,0,0,0,234,222,229,147,0,0,0,223,175,209,208,171,207,0,0,213,61,114,107,430,104,100,0,0,0,518,247,252,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,504,511,0,510,408,432,0,434,436,438,440,442,443,447,448,445,446,450,451,453,454,456,457,458,411,410,489,486,0,488,0,0,0,393,394,261,0,401,256,259,388,0,257,385,0,392,36,0,386,0,266,0,138,134,136,0,223,0,207,0,273,0,221,0,284,268,0,0,0,113,0,0,0,131,0,113,0,221,0,188,167,238,0,7,0,4,6,27,0,143,126,144,0,0,142,223,152,161,160,0,0,155,0,0,0,310,0,0,0,0,0,0,330,334,0,0,0,298,291,0,28,270,221,0,231,0,0,149,224,0,147,227,215,172,194,195,213,191,468,0,0,248,0,253,471,0,0,0,0,0,500,476,479,0,0,0,0,0,460,507,0,0,487,485,0,0,0,260,403,406,396,399,387,267,236,147,0,274,275,207,0,0,223,207,0,0,38,92,223,518,0,62,64,0,180,0,0,223,0,207,0,0,8,141,0,145,117,150,162,159,159,0,0,0,0,0,0,0,0,0,0,0,0,0,310,336,0,299,29,223,0,232,230,0,173,0,149,223,0,214,496,0,495,0,249,254,0,0,0,0,0,428,429,489,488,509,492,0,512,424,506,508,0,384,149,147,277,283,147,0,276,207,0,117,0,67,181,187,147,0,186,182,207,15,0,16,0,0,0,158,157,304,310,0,0,0,0,0,0,338,339,341,343,345,347,349,351,354,359,362,365,369,371,377,378,0,0,307,316,0,0,0,0,0,0,0,0,0,0,337,217,233,0,225,174,216,221,470,0,0,255,472,473,0,0,478,477,493,0,280,149,149,147,278,40,0,0,149,147,183,14,17,0,146,110,0,0,0,310,0,375,0,0,372,310,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,373,310,0,318,0,0,0,326,0,0,0,0,0,0,309,0,0,223,498,497,0,0,0,494,279,282,149,109,0,184,149,18,163,305,306,0,0,0,0,310,312,342,0,344,346,348,350,352,353,357,358,355,356,360,361,363,364,366,367,368,0,311,317,319,320,0,328,327,0,322,0,0,0,148,226,218,0,0,0,0,281,0,185,308,374,0,370,313,0,310,321,329,323,324,0,251,250,474,0,68,0,340,310,314,325,0,376,315,475,0,0,0};
# 1861
static short Cyc_yydefgoto[147U]={1071,51,52,470,651,851,53,308,54,292,55,472,56,474,57,144,58,59,539,233,456,457,234,62,250,235,64,166,167,65,164,66,270,271,129,130,131,272,236,436,485,486,487,661,801,67,68,666,667,668,69,488,70,461,71,72,161,162,73,120,535,323,704,622,74,623,529,695,521,525,526,430,316,258,96,97,551,477,552,410,411,412,237,309,310,624,442,295,296,297,298,299,300,783,301,302,865,866,867,868,869,870,871,872,873,874,875,876,877,878,879,880,413,421,422,414,415,416,303,198,392,199,544,200,201,202,203,204,205,206,207,208,209,210,211,212,213,808,214,569,570,215,216,76,852,417,446};
# 1879
static short Cyc_yypact[1074U]={2628,- -32768,- -32768,- -32768,- -32768,- 45,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3455,103,- 50,- -32768,3040,- -32768,85,11,- -32768,4,44,36,51,88,107,86,251,202,- -32768,152,- -32768,- -32768,- -32768,192,553,222,213,221,221,- -32768,- -32768,303,2490,- -32768,248,459,- -32768,546,3455,3455,3455,- -32768,3455,- -32768,- -32768,584,- -32768,85,3370,240,119,697,1048,- -32768,- -32768,316,329,355,- -32768,85,346,6356,- -32768,3071,182,- -32768,- -32768,- -32768,- -32768,316,6356,- -32768,375,403,3071,392,428,442,- -32768,257,- -32768,- -32768,3660,3660,166,510,2490,2490,3660,6356,275,- -32768,196,476,607,505,196,4114,6356,- -32768,- -32768,2765,- -32768,2490,2765,2490,2765,- -32768,537,555,- -32768,3242,- -32768,- -32768,- -32768,- -32768,4114,- -32768,- -32768,316,238,1777,- -32768,3370,546,574,3660,3537,3783,- -32768,240,- -32768,575,- -32768,- -32768,- -32768,- -32768,579,- -32768,- -32768,- 40,1048,3660,- -32768,- -32768,606,615,622,85,6743,604,6356,6549,632,636,643,652,664,686,688,694,696,710,6839,6839,- -32768,- -32768,- -32768,2204,716,6935,6935,6935,- -32768,- -32768,- -32768,113,- -32768,- -32768,- 52,656,713,649,718,552,92,714,201,400,- -32768,831,6935,229,- 8,- -32768,750,47,- -32768,3071,73,1224,286,2811,777,- -32768,- -32768,782,6356,316,2811,778,3868,4114,4196,4114,513,- -32768,83,83,- -32768,801,788,- -32768,- -32768,124,300,316,- -32768,- -32768,- -32768,- -32768,33,791,- -32768,- -32768,560,357,- -32768,794,799,- -32768,815,- -32768,816,- -32768,607,4804,3370,574,817,4114,- -32768,930,819,85,822,827,281,829,4263,846,837,826,838,4901,4263,333,818,- -32768,- -32768,848,1921,1921,546,1921,- -32768,- -32768,- -32768,855,- -32768,- -32768,- -32768,367,- -32768,574,856,- -32768,- -32768,841,104,874,- -32768,101,861,852,381,862,771,850,6356,3660,- -32768,868,- -32768,- -32768,104,85,- -32768,6356,869,2204,- -32768,4114,- -32768,- -32768,4360,- -32768,899,6356,6356,6356,6356,6356,6356,907,6356,4114,858,2204,- -32768,- -32768,1921,887,368,6356,- -32768,- -32768,- -32768,6356,- -32768,6935,6356,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6356,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6356,- -32768,196,- -32768,- -32768,4998,196,6356,- -32768,892,316,- -32768,893,894,- -32768,130,196,6356,3071,416,- -32768,- -32768,- -32768,896,900,891,- -32768,- -32768,- -32768,898,904,3071,- -32768,444,1224,902,3660,- -32768,906,908,- -32768,4196,4196,4196,- -32768,- -32768,2977,5095,292,- -32768,319,- -32768,- -32768,101,- -32768,- -32768,911,933,3660,- -32768,- -32768,920,- -32768,916,921,931,- -32768,2847,- -32768,344,412,- -32768,- -32768,- -32768,4114,- -32768,- -32768,- -32768,1432,- -32768,2490,- -32768,2490,- -32768,- -32768,- -32768,- -32768,574,- -32768,- -32768,- -32768,1018,6356,924,932,- -32768,- 33,634,316,316,859,6356,6356,926,935,6356,927,1030,1167,942,- -32768,- -32768,- -32768,482,1019,- -32768,5192,2065,2343,- -32768,- -32768,3242,- -32768,- -32768,- -32768,- -32768,3660,- -32768,- -32768,4114,937,3950,- -32768,- -32768,934,993,101,943,4032,852,- -32768,- -32768,- -32768,- -32768,771,948,146,811,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,949,947,971,950,- -32768,- -32768,745,4804,951,959,960,961,963,533,966,967,968,247,970,969,6646,- -32768,- -32768,973,965,- -32768,656,223,713,649,718,552,92,92,714,714,714,714,201,201,400,400,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,974,- -32768,- 35,3660,3575,4114,- -32768,- -32768,955,- -32768,- -32768,- -32768,- -32768,1266,- -32768,- -32768,2926,- -32768,- -32768,977,- -32768,189,- -32768,4114,- -32768,- -32768,- -32768,976,852,978,771,975,319,3660,3745,5289,- -32768,- -32768,6356,985,83,316,6453,980,33,3107,982,316,3660,3537,5386,- -32768,344,- -32768,990,- -32768,2490,- -32768,- -32768,- -32768,1183,- -32768,- -32768,930,986,6356,- -32768,852,- -32768,- -32768,- -32768,994,85,521,590,592,6356,853,624,4263,988,5483,5580,545,- -32768,- -32768,991,998,996,1921,- -32768,3370,- -32768,841,1010,3660,- -32768,1015,101,1061,- -32768,1013,993,147,- -32768,- -32768,- -32768,- -32768,811,- -32768,1020,162,1021,- -32768,4471,- -32768,- -32768,6356,6356,1118,6356,6549,- -32768,- -32768,- -32768,196,196,1020,1022,4582,- -32768,- -32768,6356,6356,- -32768,- -32768,104,756,1041,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,993,1024,- -32768,- -32768,771,104,1029,852,771,1025,648,- -32768,- -32768,852,1026,1032,- -32768,- -32768,1033,- -32768,104,1034,852,1035,771,1031,305,- -32768,- -32768,6356,- -32768,4114,- -32768,1037,66,859,4263,1045,1039,728,1027,1046,4263,6356,5677,610,5774,612,5871,853,- -32768,1049,- -32768,- -32768,852,197,- -32768,- -32768,1062,- -32768,1076,1061,852,4114,- -32768,- -32768,393,- -32768,6356,- -32768,- -32768,4804,1066,1067,1069,1074,- -32768,899,1070,1072,- -32768,- -32768,759,- -32768,- -32768,- -32768,- -32768,3575,- -32768,1061,993,- -32768,- -32768,993,1082,- -32768,771,1091,4114,1094,- -32768,- -32768,- -32768,993,1084,- -32768,- -32768,771,- -32768,1093,637,1088,1095,4114,- -32768,- -32768,1196,853,1108,7031,1103,2343,6935,1100,- -32768,- 46,- -32768,1146,1101,754,806,212,814,395,433,- -32768,- -32768,- -32768,- -32768,1147,6935,1921,- -32768,- -32768,650,4263,658,5968,4263,659,6065,6162,691,1119,- -32768,- -32768,- -32768,6356,1120,- -32768,- -32768,1010,- -32768,609,307,- -32768,- -32768,- -32768,4114,1217,- -32768,- -32768,- -32768,4693,- -32768,1061,1061,993,- -32768,- -32768,1122,1124,1061,993,- -32768,- -32768,- -32768,275,- -32768,- -32768,662,4263,1123,853,2204,- -32768,4114,1125,- -32768,1633,6935,6356,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6356,- -32768,853,1134,- -32768,4263,4263,665,- -32768,4263,4263,674,4263,676,6259,- -32768,1127,101,852,- -32768,- -32768,2343,1128,1131,- -32768,- -32768,- -32768,1061,- -32768,1151,- -32768,1061,- -32768,- -32768,- -32768,- -32768,1138,1135,1139,6935,853,- -32768,656,320,713,649,649,552,92,92,714,714,714,714,201,201,400,400,- -32768,- -32768,- -32768,324,- -32768,- -32768,- -32768,- -32768,4263,- -32768,- -32768,4263,- -32768,4263,4263,699,- -32768,- -32768,- -32768,1144,781,1140,4114,- -32768,1141,- -32768,- -32768,1020,162,- -32768,- -32768,6356,1633,- -32768,- -32768,- -32768,- -32768,4263,- -32768,- -32768,- -32768,1143,- -32768,527,- -32768,853,- -32768,- -32768,1145,- -32768,- -32768,- -32768,1267,1272,- -32768};
# 1990
static short Cyc_yypgoto[147U]={- -32768,12,- -32768,- -32768,- -32768,359,- -32768,- 153,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 64,- 113,41,- -32768,- -32768,0,668,- -32768,52,- 195,1172,23,- -32768,- -32768,- 134,- -32768,16,1254,- 731,- -32768,- -32768,- -32768,1038,1040,621,413,- -32768,- -32768,645,- 446,- 196,- -32768,- -32768,133,- -32768,- -32768,137,- 239,1230,- 372,140,- -32768,1133,- -32768,- -32768,1265,- 490,- -32768,614,- 114,- 147,- 126,- 398,337,623,629,- 425,- 472,- 115,- 434,- 136,- -32768,- 243,- 160,- 566,- 270,- -32768,913,- 25,- 78,- 123,29,- 265,- 80,- -32768,- -32768,- 65,- 277,- -32768,- 586,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,269,1096,- -32768,717,901,- -32768,132,693,- -32768,- 143,- 361,- 145,- 334,- 362,- 315,964,- 328,- 360,- 314,- 363,- 339,- 98,633,561,995,287,- -32768,- 367,- -32768,164,489,- 57,87,- 111,- 77};
# 2012
static short Cyc_yytable[7178U]={60,574,251,257,146,143,251,578,579,139,584,585,317,337,324,511,512,460,514,268,632,60,315,63,476,238,239,572,338,334,595,245,648,732,314,330,586,587,577,434,326,361,854,659,63,115,700,942,603,575,454,692,61,60,139,580,581,582,583,60,60,60,79,60,77,123,553,269,143,60,150,664,665,61,40,565,63,82,293,305,304,694,63,63,63,362,63,689,359,322,647,943,63,356,357,358,648,400,660,730,133,134,135,696,136,61,221,60,60,922,145,61,61,61,393,61,480,647,102,243,244,61,60,656,60,60,60,60,401,103,63,63,60,699,260,745,261,262,263,264,455,683,60,105,60,63,741,63,63,63,63,150,517,140,219,63,274,276,824,61,61,106,447,522,353,63,524,63,451,104,443,107,220,145,61,627,61,61,61,61,369,370,116,294,61,145,543,404,- 156,99,531,443,39,542,61,132,61,627,247,465,499,42,405,479,304,895,506,431,108,444,39,443,152,110,197,40,465,371,372,42,641,445,523,222,80,743,39,518,293,293,444,293,109,519,433,42,359,219,434,434,434,360,246,538,641,426,45,450,99,541,444,803,253,112,520,220,833,435,113,99,837,- 524,445,453,773,441,543,313,60,248,807,19,20,934,702,- 221,848,249,- 221,588,589,590,306,593,445,463,240,596,293,219,241,63,60,60,98,60,831,602,524,772,950,951,- 243,248,780,- 243,546,220,547,145,710,249,394,395,396,63,63,740,63,114,61,354,562,540,564,898,654,445,117,553,720,395,396,294,294,445,294,952,953,375,118,543,119,376,61,61,359,61,920,997,616,836,60,218,728,1002,397,840,44,926,425,398,399,163,225,147,846,148,124,125,275,903,635,149,397,63,440,1023,229,721,399,524,39,99,230,99,917,99,39,918,41,42,43,294,99,257,462,42,897,924,686,760,642,306,61,465,902,794,494,359,853,495,39,489,418,1049,760,505,599,438,41,42,43,359,122,439,465,850,452,685,359,513,465,433,433,433,677,44,982,813,688,163,631,359,628,684,629,359,687,143,- 12,1050,630,507,553,1051,435,435,435,640,508,646,44,354,165,1065,812,169,725,643,60,644,60,988,60,731,559,645,1069,992,466,354,650,655,652,467,653,516,403,75,359,359,420,63,573,63,567,63,431,420,60,313,223,749,530,747,662,663,765,60,224,81,226,60,100,748,101,543,764,39,762,904,61,63,61,905,61,41,42,43,763,597,63,956,75,377,63,957,977,459,605,145,75,378,379,439,524,75,408,409,145,227,61,696,145,138,818,141,754,813,75,61,703,796,613,61,228,359,958,168,907,819,804,100,99,733,959,960,440,126,127,1037,1005,642,100,826,812,99,1009,1010,99,1015,1016,431,685,809,75,75,465,462,138,465,680,252,75,359,963,901,1003,820,821,75,797,75,75,75,75,1017,1018,1008,293,795,304,489,242,669,670,631,543,673,1006,1007,678,254,75,916,1011,1012,1013,1014,438,776,367,368,777,813,439,646,48,49,50,60,755,39,827,1068,359,265,168,905,39,716,42,769,1001,790,128,39,359,42,834,44,812,45,63,41,42,43,266,604,45,48,49,50,60,844,60,459,48,49,50,39,612,439,307,48,49,50,858,41,42,43,320,61,885,137,321,63,100,63,100,139,100,359,39,359,248,980,778,100,779,986,987,42,249,294,703,145,991,327,889,333,892,359,45,359,61,328,61,256,932,39,48,49,50,464,329,359,363,41,42,43,784,861,862,75,543,928,273,339,929,483,464,340,491,752,940,496,83,256,341,359,99,359,1064,99,839,979,964,342,489,359,359,75,962,466,966,970,359,365,994,343,1042,1027,981,489,1044,359,489,359,373,374,1030,332,1032,335,335,965,787,789,969,975,259,344,359,345,168,350,351,293,84,346,359,347,335,335,335,1056,16,17,18,524,48,49,50,217,86,348,939,87,88,89,319,355,42,335,619,620,621,90,533,534,364,995,432,863,437,92,93,366,1019,1020,1021,94,664,665,864,708,709,95,194,402,139,195,196,781,782,293,828,829,738,914,915,60,423,983,1025,1026,380,424,1028,1029,273,1031,946,947,948,949,83,100,954,955,1048,1058,359,428,63,1063,448,856,857,998,100,999,449,100,468,886,888,458,891,469,894,180,294,381,382,383,384,385,386,387,388,389,390,61,809,471,473,481,490,60,906,492,501,99,502,1052,464,509,1053,464,1054,1055,493,391,497,1038,503,75,478,75,335,75,63,39,86,186,187,87,88,89,464,500,42,510,1066,515,464,445,267,520,294,528,536,349,44,293,190,527,532,545,361,61,335,222,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,537,560,566,1060,598,607,600,601,968,609,608,972,974,610,611,617,478,615,618,554,555,556,557,558,39,633,561,634,636,335,637,657,41,42,43,568,638,881,60,571,671,639,658,672,483,674,256,675,484,679,681,690,48,49,50,354,659,706,697,693,591,63,1004,701,705,707,711,256,- 524,712,713,714,592,715,256,727,734,568,723,294,1022,717,718,719,100,722,739,100,726,729,61,742,753,744,1034,758,746,761,768,791,771,1039,785,774,335,792,153,154,155,156,157,793,39,158,159,160,464,313,256,626,41,42,43,44,798,800,802,75,724,256,816,464,483,810,464,830,841,822,256,832,48,49,50,775,835,883,838,842,843,845,847,855,849,859,860,884,896,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,899,900,21,170,171,908,909,910,911,912,335,913,568,923,568,22,23,24,83,25,172,919,921,925,173,26,927,930,931,29,174,175,176,177,178,933,30,31,32,179,935,938,941,180,35,181,182,36,183,944,961,945,976,984,978,989,996,478,990,37,38,256,184,185,1000,1024,1035,1040,138,1041,1043,1045,432,1047,1046,83,256,1057,1072,1059,1061,100,1067,1073,1070,39,86,186,187,87,88,89,40,41,42,43,44,188,676,993,255,39,111,478,335,189,325,46,291,41,42,43,151,191,770,475,192,83,759,193,194,483,482,195,196,121,1036,84,806,48,49,50,805,799,751,606,736,427,614,406,576,757,407,86,0,1062,87,88,89,767,249,42,0,882,563,0,90,0,0,0,335,0,91,0,92,93,0,408,409,84,94,0,335,0,0,568,95,0,0,0,0,735,0,0,407,86,0,0,87,88,89,0,249,42,0,0,0,0,90,0,0,0,0,0,91,273,92,93,0,408,409,0,94,478,0,335,814,815,95,817,0,0,0,0,0,0,0,0,478,138,0,825,0,0,0,0,0,256,0,0,0,0,0,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,273,0,0,0,0,0,0,0,22,23,24,0,25,0,0,256,649,0,26,0,27,28,29,0,0,0,0,0,0,30,31,32,0,937,33,34,335,35,0,0,36,0,0,0,0,478,0,0,0,0,0,0,37,38,335,0,0,0,0,0,0,478,0,0,0,0,0,0,0,0,0,335,0,0,0,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,- 11,0,0,0,0,45,0,46,47,0,0,0,0,48,49,50,0,0,0,0,0,335,0,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,0,0,0,0,0,0,0,0,0,0,0,0,0,0,478,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,335,0,0,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,781,782,21,170,171,277,0,278,279,280,281,282,283,284,285,22,23,24,83,25,172,286,0,0,173,26,287,0,335,29,174,175,176,177,178,0,30,31,32,179,0,0,0,180,35,181,182,36,183,0,0,0,0,0,0,0,0,0,0,37,38,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,288,86,186,187,87,88,89,40,41,42,43,44,188,289,142,0,0,0,0,0,189,0,46,291,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,170,171,277,0,278,279,280,281,282,283,284,285,22,23,24,83,25,172,286,0,0,173,26,287,0,0,29,174,175,176,177,178,0,30,31,32,179,0,0,0,180,35,181,182,36,183,0,0,0,0,0,0,0,0,0,0,37,38,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,288,86,186,187,87,88,89,40,41,42,43,44,188,289,142,290,0,0,0,0,189,0,46,291,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,170,171,277,0,278,279,280,281,282,283,284,285,22,23,24,83,25,172,286,0,0,173,26,287,0,0,29,174,175,176,177,178,0,30,31,32,179,0,0,0,180,35,181,182,36,183,0,0,0,0,0,0,0,0,0,0,37,38,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,288,86,186,187,87,88,89,40,41,42,43,44,188,289,142,0,0,0,0,0,189,0,46,291,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,170,171,277,0,278,279,280,281,282,283,284,285,22,23,24,83,25,172,286,0,0,173,26,287,0,0,29,174,175,176,177,178,0,30,31,32,179,0,0,0,180,35,181,182,36,183,0,0,0,0,0,0,0,0,0,0,37,38,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,498,86,186,187,87,88,89,40,41,42,43,44,188,289,142,0,0,0,0,0,189,0,46,291,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,170,171,0,0,0,0,0,0,0,0,0,0,22,23,24,83,0,172,0,0,0,173,0,0,0,0,29,174,175,176,177,178,0,30,31,0,179,0,0,0,180,35,181,182,36,183,0,0,0,0,0,0,0,0,0,0,37,38,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,40,41,42,43,44,188,0,352,0,0,0,0,0,189,0,46,291,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,170,171,0,0,0,0,0,0,0,0,0,0,22,23,24,83,0,172,0,0,0,173,0,0,0,0,29,174,175,176,177,178,0,30,31,0,179,0,0,0,180,35,181,182,36,183,0,0,0,0,0,0,0,0,0,0,37,38,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,40,41,42,43,44,188,0,0,0,0,0,0,0,189,0,46,291,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,27,28,29,0,0,0,0,0,0,30,31,32,0,0,33,34,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,- 11,0,0,0,0,45,0,46,47,0,0,0,0,48,49,50,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,27,28,29,0,0,0,0,0,0,30,31,32,0,0,33,34,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,0,0,0,0,0,45,0,46,47,0,0,0,0,48,49,50,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,27,28,29,0,0,0,0,0,0,30,31,32,0,0,33,34,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,83,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,22,23,24,- 11,0,0,0,0,45,0,46,47,0,0,29,0,48,49,50,84,0,30,31,0,0,0,0,0,0,35,0,419,36,0,217,86,0,0,87,88,89,0,0,42,37,38,0,0,90,0,0,0,0,0,91,0,92,93,0,0,0,0,94,0,0,0,0,311,95,0,39,0,0,0,0,0,0,40,41,42,43,44,83,313,0,0,0,0,0,0,459,0,46,47,0,0,439,0,48,49,50,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,84,0,0,0,0,0,0,0,0,0,29,0,737,0,0,217,86,30,31,87,88,89,0,0,42,35,0,0,36,90,0,0,0,0,0,91,0,92,93,0,37,38,0,94,0,0,0,0,0,95,0,0,0,0,0,0,0,0,0,0,0,0,83,311,0,0,0,0,0,0,0,0,0,40,41,0,43,44,0,313,0,0,0,0,0,0,438,0,46,47,0,0,439,83,48,49,50,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,84,0,0,0,0,0,0,0,0,0,0,0,22,23,24,85,86,0,0,87,88,89,0,0,42,0,29,0,0,90,0,84,0,30,31,91,0,92,93,0,0,35,0,94,36,0,217,86,0,95,87,88,89,0,0,42,37,38,0,0,90,0,0,0,0,0,91,0,92,93,0,0,0,0,94,0,0,0,0,0,95,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,0,0,0,0,0,483,0,46,47,0,0,0,0,48,49,50,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,0,0,29,0,0,0,0,0,0,30,31,32,0,0,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,41,0,43,44,0,0,142,0,0,267,0,0,0,0,46,47,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,0,0,29,0,0,0,0,0,0,30,31,32,0,0,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,40,41,0,43,44,0,0,142,22,23,24,0,0,0,0,46,47,0,26,0,0,0,29,0,0,0,0,0,0,30,31,0,0,0,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,40,41,0,43,44,22,23,24,0,0,0,0,0,0,0,46,47,0,0,29,0,0,0,0,0,0,30,31,0,0,0,170,171,0,35,0,0,36,0,0,0,0,0,0,0,0,83,0,172,37,38,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,311,181,182,312,183,0,0,0,0,0,40,41,0,43,44,0,313,0,184,185,0,0,0,0,0,46,47,7,8,9,10,11,12,13,14,15,16,17,18,19,20,549,86,186,187,87,88,89,0,249,42,0,0,188,0,336,22,23,24,0,0,189,0,0,190,0,408,409,0,191,29,0,192,0,0,193,194,30,31,195,196,231,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,40,41,0,43,44,0,0,232,22,23,24,0,0,0,0,46,47,0,0,0,0,0,29,0,0,0,0,0,0,30,31,0,0,0,170,171,0,35,0,0,36,0,0,0,0,0,0,0,0,83,0,172,37,38,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,311,181,182,0,183,0,0,0,0,0,40,41,0,43,44,0,313,0,184,185,0,0,0,0,0,46,47,7,8,9,10,11,12,13,14,15,16,17,18,19,20,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,22,23,24,0,0,189,0,0,190,0,0,0,318,191,29,0,192,0,0,193,194,30,31,195,196,0,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,40,41,0,43,44,22,23,24,429,0,0,0,0,0,0,46,47,0,0,29,0,0,0,0,0,0,30,31,0,0,0,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,40,41,0,43,44,22,23,24,691,0,0,0,0,0,0,46,47,0,0,29,0,0,0,0,0,0,30,31,0,0,0,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,698,0,0,0,0,0,0,0,0,0,40,41,0,43,44,22,23,24,0,0,0,0,0,0,0,46,47,0,0,29,0,0,0,0,0,0,30,31,0,0,0,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,40,41,0,43,44,22,23,24,0,0,0,0,0,0,0,46,47,0,0,29,0,0,0,0,0,0,30,31,0,0,0,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,0,0,170,171,277,0,278,279,280,281,282,283,284,285,0,0,0,83,0,172,286,0,0,173,40,287,0,0,44,174,175,176,177,178,0,0,0,0,179,46,47,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,498,86,186,187,87,88,89,0,0,42,0,0,188,289,142,170,171,0,0,0,189,0,548,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,549,86,186,187,87,88,89,0,249,42,0,0,188,0,336,550,0,0,0,0,189,0,0,190,0,408,409,0,191,170,171,192,0,0,193,194,0,0,195,196,0,0,0,0,83,0,172,0,0,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,549,86,186,187,87,88,89,0,249,42,0,0,188,0,336,811,0,0,0,0,189,0,0,190,0,408,409,0,191,170,171,192,0,0,193,194,0,0,195,196,0,0,0,0,83,0,172,0,0,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,549,86,186,187,87,88,89,0,249,42,0,0,188,0,336,823,0,0,0,0,189,0,0,190,0,408,409,0,191,170,171,192,0,0,193,194,0,0,195,196,0,0,0,0,83,0,172,0,0,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,549,86,186,187,87,88,89,0,249,42,0,0,188,0,336,985,0,0,0,0,189,0,0,190,0,408,409,0,191,170,171,192,0,0,193,194,0,0,195,196,0,0,0,0,83,0,172,0,0,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,336,170,171,0,0,0,189,0,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,504,0,170,171,0,0,0,189,0,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,170,171,0,0,0,189,594,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,170,171,0,0,0,189,0,0,190,0,0,0,625,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,170,171,0,0,0,189,682,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,170,171,0,0,0,189,0,0,190,0,0,0,750,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,170,171,0,0,0,189,0,0,190,0,0,0,766,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,786,0,170,171,0,0,0,189,0,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,788,0,170,171,0,0,0,189,0,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,170,171,0,0,0,189,887,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,170,171,0,0,0,189,890,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,893,0,170,171,0,0,0,189,0,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,170,171,0,0,0,189,967,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,170,171,0,0,0,189,971,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,170,171,0,0,0,189,973,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,170,171,0,0,0,189,1033,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,170,171,0,0,0,189,0,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,756,86,186,187,87,88,89,0,0,42,0,0,188,0,170,171,0,0,0,0,189,0,0,190,0,0,0,0,191,83,0,192,0,0,193,194,0,0,195,196,0,174,175,176,177,178,0,0,0,0,0,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,336,170,171,0,0,0,189,0,0,190,0,0,0,0,191,0,83,192,0,0,193,194,0,0,195,196,0,0,174,175,176,177,178,0,0,0,0,0,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,724,170,171,0,0,0,189,0,0,190,0,0,0,0,191,0,83,192,0,0,193,194,0,0,195,196,0,0,174,175,176,177,178,0,0,0,0,0,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,170,171,0,0,0,0,331,0,0,190,0,0,0,0,191,83,0,192,0,0,193,194,0,0,195,196,0,174,175,176,177,178,0,0,0,0,0,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,170,171,0,0,0,0,349,0,0,190,0,0,0,0,191,83,0,192,0,0,193,194,0,0,195,196,0,174,175,176,177,178,0,0,0,0,0,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,170,171,0,0,0,0,189,0,0,190,0,0,0,0,191,83,0,192,0,0,193,194,0,0,195,196,0,174,175,176,177,178,0,0,0,0,0,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,0,0,0,0,0,936,0,0,190,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196};
# 2733
static short Cyc_yycheck[7178U]={0,363,113,118,69,69,117,367,368,66,373,374,148,173,161,292,293,256,295,132,445,21,148,0,267,103,104,361,173,172,397,109,466,599,148,169,375,376,366,234,163,93,773,76,21,45,536,93,409,364,17,523,0,53,111,369,370,371,372,59,60,61,21,63,109,53,336,132,132,69,70,5,6,21,114,352,53,127,142,144,144,527,59,60,61,137,63,521,123,129,462,137,69,191,192,193,530,105,131,134,59,60,61,528,63,53,90,107,108,840,69,59,60,61,212,63,269,489,107,107,108,69,122,484,124,125,126,127,136,125,107,108,132,531,122,625,124,125,126,127,107,508,142,107,144,122,618,124,125,126,127,151,305,66,107,132,140,141,724,107,108,125,239,62,189,142,313,144,245,125,87,120,125,132,122,440,124,125,126,127,88,89,45,142,132,144,329,114,122,25,316,87,107,327,142,58,144,462,111,256,280,116,129,268,268,791,286,232,120,126,107,87,72,127,82,114,273,125,126,116,459,138,121,91,121,623,107,123,292,293,126,295,125,310,234,116,123,107,433,434,435,128,110,321,483,229,127,123,84,326,126,697,115,51,107,125,746,234,106,95,750,131,138,247,662,236,409,120,268,107,108,20,21,859,128,128,766,115,131,377,378,379,145,394,138,256,120,398,352,107,124,268,292,293,25,295,742,408,445,660,88,89,120,107,671,123,331,125,333,268,553,115,83,84,85,292,293,128,295,127,268,189,347,323,349,128,479,138,106,599,83,84,85,292,293,138,295,125,126,138,127,484,121,142,292,293,123,295,838,935,428,749,352,84,131,941,127,755,118,849,228,132,133,125,95,125,764,127,120,121,132,805,450,133,127,352,236,963,121,132,133,528,107,219,127,221,832,223,107,835,115,116,117,352,230,510,256,116,796,845,513,640,459,266,352,462,804,684,127,123,771,130,107,273,128,1001,655,285,402,127,115,116,117,123,121,133,483,122,128,509,123,294,489,433,434,435,500,118,131,709,518,125,442,123,125,509,127,123,513,513,121,131,133,120,724,131,433,434,435,459,127,461,118,331,109,1051,709,121,566,125,470,127,472,919,474,598,344,133,1064,925,123,349,470,483,472,128,474,120,219,0,123,123,223,470,362,472,128,474,523,230,500,120,127,629,123,628,490,491,644,509,107,22,120,513,25,629,27,660,644,107,643,128,470,500,472,132,474,115,116,117,644,399,509,138,45,135,513,142,899,127,124,500,53,143,144,133,693,58,132,133,509,123,500,978,513,66,716,68,635,829,71,509,539,689,120,513,124,123,135,80,813,716,698,84,410,600,143,144,438,120,121,979,944,640,95,728,829,423,948,949,426,954,955,618,674,706,107,108,655,459,111,658,120,127,115,123,883,803,942,720,721,122,690,124,125,126,127,956,957,947,684,686,686,483,114,493,494,627,771,497,945,946,500,128,145,831,950,951,952,953,127,120,90,91,123,915,133,647,135,136,137,651,636,107,731,128,123,120,169,132,107,128,116,651,941,120,120,107,123,116,747,118,915,127,651,115,116,117,123,410,127,135,136,137,684,762,686,127,135,136,137,107,423,133,120,135,136,137,778,115,116,117,127,651,784,121,127,684,219,686,221,768,223,123,107,123,107,108,128,230,128,917,918,116,115,684,703,686,924,123,120,127,120,123,127,123,684,122,686,118,855,107,135,136,137,256,124,123,92,115,116,117,128,25,26,266,899,120,137,127,123,127,273,127,275,633,864,278,40,148,127,123,608,123,1051,611,128,903,128,127,640,123,123,294,882,123,128,128,123,140,128,127,988,128,905,655,992,123,658,123,86,87,128,170,128,172,173,887,676,677,890,120,119,127,123,127,327,184,185,883,92,127,123,127,191,192,193,128,17,18,19,978,135,136,137,107,108,127,863,111,112,113,149,127,116,212,433,434,435,121,79,80,139,933,233,127,235,129,130,141,958,959,960,135,5,6,138,122,123,141,142,121,929,145,146,22,23,941,122,123,611,122,123,883,107,910,966,967,57,107,970,971,271,973,140,141,90,91,40,410,86,87,1000,122,123,127,883,1050,107,776,777,936,423,938,126,426,122,785,786,128,788,122,790,65,883,94,95,96,97,98,99,100,101,102,103,883,1047,122,122,122,121,941,810,121,107,781,120,1027,459,131,1030,462,1032,1033,127,124,127,982,120,470,267,472,329,474,941,107,108,109,110,111,112,113,483,127,116,127,1056,122,489,138,124,107,941,131,134,127,118,1051,130,128,128,122,93,941,361,863,363,364,365,366,367,368,369,370,371,372,373,374,375,376,377,378,379,320,107,128,1041,125,122,126,126,889,131,123,892,893,128,123,122,336,128,123,339,340,341,342,343,107,127,346,107,121,409,127,120,115,116,117,355,128,781,1051,359,127,123,123,121,127,131,438,30,131,120,44,127,135,136,137,936,76,123,128,138,380,1051,943,128,128,107,128,459,131,123,123,123,392,123,466,123,134,397,122,1051,961,128,128,128,608,128,122,611,128,128,1051,128,120,128,975,128,134,128,121,121,127,982,127,122,484,120,71,72,73,74,75,128,107,78,79,80,640,120,510,439,115,116,117,118,122,77,126,651,121,521,25,655,127,125,658,107,123,128,530,128,135,136,137,667,128,131,134,128,128,128,128,127,134,121,128,122,120,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,127,114,24,25,26,128,128,127,123,128,566,128,508,108,510,37,38,39,40,41,42,128,120,128,46,47,122,128,122,51,52,53,54,55,56,28,58,59,60,61,121,127,131,65,66,67,68,69,70,92,92,139,122,25,123,122,122,553,123,81,82,629,84,85,128,120,128,128,768,127,108,122,640,123,128,40,644,122,0,128,128,781,128,0,128,107,108,109,110,111,112,113,114,115,116,117,118,119,120,929,117,107,37,599,660,127,162,129,130,115,116,117,71,135,658,266,138,40,639,141,142,127,271,145,146,49,978,92,703,135,136,137,698,693,630,411,608,230,426,104,365,637,107,108,- 1,1047,111,112,113,645,115,116,- 1,781,348,- 1,121,- 1,- 1,- 1,716,- 1,127,- 1,129,130,- 1,132,133,92,135,- 1,728,- 1,- 1,671,141,- 1,- 1,- 1,- 1,104,- 1,- 1,107,108,- 1,- 1,111,112,113,- 1,115,116,- 1,- 1,- 1,- 1,121,- 1,- 1,- 1,- 1,- 1,127,773,129,130,- 1,132,133,- 1,135,709,- 1,771,712,713,141,715,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,724,929,- 1,727,- 1,- 1,- 1,- 1,- 1,805,- 1,- 1,- 1,- 1,- 1,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,840,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,855,45,- 1,47,- 1,49,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,- 1,861,63,64,864,66,- 1,- 1,69,- 1,- 1,- 1,- 1,813,- 1,- 1,- 1,- 1,- 1,- 1,81,82,882,- 1,- 1,- 1,- 1,- 1,- 1,829,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,899,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,- 1,- 1,- 1,122,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,136,137,- 1,- 1,- 1,- 1,- 1,942,- 1,944,945,946,947,948,949,950,951,952,953,954,955,956,957,958,959,960,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,915,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1000,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,1050,51,52,53,54,55,56,- 1,58,59,60,61,- 1,- 1,- 1,65,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,- 1,51,52,53,54,55,56,- 1,58,59,60,61,- 1,- 1,- 1,65,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,- 1,51,52,53,54,55,56,- 1,58,59,60,61,- 1,- 1,- 1,65,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,- 1,51,52,53,54,55,56,- 1,58,59,60,61,- 1,- 1,- 1,65,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,- 1,- 1,65,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,- 1,- 1,65,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,49,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,- 1,- 1,63,64,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,- 1,- 1,- 1,122,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,136,137,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,49,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,- 1,- 1,63,64,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,136,137,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,49,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,- 1,- 1,63,64,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,40,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,37,38,39,122,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,51,- 1,135,136,137,92,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,104,69,- 1,107,108,- 1,- 1,111,112,113,- 1,- 1,116,81,82,- 1,- 1,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,- 1,- 1,104,141,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,40,120,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,133,- 1,135,136,137,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,- 1,104,- 1,- 1,107,108,58,59,111,112,113,- 1,- 1,116,66,- 1,- 1,69,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,81,82,- 1,135,- 1,- 1,- 1,- 1,- 1,141,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,40,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,133,40,135,136,137,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,107,108,- 1,- 1,111,112,113,- 1,- 1,116,- 1,51,- 1,- 1,121,- 1,92,- 1,58,59,127,- 1,129,130,- 1,- 1,66,- 1,135,69,- 1,107,108,- 1,141,111,112,113,- 1,- 1,116,81,82,- 1,- 1,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,- 1,- 1,- 1,141,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,136,137,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,130,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,114,115,- 1,117,118,- 1,- 1,121,37,38,39,- 1,- 1,- 1,- 1,129,130,- 1,47,- 1,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,25,26,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,40,- 1,42,81,82,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,104,67,68,107,70,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,- 1,120,- 1,84,85,- 1,- 1,- 1,- 1,- 1,129,130,8,9,10,11,12,13,14,15,16,17,18,19,20,21,107,108,109,110,111,112,113,- 1,115,116,- 1,- 1,119,- 1,121,37,38,39,- 1,- 1,127,- 1,- 1,130,- 1,132,133,- 1,135,51,- 1,138,- 1,- 1,141,142,58,59,145,146,62,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,- 1,- 1,121,37,38,39,- 1,- 1,- 1,- 1,129,130,- 1,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,25,26,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,40,- 1,42,81,82,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,104,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,- 1,120,- 1,84,85,- 1,- 1,- 1,- 1,- 1,129,130,8,9,10,11,12,13,14,15,16,17,18,19,20,21,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,37,38,39,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,135,51,- 1,138,- 1,- 1,141,142,58,59,145,146,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,37,38,39,122,- 1,- 1,- 1,- 1,- 1,- 1,129,130,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,37,38,39,122,- 1,- 1,- 1,- 1,- 1,- 1,129,130,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,- 1,40,- 1,42,43,- 1,- 1,46,114,48,- 1,- 1,118,52,53,54,55,56,- 1,- 1,- 1,- 1,61,129,130,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,120,121,25,26,- 1,- 1,- 1,127,- 1,32,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,115,116,- 1,- 1,119,- 1,121,122,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,132,133,- 1,135,25,26,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,- 1,- 1,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,115,116,- 1,- 1,119,- 1,121,122,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,132,133,- 1,135,25,26,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,- 1,- 1,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,115,116,- 1,- 1,119,- 1,121,122,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,132,133,- 1,135,25,26,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,- 1,- 1,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,115,116,- 1,- 1,119,- 1,121,122,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,132,133,- 1,135,25,26,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,- 1,- 1,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,121,25,26,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,120,- 1,25,26,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,25,26,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,25,26,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,25,26,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,25,26,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,25,26,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,120,- 1,25,26,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,120,- 1,25,26,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,25,26,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,25,26,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,120,- 1,25,26,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,25,26,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,25,26,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,25,26,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,25,26,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,25,26,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,40,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,121,25,26,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,121,25,26,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,40,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,40,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,40,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146};char Cyc_Yystack_overflow[17U]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _dyneither_ptr,int state,int token);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple27{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple28{struct _dyneither_ptr f1;void*f2;};static char _tmp4E2[8U]="stdcall";static char _tmp4E3[6U]="cdecl";static char _tmp4E4[9U]="fastcall";static char _tmp4E5[9U]="noreturn";static char _tmp4E6[6U]="const";static char _tmp4E7[8U]="aligned";static char _tmp4E8[7U]="packed";static char _tmp4E9[7U]="shared";static char _tmp4EA[7U]="unused";static char _tmp4EB[5U]="weak";static char _tmp4EC[10U]="dllimport";static char _tmp4ED[10U]="dllexport";static char _tmp4EE[23U]="no_instrument_function";static char _tmp4EF[12U]="constructor";static char _tmp4F0[11U]="destructor";static char _tmp4F1[22U]="no_check_memory_usage";static char _tmp4F2[5U]="pure";static char _tmp4F3[14U]="always_inline";struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 137 "cycbison.simple"
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 140
struct _RegionHandle _tmp428=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp428;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 146
int yychar;
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _tmp83A;(_tmp83A.YYINITIALSVAL).val=0;(_tmp83A.YYINITIALSVAL).tag=64;_tmp83A;});
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
struct _dyneither_ptr yyss=({unsigned int _tmp838=200U;short*_tmp839=_region_calloc(yyregion,sizeof(short),_tmp838);_tag_dyneither(_tmp839,sizeof(short),_tmp838);});
# 158
int yyvsp_offset;
struct _dyneither_ptr yyvs=_tag_dyneither(({unsigned int _tmp835=(unsigned int)200;union Cyc_YYSTYPE*_tmp836=(union Cyc_YYSTYPE*)({struct _RegionHandle*_tmp93D=yyregion;_region_malloc(_tmp93D,_check_times(sizeof(union Cyc_YYSTYPE),_tmp835));});{unsigned int _tmp837=_tmp835;unsigned int i;for(i=0;i < _tmp837;i ++){_tmp836[i]=(union Cyc_YYSTYPE)yylval;}}_tmp836;}),sizeof(union Cyc_YYSTYPE),(unsigned int)200);
# 163
int yylsp_offset;
struct _dyneither_ptr yyls=_tag_dyneither(({unsigned int _tmp832=(unsigned int)200;struct Cyc_Yyltype*_tmp833=(struct Cyc_Yyltype*)({struct _RegionHandle*_tmp93E=yyregion;_region_malloc(_tmp93E,_check_times(sizeof(struct Cyc_Yyltype),_tmp832));});{unsigned int _tmp834=_tmp832;unsigned int i;for(i=0;i < _tmp834;i ++){({typeof((struct Cyc_Yyltype)Cyc_yynewloc())_tmp93F=(struct Cyc_Yyltype)Cyc_yynewloc();_tmp833[i]=_tmp93F;});}}_tmp833;}),sizeof(struct Cyc_Yyltype),(unsigned int)200);
# 170
int yystacksize=200;
# 172
union Cyc_YYSTYPE yyval=yylval;
# 176
int yylen;
# 183
yystate=0;
yyerrstatus=0;
yynerrs=0;
yychar=-2;
# 193
yyssp_offset=-1;
yyvsp_offset=0;
# 196
yylsp_offset=0;
# 202
yynewstate:
# 204
*((short*)_check_dyneither_subscript(yyss,sizeof(short),++ yyssp_offset))=(short)yystate;
# 206
if(yyssp_offset >= yystacksize - 1){
# 208
if(yystacksize >= 10000){
({struct _dyneither_ptr _tmp941=({const char*_tmp429="parser stack overflow";_tag_dyneither(_tmp429,sizeof(char),22U);});int _tmp940=yystate;Cyc_yyerror(_tmp941,_tmp940,yychar);});
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _dyneither_ptr yyss1=({unsigned int _tmp432=(unsigned int)yystacksize;short*_tmp433=(short*)({struct _RegionHandle*_tmp942=yyregion;_region_malloc(_tmp942,_check_times(sizeof(short),_tmp432));});struct _dyneither_ptr _tmp435=_tag_dyneither(_tmp433,sizeof(short),_tmp432);{unsigned int _tmp434=_tmp432;unsigned int i;for(i=0;i < _tmp434;i ++){_tmp433[i]=(short)(
i <= yyssp_offset?*((short*)_check_dyneither_subscript(yyss,sizeof(short),(int)i)): 0);}}_tmp435;});
struct _dyneither_ptr yyvs1=({unsigned int _tmp42E=(unsigned int)yystacksize;union Cyc_YYSTYPE*_tmp42F=(union Cyc_YYSTYPE*)({struct _RegionHandle*_tmp943=yyregion;_region_malloc(_tmp943,_check_times(sizeof(union Cyc_YYSTYPE),_tmp42E));});struct _dyneither_ptr _tmp431=_tag_dyneither(_tmp42F,sizeof(union Cyc_YYSTYPE),_tmp42E);{unsigned int _tmp430=_tmp42E;unsigned int i;for(i=0;i < _tmp430;i ++){_tmp42F[i]=(union Cyc_YYSTYPE)(
# 219
i <= yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(int)i)): yylval);}}_tmp431;});
# 221
struct _dyneither_ptr yyls1=({unsigned int _tmp42A=(unsigned int)yystacksize;struct Cyc_Yyltype*_tmp42B=(struct Cyc_Yyltype*)({struct _RegionHandle*_tmp944=yyregion;_region_malloc(_tmp944,_check_times(sizeof(struct Cyc_Yyltype),_tmp42A));});struct _dyneither_ptr _tmp42D=_tag_dyneither(_tmp42B,sizeof(struct Cyc_Yyltype),_tmp42A);{unsigned int _tmp42C=_tmp42A;unsigned int i;for(i=0;i < _tmp42C;i ++){({typeof((struct Cyc_Yyltype)(
i <= yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(int)i)):
 Cyc_yynewloc()))_tmp945=(struct Cyc_Yyltype)(
# 222
i <= yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(int)i)):
 Cyc_yynewloc());_tmp42B[i]=_tmp945;});}}_tmp42D;});
# 225
yyss=yyss1;
yyvs=yyvs1;
# 228
yyls=yyls1;};}
# 238
goto yybackup;
# 240
yybackup:
# 252 "cycbison.simple"
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1074U,yystate)];
if(yyn == - 32768)goto yydefault;
# 259
if(yychar == - 2)
# 265
({int _tmp946=Cyc_yylex(yylex_buf,& yylval,& yylloc);yychar=_tmp946;});
# 269
if(yychar <= 0){
# 271
yychar1=0;
yychar=0;}else{
# 280
yychar1=yychar > 0  && yychar <= 374?(int)Cyc_yytranslate[_check_known_subscript_notnull(375U,yychar)]: 294;}
# 297 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0  || yyn > 7177) || Cyc_yycheck[_check_known_subscript_notnull(7178U,yyn)]!= yychar1)goto yydefault;
# 300
yyn=(int)Cyc_yytable[yyn];
# 307
if(yyn < 0){
# 309
if(yyn == - 32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 313
if(yyn == 0)goto yyerrlab;}
# 315
if(yyn == 1073){
int _tmp436=0;_npop_handler(0U);return _tmp436;}
# 326 "cycbison.simple"
if(yychar != 0)
yychar=-2;
# 329
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 331
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 336
if(yyerrstatus != 0)-- yyerrstatus;
# 338
yystate=yyn;
goto yynewstate;
# 342
yydefault:
# 344
 yyn=(int)Cyc_yydefact[yystate];
if(yyn == 0)goto yyerrlab;
# 349
yyreduce:
# 351
 yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(528U,yyn)];
if(yylen > 0)
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(yyvsp_offset + 1)- yylen));
# 369 "cycbison.simple"
{int _tmp437=yyn;switch(_tmp437){case 1U: _LL255: _LL256: {
# 372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1194 "parse.y"
yyval=yyyyvsp[0];
({struct Cyc_List_List*_tmp947=Cyc_yyget_YY16(yyyyvsp[0]);Cyc_Parse_parse_result=_tmp947;});
# 1197
goto _LL254;}case 2U: _LL257: _LL258: {
# 1199
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1201
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1200 "parse.y"
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp438;_push_handler(& _tmp438);{int _tmp43A=0;if(setjmp(_tmp438.handler))_tmp43A=1;if(!_tmp43A){({struct Cyc_List_List*_tmp948=Cyc_yyget_YY16(yyyyvsp[0]);x=_tmp948;});;_pop_handler();}else{void*_tmp439=(void*)_exn_thrown;void*_tmp43B=_tmp439;void*_tmp43C;if(((struct Cyc_Core_Failure_exn_struct*)_tmp43B)->tag == Cyc_Core_Failure){_LL672: _LL673:
 x=0;goto _LL671;}else{_LL674: _tmp43C=_tmp43B;_LL675:(int)_rethrow(_tmp43C);}_LL671:;}};}
{struct _handler_cons _tmp43D;_push_handler(& _tmp43D);{int _tmp43F=0;if(setjmp(_tmp43D.handler))_tmp43F=1;if(!_tmp43F){({struct Cyc_List_List*_tmp949=Cyc_yyget_YY16(yyyyvsp[1]);y=_tmp949;});;_pop_handler();}else{void*_tmp43E=(void*)_exn_thrown;void*_tmp440=_tmp43E;void*_tmp441;if(((struct Cyc_Core_Failure_exn_struct*)_tmp440)->tag == Cyc_Core_Failure){_LL677: _LL678:
 y=0;goto _LL676;}else{_LL679: _tmp441=_tmp440;_LL67A:(int)_rethrow(_tmp441);}_LL676:;}};}
({union Cyc_YYSTYPE _tmp94A=Cyc_YY16(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));yyval=_tmp94A;});
# 1207
goto _LL254;}case 3U: _LL259: _LL25A: {
# 1209
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1211
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1210 "parse.y"
({union Cyc_YYSTYPE _tmp950=Cyc_YY16(({struct Cyc_List_List*_tmp442=_cycalloc(sizeof(*_tmp442));({struct Cyc_Absyn_Decl*_tmp94F=({struct Cyc_Absyn_Decl*_tmp443=_cycalloc(sizeof(*_tmp443));({void*_tmp94E=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp444=_cycalloc(sizeof(*_tmp444));({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp94D=({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp445;_tmp445.tag=10U;({struct _tuple0*_tmp94B=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp445.f1=_tmp94B;});({struct Cyc_List_List*_tmp94C=Cyc_yyget_YY16(yyyyvsp[2]);_tmp445.f2=_tmp94C;});_tmp445;});_tmp444[0]=_tmp94D;});_tmp444;});_tmp443->r=_tmp94E;});_tmp443->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp443;});_tmp442->hd=_tmp94F;});_tmp442->tl=0;_tmp442;}));yyval=_tmp950;});
Cyc_Lex_leave_using();
# 1213
goto _LL254;}case 4U: _LL25B: _LL25C: {
# 1215
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1217
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1214 "parse.y"
({union Cyc_YYSTYPE _tmp957=Cyc_YY16(({struct Cyc_List_List*_tmp446=_cycalloc(sizeof(*_tmp446));({struct Cyc_Absyn_Decl*_tmp955=({struct Cyc_Absyn_Decl*_tmp447=_cycalloc(sizeof(*_tmp447));({void*_tmp954=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp448=_cycalloc(sizeof(*_tmp448));({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp953=({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp449;_tmp449.tag=10U;({struct _tuple0*_tmp951=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp449.f1=_tmp951;});({struct Cyc_List_List*_tmp952=Cyc_yyget_YY16(yyyyvsp[2]);_tmp449.f2=_tmp952;});_tmp449;});_tmp448[0]=_tmp953;});_tmp448;});_tmp447->r=_tmp954;});_tmp447->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp447;});_tmp446->hd=_tmp955;});({struct Cyc_List_List*_tmp956=Cyc_yyget_YY16(yyyyvsp[4]);_tmp446->tl=_tmp956;});_tmp446;}));yyval=_tmp957;});
goto _LL254;}case 5U: _LL25D: _LL25E: {
# 1217
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1219
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1217 "parse.y"
({union Cyc_YYSTYPE _tmp95E=Cyc_YY16(({struct Cyc_List_List*_tmp44A=_cycalloc(sizeof(*_tmp44A));({struct Cyc_Absyn_Decl*_tmp95D=({struct Cyc_Absyn_Decl*_tmp44B=_cycalloc(sizeof(*_tmp44B));({void*_tmp95C=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp44C=_cycalloc(sizeof(*_tmp44C));({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp95B=({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp44D;_tmp44D.tag=9U;({struct _dyneither_ptr*_tmp959=({struct _dyneither_ptr*_tmp44E=_cycalloc(sizeof(*_tmp44E));({struct _dyneither_ptr _tmp958=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp44E[0]=_tmp958;});_tmp44E;});_tmp44D.f1=_tmp959;});({struct Cyc_List_List*_tmp95A=Cyc_yyget_YY16(yyyyvsp[2]);_tmp44D.f2=_tmp95A;});_tmp44D;});_tmp44C[0]=_tmp95B;});_tmp44C;});_tmp44B->r=_tmp95C;});_tmp44B->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp44B;});_tmp44A->hd=_tmp95D;});_tmp44A->tl=0;_tmp44A;}));yyval=_tmp95E;});
Cyc_Lex_leave_namespace();
# 1220
goto _LL254;}case 6U: _LL25F: _LL260: {
# 1222
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1224
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1222 "parse.y"
struct _dyneither_ptr nspace;
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp44F;_push_handler(& _tmp44F);{int _tmp451=0;if(setjmp(_tmp44F.handler))_tmp451=1;if(!_tmp451){({struct _dyneither_ptr _tmp95F=Cyc_yyget_String_tok(yyyyvsp[0]);nspace=_tmp95F;});;_pop_handler();}else{void*_tmp450=(void*)_exn_thrown;void*_tmp452=_tmp450;void*_tmp454;if(((struct Cyc_Core_Failure_exn_struct*)_tmp452)->tag == Cyc_Core_Failure){_LL67C: _LL67D:
({struct _dyneither_ptr _tmp960=({const char*_tmp453="";_tag_dyneither(_tmp453,sizeof(char),1U);});nspace=_tmp960;});goto _LL67B;}else{_LL67E: _tmp454=_tmp452;_LL67F:(int)_rethrow(_tmp454);}_LL67B:;}};}
# 1227
{struct _handler_cons _tmp455;_push_handler(& _tmp455);{int _tmp457=0;if(setjmp(_tmp455.handler))_tmp457=1;if(!_tmp457){({struct Cyc_List_List*_tmp961=Cyc_yyget_YY16(yyyyvsp[2]);x=_tmp961;});;_pop_handler();}else{void*_tmp456=(void*)_exn_thrown;void*_tmp458=_tmp456;void*_tmp459;if(((struct Cyc_Core_Failure_exn_struct*)_tmp458)->tag == Cyc_Core_Failure){_LL681: _LL682:
 x=0;goto _LL680;}else{_LL683: _tmp459=_tmp458;_LL684:(int)_rethrow(_tmp459);}_LL680:;}};}
# 1230
{struct _handler_cons _tmp45A;_push_handler(& _tmp45A);{int _tmp45C=0;if(setjmp(_tmp45A.handler))_tmp45C=1;if(!_tmp45C){({struct Cyc_List_List*_tmp962=Cyc_yyget_YY16(yyyyvsp[4]);y=_tmp962;});;_pop_handler();}else{void*_tmp45B=(void*)_exn_thrown;void*_tmp45D=_tmp45B;void*_tmp45E;if(((struct Cyc_Core_Failure_exn_struct*)_tmp45D)->tag == Cyc_Core_Failure){_LL686: _LL687:
 y=0;goto _LL685;}else{_LL688: _tmp45E=_tmp45D;_LL689:(int)_rethrow(_tmp45E);}_LL685:;}};}
# 1233
({union Cyc_YYSTYPE _tmp967=Cyc_YY16(({struct Cyc_List_List*_tmp45F=_cycalloc(sizeof(*_tmp45F));({struct Cyc_Absyn_Decl*_tmp966=({struct Cyc_Absyn_Decl*_tmp460=_cycalloc(sizeof(*_tmp460));({void*_tmp965=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp461=_cycalloc(sizeof(*_tmp461));({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp964=({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp462;_tmp462.tag=9U;({struct _dyneither_ptr*_tmp963=({struct _dyneither_ptr*_tmp463=_cycalloc(sizeof(*_tmp463));_tmp463[0]=nspace;_tmp463;});_tmp462.f1=_tmp963;});_tmp462.f2=x;_tmp462;});_tmp461[0]=_tmp964;});_tmp461;});_tmp460->r=_tmp965;});_tmp460->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp460;});_tmp45F->hd=_tmp966;});_tmp45F->tl=y;_tmp45F;}));yyval=_tmp967;});
# 1235
goto _LL254;}case 7U: _LL261: _LL262: {
# 1237
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1239
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1236 "parse.y"
int _tmp464=Cyc_yyget_YY31(yyyyvsp[0]);
if(!_tmp464)
({union Cyc_YYSTYPE _tmp96D=Cyc_YY16(({struct Cyc_List_List*_tmp465=_cycalloc(sizeof(*_tmp465));({struct Cyc_Absyn_Decl*_tmp96B=({struct Cyc_Absyn_Decl*_tmp466=_cycalloc(sizeof(*_tmp466));({void*_tmp96A=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp467=_cycalloc(sizeof(*_tmp467));({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct _tmp969=({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct _tmp468;_tmp468.tag=11U;({struct Cyc_List_List*_tmp968=Cyc_yyget_YY16(yyyyvsp[2]);_tmp468.f1=_tmp968;});_tmp468;});_tmp467[0]=_tmp969;});_tmp467;});_tmp466->r=_tmp96A;});_tmp466->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp466;});_tmp465->hd=_tmp96B;});({struct Cyc_List_List*_tmp96C=Cyc_yyget_YY16(yyyyvsp[4]);_tmp465->tl=_tmp96C;});_tmp465;}));yyval=_tmp96D;});else{
# 1240
({union Cyc_YYSTYPE _tmp973=Cyc_YY16(({struct Cyc_List_List*_tmp469=_cycalloc(sizeof(*_tmp469));({struct Cyc_Absyn_Decl*_tmp971=({struct Cyc_Absyn_Decl*_tmp46A=_cycalloc(sizeof(*_tmp46A));({void*_tmp970=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp46B=_cycalloc(sizeof(*_tmp46B));({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmp96F=({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmp46C;_tmp46C.tag=12U;({struct Cyc_List_List*_tmp96E=Cyc_yyget_YY16(yyyyvsp[2]);_tmp46C.f1=_tmp96E;});_tmp46C.f2=0;_tmp46C;});_tmp46B[0]=_tmp96F;});_tmp46B;});_tmp46A->r=_tmp970;});_tmp46A->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp46A;});_tmp469->hd=_tmp971;});({struct Cyc_List_List*_tmp972=Cyc_yyget_YY16(yyyyvsp[4]);_tmp469->tl=_tmp972;});_tmp469;}));yyval=_tmp973;});}
# 1242
goto _LL254;}case 8U: _LL263: _LL264: {
# 1244
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1246
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1243 "parse.y"
if(!Cyc_yyget_YY31(yyyyvsp[0]))
({void*_tmp46D=0U;({unsigned int _tmp975=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp974=({const char*_tmp46E="expecting \"C include\"";_tag_dyneither(_tmp46E,sizeof(char),22U);});Cyc_Warn_err(_tmp975,_tmp974,_tag_dyneither(_tmp46D,sizeof(void*),0U));});});{
struct Cyc_List_List*exs=Cyc_yyget_YY52(yyyyvsp[4]);
({union Cyc_YYSTYPE _tmp97B=Cyc_YY16(({struct Cyc_List_List*_tmp46F=_cycalloc(sizeof(*_tmp46F));({struct Cyc_Absyn_Decl*_tmp979=({struct Cyc_Absyn_Decl*_tmp470=_cycalloc(sizeof(*_tmp470));({void*_tmp978=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp471=_cycalloc(sizeof(*_tmp471));({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmp977=({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmp472;_tmp472.tag=12U;({struct Cyc_List_List*_tmp976=Cyc_yyget_YY16(yyyyvsp[2]);_tmp472.f1=_tmp976;});_tmp472.f2=exs;_tmp472;});_tmp471[0]=_tmp977;});_tmp471;});_tmp470->r=_tmp978;});_tmp470->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp470;});_tmp46F->hd=_tmp979;});({struct Cyc_List_List*_tmp97A=Cyc_yyget_YY16(yyyyvsp[5]);_tmp46F->tl=_tmp97A;});_tmp46F;}));yyval=_tmp97B;});
# 1248
goto _LL254;};}case 9U: _LL265: _LL266: {
# 1250
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1252
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1249 "parse.y"
({union Cyc_YYSTYPE _tmp97E=Cyc_YY16(({struct Cyc_List_List*_tmp473=_cycalloc(sizeof(*_tmp473));({struct Cyc_Absyn_Decl*_tmp97C=({struct Cyc_Absyn_Decl*_tmp474=_cycalloc(sizeof(*_tmp474));_tmp474->r=(void*)& Cyc_Absyn_Porton_d_val;_tmp474->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp474;});_tmp473->hd=_tmp97C;});({struct Cyc_List_List*_tmp97D=Cyc_yyget_YY16(yyyyvsp[2]);_tmp473->tl=_tmp97D;});_tmp473;}));yyval=_tmp97E;});
goto _LL254;}case 10U: _LL267: _LL268: {
# 1252
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1254
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1251 "parse.y"
({union Cyc_YYSTYPE _tmp981=Cyc_YY16(({struct Cyc_List_List*_tmp475=_cycalloc(sizeof(*_tmp475));({struct Cyc_Absyn_Decl*_tmp97F=({struct Cyc_Absyn_Decl*_tmp476=_cycalloc(sizeof(*_tmp476));_tmp476->r=(void*)& Cyc_Absyn_Portoff_d_val;_tmp476->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp476;});_tmp475->hd=_tmp97F;});({struct Cyc_List_List*_tmp980=Cyc_yyget_YY16(yyyyvsp[2]);_tmp475->tl=_tmp980;});_tmp475;}));yyval=_tmp981;});
goto _LL254;}case 11U: _LL269: _LL26A:
# 1254
({union Cyc_YYSTYPE _tmp982=Cyc_YY16(0);yyval=_tmp982;});
goto _LL254;case 12U: _LL26B: _LL26C: {
# 1257
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1259
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1257 "parse.y"
struct _dyneither_ptr _tmp477=Cyc_yyget_String_tok(yyyyvsp[1]);
Cyc_Lex_enter_extern_c();
if(({struct _dyneither_ptr _tmp983=(struct _dyneither_ptr)_tmp477;Cyc_strcmp(_tmp983,({const char*_tmp478="C";_tag_dyneither(_tmp478,sizeof(char),2U);}));})== 0)
({union Cyc_YYSTYPE _tmp984=Cyc_YY31(0);yyval=_tmp984;});else{
if(({struct _dyneither_ptr _tmp985=(struct _dyneither_ptr)_tmp477;Cyc_strcmp(_tmp985,({const char*_tmp479="C include";_tag_dyneither(_tmp479,sizeof(char),10U);}));})== 0)
({union Cyc_YYSTYPE _tmp986=Cyc_YY31(1);yyval=_tmp986;});else{
# 1264
({void*_tmp47A=0U;({unsigned int _tmp988=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp987=({const char*_tmp47B="expecting \"C\" or \"C include\"";_tag_dyneither(_tmp47B,sizeof(char),29U);});Cyc_Warn_err(_tmp988,_tmp987,_tag_dyneither(_tmp47A,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmp989=Cyc_YY31(1);yyval=_tmp989;});}}
# 1268
goto _LL254;}case 13U: _LL26D: _LL26E: {
# 1270
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1272
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1271 "parse.y"
Cyc_Lex_leave_extern_c();
goto _LL254;}case 14U: _LL26F: _LL270: {
# 1274
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1276
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1275 "parse.y"
yyval=yyyyvsp[2];
goto _LL254;}case 15U: _LL271: _LL272: {
# 1278
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1280
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1276 "parse.y"
({union Cyc_YYSTYPE _tmp98A=Cyc_YY52(0);yyval=_tmp98A;});
goto _LL254;}case 16U: _LL273: _LL274: {
# 1279
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1281
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1280 "parse.y"
({union Cyc_YYSTYPE _tmp98D=Cyc_YY52(({struct Cyc_List_List*_tmp47C=_cycalloc(sizeof(*_tmp47C));({struct _tuple27*_tmp98C=({struct _tuple27*_tmp47D=_cycalloc(sizeof(*_tmp47D));_tmp47D->f1=(unsigned int)(yyyylsp[0]).first_line;({struct _tuple0*_tmp98B=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp47D->f2=_tmp98B;});_tmp47D->f3=0;_tmp47D;});_tmp47C->hd=_tmp98C;});_tmp47C->tl=0;_tmp47C;}));yyval=_tmp98D;});
goto _LL254;}case 17U: _LL275: _LL276: {
# 1283
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1285
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1281 "parse.y"
({union Cyc_YYSTYPE _tmp990=Cyc_YY52(({struct Cyc_List_List*_tmp47E=_cycalloc(sizeof(*_tmp47E));({struct _tuple27*_tmp98F=({struct _tuple27*_tmp47F=_cycalloc(sizeof(*_tmp47F));_tmp47F->f1=(unsigned int)(yyyylsp[0]).first_line;({struct _tuple0*_tmp98E=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp47F->f2=_tmp98E;});_tmp47F->f3=0;_tmp47F;});_tmp47E->hd=_tmp98F;});_tmp47E->tl=0;_tmp47E;}));yyval=_tmp990;});
goto _LL254;}case 18U: _LL277: _LL278: {
# 1284
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1286
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1283 "parse.y"
({union Cyc_YYSTYPE _tmp994=Cyc_YY52(({struct Cyc_List_List*_tmp480=_cycalloc(sizeof(*_tmp480));({struct _tuple27*_tmp992=({struct _tuple27*_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481->f1=(unsigned int)(yyyylsp[0]).first_line;({struct _tuple0*_tmp991=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp481->f2=_tmp991;});_tmp481->f3=0;_tmp481;});_tmp480->hd=_tmp992;});({struct Cyc_List_List*_tmp993=Cyc_yyget_YY52(yyyyvsp[2]);_tmp480->tl=_tmp993;});_tmp480;}));yyval=_tmp994;});
goto _LL254;}case 19U: _LL279: _LL27A: {
# 1286
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1288
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1287 "parse.y"
({union Cyc_YYSTYPE _tmp999=Cyc_YY16(({struct Cyc_List_List*_tmp482=_cycalloc(sizeof(*_tmp482));({struct Cyc_Absyn_Decl*_tmp998=({void*_tmp997=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp483=_cycalloc(sizeof(*_tmp483));({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmp996=({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmp484;_tmp484.tag=1U;({struct Cyc_Absyn_Fndecl*_tmp995=Cyc_yyget_YY15(yyyyvsp[0]);_tmp484.f1=_tmp995;});_tmp484;});_tmp483[0]=_tmp996;});_tmp483;});Cyc_Absyn_new_decl(_tmp997,(unsigned int)(yyyylsp[0]).first_line);});_tmp482->hd=_tmp998;});_tmp482->tl=0;_tmp482;}));yyval=_tmp999;});
goto _LL254;}case 20U: _LL27B: _LL27C: {
# 1290
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1292
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1288 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 21U: _LL27D: _LL27E: {
# 1291
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1293
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1289 "parse.y"
({union Cyc_YYSTYPE _tmp99A=Cyc_YY16(0);yyval=_tmp99A;});
goto _LL254;}case 24U: _LL27F: _LL280: {
# 1292
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1294
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1298 "parse.y"
({union Cyc_YYSTYPE _tmp99E=Cyc_YY15(({struct _RegionHandle*_tmp99D=yyr;struct Cyc_Parse_Declarator _tmp99C=Cyc_yyget_YY27(yyyyvsp[0]);struct Cyc_Absyn_Stmt*_tmp99B=Cyc_yyget_Stmt_tok(yyyyvsp[1]);Cyc_Parse_make_function(_tmp99D,0,_tmp99C,0,_tmp99B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp99E;});
goto _LL254;}case 25U: _LL281: _LL282: {
# 1301
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1303
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1300 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp485=Cyc_yyget_YY17(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmp9A2=Cyc_YY15(({struct _RegionHandle*_tmp9A1=yyr;struct Cyc_Parse_Declarator _tmp9A0=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Absyn_Stmt*_tmp99F=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Parse_make_function(_tmp9A1,& _tmp485,_tmp9A0,0,_tmp99F,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp9A2;});
goto _LL254;}case 26U: _LL283: _LL284: {
# 1304
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1306
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1313 "parse.y"
({union Cyc_YYSTYPE _tmp9A7=Cyc_YY15(({struct _RegionHandle*_tmp9A6=yyr;struct Cyc_Parse_Declarator _tmp9A5=Cyc_yyget_YY27(yyyyvsp[0]);struct Cyc_List_List*_tmp9A4=Cyc_yyget_YY16(yyyyvsp[1]);struct Cyc_Absyn_Stmt*_tmp9A3=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Parse_make_function(_tmp9A6,0,_tmp9A5,_tmp9A4,_tmp9A3,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp9A7;});
goto _LL254;}case 27U: _LL285: _LL286: {
# 1316
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1318
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1315 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp486=Cyc_yyget_YY17(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmp9AC=Cyc_YY15(({struct _RegionHandle*_tmp9AB=yyr;struct Cyc_Parse_Declarator _tmp9AA=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_List_List*_tmp9A9=Cyc_yyget_YY16(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmp9A8=Cyc_yyget_Stmt_tok(yyyyvsp[3]);Cyc_Parse_make_function(_tmp9AB,& _tmp486,_tmp9AA,_tmp9A9,_tmp9A8,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp9AC;});
goto _LL254;}case 28U: _LL287: _LL288: {
# 1319
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1321
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1323 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp487=Cyc_yyget_YY17(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmp9B0=Cyc_YY15(({struct _RegionHandle*_tmp9AF=yyr;struct Cyc_Parse_Declarator _tmp9AE=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Absyn_Stmt*_tmp9AD=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Parse_make_function(_tmp9AF,& _tmp487,_tmp9AE,0,_tmp9AD,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp9B0;});
goto _LL254;}case 29U: _LL289: _LL28A: {
# 1327
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1329
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1326 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp488=Cyc_yyget_YY17(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmp9B5=Cyc_YY15(({struct _RegionHandle*_tmp9B4=yyr;struct Cyc_Parse_Declarator _tmp9B3=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_List_List*_tmp9B2=Cyc_yyget_YY16(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmp9B1=Cyc_yyget_Stmt_tok(yyyyvsp[3]);Cyc_Parse_make_function(_tmp9B4,& _tmp488,_tmp9B3,_tmp9B2,_tmp9B1,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp9B5;});
goto _LL254;}case 30U: _LL28B: _LL28C: {
# 1330
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1332
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1331 "parse.y"
Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyyyvsp[1]));yyval=yyyyvsp[1];
goto _LL254;}case 31U: _LL28D: _LL28E: {
# 1334
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1336
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1334 "parse.y"
Cyc_Lex_leave_using();
goto _LL254;}case 32U: _LL28F: _LL290: {
# 1337
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1339
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1337 "parse.y"
Cyc_Lex_enter_namespace(({struct _dyneither_ptr*_tmp489=_cycalloc(sizeof(*_tmp489));({struct _dyneither_ptr _tmp9B6=Cyc_yyget_String_tok(yyyyvsp[1]);_tmp489[0]=_tmp9B6;});_tmp489;}));yyval=yyyyvsp[1];
goto _LL254;}case 33U: _LL291: _LL292: {
# 1340
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1342
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1340 "parse.y"
Cyc_Lex_leave_namespace();
goto _LL254;}case 34U: _LL293: _LL294: {
# 1343
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1345
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
int _tmp48A=(yyyylsp[0]).first_line;
({union Cyc_YYSTYPE _tmp9B9=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp9B8=Cyc_yyget_YY17(yyyyvsp[0]);unsigned int _tmp9B7=(unsigned int)_tmp48A;Cyc_Parse_make_declarations(_tmp9B8,0,_tmp9B7,(unsigned int)_tmp48A);}));yyval=_tmp9B9;});
goto _LL254;}case 35U: _LL295: _LL296: {
# 1350
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1352
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1349 "parse.y"
int _tmp48B=(yyyylsp[0]).first_line;
({union Cyc_YYSTYPE _tmp9BD=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp9BC=Cyc_yyget_YY17(yyyyvsp[0]);struct _tuple12*_tmp9BB=Cyc_yyget_YY19(yyyyvsp[1]);unsigned int _tmp9BA=(unsigned int)(yyyylsp[0]).first_line;Cyc_Parse_make_declarations(_tmp9BC,_tmp9BB,_tmp9BA,(unsigned int)_tmp48B);}));yyval=_tmp9BD;});
goto _LL254;}case 36U: _LL297: _LL298: {
# 1353
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1355
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1353 "parse.y"
({union Cyc_YYSTYPE _tmp9C1=Cyc_YY16(({struct Cyc_List_List*_tmp48C=_cycalloc(sizeof(*_tmp48C));({struct Cyc_Absyn_Decl*_tmp9C0=({struct Cyc_Absyn_Pat*_tmp9BF=Cyc_yyget_YY9(yyyyvsp[1]);struct Cyc_Absyn_Exp*_tmp9BE=Cyc_yyget_Exp_tok(yyyyvsp[3]);Cyc_Absyn_let_decl(_tmp9BF,_tmp9BE,(unsigned int)(yyyylsp[0]).first_line);});_tmp48C->hd=_tmp9C0;});_tmp48C->tl=0;_tmp48C;}));yyval=_tmp9C1;});
goto _LL254;}case 37U: _LL299: _LL29A: {
# 1356
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1358
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1355 "parse.y"
struct Cyc_List_List*_tmp48D=0;
{struct Cyc_List_List*_tmp48E=Cyc_yyget_YY36(yyyyvsp[1]);for(0;_tmp48E != 0;_tmp48E=_tmp48E->tl){
struct _dyneither_ptr*_tmp48F=(struct _dyneither_ptr*)_tmp48E->hd;
struct _tuple0*qv=({struct _tuple0*_tmp492=_cycalloc(sizeof(*_tmp492));({union Cyc_Absyn_Nmspace _tmp9C2=Cyc_Absyn_Rel_n(0);_tmp492->f1=_tmp9C2;});_tmp492->f2=_tmp48F;_tmp492;});
struct Cyc_Absyn_Vardecl*_tmp490=({struct _tuple0*_tmp9C3=qv;Cyc_Absyn_new_vardecl(0U,_tmp9C3,Cyc_Absyn_wildtyp(0),0);});
({struct Cyc_List_List*_tmp9C4=({struct Cyc_List_List*_tmp491=_cycalloc(sizeof(*_tmp491));_tmp491->hd=_tmp490;_tmp491->tl=_tmp48D;_tmp491;});_tmp48D=_tmp9C4;});}}
# 1362
({struct Cyc_List_List*_tmp9C5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp48D);_tmp48D=_tmp9C5;});
({union Cyc_YYSTYPE _tmp9C7=Cyc_YY16(({struct Cyc_List_List*_tmp493=_cycalloc(sizeof(*_tmp493));({struct Cyc_Absyn_Decl*_tmp9C6=Cyc_Absyn_letv_decl(_tmp48D,(unsigned int)(yyyylsp[0]).first_line);_tmp493->hd=_tmp9C6;});_tmp493->tl=0;_tmp493;}));yyval=_tmp9C7;});
# 1365
goto _LL254;}case 38U: _LL29B: _LL29C: {
# 1367
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1369
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1368 "parse.y"
struct _dyneither_ptr _tmp494=Cyc_yyget_String_tok(yyyyvsp[2]);
# 1370
if(({struct _dyneither_ptr _tmp9C8=(struct _dyneither_ptr)_tmp494;Cyc_zstrcmp(_tmp9C8,({const char*_tmp495="`H";_tag_dyneither(_tmp495,sizeof(char),3U);}));})== 0)
({void*_tmp496=0U;({unsigned int _tmp9CA=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp9C9=({const char*_tmp497="bad occurrence of heap region";_tag_dyneither(_tmp497,sizeof(char),30U);});Cyc_Warn_err(_tmp9CA,_tmp9C9,_tag_dyneither(_tmp496,sizeof(void*),0U));});});
if(({struct _dyneither_ptr _tmp9CB=(struct _dyneither_ptr)_tmp494;Cyc_zstrcmp(_tmp9CB,({const char*_tmp498="`U";_tag_dyneither(_tmp498,sizeof(char),3U);}));})== 0)
({void*_tmp499=0U;({unsigned int _tmp9CD=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp9CC=({const char*_tmp49A="bad occurrence of unique region";_tag_dyneither(_tmp49A,sizeof(char),32U);});Cyc_Warn_err(_tmp9CD,_tmp9CC,_tag_dyneither(_tmp499,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp4A2=_cycalloc(sizeof(*_tmp4A2));({struct _dyneither_ptr*_tmp9CE=({struct _dyneither_ptr*_tmp4A3=_cycalloc(sizeof(*_tmp4A3));_tmp4A3[0]=_tmp494;_tmp4A3;});_tmp4A2->name=_tmp9CE;});_tmp4A2->identity=- 1;({void*_tmp9CF=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp4A2->kind=_tmp9CF;});_tmp4A2;});
void*t=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4A0=_cycalloc(sizeof(*_tmp4A0));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp9D0=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4A1;_tmp4A1.tag=2U;_tmp4A1.f1=tv;_tmp4A1;});_tmp4A0[0]=_tmp9D0;});_tmp4A0;});
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmp9D5=(unsigned int)(yyyylsp[4]).first_line;struct _tuple0*_tmp9D4=({struct _tuple0*_tmp49C=_cycalloc(sizeof(*_tmp49C));_tmp49C->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmp9D2=({struct _dyneither_ptr*_tmp49D=_cycalloc(sizeof(*_tmp49D));({struct _dyneither_ptr _tmp9D1=Cyc_yyget_String_tok(yyyyvsp[4]);_tmp49D[0]=_tmp9D1;});_tmp49D;});_tmp49C->f2=_tmp9D2;});_tmp49C;});Cyc_Absyn_new_vardecl(_tmp9D5,_tmp9D4,(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp49E=_cycalloc(sizeof(*_tmp49E));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp9D3=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp49F;_tmp49F.tag=15U;_tmp49F.f1=t;_tmp49F;});_tmp49E[0]=_tmp9D3;});_tmp49E;}),0);});
({union Cyc_YYSTYPE _tmp9D7=Cyc_YY16(({struct Cyc_List_List*_tmp49B=_cycalloc(sizeof(*_tmp49B));({struct Cyc_Absyn_Decl*_tmp9D6=Cyc_Absyn_region_decl(tv,vd,0,(unsigned int)(yyyylsp[0]).first_line);_tmp49B->hd=_tmp9D6;});_tmp49B->tl=0;_tmp49B;}));yyval=_tmp9D7;});
# 1379
goto _LL254;};}case 39U: _LL29D: _LL29E: {
# 1381
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1383
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1381 "parse.y"
struct _dyneither_ptr _tmp4A4=Cyc_yyget_String_tok(yyyyvsp[1]);
if(({struct _dyneither_ptr _tmp9D8=(struct _dyneither_ptr)_tmp4A4;Cyc_zstrcmp(_tmp9D8,({const char*_tmp4A5="H";_tag_dyneither(_tmp4A5,sizeof(char),2U);}));})== 0)
({void*_tmp4A6=0U;({unsigned int _tmp9DA=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmp9D9=({const char*_tmp4A7="bad occurrence of heap region `H";_tag_dyneither(_tmp4A7,sizeof(char),33U);});Cyc_Warn_err(_tmp9DA,_tmp9D9,_tag_dyneither(_tmp4A6,sizeof(void*),0U));});});
if(({struct _dyneither_ptr _tmp9DB=(struct _dyneither_ptr)_tmp4A4;Cyc_zstrcmp(_tmp9DB,({const char*_tmp4A8="U";_tag_dyneither(_tmp4A8,sizeof(char),2U);}));})== 0)
({void*_tmp4A9=0U;({unsigned int _tmp9DD=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmp9DC=({const char*_tmp4AA="bad occurrence of unique region `U";_tag_dyneither(_tmp4AA,sizeof(char),35U);});Cyc_Warn_err(_tmp9DD,_tmp9DC,_tag_dyneither(_tmp4A9,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp4B2=_cycalloc(sizeof(*_tmp4B2));({struct _dyneither_ptr*_tmp9E0=({struct _dyneither_ptr*_tmp4B3=_cycalloc(sizeof(*_tmp4B3));({struct _dyneither_ptr _tmp9DF=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4B6;_tmp4B6.tag=0U;_tmp4B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4A4);({void*_tmp4B4[1U]={& _tmp4B6};({struct _dyneither_ptr _tmp9DE=({const char*_tmp4B5="`%s";_tag_dyneither(_tmp4B5,sizeof(char),4U);});Cyc_aprintf(_tmp9DE,_tag_dyneither(_tmp4B4,sizeof(void*),1U));});});});_tmp4B3[0]=_tmp9DF;});_tmp4B3;});_tmp4B2->name=_tmp9E0;});_tmp4B2->identity=- 1;({void*_tmp9E1=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp4B2->kind=_tmp9E1;});_tmp4B2;});
void*t=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4B0=_cycalloc(sizeof(*_tmp4B0));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp9E2=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4B1;_tmp4B1.tag=2U;_tmp4B1.f1=tv;_tmp4B1;});_tmp4B0[0]=_tmp9E2;});_tmp4B0;});
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmp9E6=(unsigned int)(yyyylsp[1]).first_line;struct _tuple0*_tmp9E5=({struct _tuple0*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));_tmp4AC->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmp9E3=({struct _dyneither_ptr*_tmp4AD=_cycalloc(sizeof(*_tmp4AD));_tmp4AD[0]=_tmp4A4;_tmp4AD;});_tmp4AC->f2=_tmp9E3;});_tmp4AC;});Cyc_Absyn_new_vardecl(_tmp9E6,_tmp9E5,(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4AE=_cycalloc(sizeof(*_tmp4AE));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp9E4=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp4AF;_tmp4AF.tag=15U;_tmp4AF.f1=t;_tmp4AF;});_tmp4AE[0]=_tmp9E4;});_tmp4AE;}),0);});
({union Cyc_YYSTYPE _tmp9E8=Cyc_YY16(({struct Cyc_List_List*_tmp4AB=_cycalloc(sizeof(*_tmp4AB));({struct Cyc_Absyn_Decl*_tmp9E7=Cyc_Absyn_region_decl(tv,vd,0,(unsigned int)(yyyylsp[0]).first_line);_tmp4AB->hd=_tmp9E7;});_tmp4AB->tl=0;_tmp4AB;}));yyval=_tmp9E8;});
# 1392
goto _LL254;};}case 40U: _LL29F: _LL2A0: {
# 1394
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 1396
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 1394 "parse.y"
struct _dyneither_ptr _tmp4B7=Cyc_yyget_String_tok(yyyyvsp[1]);
struct _dyneither_ptr _tmp4B8=Cyc_yyget_String_tok(yyyyvsp[3]);
struct Cyc_Absyn_Exp*_tmp4B9=Cyc_yyget_Exp_tok(yyyyvsp[5]);
if(({struct _dyneither_ptr _tmp9E9=(struct _dyneither_ptr)_tmp4B8;Cyc_strcmp(_tmp9E9,({const char*_tmp4BA="open";_tag_dyneither(_tmp4BA,sizeof(char),5U);}));})!= 0)({void*_tmp4BB=0U;({unsigned int _tmp9EB=(unsigned int)(yyyylsp[3]).first_line;struct _dyneither_ptr _tmp9EA=({const char*_tmp4BC="expecting `open'";_tag_dyneither(_tmp4BC,sizeof(char),17U);});Cyc_Warn_err(_tmp9EB,_tmp9EA,_tag_dyneither(_tmp4BB,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp4C4=_cycalloc(sizeof(*_tmp4C4));({struct _dyneither_ptr*_tmp9EE=({struct _dyneither_ptr*_tmp4C5=_cycalloc(sizeof(*_tmp4C5));({struct _dyneither_ptr _tmp9ED=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4C8;_tmp4C8.tag=0U;_tmp4C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4B7);({void*_tmp4C6[1U]={& _tmp4C8};({struct _dyneither_ptr _tmp9EC=({const char*_tmp4C7="`%s";_tag_dyneither(_tmp4C7,sizeof(char),4U);});Cyc_aprintf(_tmp9EC,_tag_dyneither(_tmp4C6,sizeof(void*),1U));});});});_tmp4C5[0]=_tmp9ED;});_tmp4C5;});_tmp4C4->name=_tmp9EE;});_tmp4C4->identity=- 1;({void*_tmp9EF=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp4C4->kind=_tmp9EF;});_tmp4C4;});
void*t=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4C2=_cycalloc(sizeof(*_tmp4C2));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp9F0=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4C3;_tmp4C3.tag=2U;_tmp4C3.f1=tv;_tmp4C3;});_tmp4C2[0]=_tmp9F0;});_tmp4C2;});
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmp9F4=(unsigned int)(yyyylsp[2]).first_line;struct _tuple0*_tmp9F3=({struct _tuple0*_tmp4BE=_cycalloc(sizeof(*_tmp4BE));_tmp4BE->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmp9F1=({struct _dyneither_ptr*_tmp4BF=_cycalloc(sizeof(*_tmp4BF));_tmp4BF[0]=_tmp4B7;_tmp4BF;});_tmp4BE->f2=_tmp9F1;});_tmp4BE;});Cyc_Absyn_new_vardecl(_tmp9F4,_tmp9F3,(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4C0=_cycalloc(sizeof(*_tmp4C0));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp9F2=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp4C1;_tmp4C1.tag=15U;_tmp4C1.f1=t;_tmp4C1;});_tmp4C0[0]=_tmp9F2;});_tmp4C0;}),0);});
({union Cyc_YYSTYPE _tmp9F6=Cyc_YY16(({struct Cyc_List_List*_tmp4BD=_cycalloc(sizeof(*_tmp4BD));({struct Cyc_Absyn_Decl*_tmp9F5=Cyc_Absyn_region_decl(tv,vd,_tmp4B9,(unsigned int)(yyyylsp[0]).first_line);_tmp4BD->hd=_tmp9F5;});_tmp4BD->tl=0;_tmp4BD;}));yyval=_tmp9F6;});
# 1404
goto _LL254;};}case 41U: _LL2A1: _LL2A2: {
# 1406
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1408
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1408 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 42U: _LL2A3: _LL2A4: {
# 1411
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1413
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1410 "parse.y"
({union Cyc_YYSTYPE _tmp9F8=Cyc_YY16(({struct Cyc_List_List*_tmp9F7=Cyc_yyget_YY16(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9F7,Cyc_yyget_YY16(yyyyvsp[1]));}));yyval=_tmp9F8;});
goto _LL254;}case 43U: _LL2A5: _LL2A6: {
# 1413
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1415
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmp9FC=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4C9;({enum Cyc_Parse_Storage_class*_tmp9F9=Cyc_yyget_YY20(yyyyvsp[0]);_tmp4C9.sc=_tmp9F9;});({struct Cyc_Absyn_Tqual _tmp9FA=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp4C9.tq=_tmp9FA;});({struct Cyc_Parse_Type_specifier _tmp9FB=
Cyc_Parse_empty_spec(0U);_tmp4C9.type_specs=_tmp9FB;});_tmp4C9.is_inline=0;_tmp4C9.attributes=0;_tmp4C9;}));
# 1416
yyval=_tmp9FC;});
# 1418
goto _LL254;}case 44U: _LL2A7: _LL2A8: {
# 1420
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1422
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1419 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4CA=Cyc_yyget_YY17(yyyyvsp[1]);
if(_tmp4CA.sc != 0)
({void*_tmp4CB=0U;({unsigned int _tmp9FE=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp9FD=({const char*_tmp4CC="Only one storage class is allowed in a declaration (missing ';' or ','?)";_tag_dyneither(_tmp4CC,sizeof(char),73U);});Cyc_Warn_warn(_tmp9FE,_tmp9FD,_tag_dyneither(_tmp4CB,sizeof(void*),0U));});});
# 1423
({union Cyc_YYSTYPE _tmpA00=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4CD;({enum Cyc_Parse_Storage_class*_tmp9FF=Cyc_yyget_YY20(yyyyvsp[0]);_tmp4CD.sc=_tmp9FF;});_tmp4CD.tq=_tmp4CA.tq;_tmp4CD.type_specs=_tmp4CA.type_specs;_tmp4CD.is_inline=_tmp4CA.is_inline;_tmp4CD.attributes=_tmp4CA.attributes;_tmp4CD;}));yyval=_tmpA00;});
# 1427
goto _LL254;}case 45U: _LL2A9: _LL2AA: {
# 1429
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1431
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1428 "parse.y"
({union Cyc_YYSTYPE _tmpA03=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4CE;_tmp4CE.sc=0;({struct Cyc_Absyn_Tqual _tmpA01=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp4CE.tq=_tmpA01;});({struct Cyc_Parse_Type_specifier _tmpA02=
Cyc_yyget_YY21(yyyyvsp[0]);_tmp4CE.type_specs=_tmpA02;});_tmp4CE.is_inline=0;_tmp4CE.attributes=0;_tmp4CE;}));
# 1428
yyval=_tmpA03;});
# 1430
goto _LL254;}case 46U: _LL2AB: _LL2AC: {
# 1432
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1434
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1431 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4CF=Cyc_yyget_YY17(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmpA07=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4D0;_tmp4D0.sc=_tmp4CF.sc;_tmp4D0.tq=_tmp4CF.tq;({struct Cyc_Parse_Type_specifier _tmpA06=({
unsigned int _tmpA05=(unsigned int)(yyyylsp[0]).first_line;struct Cyc_Parse_Type_specifier _tmpA04=_tmp4CF.type_specs;Cyc_Parse_combine_specifiers(_tmpA05,_tmpA04,Cyc_yyget_YY21(yyyyvsp[0]));});_tmp4D0.type_specs=_tmpA06;});_tmp4D0.is_inline=_tmp4CF.is_inline;_tmp4D0.attributes=_tmp4CF.attributes;_tmp4D0;}));
# 1432
yyval=_tmpA07;});
# 1437
goto _LL254;}case 47U: _LL2AD: _LL2AE: {
# 1439
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1441
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1438 "parse.y"
({union Cyc_YYSTYPE _tmpA0A=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4D1;_tmp4D1.sc=0;({struct Cyc_Absyn_Tqual _tmpA08=Cyc_yyget_YY23(yyyyvsp[0]);_tmp4D1.tq=_tmpA08;});({struct Cyc_Parse_Type_specifier _tmpA09=Cyc_Parse_empty_spec(0U);_tmp4D1.type_specs=_tmpA09;});_tmp4D1.is_inline=0;_tmp4D1.attributes=0;_tmp4D1;}));yyval=_tmpA0A;});
goto _LL254;}case 48U: _LL2AF: _LL2B0: {
# 1441
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1443
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1440 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4D2=Cyc_yyget_YY17(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmpA0D=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4D3;_tmp4D3.sc=_tmp4D2.sc;({struct Cyc_Absyn_Tqual _tmpA0C=({struct Cyc_Absyn_Tqual _tmpA0B=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmpA0B,_tmp4D2.tq);});_tmp4D3.tq=_tmpA0C;});_tmp4D3.type_specs=_tmp4D2.type_specs;_tmp4D3.is_inline=_tmp4D2.is_inline;_tmp4D3.attributes=_tmp4D2.attributes;_tmp4D3;}));yyval=_tmpA0D;});
# 1445
goto _LL254;}case 49U: _LL2B1: _LL2B2: {
# 1447
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1449
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1446 "parse.y"
({union Cyc_YYSTYPE _tmpA10=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4D4;_tmp4D4.sc=0;({struct Cyc_Absyn_Tqual _tmpA0E=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp4D4.tq=_tmpA0E;});({struct Cyc_Parse_Type_specifier _tmpA0F=
Cyc_Parse_empty_spec(0U);_tmp4D4.type_specs=_tmpA0F;});_tmp4D4.is_inline=1;_tmp4D4.attributes=0;_tmp4D4;}));
# 1446
yyval=_tmpA10;});
# 1448
goto _LL254;}case 50U: _LL2B3: _LL2B4: {
# 1450
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1452
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1449 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4D5=Cyc_yyget_YY17(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmpA11=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4D6;_tmp4D6.sc=_tmp4D5.sc;_tmp4D6.tq=_tmp4D5.tq;_tmp4D6.type_specs=_tmp4D5.type_specs;_tmp4D6.is_inline=1;_tmp4D6.attributes=_tmp4D5.attributes;_tmp4D6;}));yyval=_tmpA11;});
# 1453
goto _LL254;}case 51U: _LL2B5: _LL2B6: {
# 1455
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1457
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1454 "parse.y"
({union Cyc_YYSTYPE _tmpA15=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4D7;_tmp4D7.sc=0;({struct Cyc_Absyn_Tqual _tmpA12=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp4D7.tq=_tmpA12;});({struct Cyc_Parse_Type_specifier _tmpA13=
Cyc_Parse_empty_spec(0U);_tmp4D7.type_specs=_tmpA13;});_tmp4D7.is_inline=0;({struct Cyc_List_List*_tmpA14=Cyc_yyget_YY45(yyyyvsp[0]);_tmp4D7.attributes=_tmpA14;});_tmp4D7;}));
# 1454
yyval=_tmpA15;});
# 1456
goto _LL254;}case 52U: _LL2B7: _LL2B8: {
# 1458
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1460
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1457 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4D8=Cyc_yyget_YY17(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmpA18=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp4D9;_tmp4D9.sc=_tmp4D8.sc;_tmp4D9.tq=_tmp4D8.tq;_tmp4D9.type_specs=_tmp4D8.type_specs;_tmp4D9.is_inline=_tmp4D8.is_inline;({struct Cyc_List_List*_tmpA17=({
# 1460
struct Cyc_List_List*_tmpA16=Cyc_yyget_YY45(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpA16,_tmp4D8.attributes);});_tmp4D9.attributes=_tmpA17;});_tmp4D9;}));
# 1458
yyval=_tmpA18;});
# 1461
goto _LL254;}case 53U: _LL2B9: _LL2BA: {
# 1463
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1465
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1464 "parse.y"
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
({union Cyc_YYSTYPE _tmpA19=Cyc_YY20(& auto_sc);yyval=_tmpA19;});
goto _LL254;}case 54U: _LL2BB: _LL2BC: {
# 1468
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1470
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1466 "parse.y"
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
({union Cyc_YYSTYPE _tmpA1A=Cyc_YY20(& register_sc);yyval=_tmpA1A;});
goto _LL254;}case 55U: _LL2BD: _LL2BE: {
# 1470
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1472
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1468 "parse.y"
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
({union Cyc_YYSTYPE _tmpA1B=Cyc_YY20(& static_sc);yyval=_tmpA1B;});
goto _LL254;}case 56U: _LL2BF: _LL2C0: {
# 1472
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1474
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1470 "parse.y"
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
({union Cyc_YYSTYPE _tmpA1C=Cyc_YY20(& extern_sc);yyval=_tmpA1C;});
goto _LL254;}case 57U: _LL2C1: _LL2C2: {
# 1474
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1476
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1473 "parse.y"
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
if(({struct _dyneither_ptr _tmpA1D=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]);Cyc_strcmp(_tmpA1D,({const char*_tmp4DA="C";_tag_dyneither(_tmp4DA,sizeof(char),2U);}));})!= 0)
({void*_tmp4DB=0U;({unsigned int _tmpA1F=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpA1E=({const char*_tmp4DC="only extern or extern \"C\" is allowed";_tag_dyneither(_tmp4DC,sizeof(char),37U);});Cyc_Warn_err(_tmpA1F,_tmpA1E,_tag_dyneither(_tmp4DB,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpA20=Cyc_YY20(& externC_sc);yyval=_tmpA20;});
# 1478
goto _LL254;}case 58U: _LL2C3: _LL2C4: {
# 1480
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1478 "parse.y"
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
({union Cyc_YYSTYPE _tmpA21=Cyc_YY20(& typedef_sc);yyval=_tmpA21;});
goto _LL254;}case 59U: _LL2C5: _LL2C6: {
# 1482
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1484
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1481 "parse.y"
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
({union Cyc_YYSTYPE _tmpA22=Cyc_YY20(& abstract_sc);yyval=_tmpA22;});
goto _LL254;}case 60U: _LL2C7: _LL2C8:
# 1485
({union Cyc_YYSTYPE _tmpA23=Cyc_YY45(0);yyval=_tmpA23;});
goto _LL254;case 61U: _LL2C9: _LL2CA: {
# 1488
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1490
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1488 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 62U: _LL2CB: _LL2CC: {
# 1491
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1493
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1493 "parse.y"
yyval=yyyyvsp[3];
goto _LL254;}case 63U: _LL2CD: _LL2CE: {
# 1496
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1498
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1497 "parse.y"
({union Cyc_YYSTYPE _tmpA25=Cyc_YY45(({struct Cyc_List_List*_tmp4DD=_cycalloc(sizeof(*_tmp4DD));({void*_tmpA24=Cyc_yyget_YY46(yyyyvsp[0]);_tmp4DD->hd=_tmpA24;});_tmp4DD->tl=0;_tmp4DD;}));yyval=_tmpA25;});
goto _LL254;}case 64U: _LL2CF: _LL2D0: {
# 1500
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1502
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1498 "parse.y"
({union Cyc_YYSTYPE _tmpA28=Cyc_YY45(({struct Cyc_List_List*_tmp4DE=_cycalloc(sizeof(*_tmp4DE));({void*_tmpA26=Cyc_yyget_YY46(yyyyvsp[0]);_tmp4DE->hd=_tmpA26;});({struct Cyc_List_List*_tmpA27=Cyc_yyget_YY45(yyyyvsp[2]);_tmp4DE->tl=_tmpA27;});_tmp4DE;}));yyval=_tmpA28;});
goto _LL254;}case 65U: _LL2D1: _LL2D2: {
# 1501
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1503
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1503 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6U,0};
static struct _tuple28 att_map[18U]={{{_tmp4E2,_tmp4E2,_tmp4E2 + 8U},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp4E3,_tmp4E3,_tmp4E3 + 6U},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp4E4,_tmp4E4,_tmp4E4 + 9U},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp4E5,_tmp4E5,_tmp4E5 + 9U},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp4E6,_tmp4E6,_tmp4E6 + 6U},(void*)& Cyc_Absyn_Const_att_val},{{_tmp4E7,_tmp4E7,_tmp4E7 + 8U},(void*)& att_aligned},{{_tmp4E8,_tmp4E8,_tmp4E8 + 7U},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp4E9,_tmp4E9,_tmp4E9 + 7U},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp4EA,_tmp4EA,_tmp4EA + 7U},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp4EB,_tmp4EB,_tmp4EB + 5U},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp4EC,_tmp4EC,_tmp4EC + 10U},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp4ED,_tmp4ED,_tmp4ED + 10U},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp4EE,_tmp4EE,_tmp4EE + 23U},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp4EF,_tmp4EF,_tmp4EF + 12U},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp4F0,_tmp4F0,_tmp4F0 + 11U},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp4F1,_tmp4F1,_tmp4F1 + 22U},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp4F2,_tmp4F2,_tmp4F2 + 5U},(void*)& Cyc_Absyn_Pure_att_val},{{_tmp4F3,_tmp4F3,_tmp4F3 + 14U},(void*)& Cyc_Absyn_Always_inline_att_val}};
# 1524
struct _dyneither_ptr _tmp4DF=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1526
if((((_get_dyneither_size(_tmp4DF,sizeof(char))> 4  && ((const char*)_tmp4DF.curr)[0]== '_') && ((const char*)_tmp4DF.curr)[1]== '_') && *((const char*)_check_dyneither_subscript(_tmp4DF,sizeof(char),(int)(_get_dyneither_size(_tmp4DF,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(_tmp4DF,sizeof(char),(int)(_get_dyneither_size(_tmp4DF,sizeof(char))- 3)))== '_')
# 1528
({struct _dyneither_ptr _tmpA29=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp4DF,2,_get_dyneither_size(_tmp4DF,sizeof(char))- 5);_tmp4DF=_tmpA29;});{
int i=0;
for(0;i < 18U;++ i){
if(Cyc_strcmp((struct _dyneither_ptr)_tmp4DF,(struct _dyneither_ptr)(att_map[_check_known_subscript_notnull(18U,i)]).f1)== 0){
({union Cyc_YYSTYPE _tmpA2A=Cyc_YY46((att_map[i]).f2);yyval=_tmpA2A;});
break;}}
# 1535
if(i == 18U){
({void*_tmp4E0=0U;({unsigned int _tmpA2C=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpA2B=({const char*_tmp4E1="unrecognized attribute";_tag_dyneither(_tmp4E1,sizeof(char),23U);});Cyc_Warn_err(_tmpA2C,_tmpA2B,_tag_dyneither(_tmp4E0,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpA2D=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);yyval=_tmpA2D;});}
# 1540
goto _LL254;};}case 66U: _LL2D3: _LL2D4: {
# 1542
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1544
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1540 "parse.y"
({union Cyc_YYSTYPE _tmpA2E=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);yyval=_tmpA2E;});
goto _LL254;}case 67U: _LL2D5: _LL2D6: {
# 1543
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1545
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1542 "parse.y"
struct _dyneither_ptr _tmp4F5=Cyc_yyget_String_tok(yyyyvsp[0]);
struct Cyc_Absyn_Exp*_tmp4F6=Cyc_yyget_Exp_tok(yyyyvsp[2]);
void*a;
if(({struct _dyneither_ptr _tmpA30=(struct _dyneither_ptr)_tmp4F5;Cyc_zstrcmp(_tmpA30,({const char*_tmp4F7="aligned";_tag_dyneither(_tmp4F7,sizeof(char),8U);}));})== 0  || ({struct _dyneither_ptr _tmpA2F=(struct _dyneither_ptr)_tmp4F5;Cyc_zstrcmp(_tmpA2F,({const char*_tmp4F8="__aligned__";_tag_dyneither(_tmp4F8,sizeof(char),12U);}));})== 0)
({void*_tmpA32=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp4F9=_cycalloc(sizeof(*_tmp4F9));({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpA31=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp4FA;_tmp4FA.tag=6U;_tmp4FA.f1=_tmp4F6;_tmp4FA;});_tmp4F9[0]=_tmpA31;});_tmp4F9;});a=_tmpA32;});else{
if(({struct _dyneither_ptr _tmpA34=(struct _dyneither_ptr)_tmp4F5;Cyc_zstrcmp(_tmpA34,({const char*_tmp4FB="section";_tag_dyneither(_tmp4FB,sizeof(char),8U);}));})== 0  || ({struct _dyneither_ptr _tmpA33=(struct _dyneither_ptr)_tmp4F5;Cyc_zstrcmp(_tmpA33,({const char*_tmp4FC="__section__";_tag_dyneither(_tmp4FC,sizeof(char),12U);}));})== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp4F6);
({void*_tmpA36=(void*)({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp4FD=_cycalloc(sizeof(*_tmp4FD));({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct _tmpA35=({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct _tmp4FE;_tmp4FE.tag=8U;_tmp4FE.f1=str;_tmp4FE;});_tmp4FD[0]=_tmpA35;});_tmp4FD;});a=_tmpA36;});}else{
if(({struct _dyneither_ptr _tmpA37=(struct _dyneither_ptr)_tmp4F5;Cyc_zstrcmp(_tmpA37,({const char*_tmp4FF="__mode__";_tag_dyneither(_tmp4FF,sizeof(char),9U);}));})== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp4F6);
({void*_tmpA39=(void*)({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmp500=_cycalloc(sizeof(*_tmp500));({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct _tmpA38=({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct _tmp501;_tmp501.tag=24U;_tmp501.f1=str;_tmp501;});_tmp500[0]=_tmpA38;});_tmp500;});a=_tmpA39;});}else{
if(({struct _dyneither_ptr _tmpA3A=(struct _dyneither_ptr)_tmp4F5;Cyc_zstrcmp(_tmpA3A,({const char*_tmp502="alias";_tag_dyneither(_tmp502,sizeof(char),6U);}));})== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp4F6);
({void*_tmpA3C=(void*)({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*_tmp503=_cycalloc(sizeof(*_tmp503));({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct _tmpA3B=({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct _tmp504;_tmp504.tag=25U;_tmp504.f1=str;_tmp504;});_tmp503[0]=_tmpA3B;});_tmp503;});a=_tmpA3C;});}else{
# 1558
int n=Cyc_Parse_exp2int((unsigned int)(yyyylsp[2]).first_line,_tmp4F6);
if(({struct _dyneither_ptr _tmpA3E=(struct _dyneither_ptr)_tmp4F5;Cyc_zstrcmp(_tmpA3E,({const char*_tmp505="regparm";_tag_dyneither(_tmp505,sizeof(char),8U);}));})== 0  || ({struct _dyneither_ptr _tmpA3D=(struct _dyneither_ptr)_tmp4F5;Cyc_zstrcmp(_tmpA3D,({const char*_tmp506="__regparm__";_tag_dyneither(_tmp506,sizeof(char),12U);}));})== 0){
if(n < 0  || n > 3)
({void*_tmp507=0U;({unsigned int _tmpA40=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmpA3F=({const char*_tmp508="regparm requires value between 0 and 3";_tag_dyneither(_tmp508,sizeof(char),39U);});Cyc_Warn_err(_tmpA40,_tmpA3F,_tag_dyneither(_tmp507,sizeof(void*),0U));});});
({void*_tmpA42=(void*)({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp509=_cycalloc_atomic(sizeof(*_tmp509));({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct _tmpA41=({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct _tmp50A;_tmp50A.tag=0U;_tmp50A.f1=n;_tmp50A;});_tmp509[0]=_tmpA41;});_tmp509;});a=_tmpA42;});}else{
if(({struct _dyneither_ptr _tmpA44=(struct _dyneither_ptr)_tmp4F5;Cyc_zstrcmp(_tmpA44,({const char*_tmp50B="initializes";_tag_dyneither(_tmp50B,sizeof(char),12U);}));})== 0  || ({struct _dyneither_ptr _tmpA43=(struct _dyneither_ptr)_tmp4F5;Cyc_zstrcmp(_tmpA43,({const char*_tmp50C="__initializes__";_tag_dyneither(_tmp50C,sizeof(char),16U);}));})== 0)
({void*_tmpA46=(void*)({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp50D=_cycalloc_atomic(sizeof(*_tmp50D));({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct _tmpA45=({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct _tmp50E;_tmp50E.tag=20U;_tmp50E.f1=n;_tmp50E;});_tmp50D[0]=_tmpA45;});_tmp50D;});a=_tmpA46;});else{
if(({struct _dyneither_ptr _tmpA48=(struct _dyneither_ptr)_tmp4F5;Cyc_zstrcmp(_tmpA48,({const char*_tmp50F="noliveunique";_tag_dyneither(_tmp50F,sizeof(char),13U);}));})== 0  || ({struct _dyneither_ptr _tmpA47=(struct _dyneither_ptr)_tmp4F5;Cyc_zstrcmp(_tmpA47,({const char*_tmp510="__noliveunique__";_tag_dyneither(_tmp510,sizeof(char),17U);}));})== 0)
({void*_tmpA4A=(void*)({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp511=_cycalloc_atomic(sizeof(*_tmp511));({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct _tmpA49=({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct _tmp512;_tmp512.tag=21U;_tmp512.f1=n;_tmp512;});_tmp511[0]=_tmpA49;});_tmp511;});a=_tmpA4A;});else{
if(({struct _dyneither_ptr _tmpA4C=(struct _dyneither_ptr)_tmp4F5;Cyc_zstrcmp(_tmpA4C,({const char*_tmp513="noconsume";_tag_dyneither(_tmp513,sizeof(char),10U);}));})== 0  || ({struct _dyneither_ptr _tmpA4B=(struct _dyneither_ptr)_tmp4F5;Cyc_zstrcmp(_tmpA4B,({const char*_tmp514="__noconsume__";_tag_dyneither(_tmp514,sizeof(char),14U);}));})== 0)
({void*_tmpA4E=(void*)({struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp515=_cycalloc_atomic(sizeof(*_tmp515));({struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct _tmpA4D=({struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct _tmp516;_tmp516.tag=22U;_tmp516.f1=n;_tmp516;});_tmp515[0]=_tmpA4D;});_tmp515;});a=_tmpA4E;});else{
# 1570
({void*_tmp517=0U;({unsigned int _tmpA50=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpA4F=({const char*_tmp518="unrecognized attribute";_tag_dyneither(_tmp518,sizeof(char),23U);});Cyc_Warn_err(_tmpA50,_tmpA4F,_tag_dyneither(_tmp517,sizeof(void*),0U));});});
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}}}
# 1574
({union Cyc_YYSTYPE _tmpA51=Cyc_YY46(a);yyval=_tmpA51;});
# 1576
goto _LL254;}case 68U: _LL2D7: _LL2D8: {
# 1578
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 1580
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 1577 "parse.y"
struct _dyneither_ptr _tmp519=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp51A=Cyc_yyget_String_tok(yyyyvsp[2]);
unsigned int _tmp51B=({unsigned int _tmpA52=(unsigned int)(yyyylsp[4]).first_line;Cyc_Parse_cnst2uint(_tmpA52,Cyc_yyget_Int_tok(yyyyvsp[4]));});
unsigned int _tmp51C=({unsigned int _tmpA53=(unsigned int)(yyyylsp[6]).first_line;Cyc_Parse_cnst2uint(_tmpA53,Cyc_yyget_Int_tok(yyyyvsp[6]));});
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
if(({struct _dyneither_ptr _tmpA55=(struct _dyneither_ptr)_tmp519;Cyc_zstrcmp(_tmpA55,({const char*_tmp51D="format";_tag_dyneither(_tmp51D,sizeof(char),7U);}));})== 0  || ({struct _dyneither_ptr _tmpA54=(struct _dyneither_ptr)_tmp519;Cyc_zstrcmp(_tmpA54,({const char*_tmp51E="__format__";_tag_dyneither(_tmp51E,sizeof(char),11U);}));})== 0){
if(({struct _dyneither_ptr _tmpA57=(struct _dyneither_ptr)_tmp51A;Cyc_zstrcmp(_tmpA57,({const char*_tmp51F="printf";_tag_dyneither(_tmp51F,sizeof(char),7U);}));})== 0  || ({struct _dyneither_ptr _tmpA56=(struct _dyneither_ptr)_tmp51A;Cyc_zstrcmp(_tmpA56,({const char*_tmp520="__printf__";_tag_dyneither(_tmp520,sizeof(char),11U);}));})== 0)
({void*_tmpA59=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp521=_cycalloc_atomic(sizeof(*_tmp521));({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpA58=({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmp522;_tmp522.tag=19U;_tmp522.f1=Cyc_Absyn_Printf_ft;_tmp522.f2=(int)_tmp51B;_tmp522.f3=(int)_tmp51C;_tmp522;});_tmp521[0]=_tmpA58;});_tmp521;});a=_tmpA59;});else{
if(({struct _dyneither_ptr _tmpA5B=(struct _dyneither_ptr)_tmp51A;Cyc_zstrcmp(_tmpA5B,({const char*_tmp523="scanf";_tag_dyneither(_tmp523,sizeof(char),6U);}));})== 0  || ({struct _dyneither_ptr _tmpA5A=(struct _dyneither_ptr)_tmp51A;Cyc_zstrcmp(_tmpA5A,({const char*_tmp524="__scanf__";_tag_dyneither(_tmp524,sizeof(char),10U);}));})== 0)
({void*_tmpA5D=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp525=_cycalloc_atomic(sizeof(*_tmp525));({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpA5C=({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmp526;_tmp526.tag=19U;_tmp526.f1=Cyc_Absyn_Scanf_ft;_tmp526.f2=(int)_tmp51B;_tmp526.f3=(int)_tmp51C;_tmp526;});_tmp525[0]=_tmpA5C;});_tmp525;});a=_tmpA5D;});else{
# 1588
({void*_tmp527=0U;({unsigned int _tmpA5F=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmpA5E=({const char*_tmp528="unrecognized format type";_tag_dyneither(_tmp528,sizeof(char),25U);});Cyc_Warn_err(_tmpA5F,_tmpA5E,_tag_dyneither(_tmp527,sizeof(void*),0U));});});}}}else{
# 1590
({void*_tmp529=0U;({unsigned int _tmpA61=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpA60=({const char*_tmp52A="unrecognized attribute";_tag_dyneither(_tmp52A,sizeof(char),23U);});Cyc_Warn_err(_tmpA61,_tmpA60,_tag_dyneither(_tmp529,sizeof(void*),0U));});});}
({union Cyc_YYSTYPE _tmpA62=Cyc_YY46(a);yyval=_tmpA62;});
# 1593
goto _LL254;}case 69U: _LL2D9: _LL2DA: {
# 1595
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1597
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1602 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 70U: _LL2DB: _LL2DC: {
# 1605
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1607
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1604 "parse.y"
({union Cyc_YYSTYPE _tmpA67=Cyc_YY21(({void*_tmpA66=(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp52B=_cycalloc(sizeof(*_tmp52B));({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA65=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp52C;_tmp52C.tag=17U;({struct _tuple0*_tmpA63=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp52C.f1=_tmpA63;});({struct Cyc_List_List*_tmpA64=Cyc_yyget_YY40(yyyyvsp[1]);_tmp52C.f2=_tmpA64;});_tmp52C.f3=0;_tmp52C.f4=0;_tmp52C;});_tmp52B[0]=_tmpA65;});_tmp52B;});Cyc_Parse_type_spec(_tmpA66,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA67;});
goto _LL254;}case 71U: _LL2DD: _LL2DE: {
# 1607
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1609
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1608 "parse.y"
({union Cyc_YYSTYPE _tmpA68=Cyc_YY21(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpA68;});
goto _LL254;}case 72U: _LL2DF: _LL2E0: {
# 1611
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1613
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1609 "parse.y"
({union Cyc_YYSTYPE _tmpA69=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpA69;});
goto _LL254;}case 73U: _LL2E1: _LL2E2: {
# 1612
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1610 "parse.y"
({union Cyc_YYSTYPE _tmpA6A=Cyc_YY21(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpA6A;});
goto _LL254;}case 74U: _LL2E3: _LL2E4: {
# 1613
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1615
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1611 "parse.y"
({union Cyc_YYSTYPE _tmpA6B=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpA6B;});
goto _LL254;}case 75U: _LL2E5: _LL2E6: {
# 1614
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1612 "parse.y"
({union Cyc_YYSTYPE _tmpA6C=Cyc_YY21(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpA6C;});
goto _LL254;}case 76U: _LL2E7: _LL2E8: {
# 1615
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1617
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1613 "parse.y"
({union Cyc_YYSTYPE _tmpA6E=Cyc_YY21(({void*_tmpA6D=Cyc_Absyn_float_typ(0);Cyc_Parse_type_spec(_tmpA6D,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA6E;});
goto _LL254;}case 77U: _LL2E9: _LL2EA: {
# 1616
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1618
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1614 "parse.y"
({union Cyc_YYSTYPE _tmpA70=Cyc_YY21(({void*_tmpA6F=Cyc_Absyn_float_typ(1);Cyc_Parse_type_spec(_tmpA6F,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA70;});
goto _LL254;}case 78U: _LL2EB: _LL2EC: {
# 1617
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1615 "parse.y"
({union Cyc_YYSTYPE _tmpA71=Cyc_YY21(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpA71;});
goto _LL254;}case 79U: _LL2ED: _LL2EE: {
# 1618
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1616 "parse.y"
({union Cyc_YYSTYPE _tmpA72=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpA72;});
goto _LL254;}case 80U: _LL2EF: _LL2F0: {
# 1619
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1617 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 81U: _LL2F1: _LL2F2: {
# 1620
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1622
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1618 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 82U: _LL2F3: _LL2F4: {
# 1621
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1621 "parse.y"
({union Cyc_YYSTYPE _tmpA76=Cyc_YY21(({void*_tmpA75=(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp52D=_cycalloc(sizeof(*_tmp52D));({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmpA74=({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp52E;_tmp52E.tag=27U;({struct Cyc_Absyn_Exp*_tmpA73=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp52E.f1=_tmpA73;});_tmp52E;});_tmp52D[0]=_tmpA74;});_tmp52D;});Cyc_Parse_type_spec(_tmpA75,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA76;});
goto _LL254;}case 83U: _LL2F5: _LL2F6: {
# 1624
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1626
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1623 "parse.y"
({union Cyc_YYSTYPE _tmpA7A=Cyc_YY21(({void*_tmpA79=(void*)({struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*_tmp52F=_cycalloc(sizeof(*_tmp52F));({struct Cyc_Absyn_BuiltinType_Absyn_Type_struct _tmpA78=({struct Cyc_Absyn_BuiltinType_Absyn_Type_struct _tmp530;_tmp530.tag=28U;({struct _dyneither_ptr _tmpA77=({const char*_tmp531="__builtin_va_list";_tag_dyneither(_tmp531,sizeof(char),18U);});_tmp530.f1=_tmpA77;});_tmp530.f2=& Cyc_Tcutil_bk;_tmp530;});_tmp52F[0]=_tmpA78;});_tmp52F;});Cyc_Parse_type_spec(_tmpA79,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA7A;});
goto _LL254;}case 84U: _LL2F7: _LL2F8: {
# 1626
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1628
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1625 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 85U: _LL2F9: _LL2FA: {
# 1628
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1630
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1627 "parse.y"
({union Cyc_YYSTYPE _tmpA7C=Cyc_YY21(({void*_tmpA7B=Cyc_yyget_YY44(yyyyvsp[0]);Cyc_Parse_type_spec(_tmpA7B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA7C;});
goto _LL254;}case 86U: _LL2FB: _LL2FC: {
# 1630
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1632
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1629 "parse.y"
({union Cyc_YYSTYPE _tmpA7E=Cyc_YY21(({void*_tmpA7D=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_tmpA7D,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA7E;});
goto _LL254;}case 87U: _LL2FD: _LL2FE: {
# 1632
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1634
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1631 "parse.y"
({union Cyc_YYSTYPE _tmpA80=Cyc_YY21(({void*_tmpA7F=Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(yyyyvsp[2])),0);Cyc_Parse_type_spec(_tmpA7F,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA80;});
goto _LL254;}case 88U: _LL2FF: _LL300: {
# 1634
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1636
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1633 "parse.y"
({union Cyc_YYSTYPE _tmpA85=Cyc_YY21(({void*_tmpA84=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp532=_cycalloc(sizeof(*_tmp532));({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpA83=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp533;_tmp533.tag=10U;({struct Cyc_List_List*_tmpA82=({unsigned int _tmpA81=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmpA81,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[2])));});_tmp533.f1=_tmpA82;});_tmp533;});_tmp532[0]=_tmpA83;});_tmp532;});
# 1633
Cyc_Parse_type_spec(_tmpA84,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA85;});
# 1636
goto _LL254;}case 89U: _LL301: _LL302: {
# 1638
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1640
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1637 "parse.y"
({union Cyc_YYSTYPE _tmpA89=Cyc_YY21(({void*_tmpA88=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp534=_cycalloc(sizeof(*_tmp534));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA87=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp535;_tmp535.tag=15U;({void*_tmpA86=Cyc_yyget_YY44(yyyyvsp[2]);_tmp535.f1=_tmpA86;});_tmp535;});_tmp534[0]=_tmpA87;});_tmp534;});Cyc_Parse_type_spec(_tmpA88,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA89;});
goto _LL254;}case 90U: _LL303: _LL304: {
# 1640
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1642
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1639 "parse.y"
({union Cyc_YYSTYPE _tmpA8D=Cyc_YY21(({void*_tmpA8C=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp536=_cycalloc(sizeof(*_tmp536));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA8B=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp537;_tmp537.tag=15U;({void*_tmpA8A=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);_tmp537.f1=_tmpA8A;});_tmp537;});_tmp536[0]=_tmpA8B;});_tmp536;});Cyc_Parse_type_spec(_tmpA8C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA8D;});
# 1641
goto _LL254;}case 91U: _LL305: _LL306: {
# 1643
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1642 "parse.y"
void*_tmp538=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);
({union Cyc_YYSTYPE _tmpA91=Cyc_YY21(({void*_tmpA90=(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp539=_cycalloc(sizeof(*_tmp539));({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpA8F=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp53A;_tmp53A.tag=16U;({void*_tmpA8E=Cyc_yyget_YY44(yyyyvsp[2]);_tmp53A.f1=_tmpA8E;});_tmp53A.f2=_tmp538;_tmp53A;});_tmp539[0]=_tmpA8F;});_tmp539;});Cyc_Parse_type_spec(_tmpA90,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA91;});
# 1645
goto _LL254;}case 92U: _LL307: _LL308: {
# 1647
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1649
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1646 "parse.y"
({union Cyc_YYSTYPE _tmpA96=Cyc_YY21(({void*_tmpA95=(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp53B=_cycalloc(sizeof(*_tmp53B));({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpA94=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp53C;_tmp53C.tag=16U;({void*_tmpA92=Cyc_yyget_YY44(yyyyvsp[2]);_tmp53C.f1=_tmpA92;});({void*_tmpA93=Cyc_yyget_YY44(yyyyvsp[4]);_tmp53C.f2=_tmpA93;});_tmp53C;});_tmp53B[0]=_tmpA94;});_tmp53B;});Cyc_Parse_type_spec(_tmpA95,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA96;});
goto _LL254;}case 93U: _LL309: _LL30A: {
# 1649
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1651
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1648 "parse.y"
({union Cyc_YYSTYPE _tmpA9A=Cyc_YY21(({void*_tmpA99=(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp53D=_cycalloc(sizeof(*_tmp53D));({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpA98=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp53E;_tmp53E.tag=19U;({void*_tmpA97=Cyc_yyget_YY44(yyyyvsp[2]);_tmp53E.f1=_tmpA97;});_tmp53E;});_tmp53D[0]=_tmpA98;});_tmp53D;});Cyc_Parse_type_spec(_tmpA99,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA9A;});
goto _LL254;}case 94U: _LL30B: _LL30C: {
# 1651
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1653
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1650 "parse.y"
({union Cyc_YYSTYPE _tmpA9E=Cyc_YY21(({void*_tmpA9D=(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp53F=_cycalloc(sizeof(*_tmp53F));({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpA9C=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp540;_tmp540.tag=19U;({void*_tmpA9B=Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0);_tmp540.f1=_tmpA9B;});_tmp540;});_tmp53F[0]=_tmpA9C;});_tmp53F;});Cyc_Parse_type_spec(_tmpA9D,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA9E;});
goto _LL254;}case 95U: _LL30D: _LL30E: {
# 1653
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1652 "parse.y"
({union Cyc_YYSTYPE _tmpAA2=Cyc_YY21(({void*_tmpAA1=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp541=_cycalloc(sizeof(*_tmp541));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpAA0=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp542;_tmp542.tag=18U;({struct Cyc_Absyn_Exp*_tmpA9F=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp542.f1=_tmpA9F;});_tmp542;});_tmp541[0]=_tmpAA0;});_tmp541;});Cyc_Parse_type_spec(_tmpAA1,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpAA2;});
goto _LL254;}case 96U: _LL30F: _LL310: {
# 1655
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1657
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpAA4=Cyc_YY43(({struct _dyneither_ptr _tmpAA3=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Parse_id_to_kind(_tmpAA3,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpAA4;});
goto _LL254;}case 97U: _LL311: _LL312: {
# 1661
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1663
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1662 "parse.y"
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0U);
({union Cyc_YYSTYPE _tmpAA5=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp543;_tmp543.print_const=1;_tmp543.q_volatile=0;_tmp543.q_restrict=0;_tmp543.real_const=1;_tmp543.loc=loc;_tmp543;}));yyval=_tmpAA5;});
goto _LL254;}case 98U: _LL313: _LL314: {
# 1666
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1668
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1664 "parse.y"
({union Cyc_YYSTYPE _tmpAA6=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp544;_tmp544.print_const=0;_tmp544.q_volatile=1;_tmp544.q_restrict=0;_tmp544.real_const=0;_tmp544.loc=0U;_tmp544;}));yyval=_tmpAA6;});
goto _LL254;}case 99U: _LL315: _LL316: {
# 1667
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1669
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1665 "parse.y"
({union Cyc_YYSTYPE _tmpAA7=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp545;_tmp545.print_const=0;_tmp545.q_volatile=0;_tmp545.q_restrict=1;_tmp545.real_const=0;_tmp545.loc=0U;_tmp545;}));yyval=_tmpAA7;});
goto _LL254;}case 100U: _LL317: _LL318: {
# 1668
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
struct Cyc_Absyn_TypeDecl*_tmp546=({struct Cyc_Absyn_TypeDecl*_tmp549=_cycalloc(sizeof(*_tmp549));({void*_tmpAAD=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp54A=_cycalloc(sizeof(*_tmp54A));({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct _tmpAAC=({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct _tmp54B;_tmp54B.tag=1U;({struct Cyc_Absyn_Enumdecl*_tmpAAB=({struct Cyc_Absyn_Enumdecl*_tmp54C=_cycalloc(sizeof(*_tmp54C));_tmp54C->sc=Cyc_Absyn_Public;({struct _tuple0*_tmpAA8=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp54C->name=_tmpAA8;});({struct Cyc_Core_Opt*_tmpAAA=({struct Cyc_Core_Opt*_tmp54D=_cycalloc(sizeof(*_tmp54D));({struct Cyc_List_List*_tmpAA9=Cyc_yyget_YY48(yyyyvsp[3]);_tmp54D->v=_tmpAA9;});_tmp54D;});_tmp54C->fields=_tmpAAA;});_tmp54C;});_tmp54B.f1=_tmpAAB;});_tmp54B;});_tmp54A[0]=_tmpAAC;});_tmp54A;});_tmp549->r=_tmpAAD;});_tmp549->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp549;});
# 1673
({union Cyc_YYSTYPE _tmpAB0=Cyc_YY21(({void*_tmpAAF=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp547=_cycalloc(sizeof(*_tmp547));({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpAAE=({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmp548;_tmp548.tag=26U;_tmp548.f1=_tmp546;_tmp548.f2=0;_tmp548;});_tmp547[0]=_tmpAAE;});_tmp547;});Cyc_Parse_type_spec(_tmpAAF,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpAB0;});
# 1675
goto _LL254;}case 101U: _LL319: _LL31A: {
# 1677
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1679
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1676 "parse.y"
({union Cyc_YYSTYPE _tmpAB4=Cyc_YY21(({void*_tmpAB3=(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp54E=_cycalloc(sizeof(*_tmp54E));({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpAB2=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp54F;_tmp54F.tag=13U;({struct _tuple0*_tmpAB1=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp54F.f1=_tmpAB1;});_tmp54F.f2=0;_tmp54F;});_tmp54E[0]=_tmpAB2;});_tmp54E;});Cyc_Parse_type_spec(_tmpAB3,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpAB4;});
goto _LL254;}case 102U: _LL31B: _LL31C: {
# 1679
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1681
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1678 "parse.y"
({union Cyc_YYSTYPE _tmpAB8=Cyc_YY21(({void*_tmpAB7=(void*)({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp550=_cycalloc(sizeof(*_tmp550));({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpAB6=({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmp551;_tmp551.tag=14U;({struct Cyc_List_List*_tmpAB5=Cyc_yyget_YY48(yyyyvsp[2]);_tmp551.f1=_tmpAB5;});_tmp551;});_tmp550[0]=_tmpAB6;});_tmp550;});Cyc_Parse_type_spec(_tmpAB7,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpAB8;});
goto _LL254;}case 103U: _LL31D: _LL31E: {
# 1681
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1683
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpABA=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp552=_cycalloc(sizeof(*_tmp552));({struct _tuple0*_tmpAB9=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp552->name=_tmpAB9;});_tmp552->tag=0;_tmp552->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp552;}));yyval=_tmpABA;});
goto _LL254;}case 104U: _LL31F: _LL320: {
# 1687
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1689
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1686 "parse.y"
({union Cyc_YYSTYPE _tmpABD=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp553=_cycalloc(sizeof(*_tmp553));({struct _tuple0*_tmpABB=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp553->name=_tmpABB;});({struct Cyc_Absyn_Exp*_tmpABC=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp553->tag=_tmpABC;});_tmp553->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp553;}));yyval=_tmpABD;});
goto _LL254;}case 105U: _LL321: _LL322: {
# 1689
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1691
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1690 "parse.y"
({union Cyc_YYSTYPE _tmpABF=Cyc_YY48(({struct Cyc_List_List*_tmp554=_cycalloc(sizeof(*_tmp554));({struct Cyc_Absyn_Enumfield*_tmpABE=Cyc_yyget_YY47(yyyyvsp[0]);_tmp554->hd=_tmpABE;});_tmp554->tl=0;_tmp554;}));yyval=_tmpABF;});
goto _LL254;}case 106U: _LL323: _LL324: {
# 1693
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1695
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1691 "parse.y"
({union Cyc_YYSTYPE _tmpAC1=Cyc_YY48(({struct Cyc_List_List*_tmp555=_cycalloc(sizeof(*_tmp555));({struct Cyc_Absyn_Enumfield*_tmpAC0=Cyc_yyget_YY47(yyyyvsp[0]);_tmp555->hd=_tmpAC0;});_tmp555->tl=0;_tmp555;}));yyval=_tmpAC1;});
goto _LL254;}case 107U: _LL325: _LL326: {
# 1694
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1696
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1692 "parse.y"
({union Cyc_YYSTYPE _tmpAC4=Cyc_YY48(({struct Cyc_List_List*_tmp556=_cycalloc(sizeof(*_tmp556));({struct Cyc_Absyn_Enumfield*_tmpAC2=Cyc_yyget_YY47(yyyyvsp[0]);_tmp556->hd=_tmpAC2;});({struct Cyc_List_List*_tmpAC3=Cyc_yyget_YY48(yyyyvsp[2]);_tmp556->tl=_tmpAC3;});_tmp556;}));yyval=_tmpAC4;});
goto _LL254;}case 108U: _LL327: _LL328: {
# 1695
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1697
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
({union Cyc_YYSTYPE _tmpAC9=Cyc_YY21(({void*_tmpAC8=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp557=_cycalloc(sizeof(*_tmp557));({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpAC7=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp558;_tmp558.tag=12U;({enum Cyc_Absyn_AggrKind _tmpAC5=Cyc_yyget_YY22(yyyyvsp[0]);_tmp558.f1=_tmpAC5;});({struct Cyc_List_List*_tmpAC6=Cyc_yyget_YY24(yyyyvsp[2]);_tmp558.f2=_tmpAC6;});_tmp558;});_tmp557[0]=_tmpAC7;});_tmp557;});Cyc_Parse_type_spec(_tmpAC8,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpAC9;});
goto _LL254;}case 109U: _LL329: _LL32A: {
# 1701
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9U));
# 1703
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9U));
# 1704 "parse.y"
struct Cyc_List_List*_tmp559=({unsigned int _tmpACA=(unsigned int)(yyyylsp[3]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpACA,Cyc_yyget_YY40(yyyyvsp[3]));});
struct Cyc_List_List*_tmp55A=({unsigned int _tmpACB=(unsigned int)(yyyylsp[5]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpACB,Cyc_yyget_YY40(yyyyvsp[5]));});
struct Cyc_Absyn_TypeDecl*_tmp55B=({enum Cyc_Absyn_AggrKind _tmpAD1=Cyc_yyget_YY22(yyyyvsp[1]);struct _tuple0*_tmpAD0=Cyc_yyget_QualId_tok(yyyyvsp[2]);struct Cyc_List_List*_tmpACF=_tmp559;struct Cyc_Absyn_AggrdeclImpl*_tmpACE=({
struct Cyc_List_List*_tmpACD=_tmp55A;struct Cyc_List_List*_tmpACC=Cyc_yyget_YY50(yyyyvsp[6]);Cyc_Absyn_aggrdecl_impl(_tmpACD,_tmpACC,Cyc_yyget_YY24(yyyyvsp[7]),1);});
# 1706
Cyc_Absyn_aggr_tdecl(_tmpAD1,Cyc_Absyn_Public,_tmpAD0,_tmpACF,_tmpACE,0,(unsigned int)(yyyylsp[0]).first_line);});
# 1709
({union Cyc_YYSTYPE _tmpAD4=Cyc_YY21(({void*_tmpAD3=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp55C=_cycalloc(sizeof(*_tmp55C));({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpAD2=({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmp55D;_tmp55D.tag=26U;_tmp55D.f1=_tmp55B;_tmp55D.f2=0;_tmp55D;});_tmp55C[0]=_tmpAD2;});_tmp55C;});Cyc_Parse_type_spec(_tmpAD3,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpAD4;});
# 1711
goto _LL254;}case 110U: _LL32B: _LL32C: {
# 1713
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 1715
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 1715 "parse.y"
struct Cyc_List_List*_tmp55E=({unsigned int _tmpAD5=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpAD5,Cyc_yyget_YY40(yyyyvsp[2]));});
struct Cyc_List_List*_tmp55F=({unsigned int _tmpAD6=(unsigned int)(yyyylsp[4]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpAD6,Cyc_yyget_YY40(yyyyvsp[4]));});
struct Cyc_Absyn_TypeDecl*_tmp560=({enum Cyc_Absyn_AggrKind _tmpADC=Cyc_yyget_YY22(yyyyvsp[0]);struct _tuple0*_tmpADB=Cyc_yyget_QualId_tok(yyyyvsp[1]);struct Cyc_List_List*_tmpADA=_tmp55E;struct Cyc_Absyn_AggrdeclImpl*_tmpAD9=({
struct Cyc_List_List*_tmpAD8=_tmp55F;struct Cyc_List_List*_tmpAD7=Cyc_yyget_YY50(yyyyvsp[5]);Cyc_Absyn_aggrdecl_impl(_tmpAD8,_tmpAD7,Cyc_yyget_YY24(yyyyvsp[6]),0);});
# 1717
Cyc_Absyn_aggr_tdecl(_tmpADC,Cyc_Absyn_Public,_tmpADB,_tmpADA,_tmpAD9,0,(unsigned int)(yyyylsp[0]).first_line);});
# 1720
({union Cyc_YYSTYPE _tmpADF=Cyc_YY21(({void*_tmpADE=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp561=_cycalloc(sizeof(*_tmp561));({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpADD=({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmp562;_tmp562.tag=26U;_tmp562.f1=_tmp560;_tmp562.f2=0;_tmp562;});_tmp561[0]=_tmpADD;});_tmp561;});Cyc_Parse_type_spec(_tmpADE,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpADF;});
# 1722
goto _LL254;}case 111U: _LL32D: _LL32E: {
# 1724
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1726
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1723 "parse.y"
({union Cyc_YYSTYPE _tmpAE7=Cyc_YY21(({void*_tmpAE6=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp563=_cycalloc(sizeof(*_tmp563));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpAE5=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp564;_tmp564.tag=11U;({struct Cyc_Absyn_AggrInfo _tmpAE4=({struct Cyc_Absyn_AggrInfo _tmp565;({union Cyc_Absyn_AggrInfoU _tmpAE2=({enum Cyc_Absyn_AggrKind _tmpAE1=Cyc_yyget_YY22(yyyyvsp[1]);struct _tuple0*_tmpAE0=Cyc_yyget_QualId_tok(yyyyvsp[2]);Cyc_Absyn_UnknownAggr(_tmpAE1,_tmpAE0,({struct Cyc_Core_Opt*_tmp566=_cycalloc_atomic(sizeof(*_tmp566));_tmp566->v=(void*)1;_tmp566;}));});_tmp565.aggr_info=_tmpAE2;});({struct Cyc_List_List*_tmpAE3=Cyc_yyget_YY40(yyyyvsp[3]);_tmp565.targs=_tmpAE3;});_tmp565;});_tmp564.f1=_tmpAE4;});_tmp564;});_tmp563[0]=_tmpAE5;});_tmp563;});Cyc_Parse_type_spec(_tmpAE6,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpAE7;});
# 1726
goto _LL254;}case 112U: _LL32F: _LL330: {
# 1728
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1730
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1727 "parse.y"
({union Cyc_YYSTYPE _tmpAEE=Cyc_YY21(({void*_tmpAED=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp567=_cycalloc(sizeof(*_tmp567));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpAEC=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp568;_tmp568.tag=11U;({struct Cyc_Absyn_AggrInfo _tmpAEB=({struct Cyc_Absyn_AggrInfo _tmp569;({union Cyc_Absyn_AggrInfoU _tmpAE9=({enum Cyc_Absyn_AggrKind _tmpAE8=Cyc_yyget_YY22(yyyyvsp[0]);Cyc_Absyn_UnknownAggr(_tmpAE8,Cyc_yyget_QualId_tok(yyyyvsp[1]),0);});_tmp569.aggr_info=_tmpAE9;});({struct Cyc_List_List*_tmpAEA=Cyc_yyget_YY40(yyyyvsp[2]);_tmp569.targs=_tmpAEA;});_tmp569;});_tmp568.f1=_tmpAEB;});_tmp568;});_tmp567[0]=_tmpAEC;});_tmp567;});Cyc_Parse_type_spec(_tmpAED,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpAEE;});
# 1730
goto _LL254;}case 113U: _LL331: _LL332:
# 1732
({union Cyc_YYSTYPE _tmpAEF=Cyc_YY40(0);yyval=_tmpAEF;});
goto _LL254;case 114U: _LL333: _LL334: {
# 1735
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1737
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1736 "parse.y"
({union Cyc_YYSTYPE _tmpAF0=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[1])));yyval=_tmpAF0;});
goto _LL254;}case 115U: _LL335: _LL336: {
# 1739
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1741
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1740 "parse.y"
({union Cyc_YYSTYPE _tmpAF1=Cyc_YY22(Cyc_Absyn_StructA);yyval=_tmpAF1;});
goto _LL254;}case 116U: _LL337: _LL338: {
# 1743
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1745
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1741 "parse.y"
({union Cyc_YYSTYPE _tmpAF2=Cyc_YY22(Cyc_Absyn_UnionA);yyval=_tmpAF2;});
goto _LL254;}case 117U: _LL339: _LL33A:
# 1744
({union Cyc_YYSTYPE _tmpAF3=Cyc_YY24(0);yyval=_tmpAF3;});
goto _LL254;case 118U: _LL33B: _LL33C: {
# 1747
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1749
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1748 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp56A=Cyc_yyget_YY25(yyyyvsp[0]);for(0;_tmp56A != 0;_tmp56A=_tmp56A->tl){
({struct Cyc_List_List*_tmpAF4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp56A->hd,decls);decls=_tmpAF4;});}}{
# 1752
struct Cyc_List_List*_tmp56B=Cyc_Parse_get_aggrfield_tags(decls);
if(_tmp56B != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp56B,decls);
({union Cyc_YYSTYPE _tmpAF5=Cyc_YY24(decls);yyval=_tmpAF5;});
# 1757
goto _LL254;};}case 119U: _LL33D: _LL33E: {
# 1759
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1761
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpAF7=Cyc_YY25(({struct Cyc_List_List*_tmp56C=_cycalloc(sizeof(*_tmp56C));({struct Cyc_List_List*_tmpAF6=Cyc_yyget_YY24(yyyyvsp[0]);_tmp56C->hd=_tmpAF6;});_tmp56C->tl=0;_tmp56C;}));yyval=_tmpAF7;});
goto _LL254;}case 120U: _LL33F: _LL340: {
# 1765
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1767
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1764 "parse.y"
({union Cyc_YYSTYPE _tmpAFA=Cyc_YY25(({struct Cyc_List_List*_tmp56D=_cycalloc(sizeof(*_tmp56D));({struct Cyc_List_List*_tmpAF8=Cyc_yyget_YY24(yyyyvsp[1]);_tmp56D->hd=_tmpAF8;});({struct Cyc_List_List*_tmpAF9=Cyc_yyget_YY25(yyyyvsp[0]);_tmp56D->tl=_tmpAF9;});_tmp56D;}));yyval=_tmpAFA;});
goto _LL254;}case 121U: _LL341: _LL342: {
# 1767
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1769
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1768 "parse.y"
({union Cyc_YYSTYPE _tmpAFB=Cyc_YY19(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY19(yyyyvsp[0])));yyval=_tmpAFB;});
goto _LL254;}case 122U: _LL343: _LL344: {
# 1771
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1773
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpAFD=Cyc_YY19(({struct _tuple12*_tmp56E=_region_malloc(yyr,sizeof(*_tmp56E));_tmp56E->tl=0;({struct _tuple11 _tmpAFC=Cyc_yyget_YY18(yyyyvsp[0]);_tmp56E->hd=_tmpAFC;});_tmp56E;}));yyval=_tmpAFD;});
goto _LL254;}case 123U: _LL345: _LL346: {
# 1777
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1779
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1776 "parse.y"
({union Cyc_YYSTYPE _tmpB00=Cyc_YY19(({struct _tuple12*_tmp56F=_region_malloc(yyr,sizeof(*_tmp56F));({struct _tuple12*_tmpAFE=Cyc_yyget_YY19(yyyyvsp[0]);_tmp56F->tl=_tmpAFE;});({struct _tuple11 _tmpAFF=Cyc_yyget_YY18(yyyyvsp[2]);_tmp56F->hd=_tmpAFF;});_tmp56F;}));yyval=_tmpB00;});
goto _LL254;}case 124U: _LL347: _LL348: {
# 1779
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1781
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1781 "parse.y"
({union Cyc_YYSTYPE _tmpB02=Cyc_YY18(({struct _tuple11 _tmp570;({struct Cyc_Parse_Declarator _tmpB01=Cyc_yyget_YY27(yyyyvsp[0]);_tmp570.f1=_tmpB01;});_tmp570.f2=0;_tmp570;}));yyval=_tmpB02;});
goto _LL254;}case 125U: _LL349: _LL34A: {
# 1784
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1786
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1783 "parse.y"
({union Cyc_YYSTYPE _tmpB05=Cyc_YY18(({struct _tuple11 _tmp571;({struct Cyc_Parse_Declarator _tmpB03=Cyc_yyget_YY27(yyyyvsp[0]);_tmp571.f1=_tmpB03;});({struct Cyc_Absyn_Exp*_tmpB04=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp571.f2=_tmpB04;});_tmp571;}));yyval=_tmpB05;});
goto _LL254;}case 126U: _LL34B: _LL34C: {
# 1786
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1788
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1789 "parse.y"
struct _RegionHandle _tmp572=_new_region("temp");struct _RegionHandle*temp=& _tmp572;_push_region(temp);
{struct _tuple25 _tmp573=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp574=_tmp573;struct Cyc_Absyn_Tqual _tmp582;struct Cyc_Parse_Type_specifier _tmp581;struct Cyc_List_List*_tmp580;_LL68B: _tmp582=_tmp574.f1;_tmp581=_tmp574.f2;_tmp580=_tmp574.f3;_LL68C:;
if(_tmp582.loc == 0)_tmp582.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp575=Cyc_yyget_YY29(yyyyvsp[1]);for(0;_tmp575 != 0;_tmp575=_tmp575->tl){
struct _tuple24*_tmp576=(struct _tuple24*)_tmp575->hd;struct _tuple24*_tmp577=_tmp576;struct Cyc_Parse_Declarator _tmp57D;struct Cyc_Absyn_Exp*_tmp57C;struct Cyc_Absyn_Exp*_tmp57B;_LL68E: _tmp57D=_tmp577->f1;_tmp57C=_tmp577->f2;_tmp57B=_tmp577->f3;_LL68F:;
({struct _tuple10*_tmpB06=({struct _tuple10*_tmp578=_region_malloc(temp,sizeof(*_tmp578));_tmp578->tl=decls;_tmp578->hd=_tmp57D;_tmp578;});decls=_tmpB06;});
({struct Cyc_List_List*_tmpB08=({struct Cyc_List_List*_tmp579=_region_malloc(temp,sizeof(*_tmp579));({struct _tuple16*_tmpB07=({struct _tuple16*_tmp57A=_region_malloc(temp,sizeof(*_tmp57A));_tmp57A->f1=_tmp57C;_tmp57A->f2=_tmp57B;_tmp57A;});_tmp579->hd=_tmpB07;});_tmp579->tl=widths_and_reqs;_tmp579;});widths_and_reqs=_tmpB08;});}}
# 1800
({struct _tuple10*_tmpB09=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(decls);decls=_tmpB09;});
({struct Cyc_List_List*_tmpB0A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);widths_and_reqs=_tmpB0A;});{
void*_tmp57E=Cyc_Parse_speclist2typ(_tmp581,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp57F=({struct _RegionHandle*_tmpB0D=temp;struct _RegionHandle*_tmpB0C=temp;struct Cyc_List_List*_tmpB0B=
Cyc_Parse_apply_tmss(temp,_tmp582,_tmp57E,decls,_tmp580);
# 1803
((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmpB0D,_tmpB0C,_tmpB0B,widths_and_reqs);});
# 1806
({union Cyc_YYSTYPE _tmpB0E=Cyc_YY24(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple17*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp57F));yyval=_tmpB0E;});
# 1808
_npop_handler(0U);goto _LL254;};};}
# 1790
;_pop_region(temp);}case 127U: _LL34D: _LL34E: {
# 1810
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1812
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1816 "parse.y"
({union Cyc_YYSTYPE _tmpB11=Cyc_YY35(({struct _tuple25 _tmp583;({struct Cyc_Absyn_Tqual _tmpB0F=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp583.f1=_tmpB0F;});({struct Cyc_Parse_Type_specifier _tmpB10=Cyc_yyget_YY21(yyyyvsp[0]);_tmp583.f2=_tmpB10;});_tmp583.f3=0;_tmp583;}));yyval=_tmpB11;});
goto _LL254;}case 128U: _LL34F: _LL350: {
# 1819
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1821
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1818 "parse.y"
struct _tuple25 _tmp584=Cyc_yyget_YY35(yyyyvsp[1]);({union Cyc_YYSTYPE _tmpB15=Cyc_YY35(({struct _tuple25 _tmp585;_tmp585.f1=_tmp584.f1;({struct Cyc_Parse_Type_specifier _tmpB14=({unsigned int _tmpB13=(unsigned int)(yyyylsp[0]).first_line;struct Cyc_Parse_Type_specifier _tmpB12=Cyc_yyget_YY21(yyyyvsp[0]);Cyc_Parse_combine_specifiers(_tmpB13,_tmpB12,_tmp584.f2);});_tmp585.f2=_tmpB14;});_tmp585.f3=_tmp584.f3;_tmp585;}));yyval=_tmpB15;});
goto _LL254;}case 129U: _LL351: _LL352: {
# 1821
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1823
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1820 "parse.y"
({union Cyc_YYSTYPE _tmpB18=Cyc_YY35(({struct _tuple25 _tmp586;({struct Cyc_Absyn_Tqual _tmpB16=Cyc_yyget_YY23(yyyyvsp[0]);_tmp586.f1=_tmpB16;});({struct Cyc_Parse_Type_specifier _tmpB17=Cyc_Parse_empty_spec(0U);_tmp586.f2=_tmpB17;});_tmp586.f3=0;_tmp586;}));yyval=_tmpB18;});
goto _LL254;}case 130U: _LL353: _LL354: {
# 1823
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1825
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1822 "parse.y"
struct _tuple25 _tmp587=Cyc_yyget_YY35(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmpB1B=Cyc_YY35(({struct _tuple25 _tmp588;({struct Cyc_Absyn_Tqual _tmpB1A=({struct Cyc_Absyn_Tqual _tmpB19=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmpB19,_tmp587.f1);});_tmp588.f1=_tmpB1A;});_tmp588.f2=_tmp587.f2;_tmp588.f3=_tmp587.f3;_tmp588;}));yyval=_tmpB1B;});
goto _LL254;}case 131U: _LL355: _LL356: {
# 1826
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1828
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1825 "parse.y"
({union Cyc_YYSTYPE _tmpB1F=Cyc_YY35(({struct _tuple25 _tmp589;({struct Cyc_Absyn_Tqual _tmpB1C=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp589.f1=_tmpB1C;});({struct Cyc_Parse_Type_specifier _tmpB1D=Cyc_Parse_empty_spec(0U);_tmp589.f2=_tmpB1D;});({struct Cyc_List_List*_tmpB1E=Cyc_yyget_YY45(yyyyvsp[0]);_tmp589.f3=_tmpB1E;});_tmp589;}));yyval=_tmpB1F;});
goto _LL254;}case 132U: _LL357: _LL358: {
# 1828
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1830
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1827 "parse.y"
struct _tuple25 _tmp58A=Cyc_yyget_YY35(yyyyvsp[1]);({union Cyc_YYSTYPE _tmpB22=Cyc_YY35(({struct _tuple25 _tmp58B;_tmp58B.f1=_tmp58A.f1;_tmp58B.f2=_tmp58A.f2;({struct Cyc_List_List*_tmpB21=({struct Cyc_List_List*_tmpB20=Cyc_yyget_YY45(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpB20,_tmp58A.f3);});_tmp58B.f3=_tmpB21;});_tmp58B;}));yyval=_tmpB22;});
goto _LL254;}case 133U: _LL359: _LL35A: {
# 1830
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1832
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpB25=Cyc_YY35(({struct _tuple25 _tmp58C;({struct Cyc_Absyn_Tqual _tmpB23=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp58C.f1=_tmpB23;});({struct Cyc_Parse_Type_specifier _tmpB24=Cyc_yyget_YY21(yyyyvsp[0]);_tmp58C.f2=_tmpB24;});_tmp58C.f3=0;_tmp58C;}));yyval=_tmpB25;});
goto _LL254;}case 134U: _LL35B: _LL35C: {
# 1836
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1838
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1835 "parse.y"
struct _tuple25 _tmp58D=Cyc_yyget_YY35(yyyyvsp[1]);({union Cyc_YYSTYPE _tmpB29=Cyc_YY35(({struct _tuple25 _tmp58E;_tmp58E.f1=_tmp58D.f1;({struct Cyc_Parse_Type_specifier _tmpB28=({unsigned int _tmpB27=(unsigned int)(yyyylsp[0]).first_line;struct Cyc_Parse_Type_specifier _tmpB26=Cyc_yyget_YY21(yyyyvsp[0]);Cyc_Parse_combine_specifiers(_tmpB27,_tmpB26,_tmp58D.f2);});_tmp58E.f2=_tmpB28;});_tmp58E.f3=_tmp58D.f3;_tmp58E;}));yyval=_tmpB29;});
goto _LL254;}case 135U: _LL35D: _LL35E: {
# 1838
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1840
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1837 "parse.y"
({union Cyc_YYSTYPE _tmpB2C=Cyc_YY35(({struct _tuple25 _tmp58F;({struct Cyc_Absyn_Tqual _tmpB2A=Cyc_yyget_YY23(yyyyvsp[0]);_tmp58F.f1=_tmpB2A;});({struct Cyc_Parse_Type_specifier _tmpB2B=Cyc_Parse_empty_spec(0U);_tmp58F.f2=_tmpB2B;});_tmp58F.f3=0;_tmp58F;}));yyval=_tmpB2C;});
goto _LL254;}case 136U: _LL35F: _LL360: {
# 1840
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1842
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1839 "parse.y"
struct _tuple25 _tmp590=Cyc_yyget_YY35(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmpB2F=Cyc_YY35(({struct _tuple25 _tmp591;({struct Cyc_Absyn_Tqual _tmpB2E=({struct Cyc_Absyn_Tqual _tmpB2D=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmpB2D,_tmp590.f1);});_tmp591.f1=_tmpB2E;});_tmp591.f2=_tmp590.f2;_tmp591.f3=_tmp590.f3;_tmp591;}));yyval=_tmpB2F;});
goto _LL254;}case 137U: _LL361: _LL362: {
# 1843
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1845
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1842 "parse.y"
({union Cyc_YYSTYPE _tmpB33=Cyc_YY35(({struct _tuple25 _tmp592;({struct Cyc_Absyn_Tqual _tmpB30=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp592.f1=_tmpB30;});({struct Cyc_Parse_Type_specifier _tmpB31=Cyc_Parse_empty_spec(0U);_tmp592.f2=_tmpB31;});({struct Cyc_List_List*_tmpB32=Cyc_yyget_YY45(yyyyvsp[0]);_tmp592.f3=_tmpB32;});_tmp592;}));yyval=_tmpB33;});
goto _LL254;}case 138U: _LL363: _LL364: {
# 1845
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1847
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1844 "parse.y"
struct _tuple25 _tmp593=Cyc_yyget_YY35(yyyyvsp[1]);({union Cyc_YYSTYPE _tmpB36=Cyc_YY35(({struct _tuple25 _tmp594;_tmp594.f1=_tmp593.f1;_tmp594.f2=_tmp593.f2;({struct Cyc_List_List*_tmpB35=({struct Cyc_List_List*_tmpB34=Cyc_yyget_YY45(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpB34,_tmp593.f3);});_tmp594.f3=_tmpB35;});_tmp594;}));yyval=_tmpB36;});
goto _LL254;}case 139U: _LL365: _LL366: {
# 1847
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1849
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1848 "parse.y"
({union Cyc_YYSTYPE _tmpB37=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY29(yyyyvsp[0])));yyval=_tmpB37;});
goto _LL254;}case 140U: _LL367: _LL368: {
# 1851
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpB39=Cyc_YY29(({struct Cyc_List_List*_tmp595=_region_malloc(yyr,sizeof(*_tmp595));({struct _tuple24*_tmpB38=Cyc_yyget_YY28(yyyyvsp[0]);_tmp595->hd=_tmpB38;});_tmp595->tl=0;_tmp595;}));yyval=_tmpB39;});
goto _LL254;}case 141U: _LL369: _LL36A: {
# 1857
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1859
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1856 "parse.y"
({union Cyc_YYSTYPE _tmpB3C=Cyc_YY29(({struct Cyc_List_List*_tmp596=_region_malloc(yyr,sizeof(*_tmp596));({struct _tuple24*_tmpB3A=Cyc_yyget_YY28(yyyyvsp[2]);_tmp596->hd=_tmpB3A;});({struct Cyc_List_List*_tmpB3B=Cyc_yyget_YY29(yyyyvsp[0]);_tmp596->tl=_tmpB3B;});_tmp596;}));yyval=_tmpB3C;});
goto _LL254;}case 142U: _LL36B: _LL36C: {
# 1859
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1861
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1861 "parse.y"
({union Cyc_YYSTYPE _tmpB3F=Cyc_YY28(({struct _tuple24*_tmp597=_region_malloc(yyr,sizeof(*_tmp597));({struct Cyc_Parse_Declarator _tmpB3D=Cyc_yyget_YY27(yyyyvsp[0]);_tmp597->f1=_tmpB3D;});_tmp597->f2=0;({struct Cyc_Absyn_Exp*_tmpB3E=Cyc_yyget_YY55(yyyyvsp[1]);_tmp597->f3=_tmpB3E;});_tmp597;}));yyval=_tmpB3F;});
goto _LL254;}case 143U: _LL36D: _LL36E: {
# 1864
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1866
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1865 "parse.y"
({union Cyc_YYSTYPE _tmpB45=Cyc_YY28(({struct _tuple24*_tmp598=_region_malloc(yyr,sizeof(*_tmp598));({struct Cyc_Parse_Declarator _tmpB43=({struct Cyc_Parse_Declarator _tmp599;({struct _tuple0*_tmpB42=({struct _tuple0*_tmp59A=_cycalloc(sizeof(*_tmp59A));({union Cyc_Absyn_Nmspace _tmpB40=Cyc_Absyn_Rel_n(0);_tmp59A->f1=_tmpB40;});({struct _dyneither_ptr*_tmpB41=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1U);_tmp59A->f2=_tmpB41;});_tmp59A;});_tmp599.id=_tmpB42;});_tmp599.varloc=0U;_tmp599.tms=0;_tmp599;});_tmp598->f1=_tmpB43;});({struct Cyc_Absyn_Exp*_tmpB44=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp598->f2=_tmpB44;});_tmp598->f3=0;_tmp598;}));yyval=_tmpB45;});
# 1867
goto _LL254;}case 144U: _LL36F: _LL370:
# 1871
({union Cyc_YYSTYPE _tmpB4A=Cyc_YY28(({struct _tuple24*_tmp59C=_region_malloc(yyr,sizeof(*_tmp59C));({struct Cyc_Parse_Declarator _tmpB49=({struct Cyc_Parse_Declarator _tmp59D;({struct _tuple0*_tmpB48=({struct _tuple0*_tmp59E=_cycalloc(sizeof(*_tmp59E));({union Cyc_Absyn_Nmspace _tmpB46=Cyc_Absyn_Rel_n(0);_tmp59E->f1=_tmpB46;});({struct _dyneither_ptr*_tmpB47=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1U);_tmp59E->f2=_tmpB47;});_tmp59E;});_tmp59D.id=_tmpB48;});_tmp59D.varloc=0U;_tmp59D.tms=0;_tmp59D;});_tmp59C->f1=_tmpB49;});_tmp59C->f2=0;_tmp59C->f3=0;_tmp59C;}));yyval=_tmpB4A;});
# 1873
goto _LL254;case 145U: _LL371: _LL372: {
# 1875
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1877
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1873 "parse.y"
({union Cyc_YYSTYPE _tmpB4D=Cyc_YY28(({struct _tuple24*_tmp5A0=_region_malloc(yyr,sizeof(*_tmp5A0));({struct Cyc_Parse_Declarator _tmpB4B=Cyc_yyget_YY27(yyyyvsp[0]);_tmp5A0->f1=_tmpB4B;});({struct Cyc_Absyn_Exp*_tmpB4C=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp5A0->f2=_tmpB4C;});_tmp5A0->f3=0;_tmp5A0;}));yyval=_tmpB4D;});
goto _LL254;}case 146U: _LL373: _LL374: {
# 1876
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1878
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1877 "parse.y"
({union Cyc_YYSTYPE _tmpB4E=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));yyval=_tmpB4E;});
goto _LL254;}case 147U: _LL375: _LL376:
# 1880
({union Cyc_YYSTYPE _tmpB4F=Cyc_YY55(0);yyval=_tmpB4F;});
goto _LL254;case 148U: _LL377: _LL378: {
# 1883
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1885
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1882 "parse.y"
({union Cyc_YYSTYPE _tmpB50=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));yyval=_tmpB50;});
goto _LL254;}case 149U: _LL379: _LL37A:
# 1885
({union Cyc_YYSTYPE _tmpB51=Cyc_YY55(0);yyval=_tmpB51;});
goto _LL254;case 150U: _LL37B: _LL37C: {
# 1888
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1889 "parse.y"
int _tmp5A1=Cyc_yyget_YY31(yyyyvsp[0]);
struct Cyc_List_List*_tmp5A2=({unsigned int _tmpB52=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpB52,Cyc_yyget_YY40(yyyyvsp[2]));});
struct Cyc_Absyn_TypeDecl*_tmp5A3=({struct _tuple0*_tmpB57=Cyc_yyget_QualId_tok(yyyyvsp[1]);struct Cyc_List_List*_tmpB56=_tmp5A2;struct Cyc_Core_Opt*_tmpB55=({struct Cyc_Core_Opt*_tmp5A6=_cycalloc(sizeof(*_tmp5A6));({struct Cyc_List_List*_tmpB53=Cyc_yyget_YY34(yyyyvsp[4]);_tmp5A6->v=_tmpB53;});_tmp5A6;});int _tmpB54=_tmp5A1;Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,_tmpB57,_tmpB56,_tmpB55,_tmpB54,(unsigned int)(yyyylsp[0]).first_line);});
# 1893
({union Cyc_YYSTYPE _tmpB5A=Cyc_YY21(({void*_tmpB59=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB58=({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmp5A5;_tmp5A5.tag=26U;_tmp5A5.f1=_tmp5A3;_tmp5A5.f2=0;_tmp5A5;});_tmp5A4[0]=_tmpB58;});_tmp5A4;});Cyc_Parse_type_spec(_tmpB59,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB5A;});
# 1895
goto _LL254;}case 151U: _LL37D: _LL37E: {
# 1897
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1899
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1896 "parse.y"
int _tmp5A7=Cyc_yyget_YY31(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpB61=Cyc_YY21(({void*_tmpB60=(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5A8=_cycalloc(sizeof(*_tmp5A8));({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpB5F=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp5A9;_tmp5A9.tag=3U;({struct Cyc_Absyn_DatatypeInfo _tmpB5E=({struct Cyc_Absyn_DatatypeInfo _tmp5AA;({union Cyc_Absyn_DatatypeInfoU _tmpB5C=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp5AB;({struct _tuple0*_tmpB5B=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp5AB.name=_tmpB5B;});_tmp5AB.is_extensible=_tmp5A7;_tmp5AB;}));_tmp5AA.datatype_info=_tmpB5C;});({struct Cyc_List_List*_tmpB5D=Cyc_yyget_YY40(yyyyvsp[2]);_tmp5AA.targs=_tmpB5D;});_tmp5AA;});_tmp5A9.f1=_tmpB5E;});_tmp5A9;});_tmp5A8[0]=_tmpB5F;});_tmp5A8;});Cyc_Parse_type_spec(_tmpB60,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB61;});
# 1899
goto _LL254;}case 152U: _LL37F: _LL380: {
# 1901
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1903
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1900 "parse.y"
int _tmp5AC=Cyc_yyget_YY31(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpB69=Cyc_YY21(({void*_tmpB68=(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp5AD=_cycalloc(sizeof(*_tmp5AD));({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpB67=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp5AE;_tmp5AE.tag=4U;({struct Cyc_Absyn_DatatypeFieldInfo _tmpB66=({struct Cyc_Absyn_DatatypeFieldInfo _tmp5AF;({union Cyc_Absyn_DatatypeFieldInfoU _tmpB64=
Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp5B0;({struct _tuple0*_tmpB62=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp5B0.datatype_name=_tmpB62;});({struct _tuple0*_tmpB63=Cyc_yyget_QualId_tok(yyyyvsp[3]);_tmp5B0.field_name=_tmpB63;});_tmp5B0.is_extensible=_tmp5AC;_tmp5B0;}));_tmp5AF.field_info=_tmpB64;});({struct Cyc_List_List*_tmpB65=Cyc_yyget_YY40(yyyyvsp[4]);_tmp5AF.targs=_tmpB65;});_tmp5AF;});_tmp5AE.f1=_tmpB66;});_tmp5AE;});_tmp5AD[0]=_tmpB67;});_tmp5AD;});
# 1901
Cyc_Parse_type_spec(_tmpB68,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB69;});
# 1905
goto _LL254;}case 153U: _LL381: _LL382: {
# 1907
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1909
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1908 "parse.y"
({union Cyc_YYSTYPE _tmpB6A=Cyc_YY31(0);yyval=_tmpB6A;});
goto _LL254;}case 154U: _LL383: _LL384: {
# 1911
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1913
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1909 "parse.y"
({union Cyc_YYSTYPE _tmpB6B=Cyc_YY31(1);yyval=_tmpB6B;});
goto _LL254;}case 155U: _LL385: _LL386: {
# 1912
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1914
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1913 "parse.y"
({union Cyc_YYSTYPE _tmpB6D=Cyc_YY34(({struct Cyc_List_List*_tmp5B1=_cycalloc(sizeof(*_tmp5B1));({struct Cyc_Absyn_Datatypefield*_tmpB6C=Cyc_yyget_YY33(yyyyvsp[0]);_tmp5B1->hd=_tmpB6C;});_tmp5B1->tl=0;_tmp5B1;}));yyval=_tmpB6D;});
goto _LL254;}case 156U: _LL387: _LL388: {
# 1916
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1918
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1914 "parse.y"
({union Cyc_YYSTYPE _tmpB6F=Cyc_YY34(({struct Cyc_List_List*_tmp5B2=_cycalloc(sizeof(*_tmp5B2));({struct Cyc_Absyn_Datatypefield*_tmpB6E=Cyc_yyget_YY33(yyyyvsp[0]);_tmp5B2->hd=_tmpB6E;});_tmp5B2->tl=0;_tmp5B2;}));yyval=_tmpB6F;});
goto _LL254;}case 157U: _LL389: _LL38A: {
# 1917
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1919
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1915 "parse.y"
({union Cyc_YYSTYPE _tmpB72=Cyc_YY34(({struct Cyc_List_List*_tmp5B3=_cycalloc(sizeof(*_tmp5B3));({struct Cyc_Absyn_Datatypefield*_tmpB70=Cyc_yyget_YY33(yyyyvsp[0]);_tmp5B3->hd=_tmpB70;});({struct Cyc_List_List*_tmpB71=Cyc_yyget_YY34(yyyyvsp[2]);_tmp5B3->tl=_tmpB71;});_tmp5B3;}));yyval=_tmpB72;});
goto _LL254;}case 158U: _LL38B: _LL38C: {
# 1918
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1920
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1916 "parse.y"
({union Cyc_YYSTYPE _tmpB75=Cyc_YY34(({struct Cyc_List_List*_tmp5B4=_cycalloc(sizeof(*_tmp5B4));({struct Cyc_Absyn_Datatypefield*_tmpB73=Cyc_yyget_YY33(yyyyvsp[0]);_tmp5B4->hd=_tmpB73;});({struct Cyc_List_List*_tmpB74=Cyc_yyget_YY34(yyyyvsp[2]);_tmp5B4->tl=_tmpB74;});_tmp5B4;}));yyval=_tmpB75;});
goto _LL254;}case 159U: _LL38D: _LL38E:
# 1919
({union Cyc_YYSTYPE _tmpB76=Cyc_YY32(Cyc_Absyn_Public);yyval=_tmpB76;});
goto _LL254;case 160U: _LL38F: _LL390: {
# 1922
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1924
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1921 "parse.y"
({union Cyc_YYSTYPE _tmpB77=Cyc_YY32(Cyc_Absyn_Extern);yyval=_tmpB77;});
goto _LL254;}case 161U: _LL391: _LL392: {
# 1924
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1926
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1922 "parse.y"
({union Cyc_YYSTYPE _tmpB78=Cyc_YY32(Cyc_Absyn_Static);yyval=_tmpB78;});
goto _LL254;}case 162U: _LL393: _LL394: {
# 1925
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1927
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1926 "parse.y"
({union Cyc_YYSTYPE _tmpB7B=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp5B5=_cycalloc(sizeof(*_tmp5B5));({struct _tuple0*_tmpB79=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp5B5->name=_tmpB79;});_tmp5B5->typs=0;_tmp5B5->loc=(unsigned int)(yyyylsp[0]).first_line;({enum Cyc_Absyn_Scope _tmpB7A=Cyc_yyget_YY32(yyyyvsp[0]);_tmp5B5->sc=_tmpB7A;});_tmp5B5;}));yyval=_tmpB7B;});
goto _LL254;}case 163U: _LL395: _LL396: {
# 1929
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1931
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1928 "parse.y"
struct Cyc_List_List*_tmp5B6=({unsigned int _tmpB7C=(unsigned int)(yyyylsp[3]).first_line;((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmpB7C,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[3])));});
({union Cyc_YYSTYPE _tmpB7F=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp5B7=_cycalloc(sizeof(*_tmp5B7));({struct _tuple0*_tmpB7D=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp5B7->name=_tmpB7D;});_tmp5B7->typs=_tmp5B6;_tmp5B7->loc=(unsigned int)(yyyylsp[0]).first_line;({enum Cyc_Absyn_Scope _tmpB7E=Cyc_yyget_YY32(yyyyvsp[0]);_tmp5B7->sc=_tmpB7E;});_tmp5B7;}));yyval=_tmpB7F;});
goto _LL254;}case 164U: _LL397: _LL398: {
# 1932
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1934
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1934 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 165U: _LL399: _LL39A: {
# 1937
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1939
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1936 "parse.y"
struct Cyc_Parse_Declarator _tmp5B8=Cyc_yyget_YY27(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmpB82=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5B9;_tmp5B9.id=_tmp5B8.id;_tmp5B9.varloc=_tmp5B8.varloc;({struct Cyc_List_List*_tmpB81=({struct Cyc_List_List*_tmpB80=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpB80,_tmp5B8.tms);});_tmp5B9.tms=_tmpB81;});_tmp5B9;}));yyval=_tmpB82;});
goto _LL254;}case 166U: _LL39B: _LL39C: {
# 1940
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1942
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=yyyyvsp[0];
goto _LL254;}case 167U: _LL39D: _LL39E: {
# 1946
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1945 "parse.y"
struct Cyc_Parse_Declarator _tmp5BA=Cyc_yyget_YY27(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmpB85=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5BB;_tmp5BB.id=_tmp5BA.id;_tmp5BB.varloc=_tmp5BA.varloc;({struct Cyc_List_List*_tmpB84=({struct Cyc_List_List*_tmpB83=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpB83,_tmp5BA.tms);});_tmp5BB.tms=_tmpB84;});_tmp5BB;}));yyval=_tmpB85;});
goto _LL254;}case 168U: _LL39F: _LL3A0: {
# 1949
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1951 "parse.y"
({union Cyc_YYSTYPE _tmpB87=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5BC;({struct _tuple0*_tmpB86=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp5BC.id=_tmpB86;});_tmp5BC.varloc=(unsigned int)(yyyylsp[0]).first_line;_tmp5BC.tms=0;_tmp5BC;}));yyval=_tmpB87;});
goto _LL254;}case 169U: _LL3A1: _LL3A2: {
# 1954
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1956
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1953 "parse.y"
yyval=yyyyvsp[1];
goto _LL254;}case 170U: _LL3A3: _LL3A4: {
# 1956
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1958
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1957 "parse.y"
struct Cyc_Parse_Declarator _tmp5BD=Cyc_yyget_YY27(yyyyvsp[2]);
({struct Cyc_List_List*_tmpB8B=({struct Cyc_List_List*_tmp5BE=_region_malloc(yyr,sizeof(*_tmp5BE));({void*_tmpB8A=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp5BF=_region_malloc(yyr,sizeof(*_tmp5BF));({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpB89=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp5C0;_tmp5C0.tag=5U;_tmp5C0.f1=(unsigned int)(yyyylsp[1]).first_line;({struct Cyc_List_List*_tmpB88=Cyc_yyget_YY45(yyyyvsp[1]);_tmp5C0.f2=_tmpB88;});_tmp5C0;});_tmp5BF[0]=_tmpB89;});_tmp5BF;});_tmp5BE->hd=_tmpB8A;});_tmp5BE->tl=_tmp5BD.tms;_tmp5BE;});_tmp5BD.tms=_tmpB8B;});
yyval=yyyyvsp[2];
# 1961
goto _LL254;}case 171U: _LL3A5: _LL3A6: {
# 1963
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1962 "parse.y"
({union Cyc_YYSTYPE _tmpB93=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5C1;({struct _tuple0*_tmpB8C=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5C1.id=_tmpB8C;});({unsigned int _tmpB8D=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp5C1.varloc=_tmpB8D;});({struct Cyc_List_List*_tmpB92=({struct Cyc_List_List*_tmp5C2=_region_malloc(yyr,sizeof(*_tmp5C2));({void*_tmpB90=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5C3=_region_malloc(yyr,sizeof(*_tmp5C3));({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpB8F=({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmp5C4;_tmp5C4.tag=0U;({union Cyc_Absyn_Constraint*_tmpB8E=Cyc_yyget_YY51(yyyyvsp[3]);_tmp5C4.f1=_tmpB8E;});_tmp5C4.f2=(unsigned int)(yyyylsp[3]).first_line;_tmp5C4;});_tmp5C3[0]=_tmpB8F;});_tmp5C3;});_tmp5C2->hd=_tmpB90;});({struct Cyc_List_List*_tmpB91=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5C2->tl=_tmpB91;});_tmp5C2;});_tmp5C1.tms=_tmpB92;});_tmp5C1;}));yyval=_tmpB93;});
goto _LL254;}case 172U: _LL3A7: _LL3A8: {
# 1965
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1964 "parse.y"
({union Cyc_YYSTYPE _tmpB9C=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5C5;({struct _tuple0*_tmpB94=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5C5.id=_tmpB94;});({unsigned int _tmpB95=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp5C5.varloc=_tmpB95;});({struct Cyc_List_List*_tmpB9B=({struct Cyc_List_List*_tmp5C6=_region_malloc(yyr,sizeof(*_tmp5C6));({void*_tmpB99=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5C7=_region_malloc(yyr,sizeof(*_tmp5C7));({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpB98=({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmp5C8;_tmp5C8.tag=1U;({struct Cyc_Absyn_Exp*_tmpB96=
Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp5C8.f1=_tmpB96;});({union Cyc_Absyn_Constraint*_tmpB97=Cyc_yyget_YY51(yyyyvsp[4]);_tmp5C8.f2=_tmpB97;});_tmp5C8.f3=(unsigned int)(yyyylsp[4]).first_line;_tmp5C8;});_tmp5C7[0]=_tmpB98;});_tmp5C7;});_tmp5C6->hd=_tmpB99;});({struct Cyc_List_List*_tmpB9A=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5C6->tl=_tmpB9A;});_tmp5C6;});_tmp5C5.tms=_tmpB9B;});_tmp5C5;}));
# 1964
yyval=_tmpB9C;});
# 1966
goto _LL254;}case 173U: _LL3A9: _LL3AA: {
# 1968
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1970
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1967 "parse.y"
struct _tuple26*_tmp5C9=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp5CA=_tmp5C9;struct Cyc_List_List*_tmp5D7;int _tmp5D6;struct Cyc_Absyn_VarargInfo*_tmp5D5;void*_tmp5D4;struct Cyc_List_List*_tmp5D3;_LL691: _tmp5D7=_tmp5CA->f1;_tmp5D6=_tmp5CA->f2;_tmp5D5=_tmp5CA->f3;_tmp5D4=_tmp5CA->f4;_tmp5D3=_tmp5CA->f5;_LL692:;{
struct Cyc_Absyn_Exp*_tmp5CB=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp5CC=Cyc_yyget_YY55(yyyyvsp[5]);
({union Cyc_YYSTYPE _tmpBA5=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5CD;({struct _tuple0*_tmpB9D=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5CD.id=_tmpB9D;});({unsigned int _tmpB9E=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp5CD.varloc=_tmpB9E;});({struct Cyc_List_List*_tmpBA4=({struct Cyc_List_List*_tmp5CE=_region_malloc(yyr,sizeof(*_tmp5CE));({void*_tmpBA2=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5CF=_region_malloc(yyr,sizeof(*_tmp5CF));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBA1=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp5D0;_tmp5D0.tag=3U;({void*_tmpBA0=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5D1=_region_malloc(yyr,sizeof(*_tmp5D1));({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpB9F=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp5D2;_tmp5D2.tag=1U;_tmp5D2.f1=_tmp5D7;_tmp5D2.f2=_tmp5D6;_tmp5D2.f3=_tmp5D5;_tmp5D2.f4=_tmp5D4;_tmp5D2.f5=_tmp5D3;_tmp5D2.f6=_tmp5CB;_tmp5D2.f7=_tmp5CC;_tmp5D2;});_tmp5D1[0]=_tmpB9F;});_tmp5D1;});_tmp5D0.f1=_tmpBA0;});_tmp5D0;});_tmp5CF[0]=_tmpBA1;});_tmp5CF;});_tmp5CE->hd=_tmpBA2;});({struct Cyc_List_List*_tmpBA3=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5CE->tl=_tmpBA3;});_tmp5CE;});_tmp5CD.tms=_tmpBA4;});_tmp5CD;}));yyval=_tmpBA5;});
# 1972
goto _LL254;};}case 174U: _LL3AB: _LL3AC: {
# 1974
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 1976
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 1973 "parse.y"
({union Cyc_YYSTYPE _tmpBB2=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5D8;({struct _tuple0*_tmpBA6=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5D8.id=_tmpBA6;});({unsigned int _tmpBA7=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp5D8.varloc=_tmpBA7;});({struct Cyc_List_List*_tmpBB1=({struct Cyc_List_List*_tmp5D9=_region_malloc(yyr,sizeof(*_tmp5D9));({void*_tmpBAF=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5DA=_region_malloc(yyr,sizeof(*_tmp5DA));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBAE=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp5DB;_tmp5DB.tag=3U;({void*_tmpBAD=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5DC=_region_malloc(yyr,sizeof(*_tmp5DC));({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBAC=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp5DD;_tmp5DD.tag=1U;_tmp5DD.f1=0;_tmp5DD.f2=0;_tmp5DD.f3=0;({void*_tmpBA8=
# 1976
Cyc_yyget_YY49(yyyyvsp[2]);_tmp5DD.f4=_tmpBA8;});({struct Cyc_List_List*_tmpBA9=Cyc_yyget_YY50(yyyyvsp[3]);_tmp5DD.f5=_tmpBA9;});({struct Cyc_Absyn_Exp*_tmpBAA=Cyc_yyget_YY55(yyyyvsp[5]);_tmp5DD.f6=_tmpBAA;});({struct Cyc_Absyn_Exp*_tmpBAB=Cyc_yyget_YY55(yyyyvsp[6]);_tmp5DD.f7=_tmpBAB;});_tmp5DD;});_tmp5DC[0]=_tmpBAC;});_tmp5DC;});_tmp5DB.f1=_tmpBAD;});_tmp5DB;});_tmp5DA[0]=_tmpBAE;});_tmp5DA;});_tmp5D9->hd=_tmpBAF;});({struct Cyc_List_List*_tmpBB0=(
Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5D9->tl=_tmpBB0;});_tmp5D9;});_tmp5D8.tms=_tmpBB1;});_tmp5D8;}));
# 1973
yyval=_tmpBB2;});
# 1979
goto _LL254;}case 175U: _LL3AD: _LL3AE: {
# 1981
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1983
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1980 "parse.y"
({union Cyc_YYSTYPE _tmpBBC=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5DE;({struct _tuple0*_tmpBB3=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5DE.id=_tmpBB3;});({unsigned int _tmpBB4=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp5DE.varloc=_tmpBB4;});({struct Cyc_List_List*_tmpBBB=({struct Cyc_List_List*_tmp5DF=_region_malloc(yyr,sizeof(*_tmp5DF));({void*_tmpBB9=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5E0=_region_malloc(yyr,sizeof(*_tmp5E0));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBB8=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp5E1;_tmp5E1.tag=3U;({void*_tmpBB7=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp5E2=_region_malloc(yyr,sizeof(*_tmp5E2));({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpBB6=({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmp5E3;_tmp5E3.tag=0U;({struct Cyc_List_List*_tmpBB5=Cyc_yyget_YY36(yyyyvsp[2]);_tmp5E3.f1=_tmpBB5;});_tmp5E3.f2=(unsigned int)(yyyylsp[0]).first_line;_tmp5E3;});_tmp5E2[0]=_tmpBB6;});_tmp5E2;});_tmp5E1.f1=_tmpBB7;});_tmp5E1;});_tmp5E0[0]=_tmpBB8;});_tmp5E0;});_tmp5DF->hd=_tmpBB9;});({struct Cyc_List_List*_tmpBBA=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5DF->tl=_tmpBBA;});_tmp5DF;});_tmp5DE.tms=_tmpBBB;});_tmp5DE;}));yyval=_tmpBBC;});
goto _LL254;}case 176U: _LL3AF: _LL3B0: {
# 1983
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1985
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1983 "parse.y"
struct Cyc_List_List*_tmp5E4=({unsigned int _tmpBBD=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpBBD,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));});
({union Cyc_YYSTYPE _tmpBC4=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5E5;({struct _tuple0*_tmpBBE=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5E5.id=_tmpBBE;});({unsigned int _tmpBBF=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp5E5.varloc=_tmpBBF;});({struct Cyc_List_List*_tmpBC3=({struct Cyc_List_List*_tmp5E6=_region_malloc(yyr,sizeof(*_tmp5E6));({void*_tmpBC1=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp5E7=_region_malloc(yyr,sizeof(*_tmp5E7));({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpBC0=({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmp5E8;_tmp5E8.tag=4U;_tmp5E8.f1=_tmp5E4;_tmp5E8.f2=(unsigned int)(yyyylsp[0]).first_line;_tmp5E8.f3=0;_tmp5E8;});_tmp5E7[0]=_tmpBC0;});_tmp5E7;});_tmp5E6->hd=_tmpBC1;});({struct Cyc_List_List*_tmpBC2=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5E6->tl=_tmpBC2;});_tmp5E6;});_tmp5E5.tms=_tmpBC3;});_tmp5E5;}));yyval=_tmpBC4;});
# 1986
goto _LL254;}case 177U: _LL3B1: _LL3B2: {
# 1988
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1990
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1987 "parse.y"
({union Cyc_YYSTYPE _tmpBCC=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5E9;({struct _tuple0*_tmpBC5=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp5E9.id=_tmpBC5;});({unsigned int _tmpBC6=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp5E9.varloc=_tmpBC6;});({struct Cyc_List_List*_tmpBCB=({struct Cyc_List_List*_tmp5EA=_region_malloc(yyr,sizeof(*_tmp5EA));({void*_tmpBC9=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp5EB=_region_malloc(yyr,sizeof(*_tmp5EB));({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBC8=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp5EC;_tmp5EC.tag=5U;_tmp5EC.f1=(unsigned int)(yyyylsp[1]).first_line;({struct Cyc_List_List*_tmpBC7=Cyc_yyget_YY45(yyyyvsp[1]);_tmp5EC.f2=_tmpBC7;});_tmp5EC;});_tmp5EB[0]=_tmpBC8;});_tmp5EB;});_tmp5EA->hd=_tmpBC9;});({struct Cyc_List_List*_tmpBCA=(
Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp5EA->tl=_tmpBCA;});_tmp5EA;});_tmp5E9.tms=_tmpBCB;});_tmp5E9;}));
# 1987
yyval=_tmpBCC;});
# 1990
goto _LL254;}case 178U: _LL3B3: _LL3B4: {
# 1992
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1994
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpBCE=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5ED;({struct _tuple0*_tmpBCD=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp5ED.id=_tmpBCD;});_tmp5ED.varloc=(unsigned int)(yyyylsp[0]).first_line;_tmp5ED.tms=0;_tmp5ED;}));yyval=_tmpBCE;});
goto _LL254;}case 179U: _LL3B5: _LL3B6: {
# 1998
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2000
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1997 "parse.y"
({union Cyc_YYSTYPE _tmpBD0=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5EE;({struct _tuple0*_tmpBCF=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp5EE.id=_tmpBCF;});_tmp5EE.varloc=(unsigned int)(yyyylsp[0]).first_line;_tmp5EE.tms=0;_tmp5EE;}));yyval=_tmpBD0;});
goto _LL254;}case 180U: _LL3B7: _LL3B8: {
# 2000
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2002
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1999 "parse.y"
yyval=yyyyvsp[1];
goto _LL254;}case 181U: _LL3B9: _LL3BA: {
# 2002
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2004
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2003 "parse.y"
struct Cyc_Parse_Declarator _tmp5EF=Cyc_yyget_YY27(yyyyvsp[2]);
({struct Cyc_List_List*_tmpBD4=({struct Cyc_List_List*_tmp5F0=_region_malloc(yyr,sizeof(*_tmp5F0));({void*_tmpBD3=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp5F1=_region_malloc(yyr,sizeof(*_tmp5F1));({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBD2=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp5F2;_tmp5F2.tag=5U;_tmp5F2.f1=(unsigned int)(yyyylsp[1]).first_line;({struct Cyc_List_List*_tmpBD1=Cyc_yyget_YY45(yyyyvsp[1]);_tmp5F2.f2=_tmpBD1;});_tmp5F2;});_tmp5F1[0]=_tmpBD2;});_tmp5F1;});_tmp5F0->hd=_tmpBD3;});_tmp5F0->tl=_tmp5EF.tms;_tmp5F0;});_tmp5EF.tms=_tmpBD4;});
yyval=yyyyvsp[2];
# 2007
goto _LL254;}case 182U: _LL3BB: _LL3BC: {
# 2009
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2011
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2008 "parse.y"
struct Cyc_Parse_Declarator _tmp5F3=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpBD9=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5F4;_tmp5F4.id=_tmp5F3.id;_tmp5F4.varloc=_tmp5F3.varloc;({struct Cyc_List_List*_tmpBD8=({struct Cyc_List_List*_tmp5F5=_region_malloc(yyr,sizeof(*_tmp5F5));({void*_tmpBD7=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5F6=_region_malloc(yyr,sizeof(*_tmp5F6));({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpBD6=({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmp5F7;_tmp5F7.tag=0U;({union Cyc_Absyn_Constraint*_tmpBD5=
Cyc_yyget_YY51(yyyyvsp[3]);_tmp5F7.f1=_tmpBD5;});_tmp5F7.f2=(unsigned int)(yyyylsp[3]).first_line;_tmp5F7;});_tmp5F6[0]=_tmpBD6;});_tmp5F6;});_tmp5F5->hd=_tmpBD7;});_tmp5F5->tl=_tmp5F3.tms;_tmp5F5;});_tmp5F4.tms=_tmpBD8;});_tmp5F4;}));
# 2009
yyval=_tmpBD9;});
# 2011
goto _LL254;}case 183U: _LL3BD: _LL3BE: {
# 2013
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2015
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2012 "parse.y"
struct Cyc_Parse_Declarator _tmp5F8=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpBDF=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp5F9;_tmp5F9.id=_tmp5F8.id;_tmp5F9.varloc=_tmp5F8.varloc;({struct Cyc_List_List*_tmpBDE=({struct Cyc_List_List*_tmp5FA=_region_malloc(yyr,sizeof(*_tmp5FA));({void*_tmpBDD=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5FB=_region_malloc(yyr,sizeof(*_tmp5FB));({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpBDC=({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmp5FC;_tmp5FC.tag=1U;({struct Cyc_Absyn_Exp*_tmpBDA=
Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp5FC.f1=_tmpBDA;});({union Cyc_Absyn_Constraint*_tmpBDB=Cyc_yyget_YY51(yyyyvsp[4]);_tmp5FC.f2=_tmpBDB;});_tmp5FC.f3=(unsigned int)(yyyylsp[4]).first_line;_tmp5FC;});_tmp5FB[0]=_tmpBDC;});_tmp5FB;});_tmp5FA->hd=_tmpBDD;});_tmp5FA->tl=_tmp5F8.tms;_tmp5FA;});_tmp5F9.tms=_tmpBDE;});_tmp5F9;}));
# 2013
yyval=_tmpBDF;});
# 2016
goto _LL254;}case 184U: _LL3BF: _LL3C0: {
# 2018
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2020
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2017 "parse.y"
struct _tuple26*_tmp5FD=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp5FE=_tmp5FD;struct Cyc_List_List*_tmp60C;int _tmp60B;struct Cyc_Absyn_VarargInfo*_tmp60A;void*_tmp609;struct Cyc_List_List*_tmp608;_LL694: _tmp60C=_tmp5FE->f1;_tmp60B=_tmp5FE->f2;_tmp60A=_tmp5FE->f3;_tmp609=_tmp5FE->f4;_tmp608=_tmp5FE->f5;_LL695:;{
struct Cyc_Absyn_Exp*_tmp5FF=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp600=Cyc_yyget_YY55(yyyyvsp[5]);
struct Cyc_Parse_Declarator _tmp601=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpBE5=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp602;_tmp602.id=_tmp601.id;_tmp602.varloc=_tmp601.varloc;({struct Cyc_List_List*_tmpBE4=({struct Cyc_List_List*_tmp603=_region_malloc(yyr,sizeof(*_tmp603));({void*_tmpBE3=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp604=_region_malloc(yyr,sizeof(*_tmp604));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBE2=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp605;_tmp605.tag=3U;({void*_tmpBE1=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp606=_region_malloc(yyr,sizeof(*_tmp606));({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBE0=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp607;_tmp607.tag=1U;_tmp607.f1=_tmp60C;_tmp607.f2=_tmp60B;_tmp607.f3=_tmp60A;_tmp607.f4=_tmp609;_tmp607.f5=_tmp608;_tmp607.f6=_tmp5FF;_tmp607.f7=_tmp600;_tmp607;});_tmp606[0]=_tmpBE0;});_tmp606;});_tmp605.f1=_tmpBE1;});_tmp605;});_tmp604[0]=_tmpBE2;});_tmp604;});_tmp603->hd=_tmpBE3;});_tmp603->tl=_tmp601.tms;_tmp603;});_tmp602.tms=_tmpBE4;});_tmp602;}));yyval=_tmpBE5;});
# 2023
goto _LL254;};}case 185U: _LL3C1: _LL3C2: {
# 2025
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2027
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2024 "parse.y"
struct Cyc_Parse_Declarator _tmp60D=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpBEF=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp60E;_tmp60E.id=_tmp60D.id;_tmp60E.varloc=_tmp60D.varloc;({struct Cyc_List_List*_tmpBEE=({struct Cyc_List_List*_tmp60F=_region_malloc(yyr,sizeof(*_tmp60F));({void*_tmpBED=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp610=_region_malloc(yyr,sizeof(*_tmp610));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBEC=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp611;_tmp611.tag=3U;({void*_tmpBEB=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp612=_region_malloc(yyr,sizeof(*_tmp612));({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBEA=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp613;_tmp613.tag=1U;_tmp613.f1=0;_tmp613.f2=0;_tmp613.f3=0;({void*_tmpBE6=
# 2028
Cyc_yyget_YY49(yyyyvsp[2]);_tmp613.f4=_tmpBE6;});({struct Cyc_List_List*_tmpBE7=Cyc_yyget_YY50(yyyyvsp[3]);_tmp613.f5=_tmpBE7;});({struct Cyc_Absyn_Exp*_tmpBE8=Cyc_yyget_YY55(yyyyvsp[5]);_tmp613.f6=_tmpBE8;});({struct Cyc_Absyn_Exp*_tmpBE9=Cyc_yyget_YY55(yyyyvsp[6]);_tmp613.f7=_tmpBE9;});_tmp613;});_tmp612[0]=_tmpBEA;});_tmp612;});_tmp611.f1=_tmpBEB;});_tmp611;});_tmp610[0]=_tmpBEC;});_tmp610;});_tmp60F->hd=_tmpBED;});_tmp60F->tl=_tmp60D.tms;_tmp60F;});_tmp60E.tms=_tmpBEE;});_tmp60E;}));
# 2025
yyval=_tmpBEF;});
# 2031
goto _LL254;}case 186U: _LL3C3: _LL3C4: {
# 2033
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2035
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2032 "parse.y"
struct Cyc_Parse_Declarator _tmp614=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpBF6=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp615;_tmp615.id=_tmp614.id;_tmp615.varloc=_tmp614.varloc;({struct Cyc_List_List*_tmpBF5=({struct Cyc_List_List*_tmp616=_region_malloc(yyr,sizeof(*_tmp616));({void*_tmpBF4=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp617=_region_malloc(yyr,sizeof(*_tmp617));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBF3=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp618;_tmp618.tag=3U;({void*_tmpBF2=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp619=_region_malloc(yyr,sizeof(*_tmp619));({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpBF1=({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmp61A;_tmp61A.tag=0U;({struct Cyc_List_List*_tmpBF0=Cyc_yyget_YY36(yyyyvsp[2]);_tmp61A.f1=_tmpBF0;});_tmp61A.f2=(unsigned int)(yyyylsp[0]).first_line;_tmp61A;});_tmp619[0]=_tmpBF1;});_tmp619;});_tmp618.f1=_tmpBF2;});_tmp618;});_tmp617[0]=_tmpBF3;});_tmp617;});_tmp616->hd=_tmpBF4;});_tmp616->tl=_tmp614.tms;_tmp616;});_tmp615.tms=_tmpBF5;});_tmp615;}));yyval=_tmpBF6;});
goto _LL254;}case 187U: _LL3C5: _LL3C6: {
# 2036
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2038
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2036 "parse.y"
struct Cyc_List_List*_tmp61B=({unsigned int _tmpBF7=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpBF7,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));});
struct Cyc_Parse_Declarator _tmp61C=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpBFB=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp61D;_tmp61D.id=_tmp61C.id;_tmp61D.varloc=_tmp61C.varloc;({struct Cyc_List_List*_tmpBFA=({struct Cyc_List_List*_tmp61E=_region_malloc(yyr,sizeof(*_tmp61E));({void*_tmpBF9=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp61F=_region_malloc(yyr,sizeof(*_tmp61F));({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpBF8=({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmp620;_tmp620.tag=4U;_tmp620.f1=_tmp61B;_tmp620.f2=(unsigned int)(yyyylsp[0]).first_line;_tmp620.f3=0;_tmp620;});_tmp61F[0]=_tmpBF8;});_tmp61F;});_tmp61E->hd=_tmpBF9;});_tmp61E->tl=_tmp61C.tms;_tmp61E;});_tmp61D.tms=_tmpBFA;});_tmp61D;}));yyval=_tmpBFB;});
# 2040
goto _LL254;}case 188U: _LL3C7: _LL3C8: {
# 2042
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2044
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2041 "parse.y"
struct Cyc_Parse_Declarator _tmp621=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpC00=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp622;_tmp622.id=_tmp621.id;_tmp622.varloc=_tmp621.varloc;({struct Cyc_List_List*_tmpBFF=({struct Cyc_List_List*_tmp623=_region_malloc(yyr,sizeof(*_tmp623));({void*_tmpBFE=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp624=_region_malloc(yyr,sizeof(*_tmp624));({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBFD=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp625;_tmp625.tag=5U;_tmp625.f1=(unsigned int)(yyyylsp[1]).first_line;({struct Cyc_List_List*_tmpBFC=Cyc_yyget_YY45(yyyyvsp[1]);_tmp625.f2=_tmpBFC;});_tmp625;});_tmp624[0]=_tmpBFD;});_tmp624;});_tmp623->hd=_tmpBFE;});_tmp623->tl=_tmp621.tms;_tmp623;});_tmp622.tms=_tmpBFF;});_tmp622;}));yyval=_tmpC00;});
# 2044
goto _LL254;}case 189U: _LL3C9: _LL3CA: {
# 2046
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2048
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2048 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 190U: _LL3CB: _LL3CC: {
# 2051
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2053
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2049 "parse.y"
({union Cyc_YYSTYPE _tmpC02=Cyc_YY26(({struct Cyc_List_List*_tmpC01=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpC01,Cyc_yyget_YY26(yyyyvsp[1]));}));yyval=_tmpC02;});
goto _LL254;}case 191U: _LL3CD: _LL3CE: {
# 2052
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2054
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2053 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY45(yyyyvsp[3])!= 0)
({struct Cyc_List_List*_tmpC06=({struct Cyc_List_List*_tmp626=_region_malloc(yyr,sizeof(*_tmp626));({void*_tmpC05=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp627=_region_malloc(yyr,sizeof(*_tmp627));({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC04=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp628;_tmp628.tag=5U;_tmp628.f1=(unsigned int)(yyyylsp[3]).first_line;({struct Cyc_List_List*_tmpC03=Cyc_yyget_YY45(yyyyvsp[3]);_tmp628.f2=_tmpC03;});_tmp628;});_tmp627[0]=_tmpC04;});_tmp627;});_tmp626->hd=_tmpC05;});_tmp626->tl=ans;_tmp626;});ans=_tmpC06;});{
# 2057
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _tmp629=*Cyc_yyget_YY1(yyyyvsp[0]);struct _tuple21 _tmp62A=_tmp629;unsigned int _tmp638;union Cyc_Absyn_Constraint*_tmp637;union Cyc_Absyn_Constraint*_tmp636;_LL697: _tmp638=_tmp62A.f1;_tmp637=_tmp62A.f2;_tmp636=_tmp62A.f3;_LL698:;
if(Cyc_Absyn_porting_c_code)
({struct Cyc_Absyn_PtrLoc*_tmpC07=({struct Cyc_Absyn_PtrLoc*_tmp62B=_cycalloc_atomic(sizeof(*_tmp62B));_tmp62B->ptr_loc=_tmp638;_tmp62B->rgn_loc=(unsigned int)(yyyylsp[2]).first_line;_tmp62B->zt_loc=(unsigned int)(yyyylsp[1]).first_line;_tmp62B;});ptrloc=_tmpC07;});{
# 2062
struct _tuple14 _tmp62C=({unsigned int _tmpC0B=_tmp638;union Cyc_Absyn_Constraint*_tmpC0A=_tmp637;union Cyc_Absyn_Constraint*_tmpC09=_tmp636;void*_tmpC08=Cyc_yyget_YY44(yyyyvsp[2]);Cyc_Parse_collapse_pointer_quals(_tmpC0B,_tmpC0A,_tmpC09,_tmpC08,Cyc_yyget_YY54(yyyyvsp[1]));});struct _tuple14 _tmp62D=_tmp62C;union Cyc_Absyn_Constraint*_tmp635;union Cyc_Absyn_Constraint*_tmp634;union Cyc_Absyn_Constraint*_tmp633;void*_tmp632;_LL69A: _tmp635=_tmp62D.f1;_tmp634=_tmp62D.f2;_tmp633=_tmp62D.f3;_tmp632=_tmp62D.f4;_LL69B:;
({struct Cyc_List_List*_tmpC10=({struct Cyc_List_List*_tmp62E=_region_malloc(yyr,sizeof(*_tmp62E));({void*_tmpC0F=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp62F=_region_malloc(yyr,sizeof(*_tmp62F));({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpC0E=({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmp630;_tmp630.tag=2U;({struct Cyc_Absyn_PtrAtts _tmpC0C=({struct Cyc_Absyn_PtrAtts _tmp631;_tmp631.rgn=_tmp632;_tmp631.nullable=_tmp635;_tmp631.bounds=_tmp634;_tmp631.zero_term=_tmp633;_tmp631.ptrloc=ptrloc;_tmp631;});_tmp630.f1=_tmpC0C;});({struct Cyc_Absyn_Tqual _tmpC0D=Cyc_yyget_YY23(yyyyvsp[4]);_tmp630.f2=_tmpC0D;});_tmp630;});_tmp62F[0]=_tmpC0E;});_tmp62F;});_tmp62E->hd=_tmpC0F;});_tmp62E->tl=ans;_tmp62E;});ans=_tmpC10;});
({union Cyc_YYSTYPE _tmpC11=Cyc_YY26(ans);yyval=_tmpC11;});
# 2066
goto _LL254;};};}case 192U: _LL3CF: _LL3D0:
# 2068
({union Cyc_YYSTYPE _tmpC12=Cyc_YY54(0);yyval=_tmpC12;});
goto _LL254;case 193U: _LL3D1: _LL3D2: {
# 2071
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2073
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2069 "parse.y"
({union Cyc_YYSTYPE _tmpC15=Cyc_YY54(({struct Cyc_List_List*_tmp639=_region_malloc(yyr,sizeof(*_tmp639));({void*_tmpC13=Cyc_yyget_YY53(yyyyvsp[0]);_tmp639->hd=_tmpC13;});({struct Cyc_List_List*_tmpC14=Cyc_yyget_YY54(yyyyvsp[1]);_tmp639->tl=_tmpC14;});_tmp639;}));yyval=_tmpC15;});
goto _LL254;}case 194U: _LL3D3: _LL3D4: {
# 2072
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2074
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2074 "parse.y"
({union Cyc_YYSTYPE _tmpC18=Cyc_YY53((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_tmp63A=_region_malloc(yyr,sizeof(*_tmp63A));({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct _tmpC17=({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct _tmp63B;_tmp63B.tag=0U;({struct Cyc_Absyn_Exp*_tmpC16=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp63B.f1=_tmpC16;});_tmp63B;});_tmp63A[0]=_tmpC17;});_tmp63A;}));yyval=_tmpC18;});
goto _LL254;}case 195U: _LL3D5: _LL3D6: {
# 2077
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2079
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2076 "parse.y"
({union Cyc_YYSTYPE _tmpC1B=Cyc_YY53((void*)({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_tmp63C=_region_malloc(yyr,sizeof(*_tmp63C));({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct _tmpC1A=({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct _tmp63D;_tmp63D.tag=1U;({void*_tmpC19=Cyc_yyget_YY44(yyyyvsp[2]);_tmp63D.f1=_tmpC19;});_tmp63D;});_tmp63C[0]=_tmpC1A;});_tmp63C;}));yyval=_tmpC1B;});
goto _LL254;}case 196U: _LL3D7: _LL3D8: {
# 2079
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2081
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2078 "parse.y"
({union Cyc_YYSTYPE _tmpC1D=Cyc_YY53((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_tmp63E=_region_malloc(yyr,sizeof(*_tmp63E));({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct _tmpC1C=({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct _tmp63F;_tmp63F.tag=2U;_tmp63F;});_tmp63E[0]=_tmpC1C;});_tmp63E;}));yyval=_tmpC1D;});
goto _LL254;}case 197U: _LL3D9: _LL3DA: {
# 2081
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2083
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2080 "parse.y"
({union Cyc_YYSTYPE _tmpC1F=Cyc_YY53((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_tmp640=_region_malloc(yyr,sizeof(*_tmp640));({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct _tmpC1E=({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct _tmp641;_tmp641.tag=3U;_tmp641;});_tmp640[0]=_tmpC1E;});_tmp640;}));yyval=_tmpC1F;});
goto _LL254;}case 198U: _LL3DB: _LL3DC: {
# 2083
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2082 "parse.y"
({union Cyc_YYSTYPE _tmpC21=Cyc_YY53((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp642=_region_malloc(yyr,sizeof(*_tmp642));({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct _tmpC20=({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct _tmp643;_tmp643.tag=4U;_tmp643;});_tmp642[0]=_tmpC20;});_tmp642;}));yyval=_tmpC21;});
goto _LL254;}case 199U: _LL3DD: _LL3DE: {
# 2085
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2087
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2084 "parse.y"
({union Cyc_YYSTYPE _tmpC23=Cyc_YY53((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp644=_region_malloc(yyr,sizeof(*_tmp644));({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct _tmpC22=({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct _tmp645;_tmp645.tag=5U;_tmp645;});_tmp644[0]=_tmpC22;});_tmp644;}));yyval=_tmpC23;});
goto _LL254;}case 200U: _LL3DF: _LL3E0: {
# 2087
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2089
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2086 "parse.y"
({union Cyc_YYSTYPE _tmpC25=Cyc_YY53((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_tmp646=_region_malloc(yyr,sizeof(*_tmp646));({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct _tmpC24=({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct _tmp647;_tmp647.tag=6U;_tmp647;});_tmp646[0]=_tmpC24;});_tmp646;}));yyval=_tmpC25;});
goto _LL254;}case 201U: _LL3E1: _LL3E2: {
# 2089
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2091
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2088 "parse.y"
({union Cyc_YYSTYPE _tmpC27=Cyc_YY53((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_tmp648=_region_malloc(yyr,sizeof(*_tmp648));({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct _tmpC26=({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct _tmp649;_tmp649.tag=7U;_tmp649;});_tmp648[0]=_tmpC26;});_tmp648;}));yyval=_tmpC27;});
goto _LL254;}case 202U: _LL3E3: _LL3E4: {
# 2091
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2093
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2094 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
({union Cyc_YYSTYPE _tmpC29=Cyc_YY1(({struct _tuple21*_tmp64A=_cycalloc(sizeof(*_tmp64A));_tmp64A->f1=loc;_tmp64A->f2=Cyc_Absyn_true_conref;({union Cyc_Absyn_Constraint*_tmpC28=Cyc_yyget_YY2(yyyyvsp[1]);_tmp64A->f3=_tmpC28;});_tmp64A;}));yyval=_tmpC29;});
# 2097
goto _LL254;}case 203U: _LL3E5: _LL3E6: {
# 2099
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2101
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2098 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
({union Cyc_YYSTYPE _tmpC2B=Cyc_YY1(({struct _tuple21*_tmp64B=_cycalloc(sizeof(*_tmp64B));_tmp64B->f1=loc;_tmp64B->f2=Cyc_Absyn_false_conref;({union Cyc_Absyn_Constraint*_tmpC2A=Cyc_yyget_YY2(yyyyvsp[1]);_tmp64B->f3=_tmpC2A;});_tmp64B;}));yyval=_tmpC2B;});
# 2101
goto _LL254;}case 204U: _LL3E7: _LL3E8: {
# 2103
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2105
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2102 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
({union Cyc_YYSTYPE _tmpC2C=Cyc_YY1(({struct _tuple21*_tmp64C=_cycalloc(sizeof(*_tmp64C));_tmp64C->f1=loc;_tmp64C->f2=Cyc_Absyn_true_conref;_tmp64C->f3=Cyc_Absyn_bounds_dyneither_conref;_tmp64C;}));yyval=_tmpC2C;});
# 2105
goto _LL254;}case 205U: _LL3E9: _LL3EA:
# 2107
({union Cyc_YYSTYPE _tmpC2D=Cyc_YY2(Cyc_Absyn_bounds_one_conref);yyval=_tmpC2D;});
goto _LL254;case 206U: _LL3EB: _LL3EC: {
# 2110
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2112
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2108 "parse.y"
({union Cyc_YYSTYPE _tmpC30=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp64D=_cycalloc(sizeof(*_tmp64D));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpC2F=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp64E;_tmp64E.tag=1U;({struct Cyc_Absyn_Exp*_tmpC2E=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp64E.f1=_tmpC2E;});_tmp64E;});_tmp64D[0]=_tmpC2F;});_tmp64D;})));yyval=_tmpC30;});
goto _LL254;}case 207U: _LL3ED: _LL3EE:
# 2111
({union Cyc_YYSTYPE _tmpC31=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());yyval=_tmpC31;});
goto _LL254;case 208U: _LL3EF: _LL3F0: {
# 2114
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2116
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2112 "parse.y"
({union Cyc_YYSTYPE _tmpC32=Cyc_YY51(Cyc_Absyn_true_conref);yyval=_tmpC32;});
goto _LL254;}case 209U: _LL3F1: _LL3F2: {
# 2115
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2117
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2113 "parse.y"
({union Cyc_YYSTYPE _tmpC33=Cyc_YY51(Cyc_Absyn_false_conref);yyval=_tmpC33;});
goto _LL254;}case 210U: _LL3F3: _LL3F4:
# 2116
({union Cyc_YYSTYPE _tmpC34=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));yyval=_tmpC34;});
goto _LL254;case 211U: _LL3F5: _LL3F6: {
# 2119
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2121
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2119 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];
goto _LL254;}case 212U: _LL3F7: _LL3F8: {
# 2122
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2124
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2120 "parse.y"
({union Cyc_YYSTYPE _tmpC35=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));yyval=_tmpC35;});
goto _LL254;}case 213U: _LL3F9: _LL3FA:
# 2123
({union Cyc_YYSTYPE _tmpC36=Cyc_YY23(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));yyval=_tmpC36;});
goto _LL254;case 214U: _LL3FB: _LL3FC: {
# 2126
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2128
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2125 "parse.y"
({union Cyc_YYSTYPE _tmpC38=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmpC37=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmpC37,Cyc_yyget_YY23(yyyyvsp[1]));}));yyval=_tmpC38;});
goto _LL254;}case 215U: _LL3FD: _LL3FE: {
# 2128
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2130
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2130 "parse.y"
({union Cyc_YYSTYPE _tmpC3C=Cyc_YY39(({struct _tuple26*_tmp64F=_cycalloc(sizeof(*_tmp64F));({struct Cyc_List_List*_tmpC39=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp64F->f1=_tmpC39;});_tmp64F->f2=0;_tmp64F->f3=0;({void*_tmpC3A=Cyc_yyget_YY49(yyyyvsp[1]);_tmp64F->f4=_tmpC3A;});({struct Cyc_List_List*_tmpC3B=Cyc_yyget_YY50(yyyyvsp[2]);_tmp64F->f5=_tmpC3B;});_tmp64F;}));yyval=_tmpC3C;});
goto _LL254;}case 216U: _LL3FF: _LL400: {
# 2133
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2135
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2132 "parse.y"
({union Cyc_YYSTYPE _tmpC40=Cyc_YY39(({struct _tuple26*_tmp650=_cycalloc(sizeof(*_tmp650));({struct Cyc_List_List*_tmpC3D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp650->f1=_tmpC3D;});_tmp650->f2=1;_tmp650->f3=0;({void*_tmpC3E=Cyc_yyget_YY49(yyyyvsp[3]);_tmp650->f4=_tmpC3E;});({struct Cyc_List_List*_tmpC3F=Cyc_yyget_YY50(yyyyvsp[4]);_tmp650->f5=_tmpC3F;});_tmp650;}));yyval=_tmpC40;});
goto _LL254;}case 217U: _LL401: _LL402: {
# 2135
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2137
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2135 "parse.y"
struct _tuple8*_tmp651=Cyc_yyget_YY37(yyyyvsp[2]);struct _tuple8*_tmp652=_tmp651;struct _dyneither_ptr*_tmp658;struct Cyc_Absyn_Tqual _tmp657;void*_tmp656;_LL69D: _tmp658=_tmp652->f1;_tmp657=_tmp652->f2;_tmp656=_tmp652->f3;_LL69E:;{
struct Cyc_Absyn_VarargInfo*_tmp653=({struct Cyc_Absyn_VarargInfo*_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655->name=_tmp658;_tmp655->tq=_tmp657;_tmp655->type=_tmp656;({int _tmpC41=Cyc_yyget_YY31(yyyyvsp[1]);_tmp655->inject=_tmpC41;});_tmp655;});
({union Cyc_YYSTYPE _tmpC44=Cyc_YY39(({struct _tuple26*_tmp654=_cycalloc(sizeof(*_tmp654));_tmp654->f1=0;_tmp654->f2=0;_tmp654->f3=_tmp653;({void*_tmpC42=Cyc_yyget_YY49(yyyyvsp[3]);_tmp654->f4=_tmpC42;});({struct Cyc_List_List*_tmpC43=Cyc_yyget_YY50(yyyyvsp[4]);_tmp654->f5=_tmpC43;});_tmp654;}));yyval=_tmpC44;});
# 2139
goto _LL254;};}case 218U: _LL403: _LL404: {
# 2141
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2143
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2141 "parse.y"
struct _tuple8*_tmp659=Cyc_yyget_YY37(yyyyvsp[4]);struct _tuple8*_tmp65A=_tmp659;struct _dyneither_ptr*_tmp660;struct Cyc_Absyn_Tqual _tmp65F;void*_tmp65E;_LL6A0: _tmp660=_tmp65A->f1;_tmp65F=_tmp65A->f2;_tmp65E=_tmp65A->f3;_LL6A1:;{
struct Cyc_Absyn_VarargInfo*_tmp65B=({struct Cyc_Absyn_VarargInfo*_tmp65D=_cycalloc(sizeof(*_tmp65D));_tmp65D->name=_tmp660;_tmp65D->tq=_tmp65F;_tmp65D->type=_tmp65E;({int _tmpC45=Cyc_yyget_YY31(yyyyvsp[3]);_tmp65D->inject=_tmpC45;});_tmp65D;});
({union Cyc_YYSTYPE _tmpC49=Cyc_YY39(({struct _tuple26*_tmp65C=_cycalloc(sizeof(*_tmp65C));({struct Cyc_List_List*_tmpC46=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp65C->f1=_tmpC46;});_tmp65C->f2=0;_tmp65C->f3=_tmp65B;({void*_tmpC47=Cyc_yyget_YY49(yyyyvsp[5]);_tmp65C->f4=_tmpC47;});({struct Cyc_List_List*_tmpC48=Cyc_yyget_YY50(yyyyvsp[6]);_tmp65C->f5=_tmpC48;});_tmp65C;}));yyval=_tmpC49;});
# 2145
goto _LL254;};}case 219U: _LL405: _LL406: {
# 2147
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2149
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2149 "parse.y"
({union Cyc_YYSTYPE _tmpC4C=Cyc_YY44(({struct _dyneither_ptr _tmpC4B=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Parse_id2type(_tmpC4B,(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp661=_cycalloc(sizeof(*_tmp661));({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpC4A=({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp662;_tmp662.tag=1U;_tmp662.f1=0;_tmp662;});_tmp661[0]=_tmpC4A;});_tmp661;}));}));yyval=_tmpC4C;});
goto _LL254;}case 220U: _LL407: _LL408: {
# 2152
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2154
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2150 "parse.y"
({union Cyc_YYSTYPE _tmpC4E=Cyc_YY44(({struct _dyneither_ptr _tmpC4D=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Parse_id2type(_tmpC4D,Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(yyyyvsp[2])));}));yyval=_tmpC4E;});
goto _LL254;}case 221U: _LL409: _LL40A:
# 2153
({union Cyc_YYSTYPE _tmpC4F=Cyc_YY49(0);yyval=_tmpC4F;});
goto _LL254;case 222U: _LL40B: _LL40C: {
# 2156
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2158
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2154 "parse.y"
({union Cyc_YYSTYPE _tmpC52=Cyc_YY49((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp663=_cycalloc(sizeof(*_tmp663));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC51=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp664;_tmp664.tag=24U;({struct Cyc_List_List*_tmpC50=Cyc_yyget_YY40(yyyyvsp[1]);_tmp664.f1=_tmpC50;});_tmp664;});_tmp663[0]=_tmpC51;});_tmp663;}));yyval=_tmpC52;});
goto _LL254;}case 223U: _LL40D: _LL40E:
# 2157
({union Cyc_YYSTYPE _tmpC53=Cyc_YY50(0);yyval=_tmpC53;});
goto _LL254;case 224U: _LL40F: _LL410: {
# 2160
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2162
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2159 "parse.y"
yyval=yyyyvsp[1];
goto _LL254;}case 225U: _LL411: _LL412: {
# 2162
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2164
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2167 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp665=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp66C=_cycalloc(sizeof(*_tmp66C));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpC54=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp66D;_tmp66D.tag=2U;_tmp66D.f1=0;_tmp66D.f2=& Cyc_Tcutil_trk;_tmp66D;});_tmp66C[0]=_tmpC54;});_tmp66C;});
struct _dyneither_ptr _tmp666=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp667=Cyc_Parse_id2type(_tmp666,(void*)_tmp665);
({union Cyc_YYSTYPE _tmpC59=Cyc_YY50(({struct Cyc_List_List*_tmp668=_cycalloc(sizeof(*_tmp668));({struct _tuple29*_tmpC58=({struct _tuple29*_tmp669=_cycalloc(sizeof(*_tmp669));({void*_tmpC57=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp66A=_cycalloc(sizeof(*_tmp66A));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC56=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp66B;_tmp66B.tag=24U;({struct Cyc_List_List*_tmpC55=Cyc_yyget_YY40(yyyyvsp[0]);_tmp66B.f1=_tmpC55;});_tmp66B;});_tmp66A[0]=_tmpC56;});_tmp66A;});_tmp669->f1=_tmpC57;});_tmp669->f2=_tmp667;_tmp669;});_tmp668->hd=_tmpC58;});_tmp668->tl=0;_tmp668;}));yyval=_tmpC59;});
# 2172
goto _LL254;}case 226U: _LL413: _LL414: {
# 2174
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2176
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2174 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp66E=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp675=_cycalloc(sizeof(*_tmp675));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpC5A=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp676;_tmp676.tag=2U;_tmp676.f1=0;_tmp676.f2=& Cyc_Tcutil_trk;_tmp676;});_tmp675[0]=_tmpC5A;});_tmp675;});
struct _dyneither_ptr _tmp66F=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp670=Cyc_Parse_id2type(_tmp66F,(void*)_tmp66E);
({union Cyc_YYSTYPE _tmpC60=Cyc_YY50(({struct Cyc_List_List*_tmp671=_cycalloc(sizeof(*_tmp671));({struct _tuple29*_tmpC5E=({struct _tuple29*_tmp672=_cycalloc(sizeof(*_tmp672));({void*_tmpC5D=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp673=_cycalloc(sizeof(*_tmp673));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC5C=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp674;_tmp674.tag=24U;({struct Cyc_List_List*_tmpC5B=Cyc_yyget_YY40(yyyyvsp[0]);_tmp674.f1=_tmpC5B;});_tmp674;});_tmp673[0]=_tmpC5C;});_tmp673;});_tmp672->f1=_tmpC5D;});_tmp672->f2=_tmp670;_tmp672;});_tmp671->hd=_tmpC5E;});({struct Cyc_List_List*_tmpC5F=Cyc_yyget_YY50(yyyyvsp[4]);_tmp671->tl=_tmpC5F;});_tmp671;}));yyval=_tmpC60;});
# 2179
goto _LL254;}case 227U: _LL415: _LL416:
# 2181
({union Cyc_YYSTYPE _tmpC61=Cyc_YY31(0);yyval=_tmpC61;});
goto _LL254;case 228U: _LL417: _LL418: {
# 2184
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2186
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2185 "parse.y"
if(({struct _dyneither_ptr _tmpC62=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_zstrcmp(_tmpC62,({const char*_tmp677="inject";_tag_dyneither(_tmp677,sizeof(char),7U);}));})!= 0)
({void*_tmp678=0U;({unsigned int _tmpC64=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpC63=({const char*_tmp679="missing type in function declaration";_tag_dyneither(_tmp679,sizeof(char),37U);});Cyc_Warn_err(_tmpC64,_tmpC63,_tag_dyneither(_tmp678,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpC65=Cyc_YY31(1);yyval=_tmpC65;});
# 2189
goto _LL254;}case 229U: _LL419: _LL41A: {
# 2191
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2193
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2192 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 230U: _LL41B: _LL41C: {
# 2195
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2197
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2193 "parse.y"
({union Cyc_YYSTYPE _tmpC67=Cyc_YY40(({struct Cyc_List_List*_tmpC66=Cyc_yyget_YY40(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpC66,Cyc_yyget_YY40(yyyyvsp[2]));}));yyval=_tmpC67;});
goto _LL254;}case 231U: _LL41D: _LL41E: {
# 2196
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2198
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2197 "parse.y"
({union Cyc_YYSTYPE _tmpC68=Cyc_YY40(0);yyval=_tmpC68;});
goto _LL254;}case 232U: _LL41F: _LL420: {
# 2200
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2202
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2198 "parse.y"
yyval=yyyyvsp[1];
goto _LL254;}case 233U: _LL421: _LL422: {
# 2201
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2203
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2200 "parse.y"
({union Cyc_YYSTYPE _tmpC6C=Cyc_YY40(({struct Cyc_List_List*_tmp67A=_cycalloc(sizeof(*_tmp67A));({void*_tmpC6B=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp67B=_cycalloc(sizeof(*_tmp67B));({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpC6A=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp67C;_tmp67C.tag=25U;({void*_tmpC69=Cyc_yyget_YY44(yyyyvsp[2]);_tmp67C.f1=_tmpC69;});_tmp67C;});_tmp67B[0]=_tmpC6A;});_tmp67B;});_tmp67A->hd=_tmpC6B;});_tmp67A->tl=0;_tmp67A;}));yyval=_tmpC6C;});
goto _LL254;}case 234U: _LL423: _LL424: {
# 2203
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2205
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2202 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
({union Cyc_YYSTYPE _tmpC6E=Cyc_YY40(({struct Cyc_List_List*_tmp67D=_cycalloc(sizeof(*_tmp67D));({void*_tmpC6D=Cyc_yyget_YY44(yyyyvsp[0]);_tmp67D->hd=_tmpC6D;});_tmp67D->tl=0;_tmp67D;}));yyval=_tmpC6E;});
# 2205
goto _LL254;}case 235U: _LL425: _LL426: {
# 2207
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2209
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2222 "parse.y"
({union Cyc_YYSTYPE _tmpC73=Cyc_YY40(({struct Cyc_List_List*_tmp67E=_cycalloc(sizeof(*_tmp67E));({void*_tmpC72=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp67F=_cycalloc(sizeof(*_tmp67F));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC71=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp680;_tmp680.tag=23U;({void*_tmpC70=({struct _tuple8*_tmpC6F=Cyc_yyget_YY37(yyyyvsp[0]);Cyc_Parse_type_name_to_type(_tmpC6F,(unsigned int)(yyyylsp[0]).first_line);});_tmp680.f1=_tmpC70;});_tmp680;});_tmp67F[0]=_tmpC71;});_tmp67F;});_tmp67E->hd=_tmpC72;});_tmp67E->tl=0;_tmp67E;}));yyval=_tmpC73;});
goto _LL254;}case 236U: _LL427: _LL428: {
# 2225
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2227
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2224 "parse.y"
({union Cyc_YYSTYPE _tmpC79=Cyc_YY40(({struct Cyc_List_List*_tmp681=_cycalloc(sizeof(*_tmp681));({void*_tmpC77=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp682=_cycalloc(sizeof(*_tmp682));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC76=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp683;_tmp683.tag=23U;({void*_tmpC75=({struct _tuple8*_tmpC74=Cyc_yyget_YY37(yyyyvsp[0]);Cyc_Parse_type_name_to_type(_tmpC74,(unsigned int)(yyyylsp[0]).first_line);});_tmp683.f1=_tmpC75;});_tmp683;});_tmp682[0]=_tmpC76;});_tmp682;});_tmp681->hd=_tmpC77;});({struct Cyc_List_List*_tmpC78=Cyc_yyget_YY40(yyyyvsp[2]);_tmp681->tl=_tmpC78;});_tmp681;}));yyval=_tmpC79;});
goto _LL254;}case 237U: _LL429: _LL42A: {
# 2227
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2229
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpC7B=Cyc_YY38(({struct Cyc_List_List*_tmp684=_cycalloc(sizeof(*_tmp684));({struct _tuple8*_tmpC7A=Cyc_yyget_YY37(yyyyvsp[0]);_tmp684->hd=_tmpC7A;});_tmp684->tl=0;_tmp684;}));yyval=_tmpC7B;});
goto _LL254;}case 238U: _LL42B: _LL42C: {
# 2233
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2235
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2232 "parse.y"
({union Cyc_YYSTYPE _tmpC7E=Cyc_YY38(({struct Cyc_List_List*_tmp685=_cycalloc(sizeof(*_tmp685));({struct _tuple8*_tmpC7C=Cyc_yyget_YY37(yyyyvsp[2]);_tmp685->hd=_tmpC7C;});({struct Cyc_List_List*_tmpC7D=Cyc_yyget_YY38(yyyyvsp[0]);_tmp685->tl=_tmpC7D;});_tmp685;}));yyval=_tmpC7E;});
goto _LL254;}case 239U: _LL42D: _LL42E: {
# 2235
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2237
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
struct _tuple25 _tmp686=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp687=_tmp686;struct Cyc_Absyn_Tqual _tmp69D;struct Cyc_Parse_Type_specifier _tmp69C;struct Cyc_List_List*_tmp69B;_LL6A3: _tmp69D=_tmp687.f1;_tmp69C=_tmp687.f2;_tmp69B=_tmp687.f3;_LL6A4:;
if(_tmp69D.loc == 0)_tmp69D.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct Cyc_Parse_Declarator _tmp688=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Parse_Declarator _tmp689=_tmp688;struct _tuple0*_tmp69A;unsigned int _tmp699;struct Cyc_List_List*_tmp698;_LL6A6: _tmp69A=_tmp689.id;_tmp699=_tmp689.varloc;_tmp698=_tmp689.tms;_LL6A7:;{
void*_tmp68A=Cyc_Parse_speclist2typ(_tmp69C,(unsigned int)(yyyylsp[0]).first_line);
struct _tuple13 _tmp68B=Cyc_Parse_apply_tms(_tmp69D,_tmp68A,_tmp69B,_tmp698);struct _tuple13 _tmp68C=_tmp68B;struct Cyc_Absyn_Tqual _tmp697;void*_tmp696;struct Cyc_List_List*_tmp695;struct Cyc_List_List*_tmp694;_LL6A9: _tmp697=_tmp68C.f1;_tmp696=_tmp68C.f2;_tmp695=_tmp68C.f3;_tmp694=_tmp68C.f4;_LL6AA:;
if(_tmp695 != 0)
({void*_tmp68D=0U;({unsigned int _tmpC80=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpC7F=({const char*_tmp68E="parameter with bad type params";_tag_dyneither(_tmp68E,sizeof(char),31U);});Cyc_Warn_err(_tmpC80,_tmpC7F,_tag_dyneither(_tmp68D,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp69A))
({void*_tmp68F=0U;({unsigned int _tmpC82=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpC81=({const char*_tmp690="parameter cannot be qualified with a namespace";_tag_dyneither(_tmp690,sizeof(char),47U);});Cyc_Warn_err(_tmpC82,_tmpC81,_tag_dyneither(_tmp68F,sizeof(void*),0U));});});{
struct _dyneither_ptr*idopt=(*_tmp69A).f2;
if(_tmp694 != 0)
({void*_tmp691=0U;({unsigned int _tmpC84=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpC83=({const char*_tmp692="extra attributes on parameter, ignoring";_tag_dyneither(_tmp692,sizeof(char),40U);});Cyc_Warn_warn(_tmpC84,_tmpC83,_tag_dyneither(_tmp691,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpC85=Cyc_YY37(({struct _tuple8*_tmp693=_cycalloc(sizeof(*_tmp693));_tmp693->f1=idopt;_tmp693->f2=_tmp697;_tmp693->f3=_tmp696;_tmp693;}));yyval=_tmpC85;});
# 2252
goto _LL254;};};};}case 240U: _LL42F: _LL430: {
# 2254
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2256
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2253 "parse.y"
struct _tuple25 _tmp69E=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp69F=_tmp69E;struct Cyc_Absyn_Tqual _tmp6A6;struct Cyc_Parse_Type_specifier _tmp6A5;struct Cyc_List_List*_tmp6A4;_LL6AC: _tmp6A6=_tmp69F.f1;_tmp6A5=_tmp69F.f2;_tmp6A4=_tmp69F.f3;_LL6AD:;
if(_tmp6A6.loc == 0)_tmp6A6.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp6A0=Cyc_Parse_speclist2typ(_tmp6A5,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6A4 != 0)
({void*_tmp6A1=0U;({unsigned int _tmpC87=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpC86=({const char*_tmp6A2="bad attributes on parameter, ignoring";_tag_dyneither(_tmp6A2,sizeof(char),38U);});Cyc_Warn_warn(_tmpC87,_tmpC86,_tag_dyneither(_tmp6A1,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpC88=Cyc_YY37(({struct _tuple8*_tmp6A3=_cycalloc(sizeof(*_tmp6A3));_tmp6A3->f1=0;_tmp6A3->f2=_tmp6A6;_tmp6A3->f3=_tmp6A0;_tmp6A3;}));yyval=_tmpC88;});
# 2260
goto _LL254;};}case 241U: _LL431: _LL432: {
# 2262
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2264
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2261 "parse.y"
struct _tuple25 _tmp6A7=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp6A8=_tmp6A7;struct Cyc_Absyn_Tqual _tmp6B8;struct Cyc_Parse_Type_specifier _tmp6B7;struct Cyc_List_List*_tmp6B6;_LL6AF: _tmp6B8=_tmp6A8.f1;_tmp6B7=_tmp6A8.f2;_tmp6B6=_tmp6A8.f3;_LL6B0:;
if(_tmp6B8.loc == 0)_tmp6B8.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp6A9=Cyc_Parse_speclist2typ(_tmp6B7,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6AA=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp6AB=Cyc_Parse_apply_tms(_tmp6B8,_tmp6A9,_tmp6B6,_tmp6AA);struct _tuple13 _tmp6AC=_tmp6AB;struct Cyc_Absyn_Tqual _tmp6B5;void*_tmp6B4;struct Cyc_List_List*_tmp6B3;struct Cyc_List_List*_tmp6B2;_LL6B2: _tmp6B5=_tmp6AC.f1;_tmp6B4=_tmp6AC.f2;_tmp6B3=_tmp6AC.f3;_tmp6B2=_tmp6AC.f4;_LL6B3:;
if(_tmp6B3 != 0)
({void*_tmp6AD=0U;({unsigned int _tmpC8A=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpC89=({const char*_tmp6AE="bad type parameters on formal argument, ignoring";_tag_dyneither(_tmp6AE,sizeof(char),49U);});Cyc_Warn_warn(_tmpC8A,_tmpC89,_tag_dyneither(_tmp6AD,sizeof(void*),0U));});});
# 2269
if(_tmp6B2 != 0)
({void*_tmp6AF=0U;({unsigned int _tmpC8C=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpC8B=({const char*_tmp6B0="bad attributes on parameter, ignoring";_tag_dyneither(_tmp6B0,sizeof(char),38U);});Cyc_Warn_warn(_tmpC8C,_tmpC8B,_tag_dyneither(_tmp6AF,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpC8D=Cyc_YY37(({struct _tuple8*_tmp6B1=_cycalloc(sizeof(*_tmp6B1));_tmp6B1->f1=0;_tmp6B1->f2=_tmp6B5;_tmp6B1->f3=_tmp6B4;_tmp6B1;}));yyval=_tmpC8D;});
# 2273
goto _LL254;};}case 242U: _LL433: _LL434: {
# 2275
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2277
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2277 "parse.y"
({union Cyc_YYSTYPE _tmpC8E=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyyyvsp[0])));yyval=_tmpC8E;});
goto _LL254;}case 243U: _LL435: _LL436: {
# 2280
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2282
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2281 "parse.y"
({union Cyc_YYSTYPE _tmpC91=Cyc_YY36(({struct Cyc_List_List*_tmp6B9=_cycalloc(sizeof(*_tmp6B9));({struct _dyneither_ptr*_tmpC90=({struct _dyneither_ptr*_tmp6BA=_cycalloc(sizeof(*_tmp6BA));({struct _dyneither_ptr _tmpC8F=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp6BA[0]=_tmpC8F;});_tmp6BA;});_tmp6B9->hd=_tmpC90;});_tmp6B9->tl=0;_tmp6B9;}));yyval=_tmpC91;});
goto _LL254;}case 244U: _LL437: _LL438: {
# 2284
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2286
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2283 "parse.y"
({union Cyc_YYSTYPE _tmpC95=Cyc_YY36(({struct Cyc_List_List*_tmp6BB=_cycalloc(sizeof(*_tmp6BB));({struct _dyneither_ptr*_tmpC93=({struct _dyneither_ptr*_tmp6BC=_cycalloc(sizeof(*_tmp6BC));({struct _dyneither_ptr _tmpC92=Cyc_yyget_String_tok(yyyyvsp[2]);_tmp6BC[0]=_tmpC92;});_tmp6BC;});_tmp6BB->hd=_tmpC93;});({struct Cyc_List_List*_tmpC94=Cyc_yyget_YY36(yyyyvsp[0]);_tmp6BB->tl=_tmpC94;});_tmp6BB;}));yyval=_tmpC95;});
goto _LL254;}case 245U: _LL439: _LL43A: {
# 2286
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2288
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2287 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 246U: _LL43B: _LL43C: {
# 2290
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2292
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2288 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 247U: _LL43D: _LL43E: {
# 2291
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2293
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2293 "parse.y"
({union Cyc_YYSTYPE _tmpC98=Cyc_Exp_tok(({void*_tmpC97=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp6BD=_cycalloc(sizeof(*_tmp6BD));({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpC96=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp6BE;_tmp6BE.tag=36U;_tmp6BE.f1=0;_tmp6BE.f2=0;_tmp6BE;});_tmp6BD[0]=_tmpC96;});_tmp6BD;});Cyc_Absyn_new_exp(_tmpC97,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC98;});
goto _LL254;}case 248U: _LL43F: _LL440: {
# 2296
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2298
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2295 "parse.y"
({union Cyc_YYSTYPE _tmpC9C=Cyc_Exp_tok(({void*_tmpC9B=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp6BF=_cycalloc(sizeof(*_tmp6BF));({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpC9A=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp6C0;_tmp6C0.tag=36U;_tmp6C0.f1=0;({struct Cyc_List_List*_tmpC99=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1]));_tmp6C0.f2=_tmpC99;});_tmp6C0;});_tmp6BF[0]=_tmpC9A;});_tmp6BF;});Cyc_Absyn_new_exp(_tmpC9B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC9C;});
goto _LL254;}case 249U: _LL441: _LL442: {
# 2298
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2300
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2297 "parse.y"
({union Cyc_YYSTYPE _tmpCA0=Cyc_Exp_tok(({void*_tmpC9F=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp6C1=_cycalloc(sizeof(*_tmp6C1));({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpC9E=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp6C2;_tmp6C2.tag=36U;_tmp6C2.f1=0;({struct Cyc_List_List*_tmpC9D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1]));_tmp6C2.f2=_tmpC9D;});_tmp6C2;});_tmp6C1[0]=_tmpC9E;});_tmp6C1;});Cyc_Absyn_new_exp(_tmpC9F,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCA0;});
goto _LL254;}case 250U: _LL443: _LL444: {
# 2300
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2302
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2299 "parse.y"
struct Cyc_Absyn_Vardecl*_tmp6C3=({unsigned int _tmpCA5=(unsigned int)(yyyylsp[2]).first_line;struct _tuple0*_tmpCA4=({struct _tuple0*_tmp6C6=_cycalloc(sizeof(*_tmp6C6));_tmp6C6->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmpCA2=({struct _dyneither_ptr*_tmp6C7=_cycalloc(sizeof(*_tmp6C7));({struct _dyneither_ptr _tmpCA1=Cyc_yyget_String_tok(yyyyvsp[2]);_tmp6C7[0]=_tmpCA1;});_tmp6C7;});_tmp6C6->f2=_tmpCA2;});_tmp6C6;});void*_tmpCA3=Cyc_Absyn_uint_typ;Cyc_Absyn_new_vardecl(_tmpCA5,_tmpCA4,_tmpCA3,
Cyc_Absyn_uint_exp(0U,(unsigned int)(yyyylsp[2]).first_line));});
# 2302
(_tmp6C3->tq).real_const=1;
({union Cyc_YYSTYPE _tmpCAA=Cyc_Exp_tok(({void*_tmpCA9=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp6C4=_cycalloc(sizeof(*_tmp6C4));({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpCA8=({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmp6C5;_tmp6C5.tag=27U;_tmp6C5.f1=_tmp6C3;({struct Cyc_Absyn_Exp*_tmpCA6=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp6C5.f2=_tmpCA6;});({struct Cyc_Absyn_Exp*_tmpCA7=Cyc_yyget_Exp_tok(yyyyvsp[6]);_tmp6C5.f3=_tmpCA7;});_tmp6C5.f4=0;_tmp6C5;});_tmp6C4[0]=_tmpCA8;});_tmp6C4;});Cyc_Absyn_new_exp(_tmpCA9,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCAA;});
# 2305
goto _LL254;}case 251U: _LL445: _LL446: {
# 2307
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2309
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2307 "parse.y"
void*_tmp6C8=({struct _tuple8*_tmpCAB=Cyc_yyget_YY37(yyyyvsp[6]);Cyc_Parse_type_name_to_type(_tmpCAB,(unsigned int)(yyyylsp[6]).first_line);});
({union Cyc_YYSTYPE _tmpCAF=Cyc_Exp_tok(({void*_tmpCAE=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp6C9=_cycalloc(sizeof(*_tmp6C9));({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpCAD=({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmp6CA;_tmp6CA.tag=28U;({struct Cyc_Absyn_Exp*_tmpCAC=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp6CA.f1=_tmpCAC;});_tmp6CA.f2=_tmp6C8;_tmp6CA.f3=0;_tmp6CA;});_tmp6C9[0]=_tmpCAD;});_tmp6C9;});Cyc_Absyn_new_exp(_tmpCAE,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCAF;});
# 2310
goto _LL254;}case 252U: _LL447: _LL448: {
# 2312
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2314
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpCB2=Cyc_YY5(({struct Cyc_List_List*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));({struct _tuple30*_tmpCB1=({struct _tuple30*_tmp6CC=_cycalloc(sizeof(*_tmp6CC));_tmp6CC->f1=0;({struct Cyc_Absyn_Exp*_tmpCB0=Cyc_yyget_Exp_tok(yyyyvsp[0]);_tmp6CC->f2=_tmpCB0;});_tmp6CC;});_tmp6CB->hd=_tmpCB1;});_tmp6CB->tl=0;_tmp6CB;}));yyval=_tmpCB2;});
goto _LL254;}case 253U: _LL449: _LL44A: {
# 2318
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2320
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2317 "parse.y"
({union Cyc_YYSTYPE _tmpCB6=Cyc_YY5(({struct Cyc_List_List*_tmp6CD=_cycalloc(sizeof(*_tmp6CD));({struct _tuple30*_tmpCB5=({struct _tuple30*_tmp6CE=_cycalloc(sizeof(*_tmp6CE));({struct Cyc_List_List*_tmpCB3=Cyc_yyget_YY41(yyyyvsp[0]);_tmp6CE->f1=_tmpCB3;});({struct Cyc_Absyn_Exp*_tmpCB4=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp6CE->f2=_tmpCB4;});_tmp6CE;});_tmp6CD->hd=_tmpCB5;});_tmp6CD->tl=0;_tmp6CD;}));yyval=_tmpCB6;});
goto _LL254;}case 254U: _LL44B: _LL44C: {
# 2320
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2322
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2319 "parse.y"
({union Cyc_YYSTYPE _tmpCBA=Cyc_YY5(({struct Cyc_List_List*_tmp6CF=_cycalloc(sizeof(*_tmp6CF));({struct _tuple30*_tmpCB8=({struct _tuple30*_tmp6D0=_cycalloc(sizeof(*_tmp6D0));_tmp6D0->f1=0;({struct Cyc_Absyn_Exp*_tmpCB7=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp6D0->f2=_tmpCB7;});_tmp6D0;});_tmp6CF->hd=_tmpCB8;});({struct Cyc_List_List*_tmpCB9=Cyc_yyget_YY5(yyyyvsp[0]);_tmp6CF->tl=_tmpCB9;});_tmp6CF;}));yyval=_tmpCBA;});
goto _LL254;}case 255U: _LL44D: _LL44E: {
# 2322
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2324
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2321 "parse.y"
({union Cyc_YYSTYPE _tmpCBF=Cyc_YY5(({struct Cyc_List_List*_tmp6D1=_cycalloc(sizeof(*_tmp6D1));({struct _tuple30*_tmpCBD=({struct _tuple30*_tmp6D2=_cycalloc(sizeof(*_tmp6D2));({struct Cyc_List_List*_tmpCBB=Cyc_yyget_YY41(yyyyvsp[2]);_tmp6D2->f1=_tmpCBB;});({struct Cyc_Absyn_Exp*_tmpCBC=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp6D2->f2=_tmpCBC;});_tmp6D2;});_tmp6D1->hd=_tmpCBD;});({struct Cyc_List_List*_tmpCBE=Cyc_yyget_YY5(yyyyvsp[0]);_tmp6D1->tl=_tmpCBE;});_tmp6D1;}));yyval=_tmpCBF;});
goto _LL254;}case 256U: _LL44F: _LL450: {
# 2324
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2326
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2325 "parse.y"
({union Cyc_YYSTYPE _tmpCC0=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])));yyval=_tmpCC0;});
goto _LL254;}case 257U: _LL451: _LL452: {
# 2328
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2330
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2326 "parse.y"
({union Cyc_YYSTYPE _tmpCC5=Cyc_YY41(({struct Cyc_List_List*_tmp6D3=_cycalloc(sizeof(*_tmp6D3));({void*_tmpCC4=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp6D4=_cycalloc(sizeof(*_tmp6D4));({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpCC3=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp6D5;_tmp6D5.tag=1U;({struct _dyneither_ptr*_tmpCC2=({struct _dyneither_ptr*_tmp6D6=_cycalloc(sizeof(*_tmp6D6));({struct _dyneither_ptr _tmpCC1=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp6D6[0]=_tmpCC1;});_tmp6D6;});_tmp6D5.f1=_tmpCC2;});_tmp6D5;});_tmp6D4[0]=_tmpCC3;});_tmp6D4;});_tmp6D3->hd=_tmpCC4;});_tmp6D3->tl=0;_tmp6D3;}));yyval=_tmpCC5;});
goto _LL254;}case 258U: _LL453: _LL454: {
# 2329
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2331
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2331 "parse.y"
({union Cyc_YYSTYPE _tmpCC7=Cyc_YY41(({struct Cyc_List_List*_tmp6D7=_cycalloc(sizeof(*_tmp6D7));({void*_tmpCC6=Cyc_yyget_YY42(yyyyvsp[0]);_tmp6D7->hd=_tmpCC6;});_tmp6D7->tl=0;_tmp6D7;}));yyval=_tmpCC7;});
goto _LL254;}case 259U: _LL455: _LL456: {
# 2334
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2336
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2332 "parse.y"
({union Cyc_YYSTYPE _tmpCCA=Cyc_YY41(({struct Cyc_List_List*_tmp6D8=_cycalloc(sizeof(*_tmp6D8));({void*_tmpCC8=Cyc_yyget_YY42(yyyyvsp[1]);_tmp6D8->hd=_tmpCC8;});({struct Cyc_List_List*_tmpCC9=Cyc_yyget_YY41(yyyyvsp[0]);_tmp6D8->tl=_tmpCC9;});_tmp6D8;}));yyval=_tmpCCA;});
goto _LL254;}case 260U: _LL457: _LL458: {
# 2335
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2337
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2336 "parse.y"
({union Cyc_YYSTYPE _tmpCCD=Cyc_YY42((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp6D9=_cycalloc(sizeof(*_tmp6D9));({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpCCC=({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmp6DA;_tmp6DA.tag=0U;({struct Cyc_Absyn_Exp*_tmpCCB=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp6DA.f1=_tmpCCB;});_tmp6DA;});_tmp6D9[0]=_tmpCCC;});_tmp6D9;}));yyval=_tmpCCD;});
goto _LL254;}case 261U: _LL459: _LL45A: {
# 2339
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2341
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2337 "parse.y"
({union Cyc_YYSTYPE _tmpCD1=Cyc_YY42((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp6DB=_cycalloc(sizeof(*_tmp6DB));({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpCD0=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp6DC;_tmp6DC.tag=1U;({struct _dyneither_ptr*_tmpCCF=({struct _dyneither_ptr*_tmp6DD=_cycalloc(sizeof(*_tmp6DD));({struct _dyneither_ptr _tmpCCE=Cyc_yyget_String_tok(yyyyvsp[1]);_tmp6DD[0]=_tmpCCE;});_tmp6DD;});_tmp6DC.f1=_tmpCCF;});_tmp6DC;});_tmp6DB[0]=_tmpCD0;});_tmp6DB;}));yyval=_tmpCD1;});
goto _LL254;}case 262U: _LL45B: _LL45C: {
# 2340
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2342
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2342 "parse.y"
struct _tuple25 _tmp6DE=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp6DF=_tmp6DE;struct Cyc_Absyn_Tqual _tmp6E6;struct Cyc_Parse_Type_specifier _tmp6E5;struct Cyc_List_List*_tmp6E4;_LL6B5: _tmp6E6=_tmp6DF.f1;_tmp6E5=_tmp6DF.f2;_tmp6E4=_tmp6DF.f3;_LL6B6:;{
void*_tmp6E0=Cyc_Parse_speclist2typ(_tmp6E5,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6E4 != 0)
({void*_tmp6E1=0U;({unsigned int _tmpCD3=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpCD2=({const char*_tmp6E2="ignoring attributes in type";_tag_dyneither(_tmp6E2,sizeof(char),28U);});Cyc_Warn_warn(_tmpCD3,_tmpCD2,_tag_dyneither(_tmp6E1,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpCD4=Cyc_YY37(({struct _tuple8*_tmp6E3=_cycalloc(sizeof(*_tmp6E3));_tmp6E3->f1=0;_tmp6E3->f2=_tmp6E6;_tmp6E3->f3=_tmp6E0;_tmp6E3;}));yyval=_tmpCD4;});
# 2348
goto _LL254;};}case 263U: _LL45D: _LL45E: {
# 2350
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2352
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2349 "parse.y"
struct _tuple25 _tmp6E7=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp6E8=_tmp6E7;struct Cyc_Absyn_Tqual _tmp6F3;struct Cyc_Parse_Type_specifier _tmp6F2;struct Cyc_List_List*_tmp6F1;_LL6B8: _tmp6F3=_tmp6E8.f1;_tmp6F2=_tmp6E8.f2;_tmp6F1=_tmp6E8.f3;_LL6B9:;{
void*_tmp6E9=Cyc_Parse_speclist2typ(_tmp6F2,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6EA=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp6EB=Cyc_Parse_apply_tms(_tmp6F3,_tmp6E9,_tmp6F1,_tmp6EA);
if(_tmp6EB.f3 != 0)
# 2355
({void*_tmp6EC=0U;({unsigned int _tmpCD6=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpCD5=({const char*_tmp6ED="bad type params, ignoring";_tag_dyneither(_tmp6ED,sizeof(char),26U);});Cyc_Warn_warn(_tmpCD6,_tmpCD5,_tag_dyneither(_tmp6EC,sizeof(void*),0U));});});
if(_tmp6EB.f4 != 0)
({void*_tmp6EE=0U;({unsigned int _tmpCD8=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpCD7=({const char*_tmp6EF="bad specifiers, ignoring";_tag_dyneither(_tmp6EF,sizeof(char),25U);});Cyc_Warn_warn(_tmpCD8,_tmpCD7,_tag_dyneither(_tmp6EE,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpCD9=Cyc_YY37(({struct _tuple8*_tmp6F0=_cycalloc(sizeof(*_tmp6F0));_tmp6F0->f1=0;_tmp6F0->f2=_tmp6EB.f1;_tmp6F0->f3=_tmp6EB.f2;_tmp6F0;}));yyval=_tmpCD9;});
# 2360
goto _LL254;};}case 264U: _LL45F: _LL460: {
# 2362
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2364
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2363 "parse.y"
({union Cyc_YYSTYPE _tmpCDB=Cyc_YY44(({struct _tuple8*_tmpCDA=Cyc_yyget_YY37(yyyyvsp[0]);Cyc_Parse_type_name_to_type(_tmpCDA,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCDB;});
goto _LL254;}case 265U: _LL461: _LL462: {
# 2366
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2368
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2364 "parse.y"
({union Cyc_YYSTYPE _tmpCDD=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6F4=_cycalloc(sizeof(*_tmp6F4));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCDC=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp6F5;_tmp6F5.tag=24U;_tmp6F5.f1=0;_tmp6F5;});_tmp6F4[0]=_tmpCDC;});_tmp6F4;}));yyval=_tmpCDD;});
goto _LL254;}case 266U: _LL463: _LL464: {
# 2367
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2369
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2365 "parse.y"
({union Cyc_YYSTYPE _tmpCE0=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6F6=_cycalloc(sizeof(*_tmp6F6));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCDF=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp6F7;_tmp6F7.tag=24U;({struct Cyc_List_List*_tmpCDE=Cyc_yyget_YY40(yyyyvsp[1]);_tmp6F7.f1=_tmpCDE;});_tmp6F7;});_tmp6F6[0]=_tmpCDF;});_tmp6F6;}));yyval=_tmpCE0;});
goto _LL254;}case 267U: _LL465: _LL466: {
# 2368
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2370
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2366 "parse.y"
({union Cyc_YYSTYPE _tmpCE3=Cyc_YY44((void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp6F8=_cycalloc(sizeof(*_tmp6F8));({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpCE2=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp6F9;_tmp6F9.tag=25U;({void*_tmpCE1=Cyc_yyget_YY44(yyyyvsp[2]);_tmp6F9.f1=_tmpCE1;});_tmp6F9;});_tmp6F8[0]=_tmpCE2;});_tmp6F8;}));yyval=_tmpCE3;});
goto _LL254;}case 268U: _LL467: _LL468: {
# 2369
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2371
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2367 "parse.y"
({union Cyc_YYSTYPE _tmpCE8=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6FA=_cycalloc(sizeof(*_tmp6FA));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCE7=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp6FB;_tmp6FB.tag=24U;({struct Cyc_List_List*_tmpCE6=({struct Cyc_List_List*_tmp6FC=_cycalloc(sizeof(*_tmp6FC));({void*_tmpCE4=Cyc_yyget_YY44(yyyyvsp[0]);_tmp6FC->hd=_tmpCE4;});({struct Cyc_List_List*_tmpCE5=Cyc_yyget_YY40(yyyyvsp[2]);_tmp6FC->tl=_tmpCE5;});_tmp6FC;});_tmp6FB.f1=_tmpCE6;});_tmp6FB;});_tmp6FA[0]=_tmpCE7;});_tmp6FA;}));yyval=_tmpCE8;});
goto _LL254;}case 269U: _LL469: _LL46A: {
# 2370
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2372
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpCEA=Cyc_YY40(({struct Cyc_List_List*_tmp6FD=_cycalloc(sizeof(*_tmp6FD));({void*_tmpCE9=Cyc_yyget_YY44(yyyyvsp[0]);_tmp6FD->hd=_tmpCE9;});_tmp6FD->tl=0;_tmp6FD;}));yyval=_tmpCEA;});
goto _LL254;}case 270U: _LL46B: _LL46C: {
# 2376
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2378
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2374 "parse.y"
({union Cyc_YYSTYPE _tmpCED=Cyc_YY40(({struct Cyc_List_List*_tmp6FE=_cycalloc(sizeof(*_tmp6FE));({void*_tmpCEB=Cyc_yyget_YY44(yyyyvsp[2]);_tmp6FE->hd=_tmpCEB;});({struct Cyc_List_List*_tmpCEC=Cyc_yyget_YY40(yyyyvsp[0]);_tmp6FE->tl=_tmpCEC;});_tmp6FE;}));yyval=_tmpCED;});
goto _LL254;}case 271U: _LL46D: _LL46E: {
# 2377
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2379
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2379 "parse.y"
({union Cyc_YYSTYPE _tmpCEF=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp6FF;({struct Cyc_List_List*_tmpCEE=Cyc_yyget_YY26(yyyyvsp[0]);_tmp6FF.tms=_tmpCEE;});_tmp6FF;}));yyval=_tmpCEF;});
goto _LL254;}case 272U: _LL46F: _LL470: {
# 2382
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2384
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2381 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 273U: _LL471: _LL472: {
# 2384
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2386
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2383 "parse.y"
({union Cyc_YYSTYPE _tmpCF2=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp700;({struct Cyc_List_List*_tmpCF1=({struct Cyc_List_List*_tmpCF0=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpCF0,(Cyc_yyget_YY30(yyyyvsp[1])).tms);});_tmp700.tms=_tmpCF1;});_tmp700;}));yyval=_tmpCF2;});
goto _LL254;}case 274U: _LL473: _LL474: {
# 2386
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2388
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2388 "parse.y"
yyval=yyyyvsp[1];
goto _LL254;}case 275U: _LL475: _LL476: {
# 2391
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2393
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2390 "parse.y"
({union Cyc_YYSTYPE _tmpCF7=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp701;({struct Cyc_List_List*_tmpCF6=({struct Cyc_List_List*_tmp702=_region_malloc(yyr,sizeof(*_tmp702));({void*_tmpCF5=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp703=_region_malloc(yyr,sizeof(*_tmp703));({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpCF4=({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmp704;_tmp704.tag=0U;({union Cyc_Absyn_Constraint*_tmpCF3=Cyc_yyget_YY51(yyyyvsp[2]);_tmp704.f1=_tmpCF3;});_tmp704.f2=(unsigned int)(yyyylsp[2]).first_line;_tmp704;});_tmp703[0]=_tmpCF4;});_tmp703;});_tmp702->hd=_tmpCF5;});_tmp702->tl=0;_tmp702;});_tmp701.tms=_tmpCF6;});_tmp701;}));yyval=_tmpCF7;});
goto _LL254;}case 276U: _LL477: _LL478: {
# 2393
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2395
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2392 "parse.y"
({union Cyc_YYSTYPE _tmpCFD=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp705;({struct Cyc_List_List*_tmpCFC=({struct Cyc_List_List*_tmp706=_region_malloc(yyr,sizeof(*_tmp706));({void*_tmpCFA=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp707=_region_malloc(yyr,sizeof(*_tmp707));({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpCF9=({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmp708;_tmp708.tag=0U;({union Cyc_Absyn_Constraint*_tmpCF8=Cyc_yyget_YY51(yyyyvsp[3]);_tmp708.f1=_tmpCF8;});_tmp708.f2=(unsigned int)(yyyylsp[3]).first_line;_tmp708;});_tmp707[0]=_tmpCF9;});_tmp707;});_tmp706->hd=_tmpCFA;});({struct Cyc_List_List*_tmpCFB=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp706->tl=_tmpCFB;});_tmp706;});_tmp705.tms=_tmpCFC;});_tmp705;}));yyval=_tmpCFD;});
goto _LL254;}case 277U: _LL479: _LL47A: {
# 2395
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2397
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2394 "parse.y"
({union Cyc_YYSTYPE _tmpD03=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp709;({struct Cyc_List_List*_tmpD02=({struct Cyc_List_List*_tmp70A=_region_malloc(yyr,sizeof(*_tmp70A));({void*_tmpD01=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp70B=_region_malloc(yyr,sizeof(*_tmp70B));({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD00=({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmp70C;_tmp70C.tag=1U;({struct Cyc_Absyn_Exp*_tmpCFE=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp70C.f1=_tmpCFE;});({union Cyc_Absyn_Constraint*_tmpCFF=Cyc_yyget_YY51(yyyyvsp[3]);_tmp70C.f2=_tmpCFF;});_tmp70C.f3=(unsigned int)(yyyylsp[3]).first_line;_tmp70C;});_tmp70B[0]=_tmpD00;});_tmp70B;});_tmp70A->hd=_tmpD01;});_tmp70A->tl=0;_tmp70A;});_tmp709.tms=_tmpD02;});_tmp709;}));yyval=_tmpD03;});
goto _LL254;}case 278U: _LL47B: _LL47C: {
# 2397
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2399
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2396 "parse.y"
({union Cyc_YYSTYPE _tmpD0A=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp70D;({struct Cyc_List_List*_tmpD09=({struct Cyc_List_List*_tmp70E=_region_malloc(yyr,sizeof(*_tmp70E));({void*_tmpD07=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp70F=_region_malloc(yyr,sizeof(*_tmp70F));({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD06=({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmp710;_tmp710.tag=1U;({struct Cyc_Absyn_Exp*_tmpD04=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp710.f1=_tmpD04;});({union Cyc_Absyn_Constraint*_tmpD05=Cyc_yyget_YY51(yyyyvsp[4]);_tmp710.f2=_tmpD05;});_tmp710.f3=(unsigned int)(yyyylsp[4]).first_line;_tmp710;});_tmp70F[0]=_tmpD06;});_tmp70F;});_tmp70E->hd=_tmpD07;});({struct Cyc_List_List*_tmpD08=(
Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp70E->tl=_tmpD08;});_tmp70E;});_tmp70D.tms=_tmpD09;});_tmp70D;}));
# 2396
yyval=_tmpD0A;});
# 2399
goto _LL254;}case 279U: _LL47D: _LL47E: {
# 2401
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2403
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2400 "parse.y"
({union Cyc_YYSTYPE _tmpD14=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp711;({struct Cyc_List_List*_tmpD13=({struct Cyc_List_List*_tmp712=_region_malloc(yyr,sizeof(*_tmp712));({void*_tmpD12=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp713=_region_malloc(yyr,sizeof(*_tmp713));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD11=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp714;_tmp714.tag=3U;({void*_tmpD10=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp715=_region_malloc(yyr,sizeof(*_tmp715));({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD0F=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp716;_tmp716.tag=1U;_tmp716.f1=0;_tmp716.f2=0;_tmp716.f3=0;({void*_tmpD0B=Cyc_yyget_YY49(yyyyvsp[1]);_tmp716.f4=_tmpD0B;});({struct Cyc_List_List*_tmpD0C=Cyc_yyget_YY50(yyyyvsp[2]);_tmp716.f5=_tmpD0C;});({struct Cyc_Absyn_Exp*_tmpD0D=Cyc_yyget_YY55(yyyyvsp[4]);_tmp716.f6=_tmpD0D;});({struct Cyc_Absyn_Exp*_tmpD0E=Cyc_yyget_YY55(yyyyvsp[5]);_tmp716.f7=_tmpD0E;});_tmp716;});_tmp715[0]=_tmpD0F;});_tmp715;});_tmp714.f1=_tmpD10;});_tmp714;});_tmp713[0]=_tmpD11;});_tmp713;});_tmp712->hd=_tmpD12;});_tmp712->tl=0;_tmp712;});_tmp711.tms=_tmpD13;});_tmp711;}));yyval=_tmpD14;});
# 2402
goto _LL254;}case 280U: _LL47F: _LL480: {
# 2404
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2406
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2403 "parse.y"
struct _tuple26*_tmp717=Cyc_yyget_YY39(yyyyvsp[1]);struct _tuple26*_tmp718=_tmp717;struct Cyc_List_List*_tmp723;int _tmp722;struct Cyc_Absyn_VarargInfo*_tmp721;void*_tmp720;struct Cyc_List_List*_tmp71F;_LL6BB: _tmp723=_tmp718->f1;_tmp722=_tmp718->f2;_tmp721=_tmp718->f3;_tmp720=_tmp718->f4;_tmp71F=_tmp718->f5;_LL6BC:;
({union Cyc_YYSTYPE _tmpD1C=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp719;({struct Cyc_List_List*_tmpD1B=({struct Cyc_List_List*_tmp71A=_region_malloc(yyr,sizeof(*_tmp71A));({void*_tmpD1A=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp71B=_region_malloc(yyr,sizeof(*_tmp71B));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD19=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp71C;_tmp71C.tag=3U;({void*_tmpD18=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp71D=_region_malloc(yyr,sizeof(*_tmp71D));({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD17=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp71E;_tmp71E.tag=1U;_tmp71E.f1=_tmp723;_tmp71E.f2=_tmp722;_tmp71E.f3=_tmp721;_tmp71E.f4=_tmp720;_tmp71E.f5=_tmp71F;({struct Cyc_Absyn_Exp*_tmpD15=Cyc_yyget_YY55(yyyyvsp[3]);_tmp71E.f6=_tmpD15;});({struct Cyc_Absyn_Exp*_tmpD16=Cyc_yyget_YY55(yyyyvsp[4]);_tmp71E.f7=_tmpD16;});_tmp71E;});_tmp71D[0]=_tmpD17;});_tmp71D;});_tmp71C.f1=_tmpD18;});_tmp71C;});_tmp71B[0]=_tmpD19;});_tmp71B;});_tmp71A->hd=_tmpD1A;});_tmp71A->tl=0;_tmp71A;});_tmp719.tms=_tmpD1B;});_tmp719;}));yyval=_tmpD1C;});
# 2406
goto _LL254;}case 281U: _LL481: _LL482: {
# 2408
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2410
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2407 "parse.y"
({union Cyc_YYSTYPE _tmpD27=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp724;({struct Cyc_List_List*_tmpD26=({struct Cyc_List_List*_tmp725=_region_malloc(yyr,sizeof(*_tmp725));({void*_tmpD24=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp726=_region_malloc(yyr,sizeof(*_tmp726));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD23=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp727;_tmp727.tag=3U;({void*_tmpD22=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp728=_region_malloc(yyr,sizeof(*_tmp728));({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD21=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp729;_tmp729.tag=1U;_tmp729.f1=0;_tmp729.f2=0;_tmp729.f3=0;({void*_tmpD1D=Cyc_yyget_YY49(yyyyvsp[2]);_tmp729.f4=_tmpD1D;});({struct Cyc_List_List*_tmpD1E=Cyc_yyget_YY50(yyyyvsp[3]);_tmp729.f5=_tmpD1E;});({struct Cyc_Absyn_Exp*_tmpD1F=Cyc_yyget_YY55(yyyyvsp[5]);_tmp729.f6=_tmpD1F;});({struct Cyc_Absyn_Exp*_tmpD20=Cyc_yyget_YY55(yyyyvsp[6]);_tmp729.f7=_tmpD20;});_tmp729;});_tmp728[0]=_tmpD21;});_tmp728;});_tmp727.f1=_tmpD22;});_tmp727;});_tmp726[0]=_tmpD23;});_tmp726;});_tmp725->hd=_tmpD24;});({struct Cyc_List_List*_tmpD25=(
Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp725->tl=_tmpD25;});_tmp725;});_tmp724.tms=_tmpD26;});_tmp724;}));
# 2407
yyval=_tmpD27;});
# 2410
goto _LL254;}case 282U: _LL483: _LL484: {
# 2412
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2414
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2411 "parse.y"
struct _tuple26*_tmp72A=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp72B=_tmp72A;struct Cyc_List_List*_tmp736;int _tmp735;struct Cyc_Absyn_VarargInfo*_tmp734;void*_tmp733;struct Cyc_List_List*_tmp732;_LL6BE: _tmp736=_tmp72B->f1;_tmp735=_tmp72B->f2;_tmp734=_tmp72B->f3;_tmp733=_tmp72B->f4;_tmp732=_tmp72B->f5;_LL6BF:;
({union Cyc_YYSTYPE _tmpD30=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp72C;({struct Cyc_List_List*_tmpD2F=({struct Cyc_List_List*_tmp72D=_region_malloc(yyr,sizeof(*_tmp72D));({void*_tmpD2D=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp72E=_region_malloc(yyr,sizeof(*_tmp72E));({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD2C=({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp72F;_tmp72F.tag=3U;({void*_tmpD2B=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp730=_region_malloc(yyr,sizeof(*_tmp730));({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD2A=({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp731;_tmp731.tag=1U;_tmp731.f1=_tmp736;_tmp731.f2=_tmp735;_tmp731.f3=_tmp734;_tmp731.f4=_tmp733;_tmp731.f5=_tmp732;({struct Cyc_Absyn_Exp*_tmpD28=
Cyc_yyget_YY55(yyyyvsp[4]);_tmp731.f6=_tmpD28;});({struct Cyc_Absyn_Exp*_tmpD29=Cyc_yyget_YY55(yyyyvsp[5]);_tmp731.f7=_tmpD29;});_tmp731;});_tmp730[0]=_tmpD2A;});_tmp730;});_tmp72F.f1=_tmpD2B;});_tmp72F;});_tmp72E[0]=_tmpD2C;});_tmp72E;});_tmp72D->hd=_tmpD2D;});({struct Cyc_List_List*_tmpD2E=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp72D->tl=_tmpD2E;});_tmp72D;});_tmp72C.tms=_tmpD2F;});_tmp72C;}));
# 2412
yyval=_tmpD30;});
# 2415
goto _LL254;}case 283U: _LL485: _LL486: {
# 2417
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2419
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2417 "parse.y"
struct Cyc_List_List*_tmp737=({unsigned int _tmpD31=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpD31,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));});
({union Cyc_YYSTYPE _tmpD36=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp738;({struct Cyc_List_List*_tmpD35=({struct Cyc_List_List*_tmp739=_region_malloc(yyr,sizeof(*_tmp739));({void*_tmpD33=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp73A=_region_malloc(yyr,sizeof(*_tmp73A));({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpD32=({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmp73B;_tmp73B.tag=4U;_tmp73B.f1=_tmp737;_tmp73B.f2=(unsigned int)(yyyylsp[1]).first_line;_tmp73B.f3=0;_tmp73B;});_tmp73A[0]=_tmpD32;});_tmp73A;});_tmp739->hd=_tmpD33;});({struct Cyc_List_List*_tmpD34=(
Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp739->tl=_tmpD34;});_tmp739;});_tmp738.tms=_tmpD35;});_tmp738;}));
# 2418
yyval=_tmpD36;});
# 2421
goto _LL254;}case 284U: _LL487: _LL488: {
# 2423
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2425
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2422 "parse.y"
({union Cyc_YYSTYPE _tmpD3C=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp73C;({struct Cyc_List_List*_tmpD3B=({struct Cyc_List_List*_tmp73D=_region_malloc(yyr,sizeof(*_tmp73D));({void*_tmpD39=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp73E=_region_malloc(yyr,sizeof(*_tmp73E));({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpD38=({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmp73F;_tmp73F.tag=5U;_tmp73F.f1=(unsigned int)(yyyylsp[1]).first_line;({struct Cyc_List_List*_tmpD37=Cyc_yyget_YY45(yyyyvsp[1]);_tmp73F.f2=_tmpD37;});_tmp73F;});_tmp73E[0]=_tmpD38;});_tmp73E;});_tmp73D->hd=_tmpD39;});({struct Cyc_List_List*_tmpD3A=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp73D->tl=_tmpD3A;});_tmp73D;});_tmp73C.tms=_tmpD3B;});_tmp73C;}));yyval=_tmpD3C;});
# 2424
goto _LL254;}case 285U: _LL489: _LL48A: {
# 2426
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2428
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2428 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 286U: _LL48B: _LL48C: {
# 2431
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2433
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2429 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 287U: _LL48D: _LL48E: {
# 2432
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2434
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2430 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 288U: _LL48F: _LL490: {
# 2433
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2435
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2431 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 289U: _LL491: _LL492: {
# 2434
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2436
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2432 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 290U: _LL493: _LL494: {
# 2435
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2437
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2433 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 291U: _LL495: _LL496: {
# 2436
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2438
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
({union Cyc_YYSTYPE _tmpD42=Cyc_Stmt_tok(({void*_tmpD41=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp740=_cycalloc(sizeof(*_tmp740));({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpD40=({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmp741;_tmp741.tag=13U;({struct _dyneither_ptr*_tmpD3E=({struct _dyneither_ptr*_tmp742=_cycalloc(sizeof(*_tmp742));({struct _dyneither_ptr _tmpD3D=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp742[0]=_tmpD3D;});_tmp742;});_tmp741.f1=_tmpD3E;});({struct Cyc_Absyn_Stmt*_tmpD3F=Cyc_yyget_Stmt_tok(yyyyvsp[2]);_tmp741.f2=_tmpD3F;});_tmp741;});_tmp740[0]=_tmpD40;});_tmp740;});Cyc_Absyn_new_stmt(_tmpD41,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD42;});
goto _LL254;}case 292U: _LL497: _LL498: {
# 2442
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2444
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2443 "parse.y"
({union Cyc_YYSTYPE _tmpD43=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpD43;});
goto _LL254;}case 293U: _LL499: _LL49A: {
# 2446
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2448
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2444 "parse.y"
({union Cyc_YYSTYPE _tmpD45=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpD44=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_exp_stmt(_tmpD44,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD45;});
goto _LL254;}case 294U: _LL49B: _LL49C: {
# 2447
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2449
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2449 "parse.y"
({union Cyc_YYSTYPE _tmpD46=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpD46;});
goto _LL254;}case 295U: _LL49D: _LL49E: {
# 2452
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2454
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2450 "parse.y"
yyval=yyyyvsp[1];
goto _LL254;}case 296U: _LL49F: _LL4A0: {
# 2453
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2455
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2455 "parse.y"
({union Cyc_YYSTYPE _tmpD48=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpD47=Cyc_yyget_YY16(yyyyvsp[0]);Cyc_Parse_flatten_declarations(_tmpD47,Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));}));yyval=_tmpD48;});
goto _LL254;}case 297U: _LL4A1: _LL4A2: {
# 2458
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2460
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2456 "parse.y"
({union Cyc_YYSTYPE _tmpD4A=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpD49=Cyc_yyget_YY16(yyyyvsp[0]);Cyc_Parse_flatten_declarations(_tmpD49,Cyc_yyget_Stmt_tok(yyyyvsp[1]));}));yyval=_tmpD4A;});
goto _LL254;}case 298U: _LL4A3: _LL4A4: {
# 2459
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2461
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2457 "parse.y"
({union Cyc_YYSTYPE _tmpD51=Cyc_Stmt_tok(({void*_tmpD50=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp743=_cycalloc(sizeof(*_tmp743));({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpD4F=({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmp744;_tmp744.tag=13U;({struct _dyneither_ptr*_tmpD4C=({struct _dyneither_ptr*_tmp745=_cycalloc(sizeof(*_tmp745));({struct _dyneither_ptr _tmpD4B=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp745[0]=_tmpD4B;});_tmp745;});_tmp744.f1=_tmpD4C;});({struct Cyc_Absyn_Stmt*_tmpD4E=({struct Cyc_List_List*_tmpD4D=Cyc_yyget_YY16(yyyyvsp[2]);Cyc_Parse_flatten_declarations(_tmpD4D,Cyc_Absyn_skip_stmt(0U));});_tmp744.f2=_tmpD4E;});_tmp744;});_tmp743[0]=_tmpD4F;});_tmp743;});Cyc_Absyn_new_stmt(_tmpD50,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD51;});
# 2459
goto _LL254;}case 299U: _LL4A5: _LL4A6: {
# 2461
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2463
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2459 "parse.y"
({union Cyc_YYSTYPE _tmpD58=Cyc_Stmt_tok(({void*_tmpD57=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp746=_cycalloc(sizeof(*_tmp746));({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpD56=({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmp747;_tmp747.tag=13U;({struct _dyneither_ptr*_tmpD53=({struct _dyneither_ptr*_tmp748=_cycalloc(sizeof(*_tmp748));({struct _dyneither_ptr _tmpD52=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp748[0]=_tmpD52;});_tmp748;});_tmp747.f1=_tmpD53;});({struct Cyc_Absyn_Stmt*_tmpD55=({struct Cyc_List_List*_tmpD54=Cyc_yyget_YY16(yyyyvsp[2]);Cyc_Parse_flatten_declarations(_tmpD54,Cyc_yyget_Stmt_tok(yyyyvsp[3]));});_tmp747.f2=_tmpD55;});_tmp747;});_tmp746[0]=_tmpD56;});_tmp746;});Cyc_Absyn_new_stmt(_tmpD57,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD58;});
# 2461
goto _LL254;}case 300U: _LL4A7: _LL4A8: {
# 2463
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2465
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2461 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 301U: _LL4A9: _LL4AA: {
# 2464
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2466
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2462 "parse.y"
({union Cyc_YYSTYPE _tmpD5B=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpD5A=Cyc_yyget_Stmt_tok(yyyyvsp[0]);struct Cyc_Absyn_Stmt*_tmpD59=Cyc_yyget_Stmt_tok(yyyyvsp[1]);Cyc_Absyn_seq_stmt(_tmpD5A,_tmpD59,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD5B;});
goto _LL254;}case 302U: _LL4AB: _LL4AC: {
# 2465
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2467
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2463 "parse.y"
({union Cyc_YYSTYPE _tmpD60=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpD5F=({void*_tmpD5E=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp749=_cycalloc(sizeof(*_tmp749));({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpD5D=({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmp74A;_tmp74A.tag=1U;({struct Cyc_Absyn_Fndecl*_tmpD5C=Cyc_yyget_YY15(yyyyvsp[0]);_tmp74A.f1=_tmpD5C;});_tmp74A;});_tmp749[0]=_tmpD5D;});_tmp749;});Cyc_Absyn_new_decl(_tmpD5E,(unsigned int)(yyyylsp[0]).first_line);});Cyc_Parse_flatten_decl(_tmpD5F,
Cyc_Absyn_skip_stmt(0U));}));
# 2463
yyval=_tmpD60;});
# 2465
goto _LL254;}case 303U: _LL4AD: _LL4AE: {
# 2467
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2469
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2466 "parse.y"
({union Cyc_YYSTYPE _tmpD65=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpD64=({void*_tmpD63=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp74B=_cycalloc(sizeof(*_tmp74B));({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpD62=({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmp74C;_tmp74C.tag=1U;({struct Cyc_Absyn_Fndecl*_tmpD61=Cyc_yyget_YY15(yyyyvsp[0]);_tmp74C.f1=_tmpD61;});_tmp74C;});_tmp74B[0]=_tmpD62;});_tmp74B;});Cyc_Absyn_new_decl(_tmpD63,(unsigned int)(yyyylsp[0]).first_line);});Cyc_Parse_flatten_decl(_tmpD64,Cyc_yyget_Stmt_tok(yyyyvsp[1]));}));yyval=_tmpD65;});
goto _LL254;}case 304U: _LL4AF: _LL4B0: {
# 2469
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2471
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2471 "parse.y"
({union Cyc_YYSTYPE _tmpD69=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpD68=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmpD67=Cyc_yyget_Stmt_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpD66=Cyc_Absyn_skip_stmt(0U);Cyc_Absyn_ifthenelse_stmt(_tmpD68,_tmpD67,_tmpD66,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD69;});
goto _LL254;}case 305U: _LL4B1: _LL4B2: {
# 2474
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2476
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2473 "parse.y"
({union Cyc_YYSTYPE _tmpD6D=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpD6C=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmpD6B=Cyc_yyget_Stmt_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpD6A=Cyc_yyget_Stmt_tok(yyyyvsp[6]);Cyc_Absyn_ifthenelse_stmt(_tmpD6C,_tmpD6B,_tmpD6A,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD6D;});
goto _LL254;}case 306U: _LL4B3: _LL4B4: {
# 2476
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2478
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
({union Cyc_YYSTYPE _tmpD70=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpD6F=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_List_List*_tmpD6E=Cyc_yyget_YY8(yyyyvsp[5]);Cyc_Absyn_switch_stmt(_tmpD6F,_tmpD6E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD70;});
goto _LL254;}case 307U: _LL4B5: _LL4B6: {
# 2482
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2484
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2482 "parse.y"
struct Cyc_Absyn_Exp*_tmp74D=({struct _tuple0*_tmpD71=Cyc_yyget_QualId_tok(yyyyvsp[1]);Cyc_Absyn_unknownid_exp(_tmpD71,(unsigned int)(yyyylsp[1]).first_line);});
({union Cyc_YYSTYPE _tmpD74=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpD73=_tmp74D;struct Cyc_List_List*_tmpD72=Cyc_yyget_YY8(yyyyvsp[3]);Cyc_Absyn_switch_stmt(_tmpD73,_tmpD72,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD74;});
goto _LL254;}case 308U: _LL4B7: _LL4B8: {
# 2486
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2488
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2486 "parse.y"
struct Cyc_Absyn_Exp*_tmp74E=({struct Cyc_List_List*_tmpD75=Cyc_yyget_YY4(yyyyvsp[3]);Cyc_Absyn_tuple_exp(_tmpD75,(unsigned int)(yyyylsp[1]).first_line);});
({union Cyc_YYSTYPE _tmpD78=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpD77=_tmp74E;struct Cyc_List_List*_tmpD76=Cyc_yyget_YY8(yyyyvsp[6]);Cyc_Absyn_switch_stmt(_tmpD77,_tmpD76,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD78;});
# 2489
goto _LL254;}case 309U: _LL4B9: _LL4BA: {
# 2491
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2493
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2492 "parse.y"
({union Cyc_YYSTYPE _tmpD7B=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpD7A=Cyc_yyget_Stmt_tok(yyyyvsp[1]);struct Cyc_List_List*_tmpD79=Cyc_yyget_YY8(yyyyvsp[4]);Cyc_Absyn_trycatch_stmt(_tmpD7A,_tmpD79,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD7B;});
goto _LL254;}case 310U: _LL4BB: _LL4BC:
# 2495
({union Cyc_YYSTYPE _tmpD7C=Cyc_YY8(0);yyval=_tmpD7C;});
goto _LL254;case 311U: _LL4BD: _LL4BE: {
# 2498
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2500
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2510 "parse.y"
({union Cyc_YYSTYPE _tmpD81=Cyc_YY8(({struct Cyc_List_List*_tmp74F=_cycalloc(sizeof(*_tmp74F));({struct Cyc_Absyn_Switch_clause*_tmpD7F=({struct Cyc_Absyn_Switch_clause*_tmp750=_cycalloc(sizeof(*_tmp750));({struct Cyc_Absyn_Pat*_tmpD7D=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line);_tmp750->pattern=_tmpD7D;});_tmp750->pat_vars=0;_tmp750->where_clause=0;({struct Cyc_Absyn_Stmt*_tmpD7E=
Cyc_yyget_Stmt_tok(yyyyvsp[2]);_tmp750->body=_tmpD7E;});_tmp750->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp750;});_tmp74F->hd=_tmpD7F;});({struct Cyc_List_List*_tmpD80=
Cyc_yyget_YY8(yyyyvsp[3]);_tmp74F->tl=_tmpD80;});_tmp74F;}));
# 2510
yyval=_tmpD81;});
# 2513
goto _LL254;}case 312U: _LL4BF: _LL4C0: {
# 2515
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2517
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2514 "parse.y"
({union Cyc_YYSTYPE _tmpD86=Cyc_YY8(({struct Cyc_List_List*_tmp751=_cycalloc(sizeof(*_tmp751));({struct Cyc_Absyn_Switch_clause*_tmpD84=({struct Cyc_Absyn_Switch_clause*_tmp752=_cycalloc(sizeof(*_tmp752));({struct Cyc_Absyn_Pat*_tmpD82=Cyc_yyget_YY9(yyyyvsp[1]);_tmp752->pattern=_tmpD82;});_tmp752->pat_vars=0;_tmp752->where_clause=0;({struct Cyc_Absyn_Stmt*_tmpD83=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line);_tmp752->body=_tmpD83;});_tmp752->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp752;});_tmp751->hd=_tmpD84;});({struct Cyc_List_List*_tmpD85=
Cyc_yyget_YY8(yyyyvsp[3]);_tmp751->tl=_tmpD85;});_tmp751;}));
# 2514
yyval=_tmpD86;});
# 2517
goto _LL254;}case 313U: _LL4C1: _LL4C2: {
# 2519
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2521
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2518 "parse.y"
({union Cyc_YYSTYPE _tmpD8B=Cyc_YY8(({struct Cyc_List_List*_tmp753=_cycalloc(sizeof(*_tmp753));({struct Cyc_Absyn_Switch_clause*_tmpD89=({struct Cyc_Absyn_Switch_clause*_tmp754=_cycalloc(sizeof(*_tmp754));({struct Cyc_Absyn_Pat*_tmpD87=Cyc_yyget_YY9(yyyyvsp[1]);_tmp754->pattern=_tmpD87;});_tmp754->pat_vars=0;_tmp754->where_clause=0;({struct Cyc_Absyn_Stmt*_tmpD88=Cyc_yyget_Stmt_tok(yyyyvsp[3]);_tmp754->body=_tmpD88;});_tmp754->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp754;});_tmp753->hd=_tmpD89;});({struct Cyc_List_List*_tmpD8A=Cyc_yyget_YY8(yyyyvsp[4]);_tmp753->tl=_tmpD8A;});_tmp753;}));yyval=_tmpD8B;});
goto _LL254;}case 314U: _LL4C3: _LL4C4: {
# 2521
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2523
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2520 "parse.y"
({union Cyc_YYSTYPE _tmpD91=Cyc_YY8(({struct Cyc_List_List*_tmp755=_cycalloc(sizeof(*_tmp755));({struct Cyc_Absyn_Switch_clause*_tmpD8F=({struct Cyc_Absyn_Switch_clause*_tmp756=_cycalloc(sizeof(*_tmp756));({struct Cyc_Absyn_Pat*_tmpD8C=Cyc_yyget_YY9(yyyyvsp[1]);_tmp756->pattern=_tmpD8C;});_tmp756->pat_vars=0;({struct Cyc_Absyn_Exp*_tmpD8D=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp756->where_clause=_tmpD8D;});({struct Cyc_Absyn_Stmt*_tmpD8E=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line);_tmp756->body=_tmpD8E;});_tmp756->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp756;});_tmp755->hd=_tmpD8F;});({struct Cyc_List_List*_tmpD90=
Cyc_yyget_YY8(yyyyvsp[5]);_tmp755->tl=_tmpD90;});_tmp755;}));
# 2520
yyval=_tmpD91;});
# 2523
goto _LL254;}case 315U: _LL4C5: _LL4C6: {
# 2525
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2527
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2524 "parse.y"
({union Cyc_YYSTYPE _tmpD97=Cyc_YY8(({struct Cyc_List_List*_tmp757=_cycalloc(sizeof(*_tmp757));({struct Cyc_Absyn_Switch_clause*_tmpD95=({struct Cyc_Absyn_Switch_clause*_tmp758=_cycalloc(sizeof(*_tmp758));({struct Cyc_Absyn_Pat*_tmpD92=Cyc_yyget_YY9(yyyyvsp[1]);_tmp758->pattern=_tmpD92;});_tmp758->pat_vars=0;({struct Cyc_Absyn_Exp*_tmpD93=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp758->where_clause=_tmpD93;});({struct Cyc_Absyn_Stmt*_tmpD94=Cyc_yyget_Stmt_tok(yyyyvsp[5]);_tmp758->body=_tmpD94;});_tmp758->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp758;});_tmp757->hd=_tmpD95;});({struct Cyc_List_List*_tmpD96=Cyc_yyget_YY8(yyyyvsp[6]);_tmp757->tl=_tmpD96;});_tmp757;}));yyval=_tmpD97;});
goto _LL254;}case 316U: _LL4C7: _LL4C8: {
# 2527
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2529
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2531 "parse.y"
({union Cyc_YYSTYPE _tmpD9A=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpD99=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmpD98=Cyc_yyget_Stmt_tok(yyyyvsp[4]);Cyc_Absyn_while_stmt(_tmpD99,_tmpD98,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD9A;});
goto _LL254;}case 317U: _LL4C9: _LL4CA: {
# 2534
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2536
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2535 "parse.y"
({union Cyc_YYSTYPE _tmpD9D=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpD9C=Cyc_yyget_Stmt_tok(yyyyvsp[1]);struct Cyc_Absyn_Exp*_tmpD9B=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_do_stmt(_tmpD9C,_tmpD9B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD9D;});
goto _LL254;}case 318U: _LL4CB: _LL4CC: {
# 2538
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2540
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2539 "parse.y"
({union Cyc_YYSTYPE _tmpDA2=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpDA1=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpDA0=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpD9F=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpD9E=
Cyc_yyget_Stmt_tok(yyyyvsp[5]);
# 2539
Cyc_Absyn_for_stmt(_tmpDA1,_tmpDA0,_tmpD9F,_tmpD9E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDA2;});
# 2541
goto _LL254;}case 319U: _LL4CD: _LL4CE: {
# 2543
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2545
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2542 "parse.y"
({union Cyc_YYSTYPE _tmpDA7=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpDA6=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpDA5=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpDA4=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpDA3=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2542
Cyc_Absyn_for_stmt(_tmpDA6,_tmpDA5,_tmpDA4,_tmpDA3,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDA7;});
# 2544
goto _LL254;}case 320U: _LL4CF: _LL4D0: {
# 2546
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2548
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2545 "parse.y"
({union Cyc_YYSTYPE _tmpDAC=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpDAB=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpDAA=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpDA9=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpDA8=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2545
Cyc_Absyn_for_stmt(_tmpDAB,_tmpDAA,_tmpDA9,_tmpDA8,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDAC;});
# 2547
goto _LL254;}case 321U: _LL4D1: _LL4D2: {
# 2549
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2551
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2548 "parse.y"
({union Cyc_YYSTYPE _tmpDB1=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpDB0=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpDAF=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpDAE=Cyc_yyget_Exp_tok(yyyyvsp[5]);struct Cyc_Absyn_Stmt*_tmpDAD=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2548
Cyc_Absyn_for_stmt(_tmpDB0,_tmpDAF,_tmpDAE,_tmpDAD,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDB1;});
# 2550
goto _LL254;}case 322U: _LL4D3: _LL4D4: {
# 2552
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2554
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2551 "parse.y"
({union Cyc_YYSTYPE _tmpDB6=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpDB5=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpDB4=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpDB3=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpDB2=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2551
Cyc_Absyn_for_stmt(_tmpDB5,_tmpDB4,_tmpDB3,_tmpDB2,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDB6;});
# 2553
goto _LL254;}case 323U: _LL4D5: _LL4D6: {
# 2555
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2557
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2554 "parse.y"
({union Cyc_YYSTYPE _tmpDBB=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpDBA=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpDB9=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpDB8=Cyc_yyget_Exp_tok(yyyyvsp[5]);struct Cyc_Absyn_Stmt*_tmpDB7=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2554
Cyc_Absyn_for_stmt(_tmpDBA,_tmpDB9,_tmpDB8,_tmpDB7,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDBB;});
# 2556
goto _LL254;}case 324U: _LL4D7: _LL4D8: {
# 2558
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2560
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2557 "parse.y"
({union Cyc_YYSTYPE _tmpDC0=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpDBF=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpDBE=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Exp*_tmpDBD=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpDBC=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2557
Cyc_Absyn_for_stmt(_tmpDBF,_tmpDBE,_tmpDBD,_tmpDBC,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDC0;});
# 2559
goto _LL254;}case 325U: _LL4D9: _LL4DA: {
# 2561
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9U));
# 2563
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9U));
# 2560 "parse.y"
({union Cyc_YYSTYPE _tmpDC5=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpDC4=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpDC3=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Exp*_tmpDC2=Cyc_yyget_Exp_tok(yyyyvsp[6]);struct Cyc_Absyn_Stmt*_tmpDC1=
Cyc_yyget_Stmt_tok(yyyyvsp[8]);
# 2560
Cyc_Absyn_for_stmt(_tmpDC4,_tmpDC3,_tmpDC2,_tmpDC1,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDC5;});
# 2562
goto _LL254;}case 326U: _LL4DB: _LL4DC: {
# 2564
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2566
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2563 "parse.y"
struct Cyc_List_List*_tmp759=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp75A=({struct Cyc_Absyn_Exp*_tmpDC9=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpDC8=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpDC7=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpDC6=
Cyc_yyget_Stmt_tok(yyyyvsp[5]);
# 2564
Cyc_Absyn_for_stmt(_tmpDC9,_tmpDC8,_tmpDC7,_tmpDC6,(unsigned int)(yyyylsp[0]).first_line);});
# 2566
({union Cyc_YYSTYPE _tmpDCA=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp759,_tmp75A));yyval=_tmpDCA;});
# 2568
goto _LL254;}case 327U: _LL4DD: _LL4DE: {
# 2570
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2572
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2569 "parse.y"
struct Cyc_List_List*_tmp75B=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp75C=({struct Cyc_Absyn_Exp*_tmpDCE=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpDCD=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpDCC=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpDCB=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2570
Cyc_Absyn_for_stmt(_tmpDCE,_tmpDCD,_tmpDCC,_tmpDCB,(unsigned int)(yyyylsp[0]).first_line);});
# 2572
({union Cyc_YYSTYPE _tmpDCF=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp75B,_tmp75C));yyval=_tmpDCF;});
# 2574
goto _LL254;}case 328U: _LL4DF: _LL4E0: {
# 2576
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2578
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2575 "parse.y"
struct Cyc_List_List*_tmp75D=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp75E=({struct Cyc_Absyn_Exp*_tmpDD3=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpDD2=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpDD1=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpDD0=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2576
Cyc_Absyn_for_stmt(_tmpDD3,_tmpDD2,_tmpDD1,_tmpDD0,(unsigned int)(yyyylsp[0]).first_line);});
# 2578
({union Cyc_YYSTYPE _tmpDD4=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp75D,_tmp75E));yyval=_tmpDD4;});
# 2580
goto _LL254;}case 329U: _LL4E1: _LL4E2: {
# 2582
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2584
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2581 "parse.y"
struct Cyc_List_List*_tmp75F=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp760=({struct Cyc_Absyn_Exp*_tmpDD8=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpDD7=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpDD6=Cyc_yyget_Exp_tok(yyyyvsp[5]);struct Cyc_Absyn_Stmt*_tmpDD5=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2582
Cyc_Absyn_for_stmt(_tmpDD8,_tmpDD7,_tmpDD6,_tmpDD5,(unsigned int)(yyyylsp[0]).first_line);});
# 2584
({union Cyc_YYSTYPE _tmpDD9=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp75F,_tmp760));yyval=_tmpDD9;});
# 2586
goto _LL254;}case 330U: _LL4E3: _LL4E4: {
# 2588
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2590
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
({union Cyc_YYSTYPE _tmpDDC=Cyc_Stmt_tok(({struct _dyneither_ptr*_tmpDDB=({struct _dyneither_ptr*_tmp761=_cycalloc(sizeof(*_tmp761));({struct _dyneither_ptr _tmpDDA=Cyc_yyget_String_tok(yyyyvsp[1]);_tmp761[0]=_tmpDDA;});_tmp761;});Cyc_Absyn_goto_stmt(_tmpDDB,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDDC;});
goto _LL254;}case 331U: _LL4E5: _LL4E6: {
# 2594
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2596
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2592 "parse.y"
({union Cyc_YYSTYPE _tmpDDD=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpDDD;});
goto _LL254;}case 332U: _LL4E7: _LL4E8: {
# 2595
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2597
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2593 "parse.y"
({union Cyc_YYSTYPE _tmpDDE=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpDDE;});
goto _LL254;}case 333U: _LL4E9: _LL4EA: {
# 2596
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2598
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2594 "parse.y"
({union Cyc_YYSTYPE _tmpDDF=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpDDF;});
goto _LL254;}case 334U: _LL4EB: _LL4EC: {
# 2597
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2599
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2595 "parse.y"
({union Cyc_YYSTYPE _tmpDE1=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpDE0=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_return_stmt(_tmpDE0,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDE1;});
goto _LL254;}case 335U: _LL4ED: _LL4EE: {
# 2598
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2600
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2597 "parse.y"
({union Cyc_YYSTYPE _tmpDE2=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpDE2;});
goto _LL254;}case 336U: _LL4EF: _LL4F0: {
# 2600
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2602
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2598 "parse.y"
({union Cyc_YYSTYPE _tmpDE3=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpDE3;});
goto _LL254;}case 337U: _LL4F1: _LL4F2: {
# 2601
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2603
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2600 "parse.y"
({union Cyc_YYSTYPE _tmpDE5=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpDE4=Cyc_yyget_YY4(yyyyvsp[2]);Cyc_Absyn_fallthru_stmt(_tmpDE4,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDE5;});
goto _LL254;}case 338U: _LL4F3: _LL4F4: {
# 2603
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2605
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2609 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 339U: _LL4F5: _LL4F6: {
# 2612
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2612 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 340U: _LL4F7: _LL4F8: {
# 2615
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2617
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2614 "parse.y"
({union Cyc_YYSTYPE _tmpDE9=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpDE8=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpDE7=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpDE6=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_conditional_exp(_tmpDE8,_tmpDE7,_tmpDE6,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpDE9;});
goto _LL254;}case 341U: _LL4F9: _LL4FA: {
# 2617
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2617 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 342U: _LL4FB: _LL4FC: {
# 2620
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2622
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2619 "parse.y"
({union Cyc_YYSTYPE _tmpDEC=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpDEB=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpDEA=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_or_exp(_tmpDEB,_tmpDEA,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpDEC;});
goto _LL254;}case 343U: _LL4FD: _LL4FE: {
# 2622
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2624
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2622 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 344U: _LL4FF: _LL500: {
# 2625
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2627
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2624 "parse.y"
({union Cyc_YYSTYPE _tmpDEF=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpDEE=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpDED=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_and_exp(_tmpDEE,_tmpDED,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpDEF;});
goto _LL254;}case 345U: _LL501: _LL502: {
# 2627
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2629
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2627 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 346U: _LL503: _LL504: {
# 2630
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2632
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2629 "parse.y"
({union Cyc_YYSTYPE _tmpDF2=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpDF1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpDF0=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpDF1,_tmpDF0,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpDF2;});
goto _LL254;}case 347U: _LL505: _LL506: {
# 2632
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2634
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2632 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 348U: _LL507: _LL508: {
# 2635
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2637
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2634 "parse.y"
({union Cyc_YYSTYPE _tmpDF5=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpDF4=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpDF3=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpDF4,_tmpDF3,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpDF5;});
goto _LL254;}case 349U: _LL509: _LL50A: {
# 2637
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2639
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2637 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 350U: _LL50B: _LL50C: {
# 2640
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2642
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2639 "parse.y"
({union Cyc_YYSTYPE _tmpDF8=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpDF7=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpDF6=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpDF7,_tmpDF6,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpDF8;});
goto _LL254;}case 351U: _LL50D: _LL50E: {
# 2642
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2644
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2642 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 352U: _LL50F: _LL510: {
# 2645
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2647
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2644 "parse.y"
({union Cyc_YYSTYPE _tmpDFB=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpDFA=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpDF9=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_eq_exp(_tmpDFA,_tmpDF9,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpDFB;});
goto _LL254;}case 353U: _LL511: _LL512: {
# 2647
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2649
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2646 "parse.y"
({union Cyc_YYSTYPE _tmpDFE=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpDFD=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpDFC=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_neq_exp(_tmpDFD,_tmpDFC,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpDFE;});
goto _LL254;}case 354U: _LL513: _LL514: {
# 2649
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2651
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2649 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 355U: _LL515: _LL516: {
# 2652
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2654
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2651 "parse.y"
({union Cyc_YYSTYPE _tmpE01=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpE00=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpDFF=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lt_exp(_tmpE00,_tmpDFF,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE01;});
goto _LL254;}case 356U: _LL517: _LL518: {
# 2654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2653 "parse.y"
({union Cyc_YYSTYPE _tmpE04=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpE03=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpE02=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gt_exp(_tmpE03,_tmpE02,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE04;});
goto _LL254;}case 357U: _LL519: _LL51A: {
# 2656
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2655 "parse.y"
({union Cyc_YYSTYPE _tmpE07=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpE06=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpE05=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lte_exp(_tmpE06,_tmpE05,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE07;});
goto _LL254;}case 358U: _LL51B: _LL51C: {
# 2658
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2657 "parse.y"
({union Cyc_YYSTYPE _tmpE0A=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpE09=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpE08=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gte_exp(_tmpE09,_tmpE08,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE0A;});
goto _LL254;}case 359U: _LL51D: _LL51E: {
# 2660
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2662
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2660 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 360U: _LL51F: _LL520: {
# 2663
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2665
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2662 "parse.y"
({union Cyc_YYSTYPE _tmpE0D=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpE0C=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpE0B=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpE0C,_tmpE0B,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE0D;});
goto _LL254;}case 361U: _LL521: _LL522: {
# 2665
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2667
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2664 "parse.y"
({union Cyc_YYSTYPE _tmpE10=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpE0F=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpE0E=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpE0F,_tmpE0E,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE10;});
goto _LL254;}case 362U: _LL523: _LL524: {
# 2667
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2669
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2667 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 363U: _LL525: _LL526: {
# 2670
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2669 "parse.y"
({union Cyc_YYSTYPE _tmpE13=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpE12=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpE11=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpE12,_tmpE11,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE13;});
goto _LL254;}case 364U: _LL527: _LL528: {
# 2672
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2674
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2671 "parse.y"
({union Cyc_YYSTYPE _tmpE16=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpE15=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpE14=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpE15,_tmpE14,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE16;});
goto _LL254;}case 365U: _LL529: _LL52A: {
# 2674
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2676
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2674 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 366U: _LL52B: _LL52C: {
# 2677
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2679
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2676 "parse.y"
({union Cyc_YYSTYPE _tmpE19=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpE18=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpE17=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpE18,_tmpE17,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE19;});
goto _LL254;}case 367U: _LL52D: _LL52E: {
# 2679
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2681
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2678 "parse.y"
({union Cyc_YYSTYPE _tmpE1C=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpE1B=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpE1A=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpE1B,_tmpE1A,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE1C;});
goto _LL254;}case 368U: _LL52F: _LL530: {
# 2681
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2683
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2680 "parse.y"
({union Cyc_YYSTYPE _tmpE1F=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpE1E=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpE1D=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpE1E,_tmpE1D,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE1F;});
goto _LL254;}case 369U: _LL531: _LL532: {
# 2683
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2685
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2683 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 370U: _LL533: _LL534: {
# 2686
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2688
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2685 "parse.y"
void*_tmp762=({struct _tuple8*_tmpE20=Cyc_yyget_YY37(yyyyvsp[1]);Cyc_Parse_type_name_to_type(_tmpE20,(unsigned int)(yyyylsp[1]).first_line);});
({union Cyc_YYSTYPE _tmpE23=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpE22=_tmp762;struct Cyc_Absyn_Exp*_tmpE21=Cyc_yyget_Exp_tok(yyyyvsp[3]);Cyc_Absyn_cast_exp(_tmpE22,_tmpE21,1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE23;});
# 2688
goto _LL254;}case 371U: _LL535: _LL536: {
# 2690
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2692
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2691 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 372U: _LL537: _LL538: {
# 2694
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2696
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2694 "parse.y"
({union Cyc_YYSTYPE _tmpE24=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));yyval=_tmpE24;});
goto _LL254;}case 373U: _LL539: _LL53A: {
# 2697
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2699
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2696 "parse.y"
({union Cyc_YYSTYPE _tmpE27=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _tmpE26=Cyc_yyget_YY6(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpE25=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_prim1_exp(_tmpE26,_tmpE25,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE27;});
goto _LL254;}case 374U: _LL53B: _LL53C: {
# 2699
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2701
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2698 "parse.y"
void*_tmp763=({struct _tuple8*_tmpE28=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpE28,(unsigned int)(yyyylsp[2]).first_line);});
({union Cyc_YYSTYPE _tmpE29=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp763,(unsigned int)(yyyylsp[0]).first_line)));yyval=_tmpE29;});
# 2701
goto _LL254;}case 375U: _LL53D: _LL53E: {
# 2703
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2705
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2702 "parse.y"
({union Cyc_YYSTYPE _tmpE2B=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpE2A=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_sizeofexp_exp(_tmpE2A,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE2B;});
goto _LL254;}case 376U: _LL53F: _LL540: {
# 2705
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2707
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2704 "parse.y"
({union Cyc_YYSTYPE _tmpE2E=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpE2D=(*Cyc_yyget_YY37(yyyyvsp[2])).f3;struct Cyc_List_List*_tmpE2C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4]));Cyc_Absyn_offsetof_exp(_tmpE2D,_tmpE2C,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpE2E;});
goto _LL254;}case 377U: _LL541: _LL542: {
# 2707
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2709
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2709 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 378U: _LL543: _LL544: {
# 2712
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2714
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2717 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 379U: _LL545: _LL546: {
# 2720
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2722
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2722 "parse.y"
({union Cyc_YYSTYPE _tmpE2F=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpE2F;});
goto _LL254;}case 380U: _LL547: _LL548: {
# 2725
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2727
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2724 "parse.y"
({union Cyc_YYSTYPE _tmpE30=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));yyval=_tmpE30;});
goto _LL254;}case 381U: _LL549: _LL54A: {
# 2727
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2729
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2726 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(yyyyvsp[0]);
{void*_tmp764=e->r;void*_tmp765=_tmp764;struct _dyneither_ptr _tmp77D;int _tmp77C;enum Cyc_Absyn_Sign _tmp77B;int _tmp77A;enum Cyc_Absyn_Sign _tmp779;short _tmp778;enum Cyc_Absyn_Sign _tmp777;char _tmp776;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp765)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp765)->f1).LongLong_c).tag){case 2U: _LL6C1: _tmp777=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp765)->f1).Char_c).val).f1;_tmp776=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp765)->f1).Char_c).val).f2;_LL6C2:
# 2730
({union Cyc_YYSTYPE _tmpE33=Cyc_YY9(({void*_tmpE32=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp766=_cycalloc_atomic(sizeof(*_tmp766));({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct _tmpE31=({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct _tmp767;_tmp767.tag=11U;_tmp767.f1=_tmp776;_tmp767;});_tmp766[0]=_tmpE31;});_tmp766;});Cyc_Absyn_new_pat(_tmpE32,e->loc);}));yyval=_tmpE33;});goto _LL6C0;case 4U: _LL6C3: _tmp779=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp765)->f1).Short_c).val).f1;_tmp778=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp765)->f1).Short_c).val).f2;_LL6C4:
# 2732
({union Cyc_YYSTYPE _tmpE36=Cyc_YY9(({void*_tmpE35=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp768=_cycalloc_atomic(sizeof(*_tmp768));({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpE34=({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp769;_tmp769.tag=10U;_tmp769.f1=_tmp779;_tmp769.f2=(int)_tmp778;_tmp769;});_tmp768[0]=_tmpE34;});_tmp768;});Cyc_Absyn_new_pat(_tmpE35,e->loc);}));yyval=_tmpE36;});goto _LL6C0;case 5U: _LL6C5: _tmp77B=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp765)->f1).Int_c).val).f1;_tmp77A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp765)->f1).Int_c).val).f2;_LL6C6:
# 2734
({union Cyc_YYSTYPE _tmpE39=Cyc_YY9(({void*_tmpE38=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp76A=_cycalloc_atomic(sizeof(*_tmp76A));({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpE37=({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp76B;_tmp76B.tag=10U;_tmp76B.f1=_tmp77B;_tmp76B.f2=_tmp77A;_tmp76B;});_tmp76A[0]=_tmpE37;});_tmp76A;});Cyc_Absyn_new_pat(_tmpE38,e->loc);}));yyval=_tmpE39;});goto _LL6C0;case 7U: _LL6C7: _tmp77D=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp765)->f1).Float_c).val).f1;_tmp77C=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp765)->f1).Float_c).val).f2;_LL6C8:
# 2736
({union Cyc_YYSTYPE _tmpE3C=Cyc_YY9(({void*_tmpE3B=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp76C=_cycalloc(sizeof(*_tmp76C));({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct _tmpE3A=({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct _tmp76D;_tmp76D.tag=12U;_tmp76D.f1=_tmp77D;_tmp76D.f2=_tmp77C;_tmp76D;});_tmp76C[0]=_tmpE3A;});_tmp76C;});Cyc_Absyn_new_pat(_tmpE3B,e->loc);}));yyval=_tmpE3C;});goto _LL6C0;case 1U: _LL6C9: _LL6CA:
# 2738
({union Cyc_YYSTYPE _tmpE3D=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));yyval=_tmpE3D;});goto _LL6C0;case 8U: _LL6CB: _LL6CC:
# 2740
({void*_tmp76E=0U;({unsigned int _tmpE3F=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpE3E=({const char*_tmp76F="strings cannot occur within patterns";_tag_dyneither(_tmp76F,sizeof(char),37U);});Cyc_Warn_err(_tmpE3F,_tmpE3E,_tag_dyneither(_tmp76E,sizeof(void*),0U));});});goto _LL6C0;case 9U: _LL6CD: _LL6CE:
# 2742
({void*_tmp770=0U;({unsigned int _tmpE41=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpE40=({const char*_tmp771="strings cannot occur within patterns";_tag_dyneither(_tmp771,sizeof(char),37U);});Cyc_Warn_err(_tmpE41,_tmpE40,_tag_dyneither(_tmp770,sizeof(void*),0U));});});goto _LL6C0;case 6U: _LL6CF: _LL6D0:
# 2744
({void*_tmp772=0U;({unsigned int _tmpE43=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpE42=({const char*_tmp773="long long's in patterns not yet implemented";_tag_dyneither(_tmp773,sizeof(char),44U);});Cyc_Warn_err(_tmpE43,_tmpE42,_tag_dyneither(_tmp772,sizeof(void*),0U));});});goto _LL6C0;default: goto _LL6D1;}else{_LL6D1: _LL6D2:
# 2746
({void*_tmp774=0U;({unsigned int _tmpE45=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpE44=({const char*_tmp775="bad constant in case";_tag_dyneither(_tmp775,sizeof(char),21U);});Cyc_Warn_err(_tmpE45,_tmpE44,_tag_dyneither(_tmp774,sizeof(void*),0U));});});}_LL6C0:;}
# 2749
goto _LL254;}case 382U: _LL54B: _LL54C: {
# 2751
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2753
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2750 "parse.y"
({union Cyc_YYSTYPE _tmpE49=Cyc_YY9(({void*_tmpE48=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp77E=_cycalloc(sizeof(*_tmp77E));({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct _tmpE47=({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct _tmp77F;_tmp77F.tag=15U;({struct _tuple0*_tmpE46=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp77F.f1=_tmpE46;});_tmp77F;});_tmp77E[0]=_tmpE47;});_tmp77E;});Cyc_Absyn_new_pat(_tmpE48,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE49;});
goto _LL254;}case 383U: _LL54D: _LL54E: {
# 2753
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2755
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2752 "parse.y"
if(({struct _dyneither_ptr _tmpE4A=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]);Cyc_strcmp(_tmpE4A,({const char*_tmp780="as";_tag_dyneither(_tmp780,sizeof(char),3U);}));})!= 0)
({void*_tmp781=0U;({unsigned int _tmpE4C=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpE4B=({const char*_tmp782="expecting `as'";_tag_dyneither(_tmp782,sizeof(char),15U);});Cyc_Warn_err(_tmpE4C,_tmpE4B,_tag_dyneither(_tmp781,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpE54=Cyc_YY9(({void*_tmpE53=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp783=_cycalloc(sizeof(*_tmp783));({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpE52=({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp784;_tmp784.tag=1U;({struct Cyc_Absyn_Vardecl*_tmpE50=({unsigned int _tmpE4F=(unsigned int)(yyyylsp[0]).first_line;Cyc_Absyn_new_vardecl(_tmpE4F,({struct _tuple0*_tmp785=_cycalloc(sizeof(*_tmp785));_tmp785->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmpE4E=({struct _dyneither_ptr*_tmp786=_cycalloc(sizeof(*_tmp786));({struct _dyneither_ptr _tmpE4D=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp786[0]=_tmpE4D;});_tmp786;});_tmp785->f2=_tmpE4E;});_tmp785;}),(void*)& Cyc_Absyn_VoidType_val,0);});_tmp784.f1=_tmpE50;});({struct Cyc_Absyn_Pat*_tmpE51=
Cyc_yyget_YY9(yyyyvsp[2]);_tmp784.f2=_tmpE51;});_tmp784;});_tmp783[0]=_tmpE52;});_tmp783;});
# 2754
Cyc_Absyn_new_pat(_tmpE53,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE54;});
# 2757
goto _LL254;}case 384U: _LL54F: _LL550: {
# 2759
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2761
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2758 "parse.y"
if(({struct _dyneither_ptr _tmpE55=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_strcmp(_tmpE55,({const char*_tmp787="alias";_tag_dyneither(_tmp787,sizeof(char),6U);}));})!= 0)
({void*_tmp788=0U;({unsigned int _tmpE57=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpE56=({const char*_tmp789="expecting `alias'";_tag_dyneither(_tmp789,sizeof(char),18U);});Cyc_Warn_err(_tmpE57,_tmpE56,_tag_dyneither(_tmp788,sizeof(void*),0U));});});{
int _tmp78A=(yyyylsp[0]).first_line;
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp78F=_cycalloc(sizeof(*_tmp78F));({struct _dyneither_ptr*_tmpE59=({struct _dyneither_ptr*_tmp792=_cycalloc(sizeof(*_tmp792));({struct _dyneither_ptr _tmpE58=Cyc_yyget_String_tok(yyyyvsp[2]);_tmp792[0]=_tmpE58;});_tmp792;});_tmp78F->name=_tmpE59;});_tmp78F->identity=- 1;({void*_tmpE5B=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp790=_cycalloc(sizeof(*_tmp790));({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmpE5A=({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp791;_tmp791.tag=0U;_tmp791.f1=& Cyc_Tcutil_rk;_tmp791;});_tmp790[0]=_tmpE5A;});_tmp790;});_tmp78F->kind=_tmpE5B;});_tmp78F;});
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmpE60=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpE5F=({struct _tuple0*_tmp78D=_cycalloc(sizeof(*_tmp78D));_tmp78D->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmpE5D=({struct _dyneither_ptr*_tmp78E=_cycalloc(sizeof(*_tmp78E));({struct _dyneither_ptr _tmpE5C=Cyc_yyget_String_tok(yyyyvsp[5]);_tmp78E[0]=_tmpE5C;});_tmp78E;});_tmp78D->f2=_tmpE5D;});_tmp78D;});Cyc_Absyn_new_vardecl(_tmpE60,_tmpE5F,({
struct _tuple8*_tmpE5E=Cyc_yyget_YY37(yyyyvsp[4]);Cyc_Parse_type_name_to_type(_tmpE5E,(unsigned int)(yyyylsp[4]).first_line);}),0);});
({union Cyc_YYSTYPE _tmpE63=Cyc_YY9(({void*_tmpE62=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp78B=_cycalloc(sizeof(*_tmp78B));({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct _tmpE61=({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct _tmp78C;_tmp78C.tag=2U;_tmp78C.f1=tv;_tmp78C.f2=vd;_tmp78C;});_tmp78B[0]=_tmpE61;});_tmp78B;});Cyc_Absyn_new_pat(_tmpE62,(unsigned int)_tmp78A);}));yyval=_tmpE63;});
# 2766
goto _LL254;};}case 385U: _LL551: _LL552: {
# 2768
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2770
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2767 "parse.y"
struct _tuple22 _tmp793=*Cyc_yyget_YY10(yyyyvsp[2]);struct _tuple22 _tmp794=_tmp793;struct Cyc_List_List*_tmp798;int _tmp797;_LL6D4: _tmp798=_tmp794.f1;_tmp797=_tmp794.f2;_LL6D5:;
({union Cyc_YYSTYPE _tmpE66=Cyc_YY9(({void*_tmpE65=(void*)({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp795=_cycalloc(sizeof(*_tmp795));({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct _tmpE64=({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct _tmp796;_tmp796.tag=5U;_tmp796.f1=_tmp798;_tmp796.f2=_tmp797;_tmp796;});_tmp795[0]=_tmpE64;});_tmp795;});Cyc_Absyn_new_pat(_tmpE65,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE66;});
# 2770
goto _LL254;}case 386U: _LL553: _LL554: {
# 2772
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2774
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2771 "parse.y"
struct _tuple22 _tmp799=*Cyc_yyget_YY10(yyyyvsp[2]);struct _tuple22 _tmp79A=_tmp799;struct Cyc_List_List*_tmp79E;int _tmp79D;_LL6D7: _tmp79E=_tmp79A.f1;_tmp79D=_tmp79A.f2;_LL6D8:;
({union Cyc_YYSTYPE _tmpE6A=Cyc_YY9(({void*_tmpE69=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp79B=_cycalloc(sizeof(*_tmp79B));({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct _tmpE68=({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct _tmp79C;_tmp79C.tag=16U;({struct _tuple0*_tmpE67=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp79C.f1=_tmpE67;});_tmp79C.f2=_tmp79E;_tmp79C.f3=_tmp79D;_tmp79C;});_tmp79B[0]=_tmpE68;});_tmp79B;});Cyc_Absyn_new_pat(_tmpE69,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE6A;});
# 2774
goto _LL254;}case 387U: _LL555: _LL556: {
# 2776
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2778
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2775 "parse.y"
struct _tuple22 _tmp79F=*Cyc_yyget_YY14(yyyyvsp[3]);struct _tuple22 _tmp7A0=_tmp79F;struct Cyc_List_List*_tmp7A6;int _tmp7A5;_LL6DA: _tmp7A6=_tmp7A0.f1;_tmp7A5=_tmp7A0.f2;_LL6DB:;{
struct Cyc_List_List*_tmp7A1=({unsigned int _tmpE6B=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpE6B,Cyc_yyget_YY40(yyyyvsp[2]));});
({union Cyc_YYSTYPE _tmpE70=Cyc_YY9(({void*_tmpE6F=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7A2=_cycalloc(sizeof(*_tmp7A2));({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE6E=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp7A3;_tmp7A3.tag=7U;({struct Cyc_Absyn_AggrInfo*_tmpE6D=({struct Cyc_Absyn_AggrInfo*_tmp7A4=_cycalloc(sizeof(*_tmp7A4));({union Cyc_Absyn_AggrInfoU _tmpE6C=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0);_tmp7A4->aggr_info=_tmpE6C;});_tmp7A4->targs=0;_tmp7A4;});_tmp7A3.f1=_tmpE6D;});_tmp7A3.f2=_tmp7A1;_tmp7A3.f3=_tmp7A6;_tmp7A3.f4=_tmp7A5;_tmp7A3;});_tmp7A2[0]=_tmpE6E;});_tmp7A2;});Cyc_Absyn_new_pat(_tmpE6F,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE70;});
# 2780
goto _LL254;};}case 388U: _LL557: _LL558: {
# 2782
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2784
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2781 "parse.y"
struct _tuple22 _tmp7A7=*Cyc_yyget_YY14(yyyyvsp[2]);struct _tuple22 _tmp7A8=_tmp7A7;struct Cyc_List_List*_tmp7AD;int _tmp7AC;_LL6DD: _tmp7AD=_tmp7A8.f1;_tmp7AC=_tmp7A8.f2;_LL6DE:;{
struct Cyc_List_List*_tmp7A9=({unsigned int _tmpE71=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpE71,Cyc_yyget_YY40(yyyyvsp[1]));});
({union Cyc_YYSTYPE _tmpE74=Cyc_YY9(({void*_tmpE73=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7AA=_cycalloc(sizeof(*_tmp7AA));({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE72=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp7AB;_tmp7AB.tag=7U;_tmp7AB.f1=0;_tmp7AB.f2=_tmp7A9;_tmp7AB.f3=_tmp7AD;_tmp7AB.f4=_tmp7AC;_tmp7AB;});_tmp7AA[0]=_tmpE72;});_tmp7AA;});Cyc_Absyn_new_pat(_tmpE73,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE74;});
# 2785
goto _LL254;};}case 389U: _LL559: _LL55A: {
# 2787
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2789
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2786 "parse.y"
({union Cyc_YYSTYPE _tmpE78=Cyc_YY9(({void*_tmpE77=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7AE=_cycalloc(sizeof(*_tmp7AE));({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE76=({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmp7AF;_tmp7AF.tag=6U;({struct Cyc_Absyn_Pat*_tmpE75=Cyc_yyget_YY9(yyyyvsp[1]);_tmp7AF.f1=_tmpE75;});_tmp7AF;});_tmp7AE[0]=_tmpE76;});_tmp7AE;});Cyc_Absyn_new_pat(_tmpE77,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE78;});
goto _LL254;}case 390U: _LL55B: _LL55C: {
# 2789
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2791
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2788 "parse.y"
({union Cyc_YYSTYPE _tmpE7F=Cyc_YY9(({void*_tmpE7E=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7B0=_cycalloc(sizeof(*_tmp7B0));({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE7D=({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmp7B1;_tmp7B1.tag=6U;({struct Cyc_Absyn_Pat*_tmpE7C=({void*_tmpE7B=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7B2=_cycalloc(sizeof(*_tmp7B2));({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE7A=({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmp7B3;_tmp7B3.tag=6U;({struct Cyc_Absyn_Pat*_tmpE79=Cyc_yyget_YY9(yyyyvsp[1]);_tmp7B3.f1=_tmpE79;});_tmp7B3;});_tmp7B2[0]=_tmpE7A;});_tmp7B2;});Cyc_Absyn_new_pat(_tmpE7B,(unsigned int)(yyyylsp[0]).first_line);});_tmp7B1.f1=_tmpE7C;});_tmp7B1;});_tmp7B0[0]=_tmpE7D;});_tmp7B0;});Cyc_Absyn_new_pat(_tmpE7E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE7F;});
goto _LL254;}case 391U: _LL55D: _LL55E: {
# 2791
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2793
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2790 "parse.y"
({union Cyc_YYSTYPE _tmpE87=Cyc_YY9(({void*_tmpE86=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp7B4=_cycalloc(sizeof(*_tmp7B4));({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE85=({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmp7B5;_tmp7B5.tag=3U;({struct Cyc_Absyn_Vardecl*_tmpE83=({unsigned int _tmpE82=(unsigned int)(yyyylsp[0]).first_line;Cyc_Absyn_new_vardecl(_tmpE82,({struct _tuple0*_tmp7B6=_cycalloc(sizeof(*_tmp7B6));_tmp7B6->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmpE81=({struct _dyneither_ptr*_tmp7B7=_cycalloc(sizeof(*_tmp7B7));({struct _dyneither_ptr _tmpE80=Cyc_yyget_String_tok(yyyyvsp[1]);_tmp7B7[0]=_tmpE80;});_tmp7B7;});_tmp7B6->f2=_tmpE81;});_tmp7B6;}),(void*)& Cyc_Absyn_VoidType_val,0);});_tmp7B5.f1=_tmpE83;});({struct Cyc_Absyn_Pat*_tmpE84=
# 2792
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line);_tmp7B5.f2=_tmpE84;});_tmp7B5;});_tmp7B4[0]=_tmpE85;});_tmp7B4;});
# 2790
Cyc_Absyn_new_pat(_tmpE86,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE87;});
# 2794
goto _LL254;}case 392U: _LL55F: _LL560: {
# 2796
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2798
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2795 "parse.y"
if(({struct _dyneither_ptr _tmpE88=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]);Cyc_strcmp(_tmpE88,({const char*_tmp7B8="as";_tag_dyneither(_tmp7B8,sizeof(char),3U);}));})!= 0)
({void*_tmp7B9=0U;({unsigned int _tmpE8A=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmpE89=({const char*_tmp7BA="expecting `as'";_tag_dyneither(_tmp7BA,sizeof(char),15U);});Cyc_Warn_err(_tmpE8A,_tmpE89,_tag_dyneither(_tmp7B9,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpE92=Cyc_YY9(({void*_tmpE91=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp7BB=_cycalloc(sizeof(*_tmp7BB));({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE90=({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmp7BC;_tmp7BC.tag=3U;({struct Cyc_Absyn_Vardecl*_tmpE8E=({unsigned int _tmpE8D=(unsigned int)(yyyylsp[0]).first_line;Cyc_Absyn_new_vardecl(_tmpE8D,({struct _tuple0*_tmp7BD=_cycalloc(sizeof(*_tmp7BD));_tmp7BD->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmpE8C=({struct _dyneither_ptr*_tmp7BE=_cycalloc(sizeof(*_tmp7BE));({struct _dyneither_ptr _tmpE8B=Cyc_yyget_String_tok(yyyyvsp[1]);_tmp7BE[0]=_tmpE8B;});_tmp7BE;});_tmp7BD->f2=_tmpE8C;});_tmp7BD;}),(void*)& Cyc_Absyn_VoidType_val,0);});_tmp7BC.f1=_tmpE8E;});({struct Cyc_Absyn_Pat*_tmpE8F=
# 2799
Cyc_yyget_YY9(yyyyvsp[3]);_tmp7BC.f2=_tmpE8F;});_tmp7BC;});_tmp7BB[0]=_tmpE90;});_tmp7BB;});
# 2797
Cyc_Absyn_new_pat(_tmpE91,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE92;});
# 2801
goto _LL254;}case 393U: _LL561: _LL562: {
# 2803
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2805
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2802 "parse.y"
void*_tmp7BF=({struct _dyneither_ptr _tmpE93=Cyc_yyget_String_tok(yyyyvsp[2]);Cyc_Parse_id2type(_tmpE93,Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));});
({union Cyc_YYSTYPE _tmpE9D=Cyc_YY9(({void*_tmpE9C=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp7C0=_cycalloc(sizeof(*_tmp7C0));({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE9B=({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmp7C1;_tmp7C1.tag=4U;({struct Cyc_Absyn_Tvar*_tmpE94=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp7BF);_tmp7C1.f1=_tmpE94;});({struct Cyc_Absyn_Vardecl*_tmpE9A=({
unsigned int _tmpE99=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpE98=({struct _tuple0*_tmp7C2=_cycalloc(sizeof(*_tmp7C2));_tmp7C2->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmpE96=({struct _dyneither_ptr*_tmp7C3=_cycalloc(sizeof(*_tmp7C3));({struct _dyneither_ptr _tmpE95=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp7C3[0]=_tmpE95;});_tmp7C3;});_tmp7C2->f2=_tmpE96;});_tmp7C2;});Cyc_Absyn_new_vardecl(_tmpE99,_tmpE98,(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp7C4=_cycalloc(sizeof(*_tmp7C4));({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE97=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp7C5;_tmp7C5.tag=19U;_tmp7C5.f1=_tmp7BF;_tmp7C5;});_tmp7C4[0]=_tmpE97;});_tmp7C4;}),0);});_tmp7C1.f2=_tmpE9A;});_tmp7C1;});_tmp7C0[0]=_tmpE9B;});_tmp7C0;});
# 2803
Cyc_Absyn_new_pat(_tmpE9C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE9D;});
# 2807
goto _LL254;}case 394U: _LL563: _LL564: {
# 2809
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2811
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2808 "parse.y"
struct Cyc_Absyn_Tvar*_tmp7C6=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
({union Cyc_YYSTYPE _tmpEA8=Cyc_YY9(({void*_tmpEA7=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp7C7=_cycalloc(sizeof(*_tmp7C7));({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpEA6=({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmp7C8;_tmp7C8.tag=4U;_tmp7C8.f1=_tmp7C6;({struct Cyc_Absyn_Vardecl*_tmpEA5=({
unsigned int _tmpEA4=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpEA3=({struct _tuple0*_tmp7C9=_cycalloc(sizeof(*_tmp7C9));_tmp7C9->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmpE9F=({struct _dyneither_ptr*_tmp7CA=_cycalloc(sizeof(*_tmp7CA));({struct _dyneither_ptr _tmpE9E=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp7CA[0]=_tmpE9E;});_tmp7CA;});_tmp7C9->f2=_tmpE9F;});_tmp7C9;});Cyc_Absyn_new_vardecl(_tmpEA4,_tmpEA3,(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp7CB=_cycalloc(sizeof(*_tmp7CB));({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpEA2=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp7CC;_tmp7CC.tag=19U;({void*_tmpEA1=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp7CD=_cycalloc(sizeof(*_tmp7CD));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpEA0=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp7CE;_tmp7CE.tag=2U;_tmp7CE.f1=_tmp7C6;_tmp7CE;});_tmp7CD[0]=_tmpEA0;});_tmp7CD;});_tmp7CC.f1=_tmpEA1;});_tmp7CC;});_tmp7CB[0]=_tmpEA2;});_tmp7CB;}),0);});_tmp7C8.f2=_tmpEA5;});_tmp7C8;});_tmp7C7[0]=_tmpEA6;});_tmp7C7;});
# 2809
Cyc_Absyn_new_pat(_tmpEA7,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEA8;});
# 2813
goto _LL254;}case 395U: _LL565: _LL566: {
# 2815
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2817
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2816 "parse.y"
({union Cyc_YYSTYPE _tmpEAA=Cyc_YY10(({struct _tuple22*_tmp7CF=_cycalloc(sizeof(*_tmp7CF));({struct Cyc_List_List*_tmpEA9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0]));_tmp7CF->f1=_tmpEA9;});_tmp7CF->f2=0;_tmp7CF;}));yyval=_tmpEAA;});
goto _LL254;}case 396U: _LL567: _LL568: {
# 2819
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2821
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2817 "parse.y"
({union Cyc_YYSTYPE _tmpEAC=Cyc_YY10(({struct _tuple22*_tmp7D0=_cycalloc(sizeof(*_tmp7D0));({struct Cyc_List_List*_tmpEAB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0]));_tmp7D0->f1=_tmpEAB;});_tmp7D0->f2=1;_tmp7D0;}));yyval=_tmpEAC;});
goto _LL254;}case 397U: _LL569: _LL56A: {
# 2820
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2822
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2818 "parse.y"
({union Cyc_YYSTYPE _tmpEAD=Cyc_YY10(({struct _tuple22*_tmp7D1=_cycalloc(sizeof(*_tmp7D1));_tmp7D1->f1=0;_tmp7D1->f2=1;_tmp7D1;}));yyval=_tmpEAD;});
goto _LL254;}case 398U: _LL56B: _LL56C: {
# 2821
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2823
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2823 "parse.y"
({union Cyc_YYSTYPE _tmpEAF=Cyc_YY11(({struct Cyc_List_List*_tmp7D2=_cycalloc(sizeof(*_tmp7D2));({struct Cyc_Absyn_Pat*_tmpEAE=Cyc_yyget_YY9(yyyyvsp[0]);_tmp7D2->hd=_tmpEAE;});_tmp7D2->tl=0;_tmp7D2;}));yyval=_tmpEAF;});
goto _LL254;}case 399U: _LL56D: _LL56E: {
# 2826
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2828
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2825 "parse.y"
({union Cyc_YYSTYPE _tmpEB2=Cyc_YY11(({struct Cyc_List_List*_tmp7D3=_cycalloc(sizeof(*_tmp7D3));({struct Cyc_Absyn_Pat*_tmpEB0=Cyc_yyget_YY9(yyyyvsp[2]);_tmp7D3->hd=_tmpEB0;});({struct Cyc_List_List*_tmpEB1=Cyc_yyget_YY11(yyyyvsp[0]);_tmp7D3->tl=_tmpEB1;});_tmp7D3;}));yyval=_tmpEB2;});
goto _LL254;}case 400U: _LL56F: _LL570: {
# 2828
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2830
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2830 "parse.y"
({union Cyc_YYSTYPE _tmpEB4=Cyc_YY12(({struct _tuple23*_tmp7D4=_cycalloc(sizeof(*_tmp7D4));_tmp7D4->f1=0;({struct Cyc_Absyn_Pat*_tmpEB3=Cyc_yyget_YY9(yyyyvsp[0]);_tmp7D4->f2=_tmpEB3;});_tmp7D4;}));yyval=_tmpEB4;});
goto _LL254;}case 401U: _LL571: _LL572: {
# 2833
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2835
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2832 "parse.y"
({union Cyc_YYSTYPE _tmpEB7=Cyc_YY12(({struct _tuple23*_tmp7D5=_cycalloc(sizeof(*_tmp7D5));({struct Cyc_List_List*_tmpEB5=Cyc_yyget_YY41(yyyyvsp[0]);_tmp7D5->f1=_tmpEB5;});({struct Cyc_Absyn_Pat*_tmpEB6=Cyc_yyget_YY9(yyyyvsp[1]);_tmp7D5->f2=_tmpEB6;});_tmp7D5;}));yyval=_tmpEB7;});
goto _LL254;}case 402U: _LL573: _LL574: {
# 2835
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2837
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2835 "parse.y"
({union Cyc_YYSTYPE _tmpEB9=Cyc_YY14(({struct _tuple22*_tmp7D6=_cycalloc(sizeof(*_tmp7D6));({struct Cyc_List_List*_tmpEB8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0]));_tmp7D6->f1=_tmpEB8;});_tmp7D6->f2=0;_tmp7D6;}));yyval=_tmpEB9;});
goto _LL254;}case 403U: _LL575: _LL576: {
# 2838
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2840
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2836 "parse.y"
({union Cyc_YYSTYPE _tmpEBB=Cyc_YY14(({struct _tuple22*_tmp7D7=_cycalloc(sizeof(*_tmp7D7));({struct Cyc_List_List*_tmpEBA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0]));_tmp7D7->f1=_tmpEBA;});_tmp7D7->f2=1;_tmp7D7;}));yyval=_tmpEBB;});
goto _LL254;}case 404U: _LL577: _LL578: {
# 2839
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2841
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2837 "parse.y"
({union Cyc_YYSTYPE _tmpEBC=Cyc_YY14(({struct _tuple22*_tmp7D8=_cycalloc(sizeof(*_tmp7D8));_tmp7D8->f1=0;_tmp7D8->f2=1;_tmp7D8;}));yyval=_tmpEBC;});
goto _LL254;}case 405U: _LL579: _LL57A: {
# 2840
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2842
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2842 "parse.y"
({union Cyc_YYSTYPE _tmpEBE=Cyc_YY13(({struct Cyc_List_List*_tmp7D9=_cycalloc(sizeof(*_tmp7D9));({struct _tuple23*_tmpEBD=Cyc_yyget_YY12(yyyyvsp[0]);_tmp7D9->hd=_tmpEBD;});_tmp7D9->tl=0;_tmp7D9;}));yyval=_tmpEBE;});
goto _LL254;}case 406U: _LL57B: _LL57C: {
# 2845
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2847
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2844 "parse.y"
({union Cyc_YYSTYPE _tmpEC1=Cyc_YY13(({struct Cyc_List_List*_tmp7DA=_cycalloc(sizeof(*_tmp7DA));({struct _tuple23*_tmpEBF=Cyc_yyget_YY12(yyyyvsp[2]);_tmp7DA->hd=_tmpEBF;});({struct Cyc_List_List*_tmpEC0=Cyc_yyget_YY13(yyyyvsp[0]);_tmp7DA->tl=_tmpEC0;});_tmp7DA;}));yyval=_tmpEC1;});
goto _LL254;}case 407U: _LL57D: _LL57E: {
# 2847
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2849
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=yyyyvsp[0];
goto _LL254;}case 408U: _LL57F: _LL580: {
# 2853
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2855
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2852 "parse.y"
({union Cyc_YYSTYPE _tmpEC4=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEC3=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpEC2=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_seq_exp(_tmpEC3,_tmpEC2,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEC4;});
goto _LL254;}case 409U: _LL581: _LL582: {
# 2855
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2857
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2857 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 410U: _LL583: _LL584: {
# 2860
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2862
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2859 "parse.y"
({union Cyc_YYSTYPE _tmpEC8=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEC7=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Core_Opt*_tmpEC6=Cyc_yyget_YY7(yyyyvsp[1]);struct Cyc_Absyn_Exp*_tmpEC5=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_assignop_exp(_tmpEC7,_tmpEC6,_tmpEC5,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEC8;});
goto _LL254;}case 411U: _LL585: _LL586: {
# 2862
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2864
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2861 "parse.y"
({union Cyc_YYSTYPE _tmpECB=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpECA=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpEC9=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_swap_exp(_tmpECA,_tmpEC9,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpECB;});
goto _LL254;}case 412U: _LL587: _LL588: {
# 2864
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2866
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2865 "parse.y"
({union Cyc_YYSTYPE _tmpECC=Cyc_YY7(0);yyval=_tmpECC;});
goto _LL254;}case 413U: _LL589: _LL58A: {
# 2868
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2870
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2866 "parse.y"
({union Cyc_YYSTYPE _tmpECD=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7DB=_cycalloc_atomic(sizeof(*_tmp7DB));_tmp7DB->v=(void*)Cyc_Absyn_Times;_tmp7DB;}));yyval=_tmpECD;});
goto _LL254;}case 414U: _LL58B: _LL58C: {
# 2869
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2871
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2867 "parse.y"
({union Cyc_YYSTYPE _tmpECE=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7DC=_cycalloc_atomic(sizeof(*_tmp7DC));_tmp7DC->v=(void*)Cyc_Absyn_Div;_tmp7DC;}));yyval=_tmpECE;});
goto _LL254;}case 415U: _LL58D: _LL58E: {
# 2870
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2872
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2868 "parse.y"
({union Cyc_YYSTYPE _tmpECF=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7DD=_cycalloc_atomic(sizeof(*_tmp7DD));_tmp7DD->v=(void*)Cyc_Absyn_Mod;_tmp7DD;}));yyval=_tmpECF;});
goto _LL254;}case 416U: _LL58F: _LL590: {
# 2871
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2873
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2869 "parse.y"
({union Cyc_YYSTYPE _tmpED0=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7DE=_cycalloc_atomic(sizeof(*_tmp7DE));_tmp7DE->v=(void*)Cyc_Absyn_Plus;_tmp7DE;}));yyval=_tmpED0;});
goto _LL254;}case 417U: _LL591: _LL592: {
# 2872
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2874
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2870 "parse.y"
({union Cyc_YYSTYPE _tmpED1=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7DF=_cycalloc_atomic(sizeof(*_tmp7DF));_tmp7DF->v=(void*)Cyc_Absyn_Minus;_tmp7DF;}));yyval=_tmpED1;});
goto _LL254;}case 418U: _LL593: _LL594: {
# 2873
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2875
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2871 "parse.y"
({union Cyc_YYSTYPE _tmpED2=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7E0=_cycalloc_atomic(sizeof(*_tmp7E0));_tmp7E0->v=(void*)Cyc_Absyn_Bitlshift;_tmp7E0;}));yyval=_tmpED2;});
goto _LL254;}case 419U: _LL595: _LL596: {
# 2874
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2876
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2872 "parse.y"
({union Cyc_YYSTYPE _tmpED3=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7E1=_cycalloc_atomic(sizeof(*_tmp7E1));_tmp7E1->v=(void*)Cyc_Absyn_Bitlrshift;_tmp7E1;}));yyval=_tmpED3;});
goto _LL254;}case 420U: _LL597: _LL598: {
# 2875
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2877
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2873 "parse.y"
({union Cyc_YYSTYPE _tmpED4=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7E2=_cycalloc_atomic(sizeof(*_tmp7E2));_tmp7E2->v=(void*)Cyc_Absyn_Bitand;_tmp7E2;}));yyval=_tmpED4;});
goto _LL254;}case 421U: _LL599: _LL59A: {
# 2876
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2878
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2874 "parse.y"
({union Cyc_YYSTYPE _tmpED5=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7E3=_cycalloc_atomic(sizeof(*_tmp7E3));_tmp7E3->v=(void*)Cyc_Absyn_Bitxor;_tmp7E3;}));yyval=_tmpED5;});
goto _LL254;}case 422U: _LL59B: _LL59C: {
# 2877
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2879
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2875 "parse.y"
({union Cyc_YYSTYPE _tmpED6=Cyc_YY7(({struct Cyc_Core_Opt*_tmp7E4=_cycalloc_atomic(sizeof(*_tmp7E4));_tmp7E4->v=(void*)Cyc_Absyn_Bitor;_tmp7E4;}));yyval=_tmpED6;});
goto _LL254;}case 423U: _LL59D: _LL59E: {
# 2878
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2880
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2880 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 424U: _LL59F: _LL5A0: {
# 2883
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2885
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2882 "parse.y"
({union Cyc_YYSTYPE _tmpEDA=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpED9=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpED8=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpED7=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_conditional_exp(_tmpED9,_tmpED8,_tmpED7,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEDA;});
goto _LL254;}case 425U: _LL5A1: _LL5A2: {
# 2885
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2887
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2885 "parse.y"
({union Cyc_YYSTYPE _tmpEDC=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEDB=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_throw_exp(_tmpEDB,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEDC;});
goto _LL254;}case 426U: _LL5A3: _LL5A4: {
# 2888
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2888 "parse.y"
({union Cyc_YYSTYPE _tmpEDE=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEDD=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_New_exp(0,_tmpEDD,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEDE;});
goto _LL254;}case 427U: _LL5A5: _LL5A6: {
# 2891
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2893
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2890 "parse.y"
({union Cyc_YYSTYPE _tmpEE0=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEDF=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_New_exp(0,_tmpEDF,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEE0;});
goto _LL254;}case 428U: _LL5A7: _LL5A8: {
# 2893
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2895
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2892 "parse.y"
({union Cyc_YYSTYPE _tmpEE3=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEE2=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpEE1=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_New_exp(_tmpEE2,_tmpEE1,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEE3;});
goto _LL254;}case 429U: _LL5A9: _LL5AA: {
# 2895
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2897
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2894 "parse.y"
({union Cyc_YYSTYPE _tmpEE6=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEE5=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpEE4=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_New_exp(_tmpEE5,_tmpEE4,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEE6;});
goto _LL254;}case 430U: _LL5AB: _LL5AC: {
# 2897
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2899
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2898 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 431U: _LL5AD: _LL5AE: {
# 2901
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2903
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2902 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 432U: _LL5AF: _LL5B0: {
# 2905
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2907
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2904 "parse.y"
({union Cyc_YYSTYPE _tmpEE9=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEE8=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpEE7=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_or_exp(_tmpEE8,_tmpEE7,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEE9;});
goto _LL254;}case 433U: _LL5B1: _LL5B2: {
# 2907
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2909
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2908 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 434U: _LL5B3: _LL5B4: {
# 2911
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2913
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2910 "parse.y"
({union Cyc_YYSTYPE _tmpEEC=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEEB=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpEEA=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_and_exp(_tmpEEB,_tmpEEA,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEEC;});
goto _LL254;}case 435U: _LL5B5: _LL5B6: {
# 2913
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2915
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2914 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 436U: _LL5B7: _LL5B8: {
# 2917
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2919
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2916 "parse.y"
({union Cyc_YYSTYPE _tmpEEF=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEEE=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpEED=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpEEE,_tmpEED,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEEF;});
goto _LL254;}case 437U: _LL5B9: _LL5BA: {
# 2919
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2921
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2920 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 438U: _LL5BB: _LL5BC: {
# 2923
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2925
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2922 "parse.y"
({union Cyc_YYSTYPE _tmpEF2=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEF1=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpEF0=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpEF1,_tmpEF0,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEF2;});
goto _LL254;}case 439U: _LL5BD: _LL5BE: {
# 2925
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2927
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2926 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 440U: _LL5BF: _LL5C0: {
# 2929
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2931
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2928 "parse.y"
({union Cyc_YYSTYPE _tmpEF5=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEF4=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpEF3=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpEF4,_tmpEF3,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEF5;});
goto _LL254;}case 441U: _LL5C1: _LL5C2: {
# 2931
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2933
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2932 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 442U: _LL5C3: _LL5C4: {
# 2935
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2937
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2934 "parse.y"
({union Cyc_YYSTYPE _tmpEF8=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEF7=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpEF6=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_eq_exp(_tmpEF7,_tmpEF6,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEF8;});
goto _LL254;}case 443U: _LL5C5: _LL5C6: {
# 2937
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2939
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2936 "parse.y"
({union Cyc_YYSTYPE _tmpEFB=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEFA=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpEF9=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_neq_exp(_tmpEFA,_tmpEF9,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEFB;});
goto _LL254;}case 444U: _LL5C7: _LL5C8: {
# 2939
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2941
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2940 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 445U: _LL5C9: _LL5CA: {
# 2943
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2945
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2942 "parse.y"
({union Cyc_YYSTYPE _tmpEFE=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpEFD=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpEFC=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lt_exp(_tmpEFD,_tmpEFC,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpEFE;});
goto _LL254;}case 446U: _LL5CB: _LL5CC: {
# 2945
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2947
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2944 "parse.y"
({union Cyc_YYSTYPE _tmpF01=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF00=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpEFF=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gt_exp(_tmpF00,_tmpEFF,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF01;});
goto _LL254;}case 447U: _LL5CD: _LL5CE: {
# 2947
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2949
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2946 "parse.y"
({union Cyc_YYSTYPE _tmpF04=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF03=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpF02=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lte_exp(_tmpF03,_tmpF02,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF04;});
goto _LL254;}case 448U: _LL5CF: _LL5D0: {
# 2949
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2948 "parse.y"
({union Cyc_YYSTYPE _tmpF07=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF06=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpF05=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gte_exp(_tmpF06,_tmpF05,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF07;});
goto _LL254;}case 449U: _LL5D1: _LL5D2: {
# 2951
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2953
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2952 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 450U: _LL5D3: _LL5D4: {
# 2955
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2957
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2954 "parse.y"
({union Cyc_YYSTYPE _tmpF0A=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF09=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpF08=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpF09,_tmpF08,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF0A;});
goto _LL254;}case 451U: _LL5D5: _LL5D6: {
# 2957
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2959
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2956 "parse.y"
({union Cyc_YYSTYPE _tmpF0D=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF0C=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpF0B=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpF0C,_tmpF0B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF0D;});
goto _LL254;}case 452U: _LL5D7: _LL5D8: {
# 2959
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2961
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2960 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 453U: _LL5D9: _LL5DA: {
# 2963
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2962 "parse.y"
({union Cyc_YYSTYPE _tmpF10=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF0F=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpF0E=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpF0F,_tmpF0E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF10;});
goto _LL254;}case 454U: _LL5DB: _LL5DC: {
# 2965
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2964 "parse.y"
({union Cyc_YYSTYPE _tmpF13=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF12=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpF11=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpF12,_tmpF11,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF13;});
goto _LL254;}case 455U: _LL5DD: _LL5DE: {
# 2967
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2969
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2968 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 456U: _LL5DF: _LL5E0: {
# 2971
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2973
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2970 "parse.y"
({union Cyc_YYSTYPE _tmpF16=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF15=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpF14=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpF15,_tmpF14,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF16;});
goto _LL254;}case 457U: _LL5E1: _LL5E2: {
# 2973
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2975
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2972 "parse.y"
({union Cyc_YYSTYPE _tmpF19=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF18=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpF17=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpF18,_tmpF17,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF19;});
goto _LL254;}case 458U: _LL5E3: _LL5E4: {
# 2975
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2977
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2974 "parse.y"
({union Cyc_YYSTYPE _tmpF1C=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF1B=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpF1A=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpF1B,_tmpF1A,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF1C;});
goto _LL254;}case 459U: _LL5E5: _LL5E6: {
# 2977
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2979
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2978 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 460U: _LL5E7: _LL5E8: {
# 2981
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2983
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2980 "parse.y"
void*_tmp7E5=({struct _tuple8*_tmpF1D=Cyc_yyget_YY37(yyyyvsp[1]);Cyc_Parse_type_name_to_type(_tmpF1D,(unsigned int)(yyyylsp[1]).first_line);});
({union Cyc_YYSTYPE _tmpF20=Cyc_Exp_tok(({void*_tmpF1F=_tmp7E5;struct Cyc_Absyn_Exp*_tmpF1E=Cyc_yyget_Exp_tok(yyyyvsp[3]);Cyc_Absyn_cast_exp(_tmpF1F,_tmpF1E,1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF20;});
# 2983
goto _LL254;}case 461U: _LL5E9: _LL5EA: {
# 2985
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2987
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2986 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 462U: _LL5EB: _LL5EC: {
# 2989
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2991
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2987 "parse.y"
({union Cyc_YYSTYPE _tmpF22=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF21=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_pre_inc_exp(_tmpF21,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF22;});
goto _LL254;}case 463U: _LL5ED: _LL5EE: {
# 2990
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2992
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2988 "parse.y"
({union Cyc_YYSTYPE _tmpF24=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF23=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_pre_dec_exp(_tmpF23,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF24;});
goto _LL254;}case 464U: _LL5EF: _LL5F0: {
# 2991
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2993
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2989 "parse.y"
({union Cyc_YYSTYPE _tmpF26=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF25=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_address_exp(_tmpF25,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF26;});
goto _LL254;}case 465U: _LL5F1: _LL5F2: {
# 2992
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2994
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2990 "parse.y"
({union Cyc_YYSTYPE _tmpF28=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF27=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_deref_exp(_tmpF27,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF28;});
goto _LL254;}case 466U: _LL5F3: _LL5F4: {
# 2993
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2995
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2991 "parse.y"
({union Cyc_YYSTYPE _tmpF2A=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF29=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,_tmpF29,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF2A;});
goto _LL254;}case 467U: _LL5F5: _LL5F6: {
# 2994
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2996
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2992 "parse.y"
({union Cyc_YYSTYPE _tmpF2D=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _tmpF2C=Cyc_yyget_YY6(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpF2B=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_prim1_exp(_tmpF2C,_tmpF2B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF2D;});
goto _LL254;}case 468U: _LL5F7: _LL5F8: {
# 2995
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2997
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2994 "parse.y"
void*_tmp7E6=({struct _tuple8*_tmpF2E=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpF2E,(unsigned int)(yyyylsp[2]).first_line);});
({union Cyc_YYSTYPE _tmpF2F=Cyc_Exp_tok(Cyc_Absyn_sizeoftyp_exp(_tmp7E6,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpF2F;});
# 2997
goto _LL254;}case 469U: _LL5F9: _LL5FA: {
# 2999
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3001
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2997 "parse.y"
({union Cyc_YYSTYPE _tmpF31=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF30=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_sizeofexp_exp(_tmpF30,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF31;});
goto _LL254;}case 470U: _LL5FB: _LL5FC: {
# 3000
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3002
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2999 "parse.y"
void*_tmp7E7=({struct _tuple8*_tmpF32=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpF32,(unsigned int)(yyyylsp[2]).first_line);});
({union Cyc_YYSTYPE _tmpF35=Cyc_Exp_tok(({void*_tmpF34=_tmp7E7;struct Cyc_List_List*_tmpF33=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4]));Cyc_Absyn_offsetof_exp(_tmpF34,_tmpF33,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF35;});
# 3002
goto _LL254;}case 471U: _LL5FD: _LL5FE: {
# 3004
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3006
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3004 "parse.y"
({union Cyc_YYSTYPE _tmpF3A=Cyc_Exp_tok(({void*_tmpF39=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp7E8=_cycalloc(sizeof(*_tmp7E8));({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpF38=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp7E9;_tmp7E9.tag=34U;({struct Cyc_Absyn_MallocInfo _tmpF37=({struct Cyc_Absyn_MallocInfo _tmp7EA;_tmp7EA.is_calloc=0;_tmp7EA.rgn=0;_tmp7EA.elt_type=0;({struct Cyc_Absyn_Exp*_tmpF36=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7EA.num_elts=_tmpF36;});_tmp7EA.fat_result=0;_tmp7EA.inline_call=0;_tmp7EA;});_tmp7E9.f1=_tmpF37;});_tmp7E9;});_tmp7E8[0]=_tmpF38;});_tmp7E8;});Cyc_Absyn_new_exp(_tmpF39,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF3A;});
# 3006
goto _LL254;}case 472U: _LL5FF: _LL600: {
# 3008
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3010
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3007 "parse.y"
({union Cyc_YYSTYPE _tmpF40=Cyc_Exp_tok(({void*_tmpF3F=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp7EB=_cycalloc(sizeof(*_tmp7EB));({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpF3E=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp7EC;_tmp7EC.tag=34U;({struct Cyc_Absyn_MallocInfo _tmpF3D=({struct Cyc_Absyn_MallocInfo _tmp7ED;_tmp7ED.is_calloc=0;({struct Cyc_Absyn_Exp*_tmpF3B=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7ED.rgn=_tmpF3B;});_tmp7ED.elt_type=0;({struct Cyc_Absyn_Exp*_tmpF3C=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp7ED.num_elts=_tmpF3C;});_tmp7ED.fat_result=0;_tmp7ED.inline_call=0;_tmp7ED;});_tmp7EC.f1=_tmpF3D;});_tmp7EC;});_tmp7EB[0]=_tmpF3E;});_tmp7EB;});Cyc_Absyn_new_exp(_tmpF3F,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF40;});
# 3009
goto _LL254;}case 473U: _LL601: _LL602: {
# 3011
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3013
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3010 "parse.y"
({union Cyc_YYSTYPE _tmpF46=Cyc_Exp_tok(({void*_tmpF45=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp7EE=_cycalloc(sizeof(*_tmp7EE));({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpF44=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp7EF;_tmp7EF.tag=34U;({struct Cyc_Absyn_MallocInfo _tmpF43=({struct Cyc_Absyn_MallocInfo _tmp7F0;_tmp7F0.is_calloc=0;({struct Cyc_Absyn_Exp*_tmpF41=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7F0.rgn=_tmpF41;});_tmp7F0.elt_type=0;({struct Cyc_Absyn_Exp*_tmpF42=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp7F0.num_elts=_tmpF42;});_tmp7F0.fat_result=0;_tmp7F0.inline_call=1;_tmp7F0;});_tmp7EF.f1=_tmpF43;});_tmp7EF;});_tmp7EE[0]=_tmpF44;});_tmp7EE;});Cyc_Absyn_new_exp(_tmpF45,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF46;});
# 3012
goto _LL254;}case 474U: _LL603: _LL604: {
# 3014
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9U));
# 3016
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9U));
# 3013 "parse.y"
void*_tmp7F1=({struct _tuple8*_tmpF47=Cyc_yyget_YY37(yyyyvsp[6]);Cyc_Parse_type_name_to_type(_tmpF47,(unsigned int)(yyyylsp[6]).first_line);});
({union Cyc_YYSTYPE _tmpF4D=Cyc_Exp_tok(({void*_tmpF4C=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp7F2=_cycalloc(sizeof(*_tmp7F2));({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpF4B=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp7F3;_tmp7F3.tag=34U;({struct Cyc_Absyn_MallocInfo _tmpF4A=({struct Cyc_Absyn_MallocInfo _tmp7F4;_tmp7F4.is_calloc=1;_tmp7F4.rgn=0;({void**_tmpF48=({void**_tmp7F5=_cycalloc(sizeof(*_tmp7F5));_tmp7F5[0]=_tmp7F1;_tmp7F5;});_tmp7F4.elt_type=_tmpF48;});({struct Cyc_Absyn_Exp*_tmpF49=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7F4.num_elts=_tmpF49;});_tmp7F4.fat_result=0;_tmp7F4.inline_call=0;_tmp7F4;});_tmp7F3.f1=_tmpF4A;});_tmp7F3;});_tmp7F2[0]=_tmpF4B;});_tmp7F2;});Cyc_Absyn_new_exp(_tmpF4C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF4D;});
# 3016
goto _LL254;}case 475U: _LL605: _LL606: {
# 3018
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11U));
# 3020
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11U));
# 3018 "parse.y"
void*_tmp7F6=({struct _tuple8*_tmpF4E=Cyc_yyget_YY37(yyyyvsp[8]);Cyc_Parse_type_name_to_type(_tmpF4E,(unsigned int)(yyyylsp[8]).first_line);});
({union Cyc_YYSTYPE _tmpF55=Cyc_Exp_tok(({void*_tmpF54=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp7F7=_cycalloc(sizeof(*_tmp7F7));({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpF53=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp7F8;_tmp7F8.tag=34U;({struct Cyc_Absyn_MallocInfo _tmpF52=({struct Cyc_Absyn_MallocInfo _tmp7F9;_tmp7F9.is_calloc=1;({struct Cyc_Absyn_Exp*_tmpF4F=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7F9.rgn=_tmpF4F;});({void**_tmpF50=({void**_tmp7FA=_cycalloc(sizeof(*_tmp7FA));_tmp7FA[0]=_tmp7F6;_tmp7FA;});_tmp7F9.elt_type=_tmpF50;});({struct Cyc_Absyn_Exp*_tmpF51=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp7F9.num_elts=_tmpF51;});_tmp7F9.fat_result=0;_tmp7F9.inline_call=0;_tmp7F9;});_tmp7F8.f1=_tmpF52;});_tmp7F8;});_tmp7F7[0]=_tmpF53;});_tmp7F7;});Cyc_Absyn_new_exp(_tmpF54,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF55;});
# 3021
goto _LL254;}case 476U: _LL607: _LL608: {
# 3023
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3025
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3022 "parse.y"
({union Cyc_YYSTYPE _tmpF58=Cyc_Exp_tok(({struct Cyc_List_List*_tmpF57=({struct Cyc_Absyn_Exp*_tmp7FB[1U];({struct Cyc_Absyn_Exp*_tmpF56=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7FB[0U]=_tmpF56;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp7FB,sizeof(struct Cyc_Absyn_Exp*),1U));});Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,_tmpF57,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF58;});
goto _LL254;}case 477U: _LL609: _LL60A: {
# 3025
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3027
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3024 "parse.y"
({union Cyc_YYSTYPE _tmpF5E=Cyc_Exp_tok(({void*_tmpF5D=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp7FC=_cycalloc(sizeof(*_tmp7FC));({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpF5C=({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmp7FD;_tmp7FD.tag=38U;({struct Cyc_Absyn_Exp*_tmpF59=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp7FD.f1=_tmpF59;});({struct _dyneither_ptr*_tmpF5B=({struct _dyneither_ptr*_tmp7FE=_cycalloc(sizeof(*_tmp7FE));({struct _dyneither_ptr _tmpF5A=Cyc_yyget_String_tok(yyyyvsp[4]);_tmp7FE[0]=_tmpF5A;});_tmp7FE;});_tmp7FD.f2=_tmpF5B;});_tmp7FD;});_tmp7FC[0]=_tmpF5C;});_tmp7FC;});Cyc_Absyn_new_exp(_tmpF5D,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF5E;});
goto _LL254;}case 478U: _LL60B: _LL60C: {
# 3027
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3029
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3026 "parse.y"
({union Cyc_YYSTYPE _tmpF65=Cyc_Exp_tok(({void*_tmpF64=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp7FF=_cycalloc(sizeof(*_tmp7FF));({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpF63=({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmp800;_tmp800.tag=38U;({struct Cyc_Absyn_Exp*_tmpF60=({struct Cyc_Absyn_Exp*_tmpF5F=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_deref_exp(_tmpF5F,(unsigned int)(yyyylsp[2]).first_line);});_tmp800.f1=_tmpF60;});({struct _dyneither_ptr*_tmpF62=({struct _dyneither_ptr*_tmp801=_cycalloc(sizeof(*_tmp801));({struct _dyneither_ptr _tmpF61=Cyc_yyget_String_tok(yyyyvsp[4]);_tmp801[0]=_tmpF61;});_tmp801;});_tmp800.f2=_tmpF62;});_tmp800;});_tmp7FF[0]=_tmpF63;});_tmp7FF;});Cyc_Absyn_new_exp(_tmpF64,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF65;});
goto _LL254;}case 479U: _LL60D: _LL60E: {
# 3029
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3031
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3028 "parse.y"
void*_tmp802=({struct _tuple8*_tmpF66=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpF66,(unsigned int)(yyyylsp[2]).first_line);});
({union Cyc_YYSTYPE _tmpF67=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(_tmp802,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpF67;});
goto _LL254;}case 480U: _LL60F: _LL610: {
# 3032
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3034
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3031 "parse.y"
struct _tuple20 _tmp803=Cyc_yyget_Asm_tok(yyyyvsp[0]);struct _tuple20 _tmp804=_tmp803;int _tmp806;struct _dyneither_ptr _tmp805;_LL6E0: _tmp806=_tmp804.f1;_tmp805=_tmp804.f2;_LL6E1:;
({union Cyc_YYSTYPE _tmpF68=Cyc_Exp_tok(Cyc_Absyn_asm_exp(_tmp806,_tmp805,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpF68;});
goto _LL254;}case 481U: _LL611: _LL612: {
# 3035
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3037
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3036 "parse.y"
({union Cyc_YYSTYPE _tmpF69=Cyc_YY6(Cyc_Absyn_Bitnot);yyval=_tmpF69;});
goto _LL254;}case 482U: _LL613: _LL614: {
# 3039
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3041
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3037 "parse.y"
({union Cyc_YYSTYPE _tmpF6A=Cyc_YY6(Cyc_Absyn_Not);yyval=_tmpF6A;});
goto _LL254;}case 483U: _LL615: _LL616: {
# 3040
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3042
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3038 "parse.y"
({union Cyc_YYSTYPE _tmpF6B=Cyc_YY6(Cyc_Absyn_Minus);yyval=_tmpF6B;});
goto _LL254;}case 484U: _LL617: _LL618: {
# 3041
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3043
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3043 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 485U: _LL619: _LL61A: {
# 3046
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3048
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3045 "parse.y"
({union Cyc_YYSTYPE _tmpF6E=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF6D=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpF6C=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_subscript_exp(_tmpF6D,_tmpF6C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF6E;});
goto _LL254;}case 486U: _LL61B: _LL61C: {
# 3048
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3050
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3047 "parse.y"
({union Cyc_YYSTYPE _tmpF70=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF6F=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_unknowncall_exp(_tmpF6F,0,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF70;});
goto _LL254;}case 487U: _LL61D: _LL61E: {
# 3050
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3052
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3049 "parse.y"
({union Cyc_YYSTYPE _tmpF73=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF72=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_List_List*_tmpF71=Cyc_yyget_YY4(yyyyvsp[2]);Cyc_Absyn_unknowncall_exp(_tmpF72,_tmpF71,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF73;});
goto _LL254;}case 488U: _LL61F: _LL620: {
# 3052
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3054
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3051 "parse.y"
({union Cyc_YYSTYPE _tmpF77=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF76=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct _dyneither_ptr*_tmpF75=({struct _dyneither_ptr*_tmp807=_cycalloc(sizeof(*_tmp807));({struct _dyneither_ptr _tmpF74=Cyc_yyget_String_tok(yyyyvsp[2]);_tmp807[0]=_tmpF74;});_tmp807;});Cyc_Absyn_aggrmember_exp(_tmpF76,_tmpF75,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF77;});
goto _LL254;}case 489U: _LL621: _LL622: {
# 3054
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3056
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3053 "parse.y"
({union Cyc_YYSTYPE _tmpF7B=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF7A=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct _dyneither_ptr*_tmpF79=({struct _dyneither_ptr*_tmp808=_cycalloc(sizeof(*_tmp808));({struct _dyneither_ptr _tmpF78=Cyc_yyget_String_tok(yyyyvsp[2]);_tmp808[0]=_tmpF78;});_tmp808;});Cyc_Absyn_aggrarrow_exp(_tmpF7A,_tmpF79,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF7B;});
goto _LL254;}case 490U: _LL623: _LL624: {
# 3056
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3058
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3055 "parse.y"
({union Cyc_YYSTYPE _tmpF7D=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF7C=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_post_inc_exp(_tmpF7C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF7D;});
goto _LL254;}case 491U: _LL625: _LL626: {
# 3058
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3060
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3057 "parse.y"
({union Cyc_YYSTYPE _tmpF7F=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpF7E=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_post_dec_exp(_tmpF7E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF7F;});
goto _LL254;}case 492U: _LL627: _LL628: {
# 3060
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3062
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3059 "parse.y"
({union Cyc_YYSTYPE _tmpF83=Cyc_Exp_tok(({void*_tmpF82=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp809=_cycalloc(sizeof(*_tmp809));({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpF81=({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmp80A;_tmp80A.tag=25U;({struct _tuple8*_tmpF80=Cyc_yyget_YY37(yyyyvsp[1]);_tmp80A.f1=_tmpF80;});_tmp80A.f2=0;_tmp80A;});_tmp809[0]=_tmpF81;});_tmp809;});Cyc_Absyn_new_exp(_tmpF82,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF83;});
goto _LL254;}case 493U: _LL629: _LL62A: {
# 3062
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3064
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3061 "parse.y"
({union Cyc_YYSTYPE _tmpF88=Cyc_Exp_tok(({void*_tmpF87=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp80B=_cycalloc(sizeof(*_tmp80B));({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpF86=({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmp80C;_tmp80C.tag=25U;({struct _tuple8*_tmpF84=Cyc_yyget_YY37(yyyyvsp[1]);_tmp80C.f1=_tmpF84;});({struct Cyc_List_List*_tmpF85=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4]));_tmp80C.f2=_tmpF85;});_tmp80C;});_tmp80B[0]=_tmpF86;});_tmp80B;});Cyc_Absyn_new_exp(_tmpF87,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF88;});
goto _LL254;}case 494U: _LL62B: _LL62C: {
# 3064
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 3066
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 3063 "parse.y"
({union Cyc_YYSTYPE _tmpF8D=Cyc_Exp_tok(({void*_tmpF8C=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp80D=_cycalloc(sizeof(*_tmp80D));({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpF8B=({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmp80E;_tmp80E.tag=25U;({struct _tuple8*_tmpF89=Cyc_yyget_YY37(yyyyvsp[1]);_tmp80E.f1=_tmpF89;});({struct Cyc_List_List*_tmpF8A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4]));_tmp80E.f2=_tmpF8A;});_tmp80E;});_tmp80D[0]=_tmpF8B;});_tmp80D;});Cyc_Absyn_new_exp(_tmpF8C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpF8D;});
goto _LL254;}case 495U: _LL62D: _LL62E: {
# 3066
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3068
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3068 "parse.y"
({union Cyc_YYSTYPE _tmpF92=Cyc_YY3(({struct Cyc_List_List*_tmp80F=_cycalloc(sizeof(*_tmp80F));({void*_tmpF91=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp810=_cycalloc(sizeof(*_tmp810));({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpF90=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp811;_tmp811.tag=0U;({struct _dyneither_ptr*_tmpF8F=({struct _dyneither_ptr*_tmp812=_cycalloc(sizeof(*_tmp812));({struct _dyneither_ptr _tmpF8E=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp812[0]=_tmpF8E;});_tmp812;});_tmp811.f1=_tmpF8F;});_tmp811;});_tmp810[0]=_tmpF90;});_tmp810;});_tmp80F->hd=_tmpF91;});_tmp80F->tl=0;_tmp80F;}));yyval=_tmpF92;});
goto _LL254;}case 496U: _LL62F: _LL630: {
# 3071
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3073
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3071 "parse.y"
({union Cyc_YYSTYPE _tmpF97=Cyc_YY3(({struct Cyc_List_List*_tmp813=_cycalloc(sizeof(*_tmp813));({void*_tmpF96=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp814=_cycalloc_atomic(sizeof(*_tmp814));({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpF95=({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmp815;_tmp815.tag=1U;({unsigned int _tmpF94=({unsigned int _tmpF93=(unsigned int)(yyyylsp[0]).first_line;Cyc_Parse_cnst2uint(_tmpF93,Cyc_yyget_Int_tok(yyyyvsp[0]));});_tmp815.f1=_tmpF94;});_tmp815;});_tmp814[0]=_tmpF95;});_tmp814;});_tmp813->hd=_tmpF96;});_tmp813->tl=0;_tmp813;}));yyval=_tmpF97;});
goto _LL254;}case 497U: _LL631: _LL632: {
# 3074
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3076
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3073 "parse.y"
({union Cyc_YYSTYPE _tmpF9D=Cyc_YY3(({struct Cyc_List_List*_tmp816=_cycalloc(sizeof(*_tmp816));({void*_tmpF9B=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp817=_cycalloc(sizeof(*_tmp817));({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpF9A=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp818;_tmp818.tag=0U;({struct _dyneither_ptr*_tmpF99=({struct _dyneither_ptr*_tmp819=_cycalloc(sizeof(*_tmp819));({struct _dyneither_ptr _tmpF98=Cyc_yyget_String_tok(yyyyvsp[2]);_tmp819[0]=_tmpF98;});_tmp819;});_tmp818.f1=_tmpF99;});_tmp818;});_tmp817[0]=_tmpF9A;});_tmp817;});_tmp816->hd=_tmpF9B;});({struct Cyc_List_List*_tmpF9C=Cyc_yyget_YY3(yyyyvsp[0]);_tmp816->tl=_tmpF9C;});_tmp816;}));yyval=_tmpF9D;});
goto _LL254;}case 498U: _LL633: _LL634: {
# 3076
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3078
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3076 "parse.y"
({union Cyc_YYSTYPE _tmpFA3=Cyc_YY3(({struct Cyc_List_List*_tmp81A=_cycalloc(sizeof(*_tmp81A));({void*_tmpFA1=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp81B=_cycalloc_atomic(sizeof(*_tmp81B));({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpFA0=({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmp81C;_tmp81C.tag=1U;({unsigned int _tmpF9F=({unsigned int _tmpF9E=(unsigned int)(yyyylsp[2]).first_line;Cyc_Parse_cnst2uint(_tmpF9E,Cyc_yyget_Int_tok(yyyyvsp[2]));});_tmp81C.f1=_tmpF9F;});_tmp81C;});_tmp81B[0]=_tmpFA0;});_tmp81B;});_tmp81A->hd=_tmpFA1;});({struct Cyc_List_List*_tmpFA2=Cyc_yyget_YY3(yyyyvsp[0]);_tmp81A->tl=_tmpFA2;});_tmp81A;}));yyval=_tmpFA3;});
goto _LL254;}case 499U: _LL635: _LL636: {
# 3079
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3081
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpFA5=Cyc_Exp_tok(({struct _tuple0*_tmpFA4=Cyc_yyget_QualId_tok(yyyyvsp[0]);Cyc_Absyn_unknownid_exp(_tmpFA4,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpFA5;});
goto _LL254;}case 500U: _LL637: _LL638: {
# 3085
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3087
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3084 "parse.y"
({union Cyc_YYSTYPE _tmpFA7=Cyc_Exp_tok(({struct _dyneither_ptr _tmpFA6=Cyc_yyget_String_tok(yyyyvsp[2]);Cyc_Absyn_pragma_exp(_tmpFA6,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpFA7;});
goto _LL254;}case 501U: _LL639: _LL63A: {
# 3087
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3089
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3086 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 502U: _LL63B: _LL63C: {
# 3089
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3091
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3088 "parse.y"
({union Cyc_YYSTYPE _tmpFA9=Cyc_Exp_tok(({struct _dyneither_ptr _tmpFA8=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Absyn_string_exp(_tmpFA8,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpFA9;});
goto _LL254;}case 503U: _LL63D: _LL63E: {
# 3091
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3093
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3090 "parse.y"
({union Cyc_YYSTYPE _tmpFAB=Cyc_Exp_tok(({struct _dyneither_ptr _tmpFAA=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Absyn_wstring_exp(_tmpFAA,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpFAB;});
goto _LL254;}case 504U: _LL63F: _LL640: {
# 3093
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3095
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3092 "parse.y"
yyval=yyyyvsp[1];
goto _LL254;}case 505U: _LL641: _LL642: {
# 3095
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3097
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3097 "parse.y"
({union Cyc_YYSTYPE _tmpFAD=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpFAC=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_noinstantiate_exp(_tmpFAC,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpFAD;});
goto _LL254;}case 506U: _LL643: _LL644: {
# 3100
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3102
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3099 "parse.y"
({union Cyc_YYSTYPE _tmpFB0=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpFAF=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_List_List*_tmpFAE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[3]));Cyc_Absyn_instantiate_exp(_tmpFAF,_tmpFAE,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpFB0;});
goto _LL254;}case 507U: _LL645: _LL646: {
# 3102
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3104
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3102 "parse.y"
({union Cyc_YYSTYPE _tmpFB2=Cyc_Exp_tok(({struct Cyc_List_List*_tmpFB1=Cyc_yyget_YY4(yyyyvsp[2]);Cyc_Absyn_tuple_exp(_tmpFB1,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpFB2;});
goto _LL254;}case 508U: _LL647: _LL648: {
# 3105
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3107
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3105 "parse.y"
({union Cyc_YYSTYPE _tmpFB8=Cyc_Exp_tok(({void*_tmpFB7=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp81D=_cycalloc(sizeof(*_tmp81D));({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpFB6=({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp81E;_tmp81E.tag=29U;({struct _tuple0*_tmpFB3=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp81E.f1=_tmpFB3;});({struct Cyc_List_List*_tmpFB4=Cyc_yyget_YY40(yyyyvsp[2]);_tmp81E.f2=_tmpFB4;});({struct Cyc_List_List*_tmpFB5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[3]));_tmp81E.f3=_tmpFB5;});_tmp81E.f4=0;_tmp81E;});_tmp81D[0]=_tmpFB6;});_tmp81D;});Cyc_Absyn_new_exp(_tmpFB7,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpFB8;});
goto _LL254;}case 509U: _LL649: _LL64A: {
# 3108
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3110
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3108 "parse.y"
({union Cyc_YYSTYPE _tmpFBA=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_tmpFB9=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Absyn_stmt_exp(_tmpFB9,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpFBA;});
goto _LL254;}case 510U: _LL64B: _LL64C: {
# 3111
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3113
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3112 "parse.y"
({union Cyc_YYSTYPE _tmpFBB=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY4(yyyyvsp[0])));yyval=_tmpFBB;});
goto _LL254;}case 511U: _LL64D: _LL64E: {
# 3115
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3117
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpFBD=Cyc_YY4(({struct Cyc_List_List*_tmp81F=_cycalloc(sizeof(*_tmp81F));({struct Cyc_Absyn_Exp*_tmpFBC=Cyc_yyget_Exp_tok(yyyyvsp[0]);_tmp81F->hd=_tmpFBC;});_tmp81F->tl=0;_tmp81F;}));yyval=_tmpFBD;});
goto _LL254;}case 512U: _LL64F: _LL650: {
# 3121
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3123
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3120 "parse.y"
({union Cyc_YYSTYPE _tmpFC0=Cyc_YY4(({struct Cyc_List_List*_tmp820=_cycalloc(sizeof(*_tmp820));({struct Cyc_Absyn_Exp*_tmpFBE=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp820->hd=_tmpFBE;});({struct Cyc_List_List*_tmpFBF=Cyc_yyget_YY4(yyyyvsp[0]);_tmp820->tl=_tmpFBF;});_tmp820;}));yyval=_tmpFC0;});
goto _LL254;}case 513U: _LL651: _LL652: {
# 3123
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3125
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpFC2=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _tmpFC1=Cyc_yyget_Int_tok(yyyyvsp[0]);Cyc_Absyn_const_exp(_tmpFC1,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpFC2;});
goto _LL254;}case 514U: _LL653: _LL654: {
# 3129
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3131
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3127 "parse.y"
({union Cyc_YYSTYPE _tmpFC4=Cyc_Exp_tok(({char _tmpFC3=Cyc_yyget_Char_tok(yyyyvsp[0]);Cyc_Absyn_char_exp(_tmpFC3,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpFC4;});
goto _LL254;}case 515U: _LL655: _LL656: {
# 3130
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3132
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3128 "parse.y"
({union Cyc_YYSTYPE _tmpFC6=Cyc_Exp_tok(({struct _dyneither_ptr _tmpFC5=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Absyn_wchar_exp(_tmpFC5,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpFC6;});
goto _LL254;}case 516U: _LL657: _LL658: {
# 3131
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3133
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3130 "parse.y"
struct _dyneither_ptr _tmp821=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp821);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp821,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3138
({union Cyc_YYSTYPE _tmpFC7=Cyc_Exp_tok(Cyc_Absyn_float_exp(_tmp821,i,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpFC7;});
# 3140
goto _LL254;}case 517U: _LL659: _LL65A: {
# 3142
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3144
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3141 "parse.y"
({union Cyc_YYSTYPE _tmpFC8=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpFC8;});
goto _LL254;}case 518U: _LL65B: _LL65C: {
# 3144
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3146
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3145 "parse.y"
({union Cyc_YYSTYPE _tmpFCC=Cyc_QualId_tok(({struct _tuple0*_tmp822=_cycalloc(sizeof(*_tmp822));({union Cyc_Absyn_Nmspace _tmpFC9=Cyc_Absyn_Rel_n(0);_tmp822->f1=_tmpFC9;});({struct _dyneither_ptr*_tmpFCB=({struct _dyneither_ptr*_tmp823=_cycalloc(sizeof(*_tmp823));({struct _dyneither_ptr _tmpFCA=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp823[0]=_tmpFCA;});_tmp823;});_tmp822->f2=_tmpFCB;});_tmp822;}));yyval=_tmpFCC;});
goto _LL254;}case 519U: _LL65D: _LL65E: {
# 3148
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3150
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3146 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 520U: _LL65F: _LL660: {
# 3149
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3151
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3149 "parse.y"
({union Cyc_YYSTYPE _tmpFD0=Cyc_QualId_tok(({struct _tuple0*_tmp824=_cycalloc(sizeof(*_tmp824));({union Cyc_Absyn_Nmspace _tmpFCD=Cyc_Absyn_Rel_n(0);_tmp824->f1=_tmpFCD;});({struct _dyneither_ptr*_tmpFCF=({struct _dyneither_ptr*_tmp825=_cycalloc(sizeof(*_tmp825));({struct _dyneither_ptr _tmpFCE=Cyc_yyget_String_tok(yyyyvsp[0]);_tmp825[0]=_tmpFCE;});_tmp825;});_tmp824->f2=_tmpFCF;});_tmp824;}));yyval=_tmpFD0;});
goto _LL254;}case 521U: _LL661: _LL662: {
# 3152
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3154
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3150 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 522U: _LL663: _LL664: {
# 3153
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3155
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3155 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 523U: _LL665: _LL666: {
# 3158
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3160
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3156 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 524U: _LL667: _LL668: {
# 3159
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3161
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3159 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 525U: _LL669: _LL66A: {
# 3162
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3164
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3160 "parse.y"
yyval=yyyyvsp[0];
goto _LL254;}case 526U: _LL66B: _LL66C: {
# 3163
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3165
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3165 "parse.y"
goto _LL254;}case 527U: _LL66D: _LL66E: {
# 3167
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3169
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3165 "parse.y"
yylex_buf->lex_curr_pos -=1;
goto _LL254;}default: _LL66F: _LL670:
# 3170
 goto _LL254;}_LL254:;}
# 375 "cycbison.simple"
yyvsp_offset -=yylen;
yyssp_offset -=yylen;
# 378
yylsp_offset -=yylen;
# 392 "cycbison.simple"
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yyval;
# 395
++ yylsp_offset;
if(yylen == 0){
# 398
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line=yylloc.first_line;
(((struct Cyc_Yyltype*)yyls.curr)[yylsp_offset]).first_column=yylloc.first_column;
(((struct Cyc_Yyltype*)yyls.curr)[yylsp_offset]).last_line=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line;
(((struct Cyc_Yyltype*)yyls.curr)[yylsp_offset]).last_column=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_column;}else{
# 405
({int _tmpFD1=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_line;(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line=_tmpFD1;});
(((struct Cyc_Yyltype*)yyls.curr)[yylsp_offset]).last_column=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_column;}
# 415
yyn=(int)Cyc_yyr1[yyn];
# 417
({int _tmpFD3=({short _tmpFD2=Cyc_yypgoto[_check_known_subscript_notnull(147U,yyn - 147)];_tmpFD2 + *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));});yystate=_tmpFD3;});
if((yystate >= 0  && yystate <= 7177) && Cyc_yycheck[_check_known_subscript_notnull(7178U,yystate)]== ((short*)yyss.curr)[yyssp_offset])
yystate=(int)Cyc_yytable[yystate];else{
# 421
yystate=(int)Cyc_yydefgoto[_check_known_subscript_notnull(147U,yyn - 147)];}
# 423
goto yynewstate;
# 425
yyerrlab:
# 427
 if(yyerrstatus == 0){
# 430
++ yynerrs;
# 433
yyn=(int)Cyc_yypact[yystate];
# 435
if(yyn > - 32768  && yyn < 7177){
# 437
int sze=0;
struct _dyneither_ptr msg;
int x;int count;
# 441
count=0;
# 443
for(x=yyn < 0?- yyn: 0;x < 294U / sizeof(char*);++ x){
# 445
if(Cyc_yycheck[_check_known_subscript_notnull(7178U,x + yyn)]== x)
(({unsigned long _tmpFD4=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(294U,x)])+ 15;sze +=_tmpFD4;}),count ++);}
({struct _dyneither_ptr _tmpFD6=({unsigned int _tmp826=(unsigned int)(sze + 15);char*_tmp827=(char*)({struct _RegionHandle*_tmpFD5=yyregion;_region_malloc(_tmpFD5,_check_times(sizeof(char),_tmp826 + 1U));});struct _dyneither_ptr _tmp829=_tag_dyneither(_tmp827,sizeof(char),_tmp826 + 1U);{unsigned int _tmp828=_tmp826;unsigned int i;for(i=0;i < _tmp828;i ++){_tmp827[i]=(char)'\000';}_tmp827[_tmp828]=(char)0U;}_tmp829;});msg=_tmpFD6;});
({struct _dyneither_ptr _tmpFD7=msg;Cyc_strcpy(_tmpFD7,({const char*_tmp82A="parse error";_tag_dyneither(_tmp82A,sizeof(char),12U);}));});
# 450
if(count < 5){
# 452
count=0;
for(x=yyn < 0?- yyn: 0;x < 294U / sizeof(char*);++ x){
# 455
if(Cyc_yycheck[_check_known_subscript_notnull(7178U,x + yyn)]== x){
# 457
({struct _dyneither_ptr _tmpFD8=msg;Cyc_strcat(_tmpFD8,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)({const char*_tmp82B=", expecting `";_tag_dyneither(_tmp82B,sizeof(char),14U);}):(struct _dyneither_ptr)({const char*_tmp82C=" or `";_tag_dyneither(_tmp82C,sizeof(char),6U);})));});
# 460
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(294U,x)]);
({struct _dyneither_ptr _tmpFD9=msg;Cyc_strcat(_tmpFD9,({const char*_tmp82D="'";_tag_dyneither(_tmp82D,sizeof(char),2U);}));});
++ count;}}}
# 465
Cyc_yyerror((struct _dyneither_ptr)msg,yystate,yychar);}else{
# 469
({struct _dyneither_ptr _tmpFDB=({const char*_tmp82E="parse error";_tag_dyneither(_tmp82E,sizeof(char),12U);});int _tmpFDA=yystate;Cyc_yyerror(_tmpFDB,_tmpFDA,yychar);});}}
# 471
goto yyerrlab1;
# 473
yyerrlab1:
# 475
 if(yyerrstatus == 3){
# 480
if(yychar == 0){
int _tmp82F=1;_npop_handler(0U);return _tmp82F;}
# 489
yychar=-2;}
# 495
yyerrstatus=3;
# 497
goto yyerrhandle;
# 499
yyerrdefault:
# 509 "cycbison.simple"
 yyerrpop:
# 511
 if(yyssp_offset == 0){int _tmp830=1;_npop_handler(0U);return _tmp830;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));
# 515
-- yylsp_offset;
# 530 "cycbison.simple"
yyerrhandle:
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1074U,yystate)];
if(yyn == - 32768)goto yyerrdefault;
# 534
yyn +=1;
if((yyn < 0  || yyn > 7177) || Cyc_yycheck[_check_known_subscript_notnull(7178U,yyn)]!= 1)goto yyerrdefault;
# 537
yyn=(int)Cyc_yytable[yyn];
if(yyn < 0){
# 540
if(yyn == - 32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 544
if(yyn == 0)goto yyerrpop;}
# 546
if(yyn == 1073){
int _tmp831=0;_npop_handler(0U);return _tmp831;}
# 554
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 556
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 559
goto yynewstate;}
# 141 "cycbison.simple"
;_pop_region(yyregion);}
# 3168 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp83B=v;struct Cyc_Absyn_Stmt*_tmp855;struct Cyc_Absyn_Exp*_tmp854;struct _tuple0*_tmp853;struct _dyneither_ptr _tmp852;char _tmp851;union Cyc_Absyn_Cnst _tmp850;switch((_tmp83B.Stmt_tok).tag){case 1U: _LL6E3: _tmp850=(_tmp83B.Int_tok).val;_LL6E4:
({struct Cyc_String_pa_PrintArg_struct _tmp83E;_tmp83E.tag=0U;({struct _dyneither_ptr _tmpFDC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(_tmp850));_tmp83E.f1=_tmpFDC;});({void*_tmp83C[1U]={& _tmp83E};({struct Cyc___cycFILE*_tmpFDE=Cyc_stderr;struct _dyneither_ptr _tmpFDD=({const char*_tmp83D="%s";_tag_dyneither(_tmp83D,sizeof(char),3U);});Cyc_fprintf(_tmpFDE,_tmpFDD,_tag_dyneither(_tmp83C,sizeof(void*),1U));});});});goto _LL6E2;case 2U: _LL6E5: _tmp851=(_tmp83B.Char_tok).val;_LL6E6:
({struct Cyc_Int_pa_PrintArg_struct _tmp841;_tmp841.tag=1U;_tmp841.f1=(unsigned long)((int)_tmp851);({void*_tmp83F[1U]={& _tmp841};({struct Cyc___cycFILE*_tmpFE0=Cyc_stderr;struct _dyneither_ptr _tmpFDF=({const char*_tmp840="%c";_tag_dyneither(_tmp840,sizeof(char),3U);});Cyc_fprintf(_tmpFE0,_tmpFDF,_tag_dyneither(_tmp83F,sizeof(void*),1U));});});});goto _LL6E2;case 3U: _LL6E7: _tmp852=(_tmp83B.String_tok).val;_LL6E8:
({struct Cyc_String_pa_PrintArg_struct _tmp844;_tmp844.tag=0U;_tmp844.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp852);({void*_tmp842[1U]={& _tmp844};({struct Cyc___cycFILE*_tmpFE2=Cyc_stderr;struct _dyneither_ptr _tmpFE1=({const char*_tmp843="\"%s\"";_tag_dyneither(_tmp843,sizeof(char),5U);});Cyc_fprintf(_tmpFE2,_tmpFE1,_tag_dyneither(_tmp842,sizeof(void*),1U));});});});goto _LL6E2;case 5U: _LL6E9: _tmp853=(_tmp83B.QualId_tok).val;_LL6EA:
({struct Cyc_String_pa_PrintArg_struct _tmp847;_tmp847.tag=0U;({struct _dyneither_ptr _tmpFE3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp853));_tmp847.f1=_tmpFE3;});({void*_tmp845[1U]={& _tmp847};({struct Cyc___cycFILE*_tmpFE5=Cyc_stderr;struct _dyneither_ptr _tmpFE4=({const char*_tmp846="%s";_tag_dyneither(_tmp846,sizeof(char),3U);});Cyc_fprintf(_tmpFE5,_tmpFE4,_tag_dyneither(_tmp845,sizeof(void*),1U));});});});goto _LL6E2;case 7U: _LL6EB: _tmp854=(_tmp83B.Exp_tok).val;_LL6EC:
({struct Cyc_String_pa_PrintArg_struct _tmp84A;_tmp84A.tag=0U;({struct _dyneither_ptr _tmpFE6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp854));_tmp84A.f1=_tmpFE6;});({void*_tmp848[1U]={& _tmp84A};({struct Cyc___cycFILE*_tmpFE8=Cyc_stderr;struct _dyneither_ptr _tmpFE7=({const char*_tmp849="%s";_tag_dyneither(_tmp849,sizeof(char),3U);});Cyc_fprintf(_tmpFE8,_tmpFE7,_tag_dyneither(_tmp848,sizeof(void*),1U));});});});goto _LL6E2;case 8U: _LL6ED: _tmp855=(_tmp83B.Stmt_tok).val;_LL6EE:
({struct Cyc_String_pa_PrintArg_struct _tmp84D;_tmp84D.tag=0U;({struct _dyneither_ptr _tmpFE9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(_tmp855));_tmp84D.f1=_tmpFE9;});({void*_tmp84B[1U]={& _tmp84D};({struct Cyc___cycFILE*_tmpFEB=Cyc_stderr;struct _dyneither_ptr _tmpFEA=({const char*_tmp84C="%s";_tag_dyneither(_tmp84C,sizeof(char),3U);});Cyc_fprintf(_tmpFEB,_tmpFEA,_tag_dyneither(_tmp84B,sizeof(void*),1U));});});});goto _LL6E2;default: _LL6EF: _LL6F0:
({void*_tmp84E=0U;({struct Cyc___cycFILE*_tmpFED=Cyc_stderr;struct _dyneither_ptr _tmpFEC=({const char*_tmp84F="?";_tag_dyneither(_tmp84F,sizeof(char),2U);});Cyc_fprintf(_tmpFED,_tmpFEC,_tag_dyneither(_tmp84E,sizeof(void*),0U));});});goto _LL6E2;}_LL6E2:;}
# 3181
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp856=_new_region("yyr");struct _RegionHandle*yyr=& _tmp856;_push_region(yyr);
({struct _RegionHandle*_tmpFEE=yyr;Cyc_yyparse(_tmpFEE,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_tmp857=Cyc_Parse_parse_result;_npop_handler(0U);return _tmp857;};
# 3184
;_pop_region(yyr);};}
