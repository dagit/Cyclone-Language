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
 struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 86 "list.h"
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 247
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 936 "absyn.h"
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations.h"
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_Relations_RelnOp Cyc_Relations_RType(void*);union Cyc_Relations_RelnOp Cyc_Relations_RParam(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RParamNumelts(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RReturn();
# 49
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct _tuple10{struct Cyc_Absyn_Exp*f1;enum Cyc_Relations_Relation f2;struct Cyc_Absyn_Exp*f3;};
# 63
struct _tuple10 Cyc_Relations_primop2relation(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e2);
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
# 94
struct Cyc_List_List*Cyc_Relations_duplicate_relation(struct _RegionHandle*rgn,struct Cyc_Absyn_Exp*e_old,struct Cyc_Absyn_Exp*e_new,struct Cyc_List_List*relns);
# 99
struct Cyc_List_List*Cyc_Relations_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
# 102
struct Cyc_List_List*Cyc_Relations_reln_assign_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 105
struct Cyc_List_List*Cyc_Relations_reln_kill_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
# 107
struct Cyc_List_List*Cyc_Relations_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 109
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
# 111
int Cyc_Relations_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);
# 113
int Cyc_Relations_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*r1s);
# 115
struct Cyc_List_List*Cyc_Relations_join_relns(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*);
# 117
void Cyc_Relations_print_relns(struct Cyc___cycFILE*,struct Cyc_List_List*);
# 119
struct _dyneither_ptr Cyc_Relations_reln2string(struct Cyc_Relations_Reln*r);
struct _dyneither_ptr Cyc_Relations_rop2string(union Cyc_Relations_RelnOp r);
struct _dyneither_ptr Cyc_Relations_relation2string(enum Cyc_Relations_Relation r);
struct _dyneither_ptr Cyc_Relations_relns2string(struct Cyc_List_List*r);
# 128
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 32
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 34
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 55
void*Cyc_Tcutil_compress(void*t);
# 145
int Cyc_Tcutil_unify(void*,void*);
# 309 "tcutil.h"
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 348
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);struct _union_Node_NZero{int tag;int val;};struct _union_Node_NVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_Node_NNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_Node_NType{int tag;void*val;};struct _union_Node_NParam{int tag;unsigned int val;};struct _union_Node_NParamNumelts{int tag;unsigned int val;};struct _union_Node_NReturn{int tag;int val;};union Cyc_Pratt_Node{struct _union_Node_NZero NZero;struct _union_Node_NVar NVar;struct _union_Node_NNumelts NNumelts;struct _union_Node_NType NType;struct _union_Node_NParam NParam;struct _union_Node_NParamNumelts NParamNumelts;struct _union_Node_NReturn NReturn;};
# 60 "pratt.h"
extern union Cyc_Pratt_Node Cyc_Pratt_zero_node;
# 62
union Cyc_Pratt_Node Cyc_Pratt_NVar(struct Cyc_Absyn_Vardecl*);
union Cyc_Pratt_Node Cyc_Pratt_NType(void*);
union Cyc_Pratt_Node Cyc_Pratt_NNumelts(struct Cyc_Absyn_Vardecl*);
union Cyc_Pratt_Node Cyc_Pratt_NParam(unsigned int);
union Cyc_Pratt_Node Cyc_Pratt_NParamNumelts(unsigned int);
union Cyc_Pratt_Node Cyc_Pratt_NReturn();struct Cyc_Pratt_Graph;
# 75
struct Cyc_Pratt_Graph*Cyc_Pratt_empty_graph();
struct Cyc_Pratt_Graph*Cyc_Pratt_copy_graph(struct Cyc_Pratt_Graph*);
# 80
struct Cyc_Pratt_Graph*Cyc_Pratt_add_edge(struct Cyc_Pratt_Graph*G,union Cyc_Pratt_Node i,union Cyc_Pratt_Node j,int a);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 64 "string.h"
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 31 "relations.cyc"
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int i){
return({union Cyc_Relations_RelnOp _tmp0;(_tmp0.RConst).val=i;(_tmp0.RConst).tag=1;_tmp0;});}
# 34
union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*v){
return({union Cyc_Relations_RelnOp _tmp1;(_tmp1.RVar).val=v;(_tmp1.RVar).tag=2;_tmp1;});}
# 37
union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*v){
return({union Cyc_Relations_RelnOp _tmp2;(_tmp2.RNumelts).val=v;(_tmp2.RNumelts).tag=3;_tmp2;});}
# 40
union Cyc_Relations_RelnOp Cyc_Relations_RType(void*t){
return({union Cyc_Relations_RelnOp _tmp3;({void*_tmpEB=Cyc_Tcutil_compress(t);(_tmp3.RType).val=_tmpEB;});(_tmp3.RType).tag=4;_tmp3;});}
# 43
union Cyc_Relations_RelnOp Cyc_Relations_RParam(unsigned int i){
return({union Cyc_Relations_RelnOp _tmp4;(_tmp4.RParam).val=i;(_tmp4.RParam).tag=5;_tmp4;});}
# 46
union Cyc_Relations_RelnOp Cyc_Relations_RParamNumelts(unsigned int i){
return({union Cyc_Relations_RelnOp _tmp5;(_tmp5.RParamNumelts).val=i;(_tmp5.RParamNumelts).tag=6;_tmp5;});}
# 49
union Cyc_Relations_RelnOp Cyc_Relations_RReturn(){
return({union Cyc_Relations_RelnOp _tmp6;(_tmp6.RReturn).val=0U;(_tmp6.RReturn).tag=7;_tmp6;});}struct _tuple12{union Cyc_Relations_RelnOp f1;union Cyc_Relations_RelnOp f2;};
# 53
static int Cyc_Relations_same_relop(union Cyc_Relations_RelnOp r1,union Cyc_Relations_RelnOp r2){
struct _tuple12 _tmp7=({struct _tuple12 _tmp15;_tmp15.f1=r1;_tmp15.f2=r2;_tmp15;});struct _tuple12 _tmp8=_tmp7;unsigned int _tmp14;unsigned int _tmp13;unsigned int _tmp12;unsigned int _tmp11;void*_tmp10;void*_tmpF;struct Cyc_Absyn_Vardecl*_tmpE;struct Cyc_Absyn_Vardecl*_tmpD;struct Cyc_Absyn_Vardecl*_tmpC;struct Cyc_Absyn_Vardecl*_tmpB;unsigned int _tmpA;unsigned int _tmp9;switch(((_tmp8.f1).RParamNumelts).tag){case 1U: if(((_tmp8.f2).RConst).tag == 1){_LL1: _tmpA=((_tmp8.f1).RConst).val;_tmp9=((_tmp8.f2).RConst).val;_LL2:
 return _tmpA == _tmp9;}else{goto _LLF;}case 2U: if(((_tmp8.f2).RVar).tag == 2){_LL3: _tmpC=((_tmp8.f1).RVar).val;_tmpB=((_tmp8.f2).RVar).val;_LL4:
 return _tmpC == _tmpB;}else{goto _LLF;}case 3U: if(((_tmp8.f2).RNumelts).tag == 3){_LL5: _tmpE=((_tmp8.f1).RNumelts).val;_tmpD=((_tmp8.f2).RNumelts).val;_LL6:
 return _tmpE == _tmpD;}else{goto _LLF;}case 4U: if(((_tmp8.f2).RType).tag == 4){_LL7: _tmp10=((_tmp8.f1).RType).val;_tmpF=((_tmp8.f2).RType).val;_LL8:
 return Cyc_Tcutil_unify(_tmp10,_tmpF);}else{goto _LLF;}case 5U: if(((_tmp8.f2).RParam).tag == 5){_LL9: _tmp12=((_tmp8.f1).RParam).val;_tmp11=((_tmp8.f2).RParam).val;_LLA:
 return _tmp12 == _tmp11;}else{goto _LLF;}case 6U: if(((_tmp8.f2).RParamNumelts).tag == 6){_LLB: _tmp14=((_tmp8.f1).RParamNumelts).val;_tmp13=((_tmp8.f2).RParamNumelts).val;_LLC:
 return _tmp14 == _tmp13;}else{goto _LLF;}default: if(((_tmp8.f2).RReturn).tag == 7){_LLD: _LLE:
 return 1;}else{_LLF: _LL10:
 return 0;}}_LL0:;}
# 66
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation relation,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns){
# 70
{struct Cyc_List_List*_tmp16=relns;for(0;_tmp16 != 0;_tmp16=_tmp16->tl){
struct Cyc_Relations_Reln*_tmp17=(struct Cyc_Relations_Reln*)_tmp16->hd;
if((Cyc_Relations_same_relop(_tmp17->rop1,rop1) && _tmp17->relation == relation) && 
Cyc_Relations_same_relop(_tmp17->rop2,rop2))return relns;}}
# 75
return({struct Cyc_List_List*_tmp18=_region_malloc(rgn,sizeof(*_tmp18));({struct Cyc_Relations_Reln*_tmpEC=({struct Cyc_Relations_Reln*_tmp19=_region_malloc(rgn,sizeof(*_tmp19));_tmp19->rop1=rop1;_tmp19->relation=relation;_tmp19->rop2=rop2;_tmp19;});_tmp18->hd=_tmpEC;});_tmp18->tl=relns;_tmp18;});}
# 78
struct Cyc_List_List*Cyc_Relations_duplicate_relation(struct _RegionHandle*rgn,struct Cyc_Absyn_Exp*e_old,struct Cyc_Absyn_Exp*e_new,struct Cyc_List_List*relns){
# 81
union Cyc_Relations_RelnOp rop_old=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp rop_new=Cyc_Relations_RConst(0U);
if(!Cyc_Relations_exp2relnop(e_old,& rop_old))return relns;
if(!Cyc_Relations_exp2relnop(e_new,& rop_new)){
({void*_tmp1A=0U;({unsigned int _tmpEE=e_new->loc;struct _dyneither_ptr _tmpED=({const char*_tmp1B="Could not construct relation";_tag_dyneither(_tmp1B,sizeof(char),29U);});Cyc_Tcutil_warn(_tmpEE,_tmpED,_tag_dyneither(_tmp1A,sizeof(void*),0U));});});
return relns;}
# 88
{struct Cyc_List_List*_tmp1C=relns;for(0;_tmp1C != 0;_tmp1C=_tmp1C->tl){
struct Cyc_Relations_Reln*_tmp1D=(struct Cyc_Relations_Reln*)_tmp1C->hd;
union Cyc_Relations_RelnOp _tmp1E=_tmp1D->rop1;
union Cyc_Relations_RelnOp _tmp1F=_tmp1D->rop2;
int addIt=0;
if(Cyc_Relations_same_relop(_tmp1E,rop_old)){
addIt=1;
_tmp1E=rop_new;}
# 97
if(Cyc_Relations_same_relop(_tmp1F,rop_old)){
addIt=1;
_tmp1F=rop_new;}
# 101
if(addIt)
({struct Cyc_List_List*_tmpF0=({struct Cyc_List_List*_tmp20=_region_malloc(rgn,sizeof(*_tmp20));({struct Cyc_Relations_Reln*_tmpEF=({struct Cyc_Relations_Reln*_tmp21=_region_malloc(rgn,sizeof(*_tmp21));_tmp21->rop1=_tmp1E;_tmp21->relation=_tmp1D->relation;_tmp21->rop2=_tmp1F;_tmp21;});_tmp20->hd=_tmpEF;});_tmp20->tl=relns;_tmp20;});relns=_tmpF0;});}}
# 104
return relns;}
# 107
int Cyc_Relations_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*r1s){
if(r1s == r2s)return 1;
# 111
for(0;r1s != 0;r1s=r1s->tl){
struct Cyc_Relations_Reln*_tmp22=(struct Cyc_Relations_Reln*)r1s->hd;
int found=0;
{struct Cyc_List_List*_tmp23=r2s;for(0;_tmp23 != 0;_tmp23=_tmp23->tl){
struct Cyc_Relations_Reln*_tmp24=(struct Cyc_Relations_Reln*)_tmp23->hd;
if(_tmp22 == _tmp24  || (Cyc_Relations_same_relop(_tmp22->rop1,_tmp24->rop1) && _tmp22->relation == _tmp24->relation) && 
# 118
Cyc_Relations_same_relop(_tmp22->rop2,_tmp24->rop2)){
found=1;
break;}}}
# 123
if(!found)
return 0;}
# 126
return 1;}
# 129
struct Cyc_List_List*Cyc_Relations_join_relns(struct _RegionHandle*r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){
if(r1s == r2s)return r1s;{
struct Cyc_List_List*res=0;
int diff=0;
{struct Cyc_List_List*_tmp25=r1s;for(0;_tmp25 != 0;_tmp25=_tmp25->tl){
struct Cyc_Relations_Reln*_tmp26=(struct Cyc_Relations_Reln*)_tmp25->hd;
int found=0;
{struct Cyc_List_List*_tmp27=r2s;for(0;_tmp27 != 0;_tmp27=_tmp27->tl){
struct Cyc_Relations_Reln*_tmp28=(struct Cyc_Relations_Reln*)_tmp27->hd;
if(_tmp26 == _tmp28  || (Cyc_Relations_same_relop(_tmp26->rop1,_tmp28->rop1) && _tmp26->relation == _tmp28->relation) && 
# 140
Cyc_Relations_same_relop(_tmp26->rop2,_tmp28->rop2)){
({struct Cyc_List_List*_tmpF1=({struct Cyc_List_List*_tmp29=_region_malloc(r,sizeof(*_tmp29));_tmp29->hd=_tmp26;_tmp29->tl=res;_tmp29;});res=_tmpF1;});
found=1;
break;}}}
# 146
if(!found)diff=1;}}
# 148
if(!diff)res=r1s;
return res;};}
# 152
static int Cyc_Relations_rop_contains_var(union Cyc_Relations_RelnOp r,struct Cyc_Absyn_Vardecl*v){
union Cyc_Relations_RelnOp _tmp2A=r;struct Cyc_Absyn_Vardecl*_tmp2C;struct Cyc_Absyn_Vardecl*_tmp2B;switch((_tmp2A.RReturn).tag){case 2U: _LL1: _tmp2B=(_tmp2A.RVar).val;_LL2:
 return v == _tmp2B;case 3U: _LL3: _tmp2C=(_tmp2A.RNumelts).val;_LL4:
 return v == _tmp2C;case 4U: _LL5: _LL6:
 goto _LL8;case 5U: _LL7: _LL8:
 goto _LLA;case 6U: _LL9: _LLA:
 goto _LLC;case 7U: _LLB: _LLC:
 goto _LLE;default: _LLD: _LLE:
 return 0;}_LL0:;}
# 164
struct Cyc_List_List*Cyc_Relations_reln_kill_var(struct _RegionHandle*rgn,struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){
struct Cyc_List_List*p;
int found=0;
for(p=rs;!found  && p != 0;p=p->tl){
struct Cyc_Relations_Reln*_tmp2D=(struct Cyc_Relations_Reln*)p->hd;
if(Cyc_Relations_rop_contains_var(_tmp2D->rop1,v) || Cyc_Relations_rop_contains_var(_tmp2D->rop2,v)){
found=1;
break;}}
# 174
if(!found)return rs;{
# 176
struct Cyc_List_List*_tmp2E=0;
for(p=rs;p != 0;p=p->tl){
struct Cyc_Relations_Reln*_tmp2F=(struct Cyc_Relations_Reln*)p->hd;
if(Cyc_Relations_rop_contains_var(_tmp2F->rop1,v) || Cyc_Relations_rop_contains_var(_tmp2F->rop2,v))continue;
({struct Cyc_List_List*_tmpF2=({struct Cyc_List_List*_tmp30=_region_malloc(rgn,sizeof(*_tmp30));_tmp30->hd=_tmp2F;_tmp30->tl=_tmp2E;_tmp30;});_tmp2E=_tmpF2;});}
# 182
return _tmp2E;};}
# 188
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p){
# 191
RETRY:
 if(e->topt != 0){
void*_tmp31=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp32=_tmp31;struct Cyc_Absyn_Exp*_tmp34;void*_tmp33;switch(*((int*)_tmp32)){case 19U: _LL1: _tmp33=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp32)->f1;_LL2:
# 195
({union Cyc_Relations_RelnOp _tmpF3=Cyc_Relations_RType(_tmp33);*p=_tmpF3;});return 1;
goto _LL0;case 18U: _LL3: _tmp34=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp32)->f1;_LL4:
# 198
 e=_tmp34;
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 203
{void*_tmp35=e->r;void*_tmp36=_tmp35;void*_tmp40;void*_tmp3F;int _tmp3E;void*_tmp3D;struct Cyc_Absyn_Exp*_tmp3C;switch(*((int*)_tmp36)){case 14U: _LL8: _tmp3C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp36)->f2;_LL9:
 return Cyc_Relations_exp2relnop(_tmp3C,p);case 1U: _LLA: _tmp3D=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp36)->f1;_LLB: {
# 206
struct Cyc_Absyn_Vardecl*_tmp37=Cyc_Tcutil_nonesc_vardecl(_tmp3D);
if(_tmp37 != 0){
({union Cyc_Relations_RelnOp _tmpF4=Cyc_Relations_RVar(_tmp37);*p=_tmpF4;});
return 1;}
# 211
goto _LL7;}case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp36)->f1).Int_c).tag == 5){_LLC: _tmp3E=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp36)->f1).Int_c).val).f2;_LLD:
# 213
({union Cyc_Relations_RelnOp _tmpF5=Cyc_Relations_RConst((unsigned int)_tmp3E);*p=_tmpF5;});
return 1;}else{goto _LL12;}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp36)->f1 == Cyc_Absyn_Numelts){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp36)->f2 != 0){if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp36)->f2)->hd)->r)->tag == 1U){_LLE: _tmp3F=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp36)->f2)->hd)->r)->f1;_LLF: {
# 216
struct Cyc_Absyn_Vardecl*_tmp38=Cyc_Tcutil_nonesc_vardecl(_tmp3F);
if(_tmp38 != 0){
({union Cyc_Relations_RelnOp _tmpF6=Cyc_Relations_RNumelts(_tmp38);*p=_tmpF6;});
return 1;}
# 221
goto _LL7;}}else{goto _LL12;}}else{goto _LL12;}}else{goto _LL12;}case 39U: _LL10: _tmp40=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp36)->f1;_LL11:
# 223
{void*_tmp39=Cyc_Tcutil_compress(_tmp40);void*_tmp3A=_tmp39;struct Cyc_Absyn_Exp*_tmp3B;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3A)->tag == 18U){_LL15: _tmp3B=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3A)->f1;_LL16:
# 225
 e=_tmp3B;
goto RETRY;}else{_LL17: _LL18:
# 228
({union Cyc_Relations_RelnOp _tmpF7=Cyc_Relations_RType(_tmp40);*p=_tmpF7;});
return 1;}_LL14:;}
# 231
goto _LL7;default: _LL12: _LL13:
 goto _LL7;}_LL7:;}
# 235
if(Cyc_Tcutil_is_const_exp(e)){
struct _tuple11 _tmp41=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple11 _tmp42=_tmp41;unsigned int _tmp44;int _tmp43;_LL1A: _tmp44=_tmp42.f1;_tmp43=_tmp42.f2;_LL1B:;
if(_tmp43){
({union Cyc_Relations_RelnOp _tmpF8=Cyc_Relations_RConst(_tmp44);*p=_tmpF8;});
return 1;}}
# 242
return 0;}
# 245
struct Cyc_List_List*Cyc_Relations_reln_kill_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){
void*_tmp45=e->r;void*_tmp46=_tmp45;void*_tmp48;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->tag == 1U){_LL1: _tmp48=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->f1;_LL2: {
# 248
struct Cyc_Absyn_Vardecl*_tmp47=Cyc_Tcutil_nonesc_vardecl(_tmp48);
if(_tmp47 != 0)
return Cyc_Relations_reln_kill_var(rgn,r,_tmp47);else{
return r;}}}else{_LL3: _LL4:
 return r;}_LL0:;}
# 256
struct Cyc_Relations_Reln*Cyc_Relations_copy_reln(struct _RegionHandle*r2,struct Cyc_Relations_Reln*r){
return({struct Cyc_Relations_Reln*_tmp49=_region_malloc(r2,sizeof(*_tmp49));_tmp49[0]=*r;_tmp49;});}
# 260
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*r2,struct Cyc_List_List*relns){
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Relations_Reln*(*f)(struct _RegionHandle*,struct Cyc_Relations_Reln*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r2,Cyc_Relations_copy_reln,r2,relns);}
# 264
int Cyc_Relations_same_relns(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r1 != 0  && r2 != 0;(r1=r1->tl,r2=r2->tl)){
struct Cyc_Relations_Reln*_tmp4A=(struct Cyc_Relations_Reln*)r1->hd;
struct Cyc_Relations_Reln*_tmp4B=(struct Cyc_Relations_Reln*)r2->hd;
if((!Cyc_Relations_same_relop(_tmp4A->rop1,_tmp4B->rop1) || _tmp4A->relation != _tmp4B->relation) || !
# 270
Cyc_Relations_same_relop(_tmp4A->rop2,_tmp4B->rop2))return 0;}
# 272
if(r1 != 0  || r2 != 0)return 0;else{
return 1;}}
# 276
struct Cyc_List_List*Cyc_Relations_reln_assign_var(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){
# 280
if(v->escapes)return r;
# 283
({struct Cyc_List_List*_tmpF9=Cyc_Relations_reln_kill_var(rgn,r,v);r=_tmpF9;});
# 288
{void*_tmp4C=e->r;void*_tmp4D=_tmp4C;struct Cyc_Absyn_Exp*_tmp53;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4D)->tag == 34U){if((((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4D)->f1).fat_result == 1){_LL1: _tmp53=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4D)->f1).num_elts;_LL2:
# 291
 malloc_loop: {
void*_tmp4E=_tmp53->r;void*_tmp4F=_tmp4E;void*_tmp52;struct Cyc_Absyn_Exp*_tmp51;switch(*((int*)_tmp4F)){case 14U: _LL6: _tmp51=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4F)->f2;_LL7:
 _tmp53=_tmp51;goto malloc_loop;case 1U: _LL8: _tmp52=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4F)->f1;_LL9: {
# 295
struct Cyc_Absyn_Vardecl*_tmp50=Cyc_Tcutil_nonesc_vardecl(_tmp52);
if(_tmp50 != 0)
return({struct _RegionHandle*_tmpFC=rgn;union Cyc_Relations_RelnOp _tmpFB=Cyc_Relations_RVar(_tmp50);union Cyc_Relations_RelnOp _tmpFA=Cyc_Relations_RNumelts(v);Cyc_Relations_add_relation(_tmpFC,_tmpFB,Cyc_Relations_Req,_tmpFA,r);});else{
return r;}}default: _LLA: _LLB:
 return r;}_LL5:;}}else{goto _LL3;}}else{_LL3: _LL4:
# 302
 goto _LL0;}_LL0:;}
# 305
{void*_tmp54=Cyc_Tcutil_compress(v->type);void*_tmp55=_tmp54;switch(*((int*)_tmp55)){case 6U: _LLD: _LLE:
 goto _LLC;case 19U: _LLF: _LL10:
 goto _LLC;default: _LL11: _LL12:
 return r;}_LLC:;}{
# 311
union Cyc_Relations_RelnOp eop=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(e,& eop))
return({struct _RegionHandle*_tmpFF=rgn;union Cyc_Relations_RelnOp _tmpFE=Cyc_Relations_RVar(v);union Cyc_Relations_RelnOp _tmpFD=eop;Cyc_Relations_add_relation(_tmpFF,_tmpFE,Cyc_Relations_Req,_tmpFD,r);});
# 315
return r;};}
# 318
struct Cyc_List_List*Cyc_Relations_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmp56=e1->r;void*_tmp57=_tmp56;void*_tmp59;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp57)->tag == 1U){_LL1: _tmp59=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp57)->f1;_LL2: {
# 321
struct Cyc_Absyn_Vardecl*_tmp58=Cyc_Tcutil_nonesc_vardecl(_tmp59);
if(_tmp58 != 0)
return Cyc_Relations_reln_assign_var(rgn,r,_tmp58,e2);else{
return r;}}}else{_LL3: _LL4:
 return r;}_LL0:;}
# 330
static union Cyc_Pratt_Node Cyc_Relations_rop2node(union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp5A=r;unsigned int _tmp61;unsigned int _tmp60;void*_tmp5F;struct Cyc_Absyn_Vardecl*_tmp5E;struct Cyc_Absyn_Vardecl*_tmp5D;switch((_tmp5A.RReturn).tag){case 2U: _LL1: _tmp5D=(_tmp5A.RVar).val;_LL2:
 return Cyc_Pratt_NVar(_tmp5D);case 3U: _LL3: _tmp5E=(_tmp5A.RNumelts).val;_LL4:
 return Cyc_Pratt_NNumelts(_tmp5E);case 4U: _LL5: _tmp5F=(_tmp5A.RType).val;_LL6:
 return Cyc_Pratt_NType(_tmp5F);case 5U: _LL7: _tmp60=(_tmp5A.RParam).val;_LL8:
 return Cyc_Pratt_NParam(_tmp60);case 6U: _LL9: _tmp61=(_tmp5A.RParamNumelts).val;_LLA:
 return Cyc_Pratt_NParamNumelts(_tmp61);case 7U: _LLB: _LLC:
 return Cyc_Pratt_NReturn();default: _LLD: _LLE:
({void*_tmp5B=0U;({struct _dyneither_ptr _tmp100=({const char*_tmp5C="rop2node";_tag_dyneither(_tmp5C,sizeof(char),9U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp100,_tag_dyneither(_tmp5B,sizeof(void*),0U));});});}_LL0:;}
# 342
struct _dyneither_ptr Cyc_Relations_relation2string(enum Cyc_Relations_Relation r){
enum Cyc_Relations_Relation _tmp62=r;switch(_tmp62){case Cyc_Relations_Req: _LL1: _LL2:
 return({const char*_tmp63="==";_tag_dyneither(_tmp63,sizeof(char),3U);});case Cyc_Relations_Rneq: _LL3: _LL4:
 return({const char*_tmp64="!=";_tag_dyneither(_tmp64,sizeof(char),3U);});case Cyc_Relations_Rlt: _LL5: _LL6:
 return({const char*_tmp65="<";_tag_dyneither(_tmp65,sizeof(char),2U);});default: _LL7: _LL8:
 return({const char*_tmp66="<=";_tag_dyneither(_tmp66,sizeof(char),3U);});}_LL0:;}
# 351
struct _dyneither_ptr Cyc_Relations_rop2string(union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp67=r;unsigned int _tmp7E;unsigned int _tmp7D;struct Cyc_Absyn_Vardecl*_tmp7C;void*_tmp7B;struct Cyc_Absyn_Vardecl*_tmp7A;unsigned int _tmp79;switch((_tmp67.RParamNumelts).tag){case 1U: _LL1: _tmp79=(_tmp67.RConst).val;_LL2:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp6A;_tmp6A.tag=1U;_tmp6A.f1=(unsigned long)((int)_tmp79);({void*_tmp68[1U]={& _tmp6A};({struct _dyneither_ptr _tmp101=({const char*_tmp69="%d";_tag_dyneither(_tmp69,sizeof(char),3U);});Cyc_aprintf(_tmp101,_tag_dyneither(_tmp68,sizeof(void*),1U));});});});case 2U: _LL3: _tmp7A=(_tmp67.RVar).val;_LL4:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp6D;_tmp6D.tag=0U;({struct _dyneither_ptr _tmp102=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7A->name));_tmp6D.f1=_tmp102;});({void*_tmp6B[1U]={& _tmp6D};({struct _dyneither_ptr _tmp103=({const char*_tmp6C="%s";_tag_dyneither(_tmp6C,sizeof(char),3U);});Cyc_aprintf(_tmp103,_tag_dyneither(_tmp6B,sizeof(void*),1U));});});});case 4U: _LL5: _tmp7B=(_tmp67.RType).val;_LL6:
 return Cyc_Absynpp_typ2string(_tmp7B);case 3U: _LL7: _tmp7C=(_tmp67.RNumelts).val;_LL8:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp70;_tmp70.tag=0U;({struct _dyneither_ptr _tmp104=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7C->name));_tmp70.f1=_tmp104;});({void*_tmp6E[1U]={& _tmp70};({struct _dyneither_ptr _tmp105=({const char*_tmp6F="numelts(%s)";_tag_dyneither(_tmp6F,sizeof(char),12U);});Cyc_aprintf(_tmp105,_tag_dyneither(_tmp6E,sizeof(void*),1U));});});});case 5U: _LL9: _tmp7D=(_tmp67.RParam).val;_LLA:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp73;_tmp73.tag=1U;_tmp73.f1=(unsigned long)((int)_tmp7D);({void*_tmp71[1U]={& _tmp73};({struct _dyneither_ptr _tmp106=({const char*_tmp72="param(%d)";_tag_dyneither(_tmp72,sizeof(char),10U);});Cyc_aprintf(_tmp106,_tag_dyneither(_tmp71,sizeof(void*),1U));});});});case 6U: _LLB: _tmp7E=(_tmp67.RParamNumelts).val;_LLC:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp76;_tmp76.tag=1U;_tmp76.f1=(unsigned long)((int)_tmp7E);({void*_tmp74[1U]={& _tmp76};({struct _dyneither_ptr _tmp107=({const char*_tmp75="numelts(param(%d))";_tag_dyneither(_tmp75,sizeof(char),19U);});Cyc_aprintf(_tmp107,_tag_dyneither(_tmp74,sizeof(void*),1U));});});});default: _LLD: _LLE:
 return(struct _dyneither_ptr)({void*_tmp77=0U;({struct _dyneither_ptr _tmp108=({const char*_tmp78="return_value";_tag_dyneither(_tmp78,sizeof(char),13U);});Cyc_aprintf(_tmp108,_tag_dyneither(_tmp77,sizeof(void*),0U));});});}_LL0:;}
# 363
struct _dyneither_ptr Cyc_Relations_reln2string(struct Cyc_Relations_Reln*r){
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp83;_tmp83.tag=0U;({struct _dyneither_ptr _tmp109=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Relations_rop2string(r->rop2));_tmp83.f1=_tmp109;});({struct Cyc_String_pa_PrintArg_struct _tmp82;_tmp82.tag=0U;({struct _dyneither_ptr _tmp10A=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 364
Cyc_Relations_relation2string(r->relation));_tmp82.f1=_tmp10A;});({struct Cyc_String_pa_PrintArg_struct _tmp81;_tmp81.tag=0U;({struct _dyneither_ptr _tmp10B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Relations_rop2string(r->rop1));_tmp81.f1=_tmp10B;});({void*_tmp7F[3U]={& _tmp81,& _tmp82,& _tmp83};({struct _dyneither_ptr _tmp10C=({const char*_tmp80="%s %s %s";_tag_dyneither(_tmp80,sizeof(char),9U);});Cyc_aprintf(_tmp10C,_tag_dyneither(_tmp7F,sizeof(void*),3U));});});});});});}
# 368
struct _dyneither_ptr Cyc_Relations_relns2string(struct Cyc_List_List*rs){
if(rs == 0)return({const char*_tmp84="true";_tag_dyneither(_tmp84,sizeof(char),5U);});{
struct Cyc_List_List*ss=0;
for(0;rs != 0;rs=rs->tl){
({struct Cyc_List_List*_tmp10F=({struct Cyc_List_List*_tmp85=_cycalloc(sizeof(*_tmp85));({struct _dyneither_ptr*_tmp10E=({struct _dyneither_ptr*_tmp86=_cycalloc(sizeof(*_tmp86));({struct _dyneither_ptr _tmp10D=Cyc_Relations_reln2string((struct Cyc_Relations_Reln*)rs->hd);_tmp86[0]=_tmp10D;});_tmp86;});_tmp85->hd=_tmp10E;});_tmp85->tl=ss;_tmp85;});ss=_tmp10F;});
if(rs->tl != 0)({struct Cyc_List_List*_tmp111=({struct Cyc_List_List*_tmp87=_cycalloc(sizeof(*_tmp87));({struct _dyneither_ptr*_tmp110=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr))," && ",sizeof(char),5U);_tmp87->hd=_tmp110;});_tmp87->tl=ss;_tmp87;});ss=_tmp111;});}
# 375
return(struct _dyneither_ptr)Cyc_strconcat_l(ss);};}
# 378
void Cyc_Relations_print_relns(struct Cyc___cycFILE*f,struct Cyc_List_List*r){
for(0;r != 0;r=r->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp8B;_tmp8B.tag=0U;({struct _dyneither_ptr _tmp112=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Relations_reln2string((struct Cyc_Relations_Reln*)r->hd));_tmp8B.f1=_tmp112;});({void*_tmp89[1U]={& _tmp8B};({struct Cyc___cycFILE*_tmp114=f;struct _dyneither_ptr _tmp113=({const char*_tmp8A="%s";_tag_dyneither(_tmp8A,sizeof(char),3U);});Cyc_fprintf(_tmp114,_tmp113,_tag_dyneither(_tmp89,sizeof(void*),1U));});});});
if(r->tl != 0)({void*_tmp8C=0U;({struct Cyc___cycFILE*_tmp116=f;struct _dyneither_ptr _tmp115=({const char*_tmp8D=",";_tag_dyneither(_tmp8D,sizeof(char),2U);});Cyc_fprintf(_tmp116,_tmp115,_tag_dyneither(_tmp8C,sizeof(void*),0U));});});}}
# 385
static union Cyc_Relations_RelnOp Cyc_Relations_reduce_relnop(union Cyc_Relations_RelnOp rop){
{union Cyc_Relations_RelnOp _tmp8E=rop;struct Cyc_Absyn_Vardecl*_tmp99;void*_tmp98;switch((_tmp8E.RNumelts).tag){case 4U: _LL1: _tmp98=(_tmp8E.RType).val;_LL2:
# 388
{void*_tmp8F=Cyc_Tcutil_compress(_tmp98);void*_tmp90=_tmp8F;struct Cyc_Absyn_Exp*_tmp91;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp90)->tag == 18U){_LL8: _tmp91=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp90)->f1;_LL9: {
# 390
union Cyc_Relations_RelnOp rop2=rop;
if(Cyc_Relations_exp2relnop(_tmp91,& rop2))return rop2;
goto _LL7;}}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}
# 395
goto _LL0;case 3U: _LL3: _tmp99=(_tmp8E.RNumelts).val;_LL4:
# 397
{void*_tmp92=Cyc_Tcutil_compress(_tmp99->type);void*_tmp93=_tmp92;union Cyc_Absyn_Constraint*_tmp97;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->tag == 5U){_LLD: _tmp97=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).ptr_atts).bounds;_LLE:
# 399
{void*_tmp94=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp97);void*_tmp95=_tmp94;struct Cyc_Absyn_Exp*_tmp96;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp95)->tag == 1U){_LL12: _tmp96=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp95)->f1;_LL13: {
# 401
union Cyc_Relations_RelnOp rop2=rop;
if(Cyc_Relations_exp2relnop(_tmp96,& rop2))return rop2;
goto _LL11;}}else{_LL14: _LL15:
 goto _LL11;}_LL11:;}
# 406
goto _LLC;}else{_LLF: _LL10:
 goto _LLC;}_LLC:;}
# 409
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 412
return rop;}
# 417
static int Cyc_Relations_consist_relations(struct Cyc_Pratt_Graph*G,struct Cyc_List_List*rlns){
for(0;(unsigned int)G  && rlns != 0;rlns=rlns->tl){
# 424
struct Cyc_Relations_Reln*_tmp9A=(struct Cyc_Relations_Reln*)rlns->hd;
({union Cyc_Relations_RelnOp _tmp117=Cyc_Relations_reduce_relnop(_tmp9A->rop1);_tmp9A->rop1=_tmp117;});
({union Cyc_Relations_RelnOp _tmp118=Cyc_Relations_reduce_relnop(_tmp9A->rop2);_tmp9A->rop2=_tmp118;});{
struct Cyc_Relations_Reln*_tmp9B=_tmp9A;union Cyc_Relations_RelnOp _tmpB5;enum Cyc_Relations_Relation _tmpB4;union Cyc_Relations_RelnOp _tmpB3;union Cyc_Relations_RelnOp _tmpB2;enum Cyc_Relations_Relation _tmpB1;unsigned int _tmpB0;union Cyc_Relations_RelnOp _tmpAF;union Cyc_Relations_RelnOp _tmpAE;union Cyc_Relations_RelnOp _tmpAD;unsigned int _tmpAC;unsigned int _tmpAB;enum Cyc_Relations_Relation _tmpAA;union Cyc_Relations_RelnOp _tmpA9;unsigned int _tmpA8;union Cyc_Relations_RelnOp _tmpA7;unsigned int _tmpA6;enum Cyc_Relations_Relation _tmpA5;unsigned int _tmpA4;if(((((struct Cyc_Relations_Reln*)_tmp9B)->rop1).RConst).tag == 1){if(((((struct Cyc_Relations_Reln*)_tmp9B)->rop2).RConst).tag == 1){_LL1: _tmpA6=((_tmp9B->rop1).RConst).val;_tmpA5=_tmp9B->relation;_tmpA4=((_tmp9B->rop2).RConst).val;_LL2:
# 429
{enum Cyc_Relations_Relation _tmp9C=_tmpA5;switch(_tmp9C){case Cyc_Relations_Req: _LL10: _LL11:
 if(_tmpA6 != _tmpA4)return 0;goto _LLF;case Cyc_Relations_Rneq: _LL12: _LL13:
 if(_tmpA6 == _tmpA4)return 0;goto _LLF;case Cyc_Relations_Rlt: _LL14: _LL15:
 if(_tmpA6 >= _tmpA4)return 0;goto _LLF;default: _LL16: _LL17:
 if(_tmpA6 > _tmpA4)return 0;goto _LLF;}_LLF:;}
# 435
goto _LL0;}else{if(((struct Cyc_Relations_Reln*)_tmp9B)->relation == Cyc_Relations_Rneq){_LL5: _tmpA8=((_tmp9B->rop1).RConst).val;_tmpA7=_tmp9B->rop2;_LL6: {
# 442
union Cyc_Pratt_Node _tmp9D=Cyc_Relations_rop2node(_tmpA7);
struct Cyc_Pratt_Graph*G2=Cyc_Pratt_copy_graph(G);
({struct Cyc_Pratt_Graph*_tmp119=Cyc_Pratt_add_edge(G2,_tmp9D,Cyc_Pratt_zero_node,(int)(_tmpA8 - 1));G2=_tmp119;});
({struct Cyc_Pratt_Graph*_tmp11A=Cyc_Pratt_add_edge(G,Cyc_Pratt_zero_node,_tmp9D,(int)(-(_tmpA8 + 1)));G=_tmp11A;});
# 452
if(G != 0  && G2 != 0)
return Cyc_Relations_consist_relations(G,rlns->tl) || 
Cyc_Relations_consist_relations(G2,rlns->tl);else{
if(G == 0)
G=G2;}
goto _LL0;}}else{_LLB: _tmpAB=((_tmp9B->rop1).RConst).val;_tmpAA=_tmp9B->relation;_tmpA9=_tmp9B->rop2;_LLC: {
# 486
union Cyc_Pratt_Node _tmpA1=Cyc_Relations_rop2node(_tmpA9);
if(_tmpAA == Cyc_Relations_Rlt)_tmpAB=_tmpAB + 1;
({struct Cyc_Pratt_Graph*_tmp11B=Cyc_Pratt_add_edge(G,Cyc_Pratt_zero_node,_tmpA1,(int)(- _tmpAB));G=_tmp11B;});
# 491
if((unsigned int)G  && _tmpAA == Cyc_Relations_Req)
# 496
({struct Cyc_Pratt_Graph*_tmp11C=Cyc_Pratt_add_edge(G,_tmpA1,Cyc_Pratt_zero_node,(int)_tmpAB);G=_tmp11C;});
# 503
goto _LL0;}}}}else{if(((struct Cyc_Relations_Reln*)_tmp9B)->relation == Cyc_Relations_Rneq){if(((((struct Cyc_Relations_Reln*)_tmp9B)->rop2).RConst).tag == 1){_LL3: _tmpAD=_tmp9B->rop1;_tmpAC=((_tmp9B->rop2).RConst).val;_LL4:
# 440
 _tmpA8=_tmpAC;_tmpA7=_tmpAD;goto _LL6;}else{_LL7: _tmpAF=_tmp9B->rop1;_tmpAE=_tmp9B->rop2;_LL8: {
# 459
union Cyc_Pratt_Node _tmp9E=Cyc_Relations_rop2node(_tmpAF);
union Cyc_Pratt_Node _tmp9F=Cyc_Relations_rop2node(_tmpAE);
struct Cyc_Pratt_Graph*G2=Cyc_Pratt_copy_graph(G);
({struct Cyc_Pratt_Graph*_tmp11D=Cyc_Pratt_add_edge(G2,_tmp9E,_tmp9F,- 1);G2=_tmp11D;});
({struct Cyc_Pratt_Graph*_tmp11E=Cyc_Pratt_add_edge(G,_tmp9F,_tmp9E,- 1);G=_tmp11E;});
if(G != 0  && G2 != 0)
return Cyc_Relations_consist_relations(G,rlns->tl) || 
Cyc_Relations_consist_relations(G2,rlns->tl);else{
if(G == 0)
G=G2;}
goto _LL0;}}}else{if(((((struct Cyc_Relations_Reln*)_tmp9B)->rop2).RConst).tag == 1){_LL9: _tmpB2=_tmp9B->rop1;_tmpB1=_tmp9B->relation;_tmpB0=((_tmp9B->rop2).RConst).val;_LLA: {
# 473
union Cyc_Pratt_Node _tmpA0=Cyc_Relations_rop2node(_tmpB2);
if(_tmpB1 == Cyc_Relations_Rlt)_tmpB0=_tmpB0 - 1;
({struct Cyc_Pratt_Graph*_tmp11F=Cyc_Pratt_add_edge(G,_tmpA0,Cyc_Pratt_zero_node,(int)_tmpB0);G=_tmp11F;});
# 477
if((unsigned int)G  && _tmpB1 == Cyc_Relations_Req)
({struct Cyc_Pratt_Graph*_tmp120=Cyc_Pratt_add_edge(G,Cyc_Pratt_zero_node,_tmpA0,(int)(- _tmpB0));G=_tmp120;});
goto _LL0;}}else{_LLD: _tmpB5=_tmp9B->rop1;_tmpB4=_tmp9B->relation;_tmpB3=_tmp9B->rop2;_LLE: {
# 506
union Cyc_Pratt_Node _tmpA2=Cyc_Relations_rop2node(_tmpB5);
union Cyc_Pratt_Node _tmpA3=Cyc_Relations_rop2node(_tmpB3);
int i=_tmpB4 == Cyc_Relations_Rlt?- 1: 0;
# 511
({struct Cyc_Pratt_Graph*_tmp121=Cyc_Pratt_add_edge(G,_tmpA2,_tmpA3,i);G=_tmp121;});
# 513
if((unsigned int)G  && _tmpB4 == Cyc_Relations_Req)
({struct Cyc_Pratt_Graph*_tmp122=Cyc_Pratt_add_edge(G,_tmpA3,_tmpA2,i);G=_tmp122;});
goto _LL0;}}}}_LL0:;};}
# 518
if(G != 0)return 1;else{return 0;}}
# 526
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns){
# 532
struct Cyc_Pratt_Graph*G=Cyc_Pratt_empty_graph();
return Cyc_Relations_consist_relations(G,rlns);}
# 540
struct _tuple10 Cyc_Relations_primop2relation(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e2){
# 542
enum Cyc_Absyn_Primop _tmpB6=p;switch(_tmpB6){case Cyc_Absyn_Eq: _LL1: _LL2:
 return({struct _tuple10 _tmpB7;_tmpB7.f1=e1;_tmpB7.f2=Cyc_Relations_Req;_tmpB7.f3=e2;_tmpB7;});case Cyc_Absyn_Neq: _LL3: _LL4:
 return({struct _tuple10 _tmpB8;_tmpB8.f1=e1;_tmpB8.f2=Cyc_Relations_Rneq;_tmpB8.f3=e2;_tmpB8;});case Cyc_Absyn_Lt: _LL5: _LL6:
 return({struct _tuple10 _tmpB9;_tmpB9.f1=e1;_tmpB9.f2=Cyc_Relations_Rlt;_tmpB9.f3=e2;_tmpB9;});case Cyc_Absyn_Lte: _LL7: _LL8:
 return({struct _tuple10 _tmpBA;_tmpBA.f1=e1;_tmpBA.f2=Cyc_Relations_Rlte;_tmpBA.f3=e2;_tmpBA;});case Cyc_Absyn_Gt: _LL9: _LLA:
 return({struct _tuple10 _tmpBB;_tmpBB.f1=e2;_tmpBB.f2=Cyc_Relations_Rlt;_tmpBB.f3=e1;_tmpBB;});case Cyc_Absyn_Gte: _LLB: _LLC:
 return({struct _tuple10 _tmpBC;_tmpBC.f1=e2;_tmpBC.f2=Cyc_Relations_Rlte;_tmpBC.f3=e1;_tmpBC;});default: _LLD: _LLE:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpBD=_cycalloc(sizeof(*_tmpBD));({struct Cyc_Core_Impossible_exn_struct _tmp124=({struct Cyc_Core_Impossible_exn_struct _tmpBE;_tmpBE.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp123=({const char*_tmpBF="primop2relation";_tag_dyneither(_tmpBF,sizeof(char),16U);});_tmpBE.f1=_tmp123;});_tmpBE;});_tmpBD[0]=_tmp124;});_tmpBD;}));}_LL0:;}
# 554
enum Cyc_Relations_Relation Cyc_Relations_flip_relation(enum Cyc_Relations_Relation r){
enum Cyc_Relations_Relation _tmpC0=r;switch(_tmpC0){case Cyc_Relations_Req: _LL1: _LL2:
 return Cyc_Relations_Rneq;case Cyc_Relations_Rneq: _LL3: _LL4:
 return Cyc_Relations_Req;case Cyc_Relations_Rlt: _LL5: _LL6:
 return Cyc_Relations_Rlte;default: _LL7: _LL8:
 return Cyc_Relations_Rlt;}_LL0:;}
# 563
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*r,struct Cyc_Relations_Reln*rln){
return({struct Cyc_Relations_Reln*_tmpC1=_region_malloc(r,sizeof(*_tmpC1));_tmpC1->rop1=rln->rop2;({enum Cyc_Relations_Relation _tmp125=Cyc_Relations_flip_relation(rln->relation);_tmpC1->relation=_tmp125;});_tmpC1->rop2=rln->rop1;_tmpC1;});}
# 574
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
# 576
{void*_tmpC2=e->r;void*_tmpC3=_tmpC2;enum Cyc_Absyn_Primop _tmpDF;struct Cyc_Absyn_Exp*_tmpDE;struct Cyc_Absyn_Exp*_tmpDD;enum Cyc_Absyn_Primop _tmpDC;struct Cyc_Absyn_Exp*_tmpDB;struct Cyc_Absyn_Exp*_tmpDA;struct Cyc_Absyn_Exp*_tmpD9;struct Cyc_Absyn_Exp*_tmpD8;void*_tmpD7;struct Cyc_Absyn_Exp*_tmpD6;switch(*((int*)_tmpC3)){case 14U: _LL1: _tmpD7=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC3)->f1;_tmpD6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC3)->f2;_LL2:
# 579
{void*_tmpC4=Cyc_Tcutil_compress(_tmpD7);void*_tmpC5=_tmpC4;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpC5)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpC5)->f2 == Cyc_Absyn_Int_sz){_LLC: _LLD:
 return Cyc_Relations_exp2relns(r,_tmpD6);}else{goto _LLE;}}else{_LLE: _LLF:
 goto _LLB;}_LLB:;}
# 583
goto _LL0;case 7U: _LL3: _tmpD9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpC3)->f1;_tmpD8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpC3)->f2;_LL4:
 return({struct _RegionHandle*_tmp127=r;struct Cyc_List_List*_tmp126=Cyc_Relations_exp2relns(r,_tmpD9);((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp127,_tmp126,Cyc_Relations_exp2relns(r,_tmpD8));});case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3)->f2 != 0){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3)->f2)->hd)->r)->tag == 3U){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3)->f2)->hd)->r)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3)->f2)->hd)->r)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3)->f2)->hd)->r)->f2)->tl)->tl == 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3)->f2)->tl == 0){_LL5: _tmpDC=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3)->f2)->hd)->r)->f1;_tmpDB=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3)->f2)->hd)->r)->f2)->hd;_tmpDA=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3)->f2)->hd)->r)->f2)->tl)->hd;_LL6:
# 586
{enum Cyc_Absyn_Primop _tmpC6=_tmpDC;switch(_tmpC6){case Cyc_Absyn_Eq: _LL11: _LL12:
 goto _LL14;case Cyc_Absyn_Neq: _LL13: _LL14: goto _LL16;case Cyc_Absyn_Lt: _LL15: _LL16: goto _LL18;case Cyc_Absyn_Lte: _LL17: _LL18: goto _LL1A;case Cyc_Absyn_Gt: _LL19: _LL1A: goto _LL1C;case Cyc_Absyn_Gte: _LL1B: _LL1C: {
struct _tuple10 _tmpC7=Cyc_Relations_primop2relation(_tmpDB,_tmpDC,_tmpDA);struct _tuple10 _tmpC8=_tmpC7;struct Cyc_Absyn_Exp*_tmpCD;enum Cyc_Relations_Relation _tmpCC;struct Cyc_Absyn_Exp*_tmpCB;_LL20: _tmpCD=_tmpC8.f1;_tmpCC=_tmpC8.f2;_tmpCB=_tmpC8.f3;_LL21:;{
union Cyc_Relations_RelnOp rop1=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp rop2=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(_tmpCD,& rop1) && Cyc_Relations_exp2relnop(_tmpCB,& rop2))
return({struct Cyc_List_List*_tmpC9=_region_malloc(r,sizeof(*_tmpC9));({struct Cyc_Relations_Reln*_tmp129=({struct Cyc_Relations_Reln*_tmpCA=_region_malloc(r,sizeof(*_tmpCA));_tmpCA->rop1=rop2;({enum Cyc_Relations_Relation _tmp128=Cyc_Relations_flip_relation(_tmpCC);_tmpCA->relation=_tmp128;});_tmpCA->rop2=rop1;_tmpCA;});_tmpC9->hd=_tmp129;});_tmpC9->tl=0;_tmpC9;});
goto _LL10;};}default: _LL1D: _LL1E:
 goto _LL10;}_LL10:;}
# 596
goto _LL0;}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3)->f2)->tl)->tl == 0)goto _LL7;else{goto _LL9;}}}else{if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3)->f2)->tl)->tl == 0)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3)->f2)->tl)->tl == 0)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3)->f2)->tl)->tl == 0)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3)->f2)->tl)->tl == 0)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3)->f2)->tl)->tl == 0){_LL7: _tmpDF=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3)->f1;_tmpDE=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3)->f2)->hd;_tmpDD=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3)->f2)->tl)->hd;_LL8:
# 598
{enum Cyc_Absyn_Primop _tmpCE=_tmpDF;switch(_tmpCE){case Cyc_Absyn_Eq: _LL23: _LL24:
 goto _LL26;case Cyc_Absyn_Neq: _LL25: _LL26: goto _LL28;case Cyc_Absyn_Lt: _LL27: _LL28: goto _LL2A;case Cyc_Absyn_Lte: _LL29: _LL2A: goto _LL2C;case Cyc_Absyn_Gt: _LL2B: _LL2C: goto _LL2E;case Cyc_Absyn_Gte: _LL2D: _LL2E: {
struct _tuple10 _tmpCF=Cyc_Relations_primop2relation(_tmpDE,_tmpDF,_tmpDD);struct _tuple10 _tmpD0=_tmpCF;struct Cyc_Absyn_Exp*_tmpD5;enum Cyc_Relations_Relation _tmpD4;struct Cyc_Absyn_Exp*_tmpD3;_LL32: _tmpD5=_tmpD0.f1;_tmpD4=_tmpD0.f2;_tmpD3=_tmpD0.f3;_LL33:;{
union Cyc_Relations_RelnOp rop1=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp rop2=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(_tmpD5,& rop1) && Cyc_Relations_exp2relnop(_tmpD3,& rop2))
return({struct Cyc_List_List*_tmpD1=_region_malloc(r,sizeof(*_tmpD1));({struct Cyc_Relations_Reln*_tmp12A=({struct Cyc_Relations_Reln*_tmpD2=_region_malloc(r,sizeof(*_tmpD2));_tmpD2->rop1=rop1;_tmpD2->relation=_tmpD4;_tmpD2->rop2=rop2;_tmpD2;});_tmpD1->hd=_tmp12A;});_tmpD1->tl=0;_tmpD1;});
goto _LL22;};}default: _LL2F: _LL30:
 goto _LL22;}_LL22:;}
# 608
goto _LL0;}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}}default: _LL9: _LLA:
 goto _LL0;}_LL0:;}
# 612
if(Cyc_Tcutil_is_const_exp(e)){
struct _tuple11 _tmpE0=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple11 _tmpE1=_tmpE0;unsigned int _tmpE8;int _tmpE7;_LL35: _tmpE8=_tmpE1.f1;_tmpE7=_tmpE1.f2;_LL36:;
if(_tmpE7){
if((int)_tmpE8)return 0;else{
# 617
return({struct Cyc_List_List*_tmpE2=_region_malloc(r,sizeof(*_tmpE2));({struct Cyc_Relations_Reln*_tmp12D=({struct Cyc_Relations_Reln*_tmpE3=_region_malloc(r,sizeof(*_tmpE3));({union Cyc_Relations_RelnOp _tmp12B=Cyc_Relations_RConst(0U);_tmpE3->rop1=_tmp12B;});_tmpE3->relation=Cyc_Relations_Rlt;({union Cyc_Relations_RelnOp _tmp12C=Cyc_Relations_RConst(0U);_tmpE3->rop2=_tmp12C;});_tmpE3;});_tmpE2->hd=_tmp12D;});_tmpE2->tl=0;_tmpE2;});}}
# 619
({struct Cyc_String_pa_PrintArg_struct _tmpE6;_tmpE6.tag=0U;({struct _dyneither_ptr _tmp12E=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e));_tmpE6.f1=_tmp12E;});({void*_tmpE4[1U]={& _tmpE6};({unsigned int _tmp130=e->loc;struct _dyneither_ptr _tmp12F=({const char*_tmpE5="unable to convert `%s' to static relation";_tag_dyneither(_tmpE5,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp130,_tmp12F,_tag_dyneither(_tmpE4,sizeof(void*),1U));});});});}
# 623
return({struct Cyc_List_List*_tmpE9=_region_malloc(r,sizeof(*_tmpE9));({struct Cyc_Relations_Reln*_tmp133=({struct Cyc_Relations_Reln*_tmpEA=_region_malloc(r,sizeof(*_tmpEA));({union Cyc_Relations_RelnOp _tmp131=Cyc_Relations_RConst(0U);_tmpEA->rop1=_tmp131;});_tmpEA->relation=Cyc_Relations_Rlt;({union Cyc_Relations_RelnOp _tmp132=Cyc_Relations_RConst(0U);_tmpEA->rop2=_tmp132;});_tmpEA;});_tmpE9->hd=_tmp133;});_tmpE9->tl=0;_tmpE9;});}
