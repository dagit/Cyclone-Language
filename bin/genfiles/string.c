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
 struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 310 "cycboot.h"
int toupper(int);struct Cyc_Core_Opt{void*v;};
# 97 "core.h"
struct _dyneither_ptr Cyc_Core_new_string(unsigned int);
# 101
struct _dyneither_ptr Cyc_Core_rnew_string(struct _RegionHandle*,unsigned int);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 301 "core.h"
struct _dyneither_ptr Cyc_Core_mkfat(void*arr,unsigned int s,unsigned int n);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
int Cyc_strncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);
int Cyc_zstrcmp(struct _dyneither_ptr,struct _dyneither_ptr);
int Cyc_zstrncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long n);
int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
int Cyc_strcasecmp(struct _dyneither_ptr,struct _dyneither_ptr);
int Cyc_strncasecmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);
# 60
struct _dyneither_ptr Cyc_strcat(struct _dyneither_ptr dest,struct _dyneither_ptr src);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
struct _dyneither_ptr Cyc_rstrconcat(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);
struct _dyneither_ptr Cyc_rstrconcat_l(struct _RegionHandle*,struct Cyc_List_List*);
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);
struct _dyneither_ptr Cyc_rstr_sepstr(struct _RegionHandle*,struct Cyc_List_List*,struct _dyneither_ptr);
# 71
struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,struct _dyneither_ptr src);
struct _dyneither_ptr Cyc_strncpy(struct _dyneither_ptr,struct _dyneither_ptr,unsigned long);
struct _dyneither_ptr Cyc_zstrncpy(struct _dyneither_ptr,struct _dyneither_ptr,unsigned long);
# 76
struct _dyneither_ptr Cyc_realloc(struct _dyneither_ptr,unsigned long);
struct _dyneither_ptr Cyc_rrealloc(struct _RegionHandle*,struct _dyneither_ptr s,unsigned long sz);
# 81
struct _dyneither_ptr Cyc__memcpy(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned long,unsigned int);
struct _dyneither_ptr Cyc__memmove(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned long,unsigned int);
int Cyc_memcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long n);
struct _dyneither_ptr Cyc_memchr(struct _dyneither_ptr s,char c,unsigned long n);
struct _dyneither_ptr Cyc_mmemchr(struct _dyneither_ptr s,char c,unsigned long n);
struct _dyneither_ptr Cyc_memset(struct _dyneither_ptr s,char c,unsigned long n);
void Cyc_bzero(struct _dyneither_ptr s,unsigned long n);
void Cyc__bcopy(struct _dyneither_ptr src,struct _dyneither_ptr dst,unsigned long n,unsigned int sz);
# 99 "string.h"
struct _dyneither_ptr Cyc_expand(struct _dyneither_ptr s,unsigned long sz);
struct _dyneither_ptr Cyc_rexpand(struct _RegionHandle*,struct _dyneither_ptr s,unsigned long sz);
struct _dyneither_ptr Cyc_realloc_str(struct _dyneither_ptr str,unsigned long sz);
struct _dyneither_ptr Cyc_rrealloc_str(struct _RegionHandle*r,struct _dyneither_ptr str,unsigned long sz);
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);
struct _dyneither_ptr Cyc_rstrdup(struct _RegionHandle*,struct _dyneither_ptr src);
# 108
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);
struct _dyneither_ptr Cyc_rsubstring(struct _RegionHandle*,struct _dyneither_ptr,int ofs,unsigned long n);
# 113
struct _dyneither_ptr Cyc_replace_suffix(struct _dyneither_ptr,struct _dyneither_ptr,struct _dyneither_ptr);
struct _dyneither_ptr Cyc_rreplace_suffix(struct _RegionHandle*r,struct _dyneither_ptr src,struct _dyneither_ptr curr_suffix,struct _dyneither_ptr new_suffix);
# 119
struct _dyneither_ptr Cyc_strchr(struct _dyneither_ptr s,char c);
struct _dyneither_ptr Cyc_mstrchr(struct _dyneither_ptr s,char c);
struct _dyneither_ptr Cyc_mstrrchr(struct _dyneither_ptr s,char c);
struct _dyneither_ptr Cyc_strrchr(struct _dyneither_ptr s,char c);
struct _dyneither_ptr Cyc_mstrstr(struct _dyneither_ptr haystack,struct _dyneither_ptr needle);
struct _dyneither_ptr Cyc_strstr(struct _dyneither_ptr haystack,struct _dyneither_ptr needle);
struct _dyneither_ptr Cyc_strpbrk(struct _dyneither_ptr s,struct _dyneither_ptr accept);
struct _dyneither_ptr Cyc_mstrpbrk(struct _dyneither_ptr s,struct _dyneither_ptr accept);
unsigned long Cyc_strspn(struct _dyneither_ptr s,struct _dyneither_ptr accept);
unsigned long Cyc_strcspn(struct _dyneither_ptr s,struct _dyneither_ptr accept);
struct _dyneither_ptr Cyc_strtok(struct _dyneither_ptr s,struct _dyneither_ptr delim);
# 133
struct Cyc_List_List*Cyc_explode(struct _dyneither_ptr s);
struct Cyc_List_List*Cyc_rexplode(struct _RegionHandle*,struct _dyneither_ptr s);
struct _dyneither_ptr Cyc_implode(struct Cyc_List_List*c);
# 25 "strings.h"
int Cyc_strcasecmp(struct _dyneither_ptr,struct _dyneither_ptr);
int Cyc_strncasecmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);
# 29 "assert.h"
void*Cyc___assert_fail(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line);
# 39 "string.cyc"
unsigned long Cyc_strlen(struct _dyneither_ptr s){
unsigned long i;
unsigned int sz=_get_dyneither_size(s,sizeof(char));
for(i=0U;i < sz;++ i){
if(((const char*)s.curr)[(int)i]== '\000')
return i;}
# 46
return i;}
# 52
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2){
if((char*)s1.curr == (char*)s2.curr)
return 0;{
int i=0;
unsigned int sz1=_get_dyneither_size(s1,sizeof(char));
unsigned int sz2=_get_dyneither_size(s2,sizeof(char));
unsigned int minsz=sz1 < sz2?sz1: sz2;
minsz <= _get_dyneither_size(s1,sizeof(char)) && minsz <= _get_dyneither_size(s2,sizeof(char))?0:({struct _dyneither_ptr _tmp7F=({const char*_tmp0="minsz <= numelts(s1) && minsz <= numelts(s2)";_tag_dyneither(_tmp0,sizeof(char),45U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp7F,({const char*_tmp1="string.cyc";_tag_dyneither(_tmp1,sizeof(char),11U);}),59U);});
while(i < minsz){
char c1=((const char*)s1.curr)[i];
char c2=((const char*)s2.curr)[i];
if(c1 == '\000'){
if(c2 == '\000')return 0;else{
return - 1;}}else{
if(c2 == '\000')return 1;else{
# 68
int diff=c1 - c2;
if(diff != 0)return diff;}}
# 71
++ i;}
# 73
if(sz1 == sz2)return 0;
if(minsz < sz2){
if(*((const char*)_check_dyneither_subscript(s2,sizeof(char),i))== '\000')return 0;else{
return - 1;}}else{
# 78
if(*((const char*)_check_dyneither_subscript(s1,sizeof(char),i))== '\000')return 0;else{
return 1;}}};}
# 83
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2){
return Cyc_strcmp((struct _dyneither_ptr)*s1,(struct _dyneither_ptr)*s2);}
# 87
inline static int Cyc_ncmp(struct _dyneither_ptr s1,unsigned long len1,struct _dyneither_ptr s2,unsigned long len2,unsigned long n){
# 90
if(n <= 0)return 0;{
# 92
unsigned long min_len=len1 > len2?len2: len1;
unsigned long bound=min_len > n?n: min_len;
# 95
bound <= _get_dyneither_size(s1,sizeof(char)) && bound <= _get_dyneither_size(s2,sizeof(char))?0:({struct _dyneither_ptr _tmp80=({const char*_tmp2="bound <= numelts(s1) && bound <= numelts(s2)";_tag_dyneither(_tmp2,sizeof(char),45U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp80,({const char*_tmp3="string.cyc";_tag_dyneither(_tmp3,sizeof(char),11U);}),95U);});
# 97
{int i=0;for(0;i < bound;++ i){
int retc;
if((retc=((const char*)s1.curr)[i]- ((const char*)s2.curr)[i])!= 0)
return retc;}}
# 102
if(len1 < n  || len2 < n)
return(int)len1 - (int)len2;
return 0;};}
# 109
int Cyc_strncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long n){
unsigned long len1=Cyc_strlen(s1);
unsigned long len2=Cyc_strlen(s2);
return Cyc_ncmp(s1,len1,s2,len2,n);}
# 119
int Cyc_zstrcmp(struct _dyneither_ptr a,struct _dyneither_ptr b){
if((char*)a.curr == (char*)b.curr)
return 0;{
unsigned long as=_get_dyneither_size(a,sizeof(char));
unsigned long bs=_get_dyneither_size(b,sizeof(char));
unsigned long min_length=as < bs?as: bs;
int i=-1;
# 127
min_length <= _get_dyneither_size(a,sizeof(char)) && min_length <= _get_dyneither_size(b,sizeof(char))?0:({struct _dyneither_ptr _tmp81=({const char*_tmp4="min_length <= numelts(a) && min_length <= numelts(b)";_tag_dyneither(_tmp4,sizeof(char),53U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp81,({const char*_tmp5="string.cyc";_tag_dyneither(_tmp5,sizeof(char),11U);}),127U);});
# 129
while((++ i,i < min_length)){
int diff=(int)((const char*)a.curr)[i]- (int)((const char*)b.curr)[i];
if(diff != 0)
return diff;}
# 134
return(int)as - (int)bs;};}
# 137
int Cyc_zstrncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long n){
if(n <= 0)return 0;{
# 140
unsigned long s1size=_get_dyneither_size(s1,sizeof(char));
unsigned long s2size=_get_dyneither_size(s2,sizeof(char));
unsigned long min_size=s1size > s2size?s2size: s1size;
unsigned long bound=min_size > n?n: min_size;
# 145
bound <= _get_dyneither_size(s1,sizeof(char)) && bound <= _get_dyneither_size(s2,sizeof(char))?0:({struct _dyneither_ptr _tmp82=({const char*_tmp6="bound <= numelts(s1) && bound <= numelts(s2)";_tag_dyneither(_tmp6,sizeof(char),45U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp82,({const char*_tmp7="string.cyc";_tag_dyneither(_tmp7,sizeof(char),11U);}),145U);});
# 147
{int i=0;for(0;i < bound;++ i){
if(((const char*)s1.curr)[i]< ((const char*)s2.curr)[i])
return - 1;else{
if(((const char*)s2.curr)[i]< ((const char*)s1.curr)[i])
return 1;}}}
# 153
if(min_size <= bound)
return 0;
if(s1size < s2size)
return - 1;else{
# 158
return 1;}};}
# 162
int Cyc_zstrptrcmp(struct _dyneither_ptr*a,struct _dyneither_ptr*b){
if((int)a == (int)b)return 0;
return Cyc_zstrcmp((struct _dyneither_ptr)*a,(struct _dyneither_ptr)*b);}
# 172
inline static struct _dyneither_ptr Cyc_int_strcato(struct _dyneither_ptr dest,struct _dyneither_ptr src){
int i;
unsigned long dsize;unsigned long slen;unsigned long dlen;
# 176
dsize=_get_dyneither_size(dest,sizeof(char));
({unsigned long _tmp83=Cyc_strlen((struct _dyneither_ptr)dest);dlen=_tmp83;});
({unsigned long _tmp84=Cyc_strlen(src);slen=_tmp84;});
# 180
if(slen + dlen <= dsize){
slen <= _get_dyneither_size(src,sizeof(char))?0:({struct _dyneither_ptr _tmp85=({const char*_tmp8="slen <= numelts(src)";_tag_dyneither(_tmp8,sizeof(char),21U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp85,({const char*_tmp9="string.cyc";_tag_dyneither(_tmp9,sizeof(char),11U);}),181U);});
for(i=0;i < slen;++ i){
({struct _dyneither_ptr _tmpA=_dyneither_ptr_plus(dest,sizeof(char),(int)(i + dlen));char _tmpB=*((char*)_check_dyneither_subscript(_tmpA,sizeof(char),0U));char _tmpC=((const char*)src.curr)[i];if(_get_dyneither_size(_tmpA,sizeof(char))== 1U  && (_tmpB == '\000'  && _tmpC != '\000'))_throw_arraybounds();*((char*)_tmpA.curr)=_tmpC;});}
# 185
if(i != dsize)
({struct _dyneither_ptr _tmpD=_dyneither_ptr_plus(dest,sizeof(char),(int)(i + dlen));char _tmpE=*((char*)_check_dyneither_subscript(_tmpD,sizeof(char),0U));char _tmpF='\000';if(_get_dyneither_size(_tmpD,sizeof(char))== 1U  && (_tmpE == '\000'  && _tmpF != '\000'))_throw_arraybounds();*((char*)_tmpD.curr)=_tmpF;});}else{
# 189
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmp86=({const char*_tmp10="strcat";_tag_dyneither(_tmp10,sizeof(char),7U);});_tmp11->f1=_tmp86;});_tmp11;}));}
return dest;}
# 195
struct _dyneither_ptr Cyc_strcat(struct _dyneither_ptr dest,struct _dyneither_ptr src){
return Cyc_int_strcato(dest,src);}
# 200
struct _dyneither_ptr Cyc_rstrconcat(struct _RegionHandle*r,struct _dyneither_ptr a,struct _dyneither_ptr b){
unsigned long _tmp12=Cyc_strlen(a);
unsigned long _tmp13=Cyc_strlen(b);
struct _dyneither_ptr ans=Cyc_Core_rnew_string(r,(_tmp12 + _tmp13)+ 1);
int i;int j;
_tmp12 <= _get_dyneither_size(ans,sizeof(char)) && _tmp12 <= _get_dyneither_size(a,sizeof(char))?0:({struct _dyneither_ptr _tmp87=({const char*_tmp14="alen <= numelts(ans) && alen <= numelts(a)";_tag_dyneither(_tmp14,sizeof(char),43U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp87,({const char*_tmp15="string.cyc";_tag_dyneither(_tmp15,sizeof(char),11U);}),205U);});
for(i=0;i < _tmp12;++ i){({struct _dyneither_ptr _tmp16=_dyneither_ptr_plus(ans,sizeof(char),i);char _tmp17=*((char*)_check_dyneither_subscript(_tmp16,sizeof(char),0U));char _tmp18=((const char*)a.curr)[i];if(_get_dyneither_size(_tmp16,sizeof(char))== 1U  && (_tmp17 == '\000'  && _tmp18 != '\000'))_throw_arraybounds();*((char*)_tmp16.curr)=_tmp18;});}
_tmp13 <= _get_dyneither_size(b,sizeof(char))?0:({struct _dyneither_ptr _tmp88=({const char*_tmp19="blen <= numelts(b)";_tag_dyneither(_tmp19,sizeof(char),19U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp88,({const char*_tmp1A="string.cyc";_tag_dyneither(_tmp1A,sizeof(char),11U);}),207U);});
for(j=0;j < _tmp13;++ j){({struct _dyneither_ptr _tmp1B=_dyneither_ptr_plus(ans,sizeof(char),i + j);char _tmp1C=*((char*)_check_dyneither_subscript(_tmp1B,sizeof(char),0U));char _tmp1D=((const char*)b.curr)[j];if(_get_dyneither_size(_tmp1B,sizeof(char))== 1U  && (_tmp1C == '\000'  && _tmp1D != '\000'))_throw_arraybounds();*((char*)_tmp1B.curr)=_tmp1D;});}
return ans;}
# 212
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr a,struct _dyneither_ptr b){
return Cyc_rstrconcat(Cyc_Core_heap_region,a,b);}
# 217
struct _dyneither_ptr Cyc_rstrconcat_l(struct _RegionHandle*r,struct Cyc_List_List*strs){
# 219
unsigned long len;
unsigned long total_len=0U;
struct _dyneither_ptr ans;
struct _RegionHandle _tmp1E=_new_region("temp");struct _RegionHandle*temp=& _tmp1E;_push_region(temp);{
struct Cyc_List_List*lens=({struct Cyc_List_List*_tmp21=_region_malloc(temp,sizeof(*_tmp21));
_tmp21->hd=(void*)0U,_tmp21->tl=0;_tmp21;});
# 226
struct Cyc_List_List*end=lens;
{struct Cyc_List_List*p=strs;for(0;p != 0;p=p->tl){
({unsigned long _tmp89=Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)p->hd));len=_tmp89;});
total_len +=len;
({struct Cyc_List_List*_tmp8A=({struct Cyc_List_List*_tmp1F=_region_malloc(temp,sizeof(*_tmp1F));_tmp1F->hd=(void*)len,_tmp1F->tl=0;_tmp1F;});((struct Cyc_List_List*)_check_null(end))->tl=_tmp8A;});
end=end->tl;}}
# 233
lens=lens->tl;
({struct _dyneither_ptr _tmp8B=Cyc_Core_rnew_string(r,total_len + 1);ans=_tmp8B;});{
unsigned long i=0U;
while(strs != 0){
struct _dyneither_ptr _tmp20=*((struct _dyneither_ptr*)strs->hd);
len=(unsigned long)((struct Cyc_List_List*)_check_null(lens))->hd;
({struct _dyneither_ptr _tmp8D=_dyneither_ptr_decrease_size(_dyneither_ptr_plus(ans,sizeof(char),(int)i),sizeof(char),1U);struct _dyneither_ptr _tmp8C=(struct _dyneither_ptr)_tmp20;Cyc_strncpy(_tmp8D,_tmp8C,len);});
i +=len;
strs=strs->tl;
lens=lens->tl;}};}{
# 245
struct _dyneither_ptr _tmp22=ans;_npop_handler(0U);return _tmp22;};
# 222
;_pop_region(temp);}
# 248
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*strs){
return Cyc_rstrconcat_l(Cyc_Core_heap_region,strs);}
# 254
struct _dyneither_ptr Cyc_rstr_sepstr(struct _RegionHandle*r,struct Cyc_List_List*strs,struct _dyneither_ptr separator){
if(strs == 0)return Cyc_Core_rnew_string(r,0U);
if(strs->tl == 0)return Cyc_rstrdup(r,(struct _dyneither_ptr)*((struct _dyneither_ptr*)strs->hd));{
struct Cyc_List_List*_tmp23=strs;
struct _RegionHandle _tmp24=_new_region("temp");struct _RegionHandle*temp=& _tmp24;_push_region(temp);
{struct Cyc_List_List*lens=({struct Cyc_List_List*_tmp28=_region_malloc(temp,sizeof(*_tmp28));_tmp28->hd=(void*)0U,_tmp28->tl=0;_tmp28;});
struct Cyc_List_List*end=lens;
unsigned long len;
unsigned long total_len=0U;
unsigned long list_len=0U;
for(0;_tmp23 != 0;_tmp23=_tmp23->tl){
({unsigned long _tmp8E=Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp23->hd));len=_tmp8E;});
total_len +=len;
({struct Cyc_List_List*_tmp8F=({struct Cyc_List_List*_tmp25=_region_malloc(temp,sizeof(*_tmp25));_tmp25->hd=(void*)len,_tmp25->tl=0;_tmp25;});((struct Cyc_List_List*)_check_null(end))->tl=_tmp8F;});
end=end->tl;
++ list_len;}
# 271
lens=lens->tl;{
unsigned long seplen=Cyc_strlen(separator);
total_len +=(list_len - 1)* seplen;{
struct _dyneither_ptr ans=Cyc_Core_rnew_string(r,total_len + 1);
unsigned long i=0U;
while(((struct Cyc_List_List*)_check_null(strs))->tl != 0){
struct _dyneither_ptr _tmp26=*((struct _dyneither_ptr*)strs->hd);
len=(unsigned long)((struct Cyc_List_List*)_check_null(lens))->hd;
({struct _dyneither_ptr _tmp91=_dyneither_ptr_decrease_size(_dyneither_ptr_plus(ans,sizeof(char),(int)i),sizeof(char),1U);struct _dyneither_ptr _tmp90=(struct _dyneither_ptr)_tmp26;Cyc_strncpy(_tmp91,_tmp90,len);});
i +=len;
({struct _dyneither_ptr _tmp93=_dyneither_ptr_decrease_size(_dyneither_ptr_plus(ans,sizeof(char),(int)i),sizeof(char),1U);struct _dyneither_ptr _tmp92=separator;Cyc_strncpy(_tmp93,_tmp92,seplen);});
i +=seplen;
strs=strs->tl;
lens=lens->tl;}
# 286
({struct _dyneither_ptr _tmp95=_dyneither_ptr_decrease_size(_dyneither_ptr_plus(ans,sizeof(char),(int)i),sizeof(char),1U);struct _dyneither_ptr _tmp94=(struct _dyneither_ptr)*((struct _dyneither_ptr*)strs->hd);Cyc_strncpy(_tmp95,_tmp94,(unsigned long)((struct Cyc_List_List*)_check_null(lens))->hd);});{
struct _dyneither_ptr _tmp27=ans;_npop_handler(0U);return _tmp27;};};};}
# 259
;_pop_region(temp);};}
# 291
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*strs,struct _dyneither_ptr separator){
return Cyc_rstr_sepstr(Cyc_Core_heap_region,strs,separator);}
# 296
struct _dyneither_ptr Cyc_strncpy(struct _dyneither_ptr dest,struct _dyneither_ptr src,unsigned long n){
int i;
n <= _get_dyneither_size(src,sizeof(char)) && n <= _get_dyneither_size(dest,sizeof(char))?0:({struct _dyneither_ptr _tmp96=({const char*_tmp29="n <= numelts(src) && n <= numelts(dest)";_tag_dyneither(_tmp29,sizeof(char),40U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp96,({const char*_tmp2A="string.cyc";_tag_dyneither(_tmp2A,sizeof(char),11U);}),298U);});
for(i=0;i < n;++ i){
char _tmp2B=((const char*)src.curr)[i];
if(_tmp2B == '\000')break;
((char*)dest.curr)[i]=_tmp2B;}
# 304
for(0;i < n;++ i){
((char*)dest.curr)[i]='\000';}
# 307
return dest;}
# 311
struct _dyneither_ptr Cyc_zstrncpy(struct _dyneither_ptr dest,struct _dyneither_ptr src,unsigned long n){
n <= _get_dyneither_size(dest,sizeof(char)) && n <= _get_dyneither_size(src,sizeof(char))?0:({struct _dyneither_ptr _tmp97=({const char*_tmp2C="n <= numelts(dest) && n <= numelts(src)";_tag_dyneither(_tmp2C,sizeof(char),40U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp97,({const char*_tmp2D="string.cyc";_tag_dyneither(_tmp2D,sizeof(char),11U);}),312U);});{
int i;
for(i=0;i < n;++ i){
((char*)dest.curr)[i]=((const char*)src.curr)[i];}
return dest;};}
# 319
struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,struct _dyneither_ptr src){
unsigned int ssz=_get_dyneither_size(src,sizeof(char));
unsigned int dsz=_get_dyneither_size(dest,sizeof(char));
if(ssz <= dsz){
unsigned int i;
for(i=0U;i < ssz;++ i){
char _tmp2E=((const char*)src.curr)[(int)i];
({struct _dyneither_ptr _tmp2F=_dyneither_ptr_plus(dest,sizeof(char),(int)i);char _tmp30=*((char*)_check_dyneither_subscript(_tmp2F,sizeof(char),0U));char _tmp31=_tmp2E;if(_get_dyneither_size(_tmp2F,sizeof(char))== 1U  && (_tmp30 == '\000'  && _tmp31 != '\000'))_throw_arraybounds();*((char*)_tmp2F.curr)=_tmp31;});
if(_tmp2E == '\000')break;}}else{
# 331
unsigned long len=Cyc_strlen(src);
({struct _dyneither_ptr _tmp99=_dyneither_ptr_decrease_size(dest,sizeof(char),1U);struct _dyneither_ptr _tmp98=src;Cyc_strncpy(_tmp99,_tmp98,len);});
if(len < _get_dyneither_size(dest,sizeof(char)))
({struct _dyneither_ptr _tmp32=_dyneither_ptr_plus(dest,sizeof(char),(int)len);char _tmp33=*((char*)_check_dyneither_subscript(_tmp32,sizeof(char),0U));char _tmp34='\000';if(_get_dyneither_size(_tmp32,sizeof(char))== 1U  && (_tmp33 == '\000'  && _tmp34 != '\000'))_throw_arraybounds();*((char*)_tmp32.curr)=_tmp34;});}
# 336
return dest;}
# 342
struct _dyneither_ptr Cyc_rstrdup(struct _RegionHandle*r,struct _dyneither_ptr src){
unsigned long len;
struct _dyneither_ptr temp;
# 346
({unsigned long _tmp9A=Cyc_strlen(src);len=_tmp9A;});
({struct _dyneither_ptr _tmp9B=Cyc_Core_rnew_string(r,len + 1);temp=_tmp9B;});
{struct _dyneither_ptr _tmp35=_dyneither_ptr_decrease_size(temp,sizeof(char),1U);
struct _dyneither_ptr _tmp36=_tmp35;struct _dyneither_ptr _tmp37;_LL1: _tmp37=_tmp36;_LL2:;
Cyc_strncpy(_tmp37,src,len);}
# 352
return temp;}
# 355
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src){
return Cyc_rstrdup(Cyc_Core_heap_region,src);}
# 359
struct _dyneither_ptr Cyc_rrealloc(struct _RegionHandle*r,struct _dyneither_ptr s,unsigned long sz){
struct _dyneither_ptr temp;
unsigned long slen;
# 363
slen=_get_dyneither_size(s,sizeof(char));
sz=sz > slen?sz: slen;
({struct _dyneither_ptr _tmp9C=({unsigned int _tmp38=sz;char*_tmp39=_region_calloc(r,sizeof(char),_tmp38);_tag_dyneither(_tmp39,sizeof(char),_tmp38);});temp=_tmp9C;});
# 367
{struct _dyneither_ptr _tmp3A=temp;struct _dyneither_ptr _tmp3B;_LL1: _tmp3B=_tmp3A;_LL2:;
Cyc_strncpy((struct _dyneither_ptr)_tmp3B,(struct _dyneither_ptr)s,slen);}
# 371
return temp;}
# 374
struct _dyneither_ptr Cyc_rexpand(struct _RegionHandle*r,struct _dyneither_ptr s,unsigned long sz){
struct _dyneither_ptr temp;
unsigned long slen;
# 378
({unsigned long _tmp9D=Cyc_strlen(s);slen=_tmp9D;});
sz=sz > slen?sz: slen;
({struct _dyneither_ptr _tmp9E=Cyc_Core_rnew_string(r,sz);temp=_tmp9E;});
# 382
{struct _dyneither_ptr _tmp3C=temp;struct _dyneither_ptr _tmp3D;_LL1: _tmp3D=_tmp3C;_LL2:;
Cyc_strncpy((struct _dyneither_ptr)_tmp3D,s,slen);}
# 386
if(slen != _get_dyneither_size(s,sizeof(char)))
({struct _dyneither_ptr _tmp3E=_dyneither_ptr_plus(temp,sizeof(char),(int)slen);char _tmp3F=*((char*)_check_dyneither_subscript(_tmp3E,sizeof(char),0U));char _tmp40='\000';if(_get_dyneither_size(_tmp3E,sizeof(char))== 1U  && (_tmp3F == '\000'  && _tmp40 != '\000'))_throw_arraybounds();*((char*)_tmp3E.curr)=_tmp40;});
# 389
return temp;}
# 392
struct _dyneither_ptr Cyc_expand(struct _dyneither_ptr s,unsigned long sz){
return Cyc_rexpand(Cyc_Core_heap_region,s,sz);}
# 396
struct _dyneither_ptr Cyc_rrealloc_str(struct _RegionHandle*r,struct _dyneither_ptr str,unsigned long sz){
# 398
unsigned long maxsizeP=_get_dyneither_size(str,sizeof(char));
# 400
if(maxsizeP == 0){
maxsizeP=30 > sz?30U: sz;
({struct _dyneither_ptr _tmp9F=Cyc_Core_rnew_string(r,maxsizeP);str=_tmp9F;});
({struct _dyneither_ptr _tmp41=_dyneither_ptr_plus(str,sizeof(char),0);char _tmp42=*((char*)_check_dyneither_subscript(_tmp41,sizeof(char),0U));char _tmp43='\000';if(_get_dyneither_size(_tmp41,sizeof(char))== 1U  && (_tmp42 == '\000'  && _tmp43 != '\000'))_throw_arraybounds();*((char*)_tmp41.curr)=_tmp43;});}else{
# 405
if(sz > maxsizeP){
maxsizeP=maxsizeP * 2 > (sz * 5)/ 4?maxsizeP * 2:(sz * 5)/ 4;{
struct _dyneither_ptr _tmp44=str;struct _dyneither_ptr _tmp45;_LL1: _tmp45=_tmp44;_LL2:;
({struct _dyneither_ptr _tmpA0=_dyneither_ptr_decrease_size(Cyc_rexpand(r,(struct _dyneither_ptr)_tmp45,maxsizeP),sizeof(char),1U);_tmp45=_tmpA0;});};}}
# 411
return str;}
# 414
struct _dyneither_ptr Cyc_realloc_str(struct _dyneither_ptr str,unsigned long sz){
return Cyc_rrealloc_str(Cyc_Core_heap_region,str,sz);}
# 423
struct _dyneither_ptr Cyc_rsubstring(struct _RegionHandle*r,struct _dyneither_ptr s,int start,unsigned long amt){
# 427
struct _dyneither_ptr ans=Cyc_Core_rnew_string(r,amt + 1);
({struct _dyneither_ptr _tmpA1=_dyneither_ptr_plus(s,sizeof(char),start);s=_tmpA1;});
amt < _get_dyneither_size(ans,sizeof(char)) && amt <= _get_dyneither_size(s,sizeof(char))?0:({struct _dyneither_ptr _tmpA2=({const char*_tmp46="amt < numelts(ans) && amt <= numelts(s)";_tag_dyneither(_tmp46,sizeof(char),40U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpA2,({const char*_tmp47="string.cyc";_tag_dyneither(_tmp47,sizeof(char),11U);}),429U);});
{unsigned long i=0U;for(0;i < amt;++ i){
({struct _dyneither_ptr _tmp48=_dyneither_ptr_plus(ans,sizeof(char),(int)i);char _tmp49=*((char*)_check_dyneither_subscript(_tmp48,sizeof(char),0U));char _tmp4A=((const char*)s.curr)[(int)i];if(_get_dyneither_size(_tmp48,sizeof(char))== 1U  && (_tmp49 == '\000'  && _tmp4A != '\000'))_throw_arraybounds();*((char*)_tmp48.curr)=_tmp4A;});}}
({struct _dyneither_ptr _tmp4B=_dyneither_ptr_plus(ans,sizeof(char),(int)amt);char _tmp4C=*((char*)_check_dyneither_subscript(_tmp4B,sizeof(char),0U));char _tmp4D='\000';if(_get_dyneither_size(_tmp4B,sizeof(char))== 1U  && (_tmp4C == '\000'  && _tmp4D != '\000'))_throw_arraybounds();*((char*)_tmp4B.curr)=_tmp4D;});
return ans;}
# 436
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr s,int start,unsigned long amt){
return Cyc_rsubstring(Cyc_Core_heap_region,s,start,amt);}
# 442
struct _dyneither_ptr Cyc_rreplace_suffix(struct _RegionHandle*r,struct _dyneither_ptr src,struct _dyneither_ptr curr_suffix,struct _dyneither_ptr new_suffix){
unsigned long m=_get_dyneither_size(src,sizeof(char));
unsigned long n=_get_dyneither_size(curr_suffix,sizeof(char));
struct _dyneither_ptr err=({const char*_tmp50="replace_suffix";_tag_dyneither(_tmp50,sizeof(char),15U);});
if(m < n)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->tag=Cyc_Core_Invalid_argument,_tmp4E->f1=err;_tmp4E;}));
{unsigned long i=1U;for(0;i <= n;++ i){
if(({char _tmpA3=*((const char*)_check_dyneither_subscript(src,sizeof(char),(int)(m - i)));_tmpA3 != *((const char*)_check_dyneither_subscript(curr_suffix,sizeof(char),(int)(n - i)));}))
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->tag=Cyc_Core_Invalid_argument,_tmp4F->f1=err;_tmp4F;}));}}{
struct _dyneither_ptr ans=Cyc_Core_rnew_string(r,((m - n)+ _get_dyneither_size(new_suffix,sizeof(char)))+ 1);
({struct _dyneither_ptr _tmpA5=_dyneither_ptr_decrease_size(ans,sizeof(char),1U);struct _dyneither_ptr _tmpA4=src;Cyc_strncpy(_tmpA5,_tmpA4,m - n);});
({struct _dyneither_ptr _tmpA7=_dyneither_ptr_decrease_size(_dyneither_ptr_plus(ans,sizeof(char),(int)(m - n)),sizeof(char),1U);struct _dyneither_ptr _tmpA6=new_suffix;Cyc_strncpy(_tmpA7,_tmpA6,_get_dyneither_size(new_suffix,sizeof(char)));});
return ans;};}
# 457
struct _dyneither_ptr Cyc_replace_suffix(struct _dyneither_ptr src,struct _dyneither_ptr curr_suffix,struct _dyneither_ptr new_suffix){
return Cyc_rreplace_suffix(Cyc_Core_heap_region,src,curr_suffix,new_suffix);}
# 464
struct _dyneither_ptr Cyc_strpbrk(struct _dyneither_ptr s,struct _dyneither_ptr accept){
int len=(int)_get_dyneither_size(s,sizeof(char));
unsigned int asize=_get_dyneither_size(accept,sizeof(char));
char c;
unsigned int i;
for(i=0U;i < len  && (c=((const char*)s.curr)[(int)i])!= 0;++ i){
unsigned int j=0U;for(0;j < asize;++ j){
if(c == ((const char*)accept.curr)[(int)j])
return _dyneither_ptr_plus(s,sizeof(char),(int)i);}}
# 474
return _tag_dyneither(0,0,0);}
# 477
struct _dyneither_ptr Cyc_mstrpbrk(struct _dyneither_ptr s,struct _dyneither_ptr accept){
int len=(int)_get_dyneither_size(s,sizeof(char));
unsigned int asize=_get_dyneither_size(accept,sizeof(char));
char c;
unsigned int i;
for(i=0U;i < len  && (c=((char*)s.curr)[(int)i])!= 0;++ i){
unsigned int j=0U;for(0;j < asize;++ j){
if(c == ((const char*)accept.curr)[(int)j])
return _dyneither_ptr_plus(s,sizeof(char),(int)i);}}
# 487
return _tag_dyneither(0,0,0);}
# 492
struct _dyneither_ptr Cyc_mstrchr(struct _dyneither_ptr s,char c){
int len=(int)_get_dyneither_size(s,sizeof(char));
char c2;
unsigned int i;
# 497
for(i=0U;i < len  && (c2=((char*)s.curr)[(int)i])!= 0;++ i){
if(c2 == c)return _dyneither_ptr_plus(s,sizeof(char),(int)i);}
# 500
return _tag_dyneither(0,0,0);}
# 503
struct _dyneither_ptr Cyc_strchr(struct _dyneither_ptr s,char c){
int len=(int)_get_dyneither_size(s,sizeof(char));
char c2;
unsigned int i;
# 508
for(i=0U;i < len  && (c2=((const char*)s.curr)[(int)i])!= 0;++ i){
if(c2 == c)return _dyneither_ptr_plus(s,sizeof(char),(int)i);}
# 511
return _tag_dyneither(0,0,0);}
# 516
struct _dyneither_ptr Cyc_strrchr(struct _dyneither_ptr s0,char c){
int len=(int)Cyc_strlen((struct _dyneither_ptr)s0);
int i=len - 1;
struct _dyneither_ptr s=s0;
_dyneither_ptr_inplace_plus(& s,sizeof(char),i);
# 522
for(0;i >= 0;(i --,_dyneither_ptr_inplace_plus_post(& s,sizeof(char),-1))){
if(*((const char*)_check_dyneither_subscript(s,sizeof(char),0U))== c)
return(struct _dyneither_ptr)s;}
# 526
return _tag_dyneither(0,0,0);}
# 529
struct _dyneither_ptr Cyc_mstrrchr(struct _dyneither_ptr s0,char c){
int len=(int)Cyc_strlen((struct _dyneither_ptr)s0);
int i=len - 1;
struct _dyneither_ptr s=s0;
_dyneither_ptr_inplace_plus(& s,sizeof(char),i);
# 535
for(0;i >= 0;(i --,_dyneither_ptr_inplace_plus_post(& s,sizeof(char),-1))){
if(*((char*)_check_dyneither_subscript(s,sizeof(char),0U))== c)
return(struct _dyneither_ptr)s;}
# 539
return _tag_dyneither(0,0,0);}
# 544
struct _dyneither_ptr Cyc_strstr(struct _dyneither_ptr haystack,struct _dyneither_ptr needle){
if(!((unsigned int)haystack.curr) || !((unsigned int)needle.curr))(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmpA8=({const char*_tmp51="strstr";_tag_dyneither(_tmp51,sizeof(char),7U);});_tmp52->f1=_tmpA8;});_tmp52;}));
if(*((const char*)_check_dyneither_subscript(needle,sizeof(char),0U))== '\000')return haystack;{
# 548
int len=(int)Cyc_strlen((struct _dyneither_ptr)needle);
{struct _dyneither_ptr start=haystack;for(0;({
char*_tmpAA=(char*)({struct _dyneither_ptr _tmpA9=Cyc_strchr(start,*((const char*)_check_dyneither_subscript(needle,sizeof(char),0U)));start=_tmpA9;}).curr;_tmpAA != (char*)(_tag_dyneither(0,0,0)).curr;});({
struct _dyneither_ptr _tmpAC=({struct _dyneither_ptr _tmpAB=_dyneither_ptr_plus(start,sizeof(char),1);Cyc_strchr(_tmpAB,*((const char*)_check_dyneither_subscript(needle,sizeof(char),0U)));});start=_tmpAC;})){
if(Cyc_strncmp((struct _dyneither_ptr)start,(struct _dyneither_ptr)needle,(unsigned long)len)== 0)
return start;}}
# 555
return _tag_dyneither(0,0,0);};}
# 558
struct _dyneither_ptr Cyc_mstrstr(struct _dyneither_ptr haystack,struct _dyneither_ptr needle){
if(!((unsigned int)haystack.curr) || !((unsigned int)needle.curr))(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmpAD=({const char*_tmp53="mstrstr";_tag_dyneither(_tmp53,sizeof(char),8U);});_tmp54->f1=_tmpAD;});_tmp54;}));
if(*((const char*)_check_dyneither_subscript(needle,sizeof(char),0U))== '\000')return haystack;{
# 562
int len=(int)Cyc_strlen((struct _dyneither_ptr)needle);
{struct _dyneither_ptr start=haystack;for(0;({
char*_tmpAF=(char*)({struct _dyneither_ptr _tmpAE=Cyc_mstrchr(start,*((const char*)_check_dyneither_subscript(needle,sizeof(char),0U)));start=_tmpAE;}).curr;_tmpAF != (char*)(_tag_dyneither(0,0,0)).curr;});({
struct _dyneither_ptr _tmpB1=({struct _dyneither_ptr _tmpB0=_dyneither_ptr_plus(start,sizeof(char),1);Cyc_mstrchr(_tmpB0,*((const char*)_check_dyneither_subscript(needle,sizeof(char),0U)));});start=_tmpB1;})){
if(Cyc_strncmp((struct _dyneither_ptr)start,(struct _dyneither_ptr)needle,(unsigned long)len)== 0)
return start;}}
# 569
return _tag_dyneither(0,0,0);};}
# 575
unsigned long Cyc_strspn(struct _dyneither_ptr s,struct _dyneither_ptr accept){
unsigned long len=Cyc_strlen((struct _dyneither_ptr)s);
unsigned int asize=_get_dyneither_size(accept,sizeof(char));
# 579
len <= _get_dyneither_size(s,sizeof(char))?0:({struct _dyneither_ptr _tmpB2=({const char*_tmp55="len <= numelts(s)";_tag_dyneither(_tmp55,sizeof(char),18U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpB2,({const char*_tmp56="string.cyc";_tag_dyneither(_tmp56,sizeof(char),11U);}),579U);});
{unsigned long i=0U;for(0;i < len;++ i){
int j;
for(j=0;j < asize;++ j){
if(((const char*)s.curr)[(int)i]== ((const char*)accept.curr)[j])
break;}
if(j == asize)
return i;}}
# 589
return len;}
# 595
unsigned long Cyc_strcspn(struct _dyneither_ptr s,struct _dyneither_ptr accept){
unsigned long len=Cyc_strlen((struct _dyneither_ptr)s);
unsigned int asize=_get_dyneither_size(accept,sizeof(char));
# 599
len <= _get_dyneither_size(s,sizeof(char))?0:({struct _dyneither_ptr _tmpB3=({const char*_tmp57="len <= numelts(s)";_tag_dyneither(_tmp57,sizeof(char),18U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpB3,({const char*_tmp58="string.cyc";_tag_dyneither(_tmp58,sizeof(char),11U);}),599U);});
{unsigned long i=0U;for(0;i < len;++ i){
int j;
for(j=0;j < asize;++ j){
if(((const char*)s.curr)[(int)i]!= ((const char*)accept.curr)[j])
break;}
if(j == asize)
return i;}}
# 609
return len;}
# 616
struct _dyneither_ptr Cyc_strtok(struct _dyneither_ptr s,struct _dyneither_ptr delim){
# 618
static struct _dyneither_ptr olds={(void*)0,(void*)0,(void*)(0 + 0)};
struct _dyneither_ptr token;
# 621
if(({char*_tmpB4=(char*)s.curr;_tmpB4 == (char*)(_tag_dyneither(0,0,0)).curr;})){
if(({char*_tmpB5=(char*)olds.curr;_tmpB5 == (char*)(_tag_dyneither(0,0,0)).curr;}))
return _tag_dyneither(0,0,0);
s=olds;}{
# 628
unsigned long inc=Cyc_strspn((struct _dyneither_ptr)s,delim);
if(inc >= _get_dyneither_size(s,sizeof(char)) || *((char*)_check_dyneither_subscript(_dyneither_ptr_plus(s,sizeof(char),(int)inc),sizeof(char),0U))== '\000'){
# 631
({struct _dyneither_ptr _tmpB6=_tag_dyneither(0,0,0);olds=_tmpB6;});
return _tag_dyneither(0,0,0);}else{
# 635
_dyneither_ptr_inplace_plus(& s,sizeof(char),(int)inc);}
# 638
token=s;
({struct _dyneither_ptr _tmpB7=Cyc_mstrpbrk(token,(struct _dyneither_ptr)delim);s=_tmpB7;});
if(({char*_tmpB8=(char*)s.curr;_tmpB8 == (char*)(_tag_dyneither(0,0,0)).curr;}))
# 642
({struct _dyneither_ptr _tmpB9=_tag_dyneither(0,0,0);olds=_tmpB9;});else{
# 646
({struct _dyneither_ptr _tmp59=s;char _tmp5A=*((char*)_check_dyneither_subscript(_tmp59,sizeof(char),0U));char _tmp5B='\000';if(_get_dyneither_size(_tmp59,sizeof(char))== 1U  && (_tmp5A == '\000'  && _tmp5B != '\000'))_throw_arraybounds();*((char*)_tmp59.curr)=_tmp5B;});
({struct _dyneither_ptr _tmpBA=_dyneither_ptr_plus(s,sizeof(char),1);olds=_tmpBA;});}
# 649
return token;};}
# 653
struct Cyc_List_List*Cyc_rexplode(struct _RegionHandle*r,struct _dyneither_ptr s){
struct Cyc_List_List*result=0;
{int i=(int)(Cyc_strlen(s)- 1);for(0;i >= 0;-- i){
({struct Cyc_List_List*_tmpBB=({struct Cyc_List_List*_tmp5C=_region_malloc(r,sizeof(*_tmp5C));_tmp5C->hd=(void*)((int)*((const char*)_check_dyneither_subscript(s,sizeof(char),i))),_tmp5C->tl=result;_tmp5C;});result=_tmpBB;});}}
return result;}
# 660
struct Cyc_List_List*Cyc_explode(struct _dyneither_ptr s){
return Cyc_rexplode(Cyc_Core_heap_region,s);}
# 664
struct _dyneither_ptr Cyc_implode(struct Cyc_List_List*chars){
struct _dyneither_ptr s=Cyc_Core_new_string((unsigned int)(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(chars)+ 1));
unsigned long i=0U;
while(chars != 0){
({struct _dyneither_ptr _tmp5D=_dyneither_ptr_plus(s,sizeof(char),(int)i ++);char _tmp5E=*((char*)_check_dyneither_subscript(_tmp5D,sizeof(char),0U));char _tmp5F=(char)((int)chars->hd);if(_get_dyneither_size(_tmp5D,sizeof(char))== 1U  && (_tmp5E == '\000'  && _tmp5F != '\000'))_throw_arraybounds();*((char*)_tmp5D.curr)=_tmp5F;});
chars=chars->tl;}
# 671
return s;}
# 675
inline static int Cyc_casecmp(struct _dyneither_ptr s1,unsigned long len1,struct _dyneither_ptr s2,unsigned long len2){
# 678
unsigned long min_length=len1 < len2?len1: len2;
# 680
min_length <= _get_dyneither_size(s1,sizeof(char)) && min_length <= _get_dyneither_size(s2,sizeof(char))?0:({struct _dyneither_ptr _tmpBC=({const char*_tmp60="min_length <= numelts(s1) && min_length <= numelts(s2)";_tag_dyneither(_tmp60,sizeof(char),55U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpBC,({const char*_tmp61="string.cyc";_tag_dyneither(_tmp61,sizeof(char),11U);}),680U);});{
# 682
int i=-1;
while((++ i,i < min_length)){
int diff=({int _tmpBD= toupper((int)((const char*)s1.curr)[i]);_tmpBD -  toupper((int)((const char*)s2.curr)[i]);});
if(diff != 0)
return diff;}
# 688
return(int)len1 - (int)len2;};}
# 691
int Cyc_strcasecmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2){
if((char*)s1.curr == (char*)s2.curr)
return 0;{
unsigned long len1=Cyc_strlen(s1);
unsigned long len2=Cyc_strlen(s2);
return Cyc_casecmp(s1,len1,s2,len2);};}
# 699
inline static int Cyc_caseless_ncmp(struct _dyneither_ptr s1,unsigned long len1,struct _dyneither_ptr s2,unsigned long len2,unsigned long n){
# 702
if(n <= 0)return 0;{
# 704
unsigned long min_len=len1 > len2?len2: len1;
unsigned long bound=min_len > n?n: min_len;
# 707
bound <= _get_dyneither_size(s1,sizeof(char)) && bound <= _get_dyneither_size(s2,sizeof(char))?0:({struct _dyneither_ptr _tmpBE=({const char*_tmp62="bound <= numelts(s1) && bound <= numelts(s2)";_tag_dyneither(_tmp62,sizeof(char),45U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpBE,({const char*_tmp63="string.cyc";_tag_dyneither(_tmp63,sizeof(char),11U);}),707U);});
# 709
{int i=0;for(0;i < bound;++ i){
int retc;
if(({int _tmpC0=({int _tmpBF= toupper((int)((const char*)s1.curr)[i]);_tmpBF -  toupper((int)((const char*)s2.curr)[i]);});retc=_tmpC0;})!= 0)
return retc;}}
# 714
if(len1 < n  || len2 < n)
return(int)len1 - (int)len2;
return 0;};}
# 720
int Cyc_strncasecmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long n){
unsigned long len1=Cyc_strlen(s1);
unsigned long len2=Cyc_strlen(s2);
return Cyc_caseless_ncmp(s1,len1,s2,len2,n);}
# 729
void* memcpy(void*,const void*,unsigned long n);
void* memmove(void*,const void*,unsigned long n);
int memcmp(const void*,const void*,unsigned long n);
char* memchr(const char*,char c,unsigned long n);
void* memset(void*,int c,unsigned long n);
void bcopy(const void*src,void*dest,unsigned long n);
void bzero(void*s,unsigned long n);
char* GC_realloc(char*,unsigned int n);
# 741
struct _dyneither_ptr Cyc_realloc(struct _dyneither_ptr s,unsigned long n){
unsigned long _tmp64=n;unsigned int _tmp66;_LL1: _tmp66=_tmp64;_LL2:;{
char*_tmp65= GC_realloc((char*)_untag_dyneither_ptr(s,sizeof(char),1U),_tmp66);
return((struct _dyneither_ptr(*)(char*arr,unsigned int s,unsigned int n))Cyc_Core_mkfat)((char*)_check_null(_tmp65),sizeof(char),_tmp66);};}
# 747
struct _dyneither_ptr Cyc__memcpy(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned long n,unsigned int sz){
if(((({void*_tmpC2=(void*)d.curr;_tmpC2 == (void*)(_tag_dyneither(0,0,0)).curr;}) || _get_dyneither_size(d,sizeof(void))< n) || ({void*_tmpC1=(void*)s.curr;_tmpC1 == (void*)(_tag_dyneither(0,0,0)).curr;})) || _get_dyneither_size(s,sizeof(void))< n)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmpC3=({const char*_tmp67="memcpy";_tag_dyneither(_tmp67,sizeof(char),7U);});_tmp68->f1=_tmpC3;});_tmp68;}));
({void*_tmpC5=(void*)_untag_dyneither_ptr(d,sizeof(void),1U);const void*_tmpC4=(const void*)_untag_dyneither_ptr(s,sizeof(void),1U); memcpy(_tmpC5,_tmpC4,n * sz);});
return d;}
# 754
struct _dyneither_ptr Cyc__memmove(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned long n,unsigned int sz){
if(((({void*_tmpC7=(void*)d.curr;_tmpC7 == (void*)(_tag_dyneither(0,0,0)).curr;}) || _get_dyneither_size(d,sizeof(void))< n) || ({void*_tmpC6=(void*)s.curr;_tmpC6 == (void*)(_tag_dyneither(0,0,0)).curr;})) || _get_dyneither_size(s,sizeof(void))< n)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmpC8=({const char*_tmp69="memove";_tag_dyneither(_tmp69,sizeof(char),7U);});_tmp6A->f1=_tmpC8;});_tmp6A;}));
({void*_tmpCA=(void*)_untag_dyneither_ptr(d,sizeof(void),1U);const void*_tmpC9=(const void*)_untag_dyneither_ptr(s,sizeof(void),1U); memmove(_tmpCA,_tmpC9,n * sz);});
return d;}
# 761
int Cyc_memcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long n){
if(((({char*_tmpCC=(char*)s1.curr;_tmpCC == (char*)(_tag_dyneither(0,0,0)).curr;}) || ({char*_tmpCB=(char*)s2.curr;_tmpCB == (char*)(_tag_dyneither(0,0,0)).curr;})) || _get_dyneither_size(s1,sizeof(char))< n) || _get_dyneither_size(s2,sizeof(char))< n)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmpCD=({const char*_tmp6B="memcmp";_tag_dyneither(_tmp6B,sizeof(char),7U);});_tmp6C->f1=_tmpCD;});_tmp6C;}));
return({const void*_tmpCF=(const void*)_untag_dyneither_ptr(s1,sizeof(char),1U);const void*_tmpCE=(const void*)_untag_dyneither_ptr(s2,sizeof(char),1U); memcmp(_tmpCF,_tmpCE,n);});}
# 767
struct _dyneither_ptr Cyc_memchr(struct _dyneither_ptr s,char c,unsigned long n){
unsigned int sz=_get_dyneither_size(s,sizeof(char));
if(({char*_tmpD0=(char*)s.curr;_tmpD0 == (char*)(_tag_dyneither(0,0,0)).curr;}) || n > sz)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmpD1=({const char*_tmp6D="memchr";_tag_dyneither(_tmp6D,sizeof(char),7U);});_tmp6E->f1=_tmpD1;});_tmp6E;}));{
char*_tmp6F= memchr((const char*)_untag_dyneither_ptr(s,sizeof(char),1U),c,n);
if(_tmp6F == 0)return _tag_dyneither(0,0,0);{
unsigned int _tmp70=(unsigned int)((const char*)_untag_dyneither_ptr(s,sizeof(char),1U));
unsigned int _tmp71=(unsigned int)((const char*)_tmp6F);
unsigned int _tmp72=_tmp71 - _tmp70;
return _dyneither_ptr_plus(s,sizeof(char),(int)_tmp72);};};}
# 779
struct _dyneither_ptr Cyc_mmemchr(struct _dyneither_ptr s,char c,unsigned long n){
unsigned int sz=_get_dyneither_size(s,sizeof(char));
if(({char*_tmpD2=(char*)s.curr;_tmpD2 == (char*)(_tag_dyneither(0,0,0)).curr;}) || n > sz)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmpD3=({const char*_tmp73="mmemchr";_tag_dyneither(_tmp73,sizeof(char),8U);});_tmp74->f1=_tmpD3;});_tmp74;}));{
char*_tmp75= memchr((const char*)_untag_dyneither_ptr(s,sizeof(char),1U),c,n);
if(_tmp75 == 0)return _tag_dyneither(0,0,0);{
unsigned int _tmp76=(unsigned int)((const char*)_untag_dyneither_ptr(s,sizeof(char),1U));
unsigned int _tmp77=(unsigned int)_tmp75;
unsigned int _tmp78=_tmp77 - _tmp76;
return _dyneither_ptr_plus(s,sizeof(char),(int)_tmp78);};};}
# 791
struct _dyneither_ptr Cyc_memset(struct _dyneither_ptr s,char c,unsigned long n){
if(({char*_tmpD4=(char*)s.curr;_tmpD4 == (char*)(_tag_dyneither(0,0,0)).curr;}) || n > _get_dyneither_size(s,sizeof(char)))
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmpD5=({const char*_tmp79="memset";_tag_dyneither(_tmp79,sizeof(char),7U);});_tmp7A->f1=_tmpD5;});_tmp7A;}));
 memset((void*)((char*)_untag_dyneither_ptr(s,sizeof(char),1U)),(int)c,n);
return s;}
# 798
void Cyc_bzero(struct _dyneither_ptr s,unsigned long n){
if(({char*_tmpD6=(char*)s.curr;_tmpD6 == (char*)(_tag_dyneither(0,0,0)).curr;}) || _get_dyneither_size(s,sizeof(char))< n)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmpD7=({const char*_tmp7B="bzero";_tag_dyneither(_tmp7B,sizeof(char),6U);});_tmp7C->f1=_tmpD7;});_tmp7C;}));
((void(*)(char*s,unsigned long n)) bzero)((char*)_untag_dyneither_ptr(s,sizeof(char),1U),n);}
# 804
void Cyc__bcopy(struct _dyneither_ptr src,struct _dyneither_ptr dst,unsigned long n,unsigned int sz){
if(((({void*_tmpD9=(void*)src.curr;_tmpD9 == (void*)(_tag_dyneither(0,0,0)).curr;}) || _get_dyneither_size(src,sizeof(void))< n) || ({void*_tmpD8=(void*)dst.curr;_tmpD8 == (void*)(_tag_dyneither(0,0,0)).curr;})) || _get_dyneither_size(dst,sizeof(void))< n)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmpDA=({const char*_tmp7D="bcopy";_tag_dyneither(_tmp7D,sizeof(char),6U);});_tmp7E->f1=_tmpDA;});_tmp7E;}));
({const void*_tmpDC=(const void*)_untag_dyneither_ptr(src,sizeof(void),1U);void*_tmpDB=(void*)_untag_dyneither_ptr(dst,sizeof(void),1U); bcopy(_tmpDC,_tmpDB,n * sz);});}
