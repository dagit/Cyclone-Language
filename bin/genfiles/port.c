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
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 210
struct Cyc_List_List*Cyc_List_merge_sort(int(*cmp)(void*,void*),struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
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
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 510
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 916 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 926
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 930
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 933
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 951
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 956
extern void*Cyc_Absyn_sint_typ;
# 969
void*Cyc_Absyn_string_typ(void*rgn);
# 989
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1175
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
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
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
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct _tuple10{void*f1;void*f2;};struct Cyc_Tcexp_TestEnv{struct _tuple10*eq;int isTrue;};
# 35 "tcexp.h"
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);extern char Cyc_Tcdecl_Incompatible[13U];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
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
struct Cyc_Port_Const_ct_Port_Ctype_struct Cyc_Port_Const_ct_val={0U};
struct Cyc_Port_Notconst_ct_Port_Ctype_struct Cyc_Port_Notconst_ct_val={1U};
struct Cyc_Port_Thin_ct_Port_Ctype_struct Cyc_Port_Thin_ct_val={2U};
struct Cyc_Port_Fat_ct_Port_Ctype_struct Cyc_Port_Fat_ct_val={3U};
struct Cyc_Port_Void_ct_Port_Ctype_struct Cyc_Port_Void_ct_val={4U};
struct Cyc_Port_Zero_ct_Port_Ctype_struct Cyc_Port_Zero_ct_val={5U};
struct Cyc_Port_Arith_ct_Port_Ctype_struct Cyc_Port_Arith_ct_val={6U};
struct Cyc_Port_Heap_ct_Port_Ctype_struct Cyc_Port_Heap_ct_val={7U};
struct Cyc_Port_Zterm_ct_Port_Ctype_struct Cyc_Port_Zterm_ct_val={8U};
struct Cyc_Port_Nozterm_ct_Port_Ctype_struct Cyc_Port_Nozterm_ct_val={9U};
# 164
static struct _dyneither_ptr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*ts);
static struct _dyneither_ptr Cyc_Port_cfields2string(int deep,struct Cyc_List_List*ts);struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
static struct _dyneither_ptr Cyc_Port_ctype2string(int deep,void*t){
void*_tmpA=t;struct Cyc_Port_Cvar*_tmp4E;void*_tmp4D;struct Cyc_List_List*_tmp4C;struct Cyc_List_List*_tmp4B;struct Cyc_List_List*_tmp4A;void*_tmp49;struct Cyc_Absyn_Aggrdecl*_tmp48;struct Cyc_List_List*_tmp47;void*_tmp46;void*_tmp45;void*_tmp44;void*_tmp43;void*_tmp42;void*_tmp41;void*_tmp40;void*_tmp3F;struct _dyneither_ptr*_tmp3E;switch(*((int*)_tmpA)){case 0U: _LL1: _LL2:
 return({const char*_tmpB="const";_tag_dyneither(_tmpB,sizeof(char),6U);});case 1U: _LL3: _LL4:
 return({const char*_tmpC="notconst";_tag_dyneither(_tmpC,sizeof(char),9U);});case 2U: _LL5: _LL6:
 return({const char*_tmpD="thin";_tag_dyneither(_tmpD,sizeof(char),5U);});case 3U: _LL7: _LL8:
 return({const char*_tmpE="fat";_tag_dyneither(_tmpE,sizeof(char),4U);});case 4U: _LL9: _LLA:
 return({const char*_tmpF="void";_tag_dyneither(_tmpF,sizeof(char),5U);});case 5U: _LLB: _LLC:
 return({const char*_tmp10="zero";_tag_dyneither(_tmp10,sizeof(char),5U);});case 6U: _LLD: _LLE:
 return({const char*_tmp11="arith";_tag_dyneither(_tmp11,sizeof(char),6U);});case 7U: _LLF: _LL10:
 return({const char*_tmp12="heap";_tag_dyneither(_tmp12,sizeof(char),5U);});case 8U: _LL11: _LL12:
 return({const char*_tmp13="ZT";_tag_dyneither(_tmp13,sizeof(char),3U);});case 9U: _LL13: _LL14:
 return({const char*_tmp14="NZT";_tag_dyneither(_tmp14,sizeof(char),4U);});case 10U: _LL15: _tmp3E=((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmpA)->f1;_LL16:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp17;_tmp17.tag=0U;_tmp17.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3E);({void*_tmp15[1U]={& _tmp17};({struct _dyneither_ptr _tmp3B1=({const char*_tmp16="%s";_tag_dyneither(_tmp16,sizeof(char),3U);});Cyc_aprintf(_tmp3B1,_tag_dyneither(_tmp15,sizeof(void*),1U));});});});case 11U: _LL17: _tmp43=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA)->f1;_tmp42=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA)->f2;_tmp41=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA)->f3;_tmp40=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA)->f4;_tmp3F=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA)->f5;_LL18:
# 180
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1E;_tmp1E.tag=0U;({struct _dyneither_ptr _tmp3B2=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 182
Cyc_Port_ctype2string(deep,_tmp3F));_tmp1E.f1=_tmp3B2;});({struct Cyc_String_pa_PrintArg_struct _tmp1D;_tmp1D.tag=0U;({struct _dyneither_ptr _tmp3B3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp40));_tmp1D.f1=_tmp3B3;});({struct Cyc_String_pa_PrintArg_struct _tmp1C;_tmp1C.tag=0U;({struct _dyneither_ptr _tmp3B4=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 181
Cyc_Port_ctype2string(deep,_tmp41));_tmp1C.f1=_tmp3B4;});({struct Cyc_String_pa_PrintArg_struct _tmp1B;_tmp1B.tag=0U;({struct _dyneither_ptr _tmp3B5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp42));_tmp1B.f1=_tmp3B5;});({struct Cyc_String_pa_PrintArg_struct _tmp1A;_tmp1A.tag=0U;({struct _dyneither_ptr _tmp3B6=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 180
Cyc_Port_ctype2string(deep,_tmp43));_tmp1A.f1=_tmp3B6;});({void*_tmp18[5U]={& _tmp1A,& _tmp1B,& _tmp1C,& _tmp1D,& _tmp1E};({struct _dyneither_ptr _tmp3B7=({const char*_tmp19="ptr(%s,%s,%s,%s,%s)";_tag_dyneither(_tmp19,sizeof(char),20U);});Cyc_aprintf(_tmp3B7,_tag_dyneither(_tmp18,sizeof(void*),5U));});});});});});});});case 12U: _LL19: _tmp46=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpA)->f1;_tmp45=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpA)->f2;_tmp44=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpA)->f3;_LL1A:
# 184
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp23;_tmp23.tag=0U;({struct _dyneither_ptr _tmp3B8=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Port_ctype2string(deep,_tmp44));_tmp23.f1=_tmp3B8;});({struct Cyc_String_pa_PrintArg_struct _tmp22;_tmp22.tag=0U;({struct _dyneither_ptr _tmp3B9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp45));_tmp22.f1=_tmp3B9;});({struct Cyc_String_pa_PrintArg_struct _tmp21;_tmp21.tag=0U;({struct _dyneither_ptr _tmp3BA=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 184
Cyc_Port_ctype2string(deep,_tmp46));_tmp21.f1=_tmp3BA;});({void*_tmp1F[3U]={& _tmp21,& _tmp22,& _tmp23};({struct _dyneither_ptr _tmp3BB=({const char*_tmp20="array(%s,%s,%s)";_tag_dyneither(_tmp20,sizeof(char),16U);});Cyc_aprintf(_tmp3BB,_tag_dyneither(_tmp1F,sizeof(void*),3U));});});});});});case 13U: _LL1B: _tmp48=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpA)->f1)->f1;_tmp47=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpA)->f1)->f2;_LL1C: {
# 187
struct _dyneither_ptr s=_tmp48->kind == Cyc_Absyn_StructA?({const char*_tmp2D="struct";_tag_dyneither(_tmp2D,sizeof(char),7U);}):({const char*_tmp2E="union";_tag_dyneither(_tmp2E,sizeof(char),6U);});
if(!deep)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp27;_tmp27.tag=0U;({struct _dyneither_ptr _tmp3BC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp48->name));_tmp27.f1=_tmp3BC;});({struct Cyc_String_pa_PrintArg_struct _tmp26;_tmp26.tag=0U;_tmp26.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp24[2U]={& _tmp26,& _tmp27};({struct _dyneither_ptr _tmp3BD=({const char*_tmp25="%s %s";_tag_dyneither(_tmp25,sizeof(char),6U);});Cyc_aprintf(_tmp3BD,_tag_dyneither(_tmp24,sizeof(void*),2U));});});});});else{
# 191
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2C;_tmp2C.tag=0U;({struct _dyneither_ptr _tmp3BE=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Port_cfields2string(0,_tmp47));_tmp2C.f1=_tmp3BE;});({struct Cyc_String_pa_PrintArg_struct _tmp2B;_tmp2B.tag=0U;({struct _dyneither_ptr _tmp3BF=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 191
Cyc_Absynpp_qvar2string(_tmp48->name));_tmp2B.f1=_tmp3BF;});({struct Cyc_String_pa_PrintArg_struct _tmp2A;_tmp2A.tag=0U;_tmp2A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp28[3U]={& _tmp2A,& _tmp2B,& _tmp2C};({struct _dyneither_ptr _tmp3C0=({const char*_tmp29="%s %s {%s}";_tag_dyneither(_tmp29,sizeof(char),11U);});Cyc_aprintf(_tmp3C0,_tag_dyneither(_tmp28,sizeof(void*),3U));});});});});});}}case 14U: if(((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpA)->f2 != 0){_LL1D: _tmp4A=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpA)->f1;_tmp49=*((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpA)->f2;_LL1E:
# 193
 return Cyc_Port_ctype2string(deep,_tmp49);}else{_LL1F: _tmp4B=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpA)->f1;_LL20:
# 195
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp31;_tmp31.tag=0U;({struct _dyneither_ptr _tmp3C1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_cfields2string(deep,_tmp4B));_tmp31.f1=_tmp3C1;});({void*_tmp2F[1U]={& _tmp31};({struct _dyneither_ptr _tmp3C2=({const char*_tmp30="aggr {%s}";_tag_dyneither(_tmp30,sizeof(char),10U);});Cyc_aprintf(_tmp3C2,_tag_dyneither(_tmp2F,sizeof(void*),1U));});});});}case 15U: _LL21: _tmp4D=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpA)->f1;_tmp4C=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpA)->f2;_LL22:
# 197
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp35;_tmp35.tag=0U;({struct _dyneither_ptr _tmp3C3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp4D));_tmp35.f1=_tmp3C3;});({struct Cyc_String_pa_PrintArg_struct _tmp34;_tmp34.tag=0U;({struct _dyneither_ptr _tmp3C4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctypes2string(deep,_tmp4C));_tmp34.f1=_tmp3C4;});({void*_tmp32[2U]={& _tmp34,& _tmp35};({struct _dyneither_ptr _tmp3C5=({const char*_tmp33="fn(%s)->%s";_tag_dyneither(_tmp33,sizeof(char),11U);});Cyc_aprintf(_tmp3C5,_tag_dyneither(_tmp32,sizeof(void*),2U));});});});});default: _LL23: _tmp4E=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmpA)->f1;_LL24:
# 199
 if((unsigned int)_tmp4E->eq)
return Cyc_Port_ctype2string(deep,*((void**)_check_null(_tmp4E->eq)));else{
if(!deep  || _tmp4E->uppers == 0  && _tmp4E->lowers == 0)
return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp38;_tmp38.tag=1U;_tmp38.f1=(unsigned long)_tmp4E->id;({void*_tmp36[1U]={& _tmp38};({struct _dyneither_ptr _tmp3C6=({const char*_tmp37="var(%d)";_tag_dyneither(_tmp37,sizeof(char),8U);});Cyc_aprintf(_tmp3C6,_tag_dyneither(_tmp36,sizeof(void*),1U));});});});else{
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3D;_tmp3D.tag=0U;({struct _dyneither_ptr _tmp3C7=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 206
Cyc_Port_ctypes2string(0,_tmp4E->uppers));_tmp3D.f1=_tmp3C7;});({struct Cyc_Int_pa_PrintArg_struct _tmp3C;_tmp3C.tag=1U;_tmp3C.f1=(unsigned long)_tmp4E->id;({struct Cyc_String_pa_PrintArg_struct _tmp3B;_tmp3B.tag=0U;({struct _dyneither_ptr _tmp3C8=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 204
Cyc_Port_ctypes2string(0,_tmp4E->lowers));_tmp3B.f1=_tmp3C8;});({void*_tmp39[3U]={& _tmp3B,& _tmp3C,& _tmp3D};({struct _dyneither_ptr _tmp3C9=({const char*_tmp3A="var([%s]<=%d<=[%s])";_tag_dyneither(_tmp3A,sizeof(char),20U);});Cyc_aprintf(_tmp3C9,_tag_dyneither(_tmp39,sizeof(void*),3U));});});});});});}}}_LL0:;}
# 209
static struct _dyneither_ptr*Cyc_Port_ctype2stringptr(int deep,void*t){return({struct _dyneither_ptr*_tmp4F=_cycalloc(sizeof(*_tmp4F));({struct _dyneither_ptr _tmp3CA=Cyc_Port_ctype2string(deep,t);_tmp4F[0]=_tmp3CA;});_tmp4F;});}
struct Cyc_List_List*Cyc_Port_sep(struct _dyneither_ptr s,struct Cyc_List_List*xs){
struct _dyneither_ptr*_tmp50=({struct _dyneither_ptr*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54[0]=s;_tmp54;});
if(xs == 0)return xs;{
struct Cyc_List_List*_tmp51=xs;
struct Cyc_List_List*_tmp52=xs->tl;
for(0;_tmp52 != 0;(_tmp51=_tmp52,_tmp52=_tmp52->tl)){
({struct Cyc_List_List*_tmp3CB=({struct Cyc_List_List*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->hd=_tmp50;_tmp53->tl=_tmp52;_tmp53;});_tmp51->tl=_tmp3CB;});}
# 218
return xs;};}struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
# 220
static struct _dyneither_ptr*Cyc_Port_cfield2stringptr(int deep,struct Cyc_Port_Cfield*f){
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5A;_tmp5A.tag=0U;({struct _dyneither_ptr _tmp3CC=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Port_ctype2string(deep,f->type));_tmp5A.f1=_tmp3CC;});({struct Cyc_String_pa_PrintArg_struct _tmp59;_tmp59.tag=0U;_tmp59.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f->f);({struct Cyc_String_pa_PrintArg_struct _tmp58;_tmp58.tag=0U;({struct _dyneither_ptr _tmp3CD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,f->qual));_tmp58.f1=_tmp3CD;});({void*_tmp56[3U]={& _tmp58,& _tmp59,& _tmp5A};({struct _dyneither_ptr _tmp3CE=({const char*_tmp57="%s %s: %s";_tag_dyneither(_tmp57,sizeof(char),10U);});Cyc_aprintf(_tmp3CE,_tag_dyneither(_tmp56,sizeof(void*),3U));});});});});});
return({struct _dyneither_ptr*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55[0]=s;_tmp55;});}
# 225
static struct _dyneither_ptr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*ts){
return(struct _dyneither_ptr)Cyc_strconcat_l(({struct _dyneither_ptr _tmp3CF=({const char*_tmp5B=",";_tag_dyneither(_tmp5B,sizeof(char),2U);});Cyc_Port_sep(_tmp3CF,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_ctype2stringptr,deep,ts));}));}struct Cyc_Port_Cfield;
# 228
static struct _dyneither_ptr Cyc_Port_cfields2string(int deep,struct Cyc_List_List*fs){
return(struct _dyneither_ptr)Cyc_strconcat_l(({struct _dyneither_ptr _tmp3D0=({const char*_tmp5C=";";_tag_dyneither(_tmp5C,sizeof(char),2U);});Cyc_Port_sep(_tmp3D0,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(int,struct Cyc_Port_Cfield*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_cfield2stringptr,deep,fs));}));}
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
static void*Cyc_Port_rgnvar_ct(struct _dyneither_ptr*n){return(void*)({struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmp5D=_cycalloc(sizeof(*_tmp5D));({struct Cyc_Port_RgnVar_ct_Port_Ctype_struct _tmp3D1=({struct Cyc_Port_RgnVar_ct_Port_Ctype_struct _tmp5E;_tmp5E.tag=10U;_tmp5E.f1=n;_tmp5E;});_tmp5D[0]=_tmp3D1;});_tmp5D;});}
static void*Cyc_Port_unknown_aggr_ct(struct Cyc_List_List*fs){
return(void*)({struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));({struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct _tmp3D2=({struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct _tmp60;_tmp60.tag=14U;_tmp60.f1=fs;_tmp60.f2=0;_tmp60;});_tmp5F[0]=_tmp3D2;});_tmp5F;});}
# 248
static void*Cyc_Port_known_aggr_ct(struct _tuple11*p){
return(void*)({struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmp61=_cycalloc(sizeof(*_tmp61));({struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct _tmp3D3=({struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct _tmp62;_tmp62.tag=13U;_tmp62.f1=p;_tmp62;});_tmp61[0]=_tmp3D3;});_tmp61;});}
# 251
static void*Cyc_Port_ptr_ct(void*elt,void*qual,void*ptr_kind,void*r,void*zt){
# 253
return(void*)({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp63=_cycalloc(sizeof(*_tmp63));({struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmp3D4=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmp64;_tmp64.tag=11U;_tmp64.f1=elt;_tmp64.f2=qual;_tmp64.f3=ptr_kind;_tmp64.f4=r;_tmp64.f5=zt;_tmp64;});_tmp63[0]=_tmp3D4;});_tmp63;});}
# 255
static void*Cyc_Port_array_ct(void*elt,void*qual,void*zt){
return(void*)({struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp65=_cycalloc(sizeof(*_tmp65));({struct Cyc_Port_Array_ct_Port_Ctype_struct _tmp3D5=({struct Cyc_Port_Array_ct_Port_Ctype_struct _tmp66;_tmp66.tag=12U;_tmp66.f1=elt;_tmp66.f2=qual;_tmp66.f3=zt;_tmp66;});_tmp65[0]=_tmp3D5;});_tmp65;});}
# 258
static void*Cyc_Port_fn_ct(void*return_type,struct Cyc_List_List*args){
return(void*)({struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp67=_cycalloc(sizeof(*_tmp67));({struct Cyc_Port_Fn_ct_Port_Ctype_struct _tmp3D6=({struct Cyc_Port_Fn_ct_Port_Ctype_struct _tmp68;_tmp68.tag=15U;_tmp68.f1=return_type;_tmp68.f2=args;_tmp68;});_tmp67[0]=_tmp3D6;});_tmp67;});}struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
# 261
static void*Cyc_Port_var(){
static int counter=0;
return(void*)({struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmp69=_cycalloc(sizeof(*_tmp69));({struct Cyc_Port_Var_ct_Port_Ctype_struct _tmp3D8=({struct Cyc_Port_Var_ct_Port_Ctype_struct _tmp6A;_tmp6A.tag=16U;({struct Cyc_Port_Cvar*_tmp3D7=({struct Cyc_Port_Cvar*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->id=counter ++;_tmp6B->eq=0;_tmp6B->uppers=0;_tmp6B->lowers=0;_tmp6B;});_tmp6A.f1=_tmp3D7;});_tmp6A;});_tmp69[0]=_tmp3D8;});_tmp69;});}
# 265
static void*Cyc_Port_new_var(void*x){
return Cyc_Port_var();}
# 268
static struct _dyneither_ptr*Cyc_Port_new_region_var(){
static int counter=0;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp6F;_tmp6F.tag=1U;_tmp6F.f1=(unsigned long)counter ++;({void*_tmp6D[1U]={& _tmp6F};({struct _dyneither_ptr _tmp3D9=({const char*_tmp6E="`R%d";_tag_dyneither(_tmp6E,sizeof(char),5U);});Cyc_aprintf(_tmp3D9,_tag_dyneither(_tmp6D,sizeof(void*),1U));});});});
return({struct _dyneither_ptr*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C[0]=s;_tmp6C;});}
# 276
static int Cyc_Port_unifies(void*t1,void*t2);struct Cyc_Port_Cvar;
# 278
static void*Cyc_Port_compress_ct(void*t){
void*_tmp70=t;void**_tmp7C;void***_tmp7B;struct Cyc_List_List*_tmp7A;struct Cyc_List_List*_tmp79;switch(*((int*)_tmp70)){case 16U: _LL26: _tmp7B=(void***)&(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp70)->f1)->eq;_tmp7A=(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp70)->f1)->uppers;_tmp79=(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp70)->f1)->lowers;_LL27: {
# 281
void**_tmp71=*_tmp7B;
if((unsigned int)_tmp71){
# 285
void*r=Cyc_Port_compress_ct(*_tmp71);
if(*_tmp71 != r)({void**_tmp3DA=({void**_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72[0]=r;_tmp72;});*_tmp7B=_tmp3DA;});
return r;}
# 292
for(0;_tmp79 != 0;_tmp79=_tmp79->tl){
void*_tmp73=(void*)_tmp79->hd;void*_tmp74=_tmp73;switch(*((int*)_tmp74)){case 0U: _LL2D: _LL2E:
 goto _LL30;case 9U: _LL2F: _LL30:
 goto _LL32;case 4U: _LL31: _LL32:
 goto _LL34;case 13U: _LL33: _LL34:
 goto _LL36;case 15U: _LL35: _LL36:
# 299
({void**_tmp3DB=({void**_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75[0]=(void*)_tmp79->hd;_tmp75;});*_tmp7B=_tmp3DB;});
return(void*)_tmp79->hd;default: _LL37: _LL38:
# 302
 goto _LL2C;}_LL2C:;}
# 305
for(0;_tmp7A != 0;_tmp7A=_tmp7A->tl){
void*_tmp76=(void*)_tmp7A->hd;void*_tmp77=_tmp76;switch(*((int*)_tmp77)){case 1U: _LL3A: _LL3B:
 goto _LL3D;case 8U: _LL3C: _LL3D:
 goto _LL3F;case 5U: _LL3E: _LL3F:
 goto _LL41;case 13U: _LL40: _LL41:
 goto _LL43;case 15U: _LL42: _LL43:
# 312
({void**_tmp3DC=({void**_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78[0]=(void*)_tmp7A->hd;_tmp78;});*_tmp7B=_tmp3DC;});
return(void*)_tmp7A->hd;default: _LL44: _LL45:
# 315
 goto _LL39;}_LL39:;}
# 318
return t;}case 14U: _LL28: _tmp7C=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp70)->f2;_LL29:
# 321
 if((unsigned int)_tmp7C)return Cyc_Port_compress_ct(*_tmp7C);else{
return t;}default: _LL2A: _LL2B:
# 325
 return t;}_LL25:;}
# 331
static void*Cyc_Port_inst(struct Cyc_Dict_Dict*instenv,void*t){
void*_tmp7D=Cyc_Port_compress_ct(t);void*_tmp7E=_tmp7D;void*_tmp95;struct Cyc_List_List*_tmp94;void*_tmp93;void*_tmp92;void*_tmp91;void*_tmp90;void*_tmp8F;void*_tmp8E;void*_tmp8D;void*_tmp8C;struct _dyneither_ptr*_tmp8B;switch(*((int*)_tmp7E)){case 0U: _LL47: _LL48:
 goto _LL4A;case 1U: _LL49: _LL4A:
 goto _LL4C;case 2U: _LL4B: _LL4C:
 goto _LL4E;case 3U: _LL4D: _LL4E:
 goto _LL50;case 4U: _LL4F: _LL50:
 goto _LL52;case 5U: _LL51: _LL52:
 goto _LL54;case 6U: _LL53: _LL54:
 goto _LL56;case 8U: _LL55: _LL56:
 goto _LL58;case 9U: _LL57: _LL58:
 goto _LL5A;case 14U: _LL59: _LL5A:
 goto _LL5C;case 13U: _LL5B: _LL5C:
 goto _LL5E;case 16U: _LL5D: _LL5E:
 goto _LL60;case 7U: _LL5F: _LL60:
 return t;case 10U: _LL61: _tmp8B=((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp7E)->f1;_LL62:
# 347
 if(!((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*instenv,_tmp8B))
({struct Cyc_Dict_Dict _tmp3DF=({struct Cyc_Dict_Dict _tmp3DE=*instenv;struct _dyneither_ptr*_tmp3DD=_tmp8B;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp3DE,_tmp3DD,Cyc_Port_var());});*instenv=_tmp3DF;});
return((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*instenv,_tmp8B);case 11U: _LL63: _tmp90=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp7E)->f1;_tmp8F=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp7E)->f2;_tmp8E=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp7E)->f3;_tmp8D=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp7E)->f4;_tmp8C=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp7E)->f5;_LL64: {
# 351
struct _tuple10 _tmp7F=({struct _tuple10 _tmp85;({void*_tmp3E0=Cyc_Port_inst(instenv,_tmp90);_tmp85.f1=_tmp3E0;});({void*_tmp3E1=Cyc_Port_inst(instenv,_tmp8D);_tmp85.f2=_tmp3E1;});_tmp85;});struct _tuple10 _tmp80=_tmp7F;void*_tmp84;void*_tmp83;_LL6A: _tmp84=_tmp80.f1;_tmp83=_tmp80.f2;_LL6B:;
if(_tmp84 == _tmp90  && _tmp83 == _tmp8D)return t;
return(void*)({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp81=_cycalloc(sizeof(*_tmp81));({struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmp3E2=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmp82;_tmp82.tag=11U;_tmp82.f1=_tmp84;_tmp82.f2=_tmp8F;_tmp82.f3=_tmp8E;_tmp82.f4=_tmp83;_tmp82.f5=_tmp8C;_tmp82;});_tmp81[0]=_tmp3E2;});_tmp81;});}case 12U: _LL65: _tmp93=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp7E)->f1;_tmp92=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp7E)->f2;_tmp91=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp7E)->f3;_LL66: {
# 355
void*_tmp86=Cyc_Port_inst(instenv,_tmp93);
if(_tmp86 == _tmp93)return t;
return(void*)({struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp87=_cycalloc(sizeof(*_tmp87));({struct Cyc_Port_Array_ct_Port_Ctype_struct _tmp3E3=({struct Cyc_Port_Array_ct_Port_Ctype_struct _tmp88;_tmp88.tag=12U;_tmp88.f1=_tmp86;_tmp88.f2=_tmp92;_tmp88.f3=_tmp91;_tmp88;});_tmp87[0]=_tmp3E3;});_tmp87;});}default: _LL67: _tmp95=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp7E)->f1;_tmp94=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp7E)->f2;_LL68:
# 359
 return(void*)({struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp89=_cycalloc(sizeof(*_tmp89));({struct Cyc_Port_Fn_ct_Port_Ctype_struct _tmp3E6=({struct Cyc_Port_Fn_ct_Port_Ctype_struct _tmp8A;_tmp8A.tag=15U;({void*_tmp3E4=Cyc_Port_inst(instenv,_tmp95);_tmp8A.f1=_tmp3E4;});({struct Cyc_List_List*_tmp3E5=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Dict_Dict*,void*),struct Cyc_Dict_Dict*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_inst,instenv,_tmp94);_tmp8A.f2=_tmp3E5;});_tmp8A;});_tmp89[0]=_tmp3E6;});_tmp89;});}_LL46:;}
# 363
void*Cyc_Port_instantiate(void*t){
return({struct Cyc_Dict_Dict*_tmp3E8=({struct Cyc_Dict_Dict*_tmp96=_cycalloc(sizeof(*_tmp96));({struct Cyc_Dict_Dict _tmp3E7=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp96[0]=_tmp3E7;});_tmp96;});Cyc_Port_inst(_tmp3E8,t);});}
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
if(({void*_tmp3E9=t;_tmp3E9 == Cyc_Port_compress_ct((void*)ts->hd);}))continue;
({struct Cyc_List_List*_tmp3EA=({struct Cyc_List_List*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->hd=(void*)ts->hd;_tmp99->tl=res;_tmp99;});res=_tmp3EA;});}
# 382
return res;};}struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
# 387
void Cyc_Port_generalize(int is_rgn,void*t){
({void*_tmp3EB=Cyc_Port_compress_ct(t);t=_tmp3EB;});{
void*_tmp9A=t;void*_tmpA5;struct Cyc_List_List*_tmpA4;void*_tmpA3;void*_tmpA2;void*_tmpA1;void*_tmpA0;void*_tmp9F;void*_tmp9E;void*_tmp9D;void*_tmp9C;struct Cyc_Port_Cvar*_tmp9B;switch(*((int*)_tmp9A)){case 16U: _LL6D: _tmp9B=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp9A)->f1;_LL6E:
# 392
({struct Cyc_List_List*_tmp3EC=Cyc_Port_filter_self(t,_tmp9B->uppers);_tmp9B->uppers=_tmp3EC;});
({struct Cyc_List_List*_tmp3ED=Cyc_Port_filter_self(t,_tmp9B->lowers);_tmp9B->lowers=_tmp3ED;});
if(is_rgn){
# 397
if(_tmp9B->uppers == 0  && _tmp9B->lowers == 0){
({void*_tmp3EE=t;Cyc_Port_unifies(_tmp3EE,Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var()));});
return;}
# 402
if((unsigned int)_tmp9B->uppers){
Cyc_Port_unifies(t,(void*)((struct Cyc_List_List*)_check_null(_tmp9B->uppers))->hd);
Cyc_Port_generalize(1,t);}else{
# 406
Cyc_Port_unifies(t,(void*)((struct Cyc_List_List*)_check_null(_tmp9B->lowers))->hd);
Cyc_Port_generalize(1,t);}}
# 410
return;case 0U: _LL6F: _LL70:
 goto _LL72;case 1U: _LL71: _LL72:
 goto _LL74;case 2U: _LL73: _LL74:
 goto _LL76;case 3U: _LL75: _LL76:
 goto _LL78;case 4U: _LL77: _LL78:
 goto _LL7A;case 5U: _LL79: _LL7A:
 goto _LL7C;case 6U: _LL7B: _LL7C:
 goto _LL7E;case 14U: _LL7D: _LL7E:
 goto _LL80;case 13U: _LL7F: _LL80:
 goto _LL82;case 10U: _LL81: _LL82:
 goto _LL84;case 9U: _LL83: _LL84:
 goto _LL86;case 8U: _LL85: _LL86:
 goto _LL88;case 7U: _LL87: _LL88:
 return;case 11U: _LL89: _tmpA0=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9A)->f1;_tmp9F=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9A)->f2;_tmp9E=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9A)->f3;_tmp9D=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9A)->f4;_tmp9C=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9A)->f5;_LL8A:
# 427
 Cyc_Port_generalize(0,_tmpA0);Cyc_Port_generalize(1,_tmp9D);goto _LL6C;case 12U: _LL8B: _tmpA3=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9A)->f1;_tmpA2=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9A)->f2;_tmpA1=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9A)->f3;_LL8C:
# 429
 Cyc_Port_generalize(0,_tmpA3);Cyc_Port_generalize(0,_tmpA2);goto _LL6C;default: _LL8D: _tmpA5=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp9A)->f1;_tmpA4=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp9A)->f2;_LL8E:
# 431
 Cyc_Port_generalize(0,_tmpA5);((void(*)(void(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Port_generalize,0,_tmpA4);goto _LL6C;}_LL6C:;};}struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
# 437
static int Cyc_Port_occurs(void*v,void*t){
({void*_tmp3EF=Cyc_Port_compress_ct(t);t=_tmp3EF;});
if(t == v)return 1;{
void*_tmpA6=t;struct Cyc_List_List*_tmpB2;struct Cyc_List_List*_tmpB1;void*_tmpB0;struct Cyc_List_List*_tmpAF;void*_tmpAE;void*_tmpAD;void*_tmpAC;void*_tmpAB;void*_tmpAA;void*_tmpA9;void*_tmpA8;void*_tmpA7;switch(*((int*)_tmpA6)){case 11U: _LL90: _tmpAB=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA6)->f1;_tmpAA=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA6)->f2;_tmpA9=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA6)->f3;_tmpA8=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA6)->f4;_tmpA7=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA6)->f5;_LL91:
# 442
 return(((Cyc_Port_occurs(v,_tmpAB) || Cyc_Port_occurs(v,_tmpAA)) || Cyc_Port_occurs(v,_tmpA9)) || Cyc_Port_occurs(v,_tmpA8)) || 
Cyc_Port_occurs(v,_tmpA7);case 12U: _LL92: _tmpAE=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpA6)->f1;_tmpAD=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpA6)->f2;_tmpAC=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpA6)->f3;_LL93:
# 445
 return(Cyc_Port_occurs(v,_tmpAE) || Cyc_Port_occurs(v,_tmpAD)) || Cyc_Port_occurs(v,_tmpAC);case 15U: _LL94: _tmpB0=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpA6)->f1;_tmpAF=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpA6)->f2;_LL95:
# 447
 if(Cyc_Port_occurs(v,_tmpB0))return 1;
for(0;(unsigned int)_tmpAF;_tmpAF=_tmpAF->tl){
if(Cyc_Port_occurs(v,(void*)_tmpAF->hd))return 1;}
return 0;case 13U: _LL96: _tmpB1=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpA6)->f1)->f2;_LL97:
 return 0;case 14U: _LL98: _tmpB2=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpA6)->f1;_LL99:
# 453
 for(0;(unsigned int)_tmpB2;_tmpB2=_tmpB2->tl){
if(Cyc_Port_occurs(v,((struct Cyc_Port_Cfield*)_tmpB2->hd)->qual) || Cyc_Port_occurs(v,((struct Cyc_Port_Cfield*)_tmpB2->hd)->type))return 1;}
return 0;default: _LL9A: _LL9B:
 return 0;}_LL8F:;};}char Cyc_Port_Unify_ct[9U]="Unify_ct";struct Cyc_Port_Unify_ct_exn_struct{char*tag;};
# 465
struct Cyc_Port_Unify_ct_exn_struct Cyc_Port_Unify_ct_val={Cyc_Port_Unify_ct};
# 467
static int Cyc_Port_leq(void*t1,void*t2);
static void Cyc_Port_unify_cts(struct Cyc_List_List*t1,struct Cyc_List_List*t2);
static struct Cyc_List_List*Cyc_Port_merge_fields(struct Cyc_List_List*fs1,struct Cyc_List_List*fs2,int allow_subset);struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;struct Cyc_Port_Cvar;
# 471
static void Cyc_Port_unify_ct(void*t1,void*t2){
({void*_tmp3F0=Cyc_Port_compress_ct(t1);t1=_tmp3F0;});
({void*_tmp3F1=Cyc_Port_compress_ct(t2);t2=_tmp3F1;});
if(t1 == t2)return;{
struct _tuple10 _tmpB4=({struct _tuple10 _tmpE3;_tmpE3.f1=t1;_tmpE3.f2=t2;_tmpE3;});struct _tuple10 _tmpB5=_tmpB4;struct Cyc_List_List*_tmpE2;void***_tmpE1;struct Cyc_Absyn_Aggrdecl*_tmpE0;struct Cyc_List_List*_tmpDF;struct Cyc_List_List*_tmpDE;void***_tmpDD;struct Cyc_List_List*_tmpDC;void***_tmpDB;struct Cyc_Absyn_Aggrdecl*_tmpDA;struct Cyc_List_List*_tmpD9;struct Cyc_List_List*_tmpD8;void***_tmpD7;struct _tuple11*_tmpD6;struct _tuple11*_tmpD5;void*_tmpD4;struct Cyc_List_List*_tmpD3;void*_tmpD2;struct Cyc_List_List*_tmpD1;void*_tmpD0;void*_tmpCF;void*_tmpCE;void*_tmpCD;void*_tmpCC;void*_tmpCB;struct _dyneither_ptr _tmpCA;struct _dyneither_ptr _tmpC9;void*_tmpC8;void*_tmpC7;void*_tmpC6;void*_tmpC5;void*_tmpC4;void*_tmpC3;void*_tmpC2;void*_tmpC1;void*_tmpC0;void*_tmpBF;struct Cyc_Port_Cvar*_tmpBE;struct Cyc_Port_Cvar*_tmpBD;if(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmpB5.f1)->tag == 16U){_LL9D: _tmpBD=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmpB5.f1)->f1;_LL9E:
# 477
 if(!Cyc_Port_occurs(t1,t2)){
# 480
{struct Cyc_List_List*_tmpB6=Cyc_Port_filter_self(t1,_tmpBD->uppers);for(0;(unsigned int)_tmpB6;_tmpB6=_tmpB6->tl){
if(!Cyc_Port_leq(t2,(void*)_tmpB6->hd))(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}
{struct Cyc_List_List*_tmpB7=Cyc_Port_filter_self(t1,_tmpBD->lowers);for(0;(unsigned int)_tmpB7;_tmpB7=_tmpB7->tl){
if(!Cyc_Port_leq((void*)_tmpB7->hd,t2))(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}
({void**_tmp3F2=({void**_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8[0]=t2;_tmpB8;});_tmpBD->eq=_tmp3F2;});
return;}else{
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}else{if(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmpB5.f2)->tag == 16U){_LL9F: _tmpBE=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmpB5.f2)->f1;_LLA0:
 Cyc_Port_unify_ct(t2,t1);return;}else{switch(*((int*)_tmpB5.f1)){case 11U: if(((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f2)->tag == 11U){_LLA1: _tmpC8=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f1)->f1;_tmpC7=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f1)->f2;_tmpC6=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f1)->f3;_tmpC5=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f1)->f4;_tmpC4=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f1)->f5;_tmpC3=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f2)->f1;_tmpC2=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f2)->f2;_tmpC1=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f2)->f3;_tmpC0=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f2)->f4;_tmpBF=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpB5.f2)->f5;_LLA2:
# 489
 Cyc_Port_unify_ct(_tmpC8,_tmpC3);Cyc_Port_unify_ct(_tmpC7,_tmpC2);Cyc_Port_unify_ct(_tmpC6,_tmpC1);Cyc_Port_unify_ct(_tmpC5,_tmpC0);
Cyc_Port_unify_ct(_tmpC4,_tmpBF);
return;}else{goto _LLB1;}case 10U: if(((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmpB5.f2)->tag == 10U){_LLA3: _tmpCA=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmpB5.f1)->f1;_tmpC9=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmpB5.f2)->f1;_LLA4:
# 493
 if(Cyc_strcmp((struct _dyneither_ptr)_tmpCA,(struct _dyneither_ptr)_tmpC9)!= 0)(int)_throw((void*)& Cyc_Port_Unify_ct_val);
return;}else{goto _LLB1;}case 12U: if(((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpB5.f2)->tag == 12U){_LLA5: _tmpD0=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpB5.f1)->f1;_tmpCF=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpB5.f1)->f2;_tmpCE=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpB5.f1)->f3;_tmpCD=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpB5.f2)->f1;_tmpCC=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpB5.f2)->f2;_tmpCB=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpB5.f2)->f3;_LLA6:
# 496
 Cyc_Port_unify_ct(_tmpD0,_tmpCD);Cyc_Port_unify_ct(_tmpCF,_tmpCC);Cyc_Port_unify_ct(_tmpCE,_tmpCB);return;}else{goto _LLB1;}case 15U: if(((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpB5.f2)->tag == 15U){_LLA7: _tmpD4=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpB5.f1)->f1;_tmpD3=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpB5.f1)->f2;_tmpD2=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpB5.f2)->f1;_tmpD1=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmpB5.f2)->f2;_LLA8:
# 498
 Cyc_Port_unify_ct(_tmpD4,_tmpD2);Cyc_Port_unify_cts(_tmpD3,_tmpD1);return;}else{goto _LLB1;}case 13U: switch(*((int*)_tmpB5.f2)){case 13U: _LLA9: _tmpD6=((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpB5.f1)->f1;_tmpD5=((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpB5.f2)->f1;_LLAA:
# 500
 if(_tmpD6 == _tmpD5)return;else{(int)_throw((void*)& Cyc_Port_Unify_ct_val);}case 14U: _LLAF: _tmpDA=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpB5.f1)->f1)->f1;_tmpD9=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpB5.f1)->f1)->f2;_tmpD8=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpB5.f2)->f1;_tmpD7=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpB5.f2)->f2;_LLB0:
# 510
 Cyc_Port_merge_fields(_tmpD9,_tmpD8,0);
({void**_tmp3F3=({void**_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=t1;_tmpBC;});*_tmpD7=_tmp3F3;});
return;default: goto _LLB1;}case 14U: switch(*((int*)_tmpB5.f2)){case 14U: _LLAB: _tmpDE=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpB5.f1)->f1;_tmpDD=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpB5.f1)->f2;_tmpDC=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpB5.f2)->f1;_tmpDB=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpB5.f2)->f2;_LLAC: {
# 502
void*_tmpB9=Cyc_Port_unknown_aggr_ct(Cyc_Port_merge_fields(_tmpDE,_tmpDC,1));
({void**_tmp3F5=({void**_tmp3F4=({void**_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA[0]=_tmpB9;_tmpBA;});*_tmpDB=_tmp3F4;});*_tmpDD=_tmp3F5;});
return;}case 13U: _LLAD: _tmpE2=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpB5.f1)->f1;_tmpE1=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmpB5.f1)->f2;_tmpE0=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpB5.f2)->f1)->f1;_tmpDF=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmpB5.f2)->f1)->f2;_LLAE:
# 506
 Cyc_Port_merge_fields(_tmpDF,_tmpE2,0);
({void**_tmp3F6=({void**_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB[0]=t2;_tmpBB;});*_tmpE1=_tmp3F6;});
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
if(Cyc_strptrcmp(_tmpE7->f,_tmpE5->f)== 0){
({struct Cyc_List_List*_tmp3F7=({struct Cyc_List_List*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->hd=_tmpE7;_tmpE8->tl=common;_tmpE8;});common=_tmp3F7;});
Cyc_Port_unify_ct(_tmpE7->qual,_tmpE5->qual);
Cyc_Port_unify_ct(_tmpE7->type,_tmpE5->type);
found=1;
break;}}}
# 543
if(!found){
if(allow_f1_subset_f2)
({struct Cyc_List_List*_tmp3F8=({struct Cyc_List_List*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->hd=_tmpE5;_tmpE9->tl=common;_tmpE9;});common=_tmp3F8;});else{
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
({struct Cyc_List_List*_tmp3F9=({struct Cyc_List_List*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE->hd=_tmpEB;_tmpEE->tl=common;_tmpEE;});common=_tmp3F9;});}}
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
({void*_tmp3FA=Cyc_Port_compress_ct(t);t=_tmp3FA;});
{void*_tmpF4=t;switch(*((int*)_tmpF4)){case 1U: _LLB9: _LLBA:
# 590
 goto _LLBC;case 8U: _LLBB: _LLBC:
 goto _LLBE;case 5U: _LLBD: _LLBE:
 goto _LLC0;case 2U: _LLBF: _LLC0:
 goto _LLC2;case 3U: _LLC1: _LLC2:
 goto _LLC4;case 12U: _LLC3: _LLC4:
 goto _LLC6;case 13U: _LLC5: _LLC6:
 goto _LLC8;case 15U: _LLC7: _LLC8:
 goto _LLCA;case 7U: _LLC9: _LLCA:
# 601
*uppers=0;
Cyc_Port_unifies(v,t);
return*uppers;case 4U: _LLCB: _LLCC:
# 606
 goto _LLCE;case 0U: _LLCD: _LLCE:
 goto _LLD0;case 9U: _LLCF: _LLD0:
# 609
 return*uppers;default: _LLD1: _LLD2:
 goto _LLB8;}_LLB8:;}
# 613
{struct Cyc_List_List*_tmpF5=*uppers;for(0;(unsigned int)_tmpF5;_tmpF5=_tmpF5->tl){
void*_tmpF6=Cyc_Port_compress_ct((void*)_tmpF5->hd);
# 616
if(t == _tmpF6)return*uppers;{
struct _tuple10 _tmpF7=({struct _tuple10 _tmp10E;_tmp10E.f1=t;_tmp10E.f2=_tmpF6;_tmp10E;});struct _tuple10 _tmpF8=_tmpF7;void*_tmp10D;void*_tmp10C;void*_tmp10B;void*_tmp10A;void*_tmp109;void*_tmp108;void*_tmp107;void*_tmp106;void*_tmp105;void*_tmp104;switch(*((int*)_tmpF8.f1)){case 6U: switch(*((int*)_tmpF8.f2)){case 11U: _LLD4: _LLD5:
# 621
 goto _LLD7;case 5U: _LLD6: _LLD7:
 goto _LLD9;case 12U: _LLD8: _LLD9:
# 624
 return*uppers;default: goto _LLDC;}case 11U: if(((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f2)->tag == 11U){_LLDA: _tmp10D=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f1)->f1;_tmp10C=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f1)->f2;_tmp10B=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f1)->f3;_tmp10A=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f1)->f4;_tmp109=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f1)->f5;_tmp108=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f2)->f1;_tmp107=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f2)->f2;_tmp106=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f2)->f3;_tmp105=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f2)->f4;_tmp104=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpF8.f2)->f5;_LLDB: {
# 629
struct _tuple12 _tmpF9=({struct _tuple12 _tmp103;({void*_tmp3FB=Cyc_Port_var();_tmp103.f1=_tmp3FB;});({void*_tmp3FC=Cyc_Port_var();_tmp103.f2=_tmp3FC;});({void*_tmp3FD=Cyc_Port_var();_tmp103.f3=_tmp3FD;});({void*_tmp3FE=Cyc_Port_var();_tmp103.f4=_tmp3FE;});({void*_tmp3FF=Cyc_Port_var();_tmp103.f5=_tmp3FF;});_tmp103;});struct _tuple12 _tmpFA=_tmpF9;void*_tmp102;void*_tmp101;void*_tmp100;void*_tmpFF;void*_tmpFE;_LLDF: _tmp102=_tmpFA.f1;_tmp101=_tmpFA.f2;_tmp100=_tmpFA.f3;_tmpFF=_tmpFA.f4;_tmpFE=_tmpFA.f5;_LLE0:;{
struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpFB=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpFC=_cycalloc(sizeof(*_tmpFC));({struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmp400=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmpFD;_tmpFD.tag=11U;_tmpFD.f1=_tmp102;_tmpFD.f2=_tmp101;_tmpFD.f3=_tmp100;_tmpFD.f4=_tmpFF;_tmpFD.f5=_tmpFE;_tmpFD;});_tmpFC[0]=_tmp400;});_tmpFC;});
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
({void*_tmp401=Cyc_Port_compress_ct(t);t=_tmp401;});
{void*_tmp110=t;switch(*((int*)_tmp110)){case 0U: _LLE2: _LLE3:
 goto _LLE5;case 8U: _LLE4: _LLE5:
 goto _LLE7;case 2U: _LLE6: _LLE7:
 goto _LLE9;case 3U: _LLE8: _LLE9:
 goto _LLEB;case 4U: _LLEA: _LLEB:
 goto _LLED;case 13U: _LLEC: _LLED:
 goto _LLEF;case 15U: _LLEE: _LLEF:
 goto _LLF1;case 10U: _LLF0: _LLF1:
# 658
*lowers=0;
Cyc_Port_unifies(v,t);
return*lowers;case 7U: _LLF2: _LLF3:
 goto _LLF5;case 1U: _LLF4: _LLF5:
 goto _LLF7;case 9U: _LLF6: _LLF7:
# 664
 return*lowers;default: _LLF8: _LLF9:
# 666
 goto _LLE1;}_LLE1:;}
# 668
{struct Cyc_List_List*_tmp111=*lowers;for(0;(unsigned int)_tmp111;_tmp111=_tmp111->tl){
void*_tmp112=Cyc_Port_compress_ct((void*)_tmp111->hd);
if(t == _tmp112)return*lowers;{
struct _tuple10 _tmp113=({struct _tuple10 _tmp12A;_tmp12A.f1=t;_tmp12A.f2=_tmp112;_tmp12A;});struct _tuple10 _tmp114=_tmp113;void*_tmp129;void*_tmp128;void*_tmp127;void*_tmp126;void*_tmp125;void*_tmp124;void*_tmp123;void*_tmp122;void*_tmp121;void*_tmp120;if(((struct Cyc_Port_Void_ct_Port_Ctype_struct*)_tmp114.f2)->tag == 4U){_LLFB: _LLFC:
 goto _LLFE;}else{switch(*((int*)_tmp114.f1)){case 5U: switch(*((int*)_tmp114.f2)){case 6U: _LLFD: _LLFE:
 goto _LL100;case 11U: _LLFF: _LL100:
 goto _LL102;default: goto _LL107;}case 11U: switch(*((int*)_tmp114.f2)){case 6U: _LL101: _LL102:
 goto _LL104;case 11U: _LL105: _tmp129=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f1)->f1;_tmp128=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f1)->f2;_tmp127=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f1)->f3;_tmp126=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f1)->f4;_tmp125=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f1)->f5;_tmp124=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f2)->f1;_tmp123=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f2)->f2;_tmp122=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f2)->f3;_tmp121=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f2)->f4;_tmp120=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp114.f2)->f5;_LL106: {
# 682
struct _tuple12 _tmp115=({struct _tuple12 _tmp11F;({void*_tmp402=Cyc_Port_var();_tmp11F.f1=_tmp402;});({void*_tmp403=Cyc_Port_var();_tmp11F.f2=_tmp403;});({void*_tmp404=Cyc_Port_var();_tmp11F.f3=_tmp404;});({void*_tmp405=Cyc_Port_var();_tmp11F.f4=_tmp405;});({void*_tmp406=Cyc_Port_var();_tmp11F.f5=_tmp406;});_tmp11F;});struct _tuple12 _tmp116=_tmp115;void*_tmp11E;void*_tmp11D;void*_tmp11C;void*_tmp11B;void*_tmp11A;_LL10A: _tmp11E=_tmp116.f1;_tmp11D=_tmp116.f2;_tmp11C=_tmp116.f3;_tmp11B=_tmp116.f4;_tmp11A=_tmp116.f5;_LL10B:;{
struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp117=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp118=_cycalloc(sizeof(*_tmp118));({struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmp407=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct _tmp119;_tmp119.tag=11U;_tmp119.f1=_tmp11E;_tmp119.f2=_tmp11D;_tmp119.f3=_tmp11C;_tmp119.f4=_tmp11B;_tmp119.f5=_tmp11A;_tmp119;});_tmp118[0]=_tmp407;});_tmp118;});
Cyc_Port_leq(_tmp129,_tmp11E);Cyc_Port_leq(_tmp124,_tmp11E);
Cyc_Port_leq(_tmp128,_tmp11D);Cyc_Port_leq(_tmp123,_tmp11D);
Cyc_Port_leq(_tmp127,_tmp11C);Cyc_Port_leq(_tmp123,_tmp11C);
Cyc_Port_leq(_tmp126,_tmp11B);Cyc_Port_leq(_tmp121,_tmp11B);
Cyc_Port_leq(_tmp125,_tmp11A);Cyc_Port_leq(_tmp120,_tmp11A);
_tmp111->hd=(void*)((void*)_tmp117);
return*lowers;};}default: goto _LL107;}case 12U: if(((struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmp114.f2)->tag == 6U){_LL103: _LL104:
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
({void*_tmp408=Cyc_Port_compress_ct(t1);t1=_tmp408;});
({void*_tmp409=Cyc_Port_compress_ct(t2);t2=_tmp409;});{
struct _tuple10 _tmp12C=({struct _tuple10 _tmp14C;_tmp14C.f1=t1;_tmp14C.f2=t2;_tmp14C;});struct _tuple10 _tmp12D=_tmp12C;struct Cyc_Port_Cvar*_tmp14B;void*_tmp14A;void*_tmp149;void*_tmp148;void*_tmp147;void*_tmp146;void*_tmp145;void*_tmp144;void*_tmp143;void*_tmp142;void*_tmp141;void*_tmp140;void*_tmp13F;void*_tmp13E;void*_tmp13D;void*_tmp13C;void*_tmp13B;void*_tmp13A;void*_tmp139;void*_tmp138;void*_tmp137;void*_tmp136;void*_tmp135;void*_tmp134;struct Cyc_Port_Cvar*_tmp133;struct Cyc_Port_Cvar*_tmp132;struct Cyc_Port_Cvar*_tmp131;struct _dyneither_ptr _tmp130;struct _dyneither_ptr _tmp12F;struct _dyneither_ptr _tmp12E;switch(*((int*)_tmp12D.f1)){case 7U: _LL10D: _LL10E:
 return 1;case 10U: switch(*((int*)_tmp12D.f2)){case 10U: _LL10F: _tmp12F=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp12D.f1)->f1;_tmp12E=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp12D.f2)->f1;_LL110:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp12F,(struct _dyneither_ptr)_tmp12E)== 0;case 7U: _LL111: _tmp130=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp12D.f1)->f1;_LL112:
 return 0;case 16U: goto _LL139;default: goto _LL13B;}case 1U: switch(*((int*)_tmp12D.f2)){case 0U: _LL113: _LL114:
 return 1;case 16U: goto _LL139;default: goto _LL13B;}case 0U: switch(*((int*)_tmp12D.f2)){case 1U: _LL115: _LL116:
 return 0;case 16U: goto _LL139;default: goto _LL13B;}case 9U: switch(*((int*)_tmp12D.f2)){case 8U: _LL117: _LL118:
 return 0;case 16U: goto _LL139;default: goto _LL13B;}case 8U: switch(*((int*)_tmp12D.f2)){case 9U: _LL119: _LL11A:
 return 1;case 16U: goto _LL139;default: goto _LL13B;}case 16U: switch(*((int*)_tmp12D.f2)){case 0U: _LL11B: _LL11C:
 return 1;case 4U: _LL11D: _LL11E:
 return 1;case 16U: _LL135: _tmp132=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp12D.f1)->f1;_tmp131=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp12D.f2)->f1;_LL136:
# 737
({struct Cyc_List_List*_tmp40A=Cyc_Port_filter_self(t1,_tmp132->uppers);_tmp132->uppers=_tmp40A;});
({struct Cyc_List_List*_tmp40B=Cyc_Port_filter_self(t2,_tmp131->lowers);_tmp131->lowers=_tmp40B;});
({struct Cyc_List_List*_tmp40C=Cyc_Port_insert_upper(t1,t2,& _tmp132->uppers);_tmp132->uppers=_tmp40C;});
({struct Cyc_List_List*_tmp40D=Cyc_Port_insert_lower(t2,t1,& _tmp131->lowers);_tmp131->lowers=_tmp40D;});
return 1;default: _LL137: _tmp133=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp12D.f1)->f1;_LL138:
# 743
({struct Cyc_List_List*_tmp40E=Cyc_Port_filter_self(t1,_tmp133->uppers);_tmp133->uppers=_tmp40E;});
({struct Cyc_List_List*_tmp40F=Cyc_Port_insert_upper(t1,t2,& _tmp133->uppers);_tmp133->uppers=_tmp40F;});
return 1;}case 4U: _LL11F: _LL120:
# 720
 return 0;case 5U: switch(*((int*)_tmp12D.f2)){case 6U: _LL121: _LL122:
 return 1;case 11U: _LL123: _LL124:
 return 1;case 4U: _LL125: _LL126:
 return 1;case 16U: goto _LL139;default: goto _LL13B;}case 11U: switch(*((int*)_tmp12D.f2)){case 6U: _LL127: _LL128:
 return 1;case 4U: _LL129: _LL12A:
 return 1;case 11U: _LL12F: _tmp13D=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f1)->f1;_tmp13C=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f1)->f2;_tmp13B=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f1)->f3;_tmp13A=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f1)->f4;_tmp139=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f1)->f5;_tmp138=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f1;_tmp137=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f2;_tmp136=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f3;_tmp135=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f4;_tmp134=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f5;_LL130:
# 729
 return(((Cyc_Port_leq(_tmp13D,_tmp138) && Cyc_Port_leq(_tmp13C,_tmp137)) && Cyc_Port_unifies(_tmp13B,_tmp136)) && Cyc_Port_leq(_tmp13A,_tmp135)) && 
Cyc_Port_leq(_tmp139,_tmp134);case 16U: goto _LL139;default: goto _LL13B;}case 12U: switch(*((int*)_tmp12D.f2)){case 6U: _LL12B: _LL12C:
# 726
 return 1;case 4U: _LL12D: _LL12E:
 return 1;case 12U: _LL131: _tmp143=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f1)->f1;_tmp142=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f1)->f2;_tmp141=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f1)->f3;_tmp140=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f2)->f1;_tmp13F=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f2)->f2;_tmp13E=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f2)->f3;_LL132:
# 732
 return(Cyc_Port_leq(_tmp143,_tmp140) && Cyc_Port_leq(_tmp142,_tmp13F)) && Cyc_Port_leq(_tmp141,_tmp13E);case 11U: _LL133: _tmp14A=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f1)->f1;_tmp149=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f1)->f2;_tmp148=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp12D.f1)->f3;_tmp147=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f1;_tmp146=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f2;_tmp145=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f3;_tmp144=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp12D.f2)->f5;_LL134:
# 734
 return((Cyc_Port_leq(_tmp14A,_tmp147) && Cyc_Port_leq(_tmp149,_tmp146)) && Cyc_Port_unifies((void*)& Cyc_Port_Fat_ct_val,_tmp145)) && 
Cyc_Port_leq(_tmp148,_tmp144);case 16U: goto _LL139;default: goto _LL13B;}default: if(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp12D.f2)->tag == 16U){_LL139: _tmp14B=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp12D.f2)->f1;_LL13A:
# 747
({struct Cyc_List_List*_tmp410=Cyc_Port_filter_self(t2,_tmp14B->lowers);_tmp14B->lowers=_tmp410;});
({struct Cyc_List_List*_tmp411=Cyc_Port_insert_lower(t2,t1,& _tmp14B->lowers);_tmp14B->lowers=_tmp411;});
return 1;}else{_LL13B: _LL13C:
 return Cyc_Port_unifies(t1,t2);}}_LL10C:;};}struct Cyc_Port_GlobalCenv{struct Cyc_Dict_Dict typedef_dict;struct Cyc_Dict_Dict struct_dict;struct Cyc_Dict_Dict union_dict;void*return_type;struct Cyc_List_List*qualifier_edits;struct Cyc_List_List*pointer_edits;struct Cyc_List_List*zeroterm_edits;struct Cyc_List_List*vardecl_locs;struct Cyc_Hashtable_Table*varusage_tab;struct Cyc_List_List*edits;int porting;};
# 756
typedef struct Cyc_Port_GlobalCenv*Cyc_Port_gcenv_t;
# 781
enum Cyc_Port_CPos{Cyc_Port_FnRes_pos  = 0U,Cyc_Port_FnArg_pos  = 1U,Cyc_Port_FnBody_pos  = 2U,Cyc_Port_Toplevel_pos  = 3U};
typedef enum Cyc_Port_CPos Cyc_Port_cpos_t;struct Cyc_Port_GlobalCenv;struct Cyc_Port_Cenv{struct Cyc_Port_GlobalCenv*gcenv;struct Cyc_Dict_Dict var_dict;enum Cyc_Port_CPos cpos;};
# 784
typedef struct Cyc_Port_Cenv*Cyc_Port_cenv_t;struct Cyc_Port_Cenv;struct Cyc_Port_GlobalCenv;struct Cyc_Port_Cenv;struct Cyc_Port_GlobalCenv;
# 794
static struct Cyc_Port_Cenv*Cyc_Port_empty_cenv(){
struct Cyc_Port_GlobalCenv*g=({struct Cyc_Port_GlobalCenv*_tmp14E=_cycalloc(sizeof(*_tmp14E));({struct Cyc_Dict_Dict _tmp412=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp14E->typedef_dict=_tmp412;});({struct Cyc_Dict_Dict _tmp413=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp14E->struct_dict=_tmp413;});({struct Cyc_Dict_Dict _tmp414=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp14E->union_dict=_tmp414;});_tmp14E->qualifier_edits=0;_tmp14E->pointer_edits=0;_tmp14E->zeroterm_edits=0;_tmp14E->vardecl_locs=0;({struct Cyc_Hashtable_Table*_tmp415=
# 802
((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(unsigned int,unsigned int),int(*hash)(unsigned int)))Cyc_Hashtable_create)(128,Cyc_Port_cmp_seg_t,Cyc_Port_hash_seg_t);_tmp14E->varusage_tab=_tmp415;});_tmp14E->edits=0;_tmp14E->porting=0;({void*_tmp416=
# 805
Cyc_Port_void_ct();_tmp14E->return_type=_tmp416;});_tmp14E;});
return({struct Cyc_Port_Cenv*_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D->gcenv=g;_tmp14D->cpos=Cyc_Port_Toplevel_pos;({struct Cyc_Dict_Dict _tmp417=
# 808
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp14D->var_dict=_tmp417;});_tmp14D;});}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
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
void*_tmp154=_tmp155;_npop_handler(0U);return _tmp154;};}
# 828
;_pop_handler();}else{void*_tmp150=(void*)_exn_thrown;void*_tmp156=_tmp150;void*_tmp157;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp156)->tag == Cyc_Dict_Absent){_LL141: _LL142:
# 835
 return Cyc_Absyn_sint_typ;}else{_LL143: _tmp157=_tmp156;_LL144:(int)_rethrow(_tmp157);}_LL140:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 840
static void*Cyc_Port_lookup_typedef_ctype(struct Cyc_Port_Cenv*env,struct _tuple0*n){
struct _handler_cons _tmp158;_push_handler(& _tmp158);{int _tmp15A=0;if(setjmp(_tmp158.handler))_tmp15A=1;if(!_tmp15A){
{struct _tuple10 _tmp15B=*((struct _tuple10*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((env->gcenv)->typedef_dict,n);struct _tuple10 _tmp15C=_tmp15B;void*_tmp15E;_LL146: _tmp15E=_tmp15C.f2;_LL147:;{
void*_tmp15D=_tmp15E;_npop_handler(0U);return _tmp15D;};}
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
({struct Cyc_Dict_Dict _tmp418=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple11*v))Cyc_Dict_insert)((env->gcenv)->struct_dict,n,p);(env->gcenv)->struct_dict=_tmp418;});
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
({struct Cyc_Dict_Dict _tmp419=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple11*v))Cyc_Dict_insert)((env->gcenv)->union_dict,n,p);(env->gcenv)->union_dict=_tmp419;});
return p;}}struct _tuple13{struct _tuple10 f1;unsigned int f2;};struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct _tuple14{void*f1;struct _tuple10*f2;unsigned int f3;};
# 888
static struct _tuple13 Cyc_Port_lookup_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _tmp169;_push_handler(& _tmp169);{int _tmp16B=0;if(setjmp(_tmp169.handler))_tmp16B=1;if(!_tmp16B){
{struct _tuple14 _tmp16C=*((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);struct _tuple14 _tmp16D=_tmp16C;struct _tuple10*_tmp171;unsigned int _tmp170;_LL14E: _tmp171=_tmp16D.f2;_tmp170=_tmp16D.f3;_LL14F:;{
struct _tuple13 _tmp16F=({struct _tuple13 _tmp16E;_tmp16E.f1=*_tmp171;_tmp16E.f2=_tmp170;_tmp16E;});_npop_handler(0U);return _tmp16F;};}
# 890
;_pop_handler();}else{void*_tmp16A=(void*)_exn_thrown;void*_tmp172=_tmp16A;void*_tmp175;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp172)->tag == Cyc_Dict_Absent){_LL151: _LL152:
# 897
 return({struct _tuple13 _tmp173;({struct _tuple10 _tmp41C=({struct _tuple10 _tmp174;({void*_tmp41A=Cyc_Port_var();_tmp174.f1=_tmp41A;});({void*_tmp41B=Cyc_Port_var();_tmp174.f2=_tmp41B;});_tmp174;});_tmp173.f1=_tmp41C;});_tmp173.f2=0U;_tmp173;});}else{_LL153: _tmp175=_tmp172;_LL154:(int)_rethrow(_tmp175);}_LL150:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
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
LOOP: {void*_tmp17B=_tmp17F;struct _tuple0*_tmp17E;switch(*((int*)_tmp17B)){case 17U: _LL159: _tmp17E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp17B)->f1;_LL15A:
({void*_tmp41D=Cyc_Port_lookup_typedef(env,_tmp17E);_tmp17F=_tmp41D;});goto LOOP;case 9U: _LL15B: _LL15C: {
int _tmp17C=1;_npop_handler(0U);return _tmp17C;}default: _LL15D: _LL15E: {
int _tmp17D=0;_npop_handler(0U);return _tmp17D;}}_LL158:;}}
# 910
;_pop_handler();}else{void*_tmp177=(void*)_exn_thrown;void*_tmp180=_tmp177;void*_tmp181;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp180)->tag == Cyc_Dict_Absent){_LL160: _LL161:
# 921
 return 0;}else{_LL162: _tmp181=_tmp180;_LL163:(int)_rethrow(_tmp181);}_LL15F:;}};}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 925
static int Cyc_Port_isarray(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _tmp182;_push_handler(& _tmp182);{int _tmp184=0;if(setjmp(_tmp182.handler))_tmp184=1;if(!_tmp184){
{struct _tuple14 _tmp185=*((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);struct _tuple14 _tmp186=_tmp185;void*_tmp18B;_LL165: _tmp18B=_tmp186.f1;_LL166:;
LOOP: {void*_tmp187=_tmp18B;struct _tuple0*_tmp18A;switch(*((int*)_tmp187)){case 17U: _LL168: _tmp18A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp187)->f1;_LL169:
({void*_tmp41E=Cyc_Port_lookup_typedef(env,_tmp18A);_tmp18B=_tmp41E;});goto LOOP;case 8U: _LL16A: _LL16B: {
int _tmp188=1;_npop_handler(0U);return _tmp188;}default: _LL16C: _LL16D: {
int _tmp189=0;_npop_handler(0U);return _tmp189;}}_LL167:;}}
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
return({struct Cyc_Port_Cenv*_tmp18F=_cycalloc(sizeof(*_tmp18F));_tmp18F->gcenv=env->gcenv;({struct Cyc_Dict_Dict _tmp422=({
struct Cyc_Dict_Dict _tmp421=env->var_dict;struct _tuple0*_tmp420=x;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(_tmp421,_tmp420,({struct _tuple14*_tmp190=_cycalloc(sizeof(*_tmp190));_tmp190->f1=t;({struct _tuple10*_tmp41F=({struct _tuple10*_tmp191=_cycalloc(sizeof(*_tmp191));_tmp191->f1=qual;_tmp191->f2=ctype;_tmp191;});_tmp190->f2=_tmp41F;});_tmp190->f3=loc;_tmp190;}));});_tmp18F->var_dict=_tmp422;});_tmp18F->cpos=env->cpos;_tmp18F;});}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 962
static void Cyc_Port_add_typedef(struct Cyc_Port_Cenv*env,struct _tuple0*n,void*t,void*ct){
({struct Cyc_Dict_Dict _tmp425=({struct Cyc_Dict_Dict _tmp424=(env->gcenv)->typedef_dict;struct _tuple0*_tmp423=n;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple10*v))Cyc_Dict_insert)(_tmp424,_tmp423,({struct _tuple10*_tmp192=_cycalloc(sizeof(*_tmp192));_tmp192->f1=t;_tmp192->f2=ct;_tmp192;}));});(env->gcenv)->typedef_dict=_tmp425;});}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct _tuple15{struct _tuple0*f1;unsigned int f2;void*f3;};struct Cyc_Port_VarUsage;struct Cyc_Port_VarUsage;
# 966
static void Cyc_Port_register_localvar_decl(struct Cyc_Port_Cenv*env,struct _tuple0*x,unsigned int loc,void*type,struct Cyc_Absyn_Exp*init){
({struct Cyc_List_List*_tmp427=({struct Cyc_List_List*_tmp193=_cycalloc(sizeof(*_tmp193));({struct _tuple15*_tmp426=({struct _tuple15*_tmp194=_cycalloc(sizeof(*_tmp194));_tmp194->f1=x;_tmp194->f2=loc;_tmp194->f3=type;_tmp194;});_tmp193->hd=_tmp426;});_tmp193->tl=(env->gcenv)->vardecl_locs;_tmp193;});(env->gcenv)->vardecl_locs=_tmp427;});
({struct Cyc_Hashtable_Table*_tmp429=(env->gcenv)->varusage_tab;unsigned int _tmp428=loc;((void(*)(struct Cyc_Hashtable_Table*t,unsigned int key,struct Cyc_Port_VarUsage*val))Cyc_Hashtable_insert)(_tmp429,_tmp428,({struct Cyc_Port_VarUsage*_tmp195=_cycalloc(sizeof(*_tmp195));_tmp195->address_taken=0;_tmp195->init=init;_tmp195->usage_locs=0;_tmp195;}));});}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_VarUsage;struct Cyc_Port_VarUsage;struct Cyc_Port_VarUsage;struct Cyc_Port_VarUsage;struct Cyc_Port_VarUsage;
# 970
static void Cyc_Port_register_localvar_usage(struct Cyc_Port_Cenv*env,unsigned int declloc,unsigned int useloc,int takeaddress){
struct Cyc_Port_VarUsage*varusage=0;
if(((int(*)(struct Cyc_Hashtable_Table*t,unsigned int key,struct Cyc_Port_VarUsage**data))Cyc_Hashtable_try_lookup)((env->gcenv)->varusage_tab,declloc,& varusage)){
if(takeaddress)((struct Cyc_Port_VarUsage*)_check_null(varusage))->address_taken=1;{
struct Cyc_List_List*_tmp196=((struct Cyc_Port_VarUsage*)_check_null(varusage))->usage_locs;
({struct Cyc_List_List*_tmp42A=({struct Cyc_List_List*_tmp197=_cycalloc(sizeof(*_tmp197));_tmp197->hd=(void*)useloc;_tmp197->tl=_tmp196;_tmp197;});((struct Cyc_Port_VarUsage*)_check_null(varusage))->usage_locs=_tmp42A;});};}}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct _tuple16{void*f1;void*f2;unsigned int f3;};
# 981
static void Cyc_Port_register_const_cvar(struct Cyc_Port_Cenv*env,void*new_qual,void*orig_qual,unsigned int loc){
# 983
({struct Cyc_List_List*_tmp42C=({struct Cyc_List_List*_tmp198=_cycalloc(sizeof(*_tmp198));({struct _tuple16*_tmp42B=({struct _tuple16*_tmp199=_cycalloc(sizeof(*_tmp199));_tmp199->f1=new_qual;_tmp199->f2=orig_qual;_tmp199->f3=loc;_tmp199;});_tmp198->hd=_tmp42B;});_tmp198->tl=(env->gcenv)->qualifier_edits;_tmp198;});(env->gcenv)->qualifier_edits=_tmp42C;});}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 989
static void Cyc_Port_register_ptr_cvars(struct Cyc_Port_Cenv*env,void*new_ptr,void*orig_ptr,unsigned int loc){
# 991
({struct Cyc_List_List*_tmp42E=({struct Cyc_List_List*_tmp19A=_cycalloc(sizeof(*_tmp19A));({struct _tuple16*_tmp42D=({struct _tuple16*_tmp19B=_cycalloc(sizeof(*_tmp19B));_tmp19B->f1=new_ptr;_tmp19B->f2=orig_ptr;_tmp19B->f3=loc;_tmp19B;});_tmp19A->hd=_tmp42D;});_tmp19A->tl=(env->gcenv)->pointer_edits;_tmp19A;});(env->gcenv)->pointer_edits=_tmp42E;});}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 995
static void Cyc_Port_register_zeroterm_cvars(struct Cyc_Port_Cenv*env,void*new_zt,void*orig_zt,unsigned int loc){
# 997
({struct Cyc_List_List*_tmp430=({struct Cyc_List_List*_tmp19C=_cycalloc(sizeof(*_tmp19C));({struct _tuple16*_tmp42F=({struct _tuple16*_tmp19D=_cycalloc(sizeof(*_tmp19D));_tmp19D->f1=new_zt;_tmp19D->f2=orig_zt;_tmp19D->f3=loc;_tmp19D;});_tmp19C->hd=_tmp42F;});_tmp19C->tl=(env->gcenv)->zeroterm_edits;_tmp19C;});(env->gcenv)->zeroterm_edits=_tmp430;});}
# 1003
static void*Cyc_Port_main_type(){
struct _tuple8*arg1=({struct _tuple8*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3->f1=0;({struct Cyc_Absyn_Tqual _tmp431=
Cyc_Absyn_empty_tqual(0U);_tmp1A3->f2=_tmp431;});_tmp1A3->f3=Cyc_Absyn_sint_typ;_tmp1A3;});
struct _tuple8*arg2=({struct _tuple8*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));_tmp1A2->f1=0;({struct Cyc_Absyn_Tqual _tmp432=
Cyc_Absyn_empty_tqual(0U);_tmp1A2->f2=_tmp432;});({void*_tmp436=({
void*_tmp435=Cyc_Absyn_string_typ(Cyc_Absyn_wildtyp(0));void*_tmp434=Cyc_Absyn_wildtyp(0);struct Cyc_Absyn_Tqual _tmp433=
Cyc_Absyn_empty_tqual(0U);
# 1008
Cyc_Absyn_dyneither_typ(_tmp435,_tmp434,_tmp433,
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());});_tmp1A2->f3=_tmp436;});_tmp1A2;});
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp19E=_cycalloc(sizeof(*_tmp19E));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp43A=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp19F;_tmp19F.tag=9U;({struct Cyc_Absyn_FnInfo _tmp439=({struct Cyc_Absyn_FnInfo _tmp1A0;_tmp1A0.tvars=0;_tmp1A0.effect=0;({struct Cyc_Absyn_Tqual _tmp437=
Cyc_Absyn_empty_tqual(0U);_tmp1A0.ret_tqual=_tmp437;});_tmp1A0.ret_typ=Cyc_Absyn_sint_typ;({struct Cyc_List_List*_tmp438=({struct _tuple8*_tmp1A1[2U];_tmp1A1[1U]=arg2;_tmp1A1[0U]=arg1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1A1,sizeof(struct _tuple8*),2U));});_tmp1A0.args=_tmp438;});_tmp1A0.c_varargs=0;_tmp1A0.cyc_varargs=0;_tmp1A0.rgn_po=0;_tmp1A0.attributes=0;_tmp1A0.requires_clause=0;_tmp1A0.requires_relns=0;_tmp1A0.ensures_clause=0;_tmp1A0.ensures_relns=0;_tmp1A0;});_tmp19F.f1=_tmp439;});_tmp19F;});_tmp19E[0]=_tmp43A;});_tmp19E;});}struct Cyc_Port_Cenv;
# 1022
static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t);struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1025
static struct Cyc_Port_Cenv*Cyc_Port_initial_cenv(){
struct Cyc_Port_Cenv*_tmp1A4=Cyc_Port_empty_cenv();
# 1028
({struct Cyc_Port_Cenv*_tmp441=({struct Cyc_Port_Cenv*_tmp440=_tmp1A4;struct _tuple0*_tmp43F=({struct _tuple0*_tmp1A5=_cycalloc(sizeof(*_tmp1A5));_tmp1A5->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmp43B=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"main",sizeof(char),5U);_tmp1A5->f2=_tmp43B;});_tmp1A5;});void*_tmp43E=Cyc_Port_main_type();void*_tmp43D=Cyc_Port_const_ct();Cyc_Port_add_var(_tmp440,_tmp43F,_tmp43E,_tmp43D,({
struct Cyc_Port_Cenv*_tmp43C=_tmp1A4;Cyc_Port_type_to_ctype(_tmp43C,Cyc_Port_main_type());}),0U);});
# 1028
_tmp1A4=_tmp441;});
# 1030
return _tmp1A4;}
# 1036
static void Cyc_Port_region_counts(struct Cyc_Dict_Dict*cts,void*t){
void*_tmp1A7=Cyc_Port_compress_ct(t);void*_tmp1A8=_tmp1A7;void*_tmp1B5;struct Cyc_List_List*_tmp1B4;void*_tmp1B3;void*_tmp1B2;void*_tmp1B1;void*_tmp1B0;void*_tmp1AF;void*_tmp1AE;void*_tmp1AD;void*_tmp1AC;struct _dyneither_ptr*_tmp1AB;switch(*((int*)_tmp1A8)){case 0U: _LL174: _LL175:
 goto _LL177;case 1U: _LL176: _LL177:
 goto _LL179;case 2U: _LL178: _LL179:
 goto _LL17B;case 3U: _LL17A: _LL17B:
 goto _LL17D;case 4U: _LL17C: _LL17D:
 goto _LL17F;case 5U: _LL17E: _LL17F:
 goto _LL181;case 6U: _LL180: _LL181:
 goto _LL183;case 14U: _LL182: _LL183:
 goto _LL185;case 13U: _LL184: _LL185:
 goto _LL187;case 16U: _LL186: _LL187:
 goto _LL189;case 8U: _LL188: _LL189:
 goto _LL18B;case 9U: _LL18A: _LL18B:
 goto _LL18D;case 7U: _LL18C: _LL18D:
 return;case 10U: _LL18E: _tmp1AB=((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp1A8)->f1;_LL18F:
# 1052
 if(!((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*cts,_tmp1AB))
({struct Cyc_Dict_Dict _tmp444=({struct Cyc_Dict_Dict _tmp443=*cts;struct _dyneither_ptr*_tmp442=_tmp1AB;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,int*v))Cyc_Dict_insert)(_tmp443,_tmp442,({int*_tmp1A9=_cycalloc_atomic(sizeof(*_tmp1A9));_tmp1A9[0]=0;_tmp1A9;}));});*cts=_tmp444;});{
int*_tmp1AA=((int*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*cts,_tmp1AB);
*_tmp1AA=*_tmp1AA + 1;
return;};case 11U: _LL190: _tmp1B0=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1A8)->f1;_tmp1AF=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1A8)->f2;_tmp1AE=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1A8)->f3;_tmp1AD=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1A8)->f4;_tmp1AC=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1A8)->f5;_LL191:
# 1058
 Cyc_Port_region_counts(cts,_tmp1B0);
Cyc_Port_region_counts(cts,_tmp1AD);
return;case 12U: _LL192: _tmp1B3=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp1A8)->f1;_tmp1B2=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp1A8)->f2;_tmp1B1=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp1A8)->f3;_LL193:
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
({void*_tmp445=Cyc_Port_compress_ct(c);c=_tmp445;});{
struct _tuple10 _tmp1B6=({struct _tuple10 _tmp1CC;_tmp1CC.f1=t;_tmp1CC.f2=c;_tmp1CC;});struct _tuple10 _tmp1B7=_tmp1B6;void*_tmp1CB;struct Cyc_List_List*_tmp1CA;void*_tmp1C9;struct Cyc_List_List*_tmp1C8;void*_tmp1C7;void*_tmp1C6;void*_tmp1C5;void*_tmp1C4;struct Cyc_Absyn_PtrLoc*_tmp1C3;void*_tmp1C2;void*_tmp1C1;switch(*((int*)_tmp1B7.f1)){case 5U: if(((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1B7.f2)->tag == 11U){_LL197: _tmp1C5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B7.f1)->f1).elt_typ;_tmp1C4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B7.f1)->f1).ptr_atts).rgn;_tmp1C3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B7.f1)->f1).ptr_atts).ptrloc;_tmp1C2=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1B7.f2)->f1;_tmp1C1=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp1B7.f2)->f4;_LL198:
# 1081
 Cyc_Port_register_rgns(env,counts,fn_res,_tmp1C5,_tmp1C2);{
unsigned int _tmp1B8=(unsigned int)_tmp1C3?_tmp1C3->rgn_loc:(unsigned int)0;
({void*_tmp446=Cyc_Port_compress_ct(_tmp1C1);_tmp1C1=_tmp446;});
# 1098 "port.cyc"
({struct Cyc_List_List*_tmp44A=({struct Cyc_List_List*_tmp1B9=_cycalloc(sizeof(*_tmp1B9));({struct Cyc_Port_Edit*_tmp449=({struct Cyc_Port_Edit*_tmp1BA=_cycalloc(sizeof(*_tmp1BA));_tmp1BA->loc=_tmp1B8;({struct _dyneither_ptr _tmp447=({const char*_tmp1BC="`H ";_tag_dyneither(_tmp1BC,sizeof(char),4U);});_tmp1BA->old_string=_tmp447;});({struct _dyneither_ptr _tmp448=({const char*_tmp1BB="";_tag_dyneither(_tmp1BB,sizeof(char),1U);});_tmp1BA->new_string=_tmp448;});_tmp1BA;});_tmp1B9->hd=_tmp449;});_tmp1B9->tl=(env->gcenv)->edits;_tmp1B9;});(env->gcenv)->edits=_tmp44A;});
# 1101
goto _LL196;};}else{goto _LL19D;}case 8U: if(((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp1B7.f2)->tag == 12U){_LL199: _tmp1C7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B7.f1)->f1).elt_type;_tmp1C6=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp1B7.f2)->f1;_LL19A:
# 1103
 Cyc_Port_register_rgns(env,counts,fn_res,_tmp1C7,_tmp1C6);
goto _LL196;}else{goto _LL19D;}case 9U: if(((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp1B7.f2)->tag == 15U){_LL19B: _tmp1CB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B7.f1)->f1).ret_typ;_tmp1CA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B7.f1)->f1).args;_tmp1C9=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp1B7.f2)->f1;_tmp1C8=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp1B7.f2)->f2;_LL19C:
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
void*_tmp1CD=t;enum Cyc_Absyn_Size_of _tmp1CF;struct _tuple0*_tmp1CE;switch(*((int*)_tmp1CD)){case 17U: _LL1A3: _tmp1CE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1CD)->f1;_LL1A4:
# 1127
(*_tmp1CE).f1=Cyc_Absyn_Loc_n;
return({struct Cyc_Port_Cenv*_tmp44B=env;Cyc_Port_is_char(_tmp44B,Cyc_Port_lookup_typedef(env,_tmp1CE));});case 6U: _LL1A5: _tmp1CF=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1CD)->f2;_LL1A6:
 return 1;default: _LL1A7: _LL1A8:
 return 0;}_LL1A2:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1135
static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t){
void*_tmp1D0=t;struct Cyc_List_List*_tmp207;union Cyc_Absyn_AggrInfoU _tmp206;void*_tmp205;struct Cyc_List_List*_tmp204;void*_tmp203;struct Cyc_Absyn_Tqual _tmp202;union Cyc_Absyn_Constraint*_tmp201;unsigned int _tmp200;void*_tmp1FF;struct Cyc_Absyn_Tqual _tmp1FE;void*_tmp1FD;union Cyc_Absyn_Constraint*_tmp1FC;union Cyc_Absyn_Constraint*_tmp1FB;union Cyc_Absyn_Constraint*_tmp1FA;struct Cyc_Absyn_PtrLoc*_tmp1F9;struct _tuple0*_tmp1F8;switch(*((int*)_tmp1D0)){case 17U: _LL1AA: _tmp1F8=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1D0)->f1;_LL1AB:
# 1138
(*_tmp1F8).f1=Cyc_Absyn_Loc_n;
return Cyc_Port_lookup_typedef_ctype(env,_tmp1F8);case 0U: _LL1AC: _LL1AD:
 return Cyc_Port_void_ct();case 5U: _LL1AE: _tmp1FF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).elt_typ;_tmp1FE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).elt_tq;_tmp1FD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).ptr_atts).rgn;_tmp1FC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).ptr_atts).nullable;_tmp1FB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).ptr_atts).bounds;_tmp1FA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).ptr_atts).zero_term;_tmp1F9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).ptr_atts).ptrloc;_LL1AF: {
# 1142
unsigned int _tmp1D1=(unsigned int)_tmp1F9?_tmp1F9->ptr_loc:(unsigned int)0;
unsigned int _tmp1D2=(unsigned int)_tmp1F9?_tmp1F9->rgn_loc:(unsigned int)0;
unsigned int _tmp1D3=(unsigned int)_tmp1F9?_tmp1F9->zt_loc:(unsigned int)0;
# 1148
void*_tmp1D4=Cyc_Port_type_to_ctype(env,_tmp1FF);
void*new_rgn;
# 1151
{void*_tmp1D5=_tmp1FD;struct _dyneither_ptr*_tmp1D6;switch(*((int*)_tmp1D5)){case 20U: _LL1C1: _LL1C2:
# 1153
({void*_tmp44C=Cyc_Port_heap_ct();new_rgn=_tmp44C;});goto _LL1C0;case 2U: _LL1C3: _tmp1D6=(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1D5)->f1)->name;_LL1C4:
# 1155
({void*_tmp44D=Cyc_Port_rgnvar_ct(_tmp1D6);new_rgn=_tmp44D;});goto _LL1C0;case 1U: _LL1C5: _LL1C6:
# 1159
 if(Cyc_Port_in_fn_arg(env))
({void*_tmp44E=Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var());new_rgn=_tmp44E;});else{
# 1162
if(Cyc_Port_in_fn_res(env) || Cyc_Port_in_toplevel(env))
({void*_tmp44F=Cyc_Port_heap_ct();new_rgn=_tmp44F;});else{
({void*_tmp450=Cyc_Port_var();new_rgn=_tmp450;});}}
goto _LL1C0;default: _LL1C7: _LL1C8:
# 1167
({void*_tmp451=Cyc_Port_heap_ct();new_rgn=_tmp451;});goto _LL1C0;}_LL1C0:;}{
# 1169
int _tmp1D7=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp1FB)== (void*)& Cyc_Absyn_DynEither_b_val;
int orig_zt;
{union Cyc_Absyn_Constraint _tmp1D8=*_tmp1FA;union Cyc_Absyn_Constraint _tmp1D9=_tmp1D8;if((_tmp1D9.No_constr).tag == 3U){_LL1CA: _LL1CB:
({int _tmp452=Cyc_Port_is_char(env,_tmp1FF);orig_zt=_tmp452;});goto _LL1C9;}else{_LL1CC: _LL1CD:
({int _tmp453=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp1FA);orig_zt=_tmp453;});goto _LL1C9;}_LL1C9:;}
# 1175
if((env->gcenv)->porting){
void*_tmp1DA=Cyc_Port_var();
# 1180
({struct Cyc_Port_Cenv*_tmp456=env;void*_tmp455=_tmp1DA;void*_tmp454=_tmp1FE.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_register_const_cvar(_tmp456,_tmp455,_tmp454,_tmp1FE.loc);});
# 1183
if(_tmp1FE.print_const)({void*_tmp457=_tmp1DA;Cyc_Port_unify_ct(_tmp457,Cyc_Port_const_ct());});{
# 1189
void*_tmp1DB=Cyc_Port_var();
# 1192
({struct Cyc_Port_Cenv*_tmp45A=env;void*_tmp459=_tmp1DB;void*_tmp458=_tmp1D7?Cyc_Port_fat_ct(): Cyc_Port_thin_ct();Cyc_Port_register_ptr_cvars(_tmp45A,_tmp459,_tmp458,_tmp1D1);});
# 1194
if(_tmp1D7)({void*_tmp45B=_tmp1DB;Cyc_Port_unify_ct(_tmp45B,Cyc_Port_fat_ct());});{
void*_tmp1DC=Cyc_Port_var();
# 1198
({struct Cyc_Port_Cenv*_tmp45E=env;void*_tmp45D=_tmp1DC;void*_tmp45C=orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct();Cyc_Port_register_zeroterm_cvars(_tmp45E,_tmp45D,_tmp45C,_tmp1D3);});
# 1200
{union Cyc_Absyn_Constraint _tmp1DD=*_tmp1FA;union Cyc_Absyn_Constraint _tmp1DE=_tmp1DD;if((_tmp1DE.No_constr).tag == 3U){_LL1CF: _LL1D0:
# 1202
 goto _LL1CE;}else{_LL1D1: _LL1D2:
# 1204
({void*_tmp45F=_tmp1DC;Cyc_Port_unify_ct(_tmp45F,((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp1FA)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});
goto _LL1CE;}_LL1CE:;}
# 1210
return Cyc_Port_ptr_ct(_tmp1D4,_tmp1DA,_tmp1DB,new_rgn,_tmp1DC);};};}else{
# 1214
return({void*_tmp463=_tmp1D4;void*_tmp462=_tmp1FE.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();void*_tmp461=
_tmp1D7?Cyc_Port_fat_ct(): Cyc_Port_thin_ct();
# 1214
void*_tmp460=new_rgn;Cyc_Port_ptr_ct(_tmp463,_tmp462,_tmp461,_tmp460,
# 1216
orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});}};}case 6U: _LL1B0: _LL1B1:
# 1218
 goto _LL1B3;case 7U: _LL1B2: _LL1B3:
 return Cyc_Port_arith_ct();case 8U: _LL1B4: _tmp203=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D0)->f1).elt_type;_tmp202=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D0)->f1).tq;_tmp201=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D0)->f1).zero_term;_tmp200=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D0)->f1).zt_loc;_LL1B5: {
# 1222
void*_tmp1DF=Cyc_Port_type_to_ctype(env,_tmp203);
int orig_zt;
{union Cyc_Absyn_Constraint _tmp1E0=*_tmp201;union Cyc_Absyn_Constraint _tmp1E1=_tmp1E0;if((_tmp1E1.No_constr).tag == 3U){_LL1D4: _LL1D5:
 orig_zt=0;goto _LL1D3;}else{_LL1D6: _LL1D7:
({int _tmp464=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp201);orig_zt=_tmp464;});goto _LL1D3;}_LL1D3:;}
# 1228
if((env->gcenv)->porting){
void*_tmp1E2=Cyc_Port_var();
({struct Cyc_Port_Cenv*_tmp467=env;void*_tmp466=_tmp1E2;void*_tmp465=_tmp202.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_register_const_cvar(_tmp467,_tmp466,_tmp465,_tmp202.loc);});{
# 1237
void*_tmp1E3=Cyc_Port_var();
({struct Cyc_Port_Cenv*_tmp46A=env;void*_tmp469=_tmp1E3;void*_tmp468=orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct();Cyc_Port_register_zeroterm_cvars(_tmp46A,_tmp469,_tmp468,_tmp200);});
# 1240
{union Cyc_Absyn_Constraint _tmp1E4=*_tmp201;union Cyc_Absyn_Constraint _tmp1E5=_tmp1E4;if((_tmp1E5.No_constr).tag == 3U){_LL1D9: _LL1DA:
# 1242
 goto _LL1D8;}else{_LL1DB: _LL1DC:
# 1244
({void*_tmp46B=_tmp1E3;Cyc_Port_unify_ct(_tmp46B,((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp201)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});
goto _LL1D8;}_LL1D8:;}
# 1247
return Cyc_Port_array_ct(_tmp1DF,_tmp1E2,_tmp1E3);};}else{
# 1249
return({void*_tmp46D=_tmp1DF;void*_tmp46C=_tmp202.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_array_ct(_tmp46D,_tmp46C,
orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});}}case 9U: _LL1B6: _tmp205=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D0)->f1).ret_typ;_tmp204=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D0)->f1).args;_LL1B7: {
# 1253
void*_tmp1E6=({struct Cyc_Port_Cenv*_tmp46E=Cyc_Port_set_cpos(env,Cyc_Port_FnRes_pos);Cyc_Port_type_to_ctype(_tmp46E,_tmp205);});
struct Cyc_Port_Cenv*_tmp1E7=Cyc_Port_set_cpos(env,Cyc_Port_FnArg_pos);
struct Cyc_List_List*cargs=0;
for(0;_tmp204 != 0;_tmp204=_tmp204->tl){
struct _tuple8 _tmp1E8=*((struct _tuple8*)_tmp204->hd);struct _tuple8 _tmp1E9=_tmp1E8;struct Cyc_Absyn_Tqual _tmp1EC;void*_tmp1EB;_LL1DE: _tmp1EC=_tmp1E9.f2;_tmp1EB=_tmp1E9.f3;_LL1DF:;
({struct Cyc_List_List*_tmp470=({struct Cyc_List_List*_tmp1EA=_cycalloc(sizeof(*_tmp1EA));({void*_tmp46F=Cyc_Port_type_to_ctype(_tmp1E7,_tmp1EB);_tmp1EA->hd=_tmp46F;});_tmp1EA->tl=cargs;_tmp1EA;});cargs=_tmp470;});}
# 1260
return({void*_tmp471=_tmp1E6;Cyc_Port_fn_ct(_tmp471,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(cargs));});}case 11U: _LL1B8: _tmp206=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1D0)->f1).aggr_info;_LL1B9: {
# 1262
union Cyc_Absyn_AggrInfoU _tmp1ED=_tmp206;struct Cyc_Absyn_Aggrdecl**_tmp1F7;struct _tuple0*_tmp1F6;struct _tuple0*_tmp1F5;if((_tmp1ED.UnknownAggr).tag == 1U){if(((_tmp1ED.UnknownAggr).val).f1 == Cyc_Absyn_StructA){_LL1E1: _tmp1F5=((_tmp1ED.UnknownAggr).val).f2;_LL1E2:
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
return Cyc_Port_known_aggr_ct(_tmp1F4);}}_LL1E7:;}}_LL1E0:;}case 13U: _LL1BA: _LL1BB:
# 1282
 return Cyc_Port_arith_ct();case 14U: _LL1BC: _tmp207=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1D0)->f1;_LL1BD:
# 1285
 for(0;(unsigned int)_tmp207;_tmp207=_tmp207->tl){
(*((struct Cyc_Absyn_Enumfield*)_tmp207->hd)->name).f1=Cyc_Absyn_Loc_n;
({struct Cyc_Port_Cenv*_tmp476=({struct Cyc_Port_Cenv*_tmp475=env;struct _tuple0*_tmp474=((struct Cyc_Absyn_Enumfield*)_tmp207->hd)->name;void*_tmp473=Cyc_Absyn_sint_typ;void*_tmp472=Cyc_Port_const_ct();Cyc_Port_add_var(_tmp475,_tmp474,_tmp473,_tmp472,Cyc_Port_arith_ct(),0U);});env=_tmp476;});}
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
if(({void*_tmp47B=_tmp208;Cyc_Port_leq(_tmp47B,({void*_tmp47A=Cyc_Port_var();void*_tmp479=Cyc_Port_var();void*_tmp478=Cyc_Port_fat_ct();void*_tmp477=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp47A,_tmp479,_tmp478,_tmp477,Cyc_Port_var());}));})){
({void*_tmp47C=_tmp20B;Cyc_Port_leq(_tmp47C,Cyc_Port_arith_ct());});
return _tmp208;}else{
if(({void*_tmp481=_tmp20B;Cyc_Port_leq(_tmp481,({void*_tmp480=Cyc_Port_var();void*_tmp47F=Cyc_Port_var();void*_tmp47E=Cyc_Port_fat_ct();void*_tmp47D=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp480,_tmp47F,_tmp47E,_tmp47D,Cyc_Port_var());}));})){
({void*_tmp482=_tmp208;Cyc_Port_leq(_tmp482,Cyc_Port_arith_ct());});
return _tmp20B;}else{
# 1311
({void*_tmp483=_tmp208;Cyc_Port_leq(_tmp483,Cyc_Port_arith_ct());});
({void*_tmp484=_tmp20B;Cyc_Port_leq(_tmp484,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();}}}case Cyc_Absyn_Minus: _LL1EF: _LL1F0:
# 1320
 if(_tmp209 == 0){
# 1322
({void*_tmp485=_tmp208;Cyc_Port_leq(_tmp485,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();}else{
# 1325
void*_tmp20C=Cyc_Port_compress_ct((void*)_tmp209->hd);
if(({void*_tmp48A=_tmp208;Cyc_Port_leq(_tmp48A,({void*_tmp489=Cyc_Port_var();void*_tmp488=Cyc_Port_var();void*_tmp487=Cyc_Port_fat_ct();void*_tmp486=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp489,_tmp488,_tmp487,_tmp486,Cyc_Port_var());}));})){
if(({void*_tmp48F=_tmp20C;Cyc_Port_leq(_tmp48F,({void*_tmp48E=Cyc_Port_var();void*_tmp48D=Cyc_Port_var();void*_tmp48C=Cyc_Port_fat_ct();void*_tmp48B=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp48E,_tmp48D,_tmp48C,_tmp48B,Cyc_Port_var());}));}))
return Cyc_Port_arith_ct();else{
# 1330
({void*_tmp490=_tmp20C;Cyc_Port_leq(_tmp490,Cyc_Port_arith_ct());});
return _tmp208;}}else{
# 1334
({void*_tmp491=_tmp208;Cyc_Port_leq(_tmp491,Cyc_Port_arith_ct());});
({void*_tmp492=_tmp208;Cyc_Port_leq(_tmp492,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();}}case Cyc_Absyn_Times: _LL1F1: _LL1F2:
# 1339
 goto _LL1F4;case Cyc_Absyn_Div: _LL1F3: _LL1F4: goto _LL1F6;case Cyc_Absyn_Mod: _LL1F5: _LL1F6: goto _LL1F8;case Cyc_Absyn_Eq: _LL1F7: _LL1F8: goto _LL1FA;case Cyc_Absyn_Neq: _LL1F9: _LL1FA: goto _LL1FC;case Cyc_Absyn_Lt: _LL1FB: _LL1FC: goto _LL1FE;case Cyc_Absyn_Gt: _LL1FD: _LL1FE: goto _LL200;case Cyc_Absyn_Lte: _LL1FF: _LL200:
 goto _LL202;case Cyc_Absyn_Gte: _LL201: _LL202: goto _LL204;case Cyc_Absyn_Bitand: _LL203: _LL204: goto _LL206;case Cyc_Absyn_Bitor: _LL205: _LL206: goto _LL208;case Cyc_Absyn_Bitxor: _LL207: _LL208: goto _LL20A;case Cyc_Absyn_Bitlshift: _LL209: _LL20A: goto _LL20C;case Cyc_Absyn_Bitlrshift: _LL20B: _LL20C:
 goto _LL20E;case Cyc_Absyn_Bitarshift: _LL20D: _LL20E:
({void*_tmp493=_tmp208;Cyc_Port_leq(_tmp493,Cyc_Port_arith_ct());});
({void*_tmp494=(void*)((struct Cyc_List_List*)_check_null(_tmp209))->hd;Cyc_Port_leq(_tmp494,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();case Cyc_Absyn_Not: _LL20F: _LL210:
 goto _LL212;case Cyc_Absyn_Bitnot: _LL211: _LL212:
# 1347
({void*_tmp495=_tmp208;Cyc_Port_leq(_tmp495,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();default: _LL213: _tmp20F=_tmp20A;_LL214:
({void*_tmp20D=0U;({struct _dyneither_ptr _tmp496=({const char*_tmp20E=".size primop";_tag_dyneither(_tmp20E,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp496,_tag_dyneither(_tmp20D,sizeof(void*),0U));});});}_LL1EC:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
# 1354
static struct _tuple10 Cyc_Port_gen_lhs(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e,int takeaddress){
void*_tmp210=e->r;void*_tmp211=_tmp210;struct Cyc_Absyn_Exp*_tmp237;struct _dyneither_ptr*_tmp236;struct Cyc_Absyn_Exp*_tmp235;struct _dyneither_ptr*_tmp234;struct Cyc_Absyn_Exp*_tmp233;struct Cyc_Absyn_Exp*_tmp232;struct Cyc_Absyn_Exp*_tmp231;void*_tmp230;switch(*((int*)_tmp211)){case 1U: _LL216: _tmp230=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_LL217: {
# 1357
struct _tuple0*_tmp212=Cyc_Absyn_binding2qvar(_tmp230);
(*_tmp212).f1=Cyc_Absyn_Loc_n;{
struct _tuple13 _tmp213=Cyc_Port_lookup_var(env,_tmp212);struct _tuple13 _tmp214=_tmp213;struct _tuple10 _tmp216;unsigned int _tmp215;_LL223: _tmp216=_tmp214.f1;_tmp215=_tmp214.f2;_LL224:;
Cyc_Port_register_localvar_usage(env,_tmp215,e->loc,takeaddress);
return _tmp216;};}case 23U: _LL218: _tmp232=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_tmp231=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp211)->f2;_LL219: {
# 1363
void*_tmp217=Cyc_Port_var();
void*_tmp218=Cyc_Port_var();
void*_tmp219=Cyc_Port_gen_exp(0,env,_tmp232);
({void*_tmp497=Cyc_Port_gen_exp(0,env,_tmp231);Cyc_Port_leq(_tmp497,Cyc_Port_arith_ct());});{
void*_tmp21A=({void*_tmp49B=_tmp217;void*_tmp49A=_tmp218;void*_tmp499=Cyc_Port_fat_ct();void*_tmp498=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp49B,_tmp49A,_tmp499,_tmp498,Cyc_Port_var());});
Cyc_Port_leq(_tmp219,_tmp21A);
return({struct _tuple10 _tmp21B;_tmp21B.f1=_tmp218;_tmp21B.f2=_tmp217;_tmp21B;});};}case 20U: _LL21A: _tmp233=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_LL21B: {
# 1371
void*_tmp21C=Cyc_Port_var();
void*_tmp21D=Cyc_Port_var();
void*_tmp21E=({void*_tmp49F=_tmp21C;void*_tmp49E=_tmp21D;void*_tmp49D=Cyc_Port_var();void*_tmp49C=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp49F,_tmp49E,_tmp49D,_tmp49C,Cyc_Port_var());});
({void*_tmp4A0=Cyc_Port_gen_exp(0,env,_tmp233);Cyc_Port_leq(_tmp4A0,_tmp21E);});
return({struct _tuple10 _tmp21F;_tmp21F.f1=_tmp21D;_tmp21F.f2=_tmp21C;_tmp21F;});}case 21U: _LL21C: _tmp235=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_tmp234=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp211)->f2;_LL21D: {
# 1377
void*_tmp220=Cyc_Port_var();
void*_tmp221=Cyc_Port_var();
void*_tmp222=Cyc_Port_gen_exp(takeaddress,env,_tmp235);
({void*_tmp4A2=_tmp222;Cyc_Port_leq(_tmp4A2,Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp223[1U];({struct Cyc_Port_Cfield*_tmp4A1=({struct Cyc_Port_Cfield*_tmp224=_cycalloc(sizeof(*_tmp224));_tmp224->qual=_tmp221;_tmp224->f=_tmp234;_tmp224->type=_tmp220;_tmp224;});_tmp223[0U]=_tmp4A1;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp223,sizeof(struct Cyc_Port_Cfield*),1U));})));});
return({struct _tuple10 _tmp225;_tmp225.f1=_tmp221;_tmp225.f2=_tmp220;_tmp225;});}case 22U: _LL21E: _tmp237=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_tmp236=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp211)->f2;_LL21F: {
# 1383
void*_tmp226=Cyc_Port_var();
void*_tmp227=Cyc_Port_var();
void*_tmp228=Cyc_Port_gen_exp(0,env,_tmp237);
({void*_tmp4A8=_tmp228;Cyc_Port_leq(_tmp4A8,({void*_tmp4A7=Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp229[1U];({struct Cyc_Port_Cfield*_tmp4A3=({struct Cyc_Port_Cfield*_tmp22A=_cycalloc(sizeof(*_tmp22A));_tmp22A->qual=_tmp227;_tmp22A->f=_tmp236;_tmp22A->type=_tmp226;_tmp22A;});_tmp229[0U]=_tmp4A3;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp229,sizeof(struct Cyc_Port_Cfield*),1U));}));void*_tmp4A6=
Cyc_Port_var();
# 1386
void*_tmp4A5=
Cyc_Port_var();
# 1386
void*_tmp4A4=
Cyc_Port_var();
# 1386
Cyc_Port_ptr_ct(_tmp4A7,_tmp4A6,_tmp4A5,_tmp4A4,
Cyc_Port_var());}));});
return({struct _tuple10 _tmp22B;_tmp22B.f1=_tmp227;_tmp22B.f2=_tmp226;_tmp22B;});}default: _LL220: _LL221:
({void*_tmp22C=0U;({struct _dyneither_ptr _tmp4AB=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp22F;_tmp22F.tag=0U;({struct _dyneither_ptr _tmp4A9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp22F.f1=_tmp4A9;});({void*_tmp22D[1U]={& _tmp22F};({struct _dyneither_ptr _tmp4AA=({const char*_tmp22E="gen_lhs: %s";_tag_dyneither(_tmp22E,sizeof(char),12U);});Cyc_aprintf(_tmp4AA,_tag_dyneither(_tmp22D,sizeof(void*),1U));});});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4AB,_tag_dyneither(_tmp22C,sizeof(void*),0U));});});}_LL215:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1394
static void*Cyc_Port_gen_exp_false(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){
return Cyc_Port_gen_exp(0,env,e);}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;
# 1397
static void*Cyc_Port_gen_exp(int takeaddress,struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){
void*_tmp238=e->r;void*_tmp239=_tmp238;struct Cyc_Absyn_Stmt*_tmp2AF;struct Cyc_Absyn_Exp*_tmp2AE;struct Cyc_Absyn_Exp*_tmp2AD;void**_tmp2AC;struct Cyc_Absyn_Exp*_tmp2AB;struct Cyc_Absyn_Exp*_tmp2AA;struct _dyneither_ptr*_tmp2A9;struct Cyc_Absyn_Exp*_tmp2A8;struct _dyneither_ptr*_tmp2A7;struct Cyc_Absyn_Exp*_tmp2A6;struct Cyc_Absyn_Exp*_tmp2A5;struct Cyc_Absyn_Exp*_tmp2A4;struct Cyc_Absyn_Exp*_tmp2A3;void*_tmp2A2;struct Cyc_Absyn_Exp*_tmp2A1;struct Cyc_Absyn_Exp*_tmp2A0;struct Cyc_Absyn_Exp*_tmp29F;struct Cyc_List_List*_tmp29E;struct Cyc_Absyn_Exp*_tmp29D;struct Cyc_Absyn_Exp*_tmp29C;struct Cyc_Absyn_Exp*_tmp29B;struct Cyc_Absyn_Exp*_tmp29A;struct Cyc_Absyn_Exp*_tmp299;struct Cyc_Absyn_Exp*_tmp298;struct Cyc_Absyn_Exp*_tmp297;struct Cyc_Absyn_Exp*_tmp296;struct Cyc_Absyn_Exp*_tmp295;struct Cyc_Absyn_Exp*_tmp294;enum Cyc_Absyn_Incrementor _tmp293;struct Cyc_Absyn_Exp*_tmp292;struct Cyc_Core_Opt*_tmp291;struct Cyc_Absyn_Exp*_tmp290;enum Cyc_Absyn_Primop _tmp28F;struct Cyc_List_List*_tmp28E;void*_tmp28D;switch(*((int*)_tmp239)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp239)->f1).Wstring_c).tag){case 2U: _LL226: _LL227:
 goto _LL229;case 3U: _LL228: _LL229:
 goto _LL22B;case 4U: _LL22A: _LL22B:
 goto _LL22D;case 6U: _LL22C: _LL22D:
 goto _LL22F;case 7U: _LL238: _LL239:
# 1408
 return Cyc_Port_arith_ct();case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp239)->f1).Int_c).val).f2 == 0U){_LL23A: _LL23B:
 return Cyc_Port_zero_ct();}else{_LL23C: _LL23D:
 return Cyc_Port_arith_ct();}case 8U: _LL23E: _LL23F:
# 1412
 return({void*_tmp4AF=Cyc_Port_arith_ct();void*_tmp4AE=Cyc_Port_const_ct();void*_tmp4AD=Cyc_Port_fat_ct();void*_tmp4AC=Cyc_Port_heap_ct();Cyc_Port_ptr_ct(_tmp4AF,_tmp4AE,_tmp4AD,_tmp4AC,Cyc_Port_zterm_ct());});case 9U: _LL240: _LL241:
# 1414
 return({void*_tmp4B3=Cyc_Port_arith_ct();void*_tmp4B2=Cyc_Port_const_ct();void*_tmp4B1=Cyc_Port_fat_ct();void*_tmp4B0=Cyc_Port_heap_ct();Cyc_Port_ptr_ct(_tmp4B3,_tmp4B2,_tmp4B1,_tmp4B0,Cyc_Port_zterm_ct());});default: _LL242: _LL243:
 return Cyc_Port_zero_ct();}case 17U: _LL22E: _LL22F:
# 1403
 goto _LL231;case 18U: _LL230: _LL231:
 goto _LL233;case 19U: _LL232: _LL233:
 goto _LL235;case 32U: _LL234: _LL235:
 goto _LL237;case 33U: _LL236: _LL237:
 goto _LL239;case 1U: _LL244: _tmp28D=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_LL245: {
# 1417
struct _tuple0*_tmp23A=Cyc_Absyn_binding2qvar(_tmp28D);
(*_tmp23A).f1=Cyc_Absyn_Loc_n;{
struct _tuple13 _tmp23B=Cyc_Port_lookup_var(env,_tmp23A);struct _tuple13 _tmp23C=_tmp23B;struct _tuple10 _tmp245;unsigned int _tmp244;_LL28B: _tmp245=_tmp23C.f1;_tmp244=_tmp23C.f2;_LL28C:;{
struct _tuple10 _tmp23D=_tmp245;void*_tmp243;_LL28E: _tmp243=_tmp23D.f2;_LL28F:;
if(Cyc_Port_isfn(env,_tmp23A)){
({void*_tmp4B4=Cyc_Port_instantiate(_tmp243);_tmp243=_tmp4B4;});
return({void*_tmp4B8=_tmp243;void*_tmp4B7=Cyc_Port_var();void*_tmp4B6=Cyc_Port_var();void*_tmp4B5=Cyc_Port_heap_ct();Cyc_Port_ptr_ct(_tmp4B8,_tmp4B7,_tmp4B6,_tmp4B5,Cyc_Port_nozterm_ct());});}else{
# 1425
if(Cyc_Port_isarray(env,_tmp23A)){
void*_tmp23E=Cyc_Port_var();
void*_tmp23F=Cyc_Port_var();
void*_tmp240=Cyc_Port_var();
void*_tmp241=Cyc_Port_array_ct(_tmp23E,_tmp23F,_tmp240);
Cyc_Port_unifies(_tmp243,_tmp241);{
void*_tmp242=({void*_tmp4BC=_tmp23E;void*_tmp4BB=_tmp23F;void*_tmp4BA=Cyc_Port_fat_ct();void*_tmp4B9=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4BC,_tmp4BB,_tmp4BA,_tmp4B9,_tmp240);});
return _tmp242;};}else{
# 1434
Cyc_Port_register_localvar_usage(env,_tmp244,e->loc,takeaddress);
return _tmp243;}}};};}case 3U: _LL246: _tmp28F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp28E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_LL247:
# 1437
 return({struct Cyc_Port_Cenv*_tmp4BE=env;enum Cyc_Absyn_Primop _tmp4BD=_tmp28F;Cyc_Port_gen_primop(_tmp4BE,_tmp4BD,((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_gen_exp_false,env,_tmp28E));});case 4U: _LL248: _tmp292=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp291=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_tmp290=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp239)->f3;_LL249: {
# 1439
struct _tuple10 _tmp246=Cyc_Port_gen_lhs(env,_tmp292,0);struct _tuple10 _tmp247=_tmp246;void*_tmp24D;void*_tmp24C;_LL291: _tmp24D=_tmp247.f1;_tmp24C=_tmp247.f2;_LL292:;
({void*_tmp4BF=_tmp24D;Cyc_Port_unifies(_tmp4BF,Cyc_Port_notconst_ct());});{
void*_tmp248=Cyc_Port_gen_exp(0,env,_tmp290);
if((unsigned int)_tmp291){
struct Cyc_List_List x2=({struct Cyc_List_List _tmp24B;_tmp24B.hd=_tmp248;_tmp24B.tl=0;_tmp24B;});
struct Cyc_List_List x1=({struct Cyc_List_List _tmp24A;_tmp24A.hd=_tmp24C;_tmp24A.tl=& x2;_tmp24A;});
void*_tmp249=Cyc_Port_gen_primop(env,(enum Cyc_Absyn_Primop)_tmp291->v,& x1);
Cyc_Port_leq(_tmp249,_tmp24C);
return _tmp24C;}else{
# 1449
Cyc_Port_leq(_tmp248,_tmp24C);
return _tmp24C;}};}case 5U: _LL24A: _tmp294=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp293=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_LL24B: {
# 1453
struct _tuple10 _tmp24E=Cyc_Port_gen_lhs(env,_tmp294,0);struct _tuple10 _tmp24F=_tmp24E;void*_tmp251;void*_tmp250;_LL294: _tmp251=_tmp24F.f1;_tmp250=_tmp24F.f2;_LL295:;
({void*_tmp4C0=_tmp251;Cyc_Port_unifies(_tmp4C0,Cyc_Port_notconst_ct());});
# 1456
({void*_tmp4C5=_tmp250;Cyc_Port_leq(_tmp4C5,({void*_tmp4C4=Cyc_Port_var();void*_tmp4C3=Cyc_Port_var();void*_tmp4C2=Cyc_Port_fat_ct();void*_tmp4C1=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4C4,_tmp4C3,_tmp4C2,_tmp4C1,Cyc_Port_var());}));});
({void*_tmp4C6=_tmp250;Cyc_Port_leq(_tmp4C6,Cyc_Port_arith_ct());});
return _tmp250;}case 6U: _LL24C: _tmp297=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp296=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_tmp295=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp239)->f3;_LL24D: {
# 1460
void*_tmp252=Cyc_Port_var();
({void*_tmp4C7=Cyc_Port_gen_exp(0,env,_tmp297);Cyc_Port_leq(_tmp4C7,Cyc_Port_arith_ct());});
({void*_tmp4C8=Cyc_Port_gen_exp(0,env,_tmp296);Cyc_Port_leq(_tmp4C8,_tmp252);});
({void*_tmp4C9=Cyc_Port_gen_exp(0,env,_tmp295);Cyc_Port_leq(_tmp4C9,_tmp252);});
return _tmp252;}case 7U: _LL24E: _tmp299=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp298=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_LL24F:
 _tmp29B=_tmp299;_tmp29A=_tmp298;goto _LL251;case 8U: _LL250: _tmp29B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp29A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_LL251:
# 1467
({void*_tmp4CA=Cyc_Port_gen_exp(0,env,_tmp29B);Cyc_Port_leq(_tmp4CA,Cyc_Port_arith_ct());});
({void*_tmp4CB=Cyc_Port_gen_exp(0,env,_tmp29A);Cyc_Port_leq(_tmp4CB,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();case 9U: _LL252: _tmp29D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp29C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_LL253:
# 1471
 Cyc_Port_gen_exp(0,env,_tmp29D);
return Cyc_Port_gen_exp(0,env,_tmp29C);case 10U: _LL254: _tmp29F=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp29E=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_LL255: {
# 1474
void*_tmp253=Cyc_Port_var();
void*_tmp254=Cyc_Port_gen_exp(0,env,_tmp29F);
struct Cyc_List_List*_tmp255=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_gen_exp_false,env,_tmp29E);
struct Cyc_List_List*_tmp256=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(void*x))Cyc_Port_new_var,_tmp255);
({void*_tmp4D0=_tmp254;Cyc_Port_unifies(_tmp4D0,({void*_tmp4CF=Cyc_Port_fn_ct(_tmp253,_tmp256);void*_tmp4CE=Cyc_Port_var();void*_tmp4CD=Cyc_Port_var();void*_tmp4CC=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4CF,_tmp4CE,_tmp4CD,_tmp4CC,Cyc_Port_nozterm_ct());}));});
for(0;_tmp255 != 0;(_tmp255=_tmp255->tl,_tmp256=_tmp256->tl)){
Cyc_Port_leq((void*)_tmp255->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp256))->hd);}
# 1482
return _tmp253;}case 11U: _LL256: _LL257:
({void*_tmp257=0U;({struct _dyneither_ptr _tmp4D1=({const char*_tmp258="throw";_tag_dyneither(_tmp258,sizeof(char),6U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4D1,_tag_dyneither(_tmp257,sizeof(void*),0U));});});case 12U: _LL258: _tmp2A0=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_LL259:
 return Cyc_Port_gen_exp(0,env,_tmp2A0);case 13U: _LL25A: _LL25B:
({void*_tmp259=0U;({struct _dyneither_ptr _tmp4D2=({const char*_tmp25A="instantiate";_tag_dyneither(_tmp25A,sizeof(char),12U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4D2,_tag_dyneither(_tmp259,sizeof(void*),0U));});});case 14U: _LL25C: _tmp2A2=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp2A1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_LL25D: {
# 1487
void*_tmp25B=Cyc_Port_type_to_ctype(env,_tmp2A2);
void*_tmp25C=Cyc_Port_gen_exp(0,env,_tmp2A1);
Cyc_Port_leq(_tmp25B,_tmp25C);
return _tmp25C;}case 15U: _LL25E: _tmp2A3=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_LL25F: {
# 1492
struct _tuple10 _tmp25D=Cyc_Port_gen_lhs(env,_tmp2A3,1);struct _tuple10 _tmp25E=_tmp25D;void*_tmp260;void*_tmp25F;_LL297: _tmp260=_tmp25E.f1;_tmp25F=_tmp25E.f2;_LL298:;
return({void*_tmp4D6=_tmp25F;void*_tmp4D5=_tmp260;void*_tmp4D4=Cyc_Port_var();void*_tmp4D3=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4D6,_tmp4D5,_tmp4D4,_tmp4D3,Cyc_Port_var());});}case 23U: _LL260: _tmp2A5=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp2A4=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_LL261: {
# 1495
void*_tmp261=Cyc_Port_var();
({void*_tmp4D7=Cyc_Port_gen_exp(0,env,_tmp2A4);Cyc_Port_leq(_tmp4D7,Cyc_Port_arith_ct());});{
void*_tmp262=Cyc_Port_gen_exp(0,env,_tmp2A5);
({void*_tmp4DC=_tmp262;Cyc_Port_leq(_tmp4DC,({void*_tmp4DB=_tmp261;void*_tmp4DA=Cyc_Port_var();void*_tmp4D9=Cyc_Port_fat_ct();void*_tmp4D8=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4DB,_tmp4DA,_tmp4D9,_tmp4D8,Cyc_Port_var());}));});
return _tmp261;};}case 20U: _LL262: _tmp2A6=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_LL263: {
# 1501
void*_tmp263=Cyc_Port_var();
({void*_tmp4E1=Cyc_Port_gen_exp(0,env,_tmp2A6);Cyc_Port_leq(_tmp4E1,({void*_tmp4E0=_tmp263;void*_tmp4DF=Cyc_Port_var();void*_tmp4DE=Cyc_Port_var();void*_tmp4DD=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4E0,_tmp4DF,_tmp4DE,_tmp4DD,Cyc_Port_var());}));});
return _tmp263;}case 21U: _LL264: _tmp2A8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp2A7=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_LL265: {
# 1505
void*_tmp264=Cyc_Port_var();
void*_tmp265=Cyc_Port_gen_exp(takeaddress,env,_tmp2A8);
({void*_tmp4E4=_tmp265;Cyc_Port_leq(_tmp4E4,Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp266[1U];({struct Cyc_Port_Cfield*_tmp4E3=({struct Cyc_Port_Cfield*_tmp267=_cycalloc(sizeof(*_tmp267));({void*_tmp4E2=Cyc_Port_var();_tmp267->qual=_tmp4E2;});_tmp267->f=_tmp2A7;_tmp267->type=_tmp264;_tmp267;});_tmp266[0U]=_tmp4E3;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp266,sizeof(struct Cyc_Port_Cfield*),1U));})));});
return _tmp264;}case 22U: _LL266: _tmp2AA=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp2A9=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_LL267: {
# 1510
void*_tmp268=Cyc_Port_var();
void*_tmp269=Cyc_Port_gen_exp(0,env,_tmp2AA);
({void*_tmp4EB=Cyc_Port_gen_exp(0,env,_tmp2AA);Cyc_Port_leq(_tmp4EB,({void*_tmp4EA=Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp26A[1U];({struct Cyc_Port_Cfield*_tmp4E6=({struct Cyc_Port_Cfield*_tmp26B=_cycalloc(sizeof(*_tmp26B));({void*_tmp4E5=Cyc_Port_var();_tmp26B->qual=_tmp4E5;});_tmp26B->f=_tmp2A9;_tmp26B->type=_tmp268;_tmp26B;});_tmp26A[0U]=_tmp4E6;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp26A,sizeof(struct Cyc_Port_Cfield*),1U));}));void*_tmp4E9=
Cyc_Port_var();
# 1512
void*_tmp4E8=
Cyc_Port_var();
# 1512
void*_tmp4E7=
Cyc_Port_var();
# 1512
Cyc_Port_ptr_ct(_tmp4EA,_tmp4E9,_tmp4E8,_tmp4E7,
Cyc_Port_var());}));});
return _tmp268;}case 34U: _LL268: _tmp2AC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp239)->f1).elt_type;_tmp2AB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp239)->f1).num_elts;_LL269:
# 1516
({void*_tmp4EC=Cyc_Port_gen_exp(0,env,_tmp2AB);Cyc_Port_leq(_tmp4EC,Cyc_Port_arith_ct());});{
void*_tmp26C=(unsigned int)_tmp2AC?Cyc_Port_type_to_ctype(env,*_tmp2AC): Cyc_Port_var();
return({void*_tmp4F0=_tmp26C;void*_tmp4EF=Cyc_Port_var();void*_tmp4EE=Cyc_Port_fat_ct();void*_tmp4ED=Cyc_Port_heap_ct();Cyc_Port_ptr_ct(_tmp4F0,_tmp4EF,_tmp4EE,_tmp4ED,Cyc_Port_var());});};case 2U: _LL26A: _LL26B:
({void*_tmp26D=0U;({struct _dyneither_ptr _tmp4F1=({const char*_tmp26E="Pragma_e";_tag_dyneither(_tmp26E,sizeof(char),9U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F1,_tag_dyneither(_tmp26D,sizeof(void*),0U));});});case 35U: _LL26C: _tmp2AE=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_tmp2AD=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp239)->f2;_LL26D:
({void*_tmp26F=0U;({struct _dyneither_ptr _tmp4F2=({const char*_tmp270="Swap_e";_tag_dyneither(_tmp270,sizeof(char),7U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F2,_tag_dyneither(_tmp26F,sizeof(void*),0U));});});case 16U: _LL26E: _LL26F:
({void*_tmp271=0U;({struct _dyneither_ptr _tmp4F3=({const char*_tmp272="New_e";_tag_dyneither(_tmp272,sizeof(char),6U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F3,_tag_dyneither(_tmp271,sizeof(void*),0U));});});case 24U: _LL270: _LL271:
({void*_tmp273=0U;({struct _dyneither_ptr _tmp4F4=({const char*_tmp274="Tuple_e";_tag_dyneither(_tmp274,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F4,_tag_dyneither(_tmp273,sizeof(void*),0U));});});case 25U: _LL272: _LL273:
({void*_tmp275=0U;({struct _dyneither_ptr _tmp4F5=({const char*_tmp276="CompoundLit_e";_tag_dyneither(_tmp276,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F5,_tag_dyneither(_tmp275,sizeof(void*),0U));});});case 26U: _LL274: _LL275:
({void*_tmp277=0U;({struct _dyneither_ptr _tmp4F6=({const char*_tmp278="Array_e";_tag_dyneither(_tmp278,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F6,_tag_dyneither(_tmp277,sizeof(void*),0U));});});case 27U: _LL276: _LL277:
({void*_tmp279=0U;({struct _dyneither_ptr _tmp4F7=({const char*_tmp27A="Comprehension_e";_tag_dyneither(_tmp27A,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F7,_tag_dyneither(_tmp279,sizeof(void*),0U));});});case 28U: _LL278: _LL279:
({void*_tmp27B=0U;({struct _dyneither_ptr _tmp4F8=({const char*_tmp27C="ComprehensionNoinit_e";_tag_dyneither(_tmp27C,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F8,_tag_dyneither(_tmp27B,sizeof(void*),0U));});});case 29U: _LL27A: _LL27B:
({void*_tmp27D=0U;({struct _dyneither_ptr _tmp4F9=({const char*_tmp27E="Aggregate_e";_tag_dyneither(_tmp27E,sizeof(char),12U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F9,_tag_dyneither(_tmp27D,sizeof(void*),0U));});});case 30U: _LL27C: _LL27D:
({void*_tmp27F=0U;({struct _dyneither_ptr _tmp4FA=({const char*_tmp280="AnonStruct_e";_tag_dyneither(_tmp280,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4FA,_tag_dyneither(_tmp27F,sizeof(void*),0U));});});case 31U: _LL27E: _LL27F:
({void*_tmp281=0U;({struct _dyneither_ptr _tmp4FB=({const char*_tmp282="Datatype_e";_tag_dyneither(_tmp282,sizeof(char),11U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4FB,_tag_dyneither(_tmp281,sizeof(void*),0U));});});case 36U: _LL280: _LL281:
({void*_tmp283=0U;({struct _dyneither_ptr _tmp4FC=({const char*_tmp284="UnresolvedMem_e";_tag_dyneither(_tmp284,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4FC,_tag_dyneither(_tmp283,sizeof(void*),0U));});});case 37U: _LL282: _tmp2AF=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp239)->f1;_LL283:
({void*_tmp285=0U;({struct _dyneither_ptr _tmp4FD=({const char*_tmp286="StmtExp_e";_tag_dyneither(_tmp286,sizeof(char),10U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4FD,_tag_dyneither(_tmp285,sizeof(void*),0U));});});case 39U: _LL284: _LL285:
({void*_tmp287=0U;({struct _dyneither_ptr _tmp4FE=({const char*_tmp288="Valueof_e";_tag_dyneither(_tmp288,sizeof(char),10U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4FE,_tag_dyneither(_tmp287,sizeof(void*),0U));});});case 40U: _LL286: _LL287:
({void*_tmp289=0U;({struct _dyneither_ptr _tmp4FF=({const char*_tmp28A="Asm_e";_tag_dyneither(_tmp28A,sizeof(char),6U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4FF,_tag_dyneither(_tmp289,sizeof(void*),0U));});});default: _LL288: _LL289:
({void*_tmp28B=0U;({struct _dyneither_ptr _tmp500=({const char*_tmp28C="Tagcheck_e";_tag_dyneither(_tmp28C,sizeof(char),11U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp500,_tag_dyneither(_tmp28B,sizeof(void*),0U));});});}_LL225:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1539
static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Stmt*s){
void*_tmp2B0=s->r;void*_tmp2B1=_tmp2B0;struct Cyc_Absyn_Stmt*_tmp2CB;struct Cyc_Absyn_Exp*_tmp2CA;struct Cyc_Absyn_Stmt*_tmp2C9;struct Cyc_Absyn_Decl*_tmp2C8;struct Cyc_Absyn_Stmt*_tmp2C7;struct Cyc_Absyn_Exp*_tmp2C6;struct Cyc_List_List*_tmp2C5;struct Cyc_Absyn_Exp*_tmp2C4;struct Cyc_Absyn_Exp*_tmp2C3;struct Cyc_Absyn_Exp*_tmp2C2;struct Cyc_Absyn_Stmt*_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C0;struct Cyc_Absyn_Stmt*_tmp2BF;struct Cyc_Absyn_Exp*_tmp2BE;struct Cyc_Absyn_Stmt*_tmp2BD;struct Cyc_Absyn_Stmt*_tmp2BC;struct Cyc_Absyn_Exp*_tmp2BB;struct Cyc_Absyn_Stmt*_tmp2BA;struct Cyc_Absyn_Stmt*_tmp2B9;struct Cyc_Absyn_Exp*_tmp2B8;switch(*((int*)_tmp2B1)){case 0U: _LL29A: _LL29B:
 return;case 1U: _LL29C: _tmp2B8=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp2B1)->f1;_LL29D:
 Cyc_Port_gen_exp(0,env,_tmp2B8);return;case 2U: _LL29E: _tmp2BA=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp2B1)->f1;_tmp2B9=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp2B1)->f2;_LL29F:
 Cyc_Port_gen_stmt(env,_tmp2BA);Cyc_Port_gen_stmt(env,_tmp2B9);return;case 3U: _LL2A0: _tmp2BB=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp2B1)->f1;_LL2A1: {
# 1545
void*_tmp2B2=Cyc_Port_lookup_return_type(env);
void*_tmp2B3=(unsigned int)_tmp2BB?Cyc_Port_gen_exp(0,env,_tmp2BB): Cyc_Port_void_ct();
Cyc_Port_leq(_tmp2B3,_tmp2B2);
return;}case 4U: _LL2A2: _tmp2BE=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp2B1)->f1;_tmp2BD=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp2B1)->f2;_tmp2BC=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp2B1)->f3;_LL2A3:
# 1550
({void*_tmp501=Cyc_Port_gen_exp(0,env,_tmp2BE);Cyc_Port_leq(_tmp501,Cyc_Port_arith_ct());});
Cyc_Port_gen_stmt(env,_tmp2BD);
Cyc_Port_gen_stmt(env,_tmp2BC);
return;case 5U: _LL2A4: _tmp2C0=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp2B1)->f1).f1;_tmp2BF=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp2B1)->f2;_LL2A5:
# 1555
({void*_tmp502=Cyc_Port_gen_exp(0,env,_tmp2C0);Cyc_Port_leq(_tmp502,Cyc_Port_arith_ct());});
Cyc_Port_gen_stmt(env,_tmp2BF);
return;case 6U: _LL2A6: _LL2A7:
 goto _LL2A9;case 7U: _LL2A8: _LL2A9:
 goto _LL2AB;case 8U: _LL2AA: _LL2AB:
 return;case 9U: _LL2AC: _tmp2C4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp2B1)->f1;_tmp2C3=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp2B1)->f2).f1;_tmp2C2=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp2B1)->f3).f1;_tmp2C1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp2B1)->f4;_LL2AD:
# 1562
 Cyc_Port_gen_exp(0,env,_tmp2C4);
({void*_tmp503=Cyc_Port_gen_exp(0,env,_tmp2C3);Cyc_Port_leq(_tmp503,Cyc_Port_arith_ct());});
Cyc_Port_gen_exp(0,env,_tmp2C2);
Cyc_Port_gen_stmt(env,_tmp2C1);
return;case 10U: _LL2AE: _tmp2C6=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp2B1)->f1;_tmp2C5=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp2B1)->f2;_LL2AF:
# 1568
({void*_tmp504=Cyc_Port_gen_exp(0,env,_tmp2C6);Cyc_Port_leq(_tmp504,Cyc_Port_arith_ct());});
for(0;(unsigned int)_tmp2C5;_tmp2C5=_tmp2C5->tl){
# 1571
Cyc_Port_gen_stmt(env,((struct Cyc_Absyn_Switch_clause*)_tmp2C5->hd)->body);}
# 1573
return;case 11U: _LL2B0: _LL2B1:
({void*_tmp2B4=0U;({struct _dyneither_ptr _tmp505=({const char*_tmp2B5="fallthru";_tag_dyneither(_tmp2B5,sizeof(char),9U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp505,_tag_dyneither(_tmp2B4,sizeof(void*),0U));});});case 12U: _LL2B2: _tmp2C8=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp2B1)->f1;_tmp2C7=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp2B1)->f2;_LL2B3:
# 1576
({struct Cyc_Port_Cenv*_tmp506=Cyc_Port_gen_localdecl(env,_tmp2C8,0);env=_tmp506;});Cyc_Port_gen_stmt(env,_tmp2C7);return;case 13U: _LL2B4: _tmp2C9=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp2B1)->f2;_LL2B5:
 Cyc_Port_gen_stmt(env,_tmp2C9);return;case 14U: _LL2B6: _tmp2CB=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp2B1)->f1;_tmp2CA=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp2B1)->f2).f1;_LL2B7:
# 1579
 Cyc_Port_gen_stmt(env,_tmp2CB);
({void*_tmp507=Cyc_Port_gen_exp(0,env,_tmp2CA);Cyc_Port_leq(_tmp507,Cyc_Port_arith_ct());});
return;default: _LL2B8: _LL2B9:
({void*_tmp2B6=0U;({struct _dyneither_ptr _tmp508=({const char*_tmp2B7="try/catch";_tag_dyneither(_tmp2B7,sizeof(char),10U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp508,_tag_dyneither(_tmp2B6,sizeof(void*),0U));});});}_LL299:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1587
static void*Cyc_Port_gen_initializer(struct Cyc_Port_Cenv*env,void*t,struct Cyc_Absyn_Exp*e){
void*_tmp2CC=e->r;void*_tmp2CD=_tmp2CC;struct Cyc_List_List*_tmp2F4;struct Cyc_List_List*_tmp2F3;struct Cyc_List_List*_tmp2F2;switch(*((int*)_tmp2CD)){case 36U: _LL2BB: _tmp2F2=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp2CD)->f2;_LL2BC:
 _tmp2F3=_tmp2F2;goto _LL2BE;case 26U: _LL2BD: _tmp2F3=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2CD)->f1;_LL2BE:
 _tmp2F4=_tmp2F3;goto _LL2C0;case 25U: _LL2BF: _tmp2F4=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp2CD)->f2;_LL2C0:
# 1592
 LOOP: {
void*_tmp2CE=t;struct Cyc_Absyn_Aggrdecl*_tmp2EF;struct _tuple0*_tmp2EE;void*_tmp2ED;union Cyc_Absyn_Constraint*_tmp2EC;unsigned int _tmp2EB;struct _tuple0*_tmp2EA;switch(*((int*)_tmp2CE)){case 17U: _LL2C8: _tmp2EA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2CE)->f1;_LL2C9:
# 1595
(*_tmp2EA).f1=Cyc_Absyn_Loc_n;
({void*_tmp509=Cyc_Port_lookup_typedef(env,_tmp2EA);t=_tmp509;});goto LOOP;case 8U: _LL2CA: _tmp2ED=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2CE)->f1).elt_type;_tmp2EC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2CE)->f1).zero_term;_tmp2EB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2CE)->f1).zt_loc;_LL2CB: {
# 1598
void*_tmp2CF=Cyc_Port_var();
void*_tmp2D0=Cyc_Port_var();
void*_tmp2D1=Cyc_Port_var();
void*_tmp2D2=Cyc_Port_type_to_ctype(env,_tmp2ED);
for(0;_tmp2F4 != 0;_tmp2F4=_tmp2F4->tl){
struct _tuple17 _tmp2D3=*((struct _tuple17*)_tmp2F4->hd);struct _tuple17 _tmp2D4=_tmp2D3;struct Cyc_List_List*_tmp2D9;struct Cyc_Absyn_Exp*_tmp2D8;_LL2D3: _tmp2D9=_tmp2D4.f1;_tmp2D8=_tmp2D4.f2;_LL2D4:;
if((unsigned int)_tmp2D9)({void*_tmp2D5=0U;({struct _dyneither_ptr _tmp50A=({const char*_tmp2D6="designators in initializers";_tag_dyneither(_tmp2D6,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp50A,_tag_dyneither(_tmp2D5,sizeof(void*),0U));});});{
void*_tmp2D7=Cyc_Port_gen_initializer(env,_tmp2ED,_tmp2D8);
Cyc_Port_leq(_tmp2D7,_tmp2CF);};}
# 1608
return Cyc_Port_array_ct(_tmp2CF,_tmp2D0,_tmp2D1);}case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2CE)->f1).aggr_info).UnknownAggr).tag == 1U){if(((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2CE)->f1).aggr_info).UnknownAggr).val).f1 == Cyc_Absyn_StructA){_LL2CC: _tmp2EE=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2CE)->f1).aggr_info).UnknownAggr).val).f2;_LL2CD:
# 1610
(*_tmp2EE).f1=Cyc_Absyn_Loc_n;{
struct _tuple11 _tmp2DA=*Cyc_Port_lookup_struct_decl(env,_tmp2EE);struct _tuple11 _tmp2DB=_tmp2DA;struct Cyc_Absyn_Aggrdecl*_tmp2DE;_LL2D6: _tmp2DE=_tmp2DB.f1;_LL2D7:;
if(_tmp2DE == 0)({void*_tmp2DC=0U;({struct _dyneither_ptr _tmp50B=({const char*_tmp2DD="struct is not yet defined";_tag_dyneither(_tmp2DD,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp50B,_tag_dyneither(_tmp2DC,sizeof(void*),0U));});});
_tmp2EF=_tmp2DE;goto _LL2CF;};}else{goto _LL2D0;}}else{_LL2CE: _tmp2EF=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2CE)->f1).aggr_info).KnownAggr).val;_LL2CF: {
# 1615
struct Cyc_List_List*_tmp2DF=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2EF->impl))->fields;
for(0;_tmp2F4 != 0;_tmp2F4=_tmp2F4->tl){
struct _tuple17 _tmp2E0=*((struct _tuple17*)_tmp2F4->hd);struct _tuple17 _tmp2E1=_tmp2E0;struct Cyc_List_List*_tmp2E7;struct Cyc_Absyn_Exp*_tmp2E6;_LL2D9: _tmp2E7=_tmp2E1.f1;_tmp2E6=_tmp2E1.f2;_LL2DA:;
if((unsigned int)_tmp2E7)({void*_tmp2E2=0U;({struct _dyneither_ptr _tmp50C=({const char*_tmp2E3="designators in initializers";_tag_dyneither(_tmp2E3,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp50C,_tag_dyneither(_tmp2E2,sizeof(void*),0U));});});{
struct Cyc_Absyn_Aggrfield*_tmp2E4=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp2DF))->hd;
_tmp2DF=_tmp2DF->tl;{
void*_tmp2E5=Cyc_Port_gen_initializer(env,_tmp2E4->type,_tmp2E6);;};};}
# 1623
return Cyc_Port_type_to_ctype(env,t);}}default: _LL2D0: _LL2D1:
({void*_tmp2E8=0U;({struct _dyneither_ptr _tmp50D=({const char*_tmp2E9="bad type for aggregate initializer";_tag_dyneither(_tmp2E9,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp50D,_tag_dyneither(_tmp2E8,sizeof(void*),0U));});});}_LL2C7:;}case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2CD)->f1).Wstring_c).tag){case 8U: _LL2C1: _LL2C2:
# 1626
 goto _LL2C4;case 9U: _LL2C3: _LL2C4:
# 1628
 LOOP2: {
void*_tmp2F0=t;struct _tuple0*_tmp2F1;switch(*((int*)_tmp2F0)){case 17U: _LL2DC: _tmp2F1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2F0)->f1;_LL2DD:
# 1631
(*_tmp2F1).f1=Cyc_Absyn_Loc_n;
({void*_tmp50E=Cyc_Port_lookup_typedef(env,_tmp2F1);t=_tmp50E;});goto LOOP2;case 8U: _LL2DE: _LL2DF:
 return({void*_tmp510=Cyc_Port_arith_ct();void*_tmp50F=Cyc_Port_const_ct();Cyc_Port_array_ct(_tmp510,_tmp50F,Cyc_Port_zterm_ct());});default: _LL2E0: _LL2E1:
 return Cyc_Port_gen_exp(0,env,e);}_LL2DB:;}default: goto _LL2C5;}default: _LL2C5: _LL2C6:
# 1636
 return Cyc_Port_gen_exp(0,env,e);}_LL2BA:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1641
static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d,int fromglobal){
void*_tmp2F5=d->r;void*_tmp2F6=_tmp2F5;struct Cyc_Absyn_Vardecl*_tmp2FA;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2F6)->tag == 0U){_LL2E3: _tmp2FA=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2F6)->f1;_LL2E4:
# 1644
 if(!fromglobal)Cyc_Port_register_localvar_decl(env,_tmp2FA->name,_tmp2FA->varloc,_tmp2FA->type,_tmp2FA->initializer);{
void*_tmp2F7=Cyc_Port_var();
void*q;
if((env->gcenv)->porting){
({void*_tmp511=Cyc_Port_var();q=_tmp511;});
({struct Cyc_Port_Cenv*_tmp514=env;void*_tmp513=q;void*_tmp512=
(_tmp2FA->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();
# 1649
Cyc_Port_register_const_cvar(_tmp514,_tmp513,_tmp512,(_tmp2FA->tq).loc);});}else{
# 1658
({void*_tmp515=(_tmp2FA->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();q=_tmp515;});}
# 1660
(*_tmp2FA->name).f1=Cyc_Absyn_Loc_n;
({struct Cyc_Port_Cenv*_tmp516=Cyc_Port_add_var(env,_tmp2FA->name,_tmp2FA->type,q,_tmp2F7,_tmp2FA->varloc);env=_tmp516;});
({void*_tmp517=_tmp2F7;Cyc_Port_unifies(_tmp517,Cyc_Port_type_to_ctype(env,_tmp2FA->type));});
if((unsigned int)_tmp2FA->initializer){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp2FA->initializer);
void*t2=Cyc_Port_gen_initializer(env,_tmp2FA->type,e);
Cyc_Port_leq(t2,_tmp2F7);}
# 1668
return env;};}else{_LL2E5: _LL2E6:
({void*_tmp2F8=0U;({struct _dyneither_ptr _tmp518=({const char*_tmp2F9="non-local decl that isn't a variable";_tag_dyneither(_tmp2F9,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp518,_tag_dyneither(_tmp2F8,sizeof(void*),0U));});});}_LL2E2:;}
# 1673
static struct _tuple8*Cyc_Port_make_targ(struct _tuple8*arg){
struct _tuple8 _tmp2FB=*arg;struct _tuple8 _tmp2FC=_tmp2FB;struct _dyneither_ptr*_tmp300;struct Cyc_Absyn_Tqual _tmp2FF;void*_tmp2FE;_LL2E8: _tmp300=_tmp2FC.f1;_tmp2FF=_tmp2FC.f2;_tmp2FE=_tmp2FC.f3;_LL2E9:;
return({struct _tuple8*_tmp2FD=_cycalloc(sizeof(*_tmp2FD));_tmp2FD->f1=0;_tmp2FD->f2=_tmp2FF;_tmp2FD->f3=_tmp2FE;_tmp2FD;});}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1678
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*env,struct Cyc_List_List*ds);struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct _tuple18{struct _dyneither_ptr*f1;void*f2;void*f3;void*f4;};struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cfield;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1680
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d){
void*_tmp301=d->r;void*_tmp302=_tmp301;struct Cyc_Absyn_Enumdecl*_tmp342;struct Cyc_Absyn_Aggrdecl*_tmp341;struct Cyc_Absyn_Typedefdecl*_tmp340;struct Cyc_Absyn_Fndecl*_tmp33F;struct Cyc_Absyn_Vardecl*_tmp33E;switch(*((int*)_tmp302)){case 13U: _LL2EB: _LL2EC:
# 1683
(env->gcenv)->porting=1;
return env;case 14U: _LL2ED: _LL2EE:
# 1686
(env->gcenv)->porting=0;
return env;case 0U: _LL2EF: _tmp33E=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp302)->f1;_LL2F0:
# 1689
(*_tmp33E->name).f1=Cyc_Absyn_Loc_n;
# 1693
if(Cyc_Port_declared_var(env,_tmp33E->name)){
struct _tuple13 _tmp303=Cyc_Port_lookup_var(env,_tmp33E->name);struct _tuple13 _tmp304=_tmp303;struct _tuple10 _tmp309;unsigned int _tmp308;_LL2FC: _tmp309=_tmp304.f1;_tmp308=_tmp304.f2;_LL2FD:;{
struct _tuple10 _tmp305=_tmp309;void*_tmp307;void*_tmp306;_LL2FF: _tmp307=_tmp305.f1;_tmp306=_tmp305.f2;_LL300:;{
void*q2;
if((env->gcenv)->porting  && !Cyc_Port_isfn(env,_tmp33E->name)){
({void*_tmp519=Cyc_Port_var();q2=_tmp519;});
({struct Cyc_Port_Cenv*_tmp51C=env;void*_tmp51B=q2;void*_tmp51A=
(_tmp33E->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();
# 1699
Cyc_Port_register_const_cvar(_tmp51C,_tmp51B,_tmp51A,(_tmp33E->tq).loc);});}else{
# 1708
({void*_tmp51D=(_tmp33E->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();q2=_tmp51D;});}
# 1710
Cyc_Port_unifies(_tmp307,q2);
({void*_tmp51E=_tmp306;Cyc_Port_unifies(_tmp51E,Cyc_Port_type_to_ctype(env,_tmp33E->type));});
if((unsigned int)_tmp33E->initializer){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp33E->initializer);
({void*_tmp51F=Cyc_Port_gen_initializer(env,_tmp33E->type,e);Cyc_Port_leq(_tmp51F,_tmp306);});}
# 1716
return env;};};}else{
# 1718
return Cyc_Port_gen_localdecl(env,d,1);}case 1U: _LL2F1: _tmp33F=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp302)->f1;_LL2F2:
# 1724
(*_tmp33F->name).f1=Cyc_Absyn_Loc_n;{
struct _tuple14*predeclared=0;
# 1727
if(Cyc_Port_declared_var(env,_tmp33F->name))
({struct _tuple14*_tmp520=Cyc_Port_lookup_full_var(env,_tmp33F->name);predeclared=_tmp520;});{
# 1730
void*_tmp30A=_tmp33F->ret_type;
struct Cyc_List_List*_tmp30B=_tmp33F->args;
struct Cyc_List_List*_tmp30C=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Port_make_targ,_tmp30B);
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp30D=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp328=_cycalloc(sizeof(*_tmp328));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp523=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp329;_tmp329.tag=9U;({struct Cyc_Absyn_FnInfo _tmp522=({struct Cyc_Absyn_FnInfo _tmp32A;_tmp32A.tvars=0;_tmp32A.effect=0;({struct Cyc_Absyn_Tqual _tmp521=
Cyc_Absyn_empty_tqual(0U);_tmp32A.ret_tqual=_tmp521;});_tmp32A.ret_typ=_tmp30A;_tmp32A.args=_tmp30C;_tmp32A.c_varargs=0;_tmp32A.cyc_varargs=0;_tmp32A.rgn_po=0;_tmp32A.attributes=0;_tmp32A.requires_clause=0;_tmp32A.requires_relns=0;_tmp32A.ensures_clause=0;_tmp32A.ensures_relns=0;_tmp32A;});_tmp329.f1=_tmp522;});_tmp329;});_tmp328[0]=_tmp523;});_tmp328;});
# 1737
struct Cyc_Port_Cenv*_tmp30E=Cyc_Port_set_cpos(env,Cyc_Port_FnBody_pos);
void*_tmp30F=Cyc_Port_type_to_ctype(_tmp30E,_tmp30A);
struct Cyc_List_List*c_args=0;
struct Cyc_List_List*c_arg_types=0;
{struct Cyc_List_List*_tmp310=_tmp30B;for(0;(unsigned int)_tmp310;_tmp310=_tmp310->tl){
struct _tuple8 _tmp311=*((struct _tuple8*)_tmp310->hd);struct _tuple8 _tmp312=_tmp311;struct _dyneither_ptr*_tmp319;struct Cyc_Absyn_Tqual _tmp318;void*_tmp317;_LL302: _tmp319=_tmp312.f1;_tmp318=_tmp312.f2;_tmp317=_tmp312.f3;_LL303:;{
# 1745
void*_tmp313=Cyc_Port_type_to_ctype(_tmp30E,_tmp317);
void*tqv;
if((env->gcenv)->porting){
({void*_tmp524=Cyc_Port_var();tqv=_tmp524;});
({struct Cyc_Port_Cenv*_tmp527=env;void*_tmp526=tqv;void*_tmp525=_tmp318.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_register_const_cvar(_tmp527,_tmp526,_tmp525,_tmp318.loc);});}else{
# 1757
({void*_tmp528=_tmp318.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();tqv=_tmp528;});}
# 1759
({struct Cyc_List_List*_tmp52A=({struct Cyc_List_List*_tmp314=_cycalloc(sizeof(*_tmp314));({struct _tuple18*_tmp529=({struct _tuple18*_tmp315=_cycalloc(sizeof(*_tmp315));_tmp315->f1=_tmp319;_tmp315->f2=_tmp317;_tmp315->f3=tqv;_tmp315->f4=_tmp313;_tmp315;});_tmp314->hd=_tmp529;});_tmp314->tl=c_args;_tmp314;});c_args=_tmp52A;});
({struct Cyc_List_List*_tmp52B=({struct Cyc_List_List*_tmp316=_cycalloc(sizeof(*_tmp316));_tmp316->hd=_tmp313;_tmp316->tl=c_arg_types;_tmp316;});c_arg_types=_tmp52B;});};}}
# 1762
({struct Cyc_List_List*_tmp52C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(c_args);c_args=_tmp52C;});
({struct Cyc_List_List*_tmp52D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(c_arg_types);c_arg_types=_tmp52D;});{
void*_tmp31A=Cyc_Port_fn_ct(_tmp30F,c_arg_types);
# 1768
(*_tmp33F->name).f1=Cyc_Absyn_Loc_n;
({struct Cyc_Port_Cenv*_tmp532=({struct Cyc_Port_Cenv*_tmp531=_tmp30E;struct _tuple0*_tmp530=_tmp33F->name;void*_tmp52F=(void*)_tmp30D;void*_tmp52E=Cyc_Port_const_ct();Cyc_Port_add_var(_tmp531,_tmp530,_tmp52F,_tmp52E,_tmp31A,0U);});_tmp30E=_tmp532;});
Cyc_Port_add_return_type(_tmp30E,_tmp30F);
{struct Cyc_List_List*_tmp31B=c_args;for(0;(unsigned int)_tmp31B;_tmp31B=_tmp31B->tl){
struct _tuple18 _tmp31C=*((struct _tuple18*)_tmp31B->hd);struct _tuple18 _tmp31D=_tmp31C;struct _dyneither_ptr*_tmp322;void*_tmp321;void*_tmp320;void*_tmp31F;_LL305: _tmp322=_tmp31D.f1;_tmp321=_tmp31D.f2;_tmp320=_tmp31D.f3;_tmp31F=_tmp31D.f4;_LL306:;
({struct Cyc_Port_Cenv*_tmp537=({struct Cyc_Port_Cenv*_tmp536=_tmp30E;struct _tuple0*_tmp535=({struct _tuple0*_tmp31E=_cycalloc(sizeof(*_tmp31E));_tmp31E->f1=Cyc_Absyn_Loc_n;_tmp31E->f2=_tmp322;_tmp31E;});void*_tmp534=_tmp321;void*_tmp533=_tmp320;Cyc_Port_add_var(_tmp536,_tmp535,_tmp534,_tmp533,_tmp31F,0U);});_tmp30E=_tmp537;});}}
# 1775
Cyc_Port_gen_stmt(_tmp30E,_tmp33F->body);
# 1780
Cyc_Port_generalize(0,_tmp31A);{
# 1787
struct Cyc_Dict_Dict counts=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);
Cyc_Port_region_counts(& counts,_tmp31A);
# 1791
Cyc_Port_register_rgns(env,counts,1,(void*)_tmp30D,_tmp31A);
({struct Cyc_Port_Cenv*_tmp53C=({struct Cyc_Port_Cenv*_tmp53B=_tmp30E;struct _tuple0*_tmp53A=_tmp33F->name;void*_tmp539=(void*)_tmp30D;void*_tmp538=Cyc_Port_const_ct();Cyc_Port_add_var(_tmp53B,_tmp53A,_tmp539,_tmp538,_tmp31A,0U);});env=_tmp53C;});
if((unsigned int)predeclared){
# 1800
struct _tuple14 _tmp323=*predeclared;struct _tuple14 _tmp324=_tmp323;void*_tmp327;void*_tmp326;void*_tmp325;_LL308: _tmp327=_tmp324.f1;_tmp326=(_tmp324.f2)->f1;_tmp325=(_tmp324.f2)->f2;_LL309:;
({void*_tmp53D=_tmp326;Cyc_Port_unifies(_tmp53D,Cyc_Port_const_ct());});
({void*_tmp53E=_tmp325;Cyc_Port_unifies(_tmp53E,Cyc_Port_instantiate(_tmp31A));});
# 1804
Cyc_Port_register_rgns(env,counts,1,_tmp327,_tmp31A);}
# 1806
return env;};};};};case 8U: _LL2F3: _tmp340=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp302)->f1;_LL2F4: {
# 1812
void*_tmp32B=(void*)_check_null(_tmp340->defn);
void*_tmp32C=Cyc_Port_type_to_ctype(env,_tmp32B);
(*_tmp340->name).f1=Cyc_Absyn_Loc_n;
Cyc_Port_add_typedef(env,_tmp340->name,_tmp32B,_tmp32C);
return env;}case 5U: _LL2F5: _tmp341=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp302)->f1;_LL2F6: {
# 1822
struct _tuple0*_tmp32D=_tmp341->name;
(*_tmp341->name).f1=Cyc_Absyn_Loc_n;{
struct _tuple11*previous;
struct Cyc_List_List*curr=0;
{enum Cyc_Absyn_AggrKind _tmp32E=_tmp341->kind;enum Cyc_Absyn_AggrKind _tmp32F=_tmp32E;if(_tmp32F == Cyc_Absyn_StructA){_LL30B: _LL30C:
# 1828
({struct _tuple11*_tmp53F=Cyc_Port_lookup_struct_decl(env,_tmp32D);previous=_tmp53F;});
goto _LL30A;}else{_LL30D: _LL30E:
# 1831
({struct _tuple11*_tmp540=Cyc_Port_lookup_union_decl(env,_tmp32D);previous=_tmp540;});
goto _LL30A;}_LL30A:;}
# 1834
if((unsigned int)_tmp341->impl){
struct Cyc_List_List*cf=(*previous).f2;
{struct Cyc_List_List*_tmp330=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp341->impl))->fields;for(0;(unsigned int)_tmp330;_tmp330=_tmp330->tl){
struct Cyc_Absyn_Aggrfield*_tmp331=(struct Cyc_Absyn_Aggrfield*)_tmp330->hd;
void*qv;
if((env->gcenv)->porting){
({void*_tmp541=Cyc_Port_var();qv=_tmp541;});
({struct Cyc_Port_Cenv*_tmp544=env;void*_tmp543=qv;void*_tmp542=
(_tmp331->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();
# 1841
Cyc_Port_register_const_cvar(_tmp544,_tmp543,_tmp542,(_tmp331->tq).loc);});}else{
# 1850
({void*_tmp545=(_tmp331->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();qv=_tmp545;});}{
# 1852
void*_tmp332=Cyc_Port_type_to_ctype(env,_tmp331->type);
if(cf != 0){
struct Cyc_Port_Cfield _tmp333=*((struct Cyc_Port_Cfield*)cf->hd);struct Cyc_Port_Cfield _tmp334=_tmp333;void*_tmp336;void*_tmp335;_LL310: _tmp336=_tmp334.qual;_tmp335=_tmp334.type;_LL311:;
cf=cf->tl;
Cyc_Port_unifies(qv,_tmp336);
Cyc_Port_unifies(_tmp332,_tmp335);}
# 1859
({struct Cyc_List_List*_tmp547=({struct Cyc_List_List*_tmp337=_cycalloc(sizeof(*_tmp337));({struct Cyc_Port_Cfield*_tmp546=({struct Cyc_Port_Cfield*_tmp338=_cycalloc(sizeof(*_tmp338));_tmp338->qual=qv;_tmp338->f=_tmp331->name;_tmp338->type=_tmp332;_tmp338;});_tmp337->hd=_tmp546;});_tmp337->tl=curr;_tmp337;});curr=_tmp547;});};}}
# 1861
({struct Cyc_List_List*_tmp548=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(curr);curr=_tmp548;});
if((*previous).f2 == 0)
(*previous).f2=curr;}
# 1866
return env;};}case 7U: _LL2F7: _tmp342=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp302)->f1;_LL2F8:
# 1871
(*_tmp342->name).f1=Cyc_Absyn_Loc_n;
# 1873
if((unsigned int)_tmp342->fields){
struct Cyc_List_List*_tmp339=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp342->fields))->v;for(0;(unsigned int)_tmp339;_tmp339=_tmp339->tl){
(*((struct Cyc_Absyn_Enumfield*)_tmp339->hd)->name).f1=Cyc_Absyn_Loc_n;
({struct Cyc_Port_Cenv*_tmp54E=({struct Cyc_Port_Cenv*_tmp54D=env;struct _tuple0*_tmp54C=((struct Cyc_Absyn_Enumfield*)_tmp339->hd)->name;void*_tmp54B=(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp33A=_cycalloc(sizeof(*_tmp33A));({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp549=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp33B;_tmp33B.tag=13U;_tmp33B.f1=_tmp342->name;_tmp33B.f2=_tmp342;_tmp33B;});_tmp33A[0]=_tmp549;});_tmp33A;});void*_tmp54A=
Cyc_Port_const_ct();
# 1876
Cyc_Port_add_var(_tmp54D,_tmp54C,_tmp54B,_tmp54A,
Cyc_Port_arith_ct(),0U);});
# 1876
env=_tmp54E;});}}
# 1879
return env;default: _LL2F9: _LL2FA:
# 1881
 if((env->gcenv)->porting)
({void*_tmp33C=0U;({struct Cyc___cycFILE*_tmp550=Cyc_stderr;struct _dyneither_ptr _tmp54F=({const char*_tmp33D="Warning: Cyclone declaration found in code to be ported -- skipping.";_tag_dyneither(_tmp33D,sizeof(char),69U);});Cyc_fprintf(_tmp550,_tmp54F,_tag_dyneither(_tmp33C,sizeof(void*),0U));});});
return env;}_LL2EA:;}struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;struct Cyc_Port_Cenv;
# 1888
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*env,struct Cyc_List_List*ds){
for(0;(unsigned int)ds;ds=ds->tl){
({struct Cyc_Port_Cenv*_tmp551=Cyc_Port_gen_topdecl(env,(struct Cyc_Absyn_Decl*)ds->hd);env=_tmp551;});}
return env;}struct Cyc_Port_Cenv;struct Cyc_Port_VarUsage;struct Cyc_Port_VarUsage;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_VarUsage;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;
# 1895
static struct Cyc_List_List*Cyc_Port_gen_edits(struct Cyc_List_List*ds){
# 1897
struct Cyc_Port_Cenv*env=({struct Cyc_Port_Cenv*_tmp552=Cyc_Port_initial_cenv();Cyc_Port_gen_topdecls(_tmp552,ds);});
# 1899
struct Cyc_List_List*_tmp343=(env->gcenv)->pointer_edits;
struct Cyc_List_List*_tmp344=(env->gcenv)->qualifier_edits;
struct Cyc_List_List*_tmp345=(env->gcenv)->zeroterm_edits;
struct Cyc_List_List*_tmp346=(env->gcenv)->edits;
struct Cyc_List_List*_tmp347=(env->gcenv)->vardecl_locs;
# 1905
for(0;(unsigned int)_tmp347;_tmp347=_tmp347->tl){
struct _tuple15 _tmp348=*((struct _tuple15*)_tmp347->hd);struct _tuple15 _tmp349=_tmp348;struct _tuple0*_tmp364;unsigned int _tmp363;void*_tmp362;_LL313: _tmp364=_tmp349.f1;_tmp363=_tmp349.f2;_tmp362=_tmp349.f3;_LL314:;{
struct _tuple0 _tmp34A=*_tmp364;struct _tuple0 _tmp34B=_tmp34A;struct _dyneither_ptr*_tmp361;_LL316: _tmp361=_tmp34B.f2;_LL317:;{
struct Cyc_Port_VarUsage*_tmp34C=((struct Cyc_Port_VarUsage*(*)(struct Cyc_Hashtable_Table*t,unsigned int key))Cyc_Hashtable_lookup)((env->gcenv)->varusage_tab,_tmp363);
if(((struct Cyc_Port_VarUsage*)_check_null(_tmp34C))->address_taken){
if((unsigned int)_tmp34C->init){
# 1912
({struct Cyc_List_List*_tmp556=({struct Cyc_List_List*_tmp34D=_cycalloc(sizeof(*_tmp34D));({struct Cyc_Port_Edit*_tmp555=({struct Cyc_Port_Edit*_tmp34E=_cycalloc(sizeof(*_tmp34E));_tmp34E->loc=_tmp363;({struct _dyneither_ptr _tmp553=({const char*_tmp350="@";_tag_dyneither(_tmp350,sizeof(char),2U);});_tmp34E->old_string=_tmp553;});({struct _dyneither_ptr _tmp554=({const char*_tmp34F="";_tag_dyneither(_tmp34F,sizeof(char),1U);});_tmp34E->new_string=_tmp554;});_tmp34E;});_tmp34D->hd=_tmp555;});_tmp34D->tl=_tmp346;_tmp34D;});_tmp346=_tmp556;});
({struct Cyc_List_List*_tmp55A=({struct Cyc_List_List*_tmp351=_cycalloc(sizeof(*_tmp351));({struct Cyc_Port_Edit*_tmp559=({struct Cyc_Port_Edit*_tmp352=_cycalloc(sizeof(*_tmp352));_tmp352->loc=((struct Cyc_Absyn_Exp*)_check_null(_tmp34C->init))->loc;({struct _dyneither_ptr _tmp557=({const char*_tmp354="new ";_tag_dyneither(_tmp354,sizeof(char),5U);});_tmp352->old_string=_tmp557;});({struct _dyneither_ptr _tmp558=({const char*_tmp353="";_tag_dyneither(_tmp353,sizeof(char),1U);});_tmp352->new_string=_tmp558;});_tmp352;});_tmp351->hd=_tmp559;});_tmp351->tl=_tmp346;_tmp351;});_tmp346=_tmp55A;});}else{
# 1917
({struct Cyc_List_List*_tmp55F=({struct Cyc_List_List*_tmp355=_cycalloc(sizeof(*_tmp355));({struct Cyc_Port_Edit*_tmp55E=({struct Cyc_Port_Edit*_tmp356=_cycalloc(sizeof(*_tmp356));_tmp356->loc=_tmp363;({struct _dyneither_ptr _tmp55D=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp35A;_tmp35A.tag=0U;({struct _dyneither_ptr _tmp55B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp362));_tmp35A.f1=_tmp55B;});({struct Cyc_String_pa_PrintArg_struct _tmp359;_tmp359.tag=0U;_tmp359.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp361);({void*_tmp357[2U]={& _tmp359,& _tmp35A};({struct _dyneither_ptr _tmp55C=({const char*_tmp358="?%s = malloc(sizeof(%s))";_tag_dyneither(_tmp358,sizeof(char),25U);});Cyc_aprintf(_tmp55C,_tag_dyneither(_tmp357,sizeof(void*),2U));});});});});_tmp356->old_string=_tmp55D;});_tmp356->new_string=*_tmp361;_tmp356;});_tmp355->hd=_tmp55E;});_tmp355->tl=_tmp346;_tmp355;});_tmp346=_tmp55F;});}{
# 1919
struct Cyc_List_List*_tmp35B=_tmp34C->usage_locs;
for(0;(unsigned int)_tmp35B;_tmp35B=_tmp35B->tl){
({struct Cyc_List_List*_tmp563=({struct Cyc_List_List*_tmp35C=_cycalloc(sizeof(*_tmp35C));({struct Cyc_Port_Edit*_tmp562=({struct Cyc_Port_Edit*_tmp35D=_cycalloc(sizeof(*_tmp35D));_tmp35D->loc=(unsigned int)_tmp35B->hd;({struct _dyneither_ptr _tmp561=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp360;_tmp360.tag=0U;_tmp360.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp361);({void*_tmp35E[1U]={& _tmp360};({struct _dyneither_ptr _tmp560=({const char*_tmp35F="(*%s)";_tag_dyneither(_tmp35F,sizeof(char),6U);});Cyc_aprintf(_tmp560,_tag_dyneither(_tmp35E,sizeof(void*),1U));});});});_tmp35D->old_string=_tmp561;});_tmp35D->new_string=*_tmp361;_tmp35D;});_tmp35C->hd=_tmp562;});_tmp35C->tl=_tmp346;_tmp35C;});_tmp346=_tmp563;});}};}};};}
# 1927
{struct Cyc_List_List*_tmp365=_tmp343;for(0;(unsigned int)_tmp365;_tmp365=_tmp365->tl){
struct _tuple16 _tmp366=*((struct _tuple16*)_tmp365->hd);struct _tuple16 _tmp367=_tmp366;void*_tmp36A;void*_tmp369;unsigned int _tmp368;_LL319: _tmp36A=_tmp367.f1;_tmp369=_tmp367.f2;_tmp368=_tmp367.f3;_LL31A:;
Cyc_Port_unifies(_tmp36A,_tmp369);}}
# 1931
{struct Cyc_List_List*_tmp36B=_tmp344;for(0;(unsigned int)_tmp36B;_tmp36B=_tmp36B->tl){
struct _tuple16 _tmp36C=*((struct _tuple16*)_tmp36B->hd);struct _tuple16 _tmp36D=_tmp36C;void*_tmp370;void*_tmp36F;unsigned int _tmp36E;_LL31C: _tmp370=_tmp36D.f1;_tmp36F=_tmp36D.f2;_tmp36E=_tmp36D.f3;_LL31D:;
Cyc_Port_unifies(_tmp370,_tmp36F);}}
# 1935
{struct Cyc_List_List*_tmp371=_tmp345;for(0;(unsigned int)_tmp371;_tmp371=_tmp371->tl){
struct _tuple16 _tmp372=*((struct _tuple16*)_tmp371->hd);struct _tuple16 _tmp373=_tmp372;void*_tmp376;void*_tmp375;unsigned int _tmp374;_LL31F: _tmp376=_tmp373.f1;_tmp375=_tmp373.f2;_tmp374=_tmp373.f3;_LL320:;
Cyc_Port_unifies(_tmp376,_tmp375);}}
# 1941
for(0;(unsigned int)_tmp343;_tmp343=_tmp343->tl){
struct _tuple16 _tmp377=*((struct _tuple16*)_tmp343->hd);struct _tuple16 _tmp378=_tmp377;void*_tmp385;void*_tmp384;unsigned int _tmp383;_LL322: _tmp385=_tmp378.f1;_tmp384=_tmp378.f2;_tmp383=_tmp378.f3;_LL323:;
if(!Cyc_Port_unifies(_tmp385,_tmp384) && (int)_tmp383){
void*_tmp379=Cyc_Port_compress_ct(_tmp384);void*_tmp37A=_tmp379;switch(*((int*)_tmp37A)){case 2U: _LL325: _LL326:
# 1946
({struct Cyc_List_List*_tmp567=({struct Cyc_List_List*_tmp37B=_cycalloc(sizeof(*_tmp37B));({struct Cyc_Port_Edit*_tmp566=({struct Cyc_Port_Edit*_tmp37C=_cycalloc(sizeof(*_tmp37C));_tmp37C->loc=_tmp383;({struct _dyneither_ptr _tmp564=({const char*_tmp37E="?";_tag_dyneither(_tmp37E,sizeof(char),2U);});_tmp37C->old_string=_tmp564;});({struct _dyneither_ptr _tmp565=({const char*_tmp37D="*";_tag_dyneither(_tmp37D,sizeof(char),2U);});_tmp37C->new_string=_tmp565;});_tmp37C;});_tmp37B->hd=_tmp566;});_tmp37B->tl=_tmp346;_tmp37B;});_tmp346=_tmp567;});
goto _LL324;case 3U: _LL327: _LL328:
# 1949
({struct Cyc_List_List*_tmp56B=({struct Cyc_List_List*_tmp37F=_cycalloc(sizeof(*_tmp37F));({struct Cyc_Port_Edit*_tmp56A=({struct Cyc_Port_Edit*_tmp380=_cycalloc(sizeof(*_tmp380));_tmp380->loc=_tmp383;({struct _dyneither_ptr _tmp568=({const char*_tmp382="*";_tag_dyneither(_tmp382,sizeof(char),2U);});_tmp380->old_string=_tmp568;});({struct _dyneither_ptr _tmp569=({const char*_tmp381="?";_tag_dyneither(_tmp381,sizeof(char),2U);});_tmp380->new_string=_tmp569;});_tmp380;});_tmp37F->hd=_tmp56A;});_tmp37F->tl=_tmp346;_tmp37F;});_tmp346=_tmp56B;});
goto _LL324;default: _LL329: _LL32A:
 goto _LL324;}_LL324:;}}
# 1956
for(0;(unsigned int)_tmp344;_tmp344=_tmp344->tl){
struct _tuple16 _tmp386=*((struct _tuple16*)_tmp344->hd);struct _tuple16 _tmp387=_tmp386;void*_tmp394;void*_tmp393;unsigned int _tmp392;_LL32C: _tmp394=_tmp387.f1;_tmp393=_tmp387.f2;_tmp392=_tmp387.f3;_LL32D:;
if(!Cyc_Port_unifies(_tmp394,_tmp393) && (int)_tmp392){
void*_tmp388=Cyc_Port_compress_ct(_tmp393);void*_tmp389=_tmp388;switch(*((int*)_tmp389)){case 1U: _LL32F: _LL330:
# 1961
({struct Cyc_List_List*_tmp56F=({struct Cyc_List_List*_tmp38A=_cycalloc(sizeof(*_tmp38A));({struct Cyc_Port_Edit*_tmp56E=({struct Cyc_Port_Edit*_tmp38B=_cycalloc(sizeof(*_tmp38B));_tmp38B->loc=_tmp392;({struct _dyneither_ptr _tmp56C=({const char*_tmp38D="const ";_tag_dyneither(_tmp38D,sizeof(char),7U);});_tmp38B->old_string=_tmp56C;});({struct _dyneither_ptr _tmp56D=({const char*_tmp38C="";_tag_dyneither(_tmp38C,sizeof(char),1U);});_tmp38B->new_string=_tmp56D;});_tmp38B;});_tmp38A->hd=_tmp56E;});_tmp38A->tl=_tmp346;_tmp38A;});_tmp346=_tmp56F;});goto _LL32E;case 0U: _LL331: _LL332:
# 1963
({struct Cyc_List_List*_tmp573=({struct Cyc_List_List*_tmp38E=_cycalloc(sizeof(*_tmp38E));({struct Cyc_Port_Edit*_tmp572=({struct Cyc_Port_Edit*_tmp38F=_cycalloc(sizeof(*_tmp38F));_tmp38F->loc=_tmp392;({struct _dyneither_ptr _tmp570=({const char*_tmp391="";_tag_dyneither(_tmp391,sizeof(char),1U);});_tmp38F->old_string=_tmp570;});({struct _dyneither_ptr _tmp571=({const char*_tmp390="const ";_tag_dyneither(_tmp390,sizeof(char),7U);});_tmp38F->new_string=_tmp571;});_tmp38F;});_tmp38E->hd=_tmp572;});_tmp38E->tl=_tmp346;_tmp38E;});_tmp346=_tmp573;});goto _LL32E;default: _LL333: _LL334:
 goto _LL32E;}_LL32E:;}}
# 1969
for(0;(unsigned int)_tmp345;_tmp345=_tmp345->tl){
struct _tuple16 _tmp395=*((struct _tuple16*)_tmp345->hd);struct _tuple16 _tmp396=_tmp395;void*_tmp3A3;void*_tmp3A2;unsigned int _tmp3A1;_LL336: _tmp3A3=_tmp396.f1;_tmp3A2=_tmp396.f2;_tmp3A1=_tmp396.f3;_LL337:;
if(!Cyc_Port_unifies(_tmp3A3,_tmp3A2) && (int)_tmp3A1){
void*_tmp397=Cyc_Port_compress_ct(_tmp3A2);void*_tmp398=_tmp397;switch(*((int*)_tmp398)){case 8U: _LL339: _LL33A:
# 1974
({struct Cyc_List_List*_tmp577=({struct Cyc_List_List*_tmp399=_cycalloc(sizeof(*_tmp399));({struct Cyc_Port_Edit*_tmp576=({struct Cyc_Port_Edit*_tmp39A=_cycalloc(sizeof(*_tmp39A));_tmp39A->loc=_tmp3A1;({struct _dyneither_ptr _tmp574=({const char*_tmp39C="@nozeroterm ";_tag_dyneither(_tmp39C,sizeof(char),13U);});_tmp39A->old_string=_tmp574;});({struct _dyneither_ptr _tmp575=({const char*_tmp39B="";_tag_dyneither(_tmp39B,sizeof(char),1U);});_tmp39A->new_string=_tmp575;});_tmp39A;});_tmp399->hd=_tmp576;});_tmp399->tl=_tmp346;_tmp399;});_tmp346=_tmp577;});goto _LL338;case 9U: _LL33B: _LL33C:
# 1976
({struct Cyc_List_List*_tmp57B=({struct Cyc_List_List*_tmp39D=_cycalloc(sizeof(*_tmp39D));({struct Cyc_Port_Edit*_tmp57A=({struct Cyc_Port_Edit*_tmp39E=_cycalloc(sizeof(*_tmp39E));_tmp39E->loc=_tmp3A1;({struct _dyneither_ptr _tmp578=({const char*_tmp3A0="@zeroterm ";_tag_dyneither(_tmp3A0,sizeof(char),11U);});_tmp39E->old_string=_tmp578;});({struct _dyneither_ptr _tmp579=({const char*_tmp39F="";_tag_dyneither(_tmp39F,sizeof(char),1U);});_tmp39E->new_string=_tmp579;});_tmp39E;});_tmp39D->hd=_tmp57A;});_tmp39D->tl=_tmp346;_tmp39D;});_tmp346=_tmp57B;});goto _LL338;default: _LL33D: _LL33E:
 goto _LL338;}_LL338:;}}
# 1983
({struct Cyc_List_List*_tmp57C=((struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_Port_Edit*,struct Cyc_Port_Edit*),struct Cyc_List_List*x))Cyc_List_merge_sort)(Cyc_Port_cmp_edit,_tmp346);_tmp346=_tmp57C;});
# 1985
while((unsigned int)_tmp346  && ((struct Cyc_Port_Edit*)_tmp346->hd)->loc == 0){
# 1989
_tmp346=_tmp346->tl;}
# 1991
return _tmp346;}struct Cyc_Port_Edit;struct Cyc_Port_Edit;
# 1994
static unsigned int Cyc_Port_get_seg(struct Cyc_Port_Edit*e){
return e->loc;}struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;struct Cyc_Port_Edit;
# 2000
void Cyc_Port_port(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp3A4=Cyc_Port_gen_edits(ds);
struct Cyc_List_List*_tmp3A5=((struct Cyc_List_List*(*)(unsigned int(*f)(struct Cyc_Port_Edit*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Port_get_seg,_tmp3A4);
Cyc_Position_use_gcc_style_location=0;{
struct Cyc_List_List*_tmp3A6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Position_strings_of_segments(_tmp3A5));
for(0;(unsigned int)_tmp3A4;(_tmp3A4=_tmp3A4->tl,_tmp3A6=_tmp3A6->tl)){
struct Cyc_Port_Edit _tmp3A7=*((struct Cyc_Port_Edit*)_tmp3A4->hd);struct Cyc_Port_Edit _tmp3A8=_tmp3A7;unsigned int _tmp3B0;struct _dyneither_ptr _tmp3AF;struct _dyneither_ptr _tmp3AE;_LL340: _tmp3B0=_tmp3A8.loc;_tmp3AF=_tmp3A8.old_string;_tmp3AE=_tmp3A8.new_string;_LL341:;{
struct _dyneither_ptr sloc=(struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp3A6))->hd);
({struct Cyc_String_pa_PrintArg_struct _tmp3AD;_tmp3AD.tag=0U;_tmp3AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3AE);({struct Cyc_String_pa_PrintArg_struct _tmp3AC;_tmp3AC.tag=0U;_tmp3AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3AF);({struct Cyc_String_pa_PrintArg_struct _tmp3AB;_tmp3AB.tag=0U;_tmp3AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sloc);({void*_tmp3A9[3U]={& _tmp3AB,& _tmp3AC,& _tmp3AD};({struct _dyneither_ptr _tmp57D=({const char*_tmp3AA="%s: insert `%s' for `%s'\n";_tag_dyneither(_tmp3AA,sizeof(char),26U);});Cyc_printf(_tmp57D,_tag_dyneither(_tmp3A9,sizeof(void*),3U));});});});});});};}};}
