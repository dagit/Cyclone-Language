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
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
typedef struct Cyc_List_List*Cyc_List_List_t;
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 210
struct Cyc_List_List*Cyc_List_merge_sort(int(*cmp)(void*,void*),struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 64
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;
# 38
struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
# 46
extern int Cyc_Position_use_gcc_style_location;struct Cyc_Relations_Reln;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 247
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
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
# 915 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 925
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 929
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 932
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 950
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 955
extern void*Cyc_Absyn_sint_typ;
# 972
void*Cyc_Absyn_string_typ(void*rgn);
# 994
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1183
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 62
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
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
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct _tuple10{void*f1;void*f2;};struct Cyc_Tcexp_TestEnv{struct _tuple10*eq;int isTrue;};
# 35 "tcexp.h"
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 34 "tcdecl.h"
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_xdatatypefielddecl_t;struct Cyc_Hashtable_Table;
# 35 "hashtable.h"
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
# 39
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 59
int Cyc_Hashtable_try_lookup(struct Cyc_Hashtable_Table*t,void*key,void**data);struct Cyc_Port_Edit{unsigned int loc;struct _dyneither_ptr old_string;struct _dyneither_ptr new_string;};
# 81 "port.cyc"
typedef struct Cyc_Port_Edit*Cyc_Port_edit_t;
# 86
typedef struct Cyc_List_List*Cyc_Port_edits_t;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;
# 88
int Cyc_Port_cmp_edit(struct Cyc_Port_Edit*e1,struct Cyc_Port_Edit*e2){
return(int)e1 - (int)e2;}
# 92
int Cyc_Port_cmp_seg_t(unsigned int loc1,unsigned int loc2){
return(int)(loc1 - loc2);}
# 95
int Cyc_Port_hash_seg_t(unsigned int loc){
return(int)loc;}struct Cyc_Port_VarUsage{int address_taken;struct Cyc_Absyn_Exp*init;struct Cyc_List_List*usage_locs;};
# 98
typedef struct Cyc_Port_VarUsage*Cyc_Port_varusage_t;
# 107
typedef void*Cyc_Port_ctype_t;struct Cyc_Port_Cvar{int id;void**eq;struct Cyc_List_List*uppers;struct Cyc_List_List*lowers;};
# 109
typedef struct Cyc_Port_Cvar*Cyc_Port_cvar_t;struct Cyc_Port_Cfield{void*qual;struct _dyneither_ptr*f;void*type;};
# 116
typedef struct Cyc_Port_Cfield*Cyc_Port_cfield_t;
# 121
typedef struct Cyc_List_List*Cyc_Port_cfields_t;struct Cyc_Port_Const_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Notconst_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Thin_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Fat_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Void_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Zero_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Arith_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Heap_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Zterm_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Nozterm_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_RgnVar_ct_Port_Ctype_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Port_Ptr_ct_Port_Ctype_struct{int tag;void*f1;void*f2;void*f3;void*f4;void*f5;};struct Cyc_Port_Array_ct_Port_Ctype_struct{int tag;void*f1;void*f2;void*f3;};struct _tuple11{struct Cyc_Absyn_Aggrdecl*f1;struct Cyc_List_List*f2;};struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct{int tag;struct _tuple11*f1;};struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct{int tag;struct Cyc_List_List*f1;void**f2;};struct Cyc_Port_Fn_ct_Port_Ctype_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Port_Cvar;struct Cyc_Port_Var_ct_Port_Ctype_struct{int tag;struct Cyc_Port_Cvar*f1;};
# 151
struct Cyc_Port_Const_ct_Port_Ctype_struct Cyc_Port_Const_ct_val={0};
struct Cyc_Port_Notconst_ct_Port_Ctype_struct Cyc_Port_Notconst_ct_val={1};
struct Cyc_Port_Thin_ct_Port_Ctype_struct Cyc_Port_Thin_ct_val={2};
struct Cyc_Port_Fat_ct_Port_Ctype_struct Cyc_Port_Fat_ct_val={3};
struct Cyc_Port_Void_ct_Port_Ctype_struct Cyc_Port_Void_ct_val={4};
struct Cyc_Port_Zero_ct_Port_Ctype_struct Cyc_Port_Zero_ct_val={5};
struct Cyc_Port_Arith_ct_Port_Ctype_struct Cyc_Port_Arith_ct_val={6};
struct Cyc_Port_Heap_ct_Port_Ctype_struct Cyc_Port_Heap_ct_val={7};
struct Cyc_Port_Zterm_ct_Port_Ctype_struct Cyc_Port_Zterm_ct_val={8};
struct Cyc_Port_Nozterm_ct_Port_Ctype_struct Cyc_Port_Nozterm_ct_val={9};
# 164
static struct _dyneither_ptr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*ts);
static struct _dyneither_ptr Cyc_Port_cfields2string(int deep,struct Cyc_List_List*ts);struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
static struct _dyneither_ptr Cyc_Port_ctype2string(int deep,void*t){
void*_tmpA=t;struct Cyc_Port_Cvar*_tmp4E;void*_tmp4D;struct Cyc_List_List*_tmp4C;struct Cyc_List_List*_tmp4B;struct Cyc_List_List*_tmp4A;void*_tmp49;struct Cyc_Absyn_Aggrdecl*_tmp48;struct Cyc_List_List*_tmp47;void*_tmp46;void*_tmp45;void*_tmp44;void*_tmp43;void*_tmp42;void*_tmp41;void*_tmp40;void*_tmp3F;struct _dyneither_ptr*_tmp3E;switch(*((int*)_tmpA)){case 0: _LL1: _LL2:
 return({const char*_tmpB="const";_tag_dyneither(_tmpB,sizeof(char),6);});case 1: _LL3: _LL4:
 return({const char*_tmpC="notconst";_tag_dyneither(_tmpC,sizeof(char),9);});case 2: _LL5: _LL6:
 return({const char*_tmpD="thin";_tag_dyneither(_tmpD,sizeof(char),5);});case 3: _LL7: _LL8:
 return({const char*_tmpE="fat";_tag_dyneither(_tmpE,sizeof(char),4);});case 4: _LL9: _LLA:
 return({const char*_tmpF="void";_tag_dyneither(_tmpF,sizeof(char),5);});case 5: _LLB: _LLC:
 return({const char*_tmp10="zero";_tag_dyneither(_tmp10,sizeof(char),5);});case 6: _LLD: _LLE:
 return({const char*_tmp11="arith";_tag_dyneither(_tmp11,sizeof(char),6);});case 7: _LLF: _LL10:
 return({const char*_tmp12="heap";_tag_dyneither(_tmp12,sizeof(char),5);});case 8: _LL11: _LL12:
 return({const char*_tmp13="ZT";_tag_dyneither(_tmp13,sizeof(char),3);});case 9: _LL13: _LL14:
 return({const char*_tmp14="NZT";_tag_dyneither(_tmp14,sizeof(char),4);});case 10: _LL15: _tmp3E=((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmpA)->f1;_LL16:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp17;_tmp17.tag=0;_tmp17.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3E);({void*_tmp15[1]={& _tmp17};({struct _dyneither_ptr _tmp3AF=({const char*_tmp16="%s";_tag_dyneither(_tmp16,sizeof(char),3);});Cyc_aprintf(_tmp3AF,_tag_dyneither(_tmp15,sizeof(void*),1));});});});case 11: _LL17: _tmp43=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA)->f1;_tmp42=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA)->f2;_tmp41=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA)->f3;_tmp40=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA)->f4;_tmp3F=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA)->f5;_LL18:
# 180
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1E;_tmp1E.tag=0;({struct _dyneither_ptr _tmp3B0=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 182
Cyc_Port_ctype2string(deep,_tmp3F));_tmp1E.f1=_tmp3B0;});({struct Cyc_String_pa_PrintArg_struct _tmp1D;_tmp1D.tag=0;({struct _dyneither_ptr _tmp3B1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp40));_tmp1D.f1=_tmp3B1;});({struct Cyc_String_pa_PrintArg_struct _tmp1C;_tmp1C.tag=0;({struct _dyneither_ptr _tmp3B2=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 181
Cyc_Port_ctype2string(deep,_tmp41));_tmp1C.f1=_tmp3B2;});({struct Cyc_String_pa_PrintArg_struct _tmp1B;_tmp1B.tag=0;({struct _dyneither_ptr _tmp3B3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp42));_tmp1B.f1=_tmp3B3;});({struct Cyc_String_pa_PrintArg_struct _tmp1A;_tmp1A.tag=0;({struct _dyneither_ptr _tmp3B4=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 180
Cyc_Port_ctype2string(deep,_tmp43));_tmp1A.f1=_tmp3B4;});({void*_tmp18[5]={& _tmp1A,& _tmp1B,& _tmp1C,& _tmp1D,& _tmp1E};({struct _dyneither_ptr _tmp3B5=({const char*_tmp19="ptr(%s,%s,%s,%s,%s)";_tag_dyneither(_tmp19,sizeof(char),20);});Cyc_aprintf(_tmp3B5,_tag_dyneither(_tmp18,sizeof(void*),5));});});});});});});});case 12: _LL19: _tmp46=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpA)->f1;_tmp45=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpA)->f2;_tmp44=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpA)->f3;_LL1A:
# 184
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp23;_tmp23.tag=0;({struct _dyneither_ptr _tmp3B6=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Port_ctype2string(deep,_tmp44));_tmp23.f1=_tmp3B6;});({struct Cyc_String_pa_PrintArg_struct _tmp22;_tmp22.tag=0;({struct _dyneither_ptr _tmp3B7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp45));_tmp22.f1=_tmp3B7;});({struct Cyc_String_pa_PrintArg_struct _tmp21;_tmp21.tag=0;({struct _dyneither_ptr _tmp3B8=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 184
Cyc_Port_ctype2string(deep,_tmp46));_tmp21.f1=_tmp3B8;});({void*_tmp1F[3]={& _tmp21,& _tmp22,& _tmp23};({struct _dyneither_ptr _tmp3B9=({const char*_tmp20="array(%s,%s,%s)";_tag_dyneither(_tmp20,sizeof(char),16);});Cyc_aprintf(_tmp3B9,_tag_dyneither(_tmp1F,sizeof(void*),3));});});});});});case 13: _LL1B: _tmp48=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpA)->f1)->f1;_tmp47=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpA)->f1)->f2;_LL1C: {
# 187
struct _dyneither_ptr s=_tmp48->kind == Cyc_Absyn_StructA?({const char*_tmp2D="struct";_tag_dyneither(_tmp2D,sizeof(char),7);}):({const char*_tmp2E="union";_tag_dyneither(_tmp2E,sizeof(char),6);});
if(!deep)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp27;_tmp27.tag=0;({struct _dyneither_ptr _tmp3BA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp48->name));_tmp27.f1=_tmp3BA;});({struct Cyc_String_pa_PrintArg_struct _tmp26;_tmp26.tag=0;_tmp26.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp24[2]={& _tmp26,& _tmp27};({struct _dyneither_ptr _tmp3BB=({const char*_tmp25="%s %s";_tag_dyneither(_tmp25,sizeof(char),6);});Cyc_aprintf(_tmp3BB,_tag_dyneither(_tmp24,sizeof(void*),2));});});});});else{
# 191
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2C;_tmp2C.tag=0;({struct _dyneither_ptr _tmp3BC=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Port_cfields2string(0,_tmp47));_tmp2C.f1=_tmp3BC;});({struct Cyc_String_pa_PrintArg_struct _tmp2B;_tmp2B.tag=0;({struct _dyneither_ptr _tmp3BD=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 191
Cyc_Absynpp_qvar2string(_tmp48->name));_tmp2B.f1=_tmp3BD;});({struct Cyc_String_pa_PrintArg_struct _tmp2A;_tmp2A.tag=0;_tmp2A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp28[3]={& _tmp2A,& _tmp2B,& _tmp2C};({struct _dyneither_ptr _tmp3BE=({const char*_tmp29="%s %s {%s}";_tag_dyneither(_tmp29,sizeof(char),11);});Cyc_aprintf(_tmp3BE,_tag_dyneither(_tmp28,sizeof(void*),3));});});});});});}}case 14: if(((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpA)->f2 != 0){_LL1D: _tmp4A=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpA)->f1;_tmp49=*((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpA)->f2;_LL1E:
# 193
 return Cyc_Port_ctype2string(deep,_tmp49);}else{_LL1F: _tmp4B=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpA)->f1;_LL20:
# 195
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp31;_tmp31.tag=0;({struct _dyneither_ptr _tmp3BF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_cfields2string(deep,_tmp4B));_tmp31.f1=_tmp3BF;});({void*_tmp2F[1]={& _tmp31};({struct _dyneither_ptr _tmp3C0=({const char*_tmp30="aggr {%s}";_tag_dyneither(_tmp30,sizeof(char),10);});Cyc_aprintf(_tmp3C0,_tag_dyneither(_tmp2F,sizeof(void*),1));});});});}case 15: _LL21: _tmp4D=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpA)->f1;_tmp4C=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpA)->f2;_LL22:
# 197
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp35;_tmp35.tag=0;({struct _dyneither_ptr _tmp3C1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp4D));_tmp35.f1=_tmp3C1;});({struct Cyc_String_pa_PrintArg_struct _tmp34;_tmp34.tag=0;({struct _dyneither_ptr _tmp3C2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctypes2string(deep,_tmp4C));_tmp34.f1=_tmp3C2;});({void*_tmp32[2]={& _tmp34,& _tmp35};({struct _dyneither_ptr _tmp3C3=({const char*_tmp33="fn(%s)->%s";_tag_dyneither(_tmp33,sizeof(char),11);});Cyc_aprintf(_tmp3C3,_tag_dyneither(_tmp32,sizeof(void*),2));});});});});default: _LL23: _tmp4E=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmpA)->f1;_LL24:
# 199
 if((unsigned int)_tmp4E->eq)
return Cyc_Port_ctype2string(deep,*((void**)_check_null(_tmp4E->eq)));else{
if(!deep  || _tmp4E->uppers == 0  && _tmp4E->lowers == 0)
return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp38;_tmp38.tag=1;_tmp38.f1=(unsigned long)_tmp4E->id;({void*_tmp36[1]={& _tmp38};({struct _dyneither_ptr _tmp3C4=({const char*_tmp37="var(%d)";_tag_dyneither(_tmp37,sizeof(char),8);});Cyc_aprintf(_tmp3C4,_tag_dyneither(_tmp36,sizeof(void*),1));});});});else{
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3D;_tmp3D.tag=0;({struct _dyneither_ptr _tmp3C5=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 206
Cyc_Port_ctypes2string(0,_tmp4E->uppers));_tmp3D.f1=_tmp3C5;});({struct Cyc_Int_pa_PrintArg_struct _tmp3C;_tmp3C.tag=1;_tmp3C.f1=(unsigned long)_tmp4E->id;({struct Cyc_String_pa_PrintArg_struct _tmp3B;_tmp3B.tag=0;({struct _dyneither_ptr _tmp3C6=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 204
Cyc_Port_ctypes2string(0,_tmp4E->lowers));_tmp3B.f1=_tmp3C6;});({void*_tmp39[3]={& _tmp3B,& _tmp3C,& _tmp3D};({struct _dyneither_ptr _tmp3C7=({const char*_tmp3A="var([%s]<=%d<=[%s])";_tag_dyneither(_tmp3A,sizeof(char),20);});Cyc_aprintf(_tmp3C7,_tag_dyneither(_tmp39,sizeof(void*),3));});});});});});}}}_LL0:;}
# 209
static struct _dyneither_ptr*Cyc_Port_ctype2stringptr(int deep,void*t){return({struct _dyneither_ptr*_tmp4F=_cycalloc(sizeof(*_tmp4F));({struct _dyneither_ptr _tmp3C8=Cyc_Port_ctype2string(deep,t);_tmp4F[0]=_tmp3C8;});_tmp4F;});}
struct Cyc_List_List*Cyc_Port_sep(struct _dyneither_ptr s,struct Cyc_List_List*xs){
struct _dyneither_ptr*_tmp50=({struct _dyneither_ptr*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54[0]=s;_tmp54;});
if(xs == 0)return xs;{
struct Cyc_List_List*_tmp51=xs;
struct Cyc_List_List*_tmp52=xs->tl;
for(0;_tmp52 != 0;(_tmp51=_tmp52,_tmp52=_tmp52->tl)){
({struct Cyc_List_List*_tmp3C9=({struct Cyc_List_List*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->hd=_tmp50;_tmp53->tl=_tmp52;_tmp53;});_tmp51->tl=_tmp3C9;});}
# 218
return xs;};}struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
# 220
static struct _dyneither_ptr*Cyc_Port_cfield2stringptr(int deep,struct Cyc_Port_Cfield*f){
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5A;_tmp5A.tag=0;({struct _dyneither_ptr _tmp3CA=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Port_ctype2string(deep,f->type));_tmp5A.f1=_tmp3CA;});({struct Cyc_String_pa_PrintArg_struct _tmp59;_tmp59.tag=0;_tmp59.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f->f);({struct Cyc_String_pa_PrintArg_struct _tmp58;_tmp58.tag=0;({struct _dyneither_ptr _tmp3CB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,f->qual));_tmp58.f1=_tmp3CB;});({void*_tmp56[3]={& _tmp58,& _tmp59,& _tmp5A};({struct _dyneither_ptr _tmp3CC=({const char*_tmp57="%s %s: %s";_tag_dyneither(_tmp57,sizeof(char),10);});Cyc_aprintf(_tmp3CC,_tag_dyneither(_tmp56,sizeof(void*),3));});});});});});
return({struct _dyneither_ptr*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55[0]=s;_tmp55;});}
# 225
static struct _dyneither_ptr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*ts){
return(struct _dyneither_ptr)Cyc_strconcat_l(({struct _dyneither_ptr _tmp3CD=({const char*_tmp5B=",";_tag_dyneither(_tmp5B,sizeof(char),2);});Cyc_Port_sep(_tmp3CD,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_ctype2stringptr,deep,ts));}));}struct Cyc_Port_Cfield;
# 228
static struct _dyneither_ptr Cyc_Port_cfields2string(int deep,struct Cyc_List_List*fs){
return(struct _dyneither_ptr)Cyc_strconcat_l(({struct _dyneither_ptr _tmp3CE=({const char*_tmp5C=";";_tag_dyneither(_tmp5C,sizeof(char),2);});Cyc_Port_sep(_tmp3CE,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(int,struct Cyc_Port_Cfield*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_cfield2stringptr,deep,fs));}));}
# 234
static void*Cyc_Port_notconst_ct(){return(void*)& Cyc_Port_Notconst_ct_val;}
static void*Cyc_Port_const_ct(){return(void*)& Cyc_Port_Const_ct_val;}
static void*Cyc_Port_thin_ct(){return(void*)& Cyc_Port_Thin_ct_val;}
static void*Cyc_Port_fat_ct(){return(void*)& Cyc_Port_Fat_ct_val;}
static void*Cyc_Port_void_ct(){return(void*)& Cyc_Port_Void_ct_val;}
static void*Cyc_Port_zero_ct(){return(void*)& Cyc_Port_Zero_ct_val;}
static void*Cyc_Port_arith_ct(){return(void*)& Cyc_Port_Arith_ct_val;}
static void*Cyc_Port_heap_ct(){return(void*)& Cyc_Port_Heap_ct_val;}
static void*Cyc_Port_zterm_ct(){return(void*)& Cyc_Port_Zterm_ct_val;}
static void*Cyc_Port_nozterm_ct(){return(void*)& Cyc_Port_Nozterm_ct_val;}
static void*Cyc_Port_rgnvar_ct(struct _dyneither_ptr*n){return(void*)({struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmp5D=_cycalloc(sizeof(*_tmp5D));({struct Cyc_Port_RgnVar_ct_Port_Ctype_struct _tmp3CF=({struct Cyc_Port_RgnVar_ct_Port_Ctype_struct _tmp5E;_tmp5E.tag=10;_tmp5E.f1=n;_tmp5E;});_tmp5D[0]=_tmp3CF;});_tmp5D;});}
static void*Cyc_Port_unknown_aggr_ct(struct Cyc_List_List*fs){
return(void*)({struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));({struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct _tmp3D0=({struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct _tmp60;_tmp60.tag=14;_tmp60.f1=fs;_tmp60.f2=0;_tmp60;});_tmp5F[0]=_tmp3D0;});_tmp5F;});}
# 248
static void*Cyc_Port_known_aggr_ct(struct _tuple11*p){
return(void*)({struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmp61=_cycalloc(sizeof(*_tmp61));({struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct _tmp3D1=({struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct _tmp62;_tmp62.tag=13;_tmp62.f1=p;_tmp62;});_tmp61[0]=_tmp3D1;});_tmp61;});}
# 251
static void*Cyc_Port_ptr_ct(void*elt,void*qual,void*ptr_kind,void*r,void*zt){
# 253
return(void*)({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp63=_cycalloc(sizeof(*_tmp63));({struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmp3D2=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmp64;_tmp64.tag=11;_tmp64.f1=elt;_tmp64.f2=qual;_tmp64.f3=ptr_kind;_tmp64.f4=r;_tmp64.f5=zt;_tmp64;});_tmp63[0]=_tmp3D2;});_tmp63;});}
# 255
static void*Cyc_Port_array_ct(void*elt,void*qual,void*zt){
return(void*)({struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp65=_cycalloc(sizeof(*_tmp65));({struct Cyc_Port_Array_ct_Port_Ctype_struct _tmp3D3=({struct Cyc_Port_Array_ct_Port_Ctype_struct _tmp66;_tmp66.tag=12;_tmp66.f1=elt;_tmp66.f2=qual;_tmp66.f3=zt;_tmp66;});_tmp65[0]=_tmp3D3;});_tmp65;});}
# 258
static void*Cyc_Port_fn_ct(void*return_type,struct Cyc_List_List*args){
return(void*)({struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp67=_cycalloc(sizeof(*_tmp67));({struct Cyc_Port_Fn_ct_Port_Ctype_struct _tmp3D4=({struct Cyc_Port_Fn_ct_Port_Ctype_struct _tmp68;_tmp68.tag=15;_tmp68.f1=return_type;_tmp68.f2=args;_tmp68;});_tmp67[0]=_tmp3D4;});_tmp67;});}struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
# 261
static void*Cyc_Port_var(){
static int counter=0;
return(void*)({struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmp69=_cycalloc(sizeof(*_tmp69));({struct Cyc_Port_Var_ct_Port_Ctype_struct _tmp3D6=({struct Cyc_Port_Var_ct_Port_Ctype_struct _tmp6A;_tmp6A.tag=16;({struct Cyc_Port_Cvar*_tmp3D5=({struct Cyc_Port_Cvar*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->id=counter ++;_tmp6B->eq=0;_tmp6B->uppers=0;_tmp6B->lowers=0;_tmp6B;});_tmp6A.f1=_tmp3D5;});_tmp6A;});_tmp69[0]=_tmp3D6;});_tmp69;});}
# 265
static void*Cyc_Port_new_var(void*x){
return Cyc_Port_var();}
# 268
static struct _dyneither_ptr*Cyc_Port_new_region_var(){
static int counter=0;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp6F;_tmp6F.tag=1;_tmp6F.f1=(unsigned long)counter ++;({void*_tmp6D[1]={& _tmp6F};({struct _dyneither_ptr _tmp3D7=({const char*_tmp6E="`R%d";_tag_dyneither(_tmp6E,sizeof(char),5);});Cyc_aprintf(_tmp3D7,_tag_dyneither(_tmp6D,sizeof(void*),1));});});});
return({struct _dyneither_ptr*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C[0]=s;_tmp6C;});}
# 276
static int Cyc_Port_unifies(void*t1,void*t2);struct Cyc_Port_Cvar;
# 278
static void*Cyc_Port_compress_ct(void*t){
void*_tmp70=t;void**_tmp7C;void***_tmp7B;struct Cyc_List_List*_tmp7A;struct Cyc_List_List*_tmp79;switch(*((int*)_tmp70)){case 16: _LL26: _tmp7B=(void***)&(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp70)->f1)->eq;_tmp7A=(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp70)->f1)->uppers;_tmp79=(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp70)->f1)->lowers;_LL27: {
# 281
void**_tmp71=*_tmp7B;
if((unsigned int)_tmp71){
# 285
void*r=Cyc_Port_compress_ct(*_tmp71);
if(*_tmp71 != r)({void**_tmp3D8=({void**_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72[0]=r;_tmp72;});*_tmp7B=_tmp3D8;});
return r;}
# 292
for(0;_tmp79 != 0;_tmp79=_tmp79->tl){
void*_tmp73=(void*)_tmp79->hd;void*_tmp74=_tmp73;switch(*((int*)_tmp74)){case 0: _LL2D: _LL2E:
 goto _LL30;case 9: _LL2F: _LL30:
 goto _LL32;case 4: _LL31: _LL32:
 goto _LL34;case 13: _LL33: _LL34:
 goto _LL36;case 15: _LL35: _LL36:
# 299
({void**_tmp3D9=({void**_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75[0]=(void*)_tmp79->hd;_tmp75;});*_tmp7B=_tmp3D9;});
return(void*)_tmp79->hd;default: _LL37: _LL38:
# 302
 goto _LL2C;}_LL2C:;}
# 305
for(0;_tmp7A != 0;_tmp7A=_tmp7A->tl){
void*_tmp76=(void*)_tmp7A->hd;void*_tmp77=_tmp76;switch(*((int*)_tmp77)){case 1: _LL3A: _LL3B:
 goto _LL3D;case 8: _LL3C: _LL3D:
 goto _LL3F;case 5: _LL3E: _LL3F:
 goto _LL41;case 13: _LL40: _LL41:
 goto _LL43;case 15: _LL42: _LL43:
# 312
({void**_tmp3DA=({void**_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78[0]=(void*)_tmp7A->hd;_tmp78;});*_tmp7B=_tmp3DA;});
return(void*)_tmp7A->hd;default: _LL44: _LL45:
# 315
 goto _LL39;}_LL39:;}
# 318
return t;}case 14: _LL28: _tmp7C=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp70)->f2;_LL29:
# 321
 if((unsigned int)_tmp7C)return Cyc_Port_compress_ct(*_tmp7C);else{
return t;}default: _LL2A: _LL2B:
# 325
 return t;}_LL25:;}
# 331
static void*Cyc_Port_inst(struct Cyc_Dict_Dict*instenv,void*t){
void*_tmp7D=Cyc_Port_compress_ct(t);void*_tmp7E=_tmp7D;void*_tmp95;struct Cyc_List_List*_tmp94;void*_tmp93;void*_tmp92;void*_tmp91;void*_tmp90;void*_tmp8F;void*_tmp8E;void*_tmp8D;void*_tmp8C;struct _dyneither_ptr*_tmp8B;switch(*((int*)_tmp7E)){case 0: _LL47: _LL48:
 goto _LL4A;case 1: _LL49: _LL4A:
 goto _LL4C;case 2: _LL4B: _LL4C:
 goto _LL4E;case 3: _LL4D: _LL4E:
 goto _LL50;case 4: _LL4F: _LL50:
 goto _LL52;case 5: _LL51: _LL52:
 goto _LL54;case 6: _LL53: _LL54:
 goto _LL56;case 8: _LL55: _LL56:
 goto _LL58;case 9: _LL57: _LL58:
 goto _LL5A;case 14: _LL59: _LL5A:
 goto _LL5C;case 13: _LL5B: _LL5C:
 goto _LL5E;case 16: _LL5D: _LL5E:
 goto _LL60;case 7: _LL5F: _LL60:
 return t;case 10: _LL61: _tmp8B=((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp7E)->f1;_LL62:
# 347
 if(!((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*instenv,_tmp8B))
({struct Cyc_Dict_Dict _tmp3DD=({struct Cyc_Dict_Dict _tmp3DC=*instenv;struct _dyneither_ptr*_tmp3DB=_tmp8B;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp3DC,_tmp3DB,Cyc_Port_var());});*instenv=_tmp3DD;});
return((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*instenv,_tmp8B);case 11: _LL63: _tmp90=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp7E)->f1;_tmp8F=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp7E)->f2;_tmp8E=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp7E)->f3;_tmp8D=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp7E)->f4;_tmp8C=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp7E)->f5;_LL64: {
# 351
struct _tuple10 _tmp7F=({struct _tuple10 _tmp85;({void*_tmp3DE=Cyc_Port_inst(instenv,_tmp90);_tmp85.f1=_tmp3DE;});({void*_tmp3DF=Cyc_Port_inst(instenv,_tmp8D);_tmp85.f2=_tmp3DF;});_tmp85;});struct _tuple10 _tmp80=_tmp7F;void*_tmp84;void*_tmp83;_LL6A: _tmp84=_tmp80.f1;_tmp83=_tmp80.f2;_LL6B:;
if(_tmp84 == _tmp90  && _tmp83 == _tmp8D)return t;
return(void*)({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp81=_cycalloc(sizeof(*_tmp81));({struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmp3E0=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmp82;_tmp82.tag=11;_tmp82.f1=_tmp84;_tmp82.f2=_tmp8F;_tmp82.f3=_tmp8E;_tmp82.f4=_tmp83;_tmp82.f5=_tmp8C;_tmp82;});_tmp81[0]=_tmp3E0;});_tmp81;});}case 12: _LL65: _tmp93=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp7E)->f1;_tmp92=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp7E)->f2;_tmp91=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp7E)->f3;_LL66: {
# 355
void*_tmp86=Cyc_Port_inst(instenv,_tmp93);
if(_tmp86 == _tmp93)return t;
return(void*)({struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp87=_cycalloc(sizeof(*_tmp87));({struct Cyc_Port_Array_ct_Port_Ctype_struct _tmp3E1=({struct Cyc_Port_Array_ct_Port_Ctype_struct _tmp88;_tmp88.tag=12;_tmp88.f1=_tmp86;_tmp88.f2=_tmp92;_tmp88.f3=_tmp91;_tmp88;});_tmp87[0]=_tmp3E1;});_tmp87;});}default: _LL67: _tmp95=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp7E)->f1;_tmp94=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp7E)->f2;_LL68:
# 359
 return(void*)({struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp89=_cycalloc(sizeof(*_tmp89));({struct Cyc_Port_Fn_ct_Port_Ctype_struct _tmp3E4=({struct Cyc_Port_Fn_ct_Port_Ctype_struct _tmp8A;_tmp8A.tag=15;({void*_tmp3E2=Cyc_Port_inst(instenv,_tmp95);_tmp8A.f1=_tmp3E2;});({struct Cyc_List_List*_tmp3E3=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Dict_Dict*,void*),struct Cyc_Dict_Dict*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_inst,instenv,_tmp94);_tmp8A.f2=_tmp3E3;});_tmp8A;});_tmp89[0]=_tmp3E4;});_tmp89;});}_LL46:;}
# 363
void*Cyc_Port_instantiate(void*t){
return({struct Cyc_Dict_Dict*_tmp3E6=({struct Cyc_Dict_Dict*_tmp96=_cycalloc(sizeof(*_tmp96));({struct Cyc_Dict_Dict _tmp3E5=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp96[0]=_tmp3E5;});_tmp96;});Cyc_Port_inst(_tmp3E6,t);});}
# 370
struct Cyc_List_List*Cyc_Port_filter_self(void*t,struct Cyc_List_List*ts){
int found=0;
{struct Cyc_List_List*_tmp97=ts;for(0;(unsigned int)_tmp97;_tmp97=_tmp97->tl){
void*_tmp98=Cyc_Port_compress_ct((void*)_tmp97->hd);
if(t == _tmp98)found=1;}}
# 376
if(!found)return ts;{
struct Cyc_List_List*res=0;
for(0;ts != 0;ts=ts->tl){
if(({void*_tmp3E7=t;_tmp3E7 == Cyc_Port_compress_ct((void*)ts->hd);}))continue;
({struct Cyc_List_List*_tmp3E8=({struct Cyc_List_List*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->hd=(void*)ts->hd;_tmp99->tl=res;_tmp99;});res=_tmp3E8;});}
# 382
return res;};}struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
# 387
void Cyc_Port_generalize(int is_rgn,void*t){
({void*_tmp3E9=Cyc_Port_compress_ct(t);t=_tmp3E9;});{
void*_tmp9A=t;void*_tmpA5;struct Cyc_List_List*_tmpA4;void*_tmpA3;void*_tmpA2;void*_tmpA1;void*_tmpA0;void*_tmp9F;void*_tmp9E;void*_tmp9D;void*_tmp9C;struct Cyc_Port_Cvar*_tmp9B;switch(*((int*)_tmp9A)){case 16: _LL6D: _tmp9B=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp9A)->f1;_LL6E:
# 392
({struct Cyc_List_List*_tmp3EA=Cyc_Port_filter_self(t,_tmp9B->uppers);_tmp9B->uppers=_tmp3EA;});
({struct Cyc_List_List*_tmp3EB=Cyc_Port_filter_self(t,_tmp9B->lowers);_tmp9B->lowers=_tmp3EB;});
if(is_rgn){
# 397
if(_tmp9B->uppers == 0  && _tmp9B->lowers == 0){
({void*_tmp3EC=t;Cyc_Port_unifies(_tmp3EC,Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var()));});
return;}
# 402
if((unsigned int)_tmp9B->uppers){
Cyc_Port_unifies(t,(void*)((struct Cyc_List_List*)_check_null(_tmp9B->uppers))->hd);
Cyc_Port_generalize(1,t);}else{
# 406
Cyc_Port_unifies(t,(void*)((struct Cyc_List_List*)_check_null(_tmp9B->lowers))->hd);
Cyc_Port_generalize(1,t);}}
# 410
return;case 0: _LL6F: _LL70:
 goto _LL72;case 1: _LL71: _LL72:
 goto _LL74;case 2: _LL73: _LL74:
 goto _LL76;case 3: _LL75: _LL76:
 goto _LL78;case 4: _LL77: _LL78:
 goto _LL7A;case 5: _LL79: _LL7A:
 goto _LL7C;case 6: _LL7B: _LL7C:
 goto _LL7E;case 14: _LL7D: _LL7E:
 goto _LL80;case 13: _LL7F: _LL80:
 goto _LL82;case 10: _LL81: _LL82:
 goto _LL84;case 9: _LL83: _LL84:
 goto _LL86;case 8: _LL85: _LL86:
 goto _LL88;case 7: _LL87: _LL88:
 return;case 11: _LL89: _tmpA0=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9A)->f1;_tmp9F=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9A)->f2;_tmp9E=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9A)->f3;_tmp9D=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9A)->f4;_tmp9C=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9A)->f5;_LL8A:
# 427
 Cyc_Port_generalize(0,_tmpA0);Cyc_Port_generalize(1,_tmp9D);goto _LL6C;case 12: _LL8B: _tmpA3=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9A)->f1;_tmpA2=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9A)->f2;_tmpA1=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9A)->f3;_LL8C:
# 429
 Cyc_Port_generalize(0,_tmpA3);Cyc_Port_generalize(0,_tmpA2);goto _LL6C;default: _LL8D: _tmpA5=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp9A)->f1;_tmpA4=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp9A)->f2;_LL8E:
# 431
 Cyc_Port_generalize(0,_tmpA5);((void(*)(void(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Port_generalize,0,_tmpA4);goto _LL6C;}_LL6C:;};}struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
# 437
static int Cyc_Port_occurs(void*v,void*t){
({void*_tmp3ED=Cyc_Port_compress_ct(t);t=_tmp3ED;});
if(t == v)return 1;{
void*_tmpA6=t;struct Cyc_List_List*_tmpB2;struct Cyc_List_List*_tmpB1;void*_tmpB0;struct Cyc_List_List*_tmpAF;void*_tmpAE;void*_tmpAD;void*_tmpAC;void*_tmpAB;void*_tmpAA;void*_tmpA9;void*_tmpA8;void*_tmpA7;switch(*((int*)_tmpA6)){case 11: _LL90: _tmpAB=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA6)->f1;_tmpAA=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA6)->f2;_tmpA9=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA6)->f3;_tmpA8=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA6)->f4;_tmpA7=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA6)->f5;_LL91:
# 442
 return(((Cyc_Port_occurs(v,_tmpAB) || Cyc_Port_occurs(v,_tmpAA)) || Cyc_Port_occurs(v,_tmpA9)) || Cyc_Port_occurs(v,_tmpA8)) || 
Cyc_Port_occurs(v,_tmpA7);case 12: _LL92: _tmpAE=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpA6)->f1;_tmpAD=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpA6)->f2;_tmpAC=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpA6)->f3;_LL93:
# 445
 return(Cyc_Port_occurs(v,_tmpAE) || Cyc_Port_occurs(v,_tmpAD)) || Cyc_Port_occurs(v,_tmpAC);case 15: _LL94: _tmpB0=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpA6)->f1;_tmpAF=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpA6)->f2;_LL95:
# 447
 if(Cyc_Port_occurs(v,_tmpB0))return 1;
for(0;(unsigned int)_tmpAF;_tmpAF=_tmpAF->tl){
if(Cyc_Port_occurs(v,(void*)_tmpAF->hd))return 1;}
return 0;case 13: _LL96: _tmpB1=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpA6)->f1)->f2;_LL97:
 return 0;case 14: _LL98: _tmpB2=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpA6)->f1;_LL99:
# 453
 for(0;(unsigned int)_tmpB2;_tmpB2=_tmpB2->tl){
if(Cyc_Port_occurs(v,((struct Cyc_Port_Cfield*)_tmpB2->hd)->qual) || Cyc_Port_occurs(v,((struct Cyc_Port_Cfield*)_tmpB2->hd)->type))return 1;}
return 0;default: _LL9A: _LL9B:
 return 0;}_LL8F:;};}char Cyc_Port_Unify_ct[9]="Unify_ct";struct Cyc_Port_Unify_ct_exn_struct{char*tag;};
# 465
struct Cyc_Port_Unify_ct_exn_struct Cyc_Port_Unify_ct_val={Cyc_Port_Unify_ct};
# 467
static int Cyc_Port_leq(void*t1,void*t2);
static void Cyc_Port_unify_cts(struct Cyc_List_List*t1,struct Cyc_List_List*t2);
static struct Cyc_List_List*Cyc_Port_merge_fields(struct Cyc_List_List*fs1,struct Cyc_List_List*fs2,int allow_subset);struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
# 471
static void Cyc_Port_unify_ct(void*t1,void*t2){
({void*_tmp3EE=Cyc_Port_compress_ct(t1);t1=_tmp3EE;});
({void*_tmp3EF=Cyc_Port_compress_ct(t2);t2=_tmp3EF;});
if(t1 == t2)return;{
struct _tuple10 _tmpB4=({struct _tuple10 _tmpE3;_tmpE3.f1=t1;_tmpE3.f2=t2;_tmpE3;});struct _tuple10 _tmpB5=_tmpB4;struct Cyc_List_List*_tmpE2;void***_tmpE1;struct Cyc_Absyn_Aggrdecl*_tmpE0;struct Cyc_List_List*_tmpDF;struct Cyc_List_List*_tmpDE;void***_tmpDD;struct Cyc_List_List*_tmpDC;void***_tmpDB;struct Cyc_Absyn_Aggrdecl*_tmpDA;struct Cyc_List_List*_tmpD9;struct Cyc_List_List*_tmpD8;void***_tmpD7;struct _tuple11*_tmpD6;struct _tuple11*_tmpD5;void*_tmpD4;struct Cyc_List_List*_tmpD3;void*_tmpD2;struct Cyc_List_List*_tmpD1;void*_tmpD0;void*_tmpCF;void*_tmpCE;void*_tmpCD;void*_tmpCC;void*_tmpCB;struct _dyneither_ptr _tmpCA;struct _dyneither_ptr _tmpC9;void*_tmpC8;void*_tmpC7;void*_tmpC6;void*_tmpC5;void*_tmpC4;void*_tmpC3;void*_tmpC2;void*_tmpC1;void*_tmpC0;void*_tmpBF;struct Cyc_Port_Cvar*_tmpBE;struct Cyc_Port_Cvar*_tmpBD;if(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmpB5.f1)->tag == 16){_LL9D: _tmpBD=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmpB5.f1)->f1;_LL9E:
# 477
 if(!Cyc_Port_occurs(t1,t2)){
# 480
{struct Cyc_List_List*_tmpB6=Cyc_Port_filter_self(t1,_tmpBD->uppers);for(0;(unsigned int)_tmpB6;_tmpB6=_tmpB6->tl){
if(!Cyc_Port_leq(t2,(void*)_tmpB6->hd))(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}
{struct Cyc_List_List*_tmpB7=Cyc_Port_filter_self(t1,_tmpBD->lowers);for(0;(unsigned int)_tmpB7;_tmpB7=_tmpB7->tl){
if(!Cyc_Port_leq((void*)_tmpB7->hd,t2))(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}
({void**_tmp3F0=({void**_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8[0]=t2;_tmpB8;});_tmpBD->eq=_tmp3F0;});
return;}else{
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}else{if(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmpB5.f2)->tag == 16){_LL9F: _tmpBE=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmpB5.f2)->f1;_LLA0:
 Cyc_Port_unify_ct(t2,t1);return;}else{switch(*((int*)_tmpB5.f1)){case 11: if(((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f2)->tag == 11){_LLA1: _tmpC8=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f1)->f1;_tmpC7=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f1)->f2;_tmpC6=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f1)->f3;_tmpC5=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f1)->f4;_tmpC4=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f1)->f5;_tmpC3=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f2)->f1;_tmpC2=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f2)->f2;_tmpC1=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f2)->f3;_tmpC0=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f2)->f4;_tmpBF=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f2)->f5;_LLA2:
# 489
 Cyc_Port_unify_ct(_tmpC8,_tmpC3);Cyc_Port_unify_ct(_tmpC7,_tmpC2);Cyc_Port_unify_ct(_tmpC6,_tmpC1);Cyc_Port_unify_ct(_tmpC5,_tmpC0);
Cyc_Port_unify_ct(_tmpC4,_tmpBF);
return;}else{goto _LLB1;}case 10: if(((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmpB5.f2)->tag == 10){_LLA3: _tmpCA=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmpB5.f1)->f1;_tmpC9=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmpB5.f2)->f1;_LLA4:
# 493
 if(({Cyc_strcmp((struct _dyneither_ptr)_tmpCA,(struct _dyneither_ptr)_tmpC9)!= 0;}))(int)_throw((void*)& Cyc_Port_Unify_ct_val);
return;}else{goto _LLB1;}case 12: if(((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpB5.f2)->tag == 12){_LLA5: _tmpD0=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpB5.f1)->f1;_tmpCF=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpB5.f1)->f2;_tmpCE=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpB5.f1)->f3;_tmpCD=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpB5.f2)->f1;_tmpCC=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpB5.f2)->f2;_tmpCB=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpB5.f2)->f3;_LLA6:
# 496
 Cyc_Port_unify_ct(_tmpD0,_tmpCD);Cyc_Port_unify_ct(_tmpCF,_tmpCC);Cyc_Port_unify_ct(_tmpCE,_tmpCB);return;}else{goto _LLB1;}case 15: if(((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpB5.f2)->tag == 15){_LLA7: _tmpD4=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpB5.f1)->f1;_tmpD3=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpB5.f1)->f2;_tmpD2=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpB5.f2)->f1;_tmpD1=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpB5.f2)->f2;_LLA8:
# 498
 Cyc_Port_unify_ct(_tmpD4,_tmpD2);Cyc_Port_unify_cts(_tmpD3,_tmpD1);return;}else{goto _LLB1;}case 13: switch(*((int*)_tmpB5.f2)){case 13: _LLA9: _tmpD6=((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpB5.f1)->f1;_tmpD5=((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpB5.f2)->f1;_LLAA:
# 500
 if(_tmpD6 == _tmpD5)return;else{(int)_throw((void*)& Cyc_Port_Unify_ct_val);}case 14: _LLAF: _tmpDA=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpB5.f1)->f1)->f1;_tmpD9=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpB5.f1)->f1)->f2;_tmpD8=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpB5.f2)->f1;_tmpD7=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpB5.f2)->f2;_LLB0:
# 510
 Cyc_Port_merge_fields(_tmpD9,_tmpD8,0);
({void**_tmp3F1=({void**_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=t1;_tmpBC;});*_tmpD7=_tmp3F1;});
return;default: goto _LLB1;}case 14: switch(*((int*)_tmpB5.f2)){case 14: _LLAB: _tmpDE=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpB5.f1)->f1;_tmpDD=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpB5.f1)->f2;_tmpDC=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpB5.f2)->f1;_tmpDB=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpB5.f2)->f2;_LLAC: {
# 502
void*_tmpB9=Cyc_Port_unknown_aggr_ct(Cyc_Port_merge_fields(_tmpDE,_tmpDC,1));
({void**_tmp3F3=({void**_tmp3F2=({void**_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA[0]=_tmpB9;_tmpBA;});*_tmpDB=_tmp3F2;});*_tmpDD=_tmp3F3;});
return;}case 13: _LLAD: _tmpE2=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpB5.f1)->f1;_tmpE1=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpB5.f1)->f2;_tmpE0=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpB5.f2)->f1)->f1;_tmpDF=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpB5.f2)->f1)->f2;_LLAE:
# 506
 Cyc_Port_merge_fields(_tmpDF,_tmpE2,0);
({void**_tmp3F4=({void**_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB[0]=t2;_tmpBB;});*_tmpE1=_tmp3F4;});
return;default: goto _LLB1;}default: _LLB1: _LLB2:
# 513
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}}_LL9C:;};}
# 517
static void Cyc_Port_unify_cts(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Port_unify_ct((void*)t1->hd,(void*)t2->hd);}
# 521
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
# 527
static struct Cyc_List_List*Cyc_Port_merge_fields(struct Cyc_List_List*fs1,struct Cyc_List_List*fs2,int allow_f1_subset_f2){
# 529
struct Cyc_List_List*common=0;
{struct Cyc_List_List*_tmpE4=fs2;for(0;(unsigned int)_tmpE4;_tmpE4=_tmpE4->tl){
struct Cyc_Port_Cfield*_tmpE5=(struct Cyc_Port_Cfield*)_tmpE4->hd;
int found=0;
{struct Cyc_List_List*_tmpE6=fs1;for(0;(unsigned int)_tmpE6;_tmpE6=_tmpE6->tl){
struct Cyc_Port_Cfield*_tmpE7=(struct Cyc_Port_Cfield*)_tmpE6->hd;
if(({Cyc_strptrcmp(_tmpE7->f,_tmpE5->f)== 0;})){
({struct Cyc_List_List*_tmp3F5=({struct Cyc_List_List*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->hd=_tmpE7;_tmpE8->tl=common;_tmpE8;});common=_tmp3F5;});
Cyc_Port_unify_ct(_tmpE7->qual,_tmpE5->qual);
Cyc_Port_unify_ct(_tmpE7->type,_tmpE5->type);
found=1;
break;}}}
# 543
if(!found){
if(allow_f1_subset_f2)
({struct Cyc_List_List*_tmp3F6=({struct Cyc_List_List*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->hd=_tmpE5;_tmpE9->tl=common;_tmpE9;});common=_tmp3F6;});else{
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}}}
# 549
{struct Cyc_List_List*_tmpEA=fs1;for(0;(unsigned int)_tmpEA;_tmpEA=_tmpEA->tl){
struct Cyc_Port_Cfield*_tmpEB=(struct Cyc_Port_Cfield*)_tmpEA->hd;
int found=0;
{struct Cyc_List_List*_tmpEC=fs2;for(0;(unsigned int)_tmpEC;_tmpEC=_tmpEC->tl){
struct Cyc_Port_Cfield*_tmpED=(struct Cyc_Port_Cfield*)_tmpEC->hd;
if(Cyc_strptrcmp(_tmpEB->f,_tmpED->f))found=1;}}
# 556
if(!found)
({struct Cyc_List_List*_tmp3F7=({struct Cyc_List_List*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE->hd=_tmpEB;_tmpEE->tl=common;_tmpEE;});common=_tmp3F7;});}}
# 559
return common;}
# 562
static int Cyc_Port_unifies(void*t1,void*t2){
{struct _handler_cons _tmpEF;_push_handler(& _tmpEF);{int _tmpF1=0;if(setjmp(_tmpEF.handler))_tmpF1=1;if(!_tmpF1){
# 569
Cyc_Port_unify_ct(t1,t2);;_pop_handler();}else{void*_tmpF0=(void*)_exn_thrown;void*_tmpF2=_tmpF0;void*_tmpF3;if(((struct Cyc_Port_Unify_ct_exn_struct*)_tmpF2)->tag == Cyc_Port_Unify_ct){_LLB4: _LLB5:
# 576
 return 0;}else{_LLB6: _tmpF3=_tmpF2;_LLB7:(int)_rethrow(_tmpF3);}_LLB3:;}};}
# 578
return 1;}struct _tuple12{void*f1;void*f2;void*f3;void*f4;void*f5;};
# 584
static struct Cyc_List_List*Cyc_Port_insert_upper(void*v,void*t,struct Cyc_List_List**uppers){
# 586
({void*_tmp3F8=Cyc_Port_compress_ct(t);t=_tmp3F8;});
{void*_tmpF4=t;switch(*((int*)_tmpF4)){case 1: _LLB9: _LLBA:
# 590
 goto _LLBC;case 8: _LLBB: _LLBC:
 goto _LLBE;case 5: _LLBD: _LLBE:
 goto _LLC0;case 2: _LLBF: _LLC0:
 goto _LLC2;case 3: _LLC1: _LLC2:
 goto _LLC4;case 12: _LLC3: _LLC4:
 goto _LLC6;case 13: _LLC5: _LLC6:
 goto _LLC8;case 15: _LLC7: _LLC8:
 goto _LLCA;case 7: _LLC9: _LLCA:
# 601
*uppers=0;
Cyc_Port_unifies(v,t);
return*uppers;case 4: _LLCB: _LLCC:
# 606
 goto _LLCE;case 0: _LLCD: _LLCE:
 goto _LLD0;case 9: _LLCF: _LLD0:
# 609
 return*uppers;default: _LLD1: _LLD2:
 goto _LLB8;}_LLB8:;}
# 613
{struct Cyc_List_List*_tmpF5=*uppers;for(0;(unsigned int)_tmpF5;_tmpF5=_tmpF5->tl){
void*_tmpF6=Cyc_Port_compress_ct((void*)_tmpF5->hd);
# 616
if(t == _tmpF6)return*uppers;{
struct _tuple10 _tmpF7=({struct _tuple10 _tmp10E;_tmp10E.f1=t;_tmp10E.f2=_tmpF6;_tmp10E;});struct _tuple10 _tmpF8=_tmpF7;void*_tmp10D;void*_tmp10C;void*_tmp10B;void*_tmp10A;void*_tmp109;void*_tmp108;void*_tmp107;void*_tmp106;void*_tmp105;void*_tmp104;switch(*((int*)_tmpF8.f1)){case 6: switch(*((int*)_tmpF8.f2)){case 11: _LLD4: _LLD5:
# 621
 goto _LLD7;case 5: _LLD6: _LLD7:
 goto _LLD9;case 12: _LLD8: _LLD9:
# 624
 return*uppers;default: goto _LLDC;}case 11: if(((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f2)->tag == 11){_LLDA: _tmp10D=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f1)->f1;_tmp10C=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f1)->f2;_tmp10B=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f1)->f3;_tmp10A=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f1)->f4;_tmp109=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f1)->f5;_tmp108=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f2)->f1;_tmp107=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f2)->f2;_tmp106=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f2)->f3;_tmp105=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f2)->f4;_tmp104=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f2)->f5;_LLDB: {
# 629
struct _tuple12 _tmpF9=({struct _tuple12 _tmp103;({void*_tmp3F9=Cyc_Port_var();_tmp103.f1=_tmp3F9;});({void*_tmp3FA=Cyc_Port_var();_tmp103.f2=_tmp3FA;});({void*_tmp3FB=Cyc_Port_var();_tmp103.f3=_tmp3FB;});({void*_tmp3FC=Cyc_Port_var();_tmp103.f4=_tmp3FC;});({void*_tmp3FD=Cyc_Port_var();_tmp103.f5=_tmp3FD;});_tmp103;});struct _tuple12 _tmpFA=_tmpF9;void*_tmp102;void*_tmp101;void*_tmp100;void*_tmpFF;void*_tmpFE;_LLDF: _tmp102=_tmpFA.f1;_tmp101=_tmpFA.f2;_tmp100=_tmpFA.f3;_tmpFF=_tmpFA.f4;_tmpFE=_tmpFA.f5;_LLE0:;{
struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpFB=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpFC=_cycalloc(sizeof(*_tmpFC));({struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmp3FE=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmpFD;_tmpFD.tag=11;_tmpFD.f1=_tmp102;_tmpFD.f2=_tmp101;_tmpFD.f3=_tmp100;_tmpFD.f4=_tmpFF;_tmpFD.f5=_tmpFE;_tmpFD;});_tmpFC[0]=_tmp3FE;});_tmpFC;});
Cyc_Port_leq(_tmp102,_tmp10D);Cyc_Port_leq(_tmp102,_tmp108);
Cyc_Port_leq(_tmp101,_tmp10C);Cyc_Port_leq(_tmp101,_tmp107);
Cyc_Port_leq(_tmp100,_tmp10B);Cyc_Port_leq(_tmp100,_tmp107);
Cyc_Port_leq(_tmpFF,_tmp10A);Cyc_Port_leq(_tmpFF,_tmp105);
Cyc_Port_leq(_tmpFE,_tmp109);Cyc_Port_leq(_tmpFE,_tmp104);
_tmpF5->hd=(void*)((void*)_tmpFB);
return*uppers;};}}else{goto _LLDC;}default: _LLDC: _LLDD:
 goto _LLD3;}_LLD3:;};}}
# 641
return({struct Cyc_List_List*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->hd=t;_tmp10F->tl=*uppers;_tmp10F;});}
# 646
static struct Cyc_List_List*Cyc_Port_insert_lower(void*v,void*t,struct Cyc_List_List**lowers){
# 648
({void*_tmp3FF=Cyc_Port_compress_ct(t);t=_tmp3FF;});
{void*_tmp110=t;switch(*((int*)_tmp110)){case 0: _LLE2: _LLE3:
 goto _LLE5;case 8: _LLE4: _LLE5:
 goto _LLE7;case 2: _LLE6: _LLE7:
 goto _LLE9;case 3: _LLE8: _LLE9:
 goto _LLEB;case 4: _LLEA: _LLEB:
 goto _LLED;case 13: _LLEC: _LLED:
 goto _LLEF;case 15: _LLEE: _LLEF:
 goto _LLF1;case 10: _LLF0: _LLF1:
# 658
*lowers=0;
Cyc_Port_unifies(v,t);
return*lowers;case 7: _LLF2: _LLF3:
 goto _LLF5;case 1: _LLF4: _LLF5:
 goto _LLF7;case 9: _LLF6: _LLF7:
# 664
 return*lowers;default: _LLF8: _LLF9:
# 666
 goto _LLE1;}_LLE1:;}
# 668
{struct Cyc_List_List*_tmp111=*lowers;for(0;(unsigned int)_tmp111;_tmp111=_tmp111->tl){
void*_tmp112=Cyc_Port_compress_ct((void*)_tmp111->hd);
if(t == _tmp112)return*lowers;{
struct _tuple10 _tmp113=({struct _tuple10 _tmp12A;_tmp12A.f1=t;_tmp12A.f2=_tmp112;_tmp12A;});struct _tuple10 _tmp114=_tmp113;void*_tmp129;void*_tmp128;void*_tmp127;void*_tmp126;void*_tmp125;void*_tmp124;void*_tmp123;void*_tmp122;void*_tmp121;void*_tmp120;if(((struct Cyc_Port_Void_ct_Port_Ctype_struct*)_tmp114.f2)->tag == 4){_LLFB: _LLFC:
 goto _LLFE;}else{switch(*((int*)_tmp114.f1)){case 5: switch(*((int*)_tmp114.f2)){case 6: _LLFD: _LLFE:
 goto _LL100;case 11: _LLFF: _LL100:
 goto _LL102;default: goto _LL107;}case 11: switch(*((int*)_tmp114.f2)){case 6: _LL101: _LL102:
 goto _LL104;case 11: _LL105: _tmp129=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f1)->f1;_tmp128=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f1)->f2;_tmp127=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f1)->f3;_tmp126=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f1)->f4;_tmp125=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f1)->f5;_tmp124=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f2)->f1;_tmp123=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f2)->f2;_tmp122=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f2)->f3;_tmp121=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f2)->f4;_tmp120=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f2)->f5;_LL106: {
# 682
struct _tuple12 _tmp115=({struct _tuple12 _tmp11F;({void*_tmp400=Cyc_Port_var();_tmp11F.f1=_tmp400;});({void*_tmp401=Cyc_Port_var();_tmp11F.f2=_tmp401;});({void*_tmp402=Cyc_Port_var();_tmp11F.f3=_tmp402;});({void*_tmp403=Cyc_Port_var();_tmp11F.f4=_tmp403;});({void*_tmp404=Cyc_Port_var();_tmp11F.f5=_tmp404;});_tmp11F;});struct _tuple12 _tmp116=_tmp115;void*_tmp11E;void*_tmp11D;void*_tmp11C;void*_tmp11B;void*_tmp11A;_LL10A: _tmp11E=_tmp116.f1;_tmp11D=_tmp116.f2;_tmp11C=_tmp116.f3;_tmp11B=_tmp116.f4;_tmp11A=_tmp116.f5;_LL10B:;{
struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp117=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp118=_cycalloc(sizeof(*_tmp118));({struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmp405=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmp119;_tmp119.tag=11;_tmp119.f1=_tmp11E;_tmp119.f2=_tmp11D;_tmp119.f3=_tmp11C;_tmp119.f4=_tmp11B;_tmp119.f5=_tmp11A;_tmp119;});_tmp118[0]=_tmp405;});_tmp118;});
Cyc_Port_leq(_tmp129,_tmp11E);Cyc_Port_leq(_tmp124,_tmp11E);
Cyc_Port_leq(_tmp128,_tmp11D);Cyc_Port_leq(_tmp123,_tmp11D);
Cyc_Port_leq(_tmp127,_tmp11C);Cyc_Port_leq(_tmp123,_tmp11C);
Cyc_Port_leq(_tmp126,_tmp11B);Cyc_Port_leq(_tmp121,_tmp11B);
Cyc_Port_leq(_tmp125,_tmp11A);Cyc_Port_leq(_tmp120,_tmp11A);
_tmp111->hd=(void*)((void*)_tmp117);
return*lowers;};}default: goto _LL107;}case 12: if(((struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmp114.f2)->tag == 6){_LL103: _LL104:
# 677
 return*lowers;}else{goto _LL107;}default: _LL107: _LL108:
# 691
 goto _LLFA;}}_LLFA:;};}}
# 694
return({struct Cyc_List_List*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B->hd=t;_tmp12B->tl=*lowers;_tmp12B;});}struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
# 701
static int Cyc_Port_leq(void*t1,void*t2){
# 707
if(t1 == t2)return 1;
({void*_tmp406=Cyc_Port_compress_ct(t1);t1=_tmp406;});
({void*_tmp407=Cyc_Port_compress_ct(t2);t2=_tmp407;});{
struct _tuple10 _tmp12C=({struct _tuple10 _tmp14C;_tmp14C.f1=t1;_tmp14C.f2=t2;_tmp14C;});struct _tuple10 _tmp12D=_tmp12C;struct Cyc_Port_Cvar*_tmp14B;void*_tmp14A;void*_tmp149;void*_tmp148;void*_tmp147;void*_tmp146;void*_tmp145;void*_tmp144;void*_tmp143;void*_tmp142;void*_tmp141;void*_tmp140;void*_tmp13F;void*_tmp13E;void*_tmp13D;void*_tmp13C;void*_tmp13B;void*_tmp13A;void*_tmp139;void*_tmp138;void*_tmp137;void*_tmp136;void*_tmp135;void*_tmp134;struct Cyc_Port_Cvar*_tmp133;struct Cyc_Port_Cvar*_tmp132;struct Cyc_Port_Cvar*_tmp131;struct _dyneither_ptr _tmp130;struct _dyneither_ptr _tmp12F;struct _dyneither_ptr _tmp12E;switch(*((int*)_tmp12D.f1)){case 7: _LL10D: _LL10E:
 return 1;case 10: switch(*((int*)_tmp12D.f2)){case 10: _LL10F: _tmp12F=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp12D.f1)->f1;_tmp12E=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp12D.f2)->f1;_LL110:
 return({Cyc_strcmp((struct _dyneither_ptr)_tmp12F,(struct _dyneither_ptr)_tmp12E)== 0;});case 7: _LL111: _tmp130=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp12D.f1)->f1;_LL112:
 return 0;case 16: goto _LL139;default: goto _LL13B;}case 1: switch(*((int*)_tmp12D.f2)){case 0: _LL113: _LL114:
 return 1;case 16: goto _LL139;default: goto _LL13B;}case 0: switch(*((int*)_tmp12D.f2)){case 1: _LL115: _LL116:
 return 0;case 16: goto _LL139;default: goto _LL13B;}case 9: switch(*((int*)_tmp12D.f2)){case 8: _LL117: _LL118:
 return 0;case 16: goto _LL139;default: goto _LL13B;}case 8: switch(*((int*)_tmp12D.f2)){case 9: _LL119: _LL11A:
 return 1;case 16: goto _LL139;default: goto _LL13B;}case 16: switch(*((int*)_tmp12D.f2)){case 0: _LL11B: _LL11C:
 return 1;case 4: _LL11D: _LL11E:
 return 1;case 16: _LL135: _tmp132=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp12D.f1)->f1;_tmp131=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp12D.f2)->f1;_LL136:
# 737
({struct Cyc_List_List*_tmp408=Cyc_Port_filter_self(t1,_tmp132->uppers);_tmp132->uppers=_tmp408;});
({struct Cyc_List_List*_tmp409=Cyc_Port_filter_self(t2,_tmp131->lowers);_tmp131->lowers=_tmp409;});
({struct Cyc_List_List*_tmp40A=Cyc_Port_insert_upper(t1,t2,& _tmp132->uppers);_tmp132->uppers=_tmp40A;});
({struct Cyc_List_List*_tmp40B=Cyc_Port_insert_lower(t2,t1,& _tmp131->lowers);_tmp131->lowers=_tmp40B;});
return 1;default: _LL137: _tmp133=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp12D.f1)->f1;_LL138:
# 743
({struct Cyc_List_List*_tmp40C=Cyc_Port_filter_self(t1,_tmp133->uppers);_tmp133->uppers=_tmp40C;});
({struct Cyc_List_List*_tmp40D=Cyc_Port_insert_upper(t1,t2,& _tmp133->uppers);_tmp133->uppers=_tmp40D;});
return 1;}case 4: _LL11F: _LL120:
# 720
 return 0;case 5: switch(*((int*)_tmp12D.f2)){case 6: _LL121: _LL122:
 return 1;case 11: _LL123: _LL124:
 return 1;case 4: _LL125: _LL126:
 return 1;case 16: goto _LL139;default: goto _LL13B;}case 11: switch(*((int*)_tmp12D.f2)){case 6: _LL127: _LL128:
 return 1;case 4: _LL129: _LL12A:
 return 1;case 11: _LL12F: _tmp13D=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f1)->f1;_tmp13C=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f1)->f2;_tmp13B=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f1)->f3;_tmp13A=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f1)->f4;_tmp139=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f1)->f5;_tmp138=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f1;_tmp137=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f2;_tmp136=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f3;_tmp135=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f4;_tmp134=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f5;_LL130:
# 729
 return(((Cyc_Port_leq(_tmp13D,_tmp138) && Cyc_Port_leq(_tmp13C,_tmp137)) && Cyc_Port_unifies(_tmp13B,_tmp136)) && Cyc_Port_leq(_tmp13A,_tmp135)) && 
Cyc_Port_leq(_tmp139,_tmp134);case 16: goto _LL139;default: goto _LL13B;}case 12: switch(*((int*)_tmp12D.f2)){case 6: _LL12B: _LL12C:
# 726
 return 1;case 4: _LL12D: _LL12E:
 return 1;case 12: _LL131: _tmp143=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f1)->f1;_tmp142=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f1)->f2;_tmp141=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f1)->f3;_tmp140=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f2)->f1;_tmp13F=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f2)->f2;_tmp13E=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f2)->f3;_LL132:
# 732
 return(Cyc_Port_leq(_tmp143,_tmp140) && Cyc_Port_leq(_tmp142,_tmp13F)) && Cyc_Port_leq(_tmp141,_tmp13E);case 11: _LL133: _tmp14A=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f1)->f1;_tmp149=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f1)->f2;_tmp148=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f1)->f3;_tmp147=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f1;_tmp146=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f2;_tmp145=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f3;_tmp144=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f5;_LL134:
# 734
 return((Cyc_Port_leq(_tmp14A,_tmp147) && Cyc_Port_leq(_tmp149,_tmp146)) && Cyc_Port_unifies((void*)& Cyc_Port_Fat_ct_val,_tmp145)) && 
Cyc_Port_leq(_tmp148,_tmp144);case 16: goto _LL139;default: goto _LL13B;}default: if(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp12D.f2)->tag == 16){_LL139: _tmp14B=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp12D.f2)->f1;_LL13A:
# 747
({struct Cyc_List_List*_tmp40E=Cyc_Port_filter_self(t2,_tmp14B->lowers);_tmp14B->lowers=_tmp40E;});
({struct Cyc_List_List*_tmp40F=Cyc_Port_insert_lower(t2,t1,& _tmp14B->lowers);_tmp14B->lowers=_tmp40F;});
return 1;}else{_LL13B: _LL13C:
 return Cyc_Port_unifies(t1,t2);}}_LL10C:;};}struct Cyc_Port_GlobalCenv{struct Cyc_Dict_Dict typedef_dict;struct Cyc_Dict_Dict struct_dict;struct Cyc_Dict_Dict union_dict;void*return_type;struct Cyc_List_List*qualifier_edits;struct Cyc_List_List*pointer_edits;struct Cyc_List_List*zeroterm_edits;struct Cyc_List_List*vardecl_locs;struct Cyc_Hashtable_Table*varusage_tab;struct Cyc_List_List*edits;int porting;};
# 756
typedef struct Cyc_Port_GlobalCenv*Cyc_Port_gcenv_t;
# 781
enum Cyc_Port_CPos{Cyc_Port_FnRes_pos  = 0,Cyc_Port_FnArg_pos  = 1,Cyc_Port_FnBody_pos  = 2,Cyc_Port_Toplevel_pos  = 3};
typedef enum Cyc_Port_CPos Cyc_Port_cpos_t;struct Cyc_Port_GlobalCenv;struct Cyc_Port_Cenv{struct Cyc_Port_GlobalCenv*gcenv;struct Cyc_Dict_Dict var_dict;enum Cyc_Port_CPos cpos;};
# 784
typedef struct Cyc_Port_Cenv*Cyc_Port_cenv_t;struct Cyc_Port_Cenv;struct Cyc_Port_GlobalCenv;struct Cyc_Port_Cenv;struct Cyc_Port_GlobalCenv;
# 794
static struct Cyc_Port_Cenv*Cyc_Port_empty_cenv(){
struct Cyc_Port_GlobalCenv*g=({struct Cyc_Port_GlobalCenv*_tmp14E=_cycalloc(sizeof(*_tmp14E));({struct Cyc_Dict_Dict _tmp410=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp14E->typedef_dict=_tmp410;});({struct Cyc_Dict_Dict _tmp411=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp14E->struct_dict=_tmp411;});({struct Cyc_Dict_Dict _tmp412=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp14E->union_dict=_tmp412;});_tmp14E->qualifier_edits=0;_tmp14E->pointer_edits=0;_tmp14E->zeroterm_edits=0;_tmp14E->vardecl_locs=0;({struct Cyc_Hashtable_Table*_tmp413=
# 802
((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(unsigned int,unsigned int),int(*hash)(unsigned int)))Cyc_Hashtable_create)(128,Cyc_Port_cmp_seg_t,Cyc_Port_hash_seg_t);_tmp14E->varusage_tab=_tmp413;});_tmp14E->edits=0;_tmp14E->porting=0;({void*_tmp414=
# 805
Cyc_Port_void_ct();_tmp14E->return_type=_tmp414;});_tmp14E;});
return({struct Cyc_Port_Cenv*_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D->gcenv=g;_tmp14D->cpos=Cyc_Port_Toplevel_pos;({struct Cyc_Dict_Dict _tmp415=
# 808
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp14D->var_dict=_tmp415;});_tmp14D;});}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 814
static int Cyc_Port_in_fn_arg(struct Cyc_Port_Cenv*env){
return env->cpos == Cyc_Port_FnArg_pos;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 817
static int Cyc_Port_in_fn_res(struct Cyc_Port_Cenv*env){
return env->cpos == Cyc_Port_FnRes_pos;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 820
static int Cyc_Port_in_toplevel(struct Cyc_Port_Cenv*env){
return env->cpos == Cyc_Port_Toplevel_pos;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 823
static void*Cyc_Port_lookup_return_type(struct Cyc_Port_Cenv*env){
return(env->gcenv)->return_type;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 826
static void*Cyc_Port_lookup_typedef(struct Cyc_Port_Cenv*env,struct _tuple0*n){
struct _handler_cons _tmp14F;_push_handler(& _tmp14F);{int _tmp151=0;if(setjmp(_tmp14F.handler))_tmp151=1;if(!_tmp151){
{struct _tuple10 _tmp152=*((struct _tuple10*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((env->gcenv)->typedef_dict,n);struct _tuple10 _tmp153=_tmp152;void*_tmp155;_LL13E: _tmp155=_tmp153.f1;_LL13F:;{
void*_tmp154=_tmp155;_npop_handler(0);return _tmp154;};}
# 828
;_pop_handler();}else{void*_tmp150=(void*)_exn_thrown;void*_tmp156=_tmp150;void*_tmp157;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp156)->tag == Cyc_Dict_Absent){_LL141: _LL142:
# 835
 return Cyc_Absyn_sint_typ;}else{_LL143: _tmp157=_tmp156;_LL144:(int)_rethrow(_tmp157);}_LL140:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 840
static void*Cyc_Port_lookup_typedef_ctype(struct Cyc_Port_Cenv*env,struct _tuple0*n){
struct _handler_cons _tmp158;_push_handler(& _tmp158);{int _tmp15A=0;if(setjmp(_tmp158.handler))_tmp15A=1;if(!_tmp15A){
{struct _tuple10 _tmp15B=*((struct _tuple10*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((env->gcenv)->typedef_dict,n);struct _tuple10 _tmp15C=_tmp15B;void*_tmp15E;_LL146: _tmp15E=_tmp15C.f2;_LL147:;{
void*_tmp15D=_tmp15E;_npop_handler(0);return _tmp15D;};}
# 842
;_pop_handler();}else{void*_tmp159=(void*)_exn_thrown;void*_tmp15F=_tmp159;void*_tmp160;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp15F)->tag == Cyc_Dict_Absent){_LL149: _LL14A:
# 849
 return Cyc_Port_var();}else{_LL14B: _tmp160=_tmp15F;_LL14C:(int)_rethrow(_tmp160);}_LL148:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 855
static struct _tuple11*Cyc_Port_lookup_struct_decl(struct Cyc_Port_Cenv*env,struct _tuple0*n){
# 857
struct _tuple11**_tmp161=((struct _tuple11**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((env->gcenv)->struct_dict,n);
if((unsigned int)_tmp161)
return*_tmp161;else{
# 861
struct Cyc_Absyn_Aggrdecl*_tmp162=({struct Cyc_Absyn_Aggrdecl*_tmp164=_cycalloc(sizeof(*_tmp164));_tmp164->kind=Cyc_Absyn_StructA;_tmp164->sc=Cyc_Absyn_Public;_tmp164->name=n;_tmp164->tvs=0;_tmp164->impl=0;_tmp164->attributes=0;_tmp164->expected_mem_kind=0;_tmp164;});
# 864
struct _tuple11*p=({struct _tuple11*_tmp163=_cycalloc(sizeof(*_tmp163));_tmp163->f1=_tmp162;_tmp163->f2=0;_tmp163;});
({struct Cyc_Dict_Dict _tmp416=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple11*v))Cyc_Dict_insert)((env->gcenv)->struct_dict,n,p);(env->gcenv)->struct_dict=_tmp416;});
return p;}}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 872
static struct _tuple11*Cyc_Port_lookup_union_decl(struct Cyc_Port_Cenv*env,struct _tuple0*n){
# 874
struct _tuple11**_tmp165=((struct _tuple11**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((env->gcenv)->union_dict,n);
if((unsigned int)_tmp165)
return*_tmp165;else{
# 878
struct Cyc_Absyn_Aggrdecl*_tmp166=({struct Cyc_Absyn_Aggrdecl*_tmp168=_cycalloc(sizeof(*_tmp168));_tmp168->kind=Cyc_Absyn_UnionA;_tmp168->sc=Cyc_Absyn_Public;_tmp168->name=n;_tmp168->tvs=0;_tmp168->impl=0;_tmp168->attributes=0;_tmp168->expected_mem_kind=0;_tmp168;});
# 881
struct _tuple11*p=({struct _tuple11*_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167->f1=_tmp166;_tmp167->f2=0;_tmp167;});
({struct Cyc_Dict_Dict _tmp417=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple11*v))Cyc_Dict_insert)((env->gcenv)->union_dict,n,p);(env->gcenv)->union_dict=_tmp417;});
return p;}}struct _tuple13{struct _tuple10 f1;unsigned int f2;};struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct _tuple14{void*f1;struct _tuple10*f2;unsigned int f3;};
# 888
static struct _tuple13 Cyc_Port_lookup_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _tmp169;_push_handler(& _tmp169);{int _tmp16B=0;if(setjmp(_tmp169.handler))_tmp16B=1;if(!_tmp16B){
{struct _tuple14 _tmp16C=*((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);struct _tuple14 _tmp16D=_tmp16C;struct _tuple10*_tmp171;unsigned int _tmp170;_LL14E: _tmp171=_tmp16D.f2;_tmp170=_tmp16D.f3;_LL14F:;{
struct _tuple13 _tmp16F=({struct _tuple13 _tmp16E;_tmp16E.f1=*_tmp171;_tmp16E.f2=_tmp170;_tmp16E;});_npop_handler(0);return _tmp16F;};}
# 890
;_pop_handler();}else{void*_tmp16A=(void*)_exn_thrown;void*_tmp172=_tmp16A;void*_tmp175;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp172)->tag == Cyc_Dict_Absent){_LL151: _LL152:
# 897
 return({struct _tuple13 _tmp173;({struct _tuple10 _tmp41A=({struct _tuple10 _tmp174;({void*_tmp418=Cyc_Port_var();_tmp174.f1=_tmp418;});({void*_tmp419=Cyc_Port_var();_tmp174.f2=_tmp419;});_tmp174;});_tmp173.f1=_tmp41A;});_tmp173.f2=0;_tmp173;});}else{_LL153: _tmp175=_tmp172;_LL154:(int)_rethrow(_tmp175);}_LL150:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 900
static struct _tuple14*Cyc_Port_lookup_full_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
return((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 904
static int Cyc_Port_declared_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
return((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_member)(env->var_dict,x);}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 908
static int Cyc_Port_isfn(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _tmp176;_push_handler(& _tmp176);{int _tmp178=0;if(setjmp(_tmp176.handler))_tmp178=1;if(!_tmp178){
{struct _tuple14 _tmp179=*((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);struct _tuple14 _tmp17A=_tmp179;void*_tmp17F;_LL156: _tmp17F=_tmp17A.f1;_LL157:;
LOOP: {void*_tmp17B=_tmp17F;struct _tuple0*_tmp17E;switch(*((int*)_tmp17B)){case 17: _LL159: _tmp17E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp17B)->f1;_LL15A:
({void*_tmp41B=Cyc_Port_lookup_typedef(env,_tmp17E);_tmp17F=_tmp41B;});goto LOOP;case 9: _LL15B: _LL15C: {
int _tmp17C=1;_npop_handler(0);return _tmp17C;}default: _LL15D: _LL15E: {
int _tmp17D=0;_npop_handler(0);return _tmp17D;}}_LL158:;}}
# 910
;_pop_handler();}else{void*_tmp177=(void*)_exn_thrown;void*_tmp180=_tmp177;void*_tmp181;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp180)->tag == Cyc_Dict_Absent){_LL160: _LL161:
# 921
 return 0;}else{_LL162: _tmp181=_tmp180;_LL163:(int)_rethrow(_tmp181);}_LL15F:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 925
static int Cyc_Port_isarray(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _tmp182;_push_handler(& _tmp182);{int _tmp184=0;if(setjmp(_tmp182.handler))_tmp184=1;if(!_tmp184){
{struct _tuple14 _tmp185=*((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);struct _tuple14 _tmp186=_tmp185;void*_tmp18B;_LL165: _tmp18B=_tmp186.f1;_LL166:;
LOOP: {void*_tmp187=_tmp18B;struct _tuple0*_tmp18A;switch(*((int*)_tmp187)){case 17: _LL168: _tmp18A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp187)->f1;_LL169:
({void*_tmp41C=Cyc_Port_lookup_typedef(env,_tmp18A);_tmp18B=_tmp41C;});goto LOOP;case 8: _LL16A: _LL16B: {
int _tmp188=1;_npop_handler(0);return _tmp188;}default: _LL16C: _LL16D: {
int _tmp189=0;_npop_handler(0);return _tmp189;}}_LL167:;}}
# 927
;_pop_handler();}else{void*_tmp183=(void*)_exn_thrown;void*_tmp18C=_tmp183;void*_tmp18D;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp18C)->tag == Cyc_Dict_Absent){_LL16F: _LL170:
# 938
 return 0;}else{_LL171: _tmp18D=_tmp18C;_LL172:(int)_rethrow(_tmp18D);}_LL16E:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 944
static struct Cyc_Port_Cenv*Cyc_Port_set_cpos(struct Cyc_Port_Cenv*env,enum Cyc_Port_CPos cpos){
return({struct Cyc_Port_Cenv*_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E->gcenv=env->gcenv;_tmp18E->var_dict=env->var_dict;_tmp18E->cpos=cpos;_tmp18E;});}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 949
static void Cyc_Port_add_return_type(struct Cyc_Port_Cenv*env,void*t){
(env->gcenv)->return_type=t;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 954
static struct Cyc_Port_Cenv*Cyc_Port_add_var(struct Cyc_Port_Cenv*env,struct _tuple0*x,void*t,void*qual,void*ctype,unsigned int loc){
# 956
return({struct Cyc_Port_Cenv*_tmp18F=_cycalloc(sizeof(*_tmp18F));_tmp18F->gcenv=env->gcenv;({struct Cyc_Dict_Dict _tmp420=({
struct Cyc_Dict_Dict _tmp41F=env->var_dict;struct _tuple0*_tmp41E=x;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(_tmp41F,_tmp41E,({struct _tuple14*_tmp190=_cycalloc(sizeof(*_tmp190));_tmp190->f1=t;({struct _tuple10*_tmp41D=({struct _tuple10*_tmp191=_cycalloc(sizeof(*_tmp191));_tmp191->f1=qual;_tmp191->f2=ctype;_tmp191;});_tmp190->f2=_tmp41D;});_tmp190->f3=loc;_tmp190;}));});_tmp18F->var_dict=_tmp420;});_tmp18F->cpos=env->cpos;_tmp18F;});}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 962
static void Cyc_Port_add_typedef(struct Cyc_Port_Cenv*env,struct _tuple0*n,void*t,void*ct){
({struct Cyc_Dict_Dict _tmp423=({struct Cyc_Dict_Dict _tmp422=(env->gcenv)->typedef_dict;struct _tuple0*_tmp421=n;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple10*v))Cyc_Dict_insert)(_tmp422,_tmp421,({struct _tuple10*_tmp192=_cycalloc(sizeof(*_tmp192));_tmp192->f1=t;_tmp192->f2=ct;_tmp192;}));});(env->gcenv)->typedef_dict=_tmp423;});}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct _tuple15{struct _tuple0*f1;unsigned int f2;void*f3;};struct Cyc_Port_VarUsage;struct Cyc_Port_VarUsage;
# 966
static void Cyc_Port_register_localvar_decl(struct Cyc_Port_Cenv*env,struct _tuple0*x,unsigned int loc,void*type,struct Cyc_Absyn_Exp*init){
({struct Cyc_List_List*_tmp425=({struct Cyc_List_List*_tmp193=_cycalloc(sizeof(*_tmp193));({struct _tuple15*_tmp424=({struct _tuple15*_tmp194=_cycalloc(sizeof(*_tmp194));_tmp194->f1=x;_tmp194->f2=loc;_tmp194->f3=type;_tmp194;});_tmp193->hd=_tmp424;});_tmp193->tl=(env->gcenv)->vardecl_locs;_tmp193;});(env->gcenv)->vardecl_locs=_tmp425;});
({struct Cyc_Hashtable_Table*_tmp427=(env->gcenv)->varusage_tab;unsigned int _tmp426=loc;((void(*)(struct Cyc_Hashtable_Table*t,unsigned int key,struct Cyc_Port_VarUsage*val))Cyc_Hashtable_insert)(_tmp427,_tmp426,({struct Cyc_Port_VarUsage*_tmp195=_cycalloc(sizeof(*_tmp195));_tmp195->address_taken=0;_tmp195->init=init;_tmp195->usage_locs=0;_tmp195;}));});}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_VarUsage;struct Cyc_Port_VarUsage;struct Cyc_Port_VarUsage;struct Cyc_Port_VarUsage;struct Cyc_Port_VarUsage;
# 970
static void Cyc_Port_register_localvar_usage(struct Cyc_Port_Cenv*env,unsigned int declloc,unsigned int useloc,int takeaddress){
struct Cyc_Port_VarUsage*varusage=0;
if(((int(*)(struct Cyc_Hashtable_Table*t,unsigned int key,struct Cyc_Port_VarUsage**data))Cyc_Hashtable_try_lookup)((env->gcenv)->varusage_tab,declloc,& varusage)){
if(takeaddress)((struct Cyc_Port_VarUsage*)_check_null(varusage))->address_taken=1;{
struct Cyc_List_List*_tmp196=((struct Cyc_Port_VarUsage*)_check_null(varusage))->usage_locs;
({struct Cyc_List_List*_tmp428=({struct Cyc_List_List*_tmp197=_cycalloc(sizeof(*_tmp197));_tmp197->hd=(void*)useloc;_tmp197->tl=_tmp196;_tmp197;});((struct Cyc_Port_VarUsage*)_check_null(varusage))->usage_locs=_tmp428;});};}}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct _tuple16{void*f1;void*f2;unsigned int f3;};
# 981
static void Cyc_Port_register_const_cvar(struct Cyc_Port_Cenv*env,void*new_qual,void*orig_qual,unsigned int loc){
# 983
({struct Cyc_List_List*_tmp42A=({struct Cyc_List_List*_tmp198=_cycalloc(sizeof(*_tmp198));({struct _tuple16*_tmp429=({struct _tuple16*_tmp199=_cycalloc(sizeof(*_tmp199));_tmp199->f1=new_qual;_tmp199->f2=orig_qual;_tmp199->f3=loc;_tmp199;});_tmp198->hd=_tmp429;});_tmp198->tl=(env->gcenv)->qualifier_edits;_tmp198;});(env->gcenv)->qualifier_edits=_tmp42A;});}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 989
static void Cyc_Port_register_ptr_cvars(struct Cyc_Port_Cenv*env,void*new_ptr,void*orig_ptr,unsigned int loc){
# 991
({struct Cyc_List_List*_tmp42C=({struct Cyc_List_List*_tmp19A=_cycalloc(sizeof(*_tmp19A));({struct _tuple16*_tmp42B=({struct _tuple16*_tmp19B=_cycalloc(sizeof(*_tmp19B));_tmp19B->f1=new_ptr;_tmp19B->f2=orig_ptr;_tmp19B->f3=loc;_tmp19B;});_tmp19A->hd=_tmp42B;});_tmp19A->tl=(env->gcenv)->pointer_edits;_tmp19A;});(env->gcenv)->pointer_edits=_tmp42C;});}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 995
static void Cyc_Port_register_zeroterm_cvars(struct Cyc_Port_Cenv*env,void*new_zt,void*orig_zt,unsigned int loc){
# 997
({struct Cyc_List_List*_tmp42E=({struct Cyc_List_List*_tmp19C=_cycalloc(sizeof(*_tmp19C));({struct _tuple16*_tmp42D=({struct _tuple16*_tmp19D=_cycalloc(sizeof(*_tmp19D));_tmp19D->f1=new_zt;_tmp19D->f2=orig_zt;_tmp19D->f3=loc;_tmp19D;});_tmp19C->hd=_tmp42D;});_tmp19C->tl=(env->gcenv)->zeroterm_edits;_tmp19C;});(env->gcenv)->zeroterm_edits=_tmp42E;});}
# 1003
static void*Cyc_Port_main_type(){
struct _tuple8*arg1=({struct _tuple8*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3->f1=0;({struct Cyc_Absyn_Tqual _tmp42F=
Cyc_Absyn_empty_tqual(0);_tmp1A3->f2=_tmp42F;});_tmp1A3->f3=Cyc_Absyn_sint_typ;_tmp1A3;});
struct _tuple8*arg2=({struct _tuple8*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));_tmp1A2->f1=0;({struct Cyc_Absyn_Tqual _tmp430=
Cyc_Absyn_empty_tqual(0);_tmp1A2->f2=_tmp430;});({void*_tmp434=({
void*_tmp433=Cyc_Absyn_string_typ(Cyc_Absyn_wildtyp(0));void*_tmp432=Cyc_Absyn_wildtyp(0);struct Cyc_Absyn_Tqual _tmp431=
Cyc_Absyn_empty_tqual(0);
# 1008
Cyc_Absyn_dyneither_typ(_tmp433,_tmp432,_tmp431,
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());});_tmp1A2->f3=_tmp434;});_tmp1A2;});
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp19E=_cycalloc(sizeof(*_tmp19E));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp438=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp19F;_tmp19F.tag=9;({struct Cyc_Absyn_FnInfo _tmp437=({struct Cyc_Absyn_FnInfo _tmp1A0;_tmp1A0.tvars=0;_tmp1A0.effect=0;({struct Cyc_Absyn_Tqual _tmp435=
Cyc_Absyn_empty_tqual(0);_tmp1A0.ret_tqual=_tmp435;});_tmp1A0.ret_typ=Cyc_Absyn_sint_typ;({struct Cyc_List_List*_tmp436=({struct _tuple8*_tmp1A1[2];_tmp1A1[1]=arg2;_tmp1A1[0]=arg1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1A1,sizeof(struct _tuple8*),2));});_tmp1A0.args=_tmp436;});_tmp1A0.c_varargs=0;_tmp1A0.cyc_varargs=0;_tmp1A0.rgn_po=0;_tmp1A0.attributes=0;_tmp1A0.requires_clause=0;_tmp1A0.requires_relns=0;_tmp1A0.ensures_clause=0;_tmp1A0.ensures_relns=0;_tmp1A0;});_tmp19F.f1=_tmp437;});_tmp19F;});_tmp19E[0]=_tmp438;});_tmp19E;});}struct Cyc_Port_Cenv;
# 1022
static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t);struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1025
static struct Cyc_Port_Cenv*Cyc_Port_initial_cenv(){
struct Cyc_Port_Cenv*_tmp1A4=Cyc_Port_empty_cenv();
# 1028
({struct Cyc_Port_Cenv*_tmp43F=({struct Cyc_Port_Cenv*_tmp43E=_tmp1A4;struct _tuple0*_tmp43D=({struct _tuple0*_tmp1A5=_cycalloc(sizeof(*_tmp1A5));_tmp1A5->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmp439=({_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"main",sizeof(char),5);});_tmp1A5->f2=_tmp439;});_tmp1A5;});void*_tmp43C=Cyc_Port_main_type();void*_tmp43B=Cyc_Port_const_ct();Cyc_Port_add_var(_tmp43E,_tmp43D,_tmp43C,_tmp43B,({
struct Cyc_Port_Cenv*_tmp43A=_tmp1A4;Cyc_Port_type_to_ctype(_tmp43A,Cyc_Port_main_type());}),0);});
# 1028
_tmp1A4=_tmp43F;});
# 1030
return _tmp1A4;}
# 1036
static void Cyc_Port_region_counts(struct Cyc_Dict_Dict*cts,void*t){
void*_tmp1A7=Cyc_Port_compress_ct(t);void*_tmp1A8=_tmp1A7;void*_tmp1B5;struct Cyc_List_List*_tmp1B4;void*_tmp1B3;void*_tmp1B2;void*_tmp1B1;void*_tmp1B0;void*_tmp1AF;void*_tmp1AE;void*_tmp1AD;void*_tmp1AC;struct _dyneither_ptr*_tmp1AB;switch(*((int*)_tmp1A8)){case 0: _LL174: _LL175:
 goto _LL177;case 1: _LL176: _LL177:
 goto _LL179;case 2: _LL178: _LL179:
 goto _LL17B;case 3: _LL17A: _LL17B:
 goto _LL17D;case 4: _LL17C: _LL17D:
 goto _LL17F;case 5: _LL17E: _LL17F:
 goto _LL181;case 6: _LL180: _LL181:
 goto _LL183;case 14: _LL182: _LL183:
 goto _LL185;case 13: _LL184: _LL185:
 goto _LL187;case 16: _LL186: _LL187:
 goto _LL189;case 8: _LL188: _LL189:
 goto _LL18B;case 9: _LL18A: _LL18B:
 goto _LL18D;case 7: _LL18C: _LL18D:
 return;case 10: _LL18E: _tmp1AB=((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp1A8)->f1;_LL18F:
# 1052
 if(!((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*cts,_tmp1AB))
({struct Cyc_Dict_Dict _tmp442=({struct Cyc_Dict_Dict _tmp441=*cts;struct _dyneither_ptr*_tmp440=_tmp1AB;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,int*v))Cyc_Dict_insert)(_tmp441,_tmp440,({int*_tmp1A9=_cycalloc_atomic(sizeof(*_tmp1A9));_tmp1A9[0]=0;_tmp1A9;}));});*cts=_tmp442;});{
int*_tmp1AA=((int*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*cts,_tmp1AB);
*_tmp1AA=*_tmp1AA + 1;
return;};case 11: _LL190: _tmp1B0=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1A8)->f1;_tmp1AF=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1A8)->f2;_tmp1AE=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1A8)->f3;_tmp1AD=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1A8)->f4;_tmp1AC=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1A8)->f5;_LL191:
# 1058
 Cyc_Port_region_counts(cts,_tmp1B0);
Cyc_Port_region_counts(cts,_tmp1AD);
return;case 12: _LL192: _tmp1B3=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp1A8)->f1;_tmp1B2=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp1A8)->f2;_tmp1B1=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp1A8)->f3;_LL193:
# 1062
 Cyc_Port_region_counts(cts,_tmp1B3);
return;default: _LL194: _tmp1B5=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp1A8)->f1;_tmp1B4=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp1A8)->f2;_LL195:
# 1065
 Cyc_Port_region_counts(cts,_tmp1B5);
for(0;(unsigned int)_tmp1B4;_tmp1B4=_tmp1B4->tl){Cyc_Port_region_counts(cts,(void*)_tmp1B4->hd);}
return;}_LL173:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Edit;
# 1076
static void Cyc_Port_register_rgns(struct Cyc_Port_Cenv*env,struct Cyc_Dict_Dict counts,int fn_res,void*t,void*c){
# 1078
({void*_tmp443=Cyc_Port_compress_ct(c);c=_tmp443;});{
struct _tuple10 _tmp1B6=({struct _tuple10 _tmp1CC;_tmp1CC.f1=t;_tmp1CC.f2=c;_tmp1CC;});struct _tuple10 _tmp1B7=_tmp1B6;void*_tmp1CB;struct Cyc_List_List*_tmp1CA;void*_tmp1C9;struct Cyc_List_List*_tmp1C8;void*_tmp1C7;void*_tmp1C6;void*_tmp1C5;void*_tmp1C4;struct Cyc_Absyn_PtrLoc*_tmp1C3;void*_tmp1C2;void*_tmp1C1;switch(*((int*)_tmp1B7.f1)){case 5: if(((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1B7.f2)->tag == 11){_LL197: _tmp1C5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B7.f1)->f1).elt_typ;_tmp1C4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B7.f1)->f1).ptr_atts).rgn;_tmp1C3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B7.f1)->f1).ptr_atts).ptrloc;_tmp1C2=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1B7.f2)->f1;_tmp1C1=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1B7.f2)->f4;_LL198:
# 1081
 Cyc_Port_register_rgns(env,counts,fn_res,_tmp1C5,_tmp1C2);{
unsigned int _tmp1B8=(unsigned int)_tmp1C3?_tmp1C3->rgn_loc:(unsigned int)0;
({void*_tmp444=Cyc_Port_compress_ct(_tmp1C1);_tmp1C1=_tmp444;});
# 1098 "port.cyc"
({struct Cyc_List_List*_tmp448=({struct Cyc_List_List*_tmp1B9=_cycalloc(sizeof(*_tmp1B9));({struct Cyc_Port_Edit*_tmp447=({struct Cyc_Port_Edit*_tmp1BA=_cycalloc(sizeof(*_tmp1BA));_tmp1BA->loc=_tmp1B8;({struct _dyneither_ptr _tmp445=({const char*_tmp1BC="`H ";_tag_dyneither(_tmp1BC,sizeof(char),4);});_tmp1BA->old_string=_tmp445;});({struct _dyneither_ptr _tmp446=({const char*_tmp1BB="";_tag_dyneither(_tmp1BB,sizeof(char),1);});_tmp1BA->new_string=_tmp446;});_tmp1BA;});_tmp1B9->hd=_tmp447;});_tmp1B9->tl=(env->gcenv)->edits;_tmp1B9;});(env->gcenv)->edits=_tmp448;});
# 1101
goto _LL196;};}else{goto _LL19D;}case 8: if(((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp1B7.f2)->tag == 12){_LL199: _tmp1C7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B7.f1)->f1).elt_type;_tmp1C6=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp1B7.f2)->f1;_LL19A:
# 1103
 Cyc_Port_register_rgns(env,counts,fn_res,_tmp1C7,_tmp1C6);
goto _LL196;}else{goto _LL19D;}case 9: if(((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp1B7.f2)->tag == 15){_LL19B: _tmp1CB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B7.f1)->f1).ret_typ;_tmp1CA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B7.f1)->f1).args;_tmp1C9=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp1B7.f2)->f1;_tmp1C8=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp1B7.f2)->f2;_LL19C:
# 1106
 Cyc_Port_register_rgns(env,counts,1,_tmp1CB,_tmp1C9);
for(0;_tmp1CA != 0  && _tmp1C8 != 0;(_tmp1CA=_tmp1CA->tl,_tmp1C8=_tmp1C8->tl)){
struct _tuple8 _tmp1BD=*((struct _tuple8*)_tmp1CA->hd);struct _tuple8 _tmp1BE=_tmp1BD;void*_tmp1C0;_LL1A0: _tmp1C0=_tmp1BE.f3;_LL1A1:;{
void*_tmp1BF=(void*)_tmp1C8->hd;
Cyc_Port_register_rgns(env,counts,0,_tmp1C0,_tmp1BF);};}
# 1112
goto _LL196;}else{goto _LL19D;}default: _LL19D: _LL19E:
 goto _LL196;}_LL196:;};}struct Cyc_Port_Cenv;
# 1119
static void*Cyc_Port_gen_exp(int takeaddress,struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e);struct Cyc_Port_Cenv;
static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Stmt*s);struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d,int takeaddress);struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1124
static int Cyc_Port_is_char(struct Cyc_Port_Cenv*env,void*t){
void*_tmp1CD=t;enum Cyc_Absyn_Size_of _tmp1CF;struct _tuple0*_tmp1CE;switch(*((int*)_tmp1CD)){case 17: _LL1A3: _tmp1CE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1CD)->f1;_LL1A4:
# 1127
(*_tmp1CE).f1=Cyc_Absyn_Loc_n;
return({struct Cyc_Port_Cenv*_tmp449=env;Cyc_Port_is_char(_tmp449,Cyc_Port_lookup_typedef(env,_tmp1CE));});case 6: _LL1A5: _tmp1CF=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1CD)->f2;_LL1A6:
 return 1;default: _LL1A7: _LL1A8:
 return 0;}_LL1A2:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1135
static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t){
void*_tmp1D0=t;struct Cyc_List_List*_tmp207;union Cyc_Absyn_AggrInfoU _tmp206;void*_tmp205;struct Cyc_List_List*_tmp204;void*_tmp203;struct Cyc_Absyn_Tqual _tmp202;union Cyc_Absyn_Constraint*_tmp201;unsigned int _tmp200;void*_tmp1FF;struct Cyc_Absyn_Tqual _tmp1FE;void*_tmp1FD;union Cyc_Absyn_Constraint*_tmp1FC;union Cyc_Absyn_Constraint*_tmp1FB;union Cyc_Absyn_Constraint*_tmp1FA;struct Cyc_Absyn_PtrLoc*_tmp1F9;struct _tuple0*_tmp1F8;switch(*((int*)_tmp1D0)){case 17: _LL1AA: _tmp1F8=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1D0)->f1;_LL1AB:
# 1138
(*_tmp1F8).f1=Cyc_Absyn_Loc_n;
return Cyc_Port_lookup_typedef_ctype(env,_tmp1F8);case 0: _LL1AC: _LL1AD:
 return Cyc_Port_void_ct();case 5: _LL1AE: _tmp1FF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).elt_typ;_tmp1FE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).elt_tq;_tmp1FD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).ptr_atts).rgn;_tmp1FC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).ptr_atts).nullable;_tmp1FB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).ptr_atts).bounds;_tmp1FA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).ptr_atts).zero_term;_tmp1F9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).ptr_atts).ptrloc;_LL1AF: {
# 1142
unsigned int _tmp1D1=(unsigned int)_tmp1F9?_tmp1F9->ptr_loc:(unsigned int)0;
unsigned int _tmp1D2=(unsigned int)_tmp1F9?_tmp1F9->rgn_loc:(unsigned int)0;
unsigned int _tmp1D3=(unsigned int)_tmp1F9?_tmp1F9->zt_loc:(unsigned int)0;
# 1148
void*_tmp1D4=Cyc_Port_type_to_ctype(env,_tmp1FF);
void*new_rgn;
# 1151
{void*_tmp1D5=_tmp1FD;struct _dyneither_ptr*_tmp1D6;switch(*((int*)_tmp1D5)){case 20: _LL1C1: _LL1C2:
# 1153
({void*_tmp44A=Cyc_Port_heap_ct();new_rgn=_tmp44A;});goto _LL1C0;case 2: _LL1C3: _tmp1D6=(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1D5)->f1)->name;_LL1C4:
# 1155
({void*_tmp44B=Cyc_Port_rgnvar_ct(_tmp1D6);new_rgn=_tmp44B;});goto _LL1C0;case 1: _LL1C5: _LL1C6:
# 1159
 if(Cyc_Port_in_fn_arg(env))
({void*_tmp44C=Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var());new_rgn=_tmp44C;});else{
# 1162
if(Cyc_Port_in_fn_res(env) || Cyc_Port_in_toplevel(env))
({void*_tmp44D=Cyc_Port_heap_ct();new_rgn=_tmp44D;});else{
({void*_tmp44E=Cyc_Port_var();new_rgn=_tmp44E;});}}
goto _LL1C0;default: _LL1C7: _LL1C8:
# 1167
({void*_tmp44F=Cyc_Port_heap_ct();new_rgn=_tmp44F;});goto _LL1C0;}_LL1C0:;}{
# 1169
int _tmp1D7=({((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp1FB)== (void*)& Cyc_Absyn_DynEither_b_val;});
int orig_zt;
{union Cyc_Absyn_Constraint _tmp1D8=*_tmp1FA;union Cyc_Absyn_Constraint _tmp1D9=_tmp1D8;if((_tmp1D9.No_constr).tag == 3){_LL1CA: _LL1CB:
({int _tmp450=Cyc_Port_is_char(env,_tmp1FF);orig_zt=_tmp450;});goto _LL1C9;}else{_LL1CC: _LL1CD:
({int _tmp451=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp1FA);orig_zt=_tmp451;});goto _LL1C9;}_LL1C9:;}
# 1175
if((env->gcenv)->porting){
void*_tmp1DA=Cyc_Port_var();
# 1180
({struct Cyc_Port_Cenv*_tmp454=env;void*_tmp453=_tmp1DA;void*_tmp452=_tmp1FE.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_register_const_cvar(_tmp454,_tmp453,_tmp452,_tmp1FE.loc);});
# 1183
if(_tmp1FE.print_const)({void*_tmp455=_tmp1DA;Cyc_Port_unify_ct(_tmp455,Cyc_Port_const_ct());});{
# 1189
void*_tmp1DB=Cyc_Port_var();
# 1192
({struct Cyc_Port_Cenv*_tmp458=env;void*_tmp457=_tmp1DB;void*_tmp456=_tmp1D7?Cyc_Port_fat_ct(): Cyc_Port_thin_ct();Cyc_Port_register_ptr_cvars(_tmp458,_tmp457,_tmp456,_tmp1D1);});
# 1194
if(_tmp1D7)({void*_tmp459=_tmp1DB;Cyc_Port_unify_ct(_tmp459,Cyc_Port_fat_ct());});{
void*_tmp1DC=Cyc_Port_var();
# 1198
({struct Cyc_Port_Cenv*_tmp45C=env;void*_tmp45B=_tmp1DC;void*_tmp45A=orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct();Cyc_Port_register_zeroterm_cvars(_tmp45C,_tmp45B,_tmp45A,_tmp1D3);});
# 1200
{union Cyc_Absyn_Constraint _tmp1DD=*_tmp1FA;union Cyc_Absyn_Constraint _tmp1DE=_tmp1DD;if((_tmp1DE.No_constr).tag == 3){_LL1CF: _LL1D0:
# 1202
 goto _LL1CE;}else{_LL1D1: _LL1D2:
# 1204
({void*_tmp45D=_tmp1DC;Cyc_Port_unify_ct(_tmp45D,((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp1FA)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});
goto _LL1CE;}_LL1CE:;}
# 1210
return Cyc_Port_ptr_ct(_tmp1D4,_tmp1DA,_tmp1DB,new_rgn,_tmp1DC);};};}else{
# 1214
return({void*_tmp461=_tmp1D4;void*_tmp460=_tmp1FE.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();void*_tmp45F=
_tmp1D7?Cyc_Port_fat_ct(): Cyc_Port_thin_ct();
# 1214
void*_tmp45E=new_rgn;Cyc_Port_ptr_ct(_tmp461,_tmp460,_tmp45F,_tmp45E,
# 1216
orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});}};}case 6: _LL1B0: _LL1B1:
# 1218
 goto _LL1B3;case 7: _LL1B2: _LL1B3:
 return Cyc_Port_arith_ct();case 8: _LL1B4: _tmp203=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D0)->f1).elt_type;_tmp202=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D0)->f1).tq;_tmp201=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D0)->f1).zero_term;_tmp200=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D0)->f1).zt_loc;_LL1B5: {
# 1222
void*_tmp1DF=Cyc_Port_type_to_ctype(env,_tmp203);
int orig_zt;
{union Cyc_Absyn_Constraint _tmp1E0=*_tmp201;union Cyc_Absyn_Constraint _tmp1E1=_tmp1E0;if((_tmp1E1.No_constr).tag == 3){_LL1D4: _LL1D5:
 orig_zt=0;goto _LL1D3;}else{_LL1D6: _LL1D7:
({int _tmp462=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp201);orig_zt=_tmp462;});goto _LL1D3;}_LL1D3:;}
# 1228
if((env->gcenv)->porting){
void*_tmp1E2=Cyc_Port_var();
({struct Cyc_Port_Cenv*_tmp465=env;void*_tmp464=_tmp1E2;void*_tmp463=_tmp202.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_register_const_cvar(_tmp465,_tmp464,_tmp463,_tmp202.loc);});{
# 1237
void*_tmp1E3=Cyc_Port_var();
({struct Cyc_Port_Cenv*_tmp468=env;void*_tmp467=_tmp1E3;void*_tmp466=orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct();Cyc_Port_register_zeroterm_cvars(_tmp468,_tmp467,_tmp466,_tmp200);});
# 1240
{union Cyc_Absyn_Constraint _tmp1E4=*_tmp201;union Cyc_Absyn_Constraint _tmp1E5=_tmp1E4;if((_tmp1E5.No_constr).tag == 3){_LL1D9: _LL1DA:
# 1242
 goto _LL1D8;}else{_LL1DB: _LL1DC:
# 1244
({void*_tmp469=_tmp1E3;Cyc_Port_unify_ct(_tmp469,((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp201)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});
goto _LL1D8;}_LL1D8:;}
# 1247
return Cyc_Port_array_ct(_tmp1DF,_tmp1E2,_tmp1E3);};}else{
# 1249
return({void*_tmp46B=_tmp1DF;void*_tmp46A=_tmp202.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_array_ct(_tmp46B,_tmp46A,
orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});}}case 9: _LL1B6: _tmp205=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D0)->f1).ret_typ;_tmp204=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D0)->f1).args;_LL1B7: {
# 1253
void*_tmp1E6=({struct Cyc_Port_Cenv*_tmp46C=Cyc_Port_set_cpos(env,Cyc_Port_FnRes_pos);Cyc_Port_type_to_ctype(_tmp46C,_tmp205);});
struct Cyc_Port_Cenv*_tmp1E7=Cyc_Port_set_cpos(env,Cyc_Port_FnArg_pos);
struct Cyc_List_List*cargs=0;
for(0;_tmp204 != 0;_tmp204=_tmp204->tl){
struct _tuple8 _tmp1E8=*((struct _tuple8*)_tmp204->hd);struct _tuple8 _tmp1E9=_tmp1E8;struct Cyc_Absyn_Tqual _tmp1EC;void*_tmp1EB;_LL1DE: _tmp1EC=_tmp1E9.f2;_tmp1EB=_tmp1E9.f3;_LL1DF:;
({struct Cyc_List_List*_tmp46E=({struct Cyc_List_List*_tmp1EA=_cycalloc(sizeof(*_tmp1EA));({void*_tmp46D=Cyc_Port_type_to_ctype(_tmp1E7,_tmp1EB);_tmp1EA->hd=_tmp46D;});_tmp1EA->tl=cargs;_tmp1EA;});cargs=_tmp46E;});}
# 1260
return({void*_tmp46F=_tmp1E6;Cyc_Port_fn_ct(_tmp46F,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(cargs));});}case 11: _LL1B8: _tmp206=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1D0)->f1).aggr_info;_LL1B9: {
# 1262
union Cyc_Absyn_AggrInfoU _tmp1ED=_tmp206;struct Cyc_Absyn_Aggrdecl**_tmp1F7;struct _tuple0*_tmp1F6;struct _tuple0*_tmp1F5;if((_tmp1ED.UnknownAggr).tag == 1){if(((_tmp1ED.UnknownAggr).val).f1 == Cyc_Absyn_StructA){_LL1E1: _tmp1F5=((_tmp1ED.UnknownAggr).val).f2;_LL1E2:
# 1264
(*_tmp1F5).f1=Cyc_Absyn_Loc_n;{
struct _tuple11*_tmp1EE=Cyc_Port_lookup_struct_decl(env,_tmp1F5);
return Cyc_Port_known_aggr_ct(_tmp1EE);};}else{_LL1E3: _tmp1F6=((_tmp1ED.UnknownAggr).val).f2;_LL1E4:
# 1268
(*_tmp1F6).f1=Cyc_Absyn_Loc_n;{
struct _tuple11*_tmp1EF=Cyc_Port_lookup_union_decl(env,_tmp1F6);
return Cyc_Port_known_aggr_ct(_tmp1EF);};}}else{_LL1E5: _tmp1F7=(_tmp1ED.KnownAggr).val;_LL1E6: {
# 1272
struct Cyc_Absyn_Aggrdecl*_tmp1F0=*_tmp1F7;
enum Cyc_Absyn_AggrKind _tmp1F1=_tmp1F0->kind;enum Cyc_Absyn_AggrKind _tmp1F2=_tmp1F1;if(_tmp1F2 == Cyc_Absyn_StructA){_LL1E8: _LL1E9: {
# 1275
struct _tuple11*_tmp1F3=Cyc_Port_lookup_struct_decl(env,_tmp1F0->name);
return Cyc_Port_known_aggr_ct(_tmp1F3);}}else{_LL1EA: _LL1EB: {
# 1278
struct _tuple11*_tmp1F4=Cyc_Port_lookup_union_decl(env,_tmp1F0->name);
return Cyc_Port_known_aggr_ct(_tmp1F4);}}_LL1E7:;}}_LL1E0:;}case 13: _LL1BA: _LL1BB:
# 1282
 return Cyc_Port_arith_ct();case 14: _LL1BC: _tmp207=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1D0)->f1;_LL1BD:
# 1285
 for(0;(unsigned int)_tmp207;_tmp207=_tmp207->tl){
(*((struct Cyc_Absyn_Enumfield*)_tmp207->hd)->name).f1=Cyc_Absyn_Loc_n;
({struct Cyc_Port_Cenv*_tmp474=({struct Cyc_Port_Cenv*_tmp473=env;struct _tuple0*_tmp472=((struct Cyc_Absyn_Enumfield*)_tmp207->hd)->name;void*_tmp471=Cyc_Absyn_sint_typ;void*_tmp470=Cyc_Port_const_ct();Cyc_Port_add_var(_tmp473,_tmp472,_tmp471,_tmp470,Cyc_Port_arith_ct(),0);});env=_tmp474;});}
# 1289
return Cyc_Port_arith_ct();default: _LL1BE: _LL1BF:
 return Cyc_Port_arith_ct();}_LL1A9:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1295
static void*Cyc_Port_gen_primop(struct Cyc_Port_Cenv*env,enum Cyc_Absyn_Primop p,struct Cyc_List_List*args){
void*_tmp208=Cyc_Port_compress_ct((void*)((struct Cyc_List_List*)_check_null(args))->hd);
struct Cyc_List_List*_tmp209=args->tl;
enum Cyc_Absyn_Primop _tmp20A=p;enum Cyc_Absyn_Primop _tmp20F;switch(_tmp20A){case Cyc_Absyn_Plus: _LL1ED: _LL1EE: {
# 1303
void*_tmp20B=Cyc_Port_compress_ct((void*)((struct Cyc_List_List*)_check_null(_tmp209))->hd);
if(({void*_tmp479=_tmp208;Cyc_Port_leq(_tmp479,({void*_tmp478=Cyc_Port_var();void*_tmp477=Cyc_Port_var();void*_tmp476=Cyc_Port_fat_ct();void*_tmp475=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp478,_tmp477,_tmp476,_tmp475,Cyc_Port_var());}));})){
({void*_tmp47A=_tmp20B;Cyc_Port_leq(_tmp47A,Cyc_Port_arith_ct());});
return _tmp208;}else{
if(({void*_tmp47F=_tmp20B;Cyc_Port_leq(_tmp47F,({void*_tmp47E=Cyc_Port_var();void*_tmp47D=Cyc_Port_var();void*_tmp47C=Cyc_Port_fat_ct();void*_tmp47B=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp47E,_tmp47D,_tmp47C,_tmp47B,Cyc_Port_var());}));})){
({void*_tmp480=_tmp208;Cyc_Port_leq(_tmp480,Cyc_Port_arith_ct());});
return _tmp20B;}else{
# 1311
({void*_tmp481=_tmp208;Cyc_Port_leq(_tmp481,Cyc_Port_arith_ct());});
({void*_tmp482=_tmp20B;Cyc_Port_leq(_tmp482,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();}}}case Cyc_Absyn_Minus: _LL1EF: _LL1F0:
# 1320
 if(_tmp209 == 0){
# 1322
({void*_tmp483=_tmp208;Cyc_Port_leq(_tmp483,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();}else{
# 1325
void*_tmp20C=Cyc_Port_compress_ct((void*)_tmp209->hd);
if(({void*_tmp488=_tmp208;Cyc_Port_leq(_tmp488,({void*_tmp487=Cyc_Port_var();void*_tmp486=Cyc_Port_var();void*_tmp485=Cyc_Port_fat_ct();void*_tmp484=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp487,_tmp486,_tmp485,_tmp484,Cyc_Port_var());}));})){
if(({void*_tmp48D=_tmp20C;Cyc_Port_leq(_tmp48D,({void*_tmp48C=Cyc_Port_var();void*_tmp48B=Cyc_Port_var();void*_tmp48A=Cyc_Port_fat_ct();void*_tmp489=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp48C,_tmp48B,_tmp48A,_tmp489,Cyc_Port_var());}));}))
return Cyc_Port_arith_ct();else{
# 1330
({void*_tmp48E=_tmp20C;Cyc_Port_leq(_tmp48E,Cyc_Port_arith_ct());});
return _tmp208;}}else{
# 1334
({void*_tmp48F=_tmp208;Cyc_Port_leq(_tmp48F,Cyc_Port_arith_ct());});
({void*_tmp490=_tmp208;Cyc_Port_leq(_tmp490,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();}}case Cyc_Absyn_Times: _LL1F1: _LL1F2:
# 1339
 goto _LL1F4;case Cyc_Absyn_Div: _LL1F3: _LL1F4: goto _LL1F6;case Cyc_Absyn_Mod: _LL1F5: _LL1F6: goto _LL1F8;case Cyc_Absyn_Eq: _LL1F7: _LL1F8: goto _LL1FA;case Cyc_Absyn_Neq: _LL1F9: _LL1FA: goto _LL1FC;case Cyc_Absyn_Lt: _LL1FB: _LL1FC: goto _LL1FE;case Cyc_Absyn_Gt: _LL1FD: _LL1FE: goto _LL200;case Cyc_Absyn_Lte: _LL1FF: _LL200:
 goto _LL202;case Cyc_Absyn_Gte: _LL201: _LL202: goto _LL204;case Cyc_Absyn_Bitand: _LL203: _LL204: goto _LL206;case Cyc_Absyn_Bitor: _LL205: _LL206: goto _LL208;case Cyc_Absyn_Bitxor: _LL207: _LL208: goto _LL20A;case Cyc_Absyn_Bitlshift: _LL209: _LL20A: goto _LL20C;case Cyc_Absyn_Bitlrshift: _LL20B: _LL20C:
 goto _LL20E;case Cyc_Absyn_Bitarshift: _LL20D: _LL20E:
({void*_tmp491=_tmp208;Cyc_Port_leq(_tmp491,Cyc_Port_arith_ct());});
({void*_tmp492=(void*)((struct Cyc_List_List*)_check_null(_tmp209))->hd;Cyc_Port_leq(_tmp492,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();case Cyc_Absyn_Not: _LL20F: _LL210:
 goto _LL212;case Cyc_Absyn_Bitnot: _LL211: _LL212:
# 1347
({void*_tmp493=_tmp208;Cyc_Port_leq(_tmp493,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();default: _LL213: _tmp20F=_tmp20A;_LL214:
({void*_tmp20D=0;({struct _dyneither_ptr _tmp494=({const char*_tmp20E=".size primop";_tag_dyneither(_tmp20E,sizeof(char),13);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp494,_tag_dyneither(_tmp20D,sizeof(void*),0));});});}_LL1EC:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
# 1354
static struct _tuple10 Cyc_Port_gen_lhs(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e,int takeaddress){
void*_tmp210=e->r;void*_tmp211=_tmp210;struct Cyc_Absyn_Exp*_tmp237;struct _dyneither_ptr*_tmp236;struct Cyc_Absyn_Exp*_tmp235;struct _dyneither_ptr*_tmp234;struct Cyc_Absyn_Exp*_tmp233;struct Cyc_Absyn_Exp*_tmp232;struct Cyc_Absyn_Exp*_tmp231;void*_tmp230;switch(*((int*)_tmp211)){case 1: _LL216: _tmp230=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_LL217: {
# 1357
struct _tuple0*_tmp212=Cyc_Absyn_binding2qvar(_tmp230);
(*_tmp212).f1=Cyc_Absyn_Loc_n;{
struct _tuple13 _tmp213=Cyc_Port_lookup_var(env,_tmp212);struct _tuple13 _tmp214=_tmp213;struct _tuple10 _tmp216;unsigned int _tmp215;_LL223: _tmp216=_tmp214.f1;_tmp215=_tmp214.f2;_LL224:;
Cyc_Port_register_localvar_usage(env,_tmp215,e->loc,takeaddress);
return _tmp216;};}case 22: _LL218: _tmp232=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_tmp231=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp211)->f2;_LL219: {
# 1363
void*_tmp217=Cyc_Port_var();
void*_tmp218=Cyc_Port_var();
void*_tmp219=Cyc_Port_gen_exp(0,env,_tmp232);
({void*_tmp495=Cyc_Port_gen_exp(0,env,_tmp231);Cyc_Port_leq(_tmp495,Cyc_Port_arith_ct());});{
void*_tmp21A=({void*_tmp499=_tmp217;void*_tmp498=_tmp218;void*_tmp497=Cyc_Port_fat_ct();void*_tmp496=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp499,_tmp498,_tmp497,_tmp496,Cyc_Port_var());});
Cyc_Port_leq(_tmp219,_tmp21A);
return({struct _tuple10 _tmp21B;_tmp21B.f1=_tmp218;_tmp21B.f2=_tmp217;_tmp21B;});};}case 19: _LL21A: _tmp233=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_LL21B: {
# 1371
void*_tmp21C=Cyc_Port_var();
void*_tmp21D=Cyc_Port_var();
void*_tmp21E=({void*_tmp49D=_tmp21C;void*_tmp49C=_tmp21D;void*_tmp49B=Cyc_Port_var();void*_tmp49A=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp49D,_tmp49C,_tmp49B,_tmp49A,Cyc_Port_var());});
({void*_tmp49E=Cyc_Port_gen_exp(0,env,_tmp233);Cyc_Port_leq(_tmp49E,_tmp21E);});
return({struct _tuple10 _tmp21F;_tmp21F.f1=_tmp21D;_tmp21F.f2=_tmp21C;_tmp21F;});}case 20: _LL21C: _tmp235=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_tmp234=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp211)->f2;_LL21D: {
# 1377
void*_tmp220=Cyc_Port_var();
void*_tmp221=Cyc_Port_var();
void*_tmp222=Cyc_Port_gen_exp(takeaddress,env,_tmp235);
({void*_tmp4A0=_tmp222;Cyc_Port_leq(_tmp4A0,Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp223[1];({struct Cyc_Port_Cfield*_tmp49F=({struct Cyc_Port_Cfield*_tmp224=_cycalloc(sizeof(*_tmp224));_tmp224->qual=_tmp221;_tmp224->f=_tmp234;_tmp224->type=_tmp220;_tmp224;});_tmp223[0]=_tmp49F;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp223,sizeof(struct Cyc_Port_Cfield*),1));})));});
return({struct _tuple10 _tmp225;_tmp225.f1=_tmp221;_tmp225.f2=_tmp220;_tmp225;});}case 21: _LL21E: _tmp237=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_tmp236=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp211)->f2;_LL21F: {
# 1383
void*_tmp226=Cyc_Port_var();
void*_tmp227=Cyc_Port_var();
void*_tmp228=Cyc_Port_gen_exp(0,env,_tmp237);
({void*_tmp4A6=_tmp228;Cyc_Port_leq(_tmp4A6,({void*_tmp4A5=Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp229[1];({struct Cyc_Port_Cfield*_tmp4A1=({struct Cyc_Port_Cfield*_tmp22A=_cycalloc(sizeof(*_tmp22A));_tmp22A->qual=_tmp227;_tmp22A->f=_tmp236;_tmp22A->type=_tmp226;_tmp22A;});_tmp229[0]=_tmp4A1;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp229,sizeof(struct Cyc_Port_Cfield*),1));}));void*_tmp4A4=
Cyc_Port_var();
# 1386
void*_tmp4A3=
Cyc_Port_var();
# 1386
void*_tmp4A2=
Cyc_Port_var();
# 1386
Cyc_Port_ptr_ct(_tmp4A5,_tmp4A4,_tmp4A3,_tmp4A2,
Cyc_Port_var());}));});
return({struct _tuple10 _tmp22B;_tmp22B.f1=_tmp227;_tmp22B.f2=_tmp226;_tmp22B;});}default: _LL220: _LL221:
({void*_tmp22C=0;({struct _dyneither_ptr _tmp4A9=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp22F;_tmp22F.tag=0;({struct _dyneither_ptr _tmp4A7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp22F.f1=_tmp4A7;});({void*_tmp22D[1]={& _tmp22F};({struct _dyneither_ptr _tmp4A8=({const char*_tmp22E="gen_lhs: %s";_tag_dyneither(_tmp22E,sizeof(char),12);});Cyc_aprintf(_tmp4A8,_tag_dyneither(_tmp22D,sizeof(void*),1));});});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4A9,_tag_dyneither(_tmp22C,sizeof(void*),0));});});}_LL215:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1394
static void*Cyc_Port_gen_exp_false(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){
return Cyc_Port_gen_exp(0,env,e);}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
# 1397
static void*Cyc_Port_gen_exp(int takeaddress,struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){
void*_tmp238=e->r;void*_tmp239=_tmp238;struct Cyc_Absyn_Stmt*_tmp2AD;struct Cyc_Absyn_Exp*_tmp2AC;struct Cyc_Absyn_Exp*_tmp2AB;void**_tmp2AA;struct Cyc_Absyn_Exp*_tmp2A9;struct Cyc_Absyn_Exp*_tmp2A8;struct _dyneither_ptr*_tmp2A7;struct Cyc_Absyn_Exp*_tmp2A6;struct _dyneither_ptr*_tmp2A5;struct Cyc_Absyn_Exp*_tmp2A4;struct Cyc_Absyn_Exp*_tmp2A3;struct Cyc_Absyn_Exp*_tmp2A2;struct Cyc_Absyn_Exp*_tmp2A1;void*_tmp2A0;struct Cyc_Absyn_Exp*_tmp29F;struct Cyc_Absyn_Exp*_tmp29E;struct Cyc_Absyn_Exp*_tmp29D;struct Cyc_List_List*_tmp29C;struct Cyc_Absyn_Exp*_tmp29B;struct Cyc_Absyn_Exp*_tmp29A;struct Cyc_Absyn_Exp*_tmp299;struct Cyc_Absyn_Exp*_tmp298;struct Cyc_Absyn_Exp*_tmp297;struct Cyc_Absyn_Exp*_tmp296;struct Cyc_Absyn_Exp*_tmp295;struct Cyc_Absyn_Exp*_tmp294;struct Cyc_Absyn_Exp*_tmp293;struct Cyc_Absyn_Exp*_tmp292;enum Cyc_Absyn_Incrementor _tmp291;struct Cyc_Absyn_Exp*_tmp290;struct Cyc_Core_Opt*_tmp28F;struct Cyc_Absyn_Exp*_tmp28E;enum Cyc_Absyn_Primop _tmp28D;struct Cyc_List_List*_tmp28C;void*_tmp28B;switch(*((int*)_tmp239)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp239)->f1).Wstring_c).tag){case 2: _LL226: _LL227:
 goto _LL229;case 3: _LL228: _LL229:
 goto _LL22B;case 4: _LL22A: _LL22B:
 goto _LL22D;case 6: _LL22C: _LL22D:
 goto _LL22F;case 7: _LL238: _LL239:
# 1408
 return Cyc_Port_arith_ct();case 5: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp239)->f1).Int_c).val).f2 == 0){_LL23A: _LL23B:
 return Cyc_Port_zero_ct();}else{_LL23C: _LL23D:
 return Cyc_Port_arith_ct();}case 8: _LL23E: _LL23F:
# 1412
 return({void*_tmp4AD=Cyc_Port_arith_ct();void*_tmp4AC=Cyc_Port_const_ct();void*_tmp4AB=Cyc_Port_fat_ct();void*_tmp4AA=Cyc_Port_heap_ct();Cyc_Port_ptr_ct(_tmp4AD,_tmp4AC,_tmp4AB,_tmp4AA,Cyc_Port_zterm_ct());});case 9: _LL240: _LL241:
# 1414
 return({void*_tmp4B1=Cyc_Port_arith_ct();void*_tmp4B0=Cyc_Port_const_ct();void*_tmp4AF=Cyc_Port_fat_ct();void*_tmp4AE=Cyc_Port_heap_ct();Cyc_Port_ptr_ct(_tmp4B1,_tmp4B0,_tmp4AF,_tmp4AE,Cyc_Port_zterm_ct());});default: _LL242: _LL243:
 return Cyc_Port_zero_ct();}case 16: _LL22E: _LL22F:
# 1403
 goto _LL231;case 17: _LL230: _LL231:
 goto _LL233;case 18: _LL232: _LL233:
 goto _LL235;case 31: _LL234: _LL235:
 goto _LL237;case 32: _LL236: _LL237:
 goto _LL239;case 1: _LL244: _tmp28B=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_LL245: {
# 1417
struct _tuple0*_tmp23A=Cyc_Absyn_binding2qvar(_tmp28B);
(*_tmp23A).f1=Cyc_Absyn_Loc_n;{
struct _tuple13 _tmp23B=Cyc_Port_lookup_var(env,_tmp23A);struct _tuple13 _tmp23C=_tmp23B;struct _tuple10 _tmp245;unsigned int _tmp244;_LL289: _tmp245=_tmp23C.f1;_tmp244=_tmp23C.f2;_LL28A:;{
struct _tuple10 _tmp23D=_tmp245;void*_tmp243;_LL28C: _tmp243=_tmp23D.f2;_LL28D:;
if(Cyc_Port_isfn(env,_tmp23A)){
({void*_tmp4B2=Cyc_Port_instantiate(_tmp243);_tmp243=_tmp4B2;});
return({void*_tmp4B6=_tmp243;void*_tmp4B5=Cyc_Port_var();void*_tmp4B4=Cyc_Port_var();void*_tmp4B3=Cyc_Port_heap_ct();Cyc_Port_ptr_ct(_tmp4B6,_tmp4B5,_tmp4B4,_tmp4B3,Cyc_Port_nozterm_ct());});}else{
# 1425
if(Cyc_Port_isarray(env,_tmp23A)){
void*_tmp23E=Cyc_Port_var();
void*_tmp23F=Cyc_Port_var();
void*_tmp240=Cyc_Port_var();
void*_tmp241=Cyc_Port_array_ct(_tmp23E,_tmp23F,_tmp240);
Cyc_Port_unifies(_tmp243,_tmp241);{
void*_tmp242=({void*_tmp4BA=_tmp23E;void*_tmp4B9=_tmp23F;void*_tmp4B8=Cyc_Port_fat_ct();void*_tmp4B7=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4BA,_tmp4B9,_tmp4B8,_tmp4B7,_tmp240);});
return _tmp242;};}else{
# 1434
Cyc_Port_register_localvar_usage(env,_tmp244,e->loc,takeaddress);
return _tmp243;}}};};}case 2: _LL246: _tmp28D=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp28C=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_LL247:
# 1437
 return({struct Cyc_Port_Cenv*_tmp4BC=env;enum Cyc_Absyn_Primop _tmp4BB=_tmp28D;Cyc_Port_gen_primop(_tmp4BC,_tmp4BB,((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_gen_exp_false,env,_tmp28C));});case 3: _LL248: _tmp290=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp28F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_tmp28E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp239)->f3;_LL249: {
# 1439
struct _tuple10 _tmp246=Cyc_Port_gen_lhs(env,_tmp290,0);struct _tuple10 _tmp247=_tmp246;void*_tmp24D;void*_tmp24C;_LL28F: _tmp24D=_tmp247.f1;_tmp24C=_tmp247.f2;_LL290:;
({void*_tmp4BD=_tmp24D;Cyc_Port_unifies(_tmp4BD,Cyc_Port_notconst_ct());});{
void*_tmp248=Cyc_Port_gen_exp(0,env,_tmp28E);
if((unsigned int)_tmp28F){
struct Cyc_List_List x2=({struct Cyc_List_List _tmp24B;_tmp24B.hd=_tmp248;_tmp24B.tl=0;_tmp24B;});
struct Cyc_List_List x1=({struct Cyc_List_List _tmp24A;_tmp24A.hd=_tmp24C;_tmp24A.tl=& x2;_tmp24A;});
void*_tmp249=Cyc_Port_gen_primop(env,(enum Cyc_Absyn_Primop)_tmp28F->v,& x1);
Cyc_Port_leq(_tmp249,_tmp24C);
return _tmp24C;}else{
# 1449
Cyc_Port_leq(_tmp248,_tmp24C);
return _tmp24C;}};}case 4: _LL24A: _tmp292=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp291=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_LL24B: {
# 1453
struct _tuple10 _tmp24E=Cyc_Port_gen_lhs(env,_tmp292,0);struct _tuple10 _tmp24F=_tmp24E;void*_tmp251;void*_tmp250;_LL292: _tmp251=_tmp24F.f1;_tmp250=_tmp24F.f2;_LL293:;
({void*_tmp4BE=_tmp251;Cyc_Port_unifies(_tmp4BE,Cyc_Port_notconst_ct());});
# 1456
({void*_tmp4C3=_tmp250;Cyc_Port_leq(_tmp4C3,({void*_tmp4C2=Cyc_Port_var();void*_tmp4C1=Cyc_Port_var();void*_tmp4C0=Cyc_Port_fat_ct();void*_tmp4BF=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4C2,_tmp4C1,_tmp4C0,_tmp4BF,Cyc_Port_var());}));});
({void*_tmp4C4=_tmp250;Cyc_Port_leq(_tmp4C4,Cyc_Port_arith_ct());});
return _tmp250;}case 5: _LL24C: _tmp295=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp294=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_tmp293=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp239)->f3;_LL24D: {
# 1460
void*_tmp252=Cyc_Port_var();
({void*_tmp4C5=Cyc_Port_gen_exp(0,env,_tmp295);Cyc_Port_leq(_tmp4C5,Cyc_Port_arith_ct());});
({void*_tmp4C6=Cyc_Port_gen_exp(0,env,_tmp294);Cyc_Port_leq(_tmp4C6,_tmp252);});
({void*_tmp4C7=Cyc_Port_gen_exp(0,env,_tmp293);Cyc_Port_leq(_tmp4C7,_tmp252);});
return _tmp252;}case 6: _LL24E: _tmp297=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp296=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_LL24F:
 _tmp299=_tmp297;_tmp298=_tmp296;goto _LL251;case 7: _LL250: _tmp299=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp298=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_LL251:
# 1467
({void*_tmp4C8=Cyc_Port_gen_exp(0,env,_tmp299);Cyc_Port_leq(_tmp4C8,Cyc_Port_arith_ct());});
({void*_tmp4C9=Cyc_Port_gen_exp(0,env,_tmp298);Cyc_Port_leq(_tmp4C9,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();case 8: _LL252: _tmp29B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp29A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_LL253:
# 1471
 Cyc_Port_gen_exp(0,env,_tmp29B);
return Cyc_Port_gen_exp(0,env,_tmp29A);case 9: _LL254: _tmp29D=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp29C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_LL255: {
# 1474
void*_tmp253=Cyc_Port_var();
void*_tmp254=Cyc_Port_gen_exp(0,env,_tmp29D);
struct Cyc_List_List*_tmp255=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_gen_exp_false,env,_tmp29C);
struct Cyc_List_List*_tmp256=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(void*x))Cyc_Port_new_var,_tmp255);
({void*_tmp4CE=_tmp254;Cyc_Port_unifies(_tmp4CE,({void*_tmp4CD=Cyc_Port_fn_ct(_tmp253,_tmp256);void*_tmp4CC=Cyc_Port_var();void*_tmp4CB=Cyc_Port_var();void*_tmp4CA=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4CD,_tmp4CC,_tmp4CB,_tmp4CA,Cyc_Port_nozterm_ct());}));});
for(0;_tmp255 != 0;(_tmp255=_tmp255->tl,_tmp256=_tmp256->tl)){
Cyc_Port_leq((void*)_tmp255->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp256))->hd);}
# 1482
return _tmp253;}case 10: _LL256: _LL257:
({void*_tmp257=0;({struct _dyneither_ptr _tmp4CF=({const char*_tmp258="throw";_tag_dyneither(_tmp258,sizeof(char),6);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4CF,_tag_dyneither(_tmp257,sizeof(void*),0));});});case 11: _LL258: _tmp29E=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_LL259:
 return Cyc_Port_gen_exp(0,env,_tmp29E);case 12: _LL25A: _LL25B:
({void*_tmp259=0;({struct _dyneither_ptr _tmp4D0=({const char*_tmp25A="instantiate";_tag_dyneither(_tmp25A,sizeof(char),12);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4D0,_tag_dyneither(_tmp259,sizeof(void*),0));});});case 13: _LL25C: _tmp2A0=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp29F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_LL25D: {
# 1487
void*_tmp25B=Cyc_Port_type_to_ctype(env,_tmp2A0);
void*_tmp25C=Cyc_Port_gen_exp(0,env,_tmp29F);
Cyc_Port_leq(_tmp25B,_tmp25C);
return _tmp25C;}case 14: _LL25E: _tmp2A1=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_LL25F: {
# 1492
struct _tuple10 _tmp25D=Cyc_Port_gen_lhs(env,_tmp2A1,1);struct _tuple10 _tmp25E=_tmp25D;void*_tmp260;void*_tmp25F;_LL295: _tmp260=_tmp25E.f1;_tmp25F=_tmp25E.f2;_LL296:;
return({void*_tmp4D4=_tmp25F;void*_tmp4D3=_tmp260;void*_tmp4D2=Cyc_Port_var();void*_tmp4D1=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4D4,_tmp4D3,_tmp4D2,_tmp4D1,Cyc_Port_var());});}case 22: _LL260: _tmp2A3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp2A2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_LL261: {
# 1495
void*_tmp261=Cyc_Port_var();
({void*_tmp4D5=Cyc_Port_gen_exp(0,env,_tmp2A2);Cyc_Port_leq(_tmp4D5,Cyc_Port_arith_ct());});{
void*_tmp262=Cyc_Port_gen_exp(0,env,_tmp2A3);
({void*_tmp4DA=_tmp262;Cyc_Port_leq(_tmp4DA,({void*_tmp4D9=_tmp261;void*_tmp4D8=Cyc_Port_var();void*_tmp4D7=Cyc_Port_fat_ct();void*_tmp4D6=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4D9,_tmp4D8,_tmp4D7,_tmp4D6,Cyc_Port_var());}));});
return _tmp261;};}case 19: _LL262: _tmp2A4=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_LL263: {
# 1501
void*_tmp263=Cyc_Port_var();
({void*_tmp4DF=Cyc_Port_gen_exp(0,env,_tmp2A4);Cyc_Port_leq(_tmp4DF,({void*_tmp4DE=_tmp263;void*_tmp4DD=Cyc_Port_var();void*_tmp4DC=Cyc_Port_var();void*_tmp4DB=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4DE,_tmp4DD,_tmp4DC,_tmp4DB,Cyc_Port_var());}));});
return _tmp263;}case 20: _LL264: _tmp2A6=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp2A5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_LL265: {
# 1505
void*_tmp264=Cyc_Port_var();
void*_tmp265=Cyc_Port_gen_exp(takeaddress,env,_tmp2A6);
({void*_tmp4E2=_tmp265;Cyc_Port_leq(_tmp4E2,Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp266[1];({struct Cyc_Port_Cfield*_tmp4E1=({struct Cyc_Port_Cfield*_tmp267=_cycalloc(sizeof(*_tmp267));({void*_tmp4E0=Cyc_Port_var();_tmp267->qual=_tmp4E0;});_tmp267->f=_tmp2A5;_tmp267->type=_tmp264;_tmp267;});_tmp266[0]=_tmp4E1;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp266,sizeof(struct Cyc_Port_Cfield*),1));})));});
return _tmp264;}case 21: _LL266: _tmp2A8=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp2A7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_LL267: {
# 1510
void*_tmp268=Cyc_Port_var();
void*_tmp269=Cyc_Port_gen_exp(0,env,_tmp2A8);
({void*_tmp4E9=Cyc_Port_gen_exp(0,env,_tmp2A8);Cyc_Port_leq(_tmp4E9,({void*_tmp4E8=Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp26A[1];({struct Cyc_Port_Cfield*_tmp4E4=({struct Cyc_Port_Cfield*_tmp26B=_cycalloc(sizeof(*_tmp26B));({void*_tmp4E3=Cyc_Port_var();_tmp26B->qual=_tmp4E3;});_tmp26B->f=_tmp2A7;_tmp26B->type=_tmp268;_tmp26B;});_tmp26A[0]=_tmp4E4;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp26A,sizeof(struct Cyc_Port_Cfield*),1));}));void*_tmp4E7=
Cyc_Port_var();
# 1512
void*_tmp4E6=
Cyc_Port_var();
# 1512
void*_tmp4E5=
Cyc_Port_var();
# 1512
Cyc_Port_ptr_ct(_tmp4E8,_tmp4E7,_tmp4E6,_tmp4E5,
Cyc_Port_var());}));});
return _tmp268;}case 33: _LL268: _tmp2AA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp239)->f1).elt_type;_tmp2A9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp239)->f1).num_elts;_LL269:
# 1516
({void*_tmp4EA=Cyc_Port_gen_exp(0,env,_tmp2A9);Cyc_Port_leq(_tmp4EA,Cyc_Port_arith_ct());});{
void*_tmp26C=(unsigned int)_tmp2AA?Cyc_Port_type_to_ctype(env,*_tmp2AA): Cyc_Port_var();
return({void*_tmp4EE=_tmp26C;void*_tmp4ED=Cyc_Port_var();void*_tmp4EC=Cyc_Port_fat_ct();void*_tmp4EB=Cyc_Port_heap_ct();Cyc_Port_ptr_ct(_tmp4EE,_tmp4ED,_tmp4EC,_tmp4EB,Cyc_Port_var());});};case 34: _LL26A: _tmp2AC=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp2AB=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_LL26B:
({void*_tmp26D=0;({struct _dyneither_ptr _tmp4EF=({const char*_tmp26E="Swap_e";_tag_dyneither(_tmp26E,sizeof(char),7);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4EF,_tag_dyneither(_tmp26D,sizeof(void*),0));});});case 15: _LL26C: _LL26D:
({void*_tmp26F=0;({struct _dyneither_ptr _tmp4F0=({const char*_tmp270="New_e";_tag_dyneither(_tmp270,sizeof(char),6);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F0,_tag_dyneither(_tmp26F,sizeof(void*),0));});});case 23: _LL26E: _LL26F:
({void*_tmp271=0;({struct _dyneither_ptr _tmp4F1=({const char*_tmp272="Tuple_e";_tag_dyneither(_tmp272,sizeof(char),8);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F1,_tag_dyneither(_tmp271,sizeof(void*),0));});});case 24: _LL270: _LL271:
({void*_tmp273=0;({struct _dyneither_ptr _tmp4F2=({const char*_tmp274="CompoundLit_e";_tag_dyneither(_tmp274,sizeof(char),14);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F2,_tag_dyneither(_tmp273,sizeof(void*),0));});});case 25: _LL272: _LL273:
({void*_tmp275=0;({struct _dyneither_ptr _tmp4F3=({const char*_tmp276="Array_e";_tag_dyneither(_tmp276,sizeof(char),8);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F3,_tag_dyneither(_tmp275,sizeof(void*),0));});});case 26: _LL274: _LL275:
({void*_tmp277=0;({struct _dyneither_ptr _tmp4F4=({const char*_tmp278="Comprehension_e";_tag_dyneither(_tmp278,sizeof(char),16);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F4,_tag_dyneither(_tmp277,sizeof(void*),0));});});case 27: _LL276: _LL277:
({void*_tmp279=0;({struct _dyneither_ptr _tmp4F5=({const char*_tmp27A="ComprehensionNoinit_e";_tag_dyneither(_tmp27A,sizeof(char),22);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F5,_tag_dyneither(_tmp279,sizeof(void*),0));});});case 28: _LL278: _LL279:
({void*_tmp27B=0;({struct _dyneither_ptr _tmp4F6=({const char*_tmp27C="Aggregate_e";_tag_dyneither(_tmp27C,sizeof(char),12);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F6,_tag_dyneither(_tmp27B,sizeof(void*),0));});});case 29: _LL27A: _LL27B:
({void*_tmp27D=0;({struct _dyneither_ptr _tmp4F7=({const char*_tmp27E="AnonStruct_e";_tag_dyneither(_tmp27E,sizeof(char),13);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F7,_tag_dyneither(_tmp27D,sizeof(void*),0));});});case 30: _LL27C: _LL27D:
({void*_tmp27F=0;({struct _dyneither_ptr _tmp4F8=({const char*_tmp280="Datatype_e";_tag_dyneither(_tmp280,sizeof(char),11);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F8,_tag_dyneither(_tmp27F,sizeof(void*),0));});});case 35: _LL27E: _LL27F:
({void*_tmp281=0;({struct _dyneither_ptr _tmp4F9=({const char*_tmp282="UnresolvedMem_e";_tag_dyneither(_tmp282,sizeof(char),16);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F9,_tag_dyneither(_tmp281,sizeof(void*),0));});});case 36: _LL280: _tmp2AD=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_LL281:
({void*_tmp283=0;({struct _dyneither_ptr _tmp4FA=({const char*_tmp284="StmtExp_e";_tag_dyneither(_tmp284,sizeof(char),10);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4FA,_tag_dyneither(_tmp283,sizeof(void*),0));});});case 38: _LL282: _LL283:
({void*_tmp285=0;({struct _dyneither_ptr _tmp4FB=({const char*_tmp286="Valueof_e";_tag_dyneither(_tmp286,sizeof(char),10);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4FB,_tag_dyneither(_tmp285,sizeof(void*),0));});});case 39: _LL284: _LL285:
({void*_tmp287=0;({struct _dyneither_ptr _tmp4FC=({const char*_tmp288="Asm_e";_tag_dyneither(_tmp288,sizeof(char),6);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4FC,_tag_dyneither(_tmp287,sizeof(void*),0));});});default: _LL286: _LL287:
({void*_tmp289=0;({struct _dyneither_ptr _tmp4FD=({const char*_tmp28A="Tagcheck_e";_tag_dyneither(_tmp28A,sizeof(char),11);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4FD,_tag_dyneither(_tmp289,sizeof(void*),0));});});}_LL225:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1538
static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Stmt*s){
void*_tmp2AE=s->r;void*_tmp2AF=_tmp2AE;struct Cyc_Absyn_Stmt*_tmp2C9;struct Cyc_Absyn_Exp*_tmp2C8;struct Cyc_Absyn_Stmt*_tmp2C7;struct Cyc_Absyn_Decl*_tmp2C6;struct Cyc_Absyn_Stmt*_tmp2C5;struct Cyc_Absyn_Exp*_tmp2C4;struct Cyc_List_List*_tmp2C3;struct Cyc_Absyn_Exp*_tmp2C2;struct Cyc_Absyn_Exp*_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C0;struct Cyc_Absyn_Stmt*_tmp2BF;struct Cyc_Absyn_Exp*_tmp2BE;struct Cyc_Absyn_Stmt*_tmp2BD;struct Cyc_Absyn_Exp*_tmp2BC;struct Cyc_Absyn_Stmt*_tmp2BB;struct Cyc_Absyn_Stmt*_tmp2BA;struct Cyc_Absyn_Exp*_tmp2B9;struct Cyc_Absyn_Stmt*_tmp2B8;struct Cyc_Absyn_Stmt*_tmp2B7;struct Cyc_Absyn_Exp*_tmp2B6;switch(*((int*)_tmp2AF)){case 0: _LL298: _LL299:
 return;case 1: _LL29A: _tmp2B6=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp2AF)->f1;_LL29B:
 Cyc_Port_gen_exp(0,env,_tmp2B6);return;case 2: _LL29C: _tmp2B8=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp2AF)->f1;_tmp2B7=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp2AF)->f2;_LL29D:
 Cyc_Port_gen_stmt(env,_tmp2B8);Cyc_Port_gen_stmt(env,_tmp2B7);return;case 3: _LL29E: _tmp2B9=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp2AF)->f1;_LL29F: {
# 1544
void*_tmp2B0=Cyc_Port_lookup_return_type(env);
void*_tmp2B1=(unsigned int)_tmp2B9?Cyc_Port_gen_exp(0,env,_tmp2B9): Cyc_Port_void_ct();
Cyc_Port_leq(_tmp2B1,_tmp2B0);
return;}case 4: _LL2A0: _tmp2BC=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp2AF)->f1;_tmp2BB=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp2AF)->f2;_tmp2BA=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp2AF)->f3;_LL2A1:
# 1549
({void*_tmp4FE=Cyc_Port_gen_exp(0,env,_tmp2BC);Cyc_Port_leq(_tmp4FE,Cyc_Port_arith_ct());});
Cyc_Port_gen_stmt(env,_tmp2BB);
Cyc_Port_gen_stmt(env,_tmp2BA);
return;case 5: _LL2A2: _tmp2BE=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp2AF)->f1).f1;_tmp2BD=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp2AF)->f2;_LL2A3:
# 1554
({void*_tmp4FF=Cyc_Port_gen_exp(0,env,_tmp2BE);Cyc_Port_leq(_tmp4FF,Cyc_Port_arith_ct());});
Cyc_Port_gen_stmt(env,_tmp2BD);
return;case 6: _LL2A4: _LL2A5:
 goto _LL2A7;case 7: _LL2A6: _LL2A7:
 goto _LL2A9;case 8: _LL2A8: _LL2A9:
 return;case 9: _LL2AA: _tmp2C2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp2AF)->f1;_tmp2C1=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp2AF)->f2).f1;_tmp2C0=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp2AF)->f3).f1;_tmp2BF=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp2AF)->f4;_LL2AB:
# 1561
 Cyc_Port_gen_exp(0,env,_tmp2C2);
({void*_tmp500=Cyc_Port_gen_exp(0,env,_tmp2C1);Cyc_Port_leq(_tmp500,Cyc_Port_arith_ct());});
Cyc_Port_gen_exp(0,env,_tmp2C0);
Cyc_Port_gen_stmt(env,_tmp2BF);
return;case 10: _LL2AC: _tmp2C4=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp2AF)->f1;_tmp2C3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp2AF)->f2;_LL2AD:
# 1567
({void*_tmp501=Cyc_Port_gen_exp(0,env,_tmp2C4);Cyc_Port_leq(_tmp501,Cyc_Port_arith_ct());});
for(0;(unsigned int)_tmp2C3;_tmp2C3=_tmp2C3->tl){
# 1570
Cyc_Port_gen_stmt(env,((struct Cyc_Absyn_Switch_clause*)_tmp2C3->hd)->body);}
# 1572
return;case 11: _LL2AE: _LL2AF:
({void*_tmp2B2=0;({struct _dyneither_ptr _tmp502=({const char*_tmp2B3="fallthru";_tag_dyneither(_tmp2B3,sizeof(char),9);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp502,_tag_dyneither(_tmp2B2,sizeof(void*),0));});});case 12: _LL2B0: _tmp2C6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp2AF)->f1;_tmp2C5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp2AF)->f2;_LL2B1:
# 1575
({struct Cyc_Port_Cenv*_tmp503=Cyc_Port_gen_localdecl(env,_tmp2C6,0);env=_tmp503;});Cyc_Port_gen_stmt(env,_tmp2C5);return;case 13: _LL2B2: _tmp2C7=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp2AF)->f2;_LL2B3:
 Cyc_Port_gen_stmt(env,_tmp2C7);return;case 14: _LL2B4: _tmp2C9=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp2AF)->f1;_tmp2C8=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp2AF)->f2).f1;_LL2B5:
# 1578
 Cyc_Port_gen_stmt(env,_tmp2C9);
({void*_tmp504=Cyc_Port_gen_exp(0,env,_tmp2C8);Cyc_Port_leq(_tmp504,Cyc_Port_arith_ct());});
return;default: _LL2B6: _LL2B7:
({void*_tmp2B4=0;({struct _dyneither_ptr _tmp505=({const char*_tmp2B5="try/catch";_tag_dyneither(_tmp2B5,sizeof(char),10);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp505,_tag_dyneither(_tmp2B4,sizeof(void*),0));});});}_LL297:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1586
static void*Cyc_Port_gen_initializer(struct Cyc_Port_Cenv*env,void*t,struct Cyc_Absyn_Exp*e){
void*_tmp2CA=e->r;void*_tmp2CB=_tmp2CA;struct Cyc_List_List*_tmp2F2;struct Cyc_List_List*_tmp2F1;struct Cyc_List_List*_tmp2F0;switch(*((int*)_tmp2CB)){case 35: _LL2B9: _tmp2F0=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp2CB)->f2;_LL2BA:
 _tmp2F1=_tmp2F0;goto _LL2BC;case 25: _LL2BB: _tmp2F1=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1;_LL2BC:
 _tmp2F2=_tmp2F1;goto _LL2BE;case 24: _LL2BD: _tmp2F2=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp2CB)->f2;_LL2BE:
# 1591
 LOOP: {
void*_tmp2CC=t;struct Cyc_Absyn_Aggrdecl*_tmp2ED;struct _tuple0*_tmp2EC;void*_tmp2EB;union Cyc_Absyn_Constraint*_tmp2EA;unsigned int _tmp2E9;struct _tuple0*_tmp2E8;switch(*((int*)_tmp2CC)){case 17: _LL2C6: _tmp2E8=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2CC)->f1;_LL2C7:
# 1594
(*_tmp2E8).f1=Cyc_Absyn_Loc_n;
({void*_tmp506=Cyc_Port_lookup_typedef(env,_tmp2E8);t=_tmp506;});goto LOOP;case 8: _LL2C8: _tmp2EB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2CC)->f1).elt_type;_tmp2EA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2CC)->f1).zero_term;_tmp2E9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2CC)->f1).zt_loc;_LL2C9: {
# 1597
void*_tmp2CD=Cyc_Port_var();
void*_tmp2CE=Cyc_Port_var();
void*_tmp2CF=Cyc_Port_var();
void*_tmp2D0=Cyc_Port_type_to_ctype(env,_tmp2EB);
for(0;_tmp2F2 != 0;_tmp2F2=_tmp2F2->tl){
struct _tuple17 _tmp2D1=*((struct _tuple17*)_tmp2F2->hd);struct _tuple17 _tmp2D2=_tmp2D1;struct Cyc_List_List*_tmp2D7;struct Cyc_Absyn_Exp*_tmp2D6;_LL2D1: _tmp2D7=_tmp2D2.f1;_tmp2D6=_tmp2D2.f2;_LL2D2:;
if((unsigned int)_tmp2D7)({void*_tmp2D3=0;({struct _dyneither_ptr _tmp507=({const char*_tmp2D4="designators in initializers";_tag_dyneither(_tmp2D4,sizeof(char),28);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp507,_tag_dyneither(_tmp2D3,sizeof(void*),0));});});{
void*_tmp2D5=Cyc_Port_gen_initializer(env,_tmp2EB,_tmp2D6);
Cyc_Port_leq(_tmp2D5,_tmp2CD);};}
# 1607
return Cyc_Port_array_ct(_tmp2CD,_tmp2CE,_tmp2CF);}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2CC)->f1).aggr_info).UnknownAggr).tag == 1){if(((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2CC)->f1).aggr_info).UnknownAggr).val).f1 == Cyc_Absyn_StructA){_LL2CA: _tmp2EC=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2CC)->f1).aggr_info).UnknownAggr).val).f2;_LL2CB:
# 1609
(*_tmp2EC).f1=Cyc_Absyn_Loc_n;{
struct _tuple11 _tmp2D8=*Cyc_Port_lookup_struct_decl(env,_tmp2EC);struct _tuple11 _tmp2D9=_tmp2D8;struct Cyc_Absyn_Aggrdecl*_tmp2DC;_LL2D4: _tmp2DC=_tmp2D9.f1;_LL2D5:;
if(_tmp2DC == 0)({void*_tmp2DA=0;({struct _dyneither_ptr _tmp508=({const char*_tmp2DB="struct is not yet defined";_tag_dyneither(_tmp2DB,sizeof(char),26);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp508,_tag_dyneither(_tmp2DA,sizeof(void*),0));});});
_tmp2ED=_tmp2DC;goto _LL2CD;};}else{goto _LL2CE;}}else{_LL2CC: _tmp2ED=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2CC)->f1).aggr_info).KnownAggr).val;_LL2CD: {
# 1614
struct Cyc_List_List*_tmp2DD=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2ED->impl))->fields;
for(0;_tmp2F2 != 0;_tmp2F2=_tmp2F2->tl){
struct _tuple17 _tmp2DE=*((struct _tuple17*)_tmp2F2->hd);struct _tuple17 _tmp2DF=_tmp2DE;struct Cyc_List_List*_tmp2E5;struct Cyc_Absyn_Exp*_tmp2E4;_LL2D7: _tmp2E5=_tmp2DF.f1;_tmp2E4=_tmp2DF.f2;_LL2D8:;
if((unsigned int)_tmp2E5)({void*_tmp2E0=0;({struct _dyneither_ptr _tmp509=({const char*_tmp2E1="designators in initializers";_tag_dyneither(_tmp2E1,sizeof(char),28);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp509,_tag_dyneither(_tmp2E0,sizeof(void*),0));});});{
struct Cyc_Absyn_Aggrfield*_tmp2E2=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp2DD))->hd;
_tmp2DD=_tmp2DD->tl;{
void*_tmp2E3=Cyc_Port_gen_initializer(env,_tmp2E2->type,_tmp2E4);;};};}
# 1622
return Cyc_Port_type_to_ctype(env,t);}}default: _LL2CE: _LL2CF:
({void*_tmp2E6=0;({struct _dyneither_ptr _tmp50A=({const char*_tmp2E7="bad type for aggregate initializer";_tag_dyneither(_tmp2E7,sizeof(char),35);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp50A,_tag_dyneither(_tmp2E6,sizeof(void*),0));});});}_LL2C5:;}case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2CB)->f1).Wstring_c).tag){case 8: _LL2BF: _LL2C0:
# 1625
 goto _LL2C2;case 9: _LL2C1: _LL2C2:
# 1627
 LOOP2: {
void*_tmp2EE=t;struct _tuple0*_tmp2EF;switch(*((int*)_tmp2EE)){case 17: _LL2DA: _tmp2EF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2EE)->f1;_LL2DB:
# 1630
(*_tmp2EF).f1=Cyc_Absyn_Loc_n;
({void*_tmp50B=Cyc_Port_lookup_typedef(env,_tmp2EF);t=_tmp50B;});goto LOOP2;case 8: _LL2DC: _LL2DD:
 return({void*_tmp50D=Cyc_Port_arith_ct();void*_tmp50C=Cyc_Port_const_ct();Cyc_Port_array_ct(_tmp50D,_tmp50C,Cyc_Port_zterm_ct());});default: _LL2DE: _LL2DF:
 return Cyc_Port_gen_exp(0,env,e);}_LL2D9:;}default: goto _LL2C3;}default: _LL2C3: _LL2C4:
# 1635
 return Cyc_Port_gen_exp(0,env,e);}_LL2B8:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1640
static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d,int fromglobal){
void*_tmp2F3=d->r;void*_tmp2F4=_tmp2F3;struct Cyc_Absyn_Vardecl*_tmp2F8;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2F4)->tag == 0){_LL2E1: _tmp2F8=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2F4)->f1;_LL2E2:
# 1643
 if(!fromglobal)Cyc_Port_register_localvar_decl(env,_tmp2F8->name,_tmp2F8->varloc,_tmp2F8->type,_tmp2F8->initializer);{
void*_tmp2F5=Cyc_Port_var();
void*q;
if((env->gcenv)->porting){
({void*_tmp50E=Cyc_Port_var();q=_tmp50E;});
({struct Cyc_Port_Cenv*_tmp511=env;void*_tmp510=q;void*_tmp50F=
(_tmp2F8->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();
# 1648
Cyc_Port_register_const_cvar(_tmp511,_tmp510,_tmp50F,(_tmp2F8->tq).loc);});}else{
# 1657
({void*_tmp512=(_tmp2F8->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();q=_tmp512;});}
# 1659
(*_tmp2F8->name).f1=Cyc_Absyn_Loc_n;
({struct Cyc_Port_Cenv*_tmp513=Cyc_Port_add_var(env,_tmp2F8->name,_tmp2F8->type,q,_tmp2F5,_tmp2F8->varloc);env=_tmp513;});
({void*_tmp514=_tmp2F5;Cyc_Port_unifies(_tmp514,Cyc_Port_type_to_ctype(env,_tmp2F8->type));});
if((unsigned int)_tmp2F8->initializer){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp2F8->initializer);
void*t2=Cyc_Port_gen_initializer(env,_tmp2F8->type,e);
Cyc_Port_leq(t2,_tmp2F5);}
# 1667
return env;};}else{_LL2E3: _LL2E4:
({void*_tmp2F6=0;({struct _dyneither_ptr _tmp515=({const char*_tmp2F7="non-local decl that isn't a variable";_tag_dyneither(_tmp2F7,sizeof(char),37);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp515,_tag_dyneither(_tmp2F6,sizeof(void*),0));});});}_LL2E0:;}
# 1672
static struct _tuple8*Cyc_Port_make_targ(struct _tuple8*arg){
struct _tuple8 _tmp2F9=*arg;struct _tuple8 _tmp2FA=_tmp2F9;struct _dyneither_ptr*_tmp2FE;struct Cyc_Absyn_Tqual _tmp2FD;void*_tmp2FC;_LL2E6: _tmp2FE=_tmp2FA.f1;_tmp2FD=_tmp2FA.f2;_tmp2FC=_tmp2FA.f3;_LL2E7:;
return({struct _tuple8*_tmp2FB=_cycalloc(sizeof(*_tmp2FB));_tmp2FB->f1=0;_tmp2FB->f2=_tmp2FD;_tmp2FB->f3=_tmp2FC;_tmp2FB;});}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1677
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*env,struct Cyc_List_List*ds);struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct _tuple18{struct _dyneither_ptr*f1;void*f2;void*f3;void*f4;};struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1679
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d){
void*_tmp2FF=d->r;void*_tmp300=_tmp2FF;struct Cyc_Absyn_Enumdecl*_tmp340;struct Cyc_Absyn_Aggrdecl*_tmp33F;struct Cyc_Absyn_Typedefdecl*_tmp33E;struct Cyc_Absyn_Fndecl*_tmp33D;struct Cyc_Absyn_Vardecl*_tmp33C;switch(*((int*)_tmp300)){case 13: _LL2E9: _LL2EA:
# 1682
(env->gcenv)->porting=1;
return env;case 14: _LL2EB: _LL2EC:
# 1685
(env->gcenv)->porting=0;
return env;case 0: _LL2ED: _tmp33C=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp300)->f1;_LL2EE:
# 1688
(*_tmp33C->name).f1=Cyc_Absyn_Loc_n;
# 1692
if(Cyc_Port_declared_var(env,_tmp33C->name)){
struct _tuple13 _tmp301=Cyc_Port_lookup_var(env,_tmp33C->name);struct _tuple13 _tmp302=_tmp301;struct _tuple10 _tmp307;unsigned int _tmp306;_LL2FA: _tmp307=_tmp302.f1;_tmp306=_tmp302.f2;_LL2FB:;{
struct _tuple10 _tmp303=_tmp307;void*_tmp305;void*_tmp304;_LL2FD: _tmp305=_tmp303.f1;_tmp304=_tmp303.f2;_LL2FE:;{
void*q2;
if((env->gcenv)->porting  && !Cyc_Port_isfn(env,_tmp33C->name)){
({void*_tmp516=Cyc_Port_var();q2=_tmp516;});
({struct Cyc_Port_Cenv*_tmp519=env;void*_tmp518=q2;void*_tmp517=
(_tmp33C->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();
# 1698
Cyc_Port_register_const_cvar(_tmp519,_tmp518,_tmp517,(_tmp33C->tq).loc);});}else{
# 1707
({void*_tmp51A=(_tmp33C->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();q2=_tmp51A;});}
# 1709
Cyc_Port_unifies(_tmp305,q2);
({void*_tmp51B=_tmp304;Cyc_Port_unifies(_tmp51B,Cyc_Port_type_to_ctype(env,_tmp33C->type));});
if((unsigned int)_tmp33C->initializer){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp33C->initializer);
({void*_tmp51C=Cyc_Port_gen_initializer(env,_tmp33C->type,e);Cyc_Port_leq(_tmp51C,_tmp304);});}
# 1715
return env;};};}else{
# 1717
return Cyc_Port_gen_localdecl(env,d,1);}case 1: _LL2EF: _tmp33D=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp300)->f1;_LL2F0:
# 1723
(*_tmp33D->name).f1=Cyc_Absyn_Loc_n;{
struct _tuple14*predeclared=0;
# 1726
if(Cyc_Port_declared_var(env,_tmp33D->name))
({struct _tuple14*_tmp51D=Cyc_Port_lookup_full_var(env,_tmp33D->name);predeclared=_tmp51D;});{
# 1729
void*_tmp308=_tmp33D->ret_type;
struct Cyc_List_List*_tmp309=_tmp33D->args;
struct Cyc_List_List*_tmp30A=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Port_make_targ,_tmp309);
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp30B=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp326=_cycalloc(sizeof(*_tmp326));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp520=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp327;_tmp327.tag=9;({struct Cyc_Absyn_FnInfo _tmp51F=({struct Cyc_Absyn_FnInfo _tmp328;_tmp328.tvars=0;_tmp328.effect=0;({struct Cyc_Absyn_Tqual _tmp51E=
Cyc_Absyn_empty_tqual(0);_tmp328.ret_tqual=_tmp51E;});_tmp328.ret_typ=_tmp308;_tmp328.args=_tmp30A;_tmp328.c_varargs=0;_tmp328.cyc_varargs=0;_tmp328.rgn_po=0;_tmp328.attributes=0;_tmp328.requires_clause=0;_tmp328.requires_relns=0;_tmp328.ensures_clause=0;_tmp328.ensures_relns=0;_tmp328;});_tmp327.f1=_tmp51F;});_tmp327;});_tmp326[0]=_tmp520;});_tmp326;});
# 1736
struct Cyc_Port_Cenv*_tmp30C=Cyc_Port_set_cpos(env,Cyc_Port_FnBody_pos);
void*_tmp30D=Cyc_Port_type_to_ctype(_tmp30C,_tmp308);
struct Cyc_List_List*c_args=0;
struct Cyc_List_List*c_arg_types=0;
{struct Cyc_List_List*_tmp30E=_tmp309;for(0;(unsigned int)_tmp30E;_tmp30E=_tmp30E->tl){
struct _tuple8 _tmp30F=*((struct _tuple8*)_tmp30E->hd);struct _tuple8 _tmp310=_tmp30F;struct _dyneither_ptr*_tmp317;struct Cyc_Absyn_Tqual _tmp316;void*_tmp315;_LL300: _tmp317=_tmp310.f1;_tmp316=_tmp310.f2;_tmp315=_tmp310.f3;_LL301:;{
# 1744
void*_tmp311=Cyc_Port_type_to_ctype(_tmp30C,_tmp315);
void*tqv;
if((env->gcenv)->porting){
({void*_tmp521=Cyc_Port_var();tqv=_tmp521;});
({struct Cyc_Port_Cenv*_tmp524=env;void*_tmp523=tqv;void*_tmp522=_tmp316.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_register_const_cvar(_tmp524,_tmp523,_tmp522,_tmp316.loc);});}else{
# 1756
({void*_tmp525=_tmp316.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();tqv=_tmp525;});}
# 1758
({struct Cyc_List_List*_tmp527=({struct Cyc_List_List*_tmp312=_cycalloc(sizeof(*_tmp312));({struct _tuple18*_tmp526=({struct _tuple18*_tmp313=_cycalloc(sizeof(*_tmp313));_tmp313->f1=_tmp317;_tmp313->f2=_tmp315;_tmp313->f3=tqv;_tmp313->f4=_tmp311;_tmp313;});_tmp312->hd=_tmp526;});_tmp312->tl=c_args;_tmp312;});c_args=_tmp527;});
({struct Cyc_List_List*_tmp528=({struct Cyc_List_List*_tmp314=_cycalloc(sizeof(*_tmp314));_tmp314->hd=_tmp311;_tmp314->tl=c_arg_types;_tmp314;});c_arg_types=_tmp528;});};}}
# 1761
({struct Cyc_List_List*_tmp529=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(c_args);c_args=_tmp529;});
({struct Cyc_List_List*_tmp52A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(c_arg_types);c_arg_types=_tmp52A;});{
void*_tmp318=Cyc_Port_fn_ct(_tmp30D,c_arg_types);
# 1767
(*_tmp33D->name).f1=Cyc_Absyn_Loc_n;
({struct Cyc_Port_Cenv*_tmp52F=({struct Cyc_Port_Cenv*_tmp52E=_tmp30C;struct _tuple0*_tmp52D=_tmp33D->name;void*_tmp52C=(void*)_tmp30B;void*_tmp52B=Cyc_Port_const_ct();Cyc_Port_add_var(_tmp52E,_tmp52D,_tmp52C,_tmp52B,_tmp318,0);});_tmp30C=_tmp52F;});
Cyc_Port_add_return_type(_tmp30C,_tmp30D);
{struct Cyc_List_List*_tmp319=c_args;for(0;(unsigned int)_tmp319;_tmp319=_tmp319->tl){
struct _tuple18 _tmp31A=*((struct _tuple18*)_tmp319->hd);struct _tuple18 _tmp31B=_tmp31A;struct _dyneither_ptr*_tmp320;void*_tmp31F;void*_tmp31E;void*_tmp31D;_LL303: _tmp320=_tmp31B.f1;_tmp31F=_tmp31B.f2;_tmp31E=_tmp31B.f3;_tmp31D=_tmp31B.f4;_LL304:;
({struct Cyc_Port_Cenv*_tmp534=({struct Cyc_Port_Cenv*_tmp533=_tmp30C;struct _tuple0*_tmp532=({struct _tuple0*_tmp31C=_cycalloc(sizeof(*_tmp31C));_tmp31C->f1=Cyc_Absyn_Loc_n;_tmp31C->f2=_tmp320;_tmp31C;});void*_tmp531=_tmp31F;void*_tmp530=_tmp31E;Cyc_Port_add_var(_tmp533,_tmp532,_tmp531,_tmp530,_tmp31D,0);});_tmp30C=_tmp534;});}}
# 1774
Cyc_Port_gen_stmt(_tmp30C,_tmp33D->body);
# 1779
Cyc_Port_generalize(0,_tmp318);{
# 1786
struct Cyc_Dict_Dict counts=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);
Cyc_Port_region_counts(& counts,_tmp318);
# 1790
Cyc_Port_register_rgns(env,counts,1,(void*)_tmp30B,_tmp318);
({struct Cyc_Port_Cenv*_tmp539=({struct Cyc_Port_Cenv*_tmp538=_tmp30C;struct _tuple0*_tmp537=_tmp33D->name;void*_tmp536=(void*)_tmp30B;void*_tmp535=Cyc_Port_const_ct();Cyc_Port_add_var(_tmp538,_tmp537,_tmp536,_tmp535,_tmp318,0);});env=_tmp539;});
if((unsigned int)predeclared){
# 1799
struct _tuple14 _tmp321=*predeclared;struct _tuple14 _tmp322=_tmp321;void*_tmp325;void*_tmp324;void*_tmp323;_LL306: _tmp325=_tmp322.f1;_tmp324=(_tmp322.f2)->f1;_tmp323=(_tmp322.f2)->f2;_LL307:;
({void*_tmp53A=_tmp324;Cyc_Port_unifies(_tmp53A,Cyc_Port_const_ct());});
({void*_tmp53B=_tmp323;Cyc_Port_unifies(_tmp53B,Cyc_Port_instantiate(_tmp318));});
# 1803
Cyc_Port_register_rgns(env,counts,1,_tmp325,_tmp318);}
# 1805
return env;};};};};case 8: _LL2F1: _tmp33E=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp300)->f1;_LL2F2: {
# 1811
void*_tmp329=(void*)_check_null(_tmp33E->defn);
void*_tmp32A=Cyc_Port_type_to_ctype(env,_tmp329);
(*_tmp33E->name).f1=Cyc_Absyn_Loc_n;
Cyc_Port_add_typedef(env,_tmp33E->name,_tmp329,_tmp32A);
return env;}case 5: _LL2F3: _tmp33F=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp300)->f1;_LL2F4: {
# 1821
struct _tuple0*_tmp32B=_tmp33F->name;
(*_tmp33F->name).f1=Cyc_Absyn_Loc_n;{
struct _tuple11*previous;
struct Cyc_List_List*curr=0;
{enum Cyc_Absyn_AggrKind _tmp32C=_tmp33F->kind;enum Cyc_Absyn_AggrKind _tmp32D=_tmp32C;if(_tmp32D == Cyc_Absyn_StructA){_LL309: _LL30A:
# 1827
({struct _tuple11*_tmp53C=Cyc_Port_lookup_struct_decl(env,_tmp32B);previous=_tmp53C;});
goto _LL308;}else{_LL30B: _LL30C:
# 1830
({struct _tuple11*_tmp53D=Cyc_Port_lookup_union_decl(env,_tmp32B);previous=_tmp53D;});
goto _LL308;}_LL308:;}
# 1833
if((unsigned int)_tmp33F->impl){
struct Cyc_List_List*cf=(*previous).f2;
{struct Cyc_List_List*_tmp32E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp33F->impl))->fields;for(0;(unsigned int)_tmp32E;_tmp32E=_tmp32E->tl){
struct Cyc_Absyn_Aggrfield*_tmp32F=(struct Cyc_Absyn_Aggrfield*)_tmp32E->hd;
void*qv;
if((env->gcenv)->porting){
({void*_tmp53E=Cyc_Port_var();qv=_tmp53E;});
({struct Cyc_Port_Cenv*_tmp541=env;void*_tmp540=qv;void*_tmp53F=
(_tmp32F->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();
# 1840
Cyc_Port_register_const_cvar(_tmp541,_tmp540,_tmp53F,(_tmp32F->tq).loc);});}else{
# 1849
({void*_tmp542=(_tmp32F->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();qv=_tmp542;});}{
# 1851
void*_tmp330=Cyc_Port_type_to_ctype(env,_tmp32F->type);
if(cf != 0){
struct Cyc_Port_Cfield _tmp331=*((struct Cyc_Port_Cfield*)cf->hd);struct Cyc_Port_Cfield _tmp332=_tmp331;void*_tmp334;void*_tmp333;_LL30E: _tmp334=_tmp332.qual;_tmp333=_tmp332.type;_LL30F:;
cf=cf->tl;
Cyc_Port_unifies(qv,_tmp334);
Cyc_Port_unifies(_tmp330,_tmp333);}
# 1858
({struct Cyc_List_List*_tmp544=({struct Cyc_List_List*_tmp335=_cycalloc(sizeof(*_tmp335));({struct Cyc_Port_Cfield*_tmp543=({struct Cyc_Port_Cfield*_tmp336=_cycalloc(sizeof(*_tmp336));_tmp336->qual=qv;_tmp336->f=_tmp32F->name;_tmp336->type=_tmp330;_tmp336;});_tmp335->hd=_tmp543;});_tmp335->tl=curr;_tmp335;});curr=_tmp544;});};}}
# 1860
({struct Cyc_List_List*_tmp545=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(curr);curr=_tmp545;});
if((*previous).f2 == 0)
(*previous).f2=curr;}
# 1865
return env;};}case 7: _LL2F5: _tmp340=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp300)->f1;_LL2F6:
# 1870
(*_tmp340->name).f1=Cyc_Absyn_Loc_n;
# 1872
if((unsigned int)_tmp340->fields){
struct Cyc_List_List*_tmp337=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp340->fields))->v;for(0;(unsigned int)_tmp337;_tmp337=_tmp337->tl){
(*((struct Cyc_Absyn_Enumfield*)_tmp337->hd)->name).f1=Cyc_Absyn_Loc_n;
({struct Cyc_Port_Cenv*_tmp54B=({struct Cyc_Port_Cenv*_tmp54A=env;struct _tuple0*_tmp549=((struct Cyc_Absyn_Enumfield*)_tmp337->hd)->name;void*_tmp548=(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp338=_cycalloc(sizeof(*_tmp338));({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp546=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp339;_tmp339.tag=13;_tmp339.f1=_tmp340->name;_tmp339.f2=_tmp340;_tmp339;});_tmp338[0]=_tmp546;});_tmp338;});void*_tmp547=
Cyc_Port_const_ct();
# 1875
Cyc_Port_add_var(_tmp54A,_tmp549,_tmp548,_tmp547,
Cyc_Port_arith_ct(),0);});
# 1875
env=_tmp54B;});}}
# 1878
return env;default: _LL2F7: _LL2F8:
# 1880
 if((env->gcenv)->porting)
({void*_tmp33A=0;({struct Cyc___cycFILE*_tmp54D=Cyc_stderr;struct _dyneither_ptr _tmp54C=({const char*_tmp33B="Warning: Cyclone declaration found in code to be ported -- skipping.";_tag_dyneither(_tmp33B,sizeof(char),69);});Cyc_fprintf(_tmp54D,_tmp54C,_tag_dyneither(_tmp33A,sizeof(void*),0));});});
return env;}_LL2E8:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1887
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*env,struct Cyc_List_List*ds){
for(0;(unsigned int)ds;ds=ds->tl){
({struct Cyc_Port_Cenv*_tmp54E=Cyc_Port_gen_topdecl(env,(struct Cyc_Absyn_Decl*)ds->hd);env=_tmp54E;});}
return env;}struct Cyc_Port_Cenv;struct Cyc_Port_VarUsage;struct Cyc_Port_VarUsage;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_VarUsage;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;
# 1894
static struct Cyc_List_List*Cyc_Port_gen_edits(struct Cyc_List_List*ds){
# 1896
struct Cyc_Port_Cenv*env=({struct Cyc_Port_Cenv*_tmp54F=Cyc_Port_initial_cenv();Cyc_Port_gen_topdecls(_tmp54F,ds);});
# 1898
struct Cyc_List_List*_tmp341=(env->gcenv)->pointer_edits;
struct Cyc_List_List*_tmp342=(env->gcenv)->qualifier_edits;
struct Cyc_List_List*_tmp343=(env->gcenv)->zeroterm_edits;
struct Cyc_List_List*_tmp344=(env->gcenv)->edits;
struct Cyc_List_List*_tmp345=(env->gcenv)->vardecl_locs;
# 1904
for(0;(unsigned int)_tmp345;_tmp345=_tmp345->tl){
struct _tuple15 _tmp346=*((struct _tuple15*)_tmp345->hd);struct _tuple15 _tmp347=_tmp346;struct _tuple0*_tmp362;unsigned int _tmp361;void*_tmp360;_LL311: _tmp362=_tmp347.f1;_tmp361=_tmp347.f2;_tmp360=_tmp347.f3;_LL312:;{
struct _tuple0 _tmp348=*_tmp362;struct _tuple0 _tmp349=_tmp348;struct _dyneither_ptr*_tmp35F;_LL314: _tmp35F=_tmp349.f2;_LL315:;{
struct Cyc_Port_VarUsage*_tmp34A=((struct Cyc_Port_VarUsage*(*)(struct Cyc_Hashtable_Table*t,unsigned int key))Cyc_Hashtable_lookup)((env->gcenv)->varusage_tab,_tmp361);
if(((struct Cyc_Port_VarUsage*)_check_null(_tmp34A))->address_taken){
if((unsigned int)_tmp34A->init){
# 1911
({struct Cyc_List_List*_tmp553=({struct Cyc_List_List*_tmp34B=_cycalloc(sizeof(*_tmp34B));({struct Cyc_Port_Edit*_tmp552=({struct Cyc_Port_Edit*_tmp34C=_cycalloc(sizeof(*_tmp34C));_tmp34C->loc=_tmp361;({struct _dyneither_ptr _tmp550=({const char*_tmp34E="@";_tag_dyneither(_tmp34E,sizeof(char),2);});_tmp34C->old_string=_tmp550;});({struct _dyneither_ptr _tmp551=({const char*_tmp34D="";_tag_dyneither(_tmp34D,sizeof(char),1);});_tmp34C->new_string=_tmp551;});_tmp34C;});_tmp34B->hd=_tmp552;});_tmp34B->tl=_tmp344;_tmp34B;});_tmp344=_tmp553;});
({struct Cyc_List_List*_tmp557=({struct Cyc_List_List*_tmp34F=_cycalloc(sizeof(*_tmp34F));({struct Cyc_Port_Edit*_tmp556=({struct Cyc_Port_Edit*_tmp350=_cycalloc(sizeof(*_tmp350));_tmp350->loc=((struct Cyc_Absyn_Exp*)_check_null(_tmp34A->init))->loc;({struct _dyneither_ptr _tmp554=({const char*_tmp352="new ";_tag_dyneither(_tmp352,sizeof(char),5);});_tmp350->old_string=_tmp554;});({struct _dyneither_ptr _tmp555=({const char*_tmp351="";_tag_dyneither(_tmp351,sizeof(char),1);});_tmp350->new_string=_tmp555;});_tmp350;});_tmp34F->hd=_tmp556;});_tmp34F->tl=_tmp344;_tmp34F;});_tmp344=_tmp557;});}else{
# 1916
({struct Cyc_List_List*_tmp55C=({struct Cyc_List_List*_tmp353=_cycalloc(sizeof(*_tmp353));({struct Cyc_Port_Edit*_tmp55B=({struct Cyc_Port_Edit*_tmp354=_cycalloc(sizeof(*_tmp354));_tmp354->loc=_tmp361;({struct _dyneither_ptr _tmp55A=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp358;_tmp358.tag=0;({struct _dyneither_ptr _tmp558=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp360));_tmp358.f1=_tmp558;});({struct Cyc_String_pa_PrintArg_struct _tmp357;_tmp357.tag=0;_tmp357.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp35F);({void*_tmp355[2]={& _tmp357,& _tmp358};({struct _dyneither_ptr _tmp559=({const char*_tmp356="?%s = malloc(sizeof(%s))";_tag_dyneither(_tmp356,sizeof(char),25);});Cyc_aprintf(_tmp559,_tag_dyneither(_tmp355,sizeof(void*),2));});});});});_tmp354->old_string=_tmp55A;});_tmp354->new_string=*_tmp35F;_tmp354;});_tmp353->hd=_tmp55B;});_tmp353->tl=_tmp344;_tmp353;});_tmp344=_tmp55C;});}{
# 1918
struct Cyc_List_List*_tmp359=_tmp34A->usage_locs;
for(0;(unsigned int)_tmp359;_tmp359=_tmp359->tl){
({struct Cyc_List_List*_tmp560=({struct Cyc_List_List*_tmp35A=_cycalloc(sizeof(*_tmp35A));({struct Cyc_Port_Edit*_tmp55F=({struct Cyc_Port_Edit*_tmp35B=_cycalloc(sizeof(*_tmp35B));_tmp35B->loc=(unsigned int)_tmp359->hd;({struct _dyneither_ptr _tmp55E=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp35E;_tmp35E.tag=0;_tmp35E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp35F);({void*_tmp35C[1]={& _tmp35E};({struct _dyneither_ptr _tmp55D=({const char*_tmp35D="(*%s)";_tag_dyneither(_tmp35D,sizeof(char),6);});Cyc_aprintf(_tmp55D,_tag_dyneither(_tmp35C,sizeof(void*),1));});});});_tmp35B->old_string=_tmp55E;});_tmp35B->new_string=*_tmp35F;_tmp35B;});_tmp35A->hd=_tmp55F;});_tmp35A->tl=_tmp344;_tmp35A;});_tmp344=_tmp560;});}};}};};}
# 1926
{struct Cyc_List_List*_tmp363=_tmp341;for(0;(unsigned int)_tmp363;_tmp363=_tmp363->tl){
struct _tuple16 _tmp364=*((struct _tuple16*)_tmp363->hd);struct _tuple16 _tmp365=_tmp364;void*_tmp368;void*_tmp367;unsigned int _tmp366;_LL317: _tmp368=_tmp365.f1;_tmp367=_tmp365.f2;_tmp366=_tmp365.f3;_LL318:;
Cyc_Port_unifies(_tmp368,_tmp367);}}
# 1930
{struct Cyc_List_List*_tmp369=_tmp342;for(0;(unsigned int)_tmp369;_tmp369=_tmp369->tl){
struct _tuple16 _tmp36A=*((struct _tuple16*)_tmp369->hd);struct _tuple16 _tmp36B=_tmp36A;void*_tmp36E;void*_tmp36D;unsigned int _tmp36C;_LL31A: _tmp36E=_tmp36B.f1;_tmp36D=_tmp36B.f2;_tmp36C=_tmp36B.f3;_LL31B:;
Cyc_Port_unifies(_tmp36E,_tmp36D);}}
# 1934
{struct Cyc_List_List*_tmp36F=_tmp343;for(0;(unsigned int)_tmp36F;_tmp36F=_tmp36F->tl){
struct _tuple16 _tmp370=*((struct _tuple16*)_tmp36F->hd);struct _tuple16 _tmp371=_tmp370;void*_tmp374;void*_tmp373;unsigned int _tmp372;_LL31D: _tmp374=_tmp371.f1;_tmp373=_tmp371.f2;_tmp372=_tmp371.f3;_LL31E:;
Cyc_Port_unifies(_tmp374,_tmp373);}}
# 1940
for(0;(unsigned int)_tmp341;_tmp341=_tmp341->tl){
struct _tuple16 _tmp375=*((struct _tuple16*)_tmp341->hd);struct _tuple16 _tmp376=_tmp375;void*_tmp383;void*_tmp382;unsigned int _tmp381;_LL320: _tmp383=_tmp376.f1;_tmp382=_tmp376.f2;_tmp381=_tmp376.f3;_LL321:;
if(!Cyc_Port_unifies(_tmp383,_tmp382) && (int)_tmp381){
void*_tmp377=Cyc_Port_compress_ct(_tmp382);void*_tmp378=_tmp377;switch(*((int*)_tmp378)){case 2: _LL323: _LL324:
# 1945
({struct Cyc_List_List*_tmp564=({struct Cyc_List_List*_tmp379=_cycalloc(sizeof(*_tmp379));({struct Cyc_Port_Edit*_tmp563=({struct Cyc_Port_Edit*_tmp37A=_cycalloc(sizeof(*_tmp37A));_tmp37A->loc=_tmp381;({struct _dyneither_ptr _tmp561=({const char*_tmp37C="?";_tag_dyneither(_tmp37C,sizeof(char),2);});_tmp37A->old_string=_tmp561;});({struct _dyneither_ptr _tmp562=({const char*_tmp37B="*";_tag_dyneither(_tmp37B,sizeof(char),2);});_tmp37A->new_string=_tmp562;});_tmp37A;});_tmp379->hd=_tmp563;});_tmp379->tl=_tmp344;_tmp379;});_tmp344=_tmp564;});
goto _LL322;case 3: _LL325: _LL326:
# 1948
({struct Cyc_List_List*_tmp568=({struct Cyc_List_List*_tmp37D=_cycalloc(sizeof(*_tmp37D));({struct Cyc_Port_Edit*_tmp567=({struct Cyc_Port_Edit*_tmp37E=_cycalloc(sizeof(*_tmp37E));_tmp37E->loc=_tmp381;({struct _dyneither_ptr _tmp565=({const char*_tmp380="*";_tag_dyneither(_tmp380,sizeof(char),2);});_tmp37E->old_string=_tmp565;});({struct _dyneither_ptr _tmp566=({const char*_tmp37F="?";_tag_dyneither(_tmp37F,sizeof(char),2);});_tmp37E->new_string=_tmp566;});_tmp37E;});_tmp37D->hd=_tmp567;});_tmp37D->tl=_tmp344;_tmp37D;});_tmp344=_tmp568;});
goto _LL322;default: _LL327: _LL328:
 goto _LL322;}_LL322:;}}
# 1955
for(0;(unsigned int)_tmp342;_tmp342=_tmp342->tl){
struct _tuple16 _tmp384=*((struct _tuple16*)_tmp342->hd);struct _tuple16 _tmp385=_tmp384;void*_tmp392;void*_tmp391;unsigned int _tmp390;_LL32A: _tmp392=_tmp385.f1;_tmp391=_tmp385.f2;_tmp390=_tmp385.f3;_LL32B:;
if(!Cyc_Port_unifies(_tmp392,_tmp391) && (int)_tmp390){
void*_tmp386=Cyc_Port_compress_ct(_tmp391);void*_tmp387=_tmp386;switch(*((int*)_tmp387)){case 1: _LL32D: _LL32E:
# 1960
({struct Cyc_List_List*_tmp56C=({struct Cyc_List_List*_tmp388=_cycalloc(sizeof(*_tmp388));({struct Cyc_Port_Edit*_tmp56B=({struct Cyc_Port_Edit*_tmp389=_cycalloc(sizeof(*_tmp389));_tmp389->loc=_tmp390;({struct _dyneither_ptr _tmp569=({const char*_tmp38B="const ";_tag_dyneither(_tmp38B,sizeof(char),7);});_tmp389->old_string=_tmp569;});({struct _dyneither_ptr _tmp56A=({const char*_tmp38A="";_tag_dyneither(_tmp38A,sizeof(char),1);});_tmp389->new_string=_tmp56A;});_tmp389;});_tmp388->hd=_tmp56B;});_tmp388->tl=_tmp344;_tmp388;});_tmp344=_tmp56C;});goto _LL32C;case 0: _LL32F: _LL330:
# 1962
({struct Cyc_List_List*_tmp570=({struct Cyc_List_List*_tmp38C=_cycalloc(sizeof(*_tmp38C));({struct Cyc_Port_Edit*_tmp56F=({struct Cyc_Port_Edit*_tmp38D=_cycalloc(sizeof(*_tmp38D));_tmp38D->loc=_tmp390;({struct _dyneither_ptr _tmp56D=({const char*_tmp38F="";_tag_dyneither(_tmp38F,sizeof(char),1);});_tmp38D->old_string=_tmp56D;});({struct _dyneither_ptr _tmp56E=({const char*_tmp38E="const ";_tag_dyneither(_tmp38E,sizeof(char),7);});_tmp38D->new_string=_tmp56E;});_tmp38D;});_tmp38C->hd=_tmp56F;});_tmp38C->tl=_tmp344;_tmp38C;});_tmp344=_tmp570;});goto _LL32C;default: _LL331: _LL332:
 goto _LL32C;}_LL32C:;}}
# 1968
for(0;(unsigned int)_tmp343;_tmp343=_tmp343->tl){
struct _tuple16 _tmp393=*((struct _tuple16*)_tmp343->hd);struct _tuple16 _tmp394=_tmp393;void*_tmp3A1;void*_tmp3A0;unsigned int _tmp39F;_LL334: _tmp3A1=_tmp394.f1;_tmp3A0=_tmp394.f2;_tmp39F=_tmp394.f3;_LL335:;
if(!Cyc_Port_unifies(_tmp3A1,_tmp3A0) && (int)_tmp39F){
void*_tmp395=Cyc_Port_compress_ct(_tmp3A0);void*_tmp396=_tmp395;switch(*((int*)_tmp396)){case 8: _LL337: _LL338:
# 1973
({struct Cyc_List_List*_tmp574=({struct Cyc_List_List*_tmp397=_cycalloc(sizeof(*_tmp397));({struct Cyc_Port_Edit*_tmp573=({struct Cyc_Port_Edit*_tmp398=_cycalloc(sizeof(*_tmp398));_tmp398->loc=_tmp39F;({struct _dyneither_ptr _tmp571=({const char*_tmp39A="@nozeroterm ";_tag_dyneither(_tmp39A,sizeof(char),13);});_tmp398->old_string=_tmp571;});({struct _dyneither_ptr _tmp572=({const char*_tmp399="";_tag_dyneither(_tmp399,sizeof(char),1);});_tmp398->new_string=_tmp572;});_tmp398;});_tmp397->hd=_tmp573;});_tmp397->tl=_tmp344;_tmp397;});_tmp344=_tmp574;});goto _LL336;case 9: _LL339: _LL33A:
# 1975
({struct Cyc_List_List*_tmp578=({struct Cyc_List_List*_tmp39B=_cycalloc(sizeof(*_tmp39B));({struct Cyc_Port_Edit*_tmp577=({struct Cyc_Port_Edit*_tmp39C=_cycalloc(sizeof(*_tmp39C));_tmp39C->loc=_tmp39F;({struct _dyneither_ptr _tmp575=({const char*_tmp39E="@zeroterm ";_tag_dyneither(_tmp39E,sizeof(char),11);});_tmp39C->old_string=_tmp575;});({struct _dyneither_ptr _tmp576=({const char*_tmp39D="";_tag_dyneither(_tmp39D,sizeof(char),1);});_tmp39C->new_string=_tmp576;});_tmp39C;});_tmp39B->hd=_tmp577;});_tmp39B->tl=_tmp344;_tmp39B;});_tmp344=_tmp578;});goto _LL336;default: _LL33B: _LL33C:
 goto _LL336;}_LL336:;}}
# 1982
({struct Cyc_List_List*_tmp579=((struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_Port_Edit*,struct Cyc_Port_Edit*),struct Cyc_List_List*x))Cyc_List_merge_sort)(Cyc_Port_cmp_edit,_tmp344);_tmp344=_tmp579;});
# 1984
while((unsigned int)_tmp344  && ((struct Cyc_Port_Edit*)_tmp344->hd)->loc == 0){
# 1988
_tmp344=_tmp344->tl;}
# 1990
return _tmp344;}struct Cyc_Port_Edit;struct Cyc_Port_Edit;
# 1993
static unsigned int Cyc_Port_get_seg(struct Cyc_Port_Edit*e){
return e->loc;}struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;
# 1999
void Cyc_Port_port(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp3A2=Cyc_Port_gen_edits(ds);
struct Cyc_List_List*_tmp3A3=((struct Cyc_List_List*(*)(unsigned int(*f)(struct Cyc_Port_Edit*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Port_get_seg,_tmp3A2);
Cyc_Position_use_gcc_style_location=0;{
struct Cyc_List_List*_tmp3A4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Position_strings_of_segments(_tmp3A3));
for(0;(unsigned int)_tmp3A2;(_tmp3A2=_tmp3A2->tl,_tmp3A4=_tmp3A4->tl)){
struct Cyc_Port_Edit _tmp3A5=*((struct Cyc_Port_Edit*)_tmp3A2->hd);struct Cyc_Port_Edit _tmp3A6=_tmp3A5;unsigned int _tmp3AE;struct _dyneither_ptr _tmp3AD;struct _dyneither_ptr _tmp3AC;_LL33E: _tmp3AE=_tmp3A6.loc;_tmp3AD=_tmp3A6.old_string;_tmp3AC=_tmp3A6.new_string;_LL33F:;{
struct _dyneither_ptr sloc=(struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp3A4))->hd);
({struct Cyc_String_pa_PrintArg_struct _tmp3AB;_tmp3AB.tag=0;_tmp3AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3AC);({struct Cyc_String_pa_PrintArg_struct _tmp3AA;_tmp3AA.tag=0;_tmp3AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3AD);({struct Cyc_String_pa_PrintArg_struct _tmp3A9;_tmp3A9.tag=0;_tmp3A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sloc);({void*_tmp3A7[3]={& _tmp3A9,& _tmp3AA,& _tmp3AB};({struct _dyneither_ptr _tmp57A=({const char*_tmp3A8="%s: insert `%s' for `%s'\n";_tag_dyneither(_tmp3A8,sizeof(char),26);});Cyc_printf(_tmp57A,_tag_dyneither(_tmp3A7,sizeof(void*),3));});});});});});};}};}
